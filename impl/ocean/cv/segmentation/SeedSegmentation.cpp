/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/segmentation/SeedSegmentation.h"

namespace Ocean
{

namespace CV
{

namespace Segmentation
{

unsigned int SeedSegmentation::Comfort::seedSegmentation(const Frame& frame, Frame& mask, const PixelPosition& seed, const uint8_t localThreshold, const uint8_t globalThreshold, PixelBoundingBox* boundingBox, const bool setMaskFrameType)
{
	ocean_assert(frame.isValid() && (mask.isValid() || setMaskFrameType || FrameType(frame, mask.pixelFormat()) == mask.frameType()));
	ocean_assert(seed.x() < frame.width() && seed.y() < frame.height());

	if (frame.numberPlanes() == 1u && frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
	{
		if (setMaskFrameType)
		{
			if (!mask.set(FrameType(frame, FrameType::FORMAT_Y8), false /*forceOwner*/, true /*forceWritable*/))
			{
				ocean_assert(false && "This should never happen!");
				return 0u;
			}
		}
		else if (FrameType(frame, FrameType::FORMAT_Y8) != mask.frameType())
		{
			ocean_assert(false && "Invalid mask type!");
			return 0u;
		}

		switch (frame.channels())
		{
			case 1u:
				return SeedSegmentation::seedSegmentation<uint8_t, 1u>(frame.constdata<uint8_t>(), mask.data<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), mask.paddingElements(), seed, localThreshold, globalThreshold, boundingBox);

			case 2u:
				return SeedSegmentation::seedSegmentation<uint8_t, 2u>(frame.constdata<uint8_t>(), mask.data<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), mask.paddingElements(), seed, localThreshold, globalThreshold, boundingBox);

			case 3u:
				return SeedSegmentation::seedSegmentation<uint8_t, 3u>(frame.constdata<uint8_t>(), mask.data<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), mask.paddingElements(), seed, localThreshold, globalThreshold, boundingBox);

			case 4u:
				return SeedSegmentation::seedSegmentation<uint8_t, 4u>(frame.constdata<uint8_t>(), mask.data<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), mask.paddingElements(), seed, localThreshold, globalThreshold, boundingBox);
		}
	}

	ocean_assert(false && "Invalid frame type!");
	return 0u;
}

unsigned int SeedSegmentation::Comfort::iterativeSeedSegmentation(const Frame& frame, Frame& mask, const PixelPosition& seed, const unsigned char localThreshold, const unsigned char minimalGlobalThreshold, const unsigned char maximalGlobalThreshold, const unsigned int maximalIncreaseFactor, PixelBoundingBox* boundingBox, const bool setMaskFrameType, Worker* worker)
{
	ocean_assert(frame && (mask || setMaskFrameType || FrameType(frame, mask.pixelFormat()) == mask.frameType()));
	ocean_assert(seed.x() < frame.width() && seed.y() < frame.height());

	if (frame.numberPlanes() == 1u && frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
	{
		if (setMaskFrameType)
		{
			if (!mask.set(FrameType(frame, FrameType::FORMAT_Y8), false /*forceOwner*/, true /*forceWritable*/))
			{
				ocean_assert(false && "This should never happen!");
				return 0u;
			}
		}
		else if (FrameType(frame, FrameType::FORMAT_Y8) != mask.frameType())
		{
			ocean_assert(false && "Invalid mask type!");
			return 0u;
		}

		switch (frame.channels())
		{
			case 1u:
				return SeedSegmentation::iterativeSeedSegmentation<uint8_t, 1u>(frame.constdata<uint8_t>(), mask.data<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), mask.paddingElements(), seed, localThreshold, minimalGlobalThreshold, maximalGlobalThreshold, maximalIncreaseFactor, boundingBox, worker);

			case 2u:
				return SeedSegmentation::iterativeSeedSegmentation<uint8_t, 2u>(frame.constdata<uint8_t>(), mask.data<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), mask.paddingElements(), seed, localThreshold, minimalGlobalThreshold, maximalGlobalThreshold, maximalIncreaseFactor, boundingBox, worker);

			case 3u:
				return SeedSegmentation::iterativeSeedSegmentation<uint8_t, 3u>(frame.constdata<uint8_t>(), mask.data<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), mask.paddingElements(), seed, localThreshold, minimalGlobalThreshold, maximalGlobalThreshold, maximalIncreaseFactor, boundingBox, worker);

			case 4u:
				return SeedSegmentation::iterativeSeedSegmentation<uint8_t, 4u>(frame.constdata<uint8_t>(), mask.data<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), mask.paddingElements(), seed, localThreshold, minimalGlobalThreshold, maximalGlobalThreshold, maximalIncreaseFactor, boundingBox, worker);
		}
	}

	ocean_assert(false && "Invalid frame type!");
	return 0u;
}

}

}

}
