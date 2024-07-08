/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/geometry/MultipleViewGeometry.h"
#include "ocean/geometry/EpipolarGeometry.h"
#include "ocean/geometry/NonLinearOptimizationObjectPoint.h"
#include "ocean/geometry/Normalization.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/Subset.h"

#include "ocean/math/Line2.h"
#include "ocean/math/Matrix.h"
#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/StaticMatrix.h"
#include "ocean/math/Triangle2.h"

namespace Ocean
{

namespace Geometry
{

#ifdef OCEAN_INTENSIVE_DEBUG

/**
 * This function computes the factors of the polynomial equation: <br>
 * a3 * x^3 + a2 * x^2 + a1 * x + a0 = 0.<br>
 * X = b * matrix1 + (1-b) * matrix2 with det(X) is 0
 * @param matrix1 First matrix
 * @param matrix2 Second matrix
 * @return Coefficients (a3, a2, a1, a0) of polynomial equation
 */
Vector4 composeMatrixDet0(const SquareMatrix3 matrix1, const SquareMatrix3 matrix2)
{
	// pre-compute determinants made from columns of matrix1, matrix2
	Scalar determinates[8];

	determinates[0] = matrix1.determinant();
	determinates[1] = SquareMatrix3(matrix1.xAxis(), matrix1.yAxis(), matrix2.zAxis()).determinant();
	determinates[2] = SquareMatrix3(matrix1.xAxis(), matrix2.yAxis(), matrix1.zAxis()).determinant();
	determinates[3] = SquareMatrix3(matrix1.xAxis(), matrix2.yAxis(), matrix2.zAxis()).determinant();
	determinates[4] = SquareMatrix3(matrix2.xAxis(), matrix1.yAxis(), matrix1.zAxis()).determinant();
	determinates[5] = SquareMatrix3(matrix2.xAxis(), matrix1.yAxis(), matrix2.zAxis()).determinant();
	determinates[6] = SquareMatrix3(matrix2.xAxis(), matrix2.yAxis(), matrix1.zAxis()).determinant();
	determinates[7] = matrix2.determinant();

	const Scalar a3 = -determinates[4] + determinates[3] + determinates[0] + determinates[6] + determinates[5] - determinates[2] - determinates[1] - determinates[7];
	const Scalar a2 = determinates[1] - 2 * determinates[3] - 2 * determinates[5] + determinates[4] - 2 * determinates[6] + determinates[2] + 3 * determinates[7];
	const Scalar a1 = determinates[6] + determinates[3] + determinates[5] - 3 * determinates[7];
	const Scalar a0 = determinates[7];

	return Vector4(a3, a2, a1, a0);
}
#endif

bool MultipleViewGeometry::trifocalTensorNormalizedLinear(const ImagePoint* points1, const ImagePoint* points2, const ImagePoint* points3, const size_t correspondences, TrifocalTensor& trifocal)
{
	ocean_assert(points1 && points2 && points3);
	ocean_assert(correspondences >= 7);

	/**
	 * algorithm 16.1 from multiple view geometry (p.394)
	 */

	StaticMatrix<Scalar, 3, 9> trifocalTensor3x9;
	if (!trifocalTensorNormalizedLinear(points1, points2, points3, correspondences, trifocalTensor3x9.data()))
		return false;

	memcpy(trifocal[0].data(), trifocalTensor3x9.data(), sizeof(Scalar) * 9);
	trifocal[0].transpose();

	memcpy(trifocal[1].data(), trifocalTensor3x9.data() + 9, sizeof(Scalar) * 9);
	trifocal[1].transpose();

	memcpy(trifocal[2].data(), trifocalTensor3x9.data() + 18, sizeof(Scalar) * 9);
	trifocal[2].transpose();

#ifdef OCEAN_INTENSIVE_DEBUG
	SquareMatrix3 errorMat;
	Scalar error = errorMatrix(trifocal, points1, points2, points3, correspondences, &errorMat);
	ocean_assert(Numeric::isBelow(error, Scalar(0.00001)));
	ocean_assert(Numeric::isBelow(errorMat.absSum(), Scalar(0.00001)));
#endif //OCEAN_INTENSIVE_DEBUG

	return true;
}

bool MultipleViewGeometry::trifocalTensorMinimizingError(const ImagePoint* points1, const ImagePoint* points2, const ImagePoint* points3, const size_t correspondences, TrifocalTensor& trifocal)
{
	// todo: resulting trifocal tensor is not valid
	ocean_assert(points1 && points2 && points3);
	ocean_assert(correspondences >= 7);

	/**
	 * algorithm 16.2 from multiple view geometry (p.396)
	 */

	// initial estimate of trifocal tensor
	Matrix matrixA;
	Matrix trifocalTensor(27, 1);

	if (!trifocalTensorNormalizedLinear(points1, points2, points3, correspondences, trifocalTensor.data(), &matrixA))
		return false;


	// error vector should have length 27, matrixA must be [27x27]
	matrixA = matrixA.transposedMultiply(matrixA);

#ifdef OCEAN_INTENSIVE_DEBUG
	memcpy(trifocal[0].data(), trifocalTensor.data(), sizeof(Scalar) * 9);
	trifocal[0].transpose();

	memcpy(trifocal[1].data(), trifocalTensor.data() + 9, sizeof(Scalar) * 9);
	trifocal[1].transpose();

	memcpy(trifocal[2].data(), trifocalTensor.data() + 18, sizeof(Scalar) * 9);
	trifocal[2].transpose();

	Scalar error = errorMatrix(trifocal, points1, points2, points3, correspondences);
#endif //OCEAN_INTENSIVE_DEBUG

#ifdef DO_ITERATION
	/* iterate for non-linear optimization*/
	Scalar errorNormPreviousIteration(2);

	for (unsigned int iteration = 0; iteration < 20u; iteration++)
	{
#endif // DO_ITERATION
		// find two epipole e' and e'' from trifocal tensor
		Vector3 epipole2, epipole3;
		if (!epipoles<true>(trifocalTensor.data(),  trifocalTensor.data() + 9,  trifocalTensor.data() + 18, epipole2, epipole3))
			return false;

		// construct matrix E (27 x 18) such that t = Ea where t is the vector of entries of trifocal tensor, a is the vector representing entries of A and B, and where E expresses the linear relationship T_i(j, k) = a(j, i) * e''[k] - e'[j] * b(k, i)
		Matrix matrixE(27u, 18u, false);

		unsigned int rowIndex = 0u;
		for (unsigned int i = 0u; i < 3u; ++i)
		{
			for (unsigned int j = 0u; j < 3u; ++j)
			{
				for (unsigned int k = 0u; k < 3u; ++k)
				{
					matrixE(rowIndex, 3u * j + i) = epipole3[k];
					matrixE(rowIndex, 9u + 3u * k + i) = -epipole2[j];
					++rowIndex;
				}
			}
		}

		// minimize ||AEa|| with subject ||Ea|| = 1
		const size_t rank = matrixE.rank();

		Matrix u, w, v;
		if (!matrixE.singularValueDecomposition(u, w, v))
			return false;

		const Matrix matrixEleftSingularVectors = u.subMatrix(0u, 0u, matrixE.rows(), rank);
		const Matrix matrixAEleftSv = matrixA * matrixEleftSingularVectors;

		Matrix _u, _w, _v;
		if (!matrixAEleftSv.singularValueDecomposition(_u, _w, _v))
			return false;

		Matrix vectorX(rank, 1u);

		for (unsigned int i = 0; i < rank; ++i)
			vectorX(i) = _v(i, _v.columns() - 1);

		trifocalTensor = matrixEleftSingularVectors * vectorX;

#ifdef DO_ITERATION
		Matrix errorsVector = iFlippedProjectionMatrixA * trifocalTensor; //vectorT = matrixE * vectorA;
		Scalar errorNorm = errorsVector.norm();

		if ((errorNormPreviousIteration - errorNorm) < errorNormPreviousIteration * 0.001)
			break;

		errorNormPreviousIteration = errorNorm;
#endif //DO_ITERATION

#ifdef OCEAN_INTENSIVE_DEBUG
		Matrix debugWDiagonalInv(rank, rank, false);
		for (size_t camera = 0; camera < rank; camera++) // TODO Remove this?
		{
			debugWDiagonalInv(camera, camera) = Scalar(1) / w(camera, 0);
		}

		Matrix debugVectorA = v.subMatrix(0u, 0u, v.rows(), rank) * debugWDiagonalInv * vectorX;

		Matrix debugVectorT = matrixE * debugVectorA;
#endif
#ifdef DO_ITERATION
	}
#endif //DO_ITERATION

	memcpy(trifocal[0].data(), trifocalTensor.data(), sizeof(Scalar) * 9);
	trifocal[0].transpose();

	memcpy(trifocal[1].data(), trifocalTensor.data() + 9, sizeof(Scalar) * 9);
	trifocal[1].transpose();

	memcpy(trifocal[2].data(), trifocalTensor.data() + 18, sizeof(Scalar) * 9);
	trifocal[2].transpose();

	return true;
}

bool MultipleViewGeometry::trifocalTensorIF(const ImagePoint* points1, const ImagePoint* points2, const ImagePoint* points3, const size_t correspondences, HomogenousMatrix4& iFlippedProjectionMatrix1, HomogenousMatrix4& iFlippedProjectionMatrix2, HomogenousMatrix4& iFlippedProjectionMatrix3, TrifocalTensor* trifocalTensor)
{
	ocean_assert(points1 && points2 && points3);
	ocean_assert(correspondences >= 7);

	/**
	 * algorithm 16.1 from multiple view geometry (p.394)
	 */

	TrifocalTensor trifocal;

	// calculate trifocal tensor
	if (!trifocalTensorNormalizedLinear(points1, points2, points3, correspondences, trifocal))
		return false;

	//calculate epipoles
	Vector3 normedEpipoleIF2, normedEpipoleIF3;
	if (!epipolesIF(trifocal, normedEpipoleIF2, normedEpipoleIF3))
		return false;

	//determine camera project matrices
	iFlippedProjectionMatrix1 = HomogenousMatrix4(true);
	if (!cameraProjectionMatricesIF(trifocal, normedEpipoleIF2, normedEpipoleIF3, iFlippedProjectionMatrix2, iFlippedProjectionMatrix3))
		return false;

	if (trifocalTensor)
		*trifocalTensor = trifocal;

	return true;
}

bool MultipleViewGeometry::calibrateFromProjectionsMatricesIF(const ConstIndexedAccessor<HomogenousMatrix4>& iFlippedProjectionMatrices, const unsigned int imageWidth, const unsigned int imageHeight, SquareMatrix3& cameraIntrinsic, HomogenousMatrix4* posesIF)
{
	ocean_assert(imageWidth != 0u && imageHeight != 0u && posesIF);

	const size_t views = iFlippedProjectionMatrices.size();

	ocean_assert(views >= 3);
	if (views < 3)
		return false;

	SquareMatrix4 symQ;
	if (!AutoCalibration::determineAbsoluteDualQuadricLinearIF(iFlippedProjectionMatrices, symQ, imageWidth, imageHeight))
		return false;

	// estimate camera intrinsics
	SquareMatrices3 intrinsics(views);
	if (!AutoCalibration::intrinsicsFromAbsoluteDualQuadricIF(symQ, iFlippedProjectionMatrices, intrinsics.data()))
		return false;

	// averaging intrinsics, **NOTE** simply avaraging may not be the best choice

	cameraIntrinsic = intrinsics[0];
	for (size_t i = 1; i < views; ++i)
		cameraIntrinsic += intrinsics[i];

	ocean_assert(!Numeric::isEqualEps(cameraIntrinsic(2, 2)));
	if (Numeric::isEqualEps(cameraIntrinsic(2, 2)))
		return false;

	cameraIntrinsic *= Scalar(1) / cameraIntrinsic(2, 2);

	// metric upgrade
	HomogenousMatrices4 metricsIF(views);
	if (!AutoCalibration::transformProjectiveToMetricIF(symQ, iFlippedProjectionMatrices, metricsIF.data()))
		return false;

	if (!AutoCalibration::metricProjectionMatricesToPosesIF(ConstArrayAccessor<HomogenousMatrix4>(metricsIF), cameraIntrinsic, posesIF))
		return false;

	return true;
}

bool MultipleViewGeometry::calibrateFromProjectionsMatricesIF(const HomogenousMatrix4& iFlippedProjectionMatrix1, const HomogenousMatrix4& iFlippedProjectionMatrix2, const HomogenousMatrix4& iFlippedProjectionMatrix3, const unsigned int imageWidth, const unsigned int imageHeight, SquareMatrix3& cameraIntrinsics, HomogenousMatrix4& iFlippedPose1, HomogenousMatrix4& iFlippedPose2, HomogenousMatrix4& iFlippedPose3)
{
	HomogenousMatrices4 iFlippedProjectionMatrices(3);
	iFlippedProjectionMatrices[0] = iFlippedProjectionMatrix1;
	iFlippedProjectionMatrices[1] = iFlippedProjectionMatrix2;
	iFlippedProjectionMatrices[2] = iFlippedProjectionMatrix3;

	SquareMatrix4 symQ;
	if (!AutoCalibration::findCommonIntrinsicsFromProjectionMatricesIF(ConstArrayAccessor<HomogenousMatrix4>(iFlippedProjectionMatrices), imageWidth, imageHeight, cameraIntrinsics, &symQ))
		return false;

	// estimate camera intrinsics
	SquareMatrices3 matK(3);
	if (!AutoCalibration::intrinsicsFromAbsoluteDualQuadricIF(symQ, ConstArrayAccessor<HomogenousMatrix4>(iFlippedProjectionMatrices), matK.data()))
		return false;

	// averaging intrinsics, room for improvement
	cameraIntrinsics = matK[0];
	for (unsigned int r = 0; r < 3; r++)
	{
		for (unsigned int c = 0; c < 3; c++)
		{
			for (size_t i = 1; i < 3; i++)
			{
				cameraIntrinsics(r, c) += matK[i](r, c);
			}
		}
	}

	// metric upgrade
	HomogenousMatrices4 metricsIF(3);
	if (!AutoCalibration::transformProjectiveToMetricIF(symQ, ConstArrayAccessor<HomogenousMatrix4>(iFlippedProjectionMatrices), metricsIF.data()))
		return false;

	HomogenousMatrices4 posesIF(3);
	if (!AutoCalibration::metricProjectionMatricesToPosesIF(ConstArrayAccessor<HomogenousMatrix4>(metricsIF), cameraIntrinsics, posesIF.data()))
		return false;

	iFlippedPose1 = posesIF[0];
	iFlippedPose2 = posesIF[1];
	iFlippedPose3 = posesIF[2];

	return true;
}

bool MultipleViewGeometry::projectiveReconstructionFrom6PointsIF(const ImagePoint* points1, const ImagePoint* points2, const ImagePoint* points3, const size_t correspondences, HomogenousMatrix4& iFlippedProjectionMatrix1, HomogenousMatrix4& iFlippedProjectionMatrix2, HomogenousMatrix4& iFlippedProjectionMatrix3, const Scalar squaredSuccessThreshold, Scalar* squaredProjectionError)
{
	ocean_assert(points1 && points2 && points3);
	ocean_assert(correspondences >= 6);

	RandomGenerator generator;
	const ImagePoint* points[3] = {points1, points2, points3};

	/**
	 * algorithm 20.1 from multiple view geometry (p.511)
	 */

	/**
	 * I. select six point correspondences: max. 3 collinear point pair
	 */

	IndexSet32 indexSet4NonCollinear;
	Triangle2 triangleInView[3];

	// pick a valid triangle, so this 2 points are not collinear
	unsigned int iterations = 0u; // iteration to avoid endless loop
	while(true)
	{
		while (indexSet4NonCollinear.size() < 3)
		{
			const Index32 index = RandomI::random(generator, (unsigned int)(correspondences) - 1u);
			indexSet4NonCollinear.insert(index);
		}

		bool allValid = true;
		for (unsigned int v = 0u; v < 3u; ++v)
		{
			const ImagePoints permutationImagePoints = Subset::subset(points[v], correspondences, indexSet4NonCollinear);
			triangleInView[v] = Triangle2(permutationImagePoints[0], permutationImagePoints[1], permutationImagePoints[2]);
			allValid = allValid && triangleInView[v].isValid();
		}

		if (allValid)
			break;

		indexSet4NonCollinear.clear();
		++iterations;

		if (iterations > 20u)
			return false;
	}

	ocean_assert(indexSet4NonCollinear.size() == 3);

	// add three more image points with one which is not collinear to previous ones in any view
	for (size_t iPoint = 0; iPoint < correspondences; ++iPoint)
	{
		if (!pointIsCollinear(triangleInView[0], points1[iPoint], Scalar(0.05)) && !pointIsCollinear(triangleInView[1], points2[iPoint], Scalar(0.05)) && !pointIsCollinear(triangleInView[2], points3[iPoint], Scalar(0.05)))
		{
			// is not collinear to other 3 points in set
			indexSet4NonCollinear.insert(Index32(iPoint));
			break;
		}
	}

	if (indexSet4NonCollinear.size() != 4)
		return false;

	std::vector<Index32> twoPoints;
	twoPoints.reserve(2);

	size_t i = 0;
	while (i < correspondences && twoPoints.size() != 2)
	{
		const Index32 index = Index32(i++);
		if (indexSet4NonCollinear.find(index) == indexSet4NonCollinear.end())
		{
			twoPoints.push_back(index);
		}
	}

	/**
	 * II. find projective transform in each view, so that e_i = T * x_i
	 * e_i is (1,0,0), (0,1,0), (0,0,1) or (1,1,1)
	 * and apply transformation to 2 remaining points
	 */
	SquareMatrix3 projectiveTransforms[3];
	SquareMatrix3 projectiveTransformsInv[3];
	Matrix dualFundamentals(3, 5);

	Index32 index6Points[6];
	unsigned int iIndex = 0u;
	for (IndexSet32::const_iterator it = indexSet4NonCollinear.cbegin(); it != indexSet4NonCollinear.cend(); ++it)
	{
		index6Points[iIndex++] = *it;
	}
	index6Points[4] = twoPoints[0];
	index6Points[5] = twoPoints[1];

	Vector3 transformatedPoints1[3]; // dual correspondence x^_1: transformatedPoints1[i] acts as i-th image point from "first" view
	Vector3 transformatedPoints2[3]; // dual correspondence x^_2: transformatedPoints2[i] acts as i-th image point from "second" view (mental switch of points and views)

	for (unsigned int iView = 0; iView < 3u; ++iView)
	{
		const ImagePoints nonCollinearPoints = Subset::subset(points[iView], correspondences, indexSet4NonCollinear);
		if (!calculateProjectiveBasisTransform(nonCollinearPoints[0], nonCollinearPoints[1], nonCollinearPoints[2], nonCollinearPoints[3], projectiveTransforms[iView]))
			return false;

		if (!projectiveTransforms[iView].invert(projectiveTransformsInv[iView]))
			return false;
		/*
		 * e_k = projectiveTransforms * imagePoint_k
		 * imagePoint_k = projectiveTransformsInv() * e_k
		 */

#ifdef OCEAN_INTENSIVE_DEBUG
		Vectors3 standardProjectiveBasis;
		standardProjectiveBasis.push_back(Vector3(1, 0, 0));
		standardProjectiveBasis.push_back(Vector3(0, 1, 0));
		standardProjectiveBasis.push_back(Vector3(0, 0, 1));
		standardProjectiveBasis.push_back(Vector3(1, 1, 1));
		Scalar debugSqrDistanceOriginal(0);
		Scalar debugSqrDistanceProjBasis(0);
		for (unsigned int iPoint = 0; iPoint < 4; iPoint++)
		{
			const Vector3 debugPoint = Vector3(nonCollinearPoints[iPoint], Scalar(1));
			const Vector3 debugTransformatedPoint = projectiveTransforms[iView] * debugPoint;
			debugSqrDistanceProjBasis += debugTransformatedPoint.sqrDistance(standardProjectiveBasis[iPoint]);
			const Vector3 debugImagePoint3 = projectiveTransformsInv[iView] * standardProjectiveBasis[iPoint];
			const ImagePoint debugImagePoint(debugImagePoint3[0] / debugImagePoint3[2], debugImagePoint3[1] / debugImagePoint3[2]);
			debugSqrDistanceOriginal += debugImagePoint.sqrDistance(nonCollinearPoints[iPoint]);
		}
		ocean_assert(Numeric::isEqualEps(debugSqrDistanceOriginal));
#endif

		const Vector3 point1 = Vector3(points[iView][twoPoints[0]], Scalar(1));
		const Vector3 point2 = Vector3(points[iView][twoPoints[1]], Scalar(1));
		transformatedPoints1[iView] = projectiveTransforms[iView] * point1;
		transformatedPoints2[iView] = projectiveTransforms[iView] * point2;

		/**
		 * III. derive equation of reduced fundamental matrix F^ from x^_2 * F^ * x^_1
		 *		|0		p		q|
		 * F^ =	|r		0		s|
		 *		|t -(p+q+r+s+t)	0|
		 *
		 * y * f^ = 0, f^ = [p q r s t], y^ = [y1x2 - y1z2, z1x2 - y1z2, x1y2 - y1z2, z1y2 - y1z2, x1z2 - y1z2]
		 */

		// create equation for dual fundamental matrix F^
		dualFundamentals(iView, 0) = transformatedPoints1[iView].y() * transformatedPoints2[iView].x() - transformatedPoints1[iView].y() *  transformatedPoints2[iView].z();
		dualFundamentals(iView, 1) = transformatedPoints1[iView].z() * transformatedPoints2[iView].x() - transformatedPoints1[iView].y() *  transformatedPoints2[iView].z();
		dualFundamentals(iView, 2) = transformatedPoints1[iView].x() * transformatedPoints2[iView].y() - transformatedPoints1[iView].y() *  transformatedPoints2[iView].z();
		dualFundamentals(iView, 3) = transformatedPoints1[iView].z() * transformatedPoints2[iView].y() - transformatedPoints1[iView].y() *  transformatedPoints2[iView].z();
		dualFundamentals(iView, 4) = transformatedPoints1[iView].x() * transformatedPoints2[iView].z() - transformatedPoints1[iView].y() *  transformatedPoints2[iView].z();
	}

	/**
	 * IV. get solutions for dual fundamental matrices F^1, F^2
	 * both solutions lie in right null-space of yF^=0
	 */
	Matrix uMatrix, wMatrix, vMatrix;
	if (!dualFundamentals.singularValueDecomposition(uMatrix, wMatrix, vMatrix))
		return false;

	ocean_assert(Numeric::isWeakEqualEps(wMatrix(3)) && Numeric::isWeakEqualEps(wMatrix(4)));

	const Scalar p1 = vMatrix(0, 3);
	const Scalar q1 = vMatrix(1, 3);
	const Scalar r1 = vMatrix(2, 3);
	const Scalar s1 = vMatrix(3, 3);
	const Scalar t1 = vMatrix(4, 3);
	const Scalar sum1 = -p1 - q1 - r1 - s1 - t1;
	ocean_assert(Numeric::isEqual(-(vMatrix.subMatrix(0, 3, 5, 1).sum()), sum1));

	const Scalar p2 = vMatrix(0, 4);
	const Scalar q2 = vMatrix(1, 4);
	const Scalar r2 = vMatrix(2, 4);
	const Scalar s2 = vMatrix(3, 4);
	const Scalar t2 = vMatrix(4, 4);
	const Scalar sum2 = -p2 - q2 - r2 - s2 - t2;

	// construct F^1 and F^2, with following order: 0, r, t, p, 0 ,-sum, q, s, 0 (column based)
	const SquareMatrix3 dualFundamental1(0, r1, t1, p1, 0, sum1, q1, s1, 0);
	const SquareMatrix3 dualFundamental2(0, r2, t2, p2, 0, sum2, q2, s2, 0);

	/**
	 * V. find solution F^ = a * F^1 + (1-a) * F^2 with det(F^) is 0
	 * there are 1 or 3 solutions
	 *
	 * det(F^) = 0 * | 0	q | - p * | r s | + q * | r   0	| = p * s * t - q * r * (p+q+r+s+t) = 0
	 *				 |-sum	0 |		  | t 0 |		| t -sum|									x = a * x1 + x2 - a * x2 = a(x1-x2) + x2
	 */

	const Scalar a3 = (p1 - p2) * (s1 - s2) * (t1 - t2) - (p1 - p2) * (q1 - q2) * (r1 - r2) - Numeric::sqr(q1 - q2) * (r1 - r2) - (q1 - q2) * Numeric::sqr(r1 - r2) - (q1 - q2) * (r1 - r2) * (s1 - s2) - (q1 - q2) * (r1 - r2) * (t1 - t2);

	const Scalar a2 = ((p1 - p2) * s2 * (t1 - t2) + p2 * (s1 - s2) * (t1 - t2) + (p1 - p2) * (s1 - s2) * t2)
						- ((p1 - p2) * q2 * (r1 - r2) + p2 * (q1 - q2) * (r1 - r2) + (p1 - p2) * (q1 - q2) * r2)
						- ((q1 - q2) * q2 * (r1 - r2) + q2 * (q1 - q2) * (r1 - r2) + (q1 - q2) * (q1 - q2) * r2)
						- ((q1 - q2) * r2 * (r1 - r2) + q2 * (r1 - r2) * (r1 - r2) + (q1 - q2) * (r1 - r2) * r2)
						- ((q1 - q2) * r2 * (s1 - s2) + q2 * (r1 - r2) * (s1 - s2) + (q1 - q2) * (r1 - r2) * s2)
						- ((q1 - q2) * r2 * (t1 - t2) + q2 * (r1 - r2) * (t1 - t2) + (q1 - q2) * (r1 - r2) * t2);

	const Scalar a1 = (p2 * s2 * (t1 - t2) + (p1 - p2) * s2 * t2 + p2 * (s1 - s2) * t2)
						- (p2 * q2 * (r1 - r2) + (p1 - p2) * q2 * r2 + p2 * (q1 - q2) * r2)
						- (q2 * q2 * (r1 - r2) + (q1 - q2) * q2 * r2 + q2 * (q1 - q2) * r2)
						- (q2 * r2 * (r1 - r2) + (q1 - q2) * r2 * r2 + q2 * (r1 - r2) * r2)
						- (q2 * r2 * (s1 - s2) + (q1 - q2) * r2 * s2 + q2 * (r1 - r2) * s2)
						- (q2 * r2 * (t1 - t2) + (q1 - q2) * r2 * t2 + q2 * (r1 - r2) * t2);

	const Scalar a0 = p2 * s2 * t2 - p2 * q2 * r2 - q2 * q2 * r2 - q2 * r2 * r2 - q2 * r2 * s2 - q2 * r2 * t2;

#ifdef OCEAN_INTENSIVE_DEBUG
	Vector4 debugCoeffs = composeMatrixDet0(dualFundamental1, dualFundamental2);
	ocean_assert(Numeric::isEqual(debugCoeffs[0], a3));
	ocean_assert(Numeric::isEqual(debugCoeffs[1], a2));
	ocean_assert(Numeric::isEqual(debugCoeffs[2], a1));
	ocean_assert(Numeric::isEqual(debugCoeffs[3], a0));
#endif //OCEAN_INTENSIVE_DEBUG

	Scalar bestError = Numeric::maxValue();
	Vectors4 reorderedObjectPoint(6);
	Scalar solution[3];

	unsigned int numberSolutions = Equation::solveCubic(a3, a2, a1, a0, solution[0], solution[1], solution[2]);
	if (numberSolutions < 1u || numberSolutions == 2u)
		return false;

	for (unsigned int iSolution = 0; iSolution < numberSolutions; ++iSolution)
	{
		/**
		 * VI. Determine (non-dual) camera projection matrix P' and world points X from dual fundamental matrix F^.
		 * test for each solution of a in F^ = a * F^1 + (1-a) * F^2
		 *		|a 0 0 d|		|1 0 0 1|
		 * P' =	|0 b 0 d|, P =	|0 1 0 1|
		 *		|0 0 c d|		|0 0 1 1|
		 */

		const SquareMatrix3 dualFundamental = (dualFundamental1 * solution[iSolution]) + (dualFundamental2 * (1 - solution[iSolution]));

		// rank(A) = rank(A^T)
		if (Matrix::rank(dualFundamental.data(), 3, 3) != 2 && numberSolutions != 1)
		{
			// skip this solution, if there are more candidates
			continue;
		}

#ifdef OCEAN_INTENSIVE_DEBUG
		// property of dual fundamental matrix: (1,1,1)*F^*(1,1,1)=0 -> sum(F^)=0
		ocean_assert(Numeric::isEqualEps(dualFundamental.sum()));
		// det(F^)=0
		ocean_assert(Numeric::isEqualEps(dualFundamental.determinant()));
#endif //OCEAN_INTENSIVE_DEBUG

		/**
		 * ratio a : b : c is determined by following linear system
		 * [f12 f21  0 ](a)			[ p  r   0 ](a)
		 * [f13  0  f31](b) = 0	=	[ q  0   t ](b)
		 * [ 0  f23 f32](c)			[ 0  s -sum](c)
		 */

		Matrix reorderedDualFundamental(3, 3);
		reorderedDualFundamental(0, 0) = dualFundamental(0, 1);
		reorderedDualFundamental(0, 1) = dualFundamental(1, 0);
		reorderedDualFundamental(0, 2) = 0;
		reorderedDualFundamental(1, 0) = dualFundamental(0, 2);
		reorderedDualFundamental(1, 1) = 0;
		reorderedDualFundamental(1, 2) = dualFundamental(2, 0);
		reorderedDualFundamental(2, 0) = 0;
		reorderedDualFundamental(2, 1) = dualFundamental(1, 2);
		reorderedDualFundamental(2, 2) = dualFundamental(2, 1);

#ifdef OCEAN_INTENSIVE_DEBUG
		Scalar debugP = (p1 * solution[iSolution]) + (p2 * (1 - solution[iSolution]));
		ocean_assert(Numeric::isEqual(debugP, reorderedDualFundamental(0, 0)));
		Scalar debugR = (r1 * solution[iSolution]) + (r2 * (1 - solution[iSolution]));
		ocean_assert(Numeric::isEqual(debugR, reorderedDualFundamental(0, 1)));
		Scalar debugQ = (q1 * solution[iSolution]) + (q2 * (1 - solution[iSolution]));
		ocean_assert(Numeric::isEqual(debugQ, reorderedDualFundamental(1, 0)));
		Scalar debugT = (t1 * solution[iSolution]) + (t2 * (1 - solution[iSolution]));
		ocean_assert(Numeric::isEqual(debugT, reorderedDualFundamental(1, 2)));
		Scalar debugS = (s1 * solution[iSolution]) + (s2 * (1 - solution[iSolution]));
		ocean_assert(Numeric::isEqual(debugS, reorderedDualFundamental(2, 1)));
		Scalar debugSum = (sum1 * solution[iSolution]) + (sum2 * (1 - solution[iSolution]));
		ocean_assert(Numeric::isEqual(debugSum, reorderedDualFundamental(2, 2)));
#endif //OCEAN_INTENSIVE_DEBUG

		if (!reorderedDualFundamental.singularValueDecomposition(uMatrix, wMatrix, vMatrix))
			continue;

		const Scalar a = vMatrix(0, 2);
		const Scalar b = vMatrix(1, 2);
		const Scalar c = vMatrix(2, 2);

		ocean_assert(Numeric::isWeakEqualEps(wMatrix(2)));
		ocean_assert(reorderedDualFundamental.rank() == 2);

#ifdef OCEAN_INTENSIVE_DEBUG
		Matrix debugNullMatrix = reorderedDualFundamental * vMatrix.subMatrix(0, 2, 3, 1);
		ocean_assert(Numeric::isWeakEqualEps(debugNullMatrix.sum()));
#endif //OCEAN_INTENSIVE_DEBUG

		/**
		 * ratio (d-a) : (d-b) : (d-c) is determined by following linear system
		 * (d-a d-b d-a) * F^ = 0   =  F^.t() * x = 0
		 */

		if (!(Matrix(3, 3, dualFundamental.data())).singularValueDecomposition(uMatrix, wMatrix, vMatrix)) // SquareMatrix3 --> Matrix^Transpose
			continue;

		ocean_assert(Numeric::isWeakEqualEps(wMatrix(2)));

		const Scalar da = vMatrix(0, 2);
		const Scalar db = vMatrix(1, 2);
		const Scalar dc = vMatrix(2, 2);

#ifdef OCEAN_INTENSIVE_DEBUG
		Matrix debugNullMatrix2 = vMatrix.subMatrix(0, 2, 3, 1).transposed() * (Matrix(3, 3, dualFundamental.data())).transposed();
		ocean_assert(Numeric::isWeakEqualEps(debugNullMatrix.sum()));
#endif //OCEAN_INTENSIVE_DEBUG

		/**
		 * extract parameters (a,b,c,d) for second reduced camera
		 *		[0			b*(d-c)	-c*(d-b)]		[a 0 0 d]
		 * F^ =	[-a*(d-c)	0		 c*(d-a)], P' = [0 b 0 d]
		 *		[a*(d-b)	-b*(d-a)	0	]		[0 0 c d]
		 */

		Matrix matrix(6, 4);
		matrix(0, 0) = 0;
		matrix(0, 1) = -c;
		matrix(0, 2) = b;
		matrix(0, 3) = 0;
		matrix(1, 0) = c;
		matrix(1, 1) = 0;
		matrix(1, 2) = -a;
		matrix(1, 3) = 0;
		matrix(2, 0) = -b;
		matrix(2, 1) = a;
		matrix(2, 2) = 0;
		matrix(2, 3) = 0;
		matrix(3, 0) = db;
		matrix(3, 1) = -da;
		matrix(3, 2) = 0;
		matrix(3, 3) = da-db;
		matrix(4, 0) = 0;
		matrix(4, 1) = dc;
		matrix(4, 2) = -db;
		matrix(4, 3) = db-dc;
		matrix(5, 0) = -dc;
		matrix(5, 1) = 0;
		matrix(5, 2) = da;
		matrix(5, 3) = dc-da;

		if (!matrix.singularValueDecomposition(uMatrix, wMatrix, vMatrix))
			continue;

		ocean_assert(Numeric::isWeakEqualEps(wMatrix(3)));

		// [a,b,c,d] = [vMatrix(0, 3),vMatrix(1, 3),vMatrix(2, 3),vMatrix(3, 3)]

		/**
		 * VII. Reconstruct object points and compute camera projection matrices
		 * X (4D object points) of indexSet4NonCollinear are (1,0,0,0),(0,1,0,0),(0,0,1,0),(0,0,0,1)
		 *						twoPoints: (1,1,1,1),(a,b,c,d)
		 * determine P,P',P'' such that P*X_j = x_j,	X=(X1,X2,X3,X4),	x=(x1,x2,x3)
		 * so, solve for reduced camera P:
		 * [0	0	0	0	-x3_i*(X1 X2 X3 X4)	 x2_i(X1 X2 X3 X4)]
		 * [x3_i*(X1 X2 X3 X4)	0	0	0	0	-x1_i(X1 X2 X3 X4)]  * p = 0
		 * [-x2_i*(X1 X2 X3 X4)	x1_i(X1 X2 X3 X4)	0	0	0	0 ]
		 */

		reorderedObjectPoint[0] = Vector4(1, 0, 0, 0);
		reorderedObjectPoint[1] = Vector4(0, 1, 0, 0);
		reorderedObjectPoint[2] = Vector4(0, 0, 1, 0);
		reorderedObjectPoint[3] = Vector4(0, 0, 0, 1);
		reorderedObjectPoint[4] = Vector4(1, 1, 1, 1);
		reorderedObjectPoint[5] = Vector4(vMatrix(0, 3), vMatrix(1, 3), vMatrix(2, 3), vMatrix(3, 3));


		HomogenousMatrices4 candidatesProjectionMatrix;
		candidatesProjectionMatrix.reserve(3);

		bool validCameraMatrices = true;

		for (unsigned int iView = 0; iView < 3u; ++iView)
		{
#ifdef _THIS_AN_ALTERNATIVE_SOLVING_OF_CAMERA_PROJECTION_MATRIX_IN_DUALITY_SPACE
			Matrix matrixProjection(6, 4);
			// set (1,1,1,1) as object point
			matrixProjection(0, 0) = 0;
			matrixProjection(0, 1) = transformatedPoints1[iView].z();
			matrixProjection(0, 2) = -transformatedPoints1[iView].y();
			matrixProjection(0, 3) = transformatedPoints1[iView].z() - transformatedPoints1[iView].y();
			matrixProjection(1, 0) = -transformatedPoints1[iView].z();
			matrixProjection(1, 1) = 0;
			matrixProjection(1, 2) = transformatedPoints1[iView].x();
			matrixProjection(1, 3) = transformatedPoints1[iView].x() - transformatedPoints1[iView].z();
			matrixProjection(2, 0) = transformatedPoints1[iView].y();
			matrixProjection(2, 1) = -transformatedPoints1[iView].x();
			matrixProjection(2, 2) = 0;
			matrixProjection(2, 3) = transformatedPoints1[iView].y() - transformatedPoints1[iView].x();
			// set (a,b,c,d) = (v(0, 3),v(1, 3),v(2, 3),v(3, 3)) as object point
			matrixProjection(3, 0) = 0;
			matrixProjection(3, 1) = vMatrix(1, 3) * transformatedPoints2[iView].z();
			matrixProjection(3, 2) = -vMatrix(2, 3) * transformatedPoints2[iView].y();
			matrixProjection(3, 3) = vMatrix(3, 3) * (transformatedPoints2[iView].z() - transformatedPoints2[iView].y());
			matrixProjection(4, 0) = -vMatrix(0, 3) * transformatedPoints2[iView].z();
			matrixProjection(4, 1) = 0;
			matrixProjection(4, 2) = vMatrix(2, 3) * transformatedPoints2[iView].x();
			matrixProjection(4, 3) = vMatrix(3, 3) * (transformatedPoints2[iView].x() - transformatedPoints2[iView].z());
			matrixProjection(5, 0) = vMatrix(0, 3) * transformatedPoints2[iView].y();
			matrixProjection(5, 1) = -vMatrix(2, 3) * transformatedPoints2[iView].x();
			matrixProjection(5, 2) = 0;
			matrixProjection(5, 3) = vMatrix(3, 3) * (transformatedPoints2[iView].y() - transformatedPoints2[iView].x());

			Matrix _u, _w, _v;
			if (!matrixProjection.singularValueDecomposition(_u, _w, _v))
			{
				validCameraMatrices = false;
				break;
			}

			HomogenousMatrix4 currentParams(true);
			currentParams(0, 0) = _v(0, 3);
			currentParams(0, 3) = _v(3, 3);
			currentParams(1, 1) = _v(1, 3);
			currentParams(1, 3) = _v(3, 3);
			currentParams(2, 2) = _v(2, 3);
			currentParams(2, 3) = _v(3, 3);
#endif // _THIS_AN_ALTERNATIVE_SOLVING_OF_CAMERA_PROJECTION_MATRIX_IN_DUALITY_SPACE
#ifdef _THIS_AN_ALTERNATIVE_SOLVING_OF_CAMERA_PROJECTION_MATRIX
			const ImagePoint* currentPoints = points[iView];

			Matrix matrixProjection(18, 12, false);
			Matrix matrixImagePoints(18, 1);
			for (unsigned int iPoint = 0u; iPoint < 6u; ++iPoint)
			{
				for (unsigned int iComponent = 0u; iComponent < 4u; ++iComponent)
				{
					matrixProjection(3u * iPoint, iComponent) = reorderedObjectPoint[iPoint][iComponent];
					matrixProjection(3u * iPoint + 1, 4u + iComponent) = reorderedObjectPoint[iPoint][iComponent];
					matrixProjection(3u * iPoint + 2, 8u + iComponent) = reorderedObjectPoint[iPoint][iComponent];
				}

				matrixImagePoints(3u * iPoint) = currentPoints[index6Points[iPoint]].x();
				matrixImagePoints(3u * iPoint + 1u) = currentPoints[index6Points[iPoint]].y();
				matrixImagePoints(3u * iPoint + 2u) = 1;
			}
			const Matrix matrixT = matrixProjection.transposed();

			Matrix params;
			if (!(matrixT*matrixProjection).solve(matrixT*matrixImagePoints, params))
			{
				validCameraMatrices = false;
				break;
			}
			HomogenousMatrix4 currentParams;
			unsigned int count = 0;
			for (unsigned int x = 0; x < 3; x++)
			{
				for (unsigned int y = 0; y < 4; y++)
				{
					currentParams(x, y) = params(count++);
				}
			}
			currentParams(3, 0) = 0;
			currentParams(3, 1) = 0;
			currentParams(3, 2) = 0;
			currentParams(3, 3) = 1;
#endif // _THIS_AN_ALTERNATIVE_SOLVING_OF_CAMERA_PROJECTION_MATRIX

			const ImagePoint* currentPoints = points[iView];

			Matrix matrixProjection(12, 12, false);

			for (unsigned int iPoint = 0u; iPoint < 6u; ++iPoint)
			{
				const unsigned int firstRow = 2u * iPoint;
				const unsigned int secondRow = firstRow+ 1u;
				for (unsigned int iComponent = 0u; iComponent < 4u; ++iComponent)
				{
					matrixProjection(firstRow, 4u + iComponent) = -reorderedObjectPoint[iPoint][iComponent];
					matrixProjection(firstRow, 8u + iComponent) = currentPoints[index6Points[iPoint]].y() * reorderedObjectPoint[iPoint][iComponent];
					matrixProjection(secondRow, iComponent) = reorderedObjectPoint[iPoint][iComponent];
					matrixProjection(secondRow, 8u + iComponent) = -currentPoints[index6Points[iPoint]].x() * reorderedObjectPoint[iPoint][iComponent];
				}
			}

			Matrix _u, _w, _v;
			if (!matrixProjection.singularValueDecomposition(_u, _w, _v))
			{
				validCameraMatrices = false;
				break;
			}

#ifdef OCEAN_DEBUG
			{
				Vector3 rotX(_v(0, 11), _v(1, 11), _v(2, 11));
				Vector3 rotY(_v(3, 11), _v(4, 11), _v(5, 11));
				Vector3 rotZ(_v(6, 11), _v(7, 11), _v(8, 11));
				Vector3 trans(_v(9, 11), _v(10, 11), _v(11, 11));

				const Scalar xyAngle = Numeric::rad2deg(rotX.angle(rotY));
				const Scalar xzAngle = Numeric::rad2deg(rotX.angle(rotZ));
				const Scalar yzAngle = Numeric::rad2deg(rotY.angle(rotZ));

				OCEAN_SUPPRESS_UNUSED_WARNING(xyAngle);
				OCEAN_SUPPRESS_UNUSED_WARNING(xzAngle);
				OCEAN_SUPPRESS_UNUSED_WARNING(yzAngle);
			}
#endif

			HomogenousMatrix4 currentParams;
			unsigned int singularVector = 0;
			for (unsigned int x = 0; x < 3; x++)
			{
				for (unsigned int y = 0; y < 4; y++)
				{
					currentParams(x, y) = _v(singularVector++, 11);
				}
			}
			currentParams(3, 0) = 0;
			currentParams(3, 1) = 0;
			currentParams(3, 2) = 0;
			currentParams(3, 3) = 1;

			candidatesProjectionMatrix.push_back(currentParams);
		}

		if (validCameraMatrices)
		{
			Scalar backProjectionError(0);
			for (unsigned int iView = 0; iView < 3u; ++iView)
			{
#ifdef _THIS_AN_ALTERNATIVE_SOLVING_OF_CAMERA_PROJECTION_MATRIX_IN_DUALITY_SPACE
				// convert camera projection matrices from canonical back to original image bases
				candidatesProjectionMatrix[iView] = HomogenousMatrix4(projectiveTransformsInv[iView]) * candidatesProjectionMatrix[iView];
#endif// _THIS_AN_ALTERNATIVE_SOLVING_OF_CAMERA_PROJECTION_MATRIX_IN_DUALITY_SPACE

				const ImagePoint* currentPoints = points[iView];

				// calculate back-projection error
				for (unsigned int iPoints = 0; iPoints < 6; ++iPoints)
				{
					const Vector4 projPoint = candidatesProjectionMatrix[iView] * reorderedObjectPoint[iPoints];
					if (Numeric::isEqualEps(projPoint[2]))
					{
						backProjectionError = Numeric::maxValue();
						continue;
					}
					const ImagePoint backProjectedPoint(projPoint[0] / projPoint[2], projPoint[1] / projPoint[2]);
					backProjectionError += currentPoints[index6Points[iPoints]].sqrDistance(backProjectedPoint);
				}
			}

			// select best candidates
			if (backProjectionError < bestError)
			{
				iFlippedProjectionMatrix1 = candidatesProjectionMatrix[0];
				iFlippedProjectionMatrix2 = candidatesProjectionMatrix[1];
				iFlippedProjectionMatrix3 = candidatesProjectionMatrix[2];

				bestError = backProjectionError;
			}
		}
	}

	if (squaredProjectionError)
	{
		*squaredProjectionError = bestError / 18; // 6 points in 3 views
	}

	bool success = bestError < squaredSuccessThreshold * 18;// 6 points in 3 views

	return success;
}

bool MultipleViewGeometry::projectiveReconstructionFrom6PointsIF(const ConstIndexedAccessor<ImagePoints>& imagePointsPerPose, NonconstIndexedAccessor<HomogenousMatrix4>* posesIF, const Scalar squaredSuccessThreshold, Scalar* squaredProjectionError)
{
	const size_t views = imagePointsPerPose.size();

	ScopedNonconstMemoryAccessor<HomogenousMatrix4> scopedPosesIF(posesIF, views);
	ocean_assert(scopedPosesIF.size() == imagePointsPerPose.size());

	if (views < 3)
		return false;
	else if (views == 3)
	{
		HomogenousMatrix4 iFlippedProjectionMatrix1, iFlippedProjectionMatrix2, iFlippedProjectionMatrix3;
		bool success = projectiveReconstructionFrom6PointsIF(&imagePointsPerPose[0][0], &imagePointsPerPose[1][0], &imagePointsPerPose[2][0], imagePointsPerPose[0].size(), iFlippedProjectionMatrix1, iFlippedProjectionMatrix2, iFlippedProjectionMatrix3, squaredSuccessThreshold, squaredProjectionError);
		if (success)
		{
			scopedPosesIF[0] = iFlippedProjectionMatrix1;
			scopedPosesIF[1] = iFlippedProjectionMatrix2;
			scopedPosesIF[2] = iFlippedProjectionMatrix3;
		}
		return success;
	}

	size_t correspondences = imagePointsPerPose[0].size();
	ocean_assert(correspondences >= 6);

	RandomGenerator generator;

	/**
	 * algorithm 20.1 from multiple view geometry (p.511)
	 */

	/**
	 * I. select six point correspondences: max. 3 collinear point pair
	 */

	IndexSet32 indexSet4NonCollinear;
	Triangles2 triangleInView(views);

	// pick a valid triangle, so this 2 points are not collinear
	unsigned int iterations = 0u; // iteration to avoid endless loop
	while(true)
	{
		while (indexSet4NonCollinear.size() < 3)
		{
			const Index32 index = RandomI::random(generator, (unsigned int)(correspondences) - 1u);
			indexSet4NonCollinear.insert(index);
		}

		bool allValid = true;
		for (unsigned int v = 0u; v < views; ++v)
		{
			const ImagePoints permutationImagePoints = Subset::subset(imagePointsPerPose[v], indexSet4NonCollinear);
			triangleInView[v] = Triangle2(permutationImagePoints[0], permutationImagePoints[1], permutationImagePoints[2]);
			allValid = allValid && triangleInView[v].isValid();
		}

		if (allValid)
			break;

		indexSet4NonCollinear.clear();
		++iterations;

		if (iterations > 20u)
			return false;
	}

	ocean_assert(indexSet4NonCollinear.size() == 3);

	// add three more image points with one which is not collinear to previous ones in any view
	for (size_t iPoint = 0; iPoint < correspondences; ++iPoint)
	{
		bool notCollinearInAnyView = true;
		for (size_t iView = 0; iView < views; iView++)
		{
			notCollinearInAnyView = notCollinearInAnyView && !pointIsCollinear(triangleInView[iView], imagePointsPerPose[iView][iPoint], Scalar(0.05));
		}

		if (notCollinearInAnyView)
		{
			// is not collinear to other 3 points in set
			indexSet4NonCollinear.insert(Index32(iPoint));
			break;
		}
	}

	if (indexSet4NonCollinear.size() != 4)
		return false;

	std::vector<Index32> twoPoints;
	twoPoints.reserve(2);

	size_t i = 0;
	while (i < correspondences && twoPoints.size() != 2)
	{
		const Index32 index = Index32(i++);
		if (indexSet4NonCollinear.find(index) == indexSet4NonCollinear.end())
		{
			twoPoints.push_back(index);
		}
	}

	/**
	 * II. find projective transform in each view, so that e_i = T * x_i
	 * e_i is (1,0,0), (0,1,0), (0,0,1) or (1,1,1)
	 * and apply transformation to 2 remaining points
	 */
	SquareMatrices3 projectiveTransforms(views);
	SquareMatrices3 projectiveTransformsInv(views);

	Matrix dualFundamentals(views, 5);

	Index32 index6Points[6];
	unsigned int iIndex = 0u;
	for (IndexSet32::const_iterator it = indexSet4NonCollinear.cbegin(); it != indexSet4NonCollinear.cend(); ++it)
	{
		index6Points[iIndex++] = *it;
	}
	index6Points[4] = twoPoints[0];
	index6Points[5] = twoPoints[1];

	Vectors3 transformatedPoints1(views); // dual correspondence x^_1: transformatedPoints1[i] acts as i-th image point from "first" view
	Vectors3 transformatedPoints2(views); // dual correspondence x^_2: transformatedPoints2[i] acts as i-th image point from "second" view (mental switch of points and views)

	for (unsigned int iView = 0; iView < views; ++iView)
	{
		const ImagePoints nonCollinearPoints = Subset::subset(imagePointsPerPose[iView], indexSet4NonCollinear);

		/*
		 * e_k = projectiveTransforms * imagePoint_k
		 * imagePoint_k = projectiveTransformsInv * e_k
		 */
		if (!calculateProjectiveBasisTransform(nonCollinearPoints[0], nonCollinearPoints[1], nonCollinearPoints[2], nonCollinearPoints[3], projectiveTransforms[iView]))
			return false;

		if (!projectiveTransforms[iView].invert(projectiveTransformsInv[iView]))
			return false;

#ifdef OCEAN_INTENSIVE_DEBUG
		Vectors3 standardProjectiveBasis;
		standardProjectiveBasis.push_back(Vector3(1, 0, 0));
		standardProjectiveBasis.push_back(Vector3(0, 1, 0));
		standardProjectiveBasis.push_back(Vector3(0, 0, 1));
		standardProjectiveBasis.push_back(Vector3(1, 1, 1));
		Scalar debugSqrDistanceOriginal(0);
		Scalar debugSqrDistanceProjBasis(0);
		for (unsigned int iPoint = 0; iPoint < 4; iPoint++)
		{
			const Vector3 debugPoint = Vector3(nonCollinearPoints[iPoint], Scalar(1));
			const Vector3 debugTransformatedPoint = projectiveTransforms[iView] * debugPoint;
			debugSqrDistanceProjBasis += debugTransformatedPoint.sqrDistance(standardProjectiveBasis[iPoint]);
			const Vector3 debugImagePoint3 = projectiveTransforms[iView].inverted() * standardProjectiveBasis[iPoint];
			const ImagePoint debugImagePoint(debugImagePoint3[0] / debugImagePoint3[2], debugImagePoint3[1] / debugImagePoint3[2]);
			debugSqrDistanceOriginal += debugImagePoint.sqrDistance(nonCollinearPoints[iPoint]);
		}
		ocean_assert(Numeric::isEqualEps(debugSqrDistanceOriginal));
#endif

		const Vector3 point1 = Vector3(imagePointsPerPose[iView][twoPoints[0]], Scalar(1));
		const Vector3 point2 = Vector3(imagePointsPerPose[iView][twoPoints[1]], Scalar(1));
		transformatedPoints1[iView] = projectiveTransforms[iView] * point1;
		transformatedPoints2[iView] = projectiveTransforms[iView] * point2;

		/**
		 * III. derive equation of reduced fundamental matrix F^ from x^_2 * F^ * x^_1
		 *		|0		p		q|
		 * F^ =	|r		0		s|
		 *		|t -(p+q+r+s+t)	0|
		 *
		 * y * f^ = 0, f^ = [p q r s t], y^ = [y1x2 - y1z2, z1x2 - y1z2, x1y2 - y1z2, z1y2 - y1z2, x1z2 - y1z2]
		 */

		// create equation for dual fundamental matrix F^
		dualFundamentals(iView, 0) = transformatedPoints1[iView].y() * transformatedPoints2[iView].x() - transformatedPoints1[iView].y() *  transformatedPoints2[iView].z();
		dualFundamentals(iView, 1) = transformatedPoints1[iView].z() * transformatedPoints2[iView].x() - transformatedPoints1[iView].y() *  transformatedPoints2[iView].z();
		dualFundamentals(iView, 2) = transformatedPoints1[iView].x() * transformatedPoints2[iView].y() - transformatedPoints1[iView].y() *  transformatedPoints2[iView].z();
		dualFundamentals(iView, 3) = transformatedPoints1[iView].z() * transformatedPoints2[iView].y() - transformatedPoints1[iView].y() *  transformatedPoints2[iView].z();
		dualFundamentals(iView, 4) = transformatedPoints1[iView].x() * transformatedPoints2[iView].z() - transformatedPoints1[iView].y() *  transformatedPoints2[iView].z();
	}

	/**
	 * IV. get solutions for dual fundamental matrices F^
	 * unique solution lie in right null-space of yF^=0
	 */
	Matrix uMatrix, wMatrix, vMatrix;
	if (!dualFundamentals.singularValueDecomposition(uMatrix, wMatrix, vMatrix))
		return false;

#ifdef OCEAN_INTENSIVE_DEBUG
	// this is too strict for noised data
	ocean_assert(Numeric::isWeakEqualEps(wMatrix(3)) && Numeric::isWeakEqualEps(wMatrix(4)));
#endif

	const Scalar p2 = vMatrix(0, 4);
	const Scalar q2 = vMatrix(1, 4);
	const Scalar r2 = vMatrix(2, 4);
	const Scalar s2 = vMatrix(3, 4);
	const Scalar t2 = vMatrix(4, 4);
	const Scalar sum2 = -p2 - q2 - r2 - s2 - t2;

	// construct F^, with following order: 0, r, t, p, 0 ,-sum, q, s, 0 (column based)
	const SquareMatrix3 dualFundamental(0, r2, t2, p2, 0, sum2, q2, s2, 0);

	Scalar bestError = Numeric::maxValue();
	Vectors4 reorderedObjectPoint(6);

		/**
		 * VI. Determine (non-dual) camera projection matrix P' and world points X from dual fundamental matrix F^.
		 * test for each solution of a in F^ = a * F^1 + (1-a) * F^2
		 *		|a 0 0 d|		|1 0 0 1|
		 * P' =	|0 b 0 d|, P =	|0 1 0 1|
		 *		|0 0 c d|		|0 0 1 1|
		 */


#ifdef OCEAN_INTENSIVE_DEBUG
	// this is too strict for noisy data

	// property of dual fundamental matrix: (1,1,1)*F^*(1,1,1)=0 -> sum(F^)=0
	ocean_assert(Numeric::isEqualEps(dualFundamental.sum()));
	// det(F^)=0
	ocean_assert(Numeric::isEqualEps(dualFundamental.determinant()));
#endif //OCEAN_INTENSIVE_DEBUG

	/**
		* ratio a : b : c is determined by following linear system
		* [f12 f21  0 ](a)			[ p  r   0 ](a)
		* [f13  0  f31](b) = 0	=	[ q  0   t ](b)
		* [ 0  f23 f32](c)			[ 0  s -sum](c)
		*/

	Matrix reorderedDualFundamental(3, 3);
	reorderedDualFundamental(0, 0) = dualFundamental(0, 1);
	reorderedDualFundamental(0, 1) = dualFundamental(1, 0);
	reorderedDualFundamental(0, 2) = 0;
	reorderedDualFundamental(1, 0) = dualFundamental(0, 2);
	reorderedDualFundamental(1, 1) = 0;
	reorderedDualFundamental(1, 2) = dualFundamental(2, 0);
	reorderedDualFundamental(2, 0) = 0;
	reorderedDualFundamental(2, 1) = dualFundamental(1, 2);
	reorderedDualFundamental(2, 2) = dualFundamental(2, 1);

	if (!reorderedDualFundamental.singularValueDecomposition(uMatrix, wMatrix, vMatrix))
		return false;

	const Scalar a = vMatrix(0, 2);
	const Scalar b = vMatrix(1, 2);
	const Scalar c = vMatrix(2, 2);

#ifdef OCEAN_INTENSIVE_DEBUG
	// this is too strict for noised data
	ocean_assert(Numeric::isWeakEqualEps(wMatrix(2)));
	ocean_assert(reorderedDualFundamental.rank() == 2);

	Matrix debugNullMatrix = reorderedDualFundamental * vMatrix.subMatrix(0, 2, 3, 1);
	ocean_assert(Numeric::isWeakEqualEps(debugNullMatrix.sum()));
#endif //OCEAN_INTENSIVE_DEBUG

	/**
		* ratio (d-a) : (d-b) : (d-c) is determined by following linear system
		* (d-a d-b d-a) * F^ = 0   =  F^.t() * x = 0
		*/

	if (!(Matrix(3, 3, dualFundamental.data())).singularValueDecomposition(uMatrix, wMatrix, vMatrix)) // SquareMatrix3 --> Matrix^Transpose
		return false;

#ifdef OCEAN_INTENSIVE_DEBUG
	// this is too strict for noised data
	ocean_assert(Numeric::isWeakEqualEps(wMatrix(2)));
#endif

	const Scalar da = vMatrix(0, 2);
	const Scalar db = vMatrix(1, 2);
	const Scalar dc = vMatrix(2, 2);

#ifdef OCEAN_INTENSIVE_DEBUG
	Matrix debugNullMatrix2 = vMatrix.subMatrix(0, 2, 3, 1).transposed() * (Matrix(3, 3, dualFundamental.data())).transposed();
	ocean_assert(Numeric::isWeakEqualEps(debugNullMatrix.sum()));
#endif //OCEAN_INTENSIVE_DEBUG

	/**
		* extract parameters (a,b,c,d) for second reduced camera
		*		[0			b*(d-c)	-c*(d-b)]		[a 0 0 d]
		* F^ =	[-a*(d-c)	0		 c*(d-a)], P' = [0 b 0 d]
		*		[a*(d-b)	-b*(d-a)	0	]		[0 0 c d]
		*/

	Matrix matrix(6, 4);
	matrix(0, 0) = 0;
	matrix(0, 1) = -c;
	matrix(0, 2) = b;
	matrix(0, 3) = 0;
	matrix(1, 0) = c;
	matrix(1, 1) = 0;
	matrix(1, 2) = -a;
	matrix(1, 3) = 0;
	matrix(2, 0) = -b;
	matrix(2, 1) = a;
	matrix(2, 2) = 0;
	matrix(2, 3) = 0;
	matrix(3, 0) = db;
	matrix(3, 1) = -da;
	matrix(3, 2) = 0;
	matrix(3, 3) = da-db;
	matrix(4, 0) = 0;
	matrix(4, 1) = dc;
	matrix(4, 2) = -db;
	matrix(4, 3) = db-dc;
	matrix(5, 0) = -dc;
	matrix(5, 1) = 0;
	matrix(5, 2) = da;
	matrix(5, 3) = dc-da;

	if (!matrix.singularValueDecomposition(uMatrix, wMatrix, vMatrix))
		return false;

#ifdef OCEAN_INTENSIVE_DEBUG
	// this is too strict for noised data
	ocean_assert(Numeric::isWeakEqualEps(wMatrix(3)));
#endif

	// [a,b,c,d] = [vMatrix(0, 3),vMatrix(1, 3),vMatrix(2, 3),vMatrix(3, 3)]

	/**
	 * VII. Reconstruct object points and compute camera projection matrices
	 * X (4D object points) of indexSet4NonCollinear are (1,0,0,0),(0,1,0,0),(0,0,1,0),(0,0,0,1)
	 *						twoPoints: (1,1,1,1),(a,b,c,d)
	 * determine P,P',P'' such that P*X_j = x_j,	X=(X1,X2,X3,X4),	x=(x1,x2,x3)
	 * so, solve for reduced camera P:
	 * [0	0	0	0	-x3_i*(X1 X2 X3 X4)	 x2_i(X1 X2 X3 X4)]
	 * [x3_i*(X1 X2 X3 X4)	0	0	0	0	-x1_i(X1 X2 X3 X4)]  * p = 0
	 * [-x2_i*(X1 X2 X3 X4)	x1_i(X1 X2 X3 X4)	0	0	0	0 ]
	 */

	reorderedObjectPoint[0] = Vector4(1, 0, 0, 0);
	reorderedObjectPoint[1] = Vector4(0, 1, 0, 0);
	reorderedObjectPoint[2] = Vector4(0, 0, 1, 0);
	reorderedObjectPoint[3] = Vector4(0, 0, 0, 1);
	reorderedObjectPoint[4] = Vector4(1, 1, 1, 1);
	reorderedObjectPoint[5] = Vector4(vMatrix(0, 3), vMatrix(1, 3), vMatrix(2, 3), vMatrix(3, 3));

	HomogenousMatrices4 candidatesProjectionMatrix;
	candidatesProjectionMatrix.reserve(views);

	bool validCameraMatrices = true;

	for (unsigned int iView = 0; iView < views; ++iView)
	{
		const ImagePoints currentPoints = imagePointsPerPose[iView];

		Matrix matrixProjection(12, 12, false);

		for (unsigned int iPoint = 0u; iPoint < 6u; ++iPoint)
		{
			const unsigned int firstRow = 2u * iPoint;
			const unsigned int secondRow = firstRow+ 1u;
			for (unsigned int iComponent = 0u; iComponent < 4u; ++iComponent)
			{
				matrixProjection(firstRow, 4u + iComponent) = -reorderedObjectPoint[iPoint][iComponent];
				matrixProjection(firstRow, 8u + iComponent) = currentPoints[index6Points[iPoint]].y() * reorderedObjectPoint[iPoint][iComponent];
				matrixProjection(secondRow, iComponent) = reorderedObjectPoint[iPoint][iComponent];
				matrixProjection(secondRow, 8u + iComponent) = -currentPoints[index6Points[iPoint]].x() * reorderedObjectPoint[iPoint][iComponent];
			}
		}

		Matrix _u, _w, _v;
		if (!matrixProjection.singularValueDecomposition(_u, _w, _v))
		{
			validCameraMatrices = false;
			break;
		}

		HomogenousMatrix4 currentParams;
		unsigned int singularVector = 0;
		for (unsigned int x = 0; x < 3; x++)
		{
			for (unsigned int y = 0; y < 4; y++)
			{
				currentParams(x, y) = _v(singularVector++, 11);
			}
		}
		currentParams(3, 0) = 0;
		currentParams(3, 1) = 0;
		currentParams(3, 2) = 0;
		currentParams(3, 3) = 1;

		candidatesProjectionMatrix.push_back(currentParams);
	}

	if (validCameraMatrices)
	{
		Scalar backProjectionError(0);
		for (unsigned int iView = 0; iView < views; ++iView)
		{
			const ImagePoints currentPoints = imagePointsPerPose[iView];

			// calculate back-projection error
			for (unsigned int iPoints = 0; iPoints < 6; ++iPoints)
			{
				const Vector4 projPoint = candidatesProjectionMatrix[iView] * reorderedObjectPoint[iPoints];
				if (Numeric::isEqualEps(projPoint[2]))
				{
					backProjectionError = Numeric::maxValue();
					continue;
				}
				const ImagePoint backProjectedPoint(projPoint[0] / projPoint[2], projPoint[1] / projPoint[2]);
				backProjectionError += currentPoints[index6Points[iPoints]].sqrDistance(backProjectedPoint);
			}
		}

		// select best candidates
		if (backProjectionError < bestError)
		{
			for (size_t iView = 0; iView < views; ++iView)
			{
				scopedPosesIF[iView] = candidatesProjectionMatrix[iView];
			}

			bestError = backProjectionError;
		}
	}

	if (squaredProjectionError)
	{
		*squaredProjectionError = bestError / Scalar(6u * views); // 6 points in m views
	}

	bool success = bestError < squaredSuccessThreshold * Scalar(6u * views);

	return success;
}

bool MultipleViewGeometry::trifocalTensorFromProjectionMatrices(const HomogenousMatrix4& iFlippedProjectionMatrixB, const HomogenousMatrix4& iFlippedProjectionMatrixC, TrifocalTensor& trifocal)
{
	/**
	 * algorithm 15.1 from multiple view geometry (p.367)
	 *T_i = b_i * c_4.t() - b_4 * c_i.t()
	 */

	const SquareMatrix3 matrixB = iFlippedProjectionMatrixB.rotationMatrix();
	const Vector3 b4 = iFlippedProjectionMatrixB.translation();
	const SquareMatrix3 matrixC = iFlippedProjectionMatrixC.rotationMatrix();
	const Vector3 c4 = iFlippedProjectionMatrixC.translation();

	const Vector3 b1 = matrixB.xAxis(); //1st col of matrixA
	const Vector3 b2 = matrixB.yAxis();
	const Vector3 b3 = matrixB.zAxis();

	const Vector3 c1 = matrixC.xAxis();
	const Vector3 c2 = matrixC.yAxis();
	const Vector3 c3 = matrixC.zAxis();

	for (unsigned int j = 0; j < 3; ++j)
	{
		for (unsigned int k = 0; k < 3; ++k)
		{
			trifocal[0](j, k) = b1[j] * c4[k] - b4[j] * c1[k];
			trifocal[1](j, k) = b2[j] * c4[k] - b4[j] * c2[k];
			trifocal[2](j, k) = b3[j] * c4[k] - b4[j] * c3[k];
		}
	}

	return true;
}

bool MultipleViewGeometry::trifocalTensorFromProjectionMatrices(const HomogenousMatrix4& iFlippedProjectionMatrixA, const HomogenousMatrix4& iFlippedProjectionMatrixB, const HomogenousMatrix4& iFlippedProjectionMatrixC, TrifocalTensor& trifocal)
{
	/*
	 algorithm 17.12 from multiple view geometry (p.412)
							   |a_~i| a_~i means matrix A with row i omitted
	 T_i(q,r) = (-1)^(i+1) det |b_q |
							   |c_r |
	 */

	SquareMatrix4 matrix1, matrix2, matrix3;

	for (unsigned int c = 0; c < 4; c++)
	{
		for (unsigned int r = 0; r < 2; r++)
		{
			matrix1(r, c) = iFlippedProjectionMatrixA(r + 1, c); // row 1 is omitted
			matrix3(r, c) = iFlippedProjectionMatrixA(r, c); // row 3 is omitted
		}
	}

	for (unsigned int c = 0; c < 4; c++)
	{
		unsigned int row[2] = { 0, 2 };
		for (unsigned int r = 0; r < 2; r++)
		{
			matrix2(r, c) = iFlippedProjectionMatrixA(row[r], c); // row 2 is omitted
		}
	}

	for (unsigned int r = 0; r < 3; ++r)
	{
		for (unsigned int q = 0; q < 3; ++q)
		{
			for (unsigned int c = 0; c < 4; ++c)
			{
				matrix1(2, c) = iFlippedProjectionMatrixB(q, c);
				matrix1(3, c) = iFlippedProjectionMatrixC(r, c);
				matrix2(2, c) = iFlippedProjectionMatrixB(q, c);
				matrix2(3, c) = iFlippedProjectionMatrixC(r, c);
				matrix3(2, c) = iFlippedProjectionMatrixB(q, c);
				matrix3(3, c) = iFlippedProjectionMatrixC(r, c);
			}

			trifocal[0](q, r) = matrix1.determinant();
			trifocal[1](q, r) = -matrix2.determinant();
			trifocal[2](q, r) = matrix3.determinant();
		}
	}

	return true;
}

bool MultipleViewGeometry::epipoles(const TrifocalTensor& trifocal, Vector3& normedEpipole2, Vector3& normedEpipole3)
{
	return epipoles<false>(trifocal[0].transposed().data(), trifocal[1].transposed().data(), trifocal[2].transposed().data(), normedEpipole2, normedEpipole3);
}

bool MultipleViewGeometry::epipolesIF(const TrifocalTensor& trifocal, Vector3& normedEpipole2, Vector3& normedEpipole3)
{
	return epipoles<true>(trifocal[0].transposed().data(), trifocal[1].transposed().data(), trifocal[2].transposed().data(), normedEpipole2, normedEpipole3);
}

template<bool tUseIF>
bool MultipleViewGeometry::epipoles(const Scalar* const trifocal1, const Scalar* const trifocal2, const Scalar* const trifocal3, Vector3& normedEpipole2, Vector3& normedEpipole3)
{
	/**
	 * algorithm 15.1 from multiple view geometry (p.375)
	 * M(x) = sum_i( x[i] * T_i) has rank 2
	 * right null-vector of M(x) is l''
	 * left null-vector of M(x) is l'
	 * x is (1,0,0)^T, (0,1,0)^T or (0,0,1)^T
	 * e' is the common intersection of l'_i
	 * u_i is the left null-vector of T_i: u_i^T * T_i = 0^T
	 * v_i is the right null-vector of T_i: T_i * v_i = 0^T
	 * epipole e' is null-vector of U [3 x 3]: e'^T * U = 0
	 * epipole e'' is null-vector of V [3 x 3]: e''^T * V = 0
	 */

	Matrix matrixU(3, 3);
	Matrix matrixV(3, 3);

	const Matrix t1(3, 3, trifocal1);

	Matrix u, w, v, q;
	if (!t1.singularValueDecomposition(u, w, v))
		return false;

#ifdef OCEAN_INTENSIVE_DEBUG
	// this is too strict. Check will fail on noisy data
	ocean_assert(Numeric::isWeakEqualEps(w(2)));
#endif //OCEAN_INTENSIVE_DEBUG

	// U[1,:]: 1st col is null-vector of T_0 or Ut[:,1]
	for (unsigned int i = 0; i < 3; ++i)
	{
		matrixU(i, 0) = u(i, 2); // this is l'_0
		matrixV(i, 0) = v(i, 2); // this is l''_0
	}

	const Matrix t2(3, 3, trifocal2);

	if (!t2.singularValueDecomposition(u, w, v))
		return false;

#ifdef OCEAN_INTENSIVE_DEBUG
	// this is too strict. Check will fail on noisy data
	ocean_assert(Numeric::isWeakEqualEps(w(2)));
#endif //OCEAN_INTENSIVE_DEBUG

	// U[2,:]: 2nd col is null-vector of T_1
	for (unsigned int i = 0; i < 3; ++i)
	{
		matrixU(i, 1) = u(i, 2);// this is l'_1
		matrixV(i, 1) = v(i, 2);// this is l''_1
	}

	const Matrix t3(3, 3, trifocal3);

	if (!t3.singularValueDecomposition(u, w, v))
		return false;

#ifdef OCEAN_INTENSIVE_DEBUG
	// this is too strict. Check will fail on noisy data
	ocean_assert(Numeric::isWeakEqualEps(w(2)));
#endif //OCEAN_INTENSIVE_DEBUG

	// U[3,:]: 3rd col is null-vector of T_2
	for (unsigned int i = 0; i < 3; ++i)
	{
		matrixU(i, 2) = u(i, 2);// this is l'_2
		matrixV(i, 2) = v(i, 2);// this is l''_2
	}

	// epipole2.t() * [u1 u2 u3] = [0 0 0]
	if (!matrixU.qrDecomposition(q))
		return false;

	// last column is null-vector with unit norm
	normedEpipole2[0] = tUseIF ? q(0, 2) : -q(0, 2);// actually y & z must be flipped, but only changing x is the same for normalized vectors
	normedEpipole2[1] = q(1, 2);
	normedEpipole2[2] = q(2, 2);

#ifdef OCEAN_INTENSIVE_DEBUG
	Matrix debugU, debugW, debugV;
	if (matrixU.singularValueDecomposition(debugU, debugW, debugV))
	{
		ocean_assert(Vector3(q(0, 2) / q(2, 2), q(1, 2) / q(2, 2), 1).isEqual(Vector3(debugU(0, 2) / debugU(2, 2), debugU(1, 2) / debugU(2, 2), 1), Numeric::weakEps()));
	}
#endif //OCEAN_INTENSIVE_DEBUG

	// epipole3.t() * [v1 v2 v3] = [0 0 0]
	if (!matrixV.qrDecomposition(q))
		return false;

	normedEpipole3[0] = tUseIF ? q(0, 2) : -q(0, 2);// actually y & z must be flipped, but only changing x is the same for normalized vectors
	normedEpipole3[1] = q(1, 2);
	normedEpipole3[2] = q(2, 2);

#ifdef OCEAN_INTENSIVE_DEBUG
	if (matrixV.singularValueDecomposition(debugU, debugW, debugV))
	{
		ocean_assert(q.subMatrix(0, 2, 3, 1).isEqual(debugU.subMatrix(0, 2, 3, 1), Numeric::weakEps()));
	}
#endif //OCEAN_INTENSIVE_DEBUG

	return true;
}

bool MultipleViewGeometry::fundamentalMatricesIF(const TrifocalTensor& trifocal, const Vector3& iFlippedEpipole2, const Vector3& iFlippedEpipole3, SquareMatrix3& fundamental21, SquareMatrix3& fundamental31)
{
	/**
	 * algorithm 15.1 from multiple view geometry (p.375)
	 * F_21 = [e']_x * [T_1, T_2, T_3] * e''
	 * F_21 = [e'']_x * [T_1.t(), T_2.t(), T_3.t()] * e'
	 *
	 * [e']_x means cross product matrix (skewSymmetricMatrix)
	 * [T_1, T_2, T_3] * e'' means  T_1 * e'', T_2 * e'', T_3 * e''
	 */

	const SquareMatrix3 epipole2Matrix = SquareMatrix3::skewSymmetricMatrix(iFlippedEpipole2);

	fundamental21 = SquareMatrix3(
		epipole2Matrix * (trifocal[0] * iFlippedEpipole3),
		epipole2Matrix * (trifocal[1] * iFlippedEpipole3),
		epipole2Matrix * (trifocal[2] * iFlippedEpipole3)
	);

	const SquareMatrix3 epipole3Matrix = SquareMatrix3::skewSymmetricMatrix(iFlippedEpipole3);

	fundamental31 = SquareMatrix3(
		epipole3Matrix * (trifocal[0].transposed() * iFlippedEpipole2),
		epipole3Matrix * (trifocal[1].transposed() * iFlippedEpipole2),
		epipole3Matrix * (trifocal[2].transposed() * iFlippedEpipole2)
	);

	return true;
}

bool MultipleViewGeometry::cameraProjectionMatricesIF(const TrifocalTensor& trifocal, const Vector3& iFlippedNormedEpipole2, const Vector3& iFlippedNormedEpipole3, HomogenousMatrix4& iFlippedProjectionMatrix2, HomogenousMatrix4& iFlippedProjectionMatrix3)
{
	/**
	 * algorithm 15.1 from multiple view geometry (p.375)
	 * e^x in unit norm
	 * P' = [[T_1, T_2, T_3] * e'' | e']
	 * P'' = [(e''e''.t() - I) * [T_1.t(), T_2.t(), T_3.t()] * e | e'']
	 *
	 * [T_1, T_2, T_3] * e'' means  T_1 * e'', T_2 * e'', T_3 * e''
	 */

	ocean_assert(Numeric::isEqual(iFlippedNormedEpipole2.length(), 1));
	ocean_assert(Numeric::isEqual(iFlippedNormedEpipole3.length(), 1));

	const SquareMatrix3 P2_3x3(
		trifocal[0] * iFlippedNormedEpipole3,
		trifocal[1] * iFlippedNormedEpipole3,
		trifocal[2] * iFlippedNormedEpipole3
	);

	SquareMatrix3 e3e3t(iFlippedNormedEpipole3 * iFlippedNormedEpipole3[0], iFlippedNormedEpipole3 * iFlippedNormedEpipole3[1], iFlippedNormedEpipole3 * iFlippedNormedEpipole3[2]);
	e3e3t(0, 0) -= 1;
	e3e3t(1, 1) -= 1;
	e3e3t(2, 2) -= 1;

#ifdef OCEAN_DEBUG
	Matrix debugE3e3t = Matrix(3, 1, iFlippedNormedEpipole3.data()) * Matrix(1, 3, iFlippedNormedEpipole3.data()) - Matrix(3, 3, true);
	ocean_assert(debugE3e3t.isEqual(Matrix(3, 3, e3e3t.transposed().data()), Numeric::eps()));
#endif

	const SquareMatrix3 P3_3x3(
		e3e3t * (trifocal[0].transposed() * iFlippedNormedEpipole2),
		e3e3t * (trifocal[1].transposed() * iFlippedNormedEpipole2),
		e3e3t * (trifocal[2].transposed() * iFlippedNormedEpipole2)
	);

	iFlippedProjectionMatrix2.setRotation(P2_3x3);
	iFlippedProjectionMatrix2.setTranslation(iFlippedNormedEpipole2);
	iFlippedProjectionMatrix2(3, 0) = 0;
	iFlippedProjectionMatrix2(3, 1) = 0;
	iFlippedProjectionMatrix2(3, 2) = 0;
	iFlippedProjectionMatrix2(3, 3) = 1;

	iFlippedProjectionMatrix3.setRotation(P3_3x3);
	iFlippedProjectionMatrix3.setTranslation(iFlippedNormedEpipole3);
	iFlippedProjectionMatrix3(3, 0) = 0;
	iFlippedProjectionMatrix3(3, 1) = 0;
	iFlippedProjectionMatrix3(3, 2) = 0;
	iFlippedProjectionMatrix3(3, 3) = 1;

	return true;
}

bool MultipleViewGeometry::trifocalTensorLinear(const ImagePoint* points1, const ImagePoint* points2, const ImagePoint* points3, const size_t correspondences, Scalar* trifocal3x9, Matrix* matrixA)
{
	ocean_assert(points1 && points2 && points3);
	ocean_assert(correspondences >= 7);
	ocean_assert(trifocal3x9);

	/**
	 * Construct matrix A:
	 * equation 16.2 from multiple view geometry (p.393)
	 * p1_k * p2_i * p3_l * T_k(3,3) - p1_k * p3 * T_k(i,3) - p1_k * p2_i * T_k(3,l) + p1_k * T_k(i,l) = 0, for k = {1, 2, 3}, {i,l} = {1, 2}
	 * But a valid linear system is only retrieved by summation over k:
	 * Sum_k(p1_k * p2_i * p3_l * T_k(3,3) - p1_k * p3 * T_k(i,3) - p1_k * p2_i * T_k(3,l) + p1_k * T_k(i,l)) = 0, for {i,l} = {1, 2}
	 */

	Matrix matrix(4 * correspondences, 27, false);

	for (unsigned int c = 0; c < (unsigned int)(correspondences); ++c)
	{
		const ImagePoint& point1 = points1[c];
		const ImagePoint& point2 = points2[c];
		const ImagePoint& point3 = points3[c];

		for (unsigned int k = 0; k < 2; ++k)
		{
			const Scalar p1 = point1[k];
			for (unsigned int i = 0; i < 2; ++i)
			{
				const Scalar p2 = point2[i];
				for (unsigned int l = 0; l < 2; ++l)
				{
					const Scalar p3 = point3[l];

					const unsigned int rowIndex = 4 * c + 2 * i + l;
					const unsigned int index_33 = 8;
					const unsigned int index_i3 = 3 * i + 2;
					const unsigned int index_3l = 6 + l;
					const unsigned int index_il = 3 * i + l;

					matrix(rowIndex, 9 * k + index_33) = p1 * p2 * p3;
					matrix(rowIndex, 9 * k + index_i3) = -p1 * p3;
					matrix(rowIndex, 9 * k + index_3l) = -p1 * p2;
					matrix(rowIndex, 9 * k + index_il) = p1;
				}
			}
		}
		// k = 2 -> p1 = 1
		for (unsigned int i = 0; i < 2; ++i)
		{
			const Scalar p2 = point2[i];
			for (unsigned int l = 0; l < 2; ++l)
			{
				const Scalar p3 = point3[l];
				const unsigned int rowIndex = 4 * c + 2 * i + l;

				const unsigned int index_33 = 8;
				const unsigned int index_i3 = 3 * i + 2;
				const unsigned int index_3l = 6 + l;
				const unsigned int index_il = 3 * i + l;

				matrix(rowIndex, 18 + index_33) = p2 * p3;
				matrix(rowIndex, 18 + index_i3) = -p3;
				matrix(rowIndex, 18 + index_3l) = -p2;
				matrix(rowIndex, 18 + index_il) = 1;
			}
		}
	}

	/**
	 * 	solve At = 0
	 */

	Matrix u_, w_, v_;

	if (!matrix.singularValueDecomposition(u_, w_, v_))
		return false;

#ifdef OCEAN_DEBUG
	for (unsigned int n = 1; n < w_.rows(); ++n)
		ocean_assert(w_(n - 1) >= w_(n));

	ocean_assert(v_.rows() == 27);
#endif

	unsigned int eigenVectorRow = 0;
	--trifocal3x9;

	for (unsigned int i = 0; i < 3; ++i)
		for (unsigned int j = 0; j < 9; ++j)
		{
			*(++trifocal3x9) = v_(eigenVectorRow++, 26);
		}

#ifdef OCEAN_INTENSIVE_DEBUG
	Matrix test = matrix * v_.subMatrix(0, 0, v_.rows(), 26);
	Scalar norm = test.norm();
#endif //OCEAN_INTENSIVE_DEBUG

	if (matrixA)
		*matrixA = std::move(matrix);

	return true;
}

bool MultipleViewGeometry::trifocalTensorNormalizedLinear(const ImagePoint* points1, const ImagePoint* points2, const ImagePoint* points3, const size_t correspondences, Scalar* trifocal3x9, Matrix* matrixA)
{
	ocean_assert(points1 && points2 && points3);
	ocean_assert(trifocal3x9);
	ocean_assert(correspondences >= 7);

	/**
	 * algorithm 16.1 from multiple view geometry (p.394)
	 */

	/**
	 * I. & II. Points normalization:
	 */

	ImagePoints normalizationPoints1(correspondences);
	ImagePoints normalizationPoints2(correspondences);
	ImagePoints normalizationPoints3(correspondences);

	memcpy(normalizationPoints1.data(), points1, sizeof(ImagePoint) * correspondences);
	memcpy(normalizationPoints2.data(), points2, sizeof(ImagePoint) * correspondences);
	memcpy(normalizationPoints3.data(), points3, sizeof(ImagePoint) * correspondences);

	const SquareMatrix3 normalization1 = Normalization::calculateNormalizedPoints(normalizationPoints1.data(), correspondences);
	const SquareMatrix3 normalization2 = Normalization::calculateNormalizedPoints(normalizationPoints2.data(), correspondences);
	const SquareMatrix3 normalization3 = Normalization::calculateNormalizedPoints(normalizationPoints3.data(), correspondences);

#ifdef OCEAN_INTENSIVE_DEBUG
	ImagePoint check1(normalization1(0, 0) * points1[0][0] + normalization1(0, 1) * points1[0][1] + normalization1(0, 2), normalization1(1, 0) * points1[0][0] + normalization1(1, 1) * points1[0][1] + normalization1(1, 2));
	ocean_assert(check1.isEqual(normalizationPoints1[0], Numeric::eps()));

	ImagePoint check2 = normalization2 *  points2[0];
	ocean_assert(check2.isEqual(normalizationPoints2[0], Numeric::eps()));
#endif //OCEAN_INTENSIVE_DEBUG

	/**
	 * III. compute trifocal tensor of normalized data
	 */

	Scalar trifocalTensor3x9[27];
	if (!trifocalTensorLinear(normalizationPoints1.data(), normalizationPoints2.data(), normalizationPoints3.data(), correspondences, trifocalTensor3x9, matrixA))
		return false;

	/**
	 * IV. compute trifocal tensor corresponding to original data
	 */

	const SquareMatrix3 invertedNormalization2 = normalization2.inverted();
	const SquareMatrix3 invertedNormalization3 = normalization3.inverted();

	for (unsigned int k = 0; k < 3; ++k)
	{
		for (unsigned int j = 0; j < 3; ++j)
		{
			Scalar sumR0(0);
			Scalar sumR1(0);
			Scalar sumR2(0);
			for (unsigned int r = 0; r < 3; ++r)
			{
				Scalar sumS(0);
				for (unsigned int s = 0; s < 3; ++s)
				{
					Scalar sumT(0);
					for (unsigned int t = 0; t < 3; ++t)
					{
						sumT += invertedNormalization3(k, t) * trifocalTensor3x9[9 * r + 3 * s + t];
					}
					sumS += invertedNormalization2(j, s) * sumT;
				}
				sumR0 += normalization1(r, 0) * sumS;
				sumR1 += normalization1(r, 1) * sumS;
				sumR2 += normalization1(r, 2) * sumS;
			}
			trifocal3x9[j * 3u + k] = sumR0;
			trifocal3x9[9u + j * 3u + k] = sumR1;
			trifocal3x9[18u + j * 3u + k] = sumR2;
		}
	}

	return true;
}

Scalar MultipleViewGeometry::errorMatrix(const TrifocalTensor& trifocal, const ImagePoint* points1, const ImagePoint* points2, const ImagePoint* points3, const size_t correspondences, SquareMatrix3* error)
{
	ocean_assert(points1 && points2 && points3);
	ocean_assert(correspondences > 0u);

	if (error)
	{
		*error = SquareMatrix3(false);
	}

	Scalar absError(0);

	for (unsigned int p = 0; p < correspondences; ++p)
	{
		const ImagePoint& point1 = points1[p];
		const ImagePoint& point2 = points2[p];
		const ImagePoint& point3 = points3[p];

		SquareMatrix3 matrix1;
		for (unsigned int c = 0; c < 3; ++c)
		{
			for (unsigned int r = 0; r < 3; ++r)
			{
				matrix1(r, c) = point1.x() * trifocal[0](r, c) + point1.y() * trifocal[1](r, c) + trifocal[2](r, c);
			}
		}

		const SquareMatrix3 crossMatrix2 = SquareMatrix3::skewSymmetricMatrix(Vector3(point2, 1));
		const SquareMatrix3 crossMatrix3 = SquareMatrix3::skewSymmetricMatrix(Vector3(point3, 1));

		const SquareMatrix3 currentErrorMatrix = (crossMatrix2 * matrix1) * crossMatrix3;

		for (unsigned int i = 0; i < 9; ++i)
		{
			absError += Numeric::abs(currentErrorMatrix(i));
		}

		if (error)
		{
			*error += currentErrorMatrix;
		}
	}

	if (error)
	{
		*error = *error * (Scalar(1) / Scalar(correspondences));
	}

	return absError / Scalar(correspondences);
}

bool MultipleViewGeometry::calculateProjectiveBasisTransform(const Vector2& imagePointForTargetPoint100, const Vector2& imagePointForTargetPoint010, const Vector2& imagePointForTargetPoint001, const Vector2& imagePointForTargetPoint111, SquareMatrix3& baseTransformation)
{
	// first we simply define the new coordinate system based on the first three points

	const SquareMatrix3 homogenImagePoints123(Vector3(imagePointForTargetPoint100, 1), Vector3(imagePointForTargetPoint010, 1), Vector3(imagePointForTargetPoint001, 1));

	ocean_assert(homogenImagePoints123 * Vector3(1, 0, 0) == Vector3(imagePointForTargetPoint100, 1));
	ocean_assert(homogenImagePoints123 * Vector3(0, 1, 0) == Vector3(imagePointForTargetPoint010, 1));
	ocean_assert(homogenImagePoints123 * Vector3(0, 0, 1) == Vector3(imagePointForTargetPoint001, 1));

	// now we have to ensure that the base transformation matches with the fourth point (by scaling the current transformation)
	// due to the scaling the finial mapping is valid up to a scaling factor only

	SquareMatrix3 invHomogenImagePoints123;
	if (!homogenImagePoints123.invert(invHomogenImagePoints123))
		return false;

	const Vector3 scaleVector = invHomogenImagePoints123 * Vector3(imagePointForTargetPoint111, 1);
	const SquareMatrix3 scaleMatrix(scaleVector);
	if (!(homogenImagePoints123 * scaleMatrix).invert(baseTransformation))
		return false;

#ifdef OCEAN_INTENSIVE_DEBUG
	{
		const Vector3 point100 = baseTransformation * Vector3(imagePointForTargetPoint100, 1);
		const Vector3 point010 = baseTransformation * Vector3(imagePointForTargetPoint010, 1);
		const Vector3 point001 = baseTransformation * Vector3(imagePointForTargetPoint001, 1);
		const Vector3 point111 = baseTransformation * Vector3(imagePointForTargetPoint111, 1);

		ocean_assert(point100.isParallel(Vector3(1, 0, 0)));
		ocean_assert(point010.isParallel(Vector3(0, 1, 0)));
		ocean_assert(point001.isParallel(Vector3(0, 0, 1)));
		ocean_assert(point111.isParallel(Vector3(1, 1, 1)));
	}
#endif

	return true;
}

bool AutoCalibration::findCommonIntrinsicsFromProjectionMatricesIF(const ConstIndexedAccessor<HomogenousMatrix4>& iFlippedProjectionMatrices, SquareMatrix3& cameraIntrinsics, SquareMatrix4* Q, SquareMatrix3* omega)
{
	size_t views = iFlippedProjectionMatrices.size();

	ocean_assert(views >= 3);

	/**
	 * w* = (K * K^T) = P_i * Q* *P^T_i ==  P_j * Q* *P^T_j, for all i,j
	 *
	 *						|0 1 2 3|
	 * Q* is symmetric, Q*= |1 4 5 6|
	 *						|2 5 7 8|
	 *						|3 6 8 9|
	 *
	 * As the intrinsic parameters do not change between individual views we have w*_i = s * w*_j (both omegas are identical up to a scale factor s), so that we know:
	 * w*_i(0,0) / w*_j(0,0) =  w*_i(1,1) / w*_j(1,1) = w*_i(0,1) / w*_j(0,1) = w*_i(0,2) / w*_j(0,2) = w*_i(1,2) / w*_j(1,2) = constant
	 *
	 * or:
	 *
	 * w*_i(0,0) - w*_j(0,0) = 0
	 */

	// create linear system for solving Q* via A * q = 0
	const size_t noEquations = (views - 1) * 5;

	Matrix linearSystem(noEquations, 10);
	Scalar* linearSystemData = linearSystem.data();

	const HomogenousMatrix4& Pj = iFlippedProjectionMatrices[0];

	for (size_t i = 1; i < views; ++i)
	{
		const HomogenousMatrix4& Pi = iFlippedProjectionMatrices[i];
		{
			//  w*_i(0, 0) = w*_j(0, 0)
			const Matrix wi = createLinearSystemForAbsoluteDualQuadric(0, 0, Pi);
			const Matrix wj = createLinearSystemForAbsoluteDualQuadric(0, 0, Pj);
			const Matrix differenceW = wj - wi;
			memcpy(linearSystemData, differenceW.data(), sizeof(Scalar) * 10);
			linearSystemData += 10;
		}
		{
			//  w*_i(1, 1) = w*_j(1, 1)
			const Matrix wi = createLinearSystemForAbsoluteDualQuadric(1, 1, Pi);
			const Matrix wj = createLinearSystemForAbsoluteDualQuadric(1, 1, Pj);
			const Matrix differenceW = wj - wi;
			memcpy(linearSystemData, differenceW.data(), sizeof(Scalar) * 10);
			linearSystemData += 10;
		}
		{
			//  w*_i(0, 1) = w*_j(0, 1)
			const Matrix wi = createLinearSystemForAbsoluteDualQuadric(0, 1, Pi);
			const Matrix wj = createLinearSystemForAbsoluteDualQuadric(0, 1, Pj);
			const Matrix differenceW = wj - wi;
			memcpy(linearSystemData, differenceW.data(), sizeof(Scalar) * 10);
			linearSystemData += 10;
		}
		{
			//  w*_i(0, 2) = w*_j(0, 2)
			const Matrix wi = createLinearSystemForAbsoluteDualQuadric(0, 2, Pi);
			const Matrix wj = createLinearSystemForAbsoluteDualQuadric(0, 2, Pj);
			const Matrix differenceW = wj - wi;
			memcpy(linearSystemData, differenceW.data(), sizeof(Scalar) * 10);
			linearSystemData += 10;
		}
		{
			//  w*_i(1, 2) = w*_j(1, 2)
			const Matrix wi = createLinearSystemForAbsoluteDualQuadric(1, 2, Pi);
			const Matrix wj = createLinearSystemForAbsoluteDualQuadric(1, 2, Pj);
			const Matrix differenceW = wj - wi;
			memcpy(linearSystemData, differenceW.data(), sizeof(Scalar) * 10);
			linearSystemData += 10;
		}
	}

	// solve for q
	Matrix u, w, v;
	if (!linearSystem.singularValueDecomposition(u, w, v))
		return false;

	SquareMatrix4 symmetricQ;
	symmetricQ(0, 0) = v(0, 9);
	symmetricQ(1, 0) = v(1, 9);
	symmetricQ(0, 1) = v(1, 9);
	symmetricQ(2, 0) = v(2, 9);
	symmetricQ(0, 2) = v(2, 9);
	symmetricQ(3, 0) = v(3, 9);
	symmetricQ(0, 3) = v(3, 9);
	symmetricQ(1, 1) = v(4, 9);
	symmetricQ(1, 2) = v(5, 9);
	symmetricQ(2, 1) = v(5, 9);
	symmetricQ(1, 3) = v(6, 9);
	symmetricQ(3, 1) = v(6, 9);
	symmetricQ(2, 2) = v(7, 9);
	symmetricQ(2, 3) = v(8, 9);
	symmetricQ(3, 2) = v(8, 9);
	symmetricQ(3, 3) = v(9, 9);

	if (Q)
		*Q = symmetricQ;

	Matrix Pt = Matrix(4, 3);
	memcpy(Pt.data(), Pj.data(), sizeof(Scalar) * 3);
	memcpy(Pt.data() + 3, Pj.data() + 4, sizeof(Scalar) * 3);
	memcpy(Pt.data() + 6, Pj.data() + 8, sizeof(Scalar) * 3);
	memcpy(Pt.data() + 9, Pj.data() + 12, sizeof(Scalar) * 3);

	const Matrix omegaMatrix(Pt.transposedMultiply(Matrix(4, 4, symmetricQ.data())) * Pt);
	const SquareMatrix3 conic(omegaMatrix.data(), true);

	if (!upperTriangleCholeskyDecomposition(conic, cameraIntrinsics))
		return false;

	if (omega)
		*omega = conic;

	return true;
}

bool AutoCalibration::findCommonIntrinsicsFromProjectionMatricesIF(const ConstIndexedAccessor<HomogenousMatrix4>& iFlippedProjectionMatrices, const unsigned int imageWidth, const unsigned int imageHeight, SquareMatrix3& cameraIntrinsics, SquareMatrix4* Q, SquareMatrix3* omega)
{
	ocean_assert(iFlippedProjectionMatrices.size() >= 3);

	const size_t views = iFlippedProjectionMatrices.size();

	if (views < 3)
		return false;

	/**
	 * w* = (K * K^T) = P_i * Q* *P^T_i ==  P_j * Q* *P^T_j, for all i,j
	 *
	 *						|0 1 2 3|
	 * Q* is symmetric, Q*= |1 4 5 6|
	 *						|2 5 7 8|
	 *						|3 6 8 9|
	 *
	 * if known that skew s = 0 and c_x,y, shift image coordinates by cy,cx, so that (c_x,c_y) = (0,0)
	 *
	 *		| f_x^2	  0   0 |
	 * w* =	|   0	f_y^2 0 |
	 *		|   0     0   1 |
	 *
	 * w*(2, 0) = w*(0, 2) = [P_i * Q* * P^T_i](2, 0) = 0 -> < P_i(2, :) * Q*, P_i(0, :) > = 0
	 * w*(2, 1) = w*(1, 2) = [P_i * Q* * P^T_i](2, 1) = 0 -> < P_i(2, :) * Q*, P_i(1, :) > = 0
	 * w*(1, 0) = w*(0, 1) = [P_i * Q* * P^T_i](1, 0) = 0 -> < P_i(1, :) * Q*, P_i(0, :) > = 0
	 *
	 * f_x1 = f_y1 = f_x,j = f_y,j
	 * w*_i(0,0) - w*_j(1,1) = 0
	 */

	HomogenousMatrices4 normedProjectionsIF(views);
	if (!transformProjectionsZeroPrinciplePoint(iFlippedProjectionMatrices, imageWidth, imageHeight, normedProjectionsIF.data()))
		return false;

	// create linear system for solving Q* via A * q = 0
	const size_t noEquations = views * 4 - 1;

	Matrix linearSystem(noEquations, 10);
	Scalar* linearSystemData = linearSystem.data();

	const HomogenousMatrix4& Pj = normedProjectionsIF[0];

	for (size_t i = 1; i < views; ++i)
	{
		const HomogenousMatrix4& Pi = normedProjectionsIF[i];

		//  w*_i(0,0) = w*_j(1,1)
		const Matrix wi = createLinearSystemForAbsoluteDualQuadric(0, 0, Pi);
		const Matrix wj = createLinearSystemForAbsoluteDualQuadric(1, 1, Pj);

		for (unsigned int n = 0u; n < 10u; n++)
		{
			*linearSystemData++ = wj(0, n) - wj(0, n);
		}
	}

	for (size_t i = 0; i < views; ++i)
	{
		const HomogenousMatrix4& Pi = normedProjectionsIF[i];

		//  < P_i(2,:) * Q*, P_i(0,:) > = 0
		const Matrix w02 = createLinearSystemForAbsoluteDualQuadric(0, 2, Pi);
		for (unsigned int n = 0u; n < 10u; n++)
		{
			*linearSystemData++ = w02(0, n);
		}

		// < P_i(2,:) * Q*, P_i(1,:) > = 0
		const Matrix w12 = createLinearSystemForAbsoluteDualQuadric(1, 2, Pi);
		for (unsigned int n = 0u; n < 10u; n++)
		{
			*linearSystemData++ = w12(0, n);
		}

		//  < P_i(1,:) * Q*, P_i(0,:) > = 0
		const Matrix w01 = createLinearSystemForAbsoluteDualQuadric(0, 1, Pi);
		for (unsigned int n = 0u; n < 10u; n++)
		{
			*linearSystemData++ = 10 * w01(0, n);
		}
	}

	// solve for q
	Matrix u, w, v;
	if (!linearSystem.singularValueDecomposition(u, w, v))
		return false;

	SquareMatrix4 symmetricQ;
	symmetricQ(0, 0) = v(0, 9);
	symmetricQ(1, 0) = v(1, 9);
	symmetricQ(0, 1) = v(1, 9);
	symmetricQ(2, 0) = v(2, 9);
	symmetricQ(0, 2) = v(2, 9);
	symmetricQ(3, 0) = v(3, 9);
	symmetricQ(0, 3) = v(3, 9);
	symmetricQ(1, 1) = v(4, 9);
	symmetricQ(1, 2) = v(5, 9);
	symmetricQ(2, 1) = v(5, 9);
	symmetricQ(1, 3) = v(6, 9);
	symmetricQ(3, 1) = v(6, 9);
	symmetricQ(2, 2) = v(7, 9);
	symmetricQ(2, 3) = v(8, 9);
	symmetricQ(3, 2) = v(8, 9);
	symmetricQ(3, 3) = v(9, 9);

	if (Q)
		*Q = symmetricQ;

	Matrix Pt = Matrix(4, 3);
	memcpy(Pt.data(), iFlippedProjectionMatrices[0].data(), sizeof(Scalar) * 3);
	memcpy(Pt.data() + 3, iFlippedProjectionMatrices[0].data() + 4, sizeof(Scalar) * 3);
	memcpy(Pt.data() + 6, iFlippedProjectionMatrices[0].data() + 8, sizeof(Scalar) * 3);
	memcpy(Pt.data() + 9, iFlippedProjectionMatrices[0].data() + 12, sizeof(Scalar) * 3);

	const Matrix omegaMatrix(Pt.transposedMultiply(Matrix(4, 4, symmetricQ.data())) * Pt);
	const SquareMatrix3 conic(omegaMatrix.data(), true);

	if (!upperTriangleCholeskyDecomposition(conic, cameraIntrinsics))
		return false;

	if (omega)
		*omega = conic;

	return true;
}

bool AutoCalibration::determineAbsoluteDualQuadricLinearIF(const ConstIndexedAccessor<HomogenousMatrix4>& iFlippedProjectionMatrices, SquareMatrix4& matrixQ, const unsigned int imageWidth, const unsigned int imageHeight, bool equalFxFy)
{
	ocean_assert(iFlippedProjectionMatrices.size() >= 3);
	ocean_assert(imageWidth != 0u && imageHeight != 0u);

	const size_t views = iFlippedProjectionMatrices.size();

	/**
	 * w*_i = (K_i * K^T_i) = P_i * Q* *P^T_i
	 *
	 *						|0 1 2 3|
	 * Q* is symmetric, Q*= |1 4 5 6|
	 *						|2 5 7 8|
	 *						|3 6 8 9|
	 *
	 * we do not expect any skew (s = 0) and we shift the coordinate system by the principal point (m_x, m_y)
	 *
	 *		| f_x^2	  0    0 |
	 * w* =	|   0	f_y^2  0 |
	 *		|   0     0    1 |
	 *
	 * w*(2, 0) = w*(0, 2) = [P_i * Q* * P^T_i](2, 0) = 0      ->      < P_i(2, :) * Q*,     P_i(0, :) >= 0
	 * w*(2, 1) = w*(1, 2) = [P_i * Q* * P^T_i](2, 1) = 0      ->      < P_i(2, :) * Q*,     P_i(1, :) >= 0
	 * w*(1, 0) = w*(0, 1) = [P_i * Q* * P^T_i](1, 0) = 0      ->      < P_i(1, :) * Q*,     P_i(0, :) >= 0
	 */

	HomogenousMatrices4 normedProjectionsIF(views);
	if (!transformProjectionsZeroPrinciplePoint(iFlippedProjectionMatrices, imageWidth, imageHeight, normedProjectionsIF.data()))
		return false;

	// create linear system for solving Q* via A * q = 0

	size_t noEquations = 3 * views;

	if (equalFxFy)
		noEquations += views;

	Matrix linearSystem(noEquations, 10);
	Scalar* linearSystemData = linearSystem.data();

	for (size_t i = 0; i < views; i++)
	{
		const HomogenousMatrix4& P = normedProjectionsIF[i];

		//  < P_i(2,:) * Q*, P_i(0,:) > = 0
		const Matrix w02 = createLinearSystemForAbsoluteDualQuadric(0, 2, P);
		for (unsigned int n = 0u; n < 10u; n++)
		{
			*linearSystemData = 2 * w02(0, n);
			++linearSystemData;
		}

		// < P_i(2,:) * Q*, P_i(1,:) > = 0
		const Matrix w12 = createLinearSystemForAbsoluteDualQuadric(1, 2, P);
		for (unsigned int n = 0u; n < 10u; n++)
		{
			*linearSystemData = 2 * w12(0, n);
			++linearSystemData;
		}

		//  < P_i(1,:) * Q*, P_i(0,:) > = 0
		const Matrix w01 = createLinearSystemForAbsoluteDualQuadric(0, 1, P);
		for (unsigned int n = 0u; n < 10u; n++)
		{
			*linearSystemData = 20 * w01(0, n);
			++linearSystemData;
		}

		if (equalFxFy)
		{
			const Matrix w00 = createLinearSystemForAbsoluteDualQuadric(0, 0, P);
			const Matrix w11 = createLinearSystemForAbsoluteDualQuadric(1, 1, P);
			//  w*_i(0,0) = w*_j(1,1)

			for (unsigned int n = 0u; n < 10u; n++)
			{
				*linearSystemData = w00(0, n) - w11(0, n);
				++linearSystemData;
			}
		}
	}

	// solve for q
	Matrix u, w, v;
	if (!linearSystem.singularValueDecomposition(u, w, v))
		return false;

	matrixQ(0, 0) = v(0, 9);
	matrixQ(1, 0) = v(1, 9);
	matrixQ(0, 1) = v(1, 9);
	matrixQ(2, 0) = v(2, 9);
	matrixQ(0, 2) = v(2, 9);
	matrixQ(3, 0) = v(3, 9);
	matrixQ(0, 3) = v(3, 9);
	matrixQ(1, 1) = v(4, 9);
	matrixQ(1, 2) = v(5, 9);
	matrixQ(2, 1) = v(5, 9);
	matrixQ(1, 3) = v(6, 9);
	matrixQ(3, 1) = v(6, 9);
	matrixQ(2, 2) = v(7, 9);
	matrixQ(2, 3) = v(8, 9);
	matrixQ(3, 2) = v(8, 9);
	matrixQ(3, 3) = v(9, 9);

	return true;
}

bool AutoCalibration::intrinsicsFromAbsoluteDualQuadricIF(const SquareMatrix4& symmetricQ, const ConstIndexedAccessor<HomogenousMatrix4>& iFlippedProjectionMatrices, SquareMatrix3* intrinsics)
{
	ocean_assert(symmetricQ.isSymmetric(Numeric::weakEps()));
	ocean_assert(intrinsics);

	for (size_t i = 0; i < iFlippedProjectionMatrices.size(); i++)
	{
		const HomogenousMatrix4& projectionIF = iFlippedProjectionMatrices[i];
		ocean_assert(projectionIF.isValid());

		// P * Q * P^T = K * K^T
		const SquareMatrix3 omega((SquareMatrix4&)projectionIF * symmetricQ * projectionIF.transposed());

		if (!upperTriangleCholeskyDecomposition(omega, intrinsics[i]))
			return false;
	}

	return true;
}

bool AutoCalibration::transformProjectiveToMetricIF(const SquareMatrix4& Q, const ConstIndexedAccessor<HomogenousMatrix4>& iFlippedProjectionMatrices, HomogenousMatrix4* iFlippedMetricProjectionMatrices, SquareMatrix4* transformation)
{
	ocean_assert(iFlippedMetricProjectionMatrices);

	const size_t views = iFlippedProjectionMatrices.size();

	SquareMatrix4 metricTransformation;
	if (!getTransformProjectiveToMetricMatrix(Q, metricTransformation))
		return false;

	if (transformation)
		*transformation = metricTransformation;

	for (size_t j = 0; j < views; j++)
	{
		const HomogenousMatrix4& Pj = iFlippedProjectionMatrices[j];

		SquareMatrix4 metricProj = SquareMatrix4(Pj) * metricTransformation;
		metricProj(3, 0) = 0;
		metricProj(3, 1) = 0;
		metricProj(3, 2) = 0;
		metricProj(3, 3) = 1;
		iFlippedMetricProjectionMatrices[j] = HomogenousMatrix4(metricProj);

#ifdef OCEAN_DEBUG
		Matrix Pt = Matrix(4, 3);
		memcpy(Pt.data(), Pj.data(), sizeof(Scalar) * 3);
		memcpy(Pt.data() + 3, Pj.data() + 4, sizeof(Scalar) * 3);
		memcpy(Pt.data() + 6, Pj.data() + 8, sizeof(Scalar) * 3);
		memcpy(Pt.data() + 9, Pj.data() + 12, sizeof(Scalar) * 3);

		Matrix transformed = Pt.transposedMultiply(Matrix(4, 4, metricTransformation.transposed().data()));
#endif
	}

	return true;
}

bool AutoCalibration::metricProjectionMatricesToPosesIF(const ConstIndexedAccessor<HomogenousMatrix4>& metricProjectionsIF, const SquareMatrix3& intrinsic, HomogenousMatrix4* posesIF)
{
	if (metricProjectionsIF.isEmpty())
		return true;

	ocean_assert(posesIF && !intrinsic.isSingular());

	/**
	 * P = K * [R t]
	 * [Rt] = K^-1 * P
	 */

	SquareMatrix3 invIntrinsic;
	if (!intrinsic.invert(invIntrinsic))
	{
		ocean_assert(false && "Invalid intrinsic matrix!");
		return false;
	}

	const HomogenousMatrix4 invInvIntrinsic4(invIntrinsic);

	for (size_t i = 0; i < metricProjectionsIF.size(); i++)
	{
		ocean_assert(metricProjectionsIF[i].isValid());

		HomogenousMatrix4 poseIF(invInvIntrinsic4 * metricProjectionsIF[i]);

		// now we have a pose with arbitrary scale, however we need a orthonormal base for the rotation matrix

		const Scalar lengthX = poseIF.xAxis().length();
		const Scalar lengthY = poseIF.yAxis().length();
		const Scalar lengthZ = poseIF.zAxis().length();

		const Scalar averageScale = (lengthX + lengthY + lengthZ) * Scalar(0.333333333333333333333333333);

		if (Numeric::isEqualEps(averageScale))
			return false;

		const Scalar invAverageScale = Scalar(1) / averageScale;

		for (unsigned int n = 0u; n < 15u; ++n)
			poseIF[n] *= invAverageScale;

#ifdef OCEAN_DEBUG
		{
			ocean_assert(poseIF.isValid());

			const Vector3 xAxis = poseIF.xAxis();
			const Vector3 yAxis = poseIF.yAxis();
			const Vector3 zAxis = poseIF.zAxis();

			const Scalar xLength = xAxis.length();
			const Scalar yLength = yAxis.length();
			const Scalar zLength = zAxis.length();

			OCEAN_SUPPRESS_UNUSED_WARNING(xLength);
			OCEAN_SUPPRESS_UNUSED_WARNING(yLength);
			OCEAN_SUPPRESS_UNUSED_WARNING(zLength);

			const Scalar xyAngle = Numeric::rad2deg(xAxis.angle(yAxis));
			const Scalar xzAngle = Numeric::rad2deg(xAxis.angle(zAxis));
			const Scalar yzAngle = Numeric::rad2deg(yAxis.angle(zAxis));

			OCEAN_SUPPRESS_UNUSED_WARNING(xyAngle);
			OCEAN_SUPPRESS_UNUSED_WARNING(xzAngle);
			OCEAN_SUPPRESS_UNUSED_WARNING(yzAngle);

			// **TODO** the following check fails very frequently so that the general implementation/algorithm seems to have major issues
			//ocean_assert(Numeric::isEqual(xyAngle, 90, 0.5) && Numeric::isEqual(xzAngle, 90, 0.5) && Numeric::isEqual(yzAngle, 90, 0.5));
			//ocean_assert(Numeric::isEqual(xLength, 1, Scalar(0.01)) && Numeric::isEqual(yLength, 1, Scalar(0.01)) && Numeric::isEqual(zLength, 1, Scalar(0.01)));
		}
#endif

		posesIF[i] = poseIF;
	}

	return true;
}

bool AutoCalibration::transformProjectionsZeroPrinciplePoint(const ConstIndexedAccessor<HomogenousMatrix4>& iFlippedProjectionMatrices, const unsigned int imageWidth, const unsigned int imageHeight, HomogenousMatrix4* iFlippedNormalizedProjectionMatrices, SquareMatrix3* backTransformation)
{
	ocean_assert(iFlippedProjectionMatrices.size() >= 1);
	ocean_assert(imageWidth != 0u && imageHeight != 0u);
	ocean_assert(iFlippedNormalizedProjectionMatrices);

	const size_t views = iFlippedProjectionMatrices.size();
	const unsigned int width_2 = imageWidth / 2u;
	const unsigned int height_2 = imageHeight / 2u;

	const SquareMatrix3 transformation(Scalar(imageWidth + imageHeight), 0, 0, 0, Scalar(imageWidth + imageHeight), 0, Scalar(width_2), Scalar(height_2), 1);

	if (backTransformation)
		*backTransformation = transformation;

	SquareMatrix3 invTransformation;
	if (!transformation.invert(invTransformation))
		return false;

	const HomogenousMatrix4 normalization(invTransformation);

	for (size_t n = 0; n < views; n++)
		iFlippedNormalizedProjectionMatrices[n] = normalization * iFlippedProjectionMatrices[n];

	return true;
}

bool AutoCalibration::getTransformProjectiveToMetricMatrix(const SquareMatrix4& symmetricQ, SquareMatrix4& transformation)
{
	/**
	 * P_metric = P * H, X_metrix = H^1 * H
	 * Q* = H * Q*_metric * H^T
	 */

	/*
	Vector4 eigenValues;
	SquareMatrix4 eigenVectors;
	if (!symmetricQ.eigenSystem(eigenValues, eigenVectors)) // as SquareMatrix4::eigenSystem() is not implemented yet
	*/
	Matrix eigenValues, eigenVectors;
	if (!Matrix(4, 4, symmetricQ.data()).eigenSystem(eigenValues, eigenVectors))
		return false;

	std::multimap<Scalar, Index32> sort;
	for (unsigned int i = 0u; i < 4u; i++)
	{
		if (eigenValues(i) < 0)
			eigenValues(i) *= -1;

		sort.insert(std::make_pair(Numeric::sqrt(eigenValues(i)), Index32(i)));
	}

	SquareMatrix4 sortedEigenValues(false);
	SquareMatrix4 sortedEigenVectors;
	unsigned int i = 0u;

	for (std::multimap<Scalar, Index32>::const_reverse_iterator it = sort.crbegin(); it != sort.crend(); ++it)
	{
		const Index32 toCopyFrom = it->second;
		sortedEigenValues(i, i) = it->first;

		for (unsigned int j = 0u; j < 4u; j++)
		{
			sortedEigenVectors(j, i) = eigenVectors(j, toCopyFrom);
		}

		++i;
	}
	// set last eigenValue to one|zero for diagonal matrix?

	// H = EigVec * sqrt(EigVal)
	transformation = sortedEigenVectors * sortedEigenValues;

	return true;
}

bool AutoCalibration::upperTriangleCholeskyDecomposition(const SquareMatrix3& omega, SquareMatrix3& cameraIntrinsic)
{
	ocean_assert(omega.isSymmetric(Numeric::weakEps()));

	/**
	 * To compute the upper-triangular Cholesky, we flip (rotation by 180 degree) the entries of the input matrix,
	 * compute the lower-triangular Cholesky, and then we flip back the result:
	 *
	 * | A B C |      | I H G |
	 * | D E F |  ->  | F E D |
	 * | G H I |      | C B A |
	 */

	Matrix flippedOmega(3, 3);
	for (unsigned int r = 0; r < 3u; ++r)
		for (unsigned int c = 0; c < 3u; ++c)
			flippedOmega(r, c) = omega(2u - r, 2u - c);

	Matrix flippedK;
	if (!flippedOmega.choleskyDecomposition(flippedK))
		return false;

	for (unsigned int r = 0; r < 3u; ++r)
		for (unsigned int c = 0; c < 3u; ++c)
			cameraIntrinsic(r, c) = flippedK(2u - r, 2u - c);

	// we resolve sign ambiguities assuming a positive diagonal

	for (unsigned int c = 0u; c < 3u; ++c)
		if (cameraIntrinsic(c, c) < 0)
		{
			for (unsigned int r = 0u; r < 3u; ++r)
				cameraIntrinsic(r, c) *= -1;
		}

	// we finally normalize the intrinsic matrix w.r.t. the lower right element

	if (Numeric::isEqualEps(cameraIntrinsic(2, 2)))
		return false;

#ifdef OCEAN_DEBUG
	const SquareMatrix3 debugOmega = cameraIntrinsic * cameraIntrinsic.transposed();
	ocean_assert(debugOmega.isEqual(omega, Scalar(0.001)));
#endif

	cameraIntrinsic *= Scalar(1) / cameraIntrinsic(2, 2);

	return true;
}

Matrix AutoCalibration::createLinearSystemForAbsoluteDualQuadric(const unsigned int omegaRowIndex, const unsigned int omegaColumnIndex, const HomogenousMatrix4& iFlippedProjectionMatrix)
{
	ocean_assert(omegaRowIndex <= 2);
	ocean_assert(omegaColumnIndex <= 2);

	const HomogenousMatrix4& P = iFlippedProjectionMatrix;

	Matrix linearSystem(1, 10);
	linearSystem(0, 0) = P(omegaRowIndex, 0) * P(omegaColumnIndex, 0);
	linearSystem(0, 1) = P(omegaRowIndex, 1) * P(omegaColumnIndex, 0) +  P(omegaRowIndex, 0) * P(omegaColumnIndex, 1);
	linearSystem(0, 2) = P(omegaRowIndex, 2) * P(omegaColumnIndex, 0) +  P(omegaRowIndex, 0) * P(omegaColumnIndex, 2);
	linearSystem(0, 3) = P(omegaRowIndex, 3) * P(omegaColumnIndex, 0) +  P(omegaRowIndex, 0) * P(omegaColumnIndex, 3);
	linearSystem(0, 4) = P(omegaRowIndex, 1) * P(omegaColumnIndex, 1);
	linearSystem(0, 5) = P(omegaRowIndex, 2) * P(omegaColumnIndex, 1) +  P(omegaRowIndex, 1) * P(omegaColumnIndex, 2);
	linearSystem(0, 6) = P(omegaRowIndex, 3) * P(omegaColumnIndex, 1) +  P(omegaRowIndex, 1) * P(omegaColumnIndex, 3);
	linearSystem(0, 7) = P(omegaRowIndex, 2) * P(omegaColumnIndex, 2);
	linearSystem(0, 8) = P(omegaRowIndex, 3) * P(omegaColumnIndex, 2) +  P(omegaRowIndex, 2) * P(omegaColumnIndex, 3);
	linearSystem(0, 9) = P(omegaRowIndex, 3) * P(omegaColumnIndex, 3);

#ifdef OCEAN_DEBUG
	Matrix debugLinearSystem(1, 10);
	unsigned int debugCounter = 0;
	for (unsigned int i = 0; i < 4; ++i)
	{
		for (unsigned int j = i; j < 4; ++j)
		{
			SquareMatrix4 debugQ(false);
			debugQ(i, j) = 1;
			debugQ(j, i) = 1;

			SquareMatrix4 debugOmega = SquareMatrix4(P) * debugQ * SquareMatrix4(P.transposed());
			debugLinearSystem(0, debugCounter++) = debugOmega(omegaRowIndex, omegaColumnIndex);
		}
	}

	ocean_assert(linearSystem == debugLinearSystem);
#endif

	return linearSystem;
}

}

}
