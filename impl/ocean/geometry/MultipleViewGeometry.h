/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_GEOMETRY_MULTIPLE_VIEW_GEOMETRY_H
#define META_OCEAN_GEOMETRY_MULTIPLE_VIEW_GEOMETRY_H

#include "ocean/geometry/Geometry.h"
#include "ocean/geometry/NonLinearOptimization.h"

#include "ocean/math/Matrix.h"
#include "ocean/math/Line2.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/Vector3.h"

#include <vector>

namespace Ocean
{

namespace Geometry
{

/**
 * This class implements epipolar geometry functions for multiple views.
 * @ingroup geometry
 */
class OCEAN_GEOMETRY_EXPORT MultipleViewGeometry
{
	public:

		/**
		 * Definition of a trifocal tensor matrix.
		 */
		class TrifocalTensor
		{
			public:

				/**
				 * Creates a new tensor object without initialization.
				 */
				inline TrifocalTensor();

				/**
				 * Creates a new tensor object by three given sub-matrices.
				 * @param matrix0 The first matrix of the new tensor
				 * @param matrix1 The second matrix of the new tensor
				 * @param matrix2 The third matrix of the new tensor
				 */
				inline TrifocalTensor(const SquareMatrix3& matrix0, const SquareMatrix3& matrix1, const SquareMatrix3& matrix2);

				/**
				 * Creates a new tensor object by three given sub-matrices.
				 * @param matrices The three matrix of the new tensor, must not be nullptr
				 */
				explicit inline TrifocalTensor(const SquareMatrix3* matrices);

				/**
				 * Element operator.
				 * Beware: No range check will be done!
				 * @param index Index of the matrix to return [0, 2]
				 * @return Specified sub-matrix
				 */
				inline const SquareMatrix3& operator[](const unsigned int index) const;

				/**
				 * Element operator.
				 * Beware: No range check will be done!
				 * @param index Index of the matrix to return [0, 2]
				 * @return Specified sub-matrix
				 */
				inline SquareMatrix3& operator[](const unsigned int index);

				/**
				 * Data operator returning the pointer to the first matrix of this tensor.
				 * @return The pointer of the first matrix
				 */
				inline const SquareMatrix3* operator()() const;

				/**
				 * Data operator returning the pointer to the first matrix of this tensor.
				 * @return The pointer of the first matrix
				 */
				inline SquareMatrix3* operator()();

			protected:

				/// The three 3x3 matrices of this tensor.
				SquareMatrix3 tensorMatrices[3];
		};

	public:

		/**
		 * Computes geometric valid camera projection matrices which are determined up to a common 3d projection transformation. <br>
		 * The calculation uses 6 point correspondences in which three of them are not collinear in any view
		 * The projection matrices are defined in inverted flipped coordinates system and are necessary metric and orthogonally:
		 * x_k = P_k * X.
		 * @param points1 Image points of first view
		 * @param points2 Image points of second view
		 * @param points3 Image points of third view
		 * @param correspondences Number of point correspondences (at least 6)
		 * @param iFlippedProjectionMatrix1 Resulting camera projection matrix of first view (inverted flipped)
		 * @param iFlippedProjectionMatrix2 Resulting camera projection matrix of second view (inverted flipped)
		 * @param iFlippedProjectionMatrix3 Resulting camera projection matrix of third view (inverted flipped)
		 * @param squaredSuccessThreshold Threshold indicating successful matrices determination, optionally
		 * @param squaredProjectionError Squared back-projection error, optionally
		 * @return True, if succeeded and back-projection error is below threshold (note: a valid 3d scene is also maybe given with a higher error)
		 * @see projectiveReconstructionFrom6PointsIF()
		 * @see calibrateFromProjectionsMatricesIF()
		 */
		static bool projectiveReconstructionFrom6PointsIF(const ImagePoint* points1, const ImagePoint* points2, const ImagePoint* points3, const size_t correspondences, HomogenousMatrix4& iFlippedProjectionMatrix1, HomogenousMatrix4& iFlippedProjectionMatrix2, HomogenousMatrix4& iFlippedProjectionMatrix3, const Scalar squaredSuccessThreshold = (2.5 * 2.5), Scalar* squaredProjectionError = nullptr);

		/**
		 * Computes geometric valid camera projection matrices which are determined up to a common 3d projection transformation.
		 * The calculation uses 6 point correspondences in which three of them are not collinear in any view
		 * The projection matrices are defined in inverted flipped coordinates system and are not metric and orthogonally:
		 * x_k = P_k * X.
		 * @param imagePointsPerPose Image points per view (at least 3 views and at least 6 point correspondences)
		 * @param posesIF Resulting camera projection matrices per view (inverted flipped)
		 * @param squaredSuccessThreshold Threshold indicating successful matrices determination, optionally
		 * @param squaredProjectionError Squared back-projection error, optionally
		 * @return True, if succeeded and back-projection error is below threshold (note: a valid 3d scene is also maybe given with a higher error)
		 * @see projectiveReconstructionFrom6PointsIF()
		 * @see calibrateFromProjectionsMatricesIF()
		 */
		static bool projectiveReconstructionFrom6PointsIF(const ConstIndexedAccessor<ImagePoints>& imagePointsPerPose, NonconstIndexedAccessor<HomogenousMatrix4>* posesIF, const Scalar squaredSuccessThreshold = Scalar(2.5 * 2.5), Scalar* squaredProjectionError = nullptr);

		/**
		 * The normalized linear algorithm of computation of trifocal tensor.
		 * Calculates the trifocal tensor by three sets of at least seven corresponding image points and generates geometric valid camera projection matrices.
		 * The projection matrices are defined in inverted flipped coordinates system and are not metric and orthogonally:
		 * x_k = P_k * X.
		 * @param points1 Image points of first view
		 * @param points2 Image points of second view
		 * @param points3 Image points of third view
		 * @param correspondences Number of point correspondences (at least 7)
		 * @param iFlippedProjectionMatrix1 Resulting camera projection matrix of first view (inverted flipped)
		 * @param iFlippedProjectionMatrix2 Resulting camera projection matrix of second view (inverted flipped)
		 * @param iFlippedProjectionMatrix3 Resulting camera projection matrix of third view (inverted flipped)
		 * @param trifocalTensor Optional pointer to resulting trifocal tensor
		 * @return True, if succeeded
		 * @see calibrateFromProjectionsMatricesIF()
		 */
		static bool trifocalTensorIF(const ImagePoint* points1, const ImagePoint* points2, const ImagePoint* points3, const size_t correspondences, HomogenousMatrix4& iFlippedProjectionMatrix1, HomogenousMatrix4& iFlippedProjectionMatrix2, HomogenousMatrix4& iFlippedProjectionMatrix3, TrifocalTensor* trifocalTensor = nullptr);

		/**
		 * Calibrate multiple projection matrices from a single camera.
		 * The projection matrices are transformed into metric transformation matrix using absolute conic &omega; and absolute dual quadric Q&infin;: <br>
		 * &omega; = P * Q&infin; * P^T.<br>
		 * Q&infin; = H * Î * H^T.<br>
		 * P_metric = P * H.<br>
		 * The camera intrinsics are determined by Cholesky decomposition of absolute conic &omega; = K * K^T.
		 * @param iFlippedProjectionMatrices The camera projection matrices (at least 3, inverted flipped)
		 * @param imageWidth Width of image
		 * @param imageHeight Height of image
		 * @param cameraIntrinsic The resulting intrinsic camera matrix
		 * @param iFlippedPoses Resulting inverted and flipped camera poses, one for each given camera projection matrix
		 * @return True, if succeeded
		 * @see calibrateFromProjectionsMatricesIF()
		 */
		static bool calibrateFromProjectionsMatricesIF(const ConstIndexedAccessor<HomogenousMatrix4>& iFlippedProjectionMatrices, const unsigned int imageWidth, const unsigned int imageHeight, SquareMatrix3& cameraIntrinsic, HomogenousMatrix4* iFlippedPoses);

		/**
		 * Calibrate three projection matrices from a single camera.
		 * The projection matrices are transformed into metric transformation matrix using absolute conic &omega; and absolute dual quadric Q&infin;: <br>
		 * &omega; = P * Q&infin; * P^T.<br>
		 * Q&infin; = H * Î * H^T.<br>
		 * P_metric = P * H.<br>
		 * The camera intrinsics are determined by Cholesky decomposition of absolute conic &omega; = KK^T.
		 * @param iFlippedProjectionMatrix1 First camera projection matrices (inverted flipped)
		 * @param iFlippedProjectionMatrix2 Second camera projection matrices (inverted flipped)
		 * @param iFlippedProjectionMatrix3 Third camera projection matrices (inverted flipped)
		 * @param imageWidth Width of image
		 * @param imageHeight Height of image
		 * @param cameraIntrinsics Resulting camera intrinsics matrix
		 * @param iFlippedPose1 Resulting first camera pose (inverted flipped)
		 * @param iFlippedPose2 Resulting second camera pose (inverted flipped)
		 * @param iFlippedPose3 Resulting third camera pose (inverted flipped)
		 * @return True, if succeeded
		 * @see calibrateFromProjectionsMatricesIF()
		 */
		static bool calibrateFromProjectionsMatricesIF(const HomogenousMatrix4& iFlippedProjectionMatrix1, const HomogenousMatrix4& iFlippedProjectionMatrix2, const HomogenousMatrix4& iFlippedProjectionMatrix3, const unsigned int imageWidth, const unsigned int imageHeight, SquareMatrix3& cameraIntrinsics, HomogenousMatrix4& iFlippedPose1, HomogenousMatrix4& iFlippedPose2, HomogenousMatrix4& iFlippedPose3);

	protected:

		/**
		 * The normalized linear algorithm of computation of trifocal tensor.
		 * Calculates the trifocal tensor by three sets of at least seven corresponding image points.
		 * @param points1 Image points of first set
		 * @param points2 Image points of second set
		 * @param points3 Image points of third set
		 * @param correspondences Number of point correspondences (at least 7)
		 * @param trifocal Resulting trifocal tensor matrices
		 * @return True, if succeeded
		 */
		static bool trifocalTensorNormalizedLinear(const ImagePoint* points1, const ImagePoint* points2, const ImagePoint* points3, const size_t correspondences, TrifocalTensor& trifocal);

		/**
		 * Computing a geometric valid trifocal tensor minimizing algebraic error.
		 * Calculates the trifocal tensor by three sets of at least seven corresponding image points.
		 * @param points1 Image points of first set
		 * @param points2 Image points of second set
		 * @param points3 Image points of third set
		 * @param correspondences Number of point correspondences (at least 7)
		 * @param trifocal Resulting trifocal tensor matrices
		 * @return True, if succeeded
		 */
		static bool trifocalTensorMinimizingError(const ImagePoint* points1, const ImagePoint* points2, const ImagePoint* points3, const size_t correspondences, TrifocalTensor& trifocal);

		/**
		 * Calculates the trifocal tensor by two projection matrix given:
		 * P_1 = [I | 0], P_2 = [A | a4], P_3 = [B | b4] and P = C * [Rt]
		 * @param iFlippedProjectionMatrixB projection matrix P_2 in homogeneous format
		 * @param iFlippedProjectionMatrixC projection matrix P_3 in homogeneous format
		 * @param trifocal Resulting trifocal tensor matrices
		 * @return True, if succeeded
		 */
		static bool trifocalTensorFromProjectionMatrices(const HomogenousMatrix4& iFlippedProjectionMatrixB, const HomogenousMatrix4& iFlippedProjectionMatrixC, TrifocalTensor& trifocal);

		/**
		 * Calculates the trifocal tensor by three projection matrix given:
		 * P_1 = [A | a4], P_2 = [B | b4], P_3 = [C | c4] and P = CameraInstrinsics * [Rt]
		 * @param iFlippedProjectionMatrixA projection matrix P_1 in homogeneous format
		 * @param iFlippedProjectionMatrixB projection matrix P_2 in homogeneous format
		 * @param iFlippedProjectionMatrixC projection matrix P_3 in homogeneous format
		 * @param trifocal Resulting trifocal tensor matrices
		 * @return True, if succeeded
		 */
		static bool trifocalTensorFromProjectionMatrices(const HomogenousMatrix4& iFlippedProjectionMatrixA, const HomogenousMatrix4& iFlippedProjectionMatrixB, const HomogenousMatrix4& iFlippedProjectionMatrixC, TrifocalTensor& trifocal);

		/**
		 * Calculates the normalized epipoles of second and third view from trifocal tensor
		 * @param trifocal Trifocal tensor matrices
		 * @param normedEpipole2 Resulting (normalized) epipole in second view
		 * @param normedEpipole3 Resulting (normalized) epipole in third view
		 * @return True, if succeeded
		 */
		static bool epipoles(const TrifocalTensor& trifocal, Vector3& normedEpipole2, Vector3& normedEpipole3);

		/**
		 * Calculates the normalized epipoles of second and third view from trifocal tensor for inverted flipped cameras
		 * @param trifocal Trifocal tensor matrices
		 * @param iFlippedNormedEpipole2 Resulting (normalized) epipole in second view
		 * @param iFlippedNormedEpipole3 Resulting (normalized) epipole in third view
		 * @return True, if succeeded
		 */
		static bool epipolesIF(const TrifocalTensor& trifocal, Vector3& iFlippedNormedEpipole2, Vector3& iFlippedNormedEpipole3);

		/**
		 * Calculates the fundamental matrices of second view and third view from trifocal tensor
		 * @param trifocal Trifocal tensor matrices
		 * @param iFlippedEpipole2 Epipole in second view
		 * @param iFlippedEpipole3 Epipole in third view
		 * @param fundamental21 Resulting fundamental matrix between second view and first view
		 * @param fundamental31 Resulting fundamental matrix between third view and first view
		 * @return True, if succeeded
		 */
		static bool fundamentalMatricesIF(const TrifocalTensor& trifocal, const Vector3& iFlippedEpipole2, const Vector3& iFlippedEpipole3, SquareMatrix3& fundamental21, SquareMatrix3& fundamental31);

		/**
		 * Calculates the camera projection matrices of second and third view.
		 * The matrices are determined only up to a common projective transformation of 3-space.
		 * The projection matrix of first camera must be P_0 = [I | 0]
		 * @param trifocal Trifocal tensor matrices
		 * @param iFlippedNormedEpipole2 Normalized epipole in second view
		 * @param iFlippedNormedEpipole3 Normalized epipole in third view
		 * @param iFlippedProjectionMatrix2 Resulting camera projection matrices of second view (determined only up to a common projective 3d transformation)
		 * @param iFlippedProjectionMatrix3 Resulting camera projection matrices of third view (determined only up to a common projective 3d transformation)
		 * @return True, if succeeded
		 */
		static bool cameraProjectionMatricesIF(const TrifocalTensor& trifocal, const Vector3& iFlippedNormedEpipole2, const Vector3& iFlippedNormedEpipole3, HomogenousMatrix4& iFlippedProjectionMatrix2, HomogenousMatrix4& iFlippedProjectionMatrix3);

		/**
		 * Calculates the normalized epipoles of second and third view from trifocal tensor
		 * @param trifocal1 First trifocal tensor matrix T_1
		 * @param trifocal2 Second trifocal tensor matrix T_2
		 * @param trifocal3 Third trifocal tensor matrix T_3
		 * @param normedEpipole2 Resulting (normalized) epipole in second view
		 * @param normedEpipole3 Resulting (normalized) epipole in third view
		 * @tparam tUseIF Calculation for inverted flipped camera system, if set
		 * @return True, if succeeded
		 */
		template<bool tUseIF>
		static bool epipoles(const Scalar* const trifocal1, const Scalar* const trifocal2, const Scalar* const trifocal3, Vector3& normedEpipole2, Vector3& normedEpipole3);

	private:

		/**
		 * Calculates the trifocal tensor using linear system.
		 * @param points1 Image points of first set
		 * @param points2 Image points of second set
		 * @param points3 Image points of third set
		 * @param correspondences Number of point correspondences (at least 7)
		 * @param trifocal3x9 Pointer to resulting values of trifocal tensor matrices
		 * @param matrixA Pointer to constructed linear system, if needed
		 * @return True, if succeeded
		 */
		static bool trifocalTensorLinear(const ImagePoint* points1, const ImagePoint* points2, const ImagePoint* points3, const size_t correspondences, Scalar* trifocal3x9, Matrix* matrixA = nullptr);

		/**
		 * The normalized linear algorithm of computation of trifocal tensor.
		 * Calculates the trifocal tensor by three sets of at least seven corresponding image points.
		 * @param points1 Image points of first set
		 * @param points2 Image points of second set
		 * @param points3 Image points of third set
		 * @param correspondences Number of point correspondences (at least 7)
		 * @param trifocal3x9 Pointer to resulting values of trifocal tensor matrices
		 * @param matrixA Pointer to constructed linear system, if needed
		 * @return True, if succeeded
		 */
		static bool trifocalTensorNormalizedLinear(const ImagePoint* points1, const ImagePoint* points2, const ImagePoint* points3, const size_t correspondences, Scalar* trifocal3x9, Matrix* matrixA = nullptr);

		/**
		 * Calculates the trifocal tensor error in point–point–point correspondence equation:
		 * [x_2]_x * (sum_i(x_1^i * T_i) * [x_3]_x).
		 * @param trifocal Trifocal tensor matrices
		 * @param points1 Image points of first set
		 * @param points2 Image points of second set
		 * @param points3 Image points of third set
		 * @param correspondences Number of point correspondences (at least 7)
		 * @param errorMatrix Resulting 3x3 error matrix is needed
		 * @return The absolute sum of 3x3 error matrix
		 */
		static Scalar errorMatrix(const TrifocalTensor& trifocal, const ImagePoint* points1, const ImagePoint* points2, const ImagePoint* points3, const size_t correspondences, SquareMatrix3* errorMatrix = nullptr);

		/**
		 * Calculates a transformation for a projective basis defined by four individual (image) target points each corresponding to a specific (projective/3D) reference points.<br>
		 * The following projective reference points are used: e_i = (1, 0, 0),  (0, 1, 0),  (0, 0, 1),  (1, 1, 1).<br>
		 * The resulting transformation transforms the provided image points to projective references points:<br>
		 * si * ei = H * pi, each up to a scale factor si.
		 * @param imagePointForTargetPoint100 Image point p0 corresponding to s0 * (1, 0, 0)
		 * @param imagePointForTargetPoint010 Image point p1 corresponding to s1 * (0, 1, 0)
		 * @param imagePointForTargetPoint001 Image point p2 corresponding to s2 * (0, 0, 1)
		 * @param imagePointForTargetPoint111 Image point p3 corresponding to s3 * (1, 1, 1)
		 * @param baseTransformation Resulting transformation
		 * @return True, if succeeded
		 */
		static bool calculateProjectiveBasisTransform(const Vector2& imagePointForTargetPoint100, const Vector2& imagePointForTargetPoint010, const Vector2& imagePointForTargetPoint001, const Vector2& imagePointForTargetPoint111, SquareMatrix3& baseTransformation);

		/**
		 * Checks if a given 2D point is collinear to three given 2D points (defined by a 2D triangle).
		 * @param triangle The triangle defining the three points
		 * @param point The point to be check
		 * @param threshold Threshold of variance from linearity condition, with range (0, 1]
		 * @return True, if the given point is collinear to any of two corners of the triangle
		 */
		static inline bool pointIsCollinear(const Triangle2& triangle, const Vector2& point, const Scalar threshold = Scalar(0.01));
};

inline MultipleViewGeometry::TrifocalTensor::TrifocalTensor()
{
	// nothing to do here
}

inline MultipleViewGeometry::TrifocalTensor::TrifocalTensor(const SquareMatrix3& matrix0, const SquareMatrix3& matrix1, const SquareMatrix3& matrix2)
{
	tensorMatrices[0] = matrix0;
	tensorMatrices[1] = matrix1;
	tensorMatrices[2] = matrix2;
}

inline MultipleViewGeometry::TrifocalTensor::TrifocalTensor(const SquareMatrix3* matrices)
{
	ocean_assert(matrices != nullptr);

	tensorMatrices[0] = matrices[0];
	tensorMatrices[1] = matrices[1];
	tensorMatrices[2] = matrices[2];
}

inline const SquareMatrix3& MultipleViewGeometry::TrifocalTensor::operator[](const unsigned int index) const
{
	ocean_assert(index < 3u);
	return tensorMatrices[index];
}

inline SquareMatrix3& MultipleViewGeometry::TrifocalTensor::operator[](const unsigned int index)
{
	ocean_assert(index < 3u);
	return tensorMatrices[index];
}

inline const SquareMatrix3* MultipleViewGeometry::TrifocalTensor::operator()() const
{
	return tensorMatrices;
}

inline SquareMatrix3* MultipleViewGeometry::TrifocalTensor::operator()()
{
	return tensorMatrices;
}

/**
 * This class implements self-calibration for multiple views.
 * @ingroup geometry
 */
class OCEAN_GEOMETRY_EXPORT AutoCalibration
{
	public:

		/**
		 * Estimate the common intrinsic camera matrix based on an Absolute Conic omega (&omega;):<br>
		 * omega = K * K^T   ---   (&omega;_j = &omega; = KK^T).<br>
		 * Solve a linear system by assuming equal entities in each &omega;
		 * @param iFlippedProjectionMatrices The inverted and flipped camera projection matrices [K * P0], [K * P1], [K * P2], ..., with Pi = [Ri | ti], at least 3
		 * @param cameraIntrinsics Resulting intrinsic camera matrix
		 * @param Q Optional resulting absolute dual quadric Q&infin;
		 * @param omega Optional resulting absolute conic &omega
		 * @return True, if succeeded
		 * @see findAbsoluteDualQuadricLinear()
		 */
		static bool findCommonIntrinsicsFromProjectionMatricesIF(const ConstIndexedAccessor<HomogenousMatrix4>& iFlippedProjectionMatrices, SquareMatrix3& cameraIntrinsics, SquareMatrix4* Q = nullptr, SquareMatrix3* omega = nullptr);

		/**
		 * Estimate a common intrinsic camera matrix based on an Absolute Conic &omega;:<br>
		 * &omega;_j = &omega; = KK^T.<br>
		 * Solve a linear system by assuming zero skew, a known principle point (centered in the camera frame) and equal focal length for each &omega;
		 * @param iFlippedProjectionMatrices Inverted and flipped camera projection matrices [K | P0], [K | P1], [K | P2], ..., with Pi = [Ri | ti], at least 3
		 * @param imageWidth Width of the camera frame in pixel, with range [1, infinity)
		 * @param imageHeight Height of the camera frame in pixel, with range [1, infinity)
		 * @param cameraIntrinsics Resulting camera intrinsics matrix
		 * @param Q Resulting absolute dual quadric Q&infin;, optionally
		 * @param omega Resulting absolute conic &omega, optionally
		 * @return True, if succeeded
		 * @see findAbsoluteDualQuadricLinear()
		 */
		static bool findCommonIntrinsicsFromProjectionMatricesIF(const ConstIndexedAccessor<HomogenousMatrix4>& iFlippedProjectionMatrices, const unsigned int imageWidth, const unsigned int imageHeight, SquareMatrix3& cameraIntrinsics, SquareMatrix4* Q = nullptr, SquareMatrix3* omega = nullptr);

		/**
		 * Estimates the absolute dual quadric Q_infinity (Q&infin;) from several projection-pose matrices by solving a linear system:<br>
		 * omega = P_j * Q_infinity * P_j^T   ---   (&omega;_j = P_j * Q&infin; * P_j^T).<br>
		 * Here, we assume no skew and the principle point in the center of the camera frame.
		 * @param iFlippedProjectionMatrices The inverted and flipped camera projection matrices [K | P0], [K | P1], [K | P2], ..., with Pi = [Ri | ti], at least 3
		 * @param matrixQ Resulting absolute dual quadric Q_infinity   ---   (Q&infin;)
		 * @param imageWidth Width of the camera frame in pixel, with range [1, infinity)
		 * @param imageHeight Height of the camera frame in pixel, with range [1, infinity)
		 * @param equalFxFy Forces equal focal length in both dimension
		 * @return True, if succeeded
		 * @see findCommonIntrinsicsFromProjectionMatricesIF()
		 */
		static bool determineAbsoluteDualQuadricLinearIF(const ConstIndexedAccessor<HomogenousMatrix4>& iFlippedProjectionMatrices, SquareMatrix4& matrixQ, const unsigned int imageWidth, const unsigned int imageHeight, bool equalFxFy = true);

		/**
		 * Determines individual intrinsic camera matrices from a known absolute dual quadric Q_infinity (Q&infin;) and corresponding camera-projection-pose matrices:<br>
		 * omega_j = P_j * Q_infinity * P_J^T = K_j * K_j^T   ---   (&omega;_j = P_j * Q&infin; * P_J^T = K_j * K_j^T).<br>
		 * The Cholesky decomposition is applied to factorize omega into K.
		 * @param symmetricQ The absolute dual quadric q_infinity which is symmetric
		 * @param iFlippedProjectionMatrices The inverted and flipped camera projection pose matrices
		 * @param intrinsics The resulting intrinsic camera matrices, one for each projection pose matrix
		 * @return True, if succeeded
		 * @see findAbsoluteDualQuadricLinear()
		 * @see transformProjectionsZeroPrinciplePoint()
		 */
		static bool intrinsicsFromAbsoluteDualQuadricIF(const SquareMatrix4& symmetricQ, const ConstIndexedAccessor<HomogenousMatrix4>& iFlippedProjectionMatrices, SquareMatrix3* intrinsics);

		/**
		 * Transforms a projective reconstruction towards a metric reconstruction.<br>
		 * Q&infin; = H * Î * H^T.<br>
		 * P_metric = P * H.<br>
		 * @param Q Absolute dual quadric Q&infin;
		 * @param iFlippedProjectionMatrices The camera projection matrices (inverted flipped)
		 * @param iFlippedMetricProjectionMatrices Resulting metric projection matrices (inverted flipped)
		 * @param transformation Optional resulting perspective transformation matrix
		 * @return True, if succeeded
		 * @see getTransformProjectiveToMetricMatrix()
		 */
		static bool transformProjectiveToMetricIF(const SquareMatrix4& Q, const ConstIndexedAccessor<HomogenousMatrix4>& iFlippedProjectionMatrices, HomogenousMatrix4* iFlippedMetricProjectionMatrices, SquareMatrix4* transformation = nullptr);

		/**
		 * Decomposes metric camera projection matrices all containing/sharing the same known camera matrix (intrinsic camera matrix) into (inverted and flipped) camera poses simply by applying the inverted intrinsic matrix.
		 * Based on the following equation: P_metric = K * [R t] we can determine the camera poses by: [R t] = K^-1 * p_metric.
		 * @param metricProjectionsIF The inverted and flipped metric camera projection matrices
		 * @param intrinsic The already known intrinsic camera matrix K
		 * @param posesIF The resulting inverted and flipped camera poses, one for each given camera projection matrix
		 * @return True, if succeeded
		 * @see transformProjectiveToMetric().
		 */
		static bool metricProjectionMatricesToPosesIF(const ConstIndexedAccessor<HomogenousMatrix4>& metricProjectionsIF, const SquareMatrix3& intrinsic, HomogenousMatrix4* posesIF);

	protected:

		/**
		 * Transforms an inverted and flipped camera projection matrix P = K * [R|t] in a way that the principle point of the camera profile (the camera intrinsics) is zero.<br>
		 * P' = T * P.
		 * @param iFlippedProjectionMatrices Inverted and flipped camera projection matrices, at least one
		 * @param imageWidth Width of image in pixel, with range [1, infinity)
		 * @param imageHeight Height of image in pixel, with range [1, infinity)
		 * @param iFlippedNormalizedProjectionMatrices Resulting normalized inverted and flipped camera projection matrices, one for each given camera projection matrix
		 * @param backTransformation Optional resulting back transformation matrix T^-1
		 * @return True, if succeeded
		 * @see transformProjectiveToMetric()
		 */
		static bool transformProjectionsZeroPrinciplePoint(const ConstIndexedAccessor<HomogenousMatrix4>& iFlippedProjectionMatrices, const unsigned int imageWidth, const unsigned int imageHeight, HomogenousMatrix4* iFlippedNormalizedProjectionMatrices, SquareMatrix3* backTransformation = nullptr);

		/**
		 * Determines the perspective transformation to get a metric reconstruction.<br>
		 * Q&infin; = H * Î * H^T, factorized by eigen decomposition.
		 * @param Q Absolute dual quadric Q&infin;
		 * @param transformation Resulting perspective transformation H
		 * @return True, if succeeded
		 * @see transformProjectiveToMetric()
		 */
		static bool getTransformProjectiveToMetricMatrix(const SquareMatrix4& Q, SquareMatrix4& transformation);

	private:

		/**
		 * Performs the decomposition of an absolute conic omega (&omega;) into the intrinsic camera matrix:<br>
		 * omega = K * K^T   ---   (&omega; = K * K^T), where K is the camera matrix (which is a upper triangular matrix).<br>
		 * The Cholesky decomposition is applied.
		 * @param omega The (symmetric) absolute conic to be decomposed (&omega;)
		 * @param cameraIntrinsic The resulting intrinsic camera matrix
		 * @return True, if succeeded
		 */
		static bool upperTriangleCholeskyDecomposition(const SquareMatrix3& omega, SquareMatrix3& cameraIntrinsic);

		/**
		 * Creates a line for the linear system for the zero condition of absolute conic &omega;(i, j).<br>
		 * The line contains values of the upper triangle entities of absolute dual quadric matrix Q_infinity   ---   (Q&infin;):
		 * &omega;(i, j) = [P * Q&infin; * P^T](i, j) = 0
		 * @param omegaRowIndex Row index of absolute conic &omega;
		 * @param omegaColumnIndex Column index of absolute conic &omega;
		 * @param iFlippedProjectionMatrix The inverted and flipped camera projection matrix, must be valid
		 * @return A line matrix [1x10] containing the values of the upper triangle entities of the symmetric matrix Q&infin;
		 */
		static Matrix createLinearSystemForAbsoluteDualQuadric(const unsigned int omegaRowIndex, const unsigned int omegaColumnIndex, const HomogenousMatrix4& iFlippedProjectionMatrix);
};

inline bool MultipleViewGeometry::pointIsCollinear(const Triangle2& triangle2d, const Vector2& point, const Scalar threshold)
{
	ocean_assert(triangle2d.isValid() && threshold > 0);

	const Vector3 barycenter = triangle2d.cartesian2barycentric(point);
	return Numeric::isBelow(Numeric::abs(barycenter.x()), threshold) || Numeric::isBelow(Numeric::abs(barycenter.y()), threshold) || Numeric::isBelow(Numeric::abs(barycenter.z()), threshold);
}

}

}

#endif // META_OCEAN_GEOMETRY_MULTIPLE_VIEW_GEOMETRY_H
