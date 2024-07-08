/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_ADVANCED_ADVANCED_FRAME_INTERPOLATOR_BILINEAR_H
#define META_OCEAN_CV_ADVANCED_ADVANCED_FRAME_INTERPOLATOR_BILINEAR_H

#include "ocean/cv/advanced/Advanced.h"
#include "ocean/cv/advanced/AdvancedFrameInterpolatorBilinearBase.h"
#include "ocean/cv/advanced/AdvancedFrameInterpolatorBilinearNEON.h"
#include "ocean/cv/advanced/AdvancedFrameInterpolatorBilinearSSE.h"

#include "ocean/base/DataType.h"
#include "ocean/base/Frame.h"
#include "ocean/base/Utilities.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/FrameInterpolatorBilinear.h"
#include "ocean/cv/PixelBoundingBox.h"
#include "ocean/cv/SumSquareDifferences.h"

#include "ocean/cv/advanced/PixelLine.h"
#include "ocean/cv/advanced/PixelTriangle.h"

#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/Triangle2.h"
#include "ocean/math/Vector2.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

/**
 * This class implements an advanced bilinear frame interpolator.
 * @ingroup cvadvanced
 */
class OCEAN_CV_ADVANCED_EXPORT AdvancedFrameInterpolatorBilinear
{
	public:

		/**
		 * The following comfort class provides comfortable functions simplifying prototyping applications but also increasing binary size of the resulting applications.
		 * Best practice is to avoid using these functions if binary size matters,<br>
		 * as for every comfort function a corresponding function exists with specialized functionality not increasing binary size significantly.<br>
		 */
		class OCEAN_CV_ADVANCED_EXPORT Comfort
		{
			public:

				/**
				 * Transforms a given input frame into an output frame (with arbitrary frame dimension) by application of a homography.
				 * The output frame must have the same pixel format and pixel origin as the input frame, however the dimension of the output frame can be arbitrary.<br>
				 * Further, this function uses a filter mask for output pixels, only valid output mask pixels will be interpolated.<br>
				 * In case the input position lies outside the input frame's domain, the position will be clamped to ensure a valid interpolation result for each pixel with valid filter mask.<br>
				 * The frame must have a 1-plane pixel format with DT_UNSIGNED_INTEGER_8 as data type (e.g., FORMAT_Y8, FORMAT_RGB24, FORMAT_RGBA32, ...).
				 * @param input The input frame that will be transformed, must be valid
				 * @param outputFilterMask The filter mask for the output image, with same resolution as the output frame, must be valid
				 * @param output The Output frame resulting by application of the given homography, with same pixel format and pixel origin as the input frame, must have a valid dimension
				 * @param input_H_output Homography used to transform the given input frame by following equation: inputPoint = input_H_output * outputPoint, must be valid
				 * @param outputBoundingBox Optional bounding box to apply the interpolation to a subset of the output frame only, invalid to handle the entire output frame
				 * @param worker Optional worker object to distribute the computational load
				 * @return True, if succeeded
				 */
				static bool homographyFilterMask(const Frame& input, const Frame& outputFilterMask, Frame& output, const SquareMatrix3& input_H_output, const PixelBoundingBox& outputBoundingBox = PixelBoundingBox(), Worker* worker = nullptr);

				/**
				 * Determines the interpolated pixel values for a given pixel position in an 8 bit per channel frame which has a corresponding mask frame specifying valid and invalid pixels in the frame.
				 * This function allows an interpolation position outside the frame due to the resulting mask value.<br>
				 * This function uses an integer interpolation with a precision of 1/128.
				 * @param frame The frame to determine the pixel values from, must be valid
				 * @param mask The mask frame specifying valid and invalid frame pixels, must be valid
				 * @param channels Number of channels of the given frame, with range [1, 8]
				 * @param width The width of the frame (and mask) in pixel, with range [1, infinity)
				 * @param height The height of the frame (and mask) in pixel, with range [1, infinity)
				 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
				 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
				 * @param pixelCenter The pixel center to be used during interpolation, either 'PC_TOP_LEFT' or 'PC_CENTER'
				 * @param position The position for which the interpolated pixel will be determined, with ranges (-infinity, infinity)x(-infinity, infinity), for PC_TOP_LEFT and PC_CENTER
				 * @param result Resulting pixel values, must be valid, must be valid
				 * @param resultMask The resulting mask value, must be valid
				 * @param maskValue The 8 bit mask value for valid pixels, an invalid pixel is defined by 0xFF - maskValue
				 * @return True, if succeeded
				 * @tparam TScalar The scalar data type of the sub-pixel position
				 */
				template <typename TScalar = Scalar>
				static inline bool interpolatePixelWithMask8BitPerChannel(const uint8_t* frame, const uint8_t* mask, const unsigned int channels, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const PixelCenter pixelCenter, const VectorT2<TScalar>& position, uint8_t* result, uint8_t& resultMask, const uint8_t maskValue = 0xFFu);
		};

	public:

		/**
		 * Interpolates the content of an image patch with sub-pixel accuracy inside a given image and stores the interpolated data into a buffer.
		 * @param frame The image in which the interpolated patch is located, must be valid
		 * @param width The width of the image, in pixel, with range [patchWidth + 1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param buffer The target buffer with `tChannels * patchWidth * patchHeight` elements, must be valid
		 * @param position The center position of the patch in the image, with range [patchWidth/2, width - patchWidth/2 - 1)x[patchHeight/2, height - patchHeight/2 - 1) for PC_TOP_LEFT,  [0.5 + patchWidth/2, width - patchWidth/2 - 0.5)x[0.5 + patchHeight/2, height - patchHeight/2 - 0.5) for PC_CENTER
		 * @param patchWidth The width of the image patch in pixel, with range [1, infinity), must be odd
		 * @param patchHeight The height of the image patch in pixel, with range [1, infinity), must be odd
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tPixelCenter The pixel center to be used during interpolation, either 'PC_TOP_LEFT' or 'PC_CENTER'
		 * @tparam TScalar The scalar data type of the sub-pixel position
		 */
		template <unsigned int tChannels, PixelCenter tPixelCenter = PC_TOP_LEFT, typename TScalar = Scalar>
		static inline void interpolatePatch8BitPerChannel(const uint8_t* const frame, const unsigned int width, const unsigned int framePaddingElements, uint8_t* const buffer, const VectorT2<TScalar>& position, const unsigned int patchWidth, const unsigned int patchHeight);

		/**
		 * Interpolates the content of a square image patch with sub-pixel accuracy inside a given image and stores the interpolated data into a buffer.
		 * @param frame The image in which the interpolated patch is located, must be valid
		 * @param width The width of the image, in pixel, with range [tPatchSize + 1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param buffer The target buffer with `tChannels * tPatchSize * tPatchSize` elements, must be valid
		 * @param position The center position of the square region in the image, with range [tPatchSize/2, width - tPatchSize/2 - 1)x[tPatchSize/2, height - tPatchSize/2 - 1) for PC_TOP_LEFT, [tPatchSize/2, width - tPatchSize/2]x[tPatchSize/2, height - tPatchSize/2] for PC_CENTER
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 * @tparam tPixelCenter The pixel center to be used during interpolation, either 'PC_TOP_LEFT' or 'PC_CENTER'
		 */
		template <unsigned int tChannels, unsigned int tPatchSize, PixelCenter tPixelCenter = PC_TOP_LEFT>
		static inline void interpolateSquarePatch8BitPerChannel(const uint8_t* const frame, const unsigned int width, const unsigned int framePaddingElements, uint8_t* const buffer, const Vector2& position);

		/**
		 * Interpolates the content of an image patch with sub-pixel accuracy inside a given image and stores the interpolated data into a buffer.
		 * An additional mask frame specifies valid and invalid pixels.<br>
		 * This function allows an interpolation positions outside the frame due to the resulting mask.<br>
		 * @param frame The image in which the interpolated patch is located, must be valid
		 * @param mask The mask frame specifying valid and invalid frame pixels, must be valid
		 * @param width The width of the frame (and mask) in pixel, with range [1, infinity)
		 * @param height The height of the frame (and mask) in pixel, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param position The center position of the patch in the image, with range (-infinity, infinity)x(-infinity, infinity) for PC_TOP_LEFT and PC_CENTER
		 * @param patch The target patch, must be valid
		 * @param patchMask The target patch mask, must be valid
		 * @param patchWidth The width of the image patch in pixel, with range [1, infinity)
		 * @param patchHeight The height of the image patch in pixel, with range [1, infinity)
		 * @param patchPaddingElements The number of padding elements at the end of each patch row, in elements, with range [0, infinity)
		 * @param patchMaskPaddingElements The number of padding elements at the end of each patch mask row, in elements, with range [0, infinity)
		 * @param maskValue The 8 bit mask value for valid pixels, an invalid pixel is defined by 0xFF - maskValue
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tPixelCenter The pixel center to be used during interpolation, either 'PC_TOP_LEFT' or 'PC_CENTER'
		 * @tparam TScalar The scalar data type of the sub-pixel position
		 */
		template <unsigned int tChannels, PixelCenter tPixelCenter, typename TScalar = Scalar>
		static void interpolatePatchWithMask8BitPerChannel(const uint8_t* frame, const uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const VectorT2<TScalar>& position, uint8_t* patch, uint8_t* patchMask, const unsigned int patchWidth, const unsigned int patchHeight, const unsigned int patchPaddingElements, const unsigned int patchMaskPaddingElements, const uint8_t maskValue = 0xFFu);

		/**
		 * Interpolates the content of a square image patch with sub-pixel accuracy inside a given image and stores the interpolated data into a buffer.
		 * Pixels in the square region pointing outside the frame are mirrored back into the frame.
		 * @param frame The image in which the interpolated patch is located, must be valid
		 * @param width The width of the image, in pixel, with range [tPatchSize/2 + 1, infinity)
		 * @param height The height of the image, in pixel, with range [tPatchSize/2 + 1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param buffer The target buffer with `tChannels * patchSize * patchSize` elements, must be valid
		 * @param position Center position of the square region in the source frame, with range [0, width)x[0, height)
		 * @param patchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 * @tparam tChannels The number of data channels of the frame data, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void interpolateSquareMirroredBorder8BitPerChannel(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, uint8_t* const buffer, const Vector2& position, const unsigned int patchSize);

		/**
		 * Interpolates the content of a square region inside a given frame into a buffer with size of the square window.
		 * Pixels in the square region pointing outside the frame are mirrored back into the frame.
		 * @param frame The image in which the interpolated patch is located, must be valid
		 * @param width The width of the image, in pixel, with range [tPatchSize/2 + 1, infinity)
		 * @param height The height of the image, in pixel, with range [tPatchSize/2 + 1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param buffer The target buffer with `tChannels * patchSize * patchSize` elements, must be valid
		 * @param position Center position of the square region in the source frame, with range [0, width)x[0, height)
		 * @tparam tChannels The number of data channels of the frame data, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static void interpolateSquareMirroredBorder8BitPerChannel(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, uint8_t* buffer, const Vector2& position);

		/**
		 * Interpolates the content of a source triangle to target triangle.
		 * @param source The source frame, must be valid
		 * @param target The target frame, must be valid
		 * @param sourceTriangle The source triangle holding the frame content to be interpolated, must be valid
		 * @param targetTriangle The target triangle receiving the frame content to be interpolated, must be valid
		 * @param sourceWidth The width of the source frame in pixel, with range [1, infinity)
		 * @param sourceHeight The height of the source frame in pixel, with range [1, infinity)
		 * @param targetWidth The width of the target frame in pixel, with range [1, infinity)
		 * @param targetHeight The height of the target frame in pixel, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker An optional worker object to distribute the computation
		 * @tparam tChannels The number of frame channels of the source and target frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void interpolateTriangle8BitPerChannel(const uint8_t* source, uint8_t* target, const PixelTriangle& sourceTriangle, const PixelTriangle& targetTriangle, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Interpolates the content of a source triangle to target triangle.
		 * @param source The source frame, must be valid
		 * @param target The target frame, must be valid
		 * @param sourceTriangle The source triangle holding the frame content to be interpolated, must be valid
		 * @param targetTriangle The target triangle receiving the frame content to be interpolated, must be valid
		 * @param sourceWidth The width of the source frame in pixel, with range [1, infinity)
		 * @param sourceHeight The height of the source frame in pixel, with range [1, infinity)
		 * @param targetWidth The width of the target frame in pixel, with range [1, infinity)
		 * @param targetHeight The height of the target frame in pixel, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker An optional worker object to distribute the computation
		 * @tparam tChannels The number of frame channels of the source and target frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void interpolateTriangle8BitPerChannel(const uint8_t* source, uint8_t* target, const Triangle2& sourceTriangle, const Triangle2& targetTriangle, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Interpolates the content of a triangle to another triangle.
		 * @param source The source frame, must be valid
		 * @param target The target frame, must be valid
		 * @param sourceTriangle The source triangle defined in the source frame, must be valid
		 * @param targetTriangle The target triangle defined int he target frame, must be valid
		 * @param worker An optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool interpolateTriangle(const Frame& source, Frame& target, const PixelTriangle& sourceTriangle, const PixelTriangle& targetTriangle, Worker* worker = nullptr);

		/**
		 * Interpolates the content of a triangle to another triangle.
		 * @param source The source frame, must be valid
		 * @param target The target frame, must be valid
		 * @param sourceTriangle The source triangle defined in the source frame, must be valid
		 * @param targetTriangle The target triangle defined int he target frame, must be valid
		 * @param worker An optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool interpolateTriangle(const Frame& source, Frame& target, const Triangle2& sourceTriangle, const Triangle2& targetTriangle, Worker* worker = nullptr);

		/**
		 * Determines the interpolated pixel values for a given pixel position in an 8 bit per channel frame which has a corresponding mask frame specifying valid and invalid pixels in the frame.
		 * This function allows an interpolation position outside the frame due to the resulting mask value.<br>
		 * This function uses an integer interpolation with a precision of 1/128.
		 * @param frame The frame to determine the pixel values from, must be valid
		 * @param mask The mask frame specifying valid and invalid frame pixels, must be valid
		 * @param width The width of the frame (and mask) in pixel, with range [1, infinity)
		 * @param height The height of the frame (and mask) in pixel, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param position The position for which the interpolated pixel will be determined, with ranges (-infinity, infinity)x(-infinity, infinity), for PC_TOP_LEFT and PC_CENTER
		 * @param result Resulting pixel values, must be valid, must be valid
		 * @param resultMask The resulting mask value, must be valid
		 * @param maskValue The 8 bit mask value for valid pixels, an invalid pixel is defined by 0xFF - maskValue
		 * @tparam tChannels Number of channels of the given frame, with range [1, infinity)
		 * @tparam tPixelCenter The pixel center to be used during interpolation, either 'PC_TOP_LEFT' or 'PC_CENTER'
		 * @tparam TScalar The scalar data type of the sub-pixel position
		 */
		template <unsigned int tChannels, PixelCenter tPixelCenter = PC_TOP_LEFT, typename TScalar = Scalar>
		static inline void interpolatePixelWithMask8BitPerChannel(const uint8_t* frame, const uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const VectorT2<TScalar>& position, uint8_t* result, uint8_t& resultMask, const uint8_t maskValue = 0xFFu);

		/**
		 * Transforms a subset of a given input frame into an output frame (with arbitrary frame dimension) by application of a homography.
		 * The output frame must have the same pixel format and pixel origin as the input frame, however the dimension of the output frame can be arbitrary.<br>
		 * Further, this function uses a filter mask for output pixels, only valid output mask pixels will be interpolated.<br>
		 * In case the input position lies outside the input frame's domain, the position will be clamped to ensure a valid interpolation result for each pixel with valid filter mask.
		 * @param input The input frame that will be transformed, must be valid
		 * @param outputFilterMask The filter mask for the output image, with same resolution as the output frame, must be valid
		 * @param output The Output frame resulting by application of the given homography, with same pixel format and pixel origin as the input frame, must have a valid dimension
		 * @param inputWidth The width of the input frame, in pixels, with range [1, infinity)
		 * @param inputHeight The height of the input frame, in pixels, with range [1, infinity)
		 * @param outputWidth The width of the output frame (and filter mask), in pixels, with range [1, infinity)
		 * @param outputHeight The height of the output frame (and filter mask), in pixels, with range [1, infinity)
		 * @param inputPaddingElements The number of padding elements at the end of each input row, in elements, with range [0, infinity)
		 * @param outputFilterMaskPaddingElements The number of padding elements at the end of each output filter mask row, in elements, with range [0, infinity)
		 * @param outputPaddingElements The number of padding elements at the end of each output row, in elements, with range [0, infinity)
		 * @param input_H_output Homography used to transform the given input frame by following equation: inputPoint = input_H_output * outputPoint, must be valid
		 * @param outputBoundingBox Optional bounding box to apply the interpolation to a subset of the output frame only, invalid to handle the entire output frame
		 * @param worker Optional worker object to distribute the computational load
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void homographyFilterMask8BitPerChannel(const uint8_t* input, const uint8_t* outputFilterMask, uint8_t* output, const unsigned int inputWidth, const unsigned int inputHeight, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int inputPaddingElements, const unsigned int outputFilterMaskPaddingElements, const unsigned int outputPaddingElements, const SquareMatrix3& input_H_output, const PixelBoundingBox& outputBoundingBox = PixelBoundingBox(), Worker* worker = nullptr);

	private:

		/**
		 * Interpolates the subset of a content of a triangle.
		 * @param source The source frame, must be valid
		 * @param target The target frame, must be valid
		 * @param sourceWidth The width of the source frame in pixel, with range [1, infinity)
		 * @param sourceHeight The height of the source frame in pixel, with range [1, infinity)
		 * @param targetWidth The width of the target frame in pixel, with range [1, infinity)
		 * @param targetHeight The height of the target frame in pixel, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param targetTriangle The 2D triangle defined in the target frame, must be valid
		 * @param sourceTriangle The 2D triangle defined in the source frame, must be valid
		 * @param targetLine01 The pixel line between triangle point 0 and 1
		 * @param targetLine02 The pixel line between triangle point 0 and 2
		 * @param targetLine12 The pixel line between triangle point 1 and 2
		 * @param firstTargetRow The first target row to be handled, with range [0, targetHeight - 1u]
		 * @param numberTargetRows The number of target rows to be handled, with range [1, targetHeight - firstRow
		 * @tparam tChannels The number of channels the source and target frame have, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void interpolateTriangle8BitPerChannelSubset(const uint8_t* source, uint8_t* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const Triangle2* targetTriangle, const Triangle2* sourceTriangle, const PixelLine* targetLine01, const PixelLine* targetLine02, const PixelLine* targetLine12, const unsigned int firstTargetRow, const unsigned int numberTargetRows);

		/**
		 * Transforms a subset of a given input frame into an output frame (with arbitrary frame dimension) by application of a homography.
		 * @param input The input frame that will be transformed, must be valid
		 * @param outputFilterMask The filter mask for the output image, with same resolution as the output frame, must be valid
		 * @param output The Output frame resulting by application of the given homography, with same pixel format and pixel origin as the input frame, must have a valid dimension
		 * @param inputWidth The width of the input frame, in pixels, with range [1, infinity)
		 * @param inputHeight The height of the input frame, in pixels, with range [1, infinity)
		 * @param inputPaddingElements The number of padding elements at the end of each input row, in elements, with range [0, infinity)
		 * @param outputFilterMaskStrideElements The number of stride elements at the end of each output filter mask row, in elements, with range [1, infinity)
		 * @param outputStrideElements The number of stride elements at the end of each output row, in elements, with range [tChannels, infinity)
		 * @param input_H_output Homography used to transform the given input frame by following equation: inputPoint = input_H_output * outputPoint, must be valid
		 * @param firstOutputColumn The first output column to be handled, with range [0, output.width() - 1]
		 * @param numberOutputColumns The number of output columns to be handled, with range [output.width() - firstOutputColumn]
		 * @param firstOutputRow The first output row to be handled, with range [0, output.height() - 1]
		 * @param numberOutputRows The number of output rows to be handled, with range [output.height() - firstOutputRow]
		 */
		template <unsigned int tChannels>
		static void homographyFilterMask8BitPerChannelSubset(const uint8_t* input, const uint8_t* outputFilterMask, uint8_t* output, const unsigned int inputWidth, const unsigned int inputHeight, const unsigned int inputPaddingElements, const unsigned int outputFilterMaskStrideElements, const unsigned int outputStrideElements, const SquareMatrix3* input_H_output, const unsigned int firstOutputColumn, const unsigned int numberOutputColumns, const unsigned int firstOutputRow, const unsigned int numberOutputRows);
};

template <unsigned int tChannels, PixelCenter tPixelCenter, typename TScalar>
inline void AdvancedFrameInterpolatorBilinear::interpolatePatch8BitPerChannel(const uint8_t* const frame, const unsigned int width, const unsigned int framePaddingElements, uint8_t* const buffer, const VectorT2<TScalar>& position, const unsigned int patchWidth, const unsigned int patchHeight)
{
	AdvancedFrameInterpolatorBilinearBase::interpolatePatch8BitPerChannel<tChannels, tPixelCenter, TScalar>(frame, width, framePaddingElements, buffer, position, patchWidth, patchHeight);
}

template <unsigned int tChannels, unsigned int tPatchSize, PixelCenter tPixelCenter>
inline void AdvancedFrameInterpolatorBilinear::interpolateSquarePatch8BitPerChannel(const uint8_t* const frame, const unsigned int width, const unsigned int framePaddingElements, uint8_t* const buffer, const Vector2& position)
{
#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

	if constexpr (tPatchSize >= 5u)
	{
		AdvancedFrameInterpolatorBilinearSSE::interpolateSquarePatch8BitPerChannel<tChannels, tPatchSize, tPixelCenter>(frame, width, framePaddingElements, buffer, position);
		return;
	}

#endif

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	if constexpr (tPatchSize >= 5u)
	{
		AdvancedFrameInterpolatorBilinearNEON::interpolateSquarePatch8BitPerChannel<tChannels, tPatchSize, tPixelCenter>(frame, width, framePaddingElements, buffer, position);
		return;
	}

#endif

	AdvancedFrameInterpolatorBilinearBase::interpolateSquarePatch8BitPerChannelTemplate<tChannels, tPatchSize, tPixelCenter>(frame, width, framePaddingElements, buffer, position);
}

template <unsigned int tChannels, PixelCenter tPixelCenter, typename TScalar>
void AdvancedFrameInterpolatorBilinear::interpolatePatchWithMask8BitPerChannel(const uint8_t* frame, const uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const VectorT2<TScalar>& position, uint8_t* patch, uint8_t* patchMask, const unsigned int patchWidth, const unsigned int patchHeight, const unsigned int patchPaddingElements, const unsigned int patchMaskPaddingElements, const uint8_t maskValue)
{
	ocean_assert(frame != nullptr && mask != nullptr);
	ocean_assert(patch != nullptr && patchMask != nullptr);

	ocean_assert(patchWidth >= 1u && patchHeight >= 1u);

	const unsigned int patchMaskStrideElements = patchWidth + patchMaskPaddingElements;

	const TScalar left = position.x() - TScalar(patchWidth - 1u) * TScalar(0.5);
	const TScalar top = position.y() - TScalar(patchHeight - 1u) * TScalar(0.5);

	for (unsigned int y = 0u; y < patchHeight; ++y)
	{
		for (unsigned int x = 0u; x < patchWidth; ++x)
		{
			interpolatePixelWithMask8BitPerChannel<tChannels, tPixelCenter, TScalar>(frame, mask, width, height, framePaddingElements, maskPaddingElements, VectorT2<TScalar>(left + TScalar(x), top + TScalar(y)), patch, patchMask[x], maskValue);

			patch += tChannels;
		}

		patch += patchPaddingElements;
		patchMask += patchMaskStrideElements;
	}
}

template <unsigned int tChannels>
void AdvancedFrameInterpolatorBilinear::interpolateSquareMirroredBorder8BitPerChannel(const uint8_t* const frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, uint8_t* const buffer, const Vector2& position, const unsigned int patchSize)
{
	AdvancedFrameInterpolatorBilinearBase::interpolateSquareMirroredBorder8BitPerChannel<tChannels>(frame, width, height, framePaddingElements, buffer, position, patchSize);
}

template <unsigned int tChannels, unsigned int tPatchSize>
void AdvancedFrameInterpolatorBilinear::interpolateSquareMirroredBorder8BitPerChannel(const uint8_t* const frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, uint8_t* const buffer, const Vector2& position)
{
	AdvancedFrameInterpolatorBilinearBase::interpolateSquareMirroredBorderTemplate8BitPerChannel<tChannels, tPatchSize>(frame, width, height, framePaddingElements, buffer, position);
}

template <typename TScalar>
bool AdvancedFrameInterpolatorBilinear::Comfort::interpolatePixelWithMask8BitPerChannel(const uint8_t* frame, const uint8_t* mask, const unsigned int channels, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const PixelCenter pixelCenter, const VectorT2<TScalar>& position, uint8_t* result, uint8_t& resultMask, const uint8_t maskValue)
{
	ocean_assert(frame != nullptr);
	ocean_assert(mask != nullptr);
	ocean_assert(channels >= 1u && channels <= 8u);

	if (pixelCenter == PC_TOP_LEFT)
	{
		switch (channels)
		{
			case 1u:
				AdvancedFrameInterpolatorBilinear::interpolatePixelWithMask8BitPerChannel<1u, PC_TOP_LEFT, TScalar>(frame, mask, width, height, framePaddingElements, maskPaddingElements, position, result, resultMask, maskValue);
				return true;

			case 2u:
				AdvancedFrameInterpolatorBilinear::interpolatePixelWithMask8BitPerChannel<2u, PC_TOP_LEFT, TScalar>(frame, mask, width, height, framePaddingElements, maskPaddingElements, position, result, resultMask, maskValue);
				return true;

			case 3u:
				AdvancedFrameInterpolatorBilinear::interpolatePixelWithMask8BitPerChannel<3u, PC_TOP_LEFT, TScalar>(frame, mask, width, height, framePaddingElements, maskPaddingElements, position, result, resultMask, maskValue);
				return true;

			case 4u:
				AdvancedFrameInterpolatorBilinear::interpolatePixelWithMask8BitPerChannel<4u, PC_TOP_LEFT, TScalar>(frame, mask, width, height, framePaddingElements, maskPaddingElements, position, result, resultMask, maskValue);
				return true;

			case 5u:
				AdvancedFrameInterpolatorBilinear::interpolatePixelWithMask8BitPerChannel<5u, PC_TOP_LEFT, TScalar>(frame, mask, width, height, framePaddingElements, maskPaddingElements, position, result, resultMask, maskValue);
				return true;

			case 6u:
				AdvancedFrameInterpolatorBilinear::interpolatePixelWithMask8BitPerChannel<6u, PC_TOP_LEFT, TScalar>(frame, mask, width, height, framePaddingElements, maskPaddingElements, position, result, resultMask, maskValue);
				return true;

			case 7u:
				AdvancedFrameInterpolatorBilinear::interpolatePixelWithMask8BitPerChannel<7u, PC_TOP_LEFT, TScalar>(frame, mask, width, height, framePaddingElements, maskPaddingElements, position, result, resultMask, maskValue);
				return true;

			case 8u:
				AdvancedFrameInterpolatorBilinear::interpolatePixelWithMask8BitPerChannel<8u, PC_TOP_LEFT, TScalar>(frame, mask, width, height, framePaddingElements, maskPaddingElements, position, result, resultMask, maskValue);
				return true;
		}
	}
	else
	{
		ocean_assert(pixelCenter == PC_CENTER);

		switch (channels)
		{
			case 1u:
				AdvancedFrameInterpolatorBilinear::interpolatePixelWithMask8BitPerChannel<1u, PC_CENTER, TScalar>(frame, mask, width, height, framePaddingElements, maskPaddingElements, position, result, resultMask, maskValue);
				return true;

			case 2u:
				AdvancedFrameInterpolatorBilinear::interpolatePixelWithMask8BitPerChannel<2u, PC_CENTER, TScalar>(frame, mask, width, height, framePaddingElements, maskPaddingElements, position, result, resultMask, maskValue);
				return true;

			case 3u:
				AdvancedFrameInterpolatorBilinear::interpolatePixelWithMask8BitPerChannel<3u, PC_CENTER, TScalar>(frame, mask, width, height, framePaddingElements, maskPaddingElements, position, result, resultMask, maskValue);
				return true;

			case 4u:
				AdvancedFrameInterpolatorBilinear::interpolatePixelWithMask8BitPerChannel<4u, PC_CENTER, TScalar>(frame, mask, width, height, framePaddingElements, maskPaddingElements, position, result, resultMask, maskValue);
				return true;

			case 5u:
				AdvancedFrameInterpolatorBilinear::interpolatePixelWithMask8BitPerChannel<5u, PC_CENTER, TScalar>(frame, mask, width, height, framePaddingElements, maskPaddingElements, position, result, resultMask, maskValue);
				return true;

			case 6u:
				AdvancedFrameInterpolatorBilinear::interpolatePixelWithMask8BitPerChannel<6u, PC_CENTER, TScalar>(frame, mask, width, height, framePaddingElements, maskPaddingElements, position, result, resultMask, maskValue);
				return true;

			case 7u:
				AdvancedFrameInterpolatorBilinear::interpolatePixelWithMask8BitPerChannel<7u, PC_CENTER, TScalar>(frame, mask, width, height, framePaddingElements, maskPaddingElements, position, result, resultMask, maskValue);
				return true;

			case 8u:
				AdvancedFrameInterpolatorBilinear::interpolatePixelWithMask8BitPerChannel<8u, PC_CENTER, TScalar>(frame, mask, width, height, framePaddingElements, maskPaddingElements, position, result, resultMask, maskValue);
				return true;
		}
	}

	ocean_assert(false && "Invalid channel number");
	return false;
}

template <unsigned int tChannels>
void AdvancedFrameInterpolatorBilinear::interpolateTriangle8BitPerChannel(const uint8_t* source, uint8_t* target, const PixelTriangle& sourceTriangle, const PixelTriangle& targetTriangle, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source && target);
	ocean_assert(sourceWidth > 0u && sourceHeight > 0u);
	ocean_assert(targetWidth > 0u && targetHeight > 0u);

	ocean_assert(sourceTriangle.isValid() && targetTriangle.isValid());

	const unsigned int targetMinY = targetTriangle.top();
	const unsigned int targetMaxY = min(targetTriangle.bottom(), targetHeight - 1u);

	const PixelLine targetLine01(targetTriangle.point0(), targetTriangle.point1());
	const PixelLine targetLine02(targetTriangle.point0(), targetTriangle.point2());
	const PixelLine targetLine12(targetTriangle.point1(), targetTriangle.point2());

	const Triangle2 tTriangle(Vector2(Scalar(targetTriangle.point0().x()), Scalar(targetTriangle.point0().y())),
									Vector2(Scalar(targetTriangle.point1().x()), Scalar(targetTriangle.point1().y())),
									Vector2(Scalar(targetTriangle.point2().x()), Scalar(targetTriangle.point2().y())));

	const Triangle2 sTriangle(Vector2(Scalar(sourceTriangle.point0().x()), Scalar(sourceTriangle.point0().y())),
									Vector2(Scalar(sourceTriangle.point1().x()), Scalar(sourceTriangle.point1().y())),
									Vector2(Scalar(sourceTriangle.point2().x()), Scalar(sourceTriangle.point2().y())));

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&interpolateTriangle8BitPerChannelSubset<tChannels>, source, target, sourceWidth, sourceHeight, targetWidth, targetHeight, sourcePaddingElements, targetPaddingElements, &tTriangle, &sTriangle, &targetLine01, &targetLine02, &targetLine12, 0u, 0u), targetMinY, targetMaxY - targetMinY + 1u, 13u, 14u, 20u);
	}
	else
	{
		interpolateTriangle8BitPerChannelSubset<tChannels>(source, target, sourceWidth, sourceHeight, targetWidth, targetHeight, sourcePaddingElements, targetPaddingElements, &tTriangle, &sTriangle, &targetLine01, &targetLine02, &targetLine12, targetMinY, targetMaxY - targetMinY + 1u);
	}
}

template <unsigned int tChannels>
void AdvancedFrameInterpolatorBilinear::interpolateTriangle8BitPerChannel(const uint8_t* source, uint8_t* target, const Triangle2& sourceTriangle, const Triangle2& targetTriangle, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source && target);
	ocean_assert(sourceWidth > 0u && sourceHeight > 0u);
	ocean_assert(targetWidth > 0u && targetHeight > 0u);

	ocean_assert(sourceTriangle.isValid() && targetTriangle.isValid());

	const PixelTriangle targetPixelTriangle(targetTriangle, targetWidth, targetHeight);

	const unsigned int targetMinY = min(targetPixelTriangle.point0().y(), min(targetPixelTriangle.point1().y(), targetPixelTriangle.point2().y()));
	const unsigned int targetMaxY = max(targetPixelTriangle.point0().y(), max(targetPixelTriangle.point1().y(), targetPixelTriangle.point2().y()));

	const PixelLine targetLine01(targetPixelTriangle.point0(), targetPixelTriangle.point1());
	const PixelLine targetLine02(targetPixelTriangle.point0(), targetPixelTriangle.point2());
	const PixelLine targetLine12(targetPixelTriangle.point1(), targetPixelTriangle.point2());

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&interpolateTriangle8BitPerChannelSubset<tChannels>, source, target, sourceWidth, sourceHeight, targetWidth, targetHeight, sourcePaddingElements, targetPaddingElements, &targetTriangle, &sourceTriangle, &targetLine01, &targetLine02, &targetLine12, 0u, 0u), targetMinY, targetMaxY - targetMinY + 1u, 13u, 14u, 20u);
	}
	else
	{
		interpolateTriangle8BitPerChannelSubset<tChannels>(source, target, sourceWidth, sourceHeight, targetWidth, targetHeight, sourcePaddingElements, targetPaddingElements, &targetTriangle, &sourceTriangle, &targetLine01, &targetLine02, &targetLine12, targetMinY, targetMaxY - targetMinY + 1u);
	}
}

template <unsigned int tChannels>
void AdvancedFrameInterpolatorBilinear::interpolateTriangle8BitPerChannelSubset(const uint8_t* source, uint8_t* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const Triangle2* targetTriangle, const Triangle2* sourceTriangle, const PixelLine* targetLine01, const PixelLine* targetLine02, const PixelLine* targetLine12, const unsigned int firstTargetRow, const unsigned int numberTargetRows)
{
	static_assert(tChannels != 0u, "Invalid data channels!");

	ocean_assert(source != nullptr && target != nullptr);

#ifdef OCEAN_DEBUG
	const unsigned int targetMinY = (unsigned int)(Numeric::round32(targetTriangle->top()));
	const unsigned int targetMaxY = min((unsigned int)(Numeric::round32(targetTriangle->bottom())), targetHeight - 1u);

	ocean_assert(firstTargetRow >= targetMinY);
	ocean_assert(firstTargetRow + numberTargetRows <= targetMaxY + 1u);
#endif

	ocean_assert_and_suppress_unused(firstTargetRow + numberTargetRows <= targetHeight, targetHeight);

	const unsigned int targetStrideElements = targetWidth * tChannels + targetPaddingElements;

	unsigned int x01, x02, x12;

	for (unsigned int y = firstTargetRow; y < firstTargetRow + numberTargetRows; ++y)
	{
		unsigned int xMin = 0xFFFFFFFF;
		unsigned int xMax = 0;

		if (targetLine01->horizontalIntersection(y, x01))
		{
			xMin = min(xMin, x01);
			xMax = max(xMax, x01);

			if (targetLine01->isHorizontal())
			{
				xMin = min(xMin, min(targetLine01->p0().x(), targetLine01->p1().x()));
				xMax = max(xMax, max(targetLine01->p0().x(), targetLine01->p1().x()));
			}
		}

		if (targetLine02->horizontalIntersection(y, x02))
		{
			xMin = min(xMin, x02);
			xMax = max(xMax, x02);

			if (targetLine02->isHorizontal())
			{
				xMin = min(xMin, min(targetLine02->p0().x(), targetLine02->p1().x()));
				xMax = max(xMax, max(targetLine02->p0().x(), targetLine02->p1().x()));
			}
		}

		if (targetLine12->horizontalIntersection(y, x12))
		{
			xMin = min(xMin, x12);
			xMax = max(xMax, x12);

			if (targetLine12->isHorizontal())
			{
				xMin = min(xMin, min(targetLine12->p0().x(), targetLine12->p1().x()));
				xMax = max(xMax, max(targetLine12->p0().x(), targetLine12->p1().x()));
			}
		}

		uint8_t* const targetRow = target + y * targetStrideElements;

		for (unsigned int x = xMin; x <= min(xMax, targetWidth - 1u); ++x)
		{
			const Vector3 targetPositionBarycentric(targetTriangle->cartesian2barycentric(Vector2(Scalar(x), Scalar(y))));
			ocean_assert(Triangle2::isValidBarycentric(targetPositionBarycentric));

			const Vector2 sourcePosition(sourceTriangle->barycentric2cartesian(targetPositionBarycentric));
			const Vector2 clippedPosition(minmax(Scalar(0), sourcePosition.x(), Scalar(sourceWidth - 1u)), minmax(Scalar(0), sourcePosition.y(), Scalar(sourceHeight - 1u)));

			CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<tChannels, CV::PC_TOP_LEFT>(source, sourceWidth, sourceHeight, sourcePaddingElements, clippedPosition, targetRow + tChannels * x);
		}
	}
}

template <unsigned int tChannels, PixelCenter tPixelCenter, typename TScalar>
inline void AdvancedFrameInterpolatorBilinear::interpolatePixelWithMask8BitPerChannel(const uint8_t* frame, const uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const VectorT2<TScalar>& position, uint8_t* result, uint8_t& resultMask, const uint8_t maskValue)
{
	static_assert(tChannels != 0u, "Invalid channel number!");
	static_assert(tPixelCenter == PC_TOP_LEFT || tPixelCenter == PC_CENTER, "Invalid pixel center!");

	ocean_assert(frame != nullptr && mask != nullptr);
	ocean_assert(result != nullptr);
	ocean_assert(width != 0u && height != 0u);

	const unsigned int frameStrideElements = width * tChannels + framePaddingElements;
	const unsigned int maskStrideElements = width + maskPaddingElements;

	const VectorT2<TScalar> shiftedPosition = tPixelCenter == PC_TOP_LEFT ? position : position - VectorT2<TScalar>(TScalar(0.5), TScalar(0.5));

	const int left = int(NumericT<TScalar>::floor(shiftedPosition.x()));
	const int top = int(NumericT<TScalar>::floor(shiftedPosition.y()));

	// check whether the position lies outside the frame
	if (left < -1 || top < -1 || left >= int(width) || top >= int(height))
	{
		resultMask = 0xFFu - maskValue;
		return;
	}

	const TScalar sFactorRight = shiftedPosition.x() - TScalar(left);
	ocean_assert(sFactorRight >= 0 && sFactorRight <= 1);
	const unsigned int factorRight = (unsigned int)(sFactorRight * TScalar(128) + TScalar(0.5));
	const unsigned int factorLeft = 128u - factorRight;

	const TScalar sFactorBottom = shiftedPosition.y() - TScalar(top);
	ocean_assert(sFactorBottom >= 0 && sFactorBottom <= 1);
	const unsigned int factorBottom = (unsigned int)(sFactorBottom * TScalar(128) + TScalar(0.5));
	const unsigned int factorTop = 128u - factorBottom;

	const unsigned int factorTopLeft = factorTop * factorLeft;
	const unsigned int factorTopRight = factorTop * factorRight;
	const unsigned int factorBottomLeft = factorBottom * factorLeft;
	const unsigned int factorBottomRight = factorBottom * factorRight;

	const uint8_t* topLeft = frame + top * int(frameStrideElements) + int(tChannels) * left;
	const uint8_t* maskTopLeft = mask + top * int(maskStrideElements) + left;

	const uint32_t stateTopLeft = (left >= 0 && top >= 0 && left < int(width) && top < int(height) && maskTopLeft[0] == maskValue) ? 1u : 0u;
	const uint32_t stateTopRight = (left >= -1 && left < int(width - 1u) && top >= 0 && top < int(height) && maskTopLeft[1] == maskValue) ? 1u : 0u;
	const uint32_t stateBottomLeft = (left >= 0 && top >= -1 && left < int(width) && top < int(height - 1u) && maskTopLeft[maskStrideElements] == maskValue) ? 1u : 0u;
	const uint32_t stateBottomRight = (left >= -1 && left < int(width - 1u) && top >= -1 && top < int(height - 1u) && maskTopLeft[maskStrideElements + 1u] == maskValue) ? 1u : 0u;

	const uint32_t state = stateTopLeft | (stateTopRight << 8u) | (stateBottomLeft << 16u) | (stateBottomRight << 24u);

	typedef typename DataType<uint8_t, tChannels>::Type PixelType;

	switch (state)
	{
		// FF FF
		// FF FF
		case 0x01010101u:
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				result[n] = uint8_t((topLeft[n] * factorTopLeft + topLeft[tChannels + n] * factorTopRight
										+ topLeft[frameStrideElements + n] * factorBottomLeft + topLeft[frameStrideElements + tChannels + n] * factorBottomRight + 8192u) >> 14u);
			}

			resultMask = maskValue;
			return;
		}

		// 00 FF
		// FF FF
		case 0x01010100u:
		{
			const unsigned int factorSum = factorTopRight + factorBottomLeft + factorBottomRight;

			if (factorSum == 0u)
			{
				resultMask = 0xFFu - maskValue;
				return;
			}

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				result[n] = uint8_t((topLeft[tChannels + n] * factorTopRight
										+ topLeft[frameStrideElements + n] * factorBottomLeft + topLeft[frameStrideElements + tChannels + n] * factorBottomRight + factorSum / 2u) / factorSum);
			}

			resultMask = maskValue;
			return;
		}

		// FF 00
		// FF FF
		case 0x01010001u:
		{
			const unsigned int factorSum = factorTopLeft + factorBottomLeft + factorBottomRight;

			if (factorSum == 0u)
			{
				resultMask = 0xFFu - maskValue;
				return;
			}

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				result[n] = uint8_t((topLeft[n] * factorTopLeft
										+ topLeft[frameStrideElements + n] * factorBottomLeft + topLeft[frameStrideElements + tChannels + n] * factorBottomRight + factorSum / 2u) / factorSum);
			}

			resultMask = maskValue;
			return;
		}

		// FF FF
		// 00 FF
		case 0x01000101u:
		{
			const unsigned int factorSum = factorTopLeft + factorTopRight + factorBottomRight;

			if (factorSum == 0u)
			{
				resultMask = 0xFFu - maskValue;
				return;
			}

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				result[n] = uint8_t((topLeft[n] * factorTopLeft + topLeft[tChannels + n] * factorTopRight
										+ topLeft[frameStrideElements + tChannels + n] * factorBottomRight + factorSum / 2u) / factorSum);
			}

			resultMask = maskValue;
			return;
		}

		// FF FF
		// FF 00
		case 0x00010101u:
		{
			const unsigned int factorSum = factorTopLeft + factorTopRight + factorBottomLeft;

			if (factorSum == 0u)
			{
				resultMask = 0xFFu - maskValue;
				return;
			}

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				result[n] = uint8_t((topLeft[n] * factorTopLeft + topLeft[tChannels + n] * factorTopRight
										+ topLeft[frameStrideElements + n] * factorBottomLeft + factorSum / 2u) / factorSum);
			}

			resultMask = maskValue;
			return;
		}

		// 00 00
		// FF FF
		case 0x01010000u:
		{
			const unsigned int factorSum = factorBottomLeft + factorBottomRight;

			if (factorSum == 0u)
			{
				resultMask = 0xFFu - maskValue;
				return;
			}

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				result[n] = uint8_t((topLeft[frameStrideElements + n] * factorBottomLeft + topLeft[frameStrideElements + tChannels + n] * factorBottomRight + factorSum / 2u) / factorSum);
			}

			resultMask = maskValue;
			return;
		}

		// FF 00
		// FF 00
		case 0x00010001u:
		{
			const unsigned int factorSum = factorTopLeft + factorBottomLeft;

			if (factorSum == 0u)
			{
				resultMask = 0xFFu - maskValue;
				return;
			}

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				result[n] = uint8_t((topLeft[n] * factorTopLeft + topLeft[frameStrideElements + n] * factorBottomLeft + factorSum / 2u) / factorSum);
			}

			resultMask = maskValue;
			return;
		}

		// FF FF
		// 00 00
		case 0x00000101u:
		{
			const unsigned int factorSum = factorTopLeft + factorTopRight;

			if (factorSum == 0u)
			{
				resultMask = 0xFF - maskValue;
				return;
			}

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				result[n] = uint8_t((topLeft[n] * factorTopLeft + topLeft[tChannels + n] * factorTopRight + factorSum / 2u) / factorSum);
			}

			resultMask = maskValue;
			return;
		}

		// 00 FF
		// 00 FF
		case 0x01000100u:
		{
			const unsigned int factorSum = factorTopRight + factorBottomRight;

			if (factorSum == 0u)
			{
				resultMask = 0xFFu - maskValue;
				return;
			}

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				result[n] = uint8_t((topLeft[tChannels + n] * factorTopRight
										+ topLeft[frameStrideElements + tChannels + n] * factorBottomRight + factorSum / 2u) / factorSum);
			}

			resultMask = maskValue;
			return;
		}

		// 00 FF
		// FF 00
		case 0x00010100u:
		{
			const unsigned int factorSum = factorTopRight + factorBottomLeft;

			if (factorSum == 0u)
			{
				resultMask = 0xFFu - maskValue;
				return;
			}

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				result[n] = uint8_t((topLeft[tChannels + n] * factorTopRight
										+ topLeft[frameStrideElements + n] * factorBottomLeft + factorSum / 2u) / factorSum);
			}

			resultMask = maskValue;
			return;
		}

		// FF 00
		// 00 FF
		case 0x01000001u:
		{
			const unsigned int factorSum = factorTopLeft + factorBottomRight;

			if (factorSum == 0u)
			{
				resultMask = 0xFFu - maskValue;
				return;
			}

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				result[n] = uint8_t((topLeft[n] * factorTopLeft + topLeft[frameStrideElements + tChannels + n] * factorBottomRight + factorSum / 2u) / factorSum);
			}

			resultMask = maskValue;
			return;
		}

		// 00 00
		// 00 FF
		case 0x01000000u:
		{
			if (factorBottomRight == 0u)
			{
				resultMask = 0xFFu - maskValue;
				return;
			}

			*((PixelType*)(result)) = *((const PixelType*)(topLeft + frameStrideElements) + 1);
			resultMask = maskValue;

			return;
		}

		// FF 00
		// 00 00
		case 0x00000001u:
		{
			if (factorTopLeft== 0u)
			{
				resultMask = 0xFFu - maskValue;
				return;
			}

			*((PixelType*)(result)) = *((const PixelType*)(topLeft));
			resultMask = maskValue;

			return;
		}

		// 00 FF
		// 00 00
		case 0x00000100u:
		{
			if (factorTopRight == 0u)
			{
				resultMask = 0xFFu - maskValue;
				return;
			}

			*((PixelType*)(result)) = *((const PixelType*)(topLeft) + 1);
			resultMask = maskValue;

			return;
		}

		// 00 00
		// FF 00
		case 0x00010000u:
		{
			if (factorBottomLeft == 0u)
			{
				resultMask = 0xFFu - maskValue;
				return;
			}

			*((PixelType*)(result)) = *((const PixelType*)(topLeft + frameStrideElements));
			resultMask = maskValue;

			return;
		}

		// 00 00
		// 00 00
		case 0x00000000u:
		{
			resultMask = 0xFFu - maskValue;
			return;
		}
	}

	ocean_assert(false && "Invalid state!");
	resultMask = 0xFFu - maskValue;
}

template <unsigned int tChannels>
void AdvancedFrameInterpolatorBilinear::homographyFilterMask8BitPerChannel(const uint8_t* input, const uint8_t* outputFilterMask, uint8_t* output, const unsigned int inputWidth, const unsigned int inputHeight, const unsigned int outputWidth, const unsigned int outputHeight, const unsigned int inputPaddingElements, const unsigned int outputFilterMaskPaddingElements, const unsigned int outputPaddingElements, const SquareMatrix3& input_H_output, const PixelBoundingBox& outputBoundingBox, Worker* worker)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	const unsigned int outputFirstColumn = outputBoundingBox ? outputBoundingBox.left() : 0u;
	const unsigned int outputNumberColumns = outputBoundingBox ? outputBoundingBox.width() : outputWidth;

	const unsigned int outputFirstRow = outputBoundingBox ? outputBoundingBox.top() : 0u;
	const unsigned int outputNumberRows = outputBoundingBox ? outputBoundingBox.height() : outputHeight;

	const unsigned int outputFilterMaskStrideElemnets = outputWidth + outputFilterMaskPaddingElements;
	const unsigned int outputStrideElements = outputWidth * tChannels + outputPaddingElements;

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&homographyFilterMask8BitPerChannelSubset<tChannels>, input, outputFilterMask, output, inputWidth, inputHeight, inputPaddingElements, outputFilterMaskStrideElemnets, outputStrideElements, &input_H_output, outputFirstColumn, outputNumberColumns, 0u, 0u), outputFirstRow, outputNumberRows, 11u, 12u, 40u);
	}
	else
	{
		homographyFilterMask8BitPerChannelSubset<tChannels>(input, outputFilterMask, output, inputWidth, inputHeight, inputPaddingElements, outputFilterMaskStrideElemnets, outputStrideElements, &input_H_output, outputFirstColumn, outputNumberColumns, outputFirstRow, outputNumberRows);
	}
}

template <unsigned int tChannels>
void AdvancedFrameInterpolatorBilinear::homographyFilterMask8BitPerChannelSubset(const uint8_t* input, const uint8_t* outputFilterMask, uint8_t* output, const unsigned int inputWidth, const unsigned int inputHeight, const unsigned int inputPaddingElements, const unsigned int outputFilterMaskStrideElements, const unsigned int outputStrideElements, const SquareMatrix3* input_H_output, const unsigned int firstOutputColumn, const unsigned int numberOutputColumns, const unsigned int firstOutputRow, const unsigned int numberOutputRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(input != nullptr && outputFilterMask != nullptr && output != nullptr);
	ocean_assert(inputWidth >= 1u && inputHeight >= 1u);
	ocean_assert(outputFilterMaskStrideElements >= 1u && outputStrideElements >= tChannels);

	outputFilterMask += firstOutputRow * outputFilterMaskStrideElements;
	output += firstOutputRow * outputStrideElements;

	const Scalar inputWidth1 = Scalar(inputWidth - 1u);
	const Scalar inputHeight1 = Scalar(inputHeight - 1u);

	for (unsigned int y = firstOutputRow; y < firstOutputRow + numberOutputRows; ++y)
	{
		for (unsigned int x = firstOutputColumn; x < firstOutputColumn + numberOutputColumns; ++x)
		{
			if (outputFilterMask[x] != 0xFFu)
			{
				const Vector2 sourcePosition(*input_H_output * Vector2(Scalar(x), Scalar(y)));
				const Vector2 clampedPosition(minmax(Scalar(0), sourcePosition.x(), inputWidth1), minmax(Scalar(0), sourcePosition.y(), inputHeight1));

				CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<tChannels, CV::PC_TOP_LEFT>(input, inputWidth, inputHeight, inputPaddingElements, clampedPosition, output + x * tChannels);
			}
		}

		outputFilterMask += outputFilterMaskStrideElements;
		output += outputStrideElements;
	}
}

}

}

}

#endif // META_OCEAN_CV_ADVANCED_ADVANCED_FRAME_INTERPOLATOR_BILINEAR_H
