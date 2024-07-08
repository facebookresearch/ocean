/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMATH_TEST_RANDOM_H
#define META_OCEAN_TEST_TESTMATH_TEST_RANDOM_H

#include "ocean/test/testmath/TestMath.h"

#include "ocean/base/RandomGenerator.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class tests the performance of the random functionalities.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestRandom
{
	public:

		/**
		 * Executes the entire random tests.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the performance of the standard random function using a single thread.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testStandardRandomSingleThreaded(const double testDuration);

		/**
		 * Tests the performance of the standard random function using multiple threads.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testStandardRandomMultiThreaded(const double testDuration);

		/**
		 * Tests the performance of the Ocean random function using a single thread.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testOceanRandomSingleThreaded(const double testDuration);

		/**
		 * Tests the performance of the Ocean random function using multiple threads.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testOceanRandomMultiThreaded(const double testDuration);

		/**
		 * Tests the performance of the standard triple random function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testStandardRandomTriple(const double testDuration);

		/**
		 * Tests the performance of the Ocean triple random function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testOceanRandomTriple(const double testDuration);

		/**
		 * Tests the random 3D vector with specified ranges function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testStandardRandomVector3(const double testDuration);

		/**
		 * Tests the Ocean random 3D vector with specified ranges function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testOceanRandomVector3(const double testDuration);

		/**
		 * Tests the random euler with specified range function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testStandardRandomEuler(const double testDuration);

		/**
		 * Tests the Ocean random euler with specified range function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testOceanRandomEuler(const double testDuration);
	
	private:

		/**
		 * Calculates random variables for a subset of the given data using the standard random function.
		 * @param values Values receiving the random values
		 * @param firstValue First value to be handled
		 * @param numberValues Number of values to be handled
		 */
		static void calculateStandardRandomValues(int* values, const unsigned int firstValue, const unsigned int numberValues);

		/**
		 * Calculates random variables for a subset of the given data using an own implementation of the random function.
		 * @param generator Random number generator used for initialization of the local generator
		 * @param values Values receiving the random values
		 * @param firstValue First value to be handled
		 * @param numberValues Number of values to be handled
		 */
		static void calculateOceanRandomValues(RandomGenerator* generator, int* values, const unsigned int firstValue, const unsigned int numberValues);
};

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_TEST_RANDOM_H
