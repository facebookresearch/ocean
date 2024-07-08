/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/geometry/Homography.h"
#include "ocean/geometry/NonLinearOptimization.h"
#include "ocean/geometry/NonLinearOptimizationHomography.h"
#include "ocean/geometry/Utilities.h"

#include "ocean/math/Matrix.h"
#include "ocean/math/StaticMatrix.h"

namespace Ocean
{

namespace Geometry
{

SquareMatrix3 Homography::homographyMatrix(const Quaternion& left_T_right, const AnyCamera& leftCamera, const AnyCamera& rightCamera)
{
	ocean_assert(left_T_right.isValid());
	ocean_assert(leftCamera.isValid() && rightCamera.isValid());

	if (leftCamera.anyCameraType() != AnyCameraType::PINHOLE || rightCamera.anyCameraType() != AnyCameraType::PINHOLE)
	{
		ocean_assert(false && "A homography cannot be computed for the current camera type");

		return SquareMatrix3(false);
	}

	SquareMatrix3 rightIntrinsics = SquareMatrix3(false);
	rightIntrinsics(0, 0) = rightCamera.focalLengthX();
	rightIntrinsics(1, 1) = rightCamera.focalLengthY();
	rightIntrinsics(0, 2) = rightCamera.principalPointX();
	rightIntrinsics(1, 2) = rightCamera.principalPointY();
	rightIntrinsics(2, 2) = 1;

	SquareMatrix3 leftInvertedIntrinsics = SquareMatrix3(false);
	leftInvertedIntrinsics(0, 0) = leftCamera.inverseFocalLengthX();
	leftInvertedIntrinsics(1, 1) = leftCamera.inverseFocalLengthY();
	leftInvertedIntrinsics(0, 2) = -leftCamera.principalPointX() * leftCamera.inverseFocalLengthX();
	leftInvertedIntrinsics(1, 2) = -leftCamera.principalPointY() * leftCamera.inverseFocalLengthY();
	leftInvertedIntrinsics(2, 2) = 1;

	return rightIntrinsics * AnyCamera::flipMatrix3() * SquareMatrix3(left_T_right.inverted()) * AnyCamera::flipMatrix3() * leftInvertedIntrinsics;
}

SquareMatrix3 Homography::homographyMatrix(const Quaternion& left_T_right, const PinholeCamera& leftCamera, const PinholeCamera& rightCamera)
{
	ocean_assert(left_T_right.isValid());

	ocean_assert(leftCamera.isValid() && rightCamera.isValid());

	// for left camera:
	// x = K * invertedFlipped(I|0) * X
	// X = (K * invertedFlipped(I|0))^-1 * x

	// for right camera:
	// x' = K' * invertedFlipped(R|0) * X

	// x' = K' * invertedFlipped(R|0) * (K * invertedFlipped(I|0))^-1 * x
	//    = .... * x
	//    = K' * flip * inverted(R) * flip * K^-1 * x
	// x' = H * x

	//return rightCamera.intrinsic() * PinholeCamera::standard2InvertedFlipped(rotation) * (leftCamera.intrinsic() * PinholeCamera::standard2InvertedFlipped(SquareMatrix3(true))).inverted();
	return rightCamera.intrinsic() * PinholeCamera::flipMatrix3() * SquareMatrix3(left_T_right.inverted()) * PinholeCamera::flipMatrix3() * leftCamera.invertedIntrinsic();
}

SquareMatrix3 Homography::homographyMatrix(const Quaternion& world_R_left, const Quaternion& world_R_right, const PinholeCamera& leftCamera, const PinholeCamera& rightCamera)
{
	ocean_assert(world_R_left.isValid() && world_R_right.isValid());
	ocean_assert(leftCamera.isValid() && rightCamera.isValid());

	// right_H_world * (left_H_world)^-1

	return homographyMatrix(world_R_right, leftCamera, rightCamera) * homographyMatrix(world_R_left, leftCamera, leftCamera).inverted();
}

SquareMatrix3 Homography::homographyMatrix(const HomogenousMatrix4& world_T_rightCamera, const PinholeCamera& leftCamera, const PinholeCamera& rightCamera, const Plane3& plane)
{
	ocean_assert(world_T_rightCamera.isValid());
	ocean_assert(plane.isValid());

	ocean_assert(leftCamera && rightCamera);

#ifdef OCEAN_DEBUG

	// the plane must lie in front of both cameras
	const Line3 rayLeft(Vector3(0, 0, 0), Vector3(0, 0, -1));
	const Line3 rayRight(world_T_rightCamera.translation(), world_T_rightCamera.rotationMatrix() * Vector3(0, 0, -1));

	Vector3 objectPointLeft, objectPointRight;
	ocean_assert(plane.intersection(rayLeft, objectPointLeft));
	ocean_assert(plane.intersection(rayRight, objectPointRight));

	ocean_assert(objectPointLeft.z() < 0);
	ocean_assert((world_T_rightCamera.inverted() * objectPointRight).z() < 0);

#endif // OCEAN_DEBUG

	/**
	 * Homography determination in a coordinate system with viewing direction into positive z-space:
	 * The coordinate system is flipped around the x-axis about 180 degree (compared to the standard coordinate system in this framework).
	 * The first pose is expected to be located in the origin with viewing direction into the positive z-space.
	 * The second pose comes with a rotation matrix R and a translation offset t.
	 *
	 * n2 = (R + 1/d * t * n^T) * n1
	 * n2 =         H'          * n1
	 * p2 =     K * H' * K^-1   * p1
	 * p2 =         H           * p1
	 *
	 * Now we have to consider that this framework has a different camera viewing pose (with viewing direction into the negative z-space).
	 * p2 =          K * F * H' * F * K^-1            * p1
	 * p2 = K * F * (R + 1/d * t * n^T) * F * K^-1    * p1
	 *
	 * with: n2 normalized image point in the second frame, n1 normalized image point in the first frame
	 *       R the rotation matrix between both camera poses
	 *       t the translation offset between both camera poses
	 *       d the positive distance between the plane and the first camera pose (as this pose lies in the origin, it's the standard distance of the plane)
	 *       n the normal of the plane defined in the coordinate system of the first pose
	 *       H' the homography for normalized image points
	 *       K the intrinsic camera matrix
	 *       p2 image point in the second frame, n1 image point in the first frame (pixel position)
	 *       F flipping matrix allowing to flip the coordinate around the x-axis about 180 degree
	 *
	 * Therefore, the given transformation has to be inverted as this transformation transforms points located in the second coordinate system into points located in the first coordinate system.
	 * Further, a flipping has to be applied around the "internal" homography matrix so that the standard viewing transformation (along the negative z-axis can be used).
	 */

	const HomogenousMatrix4 rightCamera_T_world(world_T_rightCamera.inverted());

	const Vector3 translation(rightCamera_T_world.translation());
	const SquareMatrix3 rotation(rightCamera_T_world.rotationMatrix());

	ocean_assert(Numeric::isNotEqualEps(plane.distance()));
	const Vector3 scaledNormal(plane.normal() * (Scalar(1) / plane.distance()));

	const SquareMatrix3 translationNormal(translation * scaledNormal.x(), translation * scaledNormal.y(), translation * scaledNormal.z());

	return rightCamera.intrinsic() * PinholeCamera::flipMatrix3() * (rotation + translationNormal) * PinholeCamera::flipMatrix3() * leftCamera.invertedIntrinsic();
}

SquareMatrix3 Homography::homographyMatrix(const HomogenousMatrix4& poseLeft, const HomogenousMatrix4& poseRight, const PinholeCamera& leftCamera, const PinholeCamera& rightCamera, const Plane3& plane)
{
	const HomogenousMatrix4 iPoseLeft(poseLeft.inverted());

	const Vector3 normal(iPoseLeft.rotationMatrix(plane.normal()).normalized());
	const Scalar distance(-plane.signedDistance(poseLeft.translation()));

	ocean_assert(Numeric::isEqual(normal.length(), 1));
	const Plane3 planeLeft(normal, distance);

	return homographyMatrix(iPoseLeft * poseRight, leftCamera, rightCamera, planeLeft);
}

SquareMatrix3 Homography::homographyMatrix(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose, const Vector3& patternObjectPointTopLeft, const Vector3& patternObjectPointBottomLeft, const Vector3& patternObjectPointTopRight, const unsigned int patternWidth, const unsigned int patternHeight)
{
	ocean_assert(pinholeCamera.isValid() && pose.isValid());
	ocean_assert(patternObjectPointTopLeft != patternObjectPointBottomLeft);
	ocean_assert(patternObjectPointTopLeft != patternObjectPointTopRight);
	ocean_assert(patternObjectPointBottomLeft != patternObjectPointTopRight);

	ocean_assert(patternWidth > 0u && patternHeight > 0u);

	ocean_assert(PinholeCamera::isObjectPointInFrontIF(PinholeCamera::standard2InvertedFlipped(pose), patternObjectPointTopLeft));
	ocean_assert(PinholeCamera::isObjectPointInFrontIF(PinholeCamera::standard2InvertedFlipped(pose), patternObjectPointBottomLeft));
	ocean_assert(PinholeCamera::isObjectPointInFrontIF(PinholeCamera::standard2InvertedFlipped(pose), patternObjectPointTopRight));

	// we simply need to calculate the pose of the 'left' camera that observes the 'pattern' and uses a (can use any) perfect camera profile
	// therefore, we need to determine the center of the pattern (as the camera needs to be located above this center)
	// and we need to calculate the distance to the pattern (w.r.t. the field of view of our artificial camera)

	const Scalar horizontalLength = (patternObjectPointTopRight - patternObjectPointTopLeft).length();
	const Scalar verticalLength = (patternObjectPointBottomLeft - patternObjectPointTopLeft).length();

	const Vector3 xAxis((patternObjectPointTopRight - patternObjectPointTopLeft).normalizedOrZero());
	const Vector3 yAxis((patternObjectPointTopLeft - patternObjectPointBottomLeft).normalizedOrZero());

	// the x-axis and y-axis must be orthogonal
	ocean_assert(Numeric::isEqualEps(xAxis * yAxis));

	// our three points lie in the plane
	const Plane3 plane(patternObjectPointTopLeft, patternObjectPointBottomLeft, patternObjectPointTopRight);

	const Vector3 origin((patternObjectPointBottomLeft + patternObjectPointTopRight) * Scalar(0.5));
	const HomogenousMatrix4 originTransformation(xAxis, yAxis, plane.normal(), origin);

	// we create a perfect camera for the left pose with 45 deg horizontal field of view
	const PinholeCamera leftCamera(patternWidth, patternHeight, Numeric::pi_4());

	// we calculate the distance so that the camera perfectly captures the entire pattern
	const Scalar distance = horizontalLength * Scalar(1.2071067811865475244008443621048); // 1.2071067811865475244008443621048 = Scalar(0.5) / Numeric::tan(Numeric::pi_4() * Scalar(0.5));

	// now we address the aspect ratio of the resulting rectified frame
	// we have yScale = 1 if patternWidth / patternHeight == horizontalLength / verticalLength, otherwise we need to determine yScale
	// **NOTE** in case yScale != 1 we calculate a pose that does not come with orthonormal axes anymore - which is not a problem here in our case

	const Scalar xScale = 1;
	const Scalar yScale = (Scalar(patternWidth) * Scalar(verticalLength)) / (Scalar(patternHeight) * Scalar(horizontalLength));

	const HomogenousMatrix4 offsetTransformation(Vector3(0, 0, distance), Vector3(xScale, yScale, 1));

	const HomogenousMatrix4 leftPose(originTransformation * offsetTransformation);

	const SquareMatrix3 homography(Geometry::Homography::homographyMatrix(leftPose, pose, leftCamera, pinholeCamera, plane));

#ifdef OCEAN_DEBUG

	const Vector3 patternObjectPointBottomRight(patternObjectPointBottomLeft + patternObjectPointTopRight - patternObjectPointTopLeft);

	ocean_assert(leftCamera.projectToImage<true>(leftPose, patternObjectPointTopLeft, false).isEqual(Vector2(0, 0), Scalar(0.01)));
	ocean_assert(leftCamera.projectToImage<true>(leftPose, patternObjectPointBottomLeft, false).isEqual(Vector2(0, Scalar(patternHeight)), Scalar(0.01)));
	ocean_assert(leftCamera.projectToImage<true>(leftPose, patternObjectPointTopRight, false).isEqual(Vector2(Scalar(patternWidth), 0), Scalar(0.01)));
	ocean_assert(leftCamera.projectToImage<true>(leftPose, patternObjectPointBottomRight, false).isEqual(Vector2(Scalar(patternWidth), Scalar(patternHeight)), Scalar(0.01)));

	const Vector2 topLeftPatternCornerInFrame(homography * Vector2(0, 0));
	const Vector2 bottomLeftPatternCornerInFrame(homography * Vector2(0, Scalar(patternHeight)));
	const Vector2 topRightPatternCornerInFrame(homography * Vector2(Scalar(patternWidth), 0));
	const Vector2 bottomRightPatternCornerInFrame(homography * Vector2(Scalar(patternWidth), Scalar(patternHeight)));

	// we project the 3D object points without using the camera distortion - as obviously the homography cannot cover distortion
	const Vector2 topLeftProjected(pinholeCamera.projectToImage<true>(pose, patternObjectPointTopLeft, false));
	const Vector2 bottomLeftProjected(pinholeCamera.projectToImage<true>(pose, patternObjectPointBottomLeft, false));
	const Vector2 topRightProjected(pinholeCamera.projectToImage<true>(pose, patternObjectPointTopRight, false));
	const Vector2 bottomRightProjected(pinholeCamera.projectToImage<true>(pose, patternObjectPointBottomRight, false));

	ocean_assert(topLeftPatternCornerInFrame.isEqual(topLeftProjected, Scalar(0.1)));
	ocean_assert(bottomLeftPatternCornerInFrame.isEqual(bottomLeftProjected, Scalar(0.1)));
	ocean_assert(topRightPatternCornerInFrame.isEqual(topRightProjected, Scalar(0.1)));
	ocean_assert(bottomRightPatternCornerInFrame.isEqual(bottomRightProjected, Scalar(0.1)));

#endif

	return homography;
}

bool Homography::homographyMatrixSVD(const Vector2* leftPoints, const Vector2* rightPoints, const size_t correspondences, SquareMatrix3& right_H_left)
{
	ocean_assert(leftPoints != nullptr);
	ocean_assert(rightPoints != nullptr);
	ocean_assert(correspondences >= 4);

	Vectors2 normalizationLeftPoints(leftPoints, leftPoints + correspondences);
	Vectors2 normalizationRightPoints(rightPoints, rightPoints + correspondences);

	const SquareMatrix3 normalizedLeft_T_left = Normalization::calculateNormalizedPoints(normalizationLeftPoints.data(), correspondences);

	SquareMatrix3 right_T_normalizedRight(false);
	Normalization::calculateNormalizedPoints(normalizationRightPoints.data(), correspondences, &right_T_normalizedRight);
	ocean_assert(!right_T_normalizedRight.isSingular());

	// h = [h00, h01, h02, h10, h11, h12, h20, h21, h22]
	//
	//      [h00, h01, h02] first matrix row,
	//  H = [h10, h11, h12] second matrix row and
	//      [h20, h21, h22] third matrix row

	// left image point (x, y, 1)
	// right image point (u, v, 1)
	//
	// |  x  y  1     0  0  0     -ux  -uy  -u |
	// |  0  0  0     x  y  1     -vx  -vy  -v | * h == 0

	Matrix matrix(2 * correspondences, 9);
	for (size_t row = 0; row < correspondences; row++)
	{
		const Vector2& left = normalizationLeftPoints[row];
		const Vector2& right = normalizationRightPoints[row];

		Scalar* const row0 = matrix[2 * row + 0];

		row0[0] = left.x();
		row0[1] = left.y();
		row0[2] = 1;

		row0[3] = 0;
		row0[4] = 0;
		row0[5] = 0;

		row0[6] = -right.x() * left.x();
		row0[7] = -right.x() * left.y();
		row0[8] = -right.x();

		Scalar* const row1 = matrix[2 * row + 1];

		row1[0] = 0;
		row1[1] = 0;
		row1[2] = 0;

		row1[3] = left.x();
		row1[4] = left.y();
		row1[5] = 1;

		row1[6] = -right.y() * left.x();
		row1[7] = -right.y() * left.y();
		row1[8] = -right.y();
	}

	Matrix u, w, v;
	if (!matrix.singularValueDecomposition(u, w, v))
	{
		return false;
	}

	ocean_assert(v.rows() == 9 && v.columns() == 9);

#ifdef OCEAN_DEBUG
	// check for right order
	for (unsigned int n = 1u; n < w.rows(); ++n)
	{
		ocean_assert(w(n - 1) >= w(n));
	}
#endif

	const unsigned int lastColumn = (unsigned int)(v.columns()) - 1u;
	const SquareMatrix3 normalizedRight_H_normalizedLeft(v(0u, lastColumn), v(3u, lastColumn), v(6u, lastColumn), v(1u, lastColumn), v(4u, lastColumn), v(7u, lastColumn), v(2u, lastColumn), v(5u, lastColumn), v(8u, lastColumn));

	right_H_left = right_T_normalizedRight * normalizedRight_H_normalizedLeft * normalizedLeft_T_left;

	if (Numeric::isEqualEps(right_H_left[8]))
	{
		return false;
	}

	Homography::normalizeHomography(right_H_left);

	return right_H_left.isHomography();
}

bool Homography::homographyMatrixFromPointsAndLinesSVD(const Vector2* leftPoints, const Vector2* rightPoints, const size_t pointCorrespondences, const Line2* leftLines, const Line2* rightLines, const size_t lineCorrespondences, SquareMatrix3& right_H_left)
{
	ocean_assert(pointCorrespondences + lineCorrespondences >= 4);

	// **TODO** add normalization of points/lines to improve stability

	const size_t correspondences = pointCorrespondences + lineCorrespondences;

	MatrixD matrix(2 * correspondences, 9);

	// left image point (x, y, 1)
	// right image point (u, v, 1)
	//
	// |  x  y  1     0  0  0     -ux  -uy  -u |
	// |  0  0  0     x  y  1     -vx  -vy  -v | * h == 0

	size_t row = 0;

	for (size_t nPoint = 0; nPoint < pointCorrespondences; ++nPoint)
	{
		const VectorD2 left = VectorD2(leftPoints[nPoint]);
		const VectorD2 right = VectorD2(rightPoints[nPoint]);

		double* const row0 = matrix[row++];

		row0[0] = left.x();
		row0[1] = left.y();
		row0[2] = 1.0;

		row0[3] = 0.0;
		row0[4] = 0.0;
		row0[5] = 0.0;

		row0[6] = -right.x() * left.x();
		row0[7] = -right.x() * left.y();
		row0[8] = -right.x();

		double* const row1 = matrix[row++];

		row1[0] = 0.0;
		row1[1] = 0.0;
		row1[2] = 0.0;

		row1[3] = left.x();
		row1[4] = left.y();
		row1[5] = 1.0;

		row1[6] = -right.y() * left.x();
		row1[7] = -right.y() * left.y();
		row1[8] = -right.y();
	}

	// left image line (x, y, 1)
	// right image line (u, v, 1)
	//
	// |  u  0 -ux     v  0 -vx     1  0  -x |
	// |  0  u -uy     0  v -vy     0  1  -y | * h == 0

	for (size_t nLine = 0; nLine < lineCorrespondences; ++nLine)
	{
		LineD2 leftLine(leftLines[nLine]);
		LineD2 rightLine(rightLines[nLine]);

		if constexpr (std::is_same<float, Scalar>::value)
		{
			// for higher precision, normalizing the line direction again

			leftLine = LineD2(leftLine.point(), leftLine.direction().normalized());
			rightLine = LineD2(rightLine.point(), rightLine.direction().normalized());
		}

		VectorD3 left = leftLine.decomposeNormalDistance();
		VectorD3 right = rightLine.decomposeNormalDistance();

		if (NumericD::isEqualEps(left.z()) || NumericD::isEqualEps(right.z()))
		{
			ocean_assert(false && "One of the lines intersects the origin, we need to normalize the input to avoid this!");
			return false;
		}

		left /= left.z();
		right /= right.z();

		double* const row0 = matrix[row++];

		row0[0] = right.x();
		row0[1] = 0;
		row0[2] = -left.x() * right.x();

		row0[3] = right.y();
		row0[4] = 0;
		row0[5] = -left.x() * right.y();

		row0[6] = 1;
		row0[7] = 0;
		row0[8] = -left.x();

		double* const row1 = matrix[row++];

		row1[0] = 0;
		row1[1] = right.x();
		row1[2] = -right.x() * left.y();

		row1[3] = 0;
		row1[4] = right.y();
		row1[5] = -left.y() * right.y();

		row1[6] = 0;
		row1[7] = 1;
		row1[8] = -left.y();
	}

	ocean_assert(row == (pointCorrespondences + lineCorrespondences) * 2);

	MatrixD u, w, v;
	if (!matrix.singularValueDecomposition(u, w, v))
	{
		return false;
	}

	ocean_assert(v.rows() == 9 && v.columns() == 9);

#ifdef OCEAN_DEBUG
	// check for right order
	for (unsigned int n = 1u; n < w.rows(); ++n)
	{
		ocean_assert(w(n - 1) >= w(n));
	}
#endif

	const unsigned int lastColumn = (unsigned int)(v.columns()) - 1u;
	SquareMatrixD3 result(v(0u, lastColumn), v(3u, lastColumn), v(6u, lastColumn), v(1u, lastColumn), v(4u, lastColumn), v(7u, lastColumn), v(2u, lastColumn), v(5u, lastColumn), v(8u, lastColumn));

	if (NumericD::isEqualEps(result[8]))
	{
		// irregular transformation
		return false;
	}

	Homography::normalizeHomography(result);

	right_H_left = SquareMatrix3(result);

	return right_H_left.isHomography();
}

bool Homography::homographyMatrixLinear(const Vector2* leftPoints, const Vector2* rightPoints, const size_t correspondences, SquareMatrix3& right_H_left, unsigned int optimizationIterations)
{
	ocean_assert(leftPoints && rightPoints && correspondences >= 4);

	if (!homographyMatrixLinearWithoutOptimations(leftPoints, rightPoints, correspondences, right_H_left))
	{
		return false;
	}

	if (optimizationIterations != 0u)
	{
		SquareMatrix3 optimized_right_H_left;
		if (!NonLinearOptimizationHomography::optimizeHomography<Estimator::ET_SQUARE>(right_H_left, leftPoints, rightPoints, correspondences, 9u, optimized_right_H_left, optimizationIterations))
		{
			// we could also return True as an initial homography has been determined; however, as the non-linear-optimization failed the initial homography must be faulty
			return false;
		}

		right_H_left = optimized_right_H_left;
	}

	return right_H_left.isHomography();
}

bool Homography::homographyMatrixLinearWithoutOptimations(const Vector2* leftPoints, const Vector2* rightPoints, const size_t correspondences, SquareMatrix3& right_H_left)
{
	ocean_assert(leftPoints && rightPoints && correspondences >= 4);

	// determine    [h0 h1 h2]
	//          H = [h3 h4 h5] in equation: rightPoint = H * leftPoint
	//              [h6 h7  1]

	// [r_z * r_x]   [h0 h1 h2]   [l_x]
	// [r_z * r_y] = [h3 h4 h5] * [l_y]
	// [   r_z   ]   [h6 h7  1]   [ 1 ]

	// reorder linear equation to A * h = b, for two rows of A, each pair of image points (l_x, l_y) and (r_x, r_y) will add two additional rows
	//              [ l_x    l_y    1     0      0     0    -l_x*r_x   -l_y*r_x ]
	//          A = [  0      0     0    l_x    l_y    1    -l_x*r_y   -l_y*r_y ]
	//              [                        ...                                ]

	// with b: [ r_x ]
	//         [ r_y ]
	//         [ ... ]

	// solve A^t * A * h = A^t * b for h

	// optimizations I: solving M^t * M and M^t * b

	//                      [    l_x*l_x        l_x*l_y         l_x           0                0            0               -l_x*l_x*r_x                        -l_x*l_y*r_x           ]
	//                      [    l_x*l_y        l_y*l_y         l_y           0                0            0               -l_x*l_y*r_x                        -l_y*l_y*r_x           ]
	// A^t * A is sum of    [      l_x            l_y            1            0                0            0                 -l_x*r_x                            -l_y*r_x             ]
	//                      [       0              0             0         l_x*l_x          l_x*l_y        l_x              -l_x*l_x*r_y                        -l_x*l_y*r_y           ]
	//                      [       0              0             0         l_x*l_y          l_y*l_y        l_y              -l_x*l_y*r_y                        -l_y*l_y*r_y           ]
	//                      [       0              0             0           l_x              l_y           1                 -l_x*r_y                            -l_y*r_y             ]
	//                      [ -l_x*l_x*r_x   -l_x*l_y*r_x    -l_x*r_x   -l_x*l_x*r_y     -l_x*l_y*r_y   -l_y*r_x   l_x*l_x*r_x*r_x+l_x*l_x*r_y*r_y     l_x*l_y*r_x*r_x+l_x*l_y*r_y*r_y ]
	//                      [ -l_x*l_y*r_x   -l_y*l_y*r_x    -l_x*r_y   -l_x*l_y*r_x     -l_y*l_y*r_y   -l_y*r_y   l_x*l_y*r_x*r_x+l_x*l_y*r_y*r_y     l_y*l_y*r_x*r_x+l_y*l_y*r_y*r_y ]
	//
	// note: A^t * A is symmetric:
	//
	//                      [ a b c 0 0 0 d e ]
	//                      [ . f g 0 0 0 e h ]
	//                      [ . . ~ 0 0 0 i j ]
	//                      [ . . . a b c k l ]
	//                      [ . . . . f g l m ]
	//                      [ . . . . . ~ n o ]
	//                      [ . . . . . . p q ]
	//                      [ . . . . . . . r ], ~ = correspondences

	//                      [          l_x*r_x         ]
	//                      [          l_y*r_x         ]
	// A^t * b is sum of    [            r_x           ]
	//                      [          l_x*r_y         ]
	//                      [          l_y*r_y         ]
	//                      [            r_y           ]
	//                      [ -l_x*r_x*r_x-l_x*r_y*r_y ]
	//                      [ -l_y*r_x*r_x-l_y*r_y*r_y ]

	// note: when we have 4 correspondences we could avoid to create the pseudo inverse

	Scalar vATA[18] = {0};
	Matrix ATb(8, 1, false);

	for (size_t i = 0; i < correspondences; i++)
	{
		const Scalar& lx = leftPoints[i].x();
		const Scalar& ly = leftPoints[i].y();

		const Scalar& rx = rightPoints[i].x();
		const Scalar& ry = rightPoints[i].y();

		const Scalar lxlx = lx * lx;
		const Scalar lxly = lx * ly;
		const Scalar lyly = ly * ly;

		// a, b, c
		vATA[0] += lxlx;
		vATA[1] += lxly;
		vATA[2] += lx;

		// d, e
		vATA[3] -= lxlx * rx;
		vATA[4] -= lxly * rx;

		// f, g
		vATA[5] += lyly;
		vATA[6] += ly;

		// h
		vATA[7] -= lyly * rx;

		// i, j
		vATA[8] -= lx * rx;
		vATA[9] -= ly * rx;

		// k, l
		vATA[10] -= lxlx * ry;
		vATA[11] -= lxly * ry;

		// m
		vATA[12] -= lyly * ry;

		// n, o
		vATA[13] -= lx * ry;
		vATA[14] -= ly * ry;

		const Scalar rxrx = rx * rx;
		const Scalar ryry = ry * ry;
		const Scalar rxrx_ryry = rxrx + ryry;

		// p, q
		vATA[15] += lxlx * rxrx_ryry;
		vATA[16] += lxly * rxrx_ryry;

		// r
		vATA[17] += lyly * rxrx_ryry;

		ATb(0) += lx * rx;
		ATb(1) += ly * rx;
		ATb(2) += rx;
		ATb(3) += lx * ry;
		ATb(4) += ly * ry;
		ATb(5) += ry;
		ATb(6) -= lx * rxrx_ryry;
		ATb(7) -= ly * rxrx_ryry;
	}

	// [ a b c 0 0 0 d e ]
	// [ . f g 0 0 0 e h ]
	// [ . . ~ 0 0 0 i j ]
	// [ . . . a b c k l ]
	// [ . . . . f g l m ]
	// [ . . . . . ~ n o ]
	// [ . . . . . . p q ]
	// [ . . . . . . . r ]

	// a b c d e f g h i j  k  l  m  n  o  p  q  r
	// 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17

	Matrix ATA(8, 8, false);

	ATA(0) = vATA[0];
	ATA(1) = vATA[1];
	ATA(2) = vATA[2];
	ATA(6) = vATA[3];
	ATA(7) = vATA[4];

	ATA( 8) = vATA[1];
	ATA( 9) = vATA[5];
	ATA(10) = vATA[6];
	ATA(14) = vATA[4];
	ATA(15) = vATA[7];

	ATA(16) = vATA[2];
	ATA(17) = vATA[6];
	ATA(18) = Scalar(correspondences);
	ATA(22) = vATA[8];
	ATA(23) = vATA[9];

	ATA(27) = vATA[0];
	ATA(28) = vATA[1];
	ATA(29) = vATA[2];
	ATA(30) = vATA[10];
	ATA(31) = vATA[11];

	ATA(35) = vATA[1];
	ATA(36) = vATA[5];
	ATA(37) = vATA[6];
	ATA(38) = vATA[11];
	ATA(39) = vATA[12];

	ATA(43) = vATA[2];
	ATA(44) = vATA[6];
	ATA(45) = Scalar(correspondences);
	ATA(46) = vATA[13];
	ATA(47) = vATA[14];

	ATA(48) = vATA[3];
	ATA(49) = vATA[4];
	ATA(50) = vATA[8];
	ATA(51) = vATA[10];
	ATA(52) = vATA[11];
	ATA(53) = vATA[13];
	ATA(54) = vATA[15];
	ATA(55) = vATA[16];

	ATA(56) = vATA[4];
	ATA(57) = vATA[7];
	ATA(58) = vATA[9];
	ATA(59) = vATA[11];
	ATA(60) = vATA[12];
	ATA(61) = vATA[14];
	ATA(62) = vATA[16];
	ATA(63) = vATA[17];


	Matrix x(8, 1);
	if (!ATA.solve<Matrix::MP_SYMMETRIC>(ATb, x))
	{
		return false;
	}

	right_H_left(0, 0) = x(0);
	right_H_left(0, 1) = x(1);
	right_H_left(0, 2) = x(2);

	right_H_left(1, 0) = x(3);
	right_H_left(1, 1) = x(4);
	right_H_left(1, 2) = x(5);

	right_H_left(2, 0) = x(6);
	right_H_left(2, 1) = x(7);
	right_H_left(2, 2) = 1;

#ifdef OCEAN_USE_SLOWER_IMPLEMENTATION

	Matrix A(2 * correspondences, 8, false);
	Matrix b(2 * correspondences, 1, false);

	for (unsigned int n = 0u; n < correspondences; ++n)
	{
		A[2 * n + 0][0] = leftPoints[n].x();
		A[2 * n + 0][1] = leftPoints[n].y();
		A[2 * n + 0][2] = 1;

		A[2 * n + 1][3] = leftPoints[n].x();
		A[2 * n + 1][4] = leftPoints[n].y();
		A[2 * n + 1][5] = 1;

		A[2 * n + 0][6] = -leftPoints[n].x() * rightPoints[n].x();
		A[2 * n + 0][7] = -leftPoints[n].y() * rightPoints[n].x();

		A[2 * n + 1][6] = -leftPoints[n].x() * rightPoints[n].y();
		A[2 * n + 1][7] = -leftPoints[n].y() * rightPoints[n].y();

		b[2 * n + 0][0] = rightPoints[n].x();
		b[2 * n + 1][0] = rightPoints[n].y();
	}

	Matrix ATA = A.transposed() * A;
	Matrix ATb = A.transposed() * b;

	Matrix x(8, 1);
	ATA.solve(_ATb, x);

	right_H_left(0, 0) = x(0);
	right_H_left(0, 1) = x(1);
	right_H_left(0, 2) = x(2);

	right_H_left(1, 0) = x(3);
	right_H_left(1, 1) = x(4);
	right_H_left(1, 2) = x(5);

	right_H_left(2, 0) = x(6);
	right_H_left(2, 1) = x(7);
	right_H_left(2, 2) = 1;

#endif // OCEAN_USE_SLOWER_IMPLEMENTATION

	return true;
}

bool Homography::affineMatrix(const ImagePoint* leftPoints, const ImagePoint* rightPoints, const size_t correspondences, SquareMatrix3& right_A_left)
{
	ocean_assert(leftPoints != nullptr);
	ocean_assert(rightPoints != nullptr);
	ocean_assert(correspondences >= 3);

	// determine    [a b c]
	//          A = [d e f] in equation: A * leftPoint = rightPoint

	// reorder linear equation to M * x = rightPoint
	//          M = [lp_x   lp_y    1     0     0    0]
	//              [  0      0     0   lp_x  lp_y   1]
	//
	// solve M^t * M * x = M^t * b for x

	// optimizations I: solving M^t * M and M^t * b


	//                      [x*x   x*y   x    0     0    0]
	//                      [x*y   y*y   y    0     0    0]
	// M^t * M is sum of    [ x     y    1    0     0    0]
	//                      [ 0     0    0   x*y   y*y   y]
	//                      [ 0     0    0    x     y    1]
	//                      [ 0     0    0   x*x   x*y   x]

	//                      [y1*x2]
	//                      [x1*x2]
	// M^t * b is sum of    [x2]
	//                      [x1*y2]
	//                      [y1*y2]
	//                      [y2]

	Scalar lx2Sum = 0;
	Scalar ly2Sum = 0;
	Scalar lxlySum = 0;
	Scalar lxSum = 0;
	Scalar lySum = 0;
	Scalar rxSum = 0;
	Scalar rySum = 0;
	Scalar lxrxSum = 0;
	Scalar lyrxSum = 0;
	Scalar lxrySum = 0;
	Scalar lyrySum = 0;
	Scalar sum = Scalar(correspondences);

	for (size_t i = 0; i < correspondences; i++)
	{
		const Scalar x1 = leftPoints[i].x();
		const Scalar y1 = leftPoints[i].y();
		const Scalar x2 = rightPoints[i].x();
		const Scalar y2 = rightPoints[i].y();

		lx2Sum += x1 * x1;
		ly2Sum += y1 * y1;
		lxlySum += x1 * y1;
		lxSum += x1;
		lySum += y1;
		rxSum += x2;
		rySum += y2;
		lxrxSum += x1 * x2;
		lyrxSum += y1 * x2;
		lxrySum += x1 * y2;
		lyrySum += y1 * y2;
	}

	// optimizations II: solving equation Ax=b
	//                     R^t * R = A
	//                     R^t * y = b
	//                     R   * x = y
	//
	//          [ r00   r01   r02   0     0     0  ]
	//          [  0    r11   r12   0     0     0  ]
	// R    =   [  0     0    r22   0     0     0  ]
	//          [  0     0     0   r00   r01   r02 ]
	//          [  0     0     0    0    r11   r12 ]
	//          [  0     0     0    0     0    r22 ]

	if (lx2Sum < Numeric::eps())
	{
		ocean_assert(false && "This should not happen for a valid equation.");
		return false;
	}

	const Scalar r00 = Numeric::sqrt(lx2Sum);
	const Scalar r01 = lxlySum / r00;
	const Scalar r02 = lxSum / r00;
	const Scalar r11Sqr = ly2Sum - lxlySum * lxlySum / lx2Sum;

	if (r11Sqr < Numeric::eps())
	{
		ocean_assert(false && "This should not happen for a valid equation.");
		return false;
	}

	const Scalar r11 = Numeric::sqrt(r11Sqr);
	const Scalar r12 = (lySum - (lxlySum * lxSum/ lx2Sum)) / r11;
	const Scalar r22Sqr = sum - (lxSum * lxSum/ lx2Sum) - (r12 * r12);

	if (r22Sqr < Numeric::eps())
	{
		ocean_assert(false && "This should not happen for a valid equation.");
		return false;
	}
	//	Scalar r22 = Numeric::sqrt(r22Sqr);

	const Scalar Y0 = lxrxSum / r00;
	const Scalar Y1 = (lyrxSum - r01 * Y0) / r11;
	// Scalar Y2 = (rxSum - r12 * Y1 - r02 * Y0) / r22;
	const Scalar Y3 = (lxrySum / r00);
	const Scalar Y4 = (lyrySum - r01 * Y3) / r11;
	// Scalar Y5 = (rySum - r12 * Y4 - r02 * Y3) / r22;

	// SquareMatrix3 is column based
	right_A_left[8] = Scalar(1); // a33
	right_A_left[5] = Scalar(0); // a32
	right_A_left[2] = Scalar(0); // a31
	right_A_left[7] = (rySum - r12 * Y4 - r02 * Y3) / r22Sqr; // a23
	right_A_left[4] = (Y4 - r12 * right_A_left[7]) / r11; // a22
	right_A_left[1] = (Y3 - r02 * right_A_left[7] - r01 * right_A_left[4]) / r00; // a21
	right_A_left[6] = (rxSum - r12 * Y1 - r02 * Y0) / r22Sqr; //a13
	right_A_left[3] = (Y1 - r12 * right_A_left[6]) / r11; //a12
	right_A_left[0] = (Y0 - r02 * right_A_left[6] - r01 * right_A_left[3]) / r00; //a11

#ifdef OCEAN_USE_SLOWER_IMPLEMENTATION
	Matrix mMatrix(6, 6, false);
	mMatrix(0, 0) = lx2Sum;
	mMatrix(0, 1) = lxlySum;
	mMatrix(0, 2) = lxSum;
	mMatrix(1, 0) = lxlySum;
	mMatrix(1, 1) = ly2Sum;
	mMatrix(1, 2) = lySum;
	mMatrix(2, 0) = lxSum;
	mMatrix(2, 1) = lySum;
	mMatrix(2, 2) = sum;
	mMatrix(3, 3) = lx2Sum;
	mMatrix(3, 4) = lxlySum;
	mMatrix(3, 5) = lxSum;
	mMatrix(4, 3) = lxlySum;
	mMatrix(4, 4) = ly2Sum;
	mMatrix(4, 5) = lySum;
	mMatrix(5, 3) = lxSum;
	mMatrix(5, 4) = lySum;
	mMatrix(5, 5) = sum;

	StaticMatrix<Scalar, 6, 1> matrixMtb;
	matrixMtb[0] = lxrxSum;
	matrixMtb[1] = lyrxSum;
	matrixMtb[2] = rxSum;
	matrixMtb[3] = lxrySum;
	matrixMtb[4] = lyrySum;
	matrixMtb[5] = rySum;

	// solve for x
	StaticMatrix<Scalar, 6, 1> matrixA;
	if (!mMatrix.solve<Matrix::MP_SYMMETRIC>(matrixMtb.data(), matrixA.data()))
	{
		return false;
	}

	// SquareMatrix3 is column based
	right_A_left[0] = matrixA(0, 0);
	right_A_left[1] = matrixA(3, 0);
	right_A_left[2] = Scalar(0);
	right_A_left[3] = matrixA(1, 0);
	right_A_left[4] = matrixA(4, 0);
	right_A_left[5] = Scalar(0);
	right_A_left[6] = matrixA(2, 0);
	right_A_left[7] = matrixA(5, 0);
	right_A_left[8] = Scalar(1);

#endif // OCEAN_USE_SLOWER_IMPLEMENTATION

#ifdef OCEAN_USE_SLOWER_IMPLEMENTATION

	Matrix mMatrix(2 * correspondences, 6);

	for (size_t i = 0; i < correspondences; ++i)
	{
		const size_t j = 2 * i;

		mMatrix(j + 0, 0) = leftPoints[i].x();
		mMatrix(j + 0, 1) = leftPoints[i].y();
		mMatrix(j + 0, 2) = Scalar(1);
		mMatrix(j + 0, 3) = Scalar(0);
		mMatrix(j + 0, 4) = Scalar(0);
		mMatrix(j + 0, 5) = Scalar(0);
		mMatrix(j + 1, 0) = Scalar(0);
		mMatrix(j + 1, 1) = Scalar(0);
		mMatrix(j + 1, 2) = Scalar(0);
		mMatrix(j + 1, 3) = leftPoints[i].x();
		mMatrix(j + 1, 4) = leftPoints[i].y();
		mMatrix(j + 1, 5) = Scalar(1);
	}

	Matrix mMatrixTranspose = mMatrix.transposed();

	Matrix bMatrix(2 * correspondences, 1);
	for (size_t i = 0; i < correspondences; ++i)
	{
		const size_t j = 2 * i;

		bMatrix(j + 0, 0) = rightPoints[i].x();
		bMatrix(j + 1, 0) = rightPoints[i].y();
	}

	// A is [6 x 1]
	Matrix aMatrix;
	if (!(mMatrixTranspose * mMatrix).solve(mMatrixTranspose * bMatrix, aMatrix))
	{
		return false;
	}

	// SquareMatrix3 is column based
	right_A_left[0] = aMatrix(0);
	right_A_left[1] = aMatrix(3);
	right_A_left[2] = Scalar(0);
	right_A_left[3] = aMatrix(1);
	right_A_left[4] = aMatrix(4);
	right_A_left[5] = Scalar(0);
	right_A_left[6] = aMatrix(2);
	right_A_left[7] = aMatrix(5);
	right_A_left[8] = Scalar(1);

#endif // OCEAN_USE_SLOWER_IMPLEMENTATION

	return true;
}

bool Homography::similarityMatrix(const ImagePoint* leftPoints, const ImagePoint* rightPoints, const size_t correspondences, SquareMatrix3& right_S_left)
{
	ocean_assert(leftPoints != nullptr && rightPoints != nullptr && correspondences >= 2);

	// determine       [a  -b  t_x]
	//             S = [b   a  t_y] in equation: rightPoint = S * leftPoint
	//
	// reorder linear equation to rightPoint = M * x = M * [a b t_x t_y]^T
	//             M = [lp_x  -lp_y  1  0]
	//                 [lp_y   lp_x  0  1]
	//
	// solve M^t * M * x = M^t * b for x

	// optimizations I:solving M^t * M and M^t * b

	//                      [x*x+y*y     y*x-x*y=0   x   y]
	// M^t * M is sum of    [y*x-x*y=0   x*x+y*y     y   x]
	//                      [x              -y       1   0]
	//                      [y               x       0   1]

	//                      [x1*x2   +  y1*y2]
	// M^t * b is sum of    [-y1*x2  +  x1*y2]
	//                      [x2]
	//                      [y2]

	Scalar lx2ly2Sum = 0;
	Scalar lxSum = 0;
	Scalar lySum = 0;
	Scalar mtb0 = 0;
	Scalar mtb1 = 0;
	Scalar rxSum = 0;
	Scalar rySum = 0;
	Scalar sum = Scalar(correspondences);

	// stacked matrix multiplications
	for (size_t i = 0; i < correspondences; i++)
	{
		const Scalar lx = leftPoints[i].x();
		const Scalar ly = leftPoints[i].y();
		const Scalar rx = rightPoints[i].x();
		const Scalar ry = rightPoints[i].y();

		const Scalar lx2ly2 = lx * lx + ly * ly;

		lx2ly2Sum += lx2ly2;
		lxSum += lx;
		lySum += ly;
		mtb0 += lx * rx + ly * ry;
		mtb1 += -ly * rx + lx * ry;
		rxSum += rx;
		rySum += ry;
	}

	// optimizations II: solving equation Ax=b
	//                   R^t * R = A
	//                   R^t * y = b
	//                   R   * x = y

	//          [a   0    x/a   y/a]
	//          [0   a   -y/a   x/a]
	// R    =   [0   0     d     0 ]
	//          [0   0     0     d ]

	if (lx2ly2Sum < Numeric::eps())
	{
		ocean_assert(false && "This should not happen for a valid equation.");
		return false;
	}

	// Scalar a = Numeric::sqrt(lx2ly2Sum);
	// Scalar b = lxSum / a;
	// Scalar c = lySum / a;
	const Scalar dSqr = sum - ((lxSum * lxSum + lySum * lySum) / lx2ly2Sum);

	if (dSqr < 0)
	{
		ocean_assert(false && "This should not happen for a valid equation.");
		return false;
	}

	const Scalar d = Numeric::sqrt(dSqr);

	if (d < 0)
	{
		ocean_assert(false && "This should not happen for a valid equation.");
		return false;
	}

	// Scalar Y0 = b0 / a;
	// Scalar Y1 = b1 / a;
	const Scalar Y2 = (rxSum + (lySum * mtb1 / lx2ly2Sum) - (lxSum * mtb0 / lx2ly2Sum)) / d;
	const Scalar Y3 = (rySum - (lxSum * mtb1 / lx2ly2Sum) - (lySum * mtb0 / lx2ly2Sum)) / d;

	// SquareMatrix3 is column based
	right_S_left[8] = Scalar(1); // a33
	right_S_left[5] = Scalar(0); // a32
	right_S_left[2] = Scalar(0); // a31
	right_S_left[7] = Y3 / d; // t_y
	right_S_left[6] = Y2 / d; // t_x
	right_S_left[1] = (mtb1 + (lySum * Y2 - lxSum * Y3) / d) / lx2ly2Sum; // p2
	right_S_left[3] = -right_S_left[1];// -p2
	right_S_left[4] = (mtb0 - (lxSum * Y2 + lySum * Y3) / d) / lx2ly2Sum; // p1
	right_S_left[0] = right_S_left[4]; //p1

#ifdef OCEAN_USE_SLOWER_IMPLEMENTATION

	Matrix mMatrix(4, 4, false);
	mMatrix(0, 0) = lx2ly2Sum;
	mMatrix(0, 2) = lxSum;
	mMatrix(0, 3) = lySum;
	mMatrix(1, 1) = lx2ly2Sum;
	mMatrix(1, 2) = -lySum;
	mMatrix(1, 3) = lxSum;
	mMatrix(2, 0) = lxSum;
	mMatrix(2, 1) = -lySum;
	mMatrix(2, 2) = sum;
	mMatrix(3, 0) = lySum;
	mMatrix(3, 1) = lxSum;
	mMatrix(3, 3) = sum;

	StaticMatrix<Scalar, 4, 1> matrixMtb(false);

	// stacked matrix multiplications
	for (size_t i = 0; i < correspondences; i++)
	{
		const Scalar x1 = leftPoints[i].x();
		const Scalar y1 = leftPoints[i].y();
		const Scalar x2 = rightPoints[i].x();
		const Scalar y2 = rightPoints[i].y();

		matrixMtb[0] += x1 * x2 + y1 * y2;
		matrixMtb[1] += -y1 * x2 + x1 * y2;
		matrixMtb[2] += x2;
		matrixMtb[3] += y2;
	}

	// solve for x
	StaticMatrix<Scalar, 4, 1> matrixA;
	if (!mMatrix.solve<Matrix::MP_SYMMETRIC>(matrixMtb.data(), matrixA.data()))
	{
		return false;
	}

	// SquareMatrix3 is column based
	right_S_left[0] = matrixA(0, 0);
	right_S_left[1] = matrixA(1, 0);
	right_S_left[2] = Scalar(0);
	right_S_left[3] = -matrixA(1, 0);
	right_S_left[4] = matrixA(0, 0);
	right_S_left[5] = Scalar(0);
	right_S_left[6] = matrixA(2, 0);
	right_S_left[7] = matrixA(3, 0);
	right_S_left[8] = Scalar(1);

#endif// OCEAN_USE_SLOWER_IMPLEMENTATION

#ifdef OCEAN_USE_SLOWER_IMPLEMENTATION

	Matrix mMatrix(2 * correspondences, 4);
	for (size_t i = 0; i < correspondences; ++i)
	{
		const size_t j = 2 * i;

		mMatrix(j + 0, 0) = leftPoints[i].x();
		mMatrix(j + 0, 1) = -leftPoints[i].y();
		mMatrix(j + 0, 2) = Scalar(1);
		mMatrix(j + 0, 3) = Scalar(0);
		mMatrix(j + 1, 0) = leftPoints[i].y();
		mMatrix(j + 1, 1) = leftPoints[i].x();
		mMatrix(j + 1, 2) = Scalar(0);
		mMatrix(j + 1, 3) = Scalar(1);
	}

	Matrix mMatrixTranspose = mMatrix.transposed();

	Matrix bMatrix(2 * correspondences, 1);
	for (size_t i = 0; i < correspondences; ++i)
	{
		const size_t j = 2 * i;

		bMatrix(j + 0, 0) = rightPoints[i].x();
		bMatrix(j + 1, 0) = rightPoints[i].y();
	}

	// A is [4 x 1]
	Matrix aMatrix;
	if (!(mMatrixTranspose * mMatrix).solve(mMatrixTranspose * bMatrix, aMatrix))
	{
		return false;
	}

	// SquareMatrix3 is column based
	right_S_left[0] = aMatrix(0);
	right_S_left[1] = aMatrix(1);
	right_S_left[2] = Scalar(0);
	right_S_left[3] = -aMatrix(1);
	right_S_left[4] = aMatrix(0);
	right_S_left[5] = Scalar(0);
	right_S_left[6] = aMatrix(2);
	right_S_left[7] = aMatrix(3);
	right_S_left[8] = Scalar(1);

#endif

	return !right_S_left.isSingular();
}

bool Homography::homotheticMatrix(const ImagePoint* leftPoints, const ImagePoint* rightPoints, const size_t correspondences, SquareMatrix3& right_H_left)
{
	ocean_assert(leftPoints != nullptr && rightPoints != nullptr);
	ocean_assert(correspondences >= 2);

	// determine       [s  0  t_x]
	//             H = [0  s  t_y] in equation: rightPoint = H * leftPoint
	//
	// reorder linear equation to rightPoint = M * x = M * [s t_x t_y]^T
	//             M = [lp_x  1  0]
	//                 [lp_y  0  1]
	//
	// solve M^t * M * x = M^t * b for x

	// optimizations I: solving M^t * M and M^t * b

	//                      [lp_x^2 + lp_y^2   lp_x   lp_y]
	// M^t * M is sum of    [lp_x                1      0 ]
	//                      [lp_y                0      1 ]

	//                      [lp_x * rp_x   +  lp_y * rp_y]
	// M^t * b is sum of    [             rp_x           ]
	//                      [             rp_y           ]

	Scalar lx2ly2Sum = 0;
	Scalar lxSum = 0;
	Scalar lySum = 0;
	Scalar mtb = 0;

	Scalar rxSum = 0;
	Scalar rySum = 0;

	// stacked matrix multiplications
	for (size_t i = 0; i < correspondences; i++)
	{
		const Scalar& lx = leftPoints[i].x();
		const Scalar& ly = leftPoints[i].y();
		const Scalar& rx = rightPoints[i].x();
		const Scalar& ry = rightPoints[i].y();

		lx2ly2Sum += lx * lx + ly * ly;
		lxSum += lx;
		lySum += ly;

		mtb += lx * rx + ly * ry;
		rxSum += rx;
		rySum += ry;
	}

	const SquareMatrix3 matrix(Vector3(lx2ly2Sum, lxSum, lySum), Vector3(lxSum, Scalar(correspondences), 0), Vector3(lySum, 0, Scalar(correspondences)));

	Vector3 result;
	if (!matrix.solve(Vector3(mtb, rxSum, rySum), result))
	{
		return false;
	}

	right_H_left[0] = result[0];
	right_H_left[1] = 0;
	right_H_left[2] = 0;

	right_H_left[3] = 0;
	right_H_left[4] = result[0];
	right_H_left[5] = 0;

	right_H_left[6] = result[1];
	right_H_left[7] = result[2];
	right_H_left[8] = 1;

	return true;
}

SquareMatrix3 Homography::factorizeHomographyMatrix(const SquareMatrix3& homography, const PinholeCamera& leftCamera, const PinholeCamera& rightCamera)
{
	ocean_assert(!homography.isNull());
	ocean_assert(leftCamera.isValid() && rightCamera.isValid());

	// we invert the following function:
	// homography = rightCamera.intrinsic() * PinholeCamera::flipMatrix3() * rotation.inverted() * PinholeCamera::flipMatrix3() * leftCamera.invertedIntrinsic();

	return (PinholeCamera::flipMatrix3() * rightCamera.invertedIntrinsic() * homography * leftCamera.intrinsic() * PinholeCamera::flipMatrix3()).inverted();
}

bool Homography::factorizeHomographyMatrix(const SquareMatrix3& right_H_left, const PinholeCamera& leftCamera, const PinholeCamera& rightCamera, const ImagePoint* leftImagePoints, const ImagePoint* rightImagePoints, const size_t correspondences, HomogenousMatrix4 world_T_rightCameras[2], Vector3 normals[2])
{
	// See: An Invitation to 3D Vision, Y. Ma, S. Soatto, J. Kosecka, S. Sastry for details

	ocean_assert(right_H_left.isHomography());
	ocean_assert(leftCamera.isValid());
	ocean_assert(rightCamera.isValid());
	ocean_assert(leftImagePoints != nullptr);
	ocean_assert(rightImagePoints != nullptr);
	ocean_assert(correspondences >= 2u);

	const SquareMatrix3 cameraFreeHomography(rightCamera.invertedIntrinsic() * right_H_left * leftCamera.intrinsic());
	const Matrix notNormalizedHomography(3u, 3u, cameraFreeHomography.transposed().data());

	Matrix u, w, v;
	if (!notNormalizedHomography.singularValueDecomposition(u, w, v))
	{
		return false;
	}

	ocean_assert(w(0) >= w(1) && w(1) >= w(2));

	ocean_assert(Numeric::isNotEqualEps(w(1)));
	SquareMatrix3 normalizedHomography(cameraFreeHomography * (Scalar(1) / w(1)));

	// check whether the sign of the homography has to be flipped
	size_t positives = 0;
	for (size_t n = 0; n < correspondences; ++n)
	{
		const Vector2 normalizedLeft = leftCamera.invertedIntrinsic() * leftImagePoints[n];
		const Vector2 normalizedRight = rightCamera.invertedIntrinsic() * rightImagePoints[n];

		if (Vector3(normalizedRight, 1) * (normalizedHomography * Vector3(normalizedLeft, 1)) > Scalar(0))
		{
			++positives;
		}
	}

	const size_t correspondences_2 = correspondences / 2;

	if (positives < correspondences_2)
	{
		normalizedHomography = -normalizedHomography;
	}


	const SquareMatrix3 hTh(normalizedHomography.transposed() * normalizedHomography);
	const Matrix matrix(3, 3u, hTh.transposed().data());

	//Matrix u, w, v;
	if (!matrix.singularValueDecomposition(u, w, v))
	{
		return false;
	}

	ocean_assert(u.isEqual(v, Numeric::weakEps()));

	const Scalar s1 = w(0);
	const Scalar s2 = w(1);
	const Scalar s3 = w(2);

	ocean_assert(s1 >= s2 && s2 >= s3);
	ocean_assert_and_suppress_unused(Numeric::isWeakEqual(s2, 1), s2);

	const SquareMatrix3 vMatrix(u.transposed().data()); // here we use the fact the v itself is transposed and the SquareMatrix3 has a different data order
	const Vector3 v1(vMatrix.xAxis());
	const Vector3 v2(vMatrix.yAxis());
	const Vector3 v3(vMatrix.zAxis());

	ocean_assert((hTh * v2).isEqual(v2, Numeric::weakEps()));

	const Scalar denominator = Numeric::sqrt(s1 - s3);
	if (Numeric::isEqualEps(denominator))
	{
		return false;
	}

	const Vector3 u1((v1 * Numeric::sqrt(1 - s3) + v3 * Numeric::sqrt(s1 - 1)) / denominator);
	const Vector3 u2((v1 * Numeric::sqrt(1 - s3) - v3 * Numeric::sqrt(s1 - 1)) / denominator);

	const SquareMatrix3 U1(v2, u1, SquareMatrix3::skewSymmetricMatrix(v2) * u1);
	const SquareMatrix3 U2(v2, u2, SquareMatrix3::skewSymmetricMatrix(v2) * u2);

	const SquareMatrix3 W1(normalizedHomography * v2, normalizedHomography * u1, SquareMatrix3::skewSymmetricMatrix(normalizedHomography * v2) * normalizedHomography * u1);
	const SquareMatrix3 W2(normalizedHomography * v2, normalizedHomography * u2, SquareMatrix3::skewSymmetricMatrix(normalizedHomography * v2) * normalizedHomography * u2);

	HomogenousMatrix4 transformationCandidates[4];
	Vector3 normalCandidates[4];

	// solution1:
	const SquareMatrix3 R1(W1 * U1.transposed());
	const Vector3 N1(SquareMatrix3::skewSymmetricMatrix(v2) * u1);
	const Vector3 T1((normalizedHomography - R1) * N1);
	transformationCandidates[0] = (PinholeCamera::flipMatrix4() * HomogenousMatrix4(T1, R1).inverted() * PinholeCamera::flipMatrix4());
	normalCandidates[0] = PinholeCamera::flipMatrix4() * -N1;

	// solution2:
	const SquareMatrix3 R2(W2 * U2.transposed());
	const Vector3 N2(SquareMatrix3::skewSymmetricMatrix(v2) * u2);
	const Vector3 T2 = (normalizedHomography - R2) * N2;
	transformationCandidates[1] = (PinholeCamera::flipMatrix4() * HomogenousMatrix4(T2, R2).inverted() * PinholeCamera::flipMatrix4());
	normalCandidates[1] = PinholeCamera::flipMatrix4() * -N2;

	// solution3:
	const SquareMatrix3 R3 = R1;
	const Vector3 N3 = -N1;
	const Vector3 T3 = -T1;
	transformationCandidates[2] = (PinholeCamera::flipMatrix4() * HomogenousMatrix4(T3, R3).inverted() * PinholeCamera::flipMatrix4());
	normalCandidates[2] = PinholeCamera::flipMatrix4() * -N3;

	// solution4:
	const SquareMatrix3 R4 = R2;
	const Vector3 N4 = -N2;
	const Vector3 T4 = -T2;
	transformationCandidates[3] = (PinholeCamera::flipMatrix4() * HomogenousMatrix4(T4, R4).inverted() * PinholeCamera::flipMatrix4());
	normalCandidates[3] = PinholeCamera::flipMatrix4() * -N4;

#ifdef OCEAN_DEBUG

	const SquareMatrix3 homographies[4] =
	{
		SquareMatrix3(R1 + SquareMatrix3(T1 * N1.x(), T1 * N1.y(), T1 * N1.z())),
		SquareMatrix3(R2 + SquareMatrix3(T2 * N2.x(), T2 * N2.y(), T2 * N2.z())),
		SquareMatrix3(R3 + SquareMatrix3(T3 * N3.x(), T3 * N3.y(), T3 * N3.z())),
		SquareMatrix3(R4 + SquareMatrix3(T4 * N4.x(), T4 * N4.y(), T4 * N4.z()))
	};

	for (unsigned int n = 0u; n < 4u; ++n)
	{
		ocean_assert(Numeric::isNotEqualEps(normalizedHomography[0]));
		const Scalar factor = homographies[n][0] / normalizedHomography[0];

		for (unsigned int i = 1u; i < 9u; ++i)
		{
			ocean_assert((std::is_same<Scalar, float>::value) || Numeric::isEqual(homographies[n][i], normalizedHomography[i] * factor));
		}
	}

#endif // OCEAN_DEBUG

	size_t best[2] = {0, 0};

	for (unsigned int n = 0u; n < 4u; ++n)
	{
		const size_t number = Utilities::countFrontObjectPoints(leftCamera, rightCamera, HomogenousMatrix4(true), transformationCandidates[n], leftImagePoints, rightImagePoints, correspondences);

		if (number > best[0])
		{
			world_T_rightCameras[1] = world_T_rightCameras[0];
			normals[1] = normals[0];
			best[1] = best[0];

			world_T_rightCameras[0] = transformationCandidates[n];
			normals[0] = normalCandidates[n];
			best[0] = number;
		}
		else if (number > best[1])
		{
			world_T_rightCameras[1] = transformationCandidates[n];
			normals[1] = normalCandidates[n];
			best[1] = number;
		}
	}

#ifdef OCEAN_DEBUG

	ocean_assert((std::is_same<Scalar, float>::value) || Numeric::isEqual(normals[0].length(), 1));
	ocean_assert((std::is_same<Scalar, float>::value) || Numeric::isEqual(normals[1].length(), 1));

#endif // OCEAN_DEBUG

	// we check whether we really had at least two solutions
	return best[0] != 0 && best[1] != 0;
}

bool Homography::factorizeHomographyMatrix(const SquareMatrix3& right_H_left, const HomogenousMatrix4& world_T_leftCamera, const PinholeCamera& leftCamera, const PinholeCamera& rightCamera, const ImagePoint* leftImagePoints, const ImagePoint* rightImagePoints, const size_t correspondences, HomogenousMatrix4 world_T_rightCameras[2], Vector3 normals[2])
{
	ocean_assert(world_T_leftCamera.rotationMatrix().isOrthonormal());

	if (!factorizeHomographyMatrix(right_H_left, leftCamera, rightCamera, leftImagePoints, rightImagePoints, correspondences, world_T_rightCameras, normals))
	{
		return false;
	}

	for (unsigned int n = 0u; n < 2u; ++n)
	{
		world_T_rightCameras[n] = world_T_leftCamera * world_T_rightCameras[n];
		normals[n] = world_T_leftCamera.rotationMatrix(normals[n]);
	}

	return true;
}

bool Homography::homographyMatrixPlaneXY(const ObjectPoint* objectPoints, const ImagePoint* imagePoints, const size_t correspondences, SquareMatrix3& homography)
{
	ocean_assert(objectPoints && imagePoints);
	ocean_assert(correspondences >= 10);

	ImagePoints objectPoints2D;
	objectPoints2D.reserve(correspondences);

	for (size_t n = 0; n < correspondences; ++n)
	{
		Numeric::isEqualEps(objectPoints[n].z());
		objectPoints2D.push_back(ImagePoint(objectPoints[n].x(), objectPoints[n].y()));
	}

	return homographyMatrixPlaneXY(objectPoints2D.data(), imagePoints, correspondences, homography);
}

bool Homography::homographyMatrixPlaneXY(const ImagePoint* objectPoints, const ImagePoint* imagePoints, const size_t correspondences, SquareMatrix3& homography)
{
	ocean_assert(objectPoints && imagePoints);
	ocean_assert(correspondences >= 10);

	ImagePoints normalizedObjectPoints(correspondences);
	ImagePoints normalizedImagePoints(correspondences);

	memcpy(normalizedObjectPoints.data(), objectPoints, sizeof(ImagePoint) * correspondences);
	memcpy(normalizedImagePoints.data(), imagePoints, sizeof(ImagePoint) * correspondences);

	const SquareMatrix3 objectPointsNormalization(Normalization::calculateNormalizedPoints(normalizedObjectPoints.data(), correspondences));
	const SquareMatrix3 imagePointsNormalization(Normalization::calculateNormalizedPoints(normalizedImagePoints.data(), correspondences));

	/**
	 * Solution by Zhengyou Zhang, A Flexible New Technique for Camera Calibration.
	 *
	 * Solving equation: Lx = 0.
	 * As L is a 2n * 9 matrix.
	 *
	 * Two successive rows have the following form:
	 * | M^T   0^T   -u * M^T |
	 * | 0^T   M^T   -v * M^T |
	 * with M: (Mx, My, 1) the x and y coordinate of the 3D object point,
	 * and with (u, v) the 2D image point.
	 */

	Matrix matrix(2 * (unsigned int)correspondences, 9);

	for (unsigned int n = 0; n < correspondences; ++n)
	{
		const Vector2& M = normalizedObjectPoints[n];
		const Scalar u = normalizedImagePoints[n].x();
		const Scalar v = normalizedImagePoints[n].y();

		Scalar* row0 = matrix[2 * n + 0];
		Scalar* row1 = matrix[2 * n + 1];

		/**
		 *     M^T          0^T            -u * M^T
		 * Mx, My, 1,    0, 0, 0,     -u * Mx, -u * My, -u
		 */
		row0[0] = M.x();
		row0[1] = M.y();
		row0[2] = 1;

		row0[3] = 0;
		row0[4] = 0;
		row0[5] = 0;

		row0[6] = -u * M.x();
		row0[7] = -u * M.y();
		row0[8] = -u;

		/**
		 * 0^T              M^T               -v * M^T
		 * 0, 0, 0,     Mx, My, 1,     -v * Mx, -v * My, -v
		 */
		row1[0] = 0;
		row1[1] = 0;
		row1[2] = 0;

		row1[3] = M.x();
		row1[4] = M.y();
		row1[5] = 1;

		row1[6] = -v * M.x();
		row1[7] = -v * M.y();
		row1[8] = -v;
	}

	Matrix u, w, v;
	if (!matrix.singularValueDecomposition(u, w, v))
		return false;

	unsigned int lowestSingularValueIndex = (unsigned int)(-1);

	for (unsigned int n = 0; n < w.rows(); ++n)
		if (Numeric::isEqualEps(w(n)))
		{
			lowestSingularValueIndex = n;
			break;
		}

	lowestSingularValueIndex = min(lowestSingularValueIndex, (unsigned int)v.columns() - 1u);

	SquareMatrix3 initialMatrixNormalized(v(0, lowestSingularValueIndex), v(3, lowestSingularValueIndex), v(6, lowestSingularValueIndex),
								v(1, lowestSingularValueIndex), v(4, lowestSingularValueIndex), v(7, lowestSingularValueIndex),
								v(2, lowestSingularValueIndex), v(5, lowestSingularValueIndex), v(8, lowestSingularValueIndex));


#ifdef OCEAN_DEBUG

	for (unsigned int n = 0; n < correspondences; ++n)
	{
		const Vector3 objectPoint(normalizedObjectPoints[n], 1);
		Vector3 imagePoint(initialMatrixNormalized * objectPoint);
		imagePoint /= imagePoint.z();

		const Vector2 deImage(imagePoint.x(), imagePoint.y());

		Numeric::isWeakEqual(deImage.x(), normalizedImagePoints[n].x());
		Numeric::isWeakEqual(deImage.y(), normalizedImagePoints[n].y());
	}

#endif

	homography = imagePointsNormalization.inverted() * initialMatrixNormalized * objectPointsNormalization;

#ifdef OCEAN_DEBUG

	for (unsigned int n = 0; n < correspondences; ++n)
	{
		const Vector3 objectPoint(objectPoints[n], 1);
		Vector3 imagePoint(homography * objectPoint);
		imagePoint /= imagePoint.z();

		const Vector2 deImage(imagePoint.x(), imagePoint.y());

		Numeric::isWeakEqual(deImage.x(), imagePoints[n].x());
		Numeric::isWeakEqual(deImage.y(), imagePoints[n].y());
	}

#endif

	return true;
}

bool Homography::isHomographyPlausible(unsigned int leftImageWidth, unsigned int leftImageHeight, const unsigned int rightImageWidth, const unsigned int rightImageHeight, const SquareMatrix3& homography)
{
	ocean_assert(leftImageWidth >= 1u && leftImageHeight >= 1u);
	ocean_assert(!homography.isSingular());

	SquareMatrix3 invHomography;
	if (!homography.invert(invHomography))
		return false;

	// rightPoint = H * leftPoint
	// leftPoint = (H^-1) * rightPoint

	const Vector2 leftTransformedPoints[4] =
	{
		homography * Vector2(0, 0),
		homography * Vector2(0, Scalar(leftImageHeight)),
		homography * Vector2(Scalar(leftImageWidth), Scalar(leftImageHeight)),
		homography * Vector2(Scalar(leftImageWidth), 0)
	};

	if (!Geometry::Utilities::isPolygonConvex(leftTransformedPoints, 4))
		return false;

	const Vector2 rightTransformedPoints[4] =
	{
		invHomography * Vector2(0, 0),
		invHomography * Vector2(0, Scalar(rightImageHeight)),
		invHomography * Vector2(Scalar(rightImageWidth), Scalar(rightImageHeight)),
		invHomography * Vector2(Scalar(rightImageWidth), 0)
	};

	if (!Utilities::isPolygonConvex(rightTransformedPoints, 4))
		return false;

	return true;
}

bool Homography::intrinsicMatrix(const SquareMatrix3* homographies, const size_t number, SquareMatrix3& intrinsic)
{
	ocean_assert(homographies != nullptr && number >= 3);

	if (number < 3)
	{
		return false;
	}

	/**
	 * Solution by Zhengyou Zhang, A Flexible New Technique for Camera Calibration.
	 */

	MatrixD matrix(2 * number, 6);

	for (size_t n = 0; n < number; ++n)
	{
		const SquareMatrix3& homography = homographies[n];

		const double h11 = double(homography(0, 0));
		const double h12 = double(homography(1, 0));
		const double h13 = double(homography(2, 0));
		const double h21 = double(homography(0, 1));
		const double h22 = double(homography(1, 1));
		const double h23 = double(homography(2, 1));

		/**
		 * v12:
		 * h11 * h21
		 * h11 * h22 + h12 * h21
		 * h12 * h22
		 * h13 * h21 + h11 * h23
		 * h13 * h22 + h12 * h23
		 * h13 * h23
		 */
		double* row = matrix[2 * n];
		row[0] = h11 * h21;
		row[1] = h11 * h22 + h12 * h21;
		row[2] = h12 * h22;
		row[3] = h13 * h21 + h11 * h23;
		row[4] = h13 * h22 + h12 * h23;
		row[5] = h13 * h23;

		/**
		 * v11:
		 * h11 * h11
		 * h11 * h12 + h12 * h11
		 * h12 * h12
		 * h13 * h11 + h11 * h13
		 * h13 * h12 + h12 * h13
		 * h13 * h13
		 *
		 * v22:
		 * h21 * h21
		 * h21 * h22 + h22 * h21
		 * h22 * h22
		 * h23 * h21 + h21 * h23
		 * h23 * h22 + h22 * h23
		 * h23 * h23
		 */
		row = matrix[2 * n + 1];
		row[0] = (h11 * h11) - (h21 * h21);
		row[1] = (h11 * h12 + h12 * h11) - (h21 * h22 + h22 * h21);
		row[2] = (h12 * h12) - (h22 * h22);
		row[3] = (h13 * h11 + h11 * h13) - (h23 * h21 + h21 * h23);
		row[4] = (h13 * h12 + h12 * h13) - (h23 * h22 + h22 * h23);
		row[5] = (h13 * h13) - (h23 * h23);
	}

	MatrixD u, w, v;
	if (!matrix.singularValueDecomposition(u, w, v))
	{
		return false;
	}

	unsigned int lowestSingularValueIndex = (unsigned int)(-1);

	for (size_t n = 0; n < w.rows(); ++n)
	{
		const double singularValue = w(n);

		if (NumericD::isEqualEps(singularValue))
		{
			lowestSingularValueIndex = (unsigned int)(n);
			break;
		}
	}

	lowestSingularValueIndex = min(lowestSingularValueIndex, (unsigned int)v.columns() - 1u);

	const double b11 = v(0, lowestSingularValueIndex);
	const double b12 = v(1, lowestSingularValueIndex);
	const double b22 = v(2, lowestSingularValueIndex);
	const double b13 = v(3, lowestSingularValueIndex);
	const double b23 = v(4, lowestSingularValueIndex);
	const double b33 = v(5, lowestSingularValueIndex);

	const double denominator1 = b11 * b22 - b12 * b12;
	if (NumericD::isEqualEps(denominator1))
	{
		return false;
	}

	const double my = (b12 * b13 - b11 * b23) / denominator1;

	const double denominator2 = b12 * b13 - b11 * b23;
	if (NumericD::isEqualEps(denominator2) || NumericD::isEqualEps(b11))
	{
		return false;
	}

	const double scale = b33 - (b13 * b13 + my * denominator2) / b11;
	if (NumericD::isEqualEps(scale))
	{
		return false;
	}

	const double fxSqrt = scale / b11;
	const double fySqrt = scale * b11 / denominator1;

	if (fxSqrt < 0.0 || fySqrt < 0.0)
	{
		return false;
	}

	const double fx = NumericD::sqrt(fxSqrt);
	const double fy = NumericD::sqrt(fySqrt);

	const double s = -b12 * fx * fx * fy / scale;
	const double mx = s * my / fy - b13 * fx * fx / scale;

	intrinsic = SquareMatrix3(SquareMatrixD3(fx, 0.0, 0.0, s, fy, 0.0, mx, my, 1.0));
	return true;
}

bool Homography::extrinsicMatrix(const SquareMatrix3& intrinsic, const SquareMatrix3& homography, HomogenousMatrix4& world_T_camera)
{
	SquareMatrixD3 invIntrinsic(intrinsic);

	if (!invIntrinsic.invert())
	{
		return false;
	}

	VectorD3 xAxis(homography.xAxis());
	VectorD3 yAxis(homography.yAxis());

	const double xLength = (invIntrinsic * xAxis).length();
	const double yLength = (invIntrinsic * yAxis).length();

	if (NumericD::isEqualEps(xLength) || NumericD::isEqualEps(yLength))
	{
		return false;
	}

	VectorD3 zAxis(homography.zAxis());

	const double scale = (0.5 / xLength) + (0.5 / yLength);

	const VectorD3 translation(invIntrinsic * zAxis * scale);

	xAxis = invIntrinsic * xAxis * scale;
	yAxis = invIntrinsic * yAxis * scale;
	zAxis = xAxis.cross(yAxis);

	MatrixD matrix(3, 3);
	matrix(0, 0) = xAxis(0);
	matrix(1, 0) = xAxis(1);
	matrix(2, 0) = xAxis(2);
	matrix(0, 1) = yAxis(0);
	matrix(1, 1) = yAxis(1);
	matrix(2, 1) = yAxis(2);
	matrix(0, 2) = zAxis(0);
	matrix(1, 2) = zAxis(1);
	matrix(2, 2) = zAxis(2);

	MatrixD u, w, v;
	if (!matrix.singularValueDecomposition(u, w, v))
	{
		return false;
	}

	const MatrixD rotationMatrix = u * v.transposed();
	ocean_assert(rotationMatrix.rows() == 3 && rotationMatrix.columns() == 3);

	const SquareMatrixD3 rotation(rotationMatrix.transposed().data());
	ocean_assert(NumericD::isEqual(rotation.determinant(), 1.0));

	const HomogenousMatrixD4 flippedCamera_T_world(translation, rotation);

	world_T_camera = HomogenousMatrix4(AnyCameraD::invertedFlipped2Standard(flippedCamera_T_world));

	return true;
}

bool Homography::distortionParameters(const ConstIndexedAccessor<HomogenousMatrix4>& extrinsics, const SquareMatrix3& intrinsic, const ConstIndexedAccessor<Vectors3>& objectPointGroups, const ConstIndexedAccessor<Vectors2>& imagePointGroups, Scalar& distortion2, Scalar& distortion4)
{
	ocean_assert(extrinsics.size() == objectPointGroups.size() && extrinsics.size() == imagePointGroups.size());
	if (extrinsics.size() != objectPointGroups.size() || extrinsics.size() != imagePointGroups.size())
		return false;

	const Scalar principalPointX = intrinsic(0, 2);
	const Scalar principalPointY = intrinsic(1, 2);
	const SquareMatrix3 invIntrinsic(intrinsic.inverted());

	size_t totalPoints = 0;
	for (size_t n = 0; n < objectPointGroups.size(); ++n)
		totalPoints += objectPointGroups[n].size();

	Matrix matrix(2 * totalPoints, 2);
	Matrix result(2 * totalPoints, 1);

	size_t points = 0;
	for (size_t n = 0; n < extrinsics.size(); ++n)
	{
		const HomogenousMatrix4& wTc = extrinsics[n];
		const HomogenousMatrix4 wTfc(wTc * PinholeCamera::flipMatrix4());
		const HomogenousMatrix4 fcTw(wTfc.inverted());

		const ObjectPoints& oPoints = objectPointGroups[n];
		const ImagePoints& iPoints = imagePointGroups[n];

		ocean_assert(oPoints.size() == iPoints.size());
		if (oPoints.size() != iPoints.size())
			return false;

		const SquareMatrix3 combinedRotation(intrinsic * fcTw.rotationMatrix());
		const Vector3 combinedTranslation(intrinsic * fcTw.translation());

		for (size_t i = 0; i < oPoints.size(); ++i)
		{
			const Vector3& objectPoint = oPoints[i];
			const Vector2& distortedImagePoint = iPoints[i];

			const Vector3 homogenousImagePoint(combinedRotation * objectPoint + combinedTranslation);
			ocean_assert(Numeric::isNotEqualEps(homogenousImagePoint.z()));
			const Scalar factor = 1 / homogenousImagePoint.z();

			const Vector2 undistortedImagePoint(homogenousImagePoint.x() * factor, homogenousImagePoint.y() * factor);

			const Vector2 nUndistortedImagePoint(invIntrinsic[0] * undistortedImagePoint.x() + invIntrinsic[6],
													invIntrinsic[4] * undistortedImagePoint.y() + invIntrinsic[7]);

			const Scalar nDistance = nUndistortedImagePoint.sqr();
			const Scalar nDistance2 = Numeric::sqr(nDistance);

			Scalar* matrixRow0 = matrix[points * 2 + 0];
			Scalar* matrixRow1 = matrix[points * 2 + 1];

			matrixRow0[0] = (undistortedImagePoint.x() - principalPointX) * nDistance;
			matrixRow0[1] = (undistortedImagePoint.x() - principalPointX) * nDistance2;

			matrixRow1[0] = (undistortedImagePoint.y() - principalPointY) * nDistance;
			matrixRow1[1] = (undistortedImagePoint.y() - principalPointY) * nDistance2;

			result[points * 2 + 0][0] = distortedImagePoint.x() - undistortedImagePoint.x();
			result[points * 2 + 1][0] = distortedImagePoint.y() - undistortedImagePoint.y();
			++points;
		}
	}

	const Matrix matrixT(matrix.transposed());
	Matrix innerMatrix(matrixT * matrix);

	if (innerMatrix.invert() == false)
	{
		return false;
	}

	const Matrix k = (innerMatrix * matrixT) * result;
	ocean_assert(k.rows() == 2 && k.columns() == 1);

	distortion2 = k(0);
	distortion4 = k(1);

	return true;
}

SquareMatrix3 Homography::toFinestHomography(const SquareMatrix3& coarseHomography, const unsigned int sourceLayer)
{
	ocean_assert(!coarseHomography.isSingular());

	if (sourceLayer == 0u)
	{
		return coarseHomography;
	}
	else
	{
		// BHA = BTb * bHa * aTA
		// a: coarse source coordinate system, A fine source coordinate system
		// b: coarse target coordinate system, B fine target coordinate system

		ocean_assert(sourceLayer < 32u);
		const unsigned int layerFactor = 1u << sourceLayer;
		ocean_assert(layerFactor >= 2u);

		const Scalar invLayerFactor = Scalar(1) / Scalar(layerFactor);

		SquareMatrix3 adjustedHomography(coarseHomography);
		adjustedHomography[2] *= Scalar(invLayerFactor);
		adjustedHomography[5] *= Scalar(invLayerFactor);
		adjustedHomography[6] *= Scalar(layerFactor);
		adjustedHomography[7] *= Scalar(layerFactor);

#ifdef OCEAN_DEBUG
		const SquareMatrix3 upsampleTransformation(Vector3(Scalar(layerFactor), 0, 0), Vector3(0, Scalar(layerFactor), 0), Vector3(0, 0, 1)); // BTb
		const SquareMatrix3 downsampleTransformation(Vector3(invLayerFactor, 0, 0), Vector3(0, invLayerFactor, 0), Vector3(0, 0, 1)); // aTA
		ocean_assert(upsampleTransformation.inverted() == downsampleTransformation);

		ocean_assert(adjustedHomography.isEqual(upsampleTransformation * coarseHomography * downsampleTransformation, Numeric::weakEps()));
#endif

		return adjustedHomography;
	}
}

SquareMatrix3 Homography::toCoarseHomography(const SquareMatrix3& finestHomography, const unsigned int targetLayer)
{
	ocean_assert(!finestHomography.isSingular());

	if (targetLayer == 0u)
	{
		return finestHomography;
	}
	else
	{
		// bHa = bTB * BHA * ATa
		// a: coarse source coordinate system, A fine source coordinate system
		// b: coarse target coordinate system, B fine target coordinate system

		ocean_assert(targetLayer < 32u);
		const unsigned int layerFactor = 1u << targetLayer;
		ocean_assert(layerFactor >= 2u);

		const Scalar invLayerFactor = Scalar(1) / Scalar(layerFactor);

		SquareMatrix3 adjustedHomography(finestHomography);
		adjustedHomography[2] *= Scalar(layerFactor);
		adjustedHomography[5] *= Scalar(layerFactor);
		adjustedHomography[6] *= Scalar(invLayerFactor);
		adjustedHomography[7] *= Scalar(invLayerFactor);

#ifdef OCEAN_DEBUG
		const SquareMatrix3 upsampleTransformation(Vector3(Scalar(layerFactor), 0, 0), Vector3(0, Scalar(layerFactor), 0), Vector3(0, 0, 1)); // ATa
		const SquareMatrix3 downsampleTransformation(Vector3(invLayerFactor, 0, 0), Vector3(0, invLayerFactor, 0), Vector3(0, 0, 1)); // bTB
		ocean_assert(upsampleTransformation.inverted() == downsampleTransformation);

		ocean_assert(adjustedHomography == downsampleTransformation * finestHomography * upsampleTransformation);
#endif

		return adjustedHomography;
	}
}

}

}
