/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testopencv/TestFrameConverter.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/OpenCVUtilities.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestOpenCV
{

void TestFrameConverter::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Frame converter test:   ---";
	Log::info() << " ";

	testCast(testDuration);

	Log::info() << " ";

	Log::info() << "Frame converter test finished.";
}

void TestFrameConverter::testCast(const double testDuration)
{
	Log::info() << "Testing frame cast 'unsigned char' to 'float':";
	Log::info() << " ";

	const unsigned int widths[] = {160u, 320u, 640u, 800u, 1280u, 1920u, 3840u};
	const unsigned int heights[] = {120u, 240u, 480u, 640u,  720u, 1080u, 2160u};

	for (unsigned int n = 0u; n < sizeof(widths) / sizeof(widths[0]); ++n)
	{
		Log::info().newLine(n != 0u);

		const unsigned int width = widths[n];
		const unsigned int height = heights[n];

		Log::info() << "For image resolution " << width << "x" << height << ":";
		Log::info() << " ";

		for (unsigned int channels = 1u; channels <= 4u; ++channels)
		{
			testCast(width, height, channels, testDuration);
			Log::info() << " ";
		}
	}

	Log::info() << "Frame cast test finished.";
}

void TestFrameConverter::testCast(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration)
{
	ocean_assert(width >= 2u && height >= 2u);
	ocean_assert(channels >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... with " << channels << " channels:";
	Log::info() << " ";

	Frame sourceFrame(FrameType(width, height, FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, channels), FrameType::ORIGIN_UPPER_LEFT));
	Frame targetFrame(FrameType(sourceFrame, FrameType::genericPixelFormat(FrameType::DT_SIGNED_FLOAT_32, channels)));

	cv::Mat cvSourceFrame;
	cv::Mat cvTargetFrame;

	unsigned int iteration = 0u;

	HighPerformanceStatistic performanceOcean;
	HighPerformanceStatistic performanceAML;
	HighPerformanceStatistic performanceOpenCV;

	const Timestamp startTimestamp(true);

	do
	{
		CV::CVUtilities::randomizeFrame(sourceFrame);
		CV::CVUtilities::randomizeFrame(targetFrame);

		if (iteration % 3u == 0u)
		{
			performanceOcean.start();
			CV::FrameConverter::cast<uint8_t, float>(sourceFrame.constdata<uint8_t>(), targetFrame.data<float>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.channels(), sourceFrame.paddingElements(), targetFrame.paddingElements());
			performanceOcean.stop();
		}
		else if (iteration % 3u == 1u)
		{
			if (channels == 1u)
			{
				CV::OpenCVUtilities::toCvMat(sourceFrame).copyTo(cvSourceFrame);
				CV::OpenCVUtilities::toCvMat(targetFrame).copyTo(cvTargetFrame);

				performanceAML.start();
				convert(cvSourceFrame, cvTargetFrame, CV_32F);
				performanceAML.stop();
			}
		}
		else
		{
			CV::OpenCVUtilities::toCvMat(sourceFrame).copyTo(cvSourceFrame);
			CV::OpenCVUtilities::toCvMat(targetFrame).copyTo(cvTargetFrame);

			performanceOpenCV.start();
			cvSourceFrame.convertTo(cvTargetFrame, CV_MAKETYPE(CV_32F, channels));
			performanceOpenCV.stop();
		}

		iteration++;
	}
	while (iteration < 3u || startTimestamp + testDuration > Timestamp(true));

	if (performanceAML.measurements() != 0u)
	{
		Log::info() << "Performance AML: [" << String::toAString(performanceAML.bestMseconds(), 3u) << ", " << String::toAString(performanceAML.medianMseconds(), 3u) << ", " << String::toAString(performanceAML.worstMseconds(), 3u) << "] ms";
	}

	Log::info() << "Performance OpenCV: [" << String::toAString(performanceOpenCV.bestMseconds(), 3u) << ", " << String::toAString(performanceOpenCV.medianMseconds(), 3u) << ", " << String::toAString(performanceOpenCV.worstMseconds(), 3u) << "] ms";
	Log::info() << "Performance Ocean: [" << String::toAString(performanceOcean.bestMseconds(), 3u) << ", " << String::toAString(performanceOcean.medianMseconds(), 3u) << ", " << String::toAString(performanceOcean.worstMseconds(), 3u) << "] ms";

	if (performanceAML.measurements() != 0u)
	{
		Log::info() << "Ocean vs. AML:";
		Log::info() << "Performance ratio: [" << String::toAString(performanceAML.best() / performanceOcean.best(), 2u) << ", "<< String::toAString(performanceAML.median() / performanceOcean.median(), 2u) << ", "<< String::toAString(performanceAML.worst() / performanceOcean.worst(), 2u) << "] x";
	}

	Log::info() << "Ocean vs. OpenCV:";
	Log::info() << "Performance ratio: [" << String::toAString(performanceOpenCV.best() / performanceOcean.best(), 2u) << ", "<< String::toAString(performanceOpenCV.median() / performanceOcean.median(), 2u) << ", "<< String::toAString(performanceOpenCV.worst() / performanceOcean.worst(), 2u) << "] x";
}

// The following code is copied form AML's FaceTracker
// Source: https://phabricator.intern.facebook.com/diffusion/FBS/browse/master/xplat/arfx/tracking/facetracker/vision/ConvertImage.cpp
// e.g., D8197311

#if defined(__ARM_NEON__) || defined(__ARM_NEON)
inline void prefetch(const void* ptr, size_t offset = 32 * 10) {
#if defined(__GNUC__)
  __builtin_prefetch(reinterpret_cast<const char*>(ptr) + offset);
#elif defined(_MSC_VER)
  __prefetch(reinterpret_cast<const char*>(ptr) + offset);
#endif
}
#endif

static void convert_(
    const uint8_t* srcBase,
    const size_t width,
    const size_t height,
    ptrdiff_t srcStride,
    float* dstBase,
    ptrdiff_t dstStride) {
  const ptrdiff_t sstep = srcStride / sizeof(uint8_t);
  const ptrdiff_t dstep = dstStride / sizeof(float);
  const uint8_t* _src = srcBase;
  float* _dst = dstBase;

  for (ptrdiff_t h = height; h--; _src += sstep, _dst += dstep) {
    size_t i = 0;

#if defined(__ARM_NEON__) || defined(__ARM_NEON)
#if !defined(__aarch64__) && defined(__GNUC__) && __GNUC__ == 4 && \
    __GNUC_MINOR__ < 6 && !defined(__clang__)
    for (; i + 16 < width; i += 16) {
      prefetch(_src + i);
      __asm__(
          "vld1.8 {d0-d1}, [%[src]]                              \n\t"
          "vmovl.u8 q1, d0                                       \n\t"
          "vmovl.u8 q2, d1                                       \n\t"
          "vmovl.u16 q3, d2                                      \n\t"
          "vmovl.u16 q4, d3                                      \n\t"
          "vmovl.u16 q5, d4                                      \n\t"
          "vmovl.u16 q6, d5                                      \n\t"
          "vcvt.f32.u32 q7, q3                                   \n\t"
          "vcvt.f32.u32 q8, q4                                   \n\t"
          "vcvt.f32.u32 q9, q5                                   \n\t"
          "vcvt.f32.u32 q10, q6                                  \n\t"
          "vst1.32 {d14-d15}, [%[dst1]]                          \n\t"
          "vst1.32 {d16-d17}, [%[dst2]]                          \n\t"
          "vst1.32 {d18-d19}, [%[dst3]]                          \n\t"
          "vst1.32 {d20-d21}, [%[dst4]]                          \n\t"
          : /*no output*/
          : [src] "r"(_src + i),
            [dst1] "r"(_dst + i + 0),
            [dst2] "r"(_dst + i + 4),
            [dst3] "r"(_dst + i + 8),
            [dst4] "r"(_dst + i + 12)
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
            "d19",
            "d20",
            "d21");
    }
#else
    for (; i + 16 < width; i += 16) {
      prefetch(_src + i);
      uint8x16_t vline_u8 = vld1q_u8(_src + i);

      uint16x8_t vline1_u16 = vmovl_u8(vget_low_u8(vline_u8));
      uint16x8_t vline2_u16 = vmovl_u8(vget_high_u8(vline_u8));

      uint32x4_t vline1_u32 = vmovl_u16(vget_low_u16(vline1_u16));
      uint32x4_t vline2_u32 = vmovl_u16(vget_high_u16(vline1_u16));
      uint32x4_t vline3_u32 = vmovl_u16(vget_low_u16(vline2_u16));
      uint32x4_t vline4_u32 = vmovl_u16(vget_high_u16(vline2_u16));

      float32x4_t vline1_f32 = vcvtq_f32_u32(vline1_u32);
      float32x4_t vline2_f32 = vcvtq_f32_u32(vline2_u32);
      float32x4_t vline3_f32 = vcvtq_f32_u32(vline3_u32);
      float32x4_t vline4_f32 = vcvtq_f32_u32(vline4_u32);

      vst1q_f32(_dst + i, vline1_f32);
      vst1q_f32(_dst + i + 4, vline2_f32);
      vst1q_f32(_dst + i + 8, vline3_f32);
      vst1q_f32(_dst + i + 12, vline4_f32);
    }
#endif
#endif

    for (; i < width; i++) {
      _dst[i] = cv::saturate_cast<float>(_src[i]);
    }
  }
}

void TestFrameConverter::convert(cv::Mat& _src, cv::Mat& _dst, int _type) {
  cv::Mat src = _src;
  _type = CV_MAKETYPE(CV_MAT_DEPTH(_type), src.channels());
  _dst.create(src.size(), _type);
  convert_(
      src.data, src.cols, src.rows, src.step, (float*)_dst.data, _dst.step);
}

}

}

}

}
