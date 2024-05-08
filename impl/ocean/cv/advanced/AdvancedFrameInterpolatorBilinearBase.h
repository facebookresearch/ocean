/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_ADVANCED_ADVANCED_FRAME_INTERPOLATOR_BILINEAR_BASE_H
#define META_OCEAN_CV_ADVANCED_ADVANCED_FRAME_INTERPOLATOR_BILINEAR_BASE_H

#include "ocean/cv/advanced/Advanced.h"
#include "ocean/cv/SSE.h"

#include "ocean/cv/SumSquareDifferences.h"
#include "ocean/cv/CVUtilities.h"

#include "ocean/math/Vector2.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

/**
 * This class implements several advanced bilinear frame interpolator functions based e.g., on template parameters.
 * @ingroup cvadvanced
 */
class AdvancedFrameInterpolatorBilinearBase
{
	public:

		/**
		 * Interpolates the content of a square image patch with sub-pixel accuracy inside a given image and stores the interpolated data into a buffer.
		 * The center of a pixel is expected to be located at the top-left corner of a pixel.
		 * @param image The image in which the interpolated patch is located, must be valid
		 * @param width The width of the image, in pixel, with range [tPatchSize + 1, infinity)
		 * @param imagePaddingElements The number of padding elements at the end of each image row, in elements, with range [0, infinity)
		 * @param buffer The target buffer with `tChannels * tSize * tSize` elements, must be valid
		 * @param position The center position of the square patch in the image, with range [tPatchSize/2, width - tPatchSize/2 - 1)x[tPatchSize/2, height - tPatchSize/2 - 1) for PC_TOP_LEFT,  [0.5 + tPatchSize/2, width - tPatchSize/2 - 0.5)x[0.5 + tPatchSize/2, height - tPatchSize/2 - 0.5) for PC_CENTER
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [1, infinity), must be odd
		 * @tparam TScalar The scalar data type of the sub-pixel position
		 */
		template <unsigned int tChannels, unsigned int tPatchSize, PixelCenter tPixelCenter = PC_TOP_LEFT, typename TScalar = Scalar>
		static inline void interpolateSquarePatch8BitPerChannelTemplate(const uint8_t* const image, const unsigned int width, const unsigned int imagePaddingElements, uint8_t* buffer, const VectorT2<TScalar>& position);

		/**
		 * Interpolates the content of an image patch with sub-pixel accuracy inside a given image and stores the interpolated data into a buffer.
		 * The center of a pixel is expected to be located at the top-left corner of a pixel.
		 * @param image The image in which the interpolated patch is located, must be valid
		 * @param width The width of the image, in pixel, with range [patchWidth + 1, infinity)
		 * @param imagePaddingElements The number of padding elements at the end of each image row, in elements, with range [0, infinity)
		 * @param buffer The target buffer with `tChannels * tSize * tSize` elements, must be valid
		 * @param position The center position of the patch in the image, with range [patchWidth/2, width - patchWidth/2 - 1)x[patchHeight/2, height - patchHeight/2 - 1) for PC_TOP_LEFT,  [0.5 + patchWidth/2, width - patchWidth/2 - 0.5)x[0.5 + patchHeight/2, height - patchHeight/2 - 0.5) for PC_CENTER
		 * @param patchWidth The width of the image patch in pixel, with range [1, infinity), must be odd
		 * @param patchHeight The height of the image patch in pixel, with range [1, infinity), must be odd
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tPixelCenter The pixel center to be used during interpolation, either 'PC_TOP_LEFT' or 'PC_CENTER'
		 * @tparam TScalar The scalar data type of the sub-pixel position
		 */
		template <unsigned int tChannels, PixelCenter tPixelCenter = PC_TOP_LEFT, typename TScalar = Scalar>
		static void interpolatePatch8BitPerChannel(const uint8_t* const image, const unsigned int width, const unsigned int imagePaddingElements, uint8_t* buffer, const VectorT2<TScalar>& position, const unsigned int patchWidth, const unsigned int patchHeight);

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
		static void interpolateSquareMirroredBorder8BitPerChannel(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, uint8_t* buffer, const Vector2& position, const unsigned int patchSize);

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
		static void interpolateSquareMirroredBorderTemplate8BitPerChannel(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, uint8_t* buffer, const Vector2& position);

		/**
		 * Interpolates the content of 2x2 pixels to one pixel.
		 * @param sourceTopLeft Top left position of the 2x2 pixels to interpolate, must be valid
		 * @param sourceBottomLeft Bottom left position of the 2x2 pixels to interpolate, must be valid
		 * @param target Position of the target pixel
		 * @param xFactor Horizontal interpolation factor with range [0, 128]
		 * @param yFactor Vertical interpolation factor with range [0, 128]
		 * @tparam tChannels The number of data channels of the frame data, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static inline void interpolatePixel8BitPerChannel(const uint8_t* sourceTopLeft, const uint8_t* sourceBottomLeft, uint8_t* target, const unsigned int xFactor, const unsigned int yFactor);

		/**
		 * Interpolates the content of 2x2 pixels to one pixel.
		 * @param sourceTopLeft Top left position of the 2x2 pixels to interpolate, must be valid
		 * @param sourceBottomLeft Bottom left position of the 2x2 pixels to interpolate, must be valid
		 * @param target Position of the target pixel
		 * @param tx_ty_ (128u - xFactor) * (128u - yFactor)
		 * @param txty_ xFactor * (128u - yFactor)
		 * @param tx_ty (128u - xFactor) * yFactor
		 * @param txty xFactor * yFactor
		 * @tparam tChannels The number of data channels of the frame data, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static inline void interpolatePixel8BitPerChannel(const uint8_t* sourceTopLeft, const uint8_t* sourceBottomLeft, uint8_t* target, const unsigned int tx_ty_, const unsigned int txty_, const unsigned int tx_ty, const unsigned int txty);
};

template <unsigned int tChannels, unsigned int tPatchSize, PixelCenter tPixelCenter, typename TScalar>
inline void AdvancedFrameInterpolatorBilinearBase::interpolateSquarePatch8BitPerChannelTemplate(const uint8_t* const image, const unsigned int width, const unsigned int imagePaddingElements, uint8_t* buffer, const VectorT2<TScalar>& position)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize % 2u == 1u, "The patch size must be odd!");

	ocean_assert(image != nullptr && buffer != nullptr);
	ocean_assert(tPatchSize + 1u <= width);

	constexpr unsigned int tPatchSize_2 = tPatchSize / 2u;

	const unsigned int imageStrideElements = width * tChannels + imagePaddingElements;

	const VectorT2<TScalar> shiftedPosition = tPixelCenter == PC_TOP_LEFT ? position : position - VectorT2<TScalar>(TScalar(0.5), TScalar(0.5));

	ocean_assert(shiftedPosition.x() >= TScalar(tPatchSize_2) && shiftedPosition.y() >= TScalar(tPatchSize_2));
	ocean_assert(shiftedPosition.x() < TScalar(width - tPatchSize_2 - 1u));

	const unsigned int left = (unsigned int)(shiftedPosition.x()) - tPatchSize_2;
	const unsigned int top = (unsigned int)(shiftedPosition.y()) - tPatchSize_2;

	ocean_assert(left + tPatchSize < width);

	const TScalar tx = shiftedPosition.x() - TScalar(int(shiftedPosition.x()));
	ocean_assert(tx >= TScalar(0) && tx <= TScalar(1));
	const unsigned int factorRight = (unsigned int)(tx * TScalar(128) + TScalar(0.5));
	const unsigned int factorLeft = 128u - factorRight;

	const TScalar ty = shiftedPosition.y() - TScalar(int(shiftedPosition.y()));
	ocean_assert(ty >= 0 && ty <= 1);
	const unsigned int factorBottom = (unsigned int)(ty * TScalar(128) + TScalar(0.5));
	const unsigned int factorTop = 128u - factorBottom;

	const unsigned int factorTopLeft = factorLeft * factorTop;
	const unsigned int factorTopRight = factorRight * factorTop;
	const unsigned int factorBottomLeft = factorLeft * factorBottom;
	const unsigned int factorBottomRight = factorRight * factorBottom;

	const uint8_t* imageTop = image + top * imageStrideElements + left * tChannels;
	const uint8_t* imageBottom = imageTop + imageStrideElements;

	const uint8_t* const imageEnd = imageTop + imageStrideElements * tPatchSize;

	const unsigned int imageRowOffset = imageStrideElements - tChannels * tPatchSize;

	while (imageTop != imageEnd)
	{
		ocean_assert(imageTop < imageEnd);
		ocean_assert((imageTop - image) % imageStrideElements == tChannels * left);

		const uint8_t* const imageEndRow = imageTop + tChannels * tPatchSize;

		while (imageTop != imageEndRow)
		{
			ocean_assert(imageTop < imageEnd);
			ocean_assert(imageTop < imageEndRow);

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				*buffer++ = uint8_t((imageTop[n] * factorTopLeft + imageTop[n + tChannels] * factorTopRight + imageBottom[n] * factorBottomLeft + imageBottom[n + tChannels] * factorBottomRight + 8192u) / 16384u);
			}

			imageTop += tChannels;
			imageBottom += tChannels;
		}

		imageTop += imageRowOffset;
		imageBottom += imageRowOffset;
	}
}

template <unsigned int tChannels, PixelCenter tPixelCenter, typename TScalar>
void AdvancedFrameInterpolatorBilinearBase::interpolatePatch8BitPerChannel(const uint8_t* const image, const unsigned int width, const unsigned int imagePaddingElements, uint8_t* buffer, const VectorT2<TScalar>& position, const unsigned int patchWidth, const unsigned int patchHeight)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(image != nullptr && buffer != nullptr);
	ocean_assert(patchWidth + 1u <= width);

	const unsigned int patchWidth_2 = patchWidth / 2u;
	const unsigned int patchHeight_2 = patchHeight / 2u;

	const unsigned int imageStrideElements = width * tChannels + imagePaddingElements;

	const VectorT2<TScalar> shiftedPosition = tPixelCenter == PC_TOP_LEFT ? position : position - VectorT2<TScalar>(TScalar(0.5), TScalar(0.5));

	ocean_assert(shiftedPosition.x() >= TScalar(patchWidth_2) && shiftedPosition.y() >= TScalar(patchHeight_2));
	ocean_assert(shiftedPosition.x() < TScalar(width - patchWidth_2 - 1u));

	const unsigned int left = (unsigned int)(shiftedPosition.x()) - patchWidth_2;
	const unsigned int top = (unsigned int)(shiftedPosition.y()) - patchHeight_2;

	ocean_assert(left + patchWidth < width);

	const TScalar tx = shiftedPosition.x() - TScalar(int(shiftedPosition.x()));
	ocean_assert(tx >= TScalar(0) && tx <= TScalar(1));
	const unsigned int factorRight = (unsigned int)(tx * TScalar(128) + TScalar(0.5));
	const unsigned int factorLeft = 128u - factorRight;

	const TScalar ty = shiftedPosition.y() - TScalar(int(shiftedPosition.y()));
	ocean_assert(ty >= 0 && ty <= 1);
	const unsigned int factorBottom = (unsigned int)(ty * TScalar(128) + TScalar(0.5));
	const unsigned int factorTop = 128u - factorBottom;

	const unsigned int factorTopLeft = factorLeft * factorTop;
	const unsigned int factorTopRight = factorRight * factorTop;
	const unsigned int factorBottomLeft = factorLeft * factorBottom;
	const unsigned int factorBottomRight = factorRight * factorBottom;

	const uint8_t* imageTop = image + top * imageStrideElements + left * tChannels;
	const uint8_t* imageBottom = imageTop + imageStrideElements;

	const uint8_t* const imageEnd = imageTop + imageStrideElements * patchHeight;

	const unsigned int imageRowOffset = imageStrideElements - tChannels * patchWidth;

	while (imageTop != imageEnd)
	{
		ocean_assert(imageTop < imageEnd);
		ocean_assert((imageTop - image) % imageStrideElements == tChannels * left);

		const uint8_t* const imageEndRow = imageTop + tChannels * patchWidth;

		while (imageTop != imageEndRow)
		{
			ocean_assert(imageTop < imageEnd);
			ocean_assert(imageTop < imageEndRow);

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				*buffer++ = uint8_t((imageTop[n] * factorTopLeft + imageTop[n + tChannels] * factorTopRight + imageBottom[n] * factorBottomLeft + imageBottom[n + tChannels] * factorBottomRight + 8192u) / 16384u);
			}

			imageTop += tChannels;
			imageBottom += tChannels;
		}

		imageTop += imageRowOffset;
		imageBottom += imageRowOffset;
	}
}

template <unsigned int tChannels>
void AdvancedFrameInterpolatorBilinearBase::interpolateSquareMirroredBorder8BitPerChannel(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, uint8_t* buffer, const Vector2& position, const unsigned int patchSize)
{
	ocean_assert(frame != nullptr && buffer != nullptr);
	ocean_assert(patchSize >= 1u && patchSize % 2u == 1u);

	const unsigned int patchSize_2 = patchSize / 2u;

	ocean_assert(position.x() >= Scalar(0) && position.y() >= Scalar(0));
	ocean_assert(position.x() < Scalar(width) && position.y() < Scalar(height));

	const int left = int(position.x()) - int(patchSize_2);
	const int top = int(position.y()) - int(patchSize_2);

	const Scalar tx = position.x() - Scalar(patchSize_2) - Scalar(left);
	ocean_assert(tx >= 0 && tx <= 1);
	const unsigned int txi = (unsigned int)(tx * Scalar(128) + Scalar(0.5));
	const unsigned int txi_ = 128u - txi;

	const Scalar ty = position.y() - Scalar(patchSize_2) - Scalar(top);
	ocean_assert(ty >= 0 && ty <= 1);
	const unsigned int tyi = (unsigned int)(ty * Scalar(128) + Scalar(0.5));
	const unsigned int tyi_ = 128u - tyi;

	const unsigned int tx_y_ = txi_ * tyi_;
	const unsigned int txy_ = txi * tyi_;
	const unsigned int tx_y = txi_ * tyi;
	const unsigned int txy = txi * tyi;

	const int frameStrideElements = int(width * tChannels + framePaddingElements);

	const uint8_t* frameTop = frame + top * frameStrideElements + left * int(tChannels);
	const uint8_t* frameBottom = frameTop + frameStrideElements;

	for (unsigned int y = 0u; y < patchSize; ++y)
	{
		const uint8_t* pixelTop = frameTop + CVUtilities::mirrorOffset(y + (unsigned int)(top), height) * frameStrideElements;
		const uint8_t* pixelBottom = frameBottom + CVUtilities::mirrorOffset(y + (unsigned int)(top) + 1u, height) * frameStrideElements;

		for (int xSigned = left; xSigned < left + int(patchSize); ++xSigned)
		{
			const unsigned int x = (unsigned int)(xSigned);

			if (x < width && (x + 1u) < width)
			{
				// both pixels lies inside the frame

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					buffer[n] = uint8_t((pixelTop[0] * tx_y_ + pixelTop[tChannels] * txy_ + pixelBottom[0] * tx_y + pixelBottom[tChannels] * txy + 8192u) >> 14u);

					++pixelTop;
					++pixelBottom;
				}
			}
			else if (x < width)
			{
				// x0 lies inside the frame

				ocean_assert(x + 1u >= width);

				const int offset = CVUtilities::mirrorOffset(x + 1u, width) * int(tChannels) + int(tChannels);

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					buffer[n] = uint8_t((pixelTop[0] * tx_y_ + *(pixelTop + offset) * txy_ + pixelBottom[0] * tx_y + *(pixelBottom + offset) * txy + 8192u) >> 14u);

					++pixelTop;
					++pixelBottom;
				}
			}
			else if (x + 1u < width)
			{
				// x1 lies inside the frame

				ocean_assert(x >= width);

				const int offset = CVUtilities::mirrorOffset(x, width) * int(tChannels);

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					buffer[n] = uint8_t((*(pixelTop + 0 + offset) * tx_y_ + pixelTop[tChannels] * txy_ + *(pixelBottom + 0 + offset) * tx_y + pixelBottom[tChannels] * txy + 8192u) >> 14u);

					++pixelTop;
					++pixelBottom;
				}
			}
			else
			{
				// neither x0 nor x1 lies inside the frame

				ocean_assert(x >= width && x + 1u >= width);

				const int offsetLeft = CVUtilities::mirrorOffset(x, width) * int(tChannels);
				const int offsetRight = CVUtilities::mirrorOffset(x + 1u, width) * int(tChannels) + int(tChannels);

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					buffer[n] = uint8_t((*(pixelTop + 0 + offsetLeft) * tx_y_ + *(pixelTop + offsetRight) * txy_ + *(pixelBottom + 0 + offsetLeft) * tx_y + *(pixelBottom + offsetRight) * txy + 8192u) >> 14u);

					++pixelTop;
					++pixelBottom;
				}
			}

			buffer += tChannels;
		}

		frameTop += frameStrideElements;
		frameBottom += frameStrideElements;
	}
}

template <unsigned int tChannels, unsigned int tPatchSize>
void AdvancedFrameInterpolatorBilinearBase::interpolateSquareMirroredBorderTemplate8BitPerChannel(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, uint8_t* buffer, const Vector2& position)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tPatchSize >= 1u && tPatchSize % 2u == 1u, "The patch size must be odd!");

	ocean_assert(frame != nullptr && buffer != nullptr);

	constexpr unsigned int tPatchSize_2 = tPatchSize / 2u;

	ocean_assert(position.x() >= 0 && position.y() >= 0);
	ocean_assert(position.x() < Scalar(width) && position.y() < Scalar(height));

	const int left = int(position.x()) - int(tPatchSize_2);
	const int top = int(position.y()) - int(tPatchSize_2);

	const Scalar tx = position.x() - Scalar(tPatchSize_2) - Scalar(left);
	ocean_assert(tx >= 0 && tx <= 1);
	const unsigned int txi = (unsigned int)(tx * Scalar(128) + Scalar(0.5));
	const unsigned int txi_ = 128u - txi;

	const Scalar ty = position.y() - Scalar(tPatchSize_2) - Scalar(top);
	ocean_assert(ty >= 0 && ty <= 1);
	const unsigned int tyi = (unsigned int)(ty * Scalar(128) + Scalar(0.5));
	const unsigned int tyi_ = 128u - tyi;

	const unsigned int tx_y_ = txi_ * tyi_;
	const unsigned int txy_ = txi * tyi_;
	const unsigned int tx_y = txi_ * tyi;
	const unsigned int txy = txi * tyi;

	const int frameStrideElements = int(width * tChannels + framePaddingElements);

	const uint8_t* frameTop = frame + top * frameStrideElements + left * int(tChannels);
	const uint8_t* frameBottom = frameTop + frameStrideElements;

	for (unsigned int y = 0u; y < tPatchSize; ++y)
	{
		const uint8_t* pixelTop = frameTop + CVUtilities::mirrorOffset(y + (unsigned int)(top), height) * frameStrideElements;
		const uint8_t* pixelBottom = frameBottom + CVUtilities::mirrorOffset(y + (unsigned int)(top) + 1u, height) * frameStrideElements;

		for (int xSigned = left; xSigned < left + int(tPatchSize); ++xSigned)
		{
			const unsigned int x = (unsigned int)(xSigned);

			if (x < width && (x + 1u) < width)
			{
				// both pixels lies inside the frame

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					buffer[n] = uint8_t((pixelTop[0] * tx_y_ + pixelTop[tChannels] * txy_ + pixelBottom[0] * tx_y + pixelBottom[tChannels] * txy + 8192u) >> 14u);

					++pixelTop;
					++pixelBottom;
				}
			}
			else if (x < width)
			{
				// x0 lies inside the frame

				ocean_assert(x + 1u >= width);

				const int offset = CVUtilities::mirrorOffset(x + 1u, width) * int(tChannels) + int(tChannels);

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					buffer[n] = uint8_t((pixelTop[0] * tx_y_ + *(pixelTop + offset) * txy_ + pixelBottom[0] * tx_y + *(pixelBottom + offset) * txy + 8192u) >> 14u);

					++pixelTop;
					++pixelBottom;
				}
			}
			else if (x + 1u < width)
			{
				// x1 lies inside the frame

				ocean_assert(x >= width);

				const int offset = CVUtilities::mirrorOffset(x, width) * int(tChannels);

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					buffer[n] = uint8_t((*(pixelTop + 0 + offset) * tx_y_ + pixelTop[tChannels] * txy_ + *(pixelBottom + 0 + offset) * tx_y + pixelBottom[tChannels] * txy + 8192u) >> 14u);

					++pixelTop;
					++pixelBottom;
				}
			}
			else
			{
				// neither x0 nor x1 lies inside the frame

				ocean_assert(x >= width && x + 1u >= width);

				const int offsetLeft = CVUtilities::mirrorOffset(x, width) * int(tChannels);
				const int offsetRight = CVUtilities::mirrorOffset(x + 1u, width) * int(tChannels) + int(tChannels);

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					buffer[n] = uint8_t((*(pixelTop + 0 + offsetLeft) * tx_y_ + *(pixelTop + offsetRight) * txy_ + *(pixelBottom + 0 + offsetLeft) * tx_y + *(pixelBottom + offsetRight) * txy + 8192u) >> 14u);

					++pixelTop;
					++pixelBottom;
				}
			}

			buffer += tChannels;
		}

		frameTop += frameStrideElements;
		frameBottom += frameStrideElements;
	}
}

template <unsigned int tChannels>
inline void AdvancedFrameInterpolatorBilinearBase::interpolatePixel8BitPerChannel(const uint8_t* sourceTopLeft, const uint8_t* sourceBottomLeft, uint8_t* target, const unsigned int xFactor, const unsigned int yFactor)
{
	ocean_assert(sourceTopLeft && target);
	ocean_assert(xFactor <= 128u && yFactor <= 128u);

	interpolatePixel8BitPerChannel<tChannels>(sourceTopLeft, sourceBottomLeft, target, (128u - xFactor) * (128u - yFactor), xFactor * (128u - yFactor), (128u - xFactor) * yFactor, xFactor * yFactor);
}

template <unsigned int tChannels>
inline void AdvancedFrameInterpolatorBilinearBase::interpolatePixel8BitPerChannel(const uint8_t* sourceTopLeft, const uint8_t* sourceBottomLeft, uint8_t* target, const unsigned int tx_ty_, const unsigned int txty_, const unsigned int tx_ty, const unsigned int txty)
{
	ocean_assert(sourceTopLeft && target);
	ocean_assert(tx_ty_ + txty_ + tx_ty + txty == 128u * 128u);

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		target[n] = uint8_t((sourceTopLeft[n] * tx_ty_ + sourceTopLeft[tChannels + n] * txty_ + sourceBottomLeft[n] * tx_ty + sourceBottomLeft[tChannels + n] * txty + 8192u) >> 14u);
	}
}

}

}

}

#endif // META_OCEAN_CV_ADVANCED_ADVANCED_FRAME_INTERPOLATOR_BILINEAR_BASE_H
