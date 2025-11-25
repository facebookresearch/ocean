/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_FREAK_DESCRIPTOR_H
#define META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_FREAK_DESCRIPTOR_H

#include "ocean/test/testcv/testdetector/TestCVDetector.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/cv/detector/FREAKDescriptor.h"

#include "ocean/math/PinholeCamera.h"

#include "ocean/test/TestSelector.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

/// Forward-declaration of the test class for FREAK descriptors
template <size_t tSize>
class TestFREAKDescriptorT;

/// Typedef for the test class of the 32-bytes long FREAK descriptors
using TestFREAKDescriptor32 = TestFREAKDescriptorT<32>;

/// Typedef for the test class of the 64-bytes long FREAK descriptors
using TestFREAKDescriptor64 = TestFREAKDescriptorT<64>;

/**
 * This class implements a test for the FREAK descriptor.
 * @tparam tSize The size of the FREAK descriptor to test, in bytes, either 32 or 64
 * @ingroup testcvdetector
 */
template <size_t tSize>
class OCEAN_TEST_CV_DETECTOR_EXPORT TestFREAKDescriptorT : public CV::Detector::FREAKDescriptorT<tSize>
{
	public:

		/**
		 * Invokes all test for the FREAK descriptor.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @param selector The test selector to filter tests
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker, const TestSelector& selector);

		/**
		 * Test the function creating a blurred frame pyramid.
		 * @param testDuration Number of seconds that this test will be run, range: (0, infinity)
		 * @param worker The worker object
		 * @return True if the test has passed, otherwise false
		 */
		static bool testCreateBlurredFramePyramid(const double testDuration, Worker& worker);
};

#ifdef OCEAN_USE_EXTERNAL_TEST_FREAK_DESCRIPTOR

/**
 * Invokes all test for the FREAK descriptor.
 * @param testDuration Number of seconds for each test, with range (0, infinity)
 * @param worker The worker object
 * @return True, if succeeded
 * @tparam tSize The size of the FREAK descriptor in bytes, valid values: {32, 64}
 */
template <size_t tSize>
bool TestFREAKDescriptorT_externalTests(const double testDuration, Worker& worker);

#endif // OCEAN_USE_EXTERNAL_TEST_FREAK_DESCRIPTOR

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namespace Ocean

#endif // META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_FREAK_DESCRIPTOR_H
