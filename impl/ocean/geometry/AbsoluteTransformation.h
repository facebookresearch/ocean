/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_GEOMETRY_ABSOLUTE_TRANSFORMATION_H
#define META_OCEAN_GEOMETRY_ABSOLUTE_TRANSFORMATION_H

#include "ocean/geometry/Geometry.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace Geometry
{

/**
 * This class provides functions determining the absolute transformation between two point sets.
 * @ingroup geometry
 */
class OCEAN_GEOMETRY_EXPORT AbsoluteTransformation
{
	public:

		/**
		 * Defines whether the error term for the scale calculation is in the space of right coordinate system, the left coordinate system, or uses a symmetric formulation
		 */
		enum class ScaleErrorType
		{
			/// Error term for scaling computation is in the units of the right coordinate system (e = || Pright - s * R * Pleft ||^2).
			RightBiased,
			/// Error term for scaling computation is in the units of the left coordinate system (e = || (1 / s) * (R^{-1}) * Pright - Pleft ||^2).
			LeftBiased,
			/// Error term for scaling computation uses a symmetric formulation (e = || (1 / sqrt(s)) * Pright - sqrt(s) * R * Pleft) ||^2 ). (Recommended by Horn in the general case.)
			Symmetric
		};

		/**
		 * Calculates the absolute transformation between two corresponding 3D point sets.
		 * The implementation follows "Closed-form solution of absolute orientation using unit quaternions, Horn, 1986"<br>
		 * The resulting transformation contains translation and rotation, but no scaling. The scale term can be retrieved optionally -- this allows the transformation to be inverted more reliably.<br>
		 * Beware: This function does not support outliers.
		 * @param left The object points defined in the 'left' coordinate system, must be valid
		 * @param right The object points define in the 'right' coordinate system, one for each object point in the 'left' coordinate system, must be valid
		 * @param correspondences Number of point correspondences, with range [3, infinity)
		 * @param right_T_left Resulting transformation between left and right points, with no scaling.
		 * @param scaleErrorType The error type to use for the scale computation
		 * @param scale If non-null, will be populated with the resulting scale. The caller may want to update the transformation as follows: right_T_left.applyScale(Vector3(*scale, *scale, *scale))
		 * @return True, if succeeded
		 */
		static bool calculateTransformation(const Vector3* left, const Vector3* right, const size_t correspondences, HomogenousMatrix4& right_T_left, const ScaleErrorType scaleErrorType = ScaleErrorType::RightBiased, Scalar* scale = nullptr);

		/**
		 * Calculates the absolute transformation between two sets of 6-DOF transformations not containing outliers.
		 * The implementation follows "Comparing two sets of corresponding six degree of freedom data, Shah, 2011"<br>
		 * Beware: This function does not support outliers.
		 * @param leftWorld_T_transformations The individual transformations defined in the left world, must be valid
		 * @param rightWorld_T_transformations The idividual transformations defines in the right world, one for each transformation in the left world, must be valid
		 * @param correspondences The number of given transformation correspondences, with range [1, infinity)
		 * @param rightWorld_T_leftWorld The resulting transformation between the left world and the right world
		 * @param scaleErrorType The error type to use for the scale computation
		 * @param scale If non-null, will be populated with the resulting scale. The caller may want to update the transformation as follows: rightWorld_T_leftWorld.applyScale(Vector3(*scale, *scale, *scale))
		 * @return True, if succeeded
		 */
		static bool calculateTransformation(const HomogenousMatrix4* leftWorld_T_transformations, const HomogenousMatrix4* rightWorld_T_transformations, const size_t correspondences, HomogenousMatrix4& rightWorld_T_leftWorld, const ScaleErrorType scaleErrorType = ScaleErrorType::RightBiased, Scalar* scale = nullptr);

		/**
		 * Calculates the absolute transformation between two sets of 6-DOF transformations which may contain outliers.
		 * The implementation follows "Comparing two sets of corresponding six degree of freedom data, Shah, 2011"<br>
		 * @param leftWorld_T_transformations The individual transformations defined in the left world, must be valid
		 * @param rightWorld_T_transformations The idividual transformations defines in the right world, one for each transformation in the left world, must be valid
		 * @param correspondences the number of given transformation correspondences, with range [1, infinity)
		 * @param rightWorld_T_leftWorld The resulting transformation between the left world and the right world
		 * @param inlierRate The rate of inlier in the given input data with range (0, 1)
		 * @param scaleErrorType The error type to use for the scale computation
		 * @param scale If non-null, will be populated with the resulting scale. The caller may want to update the transformation as follows: rightWorld_T_leftWorld.applyScale(Vector3(*scale, *scale, *scale))
		 * @return True, if succeeded
		 */
		static bool calculateTransformationWithOutliers(const HomogenousMatrix4* leftWorld_T_transformations, const HomogenousMatrix4* rightWorld_T_transformations, const size_t correspondences, HomogenousMatrix4& rightWorld_T_leftWorld, const Scalar inlierRate = Scalar(0.75), const ScaleErrorType scaleErrorType = ScaleErrorType::RightBiased, Scalar* scale = nullptr);

	protected:

		/**
		 * Removes the outliers from the set of 6-DOF input transformations.
		 * @param leftWorld_T_transformations The individual transformations defined in the left world, must be valid
		 * @param rightWorld_T_transformations The idividual transformations defines in the right world, one for each transformation in the left world, must be valid
		 * @param correspondences the number of given transformation correspondences, with range [1, infinity)
		 * @param rightWorld_T_leftWorld The transformation between the left world and the right world
		 * @param leftWorld_T_subsetTransformations The resulting inlier transformations for 'leftWorld_T_transformations'
		 * @param rightWorld_T_subsetTransformations The resulting inlier transformations for 'rightWorld_T_transformations'
		 * @return True, if the input data contained some outliers; False, if the input data did not contain any outliers
		 */
		static bool removeOutliers(const HomogenousMatrix4* leftWorld_T_transformations, const HomogenousMatrix4* rightWorld_T_transformations, const size_t correspondences, const HomogenousMatrix4& rightWorld_T_leftWorld, HomogenousMatrices4& leftWorld_T_subsetTransformations, HomogenousMatrices4& rightWorld_T_subsetTransformations);
};

}

}

#endif // META_OCEAN_GEOMETRY_ABSOLUTE_TRANSFORMATION_H
