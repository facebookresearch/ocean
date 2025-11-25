/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testgeometry/TestGeometry.h"

#include "ocean/test/testgeometry/TestAbsoluteTransformation.h"
#include "ocean/test/testgeometry/TestDelaunay.h"
#include "ocean/test/testgeometry/TestEpipolarGeometry.h"
#include "ocean/test/testgeometry/TestError.h"
#include "ocean/test/testgeometry/TestEstimator.h"
#include "ocean/test/testgeometry/TestHomography.h"
#include "ocean/test/testgeometry/TestJacobian.h"
#include "ocean/test/testgeometry/TestJLinkage.h"
#include "ocean/test/testgeometry/TestMultipleViewGeometry.h"
#include "ocean/test/testgeometry/TestNonLinearOptimizationCamera.h"
#include "ocean/test/testgeometry/TestNonLinearOptimizationHomography.h"
#include "ocean/test/testgeometry/TestNonLinearOptimizationLine.h"
#include "ocean/test/testgeometry/TestNonLinearOptimizationObjectPoint.h"
#include "ocean/test/testgeometry/TestNonLinearOptimizationOrientation.h"
#include "ocean/test/testgeometry/TestNonLinearOptimizationPlane.h"
#include "ocean/test/testgeometry/TestNonLinearOptimizationPose.h"
#include "ocean/test/testgeometry/TestNonLinearOptimizationTransformation.h"
#include "ocean/test/testgeometry/TestOctree.h"
#include "ocean/test/testgeometry/TestP3P.h"
#include "ocean/test/testgeometry/TestP4P.h"
#include "ocean/test/testgeometry/TestPnP.h"
#include "ocean/test/testgeometry/TestRANSAC.h"
#include "ocean/test/testgeometry/TestSpatialDistribution.h"
#include "ocean/test/testgeometry/TestStereoscopicGeometry.h"
#include "ocean/test/testgeometry/TestUtilities.h"

#include "ocean/test/TestResult.h"

#include "ocean/base/Build.h"
#include "ocean/base/DateTime.h"
#include "ocean/base/Processor.h"
#include "ocean/base/RandomI.h"
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

namespace TestGeometry
{

bool testGeometry(const double testDuration, Worker& worker, const std::string& testFunctions)
{
	TestResult testResult("Ocean Geometry Library test");

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

	if (TestSelector subSelector = selector.shouldRun("spatialdistribution"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestSpatialDistribution::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("absolutetransformation"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestAbsoluteTransformation::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("p3p"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestP3P::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("p4p"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestP4P::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("pnp"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestPnP::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("homography"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestHomography::test(testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("jacobian"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestJacobian::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("jlinkage"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestJLinkage::testJLinkage(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("tlinkage"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestJLinkage::testTLinkage(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("error"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestError::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("estimator"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestEstimator::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("ransac"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestRANSAC::test(testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("nonlinearoptimizationcamera"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestNonLinearOptimizationCamera::test(testDuration, &worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("nonlinearoptimizationhomography"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestNonLinearOptimizationHomography::test(testDuration, &worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("nonlinearoptimizationline"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestNonLinearOptimizationLine::test(testDuration, &worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("nonlinearoptimizationorientation"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestNonLinearOptimizationOrientation::test(testDuration, &worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("nonlinearoptimizationobjectpoint"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestNonLinearOptimizationObjectPoint::test(testDuration, &worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("nonlinearoptimizationplane"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestNonLinearOptimizationPlane::test(testDuration, &worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("nonlinearoptimizationpose"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestNonLinearOptimizationPose::test(testDuration, &worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("nonlinearoptimizationtransformation"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestNonLinearOptimizationTransformation::test(testDuration, &worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("epipolargeometry"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestEpipolarGeometry::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("multipleviewgeometry"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestMultipleViewGeometry::testMultipleViewGeometry(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("delaunay"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestDelaunay::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("utilities"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestUtilities::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("octree"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestOctree::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("stereoscopicgeometry"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestStereoscopicGeometry::test(testDuration, subSelector);
	}

	Log::info() << " ";
	Log::info() << " ";
	Log::info() << " ";
	Log::info() << " ";

	Log::info() << selector << " " << testResult;

	return testResult.succeeded();
}

static void testGeometryAsynchronInternal(const double testDuration, const std::string testFunctions)
{
	ocean_assert(testDuration > 0.0);

	System::Process::setPriority(System::Process::PRIORITY_ABOVE_NORMAL);
	Log::info() << "Process priority set to above normal";
	Log::info() << " ";

	const Timestamp startTimestamp(true);

	Log::info() << "Ocean Framework test for the Geometry library:";
	Log::info() << "Platform: " << Build::buildString();
	Log::info() << "Start: " << DateTime::stringDate() << ", " << DateTime::stringTime() << " UTC";
	Log::info() << " ";

	Log::info() << "Function list: " << (testFunctions.empty() ? "All functions" : testFunctions);
	Log::info() << "Duration for each test: " << String::toAString(testDuration, 1u) << "s";
	Log::info() << " ";

	RandomI::initialize();

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
		testGeometry(testDuration, worker, testFunctions);
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

void testGeometryAsynchron(const double testDuration, const std::string& testFunctions)
{
	ocean_assert(testDuration > 0.0);

	TaskQueue::get().pushTask(TaskQueue::Task::createStatic(&testGeometryAsynchronInternal, testDuration, testFunctions));
}

}

}

}
