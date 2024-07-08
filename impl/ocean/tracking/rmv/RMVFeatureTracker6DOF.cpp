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
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameFilterGaussian.h"
#include "ocean/cv/FrameShrinker.h"

#include "ocean/geometry/NonLinearOptimizationPose.h"
#include "ocean/geometry/RANSAC.h"
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
	trackerDetectorType(detectorType),
	trackerFeatureDetectorStrength(25),
	trackerMaximalPoseProjectionFeatureNumber(150),
	trackerStrongCorrespondencesEmptyAreaRadius(10.0),
	trackerSemiStrongCorrespondencesEmptyAreaRadius(6.0),
	trackerAsynchronousDataProcessingActive(false),
	trackerAsynchronousDataProcessingPoseIF(false)
{
	startThread();
}

RMVFeatureTracker6DOF::~RMVFeatureTracker6DOF()
{
	stopThread();
	trackerAsynchronousSignal.release();
	joinThread();

	stopThreadExplicitly();
}

void RMVFeatureTracker6DOF::setFeatureMap(const RMVFeatureMap& featureMap, RandomGenerator& randomGenerator, const bool autoUpdateMaxPositionOffset)
{
	const ScopedLock scopedLock(lock_);

	trackerFeatureMap = featureMap;

	const Box3& initializationBoundingBox = featureMap.initializationBoundingBox();
	const Scalar boundingBoxDiagonal = initializationBoundingBox.diagonal();

	if (autoUpdateMaxPositionOffset)
	{
		// updates the maximal position offset between two frames to a fifth of the feature map bounding box diagonal
		const Scalar diagonal_5 = boundingBoxDiagonal * Scalar(0.2);
		maxPositionOffset_ = Vector3(diagonal_5, diagonal_5, diagonal_5); // **TODO** use/check max positions between frames
	}

	const Vectors3& initializationObjectPoints = featureMap.initializationObjectPoints();

	trackerPoseProjectionSet.setDimension(featureMap.initializationCamera().width(), featureMap.initializationCamera().height());

	if (initializationBoundingBox.isValid() && !initializationObjectPoints.empty() && trackerMaximalPoseProjectionFeatureNumber != 0)
	{
#if 1
		// we can either use random poses based on a hemisphere
		const HomogenousMatrices4 randomPoses = RandomizedPose::hemispherePoses(initializationBoundingBox, randomGenerator, boundingBoxDiagonal * Scalar(0.8), 12u, 12u);
#else
		// or we can use random poses with visibility constraints
		HomogenousMatrices4 randomPoses(12u * 12u + 12u);
		RandomizedPose::randomPoses(featureMap.initializationCamera(), initializationBoundingBox, randomGenerator, boundingBoxDiagonal * Scalar(0.7), boundingBoxDiagonal * Scalar(0.9), Scalar(0.25), randomPoses.size(), randomPoses.data(), WorkerPool::get().scopedWorker()());
#endif

		trackerPoseProjectionSet.clear();

		for (HomogenousMatrices4::const_iterator i = randomPoses.begin(); i != randomPoses.end(); ++i)
			trackerPoseProjectionSet.addPoseProjection(Tracking::PoseProjection(*i, featureMap.initializationCamera(), initializationObjectPoints.data(), min(initializationObjectPoints.size(), trackerMaximalPoseProjectionFeatureNumber), false));
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

	if (!trackerFeatureMap)
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

	HomogenousMatrix4 pose;
	if (internDeterminePose(frame, pinholeCamera, frameIsUndistorted, pose, worker))
	{
		motionModel_.update(pose);

		transformations.push_back(TransformationSample(pose, 0u));
		return true;
	}

	motionModel_.reset();
	return false;
}

bool RMVFeatureTracker6DOF::internDeterminePose(const Frame& frame, const PinholeCamera& pinholeCamera, const bool frameIsUndistorted, HomogenousMatrix4& pose, Worker* worker)
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

		const HomogenousMatrix4 roughPose = motionModel_.predictedPose();

		// first, we determine strong feature points in the current frame

		ocean_assert(imagePoints.empty());
		imagePoints = detectFeatures(yFrame, frameIsUndistorted, pinholeCamera.projectToImage<true>(roughPose, trackerFeatureMap.boundingBox(), false), worker);

		// if we have a current frame with almost no feature points we simply stop here
		if (imagePoints.size() < 10)
		{
			return false;
		}

		if (trackerFeatureMap.recentStrongObjectPointIndices().size() >= 5 && trackerFeatureMap.recentStrongObjectPointIndices().size() + trackerFeatureMap.recentSemiStrongObjectPointIndices().size() >= 20)
		{
			// in the case we have enough strong feature points from the previous frame we first try to determine the camera pose based on these feature points as this would be very simple and would provide a reliable tracking result

			if (determinePoseWithStrongPreviousCorrespondences(roughPose, pinholeCamera, imagePoints, pose, worker))
			{
				return true;
			}
		}

		if (determinePoseWithAnyPreviousCorrespondences(roughPose, pinholeCamera, imagePoints, pose, worker))
		{
			return true;
		}

		// none of the above applied tracking strategies based on known feature correspondences succeeded, so that we now try to determine the current camera pose based on the rough guess from the motion model

		if (determinePoseWithRoughPose(roughPose, pinholeCamera, imagePoints, pose, worker))
		{
			return true;
		}
	}

	const HomogenousMatrix4* initialRoughPose = nullptr;
	if (motionModel_)
	{
		initialRoughPose = &motionModel_.predictedPose();
	}

	if (RMVFeatureDetector::needPyramidInitialization(trackerFeatureMap.initializationDetectorType()))
	{
		if (!determinePoseWithoutKnowledgePyramid(yFrame, trackerFeatureMap.initializationCamera(), pose, worker))
		{
			return false;
		}

		const HomogenousMatrix4 roughPose = pose;

		imagePoints = detectFeatures(yFrame, frameIsUndistorted, pinholeCamera.projectToImage<true>(roughPose, trackerFeatureMap.boundingBox(), false), worker);

		if (!determinePoseWithRoughPose(roughPose, pinholeCamera, imagePoints, pose, worker))
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

		if (!determinePoseWithoutKnowledgeDefault(pinholeCamera, imagePoints, pose, worker))
		{
			return false;
		}
	}

	return true;
}

bool RMVFeatureTracker6DOF::determinePoseWithoutKnowledgePyramid(const Frame& frame, const PinholeCamera& pinholeCamera, HomogenousMatrix4& pose, Worker* worker)
{
	ocean_assert(frame.pixelFormat() == FrameType::FORMAT_Y8);
	ocean_assert(frame.pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT);

	Frame adjustedFrame(frame, Frame::ACM_USE_KEEP_LAYOUT);
	if (frame.width() != trackerPoseProjectionSet.width() || frame.height() != trackerPoseProjectionSet.height())
	{
		ocean_assert(frame.width() > trackerPoseProjectionSet.width() && frame.height() > trackerPoseProjectionSet.height());

		if (!CV::FrameShrinker::downsampleByTwo11(frame, adjustedFrame, worker))
		{
			return false;
		}

		while (adjustedFrame.width() != trackerPoseProjectionSet.width() || adjustedFrame.height() != trackerPoseProjectionSet.height())
		{
			ocean_assert(frame.width() > trackerPoseProjectionSet.width() && frame.height() > trackerPoseProjectionSet.height());

			if (!CV::FrameShrinker::downsampleByTwo11(adjustedFrame, worker))
			{
				return false;
			}
		}
	}

	ocean_assert(adjustedFrame.width() == pinholeCamera.width() && adjustedFrame.height() == pinholeCamera.height());

	if (RMVFeatureDetector::needSmoothedFrame(trackerDetectorType))
	{
		if (!CV::FrameFilterGaussian::filter(adjustedFrame, 3u, worker))
		{
			return false;
		}
	}

	const Vectors3& objectPoints = trackerFeatureMap.initializationObjectPoints();
	unsigned int numberImagePoints = (unsigned int)objectPoints.size() * 70u / 100u;

	const Vectors2 imagePoints(RMVFeatureDetector::detectFeatures(adjustedFrame, trackerDetectorType, 55, numberImagePoints, false, worker));
	numberImagePoints = (unsigned int)imagePoints.size();

	if (imagePoints.size() < 20)
	{
		return false;
	}

	HomogenousMatrices4 roughPosesIF;

	{
		HomogenousMatrices4 roughPoses(8);
		trackerPoseProjectionSet.findPosesWithMinimalError<Geometry::Estimator::ET_TUKEY>(imagePoints.data(), imagePoints.size(), (unsigned int)imagePoints.size() * 75u / 100u, Geometry::Error::ED_APPROXIMATED, roughPoses.size(), roughPoses.data(), nullptr, worker);

		// add default poses, **TODO** check for visibility of the default poses
		roughPoses.insert(roughPoses.begin(), HomogenousMatrix4(Vector3(Scalar(0.0), Scalar(0.25), Scalar(0.0)), Rotation(1, 0, 0, -Numeric::pi_2()) * Rotation(0, 0, 1, Numeric::pi_2())));
		roughPoses.insert(roughPoses.begin(), HomogenousMatrix4(Vector3(Scalar(0.0), Scalar(0.25), Scalar(0.0)), Rotation(1, 0, 0, -Numeric::pi_2()) * Rotation(0, 0, 1, Numeric::pi())));
		roughPoses.insert(roughPoses.begin(), HomogenousMatrix4(Vector3(Scalar(0.0), Scalar(0.25), Scalar(0.0)), Rotation(1, 0, 0, -Numeric::pi_2()) * Rotation(0, 0, 1, Numeric::pi() * Scalar(1.5))));
		roughPoses.insert(roughPoses.begin(), HomogenousMatrix4(Vector3(Scalar(0.0), Scalar(0.25), Scalar(0.0)), Rotation(1, 0, 0, -Numeric::pi_2())));

		roughPosesIF = PinholeCamera::standard2InvertedFlipped(roughPoses.data(), roughPoses.size());
	}

	const Scalar resolutionFactor = cameraResolutionFactor(pinholeCamera);

	const Scalar maxPixelError0 = Numeric::sqr(Scalar(16.0) * resolutionFactor);
	const Scalar maxPixelError1 = Numeric::sqr(Scalar(13.0) * resolutionFactor);
	const Scalar maxPixelError2 = Numeric::sqr(Scalar(9.0) * resolutionFactor);
	const Scalar maxPixelError3 = Numeric::sqr(Scalar(7.0) * resolutionFactor);


	HomogenousMatrix4 resultingPoseIF;
	if (RandomModelVariation::optimizedPoseFromPointCloudsWithSeveralInitialPosesIF<true>(roughPosesIF.data(), roughPosesIF.size(), pinholeCamera, objectPoints.data(), objectPoints.size(), imagePoints.data(), numberImagePoints, numberImagePoints * 65u / 100u, trackerRandomGenerator, resultingPoseIF, Geometry::Error::ED_APPROXIMATED, maxPixelError0, Vector3(Scalar(0.3), Scalar(0.3), Scalar(0.3)), Numeric::deg2rad(30), Scalar(0.01), nullptr, nullptr, worker))
	{
		HomogenousMatrix4 initialPoseIF = resultingPoseIF;
		if (RandomModelVariation::optimizedPoseFromPointCloudsWithOneInitialPoseIF<true>(initialPoseIF, pinholeCamera, objectPoints.data(), objectPoints.size(), imagePoints.data(), numberImagePoints, numberImagePoints * 55u / 100u, trackerRandomGenerator, resultingPoseIF, Geometry::Error::ED_APPROXIMATED, maxPixelError1, Vector3(Scalar(0.3), Scalar(0.3), Scalar(0.3)), Numeric::deg2rad(30), Scalar(0.01), nullptr, nullptr, nullptr, worker))
		{
			initialPoseIF = resultingPoseIF;

			if (RandomModelVariation::optimizedPoseFromPointCloudsWithOneInitialPoseIF<true>(initialPoseIF, pinholeCamera, objectPoints.data(), objectPoints.size(), imagePoints.data(), numberImagePoints, numberImagePoints * 55u / 100u, trackerRandomGenerator, resultingPoseIF, Geometry::Error::ED_AMBIGUOUS, maxPixelError2, Vector3(Scalar(0.25), Scalar(0.25), Scalar(0.25)), Numeric::deg2rad(25), Scalar(0.01), nullptr, nullptr, nullptr, worker))
			{
				initialPoseIF = resultingPoseIF;

				if (RandomModelVariation::optimizedPoseFromPointCloudsWithOneInitialPoseIF<true>(initialPoseIF, pinholeCamera, objectPoints.data(), objectPoints.size(), imagePoints.data(), numberImagePoints, numberImagePoints * 50u / 100u, trackerRandomGenerator, resultingPoseIF, Geometry::Error::ED_UNIQUE, maxPixelError3, Vector3(Scalar(0.2), Scalar(0.2), Scalar(0.2)), Numeric::deg2rad(20), Scalar(0.05), nullptr, nullptr, nullptr, worker))
				{
					pose = PinholeCamera::invertedFlipped2Standard(resultingPoseIF);

					return true;
				}
			}
		}
	}

	return false;
}

bool RMVFeatureTracker6DOF::determinePoseWithoutKnowledgeDefault(const PinholeCamera& pinholeCamera, const Vectors2& imagePoints, HomogenousMatrix4& pose, Worker* worker)
{
	const Vectors3& objectPoints = trackerFeatureMap.initializationObjectPoints();
	unsigned int numberImagePoints = min((unsigned int)objectPoints.size() * 70u / 100u, (unsigned int)imagePoints.size());

	if (imagePoints.size() < 20)
	{
		return false;
	}

	ocean_assert(trackerPoseProjectionSet.width() == pinholeCamera.width() && trackerPoseProjectionSet.height() == pinholeCamera.height());

	// first we guess several suitable start poses due to the set of pre-defined poses
	// however, the accuracy can be very poor

	HomogenousMatrices4 roughPosesIF;

	{
		HomogenousMatrices4 roughPoses(8);
		trackerPoseProjectionSet.findPosesWithMinimalError<Geometry::Estimator::ET_TUKEY>(imagePoints.data(), imagePoints.size(), (unsigned int)imagePoints.size() * 75u / 100u, Geometry::Error::ED_APPROXIMATED, roughPoses.size(), roughPoses.data(), nullptr, worker);

		roughPosesIF = PinholeCamera::standard2InvertedFlipped(roughPoses.data(), roughPoses.size());
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

	HomogenousMatrix4 resultingPoseIF;
	if (RandomModelVariation::optimizedPoseFromPointCloudsWithSeveralInitialPosesIF<true>(roughPosesIF.data(), roughPosesIF.size(), pinholeCamera, objectPoints.data(), objectPoints.size(), imagePoints.data(), numberImagePoints, numberImagePoints * 60u / 100u, trackerRandomGenerator, resultingPoseIF, Geometry::Error::ED_APPROXIMATED, maxPixelError0, Vector3(Scalar(0.1), Scalar(0.1), Scalar(0.1)), Numeric::deg2rad(15), timeout, &resultingError, nullptr, worker))
	{
		// second, we try to optimize the best resulting pose further (with slightly stinger conditions)

		HomogenousMatrix4 roughPoseIF = resultingPoseIF;

		if (RandomModelVariation::optimizedPoseFromPointCloudsWithOneInitialPoseIF<true>(roughPoseIF, pinholeCamera, objectPoints.data(), objectPoints.size(), imagePoints.data(), numberImagePoints, numberImagePoints * 60u / 100u, trackerRandomGenerator, resultingPoseIF, Geometry::Error::ED_APPROXIMATED, maxPixelError1, Vector3(Scalar(0.05), Scalar(0.05), Scalar(0.05)), Numeric::deg2rad(12.5), timeout, nullptr, nullptr, nullptr, worker))
		{
			// third, we try to optimize the pose further (again with stronger conditions) and we determine point correspondences

			roughPoseIF = resultingPoseIF;

			IndexPairs32 correspondences;
			if (RandomModelVariation::optimizedPoseFromPointCloudsWithOneInitialPoseIF<true>(roughPoseIF, pinholeCamera, objectPoints.data(), objectPoints.size(), imagePoints.data(), numberImagePoints, numberImagePoints * 60u / 100u, trackerRandomGenerator, resultingPoseIF, Geometry::Error::ED_AMBIGUOUS, maxPixelError2, Vector3(Scalar(0.02), Scalar(0.02), Scalar(0.02)), Numeric::deg2rad(10), timeout, nullptr, &correspondences, nullptr, worker))
			{
				// now as we found an almost precise pose we simply need to optimize the pose with traditional non-linear optimization approaches

				roughPoseIF = resultingPoseIF;

				Vectors2 validImagePoints;
				Vectors3 validObjectPoints;

				validImagePoints.reserve(correspondences.size());
				validObjectPoints.reserve(correspondences.size());

				for (IndexPairs32::const_iterator i = correspondences.begin(); i != correspondences.end(); ++i)
				{
					ocean_assert(i->first < imagePoints.size());
					ocean_assert(i->second < trackerFeatureMap.initializationObjectPoints().size());

					validObjectPoints.push_back(trackerFeatureMap.initializationObjectPoints()[i->second]);
					validImagePoints.push_back(imagePoints[i->first]);
				}

				if (Geometry::NonLinearOptimizationPose::optimizePoseIF(pinholeCamera, roughPoseIF, ConstArrayAccessor<Vector3>(validObjectPoints), ConstArrayAccessor<Vector2>(validImagePoints), false, resultingPoseIF, 30u, Geometry::Estimator::ET_HUBER, Scalar(0.001), Scalar(10), nullptr, nullptr))
				{
					// now we have a quite good pose based on the gathered points correspondences
					// now we apply a last fine-tuning with more points as we now should be able to find significant more feature correspondences

					roughPoseIF = resultingPoseIF;

					if (refinePoseIF(roughPoseIF, pinholeCamera, imagePoints, trackerFeatureMap.objectPoints(), resultingPoseIF, 200, 400, Scalar(10) * resolutionFactor, Scalar(30 * 30), Numeric::maxValue(), worker) < 5u)
					{
						return false;
					}

					roughPoseIF = resultingPoseIF;

					if (refinePoseIF(roughPoseIF, pinholeCamera, imagePoints, trackerFeatureMap.objectPoints(), resultingPoseIF, 400, 800, Scalar(4) * resolutionFactor, Scalar(2 * 2), Numeric::maxValue(), worker) < 5u)
					{
						return false;
					}

					pose = PinholeCamera::invertedFlipped2Standard(resultingPoseIF);

					return true;
				}
			}
		}
	}

	Log::info() << "Pose determination without knowledge failed, with error: " << resultingError;

	return false;
}

bool RMVFeatureTracker6DOF::determinePoseWithStrongPreviousCorrespondences(const HomogenousMatrix4& roughPose, const PinholeCamera& pinholeCamera, Vectors2& imagePoints, HomogenousMatrix4& pose, Worker* worker)
{
	ocean_assert(roughPose.isValid() && pinholeCamera.isValid());
	ocean_assert(motionModel_);

	ocean_assert(trackerFeatureMap.recentStrongObjectPointIndices().size() >= 5 && imagePoints.size() >= 10);
	if (trackerFeatureMap.recentStrongObjectPointIndices().size() < 5 || imagePoints.size() < 10)
	{
		return false;
	}

	// we try to re-find the previously used feature correspondences (we simply hope that the previous feature point locations do match quite well with the predicted location - and that in these image areas no other possible feature points occur)

	const HomogenousMatrix4 roughPoseIF(PinholeCamera::standard2InvertedFlipped(roughPose));

	HomogenousMatrix4 resultingPoseIF;
	if (!refinePoseWithStrongPreviousCorrespondencesIF(roughPoseIF, pinholeCamera, imagePoints, resultingPoseIF))
	{
		return false;
	}

	const Scalar resolutionFactor = cameraResolutionFactor(pinholeCamera);

	HomogenousMatrix4 poseIF(resultingPoseIF);

	// now we try to improve/refine the pose

	const unsigned int validCorrespondencesFirst = refinePoseIF(poseIF, pinholeCamera, imagePoints, trackerFeatureMap.objectPoints(), resultingPoseIF, 200u, 400u, Scalar(10) * resolutionFactor, Scalar(30 * 30), Numeric::maxValue(), worker);

	if (validCorrespondencesFirst <= 5u)
	{
		return false;
	}

	// and we try to improve/refine the pose again

	poseIF = resultingPoseIF;

	const unsigned int validCorrespondencesSecond = refinePoseIF(poseIF, pinholeCamera, imagePoints, trackerFeatureMap.objectPoints(), resultingPoseIF, 400u, 600u, Scalar(4) * resolutionFactor, Scalar(2 * 2), Numeric::maxValue(), worker);

	if (validCorrespondencesSecond <= 20u)
	{
		return false;
	}

	poseIF = resultingPoseIF;

	// if we could not establish enough valid feature correspondences we give it another try, or if the camera pose is not similar to the rough guess
	if (validCorrespondencesSecond < 150 || roughPoseIF.rotation().angle(resultingPoseIF.rotation()) > Numeric::deg2rad(2.5))
	{
		const unsigned int validCorrespondencesThird = refinePoseIF(poseIF, pinholeCamera, imagePoints, trackerFeatureMap.objectPoints(), resultingPoseIF, 800u, 1000u, Scalar(4) * resolutionFactor, Scalar(1.5 * 1.5), Numeric::maxValue(), worker);

		if (validCorrespondencesThird <= 30u)
		{
			return false;
		}
	}

	startAsynchronousDataProcessingIF(resultingPoseIF, pinholeCamera, std::move(imagePoints));

	pose = PinholeCamera::invertedFlipped2Standard(resultingPoseIF);

	return true;
}

bool RMVFeatureTracker6DOF::determinePoseWithAnyPreviousCorrespondences(const HomogenousMatrix4& roughPose, const PinholeCamera& pinholeCamera, Vectors2& imagePoints, HomogenousMatrix4& pose, Worker* worker)
{
	ocean_assert(roughPose.isValid() && pinholeCamera.isValid());
	ocean_assert(imagePoints.size() >= 10);

	if (imagePoints.size() < 30 || trackerFeatureMap.recentUsedObjectPointIndices().size() < 30)
	{
		return false;
	}

	const unsigned int numberPreviousUsedProjectedObjectPoints = min(50u, (unsigned int)trackerFeatureMap.recentUsedObjectPointIndices().size());
	const unsigned int numberImagePoints = min(100u, (unsigned int)imagePoints.size());

	if (numberPreviousUsedProjectedObjectPoints < 10)
	{
		return false;
	}

	// we take a very simple model (three is the minimal number of point correspondences)
	const unsigned int modelSize = 3u;

	const HomogenousMatrix4 roughPoseIF(PinholeCamera::standard2InvertedFlipped(roughPose));

	// first we extract the subset of object points that has been used in the previous iteration, and we determine their projected image points

	const Vectors3 previousUsedObjectPoints(trackerFeatureMap.recentUsedObjectPoints(numberPreviousUsedProjectedObjectPoints));
	ocean_assert(previousUsedObjectPoints.size() == size_t(numberPreviousUsedProjectedObjectPoints));

	Vectors2 previousUsedProjectedObjectPoints(previousUsedObjectPoints.size());
	pinholeCamera.projectToImageIF<true>(roughPoseIF, previousUsedObjectPoints.data(), previousUsedObjectPoints.size(), false, previousUsedProjectedObjectPoints.data());


	// now we determine a distribution array for the current image points as we want to have all possible feature candidates (out of the projected object points) for each image point within a fixed small radius

	const unsigned int bins = max(1u, min(pinholeCamera.width(), pinholeCamera.height()) / 10u); // each bin will have approx. 10^2 pixels

	const Geometry::SpatialDistribution::DistributionArray distributionImagePoints(Geometry::SpatialDistribution::distributeToArray(imagePoints.data(), numberImagePoints, Scalar(0), Scalar(0), Scalar(pinholeCamera.width()), Scalar(pinholeCamera.height()), bins, bins));

	IndexGroups32 neighborGroups;
	neighborGroups.reserve(numberPreviousUsedProjectedObjectPoints);
	for (Vectors2::const_iterator i = previousUsedProjectedObjectPoints.begin(); i != previousUsedProjectedObjectPoints.end(); ++i)
	{
		neighborGroups.push_back(Geometry::SpatialDistribution::determineNeighbors(*i, imagePoints.data(), numberImagePoints, Scalar(3), distributionImagePoints));
	}

	// now we randomly selected subset of the projected object points and image points and try to determine the best resulting pose, this is a simple RANSAC approach

	Scalar bestTotalError = Numeric::maxValue();
	HomogenousMatrix4 bestPoseIF;

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
			const unsigned int randomUsedObjectPointIndex = RandomI::random(trackerRandomGenerator, numberPreviousUsedProjectedObjectPoints - 1u);
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

				const unsigned int candidateNeighborIndex = RandomI::random(trackerRandomGenerator, (unsigned int)neihgbors.size() - 1u);
				ocean_assert(candidateNeighborIndex < neihgbors.size());

				ocean_assert(neihgbors[candidateNeighborIndex] < imagePoints.size());
				const Vector2& iterationImagePoint = imagePoints[neihgbors[candidateNeighborIndex]];
				ocean_assert_and_suppress_unused(iterationProjectedObjectPoint == pinholeCamera.projectToImageIF<true>(roughPoseIF, iterationObjectPoint, false), iterationProjectedObjectPoint);

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

		HomogenousMatrix4 optimizedRoughPoseIF;
		if (Geometry::NonLinearOptimizationPose::optimizePoseIF(pinholeCamera, roughPoseIF, ConstArrayAccessor<Vector3>(iterationObjectPoints), ConstArrayAccessor<Vector2>(iterationImagePoints), false, optimizedRoughPoseIF, 20u, Geometry::Estimator::ET_SQUARE, Scalar(0.001), Scalar(10)))
		{
			// now we count the number of perfect/good matches

			Vectors2 iterationProjectedObjectPoints(previousUsedObjectPoints.size());
			pinholeCamera.projectToImageIF<true>(optimizedRoughPoseIF, previousUsedObjectPoints.data(), previousUsedObjectPoints.size(), false, iterationProjectedObjectPoints.data());

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
			bestPoseIF = optimizedRoughPoseIF;
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
		Geometry::Error::determinePoseErrorIF<ConstTemplateArraySubsetAccessor<Vector3, Index32>, ConstTemplateArraySubsetAccessor<Vector2, Index32>, true>(bestPoseIF, pinholeCamera, ConstTemplateArraySubsetAccessor<Vector3, Index32>(previousUsedObjectPoints, bestObjectPointIndices), ConstTemplateArraySubsetAccessor<Vector2, Index32>(imagePoints.data(), bestImagePointIndices), false, sqrAverageError, sqrMinimalError, sqrMaximalError);

		ocean_assert(sqrMaximalError <= Scalar(2.5 * 2.5));
	}
#endif

	HomogenousMatrix4 optimizedPoseIF;
	if (!Geometry::NonLinearOptimizationPose::optimizePoseIF(pinholeCamera, bestPoseIF, ConstArraySubsetAccessor<Vector3, Index32>(previousUsedObjectPoints, bestObjectPointIndices), ConstArraySubsetAccessor<Vector2, Index32>(imagePoints.data(), bestImagePointIndices), false, optimizedPoseIF, 30u, Geometry::Estimator::ET_SQUARE))
	{
		return false;
	}

	bestPoseIF = optimizedPoseIF;

	const unsigned int validCorrespondencesFirst = refinePoseIF(bestPoseIF, pinholeCamera, imagePoints, trackerFeatureMap.recentUsedObjectPoints(), optimizedPoseIF, 150u, 100u, Scalar(5), Scalar(3 * 3), Scalar(1.5 * 1.5), worker);

	if (validCorrespondencesFirst < 10u)
	{
		return false;
	}

	bestPoseIF = optimizedPoseIF;

	const unsigned int validCorrespondencesSecond = refinePoseIF(bestPoseIF, pinholeCamera, imagePoints, trackerFeatureMap.objectPoints(), optimizedPoseIF, 400u, 200u, Scalar(5), Scalar(3 * 3), Scalar(1.5 * 1.5), worker);

	if (validCorrespondencesSecond < 10u)
	{
		return false;
	}

	startAsynchronousDataProcessingIF(optimizedPoseIF, pinholeCamera, std::move(imagePoints));

	pose = PinholeCamera::invertedFlipped2Standard(optimizedPoseIF);

	return true;
}

bool RMVFeatureTracker6DOF::determinePoseWithRoughPose(const HomogenousMatrix4& roughPose, const PinholeCamera& pinholeCamera, Vectors2& imagePoints, HomogenousMatrix4& pose, Worker* worker)
{
	ocean_assert(roughPose.isValid() && pinholeCamera.isValid());
	ocean_assert(imagePoints.size() >= 5);

	const Scalar resolutionFactor = cameraResolutionFactor(pinholeCamera);

	const Scalar maxPixelError0 = Numeric::sqr(Scalar(3.1) * resolutionFactor);

	const Vectors3& objectPoints = trackerFeatureMap.objectPoints();

	const unsigned int numberObjectPoints = min(60u, (unsigned int)objectPoints.size());
	const unsigned int numberImagePoints = min(numberObjectPoints * 2u, (unsigned int)imagePoints.size());

	const Scalar objectPointsDiagonal = trackerFeatureMap.boundingBox().diagonal();
	const Scalar maximalTranslationSmall = objectPointsDiagonal * Scalar(0.05);

#ifdef OCEAN_DEBUG
	const double timeout = 10.0;
#else
	const double timeout = 0.05;
#endif

	const HomogenousMatrix4 roughPoseIF(PinholeCamera::standard2InvertedFlipped(roughPose));

	HomogenousMatrix4 resultingPoseIF;
	IndexPairs32 correspondences;

	if (numberImagePoints < numberObjectPoints)
	{
		if (!RandomModelVariation::optimizedPoseFromPointCloudsWithOneInitialPoseIF<true>(roughPoseIF, pinholeCamera, objectPoints.data(), numberObjectPoints, imagePoints.data(), numberImagePoints, numberImagePoints * 60u / 100u, trackerRandomGenerator, resultingPoseIF, Geometry::Error::ED_APPROXIMATED, maxPixelError0, Vector3(maximalTranslationSmall, maximalTranslationSmall, maximalTranslationSmall), Numeric::deg2rad(5), timeout, nullptr, &correspondences, nullptr, worker))
		{
			ocean_assert(correspondences.empty());

			// we give it a second chance with slightly simpler conditions
			if (!RandomModelVariation::optimizedPoseFromPointCloudsWithOneInitialPoseIF<true>(roughPoseIF, pinholeCamera, objectPoints.data(), numberObjectPoints, imagePoints.data(), numberImagePoints, numberImagePoints * 60u / 100u, trackerRandomGenerator, resultingPoseIF, Geometry::Error::ED_APPROXIMATED, maxPixelError0, Vector3(maximalTranslationSmall, maximalTranslationSmall, maximalTranslationSmall) * Scalar(2), Numeric::deg2rad(10), timeout * 2.0, nullptr, &correspondences, nullptr, worker))
			{
				return false;
			}
		}
	}
	else
	{
		if (!RandomModelVariation::optimizedPoseFromPointCloudsWithOneInitialPoseIF<false>(roughPoseIF, pinholeCamera, objectPoints.data(), numberObjectPoints, imagePoints.data(), numberImagePoints, numberObjectPoints * 60u / 100u, trackerRandomGenerator, resultingPoseIF, Geometry::Error::ED_APPROXIMATED, maxPixelError0, Vector3(maximalTranslationSmall, maximalTranslationSmall, maximalTranslationSmall), Numeric::deg2rad(5), timeout, nullptr, &correspondences, nullptr, worker))
		{
			ocean_assert(correspondences.empty());

			// we give it a second chance with slightly simpler conditions
			if (!RandomModelVariation::optimizedPoseFromPointCloudsWithOneInitialPoseIF<false>(roughPoseIF, pinholeCamera, objectPoints.data(), numberObjectPoints, imagePoints.data(), numberImagePoints, numberObjectPoints * 60u / 100u, trackerRandomGenerator, resultingPoseIF, Geometry::Error::ED_APPROXIMATED, maxPixelError0, Vector3(maximalTranslationSmall, maximalTranslationSmall, maximalTranslationSmall) * Scalar(2), Numeric::deg2rad(10), timeout * 2.0, nullptr, &correspondences, nullptr, worker))
			{
				return false;
			}
		}
	}

	const HomogenousMatrix4 initialPoseIF(resultingPoseIF);

	// we could improve the rough pose and now we also can use the guessed point correspondences to apply a non-linear pose optimization

	Vectors3 validObjectPoints;
	validObjectPoints.reserve(correspondences.size());
	Vectors2 validImagePoints;
	validImagePoints.reserve(correspondences.size());

	for (IndexPairs32::const_iterator i = correspondences.begin(); i != correspondences.end(); ++i)
	{
		ocean_assert(i->first < imagePoints.size());
		ocean_assert(i->second < trackerFeatureMap.objectPoints().size());

		validObjectPoints.push_back(trackerFeatureMap.objectPoints()[i->first]);
		validImagePoints.push_back(imagePoints[i->second]);
	}

	ocean_assert(!validImagePoints.empty());

	if (validImagePoints.size() < 5)
	{
		return false;
	}

	// first we apply the Huber estimator as we still do not know how good the correspondences are

	if (!Geometry::NonLinearOptimizationPose::optimizePoseIF(pinholeCamera, initialPoseIF, ConstArrayAccessor<Vector3>(validObjectPoints), ConstArrayAccessor<Vector2>(validImagePoints), false, resultingPoseIF, 30u, Geometry::Estimator::ET_HUBER, Scalar(0.001), Scalar(10), nullptr, nullptr))
	{
		return false;
	}

	HomogenousMatrix4 poseIF(resultingPoseIF);

	// now we apply the Tukey estimator to filter outliers

	if (!Geometry::NonLinearOptimizationPose::optimizePoseIF(pinholeCamera, poseIF, ConstArrayAccessor<Vector3>(validObjectPoints), ConstArrayAccessor<Vector2>(validImagePoints), false, resultingPoseIF, 30u, Geometry::Estimator::ET_TUKEY, Scalar(0.001), Scalar(10), nullptr, nullptr))
	{
		return false;
	}

	poseIF = resultingPoseIF;

	// finally we refine the current pose based on the entire set of image and object points, first with a small amount of point correspondences and week uniqueness constraints

	if (refinePoseIF(poseIF, pinholeCamera, imagePoints, trackerFeatureMap.objectPoints(), resultingPoseIF, 200u, 400u, Scalar(10) * resolutionFactor, Scalar(30 * 30), Numeric::maxValue(), worker) < 10u) // **TOOD** the resulting number of correspondences should be rated wrt. the covered area (e.g., 10 points is too low if the entire pattern is visible)
	{
		return false;
	}

	poseIF = resultingPoseIF;

	// now we take a larger amount of point correspondences and we apply strong uniqueness constraints

	if (refinePoseIF(poseIF, pinholeCamera, imagePoints, trackerFeatureMap.objectPoints(), resultingPoseIF, 400u, 800u, Scalar(4) * resolutionFactor, Scalar(2 * 2), Numeric::maxValue(), worker) < 10u)
	{
		return false;
	}

	startAsynchronousDataProcessingIF(resultingPoseIF, pinholeCamera, std::move(imagePoints));

	pose = PinholeCamera::invertedFlipped2Standard(resultingPoseIF);

	return true;
}

bool RMVFeatureTracker6DOF::refinePoseWithStrongPreviousCorrespondencesIF(const HomogenousMatrix4& roughPoseIF, const PinholeCamera& pinholeCamera, const Vectors2& imagePoints, HomogenousMatrix4& poseIF)
{
	ocean_assert(roughPoseIF.isValid() && pinholeCamera.isValid() && imagePoints.size() >= 10);

	if (imagePoints.size() < 10)
	{
		return false;
	}

	const Scalar resolutionFactor = cameraResolutionFactor(pinholeCamera);

	// first we try to identify/guess the current image points that correspond to the strong 2D/3D correspondences of the previous frame

	Vectors3 candidateObjectPoints;
	Vectors2 candidateImagePoints;

	addUniqueCorrespondencesIF(roughPoseIF, pinholeCamera, imagePoints.data(), min(imagePoints.size(), size_t(200)), ConstArraySubsetAccessor<Vector3, Index32>(trackerFeatureMap.objectPoints(), trackerFeatureMap.recentStrongObjectPointIndices()), trackerStrongCorrespondencesEmptyAreaRadius * resolutionFactor, Scalar(4 * 4), candidateObjectPoints, candidateImagePoints);
	ocean_assert(candidateObjectPoints.size() == candidateImagePoints.size());

	if (candidateObjectPoints.size() <= 10 && !trackerFeatureMap.recentSemiStrongObjectPointIndices().empty())
	{
		// if we could not find enough strong correspondences so that we now weaken the conditions - so that we add more correspondences with more generous properties

		addUniqueCorrespondencesIF(roughPoseIF, pinholeCamera, imagePoints.data(), min(imagePoints.size(), size_t(300)), ConstArraySubsetAccessor<Vector3, Index32>(trackerFeatureMap.objectPoints(), trackerFeatureMap.recentSemiStrongObjectPointIndices()), trackerSemiStrongCorrespondencesEmptyAreaRadius * resolutionFactor, Scalar(2.5 * 2.5), candidateObjectPoints, candidateImagePoints);
		ocean_assert(candidateObjectPoints.size() == candidateImagePoints.size());
	}

	if (candidateObjectPoints.size() < 5)
	{
		return false;
	}

	// we could find/guess enough correspondences so that we now improve the rough pose based on a non-linear optimization, first we a Huber estimator (as we still do not know how good the quality of the correspondences is)

	HomogenousMatrix4 optimizedPoseIF;
	if (!Geometry::NonLinearOptimizationPose::optimizePoseIF(pinholeCamera, roughPoseIF, ConstArrayAccessor<Vector3>(candidateObjectPoints), ConstArrayAccessor<Vector2>(candidateImagePoints), false, optimizedPoseIF, 10u, Geometry::Estimator::ET_HUBER))
	{
		return false;
	}

	// and now we apply a Tukey estimator to remove outliers

	if (!Geometry::NonLinearOptimizationPose::optimizePoseIF(pinholeCamera, optimizedPoseIF, ConstArrayAccessor<Vector3>(candidateObjectPoints), ConstArrayAccessor<Vector2>(candidateImagePoints), false, poseIF, 5u, Geometry::Estimator::ET_TUKEY))
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
	if (RMVFeatureDetector::needSmoothedFrame(trackerDetectorType))
	{
		CV::FrameFilterGaussian::filter(yFrame, adjustedFrame, 3u, worker);
	}

	const Vectors2 imagePoints(RMVFeatureDetector::detectFeatures(adjustedFrame, boundingBox, trackerDetectorType, trackerFeatureDetectorStrength, frameIsUndistorted, worker));

	if (imagePoints.size() < featureDetectorIdealNumber)
	{
		trackerFeatureDetectorStrength = max(featureDetectorStrengthMin, trackerFeatureDetectorStrength - featureDetectorStrengthDelta);
	}
	else
	{
		trackerFeatureDetectorStrength = min(trackerFeatureDetectorStrength + featureDetectorStrengthDelta, featureDetectorStrengthMax);
	}

	return imagePoints;
}

bool RMVFeatureTracker6DOF::asynchronousDataProcessed()
{
	const ScopedLock scopedLock(trackerAsynchronousDataProcessingLock);
	return trackerAsynchronousDataProcessingActive == false;
}

void RMVFeatureTracker6DOF::startAsynchronousDataProcessingIF(const HomogenousMatrix4& poseIF, const PinholeCamera& pinholeCamera, Vectors2&& imagePoints)
{
	const ScopedLock scopedLock(trackerAsynchronousDataProcessingLock);

	ocean_assert(trackerAsynchronousDataProcessingPoseIF.isNull());
	trackerAsynchronousDataProcessingPoseIF = poseIF;

	ocean_assert(!trackerAsynchronousDataProcessingCamera.isValid());
	trackerAsynchronousDataProcessingCamera = pinholeCamera;

	ocean_assert(trackerAsynchronousDataProcessingImagePoints.empty());
	trackerAsynchronousDataProcessingImagePoints = std::move(imagePoints);

	trackerAsynchronousSignal.pulse();
}

void RMVFeatureTracker6DOF::threadRun()
{
	while (!shouldThreadStop())
	{
		trackerAsynchronousSignal.wait();

		if (shouldThreadStop())
		{
			break;
		}

		const ScopedLock scopedLock(trackerAsynchronousDataProcessingLock);

		ocean_assert(!trackerAsynchronousDataProcessingActive);
		const ScopedValueT<bool> scopedValue(trackerAsynchronousDataProcessingActive, false, true);

		ocean_assert(trackerAsynchronousDataProcessingPoseIF.isValid());

		Indices32 strongObjectPointIndices, moderateObjectPointIndices, usedObjectPointIndices;
		determineUsedFeaturesIF(trackerAsynchronousDataProcessingPoseIF, trackerAsynchronousDataProcessingCamera, trackerAsynchronousDataProcessingImagePoints, 30, strongObjectPointIndices, moderateObjectPointIndices, usedObjectPointIndices);

		trackerFeatureMap.setMostRecentObjectPointIndices(std::move(strongObjectPointIndices), std::move(moderateObjectPointIndices), std::move(usedObjectPointIndices));

		trackerAsynchronousDataProcessingPoseIF.toNull();
		trackerAsynchronousDataProcessingCamera = PinholeCamera();
		trackerAsynchronousDataProcessingImagePoints.clear();
	}
}

bool RMVFeatureTracker6DOF::determineUsedFeaturesIF(const HomogenousMatrix4& finePoseIF, const PinholeCamera& pinholeCamera, const Vectors2& imagePoints, const size_t minimalStrongObjectPoints, Indices32& strongObjectPointIndices, Indices32& moderateObjectPointIndices, Indices32& usedObjectPointIndices)
{
	ocean_assert(finePoseIF.isValid() && pinholeCamera.isValid());
	ocean_assert(strongObjectPointIndices.empty() && moderateObjectPointIndices.empty() && usedObjectPointIndices.empty());
	ocean_assert(minimalStrongObjectPoints >= 1);

	if (trackerFeatureMap.objectPoints().empty() || imagePoints.empty())
	{
		return false;
	}

	const Scalar resolutionFactor = cameraResolutionFactor(pinholeCamera);

	Vectors2 projectedObjectPoints(trackerFeatureMap.objectPoints().size());
	pinholeCamera.projectToImageIF<true>(finePoseIF, trackerFeatureMap.objectPoints().data(), projectedObjectPoints.size(), false, projectedObjectPoints.data());

	const size_t maximalImagePoints = min(size_t(200), imagePoints.size());
	const size_t maximalObjectPoints = min(size_t(400), trackerFeatureMap.objectPoints().size());

	// now we seek image features with large distance to neighboring image features

	const unsigned int bins = max(1u, min(pinholeCamera.width(), pinholeCamera.height()) / 20u); // each bin will have approx. 20^2 pixels

	const Geometry::SpatialDistribution::DistributionArray distributionProjectedObjectPoints(Geometry::SpatialDistribution::distributeToArray(projectedObjectPoints.data(), projectedObjectPoints.size(), Scalar(0), Scalar(0), Scalar(pinholeCamera.width()), Scalar(pinholeCamera.height()), bins, bins));
	const Geometry::SpatialDistribution::DistributionArray distributionImagePoints(Geometry::SpatialDistribution::distributeToArray(imagePoints.data(), maximalImagePoints, Scalar(0), Scalar(0), Scalar(pinholeCamera.width()), Scalar(pinholeCamera.height()), bins, bins));

	// first we seek for strong feature points (unique and accurate) and no other image points in the neighborhood

	const PointCorrespondences::RedundantCorrespondences strongRedundantCorrespondences(PointCorrespondences::determineNearestCandidates(imagePoints.data(), maximalImagePoints, projectedObjectPoints.data(), maximalObjectPoints, Scalar(10) * resolutionFactor, distributionProjectedObjectPoints));
	for (PointCorrespondences::RedundantCorrespondences::const_iterator i = strongRedundantCorrespondences.begin(); i != strongRedundantCorrespondences.end(); ++i)
	{
		if (i->isUniqueAndAccurate(Scalar(10 * 10), Numeric::sqr(2 * resolutionFactor)))
		{
			ocean_assert(maximalImagePoints >= strongRedundantCorrespondences.size());

			if (Geometry::SpatialDistribution::determineMinimalSqrDistance(imagePoints.data(), strongRedundantCorrespondences.size(), i->index(), distributionImagePoints) > Numeric::sqr(trackerStrongCorrespondencesEmptyAreaRadius * resolutionFactor))
			{
				// the indices of the strong object points
				strongObjectPointIndices.push_back(i->candidateIndex());
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

		for (PointCorrespondences::RedundantCorrespondences::const_iterator i = strongRedundantCorrespondences.begin(); i != strongRedundantCorrespondences.end(); ++i)
		{
			if (strongIndexSet.find(i->candidateIndex()) == strongIndexSet.end() && i->isUniqueAndAccurate(5 * 5, Numeric::sqr(4 * resolutionFactor)))
			{
				ocean_assert(maximalImagePoints >= strongRedundantCorrespondences.size());

				if (Geometry::SpatialDistribution::determineMinimalSqrDistance(imagePoints.data(), strongRedundantCorrespondences.size(), i->index(), distributionImagePoints) > Numeric::sqr(trackerSemiStrongCorrespondencesEmptyAreaRadius * resolutionFactor))
					moderateObjectPointIndices.push_back(i->candidateIndex());
			}
		}
	}

	ocean_assert(IndexSet32(moderateObjectPointIndices.begin(), moderateObjectPointIndices.end()).size() == moderateObjectPointIndices.size());
	std::sort(moderateObjectPointIndices.begin(), moderateObjectPointIndices.end());

	// now we seek any object points that is unique (may contain strong and moderate object points)

	IndexSet32 objectPointIndexSet;
	const PointCorrespondences::RedundantCorrespondences weakRedundantCorrespondences(PointCorrespondences::determineNearestCandidates(imagePoints.data(), min(size_t(400), imagePoints.size()), projectedObjectPoints.data(), min(size_t(800), projectedObjectPoints.size()), 4 * resolutionFactor, distributionProjectedObjectPoints));
	for (PointCorrespondences::RedundantCorrespondences::const_iterator i = weakRedundantCorrespondences.begin(); i != weakRedundantCorrespondences.end(); ++i)
	{
		if (i->isUnique(2 * 2))
		{
			ocean_assert(i->candidateIndex() < trackerFeatureMap.objectPoints().size());

			// although an object point should not be unique for more than one image point, some object points may occur more than once, therefore we use a set
			objectPointIndexSet.insert(i->candidateIndex());
		}
	}

	usedObjectPointIndices = Indices32(objectPointIndexSet.begin(), objectPointIndexSet.end());

	return true;
}

void RMVFeatureTracker6DOF::addUniqueCorrespondencesIF(const HomogenousMatrix4& roughPoseIF, const PinholeCamera& pinholeCamera, const Vector2* imagePoints, const size_t numberImagePoints, const ConstIndexedAccessor<Vector3>& objectPointAccessor, const Scalar searchWindow, const Scalar uniquenessSqrFactor, Vectors3& resultingObjectPoints, Vectors2& resultingImagePoints)
{
	ocean_assert(roughPoseIF.isValid() && pinholeCamera.isValid());
	ocean_assert(imagePoints && numberImagePoints != 0 && objectPointAccessor.size() != 0);

	ocean_assert(searchWindow > 0 && uniquenessSqrFactor > 0);
	ocean_assert(resultingObjectPoints.size() == resultingImagePoints.size());

	Vectors2 projectedObjectPoints;
	projectedObjectPoints.reserve(objectPointAccessor.size());

	for (size_t n = 0; n < objectPointAccessor.size(); ++n)
	{
		projectedObjectPoints.push_back(pinholeCamera.projectToImageIF<true>(roughPoseIF, objectPointAccessor[n], false));
	}

	const PointCorrespondences::RedundantCorrespondences redundantCorrespondences(PointCorrespondences::determineNearestCandidates(projectedObjectPoints.data(), projectedObjectPoints.size(), imagePoints, numberImagePoints, searchWindow));

	ocean_assert(resultingObjectPoints.size() == resultingImagePoints.size());
	resultingObjectPoints.reserve(resultingObjectPoints.size() + redundantCorrespondences.size());
	resultingImagePoints.reserve(resultingImagePoints.size() + redundantCorrespondences.size());

	for (PointCorrespondences::RedundantCorrespondences::const_iterator i = redundantCorrespondences.begin(); i != redundantCorrespondences.end(); ++i)
	{
		if (i->isUnique(uniquenessSqrFactor))
		{
			ocean_assert(i->index() < objectPointAccessor.size());
			ocean_assert(i->candidateIndex() < numberImagePoints);

			const Vector3& objectPoint = objectPointAccessor[i->index()];

			if (PinholeCamera::isObjectPointInFrontIF(roughPoseIF, objectPoint))
			{
				resultingObjectPoints.push_back(objectPoint);
				resultingImagePoints.push_back(imagePoints[i->candidateIndex()]);
			}
		}
	}
}

unsigned int RMVFeatureTracker6DOF::refinePoseIF(const HomogenousMatrix4& roughPoseIF, const PinholeCamera& pinholeCamera, const Vectors2& imagePoints, const Vectors3& objectPoints, HomogenousMatrix4& poseIF, const unsigned int useNumberImagePoints, const unsigned int useNumberObjectPoints, const Scalar searchWindow, const Scalar uniquenessSqrFactor, const Scalar maxSqrDistance, Worker* /*worker*/)
{
	ocean_assert(roughPoseIF.isValid() && pinholeCamera.isValid());
	ocean_assert(imagePoints.size() >= 10 && objectPoints.size() >= 10);

	const unsigned int numberImagePoints = min(useNumberImagePoints, (unsigned int)imagePoints.size());
	const unsigned int numberObjectPoints = min(useNumberObjectPoints, (unsigned int)objectPoints.size());

	ocean_assert(numberImagePoints >= 10u && numberObjectPoints >= 10u);
	if (numberImagePoints < 10u || numberObjectPoints < 10u)
	{
		return 0u;
	}

	Vectors2 projectedObjectPoints(numberObjectPoints);
	pinholeCamera.projectToImageIF<true>(roughPoseIF, objectPoints.data(), numberObjectPoints, false, projectedObjectPoints.data());

	// we determine possible correspondence candidates (and also store a second candidate for each point)
	const PointCorrespondences::RedundantCorrespondences redundantCorrespondences(PointCorrespondences::determineNearestCandidates(imagePoints.data(), numberImagePoints, projectedObjectPoints.data(), numberObjectPoints, pinholeCamera.width(), pinholeCamera.height(), searchWindow));

	Vectors2 correspondenceImagePoints;
	correspondenceImagePoints.reserve(redundantCorrespondences.size());

	Vectors3 correspondenceObjectPoints;
	correspondenceObjectPoints.reserve(redundantCorrespondences.size());

	for (PointCorrespondences::RedundantCorrespondences::const_iterator i = redundantCorrespondences.begin(); i != redundantCorrespondences.end(); ++i)
	{
		if (i->isUniqueAndAccurate(uniquenessSqrFactor, maxSqrDistance)) // in the case maxSqrDistance == Numeric::maxValue() we have the simple isUnique() check
		{
			ocean_assert(i->index() < imagePoints.size());
			ocean_assert(i->candidateIndex() < objectPoints.size());

			correspondenceImagePoints.push_back(imagePoints[i->index()]);
			correspondenceObjectPoints.push_back(objectPoints[i->candidateIndex()]);
		}
	}

	if (correspondenceImagePoints.size() < 5u)
	{
		return 0u;
	}

	ocean_assert(correspondenceImagePoints.size() == correspondenceObjectPoints.size());

	if (!Geometry::NonLinearOptimizationPose::optimizePoseIF(pinholeCamera, roughPoseIF, ConstArrayAccessor<Vector3>(correspondenceObjectPoints), ConstArrayAccessor<Vector2>(correspondenceImagePoints), false, poseIF, 5u, Geometry::Estimator::ET_HUBER, Scalar(0.001), Scalar(10)))
	{
		return 0u;
	}

	const HomogenousMatrix4 optimizedPoseIF = poseIF;

	if (!Geometry::NonLinearOptimizationPose::optimizePoseIF(pinholeCamera, optimizedPoseIF, ConstArrayAccessor<Vector3>(correspondenceObjectPoints), ConstArrayAccessor<Vector2>(correspondenceImagePoints), false, poseIF, 5u, Geometry::Estimator::ET_TUKEY, Scalar(0.001), Scalar(10)))
	{
		return 0u;
	}

	ocean_assert(correspondenceImagePoints.size() >= 5);
	return (unsigned int)correspondenceImagePoints.size();
}

}

}

}
