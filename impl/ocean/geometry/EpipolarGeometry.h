/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_GEOMETRY_EPIPOLAR_GEOMETRY_H
#define META_OCEAN_GEOMETRY_EPIPOLAR_GEOMETRY_H

#include "ocean/geometry/Geometry.h"

#include "ocean/base/Accessor.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/Line2.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace Geometry
{

/**
 * This class implements epipolar geometry functions.
 * @ingroup geometry
 */
class OCEAN_GEOMETRY_EXPORT EpipolarGeometry
{
	public:

		/**
		 * Calculates the fundamental matrix based on corresponding image points in a 'left' and in a 'right' camera image.
		 * The resulting fundamental matrix is defined by the following equation:
		 * <pre>
		 * rightPoint^T * right_F_left * leftPoint = 0
		 * </pre>
		 * @param leftPoints The left image points, must be valid
		 * @param rightPoints The right image points, must be valid
		 * @param correspondences The number of point correspondences, with range [8, infinity)
		 * @param fundamental The resulting fundamental matrix 'right_F_left'
		 * @return True, if succeeded
		 */
		static bool fundamentalMatrix(const Vector2* leftPoints, const Vector2* rightPoints, const size_t correspondences, SquareMatrix3& fundamental);

		/**
		 * Returns the reverted fundamental matrix.
		 * Actually the matrix will be transposed only, because the fundamental matrix is singular.
		 * @param right_F_left The fundamental matrix satisfying the equation rightPoint^T * right_F_left * leftPoint = 0
		 * @return The resulting reverted fundamental matrix 'left_F_right'
		 */
		static inline SquareMatrix3 reverseFundamentalMatrix(const SquareMatrix3& fundamental);

		/**
		 * Calculates the essential matrix based on 6-DOF camera pose between two cameras.
		 * The resulting essential matrix is defined by the following equation:
		 * <pre>
		 * normalizedRightPoint^T * normalizedRight_E_normalizedLeft * normalizedLeftPoint = 0
		 * with normalizedRightPoint = [objectPoint.x() / objectPoint.z(), objectPoint.y() / objectPoint.z(), 1]^T
		 * </pre>
		 * @param rightCamera_T_leftCamera The transformation transforming the left camera to the right camera, with default camera pointing towards the negative z-space with y-axis upwards, must be valid
		 * @return The resulting essential matrix 'normalizedRight_E_normalizedLeft'
		 */
		static SquareMatrix3 essentialMatrix(const HomogenousMatrix4& rightCamera_T_leftCamera);

		/**
		 * Calculates the fundamental matrix from a given essential matrix and the two intrinsic camera matrices.
		 * @param normalizedRight_E_normalizedLeft The essential matrix to convert, must be valid
		 * @param leftIntrinsic The left intrinsic camera matrix, must be valid
		 * @param rightIntrinsic The right intrinsic camera matrix, must be valid
		 * @param fundamental The resulting fundamental matrix 'right_F_left'
		 */
		static SquareMatrix3 essential2fundamental(const SquareMatrix3& normalizedRight_E_normalizedLeft, const SquareMatrix3& leftIntrinsic, const SquareMatrix3& rightIntrinsic);

		/**
		 * Calculates the fundamental matrix from a given essential matrix and the two intrinsic camera matrices.
		 * @param normalizedRight_E_normalizedLeft The essential matrix to convert, must be valid
		 * @param leftCamera The left camera profile defining the projection, must be a pure pinhole model without any distortion parameters
		 * @param rightCamera The right camera profile defining the projection, must be a pure pinhole model without any distortion parameters
		 * @return The resulting fundamental matrix 'right_F_left'
		 */
		static SquareMatrix3 essential2fundamental(const SquareMatrix3& normalizedRight_E_normalizedLeft, const PinholeCamera& leftCamera, const PinholeCamera& rightCamera);

		/**
		 * Calculates the essential matrix from a given fundamental matrix and the two intrinsic camera matrices.
		 * @param right_F_left The fundamental matrix to convert into essential matrix, must be valid
		 * @param leftIntrinsic Left intrinsic camera matrix
		 * @param rightIntrinsic Right intrinsic camera matrix
		 * @return The resulting essential matrix 'normalizedRight_E_normalizedLeft'
		 */
		static SquareMatrix3 fundamental2essential(const SquareMatrix3& right_F_left, const SquareMatrix3& leftIntrinsic, const SquareMatrix3& rightIntrinsic);

		/**
		 * Calculates the essential matrix by the given fundamental matrix and the two cameras.
		 * @param right_F_left The fundamental matrix to convert into essential matrix, must be valid
		 * @param leftCamera The left camera profile defining the projection, must be a pure pinhole model without any distortion parameters
		 * @param rightCamera The right camera profile defining the projection, must be a pure pinhole model without any distortion parameters
		 * @return The resulting essential matrix 'normalizedRight_E_normalizedLeft'
		 */
		static SquareMatrix3 fundamental2essential(const SquareMatrix3& right_F_left, const PinholeCamera& leftCamera, const PinholeCamera& rightCamera);

		/**
		 * Determines the two epipoles corresponding to a fundamental matrix.
		 * This method uses singular values decomposition for the calculation.
		 * @param fundamental The fundamental matrix to extract the epipoles from
		 * @param leftEpipole Resulting left epipole
		 * @param rightEpipole Resulting right epipole
		 * @return True, if succeeded
		 */
		static bool epipoles(const SquareMatrix3& fundamental, Vector2& leftEpipole, Vector2& rightEpipole);

		/**
		 * Determines the two epipoles corresponding to two cameras separated by an extrinsic camera matrix.
		 * The matrix will be calculated by the extrinsic camera matrix of the right camera relative to the left camera,<br>
		 * and the two intrinsic camera matrices of both cameras.
		 * @param extrinsic The extrinsic camera matrix of the right camera relative to the left camera (rightTleft)
		 * @param leftIntrinsic Intrinsic camera matrix of the left camera
		 * @param rightIntrinsic Intrinsic camera matrix of the right camera
		 * @param leftEpipole Resulting left epipole
		 * @param rightEpipole Resulting right epipole
		 * @return True, if succeeded
		 * @see essentialMatrix().
		 */
		static bool epipoles(const HomogenousMatrix4& extrinsic, const SquareMatrix3& leftIntrinsic, const SquareMatrix3& rightIntrinsic, Vector2& leftEpipole, Vector2& rightEpipole);

		/**
		 * Finds the two epipoles corresponding to a fundamental matrix.
		 * This method calculates the intersection of two epipolar lines.
		 * If no intersection can be found the SVD calculation is used.
		 * @param fundamental The fundamental matrix to extract the epipoles from
		 * @param leftEpipole Resulting left epipole
		 * @param rightEpipole Resulting right epipole
		 * @return True, if succeeded
		 */
		static bool epipolesFast(const SquareMatrix3& fundamental, Vector2& leftEpipole, Vector2& rightEpipole);

		/**
		 * Returns the epipolar line in the left image corresponding to a given point in the right image.
		 * @param fundamental The fundamental matrix
		 * @param rightPoint Point in the right image
		 * @return Resulting epipolar line in the left image
		 */
		static inline Line2 leftEpipolarLine(const SquareMatrix3& fundamental, const Vector2& rightPoint);

		/**
		 * Returns the epipolar line in the right image corresponding to a given point in the left image.
		 * @param fundamental The fundamental matrix
		 * @param leftPoint Point in the left image
		 * @return Resulting epipolar line in the right image
		 */
		static inline Line2 rightEpipolarLine(const SquareMatrix3& fundamental, const Vector2& leftPoint);

		/**
		 * Factorizes the essential matrix into rotation and translation.
		 * Beware: The translation can be determined up to a scale factor only.<br>
		 * The resulting factorization provides the extrinsic camera matrix for the right camera while the left camera has the identity extrinsic camera matrix.<br>
		 * Thus, the resulting transformation transforms points defined inside the right camera coordinate system into point defined inside the left camera coordinate system.
		 * @param essential The essential matrix to be factorized
		 * @param leftCamera Left camera corresponding to the left image point, must be valid
		 * @param rightCamera Right camera corresponding to the right image point, must be valid
		 * @param leftPoint One image point inside the left image, this point must correspond to the given right image point
		 * @param rightPoint One image point inside the right image, this point must correspond to the given left image point
		 * @param transformation Resulting transformation of between the left and the right camera
		 * @return True, if succeeded
		 */
		static bool factorizeEssential(const SquareMatrix3& essential, const PinholeCamera& leftCamera, const PinholeCamera& rightCamera, const Vector2& leftPoint, const Vector2& rightPoint, HomogenousMatrix4& transformation);

		/**
		 * Factorizes an essential matrix into a camera pose composed of rotation and translation.
		 * Beware: The translation can be determined up to a scale factor only.<br>
		 * The factorization provides the extrinsic camera matrix (camera pose) for the right camera while the left camera is expected to have the identity as extrinsic camera matrix.<br>
		 * The resulting transformation transforms points defined inside the right camera coordinate system into point defined inside the left camera coordinate system: pointLeft = transformation * pointRight.<br>
		 * @param essential The essential matrix to be factorized
		 * @param leftCamera Left camera corresponding to the left image point, must be valid
		 * @param rightCamera Right camera corresponding to the right image point, must be valid
		 * @param leftPoints All image point inside the left image to be checked whether they produce 3D object points lying in front of the camera
		 * @param rightPoints All image points inside the right image, one for each left point
		 * @param transformation Resulting transformation between the left and the right camera
		 * @return The number of given image points resulting in valid object points, with range [0, infinity)
		 */
		static size_t factorizeEssential(const SquareMatrix3& essential, const PinholeCamera& leftCamera, const PinholeCamera& rightCamera, const Vectors2& leftPoints, const Vectors2& rightPoints, HomogenousMatrix4& transformation);

		/**
		 * Determines the homograph for two (stereo) frames rectifying both images using the transformation between the left and the right camera.
		 * As the resulting homography may not cover the entire input images using the same camera profile a new camera (perfect) profile can be calculated instead.<br>
		 * Thus, the resulting rectified images will have a larger field of view but will cover the entire input frame data.<br>
		 * The projection center of the left camera is expected to be at the origin of the world coordinate system.<br>
		 * The viewing directions of both cameras is towards the negative z-axis in their particular coordinate systems.<br>
		 * The given transformation is equal to the extrinsic camera matrix of the right camera<br>
		 * and thus transforms points defined inside the right camera coordinate system to points defined inside the left camera coordinate system.<br>
		 * The resulting homography transformations transform 3D rectified image points (homogenous 2D coordinates) into 3D unrectified image points for their particular coordinate system.<br>
		 * The coordinate system of the 3D image points has the origin in the top left corner, while the x-axis points to right, the y-axis points to the bottom and the z-axis to the back of the image.<br>
		 * @param transformation Extrinsic camera matrix of the right camera with negative z-axis as viewing direction
		 * @param pinholeCamera The pinhole camera profile used for both images
		 * @param leftHomography Resulting left homography
		 * @param rightHomography Resulting right homography
		 * @param appliedRotation Resulting rotation applied to both cameras
		 * @param newCamera Optional resulting new camera profile used to cover the entire input image data into the output frames, otherwise nullptr
		 * @return True, if succeeded
		 */
		static bool rectificationHomography(const HomogenousMatrix4& transformation, const PinholeCamera& pinholeCamera, SquareMatrix3& leftHomography, SquareMatrix3& rightHomography, Quaternion& appliedRotation, PinholeCamera* newCamera);

		/**
		 * Calculates the 3D positions for a pair of image point correspondences with corresponding extrinsic camera transformations.
		 * @param world_T_cameraA The extrinsic camera transformations of the first camera, with the camera pointing towards the negative z-space, y-axis pointing up, and the x-axis pointing to the right, must be valid
		 * @param world_T_cameraB The extrinsic camera transformations of the second camera, with the camera pointing towards the negative z-space, y-axis pointing up, and the x-axis pointing to the right, must be valid
		 * @param anyCameraA The first camera profile, must be valid
		 * @param anyCameraB The second camera profile, must be valid
		 * @param imagePointsA The set of 2D image points in the first image, each point must correspond to the point with the same index from the second image
		 * @param imagePointsB The set of 2D image points in the second image, each point must correspond to the point with the same index from the first image
		 * @param numberPoints The number of point correspondences, with range [0, infinity)
		 * @param onlyFrontObjectPoints If true, only points that are located in front of the camera will be used for the optimization, otherwise all points will be used.
		 * @param invalidObjectPoint Optional, the location of an invalid object point which will be used as value for all object points which cannot be determined e.g., because of parallel projection rays
		 * @param invalidIndices Optional, the resulting indices of the resulting object points with invalid location
		 * @return objectPoints The resulting triangulated object points
		 */
		static Vectors3 triangulateImagePoints(const HomogenousMatrix4& world_T_cameraA, const HomogenousMatrix4& world_T_cameraB, const AnyCamera& anyCameraA, const AnyCamera& anyCameraB, const Vector2* imagePointsA, const Vector2* imagePointsB, const size_t numberPoints, const bool onlyFrontObjectPoints = true, const Vector3& invalidObjectPoint = Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()), Indices32* invalidIndices = nullptr);

		/**
		 * Calculates the 3D positions for a set of image point correspondences with corresponding poses (Rt) in inverted flipped camera system.
		 * This linear triangulation uses singular value decomposition.<br>
		 * If an object point cannot be determined than the resulting object point will have value (0, 0, 0).
		 * @param camera1 The camera profile used for the first image
		 * @param iFlippedPose1 Given projection matrix for the first camera
		 * @param camera2 The camera profile used for the second image
		 * @param iFlippedPose2 Given projection matrix for the second camera
		 * @param points1 Set of 2D image points in the first image, each point must correspond the one in the right image
		 * @param points2 Set of 2D image points in the second image
		 * @param correspondences  Number of point correspondences, with range [1, infinity)
		 * @param invalidObjectPoint Optional, the location of an invalid object point which will be used as value for all object points which cannot be determined e.g., because of parallel projection rays
		 * @param invalidIndices Optional resulting indices of the resulting object points with invalid location
		 * @return objectPoints Resulting object points in inverted flipped coordinate space
		 */
		static ObjectPoints triangulateImagePointsIF(const PinholeCamera& camera1, const HomogenousMatrix4& iFlippedPose1, const PinholeCamera& camera2, const HomogenousMatrix4& iFlippedPose2, const Vector2* points1, const Vector2* points2, const size_t correspondences, const Vector3& invalidObjectPoint = Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()), Indices32* invalidIndices = nullptr);

		/**
		 * Calculates the 3D positions for a set of image point correspondences in multiple views with corresponding camera projection matrices (K * Rt) or poses (Rt) in inverted flipped camera system.
		 * This linear triangulation uses singular value decomposition.
		 * @param posesIF Given poses or projection matrices per view
		 * @param imagePointsPerPose Set of 2D image points per the view, each point must correspond the one in the other views
		 * @param pinholeCamera The pinhole camera profile, one for all views. If no camera profile is given, posesIF act as projection matrices
		 * @param invalidObjectPoint Optional, the location of an invalid object point which will be used as value for all object points which cannot be determined e.g., because of parallel projection rays
		 * @param invalidIndices Optional resulting indices of the resulting object points with invalid location
		 * @return objectPoints Resulting object points in inverted flipped coordinates
		 */
		static ObjectPoints triangulateImagePointsIF(const ConstIndexedAccessor<HomogenousMatrix4>& posesIF, const ConstIndexedAccessor<Vectors2>& imagePointsPerPose, const PinholeCamera* pinholeCamera = nullptr, const Vector3& invalidObjectPoint = Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()), Indices32* invalidIndices = nullptr);

	protected:

		/**
		 * Determines one transformation from a set of four transformations with most given image point correspondences provide 3D object points in front of the two cameras.
		 * The transformation for the first camera is defined as unit transformation, the four transformation between second and first camera is provided instead.
		 * @param transformation0 First transformation candidate
		 * @param transformation1 Second transformation candidate
		 * @param transformation2 Third transformation candidate
		 * @param transformation3 Fourth transformation candidate
		 * @param leftCamera Left camera profile
		 * @param rightCamera Right camera profile
		 * @param leftPoints Left image points
		 * @param rightPoints Right image points, each point corresponds to one from the left set
		 * @param transformation Resulting transformation with most 3D object points in front of both cameras
		 * @return Number of valid object points
		 */
		static size_t solveAmbiguousTransformations(const HomogenousMatrix4& transformation0, const HomogenousMatrix4& transformation1, const HomogenousMatrix4& transformation2, const HomogenousMatrix4& transformation3, const PinholeCamera& leftCamera, const PinholeCamera& rightCamera, const Vectors2& leftPoints, const Vectors2& rightPoints, HomogenousMatrix4& transformation);

		/**
		 * Returns the number of 3D object points lying in front of two cameras for a given transformation between the two cameras.
		 * The pose of the first camera is located in the origin (identity transformation) while the pose of the second camera is defined by the given transformation.
		 * @param left_T_right The transformation between the right and the left camera, must be valid
		 * @param leftCamera The left camera profile defining the projection, must be valid
		 * @param rightCamera The right camera profile defining the projection, must be valid
		 * @param leftPoints The left image points, must be valid if correspondences != 0
		 * @param rightPoints The right image points, one for each left point, must be valid if correspondences != 0
		 * @param correspondences The number of provided point correspondences, with range [0, infinity)
		 * @return Number of valid object points, with range [0, correspondences]
		 */
		static size_t validateTransformation(const HomogenousMatrix4& left_T_right, const PinholeCamera& leftCamera, const PinholeCamera& rightCamera, const Vector2* leftPoints, const Vector2* rightPoints, const size_t correspondences);

		/**
		 * Converts a epipolar line to a line object.
		 * @param line The epipolar line to be converted
		 * @return Resulting line object
		 */
		static inline Line2 epipolarLine2Line(const Vector3& line);
};

inline SquareMatrix3 EpipolarGeometry::reverseFundamentalMatrix(const SquareMatrix3& fundamental)
{
	return fundamental.transposed();
}

inline Line2 EpipolarGeometry::leftEpipolarLine(const SquareMatrix3& fundamental, const Vector2& rightPoint)
{
	return epipolarLine2Line(fundamental.transposed() * Vector3(rightPoint, 1));
}

inline Line2 EpipolarGeometry::rightEpipolarLine(const SquareMatrix3& fundamental, const Vector2& leftPoint)
{
	return epipolarLine2Line(fundamental * Vector3(leftPoint, 1));
}

inline Line2 EpipolarGeometry::epipolarLine2Line(const Vector3& line)
{
	const Vector2 normal(line[0], line[1]);
	ocean_assert(!normal.isNull());

	const Scalar normalLength = normal.length();
	ocean_assert(Numeric::isNotEqualEps(normalLength));

	/*const Scalar distance = normalLength * line[2];
	const Scalar factor = 1 / normalLength;*/

	return Line2(line / normalLength);

	//return Line2(Vector3(normal.x() * factor, normal.y() * factor, distance *));
}

}

}

#endif // META_OCEAN_GEOMETRY_EPIPOLAR_GEOMETRY_H
