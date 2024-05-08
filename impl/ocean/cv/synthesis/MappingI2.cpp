/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/synthesis/MappingI2.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

MappingI2::MappingI2() :
	MappingI()
{
	// nothing to do here
}

MappingI2::MappingI2(const MappingI2& pixelMapping) :
	MappingI(pixelMapping)
{
	// nothing to do here
}

MappingI2::MappingI2(MappingI2&& mapping) noexcept :
	MappingI(std::move(mapping))
{
	// nothing to do here
}

MappingI2::MappingI2(const unsigned int width, const unsigned int height) :
	MappingI(width, height)
{
	// nothing to do here
}

template <unsigned int tChannels>
void MappingI2::applyTwoFrameMapping8BitPerChannel(uint8_t* const target, const uint8_t* const source, const unsigned int sourceWidth, Worker* worker) const
{
	ocean_assert(target && source);
	ocean_assert(sourceWidth != 0u);

	if (worker)
	{
		worker->executeFunction(Worker::Function::create(*this, &MappingI2::applyTwoFrameMapping8BitPerChannelSubset<tChannels>, target, source, sourceWidth, 0u, 0u), 0u, height_, 3u, 4u, 40u);
	}
	else
	{
		applyTwoFrameMapping8BitPerChannelSubset<tChannels>(target, source, sourceWidth, 0u, height_);
	}
}

template <unsigned int tChannels>
void MappingI2::applyTwoFrameMapping8BitPerChannelSubset(uint8_t* const target, const uint8_t* const source, const unsigned int sourceWidth, const unsigned int firstRow, const unsigned int numberRows) const
{
	ocean_assert(target && source);
	ocean_assert(sourceWidth != 0u);

	ocean_assert(firstRow + numberRows <= height_);

	const PixelPosition* mappingPointer = mappingI_ + firstRow * width_;

	typedef typename DataType<uint8_t, tChannels>::Type PixelType;

	PixelType* targetPointer = (PixelType*)target + firstRow * width_;

	const PixelType* const targetEnd = (PixelType*)targetPointer + width_ * numberRows;

	while (targetPointer != targetEnd)
	{
		ocean_assert(targetPointer < targetEnd);

		*targetPointer++ = *((const PixelType*)source + mappingPointer->y() * sourceWidth + mappingPointer->x());
		++mappingPointer;
	}
}

}

}

}
