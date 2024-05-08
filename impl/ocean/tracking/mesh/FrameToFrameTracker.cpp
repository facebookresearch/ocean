// Copyright (c) Meta Platforms, Inc. and affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "ocean/tracking/mesh/FrameToFrameTracker.h"

#include "ocean/base/Accessor.h"
#include "ocean/base/Subset.h"

#include "ocean/cv/advanced/AdvancedMotion.h"

#include "ocean/geometry/NonLinearOptimizationPose.h"

#include "ocean/math/SuccessionSubset.h"

#include <array>

namespace Ocean
{

namespace Tracking
{

namespace Mesh
{

namespace
{

// Obtain a rough pose estimate by tracking points at lower-resolution pyramid levels. This provides
// added stability when the camera pose changes significantly between frames.
//
// This implementation comes directly from:
// Ocean::Tracking::Pattern::PatternTrackerCore6DOF::trackFrame2FrameHierarchy
bool trackFrameToFrameAtLowerResolution(const PinholeCamera& pinholeCamera, const CV::FramePyramid& previousFramePyramid, const CV::FramePyramid& currentFramePyramid, const unsigned int initialPyramidLayer, const HomogenousMatrix4& predictedPoseFromIMU_world_T_currentFrame, const Vectors3& previousObjectPoints, const Vectors2& previousImagePoints, const unsigned int numberFeatures, const Scalar maxError, HomogenousMatrix4& roughPose_world_T_currentFrame, Worker* worker)
{
	constexpr unsigned int kNumChannels = 1u;
	constexpr unsigned int kPatchSize = 7u; // window size in pixels for SSD
	constexpr Scalar kPatchRadius = Scalar(3.0); // = kPatchSize / 2 - 1

	ocean_assert(previousFramePyramid && currentFramePyramid && previousFramePyramid.frameType() == currentFramePyramid.frameType());
	ocean_assert(FrameType::formatIsGeneric(previousFramePyramid.frameType().pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, kNumChannels));
	ocean_assert(FrameType::formatIsGeneric(currentFramePyramid.frameType().pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, kNumChannels));
	ocean_assert(pinholeCamera.isValid() && pinholeCamera.width() == previousFramePyramid.finestWidth() && pinholeCamera.height() == previousFramePyramid.finestHeight());
	ocean_assert(previousObjectPoints.size() == previousImagePoints.size() && previousImagePoints.size() >= 3u);

	// We'll pull points from up to 3 layers in the pyramid, starting at `initialPyramidLayer`.
	constexpr unsigned int kMaxNumTrackingLayers = 3u;

	// TODO (jtprice): hardcoded values
	if (previousFramePyramid.layers() <= initialPyramidLayer ||
		previousImagePoints.size() < 3u)
	{
		return false;
	}

	// It's fine if there are fewer than kMaxNumTrackingLayers left in the pyramid.
	const CV::FramePyramid previousFrameSubPyramid(previousFramePyramid, initialPyramidLayer, kMaxNumTrackingLayers, false /*copyData*/);
	const CV::FramePyramid currentFrameSubPyramid(currentFramePyramid, initialPyramidLayer, kMaxNumTrackingLayers, false /*copyData*/);

	const PinholeCamera lowerResolutionCamera(previousFrameSubPyramid.finestWidth(), previousFrameSubPyramid.finestHeight(), pinholeCamera);

	Vectors3 previousFrameSubPyramidObjectPoints;
	Vectors2 previousFrameSubPyramidImagePoints;

	previousFrameSubPyramidObjectPoints.reserve(numberFeatures);
	previousFrameSubPyramidImagePoints.reserve(numberFeatures);

	// To maximize the spread of points in the image, pull points based on their distance to all other points.
	SuccessionSubset<Scalar, 2> subset(reinterpret_cast<const SuccessionSubset<Scalar, 2>::Object*>(previousImagePoints.data()), previousImagePoints.size());

	const Scalar scaleFactor = Scalar(1.0) / Scalar(previousFramePyramid.sizeFactor(initialPyramidLayer));

	// Incrementally add points for tracking, until `numberFeatures` total points are obtained.
	while (previousFrameSubPyramidObjectPoints.size() < numberFeatures)
	{
		const size_t index = subset.incrementSubset();

		if (index == size_t(-1))
		{
			break; // no features left to add
		}

		const Vector2 previousImagePoint(previousImagePoints[index] * scaleFactor);

		if (previousImagePoint.x() >= kPatchRadius &&
			previousImagePoint.y() >= kPatchRadius &&
			previousImagePoint.x() < Scalar(lowerResolutionCamera.width()) - kPatchRadius &&
			previousImagePoint.y() < Scalar(lowerResolutionCamera.height()) - kPatchRadius)
		{
			previousFrameSubPyramidObjectPoints.push_back(previousObjectPoints[index]);
			previousFrameSubPyramidImagePoints.push_back(previousImagePoint);
		}
	}

	// TODO (jtprice): hardcoded values
	// TODO (jtprice): This is 3u above -- investigate this when the hardcoded values are removed
	if (previousFrameSubPyramidObjectPoints.size() <= 5u)
	{
		return false;
	}

	const Vectors2 previousFrameSubPyramidImagePointsCopy(previousFrameSubPyramidImagePoints);

	// We'll start out by hoping that the motion is small and only check a 2D motion radius of 2.
	// If this fails to pull enough correspondences using SSD matching, we'll next try radii of 4 and 8.
	constexpr std::array<unsigned int, 3u> kCoarseLayerRadii = { 2u, 4u, 8u };

	Indices32 validIndices;
	Vectors2 currentFrameSubPyramidImagePoints;

	for (const unsigned int coarseLayerRadius : kCoarseLayerRadii)
	{
		validIndices.clear();
		currentFrameSubPyramidImagePoints.clear();

		if (!CV::Advanced::AdvancedMotionSSD::trackPointsBidirectionalSubPixelMirroredBorderWithRoughLocations<kNumChannels, kPatchSize>(
				previousFrameSubPyramid,
				currentFrameSubPyramid,
				coarseLayerRadius,
				previousFrameSubPyramidImagePoints,
				previousFrameSubPyramidImagePointsCopy,
				currentFrameSubPyramidImagePoints,
				maxError,
				worker,
				&validIndices))
		{
			return false; // failed due to bad input
		}

		// TODO (jtprice): hardcoded values
		// TODO (jtprice): does it make sense to rule out points that were successfully matched?
		if (validIndices.size() >= 10u)
		{
			break;
		}
	}

	// TODO (jtprice): hardcoded values
	if (validIndices.size() <= 5u)
	{
		return false;
	}

	if (previousFrameSubPyramidImagePoints.size() != validIndices.size())
	{
		previousFrameSubPyramidObjectPoints = Subset::subset(previousFrameSubPyramidObjectPoints, validIndices);
		currentFrameSubPyramidImagePoints = Subset::subset(currentFrameSubPyramidImagePoints, validIndices);
	}

	ocean_assert(previousFrameSubPyramidObjectPoints.size() == currentFrameSubPyramidImagePoints.size());

	// TODO (jtprice): hardcoded values
	Scalar initialError, finalError;
	return Geometry::NonLinearOptimizationPose::optimizePose(
		lowerResolutionCamera,
		predictedPoseFromIMU_world_T_currentFrame,
		ConstArrayAccessor<Vector3>(previousFrameSubPyramidObjectPoints),
		ConstArrayAccessor<Vector2>(currentFrameSubPyramidImagePoints),
		pinholeCamera.hasDistortionParameters(),
		roughPose_world_T_currentFrame,
		20u,
		Geometry::Estimator::ET_HUBER,
		Scalar(0.001),
		Scalar(10),
		&initialError,
		&finalError);
}

// Obtain a fine-grained pose estimate using SSD frame-to-frame matching, potentially leveraging a
// rough current pose estimated by trackFrameToFrameAtLowerResolution, if the rough pose was successfully computed.
//
// This implementation comes from:
// Ocean::Tracking::Pattern::PatternTrackerCore6DOF::trackFrame2Frame
bool trackFrameToFrame(
	const PinholeCamera& pinholeCamera,
	const CV::FramePyramid& previousFramePyramid,
	const CV::FramePyramid& currentFramePyramid,
	const HomogenousMatrix4& roughPose_world_T_currentFrame,
	const Vectors3& previousObjectPoints,
	const Vectors2& previousImagePoints,
	HomogenousMatrix4& pose_world_T_currentFrame,
	Vectors3& currentObjectPoints,
	Vectors2& currentImagePoints,
	Indices32& validIndices,
	Worker* worker)
{
	constexpr unsigned int kNumChannels = 1u;
	constexpr unsigned int kPatchSize = 7u; // window size in pixels for SSD
	constexpr Scalar kPatchRadius = Scalar(3.0); // = kPatchSize / 2 - 1

	ocean_assert(pinholeCamera);
	ocean_assert(previousFramePyramid && currentFramePyramid);
	ocean_assert(previousFramePyramid.frameType() == currentFramePyramid.frameType());
	ocean_assert(FrameType::formatIsGeneric(previousFramePyramid.frameType().pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, kNumChannels));
	ocean_assert(FrameType::formatIsGeneric(currentFramePyramid.frameType().pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, kNumChannels));
	ocean_assert(pinholeCamera.width() == previousFramePyramid.finestWidth() && pinholeCamera.height() == previousFramePyramid.finestHeight());

	// TODO (jtprice): hardcoded values
	ocean_assert(previousObjectPoints.size() == previousImagePoints.size() && previousImagePoints.size() >= 3u);

	ocean_assert(currentImagePoints.empty());
	ocean_assert(currentObjectPoints.empty());
	ocean_assert(validIndices.empty());

	currentObjectPoints.clear();
	validIndices.clear();

	//
	// Transform the previous object points into the current image based on the rough pose estimate.
	//

	const HomogenousMatrix4 roughPose_currentFrameFlipped_T_world =
		PinholeCamera::standard2InvertedFlipped(roughPose_world_T_currentFrame);

	Vectors2 roughCurrentImagePoints;
	roughCurrentImagePoints.reserve(previousImagePoints.size());
	validIndices.reserve(previousImagePoints.size());

	for (size_t n = 0; n < previousObjectPoints.size(); ++n)
	{
		const Vector2 roughPoint = pinholeCamera.projectToImageIF<true>(roughPose_currentFrameFlipped_T_world, previousObjectPoints[n], true);

		if (roughPoint.x() >= kPatchRadius &&
			roughPoint.y() >= kPatchRadius &&
			roughPoint.x() < Scalar(previousFramePyramid.finestLayer().width()) - kPatchRadius &&
			roughPoint.y() < Scalar(previousFramePyramid.finestLayer().height()) - kPatchRadius)
		{
			roughCurrentImagePoints.push_back(roughPoint);
			validIndices.push_back(static_cast<unsigned int>(n));
		}
	}

	// TODO (jtprice): hardcoded values
	if (validIndices.size() < 3u)
	{
		return false;
	}

	// A copy is required for SSD tracking, below.
	Vectors2 validPreviousImagePoints = Subset::subset(previousImagePoints, validIndices);

	ocean_assert(validPreviousImagePoints.size() == roughCurrentImagePoints.size());

	//
	// Perform SSD correspondence refinement.
	//

	// We'll start out by hoping that the motion is small and only check a 2D motion radius of 2,
	// taking into account the rough pose estimate. If this fails to pull enough correspondences
	// using SSD matching, we'll next try a radius of 4.
	constexpr std::array<unsigned int, 2u> kCoarseLayerRadii = { 2u, 4u };

	currentImagePoints.reserve(roughCurrentImagePoints.size());

	Indices32 trackedValidIndices; // indexes validPreviousImagePoints

	for (const unsigned int coarseLayerRadius : kCoarseLayerRadii)
	{
		currentImagePoints.clear();

		// TODO (jtprice): hardcoded values
		if (!CV::Advanced::AdvancedMotionSSD::trackPointsBidirectionalSubPixelMirroredBorderWithRoughLocations<kNumChannels, kPatchSize>(
				previousFramePyramid,
				currentFramePyramid,
				coarseLayerRadius,
				validPreviousImagePoints,
				roughCurrentImagePoints,
				currentImagePoints,
				Scalar(1.9 * 1.9),
				worker,
				&trackedValidIndices,
				2u))
		{
			return false; // failed due to bad input
		}

		// TODO (jtprice): hardcoded values
		// TODO (jtprice): does it make sense to rule out points that were successfully matched?
		if (trackedValidIndices.size() >= 10)
		{
			break;
		}

		trackedValidIndices.clear();
	}

	// TODO (jtprice): hardcoded values
	if (trackedValidIndices.size() <= 3u)
	{
		return false;
	}

	// Only keep the points that were (1) valid for tracking, and (2) were actually tracked.
	if (trackedValidIndices.size() != validIndices.size())
	{
		currentImagePoints = Subset::subset(currentImagePoints, trackedValidIndices);
		validIndices = Subset::subset(validIndices, trackedValidIndices);
	}

	currentObjectPoints = Subset::subset(previousObjectPoints, validIndices);

	ocean_assert(currentObjectPoints.size() == currentImagePoints.size());

	// TODO (jtprice): hardcoded values
	Scalar initialError, finalError;
	return Geometry::NonLinearOptimizationPose::optimizePose(
		pinholeCamera,
		roughPose_world_T_currentFrame,
		ConstArrayAccessor<Vector3>(currentObjectPoints),
		ConstArrayAccessor<Vector2>(currentImagePoints),
		pinholeCamera.hasDistortionParameters(),
		pose_world_T_currentFrame,
		20u,
		Geometry::Estimator::ET_HUBER,
		Scalar(0.001),
		Scalar(10),
		&initialError,
		&finalError);
}

} // namespace

// This implementation is adapted from:
// Ocean::Tracking::Pattern::PatternTrackerCore6DOF::determinePoseWithPreviousCorrespondences
bool FrameToFrameTracker::track(const PinholeCamera& pinholeCamera, const CV::FramePyramid& previousFramePyramid, const CV::FramePyramid& currentFramePyramid, const Quaternion& quaternion_previousFrame_T_currentFrame, const HomogenousMatrix4& pose_world_T_previousFrame, const Vectors3& previousObjectPoints, const Vectors2& previousImagePoints, HomogenousMatrix4& pose_world_T_currentFrame, Vectors3& currentObjectPoints, Vectors2& currentImagePoints, Indices32& validIndices, Worker* worker)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(previousFramePyramid);
	ocean_assert(currentFramePyramid);
	ocean_assert(currentFramePyramid.frameType() == previousFramePyramid.frameType());
	ocean_assert(currentFramePyramid.layers() == previousFramePyramid.layers());

	// TODO (jtprice): hardcoded values
	ocean_assert(previousObjectPoints.size() >= 3u);
	ocean_assert(previousObjectPoints.size() == previousImagePoints.size());
	ocean_assert(pose_world_T_previousFrame.isValid());

	ocean_assert(validIndices.empty());
	validIndices.clear();

	const Quaternion quaternion_previousFrame_T_currentFrame_or_identity =
		quaternion_previousFrame_T_currentFrame.isValid() ? quaternion_previousFrame_T_currentFrame : Quaternion(true);

	const HomogenousMatrix4 predictedPoseFromIMU_world_T_currentFrame =
		pose_world_T_previousFrame * quaternion_previousFrame_T_currentFrame_or_identity;
	HomogenousMatrix4 currentRoughPose_world_T_currentFrame(false);

	// Check whether we can apply a frame-to-frame tracking on a low pyramid resolution to receive a
	// rough pose.
	if (currentFramePyramid.layers() >= 3u)
	{
		// TODO (jtprice): hardcoded values
		trackFrameToFrameAtLowerResolution(
			pinholeCamera,
			previousFramePyramid,
			currentFramePyramid,
			2u,
			predictedPoseFromIMU_world_T_currentFrame,
			previousObjectPoints,
			previousImagePoints,
			40u,
			Scalar(1.9 * 1.9),
			currentRoughPose_world_T_currentFrame,
			worker);
	}

	if (currentRoughPose_world_T_currentFrame.isValid())
	{
		// TODO (jtprice): hardcoded values
		const CV::FramePyramid previousFramePyramidSlice(previousFramePyramid, 0u, 3u, false /*copyData*/);
		const CV::FramePyramid currentFramePyramidSlice(currentFramePyramid, 0u, 3u, false /*copyData*/);

		if (!trackFrameToFrame(
				pinholeCamera,
				previousFramePyramidSlice,
				currentFramePyramidSlice,
				currentRoughPose_world_T_currentFrame,
				previousObjectPoints,
				previousImagePoints,
				pose_world_T_currentFrame,
				currentObjectPoints,
				currentImagePoints,
				validIndices,
				worker))
		{
			pose_world_T_currentFrame.toNull();
			currentObjectPoints.clear();
			currentImagePoints.clear();
			validIndices.clear();
			return false;
		}
	}
	else
	{
		// If the rough estimate failed, track using the full pyramid, and use the IMU prior, if available.
		if (!trackFrameToFrame(
				pinholeCamera,
				previousFramePyramid,
				currentFramePyramid,
				predictedPoseFromIMU_world_T_currentFrame,
				previousObjectPoints,
				previousImagePoints,
				pose_world_T_currentFrame,
				currentObjectPoints,
				currentImagePoints,
				validIndices,
				worker))
		{
			pose_world_T_currentFrame.toNull();
			currentObjectPoints.clear();
			currentImagePoints.clear();
			validIndices.clear();
			return false;
		}
	}

	return true;
}

} // namespace Mesh

} // namespace Tracking

} // namespace Ocean
