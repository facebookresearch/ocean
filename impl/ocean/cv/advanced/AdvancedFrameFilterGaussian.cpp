/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/advanced/AdvancedFrameFilterGaussian.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

bool AdvancedFrameFilterGaussian::Comfort::filter(Frame& frame, Frame& mask, const unsigned int filterSize, const uint8_t maskValue, Worker* worker)
{
	ocean_assert(frame.isValid() && mask.isValid());
	ocean_assert(filterSize >= 1u && filterSize % 2u == 1u);

	const unsigned int filterSize_2 = filterSize / 2u;

	if (!frame.isValid() || frame.width() <= filterSize_2 || frame.height() <= filterSize_2 || filterSize == 0u || filterSize % 2u != 1u)
	{
		return false;
	}

	if (!frame.isFrameTypeCompatible(FrameType(mask.frameType(), frame.pixelFormat()), false))
	{
		return false;
	}

	ocean_assert(frame.numberPlanes() == 1u);
	if (frame.numberPlanes() != 1u)
	{
		return false;
	}

	if (frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
	{
		AdvancedFrameFilterGaussian::filter<uint8_t, uint32_t>(frame.data<uint8_t>(), mask.data<uint8_t>(), frame.width(), frame.height(), frame.channels(), frame.paddingElements(), mask.paddingElements(), filterSize, filterSize, -1.0f, maskValue, worker);

		return true;
	}

	if (frame.dataType() == FrameType::DT_SIGNED_FLOAT_32)
	{
		AdvancedFrameFilterGaussian::filter<float, float>(frame.data<float>(), mask.data<uint8_t>(), frame.width(), frame.height(), frame.channels(), frame.paddingElements(), mask.paddingElements(), filterSize, filterSize, -1.0f, maskValue, worker);

		return true;
	}

	ocean_assert(false && "Unexpected pixel format!");
	return false;
}

bool AdvancedFrameFilterGaussian::Comfort::filter(const Frame& source, const Frame& sourceMask, Frame& target, Frame& targetMask, const unsigned int filterSize, const uint8_t maskValue, Worker* worker)
{
	ocean_assert(source.isValid() && sourceMask.isValid());
	ocean_assert(filterSize >= 1u && filterSize % 2u == 1u);

	const unsigned int filterSize_2 = filterSize / 2u;

	if (!source.isValid() || source.width() <= filterSize_2 || source.height() <= filterSize_2 || filterSize == 0u || filterSize % 2u != 1u)
	{
		return false;
	}

	if (!source.isFrameTypeCompatible(FrameType(sourceMask.frameType(), source.pixelFormat()), false))
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
		if (!target.set(source.frameType(), false, true))
		{
			return false;
		}
		if (!targetMask.set(sourceMask.frameType(), false, true))
		{
			return false;
		}

		AdvancedFrameFilterGaussian::filter<uint8_t, uint32_t>(source.constdata<uint8_t>(), sourceMask.constdata<uint8_t>(), target.data<uint8_t>(), targetMask.data<uint8_t>(), source.width(), source.height(), source.channels(), source.paddingElements(), sourceMask.paddingElements(), target.paddingElements(), targetMask.paddingElements(), filterSize, filterSize, -1.0f, maskValue, worker);

		return true;
	}

	if (source.dataType() == FrameType::DT_SIGNED_FLOAT_32)
	{
		if (!target.set(source.frameType(), false, true))
		{
			return false;
		}
		if (!targetMask.set(sourceMask.frameType(), false, true))
		{
			return false;
		}

		AdvancedFrameFilterGaussian::filter<float, float>(source.constdata<float>(), sourceMask.constdata<uint8_t>(), target.data<float>(), targetMask.data<uint8_t>(), source.width(), source.height(), source.channels(), source.paddingElements(), sourceMask.paddingElements(), target.paddingElements(), targetMask.paddingElements(), filterSize, filterSize, -1.0f, maskValue, worker);

		return true;
	}

	ocean_assert(false && "Unexpected pixel format!");
	return false;
}

}

}

}
