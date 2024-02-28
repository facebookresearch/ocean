// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_IO_VRS_UTILITIES_H
#define META_OCEAN_IO_VRS_UTILITIES_H

#include "ocean/base/Frame.h"

#include "ocean/io/vrs/VRS.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/SampleMap.h"

#include <perception/camera/Fisheye62CameraModelLut1D.h>

#include <vrs/RecordFormatStreamPlayer.h>

#include <vrs/utils/PixelFrame.h>

namespace Ocean
{

namespace IO
{

namespace VRS
{

/**
 * This class implements several utility functions.
 * @ingroup iovrs
 */
class OCEAN_IO_VRS_EXPORT Utilities
{
	public:

		/**
		 * Definition of an unordered set holding recordable type ids.
		 */
		typedef std::unordered_set<vrs::RecordableTypeId> RecordableTypeIdSet;

	public:

		/**
		 * Returns the list of available recordables in a vrs file with any type id.
		 * @param vrsFile The vrs file in which the recordables are investigated
		 * @return The names of available recordables
		 */
		static std::vector<std::string> availableRecordablesInFile(const std::string& vrsFile);

		/**
		 * Returns the list of available recordables in a vrs file with type id 'SlamCameraData'.
		 * @param vrsFile The vrs file in which the recordables are investigated
		 * @return The names of available recordables with matching type id
		 */
		static std::vector<std::string> availableSlamCameraDataRecordablesInFile(const std::string& vrsFile);

		/**
		 * Returns the list of available recordables in a vrs file with type id 'SlamIMUData'.
		 * @param vrsFile The vrs file in which the recordables are investigated
		 * @return The names of available recordables with matching type id
		 */
		static std::vector<std::string> availableSlamIMUDataRecordablesInFile(const std::string& vrsFile);

		/**
		 * Returns the list of available recordables in a vrs file with type id.
		 * @param vrsFile The vrs file in which the recordables are investigated
		 * @param typeIds The type ids of the recordables to return
		 * @return The names of available recordables with matching type id
		 */
		static std::vector<std::string> availableRecordablesInFile(const std::string& vrsFile, const RecordableTypeIdSet& typeIds);

		/**
		 * Converts an Ocean 3x3 homogenous matrix to a VRS 3x3 matrix.
		 * @param matrix The matrix to be converted
		 * @return The converted VRS matrix
		 * @tparam TSource The data type of the elements of the source matrix `float`, or `double`
		 * @tparam TTarget The data type of the elements of the target matrix `float`, or `double`
		 */
		template <typename TSource, typename TTarget>
		static inline vrs::MatrixND<TTarget, 3> squareMatrix3ToVRS(const SquareMatrixT3<TSource>& matrix);

		/**
		 * Converts an Ocean 4x4 homogenous matrix to a VRS 4x4 matrix.
		 * @param matrix The matrix to be converted
		 * @return The converted VRS matrix
		 * @tparam TSource The data type of the elements of the source matrix `float`, or `double`
		 * @tparam TTarget The data type of the elements of the target matrix `float`, or `double`
		 */
		template <typename TSource, typename TTarget>
		static inline vrs::MatrixND<TTarget, 4> homogenousMatrix4ToVRS(const HomogenousMatrixT4<TSource>& matrix);

		/**
		 * Converts a VRS pixel format to an Ocean pixel format.
		 * @param vrsPixelFormat The VRS pixel format that will be converted to an Ocean pixel format.
		 * @param pixelFormat The resulting Ocean pixel format; will be undefined if this function returns false.
		 * @return True on success, otherwise false.
		 */
		static bool toOceanPixelFormat(const vrs::PixelFormat& vrsPixelFormat, FrameType::PixelFormat& pixelFormat);

		/**
		 * Converts a VRS pixel frame to an Ocean frame.
		 * @param vrsPixelFrame The VRS pixel frame that will be converted to an Ocean frame, must be valid.
		 * @param frame The resulting Ocean frame.
		 * @param copyMode The copy mode that should be used for the conversion.
		 * @return True on success, otherwise false.
		 */
		static bool toOceanFrame(const vrs::utils::PixelFrame& vrsPixelFrame, Frame& frame, const Frame::CopyMode copyMode = Frame::CM_COPY_REMOVE_PADDING_LAYOUT);
};

template <typename TSource, typename TTarget>
inline vrs::MatrixND<TTarget, 3> Utilities::squareMatrix3ToVRS(const SquareMatrixT3<TSource>& matrix)
{
	vrs::MatrixND<TTarget, 3> result;
	matrix.copyElements((TTarget*)result, /* rowAligned = */ true);

	return result;
}

template <typename TSource, typename TTarget>
inline vrs::MatrixND<TTarget, 4> Utilities::homogenousMatrix4ToVRS(const HomogenousMatrixT4<TSource>& matrix)
{
	vrs::MatrixND<TTarget, 4> result;
	matrix.copyElements((TTarget*)&result, /* rowAligned = */ true);

	return result;
}

}

}

}

#endif // META_OCEAN_IO_VRS_UTILITIES_H
