/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/FrameMean.h"

namespace Ocean
{

namespace CV
{

Frame FrameMean::meanFrame(FrameProviderInterface& frameProviderInterface, const FrameType::PixelFormat& targetPixelFormat, const unsigned int firstFrameIndex, const unsigned int lastFrameIndex, Worker* worker, bool* abort)
{
	ocean_assert(frameProviderInterface.isInitialized());

	if (FrameType::numberPlanes(targetPixelFormat) == 1u && FrameType::dataType(targetPixelFormat) == FrameType::DT_UNSIGNED_INTEGER_8)
	{
		switch (FrameType::channels(targetPixelFormat))
		{
			case 1u:
				return meanFrame8BitPerChannel<1u>(frameProviderInterface, targetPixelFormat, firstFrameIndex, lastFrameIndex, worker, abort);

			case 2u:
				return meanFrame8BitPerChannel<2u>(frameProviderInterface, targetPixelFormat, firstFrameIndex, lastFrameIndex, worker, abort);

			case 3u:
				return meanFrame8BitPerChannel<3u>(frameProviderInterface, targetPixelFormat, firstFrameIndex, lastFrameIndex, worker, abort);

			case 4u:
				return meanFrame8BitPerChannel<4u>(frameProviderInterface, targetPixelFormat, firstFrameIndex, lastFrameIndex, worker, abort);
		}
	}

	ocean_assert(false && "Invalid frame type!");
	return Frame();
}

bool FrameMean::addToFrameIndividually(const Frame& source, const Frame& mask, Frame& target, Frame& denominators, const uint8_t nonMaskValue, Worker* worker)
{
	ocean_assert(source.isValid() && mask.isValid() && target.isValid() && denominators.isValid());

	ocean_assert(source.frameType() == FrameType(target, source.pixelFormat()));
	ocean_assert(mask.frameType() == FrameType(denominators, mask.pixelFormat()));

	ocean_assert(source.numberPlanes() == 1u);
	ocean_assert(source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	ocean_assert(target.dataType() == FrameType::DT_UNSIGNED_INTEGER_32);
	ocean_assert(denominators.dataType() == FrameType::DT_UNSIGNED_INTEGER_32);

	if (source.frameType() == FrameType(target, source.pixelFormat()) && mask.frameType() == FrameType(denominators, mask.pixelFormat()) && source.numberPlanes() == 1u && source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8
			&& target.dataType() == FrameType::DT_UNSIGNED_INTEGER_32 && denominators.dataType() == FrameType::DT_UNSIGNED_INTEGER_32)
	{
		switch (source.channels())
		{
			case 1u:
				addToFrameIndividually8BitPerChannel<1u>(source.constdata<uint8_t>(), mask.constdata<uint8_t>(), target.data<uint32_t>(), denominators.data<uint32_t>(), source.width(), source.height(), source.paddingElements(), mask.paddingElements(), target.paddingElements(), denominators.paddingElements(), nonMaskValue, worker);
				return true;

			case 2u:
				addToFrameIndividually8BitPerChannel<2u>(source.constdata<uint8_t>(), mask.constdata<uint8_t>(), target.data<uint32_t>(), denominators.data<uint32_t>(), source.width(), source.height(), source.paddingElements(), mask.paddingElements(), target.paddingElements(), denominators.paddingElements(), nonMaskValue, worker);
				return true;

			case 3u:
				addToFrameIndividually8BitPerChannel<3u>(source.constdata<uint8_t>(), mask.constdata<uint8_t>(), target.data<uint32_t>(), denominators.data<uint32_t>(), source.width(), source.height(), source.paddingElements(), mask.paddingElements(), target.paddingElements(), denominators.paddingElements(), nonMaskValue, worker);
				return true;

			case 4u:
				addToFrameIndividually8BitPerChannel<4u>(source.constdata<uint8_t>(), mask.constdata<uint8_t>(), target.data<uint32_t>(), denominators.data<uint32_t>(), source.width(), source.height(), source.paddingElements(), mask.paddingElements(), target.paddingElements(), denominators.paddingElements(), nonMaskValue, worker);
				return true;
		}
	}

	ocean_assert(false && "Invalid frame type!");
	return false;
}

}

}
