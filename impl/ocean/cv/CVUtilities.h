/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_CV_UTILITIES_H
#define META_OCEAN_CV_CV_UTILITIES_H

#include "ocean/cv/CV.h"

#include "ocean/base/DataType.h"
#include "ocean/base/Frame.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Worker.h"

#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Random.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements Computer Vision utility functions.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT CVUtilities
{
	public:

		/**
		 * This struct is a helper class allowing to determine the next smaller tracking patch for a given patch size.
		 * @tparam tSize The current size of the tracking patch for that the next smaller patch is requested, values might be [5; 7, 9, 15, 31 63]
		 */
		template <unsigned int tSize>
		struct SmallerPatchSize
		{
			// nothing to do here this class is empty
		};

	public:

		/**
		 * Returns all defined pixel formats.
		 * @param genericPixelFormats Optional custom generic pixel formats which will be added to the resulting pixel formats
		 * @return Ocean's defined pixel formats
		 */
		static FrameType::PixelFormats definedPixelFormats(const FrameType::PixelFormats& genericPixelFormats = FrameType::PixelFormats());

		/**
		 * Copies the pixel value from one pixel to another.
		 * @param target The target pixel, must be valid
		 * @param source The source pixel, must be valid
		 * @tparam T The data type of each pixel channel
		 * @tparam tChannels Specified the number of pixel channels, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static inline void copyPixel(T* const target, const T* const source);

		/**
		 * Copies the pixel value from one pixel to another.
		 * @param target The target pixel, must be valid
		 * @param source The source pixel, must be valid
		 * @tparam tChannels Specified the number of pixel channels, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static inline void copyPixel(uint8_t* const target, const uint8_t* const source);

		/**
		 * Copies the pixel value from one pixel to another.
		 * @param target The target image, must be valid
		 * @param source The source image, must be valid
		 * @param targetIndex Index of the pixel inside the target frame, the index is determined for a pixel (x, y) by (y * width + x), frame channels are handled inside, with range [0, width * height - 1]
		 * @param sourceIndex Index of the pixel inside the source frame, the index is determined for a pixel (x, y) by (y * width + x), frame channels are handled inside, with range [0, width * height - 1]
		 * @tparam T The data type of each pixel channel
		 * @tparam tChannels Specified the number of pixel channels, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static inline void copyPixel(T* const target, const T* const source, const unsigned int targetIndex, const unsigned int sourceIndex);

		/**
		 * Copies the pixel value from one pixel to another.
		 * @param target The target image, must be valid
		 * @param source The source image, must be valid
		 * @param targetIndex Index of the pixel inside the target frame, the index is determined for a pixel (x, y) by (y * width + x), frame channels are handled inside, with range [0, width * height - 1]
		 * @param sourceIndex Index of the pixel inside the source frame, the index is determined for a pixel (x, y) by (y * width + x), frame channels are handled inside, with range [0, width * height - 1]
		 * @tparam tChannels Specified the number of pixel channels, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static inline void copyPixel(uint8_t* const target, const uint8_t* const source, const unsigned int targetIndex, const unsigned int sourceIndex);

		/**
		 * Copies the pixel value from one pixel to another.
		 * @param target The target image, must be valid
		 * @param source The source image, must be valid
		 * @param xTarget Horizontal target position, in pixel, with range [0, targetWidth - 1]
		 * @param yTarget Vertical target position, in pixel, with range [0, targetHeight - 1]
		 * @param xSource Horizontal source position, in pixel, with range [0, sourceWidth - 1]
		 * @param ySource Vertical source position, in pixel, with range [0, sourceHeight - 1]
		 * @param targetWidth Width of the target frame, in pixel, with range [1, infinity)
		 * @param sourceWidth Width of the source frame, in pixel, with range [1, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @tparam T The data type of each pixel channel
		 * @tparam tChannels Specified the number of pixel channels, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static inline void copyPixel(T* const target, const T* const source, const unsigned int xTarget, const unsigned int yTarget, const unsigned int xSource, const unsigned int ySource, const unsigned int targetWidth, const unsigned int sourceWidth, const unsigned int targetPaddingElements, const unsigned int sourcePaddingElements);

		/**
		 * Copies the pixel value from one pixel to another.
		 * @param target The target image, must be valid
		 * @param source The source image, must be valid
		 * @param xTarget Horizontal target position, in pixel, with range [0, targetWidth - 1]
		 * @param yTarget Vertical target position, in pixel, with range [0, targetHeight - 1]
		 * @param xSource Horizontal source position, in pixel, with range [0, sourceWidth - 1]
		 * @param ySource Vertical source position, in pixel, with range [0, sourceHeight - 1]
		 * @param targetWidth Width of the target frame, in pixel, with range [1, infinity)
		 * @param sourceWidth Width of the source frame, in pixel, with range [1, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @tparam tChannels Specified the number of pixel channels, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static inline void copyPixel(uint8_t* const target, const uint8_t* const source, const unsigned int xTarget, const unsigned int yTarget, const unsigned int xSource, const unsigned int ySource, const unsigned int targetWidth, const unsigned int sourceWidth, const unsigned int targetPaddingElements, const unsigned int sourcePaddingElements);

		/**
		 * Visualizes the distortion of a camera.
		 * @param pinholeCamera The pinhole camera profile for which the distortion/undistortion is visualized
		 * @param undistort True, to visualize the undistorted frame, otherwise the distorted frame is visualized
		 * @param horizontalBins Number of horizontal bins, with range [1, pinholeCamera.width() / 2u)
		 * @param verticalBins Number of vertical bins, with range [1, pinholeCamera.height() / 2u)
		 * @param worker Optional worker object to distribute the computation
		 * @return Resulting distortion frame
		 */
		static Frame visualizeDistortion(const PinholeCamera& pinholeCamera, const bool undistort, const unsigned int horizontalBins, const unsigned int verticalBins, Worker* worker = nullptr);

		/**
		 * Creates randomized data for a given frame.
		 * Integer pixel formats as well as float pixel formats will receive values in the range [0, 255] or [-128, 127] for each pixel and channel if 'limitedValueRange == true'
		 * @param frame The frame to fill with randomized data, may be invalid
		 * @param skipPaddingArea True, to leave elements within the padding area untouched; False, to randomize the entire memory
		 * @param randomGenerator Optional explicit random generator to be used, nullptr to use any
		 * @param limitedValueRange True, to use a value range of [0, 255] and [-128, 127] independently of the actual element data type; False, to use the full value range
		 */
		static void randomizeFrame(Frame& frame, const bool skipPaddingArea = true, RandomGenerator* randomGenerator = nullptr, const bool limitedValueRange = false);

		/**
		 * Creates randomized data for a given frame while the randomized data will be in a specific value range.
		 * The pixel format of the frame must be generic (a pixel format with zipped channel layout).
		 * @param frame The frame to fill with randomized data, may be invalid
		 * @param minValue The minimal random value, with range (-infinity, maxValue]
		 * @param maxValue The maximal random value, with range [minValue, infinity)
		 * @param skipPaddingArea True, to leave elements within the padding area untouched; False, to randomize the entire memory
		 * @param randomGenerator Optional explicit random generator to be used, nullptr to use any
		 * @tparam T The data type of each pixel element, e.g., 'unsigned char' or 'float'
		 */
		template <typename T>
		static void randomizeFrame(Frame& frame, const T minValue, const T maxValue, const bool skipPaddingArea = true, RandomGenerator* randomGenerator = nullptr);

		/**
		 * Returns a randomized frame for a given frame type, the optional padding area will be filled with random (not binary) noise.
		 * The resulting frame may contain random padding elements.<br>
		 * Integer pixel formats as well as float pixel formats will receive values in the range [0, 255] or [-128, 127] for each pixel and channel if 'limitedValueRange == true'<br>
		 * The timestamp will be randomized as well.
		 * @param frameType The frame type for which the randomized frame will be created, may be invalid
		 * @param randomGenerator Optional explicit random generator to be used, nullptr to use any
		 * @param limitedValueRange True, to use a value range of [0, 255] and [-128, 127] independently of the actual element data type; False, to use the full value range
		 * @return The randomized frame, invalid if frameType is invalid
		 */
		static Frame randomizedFrame(const FrameType& frameType, RandomGenerator* randomGenerator = nullptr, const bool limitedValueRange = false);

		/**
		 * Creates an 8 bit frame with a random binary mask, the optional padding area will be filled with random (not binary) noise.
		 * The resulting mask may contain random padding elements.<br>
		 * The timestamp will be randomized as well.
		 * @param width The width of the resulting frame in pixel, with range [1, infinity)
		 * @param height The height of the resulting frame in pixel, with range [1, infinity)
		 * @param maskValue The value of mask pixels, 0xFF - maskValue will be the value of non-mask pixels, with range [0x00, 0xFF]
		 * @param randomGenerator Random generator object to be used
		 */
		static Frame randomizedBinaryMask(const unsigned int width, const unsigned int height, const uint8_t maskValue, RandomGenerator* randomGenerator = nullptr);

		/**
		 * Returns whether all border pixels of an image are set to zero.
		 * @param frame The frame to test, must be valid
		 * @return True, if so
		 */
		static bool isBorderZero(const Frame& frame);

		/**
		 * Deprecated.
		 *
		 * Returns the mirrored correction offset for a given index.
		 * @param index Index to return the correction offset for
		 * @param elements Number of maximal elements
		 * @return The resulting correction offset, with range (-infinity, infinity)
		 */
		static inline int mirrorOffset(const unsigned int index, const unsigned int elements);

		/**
		 * Returns the mirrored correction offset for a given index.
		 * The correction offset is calculated as follows:
		 * <pre>
		 *                           |<----------------------- valid value range -------------------------->|
		 *
		 * index:          -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7,  ... elements - 3, elements - 2, elements - 1, elements, elements + 1
		 * result:          5   3   1  0  0  0  0  0  0  0  0                  0             0             0  -1        -3
		 * </pre>
		 * @param index The index for which the correction offset will be returned, with range [-elements / 2, elements + elements / 2]
		 * @param elements The number of maximal elements, with range [1, infinity)
		 * @return The resulting correction offset, with range (-infinity, infinity)
		 */
		static OCEAN_FORCE_INLINE int mirrorOffset(const int index, const unsigned int elements);

		/**
		 * Returns the mirrored index for a given index.
		 * The mirrored index is calculated as follows:
		 * <pre>
		 *                           |<----------------------- valid value range -------------------------->|
		 *
		 * index:          -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7,  ... elements - 3, elements - 2, elements - 1, elements + 0, elements + 1
		 * mirrored index:  2   1   0  0  1  2  3  4  5  6  7       elements - 3  elements - 2  elements - 1  elements - 1  elements - 2
		 * </pre>
		 * @param index The index for which the mirrored index will be returned, with range [-elements / 2, elements + elements / 2]
		 * @param elements The number of maximal elements, with range [1, infinity)
		 * @return The resulting mirrored index, with range [0, elements - 1]
		 */
		static OCEAN_FORCE_INLINE unsigned int mirrorIndex(const int index, const unsigned int elements);

		/**
		 * Returns whether the padding memory at the end of each image row is identical in two given frames.
		 * In case both frames do not have a padding memory, True is returned.
		 * @param frameA The first frame top be compared, must be valid
		 * @param frameB The second frame to be compared, can have a different width as frameA, must be valid
		 * @return True, if so
		 */
		static bool isPaddingMemoryIdentical(const Frame& frameA, const Frame& frameB);

		/**
		 * Creates an image with a checkerboard pattern with pixel format FORMAT_Y8 with a dark elements in the upper left corner.
		 * @param width The width of the resulting image, with range [horizontalElements, infinity), must be a multiple of 'horizontalElements'
		 * @param height The height of the resulting image, with range [verticalElements, infinity), must be a multiple of 'verticalElements'
		 * @param horizontalElements The number of horizontal checkerboard elements, with range [1, infinity)
		 * @param verticalElements The number of vertical checkerboard elements, with range [1, infinity)
		 * @param paddingElements The number of padding elements at the end of each image row, in elements, with range [0, infinity)
		 * @param bright The color value of the bright elements
		 * @param dark The color value of the dark elements
		 * @return The resulting checkerboard image
		 */
		static Frame createCheckerboardImage(const unsigned int width, const unsigned int height, const unsigned int horizontalElements, const unsigned int verticalElements, const unsigned int paddingElements, const uint8_t bright = 0xFFu, const uint8_t dark = 0x00u);

	protected:

		/**
		 * Returns whether all border pixels of an image are set to zero.
		 * @param frame The frame to test, must be valid
		 * @return True, if so
		 * @tparam T The data type of the pixel elements
		 */
		template <typename T>
		static bool isBorderZero(const Frame& frame);

		/**
		 * Randomizes a given memory block.
		 * @param data The memory block to randomize, must be valid
		 * @param widthElements The width of the memory block, in elements, with range [1, infinity)
		 * @param height The height of the memory block, in pixels, with range [1, infinity)
		 * @param paddingElements The number of padding elements at the end of each block row, in elements, with range [0, infinity)
		 * @param randomGenerator The random generator to be used
		 * @param limitedValueRange True, to use a value range of [0, 255] and [-128, 127] independently of the actual element data type; False, to use the full value range
		 * @tparam T The data type of each element
		 */
		template <typename T>
		static void randomizMemory(T* data, const unsigned int widthElements, const unsigned int height, const unsigned int paddingElements, RandomGenerator& randomGenerator, const bool limitedValueRange);
};

/**
 * Specialization of the helper struct.
 */
template <>
struct CVUtilities::SmallerPatchSize<5u>
{
	/// The size of the next smaller patch size, which his the same as 5 is the smallest one.
	const static unsigned int size = 5u;
};

/**
 * Specialization of the helper struct.
 */
template <>
struct CVUtilities::SmallerPatchSize<7u>
{
	/// The size of the next smaller patch size.
	const static unsigned int size = 5u;
};

/**
 * Specialization of the helper struct.
 */
template <>
struct CVUtilities::SmallerPatchSize<9u>
{
	/// The size of the next smaller patch size.
	const static unsigned int size = 5u;
};

/**
 * Specialization of the helper struct.
 */
template <>
struct CVUtilities::SmallerPatchSize<15u>
{
	/// The size of the next smaller patch size.
	const static unsigned int size = 5u;
};

/**
 * Specialization of the helper struct.
 */
template <>
struct CVUtilities::SmallerPatchSize<31u>
{
	/// The size of the next smaller patch size.
	const static unsigned int size = 15u;
};

/**
 * Specialization of the helper struct.
 */
template <>
struct CVUtilities::SmallerPatchSize<63u>
{
	/// The size of the next smaller patch size.
	const static unsigned int size = 31u;
};

template <typename T, unsigned int tChannels>
inline void CVUtilities::copyPixel(T* const target, const T* const source)
{
	static_assert(tChannels != 0u, "The specified number of channels is not supported!");

	ocean_assert(target != nullptr && source != nullptr);

	typedef typename DataType<T, tChannels>::Type PixelType;

	*((PixelType*)target) = *((const PixelType*)source);
}

template <unsigned int tChannels>
inline void CVUtilities::copyPixel(uint8_t* const target, const uint8_t* const source)
{
	copyPixel<uint8_t, tChannels>(target, source);
}

template <typename T, unsigned int tChannels>
inline void CVUtilities::copyPixel(T* const target, const T* const source, const unsigned int targetIndex, const unsigned int sourceIndex)
{
	static_assert(tChannels != 0u, "The specified number of channels is not supported!");

	ocean_assert(target != nullptr && source != nullptr);

	typedef typename DataType<T, tChannels>::Type PixelType;

	*((PixelType*)target + targetIndex) = *((const PixelType*)source + sourceIndex);
}

template <unsigned int tChannels>
inline void CVUtilities::copyPixel(uint8_t* const target, const uint8_t* const source, const unsigned int targetIndex, const unsigned int sourceIndex)
{
	copyPixel<uint8_t, tChannels>(target, source, targetIndex, sourceIndex);
}

template <typename T, unsigned int tChannels>
inline void CVUtilities::copyPixel(T* const target, const T* const source, const unsigned int xTarget, const unsigned int yTarget, const unsigned int xSource, const unsigned int ySource, const unsigned int targetWidth, const unsigned int sourceWidth, const unsigned int targetPaddingElements, const unsigned int sourcePaddingElements)
{
	static_assert(tChannels != 0u, "The specified number of channels is not supported!");

	ocean_assert(target != nullptr && source != nullptr);
	ocean_assert(xSource < sourceWidth);
	ocean_assert(xTarget < targetWidth);

	typedef typename DataType<T, tChannels>::Type PixelType;

	const unsigned int sourceStrideElements = sourceWidth * tChannels + sourcePaddingElements;
	const unsigned int targetStrideElements = targetWidth * tChannels + targetPaddingElements;

	*((PixelType*)(target + yTarget * targetStrideElements) + xTarget) = *((const PixelType*)(source + ySource * sourceStrideElements) + xSource);
}

template <unsigned int tChannels>
inline void CVUtilities::copyPixel(uint8_t* const target, const uint8_t* const source, const unsigned int xTarget, const unsigned int yTarget, const unsigned int xSource, const unsigned int ySource, const unsigned int targetWidth, const unsigned int sourceWidth, const unsigned int targetPaddingElements, const unsigned int sourcePaddingElements)
{
	copyPixel<uint8_t, tChannels>(target, source, xTarget, yTarget, xSource, ySource, targetWidth, sourceWidth, targetPaddingElements, sourcePaddingElements);
}

template <>
inline void CVUtilities::randomizeFrame(Frame& frame, const uint8_t minValue, const uint8_t maxValue, const bool skipPaddingArea, RandomGenerator* randomGenerator)
{
	ocean_assert(frame.isValid());
	ocean_assert(FrameType::formatIsGeneric(frame.pixelFormat()) && frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);
	ocean_assert(minValue <= maxValue);

	RandomGenerator localRandomGenerator(randomGenerator);

	const unsigned int elementsInRow = skipPaddingArea ? frame.channels() * frame.width() : frame.strideElements();

	for (unsigned int y = 0u; y < frame.height(); ++y)
	{
		uint8_t* data = frame.row<uint8_t>(y);

		for (unsigned int x = 0u; x < elementsInRow; ++x)
		{
			data[x] = uint8_t(RandomI::random(localRandomGenerator, (unsigned int)minValue, (unsigned int)maxValue));
		}
	}
}

template <>
inline void CVUtilities::randomizeFrame(Frame& frame, const float minValue, const float maxValue, const bool skipPaddingArea, RandomGenerator* randomGenerator)
{
	ocean_assert(frame.isValid());
	ocean_assert(FrameType::formatIsGeneric(frame.pixelFormat()) && frame.dataType() == FrameType::DT_SIGNED_FLOAT_32);
	ocean_assert(minValue <= maxValue);

	RandomGenerator localRandomGenerator(randomGenerator);

	const unsigned int elementsInRow = skipPaddingArea ? frame.channels() * frame.width() : frame.strideElements();

	for (unsigned int y = 0u; y < frame.height(); ++y)
	{
		float* data = frame.row<float>(y);

		for (unsigned int x = 0u; x < elementsInRow; ++x)
		{
			data[x] = RandomF::scalar(localRandomGenerator, minValue, maxValue);
		}
	}
}

inline int CVUtilities::mirrorOffset(const unsigned int index, const unsigned int elements)
{
	return mirrorOffset(int(index), elements);
}

OCEAN_FORCE_INLINE int CVUtilities::mirrorOffset(const int index, const unsigned int elements)
{
	return int(mirrorIndex(index, elements)) - index;
}

OCEAN_FORCE_INLINE unsigned int CVUtilities::mirrorIndex(const int index, const unsigned int elements)
{
	if ((unsigned int)index < elements)
	{
		return (unsigned int)(index);
	}

	if (index < 0)
	{
		const unsigned int result = (unsigned int)(-index) - 1u;

		ocean_assert(result < elements);

		return result;
	}
	else
	{
		const unsigned int result = elements * 2u - (unsigned int)(index) - 1u;

		ocean_assert(result < elements);

		return result;
	}
}

}

}

#endif // META_OCEAN_CV_CV_UTILITIES_H
