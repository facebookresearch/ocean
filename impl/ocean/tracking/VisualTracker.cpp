/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/VisualTracker.h"

#include "ocean/cv/FrameInterpolatorBilinear.h"

namespace Ocean
{

namespace Tracking
{

bool VisualTracker::setMaxPositionOffset(const Vector3& positionOffset)
{
	if (positionOffset.x() <= 0 || positionOffset.y() <= 0 || positionOffset.z() <= 0)
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	maxPositionOffset_ = positionOffset;
	return true;
}

bool VisualTracker::setMaxOrientationOffset(const Scalar orientationOffset)
{
	if (orientationOffset <= 0 || orientationOffset >= Numeric::pi())
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	maxOrientationOffset_ = orientationOffset;
	return true;
}

bool VisualTracker::determinePoses(const Frames& frames, const SharedAnyCameras& anyCameras, TransformationSamples& transformations, const Quaternion& world_R_camera, Worker* worker)
{
	ocean_assert(frames.size() == anyCameras.size());

	if (frames.size() != 1)
	{
		ocean_assert(false && "Invalid input - exactly one camera and frame must be provided for the default implementation!");

		return false;
	}

	ocean_assert(anyCameras.front() && anyCameras.front()->isValid());
	ocean_assert(anyCameras.front()->width() == frames.front().width() && anyCameras.front()->height() == frames.front().height());

	Frame frameToUse;
	std::shared_ptr<AnyCameraPinhole> anyCameraPinhole;
	bool isFrameUndistorted = false;

	if (anyCameras.front()->anyCameraType() == AnyCameraType::PINHOLE)
	{
		// The pinhole camera and frame can be used as-is.
		anyCameraPinhole = std::dynamic_pointer_cast<AnyCameraPinhole>(anyCameras.front());
		frameToUse = Frame(frames.front(), Frame::ACM_USE_KEEP_LAYOUT);
	}
	else
	{
		// Not all VisualTracker instances are compatible with arbitrary AnyCamera instance, since most of them have been created with PinholeCamera in mind.
		// For backwards compatibility, the input image will be undistorted and the AnyCamera is converted into a PinholeCamera.

		Frame frameToRectify;
		if (!CV::FrameConverter::Comfort::convert(frames.front(), FrameType::FORMAT_Y8, frameToRectify, /* forceCopy */ false, worker))
		{
			Log::error() << "Failed to convert the frame";

			return false;
		}

		ocean_assert(frameToRectify.isValid());

		const unsigned int rectifiedWidth = anyCameras.front()->width() * 75u / 100u;
		const unsigned int rectifiedHeight = anyCameras.front()->height() * 75u / 100u;

		anyCameraPinhole = std::make_shared<AnyCameraPinhole>(PinholeCamera(rectifiedWidth, rectifiedHeight, Numeric::deg2rad(70)));

		const SquareMatrix3 fisheye_R_target = SquareMatrix3(true);

		if (!CV::FrameInterpolatorBilinear::Comfort::resampleCameraImage(frameToRectify, *anyCameras.front(), fisheye_R_target, *anyCameraPinhole, frameToUse, /* source_OLT_target */ nullptr, worker))
		{
			Log::error() << "Failed to rectify the frame";

			return false;
		}

		frameToUse.setTimestamp(frames.front().timestamp());

		isFrameUndistorted = true;
    }

	ocean_assert(anyCameraPinhole && anyCameraPinhole->isValid());
	ocean_assert(frameToUse.isValid() && frameToUse.width() == anyCameraPinhole->width() && frameToUse.height() == anyCameraPinhole->height() && frameToUse.timestamp().isValid());

	return determinePoses(frameToUse, anyCameraPinhole->actualCamera(), isFrameUndistorted, transformations, world_R_camera, worker);
}

}

}
