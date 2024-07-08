/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTGEOMETRY_TEST_P3P_H
#define META_OCEAN_TEST_TESTGEOMETRY_TEST_P3P_H

#include "ocean/test/testgeometry/TestGeometry.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

/**
 * This class implements a P3P test.
 * @ingroup testgeometry
 */
class OCEAN_TEST_GEOMETRY_EXPORT TestP3P
{
	public:

		/**
		 * Invokes all tests for the perspective pose problem for three points.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Deprecated.
		 *
		 * Tests the perspective pose problem for three image points and a pinhole camera.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testP3PWithPointsPinholeCamera(const double testDuration);

		/**
		 * Tests the perspective pose problem for three image points and any camera.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The floating point data type to be used for testing
		 */
		template <typename T>
		static bool testP3PWithPoints(const double testDuration);

		/**
		 * Tests the perspective pose problem for three rays.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The floating point data type to be used for testing
		 */
		template <typename T>
		static bool testP3PWithRays(const double testDuration);

		/**
		 * Tests whether the perspective pose problem for three points does not crash and whether basic requirements hold.
		 * This test simply creates random data without any meaning and invokes the function to ensure that the function never crashes.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The floating point data type to be used for testing
		 */
		template <typename T>
		static bool testP3PWithPointsStressTest(const double testDuration);

		/**
		 * Tests whether the perspective pose problem for three rays does not crash.
		 * This test simply creates random data without any meaning and invokes the function to ensure that the function never crashes.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The floating point data type to be used for testing
		 */
		template <typename T>
		static bool testP3PWithRaysStressTest(const double testDuration);

	private:

		/**
		 * Creates a random 3D vector fitting into a 3D bounding box of size 20,000 x 20,000 x 20,000.
		 * @param randomGenerator The random generator object to be used
		 * @return The random 3D vector, with range [-10000, 10000]x[-10000, 10000]x[-10000, 10000]
		 * @tparam T The data type of each element
		 */
		template <typename T>
		static VectorT3<T> randomVector(RandomGenerator& randomGenerator);
};

}

}

}

#endif // META_OCEAN_TEST_TESTGEOMETRY_TEST_P3P_H
