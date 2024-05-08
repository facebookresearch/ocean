/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestInterpolation.h"

#include "ocean/base/Timestamp.h"

#include "ocean/math/Interpolation.h"
#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestInterpolation::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Interpolation test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testLinear(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testBilinear(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testBilinearSubset(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Interpolation test succeeded.";
	}
	else
	{
		Log::info() << "Interpolation test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestInterpolation, Linear)
{
	EXPECT_TRUE(TestInterpolation::testLinear(GTEST_TEST_DURATION));
}

TEST(TestInterpolation, Bilinear)
{
	EXPECT_TRUE(TestInterpolation::testBilinear(GTEST_TEST_DURATION));
}

TEST(TestInterpolation, BilinearSubset)
{
	EXPECT_TRUE(TestInterpolation::testBilinearSubset(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestInterpolation::testLinear(const double /*testDuration*/)
{
	Log::info() << "Linear test:";

	bool allSucceeded = true;

	// 1D scalar

	if (NumericF::isNotWeakEqual(Interpolation::linear(0.0f, 2.0f, float(0.8)), 1.6f) || NumericF::isNotWeakEqual(Interpolation::linear(0.0f, 2.0f, double(0.8)), 1.6f))
	{
		allSucceeded = false;
	}

	if (NumericD::isNotWeakEqual(Interpolation::linear(0.0, 2.0, float(0.8)), 1.6) || NumericD::isNotWeakEqual(Interpolation::linear(0.0, 2.0, double(0.8)), 1.6))
	{
		allSucceeded = false;
	}

	if (Numeric::isNotWeakEqual(Interpolation::linear(Scalar(0.0), Scalar(2), Scalar(0.8)), Scalar(1.6)) || Numeric::isNotWeakEqual(Interpolation::linear(Scalar(0), Scalar(2.0), Scalar(0.8)), Scalar(1.6)))
	{
		allSucceeded = false;
	}


	// 2D vector

	if (!Interpolation::linear(VectorF2(1.0f, 2.0f), VectorF2(2.0f, 4.0f), float(0.5)).isEqual(VectorF2(1.5f, 3.0f), NumericF::weakEps()))
	{
		allSucceeded = false;
	}

	if (!Interpolation::linear(VectorD2(1.0, 2.0), VectorD2(2.0, 4.0), float(0.5)).isEqual(VectorD2(1.5, 3.0), NumericD::weakEps())
		|| !Interpolation::linear(VectorD2(1.0, 2.0), VectorD2(2.0, 4.0), double(0.5)).isEqual(VectorD2(1.5, 3.0), NumericD::weakEps()))
	{
		allSucceeded = false;
	}

	if (!Interpolation::linear(Vector2(Scalar(1.0), Scalar(2.0)), Vector2(Scalar(2.0), Scalar(4.0)), Scalar(0.5)).isEqual(Vector2(Scalar(1.5), Scalar(3.0)), Numeric::weakEps()))
	{
		allSucceeded = false;
	}


	// 3D vector

	if (!Interpolation::linear(VectorF3(1.0f, 2.0f, 3.0f), VectorF3(2.0f, 4.0f, 6.0f), float(0.5)).isEqual(VectorF3(1.5f, 3.0f, 4.5f), NumericF::weakEps()))
	{
		allSucceeded = false;
	}

	if (!Interpolation::linear(VectorD3(1.0, 2.0, 3.0), VectorD3(2.0, 4.0, 6.0), float(0.5)).isEqual(VectorD3(1.5, 3.0, 4.5), NumericD::weakEps())
		|| !Interpolation::linear(VectorD3(1.0, 2.0, 3.0), VectorD3(2.0, 4.0, 6.0), double(0.5)).isEqual(VectorD3(1.5, 3.0, 4.5), NumericD::weakEps()))
	{
		allSucceeded = false;
	}

	if (!Interpolation::linear(Vector3(Scalar(1.0), Scalar(2.0), Scalar(3.0)), Vector3(Scalar(2.0), Scalar(4.0), Scalar(6.0)), Scalar(0.5)).isEqual(Vector3(Scalar(1.5), Scalar(3.0), Scalar(4.5)), Numeric::weakEps()))
	{
		allSucceeded = false;
	}


	// Quaternion

	if (Interpolation::linear(QuaternionF(VectorF3(1.0f, 0.0f, 0.0f), 0.5f), QuaternionF(VectorF3(1.0f, 0.0f, 0.0f), 0.7f), float(0.5)) != QuaternionF(VectorF3(1.0f, 0.0f, 0.0f), 0.6f))
	{
		allSucceeded = false;
	}

	if (Interpolation::linear(QuaternionD(VectorD3(1.0, 0.0, 0.0), 0.5), QuaternionD(VectorD3(1.0, 0.0, 0.0), 0.7), float(0.5)) != QuaternionD(VectorD3(1.0, 0.0, 0.0), 0.6))
	{
		allSucceeded = false;
	}

	if (Interpolation::linear(QuaternionD(VectorD3(1.0, 0.0, 0.0), 0.5), QuaternionD(VectorD3(1.0, 0.0, 0.0), 0.7), double(0.5)) != QuaternionD(VectorD3(1.0, 0.0, 0.0), 0.6))
	{
		allSucceeded = false;
	}

	if (Interpolation::linear(Quaternion(Vector3(Scalar(1.0), Scalar(0.0), Scalar(0.0)), Scalar(0.5)), Quaternion(Vector3(Scalar(1.0), Scalar(0.0), Scalar(0.0)), Scalar(0.7)), Scalar(0.5)) != Quaternion(Vector3(Scalar(1.0), Scalar(0.0), Scalar(0.0)), Scalar(0.6)))
	{
		allSucceeded = false;
	}

	if (allSucceeded)
	{
		Log::info() << "Validation succeeded.";
	}
	else
	{
		Log::info() << "Validation FAILED!";
	}

	return allSucceeded;
}

bool TestInterpolation::testBilinear(const double testDuration)
{
	ocean_assert(testDuration >= 0.0);

	Log::info() << "Bilinear test:";

	// for 32 bit float values we need to weaken the epsilon by one magnitude
	const Scalar epsilon = std::is_same<Scalar, float>::value ? Numeric::eps() * Scalar(10) : Numeric::eps();

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			// v00 v01
			// v10 v11

			const Scalar v00 = Random::scalar(-100, 100);
			const Scalar v01 = Random::scalar(-100, 100);
			const Scalar v10 = Random::scalar(-100, 100);
			const Scalar v11 = Random::scalar(-100, 100);

			const Scalar tx = Random::scalar(0, 1);
			const Scalar ty = Random::scalar(0, 1);

			const Scalar result = Interpolation::bilinear(v00, v01, v10, v11, tx, ty);

			const Scalar tx_ = Scalar(1) - tx;
			const Scalar ty_ = Scalar(1) - ty;

			ocean_assert(tx_ >= 0 && tx_ <= 1);
			ocean_assert(ty_ >= 0 && ty_ <= 1);

			const Scalar test = v00 * tx_ * ty_ + v01 * tx * ty_ + v10 * tx_ * ty + v11 * tx * ty;

			if (Numeric::isEqual(result, test, epsilon))
				validIterations++;

			iterations++;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

bool TestInterpolation::testBilinearSubset(const double testDuration)
{
	ocean_assert(testDuration >= 0.0);

	Log::info() << "Bilinear subset test:";

	// for 32 bit float values we need to weaken the epsilon by one magnitude
	const Scalar epsilon = std::is_same<Scalar, float>::value ? Numeric::eps() * Scalar(10) : Numeric::eps();

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			// v00 v01
			// v10 v11

			const Scalar v00 = Random::scalar(-100, 100);
			const Scalar v01 = Random::scalar(-100, 100);
			const Scalar v10 = Random::scalar(-100, 100);
			const Scalar v11 = Random::scalar(-100, 100);

			const Scalar tx = Random::scalar(0, 1);
			const Scalar ty = Random::scalar(0, 1);

			bool b00 = RandomI::random(1u) == 0u ? true : false;
			const bool b01 = RandomI::random(1u) == 0u ? true : false;
			const bool b10 = RandomI::random(1u) == 0u ? true : false;
			const bool b11 = RandomI::random(1u) == 0u ? true : false;

			if (!b00 && !b01 && !b10 && !b11)
				b00 = true;

			const Scalar result = Interpolation::bilinear(v00, v01, v10, v11, b00, b01, b10, b11, tx, ty);

			if (b00 && b11 && !b01 && !b10)
			{
				// 1 0
				// 0 1

				const Scalar t = Numeric::sqrt(tx * tx + ty * ty) / Numeric::sqrt(2);
				const Scalar t_ = Scalar(1) - t;

				ocean_assert(t >= 0 && t <= 1 && t_ >= 0 && t_ <= 1);

				const Scalar test = v00 * t_ + v11 * t;

				if (Numeric::isEqual(result, test, epsilon))
					validIterations++;
			}
			else if (!b00 && !b11 && b01 && b10)
			{
				// 0 1
				// 1 0

				const Scalar ty_ = Scalar(1) - ty;
				ocean_assert(ty_ >= 0 && ty_ <= 1);

				const Scalar t = Numeric::sqrt(tx * tx + ty_ * ty_) / Numeric::sqrt(2);
				const Scalar t_ = Scalar(1) - t;

				ocean_assert(t >= 0 && t <= 1 && t_ >= 0 && t_ <= 1);

				const Scalar test = v10 * t_ + v01 * t;

				if (Numeric::isEqual(result, test, epsilon))
					validIterations++;
			}
			else
			{
				const Scalar tx_ = Scalar(1) - tx;
				const Scalar ty_ = Scalar(1) - ty;

				ocean_assert(tx_ >= 0 && tx_ <= 1);
				ocean_assert(ty_ >= 0 && ty_ <= 1);

				Scalar top = 0, bottom = 0, test = 0;

				if (b00 && b01)
					top = v00 * tx_ + v01 * tx;
				else if (b00)
					top = v00;
				else if (b01)
					top = v01;

				if (b10 && b11)
					bottom = v10 * tx_ + v11 * tx;
				else if (b10)
					bottom = v10;
				else if (b11)
					bottom = v11;

				if ((b00 || b01) && (b10 || b11))
					test = top * ty_ + bottom * ty;
				else if (b00 || b01)
					test = top;
				else if (b10 || b11)
					test = bottom;

				if (Numeric::isEqual(result, test, epsilon))
					validIterations++;
			}

			iterations++;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

}

}

}
