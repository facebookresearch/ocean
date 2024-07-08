/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/offline/PatternTracker.h"
#include "ocean/tracking/offline/Frame2FrameTracker.h"

#include "ocean/base/Subset.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/MaskAnalyzer.h"

#include "ocean/cv/advanced/FrameRectification.h"

#include "ocean/geometry/CameraCalibration.h"
#include "ocean/geometry/Error.h"
#include "ocean/geometry/NonLinearOptimizationCamera.h"
#include "ocean/geometry/NonLinearOptimizationPose.h"

#include "ocean/math/SuccessionSubset.h"
#include "ocean/math/Triangle2.h"
#include "ocean/math/Variance.h"

namespace Ocean
{

namespace Tracking
{

namespace Offline
{

PatternTracker::RoughPoseBaseComponent::RoughPoseBaseComponent(PatternTracker& parent, const PinholeCamera& pinholeCamera) :
	FrameTrackerComponent(parent),
	parent_(parent),
	camera_(pinholeCamera)
{
	patternId_ = subTracker_.addPattern(parent_.patternFrame_, parent_.patternDimension_, WorkerPool::get().scopedWorker()());
}

PatternTracker::RoughCameraFovComponent::RoughCameraFovComponent(PatternTracker& parent, const PinholeCamera& pinholeCamera, Scalar& resultingFovX, const unsigned int frameNumber) :
	RoughPoseBaseComponent(parent, pinholeCamera),
	resultingFovX_(resultingFovX),
	frameNumber_(frameNumber)
{
	imagePointGroups_.reserve(frameNumber);
	objectPointGroups_.reserve(frameNumber);
	poses_.reserve(frameNumber);
}

PatternTracker::FrameTrackerComponent::IterationResult PatternTracker::RoughCameraFovComponent::onFrame(const unsigned int /*previousIndex*/, const unsigned int /*currentIndex*/, const unsigned int /*iteration*/, const unsigned int /*maximalIterations*/)
{
	Pattern::PatternTracker6DOF::TransformationSamples samples;

	IterationResult iterationResult = IR_SUCCEEDED;

	if (subTracker_.determinePoses(currentFrame_, camera_, !camera_.hasDistortionParameters(), samples, Quaternion(false), WorkerPool::get().scopedWorker()()))
	{
		ocean_assert(samples.size() <= 1);

		if (!samples.empty())
		{
			ocean_assert(samples.front().id() == patternId_);

			Vectors2 imagePoints;
			Vectors3 objectPoints;
			if (subTracker_.recentFeatureCorrespondences(patternId_, imagePoints, objectPoints))
			{
				ocean_assert(imagePoints.size() == objectPoints.size());

				// take frames with more than 20 valid feature correspondences only
				if (imagePoints.size() >= 20)
				{
					const HomogenousMatrix4& pose = samples.front().transformation();

					imagePointGroups_.emplace_back(std::move(imagePoints));
					objectPointGroups_.emplace_back(std::move(objectPoints));
					poses_.emplace_back(pose);

					parent_.eventCallbacks_(AnalysisProgressEvent(parent_.id(), (unsigned int)imagePointGroups_.size() * 100u / frameNumber_));

					if (imagePointGroups_.size() >= frameNumber_)
					{
						iterationResult = IR_FINISHED;
					}
				}
			}
		}
	}

	subTracker_.reset(); // we want a re-localization with each new frame

	return iterationResult;
}

bool PatternTracker::RoughCameraFovComponent::onStop(const unsigned int lowerFrameIndex, const unsigned int initialFrameIndex, const unsigned int upperFrameIndex)
{
	if (!RoughPoseBaseComponent::onStop(lowerFrameIndex, initialFrameIndex, upperFrameIndex))
	{
		return false;
	}

	if (imagePointGroups_.empty())
	{
		return false;
	}

	ocean_assert(imagePointGroups_.size() == objectPointGroups_.size());
	ocean_assert(imagePointGroups_.size() == poses_.size());

	return Geometry::CameraCalibration::determineBestMatchingFovX(camera_.width(), camera_.height(), ConstArrayAccessor<HomogenousMatrix4>(poses_), ConstArrayAccessor<Vectors3>(objectPointGroups_), ConstArrayAccessor<Vectors2>(imagePointGroups_), resultingFovX_, true, Numeric::deg2rad(35), Numeric::deg2rad(115));
}

PatternTracker::RoughPoseComponent::RoughPoseComponent(PatternTracker& parent, const PinholeCamera& pinholeCamera, OfflinePoses& resultingPoses) :
	RoughPoseBaseComponent(parent, pinholeCamera),
	resultingPoses_(resultingPoses)
{
	// nothing to do here
}

bool PatternTracker::RoughPoseComponent::onStart(const unsigned int lowerFrameIndex, const unsigned int initialFrameIndex, const unsigned int upperFrameIndex)
{
	if (!RoughPoseBaseComponent::onStart(lowerFrameIndex, initialFrameIndex, upperFrameIndex))
	{
		return false;
	}

	const size_t frames = upperFrameIndex - lowerFrameIndex + 1;

	poses_ = OfflinePoses(lowerFrameIndex, frames, OfflinePose());

	return true;
}

PatternTracker::RoughPoseComponent::IterationResult PatternTracker::RoughPoseComponent::onFrame(const unsigned int /*previousIndex*/, const unsigned int currentIndex, const unsigned int iteration, const unsigned int maximalIterations)
{
	Pattern::PatternTracker6DOF::TransformationSamples samples;

	if (subTracker_.determinePoses(currentFrame_, camera_, !camera_.hasDistortionParameters(), samples, Quaternion(false), WorkerPool::get().scopedWorker()()))
	{
		ocean_assert(samples.size() <= 1);

		ocean_assert(poses_.isValidIndex(currentIndex));

		if (samples.empty())
		{
			poses_[currentIndex] = OfflinePose();
		}
		else
		{
			Vectors2 imagePoints;
			Vectors3 objectPoints;
			subTracker_.recentFeatureCorrespondences(patternId_, imagePoints, objectPoints);

			poses_[currentIndex] = OfflinePose(currentIndex, samples.front().transformation(), Scalar(imagePoints.size()));
		}

		parent_.eventCallbacks_(TrackerPoseStateEvent(parent_.id(), poses_[currentIndex]));
		parent_.eventCallbacks_(TrackingProgressEvent(parent_.id(), (iteration + 1u) * 100u / maximalIterations));
	}

	return IR_SUCCEEDED;
}

bool PatternTracker::RoughPoseComponent::onStop(const unsigned int /*lowerFrameIndex*/, const unsigned int /*initialFrameIndex*/, const unsigned int /*upperFrameIndex*/)
{
	resultingPoses_ = poses_;
	return true;
}

PatternTracker::FineTrackingComponent::FineTrackingComponent(PatternTracker& parent, const PinholeCamera& pinholeCamera, const OfflinePoses& poses, const unsigned int iterations, const unsigned int detectionBorder, const unsigned int horizontalBinSize, const unsigned int verticalBinSize, OfflinePoses& optimizedPoses, PinholeCamera* optimizedCamera) :
	FrameTrackerComponent(parent),
	parent_(parent),
	camera_(pinholeCamera),
	poses_(poses),
	iterations_(iterations),
	detectionBorder_(detectionBorder),
	horizontalBinSize_(horizontalBinSize),
	verticalBinSize_(verticalBinSize),
	optimizedPoses_(optimizedPoses),
	optimizedCamera_(optimizedCamera)
{
	// nothing to do here
}

bool PatternTracker::FineTrackingComponent::onStart(const unsigned int lowerFrameIndex, const unsigned int initialFrameIndex, const unsigned int upperFrameIndex)
{
	if (!FrameTrackerComponent::onStart(lowerFrameIndex, initialFrameIndex, upperFrameIndex))
	{
		return false;
	}

	const size_t frames = upperFrameIndex - lowerFrameIndex + 1;

	optimizedPoses_ = OfflinePoses(lowerFrameIndex, frames, OfflinePose());

	if (optimizedCamera_)
	{
		imagePointsSet_ = ShiftVector<ImagePoints>(lowerFrameIndex, frames, ImagePoints());
		objectPointsSet_ = ShiftVector<ObjectPoints>(lowerFrameIndex, frames, ObjectPoints());
	}

	return true;
}

FrameTracker::FrameTrackerComponent::IterationResult PatternTracker::FineTrackingComponent::onFrame(const unsigned int /*previousIndex*/, const unsigned int currentIndex, const unsigned int iteration, const unsigned int maximalIterations)
{
	ocean_assert(currentFrame_.isValid());

	const WorkerPool::ScopedWorker scopedWorker(WorkerPool::get().scopedWorker());

	// check whether the current frame has a rough pose already, otherwise the object is expected to be invisible and we can proceed with the next frame
	if (!poses_[currentIndex].isValid())
	{
		return IR_SUCCEEDED;
	}

	// check whether the pattern frame pyramid has to be created (this can not be done earlier as we need the pixel format of the camera frame)
	if (!patternFramePyramid_.isValid())
	{
		Frame targetFrame;
		if (!CV::FrameConverter::Comfort::convert(parent_.patternFrame_, currentFrame_.pixelFormat(), currentFrame_.pixelOrigin(), targetFrame, CV::FrameConverter::CP_ALWAYS_COPY, scopedWorker()))
		{
			ocean_assert(false && "This should never happen!");
			return IR_FAILED;
		}

		// we apply a simple Gaussian blur as we expect the pattern frame to have perfect edges which could be difficult for tracking
		if (!patternFramePyramid_.replace(CV::FramePyramid::DM_FILTER_14641, std::move(targetFrame), CV::FramePyramid::AS_MANY_LAYERS_AS_POSSIBLE, scopedWorker()))
		{
			ocean_assert(false && "This should never happen!");
			return IR_FAILED;
		}
	}

	const OfflinePose& offlinePose = poses_[currentIndex];
	ocean_assert(offlinePose);

	ImagePoints imagePoints;
	ObjectPoints objectPoints;

	HomogenousMatrix4 optimizedPose;
	if (!optimizePose(camera_, offlinePose.transformation(), currentFrame_, detectionBorder_, horizontalBinSize_, verticalBinSize_, imagePoints, objectPoints, optimizedPose, scopedWorker()))
	{
		// the tracking pattern seems to be invisible, thus we invalidate the pose and proceed with the next pose

		// we leave the pose id untouched
		optimizedPoses_[currentIndex].setTransformation(HomogenousMatrix4(false));
		optimizedPoses_[currentIndex].setQuality(Scalar(-1));

		return IR_SUCCEEDED;
	}

	ocean_assert(currentIndex == offlinePose.id());

	optimizedPoses_[currentIndex].setId(offlinePose.id());
	optimizedPoses_[currentIndex].setTransformation(optimizedPose);
	optimizedPoses_[currentIndex].setQuality(Scalar(imagePoints.size()));

	parent_.eventCallbacks_(TrackerPoseStateEvent(parent_.id(), optimizedPoses_[currentIndex]));
	parent_.eventCallbacks_(TrackingProgressEvent(parent_.id(), (iteration + 1u) * 100u / maximalIterations));

	if (optimizedCamera_)
	{
		ocean_assert(objectPointsSet_ && imagePointsSet_);

		objectPointsSet_[currentIndex] = std::move(objectPoints);
		imagePointsSet_[currentIndex] = std::move(imagePoints);
	}

	return IR_SUCCEEDED;
}

bool PatternTracker::FineTrackingComponent::optimizePose(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose, const Frame& frame, const unsigned int detectionBorder, const unsigned int horizontalBinSize, const unsigned int verticalBinSize, ImagePoints& imagePoints, ObjectPoints& objectPoints, HomogenousMatrix4& optimizedPose, Worker* worker)
{
	ocean_assert(pose.isValid());

	ocean_assert(pinholeCamera.width() == frame.width() && pinholeCamera.height() == frame.height());

	ocean_assert(frame.pixelFormat() == patternFramePyramid_.frameType().pixelFormat());
	ocean_assert(frame.pixelOrigin() == patternFramePyramid_.frameType().pixelOrigin());

	// the projected area of the tracking pattern in square pixel
	const double area = parent_.projectedPatternArea(pose, pinholeCamera);

	// determine the best matching layer of the pattern pyramid that matches the projected pattern area
	unsigned int layer = 0u;
	for (unsigned int n = 0u; n < patternFramePyramid_.layers(); ++n)
	{
		if ((unsigned int)area > patternFramePyramid_[n].width() * patternFramePyramid_[n].height())
		{
			break;
		}
		else
		{
			layer = n;
		}
	}

	// this layer of the pattern will be used for pose optimization
	const Frame& trackerPatternLayer = patternFramePyramid_[layer];

	HomogenousMatrix4 intermediatePose(pose);
	for (unsigned int i = 0u; i < iterations_; ++i)
	{
		if (!optimizePose(pinholeCamera, intermediatePose, frame, trackerPatternLayer, detectionBorder, horizontalBinSize, verticalBinSize, imagePoints, objectPoints, optimizedPose, worker))
		{
			return false;
		}

		intermediatePose = optimizedPose;
	}

	return true;
}

bool PatternTracker::FineTrackingComponent::optimizePose(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose, const Frame& frame, const Frame& patternLayer, const unsigned int detectionBorder, const unsigned int horizontalBinSize, const unsigned int verticalBinSize, ImagePoints& imagePoints, ObjectPoints& objectPoints, HomogenousMatrix4& optimizedPose, Worker* worker)
{
	ocean_assert(pose.isValid());

	ocean_assert(pinholeCamera.width() == frame.width() && pinholeCamera.height() == frame.height());

	ocean_assert(frame.pixelFormat() == patternFramePyramid_.frameType().pixelFormat());
	ocean_assert(frame.pixelOrigin() == patternFramePyramid_.frameType().pixelOrigin());

	// **TODO** create and use FramePyramid of pattern (below in the point tracking function)

	// Create the rectification frame
	Frame rectified(FrameType(frame, patternLayer.width(), patternLayer.height()));
	Frame rectifiedMask(FrameType(frame, FrameType::FORMAT_Y8));

	// create a rectified version of the live video tracking object
	if (!CV::Advanced::FrameRectification::Comfort::planarRectangleObjectMask(frame, AnyCameraPinhole(pinholeCamera), pose, Vector3(0, 0, 0), Vector3(parent_.patternDimension_.x(), 0, 0), Vector3(0, 0, parent_.patternDimension_.y()), rectified, rectifiedMask, worker, 0xFF))
	{
		return false;
	}

	ocean_assert(rectified.pixelFormat() == patternLayer.pixelFormat() && rectified.pixelOrigin() == patternLayer.pixelOrigin());
	if (rectified.pixelFormat() != patternLayer.pixelFormat() || rectified.pixelOrigin() != patternLayer.pixelOrigin())
	{
		return false;
	}

	// determine the bounding box of all visible pixels of the tracking pattern in the current frame
	const CV::PixelBoundingBox maskBoundingBox(CV::MaskAnalyzer::detectBoundingBox(rectifiedMask.constdata<uint8_t>(), rectifiedMask.width(), rectifiedMask.height(), 0x00u, rectifiedMask.paddingElements()));

	// we define a sub-region that shrinks the tracking area and avoids to use tracking points near the frame borders
	Box2 maskTrackingArea = maskBoundingBox.isValid() ? Box2(Scalar(maskBoundingBox.left()), Scalar(maskBoundingBox.top()), Scalar(maskBoundingBox.right()), Scalar(maskBoundingBox.bottom())) : Box2();

	// check whether the tracking area can be reduced further
	if (!maskTrackingArea.isValid() || (maskTrackingArea.width() > Scalar(detectionBorder * 10u) && maskTrackingArea.height() > Scalar(detectionBorder * 10u)))
	{
		const Scalar floatBorder = (patternLayer.width() <= detectionBorder * 2u || patternLayer.height() <= detectionBorder * 2u) ? Scalar(0) : Scalar(detectionBorder);
		const Box2 borderTrackingArea(floatBorder, floatBorder, Scalar(patternLayer.width()) - floatBorder, Scalar(patternLayer.height()) - floatBorder);

		if (maskTrackingArea.isValid())
		{
			maskTrackingArea = maskTrackingArea.intersection(borderTrackingArea);
		}
		else
		{
			maskTrackingArea = borderTrackingArea;
		}
	}

	const CV::SubRegion patternLayerSubRegion(maskTrackingArea);

	const unsigned int horizontalBins = max(4u, patternLayer.width() / max(1u, horizontalBinSize));
	const unsigned int verticalBins = max(4u, patternLayer.height() / max(1u, verticalBinSize));

	// **TODO** create and use FramePyramid of rectified

	Vectors2 rectifiedImagePoints, patternImagePoints;
	if (!CV::Advanced::AdvancedMotionZeroMeanSSD::trackArbitraryPointsBidirectionalSubPixelMirroredBorder<15u>(patternLayer, rectified, max(rectified.width(), rectified.height()) / 20u, 4u, patternImagePoints, rectifiedImagePoints, Scalar(0.5 * 0.5), patternLayerSubRegion, horizontalBins, verticalBins, 20u, CV::FramePyramid::DM_FILTER_11, worker, 3u) || rectifiedImagePoints.size() < 15)
	{
		rectifiedImagePoints.clear();
		patternImagePoints.clear();

		if (!CV::Advanced::AdvancedMotionZeroMeanSSD::trackArbitraryPointsBidirectionalSubPixelMirroredBorder<15u>(patternLayer, rectified, max(rectified.width(), rectified.height()) / 20u, 4u, patternImagePoints, rectifiedImagePoints, Scalar(1.9 * 1.9), patternLayerSubRegion, horizontalBins, verticalBins, 10u, CV::FramePyramid::DM_FILTER_11, worker, 3u) || rectifiedImagePoints.size() < 15)
		{
			// we expect the tracking object to be invisible thus the pose should be invalidated
			return false;
		}
	}

	objectPoints.clear();
	objectPoints.reserve(patternImagePoints.size());

	imagePoints.clear();
	imagePoints.reserve(patternImagePoints.size());

	const HomogenousMatrix4 poseIF(PinholeCamera::standard2InvertedFlipped(pose));
	const Scalar xPatternFactor = Scalar(1) / Scalar(patternLayer.width());
	const Scalar yPatternFactor = Scalar(1) / Scalar(patternLayer.height());

	for (size_t n = 0; n < patternImagePoints.size(); ++n)
	{
		const Vector2& patternImagePoint = patternImagePoints[n];
		const Vector3 patternObjectPoint(patternImagePoint.x() * parent_.patternDimension_.x() * xPatternFactor, 0,
											patternImagePoint.y() * parent_.patternDimension_.y() * yPatternFactor);

		const Vector2& correctedImagePoint = rectifiedImagePoints[n];
		const Vector3 correctedObjectPoint(correctedImagePoint.x() * parent_.patternDimension_.x() * xPatternFactor, 0,
											correctedImagePoint.y() * parent_.patternDimension_.y() * yPatternFactor);

		const Vector2 projectedCorrectedObjectPoint(pinholeCamera.projectToImageIF<true>(poseIF, correctedObjectPoint, true));

		objectPoints.push_back(patternObjectPoint);
		imagePoints.push_back(projectedCorrectedObjectPoint);
	}

	ocean_assert(objectPoints.size() == imagePoints.size());

	if (!Geometry::NonLinearOptimizationPose::optimizePose(pinholeCamera, pose, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), pinholeCamera.hasDistortionParameters(), optimizedPose, 20u, Geometry::Estimator::ET_HUBER, Scalar(0.001), Scalar(5)))
	{
		return false;
	}

	return true;
}

bool PatternTracker::FineTrackingComponent::onStop(const unsigned int lowerFrameIndex, const unsigned int initialFrameIndex, const unsigned int upperFrameIndex)
{
	if (!FrameTrackerComponent::onStop(lowerFrameIndex, initialFrameIndex, upperFrameIndex))
	{
		return false;
	}

	if (optimizedCamera_ && !optimizeCamera(camera_, 10u, 5u, lowerFrameIndex, upperFrameIndex, *optimizedCamera_))
	{
		// the optimization of the camera profile failed, however, than we use the normale profile
		*optimizedCamera_ = camera_;
	}

	// the optimized poses have been assigned already

	return true;
}

bool PatternTracker::FineTrackingComponent::optimizeCamera(const PinholeCamera& pinholeCamera, const unsigned int numberFrames, const unsigned int iterations, const unsigned int lowerFrameIndex, const unsigned int upperFrameIndex, PinholeCamera& optimizedCamera)
{
	ocean_assert(pinholeCamera.isValid());

	// find a subset of all poses best representing the entire tracking sequence
	Poses poses;
	poses.reserve(optimizedPoses_.size());

	Indices32 poseMapping;
	poseMapping.reserve(optimizedPoses_.size());

	// find the threshold for feature point correspondences
	Indices32 pointCorrespondences;
	pointCorrespondences.reserve(optimizedPoses_.size());

	for (OfflinePoses::Index i = optimizedPoses_.firstIndex(); i < optimizedPoses_.endIndex(); ++i)
	{
		if (optimizedPoses_[i].isValid())
		{
			pointCorrespondences.push_back((unsigned int)objectPointsSet_[i].size());
		}
	}

	const unsigned int qualityThreshold = Median::median(pointCorrespondences.data(), pointCorrespondences.size());

	for (OfflinePoses::Index i = optimizedPoses_.firstIndex(); i < optimizedPoses_.endIndex(); ++i)
	{
		if (optimizedPoses_[i].isValid() && objectPointsSet_[i].size() >= qualityThreshold)
		{
			// poses is not a shift vector so we have to adjust the index
			poses.push_back(Pose(optimizedPoses_[i].transformation()));
			poseMapping.push_back((unsigned int)i);
		}
	}

	SuccessionSubset<Scalar, 6> successionSubset((SuccessionSubset<Scalar, 6>::Object*)poses.data(), poses.size());

	HomogenousMatrices4 bestPoses;
	Geometry::ObjectPointGroups bestObjectPoints;
	Geometry::ImagePointGroups bestImagePoints;
	Indices32 bestIndices;

	bestPoses.reserve(numberFrames);
	bestObjectPoints.reserve(numberFrames);
	bestImagePoints.reserve(numberFrames);
	bestIndices.reserve(numberFrames);

	while (bestPoses.size() < numberFrames)
	{
		// find the best next pose
		const size_t indexZeroBased = successionSubset.incrementSubset();
		if (indexZeroBased == size_t(-1))
		{
			break;
		}

		// the subset index might not be identical with the frame index of the tracking sequence
		const unsigned int frameIndex = poseMapping[indexZeroBased];

		// check whether the pose of the frame is invalid
		if (!optimizedPoses_[frameIndex].isValid())
		{
			continue;
		}

		bestPoses.push_back(optimizedPoses_[frameIndex].transformation());
		bestObjectPoints.push_back(objectPointsSet_[frameIndex]);
		bestImagePoints.push_back(imagePointsSet_[frameIndex]);
		bestIndices.push_back(frameIndex);
	}

	if (bestPoses.size() < 10)
	{
		return false;
	}

	Log::info() << "Beginning camera optimization.";
	Log::info() << "Using " << bestPoses.size() << " frames...";

	PinholeCamera localCamera(pinholeCamera);

	// now determine the initial field of view so that we have a good starting point
	Scalar idealFovX = Numeric::deg2rad(localCamera.fovX());
	if (Geometry::CameraCalibration::determineBestMatchingFovX(localCamera.width(), localCamera.height(), ConstArrayAccessor<HomogenousMatrix4>(bestPoses), ConstArrayAccessor<Vectors3>(bestObjectPoints), ConstArrayAccessor<Vectors2>(bestImagePoints), idealFovX, false, Numeric::deg2rad(35), Numeric::deg2rad(115)))
	{
		Geometry::CameraCalibration::determineBestMatchingFovX(localCamera.width(), localCamera.height(), ConstArrayAccessor<HomogenousMatrix4>(bestPoses), ConstArrayAccessor<Vectors3>(bestObjectPoints), ConstArrayAccessor<Vectors2>(bestImagePoints), idealFovX, false, idealFovX - Numeric::deg2rad(5), idealFovX + Numeric::deg2rad(5));
		localCamera = PinholeCamera(localCamera.width(), localCamera.height(), idealFovX);
	}

	Log::info() << "Starting with a camera with " << Numeric::rad2deg(localCamera.fovX()) << " fovX";

	// optimize the pose for all frames individually for the new camera with new fovX
	for (size_t n = 0; n < bestPoses.size(); ++n)
	{
		HomogenousMatrix4& pose = bestPoses[n];
		ImagePoints& imagePoints = bestImagePoints[n];
		ObjectPoints& objectPoints = bestObjectPoints[n];

		HomogenousMatrix4 optimizedPose;
		if (!Geometry::NonLinearOptimizationPose::optimizePose(localCamera, pose, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), localCamera.hasDistortionParameters(), optimizedPose, 20u, Geometry::Estimator::ET_HUBER))
		{
			ocean_assert(false && "This should never happen!");
			continue;
		}

		pose = optimizedPose;
	}

	optimizedCamera = localCamera;

	for (unsigned int i = 0u; i < iterations; ++i)
	{
		Scalar initialSqrError = 0, finalSqrError = 0;

		HomogenousMatrices4 optimizedPoses;
		NonconstArrayAccessor<HomogenousMatrix4> optimizedPosesAccessor(optimizedPoses);

		if (!Geometry::NonLinearOptimizationCamera::optimizeCameraPoses(localCamera, ConstArrayAccessor<HomogenousMatrix4>(bestPoses), ConstArrayAccessor<Vectors3>(bestObjectPoints), ConstArrayAccessor<Vectors2>(bestImagePoints), optimizedCamera, &optimizedPosesAccessor, 50u, Geometry::Estimator::ET_HUBER, Scalar(0.001), Scalar(5), true, &initialSqrError, &finalSqrError))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		localCamera = optimizedCamera;
		bestPoses = optimizedPoses;

		for (size_t n = 0; n < bestPoses.size(); n++)
		{
			HomogenousMatrix4& pose = bestPoses[n];

			ObjectPoints& objectPoints = bestObjectPoints[n];
			ImagePoints& imagePoints = bestImagePoints[n];

			const unsigned int frameIndex = bestIndices[n];

			const FrameRef frame(parent_.frameProviderInterface_->synchronFrameRequest(frameIndex, 10.0, &parent_.shouldStop_));
			if (frame.isNull())
			{
				ocean_assert(false && "This should never happen!");
				return false;
			}

			const FrameType::PixelFormat targetPixelFormat = patternFramePyramid_.frameType().pixelFormat();
			const FrameType::PixelOrigin targetPixelOrigin = patternFramePyramid_.frameType().pixelOrigin();

			Frame currentFrame;
			if (!CV::FrameConverter::Comfort::convert(*frame, targetPixelFormat, targetPixelOrigin, currentFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, WorkerPool::get().scopedWorker()()))
			{
				return false;
			}

			HomogenousMatrix4 optimizedPose;
			if (!optimizePose(localCamera, pose, currentFrame, 10u, 30u, 30u, imagePoints, objectPoints, optimizedPose, WorkerPool::get().scopedWorker()()))
			{
				ocean_assert(false && "This should never happen!");
				return false;
			}

			pose = optimizedPose;

			// measure the final pose and camera quality
			finalSqrError += Geometry::Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true, false, true>(pose, localCamera, ConstTemplateArrayAccessor<Vector3>(objectPoints), ConstTemplateArrayAccessor<Vector2>(imagePoints), localCamera.hasDistortionParameters());
		}

		parent_.eventCallbacks_(TrackingProgressEvent(parent_.id(), (i + 1u) * 100u / iterations));
	}

	// finally optimized all frame poses, however most of them will be optimized with old 2D/3D point correspondences
	for (unsigned int n = lowerFrameIndex; n <= upperFrameIndex; ++n)
	{
		const HomogenousMatrix4& pose = optimizedPoses_[n].transformation();

		if (pose.isValid())
		{
			const ImagePoints& imagePoints = imagePointsSet_[n];
			const ObjectPoints& objectPoints = objectPointsSet_[n];

			HomogenousMatrix4 optimizedPose;
			if (!Geometry::NonLinearOptimizationPose::optimizePose(optimizedCamera, pose, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), optimizedCamera.hasDistortionParameters(), optimizedPose, 20u, Geometry::Estimator::ET_HUBER))
			{
				ocean_assert(false && "This should never happen!");
				continue;
			}

			optimizedPoses_[n].setTransformation(optimizedPose);
		}
	}

	return true;
}

PatternTracker::PatternTracker() :
	PlaneTracker(Plane3(Vector3(0, 1, 0), 0)),
	patternDimension_(0, 0)
{
	// The static plane of this pattern tracker will be the X-Z plane

	patternCorners_[0] = Vector3(0, 0, 0);
	patternCorners_[1] = Vector3(0, 0, 0);
	patternCorners_[2] = Vector3(0, 0, 0);
	patternCorners_[3] = Vector3(0, 0, 0);
}

bool PatternTracker::setPattern(const Frame& frame, const Vector2& dimension)
{
	if (patternFrame_)
	{
		return false;
	}

	if (frame.width() == 0 || frame.height() == 0 || dimension.x() <= 0)
	{
		return false;
	}

	const FrameType::PixelFormat targetPixelFormat = FrameType::formatRemoveAlphaChannel(FrameType::genericSinglePlanePixelFormat(frame.pixelFormat()));

	if (!CV::FrameConverter::Comfort::convert(frame, targetPixelFormat, FrameType::ORIGIN_UPPER_LEFT, patternFrame_, false))
	{
		return false;
	}

	updatePatternDimension(dimension.y() <= 0 ? Vector2(dimension.x(), Scalar(frame.height()) * dimension.x() / Scalar(frame.width())) : dimension);

	patternCorners_[0] = Vector3(0, 0, 0);
	patternCorners_[1] = Vector3(0, 0, patternDimension_.y());
	patternCorners_[2] = Vector3(patternDimension_.x(), 0, patternDimension_.y());
	patternCorners_[3] = Vector3(patternDimension_.x(), 0, 0);

	return true;
}

bool PatternTracker::setCamera(const PinholeCamera& pinholeCamera, const Scalar cameraPrecision, const bool optimizeCamera)
{
	if (!finished_)
	{
		return false;
	}

	return FrameTracker::setCamera(pinholeCamera, cameraPrecision, optimizeCamera);
}

bool PatternTracker::applyFrameTracking(const FrameType& frameType)
{
	// check whether the initial camera object matches with the current frame type
	if (camera_.isValid() && (camera_.width() != frameType.width() || camera_.height() != frameType.height()))
	{
		Log::warning() << "This given camera object does not match the frame type!";
		camera_ = PinholeCamera();
	}

	// use a default camera object with no camera object is given
	if (!camera_.isValid())
	{
		ocean_assert(optimizeCamera_);
		camera_ = PinholeCamera(frameType.width(), frameType.height(), Numeric::deg2rad(65));
	}

	// at this position a valid camera object must be defined, invoke the camera event
	ocean_assert(camera_);
	eventCallbacks_(CameraCalibrationStateEvent(id_, camera_));

	// invoke the plane event
	ocean_assert(plane_.isValid());
	eventCallbacks_(TrackerPlaneStateEvent(id_, plane_));

	// invoke the object transformation event
	ocean_assert(objectTransformation_.isValid());
	eventCallbacks_(TrackerTransformationStateEvent(id_, objectTransformation_));

	const unsigned int startFrameIndex = startFrameIndex_ == (unsigned int)(-1) ? lowerFrameIndex_ : startFrameIndex_;

	// check whether each frame has a (at least rough) valid pose
	bool fullRoughPoses = true;
	for (unsigned int n = lowerFrameIndex_; n <= upperFrameIndex_; ++n)
	{
		if (!offlinePoses_[n].isValid())
		{
			fullRoughPoses = false;
			break;
		}
	}

	ScopedEventStackLayer scopedEventLayer(*this, Scalar(0), Scalar(0.33));

	// check whether no rough poses are known for the video sequence
	if (!fullRoughPoses)
	{
		ScopedEventStackLayer localScopedEventLayer(*this, Scalar(0), Scalar(0.33));

		if (optimizeCamera_)
		{
			// determine a rough camera field of view
			if (!determineRoughCameraFov(lowerFrameIndex_, startFrameIndex, upperFrameIndex_, 10u) || shouldThreadStop())
			{
				return false;
			}
		}

		localScopedEventLayer.modify(Scalar(0.33), Scalar(0.66));

		// start to create a rough tracking result
		if (!determineRoughPoses(lowerFrameIndex_, startFrameIndex, upperFrameIndex_) || shouldThreadStop())
		{
			return false;
		}

		Variance correspondenceVariance;
		for (size_t n = lowerFrameIndex_; n <= upperFrameIndex_; ++n)
		{
			correspondenceVariance.add(offlinePoses_[n].quality());
		}
		const unsigned int minimalCorrespondences = max(20u, (unsigned int)correspondenceVariance.average() * 85u / 100u);

		localScopedEventLayer.modify(Scalar(0.66), Scalar(1.0));

		// close the tracking gaps that are due to bad visibility conditions etc.
		if (!closeGaps(lowerFrameIndex_, upperFrameIndex_, minimalCorrespondences) || shouldThreadStop())
		{
			return false;
		}
	}

	scopedEventLayer.modify(Scalar(0.33), Scalar(0.66));

	// start a first fine adjustment of the tracking poses and finally optimize the camera profile
	if (!fineAdjustment(2u, 10u, 40u, 40u, true, lowerFrameIndex_, startFrameIndex, upperFrameIndex_))
	{
		return false;
	}

	scopedEventLayer.modify(Scalar(0.66), Scalar(1.0));

	// apply a high accurate final tracking iteration
	if (!fineAdjustment(2u, 10u, 10u, 10u, false, lowerFrameIndex_, startFrameIndex, upperFrameIndex_))
	{
		return false;
	}

	removeIrregularPoses(Scalar(5));
	extrapolatePoses(15u, 25u);
	removeIrregularPoses(Scalar(5));

	// update the poses and provide an event
	updatePoses(offlinePoses_);

	return true;
}

bool PatternTracker::determineRoughCameraFov(const unsigned int lowerFrameIndex, const unsigned int initialFrameIndex, const unsigned int upperFrameIndex, const unsigned int cameraFovDeterminationFrames)
{
	ocean_assert(cameraFovDeterminationFrames != 0u);

	const AnalysisComponentEvent startEvent(id(), ComponentEvent::CS_STARTED);
	AnalysisComponentEvent breakEvent(id(), ComponentEvent::CS_STARTED);
	AnalysisComponentEvent finishEvent(id(), ComponentEvent::CS_STARTED);

	Scalar roughFovX = camera_.fovX();

	if (!RoughCameraFovComponent(*this, camera_, roughFovX, cameraFovDeterminationFrames).invoke(startEvent, breakEvent, finishEvent, lowerFrameIndex, initialFrameIndex, upperFrameIndex))
	{
		return false;
	}

	updateCamera(PinholeCamera(camera_.width(), camera_.height(), roughFovX));
	return true;
}

bool PatternTracker::determineRoughPoses(const unsigned int lowerFrameIndex, const unsigned int initialFrameIndex, const unsigned int upperFrameIndex)
{
	const TrackingComponentEvent startEvent(id(), ComponentEvent::CS_STARTED);
	TrackingComponentEvent breakEvent(id(), ComponentEvent::CS_FAILED);
	TrackingComponentEvent finishEvent(id(), ComponentEvent::CS_FINISHED);

	OfflinePoses poses;

	if (!RoughPoseComponent(*this, camera_, poses).invoke(startEvent, breakEvent, finishEvent, lowerFrameIndex, initialFrameIndex, upperFrameIndex))
	{
		return false;
	}

	updatePoses(poses);
	return true;
}

bool PatternTracker::closeGaps(const unsigned int lowerFrameIndex, const unsigned int upperFrameIndex, const unsigned int minimalCorrespondences)
{
	const TrackingComponentEvent startEvent(id(), TrackingComponentEvent::CS_STARTED);
	TrackingComponentEvent breakEvent(id(), TrackingComponentEvent::CS_FAILED);
	TrackingComponentEvent finishedEvent(id(), TrackingComponentEvent::CS_FINISHED);
	const ScopedEvent scopedEvent(startEvent, breakEvent, finishedEvent, eventCallbacks_);

	ocean_assert(frameProviderInterface_ && camera_);
	ocean_assert(minimalCorrespondences >= 3u);

	ocean_assert(patternDimension_.x() > 0 && patternDimension_.y() > 0);

	const unsigned int pyramidLayers = CV::FramePyramid::idealLayers(camera_.width(), camera_.height(), 60u, 60u, 2u, 64u);
	if (pyramidLayers == 0u)
	{
		return false;
	}

	// determine the number of gaps
	unsigned int numberGaps = 0u;
	for (size_t n = lowerFrameIndex; n <= upperFrameIndex; ++n)
	{
		if (offlinePoses_[n].quality() < Scalar(minimalCorrespondences))
		{
			numberGaps++;
		}
	}

	if (numberGaps == 0u)
	{
		scopedEvent.leave();
		return true;
	}

	// this vector will holds frame for that a gap closing iteration failed so that it does not need to be handled again
	ShiftVector<unsigned char> failedToClose(offlinePoses_.firstIndex(), offlinePoses_.size(), 0u);

	unsigned int closedGaps = 0u;

	Frame zippedFrame;
	CV::FramePyramid leftPyramid, centerPyramid, rightPyramid;

	bool oneGapClosed = true;

	while (oneGapClosed)
	{
		bool lastUpdatedLeft = false;
		oneGapClosed = false;

		for (size_t frameIndex = lowerFrameIndex; frameIndex <= upperFrameIndex; ++frameIndex)
		{
			if (offlinePoses_[frameIndex].quality() < Scalar(minimalCorrespondences) && failedToClose[frameIndex] == 0)
			{
				bool validLeft = frameIndex > lowerFrameIndex && offlinePoses_[frameIndex - 1].quality() >= Scalar(minimalCorrespondences);
				bool validRight = frameIndex + 1 <= upperFrameIndex && offlinePoses_[frameIndex + 1].quality() >= Scalar(minimalCorrespondences);

				// we need at least one neighbor with valid pose
				if (!validLeft && !validRight)
				{
					continue;
				}

				// avoid that consecutive left frames are handled, we need an equally distributed shrinking approach
				if (lastUpdatedLeft && validLeft && !validRight)
				{
					lastUpdatedLeft = false;
					continue;
				}

				lastUpdatedLeft = false;

				// create the frame pyramid for the center frame
				const FrameRef centerFrame = frameProviderInterface_->synchronFrameRequest((unsigned int)frameIndex, 10.0, &shouldStop_);
				if (centerFrame.isNull())
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				const FrameType::PixelFormat zippedPixelFormat = FrameType::genericSinglePlanePixelFormat(centerFrame->pixelFormat());
				const WorkerPool::ScopedWorker scopedWorker(WorkerPool::get().scopedWorker());

				if (!CV::FrameConverter::Comfort::convert(*centerFrame, zippedPixelFormat, FrameType::ORIGIN_UPPER_LEFT, zippedFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, scopedWorker()))
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				if (!centerPyramid.replace(zippedFrame, CV::FramePyramid::DM_FILTER_14641, pyramidLayers, true /*copyFirstLayer*/, scopedWorker()))
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				// create left frame pyramid (if needed)
				if (validLeft)
				{
					const FrameRef leftFrame = frameProviderInterface_->synchronFrameRequest((unsigned int)frameIndex - 1u, 10.0, &shouldStop_);
					if (leftFrame.isNull())
					{
						ocean_assert(false && "This should never happen!");
						return false;
					}

					if (!CV::FrameConverter::Comfort::convert(*leftFrame, zippedPixelFormat, FrameType::ORIGIN_UPPER_LEFT, zippedFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, scopedWorker()))
					{
						ocean_assert(false && "This should never happen!");
						return false;
					}

					if (!leftPyramid.replace(zippedFrame, CV::FramePyramid::DM_FILTER_14641, pyramidLayers, true /*copyFirstLayer*/, scopedWorker()))
					{
						ocean_assert(false && "This should never happen!");
						return false;
					}
				}

				// create right frame pyramid (if needed)
				if (validRight)
				{
					const FrameRef rightFrame = frameProviderInterface_->synchronFrameRequest((unsigned int)frameIndex + 1u, 10.0, &shouldStop_);
					if (rightFrame.isNull())
					{
						ocean_assert(false && "This should never happen!");
						return false;
					}

					if (!CV::FrameConverter::Comfort::convert(*rightFrame, zippedPixelFormat, FrameType::ORIGIN_UPPER_LEFT, zippedFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, scopedWorker()))
					{
						ocean_assert(false && "This should never happen!");
						return false;
					}

					if (!rightPyramid.replace(zippedFrame, CV::FramePyramid::DM_FILTER_14641, pyramidLayers, true /*copyFirstLayer*/, scopedWorker()))
					{
						ocean_assert(false && "This should never happen!");
						return false;
					}
				}

				HomogenousMatrix4 improvedLeftPose, improvedRightPose;

				if (validLeft)
				{
					ocean_assert(leftPyramid && centerPyramid);

					const Triangles2 projectedTriangles(projectedPatternTriangles(offlinePoses_[frameIndex - 1].transformation(), camera_));
					if (Frame2FrameTracker::trackPlanarObject<15u>(camera_, leftPyramid, centerPyramid, 4u, offlinePoses_[frameIndex - 1].transformation(), Plane3(Vector3(0, 1, 0), 0), CV::SubRegion(projectedTriangles), improvedLeftPose, Scalar(0.9 * 0.9), 20u, 20u, 30u, scopedWorker()) < 25)
					{
						if (Frame2FrameTracker::trackPlanarObject<15u>(camera_, leftPyramid, centerPyramid, 4u, offlinePoses_[frameIndex - 1].transformation(), Plane3(Vector3(0, 1, 0), 0), CV::SubRegion(projectedTriangles), improvedLeftPose, Scalar(1.9 * 1.9), 0u, 0u, 0u, scopedWorker()) < 10)
						{
							// the left pose could not be used
							validLeft = false;
						}
					}
				}

				if (validRight)
				{
					ocean_assert(rightPyramid && centerPyramid);

					const Triangles2 projectedTriangles(projectedPatternTriangles(offlinePoses_[frameIndex + 1].transformation(), camera_));
					if (Frame2FrameTracker::trackPlanarObject<15u>(camera_, rightPyramid, centerPyramid, 4u, offlinePoses_[frameIndex + 1].transformation(), Plane3(Vector3(0, 1, 0), 0), CV::SubRegion(projectedTriangles), improvedRightPose, Scalar(0.9 * 0.9), 20u, 20u, 30u, scopedWorker()) < 25)
					{
						if (Frame2FrameTracker::trackPlanarObject<15u>(camera_, rightPyramid, centerPyramid, 4u, offlinePoses_[frameIndex + 1].transformation(), Plane3(Vector3(0, 1, 0), 0), CV::SubRegion(projectedTriangles), improvedRightPose, Scalar(1.9 * 1.9), 0u, 0u, 0u, scopedWorker()) < 10)
						{
							// the right pose could not be used
							validRight = false;
						}
					}
				}

				if (validLeft && validRight)
				{
					ocean_assert(improvedLeftPose.isValid() && improvedRightPose.isValid());

					const Quaternion leftQuaternion(improvedLeftPose.rotation());
					const Quaternion rightQuaternion(improvedRightPose.rotation());

					const Vector3 leftTranslation(improvedLeftPose.translation());
					const Vector3 rightTranslation(improvedRightPose.translation());

					offlinePoses_[frameIndex].setTransformation(HomogenousMatrix4((leftTranslation + rightTranslation) * Scalar(0.5), leftQuaternion.slerp(rightQuaternion, Scalar(0.5))));

					ocean_assert(offlinePoses_[frameIndex].id() == (unsigned int)(-1) || offlinePoses_[frameIndex].id() == (unsigned int)frameIndex);
					offlinePoses_[frameIndex].setId((unsigned int)frameIndex);

					// sets the minimal number of correspondences so that the new pose counts as valid pose and can be used in sucessive gap closing iterations
					offlinePoses_[frameIndex].setQuality(Scalar(minimalCorrespondences));

					oneGapClosed = true;
				}
				else if (validLeft)
				{
					ocean_assert(improvedLeftPose.isValid());
					ocean_assert(!validRight);

					offlinePoses_[frameIndex].setTransformation(improvedLeftPose);

					ocean_assert(offlinePoses_[frameIndex].id() == (unsigned int)(-1) || offlinePoses_[frameIndex].id() == (unsigned int)frameIndex);
					offlinePoses_[frameIndex].setId((unsigned int)frameIndex);

					// sets the minimal number of correspondences so that the new pose counts as valid pose and can be used in sucessive gap closing iterations
					offlinePoses_[frameIndex].setQuality(Scalar(minimalCorrespondences));

					lastUpdatedLeft = true;

					oneGapClosed = true;
				}
				else if (validRight)
				{
					ocean_assert(improvedRightPose.isValid());
					ocean_assert(!validLeft);

					offlinePoses_[frameIndex].setTransformation(improvedRightPose);

					ocean_assert(offlinePoses_[frameIndex].id() == (unsigned int)(-1) || offlinePoses_[frameIndex].id() == (unsigned int)frameIndex);
					offlinePoses_[frameIndex].setId((unsigned int)frameIndex);

					// sets the minimal number of correspondences so that the new pose counts as valid pose and can be used in sucessive gap closing iterations
					offlinePoses_[frameIndex].setQuality(Scalar(minimalCorrespondences));

					oneGapClosed = true;
				}
				else
				{
					// neither the left nor the right pose could be use for gap closing, thus the tracking pattern seems to be invisible in this frame
					failedToClose[frameIndex] = 1u;
				}

				closedGaps++;

				eventCallbacks_(TrackingProgressEvent(id(), closedGaps * 100u / numberGaps));
				updateTrackerProgress(Scalar(closedGaps) / Scalar(numberGaps));
			}
		}
	}

	scopedEvent.leave();
	return true;
}

bool PatternTracker::fineAdjustment(const unsigned iterations, const unsigned int detectionBorder, const unsigned int horizontalBinSize, const unsigned int verticalBinSize, const bool optimizeCamera, const unsigned int lowerFrameIndex, const unsigned int initialFrameIndex, const unsigned int upperFrameIndex)
{
	const TrackingComponentEvent startEvent(id(), TrackingComponentEvent::CS_STARTED);
	TrackingComponentEvent breakEvent(id(), TrackingComponentEvent::CS_FAILED);
	TrackingComponentEvent finishedEvent(id(), TrackingComponentEvent::CS_FINISHED);

	OfflinePoses optimizedPoses;

	if (optimizeCamera)
	{
		PinholeCamera optimizedCamera;

		if (!FineTrackingComponent(*this, camera_, offlinePoses_, iterations, detectionBorder, horizontalBinSize, verticalBinSize, optimizedPoses, &optimizedCamera).invoke(startEvent, breakEvent, finishedEvent, lowerFrameIndex, initialFrameIndex, upperFrameIndex))
		{
			return false;
		}

		updateCamera(optimizedCamera);
	}
	else
	{
		if (!FineTrackingComponent(*this, camera_, offlinePoses_, iterations, detectionBorder, horizontalBinSize, verticalBinSize, optimizedPoses, nullptr).invoke(startEvent, breakEvent, finishedEvent, lowerFrameIndex, initialFrameIndex, upperFrameIndex))
		{
			return false;
		}
	}

	updatePoses(optimizedPoses);
	return true;
}

Scalar PatternTracker::projectedPatternArea(const HomogenousMatrix4& pose, const PinholeCamera& pinholeCamera) const
{
	ocean_assert(pose.isValid() && pinholeCamera.isValid());

	const Triangle3 triangle0(patternCorners_[0], patternCorners_[1], patternCorners_[2]);
	const Triangle3 triangle1(patternCorners_[0], patternCorners_[2], patternCorners_[3]);

	const HomogenousMatrix4 poseIF(PinholeCamera::standard2InvertedFlipped(pose));
	return pinholeCamera.projectToImageIF<true>(poseIF, triangle0, true).area() + pinholeCamera.projectToImageIF<true>(poseIF, triangle1, true).area();
}

Triangles2 PatternTracker::projectedPatternTriangles(const HomogenousMatrix4& pose, const PinholeCamera& pinholeCamera) const
{
	Triangles2 projectedTriangles(2);

	const Triangle3 triangle0(patternCorners_[0], patternCorners_[1], patternCorners_[2]);
	const Triangle3 triangle1(patternCorners_[0], patternCorners_[2], patternCorners_[3]);

	const HomogenousMatrix4 poseIF(PinholeCamera::standard2InvertedFlipped(pose));
	projectedTriangles[0] = pinholeCamera.projectToImageIF<true>(poseIF, triangle0, true);
	projectedTriangles[1] = pinholeCamera.projectToImageIF<true>(poseIF, triangle1, true);

	return projectedTriangles;
}

bool PatternTracker::updatePatternDimension(const Vector2& dimension)
{
	ocean_assert(dimension.x() > 0 && dimension.y() > 0);

	const ScopedLock scopedLock(lock_);

	patternDimension_ = dimension;

	// this tracker creates poses in relation to the detected pattern
	// the world coordinate system of this tracker (and therefore the global world coordinate system) is located at the upper left position of the detected pattern with y-axis perpendicular to the pattern

	const Vector3 translation(patternDimension_.x() * Scalar(0.5), 0, patternDimension_.y() * Scalar(0.5));
	const Rotation rotation(1, 0, 0, -Numeric::pi_2());

	objectTransformation_ = HomogenousMatrix4(translation, rotation);

	eventCallbacks_(TrackerTransformationStateEvent(id_, objectTransformation_));
	return true;
}

}

}

}
