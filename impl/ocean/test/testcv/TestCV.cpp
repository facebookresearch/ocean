/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestCV.h"
#include "ocean/test/testcv/TestBresenham.h"
#include "ocean/test/testcv/TestCanvas.h"
#include "ocean/test/testcv/TestEigenUtilities.h"
#include "ocean/test/testcv/TestFrameBlender.h"
#include "ocean/test/testcv/TestFrameChannels.h"
#include "ocean/test/testcv/TestFrameConverter.h"
#include "ocean/test/testcv/TestFrameConverterABGR32.h"
#include "ocean/test/testcv/TestFrameConverterARGB32.h"
#include "ocean/test/testcv/TestFrameConverterB_G_R24.h"
#include "ocean/test/testcv/TestFrameConverterBGR24.h"
#include "ocean/test/testcv/TestFrameConverterBGR32.h"
#include "ocean/test/testcv/TestFrameConverterBGR565.h"
#include "ocean/test/testcv/TestFrameConverterBGRA32.h"
#include "ocean/test/testcv/TestFrameConverterR_G_B24.h"
#include "ocean/test/testcv/TestFrameConverterRGB24.h"
#include "ocean/test/testcv/TestFrameConverterRGB32.h"
#include "ocean/test/testcv/TestFrameConverterRGB565.h"
#include "ocean/test/testcv/TestFrameConverterRGBA32.h"
#include "ocean/test/testcv/TestFrameConverterRGBA64.h"
#include "ocean/test/testcv/TestFrameConverterRGGB10_Packed.h"
#include "ocean/test/testcv/TestFrameConverterUYVY16.h"
#include "ocean/test/testcv/TestFrameConverterY32.h"
#include "ocean/test/testcv/TestFrameConverterY8.h"
#include "ocean/test/testcv/TestFrameConverterY10_Packed.h"
#include "ocean/test/testcv/TestFrameConverterThreshold.h"
#include "ocean/test/testcv/TestFrameConverterYA16.h"
#include "ocean/test/testcv/TestFrameConverterYUV24.h"
#include "ocean/test/testcv/TestFrameConverterYUVA32.h"
#include "ocean/test/testcv/TestFrameConverterYUYV16.h"
#include "ocean/test/testcv/TestFrameConverterYVU24.h"
#include "ocean/test/testcv/TestFrameConverterY_UV12.h"
#include "ocean/test/testcv/TestFrameConverterY_U_V12.h"
#include "ocean/test/testcv/TestFrameConverterY_U_V24.h"
#include "ocean/test/testcv/TestFrameConverterY_VU12.h"
#include "ocean/test/testcv/TestFrameConverterY_V_U12.h"
#include "ocean/test/testcv/TestFrameEnlarger.h"
#include "ocean/test/testcv/TestFrameFilter.h"
#include "ocean/test/testcv/TestFrameFilterCanny.h"
#include "ocean/test/testcv/TestFrameFilterDilation.h"
#include "ocean/test/testcv/TestFrameFilterErosion.h"
#include "ocean/test/testcv/TestFrameFilterGaussian.h"
#include "ocean/test/testcv/TestFrameFilterGradient.h"
#include "ocean/test/testcv/TestFrameFilterLaplace.h"
#include "ocean/test/testcv/TestFrameFilterMean.h"
#include "ocean/test/testcv/TestFrameFilterMedian.h"
#include "ocean/test/testcv/TestFrameFilterMax.h"
#include "ocean/test/testcv/TestFrameFilterMin.h"
#include "ocean/test/testcv/TestFrameFilterPrewitt.h"
#include "ocean/test/testcv/TestFrameFilterScharr.h"
#include "ocean/test/testcv/TestFrameFilterScharrMagnitude.h"
#include "ocean/test/testcv/TestFrameFilterSeparable.h"
#include "ocean/test/testcv/TestFrameFilterSobel.h"
#include "ocean/test/testcv/TestFrameFilterSobelMagnitude.h"
#include "ocean/test/testcv/TestFrameFilterSorted.h"
#include "ocean/test/testcv/TestFrameFilterTemplate.h"
#include "ocean/test/testcv/TestFrameInterpolator.h"
#include "ocean/test/testcv/TestFrameInterpolatorBicubic.h"
#include "ocean/test/testcv/TestFrameInterpolatorBilinear.h"
#include "ocean/test/testcv/TestFrameInterpolatorBilinearAlpha.h"
#include "ocean/test/testcv/TestFrameInterpolatorNearestPixel.h"
#include "ocean/test/testcv/TestFrameInverter.h"
#include "ocean/test/testcv/TestFrameMean.h"
#include "ocean/test/testcv/TestFrameMinMax.h"
#include "ocean/test/testcv/TestFrameNorm.h"
#include "ocean/test/testcv/TestFrameNormalizer.h"
#include "ocean/test/testcv/TestFrameOperations.h"
#include "ocean/test/testcv/TestFramePyramid.h"
#include "ocean/test/testcv/TestFrameShrinker.h"
#include "ocean/test/testcv/TestFrameShrinkerAlpha.h"
#include "ocean/test/testcv/TestFrameTransposer.h"
#include "ocean/test/testcv/TestFrameVariance.h"
#include "ocean/test/testcv/TestHistogram.h"
#include "ocean/test/testcv/TestImageQuality.h"
#include "ocean/test/testcv/TestIntegralImage.h"
#include "ocean/test/testcv/TestMaskAnalyzer.h"
#include "ocean/test/testcv/TestMotion.h"
#include "ocean/test/testcv/TestNEON.h"
#include "ocean/test/testcv/TestNonMaximumSuppression.h"
#include "ocean/test/testcv/TestPixelBoundingBox.h"
#include "ocean/test/testcv/TestPixelPosition.h"
#include "ocean/test/testcv/TestSSE.h"
#include "ocean/test/testcv/TestSumAbsoluteDifferences.h"
#include "ocean/test/testcv/TestSumSquareDifferences.h"
#include "ocean/test/testcv/TestUtilities.h"
#include "ocean/test/testcv/TestZeroMeanSumSquareDifferences.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/TestSelector.h"

#include "ocean/base/Build.h"
#include "ocean/base/DateTime.h"
#include "ocean/base/Frame.h"
#include "ocean/base/Processor.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/String.h"
#include "ocean/base/TaskQueue.h"
#include "ocean/base/Utilities.h"

#ifdef _ANDROID
	#include "ocean/platform/android/Battery.h"
	#include "ocean/platform/android/ProcessorMonitor.h"
#endif

#include "ocean/system/Process.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool testCV(const double testDuration, Worker& worker, const unsigned int width, const unsigned int height, const std::string& testFunctions)
{
	ocean_assert(width >= 32u && height >= 32u);
	ocean_assert(testDuration > 0.0);

	TestResult testResult("Computer Vision Library test");

	Log::info() << "+++   Ocean Computer Vision Library test:   +++";
	Log::info() << " ";

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41
	Log::info() << "The binary contains at most SSE4.1 instructions.";
#endif

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
	Log::info() << "The binary contains at most NEON1 instructions.";
#endif

#if defined(OCEAN_HARDWARE_AVX_VERSION) && OCEAN_HARDWARE_AVX_VERSION >= 20
	Log::info() << "The binary contains at most AVX2 instructions.";
#elif defined(OCEAN_HARDWARE_AVX_VERSION) && OCEAN_HARDWARE_AVX_VERSION >= 10
	Log::info() << "The binary contains at most AVX1 instructions.";
#endif

#if (!defined(OCEAN_HARDWARE_SSE_VERSION) || OCEAN_HARDWARE_SSE_VERSION == 0) && (!defined(OCEAN_HARDWARE_NEON_VERSION) || OCEAN_HARDWARE_NEON_VERSION == 0)
	static_assert(OCEAN_HARDWARE_AVX_VERSION == 0, "Invalid AVX version");
	Log::info() << "The binary does not contain any SIMD instructions.";
#endif

	Log::info() << "While the hardware supports the following SIMD instructions:";
	Log::info() << Processor::translateInstructions(Processor::get().instructions());

	Log::info() << " ";

	const TestSelector selector(testFunctions);

	if (TestSelector subSelector = selector.shouldRun("utilities"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestUtilities::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("bresenham"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestBresenham::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("canvas"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestCanvas::test(testDuration, subSelector);
	}

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	if (TestSelector subSelector = selector.shouldRun("neon"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestNEON::test(testDuration, subSelector);
	}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

	if (TestSelector subSelector = selector.shouldRun("sse"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestSSE::test(testDuration, subSelector);
	}

#endif // OCEAN_HARDWARE_SSE_VERSION >= 41

	if (TestSelector subSelector = selector.shouldRun("pixelposition"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestPixelPosition::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("pixelboundingbox"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestPixelBoundingBox::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("sumabsolutedifferences"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestSumAbsoluteDifferences::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("sumsquaredifferences"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestSumSquareDifferences::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("zeromeansumsquaredifferences"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestZeroMeanSumSquareDifferences::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameenlarger"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameEnlarger::test(testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameblender"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameBlender::test(testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("framechannels"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameChannels::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameconverter"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameConverter::test(testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameconverterabgr32"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameConverterABGR32::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameconverterargb32"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameConverterARGB32::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameconverterbgr24"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameConverterBGR24::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameconverterb_g_r24"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameConverterB_G_R24::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameconverterbgr32"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameConverterBGR32::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameconverterbgr565"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameConverterBGR565::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameconverterbgra32"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameConverterBGRA32::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameconverterrgb24"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameConverterRGB24::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameconverterr_g_b24"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameConverterR_G_B24::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameconverterrgb32"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameConverterRGB32::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameconverterrgb565"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameConverterRGB565::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameconverterrgba32"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameConverterRGBA32::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameconverterrgba64"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameConverterRGBA64::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameconverterrggb10_packed"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameConverterRGGB10_Packed::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameconvertery8"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameConverterY8::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameconvertery10_packed"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameConverterY10_Packed::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameconvertery32"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameConverterY32::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameconverterya16"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameConverterYA16::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameconvertery_uv12"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameConverterY_UV12::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameconvertery_vu12"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameConverterY_VU12::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameconverteryuv24"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameConverterYUV24::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameconverteryuva32"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameConverterYUVA32::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameconverteryuyv16"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameConverterYUYV16::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameconverteruyvy16"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameConverterUYVY16::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameconverteryvu24"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameConverterYVU24::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameconvertery_u_v12"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameConverterY_U_V12::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameconvertery_u_v24"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameConverterY_U_V24::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameconvertery_v_u12"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameConverterY_V_U12::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameconverterthreshold"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameConverterThreshold::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("framefiltergaussian"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameFilterGaussian::test(testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("framefilterseparable"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameFilterSeparable::test(testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("framefilterprewitt"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameFilterPrewitt::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("framefiltercanny"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameFilterCanny::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("framefilterscharr"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameFilterScharr::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("framefilterscharrmagnitude"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameFilterScharrMagnitude::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("framefiltersobel"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameFilterSobel::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("framefiltersobelmagnitude"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameFilterSobelMagnitude::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("framefiltertemplate"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameFilterTemplate::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("framefilterlaplace"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameFilterLaplace::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("framefiltermean"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameFilterMean::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("framefiltersorted"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameFilterSorted::test(testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("framefiltermedian"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameFilterMedian::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("framefiltermin"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameFilterMin::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("framefiltermax"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameFilterMax::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("framefiltererosion"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameFilterErosion::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("framefiltergradient"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameFilterGradient::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("framefilterdilation"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameFilterDilation::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("framefilter"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameFilter::test(testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameshrinker"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameShrinker::test(testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameshrinkeralpha"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameShrinkerAlpha::test(testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frametransposer"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameTransposer::test(testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameoperations"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameOperations::test(testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("framepyramid"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFramePyramid::test(testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameinterpolatornearestpixel"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameInterpolatorNearestPixel::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameinterpolatorbilinear"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameInterpolatorBilinear::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameinterpolatorbilinearalpha"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameInterpolatorBilinearAlpha::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameinterpolatorbicubic"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameInterpolatorBicubic::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameinterpolator"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameInterpolator::test(testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("maskanalyzer"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestMaskAnalyzer::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("motion"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestMotion::test(testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("integralimage"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestIntegralImage::test(width, height, testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("framevariance"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameVariance::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("nonmaximumsuppression"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestNonMaximumSuppression::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("framemean"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameMean::test(testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("framenorm"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameNorm::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("framenormalizer"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameNormalizer::test(testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameminmax"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameMinMax::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("histogram"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestHistogram::test(testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frameinverter"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestFrameInverter::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("imagequality"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestImageQuality::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("eigenutilities"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		testResult = TestEigenUtilities::test(testDuration, subSelector);
	}

	Log::info() << " ";
	Log::info() << " ";
	Log::info() << " ";
	Log::info() << " ";

	Log::info() << selector << " " << testResult;
	return testResult.succeeded();
}

static void testCVAsynchronInternal(const double testDuration, const unsigned int testWidth, const unsigned int testHeight, const std::string testFunctions)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(testWidth >= 32u && testHeight >= 32u);

	const Timestamp startTimestamp(true);

	Log::info() << "Ocean Framework test for the Computer Vision library:";
	Log::info() << "Platform: " << Build::buildString();
	Log::info() << "Start: " << DateTime::stringDate() << ", " << DateTime::stringTime() << " UTC";
	Log::info() << " ";

	Log::info() << "Default test frame dimension: " << testWidth << "x" << testHeight;
	Log::info() << "Function list: " << (testFunctions.empty() ? "All functions" : testFunctions);
	Log::info() << "Duration for each test: " << String::toAString(testDuration, 1u) << "s";
	Log::info() << " ";

	RandomI::initialize();
	System::Process::setPriority(System::Process::PRIORITY_ABOVE_NORMAL);

	Log::info() << "Random generator initialized";
	Log::info() << "Process priority set to above normal";
	Log::info() << " ";

	Worker worker;

	Log::info() << "Used worker threads: " << worker.threads();

#ifdef _ANDROID
	Platform::Android::ProcessorStatistic processorStatistic;
	processorStatistic.start();

	Log::info() << " ";
	Log::info() << "Battery: " << String::toAString(Platform::Android::Battery::currentCapacity(), 1u) << "%, temperature: " << String::toAString(Platform::Android::Battery::currentTemperature(), 1u) << "deg Celsius";
#endif

	Log::info() << " ";

	try
	{
		testCV(testDuration, worker, testWidth, testHeight, testFunctions);
	}
	catch (const std::exception& exception)
	{
		Log::error() << "Unhandled exception: " << exception.what();
	}
	catch (...)
	{
		Log::error() << "Unhandled exception!";
	}

#ifdef _ANDROID
	processorStatistic.stop();

	Log::info() << " ";
	Log::info() << "Duration: " << " in " << processorStatistic.duration() << "s";
	Log::info() << "Measurements: " << processorStatistic.measurements();
	Log::info() << "Average active cores: " << processorStatistic.averageActiveCores();
	Log::info() << "Average frequency: " << processorStatistic.averageFrequency() << "kHz";
	Log::info() << "Minimal frequency: " << processorStatistic.minimalFrequency() << "kHz";
	Log::info() << "Maximal frequency: " << processorStatistic.maximalFrequency() << "kHz";
	Log::info() << "Average CPU performance rate: " << processorStatistic.averagePerformanceRate();

	Log::info() << " ";
	Log::info() << "Battery: " << String::toAString(Platform::Android::Battery::currentCapacity(), 1u) << "%, temperature: " << String::toAString(Platform::Android::Battery::currentTemperature(), 1u) << "deg Celsius";
#endif

	Log::info() << " ";

	const Timestamp endTimestamp(true);

	Log::info() << "Time elapsed: " << DateTime::seconds2string(double(endTimestamp - startTimestamp), true);
	Log::info() << "End: " << DateTime::stringDate() << ", " << DateTime::stringTime() << " UTC";
	Log::info() << " ";
}

void testCVAsynchron(const double testDuration, const unsigned int testWidth, const unsigned int testHeight, const std::string& testFunctions)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(testWidth >= 32u && testHeight >= 32u);

	TaskQueue::get().pushTask(TaskQueue::Task::createStatic(&testCVAsynchronInternal, testDuration, testWidth, testHeight, testFunctions));
}

} // namespace TestCV

} // namespace Test

} // namespace Ocean
