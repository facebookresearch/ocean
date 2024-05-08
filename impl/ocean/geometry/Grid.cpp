/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/geometry/Grid.h"

#include "ocean/geometry/Utilities.h"
#include "ocean/geometry/VanishingProjection.h"

namespace Ocean
{

namespace Geometry
{

Grid::Grid(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose, const Vectors2& imagePoints, const HomogenousMatrix4& plane2worldTransformation, const HomogenousMatrix4& virtualGrid2planeTransformation, const bool clipBoundingBox) :
	worldTplane(plane2worldTransformation),
	gridLowerCorner(Vector2(-0.5, -0.5)),
	gridUpperCorner(Vector2(0.5, 0.5))
{
	// sets the physical and virtual grid2plane transformation
	setVirtualGrid2planeTransformation(virtualGrid2planeTransformation);

	ocean_assert(pinholeCamera.isValid() && pose.isValid());
	ocean_assert(imagePoints.size() == 4);

	if (!plane2worldTransformation.isValid() || !virtualGrid2planeTransformation.isValid() || !Geometry::Utilities::isPolygonConvex(imagePoints.data(), imagePoints.size()))
		return;

	Box2 boundingBox(Vector2(0, 0), Vector2(0, 0));

	Vector2 gridCoordinates[4];
	for (unsigned int i = 0u; i < 4u; ++i)
	{
		Vector2 gridCoordinate;
		if (!image2grid<true>(pinholeCamera, pose, imagePoints[i], gridCoordinate))
			return;

		gridCoordinates[i] = gridCoordinate;
	}

	Scalar borderSize = 0;
	for (unsigned int i = 0u; i < 4u; ++i)
		borderSize += gridCoordinates[(i + 1u) % 4u].distance(gridCoordinates[i]);

	// average half distance between two image points
	borderSize /= Scalar(8);

	for (unsigned int i = 0u; i < 4u; ++i)
	{
		boundingBox += gridCoordinates[i] + Vector2(1, 0) * borderSize;
		boundingBox += gridCoordinates[i] + Vector2(-1, 0) * borderSize;
		boundingBox += gridCoordinates[i] + Vector2(0, 1) * borderSize;
		boundingBox += gridCoordinates[i] + Vector2(0, -1) * borderSize;
	}

	gridLowerCorner = boundingBox.lower();
	gridUpperCorner = boundingBox.higher();

	if (clipBoundingBox)
		clipByImageBoundingBox(pinholeCamera, pose);

	ocean_assert(isValid());
}

Grid::Grid(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose, const Vectors2& imagePoints, const Vector2* gridCoordinates, const HomogenousMatrix4* previousPlane2worldTransformation, const bool clipBoundingBox)
{
	ocean_assert(pinholeCamera.isValid() && pose.isValid());
	ocean_assert(imagePoints.size() == 4);

	HomogenousMatrix4 newPlane2worldTransformation(true);
	HomogenousMatrix4 newVirtualGrid2planeTransformation(true);

	if (!determineVirtualGridTransformationsByFourImagePoints(pinholeCamera, pose, imagePoints.data(), gridCoordinates, previousPlane2worldTransformation, &newPlane2worldTransformation, gridCoordinates ? &newVirtualGrid2planeTransformation : nullptr))
	{
		// we could not determine a plane2world transformation from the given four image points
		*this = Grid();
		return;
	}

	if (gridCoordinates)
	{
		// we apply a fix scale to the virtual grid transformation
		// **TODO** fix scale values

		HomogenousMatrix4 scaleFixVirtualGrid2planeTransformation(newVirtualGrid2planeTransformation.xAxis().normalizedOrZero(), newVirtualGrid2planeTransformation.yAxis().normalizedOrZero(), newVirtualGrid2planeTransformation.zAxis().normalizedOrZero(), Vector3(0, 0, 0));
		scaleFixVirtualGrid2planeTransformation.applyScale(Vector3(Scalar(0.05), Scalar(0.05), Scalar(0.05)));

		newVirtualGrid2planeTransformation = scaleFixVirtualGrid2planeTransformation;
	}

	// translate real plane to the intersection point of the diagonal of the rectangle or to the center of mass of the image points if the intersection point could not be determined
	const FiniteLine2 diagonal0(imagePoints[0], imagePoints[2]);
	const FiniteLine2 diagonal1(imagePoints[1], imagePoints[3]);

	Vector2 centerImagePoint;
	if (!diagonal0.intersection(diagonal1, centerImagePoint))
		centerImagePoint = Geometry::Utilities::meanImagePoint(ConstTemplateArrayAccessor<Vector2>(imagePoints));

	Vector3 centerOfMassWorldCoordinate;

	const Vector2 undistortedImageCoordinate = pinholeCamera.undistortDamped(centerImagePoint);
	const Line3 ray = pinholeCamera.ray(undistortedImageCoordinate, pose);

	if (Plane3(newPlane2worldTransformation).intersection(ray, centerOfMassWorldCoordinate))
	{
		newPlane2worldTransformation.setTranslation(centerOfMassWorldCoordinate);
		newVirtualGrid2planeTransformation.setTranslation(Vector3(0, 0, 0));
	}

	*this = Grid(newPlane2worldTransformation, newVirtualGrid2planeTransformation);

	if (clipBoundingBox)
		clipByImageBoundingBox(pinholeCamera, pose);

	ocean_assert(isValid());
}

Grid::Grid(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose, const HomogenousMatrix4& plane2worldTransformation, const Vectors2& imagePoints, const bool clipBoundingBox)
{
	ocean_assert(pinholeCamera.isValid() && pose.isValid());
	ocean_assert(plane2worldTransformation.isValid());
	ocean_assert(imagePoints.size() == 4);

	HomogenousMatrix4 newPlane2worldTransformation(true);
	HomogenousMatrix4 newVirtualGrid2planeTransformation(true);

	if (!determineVirtualGridTransformationByPlane2WorldTransformation(pinholeCamera, pose, imagePoints, plane2worldTransformation, newPlane2worldTransformation, newVirtualGrid2planeTransformation))
	{
		// we could not determine a virtual grid transformation from the given four image points
		*this = Grid();
		return;
	}

	*this = Grid(newPlane2worldTransformation, newVirtualGrid2planeTransformation);

	if (clipBoundingBox)
		clipByImageBoundingBox(pinholeCamera, pose);

	ocean_assert(isValid());
}

HomogenousMatrix4 Grid::virtualPlane2worldTransformation() const
{
	ocean_assert(isValid());

	// we assign the possible shear of the virtual grid to the plane transformation

	const Vector3 normalizedVirtualGridXAxis = planeTvirtualGrid.xAxis().normalizedOrZero();
	const Vector3 normalizedVirtualGridYAxis = planeTvirtualGrid.yAxis().normalizedOrZero();

	if (Numeric::isEqualEps(normalizedVirtualGridXAxis * normalizedVirtualGridYAxis))
		return worldTplane;

	Vector3 virtualGridHalfAxis = normalizedVirtualGridXAxis + normalizedVirtualGridYAxis;
	if (!virtualGridHalfAxis.normalize())
	{
		ocean_assert(false && "This must never happen!");
		return worldTplane;
	}

	const Vector3 previousXAxis = worldTplane.xAxis();
	const Vector3 previousYAxis = worldTplane.yAxis();

	const Scalar previousXAxisLength = previousXAxis.length();
	const Scalar previousYAxisLength = previousYAxis.length();

	if (Numeric::isEqualEps(previousXAxisLength) || Numeric::isEqualEps(previousYAxisLength))
	{
		ocean_assert(false && "This must never happen!");
		return worldTplane;
	}

	Vector3 newHalfAxis = previousXAxis / previousXAxisLength + previousYAxis / previousYAxisLength;
	if (!newHalfAxis.normalize())
	{
		ocean_assert(false && "This must never happen!");
		return worldTplane;
	}

	const Vector3 newZAxis = worldTplane.zAxis();

	const Vector3 newXAxis = (Rotation(newZAxis, -virtualGridHalfAxis.angle(normalizedVirtualGridXAxis)) * newHalfAxis) * previousXAxisLength;
	const Vector3 newYAxis = (Rotation(newZAxis, virtualGridHalfAxis.angle(normalizedVirtualGridYAxis)) * newHalfAxis) * previousYAxisLength;

	return HomogenousMatrix4(newXAxis, newYAxis, newZAxis, worldTplane.translation());
}

bool Grid::setVirtualGrid2planeTransformation(const HomogenousMatrix4& newTransformation)
{
	ocean_assert(newTransformation.isValid());

	if (!newTransformation.isValid() || newTransformation.xAxis().isParallel(newTransformation.yAxis())
		|| !newTransformation.xAxis().isOrthogonal(newTransformation.zAxis()) || !newTransformation.yAxis().isOrthogonal(newTransformation.zAxis()))
		return false;

	planeTvirtualGrid = newTransformation;

	planeTgrid = HomogenousMatrix4(planeTvirtualGrid.translation());

	// we set the scale of the physical plane2grid transformation to the average of the x and y scale of the virtual transformation
	const Scalar scale = (planeTvirtualGrid.scale().x() + planeTvirtualGrid.scale().y()) * Scalar(0.5);
	planeTgrid.applyScale(Vector3(scale, scale, scale));

	return true;
}

bool Grid::isValid() const
{
	return worldTplane.isValid() && worldTplane.xAxis().isOrthogonal(worldTplane.yAxis()) && worldTplane.xAxis().isOrthogonal(worldTplane.zAxis()) && worldTplane.yAxis().isOrthogonal(worldTplane.zAxis())
		&& planeTgrid.isValid() && planeTgrid.xAxis().isOrthogonal(planeTgrid.yAxis()) && planeTgrid.xAxis().isOrthogonal(planeTgrid.zAxis()) && planeTgrid.yAxis().isOrthogonal(planeTgrid.zAxis())
		&& planeTvirtualGrid.isValid() && !planeTvirtualGrid.xAxis().isParallel(planeTvirtualGrid.yAxis()) && planeTvirtualGrid.xAxis().isOrthogonal(planeTvirtualGrid.zAxis()) && planeTvirtualGrid.yAxis().isOrthogonal(planeTvirtualGrid.zAxis())
		&& gridLowerCorner.x() < gridUpperCorner.x() && gridLowerCorner.y() < gridUpperCorner.y();
}

void Grid::setInvalid()
{
	worldTplane = HomogenousMatrix4(false);
	planeTgrid = HomogenousMatrix4(false);
	planeTvirtualGrid = HomogenousMatrix4(false);

	gridLowerCorner = Vector2(1, 1);
	gridUpperCorner = Vector2(-1, -1);
}

bool Grid::areVirtualAndPhysicalGridIdentical() const
{
	const Vector3 differenceXAxis = planeTgrid.xAxis().normalizedOrZero() - planeTvirtualGrid.xAxis().normalizedOrZero();
	const Vector3 differenceYAxis = planeTgrid.yAxis().normalizedOrZero() - planeTvirtualGrid.yAxis().normalizedOrZero();

	return Numeric::isWeakEqualEps(differenceXAxis.x()) && Numeric::isWeakEqualEps(differenceXAxis.y()) && Numeric::isWeakEqualEps(differenceXAxis.z()) &&
		Numeric::isWeakEqualEps(differenceYAxis.x()) && Numeric::isWeakEqualEps(differenceYAxis.y()) && Numeric::isWeakEqualEps(differenceYAxis.z());
}

bool Grid::image2world(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose, const Vector2& imageCoordinate, Vector3& worldCoordinate) const
{
	ocean_assert(isValid());
	ocean_assert(pinholeCamera.isValid() && pose.isValid());

	const Vector2 undistortedImageCoordinate = pinholeCamera.undistortDamped(imageCoordinate);
	const Line3 ray = pinholeCamera.ray(undistortedImageCoordinate, pose);

	if (Plane3(worldTplane).intersection(ray, worldCoordinate))
		return true;

	return false;
}

bool Grid::image2plane(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose, const Vector2& imageCoordinate, Vector3& planeCoordinate) const
{
	ocean_assert(isValid());
	ocean_assert(pinholeCamera.isValid() && pose.isValid());

	Vector3 worldCoordinate;
	if (!image2world(pinholeCamera, pose, imageCoordinate, worldCoordinate))
		return false;

	planeCoordinate = world2plane(worldCoordinate);

	return true;
}

void Grid::clipByImageBoundingBox(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose, const Scalar borderSizeFactor, const Vectors2& insideGridCoordinates)
{
	ocean_assert(pinholeCamera.isValid() && pose.isValid());

	const HomogenousMatrix4 poseIF(PinholeCamera::standard2InvertedFlipped(pose));

	const Scalar borderWidth = Scalar(pinholeCamera.width()) * borderSizeFactor;
	const Scalar borderHeight = Scalar(pinholeCamera.height()) * borderSizeFactor;

	const Scalar leftBorder = -borderWidth;
	const Scalar topBorder = -borderHeight;
	const Scalar rightBorder = Scalar(pinholeCamera.width()) + borderWidth;
	const Scalar bottomBorder = Scalar(pinholeCamera.height()) + borderHeight;

	const FiniteLine2 boundingBoxTopEdge(Vector2(leftBorder, topBorder), Vector2(rightBorder, topBorder));
	const FiniteLine2 boundingBoxRightEdge(Vector2(rightBorder, topBorder), Vector2(rightBorder, bottomBorder));
	const FiniteLine2 boundingBoxBottomEdge(Vector2(rightBorder, bottomBorder), Vector2(leftBorder, bottomBorder));
	const FiniteLine2 boundingBoxLeftEdge(Vector2(leftBorder, bottomBorder), Vector2(leftBorder, topBorder));

	/*
	* 0-------1
	* |       |
	* |       |
	* |       |
	* 3-------2
	*/
	const Vector3 cornersWorldCoordinates[4] =
	{
		grid2world<true>(Vector2(gridLowerCorner.x(), gridUpperCorner.y())),
		grid2world<true>(gridUpperCorner),
		grid2world<true>(Vector2(gridUpperCorner.x(), gridLowerCorner.y())),
		grid2world<true>(gridLowerCorner)
	};

	/*
	* o---0---o
	* |       |
	* 2       3
	* |       |
	* o---1---o
	*/
	const FiniteLine3 edgesWorldCoordinates[4] =
	{
		FiniteLine3(cornersWorldCoordinates[0], cornersWorldCoordinates[1]),
		FiniteLine3(cornersWorldCoordinates[3], cornersWorldCoordinates[2]),
		FiniteLine3(cornersWorldCoordinates[3], cornersWorldCoordinates[0]),
		FiniteLine3(cornersWorldCoordinates[2], cornersWorldCoordinates[1])
	};

	FiniteLine2 edgesImageCoordinates[4];

	for (unsigned int i = 0u; i < 4u; ++i)
	{
		FiniteLine3 clippedLine;
		if (clipWorldLineInFrontOfCameraBinarySearchIF(poseIF, edgesWorldCoordinates[i], clippedLine))
			edgesImageCoordinates[i] = FiniteLine2(world2imageIF(pinholeCamera, poseIF, clippedLine.point0()), world2imageIF(pinholeCamera, poseIF, clippedLine.point1()));
	}

	for (unsigned int i = 0u; i < 2u; ++i)
	{
		adjustGridCornersInXDirectionByImageLines(pinholeCamera, pose, edgesImageCoordinates[i], boundingBoxTopEdge);
		adjustGridCornersInXDirectionByImageLines(pinholeCamera, pose, edgesImageCoordinates[i], boundingBoxRightEdge);
		adjustGridCornersInXDirectionByImageLines(pinholeCamera, pose, edgesImageCoordinates[i], boundingBoxBottomEdge);
		adjustGridCornersInXDirectionByImageLines(pinholeCamera, pose, edgesImageCoordinates[i], boundingBoxLeftEdge);
	}

	for (unsigned int i = 2u; i < 4u; ++i)
	{
		adjustGridCornersInYDirectionByImageLines(pinholeCamera, pose, edgesImageCoordinates[i], boundingBoxTopEdge);
		adjustGridCornersInYDirectionByImageLines(pinholeCamera, pose, edgesImageCoordinates[i], boundingBoxRightEdge);
		adjustGridCornersInYDirectionByImageLines(pinholeCamera, pose, edgesImageCoordinates[i], boundingBoxBottomEdge);
		adjustGridCornersInYDirectionByImageLines(pinholeCamera, pose, edgesImageCoordinates[i], boundingBoxLeftEdge);
	}

	if (insideGridCoordinates.size() > 0)
	{
		// if we have given grid coordinates, we must extend the determined grid bounding box to ensure
		// that all given coordinates lie inside the grid

		Box2 gridBoundingBox(gridLowerCorner, gridUpperCorner);

		for (size_t i = 0; i < insideGridCoordinates.size(); ++i)
			gridBoundingBox += insideGridCoordinates[i];

		gridLowerCorner = gridBoundingBox.lower();
		gridUpperCorner = gridBoundingBox.higher();
	}
}

bool Grid::determineVirtualGridTransformationsByFourImagePoints(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose, const Vector2* imagePoints, const Vector2* gridCoordinates, const HomogenousMatrix4* previousPlane2worldTransformation, HomogenousMatrix4* plane2worldTransformation, HomogenousMatrix4* virtualGrid2planeTransformation)
{
	ocean_assert(pinholeCamera.isValid() && pose.isValid());
	ocean_assert(imagePoints);
	ocean_assert((virtualGrid2planeTransformation && gridCoordinates) || (!virtualGrid2planeTransformation && !gridCoordinates));

	if (!Geometry::Utilities::isPolygonConvex(imagePoints, 4))
		return false;

	// we determine the normal of the plane in the coordinate system of the camera's pose
	Vector3 planeNormal;
	if (!Geometry::VanishingProjection::planeNormal(pinholeCamera, imagePoints, pinholeCamera.hasDistortionParameters(), planeNormal))
		return false;

	HomogenousMatrix4 invPose(pose);

	if (!invPose.invert())
		return false;

	// we need the normal of the plane in the world coordinate system
	planeNormal = invPose.transposed() * planeNormal;
	if (!planeNormal.normalize())
		return false;

	// we check whether the direction of the plane normal must be inverted
	Line3 imagePointRays[4];
	for (unsigned int i = 0u; i < 4u; ++i)
		imagePointRays[i] = pinholeCamera.ray(pinholeCamera.undistortDamped(imagePoints[i]), pose);

	if (imagePointRays[0].direction() * planeNormal > 0 && imagePointRays[1].direction() * planeNormal > 0 && imagePointRays[2].direction() * planeNormal > 0 && imagePointRays[3].direction() * planeNormal > 0)
		planeNormal = -planeNormal;

	Vector3 pointOnPlane;
	if (previousPlane2worldTransformation)
	{
		// if we have an old transformation, we assume its translation
		pointOnPlane = previousPlane2worldTransformation->translation();
	}
	else
	{
		// if we have no given translation, we set the translation of the new plane to the point that is the intersection of the diagonals of the four given points at a distance of 1 from the camera center

		const FiniteLine2 diagonal0(imagePoints[0], imagePoints[2]);
		const FiniteLine2 diagonal1(imagePoints[1], imagePoints[3]);

		Vector2 centerImagePoint;
		if (!diagonal0.intersection(diagonal1, centerImagePoint))
			centerImagePoint = Geometry::Utilities::meanImagePoint(ConstTemplateArrayAccessor<Vector2>(imagePoints, 4));

		const Line3 rayCenterImagePoint = pinholeCamera.ray(pinholeCamera.undistortDamped(centerImagePoint), pose);

		pointOnPlane = rayCenterImagePoint.point(1);
	}

	const Plane3 plane(pointOnPlane, planeNormal);

	Indices32 frontObjectPointIndices;
	const Vectors3 rectanglePointsWorldCoordinates = Geometry::Utilities::backProjectImagePointsDamped(pinholeCamera, pose, plane, imagePoints, 4u, true, &frontObjectPointIndices);

	// we need all back-projected object points in front of the camera
	if (frontObjectPointIndices.size() != 4)
		return false;

	if (plane2worldTransformation || virtualGrid2planeTransformation)
	{
		Vector3 direction01 = rectanglePointsWorldCoordinates[1] - rectanglePointsWorldCoordinates[0];
		Vector3 direction12 = rectanglePointsWorldCoordinates[2] - rectanglePointsWorldCoordinates[1];

		if (!direction01.normalize() || !direction12.normalize())
			return false;

		const Vector3 yAxis = (-direction12.cross(planeNormal));

		HomogenousMatrix4 newWorldTplane = HomogenousMatrix4(direction12, yAxis, planeNormal, pointOnPlane);

		if (virtualGrid2planeTransformation)
		{
			// ensure that neighboring grid coordinates are located on the same grid line and ensure that neighboring grid coordinates are not identical
			ocean_assert(((Numeric::isEqual(gridCoordinates[0].y(), gridCoordinates[1].y()) && Numeric::isEqual(gridCoordinates[1].x(), gridCoordinates[2].x()) && Numeric::isEqual(gridCoordinates[2].y(), gridCoordinates[3].y()) && Numeric::isEqual(gridCoordinates[3].x(), gridCoordinates[0].x()))
				|| (Numeric::isEqual(gridCoordinates[0].x(), gridCoordinates[1].x()) && Numeric::isEqual(gridCoordinates[1].y(), gridCoordinates[2].y()) && Numeric::isEqual(gridCoordinates[2].x(), gridCoordinates[3].x()) && Numeric::isEqual(gridCoordinates[3].y(), gridCoordinates[0].y())))
				&& gridCoordinates[0] != gridCoordinates[1] && gridCoordinates[0] != gridCoordinates[2] && gridCoordinates[0] != gridCoordinates[3]
				&& gridCoordinates[1] != gridCoordinates[2] && gridCoordinates[1] != gridCoordinates[3]
				&& gridCoordinates[2] != gridCoordinates[3]);

			if (!Geometry::Utilities::isPolygonConvex(gridCoordinates, 4))
				return false;

			HomogenousMatrix4 invNewWorldTplane(newWorldTplane);

			if (!invNewWorldTplane.invert())
				return false;

			Vector3 virtualXAxis = invNewWorldTplane * (newWorldTplane.translation() + direction12);
			Vector3 virtualYAxis = invNewWorldTplane * (newWorldTplane.translation() - direction01);

			if (!virtualXAxis.normalize() || !virtualYAxis.normalize())
				return false;

			Vector3 bisectingLineVirtualPlane = (virtualXAxis + virtualYAxis);

			if (!bisectingLineVirtualPlane.normalize())
				return false;

			// we rotate the plane2world transformation that the vector (1, 1) is equal to the bisecting line of the x and y axis of the virtual grid
			const Rotation rot(Vector3(1, 1, 0).normalizedOrZero(), bisectingLineVirtualPlane);
			newWorldTplane = newWorldTplane * HomogenousMatrix4(rot);

			const HomogenousMatrix4 newPlaneTvirtualGrid = HomogenousMatrix4(-rot * virtualXAxis, -rot * virtualYAxis, Vector3(0, 0, 1), Vector3(0, 0, 0));

			HomogenousMatrix4 invNewPlaneTvirtualGrid(newPlaneTvirtualGrid);
			invNewWorldTplane = newWorldTplane;

			if (!invNewWorldTplane.invert() || !invNewPlaneTvirtualGrid.invert())
				return false;

			const HomogenousMatrix4 newWorldTplaneI = invNewWorldTplane;
			const HomogenousMatrix4 newPlaneTvirtualGridI = invNewPlaneTvirtualGrid;

			Vector2 newGridCoordinates[4];
			for (unsigned int i = 0u; i < 4u; ++i)
			{
				const Vector2 undistortedImageCoordinate = pinholeCamera.undistortDamped(imagePoints[i]);
				const Line3 ray = pinholeCamera.ray(undistortedImageCoordinate, pose);

				Vector3 worldCoordinate;
				if (!Plane3(newWorldTplane).intersection(ray, worldCoordinate))
					return false;

				newGridCoordinates[i] = (newPlaneTvirtualGridI * (newWorldTplaneI * worldCoordinate)).xy();
			}

			const Scalar previousLengthR1 = (gridCoordinates[1] - gridCoordinates[0]).length();
			const Scalar previousLengthR2 = (gridCoordinates[3] - gridCoordinates[0]).length();

			const Scalar currentLengthR1 = (newGridCoordinates[1] - newGridCoordinates[0]).length();
			const Scalar currentLengthR2 = (newGridCoordinates[3] - newGridCoordinates[0]).length();

			if (Numeric::isEqualEps(previousLengthR1) || Numeric::isEqualEps(previousLengthR2) || Numeric::isEqualEps(currentLengthR1) || Numeric::isEqualEps(currentLengthR2))
				return false;

			const Scalar scaleR1 = currentLengthR2 / previousLengthR2;
			const Scalar scaleR2 = currentLengthR1 / previousLengthR1;

			*virtualGrid2planeTransformation = newPlaneTvirtualGrid * HomogenousMatrix4(Vector3(newGridCoordinates[0] - gridCoordinates[0], 0)) * HomogenousMatrix4(Vector3(gridCoordinates[0], 0)) * HomogenousMatrix4(true).applyScale(Vector3(scaleR1, scaleR2, 1)) * HomogenousMatrix4(-Vector3(gridCoordinates[0], 0));
		}

		if (plane2worldTransformation)
			*plane2worldTransformation = newWorldTplane;
	}

	return true;
}

bool Grid::determineVirtualGridTransformationByPlane2WorldTransformation(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose, const Vectors2& imagePoints, const HomogenousMatrix4& previousPlane2worldTransformation, HomogenousMatrix4& plane2worldTransformation, HomogenousMatrix4& virtualGrid2planeTransformation)
{
	ocean_assert(pinholeCamera.isValid() && pose.isValid());
	ocean_assert(imagePoints.size() == 4);

	if (!Geometry::Utilities::isPolygonConvex(imagePoints.data(), imagePoints.size()))
		return false;

	Indices32 frontObjectPointIndices;
	const Vectors3 rectanglePointsWorldCoordinates = Geometry::Utilities::backProjectImagePointsDamped(pinholeCamera, pose, Plane3(previousPlane2worldTransformation), imagePoints.data(), 4u, true, &frontObjectPointIndices);

	// we need all back-projected object points in front of the camera
	if (frontObjectPointIndices.size() != 4)
		return false;

	Vector3 direction10 = rectanglePointsWorldCoordinates[0] - rectanglePointsWorldCoordinates[1];
	Vector3 direction23 = rectanglePointsWorldCoordinates[3] - rectanglePointsWorldCoordinates[2];

	Vector3 direction12 = rectanglePointsWorldCoordinates[2] - rectanglePointsWorldCoordinates[1];
	Vector3 direction03 = rectanglePointsWorldCoordinates[3] - rectanglePointsWorldCoordinates[0];

	if (!direction10.normalize() || !direction23.normalize() || !direction12.normalize() || !direction03.normalize())
		return false;

	Vector3 bisectingLineXDirection = direction12 + direction03;
	Vector3 bisectingLineYDirection = direction10 + direction23;

	if (!bisectingLineXDirection.normalize() || !bisectingLineYDirection.normalize())
		return false;

	HomogenousMatrix4 invNewWorldTplane(previousPlane2worldTransformation);

	if (!invNewWorldTplane.invert())
		return false;

	Vector3 virtualXAxis = invNewWorldTplane * (previousPlane2worldTransformation.translation() + bisectingLineXDirection);
	Vector3 virtualYAxis = invNewWorldTplane * (previousPlane2worldTransformation.translation() + bisectingLineYDirection);

	if (!virtualXAxis.normalize() || !virtualYAxis.normalize())
		return false;

	Vector3 bisectingLineVirtualPlane = (virtualXAxis + virtualYAxis);

	if (!bisectingLineVirtualPlane.normalize())
		return false;

	// we rotate the plane2world transformation that the vector (1, 1) is equal to the bisecting line of the x and y axis of the virtual grid
	const Rotation rot(Vector3(1, 1, 0).normalizedOrZero(), bisectingLineVirtualPlane);

	plane2worldTransformation = previousPlane2worldTransformation * HomogenousMatrix4(rot);

	virtualGrid2planeTransformation = HomogenousMatrix4(-rot * virtualXAxis, -rot * virtualYAxis, Vector3(0, 0, 1), Vector3(0, 0, 0));

	return true;
}

bool Grid::determineTransformationWithoutShearComponent(const HomogenousMatrix4& transformationWithShearComponent, HomogenousMatrix4& resultTransformation)
{
	ocean_assert(transformationWithShearComponent.xAxis().isOrthogonal(transformationWithShearComponent.zAxis()));
	ocean_assert(transformationWithShearComponent.yAxis().isOrthogonal(transformationWithShearComponent.zAxis()));

	HomogenousMatrix4 newTransformation(true);

	Vector3 xDirection = transformationWithShearComponent * Vector3(1, 0, 0) - transformationWithShearComponent.translation();
	Vector3 yDirection = transformationWithShearComponent * Vector3(0, 1, 0) - transformationWithShearComponent.translation();

	if (!xDirection.normalize() || !yDirection.normalize())
		return false;

	Vector3 bisectingLine = xDirection + yDirection;

	if (!bisectingLine.normalize())
		return false;

	const Rotation rot(Vector3(1, 1, 0).normalized(), bisectingLine);

	newTransformation = HomogenousMatrix4(rot) * newTransformation;

	Vector3 zAxis(transformationWithShearComponent.zAxis());

	if (!zAxis.normalize())
		return false;

	const Rotation rotNormal(newTransformation.zAxis(), zAxis);

	newTransformation = HomogenousMatrix4(rotNormal) * newTransformation;

	newTransformation.setTranslation(transformationWithShearComponent.translation());

	resultTransformation = newTransformation;

	return true;
}

void Grid::adjustGridCornersInXDirectionByImageLines(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose, const FiniteLine2& gridBoundingBoxEdgeImageCoordinate, const FiniteLine2& imageBoundingBoxEdgeImageCoordinate)
{
	ocean_assert(imageBoundingBoxEdgeImageCoordinate.isValid());

	if (!gridBoundingBoxEdgeImageCoordinate.isValid())
		return;

	Vector2 intersectionPoint;
	if (gridBoundingBoxEdgeImageCoordinate.intersection(imageBoundingBoxEdgeImageCoordinate, intersectionPoint))
	{
		Vector2 intersectionGridCoordinate;
		if (image2grid<true>(pinholeCamera, pose, intersectionPoint, intersectionGridCoordinate))
		{
			if (gridBoundingBoxEdgeImageCoordinate.direction() * imageBoundingBoxEdgeImageCoordinate.direction().perpendicular() > Numeric::eps())
				gridLowerCorner.x() = std::max(gridLowerCorner.x(), intersectionGridCoordinate.x());
			else
				gridUpperCorner.x() = std::min(gridUpperCorner.x(), intersectionGridCoordinate.x());
		}
	}
}

void Grid::adjustGridCornersInYDirectionByImageLines(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose, const FiniteLine2& gridBoundingBoxEdgeImageCoordinate, const FiniteLine2& imageBoundingBoxEdgeImageCoordinate)
{
	ocean_assert(imageBoundingBoxEdgeImageCoordinate.isValid());

	if (!gridBoundingBoxEdgeImageCoordinate.isValid())
		return;

	Vector2 intersectionPoint;
	if (gridBoundingBoxEdgeImageCoordinate.intersection(imageBoundingBoxEdgeImageCoordinate, intersectionPoint))
	{
		Vector2 intersectionGridCoordinate;
		if (image2grid<true>(pinholeCamera, pose, intersectionPoint, intersectionGridCoordinate))
		{
			if (gridBoundingBoxEdgeImageCoordinate.direction() * imageBoundingBoxEdgeImageCoordinate.direction().perpendicular() > Numeric::eps())
				gridLowerCorner.y() = std::max(gridLowerCorner.y(), intersectionGridCoordinate.y());
			else
				gridUpperCorner.y() = std::min(gridUpperCorner.y(), intersectionGridCoordinate.y());
		}
	}
}

bool Grid::clipWorldLineInFrontOfCameraBinarySearchIF(const HomogenousMatrix4& poseIF, const FiniteLine3& lineToClip, FiniteLine3& resultingLine)
{
	ocean_assert(poseIF.isValid());
	ocean_assert(lineToClip.isValid());

	const bool point0InFront = isInFrontOfCameraIF(poseIF, lineToClip.point0());
	const bool point1InFront = isInFrontOfCameraIF(poseIF, lineToClip.point1());

	if (point0InFront && point1InFront)
	{
		resultingLine = lineToClip;
		return true;
	}

	if (!point0InFront && !point1InFront)
		return false;

	Vector3 testPoint0 = point0InFront ? lineToClip.point0() : lineToClip.point1();
	Vector3 testPoint1 = point0InFront ? lineToClip.point1() : lineToClip.point0();

	for (unsigned int n = 0u; n < 15u; ++n)
	{
		Vector3 test((testPoint0 + testPoint1) * Scalar(0.5));

		// check whether the test point is in front of the camera
		if (isInFrontOfCameraIF(poseIF, test))
			testPoint0 = test;
		else
			testPoint1 = test;
	}

	Vector3 clippedPoint((testPoint0 + testPoint1) * Scalar(0.5));

	if (!isInFrontOfCameraIF(poseIF, clippedPoint))
		return false;

	if (point0InFront)
		resultingLine = FiniteLine3(lineToClip.point0(), clippedPoint);
	else
	{
		ocean_assert(point1InFront);
		resultingLine = FiniteLine3(clippedPoint, lineToClip.point1());
	}

	ocean_assert(resultingLine.isValid());
	return true;
}

}

}
