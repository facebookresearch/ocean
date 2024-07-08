/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_TEST_TRACKING_TESTTRACKING_TEST_TRACKING_H
#define FACEBOOK_APPLICATION_OCEAN_TEST_TRACKING_TESTTRACKING_TEST_TRACKING_H

#include "application/ocean/test/tracking/ApplicationTestTracking.h"

/**
 * @ingroup applicationtesttracking
 * @defgroup applicationtesttrackingtesttracking Tracking Test
 * @{
 * The test application validates the accuracy and measures the performance of the Tracking library.<br>
 * This application is almost platform independent and is available on desktop platforms like e.g., Windows or OS X.
 *
 * The application takes up to three optional command arguments:<br>
 * Parameter 1: log output file e.g., "log.txt" or default ""<br>
 * Parameter 2: functions to test e.g., "database" or default "" (testing all functions)<br>
 * Parameter 3: duration for each test in seconds e.g., "2.5" or default ""
 *
 * Examples of command arguments:
 *
 * "" "" 0.5<br>
 * (output to the console, all functions will be tested, each test takes approx. 0.5 seconds)
 *
 * output.log "database"
 * (output to output.log file, one specific function will be tested, with default test duration)
 * @}
 */

#endif // FACEBOOK_APPLICATION_OCEAN_TEST_TRACKING_TESTTRACKING_TEST_TRACKING_H
