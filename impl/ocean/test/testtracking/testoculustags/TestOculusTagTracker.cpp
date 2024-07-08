/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testtracking/testoculustags/TestOculusTagTracker.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/cv/CVUtilities.h"

#include "ocean/test/testgeometry/Utilities.h"

using namespace Ocean::Tracking::OculusTags;

namespace Ocean
{

namespace Test
{

namespace TestTracking
{

namespace TestOculusTags
{


bool TestOculusTagTracker::test(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "---   OculusTag test:   ---";
	Log::info() << " ";

	allSucceeded = testStressTestNegative(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "OculusTag test succeeded.";
	}
	else
	{
		Log::info() << "OculusTag test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestOculusTags, MacrosInCode)
{
	// Ensure that non of the debug/benchmark macros is defined

#ifdef OCN_OCULUSTAG_DEBUGGING_ENABLED
	const bool debuggingEnabled = true;
#else
	const bool debuggingEnabled = false;
#endif

	EXPECT_FALSE(debuggingEnabled);
}

TEST(TestOculusTags, OculusTagTrackerStressTestNegative)
{
	Worker worker;
	EXPECT_TRUE(TestOculusTagTracker::testStressTestNegative(GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestOculusTagTracker::testStressTestNegative(const double testDuration, Worker& /*worker*/)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Stress test:";

	// This test is not testing correctness or benchmarking performance
	// it's simply meant to ensure that the tracker does not crash

	RandomGenerator randomGenerator;
	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const SharedAnyCamera anyCameraA = Test::TestGeometry::Utilities::realisticAnyCamera(AnyCameraType::FISHEYE, RandomI::random(randomGenerator, 1u));
		const SharedAnyCamera anyCameraB = Test::TestGeometry::Utilities::realisticAnyCamera(AnyCameraType::FISHEYE, RandomI::random(randomGenerator, 1u));

		OculusTagTracker oculusTagTracker;

		const unsigned int frameNumbers = RandomI::random(randomGenerator, 1u, 5u);

		for (unsigned int n = 0u; n < frameNumbers; ++n)
		{
			const FrameType frameType(FrameType(anyCameraA->width(), anyCameraA->height(), FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));

			const Frame yFrameA = CV::CVUtilities::randomizedFrame(frameType, &randomGenerator);
			const Frame yFrameB = CV::CVUtilities::randomizedFrame(frameType, &randomGenerator);

			const HomogenousMatrix4 world_T_device = HomogenousMatrix4(Random::vector3(randomGenerator, Scalar(-5), Scalar(5)), Random::quaternion(randomGenerator));

			const HomogenousMatrix4 device_T_cameraA = HomogenousMatrix4(Random::vector3(randomGenerator, Scalar(-0.05), Scalar(0.05)), Random::euler(randomGenerator, Numeric::deg2rad(0), Numeric::deg2rad(30)));
			const HomogenousMatrix4 device_T_cameraB = HomogenousMatrix4(Random::vector3(randomGenerator, Scalar(-0.05), Scalar(0.05)), Random::euler(randomGenerator, Numeric::deg2rad(0), Numeric::deg2rad(30)));

			OculusTags tags;

			oculusTagTracker.trackTagsStereo(*anyCameraA, *anyCameraB, yFrameA, yFrameB, world_T_device, device_T_cameraA, device_T_cameraB, tags);

			const OculusTagTracker::TrackedTagMap trackedTagMap = oculusTagTracker.trackedTagMap();

			if (tags.empty() == false || trackedTagMap.empty() == false)
			{
				// Because of the random data, the tracker is not expected to detect anything
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Stress test: succeeded.";
	}
	else
	{
		Log::info() << "Stress test: FAILED!";
	}

	return allSucceeded;
}

} // namespace TestTrackingOculusTag

} // namespace TestTracking

} // namespace Test

} // namespace Ocean
