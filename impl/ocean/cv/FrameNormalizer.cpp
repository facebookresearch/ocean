/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/FrameNormalizer.h"

namespace Ocean
{

namespace CV
{

bool FrameNormalizer::normalizeToUint8(const Frame& source, Frame& target, Worker* worker)
{
	ocean_assert(source.isValid());

	if (source.channels() == 1u)
	{
		constexpr bool targetForceOwner = false;
		constexpr bool targetForceWritable = true;

		switch (source.dataType())
		{
			case FrameType::dataType<uint8_t>():
			{
				if (!target.set(FrameType(source, FrameType::FORMAT_Y8), targetForceOwner, targetForceWritable))
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				normalize1ChannelToUint8<uint8_t>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);

				return true;
			}

			case FrameType::dataType<int8_t>():
			{
				if (!target.set(FrameType(source, FrameType::FORMAT_Y8), targetForceOwner, targetForceWritable))
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				normalize1ChannelToUint8<int8_t>(source.constdata<int8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);

				return true;
			}

			case FrameType::dataType<uint16_t>():
			{
				if (!target.set(FrameType(source, FrameType::FORMAT_Y8), targetForceOwner, targetForceWritable))
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				normalize1ChannelToUint8<uint16_t>(source.constdata<uint16_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);

				return true;
			}

			case FrameType::dataType<int16_t>():
			{
				if (!target.set(FrameType(source, FrameType::FORMAT_Y8), targetForceOwner, targetForceWritable))
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				normalize1ChannelToUint8<int16_t>(source.constdata<int16_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);

				return true;
			}

			case FrameType::dataType<uint32_t>():
			{
				if (!target.set(FrameType(source, FrameType::FORMAT_Y8), targetForceOwner, targetForceWritable))
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				normalize1ChannelToUint8<uint32_t>(source.constdata<uint32_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);

				return true;
			}

			case FrameType::dataType<int32_t>():
			{
				if (!target.set(FrameType(source, FrameType::FORMAT_Y8), targetForceOwner, targetForceWritable))
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				normalize1ChannelToUint8<int32_t>(source.constdata<int32_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);

				return true;
			}

			case FrameType::dataType<uint64_t>():
			{
				if (!target.set(FrameType(source, FrameType::FORMAT_Y8), targetForceOwner, targetForceWritable))
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				normalize1ChannelToUint8<uint64_t>(source.constdata<uint64_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);

				return true;
			}

			case FrameType::dataType<int64_t>():
			{
				if (!target.set(FrameType(source, FrameType::FORMAT_Y8), targetForceOwner, targetForceWritable))
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				normalize1ChannelToUint8<int64_t>(source.constdata<int64_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);

				return true;
			}

			case FrameType::dataType<float>():
			{
				if (!target.set(FrameType(source, FrameType::FORMAT_Y8), targetForceOwner, targetForceWritable))
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				normalize1ChannelToUint8<float>(source.constdata<float>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);

				return true;
			}

			case FrameType::dataType<double>():
			{
				if (!target.set(FrameType(source, FrameType::FORMAT_Y8), targetForceOwner, targetForceWritable))
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				normalize1ChannelToUint8<double>(source.constdata<double>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);

				return true;
			}

			default:
				break;
		}
	}

	ocean_assert(false && "Invalid pixel format!");
	return false;
}

bool FrameNormalizer::normalizeToFloat32(const Frame& source, Frame& target, Worker* worker)
{
	return normalizeToFloat32(source, target, /* bias */ nullptr, /* scale */ nullptr, worker);
}

bool FrameNormalizer::normalizeToFloat32(const Frame& source, Frame& target, const float* bias, const float* scale, Worker* worker)
{
	ocean_assert(source.isValid() && source.numberPlanes() == 1u);
	ocean_assert((bias == nullptr && scale == nullptr) || (bias != nullptr && scale != nullptr));

	if (!target.set(FrameType(source, FrameType::genericPixelFormat(FrameType::DT_SIGNED_FLOAT_32, source.channels())), /* targetForceOwner */ false, /* targetForceWritable */ true))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	switch (source.dataType())
	{
		case FrameType::dataType<uint8_t>():
			return normalizeToFloat<uint8_t, float>(source.constdata<uint8_t>(), target.data<float>(), source.width(), source.height(), source.channels(), bias, scale, source.paddingElements(), target.paddingElements(), worker);

		case FrameType::dataType<int8_t>():
			return normalizeToFloat<int8_t, float>(source.constdata<int8_t>(), target.data<float>(), source.width(), source.height(), source.channels(), bias, scale, source.paddingElements(), target.paddingElements(), worker);

		case FrameType::dataType<uint16_t>():
			return normalizeToFloat<uint16_t, float>(source.constdata<uint16_t>(), target.data<float>(), source.width(), source.height(), source.channels(), bias, scale, source.paddingElements(), target.paddingElements(), worker);

		case FrameType::dataType<int16_t>():
			return normalizeToFloat<int16_t, float>(source.constdata<int16_t>(), target.data<float>(), source.width(), source.height(), source.channels(), bias, scale, source.paddingElements(), target.paddingElements(), worker);

		case FrameType::dataType<uint32_t>():
			return normalizeToFloat<uint32_t, float>(source.constdata<uint32_t>(), target.data<float>(), source.width(), source.height(), source.channels(), bias, scale, source.paddingElements(), target.paddingElements(), worker);

		case FrameType::dataType<int32_t>():
			return normalizeToFloat<int32_t, float>(source.constdata<int32_t>(), target.data<float>(), source.width(), source.height(), source.channels(), bias, scale, source.paddingElements(), target.paddingElements(), worker);

		case FrameType::dataType<uint64_t>():
			return normalizeToFloat<uint64_t, float>(source.constdata<uint64_t>(), target.data<float>(), source.width(), source.height(), source.channels(), bias, scale, source.paddingElements(), target.paddingElements(), worker);

		case FrameType::dataType<int64_t>():
			return normalizeToFloat<int64_t, float>(source.constdata<int64_t>(), target.data<float>(), source.width(), source.height(), source.channels(), bias, scale, source.paddingElements(), target.paddingElements(), worker);

		case FrameType::dataType<float>():
			return normalizeToFloat<float, float>(source.constdata<float>(), target.data<float>(), source.width(), source.height(), source.channels(), bias, scale, source.paddingElements(), target.paddingElements(), worker);

		case FrameType::dataType<double>():
			return normalizeToFloat<double, float>(source.constdata<double>(), target.data<float>(), source.width(), source.height(), source.channels(), bias, scale, source.paddingElements(), target.paddingElements(), worker);

		default:
			ocean_assert(false && "Invalid pixel format!");
			break;
	}

	ocean_assert(false && "Should never be here!");
	return false;
}

bool FrameNormalizer::normalizeLogarithmToUint8(const Frame& source, Frame& target, const Scalar scalar, Worker* worker)
{
	ocean_assert(source);
	ocean_assert(scalar > 0 && scalar <= 10);

	if (scalar <= 0 || scalar > 10)
	{
		return false;
	}

	if (source.channels() == 1u)
	{
		constexpr bool targetForceOwner = false;
		constexpr bool targetForceWritable = true;

		switch (source.dataType())
		{
			case FrameType::DT_UNSIGNED_INTEGER_8:
			{
				if (!target.set(FrameType(source, FrameType::FORMAT_Y8), targetForceOwner, targetForceWritable))
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				normalizeLogarithm1ChannelToUint8<uint8_t>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), scalar, source.paddingElements(), target.paddingElements(), worker);

				return true;
			}

			case FrameType::DT_UNSIGNED_INTEGER_16:
			{
				if (!target.set(FrameType(source, FrameType::FORMAT_Y8), targetForceOwner, targetForceWritable))
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				normalizeLogarithm1ChannelToUint8<uint16_t>(source.constdata<uint16_t>(), target.data<uint8_t>(), source.width(), source.height(), scalar, source.paddingElements(), target.paddingElements(), worker);

				return true;
			}

			case FrameType::DT_UNSIGNED_INTEGER_32:
			{
				if (!target.set(FrameType(source, FrameType::FORMAT_Y8), targetForceOwner, targetForceWritable))
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				normalizeLogarithm1ChannelToUint8<uint32_t>(source.constdata<uint32_t>(), target.data<uint8_t>(), source.width(), source.height(), scalar, source.paddingElements(), target.paddingElements(), worker);

				return true;
			}

			case FrameType::DT_UNSIGNED_INTEGER_64:
			{
				if (!target.set(FrameType(source, FrameType::FORMAT_Y8), targetForceOwner, targetForceWritable))
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				normalizeLogarithm1ChannelToUint8<uint64_t>(source.constdata<uint64_t>(), target.data<uint8_t>(), source.width(), source.height(), scalar, source.paddingElements(), target.paddingElements(), worker);

				return true;
			}

			case FrameType::DT_SIGNED_FLOAT_32:
			{
				if (!target.set(FrameType(source, FrameType::FORMAT_Y8), targetForceOwner, targetForceWritable))
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				normalizeLogarithm1ChannelToUint8<float>(source.constdata<float>(), target.data<uint8_t>(), source.width(), source.height(), scalar, source.paddingElements(), target.paddingElements(), worker);

				return true;
			}

			case FrameType::DT_SIGNED_FLOAT_64:
			{
				if (!target.set(FrameType(source, FrameType::FORMAT_Y8), targetForceOwner, targetForceWritable))
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				normalizeLogarithm1ChannelToUint8<double>(source.constdata<double>(), target.data<uint8_t>(), source.width(), source.height(), scalar, source.paddingElements(), target.paddingElements(), worker);

				return true;
			}

			default:
				break;
		}
	}

	ocean_assert(false && "Invalid pixel format!");
	return false;
}

bool FrameNormalizer::normalizeLogarithmToFloat32(const Frame& source, Frame& target, const Scalar scalar, Worker* worker)
{
	ocean_assert(source);
	ocean_assert(scalar > 0 && scalar <= 10);

	if (scalar <= 0 || scalar > 10)
	{
		return false;
	}

	if (source.channels() == 1u)
	{
		constexpr bool targetForceOwner = false;
		constexpr bool targetForceWritable = true;

		switch (source.dataType())
		{
			case FrameType::DT_UNSIGNED_INTEGER_8:
			{
				if (!target.set(FrameType(source, FrameType::FORMAT_F32), targetForceOwner, targetForceWritable))
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				normalizeLogarithm1ChannelToFloat<uint8_t, float>(source.constdata<uint8_t>(), target.data<float>(), source.width(), source.height(), scalar, source.paddingElements(), target.paddingElements(), worker);

				return true;
			}

			case FrameType::DT_UNSIGNED_INTEGER_16:
			{
				if (!target.set(FrameType(source, FrameType::FORMAT_F32), targetForceOwner, targetForceWritable))
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				normalizeLogarithm1ChannelToFloat<uint16_t, float>(source.constdata<uint16_t>(), target.data<float>(), source.width(), source.height(), scalar, source.paddingElements(), target.paddingElements(), worker);

				return true;
			}

			case FrameType::DT_UNSIGNED_INTEGER_32:
			{
				if (!target.set(FrameType(source, FrameType::FORMAT_F32), targetForceOwner, targetForceWritable))
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				normalizeLogarithm1ChannelToFloat<uint32_t, float>(source.constdata<uint32_t>(), target.data<float>(), source.width(), source.height(), scalar, source.paddingElements(), target.paddingElements(), worker);

				return true;
			}

			case FrameType::DT_UNSIGNED_INTEGER_64:
			{
				if (!target.set(FrameType(source, FrameType::FORMAT_F32), targetForceOwner, targetForceWritable))
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				normalizeLogarithm1ChannelToFloat<uint64_t, float>(source.constdata<uint64_t>(), target.data<float>(), source.width(), source.height(), scalar, source.paddingElements(), target.paddingElements(), worker);

				return true;
			}

			case FrameType::DT_SIGNED_FLOAT_32:
			{
				if (!target.set(FrameType(source, FrameType::FORMAT_F32), targetForceOwner, targetForceWritable))
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				normalizeLogarithm1ChannelToFloat<float, float>(source.constdata<float>(), target.data<float>(), source.width(), source.height(), scalar, source.paddingElements(), target.paddingElements(), worker);

				return true;
			}

			case FrameType::DT_SIGNED_FLOAT_64:
			{
				if (!target.set(FrameType(source, FrameType::FORMAT_F32), targetForceOwner, targetForceWritable))
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				normalizeLogarithm1ChannelToFloat<double, float>(source.constdata<double>(), target.data<float>(), source.width(), source.height(), scalar, source.paddingElements(), target.paddingElements(), worker);

				return true;
			}

			default:
				break;
		}
	}

	ocean_assert(false && "Invalid pixel format!");
	return false;
}

bool FrameNormalizer::normalizeToUint8(Frame& frame, Worker* worker)
{
	Frame tmpFrame;
	if (!normalizeToUint8(frame, tmpFrame, worker))
	{
		return false;
	}

	frame = std::move(tmpFrame);
	return true;
}

bool FrameNormalizer::normalizeToFloat32(Frame& frame, Worker* worker)
{
	Frame tmpFrame;
	if (!normalizeToFloat32(frame, tmpFrame, worker))
	{
		return false;
	}

	frame = std::move(tmpFrame);
	return true;
}

bool FrameNormalizer::normalizeLogarithmToUint8(Frame& frame, const Scalar octaves, Worker* worker)
{
	Frame tmpFrame;
	if (!normalizeLogarithmToUint8(frame, tmpFrame, octaves, worker))
	{
		return false;
	}

	frame = std::move(tmpFrame);
	return true;
}

bool FrameNormalizer::normalizeLogarithmToFloat32(Frame& frame, const Scalar octaves, Worker* worker)
{
	Frame tmpFrame;
	if (!normalizeLogarithmToFloat32(frame, tmpFrame, octaves, worker))
	{
		return false;
	}

	frame = std::move(tmpFrame);
	return true;
}

}

}
