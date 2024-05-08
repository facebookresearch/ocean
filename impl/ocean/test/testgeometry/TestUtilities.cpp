/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testgeometry/TestUtilities.h"
#include "ocean/test/testgeometry/Utilities.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/geometry/Utilities.h"

#include "ocean/math/Random.h"
#include "ocean/math/Triangle2.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

bool TestUtilities::test(const double testDuration)
{
	ocean_assert(testDuration > 0);

	Log::info() << "---   Utilities test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testCreateObjectPoints(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testComputePolygonArea(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testIsInsideConvexPolygon(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testRandomCameraPosePinhole(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testRandomCameraPoseFisheye(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Utilities test succeeded.";
	}
	else
	{
		Log::info() << "Utilities test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestUtilities, CreateObjectPoints)
{
	EXPECT_TRUE(TestUtilities::testCreateObjectPoints(GTEST_TEST_DURATION));
}

TEST(TestUtilities, ComputePolygonArea)
{
	EXPECT_TRUE(TestUtilities::testComputePolygonArea(GTEST_TEST_DURATION));
}

TEST(TestUtilities, IsInsideConvexPolygon)
{
	EXPECT_TRUE(TestUtilities::testIsInsideConvexPolygon(GTEST_TEST_DURATION));
}

TEST(TestUtilities, RandomCameraPosePinhole)
{
	EXPECT_TRUE(TestUtilities::testRandomCameraPosePinhole(GTEST_TEST_DURATION));
}

TEST(TestUtilities, RandomCameraPoseFisheye)
{
	EXPECT_TRUE(TestUtilities::testRandomCameraPoseFisheye(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestUtilities::testCreateObjectPoints(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Create 3D object points test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const AnyCameraType anyCameraType = Random::random(randomGenerator, Utilities::realisticCameraTypes());
		const unsigned int anyCameraIndex = RandomI::random(randomGenerator, 1u);

		const SharedAnyCamera camera = Utilities::realisticAnyCamera(anyCameraType, anyCameraIndex);
		ocean_assert(camera);

		const unsigned int numberPoints = RandomI::random(randomGenerator, 1u, 1000u);

		Vectors2 imagePoints;
		imagePoints.reserve(numberPoints);

		for (unsigned int n = 0u; n < numberPoints; ++n)
		{
			imagePoints.emplace_back(Random::vector2(randomGenerator, Scalar(5), Scalar(camera->width() - 5u), Scalar(5), Scalar(camera->height() - 5u)));
		}

		const Quaternion rotation = Random::quaternion(randomGenerator);
		const Vector3 translation = Random::vector3(randomGenerator) * Scalar(10);

		const HomogenousMatrix4 world_T_camera(translation, rotation);

		const Scalar distance = Random::scalar(randomGenerator, Scalar(0.1), Scalar(10));

		const Vectors3 objectPoints = Geometry::Utilities::createObjectPoints(*camera, world_T_camera, ConstArrayAccessor<Vector2>(imagePoints), distance);

		if (objectPoints.size() == imagePoints.size())
		{
			const HomogenousMatrix4 flippedCamera_T_world(AnyCamera::standard2InvertedFlipped(world_T_camera));

			for (size_t n = 0; n < imagePoints.size(); ++n)
			{
				if (!AnyCamera::isObjectPointInFrontIF(flippedCamera_T_world, objectPoints[n]))
				{
					allSucceeded = false;
				}

				if (Numeric::isNotEqual(world_T_camera.translation().distance(objectPoints[n]), distance, Scalar(0.001)))
				{
					allSucceeded = false;
				}

				const Vector2 projectedObjectPoint = camera->projectToImageIF(flippedCamera_T_world, objectPoints[n]);

				if (projectedObjectPoint.distance(imagePoints[n]) > 1)
				{
					allSucceeded = false;
				}
			}
		}
		else
		{
			allSucceeded = false;
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

bool TestUtilities::testComputePolygonArea(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "computePolygonArea test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		// 2D coordinate system: x-right, y-up
		const Vector2 cornerBL(Scalar(RandomI::random(randomGenerator, -10, 10)), Scalar(RandomI::random(randomGenerator, -10, 10)));
		const Vector2 cornerTR = cornerBL + Vector2(Scalar(RandomI::random(randomGenerator, 1, 10)), Scalar(RandomI::random(randomGenerator, 1, 10)));

		const Vector2 cornerTL = Vector2(cornerBL.x(), cornerTR.y());
		const Vector2 cornerBR = Vector2(cornerTR.x(), cornerBL.y());

		const Vector2 midPointBottom = (cornerBL + cornerBR) * Scalar(0.5);

		const Vector2 line[2] =
		{
			cornerBL,
			cornerTR
		};

		if (!Numeric::isEqualEps(Geometry::Utilities::computePolygonAreaSigned(line, 2)))
		{
			allSucceeded = false;
		}

		const Vector2 rectangleCCW[4] =
		{
			cornerBL,
			cornerBR,
			cornerTR,
			cornerTL
		};

		const Vector2 rectangleCW[4] =
		{
			cornerBL,
			cornerTL,
			cornerTR,
			cornerBR
		};

		const Scalar rectangleArea = (cornerBR.x() - cornerBL.x()) * (cornerTL.y() - cornerBL.y());
		ocean_assert(rectangleArea >= 0);

		const Scalar rectangleSignedAreaCCW = Geometry::Utilities::computePolygonAreaSigned(rectangleCCW, 4);
		const Scalar rectangleSignedAreaCW = Geometry::Utilities::computePolygonAreaSigned(rectangleCW, 4);

		if (!Numeric::isEqual(rectangleSignedAreaCCW, rectangleArea))
		{
			allSucceeded = false;
		}

		if (!Numeric::isEqual(rectangleSignedAreaCW, -rectangleArea))
		{
			allSucceeded = false;
		}

		if (!Numeric::isEqual(Geometry::Utilities::computePolygonArea(rectangleCCW, 4), rectangleArea))
		{
			allSucceeded = false;
		}

		if (!Numeric::isEqual(Geometry::Utilities::computePolygonArea(rectangleCW, 4),  rectangleArea))
		{
			allSucceeded = false;
		}

		const Vector2 triangleCCW[3] =
		{
			cornerTL,
			midPointBottom,
			cornerTR
		};

		const Vector2 triangleCW[3] =
		{
			cornerTL,
			cornerTR,
			midPointBottom
		};

		const Triangle2 triangle2CCW(cornerTL, midPointBottom, cornerTR);
		const Triangle2 triangle2CW(cornerTL, cornerTR, midPointBottom);
		ocean_assert((triangle2CW.area() - triangle2CCW.area()) < Numeric::eps());

		if (Geometry::Utilities::computePolygonAreaSigned(triangleCCW, 3) - triangle2CCW.area() > Numeric::eps())
		{
			allSucceeded = false;
		}

		if (Geometry::Utilities::computePolygonAreaSigned(triangleCW, 3) - triangle2CW.area() > Numeric::eps())
		{
			allSucceeded = false;
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

bool TestUtilities::testIsInsideConvexPolygon(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "isInsideConvexPolygon test:";

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	const Scalar range = std::is_same<Scalar, float>::value ? Scalar(100) : Scalar(1000);

	const Timestamp startTimestamp(true);

	do
	{
		bool localSuccess = true;

		const size_t polygonSize = size_t(RandomI::random(3u, 100u));

		const Scalar radius = Random::scalar(Scalar(0.1), range);
		const Vector2 center = Random::vector2(-range, range);

		Vectors2 circularPolygon;
		circularPolygon.reserve(polygonSize);

		ocean_assert(polygonSize > 0);
		const Scalar additionalRandomRotation = Random::scalar(-Numeric::pi(), Numeric::pi());

		const Scalar directionSign = Random::sign(); // allowing to make a cw circle or a ccw circle

		for (size_t i = 0; i < polygonSize; ++i)
		{
			const Scalar angle = Numeric::pi2() * Scalar(i) / Scalar(polygonSize);
			const Scalar adjustedAngle = (additionalRandomRotation + angle) * directionSign;

			circularPolygon.emplace_back(center + Vector2(Numeric::cos(adjustedAngle), Numeric::sin(adjustedAngle)) * radius);
		}

		const bool strict = RandomI::random(1u) == 0u;

		if (!Geometry::Utilities::isPolygonConvex(circularPolygon.data(), circularPolygon.size(), strict))
		{
			ocean_assert(false && "This should never happen!");

			localSuccess = false;
		}

		// Because this is a discretized circle, selecting a point inside the continuous circle defined by the radius may be outside the polygon.
		// To avoid that case, determine the radius of the circle that is fully enclosed by the polygon. This radius is the distance from the center to the mid point of any edge.
		const Vector2 midPoint = (circularPolygon[0] + circularPolygon[1]) * Scalar(0.5);
		const Scalar innerRadius = (midPoint - center).length();
		ocean_assert(innerRadius <= radius);

		const Vector2 pointInside = center + Random::vector2() * innerRadius;
		const Vector2 pointOutside = center + Vector2(Random::scalar(Scalar(1.01), range) * radius, Random::scalar(Scalar(1.01), range) * radius);
		const Vector2 pointOnEdge = circularPolygon[0] + ((circularPolygon[1] - circularPolygon[0]) * Scalar(0.5));
		ocean_assert(Line2(circularPolygon[0], (circularPolygon[1] - circularPolygon[0]).normalized()).isOnLine(pointOnEdge));

		if (Geometry::Utilities::isInsideConvexPolygon(circularPolygon, pointOutside) != false)
		{
			localSuccess = false;
		}

		if (Geometry::Utilities::isInsideConvexPolygon(circularPolygon, pointInside) != true)
		{
			localSuccess = false;
		}

		if (Geometry::Utilities::isInsideConvexPolygon(circularPolygon, pointOnEdge) != true)
		{
			localSuccess = false;
		}

		if (Geometry::Utilities::isInsideConvexPolygon(circularPolygon.data(), circularPolygon.size(), pointOutside) != false)
		{
			localSuccess = false;
		}

		if (Geometry::Utilities::isInsideConvexPolygon(circularPolygon.data(), circularPolygon.size(), pointInside) != true)
		{
			localSuccess = false;
		}

		if (Geometry::Utilities::isInsideConvexPolygon(circularPolygon.data(), circularPolygon.size(), pointOnEdge) != true)
		{
			localSuccess = false;
		}

		if (localSuccess)
		{
			++validIterations;
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

bool TestUtilities::testRandomCameraPosePinhole(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Random camera pose for pinhole camera test:";

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(100u, 1920u);
		const unsigned int height = RandomI::random(100u, 1080u);
		const Scalar fovX = Random::scalar(Numeric::deg2rad(30), Numeric::deg2rad(70));

		const PinholeCamera pinholeCamera(width, height, fovX);

		const Vector3 objectPoint = Random::vector3(-10, 10);
		const Vector3 cameraDirection = Random::vector3();

		const Line3 objectPointRay(objectPoint, cameraDirection);

		const Vector2 imagePoint = Random::vector2(Scalar(5), Scalar(pinholeCamera.width() - 5u), Scalar(5), Scalar(pinholeCamera.height() - 5u));
		const Scalar distance = Random::scalar(Scalar(0.01), 10);

		const HomogenousMatrix4 world_T_camera = Geometry::Utilities::randomCameraPose(pinholeCamera, objectPointRay, imagePoint, distance);
		ocean_assert(world_T_camera.isValid());

		const HomogenousMatrix4 flippedCamera_T_world = PinholeCamera::standard2InvertedFlipped(world_T_camera);

		if (PinholeCamera::isObjectPointInFrontIF(flippedCamera_T_world, objectPoint))
		{
			if (Numeric::isWeakEqual(objectPoint.distance(world_T_camera.translation()), distance))
			{
				const Vector2 projectedObjectPoint = pinholeCamera.projectToImageIF<true>(flippedCamera_T_world, objectPoint, pinholeCamera.hasDistortionParameters());

				if (projectedObjectPoint.distance(imagePoint) <= 1)
				{
					++validIterations;
				}
			}
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

bool TestUtilities::testRandomCameraPoseFisheye(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Random camera pose for fisheye camera test:";

	uint64_t iterations = 0ull;
	uint64_t validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		const FisheyeCamera fisheyeCamera = Utilities::realisticFisheyeCamera(RandomI::random(1u));

		const Vector3 objectPoint = Random::vector3(-10, 10);
		const Vector3 cameraDirection = Random::vector3();

		const Line3 objectPointRay(objectPoint, cameraDirection);

		const Vector2 imagePoint = Random::vector2(Scalar(5), Scalar(fisheyeCamera.width() - 5u), Scalar(5), Scalar(fisheyeCamera.height() - 5u));
		const Scalar distance = Random::scalar(Scalar(0.01), 10);

		const HomogenousMatrix4 world_T_camera = Geometry::Utilities::randomCameraPose(fisheyeCamera, objectPointRay, imagePoint, distance);
		ocean_assert(world_T_camera.isValid());

		const HomogenousMatrix4 flippedCamera_T_world = PinholeCamera::standard2InvertedFlipped(world_T_camera);

		if (PinholeCamera::isObjectPointInFrontIF(flippedCamera_T_world, objectPoint))
		{
			if (Numeric::isWeakEqual(objectPoint.distance(world_T_camera.translation()), distance))
			{
				const Vector2 projectedObjectPoint = fisheyeCamera.projectToImageIF(flippedCamera_T_world, objectPoint);

				if (projectedObjectPoint.distance(imagePoint) <= 1)
				{
					++validIterations;
				}
			}
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

} // TestGeometry

} // Test

} // Ocean
