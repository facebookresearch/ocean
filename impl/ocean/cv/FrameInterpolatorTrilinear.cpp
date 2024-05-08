/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/FrameInterpolatorTrilinear.h"

namespace Ocean
{

namespace CV
{

bool FrameInterpolatorTrilinear::resize(const FramePyramid& source, uint8_t* target, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source.isValid() && target != nullptr);
	ocean_assert(source.finestWidth() > 0u && source.finestHeight() > 0u);
	ocean_assert(targetWidth > 0u && targetHeight > 0u);

	FrameType::PixelFormat pixelFormat = source.finestLayer().pixelFormat();
	ocean_assert(FrameType::dataType(pixelFormat) == FrameType::DT_UNSIGNED_INTEGER_8);

	if (FrameType::numberPlanes(pixelFormat) == 1u && FrameType::dataType(pixelFormat) == FrameType::DT_UNSIGNED_INTEGER_8)
	{
		switch (FrameType::channels(pixelFormat))
		{
			case 1u:
				resize8BitPerChannel<1u>(source, target, targetWidth, targetHeight, targetPaddingElements, worker);
				return true;

			case 2u:
				resize8BitPerChannel<2u>(source, target, targetWidth, targetHeight, targetPaddingElements,worker);
				return true;

			case 3u:
				resize8BitPerChannel<3u>(source, target, targetWidth, targetHeight, targetPaddingElements,worker);
				return true;

			case 4u:
				resize8BitPerChannel<4u>(source, target, targetWidth, targetHeight, targetPaddingElements,worker);
				return true;
		}
	}

	ocean_assert(false && "Pixel format of frame type needs to be zipped and 1 Byte per color channel!");
	return false;
}

bool FrameInterpolatorTrilinear::resize(const FramePyramid& source, Frame& target, Worker* worker)
{
	ocean_assert(source.isValid() && target.isValid());
	ocean_assert(source.finestLayer().pixelFormat() && target.pixelFormat());

	if (source.finestLayer().pixelFormat() != target.pixelFormat() || source.finestLayer().pixelOrigin() != target.pixelOrigin())
	{
		return false;
	}

	return resize(source, target.data<uint8_t>(), target.width(), target.height(), target.paddingElements(), worker);
}

bool FrameInterpolatorTrilinear::resize(const Frame& source, Frame& target, Worker* worker)
{
	ocean_assert(source.isValid());

	const FramePyramid sourcePyramid(source, FramePyramid::AS_MANY_LAYERS_AS_POSSIBLE, false /*copyFirstLayer*/, worker);

	return resize(sourcePyramid, target, worker);
}

bool FrameInterpolatorTrilinear::resize(Frame& frame, const unsigned int newWidth, const unsigned int newHeight, Worker* worker)
{
	if (frame.width() == newWidth && frame.height() == newHeight)
	{
		return true;
	}

	ocean_assert(frame.isValid());
	ocean_assert(newWidth >= 1u && newHeight >= 1u);

	Frame tmp(FrameType(frame, newWidth, newHeight));

	if (!resize(frame, tmp, worker))
	{
		return false;
	}

	frame = std::move(tmp);

	return true;
}

}

}
