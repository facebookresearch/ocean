/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/geometry/Utilities.h"
#include "ocean/geometry/Homography.h"

#include "ocean/math/FiniteLine2.h"
#include "ocean/math/Random.h"

#include <numeric>

namespace Ocean
{

namespace Geometry
{

ObjectPoint Utilities::backProjectImagePoint(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose, const Plane3& plane, const ImagePoint& imagePoint, const bool useDistortionParameters, bool* frontObjectPoint)
{
	ocean_assert(pinholeCamera.isValid() && pose.isValid() && plane.isValid());

	const Vector2 distortionFreeImagePoint = useDistortionParameters ? pinholeCamera.undistort<true>(imagePoint) : imagePoint;
	const Line3 ray(pinholeCamera.ray(distortionFreeImagePoint, pose));

	Vector3 objectPoint(pose.translation());
	if (!plane.intersection(ray, objectPoint))
	{
		ocean_assert(false && "The plane is in parallel with the viewing direction!");
	}

	ocean_assert(pinholeCamera.projectToImage<true>(pose, objectPoint, useDistortionParameters).sqrDistance(imagePoint) < 1);

	if (frontObjectPoint)
	{
		*frontObjectPoint = (pose.inverted() * objectPoint).z() < -Numeric::eps();
	}

	return objectPoint;
}

ObjectPoints Utilities::backProjectImagePoints(const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_camera, const Plane3& plane, const ImagePoint* imagePoints, const size_t numberImagePoints, Indices32* frontObjectPointIndices)
{
	ocean_assert(anyCamera.isValid() && world_T_camera.isValid() && plane.isValid());
	ocean_assert(imagePoints);

	if (frontObjectPointIndices)
	{
		ocean_assert(frontObjectPointIndices->empty());
		frontObjectPointIndices->clear();
		frontObjectPointIndices->reserve(numberImagePoints);
	}

	ObjectPoints result;
	result.reserve(numberImagePoints);

	const HomogenousMatrix4 camera_T_world = world_T_camera.inverted();

	for (size_t n = 0; n < numberImagePoints; ++n)
	{
		const Vector2& imagePoint = imagePoints[n];

		const Line3 ray(anyCamera.ray(imagePoint, world_T_camera));

		Vector3 objectPoint(world_T_camera.translation());
		if (!plane.intersection(ray, objectPoint))
		{
			ocean_assert(false && "The plane is in parallel with the viewing direction!");
		}

		ocean_assert(anyCamera.projectToImage(world_T_camera, objectPoint).sqrDistance(imagePoint) < 1);

		result.emplace_back(objectPoint);

		if (frontObjectPointIndices && (camera_T_world * objectPoint).z() < -Numeric::eps())
		{
			frontObjectPointIndices->emplace_back(Index32(n));
		}
	}

	return result;
}

ObjectPoints Utilities::backProjectImagePoints(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose, const Plane3& plane, const ImagePoint* imagePoints, const size_t numberImagePoints, const bool useDistortionParameters, Indices32* frontObjectPointIndices)
{
	ocean_assert(pinholeCamera.isValid() && pose.isValid() && plane.isValid());
	ocean_assert(imagePoints);

	if (frontObjectPointIndices)
	{
		ocean_assert(frontObjectPointIndices->empty());
		frontObjectPointIndices->clear();
		frontObjectPointIndices->reserve(numberImagePoints);
	}

	ObjectPoints result;
	result.reserve(numberImagePoints);

	const HomogenousMatrix4 iPose(pose.inverted());

	const bool distortImagePoints = pinholeCamera.hasDistortionParameters() && useDistortionParameters;

	for (size_t n = 0; n < numberImagePoints; ++n)
	{
		const Vector2& imagePoint = imagePoints[n];
		const Vector2 distortionFreeImagePoint = distortImagePoints ? pinholeCamera.undistort<true>(imagePoint) : imagePoint;

		const Line3 ray(pinholeCamera.ray(distortionFreeImagePoint, pose));

		Vector3 objectPoint(pose.translation());
		if (!plane.intersection(ray, objectPoint))
		{
			ocean_assert(false && "The plane is in parallel with the viewing direction!");
		}

		ocean_assert(pinholeCamera.projectToImage<true>(pose, objectPoint, distortImagePoints).sqrDistance(imagePoint) < 1);

		result.push_back(objectPoint);

		if (frontObjectPointIndices && (iPose * objectPoint).z() < -Numeric::eps())
		{
			frontObjectPointIndices->push_back((unsigned int)n);
		}
	}

	return result;
}

ObjectPoints Utilities::backProjectImagePoints(const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_camera, const Cylinder3& cylinder, const ImagePoint* imagePoints, const size_t numberImagePoints, Indices32& intersectingPointIndices)
{
	ocean_assert(anyCamera.isValid() && world_T_camera.isValid() && cylinder.isValid());
	ocean_assert(imagePoints);

	ocean_assert(intersectingPointIndices.empty());
	intersectingPointIndices.clear();
	intersectingPointIndices.reserve(numberImagePoints);

	ObjectPoints result;
	result.reserve(numberImagePoints);

	for (size_t n = 0; n < numberImagePoints; ++n)
	{
		const Vector2& imagePoint = imagePoints[n];

		const Line3 ray(anyCamera.ray(imagePoint, world_T_camera));

		Vector3 objectPoint;
		if (cylinder.nearestIntersection(ray, objectPoint))
		{
			ocean_assert(anyCamera.projectToImage(world_T_camera, objectPoint).sqrDistance(imagePoint) < 1);
			ocean_assert(anyCamera.isObjectPointInFrontIF(CameraT<Scalar>::standard2InvertedFlipped(world_T_camera), objectPoint));

			result.emplace_back(objectPoint);

			intersectingPointIndices.emplace_back(Index32(n));
		}
	}

	return result;
}

ObjectPoints Utilities::backProjectImagePoints(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose, const Cylinder3& cylinder, const ImagePoint* imagePoints, const size_t numberImagePoints, const bool useDistortionParameters, Indices32& intersectingPointIndices)
{
	ocean_assert(pinholeCamera.isValid() && pose.isValid() && cylinder.isValid());
	ocean_assert(imagePoints);

	ocean_assert(intersectingPointIndices.empty());
	intersectingPointIndices.clear();
	intersectingPointIndices.reserve(numberImagePoints);

	ObjectPoints result;
	result.reserve(numberImagePoints);

	const bool distortImagePoints = pinholeCamera.hasDistortionParameters() && useDistortionParameters;

	for (size_t n = 0; n < numberImagePoints; ++n)
	{
		const Vector2& imagePoint = imagePoints[n];
		const Vector2 distortionFreeImagePoint = distortImagePoints ? pinholeCamera.undistort<true>(imagePoint) : imagePoint;

		const Line3 ray(pinholeCamera.ray(distortionFreeImagePoint, pose));

		Vector3 objectPoint;
		if (cylinder.nearestIntersection(ray, objectPoint))
		{
			ocean_assert(pinholeCamera.projectToImage<true>(pose, objectPoint, distortImagePoints).sqrDistance(imagePoint) < 1);
			ocean_assert(pinholeCamera.isObjectPointInFrontIF(PinholeCamera::standard2InvertedFlipped(pose), objectPoint));

			result.push_back(objectPoint);

			intersectingPointIndices.push_back((unsigned int)n);
		}
	}

	return result;
}

ObjectPoints Utilities::backProjectImagePoints(const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_camera, const Cone3& cone, const ImagePoint* imagePoints, const size_t numberImagePoints, Indices32& intersectingPointIndices)
{
	ocean_assert(anyCamera.isValid() && world_T_camera.isValid() && cone.isValid());
	ocean_assert(imagePoints);

	ocean_assert(intersectingPointIndices.empty());
	intersectingPointIndices.clear();
	intersectingPointIndices.reserve(numberImagePoints);

	ObjectPoints result;
	result.reserve(numberImagePoints);

	for (size_t n = 0; n < numberImagePoints; ++n)
	{
		const Vector2& imagePoint = imagePoints[n];

		const Line3 ray(anyCamera.ray(imagePoint, world_T_camera));

		Vector3 objectPoint;
		if (cone.nearestIntersection(ray, objectPoint))
		{
			ocean_assert(anyCamera.projectToImage(world_T_camera, objectPoint).sqrDistance(imagePoint) < 1);
			ocean_assert(anyCamera.isObjectPointInFrontIF(PinholeCamera::standard2InvertedFlipped(world_T_camera), objectPoint));

			result.emplace_back(objectPoint);

			intersectingPointIndices.push_back(Index32(n));
		}
	}

	return result;
}

ObjectPoints Utilities::backProjectImagePoints(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose, const Cone3& cone, const ImagePoint* imagePoints, const size_t numberImagePoints, const bool useDistortionParameters, Indices32& intersectingPointIndices)
{
	ocean_assert(pinholeCamera.isValid() && pose.isValid() && cone.isValid());
	ocean_assert(imagePoints);

	ocean_assert(intersectingPointIndices.empty());
	intersectingPointIndices.clear();
	intersectingPointIndices.reserve(numberImagePoints);

	ObjectPoints result;
	result.reserve(numberImagePoints);

	const bool distortImagePoints = pinholeCamera.hasDistortionParameters() && useDistortionParameters;

	for (size_t n = 0; n < numberImagePoints; ++n)
	{
		const Vector2& imagePoint = imagePoints[n];
		const Vector2 distortionFreeImagePoint = distortImagePoints ? pinholeCamera.undistort<true>(imagePoint) : imagePoint;

		const Line3 ray(pinholeCamera.ray(distortionFreeImagePoint, pose));

		Vector3 objectPoint;
		if (cone.nearestIntersection(ray, objectPoint))
		{
			ocean_assert(pinholeCamera.projectToImage<true>(pose, objectPoint, distortImagePoints).sqrDistance(imagePoint) < 1);
			ocean_assert(pinholeCamera.isObjectPointInFrontIF(PinholeCamera::standard2InvertedFlipped(pose), objectPoint));

			result.push_back(objectPoint);

			intersectingPointIndices.push_back((unsigned int)n);
		}
	}

	return result;
}

ObjectPoints Utilities::backProjectImagePointsDamped(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose, const Plane3& plane, const ImagePoint* imagePoints, const size_t numberImagePoints, const bool useDistortionParameters, Indices32* frontObjectPointIndices)
{
	ocean_assert(pinholeCamera.isValid() && pose.isValid() && plane.isValid());
	ocean_assert(imagePoints);

	if (frontObjectPointIndices)
	{
		ocean_assert(frontObjectPointIndices->empty());
		frontObjectPointIndices->clear();
		frontObjectPointIndices->reserve(numberImagePoints);
	}

	ObjectPoints result;
	result.reserve(numberImagePoints);

	const HomogenousMatrix4 iPose(pose.inverted());

	const bool distortImagePoints = pinholeCamera.hasDistortionParameters() && useDistortionParameters;

	for (size_t n = 0; n < numberImagePoints; ++n)
	{
		const Vector2& imagePoint = imagePoints[n];
		const Vector2 distortionFreeImagePoint = distortImagePoints ? pinholeCamera.undistortDamped(imagePoint) : imagePoint;

		const Line3 ray(pinholeCamera.ray(distortionFreeImagePoint, pose));

		Vector3 objectPoint(pose.translation());
		if (!plane.intersection(ray, objectPoint))
		{
			ocean_assert(false && "The plane is in parallel with the viewing direction!");
		}

		ocean_assert(pinholeCamera.projectToImageDamped(pose, objectPoint, distortImagePoints).sqrDistance(imagePoint) < 1);

		result.push_back(objectPoint);

		if (frontObjectPointIndices && (iPose * objectPoint).z() < -Numeric::eps())
		{
			frontObjectPointIndices->push_back((unsigned int)n);
		}
	}

	return result;
}

Vectors3 Utilities::createObjectPoints(const AnyCamera& camera, const HomogenousMatrix4& world_T_camera, const ConstIndexedAccessor<Vector2>& imagePoints, const Scalar distance)
{
	ocean_assert(camera.isValid() && world_T_camera.isValid());

	ocean_assert(!imagePoints.isEmpty());
	ocean_assert(distance > Scalar(0));

	Vectors3 result;
	result.reserve(imagePoints.size());

	for (size_t n = 0; n < imagePoints.size(); ++n)
	{
		const Line3 ray(camera.ray(imagePoints[n], world_T_camera));

		result.emplace_back(ray.point(distance));
	}

	return result;
}

ObjectPoints Utilities::createObjectPoints(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose, const ConstIndexedAccessor<ImagePoint>& imagePoints, const bool useDistortionParameters, const Scalar distance)
{
	const AnyCameraPinhole camera(PinholeCamera(pinholeCamera, useDistortionParameters));

	return createObjectPoints(camera, pose, imagePoints, distance);
}

void Utilities::triangulateObjectPoints(const AnyCamera& camera0, const AnyCamera& camera1, const HomogenousMatrix4& world_T_camera0, const HomogenousMatrix4& world_T_camera1, const ConstIndexedAccessor<Vector2>& imagePoints0, const ConstIndexedAccessor<Vector2>& imagePoints1, Vectors3& objectPoints, Indices32& validIndices, const bool onlyFrontPoints, const Scalar maximalSqrError)
{
	ocean_assert(camera0.isValid() && camera1.isValid());
	ocean_assert(world_T_camera0.isValid() && world_T_camera1.isValid());
	ocean_assert(imagePoints0.size() == imagePoints1.size());
	ocean_assert(imagePoints0.size() >= 1u);

	ocean_assert(objectPoints.empty() && validIndices.empty());

	objectPoints.clear();
	validIndices.clear();

	const HomogenousMatrix4 flippedCamera0_T_world(PinholeCamera::standard2InvertedFlipped(world_T_camera0));
	const HomogenousMatrix4 flippedCamera1_T_world(PinholeCamera::standard2InvertedFlipped(world_T_camera1));

	Vector3 objectPoint;

	for (size_t n = 0; n < imagePoints0.size(); ++n)
	{
		const Vector2& imagePoint0 = imagePoints0[n];
		const Vector2& imagePoint1 = imagePoints1[n];

		const Line3 ray0 = camera0.ray(imagePoint0, world_T_camera0);
		const Line3 ray1 = camera1.ray(imagePoint1, world_T_camera1);

		if (ray0.nearestPoint(ray1, objectPoint))
		{
			if (!onlyFrontPoints || (AnyCamera::isObjectPointInFrontIF(flippedCamera0_T_world, objectPoint) && PinholeCamera::isObjectPointInFrontIF(flippedCamera1_T_world, objectPoint))) // we accept any object points OR the object point lies in front of both cameras
			{
				if (maximalSqrError < 0 || (camera0.projectToImageIF(flippedCamera0_T_world, objectPoint).sqrDistance(imagePoint0) <= maximalSqrError && camera1.projectToImageIF(flippedCamera1_T_world, objectPoint).sqrDistance(imagePoint1) <= maximalSqrError)) // any error OR the projective error in both cameras is small enough
				{
					objectPoints.emplace_back(objectPoint);
					validIndices.emplace_back(Index32(n));
				}
			}
		}
	}
}

void Utilities::triangulateObjectPoints(const PinholeCamera& camera0, const PinholeCamera& camera1, const HomogenousMatrix4& pose0, const HomogenousMatrix4& pose1, const ConstIndexedAccessor<Vector2>& imagePoints0, const ConstIndexedAccessor<Vector2>& imagePoints1, Vectors3& objectPoints, Indices32& validIndices, const bool undistortImagePoints, const bool onlyFrontPoints, const Scalar maximalSqrError)
{
	const AnyCameraPinhole anyCamera0(PinholeCamera(camera0, undistortImagePoints));
	const AnyCameraPinhole anyCamera1(PinholeCamera(camera1, undistortImagePoints));

	triangulateObjectPoints(anyCamera0, anyCamera1, pose0, pose1, imagePoints0, imagePoints1, objectPoints, validIndices, onlyFrontPoints, maximalSqrError);
}

Triangles3 Utilities::backProjectTriangles(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose, const Plane3& plane, const Triangle2* triangles, const size_t numberTriangles, const bool useDistortionParameters)
{
	ocean_assert(pinholeCamera.isValid() && pose.isValid());
	ocean_assert(triangles);

	Triangles3 result;
	result.reserve(numberTriangles);

	for (size_t n = 0; n < numberTriangles; ++n)
	{
		const Triangle2& triangle = triangles[n];
		Vector3 objectPoints[3];

		for (unsigned int i = 0u; i < 3u; ++i)
		{
			const Vector2 undistortedImagePoint(useDistortionParameters ? pinholeCamera.undistort<true>(triangle[i]) : triangle[i]);
			const Line3 ray(pinholeCamera.ray(undistortedImagePoint, pose));

			if (!plane.intersection(ray, objectPoints[i]))
			{
				ocean_assert(false && "The plane is in parallel with the viewing direction!");
				return Triangles3();
			}

			ocean_assert(pinholeCamera.projectToImage<true>(pose, objectPoints[i], pinholeCamera.hasDistortionParameters()).sqrDistance(triangle[i]) < 1);
		}

		result.push_back(Triangle3(objectPoints[0], objectPoints[1], objectPoints[2]));
	}

	return result;
}

size_t Utilities::countFrontObjectPointsIF(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& invertedFlippedPose, const ObjectPoint* objectPoints, const size_t numberObjectPoints)
{
	ocean_assert_and_suppress_unused(pinholeCamera.isValid() && invertedFlippedPose.isValid(), pinholeCamera);
	ocean_assert(numberObjectPoints == 0 || objectPoints);

	size_t result = 0;

	for (size_t n = 0; n < numberObjectPoints; ++n)
	{
		if (PinholeCamera::isObjectPointInFrontIF(invertedFlippedPose, objectPoints[n]))
		{
			result++;
		}
	}

	return result;
}

size_t Utilities::countFrontObjectPoints(const PinholeCamera& cameraFirst, const PinholeCamera& cameraSecond, const HomogenousMatrix4& poseFirst, const HomogenousMatrix4& poseSecond, const ImagePoint* imagePointsFirst, const ImagePoint* imagePointsSecond, const size_t correspondences)
{
	ocean_assert(cameraFirst.isValid() && cameraSecond.isValid());
	ocean_assert(poseFirst.isValid() && poseSecond.isValid());
	ocean_assert(&poseFirst != &poseSecond);

	ocean_assert(imagePointsFirst && imagePointsSecond);

	const Vector3 translationFirst(poseFirst.translation());
	const Quaternion rotationFirst(poseFirst.rotation());
	const Vector3 viewingDirectionFirst(poseFirst.rotationMatrix(Vector3(0, 0, -1)));

	const Vector3 translationSecond(poseSecond.translation());
	const Quaternion rotationSecond(poseSecond.rotation());
	const Vector3 viewingDirectionSecond(poseSecond.rotationMatrix(Vector3(0, 0, -1)));

	size_t validPoints = 0;

	Vector3 objectPoint;
	for (size_t n = 0; n < correspondences; ++n)
	{
		if (cameraFirst.ray(imagePointsFirst[n], translationFirst, rotationFirst).nearestPoint(cameraSecond.ray(imagePointsSecond[n], translationSecond, rotationSecond), objectPoint))
		{
			const Scalar a = viewingDirectionFirst * Vector3(objectPoint - translationFirst);
			const Scalar b = viewingDirectionSecond * Vector3(objectPoint - translationSecond);

			if (a > 0 && b > 0)
			{
				validPoints++;
			}
		}
	}

	return validPoints;
}

SquareMatrix2 Utilities::covarianceMatrix(const ImagePoint* imagePoints, const size_t number, const ImagePoint& meanPoint, const Scalar minimalSigma)
{
	ocean_assert(imagePoints && number > 0);
	ocean_assert(meanPoint == meanImagePoint(ConstTemplateArrayAccessor<ImagePoint>(imagePoints, number)));

	const Scalar minimalSqrSigma = Numeric::sqr(minimalSigma);

	Scalar covXX = 0, covXY = 0, covYY = 0;
	for (size_t n = 0; n < number; ++n)
	{
		const ImagePoint& imagePoint = imagePoints[n];

		// (x - ux)^2
		covXX += max(minimalSqrSigma, Numeric::sqr(imagePoint.x() - meanPoint.x()));

		// (x - ux) * (y - uy)
		covXY += (imagePoint.x() - meanPoint.x()) * (imagePoint.y() - meanPoint.y());

		// (y - uy)^2
		covYY += max(minimalSqrSigma, Numeric::sqr(imagePoint.y() - meanPoint.y()));
	}

	const Scalar factor = 1 / Scalar(number);
	covXX *= factor;
	covXY *= factor;
	covYY *= factor;

	return SquareMatrix2(covXX, covXY, covXY, covYY);
}

SquareMatrix2 Utilities::covarianceMatrix(const ImagePoint* imagePoints, const unsigned int* indices, const size_t numberIndices, const Scalar minimalSigma, ImagePoint& meanPoint)
{
	ocean_assert(imagePoints && numberIndices > 0);

	const Scalar minimalSqrSigma = Numeric::sqr(minimalSigma);

	meanPoint = meanImagePoint(ConstTemplateArraySubsetAccessor<ImagePoint, unsigned int>(imagePoints, indices, numberIndices));

	Scalar covXX = 0, covXY = 0, covYY = 0;
	for (size_t n = 0; n < numberIndices; ++n)
	{
		const ImagePoint& imagePoint = imagePoints[indices[n]];

		// (x - ux)^2
		covXX += max(minimalSqrSigma, Numeric::sqr(imagePoint.x() - meanPoint.x()));

		// (x - ux) * (y - uy)
		covXY += (imagePoint.x() - meanPoint.x()) * (imagePoint.y() - meanPoint.y());

		// (y - uy)^2
		covYY += max(minimalSqrSigma, Numeric::sqr(imagePoint.y() - meanPoint.y()));
	}

	const Scalar factor = 1 / Scalar(numberIndices);
	covXX *= factor;
	covXY *= factor;
	covYY *= factor;

	return SquareMatrix2(covXX, covXY, covXY, covYY);
}

SquareMatrix2 Utilities::covarianceMatrix(const Vector2& direction0, const Scalar sigma0, const Vector2& direction1, const Scalar sigma1)
{
	ocean_assert(Numeric::isEqualEps(direction0 * direction1));
	ocean_assert(Numeric::isEqual(direction0.length(), 1));
	ocean_assert(Numeric::isEqual(direction1.length(), 1));

	return SquareMatrix2(Numeric::sqr(sigma0), Numeric::sqr(sigma1), direction0, direction1);
}

SquareMatrix2 Utilities::covarianceMatrix(const Vector2& direction0, const Vector2& direction1)
{
	const Scalar length0 = direction0.length();
	const Scalar length1 = direction1.length();

	ocean_assert(Numeric::isNotEqualEps(length0));
	ocean_assert(Numeric::isNotEqualEps(length1));

	return covarianceMatrix(direction0 / length0, length0, direction1 / length1, length1);
}

SquareMatrix2 Utilities::covarianceMatrix(const Vector2& direction, const Scalar minimalSigma, const Scalar minorFactor)
{
	ocean_assert(minorFactor > 0 && minorFactor <= 1);

	Scalar majorLength = direction.length();
	Vector2 majorDirection(1, 0);

	if (Numeric::isEqualEps(majorLength))
	{
		majorLength = 1;
	}
	else
	{
		majorDirection = direction / majorLength;
	}

	majorLength = max(minimalSigma, majorLength);

	const Vector2 minorDirection(majorDirection.perpendicular());
	ocean_assert(Numeric::isEqual(majorDirection.length(), 1));

	const Scalar minorLength = majorLength * minorFactor;

	return covarianceMatrix(majorDirection, majorLength, minorDirection, minorLength);
}

bool Utilities::decomposeCovarianceMatrix(const SquareMatrix2& covarianceMatrix, Vector2& direction0, Vector2& direction1)
{
	Scalar eigenValue0, eigenValue1;
	Vector2 eigenVector0, eigenVector1;

	if (!covarianceMatrix.eigenSystem(eigenValue0, eigenValue1, eigenVector0, eigenVector1))
	{
		return false;
	}

	ocean_assert(Numeric::isEqual(eigenVector0.length(), 1));
	ocean_assert(Numeric::isEqual(eigenVector1.length(), 1));

	direction0 = eigenVector0 * Numeric::sqrt(eigenValue0);
	direction1 = eigenVector1 * Numeric::sqrt(eigenValue1);
	return true;
}

bool Utilities::isPolygonConvex(const Vector2* vertices, const size_t size, const bool strict)
{
	if (size < 3)
	{
		return true;
	}

	ocean_assert(vertices != nullptr);

	Vector2 previousVector = vertices[1] - vertices[0];
	Vector2 nextVector = vertices[2] - vertices[1];

	if (strict)
	{
		const Scalar firstCrossProduct = previousVector.cross(nextVector);

		if (Numeric::isEqualEps(firstCrossProduct))
		{
			// too close to 180 deg
			return false;
		}

		for (size_t i = 1; i < size; ++i)
		{
			previousVector = nextVector;

			nextVector = Vector2(vertices[(i + 2) % size] - vertices[(i + 1) % size]);

			const Scalar nextCrossProduct = previousVector.cross(nextVector);

			if (Numeric::isEqualEps(nextCrossProduct))
			{
				// too close to 180 deg
				return false;
			}

			if (Numeric::sign(firstCrossProduct) != Numeric::sign(nextCrossProduct))
			{
				return false;
			}
		}
	}
	else
	{
		// In case the cross product of the first two segments of the polygon isn't conclusive, continue around the polygon.
		// This can happen, if neighboring segments of a polygon are located on the same line, i.e. the internal angle between them is 180 degrees.

		Scalar firstCrossProduct = previousVector.cross(nextVector);

		size_t index = 1;

		while (Numeric::isEqualEps(firstCrossProduct) && index < size)
		{
			previousVector = nextVector;

			nextVector = Vector2(vertices[(index + 2) % size] - vertices[(index + 1) % size]);

			firstCrossProduct = previousVector.cross(nextVector);

			++index;
		}

		// If no cross-product with a value |v| > eps has been found, then this polygon is a straight line, which is a border case that will be ignored.
		if (Numeric::isEqualEps(firstCrossProduct))
		{
			return false;
		}

		for (size_t i = index; i < size; ++i)
		{
			previousVector = nextVector;

			nextVector = Vector2(vertices[(i + 2) % size] - vertices[(i + 1) % size]);

			const Scalar nextCrossProduct = previousVector.cross(nextVector);

			if (Numeric::isEqualEps(nextCrossProduct))
			{
				// too close to 180 deg
				return false;
			}

			if (Numeric::sign(firstCrossProduct) != Numeric::sign(nextCrossProduct))
			{
				return false;
			}
		}
	}

	return true;
}

Scalar Utilities::computePolygonAreaSigned(const Vector2* vertices, size_t size)
{
	if (size <= 2)
	{
		return 0;
	}

	ocean_assert(vertices);

	// Shoelace formula
	Scalar area = 0;

	for (size_t i = 0; i < (size - 1); ++i)
	{
		area += Scalar(vertices[i].x() * vertices[i + 1].y());
		area -= Scalar(vertices[i].y() * vertices[i + 1].x());
	}

	area += Scalar(vertices[size - 1].x() * vertices[0].y());
	area -= Scalar(vertices[size - 1].y() * vertices[0].x());

	area *= Scalar(0.5);

	return area;
}

bool Utilities::isInsideConvexPolygon(const Vector2* vertices, size_t size, const Vector2& point)
{
	if (size < 3)
	{
		return false;
	}

	ocean_assert(vertices != nullptr);
	ocean_assert(isPolygonConvex(vertices, size, false));

	// Check on which side the test point lies relative to the first edge of the polygon.
	// If it lies on the line, it is considered as inside the polygon.
	// Otherwise store the (sign of the) side that the test point is on.
	// This sign must be identical for all edges of the convex polygon (or zero) if the point is inside the polygon.
	const Scalar firstCrossProduct = (point - vertices[0]).cross(vertices[1] - vertices[0]);

	if (Numeric::isWeakEqualEps(firstCrossProduct))
	{
		return true;
	}

	for (size_t i = 1; i < size; ++i)
	{
		const size_t nextIndex = (i + 1) % size;
		const Scalar currentCrossProduct = (point - vertices[i]).cross(vertices[nextIndex] - vertices[i]);

		if (Numeric::isWeakEqualEps(currentCrossProduct))
		{
			return true;
		}

		if (Numeric::sign(firstCrossProduct) != Numeric::sign(currentCrossProduct))
		{
			return false;
		}
	}

	return true;
}

bool Utilities::intersectConvexPolygons(const Vector2* vertices0, const size_t size0, const Vector2* vertices1, const size_t size1, Vectors2& intersectedPolygon)
{
	if (size0 < 3 || size1 < 3)
	{
		return false;
	}

	ocean_assert(vertices0 != nullptr && vertices1 != nullptr);
	ocean_assert(isPolygonConvex(vertices0, size0, false));
	ocean_assert(isPolygonConvex(vertices1, size1, false));

	// The vertices can be in clockwise or counter-clockwise order.
	// Determine the half-plane of a point that is guaranteed to be inside the first polygon.
	// That information is used below to determine whether testing points are on that side of a clipping line that face to the inside of the first polygon.

	const Vector2 centerOfGravity = std::accumulate(vertices0, vertices0 + size0, Vector2(0, 0)) / Scalar(size0);
	ocean_assert(isInsideConvexPolygon(vertices0, size0, centerOfGravity));

	const FiniteLine2 firstEdge(vertices0[0], vertices0[1]);

	const bool insideIsLeftHalfPlane = firstEdge.isLeftOfLine(centerOfGravity);

	// Sutherland-Hodgman
	Vectors2 intersection(vertices1, vertices1 + size1);

	for (size_t i = 0; i < size0; ++i)
	{
		const Vectors2 verticesToCheck = std::move(intersection);
		intersection = Vectors2();

		const FiniteLine2 clippingLine(vertices0[i], vertices0[(i + 1) % size0]);

		ocean_assert(!verticesToCheck.empty());
		Vector2 firstPoint = verticesToCheck.back();
		bool firstPointInside = clippingLine.isLeftOfLine(firstPoint) == insideIsLeftHalfPlane;

		for (size_t j = 0; j < verticesToCheck.size(); ++j)
		{
			const Vector2 secondPoint = verticesToCheck[j];
			const bool secondPointInside = clippingLine.isLeftOfLine(secondPoint) == insideIsLeftHalfPlane;

			if (secondPointInside)
			{
				if (!firstPointInside)
				{
					Vector2 intersectionPoint;
					if (!clippingLine.intersection(FiniteLine2(firstPoint, secondPoint), intersectionPoint))
					{
						return false;
					}

					intersection.emplace_back(intersectionPoint);
				}

				intersection.emplace_back(secondPoint);
			}
			else if (firstPointInside)
			{
				Vector2 intersectionPoint;
				if (!clippingLine.intersection(FiniteLine2(firstPoint, secondPoint), intersectionPoint))
				{
					return false;
				}

				intersection.emplace_back(intersectionPoint);
			}

			firstPoint = secondPoint;
			firstPointInside = secondPointInside;
		}
	}

	// Discard border cases, e.g., intersections consisting of single points or lines (adjacent polygons).

	if (!isPolygonConvex(intersection.data(), intersection.size()))
	{
		return false;
	}

	intersectedPolygon = std::move(intersection);

	return true;
}

SquareMatrix3 Utilities::createRandomHomography(const unsigned int width, const unsigned height, const Scalar maxTranslation)
{
	ocean_assert(width >= 1u && height >= 1u);

	const Vectors2 inputPoints =
	{
		Vector2(Scalar(0), Scalar(0)),
		Vector2(Scalar(width), Scalar(0)),
		Vector2(Scalar(width), Scalar(height)),
		Vector2(Scalar(0), Scalar(height))
	};

	const Scalar maxOffset = Scalar(0.1) * Scalar(std::min(Scalar(width), Scalar(height)));
	ocean_assert(maxOffset > Scalar(0));
	const Vector2 translate(Random::scalar(-maxTranslation, maxTranslation), Random::scalar(-maxTranslation, maxTranslation));

	const Vectors2 outputPoints =
	{
		inputPoints[0] + Random::vector2(-maxOffset, maxOffset) + translate,
		inputPoints[1] + Random::vector2(-maxOffset, maxOffset) + translate,
		inputPoints[2] + Random::vector2(-maxOffset, maxOffset) + translate,
		inputPoints[3] + Random::vector2(-maxOffset, maxOffset) + translate,
	};

	SquareMatrix3 transformation;
	Ocean::Geometry::Homography::homographyMatrix(inputPoints.data(), outputPoints.data(), inputPoints.size(), transformation);
	ocean_assert(transformation.isHomography());

	return transformation;
};

HomogenousMatrix4 Utilities::randomCameraPose(const PinholeCamera& pinholeCamera, const Line3& worldObjectPointRay, const Vector2& distortedImagePoint, const Scalar distance)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(worldObjectPointRay.isValid());
	ocean_assert(pinholeCamera.isInside(distortedImagePoint));
	ocean_assert(distance > Numeric::eps());

	// the center of projection of the camera, will be the translational part of the camera pose
	const Vector3 cameraTranslation = worldObjectPointRay.point(distance);

	// viewing vector in relation to identity camera pose
	const Vector3 viewingVector = pinholeCamera.vector(pinholeCamera.undistort<true>(distortedImagePoint));

	// now we need to find a rotation of the camera so that the viewing vector is perpendicular with the ray of the object point

	const Quaternion pointRay_R_viewingVector(-viewingVector, worldObjectPointRay.direction());

	ocean_assert(pinholeCamera.projectToImage<true>(HomogenousMatrix4(cameraTranslation, pointRay_R_viewingVector), worldObjectPointRay.point(), pinholeCamera.hasDistortionParameters()).sqrDistance(distortedImagePoint) < Numeric::sqr(1));
	ocean_assert(PinholeCamera::isObjectPointInFrontIF(PinholeCamera::standard2InvertedFlipped(HomogenousMatrix4(cameraTranslation, pointRay_R_viewingVector)), worldObjectPointRay.point()));

	// we have one degree of freedom left: the rotation around the viewing vector

	const Quaternion rotationAroundViewingVector(viewingVector, Random::scalar(0, Numeric::pi2()));

	const HomogenousMatrix4 world_T_camera = HomogenousMatrix4(cameraTranslation, pointRay_R_viewingVector * rotationAroundViewingVector);

	ocean_assert(pinholeCamera.projectToImage<true>(world_T_camera, worldObjectPointRay.point(), pinholeCamera.hasDistortionParameters()).sqrDistance(distortedImagePoint) < Numeric::sqr(1));
	ocean_assert(PinholeCamera::isObjectPointInFrontIF(PinholeCamera::standard2InvertedFlipped(world_T_camera), worldObjectPointRay.point()));

	return world_T_camera;
}

HomogenousMatrix4 Utilities::randomCameraPose(const FisheyeCamera& fisheyeCamera, const Line3& worldObjectPointRay, const Vector2& distortedImagePoint, const Scalar distance)
{
	ocean_assert(fisheyeCamera.isValid());
	ocean_assert(worldObjectPointRay.isValid());
	ocean_assert(fisheyeCamera.isInside(distortedImagePoint));
	ocean_assert(distance > Numeric::eps());

	// the center of projection of the camera, will be the translational part of the camera pose
	const Vector3 cameraTranslation = worldObjectPointRay.point(distance);

	// viewing vector in relation to identity camera pose
	const Vector3 viewingVector = fisheyeCamera.vector<true>(distortedImagePoint);

	// now we need to find a rotation of the camera so that the viewing vector is perpendicular with the ray of the object point

	const Quaternion pointRay_R_viewingVector(-viewingVector, worldObjectPointRay.direction());

	ocean_assert(fisheyeCamera.projectToImage(HomogenousMatrix4(cameraTranslation, pointRay_R_viewingVector), worldObjectPointRay.point()).sqrDistance(distortedImagePoint) < Numeric::sqr(1));
	ocean_assert(PinholeCamera::isObjectPointInFrontIF(PinholeCamera::standard2InvertedFlipped(HomogenousMatrix4(cameraTranslation, pointRay_R_viewingVector)), worldObjectPointRay.point()));

	// we have one degree of freedom left: the rotation around the viewing vector

	const Quaternion rotationAroundViewingVector(viewingVector, Random::scalar(0, Numeric::pi2()));

	const HomogenousMatrix4 world_T_camera = HomogenousMatrix4(cameraTranslation, pointRay_R_viewingVector * rotationAroundViewingVector);

	ocean_assert(fisheyeCamera.projectToImage(world_T_camera, worldObjectPointRay.point()).sqrDistance(distortedImagePoint) < Numeric::sqr(1));
	ocean_assert(PinholeCamera::isObjectPointInFrontIF(PinholeCamera::standard2InvertedFlipped(world_T_camera), worldObjectPointRay.point()));

	return world_T_camera;
}

HomogenousMatrix4 Utilities::randomCameraPose(const AnyCamera& anyCamera, const Line3& worldObjectPointRay, const Vector2& distortedImagePoint, const Scalar distance)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(worldObjectPointRay.isValid());
	ocean_assert(anyCamera.isInside(distortedImagePoint));
	ocean_assert(distance > Numeric::eps());

	// the center of projection of the camera, will be the translational part of the camera pose
	const Vector3 cameraTranslation = worldObjectPointRay.point(distance);

	// viewing vector in relation to identity camera pose
	const Vector3 viewingVector = anyCamera.vector(distortedImagePoint);

	// now we need to find a rotation of the camera so that the viewing vector is perpendicular with the ray of the object point

	const Quaternion pointRay_R_viewingVector(-viewingVector, worldObjectPointRay.direction());

	ocean_assert(anyCamera.projectToImage(HomogenousMatrix4(cameraTranslation, pointRay_R_viewingVector), worldObjectPointRay.point()).sqrDistance(distortedImagePoint) < Numeric::sqr(1));
	ocean_assert(PinholeCamera::isObjectPointInFrontIF(PinholeCamera::standard2InvertedFlipped(HomogenousMatrix4(cameraTranslation, pointRay_R_viewingVector)), worldObjectPointRay.point()));

	// we have one degree of freedom left: the rotation around the viewing vector

	const Quaternion rotationAroundViewingVector(viewingVector, Random::scalar(0, Numeric::pi2()));

	const HomogenousMatrix4 world_T_camera = HomogenousMatrix4(cameraTranslation, pointRay_R_viewingVector * rotationAroundViewingVector);

	ocean_assert(anyCamera.projectToImage(world_T_camera, worldObjectPointRay.point()).sqrDistance(distortedImagePoint) < Numeric::sqr(1));
	ocean_assert(PinholeCamera::isObjectPointInFrontIF(PinholeCamera::standard2InvertedFlipped(world_T_camera), worldObjectPointRay.point()));

	return world_T_camera;
}

} // namespace Geometry

} // namespace Ocean
