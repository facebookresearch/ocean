/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_OPENXR_UTILITIES_H
#define META_OCEAN_PLATFORM_OPENXR_UTILITIES_H

#include "ocean/platform/openxr/OpenXR.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/SquareMatrix4.h"

namespace Ocean
{

namespace Platform
{

namespace OpenXR
{

/**
 * This class implements utility functions for OpenXR.
 * @ingroup platformopenxr
 */
class OCEAN_PLATFORM_OPENXR_EXPORT Utilities final
{
	public:

		/**
		 * Translates an OpenXR result associated with an instance into a readable string.
		 * @param xrInstance The OpenXR instance associated with the given result, must be valid
		 * @param xrResult The OpenXR result to translate
		 * @return The translated result, 'Unknown' if unknown
		 */
		static std::string translateResult(const XrInstance& xrInstance, const XrResult xrResult);

		/**
		 * Translates an OpenXR session state into a readable string.
		 * @param xrSessionState The OpenXR session state to translate
		 * @return The translated state, 'Unknown' if unknown
		 */
		static std::string translateSessionState(const XrSessionState xrSessionState);

		/**
		 * Converts an OpenXR view configuration type into a readable string.
		 * @param xrViewConfigurationType The view configuration type to translate
		 * @return The translated type, 'Unknown' if unknown
		 */
		static std::string translateViewConfigurationType(const XrViewConfigurationType xrViewConfigurationType);

		/**
		 * Converts an OpenXR color space into a readable string.
		 * @param xrColorSpaceFB The color space to translate
		 * @return The translated color space, 'Unknown' if unknown
		 */
		static std::string translateColorSpace(const XrColorSpaceFB xrColorSpaceFB);

		/**
		 * Converts an OpenXR path to a string.
		 * @param xrInstance The OpenXR instance associated with the given path, must be valid
		 * @param xrPath The OpenXR path to convert, XR_NULL_PATH to return an empty string
		 * @return The converted path as string
		 */
		static std::string translatePath(const XrInstance& xrInstance, const XrPath& xrPath);

		/**
		 * Converts a string to an OpenXR path.
		 * @param xrInstance The OpenXR instance associated with the given path, must be valid
		 * @param path The path to convert, empty to return XR_NULL_PATH
		 * @return The converted path as string
		 */
		static XrPath translatePath(const XrInstance& xrInstance, const std::string& path);

		/**
		 * Converts an OpenXR pose to a HomogenousMatrix4.
		 * @param xrPose The pose to convert
		 * @return The resulting matrix
		 * @tparam T The data type of the scalar to be used
		 */
		template <typename T = Scalar>
		static inline HomogenousMatrixT4<T> toHomogenousMatrix4(const XrPosef& xrPose);

		/**
		 * Converts an OpenXR XrQuaternionf to a Quaternion
		 * @param xrQuaternionf The rotation to convert
		 * @return The resulting quaternion
		 * @tparam T The data type of the scalar to be used
		 */
		template <typename T = Scalar>
		static inline QuaternionT<T> toQuaternion(const XrQuaternionf& xrQuaternionf);

		/**
		 * Converts an OpenXR XrVector3f to a Vector3
		 * @param xrVector3f The vector to convert
		 * @return The resulting quaternion
		 * @tparam T The data type of the scalar to be used
		 */
		template <typename T = Scalar>
		static inline VectorT3<T> toVector3(const XrVector3f& xrVector3f);

		/**
		 * Converts a 4x4 homogenous matrix containing a pose to an OpenXR XrPosef object.
		 * @param pose The pose matrix to convert, must be valid
		 * @return The resulting OpenXR object
		 * @tparam T The data type of the scalars of the matrix
		 */
		template <typename T = Scalar>
		static inline XrPosef toXrPose(const HomogenousMatrixT4<T>& pose);

		/**
		 * Converts an OpenXR field of view into a 4x4 projection matrix.
		 * @param xrFovf The OpenXR field of view for which the projection matrix will be determined
		 * @param nearDistance Positive distance to the near clipping plane, with range (0, infinity)
		 * @param farDistance Positive distance to the far clipping plane, with range (nearDistance, infinity)
		 */
		static SquareMatrixF4 toProjectionMatrix4(const XrFovf& xrFovf, const float nearDistance, const float farDistance);

		/**
		 * Determines the pose of an OpenXR space.
		 * @param xrSpace The space for which the pose will be returned, must be valid
		 * @param xrBaseSpace The base space in relation to which the pose will be returned, must be valid
		 * @param xrTime The time for which the pose will be determined, must be valid
		 * @param xrSpaceLocationFlags Optional resulting location flags, nullptr if not of interest
		 * @return The resulting pose, invalid in case of an error
		 */
		template <typename T = Scalar>
		static inline HomogenousMatrixT4<T> determinePose(const XrSpace& xrSpace, const XrSpace& xrBaseSpace, const XrTime& xrTime, XrSpaceLocationFlags* xrSpaceLocationFlags = nullptr);
};

template <>
inline HomogenousMatrixT4<float> Utilities::toHomogenousMatrix4<float>(const XrPosef& xrPose)
{
	ocean_assert(QuaternionF(xrPose.orientation.w, xrPose.orientation.x, xrPose.orientation.y, xrPose.orientation.z).isValid());

	return HomogenousMatrixF4(VectorF3(xrPose.position.x, xrPose.position.y, xrPose.position.z), QuaternionF(xrPose.orientation.w, xrPose.orientation.x, xrPose.orientation.y, xrPose.orientation.z));
}

template <typename T>
inline HomogenousMatrixT4<T> Utilities::toHomogenousMatrix4(const XrPosef& xrPose)
{
	ocean_assert(QuaternionF(xrPose.orientation.w, xrPose.orientation.x, xrPose.orientation.y, xrPose.orientation.z).isValid());

	const VectorT3<T> position(T(xrPose.position.x), T(xrPose.position.y), T(xrPose.position.z));
	const QuaternionT<T> orientation(T(xrPose.orientation.w), T(xrPose.orientation.x), T(xrPose.orientation.y), T(xrPose.orientation.z));

	return HomogenousMatrixT4<T>(position, orientation.normalized()); // we normalize the quaternion to prevent precision issues
}

template <>
inline QuaternionT<float> Utilities::toQuaternion(const XrQuaternionf& xrQuaternionf)
{
	ocean_assert(QuaternionF(xrQuaternionf.w, xrQuaternionf.x, xrQuaternionf.y, xrQuaternionf.z).isValid());

	return QuaternionF(xrQuaternionf.w, xrQuaternionf.x, xrQuaternionf.y, xrQuaternionf.z);
}

template <typename T>
inline QuaternionT<T> Utilities::toQuaternion(const XrQuaternionf& xrQuaternionf)
{
	ocean_assert(QuaternionF(xrQuaternionf.w, xrQuaternionf.x, xrQuaternionf.y, xrQuaternionf.z).isValid());

	const QuaternionT<T> orientation(T(xrQuaternionf.w), T(xrQuaternionf.x), T(xrQuaternionf.y), T(xrQuaternionf.z));

	return orientation.normalized(); // we normalize the quaternion to prevent precision issues
}

template <typename T>
inline VectorT3<T> Utilities::toVector3(const XrVector3f& xrVector3f)
{
	return VectorT3<T>(T(xrVector3f.x), T(xrVector3f.y), T(xrVector3f.z));
}

template <>
inline XrPosef Utilities::toXrPose(const HomogenousMatrixT4<float>& pose)
{
	ocean_assert(pose.isValid());
	ocean_assert(pose.rotationMatrix().isOrthonormal(NumericF::weakEps()));

	const VectorF3 position = pose.translation();
	const QuaternionF orientation = pose.rotation();

	XrPosef result;

	result.orientation.x = orientation.x();
	result.orientation.y = orientation.y();
	result.orientation.z = orientation.z();
	result.orientation.w = orientation.w();

	result.position.x = position.x();
	result.position.y = position.y();
	result.position.z = position.z();

	return result;
}

template <typename T>
inline XrPosef Utilities::toXrPose(const HomogenousMatrixT4<T>& pose)
{
	ocean_assert(pose.isValid());
	ocean_assert(pose.rotationMatrix().isOrthonormal(NumericT<T>::weakEps()));

	const VectorT3<T> position = pose.translation();

	QuaternionF orientation(pose.rotation()); // we normalize the quaternion to prevent precision issues
	orientation.normalize();

	XrPosef result;

	result.orientation.x = orientation.x();
	result.orientation.y = orientation.y();
	result.orientation.z = orientation.z();
	result.orientation.w = orientation.w();

	result.position.x = float(position.x());
	result.position.y = float(position.y());
	result.position.z = float(position.z());

	return result;
}

template <typename T>
inline HomogenousMatrixT4<T> Utilities::determinePose(const XrSpace& xrSpace, const XrSpace& xrBaseSpace, const XrTime& xrTime, XrSpaceLocationFlags* xrSpaceLocationFlags)
{
	ocean_assert(xrSpace != XR_NULL_HANDLE);
	ocean_assert(xrBaseSpace != XR_NULL_HANDLE);

	XrSpaceLocation xrSpaceLocation = {XR_TYPE_SPACE_LOCATION};
	const XrResult xrResult = xrLocateSpace(xrSpace, xrBaseSpace, xrTime, &xrSpaceLocation);

	if (xrResult != XR_SUCCESS)
	{
		return HomogenousMatrixT4<T>(false);
	}

	if (xrSpaceLocationFlags != nullptr)
	{
		*xrSpaceLocationFlags = xrSpaceLocation.locationFlags;
	}

	return toHomogenousMatrix4<T>(xrSpaceLocation.pose);
}

}

}

}

#endif // META_OCEAN_PLATFORM_OPENXR_UTILITIES_H
