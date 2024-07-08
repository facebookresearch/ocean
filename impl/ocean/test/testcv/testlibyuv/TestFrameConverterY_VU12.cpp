/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testlibyuv/TestFrameConverterY_VU12.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/String.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/Worker.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverterY_VU12.h"

#include "ocean/math/Matrix.h"

#include "ocean/test/testcv/TestFrameConverter.h"

#include <libyuv/convert.h>

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestLibyuv
{

void TestFrameConverterY_VU12::test(const double testDuration, const bool skipValidation)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Y_VU12 converter benchmark:   ---";
	Log::info() << " ";

	testConvertY_VU12ToBGRA32(testDuration, skipValidation);

	Log::info() << " ";
	Log::info() << "Y_VU12 benchmark succeeded.";
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameConverterY_VU12, ConvertY_VU12ToBGRA32)
{
	EXPECT_TRUE(TestFrameConverterY_VU12::testConvertY_VU12ToBGRA32(GTEST_TEST_DURATION, false));
}

#endif // OCEAN_USE_GTEST

bool TestFrameConverterY_VU12::testConvertY_VU12ToBGRA32(const double testDuration, const bool skipValidation)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test conversion from full range Y_VU12 to full range BGRA32 BT.601/Android (6 bit precision) - libyuv::NV21ToARGB";
	Log::info() << " ";

	bool allSucceeded = true;

#ifdef OCEAN_USE_GTEST
	const unsigned int widths[] = {1280u, 1920u};
	const unsigned int heights[] = {720u, 1080u};
#else
	const unsigned int widths[] = {100u, 128u, 320u, 640u, 1280u, 1920u, 3840u};
	const unsigned int heights[] = {100u, 128u, 240u, 480u, 720u, 1080u, 2160u};
#endif

	MatrixD colorSpaceTransformationMatrix(4, 4, CV::FrameConverter::transformationMatrix_FullRangeYVU24_To_FullRangeBGR24_Android(), 0, 0);
	colorSpaceTransformationMatrix(3, 3) = 255.0;

	for (unsigned int n = 0u; n < sizeof(widths) / sizeof(widths[0]); ++n)
	{
		const unsigned int width = widths[n];
		const unsigned int height = heights[n];

		Log::info().newLine(n != 0u);
		Log::info().newLine(n != 0u);
		Log::info() << "... for resolution " << width << "x" << height << ":";

		HighPerformanceStatistic performanceOceanSingleCore;
		HighPerformanceStatistic performanceOceanMultiCore;
		HighPerformanceStatistic performanceLibYUV;

		double oceanSumErrorToInteger = 0.0;
		unsigned int oceanMaxErrorToInteger = 0u;
		unsigned long long oceanIterations = 0ull;

		double libyuvSumErrorToInteger = 0.0;
		unsigned int libyuvMaxErrorToInteger = 0u;
		unsigned long long libyuvIterations = 0ull;

		unsigned int iteration = 0u;

		const Timestamp startTimestamp(true);

		do
		{
			Frame::PlaneInitializers<void> sourcePlaneInitializers;
			for (unsigned int nPlane = 0u; nPlane < 2u; ++nPlane)
			{
				sourcePlaneInitializers.emplace_back(RandomI::random(1u, 100u) * RandomI::random(1u));
			}

			Frame y_vuFrame(FrameType(width, height, FrameType::FORMAT_Y_VU12, FrameType::ORIGIN_UPPER_LEFT), sourcePlaneInitializers);
			CV::CVUtilities::randomizeFrame(y_vuFrame, false);

			Frame bgraFrame(FrameType(y_vuFrame, FrameType::FORMAT_BGRA32), Indices32(1, RandomI::random(1u, 100u) * RandomI::random(1u)));
			CV::CVUtilities::randomizeFrame(bgraFrame, false);

			const Frame bgraFrameCopy(bgraFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

			if (iteration % 2u == 0u)
			{
				for (unsigned int workerIteration = 0u; workerIteration < 2u; ++workerIteration)
				{
					const bool useWorker = workerIteration != 0u;

					HighPerformanceStatistic& performance = useWorker ? performanceOceanMultiCore : performanceOceanSingleCore;

					performance.start();
						CV::FrameConverterY_VU12::convertY_VU12FullRangeToBGRA32FullRangeAndroid(y_vuFrame.constdata<uint8_t>(0u), y_vuFrame.constdata<uint8_t>(1u), bgraFrame.data<uint8_t>(), y_vuFrame.width(), y_vuFrame.height(), CV::FrameConverterY_VU12::CONVERT_NORMAL, y_vuFrame.paddingElements(0u), y_vuFrame.paddingElements(1u), bgraFrame.paddingElements(), 0xFFu, WorkerPool::get().conditionalScopedWorker(useWorker)());
					performance.stop();

					double localAverageErrorToInteger = NumericD::maxValue();
					unsigned int localMaximalErrorToInteger = (unsigned int)(-1);

					if (!skipValidation)
					{
						if (!TestFrameConverter::validateConversion(y_vuFrame, bgraFrame, pixelFunctionY_VU12ForYVU24, TestFrameConverter::functionGenericPixel, colorSpaceTransformationMatrix, CV::FrameConverter::CONVERT_NORMAL, nullptr, &localAverageErrorToInteger, nullptr, &localMaximalErrorToInteger, 0.0, 255.0))
						{
							allSucceeded = false;
						}
					}

					oceanSumErrorToInteger += localAverageErrorToInteger;
					oceanMaxErrorToInteger = max(oceanMaxErrorToInteger, localMaximalErrorToInteger);
					oceanIterations++;
				}
			}
			else
			{
				performanceLibYUV.start();
				libyuv::NV21ToARGB(y_vuFrame.constdata<uint8_t>(0u), y_vuFrame.strideBytes(0u), y_vuFrame.constdata<uint8_t>(1u), y_vuFrame.strideBytes(1u), bgraFrame.data<uint8_t>(), bgraFrame.strideBytes(), y_vuFrame.width(), y_vuFrame.height());
				performanceLibYUV.stop();

				double localAverageErrorToInteger = NumericD::maxValue();
				unsigned int localMaximalErrorToInteger = (unsigned int)(-1);

				if (!skipValidation)
				{
					constexpr bool skipPlausibilityCheck = true; // libyuv is quite off, so we need to skip the plausibility check (which ensures that the conversion is within a certain range)

					if (!TestFrameConverter::validateConversion(y_vuFrame, bgraFrame, pixelFunctionY_VU12ForYVU24, TestFrameConverter::functionGenericPixel, colorSpaceTransformationMatrix, CV::FrameConverter::CONVERT_NORMAL, nullptr, &localAverageErrorToInteger, nullptr, &localMaximalErrorToInteger, 0.0, 255.0, skipPlausibilityCheck))
					{
						allSucceeded = false;
					}
				}

				libyuvSumErrorToInteger += localAverageErrorToInteger;
				libyuvMaxErrorToInteger = max(libyuvMaxErrorToInteger, localMaximalErrorToInteger);
				libyuvIterations++;
			}

			if (!CV::CVUtilities::isPaddingMemoryIdentical(bgraFrame, bgraFrameCopy))
			{
				ocean_assert(false && "Invalid padding memory!");
				return false;
			}

			iteration++;
		}
		while (iteration < 2u || startTimestamp + testDuration > Timestamp(true));

		if (libyuvIterations != 0ull)
		{
			Log::info() << "Performance libyuv: [" << String::toAString(performanceLibYUV.bestMseconds(), 3u) << ", " << String::toAString(performanceLibYUV.medianMseconds(), 3u) << ", " << String::toAString(performanceLibYUV.worstMseconds(), 3u) << "] ms, megapixel/s: " << double(width * height) * double(0.000001) / performanceLibYUV.median();

			if (!skipValidation)
			{
				Log::info() << "Validation: average error " << String::toAString(double(libyuvSumErrorToInteger) / double(libyuvIterations), 2u) << ", maximal error: " << libyuvMaxErrorToInteger;
			}

			Log::info() << " ";
		}

		if (oceanIterations != 0ull)
		{
			Log::info() << "Performance Ocean (single-core): [" << String::toAString(performanceOceanSingleCore.bestMseconds(), 3u) << ", " << String::toAString(performanceOceanSingleCore.medianMseconds(), 3u) << ", " << String::toAString(performanceOceanSingleCore.worstMseconds(), 3u) << "] ms, megapixel/s: " << double(width * height) * double(0.000001) / performanceOceanSingleCore.median();
			Log::info() << "Performance Ocean (multi-core): [" << String::toAString(performanceOceanMultiCore.bestMseconds(), 3u) << ", " << String::toAString(performanceOceanMultiCore.medianMseconds(), 3u) << ", " << String::toAString(performanceOceanMultiCore.worstMseconds(), 3u) << "] ms";
			Log::info() << "Multi-core boost factor: [" << String::toAString(performanceOceanSingleCore.best() / performanceOceanMultiCore.best(), 1u) << ", " << String::toAString(performanceOceanSingleCore.median() / performanceOceanMultiCore.median(), 1u) << ", " << String::toAString(performanceOceanSingleCore.worst() / performanceOceanMultiCore.worst(), 1u) << "] x";

			if (!skipValidation)
			{
				Log::info() << "Validation: average error " << String::toAString(double(oceanSumErrorToInteger) / double(oceanIterations), 2u) << ", maximal error: " << oceanMaxErrorToInteger;
			}
		}

		if (oceanIterations != 0ull && libyuvIterations != 0ull)
		{
			Log::info() << " ";
			Log::info() << "Performance factor (single-core): [" << String::toAString(performanceLibYUV.best() / performanceOceanSingleCore.best(), 2u) << ", " << String::toAString(performanceLibYUV.median() / performanceOceanSingleCore.median(), 2u) << ", " << String::toAString(performanceLibYUV.worst() / performanceOceanSingleCore.worst(), 2u) << "] x";
			Log::info() << "Performance factor (multi-core): [" << String::toAString(performanceLibYUV.best() / performanceOceanMultiCore.best(), 2u) << ", " << String::toAString(performanceLibYUV.median() / performanceOceanMultiCore.median(), 2u) << ", " << String::toAString(performanceLibYUV.worst() / performanceOceanMultiCore.worst(), 2u) << "] x";
		}

		if (oceanMaxErrorToInteger > 6u)
		{
			allSucceeded = false;
		}

#if 0 // Libyuv has a wrong implementation averaging 2x2 blocks - due to lack of a correct implementation

		if (libyuvMaxErrorToInteger > 6u)
		{
			allSucceeded = false;
		}

#endif
	}

	Log::info() << " ";

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

MatrixD TestFrameConverterY_VU12::pixelFunctionY_VU12ForYVU24(const Frame& frame, const unsigned int x, const unsigned int y, const CV::FrameConverter::ConversionFlag conversionFlag)
{
	ocean_assert(frame.isValid());
	ocean_assert(x < frame.width() && y < frame.height());
	ocean_assert(conversionFlag == CV::FrameConverter::CONVERT_NORMAL); // we expect the target frame to have a conversion flag

	const unsigned int x_2 = x / 2u;
	const unsigned int y_2 = y / 2u;

	MatrixD colorVector(3, 1);

	colorVector(0, 0) = double(frame.constpixel<uint8_t>(x, y, 0u)[0]);
	colorVector(1, 0) = double(frame.constpixel<uint8_t>(x_2, y_2, 1u)[0]);
	colorVector(2, 0) = double(frame.constpixel<uint8_t>(x_2, y_2, 1u)[1]);

	return colorVector;
}

}

}

}

}
