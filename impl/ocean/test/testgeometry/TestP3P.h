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
		 * Tests the perspective pose problem for three image points and a pinhole camera.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testP3PWithPointsPinholeCamera(const double testDuration);

		/**
		 * Tests the perspective pose problem for three image points and a fisheye camera.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testP3PWithPointsFisheyeCamera(const double testDuration);

		/**
		 * Tests the perspective pose problem for three image points and any camera.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testP3PWithPointsAnyCamera(const double testDuration);

		/**
		 * Tests the perspective pose problem for three rays.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testP3PWithRays(const double testDuration);

		/**
		 * Tests whether the perspective pose problem for three rays does not crash.
		 * This test simply creates random data without any meaning and invokes the function to ensure that the function never crashes.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The floating point data type to be used for testing
		 */
		template <typename T>
		static bool testP3PWithRaysStressTest(const double testDuration);

		/**
		 * Tests the RANSAC implementation of the perspective pose problem for three random points and a pinhole camera.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testRANSACP3PPinholeCamera(const double testDuration);

		/**
		 * Tests the RANSAC implementation of the perspective pose problem for three random points including unknown zoom factor.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testRANSACP3PZoom(const double testDuration);

	private:

		/**
		 * Creates a random 3D vector fitting into a 3D bounding box of size 20,000 x 20,000 x 20,000.
		 * @param randomGenerator The random generator object to be used
		 * @return The random 3D vector, with range [-10000, 10000]x[-10000, 10000]x[-10000, 10000]
		 * @tparam T The data type of each element
		 */
		template <typename T>
		static inline VectorT3<T> randomVector(RandomGenerator& randomGenerator);
};

template <typename T>
inline VectorT3<T> TestP3P::randomVector(RandomGenerator& randomGenerator)
{
	const VectorT3<T> vector(RandomT<T>::scalar(randomGenerator, -1, 1), RandomT<T>::scalar(randomGenerator, -1, 1), RandomT<T>::scalar(randomGenerator, -1, 1));
	const T scale = RandomT<T>::scalar(randomGenerator, -10, 10) * RandomT<T>::scalar(randomGenerator, -10, 10) * RandomT<T>::scalar(randomGenerator, -10, 10) * RandomT<T>::scalar(randomGenerator, -10, 10);

	return vector * scale;
}

}

}

}

#endif // META_OCEAN_TEST_TESTGEOMETRY_TEST_P3P_H
