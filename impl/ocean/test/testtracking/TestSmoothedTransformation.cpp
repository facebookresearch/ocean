/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testtracking/TestSmoothedTransformation.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/Validation.h"

#include "ocean/math/Random.h"

#include "ocean/tracking/SmoothedTransformation.h"

namespace Ocean
{

namespace Test
{

namespace TestTracking
{

bool TestSmoothedTransformation::test(const double testDuration, Worker& /*worker*/, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("SmoothedTransformation test");
	Log::info() << " ";

	if (selector.shouldRun("transformation"))
	{
		testResult = testTransformation(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestSmoothedTransformation, SmoothedTransformation)
{
	EXPECT_TRUE(TestSmoothedTransformation::testTransformation(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestSmoothedTransformation::testTransformation(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Transformation test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const double timeInterval = RandomD::scalar(randomGenerator, 0.1, 5.0);

		Tracking::SmoothedTransformation smoothedTransformation(timeInterval);

		const Timestamp timestampA(RandomD::scalar(randomGenerator, -10, 10));
		const Vector3 translationA(Random::vector3(randomGenerator));
		const HomogenousMatrix4 transformationA(translationA);

		smoothedTransformation.setTransformation(transformationA, timestampA);

		// we have only one transformation, we expect the same transformation for any timestamp
		OCEAN_EXPECT_EQUAL(validation, smoothedTransformation.transformation(Timestamp(RandomD::scalar(randomGenerator, -10, 10))), transformationA);

		const Timestamp timestampB(timestampA + RandomD::scalar(randomGenerator, 0.01, 10));
		const Vector3 translationB(Random::vector3(randomGenerator));
		const HomogenousMatrix4 transformationB(translationB);

		smoothedTransformation.setTransformation(transformationB, timestampB);

		// before A is always A
		OCEAN_EXPECT_EQUAL(validation, smoothedTransformation.transformation(timestampA), transformationA);

		// before B is always A
		OCEAN_EXPECT_EQUAL(validation, smoothedTransformation.transformation(Timestamp(timestampB - NumericD::eps())), transformationA);

		// after B + timeinterval is always B
		OCEAN_EXPECT_EQUAL(validation, smoothedTransformation.transformation(Timestamp(timestampB + timeInterval + NumericD::eps())), transformationB);

		// between B and B + timeinterval is always interpolated (linear)

		const double intervalAB = RandomD::scalar(randomGenerator, 0.0, timeInterval);
		Scalar factorB = Scalar(intervalAB / timeInterval);
		const Scalar factorA = Scalar(1) - factorB;

		ocean_assert(factorA >= 0 && factorB <= 1);
		ocean_assert(Numeric::isEqual(factorA + factorB, 1));

		const HomogenousMatrix4 expectedTransformationAB(translationA * factorA + translationB * factorB);

		OCEAN_EXPECT_TRUE(validation, smoothedTransformation.transformation(Timestamp(timestampB + intervalAB)).translation().isEqual(expectedTransformationAB.translation(), Numeric::weakEps()));

		const Timestamp timestampC(timestampB + RandomD::scalar(randomGenerator, 0.01, 10));
		const Vector3 translationC(Random::vector3(randomGenerator));
		const HomogenousMatrix4 transformationC(translationC);

		const HomogenousMatrix4 smoothedTransformationB = smoothedTransformation.transformation(timestampC);

		smoothedTransformation.setTransformation(transformationC, timestampC);

		// before A is now always the new/smoothed B
		OCEAN_EXPECT_EQUAL(validation, smoothedTransformation.transformation(timestampA), smoothedTransformationB);

		// before B is now always B
		OCEAN_EXPECT_EQUAL(validation, smoothedTransformation.transformation(Timestamp(timestampB - NumericD::eps())), smoothedTransformationB);

		// after C + timeinterval is always C
		OCEAN_EXPECT_EQUAL(validation, smoothedTransformation.transformation(Timestamp(timestampC + timeInterval + NumericD::eps())), transformationC);

		// between C and C + timeinterval is always interpolated (linear)

		const double intervalBC = RandomD::scalar(randomGenerator, 0.0, timeInterval);
		const Scalar factorC = Scalar(intervalBC / timeInterval);
		factorB = Scalar(1) - factorC;

		ocean_assert(factorB >= 0 && factorB <= 1);
		ocean_assert(Numeric::isEqual(factorB + factorC, 1));

		const HomogenousMatrix4 expectedTransformationBC(smoothedTransformationB.translation() * factorB + translationC * factorC);

		OCEAN_EXPECT_TRUE(validation, smoothedTransformation.transformation(Timestamp(timestampC + intervalBC)).translation().isEqual(expectedTransformationBC.translation(), Numeric::weakEps()));
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
