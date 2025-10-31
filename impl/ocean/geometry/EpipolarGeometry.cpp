/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/geometry/EpipolarGeometry.h"
#include "ocean/geometry/Normalization.h"

#include "ocean/math/Line2.h"
#include "ocean/math/Matrix.h"
#include "ocean/math/Rotation.h"
#include "ocean/math/SquareMatrix3.h"

namespace Ocean
{

namespace Geometry
{

bool EpipolarGeometry::fundamentalMatrix(const Vector2* leftPoints, const Vector2* rightPoints, const size_t correspondences, SquareMatrix3& fundamental)
{
	ocean_assert(leftPoints != nullptr && rightPoints != nullptr);
	ocean_assert(correspondences >= 8);

	if (correspondences < 8)
	{
		return false;
	}

	/**
	 * 8-Point algorithm:
	 *
	 * pr F pl = 0
	 *
	 *     | f11 f12 f13 |        | xl |        | xr |
	 * F = | f21 f22 f23 |,  pl = | yl |,  pr = | yr |
	 *     | f31 f32 f33 |        | 1  |        | 1  |
	 *
	 *               | f11 f12 f13 |   | xl |
	 * | xr yr 1 | * | f21 f22 f23 | * | yl | = 0
	 *               | f31 f32 f33 |   | 1  |
	 *
	 *               | f11 xl + f12 yl + f13 |
	 * | xr yr 1 | * | f21 xl + f22 yl + f23 | = 0
	 *               | f31 xl + f32 yl + f33 |
	 *
	 * (f11xl + f12yl + f13)xr + (f21xl + f22yl + f23)yr + (f31xl + f32yl + f33) = 0
	 *
	 * f11xlxr + f12ylxr + f13xr + f21xlyr + f22ylyr + f23yr + f31xl + f32yl + f33 = 0
	 */

	/**
	 * Points normalization:
	 */

	Vectors2 normalizationLeftPoints(correspondences);
	Vectors2 normalizationRightPoints(correspondences);

	memcpy(normalizationLeftPoints.data(), leftPoints, sizeof(Vector2) * correspondences);
	memcpy(normalizationRightPoints.data(), rightPoints, sizeof(Vector2) * correspondences);

	const SquareMatrix3 normalizationLeft = Normalization::calculateNormalizedPoints(normalizationLeftPoints.data(), correspondences);
	const SquareMatrix3 normalizationRight = Normalization::calculateNormalizedPoints(normalizationRightPoints.data(), correspondences);


	Matrix matrix(correspondences, 9);
	for (size_t row = 0; row < correspondences; row++)
	{
		const Vector2& l = normalizationLeftPoints[row];
		const Vector2& r = normalizationRightPoints[row];

		matrix(row, 0) = l[0] * r[0];
		matrix(row, 1) = l[1] * r[0];
		matrix(row, 2) = r[0];
		matrix(row, 3) = l[0] * r[1];
		matrix(row, 4) = l[1] * r[1];
		matrix(row, 5) = r[1];
		matrix(row, 6) = l[0];
		matrix(row, 7) = l[1];
		matrix(row, 8) = 1.0;
	}

	Matrix u_, w_, v_;
	Matrix normalizedFundamental(3, 3);
	if (!matrix.singularValueDecomposition(u_, w_, v_))
	{
		return false;
	}

#ifdef OCEAN_DEBUG
	for (unsigned int n = 1; n < w_.rows(); ++n)
	{
		ocean_assert(w_(n - 1) >= w_(n));
	}
#endif // OCEAN_DEBUG

	ocean_assert(v_.rows() == 9);

	unsigned int eigenVectorRow = 0u;
	for (size_t r = 0; r < 3; r++)
	{
		for (size_t c = 0; c < 3; c++)
		{
			normalizedFundamental(r, c) = v_(eigenVectorRow++, 8);
		}
	}

	Matrix u, w, v;
	if (!normalizedFundamental.singularValueDecomposition(u, w, v))
	{
		return false;
	}

	ocean_assert(w.rows() == 3 && w.columns() == 1);
	ocean_assert(w(0, 0) >= w(1, 0) && w(1, 0) >= w(2, 0));
	w(2, 0) = 0;

	const Matrix normalizedFundamentalRankTwo(u * Matrix(3, 3, w) * v.transposed());
	fundamental = normalizationRight.transposed() * SquareMatrix3(normalizedFundamentalRankTwo.data()).transposed() * normalizationLeft;

	return true;
}

SquareMatrix3 EpipolarGeometry::essentialMatrix(const HomogenousMatrix4& rightCamera_T_leftCamera)
{
	ocean_assert(rightCamera_T_leftCamera.isValid());

	const HomogenousMatrix4 rightFlippedCamera_T_leftFlippedCamera = Camera::flippedTransformationLeftAndRightSide(rightCamera_T_leftCamera);

	const HomogenousMatrix4 leftFlippedCamera_T_rightFlippedCamera = rightFlippedCamera_T_leftFlippedCamera.inverted();

	return SquareMatrix3::skewSymmetricMatrix(leftFlippedCamera_T_rightFlippedCamera.translation()) * leftFlippedCamera_T_rightFlippedCamera.rotationMatrix();
}

SquareMatrix3 EpipolarGeometry::essential2fundamental(const SquareMatrix3& normalizedRight_E_normalizedLeft, const SquareMatrix3& leftIntrinsic, const SquareMatrix3& rightIntrinsic)
{
	ocean_assert(!leftIntrinsic.isSingular());
	ocean_assert(!rightIntrinsic.isSingular());

	return rightIntrinsic.inverted().transposed() * (normalizedRight_E_normalizedLeft * leftIntrinsic.inverted());
}

SquareMatrix3 EpipolarGeometry::essential2fundamental(const SquareMatrix3& normalizedRight_E_normalizedLeft, const PinholeCamera& leftCamera, const PinholeCamera& rightCamera)
{
	ocean_assert(leftCamera.isValid());
	ocean_assert(!leftCamera.hasDistortionParameters());

	ocean_assert(rightCamera.isValid());
	ocean_assert(!rightCamera.hasDistortionParameters());

	return rightCamera.intrinsic().inverted().transposed() * (normalizedRight_E_normalizedLeft * leftCamera.intrinsic().inverted());
}

SquareMatrix3 EpipolarGeometry::fundamental2essential(const SquareMatrix3& right_F_left, const SquareMatrix3& leftIntrinsic, const SquareMatrix3& rightIntrinsic)
{
	ocean_assert(!leftIntrinsic.isSingular());
	ocean_assert(!rightIntrinsic.isSingular());

	return rightIntrinsic.transposed() * (right_F_left * leftIntrinsic);
}

SquareMatrix3 EpipolarGeometry::fundamental2essential(const SquareMatrix3& right_F_left, const PinholeCamera& leftCamera, const PinholeCamera& rightCamera)
{
	ocean_assert(leftCamera.isValid());
	ocean_assert(!leftCamera.hasDistortionParameters());

	ocean_assert(rightCamera.isValid());
	ocean_assert(!rightCamera.hasDistortionParameters());

	return rightCamera.intrinsic().transposed() * (right_F_left * leftCamera.intrinsic());
}

bool EpipolarGeometry::epipoles(const SquareMatrix3& fundamental, Vector2& leftEpipole, Vector2& rightEpipole)
{
	const Matrix f(3, 3, fundamental.transposed().data());

	Matrix u, w, v;
	if (!f.singularValueDecomposition(u, w, v))
	{
		return false;
	}

	ocean_assert(Numeric::isWeakEqualEps(w(2, 0)));

	Scalar scaleLeft = v(2, 2);
	Scalar scaleRight = u(2, 2);

	if (Numeric::isEqualEps(scaleLeft) || Numeric::isEqualEps(scaleRight))
	{
		return false;
	}

	scaleLeft = Scalar(1) / scaleLeft;
	scaleRight = Scalar(1) / scaleRight;

	leftEpipole(0) = v(0, 2) * scaleLeft;
	leftEpipole(1) = v(1, 2) * scaleLeft;

	rightEpipole(0) = u(0, 2) * scaleRight;
	rightEpipole(1) = u(1, 2) * scaleRight;

#ifdef OCEAN_DEBUG

	const Vector3 left(leftEpipole, 1);
	const Vector3 testLeft(fundamental * left);

	const Vector3 right(rightEpipole, 1);
	const Vector3 testRight(fundamental.transposed() * right);

	if constexpr (std::is_same<double, Scalar>::value)
	{
		ocean_assert(Numeric::isWeakEqualEps(testLeft.length()));
		ocean_assert(Numeric::isWeakEqualEps(testRight.length()));
	}

#endif // OCEAN_DEBUG

	return true;
}

bool EpipolarGeometry::epipoles(const HomogenousMatrix4& extrinsic, const SquareMatrix3& leftIntrinsic, const SquareMatrix3& rightIntrinsic, Vector2& leftEpipole, Vector2& rightEpipole)
{
	/**
	 * the left epipole is the projection of the center-of-projection of the right camera onto the left camera image.
	 * the right epipole is the projection of the center-of-projection of the left camera onto the right camera image.
	 *
	 * the given extrinsic camera matrix defined the transformation of points defined inside the right camera coordinate system into the left camera coordinate system (leftTright)
	 * epipoles are defined for a camera pointing into the positive z-direction, thus the extrinsic camera must be flipped before!
	 */

	const HomogenousMatrix4 flippedExtrinsic(Camera::flipMatrix4() * extrinsic * Camera::flipMatrix4());

	const Vector3 hLeftEpipole(leftIntrinsic * flippedExtrinsic.translation());
	const Vector3 hRightEpipole(rightIntrinsic * flippedExtrinsic.inverted().translation());

	if (Numeric::isEqualEps(hLeftEpipole.z()) || Numeric::isEqualEps(hRightEpipole.z()))
	{
		return false;
	}

	const Scalar leftNormalization = 1 / hLeftEpipole.z();
	const Scalar rightNormalization = 1 / hRightEpipole.z();

	leftEpipole = Vector2(hLeftEpipole.x() * leftNormalization, hLeftEpipole.y() * leftNormalization);
	rightEpipole = Vector2(hRightEpipole.x() * rightNormalization, hRightEpipole.y() * rightNormalization);

	return true;
}

bool EpipolarGeometry::epipolesFast(const SquareMatrix3& fundamental, Vector2& leftEpipole, Vector2& rightEpipole)
{
	Vector2 point1(0, 0), point2(Scalar(100), Scalar(200));

	Line2 line1(rightEpipolarLine(fundamental, point1));
	Line2 line2(rightEpipolarLine(fundamental, point2));

	if (!line1.intersection(line2, rightEpipole))
	{
		return epipoles(fundamental, leftEpipole, rightEpipole);
	}

	const SquareMatrix3 inverseFundamental(fundamental.transposed());

	line1 = rightEpipolarLine(inverseFundamental, point1);
	line2 = rightEpipolarLine(inverseFundamental, point2);

	if (!line1.intersection(line2, leftEpipole))
	{
		return epipoles(fundamental, leftEpipole, rightEpipole);
	}

	return true;
}

size_t EpipolarGeometry::factorizeEssential(const SquareMatrix3& normalizedRight_E_normalizedLeft, const PinholeCamera& leftCamera, const PinholeCamera& rightCamera, const Vector2* leftPoints, const Vector2* rightPoints, const size_t correspondences, HomogenousMatrix4& left_T_right)
{
	ocean_assert(leftCamera.isValid());
	ocean_assert(rightCamera.isValid());
	ocean_assert(leftPoints != nullptr && rightPoints != nullptr);
	ocean_assert(correspondences >= 1);

	Matrix essentialMatrix(3, 3, normalizedRight_E_normalizedLeft.data(), false /*valuesRowAligned*/);

	Matrix uMatrix, wValues, vMatrix;
	if (!essentialMatrix.singularValueDecomposition(uMatrix, wValues, vMatrix))
	{
		return 0;
	}

#ifdef OCEAN_DEBUG
	const Matrix debugTest = uMatrix * Matrix(3, 3, wValues) * vMatrix.transposed();

	for (size_t n = 0; n < essentialMatrix.elements(); ++n)
	{
		ocean_assert(Numeric::isWeakEqual(essentialMatrix(n), debugTest(n)));
	}
#endif

	const Scalar singularValue0 = wValues(0);
	const Scalar singularValue1 = wValues(1);
	const Scalar singularValue2 = wValues(2);
	ocean_assert_and_suppress_unused(singularValue2 <= singularValue1 && Numeric::isWeakEqualEps(singularValue2), singularValue2);

	if (Numeric::isNotWeakEqual(singularValue0, singularValue1))
	{
		Matrix newSingularValues(3, 3, false);

		const Scalar averageSingularValue = (singularValue0 + singularValue1) * Scalar(0.5);

		newSingularValues(0, 0) = averageSingularValue;
		newSingularValues(1, 1) = averageSingularValue;
		newSingularValues(2, 2) = 0;

		essentialMatrix = uMatrix * newSingularValues * vMatrix.transposed();

		if (!essentialMatrix.singularValueDecomposition(uMatrix, wValues, vMatrix))
		{
			return 0;
		}

#ifdef OCEAN_DEBUG
		const Scalar debugSingularValue0 = wValues(0);
		const Scalar debugSingularValue1 = wValues(1);
		const Scalar debugSingularValue2 = wValues(2);
		OCEAN_SUPPRESS_UNUSED_WARNING(debugSingularValue2);

		ocean_assert(Numeric::isWeakEqual(debugSingularValue0, debugSingularValue1));
#endif
	}

	/**
	 * S matrix:
	 * |  0  1  0 |
	 * | -1  0  0 |
	 * |  0  0  1 |
	 */
	const SquareMatrix3 s(0, -1, 0, 1, 0, 0, 0, 0, 1);

	/**
	 * Four possible solutions:
	 * rotation      | translation
	 * u * s * v^T   | u(0, 0, 1)^T
	 * u * s * v^T   | -u(0, 0, 1)^T
	 * u * s^T * v^T | u(0, 0, 1)^T
	 * u * s^T * v^T | -u(0, 0, 1)^T
	 */

	const SquareMatrix3 uTransposed(uMatrix.data());
	const SquareMatrix3 u(uTransposed.transposed());
	const SquareMatrix3 vTransposed(vMatrix.data());

	SquareMatrix3 rightRotation0(u * s * vTransposed);
	SquareMatrix3 rightRotation1(u * s.transposed() * vTransposed);

	ocean_assert(Numeric::isEqual(Numeric::abs(rightRotation0.determinant()), Scalar(1)));
	ocean_assert(Numeric::isEqual(Numeric::abs(rightRotation1.determinant()), Scalar(1)));

	Vector3 rightTranslation0(u.zAxis());
	Vector3 rightTranslation1(-rightTranslation0);

	if (rightRotation0.determinant() < 0)
	{
		ocean_assert(rightRotation1.determinant() < 0);

		rightRotation0 *= Scalar(-1);
		rightRotation1 *= Scalar(-1);
	}
	else
	{
		ocean_assert(rightRotation1.determinant() > 0);
	}

	HomogenousMatrix4 bestLeft_T_bestRight(false);
	size_t bestNumberCorrespondences = 0;

	for (unsigned int nCombination = 0u; nCombination < 4u; ++nCombination)
	{
		const unsigned int translationIndex = nCombination % 2u;
		const unsigned int rotationIndex = nCombination / 2u;

		const Vector3& combinationTranslation = translationIndex == 0u ? rightTranslation0 : rightTranslation1;
		const SquareMatrix3& combinationRotation = rotationIndex == 0u ? rightRotation0 : rightRotation1;

		const HomogenousMatrix4 candidateFlippedRight_T_candidateFlippedLeft(combinationTranslation, combinationRotation);

		const HomogenousMatrix4 candidateRight_T_candidateLeft(Camera::flippedTransformationLeftAndRightSide(candidateFlippedRight_T_candidateFlippedLeft));

		const HomogenousMatrix4 candidateLeft_T_candidateRight(candidateRight_T_candidateLeft.inverted());

		const size_t numberCorrespondences = validateCameraPose(candidateLeft_T_candidateRight, leftCamera, rightCamera, leftPoints, rightPoints, correspondences);

		if (numberCorrespondences > bestNumberCorrespondences)
		{
			bestNumberCorrespondences = numberCorrespondences;
			bestLeft_T_bestRight= candidateLeft_T_candidateRight;
		}
	}

	left_T_right = bestLeft_T_bestRight;

	return bestNumberCorrespondences;
}

bool EpipolarGeometry::rectificationHomography(const HomogenousMatrix4& transformation, const PinholeCamera& pinholeCamera, SquareMatrix3& leftHomography, SquareMatrix3& rightHomography, Quaternion& appliedRotation, PinholeCamera* newCamera)
{
	ocean_assert(transformation.isValid());

	/**
	 * the world origin is the projection center of the left camera
	 * the given transformation is the extrinsic camera matrix for the right camera and is defined in relation to the world origin
	 */

	// transformation transforming points defined inside the right camera coordinate system (c) into the world (left camera) coordinate system (w)
	const HomogenousMatrix4& wTc = transformation;

	Vector3 xAxis(wTc.translation());

	if (!xAxis.normalize())
	{
		return false;
	}

	Vector3 yAxis(xAxis.cross(Vector3(0, 0, Scalar(-1))));

	if (!yAxis.normalize())
	{
		return false;
	}

	const Vector3 zAxis(xAxis.cross(yAxis));
	ocean_assert(Numeric::isEqual(zAxis.length(), 1));

	// transformation transforming 3D points defined in the rectified world coordinate system (rw) into the world coordinate system (w)
	const SquareMatrix3 wTrw(xAxis, yAxis, zAxis);
	ocean_assert(Numeric::isEqual(wTrw.determinant(), 1));

	appliedRotation = Quaternion(wTrw);

	// transformation transforming 3D points defined in the flipped rectified world coordinate system (frw) into the flipped world coordinate system (fw)
	const SquareMatrix3 fwTfrw(Camera::flipMatrix3() * wTrw * Camera::flipMatrix3());


	// transformation rotating points defined inside the world (left camera) coordinate system (w) into the right camera coordinate system (c)
	const SquareMatrix3 cTw = wTc.rotationMatrix().inverted();

	// transformation transforming 3D points defined in the rectified right camera coordinate system (rc) into the right camera coordinate system (c)
	const SquareMatrix3 cTrc(cTw * wTrw);

	// transformation transforming 3D points defined in the flipped rectified right camera coordinate system (frc) into the flipped right camera coordinate system (fc)
	const SquareMatrix3 fcTfrc(Camera::flipMatrix3() * cTrc * Camera::flipMatrix3());


	const PinholeCamera* reprojectionCamera = nullptr;


	// if the reprojection is expected to be adjusted to fit to the rectified images
	if (newCamera)
	{
		Scalar newFovX = pinholeCamera.fovX();

		{
			const SquareMatrix3 frwTfw(fwTfrw.transposed());
			ocean_assert(frwTfw == fwTfrw.inverted());

			const SquareMatrix3 frwpTfwp(frwTfw * pinholeCamera.invertedIntrinsic());

			const Vector3 topLeftUnrectified(0, 0, Scalar(1));
			Vector3 topLeftRectified(frwpTfwp * topLeftUnrectified);
			topLeftRectified /= topLeftRectified.z();

			const Vector3 bottomLeftUnrectified(0, Scalar(pinholeCamera.height()), Scalar(1));
			Vector3 bottomLeftRectified(frwpTfwp * bottomLeftUnrectified);
			bottomLeftRectified /= bottomLeftRectified.z();

			const Vector3 topRightUnrectified(Scalar(pinholeCamera.width()), 0, Scalar(1));
			Vector3 topRightRectified(frwpTfwp * topRightUnrectified);
			topRightRectified /= topRightRectified.z();

			const Vector3 bottomRightUnrectified(Scalar(pinholeCamera.width()), Scalar(pinholeCamera.height()), Scalar(1));
			Vector3 bottomRightRectified(frwpTfwp * bottomRightUnrectified);
			bottomRightRectified /= bottomRightRectified.z();

			const Scalar minX = min(topLeftRectified.x(), bottomLeftRectified.x());
			const Scalar minY = min(topLeftRectified.y(), topRightRectified.y());
			const Scalar maxX = max(topRightRectified.x(), bottomRightRectified.x());
			const Scalar maxY = max(bottomLeftRectified.y(), bottomRightRectified.y());

			const Scalar leftFovX = Numeric::rad2deg(Numeric::atan(minX));
			const Scalar rightFovX = Numeric::rad2deg(Numeric::atan(maxX));
			const Scalar topFovY = Numeric::rad2deg(Numeric::atan(minY));
			const Scalar bottomFovY = Numeric::rad2deg(Numeric::atan(maxY));

			const Scalar fovX = 2 * max(Numeric::abs(leftFovX), Numeric::abs(rightFovX));
			const Scalar fovY = 2 * max(Numeric::abs(topFovY), Numeric::abs(bottomFovY));

			const Scalar fovY2X = Camera::fovY2X(fovY, Scalar(pinholeCamera.width()) / Scalar(pinholeCamera.height()));

			const Scalar finalFovX = max(fovX, fovY2X);
			newFovX = finalFovX;

			const PinholeCamera newLeftCamera(pinholeCamera.width(), pinholeCamera.height(), Numeric::deg2rad(finalFovX));
		}

		{
			const SquareMatrix3 frcTfc(fcTfrc.transposed());
			ocean_assert(frcTfc == fcTfrc.inverted());

			const SquareMatrix3 frcpTfcp(frcTfc * pinholeCamera.invertedIntrinsic());

			const Vector3 topLeftUnrectified(0, 0, Scalar(1));
			Vector3 topLeftRectified(frcpTfcp * topLeftUnrectified);
			topLeftRectified /= topLeftRectified.z();

			const Vector3 bottomLeftUnrectified(0, Scalar(pinholeCamera.height()), Scalar(1));
			Vector3 bottomLeftRectified(frcpTfcp * bottomLeftUnrectified);
			bottomLeftRectified /= bottomLeftRectified.z();

			const Vector3 topRightUnrectified(Scalar(pinholeCamera.width()), 0, Scalar(1));
			Vector3 topRightRectified(frcpTfcp * topRightUnrectified);
			topRightRectified /= topRightRectified.z();

			const Vector3 bottomRightUnrectified(Scalar(pinholeCamera.width()), Scalar(pinholeCamera.height()), Scalar(1));
			Vector3 bottomRightRectified(frcpTfcp * bottomRightUnrectified);
			bottomRightRectified /= bottomRightRectified.z();

			const Scalar minX = min(topLeftRectified.x(), bottomLeftRectified.x());
			const Scalar minY = min(topLeftRectified.y(), topRightRectified.y());
			const Scalar maxX = max(topRightRectified.x(), bottomRightRectified.x());
			const Scalar maxY = max(bottomLeftRectified.y(), bottomRightRectified.y());

			const Scalar leftFovX = Numeric::rad2deg(Numeric::atan(minX));
			const Scalar rightFovX = Numeric::rad2deg(Numeric::atan(maxX));
			const Scalar topFovY = Numeric::rad2deg(Numeric::atan(minY));
			const Scalar bottomFovY = Numeric::rad2deg(Numeric::atan(maxY));

			const Scalar fovX = 2 * max(Numeric::abs(leftFovX), Numeric::abs(rightFovX));
			const Scalar fovY = 2 * max(Numeric::abs(topFovY), Numeric::abs(bottomFovY));

			const Scalar fovY2X = Camera::fovY2X(fovY, Scalar(pinholeCamera.width()) / Scalar(pinholeCamera.height()));

			const Scalar finalFovX = max(fovX, fovY2X);

			newFovX = max(finalFovX, newFovX);
		}

		*newCamera = PinholeCamera(pinholeCamera.width(), pinholeCamera.height(), Numeric::deg2rad(newFovX));
		reprojectionCamera = newCamera;
	}
	else
	{
		reprojectionCamera = &pinholeCamera;
	}


	ocean_assert(reprojectionCamera);

	// transformation including projection and un-projection (fwpTfrwp)
	leftHomography = pinholeCamera.intrinsic() * fwTfrw * reprojectionCamera->invertedIntrinsic();

	// transformation including projection and un-projection (fcpTfrcp)
	rightHomography = pinholeCamera.intrinsic() * fcTfrc * reprojectionCamera->invertedIntrinsic();

	return true;
}

Vectors3 EpipolarGeometry::triangulateImagePoints(const HomogenousMatrix4& world_T_cameraA, const HomogenousMatrix4& world_T_cameraB, const AnyCamera& anyCameraA, const AnyCamera& anyCameraB, const Vector2* imagePointsA, const Vector2* imagePointsB, const size_t numberPoints, const bool onlyFrontObjectPoints, const Vector3& invalidObjectPoint, Indices32* invalidIndices)
{
	ocean_assert(world_T_cameraA.isValid() && world_T_cameraB.isValid());
	ocean_assert(anyCameraA.isValid() && anyCameraB.isValid());

	if (invalidIndices)
	{
		invalidIndices->clear();
	}

	const HomogenousMatrix4 flippedCameraA_T_world = Camera::standard2InvertedFlipped(world_T_cameraA);
	const HomogenousMatrix4 flippedCameraB_T_world = Camera::standard2InvertedFlipped(world_T_cameraB);

	Vectors3 objectPoints;
	objectPoints.reserve(numberPoints);

	for (size_t n = 0; n < numberPoints; ++n)
	{
		const Vector2& imagePointA = imagePointsA[n];
		const Vector2& imagePointB = imagePointsB[n];

		const Line3 rayA(anyCameraA.ray(imagePointA, world_T_cameraA));
		const Line3 rayB(anyCameraB.ray(imagePointB, world_T_cameraB));

		Vector3 intersection;

		if (rayA.nearestPoint(rayB, intersection))
		{
			if (!onlyFrontObjectPoints || (Camera::isObjectPointInFrontIF(flippedCameraA_T_world, intersection) && Camera::isObjectPointInFrontIF(flippedCameraB_T_world, intersection)))
			{
				objectPoints.emplace_back(intersection);

				continue;
			}
		}

		objectPoints.emplace_back(invalidObjectPoint);

		if (invalidIndices)
		{
			invalidIndices->push_back(Index32(n));
		}
	}

	return objectPoints;
}

ObjectPoints EpipolarGeometry::triangulateImagePointsIF(const PinholeCamera& camera1, const HomogenousMatrix4& iFlippedPose1, const PinholeCamera& camera2, const HomogenousMatrix4& iFlippedPose2, const Vector2* points1, const Vector2* points2, const size_t correspondences, const Vector3& invalidObjectPoint, Indices32* invalidIndices)
{
	ocean_assert(iFlippedPose1.isValid() && iFlippedPose2.isValid());
	ocean_assert(points1 && points2);

	/**
	 * algorithm from multiple view geometry (p.312f)
	 * construct a linear system AX=0 expressing:
	 *		x_1 = P1 * X
	 *		x_2 = P2 * X
	 * homogeneous scale factor is eliminated by a cross product:
	 *		x_1 x (P1 * X) = 0
	 * which is equal to
	 *		x * (P1_3i * X) - (P1_1i * X) = 0
	 *		y * (P1_3i * X) - (P1_2i * X) = 0
	 *		x * (P1_2i * X) - y *(P1_1i * X) = 0
	 */

	Matrix u, w, v;
	Matrix matrixA(4, 4);

	const HomogenousMatrix4 p1 = camera1.transformationMatrixIF(iFlippedPose1);
	const HomogenousMatrix4 p2 = camera2.transformationMatrixIF(iFlippedPose2);

	ObjectPoints objectPoints;
	objectPoints.reserve(correspondences);

	for (size_t c = 0; c < correspondences; ++c)
	{
		const Vector2& point1 = points1[c];
		const Vector2& point2 = points2[c];

		// construct linear system AX=0
		for (unsigned int i = 0u; i < 4u; ++i)
		{
			matrixA(0, size_t(i)) = point1.x() * p1(2, i) - p1(0, i);
			matrixA(1, size_t(i)) = point1.y() * p1(2, i) - p1(1, i);
			matrixA(2, size_t(i)) = point2.x() * p2(2, i) - p2(0, i);
			matrixA(3, size_t(i)) = point2.y() * p2(2, i) - p2(1, i);
		}

		if (matrixA.singularValueDecomposition(u, w, v) && Numeric::isNotEqualEps(v(3, 3)))
		{
			// a unit length solution lies in null-space (last column of v)
			objectPoints.push_back(Vector3(v(0, 3), v(1, 3), v(2, 3)) / v(3, 3));
		}
		else
		{
			objectPoints.push_back(invalidObjectPoint);

			if (invalidIndices)
			{
				invalidIndices->push_back(Index32(c));
			}
		}
	}

	ocean_assert(objectPoints.size() == correspondences);
	return objectPoints;
}

ObjectPoints EpipolarGeometry::triangulateImagePointsIF(const ConstIndexedAccessor<HomogenousMatrix4>& posesIF, const ConstIndexedAccessor<Vectors2>& imagePointsPerPose, const PinholeCamera* pinholeCamera, const Vector3& invalidObjectPoint, Indices32* invalidIndices)
{
	/**
	 * algorithm from multiple view geometry (p.312f)
	 * construct a linear system AX=0 expressing:
	 *		x_1 = P1 * X
	 *		x_n = Pn * X
	 * homogeneous scale factor is eliminated by a cross product:
	 *		x_1 x (P1 * X) = 0
	 * which is equal to
	 *		x * (P1_3i * X) - (P1_1i * X) = 0
	 *		y * (P1_3i * X) - (P1_2i * X) = 0
	 *		x * (P1_2i * X) - y *(P1_1i * X) = 0 (linear dependent)
	 */

	ocean_assert(posesIF.size() == imagePointsPerPose.size() && posesIF.size() > 0);
	ocean_assert(invalidIndices == nullptr || invalidIndices->empty());

	if (invalidIndices)
	{
		invalidIndices->clear();
	}

	const ScopedConstMemoryAccessor<HomogenousMatrix4> scopedPosesIF(posesIF);
	const ScopedConstMemoryAccessor<Vectors2> scopedImagePointsPerPose(imagePointsPerPose);

	const size_t noPoses = scopedPosesIF.size();
	const size_t correspondences = imagePointsPerPose[0].size();

	HomogenousMatrices4 transformationMatricesIF;
	transformationMatricesIF.reserve(noPoses);

	if (pinholeCamera)
	{
		for (size_t i = 0; i < noPoses; ++i)
		{
			transformationMatricesIF.push_back(pinholeCamera->transformationMatrixIF(scopedPosesIF[i]));
		}
	}

	ObjectPoints objectPoints;
	objectPoints.reserve(correspondences);

	Matrix u, w, v;
	Matrix matrixA(2 * noPoses, 4);

	for (size_t c = 0; c < correspondences; ++c)
	{
		// construct linear system AX=0
		for (size_t iPose = 0; iPose < noPoses; ++iPose)
		{
			const HomogenousMatrix4& transformation = pinholeCamera ? transformationMatricesIF[iPose] : scopedPosesIF[iPose];
			const Vector2& point = scopedImagePointsPerPose[iPose][c];

			for (unsigned int i = 0u; i < 4u; ++i)
			{
				matrixA(2 * iPose, size_t(i)) = point.x() * transformation(2, i) - transformation(0, i);
				matrixA(2 * iPose + 1, size_t(i)) = point.y() * transformation(2, i) - transformation(1, i);
			}
		}

		if (matrixA.singularValueDecomposition(u, w, v) && Numeric::isNotEqualEps(v(3, 3)))
		{
			// a unit length solution lies in null-space (last column of v)
			objectPoints.push_back(Vector3(v(0, 3), v(1, 3), v(2, 3)) / v(3, 3));
		}
		else
		{
			objectPoints.push_back(invalidObjectPoint);

			if (invalidIndices)
			{
				invalidIndices->push_back(Index32(c));
			}
		}
	}

	ocean_assert(objectPoints.size() == correspondences);
	return objectPoints;
}

size_t EpipolarGeometry::validateCameraPose(const HomogenousMatrix4& leftCamera_T_rightCamera, const PinholeCamera& leftCamera, const PinholeCamera& rightCamera, const Vector2* leftPoints, const Vector2* rightPoints, const size_t correspondences)
{
	ocean_assert(leftCamera_T_rightCamera.isValid());
	ocean_assert(leftCamera.isValid() && rightCamera.isValid());

	ocean_assert(leftPoints != nullptr || correspondences == 0);
	ocean_assert(rightPoints != nullptr || correspondences == 0);

	const HomogenousMatrix4 world_T_leftCamera(true);
	const HomogenousMatrix4 world_T_rightCamera = world_T_leftCamera * leftCamera_T_rightCamera;

	const HomogenousMatrix4 leftFlippedCamera_T_world = Camera::standard2InvertedFlipped(world_T_leftCamera);
	const HomogenousMatrix4 rightFlippedCamera_T_world = Camera::standard2InvertedFlipped(world_T_rightCamera);

	const Vector3 rightTranslation(leftCamera_T_rightCamera.translation());
	const Quaternion rightOrientation(leftCamera_T_rightCamera.rotation());

	size_t validCounter = 0;

	for (size_t n = 0; n < correspondences; ++n)
	{
		const Vector2& leftPoint = leftPoints[n];
		const Vector2& rightPoint = rightPoints[n];

		const Line3 leftRay = leftCamera.ray(leftPoint, Vector3(0, 0, 0), Quaternion(true));
		const Line3 rightRay = rightCamera.ray(rightPoint, rightTranslation, rightOrientation);

		Vector3 objectPoint;
		if (leftRay.nearestPoint(rightRay, objectPoint))
		{
			// let's ensure that the object point is in front of both cameras

			if (Camera::isObjectPointInFrontIF(leftFlippedCamera_T_world, objectPoint) && Camera::isObjectPointInFrontIF(rightFlippedCamera_T_world, objectPoint))
			{
				++validCounter;
			}
		}
	}

	ocean_assert(validCounter <= correspondences);
	return validCounter;
}

}

}
