/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/advanced/WhitePointDetector.h"
#include "ocean/cv/advanced/ColorChannelMapper.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

VectorF3 WhitePointDetector::detectWhitePointHistogram(const Frame& frame, Worker* worker)
{
	ocean_assert(frame && frame.pixelFormat() == FrameType::FORMAT_RGB24);

	const CV::Histogram::Histogram8BitPerChannel<3u> histogram(CV::Histogram::determineHistogram8BitPerChannel<3u>(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), worker));

	const unsigned int threshold = max(1u, (frame.pixels() + 1000u) / 2000u); // 0.05 % of number of pixels

	VectorF3 whitePoint;

	constexpr float inv255 = 1.0f / 255.0f;

	for (unsigned int c = 0u; c < 3u; c++)
	{
		whitePoint[c] = float(colorRange(histogram, (unsigned char)(c), threshold)) * inv255;
	}

	return whitePoint;
}

VectorF3 WhitePointDetector::detectWhitePointGrayPoints(const Frame& frame, const float grayThreshold)
{
	ocean_assert(frame && frame.pixelFormat() == FrameType::FORMAT_RGB24);

	VectorF3 whitePoint(1.0f, 1.0f, 1.0f);

	unsigned int iteration = 0u;

	while (true)
	{
		Frame frameCopy(frame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

		if (iteration > 0u)
		{
			correctWhitePoint(frameCopy, whitePoint);
		}

		unsigned int grayCount = 0u;

		float accuU = 0.0f;
		float accuV = 0.0f;
		float maxY = 0.0f;

		constexpr float inv255 = 1.0f / 255.0f;

		for (unsigned int y = 0u; y < frameCopy.height(); ++y)
		{
			const uint8_t* data = frameCopy.constrow<uint8_t>(y);

			for (unsigned int x = 0u; x < frameCopy.width(); ++x)
			{
				const float r = float(data[3u * x + 0u]) * inv255;
				const float g = float(data[3u * x + 1u]) * inv255;
				const float b = float(data[3u * x + 2u]) * inv255;

				float colorY, colorU, colorV;
				rgb2yuv(r, g, b, colorY, colorU, colorV);

				ocean_assert(colorY != 0.0f);
				const bool isGray = (NumericF::abs(colorU) + NumericF::abs(colorV)) / colorY < grayThreshold;

				if (colorY > maxY)
				{
					maxY = colorY;
				}

				if (isGray)
				{
					accuU += colorU;
					accuV += colorV;
					grayCount++;
				}
			}
		}

		ocean_assert(grayCount != 0u);
		if (grayCount == 0u)
		{
			break;
		}

		const float grayMeanU = accuU / float(grayCount);
		const float grayMeanV = accuV / float(grayCount);

		float r, g, b;
		yuv2rgb(maxY, grayMeanU, grayMeanV, r, g, b);

		whitePoint.x() = whitePoint.x() * r;
		whitePoint.y() = whitePoint.y() * g;
		whitePoint.z() = whitePoint.z() * b;

		const float scaleSize = NumericF::abs(1.0f - max(r, max(g, b)));

		iteration++;

		if (scaleSize <= 0.0078431372549f || iteration >= 10u) // 2/255
		{
			break;
		}
	}

	return whitePoint;
}

void WhitePointDetector::correctWhitePoint(Frame& frame, const VectorF3& whitePoint, Worker* worker)
{
	ocean_assert(frame && frame.pixelFormat() == FrameType::FORMAT_RGB24 && !whitePoint.isNull());

	const VectorF3 scale(1.0f / whitePoint.x(), 1.0f / whitePoint.y(), 1.0f / whitePoint.z());

	Frame lookupFrame;
	ColorChannelMapper::createLookupScaleOffset(frame.pixelFormat(), lookupFrame, scale, VectorF3(0, 0, 0), VectorF3(1, 1, 1));
	ColorChannelMapper::applyLookup8BitsPerChannel(frame, lookupFrame, worker);
}

unsigned char WhitePointDetector::colorRange(const CV::Histogram::Histogram8BitPerChannel<3u>& histogram, const unsigned char channel, const unsigned int threshold)
{
	ocean_assert(channel < 3u);
	const unsigned int* bins = histogram.bins(channel);

	for (int i = 253; i >= 0; i--)
	{
		if (bins[i] > threshold)
		{
			return (unsigned char)(i);
		}
	}

	return 0u;
}

}

}

}
