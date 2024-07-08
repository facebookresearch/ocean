/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testadvanced/TestWhitePointDetector.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/String.h"

#include "ocean/cv/advanced/ColorChannelCurve.h"
#include "ocean/cv/advanced/ColorChannelMapper.h"
#include "ocean/cv/advanced/WhitePointDetector.h"

#include "ocean/math/Numeric.h"
#include "ocean/math/Random.h"

#include <array>

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestAdvanced
{

namespace
{

Scalar delinearizeCIELab(const Scalar t)
{
	constexpr Scalar threshold = Scalar(216) / Scalar(24389);
	constexpr Scalar factor = Scalar(841) / Scalar(108);
	constexpr Scalar offset = Scalar(4) / Scalar(29);
	constexpr Scalar exponent = Scalar(1) / Scalar(3);

	return t > threshold ? Numeric::pow(t, exponent) : factor * t + offset;
}

void sRGBToCIELab(const Scalar& red, const Scalar& green, const Scalar& blue, Scalar& l, Scalar& a, Scalar& b)
{
	const Scalar x = Scalar(0.4124) * red + Scalar(0.3576) * green + Scalar(0.1805) * blue;
	const Scalar y = Scalar(0.2126) * red + Scalar(0.7152) * green + Scalar(0.0722) * blue;
	const Scalar z = Scalar(0.0193) * red + Scalar(0.1192) * green + Scalar(0.9505) * blue;

	//D65 white point:
	constexpr Scalar xn = Scalar(0.95047);
	constexpr Scalar yn = Scalar(1.0);
	constexpr Scalar zn = Scalar(1.08883);

	const Scalar xt = delinearizeCIELab(x / xn);
	const Scalar yt = delinearizeCIELab(y / yn);
	const Scalar zt = delinearizeCIELab(z / zn);

	l = Scalar(116) * yt - Scalar(16);
	a = Scalar(500) * (xt - yt);
	b = Scalar(200) * (yt - zt);
}

float deltaLabE(const VectorF3& color1, const VectorF3& color2)
{
	Scalar l1, a1, b1;
	sRGBToCIELab(color1.x(), color1.y(), color1.z(), l1, a1, b1);

	Scalar l2, a2, b2;
	sRGBToCIELab(color2.x(), color2.y(), color2.z(), l2, a2, b2);

	return VectorF3(float(l1), float(a1), float(b1)).distance(VectorF3(float(l2), float(a2), float(b2)));
}

}

bool TestWhitePointDetector::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   White point detector test:   ---";
	Log::info() << " ";

	// We use a mix of 50% white and 50% random pixels as image content:
	Frame rgbFrame(FrameType(width, height, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
	randomizeWhitePointFrame(rgbFrame, 0.5f);

	bool allSucceeded = true;

	Scalar l, a, b;
	sRGBToCIELab(1.0, 1.0, 1.0, l, a, b);

	const std::array<float, 4> whitePointsR = {1.0f, 0.9f, 0.9f, 1.0f};
	const std::array<float, 4> whitePointsG = {1.0f, 0.9f, 0.9f, 0.9f};
	const std::array<float, 4> whitePointsB = {1.0f, 0.9f, 1.0f, 0.9f};

	for (size_t i = 0; i < whitePointsR.size(); ++i)
	{
		Log::info().newLine(i != 0u);
		Log::info().newLine(i != 0u);

		Frame inputFrame(rgbFrame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

		const VectorF3 whitePoint(whitePointsR[i], whitePointsG[i], whitePointsB[i]);
		CV::Advanced::ColorChannelMapper::mapScaleOffset(inputFrame, whitePoint, VectorF3(0, 0, 0), VectorF3(1, 1, 1), &worker);

		allSucceeded = testWhitepointDetectorHistogram(inputFrame, whitePoint, testDuration, worker) && allSucceeded;

		Log::info() << " ";

		allSucceeded = testWhitepointDetectorGrayPoints(inputFrame, whitePoint, testDuration, worker) && allSucceeded;
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "White point detector test succeeded.";
	}
	else
	{
		Log::info() << "White point detector test FAILED!";
	}

	return allSucceeded;
}

bool TestWhitePointDetector::testWhitepointDetectorHistogram(const Frame& frame, const VectorF3& whitePoint, const double testDuration, Worker& worker)
{
	ocean_assert(frame && frame.pixelFormat() == FrameType::FORMAT_RGB24);

	Log::info() << "Testing histogram-based white point detection (" << String::toAString(whitePoint.x(), 1u) << ", " << String::toAString(whitePoint.y(), 1u) << ", " << String::toAString(whitePoint.z(), 1u) << "):";

	bool allSucceeded = true;

	VectorF3 detectedWhitePoint;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = workerIteration == 0u ? nullptr : &worker;
		HighPerformanceStatistic& performance = workerIteration == 0u ? performanceSinglecore : performanceMulticore;

		const Timestamp startTimestamp(true);

		do
		{
			const HighPerformanceStatistic::ScopedStatistic scopedStaistic(performance);

			detectedWhitePoint = CV::Advanced::WhitePointDetector::detectWhitePointHistogram(frame, useWorker);
		}
		while (startTimestamp + testDuration > Timestamp(true));

		if (!validateWhitePoint(whitePoint, detectedWhitePoint))
		{
			allSucceeded = false;
		}
	}

	Log::info() << "Performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 2u) << "ms";

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multi-core performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 2u) << "ms";
		Log::info() << "Multi-core boost: Best: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x";
	}

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestWhitePointDetector::testWhitepointDetectorGrayPoints(const Frame& frame, const VectorF3& whitePoint, const double testDuration, Worker& worker)
{
	ocean_assert(frame && frame.pixelFormat() == FrameType::FORMAT_RGB24);

	Log::info() << "Testing gray points-based white point detection (" << String::toAString(whitePoint.x(), 1u) << ", " << String::toAString(whitePoint.y(), 1u) << ", " << String::toAString(whitePoint.z(), 1u) << "):";

	bool allSucceeded = true;

	VectorF3 detectedWhitePoint;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = workerIteration == 0u ? nullptr : &worker;
		HighPerformanceStatistic& performance = workerIteration == 0u ? performanceSinglecore : performanceMulticore;

		const Timestamp startTimestamp(true);

		do
		{
			const HighPerformanceStatistic::ScopedStatistic scopedStaistic(performance);

			detectedWhitePoint = CV::Advanced::WhitePointDetector::detectWhitePointHistogram(frame, useWorker);
		}
		while (startTimestamp + testDuration > Timestamp(true));

		if (!validateWhitePoint(whitePoint, detectedWhitePoint))
		{
			allSucceeded = false;
		}
	}

	Log::info() << "Performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 2u) << "ms";

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multi-core performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 2u) << "ms";
		Log::info() << "Multi-core boost: Best: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x";
	}

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestWhitePointDetector::validateWhitePoint(const VectorF3& idealWhitePoint, const VectorF3& detectedWhitePoint)
{
	const Scalar e = Scalar(deltaLabE(idealWhitePoint, detectedWhitePoint));
	return e <= Scalar(4);
}

void TestWhitePointDetector::randomizeWhitePointFrame(Frame& frame, const float randomAmount)
{
	ocean_assert(frame.isValid() && frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8 && frame.numberPlanes() == 1u);
	ocean_assert(randomAmount >= 0.0f && randomAmount <= 1.0f);

	const unsigned int noiseScale = (unsigned int)(minmax(0.0f, randomAmount * 255.0f, 255.0f));
	const unsigned int constScale = 255u - noiseScale;

	for (unsigned int y = 0u; y < frame.height(); ++y)
	{
		uint8_t* row = frame.row<uint8_t>(y);

		for (unsigned int x = 0u; x < frame.planeWidthBytes(0u); ++x)
		{
			*row++ = uint8_t(Random::random(noiseScale) + constScale);
		}
	}
}

bool TestWhitePointDetector::validateWhitePointFrame(const Frame& idealFrame, const Frame& correctedFrame)
{
	ocean_assert(idealFrame && idealFrame.pixelFormat() == FrameType::FORMAT_RGB24);
	ocean_assert(correctedFrame && correctedFrame.pixelFormat() == FrameType::FORMAT_RGB24);

	if (idealFrame.frameType() != correctedFrame.frameType())
	{
		return false;
	}

	for (unsigned int y = 0u; y < idealFrame.height(); ++y)
	{
		const uint8_t* data = correctedFrame.constrow<uint8_t>(y);
		const uint8_t* dataIdeal = idealFrame.constrow<uint8_t>(y);

		for (unsigned int x = 0u; x < idealFrame.width(); ++x)
		{
			const VectorF3 corrected = VectorF3(float(data[0]), float(data[1]), float(data[0])) / 255.0f;
			const VectorF3 ideal = VectorF3(float(dataIdeal[0]), float(dataIdeal[1]), float(dataIdeal[0])) / 255.0f;

			if (!validateWhitePoint(ideal, corrected))
			{
				return false;
			}

			dataIdeal += 3;
			data += 3;
		}
	}

	return true;
}

}

}

}

}
