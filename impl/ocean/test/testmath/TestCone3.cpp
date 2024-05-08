/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestCone3.h"

#include "ocean/base/Timestamp.h"

#include "ocean/math/Cone3.h"
#include "ocean/math/Random.h"
#include "ocean/math/SquareMatrix3.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestCone3::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Cone3 test:   ---";
	Log::info() << " ";

	bool allSucceeded = testConstructor();

	Log::info() << " ";

	allSucceeded = testNearestIntersection<float>() && allSucceeded;

	Log::info() << " ";

	allSucceeded = testNearestIntersection<double>() && allSucceeded;

	Log::info() << " ";

	allSucceeded = validateNearestIntersection<float>(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = validateNearestIntersection<double>(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Cone3 test succeeded.";
	}
	else
	{
		Log::info() << "Cone3 test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestCone3, Constructor)
{
	EXPECT_TRUE(TestCone3::testConstructor());
}

TEST(TestCone3, NearestIntersectionFloat)
{
	EXPECT_TRUE(TestCone3::testNearestIntersection<float>());
}

TEST(TestCone3, NearestIntersectionDouble)
{
	EXPECT_TRUE(TestCone3::testNearestIntersection<double>());
}

TEST(TestCone3, ValidateNearestIntersectionFloat)
{
	EXPECT_TRUE(TestCone3::validateNearestIntersection<float>(GTEST_TEST_DURATION));
}

TEST(TestCone3, ValidateNearestIntersectionDouble)
{
	EXPECT_TRUE(TestCone3::validateNearestIntersection<double>(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestCone3::testConstructor()
{
	Log::info() << "Constructor test: ";

	bool allSucceeded = true;

	{
		Cone3 cone;
		if (cone.isValid())
		{
			Log::info() << "Cone3 default constructor failed";
			allSucceeded = false;
		}
	}

	{
		Cone3 cone(Vector3(1.0, 1.0, 1.0), Vector3(0.0, 0.0, 1.0), Numeric::pi_2(), -1.0, 10.0);

		if (!cone.isValid() || cone.apex() != Vector3(1.0, 1.0, 1.0) || cone.axis() != Vector3(0.0, 0.0, 1.0) || cone.apexAngle() != Numeric::pi_2() || cone.minSignedDistanceAlongAxis() != -1.0 || cone.maxSignedDistanceAlongAxis() != 10.0)
		{
			Log::info() << "Cone3 valid constructor failed";
			allSucceeded = false;
		}
	}

	return allSucceeded;
}

template <typename T>
bool TestCone3::testNearestIntersection()
{
	Log::info() << "Nearest intersection (" << (std::is_same<T, float>::value ? "float" : "double") << "): ";
	Log::info() << " ";

	// Create a few simple dummy scenarios.
	const std::vector<ConeT3<T>> cones =
	{
		ConeT3<T>(VectorT3<T>(0, 0, 0), VectorT3<T>(0, 0, 1), NumericT<T>::pi_2(), -std::numeric_limits<T>::infinity()),
		ConeT3<T>(VectorT3<T>(0, 0, 0), VectorT3<T>(0, 0, 1), NumericT<T>::pi_2(), 0),
		ConeT3<T>(VectorT3<T>(0, 0, 0), VectorT3<T>(0, 0, 1), NumericT<T>::pi_2(), -1, 1),
		ConeT3<T>(VectorT3<T>(0, 0, 0), VectorT3<T>(0, 0, 1), NumericT<T>::pi_2(), 2, 10)
	};

	// Note that we don't require the ray direction to be normalized.
	const std::vector<LineT3<T>> rays =
	{
		LineT3<T>(VectorT3<T>(-1, 0, 0), VectorT3<T>(1, 0, 1)),
		LineT3<T>(VectorT3<T>(-1, 0, 0), VectorT3<T>(-1, 0, -1)),
		LineT3<T>(VectorT3<T>(-2, 0, 1), VectorT3<T>(1, 0, 0)),
		LineT3<T>(VectorT3<T>(0, 0, 1), VectorT3<T>(1, 0, 0)),
		LineT3<T>(VectorT3<T>(0, 0, 0), VectorT3<T>(1, 0, 0)),
		LineT3<T>(VectorT3<T>(-2, 0, 0), VectorT3<T>(3, 0, 1)),
		LineT3<T>(VectorT3<T>(0, -1, 0), VectorT3<T>(1, 0, 0))
	};

	// The intersection points are always the same for each ray; only truncation changes the result.
	const std::vector<VectorT3<T>> groundTruthIntersectionPoints =
	{
		VectorT3<T>(T(-0.5), 0, T(0.5)),
		VectorT3<T>(), // never intersects
		VectorT3<T>(-1, 0, 1),
		VectorT3<T>(), // never intersects
		VectorT3<T>(0, 0, 0),
		VectorT3<T>(T(-0.5), 0, T(0.5)),
		VectorT3<T>(), // never intersects
	};

	// For each cone, for each ray.
	const std::vector<std::vector<bool>> groundTruthIntersections =
	{
		{true, false, true, false, true, true, false},
		{true, false, true, false, true, true, false},
		{true, false, true, false, true, true, false},
		{false, false, false, false, false, false, false}
	};

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	for (unsigned int coneIndex = 0u; coneIndex < cones.size(); ++coneIndex)
	{
		const ConeT3<T>& cone = cones[coneIndex];

		for (unsigned int rayIndex = 0u; rayIndex < rays.size(); ++rayIndex)
		{
			VectorT3<T> point;
			const bool intersects = cone.nearestIntersection(rays[rayIndex], point);

			const VectorT3<T>& groundTruthPoint = groundTruthIntersectionPoints[rayIndex];
			const bool& groundTruthIntersection = groundTruthIntersections[coneIndex][rayIndex];

			if (intersects ^ groundTruthIntersection || (groundTruthIntersection && groundTruthPoint != point))
			{
				allSucceeded = false;

				Log::info() << "Incorrect intersection!";
				Log::info() << "Cone index = " << coneIndex << ", Ray index = " << rayIndex;
				Log::info() << "Estimated intersect = " << (intersects ? "true" : "false") << " for GT = " << (groundTruthIntersection ? "true" : "false");
				Log::info() << "Estimated point = (" << point.x() << ", " << point.y() << ", " << point.z() << ")";
				Log::info() << "GT point = (" << groundTruthPoint.x() << ", " << groundTruthPoint.y() << ", " << groundTruthPoint.z() << ")";
				Log::info() << " ";
			}
		}
	}

	Log::info() << "Test: " << (allSucceeded ? "Success" : "FAILED!");

	return allSucceeded;
}

template <typename T>
bool TestCone3::validateNearestIntersection(const double testDuration)
{
	Log::info() << "Validating nearest intersection (" << (std::is_same<T, float>::value ? "float" : "double") << "): ";
	Log::info() << " ";

	ocean_assert(testDuration > 0.0);

	constexpr unsigned int kRandomSeed = 3u;
	RandomGenerator randomGenerator(kRandomSeed);

	unsigned int numberSuccessfulTrials = 0u;
	unsigned int totalNumberTrials = 0u;

	const Timestamp startTimestamp(true);

	do
	{
		// Generate a random cone aligned with the +Z-axis and with its apex at the origin.
		// Then, rotate, translate, and scale it.
		const bool minDistanceIsAtInfinity = (RandomI::random(randomGenerator, 0u, 1u) == 0u);
		const bool maxDistanceIsAtInfinity = (RandomI::random(randomGenerator, 0u, 1u) == 0u);

		const T apexAngle = RandomT<T>::scalar(randomGenerator, NumericT<T>::weakEps(), NumericT<T>::pi() - NumericT<T>::weakEps());
		const T minSignedDistanceAlongAxis = minDistanceIsAtInfinity ? -std::numeric_limits<T>::infinity() : RandomT<T>::scalar(randomGenerator, T(-10.0), T(10.0) - NumericT<T>::weakEps());
		const T maxSignedDistanceAlongAxis = maxDistanceIsAtInfinity ? std::numeric_limits<T>::infinity() : RandomT<T>::scalar(randomGenerator, minDistanceIsAtInfinity ? T(-10.0) : minSignedDistanceAlongAxis + NumericT<T>::weakEps(), T(10.0) + T(2.0) * NumericT<T>::weakEps());

		const RotationT<T> rotation(RandomT<T>::rotation(randomGenerator));
		const VectorT3<T> translation(RandomT<T>::vector3(randomGenerator, -100.0, 100.0));
		const T scale = RandomT<T>::scalar(randomGenerator, T(0.001), T(100.0));

		const ConeT3<T> cone(translation, rotation * VectorT3<T>(0.0, 0.0, 1.0), apexAngle, scale * minSignedDistanceAlongAxis, scale * maxSignedDistanceAlongAxis);
		ocean_assert(cone.isValid());

		const HomogenousMatrixT4<T> cone_T_canonicalCone(translation, rotation, VectorT3<T>(scale, scale, scale));

		// Test by generating random cone/ray pairings. Verify by first calculating the intersection
		// in a canonical space and then applying a random similarity transform.
		constexpr unsigned int kNumRandomRays = 10u; // number of rays to test per cone

		for (unsigned int i = 0u; i < kNumRandomRays; ++i)
		{
			// Define a ray in the pre-transformed space.
			VectorT3<T> rayOrigin = RandomT<T>::vector3(randomGenerator, T(-5.0), T(5.0));
			rayOrigin.z() *= T(2.0); // allow more variation in z for the ray origin
			const VectorT3<T> rayDirection = RandomT<T>::vector3(randomGenerator);

			// In the pre-transformed space, each horizontal cross-section of cone projects onto the XY
			// plane as a circle centered at the origin. The circle radius is determined by value of Z for
			// the given cross-section. The radius is defined as
			//   r(z) = Z * tan(apexAngle / 2) = Z * sqrt(h)
			const T tanHalfApexAngle = NumericT<T>::tan(T(0.5) * apexAngle);
			const T coneRadiusAtRayOriginZValue = tanHalfApexAngle * NumericT<T>::abs(rayOrigin.z());
			const bool rayOriginIsOnCone = NumericT<T>::isEqual(VectorT2<T>(rayOrigin.x(), rayOrigin.y()).length(), coneRadiusAtRayOriginZValue);

			bool groundTruthIntersection = false;
			VectorT3<T> groundTruthPoint;

			if (rayOriginIsOnCone)
			{
				groundTruthIntersection = true;
				groundTruthPoint = cone_T_canonicalCone * rayOrigin;
			}
			else
			{
				// Let `p` be the ray origin and `v` be the ray direction. A point on the ray can be expressed
				// as X(t) = p + t * v. The ray-cone intersection is found when
				//   X(t).x^2 + X(t).y^2 = r(X(t).z)^2
				//   => (p.x + t * v.x)^2 + (p.y + t * v.y)^2 = (p.z + t * v.z)^2 * h
				//   => (v.x^2 + v.y^2 - v.z^2 * h) * t^2
				//       + 2 * (v.x * p.x + v.y * p.y - v.z * p.z * h) * t
				//       + (p.x^2 + p.y^2 - p.z^2 * h)
				//       = 0.

				const auto& p = rayOrigin;
				const auto& v = rayDirection;
				const T h = NumericT<T>::sqr(tanHalfApexAngle);
				T a = NumericT<T>::sqr(v.x()) + NumericT<T>::sqr(v.y()) - NumericT<T>::sqr(v.z()) * h;
				T b = T(2.0) * (v.x() * p.x() + v.y() * p.y() - v.z() * p.z() * h);
				T c = NumericT<T>::sqr(p.x()) + NumericT<T>::sqr(p.y()) - NumericT<T>::sqr(p.z()) * h;

				// Normalize the quadratic before solving.
				const T magnitude = std::max(std::max(NumericT<T>::abs(a), NumericT<T>::abs(b)), NumericT<T>::abs(c));
				if (NumericT<T>::isNotEqualEps(magnitude))
				{
					a /= magnitude;
					b /= magnitude;
					c /= magnitude;
				}

				T minDistance(-1.0);
				T maxDistance(-1.0);

				if (NumericT<T>::isNotEqualEps(a))
				{
					EquationT<T>::solveQuadratic(a, b, c, minDistance, maxDistance);
				}
				else
				{
					ocean_assert(NumericT<T>::isNotEqualEps(b));
					minDistance = -c / b;
					maxDistance = minDistance;
				}

				if (minDistance > maxDistance)
				{
					std::swap(minDistance, maxDistance);
				}

				if (minDistance > T(0.0))
				{
					groundTruthPoint = rayOrigin + rayDirection * minDistance;
					groundTruthIntersection = (groundTruthPoint.z() >= minSignedDistanceAlongAxis) && (groundTruthPoint.z() <= maxSignedDistanceAlongAxis);

					// Check for exit/entry behavior, in which case we'll want to grab the maximum distance
					// point.
					const VectorT3<T> groundTruthMaxDistancePoint = rayOrigin + rayDirection * maxDistance;
					if (NumericT<T>::sign(groundTruthPoint.z()) != NumericT<T>::sign(groundTruthMaxDistancePoint.z()))
					{
						groundTruthPoint = groundTruthMaxDistancePoint;
						groundTruthIntersection = !groundTruthIntersection && (groundTruthPoint.z() >= minSignedDistanceAlongAxis) && (groundTruthPoint.z() <= maxSignedDistanceAlongAxis);
					}

					groundTruthPoint = cone_T_canonicalCone * groundTruthPoint;
				}
			}

			const LineT3<T> rayInTransformedSpace(cone_T_canonicalCone * rayOrigin, rotation * rayDirection);
			VectorT3<T> point;
			const bool resultAgreesWithGroundTruth = !(groundTruthIntersection ^ cone.nearestIntersection(rayInTransformedSpace, point));

			if (resultAgreesWithGroundTruth)
			{
				bool resultIsCorrect = true;

				if (groundTruthIntersection)
				{
					// Check that the points are equal up to the precision of whichever value is largest in each component.
					for (unsigned int j = 0u; j <= 2u; ++j)
					{
						T maxValue = std::max(groundTruthPoint[j], point[j]);
						if (NumericT<T>::isEqualEps(maxValue))
						{
							maxValue = T(1.0);
						}

						resultIsCorrect = NumericT<T>::isWeakEqual(groundTruthPoint[j] / maxValue, point[j] / maxValue) && resultIsCorrect;
					}
				}

				if (resultIsCorrect)
				{
					++numberSuccessfulTrials;
				}
			}

			++totalNumberTrials;
		}
	} while (startTimestamp + testDuration > Timestamp(true));

	// Inconsistent results can occasionally pop up when (1) a is nearly 0 in the quadratic
	// equation solver or (2) the ray is nearly parallel with the cone surface. Both of these cases
	// are very rare, especially when using double precision.
	const T ratioOfCorrectTrials = T(numberSuccessfulTrials) / T(totalNumberTrials);

	constexpr T kRatioThreshold = std::is_same<T, float>::value ? T(0.95) : T(0.9999);

	Log::info() << "Validation: " << String::toAString(ratioOfCorrectTrials * 100.0, 1u) << "% succeeded.";

	const bool success = ratioOfCorrectTrials > kRatioThreshold;
	Log::info() << "Validation: " << (success ? "Success" : "FAILED!");

	return success;
}

} // namespace TestMath

} // namespace Test

} // namespace Ocean
