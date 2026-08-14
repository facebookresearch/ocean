/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestSphericalExponentialMap.h"

#include "ocean/base/Timestamp.h"

#include "ocean/math/Random.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/Validation.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestSphericalExponentialMap::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("SphericalExponentialMap test");

	Log::info() << " ";

	if (selector.shouldRun("constructorfromvectors"))
	{
		testResult = testConstructorFromVectors(testDuration);

		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestSphericalExponentialMap, ConstructorFromVectors)
{
	EXPECT_TRUE(TestSphericalExponentialMap::testConstructorFromVectors(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestSphericalExponentialMap::testConstructorFromVectors(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing the constructor with a reference and an offset vector:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	{
		// the reference vector is parallel to the y-axis, so that no axis perpendicular to it is defined by the cross product with the y-axis

		const Vector3 reference(0, 1, 0);
		const Vector3 offset(0, -1, 0);

		OCEAN_EXPECT_TRUE(validation, verifyRotation(SphericalExponentialMap(reference, offset), reference, offset));
		OCEAN_EXPECT_TRUE(validation, verifyRotation(SphericalExponentialMap(offset, reference), offset, reference));
	}

	{
		// the vectors differ only along the y-axis, so that their rotation axis is underdetermined by the plane intersection

		const Vector3 reference(Vector3(Scalar(0.6), Scalar(0.8), 0).normalized());
		const Vector3 offset(Vector3(Scalar(0.6), Scalar(-0.8), 0).normalized());

		OCEAN_EXPECT_TRUE(validation, verifyRotation(SphericalExponentialMap(reference, offset), reference, offset));
	}

	{
		// the projected vectors are nearly anti-parallel, for which inverse cosine loses precision in single precision

		const Vector3 reference(Vector3(Scalar(-0.7858555317), Scalar(-0.6534635425), Scalar(-0.2285202742)).normalized());
		const Vector3 offset(Vector3(Scalar(0.6944842339), Scalar(0.6908613443), Scalar(-0.5166417956)).normalized());

		OCEAN_EXPECT_TRUE(validation, verifyRotation(SphericalExponentialMap(reference, offset), reference, offset));
	}

	const Timestamp startTimestamp(true);

	do
	{
		const Vector3 reference(Random::vector3(randomGenerator));

		{
			// an arbitrary rotation between two vectors

			const Vector3 offset(Random::vector3(randomGenerator));

			OCEAN_EXPECT_TRUE(validation, verifyRotation(SphericalExponentialMap(reference, offset), reference, offset));
		}

		{
			// both vectors are identical, so that there is no rotation

			OCEAN_EXPECT_TRUE(validation, verifyRotation(SphericalExponentialMap(reference, reference), reference, reference));
		}

		{
			// both vectors are anti-parallel, so that the rotation is 180 degree

			const Vector3 offset(-reference);

			const SphericalExponentialMap sphericalExponentialMap(reference, offset);

			OCEAN_EXPECT_TRUE(validation, verifyRotation(sphericalExponentialMap, reference, offset));

			// the length of the 2-DOF orientation is the rotation angle
			OCEAN_EXPECT_TRUE(validation, Numeric::isEqual(sphericalExponentialMap.angle(), Numeric::pi(), Numeric::weakEps()));
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestSphericalExponentialMap::verifyRotation(const SphericalExponentialMap& sphericalExponentialMap, const Vector3& reference, const Vector3& offset)
{
	ocean_assert(Numeric::isEqual(reference.length(), 1) && Numeric::isEqual(offset.length(), 1));

	const VectorD3 rotatedReference(sphericalExponentialMap.rotation() * reference);

	return NumericD::rad2deg(rotatedReference.angle(VectorD3(offset))) <= 0.001;
}

}

}

}
