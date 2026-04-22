/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_FREQUENCY_ANALYSIS_H
#define META_OCEAN_TEST_TESTCV_TEST_FREQUENCY_ANALYSIS_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/test/TestSelector.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements tests for the FrequencyAnalysis class
 * (FFT-based image2frequencies / frequencies2image).
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrequencyAnalysis
{
	public:

		/**
		 * Starts all tests of the FrequencyAnalysis class.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker Worker object to distribute computational load
		 * @param selector Test selector for filtering sub-tests; default runs all tests
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker, const TestSelector& selector = TestSelector());

		/**
		 * Tests image2frequencies / frequencies2image round-trip identity:
		 *  applying the FFT followed by the inverse FFT returns (approximately)
		 *  the original frame.
		 * @param testDuration Number of seconds for the test, with range (0, infinity)
		 * @param worker Worker object
		 * @return True, if succeeded
		 */
		static bool testRoundTripIdentity(const double testDuration, Worker& worker);

		/**
		 * Tests that the DC component of the frequency spectrum equals the sum
		 * of all pixel values for that channel (FFT DC == sum invariant).
		 * @param testDuration Number of seconds for the test, with range (0, infinity)
		 * @param worker Worker object
		 * @return True, if succeeded
		 */
		static bool testDCComponent(const double testDuration, Worker& worker);

		/**
		 * Tests Parseval's theorem for the discrete Fourier transform:
		 *  sum(|f(t)|^2) == (1/N) * sum(|F(k)|^2). For real-valued input this
		 *  holds per channel.
		 * @param testDuration Number of seconds for the test, with range (0, infinity)
		 * @param worker Worker object
		 * @return True, if succeeded
		 */
		static bool testParseval(const double testDuration, Worker& worker);

		/**
		 * Tests that magnitudeFrame() returns a valid Y8 frame of the requested
		 * size (basic shape/sanity invariant).
		 * @param testDuration Number of seconds for the test, with range (0, infinity)
		 * @param worker Worker object
		 * @return True, if succeeded
		 */
		static bool testMagnitudeFrame(const double testDuration, Worker& worker);

		/**
		 * Stress test: large random frames and many channel counts; verifies
		 * worker vs single-thread output equality for image2frequencies.
		 * @param testDuration Number of seconds for the test, with range (0, infinity)
		 * @param worker Worker object
		 * @return True, if succeeded
		 */
		static bool testWorkerEquivalenceStress(const double testDuration, Worker& worker);

		/**
		 * Stress test: round-trip identity holds for many random parameter
		 * combinations (sizes/pixelFormats/paddings).
		 * @param testDuration Number of seconds for the test, with range (0, infinity)
		 * @param worker Worker object
		 * @return True, if succeeded
		 */
		static bool testRoundTripStress(const double testDuration, Worker& worker);

		/**
		 * Stress test: a constant-input frame produces a frequency spectrum
		 * with a single non-zero DC component (per channel).
		 * @param testDuration Number of seconds for the test, with range (0, infinity)
		 * @param worker Worker object
		 * @return True, if succeeded
		 */
		static bool testConstantInputSpectrumStress(const double testDuration, Worker& worker);

		/**
		 * Stress test: source frames with random padding produce identical
		 * frequency output to padding-free frames carrying the same content.
		 * @param testDuration Number of seconds for the test, with range (0, infinity)
		 * @param worker Worker object
		 * @return True, if succeeded
		 */
		static bool testPaddingInvarianceStress(const double testDuration, Worker& worker);
};

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_FREQUENCY_ANALYSIS_H
