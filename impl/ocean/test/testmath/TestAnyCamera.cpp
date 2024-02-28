// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/test/testmath/TestAnyCamera.h"

#include "ocean/base/Timestamp.h"
#include "ocean/base/RandomGenerator.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/FisheyeCamera.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestAnyCamera::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "---   AnyCamera test:   ---";
	Log::info() << " ";

	allSucceeded = testConstructor<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testConstructor<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPrincipalPoint<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testPrincipalPoint<double>(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "AnyCamera test succeeded.";
	}
	else
	{
		Log::info() << "AnyCamera test FAILED";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestAnyCamera, Constructor_Float)
{
	EXPECT_TRUE((TestAnyCamera::testConstructor<float>(GTEST_TEST_DURATION)));
}

TEST(TestAnyCamera, Constructor_Double)
{
	EXPECT_TRUE((TestAnyCamera::testConstructor<double>(GTEST_TEST_DURATION)));
}


TEST(TestAnyCamera, PrincipalPoint_Float)
{
	EXPECT_TRUE((TestAnyCamera::testPrincipalPoint<float>(GTEST_TEST_DURATION)));
}

TEST(TestAnyCamera, PrincipalPoint_Double)
{
	EXPECT_TRUE((TestAnyCamera::testPrincipalPoint<double>(GTEST_TEST_DURATION)));
}

#endif // OCEAN_USE_GTEST

template <typename T>
bool TestAnyCamera::testConstructor(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "AnyCamera constructor test with " << TypeNamer::name<T>() << ":";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	{
		// testing PinholeCamera

		const unsigned int width = RandomI::random(randomGenerator, 640u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 640u, 1080u);

		const T fovX = RandomT<T>::scalar(randomGenerator, NumericT<T>::deg2rad(35), NumericT<T>::deg2rad(70));

		const T principalPointX = RandomT<T>::scalar(randomGenerator, T(1), T(width - 2u));
		const T principalPointY = RandomT<T>::scalar(randomGenerator, T(1), T(height - 2u));

		PinholeCameraT<T> pinholeCamera(width, height, fovX, principalPointX, principalPointY);
		pinholeCamera.setRadialDistortion(typename PinholeCameraT<T>::DistortionPair(RandomT<T>::scalar(randomGenerator, T(-0.05), T(0.05)), RandomT<T>::scalar(randomGenerator, T(-0.05), T(0.05))));
		pinholeCamera.setTangentialDistortion(typename PinholeCameraT<T>::DistortionPair(RandomT<T>::scalar(randomGenerator, T(-0.01), T(0.01)), RandomT<T>::scalar(randomGenerator, T(-0.01), T(0.01))));

		const AnyCameraPinholeT<T> anyCamera(pinholeCamera);

		if (!verifyAnyCamera(anyCamera, testDuration, &randomGenerator))
		{
			allSucceeded = false;
		}

		const typename AnyCameraPinholeT<T>::WrappedCamera& wrappedCamera = dynamic_cast<const typename AnyCameraPinholeT<T>::WrappedCamera&>(anyCamera);

		const PinholeCameraT<T>& actualCamera = wrappedCamera.actualCamera();

		if (actualCamera.width() != width || actualCamera.height() != height)
		{
			allSucceeded = false;
		}

		if (NumericT<T>::isNotEqual(actualCamera.principalPointX(), principalPointX) || NumericT<T>::isNotEqual(actualCamera.principalPointY(), principalPointY))
		{
			allSucceeded = false;
		}

		if (NumericT<T>::isNotEqual(actualCamera.fovX(), anyCamera.fovX(), T(0.01)))
		{
			allSucceeded = false;
		}

		if (anyCamera.isEqual(AnyCameraPinholeT<T>(PinholeCameraT<T>(width + RandomI::random(randomGenerator, 1u, 10u), height, fovX, principalPointX, principalPointY)))
				|| anyCamera.isEqual(AnyCameraPinholeT<T>(PinholeCameraT<T>(width, height + RandomI::random(randomGenerator, 1u, 10u), fovX, principalPointX, principalPointY)))
				|| anyCamera.isEqual(AnyCameraPinholeT<T>(PinholeCameraT<T>(width, height, std::max(T(0.01), fovX + RandomT<T>::scalar(randomGenerator, T(0.01), T(1)) * RandomT<T>::sign(randomGenerator)), principalPointX, principalPointY)))
				|| anyCamera.isEqual(AnyCameraPinholeT<T>(PinholeCameraT<T>(width, height, fovX, principalPointX + RandomT<T>::scalar(randomGenerator, T(0.01), T(1)) * RandomT<T>::sign(randomGenerator), principalPointY)))
				|| anyCamera.isEqual(AnyCameraPinholeT<T>(PinholeCameraT<T>(width, height, fovX, principalPointX, principalPointY + RandomT<T>::scalar(randomGenerator, T(0.01), T(1)) * RandomT<T>::sign(randomGenerator)))))
		{
			allSucceeded = false;
		}
	}

	{
		const unsigned int width = RandomI::random(randomGenerator, 640u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 640u, 1080u);

		const T fovX = RandomT<T>::scalar(randomGenerator, NumericT<T>::deg2rad(35), NumericT<T>::deg2rad(70));

		const AnyCameraFisheyeT<T> anyCamera(FisheyeCameraT<T>(width, height, fovX));

		if (!verifyAnyCamera(anyCamera, testDuration, &randomGenerator))
		{
			allSucceeded = false;
		}

		const typename AnyCameraFisheyeT<T>::WrappedCamera& wrappedCamera = dynamic_cast<const typename AnyCameraFisheyeT<T>::WrappedCamera&>(anyCamera);

		const FisheyeCameraT<T>& actualCamera = wrappedCamera.actualCamera();

		if (anyCamera.width() != width)
		{
			allSucceeded = false;
		}

		if (anyCamera.height() != height)
		{
			allSucceeded = false;
		}

		if (NumericT<T>::isNotEqual(actualCamera.fovX(), anyCamera.fovX(), T(0.01)))
		{
			allSucceeded = false;
		}

		const unsigned int differentWidth = width + RandomI::random(randomGenerator, 1u, 10u);

		if (anyCamera.isEqual(AnyCameraFisheyeT<T>(FisheyeCameraT<T>(differentWidth, height, fovX))))
		{
			allSucceeded = false;
		}

		const unsigned int differentHeight = height + RandomI::random(randomGenerator, 1u, 10u);

		if (anyCamera.isEqual(AnyCameraFisheyeT<T>(FisheyeCameraT<T>(width, differentHeight, fovX))))
		{
			allSucceeded = false;
		}

		const T differentFovX = fovX + RandomT<T>::scalar(randomGenerator, NumericT<T>::deg2rad(1), NumericT<T>::deg2rad(30)) * RandomT<T>::sign(randomGenerator);

		if (anyCamera.isEqual(AnyCameraFisheyeT<T>(FisheyeCameraT<T>(width, height, differentFovX))))
		{
			allSucceeded = false;
		}
	}

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

template <typename T>
bool TestAnyCamera::testPrincipalPoint(const double testDuration)
{
	Log::info() << "Principal point test with " << TypeNamer::name<T>() << ":";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 500u, 1000u);
		const unsigned int height = RandomI::random(randomGenerator, 500u, 1000u);

		const T fovX = RandomT<T>::scalar(randomGenerator, NumericT<T>::deg2rad(30), NumericT<T>::deg2rad(90));

		const AnyCameraPinholeT<T> perfectCamera(PinholeCameraT<T>(width, height, fovX));

		const T principalPointOffsetX = RandomT<T>::scalar(randomGenerator, T(-50), T(50));
		const T principalPointOffsetY = RandomT<T>::scalar(randomGenerator, T(-50), T(50));

		const AnyCameraPinholeT<T> offsetCamera(PinholeCameraT<T>(width, height, fovX, T(width) * T(0.5) + principalPointOffsetX, T(height) * T(0.5) + principalPointOffsetY));

		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const VectorT2<T> perfectImagePoint = RandomT<T>::vector2(randomGenerator, T(0), T(width), T(0), T(height));

			const VectorT3<T> objectPoint = perfectCamera.ray(perfectImagePoint).point(RandomT<T>::scalar(randomGenerator, T(0.1), T(10)));

			// any offset to the perfect/ideal principal point will simply shift the entire image

			const VectorT2<T> offsetImagePoint = offsetCamera.projectToImage(objectPoint);

			const VectorT2<T> offset = offsetImagePoint - perfectImagePoint;

			if (NumericT<T>::isNotEqual(offset.x(), principalPointOffsetX, T(0.01)))
			{
				allSucceeded = false;
			}

			if (NumericT<T>::isNotEqual(offset.y(), principalPointOffsetY, T(0.01)))
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

template <typename T>
bool TestAnyCamera::verifyAnyCamera(const AnyCameraT<T>& anyCamera, const double testDuration, RandomGenerator* optionalRandomGenerator)
{
	if (!anyCamera.isValid())
	{
		return false;
	}

	if (anyCamera.anyCameraType() == AnyCameraType::INVALID)
	{
		return false;
	}

	if (anyCamera.name().empty())
	{
		return false;
	}

	if (anyCamera.width() == 0u || anyCamera.height() == 0u)
	{
		return false;
	}

	if (anyCamera.fovX() <= NumericT<T>::eps() || anyCamera.fovX() > NumericT<T>::pi2())
	{
		return false;
	}

	if (anyCamera.fovY() <= NumericT<T>::eps() || anyCamera.fovY() > NumericT<T>::pi2())
	{
		return false;
	}

	if (anyCamera.focalLengthX() <= NumericT<T>::eps())
	{
		return false;
	}

	if (anyCamera.focalLengthY() <= NumericT<T>::eps())
	{
		return false;
	}

	if (NumericT<T>::isNotEqual(anyCamera.inverseFocalLengthX(), T(1) / anyCamera.focalLengthX()))
	{
		return false;
	}

	if (NumericT<T>::isNotEqual(anyCamera.inverseFocalLengthY(), T(1) / anyCamera.focalLengthY()))
	{
		return false;
	}

	if (!anyCamera.isInside(anyCamera.principalPoint()))
	{
		return false;
	}

	if (!anyCamera.isEqual(anyCamera))
	{
		return false;
	}

	{
		std::unique_ptr<AnyCameraT<T>> anyCameraCloneT = anyCamera.clone();
		if (!anyCameraCloneT || !anyCameraCloneT->isValid() || anyCameraCloneT->width() != anyCamera.width() || anyCameraCloneT->height() != anyCamera.height())
		{
			return false;
		}

		// now we test the convert() function

		std::shared_ptr<AnyCameraT<T>> sharedCameraCloneT(std::move(anyCameraCloneT));

		if (!anyCamera.isEqual(*AnyCameraT<T>::convert(sharedCameraCloneT), T(NumericF::weakEps() * T(10))))
		{
			return false;
		}
	}

	{
		std::unique_ptr<AnyCameraF> anyCameraCloneF = anyCamera.cloneToFloat();
		if (!anyCameraCloneF || !anyCameraCloneF->isValid() || anyCameraCloneF->width() != anyCamera.width() || anyCameraCloneF->height() != anyCamera.height())
		{
			return false;
		}

		if (std::is_same<T, float>::value || anyCamera.name().find("Perception") == std::string::npos) // perception camera models cannot be cloned from float to double
		{
			// now we test the convert() function

			SharedAnyCameraF sharedCameraCloneF(std::move(anyCameraCloneF));

			if (!anyCamera.isEqual(*AnyCameraT<T>::convert(sharedCameraCloneF), T(NumericF::weakEps() * T(10))))
			{
				return false;
			}
		}
	}

	if (std::is_same<T, double>::value || anyCamera.name().find("Perception") == std::string::npos) // perception camera models cannot be cloned from float to double
	{
		std::unique_ptr<AnyCameraD> anyCameraCloneD = anyCamera.cloneToDouble();
		if (!anyCameraCloneD || !anyCameraCloneD->isValid() || anyCameraCloneD->width() != anyCamera.width() || anyCameraCloneD->height() != anyCamera.height())
		{
			return false;
		}

		// now we test the convert() function

		SharedAnyCameraD sharedCameraCloneD(std::move(anyCameraCloneD));

		if (!anyCamera.isEqual(*AnyCameraT<T>::convert(sharedCameraCloneD), T(NumericF::weakEps() * T(10))))
		{
			return false;
		}
	}

	RandomGenerator randomGenerator(optionalRandomGenerator);

	const std::vector<T> jacobianEpsilons = {NumericT<T>::weakEps(), NumericT<T>::weakEps() / T(10), NumericT<T>::weakEps() * T(10), NumericT<T>::weakEps() / T(100), NumericT<T>::weakEps() * T(100)};

	uint64_t succeeded = 0u;
	uint64_t iterations = 0u;

	const Timestamp startTimestamp(true);
	do
	{
		const VectorT2<T> distortedImagePoint(RandomT<T>::vector2(randomGenerator, T(anyCamera.width()) * T(0.05), T(anyCamera.width()) * T(0.95), T(anyCamera.height()) * T(0.05), T(anyCamera.height()) * T(0.95)));

		if (!anyCamera.isInside(distortedImagePoint))
		{
			return false;
		}

		for (const bool makeUnitVector : {false, true})
		{
			{
				// un-projecting and re-projecting a distorted image point with default camera pose

				const VectorT3<T> unprojectedImagePoint = anyCamera.vector(distortedImagePoint, makeUnitVector);

				if (makeUnitVector)
				{
					if (!unprojectedImagePoint.isUnit())
					{
						return false;
					}
				}
				else
				{
					if (NumericT<T>::isEqualEps(unprojectedImagePoint.z()))
					{
						return false;
					}
				}

				const VectorT2<T> reprojectedImagePoint = anyCamera.projectToImage(unprojectedImagePoint);

				if (reprojectedImagePoint.distance(distortedImagePoint) >= T(1))
				{
					return false;
				}
			}

			{
				// un-projecting and re-projecting a distorted image point with default camera pose

				const VectorT3<T> unprojectedImagePointIF = anyCamera.vectorIF(distortedImagePoint, makeUnitVector);

				if (makeUnitVector)
				{
					if (!unprojectedImagePointIF.isUnit())
					{
						return false;
					}
				}
				else
				{
					if (NumericT<T>::isEqualEps(unprojectedImagePointIF.z()))
					{
						return false;
					}
				}

				const VectorT2<T> reprojectedImagePoint = anyCamera.projectToImageIF(unprojectedImagePointIF);

				if (reprojectedImagePoint.distance(distortedImagePoint) >= T(1))
				{
					return false;
				}
			}

			{
				// un-projecting and re-projecting a distorted image points with default camera pose

				const unsigned int size = RandomI::random(randomGenerator, 1u, 100u);

				VectorsT2<T> distortedImagePoints(size);

				for (VectorT2<T>& point : distortedImagePoints)
				{
					point = RandomT<T>::vector2(randomGenerator, T(anyCamera.width()) * T(0.05), T(anyCamera.width()) * T(0.95), T(anyCamera.height()) * T(0.05), T(anyCamera.height()) * T(0.95));
				}

				VectorsT3<T> unprojectedImagePoints(size);
				anyCamera.vector(distortedImagePoints.data(), distortedImagePoints.size(), unprojectedImagePoints.data(), makeUnitVector);

				VectorsT2<T> reprojectedImagePoints(size);
				anyCamera.projectToImage(unprojectedImagePoints.data(), unprojectedImagePoints.size(), reprojectedImagePoints.data());

				for (size_t n = 0; n < size; ++n)
				{
					const VectorT3<T>& unprojectedImagePoint = unprojectedImagePoints[n];

					if (makeUnitVector)
					{
						if (!unprojectedImagePoint.isUnit())
						{
							return false;
						}
					}
					else
					{
						if (NumericT<T>::isEqualEps(unprojectedImagePoint.z()))
						{
							return false;
						}
					}

					if (reprojectedImagePoints[n].distance(distortedImagePoints[n]) >= T(1))
					{
						return false;
					}
				}
			}

			{
				// un-projecting and re-projecting a distorted image points with default camera pose

				const unsigned int size = RandomI::random(randomGenerator, 1u, 100u);

				VectorsT2<T> distortedImagePoints(size);

				for (VectorT2<T>& point : distortedImagePoints)
				{
					point = RandomT<T>::vector2(randomGenerator, T(anyCamera.width()) * T(0.05), T(anyCamera.width()) * T(0.95), T(anyCamera.height()) * T(0.05), T(anyCamera.height()) * T(0.95));
				}

				VectorsT3<T> unprojectedImagePointsIF(size);
				anyCamera.vectorIF(distortedImagePoints.data(), distortedImagePoints.size(), unprojectedImagePointsIF.data(), makeUnitVector);

				VectorsT2<T> reprojectedImagePoints(size);
				anyCamera.projectToImageIF(unprojectedImagePointsIF.data(), unprojectedImagePointsIF.size(), reprojectedImagePoints.data());

				for (size_t n = 0; n < size; ++n)
				{
					const VectorT3<T>& unprojectedImagePointIF = unprojectedImagePointsIF[n];

					if (makeUnitVector)
					{
						if (!unprojectedImagePointIF.isUnit())
						{
							return false;
						}
					}
					else
					{
						if (NumericT<T>::isEqualEps(unprojectedImagePointIF.z()))
						{
							return false;
						}
					}

					if (reprojectedImagePoints[n].distance(distortedImagePoints[n]) >= T(1))
					{
						return false;
					}
				}
			}

			{
				// un-projecting and re-projecting a distorted image point with random camera pose

				const HomogenousMatrixT4<T> world_T_camera(RandomT<T>::vector3(randomGenerator) * RandomT<T>::scalar(randomGenerator, -10, 10), RandomT<T>::quaternion(randomGenerator));

				const LineT3<T> ray = anyCamera.ray(distortedImagePoint, world_T_camera);

				if (!ray.isValid())
				{
					return false;
				}

				if (ray.point() != world_T_camera.translation())
				{
					return false;
				}

				const VectorT3<T> randomPointAlongRay = ray.point(RandomT<T>::scalar(randomGenerator, T(0.1), T(10)));

				const VectorT2<T> reprojectedImagePoint = anyCamera.projectToImage(world_T_camera, randomPointAlongRay);

				if (reprojectedImagePoint.distance(distortedImagePoint) >= T(1))
				{
					return false;
				}
			}
		}

		{
			// testing resize

			std::unique_ptr<AnyCameraT<T>> biggerClone = anyCamera.clone(anyCamera.width() * 2u, anyCamera.height() * 2u);
			if (!biggerClone)
			{
				return false;
			}

			if (biggerClone->width() != anyCamera.width() * 2u || biggerClone->height() != anyCamera.height() * 2u)
			{
				return false;
			}

			std::unique_ptr<AnyCameraT<T>> smallerClone = anyCamera.clone(anyCamera.width() / 2u, anyCamera.height() / 2u);
			if (!smallerClone)
			{
				return false;
			}

			if (smallerClone->width() != anyCamera.width() / 2u || smallerClone->height() != anyCamera.height() / 2u)
			{
				return false;
			}

			const VectorT3<T> point = anyCamera.ray(distortedImagePoint).point(10); // 10 meter in front of camera

			const VectorT3<T> biggerPoint = biggerClone->ray(distortedImagePoint * T(2)).point(10);

			if (point.distance(biggerPoint) > T(0.05)) // 5cm
			{
				return false;
			}

			const VectorT3<T> smallerPoint = smallerClone->ray(distortedImagePoint * T(0.5)).point(10);

			if (point.distance(smallerPoint) > T(0.05))
			{
				return false;
			}
		}

		{
			// verifying point jacobian

			const VectorT3<T> objectPointIF = anyCamera.vectorIF(distortedImagePoint) * T(100);

			const VectorT2<T> reprojectedImagePoint = anyCamera.projectToImageIF(objectPointIF);

			T jacobianX[3];
			T jacobianY[3];
			anyCamera.pointJacobian2x3IF(objectPointIF, jacobianX, jacobianY);

			bool accurate = true;

			for (unsigned int n = 0u; n < 3u; ++n)
			{
				bool accurateX = false;
				bool accurateY = false;

				for (const T jacobianEpsilon : jacobianEpsilons)
				{
					VectorT3<T> objectPointEps(objectPointIF);
					objectPointEps[n] += jacobianEpsilon;

					const VectorT2<T> imagePointEps(anyCamera.projectToImageIF(objectPointEps));

					const T dx = (imagePointEps.x() - reprojectedImagePoint.x()) / jacobianEpsilon;
					const T dy = (imagePointEps.y() - reprojectedImagePoint.y()) / jacobianEpsilon;

					const T xDiff = NumericT<T>::abs(jacobianX[n] - dx);
					const T xMax = max(NumericT<T>::abs(jacobianX[n]), NumericT<T>::abs(dx));

					if ((NumericT<T>::isWeakEqualEps(xMax) && xDiff <= T(0.001)) || (NumericT<T>::isNotEqualEps(xMax) && xDiff / xMax <= T(0.05)))
					{
						accurateX = true;
					}

					const T yDiff = NumericT<T>::abs(jacobianY[n] - dy);
					const T yMax = max(NumericT<T>::abs(jacobianY[n]), NumericT<T>::abs(dy));

					if ((NumericT<T>::isWeakEqualEps(yMax) && yDiff <= T(0.001)) || (NumericT<T>::isNotEqualEps(yMax) && yDiff / yMax <= T(0.05)))
					{
						accurateY = true;
					}

					if (accurateX && accurateY)
					{
						break;
					}
				}

				if (!accurateX || !accurateY)
				{
					accurate = false;
				}
			}

			if (accurate)
			{
				++succeeded;
			}

			++iterations;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	constexpr double threshold = std::is_same<T, float>::value ? 0.75 : 0.99;

	const double percent = double(succeeded) / double(iterations);

	return percent >= threshold;
}

template bool OCEAN_TEST_MATH_EXPORT TestAnyCamera::verifyAnyCamera(const AnyCameraT<float>& anyCamera, const double, RandomGenerator* randomGenerator);
template bool OCEAN_TEST_MATH_EXPORT TestAnyCamera::verifyAnyCamera(const AnyCameraT<double>& anyCamera, const double, RandomGenerator* randomGenerator);

}

}

}
