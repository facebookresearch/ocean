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

#include <array>

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

	Log::info() << "Test for original OpenCV and AML-based affine transformations (with constant border color):";

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
		CV::CVUtilities::randomizeFrame(sourceFrame);
		CV::CVUtilities::randomizeFrame(targetFrame);

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

	double amlSumAverageError = 0.0;
	double amlMaxError = 0.0;
	unsigned long long amlIterations = 0ull;

	HighPerformanceStatistic performanceOceanSingleCore;
	HighPerformanceStatistic performanceOceanMultiCore;
	HighPerformanceStatistic performanceOpenCV;
	HighPerformanceStatistic performanceAML;

	unsigned int iteration = 0u;
	const unsigned int split = (channels == 1u ? 3u : 2u);

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

		CV::CVUtilities::randomizeFrame(targetFrame);
		CV::CVUtilities::randomizeFrame(sourceFrame);

		const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		CV::FrameFilterGaussian::filter(sourceFrame, 5u);

		if (iteration % split == 0u)
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
		else if (iteration % split == 1u)
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
		else
		{
			CV::OpenCVUtilities::toCvMat(sourceFrame).copyTo(cvSourceFrame);
			CV::OpenCVUtilities::toCvMat(targetFrame).copyTo(cvTargetFrame);

			// the AML-based implementation inverts the given transformation internally
			performanceAML.start();
			amlFacetrackerWarpAffine(cvSourceFrame, cvTargetFrame, cvTransformation, cv::Size(targetFrame.width(), targetFrame.height()), cv::INTER_NEAREST);
			performanceAML.stop();

			targetFrame = CV::OpenCVUtilities::toOceanFrame(cvTargetFrame, false, sourceFrame.pixelFormat());

			double averageError = 0.0;
			double maximalError = 0u;
			Test::TestCV::TestFrameInterpolatorNearestPixel::validateHomography<uint8_t>(sourceFrame.constdata<uint8_t>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), targetFrame.constdata<uint8_t>(), targetFrame.width(), targetFrame.height(), targetFrame.paddingElements(), sourceFrame.channels(), transformation, backgroundColor.data(), CV::PixelPositionI(0u, 0u), &maximalError, &averageError);

			amlSumAverageError += averageError;
			amlMaxError = max(amlMaxError, maximalError);
			amlIterations++;
		}

		if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
		{
			assert(false && "This must never happen!");
			Log::info() << "Invalid padding memory!";
		}

		iteration++;
	}
	while (iteration < split || startTimestamp + testDuration > Timestamp(true));

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

	if (amlIterations != 0ull)
	{
		Log::info() << "AML";
		Log::info() << "Performance: [" << String::toAString(performanceAML.bestMseconds(), 3u) << ", " << String::toAString(performanceAML.medianMseconds(), 3u) << ", " << String::toAString(performanceAML.worstMseconds(), 3u) << "] ms";
		Log::info() << "Validation: average error " << String::toAString(double(amlSumAverageError) / double(amlIterations), 2u) << ", maximal error: " << amlMaxError;
		Log::info() << " ";

		if (oceanIterations != 0ull)
		{
			Log::info() << "Ocean vs. AML";
			Log::info() << "Performance ratio (single-core): [" << String::toAString(performanceAML.best() / performanceOceanSingleCore.best(), 1u) << ", " << String::toAString(performanceAML.median() / performanceOceanSingleCore.median(), 1u) << ", " << String::toAString(performanceAML.worst() / performanceOceanSingleCore.worst(), 1u) << "] x";
			Log::info() << "Performance ratio (multi-core): [" << String::toAString(performanceAML.best() / performanceOceanMultiCore.best(), 1u) << ", " << String::toAString(performanceAML.median() / performanceOceanMultiCore.median(), 1u) << ", " << String::toAString(performanceAML.worst() / performanceOceanMultiCore.worst(), 1u) << "] x";
			Log::info() << " ";
		}
	}

	Log::info() << " ";
}

namespace {

using namespace cv;

enum InterpolationFlags {
	/** nearest neighbor interpolation */
	INTER_NEAREST = 0,
	/** bilinear interpolation */
	INTER_LINEAR = 1,
	/** mask for interpolation codes */
	INTER_MAX = 7,
};

enum InterpolationMasks {
	INTER_BITS = 5,
	INTER_BITS2 = INTER_BITS * 2,
	INTER_TAB_SIZE = 1 << INTER_BITS,
	INTER_TAB_SIZE2 = INTER_TAB_SIZE * INTER_TAB_SIZE
};

constexpr int INTER_REMAP_COEF_BITS = 15;
constexpr int INTER_REMAP_COEF_SCALE = 1 << INTER_REMAP_COEF_BITS;
constexpr int AB_BITS = MAX(10, (int)INTER_BITS);
constexpr int AB_SCALE = 1 << AB_BITS;
static uchar NNDeltaTab_i[INTER_TAB_SIZE2][2];

#if defined(__SSSE3__)
#define INLINE extern inline __attribute__((__gnu_inline__, __always_inline__))

INLINE __m128i _mm_packus_epi32(__m128i t0, __m128i t1) {
	t0 = _mm_slli_epi32(t0, 16);
	t0 = _mm_srai_epi32(t0, 16);
	t1 = _mm_slli_epi32(t1, 16);
	t1 = _mm_srai_epi32(t1, 16);
	return _mm_packs_epi32(t0, t1);
}

INLINE void _mm_interleave_epi16(
		__m128i& v_r0,
		__m128i& v_r1,
		__m128i& v_g0,
		__m128i& v_g1) {
	__m128i v_mask = _mm_set1_epi32(0x0000ffff);

	__m128i layer3_chunk0 = _mm_packus_epi32(
			_mm_and_si128(v_r0, v_mask), _mm_and_si128(v_r1, v_mask));
	__m128i layer3_chunk2 =
			_mm_packus_epi32(_mm_srli_epi32(v_r0, 16), _mm_srli_epi32(v_r1, 16));
	__m128i layer3_chunk1 = _mm_packus_epi32(
			_mm_and_si128(v_g0, v_mask), _mm_and_si128(v_g1, v_mask));
	__m128i layer3_chunk3 =
			_mm_packus_epi32(_mm_srli_epi32(v_g0, 16), _mm_srli_epi32(v_g1, 16));

	__m128i layer2_chunk0 = _mm_packus_epi32(
			_mm_and_si128(layer3_chunk0, v_mask),
			_mm_and_si128(layer3_chunk1, v_mask));
	__m128i layer2_chunk2 = _mm_packus_epi32(
			_mm_srli_epi32(layer3_chunk0, 16), _mm_srli_epi32(layer3_chunk1, 16));
	__m128i layer2_chunk1 = _mm_packus_epi32(
			_mm_and_si128(layer3_chunk2, v_mask),
			_mm_and_si128(layer3_chunk3, v_mask));
	__m128i layer2_chunk3 = _mm_packus_epi32(
			_mm_srli_epi32(layer3_chunk2, 16), _mm_srli_epi32(layer3_chunk3, 16));

	__m128i layer1_chunk0 = _mm_packus_epi32(
			_mm_and_si128(layer2_chunk0, v_mask),
			_mm_and_si128(layer2_chunk1, v_mask));
	__m128i layer1_chunk2 = _mm_packus_epi32(
			_mm_srli_epi32(layer2_chunk0, 16), _mm_srli_epi32(layer2_chunk1, 16));
	__m128i layer1_chunk1 = _mm_packus_epi32(
			_mm_and_si128(layer2_chunk2, v_mask),
			_mm_and_si128(layer2_chunk3, v_mask));
	__m128i layer1_chunk3 = _mm_packus_epi32(
			_mm_srli_epi32(layer2_chunk2, 16), _mm_srli_epi32(layer2_chunk3, 16));

	v_r0 = _mm_packus_epi32(
			_mm_and_si128(layer1_chunk0, v_mask),
			_mm_and_si128(layer1_chunk1, v_mask));
	v_g0 = _mm_packus_epi32(
			_mm_srli_epi32(layer1_chunk0, 16), _mm_srli_epi32(layer1_chunk1, 16));
	v_r1 = _mm_packus_epi32(
			_mm_and_si128(layer1_chunk2, v_mask),
			_mm_and_si128(layer1_chunk3, v_mask));
	v_g1 = _mm_packus_epi32(
			_mm_srli_epi32(layer1_chunk2, 16), _mm_srli_epi32(layer1_chunk3, 16));
}
#endif

static inline void interpolateLinear(float x, float* coeffs) {
	coeffs[0] = 1.f - x;
	coeffs[1] = x;
}

static void initInterTab1D(int /*method*/, float* tab, int tabsz) {
	float scale = 1.f / tabsz;
	for (int i = 0; i < tabsz; i++, tab += 2) {
		interpolateLinear(i * scale, tab);
	}
}

static const void* initInterTab2D(int method, bool fixpt) {
	if (method == INTER_NEAREST) {
		return nullptr;
	}
	static float BilinearTab_f[INTER_TAB_SIZE2][2][2];
	static short BilinearTab_i[INTER_TAB_SIZE2][2][2];
	static short BilinearTab_iC4_buf[INTER_TAB_SIZE2 + 2][2][8];
	static short(*BilinearTab_iC4)[2][8] =
			(short(*)[2][8])alignPtr(BilinearTab_iC4_buf, 16);
	// Not thread safe!
	static bool inittab[INTER_MAX + 1] = {false};

	float* tab = 0;
	short* itab = 0;
	int ksize = 0;
	tab = BilinearTab_f[0][0], itab = BilinearTab_i[0][0], ksize = 2;

	if (!inittab[method]) {
		AutoBuffer<float> _tab(8 * INTER_TAB_SIZE);
		int i, j, k1, k2;
		initInterTab1D(method, _tab, INTER_TAB_SIZE);
		for (i = 0; i < INTER_TAB_SIZE; i++) {
			for (j = 0; j < INTER_TAB_SIZE;
					 j++, tab += ksize * ksize, itab += ksize * ksize) {
				int isum = 0;

				for (k1 = 0; k1 < ksize; k1++) {
					float vy = _tab[i * ksize + k1];
					for (k2 = 0; k2 < ksize; k2++) {
						float v = vy * _tab[j * ksize + k2];
						tab[k1 * ksize + k2] = v;
						isum += itab[k1 * ksize + k2] =
								saturate_cast<short>(v * INTER_REMAP_COEF_SCALE);
					}
				}

				if (isum != INTER_REMAP_COEF_SCALE) {
					int diff = isum - INTER_REMAP_COEF_SCALE;
					int ksize2 = ksize / 2, Mk1 = ksize2, Mk2 = ksize2, mk1 = ksize2,
							mk2 = ksize2;
					for (k1 = ksize2; k1 < ksize2 + 2; k1++)
						for (k2 = ksize2; k2 < ksize2 + 2; k2++) {
							if (itab[k1 * ksize + k2] < itab[mk1 * ksize + mk2])
								mk1 = k1, mk2 = k2;
							else if (itab[k1 * ksize + k2] > itab[Mk1 * ksize + Mk2])
								Mk1 = k1, Mk2 = k2;
						}
					if (diff < 0)
						itab[Mk1 * ksize + Mk2] = (short)(itab[Mk1 * ksize + Mk2] - diff);
					else
						itab[mk1 * ksize + mk2] = (short)(itab[mk1 * ksize + mk2] - diff);
				}
			}
		}
		tab -= INTER_TAB_SIZE2 * ksize * ksize;
		itab -= INTER_TAB_SIZE2 * ksize * ksize;
		if (method == INTER_LINEAR) {
			for (i = 0; i < INTER_TAB_SIZE2; i++)
				for (j = 0; j < 4; j++) {
					BilinearTab_iC4[i][0][j * 2] = BilinearTab_i[i][0][0];
					BilinearTab_iC4[i][0][j * 2 + 1] = BilinearTab_i[i][0][1];
					BilinearTab_iC4[i][1][j * 2] = BilinearTab_i[i][1][0];
					BilinearTab_iC4[i][1][j * 2 + 1] = BilinearTab_i[i][1][1];
				}
		}
		inittab[method] = true;
	}
	return fixpt ? (const void*)itab : (const void*)tab;
}

template <typename ST, typename DT, int bits>
struct FixedPtCast {
	typedef ST type1;
	typedef DT rtype;
	enum { SHIFT = bits, DELTA = 1 << (bits - 1) };

	DT operator()(ST val) const {
		return cv::saturate_cast<DT>((val + DELTA) >> SHIFT);
	}
};

static inline int clip(int x, int a, int b) {
	return x >= a ? (x < b ? x : b - 1) : a;
}

template <typename T>
static void remapNearest(
		const Mat& _src,
		Mat& _dst,
		const Mat& _xy,
		int borderType,
		const cv::Scalar& _borderValue) {
	Size ssize = _src.size(), dsize = _dst.size();
	const int cn = _src.channels();
	const T* S0 = _src.ptr<T>();
	T cval[CV_CN_MAX];
	size_t sstep = _src.step / sizeof(S0[0]);

	for (int k = 0; k < cn; k++)
		cval[k] = saturate_cast<T>(_borderValue[k & 3]);

	unsigned width1 = ssize.width, height1 = ssize.height;

	if (_dst.isContinuous() && _xy.isContinuous()) {
		dsize.width *= dsize.height;
		dsize.height = 1;
	}

	for (int dy = 0; dy < dsize.height; dy++) {
		T* D = _dst.ptr<T>(dy);
		const short* XY = _xy.ptr<short>(dy);
		for (int dx = 0; dx < dsize.width; dx++) {
			int sx = XY[dx * 2], sy = XY[dx * 2 + 1];
			if ((unsigned)sx < width1 && (unsigned)sy < height1) {
				D[dx] = S0[sy * sstep + sx];
			} else if (borderType == BORDER_REPLICATE) {
				sx = clip(sx, 0, ssize.width);
				sy = clip(sy, 0, ssize.height);
				D[dx] = S0[sy * sstep + sx];
			} else if (borderType == BORDER_CONSTANT) {
				D[dx] = cval[0];
			} else if (borderType != BORDER_TRANSPARENT) {
				sx = borderInterpolate(sx, ssize.width, borderType);
				sy = borderInterpolate(sy, ssize.height, borderType);
				D[dx] = S0[sy * sstep + sx];
			}
		}
	}
}

template <class CastOp, class VecOp, typename AT>
static void remapBilinear(
		const cv::Mat& _src,
		cv::Mat& _dst,
		const cv::Mat& _xy,
		const cv::Mat& _fxy,
		const void* _wtab,
		int borderType,
		const cv::Scalar& _borderValue) {
	typedef typename CastOp::rtype T;
	typedef typename CastOp::type1 WT;
	Size ssize = _src.size(), dsize = _dst.size();
	int k = 0;
	const AT* wtab = (const AT*)_wtab;
	const T* S0 = _src.ptr<T>();
	size_t sstep = _src.step / sizeof(S0[0]);
	T cval[CV_CN_MAX];
	int dx, dy;
	CastOp castOp;
	VecOp vecOp;
	cval[0] = saturate_cast<T>(_borderValue[k & 3]);
	unsigned width1 = std::max(ssize.width - 1, 0),
					 height1 = std::max(ssize.height - 1, 0);
	CV_Assert(ssize.area() > 0);

	for (dy = 0; dy < dsize.height; dy++) {
		T* D = _dst.ptr<T>(dy);
		const short* XY = _xy.ptr<short>(dy);
		const ushort* FXY = _fxy.ptr<ushort>(dy);
		int X0 = 0;
		bool prevInlier = false;

		for (dx = 0; dx <= dsize.width; dx++) {
			bool curInlier = dx < dsize.width
					? (unsigned)XY[dx * 2] < width1 && (unsigned)XY[dx * 2 + 1] < height1
					: !prevInlier;
			if (curInlier == prevInlier)
				continue;

			int X1 = dx;
			dx = X0;
			X0 = X1;
			prevInlier = curInlier;

			if (!curInlier) {
				int len = vecOp(_src, D, XY + dx * 2, FXY + dx, wtab, X1 - dx);
				D += len;
				dx += len;

#if defined(__ARM_NEON__) || defined(__ARM_NEON)
				// For the remaining we store into single vector and perform the remap
				int16x4_t sstep_ = vdup_n_s16(sstep);
				int32x4_t delta_4 = vdupq_n_s32(CastOp::DELTA / 4);

				for (; dx <= X1 - dx - 8; dx += 8, D += 8) {
					int16x8x2_t sxy = vld2q_s16(&XY[dx * 2]);
					int32x4_t sxyidx_0123 = vmlal_s16(
							vmovl_s16(vget_low_s16(sxy.val[0])),
							vget_low_s16(sxy.val[1]),
							sstep_);

					int32x4_t sxyidx_3456 = vmlal_s16(
							vmovl_s16(vget_high_s16(sxy.val[0])),
							vget_high_s16(sxy.val[1]),
							sstep_);

					std::array<int16x4_t, 8> ss;
					for (auto i = 0; i < 4; ++i) {
						const T* S = S0 + sxyidx_0123[i];
						int16x4_t ssi = {
								S[0],
								S[1],
								S[sstep],
								S[sstep + 1],
						};
						ss[i] = ssi;
					}
					for (auto i = 0; i < 4; ++i) {
						const T* S = S0 + sxyidx_3456[i];
						int16x4_t ssi = {
								S[0],
								S[1],
								S[sstep],
								S[sstep + 1],
						};
						ss[4 + i] = ssi;
					}

					std::array<int16x4_t, 8> ws;
					for (u_int i = 0; i < ws.size(); ++i) {
						ws[i] = vld1_s16(&wtab[FXY[dx + i] * 4]);
					}

					int32x4_t interp0 = vmlal_s16(delta_4, ws[0], ss[0]);
					int32x4_t interp1 = vmlal_s16(delta_4, ws[1], ss[1]);
					int32x4_t interp2 = vmlal_s16(delta_4, ws[2], ss[2]);
					int32x4_t interp3 = vmlal_s16(delta_4, ws[3], ss[3]);
					int32x4_t interp4 = vmlal_s16(delta_4, ws[4], ss[4]);
					int32x4_t interp5 = vmlal_s16(delta_4, ws[5], ss[5]);
					int32x4_t interp6 = vmlal_s16(delta_4, ws[6], ss[6]);
					int32x4_t interp7 = vmlal_s16(delta_4, ws[7], ss[7]);

					int32x2_t interps0 =
							vpadd_s32(vget_low_s32(interp0), vget_high_s32(interp0));
					int32x2_t interps1 =
							vpadd_s32(vget_low_s32(interp1), vget_high_s32(interp1));
					int32x2_t interps2 =
							vpadd_s32(vget_low_s32(interp2), vget_high_s32(interp2));
					int32x2_t interps3 =
							vpadd_s32(vget_low_s32(interp3), vget_high_s32(interp3));
					int32x2_t interps4 =
							vpadd_s32(vget_low_s32(interp4), vget_high_s32(interp4));
					int32x2_t interps5 =
							vpadd_s32(vget_low_s32(interp5), vget_high_s32(interp5));
					int32x2_t interps6 =
							vpadd_s32(vget_low_s32(interp6), vget_high_s32(interp6));
					int32x2_t interps7 =
							vpadd_s32(vget_low_s32(interp7), vget_high_s32(interp7));

					int32x2_t interp01 = vpadd_s32(interps0, interps1);
					int32x2_t interp23 = vpadd_s32(interps2, interps3);
					int32x2_t interp45 = vpadd_s32(interps4, interps5);
					int32x2_t interp67 = vpadd_s32(interps6, interps7);

					int32x4_t interp0123 = vcombine_s32(interp01, interp23);
					int32x4_t interp4567 = vcombine_s32(interp45, interp67);

					interp0123 = vshrq_n_s32(interp0123, CastOp::SHIFT);
					interp4567 = vshrq_n_s32(interp4567, CastOp::SHIFT);
					int16x4_t sinterp0123 = vqmovn_s32(interp0123);
					int16x4_t sinterp4567 = vqmovn_s32(interp4567);
					uint8x8_t sinterp01234567 =
							vqmovun_s16(vcombine_s16(sinterp0123, sinterp4567));
					vst1_u8(D, sinterp01234567);
				}
#endif

				for (; dx < X1; dx++, D++) {
					int sx = XY[dx * 2], sy = XY[dx * 2 + 1];
					const AT* w = wtab + FXY[dx] * 4;
					const T* S = S0 + sy * sstep + sx;
					*D = castOp(
							WT(S[0] * w[0] + S[1] * w[1] + S[sstep] * w[2] +
								 S[sstep + 1] * w[3]));
				}
			} else {
				for (; dx < X1; dx++, D++) {
					int sx = XY[dx * 2], sy = XY[dx * 2 + 1];
					if (borderType == BORDER_CONSTANT &&
							(sx >= ssize.width || sx + 1 < 0 || sy >= ssize.height ||
							 sy + 1 < 0)) {
						D[0] = cval[0];
					} else {
						int sx0, sx1, sy0, sy1;
						T v0, v1, v2, v3;
						const AT* w = wtab + FXY[dx] * 4;
						sx0 = borderInterpolate(sx, ssize.width, borderType);
						sx1 = borderInterpolate(sx + 1, ssize.width, borderType);
						sy0 = borderInterpolate(sy, ssize.height, borderType);
						sy1 = borderInterpolate(sy + 1, ssize.height, borderType);
						v0 = sx0 >= 0 && sy0 >= 0 ? S0[sy0 * sstep + sx0] : cval[0];
						v1 = sx1 >= 0 && sy0 >= 0 ? S0[sy0 * sstep + sx1] : cval[0];
						v2 = sx0 >= 0 && sy1 >= 0 ? S0[sy1 * sstep + sx0] : cval[0];
						v3 = sx1 >= 0 && sy1 >= 0 ? S0[sy1 * sstep + sx1] : cval[0];
						D[0] = castOp(WT(v0 * w[0] + v1 * w[1] + v2 * w[2] + v3 * w[3]));
					}
				}
			}
		}
	}
}

void remap(
		const cv::Mat& src,
		cv::Mat* dstP,
		const cv::Mat& map1,
		const cv::Mat& map2,
		cv::Mat* buf,
		int interpolation,
		int borderType,
		const cv::Scalar& borderValue) {
	typedef void (*RemapNNFunc)(
			const Mat& _src,
			Mat& _dst,
			const Mat& _xy,
			int borderType,
			const cv::Scalar& _borderValue);

	typedef void (*RemapFunc)(
			const Mat& _src,
			Mat& _dst,
			const Mat& _xy,
			const Mat& _fxy,
			const void* _wtab,
			int borderType,
			const cv::Scalar& _borderValue);

	class RemapInvoker : public ParallelLoopBody {
	 public:
		RemapInvoker(
				const Mat& _src,
				Mat& _dst,
				const Mat* _m1,
				const Mat* _m2,
				Mat* _buf,
				int _borderType,
				const cv::Scalar& _borderValue,
				int _planar_input,
				RemapNNFunc _nnfunc,
				RemapFunc _ifunc,
				const void* _ctab)
				: ParallelLoopBody(),
					src(&_src),
					dst(&_dst),
					m1(_m1),
					m2(_m2),
					buf(_buf),
					borderType(_borderType),
					borderValue(_borderValue),
					planar_input(_planar_input),
					nnfunc(_nnfunc),
					ifunc(_ifunc),
					ctab(_ctab) {}

	void operator()(const Range& range) const override
	{
		int x, y, x1, y1;
		constexpr int buf_size = 1 << 14;
		int brows0 = std::min(128, dst->rows), map_depth = m1->depth();
		int bcols0 = std::min(buf_size / brows0, dst->cols);
		brows0 = std::min(buf_size / bcols0, dst->rows);

		Mat _bufxy(brows0, bcols0, CV_16SC2);
		if (!nnfunc)
			buf->create(brows0, bcols0, CV_16UC1);

		for (y = range.start; y < range.end; y += brows0)
		{
			for (x = 0; x < dst->cols; x += bcols0)
			{
				int brows = std::min(brows0, range.end - y);
				int bcols = std::min(bcols0, dst->cols - x);
				Mat dpart(*dst, Rect(x, y, bcols, brows));
				Mat bufxy(_bufxy, Rect(0, 0, bcols, brows));
				if (nnfunc)
				{
					if (m1->type() == CV_16SC2 &&
						m2->empty())
					{ // the data is already in the right format
						bufxy = (*m1)(Rect(x, y, bcols, brows));
					}
					else if (map_depth != CV_32F)
					{
						for (y1 = 0; y1 < brows; y1++)
						{
							short* XY = bufxy.ptr<short>(y1);
							const short* sXY = m1->ptr<short>(y + y1) + x * 2;
							const ushort* sA = m2->ptr<ushort>(y + y1) + x;
							for (x1 = 0; x1 < bcols; x1++)
							{
								int a = sA[x1] & (INTER_TAB_SIZE2 - 1);
								XY[x1 * 2] = sXY[x1 * 2] + NNDeltaTab_i[a][0];
								XY[x1 * 2 + 1] = sXY[x1 * 2 + 1] + NNDeltaTab_i[a][1];
							}
						}
					}
					else if (!planar_input)
					{
						(*m1)(Rect(x, y, bcols, brows)).convertTo(bufxy, bufxy.depth());
					}
					else
					{
						for (y1 = 0; y1 < brows; y1++)
						{
							short* XY = bufxy.ptr<short>(y1);
							const float* sX = m1->ptr<float>(y + y1) + x;
							const float* sY = m2->ptr<float>(y + y1) + x;
							x1 = 0;

#if defined(__SSSE3__)
								for (; x1 <= bcols - 8; x1 += 8) {
									__m128 fx0 = _mm_loadu_ps(sX + x1);
									__m128 fx1 = _mm_loadu_ps(sX + x1 + 4);
									__m128 fy0 = _mm_loadu_ps(sY + x1);
									__m128 fy1 = _mm_loadu_ps(sY + x1 + 4);
									__m128i ix0 = _mm_cvtps_epi32(fx0);
									__m128i ix1 = _mm_cvtps_epi32(fx1);
									__m128i iy0 = _mm_cvtps_epi32(fy0);
									__m128i iy1 = _mm_cvtps_epi32(fy1);
									ix0 = _mm_packs_epi32(ix0, ix1);
									iy0 = _mm_packs_epi32(iy0, iy1);
									ix1 = _mm_unpacklo_epi16(ix0, iy0);
									iy1 = _mm_unpackhi_epi16(ix0, iy0);
									_mm_storeu_si128((__m128i*)(XY + x1 * 2), ix1);
									_mm_storeu_si128((__m128i*)(XY + x1 * 2 + 8), iy1);
								}
#endif
								for (; x1 < bcols; x1++) {
									XY[x1 * 2] = saturate_cast<short>(sX[x1]);
									XY[x1 * 2 + 1] = saturate_cast<short>(sY[x1]);
								}
							}
						}
						nnfunc(*src, dpart, bufxy, borderType, borderValue);
						continue;
					}

					Mat bufa(*buf, Rect(0, 0, bcols, brows));
					for (y1 = 0; y1 < brows; y1++) {
						ushort* A = bufa.ptr<ushort>(y1);
						bufxy = (*m1)(Rect(x, y, bcols, brows));
						const ushort* sA = m2->ptr<ushort>(y + y1) + x;
						x1 = 0;

#if defined(__SSSE3__)
						__m128i v_scale = _mm_set1_epi16(INTER_TAB_SIZE2 - 1);
						for (; x1 <= bcols - 8; x1 += 8) {
							_mm_storeu_si128(
									(__m128i*)(A + x1),
									_mm_and_si128(
											_mm_loadu_si128((const __m128i*)(sA + x1)), v_scale));
						}

#elif defined(__ARM_NEON__) || defined(__ARM_NEON)
						uint16x8_t v_scale = vdupq_n_u16(INTER_TAB_SIZE2 - 1);
						for (; x1 <= bcols - 8; x1 += 8) {
							vst1q_u16(A + x1, vandq_u16(vld1q_u16(sA + x1), v_scale));
						}
#endif

						for (; x1 < bcols; x1++)
							A[x1] = (ushort)(sA[x1] & (INTER_TAB_SIZE2 - 1));
					}
					ifunc(*src, dpart, bufxy, bufa, ctab, borderType, borderValue);
				}
			}
	}

	 private:
		const Mat* src;
		Mat* dst;
		const Mat *m1, *m2;
		Mat* buf;
		int borderType;
		cv::Scalar borderValue;
		int planar_input;
		RemapNNFunc nnfunc;
		RemapFunc ifunc;
		const void* ctab;
	};

	struct RemapNoVec {
		int operator()(
				const Mat&,
				void*,
				const short*,
				const ushort*,
				const void*,
				int) const {
			return 0;
		}
	};

	struct RemapVec_8u {
		int operator()(
				const cv::Mat& _src,
				void* _dst,
				const short* XY,
				const ushort* FXY,
				const void* _wtab,
				int width) const {
			int cn = _src.channels(), x = 0, sstep = (int)_src.step;
			const uchar *S0 = _src.ptr(), *S1 = _src.ptr(1);
			const short* wtab = (const short*)_wtab;
			uchar* D = (uchar*)_dst;

#if defined(__SSSE3__)
			__m128i delta = _mm_set1_epi32(INTER_REMAP_COEF_SCALE / 2);
			__m128i xy2ofs = _mm_set1_epi32(cn + (sstep << 16));
			__m128i z = _mm_setzero_si128();
			int iofs0[4], iofs1[4];
			for (; x <= width - 8; x += 8) {
				__m128i xy0 = _mm_loadu_si128((const __m128i*)(XY + x * 2));
				__m128i xy1 = _mm_loadu_si128((const __m128i*)(XY + x * 2 + 8));
				__m128i v0, v1, v2, v3, a0, a1, b0, b1;
				unsigned i0, i1;

				xy0 = _mm_madd_epi16(xy0, xy2ofs);
				xy1 = _mm_madd_epi16(xy1, xy2ofs);
				_mm_store_si128((__m128i*)iofs0, xy0);
				_mm_store_si128((__m128i*)iofs1, xy1);

				i0 = *(ushort*)(S0 + iofs0[0]) + (*(ushort*)(S0 + iofs0[1]) << 16);
				i1 = *(ushort*)(S0 + iofs0[2]) + (*(ushort*)(S0 + iofs0[3]) << 16);
				v0 = _mm_unpacklo_epi32(_mm_cvtsi32_si128(i0), _mm_cvtsi32_si128(i1));
				i0 = *(ushort*)(S1 + iofs0[0]) + (*(ushort*)(S1 + iofs0[1]) << 16);
				i1 = *(ushort*)(S1 + iofs0[2]) + (*(ushort*)(S1 + iofs0[3]) << 16);
				v1 = _mm_unpacklo_epi32(_mm_cvtsi32_si128(i0), _mm_cvtsi32_si128(i1));
				v0 = _mm_unpacklo_epi8(v0, z);
				v1 = _mm_unpacklo_epi8(v1, z);

				a0 = _mm_unpacklo_epi32(
						_mm_loadl_epi64((__m128i*)(wtab + FXY[x] * 4)),
						_mm_loadl_epi64((__m128i*)(wtab + FXY[x + 1] * 4)));
				a1 = _mm_unpacklo_epi32(
						_mm_loadl_epi64((__m128i*)(wtab + FXY[x + 2] * 4)),
						_mm_loadl_epi64((__m128i*)(wtab + FXY[x + 3] * 4)));
				b0 = _mm_unpacklo_epi64(a0, a1);
				b1 = _mm_unpackhi_epi64(a0, a1);
				v0 = _mm_madd_epi16(v0, b0);
				v1 = _mm_madd_epi16(v1, b1);
				v0 = _mm_add_epi32(_mm_add_epi32(v0, v1), delta);

				i0 = *(ushort*)(S0 + iofs1[0]) + (*(ushort*)(S0 + iofs1[1]) << 16);
				i1 = *(ushort*)(S0 + iofs1[2]) + (*(ushort*)(S0 + iofs1[3]) << 16);
				v2 = _mm_unpacklo_epi32(_mm_cvtsi32_si128(i0), _mm_cvtsi32_si128(i1));
				i0 = *(ushort*)(S1 + iofs1[0]) + (*(ushort*)(S1 + iofs1[1]) << 16);
				i1 = *(ushort*)(S1 + iofs1[2]) + (*(ushort*)(S1 + iofs1[3]) << 16);
				v3 = _mm_unpacklo_epi32(_mm_cvtsi32_si128(i0), _mm_cvtsi32_si128(i1));
				v2 = _mm_unpacklo_epi8(v2, z);
				v3 = _mm_unpacklo_epi8(v3, z);

				a0 = _mm_unpacklo_epi32(
						_mm_loadl_epi64((__m128i*)(wtab + FXY[x + 4] * 4)),
						_mm_loadl_epi64((__m128i*)(wtab + FXY[x + 5] * 4)));
				a1 = _mm_unpacklo_epi32(
						_mm_loadl_epi64((__m128i*)(wtab + FXY[x + 6] * 4)),
						_mm_loadl_epi64((__m128i*)(wtab + FXY[x + 7] * 4)));
				b0 = _mm_unpacklo_epi64(a0, a1);
				b1 = _mm_unpackhi_epi64(a0, a1);
				v2 = _mm_madd_epi16(v2, b0);
				v3 = _mm_madd_epi16(v3, b1);
				v2 = _mm_add_epi32(_mm_add_epi32(v2, v3), delta);

				v0 = _mm_srai_epi32(v0, INTER_REMAP_COEF_BITS);
				v2 = _mm_srai_epi32(v2, INTER_REMAP_COEF_BITS);
				v0 = _mm_packus_epi16(_mm_packs_epi32(v0, v2), z);
				_mm_storel_epi64((__m128i*)(D + x), v0);
			}

#elif defined(__ARM_NEON__) || defined(__ARM_NEON)
			int32x4_t delta = vdupq_n_s32(INTER_REMAP_COEF_SCALE / 2);
			int32x4_t xy2ofs = vdupq_n_s32(cn + (sstep << 16));
			int32x4_t z = vdupq_n_s32(0);
			int CV_DECL_ALIGNED(16) iofs0[4], iofs1[4];

			for (; x <= width - 8; x += 8) {
				int32x4_t xy0 = vld1q_s32((int32_t*)(XY + x * 2));
				int32x4_t xy1 = vld1q_s32((int32_t*)(XY + x * 2 + 8));
				unsigned i0, i1;
				auto mm_madd_epi16 = [](int32x4_t a, int32x4_t b) -> int32x4_t {
					int16x8_t a_ = vreinterpretq_s16_s32(a);
					int16x8_t b_ = vreinterpretq_s16_s32(b);
					int32x4_t abl = vmull_s16(vget_low_s16(a_), vget_low_s16(b_));
					int32x4_t abh = vmull_s16(vget_high_s16(a_), vget_high_s16(b_));

					return vcombine_s32(
							vpadd_s32(vget_low_s32(abl), vget_high_s32(abl)),
							vpadd_s32(vget_low_s32(abh), vget_high_s32(abh)));
				};

				xy0 = mm_madd_epi16(xy0, xy2ofs);
				xy1 = mm_madd_epi16(xy1, xy2ofs);
				vst1q_s32(&iofs0[0], xy0);
				vst1q_s32(&iofs1[0], xy1);

				i0 = *(ushort*)(S0 + iofs0[0]) + (*(ushort*)(S0 + iofs0[1]) << 16);
				i1 = *(ushort*)(S0 + iofs0[2]) + (*(ushort*)(S0 + iofs0[3]) << 16);
				int32x4_t v0 = {int(i0), int(i1), 0, 0};

				i0 = *(ushort*)(S1 + iofs0[0]) + (*(ushort*)(S1 + iofs0[1]) << 16);
				i1 = *(ushort*)(S1 + iofs0[2]) + (*(ushort*)(S1 + iofs0[3]) << 16);
				int32x4_t v1 = {int(i0), int(i1), 0, 0};

				auto mm_unpacklo_epi8 = [](const int32x4_t a, const int32x4_t b) {
					return vreinterpretq_s32_u8(
							vzipq_u8(vreinterpretq_u8_s32(a), vreinterpretq_u8_s32(b))
									.val[0]);
				};

				v0 = mm_unpacklo_epi8(v0, z);
				v1 = mm_unpacklo_epi8(v1, z);

				auto mm_unpacklo_epi32 = [](int32x2_t a, int32x2_t b) -> int32x4_t {
					int32x2x2_t c = vzip_s32(a, b);
					return vcombine_s32(c.val[0], c.val[1]);
				};

				int32x4_t a0 = mm_unpacklo_epi32(
						vld1_s32(((int32_t*)(wtab + FXY[x + 0] * 4))),
						vld1_s32(((int32_t*)(wtab + FXY[x + 1] * 4))));

				int32x4_t a1 = mm_unpacklo_epi32(
						vld1_s32(((int32_t*)(wtab + FXY[x + 2] * 4))),
						vld1_s32(((int32_t*)(wtab + FXY[x + 3] * 4))));

				int32x4_t b0 = vcombine_s32(vget_low_s32(a0), vget_low_s32(a1));
				int32x4_t b1 = vcombine_s32(vget_high_s32(a0), vget_high_s32(a1));

				v0 = mm_madd_epi16(v0, b0);
				v1 = mm_madd_epi16(v1, b1);

				v0 = v0 + v1 + delta;

				i0 = *(ushort*)(S0 + iofs1[0]) + (*(ushort*)(S0 + iofs1[1]) << 16);
				i1 = *(ushort*)(S0 + iofs1[2]) + (*(ushort*)(S0 + iofs1[3]) << 16);
				int32x4_t v2 = {int(i0), int(i1), 0, 0};

				i0 = *(ushort*)(S1 + iofs1[0]) + (*(ushort*)(S1 + iofs1[1]) << 16);
				i1 = *(ushort*)(S1 + iofs1[2]) + (*(ushort*)(S1 + iofs1[3]) << 16);
				int32x4_t v3 = {int(i0), int(i1), 0, 0};
				v2 = mm_unpacklo_epi8(v2, z);
				v3 = mm_unpacklo_epi8(v3, z);

				a0 = mm_unpacklo_epi32(
						vld1_s32(((int32_t*)(wtab + FXY[x + 4] * 4))),
						vld1_s32(((int32_t*)(wtab + FXY[x + 5] * 4))));
				a1 = mm_unpacklo_epi32(
						vld1_s32(((int32_t*)(wtab + FXY[x + 6] * 4))),
						vld1_s32(((int32_t*)(wtab + FXY[x + 7] * 4))));

				b0 = vcombine_s32(vget_low_s32(a0), vget_low_s32(a1));
				b1 = vcombine_s32(vget_high_s32(a0), vget_high_s32(a1));

				v2 = mm_madd_epi16(v2, b0);
				v3 = mm_madd_epi16(v3, b1);

				v2 = v2 + v3 + delta;

				v0 = vshrq_n_s32(v0, INTER_REMAP_COEF_BITS);
				v2 = vshrq_n_s32(v2, INTER_REMAP_COEF_BITS);

				auto mm_packs_epi32 = [](int32x4_t a, int32x4_t b) -> int16x8_t {
					return vcombine_s16(vqmovn_s32(a), vqmovn_s32(b));
				};
				auto mm_packus_epi16 = [](int16x8_t a, int16x8_t b) -> uint8x16_t {
					return vcombine_u8(vqmovun_s16(a), vqmovun_s16(b));
				};
				v0 = vreinterpretq_s32_u8(
						mm_packus_epi16(mm_packs_epi32(v0, v2), vreinterpretq_s16_s32(z)));
				vst1_s32((int32_t*)(D + x), vget_low_s32(v0));
			}
#else
			(void)XY;
			(void)FXY;
			(void)width;
#endif
			return x;
		}
	};

	RemapFunc ifunc = remapBilinear<
			FixedPtCast<int, uchar, INTER_REMAP_COEF_BITS>,
			RemapVec_8u,
			short>;
	RemapNNFunc nnfunc =
			interpolation == INTER_NEAREST ? remapNearest<uchar> : nullptr;
	const void* ctab = initInterTab2D(interpolation, true /* fixedpoint */);
	int planar_input = 1;
	Mat& dst = *dstP;
	RemapInvoker invoker(
			src,
			dst,
			&map1,
			&map2,
			buf,
			borderType,
			borderValue,
			planar_input,
			nnfunc,
			ifunc,
			ctab);
	invoker(Range(0, dst.rows));
}

class WarpAffineInvoker : public ParallelLoopBody {
 public:
	WarpAffineInvoker(
			const Mat& _src,
			Mat& _dst,
			int _interpolation,
			int _borderType,
			const cv::Scalar& _borderValue,
			int* _adelta,
			int* _bdelta,
			double* _M)
			: ParallelLoopBody(),
				src(_src),
				dst(_dst),
				interpolation(_interpolation),
				borderType(_borderType),
				borderValue(_borderValue),
				adelta(_adelta),
				bdelta(_bdelta),
				M(_M) {}

	void operator()(const Range& range) const override
	{
		constexpr int BLOCK_SZ = 64;
		short XY[BLOCK_SZ * BLOCK_SZ * 2], A[BLOCK_SZ * BLOCK_SZ];
		int round_delta = interpolation == INTER_NEAREST
							? AB_SCALE / 2
							: AB_SCALE / INTER_TAB_SIZE / 2,
			x, y, x1, y1;
		Mat buf;
		int bh0 = std::min(BLOCK_SZ / 2, dst.rows);
		int bw0 = std::min(BLOCK_SZ * BLOCK_SZ / bh0, dst.cols);
		bh0 = std::min(BLOCK_SZ * BLOCK_SZ / bw0, dst.rows);

		for (y = range.start; y < range.end; y += bh0)
		{
			for (x = 0; x < dst.cols; x += bw0)
			{
				int bw = std::min(bw0, dst.cols - x);
				int bh = std::min(bh0, range.end - y);

				Mat _XY(bh, bw, CV_16SC2, XY), matA;
				Mat dpart(dst, Rect(x, y, bw, bh));

				for (y1 = 0; y1 < bh; y1++)
				{
					short* xy = XY + y1 * bw * 2;
					int X0 = saturate_cast<int>((M[1] * (y + y1) + M[2]) * AB_SCALE) +
							 round_delta;
					int Y0 = saturate_cast<int>((M[4] * (y + y1) + M[5]) * AB_SCALE) +
							 round_delta;
					if (interpolation == INTER_NEAREST)
					{
						x1 = 0;

#if defined(__SSSE3__)
						__m128i v_X0 = _mm_set1_epi32(X0);
						__m128i v_Y0 = _mm_set1_epi32(Y0);
						for (; x1 <= bw - 16; x1 += 16) {
							__m128i v_x0 = _mm_packs_epi32(
									_mm_srai_epi32(
											_mm_add_epi32(
													v_X0,
													_mm_loadu_si128((__m128i const*)(adelta + x + x1))),
											AB_BITS),
									_mm_srai_epi32(
											_mm_add_epi32(
													v_X0,
													_mm_loadu_si128(
															(__m128i const*)(adelta + x + x1 + 4))),
											AB_BITS));
							__m128i v_x1 = _mm_packs_epi32(
									_mm_srai_epi32(
											_mm_add_epi32(
													v_X0,
													_mm_loadu_si128(
															(__m128i const*)(adelta + x + x1 + 8))),
											AB_BITS),
									_mm_srai_epi32(
											_mm_add_epi32(
													v_X0,
													_mm_loadu_si128(
															(__m128i const*)(adelta + x + x1 + 12))),
											AB_BITS));

							__m128i v_y0 = _mm_packs_epi32(
									_mm_srai_epi32(
											_mm_add_epi32(
													v_Y0,
													_mm_loadu_si128((__m128i const*)(bdelta + x + x1))),
											AB_BITS),
									_mm_srai_epi32(
											_mm_add_epi32(
													v_Y0,
													_mm_loadu_si128(
															(__m128i const*)(bdelta + x + x1 + 4))),
											AB_BITS));
							__m128i v_y1 = _mm_packs_epi32(
									_mm_srai_epi32(
											_mm_add_epi32(
													v_Y0,
													_mm_loadu_si128(
															(__m128i const*)(bdelta + x + x1 + 8))),
											AB_BITS),
									_mm_srai_epi32(
											_mm_add_epi32(
													v_Y0,
													_mm_loadu_si128(
															(__m128i const*)(bdelta + x + x1 + 12))),
											AB_BITS));

							_mm_interleave_epi16(v_x0, v_x1, v_y0, v_y1);

							_mm_storeu_si128((__m128i*)(xy + x1 * 2), v_x0);
							_mm_storeu_si128((__m128i*)(xy + x1 * 2 + 8), v_x1);
							_mm_storeu_si128((__m128i*)(xy + x1 * 2 + 16), v_y0);
							_mm_storeu_si128((__m128i*)(xy + x1 * 2 + 24), v_y1);
						}

#elif defined(__ARM_NEON__) || defined(__ARM_NEON)
						int32x4_t v_X0 = vdupq_n_s32(X0), v_Y0 = vdupq_n_s32(Y0);
						for (; x1 <= bw - 8; x1 += 8) {
							int16x8x2_t v_dst;
							v_dst.val[0] = vcombine_s16(
									vqmovn_s32(vshrq_n_s32(
											vaddq_s32(v_X0, vld1q_s32(adelta + x + x1)), AB_BITS)),
									vqmovn_s32(vshrq_n_s32(
											vaddq_s32(v_X0, vld1q_s32(adelta + x + x1 + 4)),
											AB_BITS)));
							v_dst.val[1] = vcombine_s16(
									vqmovn_s32(vshrq_n_s32(
											vaddq_s32(v_Y0, vld1q_s32(bdelta + x + x1)), AB_BITS)),
									vqmovn_s32(vshrq_n_s32(
											vaddq_s32(v_Y0, vld1q_s32(bdelta + x + x1 + 4)),
											AB_BITS)));
							vst2q_s16(xy + (x1 << 1), v_dst);
						}
#endif

						for (; x1 < bw; x1++) {
							int X = (X0 + adelta[x + x1]) >> AB_BITS;
							int Y = (Y0 + bdelta[x + x1]) >> AB_BITS;
							xy[x1 * 2] = saturate_cast<short>(X);
							xy[x1 * 2 + 1] = saturate_cast<short>(Y);
						}
					} else {
						short* alpha = A + y1 * bw;
						x1 = 0;

#if defined(__SSSE3__)
						__m128i fxy_mask = _mm_set1_epi32(INTER_TAB_SIZE - 1);
						__m128i XX = _mm_set1_epi32(X0), YY = _mm_set1_epi32(Y0);
						for (; x1 <= bw - 8; x1 += 8) {
							__m128i tx0, tx1, ty0, ty1;
							tx0 = _mm_add_epi32(
									_mm_loadu_si128((const __m128i*)(adelta + x + x1)), XX);
							ty0 = _mm_add_epi32(
									_mm_loadu_si128((const __m128i*)(bdelta + x + x1)), YY);
							tx1 = _mm_add_epi32(
									_mm_loadu_si128((const __m128i*)(adelta + x + x1 + 4)), XX);
							ty1 = _mm_add_epi32(
									_mm_loadu_si128((const __m128i*)(bdelta + x + x1 + 4)), YY);

							tx0 = _mm_srai_epi32(tx0, AB_BITS - INTER_BITS);
							ty0 = _mm_srai_epi32(ty0, AB_BITS - INTER_BITS);
							tx1 = _mm_srai_epi32(tx1, AB_BITS - INTER_BITS);
							ty1 = _mm_srai_epi32(ty1, AB_BITS - INTER_BITS);

							__m128i fx_ = _mm_packs_epi32(
									_mm_and_si128(tx0, fxy_mask), _mm_and_si128(tx1, fxy_mask));
							__m128i fy_ = _mm_packs_epi32(
									_mm_and_si128(ty0, fxy_mask), _mm_and_si128(ty1, fxy_mask));
							tx0 = _mm_packs_epi32(
									_mm_srai_epi32(tx0, INTER_BITS),
									_mm_srai_epi32(tx1, INTER_BITS));
							ty0 = _mm_packs_epi32(
									_mm_srai_epi32(ty0, INTER_BITS),
									_mm_srai_epi32(ty1, INTER_BITS));
							fx_ = _mm_adds_epi16(fx_, _mm_slli_epi16(fy_, INTER_BITS));

							_mm_storeu_si128(
									(__m128i*)(xy + x1 * 2), _mm_unpacklo_epi16(tx0, ty0));
							_mm_storeu_si128(
									(__m128i*)(xy + x1 * 2 + 8), _mm_unpackhi_epi16(tx0, ty0));
							_mm_storeu_si128((__m128i*)(alpha + x1), fx_);
						}

#elif defined(__ARM_NEON__) || defined(__ARM_NEON)
						int32x4_t v__X0 = vdupq_n_s32(X0), v__Y0 = vdupq_n_s32(Y0),
											v_mask = vdupq_n_s32(INTER_TAB_SIZE - 1);
						for (; x1 <= bw - 8; x1 += 8) {
							int32x4_t v_X0 = vshrq_n_s32(
									vaddq_s32(v__X0, vld1q_s32(adelta + x + x1)),
									AB_BITS - INTER_BITS);
							int32x4_t v_Y0 = vshrq_n_s32(
									vaddq_s32(v__Y0, vld1q_s32(bdelta + x + x1)),
									AB_BITS - INTER_BITS);
							int32x4_t v_X1 = vshrq_n_s32(
									vaddq_s32(v__X0, vld1q_s32(adelta + x + x1 + 4)),
									AB_BITS - INTER_BITS);
							int32x4_t v_Y1 = vshrq_n_s32(
									vaddq_s32(v__Y0, vld1q_s32(bdelta + x + x1 + 4)),
									AB_BITS - INTER_BITS);

							int16x8x2_t v_xy;
							v_xy.val[0] = vcombine_s16(
									vqmovn_s32(vshrq_n_s32(v_X0, INTER_BITS)),
									vqmovn_s32(vshrq_n_s32(v_X1, INTER_BITS)));
							v_xy.val[1] = vcombine_s16(
									vqmovn_s32(vshrq_n_s32(v_Y0, INTER_BITS)),
									vqmovn_s32(vshrq_n_s32(v_Y1, INTER_BITS)));

							vst2q_s16(xy + (x1 << 1), v_xy);

							int16x4_t v_alpha0 = vmovn_s32(vaddq_s32(
									vshlq_n_s32(vandq_s32(v_Y0, v_mask), INTER_BITS),
									vandq_s32(v_X0, v_mask)));
							int16x4_t v_alpha1 = vmovn_s32(vaddq_s32(
									vshlq_n_s32(vandq_s32(v_Y1, v_mask), INTER_BITS),
									vandq_s32(v_X1, v_mask)));
							vst1q_s16(alpha + x1, vcombine_s16(v_alpha0, v_alpha1));
						}
#endif

						for (; x1 < bw; x1++) {
							int X = (X0 + adelta[x + x1]) >> (AB_BITS - INTER_BITS);
							int Y = (Y0 + bdelta[x + x1]) >> (AB_BITS - INTER_BITS);
							xy[x1 * 2] = saturate_cast<short>(X >> INTER_BITS);
							xy[x1 * 2 + 1] = saturate_cast<short>(Y >> INTER_BITS);
							alpha[x1] =
									(short)((Y & (INTER_TAB_SIZE - 1)) * INTER_TAB_SIZE + (X & (INTER_TAB_SIZE - 1)));
						}
					}
				}

				if (interpolation == INTER_NEAREST) {
					remap(
							src,
							&dpart,
							_XY,
							Mat(),
							&buf,
							interpolation,
							borderType,
							borderValue);
				} else {
					Mat _matA(bh, bw, CV_16U, A);
					remap(
							src,
							&dpart,
							_XY,
							_matA,
							&buf,
							interpolation,
							borderType,
							borderValue);
				}
			}
		}
	}

 private:
	Mat src;
	Mat dst;
	int interpolation, borderType;
	cv::Scalar borderValue;
	int *adelta, *bdelta;
	double* M;
};
} // namespace

void TestFrameInterpolatorNearestPixel::amlFacetrackerWarpAffine(const cv::Mat& src, cv::Mat& dstP, const cv::Mat& M0, cv::Size dsize, int interpolation)
{
	dstP.create(dsize.area() == 0 ? src.size() : dsize, src.type());
	Mat& dst = dstP;
	CV_Assert(src.cols > 0 && src.rows > 0);

	constexpr static int borderType = BORDER_CONSTANT;
	const static cv::Scalar borderValue = cv::Scalar();
	double M[6];
	Mat matM(2, 3, CV_64F, M);
	int flags = interpolation & INTER_MAX;

	CV_Assert(
			(M0.type() == CV_32F || M0.type() == CV_64F) && M0.rows == 2 &&
			M0.cols == 3);
	M0.convertTo(matM, matM.type());

	double D = M[0] * M[4] - M[1] * M[3];
	D = D != 0 ? 1. / D : 0;
	double A11 = M[4] * D, A22 = M[0] * D;
	M[0] = A11;
	M[1] *= -D;
	M[3] *= -D;
	M[4] = A22;
	double b1 = -M[0] * M[2] - M[1] * M[5];
	double b2 = -M[3] * M[2] - M[4] * M[5];
	M[2] = b1;
	M[5] = b2;

	int x_tmp;
	AutoBuffer<int, 2048> _abdelta(dst.cols * 2);
	int *adelta = &_abdelta[0], *bdelta = adelta + dst.cols;

	for (x_tmp = 0; x_tmp < dst.cols; x_tmp++) {
		adelta[x_tmp] = saturate_cast<int>(M[0] * x_tmp * AB_SCALE);
		bdelta[x_tmp] = saturate_cast<int>(M[3] * x_tmp * AB_SCALE);
	}

	WarpAffineInvoker invoker(
			src, dst, flags, borderType, borderValue, adelta, bdelta, M);
	Range range(0, dst.rows);
	invoker(range);
}

} // namespace TestOpenCV

} // mamespace TestCV

} // namespace Test

} // namespace Ocean
