/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/rmv/RMVFeatureTracker6DOF.h"
#include "ocean/tracking/rmv/RandomizedPose.h"
#include "ocean/tracking/rmv/RandomModelVariation.h"
#include "ocean/tracking/rmv/RMVFeatureDetector.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/ScopedValue.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameFilterGaussian.h"
#include "ocean/cv/FrameShrinker.h"

#include "ocean/geometry/NonLinearOptimizationPose.h"
#include "ocean/geometry/SpatialDistribution.h"

#include "ocean/tracking/PointCorrespondences.h"

// **TODO** general handling of camera distortion parameters -> currently we do not distort image points

namespace Ocean
{

namespace Tracking
{

namespace RMV
{

RMVFeatureTracker6DOF::RMVFeatureTracker6DOF(const RMVFeatureDetector::DetectorType detectorType) :
	trackerDetectorType_(detectorType),
	trackerFeatureDetectorStrength_(25),
	trackerMaximalPoseProjectionFeatureNumber_(150),
	trackerStrongCorrespondencesEmptyAreaRadius_(10.0),
	trackerSemiStrongCorrespondencesEmptyAreaRadius_(6.0),
	trackerAsynchronousDataProcessingActive_(false),
	trackerAsynchronousDataProcessingFlippedCamera_T_world_(false)
{
	startThread();
}

RMVFeatureTracker6DOF::~RMVFeatureTracker6DOF()
{
	stopThread();
	trackerAsynchronousSignal_.release();
	joinThread();

	stopThreadExplicitly();
}

void RMVFeatureTracker6DOF::setFeatureMap(const RMVFeatureMap& featureMap, RandomGenerator& randomGenerator, const bool autoUpdateMaxPositionOffset)
{
	const ScopedLock scopedLock(lock_);

	trackerFeatureMap_ = featureMap;

	const Box3& initializationBoundingBox = featureMap.initializationBoundingBox();
	const Scalar boundingBoxDiagonal = initializationBoundingBox.diagonal();

	if (autoUpdateMaxPositionOffset)
	{
		// updates the maximal position offset between two frames to a fifth of the feature map bounding box diagonal
		const Scalar diagonal_5 = boundingBoxDiagonal * Scalar(0.2);
		maxPositionOffset_ = Vector3(diagonal_5, diagonal_5, diagonal_5); // **TODO** use/check max positions between frames
	}

	const Vectors3& initializationObjectPoints = featureMap.initializationObjectPoints();

	trackerPoseProjectionSet_.setDimension(featureMap.initializationCamera().width(), featureMap.initializationCamera().height());

	if (initializationBoundingBox.isValid() && !initializationObjectPoints.empty() && trackerMaximalPoseProjectionFeatureNumber_ != 0)
	{
#if 1
		// we can either use random poses based on a hemisphere
		const HomogenousMatrices4 randomPoses = RandomizedPose::hemispherePoses(initializationBoundingBox, randomGenerator, boundingBoxDiagonal * Scalar(0.8), 12u, 12u);
#else
		// or we can use random poses with visibility constraints
		HomogenousMatrices4 randomPoses(12u * 12u + 12u);
		RandomizedPose::randomPoses(featureMap.initializationCamera(), initializationBoundingBox, randomGenerator, boundingBoxDiagonal * Scalar(0.7), boundingBoxDiagonal * Scalar(0.9), Scalar(0.25), randomPoses.size(), randomPoses.data(), WorkerPool::get().scopedWorker()());
#endif

		trackerPoseProjectionSet_.clear();

		for (const HomogenousMatrix4& randomPose : randomPoses)
		{
			trackerPoseProjectionSet_.addPoseProjection(Tracking::PoseProjection(randomPose, featureMap.initializationCamera(), initializationObjectPoints.data(), min(initializationObjectPoints.size(), trackerMaximalPoseProjectionFeatureNumber_), false));
		}
	}
}

bool RMVFeatureTracker6DOF::determinePoses(const Frames& frames, const SharedAnyCameras& anyCameras, TransformationSamples& transformations, const Quaternion& world_R_camera, Worker* worker)
{
	return VisualTracker::determinePoses(frames, anyCameras, transformations, world_R_camera, worker);
}

bool RMVFeatureTracker6DOF::determinePoses(const Frame& frame, const PinholeCamera& pinholeCamera, const bool frameIsUndistorted, TransformationSamples& transformations, const Quaternion& /*absoluteOrientation*/, Worker* worker)
{
	ocean_assert(frame.isValid() && pinholeCamera.isValid());
	ocean_assert(frame.width() == pinholeCamera.width() && frame.height() == pinholeCamera.height());

	if (!trackerFeatureMap_)
	{
		return false;
	}

	while (!asynchronousDataProcessed())
	{
		// this should never happen in a release build
		sleep(1u);
	}

	ocean_assert(transformations.empty());
	transformations.clear();

	HomogenousMatrix4 world_T_camera;
	if (internDeterminePose(frame, pinholeCamera, frameIsUndistorted, world_T_camera, worker))
	{
		motionModel_.update(world_T_camera);

		transformations.push_back(TransformationSample(world_T_camera, 0u));
		return true;
	}

	motionModel_.reset();
	return false;
}

bool RMVFeatureTracker6DOF::internDeterminePose(const Frame& frame, const PinholeCamera& pinholeCamera, const bool frameIsUndistorted, HomogenousMatrix4& world_T_camera, Worker* worker)
{
	ocean_assert(frame && pinholeCamera.isValid());
	ocean_assert(frame.width() == pinholeCamera.width() && frame.height() == pinholeCamera.height());

	Frame yFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yFrame, false, worker))
	{
		return false;
	}

	Vectors2 imagePoints;

	if (motionModel_)
	{
		// we have a rough pose e.g., from the previous frame so that we first try to determine the new pose with very efficient strategies

		const HomogenousMatrix4 world_T_roughCamera = motionModel_.predictedPose();

		// first, we determine strong feature points in the current frame

		ocean_assert(imagePoints.empty());
		imagePoints = detectFeatures(yFrame, frameIsUndistorted, pinholeCamera.projectToImage<true>(world_T_roughCamera, trackerFeatureMap_.boundingBox(), false), worker);

		// if we have a current frame with almost no feature points we simply stop here
		if (imagePoints.size() < 10)
		{
			return false;
		}

		if (trackerFeatureMap_.recentStrongObjectPointIndices().size() >= 5 && trackerFeatureMap_.recentStrongObjectPointIndices().size() + trackerFeatureMap_.recentSemiStrongObjectPointIndices().size() >= 20)
		{
			// in the case we have enough strong feature points from the previous frame we first try to determine the camera pose based on these feature points as this would be very simple and would provide a reliable tracking result

			if (determinePoseWithStrongPreviousCorrespondences(world_T_roughCamera, pinholeCamera, imagePoints, world_T_camera, worker))
			{
				return true;
			}
		}

		if (determinePoseWithAnyPreviousCorrespondences(world_T_roughCamera, pinholeCamera, imagePoints, world_T_camera, worker))
		{
			return true;
		}

		// none of the above applied tracking strategies based on known feature correspondences succeeded, so that we now try to determine the current camera pose based on the rough guess from the motion model

		if (determinePoseWithRoughPose(world_T_roughCamera, pinholeCamera, imagePoints, world_T_camera, worker))
		{
			return true;
		}
	}

	if (RMVFeatureDetector::needPyramidInitialization(trackerFeatureMap_.initializationDetectorType()))
	{
		if (!determinePoseWithoutKnowledgePyramid(yFrame, trackerFeatureMap_.initializationCamera(), world_T_camera, worker))
		{
			return false;
		}

		const HomogenousMatrix4 world_T_roughCamera = world_T_camera;

		imagePoints = detectFeatures(yFrame, frameIsUndistorted, pinholeCamera.projectToImage<true>(world_T_roughCamera, trackerFeatureMap_.boundingBox(), false), worker);

		if (!determinePoseWithRoughPose(world_T_roughCamera, pinholeCamera, imagePoints, world_T_camera, worker))
		{
			return false;
		}
	}
	else
	{
		if (imagePoints.empty())
		{
			imagePoints = detectFeatures(yFrame, frameIsUndistorted, Box2(), worker);
		}

		// if we have a current frame with almost no feature points we simply stop here
		if (imagePoints.size() < 20)
		{
			return false;
		}

		if (!determinePoseWithoutKnowledgeDefault(pinholeCamera, imagePoints, world_T_camera, worker))
		{
			return false;
		}
	}

	return true;
}

bool RMVFeatureTracker6DOF::determinePoseWithoutKnowledgePyramid(const Frame& frame, const PinholeCamera& pinholeCamera, HomogenousMatrix4& world_T_camera, Worker* worker)
{
	ocean_assert(frame.pixelFormat() == FrameType::FORMAT_Y8);
	ocean_assert(frame.pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT);

	Frame adjustedFrame(frame, Frame::ACM_USE_KEEP_LAYOUT);
	if (frame.width() != trackerPoseProjectionSet_.width() || frame.height() != trackerPoseProjectionSet_.height())
	{
		ocean_assert(frame.width() > trackerPoseProjectionSet_.width() && frame.height() > trackerPoseProjectionSet_.height());

		if (!CV::FrameShrinker::downsampleByTwo11(frame, adjustedFrame, worker))
		{
			return false;
		}

		while (adjustedFrame.width() != trackerPoseProjectionSet_.width() || adjustedFrame.height() != trackerPoseProjectionSet_.height())
		{
			ocean_assert(frame.width() > trackerPoseProjectionSet_.width() && frame.height() > trackerPoseProjectionSet_.height());

			if (!CV::FrameShrinker::downsampleByTwo11(adjustedFrame, worker))
			{
				return false;
			}
		}
	}

	ocean_assert(adjustedFrame.width() == pinholeCamera.width() && adjustedFrame.height() == pinholeCamera.height());

	if (RMVFeatureDetector::needSmoothedFrame(trackerDetectorType_))
	{
		if (!CV::FrameFilterGaussian::filter(adjustedFrame, 3u, worker))
		{
			return false;
		}
	}

	const Vectors3& objectPoints = trackerFeatureMap_.initializationObjectPoints();
	unsigned int numberImagePoints = (unsigned int)objectPoints.size() * 70u / 100u;

	const Vectors2 imagePoints(RMVFeatureDetector::detectFeatures(adjustedFrame, trackerDetectorType_, 55, numberImagePoints, false, worker));
	numberImagePoints = (unsigned int)imagePoints.size();

	if (imagePoints.size() < 20)
	{
		return false;
	}

	HomogenousMatrices4 roughFlippedCameras_T_world;

	{
		HomogenousMatrices4 world_T_roughCameras(8);
		trackerPoseProjectionSet_.findPosesWithMinimalError<Geometry::Estimator::ET_TUKEY>(imagePoints.data(), imagePoints.size(), (unsigned int)imagePoints.size() * 75u / 100u, Geometry::Error::ED_APPROXIMATED, world_T_roughCameras.size(), world_T_roughCameras.data(), nullptr, worker);

		// add default poses, **TODO** check for visibility of the default poses
		world_T_roughCameras.insert(world_T_roughCameras.begin(), HomogenousMatrix4(Vector3(Scalar(0.0), Scalar(0.25), Scalar(0.0)), Rotation(1, 0, 0, -Numeric::pi_2()) * Rotation(0, 0, 1, Numeric::pi_2())));
		world_T_roughCameras.insert(world_T_roughCameras.begin(), HomogenousMatrix4(Vector3(Scalar(0.0), Scalar(0.25), Scalar(0.0)), Rotation(1, 0, 0, -Numeric::pi_2()) * Rotation(0, 0, 1, Numeric::pi())));
		world_T_roughCameras.insert(world_T_roughCameras.begin(), HomogenousMatrix4(Vector3(Scalar(0.0), Scalar(0.25), Scalar(0.0)), Rotation(1, 0, 0, -Numeric::pi_2()) * Rotation(0, 0, 1, Numeric::pi() * Scalar(1.5))));
		world_T_roughCameras.insert(world_T_roughCameras.begin(), HomogenousMatrix4(Vector3(Scalar(0.0), Scalar(0.25), Scalar(0.0)), Rotation(1, 0, 0, -Numeric::pi_2())));

		roughFlippedCameras_T_world = PinholeCamera::standard2InvertedFlipped(world_T_roughCameras.data(), world_T_roughCameras.size());
	}

	const Scalar resolutionFactor = cameraResolutionFactor(pinholeCamera);

	const Scalar maxPixelError0 = Numeric::sqr(Scalar(16.0) * resolutionFactor);
	const Scalar maxPixelError1 = Numeric::sqr(Scalar(13.0) * resolutionFactor);
	const Scalar maxPixelError2 = Numeric::sqr(Scalar(9.0) * resolutionFactor);
	const Scalar maxPixelError3 = Numeric::sqr(Scalar(7.0) * resolutionFactor);


	HomogenousMatrix4 resultingFlippedCamera_T_world;
	if (RandomModelVariation::optimizedPoseFromPointCloudsWithSeveralInitialPosesIF<true>(roughFlippedCameras_T_world.data(), roughFlippedCameras_T_world.size(), pinholeCamera, objectPoints.data(), objectPoints.size(), imagePoints.data(), numberImagePoints, numberImagePoints * 65u / 100u, trackerRandomGenerator_, resultingFlippedCamera_T_world, Geometry::Error::ED_APPROXIMATED, maxPixelError0, Vector3(Scalar(0.3), Scalar(0.3), Scalar(0.3)), Numeric::deg2rad(30), Scalar(0.01), nullptr, nullptr, worker))
	{
		HomogenousMatrix4 initialFlippedCamera_T_world = resultingFlippedCamera_T_world;
		if (RandomModelVariation::optimizedPoseFromPointCloudsWithOneInitialPoseIF<true>(initialFlippedCamera_T_world, pinholeCamera, objectPoints.data(), objectPoints.size(), imagePoints.data(), numberImagePoints, numberImagePoints * 55u / 100u, trackerRandomGenerator_, resultingFlippedCamera_T_world, Geometry::Error::ED_APPROXIMATED, maxPixelError1, Vector3(Scalar(0.3), Scalar(0.3), Scalar(0.3)), Numeric::deg2rad(30), Scalar(0.01), nullptr, nullptr, nullptr, worker))
		{
			initialFlippedCamera_T_world = resultingFlippedCamera_T_world;

			if (RandomModelVariation::optimizedPoseFromPointCloudsWithOneInitialPoseIF<true>(initialFlippedCamera_T_world, pinholeCamera, objectPoints.data(), objectPoints.size(), imagePoints.data(), numberImagePoints, numberImagePoints * 55u / 100u, trackerRandomGenerator_, resultingFlippedCamera_T_world, Geometry::Error::ED_AMBIGUOUS, maxPixelError2, Vector3(Scalar(0.25), Scalar(0.25), Scalar(0.25)), Numeric::deg2rad(25), Scalar(0.01), nullptr, nullptr, nullptr, worker))
			{
				initialFlippedCamera_T_world = resultingFlippedCamera_T_world;

				if (RandomModelVariation::optimizedPoseFromPointCloudsWithOneInitialPoseIF<true>(initialFlippedCamera_T_world, pinholeCamera, objectPoints.data(), objectPoints.size(), imagePoints.data(), numberImagePoints, numberImagePoints * 50u / 100u, trackerRandomGenerator_, resultingFlippedCamera_T_world, Geometry::Error::ED_UNIQUE, maxPixelError3, Vector3(Scalar(0.2), Scalar(0.2), Scalar(0.2)), Numeric::deg2rad(20), Scalar(0.05), nullptr, nullptr, nullptr, worker))
				{
					world_T_camera = PinholeCamera::invertedFlipped2Standard(resultingFlippedCamera_T_world);

					return true;
				}
			}
		}
	}

	return false;
}

bool RMVFeatureTracker6DOF::determinePoseWithoutKnowledgeDefault(const PinholeCamera& pinholeCamera, const Vectors2& imagePoints, HomogenousMatrix4& world_T_camera, Worker* worker)
{
	const Vectors3& objectPoints = trackerFeatureMap_.initializationObjectPoints();
	unsigned int numberImagePoints = min((unsigned int)objectPoints.size() * 70u / 100u, (unsigned int)imagePoints.size());

	if (imagePoints.size() < 20)
	{
		return false;
	}

	ocean_assert(trackerPoseProjectionSet_.width() == pinholeCamera.width() && trackerPoseProjectionSet_.height() == pinholeCamera.height());

	// first we guess several suitable start poses due to the set of pre-defined poses
	// however, the accuracy can be very poor

	HomogenousMatrices4 roughFlippedCameras_T_world;

	{
		HomogenousMatrices4 world_T_roughCameras(8);
		trackerPoseProjectionSet_.findPosesWithMinimalError<Geometry::Estimator::ET_TUKEY>(imagePoints.data(), imagePoints.size(), (unsigned int)imagePoints.size() * 75u / 100u, Geometry::Error::ED_APPROXIMATED, world_T_roughCameras.size(), world_T_roughCameras.data(), nullptr, worker);

		roughFlippedCameras_T_world = PinholeCamera::standard2InvertedFlipped(world_T_roughCameras.data(), world_T_roughCameras.size());
	}

#ifdef OCEAN_DEBUG
	const Scalar timeout = 10.0;
#else
	const Scalar timeout = 0.15;
#endif

	Log::info() << "ImagePoints: " << numberImagePoints;
	Log::info() << "ObjectPoints: " << objectPoints.size();

	const Scalar resolutionFactor = cameraResolutionFactor(pinholeCamera);

	const Scalar maxPixelError0 = Numeric::sqr(Scalar(8.5) * resolutionFactor);
	const Scalar maxPixelError1 = Numeric::sqr(Scalar(4.5) * resolutionFactor);
	const Scalar maxPixelError2 = Numeric::sqr(Scalar(2.0) * resolutionFactor);

	Scalar resultingError = -1;

	// now we try to improve the rough poses randomly

	// first, we determine the best pose out of the set of given random poses (already slightly improved)

	HomogenousMatrix4 resultingFlippedCamera_T_world;
	if (RandomModelVariation::optimizedPoseFromPointCloudsWithSeveralInitialPosesIF<true>(roughFlippedCameras_T_world.data(), roughFlippedCameras_T_world.size(), pinholeCamera, objectPoints.data(), objectPoints.size(), imagePoints.data(), numberImagePoints, numberImagePoints * 60u / 100u, trackerRandomGenerator_, resultingFlippedCamera_T_world, Geometry::Error::ED_APPROXIMATED, maxPixelError0, Vector3(Scalar(0.1), Scalar(0.1), Scalar(0.1)), Numeric::deg2rad(15), timeout, &resultingError, nullptr, worker))
	{
		// second, we try to optimize the best resulting pose further (with slightly stinger conditions)

		HomogenousMatrix4 roughFlippedCamera_T_world = resultingFlippedCamera_T_world;

		if (RandomModelVariation::optimizedPoseFromPointCloudsWithOneInitialPoseIF<true>(roughFlippedCamera_T_world, pinholeCamera, objectPoints.data(), objectPoints.size(), imagePoints.data(), numberImagePoints, numberImagePoints * 60u / 100u, trackerRandomGenerator_, resultingFlippedCamera_T_world, Geometry::Error::ED_APPROXIMATED, maxPixelError1, Vector3(Scalar(0.05), Scalar(0.05), Scalar(0.05)), Numeric::deg2rad(12.5), timeout, nullptr, nullptr, nullptr, worker))
		{
			// third, we try to optimize the pose further (again with stronger conditions) and we determine point correspondences

			roughFlippedCamera_T_world = resultingFlippedCamera_T_world;

			IndexPairs32 correspondences;
			if (RandomModelVariation::optimizedPoseFromPointCloudsWithOneInitialPoseIF<true>(roughFlippedCamera_T_world, pinholeCamera, objectPoints.data(), objectPoints.size(), imagePoints.data(), numberImagePoints, numberImagePoints * 60u / 100u, trackerRandomGenerator_, resultingFlippedCamera_T_world, Geometry::Error::ED_AMBIGUOUS, maxPixelError2, Vector3(Scalar(0.02), Scalar(0.02), Scalar(0.02)), Numeric::deg2rad(10), timeout, nullptr, &correspondences, nullptr, worker))
			{
				// now as we found an almost precise pose we simply need to optimize the pose with traditional non-linear optimization approaches

				roughFlippedCamera_T_world = resultingFlippedCamera_T_world;

				Vectors2 validImagePoints;
				Vectors3 validObjectPoints;

				validImagePoints.reserve(correspondences.size());
				validObjectPoints.reserve(correspondences.size());

				for (const IndexPair32& correspondence : correspondences)
				{
					ocean_assert(correspondence.first < imagePoints.size());
					ocean_assert(correspondence.second < trackerFeatureMap_.initializationObjectPoints().size());

					validObjectPoints.push_back(trackerFeatureMap_.initializationObjectPoints()[correspondence.second]);
					validImagePoints.push_back(imagePoints[correspondence.first]);
				}

				if (Geometry::NonLinearOptimizationPose::optimizePoseIF(AnyCameraPinhole(PinholeCamera(pinholeCamera, false)), roughFlippedCamera_T_world, ConstArrayAccessor<Vector3>(validObjectPoints), ConstArrayAccessor<Vector2>(validImagePoints), resultingFlippedCamera_T_world, 30u, Geometry::Estimator::ET_HUBER, Scalar(0.001), Scalar(10), nullptr, nullptr))
				{
					// now we have a quite good pose based on the gathered points correspondences
					// now we apply a last fine-tuning with more points as we now should be able to find significant more feature correspondences

					roughFlippedCamera_T_world = resultingFlippedCamera_T_world;

					if (refinePoseIF(roughFlippedCamera_T_world, pinholeCamera, imagePoints, trackerFeatureMap_.objectPoints(), resultingFlippedCamera_T_world, 200, 400, Scalar(10) * resolutionFactor, Scalar(30 * 30), Numeric::maxValue(), worker) < 5u)
					{
						return false;
					}

					roughFlippedCamera_T_world = resultingFlippedCamera_T_world;

					if (refinePoseIF(roughFlippedCamera_T_world, pinholeCamera, imagePoints, trackerFeatureMap_.objectPoints(), resultingFlippedCamera_T_world, 400, 800, Scalar(4) * resolutionFactor, Scalar(2 * 2), Numeric::maxValue(), worker) < 5u)
					{
						return false;
					}

					world_T_camera = PinholeCamera::invertedFlipped2Standard(resultingFlippedCamera_T_world);

					return true;
				}
			}
		}
	}

	Log::info() << "Pose determination without knowledge failed, with error: " << resultingError;

	return false;
}

bool RMVFeatureTracker6DOF::determinePoseWithStrongPreviousCorrespondences(const HomogenousMatrix4& world_T_roughCamera, const PinholeCamera& pinholeCamera, Vectors2& imagePoints, HomogenousMatrix4& world_T_camera, Worker* worker)
{
	ocean_assert(world_T_roughCamera.isValid() && pinholeCamera.isValid());
	ocean_assert(motionModel_);

	ocean_assert(trackerFeatureMap_.recentStrongObjectPointIndices().size() >= 5 && imagePoints.size() >= 10);
	if (trackerFeatureMap_.recentStrongObjectPointIndices().size() < 5 || imagePoints.size() < 10)
	{
		return false;
	}

	// we try to re-find the previously used feature correspondences (we simply hope that the previous feature point locations do match quite well with the predicted location - and that in these image areas no other possible feature points occur)

	const HomogenousMatrix4 roughFlippedCamera_T_world(PinholeCamera::standard2InvertedFlipped(world_T_roughCamera));

	HomogenousMatrix4 resultingFlippedCamera_T_world;
	if (!refinePoseWithStrongPreviousCorrespondencesIF(roughFlippedCamera_T_world, pinholeCamera, imagePoints, resultingFlippedCamera_T_world))
	{
		return false;
	}

	const Scalar resolutionFactor = cameraResolutionFactor(pinholeCamera);

	HomogenousMatrix4 flippedCamera_T_world(resultingFlippedCamera_T_world);

	// now we try to improve/refine the pose

	const unsigned int validCorrespondencesFirst = refinePoseIF(flippedCamera_T_world, pinholeCamera, imagePoints, trackerFeatureMap_.objectPoints(), resultingFlippedCamera_T_world, 200u, 400u, Scalar(10) * resolutionFactor, Scalar(30 * 30), Numeric::maxValue(), worker);

	if (validCorrespondencesFirst <= 5u)
	{
		return false;
	}

	// and we try to improve/refine the pose again

	flippedCamera_T_world = resultingFlippedCamera_T_world;

	const unsigned int validCorrespondencesSecond = refinePoseIF(flippedCamera_T_world, pinholeCamera, imagePoints, trackerFeatureMap_.objectPoints(), resultingFlippedCamera_T_world, 400u, 600u, Scalar(4) * resolutionFactor, Scalar(2 * 2), Numeric::maxValue(), worker);

	if (validCorrespondencesSecond <= 20u)
	{
		return false;
	}

	flippedCamera_T_world = resultingFlippedCamera_T_world;

	// if we could not establish enough valid feature correspondences we give it another try, or if the camera pose is not similar to the rough guess
	if (validCorrespondencesSecond < 150 || roughFlippedCamera_T_world.rotation().angle(resultingFlippedCamera_T_world.rotation()) > Numeric::deg2rad(2.5))
	{
		const unsigned int validCorrespondencesThird = refinePoseIF(flippedCamera_T_world, pinholeCamera, imagePoints, trackerFeatureMap_.objectPoints(), resultingFlippedCamera_T_world, 800u, 1000u, Scalar(4) * resolutionFactor, Scalar(1.5 * 1.5), Numeric::maxValue(), worker);

		if (validCorrespondencesThird <= 30u)
		{
			return false;
		}
	}

	startAsynchronousDataProcessingIF(resultingFlippedCamera_T_world, pinholeCamera, std::move(imagePoints));

	world_T_camera = PinholeCamera::invertedFlipped2Standard(resultingFlippedCamera_T_world);

	return true;
}

bool RMVFeatureTracker6DOF::determinePoseWithAnyPreviousCorrespondences(const HomogenousMatrix4& world_T_roughCamera, const PinholeCamera& pinholeCamera, Vectors2& imagePoints, HomogenousMatrix4& world_T_camera, Worker* worker)
{
	ocean_assert(world_T_roughCamera.isValid() && pinholeCamera.isValid());
	ocean_assert(imagePoints.size() >= 10);

	if (imagePoints.size() < 30 || trackerFeatureMap_.recentUsedObjectPointIndices().size() < 30)
	{
		return false;
	}

	const unsigned int numberPreviousUsedProjectedObjectPoints = min(50u, (unsigned int)trackerFeatureMap_.recentUsedObjectPointIndices().size());
	const unsigned int numberImagePoints = min(100u, (unsigned int)imagePoints.size());

	if (numberPreviousUsedProjectedObjectPoints < 10)
	{
		return false;
	}

	// we take a very simple model (three is the minimal number of point correspondences)
	const unsigned int modelSize = 3u;

	const HomogenousMatrix4 roughFlippedCamera_T_world(PinholeCamera::standard2InvertedFlipped(world_T_roughCamera));

	// first we extract the subset of object points that has been used in the previous iteration, and we determine their projected image points

	const Vectors3 previousUsedObjectPoints(trackerFeatureMap_.recentUsedObjectPoints(numberPreviousUsedProjectedObjectPoints));
	ocean_assert(previousUsedObjectPoints.size() == size_t(numberPreviousUsedProjectedObjectPoints));

	Vectors2 previousUsedProjectedObjectPoints(previousUsedObjectPoints.size());
	pinholeCamera.projectToImageIF<true>(roughFlippedCamera_T_world, previousUsedObjectPoints.data(), previousUsedObjectPoints.size(), false, previousUsedProjectedObjectPoints.data());


	// now we determine a distribution array for the current image points as we want to have all possible feature candidates (out of the projected object points) for each image point within a fixed small radius

	const unsigned int bins = max(1u, min(pinholeCamera.width(), pinholeCamera.height()) / 10u); // each bin will have approx. 10^2 pixels

	const Geometry::SpatialDistribution::DistributionArray distributionImagePoints(Geometry::SpatialDistribution::distributeToArray(imagePoints.data(), numberImagePoints, Scalar(0), Scalar(0), Scalar(pinholeCamera.width()), Scalar(pinholeCamera.height()), bins, bins));

	IndexGroups32 neighborGroups;
	neighborGroups.reserve(numberPreviousUsedProjectedObjectPoints);
	for (const Vector2& projectedObjectPoint : previousUsedProjectedObjectPoints)
	{
		neighborGroups.push_back(Geometry::SpatialDistribution::determineNeighbors(projectedObjectPoint, imagePoints.data(), numberImagePoints, Scalar(3), distributionImagePoints));
	}

	// now we randomly selected subset of the projected object points and image points and try to determine the best resulting pose, this is a simple RANSAC approach

	Scalar bestTotalError = Numeric::maxValue();
	HomogenousMatrix4 bestFlippedCamera_T_world;

	Indices32 bestObjectPointIndices;
	Indices32 bestImagePointIndices;

	Indices32 iterationBestObjectPointIndices;
	Indices32 iterationBestImagePointIndices;

	Vectors3 iterationObjectPoints;
	Vectors2 iterationImagePoints;

	const Timestamp startTimestamp(true);

	do
	{
		iterationObjectPoints.clear();
		iterationImagePoints.clear();

		iterationBestObjectPointIndices.clear();
		iterationBestImagePointIndices.clear();

		IndexSet32 iterationMaskedObjectPoints;

		for (unsigned int n = 0u; n < numberPreviousUsedProjectedObjectPoints; ++n)
		{
			// we select one object prevously used object points which hasn't been selected in this iteration yet
			const unsigned int randomUsedObjectPointIndex = RandomI::random(trackerRandomGenerator_, numberPreviousUsedProjectedObjectPoints - 1u);
			if (iterationMaskedObjectPoints.find(randomUsedObjectPointIndex) != iterationMaskedObjectPoints.end())
			{
				continue;
			}

			// we select one of the neighbors randomly

			const Indices32& neihgbors = neighborGroups[randomUsedObjectPointIndex];

			if (!neihgbors.empty())
			{
				iterationMaskedObjectPoints.insert(randomUsedObjectPointIndex);

				const Vector2& iterationProjectedObjectPoint = previousUsedProjectedObjectPoints[randomUsedObjectPointIndex];
				const Vector3& iterationObjectPoint = previousUsedObjectPoints[randomUsedObjectPointIndex];

				const unsigned int candidateNeighborIndex = RandomI::random(trackerRandomGenerator_, (unsigned int)neihgbors.size() - 1u);
				ocean_assert(candidateNeighborIndex < neihgbors.size());

				ocean_assert(neihgbors[candidateNeighborIndex] < imagePoints.size());
				const Vector2& iterationImagePoint = imagePoints[neihgbors[candidateNeighborIndex]];
				ocean_assert_and_suppress_unused(iterationProjectedObjectPoint == pinholeCamera.projectToImageIF<true>(roughFlippedCamera_T_world, iterationObjectPoint, false), iterationProjectedObjectPoint);

				iterationImagePoints.push_back(iterationImagePoint);
				iterationObjectPoints.push_back(iterationObjectPoint);
			}

			if (iterationImagePoints.size() >= modelSize)
			{
				break;
			}
		}

		if (iterationImagePoints.size() < modelSize)
		{
			continue;
		}

		// now we optimize our rough pose for the randomly selected correspondences and measure the number of reliable points correspondences (using all object points - not only those from the subset) and the overall error

		Scalar iterationTotalError = 0;

		// **TODO** why not using a classical P3P?
		ocean_assert(iterationObjectPoints.size() == 3);

		HomogenousMatrix4 optimizedRoughFlippedCamera_T_world;
		if (Geometry::NonLinearOptimizationPose::optimizePoseIF(AnyCameraPinhole(PinholeCamera(pinholeCamera, false)), roughFlippedCamera_T_world, ConstArrayAccessor<Vector3>(iterationObjectPoints), ConstArrayAccessor<Vector2>(iterationImagePoints), optimizedRoughFlippedCamera_T_world, 20u, Geometry::Estimator::ET_SQUARE, Scalar(0.001), Scalar(10)))
		{
			// now we count the number of perfect/good matches

			Vectors2 iterationProjectedObjectPoints(previousUsedObjectPoints.size());
			pinholeCamera.projectToImageIF<true>(optimizedRoughFlippedCamera_T_world, previousUsedObjectPoints.data(), previousUsedObjectPoints.size(), false, iterationProjectedObjectPoints.data());

			for (unsigned int n = 0u; n < previousUsedObjectPoints.size(); ++n)
			{
				Scalar sqrDistance;
				const unsigned int nearestImagePointIndex = Geometry::SpatialDistribution::determineNearestNeighbor(iterationProjectedObjectPoints[n], imagePoints.data(), numberImagePoints, Scalar(2.5), distributionImagePoints, &sqrDistance);

				if (nearestImagePointIndex != (unsigned int)(-1))
				{
					ocean_assert(sqrDistance <= Scalar(2.5 * 2.5));

					iterationTotalError += sqrDistance;

					iterationBestObjectPointIndices.push_back(n);
					iterationBestImagePointIndices.push_back(nearestImagePointIndex);
				}
			}
		}

		ocean_assert(iterationBestObjectPointIndices.size() == iterationBestImagePointIndices.size());

		if (iterationBestObjectPointIndices.size() > bestObjectPointIndices.size() || (iterationBestObjectPointIndices.size() == bestObjectPointIndices.size() && iterationTotalError < bestTotalError))
		{
			bestFlippedCamera_T_world = optimizedRoughFlippedCamera_T_world;
			bestTotalError = iterationTotalError;

			std::swap(bestObjectPointIndices, iterationBestObjectPointIndices);
			std::swap(bestImagePointIndices, iterationBestImagePointIndices);
		}

		ocean_assert(bestObjectPointIndices.size() == bestImagePointIndices.size());

		// we stop if more than 70% of all (previous) object points have a good matching
		if (bestObjectPointIndices.size() >= numberPreviousUsedProjectedObjectPoints * 70u / 100u)
		{
			break;
		}
	}
	while (Timestamp(true) < startTimestamp + 0.01);

	ocean_assert(bestObjectPointIndices.size() == bestImagePointIndices.size());

	if (bestObjectPointIndices.size() < 5)
	{
		return false;
	}

#ifdef OCEAN_DEBUG
	{
		// we simply check whether we have the correct indices for object points and image points
		Scalar sqrAverageError = Numeric::maxValue();
		Scalar sqrMinimalError = Numeric::maxValue();
		Scalar sqrMaximalError = Numeric::maxValue();
		Geometry::Error::determinePoseErrorIF<ConstTemplateArraySubsetAccessor<Vector3, Index32>, ConstTemplateArraySubsetAccessor<Vector2, Index32>, true>(bestFlippedCamera_T_world, pinholeCamera, ConstTemplateArraySubsetAccessor<Vector3, Index32>(previousUsedObjectPoints, bestObjectPointIndices), ConstTemplateArraySubsetAccessor<Vector2, Index32>(imagePoints.data(), bestImagePointIndices), false, sqrAverageError, sqrMinimalError, sqrMaximalError);

		ocean_assert(sqrMaximalError <= Scalar(2.5 * 2.5));
	}
#endif

	HomogenousMatrix4 optimizedFlippedCamera_T_world;
	if (!Geometry::NonLinearOptimizationPose::optimizePoseIF(AnyCameraPinhole(PinholeCamera(pinholeCamera, false)), bestFlippedCamera_T_world, ConstArraySubsetAccessor<Vector3, Index32>(previousUsedObjectPoints, bestObjectPointIndices), ConstArraySubsetAccessor<Vector2, Index32>(imagePoints.data(), bestImagePointIndices), optimizedFlippedCamera_T_world, 30u, Geometry::Estimator::ET_SQUARE))
	{
		return false;
	}

	bestFlippedCamera_T_world = optimizedFlippedCamera_T_world;

	const unsigned int validCorrespondencesFirst = refinePoseIF(bestFlippedCamera_T_world, pinholeCamera, imagePoints, trackerFeatureMap_.recentUsedObjectPoints(), optimizedFlippedCamera_T_world, 150u, 100u, Scalar(5), Scalar(3 * 3), Scalar(1.5 * 1.5), worker);

	if (validCorrespondencesFirst < 10u)
	{
		return false;
	}

	bestFlippedCamera_T_world = optimizedFlippedCamera_T_world;

	const unsigned int validCorrespondencesSecond = refinePoseIF(bestFlippedCamera_T_world, pinholeCamera, imagePoints, trackerFeatureMap_.objectPoints(), optimizedFlippedCamera_T_world, 400u, 200u, Scalar(5), Scalar(3 * 3), Scalar(1.5 * 1.5), worker);

	if (validCorrespondencesSecond < 10u)
	{
		return false;
	}

	startAsynchronousDataProcessingIF(optimizedFlippedCamera_T_world, pinholeCamera, std::move(imagePoints));

	world_T_camera = PinholeCamera::invertedFlipped2Standard(optimizedFlippedCamera_T_world);

	return true;
}

bool RMVFeatureTracker6DOF::determinePoseWithRoughPose(const HomogenousMatrix4& world_T_roughCamera, const PinholeCamera& pinholeCamera, Vectors2& imagePoints, HomogenousMatrix4& world_T_camera, Worker* worker)
{
	ocean_assert(world_T_roughCamera.isValid() && pinholeCamera.isValid());
	ocean_assert(imagePoints.size() >= 5);

	const Scalar resolutionFactor = cameraResolutionFactor(pinholeCamera);

	const Scalar maxPixelError0 = Numeric::sqr(Scalar(3.1) * resolutionFactor);

	const Vectors3& objectPoints = trackerFeatureMap_.objectPoints();

	const unsigned int numberObjectPoints = min(60u, (unsigned int)objectPoints.size());
	const unsigned int numberImagePoints = min(numberObjectPoints * 2u, (unsigned int)imagePoints.size());

	const Scalar objectPointsDiagonal = trackerFeatureMap_.boundingBox().diagonal();
	const Scalar maximalTranslationSmall = objectPointsDiagonal * Scalar(0.05);

#ifdef OCEAN_DEBUG
	const double timeout = 10.0;
#else
	const double timeout = 0.05;
#endif

	const HomogenousMatrix4 roughFlippedCamera_T_world(PinholeCamera::standard2InvertedFlipped(world_T_roughCamera));

	HomogenousMatrix4 resultingFlippedCamera_T_world;
	IndexPairs32 correspondences;

	if (numberImagePoints < numberObjectPoints)
	{
		if (!RandomModelVariation::optimizedPoseFromPointCloudsWithOneInitialPoseIF<true>(roughFlippedCamera_T_world, pinholeCamera, objectPoints.data(), numberObjectPoints, imagePoints.data(), numberImagePoints, numberImagePoints * 60u / 100u, trackerRandomGenerator_, resultingFlippedCamera_T_world, Geometry::Error::ED_APPROXIMATED, maxPixelError0, Vector3(maximalTranslationSmall, maximalTranslationSmall, maximalTranslationSmall), Numeric::deg2rad(5), timeout, nullptr, &correspondences, nullptr, worker))
		{
			ocean_assert(correspondences.empty());

			// we give it a second chance with slightly simpler conditions
			if (!RandomModelVariation::optimizedPoseFromPointCloudsWithOneInitialPoseIF<true>(roughFlippedCamera_T_world, pinholeCamera, objectPoints.data(), numberObjectPoints, imagePoints.data(), numberImagePoints, numberImagePoints * 60u / 100u, trackerRandomGenerator_, resultingFlippedCamera_T_world, Geometry::Error::ED_APPROXIMATED, maxPixelError0, Vector3(maximalTranslationSmall, maximalTranslationSmall, maximalTranslationSmall) * Scalar(2), Numeric::deg2rad(10), timeout * 2.0, nullptr, &correspondences, nullptr, worker))
			{
				return false;
			}
		}
	}
	else
	{
		if (!RandomModelVariation::optimizedPoseFromPointCloudsWithOneInitialPoseIF<false>(roughFlippedCamera_T_world, pinholeCamera, objectPoints.data(), numberObjectPoints, imagePoints.data(), numberImagePoints, numberObjectPoints * 60u / 100u, trackerRandomGenerator_, resultingFlippedCamera_T_world, Geometry::Error::ED_APPROXIMATED, maxPixelError0, Vector3(maximalTranslationSmall, maximalTranslationSmall, maximalTranslationSmall), Numeric::deg2rad(5), timeout, nullptr, &correspondences, nullptr, worker))
		{
			ocean_assert(correspondences.empty());

			// we give it a second chance with slightly simpler conditions
			if (!RandomModelVariation::optimizedPoseFromPointCloudsWithOneInitialPoseIF<false>(roughFlippedCamera_T_world, pinholeCamera, objectPoints.data(), numberObjectPoints, imagePoints.data(), numberImagePoints, numberObjectPoints * 60u / 100u, trackerRandomGenerator_, resultingFlippedCamera_T_world, Geometry::Error::ED_APPROXIMATED, maxPixelError0, Vector3(maximalTranslationSmall, maximalTranslationSmall, maximalTranslationSmall) * Scalar(2), Numeric::deg2rad(10), timeout * 2.0, nullptr, &correspondences, nullptr, worker))
			{
				return false;
			}
		}
	}

	const HomogenousMatrix4 initialFlippedCamera_T_world(resultingFlippedCamera_T_world);

	// we could improve the rough pose and now we also can use the guessed point correspondences to apply a non-linear pose optimization

	Vectors3 validObjectPoints;
	validObjectPoints.reserve(correspondences.size());
	Vectors2 validImagePoints;
	validImagePoints.reserve(correspondences.size());

	for (const IndexPair32& correspondence : correspondences)
	{
		ocean_assert(correspondence.first < imagePoints.size());
		ocean_assert(correspondence.second < trackerFeatureMap_.objectPoints().size());

		validObjectPoints.push_back(trackerFeatureMap_.objectPoints()[correspondence.first]);
		validImagePoints.push_back(imagePoints[correspondence.second]);
	}

	ocean_assert(!validImagePoints.empty());

	if (validImagePoints.size() < 5)
	{
		return false;
	}

	// first we apply the Huber estimator as we still do not know how good the correspondences are

	if (!Geometry::NonLinearOptimizationPose::optimizePoseIF(AnyCameraPinhole(PinholeCamera(pinholeCamera, false)), initialFlippedCamera_T_world, ConstArrayAccessor<Vector3>(validObjectPoints), ConstArrayAccessor<Vector2>(validImagePoints), resultingFlippedCamera_T_world, 30u, Geometry::Estimator::ET_HUBER, Scalar(0.001), Scalar(10), nullptr, nullptr))
	{
		return false;
	}

	HomogenousMatrix4 flippedCamera_T_world(resultingFlippedCamera_T_world);

	// now we apply the Tukey estimator to filter outliers

	if (!Geometry::NonLinearOptimizationPose::optimizePoseIF(AnyCameraPinhole(PinholeCamera(pinholeCamera, false)), flippedCamera_T_world, ConstArrayAccessor<Vector3>(validObjectPoints), ConstArrayAccessor<Vector2>(validImagePoints), resultingFlippedCamera_T_world, 30u, Geometry::Estimator::ET_TUKEY, Scalar(0.001), Scalar(10), nullptr, nullptr))
	{
		return false;
	}

	flippedCamera_T_world = resultingFlippedCamera_T_world;

	// finally we refine the current pose based on the entire set of image and object points, first with a small amount of point correspondences and week uniqueness constraints

	if (refinePoseIF(flippedCamera_T_world, pinholeCamera, imagePoints, trackerFeatureMap_.objectPoints(), resultingFlippedCamera_T_world, 200u, 400u, Scalar(10) * resolutionFactor, Scalar(30 * 30), Numeric::maxValue(), worker) < 10u) // **TOOD** the resulting number of correspondences should be rated wrt. the covered area (e.g., 10 points is too low if the entire pattern is visible)
	{
		return false;
	}

	flippedCamera_T_world = resultingFlippedCamera_T_world;

	// now we take a larger amount of point correspondences and we apply strong uniqueness constraints

	if (refinePoseIF(flippedCamera_T_world, pinholeCamera, imagePoints, trackerFeatureMap_.objectPoints(), resultingFlippedCamera_T_world, 400u, 800u, Scalar(4) * resolutionFactor, Scalar(2 * 2), Numeric::maxValue(), worker) < 10u)
	{
		return false;
	}

	startAsynchronousDataProcessingIF(resultingFlippedCamera_T_world, pinholeCamera, std::move(imagePoints));

	world_T_camera = PinholeCamera::invertedFlipped2Standard(resultingFlippedCamera_T_world);

	return true;
}

bool RMVFeatureTracker6DOF::refinePoseWithStrongPreviousCorrespondencesIF(const HomogenousMatrix4& roughFlippedCamera_T_world, const PinholeCamera& pinholeCamera, const Vectors2& imagePoints, HomogenousMatrix4& flippedCamera_T_world)
{
	ocean_assert(roughFlippedCamera_T_world.isValid() && pinholeCamera.isValid() && imagePoints.size() >= 10);

	if (imagePoints.size() < 10)
	{
		return false;
	}

	const Scalar resolutionFactor = cameraResolutionFactor(pinholeCamera);

	// first we try to identify/guess the current image points that correspond to the strong 2D/3D correspondences of the previous frame

	Vectors3 candidateObjectPoints;
	Vectors2 candidateImagePoints;

	addUniqueCorrespondencesIF(roughFlippedCamera_T_world, pinholeCamera, imagePoints.data(), min(imagePoints.size(), size_t(200)), ConstArraySubsetAccessor<Vector3, Index32>(trackerFeatureMap_.objectPoints(), trackerFeatureMap_.recentStrongObjectPointIndices()), trackerStrongCorrespondencesEmptyAreaRadius_ * resolutionFactor, Scalar(4 * 4), candidateObjectPoints, candidateImagePoints);
	ocean_assert(candidateObjectPoints.size() == candidateImagePoints.size());

	if (candidateObjectPoints.size() <= 10 && !trackerFeatureMap_.recentSemiStrongObjectPointIndices().empty())
	{
		// if we could not find enough strong correspondences so that we now weaken the conditions - so that we add more correspondences with more generous properties

		addUniqueCorrespondencesIF(roughFlippedCamera_T_world, pinholeCamera, imagePoints.data(), min(imagePoints.size(), size_t(300)), ConstArraySubsetAccessor<Vector3, Index32>(trackerFeatureMap_.objectPoints(), trackerFeatureMap_.recentSemiStrongObjectPointIndices()), trackerSemiStrongCorrespondencesEmptyAreaRadius_ * resolutionFactor, Scalar(2.5 * 2.5), candidateObjectPoints, candidateImagePoints);
		ocean_assert(candidateObjectPoints.size() == candidateImagePoints.size());
	}

	if (candidateObjectPoints.size() < 5)
	{
		return false;
	}

	// we could find/guess enough correspondences so that we now improve the rough pose based on a non-linear optimization, first we a Huber estimator (as we still do not know how good the quality of the correspondences is)

	HomogenousMatrix4 optimizedFlippedCamera_T_world;
	if (!Geometry::NonLinearOptimizationPose::optimizePoseIF(AnyCameraPinhole(PinholeCamera(pinholeCamera, false)), roughFlippedCamera_T_world, ConstArrayAccessor<Vector3>(candidateObjectPoints), ConstArrayAccessor<Vector2>(candidateImagePoints), optimizedFlippedCamera_T_world, 10u, Geometry::Estimator::ET_HUBER))
	{
		return false;
	}

	// and now we apply a Tukey estimator to remove outliers

	if (!Geometry::NonLinearOptimizationPose::optimizePoseIF(AnyCameraPinhole(PinholeCamera(pinholeCamera, false)), optimizedFlippedCamera_T_world, ConstArrayAccessor<Vector3>(candidateObjectPoints), ConstArrayAccessor<Vector2>(candidateImagePoints), flippedCamera_T_world, 5u, Geometry::Estimator::ET_TUKEY))
	{
		return false;
	}

	return true;
}

Vectors2 RMVFeatureTracker6DOF::detectFeatures(const Frame& yFrame, const bool frameIsUndistorted, const Box2& boundingBox, Worker* worker)
{
	ocean_assert(yFrame.pixelFormat() == FrameType::FORMAT_Y8);

	// the minimal and maximal strength thresholds, and the delta to adjust the thresholds
	static const Scalar featureDetectorStrengthMin = 5;
	static const Scalar featureDetectorStrengthMax = 200;
	static const Scalar featureDetectorStrengthDelta = 5;

	// the number of feature points we want to detect in each frame
	static const size_t featureDetectorIdealNumber = 400;

	Frame adjustedFrame(yFrame, Frame::ACM_USE_KEEP_LAYOUT);
	if (RMVFeatureDetector::needSmoothedFrame(trackerDetectorType_))
	{
		CV::FrameFilterGaussian::filter(yFrame, adjustedFrame, 3u, worker);
	}

	const Vectors2 imagePoints(RMVFeatureDetector::detectFeatures(adjustedFrame, boundingBox, trackerDetectorType_, trackerFeatureDetectorStrength_, frameIsUndistorted, worker));

	if (imagePoints.size() < featureDetectorIdealNumber)
	{
		trackerFeatureDetectorStrength_ = max(featureDetectorStrengthMin, trackerFeatureDetectorStrength_ - featureDetectorStrengthDelta);
	}
	else
	{
		trackerFeatureDetectorStrength_ = min(trackerFeatureDetectorStrength_ + featureDetectorStrengthDelta, featureDetectorStrengthMax);
	}

	return imagePoints;
}

bool RMVFeatureTracker6DOF::asynchronousDataProcessed()
{
	const ScopedLock scopedLock(trackerAsynchronousDataProcessingLock_);
	return trackerAsynchronousDataProcessingActive_ == false;
}

void RMVFeatureTracker6DOF::startAsynchronousDataProcessingIF(const HomogenousMatrix4& flippedCamera_T_world, const PinholeCamera& pinholeCamera, Vectors2&& imagePoints)
{
	const ScopedLock scopedLock(trackerAsynchronousDataProcessingLock_);

	ocean_assert(trackerAsynchronousDataProcessingFlippedCamera_T_world_.isNull());
	trackerAsynchronousDataProcessingFlippedCamera_T_world_ = flippedCamera_T_world;

	ocean_assert(!trackerAsynchronousDataProcessingCamera_.isValid());
	trackerAsynchronousDataProcessingCamera_ = pinholeCamera;

	ocean_assert(trackerAsynchronousDataProcessingImagePoints_.empty());
	trackerAsynchronousDataProcessingImagePoints_ = std::move(imagePoints);

	trackerAsynchronousSignal_.pulse();
}

void RMVFeatureTracker6DOF::threadRun()
{
	while (!shouldThreadStop())
	{
		trackerAsynchronousSignal_.wait();

		if (shouldThreadStop())
		{
			break;
		}

		const ScopedLock scopedLock(trackerAsynchronousDataProcessingLock_);

		ocean_assert(!trackerAsynchronousDataProcessingActive_);
		const ScopedValueT<bool> scopedValue(trackerAsynchronousDataProcessingActive_, false, true);

		ocean_assert(trackerAsynchronousDataProcessingFlippedCamera_T_world_.isValid());

		Indices32 strongObjectPointIndices, moderateObjectPointIndices, usedObjectPointIndices;
		determineUsedFeaturesIF(trackerAsynchronousDataProcessingFlippedCamera_T_world_, trackerAsynchronousDataProcessingCamera_, trackerAsynchronousDataProcessingImagePoints_, 30, strongObjectPointIndices, moderateObjectPointIndices, usedObjectPointIndices);

		trackerFeatureMap_.setMostRecentObjectPointIndices(std::move(strongObjectPointIndices), std::move(moderateObjectPointIndices), std::move(usedObjectPointIndices));

		trackerAsynchronousDataProcessingFlippedCamera_T_world_.toNull();
		trackerAsynchronousDataProcessingCamera_ = PinholeCamera();
		trackerAsynchronousDataProcessingImagePoints_.clear();
	}
}

bool RMVFeatureTracker6DOF::determineUsedFeaturesIF(const HomogenousMatrix4& fineFlippedCamera_T_world, const PinholeCamera& pinholeCamera, const Vectors2& imagePoints, const size_t minimalStrongObjectPoints, Indices32& strongObjectPointIndices, Indices32& moderateObjectPointIndices, Indices32& usedObjectPointIndices)
{
	ocean_assert(fineFlippedCamera_T_world.isValid() && pinholeCamera.isValid());
	ocean_assert(strongObjectPointIndices.empty() && moderateObjectPointIndices.empty() && usedObjectPointIndices.empty());
	ocean_assert(minimalStrongObjectPoints >= 1);

	if (trackerFeatureMap_.objectPoints().empty() || imagePoints.empty())
	{
		return false;
	}

	const Scalar resolutionFactor = cameraResolutionFactor(pinholeCamera);

	Vectors2 projectedObjectPoints(trackerFeatureMap_.objectPoints().size());
	pinholeCamera.projectToImageIF<true>(fineFlippedCamera_T_world, trackerFeatureMap_.objectPoints().data(), projectedObjectPoints.size(), false, projectedObjectPoints.data());

	const size_t maximalImagePoints = min(size_t(200), imagePoints.size());
	const size_t maximalObjectPoints = min(size_t(400), trackerFeatureMap_.objectPoints().size());

	// now we seek image features with large distance to neighboring image features

	const unsigned int bins = max(1u, min(pinholeCamera.width(), pinholeCamera.height()) / 20u); // each bin will have approx. 20^2 pixels

	const Geometry::SpatialDistribution::DistributionArray distributionProjectedObjectPoints(Geometry::SpatialDistribution::distributeToArray(projectedObjectPoints.data(), projectedObjectPoints.size(), Scalar(0), Scalar(0), Scalar(pinholeCamera.width()), Scalar(pinholeCamera.height()), bins, bins));
	const Geometry::SpatialDistribution::DistributionArray distributionImagePoints(Geometry::SpatialDistribution::distributeToArray(imagePoints.data(), maximalImagePoints, Scalar(0), Scalar(0), Scalar(pinholeCamera.width()), Scalar(pinholeCamera.height()), bins, bins));

	// first we seek for strong feature points (unique and accurate) and no other image points in the neighborhood

	const PointCorrespondences::RedundantCorrespondences strongRedundantCorrespondences(PointCorrespondences::determineNearestCandidates(imagePoints.data(), maximalImagePoints, projectedObjectPoints.data(), maximalObjectPoints, Scalar(10) * resolutionFactor, distributionProjectedObjectPoints));
	for (const PointCorrespondences::RedundantCorrespondence& redundantCorrespondence : strongRedundantCorrespondences)
	{
		if (redundantCorrespondence.isUniqueAndAccurate(Scalar(10 * 10), Numeric::sqr(2 * resolutionFactor)))
		{
			ocean_assert(maximalImagePoints >= strongRedundantCorrespondences.size());

			if (Geometry::SpatialDistribution::determineMinimalSqrDistance(imagePoints.data(), strongRedundantCorrespondences.size(), redundantCorrespondence.index(), distributionImagePoints) > Numeric::sqr(trackerStrongCorrespondencesEmptyAreaRadius_ * resolutionFactor))
			{
				// the indices of the strong object points
				strongObjectPointIndices.push_back(redundantCorrespondence.candidateIndex());
			}
		}
	}

	ocean_assert(IndexSet32(strongObjectPointIndices.begin(), strongObjectPointIndices.end()).size() == strongObjectPointIndices.size());

	// we sort the indices as we want to ensure that subsets of these indices will provide the strongest feature points (as all feature points are ordered regarding their strength)
	std::sort(strongObjectPointIndices.begin(), strongObjectPointIndices.end());

	// now we seek moderate/semi-strong features if not enough strong features have been found

	if (strongObjectPointIndices.size() < minimalStrongObjectPoints)
	{
		// semi-strong features must not be strong features
		const IndexSet32 strongIndexSet(strongObjectPointIndices.begin(), strongObjectPointIndices.end());

		for (const PointCorrespondences::RedundantCorrespondence& redundantCorrespondence : strongRedundantCorrespondences)
		{
			if (strongIndexSet.find(redundantCorrespondence.candidateIndex()) == strongIndexSet.end() && redundantCorrespondence.isUniqueAndAccurate(5 * 5, Numeric::sqr(4 * resolutionFactor)))
			{
				ocean_assert(maximalImagePoints >= strongRedundantCorrespondences.size());

				if (Geometry::SpatialDistribution::determineMinimalSqrDistance(imagePoints.data(), strongRedundantCorrespondences.size(), redundantCorrespondence.index(), distributionImagePoints) > Numeric::sqr(trackerSemiStrongCorrespondencesEmptyAreaRadius_ * resolutionFactor))
				{
					moderateObjectPointIndices.push_back(redundantCorrespondence.candidateIndex());
				}
			}
		}
	}

	ocean_assert(IndexSet32(moderateObjectPointIndices.begin(), moderateObjectPointIndices.end()).size() == moderateObjectPointIndices.size());
	std::sort(moderateObjectPointIndices.begin(), moderateObjectPointIndices.end());

	// now we seek any object points that is unique (may contain strong and moderate object points)

	IndexSet32 objectPointIndexSet;
	const PointCorrespondences::RedundantCorrespondences weakRedundantCorrespondences(PointCorrespondences::determineNearestCandidates(imagePoints.data(), min(size_t(400), imagePoints.size()), projectedObjectPoints.data(), min(size_t(800), projectedObjectPoints.size()), 4 * resolutionFactor, distributionProjectedObjectPoints));
	for (const PointCorrespondences::RedundantCorrespondence& redundantCorrespondence : weakRedundantCorrespondences)
	{
		if (redundantCorrespondence.isUnique(2 * 2))
		{
			ocean_assert(redundantCorrespondence.candidateIndex() < trackerFeatureMap_.objectPoints().size());

			// although an object point should not be unique for more than one image point, some object points may occur more than once, therefore we use a set
			objectPointIndexSet.insert(redundantCorrespondence.candidateIndex());
		}
	}

	usedObjectPointIndices = Indices32(objectPointIndexSet.begin(), objectPointIndexSet.end());

	return true;
}

void RMVFeatureTracker6DOF::addUniqueCorrespondencesIF(const HomogenousMatrix4& roughFlippedCamera_T_world, const PinholeCamera& pinholeCamera, const Vector2* imagePoints, const size_t numberImagePoints, const ConstIndexedAccessor<Vector3>& objectPointAccessor, const Scalar searchWindow, const Scalar uniquenessSqrFactor, Vectors3& resultingObjectPoints, Vectors2& resultingImagePoints)
{
	ocean_assert(roughFlippedCamera_T_world.isValid() && pinholeCamera.isValid());
	ocean_assert(imagePoints && numberImagePoints != 0 && objectPointAccessor.size() != 0);

	ocean_assert(searchWindow > 0 && uniquenessSqrFactor > 0);
	ocean_assert(resultingObjectPoints.size() == resultingImagePoints.size());

	Vectors2 projectedObjectPoints;
	projectedObjectPoints.reserve(objectPointAccessor.size());

	for (size_t n = 0; n < objectPointAccessor.size(); ++n)
	{
		projectedObjectPoints.push_back(pinholeCamera.projectToImageIF<true>(roughFlippedCamera_T_world, objectPointAccessor[n], false));
	}

	const PointCorrespondences::RedundantCorrespondences redundantCorrespondences(PointCorrespondences::determineNearestCandidates(projectedObjectPoints.data(), projectedObjectPoints.size(), imagePoints, numberImagePoints, searchWindow));

	ocean_assert(resultingObjectPoints.size() == resultingImagePoints.size());
	resultingObjectPoints.reserve(resultingObjectPoints.size() + redundantCorrespondences.size());
	resultingImagePoints.reserve(resultingImagePoints.size() + redundantCorrespondences.size());

	for (const PointCorrespondences::RedundantCorrespondence& redundantCorrespondence : redundantCorrespondences)
	{
		if (redundantCorrespondence.isUnique(uniquenessSqrFactor))
		{
			ocean_assert(redundantCorrespondence.index() < objectPointAccessor.size());
			ocean_assert(redundantCorrespondence.candidateIndex() < numberImagePoints);

			const Vector3& objectPoint = objectPointAccessor[redundantCorrespondence.index()];

			if (PinholeCamera::isObjectPointInFrontIF(roughFlippedCamera_T_world, objectPoint))
			{
				resultingObjectPoints.push_back(objectPoint);
				resultingImagePoints.push_back(imagePoints[redundantCorrespondence.candidateIndex()]);
			}
		}
	}
}

unsigned int RMVFeatureTracker6DOF::refinePoseIF(const HomogenousMatrix4& roughFlippedCamera_T_world, const PinholeCamera& pinholeCamera, const Vectors2& imagePoints, const Vectors3& objectPoints, HomogenousMatrix4& flippedCamera_T_world, const unsigned int useNumberImagePoints, const unsigned int useNumberObjectPoints, const Scalar searchWindow, const Scalar uniquenessSqrFactor, const Scalar maxSqrDistance, Worker* /*worker*/)
{
	ocean_assert(roughFlippedCamera_T_world.isValid() && pinholeCamera.isValid());
	ocean_assert(imagePoints.size() >= 10 && objectPoints.size() >= 10);

	const unsigned int numberImagePoints = min(useNumberImagePoints, (unsigned int)imagePoints.size());
	const unsigned int numberObjectPoints = min(useNumberObjectPoints, (unsigned int)objectPoints.size());

	ocean_assert(numberImagePoints >= 10u && numberObjectPoints >= 10u);
	if (numberImagePoints < 10u || numberObjectPoints < 10u)
	{
		return 0u;
	}

	Vectors2 projectedObjectPoints(numberObjectPoints);
	pinholeCamera.projectToImageIF<true>(roughFlippedCamera_T_world, objectPoints.data(), numberObjectPoints, false, projectedObjectPoints.data());

	// we determine possible correspondence candidates (and also store a second candidate for each point)
	const PointCorrespondences::RedundantCorrespondences redundantCorrespondences(PointCorrespondences::determineNearestCandidates(imagePoints.data(), numberImagePoints, projectedObjectPoints.data(), numberObjectPoints, pinholeCamera.width(), pinholeCamera.height(), searchWindow));

	Vectors2 correspondenceImagePoints;
	correspondenceImagePoints.reserve(redundantCorrespondences.size());

	Vectors3 correspondenceObjectPoints;
	correspondenceObjectPoints.reserve(redundantCorrespondences.size());

	for (const PointCorrespondences::RedundantCorrespondence& redundantCorrespondence : redundantCorrespondences)
	{
		if (redundantCorrespondence.isUniqueAndAccurate(uniquenessSqrFactor, maxSqrDistance)) // in the case maxSqrDistance == Numeric::maxValue() we have the simple isUnique() check
		{
			ocean_assert(redundantCorrespondence.index() < imagePoints.size());
			ocean_assert(redundantCorrespondence.candidateIndex() < objectPoints.size());

			correspondenceImagePoints.push_back(imagePoints[redundantCorrespondence.index()]);
			correspondenceObjectPoints.push_back(objectPoints[redundantCorrespondence.candidateIndex()]);
		}
	}

	if (correspondenceImagePoints.size() < 5u)
	{
		return 0u;
	}

	ocean_assert(correspondenceImagePoints.size() == correspondenceObjectPoints.size());

	if (!Geometry::NonLinearOptimizationPose::optimizePoseIF(AnyCameraPinhole(PinholeCamera(pinholeCamera, false)), roughFlippedCamera_T_world, ConstArrayAccessor<Vector3>(correspondenceObjectPoints), ConstArrayAccessor<Vector2>(correspondenceImagePoints), flippedCamera_T_world, 5u, Geometry::Estimator::ET_HUBER, Scalar(0.001), Scalar(10)))
	{
		return 0u;
	}

	const HomogenousMatrix4 optimizedFlippedCamera_T_world = flippedCamera_T_world;

	if (!Geometry::NonLinearOptimizationPose::optimizePoseIF(AnyCameraPinhole(PinholeCamera(pinholeCamera, false)), optimizedFlippedCamera_T_world, ConstArrayAccessor<Vector3>(correspondenceObjectPoints), ConstArrayAccessor<Vector2>(correspondenceImagePoints), flippedCamera_T_world, 5u, Geometry::Estimator::ET_TUKEY, Scalar(0.001), Scalar(10)))
	{
		return 0u;
	}

	ocean_assert(correspondenceImagePoints.size() >= 5);
	return (unsigned int)correspondenceImagePoints.size();
}

}

}

}
