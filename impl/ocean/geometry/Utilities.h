/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_GEOMETRY_UTILITIES_H
#define META_OCEAN_GEOMETRY_UTILITIES_H

#include "ocean/geometry/Geometry.h"

#include "ocean/base/Accessor.h"
#include "ocean/base/Median.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/Cone3.h"
#include "ocean/math/Cylinder3.h"
#include "ocean/math/FisheyeCamera.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Plane3.h"
#include "ocean/math/SquareMatrix2.h"

namespace Ocean
{

namespace Geometry
{

/**
 * This class implements utilities function for the tracking library.
 * @ingroup geometry
 */
class OCEAN_GEOMETRY_EXPORT Utilities
{
	public:

		/**
		 * Projects one image points onto a 3D plane and returns the resulting 3D object point.
		 * Beware: The back projected point may be located behind the camera due to the position and orientation of the given plane!<br>
		 * Beware: Ensure that the given plane is not parallel with the viewing direction of the camera.<br>
		 * @param pinholeCamera The pinhole camera object
		 * @param pose Pose of the camera
		 * @param plane 3D plane that is used to determine the position of the back-projected 3D object point
		 * @param imagePoint Image point that will be back-projected
		 * @param useDistortionParameters True, to use the distortion parameters of the camera
		 * @param frontObjectPoint Optional resulting statement whether the resulting object point is located in front of the camera; True, if so
		 * @return Resulting 3D object point
		 */
		static ObjectPoint backProjectImagePoint(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose, const Plane3& plane, const ImagePoint& imagePoint, const bool useDistortionParameters, bool* frontObjectPoint = nullptr);

		/**
		 * Projects a set of given image points onto a 3D plane and returns the resulting 3D object points.
		 * Beware: The back projected points may be located behind the camera due to the position and orientation of the given plane!<br>
		 * Beware: Ensure that the given plane is not parallel with the viewing direction of the camera.
		 * @param anyCamera The camera profile defining the projection and distortion
		 * @param world_T_camera The transformation from the camera coordinate system to world coordinates, `worldPoint = world_T_camera * cameraPoint`, must be valid
		 * @param plane 3D plane that is used to determine the position of the back-projected 3D object points
		 * @param imagePoints Image points that will be back-projected
		 * @param numberImagePoints Number of given image points
		 * @param frontObjectPointIndices Optional resulting indices of all object points lying in front of the camera
		 * @return Resulting 3D object points
		 * @see backProjectImagePointsDamped().
		 */
		static ObjectPoints backProjectImagePoints(const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_camera, const Plane3& plane, const ImagePoint* imagePoints, const size_t numberImagePoints, Indices32* frontObjectPointIndices = nullptr);

		/**
		 * Deprecated.
		 *
		 * Projects a set of given image points onto a 3D plane and returns the resulting 3D object points.
		 * Optional camera distortion will be clamped at the border of the camera's frame.<br>
		 * Beware: The back projected points may be located behind the camera due to the position and orientation of the given plane!<br>
		 * Beware: Ensure that the given plane is not parallel with the viewing direction of the camera.
		 * @param pinholeCamera The pinhole camera profile defining the projection and optional distortion
		 * @param pose Pose of the camera
		 * @param plane 3D plane that is used to determine the position of the back-projected 3D object points
		 * @param imagePoints Image points that will be back-projected
		 * @param numberImagePoints Number of given image points
		 * @param useDistortionParameters True, to use the distortion parameters of the camera
		 * @param frontObjectPointIndices Optional resulting indices of all object points lying in front of the camera
		 * @return Resulting 3D object points
		 * @see backProjectImagePointsDamped().
		 */
		static ObjectPoints backProjectImagePoints(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose, const Plane3& plane, const ImagePoint* imagePoints, const size_t numberImagePoints, const bool useDistortionParameters, Indices32* frontObjectPointIndices = nullptr);

		/**
		 * Projects a set of given image points onto a 3D cylinder and returns *only* the resulting 3D object points for rays that intersect the cylinder.
		 * @param anyCamera The camera profile defining the projection and distortion
		 * @param world_T_camera The transformation from the camera coordinate system to world coordinates, `worldPoint = world_T_camera * cameraPoint`, must be valid
		 * @param cylinder 3D cylinder that is used to determine the position of the back-projected 3D object points
		 * @param imagePoints Image points that will be back-projected
		 * @param numberImagePoints Number of given image points
		 * @param intersectingPointIndices Output indices of all 2D points whose rays actually intersect with the cylinder
		 * @return Resulting 3D object points
		 */
		static ObjectPoints backProjectImagePoints(const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_camera, const Cylinder3& cylinder, const ImagePoint* imagePoints, const size_t numberImagePoints, Indices32& intersectingPointIndices);

		/**
		 * Deprecated.
		 *
		 * Projects a set of given image points onto a 3D cylinder and returns *only* the resulting 3D object points for rays that intersect the cylinder.
		 * Optional camera distortion will be clamped at the border of the camera's frame.<br>
		 * @param pinholeCamera The pinhole camera profile defining the projection and optional distortion
		 * @param pose Pose of the camera
		 * @param cylinder 3D cylinder that is used to determine the position of the back-projected 3D object points
		 * @param imagePoints Image points that will be back-projected
		 * @param numberImagePoints Number of given image points
		 * @param useDistortionParameters True, to use the distortion parameters of the camera
		 * @param intersectingPointIndices Output indices of all 2D points whose rays actually intersect with the cylinder
		 * @return Resulting 3D object points
		 */
		static ObjectPoints backProjectImagePoints(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose, const Cylinder3& cylinder, const ImagePoint* imagePoints, const size_t numberImagePoints, const bool useDistortionParameters, Indices32& intersectingPointIndices);

		/**
		 * Projects a set of given image points onto a 3D cone and returns *only* the resulting 3D object points for rays that intersect the cone.
		 * @param anyCamera The camera profile defining the projection and distortion
		 * @param world_T_camera The transformation from the camera coordinate system to world coordinates, `worldPoint = world_T_camera * cameraPoint`, must be valid
		 * @param cone 3D cone that is used to determine the position of the back-projected 3D object points
		 * @param imagePoints Image points that will be back-projected
		 * @param numberImagePoints Number of given image points
		 * @param intersectingPointIndices Output indices of all 2D points whose rays actually intersect with the cone
		 * @return Resulting 3D object points
		 */
		static ObjectPoints backProjectImagePoints(const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_camera, const Cone3& cone, const ImagePoint* imagePoints, const size_t numberImagePoints, Indices32& intersectingPointIndices);

		/**
		 * Deprecated.
		 *
		 * Projects a set of given image points onto a 3D cone and returns *only* the resulting 3D object points for rays that intersect the cone.
		 * Optional camera distortion will be clamped at the border of the camera's frame.<br>
		 * @param pinholeCamera The pinhole camera profile defining the projection and optional distortion
		 * @param pose Pose of the camera
		 * @param cone 3D cone that is used to determine the position of the back-projected 3D object points
		 * @param imagePoints Image points that will be back-projected
		 * @param numberImagePoints Number of given image points
		 * @param useDistortionParameters True, to use the distortion parameters of the camera
		 * @param intersectingPointIndices Output indices of all 2D points whose rays actually intersect with the cone
		 * @return Resulting 3D object points
		 */
		static ObjectPoints backProjectImagePoints(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose, const Cone3& cone, const ImagePoint* imagePoints, const size_t numberImagePoints, const bool useDistortionParameters, Indices32& intersectingPointIndices);

		/**
		 * Projects a set of given image points onto a 3D plane and returns the resulting 3D object points.
		 * Optional camera distortion will be damped outside the border of the camera's frame.<br>
		 * Beware: The back projected points may be located behind the camera due to the position and orientation of the given plane!<br>
		 * Beware: Ensure that the given plane is not parallel with the viewing direction of the camera.
		 * @param pinholeCamera The pinhole camera profile defining the projection and optional distortion
		 * @param pose Pose of the camera
		 * @param plane 3D plane that is used to determine the position of the back-projected 3D object points
		 * @param imagePoints Image points that will be back-projected
		 * @param numberImagePoints Number of given image points
		 * @param useDistortionParameters True, to use the distortion parameters of the camera
		 * @param frontObjectPointIndices Optional resulting indices of all object points lying in front of the camera
		 * @return Resulting 3D object points
		 * @see backProjectImagePoints().
		 */
		static ObjectPoints backProjectImagePointsDamped(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose, const Plane3& plane, const ImagePoint* imagePoints, const size_t numberImagePoints, const bool useDistortionParameters, Indices32* frontObjectPointIndices = nullptr);

		/**
		 * Creates a set of 3D object points for a set of given 2D image points.
		 * The 3D object points will lie on the ray which intersects the individual image points and the camera's center of projection.<br>
		 * The distance between 3D object points and the camera's center of projection defines the locations of the object points.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param world_T_camera The pose of the camera, with default camera pointing towards the negative z-space with y-axis upwards, must be valid
		 * @param imagePoints Image points that will be back-projected, at least one
		 * @param distance The distance between the camera's center of project and the resulting object points, with range (0, infinity)
		 * @return The resulting 3D object points, defined in world
		 */
		static Vectors3 createObjectPoints(const AnyCamera& camera, const HomogenousMatrix4& world_T_camera, const ConstIndexedAccessor<Vector2>& imagePoints, const Scalar distance);

		/**
		 * Deprecated.
		 *
		 * Creates a set of 3D object points by a set of given 2D image points.
		 * The 3D object points are lying on the ray which intersect the individual image points and the camera's center of projection.<br>
		 * The distance between 3D object points and the camera's center of projection defines the locations of the object points.<br>
		 * @param pinholeCamera The pinhole camera profile defining the projection, must be valid
		 * @param pose Pose of the camera, must be valid
		 * @param imagePoints Image points that will be back-projected
		 * @param useDistortionParameters True, to use the distortion parameters of the camera
		 * @param distance The distance between the camera's center of project and the resulting object points, should be in range (0, infinity)
		 * @return Resulting 3D object points
		 */
		static ObjectPoints createObjectPoints(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose, const ConstIndexedAccessor<ImagePoint>& imagePoints, const bool useDistortionParameters, const Scalar distance);

		/**
		 * Determines 3D object points by triangulating two sets of 2D image points from different camera poses.
		 * @param camera0 The camera profile of the first camera, must be valid
		 * @param camera1 The camera profile of the second camera, must be valid
		 * @param world_T_camera0 The camera pose of the first camera, with default camera pointing towards the negative z-space with y-axis upwards, must be valid
		 * @param world_T_camera1 The camera pose of the second camera, with default camera pointing towards the negative z-space with y-axis upwards, must be valid
		 * @param imagePoints0 The 2D image points located in the first camera
		 * @param imagePoints1 The 2D image points located in the second camera, each point must have a corresponding point in the first camera
		 * @param objectPoints The resulting valid 3D object points, does not provide invalid object points
		 * @param validIndices The resulting indices of the valid point correspondences for which a 3D object points could be determined, one index of reach resulting 3D object point
		 * @param onlyFrontPoints True, if only front object points are determined
		 * @param maximalSqrError The maximal squared projection pixel error for valid object points
		 */
		static void triangulateObjectPoints(const AnyCamera& camera0, const AnyCamera& camera1, const HomogenousMatrix4& world_T_camera0, const HomogenousMatrix4& world_T_camera1, const ConstIndexedAccessor<Vector2>& imagePoints0, const ConstIndexedAccessor<Vector2>& imagePoints1, Vectors3& objectPoints, Indices32& validIndices, const bool onlyFrontPoints = true, const Scalar maximalSqrError = Scalar(3 * 3));

		/**
		 * Deprecated.
		 *
		 * Determines 3D object points by triangulating two sets of 2D image points from different camera poses.
		 * Beware: The resulting object points can lie behind the camera if the point correspondences or the camera poses are invalid.<br>
		 * @param camera0 The camera profile of the first camera, must be valid
		 * @param camera1 The camera profile of the second camera, must be valid
		 * @param pose0 The camera pose of the first image, must be valid
		 * @param pose1 The camera pose of the second image, must be valid and must point into the same direction as the first camera
		 * @param imagePoints0 The 2D image points located in the first camera
		 * @param imagePoints1 The 2D image points located in the second camera, each point must have a corresponding point in the first camera
		 * @param objectPoints The resulting valid 3D object points, does not provide invalid object points
		 * @param validIndices The resulting indices of the valid point correspondences for which a 3D object points could be determined, one index of reach resulting 3D object point
		 * @param undistortImagePoints True, if the distortion parameters of the camera profiles are used to undistort the image points before triangulation
		 * @param onlyFrontPoints True, if only front object points are determined
		 * @param maximalSqrError The maximal squared projection pixel error for valid object points
		 */
		static void triangulateObjectPoints(const PinholeCamera& camera0, const PinholeCamera& camera1, const HomogenousMatrix4& pose0, const HomogenousMatrix4& pose1, const ConstIndexedAccessor<Vector2>& imagePoints0, const ConstIndexedAccessor<Vector2>& imagePoints1, Vectors3& objectPoints, Indices32& validIndices, const bool undistortImagePoints, const bool onlyFrontPoints = true, const Scalar maximalSqrError = Scalar(3 * 3));

		/**
		 * Projects a set of given 2D image triangles onto a 3D plane and returns the resulting 3D object triangles.
		 * @param pinholeCamera The pinhole camera object
		 * @param pose Pose of the camera
		 * @param plane 3D plane that is used to determine the position of the back-projected 3D object points
		 * @param triangles Triangles that will be back-projected
		 * @param numberTriangles Number of given image points
		 * @param useDistortionParameters True, to use the distortion parameters of the camera
		 * @return Resulting 3D triangles
		 */
		static Triangles3 backProjectTriangles(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose, const Plane3& plane, const Triangle2* triangles, const size_t numberTriangles, const bool useDistortionParameters);

		/**
		 * Counts the number of object points that are in front of a given camera.
		 * @param pinholeCamera The pinhole camera profile to be used
		 * @param pose Pose of the camera (extrinsic camera matrix with viewing direction along the negative z-axis)
		 * @param objectPoints Object points for which the number of front object points will be determined
		 * @param numberObjectPoints The number of provided object points
		 * @return Number of object points located in front of the camera
		 * @see countFrontObjectPointsIF().
		 */
		static inline size_t countFrontObjectPoints(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose, const ObjectPoint* objectPoints, const size_t numberObjectPoints);

		/**
		 * Counts the number of object points that are in front of a given camera.
		 * @param pinholeCamera The pinhole camera profile to be used
		 * @param invertedFlippedPose Inverted and flipped pose of the camera (not the default extrinsic camera matrix)
		 * @param objectPoints Object points for which the number of front object points will be determined
		 * @param numberObjectPoints The number of provided object points
		 * @return Number of object points located in front of the camera
		 * @see countFrontObjectPoints().
		 */
		static size_t countFrontObjectPointsIF(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& invertedFlippedPose, const ObjectPoint* objectPoints, const size_t numberObjectPoints);

		/**
		 * Counts the number of object points that are visible in two individual camera frames and are located in front of both cameras.
		 * @param cameraFirst The camera profile of the first frame, optional distortion parameters of the camera will not be considered
		 * @param cameraSecond The camera profile of the first frame, optional distortion parameters of the camera will not be considered
		 * @param poseFirst Pose of the first camera
		 * @param poseSecond Pose of the second camera
		 * @param imagePointsFirst Image points located in the first frame
		 * @param imagePointsSecond Image points located in the second frame, each point corresponds to one point in the first frame (correspondence is defined by index)
		 * @param correspondences Number of given image point correspondences
		 * @return Number of image points located in front of both camera
		 */
		static size_t countFrontObjectPoints(const PinholeCamera& cameraFirst, const PinholeCamera& cameraSecond, const HomogenousMatrix4& poseFirst, const HomogenousMatrix4& poseSecond, const ImagePoint* imagePointsFirst, const ImagePoint* imagePointsSecond, const size_t correspondences);

		/**
		 * Creates the covariance matrix for a given set of image points.
		 * @param imagePoints Image points for that the covariance will be determined
		 * @param number Number of image points
		 * @param minimalSigma Defining the minimal sigma that is applied, with range [0, infinity)
		 * @return Resulting covariance matrix
		 */
		static inline SquareMatrix2 covarianceMatrix(const ImagePoint* imagePoints, const size_t number, const Scalar minimalSigma = 0);

		/**
		 * Creates the covariance matrix for a given set of image points.
		 * @param imagePoints Image points for that the covariance will be determined
		 * @param number Number of image points
		 * @param meanPoint Already determined mean point of the provided image points
		 * @param minimalSigma Defining the minimal sigma that is applied, with range [0, infinity)
		 * @return Resulting covariance matrix
		 */
		static SquareMatrix2 covarianceMatrix(const ImagePoint* imagePoints, const size_t number, const ImagePoint& meanPoint, const Scalar minimalSigma = 0);

		/**
		 * Creates the covariance matrix for a given set of image points.
		 * @param imagePoints Image points for that the covariance will be determined
		 * @param indices A set of indices that define a subset of the entire image points
		 * @param numberIndices Number of provided indices
		 * @param minimalSigma Defining the minimal sigma that is applied, with range [0, infinity)
		 * @param meanPoint Resulting mean point of the filtered image points
		 * @return Resulting covariance matrix
		 */
		static SquareMatrix2 covarianceMatrix(const ImagePoint* imagePoints, const unsigned int* indices, const size_t numberIndices, const Scalar minimalSigma, ImagePoint& meanPoint);

		/**
		 * Creates a covariance matrix by two given orthogonal vectors.
		 * @param direction0 Direction of the major axis, with unit length
		 * @param sigma0 Standard deviation of the major axis (equivalent to the square root of the first eigenvalue of the covariance matrix)
		 * @param direction1 Direction of the minor axis, perpendicular to the main direction, with unit length
		 * @param sigma1 Standard deviation of the second direction (equivalent to the square root of the second eigenvalue of the covariance matrix)
		 * @return Resulting covariance matrix
		 */
		static SquareMatrix2 covarianceMatrix(const Vector2& direction0, const Scalar sigma0, const Vector2& direction1, const Scalar sigma1);

		/**
		 * Creates a covariance matrix by two given orthogonal vectors.
		 * @param direction0 Direction of the major axis, with standard deviation as length
		 * @param direction1 Direction of the minor axis, perpendicular to the main direction, with standard deviation as length
		 * @return Resulting covariance matrix
		 */
		static SquareMatrix2 covarianceMatrix(const Vector2& direction0, const Vector2& direction1);

		/**
		 * Creates a covariance matrix by one given vector representing the major axis.
		 * @param majorDirection Direction of the major axis with standard deviation as length
		 * @param minimalSigma Minimal sigma that will be used (will be used if the length of the major direction is too small)
		 * @param minorFactor Ratio between the sigma of the minor and the major sigma, with range (0, 1]
		 * @return Resulting covariance matrix
		 */
		static SquareMatrix2 covarianceMatrix(const Vector2& majorDirection, const Scalar minimalSigma = 1, const Scalar minorFactor = Scalar(0.01));

		/**
		 * Decomposes a covariance matrix.
		 * @param covarianceMatrix Covariance matrix that has to be decomposed
		 * @param direction0 Resulting major axis, with standard deviation as length (the square root of the first Eigen value)
		 * @param direction1 Resulting minor axis, with standard deviation as length (the square root of the second Eigen value)
		 * @return True, if succeeded
		 */
		static bool decomposeCovarianceMatrix(const SquareMatrix2& covarianceMatrix, Vector2& direction0, Vector2& direction1);

		/**
		 * Returns whether a polygon of given 2D points is convex.
		 * A polygon consisting of 0 to 2 vertices is considered to be convex.
		 * @param vertices The vertices of the polygon, can be nullptr if size == 0
		 * @param size Number of vertices in the polygon, range: [0, infinity)
		 * @param strict If true, the polygon is tested for strict convexity (i.e. every internal angle is less than 180 degrees), otherwise internal angles can be up equal to 180 degrees
		 * @return True if the polygon is convex, otherwise false
		 */
		static bool isPolygonConvex(const Vector2* vertices, const size_t size, const bool strict = true);

		/**
		 * Compute the area of a polygon
		 * @param vertices The vertices of a polygon (can be convex or concave but must not be self-intersecting)
		 * @param size Number of vertices in the polygon, range: [0, infinity)
		 * @return The absolute value of the area of the polygon (this value is undefined if the polygon is self-intersecting), range: [0, infinity)
		 */
		static inline Scalar computePolygonArea(const Vector2* vertices, size_t size);

		/**
		 * Compute the area of a polygon
		 * @param vertices The vertices of a polygon (can be convex or concave but must not be self-intersecting)
		 * @return The absolute value of the area of the polygon (this value is undefined if the polygon is self-intersecting), range: [0, infinity)
		 * @sa CV::Segmentation::PixelContour::area()
		 */
		static inline Scalar computePolygonArea(const Vectors2& vertices);

		/**
		 * Compute the signed area of a polygon
		 * The value of the area will be positive if the vertices are in counter-clockwise order and negative if they are in clock-wise order.
		 * @param vertices The vertices of a polygon (can be convex or concave but must not be self-intersecting)
		 * @param size Number of vertices in the polygon, range: [0, infinity)
		 * @return The area of the polygon (this value is undefined if the polygon is self-intersecting), range: (-infinity, infinity)
		 * @sa CV::Segmentation::PixelContour::areaSigned()
		 */
		static Scalar computePolygonAreaSigned(const Vector2* vertices, size_t size);

		/**
		 * Compute the signed area of a polygon
		 * The value of the area will be positive if the vertices are in counter-clockwise order and negative if they are in clock-wise order.
		 * @param vertices The vertices of a polygon (can be convex or concave but must not be self-intersecting)
		 * @return The area of the polygon (this value is undefined if the polygon is self-intersecting), range: (-infinity, infinity)
		 */
		static inline Scalar computePolygonAreaSigned(const Vectors2& vertices);

		/**
		 * Returns whether a given point lies inside a convex polygon.
		 * For polygons consisting of less than 3 points, this function always returns false.
		 * A point located on an edge of the polygon is considered as inside the polygon.
		 * @param vertices The vertices of a convex polygon, can be nullptr if size == 0
		 * @param size The number of vertices in the polygon, range: [0, infinity)
		 * @param point The point to be tested
		 * @return True, if the point is inside the polygon, otherwise false
		 */
		static bool isInsideConvexPolygon(const Vector2* vertices, size_t size, const Vector2& point);

		/**
		 * Returns whether a given point lies inside a convex polygon.
		 * For polygons consisting of less than 3 points, this function always returns false.
		 * A point located on an edge of the polygon is considered as inside the polygon.
		 * @param vertices The vertices of a convex polygon, can be nullptr if size == 0
		 * @param point The point to be tested
		 * @return True, if the point is inside the polygon, otherwise false
		 */
		static inline bool isInsideConvexPolygon(const Vectors2& vertices, const Vector2& point);

		/**
		 * Computation of the intersection of two convex polygons
		 * @param vertices0 The vertices of the first polygon
		 * @param size0 The number of vertices in the first polygon, range: [3, infinity)
		 * @param vertices1 The vertices of the second polygon
		 * @param size1 The number of vertices of the second polygon, range: [3, infinity)
		 * @param intersectedPolygon The resulting intersection represented as a polygon
		 * @return True, if succeeded
		 */
		static bool intersectConvexPolygons(const Vector2* vertices0, const size_t size0, const Vector2* vertices1, const size_t size1, Vectors2& intersectedPolygon);

		/**
		 * Computation of the intersection of two convex polygons
		 * @param vertices0 The vertices of the first polygon, size: [3, infinity)
		 * @param vertices1 The vertices of the second polygon, size: [3, infinity)
		 * @param intersectedPolygon The resulting intersection represented as a polygon
		 * @return True, if succeeded
		 */
		static inline bool intersectConvexPolygons(const Vectors2& vertices0, const Vectors2& vertices1, Vectors2& intersectedPolygon);

		/**
		 * Returns the mean position of a set of given 2D points.
		 * @param imagePointAccessor The accessor providing the image points, at least one
		 * @return The resulting mean position
		 * @tparam TAccessor The data type of the accessor providing the image points
		 */
		template <typename TAccessor>
		static inline ImagePoint meanImagePoint(const TAccessor& imagePointAccessor);

		/**
		 * Returns the mean position of a set of given 3D points.
		 * @param objectPointAccessor The accessor for the object points, at least one
		 * @return The resulting median position
		 * @tparam TAccessor The data type of the accessor providing the object points
		 */
		template <typename TAccessor>
		static inline ObjectPoint meanObjectPoint(const TAccessor& objectPointAccessor);

		/**
		 * Returns the median position of a set of given 2D points.
		 * The resulting position is determined by a component-wise median determination.
		 * @param imagePointAccessor The accessor for the image points, at least one
		 * @return The resulting median position
		 * @tparam TAccessor The data type of the accessor providing the object points
		 */
		template <typename TAccessor>
		static inline ImagePoint medianImagePoint(const TAccessor& imagePointAccessor);

		/**
		 * Returns the median position of a set of given 3D points.
		 * The resulting position is determined by a component-wise median determination.
		 * @param objectPointAccessor The accessor for the object points, at least one
		 * @return The resulting median position
		 * @tparam TAccessor The data type of the accessor providing the object points
		 */
		template <typename TAccessor>
		static inline ObjectPoint medianObjectPoint(const TAccessor& objectPointAccessor);

		/**
		 * Returns the median distance between a given 2D object point and a set of given 2D points.
		 * @param imagePoint The image point to which the median distance will be determined
		 * @param imagePointAccessor The accessor for the image points, at least one
		 * @return The resulting median distance
		 * @tparam TAccessor The data type of the accessor providing the image points
		 */
		template <typename TAccessor>
		static inline Scalar medianDistance(const ImagePoint& imagePoint, const TAccessor& imagePointAccessor);

		/**
		 * Returns the median distance between a given 3D object point and a set of given 3D points.
		 * @param objectPoint The object point to which the median distance will be determined
		 * @param objectPointAccessor The accessor for the object points, at least one
		 * @return The resulting median distance
		 * @tparam TAccessor The data type of the accessor providing the object points
		 */
		template <typename TAccessor>
		static inline Scalar medianDistance(const ObjectPoint& objectPoint, const TAccessor& objectPointAccessor);

		/**
		 * Create a random homography transformation
		 * The homography is created in such a way that points in an output image are mapped into the input image, i.e., inputPoints_i = homography * outputPoints_i
		 * @param width The width of the input image, range: [1, infinity)
		 * @param height The height of the input image, range: [1, infinity)
		 * @param maxTranslation A random global translation that is added to the random homography
		 * @return A random homography
		 */
		static SquareMatrix3 createRandomHomography(const unsigned int width, const unsigned height, const Scalar maxTranslation);

		/**
		 * Deprecated.
		 *
		 * Returns a random 6-DOF camera pose for a pinhole camera which observes a given 3D object point at a specified 2D image point location while having a specified distance to the 3D object point.
		 * @param pinholeCamera The pinhole camera profile defining the projection, must be valid
		 * @param worldObjectPointRay The 3D ray starting at the 3D object point and pointing towards the camera's center of projection, must be valid
		 * @param distortedImagePoint The 2D image point within the fisheye camera image, with range [0, fisheyeCamera.width())x[0, fisheyeCamera.height())
		 * @param distance The desired distance between 3D object point and camera (center of projection), with range (0, infinity)
		 * @return The 6-DOF camera pose matching with the specified constraints, will be world_T_camera
		 */
		static HomogenousMatrix4 randomCameraPose(const PinholeCamera& pinholeCamera, const Line3& worldObjectPointRay, const Vector2& distortedImagePoint, const Scalar distance);

		/**
		 * Deprecated.
		 *
		 * Returns a random 6-DOF camera pose for a fisheye camera which observes a given 3D object point at a specified 2D image point location while having a specified distance to the 3D object point.
		 * @param fisheyeCamera The fisheye camera profile defining the projection, must be valid
		 * @param worldObjectPointRay The 3D ray starting at the 3D object point and pointing towards the camera's center of projection, must be valid
		 * @param distortedImagePoint The 2D image point within the fisheye camera image, with range [0, fisheyeCamera.width())x[0, fisheyeCamera.height())
		 * @param distance The desired distance between 3D object point and camera (center of projection), with range (0, infinity)
		 * @return The 6-DOF camera pose matching with the specified constraints, will be world_T_camera
		 */
		static HomogenousMatrix4 randomCameraPose(const FisheyeCamera& fisheyeCamera, const Line3& worldObjectPointRay, const Vector2& distortedImagePoint, const Scalar distance);

		/**
		 * Returns a random 6-DOF camera pose for any camera which observes a given 3D object point at a specified 2D image point location while having a specified distance to the 3D object point.
		 * @param anyCamera The camera profile defining the projection, must be valid
		 * @param worldObjectPointRay The 3D ray starting at the 3D object point and pointing towards the camera's center of projection, must be valid
		 * @param distortedImagePoint The 2D image point within the camera image, with range [0, anyCamera.width())x[0, anyCamera.height())
		 * @param distance The desired distance between 3D object point and camera (center of projection), with range (0, infinity)
		 * @return The 6-DOF camera pose matching with the specified constraints, will be world_T_camera
		 */
		static HomogenousMatrix4 randomCameraPose(const AnyCamera& anyCamera, const Line3& worldObjectPointRay, const Vector2& distortedImagePoint, const Scalar distance);
};

inline size_t Utilities::countFrontObjectPoints(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose, const ObjectPoint* objectPoints, const size_t numberObjectPoints)
{
	ocean_assert(pose.isValid());

	return countFrontObjectPointsIF(pinholeCamera, PinholeCamera::standard2InvertedFlipped(pose), objectPoints, numberObjectPoints);
}

inline SquareMatrix2 Utilities::covarianceMatrix(const ImagePoint* imagePoints, const size_t number, const Scalar minimalSigma)
{
	return covarianceMatrix(imagePoints, number, meanImagePoint(ConstTemplateArrayAccessor<ImagePoint>(imagePoints, number)), minimalSigma);
}

inline Scalar Utilities::computePolygonArea(const Vector2* vertices, size_t size)
{
	return Numeric::abs(computePolygonAreaSigned(vertices, size));
}

inline Scalar Utilities::computePolygonArea(const Vectors2& vertices)
{
	return Numeric::abs(computePolygonAreaSigned(vertices.data(), vertices.size()));
}

inline Scalar Utilities::computePolygonAreaSigned(const Vectors2& vertices)
{
	return computePolygonAreaSigned(vertices.data(), vertices.size());
}

inline bool Utilities::isInsideConvexPolygon(const Vectors2& vertices, const Vector2& point)
{
	return isInsideConvexPolygon(vertices.data(), vertices.size(), point);
}

inline bool Utilities::intersectConvexPolygons(const Vectors2& vertices0, const Vectors2& vertices1, Vectors2& intersectedPolygon)
{
	return intersectConvexPolygons(vertices0.data(), vertices0.size(), vertices1.data(), vertices1.size(), intersectedPolygon);
}

template <typename TAccessor>
inline ImagePoint Utilities::meanImagePoint(const TAccessor& imagePointAccessor)
{
	ocean_assert(imagePointAccessor.size() > 0);

	ImagePoint meanPosition(0, 0);
	for (size_t n = 0; n < imagePointAccessor.size(); ++n)
	{
		meanPosition += imagePointAccessor[n];
	}

	return meanPosition / Scalar(imagePointAccessor.size());
}

template <typename TAccessor>
inline ObjectPoint Utilities::meanObjectPoint(const TAccessor& objectPointAccessor)
{
	ocean_assert(objectPointAccessor.size() > 0);

	ObjectPoint meanPosition(0, 0, 0);
	for (size_t n = 0; n < objectPointAccessor.size(); ++n)
	{
		meanPosition += objectPointAccessor[n];
	}

	return meanPosition / Scalar(objectPointAccessor.size());
}

template <typename TAccessor>
inline ImagePoint Utilities::medianImagePoint(const TAccessor& imagePointAccessor)
{
	ocean_assert(!imagePointAccessor.isEmpty());

	if (imagePointAccessor.size() == 1)
	{
		return imagePointAccessor[0];
	}

	Scalars xValues(imagePointAccessor.size());
	Scalars yValues(imagePointAccessor.size());

	for (size_t n = 0; n < imagePointAccessor.size(); ++n)
	{
		const ImagePoint& imagePoint = imagePointAccessor[n];

		xValues[n] = imagePoint.x();
		yValues[n] = imagePoint.y();
	}

	const Scalar xMedian = Median::median(xValues.data(), xValues.size());
	const Scalar yMedian = Median::median(yValues.data(), yValues.size());

	return ImagePoint(xMedian, yMedian);
}

template <typename TAccessor>
inline ObjectPoint Utilities::medianObjectPoint(const TAccessor& objectPointAccessor)
{
	ocean_assert(!objectPointAccessor.isEmpty());

	if (objectPointAccessor.size() == 1)
	{
		return objectPointAccessor[0];
	}

	Scalars xValues(objectPointAccessor.size());
	Scalars yValues(objectPointAccessor.size());
	Scalars zValues(objectPointAccessor.size());

	for (size_t n = 0; n < objectPointAccessor.size(); ++n)
	{
		const ObjectPoint& objectPoint = objectPointAccessor[n];

		xValues[n] = objectPoint.x();
		yValues[n] = objectPoint.y();
		zValues[n] = objectPoint.z();
	}

	const Scalar xMedian = Median::median(xValues.data(), xValues.size());
	const Scalar yMedian = Median::median(yValues.data(), yValues.size());
	const Scalar zMedian = Median::median(zValues.data(), zValues.size());

	return ObjectPoint(xMedian, yMedian, zMedian);
}

template <typename TAccessor>
inline Scalar Utilities::medianDistance(const ImagePoint& imagePoint, const TAccessor& imagePointAccessor)
{
	Scalars sqrDistances;
	sqrDistances.reserve(imagePointAccessor.size());

	for (size_t n = 0; n < imagePointAccessor.size(); ++n)
	{
		sqrDistances.push_back(imagePoint.sqrDistance(imagePointAccessor[n]));
	}

	return Numeric::sqrt(Median::median(sqrDistances.data(), sqrDistances.size()));
}

template <typename TAccessor>
inline Scalar Utilities::medianDistance(const ObjectPoint& objectPoint, const TAccessor& objectPointAccessor)
{
	Scalars sqrDistances;
	sqrDistances.reserve(objectPointAccessor.size());

	for (size_t n = 0; n < objectPointAccessor.size(); ++n)
	{
		sqrDistances.push_back(objectPoint.sqrDistance(objectPointAccessor[n]));
	}

	return Numeric::sqrt(Median::median(sqrDistances.data(), sqrDistances.size()));
}

}

}

#endif // META_OCEAN_GEOMETRY_UTILITIES_H
