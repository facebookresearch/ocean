/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testopencv/TestFrameInterpolatorNearestPixel.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameFilterGaussian.h"
#include "ocean/cv/FrameInterpolatorNearestPixel.h"
#include "ocean/cv/OpenCVUtilities.h"

#include "ocean/test/testcv/TestFrameInterpolatorNearestPixel.h"

#include <opencv2/imgproc.hpp>

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestOpenCV
{

void TestFrameInterpolatorNearestPixel::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Frame interpolator nearest pixel test:   ---";
	Log::info() << " ";

	testHomography(testDuration);

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	testAffine(testDuration);

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	Log::info() << "Frame interpolator nearest pixel test succeeded.";
}

void TestFrameInterpolatorNearestPixel::testHomography(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	const unsigned int widths[] = {640u, 800u, 1280u, 1920u, 3840u};
	const unsigned int heights[] = {480u, 640u, 720u, 1080u, 2160u};

	Log::info() << "Homography test (with constant border color):";
	Log::info() << " ";

	for (unsigned int n = 0u; n < sizeof(widths) / sizeof(widths[0]); ++n)
	{
		Log::info().newLine(n != 0u);

		const unsigned int width = widths[n];
		const unsigned int height = heights[n];

		for (unsigned int channel = 1u; channel <= 4u; ++channel)
		{
			testHomography(width, height, channel, testDuration);
			Log::info() << " ";
		}
	}

	Log::info() << " ";
	Log::info() << "Homography test finished.";
}

void TestFrameInterpolatorNearestPixel::testAffine(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	const unsigned int widths[] = {64u, 400u, 640u, 800u, 1280u, 1920u, 3840u};
	const unsigned int heights[] = {64u, 400u, 480u, 640u, 720u, 1080u, 2160u};

	Log::info() << "Test for original OpenCV affine transformations (with constant border color):";

	Log::info() << " ";

	for (unsigned int n = 0u; n < sizeof(widths) / sizeof(widths[0]); ++n)
	{
		Log::info().newLine(n != 0u);

		const unsigned int width = widths[n];
		const unsigned int height = heights[n];

		for (unsigned int channel = 1u; channel <= 4u; ++channel)
		{
			testAffine(width, height, channel, testDuration);
			Log::info() << " ";
		}
	}

	Log::info() << " ";
	Log::info() << "Affine transformation test finished.";
}

void TestFrameInterpolatorNearestPixel::testHomography(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration)
{
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(channels != 0u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for a " << width << "x" << height << " frame with " << channels << " channels:";
	Log::info() << " ";

	RandomGenerator randomgGenerator;

	const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, channels);

	Frame sourceFrame(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT));
	Frame targetFrame(sourceFrame.frameType());

	std::vector<uint8_t> backgroundColor(channels);

	cv::Mat cvSourceFrame(height, width, CV_MAKETYPE(CV_8U, channels));
	cv::Mat cvTargetFrame(height, width, CV_MAKETYPE(CV_8U, channels));

	// 0.95, 1.05, 35.8
	// -0.05, 1, -20.4,
	// 0.05, 0.00, 1.0
	const SquareMatrix3 transformation(Scalar(0.95), Scalar(-0.05), Scalar(0.05), Scalar(1.05), Scalar(1), Scalar(0), Scalar(35.8), Scalar(-20.4), Scalar(1));
	ocean_assert(!transformation.isSingular());

	const cv::Matx33d cvTransformation = CV::OpenCVUtilities::toCvMatx33(transformation);

	double oceanSumAverageError = 0.0;
	double oceanMaxError = 0.0;
	unsigned long long oceanIterations = 0ull;

	double opencvSumAverageError = 0.0;
	double opencvMaxError = 0.0;
	unsigned long long opencvIterations = 0ull;

	HighPerformanceStatistic performanceOceanSingleCore;
	HighPerformanceStatistic performanceOceanMultiCore;
	HighPerformanceStatistic performanceOpenCV;

	unsigned int iteration = 0u;

	const Timestamp startTimestamp(true);

	do
	{
		CV::CVUtilities::randomizeFrame(sourceFrame, /* skipPaddingArea */ true, &randomgGenerator);
		CV::CVUtilities::randomizeFrame(targetFrame, /* skipPaddingArea */ true, &randomgGenerator);

		if (iteration % 2u == 0u)
		{
			performanceOceanSingleCore.start();
			CV::FrameInterpolatorNearestPixel::Comfort::homography(sourceFrame, targetFrame, transformation, backgroundColor.data(), nullptr);
			performanceOceanSingleCore.stop();

			double averageError = 0.0;
			double maximalError = 0u;
			Test::TestCV::TestFrameInterpolatorNearestPixel::validateHomography<uint8_t>(sourceFrame.constdata<uint8_t>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), targetFrame.constdata<uint8_t>(), targetFrame.width(), targetFrame.height(), targetFrame.paddingElements(), sourceFrame.channels(), transformation, backgroundColor.data(), CV::PixelPositionI(0u, 0u), &maximalError, &averageError);

			oceanSumAverageError += averageError;
			oceanMaxError = max(oceanMaxError, maximalError);
			oceanIterations++;


			performanceOceanMultiCore.start();
			CV::FrameInterpolatorNearestPixel::Comfort::homography(sourceFrame, targetFrame, transformation, backgroundColor.data(), WorkerPool::get().scopedWorker()());
			performanceOceanMultiCore.stop();

			averageError = 0.0;
			maximalError = 0u;
			Test::TestCV::TestFrameInterpolatorNearestPixel::validateHomography<uint8_t>(sourceFrame.constdata<uint8_t>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), targetFrame.constdata<uint8_t>(), targetFrame.width(), targetFrame.height(), targetFrame.paddingElements(), sourceFrame.channels(), transformation, backgroundColor.data(), CV::PixelPositionI(0u, 0u), &maximalError, &averageError);

			oceanSumAverageError += averageError;
			oceanMaxError = max(oceanMaxError, maximalError);

			oceanIterations++;
		}
		else
		{
			CV::OpenCVUtilities::toCvMat(sourceFrame).copyTo(cvSourceFrame);
			CV::OpenCVUtilities::toCvMat(targetFrame).copyTo(cvTargetFrame);

			cv::Scalar cvBackgroundColor;
			for (unsigned int n = 0u; n < channels; ++n)
			{
				cvBackgroundColor.val[n] = double(backgroundColor[n]);
			}

			performanceOpenCV.start();
			cv::warpPerspective(cvSourceFrame, cvTargetFrame, cvTransformation, cv::Size(targetFrame.width(), targetFrame.height()), cv::WARP_INVERSE_MAP | cv::INTER_NEAREST, cv::BORDER_CONSTANT, cvBackgroundColor);
			performanceOpenCV.stop();

			targetFrame = CV::OpenCVUtilities::toOceanFrame(cvTargetFrame, false, sourceFrame.pixelFormat());

			double averageError = 0.0;
			double maximalError = 0u;
			Test::TestCV::TestFrameInterpolatorNearestPixel::validateHomography<uint8_t>(sourceFrame.constdata<uint8_t>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), targetFrame.constdata<uint8_t>(), targetFrame.width(), targetFrame.height(), targetFrame.paddingElements(), sourceFrame.channels(), transformation, backgroundColor.data(), CV::PixelPositionI(0u, 0u), &maximalError, &averageError);

			opencvSumAverageError += averageError;
			opencvMaxError = max(opencvMaxError, maximalError);
			opencvIterations++;
		}

		iteration++;
	}
	while (iteration < 2u || startTimestamp + testDuration > Timestamp(true));

	if (opencvIterations != 0ull)
	{
		Log::info() << "Performance OpenCV: [" << String::toAString(performanceOpenCV.bestMseconds(), 3u) << ", " << String::toAString(performanceOpenCV.medianMseconds(), 3u) << ", " << String::toAString(performanceOpenCV.worstMseconds(), 3u) << "] ms";
		Log::info() << "Validation: average error " << String::toAString(double(opencvSumAverageError) / double(opencvIterations), 2u) << ", maximal error: " << opencvMaxError;
		Log::info() << " ";
	}

	if (oceanIterations != 0ull)
	{
		Log::info() << "Performance Ocean (single-core): [" << String::toAString(performanceOceanSingleCore.bestMseconds(), 3u) << ", " << String::toAString(performanceOceanSingleCore.medianMseconds(), 3u) << ", " << String::toAString(performanceOceanSingleCore.worstMseconds(), 3u) << "] ms";
		Log::info() << "Performance Ocean (multi-core): [" << String::toAString(performanceOceanMultiCore.bestMseconds(), 3u) << ", " << String::toAString(performanceOceanMultiCore.medianMseconds(), 3u) << ", " << String::toAString(performanceOceanMultiCore.worstMseconds(), 3u) << "] ms";
		Log::info() << "Multi-core boost factor: [" << String::toAString(performanceOceanSingleCore.best() / performanceOceanMultiCore.best(), 1u) << ", " << String::toAString(performanceOceanSingleCore.median() / performanceOceanMultiCore.median(), 1u) << ", " << String::toAString(performanceOceanSingleCore.worst() / performanceOceanMultiCore.worst(), 1u) << "] x";
		Log::info() << "Validation: average error " << String::toAString(double(oceanSumAverageError) / double(oceanIterations), 2u) << ", maximal error: " << oceanMaxError;
	}

	if (oceanIterations != 0ull && opencvIterations != 0ull)
	{
		Log::info() << " ";
		Log::info() << "Ocean vs. OpenCV";
		Log::info() << "Performance ratio (single-core): [" << String::toAString(performanceOpenCV.best() / performanceOceanSingleCore.best(), 1u) << ", "<< String::toAString(performanceOpenCV.median() / performanceOceanSingleCore.median(), 1u) << ", "<< String::toAString(performanceOpenCV.worst() / performanceOceanSingleCore.worst(), 1u) << "] x";
		Log::info() << "Performance ratio (multi-core): [" << String::toAString(performanceOpenCV.best() / performanceOceanMultiCore.best(), 1u) << ", "<< String::toAString(performanceOpenCV.median() / performanceOceanMultiCore.median(), 1u) << ", "<< String::toAString(performanceOpenCV.worst() / performanceOceanMultiCore.worst(), 1u) << "] x";
	}
}

void TestFrameInterpolatorNearestPixel::testAffine(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration)
{
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(channels != 0u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for a " << width << "x" << height << " frame with " << channels << " channels:";
	Log::info() << " ";

	RandomGenerator randomGenerator;

	const FrameType::PixelFormat pixelFormat = FrameType::findPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, channels);

	std::vector<unsigned char> backgroundColor(channels);

	cv::Mat cvSourceFrame(height, width, CV_MAKETYPE(CV_8U, channels));
	cv::Mat cvTargetFrame(height, width, CV_MAKETYPE(CV_8U, channels));

	const SquareMatrix3 rotation(Rotation(0, 0, 1, Numeric::deg2rad(5)));
	const SquareMatrix3 scaleXY(Vector3(Scalar(0.95), 0, 0), Vector3(0, Scalar(1.05), 0), Vector3(0, 0, 1));
	const SquareMatrix3 translation(Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(Scalar(width) * Scalar(0.5), Scalar(height) * Scalar(0.5), 1));
	const SquareMatrix3 transformation = translation * rotation * scaleXY * translation.inverted();

	ocean_assert(!transformation.isSingular());
	const cv::Mat_<Scalar> cvTransformation(CV::OpenCVUtilities::toCvMatx23Affine(transformation.inverted()));

	double oceanSumAverageError = 0.0;
	double oceanMaxError = 0.0;
	unsigned long long oceanIterations = 0ull;

	double opencvSumAverageError = 0.0;
	double opencvMaxError = 0.0;
	unsigned long long opencvIterations = 0ull;

	HighPerformanceStatistic performanceOceanSingleCore;
	HighPerformanceStatistic performanceOceanMultiCore;
	HighPerformanceStatistic performanceOpenCV;

	unsigned int iteration = 0u;

	cv::Scalar cvBackgroundColor;
	for (unsigned int n = 0u; n < channels; ++n)
	{
		cvBackgroundColor.val[n] = double(backgroundColor[n]);
	}

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int sourcePaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);
		const unsigned int targetPaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);

		Frame sourceFrame(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
		Frame targetFrame(sourceFrame.frameType(), targetPaddingElements);

		CV::CVUtilities::randomizeFrame(targetFrame, /* skipPaddingArea */ true, &randomGenerator);
		CV::CVUtilities::randomizeFrame(sourceFrame, /* skipPaddingArea */ true, &randomGenerator);

		const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		CV::FrameFilterGaussian::filter(sourceFrame, 5u);

		if (iteration % 2u == 0u)
		{
			performanceOceanSingleCore.start();
			CV::FrameInterpolatorNearestPixel::Comfort::affine(sourceFrame, targetFrame, transformation, backgroundColor.data(), nullptr);
			performanceOceanSingleCore.stop();

			double averageError = 0.0;
			double maximalError = 0u;
			Test::TestCV::TestFrameInterpolatorNearestPixel::validateHomography<uint8_t>(sourceFrame.constdata<uint8_t>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), targetFrame.constdata<uint8_t>(), targetFrame.width(), targetFrame.height(), targetFrame.paddingElements(), sourceFrame.channels(), transformation, backgroundColor.data(), CV::PixelPositionI(0u, 0u), &maximalError, &averageError);

			oceanSumAverageError += averageError;
			oceanMaxError = max(oceanMaxError, maximalError);
			oceanIterations++;

			performanceOceanMultiCore.start();
			CV::FrameInterpolatorNearestPixel::Comfort::affine(sourceFrame, targetFrame, transformation, backgroundColor.data(), WorkerPool::get().scopedWorker()());
			performanceOceanMultiCore.stop();

			averageError = 0.0;
			maximalError = 0u;
			Test::TestCV::TestFrameInterpolatorNearestPixel::validateHomography<uint8_t>(sourceFrame.constdata<uint8_t>(), sourceFrame.width(), sourceFrame.height(), 0u, targetFrame.constdata<uint8_t>(), targetFrame.width(), targetFrame.height(), 0u, sourceFrame.channels(), transformation, backgroundColor.data(), CV::PixelPositionI(0u, 0u), &maximalError, &averageError);

			oceanSumAverageError += averageError;
			oceanMaxError = max(oceanMaxError, maximalError);
			oceanIterations++;
		}
		else
		{
			CV::OpenCVUtilities::toCvMat(sourceFrame).copyTo(cvSourceFrame);
			CV::OpenCVUtilities::toCvMat(targetFrame).copyTo(cvTargetFrame);

			performanceOpenCV.start();
			cv::warpAffine(cvSourceFrame, cvTargetFrame, cvTransformation, cv::Size(targetFrame.width(), targetFrame.height()), cv::INTER_NEAREST, cv::BORDER_CONSTANT, cvBackgroundColor);
			performanceOpenCV.stop();

			targetFrame = CV::OpenCVUtilities::toOceanFrame(cvTargetFrame, false, sourceFrame.pixelFormat());

			double averageError = 0.0;
			double maximalError = 0u;
			Test::TestCV::TestFrameInterpolatorNearestPixel::validateHomography<uint8_t>(sourceFrame.constdata<uint8_t>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), targetFrame.constdata<uint8_t>(), targetFrame.width(), targetFrame.height(), targetFrame.paddingElements(), sourceFrame.channels(), transformation, backgroundColor.data(), CV::PixelPositionI(0u, 0u), &maximalError, &averageError);

			opencvSumAverageError += averageError;
			opencvMaxError = max(opencvMaxError, maximalError);
			opencvIterations++;
		}

		if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
		{
			assert(false && "This must never happen!");
			Log::info() << "Invalid padding memory!";
		}

		iteration++;
	}
	while (iteration < 2u || startTimestamp + testDuration > Timestamp(true));

	if (oceanIterations != 0ull)
	{
		Log::info() << "Ocean";
		Log::info() << "Performance (single-core): [" << String::toAString(performanceOceanSingleCore.bestMseconds(), 3u) << ", " << String::toAString(performanceOceanSingleCore.medianMseconds(), 3u) << ", " << String::toAString(performanceOceanSingleCore.worstMseconds(), 3u) << "] ms";
		Log::info() << "Performance (multi-core): [" << String::toAString(performanceOceanMultiCore.bestMseconds(), 3u) << ", " << String::toAString(performanceOceanMultiCore.medianMseconds(), 3u) << ", " << String::toAString(performanceOceanMultiCore.worstMseconds(), 3u) << "] ms";
		Log::info() << "Multi-core boost: [" << String::toAString(performanceOceanSingleCore.best() / performanceOceanMultiCore.best(), 1u) << ", " << String::toAString(performanceOceanSingleCore.median() / performanceOceanMultiCore.median(), 1u) << ", " << String::toAString(performanceOceanSingleCore.worst() / performanceOceanMultiCore.worst(), 1u) << "] x";
		Log::info() << "Validation: average error " << String::toAString(double(oceanSumAverageError) / double(oceanIterations), 2u) << ", maximal error: " << oceanMaxError;
		Log::info() << " ";
	}

	if (opencvIterations != 0ull)
	{
		Log::info() << "OpenCV";
		Log::info() << "Performance: [" << String::toAString(performanceOpenCV.bestMseconds(), 3u) << ", " << String::toAString(performanceOpenCV.medianMseconds(), 3u) << ", " << String::toAString(performanceOpenCV.worstMseconds(), 3u) << "] ms";
		Log::info() << "Validation: average error " << String::toAString(double(opencvSumAverageError) / double(opencvIterations), 2u) << ", maximal error: " << opencvMaxError;
		Log::info() << " ";

		if (oceanIterations != 0ull)
		{
			Log::info() << "Ocean vs. OpenCV";
			Log::info() << "Performance ratio (single-core): [" << String::toAString(performanceOpenCV.best() / performanceOceanSingleCore.best(), 1u) << ", " << String::toAString(performanceOpenCV.median() / performanceOceanSingleCore.median(), 1u) << ", " << String::toAString(performanceOpenCV.worst() / performanceOceanSingleCore.worst(), 1u) << "] x";
			Log::info() << "Performance ratio (multi-core): [" << String::toAString(performanceOpenCV.best() / performanceOceanMultiCore.best(), 1u) << ", " << String::toAString(performanceOpenCV.median() / performanceOceanMultiCore.median(), 1u) << ", " << String::toAString(performanceOpenCV.worst() / performanceOceanMultiCore.worst(), 1u) << "] x";
			Log::info() << " ";
		}
	}

	Log::info() << " ";
}

} // namespace TestOpenCV

} // mamespace TestCV

} // namespace Test

} // namespace Ocean
