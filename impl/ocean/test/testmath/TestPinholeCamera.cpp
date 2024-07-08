/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestPinholeCamera.h"

#include "ocean/base/DataType.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Random.h"
#include "ocean/math/SquareMatrix3.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestPinholeCamera::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "---   PinholeCamera test:   ---";
	Log::info() << " ";

	allSucceeded = testCameraConstructor<float>(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testCameraConstructor<double>(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatternCamera<float>(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatternCamera<double>(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testSubFrameCamera<float>(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testSubFrameCamera<double>(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testDistortion<float>(640u, 480u, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testDistortion<double>(640u, 480u, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testVectorDistortionFree<float>(640u, 480u, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testVectorDistortionFree<double>(640u, 480u, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testVectorDistorted<float>(640u, 480u, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testVectorDistorted<double>(640u, 480u, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testObjectPointInFront<float>(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testObjectPointInFront<double>(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testStandard2InvertedFlippedHomogenousMatrix4<float>(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testStandard2InvertedFlippedHomogenousMatrix4<double>(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testStandard2InvertedFlippedSquareMatrix3<float>(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testStandard2InvertedFlippedSquareMatrix3<double>(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testStandard2InvertedFlippedQuaternion<float>(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testStandard2InvertedFlippedQuaternion<double>(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() <<  "PinholeCamera test succeeded.";
	}
	else
	{
		Log::info() <<  "PinholeCamera test FAILED";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestPinholeCamera, CameraConstructorFloat)
{
	EXPECT_TRUE(TestPinholeCamera::testCameraConstructor<float>(GTEST_TEST_DURATION));
}

TEST(TestPinholeCamera, CameraConstructorDouble)
{
	EXPECT_TRUE(TestPinholeCamera::testCameraConstructor<double>(GTEST_TEST_DURATION));
}

TEST(TestPinholeCamera, PatternCameraFloat)
{
	EXPECT_TRUE(TestPinholeCamera::testPatternCamera<float>(GTEST_TEST_DURATION));
}

TEST(TestPinholeCamera, PatternCameraDouble)
{
	EXPECT_TRUE(TestPinholeCamera::testPatternCamera<double>(GTEST_TEST_DURATION));
}

TEST(TestPinholeCamera, SubFrameCameraFloat)
{
	EXPECT_TRUE(TestPinholeCamera::testSubFrameCamera<float>(GTEST_TEST_DURATION));
}

TEST(TestPinholeCamera, SubFrameCameraDouble)
{
	EXPECT_TRUE(TestPinholeCamera::testSubFrameCamera<double>(GTEST_TEST_DURATION));
}

TEST(TestPinholeCamera, DistortionFloat)
{
	EXPECT_TRUE(TestPinholeCamera::testDistortion<float>(640u, 480u, GTEST_TEST_DURATION));
}

TEST(TestPinholeCamera, DistortionDouble)
{
	EXPECT_TRUE(TestPinholeCamera::testDistortion<double>(640u, 480u, GTEST_TEST_DURATION));
}

TEST(TestPinholeCamera, VectorDistortionFreeFloat)
{
	EXPECT_TRUE(TestPinholeCamera::testVectorDistortionFree<float>(640u, 480u, GTEST_TEST_DURATION));
}

TEST(TestPinholeCamera, VectorDistortionFreeDouble)
{
	EXPECT_TRUE(TestPinholeCamera::testVectorDistortionFree<double>(640u, 480u, GTEST_TEST_DURATION));
}

TEST(TestPinholeCamera, VectorDistortedFloat)
{
	EXPECT_TRUE(TestPinholeCamera::testVectorDistorted<float>(640u, 480u, GTEST_TEST_DURATION));
}

TEST(TestPinholeCamera, VectorDistortedDouble)
{
	EXPECT_TRUE(TestPinholeCamera::testVectorDistorted<double>(640u, 480u, GTEST_TEST_DURATION));
}

TEST(TestPinholeCamera, ObjectPointInFrontFloat)
{
	EXPECT_TRUE(TestPinholeCamera::testObjectPointInFront<float>(GTEST_TEST_DURATION));
}

TEST(TestPinholeCamera, ObjectPointInFrontDouble)
{
	EXPECT_TRUE(TestPinholeCamera::testObjectPointInFront<double>(GTEST_TEST_DURATION));
}

TEST(TestPinholeCamera, Standard2InvertedFlippedHomogenousMatrix4Float)
{
	EXPECT_TRUE(TestPinholeCamera::testStandard2InvertedFlippedHomogenousMatrix4<float>(GTEST_TEST_DURATION));
}

TEST(TestPinholeCamera, Standard2InvertedFlippedHomogenousMatrix4Double)
{
	EXPECT_TRUE(TestPinholeCamera::testStandard2InvertedFlippedHomogenousMatrix4<double>(GTEST_TEST_DURATION));
}

TEST(TestPinholeCamera, Standard2InvertedFlippedSquareMatrix3Float)
{
	EXPECT_TRUE(TestPinholeCamera::testStandard2InvertedFlippedSquareMatrix3<float>(GTEST_TEST_DURATION));
}

TEST(TestPinholeCamera, Standard2InvertedFlippedSquareMatrix3Double)
{
	EXPECT_TRUE(TestPinholeCamera::testStandard2InvertedFlippedSquareMatrix3<double>(GTEST_TEST_DURATION));
}

TEST(TestPinholeCamera, Standard2InvertedFlippedQuaternionFloat)
{
	EXPECT_TRUE(TestPinholeCamera::testStandard2InvertedFlippedQuaternion<float>(GTEST_TEST_DURATION));
}

TEST(TestPinholeCamera, Standard2InvertedFlippedQuaternionDouble)
{
	EXPECT_TRUE(TestPinholeCamera::testStandard2InvertedFlippedQuaternion<double>(GTEST_TEST_DURATION));
}


#endif // OCEAN_USE_GTEST

template <typename T>
bool TestPinholeCamera::testCameraConstructor(const double testDuration)
{
	static_assert(std::is_same<T, float>::value || std::is_same<T, double>::value, "Template parameter T must be float or double.");

	ocean_assert(testDuration > 0.0);

	Log::info() <<  "PinholeCameraT<" << TypeNamer::name<T>() << "> constructor test:";

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	constexpr T eps = NumericT<T>::deg2rad(T(0.1));

	const Timestamp startTimestamp(true);

	do
	{
		{
			// testing constructor with width, height, a fovx

			const unsigned int width = RandomI::random(320u, 1920u);
			const unsigned int height = RandomI::random(240u, 1080u);

			const T fovX = RandomT<T>::scalar(NumericT<T>::deg2rad(30), NumericT<T>::deg2rad(70));

			const PinholeCameraT<T> pinholeCamera(width, height, fovX);

			if (pinholeCamera.width() == width && pinholeCamera.height() == height)
			{
				if (NumericT<T>::isEqual(pinholeCamera.fovX(), fovX, eps))
				{
					const PinholeCameraT<T> cameraPerfectPrincipal(width, height, fovX, T(width) * T(0.5), T(height) * T(0.5));

					if (NumericT<T>::isEqual(pinholeCamera.fovX(), cameraPerfectPrincipal.fovX(), NumericT<T>::deg2rad(T(0.1))))
					{
						validIterations++;
					}
				}
			}

			iterations++;
		}

		{
			// testing constructor with width, height, fovx, and principal point

			const unsigned int width = RandomI::random(320u, 1920u);
			const unsigned int height = RandomI::random(240u, 1080u);

			const T principalX = RandomT<T>::scalar(T(width) * T(-0.5), T(width) * T(1.5));
			const T principalY = RandomT<T>::scalar(T(height) * T(-0.5), T(height) * T(1.5));

			const T fovX = RandomT<T>::scalar(NumericT<T>::deg2rad(30), NumericT<T>::deg2rad(70));

			// first we check a pinhole camera with perfect principal point

			if (NumericT<T>::isEqual(fovX, PinholeCameraT<T>(width, height, fovX, T(width) * T(0.5), T(height) * T(0.5)).fovX(), eps))
			{
				validIterations++;
			}

			iterations++;

			const PinholeCameraT<T> pinholeCamera(width, height, fovX, principalX, principalY);

			if (pinholeCamera.width() == width && pinholeCamera.height() == height && pinholeCamera.principalPointX() == principalX && pinholeCamera.principalPointY() == principalY)
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
					if (NumericT<T>::isEqual(leftFovX, pinholeCamera.fovXLeft(), eps) && NumericT<T>::isEqual(rightFovX, pinholeCamera.fovXRight(), eps))
					{
						if (NumericT<T>::isEqual(topFovY, pinholeCamera.fovYTop(), eps) && NumericT<T>::isEqual(bottomFovY, pinholeCamera.fovYBottom(), eps))
						{
							if (leftFovX + rightFovX <= fovX + NumericT<T>::weakEps())
							{
								if (NumericT<T>::isEqual(pinholeCamera.fovX(), leftFovX + rightFovX, eps) && NumericT<T>::isEqual(pinholeCamera.fovY(), topFovY + bottomFovY, eps))
								{
									validIterations++;
								}
							}
						}
					}
				}
			}

			iterations++;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.999;
}

template <typename T>
bool TestPinholeCamera::testPatternCamera(const double testDuration)
{
	static_assert(std::is_same<T, float>::value || std::is_same<T, double>::value, "Template parameter T must be float or double.");

	ocean_assert(testDuration > 0.0);

	Log::info() << "Pattern pinhole camera test (" << TypeNamer::name<T>() << "):";

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(320u, 1920u);
		const unsigned int height = RandomI::random(240u, 1080u);

		const T principalX = RandomT<T>::scalar(T(-100), T(width + 100u));
		const T principalY = RandomT<T>::scalar(T(-100), T(height + 100u));

		const T fovX = RandomT<T>::scalar(NumericT<T>::deg2rad(30), NumericT<T>::deg2rad(70));

		const PinholeCameraT<T> patternCamera(width, height, fovX, principalX, principalY);

		const T factor = RandomT<T>::scalar(T(0.25), T(4.0));

		const unsigned int newWidth = (unsigned int)(T(width) * factor + T(0.5));
		const unsigned int newHeight = (unsigned int)(T(height) * factor + T(0.5));

		const PinholeCameraT<T> newCamera(newWidth, newHeight, patternCamera);

		// we need to ensure that the field of view is almost identical, and that the (normalized) principal point is almost identical

		const T patternFovX = patternCamera.fovX();
		const T patternFovY = patternCamera.fovY();

		const T newFovX = newCamera.fovX();
		const T newFovY = newCamera.fovY();

		if (NumericT<T>::isEqual(patternFovX, newFovX, NumericT<T>::deg2rad(5)) && NumericT<T>::isEqual(patternFovY, newFovY, NumericT<T>::deg2rad(5)))
		{
			const T patternPrincipalX = patternCamera.principalPointX() / T(patternCamera.width());
			const T patternPrincipalY = patternCamera.principalPointY() / T(patternCamera.height());

			const T newPrincipalX = newCamera.principalPointX() / T(newCamera.width());
			const T newPrincipalY = newCamera.principalPointY() / T(newCamera.height());

			if (NumericT<T>::isEqual(patternPrincipalX, newPrincipalX, T(0.05)) && NumericT<T>::isEqual(patternPrincipalY, newPrincipalY, T(0.05))) // 5%
			{
				validIterations++;
			}
		}

		iterations++;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.999;
}

template <typename T>
bool TestPinholeCamera::testSubFrameCamera(const double testDuration)
{
	static_assert(std::is_same<T, float>::value || std::is_same<T, double>::value, "Template parameter T must be float or double.");

	ocean_assert(testDuration > 0.0);

	Log::info() << "Sub-frame pinhole camera test (" << TypeNamer::name<T>() << "):";

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(320u, 1920u);
		const unsigned int height = RandomI::random(240u, 1080u);

		const T principalX = T(width) * T(0.5) + RandomT<T>::scalar(-100, 100);
		const T principalY = T(height) * T(0.5) + RandomT<T>::scalar(-100, 100);

		const T fovX = RandomT<T>::scalar(NumericT<T>::deg2rad(30), NumericT<T>::deg2rad(70));

		const PinholeCameraT<T> cameraFull(width, height, fovX, principalX, principalY);

		const unsigned int subFrameWidth = RandomI::random(50u, width);
		const unsigned int subFrameHeight = RandomI::random(50u, height);

		const T subFrameLeft = RandomT<T>::scalar(T(0u), T(width - subFrameWidth));
		const T subFrameTop = RandomT<T>::scalar(T(0u), T(height - subFrameHeight));

		const PinholeCameraT<T> cameraSubFrame(subFrameLeft, subFrameTop, subFrameWidth, subFrameHeight, cameraFull);

		const VectorT2<T> pointInSubFrame = RandomT<T>::vector2(T(0), T(cameraSubFrame.width()), T(0), T(cameraSubFrame.height()));

		const VectorT3<T> ray = cameraSubFrame.vector(pointInSubFrame);
		const VectorT2<T> pointInFullFrame = cameraFull.template projectToImage<false>(HomogenousMatrixT4<T>(true), ray, false);

		const T distance = pointInFullFrame.distance(pointInSubFrame + VectorT2<T>(subFrameLeft, subFrameTop));

		if (distance < T(0.1))
		{
			validIterations++;
		}

		iterations++;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.999;
}

template <typename T>
bool TestPinholeCamera::testDistortion(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(std::is_same<T, float>::value || std::is_same<T, double>::value, "Template parameter T must be float or double.");

	ocean_assert(testDuration > 0.0);

	Log::info() << "Distortion test (" << TypeNamer::name<T>() << "):";

	unsigned long long succeeded = 0ull;
	unsigned long long iterations = 0ull;

	const Timestamp startTimestamp(true);
	do
	{
		for (unsigned int n = 0u; n < 4u; ++n)
		{
			T k1 = (n % 2u == 0u) ? 0 : RandomT<T>::scalar(T(-0.1), T(0.1));
			T k2 = (n % 2u == 0u) ? 0 : RandomT<T>::scalar(T(-0.1), T(0.1));

			if (k1 * k2 > 0)
				continue;

			T p1 = (n <= 1) ? 0 : RandomT<T>::scalar(T(-0.01), T(0.01));
			T p2 = (n <= 1) ? 0 : RandomT<T>::scalar(T(-0.01), T(0.01));

			const PinholeCameraT<T> pinholeCamera(width, height, RandomT<T>::scalar(500, 600), RandomT<T>::scalar(500, 600),
								RandomT<T>::scalar(T(width) * T(0.5) - 50, T(width) * T(0.5) + 50),
								RandomT<T>::scalar(T(height) * T(0.5) - 50, T(height) * T(0.5) + 50),
								typename PinholeCameraT<T>::DistortionPair(k1, k2),
								typename PinholeCameraT<T>::DistortionPair(p1, p2));

			const VectorT2<T> undistortedPoint(RandomT<T>::scalar(0, T(width - 1)), RandomT<T>::scalar(0, T(height - 1)));

			const VectorT2<T> distortedPoint = pinholeCamera.template distort<true>(undistortedPoint);
			if (distortedPoint.x() > 0 && distortedPoint.x() <= T(width - 1) && distortedPoint.y() > 0 && distortedPoint.y() <= T(height - 1))
			{
				const VectorT2<T> calculatedUndistortedPoint = pinholeCamera.template undistort<true>(distortedPoint, 100u);

				if (!pinholeCamera.isInside(distortedPoint) || (NumericT<T>::isEqual(undistortedPoint.x(), calculatedUndistortedPoint.x(), T(0.1))
					&& NumericT<T>::isEqual(undistortedPoint.y(), calculatedUndistortedPoint.y(), T(0.1))))
					++succeeded;

				++iterations;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.97;
}

template <typename T>
bool TestPinholeCamera::testVectorDistortionFree(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(std::is_same<T, float>::value || std::is_same<T, double>::value, "Template parameter T must be float or double.");

	ocean_assert(testDuration > 0.0);

	Log::info() << "Vector test (" << TypeNamer::name<T>() << "):";

	unsigned long long iterations = 0ull;
	unsigned long long succeeded = 0ull;

	bool result = true;

	const Timestamp startTimestamp(true);
	do
	{
		const T principalPointX(RandomT<T>::scalar(T(width) * T(0.5) - T(30), T(width) * T(0.5) + T(30)));
		const T principalPointY(RandomT<T>::scalar(T(height) * T(0.5) - T(30), T(height) * T(0.5) + T(30)));

		ocean_assert(principalPointX > 0 && principalPointX < T(width));
		ocean_assert(principalPointY > 0 && principalPointY < T(height));

		const PinholeCameraT<T> pinholeCamera(width, height, NumericT<T>::deg2rad(T(55.1)), principalPointX, principalPointY);

		const VectorT2<T> imagePoint(RandomT<T>::scalar(0, T(width - 1)), RandomT<T>::scalar(0, T(height - 1)));
		const VectorT3<T> rayVector(pinholeCamera.vector(imagePoint));

		// the vector must have length 1
		if (NumericT<T>::isNotEqual(rayVector.length(), 1) || rayVector.z() > 0)
			result = false;

		// 3D -> 2D projection
		const VectorT3<T> rayVectorFlipped(rayVector.x(), -rayVector.y(), -rayVector.z());
		ocean_assert(rayVectorFlipped.isEqual(pinholeCamera.vectorIF(imagePoint), NumericT<T>::eps()));
		ocean_assert(rayVectorFlipped.isEqual(pinholeCamera.vectorIF(imagePoint, T(1)), NumericT<T>::eps()));

		const VectorT3<T> projectedHomogenousImagePoint(pinholeCamera.intrinsic() * rayVectorFlipped);
		ocean_assert(NumericT<T>::isNotEqualEps(projectedHomogenousImagePoint.z()));

		const VectorT2<T> projectedImagePoint(projectedHomogenousImagePoint.x() / projectedHomogenousImagePoint.z(), projectedHomogenousImagePoint.y() / projectedHomogenousImagePoint.z());

		if (NumericT<T>::abs(projectedImagePoint.x() - imagePoint.x()) <= 0.05 && NumericT<T>::abs(projectedImagePoint.y() - imagePoint.y()) <= 0.05)
			succeeded++;

		iterations++;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	if (result)
		Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";
	else
		Log::info() << "Validation: FAILED!";

	return result && percent >= 0.97;
}

template <typename T>
bool TestPinholeCamera::testVectorDistorted(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(std::is_same<T, float>::value || std::is_same<T, double>::value, "Template parameter T must be float or double.");

	ocean_assert(testDuration > 0.0);

	Log::info() << "Distorted vector test (" << TypeNamer::name<T>() << "):";

	unsigned long long iterations = 0ull;
	unsigned long long succeeded = 0ull;

	bool result = true;

	const Timestamp startTimestamp(true);

	do
	{
		const T principalPointX(RandomT<T>::scalar(T(width) * T(0.5) - T(30), T(width) * T(0.5) + T(30)));
		const T principalPointY(RandomT<T>::scalar(T(height) * T(0.5) - T(30), T(height) * T(0.5) + T(30)));

		ocean_assert_and_suppress_unused(principalPointX > 0 && principalPointX < T(width), principalPointX);
		ocean_assert_and_suppress_unused(principalPointY > 0 && principalPointY < T(height), principalPointY);

		PinholeCameraT<T> pinholeCamera(width, height, NumericT<T>::deg2rad(T(55.1)));
		pinholeCamera.setRadialDistortion(typename PinholeCameraT<T>::DistortionPair(RandomT<T>::scalar(T(-0.1), T(0.1)), RandomT<T>::scalar(T(-0.1), T(0.1))));
		pinholeCamera.setTangentialDistortion(typename PinholeCameraT<T>::DistortionPair(RandomT<T>::scalar(T(-0.05), T(0.05)), RandomT<T>::scalar(T(-0.05), T(0.05))));

		const VectorT2<T> distortedImagePoint(RandomT<T>::scalar(0, T(width - 1)), RandomT<T>::scalar(0, T(height - 1)));
		const VectorT2<T> undistortedImagePoint(pinholeCamera.template undistort<true>(distortedImagePoint));

		const VectorT3<T> rayVector(pinholeCamera.vector(undistortedImagePoint));
		bool localResult = true;

		// the vector must have length 1
		if (NumericT<T>::isNotEqual(rayVector.length(), 1) || rayVector.z() > 0)
			localResult = false;

		// 3D -> 2D projection
		const VectorT3<T> rayVectorFlipped(rayVector.x(), -rayVector.y(), -rayVector.z());

		const VectorT3<T> undistortedProjectedHomogenousImagePoint(pinholeCamera.intrinsic() * rayVectorFlipped);
		ocean_assert(NumericT<T>::isNotEqualEps(undistortedProjectedHomogenousImagePoint.z()));

		const VectorT2<T> undistortedProjectedImagePoint(undistortedProjectedHomogenousImagePoint.x() / undistortedProjectedHomogenousImagePoint.z(), undistortedProjectedHomogenousImagePoint.y() / undistortedProjectedHomogenousImagePoint.z());

		if (NumericT<T>::abs(undistortedProjectedImagePoint.x() - undistortedImagePoint.x()) > 0.05 || NumericT<T>::abs(undistortedProjectedImagePoint.y() - undistortedImagePoint.y()) > 0.05)
			localResult = false;

		const VectorT2<T> distortedProjectedImagePoint(pinholeCamera.template distort<true>(undistortedProjectedImagePoint));

		if (NumericT<T>::abs(distortedProjectedImagePoint.x() - distortedImagePoint.x()) > 0.05 || NumericT<T>::abs(distortedProjectedImagePoint.y() - distortedImagePoint.y()) > 0.05)
			localResult = false;

		if (localResult)
			succeeded++;

		iterations++;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	if (result)
		Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";
	else
		Log::info() << "Validation: FAILED!";

	return result && percent >= 0.97;
}

template <typename T>
bool TestPinholeCamera::testObjectPointInFront(const double testDuration)
{
	static_assert(std::is_same<T, float>::value || std::is_same<T, double>::value, "Template parameter T must be float or double.");

	ocean_assert(testDuration > 0.0);

	Log::info() << "Object Point in front test (" << TypeNamer::name<T>() << "):";

	unsigned long long iterations = 0ull;
	unsigned long long validIteration = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			{
				// pose with default orientation

				const SquareMatrixT3<T> orientation(true);
				const HomogenousMatrixT4<T> pose(VectorT3<T>(RandomT<T>::scalar(-10, 10), RandomT<T>::scalar(-10, -10), 0));

				const SquareMatrixT3<T> orientationIF(PinholeCameraT<T>::standard2InvertedFlipped(orientation));
				const HomogenousMatrixT4<T> poseIF(PinholeCameraT<T>::standard2InvertedFlipped(pose));

				const VectorT3<T> frontObjectPoint(RandomT<T>::scalar(-100, 100), RandomT<T>::scalar(-100, 100), RandomT<T>::scalar(-100, -NumericT<T>::eps() * T(100)));
				const VectorT3<T> backObjectPoint(RandomT<T>::scalar(-100, 100), RandomT<T>::scalar(-100, 100), RandomT<T>::scalar(0, 100));

				if (PinholeCameraT<T>::isObjectPointInFrontIF(poseIF, frontObjectPoint) && PinholeCameraT<T>::isObjectPointInFrontIF(orientationIF, frontObjectPoint) && !PinholeCameraT<T>::isObjectPointInFrontIF(poseIF, backObjectPoint) && !PinholeCameraT<T>::isObjectPointInFrontIF(orientationIF, backObjectPoint))
					validIteration++;

				iterations++;
			}

			{
				// arbitrary pose

				const SquareMatrixT3<T> orientation(RandomT<T>::quaternion());
				const HomogenousMatrixT4<T> pose(RandomT<T>::vector3(-10, 10), orientation);

				const HomogenousMatrixT4<T> poseIF(PinholeCameraT<T>::standard2InvertedFlipped(pose));
				const SquareMatrixT3<T> orientationIF(PinholeCameraT<T>::standard2InvertedFlipped(orientation));

				const VectorT3<T> localFrontObjectPoint(RandomT<T>::scalar(-100, 100), RandomT<T>::scalar(-100, 100), RandomT<T>::scalar(-100, -NumericT<T>::eps() * T(100)));
				const VectorT3<T> localBackObjectPoint(RandomT<T>::scalar(-100, 100), RandomT<T>::scalar(-100, 100), RandomT<T>::scalar(0, 100));

				const VectorT3<T> orientationFrontObjectPoint(orientation * localFrontObjectPoint);
				const VectorT3<T> orientationBackObjectPoint(orientation * localBackObjectPoint);

				const VectorT3<T> poseFrontObjectPoint(pose * localFrontObjectPoint);
				const VectorT3<T> poseBackObjectPoint(pose * localBackObjectPoint);

				if (PinholeCameraT<T>::isObjectPointInFrontIF(poseIF, poseFrontObjectPoint) && PinholeCameraT<T>::isObjectPointInFrontIF(orientationIF, orientationFrontObjectPoint) && !PinholeCameraT<T>::isObjectPointInFrontIF(poseIF, poseBackObjectPoint) && !PinholeCameraT<T>::isObjectPointInFrontIF(orientationIF, orientationBackObjectPoint))
					validIteration++;

				iterations++;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIteration) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.999;
}

template <typename T>
bool TestPinholeCamera::testStandard2InvertedFlippedHomogenousMatrix4(const double testDuration)
{
	static_assert(std::is_same<T, float>::value || std::is_same<T, double>::value, "Template parameter T must be float or double.");

	ocean_assert(testDuration > 0.0);

	Log::info() << "Standard2InvertedFlipped of HomogenousMatrixT4<" << TypeNamer::name<T>() << "> test:";

	unsigned long long iterations = 0ull;
	unsigned long long validIteration = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		const VectorT3<T> translation(RandomT<T>::vector3(-10, 10));
		const VectorT3<T> scale(RandomT<T>::vector3(T(0.5), T(2.0)));
		const QuaternionT<T> quaternion(RandomT<T>::quaternion());

		{
			const HomogenousMatrixT4<T> wTc(translation, quaternion, scale);
			ocean_assert(wTc.isValid());

			const HomogenousMatrixT4<T> fTw = PinholeCameraT<T>::standard2InvertedFlipped(wTc);
			ocean_assert(fTw.isValid());

			const HomogenousMatrixT4<T> wTf = wTc * HomogenousMatrixT4<T>(RotationT<T>(1, 0, 0, NumericT<T>::pi()));

			HomogenousMatrixT4<T> test_fTw;
			if (wTf.invert(test_fTw))
			{
				for (unsigned int n = 0u; n < 5u; ++n)
				{
					const VectorT3<T> testVector = RandomT<T>::vector3(-1, 1);

					if ((fTw * testVector).isEqual(test_fTw * testVector, NumericT<T>::weakEps()))
					{
						validIteration++;
					}
				}
			}

			iterations += 5ull;

			const HomogenousMatrixT4<T> test_wTc = PinholeCameraT<T>::invertedFlipped2Standard(fTw);

			for (unsigned int n = 0u; n < 5u; ++n)
			{
				const VectorT3<T> testVector = RandomT<T>::vector3(-1, 1);

				if ((wTc * testVector).isEqual(test_wTc * testVector, NumericT<T>::weakEps()))
				{
					validIteration++;
				}
			}

			iterations += 5ull;
		}

		{
			const HomogenousMatrixT4<T> wTc(translation, quaternion);
			ocean_assert(wTc.isValid());

			const HomogenousMatrixT4<T> fTw = PinholeCameraT<T>::standard2InvertedFlipped(wTc);
			ocean_assert(fTw.isValid());

			const unsigned int width = RandomI::random(1u, 1920u);
			const unsigned int height = RandomI::random(1u, 1080u);
			const T fovX = RandomT<T>::scalar(NumericT<T>::deg2rad(25), NumericT<T>::deg2rad(120));

			const PinholeCameraT<T> pinholeCamera(width, height, fovX);

			const VectorT2<T> observation = RandomT<T>::vector2(0, T(width), 0, T(height));

			const LineT3<T> ray = pinholeCamera.ray(observation, wTc);
			const VectorT3<T> position = ray.point(RandomT<T>::scalar(T(0.1), T(2)));

			if (pinholeCamera.template projectToImageIF<false, false>(fTw, position).isEqual(observation, 2))
			{
				validIteration++;
			}

			iterations++;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIteration) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.999;
}

template <typename T>
bool TestPinholeCamera::testStandard2InvertedFlippedSquareMatrix3(const double testDuration)
{
	static_assert(std::is_same<T, float>::value || std::is_same<T, double>::value, "Template parameter T must be float or double.");

	ocean_assert(testDuration > 0.0);

	Log::info() << "Standard2InvertedFlipped of SquareMatrixT3<" << TypeNamer::name<T>() << "> test:";

	unsigned long long iterations = 0ull;
	unsigned long long validIteration = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		const SquareMatrixT3<T> wTc(RandomT<T>::rotation());
		ocean_assert(!wTc.isSingular());

		const SquareMatrixT3<T> fTw = PinholeCameraT<T>::standard2InvertedFlipped(wTc);
		ocean_assert(!fTw.isSingular());

		{
			const SquareMatrixT3<T> wTf = wTc * SquareMatrixT3<T>(RotationT<T>(1, 0, 0, NumericT<T>::pi()));

			SquareMatrixT3<T> test_fTw;
			if (wTf.invert(test_fTw))
			{
				for (unsigned int n = 0u; n < 5u; ++n)
				{
					const VectorT3<T> testVector = RandomT<T>::vector3(-1, 1);

					if ((fTw * testVector).isEqual(test_fTw * testVector, NumericT<T>::weakEps()))
					{
						validIteration++;
					}
				}
			}

			iterations += 5ull;
		}

		{
			const SquareMatrixT3<T> test_wTc = PinholeCameraT<T>::invertedFlipped2Standard(fTw);

			for (unsigned int n = 0u; n < 5u; ++n)
			{
				const VectorT3<T> testVector = RandomT<T>::vector3(-1, 1);

				if ((wTc * testVector).isEqual(test_wTc * testVector, NumericT<T>::weakEps()))
				{
					validIteration++;
				}
			}

			iterations += 5ull;
		}

		{
			const unsigned int width = RandomI::random(1u, 1920u);
			const unsigned int height = RandomI::random(1u, 1080u);
			const T fovX = RandomT<T>::scalar(NumericT<T>::deg2rad(25), NumericT<T>::deg2rad(120));

			const PinholeCameraT<T> pinholeCamera(width, height, fovX);

			const VectorT2<T> observation = RandomT<T>::vector2(0, T(width), 0, T(height));

			const LineT3<T> ray = pinholeCamera.ray(observation, HomogenousMatrixT4<T>(wTc));
			const VectorT3<T> position = ray.point(RandomT<T>::scalar(T(0.1), T(2)));

			if (pinholeCamera.template projectToImageIF<false, false>(HomogenousMatrixT4<T>(fTw), position).isEqual(observation, 2))
			{
				validIteration++;
			}

			iterations++;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIteration) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.999;
}

template <typename T>
bool TestPinholeCamera::testStandard2InvertedFlippedQuaternion(const double testDuration)
{
	static_assert(std::is_same<T, float>::value || std::is_same<T, double>::value, "Template parameter T must be float or double.");

	ocean_assert(testDuration > 0.0);

	Log::info() << "Standard2InvertedFlipped of QuaternionT<" << TypeNamer::name<T>() << "> test:";

	unsigned long long iterations = 0ull;
	unsigned long long validIteration = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		const QuaternionT<T> wTc(RandomT<T>::quaternion());
		ocean_assert(wTc.isValid());

		const QuaternionT<T> fTw = PinholeCameraT<T>::standard2InvertedFlipped(wTc);
		ocean_assert(fTw.isValid());

		{
			const QuaternionT<T> wTf = wTc * QuaternionT<T>(RotationT<T>(1, 0, 0, NumericT<T>::pi()));

			QuaternionT<T> test_fTw;
			if (wTf.invert(test_fTw))
			{
				for (unsigned int n = 0u; n < 5u; ++n)
				{
					const VectorT3<T> testVector = RandomT<T>::vector3(-1, 1);

					if ((fTw * testVector).isEqual(test_fTw * testVector, NumericT<T>::weakEps()))
					{
						validIteration++;
					}
				}
			}

			iterations += 5ull;
		}

		{
			const QuaternionT<T> test_wTc = PinholeCameraT<T>::invertedFlipped2Standard(fTw);

			for (unsigned int n = 0u; n < 5u; ++n)
			{
				const VectorT3<T> testVector = RandomT<T>::vector3(-1, 1);

				if ((wTc * testVector).isEqual(test_wTc * testVector, NumericT<T>::weakEps()))
				{
					validIteration++;
				}
			}

			iterations += 5ull;
		}

		{
			const unsigned int width = RandomI::random(1u, 1920u);
			const unsigned int height = RandomI::random(1u, 1080u);
			const T fovX = RandomT<T>::scalar(NumericT<T>::deg2rad(25), NumericT<T>::deg2rad(120));

			const PinholeCameraT<T> pinholeCamera(width, height, fovX);

			const VectorT2<T> observation = RandomT<T>::vector2(0, T(width), 0, T(height));

			const LineT3<T> ray = pinholeCamera.ray(observation, HomogenousMatrixT4<T>(wTc));
			const VectorT3<T> position = ray.point(RandomT<T>::scalar(T(0.1), T(2)));

			if (pinholeCamera.template projectToImageIF<false, false>(HomogenousMatrixT4<T>(fTw), position).isEqual(observation, 2))
			{
				validIteration++;
			}

			iterations++;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIteration) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.999;
}

}

}

}
