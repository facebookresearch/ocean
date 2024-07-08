/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/FrameInterpolatorBicubic.h"

namespace Ocean
{

namespace CV
{

bool FrameInterpolatorBicubic::resize(const uint8_t* source, uint8_t* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const FrameType::PixelFormat pixelFormat, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(sourceWidth != 0u && sourceHeight != 0u);
	ocean_assert(targetWidth != 0u && targetHeight != 0u);

	if (FrameType::numberPlanes(pixelFormat) == 1u && FrameType::dataType(pixelFormat) == FrameType::DT_UNSIGNED_INTEGER_8)
	{
		switch (FrameType::channels(pixelFormat))
		{
			case 1u:
				resize8BitPerChannel<1u>(source, target, sourceWidth, sourceHeight, targetWidth, targetHeight, sourcePaddingElements, targetPaddingElements, worker);
				return true;

			case 2u:
				resize8BitPerChannel<2u>(source, target, sourceWidth, sourceHeight, targetWidth, targetHeight, sourcePaddingElements, targetPaddingElements, worker);
				return true;

			case 3u:
				resize8BitPerChannel<3u>(source, target, sourceWidth, sourceHeight, targetWidth, targetHeight, sourcePaddingElements, targetPaddingElements, worker);
				return true;

			case 4u:
				resize8BitPerChannel<4u>(source, target, sourceWidth, sourceHeight, targetWidth, targetHeight, sourcePaddingElements, targetPaddingElements, worker);
				return true;
		}
	}

	ocean_assert(false && "Invalid frame type!");
	return false;
}

}

}
