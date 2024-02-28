// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_TEST_TESTCV_TEST_NEON_H
#define META_OCEAN_TEST_TESTCV_TEST_NEON_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/base/RandomGenerator.h"

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements a Computer Vision NEON test.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestNEON
{
	protected:

		/**
		 * Definition of a pointer to a function allowing to averaging 2x2 pixel blocks.
		 * @param source0 The upper source row, must be valid
		 * @param source1 The lower source row, must be valid
		 * @param target The resulting averaged elements, must be valid
		 */
		typedef void (*AveragingFunction)(const unsigned char* const source0, const unsigned char* const source1, unsigned char* const target);

	public:

		/**
		 * Invokes all tests of the NEON class.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the NEON-based calculation of 3x3 block sums of 16 bit values.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if all result are valid
		 */
		static bool testSum16Bit4Blocks3x3(const double testDuration);

		/**
		 * Tests the NEON-based averaging of 2x2 pixel blocks.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if all result are valid
		 */
		static bool testAveragingElements2x2(const double testDuration);

		/**
		 * Tests the NEON-based multiply functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if all result are valid
		 */
		static bool testMultiply(const double testDuration);

		/**
		 * Tests the NEON-based copySign functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if all result are valid
		 */
		static bool testCopySign(const double testDuration);

		/**
		 * Tests the NEON-based cast functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if all result are valid
		 */
		static bool testCastElements(const double testDuration);

	protected:

		/*
		 * Validates the NEON-based averaging of 2x2 pixel blocks.
		 * @param channels The number of data channels, with range [1, infinity)
		 * @param sourceElements The number of source elements to be averaged, with range [2, infinity), must be even
		 * @param averagingFunction The actual averaging function to be tested, must be valid
		 * @param randomGenerator The random generator object to be used
		 * @return True, if succeeded
		 */
		static bool validateAveragingElements2x2(const unsigned int channels, const unsigned int sourceElements, const AveragingFunction averagingFunction, RandomGenerator& randomGenerator);
};

}

}

}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

#endif // META_OCEAN_TEST_TESTCV_TEST_NEON_H
