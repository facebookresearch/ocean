// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "application/ocean/test/unifiedfeatures/ApplicationTestUnifiedFeatures.h"

/**
 * @ingroup applicationtestunifiedfeatures
 * @defgroup applicationtestunifiedfeaturestestunifiedfeatures Unified Features Test
 * @{
 * The test application validates the accuracy and measures the performance of the Unified Features library.<br>
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
