/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/UndistortedBackground.h"
#include "ocean/rendering/Engine.h"

#include "ocean/io/CameraCalibrationManager.h"

#include "ocean/math/AnyCamera.h"

namespace Ocean
{

namespace Rendering
{

UndistortedBackground::UndistortedBackground() :
	Background()
{
	// nothing to do here
}

UndistortedBackground::~UndistortedBackground()
{
	// nothing to do here
}

const Media::FrameMediumRef& UndistortedBackground::medium() const
{
	return medium_;
}

const PinholeCamera& UndistortedBackground::camera() const
{
	return mediumCamera_;
}

UndistortedBackground::DisplayType UndistortedBackground::displayType() const
{
	return DT_FASTEST;
}

void UndistortedBackground::setMedium(const Media::FrameMediumRef& medium)
{
	const ScopedLock scopedLock(objectLock);

	medium_ = medium;
	mediumCamera_ = PinholeCamera();

	if (texture_)
	{
		texture_->setMedium(medium);
	}
}

bool UndistortedBackground::setDisplayType(const DisplayType /*type*/)
{
	throw NotSupportedException("UndistortedBackground::setDisplayType() is not supported.");
}

bool UndistortedBackground::setDistance(const Scalar distance)
{
	if (distance <= Numeric::eps())
	{
		return false;
	}

	const ScopedLock scopedLock(objectLock);

	if (Numeric::isEqual(backgroundDistance, distance))
	{
		return true;
	}

	backgroundDistance = distance;
	cameraChanged_ = true;
	return true;
}

UndistortedBackground::ObjectType UndistortedBackground::type() const
{
	return TYPE_UNDISTORTED_BACKGROUND;
}

void UndistortedBackground::onDynamicUpdate(const ViewRef& /*view*/, const Timestamp timestamp)
{
	const ScopedLock scopedLock(objectLock);

	if (medium_)
	{
		SharedAnyCamera anyCamera;
		const FrameRef frame = medium_->frame(timestamp, &anyCamera);

		if (anyCamera)
		{
			if (anyCamera->name() == AnyCameraPinhole::WrappedCamera::name())
			{
				const PinholeCamera& pinholeCamera = ((const AnyCameraPinhole&)(*anyCamera)).actualCamera();

				if (mediumCamera_ != pinholeCamera)
				{
					mediumCamera_ = pinholeCamera;
					cameraChanged_ = true;
				}
			}
			else
			{
				ocean_assert(false && "Current not supported!");
			}
		}
		else
		{
			if (!mediumCamera_.isValid() && frame)
			{
				Log::warning() << "Using default field of view in UndistortedBackground node";

				mediumCamera_ = PinholeCamera(frame->width(), frame->height(), Numeric::deg2rad(60));
				cameraChanged_ = true;
			}
		}
	}

	if (cameraChanged_ && mediumCamera_ && texture_ && texture_->isValid())
	{
		onMediumCameraChanged(timestamp);
		cameraChanged_ = false;
	}
}

}

}
