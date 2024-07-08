/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_INTERPOLATOR_H
#define META_OCEAN_CV_FRAME_INTERPOLATOR_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"
#include "ocean/cv/FrameInterpolatorNearestPixel.h"
#include "ocean/cv/FramePyramid.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements functions interpolating frames and image content.
 * In general, this class is just a thin wrapper around the actual implementation based on the desired interpolation method.<br>
 * Please be aware that all non-template-based functions of this class are intended for prototyping only.<br>
 * Binary size can increase significantly when using non-template-based functions as the wrapper will add binary size of every interpolation method.<br>
 * Thus, in case binary size matters, use the template-based functions directly.<br>
 * For more details and a visual comparisons of the available image resizing methods see https://facebookresearch.github.io/ocean/docs/images/resizing/
 * @see FrameInterpolatorBilinear, FrameInterpolatorNearestPixel.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameInterpolator
{
	public:

		/**
		 * Definition of individual interpolation methods.
		 */
		enum InterpolationMethod
		{
			/// An invalid interpolation method.
			IM_INVALID,
			/// An interpolation applying a nearest pixel (nearest neighbor) lookup.
			IM_NEAREST_PIXEL,
			/// An interpolation applying a bilinear interpolation.
			IM_BILINEAR
		};

		/**
		 * Definition of individual resize methods.
		 * Commonly, higher enum values will create better image qualities, while also will need more computational time.
		 */
		enum ResizeMethod
		{
			/// An invalid resize method.
			RM_INVALID,
			/// An interpolation applying a nearest pixel (nearest neighbor) lookup.
			RM_NEAREST_PIXEL,
			/// An interpolation applying a bilinear interpolation.
			RM_BILINEAR,
			/// A two-step interpolation, first applying a pyramid down sampling with a 11 filtering, followed by bilinear interpolation from pyramid layer to target image.
			RM_NEAREST_PYRAMID_LAYER_11_BILINEAR,
			/// A two-step interpolation, first applying a pyramid down sampling with a 14641 filtering, followed by bilinear interpolation from pyramid layer to target image.
			RM_NEAREST_PYRAMID_LAYER_14641_BILINEAR,
			/// The resize method with best quality/performance ratio providing high image qualities with good performance values.
			RM_AUTOMATIC = RM_NEAREST_PYRAMID_LAYER_11_BILINEAR
		};

	public:

		/**
		 * Resizes/rescales a given frame by application of a specified interpolation method.
		 * @param frame The frame to resize, must not have a packed pixel format, must be valid
		 * @param width The width of the resized frame in pixel, with range [1, infinity)
		 * @param height The height of the resized frame in pixel, with range [1, infinity)
		 * @param resizeMethod The resize method to be used
		 * @param worker Optional worker object used for load distribution, must be valid
		 * @return True, if the frame could be resized
		 * @see FrameType::formatIsPacked().
		 */
		static bool resize(Frame& frame, const unsigned int width, const unsigned int height, const ResizeMethod resizeMethod = RM_AUTOMATIC, Worker* worker = nullptr);

		/**
		 * Resizes/rescales a given frame by application of a specified interpolation method.
		 * @param source The source frame to resize, must not have a packed pixel format, must be valid
		 * @param target Resulting target frame with identical frame pixel format and pixel origin as the source frame, must be valid
		 * @param resizeMethod The resize method to be used
		 * @param worker Optional worker object used for load distribution, must be valid
		 * @return True, if the frame could be resized
		 * @see FrameType::formatIsPacked().
		 */
		static bool resize(const Frame& source, Frame& target, const ResizeMethod resizeMethod = RM_AUTOMATIC, Worker* worker = nullptr);

		/**
		 * Resizes/rescales a given 1-plane frame by application of a specified interpolation method.
		 * This template-based implementation ensures that the binary impact is as small as possible.
		 * @param source The source frame buffer to resize, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param sourceWidth Width of the source frame in pixel, with range [1, infinity)
		 * @param sourceHeight Height of the source frame in pixel, with range [1, infinity)
		 * @param targetWidth Width of the target frame in pixel, with range [1, infinity)
		 * @param targetHeight Height of the target frame in pixel, with range [1, infinity)
		 * @param sourcePaddingElements Optional padding at the end of each source row in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional padding at the end of each target row in elements, with range [0, infinity)
		 * @param worker Optional worker object used for load distribution
		 * @return True, if the frame could be resized
		 * @tparam T Data type of each pixel channel, e.g., 'uint8_t', 'float'
		 * @tparam tChannels Number of channels of the frame, with range [1, infinity)
		 * @tparam tResizeMethod The resize method to be used
		 */
		template <typename T, unsigned int tChannels, ResizeMethod tResizeMethod = RM_AUTOMATIC>
		static bool resize(const T* source, T* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Applies an affine transformation to an image (with zipped pixel format).
		 * The output frame must have the same pixel format and pixel origin as the input frame, however the dimension (and position) of the output frame can be arbitrary.<br>
		 * This function allows the creation of an output frame fully covering the input frame (if the position and dimension of the output frame covers the transformation of the affine transformation.<br>
		 * The multiplication of the affine transformation with pixel location in the output image yield their location in the input image, i.e., inputPoint = affineTransform * outputPoint.<br>
		 * The parameter 'outputOrigin' applies an additional translation to the provided affine transformation i.e., input_A_output * create_translation_matrix3x3(outputOrigin.x(), outputOrigin.y()).<br>
		 * Please note that here the affine transformation is specified as a 3-by-3 matrix (in contrast to the more commonly used 2-by-3 matrix) and should take of the form:
		 * <pre>
		 * Rxx  Ryx  Tx
		 * Rxy  Ryy  Ty
		 *  0    0    1
		 * </pre>
		 * However, this function disregards the last row completely and only uses the top two rows, i.e., the elements a through f.
		 * Information: This function is the equivalent to OpenCV's cv::warpAffine().<br>
		 * Note: For applications running on mobile devices, in order to keep the impact on binary size to a minimum please prefer a specialized transformation function (those that work on image pointers instead of Frame instances).
		 * @param input The input frame that will be transformed, must be valid
		 * @param output The resulting frame after applying the affine transformation to the input frame; pixel format and pixel origin must be identical to input frame; memory of output frame must be allocated by the caller
		 * @param input_A_output The affine transform used to transform the given input frame, transforming points defined in the output frame into points defined in the input frame
		 * @param interpolationMethod The interpolation method to be used, must be either IM_BILINEAR or IM_NEAREST_PIXEL
		 * @param borderColor Color of undefined pixel positions, the size of the buffer must match to the number of channels, nullptr to assign 0x00 to each channel
		 * @param worker Optional worker object to distribute the computational load
		 * @param outputOrigin The origin of the output frame defining the global position of the output frame's pixel coordinate (0, 0), with range (-infinity, infinity)x(-infinity, infinity)
		 * @return True, if succeeded
		 */
		static bool affine(const Frame& input, Frame& output, const SquareMatrix3& input_A_output, const InterpolationMethod interpolationMethod = IM_BILINEAR, const uint8_t* borderColor = nullptr, Worker* worker = nullptr, const PixelPositionI& outputOrigin = PixelPositionI(0, 0));

		/**
		 * Transforms a given input frame (with zipped pixel format) into an output frame (with arbitrary frame dimension) by application of a homography.
		 * The output frame must have the same pixel format and pixel origin as the input frame, however the dimension (and position) of the output frame can be arbitrary.<br>
		 * This function allows the creation of an output frame fully covering the input frame (if the position and dimension of the output frame covers the transformation of the homography.<br>
		 * The homography given defines the transformation of output pixels to input pixels (inputPoint = homography * outputPoint).<br>
		 * The 'outputOrigin' parameter simply applies an additional translation onto the provided homography i.e., homography * create_translation_matrix3x3(outputOrigin.x(), outputOrigin.y()).<br>
		 * Information: This function is the equivalent to OpenCV's cv::warpPerspective().
		 * @param input The input frame that will be transformed, must be valid
		 * @param output The output frame resulting by application of the given homography, with same pixel format and pixel origin as the input frame, must be valid
		 * @param input_H_output The homography used to transform the given input frame, transforming points defined in the output frame into points defined in the input frame
		 * @param interpolationMethod The interpolation method to be used, must be either IM_BILINEAR or IM_NEAREST_PIXEL
		 * @param borderColor Color of undefined pixel positions, the size of the buffer must match to the number of channels, nullptr to assign 0x00 to each channel
		 * @param worker Optional worker object to distribute the computational load
		 * @param outputOrigin The origin of the output frame defining the global position of the output frame's pixel coordinate (0, 0), with range (-infinity, infinity)x(-infinity, infinity)
		 * @return True, if succeeded
		 */
		static bool homography(const Frame& input, Frame& output, const SquareMatrix3& input_H_output, const InterpolationMethod interpolationMethod = IM_BILINEAR, const uint8_t* borderColor = nullptr, Worker* worker = nullptr, const PixelPositionI& outputOrigin = PixelPositionI(0, 0));

	protected:

		/**
		 * Resizes/rescales a given 1-plane frame by application of a specified interpolation method.
		 * This template-based implementation ensures that the binary impact is as small as possible.
		 * @param source The source frame buffer to resize, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param sourceWidth Width of the source frame in pixel, with range [1, infinity)
		 * @param sourceHeight Height of the source frame in pixel, with range [1, infinity)
		 * @param targetWidth Width of the target frame in pixel, with range [1, infinity)
		 * @param targetHeight Height of the target frame in pixel, with range [1, infinity)
		 * @param sourcePaddingElements Optional padding at the end of each source row in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional padding at the end of each target row in elements, with range [0, infinity)
		 * @param resizeMethod The resize method to be used
		 * @param worker Optional worker object used for load distribution
		 * @return True, if the frame could be resized
		 * @tparam T Data type of each pixel channel, e.g., 'uint8_t', 'float'
		 * @tparam tChannels Number of channels of the frame, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static bool resize(const T* source, T* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const ResizeMethod resizeMethod, Worker* worker = nullptr);
};

template <typename T, unsigned int tChannels, FrameInterpolator::ResizeMethod tResizeMethod>
bool FrameInterpolator::resize(const T* source, T* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tResizeMethod != RM_INVALID, "Invalid resize method!");
	static_assert((std::is_same<T, uint8_t>::value || tResizeMethod != RM_NEAREST_PYRAMID_LAYER_14641_BILINEAR), "Resize method is not supported for this data type!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(sourceWidth != 0u && sourceHeight != 0u);
	ocean_assert(targetWidth != 0u && targetHeight != 0u);

	if (source == nullptr || target == nullptr || sourceWidth == 0u || sourceHeight == 0u || targetWidth == 0u || targetHeight == 0u)
	{
		return false;
	}

	if (sourceWidth == targetWidth && sourceHeight == targetHeight)
	{
		const bool result = CV::FrameConverter::subFrame<T>(source, target, sourceWidth, sourceHeight, sourceWidth, sourceHeight, tChannels, 0u, 0u, 0u, 0u, sourceWidth, sourceHeight, sourcePaddingElements, targetPaddingElements);
		ocean_assert(result);

		return result;
	}

	if constexpr (tResizeMethod == RM_NEAREST_PIXEL)
	{
		FrameInterpolatorNearestPixel::resize<T, tChannels>(source, target, sourceWidth, sourceHeight, targetWidth, targetHeight, sourcePaddingElements, targetPaddingElements, worker);
		return true;
	}

	if constexpr (tResizeMethod == RM_BILINEAR)
	{
		FrameInterpolatorBilinear::resize<T, tChannels>(source, target, sourceWidth, sourceHeight, targetWidth, targetHeight, sourcePaddingElements, targetPaddingElements, worker);
		return true;
	}

	if constexpr (tResizeMethod == RM_NEAREST_PYRAMID_LAYER_11_BILINEAR || tResizeMethod == RM_NEAREST_PYRAMID_LAYER_14641_BILINEAR)
	{
		if constexpr (std::is_same<T, uint8_t>::value == false)
		{
			ocean_assert(false && "Missing implementation!");
			return false;
		}

		const uint8_t* const source_u8 = (const uint8_t*)(source);
		uint8_t* const target_u8 = (uint8_t*)(target);

		if (sourceWidth / 2u == targetWidth && sourceHeight / 2u == targetHeight)
		{
			if constexpr (tResizeMethod == RM_NEAREST_PYRAMID_LAYER_11_BILINEAR)
			{
				FrameShrinker::downsampleByTwo8BitPerChannel11(source_u8, target_u8, sourceWidth, sourceHeight, tChannels, sourcePaddingElements, targetPaddingElements, worker);
			}
			else
			{
				ocean_assert(tResizeMethod == RM_NEAREST_PYRAMID_LAYER_14641_BILINEAR);
				FrameShrinker::downsampleByTwo8BitPerChannel14641(source_u8, target_u8, sourceWidth, sourceHeight, targetWidth, targetHeight, tChannels, sourcePaddingElements, targetPaddingElements, worker);
			}

			return true;
		}
		else if (targetWidth < sourceWidth && targetHeight < sourceHeight)
		{
			ocean_assert(targetWidth > 0u && targetHeight >= 0u);
			const unsigned int invalidCoarsestWidth = targetWidth - 1u;
			const unsigned int invalidCoarsestHeight = targetHeight - 1u;

			unsigned int coarsestLayerWidth = 0u;
			unsigned int coarsestLayerHeight = 0u;

			unsigned int layers = CV::FramePyramid::idealLayers(sourceWidth, sourceHeight, invalidCoarsestWidth, invalidCoarsestHeight, &coarsestLayerWidth, &coarsestLayerHeight);

			if (layers == 0u)
			{
				ocean_assert(false && "This should never happen!");
				return false;
			}

			if (coarsestLayerWidth == targetWidth && coarsestLayerHeight == targetHeight)
			{
				// the target frame matches with the resolution of the last pyramid layer, so that we can avoid copying the memory from the coarsest pyramid layer

				ocean_assert(layers >= 2u);
				layers -= 1u;
			}

			if (layers >= 2u)
			{
				constexpr FrameType::PixelOrigin anyPixelOrientation = FrameType::ORIGIN_UPPER_LEFT;

				FramePyramid framePyramid;

				if constexpr (tResizeMethod == RM_NEAREST_PYRAMID_LAYER_11_BILINEAR)
				{
					framePyramid = FramePyramid(source_u8, sourceWidth, sourceHeight, tChannels, anyPixelOrientation, layers, sourcePaddingElements, false /*copyFirstLayer*/, worker);
				}
				else
				{
					ocean_assert(tResizeMethod == RM_NEAREST_PYRAMID_LAYER_14641_BILINEAR);
					framePyramid = FramePyramid(source_u8, sourceWidth, sourceHeight, tChannels, anyPixelOrientation, FramePyramid::DM_FILTER_14641, layers, sourcePaddingElements, false /*copyFirstLayer*/, worker);
				}

				if (!framePyramid.isValid())
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				const Frame& coarsestPyramidLayer = framePyramid.coarsestLayer();

				FrameInterpolatorBilinear::resize<T, tChannels>(coarsestPyramidLayer.constdata<T>(), target, coarsestPyramidLayer.width(), coarsestPyramidLayer.height(), targetWidth, targetHeight, coarsestPyramidLayer.paddingElements(), targetPaddingElements, worker);
				return true;
			}
		}

		FrameInterpolatorBilinear::resize<T, tChannels>(source, target, sourceWidth, sourceHeight, targetWidth, targetHeight, sourcePaddingElements, targetPaddingElements, worker);
		return true;
	}

	ocean_assert(false && "Invalid interpolation type!");
	return false;
}

}

}

#endif // META_OCEAN_CV_FRAME_INTERPOLATOR_BICUBIC_H
