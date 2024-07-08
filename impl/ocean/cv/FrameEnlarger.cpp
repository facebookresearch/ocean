/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/FrameEnlarger.h"

namespace Ocean
{

namespace CV
{

bool FrameEnlarger::Comfort::addBorder(const Frame& source, Frame& target, const unsigned int borderSizeLeft, const unsigned int borderSizeTop, const unsigned int borderSizeRight, const unsigned int borderSizeBottom, const void* color)
{
	ocean_assert(source.isValid());
	ocean_assert(color != nullptr);

	if (source.numberPlanes() == 1u)
	{
		constexpr bool targetForceOwner = false;
		constexpr bool targetForceWritable = true;

		if (source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
		{
			if (!target.set(FrameType(source, source.width() + borderSizeLeft + borderSizeRight, source.height() + borderSizeTop + borderSizeBottom), targetForceOwner, targetForceWritable))
			{
				ocean_assert(false && "This should never happen!");
				return false;
			}

			switch (source.channels())
			{
				case 1u:
					FrameEnlarger::addBorder<uint8_t, 1u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), borderSizeLeft, borderSizeTop, borderSizeRight, borderSizeBottom, (const uint8_t*)color, source.paddingElements(), target.paddingElements());
					return true;

				case 2u:
					FrameEnlarger::addBorder<uint8_t, 2u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), borderSizeLeft, borderSizeTop, borderSizeRight, borderSizeBottom, (const uint8_t*)color, source.paddingElements(), target.paddingElements());
					return true;

				case 3u:
					FrameEnlarger::addBorder<uint8_t, 3u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), borderSizeLeft, borderSizeTop, borderSizeRight, borderSizeBottom, (const uint8_t*)color, source.paddingElements(), target.paddingElements());
					return true;

				case 4u:
					FrameEnlarger::addBorder<uint8_t, 4u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), borderSizeLeft, borderSizeTop, borderSizeRight, borderSizeBottom, (const uint8_t*)color, source.paddingElements(), target.paddingElements());
					return true;
			}
		}

		else if (source.dataType() == FrameType::DT_SIGNED_FLOAT_32)
		{
			if (!target.set(FrameType(source, source.width() + borderSizeLeft + borderSizeRight, source.height() + borderSizeTop + borderSizeBottom), targetForceOwner, targetForceWritable))
			{
				ocean_assert(false && "This should never happen!");
				return false;
			}

			ocean_assert(FrameType::dataIsAligned<float>(color));

			switch (source.channels())
			{
				case 1u:
					FrameEnlarger::addBorder<float, 1u>(source.constdata<float>(), target.data<float>(), source.width(), source.height(), borderSizeLeft, borderSizeTop, borderSizeRight, borderSizeBottom, (const float*)color, source.paddingElements(), target.paddingElements());
					return true;

				case 2u:
					FrameEnlarger::addBorder<float, 2u>(source.constdata<float>(), target.data<float>(), source.width(), source.height(), borderSizeLeft, borderSizeTop, borderSizeRight, borderSizeBottom, (const float*)color, source.paddingElements(), target.paddingElements());
					return true;

				case 3u:
					FrameEnlarger::addBorder<float, 3u>(source.constdata<float>(), target.data<float>(), source.width(), source.height(), borderSizeLeft, borderSizeTop, borderSizeRight, borderSizeBottom, (const float*)color, source.paddingElements(), target.paddingElements());
					return true;

				case 4u:
					FrameEnlarger::addBorder<float, 4u>(source.constdata<float>(), target.data<float>(), source.width(), source.height(), borderSizeLeft, borderSizeTop, borderSizeRight, borderSizeBottom, (const float*)color, source.paddingElements(), target.paddingElements());
					return true;
			}
		}
	}

	ocean_assert(false && "Invalid pixel format!");
	return false;
}

bool FrameEnlarger::Comfort::addBorderNearestPixel(const Frame& source, Frame& target, const unsigned int borderSizeLeft, const unsigned int borderSizeTop, const unsigned int borderSizeRight, const unsigned int borderSizeBottom)
{
	ocean_assert(source.isValid());

	if (source.numberPlanes() == 1u)
	{
		constexpr bool targetForceOwner = false;
		constexpr bool targetForceWritable = true;

		if (source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
		{
			if (!target.set(FrameType(source, source.width() + borderSizeLeft + borderSizeRight, source.height() + borderSizeTop + borderSizeBottom), targetForceOwner, targetForceWritable))
			{
				ocean_assert(false && "This should never happen!");
				return false;
			}

			switch (source.channels())
			{
				case 1u:
					FrameEnlarger::addBorderNearestPixel<uint8_t, 1u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), borderSizeLeft, borderSizeTop, borderSizeRight, borderSizeBottom, source.paddingElements(), target.paddingElements());
					return true;

				case 2u:
					FrameEnlarger::addBorderNearestPixel<uint8_t, 2u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), borderSizeLeft, borderSizeTop, borderSizeRight, borderSizeBottom, source.paddingElements(), target.paddingElements());
					return true;

				case 3u:
					FrameEnlarger::addBorderNearestPixel<uint8_t, 3u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), borderSizeLeft, borderSizeTop, borderSizeRight, borderSizeBottom, source.paddingElements(), target.paddingElements());
					return true;

				case 4u:
					FrameEnlarger::addBorderNearestPixel<uint8_t, 4u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), borderSizeLeft, borderSizeTop, borderSizeRight, borderSizeBottom, source.paddingElements(), target.paddingElements());
					return true;
			}
		}

		else if (source.dataType() == FrameType::DT_SIGNED_FLOAT_32)
		{
			if (!target.set(FrameType(source, source.width() + borderSizeLeft + borderSizeRight, source.height() + borderSizeTop + borderSizeBottom), targetForceOwner, targetForceWritable))
			{
				ocean_assert(false && "This should never happen!");
				return false;
			}

			switch (source.channels())
			{
				case 1u:
					FrameEnlarger::addBorderNearestPixel<float, 1u>(source.constdata<float>(), target.data<float>(), source.width(), source.height(), borderSizeLeft, borderSizeTop, borderSizeRight, borderSizeBottom, source.paddingElements(), target.paddingElements());
					return true;

				case 2u:
					FrameEnlarger::addBorderNearestPixel<float, 2u>(source.constdata<float>(), target.data<float>(), source.width(), source.height(), borderSizeLeft, borderSizeTop, borderSizeRight, borderSizeBottom, source.paddingElements(), target.paddingElements());
					return true;

				case 3u:
					FrameEnlarger::addBorderNearestPixel<float, 3u>(source.constdata<float>(), target.data<float>(), source.width(), source.height(), borderSizeLeft, borderSizeTop, borderSizeRight, borderSizeBottom, source.paddingElements(), target.paddingElements());
					return true;

				case 4u:
					FrameEnlarger::addBorderNearestPixel<float, 4u>(source.constdata<float>(), target.data<float>(), source.width(), source.height(), borderSizeLeft, borderSizeTop, borderSizeRight, borderSizeBottom, source.paddingElements(), target.paddingElements());
					return true;
			}
		}
	}

	ocean_assert(false && "Invalid pixel format!");
	return false;
}

bool FrameEnlarger::Comfort::addBorderMirrored(const Frame& source, Frame& target, const unsigned int borderSizeLeft, const unsigned int borderSizeTop, const unsigned int borderSizeRight, const unsigned int borderSizeBottom)
{
	ocean_assert(source.isValid());

	if (borderSizeLeft > source.width() || borderSizeRight > source.width() || borderSizeTop > source.height() || borderSizeBottom > source.height())
	{
		ocean_assert(false && "Invalid border!");
		return false;
	}

	if (source.numberPlanes() == 1u)
	{
		constexpr bool targetForceOwner = false;
		constexpr bool targetForceWritable = true;

		if (source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
		{
			if (!target.set(FrameType(source, source.width() + borderSizeLeft + borderSizeRight, source.height() + borderSizeTop + borderSizeBottom), targetForceOwner, targetForceWritable))
			{
				ocean_assert(false && "This should never happen!");
				return false;
			}

			switch (source.channels())
			{
				case 1u:
					FrameEnlarger::addBorderMirrored<uint8_t, 1u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), borderSizeLeft, borderSizeTop, borderSizeRight, borderSizeBottom, source.paddingElements(), target.paddingElements());
					return true;

				case 2u:
					FrameEnlarger::addBorderMirrored<uint8_t, 2u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), borderSizeLeft, borderSizeTop, borderSizeRight, borderSizeBottom, source.paddingElements(), target.paddingElements());
					return true;

				case 3u:
					FrameEnlarger::addBorderMirrored<uint8_t, 3u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), borderSizeLeft, borderSizeTop, borderSizeRight, borderSizeBottom, source.paddingElements(), target.paddingElements());
					return true;

				case 4u:
					FrameEnlarger::addBorderMirrored<uint8_t, 4u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), borderSizeLeft, borderSizeTop, borderSizeRight, borderSizeBottom, source.paddingElements(), target.paddingElements());
					return true;
			}
		}

		else if (source.dataType() == FrameType::DT_SIGNED_FLOAT_32)
		{
			if (!target.set(FrameType(source, source.width() + borderSizeLeft + borderSizeRight, source.height() + borderSizeTop + borderSizeBottom), targetForceOwner, targetForceWritable))
			{
				ocean_assert(false && "This should never happen!");
				return false;
			}

			switch (source.channels())
			{
				case 1u:
					FrameEnlarger::addBorderMirrored<float, 1u>(source.constdata<float>(), target.data<float>(), source.width(), source.height(), borderSizeLeft, borderSizeTop, borderSizeRight, borderSizeBottom, source.paddingElements(), target.paddingElements());
					return true;

				case 2u:
					FrameEnlarger::addBorderMirrored<float, 2u>(source.constdata<float>(), target.data<float>(), source.width(), source.height(), borderSizeLeft, borderSizeTop, borderSizeRight, borderSizeBottom, source.paddingElements(), target.paddingElements());
					return true;

				case 3u:
					FrameEnlarger::addBorderMirrored<float, 3u>(source.constdata<float>(), target.data<float>(), source.width(), source.height(), borderSizeLeft, borderSizeTop, borderSizeRight, borderSizeBottom, source.paddingElements(), target.paddingElements());
					return true;

				case 4u:
					FrameEnlarger::addBorderMirrored<float, 4u>(source.constdata<float>(), target.data<float>(), source.width(), source.height(), borderSizeLeft, borderSizeTop, borderSizeRight, borderSizeBottom, source.paddingElements(), target.paddingElements());
					return true;
			}
		}
	}

	ocean_assert(false && "Invalid pixel format!");
	return false;
}

bool FrameEnlarger::Comfort::multiplyByTwo(const Frame& source, Frame& target, Worker* worker)
{
	ocean_assert(source.isValid());

	if (source.numberPlanes() == 1u && source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
	{
		if (!target.set(FrameType(source, source.width() * 2u, source.height() * 2u), true, true))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		switch (source.channels())
		{
			case 1u:
				FrameEnlarger::multiplyByTwo<uint8_t, 1u>(source.constdata<uint8_t>(), target.data<uint8_t>(), target.width(), target.height(), source.paddingElements(), target.paddingElements(), worker);
				return true;

			case 2u:
				FrameEnlarger::multiplyByTwo<uint8_t, 2u>(source.constdata<uint8_t>(), target.data<uint8_t>(), target.width(), target.height(), source.paddingElements(), target.paddingElements(), worker);
				return true;

			case 3u:
				FrameEnlarger::multiplyByTwo<uint8_t, 3u>(source.constdata<uint8_t>(), target.data<uint8_t>(), target.width(), target.height(), source.paddingElements(), target.paddingElements(), worker);
				return true;

			case 4u:
				FrameEnlarger::multiplyByTwo<uint8_t, 4u>(source.constdata<uint8_t>(), target.data<uint8_t>(), target.width(), target.height(), source.paddingElements(), target.paddingElements(), worker);
				return true;
		}
	}

	ocean_assert(false && "Invalid pixel format!");
	return false;
}

}

}
