/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_OPEN_CV_H
#define META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_OPEN_CV_H

#include "ocean/test/testcv/TestCV.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestOpenCV
{

/**
 * @ingroup testcv
 *
 * @defgroup testcvopencv Ocean Test CV OpenCV Library
 * @{
 * The Ocean Test CV OpenCV Library provides several functions to test the performance of the 3rdparty OpenCV library.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::Test::TestCV::TestOpenCV Namespace of the Ocean CV OpenCV test library.<p>
 * The Namespace Ocean::Test::TestCV:TestOpencv is used in the entire Ocean CV OpenCV Library.
 */

// Defines OCEAN_TEST_CV_OPENCV_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_TEST_CV_OPENCV_EXPORT
		#define OCEAN_TEST_CV_OPENCV_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_TEST_CV_OPENCV_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_TEST_CV_OPENCV_EXPORT
#endif

/**
 * Tests the entire CV OpenCV library.
 * @param testDuration Number of seconds for each test, with range (0, infinity)
 * @param imageFilename The filename of the image to be used for testing
 * @param testFunctions Optional name of the functions to be tested
 * @return True, if the entire test succeeded
 * @ingroup testcvopencv
 */
OCEAN_TEST_CV_OPENCV_EXPORT bool testCVOpenCV(const double testDuration, const std::string& imageFilename, const std::string& testFunctions = std::string());

/**
 * Tests the entire CV OpenCV library.
 * This function returns directly as the actual test is invoked in an own thread.<br>
 * Use this function in intendet for non-console applications like e.g., mobile devices.
 * @param testDuration Number of seconds for each test, with range (0, infinity)
 * @param imageFilename The filename of the image to be used for testing
 * @param testFunctions Optional name of the functions to be tested
 * @ingroup testcvopencv
 */
OCEAN_TEST_CV_OPENCV_EXPORT void testCVOpenCVAsynchron(const double testDuration, const std::string& imageFilename, const std::string& testFunctions = std::string());


} // namespace TestOpenCV

} // namespace TestCV

} // namespace Test

} // namespace Ocean

#endif // META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_OPEN_CV_H
