/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/segmentation/FrameFilterBlur.h"

namespace Ocean
{

namespace CV
{

namespace Segmentation
{

bool FrameFilterBlur::Comfort::blurMaskRegions(Frame& image, const Frame& mask, const unsigned int blurBorder, RandomGenerator* randomGenerator)
{
	ocean_assert(image.isValid());
	ocean_assert(mask.isValid());

	ocean_assert(image.numberPlanes() == 1u && image.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);
	ocean_assert(mask.isPixelFormatCompatible(FrameType::genericPixelFormat<uint8_t, 1u>()));

	ocean_assert(image.width() == mask.width() && image.height() == mask.height() && image.pixelOrigin() == mask.pixelOrigin());
	ocean_assert(!image.hasAlphaChannel());

	if (!image.isValid() || image.numberPlanes() != 1u || image.dataType() != FrameType::DT_UNSIGNED_INTEGER_8 || image.hasAlphaChannel())
	{
		return false;
	}

	if (!mask.isValid() || !mask.isPixelFormatCompatible(FrameType::genericPixelFormat<uint8_t, 1u>()) || !image.isFrameTypeCompatible(FrameType(mask, image.pixelFormat()), false))
	{
		return false;
	}

	switch (image.channels())
	{
		case 1u:
			return blurMaskRegions8BitPerChannel<1u>(image.data<uint8_t>(), mask.constdata<uint8_t>(), image.width(), image.height(), image.paddingElements(), mask.paddingElements(), blurBorder, randomGenerator);

		case 2u:
			return blurMaskRegions8BitPerChannel<2u>(image.data<uint8_t>(), mask.constdata<uint8_t>(), image.width(), image.height(), image.paddingElements(), mask.paddingElements(), blurBorder, randomGenerator);

		case 3u:
			return blurMaskRegions8BitPerChannel<3u>(image.data<uint8_t>(), mask.constdata<uint8_t>(), image.width(), image.height(), image.paddingElements(), mask.paddingElements(), blurBorder, randomGenerator);

		case 4u:
			return blurMaskRegions8BitPerChannel<4u>(image.data<uint8_t>(), mask.constdata<uint8_t>(), image.width(), image.height(), image.paddingElements(), mask.paddingElements(), blurBorder, randomGenerator);
	}

	ocean_assert(false && "Unsupported number of channels!");
	return false;
}

}

}

}
