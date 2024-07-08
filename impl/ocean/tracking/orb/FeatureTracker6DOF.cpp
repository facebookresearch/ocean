/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/orb/FeatureTracker6DOF.h"
#include "ocean/tracking/orb/UnidirectionalCorrespondences.h"

#include "ocean/cv/detector/FASTFeatureDetector.h"
#include "ocean/cv/detector/ORBFeatureDescriptor.h"
#include "ocean/cv/detector/ORBFeatureOrientation.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/IntegralImage.h"

#include "ocean/geometry/Error.h"
#include "ocean/geometry/NonLinearOptimizationPose.h"
#include "ocean/geometry/RANSAC.h"

namespace Ocean
{

namespace Tracking
{

namespace ORB
{

using namespace CV;
using namespace CV::Detector;

FeatureTracker6DOF::~FeatureTracker6DOF()
{
	// nothing to do here
}

bool FeatureTracker6DOF::setFeatureMap(const FeatureMap& featureMap, const bool autoUpdateMaxPositionOffset)
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(featureMap.isUsingHarrisFeatures() == usingHarrisFeatures_);
	if (featureMap.isUsingHarrisFeatures() != usingHarrisFeatures_)
	{
		return false;
	}

	featureMap_ = featureMap;

	if (autoUpdateMaxPositionOffset)
	{
		// updates the maximal position offset between two frames to a fifth of the feature map bounding box diagonal
		const Scalar diagonal = (featureMap_.boundingBox().higher() - featureMap_.boundingBox().lower()).length() * Scalar(0.2);
		maxPositionOffset_ = Vector3(diagonal, diagonal, diagonal);
	}

	return true;
}

bool FeatureTracker6DOF::determinePoses(const Frame& frame, const PinholeCamera& pinholeCamera, const bool frameIsUndistorted, TransformationSamples& transformations, const Quaternion& /*absoluteOrientation*/, Worker* worker)
{
	ocean_assert(pinholeCamera.width() == frame.width());
	ocean_assert(pinholeCamera.height() == frame.height());

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

	ocean_assert(frame.timestamp().isValid());

	if (recentTimestamp_.isValid() && frame.timestamp() <= recentTimestamp_)
	{
		transformations.push_back(TransformationSample(recentPose_, 0));
		Log::info() << "Pose already detected for the provided frame timestamp.";

		return true;
	}

	if (motionModel_)
	{
		recentPose_ = motionModel_.predictedPose();
	}

	const uint32_t* linedIntegralImage = createLinedIntegralImage(frame, worker);
	ocean_assert(linedIntegralImage != nullptr);

	HomogenousMatrix4 pose;
	const bool trackerResult = determinePose(frame, linedIntegralImage, pinholeCamera, frameIsUndistorted, pose, worker);

	if (trackerResult)
	{
		motionModel_.update(pose);
		transformations.push_back(TransformationSample(pose, 0));
	}

	return true;
}

bool FeatureTracker6DOF::determinePose(const Frame& frame, const uint32_t* integralImage, const PinholeCamera& pinholeCamera, const bool /*frameIsUndistorted*/, HomogenousMatrix4& pose, Worker* worker)
{
	ocean_assert(frame && pinholeCamera.isValid());

	ORBFeatures features;

	// use the number of features tracking in the previous frame to reserve memory for new features
	features.reserve(numberDetectedFeatures_);

	FASTFeatures featuresFAST;
	FASTFeatureDetector::detectFeatures(yFrame_.constdata<uint8_t>(), frame.width(), frame.height(), featureStrengthThreshold_, false, false, featuresFAST, yFrame_.paddingElements(), worker);
	std::sort(featuresFAST.begin(), featuresFAST.end());

	features = CV::Detector::ORBFeature::features2ORBFeatures(featuresFAST, frame.width(), frame.height());

	if (features.size() <= 10)
	{
		return false;
	}

	ORBFeatureOrientation::determineFeatureOrientation(integralImage, frame.width(), frame.height(), features, worker);
	ORBFeatureDescriptor::determineDescriptors(integralImage, frame.width(), frame.height(), features, false, worker);

	numberDetectedFeatures_ = features.size();

	if (recentTimestamp_.isValid())
	{
		int left, top;
		unsigned int width, height, leftClamped, topClamped, widthClamped, heightClamped;
		if (!featureMap_.projectToImagePlane(recentPose_, pinholeCamera, 10, left, top, width, height, leftClamped, topClamped, widthClamped, heightClamped))
		{
			return false;
		}

		const Box2 featureMapBoundingBox(Scalar(leftClamped), Scalar(topClamped), Scalar(leftClamped + widthClamped), Scalar(topClamped + heightClamped));

		// if a pose from the previous frame is known
		if (!recentTimestamp_.isInvalid())
		{
			if (determinePoseWithPreviousPose(features, frame, pinholeCamera, pose))
			{
				return true;
			}
		}

		recentTimestamp_.toInvalid();
	}

	ocean_assert(recentTimestamp_.isInvalid());

	// if now information can be used from previous frames
	return determinePoseWithoutKnowledge(features, frame, pinholeCamera, pose, worker);
}

bool FeatureTracker6DOF::determinePoseWithoutKnowledge(ORBFeatures& features, const Frame& frame, const PinholeCamera& pinholeCamera, HomogenousMatrix4& pose, Worker* worker)
{
	ocean_assert(frame);
	ocean_assert(features.size() >= 1);

	const Correspondences::CorrespondencePairs correspondenceCandidates(UnidirectionalCorrespondences::determineBijectiveCorrespondences(features, featureMap_.features(), matchingThreshold_, worker));

	if (correspondenceCandidates.size() >= 12)
	{
		Geometry::ImagePoints imagePoints;
		Geometry::ObjectPoints objectPoints;
		Correspondences::extractCorrespondingPoints(features, featureMap_.features(), correspondenceCandidates, imagePoints, objectPoints);
		ocean_assert(objectPoints.size() == imagePoints.size());

		HomogenousMatrix4 resultingPose;
		FeatureIndices resultingValidCorrespondences;

		const unsigned int minimalValidCorrespondences = 12u;
		const unsigned int ransacIterations = Geometry::RANSAC::iterations(minimalValidCorrespondences, Scalar(0.99), Scalar(0.2));

		if (Geometry::RANSAC::p3p(AnyCameraPinhole(pinholeCamera), ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), randomGenerator_, resultingPose, minimalValidCorrespondences, true, ransacIterations, 16, &resultingValidCorrespondences))
		{
			ORBFeatures validImageFeatures, validObjectFeatures;
			Correspondences::extractCorrespondingFeatures(features, featureMap_.features(), correspondenceCandidates, resultingValidCorrespondences, validImageFeatures, validObjectFeatures);

			recentTimestamp_ = frame.timestamp();
			recentPose_ = resultingPose;
			pose = resultingPose;

			ORBFeatures newBackward;
			newBackward.reserve(features.size());

			useProjectedFeatures_ = false;

			for (size_t i = 0; i < features.size(); ++i)
			{
				ORBFeature newFeature = features[i];

				const Line3 featureRay(pinholeCamera.ray(newFeature.observation(), pose));
				Vector3 objectPoint;

				if (featureMap_.plane().intersection(featureRay, objectPoint) && featureMap_.boundingBox().isInside(objectPoint))
				{
					newFeature.setPosition(objectPoint);
					newBackward.push_back(newFeature);
				}

				if (newBackward.size() == 200)
				{
					featureMap_.setProjectedFeatures(newBackward);
					useProjectedFeatures_ = true;
					break;
				}
			}
			return true;
		}
	}

	recentTimestamp_.toInvalid();
	return false;
}

bool FeatureTracker6DOF::determinePoseWithPreviousPose(ORBFeatures& features, const Frame& frame, const PinholeCamera& pinholeCamera, HomogenousMatrix4& pose)
{
	ocean_assert(!recentTimestamp_.isInvalid());

	const ORBFeatures& referenceFeaturePoints = useProjectedFeatures_ ? featureMap_.projectedFeatures() : featureMap_.features();

	const Correspondences::CorrespondencePairs correspondenceCandidates(UnidirectionalCorrespondences::determineBijectiveCorrespondencesWithPose(features, referenceFeaturePoints, pinholeCamera, recentPose_, matchingThreshold_));

	if (correspondenceCandidates.size() >= 12)
	{
		Geometry::ImagePoints imagePoints;
		Geometry::ObjectPoints objectPoints;
		Correspondences::extractCorrespondingPoints(features, referenceFeaturePoints, correspondenceCandidates, imagePoints, objectPoints);
		ocean_assert(objectPoints.size() == imagePoints.size());

		HomogenousMatrix4 resultingPose;
		FeatureIndices resultingValidCorrespondences;

		Scalar initialError = 0, finalError = 0;
		if (Geometry::NonLinearOptimizationPose::optimizePose(pinholeCamera, recentPose_, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), false, resultingPose, 20u, Geometry::Estimator::ET_HUBER, Scalar(0.001), Scalar(10), &initialError, &finalError))
		{
			// check whether the determined pose is close enough to the previous pose
			if (Geometry::Error::posesAlmostEqual(recentPose_, resultingPose, maxPositionOffset_, maxOrientationOffset_))
			{
				ORBFeatures validImageFeatures, validObjectFeatures;
				Correspondences::extractCorrespondingFeatures(features, referenceFeaturePoints, correspondenceCandidates, resultingPose, pinholeCamera, 25, validImageFeatures, validObjectFeatures);

				recentTimestamp_ = frame.timestamp();
				recentPose_ = resultingPose;
				pose = resultingPose;

				return true;
			}
		}

		const unsigned int minimalValidCorrespondences = 12u;
		const unsigned int ransacIterations = Geometry::RANSAC::iterations(minimalValidCorrespondences, Scalar(0.99), Scalar(0.2));

		if (Geometry::RANSAC::p3p(recentPose_, pinholeCamera, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), randomGenerator_, pinholeCamera.hasDistortionParameters(), resultingPose, maxPositionOffset_, maxOrientationOffset_, minimalValidCorrespondences, true, ransacIterations, 16, &resultingValidCorrespondences))
		{
			ORBFeatures validImageFeatures, validObjectFeatures;
			Correspondences::extractCorrespondingFeatures(features, referenceFeaturePoints, correspondenceCandidates, resultingValidCorrespondences, validImageFeatures, validObjectFeatures);

			recentTimestamp_ = frame.timestamp();
			recentPose_ = resultingPose;
			pose = resultingPose;

			return true;
		}
	}

	return false;
}

const uint32_t* FeatureTracker6DOF::createLinedIntegralImage(const Frame& frame, Worker* worker)
{
	ocean_assert(frame.isValid());

	if (!FrameConverter::Comfort::convert(frame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yFrame_, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
	{
		ocean_assert(false && "This should never happen!");
		return nullptr;
	}

	ocean_assert(yFrame_.isValid());
	if (!linedIntegralImage_.set(FrameType(yFrame_.width() + 1u, yFrame_.height() + 1u, FrameType::FORMAT_Y32, FrameType::ORIGIN_UPPER_LEFT), true /*forceOwner*/, true /*forceWritable*/))
	{
		ocean_assert(false && "This should never happen!");
		return nullptr;
	}

	ocean_assert(linedIntegralImage_.isValid() && linedIntegralImage_.isContinuous());
	IntegralImage::createLinedImage<uint8_t, uint32_t, 1u>(yFrame_.constdata<uint8_t>(), linedIntegralImage_.data<uint32_t>(), yFrame_.width(), yFrame_.height(), yFrame_.paddingElements(), linedIntegralImage_.paddingElements());

	return linedIntegralImage_.data<uint32_t>();
}

}

}

}
