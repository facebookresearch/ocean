/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_OPENCV_UTILITIES_H
#define META_OCEAN_CV_OPENCV_UTILITIES_H

#include "ocean/base/Frame.h"

#include "ocean/cv/CV.h"

#include "ocean/math/SquareMatrix3.h"

#include <climits>

#include <opencv2/core.hpp>

namespace Ocean
{

namespace CV
{

/**
 * This class implements utility functions for the migration from OpenCV to Ocean.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT OpenCVUtilities
{
	public:

		/**
		 * Convert an Ocean Frame image to an OpenCV image
		 * @param frame An image instance of the Ocean framework
		 * @param copy If true, the data will be copied to (and owned by) to OpenCV Mat.
		 * @return An OpenCV-based version of the input image, will be empty on failure
		 */
		static inline cv::Mat toCvMat(const Frame& frame, const bool copy = false);

		/**
		 * Convert an OpenCV image to an Ocean Frame image object.
		 * @param frame The OpenCV image to be converted
		 * @param pixelFormat OpenCV image types do not store the pixel format. For images with 2 or more channels, specify the pixel format.
		 * @param copy If true, the data will be copied to (and owned by) to Ocean frame
		 * @return An Ocean-based version of the input image, will be invalid on failure
		 */
		static inline Frame toOceanFrame(const cv::Mat& frame, const bool copy = false, const FrameType::PixelFormat pixelFormat = FrameType::FORMAT_UNDEFINED);

		/**
		 * Convert an OpenCV 3-by-3 matrix (row-major) to an Ocean SquareMatrix3 (column-wise order)
		 * @param matrix An OpenCV instance of a 3-by-3 matrix
		 * @tparam T Type of the matrix elements (usually float or double)
		 * @return The corresponding Ocean matrix
		 */
		template <typename T>
		static inline SquareMatrixT3<T> toOceanSquareMatrix3(const cv::Matx<T, 3, 3>& matrix);

		/**
		 * Convert an OpenCV 2-by-3 matrix (row-major) to an Ocean SquareMatrix3 (column-major)
		 * @param affineTransformation An OpenCV instance of a 2-by-3 matrix (OpenCV assumes the third row to be [0, 0, 1])
		 * @tparam T Type of the matrix elements (usually float or double)
		 * @return The corresponding affine transformation Ocean matrix (third row is [0, 0, 1])
		 */
		template <typename T>
		static inline SquareMatrixT3<T> toOceanSquareMatrix3Affine(const cv::Matx<T, 2, 3>& affineTransformation);

		/**
		 * Convert an Ocean SquareMatrix3 (column-major) to an OpenCV 3-by-3 matrix (row-major)
		 * @param matrix An Ocean SquareMatrix3
		 * @tparam T Type of the matrix elements (usually float or double)
		 * @return OpenCV instance of a 3-by-3 matrix
		 */
		template <typename T>
		static inline cv::Matx<T, 3, 3> toCvMatx33(const SquareMatrixT3<T>& matrix);

		/**
		 * Convert an Ocean SquareMatrix3 (column-major) to an OpenCV 2-by-3 matrix (row-major)
		 * @param affineTransformation An Ocean SquareMatrix3, must be an affine transformation
		 * @tparam T Type of the matrix elements (usually float or double)
		 * @return The corresponding affine transformation as a 2-by-3 OpenCV matrix
		 */
		template <typename T>
		static inline cv::Matx<T, 2, 3> toCvMatx23Affine(const SquareMatrixT3<T>& affineTransformation);

		/**
		 * Converts an Ocean frame data type to a corresponding depth type in OpenCV
		 * @param dataType The Ocean data type that will be converted
		 * @param cvDepth The corresponding OpenCV depth type
		 * @return True if a correspondence has been found, otherwise false
		 */
		static inline bool toCvDepth(const FrameType::DataType dataType, int& cvDepth);

		/**
		 * Converts an OpenCV depth type to a corresponding Ocean frame data type
		 * @param cvDepth The OpenCV depth type that will be converted
		 * @param dataType The corresponding Ocean data type
		 * @return True if a correspondence has been found, otherwise false
		 */
		static inline bool toOceanFrameTypeDataType(const int cvDepth, FrameType::DataType& dataType);
};

inline cv::Mat OpenCVUtilities::toCvMat(const Frame& frame, const bool copy)
{
	ocean_assert(frame.isValid());

	const unsigned int channels = frame.channels();
	const FrameType::DataType dataType = frame.dataType();

	int cvDepth = CV_8U;
	if (toCvDepth(dataType, cvDepth) == false)
	{
		ocean_assert(false && "Ocean data type not supported by OpenCV");
		return cv::Mat();
	}

	cv::Mat cvMat(frame.height(), frame.width(), CV_MAKETYPE(cvDepth, channels), const_cast<uint8_t*>(frame.constdata<uint8_t>()), frame.strideBytes());

	if (copy)
	{
		cvMat = cvMat.clone();
	}

	return cvMat;
}

inline Frame OpenCVUtilities::toOceanFrame(const cv::Mat& mat, const bool copy, const FrameType::PixelFormat explicitPixelFormat)
{
	const unsigned int width = (unsigned int)(mat.size().width);
	const unsigned int height = (unsigned int)(mat.size().height);

	const unsigned int channels = (unsigned int)(mat.channels());
	ocean_assert(channels > 0u);

	const int cvDepth = mat.depth();

	FrameType::PixelFormat pixelFormat = explicitPixelFormat;

	if (pixelFormat == FrameType::FORMAT_UNDEFINED)
	{
		FrameType::DataType dataType = FrameType::DT_UNDEFINED;

		if (toOceanFrameTypeDataType(cvDepth, dataType) == false)
		{
			ocean_assert(false && "OpenCV depth type not supported by Ocean");
			return Frame();
		}

		ocean_assert(dataType != FrameType::DT_UNDEFINED);
		pixelFormat = FrameType::genericPixelFormat(dataType, channels);

		switch ((std::underlying_type<FrameType::PixelFormat>::type)(pixelFormat))
		{
			case FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, 1u>():
				pixelFormat = FrameType::FORMAT_Y8;
				break;

			case FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, 3u>():
				pixelFormat = FrameType::FORMAT_BGR24;
				break;

			case FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, 4u>():
				pixelFormat = FrameType::FORMAT_BGRA32;
				break;

			case FrameType::genericPixelFormat<FrameType::DT_SIGNED_FLOAT_32, 1u>():
				pixelFormat = FrameType::FORMAT_F32;
				break;

			case FrameType::genericPixelFormat<FrameType::DT_SIGNED_FLOAT_64, 1u>():
				pixelFormat = FrameType::FORMAT_F64;
				break;

			default:
				// we do not have a better mapping for the remaining pixel formats
				break;
		}
	}

	const unsigned int matStrideBytes = (unsigned int)(mat.step);

	unsigned int paddingElements = 0u;
	if (!Frame::strideBytes2paddingElements(pixelFormat, width, matStrideBytes, paddingElements))
	{
		ocean_assert(false && "Invalid pixel format!");
		return Frame();
	}

	const Frame::CopyMode copyMode = copy ? Frame::CM_COPY_KEEP_LAYOUT_DO_NOT_COPY_PADDING_DATA : Frame::CM_USE_KEEP_LAYOUT;

	return Frame(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), (void*)(mat.data), copyMode, paddingElements);
}

template <typename T>
inline SquareMatrixT3<T> OpenCVUtilities::toOceanSquareMatrix3(const cv::Matx<T, 3, 3>& matrix)
{
	return SquareMatrixT3<T>(matrix.val, true);
}

template <typename T>
inline SquareMatrixT3<T> OpenCVUtilities::toOceanSquareMatrix3Affine(const cv::Matx<T, 2, 3>& transformation)
{
	const T* v = transformation.val;
	return SquareMatrixT3<T>(v[0], v[3], T(0), v[1], v[4], T(0), v[2], v[5], T(1));
}

template <typename T>
inline cv::Matx<T, 3, 3> OpenCVUtilities::toCvMatx33(const SquareMatrixT3<T>& matrix)
{
	return cv::Matx<T, 3, 3>(matrix[0], matrix[3], matrix[6], matrix[1], matrix[4], matrix[7], matrix[2], matrix[5], matrix[8]);
}

template <typename T>
inline cv::Matx<T, 2, 3> OpenCVUtilities::toCvMatx23Affine(const SquareMatrixT3<T>& transformation)
{
	ocean_assert(transformation.isAffine());
	return cv::Matx<T, 2, 3>(transformation[0], transformation[3], transformation[6], transformation[1], transformation[4], transformation[7]);
}

inline bool OpenCVUtilities::toCvDepth(const FrameType::DataType dataType, int& cvDepth)
{
#if defined(CV_VERSION_MAJOR) && CV_VERSION_MAJOR >= 4

	switch (dataType)
	{
		case FrameType::DT_UNSIGNED_INTEGER_8:
			cvDepth = CV_8U;
			break;

		case FrameType::DT_SIGNED_INTEGER_8:
			cvDepth = CV_8S;
			break;

		case FrameType::DT_UNSIGNED_INTEGER_16:
			cvDepth = CV_16U;
			break;

		case FrameType::DT_SIGNED_INTEGER_16:
			cvDepth = CV_16S;
			break;

		case FrameType::DT_SIGNED_INTEGER_32:
			cvDepth = CV_32S;
			break;

		case FrameType::DT_SIGNED_FLOAT_16:
			cvDepth = CV_16F;
			break;

		case FrameType::DT_SIGNED_FLOAT_32:
			cvDepth = CV_32F;
			break;

		case FrameType::DT_SIGNED_FLOAT_64:
			cvDepth = CV_64F;
			break;

		default:
			return false;
	}

#else

	switch (dataType)
	{
		case FrameType::DT_UNSIGNED_INTEGER_8:
			cvDepth = CV_8U;
			break;

		case FrameType::DT_SIGNED_INTEGER_8:
			cvDepth = CV_8S;
			break;

		case FrameType::DT_UNSIGNED_INTEGER_16:
			cvDepth = CV_16U;
			break;

		case FrameType::DT_SIGNED_INTEGER_16:
			cvDepth = CV_16S;
			break;

		case FrameType::DT_UNSIGNED_INTEGER_32:
			cvDepth = CV_USRTYPE1;
			break;

		case FrameType::DT_SIGNED_INTEGER_32:
			cvDepth = CV_32S;
			break;

		case FrameType::DT_UNSIGNED_INTEGER_64:
			cvDepth = CV_USRTYPE1;
			break;

		case FrameType::DT_SIGNED_INTEGER_64:
			cvDepth = CV_USRTYPE1;
			break;

		case FrameType::DT_SIGNED_FLOAT_16:
			cvDepth = CV_USRTYPE1;
			break;

		case FrameType::DT_SIGNED_FLOAT_32:
			cvDepth = CV_32F;
			break;

		case FrameType::DT_SIGNED_FLOAT_64:
			cvDepth = CV_64F;
			break;

		default:
			cvDepth = CV_USRTYPE1;
			break;
	}

#endif

	return true;
}

inline bool OpenCVUtilities::toOceanFrameTypeDataType(const int cvDepth, FrameType::DataType& dataType)
{
	switch (cvDepth)
	{
		case CV_8U:
			dataType = FrameType::DT_UNSIGNED_INTEGER_8;
			break;

		case CV_8S:
			dataType = FrameType::DT_SIGNED_INTEGER_8;
			break;

		case CV_16U:
			dataType = FrameType::DT_UNSIGNED_INTEGER_16;
			break;

		case CV_16S:
			dataType = FrameType::DT_SIGNED_INTEGER_16;
			break;

		case CV_32S:
			dataType = FrameType::DT_SIGNED_INTEGER_32;
			break;

#if defined(CV_VERSION_MAJOR) && CV_VERSION_MAJOR >= 4

		case CV_16F:
			dataType = FrameType::DT_SIGNED_FLOAT_16;
			break;

#endif

		case CV_32F:
			dataType = FrameType::DT_SIGNED_FLOAT_32;
			break;

		case CV_64F:
			dataType = FrameType::DT_SIGNED_FLOAT_64;
			break;

		default:
			dataType = FrameType::DT_UNDEFINED;
			return false;
	}

	return true;
}

} // namespace CV

} // namespace Ocean

#endif // META_OCEAN_CV_OPENCV_UTILITIES_H
