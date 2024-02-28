// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_PLATFORM_META_QUEST_VRAPI_UTILITIES_H
#define META_OCEAN_PLATFORM_META_QUEST_VRAPI_UTILITIES_H

#include "ocean/platform/meta/quest/vrapi/VrApi.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Pose.h"
#include "ocean/math/SquareMatrix4.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace VrApi
{

/**
 * This class implements utility functions for VrApi platforms.
 * @ingroup platformmetaquestvrapi
 */
class Utilities
{
	public:

		/**
		 * Converts an ovrMatrix4f matrix to an Ocean 4x4 matrix.
		 * @param matrix The matrix to convert
		 * @return The converted matrix
		 * @tparam T The data type of the elements of the resulting matrix, e.g., 'float' or 'double'
		 */
		template <typename T>
		static inline SquareMatrixT4<T> toSquareMatrix4(const ovrMatrix4f& matrix);

		/**
		 * Converts an ovrMatrix4f matrix to an Ocean homogenous 4x4 matrix.
		 * @param matrix The matrix to convert
		 * @return The converted matrix
		 * @tparam T The data type of the elements of the resulting matrix, e.g., 'float' or 'double'
		 */
		template <typename T>
		static inline HomogenousMatrixT4<T> toHomogenousMatrix4(const ovrMatrix4f& matrix);

		/**
		 * Converts an ovrPosef 6-DOF pose to an Ocean homogenous 4x4 matrix.
		 * @param pose The pose to convert
		 * @return The converted matrix
		 * @tparam T The data type of the elements of the resulting matrix, e.g., 'float' or 'double'
		 */
		template <typename T>
		static inline HomogenousMatrixT4<T> toHomogenousMatrix4(const ovrPosef& pose);

		/**
		 * Converts an ovrPosef 6-DOF pose to an Ocean pose.
		 * @param pose The pose to convert
		 * @return The converted pose
		 */
		static inline Pose toPose(const ovrPosef& pose);
};

template <typename T>
inline SquareMatrixT4<T> Utilities::toSquareMatrix4(const ovrMatrix4f& matrix)
{
	// OVR's matrix is row aligned
	const bool matrixIsRowAligned = true;

	return SquareMatrixT4<T>((const float*)(&matrix), matrixIsRowAligned);
}

template <typename T>
inline HomogenousMatrixT4<T> Utilities::toHomogenousMatrix4(const ovrMatrix4f& matrix)
{
	// OVR's matrix is row aligned
	const bool matrixIsRowAligned = true;

	return HomogenousMatrixT4<T>((const float*)(&matrix), matrixIsRowAligned);
}

template <typename T>
inline HomogenousMatrixT4<T> Utilities::toHomogenousMatrix4(const ovrPosef& pose)
{
	const VectorT3<T> translation(T(pose.Position.x), T(pose.Position.y), T(pose.Position.z));
	const QuaternionT<T> orientation(T(pose.Orientation.w), T(pose.Orientation.x), T(pose.Orientation.y), T(pose.Orientation.z));

	return HomogenousMatrix4(translation, orientation);
}

inline Pose Utilities::toPose(const ovrPosef& pose)
{
	const Vector3 translation(Scalar(pose.Position.x), Scalar(pose.Position.y), Scalar(pose.Position.z));
	const Quaternion orientation(Scalar(pose.Orientation.w), Scalar(pose.Orientation.x), Scalar(pose.Orientation.y), Scalar(pose.Orientation.z));

	return Pose(translation, orientation);
}

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_VRAPI_UTILITIES_H
