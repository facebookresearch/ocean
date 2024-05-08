/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_INVERTER_H
#define META_OCEAN_CV_FRAME_INVERTER_H

#include "ocean/cv/CV.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements a frame inverter inverting the internal frame data.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameInverter
{
	public:

		/**
		 * Inverts the pixel values of a given frame.
		 * @param source The source frame to be inverted
		 * @param target The target frame receiving the inverted pixel values
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool invert(const Frame& source, Frame& target, Worker* worker = nullptr);

		/**
		 * Inverts the pixel values of a given frame.
		 * @param frame The frame to be inverted
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool invert(Frame& frame, Worker* worker = nullptr);

		/**
		 * Inverts one channel of an 8 bit per channel frame.
		 * @param frame The frame in that one channel will be inverted
		 * @param width The width of the frame in pixel
		 * @param height The height of the frame in pixel
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannel Index of the channel that will be inverted, with range [0, tChannels)
		 * @tparam tChannels Number of data channels of the frames
		 */
		template <unsigned int tChannel, unsigned int tChannels>
		static inline void invert8BitChannel(uint8_t* frame, const unsigned int width, const unsigned int height, Worker* worker = nullptr);

		/**
		 * Inverts one channel of an 8 bit per channel frame.
		 * @param source The source frame data to be inverted
		 * @param target The target frame data receiving the inverted data
		 * @param width The width of the frame in pixel
		 * @param height The height of the frame in pixel
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannel Index of the channel that will be inverted, with range [0, tChannels)
		 * @tparam tChannels Number of data channels of the frames
		 */
		template <unsigned int tChannel, unsigned int tChannels>
		static inline void invert8BitChannel(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, Worker* worker = nullptr);

		/**
		 * Inverts several channels of an 8 bit per channel frame.
		 * @param frame The frame in that one channel will be inverted
		 * @param width The width of the frame in pixel
		 * @param height The height of the frame in pixel
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannelFirst Index of the first channel that will be inverted, with range [0, tChannels)
		 * @tparam tChannelLast Index of the last (including) channel that will be inverted, with range [tChannelFirst, tChannels)
		 * @tparam tChannels Number of data channels of the frames
		 */
		template <unsigned int tChannelFirst, unsigned int tChannelLast, unsigned int tChannels>
		static inline void invert8BitChannels(uint8_t* frame, const unsigned int width, const unsigned int height, Worker* worker = nullptr);

		/**
		 * Inverts several channels of an 8 bit per channel frame.
		 * @param source The source frame data to be inverted
		 * @param target The target frame data receiving the inverted data
		 * @param width The width of the frame in pixel
		 * @param height The height of the frame in pixel
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannelFirst Index of the first channel that will be inverted, with range [0, tChannels)
		 * @tparam tChannelLast Index of the last (including) channel that will be inverted, with range [tChannelFirst, tChannels)
		 * @tparam tChannels Number of data channels of the frames
		 */
		template <unsigned int tChannelFirst, unsigned int tChannelLast, unsigned int tChannels>
		static inline void invert8BitChannels(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, Worker* worker = nullptr);

		/**
		 * Inverts an 8 bit per channel frame.
		 * @param source The source frame data to be inverted, must be valid
		 * @param target The target frame data receiving the inverted data, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param channels Number of data channels of both frames, with range [1, infinity)
		 * @param sourcePaddingElements Optional number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 */
		static inline void invert8BitPerChannel(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

	protected:

		/**
		 * Inverts one channel of a subset of an 8 bit per channel frame.
		 * @param frame The frame in that one channel will be inverted
		 * @param width The width of the frame in pixel
		 * @param height The height of the frame in pixel
		 * @param firstRow First row to be handled
		 * @param numberRows Number of rows to be handled
		 * @tparam tChannel Index of the channel that will be inverted, with range [0, tChannels)
		 * @tparam tChannels Number of data channels of the frames
		 */
		template <unsigned int tChannel, unsigned int tChannels>
		static void invert8BitChannelSubset(uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Inverts one channel of a subset of an 8 bit per channel frame.
		 * @param source The source frame data to be inverted
		 * @param target The target frame data receiving the inverted data
		 * @param width The width of the frame in pixel
		 * @param height The height of the frame in pixel
		 * @param firstRow First row to be handled
		 * @param numberRows Number of rows to be handled
		 * @tparam tChannel Index of the channel that will be inverted, with range [0, tChannels)
		 * @tparam tChannels Number of data channels of the frames
		 */
		template <unsigned int tChannel, unsigned int tChannels>
		static void invert8BitChannelSubset(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Inverts several channels of a subset of an 8 bit per channel frame.
		 * @param frame The frame in that one channel will be inverted
		 * @param width The width of the frame in pixel
		 * @param height The height of the frame in pixel
		 * @param firstRow First row to be handled
		 * @param numberRows Number of rows to be handled
		 * @tparam tChannelFirst Index of the first channel that will be inverted, with range [0, tChannels)
		 * @tparam tChannelLast Index of the last (including) channel that will be inverted, with range [tChannelFirst, tChannels)
		 * @tparam tChannels Number of data channels of the frames
		 */
		template <unsigned int tChannelFirst, unsigned int tChannelLast, unsigned int tChannels>
		static void invert8BitChannelsSubset(uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Inverts several channels of a subset of an 8 bit per channel frame.
		 * @param source The source frame data to be inverted
		 * @param target The target frame data receiving the inverted data
		 * @param width The width of the frame in pixel
		 * @param height The height of the frame in pixel
		 * @param firstRow First row to be handled
		 * @param numberRows Number of rows to be handled
		 * @tparam tChannelFirst Index of the first channel that will be inverted, with range [0, tChannels)
		 * @tparam tChannelLast Index of the last (including) channel that will be inverted, with range [tChannelFirst, tChannels)
		 * @tparam tChannels Number of data channels of the frames
		 */
		template <unsigned int tChannelFirst, unsigned int tChannelLast, unsigned int tChannels>
		static void invert8BitChannelsSubset(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Inverts the pixel values of a subset of a given frame.
		 * @param source The source frame to be inverted, must be valid
		 * @param target The target frame receiving the inverted pixel values, must be valid
		 * @param horizontalElements The number of horizontal elements to be inverted (which is width * channels), with range [1, infinity)
		 * @param sourcePaddingElements Optional number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param firstRow The first row to be handled, with range [0, height - 1]
		 * @param numberRows The number of rows to be handled, with range [1, height - firstRow]
		 */
		static void invert8BitPerChannelSubset(const uint8_t* source, uint8_t* target, const unsigned int horizontalElements, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows);

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

		/**
		 * Inverts the pixel values of a subset of a given frame.
		 * @param source The source frame to be inverted, must be valid
		 * @param target The target frame receiving the inverted pixel values, must be valid
		 * @param horizontalElements The number of horizontal elements to be inverted (which is width * channels), with range [16, infinity)
		 * @param sourcePaddingElements Optional number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param firstRow The first row to be handled, with range [0, height - 1]
		 * @param numberRows The number of rows to be handled, with range [1, height - firstRow]
		 */
		static void invert8BitPerChannelSubsetNEON(const uint8_t* source, uint8_t* target, const unsigned int horizontalElements, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows);

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

};

template <unsigned int tChannel, unsigned int tChannels>
inline void FrameInverter::invert8BitChannel(uint8_t* frame, const unsigned int width, const unsigned int height, Worker* worker)
{
	ocean_assert(frame);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&invert8BitChannelSubset<tChannel, tChannels>, frame, width, height, 0u, 0u), 0u, height);
	}
	else
	{
		invert8BitChannelSubset<tChannel, tChannels>(frame, width, height, 0u, height);
	}
}

template <unsigned int tChannel, unsigned int tChannels>
inline void FrameInverter::invert8BitChannel(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, Worker* worker)
{
	ocean_assert(source && target);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&invert8BitChannelSubset<tChannel, tChannels>, source, target, width, height, 0u, 0u), 0u, height);
	}
	else
	{
		invert8BitChannelSubset<tChannel, tChannels>(source, target, width, height, 0u, height);
	}
}

template <unsigned int tChannelFirst, unsigned int tChannelLast, unsigned int tChannels>
inline void FrameInverter::invert8BitChannels(uint8_t* frame, const unsigned int width, const unsigned int height, Worker* worker)
{
	ocean_assert(frame);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&invert8BitChannelsSubset<tChannelFirst, tChannelLast, tChannels>, frame, width, height, 0u, 0u), 0u, height);
	}
	else
	{
		invert8BitChannelsSubset<tChannelFirst, tChannelLast, tChannels>(frame, width, height, 0u, height);
	}
}

template <unsigned int tChannelFirst, unsigned int tChannelLast, unsigned int tChannels>
inline void FrameInverter::invert8BitChannels(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, Worker* worker)
{
	ocean_assert(source && target);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&invert8BitChannelsSubset<tChannelFirst, tChannelLast, tChannels>, source, target, width, height, 0u, 0u), 0u, height);
	}
	else
	{
		invert8BitChannelsSubset<tChannelFirst, tChannelLast, tChannels>(source, target, width, height, 0u, height);
	}
}

inline void FrameInverter::invert8BitPerChannel(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(channels != 0u);

	const unsigned int horizontalElements = width * channels;

	if (worker && width * height > 1920u * 1080u)
	{
#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
		if (width * channels >= 32u)
		{
			worker->executeFunction(Worker::Function::createStatic(&FrameInverter::invert8BitPerChannelSubsetNEON, source, target, horizontalElements, sourcePaddingElements, targetPaddingElements, 0u, 0u), 0u, height, 5u, 6u, 20u);
			return;
		}
#endif

		worker->executeFunction(Worker::Function::createStatic(&FrameInverter::invert8BitPerChannelSubset, source, target, horizontalElements, sourcePaddingElements, targetPaddingElements, 0u, 0u), 0u, height, 5u, 6u, 20u);
	}
	else
	{
#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
		if (width * channels >= 32u)
		{
			invert8BitPerChannelSubsetNEON(source, target, horizontalElements, sourcePaddingElements, targetPaddingElements, 0u, height);
			return;
		}
#endif

		invert8BitPerChannelSubset(source, target, horizontalElements, sourcePaddingElements, targetPaddingElements, 0u, height);
	}
}

template <unsigned int tChannel, unsigned int tChannels>
void FrameInverter::invert8BitChannelSubset(uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tChannel < tChannels, "Invalid channel index!");

	ocean_assert(frame != nullptr);
	ocean_assert_and_suppress_unused(firstRow + numberRows <= height, height);

	frame += firstRow * width * tChannels + tChannel;

	uint8_t* const frameEnd = frame + numberRows * width * tChannels;

	while (frame != frameEnd)
	{
		ocean_assert(frame < frameEnd);

		*frame = 0xFF - *frame;
		frame += tChannels;
	}
}

template <unsigned int tChannel, unsigned int tChannels>
void FrameInverter::invert8BitChannelSubset(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tChannel < tChannels, "Invalid channel index!");

	ocean_assert(source && target);
	ocean_assert_and_suppress_unused(firstRow + numberRows <= height, height);

	source += firstRow * width * tChannels + tChannel;
	target += firstRow * width * tChannels + tChannel;

	const uint8_t* const sourceEnd = source + numberRows * width * tChannels;

	while (source != sourceEnd)
	{
		ocean_assert(source < sourceEnd);

		*target = 0xFF - *source;

		source += tChannels;
		target += tChannels;
	}
}

template <unsigned int tChannelFirst, unsigned int tChannelLast, unsigned int tChannels>
void FrameInverter::invert8BitChannelsSubset(uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tChannelFirst < tChannels, "Invalid channel index!");
	static_assert(tChannelFirst <= tChannelLast, "Invalid channel index!");
	static_assert(tChannelLast < tChannels, "Invalid channel index!");

	ocean_assert(frame);
	ocean_assert_and_suppress_unused(firstRow + numberRows <= height, height);

	frame += firstRow * width * tChannels + tChannelFirst;

	uint8_t* const frameEnd = frame + numberRows * width * tChannels;

	while (frame != frameEnd)
	{
		ocean_assert(frame < frameEnd);

		for (unsigned int n = 0u; n < tChannelLast - tChannelFirst + 1u; ++n)
			frame[n] = 0xFF - frame[n];

		frame += tChannels;
	}
}

template <unsigned int tChannelFirst, unsigned int tChannelLast, unsigned int tChannels>
void FrameInverter::invert8BitChannelsSubset(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tChannelFirst < tChannels, "Invalid channel index!");
	static_assert(tChannelFirst <= tChannelLast, "Invalid channel index!");
	static_assert(tChannelLast < tChannels, "Invalid channel index!");

	ocean_assert(source && target);
	ocean_assert_and_suppress_unused(firstRow + numberRows <= height, height);

	source += firstRow * width * tChannels + tChannelFirst;
	target += firstRow * width * tChannels + tChannelFirst;

	const uint8_t* const sourceEnd = source + numberRows * width * tChannels;

	while (source != sourceEnd)
	{
		ocean_assert(source < sourceEnd);

		for (unsigned int n = 0u; n < tChannelLast - tChannelFirst + 1u; ++n)
			target[n] = 0xFF - source[n];

		source += tChannels;
		target += tChannels;
	}
}

}

}

#endif // META_OCEAN_CV_FRAME_INVERTER_H
