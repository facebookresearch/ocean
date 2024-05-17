/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/FrameChannels.h"

namespace Ocean
{

namespace CV
{

bool FrameChannels::Comfort::separateTo1Channel(const Frame& sourceFrame, Frames& targetFrames, const FrameType::PixelFormat targetPixelFormat)
{
	if (!sourceFrame.isValid() || sourceFrame.numberPlanes() != 1u || !sourceFrame.formatIsGeneric(sourceFrame.pixelFormat()))
	{
		ocean_assert(false && "Invalid source frame!");
		return false;
	}

	const FrameType::DataType pixelFormatDataType = sourceFrame.dataType();
	const unsigned int channels = sourceFrame.channels();

	const FrameType::PixelFormat genericPixelFormat = FrameType::genericPixelFormat(pixelFormatDataType, 1u);

	if (targetPixelFormat != FrameType::FORMAT_UNDEFINED && !FrameType::arePixelFormatsCompatible(targetPixelFormat, genericPixelFormat))
	{
		ocean_assert(false && "Invalid explicit pixel format!");
		return false;
	}

	const FrameType individualFrameType(sourceFrame.width(), sourceFrame.height(), targetPixelFormat == FrameType::FORMAT_UNDEFINED ? genericPixelFormat : targetPixelFormat, sourceFrame.pixelOrigin());
	ocean_assert(individualFrameType.isValid());

	if (targetFrames.size() > channels)
	{
		return false;
	}

	targetFrames.reserve(channels);
	std::vector<void*> targetFramePointers(channels);

	Indices32 targetFramesPaddingElements(channels);

	for (unsigned int n = 0u; n < channels; ++n)
	{
		if (n < targetFrames.size())
		{
			if (!FrameType::arePixelFormatsCompatible(targetFrames[n].pixelFormat(), individualFrameType.pixelFormat()) || targetFrames[n].pixelOrigin() != individualFrameType.pixelOrigin())
			{
				if (!targetFrames[n].set(individualFrameType, false /*forceOwner*/, true /*forceWritable*/))
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}
			}
		}
		else
		{
			ocean_assert(targetFrames.size() == n);
			targetFrames.emplace_back(individualFrameType);
		}

		targetFramePointers[n] = targetFrames[n].data<uint8_t>();

		targetFramesPaddingElements[n] = targetFrames[n].paddingElements();
	}

	ocean_assert(targetFrames.size() == channels);

	switch (pixelFormatDataType)
	{
		case FrameType::DT_UNSIGNED_INTEGER_8:
		case FrameType::DT_SIGNED_INTEGER_8:
			FrameChannels::separateTo1Channel<uint8_t, uint8_t, CHANNELS_NOT_KNOWN_AT_COMPILE_TIME>(sourceFrame.constdata<uint8_t>(), (uint8_t**)(targetFramePointers.data()), sourceFrame.width(), sourceFrame.height(), channels, sourceFrame.paddingElements(), targetFramesPaddingElements.data());
			return true;

		case FrameType::DT_UNSIGNED_INTEGER_16:
		case FrameType::DT_SIGNED_INTEGER_16:
		case FrameType::DT_SIGNED_FLOAT_16:
			FrameChannels::separateTo1Channel<uint16_t, uint16_t, CHANNELS_NOT_KNOWN_AT_COMPILE_TIME>(sourceFrame.constdata<uint16_t>(), (uint16_t**)(targetFramePointers.data()), sourceFrame.width(), sourceFrame.height(), channels, sourceFrame.paddingElements(), targetFramesPaddingElements.data());
			return true;

		case FrameType::DT_UNSIGNED_INTEGER_32:
		case FrameType::DT_SIGNED_INTEGER_32:
		case FrameType::DT_SIGNED_FLOAT_32:
			FrameChannels::separateTo1Channel<uint32_t, uint32_t, CHANNELS_NOT_KNOWN_AT_COMPILE_TIME>(sourceFrame.constdata<uint32_t>(), (uint32_t**)(targetFramePointers.data()), sourceFrame.width(), sourceFrame.height(), channels, sourceFrame.paddingElements(), targetFramesPaddingElements.data());
			return true;

		case FrameType::DT_UNSIGNED_INTEGER_64:
		case FrameType::DT_SIGNED_INTEGER_64:
		case FrameType::DT_SIGNED_FLOAT_64:
			FrameChannels::separateTo1Channel<uint64_t, uint64_t, CHANNELS_NOT_KNOWN_AT_COMPILE_TIME>(sourceFrame.constdata<uint64_t>(), (uint64_t**)(targetFramePointers.data()), sourceFrame.width(), sourceFrame.height(), channels, sourceFrame.paddingElements(), targetFramesPaddingElements.data());
			return true;

		default:
			break;
	}

	ocean_assert(false && "Not supported data type!");
	return false;
}

bool FrameChannels::Comfort::separateTo1Channel(const Frame& sourceFrame, const std::initializer_list<Frame*>& targetFrames, const FrameType::PixelFormat targetPixelFormat)
{
	if (!sourceFrame.isValid() || sourceFrame.numberPlanes() != 1u || !sourceFrame.formatIsGeneric(sourceFrame.pixelFormat()))
	{
		ocean_assert(false && "Invalid source frame!");
		return false;
	}

	const FrameType::DataType pixelFormatDataType = sourceFrame.dataType();
	const unsigned int channels = sourceFrame.channels();

	const FrameType::PixelFormat genericPixelFormat = FrameType::genericPixelFormat(pixelFormatDataType, 1u);

	if (targetPixelFormat != FrameType::FORMAT_UNDEFINED && !FrameType::arePixelFormatsCompatible(targetPixelFormat, genericPixelFormat))
	{
		ocean_assert(false && "Invalid explicit pixel format!");
		return false;
	}

	const FrameType individualFrameType(sourceFrame.width(), sourceFrame.height(), targetPixelFormat == FrameType::FORMAT_UNDEFINED ? genericPixelFormat : targetPixelFormat, sourceFrame.pixelOrigin());
	ocean_assert(individualFrameType.isValid());

	if (targetFrames.size() != channels)
	{
		ocean_assert(false && "Invalid target frames!");
		return false;
	}

	std::vector<void*> targetFramePointers;
	Indices32 targetFramesPaddingElements;

	targetFramePointers.reserve(channels);
	targetFramesPaddingElements.reserve(channels);

	for (Frame* targetFrame : targetFrames)
	{
		if (!FrameType::arePixelFormatsCompatible(targetFrame->pixelFormat(), individualFrameType.pixelFormat()) || targetFrame->pixelOrigin() != individualFrameType.pixelOrigin())
		{
			targetFrame->set(individualFrameType, false /*forceOwner*/, true /*forceWritable*/);
		}

		targetFramePointers.emplace_back(targetFrame->data<uint8_t>());

		targetFramesPaddingElements.emplace_back(targetFrame->paddingElements());
	}

	switch (pixelFormatDataType)
	{
		case FrameType::DT_UNSIGNED_INTEGER_8:
		case FrameType::DT_SIGNED_INTEGER_8:
			FrameChannels::separateTo1Channel<uint8_t, uint8_t, CHANNELS_NOT_KNOWN_AT_COMPILE_TIME>(sourceFrame.constdata<uint8_t>(), (uint8_t**)(targetFramePointers.data()), sourceFrame.width(), sourceFrame.height(), channels, sourceFrame.paddingElements(), targetFramesPaddingElements.data());
			return true;

		case FrameType::DT_UNSIGNED_INTEGER_16:
		case FrameType::DT_SIGNED_INTEGER_16:
		case FrameType::DT_SIGNED_FLOAT_16:
			FrameChannels::separateTo1Channel<uint16_t, uint16_t, CHANNELS_NOT_KNOWN_AT_COMPILE_TIME>(sourceFrame.constdata<uint16_t>(), (uint16_t**)(targetFramePointers.data()), sourceFrame.width(), sourceFrame.height(), channels, sourceFrame.paddingElements(), targetFramesPaddingElements.data());
			return true;

		case FrameType::DT_UNSIGNED_INTEGER_32:
		case FrameType::DT_SIGNED_INTEGER_32:
		case FrameType::DT_SIGNED_FLOAT_32:
			FrameChannels::separateTo1Channel<uint32_t, uint32_t, CHANNELS_NOT_KNOWN_AT_COMPILE_TIME>(sourceFrame.constdata<uint32_t>(), (uint32_t**)(targetFramePointers.data()), sourceFrame.width(), sourceFrame.height(), channels, sourceFrame.paddingElements(), targetFramesPaddingElements.data());
			return true;

		case FrameType::DT_UNSIGNED_INTEGER_64:
		case FrameType::DT_SIGNED_INTEGER_64:
		case FrameType::DT_SIGNED_FLOAT_64:
			FrameChannels::separateTo1Channel<uint64_t, uint64_t, CHANNELS_NOT_KNOWN_AT_COMPILE_TIME>(sourceFrame.constdata<uint64_t>(), (uint64_t**)(targetFramePointers.data()), sourceFrame.width(), sourceFrame.height(), channels, sourceFrame.paddingElements(), targetFramesPaddingElements.data());
			return true;

		default:
			break;
	}

	ocean_assert(false && "Not supported data type!");
	return false;
}

bool FrameChannels::Comfort::zipChannels(const std::initializer_list<Frame>& sourceFrames, Frame& targetFrame, const FrameType::PixelFormat targetPixelFormat)
{
	if (sourceFrames.size() == 0)
	{
		ocean_assert(false && "Invalid source frames!");
		return false;
	}

	FrameType sourceFrameType = sourceFrames.begin()->frameType();

	if (!sourceFrameType.isValid() || sourceFrameType.numberPlanes() != 1u)
	{
		ocean_assert(false && "Invalid data type!");
		return false;
	}

	std::vector<const void*> sourcePointers;
	Indices32 sourcePaddingElements;

	sourcePointers.reserve(sourceFrames.size());
	sourcePaddingElements.reserve(sourceFrames.size());

	for (const Frame& sourceFrame : sourceFrames)
	{
		if (!sourceFrame.isFrameTypeCompatible(sourceFrameType, false))
		{
			ocean_assert(false && "Source frame does not match!");
			return false;
		}

		sourcePointers.emplace_back(sourceFrame.constdata<void>());
		sourcePaddingElements.emplace_back(sourceFrame.paddingElements());
	}

	const FrameType::DataType dataType = sourceFrameType.dataType();
	const unsigned int channels = (unsigned int)(sourceFrames.size());
	ocean_assert(dataType != FrameType::DT_UNDEFINED && channels != 0u);

	FrameType::PixelFormat pixelFormatToUse = FrameType::genericPixelFormat(dataType, channels);

	if (targetFrame.isValid() && FrameType::arePixelFormatsCompatible(targetFrame.pixelFormat(), pixelFormatToUse))
	{
		pixelFormatToUse = targetFrame.pixelFormat();
	}

	if (targetPixelFormat != FrameType::FORMAT_UNDEFINED)
	{
		if (!FrameType::arePixelFormatsCompatible(targetPixelFormat, pixelFormatToUse))
		{
			ocean_assert(false && "Target pixel format does not match!");
			return false;
		}

		pixelFormatToUse = targetPixelFormat;
	}

	if (!targetFrame.set(FrameType(sourceFrameType, pixelFormatToUse), false /*forceOwner*/, true /*forceWritable*/))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	switch (dataType)
	{
		case FrameType::DT_UNSIGNED_INTEGER_8:
		case FrameType::DT_SIGNED_INTEGER_8:
			FrameChannels::zipChannels<uint8_t, uint8_t, CHANNELS_NOT_KNOWN_AT_COMPILE_TIME>((const uint8_t**)(sourcePointers.data()), targetFrame.data<uint8_t>(), targetFrame.width(), targetFrame.height(), channels, sourcePaddingElements.data(), targetFrame.paddingElements());
			return true;

		case FrameType::DT_UNSIGNED_INTEGER_16:
		case FrameType::DT_SIGNED_INTEGER_16:
		case FrameType::DT_SIGNED_FLOAT_16:
			FrameChannels::zipChannels<uint16_t, uint16_t, CHANNELS_NOT_KNOWN_AT_COMPILE_TIME>((const uint16_t**)(sourcePointers.data()), targetFrame.data<uint16_t>(), targetFrame.width(), targetFrame.height(), channels, sourcePaddingElements.data(), targetFrame.paddingElements());
			return true;

		case FrameType::DT_UNSIGNED_INTEGER_32:
		case FrameType::DT_SIGNED_INTEGER_32:
		case FrameType::DT_SIGNED_FLOAT_32:
			FrameChannels::zipChannels<uint32_t, uint32_t, CHANNELS_NOT_KNOWN_AT_COMPILE_TIME>((const uint32_t**)(sourcePointers.data()), targetFrame.data<uint32_t>(), targetFrame.width(), targetFrame.height(), channels, sourcePaddingElements.data(), targetFrame.paddingElements());
			return true;

		case FrameType::DT_UNSIGNED_INTEGER_64:
		case FrameType::DT_SIGNED_INTEGER_64:
		case FrameType::DT_SIGNED_FLOAT_64:
			FrameChannels::zipChannels<uint64_t, uint64_t, CHANNELS_NOT_KNOWN_AT_COMPILE_TIME>((const uint64_t**)(sourcePointers.data()), targetFrame.data<uint64_t>(), targetFrame.width(), targetFrame.height(), channels, sourcePaddingElements.data(), targetFrame.paddingElements());
			return true;

		default:
			break;
	}

	ocean_assert(false && "Pixel format not supported!");
	return false;
}

bool FrameChannels::Comfort::zipChannels(const Frames& sourceFrames, Frame& targetFrame, const FrameType::PixelFormat targetPixelFormat)
{
	if (sourceFrames.empty())
	{
		ocean_assert(false && "Invalid source frames!");
		return false;
	}

	FrameType sourceFrameType = sourceFrames.front().frameType();

	if (!sourceFrameType.isValid() || sourceFrameType.numberPlanes() != 1u)
	{
		ocean_assert(false && "Invalid data type!");
		return false;
	}

	std::vector<const void*> sourcePointers;
	Indices32 sourcePaddingElements;

	sourcePointers.reserve(sourceFrames.size());
	sourcePaddingElements.reserve(sourceFrames.size());

	for (const Frame& sourceFrame : sourceFrames)
	{
		if (!sourceFrame.isFrameTypeCompatible(sourceFrameType, false))
		{
			ocean_assert(false && "Source frame does not match!");
			return false;
		}

		sourcePointers.emplace_back(sourceFrame.constdata<void>());
		sourcePaddingElements.emplace_back(sourceFrame.paddingElements());
	}

	const FrameType::DataType dataType = sourceFrameType.dataType();
	const unsigned int channels = (unsigned int)(sourceFrames.size());
	ocean_assert(dataType != FrameType::DT_UNDEFINED && channels != 0u);

	FrameType::PixelFormat pixelFormatToUse = FrameType::genericPixelFormat(dataType, channels);

	if (targetFrame.isValid() && FrameType::arePixelFormatsCompatible(targetFrame.pixelFormat(), pixelFormatToUse))
	{
		pixelFormatToUse = targetFrame.pixelFormat();
	}

	if (targetPixelFormat != FrameType::FORMAT_UNDEFINED)
	{
		if (!FrameType::arePixelFormatsCompatible(targetPixelFormat, pixelFormatToUse))
		{
			ocean_assert(false && "Target pixel format does not match!");
			return false;
		}

		pixelFormatToUse = targetPixelFormat;
	}

	if (!targetFrame.set(FrameType(sourceFrameType, pixelFormatToUse), false /*forceOwner*/, true /*forceWritable*/))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	switch (dataType)
	{
		case FrameType::DT_UNSIGNED_INTEGER_8:
		case FrameType::DT_SIGNED_INTEGER_8:
			FrameChannels::zipChannels<uint8_t>((const uint8_t**)(sourcePointers.data()), targetFrame.data<uint8_t>(), targetFrame.width(), targetFrame.height(), channels, sourcePaddingElements.data(), targetFrame.paddingElements());
			return true;

		case FrameType::DT_UNSIGNED_INTEGER_16:
		case FrameType::DT_SIGNED_INTEGER_16:
		case FrameType::DT_SIGNED_FLOAT_16:
			FrameChannels::zipChannels<uint16_t>((const uint16_t**)(sourcePointers.data()), targetFrame.data<uint16_t>(), targetFrame.width(), targetFrame.height(), channels, sourcePaddingElements.data(), targetFrame.paddingElements());
			return true;

		case FrameType::DT_UNSIGNED_INTEGER_32:
		case FrameType::DT_SIGNED_INTEGER_32:
		case FrameType::DT_SIGNED_FLOAT_32:
			FrameChannels::zipChannels<uint32_t>((const uint32_t**)(sourcePointers.data()), targetFrame.data<uint32_t>(), targetFrame.width(), targetFrame.height(), channels, sourcePaddingElements.data(), targetFrame.paddingElements());
			return true;

		case FrameType::DT_UNSIGNED_INTEGER_64:
		case FrameType::DT_SIGNED_INTEGER_64:
		case FrameType::DT_SIGNED_FLOAT_64:
			FrameChannels::zipChannels<uint64_t>((const uint64_t**)(sourcePointers.data()), targetFrame.data<uint64_t>(), targetFrame.width(), targetFrame.height(), channels, sourcePaddingElements.data(), targetFrame.paddingElements());
			return true;

		default:
			break;
	}

	ocean_assert(false && "Pixel format not supported!");
	return false;
}

bool FrameChannels::Comfort::premultipliedAlphaToStraightAlpha(Frame& frame, Worker* worker)
{
	ocean_assert(frame.isValid());

	switch (frame.pixelFormat())
	{
		case FrameType::FORMAT_ABGR32:
		case FrameType::FORMAT_ARGB32:
		{
			FrameChannels::premultipliedAlphaToStraightAlpha8BitPerChannel<4u, 0u>(frame.data<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), worker);
			return true;
		}

		case FrameType::FORMAT_RGBA32:
		case FrameType::FORMAT_BGRA32:
		case FrameType::FORMAT_YUVA32:
		{
			FrameChannels::premultipliedAlphaToStraightAlpha8BitPerChannel<4u, 3u>(frame.data<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), worker);
			return true;
		}

		case FrameType::FORMAT_YA16:
		{
			FrameChannels::premultipliedAlphaToStraightAlpha8BitPerChannel<2u, 1u>(frame.data<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), worker);
			return true;
		}

		default:
			break;
	}

	ocean_assert(false && "Invalid pixel format!");
	return false;
}

bool FrameChannels::Comfort::premultipliedAlphaToStraightAlpha(const Frame& source, Frame& target, Worker* worker)
{
	ocean_assert(source.isValid());

	switch (source.pixelFormat())
	{
		case FrameType::FORMAT_ABGR32:
		case FrameType::FORMAT_ARGB32:
		{
			if (!target.set(source.frameType(), false, true))
			{
				ocean_assert(false && "This should never happen!");
				return false;
			}

			FrameChannels::premultipliedAlphaToStraightAlpha8BitPerChannel<4u, 0u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
			return true;
		}

		case FrameType::FORMAT_RGBA32:
		case FrameType::FORMAT_BGRA32:
		case FrameType::FORMAT_YUVA32:
		{
			if (!target.set(source.frameType(), false, true))
			{
				ocean_assert(false && "This should never happen!");
				return false;
			}

			FrameChannels::premultipliedAlphaToStraightAlpha8BitPerChannel<4u, 3u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
			return true;
		}

		case FrameType::FORMAT_YA16:
		{
			if (!target.set(source.frameType(), false, true))
			{
				ocean_assert(false && "This should never happen!");
				return false;
			}

			FrameChannels::premultipliedAlphaToStraightAlpha8BitPerChannel<2u, 1u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
			return true;
		}

		default:
			break;
	}

	ocean_assert(false && "Invalid pixel format!");
	return false;
}

bool FrameChannels::Comfort::straightAlphaToPremultipliedAlpha(Frame& frame, Worker* worker)
{
	ocean_assert(frame.isValid());

	switch (frame.pixelFormat())
	{
		case FrameType::FORMAT_ABGR32:
		case FrameType::FORMAT_ARGB32:
		{
			FrameChannels::straightAlphaToPremultipliedAlpha8BitPerChannel<4u, 0u>(frame.data<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), worker);
			return true;
		}

		case FrameType::FORMAT_RGBA32:
		case FrameType::FORMAT_BGRA32:
		case FrameType::FORMAT_YUVA32:
		{
			FrameChannels::straightAlphaToPremultipliedAlpha8BitPerChannel<4u, 3u>(frame.data<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), worker);
			return true;
		}

		case FrameType::FORMAT_YA16:
		{
			FrameChannels::straightAlphaToPremultipliedAlpha8BitPerChannel<2u, 1u>(frame.data<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), worker);
			return true;
		}

		default:
			break;
	}

	ocean_assert(false && "Invalid pixel format!");
	return false;
}

bool FrameChannels::Comfort::straightAlphaToPremultipliedAlpha(const Frame& source, Frame& target, Worker* worker)
{
	ocean_assert(source.isValid());

	switch (source.pixelFormat())
	{
		case FrameType::FORMAT_ABGR32:
		case FrameType::FORMAT_ARGB32:
		{
			if (!target.set(source.frameType(), false, true))
			{
				ocean_assert(false && "This should never happen!");
				return false;
			}

			FrameChannels::straightAlphaToPremultipliedAlpha8BitPerChannel<4u, 0u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
			return true;
		}

		case FrameType::FORMAT_RGBA32:
		case FrameType::FORMAT_BGRA32:
		case FrameType::FORMAT_YUVA32:
		{
			if (!target.set(source.frameType(), false, true))
			{
				ocean_assert(false && "This should never happen!");
				return false;
			}

			FrameChannels::straightAlphaToPremultipliedAlpha8BitPerChannel<4u, 3u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
			return true;
		}

		case FrameType::FORMAT_YA16:
		{
			if (!target.set(source.frameType(), false, true))
			{
				ocean_assert(false && "This should never happen!");
				return false;
			}

			FrameChannels::straightAlphaToPremultipliedAlpha8BitPerChannel<2u, 1u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
			return true;
		}

		default:
			break;
	}

	ocean_assert(false && "Invalid pixel format!");
	return false;
}

void FrameChannels::convertRow3ChannelsTo3Channels8BitPerChannel6BitPrecision(const uint8_t* source, uint8_t* target, const size_t size, const void* parameters)
{
	ocean_assert(source != nullptr && target != nullptr && size >= 1);

	ocean_assert(parameters != nullptr);
	const int* factors_64_and_bias = reinterpret_cast<const int*>(parameters);
	ocean_assert(factors_64_and_bias != nullptr);

	// layout of parameters f00, f10, f20, f01, f02, ..., f22, b0, b1, b2
	// with transformation:
	// t0 = clamp(0, f00 * (s0 - b0) + f01 * (s1 - b1) + f02 * (s2 - b2), 255)
	// t1 = clamp(0, f10 * (s0 - b0) + f11 * (s1 - b1) + f12 * (s2 - b2), 255)
	// t2 = clamp(0, f20 * (s0 - b0) + f21 * (s1 - b1) + f22 * (s2 - b2), 255)

	const int factorChannel00_64 = factors_64_and_bias[0];
	const int factorChannel10_64 = factors_64_and_bias[1];
	const int factorChannel20_64 = factors_64_and_bias[2];

	const int factorChannel01_64 = factors_64_and_bias[3];
	const int factorChannel11_64 = factors_64_and_bias[4];
	const int factorChannel21_64 = factors_64_and_bias[5];

	const int factorChannel02_64 = factors_64_and_bias[6];
	const int factorChannel12_64 = factors_64_and_bias[7];
	const int factorChannel22_64 = factors_64_and_bias[8];

	ocean_assert(std::abs(factorChannel00_64 + factorChannel01_64 + factorChannel02_64) < 64 * 4);
	ocean_assert(std::abs(factorChannel10_64 + factorChannel11_64 + factorChannel12_64) < 64 * 4);
	ocean_assert(std::abs(factorChannel20_64 + factorChannel21_64 + factorChannel22_64) < 64 * 4);

	const int bias0 = factors_64_and_bias[9];
	const int bias1 = factors_64_and_bias[10];
	const int bias2 = factors_64_and_bias[11];

	ocean_assert(bias0 >= 0 && bias0 <= 128);
	ocean_assert(bias1 >= 0 && bias1 <= 128);
	ocean_assert(bias2 >= 0 && bias2 <= 128);

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

	// **TODO** add SSE-based implementation

#elif defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	constexpr size_t blockSize = 16;
	const size_t blocks = size / blockSize;

	if (blocks >= 1)
	{
		const int16x8_t factorChannel00_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel00_64));
		const int16x8_t factorChannel10_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel10_64));
		const int16x8_t factorChannel20_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel20_64));

		const int16x8_t factorChannel01_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel01_64));
		const int16x8_t factorChannel11_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel11_64));
		const int16x8_t factorChannel21_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel21_64));

		const int16x8_t factorChannel02_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel02_64));
		const int16x8_t factorChannel12_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel12_64));
		const int16x8_t factorChannel22_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel22_64));

		const uint8x8_t bias0_u_8x8 = vdup_n_u8((uint8_t)(bias0));
		const uint8x8_t bias1_u_8x8 = vdup_n_u8((uint8_t)(bias1));
		const uint8x8_t bias2_u_8x8 = vdup_n_u8((uint8_t)(bias2));

		for (size_t n = 0; n < blocks; ++n)
		{
			if (blockSize == 8)
			{
				convert3ChannelsTo3Channels8Pixels8BitPerChannel6BitPrecisionNEON(source, target, factorChannel00_64_s_16x8, factorChannel10_64_s_16x8, factorChannel20_64_s_16x8, factorChannel01_64_s_16x8, factorChannel11_64_s_16x8, factorChannel21_64_s_16x8, factorChannel02_64_s_16x8, factorChannel12_64_s_16x8, factorChannel22_64_s_16x8, bias0_u_8x8, bias1_u_8x8, bias2_u_8x8);
			}
			else
			{
				static_assert(blockSize == 8 || blockSize == 16, "Invalid block size!");
				ocean_assert(blockSize == 16);

				convert3ChannelsTo3Channels16Pixels8BitPerChannel6BitPrecisionNEON(source, target, factorChannel00_64_s_16x8, factorChannel10_64_s_16x8, factorChannel20_64_s_16x8, factorChannel01_64_s_16x8, factorChannel11_64_s_16x8, factorChannel21_64_s_16x8, factorChannel02_64_s_16x8, factorChannel12_64_s_16x8, factorChannel22_64_s_16x8, bias0_u_8x8, bias1_u_8x8, bias2_u_8x8);
			}

			source += blockSize * size_t(3);
			target += blockSize * size_t(3);
		}

		const size_t remainingPixels = size % blockSize;

		if (remainingPixels)
		{
			// we need to apply another iteration with (back shifted) pointers

			ocean_assert(remainingPixels < blockSize);

			const size_t offset = blockSize - remainingPixels;

			source -= offset * size_t(3);
			target -= offset * size_t(3);

			if (blockSize == 8)
			{
				convert3ChannelsTo3Channels8Pixels8BitPerChannel6BitPrecisionNEON(source, target, factorChannel00_64_s_16x8, factorChannel10_64_s_16x8, factorChannel20_64_s_16x8, factorChannel01_64_s_16x8, factorChannel11_64_s_16x8, factorChannel21_64_s_16x8, factorChannel02_64_s_16x8, factorChannel12_64_s_16x8, factorChannel22_64_s_16x8, bias0_u_8x8, bias1_u_8x8, bias2_u_8x8);
			}
			else
			{
				static_assert(blockSize == 8 || blockSize == 16, "Invalid block size!");
				ocean_assert(blockSize == 16);

				convert3ChannelsTo3Channels16Pixels8BitPerChannel6BitPrecisionNEON(source, target, factorChannel00_64_s_16x8, factorChannel10_64_s_16x8, factorChannel20_64_s_16x8, factorChannel01_64_s_16x8, factorChannel11_64_s_16x8, factorChannel21_64_s_16x8, factorChannel02_64_s_16x8, factorChannel12_64_s_16x8, factorChannel22_64_s_16x8, bias0_u_8x8, bias1_u_8x8, bias2_u_8x8);
			}
		}

		return;
	}

#endif

	const uint8_t* const targetEnd = target + size * size_t(3);

	while (target != targetEnd)
	{
		ocean_assert(target < targetEnd);

		const int16_t source0 = int16_t(source[0] - int16_t(bias0));
		const int16_t source1 = int16_t(source[1] - int16_t(bias1));
		const int16_t source2 = int16_t(source[2] - int16_t(bias2));

		target[0] = (uint8_t)(minmax<int16_t>(0, (source0 * int16_t(factorChannel00_64) + source1 * int16_t(factorChannel01_64) + source2 * int16_t(factorChannel02_64)) / 64, 255));
		target[1] = (uint8_t)(minmax<int16_t>(0, (source0 * int16_t(factorChannel10_64) + source1 * int16_t(factorChannel11_64) + source2 * int16_t(factorChannel12_64)) / 64, 255));
		target[2] = (uint8_t)(minmax<int16_t>(0, (source0 * int16_t(factorChannel20_64) + source1 * int16_t(factorChannel21_64) + source2 * int16_t(factorChannel22_64)) / 64, 255));

		source += 3;
		target += 3;
	}
}

void FrameChannels::convertRow3ChannelsTo3Channels8BitPerChannel7BitPrecision(const uint8_t* source, uint8_t* target, const size_t size, const void* parameters)
{
	ocean_assert(source != nullptr && target != nullptr && size >= 1);

	ocean_assert(parameters != nullptr);
	const int* factors_128_and_bias = reinterpret_cast<const int*>(parameters);
	ocean_assert(factors_128_and_bias != nullptr);

	// layout of parameters f00, f10, f20, f01, f02, ..., f22, b0, b1, b2
	// with transformation:
	// t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
	// t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
	// t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2

	const int factorChannel00_128 = factors_128_and_bias[0];
	const int factorChannel10_128 = factors_128_and_bias[1];
	const int factorChannel20_128 = factors_128_and_bias[2];

	const int factorChannel01_128 = factors_128_and_bias[3];
	const int factorChannel11_128 = factors_128_and_bias[4];
	const int factorChannel21_128 = factors_128_and_bias[5];

	const int factorChannel02_128 = factors_128_and_bias[6];
	const int factorChannel12_128 = factors_128_and_bias[7];
	const int factorChannel22_128 = factors_128_and_bias[8];

	ocean_assert(std::abs(factorChannel00_128 + factorChannel01_128 + factorChannel02_128) < 128);
	ocean_assert(std::abs(factorChannel10_128 + factorChannel11_128 + factorChannel12_128) < 128);
	ocean_assert(std::abs(factorChannel20_128 + factorChannel21_128 + factorChannel22_128) < 128);

	const int bias0 = factors_128_and_bias[9];
	const int bias1 = factors_128_and_bias[10];
	const int bias2 = factors_128_and_bias[11];

	ocean_assert(std::abs(bias0) <= 128 && std::abs(bias1) <= 128 && std::abs(bias2) <= 128);

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

	constexpr size_t blockSize = 16;
	const size_t blocks = size / blockSize;

	if (blocks >= 1)
	{
		const __m128i factorChannel00_128_s_8x8 = _mm_set1_epi16(int16_t(factorChannel00_128));
		const __m128i factorChannel10_128_s_8x8 = _mm_set1_epi16(int16_t(factorChannel10_128));
		const __m128i factorChannel20_128_s_8x8 = _mm_set1_epi16(int16_t(factorChannel20_128));

		const __m128i factorChannel01_128_s_8x8 = _mm_set1_epi16(int16_t(factorChannel01_128));
		const __m128i factorChannel11_128_s_8x8 = _mm_set1_epi16(int16_t(factorChannel11_128));
		const __m128i factorChannel21_128_s_8x8 = _mm_set1_epi16(int16_t(factorChannel21_128));

		const __m128i factorChannel02_128_s_8x8 = _mm_set1_epi16(int16_t(factorChannel02_128));
		const __m128i factorChannel12_128_s_8x8 = _mm_set1_epi16(int16_t(factorChannel12_128));
		const __m128i factorChannel22_128_s_8x8 = _mm_set1_epi16(int16_t(factorChannel22_128));

		const __m128i bias0_s_8x8 = _mm_set1_epi16(int16_t(bias0));
		const __m128i bias1_s_8x8 = _mm_set1_epi16(int16_t(bias1));
		const __m128i bias2_s_8x8 = _mm_set1_epi16(int16_t(bias2));

		for (size_t n = 0; n < blocks; ++n)
		{
			convert3ChannelsTo3Channels16Pixels8BitPerChannel7BitPrecisionSSE(source, target, factorChannel00_128_s_8x8, factorChannel10_128_s_8x8, factorChannel20_128_s_8x8, factorChannel01_128_s_8x8, factorChannel11_128_s_8x8, factorChannel21_128_s_8x8, factorChannel02_128_s_8x8, factorChannel12_128_s_8x8, factorChannel22_128_s_8x8, bias0_s_8x8, bias1_s_8x8, bias2_s_8x8);

			source += blockSize * size_t(3);
			target += blockSize * size_t(3);
		}

		const size_t remainingPixels = size % blockSize;

		if (remainingPixels)
		{
			// we need to apply another iteration with (back shifted) pointers

			ocean_assert(remainingPixels < blockSize);

			const size_t offset = blockSize - remainingPixels;

			source -= offset * size_t(3);
			target -= offset * size_t(3);

			convert3ChannelsTo3Channels16Pixels8BitPerChannel7BitPrecisionSSE(source, target, factorChannel00_128_s_8x8, factorChannel10_128_s_8x8, factorChannel20_128_s_8x8, factorChannel01_128_s_8x8, factorChannel11_128_s_8x8, factorChannel21_128_s_8x8, factorChannel02_128_s_8x8, factorChannel12_128_s_8x8, factorChannel22_128_s_8x8, bias0_s_8x8, bias1_s_8x8, bias2_s_8x8);
		}

		return;
	}

#elif defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	constexpr size_t blockSize = 16;
	const size_t blocks = size / blockSize;

	if (blocks >= 1)
	{
		const int16x8_t factorChannel00_128_s_16x8 = vdupq_n_s16(int16_t(factorChannel00_128));
		const int16x8_t factorChannel10_128_s_16x8 = vdupq_n_s16(int16_t(factorChannel10_128));
		const int16x8_t factorChannel20_128_s_16x8 = vdupq_n_s16(int16_t(factorChannel20_128));

		const int16x8_t factorChannel01_128_s_16x8 = vdupq_n_s16(int16_t(factorChannel01_128));
		const int16x8_t factorChannel11_128_s_16x8 = vdupq_n_s16(int16_t(factorChannel11_128));
		const int16x8_t factorChannel21_128_s_16x8 = vdupq_n_s16(int16_t(factorChannel21_128));

		const int16x8_t factorChannel02_128_s_16x8 = vdupq_n_s16(int16_t(factorChannel02_128));
		const int16x8_t factorChannel12_128_s_16x8 = vdupq_n_s16(int16_t(factorChannel12_128));
		const int16x8_t factorChannel22_128_s_16x8 = vdupq_n_s16(int16_t(factorChannel22_128));

		const int16x8_t bias0_128_s_16x8 = vdupq_n_s16(int16_t(bias0 * 128));
		const int16x8_t bias1_128_s_16x8 = vdupq_n_s16(int16_t(bias1 * 128));
		const int16x8_t bias2_128_s_16x8 = vdupq_n_s16(int16_t(bias2 * 128));

		for (size_t n = 0; n < blocks; ++n)
		{
			if (blockSize == 8)
			{
				convert3ChannelsTo3Channels8Pixels8BitPerChannel7BitPrecisionNEON(source, target, factorChannel00_128_s_16x8, factorChannel10_128_s_16x8, factorChannel20_128_s_16x8, factorChannel01_128_s_16x8, factorChannel11_128_s_16x8, factorChannel21_128_s_16x8, factorChannel02_128_s_16x8, factorChannel12_128_s_16x8, factorChannel22_128_s_16x8, bias0_128_s_16x8, bias1_128_s_16x8, bias2_128_s_16x8);
			}
			else
			{
				static_assert(blockSize == 8 || blockSize == 16, "Invalid block size!");
				ocean_assert(blockSize == 16);

				convert3ChannelsTo3Channels16Pixels8BitPerChannel7BitPrecisionNEON(source, target, factorChannel00_128_s_16x8, factorChannel10_128_s_16x8, factorChannel20_128_s_16x8, factorChannel01_128_s_16x8, factorChannel11_128_s_16x8, factorChannel21_128_s_16x8, factorChannel02_128_s_16x8, factorChannel12_128_s_16x8, factorChannel22_128_s_16x8, bias0_128_s_16x8, bias1_128_s_16x8, bias2_128_s_16x8);
			}

			source += blockSize * size_t(3);
			target += blockSize * size_t(3);
		}

		const size_t remainingPixels = size % blockSize;

		if (remainingPixels)
		{
			// we need to apply another iteration with (back shifted) pointers

			ocean_assert(remainingPixels < blockSize);

			const size_t offset = blockSize - remainingPixels;

			source -= offset * size_t(3);
			target -= offset * size_t(3);

			if (blockSize == 8)
			{
				convert3ChannelsTo3Channels8Pixels8BitPerChannel7BitPrecisionNEON(source, target, factorChannel00_128_s_16x8, factorChannel10_128_s_16x8, factorChannel20_128_s_16x8, factorChannel01_128_s_16x8, factorChannel11_128_s_16x8, factorChannel21_128_s_16x8, factorChannel02_128_s_16x8, factorChannel12_128_s_16x8, factorChannel22_128_s_16x8, bias0_128_s_16x8, bias1_128_s_16x8, bias2_128_s_16x8);
			}
			else
			{
				static_assert(blockSize == 8 || blockSize == 16, "Invalid block size!");
				ocean_assert(blockSize == 16);

				convert3ChannelsTo3Channels16Pixels8BitPerChannel7BitPrecisionNEON(source, target, factorChannel00_128_s_16x8, factorChannel10_128_s_16x8, factorChannel20_128_s_16x8, factorChannel01_128_s_16x8, factorChannel11_128_s_16x8, factorChannel21_128_s_16x8, factorChannel02_128_s_16x8, factorChannel12_128_s_16x8, factorChannel22_128_s_16x8, bias0_128_s_16x8, bias1_128_s_16x8, bias2_128_s_16x8);
			}
		}

		return;
	}

#endif

	const uint8_t* const targetEnd = target + size * size_t(3);

	while (target != targetEnd)
	{
		ocean_assert(target < targetEnd);

		target[0] = (uint8_t)(minmax<int16_t>(0, (int16_t(source[0]) * int16_t(factorChannel00_128) + int16_t(source[1]) * int16_t(factorChannel01_128) + int16_t(source[2]) * int16_t(factorChannel02_128)) / 128 + int16_t(bias0), 255));
		target[1] = (uint8_t)(minmax<int16_t>(0, (int16_t(source[0]) * int16_t(factorChannel10_128) + int16_t(source[1]) * int16_t(factorChannel11_128) + int16_t(source[2]) * int16_t(factorChannel12_128)) / 128 + int16_t(bias1), 255));
		target[2] = (uint8_t)(minmax<int16_t>(0, (int16_t(source[0]) * int16_t(factorChannel20_128) + int16_t(source[1]) * int16_t(factorChannel21_128) + int16_t(source[2]) * int16_t(factorChannel22_128)) / 128 + int16_t(bias2), 255));

		source += 3;
		target += 3;
	}
}

void FrameChannels::convertRow3ChannelsTo3Channels8BitPerChannel10BitPrecision(const uint8_t* source, uint8_t* target, const size_t size, const void* parameters)
{
	ocean_assert(source != nullptr && target != nullptr && size >= 1);

	ocean_assert(parameters != nullptr);
	const int* factors_1024_and_bias = reinterpret_cast<const int*>(parameters);
	ocean_assert(factors_1024_and_bias != nullptr);

#ifdef OCEAN_DEBUG
	for (unsigned int n = 0u; n < 12u; ++n)
	{
		ocean_assert(std::abs(factors_1024_and_bias[n]) <= 1024 * 16);
	}
#endif

	// layout of parameters f00, f10, f20, f01, f02, ..., f22, b0, b1, b2
	// with transformation:
	// t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
	// t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
	// t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2

	const int factorChannel00_1024 = factors_1024_and_bias[0];
	const int factorChannel10_1024 = factors_1024_and_bias[1];
	const int factorChannel20_1024 = factors_1024_and_bias[2];

	const int factorChannel01_1024 = factors_1024_and_bias[3];
	const int factorChannel11_1024 = factors_1024_and_bias[4];
	const int factorChannel21_1024 = factors_1024_and_bias[5];

	const int factorChannel02_1024 = factors_1024_and_bias[6];
	const int factorChannel12_1024 = factors_1024_and_bias[7];
	const int factorChannel22_1024 = factors_1024_and_bias[8];

	const int bias0 = factors_1024_and_bias[9];
	const int bias1 = factors_1024_and_bias[10];
	const int bias2 = factors_1024_and_bias[11];

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

	constexpr size_t blockSize = 16;
	const size_t blocks = size / blockSize;

	if (blocks >= 1)
	{
		const __m128i factorChannel00_1024_s_16x8 = _mm_set1_epi16(int16_t(factorChannel00_1024));
		const __m128i factorChannel10_1024_s_16x8 = _mm_set1_epi16(int16_t(factorChannel10_1024));
		const __m128i factorChannel20_1024_s_16x8 = _mm_set1_epi16(int16_t(factorChannel20_1024));

		const __m128i factorChannel01_1024_s_16x8 = _mm_set1_epi16(int16_t(factorChannel01_1024));
		const __m128i factorChannel11_1024_s_16x8 = _mm_set1_epi16(int16_t(factorChannel11_1024));
		const __m128i factorChannel21_1024_s_16x8 = _mm_set1_epi16(int16_t(factorChannel21_1024));

		const __m128i factorChannel02_1024_s_16x8 = _mm_set1_epi16(int16_t(factorChannel02_1024));
		const __m128i factorChannel12_1024_s_16x8 = _mm_set1_epi16(int16_t(factorChannel12_1024));
		const __m128i factorChannel22_1024_s_16x8 = _mm_set1_epi16(int16_t(factorChannel22_1024));

		const __m128i bias0_1024_s_32x4 = _mm_set1_epi32(bias0 * 1024);
		const __m128i bias1_1024_s_32x4 = _mm_set1_epi32(bias1 * 1024);
		const __m128i bias2_1024_s_32x4 = _mm_set1_epi32(bias2 * 1024);

		for (size_t n = 0; n < blocks; ++n)
		{
			convert3ChannelsTo3Channels16Pixels8BitPerChannel10BitPrecisionSSE(source, target, factorChannel00_1024_s_16x8, factorChannel10_1024_s_16x8, factorChannel20_1024_s_16x8, factorChannel01_1024_s_16x8, factorChannel11_1024_s_16x8, factorChannel21_1024_s_16x8, factorChannel02_1024_s_16x8, factorChannel12_1024_s_16x8, factorChannel22_1024_s_16x8, bias0_1024_s_32x4, bias1_1024_s_32x4, bias2_1024_s_32x4);

			source += blockSize * size_t(3);
			target += blockSize * size_t(3);
		}

		const size_t remainingPixels = size % blockSize;

		if (remainingPixels)
		{
			// we need to apply another iteration with (back shifted) pointers

			ocean_assert(remainingPixels < blockSize);

			const size_t offset = blockSize - remainingPixels;

			source -= offset * size_t(3);
			target -= offset * size_t(3);

			convert3ChannelsTo3Channels16Pixels8BitPerChannel10BitPrecisionSSE(source, target, factorChannel00_1024_s_16x8, factorChannel10_1024_s_16x8, factorChannel20_1024_s_16x8, factorChannel01_1024_s_16x8, factorChannel11_1024_s_16x8, factorChannel21_1024_s_16x8, factorChannel02_1024_s_16x8, factorChannel12_1024_s_16x8, factorChannel22_1024_s_16x8, bias0_1024_s_32x4, bias1_1024_s_32x4, bias2_1024_s_32x4);
		}

		return;
	}

#elif defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	constexpr size_t blockSize = 16;
	const size_t blocks = size / blockSize;

	if (blocks >= 1)
	{
		const int16x4_t factorChannel00_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel00_1024));
		const int16x4_t factorChannel10_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel10_1024));
		const int16x4_t factorChannel20_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel20_1024));

		const int16x4_t factorChannel01_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel01_1024));
		const int16x4_t factorChannel11_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel11_1024));
		const int16x4_t factorChannel21_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel21_1024));

		const int16x4_t factorChannel02_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel02_1024));
		const int16x4_t factorChannel12_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel12_1024));
		const int16x4_t factorChannel22_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel22_1024));

		const int32x4_t bias0_1024_s_32x4 = vdupq_n_s32(bias0 * 1024);
		const int32x4_t bias1_1024_s_32x4 = vdupq_n_s32(bias1 * 1024);
		const int32x4_t bias2_1024_s_32x4 = vdupq_n_s32(bias2 * 1024);

		for (size_t n = 0; n < blocks; ++n)
		{
			if (blockSize == 8)
			{
				convert3ChannelsTo3Channels8Pixels8BitPerChannel10BitPrecisionNEON(source, target, factorChannel00_1024_s_16x4, factorChannel10_1024_s_16x4, factorChannel20_1024_s_16x4, factorChannel01_1024_s_16x4, factorChannel11_1024_s_16x4, factorChannel21_1024_s_16x4, factorChannel02_1024_s_16x4, factorChannel12_1024_s_16x4, factorChannel22_1024_s_16x4, bias0_1024_s_32x4, bias1_1024_s_32x4, bias2_1024_s_32x4);
			}
			else
			{
				static_assert(blockSize == 8 || blockSize == 16, "Invalid block size!");
				ocean_assert(blockSize == 16);

				convert3ChannelsTo3Channels16Pixels8BitPerChannel10BitPrecisionNEON(source, target, factorChannel00_1024_s_16x4, factorChannel10_1024_s_16x4, factorChannel20_1024_s_16x4, factorChannel01_1024_s_16x4, factorChannel11_1024_s_16x4, factorChannel21_1024_s_16x4, factorChannel02_1024_s_16x4, factorChannel12_1024_s_16x4, factorChannel22_1024_s_16x4, bias0_1024_s_32x4, bias1_1024_s_32x4, bias2_1024_s_32x4);
			}

			source += blockSize * size_t(3);
			target += blockSize * size_t(3);
		}

		const size_t remainingPixels = size % blockSize;

		if (remainingPixels)
		{
			// we need to apply another iteration with (back shifted) pointers

			ocean_assert(remainingPixels < blockSize);

			const size_t offset = blockSize - remainingPixels;

			source -= offset * size_t(3);
			target -= offset * size_t(3);

			if (blockSize == 8)
			{
				convert3ChannelsTo3Channels8Pixels8BitPerChannel10BitPrecisionNEON(source, target, factorChannel00_1024_s_16x4, factorChannel10_1024_s_16x4, factorChannel20_1024_s_16x4, factorChannel01_1024_s_16x4, factorChannel11_1024_s_16x4, factorChannel21_1024_s_16x4, factorChannel02_1024_s_16x4, factorChannel12_1024_s_16x4, factorChannel22_1024_s_16x4, bias0_1024_s_32x4, bias1_1024_s_32x4, bias2_1024_s_32x4);
			}
			else
			{
				static_assert(blockSize == 8 || blockSize == 16, "Invalid block size!");
				ocean_assert(blockSize == 16);

				convert3ChannelsTo3Channels16Pixels8BitPerChannel10BitPrecisionNEON(source, target, factorChannel00_1024_s_16x4, factorChannel10_1024_s_16x4, factorChannel20_1024_s_16x4, factorChannel01_1024_s_16x4, factorChannel11_1024_s_16x4, factorChannel21_1024_s_16x4, factorChannel02_1024_s_16x4, factorChannel12_1024_s_16x4, factorChannel22_1024_s_16x4, bias0_1024_s_32x4, bias1_1024_s_32x4, bias2_1024_s_32x4);
			}
		}

		return;
	}

#endif

	const uint8_t* const targetEnd = target + size * size_t(3);

	while (target != targetEnd)
	{
		ocean_assert(target < targetEnd);

		target[0] = (uint8_t)(minmax<int>(0, (source[0] * int16_t(factorChannel00_1024) + source[1] * int16_t(factorChannel01_1024) + source[2] * int16_t(factorChannel02_1024)) / 1024 + int16_t(bias0), 255));
		target[1] = (uint8_t)(minmax<int>(0, (source[0] * int16_t(factorChannel10_1024) + source[1] * int16_t(factorChannel11_1024) + source[2] * int16_t(factorChannel12_1024)) / 1024 + int16_t(bias1), 255));
		target[2] = (uint8_t)(minmax<int>(0, (source[0] * int16_t(factorChannel20_1024) + source[1] * int16_t(factorChannel21_1024) + source[2] * int16_t(factorChannel22_1024)) / 1024 + int16_t(bias2), 255));

		source += 3;
		target += 3;
	}
}

void FrameChannels::convertRow3ChannelsTo4Channels8BitPerChannel6BitPrecision(const uint8_t* source, uint8_t* target, const size_t size, const void* parameters)
{
	ocean_assert(source != nullptr && target != nullptr && size >= 1);

	ocean_assert(parameters != nullptr);
	const int* factors_64_and_bias = reinterpret_cast<const int*>(parameters);
	ocean_assert(factors_64_and_bias != nullptr);

	// layout of parameters f00, f10, f20, f01, f02, ..., f22, b0, b1, b2, vc3
	// with transformation:
	// t0 = clamp(0, f00 * (s0 - b0) + f01 * (s1 - b1) + f02 * (s2 - b2), 255)
	// t1 = clamp(0, f10 * (s0 - b0) + f11 * (s1 - b1) + f12 * (s2 - b2), 255)
	// t2 = clamp(0, f20 * (s0 - b0) + f21 * (s1 - b1) + f22 * (s2 - b2), 255)

	const int factorChannel00_64 = factors_64_and_bias[0];
	const int factorChannel10_64 = factors_64_and_bias[1];
	const int factorChannel20_64 = factors_64_and_bias[2];

	const int factorChannel01_64 = factors_64_and_bias[3];
	const int factorChannel11_64 = factors_64_and_bias[4];
	const int factorChannel21_64 = factors_64_and_bias[5];

	const int factorChannel02_64 = factors_64_and_bias[6];
	const int factorChannel12_64 = factors_64_and_bias[7];
	const int factorChannel22_64 = factors_64_and_bias[8];

	ocean_assert(std::abs(factorChannel00_64 + factorChannel01_64 + factorChannel02_64) < 64 * 4);
	ocean_assert(std::abs(factorChannel10_64 + factorChannel11_64 + factorChannel12_64) < 64 * 4);
	ocean_assert(std::abs(factorChannel20_64 + factorChannel21_64 + factorChannel22_64) < 64 * 4);

	const int bias0 = factors_64_and_bias[9];
	const int bias1 = factors_64_and_bias[10];
	const int bias2 = factors_64_and_bias[11];

	ocean_assert(bias0 >= 0 && bias0 <= 128);
	ocean_assert(bias1 >= 0 && bias1 <= 128);
	ocean_assert(bias2 >= 0 && bias2 <= 128);

	const int valueChannel3 = factors_64_and_bias[12];
	ocean_assert(valueChannel3 >= 0 && valueChannel3 <= 255);

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

	// **TODO** add SSE-based implementation

#elif defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	constexpr size_t blockSize = 16;
	const size_t blocks = size / blockSize;

	if (blocks >= 1)
	{
		const int16x8_t factorChannel00_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel00_64));
		const int16x8_t factorChannel10_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel10_64));
		const int16x8_t factorChannel20_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel20_64));

		const int16x8_t factorChannel01_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel01_64));
		const int16x8_t factorChannel11_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel11_64));
		const int16x8_t factorChannel21_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel21_64));

		const int16x8_t factorChannel02_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel02_64));
		const int16x8_t factorChannel12_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel12_64));
		const int16x8_t factorChannel22_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel22_64));

		const uint8x8_t bias0_u_8x8 = vdup_n_u8((uint8_t)(bias0));
		const uint8x8_t bias1_u_8x8 = vdup_n_u8((uint8_t)(bias1));
		const uint8x8_t bias2_u_8x8 = vdup_n_u8((uint8_t)(bias2));

		const uint8x16_t valueChannel3_u_8x16 = vdupq_n_u8((uint8_t)(valueChannel3));

		for (size_t n = 0; n < blocks; ++n)
		{
			convert3ChannelsTo4Channels16Pixels8BitPerChannel6BitPrecisionNEON(source, target, factorChannel00_64_s_16x8, factorChannel10_64_s_16x8, factorChannel20_64_s_16x8, factorChannel01_64_s_16x8, factorChannel11_64_s_16x8, factorChannel21_64_s_16x8, factorChannel02_64_s_16x8, factorChannel12_64_s_16x8, factorChannel22_64_s_16x8, bias0_u_8x8, bias1_u_8x8, bias2_u_8x8, valueChannel3_u_8x16);

			source += blockSize * size_t(3);
			target += blockSize * size_t(4);
		}

		const size_t remainingPixels = size % blockSize;

		if (remainingPixels)
		{
			// we need to apply another iteration with (back shifted) pointers

			ocean_assert(remainingPixels < blockSize);

			const size_t offset = blockSize - remainingPixels;

			source -= offset * size_t(3);
			target -= offset * size_t(4);

			convert3ChannelsTo4Channels16Pixels8BitPerChannel6BitPrecisionNEON(source, target, factorChannel00_64_s_16x8, factorChannel10_64_s_16x8, factorChannel20_64_s_16x8, factorChannel01_64_s_16x8, factorChannel11_64_s_16x8, factorChannel21_64_s_16x8, factorChannel02_64_s_16x8, factorChannel12_64_s_16x8, factorChannel22_64_s_16x8, bias0_u_8x8, bias1_u_8x8, bias2_u_8x8, valueChannel3_u_8x16);
		}

		return;
	}

#endif

	const uint8_t* const targetEnd = target + size * size_t(4);

	while (target != targetEnd)
	{
		ocean_assert(target < targetEnd);

		const int16_t source0 = int16_t(source[0] - int16_t(bias0));
		const int16_t source1 = int16_t(source[1] - int16_t(bias1));
		const int16_t source2 = int16_t(source[2] - int16_t(bias2));

		target[0] = (uint8_t)(minmax<int16_t>(0, (source0 * int16_t(factorChannel00_64) + source1 * int16_t(factorChannel01_64) + source2 * int16_t(factorChannel02_64)) / 64, 255));
		target[1] = (uint8_t)(minmax<int16_t>(0, (source0 * int16_t(factorChannel10_64) + source1 * int16_t(factorChannel11_64) + source2 * int16_t(factorChannel12_64)) / 64, 255));
		target[2] = (uint8_t)(minmax<int16_t>(0, (source0 * int16_t(factorChannel20_64) + source1 * int16_t(factorChannel21_64) + source2 * int16_t(factorChannel22_64)) / 64, 255));
		target[3] = (uint8_t)valueChannel3;

		source += 3;
		target += 4;
	}
}

void FrameChannels::convertRow4ChannelsTo2Channels8BitPerChannel7BitPrecision(const uint8_t* source, uint8_t* target, const size_t size, const void* multiplicationFactors_128)
{
	ocean_assert(multiplicationFactors_128 != nullptr);
	const unsigned int* channelFactors_128 = reinterpret_cast<const unsigned int*>(multiplicationFactors_128);
	ocean_assert(channelFactors_128 != nullptr);

	const unsigned int factorChannel00_128 = channelFactors_128[0];
	const unsigned int factorChannel10_128 = channelFactors_128[1];

	const unsigned int factorChannel01_128 = channelFactors_128[2];
	const unsigned int factorChannel11_128 = channelFactors_128[3];

	const unsigned int factorChannel02_128 = channelFactors_128[4];
	const unsigned int factorChannel12_128 = channelFactors_128[5];

	const unsigned int factorChannel03_128 = channelFactors_128[6];
	const unsigned int factorChannel13_128 = channelFactors_128[7];

	ocean_assert(factorChannel00_128 <= 128u && factorChannel01_128 <= 128u && factorChannel02_128 <= 128u && factorChannel03_128 <= 128u);
	ocean_assert(factorChannel10_128 <= 128u && factorChannel11_128 <= 128u && factorChannel12_128 <= 128u && factorChannel13_128 <= 128u);

	ocean_assert(factorChannel00_128 + factorChannel01_128 + factorChannel02_128 + factorChannel03_128 == 128u);
	ocean_assert(factorChannel10_128 + factorChannel11_128 + factorChannel12_128 + factorChannel13_128 == 128u);

	ocean_assert(source != nullptr && target != nullptr && size >= 1);

	const uint8_t* const targetEnd = target + size * size_t(2);

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

	constexpr size_t blockSize = 16;
	const size_t blocks = size / blockSize;

	const __m128i multiplicationFactorsChannel0_0123_128_s_16x8 = _mm_set1_epi64x(((unsigned long long)factorChannel00_128 << 0ull) | ((unsigned long long)factorChannel01_128 << 16ull) | ((unsigned long long)factorChannel02_128 << 32ull) | ((unsigned long long)factorChannel03_128 << 48ull));
	const __m128i multiplicationFactorsChannel1_0123_128_s_16x8 = _mm_set1_epi64x(((unsigned long long)factorChannel10_128 << 0ull) | ((unsigned long long)factorChannel11_128 << 16ull) | ((unsigned long long)factorChannel12_128 << 32ull) | ((unsigned long long)factorChannel13_128 << 48ull));

	for (size_t n = 0; n < blocks; ++n)
	{
		convert4ChannelsTo2Channels16Pixels8BitPerChannel7BitPrecisionSSE(source, target, multiplicationFactorsChannel0_0123_128_s_16x8, multiplicationFactorsChannel1_0123_128_s_16x8);

		source += blockSize * size_t(4);
		target += blockSize * size_t(2);
	}

#elif defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	constexpr size_t blockSize = 8;
	const size_t blocks = size / blockSize;

	const uint8x8_t factorChannel00_128_u_8x8 = vdup_n_u8((uint8_t)factorChannel00_128);
	const uint8x8_t factorChannel10_128_u_8x8 = vdup_n_u8((uint8_t)factorChannel10_128);

	const uint8x8_t factorChannel01_128_u_8x8 = vdup_n_u8((uint8_t)factorChannel01_128);
	const uint8x8_t factorChannel11_128_u_8x8 = vdup_n_u8((uint8_t)factorChannel11_128);

	const uint8x8_t factorChannel02_128_u_8x8 = vdup_n_u8((uint8_t)factorChannel02_128);
	const uint8x8_t factorChannel12_128_u_8x8 = vdup_n_u8((uint8_t)factorChannel12_128);

	const uint8x8_t factorChannel03_128_u_8x8 = vdup_n_u8((uint8_t)factorChannel03_128);
	const uint8x8_t factorChannel13_128_u_8x8 = vdup_n_u8((uint8_t)factorChannel13_128);

	for (size_t n = 0; n < blocks; ++n)
	{
		convert4ChannelsTo2Channels8Pixels8BitPerChannel7BitPrecisionNEON(source, target, factorChannel00_128_u_8x8, factorChannel10_128_u_8x8, factorChannel01_128_u_8x8, factorChannel11_128_u_8x8, factorChannel02_128_u_8x8, factorChannel12_128_u_8x8, factorChannel03_128_u_8x8, factorChannel13_128_u_8x8);

		source += blockSize * size_t(4);
		target += blockSize * size_t(2);
	}

#endif

	while (target != targetEnd)
	{
		ocean_assert(target < targetEnd);

		*target++ = (uint8_t)((source[0] * factorChannel00_128 + source[1] * factorChannel01_128 + source[2] * factorChannel02_128 + source[3] * factorChannel03_128 + 64u) >> 7u);
		*target++ = (uint8_t)((source[0] * factorChannel10_128 + source[1] * factorChannel11_128 + source[2] * factorChannel12_128 + source[3] * factorChannel13_128 + 64u) >> 7u);

		source += 4;
	}
}

void FrameChannels::convertRow4ChannelsTo3Channels8BitPerChannel7BitPrecision(const uint8_t* source, uint8_t* target, const size_t size, const void* parameters)
{
	ocean_assert(source != nullptr && target != nullptr && size >= 1);

	ocean_assert(parameters != nullptr);
	const int* factors_128_and_bias = reinterpret_cast<const int*>(parameters);
	ocean_assert(factors_128_and_bias != nullptr);

	// layout of parameters f00, f10, f20, f01, f02, ..., f23, b0, b1, b2
	// with transformation:
	// t0 = f00 * s0 + f01 * s1 + f02 * s2 + f03 * s3 + b0
	// t1 = f10 * s0 + f11 * s1 + f12 * s2 + f13 * s3 + b1
	// t2 = f20 * s0 + f21 * s1 + f22 * s2 + f23 * s3 + b2

	const int factorChannel00_128 = factors_128_and_bias[0];
	const int factorChannel10_128 = factors_128_and_bias[1];
	const int factorChannel20_128 = factors_128_and_bias[2];

	const int factorChannel01_128 = factors_128_and_bias[3];
	const int factorChannel11_128 = factors_128_and_bias[4];
	const int factorChannel21_128 = factors_128_and_bias[5];

	const int factorChannel02_128 = factors_128_and_bias[6];
	const int factorChannel12_128 = factors_128_and_bias[7];
	const int factorChannel22_128 = factors_128_and_bias[8];

	const int factorChannel03_128 = factors_128_and_bias[9];
	const int factorChannel13_128 = factors_128_and_bias[10];
	const int factorChannel23_128 = factors_128_and_bias[11];

	ocean_assert(std::abs(factorChannel00_128 + factorChannel01_128 + factorChannel02_128 + factorChannel03_128) < 128);
	ocean_assert(std::abs(factorChannel10_128 + factorChannel11_128 + factorChannel12_128 + factorChannel13_128) < 128);
	ocean_assert(std::abs(factorChannel20_128 + factorChannel21_128 + factorChannel22_128 + factorChannel23_128) < 128);

	const int bias0 = factors_128_and_bias[12];
	const int bias1 = factors_128_and_bias[13];
	const int bias2 = factors_128_and_bias[14];

	ocean_assert(std::abs(bias0) <= 128 && std::abs(bias1) <= 128 && std::abs(bias2) <= 128);

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

	// **TODO** add SSE implementation

#elif defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	// **TODO** add NEON implementation

#endif

	const uint8_t* const targetEnd = target + size * size_t(3);

	while (target != targetEnd)
	{
		ocean_assert(target < targetEnd);

		target[0] = (uint8_t)(minmax<int16_t>(0, (int16_t(source[0]) * int16_t(factorChannel00_128) + int16_t(source[1]) * int16_t(factorChannel01_128) + int16_t(source[2]) * int16_t(factorChannel02_128) + int16_t(source[3]) * int16_t(factorChannel03_128)) / 128 + int16_t(bias0), 255));
		target[1] = (uint8_t)(minmax<int16_t>(0, (int16_t(source[0]) * int16_t(factorChannel10_128) + int16_t(source[1]) * int16_t(factorChannel11_128) + int16_t(source[2]) * int16_t(factorChannel12_128) + int16_t(source[3]) * int16_t(factorChannel13_128)) / 128 + int16_t(bias1), 255));
		target[2] = (uint8_t)(minmax<int16_t>(0, (int16_t(source[0]) * int16_t(factorChannel20_128) + int16_t(source[1]) * int16_t(factorChannel21_128) + int16_t(source[2]) * int16_t(factorChannel22_128) + int16_t(source[3]) * int16_t(factorChannel23_128)) / 128 + int16_t(bias2), 255));

		source += 4;
		target += 3;
	}
}

void FrameChannels::transformGenericSubset(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const RowReversePixelOrderFunction<void> rowReversePixelOrderFunction, const unsigned int bytesPerRow, const unsigned int sourceStrideBytes, const unsigned int targetStrideBytes, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(source != target);

	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(rowReversePixelOrderFunction != nullptr);

	ocean_assert(sourceStrideBytes >= width && targetStrideBytes >= width);

	ocean_assert(numberRows > 0u);
	ocean_assert(firstRow + numberRows <= height);

	switch (flag)
	{
		case CONVERT_NORMAL:
		case CONVERT_FLIPPED:
		{
			const int signedTargetStrideBytes = flag == CONVERT_NORMAL ? int(targetStrideBytes) : -int(targetStrideBytes);

			if (flag == CONVERT_FLIPPED)
			{
				target += (height - 1u) * targetStrideBytes;
			}
			else if (sourceStrideBytes == targetStrideBytes && sourceStrideBytes == bytesPerRow)
			{
				ocean_assert(flag == CONVERT_NORMAL);

				// we do not have padding elements in source and target frame
				memcpy(target + bytesPerRow * firstRow, source + bytesPerRow * firstRow, bytesPerRow * numberRows);
				break;
			}

			target += signedTargetStrideBytes * int(firstRow);
			source += sourceStrideBytes * firstRow;

			for (unsigned int r = 0u; r < numberRows; ++r)
			{
				memcpy(target, source, bytesPerRow);

				source += sourceStrideBytes;
				target += signedTargetStrideBytes;
			}

			break;
		}

		case CONVERT_MIRRORED:
		case CONVERT_FLIPPED_AND_MIRRORED:
		{
			const int signedTargetStrideBytes = flag == CONVERT_MIRRORED ? int(targetStrideBytes) : -int(targetStrideBytes);

			if (flag == CONVERT_FLIPPED_AND_MIRRORED)
			{
				target += (height - 1u) * targetStrideBytes;
			}

			target += signedTargetStrideBytes * int(firstRow);
			source += sourceStrideBytes * firstRow;

			for (unsigned int r = 0u; r < numberRows; ++r)
			{
				rowReversePixelOrderFunction((const void*)(source), (void*)(target), width);

				source += sourceStrideBytes;
				target += signedTargetStrideBytes;
			}

			break;
		}

		default:
			ocean_assert(false && "Not supported flag!");
	}
}

}

}
