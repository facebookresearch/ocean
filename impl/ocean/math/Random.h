/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_RANDOM_H
#define META_OCEAN_MATH_RANDOM_H

#include "ocean/math/Math.h"

#include "ocean/math/Euler.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Numeric.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/Rotation.h"
#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/Vector2.h"
#include "ocean/math/Vector3.h"
#include "ocean/math/Vector4.h"

#include "ocean/base/RandomGenerator.h"
#include "ocean/base/RandomI.h"

namespace Ocean
{

template <typename T> class RandomT;

/**
 * Definition of the Random object, depending on the OCEAN_MATH_USE_SINGLE_PRECISION either with single or double precision float data type.
 * @see RandomT
 * @ingroup math
 */
typedef RandomT<Scalar> Random;

/**
 * Instantiation of the RandomT template class using a double precision float data type.
 * @see RandomT
 * @ingroup math
 */
typedef RandomT<double> RandomD;

/**
 * Instantiation of the RandomT template class using a single precision float data type.
 * @see RandomT
 * @ingroup math
 */
typedef RandomT<float> RandomF;

/**
 * This class provides several random functions for different data types using a floating point type for its elements that is specified by T.
 * Beware: All function must not be used without a initialize() call for each thread!
 * @tparam T Data type of passed and generated values
 * @see Random, RandomF, RandomD.
 * @ingroup math
 */
 template <typename T>
class RandomT : public RandomI
{
	public:

		/**
		 * Returns a random sign (either +1 or -1).
		 * @return The random sign
		 */
		static inline T sign();

		/**
		 * Returns a random sign (either +1 or -1).
		 * @param randomGenerator The random generator to be used
		 * @return The random sign
		 */
		static inline T sign(RandomGenerator& randomGenerator);

		/**
		 * Returns a random number between two borders.
		 * @param lower The lower border, with range (-infinity, infinity)
		 * @param upper The upper border, with range [lower, infinity)
		 * @return Random number, with range [lower, upper]
		 */
		static T scalar(const T lower, const T upper);

		/**
		 * Returns a random number between two borders using an explicit random generator.
		 * @param randomGenerator The random generator to be used
		 * @param lower The lower border, with range (-infinity, infinity)
		 * @param upper The upper border, with range [lower, infinity)
		 * @return Random number, with range [lower, upper]
		 */
		static T scalar(RandomGenerator& randomGenerator, const T lower, const T upper);

		/**
		 * Returns a random number with Gaussian distribution.
		 * The returned value lies inside the range [-5 * sigma, 5 * sigma].
		 * @param sigma The sigma parameter defining the standard deviation of the Gaussian distribution, with range (0, infinity)
		 * @return Gaussian distributed random value
		 */
		static T gaussianNoise(const T sigma);

		/**
		 * Returns a random number with Gaussian distribution using an explicit random generator.
		 * @param randomGenerator The random generator to be used
		 * The returned value lies inside the range [-5 * sigma, 5 * sigma].
		 * @param sigma The sigma parameter defining the standard deviation of the Gaussian distribution, with range (0, infinity)
		 * @return Gaussian distributed random value
		 */
		static T gaussianNoise(RandomGenerator& randomGenerator, const T sigma);

		/**
		 * Returns a random 2D vector with Gaussian distribution.
		 * The returned value lies inside the range [-5 * sigma, 5 * sigma]x[-5 * sigma, 5 * sigma].
		 * @param sigmaX The sigma parameter defining the standard deviation of the Gaussian distribution for the x value, with range (0, infinity)
		 * @param sigmaY The sigma parameter defining the standard deviation of the Gaussian distribution for the y value, with range (0, infinity)
		 * @return Gaussian distributed random 2D vector
		 */
		static VectorT2<T> gaussianNoiseVector2(const T sigmaX, const T sigmaY);

		/**
		 * Returns a random 2D vector with Gaussian distribution using an explicit random generator.
		 * The returned value lies inside the range [-5 * sigma, 5 * sigma]x[-5 * sigma, 5 * sigma].
		 * @param randomGenerator The random generator to be used
		 * @param sigmaX The sigma parameter defining the standard deviation of the Gaussian distribution for the x value, with range (0, infinity)
		 * @param sigmaY The sigma parameter defining the standard deviation of the Gaussian distribution for the y value, with range (0, infinity)
		 * @return Gaussian distributed random 2D vector
		 */
		static VectorT2<T> gaussianNoiseVector2(RandomGenerator& randomGenerator, const T sigmaX, const T sigmaY);

		/**
		 * Returns a random 2D vector with length 1 which is equal distributed within a circle.
		 * @return The resulting random 2D vector, with vector2().length() == 1
		 */
		static VectorT2<T> vector2();

		/**
		 * Returns a random 2D vector with length 1 which is equal distributed within a circle using an explicit random generator.
		 * @param randomGenerator The random generator to be used
		 * @return The resulting random 2D vector, with vector2().length() == 1
		 */
		static VectorT2<T> vector2(RandomGenerator& randomGenerator);

		/**
		 * Returns a random 2D vector with coordinates in a given range.
		 * @param min Minimum coordinate value for each axis, with range (-infinity, infinity)
		 * @param max Maximum coordinate value for each axis, with range [min, infinity)
		 * @return Random 2D vector with random length
		 */
		static VectorT2<T> vector2(const T min, const T max);

		/**
		 * Returns a random 2D vector with coordinates in a given range using an explicit random generator.
		 * @param randomGenerator The random generator to be used
		 * @param min Minimum coordinate value for each axis, with range (-infinity, infinity)
		 * @param max Maximum coordinate value for each axis, with range [min, infinity)
		 * @return Random 2D vector with random length
		 */
		static VectorT2<T> vector2(RandomGenerator& randomGenerator, const T min, const T max);

		/**
		 * Returns a random 2D vector with coordinates in a given range.
		 * @param xMin Minimum x coordinate value for each axis, with range (-infinity, infinity)
		 * @param xMax Maximum x coordinate value for each axis, with range [xMin, infinity)
		 * @param yMin Minimum x coordinate value for each axis, with range (-infinity, infinity)
		 * @param yMax Maximum x coordinate value for each axis, with range [yMin, infinity)
		 * @return Random 2D vector with random length
		 */
		static VectorT2<T> vector2(const T xMin, const T xMax, const T yMin, const T yMax);

		/**
		 * Returns a random 2D vector with coordinates in a given range using an explicit random generator.
		 * @param randomGenerator The random generator to be used
		 * @param xMin Minimum x coordinate value for each axis, with range (-infinity, infinity)
		 * @param xMax Maximum x coordinate value for each axis, with range [xMin, infinity)
		 * @param yMin Minimum x coordinate value for each axis, with range (-infinity, infinity)
		 * @param yMax Maximum x coordinate value for each axis, with range [yMin, infinity)
		 * @return Random 2D vector with random length
		 */
		static VectorT2<T> vector2(RandomGenerator& randomGenerator, const T xMin, const T xMax, const T yMin, const T yMax);

		/**
		 * Returns a random 3D vector with length 1 which is equal distributed within a sphere.
		 * @return The resulting random 3D vector, with vector3().length() == 1
		 */
		static VectorT3<T> vector3();

		/**
		 * Returns a random 3D vector with length 1 which is equal distributed within a sphere using an explicit random generator.
		 * @param randomGenerator The random generator to be used
		 * @return The resulting random 3D vector, with vector3().length() == 1
		 */
		static VectorT3<T> vector3(RandomGenerator& randomGenerator);

		/**
		 * Returns a random 3D vector with coordinates in a given range.
		 * @param min Minimum coordinate value for each axis, with range (-infinity, infinity)
		 * @param max Maximum coordinate value for each axis, with range [min, infinity)
		 * @return Random 3D vector with random length
		 */
		static VectorT3<T> vector3(const T min, const T max);

		/**
		 * Returns a random 3D vector with coordinates in a given range.
		 * @param range 3D vector defining the +/- ranges separately for each axis, with range [0, infinity] x [0, infinity] x [0, infinity]
		 * @return Random 3D vector with random length
		 */
		static VectorT3<T> vector3(const VectorT3<T>& range);

		/**
		 * Returns a random 3D vector with coordinates in a given range using an explicit random generator.
		 * @param randomGenerator The random generator to be used
		 * @param min Minimum coordinate value for each axis, with range (-infinity, infinity)
		 * @param max Maximum coordinate value for each axis, with range [min, infinity)
		 * @return Random 3D vector with random length
		 */
		static VectorT3<T> vector3(RandomGenerator& randomGenerator, const T min, const T max);

		/**
		 * Returns a random 3D vector with coordinates in a given range using an explicit random generator.
		 * @param randomGenerator The random generator to be used
		 * @param range 3D vector defining the +/- ranges separately for each axis, with range [0, infinity] x [0, infinity] x [0, infinity]
		 * @return Random 3D vector with random length
		 */
		static VectorT3<T> vector3(RandomGenerator& randomGenerator, const VectorT3<T>& range);

		/**
		 * Returns a random 3D vector with coordinates in a given range using an explicit random generator.
		 * @param randomGenerator The random generator to be used
		 * @param xMin Minimum coordinate value for x-axis, with range (-infinity, infinity)
		 * @param xMax Maximum coordinate value for x-axis, with range [xMin, infinity)
		 * @param yMin Minimum coordinate value for y-axis, with range (-infinity, infinity)
		 * @param yMax Maximum coordinate value for y-axis, with range [yMin, infinity)
		 * @param zMin Minimum coordinate value for z-axis, with range (-infinity, infinity)
		 * @param zMax Maximum coordinate value for z-axis, with range [zMin, infinity)
		 * @return Random 3D vector with random length
		 */
		static VectorT3<T> vector3(RandomGenerator& randomGenerator, const T xMin, const T xMax, const T yMin, const T yMax, const T zMin, const T zMax);

		/**
		 * Returns a random 4D vector with length 1 which is equal distributed within a hyper sphere.
		 * @return The resulting random 4D vector, with vector4().length() == 1
		 */
		static VectorT4<T> vector4();

		/**
		 * Returns a random 4D vector with length 1 which is equal distributed within a hyper sphere using an explicit random generator.
		 * @param randomGenerator The random generator to be used
		 * @return The resulting random 4D vector, with vector4().length() == 1
		 */
		static VectorT4<T> vector4(RandomGenerator& randomGenerator);

		/**
		 * Returns a random 4D vector with coordinates in a given range.
		 * @param min Minimum coordinate value for each axis, with range (-infinity, infinity)
		 * @param max Maximum coordinate value for each axis, with range [min, infinity)
		 * @return Random 4D vector with random length
		 */
		static VectorT4<T> vector4(const T min, const T max);

		/**
		 * Returns a random 4D vector with coordinates in a given range using an explicit random generator.
		 * @param randomGenerator The random generator to be used
		 * @param min Minimum coordinate value for each axis, with range (-infinity, infinity)
		 * @param max Maximum coordinate value for each axis, with range [min, infinity)
		 * @return Random 4D vector with random length
		 */
		static VectorT4<T> vector4(RandomGenerator& randomGenerator, const T min, const T max);

		/**
		 * Returns a random rotation as unit quaternion.
		 * @return Random rotation
		 */
		static QuaternionT<T> quaternion();

		/**
		 * Returns a random rotation as unit quaternion using an explicit random generator.
		 * @param randomGenerator The random generator to be used
		 * @return Random rotation
		 */
		static QuaternionT<T> quaternion(RandomGenerator& randomGenerator);

		/**
		 * Returns a random rotation.
		 * @return Random rotation
		 */
		static RotationT<T> rotation();

		/**
		 * Returns a random rotation using an explicit random generator.
		 * @param randomGenerator The random generator to be used
		 * @return Random rotation
		 */
		static RotationT<T> rotation(RandomGenerator& randomGenerator);

		/**
		 * Returns a random euler.
		 * @return Random euler
		 */
		static EulerT<T> euler();

		/**
		 * Returns a random euler with angles in a given range.
		 * @param range Scalar defining the +/- range for each angle axis in radian, with range [0, PI/2)
		 * @return Random euler
		 */
		static EulerT<T> euler(const T range);

		/**
		 * Returns a random euler with angles in a given range.
		 * This function allows to specified an angle range so that a minimal and maximal rotation is guaranteed.<br>
		 * First, three individual random angles are determined lying inside the specified range.<br>
		 * Second, the signs of the three angles are determined randomly (as the range is specified with positive values).<br>
		 * @param minRange Scalar defining the minimal range for each angle axis (in negative and positive direction) in radian, with range [0, PI/2)
		 * @param maxRange Scalar defining the minimal range for each angle axis (in negative and positive direction) in radian, with range [minRange, PI/2)
		 * @return Random euler
		 */
		static EulerT<T> euler(const T minRange, const T maxRange);

		/**
		 * Returns a random euler using an explicit random generator using an explicit random generator.
		 * @param randomGenerator The random generator to be used
		 * @return Random euler
		 */
		static EulerT<T> euler(RandomGenerator& randomGenerator);

		/**
		 * Returns a random euler with angles in a given range using an explicit random generator using an explicit random generator.
		 * @param randomGenerator The random generator to be used
		 * @param range Scalar defining the +/- range for each angle axis in radian, with range [0, PI/2)
		 * @return Random euler
		 */
		static EulerT<T> euler(RandomGenerator& randomGenerator, const T range);

		/**
		 * Returns a random euler with angles in a given range using an explicit random generator using an explicit random generator.
		 * This function allows to specified an angle range so that a minimal and maximal rotation is guaranteed.<br>
		 * First, three individual random angles are determined lying inside the specified range.<br>
		 * Second, the signs of the three angles are determined randomly (as the range is specified with positive values).<br>
		 * @param randomGenerator The random generator to be used
		 * @param minRange Scalar defining the minimal range for each angle axis in radian, with range [0, PI/2)
		 * @param maxRange Scalar defining the minimal range for each angle axis in radian, with range [minRange, PI/2)
		 * @return Random euler
		 */
		static EulerT<T> euler(RandomGenerator& randomGenerator, const T minRange, const T maxRange);

	protected:

		/**
		 * Returns the inverse of MAX_RAND.
		 * @return 1 / MAX_RAND
		 */
		static constexpr T inverseMaxRand();

		/**
		 * Returns the inverse of RandomGenerator::randMax().
		 * @return 1 / RandomGenerator::randMax()
		 */
		static constexpr T inverseMaxRandomGenerator();
};

template <typename T>
inline T RandomT<T>::sign()
{
	if (rand() % 2u == 0u)
	{
		return T(-1);
	}
	else
	{
		return T(1);
	}
}

template <typename T>
inline T RandomT<T>::sign(RandomGenerator& randomGenerator)
{
	if (randomGenerator.rand() % 2u == 0u)
	{
		return T(-1);
	}
	else
	{
		return T(1);
	}
}

template <typename T>
T RandomT<T>::scalar(const T lower, const T upper)
{
	ocean_assert(std::is_floating_point<T>::value);

	ocean_assert(upper >= lower);

	return T(rand()) * inverseMaxRand() * (upper - lower) + lower;
}

template <typename T>
T RandomT<T>::scalar(RandomGenerator& randomGenerator, const T lower, const T upper)
{
	ocean_assert(std::is_floating_point<T>::value);

	ocean_assert(upper >= lower);

	return T(randomGenerator.rand()) * inverseMaxRandomGenerator() * (upper - lower) + lower;
}

template <typename T>
T RandomT<T>::gaussianNoise(const T sigma)
{
	const T maxValue = NumericT<T>::gaussianDistribution(T(0.0), sigma);

#ifdef OCEAN_DEBUG
	unsigned int debugN = 0u;
#endif

	while (true)
	{
		const T randomValue = scalar(T(-5.0) * sigma, T(5.0) * sigma);
		const T check = scalar(T(0.0), maxValue);

		if (check <= NumericT<T>::gaussianDistribution(randomValue, sigma))
		{
			return randomValue;
		}

		ocean_assert(++debugN < 1000);
	}
}

template <typename T>
T RandomT<T>::gaussianNoise(RandomGenerator& randomGenerator, const T sigma)
{
	const T maxValue = NumericT<T>::gaussianDistribution(T(0.0), sigma);

#ifdef OCEAN_DEBUG
	unsigned int debugN = 0u;
#endif

	while (true)
	{
		const T randomValue = scalar(randomGenerator, T(-5.0) * sigma, T(5.0) * sigma);
		const T check = scalar(randomGenerator, T(0.0), maxValue);

		if (check <= NumericT<T>::gaussianDistribution(randomValue, sigma))
		{
			return randomValue;
		}

		ocean_assert(++debugN < 1000);
	}
}

template <typename T>
VectorT2<T> RandomT<T>::gaussianNoiseVector2(const T sigmaX, const T sigmaY)
{
	const T x = gaussianNoise(sigmaX);
	const T y = gaussianNoise(sigmaY);

	return VectorT2<T>(x, y);
}

template <typename T>
VectorT2<T> RandomT<T>::gaussianNoiseVector2(RandomGenerator& randomGenerator, const T sigmaX, const T sigmaY)
{
	const T x = gaussianNoise(randomGenerator, sigmaX);
	const T y = gaussianNoise(randomGenerator, sigmaY);

	return VectorT2<T>(x, y);
}

template <typename T>
VectorT2<T> RandomT<T>::vector2()
{
	while (true)
	{
		VectorT2<T> vector(scalar(T(-1.0), T(1.0)), scalar(T(-1.0), T(1.0)));

		if (vector.sqr() <= T(1.0) && vector.normalize())
		{
			return vector;
		}
	}

	ocean_assert(false && "This should never happen!");
	return VectorT2<T>(1, 0);
}

template <typename T>
VectorT2<T> RandomT<T>::vector2(RandomGenerator& randomGenerator)
{
	while (true)
	{
		const T x = scalar(randomGenerator, T(-1.0), T(1.0));
		const T y = scalar(randomGenerator, T(-1.0), T(1.0));

		VectorT2<T> vector(x, y);

		if (vector.sqr() <= T(1.0) && vector.normalize())
		{
			return vector;
		}
	}

	ocean_assert(false && "This should never happen!");
	return VectorT2<T>(1, 0);
}

template <typename T>
VectorT2<T> RandomT<T>::vector2(const T min, const T max)
{
	return VectorT2<T>(scalar(min, max), scalar(min, max));
}

template <typename T>
VectorT2<T> RandomT<T>::vector2(RandomGenerator& randomGenerator, const T min, const T max)
{
	const T x = scalar(randomGenerator, min, max);
	const T y = scalar(randomGenerator, min, max);

	return VectorT2<T>(x, y);
}

template <typename T>
VectorT2<T> RandomT<T>::vector2(const T xMin, const T xMax, const T yMin, const T yMax)
{
	return VectorT2<T>(scalar(xMin, xMax), scalar(yMin, yMax));
}

template <typename T>
VectorT2<T> RandomT<T>::vector2(RandomGenerator& randomGenerator, const T xMin, const T xMax, const T yMin, const T yMax)
{
	const T x = scalar(randomGenerator, xMin, xMax);
	const T y = scalar(randomGenerator, yMin, yMax);

	return VectorT2<T>(x, y);
}

template <typename T>
VectorT3<T> RandomT<T>::vector3(const VectorT3<T>& range)
{
	ocean_assert(range.x() >= T(0.0) && range.y() >= T(0.0) && range.z() >= T(0.0));

	return VectorT3<T>(scalar(-range.x(), range.x()), scalar(-range.y(), range.y()), scalar(-range.z(), range.z()));
}

template <typename T>
VectorT3<T> RandomT<T>::vector3(RandomGenerator& randomGenerator, const VectorT3<T>& range)
{
	ocean_assert(range.x() >= T(0.0) && range.y() >= T(0.0) && range.z() >= T(0.0));

	const T x = scalar(randomGenerator, -range.x(), range.x());
	const T y = scalar(randomGenerator, -range.y(), range.y());
	const T z = scalar(randomGenerator, -range.z(), range.z());

	return VectorT3<T>(x, y, z);
}

template <typename T>
VectorT3<T> RandomT<T>::vector3()
{
	while (true)
	{
		VectorT3<T> vector(scalar(T(-1.0), T(1.0)), scalar(T(-1.0), T(1.0)), scalar(T(-1.0), T(1.0)));

		if (vector.sqr() <= T(1.0) && vector.normalize())
		{
			return vector;
		}
	}

	ocean_assert(false && "This should never happen!");
	return VectorT3<T>(1, 0, 0);
}

template <typename T>
VectorT3<T> RandomT<T>::vector3(RandomGenerator& randomGenerator)
{
	while (true)
	{
		const T x = scalar(randomGenerator, T(-1.0), T(1.0));
		const T y = scalar(randomGenerator, T(-1.0), T(1.0));
		const T z = scalar(randomGenerator, T(-1.0), T(1.0));

		VectorT3<T> vector(x, y, z);

		if (vector.sqr() <= T(1.0) && vector.normalize())
		{
			return vector;
		}
	}

	ocean_assert(false && "This should never happen!");
	return VectorT3<T>(1, 0, 0);
}

template <typename T>
VectorT3<T> RandomT<T>::vector3(const T min, const T max)
{
	return VectorT3<T>(scalar(min, max), scalar(min, max), scalar(min, max));
}

template <typename T>
VectorT3<T> RandomT<T>::vector3(RandomGenerator& randomGenerator, const T min, const T max)
{
	const T x = scalar(randomGenerator, min, max);
	const T y = scalar(randomGenerator, min, max);
	const T z = scalar(randomGenerator, min, max);

	return VectorT3<T>(x, y, z);
}

template <typename T>
VectorT3<T> RandomT<T>::vector3(RandomGenerator& randomGenerator, const T xMin, const T xMax, const T yMin, const T yMax, const T zMin, const T zMax)
{
	const T x = scalar(randomGenerator, xMin, xMax);
	const T y = scalar(randomGenerator, yMin, yMax);
	const T z = scalar(randomGenerator, zMin, zMax);

	return VectorT3<T>(x, y, z);
}

template <typename T>
VectorT4<T> RandomT<T>::vector4()
{
	while (true)
	{
		VectorT4<T> vector(scalar(T(-1.0), T(1.0)), scalar(T(-1.0), T(1.0)), scalar(T(-1.0), T(1.0)), scalar(T(-1.0), T(1.0)));

		if (vector.sqr() <= T(1.0) && vector.normalize())
		{
			return vector;
		}
	}

	ocean_assert(false && "This should never happen!");
	return VectorT4<T>(1, 0, 0, 0);
}

template <typename T>
VectorT4<T> RandomT<T>::vector4(RandomGenerator& randomGenerator)
{
	while (true)
	{
		const T x = scalar(randomGenerator, T(-1.0), T(1.0));
		const T y = scalar(randomGenerator, T(-1.0), T(1.0));
		const T z = scalar(randomGenerator, T(-1.0), T(1.0));
		const T w = scalar(randomGenerator, T(-1.0), T(1.0));

		VectorT4<T> vector(x, y, z, w);

		if (vector.sqr() <= T(1.0) && vector.normalize())
		{
			return vector;
		}
	}

	ocean_assert(false && "This should never happen!");
	return VectorT4<T>(1, 0, 0, 0);
}

template <typename T>
VectorT4<T> RandomT<T>::vector4(const T min, const T max)
{
	return VectorT4<T>(scalar(min, max), scalar(min, max), scalar(min, max), scalar(min, max));
}

template <typename T>
VectorT4<T> RandomT<T>::vector4(RandomGenerator& randomGenerator, const T min, const T max)
{
	const T x = scalar(randomGenerator, min, max);
	const T y = scalar(randomGenerator, min, max);
	const T z = scalar(randomGenerator, min, max);
	const T w = scalar(randomGenerator, min, max);

	return VectorT4<T>(x, y, z, w);
}

template <typename T>
QuaternionT<T> RandomT<T>::quaternion()
{
	return QuaternionT<T>(vector3(), scalar(T(0.0), NumericT<T>::pi2() - NumericT<T>::eps()));
}

template <typename T>
QuaternionT<T> RandomT<T>::quaternion(RandomGenerator& randomGenerator)
{
	const VectorT3<T> axis = vector3(randomGenerator);
	const T angle = scalar(randomGenerator, T(0.0), NumericT<T>::pi2() - NumericT<T>::eps());

	return QuaternionT<T>(axis, angle);
}

template <typename T>
RotationT<T> RandomT<T>::rotation()
{
	return RotationT<T>(vector3(), scalar(T(0.0), NumericT<T>::pi2() - NumericT<T>::eps()));
}

template <typename T>
RotationT<T> RandomT<T>::rotation(RandomGenerator& randomGenerator)
{
	const VectorT3<T> axis = vector3(randomGenerator);
	const T angle = scalar(randomGenerator, T(0.0), NumericT<T>::pi2() - NumericT<T>::eps());

	return RotationT<T>(axis, angle);
}

template <typename T>
EulerT<T> RandomT<T>::euler()
{
	return EulerT<T>(scalar(-NumericT<T>::pi(), NumericT<T>::pi()), scalar(-NumericT<T>::pi_2(), NumericT<T>::pi_2()), scalar(-NumericT<T>::pi(), NumericT<T>::pi()));
}

template <typename T>
EulerT<T> RandomT<T>::euler(const T range)
{
	ocean_assert(range >= T(0.0) && range < NumericT<T>::pi_2());

	return EulerT<T>(scalar(-range, range), scalar(-range, range), scalar(-range, range));
}

template <typename T>
EulerT<T> RandomT<T>::euler(const T minRange, const T maxRange)
{
	ocean_assert(minRange >= T(0.0) && minRange < NumericT<T>::pi_2());
	ocean_assert(maxRange >= T(0.0) && maxRange < NumericT<T>::pi_2());
	ocean_assert(minRange <= maxRange);

	return EulerT<T>(scalar(minRange, maxRange) * sign(), scalar(minRange, maxRange) * sign(), scalar(minRange, maxRange) * sign());
}

template <typename T>
EulerT<T> RandomT<T>::euler(RandomGenerator& randomGenerator)
{
	const T yaw = scalar(randomGenerator, -NumericT<T>::pi(), NumericT<T>::pi());
	const T pitch = scalar(randomGenerator, -NumericT<T>::pi_2(), NumericT<T>::pi_2());
	const T roll = scalar(randomGenerator, -NumericT<T>::pi(), NumericT<T>::pi());

	return EulerT<T>(yaw, pitch, roll);
}

template <typename T>
EulerT<T> RandomT<T>::euler(RandomGenerator& randomGenerator, const T range)
{
	ocean_assert(range >= T(0.0) && range < NumericT<T>::pi_2());

	const T yaw = scalar(randomGenerator, -range, range);
	const T pitch = scalar(randomGenerator, -range, range);
	const T roll = scalar(randomGenerator, -range, range);

	return EulerT<T>(yaw, pitch, roll);
}

template <typename T>
EulerT<T> RandomT<T>::euler(RandomGenerator& randomGenerator, const T minRange, const T maxRange)
{
	ocean_assert(minRange >= T(0.0) && minRange < NumericT<T>::pi_2());
	ocean_assert(maxRange >= T(0.0) && maxRange < NumericT<T>::pi_2());
	ocean_assert(minRange <= maxRange);

	const T yawSign = sign(randomGenerator);
	const T yaw = scalar(randomGenerator, minRange, maxRange) * yawSign;

	const T pitchSign = sign(randomGenerator);
	const T pitch = scalar(randomGenerator, minRange, maxRange) * pitchSign;

	const T rollSign = sign(randomGenerator);
	const T roll = scalar(randomGenerator, minRange, maxRange) * rollSign;

	return EulerT<T>(yaw, pitch, roll);
}

template <typename T>
constexpr T RandomT<T>::inverseMaxRand()
{
	return T(1.0) / T(RAND_MAX);
}

template <typename T>
constexpr T RandomT<T>::inverseMaxRandomGenerator()
{
	return T(1.0) / T(RandomGenerator::randMax());
}

}

#endif // META_OCEAN_MATH_RANDOM_H
