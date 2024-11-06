/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_ARKIT_UTILITIES_H
#define META_OCEAN_DEVICES_ARKIT_UTILITIES_H

#include "ocean/devices/arkit/ARKit.h"

#include "ocean/base/Frame.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/SquareMatrix3.h"

#include <ARKit/ARKit.h>

namespace Ocean
{

namespace Devices
{

namespace ARKit
{

/**
 * This class impelements utility functions.
 */
class OCEAN_DEVICES_ARKIT_EXPORT Utilities
{
	public:

		/**
		 * Converts a ARKit simd_float3x3 matrix to an Ocean SquareMatrix3 matrix.
		 * @param simdTransform The ARKit simd_float3x3 matrix to convert
		 * @return The resulting Ocean SquareMatrix3 matrix
		 * @tparam T The data type of the matrix elements, either 'float' or 'double'
		 */
		template <typename T = Scalar>
		static SquareMatrixT3<T> toSquareMatrix3(const simd_float3x3& simdTransform);

		/**
		 * Converts a ARKit simd_float4x4 matrix to an Ocean HomogenousMatrix4 matrix.
		 * @param simdTransform The ARKit simd_float4x4 matrix to convert
		 * @return The resulting Ocean HomogenousMatrix4 matrix
		 * @tparam T The data type of the matrix elements, either 'float' or 'double'
		 */
		template <typename T = Scalar>
		static HomogenousMatrixT4<T> toHomogenousMatrix4(const simd_float4x4& simdTransform);

		/**
		 * Converts an Ocean HomogenousMatrix4 matrix to an ARKit simd_float4x4 matrix.
		 * @param matrix The Ocean HomogenousMatrix4 matrix to convert
		 * @return The resulting ARKit simd_float4x4 matrix
		 * @tparam T The data type of the matrix elements, either 'float' or 'double'
		 */
		template <typename T = Scalar>
		static simd_float4x4 fromHomogenousMatrix4(const HomogenousMatrixT4<T>& matrix);

		/**
		 * Extracts the color image from an ARKit frame.
		 * @param arFrame The ARKit frame from which the color image will be extracted, must be valid
		 * @param copyData True, to copy the data; False, to only use the memory of the ARKit frame
		 * @param preferredPixelFormat Optional preferred pixel format of the resulting frame, however the resulting frame can have any pixel format, FORMAT_UNDEFINED if not of interest
		 */
		static Frame extractFrame(const ARFrame* arFrame, const bool copyData = true, const FrameType::PixelFormat preferredPixelFormat = FrameType::FORMAT_UNDEFINED);

		/**
		 * Extracts the depth image from an ARKit frame, if existing.
		 * @param arFrame The ARKit frame from which the depth image will be extracted, must be valid
		 * @param confidenceFrame Optional resulting confidence frame, if existing, nullptr if not of interest
		 * @param copyData True, to copy the data; False, to only use the memory of the ARKit frame
		 * @return The resulting depth frame, invalid if no depth information was available
		 */
		static Frame extractDepthFrame(const ARFrame* arFrame, Frame* confidenceFrame = nullptr, const bool copyData = true);

		/**
		 * Extracts the camera profile of the color image from an ARKit frame.
		 * @param arFrame The ARKit frame from which the camera profile will be extracted, must be valid
		 * @return The resulting camera profile, invalid if the camera profile could not be extracted
		 */
		template <typename T = Scalar>
		static SharedAnyCameraT<T> extractCameraModel(const ARFrame* arFrame);
};

template <typename T>
SquareMatrixT3<T> Utilities::toSquareMatrix3(const simd_float3x3& simdTransform)
{
	if constexpr (std::is_same<T, float>::value)
	{
		SquareMatrixF3 matrix;

		memcpy(matrix.data() + 0, &simdTransform.columns[0], sizeof(float) * 3);
		memcpy(matrix.data() + 3, &simdTransform.columns[1], sizeof(float) * 3);
		memcpy(matrix.data() + 6, &simdTransform.columns[2], sizeof(float) * 3);

		return matrix;
	}
	else
	{
		return SquareMatrixT3<T>(toSquareMatrix3<float>(simdTransform));
	}
}

template <typename T>
HomogenousMatrixT4<T> Utilities::toHomogenousMatrix4(const simd_float4x4& simdTransform)
{
	if constexpr (std::is_same<T, float>::value)
	{
		HomogenousMatrixF4 matrix;

		memcpy(matrix.data() + 0, &simdTransform.columns[0], sizeof(float) * 4);
		memcpy(matrix.data() + 4, &simdTransform.columns[1], sizeof(float) * 4);
		memcpy(matrix.data() + 8, &simdTransform.columns[2], sizeof(float) * 4);
		memcpy(matrix.data() + 12, &simdTransform.columns[3], sizeof(float) * 4);

		return matrix;
	}
	else
	{
		return HomogenousMatrixT4<T>(toHomogenousMatrix4<float>(simdTransform));
	}
}

template <typename T>
simd_float4x4 Utilities::fromHomogenousMatrix4(const HomogenousMatrixT4<T>& matrix)
{
	if constexpr (std::is_same<T, float>::value)
	{
		simd_float4x4 simdTransform;

		memcpy(&simdTransform.columns[0], matrix.data() + 0, sizeof(float) * 4);
		memcpy(&simdTransform.columns[1], matrix.data() + 4, sizeof(float) * 4);
		memcpy(&simdTransform.columns[2], matrix.data() + 8, sizeof(float) * 4);
		memcpy(&simdTransform.columns[3], matrix.data() + 12, sizeof(float) * 4);

		return simdTransform;
	}
	else
	{
		return fromHomogenousMatrix4(HomogenousMatrixF4(matrix));
	}
}

template <typename T>
SharedAnyCameraT<T> Utilities::extractCameraModel(const ARFrame* arFrame)
{
	ocean_assert(arFrame != nullptr);

	const SquareMatrixT3<T> cameraIntrinsics = toSquareMatrix3<T>(arFrame.camera.intrinsics);

	const int width = NumericD::round32(arFrame.camera.imageResolution.width);
	const int height = NumericD::round32(arFrame.camera.imageResolution.height);

	if (width > 0 && height > 0 && !cameraIntrinsics.isSingular())
	{
		return std::make_shared<AnyCameraPinholeT<T>>(PinholeCameraT<T>(cameraIntrinsics, (unsigned int)(width), (unsigned int)(height)));
	}

	ocean_assert(false && "This should never happen");
	return nullptr;
}

}

}

}

#endif // META_OCEAN_DEVICES_ARKIT_UTILITIES_H
