/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"

#include "ocean/base/DataType.h"

namespace Ocean
{

namespace CV
{

FrameType::PixelFormats CVUtilities::definedPixelFormats(const FrameType::PixelFormats& genericPixelFormats)
{
	FrameType::PixelFormats pixelFormats = FrameType::definedPixelFormats();

	for (const FrameType::PixelFormat& genericPixelFormat : genericPixelFormats)
	{
		ocean_assert(FrameType::formatIsPureGeneric(genericPixelFormat));

		pixelFormats.emplace_back(genericPixelFormat);
	}

	return pixelFormats;
}

Frame CVUtilities::visualizeDistortion(const PinholeCamera& pinholeCamera, const bool undistort, const unsigned int horizontalBins, const unsigned int verticalBins, Worker* worker)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(horizontalBins != 0u || verticalBins != 0u);

	Frame frame(FrameType(pinholeCamera.width(), pinholeCamera.height(), FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
	frame.setValue(0xFFu);

	// horizontal lines
	for (unsigned int n = 1u; n < verticalBins; ++n)
	{
		const unsigned int y = n * frame.height() / verticalBins;
		memset(frame.row<uint8_t>(y), 0x00, frame.planeWidthBytes(0u));
	}

	// vertical lines
	for (unsigned int y = 0u; y < frame.height(); ++y)
	{
		uint8_t* const row = frame.row<uint8_t>(y);

		for (unsigned int n = 1u; n < horizontalBins; ++n)
		{
			const unsigned int x = n * frame.width() / horizontalBins;
			row[x] = 0x00u;
		}
	}

	const PinholeCamera perfectPinholeCamera(pinholeCamera.intrinsic(), pinholeCamera.width(), pinholeCamera.height());

	Frame result;

	if (undistort)
	{
		if (!FrameInterpolatorBilinear::Comfort::resampleCameraImage(frame, AnyCameraPinhole(pinholeCamera), SquareMatrix3(true), AnyCameraPinhole(perfectPinholeCamera), result, nullptr, worker))
		{
			ocean_assert(false && "This should never happen!");
		}
	}
	else
	{
		if (!FrameInterpolatorBilinear::Comfort::resampleCameraImage(frame, AnyCameraPinhole(perfectPinholeCamera), SquareMatrix3(true), AnyCameraPinhole(pinholeCamera), result, nullptr, worker))
		{
			ocean_assert(false && "This should never happen!");
		}
	}

	return result;
}

template <>
void CVUtilities::randomizMemory(Float16* data, const unsigned int widthElements, const unsigned int height, const unsigned int paddingElements, RandomGenerator& randomGenerator, const bool limitedValueRange)
{
	ocean_assert(data != nullptr);
	ocean_assert(widthElements >= 1u && height >= 1u);
	ocean_assert_and_suppress_unused(limitedValueRange == false, limitedValueRange);

	for (unsigned int y = 0u; y < height; ++y)
	{
		for (unsigned int x = 0u; x < widthElements; ++x)
		{
			const uint16_t sign = uint16_t(RandomI::random(randomGenerator, 1u));
			const uint16_t fraction = uint16_t(RandomI::random(randomGenerator, 1023u));
			const uint16_t exponent = uint16_t(RandomI::random(randomGenerator, 0u, 23u));

			*data++ = Float16(sign, fraction, exponent);
		}

		data += paddingElements;
	}
}

void CVUtilities::randomizeFrame(Frame& frame, const bool skipPaddingArea, RandomGenerator* randomGenerator, const bool limitedValueRange)
{
	if (!frame.isValid())
	{
		return;
	}

	if (skipPaddingArea == false)
	{
		for (unsigned int planeIndex = 0u; planeIndex < frame.planes().size(); ++planeIndex)
		{
			Frame fullFrame(FrameType(frame.strideElements(planeIndex), frame.planeHeight(planeIndex), FrameType::genericPixelFormat(frame.dataType(), 1u), FrameType::ORIGIN_UPPER_LEFT), frame.data<void>(planeIndex), Frame::CM_USE_KEEP_LAYOUT);

			randomizeFrame(fullFrame, true, randomGenerator, limitedValueRange);
		}

		return;
	}

	RandomGenerator localRandomGenerator(randomGenerator);

	for (unsigned int planeIndex = 0u; planeIndex < frame.planes().size(); ++planeIndex)
	{
		const Frame::Plane& plane = frame.planes()[planeIndex];

		const unsigned int widthElements = skipPaddingArea ? plane.widthElements() : plane.strideElements();
		const unsigned int paddingElements = skipPaddingArea ? frame.paddingElements() : 0u;

		switch (frame.dataType())
		{
			case FrameType::DT_UNSIGNED_INTEGER_8:
				randomizMemory<uint8_t>(frame.data<uint8_t>(planeIndex), widthElements, plane.height(), paddingElements, localRandomGenerator, limitedValueRange);
				break;

			case FrameType::DT_SIGNED_INTEGER_8:
				randomizMemory<int8_t>(frame.data<int8_t>(planeIndex), widthElements, plane.height(), paddingElements, localRandomGenerator, limitedValueRange);
				break;

			case FrameType::DT_UNSIGNED_INTEGER_16:
				randomizMemory<uint16_t>(frame.data<uint16_t>(planeIndex), widthElements, plane.height(), paddingElements, localRandomGenerator, limitedValueRange);
				break;

			case FrameType::DT_SIGNED_INTEGER_16:
				randomizMemory<int16_t>(frame.data<int16_t>(planeIndex), widthElements, plane.height(), paddingElements, localRandomGenerator, limitedValueRange);
				break;

			case FrameType::DT_UNSIGNED_INTEGER_32:
				randomizMemory<uint32_t>(frame.data<uint32_t>(planeIndex), widthElements, plane.height(), paddingElements, localRandomGenerator, limitedValueRange);
				break;

			case FrameType::DT_SIGNED_INTEGER_32:
				randomizMemory<int32_t>(frame.data<int32_t>(planeIndex), widthElements, plane.height(), paddingElements, localRandomGenerator, limitedValueRange);
				break;

			case FrameType::DT_UNSIGNED_INTEGER_64:
				randomizMemory<uint64_t>(frame.data<uint64_t>(planeIndex), widthElements, plane.height(), paddingElements, localRandomGenerator, limitedValueRange);
				break;

			case FrameType::DT_SIGNED_INTEGER_64:
				randomizMemory<int64_t>(frame.data<int64_t>(planeIndex), widthElements, plane.height(), paddingElements, localRandomGenerator, limitedValueRange);
				break;

			case FrameType::DT_SIGNED_FLOAT_16:
				randomizMemory<Float16>(frame.data<Float16>(planeIndex), widthElements, plane.height(), paddingElements, localRandomGenerator, limitedValueRange);
				break;

			case FrameType::DT_SIGNED_FLOAT_32:
				randomizMemory<float>(frame.data<float>(planeIndex), widthElements, plane.height(), paddingElements, localRandomGenerator, limitedValueRange);
				break;

			case FrameType::DT_SIGNED_FLOAT_64:
				randomizMemory<double>(frame.data<double>(planeIndex), widthElements, plane.height(), paddingElements, localRandomGenerator, limitedValueRange);
				break;

			default:
				ocean_assert(false && "Invalid data type!");
				break;
		}
	}
}

Frame CVUtilities::randomizedFrame(const FrameType& frameType, RandomGenerator* randomGenerator, const bool limitedValueRange)
{
	if (!frameType.isValid())
	{
		return Frame();
	}

	RandomGenerator localRandomGenerator(randomGenerator);

	Indices32 paddingElementsPerPlane;

	if (RandomI::boolean(localRandomGenerator))
	{
		for (unsigned int n = 0u; n < frameType.numberPlanes(); ++n)
		{
			if (RandomI::boolean(localRandomGenerator))
			{
				paddingElementsPerPlane.emplace_back(RandomI::random(localRandomGenerator, 1u, 100u));
			}
			else
			{
				paddingElementsPerPlane.emplace_back(0u);
			}
		}
	}

	Frame frame(frameType, paddingElementsPerPlane);

	randomizeFrame(frame, false /*skipPaddingArea*/, &localRandomGenerator, limitedValueRange);

	frame.setTimestamp(Timestamp(RandomD::scalar(localRandomGenerator, -1000, 1000)));
	frame.setRelativeTimestamp(Timestamp(RandomD::scalar(localRandomGenerator, -1000, 1000)));

	return frame;
}

Frame CVUtilities::randomizedBinaryMask(const unsigned int width, const unsigned int height, const uint8_t maskValue, RandomGenerator* randomGenerator)
{
	ocean_assert(width >= 1u && height >= 1u);

	RandomGenerator localRandomGenerator(randomGenerator);

	const uint8_t nonMaskValue = 0xFFu - maskValue;

	unsigned int paddingElements = 0u;

	if (RandomI::boolean(localRandomGenerator))
	{
		paddingElements = RandomI::random(localRandomGenerator, 1u, 100u);
	}

	Frame mask(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), paddingElements);
	mask.setValue(nonMaskValue);

	const unsigned int pixels = mask.pixels();
	const unsigned int pixels_001 = pixels / 10000u; // 0.01%
	const unsigned int pixels_0005 = pixels / 20000u; // 0.005%
	const unsigned int pixels_10 = pixels / 10u; // 10%

	const unsigned int minDimension = std::min(width, height);

	// some random circles (with color maskValue)

	for (unsigned int n = 0u; n < pixels_001; ++n) // 0.01%
	{
		const unsigned int x = RandomI::random(localRandomGenerator, width - 1u);
		const unsigned int y = RandomI::random(localRandomGenerator, height - 1u);

		const unsigned int sizeX = RandomI::random(localRandomGenerator, 3u, std::max(3u, minDimension / 10u)) | 0x00000001u;
		const unsigned int sizeY = RandomI::random(localRandomGenerator, 3u, std::max(3u, minDimension / 10u)) | 0x00000001u;

		CV::Canvas::ellipse8BitPerChannel<1u>(mask.data<uint8_t>(), mask.width(), mask.height(), CV::PixelPosition(x, y), sizeX, sizeY, &maskValue, mask.paddingElements());
	}

	// some random circles (with color nonMaskValue)

	for (unsigned int n = 0u; n < pixels_0005; ++n) // 0.005%
	{
		const unsigned int x = RandomI::random(localRandomGenerator, width - 1u);
		const unsigned int y = RandomI::random(localRandomGenerator, height - 1u);

		const unsigned int sizeX = RandomI::random(localRandomGenerator, 3u, std::max(3u, minDimension / 10u)) | 0x00000001u;
		const unsigned int sizeY = RandomI::random(localRandomGenerator, 3u, std::max(3u, minDimension / 10u)) | 0x00000001u;

		CV::Canvas::ellipse8BitPerChannel<1u>(mask.data<uint8_t>(), mask.width(), mask.height(), CV::PixelPosition(x, y), sizeX, sizeY, &nonMaskValue, mask.paddingElements());
	}

	// some random pepper

	for (unsigned int n = 0u; n < pixels_10; ++n) // 10%
	{
		const unsigned int x = RandomI::random(localRandomGenerator, width - 1u);
		const unsigned int y = RandomI::random(localRandomGenerator, height - 1u);

		mask.pixel<uint8_t>(x, y)[0] = maskValue;
	}

	// random noise in padding area

	if (mask.paddingElements() != 0u)
	{
		for (unsigned int y = 0u; y < mask.height(); ++y)
		{
			uint8_t* const row = mask.row<uint8_t>(y) + mask.width();

			for (unsigned int x = 0u; x < mask.paddingElements(); ++x)
			{
				row[x] = uint8_t(RandomI::random(localRandomGenerator, 255u));
			}
		}
	}

	mask.setTimestamp(Timestamp(RandomD::scalar(localRandomGenerator, -1000, 1000)));
	mask.setRelativeTimestamp(Timestamp(RandomD::scalar(localRandomGenerator, -1000, 1000)));

	return mask;
}

bool CVUtilities::isBorderZero(const Frame& frame)
{
	if (frame.isValid() && frame.numberPlanes() == 1u)
	{
		switch (frame.dataType())
		{
			case FrameType::DT_UNSIGNED_INTEGER_8:
				return isBorderZero<uint8_t>(frame);

			case FrameType::DT_SIGNED_INTEGER_8:
				return isBorderZero<int8_t>(frame);

			case FrameType::DT_UNSIGNED_INTEGER_16:
				return isBorderZero<uint16_t>(frame);

			case FrameType::DT_SIGNED_INTEGER_16:
				return isBorderZero<int16_t>(frame);

			case FrameType::DT_UNSIGNED_INTEGER_32:
				return isBorderZero<uint32_t>(frame);

			case FrameType::DT_SIGNED_INTEGER_32:
				return isBorderZero<int32_t>(frame);

			case FrameType::DT_UNSIGNED_INTEGER_64:
				return isBorderZero<uint64_t>(frame);

			case FrameType::DT_SIGNED_INTEGER_64:
				return isBorderZero<int64_t>(frame);

			case FrameType::DT_SIGNED_FLOAT_32:
				return isBorderZero<float>(frame);

			case FrameType::DT_SIGNED_FLOAT_64:
				return isBorderZero<double>(frame);

			default:
				break;
		}
	}

	ocean_assert(false && "Invalid frame type!");
	return false;
}

bool CVUtilities::isPaddingMemoryIdentical(const Frame& frameA, const Frame& frameB)
{
	ocean_assert(frameA && frameB);

	if (frameA.paddingElements() != frameB.paddingElements())
	{
		return false;
	}

	if (frameA.height() != frameB.height())
	{
		return false;
	}

	if (frameA.paddingElements() == 0u)
	{
		ocean_assert(frameB.paddingElements() == 0u);
		return true;
	}

	const unsigned int imageBytes = frameA.strideBytes() - frameA.paddingBytes();

	for (unsigned int y = 0u; y < frameA.height(); ++y)
	{
		if (memcmp((void*)(frameA.constrow<uint8_t>(y) + imageBytes), (void*)(frameB.constrow<uint8_t>(y) + imageBytes), frameA.paddingBytes()) != 0)
		{
			return false;
		}
	}

	return true;
}

Frame CVUtilities::createCheckerboardImage(const unsigned int width, const unsigned int height, const unsigned int horizontalElements, const unsigned int verticalElements, const unsigned int paddingElements, const uint8_t bright, const uint8_t dark)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(horizontalElements >= 1u && verticalElements >= 1u);
	ocean_assert(width % horizontalElements == 0u && height % verticalElements == 0u);

	Frame frame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), paddingElements);
	frame.setValue(bright);

	const unsigned int elementColumns = width / horizontalElements;
	const unsigned int elementRows = height / verticalElements;

	for (unsigned int vElement = 0u; vElement < verticalElements; ++vElement)
	{
		for (unsigned int hElement = 0u; hElement < horizontalElements; ++hElement)
		{
			// the upper left element is a dark elements, neighboring elements switch between dark and bright

			if ((vElement % 2u) == (hElement % 2u))
			{
				frame.subFrame(hElement * elementColumns, vElement * elementRows, elementColumns, elementRows).setValue(dark);
			}
		}
	}

	return frame;
}

template <typename T>
bool CVUtilities::isBorderZero(const Frame& frame)
{
	const unsigned int width_1 = frame.width() - 1u;
	const unsigned int height_1 = frame.height() - 1u;

	// upper and lower border
	for (unsigned int x = 0u; x < frame.width(); ++x)
	{
		for (unsigned int n = 0u; n < frame.channels(); ++n)
		{
			if (frame.constpixel<T>(x, 0u)[n] != T(0))
			{
				return false;
			}

			if (frame.constpixel<T>(x, height_1)[n] != T(0))
			{
				return false;
			}
		}
	}

	// left and right border
	for (unsigned int y = 0u; y < frame.height(); ++y)
	{
		for (unsigned int n = 0u; n < frame.channels(); ++n)
		{
			if (frame.constpixel<T>(0u, y)[n] != T(0))
			{
				return false;
			}

			if (frame.constpixel<T>(width_1, y)[n] != T(0))
			{
				return false;
			}
		}
	}

	return true;
}

template <typename T>
void CVUtilities::randomizMemory(T* data, const unsigned int widthElements, const unsigned int height, const unsigned int paddingElements, RandomGenerator& randomGenerator, const bool limitedValueRange)
{
	ocean_assert(data != nullptr);
	ocean_assert(widthElements >= 1u && height >= 1u);

	for (unsigned int y = 0u; y < height; ++y)
	{
		for (unsigned int x = 0u; x < widthElements; ++x)
		{
			if constexpr (std::is_floating_point<T>::value)
			{
				*data = RandomT<T>::scalar(randomGenerator, T(0), T(255));
			}
			else if constexpr (std::is_signed<T>::value)
			{
				if (limitedValueRange)
				{
					*data = T(RandomI::random(randomGenerator, -128, 127));
				}
				else
				{
					for (unsigned int n = 0u; n < sizeof(T); ++n)
					{
						((uint8_t*)data)[n] = uint8_t(RandomI::random(randomGenerator, 255u));
					}
				}
			}
			else
			{
				ocean_assert(std::is_unsigned<T>::value);

				if (limitedValueRange)
				{
					*data = T(RandomI::random(randomGenerator, 255u));
				}
				else
				{
					for (unsigned int n = 0u; n < sizeof(T); ++n)
					{
						((uint8_t*)data)[n] = uint8_t(RandomI::random(randomGenerator, 255u));
					}
				}
			}

			++data;
		}

		data += paddingElements;
	}
}

}

}
