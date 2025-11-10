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

#include "ocean/test/ValidationPrecision.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestCamera::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "---   Camera test:   ---";
	Log::info() << " ";

	allSucceeded = testObjectPointInFront<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testObjectPointInFront<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testStandard2InvertedFlippedHomogenousMatrix4<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testStandard2InvertedFlippedHomogenousMatrix4<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testStandard2InvertedFlippedSquareMatrix3<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testStandard2InvertedFlippedSquareMatrix3<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testStandard2InvertedFlippedQuaternion<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testStandard2InvertedFlippedQuaternion<double>(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() <<  "Camera test succeeded.";
	}
	else
	{
		Log::info() <<  "Camera test FAILED";
	}

	return allSucceeded;
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

				const SquareMatrixT3<T> orientation(true);
				const HomogenousMatrixT4<T> pose(VectorT3<T>(RandomT<T>::scalar(randomGenerator, -10, 10), RandomT<T>::scalar(randomGenerator, -10, -10), 0));

				const SquareMatrixT3<T> orientationIF(CameraT<T>::standard2InvertedFlipped(orientation));
				const HomogenousMatrixT4<T> poseIF(CameraT<T>::standard2InvertedFlipped(pose));

				const VectorT3<T> frontObjectPoint(RandomT<T>::scalar(randomGenerator, -100, 100), RandomT<T>::scalar(randomGenerator, -100, 100), RandomT<T>::scalar(randomGenerator, -100, -NumericT<T>::eps() * T(100)));
				const VectorT3<T> backObjectPoint(RandomT<T>::scalar(randomGenerator, -100, 100), RandomT<T>::scalar(randomGenerator, -100, 100), RandomT<T>::scalar(randomGenerator, 0, 100));

				if (!CameraT<T>::isObjectPointInFrontIF(poseIF, frontObjectPoint) || !CameraT<T>::isObjectPointInFrontIF(orientationIF, frontObjectPoint) || CameraT<T>::isObjectPointInFrontIF(poseIF, backObjectPoint) || CameraT<T>::isObjectPointInFrontIF(orientationIF, backObjectPoint))
				{
					scopedIteration.setInaccurate();
				}
			}

			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				// arbitrary pose

				const SquareMatrixT3<T> orientation(RandomT<T>::quaternion(randomGenerator));
				const HomogenousMatrixT4<T> pose(RandomT<T>::vector3(randomGenerator, -10, 10), orientation);

				const HomogenousMatrixT4<T> poseIF(CameraT<T>::standard2InvertedFlipped(pose));
				const SquareMatrixT3<T> orientationIF(CameraT<T>::standard2InvertedFlipped(orientation));

				const VectorT3<T> localFrontObjectPoint(RandomT<T>::scalar(randomGenerator, -100, 100), RandomT<T>::scalar(randomGenerator, -100, 100), RandomT<T>::scalar(randomGenerator, -100, -NumericT<T>::eps() * T(100)));
				const VectorT3<T> localBackObjectPoint(RandomT<T>::scalar(randomGenerator, -100, 100), RandomT<T>::scalar(randomGenerator, -100, 100), RandomT<T>::scalar(randomGenerator, 0, 100));

				const VectorT3<T> orientationFrontObjectPoint(orientation * localFrontObjectPoint);
				const VectorT3<T> orientationBackObjectPoint(orientation * localBackObjectPoint);

				const VectorT3<T> poseFrontObjectPoint(pose * localFrontObjectPoint);
				const VectorT3<T> poseBackObjectPoint(pose * localBackObjectPoint);

				if (!CameraT<T>::isObjectPointInFrontIF(poseIF, poseFrontObjectPoint) || !CameraT<T>::isObjectPointInFrontIF(orientationIF, orientationFrontObjectPoint) || CameraT<T>::isObjectPointInFrontIF(poseIF, poseBackObjectPoint) || PinholeCameraT<T>::isObjectPointInFrontIF(orientationIF, orientationBackObjectPoint))
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
			const HomogenousMatrixT4<T> wTc(translation, quaternion, scale);
			ocean_assert(wTc.isValid());

			const HomogenousMatrixT4<T> fTw = CameraT<T>::standard2InvertedFlipped(wTc);
			ocean_assert(fTw.isValid());

			const HomogenousMatrixT4<T> wTf = wTc * HomogenousMatrixT4<T>(RotationT<T>(1, 0, 0, NumericT<T>::pi()));

			HomogenousMatrixT4<T> test_fTw;
			if (wTf.invert(test_fTw))
			{
				for (unsigned int n = 0u; n < 5u; ++n)
				{
					ValidationPrecision::ScopedIteration scopedIteration(validation);

					const VectorT3<T> testVector = RandomT<T>::vector3(randomGenerator, -1, 1);

					if (!(fTw * testVector).isEqual(test_fTw * testVector, NumericT<T>::weakEps()))
					{
						scopedIteration.setInaccurate();
					}
				}
			}

			const HomogenousMatrixT4<T> test_wTc = CameraT<T>::invertedFlipped2Standard(fTw);

			for (unsigned int n = 0u; n < 5u; ++n)
			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				const VectorT3<T> testVector = RandomT<T>::vector3(randomGenerator, -1, 1);

				if (!(wTc * testVector).isEqual(test_wTc * testVector, NumericT<T>::weakEps()))
				{
					scopedIteration.setInaccurate();
				}
			}
		}

		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const HomogenousMatrixT4<T> wTc(translation, quaternion);
			ocean_assert(wTc.isValid());

			const HomogenousMatrixT4<T> fTw = CameraT<T>::standard2InvertedFlipped(wTc);
			ocean_assert(fTw.isValid());

			const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
			const unsigned int height = RandomI::random(randomGenerator, 1u, 1080u);
			const T fovX = RandomT<T>::scalar(randomGenerator, NumericT<T>::deg2rad(25), NumericT<T>::deg2rad(120));

			const PinholeCameraT<T> pinholeCamera(width, height, fovX);

			const VectorT2<T> observation = RandomT<T>::vector2(randomGenerator, 0, T(width), 0, T(height));

			const LineT3<T> ray = pinholeCamera.ray(observation, wTc);
			const VectorT3<T> position = ray.point(RandomT<T>::scalar(randomGenerator, T(0.1), T(2)));

			if (!pinholeCamera.template projectToImageIF<false, false>(fTw, position).isEqual(observation, 2))
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
		const SquareMatrixT3<T> wTc(RandomT<T>::rotation(randomGenerator));
		ocean_assert(!wTc.isSingular());

		const SquareMatrixT3<T> fTw = CameraT<T>::standard2InvertedFlipped(wTc);
		ocean_assert(!fTw.isSingular());

		{
			const SquareMatrixT3<T> wTf = wTc * SquareMatrixT3<T>(RotationT<T>(1, 0, 0, NumericT<T>::pi()));

			SquareMatrixT3<T> test_fTw;
			if (wTf.invert(test_fTw))
			{
				for (unsigned int n = 0u; n < 5u; ++n)
				{
					ValidationPrecision::ScopedIteration scopedIteration(validation);

					const VectorT3<T> testVector = RandomT<T>::vector3(randomGenerator, -1, 1);

					if (!(fTw * testVector).isEqual(test_fTw * testVector, NumericT<T>::weakEps()))
					{
						scopedIteration.setInaccurate();
					}
				}
			}
		}

		{
			const SquareMatrixT3<T> test_wTc = CameraT<T>::invertedFlipped2Standard(fTw);

			for (unsigned int n = 0u; n < 5u; ++n)
			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				const VectorT3<T> testVector = RandomT<T>::vector3(randomGenerator, -1, 1);

				if (!(wTc * testVector).isEqual(test_wTc * testVector, NumericT<T>::weakEps()))
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

			const LineT3<T> ray = pinholeCamera.ray(observation, HomogenousMatrixT4<T>(wTc));
			const VectorT3<T> position = ray.point(RandomT<T>::scalar(randomGenerator, T(0.1), T(2)));

			if (!pinholeCamera.template projectToImageIF<false, false>(HomogenousMatrixT4<T>(fTw), position).isEqual(observation, 2))
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
		const QuaternionT<T> wTc(RandomT<T>::quaternion(randomGenerator));
		ocean_assert(wTc.isValid());

		const QuaternionT<T> fTw = CameraT<T>::standard2InvertedFlipped(wTc);
		ocean_assert(fTw.isValid());

		{
			const QuaternionT<T> wTf = wTc * QuaternionT<T>(RotationT<T>(1, 0, 0, NumericT<T>::pi()));

			QuaternionT<T> test_fTw;
			if (wTf.invert(test_fTw))
			{
				for (unsigned int n = 0u; n < 5u; ++n)
				{
					ValidationPrecision::ScopedIteration scopedIteration(validation);

					const VectorT3<T> testVector = RandomT<T>::vector3(randomGenerator, -1, 1);

					if (!(fTw * testVector).isEqual(test_fTw * testVector, NumericT<T>::weakEps()))
					{
						scopedIteration.setInaccurate();
					}
				}
			}
		}

		{
			const QuaternionT<T> test_wTc = CameraT<T>::invertedFlipped2Standard(fTw);

			for (unsigned int n = 0u; n < 5u; ++n)
			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				const VectorT3<T> testVector = RandomT<T>::vector3(randomGenerator, -1, 1);

				if (!(wTc * testVector).isEqual(test_wTc * testVector, NumericT<T>::weakEps()))
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

			const LineT3<T> ray = pinholeCamera.ray(observation, HomogenousMatrixT4<T>(wTc));
			const VectorT3<T> position = ray.point(RandomT<T>::scalar(randomGenerator, T(0.1), T(2)));

			if (!pinholeCamera.template projectToImageIF<false, false>(HomogenousMatrixT4<T>(fTw), position).isEqual(observation, 2))
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
