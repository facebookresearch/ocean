/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_INTERPOLATOR_BILINEAR_ALPHA_H
#define META_OCEAN_CV_FRAME_INTERPOLATOR_BILINEAR_ALPHA_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameBlender.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Vector2.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements bilinear frame interpolator functions for frames holding an alpha channel.
 * @tparam tAlphaAtFront True, if the alpha channel is in the front of the data channels
 * @tparam tTransparentIs0xFF True, if 0xFF is interpreted as fully transparent
 * @ingroup cv
 */
template <bool tAlphaAtFront, bool tTransparentIs0xFF>
class FrameInterpolatorBilinearAlpha
{
	public:

		/**
		 * The following comfort class provides comfortable functions simplifying prototyping applications but also increasing binary size of the resulting applications.
		 * Best practice is to avoid using these functions if binary size matters,<br>
		 * as for every comfort function a corresponding function exists with specialized functionality not increasing binary size significantly.<br>
		 */
		class OCEAN_CV_EXPORT Comfort
		{
			public:

				/**
				 * Determines the interpolated pixel values for a given pixel position in an 8 bit per channel frame.
				 * This function uses an integer interpolation with a precision of 1/128.
				 * @param frame The frame to determine the pixel values from, must be valid
				 * @param channels Number of channels of the given frame, with range [1, 8]
				 * @param width The width of the frame in pixel, with range [1, infinity)
				 * @param height The height of the frame in pixel, with range [1, infinity)
				 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
				 * @param pixelCenter The pixel center to be used during interpolation, either 'PC_TOP_LEFT' or 'PC_CENTER'
				 * @param position The position for which the interpolated pixel will be determined, with ranges [0, width - 1]x[0, height - 1] for PC_TOP_LEFT, [0, width]x[0, height] for PC_CENTER
				 * @param result Resulting pixel values, must be valid, must be valid
				 * @return True, if succeeded
				 * @tparam TScalar The scalar data type of the sub-pixel position
				 */
				template <typename TScalar = Scalar>
				static bool interpolatePixel8BitPerChannel(const uint8_t* frame, const unsigned int channels, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const PixelCenter pixelCenter, const VectorT2<TScalar>& position, uint8_t* result);
		};

	public:

		/**
		 * Determines the interpolated pixel values for a given pixel position in an 8 bit per channel frame.
		 * This function uses an integer interpolation with a precision of 1/128.
		 * @param frame The frame to determine the pixel values from, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param position The position for which the interpolated pixel will be determined, with ranges [0, width - 1]x[0, height - 1] for PC_TOP_LEFT, [0, width]x[0, height] for PC_CENTER
		 * @param result Resulting pixel values, must be valid, must be valid
		 * @tparam tChannels Number of channels of the given frame, with range [1, infinity)
		 * @tparam tPixelCenter The pixel center to be used during interpolation, either 'PC_TOP_LEFT' or 'PC_CENTER'
		 * @tparam TScalar The scalar data type of the sub-pixel position
		 */
		template <unsigned int tChannels, PixelCenter tPixelCenter = PC_TOP_LEFT, typename TScalar = Scalar>
		static inline void interpolatePixel8BitPerChannel(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const VectorT2<TScalar>& position, uint8_t* result);

		/**
		 * Determines the interpolated pixel values for a given pixel position in an 8 bit per channel frame with alpha channel.
		 * The center of each pixel is located with an offset of (0.5 x 0.5) in relation to the real pixel position.<br>
		 * The given frame is virtually extended by a fully transparent border so that this functions supports arbitrary interpolation positions.<br>
		 * If the given position lies inside the frame area of (-0.5, -0.5) -> (width + 0.5, height + 0.5) the resulting interpolation result will contain color information of the frame, otherwise a fully transparent interpolation result is provided.<br>
		 * @param frame The frame to determine the pixel values from, must be valid, with range [1, infinity)
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param position The position to determine the interpolated pixel values for, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param result Resulting pixel values, must be valid, with range [1, infinity)
		 * @tparam tChannels Number of channels of the given frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static inline void interpolateInfiniteBorder8BitPerChannel(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const Vector2& position, uint8_t* result);

	protected:

		/**
		 * Returns the offset that is applied to access the alpha channel.
		 * @return Offset for the alpha channel
		 * @tparam tChannelsWithAlpha Number of channels in the source frame (including the alpha channel)
		 */
		template <unsigned int tChannelsWithAlpha>
		static inline unsigned int alphaOffset();

		/**
		 * Returns the offset that is applied to access the first data channel.
		 * @return Offset for the first data channel
		 */
		static inline unsigned int dataOffset();
};

template <bool tAlphaAtFront, bool tTransparentIs0xFF>
template <typename TScalar>
bool FrameInterpolatorBilinearAlpha<tAlphaAtFront, tTransparentIs0xFF>::Comfort::interpolatePixel8BitPerChannel(const uint8_t* frame, const unsigned int channels, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const PixelCenter pixelCenter, const VectorT2<TScalar>& position, uint8_t* result)
{
	ocean_assert(frame != nullptr);
	ocean_assert(channels >= 1u && channels <= 8u);

	if (pixelCenter == PC_TOP_LEFT)
	{
		switch (channels)
		{
			case 1u:
				FrameInterpolatorBilinearAlpha<tAlphaAtFront, tTransparentIs0xFF>::interpolatePixel8BitPerChannel<1u, PC_TOP_LEFT, TScalar>(frame, width, height, framePaddingElements, position, result);
				return true;

			case 2u:
				FrameInterpolatorBilinearAlpha<tAlphaAtFront, tTransparentIs0xFF>::interpolatePixel8BitPerChannel<2u, PC_TOP_LEFT, TScalar>(frame, width, height, framePaddingElements, position, result);
				return true;

			case 3u:
				FrameInterpolatorBilinearAlpha<tAlphaAtFront, tTransparentIs0xFF>::interpolatePixel8BitPerChannel<3u, PC_TOP_LEFT, TScalar>(frame, width, height, framePaddingElements, position, result);
				return true;

			case 4u:
				FrameInterpolatorBilinearAlpha<tAlphaAtFront, tTransparentIs0xFF>::interpolatePixel8BitPerChannel<4u, PC_TOP_LEFT, TScalar>(frame, width, height, framePaddingElements, position, result);
				return true;

			case 5u:
				FrameInterpolatorBilinearAlpha<tAlphaAtFront, tTransparentIs0xFF>::interpolatePixel8BitPerChannel<5u, PC_TOP_LEFT, TScalar>(frame, width, height, framePaddingElements, position, result);
				return true;

			case 6u:
				FrameInterpolatorBilinearAlpha<tAlphaAtFront, tTransparentIs0xFF>::interpolatePixel8BitPerChannel<6u, PC_TOP_LEFT, TScalar>(frame, width, height, framePaddingElements, position, result);
				return true;

			case 7u:
				FrameInterpolatorBilinearAlpha<tAlphaAtFront, tTransparentIs0xFF>::interpolatePixel8BitPerChannel<7u, PC_TOP_LEFT, TScalar>(frame, width, height, framePaddingElements, position, result);
				return true;

			case 8u:
				FrameInterpolatorBilinearAlpha<tAlphaAtFront, tTransparentIs0xFF>::interpolatePixel8BitPerChannel<8u, PC_TOP_LEFT, TScalar>(frame, width, height, framePaddingElements, position, result);
				return true;
		}
	}
	else
	{
		ocean_assert(pixelCenter == PC_CENTER);

		switch (channels)
		{
			case 1u:
				FrameInterpolatorBilinearAlpha<tAlphaAtFront, tTransparentIs0xFF>::interpolatePixel8BitPerChannel<1u, PC_CENTER, TScalar>(frame, width, height, framePaddingElements, position, result);
				return true;

			case 2u:
				FrameInterpolatorBilinearAlpha<tAlphaAtFront, tTransparentIs0xFF>::interpolatePixel8BitPerChannel<2u, PC_CENTER, TScalar>(frame, width, height, framePaddingElements, position, result);
				return true;

			case 3u:
				FrameInterpolatorBilinearAlpha<tAlphaAtFront, tTransparentIs0xFF>::interpolatePixel8BitPerChannel<3u, PC_CENTER, TScalar>(frame, width, height, framePaddingElements, position, result);
				return true;

			case 4u:
				FrameInterpolatorBilinearAlpha<tAlphaAtFront, tTransparentIs0xFF>::interpolatePixel8BitPerChannel<4u, PC_CENTER, TScalar>(frame, width, height, framePaddingElements, position, result);
				return true;

			case 5u:
				FrameInterpolatorBilinearAlpha<tAlphaAtFront, tTransparentIs0xFF>::interpolatePixel8BitPerChannel<5u, PC_CENTER, TScalar>(frame, width, height, framePaddingElements, position, result);
				return true;

			case 6u:
				FrameInterpolatorBilinearAlpha<tAlphaAtFront, tTransparentIs0xFF>::interpolatePixel8BitPerChannel<6u, PC_CENTER, TScalar>(frame, width, height, framePaddingElements, position, result);
				return true;

			case 7u:
				FrameInterpolatorBilinearAlpha<tAlphaAtFront, tTransparentIs0xFF>::interpolatePixel8BitPerChannel<7u, PC_CENTER, TScalar>(frame, width, height, framePaddingElements, position, result);
				return true;

			case 8u:
				FrameInterpolatorBilinearAlpha<tAlphaAtFront, tTransparentIs0xFF>::interpolatePixel8BitPerChannel<8u, PC_CENTER, TScalar>(frame, width, height, framePaddingElements, position, result);
				return true;
		}
	}

	ocean_assert(false && "Invalid channel number");
	return false;
}

template <bool tAlphaAtFront, bool tTransparentIs0xFF>
template <unsigned int tChannels, PixelCenter tPixelCenter, typename TScalar>
inline void FrameInterpolatorBilinearAlpha<tAlphaAtFront, tTransparentIs0xFF>::interpolatePixel8BitPerChannel(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const VectorT2<TScalar>& position, uint8_t* result)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(frame != nullptr && result != nullptr);
	ocean_assert(width > 0u && height > 0u);

	ocean_assert(position.x() >= TScalar(0));
	ocean_assert(position.y() >= TScalar(0));

	if constexpr (tPixelCenter == PC_TOP_LEFT)
	{
		ocean_assert(position.x() <= TScalar(width - 1u));
		ocean_assert(position.y() <= TScalar(height - 1u));
	}
	else
	{
		ocean_assert(tPixelCenter == PC_CENTER);

		ocean_assert(position.x() <= TScalar(width));
		ocean_assert(position.y() <= TScalar(height));
	}

	const unsigned int frameStrideElements = width * tChannels + framePaddingElements;

	const TScalar xShifted = tPixelCenter == PC_TOP_LEFT ? position.x() : std::max(TScalar(0.0), position.x() - TScalar(0.5));
	const TScalar yShifted = tPixelCenter == PC_TOP_LEFT ? position.y() : std::max(TScalar(0.0), position.y() - TScalar(0.5));

	const unsigned int left = (unsigned int)(xShifted);
	const unsigned int top = (unsigned int)(yShifted);

	ocean_assert(left < width);
	ocean_assert(top < height);

	const unsigned int rightOffset = left + 1u < width ? tChannels : 0u;
	const unsigned int bottomOffset = top + 1u < height ? frameStrideElements : 0u;

	const uint8_t* topLeft = frame + top * frameStrideElements + left * tChannels;

	const TScalar sFactorRight = xShifted - TScalar(left);
	const TScalar sFactorBottom = yShifted - TScalar(top);

	const unsigned int factorRight = (unsigned int)(sFactorRight * TScalar(128) + TScalar(0.5));
	const unsigned int factorBottom = (unsigned int)(sFactorBottom * TScalar(128) + TScalar(0.5));

	const unsigned int factorLeft = 128u - factorRight;
	const unsigned int factorTop = 128u - factorBottom;

	const unsigned int factorTopLeft = factorTop * factorLeft;
	const unsigned int factorTopRight = factorTop * factorRight;
	const unsigned int factorBottomLeft = factorBottom * factorLeft;
	const unsigned int factorBottomRight = factorBottom * factorRight;

	const unsigned int factorTopLeftAlpha = factorTopLeft * FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(topLeft[alphaOffset<tChannels>()]);
	const unsigned int factorTopRightAlpha = factorTopRight * FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(topLeft[rightOffset + alphaOffset<tChannels>()]);
	const unsigned int factorBottomLeftAlpha = factorBottomLeft * FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(topLeft[bottomOffset + alphaOffset<tChannels>()]);
	const unsigned int factorBottomRightAlpha = factorBottomRight * FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(topLeft[bottomOffset + rightOffset + alphaOffset<tChannels>()]);

	const unsigned int sumFactorsAlpha = factorTopLeftAlpha + factorTopRightAlpha + factorBottomLeftAlpha + factorBottomRightAlpha;

	const uint8_t* const pixelTopLeft = topLeft;
	const uint8_t* const pixelTopRight = topLeft + rightOffset;
	const uint8_t* const pixelBottomLeft = topLeft + bottomOffset;
	const uint8_t* const pixelBottomRight = topLeft + bottomOffset + rightOffset;

	// determine interpolated color data channels
	if (sumFactorsAlpha != 0u)
	{
		const unsigned int sumFactorsAlpha_2 = sumFactorsAlpha / 2u;

		for (unsigned int n = dataOffset(); n < tChannels + dataOffset() - 1u; ++n)
		{
			result[n] = uint8_t((pixelTopLeft[n] * factorTopLeftAlpha + pixelTopRight[n] * factorTopRightAlpha + pixelBottomLeft[n] * factorBottomLeftAlpha + pixelBottomRight[n] * factorBottomRightAlpha + sumFactorsAlpha_2) / sumFactorsAlpha);
		}
	}
	else
	{
		for (unsigned int n = dataOffset(); n < tChannels + dataOffset() - 1u; ++n)
		{
			result[n] = uint8_t((pixelTopLeft[n] * factorTopLeft + pixelTopRight[n] * factorTopRight + pixelBottomLeft[n] * factorBottomLeft + pixelBottomRight[n] * factorBottomRight + 8192u) / 16384u);
		}
	}

	// determine interpolated transparency data channel (which is a standard bi-linear interpolation)
	if constexpr (tTransparentIs0xFF)
	{
		result[alphaOffset<tChannels>()] = uint8_t((factorTopLeft * pixelTopLeft[alphaOffset<tChannels>()]
														+ factorTopRight * pixelTopRight[alphaOffset<tChannels>()]
														+ factorBottomLeft * pixelBottomLeft[alphaOffset<tChannels>()]
														+ factorBottomRight * pixelBottomRight[alphaOffset<tChannels>()] + 8192u) / 16384u);
	}
	else
	{
		result[alphaOffset<tChannels>()] = FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(uint8_t((sumFactorsAlpha + 8192u) / 16384u));
	}
}

template <bool tAlphaAtFront, bool tTransparentIs0xFF>
template <unsigned int tChannels>
inline void FrameInterpolatorBilinearAlpha<tAlphaAtFront, tTransparentIs0xFF>::interpolateInfiniteBorder8BitPerChannel(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const Vector2& position, uint8_t* result)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(frame && result);

	const Vector2 pos(position.x() - Scalar(0.5), position.y() - Scalar(0.5));

	// check whether the position is outside the frame and will therefore be 100% transparent
	if (pos.x() <= Scalar(-1) || pos.y() <= Scalar(-1) || pos.x() >= Scalar(width) || pos.y() >= Scalar(height))
	{
		for (unsigned int n = 0u; n < tChannels - 1u; ++n)
		{
			result[n + FrameBlender::SourceOffset<tAlphaAtFront>::data()] = 0x00;
		}

		result[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()] = FrameBlender::fullTransparent8Bit<tTransparentIs0xFF>();
		return;
	}

	const unsigned int frameStrideElements = width * tChannels + framePaddingElements;

	const int left = int(Numeric::floor(pos.x()));
	const int top = int(Numeric::floor(pos.y()));

	ocean_assert(left >= -1 && left < int(width));
	ocean_assert(top >= -1 && top < int(height));

	if ((unsigned int)left < width - 1u && (unsigned int)top < height - 1u)
	{
		// we have a valid pixel position for the left, top, right and bottom pixel

		const unsigned int txi = (unsigned int)((pos.x() - Scalar(left)) * Scalar(128) + Scalar(0.5));
		const unsigned int txi_ = 128u - txi;

		const unsigned int tyi = (unsigned int)((pos.y() - Scalar(top)) * Scalar(128) + Scalar(0.5));
		const unsigned int tyi_ = 128u - tyi;

		const uint8_t* topLeft = frame + top * frameStrideElements + left * tChannels;

		const unsigned int txty = txi * tyi * FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(topLeft[frameStrideElements + tChannels + alphaOffset<tChannels>()]);
		const unsigned int txty_ = txi * tyi_ * FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(topLeft[tChannels + alphaOffset<tChannels>()]);
		const unsigned int tx_ty = txi_ * tyi * FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(topLeft[frameStrideElements + alphaOffset<tChannels>()]);
		const unsigned int tx_ty_ = txi_ * tyi_ * FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(topLeft[alphaOffset<tChannels>()]);

		const unsigned int denominator = tx_ty_ + txty_ + tx_ty + txty;

		// determine interpolated color data channels
		if (denominator != 0u)
		{
			const unsigned int denominator_2 = denominator / 2u;

			for (unsigned int n = dataOffset(); n < tChannels + dataOffset() - 1u; ++n)
			{
				result[n] = uint8_t((topLeft[n] * tx_ty_ + topLeft[tChannels + n] * txty_ + topLeft[frameStrideElements + n] * tx_ty + topLeft[frameStrideElements + tChannels + n] * txty + denominator_2) / denominator);
			}
		}
		else
		{
			for (unsigned int n = dataOffset(); n < tChannels + dataOffset() - 1u; ++n)
			{
				result[n] = uint8_t((topLeft[n] * txi_ * tyi_ + topLeft[tChannels + n] * txi * tyi_ + topLeft[frameStrideElements + n] * txi_ * tyi + topLeft[frameStrideElements + tChannels + n] * txi * tyi + 8192u) >> 14u);
			}
		}

		// determine interpolated transparency data channel (which is a standard bi-linear interpolation)
		if constexpr (tTransparentIs0xFF)
		{
			// we have to handle this in a special way as we might get rounding errors otherwise
			result[alphaOffset<tChannels>()] = uint8_t((txi_ * tyi_ * topLeft[alphaOffset<tChannels>()]
																+ txi * tyi_ * topLeft[tChannels + alphaOffset<tChannels>()]
																+ txi_ * tyi * topLeft[frameStrideElements + alphaOffset<tChannels>()]
																+ txi * tyi * topLeft[frameStrideElements + tChannels + alphaOffset<tChannels>()] + 8192u) >> 14u);
		}
		else
		{
			result[alphaOffset<tChannels>()] = FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(uint8_t((denominator + 8192u) >> 14u));
		}
	}
	else
	{
		// we do not have a valid pixel for all 4-neighborhood pixels

		const unsigned int txi = (unsigned int)((pos.x() - Scalar(left)) * Scalar(128) + Scalar(0.5));
		const unsigned int txi_ = 128u - txi;

		const unsigned int tyi = (unsigned int)((pos.y() - Scalar(top)) * Scalar(128) + Scalar(0.5));
		const unsigned int tyi_ = 128u - tyi;

		const unsigned int rightOffset = (left >= 0 && left + 1u < width) ? tChannels : 0u;
		const unsigned int bottomOffset = (top >= 0 && top + 1u < height) ? frameStrideElements : 0u;

		ocean_assert(left < int(width) && top < int(height));
		const uint8_t* topLeft = frame + max(0, top) * frameStrideElements + max(0, left) * tChannels;

		const uint8_t alphaTopLeft = (left >= 0 && top >= 0) ? FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(topLeft[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()])
																	: FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(FrameBlender::fullTransparent8Bit<tTransparentIs0xFF>());

		const uint8_t alphaTopRight = (left + 1u < width && top >= 0) ? FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(topLeft[rightOffset + FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()])
																	: FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(FrameBlender::fullTransparent8Bit<tTransparentIs0xFF>());

		const uint8_t alphaBottomLeft = (left >= 0 && top + 1u < height) ? FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(topLeft[bottomOffset + FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()])
																	: FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(FrameBlender::fullTransparent8Bit<tTransparentIs0xFF>());

		const uint8_t alphaBottomRight = (left + 1u < width && top + 1u < height) ? FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(topLeft[bottomOffset + rightOffset + FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()])
																	: FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(FrameBlender::fullTransparent8Bit<tTransparentIs0xFF>());

		const unsigned int txty = txi * tyi * alphaBottomRight;
		const unsigned int txty_ = txi * tyi_ * alphaTopRight;
		const unsigned int tx_ty = txi_ * tyi * alphaBottomLeft;
		const unsigned int tx_ty_ = txi_ * tyi_ * alphaTopLeft;

		const unsigned int denominator = tx_ty_ + txty_ + tx_ty + txty;

		// determine interpolated color data channels
		if (denominator != 0u)
		{
			const unsigned int denominator_2 = denominator / 2u;

			for (unsigned int n = dataOffset(); n < tChannels + dataOffset() - 1u; ++n)
			{
				result[n] = uint8_t((topLeft[n] * tx_ty_ + topLeft[rightOffset + n] * txty_ + topLeft[bottomOffset + n] * tx_ty + topLeft[bottomOffset + rightOffset + n] * txty + denominator_2) / denominator);
			}
		}
		else
		{
			for (unsigned int n = dataOffset(); n < tChannels + dataOffset() - 1u; ++n)
			{
				result[n] = uint8_t((topLeft[n] * txi_ * tyi_ + topLeft[rightOffset + n] * txi * tyi_ + topLeft[bottomOffset + n] * txi_ * tyi + topLeft[bottomOffset + rightOffset + n] * txi * tyi + 8192u) >> 14u);
			}
		}

		if constexpr (tTransparentIs0xFF)
		{
			// we have to handle this in a special way as we might get rounding errors otherwise
			result[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()] = uint8_t((txi_ * tyi_ * FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(alphaTopLeft)
																								+ txi * tyi_ * FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(alphaTopRight)
																								+ txi_ * tyi * FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(alphaBottomLeft)
																								+ txi * tyi * FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(alphaBottomRight) + 8192u) >> 14u);
		}
		else
		{
			result[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()] = FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(uint8_t((denominator + 8192u) >> 14u));
		}
	}
}

template <bool tAlphaAtFront, bool tTransparentIs0xFF>
template <unsigned int tChannelsWithAlpha>
inline unsigned int FrameInterpolatorBilinearAlpha<tAlphaAtFront, tTransparentIs0xFF>::alphaOffset()
{
	return FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannelsWithAlpha>();
}

template <bool tAlphaAtFront, bool tTransparentIs0xFF>
inline unsigned int FrameInterpolatorBilinearAlpha<tAlphaAtFront, tTransparentIs0xFF>::dataOffset()
{
	return FrameBlender::SourceOffset<tAlphaAtFront>::data();
}

}

}

#endif // META_OCEAN_CV_FRAME_INTERPOLATOR_BILINEAR_ALPHA_H
