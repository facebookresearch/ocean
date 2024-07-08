/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testlibyuv/TestFrameConverterBGRA32.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/String.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/Worker.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverterBGRA32.h"

#include "ocean/math/Matrix.h"

#include "ocean/test/testcv/TestFrameConverter.h"

#include <libyuv/convert.h>

#ifdef OCEAN_TEST_CV_LIBYUV_HAS_EXTRAS
	#include <libyuv_extras/libyuv_extras.h>
#endif

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestLibyuv
{

void TestFrameConverterBGRA32::test(const double testDuration, const bool skipValidation)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   BGRA32 converter benchmark:   ---";
	Log::info() << " ";

#ifdef OCEAN_TEST_CV_LIBYUV_HAS_EXTRAS
	testConvertBGRA32ToY8(testDuration, skipValidation);
#else
	Log::info() << "Skiping as `OCEAN_TEST_CV_LIBYUV_HAS_EXTRAS` is not defined";
#endif

	Log::info() << " ";
	Log::info() << "BGRA32 benchmark succeeded.";
}

#ifdef OCEAN_USE_GTEST

#ifdef OCEAN_TEST_CV_LIBYUV_HAS_EXTRAS

TEST(TestFrameConverterBGRA32, ConvertBGRA32ToY8)
{
	EXPECT_TRUE(TestFrameConverterBGRA32::testConvertBGRA32ToY8(GTEST_TEST_DURATION, false));
}

#endif // OCEAN_TEST_CV_LIBYUV_HAS_EXTRAS

#endif // OCEAN_USE_GTEST

bool TestFrameConverterBGRA32::testConvertBGRA32ToY8(const double testDuration, const bool skipValidation)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test conversion from BGRA32 to Y8:";
	Log::info() << " ";

#ifdef OCEAN_TEST_CV_LIBYUV_HAS_EXTRAS

	bool allSucceeded = true;

#ifdef OCEAN_USE_GTEST
	const unsigned int widths[] = {1280u, 1919u, 1920u};
	const unsigned int heights[] = {720u, 1080u, 1080u};
#else
	const unsigned int widths[] = {100u, 320u, 640u, 1280u, 1919u, 1920u, 3840u};
	const unsigned int heights[] = {100u, 240u, 480u, 720u, 1080u, 1080u, 2160u};
#endif

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
			Frame frameBGRA32(FrameType(width, height, FrameType::FORMAT_BGRA32, FrameType::ORIGIN_UPPER_LEFT), Indices32(1, RandomI::random(1u, 100u) * RandomI::random(1u)));
			CV::CVUtilities::randomizeFrame(frameBGRA32, false);

			Frame frameY8(FrameType(frameBGRA32, FrameType::FORMAT_Y8), Indices32(1, RandomI::random(1u, 100u) * RandomI::random(1u)));
			CV::CVUtilities::randomizeFrame(frameY8, false);

			const Frame copyFrameY8(frameY8, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

			if (iteration % 2u == 0u)
			{
				MatrixD colorSpaceTransformationMatrix(1, 4, false);
				colorSpaceTransformationMatrix(0, 0) = 0.114;
				colorSpaceTransformationMatrix(0, 1) = 0.587;
				colorSpaceTransformationMatrix(0, 2) = 0.299;

				for (unsigned int workerIteration = 0u; workerIteration < 2u; ++workerIteration)
				{
					const bool useWorker = workerIteration != 0u;

					HighPerformanceStatistic& performnace = useWorker ? performanceOceanMultiCore : performanceOceanSingleCore;

					performnace.start();
					CV::FrameConverterBGRA32::convertBGRA32ToY8(frameBGRA32.constdata<uint8_t>(), frameY8.data<uint8_t>(), frameBGRA32.width(), frameBGRA32.height(), CV::FrameConverterBGRA32::CONVERT_NORMAL, frameBGRA32.paddingElements(), frameY8.paddingElements(), WorkerPool::get().conditionalScopedWorker(useWorker)());
					performnace.stop();

					double localAverageErrorToInteger = NumericD::maxValue();
					unsigned int localMaximalErrorToInteger = (unsigned int)(-1);

					if (!skipValidation)
					{
						if (!TestFrameConverter::validateConversion(frameBGRA32, frameY8, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, colorSpaceTransformationMatrix, CV::FrameConverter::CONVERT_NORMAL, nullptr, &localAverageErrorToInteger, nullptr, &localMaximalErrorToInteger, 0.0, 255.0))
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
				MatrixD colorSpaceTransformationMatrix(1, 5, false);
				colorSpaceTransformationMatrix(0, 0) = 0.0;
				colorSpaceTransformationMatrix(0, 1) = 0.2578f;
				colorSpaceTransformationMatrix(0, 2) = 0.5078f;
				colorSpaceTransformationMatrix(0, 3) = 0.1016f;
				colorSpaceTransformationMatrix(0, 4) = 16.0;

				performanceLibYUV.start();
				facebook::libyuv::BGRAToY(frameBGRA32.constdata<uint8_t>(), frameBGRA32.strideBytes(), frameY8.data<uint8_t>(), frameY8.strideBytes(), frameBGRA32.width(), frameBGRA32.height());
				performanceLibYUV.stop();

				double localAverageErrorToInteger = NumericD::maxValue();
				unsigned int localMaximalErrorToInteger = (unsigned int)(-1);

				if (!skipValidation)
				{
					if (!TestFrameConverter::validateConversion(frameBGRA32, frameY8, TestFrameConverter::functionGenericPixel, TestFrameConverter::functionGenericPixel, colorSpaceTransformationMatrix, CV::FrameConverter::CONVERT_NORMAL, nullptr, &localAverageErrorToInteger, nullptr, &localMaximalErrorToInteger, 0.0, 255.0))
					{
						allSucceeded = false;
					}
				}

				libyuvSumErrorToInteger += localAverageErrorToInteger;
				libyuvMaxErrorToInteger = max(libyuvMaxErrorToInteger, localMaximalErrorToInteger);
				libyuvIterations++;
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

		if (libyuvMaxErrorToInteger > 2u || oceanMaxErrorToInteger > 2u)
		{
			allSucceeded = false;
		}
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

#else // OCEAN_TEST_CV_LIBYUV_HAS_EXTRAS

	ocean_assert(false && "Not available!");
	return false;

#endif // OCEAN_TEST_CV_LIBYUV_HAS_EXTRAS
}

}

}

}

}
