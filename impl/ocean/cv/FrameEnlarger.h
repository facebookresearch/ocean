/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_ENLARGER_H
#define META_OCEAN_CV_FRAME_ENLARGER_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameBlender.h"

#include "ocean/base/DataType.h"
#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements functions to enlarge/up-sample frames.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameEnlarger
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
				 * Adds a border to the given frame while all new pixels will receive a specified border color.
				 * @param source The source frame to be extended with a border, must be valid
				 * @param target The target frame receiving the extended frame, will be set to the correct pixel format and frame dimension
				 * @param borderSizeLeft The size of the border at the left image boundary, in pixel, with range [0, infinity)
				 * @param borderSizeTop The size of the border at the top image boundary, in pixel, with range [0, infinity)
				 * @param borderSizeRight The size of the border at the right image boundary, in pixel, with range [0, infinity)
				 * @param borderSizeBottom The size of the border at the bottom image boundary, in pixel, with range [0, infinity)
				 * @param color The border color to be used for the new border pixels, one value for each channel, must be valid
				 * @return True, if succeeded
				 */
				static bool addBorder(const Frame& source, Frame& target, const unsigned int borderSizeLeft, const unsigned int borderSizeTop, const unsigned int borderSizeRight, const unsigned int borderSizeBottom, const void* color);

				/**
				 * Adds a border to the given frame while all new pixels will receive a specified border color.
				 * @param frame The frame to be extended by a border, must be valid
				 * @param borderSizeLeft The size of the border at the left image boundary, in pixel, with range [0, infinity)
				 * @param borderSizeTop The size of the border at the top image boundary, in pixel, with range [0, infinity)
				 * @param borderSizeRight The size of the border at the right image boundary, in pixel, with range [0, infinity)
				 * @param borderSizeBottom The size of the border at the bottom image boundary, in pixel, with range [0, infinity)
				 * @param color The border color to be used for the new border pixels, one value for each channel, must be valid
				 * @return True, if succeeded
				 */
				static inline bool addBorder(Frame& frame, const unsigned int borderSizeLeft, const unsigned int borderSizeTop, const unsigned int borderSizeRight, const unsigned int borderSizeBottom, const void* color);

				/**
				 * Adds a border to the given frame.
				 * The color of the border pixels are defined by the nearest pixels of the original frame.
				 * @param source The source frame to be extended with a border, must be valid
				 * @param target The target frame receiving the extended frame, will be set to the correct pixel format and frame dimension
				 * @param borderSizeLeft The size of the border at the left image boundary, in pixel, with range [0, infinity)
				 * @param borderSizeTop The size of the border at the top image boundary, in pixel, with range [0, infinity)
				 * @param borderSizeRight The size of the border at the right image boundary, in pixel, with range [0, infinity)
				 * @param borderSizeBottom The size of the border at the bottom image boundary, in pixel, with range [0, infinity)
				 * @return True, if succeeded
				 */
				static bool addBorderNearestPixel(const Frame& source, Frame& target, const unsigned int borderSizeLeft, const unsigned int borderSizeTop, const unsigned int borderSizeRight, const unsigned int borderSizeBottom);

				/**
				 * Adds a border to the given frame.
				 * The color of the border pixels are defined by the nearest pixels of the original frame.
				 * @param frame The frame to be extended by a border, must be valid
				 * @param borderSizeLeft The size of the border at the left image boundary, in pixel, with range [0, infinity)
				 * @param borderSizeTop The size of the border at the top image boundary, in pixel, with range [0, infinity)
				 * @param borderSizeRight The size of the border at the right image boundary, in pixel, with range [0, infinity)
				 * @param borderSizeBottom The size of the border at the bottom image boundary, in pixel, with range [0, infinity)
				 * @return True, if succeeded
				 */
				static inline bool addBorderNearestPixel(Frame& frame, const unsigned int borderSizeLeft, const unsigned int borderSizeTop, const unsigned int borderSizeRight, const unsigned int borderSizeBottom);

				/**
				 * Adds a border to the given frame by mirroring the frame border (and border neighbor) pixels.
				 * @param source The source frame to be extended with a border, must be valid
				 * @param target The target frame receiving the extended frame, will be set to the correct pixel format and frame dimension
				 * @param borderSizeLeft The size of the border at the left image boundary, in pixel, with range [0, frame.width()]
				 * @param borderSizeTop The size of the border at the top image boundary, in pixel, with range [0, frame.height()]
				 * @param borderSizeRight The size of the border at the right image boundary, in pixel, with range [0, frame.width()]
				 * @param borderSizeBottom The size of the border at the bottom image boundary, in pixel, with range [0, frame.height()]
				 * @return True, if succeeded
				 */
				static bool addBorderMirrored(const Frame& source, Frame& target, const unsigned int borderSizeLeft, const unsigned int borderSizeTop, const unsigned int borderSizeRight, const unsigned int borderSizeBottom);

				/**
				 * Adds a border to the given frame by mirroring the frame border (and border neighbor) pixels.
				 * @param frame The frame to be extended by a border, must be valid
				 * @param borderSizeLeft The size of the border at the left image boundary, in pixel, with range [0, frame.width()]
				 * @param borderSizeTop The size of the border at the top image boundary, in pixel, with range [0, frame.height()]
				 * @param borderSizeRight The size of the border at the right image boundary, in pixel, with range [0, frame.width()]
				 * @param borderSizeBottom The size of the border at the bottom image boundary, in pixel, with range [0, frame.height()]
				 * @return True, if succeeded
				 */
				static inline bool addBorderMirrored(Frame& frame, const unsigned int borderSizeLeft, const unsigned int borderSizeTop, const unsigned int borderSizeRight, const unsigned int borderSizeBottom);

				/**
				 * Doubles the size of a given frame by a pixel repeating upsampling.
				 * @param source The source (and untouched) frame to be upsized, must be valid
				 * @param target The target frame receiving the upsized frame, will be set to the correct pixel format and frame dimension
				 * @param worker Optional worker object to distribute the computation
				 * @return True, if succeeded
				 */
				static bool multiplyByTwo(const Frame& source, Frame& target, Worker* worker = nullptr);

				/**
				 * Adds a transparent border to a given frame.
				 * The color of the transparent border pixels are taken from the nearest image pixel.<br>
				 * @param source The source frame which will be extended with a transparent border
				 * @param target Resulting target frame with transparent border
				 * @param leftBorder Size of the left frame border in pixel, with range [0, infinity)
				 * @param topBorder Size of the top frame border in pixel, with range [0, infinity)
				 * @param rightBorder Size of the right frame border in pixel, with range [0, infinity)
				 * @param bottomBorder Size of the bottom frame border in pixel, with range [0, infinity)
				 * @return True, if succeeded
				 * @tparam tTransparentIs0xFF True, if 0xFF is interpreted as fully transparent
				 */
				template <bool tTransparentIs0xFF>
				static bool addTransparentBorder(const Frame& source, Frame& target, const unsigned int leftBorder, const unsigned int topBorder, const unsigned int rightBorder, const unsigned int bottomBorder);

				/**
				 * Adds a transparent border to a given frame.
				 * The color of the transparent border pixels are taken from the nearest image pixel.<br>
				 * @param frame The frame which will be extended with a transparent border
				 * @param leftBorder Size of the left frame border in pixel, with range [0, infinity)
				 * @param topBorder Size of the top frame border in pixel, with range [0, infinity)
				 * @param rightBorder Size of the right frame border in pixel, with range [0, infinity)
				 * @param bottomBorder Size of the bottom frame border in pixel, with range [0, infinity)
				 * @return True, if succeeded
				 * @tparam tTransparentIs0xFF True, if 0xFF is interpreted as fully transparent
				 */
				template <bool tTransparentIs0xFF>
				static bool addTransparentBorder(Frame& frame, const unsigned int leftBorder, const unsigned int topBorder, const unsigned int rightBorder, const unsigned int bottomBorder);
		};

		/**
		 * Adds a transparent border to a given frame.
		 * The color of the transparent border pixels are taken from the nearest image pixel.<br>
		 * @param source The source frame which will be extended with a transparent border
		 * @param target Resulting target frame with transparent border
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param leftBorder Size of the left frame border in pixel, with range [1, infinity)
		 * @param topBorder Size of the top frame border in pixel, with range [1, infinity)
		 * @param rightBorder Size of the right frame border in pixel, with range [1, infinity)
		 * @param bottomBorder Size of the bottom frame border in pixel, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @tparam tChannelsWithAlpha Number of channels in the source frame (including the alpha channel)
		 * @tparam tAlphaAtFront True, if the alpha channel is in the front of the data channels
		 * @tparam tSourceHasAlpha True, if not only the source frame holds an alpha channel but also the target frame, however values in a target alpha channel will be untouched
		 * @tparam tTransparentIs0xFF True, if 0xFF is interpreted as fully transparent
		 */
		template <unsigned int tChannelsWithAlpha, bool tAlphaAtFront, bool tSourceHasAlpha, bool tTransparentIs0xFF>
		static void addTransparentBorder8BitPerChannel(const unsigned char* source, unsigned char* target, const unsigned int width, const unsigned int height, const unsigned int leftBorder, const unsigned int topBorder, const unsigned int rightBorder, const unsigned int bottomBorder, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements);

		/**
		 * Adds a border to a given frame while all new pixels will receive a specified border color/value.
		 * @param source The source image to be extended, must be valid
		 * @param target The target image receiving the extended frame, with resolution (sourceWidth + borderSizeLeft + borderSizeRight)x(sourceHeight + borderSizeTop + borderSizeBottom), must be valid
		 * @param sourceWidth Width of the original source image in pixel, with range [1, infinity)
		 * @param sourceHeight Height of the original source image in pixel, with range [1, infinity)
		 * @param borderSizeLeft The size of the border at the left image boundary, in pixel, with range [0, infinity)
		 * @param borderSizeTop The size of the border at the top image boundary, in pixel, with range [0, infinity)
		 * @param borderSizeRight The size of the border at the right image boundary, in pixel, with range [0, infinity)
		 * @param borderSizeBottom The size of the border at the bottom image boundary, in pixel, with range [0, infinity)
		 * @param color Border color/value to be used for the new border pixels, must be tChannels values
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @tparam T The data type of a pixel channel
		 * @tparam tChannels The number of data channels, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static void addBorder(const T* source, T* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int borderSizeLeft, const unsigned int borderSizeTop, const unsigned int borderSizeRight, const unsigned int borderSizeBottom, const T* color, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements);

		/**
		 * Adds a border to a given frame while the color of the border pixels are defined by the nearest pixels of the original frame.
		 * @param source The source image to be extended, must be valid
		 * @param target The target image receiving the extended frame, with resolution (sourceWidth + borderSizeLeft + borderSizeRight)x(sourceHeight + borderSizeTop + borderSizeBottom), must be valid
		 * @param sourceWidth Width of the original source image in pixel, with range [1, infinity)
		 * @param sourceHeight Height of the original source image in pixel, with range [1, infinity)
		 * @param borderSizeLeft The size of the border at the left image boundary, in pixel, with range [0, infinity)
		 * @param borderSizeTop The size of the border at the top image boundary, in pixel, with range [0, infinity)
		 * @param borderSizeRight The size of the border at the right image boundary, in pixel, with range [0, infinity)
		 * @param borderSizeBottom The size of the border at the bottom image boundary, in pixel, with range [0, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @tparam T The data type of a pixel channel
		 * @tparam tChannels The number of data channels, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static void addBorderNearestPixel(const T* source, T* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int borderSizeLeft, const unsigned int borderSizeTop, const unsigned int borderSizeRight, const unsigned int borderSizeBottom, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements);

		/**
		 * Adds a border to a given frame by mirroring the frame's content.
		 * @param source The source image to be extended, must be valid
		 * @param target The target image receiving the extended frame, with resolution (sourceWidth + borderSizeLeft + borderSizeRight)x(sourceHeight + borderSizeTop + borderSizeBottom), must be valid
		 * @param sourceWidth Width of the original source image in pixel, with range [1, infinity)
		 * @param sourceHeight Height of the original source image in pixel, with range [1, infinity)
		 * @param borderSizeLeft The size of the border at the left image boundary, in pixel, with range [0, sourceWidth]
		 * @param borderSizeTop The size of the border at the top image boundary, in pixel, with range [0, sourceHeight]
		 * @param borderSizeRight The size of the border at the right image boundary, in pixel, with range [0, sourceWidth]
		 * @param borderSizeBottom The size of the border at the bottom image boundary, in pixel, with range [0, sourceHeight]
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @tparam T The data type of a pixel channel
		 * @tparam tChannels The number of data channels, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static void addBorderMirrored(const T* source, T* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int borderSizeLeft, const unsigned int borderSizeTop, const unsigned int borderSizeRight, const unsigned int borderSizeBottom, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements);

		/**
		 * Doubles the size of a given frame by a pixel repeating upsampling.
		 * @param source Buffer of the source frame to be upsampled
		 * @param target Buffer of the target frame receiving the upsampled frame
		 * @param targetWidth Width of the target buffer in pixel, with range [2, infinity) and targetWidth / 2 == sourceWidth
		 * @param targetHeight Height of the target buffer in pixel, with range [2, infinity) and targetHeight / 2 == sourceHeight
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam T The data type of a pixel channel
		 * @tparam tChannels The number of data channels, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static void multiplyByTwo(const T* source, T* target, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

	private:

		/**
		 * Doubles a subset of a given frame by a pixel repeating upsampling.
		 * @param source Buffer of the source frame to be upsampled
		 * @param target Buffer of the target frame receiving the upsampled frame
		 * @param targetWidth Width of the target buffer in pixel, with range [2, infinity) and targetWidth / 2 == sourceWidth
		 * @param targetHeight Height of the target buffer in pixel, with range [2, infinity) and targetHeight / 2 == sourceHeight
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param firstTargetRow First target row to be handled, with range [0, targetHeight)
		 * @param numberTargetRows Number of target rows to be handled, with range [1, targetHeight]
		 * @tparam T The data type of a pixel channel
		 * @tparam tChannels The number of data channels, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static void multiplyByTwoSubset(const T* source, T* target, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstTargetRow, const unsigned int numberTargetRows);
};

inline bool FrameEnlarger::Comfort::addBorder(Frame& frame, const unsigned int borderSizeLeft, const unsigned int borderSizeTop, const unsigned int borderSizeRight, const unsigned int borderSizeBottom, const void* color)
{
	ocean_assert(frame.isValid());
	ocean_assert(color != nullptr);

	Frame tmpFrame;
	if (!addBorder(frame, tmpFrame, borderSizeLeft, borderSizeTop, borderSizeRight, borderSizeBottom, color))
	{
		return false;
	}

	frame = std::move(tmpFrame);
	return true;
}

bool FrameEnlarger::Comfort::addBorderNearestPixel(Frame& frame, const unsigned int borderSizeLeft, const unsigned int borderSizeTop, const unsigned int borderSizeRight, const unsigned int borderSizeBottom)
{
	ocean_assert(frame.isValid());

	Frame tmpFrame;
	if (!addBorderNearestPixel(frame, tmpFrame, borderSizeLeft, borderSizeTop, borderSizeRight, borderSizeBottom))
	{
		return false;
	}

	frame = std::move(tmpFrame);
	return true;
}

bool FrameEnlarger::Comfort::addBorderMirrored(Frame& frame, const unsigned int borderSizeLeft, const unsigned int borderSizeTop, const unsigned int borderSizeRight, const unsigned int borderSizeBottom)
{
	ocean_assert(frame.isValid());

	Frame tmpFrame;
	if (!addBorderMirrored(frame, tmpFrame, borderSizeLeft, borderSizeTop, borderSizeRight, borderSizeBottom))
	{
		return false;
	}

	frame = std::move(tmpFrame);
	return true;
}


template <typename T, unsigned int tChannels>
void FrameEnlarger::addBorder(const T* source, T* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int borderSizeLeft, const unsigned int borderSizeTop, const unsigned int borderSizeRight, const unsigned int borderSizeBottom, const T* color, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(sourceWidth != 0u && sourceHeight != 0u);

	typedef typename DataType<T, tChannels>::Type PixelType;
	static_assert(sizeof(PixelType) == sizeof(T) * tChannels, "Invalid pixel type!");

	const unsigned int targetWidth = sourceWidth + borderSizeLeft + borderSizeRight;

	const unsigned int sourceStrideElements = sourceWidth * tChannels + sourcePaddingElements;
	const unsigned int targetStrideElements = targetWidth * tChannels + targetPaddingElements;

	const PixelType borderColorPixel = *((const PixelType*)color);

	if (borderSizeTop != 0u)
	{
		// top border rows

		// first row
		for (unsigned int n = 0u; n < targetWidth; ++n)
		{
			((PixelType*)target)[n] = borderColorPixel;
		}

		// following top borders
		for (unsigned int y = 1u; y < borderSizeTop; ++y)
		{
			memcpy(target + y * targetStrideElements, target, targetWidth * sizeof(PixelType));
		}
	}

	for (unsigned int y = 0u; y < sourceHeight; ++y)
	{
		const PixelType* sourcePixel = (const PixelType*)(source + y * sourceStrideElements);
		PixelType* targetPixel = (PixelType*)(target + (borderSizeTop + y) * targetStrideElements);

		if (borderSizeTop == 0u)
		{
			for (unsigned int n = 0u; n < borderSizeLeft; ++n)
			{
				targetPixel[n] = borderColorPixel;
			}
		}
		else
		{
			// left
			memcpy(targetPixel, target, borderSizeLeft * sizeof(PixelType));
		}

		targetPixel += borderSizeLeft;

		// center
		memcpy(targetPixel, sourcePixel, sourceWidth * sizeof(PixelType));

		targetPixel += sourceWidth;
		sourcePixel += sourceWidth;

		if (borderSizeTop == 0u)
		{
			for (unsigned int n = 0u; n < borderSizeRight; ++n)
			{
				targetPixel[n] = borderColorPixel;
			}
		}
		else
		{
			// right
			memcpy(targetPixel, target, borderSizeRight * sizeof(PixelType));
		}
	}

	if (borderSizeBottom != 0u)
	{
		PixelType* targetPixel = (PixelType*)(target + (borderSizeTop + sourceHeight) * targetStrideElements);

		// first row
		for (unsigned int n = 0u; n < targetWidth; ++n)
		{
			targetPixel[n] = borderColorPixel;
		}

		// following top borders
		for (unsigned int y = 1u; y < borderSizeBottom; ++y)
		{
			memcpy(target + (borderSizeTop + sourceHeight + y) * targetStrideElements, targetPixel, targetWidth * sizeof(PixelType));
		}
	}
}

template <typename T, unsigned int tChannels>
void FrameEnlarger::addBorderNearestPixel(const T* source, T* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int borderSizeLeft, const unsigned int borderSizeTop, const unsigned int borderSizeRight, const unsigned int borderSizeBottom, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(sourceWidth != 0u && sourceHeight != 0u);

	typedef typename DataType<T, tChannels>::Type PixelType;
	static_assert(sizeof(PixelType) == sizeof(T) * tChannels, "Invalid pixel type!");

	const unsigned int targetWidth = sourceWidth + borderSizeLeft + borderSizeRight;

	const unsigned int sourceStrideElements = sourceWidth * tChannels + sourcePaddingElements;
	const unsigned int targetStrideElements = targetWidth * tChannels + targetPaddingElements;

	const PixelType* sourcePixel = (const PixelType*)source;
	PixelType* targetPixel = (PixelType*)target;

	if (borderSizeTop != 0u)
	{
		// top border rows
		for (unsigned int x = 0u; x < borderSizeLeft; ++x)
		{
			targetPixel[x] = *sourcePixel;
		}

		targetPixel += borderSizeLeft;

		memcpy(targetPixel, sourcePixel, sourceWidth * sizeof(PixelType));
		targetPixel += sourceWidth;

		for (unsigned int x = 0u; x < borderSizeRight; ++x)
		{
			targetPixel[x] = sourcePixel[sourceWidth - 1u];
		}

		targetPixel += borderSizeRight;

		// following top borders
		for (unsigned int y = 1u; y < borderSizeTop; ++y)
		{
			memcpy(target + y * targetStrideElements, target, targetWidth * sizeof(PixelType));
		}
	}

	// middle rows
	for (unsigned int y = 0u; y < sourceHeight; ++y)
	{
		sourcePixel = (const PixelType*)(source + y * sourceStrideElements);
		targetPixel = (PixelType*)(target + (borderSizeTop + y) * targetStrideElements);

		// left border columns
		for (unsigned int x = 0u; x < borderSizeLeft; ++x)
		{
			targetPixel[x] = *sourcePixel;
		}

		targetPixel += borderSizeLeft;

		memcpy(targetPixel, sourcePixel, sourceWidth * sizeof(PixelType));

		targetPixel += sourceWidth;
		sourcePixel += sourceWidth - 1u;

		// right border columns
		for (unsigned int x = 0u; x < borderSizeRight; ++x)
		{
			targetPixel[x] = *sourcePixel;
		}
	}

	if (borderSizeBottom != 0u)
	{
		sourcePixel = (const PixelType*)(source + (sourceHeight - 1u) * sourceStrideElements);
		targetPixel = (PixelType*)(target + (borderSizeTop + sourceHeight) * targetStrideElements);

		// first bottom row
		for (unsigned int x = 0u; x < borderSizeLeft; ++x)
		{
			targetPixel[x] = *sourcePixel;
		}

		targetPixel += borderSizeLeft;

		memcpy(targetPixel, sourcePixel, sourceWidth * sizeof(PixelType));
		targetPixel += sourceWidth;

		for (unsigned int x = 0u; x < borderSizeRight; ++x)
		{
			targetPixel[x] = sourcePixel[sourceWidth - 1u];
		}

		// following top borders
		const unsigned int targetHeight = sourceHeight + borderSizeTop + borderSizeBottom;

		targetPixel = (PixelType*)(target + (targetHeight - borderSizeBottom) * targetStrideElements);

		for (unsigned int y = targetHeight - borderSizeBottom + 1u; y < targetHeight; ++y)
		{
			memcpy(target + y * targetStrideElements,targetPixel, targetWidth * sizeof(PixelType));
		}
	}
}

template <typename T, unsigned int tChannels>
void FrameEnlarger::addBorderMirrored(const T* source, T* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int borderSizeLeft, const unsigned int borderSizeTop, const unsigned int borderSizeRight, const unsigned int borderSizeBottom, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(sourceWidth != 0u && sourceHeight != 0u);
	ocean_assert(borderSizeLeft <= sourceWidth && borderSizeRight <= sourceWidth);
	ocean_assert(borderSizeTop <= sourceHeight && borderSizeBottom <= sourceHeight);

	typedef typename DataType<T, tChannels>::Type PixelType;
	static_assert(sizeof(PixelType) == sizeof(T) * tChannels, "Invalid pixel type!");

	const unsigned int targetWidth = sourceWidth + borderSizeLeft + borderSizeRight;

	const unsigned int sourceStrideElements = sourceWidth * tChannels + sourcePaddingElements;
	const unsigned int targetStrideElements = targetWidth * tChannels + targetPaddingElements;

	// top rows

	for (unsigned int y = 0u; y < borderSizeTop; ++y)
	{
		const PixelType* sourcePixel = (const PixelType*)(source + (borderSizeTop - y - 1u) * sourceStrideElements);
		PixelType* targetPixel = (PixelType*)(target + y * targetStrideElements);

		// left
		for (unsigned int n = 0u; n < borderSizeLeft; ++n)
		{
			targetPixel[n] = *(sourcePixel + borderSizeLeft - n - 1u);
		}

		targetPixel += borderSizeLeft;

		// middle
		memcpy(targetPixel, sourcePixel, sourceWidth * sizeof(PixelType));

		targetPixel += sourceWidth;
		sourcePixel += sourceWidth;

		// right
		for (unsigned int n = 0u; n < borderSizeRight; ++n)
		{
			targetPixel[n] = *(sourcePixel - n - 1u);
		}
	}

	// middle rows

	for (unsigned int y = 0u; y < sourceHeight; ++y)
	{
		const PixelType* sourcePixel = (const PixelType*)(source + y * sourceStrideElements);
		PixelType* targetPixel = (PixelType*)(target + (borderSizeTop + y) * targetStrideElements);

		// left
		for (unsigned int n = 0u; n < borderSizeLeft; ++n)
		{
			targetPixel[n] = *(sourcePixel + borderSizeLeft - n - 1u);
		}

		targetPixel += borderSizeLeft;

		// middle
		memcpy(targetPixel, sourcePixel, sourceWidth * sizeof(PixelType));

		targetPixel += sourceWidth;
		sourcePixel += sourceWidth;

		// right
		for (unsigned int n = 0u; n < borderSizeRight; ++n)
		{
			targetPixel[n] = *(sourcePixel - n - 1u);
		}
	}

	for (unsigned int y = 0u; y < borderSizeBottom; ++y)
	{
		const PixelType* sourcePixel = (const PixelType*)(source + (sourceHeight - y - 1u) * sourceStrideElements);
		PixelType* targetPixel = (PixelType*)(target + (borderSizeTop + sourceHeight + y) * targetStrideElements);

		// left
		for (unsigned int n = 0u; n < borderSizeLeft; ++n)
		{
			targetPixel[n] = *(sourcePixel + borderSizeLeft - n - 1u);
		}

		targetPixel += borderSizeLeft;

		// middle
		memcpy(targetPixel, sourcePixel, sourceWidth * sizeof(PixelType));

		targetPixel += sourceWidth;
		sourcePixel += sourceWidth;

		// right
		for (unsigned int n = 0u; n < borderSizeRight; ++n)
		{
			targetPixel[n] = *(sourcePixel - n - 1u);
		}
	}
}

template <bool tTransparentIs0xFF>
bool FrameEnlarger::Comfort::addTransparentBorder(const Frame& source, Frame& target, const unsigned int leftBorder, const unsigned int topBorder, const unsigned int rightBorder, const unsigned int bottomBorder)
{
	ocean_assert(source);

	if (leftBorder == 0u && topBorder == 0u && rightBorder == 0u && bottomBorder == 0u)
	{
		target = source;
		return true;
	}

	if (source.numberPlanes() == 1u)
	{
		FrameType::PixelFormat targetPixelFormat = FrameType::FORMAT_UNDEFINED;

		const bool sourceHasAlpha = FrameType::formatHasAlphaChannel(source.pixelFormat());

		if (sourceHasAlpha)
		{
			targetPixelFormat = source.pixelFormat();
		}
		else if (FrameType::formatHasAlphaChannel(target.pixelFormat()) && FrameType::formatRemoveAlphaChannel(target.pixelFormat()) == source.pixelFormat())
		{
			targetPixelFormat = target.pixelFormat();
		}
		else
		{
			targetPixelFormat = FrameType::formatAddAlphaChannel(source.pixelFormat());
		}

		if (!target.set(FrameType(source.width() + leftBorder + rightBorder, source.height() + topBorder + bottomBorder, targetPixelFormat, source.pixelOrigin()), false /*forceOwner*/, true /*forceWritable*/))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		ocean_assert(target && FrameType::formatHasAlphaChannel(target.pixelFormat()));

		if (!target)
		{
			return false;
		}

		bool alphaAtBack = false;
		const bool tmpResult = FrameType::formatHasAlphaChannel(target.pixelFormat(), &alphaAtBack);
		ocean_assert_and_suppress_unused(tmpResult, tmpResult);

		switch (target.channels())
		{
			case 2u:
			{
				if (alphaAtBack)
				{
					if (sourceHasAlpha)
					{
						addTransparentBorder8BitPerChannel<2u, false, true, tTransparentIs0xFF>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), leftBorder, topBorder, rightBorder, bottomBorder, source.paddingElements(), target.paddingElements());
					}
					else
					{
						addTransparentBorder8BitPerChannel<2u, false, false, tTransparentIs0xFF>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), leftBorder, topBorder, rightBorder, bottomBorder, source.paddingElements(), target.paddingElements());
					}
				}
				else
				{
					if (sourceHasAlpha)
					{
						addTransparentBorder8BitPerChannel<2u, true, true, tTransparentIs0xFF>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), leftBorder, topBorder, rightBorder, bottomBorder, source.paddingElements(), target.paddingElements());
					}
					else
					{
						addTransparentBorder8BitPerChannel<2u, true, false, tTransparentIs0xFF>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), leftBorder, topBorder, rightBorder, bottomBorder, source.paddingElements(), target.paddingElements());
					}
				}

				return true;
			}

			case 3u:
			{
				if (alphaAtBack)
				{
					if (sourceHasAlpha)
					{
						addTransparentBorder8BitPerChannel<3u, false, true, tTransparentIs0xFF>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), leftBorder, topBorder, rightBorder, bottomBorder, source.paddingElements(), target.paddingElements());
					}
					else
					{
						addTransparentBorder8BitPerChannel<3u, false, false, tTransparentIs0xFF>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), leftBorder, topBorder, rightBorder, bottomBorder, source.paddingElements(), target.paddingElements());
					}
				}
				else
				{
					if (sourceHasAlpha)
					{
						addTransparentBorder8BitPerChannel<3u, true, true, tTransparentIs0xFF>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), leftBorder, topBorder, rightBorder, bottomBorder, source.paddingElements(), target.paddingElements());
					}
					else
					{
						addTransparentBorder8BitPerChannel<3u, true, false, tTransparentIs0xFF>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), leftBorder, topBorder, rightBorder, bottomBorder, source.paddingElements(), target.paddingElements());
					}
				}

				return true;
			}

			case 4u:
			{
				if (alphaAtBack)
				{
					if (sourceHasAlpha)
					{
						addTransparentBorder8BitPerChannel<4u, false, true, tTransparentIs0xFF>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), leftBorder, topBorder, rightBorder, bottomBorder, source.paddingElements(), target.paddingElements());
					}
					else
					{
						addTransparentBorder8BitPerChannel<4u, false, false, tTransparentIs0xFF>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), leftBorder, topBorder, rightBorder, bottomBorder, source.paddingElements(), target.paddingElements());
					}
				}
				else
				{
					if (sourceHasAlpha)
					{
						addTransparentBorder8BitPerChannel<4u, true, true, tTransparentIs0xFF>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), leftBorder, topBorder, rightBorder, bottomBorder, source.paddingElements(), target.paddingElements());
					}
					else
					{
						addTransparentBorder8BitPerChannel<4u, true, false, tTransparentIs0xFF>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), leftBorder, topBorder, rightBorder, bottomBorder, source.paddingElements(), target.paddingElements());
					}
				}

				return true;
			}

			default:
				ocean_assert(false && "Missing implementation");
				break;
		}
	}

	return false;
}

template <bool tTransparentIs0xFF>
bool FrameEnlarger::Comfort::addTransparentBorder(Frame& frame, const unsigned int leftBorder, const unsigned int topBorder, const unsigned int rightBorder, const unsigned int bottomBorder)
{
	Frame tmpFrame;

	if (!addTransparentBorder<tTransparentIs0xFF>(frame, tmpFrame, leftBorder, topBorder, rightBorder, bottomBorder))
	{
		return false;
	}

	frame = std::move(tmpFrame);
	return true;
}

template <unsigned int tChannelsWithAlpha, bool tAlphaAtFront, bool tSourceHasAlpha, bool tTransparentIs0xFF>
void FrameEnlarger::addTransparentBorder8BitPerChannel(const unsigned char* source, unsigned char* target, const unsigned int width, const unsigned int height, const unsigned int leftBorder, const unsigned int topBorder, const unsigned int rightBorder, const unsigned int bottomBorder, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements)
{
	ocean_assert(source && target);

	typedef typename DataType<uint8_t, tChannelsWithAlpha - 1u>::Type TypeWithoutAlpha;

	if (topBorder != 0u)
	{
		// first row, left block: use the color values of the top left pixel of the source frame
		for (unsigned int x = 0u; x < leftBorder; ++x)
		{
			*(TypeWithoutAlpha*)(target + x * tChannelsWithAlpha + FrameBlender::SourceOffset<tAlphaAtFront>::data()) =
									*(TypeWithoutAlpha*)(source + FrameBlender::SourceOffset<tAlphaAtFront>::template data<tSourceHasAlpha>());

			*(target + x * tChannelsWithAlpha + FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannelsWithAlpha>()) = FrameBlender::fullTransparent8Bit<tTransparentIs0xFF>();
		}

		// first row, center block: use the color values of the top row of the source frame
		for (unsigned int x = 0u; x < width; ++x)
		{
			*(TypeWithoutAlpha*)(target + (x + leftBorder) * tChannelsWithAlpha + FrameBlender::SourceOffset<tAlphaAtFront>::data()) =
									*(TypeWithoutAlpha*)(source + x * FrameBlender::FrameChannels<tSourceHasAlpha>::template channels<tChannelsWithAlpha>() +  FrameBlender::SourceOffset<tAlphaAtFront>::template data<tSourceHasAlpha>());

			*(target + (x + leftBorder) * tChannelsWithAlpha + FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannelsWithAlpha>()) = FrameBlender::fullTransparent8Bit<tTransparentIs0xFF>();
		}

		// first row, right block: use the color values of the top right pixel of the source frame
		for (unsigned int x = 0u; x < rightBorder; ++x)
		{
			*(TypeWithoutAlpha*)(target + (x + leftBorder + width) * tChannelsWithAlpha + FrameBlender::SourceOffset<tAlphaAtFront>::data()) =
									*(TypeWithoutAlpha*)(source + (width - 1u) * FrameBlender::FrameChannels<tSourceHasAlpha>::template channels<tChannelsWithAlpha>() + FrameBlender::SourceOffset<tAlphaAtFront>::template data<tSourceHasAlpha>());

			*(target + (x + leftBorder + width) * tChannelsWithAlpha + FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannelsWithAlpha>()) = FrameBlender::fullTransparent8Bit<tTransparentIs0xFF>();
		}

		// the remaining rows of the top border are copies of the first row
		for (unsigned int y = 1u; y < topBorder; ++y)
		{
			memcpy(target + y * (width + leftBorder + rightBorder) * tChannelsWithAlpha + targetPaddingElements, target, (width + leftBorder + rightBorder) * tChannelsWithAlpha);
		}

		// next row
		target += topBorder * (tChannelsWithAlpha * (width + leftBorder + rightBorder) + targetPaddingElements);
	}


	for (unsigned int y = 0u; y < height; ++y)
	{
		// left border
		for (unsigned int x = 0u; x < leftBorder; ++x)
		{
			*(TypeWithoutAlpha*)(target + x * tChannelsWithAlpha + FrameBlender::SourceOffset<tAlphaAtFront>::data()) =
								*(TypeWithoutAlpha*)(source + FrameBlender::SourceOffset<tAlphaAtFront>::template data<tSourceHasAlpha>());

			*(target + x * tChannelsWithAlpha + FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannelsWithAlpha>()) = FrameBlender::fullTransparent8Bit<tTransparentIs0xFF>();
		}

		// center block
		if constexpr (tSourceHasAlpha)
		{
			memcpy(target + leftBorder * tChannelsWithAlpha, source, width * tChannelsWithAlpha);
		}
		else
		{
			for (unsigned int x = 0u; x < width; ++x)
			{
				*(TypeWithoutAlpha*)(target + (x + leftBorder) * tChannelsWithAlpha + FrameBlender::SourceOffset<tAlphaAtFront>::data()) =
								*(TypeWithoutAlpha*)(source + x * FrameBlender::FrameChannels<tSourceHasAlpha>::template channels<tChannelsWithAlpha>() +  FrameBlender::SourceOffset<tAlphaAtFront>::template data<tSourceHasAlpha>());

				*(target + (x + leftBorder) * tChannelsWithAlpha + FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannelsWithAlpha>()) = FrameBlender::fullOpaque8Bit<tTransparentIs0xFF>();
			}
		}

		// right border
		for (unsigned int x = 0u; x < rightBorder; ++x)
		{
			*(TypeWithoutAlpha*)(target + (x + leftBorder + width) * tChannelsWithAlpha + FrameBlender::SourceOffset<tAlphaAtFront>::data()) =
									*(TypeWithoutAlpha*)(source + (width - 1u) * FrameBlender::FrameChannels<tSourceHasAlpha>::template channels<tChannelsWithAlpha>() + FrameBlender::SourceOffset<tAlphaAtFront>::template data<tSourceHasAlpha>());

			*(target + (x + leftBorder + width) * tChannelsWithAlpha + FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannelsWithAlpha>()) = FrameBlender::fullTransparent8Bit<tTransparentIs0xFF>();
		}

		// next row
		source += FrameBlender::FrameChannels<tSourceHasAlpha>::template channels<tChannelsWithAlpha>() * width + sourcePaddingElements;
		target += tChannelsWithAlpha * (width + leftBorder + rightBorder) + targetPaddingElements;
	}

	if (bottomBorder != 0u)
	{
		// we need the last row of the source frame
		source -= FrameBlender::FrameChannels<tSourceHasAlpha>::template channels<tChannelsWithAlpha>() * width + sourcePaddingElements;

		// first row of bottom border, left block: use the color values of the bottom left pixel of the source frame
		for (unsigned int x = 0u; x < leftBorder; ++x)
		{
			*(TypeWithoutAlpha*)(target + x * tChannelsWithAlpha + FrameBlender::SourceOffset<tAlphaAtFront>::data()) =
									*(TypeWithoutAlpha*)(source + FrameBlender::SourceOffset<tAlphaAtFront>::template data<tSourceHasAlpha>());

			*(target + x * tChannelsWithAlpha + FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannelsWithAlpha>()) = FrameBlender::fullTransparent8Bit<tTransparentIs0xFF>();
		}

		// first row of the bottom border, center block: use the color values of the top row of the source frame
		for (unsigned int x = 0u; x < width; ++x)
		{
			*(TypeWithoutAlpha*)(target + (x + leftBorder) * tChannelsWithAlpha + FrameBlender::SourceOffset<tAlphaAtFront>::data()) =
									*(TypeWithoutAlpha*)(source + x * FrameBlender::FrameChannels<tSourceHasAlpha>::template channels<tChannelsWithAlpha>() +  FrameBlender::SourceOffset<tAlphaAtFront>::template data<tSourceHasAlpha>());

			*(target + (x + leftBorder) * tChannelsWithAlpha + FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannelsWithAlpha>()) = FrameBlender::fullTransparent8Bit<tTransparentIs0xFF>();
		}

		// first row of the bottom border, right block: use the color values of the top right pixel of the source frame
		for (unsigned int x = 0u; x < rightBorder; ++x)
		{
			*(TypeWithoutAlpha*)(target + (x + leftBorder + width) * tChannelsWithAlpha + FrameBlender::SourceOffset<tAlphaAtFront>::data()) =
									*(TypeWithoutAlpha*)(source + (width - 1u) * FrameBlender::FrameChannels<tSourceHasAlpha>::template channels<tChannelsWithAlpha>() + FrameBlender::SourceOffset<tAlphaAtFront>::template data<tSourceHasAlpha>());

			*(target + (x + leftBorder + width) * tChannelsWithAlpha + FrameBlender::SourceOffset<tAlphaAtFront>::template alpha<tChannelsWithAlpha>()) = FrameBlender::fullTransparent8Bit<tTransparentIs0xFF>();
		}

		// the remaining rows of the bottom border are copies of the first row
		for (unsigned int y = 1u; y < bottomBorder; ++y)
		{
			memcpy(target + y * (width + leftBorder + rightBorder) * tChannelsWithAlpha + targetPaddingElements, target, (width + leftBorder + rightBorder) * tChannelsWithAlpha);
		}
	}
}

template <typename T, unsigned int tChannels>
void FrameEnlarger::multiplyByTwo(const T* source, T* target, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(targetWidth >= 2u && targetHeight >= 2u);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameEnlarger::multiplyByTwoSubset<T, tChannels>, source, target, targetWidth, targetHeight, sourcePaddingElements, targetPaddingElements, 0u, 0u), 0u, targetHeight);
	}
	else
	{
		multiplyByTwoSubset<T, tChannels>(source, target, targetWidth, targetHeight, sourcePaddingElements, targetPaddingElements, 0u, targetHeight);
	}
}

template <typename T, unsigned int tChannels>
void FrameEnlarger::multiplyByTwoSubset(const T* source, T* target, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstTargetRow, const unsigned int numberTargetRows)
{
	ocean_assert(targetWidth >= 2u && targetHeight >= 2u);

	ocean_assert(source && target);
	ocean_assert(firstTargetRow + numberTargetRows <= targetHeight);

	const unsigned int sourceWidth = targetWidth / 2u;
	const unsigned int sourceHeight = targetHeight / 2u;
	ocean_assert(sourceWidth >= 1u && sourceHeight >= 1u);

	const unsigned int sourceStrideElements = sourceWidth * tChannels + sourcePaddingElements;
	const unsigned int targetStrideElements = targetWidth * tChannels + targetPaddingElements;

	typedef typename DataType<T, tChannels>::Type PixelType;

	// check whether no extra right column handling is necessary
	if (targetWidth % 2u == 0u)
	{
		for (unsigned int targetRowIndex = firstTargetRow; targetRowIndex < (firstTargetRow + numberTargetRows); ++targetRowIndex)
		{
			const PixelType* sourcePixel = (const PixelType*)(source + min(targetRowIndex / 2u, sourceHeight - 1u) * sourceStrideElements);

			PixelType* targetPixel = (PixelType*)(target + targetRowIndex * targetStrideElements);

			const PixelType* const targetRowEnd = targetPixel + targetWidth;

			while (targetPixel != targetRowEnd)
			{
				ocean_assert(targetPixel < targetRowEnd);

				*targetPixel++ = *sourcePixel;

				ocean_assert(targetPixel < targetRowEnd);

				*targetPixel++ = *sourcePixel++;
			}
		}
	}
	else
	{
		ocean_assert((targetWidth - 1u) % 2u == 0u);

		for (unsigned int targetRowIndex = firstTargetRow; targetRowIndex < (firstTargetRow + numberTargetRows); ++targetRowIndex)
		{
			const PixelType* sourcePixel = (const PixelType*)(source + min(targetRowIndex / 2u, sourceHeight - 1u) * sourcePaddingElements);
			--sourcePixel;

			PixelType* targetPixel = (PixelType*)(target + targetRowIndex * targetStrideElements);

			const PixelType* const targetRowEnd = targetPixel + targetWidth - 1u;

			while (targetPixel != targetRowEnd)
			{
				ocean_assert(targetPixel < targetRowEnd);

				*targetPixel++ = *++sourcePixel;

				ocean_assert(targetPixel < targetRowEnd);

				*targetPixel++ = *sourcePixel;
			}

			// special handling for the last column
			*targetPixel++ = *sourcePixel;
		}
	}
}

}

}

#endif // META_OCEAN_CV_FRAME_ENLARGER_H
