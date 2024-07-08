/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_INTERPOLATOR_BICUBIC_H
#define META_OCEAN_CV_FRAME_INTERPOLATOR_BICUBIC_H

#include "ocean/cv/CV.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements a bicubic frame interpolator.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameInterpolatorBicubic
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
			 * Resizes a given frame by a bicubic interpolation.
			 * The pixel format of the frame must be zipped with DT_UNSIGNED_INTEGER_8 as data type (e.g., FORMAT_Y8, FORMAT_RGB24, FORMAT_RGBA32, ...).
			 * @param frame The frame to resize, must be valid
			 * @param width The width of the resized frame in pixel, with range [1, infinity)
			 * @param height The height of the resized frame in pixel, with range [1, infinity)
			 * @param worker Optional worker object used for load distribution
			 * @return True, if the frame could be resized
			 */
			static inline bool resize(Frame& frame, const unsigned int width, const unsigned int height, Worker* worker = nullptr);

			/**
			 * Resizes a given frame by a bicubic interpolation.
			 * The pixel format of the frame must be zipped with DT_UNSIGNED_INTEGER_8 as data type (e.g., FORMAT_Y8, FORMAT_RGB24, FORMAT_RGBA32, ...).
			 * @param source The source frame to resize, must be valid
			 * @param target Resulting target frame with identical frame pixel format and pixel origin as the source frame, must be valid
			 * @param worker Optional worker object used for load distribution
			 * @return True, if the frame could be resized
			 */
			static inline bool resize(const Frame& source, Frame& target, Worker* worker = nullptr);
		};

		/**
		 * Resizes a given frame by a bicubic interpolation.
		 * @param source The source frame buffer to resize, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param sourceWidth Width of the source frame in pixel, with range [1, infinity)
		 * @param sourceHeight Height of the source frame in pixel, with range [1, infinity)
		 * @param targetWidth Width of the target frame in pixel, with range [1, infinity)
		 * @param targetHeight Height of the target frame in pixel, with range [1, infinity)
		 * @param format Pixel format of source and target frame
		 * @param sourcePaddingElements Number of padding elements used in the source frame, range: [0, infinity)
		 * @param targetPaddingElements Number of padding elements used in the target frame, range: [0, infinity)
		 * @param worker Optional worker object used for load distribution
		 * @return True, if the frame could be resized
		 */
		static bool resize(const uint8_t* source, uint8_t* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const FrameType::PixelFormat format, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Resizes a given zipped frame by a bicubic interpolation.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param sourceWidth Width of the source frame in pixel, with range [1, infinity)
		 * @param sourceHeight Height of the source frame in pixel, with range [1, infinity)
		 * @param targetWidth Width of the target frame in pixel, with range [1, infinity)
		 * @param targetHeight Height of the target frame in pixel, with range [1, infinity)
		 * @param sourcePaddingElements Number of padding elements used in the source frame, range: [0, infinity)
		 * @param targetPaddingElements Number of padding elements used in the target frame, range: [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannels Number of frame channels, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void resize8BitPerChannel(const uint8_t* source, uint8_t* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

	private:

		/**
		 * Resizes a subset of a given zipped frame by a horizontal bicubic interpolation only.
		 * @param source The source frame buffer to resize, with size (sourceWidth x sourceHeight)
		 * @param target Intermediate target frame buffer, with size (targetWidth x sourceHeight)
		 * @param sourceWidth Width of the source frame in pixel, with range [1, infinity)
		 * @param targetWidth Width of the target frame in pixel, with range [1, infinity)
		 * @param height The height of the source and target frame in pixel, with range [1, infinity)
		 * @param sourcePaddingElements Number of padding elements used in the source frame, range: [0, infinity)
		 * @param targetPaddingElements Number of padding elements used in the target frame, range: [0, infinity)
		 * @param firstRow First (including) row to convert, with range [0, height)
		 * @param numberRows Number of rows to convert, with range [1, height - firstRow]
		 * @tparam tChannels Number of frame channels, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void resizeHorizontal8BitPerChannelSubset(const uint8_t* source, uint8_t* target, const unsigned int sourceWidth, const unsigned int targetWidth, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Resizes a subset of a given zipped frame by a vertical bicubic interpolation only.
		 * @param source Resized and horizontal filtered source frame buffer, with size (targetWidth x sourceHeight)
		 * @param target The target frame buffer, with size (targetWidth x targetHeight)
		 * @param sourceHeight Height of the source frame in pixel, with range [1, infinity)
		 * @param targetHeight Height of the target frame in pixel, with range [1, infinity)
		 * @param width The width of the source and target frame in pixel, with range [1, infinity)
		 * @param sourcePaddingElements Number of padding elements used in the source frame, range: [0, infinity)
		 * @param targetPaddingElements Number of padding elements used in the target frame, range: [0, infinity)
		 * @param firstColumn First (including) column to convert, with range [0, width)
		 * @param numberColumns Number of columns to convert, with range [1, width - firstColumn]
		 * @tparam tChannels Number of frame channels, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void resizeVertical8BitPerChannelSubset(const uint8_t* source, uint8_t* target, const unsigned int sourceHeight, const unsigned int targetHeight, const unsigned int width, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstColumn, const unsigned int numberColumns);
};

inline bool FrameInterpolatorBicubic::Comfort::resize(Frame& frame, const unsigned int width, const unsigned int height, Worker* worker)
{
	ocean_assert(frame.isValid());
	ocean_assert(width >= 1u && height >= 1u);

	if (frame.dataType() != FrameType::DT_UNSIGNED_INTEGER_8)
	{
		// we need a generic pixel format with unsigned char data type e.g., FORMAT_Y8, FORAT_RGB24, etc.
		ocean_assert(false && "Invalid frame type!");
		return false;
	}

	if (width == frame.width() && height == frame.height())
	{
		return true;
	}

	Frame target(FrameType(frame, width, height));

	if (!Comfort::resize(frame, target, worker))
	{
		return false;
	}

	frame = std::move(target);
	return true;
}

inline bool FrameInterpolatorBicubic::Comfort::resize(const Frame& source, Frame& target, Worker* worker)
{
	ocean_assert(source && target);

	if (source.dataType() != FrameType::DT_UNSIGNED_INTEGER_8 || source.pixelFormat() != target.pixelFormat() || source.pixelOrigin() != target.pixelOrigin())
	{
		ocean_assert(false && "Invalid frame type!");
		return false;
	}

	return FrameInterpolatorBicubic::resize(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), target.width(), target.height(), source.pixelFormat(), source.paddingElements(), target.paddingElements(), worker);
}

template <unsigned int tChannels>
void FrameInterpolatorBicubic::resize8BitPerChannel(const uint8_t* source, uint8_t* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(sourceWidth != 0u && sourceHeight != 0u);
	ocean_assert(targetWidth != 0u && targetHeight != 0u);

	if (sourceWidth == targetWidth)
	{
		if (worker)
		{
			worker->executeFunction(Worker::Function::createStatic(&FrameInterpolatorBicubic::resizeVertical8BitPerChannelSubset<tChannels>, source, target, sourceHeight, targetHeight, targetWidth, sourcePaddingElements, targetPaddingElements, 0u, 0u), 0u, targetWidth);
		}
		else
		{
			resizeVertical8BitPerChannelSubset<tChannels>(source, target, sourceHeight, targetHeight, targetWidth, sourcePaddingElements, targetPaddingElements, 0u, targetWidth);
		}
	}
	else if (sourceHeight == targetHeight)
	{
		if (worker)
		{
			worker->executeFunction(Worker::Function::createStatic(&FrameInterpolatorBicubic::resizeHorizontal8BitPerChannelSubset<tChannels>, source, target, sourceWidth, targetWidth, targetHeight, sourcePaddingElements, targetPaddingElements, 0u, 0u), 0u, sourceHeight);
		}
		else
		{
			resizeHorizontal8BitPerChannelSubset<tChannels>(source, target, sourceWidth, targetWidth, targetHeight, sourcePaddingElements, targetPaddingElements, 0u, targetHeight);
		}
	}
	else
	{
		Frame intermediateFrame(FrameType(targetWidth, sourceHeight, FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, tChannels>(), FrameType::ORIGIN_UPPER_LEFT));

		if (worker)
		{
			worker->executeFunction(Worker::Function::createStatic(&FrameInterpolatorBicubic::resizeHorizontal8BitPerChannelSubset<tChannels>, source, intermediateFrame.data<uint8_t>(), sourceWidth, intermediateFrame.width(), sourceHeight, sourcePaddingElements, intermediateFrame.paddingElements(), 0u, 0u), 0u, sourceHeight);
			worker->executeFunction(Worker::Function::createStatic(&FrameInterpolatorBicubic::resizeVertical8BitPerChannelSubset<tChannels>, intermediateFrame.constdata<uint8_t>(), target, intermediateFrame.height(), targetHeight, targetWidth, intermediateFrame.paddingElements(), targetPaddingElements, 0u, 0u), 0u, targetWidth);
		}
		else
		{
			resizeHorizontal8BitPerChannelSubset<tChannels>(source, intermediateFrame.data<uint8_t>(), sourceWidth, intermediateFrame.width(), sourceHeight, sourcePaddingElements, intermediateFrame.paddingElements(), 0u, sourceHeight);
			resizeVertical8BitPerChannelSubset<tChannels>(intermediateFrame.constdata<uint8_t>(), target, intermediateFrame.height(), targetHeight, targetWidth, intermediateFrame.paddingElements(), targetPaddingElements, 0u, targetWidth);
		}
	}
}

template <unsigned int tChannels>
void FrameInterpolatorBicubic::resizeHorizontal8BitPerChannelSubset(const uint8_t* source, uint8_t* target, const unsigned int sourceWidth, const unsigned int targetWidth, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(sourceWidth != 0u && targetWidth != 0u);
	ocean_assert(height != 0u);

	ocean_assert_and_suppress_unused(firstRow + numberRows <= height, height);

	const float targetToSourceX = float(sourceWidth) / float(targetWidth);

	const unsigned int sourceStrideElements = sourceWidth * tChannels + sourcePaddingElements;
	const unsigned int targetStrideElements = targetWidth * tChannels + targetPaddingElements;

	const uint8_t* sourceRow = source + firstRow * sourceStrideElements;
	uint8_t* targetElement = target + firstRow * targetStrideElements;

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		for (unsigned int tx = 0; tx < targetWidth; ++tx)
		{
			const float sx = targetToSourceX * float(tx);

			ocean_assert(sx >= 0.0f && sx <= float(sourceWidth));

			const unsigned int sx1 = (unsigned int)(min(NumericF::floor(sx), float(sourceWidth - 1)));
			ocean_assert(int(sx1) >= 0);

			const unsigned int sx0 = (unsigned int)(max(0, int(sx1) - 1));
			const unsigned int sx2 = min(sx1 + 1u, sourceWidth - 1u);
			const unsigned int sx3 = min(sx2 + 1u, sourceWidth - 1u);

			ocean_assert(sx >= float(sx1));
			ocean_assert(sx1 >= sx0 && sx2 >= sx1 && sx3 >= sx2);

			const float d = sx - float(sx1);

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				const float color0 = sourceRow[sx0 * tChannels + n];
				const float color1 = sourceRow[sx1 * tChannels + n];
				const float color2 = sourceRow[sx2 * tChannels + n];
				const float color3 = sourceRow[sx3 * tChannels + n];

				float color = 0.166666666667f * color0 + 0.66666666667f * color1 + 0.166666666667f * color2;

				if (NumericF::isNotEqualEps(d))
				{
					color += (-0.166666666667f * color0 + 0.5f * color1 - 0.5f * color2 + 0.166666666667f * color3) * d * d * d
							+ (0.5f * color0 - 1.0f * color1 + 0.5f * color2) * d * d
							+ (-0.5f * color0 + 0.5f * color2) * d;
				}

				ocean_assert(NumericF::isInsideRange(0.0f, color, 256.0f));

				*targetElement++ = uint8_t(color);
			}
		}

		targetElement += targetPaddingElements;
		sourceRow += sourceStrideElements;
	}
}

template <unsigned int tChannels>
void FrameInterpolatorBicubic::resizeVertical8BitPerChannelSubset(const uint8_t* source, uint8_t* target, const unsigned int sourceHeight, const unsigned int targetHeight, const unsigned int width, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstColumn, const unsigned int numberColumns)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(sourceHeight != 0u && targetHeight != 0u);
	ocean_assert(width != 0u);

	ocean_assert(firstColumn + numberColumns <= width);

	const float targetToSourceY = float(sourceHeight) / float(targetHeight);

	const unsigned int sourceStrideElements = width * tChannels + sourcePaddingElements;
	const unsigned int targetStrideElements = width * tChannels + targetPaddingElements;

	uint8_t* targetColor = target + firstColumn * tChannels - 1u;

	for (unsigned int ty = 0u; ty < targetHeight; ++ty)
	{
		const float sy = targetToSourceY * float(ty);

		ocean_assert(sy >= 0.0f && sy <= float(sourceHeight));

		const unsigned int sy1 = (unsigned int)(min(NumericF::floor(sy), float(sourceHeight - 1u)));
		ocean_assert(int(sy1) >= 0);

		const unsigned int sy0 = (unsigned int)(max(0, int(sy1) - 1));
		const unsigned int sy2 = min(sy1 + 1u, sourceHeight - 1u);
		const unsigned int sy3 = min(sy2 + 1u, sourceHeight - 1u);

		ocean_assert(sy >= float(sy1));
		ocean_assert(sy1 >= sy0 && sy2 >= sy1 && sy3 >= sy2);

		const float d = sy - float(sy1);

		const uint8_t* color0 = source + sy0 * sourceStrideElements + tChannels * firstColumn;
		const uint8_t* color1 = source + sy1 * sourceStrideElements + tChannels * firstColumn;
		const uint8_t* color2 = source + sy2 * sourceStrideElements + tChannels * firstColumn;
		const uint8_t* color3 = source + sy3 * sourceStrideElements + tChannels * firstColumn;

		for (unsigned int tx = firstColumn; tx < firstColumn + numberColumns; ++tx)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				float color = 0.166666666667f * float(color0[n]) + 0.66666666667f * float(color1[n]) + 0.166666666667f * float(color2[n]);

				if (NumericF::isNotEqualEps(d))
				{
					color += (-0.166666666667f * float(color0[n]) + 0.5f * float(color1[n]) - 0.5f * float(color2[n]) + 0.166666666667f * float(color3[n])) * d * d * d
								+ (0.5f * float(color0[n]) - 1.0f * float(color1[n]) + 0.5f * float(color2[n])) * d * d
								+ (-0.5f * float(color0[n]) + 0.5f * float(color2[n])) * d;
				}

				ocean_assert(color >= 0.0f && color < 256.0f);
				*++targetColor = uint8_t(color);
			}

			color0 += tChannels;
			color1 += tChannels;
			color2 += tChannels;
			color3 += tChannels;
		}

		targetColor += targetStrideElements - numberColumns * tChannels;
	}
}

}

}

#endif // META_OCEAN_CV_FRAME_INTERPOLATOR_BICUBIC_H
