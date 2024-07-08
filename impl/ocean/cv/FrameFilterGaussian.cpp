/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/FrameFilterGaussian.h"

#include "ocean/base/Frame.h"

namespace Ocean
{

namespace CV
{

bool FrameFilterGaussian::filter(const Frame& source, Frame& target, const unsigned int filterSize, Worker* worker, ReusableMemory* reusableMemory)
{
	ocean_assert(source.isValid());
	ocean_assert(filterSize >= 1u && filterSize % 2u == 1u);

	if (!source.isValid() || source.width() < filterSize || source.height() < filterSize || filterSize == 0u || filterSize % 2u != 1u)
	{
		return false;
	}

	ocean_assert(source.numberPlanes() == 1u);
	if (source.numberPlanes() != 1u)
	{
		return false;
	}

	if (source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
	{
		if (!target.set(source.frameType(), false /*forceOwner*/, true /*forceWritable*/))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		return filter<uint8_t, uint32_t>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.channels(), source.paddingElements(), target.paddingElements(), filterSize, filterSize, -1.0f, worker, reusableMemory, Processor::get().instructions());
	}

	if (source.dataType() == FrameType::DT_SIGNED_FLOAT_32)
	{
		if (!target.set(source.frameType(), false /*forceOwner*/, true /*forceWritable*/))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		return filter<float, float>(source.constdata<float>(), target.data<float>(), source.width(), source.height(), source.channels(), source.paddingElements(), target.paddingElements(), filterSize, filterSize, -1.0f, worker, reusableMemory, Processor::get().instructions());
	}

	ocean_assert(false && "Unexpected pixel format!");
	return false;
}

bool FrameFilterGaussian::filter(Frame& frame, const unsigned int filterSize, Worker* worker, ReusableMemory* reusableMemory)
{
	ocean_assert(frame.isValid());
	ocean_assert(filterSize >= 1u && filterSize % 2u == 1u);

	if (!frame.isValid() || frame.width() < filterSize || frame.height() < filterSize || filterSize == 0u || filterSize % 2u != 1u)
	{
		return false;
	}

	if (frame.isReadOnly())
	{
		Frame tmpFrame;

		if (!filter(frame, tmpFrame, filterSize, worker))
		{
			return false;
		}

		frame = std::move(tmpFrame);

		return true;
	}

	ocean_assert(frame.numberPlanes() == 1u);
	if (frame.numberPlanes() != 1u)
	{
		return false;
	}

	if (frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
	{
		ocean_assert(frame.constdata<uint8_t>() != nullptr && frame.data<uint8_t>() != nullptr);

		return filter<uint8_t, uint32_t>(frame.constdata<uint8_t>(), frame.data<uint8_t>(), frame.width(), frame.height(), frame.channels(), frame.paddingElements(), frame.paddingElements(), filterSize, filterSize, -1.0f, worker, reusableMemory, Processor::get().instructions());
	}

	if (frame.dataType() == FrameType::DT_SIGNED_FLOAT_32)
	{
		ocean_assert(frame.constdata<float>() != nullptr && frame.data<float>() != nullptr);

		return filter<float, float>(frame.constdata<float>(), frame.data<float>(), frame.width(), frame.height(), frame.channels(), frame.paddingElements(), frame.paddingElements(), filterSize, filterSize, -1.0f, worker, reusableMemory, Processor::get().instructions());
	}

	ocean_assert(false && "Unexpected pixel format!");
	return false;
}

}

}
