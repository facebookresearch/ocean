/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestPinholeCamera.h"

#include "ocean/base/DataType.h"
#include "ocean/base/Timestamp.h"

#include "ocean/test/TestResult.h"

#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Random.h"
#include "ocean/math/SquareMatrix3.h"

#include "ocean/test/Validation.h"
#include "ocean/test/ValidationPrecision.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestPinholeCamera::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("PinholeCamera test");

	Log::info() << " ";

	if (selector.shouldRun("cameraconstructor"))
	{
		testResult = testCameraConstructor<float>(testDuration);
		Log::info() << " ";
		testResult = testCameraConstructor<double>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("patterncamera"))
	{
		testResult = testPatternCamera<float>(testDuration);
		Log::info() << " ";
		testResult = testPatternCamera<double>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("subframecamera"))
	{
		testResult = testSubFrameCamera<float>(testDuration);
		Log::info() << " ";
		testResult = testSubFrameCamera<double>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("distortion"))
	{
		testResult = testDistortion<float>(640u, 480u, testDuration);
		Log::info() << " ";
		testResult = testDistortion<double>(640u, 480u, testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("vectordistortionfree"))
	{
		testResult = testVectorDistortionFree<float>(640u, 480u, testDuration);
		Log::info() << " ";
		testResult = testVectorDistortionFree<double>(640u, 480u, testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("vectordistorted"))
	{
		testResult = testVectorDistorted<float>(640u, 480u, testDuration);
		Log::info() << " ";
		testResult = testVectorDistorted<double>(640u, 480u, testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("fovdiagonal"))
	{
		testResult = testFovDiagonal<float>(testDuration);
		Log::info() << " ";
		testResult = testFovDiagonal<double>(testDuration);

		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestPinholeCamera, CameraConstructor_float)
{
	EXPECT_TRUE(TestPinholeCamera::testCameraConstructor<float>(GTEST_TEST_DURATION));
}

TEST(TestPinholeCamera, CameraConstructor_double)
{
	EXPECT_TRUE(TestPinholeCamera::testCameraConstructor<double>(GTEST_TEST_DURATION));
}

TEST(TestPinholeCamera, PatternCamera_float)
{
	EXPECT_TRUE(TestPinholeCamera::testPatternCamera<float>(GTEST_TEST_DURATION));
}

TEST(TestPinholeCamera, PatternCamera_double)
{
	EXPECT_TRUE(TestPinholeCamera::testPatternCamera<double>(GTEST_TEST_DURATION));
}

TEST(TestPinholeCamera, SubFrameCamera_float)
{
	EXPECT_TRUE(TestPinholeCamera::testSubFrameCamera<float>(GTEST_TEST_DURATION));
}

TEST(TestPinholeCamera, SubFrameCamera_double)
{
	EXPECT_TRUE(TestPinholeCamera::testSubFrameCamera<double>(GTEST_TEST_DURATION));
}

TEST(TestPinholeCamera, Distortion_float)
{
	EXPECT_TRUE(TestPinholeCamera::testDistortion<float>(640u, 480u, GTEST_TEST_DURATION));
}

TEST(TestPinholeCamera, Distortion_double)
{
	EXPECT_TRUE(TestPinholeCamera::testDistortion<double>(640u, 480u, GTEST_TEST_DURATION));
}

TEST(TestPinholeCamera, VectorDistortionFree_float)
{
	EXPECT_TRUE(TestPinholeCamera::testVectorDistortionFree<float>(640u, 480u, GTEST_TEST_DURATION));
}

TEST(TestPinholeCamera, VectorDistortionFree_double)
{
	EXPECT_TRUE(TestPinholeCamera::testVectorDistortionFree<double>(640u, 480u, GTEST_TEST_DURATION));
}

TEST(TestPinholeCamera, VectorDistorted_float)
{
	EXPECT_TRUE(TestPinholeCamera::testVectorDistorted<float>(640u, 480u, GTEST_TEST_DURATION));
}

TEST(TestPinholeCamera, VectorDistorted_double)
{
	EXPECT_TRUE(TestPinholeCamera::testVectorDistorted<double>(640u, 480u, GTEST_TEST_DURATION));
}

TEST(TestPinholeCamera, FovDiagonal_float)
{
	EXPECT_TRUE(TestPinholeCamera::testFovDiagonal<float>(GTEST_TEST_DURATION));
}

TEST(TestPinholeCamera, FovDiagonal_double)
{
	EXPECT_TRUE(TestPinholeCamera::testFovDiagonal<double>(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

template <typename T>
bool TestPinholeCamera::testCameraConstructor(const double testDuration)
{
	static_assert(std::is_same<T, float>::value || std::is_same<T, double>::value, "Template parameter T must be float or double.");

	ocean_assert(testDuration > 0.0);

	Log::info() << "PinholeCameraT<" << TypeNamer::name<T>() << "> constructor test:";

	RandomGenerator randomGenerator;
	ValidationPrecision validation(0.999, randomGenerator);

	constexpr T eps = NumericT<T>::deg2rad(T(0.1));

	const Timestamp startTimestamp(true);

	do
	{
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			// testing constructor with width, height, a fovx

			const unsigned int width = RandomI::random(randomGenerator, 320u, 1920u);
			const unsigned int height = RandomI::random(randomGenerator, 240u, 1080u);

			const T fovX = RandomT<T>::scalar(randomGenerator, NumericT<T>::deg2rad(30), NumericT<T>::deg2rad(70));

			const PinholeCameraT<T> pinholeCamera(width, height, fovX);

			if (pinholeCamera.width() != width || pinholeCamera.height() != height)
			{
				OCEAN_SET_FAILED(validation);
			}

			if (!NumericT<T>::isEqual(pinholeCamera.fovX(), fovX, eps))
			{
				scopedIteration.setInaccurate();
			}

			const PinholeCameraT<T> cameraPerfectPrincipal(width, height, fovX, T(width) * T(0.5), T(height) * T(0.5));

			if (!NumericT<T>::isEqual(pinholeCamera.fovX(), cameraPerfectPrincipal.fovX(), NumericT<T>::deg2rad(T(0.1))))
			{
				scopedIteration.setInaccurate();
			}
		}

		{
			// testing constructor with width, height, fovx, and principal point

			const unsigned int width = RandomI::random(randomGenerator, 320u, 1920u);
			const unsigned int height = RandomI::random(randomGenerator, 240u, 1080u);

			const T principalX = RandomT<T>::scalar(randomGenerator, T(width) * T(-0.5), T(width) * T(1.5));
			const T principalY = RandomT<T>::scalar(randomGenerator, T(height) * T(-0.5), T(height) * T(1.5));

			const T fovX = RandomT<T>::scalar(randomGenerator, NumericT<T>::deg2rad(30), NumericT<T>::deg2rad(70));

			// first we check a pinhole camera with perfect principal point

			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				if (!NumericT<T>::isEqual(fovX, PinholeCameraT<T>(width, height, fovX, T(width) * T(0.5), T(height) * T(0.5)).fovX(), eps))
				{
					scopedIteration.setInaccurate();
				}
			}

			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				const PinholeCameraT<T> pinholeCamera(width, height, fovX, principalX, principalY);

				if (pinholeCamera.width() != width || pinholeCamera.height() != height || pinholeCamera.principalPointX() != principalX || pinholeCamera.principalPointY() != principalY)
				{
					OCEAN_SET_FAILED(validation);
				}
				else
				{
					// the real horizontal/vertical fov depends on the principal point

					//      px/py
					// ---------------------------
					// \      |                  /
					//  \     |               /
					//   \ fa |            /
					//    \   |  fb     /
					//     \  |      /
					//      \ |   /
					//       \|/
					//
					// fov = fa + fb (if principal point is in the perfect center we have fa == fb)


					// in case the principal point is outside of the frame
					//
					// px/py
					//          -------------
					//   |     /            /
					//   |    /          /
					//   |fa /        /
					//   |  /      /
					//   | / fb /
					//   |/  /
					//   |/
					//
					// fov = -fa + fb

					const T idealFocalLength = (T(width) * T(0.5)) / NumericT<T>::tan(fovX * T(0.5));

					const T leftFovX = NumericT<T>::atan(principalX / idealFocalLength);
					const T rightFovX = NumericT<T>::atan((T(width) - principalX) / idealFocalLength);
					ocean_assert(leftFovX + rightFovX > NumericT<T>::eps());

					const T topFovY = NumericT<T>::atan(principalY / idealFocalLength);
					const T bottomFovY = NumericT<T>::atan((T(height) - principalY) / idealFocalLength);
					ocean_assert(topFovY + bottomFovY > NumericT<T>::eps());

					if (leftFovX + rightFovX > NumericT<T>::eps() && topFovY + bottomFovY > NumericT<T>::weakEps())
					{
						if (!NumericT<T>::isEqual(leftFovX, pinholeCamera.fovXLeft(), eps) || !NumericT<T>::isEqual(rightFovX, pinholeCamera.fovXRight(), eps))
						{
							scopedIteration.setInaccurate();
						}

						if (!NumericT<T>::isEqual(topFovY, pinholeCamera.fovYTop(), eps) || !NumericT<T>::isEqual(bottomFovY, pinholeCamera.fovYBottom(), eps))
						{
							scopedIteration.setInaccurate();
						}

						if (leftFovX + rightFovX <= fovX + NumericT<T>::weakEps())
						{
							if (!NumericT<T>::isEqual(pinholeCamera.fovX(), leftFovX + rightFovX, eps) || !NumericT<T>::isEqual(pinholeCamera.fovY(), topFovY + bottomFovY, eps))
							{
								scopedIteration.setInaccurate();
							}
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
bool TestPinholeCamera::testPatternCamera(const double testDuration)
{
	static_assert(std::is_same<T, float>::value || std::is_same<T, double>::value, "Template parameter T must be float or double.");

	ocean_assert(testDuration > 0.0);

	Log::info() << "Pattern pinhole camera test (" << TypeNamer::name<T>() << "):";

	RandomGenerator randomGenerator;
	ValidationPrecision validation(0.999, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		const unsigned int width = RandomI::random(randomGenerator, 320u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 240u, 1080u);

		const T principalX = RandomT<T>::scalar(randomGenerator, T(-100), T(width + 100u));
		const T principalY = RandomT<T>::scalar(randomGenerator, T(-100), T(height + 100u));

		const T fovX = RandomT<T>::scalar(randomGenerator, NumericT<T>::deg2rad(30), NumericT<T>::deg2rad(70));

		const PinholeCameraT<T> patternCamera(width, height, fovX, principalX, principalY);

		const T factor = RandomT<T>::scalar(randomGenerator, T(0.25), T(4.0));

		const unsigned int newWidth = (unsigned int)(T(width) * factor + T(0.5));
		const unsigned int newHeight = (unsigned int)(T(height) * factor + T(0.5));

		const PinholeCameraT<T> newCamera(newWidth, newHeight, patternCamera);

		// we need to ensure that the field of view is almost identical, and that the (normalized) principal point is almost identical

		const T patternFovX = patternCamera.fovX();
		const T patternFovY = patternCamera.fovY();

		const T newFovX = newCamera.fovX();
		const T newFovY = newCamera.fovY();

		if (!NumericT<T>::isEqual(patternFovX, newFovX, NumericT<T>::deg2rad(5)))
		{
			scopedIteration.setInaccurate();
		}

		if (!NumericT<T>::isEqual(patternFovY, newFovY, NumericT<T>::deg2rad(5)))
		{
			scopedIteration.setInaccurate();
		}

		const T patternPrincipalX = patternCamera.principalPointX() / T(patternCamera.width());
		const T patternPrincipalY = patternCamera.principalPointY() / T(patternCamera.height());

		const T newPrincipalX = newCamera.principalPointX() / T(newCamera.width());
		const T newPrincipalY = newCamera.principalPointY() / T(newCamera.height());

		if (!NumericT<T>::isEqual(patternPrincipalX, newPrincipalX, T(0.05))) // 5%
		{
			scopedIteration.setInaccurate();
		}

		if (!NumericT<T>::isEqual(patternPrincipalY, newPrincipalY, T(0.05))) // 5%
		{
			scopedIteration.setInaccurate();
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestPinholeCamera::testSubFrameCamera(const double testDuration)
{
	static_assert(std::is_same<T, float>::value || std::is_same<T, double>::value, "Template parameter T must be float or double.");

	ocean_assert(testDuration > 0.0);

	Log::info() << "Sub-frame pinhole camera test (" << TypeNamer::name<T>() << "):";

	RandomGenerator randomGenerator;
	ValidationPrecision validation(0.999, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		const unsigned int width = RandomI::random(randomGenerator, 320u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 240u, 1080u);

		const T principalX = T(width) * T(0.5) + RandomT<T>::scalar(randomGenerator, -100, 100);
		const T principalY = T(height) * T(0.5) + RandomT<T>::scalar(randomGenerator, -100, 100);

		const T fovX = RandomT<T>::scalar(randomGenerator, NumericT<T>::deg2rad(30), NumericT<T>::deg2rad(70));

		const PinholeCameraT<T> cameraFull(width, height, fovX, principalX, principalY);

		const unsigned int subFrameWidth = RandomI::random(randomGenerator, 50u, width);
		const unsigned int subFrameHeight = RandomI::random(randomGenerator, 50u, height);

		const T subFrameLeft = RandomT<T>::scalar(randomGenerator, T(0u), T(width - subFrameWidth));
		const T subFrameTop = RandomT<T>::scalar(randomGenerator, T(0u), T(height - subFrameHeight));

		const PinholeCameraT<T> cameraSubFrame(subFrameLeft, subFrameTop, subFrameWidth, subFrameHeight, cameraFull);

		const VectorT2<T> pointInSubFrame = RandomT<T>::vector2(randomGenerator, T(0), T(cameraSubFrame.width()), T(0), T(cameraSubFrame.height()));

		const VectorT3<T> ray = cameraSubFrame.vector(pointInSubFrame);
		const VectorT2<T> pointInFullFrame = cameraFull.template projectToImage<false>(HomogenousMatrixT4<T>(true), ray, false);

		const T distance = pointInFullFrame.distance(pointInSubFrame + VectorT2<T>(subFrameLeft, subFrameTop));

		if (distance >= T(0.1))
		{
			scopedIteration.setInaccurate();
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestPinholeCamera::testDistortion(const unsigned int width, const unsigned int height, const double testDuration)
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
			T k1 = (n % 2u == 0u) ? 0 : RandomT<T>::scalar(randomGenerator, T(-0.1), T(0.1));
			T k2 = (n % 2u == 0u) ? 0 : RandomT<T>::scalar(randomGenerator, T(-0.1), T(0.1));

			if (k1 * k2 > 0)
			{
				continue;
			}

			T p1 = (n <= 1) ? 0 : RandomT<T>::scalar(randomGenerator, T(-0.01), T(0.01));
			T p2 = (n <= 1) ? 0 : RandomT<T>::scalar(randomGenerator, T(-0.01), T(0.01));

			const PinholeCameraT<T> pinholeCamera(width, height, RandomT<T>::scalar(randomGenerator, 500, 600), RandomT<T>::scalar(randomGenerator, 500, 600),
								RandomT<T>::scalar(randomGenerator, T(width) * T(0.5) - 50, T(width) * T(0.5) + 50),
								RandomT<T>::scalar(randomGenerator, T(height) * T(0.5) - 50, T(height) * T(0.5) + 50),
								typename PinholeCameraT<T>::DistortionPair(k1, k2),
								typename PinholeCameraT<T>::DistortionPair(p1, p2));

			const VectorT2<T> undistortedPoint(RandomT<T>::scalar(randomGenerator, 0, T(width - 1)), RandomT<T>::scalar(randomGenerator, 0, T(height - 1)));

			const VectorT2<T> distortedPoint = pinholeCamera.template distort<true>(undistortedPoint);
			if (distortedPoint.x() > 0 && distortedPoint.x() <= T(width - 1) && distortedPoint.y() > 0 && distortedPoint.y() <= T(height - 1))
			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				const VectorT2<T> calculatedUndistortedPoint = pinholeCamera.template undistort<true>(distortedPoint, 100u);

				if (!pinholeCamera.isInside(distortedPoint) || (NumericT<T>::isEqual(undistortedPoint.x(), calculatedUndistortedPoint.x(), T(0.1))
					&& NumericT<T>::isEqual(undistortedPoint.y(), calculatedUndistortedPoint.y(), T(0.1))))
				{
					// validation succeeded
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
bool TestPinholeCamera::testVectorDistortionFree(const unsigned int width, const unsigned int height, const double testDuration)
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

		const T principalPointX(RandomT<T>::scalar(randomGenerator, T(width) * T(0.5) - T(30), T(width) * T(0.5) + T(30)));
		const T principalPointY(RandomT<T>::scalar(randomGenerator, T(height) * T(0.5) - T(30), T(height) * T(0.5) + T(30)));

		ocean_assert(principalPointX > 0 && principalPointX < T(width));
		ocean_assert(principalPointY > 0 && principalPointY < T(height));

		const PinholeCameraT<T> pinholeCamera(width, height, NumericT<T>::deg2rad(T(55.1)), principalPointX, principalPointY);

		const VectorT2<T> imagePoint(RandomT<T>::scalar(randomGenerator, 0, T(width - 1)), RandomT<T>::scalar(randomGenerator, 0, T(height - 1)));
		const VectorT3<T> rayVector(pinholeCamera.vector(imagePoint));

		// the vector must have length 1
		if (NumericT<T>::isNotEqual(rayVector.length(), 1) || rayVector.z() > 0)
		{
			scopedIteration.setInaccurate();
		}

		// 3D -> 2D projection
		const VectorT3<T> rayVectorFlipped(rayVector.x(), -rayVector.y(), -rayVector.z());
		ocean_assert(rayVectorFlipped.isEqual(pinholeCamera.vectorIF(imagePoint), NumericT<T>::eps()));
		ocean_assert(rayVectorFlipped.isEqual(pinholeCamera.vectorIF(imagePoint, T(1)), NumericT<T>::eps()));

		const VectorT3<T> projectedHomogenousImagePoint(pinholeCamera.intrinsic() * rayVectorFlipped);
		ocean_assert(NumericT<T>::isNotEqualEps(projectedHomogenousImagePoint.z()));

		const VectorT2<T> projectedImagePoint(projectedHomogenousImagePoint.x() / projectedHomogenousImagePoint.z(), projectedHomogenousImagePoint.y() / projectedHomogenousImagePoint.z());

		if (NumericT<T>::abs(projectedImagePoint.x() - imagePoint.x()) > 0.05 || NumericT<T>::abs(projectedImagePoint.y() - imagePoint.y()) > 0.05)
		{
			scopedIteration.setInaccurate();
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestPinholeCamera::testVectorDistorted(const unsigned int width, const unsigned int height, const double testDuration)
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

		const T principalPointX(RandomT<T>::scalar(randomGenerator, T(width) * T(0.5) - T(30), T(width) * T(0.5) + T(30)));
		const T principalPointY(RandomT<T>::scalar(randomGenerator, T(height) * T(0.5) - T(30), T(height) * T(0.5) + T(30)));

		ocean_assert_and_suppress_unused(principalPointX > 0 && principalPointX < T(width), principalPointX);
		ocean_assert_and_suppress_unused(principalPointY > 0 && principalPointY < T(height), principalPointY);

		PinholeCameraT<T> pinholeCamera(width, height, NumericT<T>::deg2rad(T(55.1)));
		pinholeCamera.setRadialDistortion(typename PinholeCameraT<T>::DistortionPair(RandomT<T>::scalar(randomGenerator, T(-0.1), T(0.1)), RandomT<T>::scalar(randomGenerator, T(-0.1), T(0.1))));
		pinholeCamera.setTangentialDistortion(typename PinholeCameraT<T>::DistortionPair(RandomT<T>::scalar(randomGenerator, T(-0.05), T(0.05)), RandomT<T>::scalar(randomGenerator, T(-0.05), T(0.05))));

		const VectorT2<T> distortedImagePoint(RandomT<T>::scalar(randomGenerator, 0, T(width - 1)), RandomT<T>::scalar(randomGenerator, 0, T(height - 1)));
		const VectorT2<T> undistortedImagePoint(pinholeCamera.template undistort<true>(distortedImagePoint));

		const VectorT3<T> rayVector(pinholeCamera.vector(undistortedImagePoint));

		// the vector must have length 1
		if (NumericT<T>::isNotEqual(rayVector.length(), 1) || rayVector.z() > 0)
		{
			scopedIteration.setInaccurate();
		}

		// 3D -> 2D projection
		const VectorT3<T> rayVectorFlipped(rayVector.x(), -rayVector.y(), -rayVector.z());

		const VectorT3<T> undistortedProjectedHomogenousImagePoint(pinholeCamera.intrinsic() * rayVectorFlipped);
		ocean_assert(NumericT<T>::isNotEqualEps(undistortedProjectedHomogenousImagePoint.z()));

		const VectorT2<T> undistortedProjectedImagePoint(undistortedProjectedHomogenousImagePoint.x() / undistortedProjectedHomogenousImagePoint.z(), undistortedProjectedHomogenousImagePoint.y() / undistortedProjectedHomogenousImagePoint.z());

		if (NumericT<T>::abs(undistortedProjectedImagePoint.x() - undistortedImagePoint.x()) > 0.05 || NumericT<T>::abs(undistortedProjectedImagePoint.y() - undistortedImagePoint.y()) > 0.05)
		{
			scopedIteration.setInaccurate();
		}

		const VectorT2<T> distortedProjectedImagePoint(pinholeCamera.template distort<true>(undistortedProjectedImagePoint));

		if (NumericT<T>::abs(distortedProjectedImagePoint.x() - distortedImagePoint.x()) > 0.05 || NumericT<T>::abs(distortedProjectedImagePoint.y() - distortedImagePoint.y()) > 0.05)
		{
			scopedIteration.setInaccurate();
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestPinholeCamera::testFovDiagonal(const double testDuration)
{
	static_assert(std::is_same<T, float>::value || std::is_same<T, double>::value, "Template parameter T must be float or double.");

	ocean_assert(testDuration > 0.0);

	Log::info() << "Diagonal field of view test (" << TypeNamer::name<T>() << "):";

	RandomGenerator randomGenerator;
	ValidationPrecision validation(0.99, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		const unsigned int width = RandomI::random(randomGenerator, 320u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 240u, 1080u);

		const T focalLengthX = RandomT<T>::scalar(randomGenerator, T(300), T(1000));
		const T focalLengthY = RandomT<T>::scalar(randomGenerator, T(300), T(1000));

		{
			const T principalX = RandomT<T>::scalar(randomGenerator, T(width) * T(0.3), T(width) * T(0.7));
			const T principalY = RandomT<T>::scalar(randomGenerator, T(height) * T(0.3), T(height) * T(0.7));

			const PinholeCameraT<T> pinholeCamera(width, height, focalLengthX, focalLengthY, principalX, principalY);

			/*
			 * The diagonal FOV is computed as an approximation based on the maximum sum of distances
			 * from the principal point to opposite corners of the image:
			 * <pre>
			 *   Image plane (normalized coordinates, principal point at origin):
			 *
			 *   TL (-px, -py)        TR (w-px, -py)
			 *    +--------------------+
			 *    |        |           |
			 *    |        |           |
			 *    |--------O (0,0)     |  O = principal point
			 *    |        |           |
			 *    |        |           |
			 *    +--------------------+
			 *   BL (-px, h-py)       BR (w-px, h-py)
			 *
			 *   Calculation:
			 *   1. Compute d1 = |TL| + |BR| and d2 = |TR| + |BL|
			 *   2. maxDiagonal = max(d1, d2)
			 *   3. avgFocalLength = (fx + fy) / 2
			 *   4. fovDiagonal = 2 * atan(maxDiagonal / (2 * avgFocalLength))
			 *
			 *   Note: This is an approximation. For off-center principal points,
			 *   the true angle between corner rays would differ from this formula.
			 * </pre>
			 */

			const VectorT2<T> cornerTopLeft(-principalX, -principalY);
			const VectorT2<T> cornerTopRight(T(width) - principalX, -principalY);
			const VectorT2<T> cornerBottomLeft(-principalX, T(height) - principalY);
			const VectorT2<T> cornerBottomRight(T(width) - principalX, T(height) - principalY);

			const T lengthDiagonalTopLeftBottomRight = cornerTopLeft.length() + cornerBottomRight.length();
			const T lengthDiagonalBottomLeftTopRight = cornerBottomLeft.length() + cornerTopRight.length();

			const T maxDiagonal = std::max(lengthDiagonalTopLeftBottomRight, lengthDiagonalBottomLeftTopRight);
			const T maxDiagonal_2 = maxDiagonal * T(0.5);

			const T focalLength = (focalLengthX + focalLengthY) * T(0.5);
			const T expectedFovDiagonal = T(2) * NumericT<T>::atan(maxDiagonal_2 / focalLength);

			const T actualFovDiagonal = pinholeCamera.fovDiagonal();

			if (!NumericT<T>::isEqual(actualFovDiagonal, expectedFovDiagonal, NumericT<T>::eps()))
			{
				scopedIteration.setInaccurate();
			}

			if (actualFovDiagonal <= T(0) || actualFovDiagonal >= NumericT<T>::pi())
			{
				OCEAN_SET_FAILED(validation);
			}
		}

		{
			const T focalLength = (focalLengthX + focalLengthY) * T(0.5);

			const PinholeCameraT<T> perfectCamera(width, height, focalLength, focalLength, T(width) * T(0.5), T(height) * T(0.5));

			const T fovDiagonalPerfect = perfectCamera.fovDiagonal();

			const T fovX = perfectCamera.fovX();
			const T fovY = perfectCamera.fovY();

			if (fovDiagonalPerfect <= fovX || fovDiagonalPerfect <= fovY)
			{
				OCEAN_SET_FAILED(validation);
			}

			if (fovDiagonalPerfect > fovX + fovY)
			{
				OCEAN_SET_FAILED(validation);
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
