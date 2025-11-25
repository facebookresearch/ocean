/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestCamera.h"

#include "ocean/base/DataType.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/Camera.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Random.h"
#include "ocean/math/SquareMatrix3.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/ValidationPrecision.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestCamera::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("Camera test");

	Log::info() << " ";

	if (selector.shouldRun("objectpointinfront"))
	{
		testResult = testObjectPointInFront<float>(testDuration);
		Log::info() << " ";
		testResult = testObjectPointInFront<double>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("standard2invertedflippedhomogenousmatrix4"))
	{
		testResult = testStandard2InvertedFlippedHomogenousMatrix4<float>(testDuration);
		Log::info() << " ";
		testResult = testStandard2InvertedFlippedHomogenousMatrix4<double>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("standard2invertedflippedsquarematrix3"))
	{
		testResult = testStandard2InvertedFlippedSquareMatrix3<float>(testDuration);
		Log::info() << " ";
		testResult = testStandard2InvertedFlippedSquareMatrix3<double>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("standard2invertedflippedquaternion"))
	{
		testResult = testStandard2InvertedFlippedQuaternion<float>(testDuration);
		Log::info() << " ";
		testResult = testStandard2InvertedFlippedQuaternion<double>(testDuration);

		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestCamera, ObjectPointInFront_float)
{
	EXPECT_TRUE(TestCamera::testObjectPointInFront<float>(GTEST_TEST_DURATION));
}

TEST(TestCamera, ObjectPointInFront_double)
{
	EXPECT_TRUE(TestCamera::testObjectPointInFront<double>(GTEST_TEST_DURATION));
}

TEST(TestCamera, Standard2InvertedFlippedHomogenousMatrix4_float)
{
	EXPECT_TRUE(TestCamera::testStandard2InvertedFlippedHomogenousMatrix4<float>(GTEST_TEST_DURATION));
}

TEST(TestCamera, Standard2InvertedFlippedHomogenousMatrix4_double)
{
	EXPECT_TRUE(TestCamera::testStandard2InvertedFlippedHomogenousMatrix4<double>(GTEST_TEST_DURATION));
}

TEST(TestCamera, Standard2InvertedFlippedSquareMatrix3_float)
{
	EXPECT_TRUE(TestCamera::testStandard2InvertedFlippedSquareMatrix3<float>(GTEST_TEST_DURATION));
}

TEST(TestCamera, Standard2InvertedFlippedSquareMatrix3_double)
{
	EXPECT_TRUE(TestCamera::testStandard2InvertedFlippedSquareMatrix3<double>(GTEST_TEST_DURATION));
}

TEST(TestCamera, Standard2InvertedFlippedQuaternion_float)
{
	EXPECT_TRUE(TestCamera::testStandard2InvertedFlippedQuaternion<float>(GTEST_TEST_DURATION));
}

TEST(TestCamera, Standard2InvertedFlippedQuaternion_double)
{
	EXPECT_TRUE(TestCamera::testStandard2InvertedFlippedQuaternion<double>(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

template <typename T>
bool TestCamera::testObjectPointInFront(const double testDuration)
{
	static_assert(std::is_same<T, float>::value || std::is_same<T, double>::value, "Template parameter T must be float or double.");

	ocean_assert(testDuration > 0.0);

	Log::info() << "Object Point in front test (" << TypeNamer::name<T>() << "):";

	RandomGenerator randomGenerator;
	ValidationPrecision validation(0.999, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				// pose with default orientation

				const SquareMatrixT3<T> world_R_camera(true);
				const HomogenousMatrixT4<T> world_T_camera(VectorT3<T>(RandomT<T>::scalar(randomGenerator, -10, 10), RandomT<T>::scalar(randomGenerator, -10, -10), 0));

				const SquareMatrixT3<T> flippedCamera_R_world(CameraT<T>::standard2InvertedFlipped(world_R_camera));
				const HomogenousMatrixT4<T> flippedCamera_T_world(CameraT<T>::standard2InvertedFlipped(world_T_camera));

				const VectorT3<T> frontObjectPoint(RandomT<T>::scalar(randomGenerator, -100, 100), RandomT<T>::scalar(randomGenerator, -100, 100), RandomT<T>::scalar(randomGenerator, -100, -NumericT<T>::eps() * T(100)));
				const VectorT3<T> backObjectPoint(RandomT<T>::scalar(randomGenerator, -100, 100), RandomT<T>::scalar(randomGenerator, -100, 100), RandomT<T>::scalar(randomGenerator, 0, 100));

				if (!CameraT<T>::isObjectPointInFrontIF(flippedCamera_T_world, frontObjectPoint) || !CameraT<T>::isObjectPointInFrontIF(flippedCamera_R_world, frontObjectPoint) || CameraT<T>::isObjectPointInFrontIF(flippedCamera_T_world, backObjectPoint) || CameraT<T>::isObjectPointInFrontIF(flippedCamera_R_world, backObjectPoint))
				{
					scopedIteration.setInaccurate();
				}
			}

			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				// arbitrary pose

				const SquareMatrixT3<T> world_R_camera(RandomT<T>::quaternion(randomGenerator));
				const HomogenousMatrixT4<T> world_T_camera(RandomT<T>::vector3(randomGenerator, -10, 10), world_R_camera);

				const HomogenousMatrixT4<T> flippedCamera_T_world(CameraT<T>::standard2InvertedFlipped(world_T_camera));
				const SquareMatrixT3<T> flippedCamera_R_world(CameraT<T>::standard2InvertedFlipped(world_R_camera));

				const VectorT3<T> localFrontObjectPoint(RandomT<T>::scalar(randomGenerator, -100, 100), RandomT<T>::scalar(randomGenerator, -100, 100), RandomT<T>::scalar(randomGenerator, -100, -NumericT<T>::eps() * T(100)));
				const VectorT3<T> localBackObjectPoint(RandomT<T>::scalar(randomGenerator, -100, 100), RandomT<T>::scalar(randomGenerator, -100, 100), RandomT<T>::scalar(randomGenerator, 0, 100));

				const VectorT3<T> orientationFrontObjectPoint(world_R_camera * localFrontObjectPoint);
				const VectorT3<T> orientationBackObjectPoint(world_R_camera * localBackObjectPoint);

				const VectorT3<T> poseFrontObjectPoint(world_T_camera * localFrontObjectPoint);
				const VectorT3<T> poseBackObjectPoint(world_T_camera * localBackObjectPoint);

				if (!CameraT<T>::isObjectPointInFrontIF(flippedCamera_T_world, poseFrontObjectPoint) || !CameraT<T>::isObjectPointInFrontIF(flippedCamera_R_world, orientationFrontObjectPoint) || CameraT<T>::isObjectPointInFrontIF(flippedCamera_T_world, poseBackObjectPoint) || PinholeCameraT<T>::isObjectPointInFrontIF(flippedCamera_R_world, orientationBackObjectPoint))
				{
					scopedIteration.setInaccurate();
				}
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestCamera::testStandard2InvertedFlippedHomogenousMatrix4(const double testDuration)
{
	static_assert(std::is_same<T, float>::value || std::is_same<T, double>::value, "Template parameter T must be float or double.");

	ocean_assert(testDuration > 0.0);

	Log::info() << "Standard2InvertedFlipped of HomogenousMatrixT4<" << TypeNamer::name<T>() << "> test:";

	RandomGenerator randomGenerator;
	ValidationPrecision validation(0.999, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const VectorT3<T> translation(RandomT<T>::vector3(randomGenerator, -10, 10));
		const VectorT3<T> scale(RandomT<T>::vector3(randomGenerator, T(0.5), T(2.0)));
		const QuaternionT<T> quaternion(RandomT<T>::quaternion(randomGenerator));

		{
			const HomogenousMatrixT4<T> world_T_camera(translation, quaternion, scale);
			ocean_assert(world_T_camera.isValid());

			const HomogenousMatrixT4<T> flippedCamera_T_world = CameraT<T>::standard2InvertedFlipped(world_T_camera);
			ocean_assert(flippedCamera_T_world.isValid());

			const HomogenousMatrixT4<T> world_T_flippedCamera = world_T_camera * HomogenousMatrixT4<T>(RotationT<T>(1, 0, 0, NumericT<T>::pi()));

			HomogenousMatrixT4<T> test_flippedCamera_T_world;
			if (world_T_flippedCamera.invert(test_flippedCamera_T_world))
			{
				for (unsigned int n = 0u; n < 5u; ++n)
				{
					ValidationPrecision::ScopedIteration scopedIteration(validation);

					const VectorT3<T> testVector = RandomT<T>::vector3(randomGenerator, -1, 1);

					if (!(flippedCamera_T_world * testVector).isEqual(test_flippedCamera_T_world * testVector, NumericT<T>::weakEps()))
					{
						scopedIteration.setInaccurate();
					}
				}
			}

			const HomogenousMatrixT4<T> test_world_T_camera = CameraT<T>::invertedFlipped2Standard(flippedCamera_T_world);

			for (unsigned int n = 0u; n < 5u; ++n)
			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				const VectorT3<T> testVector = RandomT<T>::vector3(randomGenerator, -1, 1);

				if (!(world_T_camera * testVector).isEqual(test_world_T_camera * testVector, NumericT<T>::weakEps()))
				{
					scopedIteration.setInaccurate();
				}
			}
		}

		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const HomogenousMatrixT4<T> world_T_camera(translation, quaternion);
			ocean_assert(world_T_camera.isValid());

			const HomogenousMatrixT4<T> flippedCamera_T_world = CameraT<T>::standard2InvertedFlipped(world_T_camera);
			ocean_assert(flippedCamera_T_world.isValid());

			const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
			const unsigned int height = RandomI::random(randomGenerator, 1u, 1080u);
			const T fovX = RandomT<T>::scalar(randomGenerator, NumericT<T>::deg2rad(25), NumericT<T>::deg2rad(120));

			const PinholeCameraT<T> pinholeCamera(width, height, fovX);

			const VectorT2<T> observation = RandomT<T>::vector2(randomGenerator, 0, T(width), 0, T(height));

			const LineT3<T> ray = pinholeCamera.ray(observation, world_T_camera);
			const VectorT3<T> position = ray.point(RandomT<T>::scalar(randomGenerator, T(0.1), T(2)));

			if (!pinholeCamera.template projectToImageIF<false, false>(flippedCamera_T_world, position).isEqual(observation, 2))
			{
				scopedIteration.setInaccurate();
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestCamera::testStandard2InvertedFlippedSquareMatrix3(const double testDuration)
{
	static_assert(std::is_same<T, float>::value || std::is_same<T, double>::value, "Template parameter T must be float or double.");

	ocean_assert(testDuration > 0.0);

	Log::info() << "Standard2InvertedFlipped of SquareMatrixT3<" << TypeNamer::name<T>() << "> test:";

	RandomGenerator randomGenerator;
	ValidationPrecision validation(0.999, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const SquareMatrixT3<T> world_R_camera(RandomT<T>::rotation(randomGenerator));
		ocean_assert(!world_R_camera.isSingular());

		const SquareMatrixT3<T> flippedCamera_R_world = CameraT<T>::standard2InvertedFlipped(world_R_camera);
		ocean_assert(!flippedCamera_R_world.isSingular());

		{
			const SquareMatrixT3<T> world_R_flippedCamera = world_R_camera * SquareMatrixT3<T>(RotationT<T>(1, 0, 0, NumericT<T>::pi()));

			SquareMatrixT3<T> test_flippedCamera_R_world;
			if (world_R_flippedCamera.invert(test_flippedCamera_R_world))
			{
				for (unsigned int n = 0u; n < 5u; ++n)
				{
					ValidationPrecision::ScopedIteration scopedIteration(validation);

					const VectorT3<T> testVector = RandomT<T>::vector3(randomGenerator, -1, 1);

					if (!(flippedCamera_R_world * testVector).isEqual(test_flippedCamera_R_world * testVector, NumericT<T>::weakEps()))
					{
						scopedIteration.setInaccurate();
					}
				}
			}
		}

		{
			const SquareMatrixT3<T> test_world_R_camera = CameraT<T>::invertedFlipped2Standard(flippedCamera_R_world);

			for (unsigned int n = 0u; n < 5u; ++n)
			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				const VectorT3<T> testVector = RandomT<T>::vector3(randomGenerator, -1, 1);

				if (!(world_R_camera * testVector).isEqual(test_world_R_camera * testVector, NumericT<T>::weakEps()))
				{
					scopedIteration.setInaccurate();
				}
			}
		}

		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
			const unsigned int height = RandomI::random(randomGenerator, 1u, 1080u);
			const T fovX = RandomT<T>::scalar(randomGenerator, NumericT<T>::deg2rad(25), NumericT<T>::deg2rad(120));

			const PinholeCameraT<T> pinholeCamera(width, height, fovX);

			const VectorT2<T> observation = RandomT<T>::vector2(randomGenerator, 0, T(width), 0, T(height));

			const LineT3<T> ray = pinholeCamera.ray(observation, HomogenousMatrixT4<T>(world_R_camera));
			const VectorT3<T> position = ray.point(RandomT<T>::scalar(randomGenerator, T(0.1), T(2)));

			if (!pinholeCamera.template projectToImageIF<false, false>(HomogenousMatrixT4<T>(flippedCamera_R_world), position).isEqual(observation, 2))
			{
				scopedIteration.setInaccurate();
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestCamera::testStandard2InvertedFlippedQuaternion(const double testDuration)
{
	static_assert(std::is_same<T, float>::value || std::is_same<T, double>::value, "Template parameter T must be float or double.");

	ocean_assert(testDuration > 0.0);

	Log::info() << "Standard2InvertedFlipped of QuaternionT<" << TypeNamer::name<T>() << "> test:";

	RandomGenerator randomGenerator;
	ValidationPrecision validation(0.999, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const QuaternionT<T> world_Q_camera(RandomT<T>::quaternion(randomGenerator));
		ocean_assert(world_Q_camera.isValid());

		const QuaternionT<T> flippedCamera_Q_world = CameraT<T>::standard2InvertedFlipped(world_Q_camera);
		ocean_assert(flippedCamera_Q_world.isValid());

		{
			const QuaternionT<T> world_Q_flippedCamera = world_Q_camera * QuaternionT<T>(RotationT<T>(1, 0, 0, NumericT<T>::pi()));

			QuaternionT<T> test_flippedCamera_Q_world;
			if (world_Q_flippedCamera.invert(test_flippedCamera_Q_world))
			{
				for (unsigned int n = 0u; n < 5u; ++n)
				{
					ValidationPrecision::ScopedIteration scopedIteration(validation);

					const VectorT3<T> testVector = RandomT<T>::vector3(randomGenerator, -1, 1);

					if (!(flippedCamera_Q_world * testVector).isEqual(test_flippedCamera_Q_world * testVector, NumericT<T>::weakEps()))
					{
						scopedIteration.setInaccurate();
					}
				}
			}
		}

		{
			const QuaternionT<T> test_world_Q_camera = CameraT<T>::invertedFlipped2Standard(flippedCamera_Q_world);

			for (unsigned int n = 0u; n < 5u; ++n)
			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				const VectorT3<T> testVector = RandomT<T>::vector3(randomGenerator, -1, 1);

				if (!(world_Q_camera * testVector).isEqual(test_world_Q_camera * testVector, NumericT<T>::weakEps()))
				{
					scopedIteration.setInaccurate();
				}
			}
		}

		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
			const unsigned int height = RandomI::random(randomGenerator, 1u, 1080u);
			const T fovX = RandomT<T>::scalar(randomGenerator, NumericT<T>::deg2rad(25), NumericT<T>::deg2rad(120));

			const PinholeCameraT<T> pinholeCamera(width, height, fovX);

			const VectorT2<T> observation = RandomT<T>::vector2(randomGenerator, 0, T(width), 0, T(height));

			const LineT3<T> ray = pinholeCamera.ray(observation, HomogenousMatrixT4<T>(world_Q_camera));
			const VectorT3<T> position = ray.point(RandomT<T>::scalar(randomGenerator, T(0.1), T(2)));

			if (!pinholeCamera.template projectToImageIF<false, false>(HomogenousMatrixT4<T>(flippedCamera_Q_world), position).isEqual(observation, 2))
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
