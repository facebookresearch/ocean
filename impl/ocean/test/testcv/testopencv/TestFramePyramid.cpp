/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testopencv/TestFramePyramid.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/String.h"
#include "ocean/base/Timestamp.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FramePyramid.h"
#include "ocean/cv/FrameShrinker.h"
#include "ocean/cv/OpenCVUtilities.h"

#include <opencv2/imgproc.hpp>

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestOpenCV
{

void TestFramePyramid::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Frame pyramid test:   ---";
	Log::info() << " ";

	testFrameDownsampling(testDuration);

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	testFramePyramid(testDuration);

	Log::info() << " ";

	Log::info() << "Frame pyramid test finished.";
}

void TestFramePyramid::testFrameDownsampling(const double testDuration)
{
	Log::info() << "Testing frame downsampling by two:";
	Log::info() << " ";

	const Indices32 widths = {160u, 320u, 640u, 800u, 1280u, 1920u, 3840u};
	const Indices32 heights = {120u, 240u, 480u, 640u,  720u, 1080u, 2160u};

	for (unsigned int n = 0u; n < widths.size(); ++n)
	{
		Log::info().newLine(n != 0u);

		const unsigned int width = widths[n];
		const unsigned int height = heights[n];

		Log::info() << "For image resolution " << width << "x" << height << ":";
		Log::info() << " ";

		for (unsigned int channels = 1u; channels <= 4u; ++channels)
		{
			testFrameDownsampling(width, height, channels, testDuration);
			Log::info() << " ";
		}
	}

	Log::info() << "Frame pyramid creation test finished.";
}

void TestFramePyramid::testFramePyramid(const double testDuration)
{
	Log::info() << "Testing creation of frame pyramid:";
	Log::info() << " ";

	const Indices32 widths = {640u, 800u, 1280u, 1920u, 3840u};
	const Indices32 heights = {480u, 640u,  720u, 1080u, 2160u};

	for (unsigned int n = 0u; n < widths.size(); ++n)
	{
		Log::info().newLine(n != 0u);

		const unsigned int width = widths[n];
		const unsigned int height = heights[n];

		Log::info() << "For image resolution " << width << "x" << height << ":";
		Log::info() << " ";

		for (unsigned int channels = 1u; channels <= 4u; ++channels)
		{
			testFramePyramid(width, height, channels, testDuration);
			Log::info() << " ";
		}
	}

	Log::info() << "Frame pyramid creation test finished.";
}

void TestFramePyramid::testFrameDownsampling(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration)
{
	ocean_assert(width >= 2u && height >= 2u);
	ocean_assert(channels >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... with " << channels << " channels:";
	Log::info() << " ";

	RandomGenerator randomGenerator;

	double oceanAverageAbsError14641 = 0.0;
	double oceanMaximalAbsError14641 = 0.0;

	double opencvAverageAbsError14641 = 0.0;
	double opencvMaximalAbsError14641 = 0.0;

	unsigned int iteration = 0u;

	HighPerformanceStatistic performanceOcean11;
	HighPerformanceStatistic performanceOcean14641;
	HighPerformanceStatistic performanceOpenCV14641;

	const Timestamp startTimestamp(true);

	do
	{
		Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat<uint8_t>(channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
		Frame targetFrame = CV::CVUtilities::randomizedFrame(FrameType(sourceFrame, width / 2u, height / 2u), &randomGenerator);

		cv::Mat cvSourceFrame;
		cv::Mat cvTargetFrame;

		if (iteration % 3u == 0u)
		{
			performanceOcean11.start();
				CV::FrameShrinker::downsampleByTwo11(sourceFrame, targetFrame);
			performanceOcean11.stop();
		}
		else if (iteration % 3u == 1u)
		{
			performanceOcean14641.start();
				CV::FrameShrinker::downsampleByTwo14641(sourceFrame, targetFrame);
			performanceOcean14641.stop();

			double averageAbsError = NumericD::maxValue();
			double maximalAbsError = NumericD::maxValue();
			validateDownsamplingByTwo8Bit14641(sourceFrame.constdata<uint8_t>(), targetFrame.constdata<uint8_t>(), sourceFrame.width(), sourceFrame.height(), targetFrame.width(), targetFrame.height(), sourceFrame.channels(), sourceFrame.paddingElements(), targetFrame.paddingElements(), &averageAbsError, &maximalAbsError);

			oceanAverageAbsError14641 += averageAbsError;
			oceanMaximalAbsError14641 = std::max(oceanMaximalAbsError14641, maximalAbsError);
		}
		else
		{
			CV::OpenCVUtilities::toCvMat(sourceFrame).copyTo(cvSourceFrame);
			CV::OpenCVUtilities::toCvMat(targetFrame).copyTo(cvTargetFrame);

			performanceOpenCV14641.start();
				cv::pyrDown(cvSourceFrame, cvTargetFrame, cv::Size(cvSourceFrame.cols / 2, cvSourceFrame.rows / 2));
			performanceOpenCV14641.stop();

			sourceFrame = CV::OpenCVUtilities::toOceanFrame(cvSourceFrame);
			targetFrame = CV::OpenCVUtilities::toOceanFrame(cvTargetFrame);

			double averageAbsError = NumericD::maxValue();
			double maximalAbsError = NumericD::maxValue();
			validateDownsamplingByTwo8Bit14641(sourceFrame.constdata<uint8_t>(), targetFrame.constdata<uint8_t>(), sourceFrame.width(), sourceFrame.height(), targetFrame.width(), targetFrame.height(), sourceFrame.channels(), sourceFrame.paddingElements(), targetFrame.paddingElements(), &averageAbsError, &maximalAbsError);

			opencvAverageAbsError14641 += averageAbsError;
			opencvMaximalAbsError14641 = std::max(opencvMaximalAbsError14641, maximalAbsError);
		}

		++iteration;
	}
	while (iteration < 4u || startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance OpenCV (14641): [" << String::toAString(performanceOpenCV14641.bestMseconds(), 3u) << ", " << String::toAString(performanceOpenCV14641.medianMseconds(), 3u) << ", " << String::toAString(performanceOpenCV14641.worstMseconds(), 3u) << "] ms";
	Log::info() << "Performance Ocean (14641): [" << String::toAString(performanceOcean14641.bestMseconds(), 3u) << ", " << String::toAString(performanceOcean14641.medianMseconds(), 3u) << ", " << String::toAString(performanceOcean14641.worstMseconds(), 3u) << "] ms";
	Log::info() << "Performance Ocean (11): [" << String::toAString(performanceOcean11.bestMseconds(), 3u) << ", " << String::toAString(performanceOcean11.medianMseconds(), 3u) << ", " << String::toAString(performanceOcean11.worstMseconds(), 3u) << "] ms";

	Log::info() << "Ocean vs. OpenCV:";
	Log::info() << "Performance ratio (14641 vs 14641): [" << String::toAString(performanceOpenCV14641.best() / performanceOcean14641.best(), 2u) << ", "<< String::toAString(performanceOpenCV14641.median() / performanceOcean14641.median(), 2u) << ", "<< String::toAString(performanceOpenCV14641.worst() / performanceOcean14641.worst(), 2u) << "] x";
	Log::info() << "Performance ratio (11 vs 14641): [" << String::toAString(performanceOpenCV14641.best() / performanceOcean11.best(), 2u) << ", "<< String::toAString(performanceOpenCV14641.median() / performanceOcean11.median(), 2u) << ", "<< String::toAString(performanceOpenCV14641.worst() / performanceOcean11.worst(), 2u) << "] x";

	Log::info() << "Validation: ";

	Log::info() << "OpenCV 14641 average: " << String::toAString(opencvAverageAbsError14641 / double(performanceOpenCV14641.measurements()), 2u) << ", maximal: " << String::toAString(opencvMaximalAbsError14641, 2u);
	Log::info() << "Ocean 14641 average: " << String::toAString(oceanAverageAbsError14641 / double(performanceOcean14641.measurements()), 2u) << ", maximal: " << String::toAString(oceanMaximalAbsError14641, 2u);
}

void TestFramePyramid::testFramePyramid(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration)
{
	ocean_assert(width >= 2u && height >= 2u);
	ocean_assert(channels >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... with " << channels << " channels:";
	Log::info() << " ";

	unsigned int iteration = 0u;

	const unsigned int layers = CV::FramePyramid::idealLayers(width, height, 1u, 1u);
	ocean_assert(layers != 0u);

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performanceOcean11;
	HighPerformanceStatistic performanceOcean14641;
	HighPerformanceStatistic performanceOpenCV14641;

	const Timestamp startTimestamp(true);

	do
	{
		const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
		cv::Mat cvFrame;

		if (iteration % 3u == 0u)
		{
			performanceOcean11.start();
				const CV::FramePyramid framePyramid(frame, CV::FramePyramid::DM_FILTER_11, layers, true /*copyFirstLayer*/, nullptr);
			performanceOcean11.stop();

			ocean_assert(framePyramid.layers() == layers);
		}
		else if (iteration % 3u == 1u)
		{
			performanceOcean14641.start();
				const CV::FramePyramid framePyramid(frame, CV::FramePyramid::DM_FILTER_14641, layers, true /*copyFirstLayer*/, nullptr);
			performanceOcean14641.stop();

			ocean_assert(framePyramid.layers() == layers);
		}
		else
		{
			CV::OpenCVUtilities::toCvMat(frame).copyTo(cvFrame);

			performanceOpenCV14641.start();
				std::vector<cv::Mat> cvframePyramidLayers;
				cv::buildPyramid(cvFrame, cvframePyramidLayers, int(layers) - 1); // layers == 1: create the original frame + one additional pyramid layer
			performanceOpenCV14641.stop();

			ocean_assert(cvframePyramidLayers.size() == size_t(layers));
		}

		iteration++;
	}
	while (iteration < 3u || startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance OpenCV (14641): [" << String::toAString(performanceOpenCV14641.bestMseconds(), 3u) << ", " << String::toAString(performanceOpenCV14641.medianMseconds(), 3u) << ", " << String::toAString(performanceOpenCV14641.worstMseconds(), 3u) << "] ms";
	Log::info() << "Performance Ocean (14641): [" << String::toAString(performanceOcean14641.bestMseconds(), 3u) << ", " << String::toAString(performanceOcean14641.medianMseconds(), 3u) << ", " << String::toAString(performanceOcean14641.worstMseconds(), 3u) << "] ms";
	Log::info() << "Performance Ocean (11): [" << String::toAString(performanceOcean11.bestMseconds(), 3u) << ", " << String::toAString(performanceOcean11.medianMseconds(), 3u) << ", " << String::toAString(performanceOcean11.worstMseconds(), 3u) << "] ms";

	Log::info() << "Ocean vs. OpenCV:";
	Log::info() << "Performance ratio (14641 vs 14641): [" << String::toAString(performanceOpenCV14641.best() / performanceOcean14641.best(), 1u) << ", "<< String::toAString(performanceOpenCV14641.median() / performanceOcean14641.median(), 1u) << ", "<< String::toAString(performanceOpenCV14641.worst() / performanceOcean14641.worst(), 1u) << "] x";
	Log::info() << "Performance ratio (11 vs 14641): [" << String::toAString(performanceOpenCV14641.best() / performanceOcean11.best(), 1u) << ", "<< String::toAString(performanceOpenCV14641.median() / performanceOcean11.median(), 1u) << ", "<< String::toAString(performanceOpenCV14641.worst() / performanceOcean11.worst(), 1u) << "] x";
}

}

}

}

}
