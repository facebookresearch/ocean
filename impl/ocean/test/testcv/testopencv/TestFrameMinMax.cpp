/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testopencv/TestFrameMinMax.h"

#include "ocean/base/DataType.h"
#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameMinMax.h"
#include "ocean/cv/OpenCVUtilities.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestOpenCV
{

void TestFrameMinMax::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Frame min-max test:   ---";
	Log::info() << " ";

	testMinLocation(testDuration);

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	testMaxLocation(testDuration);

	Log::info() << " ";

	Log::info() << "Frame converter test finished.";
}

void TestFrameMinMax::testMinLocation(const double testDuration)
{
	Log::info() << "Testing min location:";
	Log::info() << " ";

	const unsigned int widths[] = {160u, 320u, 640u, 800u, 1280u, 1920u, 3840u};
	const unsigned int heights[] = {120u, 240u, 480u, 640u, 720u, 1080u, 2160u};

	for (unsigned int n = 0u; n < sizeof(widths) / sizeof(widths[0]); ++n)
	{
		Log::info().newLine(n != 0u);

		const unsigned int width = widths[n];
		const unsigned int height = heights[n];

		Log::info() << "For image resolution " << width << "x" << height << ":";
		Log::info() << " ";

		testMinLocation<unsigned char>(width, height, testDuration);
		Log::info() << " ";

		testMinLocation<int>(width, height, testDuration);
		Log::info() << " ";

		testMinLocation<float>(width, height, testDuration);
		Log::info() << " ";
	}

	testMinLocation<float>(1920u, 1080u, testDuration);

	Log::info() << "Min location test finished.";
}

void TestFrameMinMax::testMaxLocation(const double testDuration)
{
	Log::info() << "Testing max location:";
	Log::info() << " ";

	const unsigned int widths[] = {160u, 320u, 640u, 800u, 1280u, 1920u, 3840u};
	const unsigned int heights[] = {120u, 240u, 480u, 640u, 720u, 1080u, 2160u};

	for (unsigned int n = 0u; n < sizeof(widths) / sizeof(widths[0]); ++n)
	{
		Log::info().newLine(n != 0u);

		const unsigned int width = widths[n];
		const unsigned int height = heights[n];

		Log::info() << "For image resolution " << width << "x" << height << ":";
		Log::info() << " ";

		testMaxLocation<unsigned char>(width, height, testDuration);
		Log::info() << " ";

		testMaxLocation<int>(width, height, testDuration);
		Log::info() << " ";

		testMaxLocation<float>(width, height, testDuration);
		Log::info() << " ";
	}

	Log::info() << "Max location test finished.";
}

template <typename T>
void TestFrameMinMax::testMinLocation(const unsigned int width, const unsigned int height, const double testDuration)
{
	ocean_assert(width >= 2u && height >= 2u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for '" << TypeNamer::name<T>() << "' :";

	cv::Mat cvFrame;

	unsigned int iteration = 0u;

	HighPerformanceStatistic performanceOcean;
	HighPerformanceStatistic performanceOpenCV;

	// we use the following dummy locations to ensure that the compiler does not apply unintended optimizations
	T dummyOceanValue = T(0);
	unsigned int dummyOceanLocationX = 0u;
	unsigned int dummyOceanLocationY = 0u;

	T dummyOpenCVValue = T(0);
	int dummyOpenCVLocationX = 0;
	int dummyOpenCVLocationY = 0;

	const Timestamp startTimestamp(true);

	do
	{
		Frame frame(FrameType(width, height, FrameType::genericPixelFormat<T, 1u>(), FrameType::ORIGIN_UPPER_LEFT));
		CV::CVUtilities::randomizeFrame(frame);

		if (iteration % 2u == 0u)
		{
			T minValue = NumericT<T>::maxValue();
			CV::PixelPosition minLocation((unsigned int)(-1), (unsigned int)(-1));

			performanceOcean.start();
			CV::FrameMinMax::determineMinValue(frame.constdata<T>(), frame.width(), frame.height(), frame.paddingElements(), &minValue, &minLocation);
			performanceOcean.stop();

			dummyOceanValue += minValue;
			dummyOceanLocationX += minLocation.x();
			dummyOceanLocationY += minLocation.y();
		}
		else
		{
			CV::OpenCVUtilities::toCvMat(frame).copyTo(cvFrame);

			double minValue = NumericD::maxValue();
			cv::Point minLocation;

			performanceOpenCV.start();
			cv::minMaxLoc(cvFrame, &minValue, nullptr, &minLocation, nullptr);
			performanceOpenCV.stop();

			dummyOpenCVValue += T(minValue);
			dummyOpenCVLocationX += minLocation.x;
			dummyOpenCVLocationY += minLocation.y;
		}

		iteration++;
	}
	while (iteration < 2u || startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance OpenCV: [" << String::toAString(performanceOpenCV.bestMseconds(), 3u) << ", " << String::toAString(performanceOpenCV.medianMseconds(), 3u) << ", " << String::toAString(performanceOpenCV.worstMseconds(), 3u) << "] ms";
	Log::info() << "Performance Ocean: [" << String::toAString(performanceOcean.bestMseconds(), 3u) << ", " << String::toAString(performanceOcean.medianMseconds(), 3u) << ", " << String::toAString(performanceOcean.worstMseconds(), 3u) << "] ms";

	if (dummyOceanValue > T(5) && dummyOceanLocationX > 5u && dummyOceanLocationY > 5u && dummyOpenCVValue > T(5) && dummyOpenCVLocationX > 5 && dummyOpenCVLocationY > 5)
	{
		Log::info() << "Ocean vs. OpenCV:";
	}
	else
	{
		Log::info() << "Ocean vs. OpenCV:";
	}

	Log::info() << "Performance ratio: [" << String::toAString(performanceOpenCV.best() / performanceOcean.best(), 2u) << ", " << String::toAString(performanceOpenCV.median() / performanceOcean.median(), 2u) << ", " << String::toAString(performanceOpenCV.worst() / performanceOcean.worst(), 2u) << "] x";
}

template <typename T>
void TestFrameMinMax::testMaxLocation(const unsigned int width, const unsigned int height, const double testDuration)
{
	ocean_assert(width >= 2u && height >= 2u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for '" << TypeNamer::name<T>() << "' :";

	Frame frame(FrameType(width, height, FrameType::genericPixelFormat<T, 1u>(), FrameType::ORIGIN_UPPER_LEFT));

	cv::Mat cvFrame;

	unsigned int iteration = 0u;

	HighPerformanceStatistic performanceOcean;
	HighPerformanceStatistic performanceOpenCV;

	// we use the following dummy locations to ensure that the compiler does not apply unintended optimizations
	T dummyOceanValue = T(0);
	unsigned int dummyOceanLocationX = 0u;
	unsigned int dummyOceanLocationY = 0u;

	T dummyOpenCVValue = T(0);
	int dummyOpenCVLocationX = 0;
	int dummyOpenCVLocationY = 0;

	const Timestamp startTimestamp(true);

	do
	{
		CV::CVUtilities::randomizeFrame(frame);

		if (iteration % 2u == 0u)
		{
			T maxValue = NumericT<T>::minValue();
			CV::PixelPosition maxLocation((unsigned int)(-1), (unsigned int)(-1));

			performanceOcean.start();
			CV::FrameMinMax::determineMaxValue(frame.constdata<T>(), frame.width(), frame.height(), frame.paddingElements(), &maxValue, &maxLocation);
			performanceOcean.stop();

			dummyOceanValue += maxValue;
			dummyOceanLocationX += maxLocation.x();
			dummyOceanLocationY += maxLocation.y();
		}
		else
		{
			CV::OpenCVUtilities::toCvMat(frame).copyTo(cvFrame);

			double maxValue = NumericD::maxValue();
			cv::Point maxLocation;

			performanceOpenCV.start();
			cv::minMaxLoc(cvFrame, nullptr, &maxValue, nullptr, &maxLocation);
			performanceOpenCV.stop();

			dummyOpenCVValue += T(maxValue);
			dummyOpenCVLocationX += maxLocation.x;
			dummyOpenCVLocationY += maxLocation.y;
		}

		iteration++;
	}
	while (iteration < 2u || startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance OpenCV: [" << String::toAString(performanceOpenCV.bestMseconds(), 3u) << ", " << String::toAString(performanceOpenCV.medianMseconds(), 3u) << ", " << String::toAString(performanceOpenCV.worstMseconds(), 3u) << "] ms";
	Log::info() << "Performance Ocean: [" << String::toAString(performanceOcean.bestMseconds(), 3u) << ", " << String::toAString(performanceOcean.medianMseconds(), 3u) << ", " << String::toAString(performanceOcean.worstMseconds(), 3u) << "] ms";

	if (dummyOceanValue > T(5) && dummyOceanLocationX > 5u && dummyOceanLocationY > 5u && dummyOpenCVValue > T(5) && dummyOpenCVLocationX > 5 && dummyOpenCVLocationY > 5)
	{
		Log::info() << "Ocean vs. OpenCV:";
	}
	else
	{
		Log::info() << "Ocean vs. OpenCV:";
	}

	Log::info() << "Performance ratio: [" << String::toAString(performanceOpenCV.best() / performanceOcean.best(), 2u) << ", " << String::toAString(performanceOpenCV.median() / performanceOcean.median(), 2u) << ", " << String::toAString(performanceOpenCV.worst() / performanceOcean.worst(), 2u) << "] x";
}

}

}

}

}
