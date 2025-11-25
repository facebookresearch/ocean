/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestMath.h"
#include "ocean/test/testmath/TestAnyCamera.h"
#include "ocean/test/testmath/TestApproximation.h"
#include "ocean/test/testmath/TestBoundingBox.h"
#include "ocean/test/testmath/TestBoundingSphere.h"
#include "ocean/test/testmath/TestBox2.h"
#include "ocean/test/testmath/TestBox3.h"
#include "ocean/test/testmath/TestCamera.h"
#include "ocean/test/testmath/TestCone3.h"
#include "ocean/test/testmath/TestCylinder3.h"
#include "ocean/test/testmath/TestEquation.h"
#include "ocean/test/testmath/TestEuler.h"
#include "ocean/test/testmath/TestExponentialMap.h"
#include "ocean/test/testmath/TestFiniteLine2.h"
#include "ocean/test/testmath/TestFiniteLine3.h"
#include "ocean/test/testmath/TestFisheyeCamera.h"
#include "ocean/test/testmath/TestFourierTransformation.h"
#include "ocean/test/testmath/TestFrustum.h"
#include "ocean/test/testmath/TestHomogenousMatrix4.h"
#include "ocean/test/testmath/TestHSVAColor.h"
#include "ocean/test/testmath/TestInterpolation.h"
#include "ocean/test/testmath/TestLine2.h"
#include "ocean/test/testmath/TestLine3.h"
#include "ocean/test/testmath/TestLinearAlgebra.h"
#include "ocean/test/testmath/TestLookup2.h"
#include "ocean/test/testmath/TestMathUtilities.h"
#include "ocean/test/testmath/TestMatrix.h"
#include "ocean/test/testmath/TestNumeric.h"
#include "ocean/test/testmath/TestPinholeCamera.h"
#include "ocean/test/testmath/TestPlane3.h"
#include "ocean/test/testmath/TestQuaternion.h"
#include "ocean/test/testmath/TestRandom.h"
#include "ocean/test/testmath/TestRateCalculator.h"
#include "ocean/test/testmath/TestRGBAColor.h"
#include "ocean/test/testmath/TestRotation.h"
#include "ocean/test/testmath/TestSampleMap.h"
#include "ocean/test/testmath/TestAutomaticDifferentiation.h"
#include "ocean/test/testmath/TestSparseMatrix.h"
#include "ocean/test/testmath/TestSphere3.h"
#include "ocean/test/testmath/TestSquareMatrix2.h"
#include "ocean/test/testmath/TestSquareMatrix3.h"
#include "ocean/test/testmath/TestSquareMatrix4.h"
#include "ocean/test/testmath/TestStaticMatrix.h"
#include "ocean/test/testmath/TestTriangle2.h"
#include "ocean/test/testmath/TestVariance.h"
#include "ocean/test/testmath/TestVector2.h"
#include "ocean/test/testmath/TestVector3.h"
#include "ocean/test/testmath/TestVector4.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/TestSelector.h"

#include "ocean/base/Build.h"
#include "ocean/base/DateTime.h"
#include "ocean/base/Processor.h"
#include "ocean/base/String.h"
#include "ocean/base/TaskQueue.h"
#include "ocean/base/Timestamp.h"
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

namespace TestMath
{

bool testMath(const double testDuration, Worker& worker, const std::string& testFunctions)
{
	TestResult testResult("Ocean Math Library test");

	Log::info() << " ";
	Log::info() << "Test with: " << String::toAString(sizeof(Scalar)) << "byte floats";
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

	if (TestSelector subSelector = selector.shouldRun("random"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestRandom::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("camera"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestCamera::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("pinholecamera"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestPinholeCamera::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("fisheyecamera"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestFisheyeCamera::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("anycamera"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestAnyCamera::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("equation"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestEquation::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("homogenousmatrix4"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestHomogenousMatrix4::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("numeric"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestNumeric::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("interpolation"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestInterpolation::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("line2"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestLine2::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("line3"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestLine3::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("finiteline2"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestFiniteLine2::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("finiteline3"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestFiniteLine3::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("triangle2"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestTriangle2::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("linearalgebra"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestLinearAlgebra::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("lookup2"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestLookup2::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("plane3"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestPlane3::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("box2"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestBox2::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("box3"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestBox3::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("cone3"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestCone3::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("cylinder3"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestCylinder3::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("sphere3"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestSphere3::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("boundingbox"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestBoundingBox::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("boundingsphere"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestBoundingSphere::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("approximation"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestApproximation::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("matrix"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestMatrix::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("squarematrix2"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestSquareMatrix2::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("squarematrix3"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestSquareMatrix3::test(testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("squarematrix4"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestSquareMatrix4::test(testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("staticmatrix"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestStaticMatrix::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("vector2"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestVector2::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("vector3"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestVector3::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("vector4"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestVector4::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("rotation"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestRotation::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("quaternion"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestQuaternion::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("euler"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestEuler::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("exponentialmap"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestExponentialMap::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("sparsematrix"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestSparseMatrix::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("automaticdifferentiation"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestAutomaticDifferentiation::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("fouriertransformation"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestFourierTransformation::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("samplemap"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestSampleMap::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("rgbacolor"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestRGBAColor::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("hsvacolor"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestHSVAColor::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("ratecalculator"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestRateCalculator::test(testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frustum"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestFrustum::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("mathutilities"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestMathUtilities::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("variance"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestVariance::test(testDuration, subSelector);
	}

	Log::info() << " ";
	Log::info() << " ";
	Log::info() << " ";
	Log::info() << " ";

	Log::info() << selector << " " << testResult;

	return testResult.succeeded();
}

static void testMathAsynchronInternal(const double testDuration, const std::string testFunctions)
{
	ocean_assert(testDuration > 0.0);

	System::Process::setPriority(System::Process::PRIORITY_ABOVE_NORMAL);
	Log::info() << "Process priority set to above normal";
	Log::info() << " ";

	const Timestamp startTimestamp(true);

	Log::info() << "Ocean Framework test for the Math library:";
	Log::info() << "Platform: " << Build::buildString();
	Log::info() << "Start: " << DateTime::stringDate() << ", " << DateTime::stringTime() << " UTC";
	Log::info() << " ";

	Log::info() << "Function list: " << (testFunctions.empty() ? "All functions" : testFunctions);
	Log::info() << "Duration for each test: " << String::toAString(testDuration, 1u) << "s";
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
		testMath(testDuration, worker, testFunctions);
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

void testMathAsynchron(const double testDuration, const std::string& testFunctions)
{
	TaskQueue::get().pushTask(TaskQueue::Task::createStatic(&testMathAsynchronInternal, testDuration, testFunctions));
}

}

}

}
