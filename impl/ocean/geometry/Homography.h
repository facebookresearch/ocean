/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_GEOMETRY_HOMOGRAPHY_H
#define META_OCEAN_GEOMETRY_HOMOGRAPHY_H

#include "ocean/geometry/Geometry.h"
#include "ocean/geometry/Normalization.h"

#include "ocean/base/Accessor.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Plane3.h"
#include "ocean/math/SquareMatrix3.h"

namespace Ocean
{

namespace Geometry
{

/**
 * This class implements functions necessary for computations with homographies.
 * @ingroup geometry
 */
class OCEAN_GEOMETRY_EXPORT Homography
{
	public:

		/**
		 * Calculates the homography between two images transforming the projected planar object points between the two images.
		 * The left camera is expected to be not rotated (the camera has the default viewing direction).<br>
		 * The default camera viewing direction is defined along the negative z-axis and has the y-axis as up vector.<br>
		 * The resulting homography transforms image points defined in the left image to image points defined in the right image (rightPoint = H * leftPoint).
		 * @param left_T_right The rotation between right camera and left camera, must be valid
		 * @param leftCamera The left camera profile, must be valid
		 * @param rightCamera The right camera profile, must be valid
		 * @return Resulting homography between left and right points (right_H_left)
		 */
		static SquareMatrix3 homographyMatrix(const Quaternion& left_T_right, const AnyCamera& leftCamera, const AnyCamera& rightCamera);

		/**
		 * Deprecated.
		 *
		 * Calculates the homography between two images transforming the projected planar object points between the two images.
		 * The left camera is expected to be not rotated (the camera has the default viewing direction).<br>
		 * The default camera viewing direction is defined along the negative z-axis and has the y-axis as up vector.<br>
		 * The resulting homography transforms image points defined in the left image to image points defined in the right image (rightPoint = H * leftPoint).
		 * @param left_T_right The rotation between right camera and left camera, must be valid
		 * @param leftCamera The left camera profile, must be valid
		 * @param rightCamera The right camera profile, must be valid
		 * @return Resulting homography between left and right points (right_H_left)
		 */
		static SquareMatrix3 homographyMatrix(const Quaternion& left_T_right, const PinholeCamera& leftCamera, const PinholeCamera& rightCamera);

		/**
		 * Calculates the homography between two images transforming the projected planar object points between the two images.
		 * The default camera viewing direction is defined along the negative z-axis and has the y-axis as up vector.<br>
		 * The resulting homography transforms image points defined in the left image to image points defined in the right image (rightPoint = H * leftPoint).
		 * @param world_R_left The rotation between left camera and world, must be valid
		 * @param world_R_right The rotation between right camera and world, must be valid
		 * @param leftCamera The left camera profile, must be valid
		 * @param rightCamera The right camera profile, must be valid
		 * @return Resulting homography between left and right points (right_H_left)
		 */
		static SquareMatrix3 homographyMatrix(const Quaternion& world_R_left, const Quaternion& world_R_right, const PinholeCamera& leftCamera, const PinholeCamera& rightCamera);

		/**
		 * Calculates the homography between two images transforming the projected planar object points between the two images.
		 * The left camera is expected to be in the world origin (an identity camera pose).<br>
		 * Further, the given plane must be defined in relation to the world.
		 * @param world_T_rightCamera The camera pose of the right camera, with default camera pointing towards the negative z-space with y-axis upwards, must be valid
		 * @param leftCamera The camera profile defining the projection of the left camera, must be valid
		 * @param rightCamera The camera profile defining the projection of the right camera, must be valid
		 * @param plane The plane defined in world, must be valid
		 * @return The resulting homography transforming points in the left image to points in the right image (right_H_left)
		 */
		static SquareMatrix3 homographyMatrix(const HomogenousMatrix4& world_T_rightCamera, const PinholeCamera& leftCamera, const PinholeCamera& rightCamera, const Plane3& plane);

		/**
		 * Calculates the homography between two images transforming the projected planar object points between the two images.
		 * The given plane must be defined in relation to the left camera coordinate system.<br>
		 * The resulting homography transforms image points defined in the left image to image points defined in the right image (rightPoint = H * leftPoint).<br>
		 * @param poseLeft The camera pose of the left camera in relation to a world coordinate system
		 * @param poseRight The camera pose of the right camera in relation to a world coordinate system
		 * @param leftCamera Left camera profile
		 * @param rightCamera Right camera profile
		 * @param plane The plane defined in the coordinate system of the left camera
		 * @return Resulting homography
		 */
		static SquareMatrix3 homographyMatrix(const HomogenousMatrix4& poseLeft, const HomogenousMatrix4& poseRight, const PinholeCamera& leftCamera, const PinholeCamera& rightCamera, const Plane3& plane);

		/**
		 * Calculates the homography that transforms points defined in a (planar) pattern image to points defined in a camera frame in which the pattern is visible.
		 * Thus, the resulting homography can be used to rectify a specific image content of a given live camera frame.<br>
		 * The resolution and aspect ratio of the resulting rectified frame (defined by 'patternWidth' and 'patternHeight') can be arbitrary.<br>
		 * The 3D coordinates of the corners of the planar pattern must be known (in relation to the given camera pose).<br>
		 * The given 6DOF pose is a standard extrinsic camera matrix with default transformation looking into the negative z-space with y-axis as up-vector.<br>
		 * The resulting homography transforms image points defined in the pattern image to image points defined in the camera frame (cameraPoint = H * patternPoint).<br>
		 * Beware: The resulting transformation does not cover the possible distortion of the camera profile.
		 * @param pinholeCamera The profile of the pinhole camera defining the project, must be valid
		 * @param pose The pose of the camera frame in which the pattern is visible, defined in relation to the world coordinate system must be valid
		 * @param patternObjectPointTopLeft The 3D location of the upper left corner of the pattern, defined in the world coordinate system, must be located in front of the camera, must be different from all other corners
		 * @param patternObjectPointBottomLeft The 3D location of the bottom left corner of the pattern, defined in the world coordinate system, must be located in front of the camera, must be different from all other corners
		 * @param patternObjectPointTopRight The 3D location of the upper right corner of the pattern, defined in the world coordinate system, must be located in front of the camera, must be different from all other corners
		 * @param patternWidth The width of the pattern frame in pixel space (e.g., the width of a resulting frame that shows the rectified pattern), with range [1, infinity)
		 * @param patternHeight The height of the pattern frame in pixel space (e.g., the width of a resulting frame that shows the rectified pattern), with range [1, infinity)
		 * @return The resulting homography transforming pattern points to camera points (cameraPoint = H * patternPoint)
		 * @see CV::Advanced::FrameRectification.
		 */
		static SquareMatrix3 homographyMatrix(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose, const Vector3& patternObjectPointTopLeft, const Vector3& patternObjectPointBottomLeft, const Vector3& patternObjectPointTopRight, const unsigned int patternWidth, const unsigned int patternHeight);

		/**
		 * Calculates the homography (8DOF - translation, rotation, scale, aspect ratio, shear, perspective) between two images transforming the projected planar object points between the two images.
		 * This function either uses a SVD for the determination of a two-step approach which is several times faster.<br>
		 * The resulting homography transforms image points defined in the left image to image points defined in the right image (rightPoint = H * leftPoint).<br>
		 * As this function applies the image points in pixel space, the intrinsic camera matrices of both frames are not necessary.
		 * @param leftPoints Image points in the left camera, each point corresponds to one point in the right image
		 * @param rightPoints Image points in the right camera
		 * @param correspondences Number of points correspondences, with range [4, infinity)
		 * @param right_H_left Resulting homography for the given point correspondences (rightPoint = right_H_left * leftPoint)
		 * @param useSVD True, to use the slower SVD approach (i.e., homographyMatrixSVD); False, to use the two-step approach (i.e., homographyMatrixLinearWithOptimizations)
		 * @return True, if succeeded
		 * @see homographyMatrixSVD(), homographyMatrixLinear(), affineMatrix(), similarityMatrix().
		 */
		static inline bool homographyMatrix(const Vector2* leftPoints, const Vector2* rightPoints, const size_t correspondences, SquareMatrix3& right_H_left, const bool useSVD = true);

		/**
		 * Calculates the homography (8DOF - translation, rotation, scale, aspect ratio, shear, perspective) between two images transforming the projected planar object points between the two images.
		 * This function uses a singular value decomposition for the determination of the homography.<br>
		 * The resulting homography transforms image points defined in the left image to image points defined in the right image (rightPoint = H * leftPoint).<br>
		 * As this function applies the image points in pixel space, the intrinsic camera matrices of both frames are not necessary.
		 * @param leftPoints Image points in the left camera, each point corresponds to one point in the right image
		 * @param rightPoints Image points in the right camera
		 * @param correspondences Number of provided point correspondences, with range [4, infinity)
		 * @param right_H_left Resulting homography for the given point correspondences (rightPoint = right_H_left * leftPoint)
		 * @return True, if succeeded
		 * @see homographyMatrixLinear(), affineMatrix(), similarityMatrix().
		 */
		static bool homographyMatrixSVD(const Vector2* leftPoints, const Vector2* rightPoints, const size_t correspondences, SquareMatrix3& right_H_left);

		/**
		 * Calculates the homography (8DOF - translation, rotation, scale, aspect ratio, shear, perspective) between two images transforming the projected planar object points and lines between the two images.
		 * This function uses a singular value decomposition for the determination of the homography.<br>
		 * The resulting homography transforms image points defined in the left image to image points defined in the right image, and lines defined in the left image to lines defined in the right image, (rightPoint = H * leftPoint).<br>
		 * @param leftPoints Image points in the left camera, each point corresponds to one point in the right image
		 * @param rightPoints Image points in the right camera
		 * @param pointCorrespondences Number of provided point correspondences, with range [0, infinity), while pointCorrespondences + lineCorrespondences >= 4
		 * @param leftLines Lines in the left camera, each line corresponds to one line in the right image
		 * @param rightLines Lines in the right camera, each line
		 * @param lineCorrespondences Number of provided line correspondences, with range [0, infinity), while pointCorrespondences + lineCorrespondences >= 4
		 * @param right_H_left Resulting homography for the given point correspondences (rightPoint = right_H_left * leftPoint)
		 * @return True, if succeeded
		 * @see homographyMatrixLinear(), affineMatrix(), similarityMatrix().
		 */
		static bool homographyMatrixFromPointsAndLinesSVD(const Vector2* leftPoints, const Vector2* rightPoints, const size_t pointCorrespondences, const Line2* leftLines, const Line2* rightLines, const size_t lineCorrespondences, SquareMatrix3& right_H_left);

		/**
		 * Calculates the homography (8DOF - translation, rotation, scale, aspect ratio, shear, perspective) between two images transforming the projected planar object points between the two images.
		 * This function uses a linear equation to determine an initial homography followed by a non-linear optimization.<br>
		 * The initial homography is not as accurate as the homography determined by a SVD; however, the following optimization steps fix the accuracy while being several magnitudes faster.<br>
		 * The resulting homography transforms image points defined in the left image to image points defined in the right image (rightPoint = H * leftPoint).<br>
		 * As this function applies the image points in pixel space, the intrinsic camera matrices of both frames are not necessary.
		 * @param leftPoints Image points in the left camera, each point corresponds to one point in the right image
		 * @param rightPoints Image points in the right camera
		 * @param correspondences Number of points correspondences, with range [4, infinity)
		 * @param right_H_left Resulting homography for the given point correspondences (rightPoint = right_H_left * leftPoint)
		 * @param optimizationIterations The number of non-linear-optimization iterations that will be executed after the initial homography has been determined, with range [0, infinity).
		 * @return True, if succeeded
		 * @see homographyMatrixLinearWithOptimizations(), homographyMatrixLinearWithoutOptimations(), homographyMatrixSVD(), affineMatrix(), similarityMatrix().
		 */
		static bool homographyMatrixLinear(const Vector2* leftPoints, const Vector2* rightPoints, const size_t correspondences, SquareMatrix3& right_H_left, unsigned int optimizationIterations);

		/**
		 * Calculates the homography (8DOF - translation, rotation, scale, aspect ratio, shear, perspective) between two images transforming the projected planar object points between the two images.
		 * This function uses a linear equation to determine an initial homography followed by a non-linear optimization.<br>
		 * The initial homography is not as accurate as the homography determined by a SVD; however, the following 10 optimization steps fix the accuracy while being several magnitudes faster (at least for a high number of correspondences).<br>
		 * The resulting homography transforms image points defined in the left image to image points defined in the right image (rightPoint = H * leftPoint).<br>
		 * As this function applies the image points in pixel space, the intrinsic camera matrices of both frames are not necessary.
		 * @param leftPoints Image points in the left camera, each point corresponds to one point in the right image
		 * @param rightPoints Image points in the right camera
		 * @param correspondences Number of points correspondences, with range [4, infinity)
		 * @param right_H_left Resulting homography for the given point correspondences (rightPoint = right_H_left * leftPoint)
		 * @return True, if succeeded
		 * @see homographyMatrixSVD(), homographyMatrixLinearWithoutOptimations(), affineMatrix(), similarityMatrix().
		 */
		static inline bool homographyMatrixLinearWithOptimizations(const Vector2* leftPoints, const Vector2* rightPoints, const size_t correspondences, SquareMatrix3& right_H_left);

		/**
		 * Calculates the homography (8DOF - translation, rotation, scale, aspect ratio, shear, perspective) between two images transforming the projected planar object points between the two images.
		 * This function uses a linear equation to determine an initial homography followed by a non-linear optimization.<br>
		 * The resulting homography transforms image points defined in the left image to image points defined in the right image (rightPoint = H * leftPoint).<br>
		 * As this function applies the image points in pixel space, the intrinsic camera matrices of both frames are not necessary.
		 * @param leftPoints Image points in the left camera, each point corresponds to one point in the right image
		 * @param rightPoints Image points in the right camera
		 * @param correspondences Number of points correspondences, with range [4, infinity)
		 * @param right_H_left Resulting homography for the given point correspondences (rightPoint = right_H_left * leftPoint)
		 * @return True, if succeeded
		 * @see homographyMatrixSVD(), homographyMatrixLinearWithOptimizations(), affineMatrix(), similarityMatrix().
		 */
		static bool homographyMatrixLinearWithoutOptimations(const Vector2* leftPoints, const Vector2* rightPoints, const size_t correspondences, SquareMatrix3& right_H_left);

		/**
		 * Calculates the affine transformation (6DOF - translation, rotation, scale, aspect ratio, shear) between two sets of 2D image points.
		 * @param leftPoints The image points in the left image, must be valid
		 * @param rightPoints The image points in the right image, one for each point in the left image, must be valid
		 * @param correspondences The number of points correspondences, with range [3, infinity)
		 * @param right_A_left The resulting affine transformation matrix transforming left to right image points
		 * @return True, if succeeded
		 * @see homographyMatrix(), similarityMatrix().
		 */
		static bool affineMatrix(const ImagePoint* leftPoints, const ImagePoint* rightPoints, const size_t correspondences, SquareMatrix3& right_A_left);

		/**
		 * Calculates the similarity transformation (4DOF - translation, rotation, scale) between two images transforming the projected planar object points between the two images.
		 * The resulting similarity matrix transforms image points defined in the left image to image points defined in the right image (rightPoint = S * leftPoint).<br>
		 * The resulting 3x3 matrix has the following layout:
		 * <pre>
		 * | a  -b  tx |
		 * | b   a  ty |
		 * | 0   0   1 |
		 * </pre>
		 * With 'a' and 'b' defining scale and rotation and 'tx' and 'ty' defining the translation.
		 * As this function applies the image points in pixel space, the intrinsic camera matrices of both frames are not necessary.
		 * @param leftPoints Image points in the left camera, each point corresponds to one point in the right image
		 * @param rightPoints Image points in the right camera
		 * @param correspondences Number of points correspondences, with range [2, infinity)
		 * @param right_S_left The resulting similarity transformation matrix transforming left points to right points
		 * @return True, if succeeded
		 * @see homographyMatrix(), affineMatrix(), homotheticMatrix().
		 */
		static bool similarityMatrix(const ImagePoint* leftPoints, const ImagePoint* rightPoints, const size_t correspondences, SquareMatrix3& right_S_left);

		/**
		 * Calculates the homothetic transformation (3DOF - translation,  scale) between two sets of image points.
		 * The resulting homothetic matrix transforms image points defined in the left image to image points defined in the right image (rightPoint = H * leftPoint).<br>
		 * The resulting 3x3 matrix has the following layout:
		 * <pre>
		 * | s  0  tx |
		 * | 0  s  ty |
		 * | 0  0  1 |
		 * </pre>
		 * With 's' defining scale  and 'tx' and 'ty' defining the translation.
		 * @param leftPoints Image points in the left image, each point corresponds to one point in the right image
		 * @param rightPoints Image points in the right image, each point corresponds to one point in the left image
		 * @param correspondences Number of points correspondences, with range [2, infinity)
		 * @param right_H_left Resulting homothetic transformation matrix for the given image points (rightPoint = H * leftPoint)
		 * @return True, if succeeded
		 * @see homographyMatrix(), affineMatrix(), similarityMatrix().
		 */
		static bool homotheticMatrix(const ImagePoint* leftPoints, const ImagePoint* rightPoints, const size_t correspondences, SquareMatrix3& right_H_left);

		/**
		 * Factorizes a homography which contains only a rotational part into the corresponding rotation (of the right camera).
		 * The given homography transforms image points defined in the left image to image points defined in the right image (rightPoint = H * leftPoint).<br>
		 * Beware: Depending on the accuracy of the given homography (e.g., whether it might contain a tiny translational part) the resulting rotation matrix needs to be adjusted further.<br>
		 * @param homography the homography which will be factorized
		 * @param leftCamera Profile of the left camera object
		 * @param rightCamera Profile of the right camera object
		 * @return The rotation matrix which transforms points defined in the right camera coordinate system into points defined in the left camera coordinate system
		 */
		static SquareMatrix3 factorizeHomographyMatrix(const SquareMatrix3& homography, const PinholeCamera& leftCamera, const PinholeCamera& rightCamera);

		/**
		 * Factorizes a planar homography into translation and rotation of the camera.
		 * The resulting factorization provides the camera pose for the right camera while the left camera has the identity camera pose.<br>
		 * Further, a plane normal is provided which is defined in relation to the left camera coordinate system.<br>
		 * Two individual camera poses and normals are provided as the factorization cannot be determined uniquely.
		 * @param right_H_left The homography transforming left points to right points, must be valid
		 * @param leftCamera The camera profile defining the projection of the left camera, must be valid
		 * @param rightCamera The camera profile defining the projection of the right camera, must be valid
		 * @param leftImagePoints The image points in the left camera frame (projected 3D plane object points) that have been used to determine the homography
		 * @param rightImagePoints The image points in the right camera frame, each point corresponds to one point in the left camera frame
		 * @param correspondences The number of image point correspondences, with range [2, infinity)
		 * @param world_T_rightCameras Two resulting camera poses for the right cameras, with default camera pointing towards the negative z-space with y-axis upwards
		 * @param normals Two resulting plane normals (one for each transformation), the normals are defined in relation to the left camera coordinate system
		 * @return True, if succeeded
		 */
		static bool factorizeHomographyMatrix(const SquareMatrix3& right_H_left, const PinholeCamera& leftCamera, const PinholeCamera& rightCamera, const ImagePoint* leftImagePoints, const ImagePoint* rightImagePoints, const size_t correspondences, HomogenousMatrix4 world_T_rightCameras[2], Vector3 normals[2]);

		/**
		 * Factorizes a planar homography into translation and rotation of the camera.
		 * The resulting factorization provides the camera pose for the right camera.<br>
		 * Further, a plane normal is provided which is defined in relation to the left camera coordinate system.<br>
		 * Two individual camera poses and normals are provided as the factorization cannot be determined uniquely.
		 * @param right_H_left The homography transforming left points to right points, must be valid
		 * @param world_T_leftCamera The camera pose of the left camera, with default camera pointing towards the negative z-space with y-axis upwards, must be valid
		 * @param leftCamera The camera profile defining the projection of the left camera, must be valid
		 * @param rightCamera The camera profile defining the projection of the right camera, must be valid
		 * @param leftImagePoints Image points in the left camera frame (projected 3D plane object points) that have been used to determine the homography
		 * @param rightImagePoints Image points in the right camera frame, each point corresponds to one point in the left camera frame
		 * @param correspondences The number of image point correspondences, with range [2, infinity)
		 * @param world_T_rightCameras Two resulting camera poses for the right cameras, with default camera pointing towards the negative z-space with y-axis upwards
		 * @param normals Two resulting plane normals (one for each transformation), the normals are defined in relation to the world coordinate system
		 * @return True, if succeeded
		 */
		static bool factorizeHomographyMatrix(const SquareMatrix3& right_H_left, const HomogenousMatrix4& world_T_leftCamera, const PinholeCamera& leftCamera, const PinholeCamera& rightCamera, const ImagePoint* leftImagePoints, const ImagePoint* rightImagePoints, const size_t correspondences, HomogenousMatrix4 world_T_rightCameras[2], Vector3 normals[2]);

		/**
		 * Calculates the homography for given 3D object points lying on the Z == 0 plane and corresponding 2D image points.
		 * The resulting homography transforms the object points to the image points (imagePoint = H * objectPoint).<br>
		 * Or more precisely: (imagePointX, imagePointY, 1) = H * (objectPointX, objectPointY, 1)
		 * @param objectPoints 3D object points (with z-axis value equal zero), must be valid
		 * @param imagePoints 2D image points each corresponding to an object point, must be valid
		 * @param correspondences The number of given point correspondences, with range [10, infinity)
		 * @param homography The resulting homography
		 * @return True, if succeeded
		 */
		static bool homographyMatrixPlaneXY(const ObjectPoint* objectPoints, const ImagePoint* imagePoints, const size_t correspondences, SquareMatrix3& homography);

		/**
		 * Calculates the homography for given 3D object points lying on the Z == 0 plane and 2D image points.
		 * @param objectPoints 2D object points with z-axis value equal zero, must be valid
		 * @param imagePoints 2D image points each corresponding the to an object point, must be valid
		 * @param correspondences Number of given correspondences, with range [10, infinity)
		 * @param homography The resulting homography
		 * @return True, if succeeded
		 */
		static bool homographyMatrixPlaneXY(const ImagePoint* objectPoints, const ImagePoint* imagePoints, const size_t correspondences, SquareMatrix3& homography);

		/**
		 * Normalizes a given homography forcing a 1 in the lower right matrix corner.
		 * @param homography The homography to normalize, must be valid
		 * @return Normalized homography
		 * @tparam T The data type of the scalar to be used, either 'float' or 'double'
		 */
		template <typename T>
		static inline SquareMatrixT3<T> normalizedHomography(const SquareMatrixT3<T>& homography);

		/**
		 * Normalizes a given homography forcing a 1 in the lower right matrix corner.
		 * @param homography The homography to be normalized, must be valid
		 * @tparam T The data type of the scalar to be used, either 'float' or 'double'
		 */
		template <typename T>
		static inline void normalizeHomography(SquareMatrixT3<T>& homography);

		/**
		 * Scales a given homography so that it fits to an individually scaled input image and an individually scaled output image.
		 * @param homography The homography converting points defined in the left image to points defined in the right image (rightPoint = H * leftPoint)
		 * @param scaleLeft The scale which will be applied to the left image, e.g., a scale of 2 means that the image is twice as large, with range (0, infinity)
		 * @param scaleRight The scale which will be applied to the right image, e.g., a scale of 2 means that the image is twice as large, with range (0, infinity)
		 * @tparam T the element type of the homography matrix, e.g., 'float' or 'double'
		 */
		template <typename T>
		static inline SquareMatrixT3<T> scaleHomography(const SquareMatrixT3<T>& homography, const T& scaleLeft, const T& scaleRight);

		/**
		 * Returns whether a given homography represents a plausible transformation.
		 * A homography is voted as plausible as long as the transformed frame corners form a convex shape.<br>
		 * The forward transformation as well as the backward transformation (the inverse) is tested.<br>
		 * The homography transforms image points defined in the left image to image points defined in the right image (rightPoint = H * leftPoint).
		 * @param leftImageWidth The width of the left image in pixel, with range [1, infinity)
		 * @param leftImageHeight The height of the left image in pixel, with range [1, infinity)
		 * @param rightImageWidth The width of the left image in pixel, with range [1, infinity)
		 * @param rightImageHeight The height of the left image in pixel, with range [1, infinity)
		 * @param homography The homography transforming points defined in the left image to points defined in the right image, must be valid
		 * @return True, if the homography is plausible
		 */
		static bool isHomographyPlausible(const unsigned int leftImageWidth, const unsigned int leftImageHeight, const unsigned int rightImageWidth, const unsigned int rightImageHeight, const SquareMatrix3& homography);

		/**
		 * Calculates the intrinsic camera matrix for a set of given homographies transforming 3D object points from the z=0 plane to the image plane.
		 * @param homographies Set of given homographies
		 * @param number The number of given homographies with range [3, infinity)
		 * @param intrinsic Resulting intrinsic camera matrix
		 * @return True, if succeeded
		 */
		static bool intrinsicMatrix(const SquareMatrix3* homographies, const size_t number, SquareMatrix3& intrinsic);

		/**
		 * Calculates the extrinsic camera matrix for a given intrinsic camera matrix and a corresponding homography transforming 3D object points from the z=0 plane to the image plane.
		 * @param intrinsic The intrinsic camera matrix to return the extrinsic camera matrix for
		 * @param homography The homography corresponding to the intrinsic camera matrix
		 * @param world_T_camera The resulting extrinsic camera matrix (the camera pose), with default camera pointing towards the negative z-space with y-axis upwards
		 * @return True, if succeeded
		 */
		static bool extrinsicMatrix(const SquareMatrix3& intrinsic, const SquareMatrix3& homography, HomogenousMatrix4& world_T_camera);

		/**
		 * Calculates the first two radial distortion parameter.
		 * The distortion parameters are determined for several different extrinsic camera positions.<br>
		 * The more point correspondences for individual extrinsic camera positions are provided the more accurate the result.
		 * @param extrinsics Set of extrinsic camera parameters
		 * @param intrinsic The intrinsic camera matrix used for all point correspondences
		 * @param objectPointGroups The groups of object points, each group corresponds to a group of image points and to one extrinsic camera
		 * @param imagePointGroups The groups of image points, each group corresponds to a set of object points
		 * @param distortion2 Resulting first radial distortion parameter for the square distance (r^2)
		 * @param distortion4 Resulting second radial distortion parameter for the square of the square distance (r^4)
		 * @return True, if succeeded
		 */
		static bool distortionParameters(const ConstIndexedAccessor<HomogenousMatrix4>& extrinsics, const SquareMatrix3& intrinsic, const ConstIndexedAccessor<Vectors3>& objectPointGroups, const ConstIndexedAccessor<Vectors2>& imagePointGroups, Scalar& distortion2, Scalar& distortion4);

		/**
		 * Converts a given homography determined for a coarser pyramid layer to a homography matching with the finest pyramid layer.
		 * The new homography is determined via:
		 * <pre>
		 * finestHomography = upsample * coarseHomography * downsample
		 * </pre>
		 * @param coarseHomography The homography determined for the coarser pyramid layer, must be valid
		 * @param sourceLayer The index of the coarser pyramid layer, with range [0, infinity)
		 * @return The homography matching with the finest pyramid layer, 'homography' if 'sourceLayer == 0'
		 */
		static SquareMatrix3 toFinestHomography(const SquareMatrix3& coarseHomography, const unsigned int sourceLayer);

		/**
		 * Converts a given homography determined for the finest pyramid layer to a homography matching with a coarser pyramid layer.
		 * The new homography is determined via:
		 * <pre>
		 * coarseHomography = downsample * finestHomography * upsample
		 * </pre>
		 * @param finestHomography The homography determined for the finest pyramid layer, must be valid
		 * @param targetLayer The index of the coarser pyramid layer, with range [0, infinity)
		 * @return The homography matching with the coarser pyramid layer, 'homography' if 'targetLayer == 0'
		 */
		static SquareMatrix3 toCoarseHomography(const SquareMatrix3& finestHomography, const unsigned int targetLayer);

		/**
		 * Returns the homography transforming lines from one image to another image based on a homography transforming points from one image to another image.
		 * This function is mainly returning:
		 * <pre>
		 * (homographyForPoints^T)^-1
		 * </pre>
		 * @param homographyForPoints The homography transforming points from one image to another image, must be valid
		 * @return The homography allowing to transform lines
		 * @tparam T The data type of the matrix's scalar type, either 'float' or 'double'
		 */
		template <typename T>
		static inline SquareMatrixT3<T> homographyForLines(const SquareMatrixT3<T>& homographyForPoints);
};

inline bool Homography::homographyMatrix(const Vector2* leftPoints, const Vector2* rightPoints, const size_t correspondences, SquareMatrix3& right_H_left, const bool useSVD)
{
	ocean_assert(leftPoints && rightPoints && correspondences >= 4);

	if (useSVD)
	{
		return homographyMatrixSVD(leftPoints, rightPoints, correspondences, right_H_left);
	}
	else
	{
		return homographyMatrixLinearWithOptimizations(leftPoints, rightPoints, correspondences, right_H_left);
	}
}

inline bool Homography::homographyMatrixLinearWithOptimizations(const Vector2* leftPoints, const Vector2* rightPoints, const size_t correspondences, SquareMatrix3& right_H_left)
{
	ocean_assert(leftPoints && rightPoints && correspondences >= 4);

	return homographyMatrixLinear(leftPoints, rightPoints, correspondences, right_H_left, 10u);
}

template <typename T>
inline SquareMatrixT3<T> Homography::normalizedHomography(const SquareMatrixT3<T>& homography)
{
	return Normalization::normalizedTransformation(homography);
}

template <typename T>
inline void Homography::normalizeHomography(SquareMatrixT3<T>& homography)
{
	Normalization::normalizeTransformation(homography);
}

template <typename T>
inline SquareMatrixT3<T> Homography::scaleHomography(const SquareMatrixT3<T>& homography, const T& scaleLeft, const T& scaleRight)
{
	ocean_assert(!homography.isSingular());
	ocean_assert(scaleLeft > NumericT<T>::eps() && scaleRight > NumericT<T>::eps());

	// RTr * rHl * lTL

	const T invScaleLeft = T(1) / scaleLeft;

	SquareMatrix3 scaledHomography;

	scaledHomography[0] = homography[0] * scaleRight * invScaleLeft;
	scaledHomography[1] = homography[1] * scaleRight * invScaleLeft;
	scaledHomography[2] = homography[2] * invScaleLeft;

	scaledHomography[3] = homography[3] * scaleRight * invScaleLeft;
	scaledHomography[4] = homography[4] * scaleRight * invScaleLeft;
	scaledHomography[5] = homography[5] * invScaleLeft;

	scaledHomography[6] = homography[6] * scaleRight;
	scaledHomography[7] = homography[7] * scaleRight;
	scaledHomography[8] = homography[8];

#ifdef OCEAN_DEBUG
	const SquareMatrix3 rightTransformation(VectorT3<T>(scaleRight, 0, 0), VectorT3<T>(0, scaleRight, 0), VectorT3<T>(0, 0, 1)); // RTr
	const SquareMatrix3 leftTransformation(VectorT3<T>(scaleLeft, 0, 0), VectorT3<T>(0, scaleLeft, 0), VectorT3<T>(0, 0, 1)); // lTL
	ocean_assert(scaledHomography.isEqual(rightTransformation * homography * leftTransformation.inverted(), NumericT<T>::weakEps()));
#endif

	return scaledHomography;
}

template <typename T>
inline SquareMatrixT3<T> Homography::homographyForLines(const SquareMatrixT3<T>& homographyForPoints)
{
	ocean_assert(homographyForPoints.isHomography());

	return homographyForPoints.transposed().inverted();
}

}

}

#endif // META_OCEAN_GEOMETRY_HOMOGRAPHY_H
