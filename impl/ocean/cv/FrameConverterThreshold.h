/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_CONVERTER_THRESHOLD_H
#define META_OCEAN_CV_FRAME_CONVERTER_THRESHOLD_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameConverter.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements frame converters dividing a frame by application of a threshold.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameConverterThreshold : public FrameConverter
{
	public:

		/**
		 * Converts an 8 bit (grayscale) frame into a binary frame using a threshold value.
		 * The resulting target pixel values will be 0 for all input values smaller than the specified threshold value and 255 for all input values equal or larger than the specified threshold value.
		 * @param buffer Source and target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param bufferPaddingElements The number of padding elements at the end of each buffer row, in elements, with range [0, infinity)
		 * @param threshold The threshold dividing the frame into black and white, with range [1, 255]
		 * @param worker Optional worker used to distribute the CPU load
		 */
		static inline void convertY8ToB8(uint8_t* buffer, const unsigned int width, const unsigned int height, const unsigned int bufferPaddingElements, const uint8_t threshold, Worker* worker = nullptr);

		/**
		 * Converts an 8 bit gray scale frame into a second binary frame using a threshold.
		 * The resulting target pixel values will be 0 for all input values smaller than the specified threshold value and 255 for all input values equal or larger than the specified threshold value.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param threshold The threshold dividing the frame into black and white, with range [1, 255]
		 * @param worker Optional worker used to distribute the CPU load
		 */
		static inline void convertY8ToB8(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const uint8_t threshold, Worker* worker = nullptr);

		/**
		 * Converts a gray scale frame into a second binary frame using a relative threshold and a kernel size.
		 * Instead using the gray scale frame only, additional the bordered integral image of the gray scale image is used.<br>
		 * The resulting binary frame uses a relative threshold resulting in better black-white borders.<br>
		 * Beware: The specified kernel size must have an odd value.<br>
		 * Beware: The border size of the integral image must have size: (kernel - 1) / 2 be the same as the used kernel size.<br>
		 * The corresponding integral image can be created with IntegralImage::createBorderedImage().<br>
		 * The resulting target pixel values will be 0 for all input values smaller than the specified threshold value and 255 for all input values equal or larger than the specified threshold value.<br>
		 * Compare: Adaptive Thresholding Using the Integral Image, Bradley
		 * @param source The source frame buffer of the normal 8 bit gray scale image, must be valid
		 * @param sourceBorderedIntegral Source frame buffer of the integral image, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param sourceBorderedIntegralPaddingElements The number of padding elements at the end of each source integral row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param kernelSize Size of the kernel used for adaptive thresholding, the kernel size must be: (integralBorder * 2) + 1
		 * @param threshold The threshold (percentage) dividing the frame adaptive into black and white, with range (0, 1)
		 * @param worker Optional worker used to distribute the CPU load
		 * @see IntegralImage::createBorderedImage().
		 */
		static inline void convertBorderedY8ToB8(const uint8_t* source, const uint32_t* sourceBorderedIntegral, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int sourceBorderedIntegralPaddingElements, const unsigned int targetPaddingElements, const unsigned int kernelSize, const float threshold, Worker* worker = nullptr);

		/**
		 * Calculate binarization threshold for a given image by application of the Otsu algorithm.
		 * The implementation follows the work of N. Otsu: "A threshold selection method from gray-level histograms", IEEE Trans. Systems, Man and Cybernetics 9(1), pp. 62-66, 1979.
		 * @param source The source frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 * @return Returns binarization threshold, with range [0, 255]
		 * @see convertY8ToB8()
		 *
		 * The following tutorial shows how this function may be used:
		 * @code
		 * bool applyOtsu(const Frame& source, Frame& target, Worker* worker)
		 * {
		 *     Frame yFrame;
		 *     if (!CV::FrameConverter::Comfort::convert(source, FrameType::FORMAT_Y8, yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
		 *     {
		 *         ocean_assert(false && "Unsupported pixel format!");
		 *         return false;
		 *     }
		 *
		 *     const uint8_t otsuThreshold = CV::FrameConverterThreshold::calculateOtsuThreshold(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), worker);
		 *
		 *     if (!target.set(yFrame.frameType(), true, true))
		 *     {
		 *         return false;
		 *     }
		 *
		 *     CV::FrameConverterThreshold::convertY8ToB8(yFrame.constdata<uint8_t>(), target.data<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), target.paddingElements(), otsuThreshold, worker);
		 *
		 *     return true;
		 * }
		 * @endcode
		 */
		static uint8_t calculateOtsuThreshold(const uint8_t* source, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, Worker* worker = nullptr);

	protected:

		/**
		 * Converts an 8 bit gray scale frame into a binary frame using a threshold.
		 * The resulting target pixel values will be 0 for all input values smaller than the specified threshold value and 255 for all input values equal or larger than the specified threshold value.
		 * @param buffer Source and target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param bufferPaddingElements The number of padding elements at the end of each buffer row, in elements, with range [0, infinity)
		 * @param threshold The threshold dividing the frame into black and white, with range [0, 255]
		 * @param firstRow First (including) row to convert, with range [0, height)
		 * @param numberRows Number of rows to convert, with range [1, height - firstRow]
		 */
		static void convertY8ToB8Subset(uint8_t* buffer, const unsigned int width, const unsigned int height, const unsigned int bufferPaddingElements, const uint8_t threshold, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Converts an 8 bit gray scale frame into a second binary frame using a threshold.
		 * The resulting target pixel values will be 0 for all input values smaller than the specified threshold value and 255 for all input values equal or larger than the specified threshold value.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param threshold The threshold dividing the frame into black and white, with range [0, 255]
		 * @param firstRow First (including) row to convert, with range [0, height)
		 * @param numberRows Number of rows to convert, with range [1, height - firstRow]
		 */
		static void convertY8ToB8Subset(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const uint8_t threshold, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Converts a gray scale frame into a second binary frame using a relative threshold and a kernel size.
		 * Instead using the gray scale frame only, additional the bordered integral image of the gray scale image is used.<br>
		 * The resulting binary frame uses a relative threshold resulting in better black-white borders.<br>
		 * Beware: The specified kernel size must have an odd value.<br>
		 * Beware: The border size of the integral image must have size: (kernel - 1) / 2 be the same as the used kernel size.<br>
		 * The corresponding integral image can be created with IntegralImage::createBorderedImage().<br>
		 * The resulting target pixel values will be 0 for all input values smaller than the specified threshold value and 255 for all input values equal or larger than the specified threshold value.<br>
		 * Compare: Adaptive Thresholding Using the Integral Image, Bradley
		 * @param source The source frame buffer of the normal 8 bit gray scale image
		 * @param sourceBorderedIntegral Source frame buffer of the integral image
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param sourceBorderedIntegralPaddingElements The number of padding elements at the end of each source integral row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param kernelSize Size of the kernel used for adaptive thresholding, the kernel size must be: (integralBorder * 2) + 1
		 * @param threshold The threshold (percentage) dividing the frame adaptive into black and white, with range (0, 1)
		 * @param firstRow First (including) row to convert, with range [0, height)
		 * @param numberRows Number of rows to convert, with range [1, height - firstRow]
		 * @see IntegralImage::createBorderedImage().
		 */
		static void convertBorderedY8ToB8Subset(const uint8_t* source, const uint32_t* sourceBorderedIntegral, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int sourceBorderedIntegralPaddingElements, const unsigned int targetPaddingElements, const unsigned int kernelSize, const float threshold, const unsigned int firstRow, const unsigned int numberRows);
};

inline void FrameConverterThreshold::convertY8ToB8(uint8_t* buffer, const unsigned int width, const unsigned int height, const unsigned int bufferPaddingElements, const uint8_t threshold, Worker* worker)
{
	ocean_assert(buffer != nullptr);
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(threshold >= 1u);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameConverterThreshold::convertY8ToB8Subset, buffer, width, height, bufferPaddingElements, threshold, 0u, 0u), 0u, height);
	}
	else
	{
		convertY8ToB8Subset(buffer, width, height, bufferPaddingElements, threshold, 0u, height);
	}
}

inline void FrameConverterThreshold::convertY8ToB8(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const uint8_t threshold, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(threshold >= 1u);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameConverterThreshold::convertY8ToB8Subset, source, target, width, height, sourcePaddingElements, targetPaddingElements, threshold, 0u, 0u), 0u, height);
	}
	else
	{
		convertY8ToB8Subset(source, target, width, height, sourcePaddingElements, targetPaddingElements, threshold, 0u, height);
	}
}

inline void FrameConverterThreshold::convertBorderedY8ToB8(const uint8_t* source, const uint32_t* sourceBorderedIntegral, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int sourceBorderedIntegralPaddingElements, const unsigned int targetPaddingElements, const unsigned int kernelSize, const float threshold, Worker* worker)
{
	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&convertBorderedY8ToB8Subset, source, sourceBorderedIntegral, target, width, height, sourcePaddingElements, sourceBorderedIntegralPaddingElements, targetPaddingElements, kernelSize, threshold, 0u, 0u), 0, height);
	}
	else
	{
		convertBorderedY8ToB8Subset(source, sourceBorderedIntegral, target, width, height, sourcePaddingElements, sourceBorderedIntegralPaddingElements, targetPaddingElements, kernelSize, threshold, 0u, height);
	}
}

}

}

#endif // META_OCEAN_CV_FRAME_CONVERTER_THRESHOLD_H
