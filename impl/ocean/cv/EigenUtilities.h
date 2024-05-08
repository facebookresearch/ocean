/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_EIGEN_UTILITIES_H
#define META_OCEAN_CV_EIGEN_UTILITIES_H

#include "ocean/math/Math.h"

#include "ocean/base/Frame.h"

#include "ocean/cv/FrameConverter.h"

#if defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable: 4244) // 'argument': conversion from 'const __int64' to 'int', possible loss of data
#elif defined(__clang__)
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wconversion"
#endif

#include <Eigen/Dense>

#if defined(_MSC_VER)
	#pragma warning(pop)
#elif defined(__clang__)
	#pragma clang diagnostic pop
#endif

namespace Ocean
{

namespace CV
{

/**
 * This class provides several helper classes allowing to convert between Ocean data structures and Eigen data structures.
 * @ingroup cv
 */
class EigenUtilities
{
	public:

		/**
		 * Converts a frame to an Eigen matrix with column major memory layout (default in Eigen).
		 * @param frame The frame to be converted, must be a 1 channel 'unsigned char' or 'float' frame, must be valid
		 * @param matrix The resulting matrix receiving a copy of the frame data
		 * @return True, if succeeded
		 */
		template <typename T>
		static bool frame2matrix(const Frame& frame, Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor>& matrix);

		/**
		 * Converts a frame to an Eigen matrix with row major memory layout.
		 * @param frame The frame to be converted, must be a 1 channel 'unsigned char' or 'float' frame, must be valid
		 * @param matrix The resulting matrix receiving a copy of the frame data
		 * @return True, if succeeded
		 */
		template <typename T>
		static bool frame2matrix(const Frame& frame, Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>& matrix);

		/**
		 * Converts an Ocean 2D vector to a corresponding Eigen vector
		 * @param ocnVector The Ocean vector that will be converted
		 * @return The Eigen vector with the same values as the Ocean vector
		 * @tparam TType The type of the vector elements, must be either `int`, `float`, or `double`
		 */
		template <typename TType>
		static Eigen::Matrix<TType, 2, 1> toEigenVector(const VectorT2<TType>& ocnVector);

		/**
		 * Converts an Ocean 3D vector to a corresponding Eigen vector
		 * @param ocnVector The Ocean vector that will be converted
		 * @return The Eigen vector with the same values as the Ocean vector
		 * @tparam TType The type of the vector elements, must be either `int`, `float`, or `double`
		 */
		template <typename TType>
		static Eigen::Matrix<TType, 3, 1> toEigenVector(const VectorT3<TType>& ocnVector);

		/**
		 * Converts an Ocean 4D vector to a corresponding Eigen vector
		 * @param ocnVector The Ocean vector that will be converted
		 * @return The Eigen vector with the same values as the Ocean vector
		 * @tparam TType The type of the vector elements, must be either `int`, `float`, or `double`
		 */
		template <typename TType>
		static Eigen::Matrix<TType, 4, 1> toEigenVector(const VectorT4<TType>& ocnVector);

		/**
		 * Converts an Eigen 2D vector to a corresponding Ocean vector
		 * @param eigenVector The Eigen vector that will be converted
		 * @return The Ocean vector with the same value as the Eigen vector
		 * @tparam TType The type of the vector elements, must be either `int`, `float`, or `double`
		 */
		template <typename TType>
		static VectorT2<TType> toOceanVector(const Eigen::Matrix<TType, 2, 1>& eigenVector);

		/**
		 * Converts an Eigen 3D vector to a corresponding Ocean vector
		 * @param eigenVector The Eigen vector that will be converted
		 * @return The Ocean vector with the same value as the Eigen vector
		 * @tparam TType The type of the vector elements, must be either `int`, `float`, or `double`
		 */
		template <typename TType>
		static VectorT3<TType> toOceanVector(const Eigen::Matrix<TType, 3, 1>& eigenVector);

		/**
		 * Converts an Eigen 4D vector to a corresponding Ocean vector
		 * @param eigenVector The Eigen vector that will be converted
		 * @return The Ocean vector with the same value as the Eigen vector
		 * @tparam TType The type of the vector elements, must be either `int`, `float`, or `double`
		 */
		template <typename TType>
		static VectorT4<TType> toOceanVector(const Eigen::Matrix<TType, 4, 1>& eigenVector);

		/**
		 * Converts an Ocean quaternion to an Eigen quaternion
		 * @param ocnQuaternion The Ocean quaternion that will be converted to an Eigen quaternion
		 * @return The resulting Eigen quaternion
		 * @tparam TType The type of the quaternion elements, must be either `float` or `double`
		 */
		template <typename TType>
		static Eigen::Quaternion<TType> toEigenQuaternion(const QuaternionT<TType>& ocnQuaternion);

		/**
		 * Converts an Eigen quaternion to an Ocean quaternion
		 * @param eigenQuaternion The Eigen quaternion that will be converted to an Ocean quaternion
		 * @return The resulting Ocean quaternion
		 * @tparam TType The type of the quaternion elements, must be either `float` or `double`
		 */
		template <typename TType>
		static QuaternionT<TType> toOceanQuaternion(const Eigen::Quaternion<TType>& eigenQuaternion);
};

template <typename T>
bool EigenUtilities::frame2matrix(const Frame& frame, Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor>& matrix)
{
	ocean_assert(frame.isValid() && frame.numberPlanes() == 1u && frame.channels() == 1u);

	if (!frame.isValid() || frame.numberPlanes() != 1u || frame.channels() != 1u)
	{
		return false;
	}

	ocean_assert((matrix.Flags & Eigen::RowMajorBit) == 0);

	Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> rowMajorMatrix(frame.height(), frame.width());

	constexpr unsigned int matrixPaddingElements = 0u;

	if (FrameType::formatIsGeneric(frame.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 1u))
	{
		CV::FrameConverter::cast<uint8_t, T>(frame.constdata<uint8_t>(), rowMajorMatrix.data(), frame.width(), frame.height(), 1u, frame.paddingElements(), matrixPaddingElements);
	}
	else if (FrameType::formatIsGeneric(frame.pixelFormat(), FrameType::DT_SIGNED_FLOAT_32, 1u))
	{
		CV::FrameConverter::cast<float, T>(frame.constdata<float>(), rowMajorMatrix.data(), frame.width(), frame.height(), 1u, frame.paddingElements(), matrixPaddingElements);
	}
	else
	{
		ocean_assert(false && "Data type not supported!");
		return false;
	}

	matrix = rowMajorMatrix;

	ocean_assert((matrix.Flags & Eigen::RowMajorBit) == 0);

	return true;
}

template <typename T>
bool EigenUtilities::frame2matrix(const Frame& frame, Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>& matrix)
{
	ocean_assert(frame.isValid() && frame.numberPlanes() == 1u && frame.channels() == 1u);

	if (!frame.isValid() || frame.numberPlanes() != 1u || frame.channels() != 1u)
	{
		return false;
	}

	ocean_assert((matrix.Flags & Eigen::RowMajorBit) != 0);

	matrix.resize(frame.height(), frame.width());

	constexpr unsigned int matrixPaddingElements = 0u;

	if (FrameType::formatIsGeneric(frame.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 1u))
	{
		CV::FrameConverter::cast<uint8_t, T>(frame.constdata<uint8_t>(), matrix.data(), frame.width(), frame.height(), 1u, frame.paddingElements(), matrixPaddingElements);
		return true;
	}
	else if (FrameType::formatIsGeneric(frame.pixelFormat(), FrameType::DT_SIGNED_FLOAT_32, 1u))
	{
		CV::FrameConverter::cast<float, T>(frame.constdata<float>(), matrix.data(), frame.width(), frame.height(), 1u, frame.paddingElements(), matrixPaddingElements);
		return true;
	}

	ocean_assert(false && "Data type not supported!");
	return false;
}

template <typename TType>
Eigen::Matrix<TType, 2, 1> EigenUtilities::toEigenVector(const VectorT2<TType>& ocnVector)
{
	return Eigen::Matrix<TType, 2, 1>(ocnVector.data());
}

template <typename TType>
Eigen::Matrix<TType, 3, 1> EigenUtilities::toEigenVector(const VectorT3<TType>& ocnVector)
{
	return Eigen::Matrix<TType, 3, 1>(ocnVector.data());
}

template <typename TType>
Eigen::Matrix<TType, 4, 1> EigenUtilities::toEigenVector(const VectorT4<TType>& ocnVector)
{
	return Eigen::Matrix<TType, 4, 1>(ocnVector.data());
}

template <typename TType>
VectorT2<TType> EigenUtilities::toOceanVector(const Eigen::Matrix<TType, 2, 1>& eigenVector)
{
	return VectorT2<TType>(eigenVector.data());
}

template <typename TType>
VectorT3<TType> EigenUtilities::toOceanVector(const Eigen::Matrix<TType, 3, 1>& eigenVector)
{
	return VectorT3<TType>(eigenVector.data());
}

template <typename TType>
VectorT4<TType> EigenUtilities::toOceanVector(const Eigen::Matrix<TType, 4, 1>& eigenVector)
{
	return VectorT4<TType>(eigenVector.data());
}

template <typename TType>
Eigen::Quaternion<TType> EigenUtilities::toEigenQuaternion(const QuaternionT<TType>& ocnQuaternion)
{
	static_assert(std::is_same<TType, float>::value || std::is_same<TType, double>::value, "Invalid value for type TType");

	return Eigen::Quaternion<TType>(ocnQuaternion.w(), ocnQuaternion.x(), ocnQuaternion.y(), ocnQuaternion.z());
}

template <typename TType>
QuaternionT<TType> EigenUtilities::toOceanQuaternion(const Eigen::Quaternion<TType>& eigenQuaternion)
{
	static_assert(std::is_same<TType, float>::value || std::is_same<TType, double>::value, "Invalid value for type TType");

	return QuaternionT<TType>(eigenQuaternion.w(), eigenQuaternion.x(), eigenQuaternion.y(), eigenQuaternion.z());
}

} // namespace CV

} // namespace Ocean

#endif // META_OCEAN_CV_EIGEN_UTILITIES_H
