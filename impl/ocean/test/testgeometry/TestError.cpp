/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testgeometry/TestError.h"
#include "ocean/test/testgeometry/Utilities.h"

#include "ocean/base/Timestamp.h"

#include "ocean/geometry/Error.h"

#include "ocean/math/Random.h"

#include "ocean/test/ValidationPrecision.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

bool TestError::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Error test:   ---";

	bool allSucceeded = true;

	Log::info() << " ";

	allSucceeded = testDeterminePoseErrorSeparatePinhole(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testDeterminePoseErrorSeparateAnyCamera(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testDeterminePoseErrorCombinedPinhole(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testDeterminePoseErrorCombinedAnyCamera(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testDetermineHomographyErrorSeparate(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Error test succeeded.";
	}
	else
	{
		Log::info() << "Error test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestError, DeterminePoseErrorSeparatePinhole)
{
	EXPECT_TRUE(TestError::testDeterminePoseErrorSeparatePinhole(GTEST_TEST_DURATION));
}

TEST(TestError, DeterminePoseErrorSeparateAnyCamera)
{
	EXPECT_TRUE(TestError::testDeterminePoseErrorSeparateAnyCamera(GTEST_TEST_DURATION));
}

TEST(TestError, DeterminePoseErrorCombinedPinhole)
{
	EXPECT_TRUE(TestError::testDeterminePoseErrorCombinedPinhole(GTEST_TEST_DURATION));
}

TEST(TestError, DeterminePoseErrorCombinedAnyCamera)
{
	EXPECT_TRUE(TestError::testDeterminePoseErrorCombinedAnyCamera(GTEST_TEST_DURATION));
}

TEST(TestError, DetermineHomographyErrorSeparate)
{
	EXPECT_TRUE(TestError::testDetermineHomographyErrorSeparate(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestError::testDeterminePoseErrorSeparatePinhole(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing separate pose error determination with pinhole camera:";

	RandomGenerator randomGenerator;

	ValidationPrecision validation(0.999, randomGenerator);

	const Scalar epsilon = std::is_same<float, Scalar>::value ? Scalar(0.01) : Scalar(0.001);

	const PinholeCamera patternCamera(1280u, 720u, Numeric::deg2rad(45));

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int distortionIteration = 0u; distortionIteration < 4u; ++distortionIteration)
		{
			constexpr bool principalPointDistortion = true;

			const bool radialDistortion = distortionIteration == 1u || distortionIteration == 3u;
			const bool tangentialDistortion = distortionIteration == 2u || distortionIteration == 3u;

			const PinholeCamera pinholeCamera(Utilities::distortedCamera(patternCamera, principalPointDistortion, radialDistortion, tangentialDistortion));

			const Vectors3 objectPoints = Utilities::objectPoints(Box3(Vector3(-10, -10, -10), Vector3(10, 10, 10)), 100u, &randomGenerator);
			const HomogenousMatrix4 world_T_camera(Utilities::viewPosition(pinholeCamera, objectPoints, false, &randomGenerator));

			Vectors2 perfectImagePoints;
			Vectors2 distortedImagePoints;
			Vectors2 noisedImagePoints;

			perfectImagePoints.reserve(objectPoints.size());
			distortedImagePoints.reserve(objectPoints.size());
			noisedImagePoints.reserve(objectPoints.size());

			for (const Vector3& objectPoint : objectPoints)
			{
				perfectImagePoints.emplace_back(pinholeCamera.projectToImage<true>(world_T_camera, objectPoint, false));
				distortedImagePoints.emplace_back(pinholeCamera.projectToImage<true>(world_T_camera, objectPoint, true));

				noisedImagePoints.emplace_back(distortedImagePoints.back() + Vector2(Random::gaussianNoise(5), Random::gaussianNoise(5)));
			}

			for (const bool distortImagePoints : {false, true})
			{
				for (const Vectors2& imagePoints : {perfectImagePoints, distortedImagePoints, noisedImagePoints})
				{
					Vectors2 errors(objectPoints.size());
					Scalars sqrErrors(objectPoints.size());
					const Scalar averageSqrError = Geometry::Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true, true, true>(world_T_camera, pinholeCamera, ConstTemplateArrayAccessor<Vector3>(objectPoints), ConstTemplateArrayAccessor<Vector2>(imagePoints), distortImagePoints, 1, errors.data(), sqrErrors.data());

					Scalar testAverageSqrError = 0;

					for (size_t n = 0; n < objectPoints.size(); ++n)
					{
						ValidationPrecision::ScopedIteration scopedIteration(validation);

						const Vector2 testProjectedImagePoint = pinholeCamera.projectToImage<true>(world_T_camera, objectPoints[n], distortImagePoints);
						const Vector2 testError = testProjectedImagePoint - imagePoints[n];

						const Scalar testSqrError = testError.sqr();

						if (!testError.isEqual(errors[n], epsilon))
						{
							scopedIteration.setInaccurate();
						}

						if (!Numeric::isEqual(testSqrError, sqrErrors[n], epsilon))
						{
							scopedIteration.setInaccurate();
						}

						testAverageSqrError += testSqrError;
					}

					ocean_assert(objectPoints.size() != 0);
					testAverageSqrError /= Scalar(objectPoints.size());

					if (Numeric::isNotEqual(testAverageSqrError, averageSqrError, epsilon))
					{
						OCEAN_SET_FAILED(validation);
					}
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestError::testDeterminePoseErrorSeparateAnyCamera(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing separate pose error determination with any camera:";

	RandomGenerator randomGenerator;

	uint64_t distortionIteration = 0ull;

	ValidationPrecision validation(0.999, randomGenerator);

	const Scalar epsilon = std::is_same<float, Scalar>::value ? Scalar(0.01) : Scalar(0.001);

	const PinholeCamera patternCamera(1280u, 720u, Numeric::deg2rad(45));

	const Timestamp startTimestamp(true);

	do
	{
		std::shared_ptr<AnyCamera> anyCamera;

		for (const std::string& cameraName : {AnyCameraPinhole::WrappedCamera::name(), AnyCameraFisheye::WrappedCamera::name()})
		{
			if (cameraName == AnyCameraPinhole::WrappedCamera::name())
			{
				PinholeCamera pinholeCamera(Utilities::distortedCamera(patternCamera, true, distortionIteration % 3ull == 1ull || distortionIteration % 3ull == 2ull, distortionIteration % 3ull == 2ull));

				anyCamera = std::make_shared<AnyCameraPinhole>(std::move(pinholeCamera));
			}
			else
			{
				ocean_assert(cameraName == AnyCameraFisheye::WrappedCamera::name());

				anyCamera = std::make_shared<AnyCameraFisheye>(Utilities::realisticFisheyeCamera((unsigned int)(distortionIteration % 2ull)));
			}

			ocean_assert(anyCamera);

			const Vectors3 objectPoints = Utilities::objectPoints(Box3(Vector3(-10, -10, -10), Vector3(10, 10, 10)), 100u, &randomGenerator);
			const HomogenousMatrix4 world_T_camera(Utilities::viewPosition(*anyCamera, objectPoints, false, &randomGenerator));

			Vectors2 imagePoints;
			Vectors2 noisedImagePoints;

			imagePoints.reserve(objectPoints.size());
			noisedImagePoints.reserve(objectPoints.size());

			for (const Vector3& objectPoint : objectPoints)
			{
				imagePoints.push_back(anyCamera->projectToImage(world_T_camera, objectPoint));
				noisedImagePoints.push_back(imagePoints.back() + Vector2(Random::gaussianNoise(5), Random::gaussianNoise(5)));
			}

			for (const Vectors2& currentImagePoints : {imagePoints, noisedImagePoints})
			{
				Vectors2 errors(objectPoints.size());
				Scalars sqrErrors(objectPoints.size());
				const Scalar averageSqrError = Geometry::Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true, true>(world_T_camera, *anyCamera, ConstTemplateArrayAccessor<Vector3>(objectPoints), ConstTemplateArrayAccessor<Vector2>(currentImagePoints), errors.data(), sqrErrors.data());

				Scalar testAverageSqrError = 0;

				for (size_t n = 0; n < objectPoints.size(); ++n)
				{
					ValidationPrecision::ScopedIteration scopedIteration(validation);

					const Vector2 testProjectedImagePoint = anyCamera->projectToImage(world_T_camera, objectPoints[n]);
					const Vector2 testError = testProjectedImagePoint - currentImagePoints[n];

					const Scalar testSqrError = testError.sqr();

					if (!testError.isEqual(errors[n], epsilon))
					{
						scopedIteration.setInaccurate();
					}

					if (!Numeric::isEqual(testSqrError, sqrErrors[n], epsilon))
					{
						scopedIteration.setInaccurate();
					}

					testAverageSqrError += testSqrError;
				}

				ocean_assert(objectPoints.size() != 0);
				testAverageSqrError /= Scalar(objectPoints.size());

				if (Numeric::isNotEqual(testAverageSqrError, averageSqrError, epsilon))
				{
					OCEAN_SET_FAILED(validation);
				}
			}

			++distortionIteration;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestError::testDeterminePoseErrorCombinedPinhole(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing combined pose error determination with pinhole camera:";

	RandomGenerator randomGenerator;

	ValidationPrecision validation(0.999, randomGenerator);

	const Scalar epsilon = std::is_same<float, Scalar>::value ? Scalar(0.01) : Scalar(0.001);

	const PinholeCamera patternCamera(1280u, 720u, Numeric::deg2rad(45));

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int distortionIteration = 0u; distortionIteration < 4u; ++distortionIteration)
		{
			constexpr bool principalPointDistortion = true;

			const bool radialDistortion = distortionIteration == 1u || distortionIteration == 3u;
			const bool tangentialDistortion = distortionIteration == 2u || distortionIteration == 3u;

			const PinholeCamera pinholeCamera(Utilities::distortedCamera(patternCamera, principalPointDistortion, radialDistortion, tangentialDistortion));

			const Vectors3 objectPoints = Utilities::objectPoints(Box3(Vector3(-10, -10, -10), Vector3(10, 10, 10)), 100u, &randomGenerator);
			const HomogenousMatrix4 pose(Utilities::viewPosition(pinholeCamera, objectPoints, false, &randomGenerator));

			Vectors2 perfectImagePoints;
			Vectors2 distortedImagePoints;
			Vectors2 noisedImagePoints;

			perfectImagePoints.reserve(objectPoints.size());
			distortedImagePoints.reserve(objectPoints.size());
			noisedImagePoints.reserve(objectPoints.size());

			for (const Vector3& objectPoint : objectPoints)
			{
				perfectImagePoints.push_back(pinholeCamera.projectToImage<true>(pose, objectPoint, false));
				distortedImagePoints.push_back(pinholeCamera.projectToImage<true>(pose, objectPoint, true));

				noisedImagePoints.push_back(distortedImagePoints.back() + Vector2(Random::gaussianNoise(5), Random::gaussianNoise(5)));
			}

			for (const bool distortImagePoints : {false, true})
			{
				for (const Vectors2& imagePoints : {perfectImagePoints, distortedImagePoints, noisedImagePoints})
				{
					ValidationPrecision::ScopedIteration scopedIteration(validation);

					Scalar averageSqrError = Numeric::minValue();
					Scalar minimalSqrError = Numeric::minValue();
					Scalar maximalSqrError = Numeric::minValue();
					Geometry::Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true>(pose, pinholeCamera, ConstTemplateArrayAccessor<Vector3>(objectPoints), ConstTemplateArrayAccessor<Vector2>(imagePoints), distortImagePoints, averageSqrError, minimalSqrError, maximalSqrError, Scalar(1));

					Scalar testAverageSqrError = 0;
					Scalar testMinimalSqrError = Numeric::maxValue();
					Scalar testMaximalSqrError = Numeric::minValue();

					for (size_t n = 0; n < objectPoints.size(); ++n)
					{
						const Vector2 testProjectedImagePoint = pinholeCamera.projectToImage<true>(pose, objectPoints[n], distortImagePoints);
						const Vector2 testError = testProjectedImagePoint - imagePoints[n];
						const Scalar testSqrError = testError.sqr();

						testMinimalSqrError = min(testMinimalSqrError, testSqrError);
						testMaximalSqrError = max(testMaximalSqrError, testSqrError);
						testAverageSqrError += testSqrError;
					}

					ocean_assert(objectPoints.size() != 0);
					testAverageSqrError /= Scalar(objectPoints.size());

					if (!Numeric::isEqual(testMinimalSqrError, minimalSqrError, epsilon))
					{
						scopedIteration.setInaccurate();
					}

					if (!Numeric::isEqual(testMaximalSqrError, maximalSqrError, epsilon))
					{
						scopedIteration.setInaccurate();
					}

					if (!Numeric::isEqual(testAverageSqrError, averageSqrError, epsilon))
					{
						scopedIteration.setInaccurate();
					}
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestError::testDeterminePoseErrorCombinedAnyCamera(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing combined pose error determination with any camera:";

	RandomGenerator randomGenerator;

	const Scalar epsilon = std::is_same<float, Scalar>::value ? Scalar(0.01) : Scalar(0.001);

	ValidationPrecision validation(0.999, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const AnyCameraType anyCameraType = Random::random(randomGenerator, Utilities::realisticCameraTypes());
		const unsigned int anyCameraIndex = RandomI::random(randomGenerator, 1u);

		const SharedAnyCamera camera = Utilities::realisticAnyCamera(anyCameraType, anyCameraIndex);
		ocean_assert(camera);

		const size_t numberCorrespondences = size_t(RandomI::random(randomGenerator, 1u, 100u));

		Vectors3 objectPoints = Utilities::objectPoints(Box3(Vector3(-10, -10, -10), Vector3(10, 10, 10)), numberCorrespondences, &randomGenerator);
		const HomogenousMatrix4 world_T_camera = Utilities::viewPosition(*camera, objectPoints, false, &randomGenerator);

		Vectors2 perfectImagePoints;
		Vectors2 noisedImagePoints;

		for (const Vector3& objectPoint : objectPoints)
		{
			const Vector2 imagePoint = camera->projectToImage(world_T_camera, objectPoint);

			ocean_assert(imagePoint.isEqual(camera->projectToImage(world_T_camera, objectPoint), Scalar(1)));

			perfectImagePoints.push_back(imagePoint);
			noisedImagePoints.emplace_back(imagePoint + Vector2(Random::gaussianNoise(5), Random::gaussianNoise(5)));
		}

		const bool placeObjectPointsBehindCamera = RandomI::random(randomGenerator, 1u);

		if (placeObjectPointsBehindCamera)
		{
			const size_t numberCorrespondencesBehindCamera = size_t(RandomI::random(randomGenerator, 1u, (unsigned int)(numberCorrespondences)));

			for (unsigned int n = 0u; n < numberCorrespondencesBehindCamera; ++n)
			{
				const Line3 ray = Line3(world_T_camera.translation(), (objectPoints[n] - world_T_camera.translation()).normalizedOrZero());

				objectPoints[n] = ray.point(Random::scalar(randomGenerator, Scalar(-10), Scalar(-0.01))); // we keep the same image point, but place the object point along the viewing ray behind the camera

				ocean_assert(!AnyCamera::isObjectPointInFrontIF(AnyCamera::standard2InvertedFlipped(world_T_camera), objectPoints[n]));

				if constexpr (std::is_same<double, Scalar>::value)
				{
					ocean_assert(perfectImagePoints[n].isEqual(camera->projectToImage(world_T_camera, objectPoints[n]), Scalar(1)));
				}
			}
		}

		const bool allowOnlyFrontObjectPoints = RandomI::random(randomGenerator, 1u);

		for (const Vectors2& imagePoints : {perfectImagePoints, noisedImagePoints})
		{
			Scalar averageSqrError = Numeric::minValue();
			Scalar minimalSqrError = Numeric::minValue();
			Scalar maximalSqrError = Numeric::minValue();

			bool poseErrorResult = false;

			if (allowOnlyFrontObjectPoints)
			{
				poseErrorResult = Geometry::Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true>(world_T_camera, *camera, ConstTemplateArrayAccessor<Vector3>(objectPoints), ConstTemplateArrayAccessor<Vector2>(imagePoints), averageSqrError, minimalSqrError, maximalSqrError);
			}
			else
			{
				poseErrorResult = Geometry::Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, false>(world_T_camera, *camera, ConstTemplateArrayAccessor<Vector3>(objectPoints), ConstTemplateArrayAccessor<Vector2>(imagePoints), averageSqrError, minimalSqrError, maximalSqrError);
			}

			if (allowOnlyFrontObjectPoints)
			{
				if (poseErrorResult != !placeObjectPointsBehindCamera)
				{
					OCEAN_SET_FAILED(validation);
				}
			}

			if (poseErrorResult)
			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				Scalar testAverageSqrError = 0;
				Scalar testMinimalSqrError = Numeric::maxValue();
				Scalar testMaximalSqrError = Numeric::minValue();

				for (size_t n = 0; n < objectPoints.size(); ++n)
				{
					const Vector2 testProjectedImagePoint = camera->projectToImage(world_T_camera, objectPoints[n]);
					const Vector2 testError = testProjectedImagePoint - imagePoints[n];
					const Scalar testSqrError = testError.sqr();

					testMinimalSqrError = min(testMinimalSqrError, testSqrError);
					testMaximalSqrError = max(testMaximalSqrError, testSqrError);
					testAverageSqrError += testSqrError;
				}

				ocean_assert(objectPoints.size() != 0);
				testAverageSqrError /= Scalar(objectPoints.size());

				if (!Numeric::isEqual(testMinimalSqrError, minimalSqrError, epsilon))
				{
					scopedIteration.setInaccurate();
				}

				if (!Numeric::isEqual(testMaximalSqrError, maximalSqrError, epsilon))
				{
					scopedIteration.setInaccurate();
				}

				if (!Numeric::isEqual(testAverageSqrError, averageSqrError, epsilon))
				{
					scopedIteration.setInaccurate();
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestError::testDetermineHomographyErrorSeparate(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing separate homography error determination:";

	RandomGenerator randomGenerator;

	ValidationPrecision validation(0.999, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int size = RandomI::random(randomGenerator, 1u, 1000u);

		Vectors2 points0;
		Vectors2 points1;

		points0.reserve(size);
		points1.reserve(size);

		for (unsigned int n = 0u; n < size; ++n)
		{
			points0.emplace_back(Random::vector2(randomGenerator, -10, 10));
			points1.emplace_back(Random::vector2(randomGenerator, -10, 10));
		}

		// we create a random homography

		SquareMatrix3 homography(true);

		for (unsigned int n = 0u; n < 6u; ++n)
		{
			homography[n] += Random::scalar(randomGenerator, Scalar(-0.05), Scalar(0.05));
		}

		homography[6] = Random::scalar(randomGenerator, -10, 10); // tx
		homography[7] = Random::scalar(randomGenerator, -10, 10); // ty

		if (homography.isSingular() == false)
		{
			Scalar testAverageSqrError = Scalar(0);
			Vectors2 testErrors;
			Scalars testSqrErrors;

			testErrors.reserve(size);
			testSqrErrors.reserve(size);

			for (unsigned int n = 0u; n < size; ++n)
			{
				const Vector3 transformedPoint = homography * Vector3(points0[n], 1);

				if (Numeric::isEqualEps(transformedPoint.z()))
				{
					testErrors.emplace_back(Numeric::maxValue(), Numeric::maxValue());
					testSqrErrors.push_back(Numeric::maxValue());

					testAverageSqrError += Numeric::maxValue();
				}
				else
				{
					const Vector2 normalizedTransformedPoint(transformedPoint.x() / transformedPoint.z(), transformedPoint.y() / transformedPoint.z());

					testErrors.emplace_back(normalizedTransformedPoint - points1[n]);

					const Scalar sqrError = normalizedTransformedPoint.sqrDistance(points1[n]);
					testSqrErrors.push_back(sqrError);

					testAverageSqrError += sqrError;
				}
			}

			if (size != 0)
			{
				testAverageSqrError /= Scalar(size);
			}

			{
				// testing runtime-based accessor

				typedef ConstArrayAccessor<Vector2> Accessor;

				Vectors2 errors(size);
				Scalars sqrErrors(size);

				const Scalar averageSqrError = Geometry::Error::determineHomographyError<Accessor, true, true>(homography, Accessor(points0), Accessor(points1), errors.data(), sqrErrors.data());

				if (!Numeric::isEqual<-6>(averageSqrError, testAverageSqrError, Numeric::weakEps()))
				{
					OCEAN_SET_FAILED(validation);
				}

				for (unsigned int n = 0u; n < size; ++n)
				{
					ValidationPrecision::ScopedIteration scopedIteration(validation);

					if (!Numeric::isEqual<-6>(errors[n].x(), testErrors[n].x(), Numeric::weakEps()))
					{
						scopedIteration.setInaccurate();
					}

					if (!Numeric::isEqual<-6>(errors[n].y(), testErrors[n].y(), Numeric::weakEps()))
					{
						scopedIteration.setInaccurate();
					}

					if (!Numeric::isEqual<-6>(sqrErrors[n], testSqrErrors[n], Numeric::weakEps()))
					{
						scopedIteration.setInaccurate();
					}
				}
			}

			{
				// testing template-based accessor

				typedef ConstTemplateArrayAccessor<Vector2> Accessor;

				Vectors2 errors(size);
				Scalars sqrErrors(size);

				const Scalar averageSqrError = Geometry::Error::determineHomographyError<Accessor, true, true>(homography, Accessor(points0), Accessor(points1), errors.data(), sqrErrors.data());

				if (!Numeric::isEqual<-6>(averageSqrError, testAverageSqrError, Numeric::weakEps()))
				{
					OCEAN_SET_FAILED(validation);
				}

				for (unsigned int n = 0u; n < size; ++n)
				{
					ValidationPrecision::ScopedIteration scopedIteration(validation);

					if (Numeric::isNotWeakEqual(errors[n].x(), testErrors[n].x()))
					{
						scopedIteration.setInaccurate();
					}

					if (Numeric::isNotWeakEqual(errors[n].y(), testErrors[n].y()))
					{
						scopedIteration.setInaccurate();
					}

					if (!Numeric::isEqual<-6>(sqrErrors[n], testSqrErrors[n], Numeric::weakEps()))
					{
						scopedIteration.setInaccurate();
					}
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
