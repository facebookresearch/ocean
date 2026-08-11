/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_GEOMETRY_GRID_H
#define META_OCEAN_GEOMETRY_GRID_H

#include "ocean/geometry/Geometry.h"

#include "ocean/math/FiniteLine2.h"
#include "ocean/math/FiniteLine3.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"

namespace Ocean
{

namespace Geometry
{

/**
 * This class implements the basic data of an planar grid.
 * There are 3 coordinate systems:
 * (1) world coordinates:
 *	   - describe points in the world system
 * (2) plane coordinates:
 *	   - describe points in the plane of the grid
 *	   - the z coordinate is zero, if the point lies directly in the plane
 * (3) grid coordinates:
 *	   - describe points in the grid refer to the plane
 *	   - the grid center must not be identical with the plane center
 *	   - the following sketch shows a top-view of the grid:
 * <pre>
 *		 y
 *		 ^
 *       |   o - o - o - o - o
 *       |   |   |   |   |   |
 *       |   o - o - o - p - o
 *       |   |   |   |   |   |
 *       |   o - o - c - o - o
 *       |   |   |   |   |   |
 *       |   o - o - o - o - o
 *       |   |   |   |   |   |
 *       |   o - o - o - o - o
 *       |
 *		 z---------------------> x
 * </pre>
 *	   - the z coordinate is always zero, so in following the grid coordinates are represent as a Vector2
 *
 * This class holds 3 transformations:
 * (1) plane2world transformation:
 *	   - transforms points defined in a coordinate system of the plane into points defined in the world coordinate system
 *	   - all axes must be perpendicular to each other
 *	   - the z axis always describe the normal of the plane
 * (2) grid2plane transformation:
 *	   - transforms points defined in a coordinate system of the grid into points defined in the plane coordinate system
 *	   - all axes must be perpendicular to each other
 *	   - the z axis is always (0, 0, 1)
 * (3) virtualGrid2plane transformation
 *	   - transforms points defined in a coordinate system of the virtual grid into points defined in the plane coordinate system
 *	   - the z axis is always (0, 0, 1)
 *	   - the x and y axes must be perpendicular to the z axis
 *	   - the x axis must not be perpendicular to the y axis to skew the grid
 *
 * The grid holds an visualization bounding box defined in plane coordinates.
 *
 * This class implements convert functions to convert point from one coordinate system to an other.
 * @ingroup geometry
 */
class OCEAN_GEOMETRY_EXPORT Grid
{
	public:

		/**
		 * Creates a new invalid grid element.
		 */
		Grid() = default;

		/**
		 * Creates a new Grid by a plane2world transformation.
		 * Whether the virtualGrid2plane transformation is set, the grid2plane transformation is set to the same matrix.
		 * If the grid2plane transformation should be set, the virtualGrid2plane transformation can be set to the appropriate matrix.
		 * @param plane2worldTransformation Transformation that transforms points defined in a coordinate system of the plane into points defined in the world coordinate system
		 * @param virtualGrid2planeTransformation Optional transformation that transforms points defined in a coordinate system of the virtual grid into points defined in the plane coordinate system
		 * @param lowerCorner Optional lower corner of the grid bounding box for visualization in plane coordinates
		 * @param upperCorner Optional upper corner of the grid bounding box for visualization in plane coordinates
		 */
		inline explicit Grid(const HomogenousMatrix4& plane2worldTransformation, const HomogenousMatrix4& virtualGrid2planeTransformation = HomogenousMatrix4(true), const Vector2& lowerCorner = Vector2(-0.5, -0.5), const Vector2& upperCorner = Vector2(0.5, 0.5));

		/**
		 * Creates a new Grid by a plane2world transformation and four image points defining a rectangle.
		 * The bounding box is adjusted, so the rectangle plus a border lies inside.
		 * The border has a size of the half average length of two neighboring projected plane points.
		 * If the virtualGrid2plane transformation is set, the grid2plane transformation is set to the same matrix.
		 * If the grid2plane transformation should be set, the virtualGrid2plane transformation can be set to the appropriate matrix.
		 * @param pinholeCamera The pinhole camera profile to be applied
		 * @param world_T_camera The camera pose to be applied, transforming camera to world
		 * @param imagePoints The image points to adjust the bounding box, size must be four
		 * @param plane2worldTransformation Transformation that transforms points defined in a coordinate system of the plane into points defined in the world coordinate system
		 * @param virtualGrid2planeTransformation Optional transformation that transforms points defined in a coordinate system of the virtual grid into points defined in the plane coordinate system
		 * @param clipBoundingBox Optional flag to clip the grid lower and upper grid corners by an image bounding box around the camera frame
		 */
		Grid(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& world_T_camera, const Vectors2& imagePoints, const HomogenousMatrix4& plane2worldTransformation, const HomogenousMatrix4& virtualGrid2planeTransformation = HomogenousMatrix4(true), const bool clipBoundingBox = false);

		/**
		 * Creates a new Grid by four given image points using vanishing projection.
		 * The four image points must define a convex polygon.
		 * If additionally four grid coordinates are given, the virtualGrid2plane transformation is set, too.
		 * The four grid coordinates must be given in the same order (clockwise or counter clockwise) like the four image points!
		 * The grid coordinates are only used for determine the shear of the virtualGrid2plane transformation, the lower and upper grid corners are not adjust!
		 * If no grid could be determined from the four image points, a invalid grid is create.
		 * @param pinholeCamera The pinhole camera profile to be applied
		 * @param world_T_camera The camera pose to be applied, transforming camera to world
		 * @param imagePoints The four image points (must be in clockwise or counterclockwise order)
		 * @param gridCoordinates Optional four grid coordinates (must be in clockwise or counterclockwise order)
		 * @param previousPlane2worldTransformation Optional Transformation that transforms points defined in a coordinate system of the plane into points defined in the world coordinate system to adapt the plane translation
		 * @param clipBoundingBox Optional flag to clip the grid lower and upper grid corners by an image bounding box around the camera frame
		 */
		Grid(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& world_T_camera, const Vectors2& imagePoints, const Vector2* gridCoordinates = nullptr, const HomogenousMatrix4* previousPlane2worldTransformation = nullptr, const bool clipBoundingBox = false);

		/**
		 * Creates a new Grid by a given transformation that transforms points defined in a coordinate system of the plane into points defined in the world coordinate system.
		 * The virtualGrid2plane transformation is adjusted by four given image points.
		 * The virtual grid is sheared that the four points define a rectangle on the virtual grid in the best case.
		 * @param pinholeCamera The pinhole camera profile to be applied
		 * @param world_T_camera The camera pose to be applied, transforming camera to world
		 * @param plane2worldTransformation Transformation that transforms points defined in a coordinate system of the plane into points defined in the world coordinate system
		 * @param imagePoints The four image points (must be in clockwise or counterclockwise order)
		 * @param clipBoundingBox Optional flag to clip the grid lower and upper grid corners by an image bounding box around the camera frame
		 */
		Grid(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& world_T_camera, const HomogenousMatrix4& plane2worldTransformation, const Vectors2& imagePoints, const bool clipBoundingBox = false);

		/**
		 * Returns the transformation that transforms points defined in a coordinate system of the plane into points defined in the world coordinate system.
		 * @return The plane transformation
		 */
		inline const HomogenousMatrix4& plane2worldTransformation() const;

		/**
		 * Returns the transformation that transforms points defined in a coordinate system of the grid into points defined in the plane coordinate system.
		 * @return The grid transformation.
		 */
		inline const HomogenousMatrix4& grid2planeTransformation() const;

		/**
		 * Returns the transformation that transform points defined in a coordinate system of the virtual grid into points defined in the plane coordinate system.
		 * @return The virtual grid transformation
		 */
		inline const HomogenousMatrix4& virtualGrid2planeTransformation() const;

		/**
		 * Returns a transformation that transforms points defined in a coordinate system of the virtual plane into points defined in the world coordinate system.
		 * The virtual plane has the orientation scale and depth of the normal plane while an optional shear component is extracted from the virtual grid if the virtual grid has a shear component.<br>
		 * @return The virtual plane transformation
		 */
		HomogenousMatrix4 virtualPlane2worldTransformation() const;

		/**
		 * Returns the transformation that transform points defined in a coordinate system of the grid into points defined in the world coordinate system.
		 * @return The grid transformation
		 */
		inline HomogenousMatrix4 grid2worldTransformation() const;

		/**
		 * Returns the transformation that transform points defined in a coordinate system of the virtual grid into points defined in the world coordinate system.
		 * @return The virtual grid transformation
		 */
		inline HomogenousMatrix4 virtualGrid2worldTransformation() const;

		/**
		 * Returns the lower grid corner defined in the grid coordinate system of the visualization bounding box of the grid.
		 * @return lower grid corner
		 */
		inline const Vector2& lowerGridCorner() const;

		/**
		 * Returns the upper grid corner defined in the grid coordinate system of the visualization bounding box of the grid.
		 * @return upper grid corner
		 */
		inline const Vector2& upperGridCorner() const;

		/**
		 * Sets the transformation that transforms points defined in a coordinate system of the plane into points defined in the world coordinate system.
		 * @param newTransformation New transformation to set
		 */
		inline void setPlane2worldTransformation(const HomogenousMatrix4& newTransformation);

		/**
		 * Sets the transformation that transforms points defined in a coordinate system of the grid into points defined in the plane coordinate system.
		 * The transformation that transforms points defined in a coordinate system of the virtual grid into points defined in the plane coordinate system is also set to the same matrix.
		 * @param newTransformation New transformation to set
		 */
		inline void setGrid2planeTransformation(const HomogenousMatrix4& newTransformation);

		/**
		 * Sets the transformation that transform points defined in a coordinate system of the virtual grid into points defined in the plane coordinate system.
		 * The transformation that transforms points defined in a coordinate system of the grid into points defined in the plane coordinate system is also set:
		 * If the given matrix is not valid, the grid2plane transformation will also be invalid.
		 * If the given matrix is valid, the grid2plane transformation will be set without the shear of the virtual transformation and a scale which is an average of the x and y scale.
		 * @param newTransformation New transformation to set
		 * @return True, if succeeded
		 */
		bool setVirtualGrid2planeTransformation(const HomogenousMatrix4& newTransformation);

		/**
		 * Sets the lower grid corner defined in the plane coordinate system of the visualization bounding box of the grid.
		 * If the lower grid corner is set to a value higher than the upper corner, the grid will be invalid.
		 * @param newCorner New lower grid corner
		 */
		inline void setLowerGridCorner(const Vector2& newCorner);

		/**
		 * Sets the upper grid corner defined in the plane coordinate system of the visualization bounding box of the grid.
		 * If the upper grid corner is set to a value smaller than the lower corner, the grid will be invalid.
		 * @param newCorner New upper grid corner
		 */
		inline void setUpperGridCorner(const Vector2& newCorner);

		/**
		 * Returns whether the grid is valid.
		 * The grid is valid, whether the lower grid corner is smaller than the upper grid corner, the 3 transformation are valid matrices and:
		 * the x, y and z axis of the plane2world and grid2plane transformation are orthogonal to each other and the x and y axis of the virtualGrid2plane transformation are orthogonal to the z axis.
		 * @return True, if so
		 */
		bool isValid() const;

		/**
		 * Sets the grid to invalid.
		 * All transformations will be set to invalid transformations.
		 * The upper grid corner will be set higher as the lower grid corner.
		 */
		void setInvalid();

		/**
		 * Returns whether the physical and virtual grid transformations are identical.
		 * @return True, if so
		 */
		bool areVirtualAndPhysicalGridIdentical() const;

		/**
		 * Clips the grid bounding box and adjusts the lower and upper grid corner by an image bounding box.
		 * This bounding box is defined by a given camera, a pose and a border size factor.
		 * The image bounding box will have the following bounds:
		 * Left: -width * factor
		 * Right width + width * factor
		 * Top: -height * factor
		 * Bottom: height + height * factor
		 * @param pinholeCamera The pinhole camera profile to be applied
		 * @param world_T_camera The camera pose to be applied, transforming camera to world
		 * @param borderSizeFactor the factor for the width and height for the border around the camera frame, Range [0, infinity)
		 * @param insideGridCoordinates Optional grid coordinates that must lie inside the new clipped grid
		 */
		void clipByImageBoundingBox(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& world_T_camera, const Scalar borderSizeFactor = Scalar(2.0), const Vectors2& insideGridCoordinates = Vectors2());

		/**
		 * Convert an point defined in a coordinate system of the camera image into an point on the plane defined in the world coordinate system.
		 * The grid must be valid!
		 * @param pinholeCamera The pinhole camera profile to be applied
		 * @param world_T_camera The camera pose to be applied, transforming camera to world
		 * @param imageCoordinate Image coordinate which will be converted
		 * @param worldCoordinate resulting world coordinate, if the conversion was successful
		 * @return True, if the conversion was successful
		 */
		bool image2world(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& world_T_camera, const Vector2& imageCoordinate, Vector3& worldCoordinate) const;

		/**
		 * Convert an point defined in a coordinate system of the camera image into an point on the plane defined in the plane coordinate system.
		 * The grid must be valid!
		 * @param pinholeCamera The pinhole camera profile to be applied
		 * @param world_T_camera The camera pose to be applied, transforming camera to world
		 * @param imageCoordinate Image coordinate which will be converted
		 * @param planeCoordinate resulting plane coordinate, if the conversion was successful
		 * @return True, if the conversion was successful
		 */
		bool image2plane(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& world_T_camera, const Vector2& imageCoordinate, Vector3& planeCoordinate) const;

		/**
		 * Convert an point defined in a coordinate system of the camera image into an point on the plane defined in the grid coordinate system.
		 * The grid must be valid!
		 * @param pinholeCamera The pinhole camera profile to be applied
		 * @param world_T_camera The camera pose to be applied, transforming camera to world
		 * @param imageCoordinate Image coordinate which will be converted
		 * @param gridCoordinate resulting grid coordinate, if the conversion was successful
		 * @tparam tVirtual Indicate whether the virtual grid is used
		 * @return True, if the conversion was successful
		 */
		template <bool tVirtual>
		bool image2grid(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& world_T_camera, const Vector2& imageCoordinate, Vector2& gridCoordinate) const;

		/**
		 * Convert an point on the plane defined in a coordinate system of the world into an point defined in the camera image coordinate system.
		 * The grid must be valid!
		 * @param pinholeCamera The pinhole camera profile to be applied
		 * @param world_T_camera The camera pose to be applied, transforming camera to world
		 * @param worldCoordinate World coordinate which will be converted
		 * @return resulting image coordinate
		 */
		inline Vector2 world2image(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& world_T_camera, const Vector3& worldCoordinate) const;

		/**
		 * Convert an point on the plane defined in a coordinate system of the world into an point defined in the camera image coordinate system.
		 * The grid must be valid!
		 * @param pinholeCamera The pinhole camera profile to be applied
		 * @param flippedCamera_T_world Inverted flipped pose to be applied
		 * @param worldCoordinate World coordinate which will be converted
		 * @return resulting image coordinate
		 */
		inline Vector2 world2imageIF(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& flippedCamera_T_world, const Vector3& worldCoordinate) const;

		/**
		 * Convert an point on the plane defined in a coordinate system of the world into an point defined in the plane coordinate system.
		 * The grid must be valid!
		 * @param worldCoordinate World coordinate which will be converted
		 * @return resulting plane coordinate
		 */
		inline Vector3 world2plane(const Vector3& worldCoordinate) const;

		/**
		 * Convert an point on the plane defined in a coordinate system of the world into an point defined in the grid coordinate system.
		 * The grid must be valid!
		 * @param worldCoordinate World coordinate which will be converted
		 * @tparam tVirtual Indicate whether the virtual grid is used
		 * @return resulting grid coordinate
		 */
		template <bool tVirtual>
		inline Vector2 world2grid(const Vector3& worldCoordinate) const;

		/**
		 * Convert an point on the plane defined in a coordinate system of the plane into an point defined in the camera image coordinate system.
		 * The grid must be valid!
		 * @param pinholeCamera The pinhole camera profile to be applied
		 * @param world_T_camera The camera pose to be applied, transforming camera to world
		 * @param planeCoordinate Plane coordinate which will be converted
		 * @return resulting image coordinate
		 */
		inline Vector2 plane2image(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& world_T_camera, const Vector3& planeCoordinate) const;

		/**
		 * Convert an point on the plane defined in a coordinate system of the plane into an point defined in the camera image coordinate system.
		 * The grid must be valid!
		 * @param pinholeCamera The pinhole camera profile to be applied
		 * @param flippedCamera_T_world inverted flipped pose to be applied
		 * @param planeCoordinate Plane coordinate which will be converted
		 * @return resulting image coordinate
		 */
		inline Vector2 plane2imageIF(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& flippedCamera_T_world, const Vector3& planeCoordinate) const;

		/**
		 * Convert an point on the plane defined in a coordinate system of the plane into an point defined in the world coordinate system.
		 * The grid must be valid!
		 * @param planeCoordinate Plane coordinate which will be converted
		 * @return resulting world coordinate
		 */
		inline Vector3 plane2world(const Vector3& planeCoordinate) const;

		/**
		 * Convert an point on the plane defined in a coordinate system of the plane into an point defined in the grid coordinate system.
		 * The grid must be valid!
		 * @param planeCoordinate Plane coordinate which will be converted
		 * @tparam tVirtual Indicate whether the virtual grid is used
		 * @return resulting grid coordinate
		 */
		template <bool tVirtual>
		inline Vector2 plane2grid(const Vector3& planeCoordinate) const;

		/**
		 * Convert an point on the plane defined in a coordinate system of the grid into an point defined in the camera image coordinate system.
		 * The grid must be valid!
		 * @param pinholeCamera The pinhole camera profile to be applied
		 * @param world_T_camera The camera pose to be applied, transforming camera to world
		 * @param gridCoordinate Grid coordinate which will be converted
		 * @tparam tVirtual Indicate whether the virtual grid is used
		 * @return resulting image coordinate
		 */
		template <bool tVirtual>
		inline Vector2 grid2image(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& world_T_camera, const Vector2& gridCoordinate) const;

		/**
		 * Convert an point on the plane defined in a coordinate system of the grid into an point defined in the camera image coordinate system.
		 * The grid must be valid!
		 * @param pinholeCamera The pinhole camera profile to be applied
		 * @param flippedCamera_T_world Inverted flipped pose to be applied
		 * @param gridCoordinate Grid coordinate which will be converted
		 * @tparam tVirtual Indicate whether the virtual grid is used
		 * @return resulting image coordinate
		 */
		template <bool tVirtual>
		inline Vector2 grid2imageIF(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& flippedCamera_T_world, const Vector2& gridCoordinate) const;

		/**
		 * Convert an point on the plane defined in a coordinate system of the grid into an point defined in the world coordinate system.
		 * The grid must be valid!
		 * @param gridCoordinate Grid coordinate which will be converted
		 * @tparam tVirtual Indicate whether the virtual grid is used
		 * @return resulting world coordinate
		 */
		template <bool tVirtual>
		inline Vector3 grid2world(const Vector2& gridCoordinate) const;

		/**
		 * Convert an point on the plane defined in a coordinate system of the grid into an point defined in the plane coordinate system.
		 * The grid must be valid!
		 * @param gridCoordinate Grid coordinate which will be converted
		 * @tparam tVirtual Indicate whether the virtual grid is used
		 * @return resulting plane coordinate
		 */
		template <bool tVirtual>
		inline Vector3 grid2plane(const Vector2& gridCoordinate) const;

		/**
		 * Returns whether two grids are identical.
		 * @param grid Right grid
		 * @return True, if so
		 */
		inline bool operator==(const Grid& grid) const;

		/**
		 * Returns whether two grids are not identical.
		 * @param grid Right grid
		 * @return True, if so
		 */
		inline bool operator!=(const Grid& grid) const;

		/**
		 * Determine the transformation that transforms points defined in a coordinate system of the plane into points defined in the world coordinate system.
		 * If additionally four grid coordinates are given, the transformation that transforms points defined in a coordinate system of the grid/virtual grid into points defined in the plane coordinate system by four given image points are determined, too.
		 * The four grid coordinates must be given in the same order (clockwise or counter clockwise) like the four image points!
		 * If no grid coordinates are given, the resulting virtualGrid2plane transformation equals to the grid2plane transformation.
		 * If a previous plane2world transformation is given, the new one will be oriented at the same translation, otherwise the origin of the new plane2world transformation will be set to a distance of 1 from the camera.
		 * @param pinholeCamera The pinhole camera profile to be applied
		 * @param world_T_camera The camera pose to be applied, transforming camera to world
		 * @param imagePoints The four image points to determine the transformations (must be in clockwise or counterclockwise order)
		 * @param gridCoordinates Optional four grid coordinates (must be in clockwise or counterclockwise order)
		 * @param previousPlane2worldTransformation Optional previous plane2world transformation
		 * @param plane2worldTransformation Optional resulting plane2world transformation
		 * @param virtualGrid2planeTransformation Optional resulting virtualGrid2plane transformation
		 * @return True, if the Transformations could be determined
		 */
		static bool determineVirtualGridTransformationsByFourImagePoints(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& world_T_camera, const Vector2* imagePoints, const Vector2* gridCoordinates = nullptr, const HomogenousMatrix4* previousPlane2worldTransformation = nullptr, HomogenousMatrix4* plane2worldTransformation = nullptr, HomogenousMatrix4* virtualGrid2planeTransformation = nullptr);

		/**
		 * Determines a virtualGrid2plane transformation by a plane2world transformation and four given image points.
		 * Furthermore an adjusted plane2world transformation is determined which is the previous plane2world transformation rotated around the plane normal
		 * that the bisection of the x and y axis of the virtual and non virtual grid transformation have the same orientation.
		 * @param pinholeCamera The pinhole camera profile to be applied
		 * @param world_T_camera The camera pose to be applied, transforming camera to world
		 * @param imagePoints The four image points to determine the transformations (must be in clockwise or counterclockwise order)
		 * @param previousPlane2worldTransformation Previous plane2world transformation
		 * @param plane2worldTransformation The new plane2world transformation that is a rotation of the previous plane2world transformation around the plane normal
		 * @param virtualGrid2planeTransformation The determined virtualGrid2plane transformation
		 * @return True, if succeeded
		 */
		static bool determineVirtualGridTransformationByPlane2WorldTransformation(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& world_T_camera, const Vectors2& imagePoints, const HomogenousMatrix4& previousPlane2worldTransformation, HomogenousMatrix4& plane2worldTransformation, HomogenousMatrix4& virtualGrid2planeTransformation);

		/**
		 * Determine the nearest integer grid node from a given grid coordinate.
		 * An x or y value from >= 2.5 to < 3.5 is rounded to 3.0.
		 * @param gridCoordinate Grid coordinate to get the nearest node from
		 * @return Nearest node in grid coordinates
		 */
		static inline Vector2 grid2nearestNode(const Vector2& gridCoordinate);

		/**
		 * Returns whether a given transformation has shear component in x/y direction.
		 * The x and y axes of the given transformation must be orthogonal to the z axis.
		 * @param transformation The transformation to check
		 * @return True, if so
		 */
		static inline bool hasTransformationShearComponent(const HomogenousMatrix4& transformation);

		/**
		 * Determines a transformation without a shear component from a given transformation with shear component.
		 * The x and y axes of the given transformation with shear component must be orthogonal to the z axis.
		 * The resulting transformation has the same z axis and translation like the given transformation.
		 * @param transformationWithShearComponent Transformation with shear component
		 * @param resultTransformation Resulting transformation without a shear component
		 * @return True, if succeeded
		 */
		static bool determineTransformationWithoutShearComponent(const HomogenousMatrix4& transformationWithShearComponent, HomogenousMatrix4& resultTransformation);

		/**
		 * Returns whether a given world coordinate is in front of the camera.
		 * @param world_T_camera Current camera pose, transforming camera to world
		 * @param worldCoordinate World coordinate to be checked
		 * @return True, if so
		 */
		static inline bool isInFrontOfCamera(const HomogenousMatrix4& world_T_camera, const Vector3& worldCoordinate);

		/**
		 * Returns whether a given world coordinate is in front of the camera.
		 * @param flippedCamera_T_world Current inverted flipped camera pose
		 * @param worldCoordinate World coordinate to be checked
		 * @return True, if so
		 */
		static inline bool isInFrontOfCameraIF(const HomogenousMatrix4& flippedCamera_T_world, const Vector3& worldCoordinate);

	protected:

		/**
		 * Adjust the lower and upper grid corner in x direction by a given finite image line that is a projected grid edge in x direction and a finite image line of an image bounding box edge that clips the grid.
		 * Beware: The grid edge must be have a orientation of (1, 0) in grid coordinates
		 * @param pinholeCamera The pinhole camera profile to be applied
		 * @param world_T_camera The camera pose to be applied, transforming camera to world
		 * @param gridBoundingBoxEdgeImageCoordinate The finite image line of a grid edge in x direction
		 * @param imageBoundingBoxEdgeImageCoordinate The finite image line of image bounding box edge that clips the grid
		 */
		void adjustGridCornersInXDirectionByImageLines(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& world_T_camera, const FiniteLine2& gridBoundingBoxEdgeImageCoordinate, const FiniteLine2& imageBoundingBoxEdgeImageCoordinate);

		/**
		 * Adjust the lower and upper grid corner in y direction by a given finite image line that is a projected grid edge in y direction and a finite image line of an image bounding box edge that clips the grid.
		 * Beware: The grid edge must be have a orientation of (0, 1) in grid coordinates
		 * @param pinholeCamera The pinhole camera profile to be applied
		 * @param world_T_camera The camera pose to be applied, transforming camera to world
		 * @param gridBoundingBoxEdgeImageCoordinate The finite image line of a grid edge in y direction
		 * @param imageBoundingBoxEdgeImageCoordinate The finite image line of image bounding box edge that clips the grid
		 */
		void adjustGridCornersInYDirectionByImageLines(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& world_T_camera, const FiniteLine2& gridBoundingBoxEdgeImageCoordinate, const FiniteLine2& imageBoundingBoxEdgeImageCoordinate);

		/**
		 * Clips a finite line in world coordinates, that the start and end point lie in front of the camera.
		 * A binary search is determine the possible start and end point who are in front of the camera.
		 * If both points lie behind the camera, the line is not adjusted.
		 * @param flippedCamera_T_world Inverted flipped pose to be applied
		 * @param lineToClip The finite line to clip
		 * @param resultingLine The resulting clipped line if succeeded
		 * @return True, if succeeded
		 */
		static bool clipWorldLineInFrontOfCameraBinarySearchIF(const HomogenousMatrix4& flippedCamera_T_world, const FiniteLine3& lineToClip, FiniteLine3& resultingLine);

	protected:

		/// Transformation that transforms points defined in a coordinate system of the plane into points defined in the world coordinate system, invalid until set.
		HomogenousMatrix4 world_T_plane_ = HomogenousMatrix4(false);

		/// Transformation that transforms points defined in a coordinate system of the grid into points defined in the plane coordinate system, invalid until set.
		HomogenousMatrix4 plane_T_grid_ = HomogenousMatrix4(false);

		/// Transformation that transform points defined in a coordinate system of the virtual grid into points defined in the plane coordinate system, invalid until set.
		HomogenousMatrix4 plane_T_virtualGrid_ = HomogenousMatrix4(false);

		/// Lower grid corner of the visualization bounding box of the grid defined in the grid coordinate system, an empty box until set.
		Vector2 gridLowerCorner_ = Vector2(1, 1);

		/// Upper grid corner of the visualization bounding box of the grid defined in the grid coordinate system, an empty box until set.
		Vector2 gridUpperCorner_ = Vector2(-1, -1);
};

inline Grid::Grid(const HomogenousMatrix4& plane2worldTransformation, const HomogenousMatrix4& virtualGrid2planeTransformation, const Vector2& lowerCorner, const Vector2& upperCorner) :
	world_T_plane_(plane2worldTransformation),
	gridLowerCorner_(lowerCorner),
	gridUpperCorner_(upperCorner)
{
	// sets the physical and virtual grid2plane transformation
	setVirtualGrid2planeTransformation(virtualGrid2planeTransformation);
}

inline const HomogenousMatrix4& Grid::plane2worldTransformation() const
{
	return world_T_plane_;
}

inline const HomogenousMatrix4& Grid::grid2planeTransformation() const
{
	return plane_T_grid_;
}

inline const HomogenousMatrix4& Grid::virtualGrid2planeTransformation() const
{
	return plane_T_virtualGrid_;
}

inline HomogenousMatrix4 Grid::grid2worldTransformation() const
{
	return world_T_plane_ * plane_T_grid_;
}

inline HomogenousMatrix4 Grid::virtualGrid2worldTransformation() const
{
	return world_T_plane_ * plane_T_virtualGrid_;
}

inline const Vector2& Grid::lowerGridCorner() const
{
	return gridLowerCorner_;
}

inline const Vector2& Grid::upperGridCorner() const
{
	return gridUpperCorner_;
}

inline void Grid::setPlane2worldTransformation(const HomogenousMatrix4& newTransformation)
{
	ocean_assert(newTransformation.isValid());

	world_T_plane_ = newTransformation;
}

inline void Grid::setGrid2planeTransformation(const HomogenousMatrix4& newTransformation)
{
	ocean_assert(newTransformation.isValid());

	plane_T_grid_ = newTransformation;
	plane_T_virtualGrid_ = newTransformation;
}

inline void Grid::setLowerGridCorner(const Vector2& newCorner)
{
	gridLowerCorner_ = newCorner;
}

inline void Grid::setUpperGridCorner(const Vector2& newCorner)
{
	gridUpperCorner_ = newCorner;
}

template <bool tVirtual>
bool Grid::image2grid(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& world_T_camera, const Vector2& imageCoordinate, Vector2& gridCoordinate) const
{
	ocean_assert(isValid());
	ocean_assert(pinholeCamera.isValid() && world_T_camera.isValid());

	Vector3 planeCoordinate;
	if (!image2plane(pinholeCamera, world_T_camera, imageCoordinate, planeCoordinate))
	{
		return false;
	}

	gridCoordinate = plane2grid<tVirtual>(planeCoordinate);

	return true;
}

inline Vector2 Grid::world2image(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& world_T_camera, const Vector3& worldCoordinate) const
{
	ocean_assert(isValid());
	ocean_assert(pinholeCamera.isValid() && world_T_camera.isValid());
	//ocean_assert(Plane3(world_T_plane_).isInPlane(worldCoordinate, Numeric::eps()));

	return pinholeCamera.projectToImageDamped(world_T_camera, worldCoordinate, pinholeCamera.hasDistortionParameters());
}

inline Vector2 Grid::world2imageIF(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& flippedCamera_T_world, const Vector3& worldCoordinate) const
{
	ocean_assert(isValid());
	ocean_assert(pinholeCamera.isValid() && flippedCamera_T_world.isValid());
	//ocean_assert(Plane3(world_T_plane_).isInPlane(worldCoordinate, Numeric::eps()));

	return pinholeCamera.projectToImageDampedIF(flippedCamera_T_world, worldCoordinate, pinholeCamera.hasDistortionParameters());
}

inline Vector3 Grid::world2plane(const Vector3& worldCoordinate) const
{
	ocean_assert(isValid());
	ocean_assert(Plane3(world_T_plane_).isInPlane(worldCoordinate, Numeric::eps()));

	return world_T_plane_.inverted() * worldCoordinate;
}

template <bool tVirtual>
inline Vector2 Grid::world2grid(const Vector3& worldCoordinate) const
{
	ocean_assert(isValid());

	return plane2grid<tVirtual>(world2plane(worldCoordinate));
}

inline Vector2 Grid::plane2image(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& world_T_camera, const Vector3& planeCoordinate) const
{
	ocean_assert(isValid());
	ocean_assert(pinholeCamera.isValid() && world_T_camera.isValid());

	return world2image(pinholeCamera, world_T_camera, plane2world(planeCoordinate));
}

inline Vector2 Grid::plane2imageIF(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& flippedCamera_T_world, const Vector3& planeCoordinate) const
{
	ocean_assert(isValid());
	ocean_assert(pinholeCamera.isValid() && flippedCamera_T_world.isValid());

	return world2imageIF(pinholeCamera, flippedCamera_T_world, plane2world(planeCoordinate));
}

inline Vector3 Grid::plane2world(const Vector3& planeCoordinate) const
{
	ocean_assert(isValid());

	return world_T_plane_ * planeCoordinate;
}

template <bool tVirtual>
inline Vector2 Grid::plane2grid(const Vector3& planeCoordinate) const
{
	ocean_assert(isValid());

	const Vector3 gridCoordinate = tVirtual ? plane_T_virtualGrid_.inverted() * planeCoordinate : plane_T_grid_.inverted() * planeCoordinate;
	//ocean_assert(Numeric::isEqualEps(gridCoordinate.z()));

	return Vector2(gridCoordinate.x(), gridCoordinate.y());
}

template <bool tVirtual>
inline Vector2 Grid::grid2image(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& world_T_camera, const Vector2& gridCoordinate) const
{
	ocean_assert(isValid());
	ocean_assert(pinholeCamera.isValid() && world_T_camera.isValid());

	return world2image(pinholeCamera, world_T_camera, grid2world<tVirtual>(gridCoordinate));
}

template <bool tVirtual>
inline Vector2 Grid::grid2imageIF(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& flippedCamera_T_world, const Vector2& gridCoordinate) const
{
	ocean_assert(isValid());
	ocean_assert(pinholeCamera.isValid() && flippedCamera_T_world.isValid());

	return world2imageIF(pinholeCamera, flippedCamera_T_world, grid2world<tVirtual>(gridCoordinate));
}

template <bool tVirtual>
inline Vector3 Grid::grid2world(const Vector2& gridCoordinate) const
{
	ocean_assert(isValid());

	return plane2world(grid2plane<tVirtual>(gridCoordinate));
}

template <bool tVirtual>
inline Vector3 Grid::grid2plane(const Vector2& gridCoordinate) const
{
	ocean_assert(isValid());

	if constexpr (tVirtual)
	{
		return plane_T_virtualGrid_ * Vector3(gridCoordinate, 0);
	}
	else
	{
		return plane_T_grid_ * Vector3(gridCoordinate, 0);
	}
}

inline bool Grid::operator==(const Grid& grid) const
{
	return world_T_plane_ == grid.world_T_plane_ && plane_T_grid_ == grid.plane_T_grid_ && plane_T_virtualGrid_ == grid.plane_T_virtualGrid_
		&& gridLowerCorner_ == grid.gridLowerCorner_ && gridUpperCorner_ == grid.gridUpperCorner_;
}

inline bool Grid::operator!=(const Grid& grid) const
{
	return !(*this == grid);
}

inline Vector2 Grid::grid2nearestNode(const Vector2& gridCoordinate)
{
	return Vector2(Scalar(Numeric::round32(gridCoordinate.x())), Scalar(Numeric::round32(gridCoordinate.y())));
}

inline bool Grid::hasTransformationShearComponent(const HomogenousMatrix4& transformation)
{
	return !transformation.xAxis().isOrthogonal(transformation.yAxis());
}

inline bool Grid::isInFrontOfCamera(const HomogenousMatrix4& world_T_camera, const Vector3& worldCoordinate)
{
	return isInFrontOfCameraIF(PinholeCamera::standard2InvertedFlipped(world_T_camera), worldCoordinate);
}

inline bool Grid::isInFrontOfCameraIF(const HomogenousMatrix4& flippedCamera_T_world, const Vector3& worldCoordinate)
{
	return (flippedCamera_T_world * worldCoordinate).z() > Scalar(0.0001);
}

}

}

#endif // META_OCEAN_GEOMETRY_GRID_H
