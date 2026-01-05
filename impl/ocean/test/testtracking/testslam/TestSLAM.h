/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTTRACKING_TESTSLAM_TESTSLAM_H
#define META_OCEAN_TEST_TESTTRACKING_TESTSLAM_TESTSLAM_H

#include "ocean/test/testtracking/TestTracking.h"

#include "ocean/tracking/slam/SLAM.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace Test
{

namespace TestTracking
{

namespace TestSLAM
{

/**
 * @ingroup testtracking
 * @defgroup testtrackingtestslam Ocean Test Tracking SLAM Library
 * @{
 * The Ocean Test Tracking SLAM Library provides several functions to test the performance and validation of the Ocean Tracking SLAM Library.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::Test::TestTracking::TestSLAM Namespace of the SLAM Tracking Test library.<p>
 * The Namespace Ocean::Test::TestTracking::TestSLAM is used in the entire Ocean SLAM Tracking Test Library.
 */

// Defines OCEAN_TEST_TRACKING_SLAM_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_TEST_TRACKING_SLAM_EXPORT
		#define OCEAN_TEST_TRACKING_SLAM_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_TEST_TRACKING_SLAM_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_TEST_TRACKING_SLAM_EXPORT
#endif

/**
 * Tests the entire SLAM tracking library.
 * @param testDuration Number of seconds for each test, with range (0, infinity)
 * @param worker The worker object to distribute some computation on as many CPU cores as defined in the worker object.
 * @param testFunctions Optional name of the functions to be tested
 * @return True, if the entire test succeeded
 * @ingroup testtrackingtestslam
 */
OCEAN_TEST_TRACKING_SLAM_EXPORT bool testSLAM(const double testDuration, Worker& worker, const std::string& testFunctions = std::string());

/**
 * Tests the entire SLAM tracking library.
 * This function returns directly as the actual test is invoked in an own thread.<br>
 * This function is intended for non-console applications like e.g., mobile devices.
 * @param testDuration Number of seconds for each test, with range (0, infinity)
 * @param testFunctions Optional name of the functions to be tested
 * @ingroup testtrackingtestslam
 */
OCEAN_TEST_TRACKING_SLAM_EXPORT void testSLAMAsynchron(const double testDuration, const std::string& testFunctions = std::string());

}

}

}

}

#endif // META_OCEAN_TEST_TESTTRACKING_TESTSLAM_TESTSLAM_H
