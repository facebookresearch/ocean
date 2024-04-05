// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/test/testcv/testopencv/TestFramePyramid.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/String.h"
#include "ocean/base/Timestamp.h"

#include "ocean/cv/FramePyramid.h"
#include "ocean/cv/FrameShrinker.h"
#include "ocean/cv/OpenCVUtilities.h"

#include "ocean/test/testcv/TestFrameShrinker.h"

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

	double amlAverageAbsError14641 = 0.0;
	double amlMaximalAbsError14641 = 0.0;

	double opencvAverageAbsError14641 = 0.0;
	double opencvMaximalAbsError14641 = 0.0;

	unsigned int iteration = 0u;

	HighPerformanceStatistic performanceOcean11;
	HighPerformanceStatistic performanceOcean14641;
	HighPerformanceStatistic performanceAML14641;
	HighPerformanceStatistic performanceOpenCV14641;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int sourcePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
		const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

		Frame sourceFrame(FrameType(width, height, FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, channels), FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
		Frame targetFrame(FrameType(sourceFrame, width / 2u, height / 2u), targetPaddingElements);

		CV::CVUtilities::randomizeFrame(sourceFrame, false, &randomGenerator);
		CV::CVUtilities::randomizeFrame(targetFrame, false, &randomGenerator);

		cv::Mat cvSourceFrame;
		cv::Mat cvTargetFrame;

		if (iteration % 4u == 0u)
		{
			performanceOcean11.start();
			CV::FrameShrinker::downsampleByTwo11(sourceFrame, targetFrame);
			performanceOcean11.stop();
		}
		else if (iteration % 4u == 1u)
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
		else if (iteration % 4u == 2u)
		{
			if (channels == 1u)
			{
				CV::OpenCVUtilities::toCvMat(sourceFrame).copyTo(cvSourceFrame);
				CV::OpenCVUtilities::toCvMat(targetFrame).copyTo(cvTargetFrame);

				performanceAML14641.start();
				pyrDown(cvSourceFrame, cvTargetFrame, 1u);
				performanceAML14641.stop();

				sourceFrame = CV::OpenCVUtilities::toOceanFrame(cvSourceFrame);
				targetFrame = CV::OpenCVUtilities::toOceanFrame(cvTargetFrame);

				double averageAbsError = NumericD::maxValue();
				double maximalAbsError = NumericD::maxValue();
				validateDownsamplingByTwo8Bit14641(sourceFrame.constdata<uint8_t>(), targetFrame.constdata<uint8_t>(), sourceFrame.width(), sourceFrame.height(), targetFrame.width(), targetFrame.height(), sourceFrame.channels(), sourceFrame.paddingElements(), targetFrame.paddingElements(), &averageAbsError, &maximalAbsError);

				amlAverageAbsError14641 += averageAbsError;
				amlMaximalAbsError14641 = std::max(amlMaximalAbsError14641, maximalAbsError);
			}
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

		iteration++;
	}
	while (iteration < 4u || startTimestamp + testDuration > Timestamp(true));

	if (performanceAML14641.measurements() != 0u)
	{
		Log::info() << "Performance AML (14641): [" << String::toAString(performanceAML14641.bestMseconds(), 3u) << ", " << String::toAString(performanceAML14641.medianMseconds(), 3u) << ", " << String::toAString(performanceAML14641.worstMseconds(), 3u) << "] ms";
	}

	Log::info() << "Performance OpenCV (14641): [" << String::toAString(performanceOpenCV14641.bestMseconds(), 3u) << ", " << String::toAString(performanceOpenCV14641.medianMseconds(), 3u) << ", " << String::toAString(performanceOpenCV14641.worstMseconds(), 3u) << "] ms";
	Log::info() << "Performance Ocean (14641): [" << String::toAString(performanceOcean14641.bestMseconds(), 3u) << ", " << String::toAString(performanceOcean14641.medianMseconds(), 3u) << ", " << String::toAString(performanceOcean14641.worstMseconds(), 3u) << "] ms";
	Log::info() << "Performance Ocean (11): [" << String::toAString(performanceOcean11.bestMseconds(), 3u) << ", " << String::toAString(performanceOcean11.medianMseconds(), 3u) << ", " << String::toAString(performanceOcean11.worstMseconds(), 3u) << "] ms";

	if (performanceAML14641.measurements() != 0u)
	{
		Log::info() << "Ocean vs. AML:";
		Log::info() << "Performance ratio (14641 vs 14641): [" << String::toAString(performanceAML14641.best() / performanceOcean14641.best(), 2u) << ", "<< String::toAString(performanceAML14641.median() / performanceOcean14641.median(), 2u) << ", "<< String::toAString(performanceAML14641.worst() / performanceOcean14641.worst(), 2u) << "] x";
	}

	Log::info() << "Ocean vs. OpenCV:";
	Log::info() << "Performance ratio (14641 vs 14641): [" << String::toAString(performanceOpenCV14641.best() / performanceOcean14641.best(), 2u) << ", "<< String::toAString(performanceOpenCV14641.median() / performanceOcean14641.median(), 2u) << ", "<< String::toAString(performanceOpenCV14641.worst() / performanceOcean14641.worst(), 2u) << "] x";
	Log::info() << "Performance ratio (11 vs 14641): [" << String::toAString(performanceOpenCV14641.best() / performanceOcean11.best(), 2u) << ", "<< String::toAString(performanceOpenCV14641.median() / performanceOcean11.median(), 2u) << ", "<< String::toAString(performanceOpenCV14641.worst() / performanceOcean11.worst(), 2u) << "] x";

	Log::info() << "Validation: ";

	if (performanceAML14641.measurements() != 0u)
	{
		Log::info() << "AML 14641 average: " << String::toAString(amlAverageAbsError14641 / double(performanceAML14641.measurements()), 2u) << ", maximal: " << String::toAString(amlMaximalAbsError14641, 2u);
	}

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
		const unsigned int framePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

		Frame frame(FrameType(width, height, FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, channels), FrameType::ORIGIN_UPPER_LEFT), framePaddingElements);
		cv::Mat cvFrame;

		CV::CVUtilities::randomizeFrame(frame, false, &randomGenerator);

		if (iteration % 3u == 0u)
		{
			performanceOcean11.start();
			CV::FramePyramid framePyramid(frame, layers, nullptr, CV::FramePyramid::DM_FILTER_11);
			performanceOcean11.stop();

			ocean_assert(framePyramid.layers() == layers);
		}
		else if (iteration % 3u == 1u)
		{
			performanceOcean14641.start();
			CV::FramePyramid framePyramid(frame, layers, nullptr, CV::FramePyramid::DM_FILTER_14641);
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

// The following code is copied form AML's FaceTracker
// Source: https://phabricator.intern.facebook.com/diffusion/FBS/browse/master/xplat/arfx/tracking/facetracker/vision/ImagePyramid.cpp
// e.g., D8197311

ptrdiff_t borderInterpolate(ptrdiff_t p, ptrdiff_t len);

#if defined(__ARM_NEON__) || defined(__ARM_NEON)
inline void prefetch(const void* ptr, size_t offset = 32 * 10) {
#if defined(__GNUC__)
  __builtin_prefetch(reinterpret_cast<const char*>(ptr) + offset);
#elif defined(_MSC_VER)
  __prefetch(reinterpret_cast<const char*>(ptr) + offset);
#endif
}
#endif

template <typename T>
inline T* getRowPtr(T* base, ptrdiff_t stride, size_t row) {
  char* baseRaw = const_cast<char*>(reinterpret_cast<const char*>(base));
  return reinterpret_cast<T*>(baseRaw + ptrdiff_t(row) * stride);
}

template <typename T>
inline T* alignPtr(T* ptr, size_t n = sizeof(T)) {
  return (T*)(((size_t)ptr + n - 1) & -n);
}

ptrdiff_t borderInterpolate(ptrdiff_t p, ptrdiff_t len) {
  if (len == 1) {
    return 0;
  } else {
    while ((unsigned)p >= (unsigned)len) {
      if (p < 0) {
        p = -p;
      } else {
        p = (len - 1) * 2 - p;
      }
    }
  }
  return p;
}

/*
 * PyrDown is the pyramid downscaling operation by convoluting with a Gaussian
 * kernel of size 5x5 and removing the even numbered rows and columns. This
 * implementation specifically works for CV_8U type images. More details and
 * original implementation can be found here:
 * http://docs.opencv.org/2.4/doc/tutorials/imgproc/pyramids/pyramids.html
 */
void TestFramePyramid::pyrDown(const cv::Mat& _src, cv::Mat& _dst, uint8_t cn) {
  const uint8_t* srcBase = _src.data;
  uint8_t* dstBase = _dst.data;
  cv::Size srcSize = _src.size();
  cv::Size dstSize = _dst.size();
  ptrdiff_t srcStride = _src.step;
  ptrdiff_t dstStride = _dst.step;

  size_t dcolcn = dstSize.width * cn;
  size_t scolcn = srcSize.width * cn;

  size_t idx_l1 = borderInterpolate(-1, srcSize.width) * cn;
  size_t idx_l2 = borderInterpolate(-2, srcSize.width) * cn;
  size_t idx_r1 = borderInterpolate(srcSize.width + 0, srcSize.width) * cn;
  size_t idx_r2 = borderInterpolate(srcSize.width + 1, srcSize.width) * cn;

  // Initialize buffer
  std::vector<uint16_t> _buf(cn * (srcSize.width + 4) + 32 / sizeof(uint16_t));
  uint16_t* lane = alignPtr(&_buf[2 * cn], 32);

#if defined(__ARM_NEON__) || defined(__ARM_NEON)
  uint8x8_t vc6u8 = vmov_n_u8(6);
  uint16x8_t vc6u16 = vmovq_n_u16(6);
  uint16x8_t vc4u16 = vmovq_n_u16(4);
#endif

  for (int i = 0; i < dstSize.height; ++i) {
    uint8_t* dst = getRowPtr(dstBase, dstStride, i);

    // vertical convolution
    const uint8_t* ln0 = getRowPtr(
        srcBase, srcStride, borderInterpolate(i * 2 - 2, srcSize.height));
    const uint8_t* ln1 = getRowPtr(
        srcBase, srcStride, borderInterpolate(i * 2 - 1, srcSize.height));
    const uint8_t* ln2 = getRowPtr(
        srcBase, srcStride, borderInterpolate(i * 2 + 0, srcSize.height));
    const uint8_t* ln3 = getRowPtr(
        srcBase, srcStride, borderInterpolate(i * 2 + 1, srcSize.height));
    const uint8_t* ln4 = getRowPtr(
        srcBase, srcStride, borderInterpolate(i * 2 + 2, srcSize.height));

    int x = 0;
#if defined(__ARM_NEON__) || defined(__ARM_NEON)
    for (; x <= int(scolcn) - 8; x += 8) {
      prefetch(getRowPtr(ln2 + x, srcStride, (ptrdiff_t)x % 5 - 2));
      uint8x8_t v0 = vld1_u8(ln0 + x);
      uint8x8_t v1 = vld1_u8(ln1 + x);
      uint8x8_t v2 = vld1_u8(ln2 + x);
      uint8x8_t v3 = vld1_u8(ln3 + x);
      uint8x8_t v4 = vld1_u8(ln4 + x);

      uint16x8_t v = vaddl_u8(v0, v4);
      uint16x8_t v13 = vaddl_u8(v1, v3);

      v = vmlal_u8(v, v2, vc6u8);
      v = vmlaq_u16(v, v13, vc4u16);

      vst1q_u16(lane + x, v);
    }
#endif
    for (; x < int(scolcn); ++x) {
      lane[x] = ln0[x] + ln4[x] + 4u * (ln1[x] + ln3[x]) + 6u * ln2[x];
    }

    // left & right borders
    for (uint32_t k = 0; k < cn; ++k) {
      lane[(int32_t)(-cn + k)] = lane[idx_l1 + k];
      lane[(int32_t)(-cn - cn + k)] = lane[idx_l2 + k];

      lane[scolcn + k] = lane[idx_r1 + k];
      lane[scolcn + cn + k] = lane[idx_r2 + k];
    }

    // horizontal convolution
    x = 0;
#if defined(__ARM_NEON__) || defined(__ARM_NEON)
    for (; x < int(dcolcn) - 8; x += 8) {
      prefetch(lane + 2 * x);
#if !defined(__aarch64__) && defined(__GNUC__) && __GNUC__ == 4 && \
    __GNUC_MINOR__ < 7 && !defined(__clang__)
      __asm__ __volatile(
          "vld2.16 {d0-d3}, [%[in0]]                               \n\t"
          "vld2.16 {d4-d7}, [%[in4]]                               \n\t"
          "vld2.16 {d12-d15}, [%[in1]]                             \n\t"
          "vld2.16 {d16-d19}, [%[in3]]                             \n\t"
          "vld2.16 {d8-d11}, [%[in2],:256]                         \n\t"
          "vadd.i16 q0, q2                  /*q0 = v0 + v4*/       \n\t"
          "vadd.i16 q6, q8                  /*q6 = v1 + v3*/       \n\t"
          "vmla.i16 q0, q4, %q[c6]          /*q0 += v2 * 6*/       \n\t"
          "vmla.i16 q0, q6, %q[c4]          /*q1 += (v1+v3) * 4*/  \n\t"
          "vrshrn.u16 d8, q0, #8                                   \n\t"
          "vst1.8 {d8}, [%[out]]                                   \n\t"
          : /*no output*/
          : [out] "r"(dst + x),
            [in0] "r"(lane + 2 * x - 2),
            [in1] "r"(lane + 2 * x - 1),
            [in2] "r"(lane + 2 * x + 0),
            [in3] "r"(lane + 2 * x + 1),
            [in4] "r"(lane + 2 * x + 2),
            [c4] "w"(vc4u16),
            [c6] "w"(vc6u16)
          : "d0",
            "d1",
            "d2",
            "d3",
            "d4",
            "d5",
            "d6",
            "d7",
            "d8",
            "d9",
            "d10",
            "d11",
            "d12",
            "d13",
            "d14",
            "d15",
            "d16",
            "d17",
            "d18",
            "d19");
#else
      uint16x8x2_t vLane0 = vld2q_u16(lane + 2 * x - 2);
      uint16x8x2_t vLane1 = vld2q_u16(lane + 2 * x - 1);
      uint16x8x2_t vLane2 = vld2q_u16(lane + 2 * x + 0);
      uint16x8x2_t vLane3 = vld2q_u16(lane + 2 * x + 1);
      uint16x8x2_t vLane4 = vld2q_u16(lane + 2 * x + 2);

      uint16x8_t vSum_0_4 = vaddq_u16(vLane0.val[0], vLane4.val[0]);
      uint16x8_t vSum_1_3 = vaddq_u16(vLane1.val[0], vLane3.val[0]);
      vSum_0_4 = vmlaq_u16(vSum_0_4, vLane2.val[0], vc6u16);
      vSum_0_4 = vmlaq_u16(vSum_0_4, vSum_1_3, vc4u16);
      uint8x8_t vRes = vrshrn_n_u16(vSum_0_4, 8);

      vst1_u8(dst + x, vRes);
#endif
    }
#endif

    for (; x < int(dcolcn); x++) {
      dst[x] = (uint8_t)(
          (lane[2 * x - 2] + lane[2 * x + 2] +
           4u * (lane[2 * x - 1] + lane[2 * x + 1]) + 6u * lane[2 * x] +
           (1 << 7)) >>
          8);
    }
  }
}

}

}

}

}
