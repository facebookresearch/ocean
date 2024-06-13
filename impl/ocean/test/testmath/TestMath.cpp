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
#include "ocean/test/testmath/TestCone3.h"
#include "ocean/test/testmath/TestCylinder3.h"
#include "ocean/test/testmath/TestEquation.h"
#include "ocean/test/testmath/TestEuler.h"
#include "ocean/test/testmath/TestExponentialMap.h"
#include "ocean/test/testmath/TestFiniteLine2.h"
#include "ocean/test/testmath/TestFiniteLine3.h"
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
	bool allSucceeded = true;

	Log::info() << "+++   Ocean Math Library test:   +++";
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

	std::vector<std::string> tests(Utilities::separateValues(String::toLower(testFunctions), ',', true, true));
	const std::set<std::string> testSet(tests.begin(), tests.end());

	if (testSet.empty() || testSet.find("random") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestRandom::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("pinholecamera") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestPinholeCamera::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("anycamera") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestAnyCamera::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("equation") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestEquation::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("homogenousmatrix4") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestHomogenousMatrix4::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("numeric") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestNumeric::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("interpolation") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestInterpolation::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("line2") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestLine2::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("line3") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestLine3::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("finiteline2") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestFiniteLine2::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("finiteline3") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestFiniteLine3::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("triangle2") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestTriangle2::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("linearalgebra") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestLinearAlgebra::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("lookup2") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestLookup2::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("plane3") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestPlane3::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("box2") != testSet.end() || testSet.find("box*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestBox2::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("box3") != testSet.end() || testSet.find("box*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestBox3::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("cone3") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestCone3::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("cylinder3") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestCylinder3::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("sphere3") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestSphere3::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("boundingbox") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestBoundingBox::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("boundingsphere") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestBoundingSphere::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("approximation") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestApproximation::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("matrix") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestMatrix::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("squarematrix2") != testSet.end() || testSet.find("squarematrix*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestSquareMatrix2::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("squarematrix3") != testSet.end() || testSet.find("squarematrix*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestSquareMatrix3::test(testDuration, worker) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("squarematrix4") != testSet.end() || testSet.find("squarematrix*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestSquareMatrix4::test(testDuration, worker) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("staticmatrix") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestStaticMatrix::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("vector2") != testSet.end() || testSet.find("vector*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestVector2::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("vector3") != testSet.end() || testSet.find("vector*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestVector3::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("vector4") != testSet.end() || testSet.find("vector*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestVector4::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("rotation") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestRotation::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("quaternion") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestQuaternion::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("euler") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestEuler::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("exponentialmap") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestExponentialMap::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("sparsematrix") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestSparseMatrix::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("automaticdifferentiation") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestAutomaticDifferentiation::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("fouriertransformation") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestFourierTransformation::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("samplemap") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestSampleMap::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("rgbacolor") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestRGBAColor::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("hsvacolor") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestHSVAColor::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("ratecalculator") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestRateCalculator::test(testDuration, worker) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("frustum") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestFrustum::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("mathutilities") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestMathUtilities::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("variance") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestVariance::test(testDuration) && allSucceeded;
	}

	Log::info() << " ";
	Log::info() << " ";
	Log::info() << " ";
	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << (testSet.empty() ? "Entire" : "Partial") << " Ocean Math Library test succeeded.";
	}
	else
	{
		Log::info() << (testSet.empty() ? "Entire" : "Partial") << " Ocean Math Library test FAILED!";
	}

	return allSucceeded;
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
