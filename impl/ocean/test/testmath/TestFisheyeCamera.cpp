/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestFisheyeCamera.h"

#include "ocean/base/DataType.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/FisheyeCamera.h"
#include "ocean/math/Random.h"

#include "ocean/test/Validation.h"
#include "ocean/test/ValidationPrecision.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestFisheyeCamera::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "---   FisheyeCamera test:   ---";
	Log::info() << " ";

	allSucceeded = testCameraConstructor<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testCameraConstructor<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDistortion<float>(640u, 480u, testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testDistortion<double>(640u, 480u, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testVectorDistortionFree<float>(640u, 480u, testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testVectorDistortionFree<double>(640u, 480u, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testVectorDistorted<float>(640u, 480u, testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testVectorDistorted<double>(640u, 480u, testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() <<  "FisheyeCamera test succeeded.";
	}
	else
	{
		Log::info() <<  "FisheyeCamera test FAILED";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFisheyeCamera, CameraConstructor_float)
{
	EXPECT_TRUE(TestFisheyeCamera::testCameraConstructor<float>(GTEST_TEST_DURATION));
}

TEST(TestFisheyeCamera, CameraConstructor_double)
{
	EXPECT_TRUE(TestFisheyeCamera::testCameraConstructor<double>(GTEST_TEST_DURATION));
}

TEST(TestFisheyeCamera, Distortion_float)
{
	EXPECT_TRUE(TestFisheyeCamera::testDistortion<float>(640u, 480u, GTEST_TEST_DURATION));
}

TEST(TestFisheyeCamera, Distortion_double)
{
	EXPECT_TRUE(TestFisheyeCamera::testDistortion<double>(640u, 480u, GTEST_TEST_DURATION));
}

TEST(TestFisheyeCamera, VectorDistortionFree_float)
{
	EXPECT_TRUE(TestFisheyeCamera::testVectorDistortionFree<float>(640u, 480u, GTEST_TEST_DURATION));
}

TEST(TestFisheyeCamera, VectorDistortionFree_double)
{
	EXPECT_TRUE(TestFisheyeCamera::testVectorDistortionFree<double>(640u, 480u, GTEST_TEST_DURATION));
}

TEST(TestFisheyeCamera, VectorDistorted_float)
{
	EXPECT_TRUE(TestFisheyeCamera::testVectorDistorted<float>(640u, 480u, GTEST_TEST_DURATION));
}

TEST(TestFisheyeCamera, VectorDistorted_double)
{
	EXPECT_TRUE(TestFisheyeCamera::testVectorDistorted<double>(640u, 480u, GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

template <typename T>
bool TestFisheyeCamera::testCameraConstructor(const double testDuration)
{
	static_assert(std::is_same<T, float>::value || std::is_same<T, double>::value, "Template parameter T must be float or double.");

	ocean_assert(testDuration > 0.0);

	Log::info() << "FisheyeCameraT<" << TypeNamer::name<T>() << "> constructor test:";

	RandomGenerator randomGenerator;
	ValidationPrecision validation(0.999, randomGenerator);

	constexpr T eps = NumericT<T>::deg2rad(T(0.1));

	const Timestamp startTimestamp(true);

	do
	{
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const unsigned int width = RandomI::random(randomGenerator, 320u, 1920u);
			const unsigned int height = RandomI::random(randomGenerator, 240u, 1080u);

			const T fovX = RandomT<T>::scalar(randomGenerator, NumericT<T>::deg2rad(30), NumericT<T>::deg2rad(140));

			const FisheyeCameraT<T> fisheyeCamera(width, height, fovX);

			if (fisheyeCamera.width() != width || fisheyeCamera.height() != height)
			{
				OCEAN_SET_FAILED(validation);
			}

			if (!NumericT<T>::isEqual(fisheyeCamera.fovX(), fovX, eps))
			{
				scopedIteration.setInaccurate();
			}

			const FisheyeCameraT<T> cameraPerfectPrincipal(width, height, fisheyeCamera.focalLengthX(), fisheyeCamera.focalLengthY(), T(width) * T(0.5), T(height) * T(0.5));

			if (!NumericT<T>::isEqual(fisheyeCamera.fovX(), cameraPerfectPrincipal.fovX(), NumericT<T>::deg2rad(T(0.1))))
			{
				scopedIteration.setInaccurate();
			}
		}

		{
			const unsigned int width = RandomI::random(randomGenerator, 320u, 1920u);
			const unsigned int height = RandomI::random(randomGenerator, 240u, 1080u);

			const T principalX = RandomT<T>::scalar(randomGenerator, T(width) * T(0.1), T(width) * T(0.9));
			const T principalY = RandomT<T>::scalar(randomGenerator, T(height) * T(0.1), T(height) * T(0.9));

			const T fovX = RandomT<T>::scalar(randomGenerator, NumericT<T>::deg2rad(30), NumericT<T>::deg2rad(140));

			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				const T focalLength = (T(width) * T(0.5)) / NumericT<T>::tan(fovX * T(0.5));

				if (!NumericT<T>::isEqual(fovX, FisheyeCameraT<T>(width, height, focalLength, focalLength, T(width) * T(0.5), T(height) * T(0.5)).fovX(), eps))
				{
					scopedIteration.setInaccurate();
				}
			}

			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				const T focalLength = (T(width) * T(0.5)) / NumericT<T>::tan(fovX * T(0.5));

				const FisheyeCameraT<T> fisheyeCamera(width, height, focalLength, focalLength, principalX, principalY);

				if (fisheyeCamera.width() != width || fisheyeCamera.height() != height || fisheyeCamera.principalPointX() != principalX || fisheyeCamera.principalPointY() != principalY)
				{
					OCEAN_SET_FAILED(validation);
				}
				else
				{
					const T leftFovX = NumericT<T>::abs(NumericT<T>::atan(-principalX / focalLength));
					const T rightFovX = NumericT<T>::atan((T(width) - principalX) / focalLength);
					ocean_assert(leftFovX + rightFovX > NumericT<T>::eps());

					const T topFovY = NumericT<T>::abs(NumericT<T>::atan(-principalY / focalLength));
					const T bottomFovY = NumericT<T>::atan((T(height) - principalY) / focalLength);
					ocean_assert(topFovY + bottomFovY > NumericT<T>::eps());

					if (leftFovX + rightFovX > NumericT<T>::eps() && topFovY + bottomFovY > NumericT<T>::weakEps())
					{
						if (!NumericT<T>::isEqual(fisheyeCamera.fovX(), leftFovX + rightFovX, eps) || !NumericT<T>::isEqual(fisheyeCamera.fovY(), topFovY + bottomFovY, eps))
						{
							scopedIteration.setInaccurate();
						}
					}
				}
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestFisheyeCamera::testDistortion(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(std::is_same<T, float>::value || std::is_same<T, double>::value, "Template parameter T must be float or double.");

	ocean_assert(testDuration > 0.0);

	Log::info() << "Distortion test (" << TypeNamer::name<T>() << "):";

	RandomGenerator randomGenerator;
	ValidationPrecision validation(0.97, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 4u; ++n)
		{
			T radialDistortion[6];
			for (unsigned int i = 0u; i < 6u; ++i)
			{
				radialDistortion[i] = (n % 2u == 0u) ? T(0) : RandomT<T>::scalar(randomGenerator, T(-0.001), T(0.001));
			}

			T tangentialDistortion[2];
			tangentialDistortion[0] = (n <= 1) ? T(0) : RandomT<T>::scalar(randomGenerator, T(-0.001), T(0.001));
			tangentialDistortion[1] = (n <= 1) ? T(0) : RandomT<T>::scalar(randomGenerator, T(-0.001), T(0.001));

			const FisheyeCameraT<T> fisheyeCamera(width, height, RandomT<T>::scalar(randomGenerator, 500, 600), RandomT<T>::scalar(randomGenerator, 500, 600),
								RandomT<T>::scalar(randomGenerator, T(width) * T(0.5) - 50, T(width) * T(0.5) + 50),
								RandomT<T>::scalar(randomGenerator, T(height) * T(0.5) - 50, T(height) * T(0.5) + 50),
								radialDistortion, tangentialDistortion);

			const VectorT2<T> distortedImagePoint(RandomT<T>::scalar(randomGenerator, 0, T(width - 1)), RandomT<T>::scalar(randomGenerator, 0, T(height - 1)));

			const VectorT2<T> distortedNormalized((distortedImagePoint.x() - fisheyeCamera.principalPointX()) * fisheyeCamera.inverseFocalLengthX(),
													(distortedImagePoint.y() - fisheyeCamera.principalPointY()) * fisheyeCamera.inverseFocalLengthY());

			const VectorT2<T> undistortedNormalized = fisheyeCamera.template undistortNormalized<true>(distortedNormalized);
			const VectorT2<T> distortedNormalizedTest = fisheyeCamera.template distortNormalized<true>(undistortedNormalized);

			if (distortedNormalizedTest.x() > T(-2) && distortedNormalizedTest.x() <= T(2) && distortedNormalizedTest.y() > T(-2) && distortedNormalizedTest.y() <= T(2))
			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				if (NumericT<T>::isEqual(distortedNormalized.x(), distortedNormalizedTest.x(), T(0.01))
					&& NumericT<T>::isEqual(distortedNormalized.y(), distortedNormalizedTest.y(), T(0.01)))
				{
				}
				else
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
bool TestFisheyeCamera::testVectorDistortionFree(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(std::is_same<T, float>::value || std::is_same<T, double>::value, "Template parameter T must be float or double.");

	ocean_assert(testDuration > 0.0);

	Log::info() << "Vector test (" << TypeNamer::name<T>() << "):";

	RandomGenerator randomGenerator;
	ValidationPrecision validation(0.97, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		const T focalLengthX = RandomT<T>::scalar(randomGenerator, T(400), T(700));
		const T focalLengthY = focalLengthX + RandomT<T>::scalar(randomGenerator, T(-1), T(1));

		const T principalPointX(RandomT<T>::scalar(randomGenerator, T(width) * T(0.5) - T(30), T(width) * T(0.5) + T(30)));
		const T principalPointY(RandomT<T>::scalar(randomGenerator, T(height) * T(0.5) - T(30), T(height) * T(0.5) + T(30)));

		const FisheyeCameraT<T> fisheyeCamera(width, height, focalLengthX, focalLengthY, principalPointX, principalPointY);

		const VectorT2<T> imagePoint(RandomT<T>::scalar(randomGenerator, 0, T(width - 1)), RandomT<T>::scalar(randomGenerator, 0, T(height - 1)));
		const VectorT3<T> rayVector(fisheyeCamera.vector(imagePoint));

		if (NumericT<T>::isNotEqual(rayVector.length(), 1) || rayVector.z() > 0)
		{
			scopedIteration.setInaccurate();
		}

		const VectorT3<T> rayVectorFlipped(rayVector.x(), -rayVector.y(), -rayVector.z());
		ocean_assert(rayVectorFlipped.isEqual(fisheyeCamera.vectorIF(imagePoint, true), NumericT<T>::eps()));

		const VectorT2<T> projectedImagePoint = fisheyeCamera.template projectToImageIF<false>(rayVectorFlipped);

		if (NumericT<T>::abs(projectedImagePoint.x() - imagePoint.x()) > 0.1 || NumericT<T>::abs(projectedImagePoint.y() - imagePoint.y()) > 0.1)
		{
			scopedIteration.setInaccurate();
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestFisheyeCamera::testVectorDistorted(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(std::is_same<T, float>::value || std::is_same<T, double>::value, "Template parameter T must be float or double.");

	ocean_assert(testDuration > 0.0);

	Log::info() << "Distorted vector test (" << TypeNamer::name<T>() << "):";

	RandomGenerator randomGenerator;
	ValidationPrecision validation(0.97, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		const T focalLengthX = RandomT<T>::scalar(randomGenerator, T(400), T(700));
		const T focalLengthY = focalLengthX + RandomT<T>::scalar(randomGenerator, T(-1), T(1));

		const T principalPointX(RandomT<T>::scalar(randomGenerator, T(width) * T(0.5) - T(30), T(width) * T(0.5) + T(30)));
		const T principalPointY(RandomT<T>::scalar(randomGenerator, T(height) * T(0.5) - T(30), T(height) * T(0.5) + T(30)));

		T radialDistortion[6];
		for (unsigned int i = 0u; i < 6u; ++i)
		{
			radialDistortion[i] = RandomT<T>::scalar(randomGenerator, T(-0.001), T(0.001));
		}

		T tangentialDistortion[2];
		tangentialDistortion[0] = RandomT<T>::scalar(randomGenerator, T(-0.001), T(0.001));
		tangentialDistortion[1] = RandomT<T>::scalar(randomGenerator, T(-0.001), T(0.001));

		const FisheyeCameraT<T> fisheyeCamera(width, height, focalLengthX, focalLengthY, principalPointX, principalPointY, radialDistortion, tangentialDistortion);

		const VectorT2<T> distortedImagePoint(RandomT<T>::scalar(randomGenerator, 0, T(width - 1)), RandomT<T>::scalar(randomGenerator, 0, T(height - 1)));

		const VectorT3<T> rayVector(fisheyeCamera.vector(distortedImagePoint));

		if (NumericT<T>::isNotEqual(rayVector.length(), 1) || rayVector.z() > 0)
		{
			scopedIteration.setInaccurate();
		}

		const VectorT3<T> rayVectorFlipped(rayVector.x(), -rayVector.y(), -rayVector.z());

		const VectorT2<T> projectedImagePoint(fisheyeCamera.template projectToImageIF<true>(rayVectorFlipped));

		if (NumericT<T>::abs(projectedImagePoint.x() - distortedImagePoint.x()) > 0.1 || NumericT<T>::abs(projectedImagePoint.y() - distortedImagePoint.y()) > 0.1)
		{
			scopedIteration.setInaccurate();
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
