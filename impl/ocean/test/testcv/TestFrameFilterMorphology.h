/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_MORPHOLOGY_H
#define META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_MORPHOLOGY_H

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
 * This class implements tests for the FrameFilterMorphology class (open/close mask helpers).
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameFilterMorphology
{
	public:

		/**
		 * Starts all tests of the FrameFilterMorphology class.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker Worker object to distribute computational load
		 * @param selector Test selector for filtering sub-tests; default runs all tests
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker, const TestSelector& selector = TestSelector());

		/**
		 * Tests openMask<MF_SQUARE_3>() basic invariants on small random masks.
		 * @param testDuration Number of seconds for the test, with range (0, infinity)
		 * @param worker Worker object
		 * @return True, if succeeded
		 */
		static bool testOpenMaskSquare3(const double testDuration, Worker& worker);

		/**
		 * Tests openMask<MF_SQUARE_5>() basic invariants on small random masks.
		 * @param testDuration Number of seconds for the test, with range (0, infinity)
		 * @param worker Worker object
		 * @return True, if succeeded
		 */
		static bool testOpenMaskSquare5(const double testDuration, Worker& worker);

		/**
		 * Tests closeMask<MF_SQUARE_3>() basic invariants on small random masks.
		 * @param testDuration Number of seconds for the test, with range (0, infinity)
		 * @param worker Worker object
		 * @return True, if succeeded
		 */
		static bool testCloseMaskSquare3(const double testDuration, Worker& worker);

		/**
		 * Tests closeMask<MF_SQUARE_5>() basic invariants on small random masks.
		 * @param testDuration Number of seconds for the test, with range (0, infinity)
		 * @param worker Worker object
		 * @return True, if succeeded
		 */
		static bool testCloseMaskSquare5(const double testDuration, Worker& worker);

		/**
		 * Tests that an all-mask frame is preserved by open and close.
		 * @return True, if succeeded
		 */
		static bool testIdempotentOnFullFrame();

		/**
		 * Tests that an all-non-mask frame stays all-non-mask under open/close.
		 * @return True, if succeeded
		 */
		static bool testEmptyFrameStaysEmpty();

		/**
		 * Stress test: run open/close repeatedly on random masks across all
		 * filter shapes, varied paddings, and worker on/off, checking only
		 * that the call returns and produces a valid mask buffer.
		 * @param testDuration Number of seconds for the test, with range (0, infinity)
		 * @param worker Worker object
		 * @return True, if succeeded
		 */
		static bool testRandomMasksStress(const double testDuration, Worker& worker);

		/**
		 * Stress test: compare worker vs single-thread output for the same
		 * input mask across all filter shapes.
		 * @param testDuration Number of seconds for the test, with range (0, infinity)
		 * @param worker Worker object
		 * @return True, if succeeded
		 */
		static bool testWorkerEquivalenceStress(const double testDuration, Worker& worker);

		/**
		 * Stress test: open() removes single-pixel noise mask blobs (specks).
		 * @param testDuration Number of seconds for the test, with range (0, infinity)
		 * @param worker Worker object
		 * @return True, if succeeded
		 */
		static bool testOpenRemovesSpecksStress(const double testDuration, Worker& worker);

		/**
		 * Stress test: close() fills isolated single-pixel holes inside a
		 * mostly-mask region.
		 * @param testDuration Number of seconds for the test, with range (0, infinity)
		 * @param worker Worker object
		 * @return True, if succeeded
		 */
		static bool testCloseFillsHolesStress(const double testDuration, Worker& worker);

		/**
		 * Stress test: random mask values (not just 0/255) leave non-mask
		 * pixels untouched and only modify mask-valued pixels in expected
		 * ways.
		 * @param testDuration Number of seconds for the test, with range (0, infinity)
		 * @param worker Worker object
		 * @return True, if succeeded
		 */
		static bool testNonMaskPixelsUntouchedStress(const double testDuration, Worker& worker);
};

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_MORPHOLOGY_H
