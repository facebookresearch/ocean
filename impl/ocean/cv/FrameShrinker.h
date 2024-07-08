/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_SHRINKER_H
#define META_OCEAN_CV_FRAME_SHRINKER_H

#include "ocean/cv/CV.h"
#include "ocean/cv/NEON.h"
#include "ocean/cv/SSE.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements function to downsize a frame.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameShrinker
{
	protected:

		/**
		 * Definition of a function pointer allowing to down sample a block of elements by using a 11 pattern.
		 * @param sourceRow0 The upper source row, must be valid
		 * @param sourceRow1 The lower source row, must be valid
		 * @param target The target receiving the down sampled data
		 */
		typedef void (*DownsampleBlockByTwo8BitPerChannelFunction)(const uint8_t* const sourceRow0, const uint8_t* const sourceRow1, uint8_t* const target);

		/**
		 * Definition of a function pointer allowing to down sample a binary block of elements by using a 11 pattern.
		 * @param sourceRow0 The upper source row, must be valid
		 * @param sourceRow1 The lower source row, must be valid
		 * @param target The target receiving the down sampled data
		 * @param threshold Minimal sum threshold of four pixels to result in a pixel with value 255
		 */
		typedef void (*DownsampleBlockByTwoBinary8BitPerChannelFunction)(const uint8_t* const sourceRow0, const uint8_t* const sourceRow1, uint8_t* const target, const uint16_t threshold);

	public:

		/**
		 * Reduces the resolution of a given frame by two, applying a 1-1 downsampling.
		 * Each downsampled pixel is based on 2x2 (= four) corresponding pixels from the source image:
		 * <pre>
		 * | 1 1 |
		 * | 1 1 | * 1/4
		 * </pre>
		 * If the given source image has an odd frame dimension the last pixel row or the last pixel column is filtered together with the two valid rows or columns respectively.<br>
		 * If the type of the target frame does not match to the input frame the target frame (and image buffer) will be replaced by the correct one.
		 * @param source The source frame to resize, must be valid
		 * @param target The target frame receiving the down sampled frame data, can be invalid
		 * @param worker Optional worker object to distribute the computational load to several CPU cores
		 * @return True, if succeeded
		 */
		static bool downsampleByTwo11(const Frame& source, Frame& target, Worker* worker = nullptr);

		/**
		 * Reduces the resolution of a given binary mask by two, applying a 1-1 downsampling.
		 * Each downsampled pixel is based on 2x2 (= four) corresponding pixels from the source image:
		 * <pre>
		 * | 1 1 |
		 * | 1 1 | * 1/4
		 * </pre>
		 * If the given source image has an odd frame dimension the last pixel row or the last pixel column is filtered together with the two valid rows or columns respectively.<br>
		 * If the type of the target frame does not match to the input frame the target frame (and image buffer) will be replaced by the correct one.
		 * @param source The source mask to resize, must be valid
		 * @param target The target mask receiving the down sampled frame data, can be invalid
		 * @param threshold The threshold of the minimal sum of the four mask pixels values to result in a downsampled pixel with value 255, with range [0, 255 * 4]
		 * @param worker Optional worker object to distribute the computational load to several CPU cores
		 * @return True, if succeeded
		 */
		static bool downsampleBinayMaskByTwo11(const Frame& source, Frame& target, const unsigned int threshold = 766u, Worker* worker = nullptr);

		/**
		 * Reduces the resolution of a given frame by two, applying a 1-4-6-4-1 downsampling.
		 * Each downsampled pixel is based on  5x5 (= 25) corresponding pixels from the source image:
		 * <pre>
		 * | 1  4  6  4 1 |
		 * | 4 16 24 16 4 |
		 * | 6 24 36 24 6 | * 1/256
		 * | 4 16 24 16 4 |
		 * | 1  4  6  4 1 |
		 * </pre>
		 * The filter values are determined at even pixel coordinates (0, 2, 4, ...).
		 * If the type of the target frame does not match to the input frame the target frame (and image buffer) will be replaced by the correct one.<br>
		 * By default, the resolution of the target frame will be set to (source.width() / 2, source.height() / 2).
		 * @param source The source frame to resize, must be valid
		 * @param target The target frame receiving the down sampled frame data, with resolution [source.width() / 2, (source.width() + 1) / 2]x[source.height() / 2, (source.height() + 1) / 2], can be invalid
		 * @param worker Optional worker object to distribute the computational load to several CPU cores
		 * @return True, if succeeded
		 */
		static bool downsampleByTwo14641(const Frame& source, Frame& target, Worker* worker = nullptr);

		/**
		 * Reduces the resolution of a given frame by two, taking four pixel values into account.
		 * If the given source image has an odd frame dimension the last pixel row or the last pixel column is filtered together with the two valid rows or columns respectively.<br>
		 * @param frame The frame to down sample, must be valid
		 * @param worker Optional worker object to distribute the computational load to several CPU cores
		 * @return True, if succeeded
		 */
		static inline bool downsampleByTwo11(Frame& frame, Worker* worker = nullptr);

		/**
		 * Reduces the resolution of a given binary mask by two, taking 2x2 (= four) mask pixel values into account.
		 * @param mask The mask to down sample, must be valid
		 * @param threshold The threshold of the minimal sum of the four mask pixels values to result in a downsampled pixel with value 255, with range [0, 255 * 4]
		 * @param worker Optional worker object to distribute the computational load to several CPU cores
		 * @return True, if succeeded
		 */
		static inline bool downsampleBinayMaskByTwo11(Frame& mask, const unsigned int threshold = 766u, Worker* worker = nullptr);

		/**
		 * Reduces the resolution of a given frame by two, applying a 1-4-6-4-1 downsampling.
		 * Each downsampled pixel is based on  5x5 (= 25) corresponding pixels from the source image:
		  * <pre>
		 * | 1  4  6  4 1 |
		 * | 4 16 24 16 4 |
		 * | 6 24 36 24 6 | * 1/256
		 * | 4 16 24 16 4 |
		 * | 1  4  6  4 1 |
		 * </pre>
		 * The filter values are determined at even pixel coordinates (0, 2, 4, ...).<br>
		 * The resulting frame will have the resolution (frame.width() / 2, frame.height() / 2).
		 * @param frame The frame to down sample, must be valid
		 * @param worker Optional worker object to distribute the computational load to several CPU cores
		 * @return True, if succeeded
		 */
		static inline bool downsampleByTwo14641(Frame& frame, Worker* worker = nullptr);

		/**
		 * Fills the buffer of a pyramid frame for frames with 1 plane and data type DT_UNSIGNED_INTEGER_8.
		 * @param source The source frame buffer to be used, must be valid
		 * @param pyramidTarget The frame buffer of the frame pyramid, large enough for the requested layers, must be valid
		 * @param pyramidTargetSize The size of the pyramid target memory, in bytes, with range [1, infinity)
		 * @param layers The number of pyramid layers to be created, with range [1, infinity)
		 * @param copyFirstLayer True, to copy the first layer before processing the next finer layers; False, to start directly with the next coarser layer (the memory for pyramidTarget can then be correspondingly smaller)
		 * @param worker Optional worker object to distribute the computational load, nullptr otherwise
		 * @return True, if succeeded
		 */
		static bool pyramidByTwo11(const Frame& source, uint8_t* const pyramidTarget, const size_t pyramidTargetSize, const unsigned int layers, const bool copyFirstLayer, Worker* worker);

		/**
		 * Fills the buffer of a pyramid frame for frames with 1 plane and data type DT_UNSIGNED_INTEGER_8 applying a 1-1 downsampling.
		 * Each pixel of a coarser pyramid level is based on 2x2 pixels from the corresponding finer pyramid level.
		 * @param source The source frame buffer to be used, must be valid
		 * @param pyramidTarget The frame buffer of the frame pyramid, large enough for the requested layers, must be valid
		 * @param sourceWidth Width of the source frame in pixel, with range [1, infinity)
		 * @param sourceHeight Height of the source frame in pixel, with range [1, infinity)
		 * @param channels The number of data channel the frames have, with range  [1, infinity)
		 * @param pyramidTargetSize The size of the pyramid target memory, in bytes, with range [1, infinity)
		 * @param layers The number of pyramid layers to be created, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param copyFirstLayer True, to copy the first layer before processing the next finer layers; False, to start directly with the next coarser layer (the memory for pyramidTarget can then be correspondingly smaller)
		 * @param worker Optional worker object to distribute the computational load, nullptr otherwise
		 * @return True, if succeeded
		 */
		static bool pyramidByTwo8BitPerChannel11(const uint8_t* source, uint8_t* pyramidTarget, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int channels, const size_t pyramidTargetSize, const unsigned int layers, const unsigned int sourcePaddingElements, const bool copyFirstLayer, Worker* worker);

		/**
		 * Reduces the resolution of a given frame by two, applying a 1-1 downsampling.
		 * Each downsampled pixel is based on 2x2 (= four) corresponding pixels from the source image:
		 * <pre>
		 * | 1 1 |
		 * | 1 1 | * 1/4
		 * </pre>
		 * The target width and target height will be sourceWidth / 2, targetHeight / 2.<br>
		 * In case, the given source image has an odd frame resolution the last pixel row or the last pixel column is filtered together with the two valid rows or columns respectively.<br>
		 * However, in general, this function is meant to be used for images with even width and height.
		 * @param source The source frame buffer to resize, must be valid
		 * @param target The target frame buffer receiving the down sampled image information, must be valid
		 * @param sourceWidth Width of the source frame in pixel, with range [2, infinity)
		 * @param sourceHeight Height of the source frame in pixel, with range [2, infinity)
		 * @param channels The number of data channel the frames have, with range  [1, infinity)
		 * @param sourcePaddingElements Optional padding at the end of each source row in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional padding at the end of each target row in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load to several CPU cores
		 */
		static inline void downsampleByTwo8BitPerChannel11(const uint8_t* source, uint8_t* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int channels, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Reduces the resolution of a given binary frame with 8bit per pixel with values 0 and 255 by two, taking four pixel values into account.
		 * @param source Binary source frame buffer to resize
		 * @param target Binary target frame buffer
		 * @param sourceWidth Width of the source frame in pixel, with range [2, infinity)
		 * @param sourceHeight Height of the source frame in pixel, with range [2, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param threshold Minimal sum threshold of four pixels to result in a pixel with value 255
		 * @param worker Optional worker object to distribute the computational load to several CPU cores
		 */
		static inline void downsampleBinayMaskByTwo8BitPerChannel11(const uint8_t* source, uint8_t* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int threshold = 766u, Worker* worker = nullptr);

		/**
		 * Reduces the resolution of a given frame by two, applying a 1-4-6-4-1 downsampling.
		 * Each downsampled pixel is based on  5x5 (= 25) corresponding pixels from the source image:
		 * <pre>
		 * | 1  4  6  4 1 |
		 * | 4 16 24 16 4 |
		 * | 6 24 36 24 6 | * 1/256
		 * | 4 16 24 16 4 |
		 * | 1  4  6  4 1 |
		 * </pre>
		 * The filter values are determined at even pixel coordinates.
		 * @param source The source frame to resize, must be valid
		 * @param target The target frame receiving the down sampled frame data, with memory for (targetWidth * targetHeight) pixels, must be valid
		 * @param sourceWidth The width of the source frame in pixel, with range [2, infinity)
		 * @param sourceHeight The height of the source frame in pixel, with range [2, infinity)
		 * @param targetWidth The width of the target frame in pixel, with range [sourceWidth / 2, (sourceWidth + 1) / 2]
		 * @param targetHeight The height of the target frame in pixel, with range [sourceHeight / 2, (sourceHeight + 1) / 2]
		 * @param channels The number of frame channels, with range [1, infinity)
		 * @param sourcePaddingElements Optional padding at the end of each source row in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional padding at the end of each target row in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load to several CPU cores
		 */
		static inline void downsampleByTwo8BitPerChannel14641(const uint8_t* const source, uint8_t* const target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int channels, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

	protected:

		/**
		 * Fills the buffer of a pyramid frame for frames with 1 plane and data type DT_UNSIGNED_INTEGER_8.
		 * @param source The source frame buffer from which the pyramid will be created, must be valid
		 * @param pyramidTarget The frame buffer of the frame pyramid, large enough for the requested layers, must be valid
		 * @param sourceWidth Width of the source frame in pixel, with range [1, infinity)
		 * @param sourceHeight Height of the source frame in pixel, with range [1, infinity)
		 * @param channels The number of frame channels the source frame has, with range [1, infinity)
		 * @param pyramidTargetSize The size of the pyramid target memory, in bytes, with range [1, infinity)
		 * @param layers The number of pyramid layers to be created, with range [1, infinity)
		 * @param copyFirstLayer True, to copy the first layer before processing the next finer layers; False, to start directly with the next coarser layer
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 * @param threads The number of threads to be used if a valid worker is defined, with range [1u, worker->threads()] and the following must hold: threads * 2^layers <= sourceHeight
		 * @return True, if succeeded
		 */
		static bool pyramidByTwo8BitPerChannel11WithThreads(const uint8_t* source, uint8_t* pyramidTarget, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int channels, const size_t pyramidTargetSize, const unsigned int layers, const bool copyFirstLayer, const unsigned int sourcePaddingElements, const unsigned int threads, Worker* worker);

		/**
		 * Fills a subset of the buffer of a pyramid frame for a given frame with 1 plane and data type DT_UNSIGNED_INTEGER_8.
		 * @param source The source frame buffer from which the pyramid will be created, must be valid
		 * @param pyramidTarget The frame buffer of the frame pyramid, large enough for the requested layers, must be valid
		 * @param sourceWidth Width of the source frame in pixel, with range [1, infinity)
		 * @param sourceHeight Height of the source frame in pixel, with range [1, infinity)
		 * @param channels The number of frame channels the source image has, with range [1, infinity)
		 * @param pyramidTargetSize The size of the pyramid target memory, in bytes, with range [1, infinity)
		 * @param layers The number of pyramid layers to be created, with range [1, infinity)
		 * @param copyFirstLayer True, to copy the first layer before processing the next finer layers; False, to start directly with the next coarser layer
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param firstSubsetsSourceHeight The height of the first subsets on the finest pyramid layer, with range [1, sourceHeight]
		 * @param subsets The number of subsets which will be used in parallel (the number of threads executing the function in parallel)
		 * @param subsetIndex The index of the subset which is handled, with range [0, subsetIndex)
		 * @param valueOne The parameter must be 1
		 */
		static void pyramidByTwo8BitPerChannel11WithThreadsSubset(const uint8_t* source, uint8_t* pyramidTarget, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int channels, const size_t pyramidTargetSize, const unsigned int layers, const bool copyFirstLayer, const unsigned int sourcePaddingElements, const unsigned int firstSubsetsSourceHeight, const unsigned int subsets, const unsigned int subsetIndex, const unsigned int valueOne);

		/**
		 * Reduces the resolution of a given frame by two.
		 * @param source The source frame buffer to resize, must be valid
		 * @param target The target frame buffer receiving the down sampled image information, must be valid
		 * @param sourceWidth Width of the source frame in pixel, with range [2, infinity)
		 * @param sourceHeight Height of the source frame in pixel, with range [2, infinity)
		 * @param channels The number of data channel the frames have, with range  [1, infinity)
		 * @param sourcePaddingElements Optional padding at the end of each source row in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional padding at the end of each target row in elements, with range [0, infinity)
		 * @param firstTargetRow The first target row to be handled, with range [0, sourceHeight / 2 - 1]
		 * @param numberTargetRows The number of target rows to be handled, with range [1, sourceHeight / 2 - firstTargetRow]
		 */
		static void downsampleByTwo8BitPerChannel11Subset(const uint8_t* source, uint8_t* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int channels, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstTargetRow, const unsigned int numberTargetRows);

		/**
		 * Reduces the resolution of a given binary frame with 8bit per pixel with values 0 and 255 by two, taking four pixel values into account.
		 * @param source The binary source frame, must be valid
		 * @param target The binary target frame, must be valid
		 * @param sourceWidth Width of the source frame in pixel, with range [2, infinity)
		 * @param sourceHeight Height of the source frame in pixel, with range [2, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param threshold The threshold of the minimal sum of the four mask pixels values to result in a downsampled pixel with value 255, with range [0, 255 * 4]
		 * @param firstTargetRow The first target row to be handled, with range [0, sourceHeight / 2 - 1]
		 * @param numberTargetRows The number of target rows to be handled, with range [1, sourceHeight / 2 - firstTargetRow]
		 */
		static void downsampleBinayMaskByTwo8BitPerChannel11Subset(const uint8_t* source, uint8_t* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int threshold, const unsigned int firstTargetRow, const unsigned int numberTargetRows);

		/**
		 * Determines the function to down sample a block of pixel elements by two with a 11 filter pattern.
		 * This function will return the function able to handle the largest block of elements, if any function exists.
		 * @param sourceWidth The width of the source frame in pixel, with range [2, infinity)
		 * @param channels The number of frame channels, with range [1, infinity)
		 * @param downsampleBlockFunction The resulting function down sampling the block of elements, nullptr if no matching function could be determined
		 * @param sourceElementsPerBlock The resulting number of source elements which will be handled by the resulting down sampling function, 0 if no matching function could be determined
		 */
		static void determineFunctionDownsampleBlockByTwo8Bit11(const unsigned int sourceWidth, const unsigned int channels, DownsampleBlockByTwo8BitPerChannelFunction& downsampleBlockFunction, unsigned int& sourceElementsPerBlock);

		/**
		 * Downsamples an area of 2x3 pixels (two rows and three columns) to one pixel in a frame, applying a 1-2-1 down sampling filter.
		 * This function is typically handling the last pixel in a frame (if the frame has an odd width).
		 * @param source The location of the top left pixel in the source frame, must be valid
		 * @param target The location in the target frame, must be valid
		 * @param channels The number of channels the source (and target) frame have, with range [1, infinity)
		 * @param sourceStrideElements The number of elements between two successive rows in the source frame, in elements, with range [sourceWidth * channels, infinity)
		 */
		OCEAN_PREVENT_INLINE static void downsampleByTwoOneRowThreeColumns8BitPerChannel121(const uint8_t* source, uint8_t* target, const unsigned int channels, const unsigned int sourceStrideElements);

		/**
		 * Downsamples three rows to one row in a frame, applying a 2x3 [1-2-1, 1-2-1] down sampling filter.
		 * In case the width of the source rows is 1 pixel, a 1x3 [1-2-1] is applied.<br>
		 * In case the width of the source rows is not a multiple of two, a 3x3 [1-2-1, 2-4-2, 1-2-1] filter is applied to the last 3 pixels.
		 * @param source The location of the left pixel in the top source row, must be valid
		 * @param target The location in the left pixel target row, must be valid
		 * @param sourceWidth The number of pixels in the source frame, with range [1, infinity)
		 * @param channels The number of channels the source (and target) frame have, with range [1, infinity)
		 * @param sourceStrideElements The number of elements between two successive rows in the source frame, in elements, with range [sourceWidth * channels, infinity)
		 */
		OCEAN_PREVENT_INLINE static void downsampleByTwoThreeRows8BitPerChannel121(const uint8_t* source, uint8_t* target, const unsigned int sourceWidth, const unsigned int channels, const unsigned int sourceStrideElements);

		/**
		 * Applies a vertical 14641 filter to each pixel in a given row not applying any SIMD instructions.
		 * This function can be used to down sample an image e.g., by a factor of two.
		 * @param source The entire source frame holding the row to which the filter is applied, must be valid
		 * @param targetRow The target row receiving the filter results which are not normalized, must be valid
		 * @param sourceElements The number of elements in each row (pixels * channels), with range [1, infinity)
		 * @param sourceHeight The height of the source frame in pixel, with range [2, infinity)
		 * @param sourceStrideElements The stride of the source frame (the number of elements in each row - may including padding at the end of each row), with range [sourceElements, infinity)
		 * @param ySource The row within the source frame to which the filter will be applied, with range [0, sourceHeight)
		 * @see downsampleByTwoRowHorizontal8BitPerChannel14641().
		 */
		static void downsampleByTwoRowVertical8BitPerChannel14641(const uint8_t* const source, uint16_t* targetRow, const unsigned int sourceElements, const unsigned int sourceHeight, const unsigned int sourceStrideElements, const unsigned int ySource);

		/**
		 * Applies a horizontal 14641 filter to each second pixel in a given row not applying any SIMD instructions.
		 * Each second pixel is skipped so that the target row has a length half of the source row.<br>
		 * This function can be used to down sample an image e.g., by a factor of two.
		 * @param sourceRow The source row already holding the vertical filter responses to which now the horizontal filter will be applied, must be valid
		 * @param targetRow The target row receiving the filter results which will be normalized before assignment, must be valid
		 * @param targetWidth The width of the target row in pixel, with range [1, infinity)
		 * @param channels The number of channels the target row (and source row) has, with range [1, infinity)
		 * @see downsampleByTwoRowVertical8BitPerChannel14641().
		 */
		static void downsampleByTwoRowHorizontal8BitPerChannel14641(const uint16_t* sourceRow, uint8_t* targetRow, const unsigned int targetWidth, const unsigned int channels);

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

		/**
		 * Applies a vertical 14641 filter to each pixel in a given row using SSE instructions.
		 * This function can be used to down sample an image e.g., by a factor of two.
		 * @param source The entire source frame holding the row to which the filter is applied, must be valid
		 * @param targetRow The target row receiving the filter results which are not normalized, must be valid
		 * @param sourceElements The number of elements in each row (pixels * channels), with range [1, infinity)
		 * @param sourceHeight The height of the source frame in pixel, with range [2, infinity)
		 * @param sourceStrideElements The stride of the source frame (the number of elements in each row - may including padding at the end of each row), with range [sourceElements, infinity)
		 * @param ySource The row within the source frame to which the filter will be applied, with range [0, sourceHeight)
		 * @see downsampleByTwoRowHorizontal8BitPerChannel14641NEON().
		 */
		static void downsampleByTwoRowVertical8BitPerChannel14641SSE(const uint8_t* const source, uint16_t* targetRow, const unsigned int sourceElements, const unsigned int sourceHeight, const unsigned int sourceStrideElements, const unsigned int ySource);

#endif // OCEAN_HARDWARE_SSE_VERSION >= 41

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

		/**
		 * Applies a vertical 14641 filter to each pixel in a given row using NEON instructions.
		 * This function can be used to down sample an image e.g., by a factor of two.
		 * @param source The entire source frame holding the row to which the filter is applied, must be valid
		 * @param targetRow The target row receiving the filter results which are not normalized, must be valid
		 * @param sourceElements The number of elements in each row (pixels * channels), with range [16, infinity)
		 * @param sourceHeight The height of the source frame in pixel, with range [2, infinity)
		 * @param sourceStrideElements The stride of the source frame (the number of elements in each row - may including padding at the end of each row), with range [sourceElements, infinity)
		 * @param ySource The row within the source frame to which the filter will be applied, with range [0, sourceHeight)
		 * @see downsampleByTwoRowHorizontal8BitPerChannel14641NEON().
		 */
		static void downsampleByTwoRowVertical8BitPerChannel14641NEON(const uint8_t* const source, uint16_t* targetRow, const unsigned int sourceElements, const unsigned int sourceHeight, const unsigned int sourceStrideElements, const unsigned int ySource);

		/**
		 * Applies a horizontal 14641 filter to each second pixel in a given row applying NEON instructions.
		 * Each second pixel is skipped so that the target row has a length half of the source row.<br>
		 * This function can be used to down sample an image e.g., by a factor of two.
		 * @param sourceRow The source row already holding the vertical filter responses to which now the horizontal filter will be applied, must be valid
		 * @param targetRow The target row receiving the filter results which will be normalized before assignment, must be valid
		 * @param targetWidth The width of the target row in pixel, with individual ranges depending on tChannels
		 * @param channels The number of frame channels the target row (and source row) has, with range [1, infinity)
		 * @see downsampleByTwoRowVertical8BitPerChannel14641NEON().
		 * @tparam tChannels The number of channels the target row (and source row) has, with range [1, 4]
		 */
		template <unsigned int tChannels>
		static void downsampleByTwoRowHorizontal8BitPerChannel14641NEON(const uint16_t* sourceRow, uint8_t* targetRow, const unsigned int targetWidth, const unsigned int channels);

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

		/**
		 * Reduces the resolution in a subregion of a given frame by two, taking 5x5 (= 25) pixel values into account.
		 * The filter values are determined at even pixel coordinates.
		 * @param source The source frame to resize, must be valid
		 * @param target The target frame receiving the down sample frame data, with memory for (targetWidth * targetHeight) pixels, must be valid
		 * @param sourceWidth The width of the source frame in pixel, with range [2, infinity)
		 * @param sourceHeight The height of the source frame in pixel, with range [2, infinity)
		 * @param targetWidth The width of the target frame in pixel, with range [sourceWidth / 2, (sourceWidth + 1) / 2]
		 * @param channels The number of frame channels, with range [1, infinity)
		 * @param sourceStrideElements The stride of the source frame in elements (sourceWidth * channels + optional padding), with range [sourceWidth * channels, infinity)
		 * @param targetStrideElements The stride of the target frame in elements (targetWidth * channels + optional padding), with range [targetWidth * channels, infinity)
		 * @param firstTargetRow The first target row to be handled, with range [0, targetHeight)
		 * @param numberTargetRows The number of target rows to be handled, with range [1, targetHeight - firstTargetRow]
		 */
		static void downsampleByTwo8BitPerChannel14641Subset(const uint8_t* source, uint8_t* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int channels, const unsigned int sourceStrideElements, const unsigned int targetStrideElements, const unsigned int firstTargetRow, const unsigned int numberTargetRows);

		/**
		 * Mirrors a given value at the left border if necessary.
		 * The function provides a result as below:<br>
		 * <pre>
		 * Original: -3 -2 -1 |  0  1  2  3  4  5  6
		 *   Result:  2  1  0 |  0  1  2  3  4  5  6
		 * </pre>
		 * @param value The value to be mirrored, with range (-infinity, infinity)
		 * @return Mirrored value
		 * @ingroup base
		 */
		static inline unsigned int mirroredBorderLocationLeft(const int value);

		/**
		 * Mirrors a given value at the right border if necessary.
		 * The values is mirrored according to a given size parameter.<br>
		 * The function provides a result as below:<br>
		 * <pre>
		 * Original: 4  5  6 ... s-2  s-1 |   s  s+1  s+2
		 *   Result: 4  5  6 ... s-2  s-1 | s-1  s-2  s-3
		 * </pre>
		 * @param value The value to be mirrored, with range [0, 2*size)
		 * @param size Specified size defining the upper mirror border, with range [1, 2147483647]
		 * @return Mirrored value
		 * @ingroup base
		 */
		static inline unsigned int mirroredBorderLocationRight(const unsigned int value, const unsigned int size);
};

inline bool FrameShrinker::downsampleByTwo11(Frame& frame, Worker* worker)
{
	Frame tmpFrame;
	if (!downsampleByTwo11(frame, tmpFrame, worker))
	{
		return false;
	}

	frame = std::move(tmpFrame);
	return true;
}

inline bool FrameShrinker::downsampleBinayMaskByTwo11(Frame& mask, const unsigned int threshold, Worker* worker)
{
	Frame tmpMask;
	if (!downsampleBinayMaskByTwo11(mask, tmpMask, threshold, worker))
	{
		return false;
	}

	mask = std::move(tmpMask);
	return true;
}

inline bool FrameShrinker::downsampleByTwo14641(Frame& frame, Worker* worker)
{
	Frame tmpFrame;
	if (!downsampleByTwo14641(frame, tmpFrame, worker))
	{
		return false;
	}

	frame = std::move(tmpFrame);
	return true;
}

inline void FrameShrinker::downsampleBinayMaskByTwo8BitPerChannel11(const uint8_t* source, uint8_t* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int threshold, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(sourceWidth >= 2u && sourceHeight >= 2u);
	ocean_assert(threshold <= 255 * 4u);

	const unsigned int targetHeight = sourceHeight / 2u;
	ocean_assert(targetHeight > 0u);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&downsampleBinayMaskByTwo8BitPerChannel11Subset, source, target, sourceWidth, sourceHeight, sourcePaddingElements, targetPaddingElements, threshold, 0u, 0u), 0u, targetHeight, 7u, 8u, 20u);
	}
	else
	{
		downsampleBinayMaskByTwo8BitPerChannel11Subset(source, target, sourceWidth, sourceHeight, sourcePaddingElements, targetPaddingElements, threshold, 0u, targetHeight);
	}
}

inline void FrameShrinker::downsampleByTwo8BitPerChannel11(const uint8_t* source, uint8_t* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int channels, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(sourceWidth >= 2u && sourceHeight >= 1u);
	ocean_assert(channels >= 1u);

	const unsigned int targetHeight = sourceHeight / 2u;

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(FrameShrinker::downsampleByTwo8BitPerChannel11Subset, source, target, sourceWidth, sourceHeight, channels, sourcePaddingElements, targetPaddingElements, 0u, 0u), 0u, targetHeight);
	}
	else
	{
		downsampleByTwo8BitPerChannel11Subset(source, target, sourceWidth, sourceHeight, channels, sourcePaddingElements, targetPaddingElements, 0u, targetHeight);
	}
}

inline unsigned int FrameShrinker::mirroredBorderLocationLeft(const int value)
{
	//            Outside | Inside
	// Original: -3 -2 -1 |  0  1  2  3  4  5  6
	//   Result:  2  1  0 |  0  1  2  3  4  5  6

	if (value >= 0)
	{
		return value;
	}
	else
	{
		return -value - 1;
	}
}

inline unsigned int FrameShrinker::mirroredBorderLocationRight(const unsigned int value, const unsigned int size)
{
	ocean_assert(value < 2u * size);

	//                         Inside | Outside
	// Original: 4  5  6 ... s-2  s-1 |   s  s+1  s+2
	//   Result: 4  5  6 ... s-2  s-1 | s-1  s-2  s-3

	if (value < size)
	{
		return value;
	}
	else
	{
		ocean_assert(size * 2u - value - 1u < size);
		return size * 2u - value - 1u;
	}
}

inline void FrameShrinker::downsampleByTwo8BitPerChannel14641(const uint8_t* const source, uint8_t* const target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int channels, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(sourceWidth >= 2u && sourceHeight >= 2u);
	ocean_assert((sourceWidth + 0u) / 2u == targetWidth || (sourceWidth + 1u) / 2u == targetWidth);
	ocean_assert((sourceHeight + 0u) / 2u == targetHeight || (sourceHeight + 1u) / 2u == targetHeight);
	ocean_assert(channels != 0u);

	const unsigned int sourceStrideElements = sourceWidth * channels + sourcePaddingElements;
	const unsigned int targetStrideElements = targetWidth * channels + targetPaddingElements;

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(downsampleByTwo8BitPerChannel14641Subset, source, target, sourceWidth, sourceHeight, targetWidth, channels, sourceStrideElements, targetStrideElements, 0u, 0u), 0u, targetHeight);
	}
	else
	{
		downsampleByTwo8BitPerChannel14641Subset(source, target, sourceWidth, sourceHeight, targetWidth, channels, sourceStrideElements, targetStrideElements, 0u, targetHeight);
	}
}

}

}

#endif // META_OCEAN_CV_FRAME_SHRINKER_H
