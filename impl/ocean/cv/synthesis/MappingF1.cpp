/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/synthesis/MappingF1.h"

#include "ocean/cv/SumSquareDifferences.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

MappingF1::MappingF1() :
	MappingF()
{
	// nothing to do here
}

MappingF1::MappingF1(const MappingF1& pixelMapping) :
	MappingF(pixelMapping)
{
	// nothing to do here
}

MappingF1::MappingF1(MappingF1&& mapping) noexcept :
	MappingF(std::move(mapping))
{
	// nothing to do here
}

MappingF1::MappingF1(const unsigned int width, const unsigned int height) :
	MappingF(width, height)
{
	const unsigned int windowHalf = 2u; // **TODO** find a good solution to be able to use flexible patch sizes (not always 5x5)

	ocean_assert(int64_t(appearanceCostNormalizationInt8_) * int64_t(sqr(2u * windowHalf + 1u)) * int64_t(sqr(width_) + sqr(height_)) +
				int64_t(sapatialCostNormalizationInt8_) +  int64_t(sqr(2u * windowHalf + 1u)) * int64_t(sqr(255u)) * 26ll < int64_t((unsigned int)(-1)));

	ocean_assert(int64_t(appearanceCostNormalizationInt16_) * int64_t(sqr(2u * windowHalf + 1u)) * int64_t(sqr(width_) + sqr(height_)) +
				int64_t(spatialCostNormalizationInt16_) +  int64_t(sqr(2u * windowHalf + 1u)) * int64_t(sqr(255u)) * 2ll * 26ll < int64_t((unsigned int)(-1)));

	ocean_assert(int64_t(appearanceCostNormalizationInt24_) * int64_t(sqr(2u * windowHalf + 1u)) * int64_t(sqr(width_) + sqr(height_)) +
				int64_t(spatialCostNormalizationInt24_) +  int64_t(sqr(2u * windowHalf + 1u)) * int64_t(sqr(255u)) * 3ll * 26ll < int64_t((unsigned int)(-1)));

	ocean_assert(int64_t(appearanceCostNormalizationInt32_) * int64_t(sqr(2u * windowHalf + 1u)) * int64_t(sqr(width_) + sqr(height_)) +
				int64_t(spatialCostNormalizationInt32_) +  int64_t(sqr(2u * windowHalf + 1u)) * int64_t(sqr(255u)) * 4ll * 26ll < int64_t((unsigned int)(-1)));

	OCEAN_SUPPRESS_UNUSED_WARNING(windowHalf);
}

MappingF1& MappingF1::operator=(const MappingI& right)
{
	const unsigned int oldSize = width_ * height_;
	const unsigned int newSize = right.width_ * right.height_;

	if (oldSize != newSize)
	{
		free(mappingF_);
		mappingF_ = nullptr;

		if (newSize != 0)
		{
			mappingF_ = (Vector2*)malloc(newSize * sizeof(Vector2));
			ocean_assert(mappingF_ != nullptr);
		}
	}

	width_ = right.width_;
	height_ = right.height_;

	appearanceCostNormalizationInt8_ = right.appearanceCostNormalizationInt8_;
	appearanceCostNormalizationInt16_ = right.appearanceCostNormalizationInt16_;
	appearanceCostNormalizationInt24_ = right.appearanceCostNormalizationInt24_;
	appearanceCostNormalizationInt32_ = right.appearanceCostNormalizationInt32_;
	sapatialCostNormalizationInt8_ = right.sapatialCostNormalizationInt8_;
	spatialCostNormalizationInt16_ = right.spatialCostNormalizationInt16_;
	spatialCostNormalizationInt24_ = right.spatialCostNormalizationInt24_;
	spatialCostNormalizationInt32_ = right.spatialCostNormalizationInt32_;

	if (newSize != 0)
	{
		const PixelPosition* source = right();
		Vector2* target = mappingF_;

		const PixelPosition* const sourceEnd = source + width_ * height_;
		while (source != sourceEnd)
		{
			ocean_assert(source < sourceEnd);

			*target++ = Vector2(Scalar(source->x()), Scalar(source->y()));
			source++;
		}
	}

	return *this;
}

void MappingF1::applyMapping(Frame& frame, const Frame& mask, const unsigned int xStart, const unsigned int xWidth, const unsigned int yStart, const unsigned int yHeight, Worker* worker) const
{
	ocean_assert(frame.isValid() && mask.isValid());
	ocean_assert(frame.width() == mask.width() && frame.height() == mask.height());
	ocean_assert(FrameType::formatIsGeneric(mask.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 1u));
	ocean_assert(frame.pixelOrigin() == mask.pixelOrigin());

	ocean_assert(frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);
	ocean_assert(frame.numberPlanes() == 1u);

	switch (frame.channels())
	{
		case 1u:
			applyOneFrameMapping8BitPerChannel<1u>(frame.data<uint8_t>(), mask.constdata<uint8_t>(), frame.paddingElements(), mask.paddingElements(), xStart, xWidth, yStart, yHeight, worker);
			break;

		case 2u:
			applyOneFrameMapping8BitPerChannel<2u>(frame.data<uint8_t>(), mask.constdata<uint8_t>(), frame.paddingElements(), mask.paddingElements(), xStart, xWidth, yStart, yHeight, worker);
			break;

		case 3u:
			applyOneFrameMapping8BitPerChannel<3u>(frame.data<uint8_t>(), mask.constdata<uint8_t>(), frame.paddingElements(), mask.paddingElements(), xStart, xWidth, yStart, yHeight, worker);
			break;

		case 4u:
			applyOneFrameMapping8BitPerChannel<4u>(frame.data<uint8_t>(), mask.constdata<uint8_t>(), frame.paddingElements(), mask.paddingElements(), xStart, xWidth, yStart, yHeight, worker);
			break;

		case 6u:
			applyOneFrameMapping8BitPerChannel<6u>(frame.data<uint8_t>(), mask.constdata<uint8_t>(), frame.paddingElements(), mask.paddingElements(), xStart, xWidth, yStart, yHeight, worker);
			break;

		case 8u:
			applyOneFrameMapping8BitPerChannel<8u>(frame.data<uint8_t>(), mask.constdata<uint8_t>(), frame.paddingElements(), mask.paddingElements(), xStart, xWidth, yStart, yHeight, worker);
			break;

		default:
			ocean_assert(false && "Invalid pixel format!");
	}
}

}

}

}
