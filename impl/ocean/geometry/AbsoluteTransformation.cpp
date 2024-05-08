// Copyright (c) Meta Platforms, Inc. and affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "ocean/geometry/AbsoluteTransformation.h"

#include "ocean/base/Median.h"

#include "ocean/math/Matrix.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/Rotation.h"
#include "ocean/math/Vector4.h"

namespace Ocean
{

namespace Geometry
{

bool AbsoluteTransformation::calculateTransformation(const Vector3* left, const Vector3* right, const size_t correspondences, HomogenousMatrix4& right_T_left, const ScaleErrorType scaleErrorType, Scalar* scalePtr)
{
	ocean_assert(left != nullptr && right != nullptr);
	ocean_assert(correspondences >= 3);

	if (correspondences < 3)
	{
		return false;
	}

	Vector3 centerLeft(0, 0, 0);
	Vector3 centerRight(0, 0, 0);

	// calculate the center of both point clouds
	for (size_t n = 0; n < correspondences; ++n)
	{
		centerLeft += left[n];
		centerRight += right[n];
	}

	centerLeft /= Scalar(correspondences);
	centerRight /= Scalar(correspondences);

	Scalar xx = 0, xy = 0, xz = 0;
	Scalar yx = 0, yy = 0, yz = 0;
	Scalar zx = 0, zy = 0, zz = 0;

	for (size_t n = 0; n < correspondences; n++)
	{
		const Vector3 nLeft = left[n] - centerLeft;
		const Vector3 nRight = right[n] - centerRight;

		xx += nLeft(0) * nRight(0);
		xy += nLeft(0) * nRight(1);
		xz += nLeft(0) * nRight(2);

		yx += nLeft(1) * nRight(0);
		yy += nLeft(1) * nRight(1);
		yz += nLeft(1) * nRight(2);

		zx += nLeft(2) * nRight(0);
		zy += nLeft(2) * nRight(1);
		zz += nLeft(2) * nRight(2);
	}

	// row aligned matrix values
	Scalar matrixValues[16] =
	{
		xx + yy + zz,   yz - zy,       zx - xz,        xy - yx,
		yz - zy,        xx - yy - zz,  xy + yx,        zx + xz,
		zx - xz,        xy + yx,       -xx + yy - zz,  yz + zy,
		xy - yx,        zx + xz,       yz + zy,        -xx - yy + zz
	};

	const Matrix matrix(4, 4, matrixValues);
	Matrix values, vectors;

	if (!matrix.eigenSystem(values, vectors))
	{
		return false;
	}

	size_t greatestEigenValueIndex = 0;
	Scalar greatestEigenValue = Numeric::minValue();

	for (size_t n = 0; n < values.rows(); n++)
	{
		if (values(n) > greatestEigenValue)
		{
			greatestEigenValue = values(n);
			greatestEigenValueIndex = n;
		}
	}

	const Vector4 result(vectors(0, greatestEigenValueIndex), vectors(1, greatestEigenValueIndex), vectors(2, greatestEigenValueIndex), vectors(3, greatestEigenValueIndex));
	ocean_assert(Numeric::isEqual(result.length(), 1.0));

	const Quaternion right_Q_left(result);
	ocean_assert(right_Q_left.isValid());

	Scalar scale = 1;

	if (correspondences > 1)
	{
		scale = 0;

		switch (scaleErrorType)
		{
			case ScaleErrorType::RightBiased:
			{
				Scalar factor = 0;

				for (size_t n = 0; n < correspondences; ++n)
				{
					const Vector3 nLeft = left[n] - centerLeft;
					const Vector3 nRight = right[n] - centerRight;

					scale += nRight * (right_Q_left * nLeft);
					factor += nLeft.sqr();
				}

				if (Numeric::isEqualEps(factor) || scale < Scalar(0))
				{
					return false;
				}

				scale /= factor;

				break;
			}

			case ScaleErrorType::LeftBiased:
			{
				Scalar factor = 0.0;

				const Quaternion left_Q_right = right_Q_left.inverted();

				for (size_t n = 0; n < correspondences; ++n)
				{
					const Vector3 nLeft = left[n] - centerLeft;
					const Vector3 nRight = right[n] - centerRight;

					scale += nRight.sqr();
					factor += nLeft * (left_Q_right * nRight);
				}

				if (factor < Numeric::eps())
				{
					return false;
				}

				scale /= factor;

				break;
			}

			case ScaleErrorType::Symmetric:
			{
				Scalar factor = 0.0;

				for (size_t n = 0; n < correspondences; ++n)
				{
					const Vector3 nLeft = left[n] - centerLeft;
					const Vector3 nRight = right[n] - centerRight;

					scale += nRight.sqr();
					factor += nLeft.sqr();
				}

				ocean_assert(scale >= 0);
				ocean_assert(factor >= 0);

				factor = Numeric::sqrt(factor);

				if (Numeric::isEqualEps(factor))
				{
					return false;
				}

				scale = Numeric::sqrt(scale) / factor;

				break;
			}

			default:
			{
				ocean_assert(false && "Should never be here");

				break;
			}
		}
	}

	ocean_assert(scale >= Scalar(0));

	const Vector3 translation = centerRight - (right_Q_left * centerLeft) * scale;

	ocean_assert(Rotation(right_Q_left).isValid());

	right_T_left = HomogenousMatrix4(translation, right_Q_left, Vector3(1, 1, 1));

	if (scalePtr)
	{
		*scalePtr = scale;
	}

	return true;
}

bool AbsoluteTransformation::calculateTransformation(const HomogenousMatrix4* lefts_T_world, const HomogenousMatrix4* rights_T_world, const size_t correspondences, HomogenousMatrix4& right_T_left, const ScaleErrorType scaleErrorType, Scalar* scalePtr)
{
	ocean_assert(lefts_T_world != nullptr);
	ocean_assert(rights_T_world != nullptr);
	ocean_assert(correspondences >= 1);

	Vector3 meanTranslationLeft(0, 0, 0);
	Vector3 meanTranslationRight(0, 0, 0);

	for (size_t n = 0; n < correspondences; ++n)
	{
		meanTranslationLeft += lefts_T_world[n].translation();
		meanTranslationRight += rights_T_world[n].translation();
	}

	meanTranslationLeft /= Scalar(correspondences);
	meanTranslationRight /= Scalar(correspondences);

	// X * Y^T = [R_0 t_0, R_1 t_1, ... R_n-1 t_n-1] * [R'_0 t'_0, ..., ]^T

	SquareMatrix3 XY(false);

	for (size_t n = 0; n < correspondences; ++n)
	{
		const HomogenousMatrix4& left_T_world = lefts_T_world[n];
		const HomogenousMatrix4& right_T_world = rights_T_world[n];

		const Vector3 translationLeft = left_T_world.translation() - meanTranslationLeft;
		const Vector3 translationRight = right_T_world.translation() - meanTranslationRight;

		XY[0] += left_T_world[0] * right_T_world[0] + left_T_world[4] * right_T_world[4] + left_T_world[ 8] * right_T_world[ 8] + translationLeft[0] * translationRight[0];
		XY[1] += left_T_world[1] * right_T_world[0] + left_T_world[5] * right_T_world[4] + left_T_world[ 9] * right_T_world[ 8] + translationLeft[1] * translationRight[0];
		XY[2] += left_T_world[2] * right_T_world[0] + left_T_world[6] * right_T_world[4] + left_T_world[10] * right_T_world[ 8] + translationLeft[2] * translationRight[0];

		XY[3] += left_T_world[0] * right_T_world[1] + left_T_world[4] * right_T_world[5] + left_T_world[ 8] * right_T_world[ 9] + translationLeft[0] * translationRight[1];
		XY[4] += left_T_world[1] * right_T_world[1] + left_T_world[5] * right_T_world[5] + left_T_world[ 9] * right_T_world[ 9] + translationLeft[1] * translationRight[1];
		XY[5] += left_T_world[2] * right_T_world[1] + left_T_world[6] * right_T_world[5] + left_T_world[10] * right_T_world[ 9] + translationLeft[2] * translationRight[1];

		XY[6] += left_T_world[0] * right_T_world[2] + left_T_world[4] * right_T_world[6] + left_T_world[ 8] * right_T_world[10] + translationLeft[0] * translationRight[2];
		XY[7] += left_T_world[1] * right_T_world[2] + left_T_world[5] * right_T_world[6] + left_T_world[ 9] * right_T_world[10] + translationLeft[1] * translationRight[2];
		XY[8] += left_T_world[2] * right_T_world[2] + left_T_world[6] * right_T_world[6] + left_T_world[10] * right_T_world[10] + translationLeft[2] * translationRight[2];
	}

#ifdef OCEAN_DEBUG
	if (std::is_same<Scalar, double>::value)
	{
		Matrix debugX(3, 4 * correspondences);
		Matrix debugY(3, 4 * correspondences);

		for (size_t n = 0; n < correspondences; ++n)
		{
			const HomogenousMatrix4& left_T_world = lefts_T_world[n];
			const HomogenousMatrix4& right_T_world = rights_T_world[n];

			const Vector3 translationLeft = left_T_world.translation() - meanTranslationLeft;
			const Vector3 translationRight = right_T_world.translation() - meanTranslationRight;

			debugX(0, n * 4 + 0) = left_T_world(0, 0);
			debugX(1, n * 4 + 0) = left_T_world(1, 0);
			debugX(2, n * 4 + 0) = left_T_world(2, 0);

			debugX(0, n * 4 + 1) = left_T_world(0, 1);
			debugX(1, n * 4 + 1) = left_T_world(1, 1);
			debugX(2, n * 4 + 1) = left_T_world(2, 1);

			debugX(0, n * 4 + 2) = left_T_world(0, 2);
			debugX(1, n * 4 + 2) = left_T_world(1, 2);
			debugX(2, n * 4 + 2) = left_T_world(2, 2);

			debugX(0, n * 4 + 3) = translationLeft[0];
			debugX(1, n * 4 + 3) = translationLeft[1];
			debugX(2, n * 4 + 3) = translationLeft[2];


			debugY(0, n * 4 + 0) = right_T_world(0, 0);
			debugY(1, n * 4 + 0) = right_T_world(1, 0);
			debugY(2, n * 4 + 0) = right_T_world(2, 0);

			debugY(0, n * 4 + 1) = right_T_world(0, 1);
			debugY(1, n * 4 + 1) = right_T_world(1, 1);
			debugY(2, n * 4 + 1) = right_T_world(2, 1);

			debugY(0, n * 4 + 2) = right_T_world(0, 2);
			debugY(1, n * 4 + 2) = right_T_world(1, 2);
			debugY(2, n * 4 + 2) = right_T_world(2, 2);

			debugY(0, n * 4 + 3) = translationRight[0];
			debugY(1, n * 4 + 3) = translationRight[1];
			debugY(2, n * 4 + 3) = translationRight[2];
		}

		const SquareMatrix3 debugXY((debugX * debugY.transposed()).data(), true /*valuesRowAligned*/);

		ocean_assert(XY.isEqual(debugXY, Numeric::weakEps()));
	}
#endif // OCEAN_DEBUG

	// singular value decomposition:
	// X * Y^T = U * W * V^T

	Matrix u, w, v;
	if (!Matrix(3, 3, XY.data(), false /*valuesRowAligned*/).singularValueDecomposition(u, w, v))
	{
		return false;
	}

	ocean_assert(u.rows() == 3 && u.columns() == 3);
	ocean_assert(w.rows() == 3 && w.columns() == 1);
	ocean_assert(v.rows() == 3 && v.columns() == 3);

	// det(V * U^T)
	const Scalar determinant = (SquareMatrix3(v.data(), true) * SquareMatrix3(u.data(), true).transposed()).determinant();

	ocean_assert(Numeric::isEqual(Numeric::abs(determinant), 1, Numeric::weakEps()));

	SquareMatrix3 diagonal(true);

	if (determinant < 0)
	{
		diagonal[8] = -1;
	}

	// R = V * D * U^T
	const SquareMatrix3 right_R_left = SquareMatrix3(v.data(), true) * diagonal * SquareMatrix3(u.data(), true).transposed();

	Scalar scale = 1;

	if (correspondences > 1)
	{
		scale = 0;

		switch (scaleErrorType)
		{
			case ScaleErrorType::RightBiased:
			{
				Scalar factor = 0;

				for (size_t n = 0; n < correspondences; ++n)
				{
					const HomogenousMatrix4& left_T_world = lefts_T_world[n];
					const HomogenousMatrix4& right_T_world = rights_T_world[n];

					const Vector3 translationLeft = left_T_world.translation() - meanTranslationLeft;
					const Vector3 translationRight = right_T_world.translation() - meanTranslationRight;

					scale += translationRight * (right_R_left * translationLeft);
					factor += translationLeft.sqr();
				}

				if (Numeric::isEqualEps(factor) || scale < Scalar(0))
				{
					return false;
				}

				scale /= factor;

				break;
			}

			case ScaleErrorType::LeftBiased:
			{
				Scalar factor = 0.0;

				const Quaternion left_Q_right = Quaternion(right_R_left.inverted());

				for (size_t n = 0; n < correspondences; ++n)
				{
					const HomogenousMatrix4& left_T_world = lefts_T_world[n];
					const HomogenousMatrix4& right_T_world = rights_T_world[n];

					const Vector3 translationLeft = left_T_world.translation() - meanTranslationLeft;
					const Vector3 translationRight = right_T_world.translation() - meanTranslationRight;

					scale += translationRight.sqr();
					factor += translationLeft * (left_Q_right * translationRight);
				}

				if (factor < Numeric::eps())
				{
					return false;
				}

				scale /= factor;

				break;
			}

			case ScaleErrorType::Symmetric:
			{
				Scalar factor = 0.0;

				for (size_t n = 0; n < correspondences; ++n)
				{
					const HomogenousMatrix4& left_T_world = lefts_T_world[n];
					const HomogenousMatrix4& right_T_world = rights_T_world[n];

					const Vector3 translationLeft = left_T_world.translation() - meanTranslationLeft;
					const Vector3 translationRight = right_T_world.translation() - meanTranslationRight;

					scale += translationRight.sqr();
					factor += translationLeft.sqr();
				}

				ocean_assert(scale >= 0);
				ocean_assert(factor >= 0);

				factor = Numeric::sqrt(factor);

				if (Numeric::isEqualEps(factor))
				{
					return false;
				}

				scale = Numeric::sqrt(scale) / factor;

				break;
			}

			default:
			{
				ocean_assert(false && "Should never be here");

				break;
			}
		}
	}

	ocean_assert(scale >= Scalar(0));

	const Vector3 translation = meanTranslationRight - (right_R_left * meanTranslationLeft) * scale;

	right_T_left = HomogenousMatrix4(translation, right_R_left);

	if (scalePtr)
	{
		*scalePtr = scale;
	}

	return true;
}

bool AbsoluteTransformation::calculateTransformationWithOutliers(const HomogenousMatrix4* lefts_T_world, const HomogenousMatrix4* rights_T_world, const size_t correspondences, HomogenousMatrix4& right_T_left, const Scalar inlierRate, const ScaleErrorType scaleErrorType, Scalar* scalePtr)
{
	ocean_assert(lefts_T_world != nullptr);
	ocean_assert(rights_T_world != nullptr);
	ocean_assert(correspondences >= 1);
	ocean_assert(inlierRate > 0 && inlierRate < 1);

	Scalar scale = 1;
	if (!calculateTransformation(lefts_T_world, rights_T_world, correspondences, right_T_left, scaleErrorType, &scale))
	{
		return false;
	}

	HomogenousMatrix4 scaledRight_T_scaledLeft(right_T_left);
	scaledRight_T_scaledLeft.applyScale(Vector3(scale, scale, scale));

	HomogenousMatrices4 subsetLefts_T_world;
	HomogenousMatrices4 subsetRights_T_world;
	HomogenousMatrices4 intermediateSubsetLefts_T_world;
	HomogenousMatrices4 intermediateSubsetRights_T_world;

	if (removeOutliers(lefts_T_world, rights_T_world, correspondences, scaledRight_T_scaledLeft, subsetLefts_T_world, subsetRights_T_world))
	{
		const size_t minCorrespondences = std::max(size_t(2), size_t(Scalar(correspondences) * inlierRate + Scalar(0.5)));

		while (subsetLefts_T_world.size() > minCorrespondences)
		{
			if (!calculateTransformation(subsetLefts_T_world.data(), subsetRights_T_world.data(), subsetLefts_T_world.size(), right_T_left, scaleErrorType, &scale))
			{
				return false;
			}

			scaledRight_T_scaledLeft = right_T_left;
			scaledRight_T_scaledLeft.applyScale(Vector3(scale, scale, scale));

			std::swap(subsetLefts_T_world, intermediateSubsetLefts_T_world);
			std::swap(subsetRights_T_world, intermediateSubsetRights_T_world);

			subsetLefts_T_world.clear();
			subsetRights_T_world.clear();
			if (!removeOutliers(intermediateSubsetLefts_T_world.data(), intermediateSubsetRights_T_world.data(), intermediateSubsetLefts_T_world.size(), scaledRight_T_scaledLeft, subsetLefts_T_world, subsetRights_T_world))
			{
				// the input data did not contain any outliers
				break;
			}
		}
	}

	if (scalePtr != nullptr)
	{
		*scalePtr = scale;
	}

	return true;
}

bool AbsoluteTransformation::removeOutliers(const HomogenousMatrix4* lefts_T_world, const HomogenousMatrix4* rights_T_world, const size_t correspondences, const HomogenousMatrix4& right_T_left, HomogenousMatrices4& subsetLefts_T_world, HomogenousMatrices4& subsetRights_T_world)
{
	Scalars translationErrors;
	translationErrors.reserve(correspondences);

	for (size_t n = 0; n < correspondences; ++n)
	{
		translationErrors.emplace_back((right_T_left * lefts_T_world[n]).translation().distance(rights_T_world[n].translation()));
	}

	Scalars sortedTranslationErrors(translationErrors);

	std::sort(sortedTranslationErrors.begin(), sortedTranslationErrors.end());

	const Scalar maxTranslationError = std::max(Numeric::weakEps(), sortedTranslationErrors[sortedTranslationErrors.size() * 75 / 100] * Scalar(1.1));

	if (sortedTranslationErrors.back() <= maxTranslationError)
	{
		return false;
	}

	// we have at least one outlier

	subsetLefts_T_world.clear();
	subsetLefts_T_world.reserve(correspondences);

	subsetRights_T_world.clear();
	subsetRights_T_world.reserve(correspondences);

	for (size_t n = 0; n < correspondences; ++n)
	{
		if (translationErrors[n] <= maxTranslationError)
		{
			subsetLefts_T_world.emplace_back(lefts_T_world[n]);
			subsetRights_T_world.emplace_back(rights_T_world[n]);
		}
	}

	return true;
}

}

}
