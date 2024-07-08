/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/oculustags/OculusTagTracker.h"

#include "ocean/cv/advanced/AdvancedMotion.h"

#include "ocean/cv/detector/LineDetectorULF.h"

#include "ocean/geometry/EpipolarGeometry.h"
#include "ocean/geometry/NonLinearOptimizationPose.h"
#include "ocean/geometry/NonLinearOptimizationTransformation.h"
#include "ocean/geometry/RANSAC.h"

#include "ocean/tracking/oculustags/OculusTagDebugElements.h"
#include "ocean/tracking/oculustags/Utilities.h"

#if defined(OCN_OCULUSTAG_DEBUGGING_ENABLED)
	#include "ocean/cv/Canvas.h"
	#include "ocean/cv/detector/Utilities.h"

	#if defined(_WINDOWS)
		#include "ocean/platform/win/Utilities.h"
	#endif
#endif

namespace Ocean
{

namespace Tracking
{

namespace OculusTags
{

size_t OculusTagTracker::TagObservationHistory::removeObservations(const AnyCamera& anyCamera, const HomogenousMatrix4& tag_T_world, const Scalar maximalError, const Scalar maximalOutliersPercent)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(tag_T_world.isValid());
	ocean_assert(maximalError >= 0);
	ocean_assert(maximalOutliersPercent >= 0 && maximalOutliersPercent <= 1);

	if (size() == 0)
	{
		return 0;
	}

	const Scalar maximalSqrError = Numeric::sqr(maximalError);

	// Starting with the newest observation iterate in reverse order until the first observation is found that does not match the given pose.

	size_t firstViolatingObservationIndex = size_t(-1);

	for (size_t n = cameraPoses_world_T_camera_.size() - 1; firstViolatingObservationIndex == size_t(-1) && n < cameraPoses_world_T_camera_.size(); --n)
	{
		const Vectors3& testObjectPoints = objectPointsGroups_[n];
		const Vectors2& testImagePoints = imagePointsGroups_[n];

		const HomogenousMatrix4& world_T_camera = cameraPoses_world_T_camera_[n];
		const HomogenousMatrix4 tag_T_camera = tag_T_world * world_T_camera;

		const HomogenousMatrix4 flippedCamera_T_tag = AnyCamera::standard2InvertedFlipped(tag_T_camera);

		const size_t maximalOutliers = size_t(Scalar(testObjectPoints.size()) * maximalOutliersPercent + Scalar(0.5));

		size_t outliers = 0;

		for (size_t i = 0; i < testObjectPoints.size(); ++i)
		{
			const Vector3& objectPoint = testObjectPoints[i];
			const Vector2& testImagePoint = testImagePoints[i];

			const Vector2 projectedObjectPoint = anyCamera.projectToImageIF(flippedCamera_T_tag, objectPoint);

			const Scalar sqrError = testImagePoint.sqrDistance(projectedObjectPoint);

			if (sqrError > maximalSqrError)
			{
				outliers++;
			}

			if (outliers > maximalOutliers)
			{
				firstViolatingObservationIndex = n;
				break;
			}
		}
	}

	if (firstViolatingObservationIndex >= size())
	{
		return 0;
	}

	// Retain only those elements that come after the one found above, remove all others

	const size_t removedElements = firstViolatingObservationIndex + 1;

	cameraPoses_world_T_camera_.erase(cameraPoses_world_T_camera_.begin(), cameraPoses_world_T_camera_.begin() + removedElements);
	objectPointsGroups_.erase(objectPointsGroups_.begin(), objectPointsGroups_.begin() + removedElements);
	imagePointsGroups_.erase(imagePointsGroups_.begin(), imagePointsGroups_.begin() + removedElements);
	trackingImagePointsGroups_.erase(trackingImagePointsGroups_.begin(), trackingImagePointsGroups_.begin() + removedElements);
	trackingObjectPointsGroups_.erase(trackingObjectPointsGroups_.begin(), trackingObjectPointsGroups_.begin() + removedElements);

	return removedElements;
}

bool OculusTagTracker::TagObservationHistory::optimizePose(const AnyCamera& anyCamera, const HomogenousMatrix4& unoptimized_world_T_tag, HomogenousMatrix4& optimized_world_T_tag)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(unoptimized_world_T_tag.isValid());

	return Geometry::NonLinearOptimizationTransformation::optimizeObjectTransformation(anyCamera, cameraPoses_world_T_camera_, unoptimized_world_T_tag, objectPointsGroups_, imagePointsGroups_, optimized_world_T_tag, 50u, Geometry::Estimator::ET_SQUARE, Scalar(0.001), Scalar(5));
}

bool OculusTagTracker::TagObservationHistory::optimizePose(const AnyCamera& anyCameraA, const AnyCamera& anyCameraB, const TagObservationHistory& tagObservationHistoryA, const TagObservationHistory& tagObservationHistoryB, const HomogenousMatrix4& unoptimized_world_T_tag, HomogenousMatrix4& optimized_world_T_tag)
{
	ocean_assert(anyCameraA.isValid() && anyCameraB.isValid());
	ocean_assert(tagObservationHistoryA.empty() == false && tagObservationHistoryB.empty() == false);
	ocean_assert(unoptimized_world_T_tag.isValid());

	return Geometry::NonLinearOptimizationTransformation::optimizeObjectTransformationStereo(anyCameraA, anyCameraB, tagObservationHistoryA.cameraPoses_world_T_camera(), tagObservationHistoryB.cameraPoses_world_T_camera(), unoptimized_world_T_tag, tagObservationHistoryA.objectPointsGroups(), tagObservationHistoryB.objectPointsGroups(), tagObservationHistoryA.imagePointsGroups(), tagObservationHistoryB.imagePointsGroups(), optimized_world_T_tag, 50u, Geometry::Estimator::ET_SQUARE, Scalar(0.001), Scalar(5));
}

OculusTagTracker::OculusTagTracker() :
	frameCounter_(0u)
{
	// Nothing else todo
}

OculusTagTracker::OculusTagTracker(OculusTagTracker&& oculusTagTracker)
{
	*this = std::move(oculusTagTracker);
}

OculusTagTracker::~OculusTagTracker()
{
	// Nothing else to do
}

OculusTagTracker& OculusTagTracker::operator=(OculusTagTracker&& otherTracker)
{
	if (this != &otherTracker)
	{
		frameCounter_ = otherTracker.frameCounter_;
		trackedTagMap_ = std::move(otherTracker.trackedTagMap_);

		for (unsigned int cameraIndex = 0u; cameraIndex < 2u; ++cameraIndex)
		{
			previousYFrames_[cameraIndex] = std::move(otherTracker.previousYFrames_[cameraIndex]);
			previousFramePyramids_[cameraIndex] = std::move(otherTracker.previousFramePyramids_[cameraIndex]);

			previous_world_T_device_[cameraIndex] = std::move(otherTracker.previous_world_T_device_[cameraIndex]);
			previousDevice_T_cameras_[cameraIndex] = std::move(otherTracker.previousDevice_T_cameras_[cameraIndex]);
		}
	}

	return *this;
}

bool OculusTagTracker::trackTagsStereo(const AnyCamera& anyCameraA, const AnyCamera& anyCameraB, const Frame& yFrameA, const Frame& yFrameB, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_cameraA, const HomogenousMatrix4& device_T_cameraB, OculusTags& tags)
{
	static_assert(numberRequiredObservationForStatic_ != 0u, "Minimum number of required observation must be in range: [1, infinity)");
	static_assert(numberMaxAllowedObservations_ != 0u, "Maximum number of observations per tag must be in range: [1, infinity)");
	static_assert(detectionCadence_ != 0u, "Detection refresh interval must be in range: [1, infinity)");
	static_assert(maxAllowedProjectionError_ >= 0, "Maximum projection error for tag observation must be in range: [0, infinity)");
	static_assert(numberFrameLayers_ != 0u, "Number of frame pyramid layers must be in range: [1, infinity)");

	// TODO Use this in the function signature
	const SharedAnyCameras anyCameras = { anyCameraA.clone(), anyCameraB.clone() };
	const Frames yFrames = { Frame(yFrameA, Frame::ACM_USE_KEEP_LAYOUT), Frame(yFrameB, Frame::ACM_USE_KEEP_LAYOUT) };
	const HomogenousMatrices4 device_T_cameras = { device_T_cameraA, device_T_cameraB };

	ocean_assert(world_T_device.isValid());

#if defined(OCEAN_DEBUG)
	for (size_t cameraIndex = 0; cameraIndex < 2; ++cameraIndex)
	{
		ocean_assert(anyCameras[cameraIndex] && anyCameras[cameraIndex]->isValid());

		ocean_assert(yFrames[cameraIndex].isValid());
		ocean_assert(FrameType::arePixelFormatsCompatible(yFrames[cameraIndex].pixelFormat(), FrameType::FORMAT_Y8));
		ocean_assert(yFrames[cameraIndex].pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT);

		ocean_assert(device_T_cameras[cameraIndex].isValid());
	}
#endif

	std::unordered_set<uint32_t> visibleTagsIndices;

	// Tracking

	CV::FramePyramid framePyramids[2] =
	{
		createFramePyramid(yFrames[0], numberFrameLayers_),
		createFramePyramid(yFrames[1], numberFrameLayers_),
	};

	ocean_assert(framePyramids[0].layers() == numberFrameLayers_ && framePyramids[0].layers() == numberFrameLayers_);

	ocean_assert(frameCounter_ == 0u || (previousYFrames_[0].isValid() && previousYFrames_[1].isValid()));
	ocean_assert(frameCounter_ == 0u || (previousFramePyramids_[0].isValid() && previousFramePyramids_[1].isValid()));

	for (TrackedTagMap::value_type& trackedTagIter : trackedTagMap_)
	{
		TrackedTag& trackedTag = trackedTagIter.second;
		ocean_assert(trackedTagIter.first == trackedTag.tag_.tagID());

		if (trackedTag.trackingState_ != TS_TRACKING && trackedTag.trackingState_ != TS_NEW_DETECTION)
		{
			continue;
		}

		bool isTagTracked[2] = { false, false };

		OculusTag currentTags[2];

		QuadDetector::Quad quads[2];

		Vectors2 trackedCornerGroups[2];

		for (size_t cameraIndex = 0; cameraIndex < 2; ++cameraIndex)
		{
			const Frame& yFrame = yFrames[cameraIndex];

			const SharedAnyCamera& anyCamera = anyCameras[cameraIndex];
			const HomogenousMatrix4& device_T_camera = device_T_cameras[cameraIndex];

			const CV::FramePyramid& framePyramid = framePyramids[cameraIndex];
			const CV::FramePyramid& previousFramePyramid = previousFramePyramids_[cameraIndex];

			TagObservationHistory& tagObservationHistory = cameraIndex == 0 ? trackedTag.tagObservationHistoryA_ : trackedTag.tagObservationHistoryB_;
			OculusTag& tag = currentTags[cameraIndex];

			QuadDetector::Quad& quad = quads[cameraIndex];

			Vectors2& trackedCorners = trackedCornerGroups[cameraIndex];

			if (!tagObservationHistory.empty() && isTagVisible(*anyCamera, trackedTag.tag_.world_T_tag().inverted() * world_T_device * device_T_camera, trackedTag.tag_.tagSize(), frameBorder_))
			{
				const Vectors2& trackingImagePoints = tagObservationHistory.latestTrackingImagePoints();
				const Vectors3& trackingObjectPoints = tagObservationHistory.latestTrackingObjectPoints();
				ocean_assert(trackingImagePoints.size() == trackingObjectPoints.size());

				if (trackTagCornersTemporally(*anyCamera, framePyramid, previousFramePyramid, world_T_device * device_T_camera, previous_world_T_device_ * world_T_device.inverted() * trackedTag.tag_.world_T_tag(), trackingObjectPoints, trackingImagePoints, trackedCorners))
				{
					ocean_assert(trackedCorners.size() >= 4);

					QuadDetector::Quad initialQuad;
					bool haveInitialQuad = true;

					for (size_t i = 0; i < 4; ++i)
					{
						if (Utilities::refineCorner(yFrame, trackedCorners[i], 3u) == false)
						{
							haveInitialQuad = false;
							break;
						}

						initialQuad[i] = trackedCorners[i];
					}

					if (haveInitialQuad)
					{
						if (readTag(*anyCamera, yFrame, initialQuad, world_T_device, device_T_camera, trackedTag.tag_.tagSize(), tag, quad, TagSizeMap()))
						{
							TagObservationHistory newTagObservationHistory;
							if (addTagObservationAndOptimize(*anyCamera, yFrame, world_T_device, device_T_camera, tag, quad, newTagObservationHistory))
							{
								const size_t removedObservations = tagObservationHistory.removeObservations(*anyCamera, tag.world_T_tag().inverted(), maxAllowedProjectionError_);
								OCEAN_SUPPRESS_UNUSED_WARNING(removedObservations);

								tagObservationHistory.append(newTagObservationHistory);

								isTagTracked[cameraIndex] = true;
								ocean_assert(tagObservationHistory.size() != 0);
							}
						}
					}
				}
			}
		}

		// Invalidate the tracking state and then update it again accordingly

		trackedTag.trackingState_ = TS_NOT_TRACKING;
		trackedTag.motionType_ = MT_UNKNOWN;

		if (isTagTracked[0] || isTagTracked[1])
		{
			if (isTagTracked[0] && isTagTracked[1])
			{
				ocean_assert(currentTags[0].isValid() && currentTags[1].isValid());

				const HomogenousMatrix4 unoptimized_world_T_tag = currentTags[0].world_T_tag();
				HomogenousMatrix4 optimized_world_T_tag;

				if (TagObservationHistory::optimizePose(anyCameraA, anyCameraB, trackedTag.tagObservationHistoryA_, trackedTag.tagObservationHistoryB_, unoptimized_world_T_tag, optimized_world_T_tag))
				{
					currentTags[0].setWorld_T_tag(optimized_world_T_tag);
				}
			}

			if (currentTags[0].isValid())
			{
				trackedTag.tag_ = std::move(currentTags[0]);
			}
			else
			{
				trackedTag.tag_ = std::move(currentTags[1]);
			}
			ocean_assert(trackedTag.tag_.isValid());

			trackedTag.trackingState_ = TS_TRACKING;
			trackedTag.motionType_ = std::max(trackedTag.tagObservationHistoryA_.size(), trackedTag.tagObservationHistoryB_.size()) < numberRequiredObservationForStatic_ ? MT_DYNAMIC : MT_STATIC;

			visibleTagsIndices.insert(trackedTag.tag_.tagID());
		}

		ocean_assert(trackedTag.trackingState_ != TS_TRACKING || trackedTag.tagObservationHistoryA_.size() != 0  || trackedTag.tagObservationHistoryB_.size() != 0);
	}


	// Detection

	if (visibleTagsIndices.empty() || frameCounter_ % detectionCadence_ == 0u)
	{
		TrackedTags detectedTags = detectTagsStereo(anyCameras, yFrames, world_T_device, device_T_cameras);

		for (size_t t = 0; t < detectedTags.size(); ++t)
		{
			ocean_assert(detectedTags[t].tagObservationHistoryA_.size() == 1 || detectedTags[t].tagObservationHistoryB_.size() == 1);

			TrackedTagMap::iterator iterTrackedTag = trackedTagMap_.find(detectedTags[t].tag_.tagID());

			if (iterTrackedTag == trackedTagMap_.end())
			{
				trackedTagMap_.emplace(std::piecewise_construct, std::forward_as_tuple(detectedTags[t].tag_.tagID()), std::forward_as_tuple(detectedTags[t]));
				visibleTagsIndices.insert(detectedTags[t].tag_.tagID());
			}
			else
			{
				TrackedTag& trackedTag = iterTrackedTag->second;

				ocean_assert(trackedTag.tag_.tagID() == detectedTags[t].tag_.tagID());

				if (trackedTag.trackingState_ != TS_TRACKING)
				{
					trackedTag.tag_ = detectedTags[t].tag_;

					const HomogenousMatrix4 tag_T_world = trackedTag.tag_.world_T_tag().inverted();
					const size_t removedObservationsA = trackedTag.tagObservationHistoryA_.removeObservations(anyCameraA, tag_T_world, maxAllowedProjectionError_);
					const size_t removedObservationsB = trackedTag.tagObservationHistoryB_.removeObservations(anyCameraB, tag_T_world, maxAllowedProjectionError_);

					if (detectedTags[t].tagObservationHistoryA_.size() != 0 && trackedTag.tagObservationHistoryA_.size() <= numberMaxAllowedObservations_)
					{
						ocean_assert(detectedTags[t].tagObservationHistoryA_.size() == 1);
						trackedTag.tagObservationHistoryA_.append(detectedTags[t].tagObservationHistoryA_);
					}

					if (detectedTags[t].tagObservationHistoryB_.size() != 0 && trackedTag.tagObservationHistoryB_.size() < numberMaxAllowedObservations_)
					{
						ocean_assert(detectedTags[t].tagObservationHistoryB_.size() == 1);
						trackedTag.tagObservationHistoryB_.append(detectedTags[t].tagObservationHistoryB_);
					}

					trackedTag.trackingState_ = TS_NEW_DETECTION;
					trackedTag.motionType_ = removedObservationsA == 0 && removedObservationsB == 0 ? MT_STATIC : MT_DYNAMIC;

					visibleTagsIndices.insert(detectedTags[t].tag_.tagID());
				}
			}
		}
	}

	// Updates and optimizations

	OculusTags visibleTags;

	std::vector<uint32_t> sortedTagIndices(visibleTagsIndices.begin(), visibleTagsIndices.end());
	std::sort(sortedTagIndices.begin(), sortedTagIndices.end());

	for (const uint32_t tagID : sortedTagIndices)
	{
		TrackedTagMap::iterator iterTrackedTag = trackedTagMap_.find(tagID);
		ocean_assert(iterTrackedTag != trackedTagMap_.end());

		TrackedTag& trackedTag = iterTrackedTag->second;
		ocean_assert(trackedTag.tag_.isValid());

		visibleTags.emplace_back(trackedTag.tag_);

#if defined(OCN_OCULUSTAG_DEBUGGING_ENABLED)
		if (OculusTagDebugElements::get().isElementActive(OculusTagDebugElements::EI_TRACKER_RECTIFIED_TAG))
		{
			const OculusTagDebugElements::ScopedHierarchy scopedHierarchyTagID(String::toAString(trackedTag.tag_.tagID()));

			constexpr unsigned int rectifiedSize = 128u;

			Frame rectifiedYFrame;
			if (extractRectifiedTagImage(anyCameraA, anyCameraB, yFrameA, yFrameB, world_T_device, device_T_cameraA, device_T_cameraB, trackedTag.tag_, rectifiedYFrame, rectifiedSize) == false)
			{
				ocean_assert(false && "This should never happen!");
			}

			ocean_assert(rectifiedYFrame.isValid());

#if 1
			// Draw the corners of the tag as points

			Frame rectifiedRgbFrame;
			CV::FrameConverter::Comfort::convert(rectifiedYFrame, FrameType::FORMAT_RGB24, rectifiedRgbFrame);

			const HomogenousMatrix4 tag_T_cameraA = trackedTag.tag_.world_T_tag().inverted() * world_T_device * device_T_cameraA;
			const HomogenousMatrix4 flippedCameraA_T_tag = AnyCamera::standard2InvertedFlipped(tag_T_cameraA);

			OculusTag::DataMatrix dataMatrix;
			if (!Utilities::encode(trackedTag.tag_.tagID(), dataMatrix))
			{
				ocean_assert(false && "This should never happen!");
			}

			const Vectors3 cornerObjectPoints = getTagObjectPoints(TPG_CORNERS_ALL_AVAILABLE, trackedTag.tag_.tagSize(), dataMatrix);
			Vectors2 fisheyePoints;
			fisheyePoints.reserve(cornerObjectPoints.size());

			for (const Vector3& objectPoint : cornerObjectPoints)
			{
				fisheyePoints.emplace_back(anyCameraA.projectToImageIF(flippedCameraA_T_tag * objectPoint));
			}

			const Vectors3 tagObjectCorners = getTagObjectPoints(TPG_CORNERS_0_TO_3, trackedTag.tag_.tagSize());
			const Vectors2 rectifiedImageCorners =
			{
				Vector2(0, 0),
				Vector2(0, rectifiedRgbFrame.height()),
				Vector2(rectifiedRgbFrame.width(), rectifiedRgbFrame.height()),
				Vector2(rectifiedRgbFrame.width(), 0),
			};

			Vectors2 rectifiedPoints;
			Utilities::debugFisheyeImagePointsToRectifiedImagePoints(anyCameraA, tag_T_cameraA, tagObjectCorners, rectifiedImageCorners, fisheyePoints, rectifiedPoints);

			for (const Vector2& rectifiedPoint : rectifiedPoints)
			{
				CV::Canvas::point<7u>(rectifiedRgbFrame, rectifiedPoint, CV::Canvas::black(rectifiedRgbFrame.pixelFormat()));
				CV::Canvas::point<5u>(rectifiedRgbFrame, rectifiedPoint, CV::Canvas::red(rectifiedRgbFrame.pixelFormat()));
			}

			OculusTagDebugElements::get().updateElement(OculusTagDebugElements::EI_TRACKER_RECTIFIED_TAG, rectifiedRgbFrame);
#else
			OculusTagDebugElements::get().updateElement(OculusTagDebugElements::EI_TRACKER_RECTIFIED_TAG, std::move(rectifiedYFrame));
#endif
		}
#endif // OCN_OCULUSTAG_DEBUGGING_ENABLED
	}

	tags = std::move(visibleTags);

	++frameCounter_;

	previous_world_T_device_ = world_T_device;

	for (size_t cameraIndex = 0; cameraIndex < 2; ++ cameraIndex)
	{
		previousYFrames_[cameraIndex] =  Frame(yFrames[cameraIndex], Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);
		previousFramePyramids_[cameraIndex] = std::move(framePyramids[cameraIndex]);
		previousDevice_T_cameras_[cameraIndex] = device_T_cameras[cameraIndex];
	}

	return tags.empty() == false;
}

bool OculusTagTracker::extractRectifiedTagImage(const AnyCamera& anyCameraA, const AnyCamera& anyCameraB, const Frame& yFrameA, const Frame& yFrameB, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_cameraA, const HomogenousMatrix4& device_T_cameraB, const OculusTag& tag, Frame& rectifiedFrame, const uint32_t rectifiedFrameSize)
{
	ocean_assert(anyCameraA.isValid() && anyCameraB.isValid());
	ocean_assert(yFrameA.isValid() && yFrameB.isValid());
	ocean_assert(FrameType::arePixelFormatsCompatible(yFrameA.pixelFormat(), FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, 1u>()));
	ocean_assert(FrameType::arePixelFormatsCompatible(yFrameB.pixelFormat(), FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, 1u>()));
	ocean_assert(yFrameA.pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT);
	ocean_assert(yFrameB.pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT);
	ocean_assert(world_T_device.isValid() && device_T_cameraA.isValid() && device_T_cameraB.isValid());
	ocean_assert_and_suppress_unused(rectifiedFrameSize != 0u, rectifiedFrameSize);

	if (tag.isValid() == false)
	{
		return false;
	}

	const HomogenousMatrix4 tag_T_device = tag.world_T_tag().inverted() * world_T_device;

	const bool isVisibleInCameraA = isTagVisible(anyCameraA, tag_T_device * device_T_cameraA, tag.tagSize(), Scalar(frameBorder_));
	const bool isVisibleInCameraB = isTagVisible(anyCameraB, tag_T_device * device_T_cameraB, tag.tagSize(), Scalar(frameBorder_));

	if (isVisibleInCameraA == false && isVisibleInCameraB == false)
	{
		return false;
	}

	bool useFrameA = isVisibleInCameraA;

	if (isVisibleInCameraA && isVisibleInCameraB)
	{
		// If the tag is visible in both cameras, use that image for rectification in which the tag is closer to the image center.
		ocean_assert(yFrameA.frameType() == yFrameB.frameType());
		const Vector2 imageCenter(Scalar(0.5) * Scalar(yFrameA.width()), Scalar(0.5) * Scalar(yFrameA.height()));

		const HomogenousMatrix4 flippedCameraA_T_tag = AnyCamera::standard2InvertedFlipped(tag_T_device * device_T_cameraA);
		const HomogenousMatrix4 flippedCameraB_T_tag = AnyCamera::standard2InvertedFlipped(tag_T_device * device_T_cameraB);
		ocean_assert(flippedCameraA_T_tag.isValid() && flippedCameraB_T_tag.isValid());

		const Vector3 tagCenterPoint = Vector3(Scalar(0.5), Scalar(0.5), 0) * tag.tagSize();
		const Vector2 imagePointA = anyCameraA.projectToImageIF(flippedCameraA_T_tag * tagCenterPoint);
		const Vector2 imagePointB = anyCameraA.projectToImageIF(flippedCameraB_T_tag * tagCenterPoint);

		useFrameA = imageCenter.sqrDistance(imagePointA) < imageCenter.sqrDistance(imagePointB);
	}

	const AnyCamera& anyCamera = useFrameA ? anyCameraA : anyCameraB;
	const Frame& yFrame = useFrameA ? yFrameA : yFrameB;
	const HomogenousMatrix4& device_T_camera = useFrameA ? device_T_cameraA : device_T_cameraB;

	const bool rectificationSuccessful = Utilities::createRectifiedFrameFromFisheyeFrame<1u>(anyCamera, yFrame, world_T_device * device_T_camera, tag.world_T_tag(), tag.tagSize(), getTagObjectPoints(TPG_CORNERS_0_TO_3, tag.tagSize()), 128u, 128u, rectifiedFrame, /* extraBorder */ 0u);
	ocean_assert(rectificationSuccessful);

	return rectificationSuccessful;
}

OculusTags OculusTagTracker::detectTagsMono(const AnyCamera& anyCamera, const Frame& yFrame, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_camera, const Scalar defaultTagSize, const TagSizeMap& tagSizeMap, TagObservationHistories* tagObservationHistories)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(yFrame.isValid() && FrameType::arePixelFormatsCompatible(yFrame.pixelFormat(), FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, 1u>()));
	ocean_assert(yFrame.width() == anyCamera.width() && yFrame.height() == yFrame.height());
	ocean_assert(yFrame.width() > 2u * frameBorder_ && yFrame.height() > 2u * frameBorder_);
	ocean_assert(world_T_device.isNull() == false);
	ocean_assert(device_T_camera.isNull() == false);
	ocean_assert(defaultTagSize > 0);

	OculusTags tags;
	TagObservationHistories localTagObservationHistories;

	const QuadDetector::Quads candidateQuads = QuadDetector::detectQuads(yFrame, frameBorder_);

	for (const QuadDetector::Quad& candidateQuad : candidateQuads)
	{
		QuadDetector::Quad quad;
		OculusTag tag;
		TagObservationHistory tagObservationsHistory;

		if (readTag(anyCamera, yFrame, candidateQuad, world_T_device, device_T_camera, defaultTagSize, tag, quad, tagSizeMap))
		{
			ocean_assert(tag.isValid());

			if (addTagObservationAndOptimize(anyCamera, yFrame, world_T_device, device_T_camera, tag, quad, tagObservationsHistory))
			{
				ocean_assert(tag.isValid());
				ocean_assert(tagObservationsHistory.size() != 0);

				tags.emplace_back(std::move(tag));
				localTagObservationHistories.emplace_back(std::move(tagObservationsHistory));
			}
		}
	}

	if (tagObservationHistories)
	{
		*tagObservationHistories = std::move(localTagObservationHistories);
	}

	return tags;
}

bool OculusTagTracker::locateTagInCamera(const AnyCamera& anyCamera, const Frame& yFrame, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_camera, const OculusTag& tag, TagObservationHistory& tagObservationHistory)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(yFrame.isValid() && FrameType::arePixelFormatsCompatible(yFrame.pixelFormat(), FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, 1u>()));
	ocean_assert(yFrame.width() == anyCamera.width() && yFrame.height() == yFrame.height());
	ocean_assert(yFrame.width() > 2u * frameBorder_ && yFrame.height() > 2u * frameBorder_);
	ocean_assert(world_T_device.isNull() == false);
	ocean_assert(device_T_camera.isNull() == false);

	const HomogenousMatrix4 tag_T_camera1 = tag.world_T_tag().inverted() * world_T_device * device_T_camera;

	if (isTagVisible(anyCamera, tag_T_camera1, tag.tagSize(), frameBorder_))
	{
		const HomogenousMatrix4 flippedCamera1_T_tag = AnyCamera::standard2InvertedFlipped(tag_T_camera1);

		const Vectors3 outerCornerObjectPoints = getTagObjectPoints(TPG_CORNERS_0_TO_3, tag.tagSize());
		ocean_assert(outerCornerObjectPoints.size() == 4);
		QuadDetector::Quad predictedQuad1;
		bool havePredictedQuad1 = true;

		for (size_t i = 0; i < 4; ++i)
		{
			predictedQuad1[i] = anyCamera.projectToImageIF(flippedCamera1_T_tag * outerCornerObjectPoints[i]);
			ocean_assert(anyCamera.isInside(predictedQuad1[i], frameBorder_));

			if (Utilities::refineCorner(yFrame, predictedQuad1[i], 2u) == false)
			{
				havePredictedQuad1 = false;
				break;
			}
		}

		OculusTag stereoTag;
		QuadDetector::Quad quad;

		if (havePredictedQuad1 && readTag(anyCamera, yFrame, predictedQuad1, world_T_device, device_T_camera, tag.tagSize(), stereoTag, quad))
		{
			ocean_assert(stereoTag.isValid());
			ocean_assert(tag.tagSize() == stereoTag.tagSize());

			return addTagObservationAndOptimize(anyCamera, yFrame, world_T_device, device_T_camera, stereoTag, quad, tagObservationHistory) && tag.tagID() == stereoTag.tagID();
		}
	}

	return false;
}

Vectors3 OculusTagTracker::getTagObjectPoints(const TagPointGroup tagPointGroup, const Scalar tagSize, const OculusTag::DataMatrix dataMatrix)
{
	ocean_assert(tagSize > 0);

	const Scalar moduleSize = tagSize * Scalar(0.125);

	Vectors3 objectPoints;
	objectPoints.reserve(4 + 10 + 23);

	if (tagPointGroup & TPG_CORNERS_0_TO_3)
	{
		objectPoints.emplace_back(Scalar(0), Scalar(0), Scalar(0)); // 0
		objectPoints.emplace_back(Scalar(0), -tagSize, Scalar(0)); // 1
		objectPoints.emplace_back(tagSize, -tagSize, Scalar(0)); // 2
		objectPoints.emplace_back(tagSize, Scalar(0), Scalar(0)); // 3
	}

	bool excludeCorners69C = false;

	if (tagPointGroup & TPG_CORNERS_4_TO_D)
	{
		objectPoints.emplace_back(moduleSize * Scalar(1), -moduleSize * Scalar(1), Scalar(0)); // 4
		objectPoints.emplace_back(moduleSize * Scalar(1), -moduleSize * Scalar(6), Scalar(0)); // 5
		objectPoints.emplace_back(moduleSize * Scalar(2), -moduleSize * Scalar(6), Scalar(0)); // 6
		objectPoints.emplace_back(moduleSize * Scalar(2), -moduleSize * Scalar(7), Scalar(0)); // 7
		objectPoints.emplace_back(moduleSize * Scalar(6), -moduleSize * Scalar(7), Scalar(0)); // 8
		objectPoints.emplace_back(moduleSize * Scalar(6), -moduleSize * Scalar(6), Scalar(0)); // 9
		objectPoints.emplace_back(moduleSize * Scalar(7), -moduleSize * Scalar(6), Scalar(0)); // A
		objectPoints.emplace_back(moduleSize * Scalar(7), -moduleSize * Scalar(2), Scalar(0)); // B
		objectPoints.emplace_back(moduleSize * Scalar(6), -moduleSize * Scalar(2), Scalar(0)); // C
		objectPoints.emplace_back(moduleSize * Scalar(6), -moduleSize * Scalar(1), Scalar(0)); // D

		excludeCorners69C = true;
	}

	if (tagPointGroup & TPG_CORNERS_IN_DATA_MATRIX)
	{
		const std::vector<uint8_t> modules = OculusTagTracker::generateModuleValues(dataMatrix);

		const uint32_t columnStartExcluding69C[5] = { 2u, 2u, 2u, 2u, 3u };
		const uint32_t columnEndExcluding69C[5] = { 6u, 7u, 7u, 7u, 6u };

		const uint32_t columnStartIncluding69C[5] = { 2u, 2u, 2u, 2u, 2u };
		const uint32_t columnEndIncluding69C[5] = { 7u, 7u, 7u, 7u, 7u };

		const uint32_t* columnStart = excludeCorners69C ? columnStartExcluding69C : columnStartIncluding69C;
		const uint32_t* columnEnd = excludeCorners69C ? columnEndExcluding69C : columnEndIncluding69C;

		for (uint32_t y = 2u; y < (OculusTag::numberOfModules - 1u); ++y)
		{
			for (uint32_t x = columnStart[y - 2u]; x < columnEnd[y - 2u]; ++x)
			{
				const uint32_t indexTopLeft = (y - 1u) * OculusTag::numberOfModules + x - 1u;
				const uint32_t indexTop = indexTopLeft + 1u;
				const uint32_t indexLeft = y * OculusTag::numberOfModules + x - 1u;
				const uint32_t index = indexLeft + 1u;

				// Accept all of the following configurations of 2-by-2 module neighborhoods
				//
				// 10  01  00  00  10  01
				// 00  00  10  01  01  10
				//
				// (as well as the inverse of these)
				const uint8_t moduleValuesSum = modules[indexTopLeft] + modules[indexTop] + modules[indexLeft] + modules[index];
				ocean_assert(moduleValuesSum <= 4u);

				if (moduleValuesSum == 1u || moduleValuesSum == 3u || (moduleValuesSum == 2u && modules[indexTopLeft] == modules[index]))
				{
					objectPoints.emplace_back(moduleSize * Scalar(x), -moduleSize * Scalar(y), Scalar(0));
				}
			}
		}
	}

	if (tagPointGroup & TPG_DATA_MATRIX_MODULE_CENTERS)
	{
		// +---+---+---+---+---+---+---+---+
		// |   |   |   |   |   |   |   |   |
		// +---+---+---+---+---+---+---+---+
		// |   |   |   |   |   |   |   |   |
		// +---+---+---+---+---+---+---+---+
		// |   |   | 0 | 1 | 2 | 3 |   |   |
		// +---+---+---+---+---+---+---+---+
		// |   |   | 4 | 5 | 6 | 7 |   |   |
		// +---+---+---+---+---+---+---+---+
		// |   |   | 8 | 9 | 10| 11|   |   |
		// +---+---+---+---+---+---+---+---+
		// |   |   | 12| 13| 14| 15|   |   |
		// +---+---+---+---+---+---+---+---+
		// |   |   |   |   |   |   |   |   |
		// +---+---+---+---+---+---+---+---+
		// |   |   |   |   |   |   |   |   |
		// +---+---+---+---+---+---+---+---+
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(2.5), moduleSize * Scalar(-2.5), 0)); // 0
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(3.5), moduleSize * Scalar(-2.5), 0)); // 1
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(4.5), moduleSize * Scalar(-2.5), 0)); // 2
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(5.5), moduleSize * Scalar(-2.5), 0)); // 3
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(2.5), moduleSize * Scalar(-3.5), 0)); // 4
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(3.5), moduleSize * Scalar(-3.5), 0)); // 5
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(4.5), moduleSize * Scalar(-3.5), 0)); // 6
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(5.5), moduleSize * Scalar(-3.5), 0)); // 7
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(2.5), moduleSize * Scalar(-4.5), 0)); // 8
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(3.5), moduleSize * Scalar(-4.5), 0)); // 9
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(4.5), moduleSize * Scalar(-4.5), 0)); // 10
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(5.5), moduleSize * Scalar(-4.5), 0)); // 11
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(2.5), moduleSize * Scalar(-5.5), 0)); // 12
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(3.5), moduleSize * Scalar(-5.5), 0)); // 13
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(4.5), moduleSize * Scalar(-5.5), 0)); // 14
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(5.5), moduleSize * Scalar(-5.5), 0)); // 15
	}

	if (tagPointGroup & TPG_FOREGROUND_MODULE_CENTERS)
	{
		// +---+---+---+---+---+---+---+---+
		// | 0 | 19| 18| * | * | 17| 16| 15|
		// +---+---+---+---+---+---+---+---+
		// | 1 |   |   |   |   |   |   | 14|
		// +---+---+---+---+---+---+---+---+
		// | 2 |   |   |   |   |   |   | 13|
		// +---+---+---+---+---+---+---+---+
		// | * |   |   |   |   |   |   | * | <- modules marked with '*' are excluded because the Oculus
		// +---+---+---+---+---+---+---+---+    logo could be at these locations (i.e. their value may by
		// | * |   |   |   |   |   |   | * |    different from "pure" foreground modules)
		// +---+---+---+---+---+---+---+---+
		// | 3 |   |   |   |   |   |   | 12|
		// +---+---+---+---+---+---+---+---+
		// | 4 |   |   |   |   |   |   | 11|
		// +---+---+---+---+---+---+---+---+
		// | 5 | 6 | 7 | * | * | 8 | 9 | 10|
		// +---+---+---+---+---+---+---+---+
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(0.5), moduleSize * Scalar(-0.5), 0)); // 0
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(0.5), moduleSize * Scalar(-1.5), 0)); // 1
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(0.5), moduleSize * Scalar(-2.5), 0)); // 2
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(0.5), moduleSize * Scalar(-5.5), 0)); // 3
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(0.5), moduleSize * Scalar(-6.5), 0)); // 4
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(0.5), moduleSize * Scalar(-7.5), 0)); // 5
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(1.5), moduleSize * Scalar(-7.5), 0)); // 6
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(2.5), moduleSize * Scalar(-7.5), 0)); // 7
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(5.5), moduleSize * Scalar(-7.5), 0)); // 8
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(6.5), moduleSize * Scalar(-7.5), 0)); // 9
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(7.5), moduleSize * Scalar(-7.5), 0)); // 10
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(7.5), moduleSize * Scalar(-6.5), 0)); // 11
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(7.5), moduleSize * Scalar(-5.5), 0)); // 12
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(7.5), moduleSize * Scalar(-2.5), 0)); // 13
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(7.5), moduleSize * Scalar(-1.5), 0)); // 14
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(7.5), moduleSize * Scalar(-0.5), 0)); // 15
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(6.5), moduleSize * Scalar(-0.5), 0)); // 16
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(5.5), moduleSize * Scalar(-0.5), 0)); // 17
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(2.5), moduleSize * Scalar(-0.5), 0)); // 18
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(1.5), moduleSize * Scalar(-0.5), 0)); // 19
	}

	if (tagPointGroup & TPG_BACKGROUND_MODULE_CENTERS)
	{
		 // +---+---+---+---+---+---+---+---+
		 // |   |   |   |   |   |   |   |   |
		 // +---+---+---+---+---+---+---+---+
		 // |   | * | 15| 14| 13| 12| * |   | <- modules marked with '*' are excluded because out of
		 // +---+---+---+---+---+---+---+---+    these four module only one has a background while
		 // |   | 0 |   |   |   |   | 11|   |    other three have foreground values.
		 // +---+---+---+---+---+---+---+---+
		 // |   | 1 |   |   |   |   | 10|   |
		 // +---+---+---+---+---+---+---+---+
		 // |   | 2 |   |   |   |   | 9 |   |
		 // +---+---+---+---+---+---+---+---+
		 // |   | 3 |   |   |   |   | 8 |   |
		 // +---+---+---+---+---+---+---+---+
		 // |   | * | 4 | 5 | 6 | 7 | * |   |
		 // +---+---+---+---+---+---+---+---+
		 // |   |   |   |   |   |   |   |   |
		 // +---+---+---+---+---+---+---+---+
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(1.5), moduleSize * Scalar(-2.5), 0)); // 0
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(1.5), moduleSize * Scalar(-3.5), 0)); // 1
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(1.5), moduleSize * Scalar(-4.5), 0)); // 2
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(1.5), moduleSize * Scalar(-5.5), 0)); // 3
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(2.5), moduleSize * Scalar(-6.5), 0)); // 4
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(3.5), moduleSize * Scalar(-6.5), 0)); // 5
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(4.5), moduleSize * Scalar(-6.5), 0)); // 6
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(5.5), moduleSize * Scalar(-6.5), 0)); // 7
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(6.5), moduleSize * Scalar(-5.5), 0)); // 8
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(6.5), moduleSize * Scalar(-4.5), 0)); // 9
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(6.5), moduleSize * Scalar(-3.5), 0)); // 10
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(6.5), moduleSize * Scalar(-2.5), 0)); // 11
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(5.5), moduleSize * Scalar(-1.5), 0)); // 12
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(4.5), moduleSize * Scalar(-1.5), 0)); // 13
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(3.5), moduleSize * Scalar(-1.5), 0)); // 14
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(2.5), moduleSize * Scalar(-1.5), 0)); // 15
	}

	if (tagPointGroup & TPG_ORIENTATION_MODULE_CENTERS)
	{
		 // +---+---+---+---+---+---+---+---+
		 // |   |   |   |   |   |   |   |   |
		 // +---+---+---+---+---+---+---+---+
		 // |   | 0 |   |   |   |   | 3 |   |`
		 // +---+---+---+---+---+---+---+---+`
		 // |   |   |   |   |   |   |   |   |`
		 // +---+---+---+---+---+---+---+---+
		 // |   |   |   |   |   |   |   |   |
		 // +---+---+---+---+---+---+---+---+
		 // |   |   |   |   |   |   |   |   |
		 // +---+---+---+---+---+---+---+---+
		 // |   |   |   |   |   |   |   |   |
		 // +---+---+---+---+---+---+---+---+
		 // |   | 1 |   |   |   |   | 2 |   |
		 // +---+---+---+---+---+---+---+---+
		 // |   |   |   |   |   |   |   |   |
		 // +---+---+---+---+---+---+---+---+
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(1.5), moduleSize * Scalar(-1.5), 0)); // 0
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(1.5), moduleSize * Scalar(-6.5), 0)); // 1
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(6.5), moduleSize * Scalar(-6.5), 0)); // 2
		objectPoints.emplace_back(Vector3(moduleSize * Scalar(6.5), moduleSize * Scalar(-1.5), 0)); // 3
	}

	return objectPoints;
}

std::vector<uint8_t> OculusTagTracker::generateModuleValues(const OculusTag::DataMatrix& dataMatrix)
{
	std::vector<uint8_t> modules =
	{
		// clang-format off
		1u, 1u, 1u,                               1u,                               1u,                               1u,                               1u, 1u,
		1u, 0u, 0u,                               0u,                               0u,                               0u,                               1u, 1u,
		1u, 0u, uint8_t((dataMatrix >> 15) & 1u), uint8_t((dataMatrix >> 14) & 1u), uint8_t((dataMatrix >> 13) & 1u), uint8_t((dataMatrix >> 12) & 1u), 0u, 1u,
		1u, 0u, uint8_t((dataMatrix >> 11) & 1u), uint8_t((dataMatrix >> 10) & 1u), uint8_t((dataMatrix >>  9) & 1u), uint8_t((dataMatrix >>  8) & 1u), 0u, 1u,
		1u, 0u, uint8_t((dataMatrix >>  7) & 1u), uint8_t((dataMatrix >>  6) & 1u), uint8_t((dataMatrix >>  5) & 1u), uint8_t((dataMatrix >>  4) & 1u), 0u, 1u,
		1u, 0u, uint8_t((dataMatrix >>  3) & 1u), uint8_t((dataMatrix >>  2) & 1u), uint8_t((dataMatrix >>  1) & 1u), uint8_t((dataMatrix >>  0) & 1u), 0u, 1u,
		1u, 1u, 0u,                               0u,                               0u,                               0u,                               1u, 1u,
		1u, 1u, 1u,                               1u,                               1u,                               1u,                               1u, 1u,
		// clang-format on
	};

	return modules;
}

bool OculusTagTracker::isTagVisible(const AnyCamera& anyCamera, const HomogenousMatrix4& tag_T_camera, const Scalar tagSize, const Scalar signedBorder)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(tag_T_camera.isValid());
	ocean_assert(tagSize > 0);

	const HomogenousMatrix4 flippedCamera_T_tag = AnyCamera::standard2InvertedFlipped(tag_T_camera);

	const Vectors3 tagObjectPoints = OculusTagTracker::getTagObjectPoints(OculusTagTracker::TPG_CORNERS_0_TO_3, tagSize);
	ocean_assert(tagObjectPoints.size() == 4);

	for (size_t i = 0; i < tagObjectPoints.size(); ++i)
	{
		const Vector2 imagePoint = anyCamera.projectToImageIF(flippedCamera_T_tag * tagObjectPoints[i]);

		if (anyCamera.isInside(imagePoint, signedBorder) == false)
		{
			return false;
		}
	}

	return true;
}

bool OculusTagTracker::trackTagCornersTemporally(const AnyCamera& anyCamera, const CV::FramePyramid& framePyramid, const CV::FramePyramid& previousFramePyramid, const HomogenousMatrix4& world_T_camera, const HomogenousMatrix4& previous_world_T_tag, const Vectors3& objectPoints, const Vectors2& previousImagePoints, Vectors2& imagePoints)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(framePyramid.isValid() && framePyramid.frameType() == previousFramePyramid.frameType());
	ocean_assert(world_T_camera.isValid() && previous_world_T_tag.isValid());
	ocean_assert(objectPoints.size() >= 4 && objectPoints.size() == previousImagePoints.size());

	const HomogenousMatrix4 flippedCamera_T_world = AnyCamera::standard2InvertedFlipped(world_T_camera);

	Vectors2 predictedImagePoints;
	predictedImagePoints.reserve(objectPoints.size());

	for (size_t i = 0; i < objectPoints.size(); ++i)
	{
		const Vector3 previous_worldPoint = previous_world_T_tag * objectPoints[i];
		predictedImagePoints.emplace_back(anyCamera.projectToImageIF(flippedCamera_T_world * previous_worldPoint));

		if (anyCamera.isInside(predictedImagePoints.back(), frameBorder_) == false)
		{
			return false;
		}
	}

	ocean_assert(objectPoints.size() == predictedImagePoints.size());
	return CV::Advanced::AdvancedMotionZeroMeanSSD::trackPointsSubPixelMirroredBorder<1u, 7u>(previousFramePyramid, framePyramid, previousImagePoints, predictedImagePoints, imagePoints, /* coarsestLayerRadius */ 2u);
}

OculusTagTracker::TrackedTags OculusTagTracker::detectTagsStereo(const SharedAnyCameras& anyCameras, const Frames& yFrames,  const HomogenousMatrix4& world_T_device, const HomogenousMatrices4& device_T_cameras)
{
	ocean_assert(anyCameras.size() >= 2);
	ocean_assert(anyCameras.size() == yFrames.size());
	ocean_assert(anyCameras.size() == device_T_cameras.size());

#ifdef OCEAN_DEBUG
	for (size_t iCamera = 0; iCamera < 2; ++iCamera)
	{
		ocean_assert(anyCameras[iCamera] && anyCameras[iCamera]->isValid());
		ocean_assert(yFrames[iCamera].isValid() && FrameType::arePixelFormatsCompatible(yFrames[iCamera].pixelFormat(), FrameType::FORMAT_Y8));
		ocean_assert(yFrames[iCamera].width() == anyCameras[iCamera]->width() && yFrames[iCamera].height() == anyCameras[iCamera]->height());
		ocean_assert(device_T_cameras[iCamera].isValid());
	}
#endif

	const HomogenousMatrix4 world_T_cameras[2] =
	{
		world_T_device * device_T_cameras[0],
		world_T_device * device_T_cameras[1],
	};

	constexpr Scalar dummyTagSize = Scalar(1); // Choosing an arbitrary tag size here. The true metric size will be determined later

	TagObservationHistories observationHistoryGroups[2];
	OculusTags tagGroups[2];
	for (size_t cameraIndex : {0, 1})
	{
		tagGroups[cameraIndex] = OculusTagTracker::detectTagsMono(*anyCameras[cameraIndex], yFrames[cameraIndex], world_T_device, device_T_cameras[cameraIndex], dummyTagSize, TagSizeMap(), &observationHistoryGroups[cameraIndex]);
		ocean_assert(tagGroups[cameraIndex].size() == observationHistoryGroups[cameraIndex].size());
	};

	const Vectors3 dummyObjectCorners = getTagObjectPoints(TPG_CORNERS_0_TO_3, dummyTagSize);
	ocean_assert(dummyObjectCorners.size() == 4);

	Vectors2 imageCornerGroups[2] =
	{
		Vectors2(dummyObjectCorners.size()),
		Vectors2(dummyObjectCorners.size()),
	};

	TrackedTags newTags;

	for (size_t tagIndex0 = 0; tagIndex0 < tagGroups[0].size(); ++tagIndex0)
	{
		const OculusTag& tag0 = tagGroups[0][tagIndex0];
		ocean_assert(tag0.isValid());

		const HomogenousMatrix4 flippedCameraA_T_tag = AnyCamera::standard2InvertedFlipped(tag0.world_T_tag().inverted() * world_T_cameras[0]);
		ocean_assert(flippedCameraA_T_tag.isValid());

		anyCameras[0]->projectToImageIF(flippedCameraA_T_tag, dummyObjectCorners.data(), dummyObjectCorners.size(), imageCornerGroups[0].data());

		for (size_t tagIndex1 = 0; tagIndex1 < tagGroups[1].size(); ++tagIndex1)
		{
			const OculusTag& tag1 = tagGroups[1][tagIndex1];
			ocean_assert(tag1.isValid());

			if (tag0.tagID() != tag1.tagID() || tag0.reflectanceType() != tag1.reflectanceType())
			{
				continue;
			}

			const HomogenousMatrix4 flippedCameraB_T_tag = AnyCamera::standard2InvertedFlipped(tag1.world_T_tag().inverted() * world_T_cameras[1]);
			ocean_assert(flippedCameraB_T_tag.isValid());

			anyCameras[1]->projectToImageIF(flippedCameraB_T_tag, dummyObjectCorners.data(), dummyObjectCorners.size(), imageCornerGroups[1].data());

			ocean_assert(!imageCornerGroups[0].empty() && imageCornerGroups[0].size() == imageCornerGroups[1].size());

			Indices32 invalidIndices;
			const Vectors3 worldCorners = Geometry::EpipolarGeometry::triangulateImagePoints(world_T_cameras[0], world_T_cameras[1], *anyCameras[0], *anyCameras[1], imageCornerGroups[0].data(), imageCornerGroups[1].data(), imageCornerGroups[0].size(), /* onlyFrontObjectPoints */ true, /* invalidObjectPoint */ Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()), &invalidIndices);

			if (worldCorners.size() != imageCornerGroups[0].size() || !invalidIndices.empty())
			{
				continue;
			}

			// Compute sum of edge lengths and use its average as the tag size.
			Scalar edgeLengthSum = Scalar(0);

			for (size_t i = 1; i < worldCorners.size(); ++i)
			{
				edgeLengthSum += (worldCorners[i - 1] - worldCorners[i]).length();
			}

			edgeLengthSum += (worldCorners.back() - worldCorners.front()).length();

			ocean_assert(edgeLengthSum > Scalar(0));
			ocean_assert(worldCorners.size() != 0);

			const Scalar tagSize = edgeLengthSum / Scalar(worldCorners.size());

			if (tagSize > Scalar(0))
			{
				// Re-compute the pose of the tag using its true metric size.
				HomogenousMatrix4 world_T_tag(false);

				const Vectors3 objectCorners = getTagObjectPoints(TPG_CORNERS_0_TO_3, tagSize);
				ocean_assert(objectCorners.size() == 4);

				for (const size_t cameraIndex : {0, 1})
				{
					HomogenousMatrix4 tag_T_camera(false);

					if (computePose(*anyCameras[cameraIndex], imageCornerGroups[cameraIndex], objectCorners, tag_T_camera) && tag_T_camera.isValid())
					{
						 world_T_tag = world_T_cameras[cameraIndex] * tag_T_camera.inverted();
						 ocean_assert(world_T_tag.isValid());

						 break;
					}
				}

				if (!world_T_tag.isValid())
				{
					continue;
				}

				// Create a new tag with the correct metric size.
				const uint8_t averageIntensityThreshold = (tag0.intensityThreshold() + tag1.intensityThreshold() + 1u) / 2u;

				OculusTag tag(tag0.tagID(), tag0.reflectanceType(), averageIntensityThreshold, world_T_tag, tagSize);

				// Create updated observation histories.
				TagObservationHistory updatedObservationHistoryGroups[2];

				bool updateSuccessful = true;

				for (size_t cameraIndex : {0, 1})
				{
					ocean_assert(imageCornerGroups[cameraIndex].size() == 4);
					const QuadDetector::Quad quad =
					{
						imageCornerGroups[cameraIndex][0],
						imageCornerGroups[cameraIndex][1],
						imageCornerGroups[cameraIndex][2],
						imageCornerGroups[cameraIndex][3],
					};

					if (!addTagObservation(*anyCameras[cameraIndex], yFrames[cameraIndex], world_T_device, device_T_cameras[cameraIndex], tag, quad, updatedObservationHistoryGroups[cameraIndex]))
					{
						updateSuccessful = false;

						break;
					}
				}

				if (!updateSuccessful)
				{
					continue;
				}

				// Optimize the pose using stereo.
				HomogenousMatrix4 optimizedWorld_T_tag(false);
				if (TagObservationHistory::optimizePose(*anyCameras[0], *anyCameras[1], updatedObservationHistoryGroups[0], updatedObservationHistoryGroups[1], world_T_tag, optimizedWorld_T_tag) && optimizedWorld_T_tag.isValid())
				{
					tag.setWorld_T_tag(optimizedWorld_T_tag);
				}

				// Save the results.
				newTags.emplace_back(std::move(tag), std::move(updatedObservationHistoryGroups[0]), std::move(updatedObservationHistoryGroups[1]), TS_NEW_DETECTION, MT_UNKNOWN);

				// The current two tags, tag0 and tag1, have been matched and should not be used in future iterations.
				std::swap(tagGroups[0][tagIndex0], tagGroups[0].back());
				tagGroups[0].pop_back();

				std::swap(tagGroups[1][tagIndex1], tagGroups[1].back());
				tagGroups[1].pop_back();

				// Because of the swap, decrement the index to counter the increment at the beginning of the next iteration
				--tagIndex0;

				break;
			}
		}
	}

	return newTags;
}

bool OculusTagTracker::readTag(const AnyCamera& anyCamera, const Frame& yFrame, const QuadDetector::Quad& unorientedQuad, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_camera, const Scalar tagSize, OculusTag& tag, QuadDetector::Quad& quad, const TagSizeMap& tagSizeMap)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(yFrame.isValid() && FrameType::arePixelFormatsCompatible(yFrame.pixelFormat(), FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, 1u>()));
	ocean_assert(yFrame.width() == anyCamera.width() && yFrame.height() == anyCamera.height());
	ocean_assert(device_T_camera.isValid());
	ocean_assert(tagSize > 0);

	const Vectors3 outerCornerObjectPoints = getTagObjectPoints(TPG_CORNERS_0_TO_3, tagSize);
	Vectors2 outerCornerImagePoints(unorientedQuad.begin(), unorientedQuad.end());
	ocean_assert(outerCornerObjectPoints.size() == 4 && outerCornerObjectPoints.size() == outerCornerImagePoints.size());

	HomogenousMatrix4 unorientedTag_T_camera(false);

	if (computePose(anyCamera, outerCornerImagePoints, outerCornerObjectPoints, unorientedTag_T_camera))
	{
		ocean_assert(unorientedTag_T_camera.isValid());

		OculusTag::ReflectanceType reflectanceType = OculusTag::RT_REFLECTANCE_UNDEFINED;
		uint8_t intensityThreshold = uint8_t(128);
		uint8_t moduleValueDark = uint8_t(1);

		if (determineReflectanceTypeAndIntensityThreshold(anyCamera, yFrame, unorientedTag_T_camera, tagSize, reflectanceType, intensityThreshold, moduleValueDark))
		{
			ocean_assert(moduleValueDark <= 1u);

			HomogenousMatrix4 tag_T_camera(false);

			if (determineOrientation(anyCamera, yFrame, unorientedQuad, unorientedTag_T_camera, tagSize, quad, tag_T_camera, intensityThreshold, moduleValueDark))
			{
				ocean_assert(tag_T_camera.isValid());

				OculusTag::DataMatrix dataMatrix = 0;

				if (readDataMatrix(anyCamera, yFrame, tag_T_camera, tagSize, intensityThreshold, moduleValueDark, (1u - moduleValueDark), dataMatrix))
				{
					uint32_t tagID = uint32_t(-1);

					if (Utilities::decode(dataMatrix, tagID))
					{
						Scalar tagSizeToUse = tagSize;

						// Check if the default tag size applies for this tag or if a different size has been specified

						const TagSizeMap::const_iterator tagSizeIter = tagSizeMap.find(tagID);

						if (tagSizeIter != tagSizeMap.cend() && !Numeric::isEqualEps(tagSizeIter->second - tagSizeToUse))
						{
							ocean_assert(tagSizeIter->second > 0);

							// The size of this tag is fixed. The rotational part of the pose does not change
							// but the translation has to be scaled by the ratio of the actual tag size and the default one.
							ocean_assert(tagSizeToUse > 0);
							tag_T_camera.setTranslation(tag_T_camera.translation() * (tagSizeIter->second / tagSize));

							tagSizeToUse = tagSizeIter->second;
						}

						tag = OculusTag(tagID, reflectanceType, intensityThreshold, world_T_device * device_T_camera * tag_T_camera.inverted(), tagSizeToUse);

						return tag.isValid();
					}
				}
			}
		}
	}

	return false;
}

bool OculusTagTracker::determineReflectanceTypeAndIntensityThreshold(const AnyCamera& anyCamera, const Frame& yFrame, const HomogenousMatrix4& tag_T_camera, const Scalar tagSize, OculusTag::ReflectanceType& reflectanceType, uint8_t& intensityThreshold, uint8_t& moduleValueDark)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(yFrame.isValid() && FrameType::arePixelFormatsCompatible(yFrame.pixelFormat(), FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, 1u>()));
	ocean_assert(yFrame.width() == anyCamera.width() && yFrame.height() == anyCamera.height());
	ocean_assert(tag_T_camera.isValid());
	ocean_assert(tagSize > 0);

	reflectanceType = OculusTag::RT_REFLECTANCE_UNDEFINED;
	intensityThreshold = 255u;
	moduleValueDark = 255u;

	const Vectors3 foregroundTagPoints = getTagObjectPoints(TPG_FOREGROUND_MODULE_CENTERS, tagSize);
	const Vectors3 backgroundTagPoints = getTagObjectPoints(TPG_BACKGROUND_MODULE_CENTERS, tagSize);
	ocean_assert(!foregroundTagPoints.empty() && !backgroundTagPoints.empty());

	uint32_t foregroundValueSum = 0u;
	std::vector<uint8_t> foregroundValues;
	foregroundValues.reserve(foregroundTagPoints.size());

	for (size_t i = 0; i < foregroundTagPoints.size(); ++i)
	{
		uint8_t moduleValue = 0u;
		const Vector2 moduleCenterImage = anyCamera.projectToImage(tag_T_camera, foregroundTagPoints[i]);
		ocean_assert(anyCamera.isInside(moduleCenterImage));

		CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<1u, CV::PC_TOP_LEFT>(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), moduleCenterImage, &moduleValue);

		foregroundValues.push_back(moduleValue);
		foregroundValueSum += uint32_t(moduleValue);
	}

	uint32_t backgroundValueSum = 0u;
	std::vector<uint8_t> backgroundValues;
	backgroundValues.reserve(backgroundTagPoints.size());

	for (size_t i = 0; i < backgroundTagPoints.size(); ++i)
	{
		uint8_t moduleValue = 0u;
		const Vector2 moduleCenterImage = anyCamera.projectToImage(tag_T_camera, backgroundTagPoints[i]);
		ocean_assert(anyCamera.isInside(moduleCenterImage));

		CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<1u, CV::PC_TOP_LEFT>(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), moduleCenterImage, &moduleValue);

		backgroundValues.push_back(moduleValue);
		backgroundValueSum += uint32_t(moduleValue);
	}

	// Determine the reflectance type and the binary values of the dark modules

	const uint32_t avgForegroundValue = uint32_t(Numeric::round32(Scalar(foregroundValueSum) / Scalar(foregroundTagPoints.size())));
	const uint32_t avgBackgroundValue = uint32_t(Numeric::round32(Scalar(backgroundValueSum) / Scalar(backgroundTagPoints.size())));
	const uint32_t avgIntensityDifference = uint32_t(std::abs(int32_t(avgForegroundValue) - int32_t(avgBackgroundValue)));

	if (avgIntensityDifference >= minIntensityThreshold_)
	{
		ocean_assert((avgForegroundValue + avgBackgroundValue + 1u) / 2u <= 255u);
		intensityThreshold = uint8_t((avgForegroundValue + avgBackgroundValue + 1u) / 2u);

		uint32_t foregroundValuesCorrect = 0u;
		uint32_t backgroundValuesCorrect = 0u;

		if (avgForegroundValue < avgBackgroundValue)
		{
			reflectanceType = OculusTag::RT_REFLECTANCE_NORMAL;
			moduleValueDark = 1u;

			for (const uint8_t value : foregroundValues)
			{
				if (value < intensityThreshold)
				{
					++foregroundValuesCorrect;
				}
			}

			for (const uint8_t value : backgroundValues)
			{
				if (value >= intensityThreshold)
				{
					++backgroundValuesCorrect;
				}
			}
		}
		else
		{
			reflectanceType = OculusTag::RT_REFLECTANCE_INVERTED;
			moduleValueDark = 0u;

			for (const uint8_t value : foregroundValues)
			{
				if (value >= intensityThreshold)
				{
					++foregroundValuesCorrect;
				}
			}

			for (const uint8_t value : backgroundValues)
			{
				if (value < intensityThreshold)
				{
					++backgroundValuesCorrect;
				}
			}
		}

		const Scalar foregroundCorrectRatio = Scalar(foregroundValuesCorrect) / Scalar(foregroundTagPoints.size());
		const Scalar backgroundCorrectRatio = Scalar(backgroundValuesCorrect) / Scalar(backgroundTagPoints.size());
		ocean_assert(foregroundCorrectRatio >= 0 && foregroundCorrectRatio <= Scalar(1));
		ocean_assert(backgroundCorrectRatio >= 0 && backgroundCorrectRatio <= Scalar(1));

		if (foregroundCorrectRatio >= 0.9 && backgroundCorrectRatio >= 0.9)
		{
			return true;
		}
	}

	return false;
}

bool OculusTagTracker::determineOrientation(const AnyCamera& anyCamera, const Frame& yFrame, const QuadDetector::Quad& unorientedQuad, const HomogenousMatrix4& unorientedTag_T_camera, const Scalar tagSize, QuadDetector::Quad& orientedQuad, HomogenousMatrix4& orientedTag_T_camera, const uint8_t& intensityThreshold, const uint8_t& moduleValueDark)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(yFrame.isValid() && FrameType::arePixelFormatsCompatible(yFrame.pixelFormat(), FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, 1u>()));
	ocean_assert(yFrame.width() == anyCamera.width() && yFrame.height() == yFrame.height());
	ocean_assert(unorientedTag_T_camera.isValid());
	ocean_assert(tagSize > 0);
	ocean_assert(moduleValueDark <= 1u);

	static_assert(OculusTag::numberOfModules == 8u, "This function is hard-coded to 8 modules in horizontal and vertical dimension");

	const Vectors3 orientationTagPoints = getTagObjectPoints(TPG_ORIENTATION_MODULE_CENTERS, tagSize);
	ocean_assert(orientationTagPoints.size() == 4);

	uint8_t moduleValueSum = 0u;
	uint8_t topLeftIndex = uint8_t(-1);

	for (size_t i = 0; i < orientationTagPoints.size(); ++i)
	{
		const Vector2 framePoint = anyCamera.projectToImage(unorientedTag_T_camera, orientationTagPoints[i]);

		uint8_t pixelValue;
		CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<1u, CV::PC_TOP_LEFT>(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), framePoint, &pixelValue);

		const uint8_t moduleValue = pixelValue < intensityThreshold ? moduleValueDark : (1u - moduleValueDark);

		if (moduleValue == 0u)
		{
			ocean_assert(i < size_t(256));
			topLeftIndex = uint8_t(i);
		}

		moduleValueSum += moduleValue;
	}

	ocean_assert(moduleValueSum <= 4u);
	ocean_assert(topLeftIndex < 4u || moduleValueSum == 4u);

	if (moduleValueSum == 3u && topLeftIndex < 4u)
	{
		if (topLeftIndex == 0u)
		{
			orientedQuad = unorientedQuad;
			orientedTag_T_camera = unorientedTag_T_camera;

			return true;
		}
		else
		{
			// Shift the corner points so that they start with the corner that has been identified as the top-left corner

			for (size_t i = 0; i < 4; ++i)
			{
				orientedQuad[i] = unorientedQuad[(topLeftIndex + i) & 0b0011]; // (i + 1) & 0b0111 ~ (i + 1) % 4
			}

			// Rotate the transformation so that it maps the top-left corner of the tag to the corresponding location in the image

			const Scalar angles[4] =
			{
				Numeric::deg2rad(Scalar(0)),
				Numeric::deg2rad(Scalar(270)),
				Numeric::deg2rad(Scalar(180)),
				Numeric::deg2rad(Scalar(90))
			};

			const Scalar moduleSize = tagSize / Scalar(8);
			ocean_assert(moduleSize > 0);

			const Scalar center = Scalar(4) * moduleSize;

			const HomogenousMatrix4 translation = HomogenousMatrix4(Vector3(-center, center, 0));
			const HomogenousMatrix4 translationInv = HomogenousMatrix4(Vector3(center, -center, 0));
			const HomogenousMatrix4 rotation = HomogenousMatrix4(Quaternion(Vector3(0, 0, 1), angles[topLeftIndex]));

			orientedTag_T_camera = translationInv * rotation * translation * unorientedTag_T_camera;

			return orientedTag_T_camera.isValid();
		}
	}

	return false;
}

bool OculusTagTracker::readDataMatrix(const AnyCamera& anyCamera, const Frame& yFrame, const HomogenousMatrix4& tag_T_camera, const Scalar tagSize, const uint8_t& intensityThreshold, const uint8_t& binaryModuleValueDark, const uint8_t& binaryModuleValueLight, OculusTag::DataMatrix& dataMatrix)
{
	static_assert(OculusTag::numberOfModules == 8u, "This function is hard-coded to 8 modules in horizontal and vertical dimension");
	ocean_assert(anyCamera.isValid());
	ocean_assert(yFrame.isValid() && FrameType::arePixelFormatsCompatible(yFrame.pixelFormat(), FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, 1u>()));
	ocean_assert(yFrame.width() == anyCamera.width() && yFrame.height() == yFrame.height());
	ocean_assert(tag_T_camera.isNull() == false);
	ocean_assert(tagSize > 0);
	ocean_assert(binaryModuleValueDark != binaryModuleValueLight && binaryModuleValueDark <= 1u && binaryModuleValueLight <= 1u);

	dataMatrix = 0u;

	const Vectors3 moduleCenters = getTagObjectPoints(TPG_DATA_MATRIX_MODULE_CENTERS, tagSize);

	for (size_t i = 0; i < 16; ++i)
	{
		const Vector2 moduleCenterImage = anyCamera.projectToImage(tag_T_camera, moduleCenters[i]);
		uint8_t moduleValue;

		CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<1u, CV::PC_TOP_LEFT>(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), moduleCenterImage, &moduleValue);

		dataMatrix |= (moduleValue >= intensityThreshold ? binaryModuleValueLight : binaryModuleValueDark) << (15 - i);
	}

	return true;
}

bool OculusTagTracker::addTagObservation(const AnyCamera& anyCamera, const Frame& yFrame, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_camera, const OculusTag& tag, const QuadDetector::Quad& quad, TagObservationHistory& tagObservationHistory)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(yFrame.isValid() && FrameType::arePixelFormatsCompatible(yFrame.pixelFormat(), FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, 1u>()));
	ocean_assert(yFrame.width() == anyCamera.width() && yFrame.height() == anyCamera.height());
	ocean_assert(world_T_device.isValid());
	ocean_assert(device_T_camera.isValid());
	ocean_assert(tag.isValid());

	const HomogenousMatrix4 tag_T_camera = tag.world_T_tag().inverted() * world_T_device * device_T_camera;

	if (isTagVisible(anyCamera, tag_T_camera, tag.tagSize(), frameBorder_) == false)
	{
		return false;
	}

	Vectors3 objectPoints = getTagObjectPoints(TPG_CORNERS_0_TO_3, tag.tagSize());
	Vectors2 imagePoints(quad.begin(), quad.end());
	ocean_assert(objectPoints.size() == 4 && objectPoints.size() == imagePoints.size());

	// Use additional points if the image of the current tag is large enough.

	Scalar minSquareEdgeLength = Numeric::maxValue();

	for (size_t i = 0; i < 4; ++i)
	{
		const Scalar squareEdgeLength = imagePoints[i].sqrDistance(imagePoints[(i + 1) & 0b0011]); // (i + 1) & 0b0111 ~ (i + 1) % 4
		minSquareEdgeLength = std::min(minSquareEdgeLength, squareEdgeLength);
	}

	// Use the minimum length of the edges connecting the image corners above to define the size of the search window for corner refinement.
	const Scalar minEdgeLength = Numeric::sqrt(minSquareEdgeLength);
	unsigned int searchWindowSize = 0u;

	if (minEdgeLength < Scalar(50))
	{
		searchWindowSize = 2u;
	}
	else if (minEdgeLength < Scalar(100))
	{
		searchWindowSize = 3u;
	}
	else
	{
		searchWindowSize = 4u;
	}

	ocean_assert(searchWindowSize != 0u);

	const HomogenousMatrix4 flippedCamera_T_tag = AnyCamera::standard2InvertedFlipped(tag_T_camera);

	OculusTag::DataMatrix dataMatrix;
	if (!Utilities::encode(tag.tagID(), dataMatrix))
	{
		ocean_assert(false && "This should never happen!");
	}

	const Vectors3 tagObjectPoints = getTagObjectPoints(TPG_CORNERS_ALL_AVAILABLE, tag.tagSize(), dataMatrix);
	ocean_assert(tagObjectPoints.size() > 4);

	for (size_t i = 4; i < tagObjectPoints.size(); ++i)
	{
		Vector2 imagePoint = anyCamera.projectToImageIF(flippedCamera_T_tag * tagObjectPoints[i]);
		ocean_assert(anyCamera.isInside(imagePoint));

		if (Utilities::refineCorner(yFrame, imagePoint, searchWindowSize))
		{
			imagePoints.emplace_back(imagePoint);
			objectPoints.emplace_back(tagObjectPoints[i]);
		}
	}

	Vectors2 trackingImagePoints = Vectors2(quad.begin(), quad.end());
	Vectors3 trackingObjectPoints = getTagObjectPoints(TPG_CORNERS_0_TO_3, tag.tagSize());
	tagObservationHistory.addObservation(world_T_device * device_T_camera, std::move(objectPoints), std::move(imagePoints), std::move(trackingImagePoints), std::move(trackingObjectPoints));

	return true;
}

bool OculusTagTracker::addTagObservationAndOptimize(const AnyCamera& anyCamera, const Frame& yFrame, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_camera, OculusTag& tag, const QuadDetector::Quad& quad, TagObservationHistory& tagObservationHistory)
{
	if (addTagObservation(anyCamera, yFrame, world_T_device, device_T_camera, tag, quad, tagObservationHistory))
	{
		const HomogenousMatrix4 unoptimized_world_T_tag = tag.world_T_tag();
		HomogenousMatrix4 optimized_world_T_tag(false);

		if (tagObservationHistory.optimizePose(anyCamera, unoptimized_world_T_tag, optimized_world_T_tag))
		{
			ocean_assert(optimized_world_T_tag.isValid());
			tag.setWorld_T_tag(optimized_world_T_tag);
			return true;
		}
	}

	return false;
}

bool OculusTagTracker::computePose(const AnyCamera& anyCamera, const Vectors2& imagePoints, const Vectors3& objectPoints, HomogenousMatrix4& object_T_camera, const uint32_t minPoints)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(imagePoints.size() >= minPoints && imagePoints.size() == objectPoints.size());
	ocean_assert(minPoints >= 3u);

	constexpr Scalar maximalProjectionError = Scalar(2.5);
	RandomGenerator randomGenerator;
	Indices32 usedIndices;

	return Geometry::RANSAC::p3p(anyCamera, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), randomGenerator, object_T_camera, minPoints, /* refine */ true, /* iterations */ 10u, Numeric::sqr(maximalProjectionError), &usedIndices);
}

bool OculusTagTracker::confirmDetectionInFrame(const AnyCamera& anyCamera, const Frame& yFrame, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_camera, const OculusTag& tag)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(yFrame.isValid());
	ocean_assert(world_T_device.isValid() && device_T_camera.isValid());

	if (tag.isValid() == false)
	{
		return false;
	}

	if (isTagVisible(anyCamera, tag.world_T_tag().inverted() * world_T_device * device_T_camera, tag.tagSize(), Scalar(frameBorder_)) == false)
	{
		return false;
	}

	const HomogenousMatrix4 tag_T_camera = tag.world_T_tag().inverted() * world_T_device * device_T_camera;
	ocean_assert(tag_T_camera.isValid());

	const uint8_t binaryModuleValueDark = tag.reflectanceType() == OculusTag::RT_REFLECTANCE_NORMAL ? 1u : 0u;
	ocean_assert(binaryModuleValueDark <= 1u);

	OculusTag::DataMatrix dataMatrix;

	if (readDataMatrix(anyCamera, yFrame, tag_T_camera, tag.tagSize(), tag.intensityThreshold(), binaryModuleValueDark, 1u - binaryModuleValueDark, dataMatrix) == false)
	{
		return false;
	}

	uint32_t tagID = uint32_t(-1);
	if (Utilities::decode(dataMatrix, tagID) == false)
	{
		return false;
	}

	ocean_assert(tagID < 1024u);

	return tagID == tag.tagID();
}

CV::FramePyramid OculusTagTracker::createFramePyramid(const Frame& yFrame, const uint32_t layers)
{
	ocean_assert(yFrame.isValid() && yFrame.isPixelFormatCompatible(FrameType::FORMAT_Y8));

	return CV::FramePyramid(yFrame, layers, true /*copyFirstLayer*/);
}

}  // namespace OculusTags

}  // namespace Tracking

}  // namespace Ocean
