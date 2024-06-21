/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testopencv/TestFrameInterpolatorBilinear.h"

#include "ocean/base/RandomGenerator.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Worker.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameInterpolatorBilinear.h"
#include "ocean/cv/OpenCVUtilities.h"

#include "ocean/math/Random.h"

#include "ocean/test/testcv/TestFrameInterpolatorBilinear.h"

#include <opencv2/imgproc.hpp>

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestOpenCV
{

void TestFrameInterpolatorBilinear::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Frame interpolator bilinear test:   ---";
	Log::info() << " ";

	testHomography(testDuration);

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	testAffine(testDuration);

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	testResize<uint8_t>(testDuration);

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	testResize<float>(testDuration);

	Log::info() << " ";

	Log::info() << "Frame interpolation test succeeded.";
}

void TestFrameInterpolatorBilinear::testHomography(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	const unsigned int widths[] = {640u, 800u, 1280u, 1920u, 3840u};
	const unsigned int heights[] = {480u, 640u, 720u, 1080u, 2160u};

	Log::info() << "Homography interpolation test (with constant border color):";
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
	Log::info() << "Homography + bilinear interpolation validation succeeded.";
}

void TestFrameInterpolatorBilinear::testAffine(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	const unsigned int widths[] = {640u, 800u, 1280u, 1920u, 3840u};
	const unsigned int heights[] = {480u, 640u, 720u, 1080u, 2160u};

	Log::info() << "Test for interpolation based on affine transformations (with constant border color):";
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
	Log::info() << "Affine transformation + bilinear interpolation validation succeeded.";
}

void TestFrameInterpolatorBilinear::testHomography(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration)
{
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(channels != 0u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for a " << width << "x" << height << " frame with " << channels << " channels:";
	Log::info() << " ";

	const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, channels);

	Frame sourceFrame(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT));
	Frame targetFrame(sourceFrame.frameType());

	std::vector<unsigned char> backgroundColor(channels);

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
		CV::CVUtilities::randomizeFrame(sourceFrame);
		CV::CVUtilities::randomizeFrame(targetFrame);

		if (iteration % 2u == 0u)
		{
			performanceOceanSingleCore.start();
				CV::FrameInterpolatorBilinear::Comfort::homography(sourceFrame, targetFrame, transformation, backgroundColor.data(), nullptr);
			performanceOceanSingleCore.stop();

			double averageError = 0.0;
			double maximalError = 0u;
			Test::TestCV::TestFrameInterpolatorBilinear::validateHomography<uint8_t>(sourceFrame, targetFrame, transformation, backgroundColor.data(), CV::PixelPositionI(0u, 0u), &averageError, &maximalError);

			oceanSumAverageError += averageError;
			oceanMaxError = max(oceanMaxError, maximalError);
			oceanIterations++;

			const WorkerPool::ScopedWorker scopedWorker(WorkerPool::get().scopedWorker());

			performanceOceanMultiCore.start();
				CV::FrameInterpolatorBilinear::Comfort::homography(sourceFrame, targetFrame, transformation, backgroundColor.data(), scopedWorker());
			performanceOceanMultiCore.stop();

			averageError = 0.0;
			maximalError = 0u;
			Test::TestCV::TestFrameInterpolatorBilinear::validateHomography<uint8_t>(sourceFrame, targetFrame, transformation, backgroundColor.data(), CV::PixelPositionI(0u, 0u), &averageError, &maximalError);

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
				cv::warpPerspective(cvSourceFrame, cvTargetFrame, cvTransformation, cv::Size(targetFrame.width(), targetFrame.height()), cv::WARP_INVERSE_MAP | cv::INTER_LINEAR, cv::BORDER_CONSTANT, cvBackgroundColor);
			performanceOpenCV.stop();

			double averageError = 0.0;
			double maximalError = 0u;

			Test::TestCV::TestFrameInterpolatorBilinear::validateHomography<uint8_t>(sourceFrame, CV::OpenCVUtilities::toOceanFrame(cvTargetFrame), transformation, backgroundColor.data(), CV::PixelPositionI(0u, 0u), &averageError, &maximalError);

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

void TestFrameInterpolatorBilinear::testAffine(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration)
{
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(channels != 0u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for a " << width << "x" << height << " frame with " << channels << " channels:";
	Log::info() << " ";

	const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, channels);

	RandomGenerator randomGenerator;

	const unsigned int sourcePaddingElements = RandomI::random(randomGenerator, 128u) * RandomI::random(randomGenerator, 1u);
	const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 128u) * RandomI::random(randomGenerator, 1u);

	Frame sourceFrame(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
	Frame targetFrame(sourceFrame.frameType(), targetPaddingElements);

	std::vector<uint8_t> backgroundColor(channels);

	cv::Mat cvSourceFrame(height, width, CV_MAKETYPE(CV_8U, channels));
	cv::Mat cvTargetFrame(height, width, CV_MAKETYPE(CV_8U, channels));

	// Affine transformation
	// 0.732090831 -0.237448305 59.5
	// 0.239417255 0.430772632  -122.5
	// 0           0            1
	const SquareMatrix3 rotation(Rotation(Scalar(0), Scalar(0), Scalar(1), Numeric::deg2rad(Scalar(5))));
	const SquareMatrix3 scaleXY(Scalar(0.75), Scalar(0), Scalar(0), Scalar(0), Scalar(0.45), Scalar(0), Scalar(0), Scalar(0), Scalar(1));
	const SquareMatrix3 shearXY(Scalar(1), Numeric::tan(Numeric::deg2rad(Scalar(15))), Scalar(0), Numeric::tan(Numeric::deg2rad(Scalar(-21))), Scalar(1), Scalar(0), Scalar(0), Scalar(0), Scalar(1));
	const SquareMatrix3 translation(Scalar(1), Scalar(0), Scalar(0), Scalar(0), Scalar(1), Scalar(0), Scalar(59.5), Scalar(-122.5), Scalar(1));
	const SquareMatrix3 transformation = translation * shearXY * scaleXY * rotation; // Random order of transformations.
	ocean_assert(!transformation.isSingular());

	const cv::Mat_<Scalar> cvTransformation(CV::OpenCVUtilities::toCvMatx23Affine(transformation));

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
		CV::CVUtilities::randomizeFrame(sourceFrame);
		CV::CVUtilities::randomizeFrame(targetFrame);

		if (iteration % 2u == 0u)
		{
			performanceOceanSingleCore.start();
				CV::FrameInterpolatorBilinear::Comfort::affine(sourceFrame, targetFrame, transformation, backgroundColor.data(), nullptr);
			performanceOceanSingleCore.stop();

			double averageError = 0.0;
			double maximalError = 0u;
			Test::TestCV::TestFrameInterpolatorBilinear::validateHomography<uint8_t>(sourceFrame, targetFrame, transformation, backgroundColor.data(), CV::PixelPositionI(0u, 0u), &averageError, &maximalError);

			oceanSumAverageError += averageError;
			oceanMaxError = max(oceanMaxError, maximalError);
			oceanIterations++;

			const WorkerPool::ScopedWorker scopedWorker(WorkerPool::get().scopedWorker());

			performanceOceanMultiCore.start();
				CV::FrameInterpolatorBilinear::Comfort::affine(sourceFrame, targetFrame, transformation, backgroundColor.data(), scopedWorker());
			performanceOceanMultiCore.stop();

			averageError = 0.0;
			maximalError = 0u;
			Test::TestCV::TestFrameInterpolatorBilinear::validateHomography<uint8_t>(sourceFrame, targetFrame, transformation, backgroundColor.data(), CV::PixelPositionI(0u, 0u), &averageError, &maximalError);

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
			cv::warpAffine(cvSourceFrame, cvTargetFrame, cvTransformation, cv::Size(targetFrame.width(), targetFrame.height()), cv::WARP_INVERSE_MAP | cv::INTER_LINEAR, cv::BORDER_CONSTANT, cvBackgroundColor);
			performanceOpenCV.stop();

			unsigned int cvTargetFramePaddingElements;
			if (!Frame::strideBytes2paddingElements(targetFrame.pixelFormat(), (unsigned int)cvTargetFrame.cols, cvTargetFrame.step, cvTargetFramePaddingElements))
			{
				ocean_assert(false && "Failed to compute the number of padding elements - never be here!");
			}

			double averageError = 0.0;
			double maximalError = 0u;
			Test::TestCV::TestFrameInterpolatorBilinear::validateHomography<uint8_t>(sourceFrame, CV::OpenCVUtilities::toOceanFrame(cvTargetFrame), transformation, backgroundColor.data(), CV::PixelPositionI(0u, 0u), &averageError, &maximalError);

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

template <typename T>
void TestFrameInterpolatorBilinear::testResize(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Performance test for frame resizing (data type '" << TypeNamer::name<T>() << "'):";
	Log::info() << " ";

	const std::vector<unsigned int> sourceWidths = {55u, 70u, 415u, 679u, 1280u, 1920u, 3840u, 3840u};
	const std::vector<unsigned int> sourceHeights = {67u, 55u, 319u, 455u, 720u, 1080u, 2160u, 2160u};

	const std::vector<unsigned int> targetWidths = {64u, 60u, 320u, 640u, 1000u, 2000u, 4000u, 3840u};
	const std::vector<unsigned int> targetHeights = {66u, 60u, 240u, 480u, 680u, 1000u, 2155u, 2155u};

	ocean_assert(sourceWidths.size() == sourceHeights.size());
	ocean_assert(sourceWidths.size() == targetWidths.size());
	ocean_assert(sourceHeights.size() == targetHeights.size());

	for (size_t i = 0; i < sourceWidths.size(); ++i)
	{
		const unsigned int sourceWidth = sourceWidths[i];
		const unsigned int sourceHeight = sourceHeights[i];
		const unsigned int targetWidth = targetWidths[i];
		const unsigned int targetHeight = targetHeights[i];

		for (unsigned int channels = 1u; channels <= 4u; ++channels)
		{
			testResize<T>(sourceWidth, sourceHeight, targetWidth, targetHeight, channels, testDuration);
			Log::info() << " ";
		}
	}
}

template <typename T>
void TestFrameInterpolatorBilinear::testResize(const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int channels, const double testDuration)
{
	ocean_assert(sourceWidth != 0u && sourceHeight != 0u);
	ocean_assert(targetWidth != 0u && targetHeight != 0u);
	ocean_assert(channels != 0u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... resize frame: " << sourceWidth << "x" << sourceHeight << " with " << channels << " channels to " << targetWidth << "x" << targetHeight;
	Log::info() << " ";

	RandomGenerator randomGenerator;

	Worker worker;

	unsigned int iteration = 0u;
	unsigned int iterationOcean = 0u;
	unsigned int iterationOpenCV = 0u;

	double sumAverageErrorOcean = 0.;
	double sumAverageErrorOpenCV = 0.;

	double maximalErrorOcean = 0.;
	double maximalErrorOpenCV = 0.;

	HighPerformanceStatistic performanceOceanSingleCore;
	HighPerformanceStatistic performanceOceanMultiCore;
	HighPerformanceStatistic performanceOpenCV;

	const double xTargetToSource = double(sourceWidth) / double(targetWidth);
	const double yTargetToSource = double(sourceHeight) / double(targetHeight);

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int sourcePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
		const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

		Frame sourceFrame(FrameType(sourceWidth, sourceHeight, FrameType::genericPixelFormat<T>(channels), FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
		Frame targetFrame(FrameType(targetWidth, targetHeight, FrameType::genericPixelFormat<T>(channels), FrameType::ORIGIN_UPPER_LEFT), targetPaddingElements);

		for (unsigned int n = 0u; n < sourceFrame.pixels() * channels; ++ n)
		{
			sourceFrame.data<T>()[n] = T(RandomD::scalar(randomGenerator, -255.0, 255.0));
		}

		if (iteration % 2u == 0u)
		{
			for (const bool multiCoreIteration : {false, true})
			{
				Worker* useWorker = multiCoreIteration ? &worker : nullptr;
				HighPerformanceStatistic& performanceOcean = multiCoreIteration ? performanceOceanMultiCore : performanceOceanSingleCore;

				performanceOcean.start();

				// Ocean single-core
				switch (channels)
				{
					case 1u:
						CV::FrameInterpolatorBilinear::scale<T, 1u>(sourceFrame.constdata<T>(), targetFrame.data<T>(), sourceFrame.width(), sourceFrame.height(), targetFrame.width(), targetFrame.height(), xTargetToSource, yTargetToSource, sourceFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
						break;

					case 2u:
						CV::FrameInterpolatorBilinear::scale<T, 2u>(sourceFrame.constdata<T>(), targetFrame.data<T>(), sourceFrame.width(), sourceFrame.height(), targetFrame.width(), targetFrame.height(), xTargetToSource, yTargetToSource, sourceFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
						break;

					case 3u:
						CV::FrameInterpolatorBilinear::scale<T, 3u>(sourceFrame.constdata<T>(), targetFrame.data<T>(), sourceFrame.width(), sourceFrame.height(), targetFrame.width(), targetFrame.height(), xTargetToSource, yTargetToSource, sourceFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
						break;

					case 4u:
						CV::FrameInterpolatorBilinear::scale<T, 4u>(sourceFrame.constdata<T>(), targetFrame.data<T>(), sourceFrame.width(), sourceFrame.height(), targetFrame.width(), targetFrame.height(), xTargetToSource, yTargetToSource, sourceFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
						break;

					default:
						ocean_assert(false && "This should never be the case.");
						break;
				}

				performanceOcean.stop();

				double averageAbsError = NumericD::maxValue();
				double maximalAbsError = NumericD::maxValue();
				TestCV::TestFrameInterpolatorBilinear::validateScaleFrame<T>(sourceFrame.constdata<T>(), sourceWidth, sourceHeight, channels, targetFrame.constdata<T>(), targetWidth, targetHeight, xTargetToSource, yTargetToSource, sourceFrame.paddingElements(), targetFrame.paddingElements(), &averageAbsError, &maximalAbsError);

				sumAverageErrorOcean += averageAbsError;
				maximalErrorOcean = max(maximalErrorOcean, maximalAbsError);

				++iterationOcean;
			}
		}
		else
		{
			// OpenCV
			const cv::Mat cvSourceFrame = CV::OpenCVUtilities::toCvMat(sourceFrame);
			cv::Mat cvTargetFrame = CV::OpenCVUtilities::toCvMat(targetFrame);

			performanceOpenCV.start();
				cv::resize(cvSourceFrame, cvTargetFrame, cv::Size(targetWidth, targetHeight), -1., -1.);
			performanceOpenCV.stop();

			targetFrame = CV::OpenCVUtilities::toOceanFrame(cvTargetFrame, true, targetFrame.pixelFormat());

			double averageAbsError = NumericD::maxValue();
			double maximalAbsError = NumericD::maxValue();
			TestCV::TestFrameInterpolatorBilinear::validateScaleFrame<T>(sourceFrame.constdata<T>(), sourceWidth, sourceHeight, channels, targetFrame.constdata<T>(), targetWidth, targetHeight, xTargetToSource, yTargetToSource, 0u, 0u, &averageAbsError, &maximalAbsError);

			sumAverageErrorOpenCV += averageAbsError;
			maximalErrorOpenCV = max(maximalErrorOpenCV, maximalAbsError);

			iterationOpenCV++;
		}

		iteration++;
	}
	while (iteration < 2u || startTimestamp + testDuration > Timestamp(true));

	if (iterationOcean != 0ull)
	{
		Log::info() << "Ocean";
		Log::info() << "Performance (single-core): [" << String::toAString(performanceOceanSingleCore.bestMseconds(), 3u) << ", " << String::toAString(performanceOceanSingleCore.medianMseconds(), 3u) << ", " << String::toAString(performanceOceanSingleCore.worstMseconds(), 3u) << "] ms";
		Log::info() << "Performance (multi-core): [" << String::toAString(performanceOceanMultiCore.bestMseconds(), 3u) << ", " << String::toAString(performanceOceanMultiCore.medianMseconds(), 3u) << ", " << String::toAString(performanceOceanMultiCore.worstMseconds(), 3u) << "] ms";
		Log::info() << "Multi-core boost: [" << String::toAString(performanceOceanSingleCore.best() / performanceOceanMultiCore.best(), 1u) << ", " << String::toAString(performanceOceanSingleCore.median() / performanceOceanMultiCore.median(), 1u) << ", " << String::toAString(performanceOceanSingleCore.worst() / performanceOceanMultiCore.worst(), 1u) << "] x";
		Log::info() << "Validation: average error " << String::toAString(double(sumAverageErrorOcean) / double(iterationOcean), 2u) << ", maximal error: " << String::toAString(maximalErrorOcean, 2u);
		Log::info() << " ";
	}

	if (iterationOpenCV != 0ull)
	{
		Log::info() << "OpenCV";
		Log::info() << "Performance: [" << String::toAString(performanceOpenCV.bestMseconds(), 3u) << ", " << String::toAString(performanceOpenCV.medianMseconds(), 3u) << ", " << String::toAString(performanceOpenCV.worstMseconds(), 3u) << "] ms";
		Log::info() << "Validation: average error " << String::toAString(double(sumAverageErrorOpenCV) / double(iterationOpenCV), 2u) << ", maximal error: " << String::toAString(maximalErrorOpenCV, 2u);
		Log::info() << " ";

		if (iterationOcean != 0ull)
		{
			Log::info() << "Ocean vs. OpenCV";
			Log::info() << "Performance ratio (single-core): [" << String::toAString(performanceOpenCV.best() / performanceOceanSingleCore.best(), 2u) << ", " << String::toAString(performanceOpenCV.median() / performanceOceanSingleCore.median(), 2u) << ", " << String::toAString(performanceOpenCV.worst() / performanceOceanSingleCore.worst(), 2u) << "] x";
			Log::info() << "Performance ratio (multi-core): [" << String::toAString(performanceOpenCV.best() / performanceOceanMultiCore.best(), 2u) << ", " << String::toAString(performanceOpenCV.median() / performanceOceanMultiCore.median(), 2u) << ", " << String::toAString(performanceOpenCV.worst() / performanceOceanMultiCore.worst(), 2u) << "] x";
			Log::info() << " ";
		}
	}
}

} // namespace TestOpenCV

} // mamespace TestCV

} // namespace Test

} // namespace Ocean
