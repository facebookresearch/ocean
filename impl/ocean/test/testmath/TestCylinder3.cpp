/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestCylinder3.h"

#include "ocean/base/Timestamp.h"

#include "ocean/math/Cylinder3.h"
#include "ocean/math/Random.h"
#include "ocean/math/SquareMatrix3.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestCylinder3::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Cylinder3 test:   ---";
	Log::info() << " ";

	bool allSucceeded = testConstructor();

	Log::info() << " ";

	allSucceeded = testNearestIntersection<float>(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testNearestIntersection<double>(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Cylinder3 test succeeded.";
	}
	else
	{
		Log::info() << "Cylinder3 test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestCylinder3, Constructor)
{
	EXPECT_TRUE(TestCylinder3::testConstructor());
}

TEST(TestCylinder3, NearestIntersectionFloat)
{
	EXPECT_TRUE(TestCylinder3::testNearestIntersection<float>(GTEST_TEST_DURATION));
}

TEST(TestCylinder3, NearestIntersectionDouble)
{
	EXPECT_TRUE(TestCylinder3::testNearestIntersection<double>(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestCylinder3::testConstructor()
{
	bool allSucceeded = true;
	{
		Cylinder3 cylinder;
		if (cylinder.isValid())
		{
			Log::info() << "Cylinder3 default constructor failed";
			allSucceeded = false;
		}
	}
	{
		// Height constructor.
		Cylinder3 cylinder(Vector3(1., 1., 1.), Vector3(0., 0., 1.), 1., 10.);
		if (!cylinder.isValid() || cylinder.origin() != Vector3(1., 1., 1.) || cylinder.axis() != Vector3(0., 0., 1.) || cylinder.radius() != 1. || cylinder.minSignedDistanceAlongAxis() != 0. || cylinder.maxSignedDistanceAlongAxis() != 10. || cylinder.height() != 10.)
		{
			Log::info() << "Cylinder3 valid constructor failed";
			allSucceeded = false;
		}
	}
	{
		// Min/max distance constructor.
		Cylinder3 cylinder(Vector3(1., 1., 1.), Vector3(0., 0., 1.), 1., -10., 10.);
		if (!cylinder.isValid() || cylinder.origin() != Vector3(1., 1., 1.) || cylinder.axis() != Vector3(0., 0., 1.) || cylinder.radius() != 1. || cylinder.minSignedDistanceAlongAxis() != -10. || cylinder.maxSignedDistanceAlongAxis() != 10. || cylinder.height() != 20.)
		{
			Log::info() << "Cylinder3 valid constructor failed";
			allSucceeded = false;
		}
	}

	return allSucceeded;
}

template <typename T>
bool TestCylinder3::testNearestIntersection(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int kRandomSeed = 3u;
	RandomGenerator randomGenerator(kRandomSeed);

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	unsigned int testId = 0u;

	// There are a few cases where numerical issues give us a disagreement between the ground-truth
	// and estimated results. Because of these, we choose an epsilon between the "weak" and "strong"
	// values for epsilon, since the strong epsilon is likely too small for the check.
	unsigned int numGroundTruthFalseButPointsEqual = 0u;
	unsigned int numGroundTruthTrueButPointNearSurface = 0u;
	unsigned int numGroundTruthTrueButDiscriminantNearZero = 0u;
	const T semiWeakEps = NumericT<T>::pow(NumericT<T>::weakEps(), T(1.5));

	do
	{
		constexpr unsigned int kNumIter = 1000u; // number of random cylinders to test
		constexpr unsigned int kNumRandomRays = 10u; // number of rays to test per cylinder

		// Test by generating random cylinder/ray pairings. Verify by first calculating the intersection
		// in a canonical space (where the cylinder projects to a unit circle in x/y plane) and then
		// applying a random similarity transform.
		for (unsigned int n = 0u; n < kNumIter; ++n)
		{
			// Mapping from the unit cylinder (with random, non-unit height) to arbitrary cylinders.
			const RotationT<T> rotation(RandomT<T>::rotation(randomGenerator));
			const VectorT3<T> translation(RandomT<T>::vector3(randomGenerator, -100., 100.));
			const T scale = RandomT<T>::scalar(randomGenerator, T(0.001), T(100.0));
			const T minSignedDistanceAlongAxis = RandomT<T>::scalar(randomGenerator, -10., 10.); // before scaling
			const T maxSignedDistanceAlongAxis = minSignedDistanceAlongAxis + RandomT<T>::scalar(randomGenerator, 1., 10.); // before scaling

			const auto transform = [&](VectorT3<T>& v) {
				v = (rotation * v) * scale + translation;
			};

			// Define the cylinder after the similarity transformation. We'll check the "ground-truth"
			// method against the result of cylinder.nearestIntersection().
			const VectorT3<T>& origin = translation;
			const VectorT3<T> axis = rotation * VectorT3<T>(0., 0., 1.);
			const CylinderT3<T> cylinder(origin, axis, scale, scale * minSignedDistanceAlongAxis, scale * maxSignedDistanceAlongAxis);

			for (unsigned int i = 0u; i < kNumRandomRays; ++i)
			{
				// Define a ray in the pre-transformed space.
				VectorT3<T> rayOrigin = RandomT<T>::vector3(randomGenerator, T(-5.), T(5.));
				rayOrigin.z() *= T(2.); // allow more variation in z for the ray origin
				VectorT3<T> rayDirection = RandomT<T>::vector3(randomGenerator);

				// Compute the intersection in the pre-transformed space and check whether there might be
				// any numerical issues with the computation.
				bool gtIntersection = false;
				bool discriminantIsNearlyZero = false;
				VectorT3<T> gtPoint = rayOrigin;
				T gtMinDistance3D = T(0.); // only used for debugging

				VectorT2<T> projectedOrigin(rayOrigin.x(), rayOrigin.y());
				const T sqrDistanceOriginToCircle = projectedOrigin.sqr();

				// (Only ray origins outside (or on) the unit circle are valid.)
				if (sqrDistanceOriginToCircle > T(1.) && !NumericT<T>::isEqual(sqrDistanceOriginToCircle, T(1.)))
				{
					// Project the ray onto the xy plane and calculate the intersection point with the unit circle.
					VectorT2<T> projectedDirection(rayDirection.x(), rayDirection.y());
					if (!projectedDirection.isNull())
					{
						// Let the 2D ray be defined by origin p and direction v.
						// Solve for closest distance along the ray t, which gives 2D point X on the unit circle:
						//   X = t * v + p, || X || = 1
						//   => v.v * t^2 + [ 2 * v.p ] * t + [ p.p - 1 ] = 0.
						T minDistance = T(-1.), maxDistance = T(-1.);

						const T a = projectedDirection.sqr();
						const T b = T(2.) * (projectedDirection * projectedOrigin);
						const T c = projectedOrigin.sqr() - T(1.);
						if (EquationT<T>::solveQuadratic(a, b, c, minDistance, maxDistance))
						{
							minDistance = min(minDistance, maxDistance);
						}

						discriminantIsNearlyZero = NumericT<T>::isEqual(b * b - T(4.) * a * c, T(0.), semiWeakEps);

						// Having computed the distance along the 2D-projected ray, now compute the distance
						// along the 3D ray. The (x, y) coordinates are the same for the intersection points of
						// both the 3D ray and its projection: t_3D * v_3D[x] = t_2D * v_2D[x].
						if (minDistance >= T(0.))
						{
							gtMinDistance3D = minDistance * (NumericT<T>::isWeakEqualEps(projectedDirection.x()) ? projectedDirection.y() / rayDirection.y() : projectedDirection.x() / rayDirection.x());
							gtPoint = rayOrigin + rayDirection * gtMinDistance3D;
							// Intersection holds if the point lies within the vertical range of the cylinder.
							gtIntersection = gtPoint.z() >= minSignedDistanceAlongAxis && gtPoint.z() <= maxSignedDistanceAlongAxis;
						}

						// The discriminant may be very close to zero but slightly negative (i.e., the ray is
						// very nearly tangent), in which case we might get a valid intersection after the
						// transformation due to numerical issues. In this case, we'll still compute the point
						// as if the discriminant were zero.
						else if (!NumericT<T>::isEqualEps(a))
						{
							gtMinDistance3D = T(-0.5) * b / a;
							gtPoint = rayOrigin + rayDirection * gtMinDistance3D;
						}
					}
				}
				// If the point is on the unit circle, then it just needs to be within the z-range of the
				// cylinder to be a valid intersection.
				else if (NumericT<T>::isEqual(sqrDistanceOriginToCircle, T(1.)))
				{
					gtIntersection = gtPoint.z() >= minSignedDistanceAlongAxis && gtPoint.z() <= maxSignedDistanceAlongAxis;
				}

				// Apply the random similarity transformation to the ground truth.
				transform(rayOrigin);
				rayDirection = rotation * rayDirection;
				transform(gtPoint);
				gtMinDistance3D *= scale;

				// Compute the intersection using the Cylinder3 class, then check that both
				// intersection-estimation methods agree.
				VectorT3<T> point(T(0.), T(0.), T(0.));
				const bool intersects = cylinder.nearestIntersection(LineT3<T>(rayOrigin, rayDirection), point);

				bool success = !gtIntersection;
				if (intersects)
				{
					// Allow up to a 0.5% error compared to the largest component of the ground-truth
					// coordinate, to account for small numerical differences in the estimation.
					const T errorThreshold = T(0.005) * std::max(
							NumericT<T>::abs(gtPoint.x()),
							std::max(NumericT<T>::abs(gtPoint.y()), NumericT<T>::abs(gtPoint.z())));
					success = point.isEqual(gtPoint, errorThreshold);
					if (success && !gtIntersection)
					{
						++numGroundTruthFalseButPointsEqual; // ray was nearly tangent
					}
				}
				else
				{
					// If the origin is nearly on the cylinder or the ray is nearly tangent, there's a small
					// chance that numerical issues cause the GT to be true, but the intersection is not found
					// after the transformation. So, we'll ignore this case.
					if (NumericT<T>::isEqual(sqrDistanceOriginToCircle, T(1.), semiWeakEps))
					{
						success = true;
						++numGroundTruthTrueButPointNearSurface;
					}
					if (discriminantIsNearlyZero)
					{
						success = true;
						++numGroundTruthTrueButDiscriminantNearZero; // ray was nearly tangent
					}
				}

				if (!success)
				{
					allSucceeded = false;

					Log::info() << "Incorrect intersection!";
					Log::info() << "Test instance = " << testId;
					Log::info() << "Estimated intersect = " << (intersects ? "true" : "false") << " for GT = " << (gtIntersection ? "true" : "false");
					Log::info() << "Estimated point = " << point;
					Log::info() << "GT point = " << gtPoint;
					Log::info() << "Point distance = " << (point - rayOrigin).length();
					Log::info() << "GT distance = " << gtMinDistance3D;
					Log::info() << "Cylinder Origin = " << origin;
					Log::info() << "Cylinder Axis = " << axis;
					Log::info() << "Cylinder Radius = " << cylinder.radius();
					Log::info() << "Cylinder Z Range = (" << cylinder.minSignedDistanceAlongAxis() << ", " << cylinder.maxSignedDistanceAlongAxis() << ")";
					Log::info() << "Ray Origin = " << rayOrigin;
					Log::info() << "Ray Direction = " << rayDirection;
					Log::info() << " ";
				}

				++testId;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	// Sanity-check the times we allowed a disagreement.
	if (testId > 0u)
	{
		Log::info() << "In rare cases, the ground-truth (GT) intersection result will disagree with the estimated intersection result due to issues with numerical precision.";
		Log::info() << "The following percentages should all be much less than 0.01%:";
		// For this first one, the GT point was calculated as if the discriminant was zero (see above).
		Log::info() << "Num GT False but Points Equal: " << numGroundTruthFalseButPointsEqual << " / " << testId << " (" << (T(numGroundTruthFalseButPointsEqual) / T(testId) * T(100)) << "%)";
		Log::info() << "Num GT True but Point Near Surface: " << numGroundTruthTrueButPointNearSurface << " / " << testId << " (" << (T(numGroundTruthTrueButPointNearSurface) / T(testId) * T(100)) << "%)";
		Log::info() << "Num GT True but Discriminant Near Zero: " << numGroundTruthTrueButDiscriminantNearZero << " / " << testId << " (" << (T(numGroundTruthTrueButDiscriminantNearZero) / T(testId) * T(100)) << "%)";
	}

	return allSucceeded;
}

} // namespace TestMath

} // namespace Test

} // namespace Ocean
