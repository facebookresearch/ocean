/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/advanced/AdvancedFrameShrinker.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

bool AdvancedFrameShrinker::divideByTwo(const Frame& source, Frame& target, const Frame& sourceMask, Frame& targetMask, const bool handleMask, bool* targetMaskHasPixel, Worker* worker)
{
	ocean_assert(source.isValid() && sourceMask.isValid());
	ocean_assert(source.pixelOrigin() == sourceMask.pixelOrigin());

	ocean_assert(source.numberPlanes() == 1u);
	ocean_assert(FrameType::formatIsGeneric(sourceMask.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 1u));

	ocean_assert(source.width() >= 2u && source.height() >= 2u);

	if (source.width() >= 2u && source.height() >= 2u
			&& source.numberPlanes() == 1u
			&& sourceMask.isPixelFormatCompatible(FrameType::FORMAT_Y8)
			&& source.pixelOrigin() == sourceMask.pixelOrigin())
	{
		if (!target.set(FrameType(source, source.width() / 2u, source.height() / 2u), false /*forceOwner*/, true /*forceWritable*/))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		if (!targetMask.set(FrameType(sourceMask, sourceMask.width() / 2u, sourceMask.height() / 2u), false /*forceOwner*/, true /*forceWritable*/))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		switch (source.channels())
		{
			case 1u:
				divideByTwo8BitPerChannel<1u>(source.constdata<uint8_t>(), target.data<uint8_t>(), sourceMask.constdata<uint8_t>(), targetMask.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), sourceMask.paddingElements(), targetMask.paddingElements(), handleMask, targetMaskHasPixel, worker);
				return true;

			case 2u:
				divideByTwo8BitPerChannel<2u>(source.constdata<uint8_t>(), target.data<uint8_t>(), sourceMask.constdata<uint8_t>(), targetMask.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), sourceMask.paddingElements(), targetMask.paddingElements(), handleMask, targetMaskHasPixel, worker);
				return true;

			case 3u:
				divideByTwo8BitPerChannel<3u>(source.constdata<uint8_t>(), target.data<uint8_t>(), sourceMask.constdata<uint8_t>(), targetMask.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), sourceMask.paddingElements(), targetMask.paddingElements(), handleMask, targetMaskHasPixel, worker);
				return true;

			case 4u:
				divideByTwo8BitPerChannel<4u>(source.constdata<uint8_t>(), target.data<uint8_t>(), sourceMask.constdata<uint8_t>(), targetMask.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), sourceMask.paddingElements(), targetMask.paddingElements(), handleMask, targetMaskHasPixel, worker);
				return true;
		}
	}

	ocean_assert(false && "Invalid pixel format!");
	return false;
}

}

}

}
