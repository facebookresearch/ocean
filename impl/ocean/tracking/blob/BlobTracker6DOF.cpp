// Copyright (c) Meta Platforms, Inc. and affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "ocean/tracking/blob/BlobTracker6DOF.h"
#include "ocean/tracking/blob/UnidirectionalCorrespondences.h"

#include "ocean/cv/detector/blob/BlobFeatureDescriptor.h"
#include "ocean/cv/detector/blob/BlobFeatureDetector.h"

#include "ocean/base/String.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameShrinker.h"
#include "ocean/cv/IntegralImage.h"

#include "ocean/geometry/Error.h"
#include "ocean/geometry/NonLinearOptimizationPose.h"
#include "ocean/geometry/PerspectivePose.h"
#include "ocean/geometry/RANSAC.h"
#include "ocean/geometry/SpatialDistribution.h"

#include "ocean/math/Quaternion.h"

#include <algorithm>

namespace Ocean
{

namespace Tracking
{

namespace Blob
{

using namespace CV;
using namespace CV::Detector;
using namespace CV::Detector::Blob;

BlobTracker6DOF::BlobTracker6DOF(const bool realtimePerformance) :
	realtimePerformance_(realtimePerformance)
{
	// nothing to do here
}

BlobTracker6DOF::~BlobTracker6DOF()
{
	// nothing to do here
}

void BlobTracker6DOF::setFeatureMap(const FeatureMap& featureMap, const bool autoUpdateMaxPositionOffset)
{
	const ScopedLock scopedLock(lock_);

	featureMap_ = featureMap;
	correspondenceManager_.clear();

	if (autoUpdateMaxPositionOffset)
	{
		// updates the maximal position offset between two frames to a fifth of the feature map bounding box diagonal
		const Scalar diagonal = (featureMap_.boundingBox().higher() - featureMap_.boundingBox().lower()).length() * Scalar(0.2);
		maxPositionOffset_ = Vector3(diagonal, diagonal, diagonal);
	}
}

bool BlobTracker6DOF::determinePoses(const Frame& frame, const PinholeCamera& pinholeCamera, const bool frameIsUndistorted, TransformationSamples& transformations, const Quaternion& /*world_R_camera*/, Worker* worker)
{
	ocean_assert(frame.isValid() && pinholeCamera.isValid());
	ocean_assert(frame.width() == pinholeCamera.width() && frame.height() == pinholeCamera.height());

	if (!frame.isValid() || frame.width() != pinholeCamera.width() || frame.height() != pinholeCamera.height())
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	if (featureMap_.isNull())
	{
		return false;
	}

	ocean_assert(transformations.empty());
	transformations.clear();

	if (!frame.isValid())
	{
		return false;
	}

	ocean_assert(!realtimePerformance_ || frame.timestamp().isValid());

	if (realtimePerformance_ && recentTimestamp_.isValid() && frame.timestamp() <= recentTimestamp_)
	{
		transformations.push_back(TransformationSample(recentPose_, 0));
		Log::info() << "Pose already detected for the provided frame timestamp.";

		return true;
	}

	if (motionModel_)
	{
		recentPose_ = motionModel_.predictedPose();
	}

	const uint32_t* const integralImage = createIntegralImage(frame, worker);
	ocean_assert(integralImage);

	HomogenousMatrix4 pose;
	const bool trackerResult = determinePose(frame, integralImage, pinholeCamera, frameIsUndistorted, pose, worker);

	if (numberDetectedFeatures_ < usedFeatures_)
	{
		featureStrengthThreshold_ = max(featureStrengthThreshold_ * featureStrengthThresholdDecreaseFactor_, minimalFeatureStrengthThreshold_);
	}
	else if (numberDetectedFeatures_ > usedFeatures_)
	{
		featureStrengthThreshold_ = min(featureStrengthThreshold_ * featureStrengthThresholdIncreaseFactor_, maximalFeatureStrengthThreshold_);
	}

	if (trackerResult)
	{
		motionModel_.update(pose);
		transformations.push_back(TransformationSample(pose, 0));
	}

	return true;
}

bool BlobTracker6DOF::setUsedFeatures(const unsigned int minimal, const unsigned int maximal)
{
	if (minimal <= 10u || minimal > maximal)
		return false;

	const ScopedLock scopedLock(lock_);

	minimalUsedFeatures_ = minimal;
	maximalUsedFeatures_ = maximal;

	usedFeatures_ = minmax(minimal, usedFeatures_, maximal);

	return true;
}

void BlobTracker6DOF::reset()
{
	correspondenceManager_.clear();
	motionModel_.reset();
}

bool BlobTracker6DOF::determinePoses(const Frames& frames, const SharedAnyCameras& anyCameras, TransformationSamples& transformations, const Quaternion& world_R_camera, Worker* worker)
{
	return VisualTracker::determinePoses(frames, anyCameras, transformations, world_R_camera, worker);
}

bool BlobTracker6DOF::determinePose(const Frame& yFrame, const AnyCamera& camera, HomogenousMatrix4& model_T_camera, CV::Detector::Blob::BlobFeatures& modelFeatures, const size_t minimalCorrespondences, size_t* correspondences, Worker* worker, const Scalar faultyRate, Vectors3* usedObjectPoints, Vectors2* usedImagePoints, const HomogenousMatrix4& model_T_roughCamera)
{
	if (!yFrame.isValid() || !yFrame.isPixelFormatCompatible(FrameType::FORMAT_Y8) || !camera.isValid())
	{
		ocean_assert(false && "Invalid input frame!");
		return false;
	}

	ocean_assert(yFrame.width() == camera.width());
	ocean_assert(yFrame.height() == camera.height());

	if (minimalCorrespondences > modelFeatures.size())
	{
		ocean_assert(false && "Not enough features in feature map!");
		return false;
	}

	ocean_assert(faultyRate >= 0 && faultyRate < 1);

	Frame linedIntegralImage(FrameType(yFrame.width() + 1u, yFrame.height() + 1u, FrameType::FORMAT_Y32, FrameType::ORIGIN_UPPER_LEFT));
	CV::IntegralImage::createLinedImage<uint8_t, uint32_t, 1u>(yFrame.constdata<uint8_t>(), linedIntegralImage.data<uint32_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), linedIntegralImage.paddingElements());

	BlobFeatures liveFeatures;
	const unsigned int detectedLiveFeatures = BlobFeatureDetector::detectFeatures(linedIntegralImage.constdata<uint32_t>(), yFrame.width(), yFrame.height(), BlobFeatureDetector::SAMPLING_NORMAL, Scalar(10), false, liveFeatures, worker);

	BlobFeatureDescriptor::calculateOrientationsAndDescriptors(linedIntegralImage.constdata<uint32_t>(), yFrame.width(), yFrame.height(), FrameType::ORIGIN_UPPER_LEFT, BlobFeature::ORIENTATION_SLIDING_WINDOW, liveFeatures, detectedLiveFeatures, true, worker);

	constexpr Scalar thrshold = Scalar(0.1);

	Correspondences::CorrespondencePairs correspondenceCandidates;

	if (model_T_roughCamera.isValid())
	{
		const Scalar maxSqrProjectionError = Numeric::sqr(Scalar(std::max(camera.width(), camera.height())) * Scalar(0.05)); // 5% of camera resolution

		correspondenceCandidates = Tracking::Blob::Correspondences::determineFeatureCorrespondencesGuided(camera, model_T_roughCamera, liveFeatures.data(), modelFeatures.data(), liveFeatures.size(), modelFeatures.size(), thrshold, maxSqrProjectionError, true, worker);
	}
	else
	{
		correspondenceCandidates = Tracking::Blob::Correspondences::determineFeatureCorrespondencesEarlyReject(liveFeatures, modelFeatures, liveFeatures.size(), thrshold, worker);
	}

	if (correspondenceCandidates.size() < minimalCorrespondences)
	{
		return false;
	}

	Vectors2 imagePoints;
	Vectors3 objectPoints;
	Correspondences::extractCorrespondingPoints(liveFeatures, modelFeatures, correspondenceCandidates, imagePoints, objectPoints);
	ocean_assert(objectPoints.size() == imagePoints.size());

	RandomGenerator randomGenerator;

	const unsigned int ransacIterations = Geometry::RANSAC::iterations(3u, Scalar(0.99), faultyRate);
	const Scalar sqrPixelError(5 * 5);

	HomogenousMatrix4 internal_model_T_camera(false);

	Indices32 validCorrespondences;
	if (!Geometry::RANSAC::p3p(camera, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), randomGenerator, internal_model_T_camera, (unsigned int)(minimalCorrespondences), true, ransacIterations, sqrPixelError, &validCorrespondences))
	{
		return false;
	}

	if (validCorrespondences.size() < minimalCorrespondences)
	{
		return false;
	}

	if (usedObjectPoints)
	{
		*usedObjectPoints = Subset::subset(objectPoints, validCorrespondences);
	}

	if (usedImagePoints)
	{
		*usedImagePoints = Subset::subset(imagePoints, validCorrespondences);
	}

	model_T_camera = internal_model_T_camera;

	if (correspondences)
	{
		*correspondences = validCorrespondences.size();
	}

	return true;
}

bool BlobTracker6DOF::determinePose(const Frame& frame, const uint32_t* integralImage, const PinholeCamera& pinholeCamera, const bool frameIsUndistorted, HomogenousMatrix4& pose, Worker* worker)
{
	ocean_assert(frame && integralImage != nullptr);
	ocean_assert(frame.width() == pinholeCamera.width() && frame.height() == pinholeCamera.height());

	BlobFeatures features;

	// use the number of features tracking in the previous frame to reserve memory for new features
	features.reserve(numberDetectedFeatures_);

	if (correspondenceManager_ || recentTimestamp_.isValid())
	{
		// a previous pose must be known from the previous frame
		// thus the area in which feature points will be detected can be reduced

		int left, top;
		unsigned int width, height, leftClamped, topClamped, widthClamped, heightClamped;
		if (!featureMap_.projectToImagePlane(recentPose_, pinholeCamera, 10, left, top, width, height, leftClamped, topClamped, widthClamped, heightClamped))
		{
			return false;
		}

		const Box2 featureMapBoundingBox(Scalar(leftClamped), Scalar(topClamped), Scalar(leftClamped + widthClamped), Scalar(topClamped + heightClamped));

		const Scalar size = Scalar(width * height) / Scalar(frame.width() * frame.height());
		BlobFeatureDetector::SamplingDense samplingDense(BlobFeatureDetector::SAMPLING_SPARSE);

		if (size < Scalar(0.5))
		{
			samplingDense = BlobFeatureDetector::SAMPLING_DENSE;
		}
		else if (size < Scalar(1.5))
		{
			samplingDense = BlobFeatureDetector::SAMPLING_NORMAL;
		}

		numberDetectedFeatures_ = BlobFeatureDetector::detectFeatures(integralImage, frame.width(), frame.height(), leftClamped, topClamped, widthClamped, heightClamped, samplingDense, featureStrengthThreshold_, frameIsUndistorted, features, worker);
		std::sort(features.begin(), features.end());

		// if known 2D/3D feature correspondences are known from the previous frame and if the tracker is in real-time mode
		if (realtimePerformance_ && correspondenceManager_)
		{
			if (determinePoseWithPreviousCorrespondences(features, frame, integralImage, pinholeCamera, featureMapBoundingBox, pose, worker))
			{
				return true;
			}
		}

		// if a pose from the previous frame is known
		if (!recentTimestamp_.isInvalid())
		{
			if (determinePoseWithPreviousPose(features, frame, integralImage, pinholeCamera, frameIsUndistorted, pose, worker))
			{
				return true;
			}
		}

		recentTimestamp_.toInvalid();

		// remove orientation and description so that the tracking iteration without knowledge can use the already detected features
		for (BlobFeatures::iterator i = features.begin(); i != features.end(); ++i)
		{
			i->setType(BlobFeature::DESCRIPTOR_UNDESCRIBED, BlobFeature::ORIENTATION_NOT_ORIENTED);
		}
	}
	else
	{
		BlobFeatureDetector::SamplingDense samplingDense(BlobFeatureDetector::SAMPLING_NORMAL);

		ocean_assert(usedFeatures_ <= maximalUsedFeatures_);
		if (usedFeatures_ == maximalUsedFeatures_)
		{
			samplingDense = BlobFeatureDetector::SAMPLING_DENSE;
		}

		numberDetectedFeatures_ = BlobFeatureDetector::detectFeatures(integralImage, frame.width(), frame.height(), samplingDense, featureStrengthThreshold_, frameIsUndistorted, features, worker);

		std::sort(features.begin(), features.end());
	}

	ocean_assert(correspondenceManager_.isNull());
	ocean_assert(recentTimestamp_.isInvalid());

	// if now information can be used from previous frames
	return determinePoseWithoutKnowledge(features, frame, integralImage, pinholeCamera, frameIsUndistorted, pose, worker);
}

bool BlobTracker6DOF::determinePoseWithoutKnowledge(BlobFeatures& features, const Frame& frame, const unsigned int* integralImage, const PinholeCamera& pinholeCamera, const bool frameIsUndistorted, HomogenousMatrix4& pose, Worker* worker)
{
	ocean_assert(frame && integralImage != nullptr);
	ocean_assert(frame.width() == pinholeCamera.width() && frame.height() == pinholeCamera.height());

#ifdef OCEAN_DEBUG
	for (BlobFeatures::iterator i = features.begin(); i != features.end(); ++i)
	{
		ocean_assert(i->descriptorType() == BlobFeature::DESCRIPTOR_UNDESCRIBED);
		ocean_assert(i->orientationType() == BlobFeature::ORIENTATION_NOT_ORIENTED);
	}
#endif // OCEAN_DEBUG

	while (true)
	{
		ocean_assert(usedFeatures_ >= minimalUsedFeatures_);
		ocean_assert(usedFeatures_ <= maximalUsedFeatures_);

		const unsigned int usedFeatures = min(usedFeatures_, (unsigned int)(features.size()));

		BlobFeatureDescriptor::calculateOrientationsAndDescriptors(integralImage, frame.width(), frame.height(), FrameType::ORIGIN_UPPER_LEFT, BlobFeature::ORIENTATION_SLIDING_WINDOW, features, usedFeatures, false, worker);

		// now as the descriptors has been calculated on the distorted image position,
		// we can compute the undistorted image positions to receive an accurate tracking result
		if (pinholeCamera.hasDistortionParameters())
		{
			for (unsigned int n = 0u; n < usedFeatures; ++n)
			{
				BlobFeature& feature = features[n];

				ocean_assert(feature.distortionState() == BlobFeature::DS_DISTORTED || feature.distortionState() == BlobFeature::DS_UNDISTORTED);

				if (feature.distortionState() == BlobFeature::DS_DISTORTED)
				{
					feature.setObservation(pinholeCamera.undistort<true>(feature.observation()), BlobFeature::DS_UNDISTORTED);
				}
			}
		}

		const Correspondences::CorrespondencePairs correspondenceCandidates(UnidirectionalCorrespondences::determineFeatureCorrespondencesWithQualityEarlyReject(features, featureMap_.features(), usedFeatures, Scalar(0.1), Scalar(0.6), worker));

		if (correspondenceCandidates.size() >= 8)
		{
			Geometry::ImagePoints imagePoints;
			Geometry::ObjectPoints objectPoints;
			Correspondences::extractCorrespondingPoints(features, featureMap_.features(), correspondenceCandidates, imagePoints, objectPoints);
			ocean_assert(objectPoints.size() == imagePoints.size());

			HomogenousMatrix4 resultingPose;
			FeatureIndices resultingValidCorrespondences;
			Scalar sqrAccuracy = 0;

			const unsigned int minimalValidCorrespondences = realtimePerformance_ ? 7u : 20u;
			const unsigned int ransacIterations = realtimePerformance_ ? 80u : 400u;

			if (Geometry::RANSAC::p3p(pinholeCamera, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), randomGenerator_, pinholeCamera.hasDistortionParameters(), resultingPose, minimalValidCorrespondences, true, ransacIterations, 25, &resultingValidCorrespondences, &sqrAccuracy))
			{
				BlobFeatures validImageFeatures, validObjectFeatures;
				Correspondences::extractCorrespondingFeatures(features, featureMap_.features(), correspondenceCandidates, resultingValidCorrespondences, validImageFeatures, validObjectFeatures);

				Geometry::ObjectPoints oPoints;
				Geometry::ImagePoints iPoints;

				for (FeatureIndices::const_iterator i = resultingValidCorrespondences.begin(); i != resultingValidCorrespondences.end(); ++i)
				{
					oPoints.push_back(objectPoints[*i]);
					iPoints.push_back(imagePoints[*i]);
				}

				// if the tracker is in real-time mode, the unoriented descriptors are used for the next frame
				if (realtimePerformance_)
				{
					if (!frameIsUndistorted)
					{
						// we have to ensure that all feature get the distorted locations again - to get the correct descriptor information

						for (BlobFeatures::iterator i = validImageFeatures.begin(); i != validImageFeatures.end(); ++i)
						{
							if (i->distortionState() == BlobFeature::DS_UNDISTORTED)
							{
								i->setObservation(pinholeCamera.distort<true>(i->observation()), BlobFeature::DS_DISTORTED);
							}
						}
					}

					BlobFeatureDescriptor::calculateNotOrientedDescriptors(integralImage, frame.width(), frame.height(), FrameType::ORIGIN_UPPER_LEFT, validImageFeatures, true, worker);
				}

				correspondenceManager_.setCorrespondences(validImageFeatures, validObjectFeatures, frame.timestamp());

				recentFeatureCorrespondences_ = (unsigned int)validImageFeatures.size();

				recentTimestamp_ = frame.timestamp().isValid() ? frame.timestamp() : Timestamp(true);
				recentPose_ = resultingPose;

				pose = resultingPose;
				usedFeatures_ = max((unsigned int)(Scalar(usedFeatures_) * featuresDecreaseFactor_), minimalUsedFeatures_);

				return true;
			}
		}

		if (usedFeatures_ >= maximalUsedFeatures_)
			break;

		usedFeatures_ = min((unsigned int)(Scalar(usedFeatures_) * featuresIncreaseFactor_), maximalUsedFeatures_);
	}

	recentTimestamp_.toInvalid();
	recentFeatureCorrespondences_ = 0u;
	return false;
}

bool BlobTracker6DOF::determinePoseWithPreviousCorrespondences(BlobFeatures& features, const Frame& frame, const uint32_t* integralImage, const PinholeCamera& pinholeCamera, const Box2& boundingBox, HomogenousMatrix4& pose, Worker* worker)
{
	ocean_assert(frame && integralImage != nullptr);
	ocean_assert(frame.width() == pinholeCamera.width() && frame.height() == pinholeCamera.height());

	ocean_assert(realtimePerformance_);
	ocean_assert(!recentTimestamp_.isInvalid());
	ocean_assert(correspondenceManager_);

	const BlobFeatures managerImageFeatures(correspondenceManager_.imageFeatures());

	Correspondences::MultiCandidates multiCandidates;
	FeatureIndices candidates;
	Correspondences::determineCandidates(managerImageFeatures, features, 20, multiCandidates, candidates);

	BlobFeatureDescriptor::calculateNotOrientedDescriptors(integralImage, frame.width(), frame.height(), FrameType::ORIGIN_UPPER_LEFT, features, candidates, true, worker);
	//BlobFeatureDescriptor::calculateOrientationsAndDescriptors(integralImage, frame.width(), frame.height(), FrameType::ORIGIN_UPPER_LEFT, BlobFeature::ORIENTATION_SLIDING_WINDOW, features, candidates, false, worker);

	// now as the descriptors has been calculated on the distorted image position,
	// we can compute the undistorted image positions to receive an accurate tracking result
	if (pinholeCamera.hasDistortionParameters())
	{
		for (FeatureIndices::const_iterator i = candidates.cbegin(); i != candidates.cend(); ++i)
		{
			BlobFeature& feature = features[*i];

			ocean_assert(feature.distortionState() == BlobFeature::DS_DISTORTED || feature.distortionState() == BlobFeature::DS_UNDISTORTED);

			if (feature.distortionState() == BlobFeature::DS_DISTORTED)
			{
				feature.setObservation(pinholeCamera.undistort<true>(feature.observation()), BlobFeature::DS_UNDISTORTED);
			}
		}
	}

	Correspondences::CorrespondencePairs correspondenceCandidates(UnidirectionalCorrespondences::determineFeatureCorrespondences(managerImageFeatures, features, multiCandidates, Scalar(0.07)));

	static const unsigned int minCorr = 10;

#ifdef OCEAN_HARDWARE_REDUCED_PERFORMANCE
	static const unsigned int spatialElements = 5u;
#else
	static const unsigned int spatialElements = realtimePerformance_ ? 6u : 10u;
#endif

	if (correspondenceCandidates.size() > minCorr)
	{
		Geometry::ObjectPoints objectPoints(correspondenceManager_.objectPositions(correspondenceCandidates));
		Geometry::ImagePoints imagePoints;
		imagePoints.reserve(correspondenceCandidates.size());

		for (Correspondences::CorrespondencePairs::const_iterator i = correspondenceCandidates.begin(); i != correspondenceCandidates.end(); ++i)
		{
			ocean_assert(i->second < features.size());
			imagePoints.push_back(features[i->second].observation());

			ocean_assert(features[i->second].scale() > 0);
		}

		ocean_assert(objectPoints.size() == imagePoints.size());

		const unsigned int iterations = realtimePerformance_ ? 20u : 100u;

		HomogenousMatrix4 resultingPoseFirstIteration;
		FeatureIndices resultingValidCorrespondences;

		Scalar initialError = 0, finalError = 0;
		if (Geometry::NonLinearOptimizationPose::optimizePose(pinholeCamera, recentPose_, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), false, resultingPoseFirstIteration, iterations, Geometry::Estimator::ET_HUBER, Scalar(0.001), 10, &initialError, &finalError))
		{
			const Geometry::SpatialDistribution::DistributionArray spatialIndexArrayUsed(Geometry::SpatialDistribution::distributeToArray(imagePoints.data(), imagePoints.size(), boundingBox.lower().x(), boundingBox.lower().y(), boundingBox.width(), boundingBox.height(), spatialElements, spatialElements));
			const Geometry::SpatialDistribution::DistributionArray spatialIndexArrayNew(Geometry::SpatialDistribution::distributeToArray<BlobFeature, feature2Vector>(features.data(), features.size(),  boundingBox.lower().x(), boundingBox.lower().y(), boundingBox.width(), boundingBox.height(), spatialElements, spatialElements));

			// we expect a scale and shear free orientation matrix
			ocean_assert(resultingPoseFirstIteration.rotationMatrix() == resultingPoseFirstIteration.orthonormalRotationMatrix());
			const Vector3 poseTranslation(resultingPoseFirstIteration.translation());
			const Quaternion poseOrientation(resultingPoseFirstIteration.rotationMatrix());

			// find empty spatial bins and try to finde feature matches
			for (unsigned int n = 0; n < spatialIndexArrayUsed.bins(); ++n)
			{
				if (spatialIndexArrayUsed[n].empty())
				{
					ocean_assert(n < spatialIndexArrayNew.bins());
					const Indices32& binIndices = spatialIndexArrayNew[n];

#ifdef OCEAN_DEBUG
					// features should be sorted by their strength values
					for (unsigned int i = 0; i + 1 < binIndices.size(); ++i)
						ocean_assert(features[binIndices[i]].strength() >= features[binIndices[i + 1]].strength());
#endif

					// try to find correspondences for at most the first 2 strongest features
					for (unsigned int i = 0; i < 2 && i < binIndices.size(); ++i)
					{
						const unsigned int imageFeatureIndex = binIndices[i];

						BlobFeature& interestFeature = features[imageFeatureIndex];
						if (interestFeature.descriptorType() != BlobFeature::DESCRIPTOR_ORIENTED)
						{
							BlobFeatureDescriptor::calculateOrientation(integralImage, frame.width(), frame.height(), FrameType::ORIGIN_UPPER_LEFT,  BlobFeature::ORIENTATION_SLIDING_WINDOW, interestFeature, true);
							BlobFeatureDescriptor::calculateDescriptor(integralImage, frame.width(), frame.height(), FrameType::ORIGIN_UPPER_LEFT, interestFeature, true);
						}

						if (interestFeature.distortionState() == CV::Detector::PointFeature::DS_DISTORTED && pinholeCamera.hasDistortionParameters())
							interestFeature.setObservation(pinholeCamera.undistort<true>(interestFeature.observation()), CV::Detector::PointFeature::DS_UNDISTORTED);

						unsigned int correspondenceFeatureMapIndex = (unsigned int)(-1);
						if (Correspondences::determineCorresponding3DFeature(interestFeature, featureMap_, poseTranslation, poseOrientation, pinholeCamera, Numeric::deg2rad(3), Scalar(0.1), correspondenceFeatureMapIndex))
						{
							const BlobFeature& objectFeature = featureMap_.features()[correspondenceFeatureMapIndex];
							ocean_assert(correspondenceFeatureMapIndex < featureMap_.features().size());

							BlobFeatureDescriptor::calculateNotOrientedDescriptor(integralImage, frame.width(), frame.height(), FrameType::ORIGIN_UPPER_LEFT, interestFeature, true);
							const unsigned int newCorrspondenceIndex = correspondenceManager_.addCorrespondence(interestFeature, objectFeature, frame.timestamp());

							correspondenceCandidates.push_back(Correspondences::CorrespondencePair(newCorrspondenceIndex, imageFeatureIndex));
							objectPoints.push_back(objectFeature.position());
							imagePoints.push_back(interestFeature.observation());

							ocean_assert(interestFeature.scale() > 0);
							break;
						}
					}
				}
			}

			ocean_assert(objectPoints.size() == imagePoints.size());
			ocean_assert(objectPoints.size() == correspondenceCandidates.size());

			HomogenousMatrix4 resultingPoseSecondIteration;

			initialError = 0;
			finalError = 0;
			if (Geometry::NonLinearOptimizationPose::optimizePose(pinholeCamera, resultingPoseFirstIteration, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), false, resultingPoseSecondIteration, iterations, Geometry::Estimator::ET_HUBER, Scalar(0.001), 10, &initialError, &finalError))
			{

#ifdef OCEAN_HARDWARE_REDUCED_PERFORMANCE
				if (correspondenceCandidates.size() >= 12)
#else
				if (correspondenceCandidates.size() >= 15)
#endif // OCEAN_HARDWARE_REDUCED_PERFORMANCE
				{
					// check whether the determined pose is close enough to the previous pose
					if (Geometry::Error::posesAlmostEqual(recentPose_, resultingPoseSecondIteration, maxPositionOffset_, maxOrientationOffset_))
					{
						Scalars sqrErrors(objectPoints.size(), 0);
						Geometry::Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true, false, true>(resultingPoseSecondIteration, pinholeCamera, ConstTemplateArrayAccessor<Vector3>(objectPoints), ConstTemplateArrayAccessor<Vector2>(imagePoints), false, Scalar(1), nullptr, sqrErrors.data());

						for (unsigned int n = 0; n < correspondenceCandidates.size(); ++n)
						{
							ocean_assert(n < sqrErrors.size());
							const Correspondences::CorrespondencePair& correspondence = correspondenceCandidates[n];

							if (sqrErrors[n] < 3 * 3)
							{
								ocean_assert(correspondence.second < features.size());
								correspondenceManager_.updateCorrespondence(features[correspondence.second], correspondence.first, frame.timestamp());
							}
							else
								correspondenceManager_.invalidateCorrespondence(correspondence.first);
						}

						correspondenceManager_.garbageCollector(frame.timestamp(), 2, pinholeCamera, resultingPoseSecondIteration);
						recentFeatureCorrespondences_ = correspondenceManager_.correspondences();

						recentTimestamp_ = frame.timestamp().isValid() ? frame.timestamp() : Timestamp(true);
						recentPose_ = resultingPoseSecondIteration;

						pose = resultingPoseSecondIteration;
						return true;
					}
				}

			}
		}
	}

	correspondenceManager_.clear();
	recentFeatureCorrespondences_ = 0u;

	return false;
}

bool BlobTracker6DOF::determinePoseWithPreviousPose(BlobFeatures& features, const Frame& frame, const uint32_t* integralImage, const PinholeCamera& pinholeCamera, const bool frameIsUndistorted, HomogenousMatrix4& pose, Worker* worker)
{
	ocean_assert(frame.width() == pinholeCamera.width() && frame.height() == pinholeCamera.height());
	ocean_assert(!recentTimestamp_.isInvalid());

	while (true)
	{
		ocean_assert(usedFeatures_ >= minimalUsedFeatures_);
		ocean_assert(usedFeatures_ <= maximalUsedFeatures_);

		const unsigned int usedFeatures = min(usedFeatures_, (unsigned int)(features.size()));

		BlobFeatureDescriptor::calculateOrientationsAndDescriptors(integralImage, frame.width(), frame.height(), FrameType::ORIGIN_UPPER_LEFT, BlobFeature::ORIENTATION_SLIDING_WINDOW, features, usedFeatures, false, worker);

		// now as the descriptors has been calculated on the distorted image position,
		// we can compute the undistorted image positions to receive an accurate tracking result
		if (pinholeCamera.hasDistortionParameters())
		{
			for (unsigned int n = 0u; n < usedFeatures; ++n)
			{
				BlobFeature& feature = features[n];

				ocean_assert(feature.distortionState() == BlobFeature::DS_DISTORTED || feature.distortionState() == BlobFeature::DS_UNDISTORTED);

				if (feature.distortionState() == BlobFeature::DS_DISTORTED)
				{
					feature.setObservation(pinholeCamera.undistort<true>(feature.observation()), BlobFeature::DS_UNDISTORTED);
				}
			}
		}

		// determine 2D/3D correspondences by finding 3D features for the given 2D positions
		const Correspondences::CorrespondencePairs correspondences(UnidirectionalCorrespondences::determineFeatureCorrespondencesWithPose(AnyCameraPinhole(pinholeCamera), recentPose_, features, featureMap_.features(), usedFeatures, Scalar(5), Scalar(0.05), Scalar(0.6)));

		if (correspondences.size() >= 10)
		{
			Geometry::ImagePoints imagePoints;
			Geometry::ObjectPoints objectPoints;
			Correspondences::extractCorrespondingPoints(features, featureMap_.features(), correspondences, imagePoints, objectPoints);
			ocean_assert(objectPoints.size() == imagePoints.size());

			const unsigned int iterations = realtimePerformance_ ? 20u : 100u;

			HomogenousMatrix4 resultingPose;
			Scalar initialError = 0, finalError = 0;
			if (Geometry::NonLinearOptimizationPose::optimizePose(pinholeCamera, recentPose_, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), false, resultingPose, iterations, Geometry::Estimator::ET_HUBER, Scalar(0.001), Scalar(10), &initialError, &finalError))
			{
				// check whether the determined pose is close enough to the previous pose
				if (Geometry::Error::posesAlmostEqual(recentPose_, resultingPose, maxPositionOffset_, maxOrientationOffset_))
				{
					BlobFeatures validImageFeatures, validObjectFeatures;
					Correspondences::extractCorrespondingFeatures(features, featureMap_.features(), correspondences, resultingPose, pinholeCamera, 25, validImageFeatures, validObjectFeatures);

					// if the tracker is in real-time mode, the unoriented descriptors are used for the next frame
					if (realtimePerformance_)
					{
						if (!frameIsUndistorted)
						{
							// we have to ensure that all feature get the distorted locations again - to get the correct descriptor information

							for (BlobFeatures::iterator i = validImageFeatures.begin(); i != validImageFeatures.end(); ++i)
							{
								if (i->distortionState() == BlobFeature::DS_UNDISTORTED)
								{
									i->setObservation(pinholeCamera.distort<true>(i->observation()), BlobFeature::DS_DISTORTED);
								}
							}
						}

						BlobFeatureDescriptor::calculateNotOrientedDescriptors(integralImage, frame.width(), frame.height(), FrameType::ORIGIN_UPPER_LEFT, validImageFeatures, true, worker);
					}

					correspondenceManager_.setCorrespondences(validImageFeatures, validObjectFeatures, frame.timestamp());

					recentTimestamp_ = frame.timestamp().isValid() ? frame.timestamp() : Timestamp(true);
					recentPose_ = resultingPose;

					pose = resultingPose;
					usedFeatures_ = max((unsigned int)(Scalar(usedFeatures_) * featuresDecreaseFactor_), minimalUsedFeatures_);
					recentFeatureCorrespondences_ = (unsigned int)validImageFeatures.size();

					return true;
				}
			}
		}
		else
		{
			// determine 2D/3D correspondences brute force search
			const Correspondences::CorrespondencePairs correspondenceCandidates(UnidirectionalCorrespondences::determineFeatureCorrespondencesWithQualityEarlyReject(features, featureMap_.features(), usedFeatures, Scalar(0.1), Scalar(0.6), worker));

			if (correspondenceCandidates.size() >= 7)
			{
				Geometry::ImagePoints imagePoints;
				Geometry::ObjectPoints objectPoints;
				Correspondences::extractCorrespondingPoints(features, featureMap_.features(), correspondenceCandidates, imagePoints, objectPoints);
				ocean_assert(objectPoints.size() == imagePoints.size());

				HomogenousMatrix4 resultingPose;
				FeatureIndices resultingValidCorrespondences;

				const unsigned int minimalValidCorrespondences = realtimePerformance_ ? 7u : 30u;
				const unsigned int ransacIterations = realtimePerformance_ ? 40u : 400u;

				Scalar sqrAccuracy = 0;
				if (Geometry::RANSAC::p3p(recentPose_, pinholeCamera, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), randomGenerator_, pinholeCamera.hasDistortionParameters(), resultingPose, maxPositionOffset_, maxOrientationOffset_, minimalValidCorrespondences, true, ransacIterations, 25, &resultingValidCorrespondences, &sqrAccuracy))
				{
					BlobFeatures validImageFeatures, validObjectFeatures;
					Correspondences::extractCorrespondingFeatures(features, featureMap_.features(), correspondenceCandidates, resultingValidCorrespondences, validImageFeatures, validObjectFeatures);

					// if the tracker is in real-time mode, the unoriented descriptors are used for the next frame
					if (realtimePerformance_)
					{
						if (!frameIsUndistorted)
						{
							// we have to ensure that all feature get the distorted locations again - to get the correct descriptor information

							for (BlobFeatures::iterator i = validImageFeatures.begin(); i != validImageFeatures.end(); ++i)
							{
								if (i->distortionState() == BlobFeature::DS_UNDISTORTED)
								{
									i->setObservation(pinholeCamera.distort<true>(i->observation()), BlobFeature::DS_DISTORTED);
								}
							}
						}

						BlobFeatureDescriptor::calculateNotOrientedDescriptors(integralImage, frame.width(), frame.height(), FrameType::ORIGIN_UPPER_LEFT, validImageFeatures, true, worker);
					}

					correspondenceManager_.setCorrespondences(validImageFeatures, validObjectFeatures, frame.timestamp());

					recentTimestamp_ = frame.timestamp().isValid() ? frame.timestamp() : Timestamp(true);
					recentPose_ = resultingPose;

					pose = resultingPose;
					usedFeatures_ = max((unsigned int)(Scalar(usedFeatures_) * featuresDecreaseFactor_), minimalUsedFeatures_);
					recentFeatureCorrespondences_ = (unsigned int)validImageFeatures.size();

					return true;
				}
			}
		}

		if (usedFeatures_ >= maximalUsedFeatures_)
		{
			break;
		}

		usedFeatures_ = min((unsigned int)(Scalar(usedFeatures_) * featuresIncreaseFactor_), maximalUsedFeatures_);
	}

	correspondenceManager_.clear();
	recentFeatureCorrespondences_ = 0u;

	return false;
}

const uint32_t* BlobTracker6DOF::createIntegralImage(const Frame& frame, Worker* worker)
{
	ocean_assert(frame.isValid());

	if (!FrameConverter::Comfort::convert(frame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yFrame_, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
	{
		ocean_assert(false && "This should never happen!");
		return nullptr;
	}

	integralImage_.set(FrameType(frame.width() + 1u, frame.height() + 1u, FrameType::FORMAT_Y32, FrameType::ORIGIN_UPPER_LEFT), true /*forceOwner*/, true /*forceWritable*/);
	ocean_assert(integralImage_.isValid());

	ocean_assert(yFrame_.isValid());
	IntegralImage::createLinedImage<uint8_t, uint32_t, 1u>(yFrame_.constdata<uint8_t>(), integralImage_.data<uint32_t>(), frame.width(), frame.height(), yFrame_.paddingElements(), integralImage_.paddingElements());

	return integralImage_.data<uint32_t>();
}

}

}

}
