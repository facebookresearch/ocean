/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testsynthesis/TestCVSynthesis.h"
#include "ocean/test/testcv/testsynthesis/TestCreatorF1.h"
#include "ocean/test/testcv/testsynthesis/TestCreatorI1.h"
#include "ocean/test/testcv/testsynthesis/TestInitializerF1.h"
#include "ocean/test/testcv/testsynthesis/TestInitializerI1.h"
#include "ocean/test/testcv/testsynthesis/TestMappingF1.h"
#include "ocean/test/testcv/testsynthesis/TestMappingI1.h"
#include "ocean/test/testcv/testsynthesis/TestOptimizerF1.h"
#include "ocean/test/testcv/testsynthesis/TestOptimizerI1.h"

#include "ocean/test/TestResult.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Processor.h"
#include "ocean/base/Utilities.h"
#include "ocean/base/String.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameConverter.h"

#include "ocean/math/Random.h"

#include <set>

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestSynthesis
{

bool testCVSynthesis(const double testDuration, Worker& worker, const unsigned int testWidth, const unsigned int testHeight, const std::string& testFunctions)
{
	TestResult testResult("Ocean Synthesis Computer Vision Library test");

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

	if (TestSelector subSelector = selector.shouldRun("mappingi1"))
	{
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";

		testResult = TestMappingI1::test(testWidth, testHeight, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("mappingf1"))
	{
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";

		testResult = TestMappingF1::test(testWidth, testHeight, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("creatori1"))
	{
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";

		testResult = TestCreatorI1::test(testWidth, testHeight, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("creatorf1"))
	{
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";

		testResult = TestCreatorF1::test(testWidth, testHeight, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("initializeri1"))
	{
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";

		testResult = TestInitializerI1::test(testWidth, testHeight, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("initializerf1"))
	{
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";

		testResult = TestInitializerF1::test(testWidth, testHeight, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("optimizeri1"))
	{
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";

		testResult = TestOptimizerI1::test(testWidth, testHeight, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("optimizerf1"))
	{
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";

		testResult = TestOptimizerF1::test(testWidth, testHeight, testDuration, worker, subSelector);
	}

	Log::info() << " ";

	Log::info() << selector << " " << testResult;

	return testResult.succeeded();
}

}

}

}

}
