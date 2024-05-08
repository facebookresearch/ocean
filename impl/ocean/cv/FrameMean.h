/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_MEAN_H
#define META_OCEAN_CV_FRAME_MEAN_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"
#include "ocean/cv/FrameProviderInterface.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements functions allowing to determine e.g., the mean pixel value of a frame of a mean frame based on several frames.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameMean
{
	public:

		/**
		 * Determines the mean pixel value for a given frame.
		 * @param frame The data of the frame for which the mean value will be determined, must be valid
		 * @param width The width of the given frame in pixel, with range [1, infinity)
		 * @param height The height of the given frame in pixel, with range [1, infinity)
		 * @param meanValues The resulting mean values, one for each channel
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param worker An optional worker object to distribute the computation
		 * @tparam T The data type of each pixel channel
		 * @tparam TMean The data type of each mean value
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <typename T, typename TMean, unsigned int tChannels>
		static void meanValue(const T* frame, const unsigned int width, const unsigned int height, TMean* meanValues, const unsigned int framePaddingElements, Worker* worker = nullptr);

		/**
		 * Determines the mean frame for a given set of frames.
		 * The resulting mean frame is determined by adding the pixel color values of all frames individually with a subsequent normalization (by the number of frames).<br>
		 * Beware: This function cannot handle more than 16843010 frames.<br>
		 * @param frameProviderInterface The frame provider interface providing the set of frames, must be initialized
		 * @param targetPixelFormat The pixel format of the resulting mean frame, every frame from the frame provider will be converted into this pixel format before any application
		 * @param firstFrameIndex The index of the first frame from the frame provider interface which is used to determine the mean frame
		 * @param lastFrameIndex The index of the last (including) frame from the frame provider interface, with range [first, number frames from the interface)
		 * @param worker An optional worker object to distribute the computation
		 * @param abort An optional abort statement allowing to abort the determination at any time; set the value True to abort the request
		 * @return The resulting mean frame
		 * @see meanFrame8BitPerChannel().
		 */
		static Frame meanFrame(FrameProviderInterface& frameProviderInterface, const FrameType::PixelFormat& targetPixelFormat, const unsigned int firstFrameIndex, const unsigned int lastFrameIndex, Worker* worker = nullptr, bool* abort = nullptr);

		/**
		 * Determines the mean frame for a given set of frames.
		 * The resulting mean frame is determined by adding the pixel color values of all frames individually with a subsequent normalization (by the number of frames).<br>
		 * @param frameProviderInterface The frame provider interface providing the set of frames, must be initialized
		 * @param targetPixelFormat The pixel format of the resulting mean frame, every frame from the frame provider will be converted into this pixel format before any application
		 * @param firstFrameIndex The index of the first frame from the frame provider interface which is used to determine the mean frame
		 * @param lastFrameIndex The index of the last (including) frame from the frame provider interface, with range [first, number frames from the interface)
		 * @param worker An optional worker object to distribute the computation
		 * @param abort An optional abort statement allowing to abort the determination at any time; set the value True to abort the request
		 * @return The resulting mean frame
		 * @tparam tChannels The number of frame data channels which must correspond to the given targetPixelFormat
		 * @see meanFrame().
		 */
		template <unsigned int tChannels>
		static Frame meanFrame8BitPerChannel(FrameProviderInterface& frameProviderInterface, const FrameType::PixelFormat& targetPixelFormat, const unsigned int firstFrameIndex, const unsigned int lastFrameIndex, Worker* worker = nullptr, bool* abort = nullptr);

		/**
		 * Adds the individual pixel values of a given source frame to a target frame.
		 * @param source The source frame which pixels values are copied
		 * @param target The target frame to which the source pixel values will be added, individually for each pixel and channel
		 * @param width The width of both frames in pixels, with range [1, infinity)
		 * @param height The height of both frames in pixels, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker An optional worker object to distribute the computation
		 * @tparam TSource The data type of the elements of the source frame
		 * @tparam TTarget The data type of the elements of the target frame
		 * @tparam tChannels The number of frame data channels, with range [1, infinity)
		 */
		template <typename TSource, typename TTarget, unsigned int tChannels>
		static inline void addToFrame(const TSource* const source, TTarget* const target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Adds the individual pixel values of a given source frame to a target frame if the corresponding mask pixel is valid.
		 * @param source The source frame which pixels values are copied, must be valid and not have more than 4 channels and an 8 bit, unsigned data type
		 * @param mask The mask frame defining whether a source pixel is copied or not (and whether the corresponding normalization denominator of the target frame is increased by 1), must be valid and the data type must 8 bit, unsigned
		 * @param target The target frame to which the source pixel values will be added, must be valid and the frame type like source except that the data type must be 32 bit, unsigned
		 * @param denominators The denominator values individually for each pixel, must be valid and the data type must be 32 bit, unsigned
		 * @param worker An optional worker object to distribute the computation
		 * @param nonMaskValue The value indicating invalid mask pixels, i.e. pixels that should not be processed by this function, all other values will be interpreted as valid, range: [0, 255]
		 * @return True, if succeeded
		 */
		static bool addToFrameIndividually(const Frame& source, const Frame& mask, Frame& target, Frame& denominators, const uint8_t nonMaskValue, Worker* worker = nullptr);

		/**
		 * Adds the individual pixel values of a given source frame to a target frame if the corresponding mask pixel is valid.
		 * @param source The source frame which pixels values are copied
		 * @param mask The mask frame defining whether a source pixel is copied or not (and whether the corresponding normalization denominator of the target frame is increased by 1)
		 * @param target The target frame to which the source pixel values will be added
		 * @param denominators The denominator values individually for each pixel
		 * @param width The width of both frames in pixels, with range [1, infinity)
		 * @param height The height of both frames in pixels, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements of the source frame, range: [0, infinity)
		 * @param maskPaddingElements The number of padding elements of the mask frame, range: [0, infinity)
		 * @param targetPaddingElements The number of padding elements of the target frame, range: [0, infinity)
		 * @param denominatorsPaddingElements The number of padding elements of the denominators frame, range: [0, infinity)
		 * @param nonMaskValue The value indicating invalid mask pixels, i.e. pixels that should not be processed by this function, all other values will be interpreted as valid, range: [0, 255]
		 * @param worker An optional worker object to distribute the computation
		 * @tparam tChannels The number of frame data channels, not including the extra channel for the denominator value
		 */
		template <unsigned int tChannels>
		static inline void addToFrameIndividually8BitPerChannel(const uint8_t* source, const uint8_t* mask, unsigned int* target, unsigned int* denominators, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int maskPaddingElements, const unsigned int targetPaddingElements, const unsigned int denominatorsPaddingElements, const uint8_t nonMaskValue, Worker* worker = nullptr);

		/**
		 * Adds the individual pixel values of a given source frame to a target frame.
		 * @param source The source frame which pixels values are copied
		 * @param targetWithDenominator The target frame to which the source pixel values will be added, with individual denominator values, the denominator of each pixel is increased by 1
		 * @param width The width of both frames in pixels, with range [1, infinity)
		 * @param height The height of both frames in pixels, with range [1, infinity)
		 * @param worker An optional worker object to distribute the computation
		 * @tparam tChannels The number of frame data channels, not including the extra channel for the denominator value
		 */
		template <unsigned int tChannels>
		static inline void addToFrameIndividually8BitPerChannel(const uint8_t* source, unsigned int* targetWithDenominator, const unsigned int width, const unsigned int height, Worker* worker = nullptr);

		/**
		 * Adds the individual pixel values of a given source frame to a target frame if the corresponding mask pixel is valid.
		 * @param source The source frame which pixels values are copied
		 * @param mask The mask frame defining whether a source pixel is copied or not (and whether the corresponding normalization denominator of the target frame is increased by 1)
		 * @param targetWithDenominator The target frame to which the source pixel values will be added, with individual denominator values
		 * @param width The width of both frames in pixels, with range [1, infinity)
		 * @param height The height of both frames in pixels, with range [1, infinity)
		 * @param worker An optional worker object to distribute the computation
		 * @tparam tChannels The number of frame data channels, not including the extra channel for the denominator value
		 */
		template <unsigned int tChannels>
		static inline void addToFrameIndividually8BitPerChannel(const uint8_t* source, const uint8_t* mask, unsigned int* targetWithDenominator, const unsigned int width, const unsigned int height, Worker* worker = nullptr);

		/**
		 * Normalizes the pixels values of a given (accumulated frame) by a given denominator.
		 * @param source The accumulated source frame which has to be normalized, must be valid
		 * @param target The target frame which receives the normalized source frame, must be valid
		 * @param denominator The denominator which is used to normalize each frame pixel and channel of the source frame, with range [1, infinity)
		 * @param width The width of both frames in pixels, with range [1, infinity)
		 * @param height The height of both frames in pixels, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker An optional worker object to distribute the computation
		 * @tparam TSource The data type of the elements of the source frame
		 * @tparam TTarget The data type of the elements of the target frame
		 * @tparam tChannels The number of frame data channels, with range [1, infinity)
		 */
		template <typename TSource, typename TTarget, unsigned int tChannels>
		static inline void normalizeFrame(const TSource* const source, TTarget* const target, const TSource denominator, unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Normalizes the pixels values of a given (accumulated frame) by individual denominator values.
		 * The denominator values are provided as a second frame with corresponding pixel layout as the source frame.<br>
		 * @param source The accumulated source frame which has to be normalized
		 * @param denominators The normalization denominator frame individually for each pixel, with range [0, infinity)
		 * @param target The target frame which receives the normalized source frame
		 * @param width The width of both frames in pixels, with range [1, infinity)
		 * @param height The height of both frames in pixels, with range [1, infinity)
		 * @param zeroValue The target pixel value for pixels with denominator value equal to zero
		 * @param worker An optional worker object to distribute the computation
		 * @tparam tChannels The number of frame data channels
		 */
		template <unsigned int tChannels>
		static inline void normalizeFrameIndividually8BitPerChannel(const unsigned int* source, const unsigned int* denominators, uint8_t* target, const unsigned int width, const unsigned int height, const uint8_t* zeroValue = nullptr, Worker* worker = nullptr);

		/**
		 * Normalizes the pixels values of a given (accumulated frame) by individual denominator values.
		 * @param sourceWithDenominator The accumulated source frame together with the individual denominator values
		 * @param target The target frame which receives the normalized source frame
		 * @param width The width of both frames in pixels, with range [1, infinity)
		 * @param height The height of both frames in pixels, with range [1, infinity)
		 * @param zeroValue The target pixel value for pixels with denominator value equal to zero
		 * @param worker An optional worker object to distribute the computation
		 * @tparam tChannels The number of frame data channels, not including the extra channel for the denominator value
		 */
		template <unsigned int tChannels>
		static inline void normalizeFrameIndividually8BitPerChannel(const unsigned int* sourceWithDenominator, uint8_t* target, const unsigned int width, const unsigned int height, const uint8_t* zeroValue, Worker* worker = nullptr);

	protected:

		/**
		 * Sums the pixel values in a subset of a given frame.
		 * @param frame The data of the frame for which the summed pixel values will be determined, must be valid
		 * @param width The width of the given frame in pixel, with range [1, infinity)
		 * @param sumValues The resulting summed pixel values values, one for each channel
		 * @param lock Lock object which must be provided if this function is invoked in parallel; nullptr otherwise
		 * @param framePaddingElements The number of padding elements at the end of each image row, in elements, with range [0, infinity)
		 * @param firstRow The first row to be handled, with range [0, 'frame height')
		 * @param numberRows The number of rows to be handled, with range [1, 'frame height' - firstRow]
		 * @tparam T The data type of each pixel channel
		 * @tparam TSum Data type of each sum value
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <typename T, typename TSum, unsigned int tChannels>
		static void sumFrameSubset(const T* frame, const unsigned int width, TSum* sumValues, Lock* lock, const unsigned int framePaddingElements, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Adds the subset of individual pixel values of a given source frame to a target frame.
		 * @param source The source frame which pixels values are copied, must be valid
		 * @param target The target frame to which the source pixel values will be added, individually for each pixel and channel, must be valid
		 * @param width The width of both frames in pixels, with range [1, infinity)
		 * @param height The height of both frames in pixels, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param firstRow The first row to be handled, with range [0,  height - 1]
		 * @param numberRows The number of rows to be handled, with range [1, height - firstRow]
		 * @tparam TSource The data type of the elements of the source frame
		 * @tparam TTarget The data type of the elements of the target frame
		 * @tparam tChannels The number of frame data channels, with range [1, infinity)
		 */
		template <typename TSource, typename TTarget, unsigned int tChannels>
		static void addToFrameSubset(const TSource* const source, TTarget* const target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Adds the subset of individual pixel values of a given source frame to a target frame (with denominators as separate block) if the corresponding mask pixel is valid.
		 * @param source The source frame from which pixels values are copied
		 * @param mask The mask frame defining whether a source pixel is copied or not (and whether the corresponding normalization denominator of the target frame is increased by 1)
		 * @param target The target frame to which the source pixel values will be added
		 * @param denominators The denominator values individually for each pixel
		 * @param width The width of both frames in pixels
		 * @param height The height of both frames in pixels
		 * @param sourcePaddingElements The number of padding elements of the source frame, range: [0, infinity)
		 * @param maskPaddingElements The number of padding elements of the mask frame, range: [0, infinity)
		 * @param targetPaddingElements The number of padding elements of the target frame, range: [0, infinity)
		 * @param denominatorsPaddingElements The number of padding elements of the denominators frame, range: [0, infinity)
		 * @param nonMaskValue The value indicating invalid mask pixels, i.e. pixels that should not be processed by this function, all other values will be interpreted as valid, range: [0, 255]
		 * @param firstRow The first row to be handled
		 * @param numberRows The number of rows to be handled
		 * @tparam tChannels The number of frame data channels
		 */
		template <unsigned int tChannels>
		static void addToFrameIndividuallyBlock8BitPerChannelSubset(const uint8_t* source, const uint8_t* mask, unsigned int* target, unsigned int* denominators, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int maskPaddingElements, const unsigned int targetPaddingElements, const unsigned int denominatorsPaddingElements, const uint8_t nonMaskValue, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Adds the subset of individual pixel values of a given source frame to a target frame (with zipped denominator).
		 * @param source The source frame from which pixels values are copied
		 * @param targetWithDenominator The target frame to which the source pixel values will be added, with individual denominator values, the denominator of each pixel is increased by 1
		 * @param width The width of both frames in pixels
		 * @param height The height of both frames in pixels
		 * @param firstRow The first row to be handled
		 * @param numberRows The number of rows to be handled
		 * @tparam tChannels The number of frame data channels, not including the extra channel for the denominator value
		 */
		template <unsigned int tChannels>
		static void addToFrameIndividuallyZipped8BitPerChannelSubset(const uint8_t* source, unsigned int* targetWithDenominator, const unsigned int width, const unsigned int height, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Adds the subset of individual pixel values of a given source frame to a target frame (with zipped denominator) if the corresponding mask pixel is valid.
		 * @param source The source frame from which pixels values are copied
		 * @param mask The mask frame defining whether a source pixel is copied or not (and whether the corresponding normalization denominator of the target frame is increased by 1)
		 * @param targetWithDenominator The target frame to which the source pixel values will be added, with individual denominator values
		 * @param width The width of both frames in pixels
		 * @param height The height of both frames in pixels
		 * @param firstRow The first row to be handled
		 * @param numberRows The number of rows to be handled
		 * @tparam tChannels The number of frame data channels, not including the extra channel for the denominator value
		 */
		template <unsigned int tChannels>
		static void addToFrameIndividuallyZipped8BitPerChannelSubset(const uint8_t* source, const uint8_t* mask, unsigned int* targetWithDenominator, const unsigned int width, const unsigned int height, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Normalizes a subset of the pixels values of a given (accumulated frame) by a given denominator.
		 * @param source The accumulated source frame which has to be normalized, must be valid
		 * @param target The target frame which receives the normalized source frame, must be valid
		 * @param denominator The denominator which is used to normalize each frame pixel and channel of the source frame, with range [1, infinity)
		 * @param width The width of both frames in pixels, with range [1, infinity)
		 * @param height The height of both frames in pixels, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param firstRow The first row to be handled, with range [0, height - 1]
		 * @param numberRows The number of rows to be handled, with range [1, height - firstRow]
		 * @tparam TSource The data type of the elements of the source frame
		 * @tparam TTarget The data type of the elements of the target frame
		 * @tparam tChannels The number of frame data channels, with range [1, infinity)
		 */
		template <typename TSource, typename TTarget, unsigned int tChannels>
		static void normalizeFrameSubset(const TSource* const source, TTarget* const target, const TSource denominator, unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Normalizes a subset of the pixels values of a given (accumulated frame) by individual denominator values.
		 * @param source The accumulated source frame which has to be normalized
		 * @param denominators The normalization denominators individually for each pixel, with range [0, infinity)
		 * @param target The target frame which receives the normalized source frame
		 * @param width The width of both frames in pixels
		 * @param height The height of both frames in pixels
		 * @param zeroValue The target pixel value for pixels with denominator value equal to zero
		 * @param firstRow The first row to be handled
		 * @param numberRows The number of rows to be handled
		 * @tparam tChannels The number of frame data channels
		 */
		template <unsigned int tChannels>
		static void normalizeFrameIndividually8BitPerChannelSubset(const unsigned int* source, const unsigned int* denominators, uint8_t* target, const unsigned int width, const unsigned int height, const uint8_t* zeroValue, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Normalizes a subset of the pixels values of a given (accumulated frame) by individual denominator values.
		 * @param sourceWithDenominator The accumulated source frame together with the individual denominator values
		 * @param target The target frame which receives the normalized source frame
		 * @param width The width of both frames in pixels
		 * @param height The height of both frames in pixels
		 * @param zeroValue The target pixel value for pixels with denominator value equal to zero
		 * @param firstRow The first row to be handled
		 * @param numberRows The number of rows to be handled
		 * @tparam tChannels The number of frame data channels, not including the extra channel for the denominator value
		 */
		template <unsigned int tChannels>
		static void normalizeFrameIndividually8BitPerChannelSubset(const unsigned int* sourceWithDenominator, uint8_t* target, const unsigned int width, const unsigned int height, const uint8_t* zeroValue, const unsigned int firstRow, const unsigned int numberRows);
};

template <typename T, typename TMean, unsigned int tChannels>
void FrameMean::meanValue(const T* frame, const unsigned int width, const unsigned int height, TMean* meanValues, const unsigned int framePaddingElements, Worker* worker)
{
	static_assert(tChannels > 0u, "Invalid channel number!");

	ocean_assert(frame != nullptr && meanValues != nullptr);
	ocean_assert(width > 0u && height > 0u);

	typedef typename NextLargerTyper<T>::TypePerformance TSum;

	TSum sumValues[tChannels];
	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		sumValues[n] = TSum(0);
	}

	if (worker)
	{
		Lock lock;
		worker->executeFunction(Worker::Function::createStatic(sumFrameSubset<T, TSum, tChannels>, frame, width, sumValues, &lock, framePaddingElements, 0u, 0u), 0u, height, 5u, 6u, 40u);
	}
	else
	{
		sumFrameSubset<T, TSum, tChannels>(frame, width, sumValues, nullptr, framePaddingElements, 0u, height);
	}

	const unsigned int pixels = width * height;

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		// we apply a simple division for floating point values
		// and add a rounding for non floating point values

		if constexpr (std::is_same<float, TMean>::value || std::is_same<double, TMean>::value)
		{
			meanValues[n] = TMean(double(sumValues[n]) / double(pixels));
		}
		else
		{
			meanValues[n] = TMean((sumValues[n] + TSum(pixels) / TSum(2)) / TSum(pixels));
		}
	}
}

template <unsigned int tChannels>
Frame FrameMean::meanFrame8BitPerChannel(FrameProviderInterface& frameProviderInterface, const FrameType::PixelFormat& targetPixelFormat, const unsigned int firstFrameIndex, const unsigned int lastFrameIndex, Worker* worker, bool* abort)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(frameProviderInterface.isInitialized());
	ocean_assert(firstFrameIndex <= lastFrameIndex);

	ocean_assert(FrameType::channels(targetPixelFormat) == tChannels);

	const FrameType sourceFrameType = frameProviderInterface.synchronFrameTypeRequest(10, abort);
	ocean_assert(sourceFrameType.isValid());

	const unsigned int interfaceNumberFrames = frameProviderInterface.synchronFrameNumberRequest(10, abort);
	ocean_assert(lastFrameIndex < interfaceNumberFrames);

	const unsigned int meanFrames = lastFrameIndex - firstFrameIndex + 1u;
	ocean_assert(meanFrames >= 1u && meanFrames < 16843010u);

	const unsigned int width = sourceFrameType.width();
	const unsigned int height = sourceFrameType.height();

	const FrameType targetFrameType(sourceFrameType, targetPixelFormat);

	ocean_assert(width > 0u && height > 0u);
	if (width == 0u || height == 0u || meanFrames >= 16843010u || interfaceNumberFrames <= lastFrameIndex)
	{
		return Frame();
	}

	Frame accumulatedFrame(FrameType(width, height, FrameType::genericPixelFormat<uint32_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT));
	accumulatedFrame.setValue(0x00u);

	for (unsigned int n = firstFrameIndex; n <= lastFrameIndex; ++n)
	{
		const FrameRef frame(frameProviderInterface.synchronFrameRequest(n, 10, abort));

		if (n + 1u <= lastFrameIndex)
		{
			frameProviderInterface.frameCacheRequest(n + 1u, n + 1u);
		}

		if (frame.isNull() || !frame->isValid())
		{
			return Frame();
		}

		Frame targetFrame;

		if (!CV::FrameConverter::Comfort::convert(*frame, targetFrameType.pixelFormat(), targetFrameType.pixelOrigin(), targetFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
		{
			return Frame();
		}

		addToFrame<uint8_t, uint32_t, tChannels>(targetFrame.constdata<uint8_t>(), accumulatedFrame.data<uint32_t>(), width, height, targetFrame.paddingElements(), accumulatedFrame.paddingElements(), worker);
	}

	Frame result(targetFrameType);

	normalizeFrame<uint32_t, uint8_t, tChannels>(accumulatedFrame.constdata<uint32_t>(), result.data<uint8_t>(), meanFrames, width, height, accumulatedFrame.paddingElements(), result.paddingElements(), worker);

	return result;
}

template <typename TSource, typename TTarget, unsigned int tChannels>
inline void FrameMean::addToFrame(const TSource* const source, TTarget* const target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(sizeof(TSource) <= sizeof(TTarget), "Invalid data type!");
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width != 0u && height != 0u);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameMean::addToFrameSubset<TSource, TTarget, tChannels>, source, target, width, height, sourcePaddingElements, targetPaddingElements, 0u, 0u), 0u, height);
	}
	else
	{
		addToFrameSubset<TSource, TTarget, tChannels>(source, target, width, height, sourcePaddingElements, targetPaddingElements, 0u, height);
	}
}

template <unsigned int tChannels>
inline void FrameMean::addToFrameIndividually8BitPerChannel(const uint8_t* source, const uint8_t* mask, unsigned int* target, unsigned int* denominators, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int maskPaddingElements, const unsigned int targetPaddingElements, const unsigned int denominatorsPaddingElements, const uint8_t nonMaskValue, Worker* worker)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source && mask && target && denominators);
	ocean_assert(width != 0u && height != 0u);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameMean::addToFrameIndividuallyBlock8BitPerChannelSubset<tChannels>, source, mask, target, denominators, width, height, sourcePaddingElements, maskPaddingElements, targetPaddingElements, denominatorsPaddingElements, nonMaskValue, 0u, 0u), 0u, height);
	}
	else
	{
		addToFrameIndividuallyBlock8BitPerChannelSubset<tChannels>(source, mask, target, denominators, width, height, sourcePaddingElements, maskPaddingElements, targetPaddingElements, denominatorsPaddingElements, nonMaskValue, 0u, height);
	}
}

template <unsigned int tChannels>
inline void FrameMean::addToFrameIndividually8BitPerChannel(const uint8_t* source, const uint8_t* mask, unsigned int* targetWithDenominator, const unsigned int width, const unsigned int height, Worker* worker)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source && mask && targetWithDenominator);
	ocean_assert(width != 0u && height != 0u);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameMean::addToFrameIndividuallyZipped8BitPerChannelSubset<tChannels>, source, mask, targetWithDenominator, width, height, 0u, 0u), 0u, height, 5u, 6u);
	}
	else
	{
		addToFrameIndividuallyZipped8BitPerChannelSubset<tChannels>(source, mask, targetWithDenominator, width, height, 0u, height);
	}
}

template <unsigned int tChannels>
inline void FrameMean::addToFrameIndividually8BitPerChannel(const uint8_t* source, unsigned int* targetWithDenominator, const unsigned int width, const unsigned int height, Worker* worker)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source && targetWithDenominator);
	ocean_assert(width != 0u && height != 0u);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameMean::addToFrameIndividuallyZipped8BitPerChannelSubset<tChannels>, source, targetWithDenominator, width, height, 0u, 0u), 0u, height, 4u, 5u);
	}
	else
	{
		addToFrameIndividuallyZipped8BitPerChannelSubset<tChannels>(source, targetWithDenominator, width, height, 0u, height);
	}
}

template <typename TSource, typename TTarget, unsigned int tChannels>
inline void FrameMean::normalizeFrame(const TSource* const source, TTarget* const target, const TSource denominator, unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width != 0u && height != 0u);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameMean::normalizeFrameSubset<TSource, TTarget, tChannels>, source, target, denominator, width, height, sourcePaddingElements, targetPaddingElements, 0u, 0u), 0u, height);
	}
	else
	{
		normalizeFrameSubset<TSource, TTarget, tChannels>(source, target, denominator, width, height, sourcePaddingElements, targetPaddingElements, 0u, height);
	}
}

template <unsigned int tChannels>
inline void FrameMean::normalizeFrameIndividually8BitPerChannel(const unsigned int* source, const unsigned int* denominators, uint8_t* target, const unsigned int width, const unsigned int height, const uint8_t* zeroValue, Worker* worker)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source && denominators && target);
	ocean_assert(width != 0u && height != 0u);

	const uint8_t zeroPixel[tChannels] = {0u};

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameMean::normalizeFrameIndividually8BitPerChannelSubset<tChannels>, source, denominators, target, width, height, zeroValue ? zeroValue : zeroPixel, 0u, 0u), 0u, height, 6u, 7u);
	}
	else
	{
		normalizeFrameIndividually8BitPerChannelSubset<tChannels>(source, denominators, target, width, height, zeroValue ? zeroValue : zeroPixel, 0u, height);
	}
}

template <unsigned int tChannels>
inline void FrameMean::normalizeFrameIndividually8BitPerChannel(const unsigned int* sourceWithDenominator, uint8_t* target, const unsigned int width, const unsigned int height, const uint8_t* zeroValue, Worker* worker)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(sourceWithDenominator && target);
	ocean_assert(width != 0u && height != 0u);

	const uint8_t zeroPixel[tChannels] = {0u};

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameMean::normalizeFrameIndividually8BitPerChannelSubset<tChannels>, sourceWithDenominator, target, width, height, zeroValue ? zeroValue : zeroPixel, 0u, 0u), 0u, height, 5u, 6u);
	}
	else
	{
		normalizeFrameIndividually8BitPerChannelSubset<tChannels>(sourceWithDenominator, target, width, height, zeroValue ? zeroValue : zeroPixel, 0u, height);
	}
}

template <typename T, typename TSum, unsigned int tChannels>
void FrameMean::sumFrameSubset(const T* frame, const unsigned int width, TSum* sumValues, Lock* lock, const unsigned int framePaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(frame != nullptr && sumValues != nullptr);
	ocean_assert(width > 0u);

	TSum localSumValues[tChannels];

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		localSumValues[n] = TSum(0);
	}

	const unsigned int frameStrideElements = width * tChannels + framePaddingElements;

	frame += firstRow * frameStrideElements;

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		const T* frameRowEnd = frame + width * tChannels;

		while (frame != frameRowEnd)
		{
			ocean_assert(frame < frameRowEnd);

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
#ifdef OCEAN_DEBUG
				// checking for an overflow

				if (TSum(frame[n]) > TSum(0))
				{
					ocean_assert(NumericT<TSum>::maxValue() - TSum(frame[n]) > localSumValues[n]);
				}
				else
				{
					ocean_assert(NumericT<TSum>::minValue() - TSum(frame[n]) <= localSumValues[n]);
				}
#endif

				localSumValues[n] += TSum(frame[n]);
			}

			frame += tChannels;
		}

		frame += framePaddingElements;
	}

	const OptionalScopedLock scopedLock(lock);

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		sumValues[n] += localSumValues[n];
	}
}

template <typename TSource, typename TTarget, unsigned int tChannels>
void FrameMean::addToFrameSubset(const TSource* const source, TTarget* const target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(sizeof(TSource) <= sizeof(TTarget), "Invalid data type!");
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert_and_suppress_unused(firstRow + numberRows <= height, height);

	const unsigned int sourceStrideElements = width * tChannels + sourcePaddingElements;
	const unsigned int targetStrideElements = width * tChannels + targetPaddingElements;

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		const TSource* const sourceRow = source + y * sourceStrideElements;
		TTarget* const targetRow = target + y * targetStrideElements;

		for (unsigned int n = 0u; n < width * tChannels; ++n)
		{
			targetRow[n] = targetRow[n] + TTarget(sourceRow[n]);
		}
	}
}

template <unsigned int tChannels>
void FrameMean::addToFrameIndividuallyBlock8BitPerChannelSubset(const uint8_t* source, const uint8_t* mask, unsigned int* target, unsigned int* denominators, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int maskPaddingElements, const unsigned int targetPaddingElements, const unsigned int denominatorsPaddingElements, const uint8_t nonMaskValue, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source && mask && target && denominators);
	ocean_assert_and_suppress_unused(firstRow + numberRows <= height, height);

	const unsigned int sourceStrideElements = width * tChannels + sourcePaddingElements;
	const unsigned int maskStrideElements = width + maskPaddingElements;
	const unsigned int targetStrideElements = width * tChannels + targetPaddingElements;
	const unsigned int denominatorsStrideElements = width + denominatorsPaddingElements;

	target += firstRow * targetStrideElements;
	denominators += firstRow * denominatorsStrideElements;

	source += firstRow * sourceStrideElements;
	mask += firstRow * maskStrideElements;

	for (unsigned int rowIndex = firstRow; rowIndex < firstRow + numberRows; ++rowIndex)
	{
		const uint8_t* const sourceRowEnd = source + width * tChannels;

		while (source != sourceRowEnd)
		{
			ocean_assert(source < sourceRowEnd);

			if (*mask++ != nonMaskValue)
			{
				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					target[n] += source[n];
				}

				(*denominators)++;
			}

			target += tChannels;
			denominators++;
			source += tChannels;
		}

		target += targetPaddingElements;
		denominators += denominatorsPaddingElements;

		source += sourcePaddingElements;
		mask += maskPaddingElements;
	}
}

template <unsigned int tChannels>
void FrameMean::addToFrameIndividuallyZipped8BitPerChannelSubset(const uint8_t* source, unsigned int* targetWithDenominator, const unsigned int width, const unsigned int height, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source && targetWithDenominator);
	ocean_assert_and_suppress_unused(firstRow + numberRows <= height, height);

	targetWithDenominator += firstRow * width * (tChannels + 1u);

	source += firstRow * width * tChannels;

	const uint8_t* const sourceEnd = source + numberRows * width * tChannels;

	while (source != sourceEnd)
	{
		ocean_assert(source < sourceEnd);

		for (unsigned int n = 0u; n < tChannels; ++n)
			*targetWithDenominator++ += *source++;

		(*targetWithDenominator++)++;
	}
}

template <unsigned int tChannels>
void FrameMean::addToFrameIndividuallyZipped8BitPerChannelSubset(const uint8_t* source, const uint8_t* mask, unsigned int* targetWithDenominator, const unsigned int width, const unsigned int height, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source && mask && targetWithDenominator);
	ocean_assert_and_suppress_unused(firstRow + numberRows <= height, height);

	targetWithDenominator += firstRow * width * (tChannels + 1u);

	source += firstRow * width * tChannels;
	mask += firstRow * width;

	const uint8_t* const sourceEnd = source + numberRows * width * tChannels;

	while (source != sourceEnd)
	{
		ocean_assert(source < sourceEnd);

		if (*mask++)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				targetWithDenominator[n] += source[n];
			}

			targetWithDenominator[tChannels]++;
		}

		targetWithDenominator += tChannels + 1u;
		source += tChannels;
	}
}

template <typename TSource, typename TTarget, unsigned int tChannels>
void FrameMean::normalizeFrameSubset(const TSource* const source, TTarget* const target, const TSource denominator, unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(denominator != 0u);
	ocean_assert_and_suppress_unused(firstRow + numberRows <= height, height);

	const TSource denominator_2 = denominator / TSource(2);

	const unsigned int sourceStrideElements = width * tChannels + sourcePaddingElements;
	const unsigned int targetStrideElements = width * tChannels + targetPaddingElements;

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		const TSource* const sourceRow = source + y * sourceStrideElements;
		TTarget* const targetRow = target + y * targetStrideElements;

		for (unsigned int n = 0u; n < width * tChannels; ++n)
		{
			if (std::is_signed<TSource>::value)
			{
				targetRow[n] = TTarget(sourceRow[n] / denominator);
			}
			else
			{
				targetRow[n] = TTarget((sourceRow[n] + denominator_2) / denominator);
			}
		}
	}
}

template <unsigned int tChannels>
void FrameMean::normalizeFrameIndividually8BitPerChannelSubset(const unsigned int* source, const unsigned int* denominators, uint8_t* target, const unsigned int width, const unsigned int height, const uint8_t* zeroValue, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source && denominators && target && zeroValue);
	ocean_assert_and_suppress_unused(firstRow + numberRows <= height, height);

	typedef typename DataType<uint8_t, tChannels>::Type PixelType;

	target += firstRow * width * tChannels;
	denominators += firstRow * width;
	source += firstRow * width * tChannels;

	const unsigned int* const sourceEnd = source + numberRows * width * tChannels;

	while (source != sourceEnd)
	{
		ocean_assert(source < sourceEnd);

		if (*denominators)
		{
			const unsigned int denominator = *denominators;
			const unsigned int denominator_2 = denominator / 2u;

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				target[n] = (uint8_t)((source[n] + denominator_2) / denominator);
			}
		}
		else
		{
			*((PixelType*)target) = *((PixelType*)zeroValue);
		}

		target += tChannels;
		source += tChannels;

		denominators++;
	}
}

template <unsigned int tChannels>
void FrameMean::normalizeFrameIndividually8BitPerChannelSubset(const unsigned int* sourceWithDenominator, uint8_t* target, const unsigned int width, const unsigned int height, const uint8_t* zeroValue, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(sourceWithDenominator && target && zeroValue);
	ocean_assert_and_suppress_unused(firstRow + numberRows <= height, height);

	typedef typename DataType<uint8_t, tChannels>::Type PixelType;

	target += firstRow * width * tChannels;
	sourceWithDenominator += firstRow * width * (tChannels + 1u);

	const unsigned int* const sourceEnd = sourceWithDenominator + numberRows * width * (tChannels + 1u);

	while (sourceWithDenominator != sourceEnd)
	{
		ocean_assert(sourceWithDenominator < sourceEnd);

		if (sourceWithDenominator[tChannels])
		{
			const unsigned int denominator = sourceWithDenominator[tChannels];
			const unsigned int denominator_2 = denominator / 2u;

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				target[n] = (uint8_t)((sourceWithDenominator[n] + denominator_2) / denominator);
			}
		}
		else
		{
			*((PixelType*)target) = *((PixelType*)zeroValue);
		}

		target += tChannels;
		sourceWithDenominator += (tChannels + 1u);
	}
}

}

}

#endif // META_OCEAN_CV_FRAME_MEAN_H
