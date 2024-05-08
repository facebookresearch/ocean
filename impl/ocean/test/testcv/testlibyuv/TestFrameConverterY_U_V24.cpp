/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testlibyuv/TestFrameConverterY_U_V24.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/String.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/Worker.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverterY_U_V24.h"

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

void TestFrameConverterY_U_V24::test(const double testDuration, const bool skipValidation)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Y_U_V24 converter benchmark:   ---";
	Log::info() << " ";

	testConvertY_U_V24ToBGRA32Precision6Bit(testDuration, skipValidation);

	Log::info() << " ";
	Log::info() << "Y_U_V24 benchmark succeeded.";
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameConverterY_U_V24, ConvertY_U_V24ToBGRA32Precision6Bit)
{
	EXPECT_TRUE(TestFrameConverterY_U_V24::testConvertY_U_V24ToBGRA32Precision6Bit(GTEST_TEST_DURATION, false));
}

#endif // OCEAN_USE_GTEST

bool TestFrameConverterY_U_V24::testConvertY_U_V24ToBGRA32Precision6Bit(const double testDuration, const bool skipValidation)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test conversion from full range Y_U_V24 to full range BGRA32 BT.601/Android (6 bit precision) - libyuv::J444ToARGB";
	Log::info() << " ";

	bool allSucceeded = true;

#ifdef OCEAN_USE_GTEST
	const unsigned int widths[] = {1280u, 1920u};
	const unsigned int heights[] = {720u, 1080u};
#else
	const unsigned int widths[] = {100u, 320u, 640u, 1280u, 1920u, 3840u};
	const unsigned int heights[] = {100u, 240u, 480u, 720u, 1080u, 2160u};
#endif

	MatrixD colorSpaceTransformationMatrix(4, 4, CV::FrameConverter::transformationMatrix_FullRangeYUV24_To_FullRangeBGR24_Android(), 0, 0);
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
			for (unsigned int nPlane = 0u; nPlane < 3u; ++nPlane)
			{
				sourcePlaneInitializers.emplace_back(RandomI::random(1u, 100u) * RandomI::random(1u));
			}

			Frame y_u_vFrame(FrameType(width, height, FrameType::FORMAT_Y_U_V24, FrameType::ORIGIN_UPPER_LEFT), sourcePlaneInitializers);

			// libyuv needs Y_U_V24 images with identical 2x2 areas in u-plane and v-plane
			specialRandomizeFrame(y_u_vFrame);

			Frame bgraFrame(FrameType(y_u_vFrame, FrameType::FORMAT_BGRA32), Indices32(1, RandomI::random(1u, 100u) * RandomI::random(1u)));
			CV::CVUtilities::randomizeFrame(bgraFrame, false);

			const Frame bgraFrameCopy(bgraFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

			if (iteration % 2u == 0u)
			{
				for (unsigned int workerIteration = 0u; workerIteration < 2u; ++workerIteration)
				{
					const bool useWorker = workerIteration != 0u;

					HighPerformanceStatistic& performnace = useWorker ? performanceOceanMultiCore : performanceOceanSingleCore;

					performnace.start();
					CV::FrameConverterY_U_V24::convertY_U_V24FullRangeToBGRA32FullRangeAndroid(y_u_vFrame.constdata<uint8_t>(0u), y_u_vFrame.constdata<uint8_t>(1u), y_u_vFrame.constdata<uint8_t>(2u), bgraFrame.data<uint8_t>(), y_u_vFrame.width(), y_u_vFrame.height(), CV::FrameConverterY_U_V24::CONVERT_NORMAL, y_u_vFrame.paddingElements(0u), y_u_vFrame.paddingElements(1u), y_u_vFrame.paddingElements(2u), bgraFrame.paddingElements(), 0xFFu, WorkerPool::get().conditionalScopedWorker(useWorker)());
					performnace.stop();

					double localAverageErrorToInteger = NumericD::maxValue();
					unsigned int localMaximalErrorToInteger = (unsigned int)(-1);

					if (!skipValidation)
					{
						if (!TestFrameConverter::validateConversion(y_u_vFrame, bgraFrame, pixelFunctionY_U_V24ForYUV24, TestFrameConverter::functionGenericPixel, colorSpaceTransformationMatrix, CV::FrameConverter::CONVERT_NORMAL, nullptr, &localAverageErrorToInteger, nullptr, &localMaximalErrorToInteger, 0.0, 255.0))
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
				libyuv::J444ToARGB(y_u_vFrame.constdata<uint8_t>(0u), y_u_vFrame.strideBytes(0u), y_u_vFrame.constdata<uint8_t>(1u), y_u_vFrame.strideBytes(1u), y_u_vFrame.constdata<uint8_t>(2u), y_u_vFrame.strideBytes(2u), bgraFrame.data<uint8_t>(), bgraFrame.strideBytes(), y_u_vFrame.width(), y_u_vFrame.height());
				performanceLibYUV.stop();

				double localAverageErrorToInteger = NumericD::maxValue();
				unsigned int localMaximalErrorToInteger = (unsigned int)(-1);

				if (!skipValidation)
				{
					if (!TestFrameConverter::validateConversion(y_u_vFrame, bgraFrame, pixelFunctionY_U_V24ForYUV24, TestFrameConverter::functionGenericPixel, colorSpaceTransformationMatrix, CV::FrameConverter::CONVERT_NORMAL, nullptr, &localAverageErrorToInteger, nullptr, &localMaximalErrorToInteger, 0.0, 255.0))
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
			Log::info() << "Performance libyuv: [" << String::toAString(performanceLibYUV.bestMseconds(), 3u) << ", " << String::toAString(performanceLibYUV.medianMseconds(), 3u) << ", " << String::toAString(performanceLibYUV.worstMseconds(), 3u) << "] ms";

			if (!skipValidation)
			{
				Log::info() << "Validation: average error " << String::toAString(double(libyuvSumErrorToInteger) / double(libyuvIterations), 2u) << ", maximal error: " << libyuvMaxErrorToInteger;
			}

			Log::info() << " ";
		}

		if (oceanIterations != 0ull)
		{
			Log::info() << "Performance Ocean (single-core): [" << String::toAString(performanceOceanSingleCore.bestMseconds(), 3u) << ", " << String::toAString(performanceOceanSingleCore.medianMseconds(), 3u) << ", " << String::toAString(performanceOceanSingleCore.worstMseconds(), 3u) << "] ms";
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

		if (libyuvMaxErrorToInteger > 2u)
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

void TestFrameConverterY_U_V24::specialRandomizeFrame(Frame& frame)
{
	ocean_assert(frame && frame.pixelFormat() == FrameType::FORMAT_Y_U_V24);
	ocean_assert(frame.width() >= 2u && frame.width() % 2u == 0u);
	ocean_assert(frame.height() >= 2u && frame.height() % 2u == 0u);

	CV::CVUtilities::randomizeFrame(frame, false);

	const unsigned int width = frame.width();
	const unsigned int height = frame.height();

	for (unsigned int y = 0u; y < height; y += 2u)
	{
		uint8_t* const uRow = frame.row<uint8_t>(y, 1u);
		uint8_t* const uRowBelow = frame.row<uint8_t>(y + 1u, 1u);

		for (unsigned int x = 0u; x < width; x += 2u)
		{
			const uint8_t value = uRow[x + 0u];

			uRow[x + 1u] = value;
			uRowBelow[x + 0u] = value;
			uRowBelow[x + 1u] = value;
		}
	}

	for (unsigned int y = 0u; y < height; y += 2u)
	{
		uint8_t* const vRow = frame.row<uint8_t>(y, 2u);
		uint8_t* const vRowBelow = frame.row<uint8_t>(y + 1u, 2u);

		for (unsigned int x = 0u; x < width; x += 2u)
		{
			const uint8_t value = vRow[x + 0u];

			vRow[x + 1u] = value;
			vRowBelow[x + 0u] = value;
			vRowBelow[x + 1u] = value;
		}
	}
}

MatrixD TestFrameConverterY_U_V24::pixelFunctionY_U_V24ForYUV24(const Frame& frame, const unsigned int x, const unsigned int y, const CV::FrameConverter::ConversionFlag conversionFlag)
{
	ocean_assert(frame.isValid());
	ocean_assert(x < frame.width() && y < frame.height());
	ocean_assert(conversionFlag == CV::FrameConverter::CONVERT_NORMAL); // we expect the target frame to have a conversion flag

	MatrixD colorVector(3, 1);

	colorVector(0, 0) = double(*frame.constpixel<uint8_t>(x, y, 0u));
	colorVector(1, 0) = double(*frame.constpixel<uint8_t>(x, y, 1u));
	colorVector(2, 0) = double(*frame.constpixel<uint8_t>(x, y, 2u));

	return colorVector;
}

}

}

}

}
