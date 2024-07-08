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
	bool allSucceeded = true;

	Log::info() << "+++   Ocean Geometry Library test:   +++";
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

	if (testSet.empty() || testSet.find("spatialdistribution") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestSpatialDistribution::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("absolutetransformation") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestAbsoluteTransformation::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("p3p") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestP3P::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("p4p") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestP4P::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("pnp") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestPnP::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("homography") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestHomography::test(testDuration, worker) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("jacobian") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestJacobian::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("jlinkage") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestJLinkage::testJLinkage(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("tlinkage") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestJLinkage::testTLinkage(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("error") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestError::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("estimator") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestEstimator::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("ransac") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestRANSAC::test(testDuration, worker) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("nonlinearoptimizationcamera") != testSet.end() || testSet.find("nonlinearoptimization*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestNonLinearOptimizationCamera::test(testDuration, &worker) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("nonlinearoptimizationhomography") != testSet.end() || testSet.find("nonlinearoptimization*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestNonLinearOptimizationHomography::test(testDuration, &worker) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("nonlinearoptimizationline") != testSet.end() || testSet.find("nonlinearoptimization*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestNonLinearOptimizationLine::test(testDuration, &worker) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("nonlinearoptimizationorientation") != testSet.end() || testSet.find("nonlinearoptimization*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestNonLinearOptimizationOrientation::test(testDuration, &worker) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("nonlinearoptimizationobjectpoint") != testSet.end() || testSet.find("nonlinearoptimization*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestNonLinearOptimizationObjectPoint::test(testDuration, &worker) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("nonlinearoptimizationplane") != testSet.end() || testSet.find("nonlinearoptimization*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestNonLinearOptimizationPlane::test(testDuration, &worker) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("nonlinearoptimizationpose") != testSet.end() || testSet.find("nonlinearoptimization*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestNonLinearOptimizationPose::test(testDuration, &worker) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("nonlinearoptimizationtransformation") != testSet.end() || testSet.find("nonlinearoptimization*") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestNonLinearOptimizationTransformation::test(testDuration, &worker) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("epipolargeometry") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestEpipolarGeometry::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("multipleviewgeometry") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestMultipleViewGeometry::testMultipleViewGeometry(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("delaunay") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestDelaunay::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("utilities") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestUtilities::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("octree") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestOctree::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("stereoscopicgeometry") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestStereoscopicGeometry::test(testDuration) && allSucceeded;
	}

	Log::info() << " ";
	Log::info() << " ";
	Log::info() << " ";
	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << (testSet.empty() ? "Entire" : "Partial") << " geometry library test succeeded.";
	}
	else
	{
		Log::info() << (testSet.empty() ? "Entire" : "Partial") << " geometry library test FAILED!";
	}

	return allSucceeded;
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
