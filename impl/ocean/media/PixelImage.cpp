/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/PixelImage.h"

namespace Ocean
{

namespace Media
{

PixelImage::PixelImage(const std::string& url) :
	Medium(url),
	FrameMedium(url),
	isStarted_(false),
	device_T_camera_(true)
{
	type_ = Type(type_ | PIXEL_IMAGE);
	isValid_ = true;
}

bool PixelImage::isStarted() const
{
	const ScopedLock scopedLock(lock_);

	return isStarted_;
}

HomogenousMatrixD4 PixelImage::device_T_camera() const
{
	const ScopedLock scopedLock(lock_);

	return device_T_camera_;
}

void PixelImage::setDevice_T_camera(const HomogenousMatrixD4& device_T_camera)
{
	ocean_assert(device_T_camera.isValid());

	const ScopedLock scopedLock(lock_);

	device_T_camera_= device_T_camera;
}

bool PixelImage::start()
{
	const ScopedLock scopedLock(lock_);

	isStarted_ = true;

	startTimestamp_.toNow();
	pauseTimestamp_.toInvalid();
	stopTimestamp_ .toInvalid();

	return true;
}

bool PixelImage::pause()
{
	const ScopedLock scopedLock(lock_);

	isStarted_ = false;

	startTimestamp_.toInvalid();
	pauseTimestamp_.toNow();
	stopTimestamp_.toInvalid();

	return true;
}

bool PixelImage::stop()
{
	const ScopedLock scopedLock(lock_);

	isStarted_ = false;

	startTimestamp_.toInvalid();
	pauseTimestamp_.toInvalid();
	stopTimestamp_.toNow();

	return true;
}

Timestamp PixelImage::startTimestamp() const
{
	const ScopedLock scopedLock(lock_);

	return startTimestamp_;
}

Timestamp PixelImage::pauseTimestamp() const
{
	const ScopedLock scopedLock(lock_);

	return pauseTimestamp_;
}

Timestamp PixelImage::stopTimestamp() const
{
	const ScopedLock scopedLock(lock_);

	return stopTimestamp_;
}

bool PixelImage::setPixelImage(Frame&& frame, SharedAnyCamera anyCamera)
{
	const ScopedLock scopedLock(lock_);

	if (isStarted_)
	{
		return deliverNewFrame(std::move(frame), std::move(anyCamera));
	}
	else
	{
		ocean_assert(false && "Pixel image is not started!");
	}

	return false;
}

bool PixelImage::setPixelImage(const Frame& frame, SharedAnyCamera anyCamera)
{
	return setPixelImage(Frame(frame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT), std::move(anyCamera));
}

}

}
