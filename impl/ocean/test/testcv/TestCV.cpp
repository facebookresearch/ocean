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
#include "ocean/test/testcv/TestFrameConverterBGR24.h"
#include "ocean/test/testcv/TestFrameConverterBGR32.h"
#include "ocean/test/testcv/TestFrameConverterBGR565.h"
#include "ocean/test/testcv/TestFrameConverterBGRA32.h"
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

	bool allSucceeded = true;

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

	std::vector<std::string> tests(Utilities::separateValues(String::toLower(testFunctions), ',', true, true));
	const std::set<std::string> testSet(tests.begin(), tests.end());

	if (testSet.empty() || testSet.find("utilities") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestUtilities::test(testDuration))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("bresenham") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestBresenham::test(testDuration))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("canvas") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestCanvas::test(testDuration))
		{
			allSucceeded = false;
		}
	}

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	if (testSet.empty() || testSet.find("neon") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestNEON::test(testDuration))
		{
			allSucceeded = false;
		}
	}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

	if (testSet.empty() || testSet.find("sse") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestSSE::test(testDuration))
		{
			allSucceeded = false;
		}
	}

#endif // OCEAN_HARDWARE_SSE_VERSION >= 41

	if (testSet.empty() || testSet.find("pixelposition") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestPixelPosition::test(testDuration))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("pixelboundingbox") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestPixelBoundingBox::test(testDuration))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("sumabsolutedifferences") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestSumAbsoluteDifferences::test(testDuration))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("sumsquaredifferences") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestSumSquareDifferences::test(testDuration))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("zeromeansumsquaredifferences") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestZeroMeanSumSquareDifferences::test(testDuration))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("frameenlarger") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameEnlarger::test(testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("frameblender") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameBlender::test(testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("framechannels") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameChannels::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("frameconverter") != testSet.end() || testSet.find("frameconverter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameConverter::test(testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("frameconverterabgr32") != testSet.end() || testSet.find("frameconverter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameConverterABGR32::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("frameconverterargb32") != testSet.end() || testSet.find("frameconverter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameConverterARGB32::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("frameconverterbgr24") != testSet.end() || testSet.find("frameconverter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameConverterBGR24::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("frameconverterbgr32") != testSet.end() || testSet.find("frameconverter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameConverterBGR32::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("frameconverterbgr565") != testSet.end() || testSet.find("frameconverter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameConverterBGR565::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("frameconverterbgra32") != testSet.end() || testSet.find("frameconverter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameConverterBGRA32::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("frameconverterrgb24") != testSet.end() || testSet.find("frameconverter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameConverterRGB24::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("frameconverterrgb32") != testSet.end() || testSet.find("frameconverter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameConverterRGB32::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("frameconverterrgb565") != testSet.end() || testSet.find("frameconverter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameConverterRGB565::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("frameconverterrgba32") != testSet.end() || testSet.find("frameconverter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameConverterRGBA32::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("frameconverterrgba64") != testSet.end() || testSet.find("frameconverter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameConverterRGBA64::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("frameconverterrggb10_packed") != testSet.end() || testSet.find("frameconverter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameConverterRGGB10_Packed::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("frameconvertery8") != testSet.end() || testSet.find("frameconverter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameConverterY8::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("frameconvertery10_packed") != testSet.end() || testSet.find("frameconverter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameConverterY10_Packed::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("frameconvertery32") != testSet.end() || testSet.find("frameconverter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameConverterY32::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("frameconverterya16") != testSet.end() || testSet.find("frameconverter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameConverterYA16::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("frameconvertery_uv12") != testSet.end() || testSet.find("frameconverter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameConverterY_UV12::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("frameconvertery_vu12") != testSet.end() || testSet.find("frameconverter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameConverterY_VU12::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("frameconverteryuv24") != testSet.end() || testSet.find("frameconverter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameConverterYUV24::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("frameconverteryuva32") != testSet.end() || testSet.find("frameconverter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameConverterYUVA32::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("frameconverteryuyv16") != testSet.end() || testSet.find("frameconverter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameConverterYUYV16::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("frameconverteruyvy16") != testSet.end() || testSet.find("frameconverter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameConverterUYVY16::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("frameconverteryvu24") != testSet.end() || testSet.find("frameconverter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameConverterYVU24::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("frameconvertery_u_v12") != testSet.end() || testSet.find("frameconverter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameConverterY_U_V12::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("frameconvertery_u_v24") != testSet.end() || testSet.find("frameconverter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameConverterY_U_V24::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("frameconvertery_v_u12") != testSet.end() || testSet.find("frameconverter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameConverterY_V_U12::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("frameconverterthreshold") != testSet.end() || testSet.find("frameconverter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameConverterThreshold::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("framefiltergaussian") != testSet.end() || testSet.find("framefilter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameFilterGaussian::test(testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("framefilterseparable") != testSet.end() || testSet.find("framefilter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameFilterSeparable::test(testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("framefilterprewitt") != testSet.end() || testSet.find("framefilter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameFilterPrewitt::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("framefiltercanny") != testSet.end() || testSet.find("framefiltercanny*") != testSet.end() || testSet.find("framefilter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameFilterCanny::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("framefilterscharr") != testSet.end() || testSet.find("framefilterscharr*") != testSet.end() || testSet.find("framefilter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameFilterScharr::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("framefilterscharrmagnitude") != testSet.end() || testSet.find("framefilterscharr*") != testSet.end() || testSet.find("framefilter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameFilterScharrMagnitude::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("framefiltersobel") != testSet.end() || testSet.find("framefiltersobel*") != testSet.end() || testSet.find("framefilter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameFilterSobel::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("framefiltersobelmagnitude") != testSet.end() || testSet.find("framefiltersobel*") != testSet.end() || testSet.find("framefilter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameFilterSobelMagnitude::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("framefiltertemplate") != testSet.end() || testSet.find("framefilter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameFilterTemplate::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("framefilterlaplace") != testSet.end() || testSet.find("framefilter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameFilterLaplace::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("framefiltermean") != testSet.end() || testSet.find("framefilter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameFilterMean::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("framefiltersorted") != testSet.end() || testSet.find("framefilter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameFilterSorted::test(testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("framefiltermedian") != testSet.end() || testSet.find("framefilter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameFilterMedian::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("framefiltermin") != testSet.end() || testSet.find("framefilter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameFilterMin::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("framefiltermax") != testSet.end() || testSet.find("framefilter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameFilterMax::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("framefiltererosion") != testSet.end() || testSet.find("framefilter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameFilterErosion::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("framefiltergradient") != testSet.end() || testSet.find("framefilter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameFilterGradient::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("framefilterdilation") != testSet.end() || testSet.find("framefilter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameFilterDilation::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("framefilter") != testSet.end() || testSet.find("framefilter*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameFilter::test(testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("frameshrinker") != testSet.end() || testSet.find("frameshrinker*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameShrinker::test(testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("frameshrinkeralpha") != testSet.end() || testSet.find("frameshrinker*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameShrinkerAlpha::test(testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("frametransposer") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameTransposer::test(testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("frameoperations") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameOperations::test(testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("framepyramid") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFramePyramid::test(testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("frameinterpolatornearestpixel") != testSet.end() || testSet.find("frameinterpolator*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameInterpolatorNearestPixel::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("frameinterpolatorbilinear") != testSet.end() || testSet.find("frameinterpolator*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameInterpolatorBilinear::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("frameinterpolatorbilinearalpha") != testSet.end() || testSet.find("frameinterpolator*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameInterpolatorBilinearAlpha::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("frameinterpolatorbicubic") != testSet.end() || testSet.find("frameinterpolator*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameInterpolatorBicubic::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("frameinterpolator") != testSet.end() || testSet.find("frameinterpolator*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameInterpolator::test(testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("maskanalyzer") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestMaskAnalyzer::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("motion") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestMotion::test(testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("integralimage") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestIntegralImage::test(width, height, testDuration))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("framevariance") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameVariance::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("nonmaximumsuppression") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestNonMaximumSuppression::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("framemean") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameMean::test(testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("framenorm") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameNorm::test(testDuration))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("framenormalizer") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameNormalizer::test(testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("frameminmax") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameMinMax::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("histogram") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestHistogram::test(testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("frameinverter") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestFrameInverter::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("imagequality") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestImageQuality::test(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	if (testSet.empty() || testSet.find("eigenutilities") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";

		if (!TestEigenUtilities::test(testDuration))
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";
	Log::info() << " ";
	Log::info() << " ";
	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << (testSet.empty() ? "Entire" : "Partial") << " Computer Vision library test succeeded.";
	}
	else
	{
		Log::info() << (testSet.empty() ? "Entire" : "Partial") << " Computer Vision library test FAILED!";
	}

	return allSucceeded;
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
