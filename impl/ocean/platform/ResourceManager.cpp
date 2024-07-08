/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/ResourceManager.h"

#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameInterpolatorBilinear.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace Platform
{

ResourceManager::ResourceManager()
{
	// nothing to do here
}

ResourceManager::~ResourceManager()
{
	release();
}

void ResourceManager::addFrameResolution(const unsigned int id, const FrameRef& frame)
{
	ocean_assert(frame && frame->isValid());

	if (frame.isNull() || frame->width() == 0u || frame->height() == 0u)
	{
		return;
	}

	const ScopedLock scopedLock(lock_);

	ocean_assert(bufferMap_.find(id) == bufferMap_.cend() && "This id has already been used for a buffer!");

	FrameMap::iterator iNativeFrame = nativeFrames_.find(id);
	if (iNativeFrame == nativeFrames_.end())
	{
		iNativeFrame = nativeFrames_.emplace(id, FrameRefs()).first;
	}

	ocean_assert(iNativeFrame != nativeFrames_.end());
	FrameRefs& resolutions = iNativeFrame->second;

	for (FrameRef& resolution : resolutions)
	{
		ocean_assert(resolution);

		if (resolution->width() == frame->width() && resolution->height() == frame->height())
		{
			ocean_assert(false && "This resolution is defined already, however we will overwrite this resolution!");
			resolution = frame;

			return;
		}
	}

	resolutions.emplace_back(frame);
}

void ResourceManager::addData(const unsigned int id, const BufferRef& buffer)
{
	ocean_assert(buffer && !buffer->empty());

	if (buffer.isNull())
	{
		return;
	}

	const ScopedLock scopedLock(lock_);

	ocean_assert(bufferMap_.find(id) == bufferMap_.cend() && "This id has already been used for a buffer!");
	ocean_assert(nativeFrames_.find(id) == nativeFrames_.cend() && "This id has already been used for a frame!");

	bufferMap_[id] = buffer;
}

void ResourceManager::addData(const unsigned int id, Buffer&& buffer)
{
	addData(id, BufferRef(new Buffer(std::move(buffer))));
}

FrameRef ResourceManager::frame(const unsigned int id, const unsigned int width, const unsigned int height, const bool downscaleIfNecessary)
{
	const ScopedLock scopedLock(lock_);

	// check whether the frame exists
	const FrameMap::const_iterator iF = nativeFrames_.find(id);
	if (iF == nativeFrames_.cend())
	{
		ocean_assert(interpolatedFrames_.find(id) == interpolatedFrames_.cend());
		return FrameRef();
	}

	const FrameRefs& resolutions = iF->second;
	if (resolutions.empty())
	{
		ocean_assert(false && "We expect at least one entry!");
		return FrameRef();
	}

	// case 0: width and height is not specified, here downscaleIfNecessary has no meaning
	if (width == 0u && height == 0u)
	{
		// we return the native frame
		return resolutions.front();
	}

	// case 1: width and height is specified
	if (width != 0u && height != 0u)
	{
		// check whether we have the defined frame size exactly

		for (const FrameRef& resolution : resolutions)
		{
			if (resolution->width() == width && resolution->height() == height)
			{
				return resolution;
			}
		}

		if (!downscaleIfNecessary)
		{
			// the caller does not want a down scaled frame, thus we return the first frame which is larger than specified
			for (const FrameRef& resolution : resolutions)
			{
				if (resolution->width() >= width && resolution->height() >= height)
				{
					return resolution;
				}
			}

			// there is no frame equal or larger to the specified frame size, thus we return the largest frame

			return largestFrame(resolutions);
		}

		return interpolatedFrame(id, width, height);
	}

	// case 2: width is specified, height is not specified
	if (width != 0u)
	{
		// check whether we have the defined frame size (width) exactly

		for (const FrameRef& resolution : resolutions)
		{
			if (resolution->width() == width)
			{
				return resolution;
			}
		}

		FrameRef largerFrame;
		for (const FrameRef& resolution : resolutions)
		{
			if (resolution->width() >= width)
			{
				largerFrame = resolution;
				break;
			}
		}

		if (!downscaleIfNecessary)
		{
			// the caller does not want a down scaled frame, thus we return the frame slightly larger than specified

			if (largerFrame)
			{
				return largerFrame;
			}

			// there is no frame equal or larger to the specified frame size, thus we return the largest frame

			return largestFrame(resolutions);
		}

		if (largerFrame.isNull())
		{
			largerFrame = largestFrame(resolutions);
		}

		ocean_assert(largerFrame->width() != 0u);
		const unsigned int interpolatedHeight = (unsigned int)(max(1, Numeric::round32(Scalar(largerFrame->height() * width) / Scalar(largerFrame->width()))));

		return interpolatedFrame(id, width, interpolatedHeight);
	}

	// case 3: width is not specified, height is specified
	if (height != 0u)
	{
		// check whether we have the defined frame size (height) exactly

		for (const FrameRef& resolution : resolutions)
		{
			if (resolution->height() == height)
			{
				return resolution;
			}
		}

		FrameRef largerFrame;
		for (const FrameRef& resolution : resolutions)
		{
			if (resolution->height() >= height)
			{
				largerFrame = resolution;
				break;
			}
		}

		if (!downscaleIfNecessary)
		{
			// the caller does not want a down scaled frame, thus we return the frame slightly larger than specified

			if (largerFrame)
			{
				return largerFrame;
			}

			// there is no frame equal or larger to the specified frame size, thus we return the largest frame

			return largestFrame(resolutions);
		}

		if (largerFrame.isNull())
		{
			largerFrame = largestFrame(resolutions);
		}

		ocean_assert(largerFrame->height() != 0u);
		const unsigned int interpolatedWidth = (unsigned int)(max(1, Numeric::round32(Scalar(largerFrame->width() * height) / Scalar(largerFrame->height()))));

		return interpolatedFrame(id, interpolatedWidth, height);
	}

	ocean_assert(false && "This must never happen!");
	return FrameRef();
}

FrameRef ResourceManager::frame(const unsigned int id, const double scaleFactor)
{
	ocean_assert(scaleFactor > 0.0);
	if (scaleFactor <= 0)
	{
		return FrameRef();
	}

	const ScopedLock scopedLock(lock_);

	// check whether the frame exists
	const FrameMap::const_iterator iF = nativeFrames_.find(id);
	if (iF == nativeFrames_.cend())
	{
		ocean_assert(interpolatedFrames_.find(id) == interpolatedFrames_.cend());
		return FrameRef();
	}

	const FrameRefs& resolutions = iF->second;
	if (resolutions.empty())
	{
		ocean_assert(false && "We expect at least one entry!");
		return FrameRef();
	}

	const double adjustedWidth = double(resolutions.front()->width()) * scaleFactor;
	const double adjustedHeight = double(resolutions.front()->height()) * scaleFactor;

	ocean_assert(adjustedWidth >= 0 && adjustedHeight >= 0);

	const unsigned int width = max(1u, (unsigned int)(adjustedWidth + 0.5));
	const unsigned int height = max(1u, (unsigned int)(adjustedHeight + 0.5));

	return frame(id, width, height);
}

ResourceManager::BufferRef ResourceManager::data(const unsigned int id)
{
	const ScopedLock scopedLock(lock_);

	const BufferMap::const_iterator i = bufferMap_.find(id);
	if (i == bufferMap_.cend())
	{
		return BufferRef();
	}

	return i->second;
}

void ResourceManager::release()
{
	const ScopedLock scopedLock(lock_);

	nativeFrames_.clear();
	interpolatedFrames_.clear();
	bufferMap_.clear();
}

FrameRef ResourceManager::interpolatedFrame(const unsigned int id, const unsigned int width, const unsigned int height)
{
	ocean_assert(width != 0u && height != 0u);

	const ScopedLock scopedLock(lock_);

	FrameMap::iterator iIF = interpolatedFrames_.find(id);
	if (iIF == interpolatedFrames_.cend())
	{
		iIF = interpolatedFrames_.emplace(id, FrameRefs()).first;
	}

	ocean_assert(iIF != interpolatedFrames_.end());
	const FrameRefs& interpolatedResolutions = iIF->second;

	for (const FrameRef& interpolatedResolution : interpolatedResolutions)
	{
		if (interpolatedResolution->width() == width && interpolatedResolution->height() == height)
		{
			return interpolatedResolution;
		}
	}

	// we do not have an already existing interpolated frame, thus we have to create one

	const FrameMap::const_iterator iNF = nativeFrames_.find(id);
	ocean_assert(iNF != nativeFrames_.cend());

	FrameRef sourceFrame;

	for (const FrameRef& interpolatedFrame : iNF->second)
	{
		if (interpolatedFrame->width() >= width && interpolatedFrame->height() >= height)
		{
			sourceFrame = interpolatedFrame;
			break;
		}
	}

	if (sourceFrame.isNull())
	{
		// we could not find a native frame larger than the specified frame dimension, thus we take the largest native frame
		sourceFrame = largestFrame(iNF->second);
	}

	// we use the bilinear interpolation (and not the tri-linear interpolation as the bilinear interpolation creates sharper results)
	Frame targetFrame(FrameType(*sourceFrame, width, height));
	const bool result = CV::FrameInterpolatorBilinear::Comfort::resize(*sourceFrame, targetFrame, WorkerPool::get().conditionalScopedWorker(targetFrame.pixels() > 100u * 100u)());
	ocean_assert_and_suppress_unused(result, result);

	FrameRef interpolatedFrame(new Frame(std::move(targetFrame)));

	iIF->second.emplace_back(interpolatedFrame);
	return interpolatedFrame;
}

FrameRef ResourceManager::largestFrame(const FrameRefs& frames)
{
	ocean_assert(!frames.empty());

	if (frames.empty())
	{
		return FrameRef();
	}

	FrameRef largest(frames.front());

	for (const FrameRef& frame : frames)
	{
		ocean_assert(frame);

		if (frame->width() * frame->height() > largest->width() * largest->height())
		{
			largest = frame;
		}
	}

	return largest;
}

}

}
