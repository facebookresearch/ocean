/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_INTERPOLATOR_TRILINEAR_H
#define META_OCEAN_CV_FRAME_INTERPOLATOR_TRILINEAR_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FramePyramid.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"
#include "ocean/cv/FrameInterpolatorBilinearAlpha.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements tri-linear frame interpolator functions.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameInterpolatorTrilinear
{
	public:

		/**
		 * Resizes the finest layer of a given frame pyramid by a tri-linear interpolation and optionally uses a worker object to distribute the computational load.
		 * Beware: This method assumes that the pixel format of the target is identical to the pixel format of the frame pyramid.
		 * @param source Frame pyramid provided the frame which will be resized
		 * @param target The target frame buffer
		 * @param targetWidth Width of the target frame in pixel, with range [1, infinity)
		 * @param targetHeight Height of the target frame in pixel, with range [1, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object used for load distribution
		 * @return True, if the frame could be resized
		 */
		static bool resize(const FramePyramid& source, uint8_t* target, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Resizes the finest layer of a given frame pyramid by a tri-linear interpolation and optionally uses a worker object to distribute the computational load.
		 * @param source Frame pyramid whose finest layer is resized
		 * @param target The target frame buffer
		 * @param worker Optional worker object used for load distribution
		 * @return True, if the frame could be resized
		 */
		static bool resize(const FramePyramid& source, Frame& target, Worker* worker = nullptr);

		/**
		 * Resizes a given frame by a tri-linear interpolation and optionally uses a worker object to distribute the computational load.
		 * This method creates a new frame pyramid of the source frame, which creates additional computational load.
		 * @param source The source frame buffer to resize
		 * @param target The target frame buffer
		 * @param worker Optional worker object used for load distribution
		 * @return True, if the frame could be resized
		 */
		static bool resize(const Frame& source, Frame& target, Worker* worker = nullptr);

		/**
		 * Resizes a given frame by a tri-linear interpolation and optionally uses a worker object to distribute the computational load.
		 * This method creates a new frame pyramid of the source frame, which creates additional computational load.
		 * @param frame The frame to be resized in place
		 * @param newWidth New width of the frame in pixel, with range [1, infinity)
		 * @param newHeight New height of the frame in pixel, with range [1, infinity)
		 * @param worker Optional worker object used for load distribution
		 * @return True, if the frame could be resized
		 */
		static bool resize(Frame& frame, const unsigned int newWidth, const unsigned int newHeight, Worker* worker = nullptr);

		/**
		 * Transforms a given 8 bit per channel input frame into an output frame by application of a homography.
		 * @param sourcePyramid Frame pyramid of input frame that will be transformed
		 * @param width The width of both input images in pixel
		 * @param height The height of both input images pixel
		 * @param homography Homography used to transform the given input frame
		 * @param borderColor Color of undefined pixel positions, the size of the buffer must match to the number of channels, nullptr to assign 0x00 to each channel
		 * @param target Output frame using the given homography
		 * @param worker Optional worker object to distribute the computational load
		 * @tparam tChannels Number of channels of the frame
		 * @see homographyWithCamera8BitPerChannel().
		 */
		template <unsigned int tChannels>
		static inline void homography8BitPerChannel(const FramePyramid& sourcePyramid, const unsigned int width, const unsigned int height, const SquareMatrix3& homography, const uint8_t* borderColor, uint8_t* target, Worker* worker = nullptr);

		/**
		 * Calculates a 3D position usable for tri-linear interpolation. The specified image coordinates define the center and four corner positions of quad.
		 * The center position determines the x- and y-coordinate of the resulting 3D sampling position. The corner positions determine the z-coordinate of the
		 * resulting sampling position and the pyramid layers from which samples are retrieved. The vertices have to be specified in clockwise order.
		 * @param layerCount Number of layers supported by the pyramid
		 * @param centerPosition Quad center coordinates in pixels of the finest pyramid layer, used for the actual texture look-up
		 * @param cornerPosition1 First quad corner coordinates in pixels of the finest pyramid layer, used for interpolation between pyramid layers
		 * @param cornerPosition2 Second quad corner coordinates in pixels of the finest pyramid layer, used for interpolation between pyramid layers
		 * @param cornerPosition3 Third quad corner coordinates in pixels of the finest pyramid layer, used for interpolation between pyramid layers
		 * @param cornerPosition4 Fourth quad corner coordinates in pixels of the finest pyramid layer, used for interpolation between pyramid layers
		 * @return The image coordinates (x, y) and layer (z) within the range [0, framePyramid.finestLayerWidth()]x[0, framePyramid.finestLayerWidth()]x[0, layerCount - 1u]
		 */
		static inline Vector3 interpolatePosition(const unsigned int layerCount, const Vector2& centerPosition, const Vector2& cornerPosition1, const Vector2& cornerPosition2, const Vector2& cornerPosition3, const Vector2& cornerPosition4);

		/**
		 * Performs a pixel lookup in the frame pyramid using tri-linear interpolation.<br>
		 * The position is given as 3D-vector, where x & y specify the 2d image coordinates in pixel units of the finest frame pyramid layer<br>
		 * and z specifies the pyramid layer index. Values with fractions for x,y & z are used as interpolation weights between neighboring pixels and layers.
		 * @param framePyramid Frame pyramid to determine the pixel values from
		 * @param position Image coordinates (x, y) and layer (z), for which pixel values are interpolated  using tri-linear interpolation within the range [0, framePyramid.finestLayerWidth()]x[0, framePyramid.finestLayerWidth()]x[0, layerCount - 1u]
		 * @param result Pointer to resulting interpolated pixel values (1Byte per color channel)
		 * @tparam tChannels Defines the number of channels the frame holds
		 */
		template <unsigned int tChannels>
		static inline void interpolateFullBorder8BitPerChannel(const FramePyramid& framePyramid, const Vector3& position, uint8_t* result);

		/**
		 * Performs a pixel lookup in the frame pyramid using tri-linear interpolation with infinite transparent frame border.<br>
		 * The position is given as 3D-vector, where x & y specify the 2d image coordinates in pixel units of the finest frame pyramid layer<br>
		 * and z specifies the pyramid layer index. Values with fractions for x,y & z are used as interpolation weights between neighboring pixels and layers.
		 * @param framePyramid Frame pyramid to determine the pixel values from
		 * @param position Image coordinates (x, y) and layer (z), for which pixel values are interpolated  using tri-linear interpolation within the range [0, framePyramid.finestLayerWidth()]x[0, framePyramid.finestLayerWidth()]x[0, layerCount - 1u]
		 * @param result Pointer to resulting interpolated pixel values (1Byte per color channel)
		 * @tparam tChannels Defines the number of channels the frame holds
		 * @tparam tAlphaAtFront True, if the alpha channel is in the front of the data channels
		 * @tparam tTransparentIs0xFF True, if 0xFF is interpreted as fully transparent
		 */
		template <unsigned int tChannels, bool tAlphaAtFront, bool tTransparentIs0xFF>
		static inline void interpolateInfiniteBorder8BitPerChannelAlpha(const FramePyramid& framePyramid, const Vector3& position, uint8_t* result);

		/**
		 * Renders the passed source frame pyramid into the target frame using tri-linear interpolation.
		 * @param source The source frame pyramid that is read from. The pyramid needs to have the appropriate layers
		 * @param target The target frame buffer
		 * @param targetWidth Width of the target frame in pixel
		 * @param targetHeight Height of the target frame in pixel
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object used for load distribution
		 * @tparam tChannels Defines the number of channels the frame holds
		 */
		template <unsigned int tChannels>
		static inline void resize8BitPerChannel(const FramePyramid& source, uint8_t* target, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int targetPaddingElements, Worker* worker = nullptr);

	protected:

		/**
		 * Performs a pixel lookup in the frame pyramid using tri-linear interpolation.<br>
		 * The position is given as 3D-vector, where x & y specify the 2d image coordinates in pixel units of the finest frame pyramid layer<br>
		 * and z specifies the pyramid layer index. Values with fractions for x,y & z are used as interpolation weights between neighboring pixels and layers.
		 * @param framePyramid Frame pyramid to determine the pixel values from
		 * @param position Image coordinates (x, y) and layer (z), for which pixel values are interpolated  using tri-linear interpolation within the range [0, framePyramid.finestLayerWidth()]x[0, framePyramid.finestLayerWidth()]x[0, layerCount - 1u]
		 * @param result Pointer to resulting interpolated pixel values (1Byte per color channel)
		 * @tparam tChannels Defines the number of channels the frame holds
		 * @tparam tBilinearInterpolationFunction The bilinear interpolation function that will be used to interpolate the pixel values within one frame pyramid layer
		 * @tparam tLinearInterpolationFunction The linear interpolation function that will be used to interpolate the pixel values between two frame pyramid layers
		 * @see FrameInterpolatorBilinear::interpolatePixel8BitPerChannel(), FrameInterpolatorBilinear::interpolateFullAlphaBorder8BitPerChannel().
		 */
		template <unsigned int tChannels, void tBilinearInterpolationFunction(const uint8_t*, const unsigned int, const unsigned int, const unsigned int, const Vector2&, uint8_t*), void tLinearInterpolationFunction(const uint8_t*, const unsigned int, const uint8_t*, uint8_t*)>
		static inline void interpolate8BitPerChannel(const FramePyramid& framePyramid, const Vector3& position, uint8_t* result);

		/**
		 * This function determines the linear interpolation result for to given layer pixels.
		 * @param first The first pixel data
		 * @param firstFactor First interpolation factor, with range [0, 128]
		 * @param second The second pixel data
		 * @param result Interpolation result
		 * @tparam tChannels Defines the number of channels the frame holds
		 */
		template <unsigned int tChannels>
		static inline void interpolateTwoPixels8BitPerChannel(const uint8_t* first, const unsigned int firstFactor, const uint8_t* second, uint8_t* result);

		/**
		 * This function determines the linear interpolation result for to given layer pixels while the interpolation result respects the alpha values of both pixels.
		 * @param first The first pixel data
		 * @param firstFactor First interpolation factor, with range [0, 128]
		 * @param second The second pixel data
		 * @param result Interpolation result
		 * @tparam tChannels Defines the number of channels the frame holds
		 * @tparam tAlphaAtFront True, if the alpha channel is in the front of the data channels
		 * @tparam tTransparentIs0xFF True, if 0xFF is interpreted as fully transparent
		 */
		template <unsigned int tChannels, bool tAlphaAtFront, bool tTransparentIs0xFF>
		static inline void interpolateTwoPixels8BitPerChannelAlpha(const uint8_t* first, const unsigned int firstFactor, const uint8_t* second, uint8_t* result);

		/**
		 * Renders the passed source frame pyramid into the target frame using tri-linear interpolation.
		 * @param source The source frame pyramid that is read from. The pyramid needs to have the appropriate layers
		 * @param target The target frame buffer
		 * @param targetWidth Width of the target frame in pixel
		 * @param targetHeight Height of the target frame in pixel
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param firstTargetRow First (including) row to convert
		 * @param numberTargetRows Number of rows to convert
		 * @tparam tChannels Defines the number of channels the frame holds
		 */
		template <unsigned int tChannels>
		static void resize8BitPerChannelSubset(const FramePyramid* source, uint8_t* target, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int targetPaddingElements, const unsigned int firstTargetRow, const unsigned int numberTargetRows);

		/**
		 * Transforms an 8 bit per channel frame using the given homography.
		 * @param sourcePyramid Frame pyramid of input frame that will be transformed
		 * @param width The width of both input images in pixel
		 * @param height The height of both input images pixel
		 * @param homography Homography used to transform the given input frame
		 * @param borderColor Color of undefined pixel positions, the size of the buffer must match to the number of channels, nullptr to assign 0x00 to each channel
		 * @param target Output frame resulting by application of the given homography
		 * @param firstRow The first row to be handled
		 * @param numberRows Number of rows to be handled
		 * @tparam tChannels Number of frame channels
		 */
		template <unsigned int tChannels>
		static void homography8BitPerChannelSubset(const FramePyramid* sourcePyramid, const unsigned int width, const unsigned int height, const SquareMatrix3* homography, const uint8_t* borderColor, uint8_t* target, const unsigned int firstRow, const unsigned int numberRows);
};

inline Vector3 Ocean::CV::FrameInterpolatorTrilinear::interpolatePosition(const unsigned int layerCount, const Vector2& centerPosition, const Vector2& cornerPosition1, const Vector2& cornerPosition2, const Vector2& cornerPosition3, const Vector2& cornerPosition4)
{
	const Scalar distance2 = ((cornerPosition3 - cornerPosition1).length() + (cornerPosition4 - cornerPosition2).length()) * Scalar(0.5);

	// 1.4426950408889634073599246810019 == Scalar(1) / Numeric::log(2);
	return Vector3(centerPosition, minmax<Scalar>(Scalar(0), Numeric::log(distance2) * Scalar(1.4426950408889634073599246810019), Scalar(layerCount - 1u)));
}

template <unsigned int tChannels>
inline void FrameInterpolatorTrilinear::interpolateFullBorder8BitPerChannel(const FramePyramid& framePyramid, const Vector3& position, uint8_t* result)
{
	ocean_assert(framePyramid.isValid() && result != nullptr);
	ocean_assert(framePyramid.frameType().numberPlanes() == 1u);
	ocean_assert(FrameType::formatIsGeneric(framePyramid.frameType().pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, tChannels));

	interpolate8BitPerChannel<tChannels, &FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<tChannels, PC_CENTER, Scalar>, &FrameInterpolatorTrilinear::interpolateTwoPixels8BitPerChannel<tChannels> >(framePyramid, position, result);
}

template <unsigned int tChannels, bool tAlphaAtFront, bool tTransparentIs0xFF>
inline void FrameInterpolatorTrilinear::interpolateInfiniteBorder8BitPerChannelAlpha(const FramePyramid& framePyramid, const Vector3& position, uint8_t* result)
{
	ocean_assert(framePyramid.isValid() && result != nullptr);
	ocean_assert(framePyramid.frameType().numberPlanes() == 1u);
	ocean_assert(FrameType::formatIsGeneric(framePyramid.frameType().pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, tChannels));

	interpolate8BitPerChannel<tChannels, &FrameInterpolatorBilinearAlpha<tAlphaAtFront, tTransparentIs0xFF>::template interpolateInfiniteBorder8BitPerChannel<tChannels>, &FrameInterpolatorTrilinear::interpolateTwoPixels8BitPerChannelAlpha<tChannels, tAlphaAtFront, tTransparentIs0xFF> >(framePyramid, position, result);
}

template <unsigned int tChannels, void tBilinearInterpolationFunction(const uint8_t*, const unsigned int, const unsigned int, const unsigned int, const Vector2&, uint8_t*), void tLinearInterpolationFunction(const uint8_t*, const unsigned int, const uint8_t*, uint8_t*)>
inline void FrameInterpolatorTrilinear::interpolate8BitPerChannel(const FramePyramid& framePyramid, const Vector3& position, uint8_t* result)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(result != nullptr);
	ocean_assert(framePyramid.layers() > 0u);

	ocean_assert(position.z() >= 0);
	ocean_assert(position.z() <= Scalar(framePyramid.layers() - 1u));

	const unsigned int indexFine = (unsigned int)position.z();
	const unsigned int indexCoarse = indexFine + (indexFine + 1u < framePyramid.layers() ? 1u : 0u);

	const Frame& frameFine = framePyramid.layer(indexFine);
	const unsigned int widthFine = frameFine.width();
	const unsigned int heightFine = frameFine.height();

	const Frame& frameCoarse = framePyramid.layer(indexCoarse);
	const unsigned int widthCoarse = frameCoarse.width();
	const unsigned int heightCoarse = frameCoarse.height();

	// Pixel scale factor between layer 0 and coarse/fine layer:
	//const Scalar scaleFineFactor   = Scalar(1) / Scalar(1u << indexFine);
	//const Scalar scaleCoarseFactor = Scalar(1) / Scalar(1u << indexCoarse);

	const Vector2 positionFine = Vector2(position.x() * (Scalar(widthFine)  / Scalar(framePyramid.finestWidth())), position.y() * (Scalar(heightFine) / Scalar(framePyramid.finestHeight())));
	const Vector2 positionCoarse = Vector2(position.x() * (Scalar(widthCoarse)  / Scalar(framePyramid.finestWidth())), position.y() * (Scalar(heightCoarse) / Scalar(framePyramid.finestHeight())));

	// Perform bilinear interpolation on the two layers:
	uint8_t valueFine[tChannels];
	uint8_t valueCoarse[tChannels];

	tBilinearInterpolationFunction(frameFine.constdata<uint8_t>(), widthFine, heightFine, frameFine.paddingElements(), positionFine, valueFine);
	tBilinearInterpolationFunction(frameCoarse.constdata<uint8_t>(), widthCoarse, heightCoarse, frameCoarse.paddingElements(), positionCoarse, valueCoarse);

	// Interpolate both values:
	const Scalar tz = position.z() - Scalar(indexFine);
	ocean_assert(tz >= 0 && tz <= 1);

	const unsigned int tzi = (unsigned int)(tz * Scalar(128) + Scalar(0.5));
	ocean_assert(tzi >= 0u && tzi <= 128u);

	tLinearInterpolationFunction(valueCoarse, tzi, valueFine, result);
}

template <unsigned int tChannels>
inline void FrameInterpolatorTrilinear::interpolateTwoPixels8BitPerChannel(const uint8_t* first, const unsigned int firstFactor, const uint8_t* second, uint8_t* result)
{
	ocean_assert(first && second && result);
	ocean_assert(firstFactor <= 128u);

	const unsigned int secondFactor = 128u - firstFactor;

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		result[n] = (unsigned char)((first[n] * firstFactor + second[n] * secondFactor + 64u) >> 7u);
	}
}

template <unsigned int tChannels, bool tAlphaAtFront, bool tTransparentIs0xFF>
inline void FrameInterpolatorTrilinear::interpolateTwoPixels8BitPerChannelAlpha(const uint8_t* first, const unsigned int firstFactor, const uint8_t* second, uint8_t* result)
{
	ocean_assert(first && second && result);
	ocean_assert(firstFactor <= 128u);

	const unsigned char firstAlpha = first[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()];
	const unsigned char secondAlpha = second[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()];

	const unsigned int secondFactor = 128u - firstFactor;
	const unsigned int denominator = firstFactor * FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(firstAlpha)
										+ secondFactor * FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(secondAlpha);

	if (denominator != 0u)
	{
		const unsigned int denominator_2 = denominator / 2u;

		for (unsigned int n = FrameBlender::SourceOffset<tAlphaAtFront>::data(); n < tChannels + FrameBlender::SourceOffset<tAlphaAtFront>::data() - 1u; ++n)
		{
			result[n] = (unsigned char)((first[n] * firstFactor * FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(firstAlpha)
								+ second[n] * secondFactor * FrameBlender::alpha8BitToOpaqueIs0xFF<tTransparentIs0xFF>(secondAlpha) + denominator_2) / denominator);
		}

		result[FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannels>()] = (unsigned char)((firstAlpha * firstFactor + secondAlpha * secondFactor + 64) >> 7u);
	}
	else
	{
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			result[n] = uint8_t((first[n] * firstFactor + second[n] * secondFactor + 64u) >> 7u);
		}
	}
}

template <unsigned int tChannels>
inline void FrameInterpolatorTrilinear::resize8BitPerChannel(const FramePyramid& source, uint8_t* target, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int targetPaddingElements, Worker* worker)
{
	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameInterpolatorTrilinear::resize8BitPerChannelSubset<tChannels>, &source, target, targetWidth, targetHeight, targetPaddingElements, 0u, 0u), 0u, targetHeight);
	}
	else
	{
		resize8BitPerChannelSubset<tChannels>(&source, target, targetWidth, targetHeight, targetPaddingElements, 0u, targetHeight);
	}
}

template <unsigned int tChannels>
void FrameInterpolatorTrilinear::resize8BitPerChannelSubset(const FramePyramid* source, uint8_t* target, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int targetPaddingElements, const unsigned int firstTargetRow, const unsigned int numberTargetRows)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(source->isValid());
	ocean_assert(source->layers() != 0u);

	ocean_assert(source->finestLayer().numberPlanes() == 1u);
	ocean_assert(FrameType::formatIsGeneric(source->finestLayer().pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, tChannels));

	const unsigned int sourceWidth = source->finestWidth();
	const unsigned int sourceHeight = source->finestHeight();

	ocean_assert(sourceWidth > 0);
	ocean_assert(sourceHeight > 0);

	const Scalar targetToSourceX = Scalar(sourceWidth) / Scalar(targetWidth);
	const Scalar targetToSourceY = Scalar(sourceHeight) / Scalar(targetHeight);

	const Scalar squareDiagonal = Numeric::sqr(targetToSourceX) + Numeric::sqr(targetToSourceY);
	const Scalar recipprocalLog2 = Scalar(1) / (2 * Numeric::log(2));

	const Scalar layer = minmax(Scalar(0), Numeric::log(squareDiagonal) * recipprocalLog2 - Scalar(0.5), Scalar(source->layers() - 1u)); // log_2(diagonal) - 0.5
	ocean_assert(Numeric::ceil(layer) <= Scalar(source->layers()));

	const unsigned int targetStrideElements = targetWidth * tChannels + targetPaddingElements;

	Vector3 position = Vector3(0, 0, layer);

	for (unsigned int ty = firstTargetRow; ty < firstTargetRow + numberTargetRows; ++ty)
	{
		uint8_t* targetRow = target + ty * targetStrideElements;

		position.y() = targetToSourceY * (Scalar(ty) + Scalar(0.5));

		for (unsigned int tx = 0; tx < targetWidth; ++tx)
		{
			position.x() = targetToSourceX * (Scalar(tx) + Scalar(0.5));

			interpolateFullBorder8BitPerChannel<tChannels>(*source, position, targetRow);
			targetRow += tChannels;
		}
	}
}

template <unsigned int tChannels>
inline void FrameInterpolatorTrilinear::homography8BitPerChannel(const FramePyramid& sourcePyramid, const unsigned int width, const unsigned int height, const SquareMatrix3& homography, const uint8_t* borderColor, uint8_t* target, Worker* worker)
{
	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameInterpolatorTrilinear::homography8BitPerChannelSubset<tChannels>, &sourcePyramid, width, height, &homography, borderColor, target, 0u, 0u), 0, height, 6u, 7u, 20u);
	}
	else
	{
		homography8BitPerChannelSubset<tChannels>(&sourcePyramid, width, height, &homography, borderColor, target, 0u, height);
	}
}

template <unsigned int tChannels>
void FrameInterpolatorTrilinear::homography8BitPerChannelSubset(const FramePyramid* sourcePyramid, const unsigned int width, const unsigned int height, const SquareMatrix3* homography, const uint8_t* borderColor, uint8_t* target, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(sourcePyramid && target);
	ocean_assert(width > 0u && height > 0u);
	ocean_assert(homography);

	ocean_assert(firstRow + numberRows <= height);

	const Scalar scalarWidth_1 = Scalar(width - 1u);
	const Scalar scalarHeight_1 = Scalar(height - 1u);

	typedef typename DataType<uint8_t, tChannels>::Type PixelType;

	PixelType zeroColor;
	memset(&zeroColor, 0x00, sizeof(PixelType));
	const PixelType* const bColor = borderColor ? (PixelType*)borderColor : &zeroColor;

	PixelType* targetData = (PixelType*)target + firstRow * width;

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		for (unsigned int x = 0; x < width; ++x)
		{
			const Vector2 centerPosition = Vector2(Scalar(x + 0.5), Scalar(y + 0.5));
			const Vector2 cornerPosition1 = centerPosition + Vector2(-0.5, -0.5);
			const Vector2 cornerPosition2 = centerPosition + Vector2(0.5, -0.5);
			const Vector2 cornerPosition3 = centerPosition + Vector2(-0.5, 0.5);
			const Vector2 cornerPosition4 = centerPosition + Vector2(0.5, 0.5);

			const Vector2 centerPositionHomography(*homography * centerPosition);
			const Vector2 cornerPosition1Homography(*homography * cornerPosition1);
			const Vector2 cornerPosition2Homography(*homography * cornerPosition2);
			const Vector2 cornerPosition3Homography(*homography * cornerPosition3);
			const Vector2 cornerPosition4Homography(*homography * cornerPosition4);

			const Vector3 pyramidPosition = interpolatePosition(sourcePyramid->layers(), centerPositionHomography, cornerPosition1Homography, cornerPosition2Homography, cornerPosition3Homography, cornerPosition4Homography);

			if (centerPositionHomography.x() < Scalar(0) || centerPositionHomography.x() > scalarWidth_1 || centerPositionHomography.y() < Scalar(0) || centerPositionHomography.y() > scalarHeight_1)
			{
				*targetData = *bColor;
			}
			else
			{
				interpolateFullBorder8BitPerChannel<tChannels>(*sourcePyramid, pyramidPosition, (uint8_t*)targetData);
			}

			targetData++;
		}
	}
}

}

}

#endif //OCEAN_CV_FRAME_INTERPOLATOR_TRILINEAR_H
