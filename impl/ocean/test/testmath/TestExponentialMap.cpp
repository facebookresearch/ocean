/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestExponentialMap.h"

#include "ocean/base/Timestamp.h"

#include "ocean/math/ExponentialMap.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/Random.h"
#include "ocean/math/Rotation.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/ValidationPrecision.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestExponentialMap::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("ExponentialMap test");

	Log::info() << " ";

	if (selector.shouldRun("constructors"))
	{
		testResult = testConstructors(testDuration);

		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestExponentialMap, Constructors)
{
	EXPECT_TRUE(TestExponentialMap::testConstructors(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestExponentialMap::testConstructors(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing the constructors:";

	RandomGenerator randomGenerator;
	ValidationPrecision validation(0.95, randomGenerator);

	Timestamp startTimestamp(true);

	do
	{
		{
			// angle-axis rotation

			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const Rotation rotation = Random::rotation(randomGenerator);

			const ExponentialMap exponentialMap(rotation);

			const Rotation resultRotation = exponentialMap.rotation();

			if (resultRotation != rotation)
			{
				scopedIteration.setInaccurate();
			}
		}

		{
			// axis and angle

			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const Vector3 axis = Random::vector3(randomGenerator);
			ocean_assert(axis.isUnit());

			const Scalar angle = Random::scalar(randomGenerator, 0, Numeric::pi2());

			const ExponentialMap exponentialMap(axis, angle);

			const Rotation resultRotation = exponentialMap.rotation();

			if (resultRotation != Rotation(axis, angle))
			{
				scopedIteration.setInaccurate();
			}
		}

		{
			// non-normalized axis

			const Vector3 rotationValue = Random::vector3(randomGenerator, -10, 10);

			const ExponentialMap exponentialMap(rotationValue.x(), rotationValue.y(), rotationValue.z());

			const Scalar angle = rotationValue.length();

			Vector3 axis(rotationValue);
			if (Numeric::isNotEqualEps(angle))
			{
				axis /= angle;
			}

			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				const Rotation resultRotation = exponentialMap.rotation();

				if (!((Numeric::isEqualEps(angle) && resultRotation == Rotation()) || (!Numeric::isEqualEps(angle) && resultRotation == Rotation(axis, angle))))
				{
					scopedIteration.setInaccurate();
				}
			}

			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				const Quaternion resultQuaternion = exponentialMap.quaternion();

				if (!((Numeric::isEqualEps(angle) && resultQuaternion == Quaternion(true)) || (!Numeric::isEqualEps(angle) && resultQuaternion == Quaternion(axis, angle))))
				{
					scopedIteration.setInaccurate();
				}
			}
		}

		{
			// quaternion

			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const Quaternion quaternion = Random::quaternion(randomGenerator);

			const ExponentialMap exponentialMap(quaternion);

			const Quaternion resultQuaternion = exponentialMap.quaternion();

			if (resultQuaternion != quaternion)
			{
				scopedIteration.setInaccurate();
			}
		}

		{
			// 3x3 rotation matrix

			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const Quaternion quaternion = Random::quaternion(randomGenerator);

			const ExponentialMap exponentialMap = ExponentialMap(SquareMatrix3(quaternion));

			const Quaternion resultQuaternion = exponentialMap.quaternion();

			if (resultQuaternion != quaternion)
			{
				scopedIteration.setInaccurate();
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
