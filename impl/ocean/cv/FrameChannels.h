/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_CHANNELS_H
#define META_OCEAN_CV_FRAME_CHANNELS_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/NEON.h"
#include "ocean/cv/SSE.h"

#include "ocean/base/DataType.h"
#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements frame channel conversion, transformation and extraction functions.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameChannels : public FrameConverter
{
	public:

		/**
		 * Definition of a constant to specify that the number of channels are not known at compile time but at runtime only.
		 */
		static constexpr unsigned int CHANNELS_NOT_KNOWN_AT_COMPILE_TIME = 0u;

		/**
		 * Definition of a function pointer to a function able to operate on an entire image row.
		 */
		template <typename TSource, typename TTarget, unsigned int tSourceChannels, unsigned int tTargetChannels>
		using RowOperatorFunction = void(*)(const TSource* sourceRow, TTarget* targetRow, const unsigned int width, const unsigned int height, unsigned int rowIndex, const unsigned int sourceStrideElements, const unsigned int targetStrideElements);

		/**
		 * The following comfort class provides comfortable functions simplifying prototyping applications but also increasing binary size of the resulting applications.
		 * Best practice is to avoid using these functions if binary size matters,<br>
		 * as for every comfort function a corresponding function exists with specialized functionality not increasing binary size significantly.<br>
		 */
		class OCEAN_CV_EXPORT Comfort
		{
			public:

				/**
				 * Separates a given frame with zipped pixel format e.g., FORMAT_RGB24, FORMAT_YUV24, FORMAT_BGRA32 into individual frames with one channel only.
				 * Usage:
				 * @code
				 * Frame rgbSourceFrame = ...;
				 *
				 * Frames targetFrames;
				 *
				 * if (separateTo1Channel(rgbSourceFrame, targetFrames))
				 * {
				 *     ocean_assert(targetFrames.size() == 3);
				 *
				 *     // do something with targetFrames
				 * }
				 * @endcode
				 * @param sourceFrame The frame to be separated, must be valid
				 * @param targetFrames The resulting frames each holding one channel of the source frame, will be set automatically
				 * @param targetPixelFormat Optional explicit pixel format of the target frames, must bet a pixel format with 1 channel and must fit with the data type of the source pixel format, otherwise FORMAT_UNDEFINED
				 * @return True, if succeeded
				 */
				static bool separateTo1Channel(const Frame& sourceFrame, Frames& targetFrames, const FrameType::PixelFormat targetPixelFormat = FrameType::FORMAT_UNDEFINED);

				/**
				 * Separates a given frame with zipped pixel format e.g., FORMAT_RGB24, FORMAT_YUV24, FORMAT_BGRA32 into individual frames with one channel only.
				 * Usage:
				 * @code
				 * Frame rgbSourceFrame = ...;
				 *
				 * Frame targetFrameA;
				 * Frame targetFrameB;
				 * Frame targetFrameC;
				 *
				 * if (separateTo1Channel(rgbSourceFrame, {&targetFrameA, &targetFrameB, &targetFrameC}))
				 * {
				 *     // do something with targetFrames
				 * }
				 * @endcode
				 * @param sourceFrame The frame to be separated, must be valid
				 * @param targetFrames The resulting frames each holding one channel of the source frame, one for each source channels
				 * @param targetPixelFormat Optional explicit pixel format of the target frames, must bet a pixel format with 1 channel and must fit with the data type of the source pixel format, otherwise FORMAT_UNDEFINED
				 * @return True, if succeeded
				 */
				static bool separateTo1Channel(const Frame& sourceFrame, const std::initializer_list<Frame*>& targetFrames, const FrameType::PixelFormat targetPixelFormat = FrameType::FORMAT_UNDEFINED);

				/**
				 * Zips/interleaves 1-channel images into one image with n-channels.
				 * Usage:
				 * @code
				 * Frame sourceFrameA = ...;
				 * Frame sourceFrameB = ...;
				 * Frame sourceFrameC = ...;
				 *
				 * Frame targetFrame;
				 * if (zipChannels({sourceFrameA, sourceFrameB, sourceFrameC}, targetFrame))
				 * {
				 *     ocean_assert(targetFrame.channels() == 3u);
				 *
				 *     // do something with targetFrame
				 * }
				 * @endcode
				 * @param sourceFrames The frames to be zipped/interleaved, must be valid
				 * @param targetFrame The resulting frame holding n channels, will be set automatically
				 * @param targetPixelFormat Optional explicit pixel format of the target frames, must bet a pixel format with 1 channel and must fit with the data type of the source pixel format, otherwise FORMAT_UNDEFINED
				 * @return True, if succeeded
				 */
				static bool zipChannels(const std::initializer_list<Frame>& sourceFrames, Frame& targetFrame, const FrameType::PixelFormat targetPixelFormat = FrameType::FORMAT_UNDEFINED);

				/**
				 * Zips/interleaves 1-channel images into one image with n-channels.
				 * Usage:
				 * @code
				 * Frames sourceFrames = ...;
				 *
				 * Frame targetFrame;
				 * if (zipChannels(sourceFrames, targetFrame))
				 * {
				 *     ocean_assert(targetFrame.channels() == sourceFrames.size());
				 *
				 *     // do something with targetFrame
				 * }
				 * @endcode
				 * @param sourceFrames The frames to be zipped/interleaved, must be valid
				 * @param targetFrame The resulting frame holding n channels, will be set automatically
				 * @param targetPixelFormat Optional explicit pixel format of the target frames, must bet a pixel format with 1 channel and must fit with the data type of the source pixel format, otherwise FORMAT_UNDEFINED
				 * @return True, if succeeded
				 */
				static bool zipChannels(const Frames& sourceFrames, Frame& targetFrame, const FrameType::PixelFormat targetPixelFormat = FrameType::FORMAT_UNDEFINED);

				/**
				 * Converts an image with premultiplied alpha to a straight image (without premultiplied alpha).
				 * @param frame The image to convert, must be valid
				 * @param worker Optional worker object to distribute the computation
				 * @return True, if succeeded
				 * @see straightAlphaToPremultipliedAlpha().
				 */
				static bool premultipliedAlphaToStraightAlpha(Frame& frame, Worker* worker = nullptr);

				/**
				 * Converts an image with premultiplied alpha to a straight image (without premultiplied alpha).
				 * @param source The source image to convert, must be valid
				 * @param target The resulting converted target image, the frame type will be changed if it is not match to the source frame
				 * @param worker Optional worker object to distribute the computation
				 * @return True, if succeeded
				 * @see straightAlphaToPremultipliedAlpha().
				 */
				static bool premultipliedAlphaToStraightAlpha(const Frame& source, Frame& target, Worker* worker = nullptr);

				/**
				 * Converts an image with straight alpha (without premultiplied alpha) to an image with premultiplied alpha.
				 * @param frame The image to convert, must be valid
				 * @param worker Optional worker object to distribute the computation
				 * @see premultipliedAlphaToStraightAlpha().
				 */
				static bool straightAlphaToPremultipliedAlpha(Frame& frame, Worker* worker = nullptr);

				/**
				 * Converts an image with straight alpha (without premultiplied alpha) to an image with premultiplied alpha.
				 * @param source The source image to convert, must be valid
				 * @param target The resulting converted target image, must be valid
				 * @param worker Optional worker object to distribute the computation
				 * @see premultipliedAlphaToStraightAlpha().
				 */
				static bool straightAlphaToPremultipliedAlpha(const Frame& source, Frame& target, Worker* worker = nullptr);
		};

		/**
		 * Separates a given frame with zipped pixel format e.g., FORMAT_RGB24, FORMAT_YUV24, FORMAT_BGRA32 into individual frames with one channel only.
		 * Usage:
		 * @code
		 * const unsigned int width = ...;
		 * const unsigned int height = ...;
		 *
		 * uint8_t* sourceFrame = ...;
		 * const unsigned int sourceFramePaddingElements = ...;
		 *
		 * constexpr unsigned int channels = 2u;
		 *
		 * const uint8_t* targetFrames[channels] = {..., ...};
		 * const unsigned int targetFramesPaddingElements[2] = {..., ...};
		 *
		 * separateTo1Channel<uint8_t, uint8_t, channels>(sourceFrame, targetFrames, width, height, channels, sourceFramePaddingElements, targetFramesPaddingElements);
		 * @endcode
		 * @param sourceFrame The frame to be separated, must be valid
		 * @param targetFrames The pointers to the resulting separated frames each holding one channel of the source frame, with already allocated memory
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param channels The number of channels the source frame has, with range [1, infinity)
		 * @param sourceFramePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetFramesPaddingElements The array of padding elements at the end of each target row, one for each target frame, in elements, with range [0, infinity), nullptr if all are zero
		 * @tparam TSource The data type of each source pixel channel, e.g., 'uint8_t', 'float', ...
		 * @tparam TTarget The data type of each target pixel channel, e.g., 'uint8_t', 'float', ...
		 * @tparam tChannels The number of source frames (and target channels) if known at compile time; otherwise CHANNELS_NOT_KNOWN_AT_COMPILE_TIME == 0, if know at compile time must be identical with 'channels'
		 */
		template <typename TSource, typename TTarget, unsigned int tChannels = CHANNELS_NOT_KNOWN_AT_COMPILE_TIME>
		static void separateTo1Channel(const TSource* const sourceFrame, TTarget* const* const targetFrames, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int sourceFramePaddingElements, const unsigned int* targetFramesPaddingElements);

		/**
		 * Separates a given frame with zipped pixel format e.g., FORMAT_RGB24, FORMAT_YUV24, FORMAT_BGRA32 into individual frames with one channel only.
		 * Usage:
		 * @code
		 * const unsigned int width = ...;
		 * const unsigned int height = ...;
		 *
		 * const uint8_t* sourceFrame = ...;
		 * const unsigned int sourceFramePaddingElements = ...;
		 *
		 * uint8_t* targetFrame0 = ...;
		 * uint8_t* targetFrame1 = ...;
		 * const unsigned int targetFramePaddingElements0 = ...;
		 * const unsigned int targetFramePaddingElements1 = ...;
		 *
		 * separateTo1Channel<uint8_t, uint8_t>(sourceFrame, {targetFrame0, targetFrame1}, width, height, sourceFramePaddingElements, {targetFramePaddingElements0, targetFramePaddingElements1});
		 * @endcode
		 * @param sourceFrame The frame to be separated, must be valid
		 * @param targetFrames The pointers to the resulting separated frames each holding one channel of the source frame, with already allocated memory
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param sourceFramePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetFramesPaddingElements The array of padding elements at the end of each target row, one for each target frame, in elements, with range [0, infinity)
		 * @tparam TSource The data type of each source pixel channel, e.g., 'uint8_t', 'float', ...
		 * @tparam TTarget The data type of each target pixel channel, e.g., 'uint8_t', 'float', ...
		 */
		template <typename TSource, typename TTarget>
		static void separateTo1Channel(const TSource* const sourceFrame, const std::initializer_list<TTarget*>& targetFrames, const unsigned int width, const unsigned int height, const unsigned int sourceFramePaddingElements, const std::initializer_list<const unsigned int>& targetFramesPaddingElements);

		/**
		 * Zips/interleaves 1-channel images into one image with n-channels.
		 * Usage:
		 * @code
		 * const unsigned int width = ...;
		 * const unsigned int height = ...;
		 *
		 * const uint8_t* sourceFrames[2] = {..., ...};
		 * const unsigned int sourceFramesPaddingElements[2] = {..., ...};
		 *
		 * uint8_t* targetFrame = ...;
		 * const unsigned int targetFramePaddingElements = ...;
		 *
		 * zipChannels<uint8_t, uint8_t>(sourceFrames, targetFrame, width, height, 2u, sourceFramesPaddingElements, targetFramePaddingElements);
		 * @endcode
		 * @param sourceFrames The pointers to the individual 1-channel frames, one for each image, must be valid
		 * @param targetFrame The pointer to the resulting zipped frame holding n-channels, must be valid
		 * @param width The width of the source frames in pixel, with range [1, infinity)
		 * @param height The height of the source frames in pixel, with range [1, infinity)
		 * @param channels The number of provided source frames (and the number of channels the target frame will have), with range [1, infinity)
		 * @param sourceFramesPaddingElements The array of padding elements at the end of each source row, one for each source frame, in elements, with range [0, infinity), nullptr if all are zero
		 * @param targetFramePaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @tparam TSource The data type of each source pixel channel, e.g., 'uint8_t', 'float', ...
		 * @tparam TTarget The data type of each target pixel channel, e.g., 'uint8_t', 'float', ...
		 * @tparam tChannels The number of source frames (and target channels) if known at compile time; otherwise CHANNELS_NOT_KNOWN_AT_COMPILE_TIME == 0, if know at compile time must be identical with 'channels'
		 */
		template <typename TSource, typename TTarget, unsigned int tChannels = CHANNELS_NOT_KNOWN_AT_COMPILE_TIME>
		static void zipChannels(const TSource* const* const sourceFrames, TTarget* const targetFrame, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int* sourceFramesPaddingElements, const unsigned int targetFramePaddingElements);

		/**
		 * Zips/interleaves 1-channel images into one image with n-channels.
		 * Usage:
		 * @code
		 * const unsigned int width = ...;
		 * const unsigned int height = ...;
		 *
		 * const uint8_t* sourceFrame0 = ...;
		 * const uint8_t* sourceFrame1 = ...;
		 * const unsigned int sourceFramePaddingElements0 = ...;
		 * const unsigned int sourceFramePaddingElements1 = ...;
		 *
		 * uint8_t* targetFrame = ...;
		 * const unsigned int targetFramePaddingElements = ...;
		 *
		 * zipChannels<uint8_t, uint8_t>({sourceFrame0, sourceFrame1}, targetFrame, width, height, {sourceFramePaddingElements0, sourceFramePaddingElements1}, targetFramePaddingElements);
		 * @endcode
		 * @param sourceFrames The pointers to the individual 1-channel frames, one for each image, must be valid
		 * @param targetFrame The pointer to the resulting zipped frame holding n-channels, must be valid
		 * @param width The width of the source frames in pixel, with range [1, infinity)
		 * @param height The height of the source frames in pixel, with range [1, infinity)
		 * @param sourceFramesPaddingElements The array of padding elements at the end of each source row, one for each source frame, in elements, with range [0, infinity)
		 * @param targetFramePaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @tparam TSource The data type of each source pixel channel, e.g., 'uint8_t', 'float', ...
		 * @tparam TTarget The data type of each target pixel channel, e.g., 'uint8_t', 'float', ...
		 */
		template <typename TSource, typename TTarget>
		static void zipChannels(const std::initializer_list<const TSource*>& sourceFrames, TTarget* const targetFrame, const unsigned int width, const unsigned int height, const std::initializer_list<unsigned int>& sourceFramesPaddingElements, const unsigned int targetFramePaddingElements);

		/**
		 * Adds a new channel to a given frame with zipped pixel format, the new channel will be added to the front of all existing channels.
		 * @param source The source frame to which the new channel will be added, must be valid
		 * @param sourceNewChannel The 1-channel frame providing the new channel information, must be valid
		 * @param target The target frame receiving the joined channels, must be valid
		 * @param width The width of the frames in pixel, with range [1, infinity)
		 * @param height The height of the frames in pixel, with range [1, infinity)
		 * @param conversionFlag The conversion to be applied
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param sourceNewChannelPaddingElements The number of padding elements at the end of each new-channel-source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 * @tparam T Data type of each channel pixel value
		 * @tparam tSourceChannels Number of channels of the source frame (without the new channel), with range [1, infinity)
		 */
		template <typename T, unsigned int tSourceChannels>
		static inline void addFirstChannel(const T* source, const T* sourceNewChannel, T* target, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const unsigned int sourcePaddingElements, const unsigned int sourceNewChannelPaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Adds a new channel to a given frame with zipped pixel format, the value of the new channel will be the same for each pixel.
		 * @param source The source frame that provided the existing channels
		 * @param newChannelValue Value that will be assigned to the new channel for each pixel
		 * @param target The target frame to that the existing channels and the new channel will be added (as new first channel)
		 * @param width The width of the frames in pixel, with range [1, infinity)
		 * @param height The height of the frames in pixel, with range [1, infinity)
		 * @param conversionFlag The conversion to be applied
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 * @tparam T Data type of each channel pixel value
		 * @tparam tSourceChannels Number of channels of the source frame (without the new channel)
		 */
		template <typename T, unsigned int tSourceChannels>
		static inline void addFirstChannelValue(const T* source, const T newChannelValue, T* target, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Adds a new channel to a given frame with zipped pixel format, the new channel will be added to the back of all existing channels.
		 * @param source The source frame to which the new channel will be added, must be valid
		 * @param sourceNewChannel The 1-channel frame providing the new channel information, must be valid
		 * @param target The target frame receiving the joined channels, must be valid
		 * @param width The width of the frames in pixel, with range [1, infinity)
		 * @param height The height of the frames in pixel, with range [1, infinity)
		 * @param conversionFlag The conversion to be applied
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param sourceNewChannelPaddingElements The number of padding elements at the end of each new-channel-source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 * @tparam T Data type of each channel pixel value
		 * @tparam tSourceChannels Number of channels of the source frame (without the new channel), with range [1, infinity)
		 */
		template <typename T, unsigned int tSourceChannels>
		static inline void addLastChannel(const T* source, const T* sourceNewChannel, T* target, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const unsigned int sourcePaddingElements, const unsigned int sourceNewChannelPaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Adds a new channel to a given frame with zipped pixel format, the value of the new channel will be the same for each pixel.
		 * @param source The source frame that provided the existing channels
		 * @param newChannelValue Value that will be assigned to the new channel for each pixel
		 * @param target The target frame to that the existing channels and the new channel will be added (as new last channel)
		 * @param width The width of the frames in pixel, with range [1, infinity)
		 * @param height The height of the frames in pixel, with range [1, infinity)
		 * @param conversionFlag The conversion to be applied
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 * @tparam T Data type of each channel pixel value
		 * @tparam tSourceChannels Number of channels of the source frame (without the new channel)
		 */
		template <typename T, unsigned int tSourceChannels>
		static inline void addLastChannelValue(const T* source, const T newChannelValue, T* target, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Removes the first channel from a given frame with zipped (generic) pixel format.
		 * This function is mainly a wrapper around FrameChannels::shuffleChannels().
		 * @param source The source frame from that the first channel will be removed, must be valid
		 * @param target The target frame without the first channel, must be valid
		 * @param width The width of the frames in pixel, with range [1, infinity)
		 * @param height The height of the frames in pixel, with range [1, infinity)
		 * @param conversionFlag The conversion to be applied
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 * @tparam T Data type of each channel pixel value
		 * @tparam tSourceChannels Number of channels of the source frame (including the channel that will be removed), with range [2, infinity)
		 * @see FrameChannels::shuffleChannels<T, tSourceChannels, tTargetChannels, tShufflePattern>(), removeLastChannel().
		 */
		template <typename T, unsigned int tSourceChannels>
		static inline void removeFirstChannel(const T* source, T* target, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Removes the last channel from a given frame with zipped (generic) pixel format.
		 * This function is mainly a wrapper around FrameChannels::shuffleChannels().
		 * @param source The source frame from that the first channel will be removed, must be valid
		 * @param target The target frame without the first channel, must be valid
		 * @param width The width of the frames in pixel, with range [1, infinity)
		 * @param height The height of the frames in pixel, with range [1, infinity)
		 * @param conversionFlag The conversion to be applied
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 * @tparam T Data type of each channel pixel value
		 * @tparam tSourceChannels Number of channels of the frame (including the channel that will be removed), with range [2, infinity)
		 * @see FrameChannels::shuffleChannels<T, tSourceChannels, tTargetChannels, tShufflePattern>(), removeFirstChannel().
		 */
		template <typename T, unsigned int tSourceChannels>
		static inline void removeLastChannel(const T* source, T* target, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Copies one channel from a given frame with zipped pixel format to another frame with zipped pixel format.
		 * @param source The source frame from that the channel will be copied, must be valid
		 * @param target The target frame to which the channel will be copied, must be valid
		 * @param width The width of both frames in pixel, with range [1, infinity)
		 * @param height The height of both frames in pixel, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computational load
		 * @tparam T Data type of each channel pixel value
		 * @tparam tSourceChannels Number of channels in the source frame, with range [1, infinity)
		 * @tparam tTargetChannels Number of channels in the target frame, with range [1, infinity)
		 * @tparam tSourceChannelIndex The index of the source channel that will be copied, with range [0, tSourceChannels - 1]
		 * @tparam tTargetChannelIndex The index of the target channel that will be copied, with range [0, tTargetChannels - 1]
		 */
		template <typename T, unsigned int tSourceChannels, unsigned int tTargetChannels, unsigned int tSourceChannelIndex, unsigned int tTargetChannelIndex>
		static inline void copyChannel(const T* source, T* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Sets one channel of a frame with a specific unique value.
		 * @param frame The frame in that one channel of each pixel will be set
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param value The value to be set
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam T Data type of each channel pixel value
		 * @tparam tChannel Index of the channel that will be inverted, with range [0, tChannels)
		 * @tparam tChannels Number of data channels of the frames, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannel, unsigned int tChannels>
		static inline void setChannel(T* frame, const unsigned int width, const unsigned int height, const T value, const unsigned int framePaddingElements, Worker* worker = nullptr);

		/**
		 * Reverses the order of the channels of a frame with zipped pixel format.
		 * The first channel will be exchanged with the last channel, the second channel will be exchanged with the second last channel and so on.
		 * @param source The source frame from that the channels will be swapped, must be valid
		 * @param target The target frame that receives the swapped channels, must be valid
		 * @param width The width of the source frame in pixel, with range (0, infinity)
		 * @param height The height of the source frame in pixel, with range (0, infinity)
		 * @param conversionFlag The conversion to be applied
		 * @param sourcePaddingElements Optional padding at the end of each source row in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional padding at the end of each target row in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam T Data type of each channel pixel value
		 * @tparam tChannels Number of data channels, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static inline void reverseChannelOrder(const T* source, T* target, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Shuffles the channels of a frame by an arbitrary pattern.
		 * The shuffle pattern is defined in groups of four bits defining the source channels.<br>
		 * For the shuffling from e.g., an RGBA32 row to a BGRA32 row the pattern 0x3012u must be defined:
		 * <pre>
		 * source pixel  R G B A
		 *               0 1 2 3
		 * target pixel  B G R A
		 *               2 1 0 3
		 * pattern (with reversed order): 0x3012
		 * </pre>
		 * @param source The source frame for which the channels will be shuffled, must be valid
		 * @param target The target frame that receives the shuffled channels, must be valid
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param conversionFlag The conversion to be applied
		 * @param sourcePaddingElements Optional padding at the end of each source row in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional padding at the end of each target row in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam T Data type of each channel pixel value
		 * @tparam tSourceChannels Number of source data channels, with range [1, 8u]
		 * @tparam tTargetChannels Number of target data channels, with range [1, 8u]
		 * @tparam tShufflePattern Groups of four bits define the source channel, e.g., 0x76543210 defines the identity transformation, 0x01234567 defines the reverse transformation
		 */
		template <typename T, unsigned int tSourceChannels, unsigned int tTargetChannels, unsigned int tShufflePattern>
		static inline void shuffleChannels(const T* source, T* target, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Shuffles the channels of source frame and sets the last channel with constant value in the target frame.
		 * The shuffle pattern is defined in groups of four bits defining the source channels.<br>
		 * For the shuffling from e.g., an RGB24 row to a BGRA32 row the pattern 0x012u must be defined:
		 * <pre>
		 * source pixel  R G B
		 *               0 1 2
		 * target pixel  B G R A
		 *               2 1 0
		 * pattern (with reversed order): 0x012
		 * </pre>
		 * @param source The source frame for which the channels will be shuffled, must be valid
		 * @param newChannelValue The constant channel value which will be added as last channel to the target frame, with range [0, infinity)
		 * @param target The target frame that receives the shuffled channels, must be valid
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param conversionFlag The conversion to be applied
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam T Data type of each channel pixel value
		 * @tparam tSourceChannels Number of source data channels, with range [1, 8u]
		 * @tparam tTargetChannels Number of target data channels, including the additional extra target channel, with range [2, 8u]
		 * @tparam tShufflePattern Groups of four bits define the source channel, e.g., 0x76543210 defines the identity transformation, 0x01234567 defines the reverse transformation
		 */
		template <typename T, unsigned int tSourceChannels, unsigned int tTargetChannels, unsigned int tShufflePattern>
		static inline void shuffleChannelsAndSetLastChannelValue(const T* source, const T newChannelValue, T* target, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Narrows 16 bit channels of a frame to 8 bit channels.
		 * @param source The source frame for which the channels will be narrowed, must be valid
		 * @param target The target frame that receives the narrowed channels, must be valid
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param conversionFlag The conversion to be applied
		 * @param sourcePaddingElements Optional padding at the end of each source row in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional padding at the end of each target row in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannels Number of source data channels, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static inline void narrow16BitPerChannelTo8BitPerChannel(const uint16_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Applies a specific modifier function on each pixel.
		 * @param source The source frame providing the pixel information, must be valid
		 * @param target The target frame receiving the pixel information, must be valid
		 * @param width The width of the source frame in pixel, with range (0, infinity)
		 * @param height The height of the source frame in pixel, with range (0, infinity)
		 * @param conversionFlag The conversion to be applied
		 * @param worker Optional worker object to distribute the computation
		 * @tparam T Data type of each channel pixel value
		 * @tparam tChannels Number of data channels, with range [1, infinity)
		 * @tparam tPixelFunction Pixel modification function
		 */
		template <typename T, unsigned int tChannels, void (*tPixelFunction)(const T*, T*)>
		static void applyPixelModifier(const T* source, T* target, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, Worker* worker = nullptr);

		/**
		 * Applies a specific modifier function on each pixel.
		 * @param source The source frame providing the pixel information, must be valid
		 * @param target The target frame receiving the pixel information, must be valid
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each row of the source frame, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each row of the target frame, in elements, with range [0, infinity)
		 * @param conversionFlag The conversion to be applied
		 * @param worker Optional worker object to distribute the computation
		 * @tparam TSource Data type of each source channel pixel value
		 * @tparam TTarget Data type of each target channel pixel value
		 * @tparam tSourceChannels Number of source data channels, with range [1, infinity)
		 * @tparam tTargetChannels Number of target data channels, with range [1, infinity)
		 * @tparam tPixelFunction Pixel modification function
		 */
		template <typename TSource, typename TTarget, unsigned int tSourceChannels, unsigned int tTargetChannels, void (*tPixelFunction)(const TSource*, TTarget*)>
		static void applyAdvancedPixelModifier(const TSource* source, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const ConversionFlag conversionFlag, Worker* worker = nullptr);

		/**
		 * Generic bivariate pixel operations
		 * Applies bivariate per-pixel operators: `C(y, x) = op(A(y, x), B(y, x))`. Input and output must have the same frame type and have a single plane.
		 * @param source0 First source frame
		 * @param source1 Second source frame
		 * @param target The target frame
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param source0PaddingElements The number of padding elements at the end of each row of the first source, in elements, with range [0, infinity)
		 * @param source1PaddingElements The number of padding elements at the end of each row of the second source, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param conversionFlag The conversion to be applied
		 * @param worker Optional worker object to distribute the computation
		 * @tparam TSource0 Type of the first data source
		 * @tparam TSource1 Type of the second data source
		 * @tparam TTarget Type of the target
		 * @tparam TIntermediate Data type that is used for the computation of intermediate results, e.g. if TSource0 and TSource1 are different
		 * @tparam tSourceChannels Number of channels of the two sources, range: [1, infinity)
		 * @tparam tTargetChannels Number of channels of the target, range: [1, infinity)
		 * @tparam tOperator The operation (function) that is applied on both sources to yield the value for the target (called per pixel)
		 */
		template <typename TSource0, typename TSource1, typename TTarget, typename TIntermediate, unsigned int tSourceChannels, unsigned int tTargetChannels, void (*tOperator)(const TSource0*, const TSource1*, TTarget*)>
		static void applyBivariateOperator(const TSource0* source0, const TSource1* source1, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int source0PaddingElements, const unsigned int source1PaddingElements, const unsigned int targetPaddingElements, const ConversionFlag conversionFlag, Worker* worker = nullptr);

		/**
		 * Applies a row operator to all rows of a source image.
		 * The row operator is given as function pointer and is intended to transform a source row to a target row.<br>
		 * The function allows to implement e.g., frame filters with few lines of code, source and target frame must have the same size.
		 * @param source The source frame to which the row operator is applied, must be valid
		 * @param target The target frame receiving the result of the row operator, must be valid
		 * @param width The width of the source frame and target frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame and target frame in pixel, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param rowOperatorFunction The pointer to the row operator function, must be valid
		 * @param worker Optional worker object to distribute the computation
		 * @tparam TSource The data type of the source elements
		 * @tparam TTarget The data type of the target elements
		 * @tparam tSourceChannels The number of channels the source frame has, with range [1, infinity)
		 * @tparam tTargetChannels The number of channels the target frame has, with range [1, infinity)
		 */
		template <typename TSource, typename TTarget, unsigned int tSourceChannels, unsigned int tTargetChannels>
		static void applyRowOperator(const TSource* source, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const RowOperatorFunction<TSource, TTarget, tSourceChannels, tTargetChannels>& rowOperatorFunction, Worker* worker = nullptr);

		/**
		 * Transforms a frame with generic pixel format (with zipped pixel information) like RGB24 or YUV24, to a frame with same pixel format and channel number.
		 * This function mainly mirrors or flips an image.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param conversionFlag The conversion to be applied
		 * @param sourcePaddingElements Optional padding at the end of each source row in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional padding at the end of each target row in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam T Data type of each channel pixel value, e.g., 'uint8_t', 'float', ...
		 * @tparam tChannels Number of channels of the frame, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static inline void transformGeneric(const T* source, T* target, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker);

		/**
		 * Converts an image with premultiplied alpha to a straight image (without premultiplied alpha).
		 * @param frame The image to convert, must be valid
		 * @param width The width of the image in pixel, with range [1, infinity)
		 * @param height The height of the image in pixel, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannels The number of frame channels, with range [2, infinity)
		 * @tparam tAlphaChannelIndex The index of the alpha channel, with range [0, tChannels - 1]
		 * @see straightAlphaToPremultipliedAlpha8BitPerChannel().
		 */
		template <unsigned int tChannels, unsigned int tAlphaChannelIndex>
		static inline void premultipliedAlphaToStraightAlpha8BitPerChannel(uint8_t* const frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, Worker* worker = nullptr);

		/**
		 * Converts an image with premultiplied alpha to a straight image (without premultiplied alpha).
		 * @param source The source image to convert, must be valid
		 * @param target The resulting converted target image, must be valid
		 * @param width The width of the image in pixel, with range [1, infinity)
		 * @param height The height of the image in pixel, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannels The number of frame channels, with range [2, infinity)
		 * @tparam tAlphaChannelIndex The index of the alpha channel, with range [0, tChannels - 1]
		 * @see straightAlphaToPremultipliedAlpha8BitPerChannel().
		 */
		template <unsigned int tChannels, unsigned int tAlphaChannelIndex>
		static inline void premultipliedAlphaToStraightAlpha8BitPerChannel(const uint8_t* const source, uint8_t* const target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Converts an image with straight alpha (without premultiplied alpha) to an image with premultiplied alpha.
		 * @param frame The image to convert, must be valid
		 * @param width The width of the image in pixel, with range [1, infinity)
		 * @param height The height of the image in pixel, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannels The number of frame channels, with range [2, infinity)
		 * @tparam tAlphaChannelIndex The index of the alpha channel, with range [0, tChannels - 1]
		 * @see premultipliedAlphaToStraightAlpha8BitPerChannel().
		 */
		template <unsigned int tChannels, unsigned int tAlphaChannelIndex>
		static inline void straightAlphaToPremultipliedAlpha8BitPerChannel(uint8_t* const frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, Worker* worker = nullptr);

		/**
		 * Converts an image with straight alpha (without premultiplied alpha) to an image with premultiplied alpha.
		 * @param source The source image to convert, must be valid
		 * @param target The resulting converted target image, must be valid
		 * @param width The width of the image in pixel, with range [1, infinity)
		 * @param height The height of the image in pixel, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannels The number of frame channels, with range [2, infinity)
		 * @tparam tAlphaChannelIndex The index of the alpha channel, with range [0, tChannels - 1]
		 * @see premultipliedAlphaToStraightAlpha8BitPerChannel().
		 */
		template <unsigned int tChannels, unsigned int tAlphaChannelIndex>
		static inline void straightAlphaToPremultipliedAlpha8BitPerChannel(const uint8_t* const source, uint8_t* const target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker = nullptr);

		/**
		 * Reverses/mirrors the order of pixels in a given row (or a memory block in general).
		 * @param source The pointer to the source pixels, must be valid
		 * @param target The pointer to the target pixels receiving the reversed/mirrored pixel data, must be valid
		 * @param size The number of source (and target pixels) to convert, with range [1, infinity)
		 * @tparam T The data type of the pixel elements, e.g, 'uint8_t', 'int'
		 * @tparam tChannels The number of channels (the number of elements) each pixel has, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static void reverseRowPixelOrder(const T* source, T* target, const size_t size);

		/**
		 * Reverses/mirrors the order of pixels in a given row (or a memory block in general) in place.
		 * @param data The pointer to the pixels, must be valid
		 * @param size The number of source (and target pixels) to convert, with range [1, infinity)
		 * @tparam T The data type of the pixel elements, e.g, 'uint8_t', 'int'
		 * @tparam tChannels The number of channels (the number of elements) each pixel has, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static void reverseRowPixelOrderInPlace(T* data, const size_t size);

		/**
		 * Reverses/mirrors the order of channels in a given row (or a memory block in general).
		 * @param source The pointer to the source pixels, must be valid
		 * @param target The pointer to the target pixels receiving the reversed/mirrored channels, must be valid
		 * @param size The number of source (and target pixels) to convert, with range [1, infinity)
		 * @param unusedOptions An unused options parameters, must be nullptr
		 * @tparam T The data type of the pixel elements, e.g, 'uint8_t', 'int'
		 * @tparam tChannels The number of channels (the number of elements) each pixel has, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static void reverseRowChannelOrder(const T* source, T* target, const size_t size, const void* unusedOptions = nullptr);

		/**
		 * Shuffles the channels of row pixels by application of a specified shuffle pattern.
		 * The shuffle pattern is defined in groups of four bits defining the source channels.<br>
		 * For the shuffling from e.g., an RGBA32 row to a BGRA32 row the pattern 0x3012u must be defined:
		 * <pre>
		 * source pixel  R G B A
		 *               0 1 2 3
		 * target pixel  B G R A
		 *               2 1 0 3
		 * pattern (with reversed order): 0x3012
		 * </pre>
		 * @param source The pointer to the source pixels, must be valid
		 * @param target The pointer to the target pixels, receiving the shuffled channels, must be valid
		 * @param size The number of source (and target pixels) to convert, with range [1, infinity)
		 * @param unusedOptions An unused options parameters, must be nullptr
		 * @tparam T Data type of each channel pixel value, e.g, 'uint8_t' or 'float'
		 * @tparam tSourceChannels Number of source data channels, with range [1, 8u]
		 * @tparam tTargetChannels Number of target data channels, with range [1, 8u]
		 * @tparam tShufflePattern Groups of four bits define the source channel, e.g., 0x76543210 defines the identity transformation, 0x01234567 defines the reverse transformation
		 */
		template <typename T, unsigned int tSourceChannels, unsigned int tTargetChannels, unsigned int tShufflePattern>
		static inline void shuffleRowChannels(const T* source, T* target, const size_t size, const void* unusedOptions = nullptr);

		/**
		 * Shuffles the channels of row pixels by application of a specified shuffle pattern and sets the last channel with constant value in the target row.
		 * The shuffle pattern is defined in groups of four bits defining the source channels.<br>
		 * For the shuffling from e.g., an RGB24 row to a BGRA32 row the pattern 0x012u must be defined:
		 * <pre>
		 * source pixel  R G B
		 *               0 1 2
		 * target pixel  B G R A
		 *               2 1 0
		 * pattern (with reversed order): 0x012
		 * </pre>
		 * @param source The pointer to the source pixels, must be valid
		 * @param target The pointer to the target pixels, receiving the shuffled channels, must be valid
		 * @param size The number of source (and target pixels) to convert, with range [1, infinity)
		 * @param options Pointer to the constant channel value which will be added to the end of the target channels, must be valid
		 * @tparam T Data type of each channel pixel value, e.g, 'uint8_t' or 'float'
		 * @tparam tSourceChannels Number of source data channels, with range [1, 8u]
		 * @tparam tTargetChannels Number of target data channels, including the additional extra target channel, with range [2, 8u]
		 * @tparam tShufflePattern Groups of four bits define the source channel, e.g., 0x76543210 defines the identity transformation, 0x01234567 defines the reverse transformation
		 */
		template <typename T, unsigned int tSourceChannels, unsigned int tTargetChannels, unsigned int tShufflePattern>
		static inline void shuffleRowChannelsAndSetLastChannelValue(const T* source, T* target, const size_t size, const void* options = nullptr);

		/**
		 * Converts a row of pixels with 3 channels to pixels with one channel by a linear combination of the four channels.
		 * This function can be used to e.g., convert RGB24 to Y8, or BGR24 to Y8.
		 * The linear combination is defined by one integer multiplication factor for each channel with 128 as denominator.<br>
		 * Beware: As this function applies integer multiplication factors (with 7 bits precision) the conversion result has an accuracy of +/- 2 color intensities.
		 * @param source The pointer to the source pixels, must be valid
		 * @param target The pointer to the target pixels receiving the converted pixel data, must be valid
		 * @param size The number of source (and target pixels) to convert, with range [1, infinity)
		 * @param channelMultiplicationFactors_128 The three uint32_t multiplication factors, one for each channel, with range [0, 128], while the sum of all four factors must be 128, must be valid
		 * @tparam tUseFactorChannel0 True, if the value(s) of factorChannel0 is not zero; False, if the value(s) of factorChannel0 is zero
		 * @tparam tUseFactorChannel1 True, if the value(s) of factorChannel1 is not zero; False, if the value(s) of factorChannel1 is zero
		 * @tparam tUseFactorChannel2 True, if the value(s) of factorChannel2 is not zero; False, if the value(s) of factorChannel2 is zero
		 */
		template <bool tUseFactorChannel0, bool tUseFactorChannel1, bool tUseFactorChannel2>
		static void convertRow3ChannelsTo1Channel8BitPerChannel7BitPrecision(const uint8_t* source, uint8_t* target, const size_t size, const void* channelMultiplicationFactors_128);

		/**
		 * Converts a row of pixels with 3 channels to pixels with 3 channels by a linear combination of the three channels plus an translational part applied to the source data before applying the linear transformation.
		 * This function can be used to e.g., convert RGB24 to YUV24, or YUV24 to RGB24.
		 * The linear combination is defined by three integer multiplication factor for each source channel with 64 as denominator, plus one translation parameter for each source channel (with 1 as denominator).<br>
		 * Beware: As this function applies integer multiplication factors (with 6 bits precision) the conversion result has an accuracy of +/- 4 color intensities.<br>
		 * The transformation is based on the following pattern:
		 * <pre>
		 * t0 = clamp(0, f00 * (s0 - b0) + f01 * (s1 - b1) + f02 * (s2 - b2), 255)
		 * t1 = clamp(0, f10 * (s0 - b0) + f11 * (s1 - b1) + f12 * (s2 - b2), 255)
		 * t2 = clamp(0, f20 * (s0 - b0) + f21 * (s1 - b1) + f22 * (s2 - b2), 255)
		 * </pre>
		 * With t target, s source, f factor, and b bias/translation.<br>
		 * Factors must be specified in relation to a denominator of 64, bias values must be specified with a denominator of 1.
		 * @param source The pointer to the source pixels, must be valid
		 * @param target The pointer to the target pixels receiving the converted pixel data, must be valid
		 * @param size The number of source (and target pixels) to convert, with range [1, infinity)
		 * @param parameters The 12 int32_t parameters of the column-aligned 3x3 transformation matrix, plus 3 translation parameters: f00_64, f10_64, f20_64, f01_64, f02_64, ..., f22_64, with ranges [-128, 128], b0, b1, b2, with ranges [0, 128]
		 */
		static void convertRow3ChannelsTo3Channels8BitPerChannel6BitPrecision(const uint8_t* source, uint8_t* target, const size_t size, const void* parameters);

		/**
		 * Converts a row of pixels with 3 channels to pixels with 3 channels by a linear combination of the three channels plus a bias (translation) part.
		 * This function can be used to e.g., convert RGB24 to YUV24, or BGR24 to YVU24.
		 * The linear combination is defined by three integer multiplication factor for each source channel with 128 as denominator, plus one bias (translation) parameter for each source channel (with 1 as denominator).<br>
		 * Beware: As this function applies integer multiplication factors (with 7 bits precision) the conversion result has an accuracy of +/- 2 color intensities.<br>
		 * The transformation is based on the following pattern:
		 * <pre>
		 * t0 = clamp(0, f00 * s0 + f01 * s1 + f02 * s2 + b0, 255)
		 * t1 = clamp(0, f10 * s0 + f11 * s1 + f12 * s2 + b1, 255)
		 * t2 = clamp(0, f20 * s0 + f21 * s1 + f22 * s2 + b2, 255)
		 * </pre>
		 * With t target, s source, f factor, and b bias.<br>
		 * Factors must be specified in relation to a denominator of 128, bias values must be specified with a denominator of 1.
		 * @param source The pointer to the source pixels, must be valid
		 * @param target The pointer to the target pixels receiving the converted pixel data, must be valid
		 * @param size The number of source (and target pixels) to convert, with range [1, infinity)
		 * @param parameters The 12 int32_t parameters of the column-aligned 3x4 transformation matrix: f00_128, f10_128, f20_128, f01_128, f02_128, ..., f22_128, b0, b1, b2, with ranges [-127, 127]
		 */
		static void convertRow3ChannelsTo3Channels8BitPerChannel7BitPrecision(const uint8_t* source, uint8_t* target, const size_t size, const void* parameters);

		/**
		 * Converts a row of pixels with 3 channels to pixels with 3 channels by a linear combination of the three channels plus a bias (translation) part.
		 * This function can be used to e.g., convert YUV24 to RGB24, or YVU24 to BGR24.
		 * The linear combination is defined by three integer multiplication factor for each source channel with 1024 as denominator. plus one bias (translation) parameter for each source channel (with 1 as denominator).<br>
		 * Beware: As this function applies integer multiplication factors (with 10 bits precision) the conversion result has an accuracy of +/- 1 color intensities.<br>
		 * The transformation is based on the following pattern:
		 * <pre>
		 * t0 = clamp(0, f00 * s0 + f01 * s1 + f02 * s2 + b0, 255)
		 * t1 = clamp(0, f10 * s0 + f11 * s1 + f12 * s2 + b1, 255)
		 * t2 = clamp(0, f20 * s0 + f21 * s1 + f22 * s2 + b2, 255)
		 * </pre>
		 * With t target, s source, f factor, and b bias.<br>
		 * Factors must be specified in relation to a denominator of 1024, bias values must be specified with a denominator of 1.
		 * @param source The pointer to the source pixels, must be valid
		 * @param target The pointer to the target pixels receiving the converted pixel data, must be valid
		 * @param size The number of source (and target pixels) to convert, with range [1, infinity)
		 * @param parameters The 12 int32_t parameters of the column-aligned 3x4 transformation matrix: f00_1024, f10_1024, f20_1024, f01_1024, f02_1024, ..., f22_1024, b0, b1, b2, with ranges [-1024 * 16, 1024 * 16]
		 */
		static void convertRow3ChannelsTo3Channels8BitPerChannel10BitPrecision(const uint8_t* source, uint8_t* target, const size_t size, const void* parameters);

		/**
		 * Converts a row of pixels with 3 channels to pixels with 4 channels by a linear combination of the three channels plus an translational part applied to the source data before applying the linear transformation (for the first three channels).
		 * The fourth channel is set to a constant value, e.g., for an alpha channel.<br>
		 * This function can be used to e.g., convert YUV24 to RGBA32, or YVU24 to BGRA32.<br>
		 * The linear combination is defined by three integer multiplication factor for each source channel with 64 as denominator, plus one translation parameter for each source channel (with 1 as denominator).<br>
		 * Beware: As this function applies integer multiplication factors (with 6 bits precision) the conversion result has an accuracy of +/- 4 color intensities.<br>
		 * The transformation is based on the following pattern:
		 * <pre>
		 * t0 = clamp(0, f00 * (s0 - b0) + f01 * (s1 - b1) + f02 * (s2 - b2), 255)
		 * t1 = clamp(0, f10 * (s0 - b0) + f11 * (s1 - b1) + f12 * (s2 - b2), 255)
		 * t2 = clamp(0, f20 * (s0 - b0) + f21 * (s1 - b1) + f22 * (s2 - b2), 255)
		 * t3 = valueChannel3
		 * </pre>
		 * With t target, s source, f factor, and b bias/translation.<br>
		 * Factors must be specified in relation to a denominator of 64, bias values must be specified with a denominator of 1.
		 * @param source The pointer to the source pixels, must be valid
		 * @param target The pointer to the target pixels receiving the converted pixel data, must be valid
		 * @param size The number of source (and target pixels) to convert, with range [1, infinity)
		 * @param parameters The 13 int32_t parameters of the column-aligned 3x3 transformation matrix, plus 3 translation parameters: f00_64, f10_64, f20_64, f01_64, f02_64, ..., f22_64, with ranges [-128, 128], b0, b1, b2, with ranges [0, 128], valueChannel3, with range [0, 255]
		 */
		static void convertRow3ChannelsTo4Channels8BitPerChannel6BitPrecision(const uint8_t* source, uint8_t* target, const size_t size, const void* parameters);

		/**
		 * Converts a row of pixels with 4 channels to pixels with one channel by a linear combination of the four channels.
		 * This function can be used to e.g., convert RGBA32 to Y8, or ARGB32 to Y8, or RGB32 to Y8.
		 * The linear combination is defined by one integer multiplication factor for each channel with 128 as denominator.<br>
		 * Beware: As this function applies integer multiplication factors (with 7 bits precision) the conversion result has an accuracy of +/- 2 color intensities.
 		 * <pre>
		 * t0 = f0 * s0 + f1 * s1 + f2 * s2 + f3 * s3
		 * </pre>
		 * @param source The pointer to the source pixels, must be valid
		 * @param target The pointer to the target pixels receiving the converted pixel data, must be valid
		 * @param size The number of source (and target pixels) to convert, with range [1, infinity)
		 * @param channelMultiplicationFactors_128 The four uint32_t multiplication factors, one for each channel, with range [0, 127], while the sum of all four factors must be 128, must be valid
		 * @tparam tUseFactorChannel0 True, if the value(s) of factorChannel0 is not zero; False, if the value(s) of factorChannel0 is zero
		 * @tparam tUseFactorChannel1 True, if the value(s) of factorChannel1 is not zero; False, if the value(s) of factorChannel1 is zero
		 * @tparam tUseFactorChannel2 True, if the value(s) of factorChannel2 is not zero; False, if the value(s) of factorChannel2 is zero
		 * @tparam tUseFactorChannel3 True, if the value(s) of factorChannel3 is not zero; False, if the value(s) of factorChannel3 is zero
		 */
		template <bool tUseFactorChannel0, bool tUseFactorChannel1, bool tUseFactorChannel2, bool tUseFactorChannel3>
		static void convertRow4ChannelsTo1Channel8BitPerChannel7BitPrecision(const uint8_t* source, uint8_t* target, const size_t size, const void* channelMultiplicationFactors_128);

		/**
		 * Converts a row of pixels with 4 channels to pixels with two channel by a linear combination of the four channels.
		 * This function can be used to e.g., convert RGBA32 to YA16, or ARGB32 to AY16.
		 * The linear combination is defined by one integer multiplication factor for each channel with 128 as denominator.<br>
		 * Beware: As this function applies integer multiplication factors (with 7 bits precision) the conversion result has an accuracy of +/- 2 color intensities.
		 * The transformation is based on the following pattern:
		 * <pre>
		 * t0 = f00 * s0 + f01 * s1 + f02 * s2 + f03 * s3
		 * t1 = f10 * s0 + f11 * s1 + f12 * s2 + f13 * s3
		 * </pre>
		 * @param source The pointer to the source pixels, must be valid
		 * @param target The pointer to the target pixels receiving the converted pixel data, must be valid
		 * @param size The number of source (and target pixels) to convert, with range [1, infinity)
		 * @param multiplicationFactors_128 The 8 int32_t parameters of the column-aligned 2x4 transformation matrix: f00_128, f10_128, f01_128, ..., f13_128, with range [0, 127], while the sum of all four row factors must be 128, must be valid
		 */
		static void convertRow4ChannelsTo2Channels8BitPerChannel7BitPrecision(const uint8_t* source, uint8_t* target, const size_t size, const void* multiplicationFactors_128);

		/**
		 * Converts a row of pixels with 3 channels to pixels with 3 channels by a linear combination of the three channels plus a bias (translation) part.
		 * This function can be used to e.g., convert RGBA32 to YUV24, or BGRA24 to YVU24.
		 * The linear combination is defined by three integer multiplication factor for each source channel with 128 as denominator, plus one bias (translation) parameter for each source channel (with 1 as denominator).<br>
		 * Beware: As this function applies integer multiplication factors (with 7 bits precision) the conversion result has an accuracy of +/- 2 color intensities.<br>
		 * The transformation is based on the following pattern:
		 * <pre>
		 * t0 = clamp(0, f00 * s0 + f01 * s1 + f02 * s2 + f03 * s3 + b0, 255)
		 * t1 = clamp(0, f10 * s0 + f11 * s1 + f12 * s2 + f13 * s3 + b1, 255)
		 * t2 = clamp(0, f20 * s0 + f21 * s1 + f22 * s2 + f23 * s3 + b2, 255)
		 * </pre>
		 * With t target, s source, f factor, and b bias.<br>
		 * Factors must be specified in relation to a denominator of 128, bias values must be specified with a denominator of 1.
		 * @param source The pointer to the source pixels, must be valid
		 * @param target The pointer to the target pixels receiving the converted pixel data, must be valid
		 * @param size The number of source (and target pixels) to convert, with range [1, infinity)
		 * @param parameters The 12 int32_t parameters of the column-aligned 3x4 transformation matrix: f00_128, f10_128, f20_128, f01_128, f02_128, ..., f23_128, b0, b1, b2, with ranges [-127, 127]
		 */
		static void convertRow4ChannelsTo3Channels8BitPerChannel7BitPrecision(const uint8_t* source, uint8_t* target, const size_t size, const void* parameters);

		/**
		 * Narrows a row of pixels with 16 bit channels to pixels with 8 bit channels.
		 * @param source The pointer to the source pixels, must be valid
		 * @param target The pointer to the target pixels receiving the converted pixel data, must be valid
		 * @param size The number of source (and target pixels) to convert, with range [1, infinity)
		 * @param unusedParameters Unused parameter, must be nullptr
		 * @tparam tChannels The number of channels the source (and target) frame have, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void narrowRow16BitPerChannelTo8BitPerChannel(const uint16_t* source, uint8_t* target, const size_t size, const void* unusedParameters = nullptr);

		/**
		 * Adds a channel to a given row with generic (zipped) pixel format and copies the information of the new channel from a one-channel image.
		 * The channel can be added at new first channel or as new last channel.
		 * @param sources The pointer to the multi-channel source frame and to the single-channel source frame, must be valid
		 * @param targets The one pointer to the target image, must be valid
		 * @param multipleRowIndex The index of the multiple-row to be handled, with range [0, height - 1]
		 * @param width The width of the frame in pixel, with range [1, infinity), must be even
		 * @param height The height of the frame in pixel, with range [1, infinity), must be even
		 * @param conversionFlag The conversion to be applied
		 * @param options The 1 options parameters: padding parameters of 1-channel source frame, must be valid
		 * @tparam T Data type of each channel pixel value, e.g, 'uint8_t' or 'float'
		 * @tparam tSourceChannels Number of channels of the source frame (without the new channel), with range [1, infinity)
		 * @tparam tAddToFront True, to add the channel to the front (as new first channel); False, to add the channel to the back (as new last channel).
		 */
		template <typename T, unsigned int tSourceChannels, bool tAddToFront>
		static void addChannelRow(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options);

		/**
		 * Adds a channel to a given row with generic (zipped) pixel format and sets all values to a specified value.
		 * The channel can be added at new first channel or as new last channel.
		 * @param source The pointer to the source pixels, must be valid
		 * @param target The pointer to the target pixels, receiving the additional channels, must be valid
		 * @param size The number of source (and target pixels) to convert, with range [1, infinity)
		 * @param channelValueParameter The pointer to the value of the channel to be set (with data type 'T'), must be valid
		 * @tparam T Data type of each channel pixel value, e.g, 'uint8_t' or 'float'
		 * @tparam tSourceChannels Number of channels of the source frame (without the new channel), with range [1, infinity)
		 * @tparam tAddToFront True, to add the channel to the front (as new first channel); False, to add the channel to the back (as new last channel).
		 */
		template <typename T, unsigned int tSourceChannels, bool tAddToFront>
		static void addChannelValueRow(const T* source, T* target, const size_t size, const void* channelValueParameter);

		/**
		 * Copies one channel from a source row to a target row with generic (zipped) pixel format.
		 * @param source The pointer to the source pixels, must be valid
		 * @param target The pointer to the target pixels, receiving the additional channels, must be valid
		 * @param size The number of source (and target pixels) to convert, with range [1, infinity)
		 * @tparam T Data type of each channel pixel value, e.g, 'uint8_t' or 'float'
		 * @param unusedParameters Unused parameters, must be nullptr
		 * @tparam tSourceChannels Number of channels of the source frame, with range [1, infinity)
		 * @tparam tTargetChannels Number of channels of the target frame, with range [1, infinity)
		 * @tparam tSourceChannelIndex The index of the source channel to be copied, with range [0, tSourceChannels - 1]
		 * @tparam tTargetChannelIndex The index of the target channel to be copied, with range [0, tTargetChannels - 1]
		 */
		template <typename T, unsigned int tSourceChannels, unsigned int tTargetChannels, unsigned int tSourceChannelIndex, unsigned int tTargetChannelIndex>
		static void copyChannelRow(const T* source, T* target, const size_t size, const void* unusedParameters = nullptr);

	protected:

		/**
		 * Separates a given frame with zipped pixel format e.g., FORMAT_RGB24, FORMAT_YUV24, FORMAT_BGRA32 into individual frames with one channel only.
		 * @param sourceFrame The frame to be separated, must be valid
		 * @param targetFrames The pointers to the resulting separated frames each holding one channel of the source frame, with already allocated memory
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param channels The number of channels the source frame has, with range [1, infinity)
		 * @param sourceFramePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetFramesPaddingElements The array of padding elements at the end of each target row, one for each target frame, in elements, with range [0, infinity)
		 * @tparam TSource The data type of each source pixel channel, e.g., 'uint8_t', 'float', ...
		 * @tparam TTarget The data type of each target pixel channel, e.g., 'uint8_t', 'float', ...
		 */
		template <typename TSource, typename TTarget>
		static void separateTo1ChannelRuntime(const TSource* const sourceFrame, TTarget* const* const targetFrames, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int sourceFramePaddingElements, const unsigned int* targetFramesPaddingElements);

		/**
		 * Zips/interleaves 1-channel images into one image with n-channels.
		 * @param sourceFrames The pointers to the individual 1-channel frames, one for each image, must be valid
		 * @param targetFrame The pointer to the resulting zipped frame holding n-channels, must be valid
		 * @param width The width of the source frames in pixel, with range [1, infinity)
		 * @param height The height of the source frames in pixel, with range [1, infinity)
		 * @param channels The number of provided source frames (and the number of channels the target frame will have), with range [1, infinity)
		 * @param sourceFramesPaddingElements The array of padding elements at the end of each source row, one for each source frame, in elements, with range [0, infinity)
		 * @param targetFramePaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @tparam TSource The data type of each source pixel channel, e.g., 'uint8_t', 'float', ...
		 * @tparam TTarget The data type of each target pixel channel, e.g., 'uint8_t', 'float', ...
		 */
		template <typename TSource, typename TTarget>
		static void zipChannelsRuntime(const TSource* const* const sourceFrames, TTarget* const targetFrame, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int* sourceFramesPaddingElements, const unsigned int targetFramePaddingElements);

		/**
		 * Sets one channel of a frame with one unique value.
		 * @param frame The frame in that one channel of each pixel will be set, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param value The value to be set
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param firstRow First row to be handled
		 * @param numberRows Number of rows to be handled
		 * @tparam T Data type of each channel pixel value
		 * @tparam tChannel Index of the channel that will be inverted, with range [0, tChannels)
		 * @tparam tChannels Number of data channels of the frames, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannel, unsigned int tChannels>
		static void setChannelSubset(T* frame, const unsigned int width, const T value, const unsigned int framePaddingElements, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Applies a specific modifier function on each pixel.
		 * @param source The source frame providing the pixel information, must be valid
		 * @param target The target frame receiving the pixel information, must be valid
		 * @param width The width of the source frame in pixel
		 * @param height The height of the source frame in pixel
		 * @param conversionFlag The conversion to be applied
		 * @param firstRow First row to be handled
		 * @param numberRows Number of rows to be handled
		 * @tparam T Data type of each channel pixel value
		 * @tparam tChannels Number of data channels, with range [1, infinity)
		 * @tparam tPixelFunction Pixel modification function
		 */
		template <typename T, unsigned int tChannels, void (*tPixelFunction)(const T*, T*)>
		static void applyPixelModifierSubset(const T* source, T* target, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Applies a specific modifier function on each pixel.
		 * @param source The source frame providing the pixel information, must be valid
		 * @param target The target frame receiving the pixel information, must be valid
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each row of the source frame, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each row of the target frame, in elements, with range [0, infinity)
		 * @param conversionFlag The conversion to be applied
		 * @param firstRow First row to be handled
		 * @param numberRows Number of rows to be handled
		 * @tparam TSource Data type of each source channel pixel value
		 * @tparam TTarget Data type of each target channel pixel value
		 * @tparam tSourceChannels Number of source data channels, with range [1, infinity)
		 * @tparam tTargetChannels Number of target data channels, with range [1, infinity)
		 * @tparam tPixelFunction Pixel modification function
		 */
		template <typename TSource, typename TTarget, unsigned int tSourceChannels, unsigned int tTargetChannels, void (*tPixelFunction)(const TSource*, TTarget*)>
		static void applyAdvancedPixelModifierSubset(const TSource* source, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const ConversionFlag conversionFlag, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Generic bivariate pixel operations
		 * @param source0 First source frame
		 * @param source1 Second source frame
		 * @param target The target frame
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param source0PaddingElements The number of padding elements at the end of each row of the first source, in elements, with range [0, infinity)
		 * @param source1PaddingElements The number of padding elements at the end of each row of the second source, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param conversionFlag The conversion to be applied
		 * @param firstRow First row to be handled
		 * @param numberRows Number of rows to be handled
		 * @tparam TSource0 Type of the first data source
		 * @tparam TSource1 Type of the second data source
		 * @tparam TTarget Type of the target
		 * @tparam TIntermediate Type for the computation of intermediate result, e.g. if TSource0 and TSource1 are different
		 * @tparam tSourceChannels Number of channels of the two sources, range: [1, infinity)
		 * @tparam tTargetChannels Number of channels of the target, range: [1, infinity)
		 * @tparam tOperator The operation (function) that is applied on both sources to yield the value for the target (called per pixel)
		 */
		template <typename TSource0, typename TSource1, typename TTarget, typename TIntermediate, unsigned int tSourceChannels, unsigned int tTargetChannels, void (*tOperator)(const TSource0*, const TSource1*, TTarget*)>
		static void applyBivariateOperatorSubset(const TSource0* source0, const TSource1* source1, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int source0PaddingElements, const unsigned int source1PaddingElements, const unsigned int targetPaddingElements, const ConversionFlag conversionFlag, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Applies a row operator to a subset of all rows of a source image.
		 * The row operator is given as function pointer and is intended to transform a source row to a target row.<br>
		 * The function allows to implement e.g., frame filters with few lines of code, source and target frame must have the same size.
		 * @param source The source frame to which the row operator is applied, must be valid
		 * @param target The target frame receiving the result of the row operator, must be valid
		 * @param width The width of the source frame and target frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame and target frame in pixel, with range [1, infinity)
		 * @param sourceStrideElements The number of stride elements at the end of each source row, in elements, with range [width * tSourceChannels, infinity)
		 * @param targetStrideElements The number of padding elements at the end of each target row, in elements, with range [width * tTargetChannels, infinity)
		 * @param rowOperatorFunction The pointer to the row operator function, must be valid
		 * @param firstRow The first row to be handled, with range [0, height - 1]
		 * @param numberRows The number of rows to be handled, with range [1, height - firstRow]
		 * @tparam TSource The data type of the source elements
		 * @tparam TTarget The data type of the target elements
		 * @tparam tSourceChannels The number of channels the source frame has, with range [1, infinity)
		 * @tparam tTargetChannels The number of channels the target frame has, with range [1, infinity)
		 */
		template <typename TSource, typename TTarget, unsigned int tSourceChannels, unsigned int tTargetChannels>
		static void applyRowOperatorSubset(const TSource* source, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int sourceStrideElements, const unsigned int targetStrideElements, const RowOperatorFunction<TSource, TTarget, tSourceChannels, tTargetChannels> rowOperatorFunction, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Transforms a subset of a frame with generic pixel format (with zipped pixel information) like RGB24 or YUV24, to a frame with same pixel format and channel number.
		 * @param source The source frame buffer, must be valid
		 * @param target The target frame buffer, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param conversionFlag The conversion to be applied
		 * @param rowReversePixelOrderFunction The function able to reverse the pixel order, must be valid
		 * @param bytesPerRow The actual number of bytes each row covers, not including optional padding bytes at the end of each row, with range [width, infinity)
		 * @param sourceStrideBytes The number of bytes between to start points of successive rows in the source frame, with range [0, infinity)
		 * @param targetStrideBytes The number of bytes between to start points of successive rows in the target frame, with range [0, infinity)
		 * @param firstRow The first row to be handled, with range [0, height - 1]
		 * @param numberRows The number of rows to be handled, with range [1, height - firstRow]
		 */
		static void transformGenericSubset(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const RowReversePixelOrderFunction<void> rowReversePixelOrderFunction, const unsigned int bytesPerRow, const unsigned int sourceStrideBytes, const unsigned int targetStrideBytes, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Converts an image with premultiplied alpha to a straight image (without premultiplied alpha).
		 * @param frame The image to convert, must be valid
		 * @param width The width of the image in pixel, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param firstRow The first row to be handled, with range [0, height - 1]
		 * @param numberRows The number of rows to be handled, with range [1, height - firstRow]
		 * @tparam tChannels The number of frame channels, with range [2, infinity)
		 * @tparam tAlphaChannelIndex The index of the alpha channel, with range [0, tChannels - 1]
		 */
		template <unsigned int tChannels, unsigned int tAlphaChannelIndex>
		static void premultipliedAlphaToStraightAlpha8BitPerChannelSubset(uint8_t* const frame, const unsigned int width, const unsigned int framePaddingElements, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Converts an image with premultiplied alpha to a straight image (without premultiplied alpha).
		 * @param source The source image to convert, must be valid
		 * @param target The resulting converted target image, must be valid
		 * @param width The width of the image in pixel, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param firstRow The first row to be handled, with range [0, height - 1]
		 * @param numberRows The number of rows to be handled, with range [1, height - firstRow]
		 * @tparam tChannels The number of frame channels, with range [2, infinity)
		 * @tparam tAlphaChannelIndex The index of the alpha channel, with range [0, tChannels - 1]
		 */
		template <unsigned int tChannels, unsigned int tAlphaChannelIndex>
		static void premultipliedAlphaToStraightAlpha8BitPerChannelSubset(const uint8_t* const source, uint8_t* const target, const unsigned int width, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Converts an image with straight alpha (without premultiplied alpha) to an image with premultiplied alpha.
		 * @param frame The image to convert, must be valid
		 * @param width The width of the image in pixel, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param firstRow The first row to be handled, with range [0, height - 1]
		 * @param numberRows The number of rows to be handled, with range [1, height - firstRow]
		 * @tparam tChannels The number of frame channels, with range [2, infinity)
		 * @tparam tAlphaChannelIndex The index of the alpha channel, with range [0, tChannels - 1]
		 */
		template <unsigned int tChannels, unsigned int tAlphaChannelIndex>
		static void straightAlphaToPremultipliedAlpha8BitPerChannelSubset(uint8_t* const frame, const unsigned int width, const unsigned int framePaddingElements, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Converts an image with straight alpha (without premultiplied alpha) to an image with premultiplied alpha.
		 * @param source The source image to convert, must be valid
		 * @param target The resulting converted target image, must be valid
		 * @param width The width of the image in pixel, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param firstRow The first row to be handled, with range [0, height - 1]
		 * @param numberRows The number of rows to be handled, with range [1, height - firstRow]
		 * @tparam tChannels The number of frame channels, with range [2, infinity)
		 * @tparam tAlphaChannelIndex The index of the alpha channel, with range [0, tChannels - 1]
		 */
		template <unsigned int tChannels, unsigned int tAlphaChannelIndex>
		static void straightAlphaToPremultipliedAlpha8BitPerChannelSubset(const uint8_t* const source, uint8_t* const target, const unsigned int width, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows);

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

		/**
		 * Converts 16 pixels with 3 channels per pixel to 16 pixels with one channel per pixel by a linear combination of the three channels.
		 * This function can be used to e.g., convert RGB24 to Y8, or RGB24 to Y8.
		 * The linear combination is defined by one integer multiplication factor for each channel with 128 as denominator.<br>
		 * Beware: As this function applies integer multiplication factors (with 7 bits precision) the conversion result has an accuracy of +/- 2 color intensities.
		 * @param source The pointer to the 16 source pixels (with 3 channels = 64 bytes) to convert, must be valid
		 * @param target The pointer to the 16 target pixels (with 1 channel = 16 bytes) receiving the converted pixel data, must be valid
		 * @param multiplicationFactors0_128_u_16x8 The multiplication factor for the first channel (8 identical 16 bit values), with ranges [0, 128], while the sum of all three factors must be 128
		 * @param multiplicationFactors1_128_u_16x8 The multiplication factor for the second channel (8 identical 16 bit values), with ranges [0, 128], while the sum of all three factors must be 128
		 * @param multiplicationFactors2_128_u_16x8 The multiplication factor for the third channel (8 identical 16 bit values), with ranges [0, 128], while the sum of all three factors must be 128
		 */
		static OCEAN_FORCE_INLINE void convert3ChannelsTo1Channel16Pixels8BitPerChannel7BitPrecisionSSE(const uint8_t* const source, uint8_t* const target, const __m128i& multiplicationFactors0_128_u_16x8, const __m128i& multiplicationFactors1_128_u_16x8, const __m128i& multiplicationFactors2_128_u_16x8);

		/**
		 * Converts 16 pixels with 3 channels per pixel to 16 pixels with three channel per pixel by a linear combination of the three channels plus a bias (translation) parameter.
		 * Thus, this function can be used to e.g., convert RGB24 to YUV24, or YUV24 to RGB24.
		 * The linear combination is defined by three integer multiplication factor for each source channel with 128 as denominator. plus one bias (translation) parameter for each source channel (with 1 as denominator).<br>
		 * Beware: As this function applies integer multiplication factors (with 7 bits precision) the conversion result has an accuracy of +/- 2 color intensities.<br>
		 * The transformation is based on the following pattern:
		 * <pre>
		 * t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
		 * t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
		 * t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2
		 * </pre>
		 * With t target, s source, f factor, and b bias.
		 * @param source The pointer to the 16 source pixels (with 3 channels = 48 bytes) to convert, must be valid
		 * @param target The pointer to the 16 target pixels (with 3 channels = 48 bytes) receiving the converted pixel data, must be valid
		 * @param factorChannel00_128_s_16x8 The multiplication factor (8 identical factors) for the first source channel and for the first target channel, with range [-127, 127]
		 * @param factorChannel10_128_s_16x8 The multiplication factor (8 identical factors) for the first source channel and for the second target channel, with range [-127, 127]
		 * @param factorChannel20_128_s_16x8 The multiplication factor (8 identical factors) for the first source channel and for the third target channel, with range [-127, 127]
		 * @param factorChannel01_128_s_16x8 The multiplication factor (8 identical factors) for the second source channel and for the first target channel, with range [-127, 127]
		 * @param factorChannel11_128_s_16x8 The multiplication factor (8 identical factors) for the second source channel and for the second target channel, with range [-127, 127]
		 * @param factorChannel21_128_s_16x8 The multiplication factor (8 identical factors) for the second source channel and for the third target channel, with range [-127, 127]
		 * @param factorChannel02_128_s_16x8 The multiplication factor (8 identical factors) for the third source channel and for the first target channel, with range [-127, 127]
		 * @param factorChannel12_128_s_16x8 The multiplication factor (8 identical factors) for the third source channel and for the second target channel, with range [-127, 127
		 * @param factorChannel22_128_s_16x8 The multiplication factor (8 identical factors) for the third source channel and for the third target channel, with range [-127, 127]
		 * @param biasChannel0_s_16x8 The bias (translation) value for the first target channel, with range [-127, 127]
		 * @param biasChannel1_s_16x8 The bias (translation) value for the second target channel, with range [-127, 127]
		 * @param biasChannel2_s_16x8 The bias (translation) value for the third target channel, with range [-127, 127]
		 */
		static OCEAN_FORCE_INLINE void convert3ChannelsTo3Channels16Pixels8BitPerChannel7BitPrecisionSSE(const uint8_t* const source, uint8_t* const target, const __m128i& factorChannel00_128_s_16x8, const __m128i& factorChannel10_128_s_16x8, const __m128i& factorChannel20_128_s_16x8, const __m128i& factorChannel01_128_s_16x8, const __m128i& factorChannel11_128_s_16x8, const __m128i& factorChannel21_128_s_16x8, const __m128i& factorChannel02_128_s_16x8, const __m128i& factorChannel12_128_s_16x8, const __m128i& factorChannel22_128_s_16x8, const __m128i& biasChannel0_s_16x8, const __m128i& biasChannel1_s_16x8, const __m128i& biasChannel2_s_16x8);

		/**
		 * Converts 16 pixels with 3 channels per pixel to 16 pixels with three channel per pixel by a linear combination of the three channels plus a bias (translation) parameter.
		 * Thus, this function can be used to e.g., convert RGB24 to YUV24, or YUV24 to RGB24.
		 * The linear combination is defined by three integer multiplication factor for each source channel with 1024 as denominator. plus one bias (translation) parameter for each source channel (with 1 as denominator).<br>
		 * Beware: As this function applies integer multiplication factors (with 10 bits precision) the conversion result has an accuracy of +/- 1 color intensities.<br>
		 * The transformation is based on the following pattern:
		 * <pre>
		 * t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
		 * t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
		 * t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2
		 * </pre>
		 * With t target, s source, f factor, and b bias.
		 * @param source The pointer to the 16 source pixels (with 3 channels = 48 bytes) to convert, must be valid
		 * @param target The pointer to the 16 target pixels (with 3 channels = 48 bytes) receiving the converted pixel data, must be valid
		 * @param factorChannel00_1024_s_16x8 The multiplication factor (8 identical factors) for the first source channel and for the first target channel, with range [-1024 * 16, 1024 * 16]
		 * @param factorChannel10_1024_s_16x8 The multiplication factor (8 identical factors) for the first source channel and for the second target channel, with range [-1024 * 16, 1024 * 16]
		 * @param factorChannel20_1024_s_16x8 The multiplication factor (8 identical factors) for the first source channel and for the third target channel, with range [-1024 * 16, 1024 * 16]
		 * @param factorChannel01_1024_s_16x8 The multiplication factor (8 identical factors) for the second source channel and for the first target channel, with range [-1024 * 16, 1024 * 16]
		 * @param factorChannel11_1024_s_16x8 The multiplication factor (8 identical factors) for the second source channel and for the second target channel, with range [-1024 * 16, 1024 * 16]
		 * @param factorChannel21_1024_s_16x8 The multiplication factor (8 identical factors) for the second source channel and for the third target channel, with range [-1024 * 16, 1024 * 16]
		 * @param factorChannel02_1024_s_16x8 The multiplication factor (8 identical factors) for the third source channel and for the first target channel, with range [-1024 * 16, 1024 * 16]
		 * @param factorChannel12_1024_s_16x8 The multiplication factor (8 identical factors) for the third source channel and for the second target channel, with range [-1024 * 16, 1024 * 16]
		 * @param factorChannel22_1024_s_16x8 The multiplication factor (8 identical factors) for the third source channel and for the third target channel, with range [-1024 * 16, 1024 * 16]
		 * @param biasChannel0_1024_s_32x4 The bias (translation) value for the first target channel, with range [-1024 * 16, 1024 * 16]
		 * @param biasChannel1_1024_s_32x4 The bias (translation) value for the second target channel, with range [-1024 * 16, 1024 * 16]
		 * @param biasChannel2_1024_s_32x4 The bias (translation) value for the third target channel, with range [-1024 * 16, 1024 * 16]
		 */
		static OCEAN_FORCE_INLINE void convert3ChannelsTo3Channels16Pixels8BitPerChannel10BitPrecisionSSE(const uint8_t* const source, uint8_t* const target, const __m128i& factorChannel00_1024_s_16x8, const __m128i& factorChannel10_1024_s_16x8, const __m128i& factorChannel20_1024_s_16x8, const __m128i& factorChannel01_1024_s_16x8, const __m128i& factorChannel11_1024_s_16x8, const __m128i& factorChannel21_1024_s_16x8, const __m128i& factorChannel02_1024_s_16x8, const __m128i& factorChannel12_1024_s_16x8, const __m128i& factorChannel22_1024_s_16x8, const __m128i& biasChannel0_1024_s_32x4, const __m128i& biasChannel1_1024_s_32x4, const __m128i& biasChannel2_1024_s_32x4);

		/**
		 * Converts 16 pixels with 4 channels per pixel to 16 pixels with one channel per pixel by a linear combination of the four channels.
		 * This function can be used to e.g., convert RGBA32 to Y8, or ARGB32 to Y8, or RGB32 to Y8.
		 * The linear combination is defined by one integer multiplication factor for each channel with 128 as denominator.<br>
		 * Beware: As this function applies integer multiplication factors (with 7 bits precision) the conversion result has an accuracy of +/- 2 color intensities.
		 * @param source The pointer to the 16 source pixels (with 4 channels = 64 bytes) to convert, must be valid
		 * @param target The pointer to the 16 target pixels (with 1 channel = 16 bytes) receiving the converted pixel data, must be valid
		 * @param multiplicationFactors0123_128_s_32x The four individual multiplication factors, one for each channel, with ranges [0, 127], while the sum of all four factors must be 128
		 */
		static OCEAN_FORCE_INLINE void convert4ChannelsTo1Channel16Pixels8BitPerChannel7BitPrecisionSSE(const uint8_t* const source, uint8_t* const target, const __m128i& multiplicationFactors0123_128_s_32x);

		/**
		 * Converts 16 pixels with 4 channels per pixel to 16 pixels with two channel per pixel by a linear combination of the four channels.
		 * This function can be used to e.g., convert RGBA32 to YA16, or ARGB32 to AY16.
		 * The linear combination is defined by one integer multiplication factor for each channel with 128 as denominator.<br>
		 * Beware: As this function applies integer multiplication factors (with 7 bits precision) the conversion result has an accuracy of +/- 2 color intensities.
		 * @param source The pointer to the 16 source pixels (with 4 channels = 64 bytes) to convert, must be valid
		 * @param target The pointer to the 16 target pixels (with 2 channel = 32 bytes) receiving the converted pixel data, must be valid
		 * @param multiplicationFactorsChannel0_0123_128_s_16x8 The four individual multiplication factors for the first target channel (two sets), one for each source channel, with ranges [0, 128], while the sum of all four factors must be 128
		 * @param multiplicationFactorsChannel1_0123_128_s_16x8 The four individual multiplication factors for the second target channel (two sets), one for each source channel, with ranges [0, 128], while the sum of all four factors must be 128
		 */
		static OCEAN_FORCE_INLINE void convert4ChannelsTo2Channels16Pixels8BitPerChannel7BitPrecisionSSE(const uint8_t* const source, uint8_t* const target, const __m128i& multiplicationFactorsChannel0_0123_128_s_16x8, const __m128i& multiplicationFactorsChannel1_0123_128_s_16x8);

#endif // OCEAN_HARDWARE_SSE_VERSION >= 41

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

		/**
		 * Converts 8 pixels with 3 channels per pixel to 8 pixels with one channel per pixel by a linear combination of the three channels.
		 * Thus, this function can be used to e.g., convert RGB24 to Y8, or BGR24 to Y8.
		 * The linear combination is defined by one integer multiplication factor for each channel with 128 as denominator.<br>
		 * Beware: As this function applies integer multiplication factors (with 7 bits precision) the conversion result has an accuracy of +/- 2 color intensities.
		 * @param source The pointer to the 8 source pixels (with 3 channels = 24 bytes) to convert, must be valid
		 * @param target The pointer to the 8 target pixels (with 1 channel = 8 bytes) receiving the converted pixel data, must be valid
		 * @param factorChannel0_128_u_8x8 The multiplication factor (8 identical factors) for the first channel, with range [0, 128]
		 * @param factorChannel1_128_u_8x8 The multiplication factor (8 identical factors) for the second channel, with range [0, 128 - factorChannel0 - factorChannel2]
		 * @param factorChannel2_128_u_8x8 The multiplication factor (8 identical factors) for the third channel, with range [0, 128 - factorChannel0 - factorChannel1]
		 * @tparam tUseFactorChannel0 True, if the value(s) of factorChannel0 is not zero; False, if the value(s) of factorChannel0 is zero
		 * @tparam tUseFactorChannel1 True, if the value(s) of factorChannel1 is not zero; False, if the value(s) of factorChannel1 is zero
		 * @tparam tUseFactorChannel2 True, if the value(s) of factorChannel2 is not zero; False, if the value(s) of factorChannel2 is zero
		 */
		template <bool tUseFactorChannel0, bool tUseFactorChannel1, bool tUseFactorChannel2>
		static OCEAN_FORCE_INLINE void convert3ChannelsTo1Channel8Pixels8BitPerChannel7BitPrecisionNEON(const uint8_t* const source, uint8_t* const target, const uint8x8_t& factorChannel0_128_u_8x8, const uint8x8_t& factorChannel1_128_u_8x8, const uint8x8_t& factorChannel2_128_u_8x8);

		/**
		 * Converts 8 pixels with 3 channels per pixel to 8 pixels with three channel per pixel by a linear combination of the three channels plus an in advance bias (translation) parameter.
		 * Thus, this function can be used to e.g., convert RGB24 to YUV24, or RGB24 to YUV24.
		 * The linear combination is defined by three integer multiplication factor for each source channel with 64 as denominator. plus one bias (translation) parameter for each source channel (with 1 as denominator).<br>
		 * Beware: As this function applies integer multiplication factors (with 6 bits precision) the conversion result has an accuracy of +/- 4 color intensities.<br>
		 * The transformation is based on the following pattern:
		 * <pre>
		 * t0 = clamp(0, f00 * (s0 - b0) + f01 * (s1 - b1) + f02 * (s2 - b2), 255)
		 * t1 = clamp(0, f10 * (s0 - b0) + f11 * (s1 - b1) + f12 * (s2 - b2), 255)
		 * t2 = clamp(0, f20 * (s0 - b0) + f21 * (s1 - b1) + f22 * (s2 - b2), 255)
		 * </pre>
		 * With t target, s source, f factor, and b bias/translation.
		 * @param source The pointer to the 8 source pixels (with 3 channels = 24 bytes) to convert, must be valid
		 * @param target The pointer to the 8 target pixels (with 3 channels = 24 bytes) receiving the converted pixel data, must be valid
		 * @param factorChannel00_64_s_16x8 The multiplication factor (8 identical factors) for the first source channel and for the first target channel, with range [-127, 127]
		 * @param factorChannel10_64_s_16x8 The multiplication factor (8 identical factors) for the first source channel and for the second target channel, with range [-127, 127]
		 * @param factorChannel20_64_s_16x8 The multiplication factor (8 identical factors) for the first source channel and for the third target channel, with range [-127, 127]
		 * @param factorChannel01_64_s_16x8 The multiplication factor (8 identical factors) for the second source channel and for the first target channel, with range [-127, 127]
		 * @param factorChannel11_64_s_16x8 The multiplication factor (8 identical factors) for the second source channel and for the second target channel, with range [-127, 127]
		 * @param factorChannel21_64_s_16x8 The multiplication factor (8 identical factors) for the second source channel and for the third target channel, with range [-127, 127]
		 * @param factorChannel02_64_s_16x8 The multiplication factor (8 identical factors) for the third source channel and for the first target channel, with range [-127, 127]
		 * @param factorChannel12_64_s_16x8 The multiplication factor (8 identical factors) for the third source channel and for the second target channel, with range [-127, 127]
		 * @param factorChannel22_64_s_16x8 The multiplication factor (8 identical factors) for the third source channel and for the third target channel, with range [-127, 127]
		 * @param biasChannel0_u_8x8 The bias (translation) value for the first target channel, with range [0, 128]
		 * @param biasChannel1_u_8x8 The bias (translation) value for the second target channel, with range [0, 128]
		 * @param biasChannel2_u_8x8 The bias (translation) value for the third target channel, with range [0, 128]
		 */
		static OCEAN_FORCE_INLINE void convert3ChannelsTo3Channels8Pixels8BitPerChannel6BitPrecisionNEON(const uint8_t* const source, uint8_t* const target, const int16x8_t& factorChannel00_64_s_16x8, const int16x8_t& factorChannel10_64_s_16x8, const int16x8_t& factorChannel20_64_s_16x8, const int16x8_t& factorChannel01_64_s_16x8, const int16x8_t& factorChannel11_64_s_16x8, const int16x8_t& factorChannel21_64_s_16x8, const int16x8_t& factorChannel02_64_s_16x8, const int16x8_t& factorChannel12_64_s_16x8, const int16x8_t& factorChannel22_64_s_16x8, const uint8x8_t& biasChannel0_u_8x8, const uint8x8_t& biasChannel1_u_8x8, const uint8x8_t& biasChannel2_u_8x8);

		/**
		 * Converts 16 pixels with 3 channels per pixel to 16 pixels with three channel per pixel by a linear combination of the three channels plus an in advance bias (translation) parameter.
		 * Thus, this function can be used to e.g., convert RGB24 to YUV24, or RGB24 to YUV24.
		 * The linear combination is defined by three integer multiplication factor for each source channel with 64 as denominator. plus one bias (translation) parameter for each source channel (with 1 as denominator).<br>
		 * Beware: As this function applies integer multiplication factors (with 6 bits precision) the conversion result has an accuracy of +/- 4 color intensities.<br>
		 * The transformation is based on the following pattern:
		 * <pre>
		 * t0 = clamp(0, f00 * (s0 - b0) + f01 * (s1 - b1) + f02 * (s2 - b2), 255)
		 * t1 = clamp(0, f10 * (s0 - b0) + f11 * (s1 - b1) + f12 * (s2 - b2), 255)
		 * t2 = clamp(0, f20 * (s0 - b0) + f21 * (s1 - b1) + f22 * (s2 - b2), 255)
		 * </pre>
		 * With t target, s source, f factor, and b bias/translation.
		 * @param source The pointer to the 16 source pixels (with 3 channels = 48 bytes) to convert, must be valid
		 * @param target The pointer to the 16 target pixels (with 3 channels = 48 bytes) receiving the converted pixel data, must be valid
		 * @param factorChannel00_64_s_16x8 The multiplication factor (8 identical factors) for the first source channel and for the first target channel, with range [-127, 127]
		 * @param factorChannel10_64_s_16x8 The multiplication factor (8 identical factors) for the first source channel and for the second target channel, with range [-127, 127]
		 * @param factorChannel20_64_s_16x8 The multiplication factor (8 identical factors) for the first source channel and for the third target channel, with range [-127, 127]
		 * @param factorChannel01_64_s_16x8 The multiplication factor (8 identical factors) for the second source channel and for the first target channel, with range [-127, 127]
		 * @param factorChannel11_64_s_16x8 The multiplication factor (8 identical factors) for the second source channel and for the second target channel, with range [-127, 127]
		 * @param factorChannel21_64_s_16x8 The multiplication factor (8 identical factors) for the second source channel and for the third target channel, with range [-127, 127]
		 * @param factorChannel02_64_s_16x8 The multiplication factor (8 identical factors) for the third source channel and for the first target channel, with range [-127, 127]
		 * @param factorChannel12_64_s_16x8 The multiplication factor (8 identical factors) for the third source channel and for the second target channel, with range [-127, 127]
		 * @param factorChannel22_64_s_16x8 The multiplication factor (8 identical factors) for the third source channel and for the third target channel, with range [-127, 127]
		 * @param biasChannel0_u_8x8 The bias (translation) value for the first target channel, with range [0, 128]
		 * @param biasChannel1_u_8x8 The bias (translation) value for the second target channel, with range [0, 128]
		 * @param biasChannel2_u_8x8 The bias (translation) value for the third target channel, with range [0, 128]
		 */
		static OCEAN_FORCE_INLINE void convert3ChannelsTo3Channels16Pixels8BitPerChannel6BitPrecisionNEON(const uint8_t* const source, uint8_t* const target, const int16x8_t& factorChannel00_64_s_16x8, const int16x8_t& factorChannel10_64_s_16x8, const int16x8_t& factorChannel20_64_s_16x8, const int16x8_t& factorChannel01_64_s_16x8, const int16x8_t& factorChannel11_64_s_16x8, const int16x8_t& factorChannel21_64_s_16x8, const int16x8_t& factorChannel02_64_s_16x8, const int16x8_t& factorChannel12_64_s_16x8, const int16x8_t& factorChannel22_64_s_16x8, const uint8x8_t& biasChannel0_u_8x8, const uint8x8_t& biasChannel1_u_8x8, const uint8x8_t& biasChannel2_u_8x8);

		/**
		 * Converts 8 pixels with 3 channels per pixel to 8 pixels with three channel per pixel by a linear combination of the three channels plus a bias (translation) parameter.
		 * Thus, this function can be used to e.g., convert RGB24 to YUV24, or YUV24 to RGB24.
		 * The linear combination is defined by three integer multiplication factor for each source channel with 128 as denominator. plus one bias (translation) parameter for each source channel (also with 128 as denominator).<br>
		 * Beware: As this function applies integer multiplication factors (with 7 bits precision) the conversion result has an accuracy of +/- 2 color intensities.<br>
		 * The transformation is based on the following pattern:
		 * <pre>
		 * t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
		 * t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
		 * t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2
		 * </pre>
		 * With t target, s source, f factor, and b bias.
		 * @param source The pointer to the 8 source pixels (with 3 channels = 24 bytes) to convert, must be valid
		 * @param target The pointer to the 8 target pixels (with 3 channels = 24 bytes) receiving the converted pixel data, must be valid
		 * @param factorChannel00_128_s_16x8 The multiplication factor (8 identical factors) for the first source channel and for the first target channel, with range [-127, 127]
		 * @param factorChannel10_128_s_16x8 The multiplication factor (8 identical factors) for the first source channel and for the second target channel, with range [-127, 127]
		 * @param factorChannel20_128_s_16x8 The multiplication factor (8 identical factors) for the first source channel and for the third target channel, with range [-127, 127]
		 * @param factorChannel01_128_s_16x8 The multiplication factor (8 identical factors) for the second source channel and for the first target channel, with range [-127, 127]
		 * @param factorChannel11_128_s_16x8 The multiplication factor (8 identical factors) for the second source channel and for the second target channel, with range [-127, 127]
		 * @param factorChannel21_128_s_16x8 The multiplication factor (8 identical factors) for the second source channel and for the third target channel, with range [-127, 127]
		 * @param factorChannel02_128_s_16x8 The multiplication factor (8 identical factors) for the third source channel and for the first target channel, with range [-127, 127]
		 * @param factorChannel12_128_s_16x8 The multiplication factor (8 identical factors) for the third source channel and for the second target channel, with range [-127, 127]
		 * @param factorChannel22_128_s_16x8 The multiplication factor (8 identical factors) for the third source channel and for the third target channel, with range [-127, 127]
		 * @param biasChannel0_128_s_16x8 The bias (translation) value for the first target channel, with range [-128 * 128, 128 * 128]
		 * @param biasChannel1_128_s_16x8 The bias (translation) value for the second target channel, with range [-128 * 128, 128 * 128]
		 * @param biasChannel2_128_s_16x8 The bias (translation) value for the third target channel, with range [-128 * 128, 128 * 128]
		 */
		static OCEAN_FORCE_INLINE void convert3ChannelsTo3Channels8Pixels8BitPerChannel7BitPrecisionNEON(const uint8_t* const source, uint8_t* const target, const int16x8_t& factorChannel00_128_s_16x8, const int16x8_t& factorChannel10_128_s_16x8, const int16x8_t& factorChannel20_128_s_16x8, const int16x8_t& factorChannel01_128_s_16x8, const int16x8_t& factorChannel11_128_s_16x8, const int16x8_t& factorChannel21_128_s_16x8, const int16x8_t& factorChannel02_128_s_16x8, const int16x8_t& factorChannel12_128_s_16x8, const int16x8_t& factorChannel22_128_s_16x8, const int16x8_t& biasChannel0_128_s_16x8, const int16x8_t& biasChannel1_128_s_16x8, const int16x8_t& biasChannel2_128_s_16x8);

		/**
		 * Converts 8 pixels with 3 channels per pixel to 8 pixels with three channel per pixel by a linear combination of the three channels plus a bias (translation) parameter.
		 * Thus, this function can be used to e.g., convert YUV24 to RGB24, or YVU24 to BGR24.
		 * The linear combination is defined by three integer multiplication factor for each source channel with 1024 as denominator. plus one bias (translation) parameter for each source channel (also with 1024 as denominator).<br>
		 * Beware: As this function applies integer multiplication factors (with 10 bits precision) the conversion result has an accuracy of +/- 1 color intensities.<br>
		 * The transformation is based on the following pattern:
		 * <pre>
		 * t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
		 * t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
		 * t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2
		 * </pre>
		 * With t target, s source, f factor, and b bias.
		 * @param source The pointer to the 8 source pixels (with 3 channels = 24 bytes) to convert, must be valid
		 * @param target The pointer to the 8 target pixels (with 3 channels = 24 bytes) receiving the converted pixel data, must be valid
		 * @param factorChannel00_1024_s_16x4 The multiplication factor (4 identical factors) for the first source channel and for the first target channel, with range [-32767, 32767]
		 * @param factorChannel10_1024_s_16x4 The multiplication factor (4 identical factors) for the first source channel and for the second target channel, with range [-32767, 32767]
		 * @param factorChannel20_1024_s_16x4 The multiplication factor (4 identical factors) for the first source channel and for the third target channel, with range [-32767, 32767]
		 * @param factorChannel01_1024_s_16x4 The multiplication factor (4 identical factors) for the second source channel and for the first target channel, with range [-32767, 32767]
		 * @param factorChannel11_1024_s_16x4 The multiplication factor (4 identical factors) for the second source channel and for the second target channel, with range [-32767, 32767]
		 * @param factorChannel21_1024_s_16x4 The multiplication factor (4 identical factors) for the second source channel and for the third target channel, with range [-32767, 32767]
		 * @param factorChannel02_1024_s_16x4 The multiplication factor (4 identical factors) for the third source channel and for the first target channel, with range [-32767, 32767]
		 * @param factorChannel12_1024_s_16x4 The multiplication factor (4 identical factors) for the third source channel and for the second target channel, with range [-32767, 32767
		 * @param factorChannel22_1024_s_16x4 The multiplication factor (4 identical factors) for the third source channel and for the third target channel, with range [-32767, 32767]
		 * @param biasChannel0_1024_s_32x4 The bias (translation) value for the first target channel, with range [-32767, 32767]
		 * @param biasChannel1_1024_s_32x4 The bias (translation) value for the second target channel, with range [-32767, 32767]
		 * @param biasChannel2_1024_s_32x4 The bias (translation) value for the third target channel, with range [-32767, 32767]
		 */
		static OCEAN_FORCE_INLINE void convert3ChannelsTo3Channels8Pixels8BitPerChannel10BitPrecisionNEON(const uint8_t* const source, uint8_t* const target, const int16x4_t& factorChannel00_1024_s_16x4, const int16x4_t& factorChannel10_1024_s_16x4, const int16x4_t& factorChannel20_1024_s_16x4, const int16x4_t& factorChannel01_1024_s_16x4, const int16x4_t& factorChannel11_1024_s_16x4, const int16x4_t& factorChannel21_1024_s_16x4, const int16x4_t& factorChannel02_1024_s_16x4, const int16x4_t& factorChannel12_1024_s_16x4, const int16x4_t& factorChannel22_1024_s_16x4, const int32x4_t& biasChannel0_1024_s_32x4, const int32x4_t& biasChannel1_1024_s_32x4, const int32x4_t& biasChannel2_1024_s_32x4);

		/**
		 * Converts 8 pixels with 3 channels per pixel to 16 pixels with three channel per pixel by a linear combination of the three channels plus a bias (translation) parameter.
		 * Thus, this function can be used to e.g., convert YUV24 to RGB24, or YVU24 to BGR24.
		 * The linear combination is defined by three integer multiplication factor for each source channel with 1024 as denominator. plus one bias (translation) parameter for each source channel (also with 1024 as denominator).<br>
		 * Beware: As this function applies integer multiplication factors (with 10 bits precision) the conversion result has an accuracy of +/- 1 color intensities.<br>
		 * The transformation is based on the following pattern:
		 * <pre>
		 * t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
		 * t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
		 * t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2
		 * </pre>
		 * With t target, s source, f factor, and b bias.
		 * @param source The pointer to the 16 source pixels (with 3 channels = 24 bytes) to convert, must be valid
		 * @param target The pointer to the 16 target pixels (with 3 channels = 24 bytes) receiving the converted pixel data, must be valid
		 * @param factorChannel00_1024_s_16x4 The multiplication factor (4 identical factors) for the first source channel and for the first target channel, with range [-32767, 32767]
		 * @param factorChannel10_1024_s_16x4 The multiplication factor (4 identical factors) for the first source channel and for the second target channel, with range [-32767, 32767]
		 * @param factorChannel20_1024_s_16x4 The multiplication factor (4 identical factors) for the first source channel and for the third target channel, with range [-32767, 32767]
		 * @param factorChannel01_1024_s_16x4 The multiplication factor (4 identical factors) for the second source channel and for the first target channel, with range [-32767, 32767]
		 * @param factorChannel11_1024_s_16x4 The multiplication factor (4 identical factors) for the second source channel and for the second target channel, with range [-32767, 32767]
		 * @param factorChannel21_1024_s_16x4 The multiplication factor (4 identical factors) for the second source channel and for the third target channel, with range [-32767, 32767]
		 * @param factorChannel02_1024_s_16x4 The multiplication factor (4 identical factors) for the third source channel and for the first target channel, with range [-32767, 32767]
		 * @param factorChannel12_1024_s_16x4 The multiplication factor (4 identical factors) for the third source channel and for the second target channel, with range [-32767, 32767
		 * @param factorChannel22_1024_s_16x4 The multiplication factor (4 identical factors) for the third source channel and for the third target channel, with range [-32767, 32767]
		 * @param biasChannel0_1024_s_32x4 The bias (translation) value for the first target channel, with range [-32767, 32767]
		 * @param biasChannel1_1024_s_32x4 The bias (translation) value for the second target channel, with range [-32767, 32767]
		 * @param biasChannel2_1024_s_32x4 The bias (translation) value for the third target channel, with range [-32767, 32767]
		 */
		static OCEAN_FORCE_INLINE void convert3ChannelsTo3Channels16Pixels8BitPerChannel10BitPrecisionNEON(const uint8_t* const source, uint8_t* const target, const int16x4_t& factorChannel00_1024_s_16x4, const int16x4_t& factorChannel10_1024_s_16x4, const int16x4_t& factorChannel20_1024_s_16x4, const int16x4_t& factorChannel01_1024_s_16x4, const int16x4_t& factorChannel11_1024_s_16x4, const int16x4_t& factorChannel21_1024_s_16x4, const int16x4_t& factorChannel02_1024_s_16x4, const int16x4_t& factorChannel12_1024_s_16x4, const int16x4_t& factorChannel22_1024_s_16x4, const int32x4_t& biasChannel0_1024_s_32x4, const int32x4_t& biasChannel1_1024_s_32x4, const int32x4_t& biasChannel2_1024_s_32x4);

		/**
		 * Converts 16 pixels with 3 channels per pixel to 16 pixels with 3 channels per pixel by a linear combination of the three channels plus a bias (translation) parameter.
		 * Thus, this function can be used to e.g., convert RGB24 to YUV24, or YUV24 to RGB24.
		 * The linear combination is defined by three integer multiplication factor for each source channel with 128 as denominator. plus one bias (translation) parameter for each source channel (with 128 as denominator).<br>
		 * Beware: As this function applies integer multiplication factors (with 7 bits precision) the conversion result has an accuracy of +/- 2 color intensities.<br>
		 * The transformation is based on the following pattern:
		 * <pre>
		 * t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
		 * t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
		 * t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2
		 * </pre>
		 * With t target, s source, f factor, and b bias.
		 * @param source The pointer to the 16 source pixels (with 3 channels = 48 bytes) to convert, must be valid
		 * @param target The pointer to the 16 target pixels (with 3 channels = 48 bytes) receiving the converted pixel data, must be valid
		 * @param factorChannel00_128_s_16x8 The multiplication factor (8 identical factors) for the first source channel and for the first target channel, with range [-127, 127]
		 * @param factorChannel10_128_s_16x8 The multiplication factor (8 identical factors) for the first source channel and for the second target channel, with range [-127, 127]
		 * @param factorChannel20_128_s_16x8 The multiplication factor (8 identical factors) for the first source channel and for the third target channel, with range [-127, 127]
		 * @param factorChannel01_128_s_16x8 The multiplication factor (8 identical factors) for the second source channel and for the first target channel, with range [-127, 127]
		 * @param factorChannel11_128_s_16x8 The multiplication factor (8 identical factors) for the second source channel and for the second target channel, with range [-127, 127]
		 * @param factorChannel21_128_s_16x8 The multiplication factor (8 identical factors) for the second source channel and for the third target channel, with range [-127, 127]
		 * @param factorChannel02_128_s_16x8 The multiplication factor (8 identical factors) for the third source channel and for the first target channel, with range [-127, 127]
		 * @param factorChannel12_128_s_16x8 The multiplication factor (8 identical factors) for the third source channel and for the second target channel, with range [-127, 127
		 * @param factorChannel22_128_s_16x8 The multiplication factor (8 identical factors) for the third source channel and for the third target channel, with range [-127, 127]
		 * @param biasChannel0_128_s_16x8 The bias (translation) value for the first target channel, with range [-128 * 128, 128 * 128]
		 * @param biasChannel1_128_s_16x8 The bias (translation) value for the second target channel, with range [-128 * 128, 128 * 128]
		 * @param biasChannel2_128_s_16x8 The bias (translation) value for the third target channel, with range [-128 * 128, 128 * 128]
		 */
		static OCEAN_FORCE_INLINE void convert3ChannelsTo3Channels16Pixels8BitPerChannel7BitPrecisionNEON(const uint8_t* const source, uint8_t* const target, const int16x8_t& factorChannel00_128_s_16x8, const int16x8_t& factorChannel10_128_s_16x8, const int16x8_t& factorChannel20_128_s_16x8, const int16x8_t& factorChannel01_128_s_16x8, const int16x8_t& factorChannel11_128_s_16x8, const int16x8_t& factorChannel21_128_s_16x8, const int16x8_t& factorChannel02_128_s_16x8, const int16x8_t& factorChannel12_128_s_16x8, const int16x8_t& factorChannel22_128_s_16x8, const int16x8_t& biasChannel0_128_s_16x8, const int16x8_t& biasChannel1_128_s_16x8, const int16x8_t& biasChannel2_128_s_16x8);

		/**
		 * Converts 16 pixels with 3 channels per pixel to 16 pixels with 4 channels per pixel by a linear combination of the three channels plus a bias (translation) parameter.
		 * The fourth channel is set to a constant value, e.g., for an alpha channel.<br>
		 * Thus, this function can be used to e.g., convert YUV24 to RGBA32, or YVU24 to BGRA32.<br>
		 * The linear combination is defined by three integer multiplication factor for each source channel with 128 as denominator. plus one bias (translation) parameter for each source channel (with 128 as denominator).<br>
		 * Beware: As this function applies integer multiplication factors (with 7 bits precision) the conversion result has an accuracy of +/- 2 color intensities.<br>
		 * The transformation is based on the following pattern:
		 * <pre>
		 * t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
		 * t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
		 * t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2
		 * t3 = valueChannel3
		 * </pre>
		 * With t target, s source, f factor, and b bias.
		 * @param source The pointer to the 16 source pixels (with 3 channels = 48 bytes) to convert, must be valid
		 * @param target The pointer to the 16 target pixels (with 3 channels = 48 bytes) receiving the converted pixel data, must be valid
		 * @param factorChannel00_64_s_16x8 The multiplication factor (8 identical factors) for the first source channel and for the first target channel, with range [-127, 127]
		 * @param factorChannel10_64_s_16x8 The multiplication factor (8 identical factors) for the first source channel and for the second target channel, with range [-127, 127]
		 * @param factorChannel20_64_s_16x8 The multiplication factor (8 identical factors) for the first source channel and for the third target channel, with range [-127, 127]
		 * @param factorChannel01_64_s_16x8 The multiplication factor (8 identical factors) for the second source channel and for the first target channel, with range [-127, 127]
		 * @param factorChannel11_64_s_16x8 The multiplication factor (8 identical factors) for the second source channel and for the second target channel, with range [-127, 127]
		 * @param factorChannel21_64_s_16x8 The multiplication factor (8 identical factors) for the second source channel and for the third target channel, with range [-127, 127]
		 * @param factorChannel02_64_s_16x8 The multiplication factor (8 identical factors) for the third source channel and for the first target channel, with range [-127, 127]
		 * @param factorChannel12_64_s_16x8 The multiplication factor (8 identical factors) for the third source channel and for the second target channel, with range [-127, 127
		 * @param factorChannel22_64_s_16x8 The multiplication factor (8 identical factors) for the third source channel and for the third target channel, with range [-127, 127]
		 * @param biasChannel0_u_8x8 The bias (translation) value for the first target channel, with range [0, 128]
		 * @param biasChannel1_u_8x8 The bias (translation) value for the second target channel, with range [0, 128]
		 * @param biasChannel2_u_8x8 The bias (translation) value for the third target channel, with range [0, 138]
		 * @param channelValue3_u_8x16 The constant value for the fourth target channel, with range [0, 255]
		 */
		static OCEAN_FORCE_INLINE void convert3ChannelsTo4Channels16Pixels8BitPerChannel6BitPrecisionNEON(const uint8_t* const source, uint8_t* const target, const int16x8_t& factorChannel00_64_s_16x8, const int16x8_t& factorChannel10_64_s_16x8, const int16x8_t& factorChannel20_64_s_16x8, const int16x8_t& factorChannel01_64_s_16x8, const int16x8_t& factorChannel11_64_s_16x8, const int16x8_t& factorChannel21_64_s_16x8, const int16x8_t& factorChannel02_64_s_16x8, const int16x8_t& factorChannel12_64_s_16x8, const int16x8_t& factorChannel22_64_s_16x8, const uint8x8_t& biasChannel0_u_8x8, const uint8x8_t& biasChannel1_u_8x8, const uint8x8_t& biasChannel2_u_8x8, const uint8x16_t& channelValue3_u_8x16);

		/**
		 * Converts 8 pixels with 4 channels per pixel to 8 pixels with one channel per pixel by a linear combination of the four channels.
		 * Thus, this function can be used to e.g., convert RGBA32 to Y8, or ARGB32 to Y8, or RGB32 to Y8.
		 * The linear combination is defined by one integer multiplication factor for each channel with 128 as denominator.<br>
		 * Beware: As this function applies integer multiplication factors (with 7 bits precision) the conversion result has an accuracy of +/- 2 color intensities.
		 * @param source The pointer to the 8 source pixels (with 4 channels = 32 bytes) to convert, must be valid
		 * @param target The pointer to the 8 target pixels (with 1 channel = 8 bytes) receiving the converted pixel data, must be valid
		 * @param factorChannel0_128_u_8x8 The multiplication factor (8 identical factors) for the first channel, with range [0, 127]
		 * @param factorChannel1_128_u_8x8 The multiplication factor (8 identical factors) for the second channel, with range [0, 127 - factorChannel0 - factorChannel2 - factorChannel3]
		 * @param factorChannel2_128_u_8x8 The multiplication factor (8 identical factors) for the third channel, with range [0, 127 - factorChannel0 - factorChannel1 - factorChannel3]
		 * @param factorChannel3_128_u_8x8 The multiplication factor (8 identical factors) for the fourth channel, with range [0, 127 - factorChannel0 - factorChannel1 - factorChannel2]
		 * @tparam tUseFactorChannel0 True, if the value(s) of factorChannel0 is not zero; False, if the value(s) of factorChannel0 is zero
		 * @tparam tUseFactorChannel1 True, if the value(s) of factorChannel1 is not zero; False, if the value(s) of factorChannel1 is zero
		 * @tparam tUseFactorChannel2 True, if the value(s) of factorChannel2 is not zero; False, if the value(s) of factorChannel2 is zero
		 * @tparam tUseFactorChannel3 True, if the value(s) of factorChannel3 is not zero; False, if the value(s) of factorChannel3 is zero
		 */
		template <bool tUseFactorChannel0, bool tUseFactorChannel1, bool tUseFactorChannel2, bool tUseFactorChannel3>
		static OCEAN_FORCE_INLINE void convert4ChannelsTo1Channel8Pixels8BitPerChannel7BitPrecisionNEON(const uint8_t* const source, uint8_t* const target, const uint8x8_t& factorChannel0_128_u_8x8, const uint8x8_t& factorChannel1_128_u_8x8, const uint8x8_t& factorChannel2_128_u_8x8, const uint8x8_t& factorChannel3_128_u_8x8);

		/**
		 * Converts 8 pixels with 4 channels per pixel to 8 pixels with two channels per pixel by a linear combination of the four channels.
		 * Thus, this function can be used to e.g., convert RGBA32 to YA16, or ARGB32 to AY16.
		 * The linear combination is defined by one integer multiplication factor for each channel with 128 as denominator.<br>
		 * Beware: As this function applies integer multiplication factors (with 7 bits precision) the conversion result has an accuracy of +/- 2 color intensities.
		 * @param source The pointer to the 8 source pixels (with 4 channels = 32 bytes) to convert, must be valid
		 * @param target The pointer to the 8 target pixels (with 1 channel = 8 bytes) receiving the converted pixel data, must be valid
		 * @param factorChannel00_128_u_8x8 The multiplication factor (8 identical factors) for the first target and first source channel, with range [0, 127]
		 * @param factorChannel10_128_u_8x8 The multiplication factor (8 identical factors) for the second target and first source channel, with range [0, 127]
		 * @param factorChannel01_128_u_8x8 The multiplication factor (8 identical factors) for the first target and second source channel, with range [0, 127 - factorChannel00 - factorChannel02 - factorChannel03]
		 * @param factorChannel11_128_u_8x8 The multiplication factor (8 identical factors) for the second target and second source channel, with range [0, 127 - factorChannel10 - factorChannel12 - factorChannel13]
		 * @param factorChannel02_128_u_8x8 The multiplication factor (8 identical factors) for the first target and third source channel, with range [0, 127 - factorChannel00 - factorChannel01 - factorChannel03]
		 * @param factorChannel12_128_u_8x8 The multiplication factor (8 identical factors) for the second target and third source channel, with range [0, 127 - factorChannel10 - factorChannel11 - factorChannel13]
		 * @param factorChannel03_128_u_8x8 The multiplication factor (8 identical factors) for the first target and fourth source channel, with range [0, 127 - factorChannel00 - factorChannel01 - factorChannel02]
		 * @param factorChannel13_128_u_8x8 The multiplication factor (8 identical factors) for the second target and fourth source channel, with range [0, 127 - factorChannel10 - factorChannel11 - factorChannel12]
		 */
		static OCEAN_FORCE_INLINE void convert4ChannelsTo2Channels8Pixels8BitPerChannel7BitPrecisionNEON(const uint8_t* const source, uint8_t* const target, const uint8x8_t& factorChannel00_128_u_8x8, const uint8x8_t& factorChannel10_128_u_8x8, const uint8x8_t& factorChannel01_128_u_8x8, const uint8x8_t& factorChannel11_128_u_8x8, const uint8x8_t& factorChannel02_128_u_8x8, const uint8x8_t& factorChannel12_128_u_8x8, const uint8x8_t& factorChannel03_128_u_8x8, const uint8x8_t& factorChannel13_128_u_8x8);

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

};

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

template <>
inline void FrameChannels::separateTo1Channel<uint8_t, uint8_t, 2u>(const uint8_t* const sourceFrame, uint8_t* const* const targetFrames, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int sourceFramePaddingElements, const unsigned int* targetFramesPaddingElements)
{
	ocean_assert(sourceFrame != nullptr);
	ocean_assert(targetFrames != nullptr);

	ocean_assert(width != 0u && height != 0u);
	ocean_assert(channels == 2u);

	constexpr unsigned int tChannels = 2u;

	bool allTargetFramesContinuous = true;

	if (targetFramesPaddingElements != nullptr)
	{
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			if (targetFramesPaddingElements[n] != 0u)
			{
				allTargetFramesContinuous = false;
				break;
			}
		}
	}

	const uint8_t* source = sourceFrame;
	uint8_t* target0 = targetFrames[0];
	uint8_t* target1 = targetFrames[1];

	constexpr unsigned int tBlockSize = 16u;

	uint8x16x2_t source_8x16x2;

	if (allTargetFramesContinuous && sourceFramePaddingElements == 0u)
	{
		const unsigned int pixels = width * height;
		const unsigned int blocks = pixels / tBlockSize;
		const unsigned int remaining = pixels % tBlockSize;

		for (unsigned int n = 0u; n < blocks; ++n)
		{
			source_8x16x2 = vld2q_u8(source);

			vst1q_u8(target0, source_8x16x2.val[0]);
			vst1q_u8(target1, source_8x16x2.val[1]);

			source += tBlockSize * tChannels;

			target0 += tBlockSize;
			target1 += tBlockSize;
		}

		for (unsigned int n = 0u; n < remaining; ++n)
		{
			target0[n] = source[n * tChannels + 0u];
			target1[n] = source[n * tChannels + 1u];
		}
	}
	else
	{
		const unsigned int targetFrame0PaddingElements = targetFramesPaddingElements == nullptr ? 0u : targetFramesPaddingElements[0];
		const unsigned int targetFrame1PaddingElements = targetFramesPaddingElements == nullptr ? 0u : targetFramesPaddingElements[1];

		const unsigned int blocks = width / tBlockSize;
		const unsigned int remaining = width % tBlockSize;

		for (unsigned int y = 0u; y < height; ++y)
		{
			for (unsigned int n = 0u; n < blocks; ++n)
			{
				source_8x16x2 = vld2q_u8(source);

				vst1q_u8(target0, source_8x16x2.val[0]);
				vst1q_u8(target1, source_8x16x2.val[1]);

				source += tBlockSize * tChannels;

				target0 += tBlockSize;
				target1 += tBlockSize;
			}

			for (unsigned int n = 0u; n < remaining; ++n)
			{
				target0[n] = source[n * tChannels + 0u];
				target1[n] = source[n * tChannels + 1u];
			}

			source += remaining * tChannels + sourceFramePaddingElements;
			target0 += remaining + targetFrame0PaddingElements;
			target1 += remaining + targetFrame1PaddingElements;
		}
	}
}

template <>
inline void FrameChannels::separateTo1Channel<uint8_t, uint8_t, 3u>(const uint8_t* const sourceFrame, uint8_t* const* const targetFrames, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int sourceFramePaddingElements, const unsigned int* targetFramesPaddingElements)
{
	ocean_assert(sourceFrame != nullptr);
	ocean_assert(targetFrames != nullptr);

	ocean_assert(width != 0u && height != 0u);
	ocean_assert(channels == 3u);

	constexpr unsigned int tChannels = 3u;

	bool allTargetFramesContinuous = true;

	if (targetFramesPaddingElements != nullptr)
	{
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			if (targetFramesPaddingElements[n] != 0u)
			{
				allTargetFramesContinuous = false;
				break;
			}
		}
	}

	const uint8_t* source = sourceFrame;
	uint8_t* target0 = targetFrames[0];
	uint8_t* target1 = targetFrames[1];
	uint8_t* target2 = targetFrames[2];

	constexpr unsigned int tBlockSize = 16u;

	uint8x16x3_t source_8x16x3;

	if (allTargetFramesContinuous && sourceFramePaddingElements == 0u)
	{
		const unsigned int pixels = width * height;
		const unsigned int blocks = pixels / tBlockSize;
		const unsigned int remaining = pixels % tBlockSize;

		for (unsigned int n = 0u; n < blocks; ++n)
		{
			source_8x16x3 = vld3q_u8(source);

			vst1q_u8(target0, source_8x16x3.val[0]);
			vst1q_u8(target1, source_8x16x3.val[1]);
			vst1q_u8(target2, source_8x16x3.val[2]);

			source += tBlockSize * tChannels;

			target0 += tBlockSize;
			target1 += tBlockSize;
			target2 += tBlockSize;
		}

		for (unsigned int n = 0u; n < remaining; ++n)
		{
			target0[n] = source[n * tChannels + 0u];
			target1[n] = source[n * tChannels + 1u];
			target2[n] = source[n * tChannels + 2u];
		}
	}
	else
	{
		const unsigned int targetFrame0PaddingElements = targetFramesPaddingElements == nullptr ? 0u : targetFramesPaddingElements[0];
		const unsigned int targetFrame1PaddingElements = targetFramesPaddingElements == nullptr ? 0u : targetFramesPaddingElements[1];
		const unsigned int targetFrame2PaddingElements = targetFramesPaddingElements == nullptr ? 0u : targetFramesPaddingElements[2];

		const unsigned int blocks = width / tBlockSize;
		const unsigned int remaining = width % tBlockSize;

		for (unsigned int y = 0u; y < height; ++y)
		{
			for (unsigned int n = 0u; n < blocks; ++n)
			{
				source_8x16x3 = vld3q_u8(source);

				vst1q_u8(target0, source_8x16x3.val[0]);
				vst1q_u8(target1, source_8x16x3.val[1]);
				vst1q_u8(target2, source_8x16x3.val[2]);

				source += tBlockSize * tChannels;

				target0 += tBlockSize;
				target1 += tBlockSize;
				target2 += tBlockSize;
			}

			for (unsigned int n = 0u; n < remaining; ++n)
			{
				target0[n] = source[n * tChannels + 0u];
				target1[n] = source[n * tChannels + 1u];
				target2[n] = source[n * tChannels + 2u];
			}

			source += remaining * tChannels + sourceFramePaddingElements;
			target0 += remaining + targetFrame0PaddingElements;
			target1 += remaining + targetFrame1PaddingElements;
			target2 += remaining + targetFrame2PaddingElements;
		}
	}
}

template <>
inline void FrameChannels::separateTo1Channel<uint8_t, uint8_t, 4u>(const uint8_t* const sourceFrame, uint8_t* const* const targetFrames, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int sourceFramePaddingElements, const unsigned int* targetFramesPaddingElements)
{
	ocean_assert(sourceFrame != nullptr);
	ocean_assert(targetFrames != nullptr);

	ocean_assert(width != 0u && height != 0u);
	ocean_assert(channels == 4u);

	constexpr unsigned int tChannels = 4u;

	bool allTargetFramesContinuous = true;

	if (targetFramesPaddingElements != nullptr)
	{
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			if (targetFramesPaddingElements[n] != 0u)
			{
				allTargetFramesContinuous = false;
				break;
			}
		}
	}

	const uint8_t* source = sourceFrame;
	uint8_t* target0 = targetFrames[0];
	uint8_t* target1 = targetFrames[1];
	uint8_t* target2 = targetFrames[2];
	uint8_t* target3 = targetFrames[3];

	constexpr unsigned int tBlockSize = 16u;

	uint8x16x4_t source_8x16x4;

	if (allTargetFramesContinuous && sourceFramePaddingElements == 0u)
	{
		const unsigned int pixels = width * height;
		const unsigned int blocks = pixels / tBlockSize;
		const unsigned int remaining = pixels % tBlockSize;

		for (unsigned int n = 0u; n < blocks; ++n)
		{
			source_8x16x4 = vld4q_u8(source);

			vst1q_u8(target0, source_8x16x4.val[0]);
			vst1q_u8(target1, source_8x16x4.val[1]);
			vst1q_u8(target2, source_8x16x4.val[2]);
			vst1q_u8(target3, source_8x16x4.val[3]);

			source += tBlockSize * tChannels;

			target0 += tBlockSize;
			target1 += tBlockSize;
			target2 += tBlockSize;
			target3 += tBlockSize;
		}

		for (unsigned int n = 0u; n < remaining; ++n)
		{
			target0[n] = source[n * tChannels + 0u];
			target1[n] = source[n * tChannels + 1u];
			target2[n] = source[n * tChannels + 2u];
			target3[n] = source[n * tChannels + 3u];
		}
	}
	else
	{
		const unsigned int targetFrame0PaddingElements = targetFramesPaddingElements == nullptr ? 0u : targetFramesPaddingElements[0];
		const unsigned int targetFrame1PaddingElements = targetFramesPaddingElements == nullptr ? 0u : targetFramesPaddingElements[1];
		const unsigned int targetFrame2PaddingElements = targetFramesPaddingElements == nullptr ? 0u : targetFramesPaddingElements[2];
		const unsigned int targetFrame3PaddingElements = targetFramesPaddingElements == nullptr ? 0u : targetFramesPaddingElements[3];

		const unsigned int blocks = width / tBlockSize;
		const unsigned int remaining = width % tBlockSize;

		for (unsigned int y = 0u; y < height; ++y)
		{
			for (unsigned int n = 0u; n < blocks; ++n)
			{
				source_8x16x4 = vld4q_u8(source);

				vst1q_u8(target0, source_8x16x4.val[0]);
				vst1q_u8(target1, source_8x16x4.val[1]);
				vst1q_u8(target2, source_8x16x4.val[2]);
				vst1q_u8(target3, source_8x16x4.val[3]);

				source += tBlockSize * tChannels;

				target0 += tBlockSize;
				target1 += tBlockSize;
				target2 += tBlockSize;
				target3 += tBlockSize;
			}

			for (unsigned int n = 0u; n < remaining; ++n)
			{
				target0[n] = source[n * tChannels + 0u];
				target1[n] = source[n * tChannels + 1u];
				target2[n] = source[n * tChannels + 2u];
				target3[n] = source[n * tChannels + 3u];
			}

			source += remaining * tChannels + sourceFramePaddingElements;
			target0 += remaining + targetFrame0PaddingElements;
			target1 += remaining + targetFrame1PaddingElements;
			target2 += remaining + targetFrame2PaddingElements;
			target3 += remaining + targetFrame3PaddingElements;
		}
	}
}

#endif // OCEAN_HARDWARE_NEON_VERSION

template <typename TSource, typename TTarget, unsigned int tChannels>
void FrameChannels::separateTo1Channel(const TSource* const sourceFrame, TTarget* const* const targetFrames, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int sourceFramePaddingElements, const unsigned int* targetFramesPaddingElements)
{
	ocean_assert(sourceFrame != nullptr);
	ocean_assert(targetFrames != nullptr);

	ocean_assert(width != 0u && height != 0u);

	ocean_assert(tChannels == CHANNELS_NOT_KNOWN_AT_COMPILE_TIME || tChannels == channels);

	if constexpr (tChannels == CHANNELS_NOT_KNOWN_AT_COMPILE_TIME)
	{
		separateTo1ChannelRuntime<TSource, TTarget>(sourceFrame, targetFrames, width, height, channels, sourceFramePaddingElements, targetFramesPaddingElements);
		return;
	}

#ifdef OCEAN_DEBUG
	for (unsigned int c = 0u; c < tChannels; ++c)
	{
		ocean_assert(targetFrames[c] != nullptr);
	}
#endif

	if (sourceFramePaddingElements == 0u && targetFramesPaddingElements == nullptr)
	{
		for (unsigned int n = 0u; n < width * height; ++n)
		{
			for (unsigned int c = 0u; c < tChannels; ++c)
			{
				targetFrames[c][n] = TTarget(sourceFrame[n * tChannels + c]);
			}
		}
	}
	else if (targetFramesPaddingElements == nullptr)
	{
		ocean_assert(sourceFramePaddingElements != 0u);

		const unsigned int sourceFrameStrideElements = width * tChannels + sourceFramePaddingElements;

		for (unsigned int y = 0u; y < height; ++y)
		{
			const TSource* const sourceRow = sourceFrame + y * sourceFrameStrideElements;

			const unsigned int targetRowOffset = y * width;

			for (unsigned int x = 0u; x < width; ++x)
			{
				for (unsigned int c = 0u; c < tChannels; ++c)
				{
					*(targetFrames[c] + targetRowOffset + x) = TTarget(*(sourceRow + x * tChannels + c));
				}
			}
		}
	}
	else
	{
		const unsigned int sourceFrameStrideElements = width * tChannels + sourceFramePaddingElements;

		Indices32 targetFrameStrideElements(tChannels);

		for (unsigned int c = 0u; c < tChannels; ++c)
		{
			targetFrameStrideElements[c] = width + targetFramesPaddingElements[c];
		}

		for (unsigned int y = 0u; y < height; ++y)
		{
			const TSource* const sourceRow = sourceFrame + y * sourceFrameStrideElements;

			for (unsigned int x = 0u; x < width; ++x)
			{
				for (unsigned int c = 0u; c < tChannels; ++c)
				{
					*(targetFrames[c] + y * targetFrameStrideElements[c] + x) = TTarget(*(sourceRow + x * tChannels + c));
				}
			}
		}
	}
}

template <typename TSource, typename TTarget>
void FrameChannels::separateTo1Channel(const TSource* const sourceFrame, const std::initializer_list<TTarget*>& targetFrames, const unsigned int width, const unsigned int height, const unsigned int sourceFramePaddingElements, const std::initializer_list<const unsigned int>& targetFramesPaddingElements)
{
	ocean_assert(targetFrames.size() >= 1);
	ocean_assert(targetFramesPaddingElements.size() == 0 || targetFrames.size() == targetFramesPaddingElements.size());

	if (targetFrames.size() == 2)
	{
		separateTo1Channel<TSource, TTarget, 2u>(sourceFrame, targetFrames.begin(), width, height, (unsigned int)(targetFrames.size()), sourceFramePaddingElements, targetFramesPaddingElements.size() == 0 ? nullptr : targetFramesPaddingElements.begin());
	}
	else if (targetFrames.size() == 3)
	{
		separateTo1Channel<TSource, TTarget, 3u>(sourceFrame, targetFrames.begin(), width, height, (unsigned int)(targetFrames.size()), sourceFramePaddingElements, targetFramesPaddingElements.size() == 0 ? nullptr : targetFramesPaddingElements.begin());
	}
	else if (targetFrames.size() == 4)
	{
		separateTo1Channel<TSource, TTarget, 4u>(sourceFrame, targetFrames.begin(), width, height, (unsigned int)(targetFrames.size()), sourceFramePaddingElements, targetFramesPaddingElements.size() == 0 ? nullptr : targetFramesPaddingElements.begin());
	}
	else
	{
		separateTo1Channel<TSource, TTarget, CHANNELS_NOT_KNOWN_AT_COMPILE_TIME>(sourceFrame, targetFrames.begin(), width, height, (unsigned int)(targetFrames.size()), sourceFramePaddingElements, targetFramesPaddingElements.size() == 0 ? nullptr : targetFramesPaddingElements.begin());
	}
}

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

template <>
inline void FrameChannels::zipChannels<uint8_t, uint8_t, 2u>(const uint8_t* const* sourceFrames, uint8_t* const targetFrame, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int* sourceFramesPaddingElements, const unsigned int targetFramePaddingElements)
{
	ocean_assert(sourceFrames != nullptr);
	ocean_assert(targetFrame != nullptr);

	ocean_assert(width != 0u && height != 0u);
	ocean_assert(channels == 2u);

	constexpr unsigned int tChannels = 2u;

	bool allSourceFramesContinuous = true;

	if (sourceFramesPaddingElements != nullptr)
	{
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			if (sourceFramesPaddingElements[n] != 0u)
			{
				allSourceFramesContinuous = false;
				break;
			}
		}
	}

	const uint8_t* source0 = sourceFrames[0];
	const uint8_t* source1 = sourceFrames[1];
	uint8_t* target = targetFrame;

	constexpr unsigned int tBlockSize = 16u;

	uint8x16x2_t source_8x16x2;

	if (allSourceFramesContinuous && targetFramePaddingElements == 0u)
	{
		const unsigned int pixels = width * height;
		const unsigned int blocks = pixels / tBlockSize;
		const unsigned int remaining = pixels % tBlockSize;

		for (unsigned int n = 0u; n < blocks; ++n)
		{
			source_8x16x2.val[0] = vld1q_u8(source0);
			source_8x16x2.val[1] = vld1q_u8(source1);

			vst2q_u8(target, source_8x16x2);

			source0 += tBlockSize;
			source1 += tBlockSize;

			target += tBlockSize * tChannels;
		}

		for (unsigned int n = 0u; n < remaining; ++n)
		{
			target[n * tChannels + 0u] = source0[n];
			target[n * tChannels + 1u] = source1[n];
		}
	}
	else
	{
		const unsigned int sourceFrame0PaddingElements = sourceFramesPaddingElements == nullptr ? 0u : sourceFramesPaddingElements[0];
		const unsigned int sourceFrame1PaddingElements = sourceFramesPaddingElements == nullptr ? 0u : sourceFramesPaddingElements[1];

		const unsigned int blocks = width / tBlockSize;
		const unsigned int remaining = width % tBlockSize;

		for (unsigned int y = 0u; y < height; ++y)
		{
			for (unsigned int n = 0u; n < blocks; ++n)
			{
				source_8x16x2.val[0] = vld1q_u8(source0);
				source_8x16x2.val[1] = vld1q_u8(source1);

				vst2q_u8(target, source_8x16x2);

				source0 += tBlockSize;
				source1 += tBlockSize;

				target += tBlockSize * tChannels;
			}

			for (unsigned int n = 0u; n < remaining; ++n)
			{
				target[n * tChannels + 0u] = source0[n];
				target[n * tChannels + 1u] = source1[n];
			}

			source0 += remaining + sourceFrame0PaddingElements;
			source1 += remaining + sourceFrame1PaddingElements;
			target += remaining * tChannels + targetFramePaddingElements;
		}
	}
}

template <>
inline void FrameChannels::zipChannels<uint8_t, uint8_t, 3u>(const uint8_t* const* sourceFrames, uint8_t* const targetFrame, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int* sourceFramesPaddingElements, const unsigned int targetFramePaddingElements)
{
	ocean_assert(sourceFrames != nullptr);
	ocean_assert(targetFrame != nullptr);

	ocean_assert(width != 0u && height != 0u);
	ocean_assert(channels == 3u);

	constexpr unsigned int tChannels = 3u;

	bool allSourceFramesContinuous = true;

	if (sourceFramesPaddingElements != nullptr)
	{
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			if (sourceFramesPaddingElements[n] != 0u)
			{
				allSourceFramesContinuous = false;
				break;
			}
		}
	}

	const uint8_t* source0 = sourceFrames[0];
	const uint8_t* source1 = sourceFrames[1];
	const uint8_t* source2 = sourceFrames[2];
	uint8_t* target = targetFrame;

	constexpr unsigned int tBlockSize = 16u;

	uint8x16x3_t source_8x16x3;

	if (allSourceFramesContinuous && targetFramePaddingElements == 0u)
	{
		const unsigned int pixels = width * height;
		const unsigned int blocks = pixels / tBlockSize;
		const unsigned int remaining = pixels % tBlockSize;

		for (unsigned int n = 0u; n < blocks; ++n)
		{
			source_8x16x3.val[0] = vld1q_u8(source0);
			source_8x16x3.val[1] = vld1q_u8(source1);
			source_8x16x3.val[2] = vld1q_u8(source2);

			vst3q_u8(target, source_8x16x3);

			source0 += tBlockSize;
			source1 += tBlockSize;
			source2 += tBlockSize;

			target += tBlockSize * tChannels;
		}

		for (unsigned int n = 0u; n < remaining; ++n)
		{
			target[n * tChannels + 0u] = source0[n];
			target[n * tChannels + 1u] = source1[n];
			target[n * tChannels + 2u] = source2[n];
		}
	}
	else
	{
		const unsigned int sourceFrame0PaddingElements = sourceFramesPaddingElements == nullptr ? 0u : sourceFramesPaddingElements[0];
		const unsigned int sourceFrame1PaddingElements = sourceFramesPaddingElements == nullptr ? 0u : sourceFramesPaddingElements[1];
		const unsigned int sourceFrame2PaddingElements = sourceFramesPaddingElements == nullptr ? 0u : sourceFramesPaddingElements[2];

		const unsigned int blocks = width / tBlockSize;
		const unsigned int remaining = width % tBlockSize;

		for (unsigned int y = 0u; y < height; ++y)
		{
			for (unsigned int n = 0u; n < blocks; ++n)
			{
				source_8x16x3.val[0] = vld1q_u8(source0);
				source_8x16x3.val[1] = vld1q_u8(source1);
				source_8x16x3.val[2] = vld1q_u8(source2);

				vst3q_u8(target, source_8x16x3);

				source0 += tBlockSize;
				source1 += tBlockSize;
				source2 += tBlockSize;

				target += tBlockSize * tChannels;
			}

			for (unsigned int n = 0u; n < remaining; ++n)
			{
				target[n * tChannels + 0u] = source0[n];
				target[n * tChannels + 1u] = source1[n];
				target[n * tChannels + 2u] = source2[n];
			}

			source0 += remaining + sourceFrame0PaddingElements;
			source1 += remaining + sourceFrame1PaddingElements;
			source2 += remaining + sourceFrame2PaddingElements;
			target += remaining * tChannels + targetFramePaddingElements;
		}
	}
}

template <>
inline void FrameChannels::zipChannels<uint8_t, uint8_t, 4u>(const uint8_t* const* sourceFrames, uint8_t* const targetFrame, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int* sourceFramesPaddingElements, const unsigned int targetFramePaddingElements)
{
	ocean_assert(sourceFrames != nullptr);
	ocean_assert(targetFrame != nullptr);

	ocean_assert(width != 0u && height != 0u);
	ocean_assert(channels == 4u);

	constexpr unsigned int tChannels = 4u;

	bool allSourceFramesContinuous = true;

	if (sourceFramesPaddingElements != nullptr)
	{
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			if (sourceFramesPaddingElements[n] != 0u)
			{
				allSourceFramesContinuous = false;
				break;
			}
		}
	}

	const uint8_t* source0 = sourceFrames[0];
	const uint8_t* source1 = sourceFrames[1];
	const uint8_t* source2 = sourceFrames[2];
	const uint8_t* source3 = sourceFrames[3];
	uint8_t* target = targetFrame;

	constexpr unsigned int tBlockSize = 16u;

	uint8x16x4_t source_8x16x4;

	if (allSourceFramesContinuous && targetFramePaddingElements == 0u)
	{
		const unsigned int pixels = width * height;
		const unsigned int blocks = pixels / tBlockSize;
		const unsigned int remaining = pixels % tBlockSize;

		for (unsigned int n = 0u; n < blocks; ++n)
		{
			source_8x16x4.val[0] = vld1q_u8(source0);
			source_8x16x4.val[1] = vld1q_u8(source1);
			source_8x16x4.val[2] = vld1q_u8(source2);
			source_8x16x4.val[3] = vld1q_u8(source3);

			vst4q_u8(target, source_8x16x4);

			source0 += tBlockSize;
			source1 += tBlockSize;
			source2 += tBlockSize;
			source3 += tBlockSize;

			target += tBlockSize * tChannels;
		}

		for (unsigned int n = 0u; n < remaining; ++n)
		{
			target[n * tChannels + 0u] = source0[n];
			target[n * tChannels + 1u] = source1[n];
			target[n * tChannels + 2u] = source2[n];
			target[n * tChannels + 3u] = source3[n];
		}
	}
	else
	{
		const unsigned int sourceFrame0PaddingElements = sourceFramesPaddingElements == nullptr ? 0u : sourceFramesPaddingElements[0];
		const unsigned int sourceFrame1PaddingElements = sourceFramesPaddingElements == nullptr ? 0u : sourceFramesPaddingElements[1];
		const unsigned int sourceFrame2PaddingElements = sourceFramesPaddingElements == nullptr ? 0u : sourceFramesPaddingElements[2];
		const unsigned int sourceFrame3PaddingElements = sourceFramesPaddingElements == nullptr ? 0u : sourceFramesPaddingElements[3];

		const unsigned int blocks = width / tBlockSize;
		const unsigned int remaining = width % tBlockSize;

		for (unsigned int y = 0u; y < height; ++y)
		{
			for (unsigned int n = 0u; n < blocks; ++n)
			{
				source_8x16x4.val[0] = vld1q_u8(source0);
				source_8x16x4.val[1] = vld1q_u8(source1);
				source_8x16x4.val[2] = vld1q_u8(source2);
				source_8x16x4.val[3] = vld1q_u8(source3);

				vst4q_u8(target, source_8x16x4);

				source0 += tBlockSize;
				source1 += tBlockSize;
				source2 += tBlockSize;
				source3 += tBlockSize;

				target += tBlockSize * tChannels;
			}

			for (unsigned int n = 0u; n < remaining; ++n)
			{
				target[n * tChannels + 0u] = source0[n];
				target[n * tChannels + 1u] = source1[n];
				target[n * tChannels + 2u] = source2[n];
				target[n * tChannels + 3u] = source3[n];
			}

			source0 += remaining + sourceFrame0PaddingElements;
			source1 += remaining + sourceFrame1PaddingElements;
			source2 += remaining + sourceFrame2PaddingElements;
			source3 += remaining + sourceFrame3PaddingElements;
			target += remaining * tChannels + targetFramePaddingElements;
		}
	}
}

template <>
inline void FrameChannels::zipChannels<float, uint8_t, 2u>(const float* const* sourceFrames, uint8_t* const targetFrame, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int* sourceFramesPaddingElements, const unsigned int targetFramePaddingElements)
{
	ocean_assert(sourceFrames != nullptr);
	ocean_assert(targetFrame != nullptr);

	ocean_assert(width != 0u && height != 0u);
	ocean_assert(channels == 2u);

	constexpr unsigned int tChannels = 2u;

	bool allSourceFramesContinuous = true;

	if (sourceFramesPaddingElements != nullptr)
	{
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			if (sourceFramesPaddingElements[n] != 0u)
			{
				allSourceFramesContinuous = false;
				break;
			}
		}
	}

	const float* source0 = sourceFrames[0];
	const float* source1 = sourceFrames[1];
	uint8_t* target = targetFrame;

	constexpr unsigned int tBlockSize = 16u;

	uint8x16x2_t target_8x16x2;

	if (allSourceFramesContinuous && targetFramePaddingElements == 0u)
	{
		const unsigned int pixels = width * height;
		const unsigned int blocks = pixels / tBlockSize;
		const unsigned int remaining = pixels % tBlockSize;

		for (unsigned int n = 0u; n < blocks; ++n)
		{
			target_8x16x2.val[0] = NEON::cast16ElementsNEON(source0);
			target_8x16x2.val[1] = NEON::cast16ElementsNEON(source1);

			vst2q_u8(target, target_8x16x2);

			source0 += tBlockSize;
			source1 += tBlockSize;

			target += tBlockSize * tChannels;
		}

		for (unsigned int n = 0u; n < remaining; ++n)
		{
			ocean_assert(source0[n] >= 0.0f && source0[n] < 256.0f);
			ocean_assert(source1[n] >= 0.0f && source1[n] < 256.0f);

			target[n * tChannels + 0u] = uint8_t(source0[n]);
			target[n * tChannels + 1u] = uint8_t(source1[n]);
		}
	}
	else
	{
		const unsigned int sourceFrame0PaddingElements = sourceFramesPaddingElements == nullptr ? 0u : sourceFramesPaddingElements[0];
		const unsigned int sourceFrame1PaddingElements = sourceFramesPaddingElements == nullptr ? 0u : sourceFramesPaddingElements[1];

		const unsigned int blocks = width / tBlockSize;
		const unsigned int remaining = width % tBlockSize;

		for (unsigned int y = 0u; y < height; ++y)
		{
			for (unsigned int n = 0u; n < blocks; ++n)
			{
				target_8x16x2.val[0] = NEON::cast16ElementsNEON(source0);
				target_8x16x2.val[1] = NEON::cast16ElementsNEON(source1);

				vst2q_u8(target, target_8x16x2);

				source0 += tBlockSize;
				source1 += tBlockSize;

				target += tBlockSize * tChannels;
			}

			for (unsigned int n = 0u; n < remaining; ++n)
			{
				ocean_assert(source0[n] >= 0.0f && source0[n] < 256.0f);
				ocean_assert(source1[n] >= 0.0f && source1[n] < 256.0f);

				target[n * tChannels + 0u] = uint8_t(source0[n]);
				target[n * tChannels + 1u] = uint8_t(source1[n]);
			}

			source0 += remaining + sourceFrame0PaddingElements;
			source1 += remaining + sourceFrame1PaddingElements;
			target += remaining * tChannels + targetFramePaddingElements;
		}
	}
}

template <>
inline void FrameChannels::zipChannels<float, uint8_t, 3u>(const float* const* sourceFrames, uint8_t* const targetFrame, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int* sourceFramesPaddingElements, const unsigned int targetFramePaddingElements)
{
	ocean_assert(sourceFrames != nullptr);
	ocean_assert(targetFrame != nullptr);

	ocean_assert(width != 0u && height != 0u);
	ocean_assert(channels == 3u);

	constexpr unsigned int tChannels = 3u;

	bool allSourceFramesContinuous = true;

	if (sourceFramesPaddingElements != nullptr)
	{
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			if (sourceFramesPaddingElements[n] != 0u)
			{
				allSourceFramesContinuous = false;
				break;
			}
		}
	}

	const float* source0 = sourceFrames[0];
	const float* source1 = sourceFrames[1];
	const float* source2 = sourceFrames[2];
	uint8_t* target = targetFrame;

	constexpr unsigned int tBlockSize = 16u;

	uint8x16x3_t target_8x16x3;

	if (allSourceFramesContinuous && targetFramePaddingElements == 0u)
	{
		const unsigned int pixels = width * height;
		const unsigned int blocks = pixels / tBlockSize;
		const unsigned int remaining = pixels % tBlockSize;

		for (unsigned int n = 0u; n < blocks; ++n)
		{
			target_8x16x3.val[0] = NEON::cast16ElementsNEON(source0);
			target_8x16x3.val[1] = NEON::cast16ElementsNEON(source1);
			target_8x16x3.val[2] = NEON::cast16ElementsNEON(source2);

			vst3q_u8(target, target_8x16x3);

			source0 += tBlockSize;
			source1 += tBlockSize;
			source2 += tBlockSize;

			target += tBlockSize * tChannels;
		}

		for (unsigned int n = 0u; n < remaining; ++n)
		{
			ocean_assert(source0[n] >= 0.0f && source0[n] < 256.0f);
			ocean_assert(source1[n] >= 0.0f && source1[n] < 256.0f);
			ocean_assert(source2[n] >= 0.0f && source2[n] < 256.0f);

			target[n * tChannels + 0u] = uint8_t(source0[n]);
			target[n * tChannels + 1u] = uint8_t(source1[n]);
			target[n * tChannels + 2u] = uint8_t(source2[n]);
		}
	}
	else
	{
		const unsigned int sourceFrame0PaddingElements = sourceFramesPaddingElements == nullptr ? 0u : sourceFramesPaddingElements[0];
		const unsigned int sourceFrame1PaddingElements = sourceFramesPaddingElements == nullptr ? 0u : sourceFramesPaddingElements[1];
		const unsigned int sourceFrame2PaddingElements = sourceFramesPaddingElements == nullptr ? 0u : sourceFramesPaddingElements[2];

		const unsigned int blocks = width / tBlockSize;
		const unsigned int remaining = width % tBlockSize;

		for (unsigned int y = 0u; y < height; ++y)
		{
			for (unsigned int n = 0u; n < blocks; ++n)
			{
				target_8x16x3.val[0] = NEON::cast16ElementsNEON(source0);
				target_8x16x3.val[1] = NEON::cast16ElementsNEON(source1);
				target_8x16x3.val[2] = NEON::cast16ElementsNEON(source2);


				vst3q_u8(target, target_8x16x3);

				source0 += tBlockSize;
				source1 += tBlockSize;
				source2 += tBlockSize;

				target += tBlockSize * tChannels;
			}

			for (unsigned int n = 0u; n < remaining; ++n)
			{
				ocean_assert(source0[n] >= 0.0f && source0[n] < 256.0f);
				ocean_assert(source1[n] >= 0.0f && source1[n] < 256.0f);
				ocean_assert(source2[n] >= 0.0f && source2[n] < 256.0f);

				target[n * tChannels + 0u] = uint8_t(source0[n]);
				target[n * tChannels + 1u] = uint8_t(source1[n]);
				target[n * tChannels + 2u] = uint8_t(source2[n]);
			}

			source0 += remaining + sourceFrame0PaddingElements;
			source1 += remaining + sourceFrame1PaddingElements;
			source2 += remaining + sourceFrame2PaddingElements;
			target += remaining * tChannels + targetFramePaddingElements;
		}
	}
}

template <>
inline void FrameChannels::zipChannels<float, uint8_t, 4u>(const float* const* sourceFrames, uint8_t* const targetFrame, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int* sourceFramesPaddingElements, const unsigned int targetFramePaddingElements)
{
	ocean_assert(sourceFrames != nullptr);
	ocean_assert(targetFrame != nullptr);

	ocean_assert(width != 0u && height != 0u);
	ocean_assert(channels == 4u);

	constexpr unsigned int tChannels = 4u;

	bool allSourceFramesContinuous = true;

	if (sourceFramesPaddingElements != nullptr)
	{
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			if (sourceFramesPaddingElements[n] != 0u)
			{
				allSourceFramesContinuous = false;
				break;
			}
		}
	}

	const float* source0 = sourceFrames[0];
	const float* source1 = sourceFrames[1];
	const float* source2 = sourceFrames[2];
	const float* source3 = sourceFrames[3];
	uint8_t* target = targetFrame;

	constexpr unsigned int tBlockSize = 16u;

	uint8x16x4_t target_8x16x4;

	if (allSourceFramesContinuous && targetFramePaddingElements == 0u)
	{
		const unsigned int pixels = width * height;
		const unsigned int blocks = pixels / tBlockSize;
		const unsigned int remaining = pixels % tBlockSize;

		for (unsigned int n = 0u; n < blocks; ++n)
		{
			target_8x16x4.val[0] = NEON::cast16ElementsNEON(source0);
			target_8x16x4.val[1] = NEON::cast16ElementsNEON(source1);
			target_8x16x4.val[2] = NEON::cast16ElementsNEON(source2);
			target_8x16x4.val[3] = NEON::cast16ElementsNEON(source3);

			vst4q_u8(target, target_8x16x4);

			source0 += tBlockSize;
			source1 += tBlockSize;
			source2 += tBlockSize;
			source3 += tBlockSize;

			target += tBlockSize * tChannels;
		}

		for (unsigned int n = 0u; n < remaining; ++n)
		{
			ocean_assert(source0[n] >= 0.0f && source0[n] < 256.0f);
			ocean_assert(source1[n] >= 0.0f && source1[n] < 256.0f);
			ocean_assert(source2[n] >= 0.0f && source2[n] < 256.0f);
			ocean_assert(source3[n] >= 0.0f && source3[n] < 256.0f);

			target[n * tChannels + 0u] = uint8_t(source0[n]);
			target[n * tChannels + 1u] = uint8_t(source1[n]);
			target[n * tChannels + 2u] = uint8_t(source2[n]);
			target[n * tChannels + 3u] = uint8_t(source3[n]);
		}
	}
	else
	{
		const unsigned int sourceFrame0PaddingElements = sourceFramesPaddingElements == nullptr ? 0u : sourceFramesPaddingElements[0];
		const unsigned int sourceFrame1PaddingElements = sourceFramesPaddingElements == nullptr ? 0u : sourceFramesPaddingElements[1];
		const unsigned int sourceFrame2PaddingElements = sourceFramesPaddingElements == nullptr ? 0u : sourceFramesPaddingElements[2];
		const unsigned int sourceFrame3PaddingElements = sourceFramesPaddingElements == nullptr ? 0u : sourceFramesPaddingElements[3];

		const unsigned int blocks = width / tBlockSize;
		const unsigned int remaining = width % tBlockSize;

		for (unsigned int y = 0u; y < height; ++y)
		{
			for (unsigned int n = 0u; n < blocks; ++n)
			{
				target_8x16x4.val[0] = NEON::cast16ElementsNEON(source0);
				target_8x16x4.val[1] = NEON::cast16ElementsNEON(source1);
				target_8x16x4.val[2] = NEON::cast16ElementsNEON(source2);
				target_8x16x4.val[3] = NEON::cast16ElementsNEON(source3);

				vst4q_u8(target, target_8x16x4);

				source0 += tBlockSize;
				source1 += tBlockSize;
				source2 += tBlockSize;
				source3 += tBlockSize;

				target += tBlockSize * tChannels;
			}

			for (unsigned int n = 0u; n < remaining; ++n)
			{
				ocean_assert(source0[n] >= 0.0f && source0[n] < 256.0f);
				ocean_assert(source1[n] >= 0.0f && source1[n] < 256.0f);
				ocean_assert(source2[n] >= 0.0f && source2[n] < 256.0f);
				ocean_assert(source3[n] >= 0.0f && source3[n] < 256.0f);

				target[n * tChannels + 0u] = uint8_t(source0[n]);
				target[n * tChannels + 1u] = uint8_t(source1[n]);
				target[n * tChannels + 2u] = uint8_t(source2[n]);
				target[n * tChannels + 3u] = uint8_t(source3[n]);
			}

			source0 += remaining + sourceFrame0PaddingElements;
			source1 += remaining + sourceFrame1PaddingElements;
			source2 += remaining + sourceFrame2PaddingElements;
			source3 += remaining + sourceFrame3PaddingElements;
			target += remaining * tChannels + targetFramePaddingElements;
		}
	}
}

#endif // defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

template <typename TSource, typename TTarget, unsigned int tChannels>
void FrameChannels::zipChannels(const TSource* const* sourceFrames, TTarget* const targetFrame, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int* sourceFramesPaddingElements, const unsigned int targetFramePaddingElements)
{
	ocean_assert(sourceFrames != nullptr);
	ocean_assert(targetFrame != nullptr);

	ocean_assert(width != 0u && height != 0u);

	ocean_assert(tChannels == CHANNELS_NOT_KNOWN_AT_COMPILE_TIME || tChannels == channels);

	if constexpr (tChannels == CHANNELS_NOT_KNOWN_AT_COMPILE_TIME)
	{
		zipChannelsRuntime<TSource, TTarget>(sourceFrames, targetFrame, width, height, channels, sourceFramesPaddingElements, targetFramePaddingElements);
		return;
	}

	bool allSourceFramesContinuous = true;

	if (sourceFramesPaddingElements != nullptr)
	{
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			if (sourceFramesPaddingElements[n] != 0u)
			{
				allSourceFramesContinuous = false;
				break;
			}
		}
	}

	if (allSourceFramesContinuous && targetFramePaddingElements == 0u)
	{
		for (unsigned int n = 0u; n < width * height; ++n)
		{
			for (unsigned int c = 0u; c < tChannels; ++c)
			{
				targetFrame[n * tChannels + c] = TTarget(sourceFrames[c][n]);
			}
		}
	}
	else
	{
		const unsigned int targetFrameStrideElements = width * tChannels + targetFramePaddingElements;

		Indices32 sourceFrameStrideElements(tChannels);

		for (unsigned int c = 0u; c < tChannels; ++c)
		{
			if (sourceFramesPaddingElements == nullptr)
			{
				sourceFrameStrideElements[c] = width;
			}
			else
			{
				sourceFrameStrideElements[c] = width + sourceFramesPaddingElements[c];
			}
		}

		for (unsigned int y = 0u; y < height; ++y)
		{
			TTarget* const targetRow = targetFrame + y * targetFrameStrideElements;

			for (unsigned int x = 0u; x < width; ++x)
			{
				for (unsigned int c = 0u; c < tChannels; ++c)
				{
					*(targetRow + x * tChannels + c) = TTarget(*(sourceFrames[c] + y * sourceFrameStrideElements[c] + x));
				}
			}
		}
	}
}

template <typename TSource, typename TTarget>
void FrameChannels::zipChannels(const std::initializer_list<const TSource*>& sourceFrames, TTarget* const targetFrame, const unsigned int width, const unsigned int height, const std::initializer_list<unsigned int>& sourceFramePaddingElements, const unsigned int targetFramePaddingElements)
{
	ocean_assert(sourceFrames.size() >= 1);
	ocean_assert(sourceFramePaddingElements.size() == 0 || sourceFrames.size() == sourceFramePaddingElements.size());

	if (sourceFrames.size() == 2)
	{
		zipChannels<TSource, TTarget, 2u>(sourceFrames.begin(), targetFrame, width, height, (unsigned int)(sourceFrames.size()), sourceFramePaddingElements.size() == 0 ? nullptr : sourceFramePaddingElements.begin(), targetFramePaddingElements);
	}
	else if (sourceFrames.size() == 3)
	{
		zipChannels<TSource, TTarget, 3u>(sourceFrames.begin(), targetFrame, width, height, (unsigned int)(sourceFrames.size()), sourceFramePaddingElements.size() == 0 ? nullptr : sourceFramePaddingElements.begin(), targetFramePaddingElements);
	}
	else if (sourceFrames.size() == 4)
	{
		zipChannels<TSource, TTarget, 4u>(sourceFrames.begin(), targetFrame, width, height, (unsigned int)(sourceFrames.size()), sourceFramePaddingElements.size() == 0 ? nullptr : sourceFramePaddingElements.begin(), targetFramePaddingElements);
	}
	else
	{
		zipChannels<TSource, TTarget, CHANNELS_NOT_KNOWN_AT_COMPILE_TIME>(sourceFrames.begin(), targetFrame, width, height, (unsigned int)(sourceFrames.size()), sourceFramePaddingElements.size() == 0 ? nullptr : sourceFramePaddingElements.begin(), targetFramePaddingElements);
	}
}

template <typename T, unsigned int tSourceChannels>
inline void FrameChannels::addFirstChannel(const T* source, const T* sourceNewChannel, T* target, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const unsigned int sourcePaddingElements, const unsigned int sourceNewChannelPaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(tSourceChannels != 0u, "Invalid channel number!");

	ocean_assert(source != nullptr && sourceNewChannel != nullptr && target != nullptr);
	ocean_assert(source != target);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int options[3] = {sourcePaddingElements, sourceNewChannelPaddingElements, targetPaddingElements};

	const void* sources[2] = {source, sourceNewChannel};

	FrameConverter::convertArbitraryPixelFormat(sources, (void**)&target, width, height, conversionFlag, 1u, FrameChannels::addChannelRow<T, tSourceChannels, true>, options, worker);
}

template <typename T, unsigned int tSourceChannels>
inline void FrameChannels::addFirstChannelValue(const T* source, const T newChannelValue, T* target, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(tSourceChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int targetChannels = tSourceChannels + 1u;

	const unsigned int sourceStrideElements = width * tSourceChannels + sourcePaddingElements;
	const unsigned int targetStrideElements = width * targetChannels + targetPaddingElements;

	const void* channelValueParameter = (const void*)(&newChannelValue);

	const bool areContinuous = sourcePaddingElements == 0u && targetPaddingElements == 0u;

	FrameConverter::convertGenericPixelFormat<T>(source, target, width, height, sourceStrideElements, targetStrideElements, conversionFlag, FrameChannels::addChannelValueRow<T, tSourceChannels, true>, FrameChannels::reverseRowPixelOrderInPlace<T, targetChannels>, areContinuous, channelValueParameter, worker);
}

template <typename T, unsigned int tSourceChannels>
inline void FrameChannels::addLastChannel(const T* source, const T* sourceNewChannel, T* target, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const unsigned int sourcePaddingElements, const unsigned int sourceNewChannelPaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(tSourceChannels != 0u, "Invalid channel number!");

	ocean_assert(source != nullptr && sourceNewChannel != nullptr && target != nullptr);
	ocean_assert(source != target);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int options[3] = {sourcePaddingElements, sourceNewChannelPaddingElements, targetPaddingElements};

	const void* sources[2] = {source, sourceNewChannel};

	FrameConverter::convertArbitraryPixelFormat(sources, (void**)&target, width, height, conversionFlag, 1u, FrameChannels::addChannelRow<T, tSourceChannels, false>, options, worker);
}

template <typename T, unsigned int tSourceChannels>
inline void FrameChannels::addLastChannelValue(const T* source, const T newChannelValue, T* target, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(tSourceChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int targetChannels = tSourceChannels + 1u;

	const unsigned int sourceStrideElements = width * tSourceChannels + sourcePaddingElements;
	const unsigned int targetStrideElements = width * targetChannels + targetPaddingElements;

	const void* channelValueParameter = (const void*)(&newChannelValue);

	const bool areContinuous = sourcePaddingElements == 0u && targetPaddingElements == 0u;

	FrameConverter::convertGenericPixelFormat<T>(source, target, width, height, sourceStrideElements, targetStrideElements, conversionFlag, FrameChannels::addChannelValueRow<T, tSourceChannels, false>, FrameChannels::reverseRowPixelOrderInPlace<T, targetChannels>, areContinuous, channelValueParameter, worker);
}

template <typename T, unsigned int tSourceChannels>
inline void FrameChannels::removeFirstChannel(const T* source, T* target, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(tSourceChannels >= 2u && tSourceChannels <= 8u, "Invalid channel number!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int shufflePatternMax = 0x07654321u;
	const unsigned int mask = 0xFFFFFFFFu >> ((8u - tSourceChannels + 1u) * 4u); // e.g., 0xFF for tChannels == 3u, 0xFFF for tChannels == 4u

	const unsigned int shufflePattern = shufflePatternMax & mask;

	FrameChannels::shuffleChannels<T, tSourceChannels, tSourceChannels - 1u, shufflePattern>(source, target, width, height, conversionFlag, sourcePaddingElements, targetPaddingElements, worker);
}

template <typename T, unsigned int tSourceChannels>
inline void FrameChannels::removeLastChannel(const T* source, T* target, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(tSourceChannels >= 2u && tSourceChannels <= 8u, "Invalid channel number!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int shufflePatternMax = 0x76543210u;
	const unsigned int mask = 0xFFFFFFFFu >> ((8u - tSourceChannels + 1u) * 4u); // e.g., 0xFF for tChannels == 3u, 0xFFF for tChannels == 4u

	const unsigned int shufflePattern = shufflePatternMax & mask;

	FrameChannels::shuffleChannels<T, tSourceChannels, tSourceChannels - 1u, shufflePattern>(source, target, width, height, conversionFlag, sourcePaddingElements, targetPaddingElements, worker);
}

template <typename T, unsigned int tSourceChannels, unsigned int tTargetChannels, unsigned int tSourceChannelIndex, unsigned int tTargetChannelIndex>
inline void FrameChannels::copyChannel(const T* source, T* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(tSourceChannels >= 1u, "Invalid number of channels!");
	static_assert(tTargetChannels >= 1u, "Invalid number of channels!");

	static_assert(tSourceChannelIndex < tSourceChannels, "Invalid channel index!");
	static_assert(tTargetChannelIndex < tTargetChannels, "Invalid channel index!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int sourceStrideElements = width * tSourceChannels + sourcePaddingElements;
	const unsigned int targetStrideElements = width * tTargetChannels + targetPaddingElements;

	constexpr RowReversePixelOrderInPlaceFunction<T> reversePixelOrderRowInPlaceFunction = nullptr;

	const bool areContinuous = sourcePaddingElements == 0u && targetPaddingElements == 0u;

	FrameConverter::convertGenericPixelFormat<T>(source, target, width, height, sourceStrideElements, targetStrideElements, CONVERT_NORMAL, FrameChannels::copyChannelRow<T, tSourceChannels, tTargetChannels, tSourceChannelIndex, tTargetChannelIndex>, reversePixelOrderRowInPlaceFunction, areContinuous, nullptr, worker);
}

template <typename T, unsigned int tChannel, unsigned int tChannels>
inline void FrameChannels::setChannel(T* frame, const unsigned int width, const unsigned int height, const T value, const unsigned int framePaddingElements, Worker* worker)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tChannel < tChannels, "Invalid channel index!");

	ocean_assert(frame != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&setChannelSubset<T, tChannel, tChannels>, frame, width, value, framePaddingElements, 0u, 0u), 0u, height);
	}
	else
	{
		setChannelSubset<T, tChannel, tChannels>(frame, width, value, framePaddingElements, 0u, height);
	}
}

template <typename T, unsigned int tChannels>
inline void FrameChannels::reverseChannelOrder(const T* source, T* target, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int sourceStrideElements = width * tChannels + sourcePaddingElements;
	const unsigned int targetStrideElements = width * tChannels + targetPaddingElements;

	constexpr bool areContinuous = false; // even if both images are continuous, we must reverse each line by another

	FrameConverter::convertGenericPixelFormat<T>(source, target, width, height, sourceStrideElements, targetStrideElements, conversionFlag, FrameChannels::reverseRowChannelOrder<T, tChannels>, FrameChannels::reverseRowPixelOrderInPlace<T, tChannels>, areContinuous, nullptr, worker);
}

template <typename T, unsigned int tChannels>
void FrameChannels::reverseRowPixelOrder(const T* source, T* target, const size_t size)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(size >= 1);

#ifdef OCEAN_DEBUG
	const T* const debugSourceStart = source;
	const T* const debugSourceEnd = debugSourceStart + size * tChannels;

	const T* const debugTargetStart = target;
	const T* const debugTargetEnd = debugTargetStart + size * tChannels;
#endif

	// moving target to the end of the memory block
	target += size * tChannels;

	const T* const sourceEnd = source + size * tChannels;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	if constexpr (std::is_same<typename TypeMapper<T>::Type, uint8_t>::value)
	{
		const size_t blocks16 = size / size_t(16);

		switch (tChannels)
		{
			case 1u:
			{
				for (size_t n = 0; n < blocks16; ++n)
				{
					target -= 16u * tChannels;

					ocean_assert(source >= debugSourceStart && source + 16u * tChannels <= debugSourceEnd);
					ocean_assert(target >= debugTargetStart && target + 16u * tChannels <= debugTargetEnd);

					const uint8x16_t source_u_8x16 = vld1q_u8((const uint8_t*)(source));
					uint8x16_t revSource_u_8x16 = vrev64q_u8(source_u_8x16);
					revSource_u_8x16 = vcombine_u8(vget_high_u8(revSource_u_8x16), vget_low_u8(revSource_u_8x16));

					vst1q_u8((uint8_t*)(target), revSource_u_8x16);

					source += 16u * tChannels;
				}

				break;
			}

			case 2u:
			{
				for (size_t n = 0; n < blocks16; ++n)
				{
					target -= 16u * tChannels;

					ocean_assert(source >= debugSourceStart && source + 16u * tChannels <= debugSourceEnd);
					ocean_assert(target >= debugTargetStart && target + 16u * tChannels <= debugTargetEnd);

					const uint8x16_t sourceA_u_8x16 = vld1q_u8((const uint8_t*)(source) + 0);
					const uint8x16_t sourceB_u_8x16 = vld1q_u8((const uint8_t*)(source) + 16);

					const uint8x16_t revSourceA_u_8x16 = vreinterpretq_u8_u16(vrev64q_u16(vreinterpretq_u16_u8(sourceA_u_8x16)));
					const uint8x16_t revSourceB_u_8x16 = vreinterpretq_u8_u16(vrev64q_u16(vreinterpretq_u16_u8(sourceB_u_8x16)));

					const uint8x16_t targetA_u_8x16 = vcombine_u8(vget_high_u8(revSourceA_u_8x16), vget_low_u8(revSourceA_u_8x16));
					const uint8x16_t targetB_u_8x16 = vcombine_u8(vget_high_u8(revSourceB_u_8x16), vget_low_u8(revSourceB_u_8x16));

					vst1q_u8((uint8_t*)(target) + 0, targetB_u_8x16);
					vst1q_u8((uint8_t*)(target) + 16, targetA_u_8x16);

					source += 16u * tChannels;
				}

				break;
			}

			case 3u:
			{
				for (size_t n = 0; n < blocks16; ++n)
				{
					target -= 16u * tChannels;

					ocean_assert(source >= debugSourceStart && source + 16u * tChannels <= debugSourceEnd);
					ocean_assert(target >= debugTargetStart && target + 16u * tChannels <= debugTargetEnd);

					const uint8x16x3_t source_u_8x16x3 = vld3q_u8((const uint8_t*)(source));

					uint8x16x3_t revSource_u_8x16x3;
					revSource_u_8x16x3.val[0] = vcombine_u8(vrev64_u8(vget_high_u8(source_u_8x16x3.val[0])), vrev64_u8(vget_low_u8(source_u_8x16x3.val[0])));
					revSource_u_8x16x3.val[1] = vcombine_u8(vrev64_u8(vget_high_u8(source_u_8x16x3.val[1])), vrev64_u8(vget_low_u8(source_u_8x16x3.val[1])));
					revSource_u_8x16x3.val[2] = vcombine_u8(vrev64_u8(vget_high_u8(source_u_8x16x3.val[2])), vrev64_u8(vget_low_u8(source_u_8x16x3.val[2])));

					vst3q_u8((uint8_t*)(target), revSource_u_8x16x3);

					source += 16u * tChannels;
				}

				break;
			}

			case 4u:
			{
				for (size_t n = 0; n < blocks16; ++n)
				{
					target -= 16u * tChannels;

					ocean_assert(source >= debugSourceStart && source + 16u * tChannels <= debugSourceEnd);
					ocean_assert(target >= debugTargetStart && target + 16u * tChannels <= debugTargetEnd);

					const uint8x16_t sourceA_u_8x16 = vld1q_u8((const uint8_t*)(source) + 0);
					const uint8x16_t sourceB_u_8x16 = vld1q_u8((const uint8_t*)(source) + 16);
					const uint8x16_t sourceC_u_8x16 = vld1q_u8((const uint8_t*)(source) + 32);
					const uint8x16_t sourceD_u_8x16 = vld1q_u8((const uint8_t*)(source) + 48);

					const uint8x16_t revSourceA_u_8x16 = vreinterpretq_u8_u32(vrev64q_u32(vreinterpretq_u32_u8(sourceA_u_8x16)));
					const uint8x16_t revSourceB_u_8x16 = vreinterpretq_u8_u32(vrev64q_u32(vreinterpretq_u32_u8(sourceB_u_8x16)));
					const uint8x16_t revSourceC_u_8x16 = vreinterpretq_u8_u32(vrev64q_u32(vreinterpretq_u32_u8(sourceC_u_8x16)));
					const uint8x16_t revSourceD_u_8x16 = vreinterpretq_u8_u32(vrev64q_u32(vreinterpretq_u32_u8(sourceD_u_8x16)));

					const uint8x16_t targetA_u_8x16 = vcombine_u8(vget_high_u8(revSourceA_u_8x16), vget_low_u8(revSourceA_u_8x16));
					const uint8x16_t targetB_u_8x16 = vcombine_u8(vget_high_u8(revSourceB_u_8x16), vget_low_u8(revSourceB_u_8x16));
					const uint8x16_t targetC_u_8x16 = vcombine_u8(vget_high_u8(revSourceC_u_8x16), vget_low_u8(revSourceC_u_8x16));
					const uint8x16_t targetD_u_8x16 = vcombine_u8(vget_high_u8(revSourceD_u_8x16), vget_low_u8(revSourceD_u_8x16));

					vst1q_u8((uint8_t*)(target) + 0, targetD_u_8x16);
					vst1q_u8((uint8_t*)(target) + 16, targetC_u_8x16);
					vst1q_u8((uint8_t*)(target) + 32, targetB_u_8x16);
					vst1q_u8((uint8_t*)(target) + 48, targetA_u_8x16);

					source += 16u * tChannels;
				}

				break;
			}

			default:
				break;
		}
	}

#endif // OCEAN_HARDWARE_NEON_VERSION

	while (source != sourceEnd)
	{
		ocean_assert(source < sourceEnd);

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			ocean_assert(source + tChannels - n - 1u >= debugSourceStart);
			ocean_assert(source + tChannels - n - 1u < debugSourceEnd);

			ocean_assert(target > debugTargetStart && target <= debugTargetEnd);

			*--target = source[tChannels - n - 1u];
		}

		source += tChannels;
	}
}

template <typename T, unsigned int tChannels>
void FrameChannels::reverseRowPixelOrderInPlace(T* data, const size_t size)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(data != nullptr);
	ocean_assert(size >= 1);

	typedef typename DataType<T, tChannels>::Type PixelType;

	size_t n = 0;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	if (std::is_same<typename TypeMapper<T>::Type, uint8_t>::value)
	{
		const size_t blocks32 = size / size_t(32);

		uint8_t* left = (uint8_t*)(data);
		uint8_t* right = (uint8_t*)(data) + (size - 16u) * tChannels;

		switch (tChannels)
		{
			case 1u:
			{
				for (size_t nBlock = 0; nBlock < blocks32; ++nBlock)
				{
					const uint8x16_t left_u_8x16 = vld1q_u8(left);
					const uint8x16_t right_u_8x16 = vld1q_u8(right);

					uint8x16_t revLeft_u_8x16 = vrev64q_u8(left_u_8x16);
					revLeft_u_8x16 = vcombine_u8(vget_high_u8(revLeft_u_8x16), vget_low_u8(revLeft_u_8x16));

					uint8x16_t revRight_u_8x16 = vrev64q_u8(right_u_8x16);
					revRight_u_8x16 = vcombine_u8(vget_high_u8(revRight_u_8x16), vget_low_u8(revRight_u_8x16));

					vst1q_u8(left, revRight_u_8x16);
					vst1q_u8(right, revLeft_u_8x16);

					left += 16u * tChannels;
					right -= 16u * tChannels;
				}

				n += blocks32 * 16u;

				break;
			}

			case 2u:
			{
				for (size_t nBlock = 0; nBlock < blocks32; ++nBlock)
				{
					const uint8x16x2_t left_u_8x16x2 = vld2q_u8(left);
					const uint8x16x2_t right_u_8x16x2 = vld2q_u8(right);

					uint8x16x2_t revLeft_u_8x16x2;
					revLeft_u_8x16x2.val[0] = vrev64q_u8(left_u_8x16x2.val[0]);
					revLeft_u_8x16x2.val[1] = vrev64q_u8(left_u_8x16x2.val[1]);
					revLeft_u_8x16x2.val[0] = vcombine_u8(vget_high_u8(revLeft_u_8x16x2.val[0]), vget_low_u8(revLeft_u_8x16x2.val[0]));
					revLeft_u_8x16x2.val[1] = vcombine_u8(vget_high_u8(revLeft_u_8x16x2.val[1]), vget_low_u8(revLeft_u_8x16x2.val[1]));

					uint8x16x2_t revRight_u_8x16x2;
					revRight_u_8x16x2.val[0] = vrev64q_u8(right_u_8x16x2.val[0]);
					revRight_u_8x16x2.val[1] = vrev64q_u8(right_u_8x16x2.val[1]);
					revRight_u_8x16x2.val[0] = vcombine_u8(vget_high_u8(revRight_u_8x16x2.val[0]), vget_low_u8(revRight_u_8x16x2.val[0]));
					revRight_u_8x16x2.val[1] = vcombine_u8(vget_high_u8(revRight_u_8x16x2.val[1]), vget_low_u8(revRight_u_8x16x2.val[1]));

					vst2q_u8(left, revRight_u_8x16x2);
					vst2q_u8(right, revLeft_u_8x16x2);

					left += 16u * tChannels;
					right -= 16u * tChannels;
				}

				n += blocks32 * 16u;

				break;
			}

			case 3u:
			{
				for (size_t nBlock = 0; nBlock < blocks32; ++nBlock)
				{
					const uint8x16x3_t left_u_8x16x3 = vld3q_u8(left);
					const uint8x16x3_t right_u_8x16x3 = vld3q_u8(right);

					uint8x16x3_t revLeft_u_8x16x3;
					revLeft_u_8x16x3.val[0] = vrev64q_u8(left_u_8x16x3.val[0]);
					revLeft_u_8x16x3.val[1] = vrev64q_u8(left_u_8x16x3.val[1]);
					revLeft_u_8x16x3.val[2] = vrev64q_u8(left_u_8x16x3.val[2]);
					revLeft_u_8x16x3.val[0] = vcombine_u8(vget_high_u8(revLeft_u_8x16x3.val[0]), vget_low_u8(revLeft_u_8x16x3.val[0]));
					revLeft_u_8x16x3.val[1] = vcombine_u8(vget_high_u8(revLeft_u_8x16x3.val[1]), vget_low_u8(revLeft_u_8x16x3.val[1]));
					revLeft_u_8x16x3.val[2] = vcombine_u8(vget_high_u8(revLeft_u_8x16x3.val[2]), vget_low_u8(revLeft_u_8x16x3.val[2]));

					uint8x16x3_t revRight_u_8x16x3;
					revRight_u_8x16x3.val[0] = vrev64q_u8(right_u_8x16x3.val[0]);
					revRight_u_8x16x3.val[1] = vrev64q_u8(right_u_8x16x3.val[1]);
					revRight_u_8x16x3.val[2] = vrev64q_u8(right_u_8x16x3.val[2]);
					revRight_u_8x16x3.val[0] = vcombine_u8(vget_high_u8(revRight_u_8x16x3.val[0]), vget_low_u8(revRight_u_8x16x3.val[0]));
					revRight_u_8x16x3.val[1] = vcombine_u8(vget_high_u8(revRight_u_8x16x3.val[1]), vget_low_u8(revRight_u_8x16x3.val[1]));
					revRight_u_8x16x3.val[2] = vcombine_u8(vget_high_u8(revRight_u_8x16x3.val[2]), vget_low_u8(revRight_u_8x16x3.val[2]));

					vst3q_u8(left, revRight_u_8x16x3);
					vst3q_u8(right, revLeft_u_8x16x3);

					left += 16u * tChannels;
					right -= 16u * tChannels;
				}

				n += blocks32 * 16u;

				break;
			}

			case 4u:
			{
				for (size_t nBlock = 0; nBlock < blocks32; ++nBlock)
				{
					const uint8x16x4_t left_u_8x16x4 = vld4q_u8(left);
					const uint8x16x4_t right_u_8x16x4 = vld4q_u8(right);

					uint8x16x4_t revLeft_u_8x16x4;
					revLeft_u_8x16x4.val[0] = vrev64q_u8(left_u_8x16x4.val[0]);
					revLeft_u_8x16x4.val[1] = vrev64q_u8(left_u_8x16x4.val[1]);
					revLeft_u_8x16x4.val[2] = vrev64q_u8(left_u_8x16x4.val[2]);
					revLeft_u_8x16x4.val[3] = vrev64q_u8(left_u_8x16x4.val[3]);
					revLeft_u_8x16x4.val[0] = vcombine_u8(vget_high_u8(revLeft_u_8x16x4.val[0]), vget_low_u8(revLeft_u_8x16x4.val[0]));
					revLeft_u_8x16x4.val[1] = vcombine_u8(vget_high_u8(revLeft_u_8x16x4.val[1]), vget_low_u8(revLeft_u_8x16x4.val[1]));
					revLeft_u_8x16x4.val[2] = vcombine_u8(vget_high_u8(revLeft_u_8x16x4.val[2]), vget_low_u8(revLeft_u_8x16x4.val[2]));
					revLeft_u_8x16x4.val[3] = vcombine_u8(vget_high_u8(revLeft_u_8x16x4.val[3]), vget_low_u8(revLeft_u_8x16x4.val[3]));

					uint8x16x4_t revRight_u_8x16x4;
					revRight_u_8x16x4.val[0] = vrev64q_u8(right_u_8x16x4.val[0]);
					revRight_u_8x16x4.val[1] = vrev64q_u8(right_u_8x16x4.val[1]);
					revRight_u_8x16x4.val[2] = vrev64q_u8(right_u_8x16x4.val[2]);
					revRight_u_8x16x4.val[3] = vrev64q_u8(right_u_8x16x4.val[3]);
					revRight_u_8x16x4.val[0] = vcombine_u8(vget_high_u8(revRight_u_8x16x4.val[0]), vget_low_u8(revRight_u_8x16x4.val[0]));
					revRight_u_8x16x4.val[1] = vcombine_u8(vget_high_u8(revRight_u_8x16x4.val[1]), vget_low_u8(revRight_u_8x16x4.val[1]));
					revRight_u_8x16x4.val[2] = vcombine_u8(vget_high_u8(revRight_u_8x16x4.val[2]), vget_low_u8(revRight_u_8x16x4.val[2]));
					revRight_u_8x16x4.val[3] = vcombine_u8(vget_high_u8(revRight_u_8x16x4.val[3]), vget_low_u8(revRight_u_8x16x4.val[3]));

					vst4q_u8(left, revRight_u_8x16x4);
					vst4q_u8(right, revLeft_u_8x16x4);

					left += 16u * tChannels;
					right -= 16u * tChannels;
				}

				n += blocks32 * 16u;

				break;
			}

			default:
				break;
		}
	}

#endif

	PixelType intermediate;

	PixelType* const pixels = (PixelType*)(data);

	while (n < size / 2)
	{
		intermediate = pixels[n];

		pixels[n] = pixels[size - n - 1];
		pixels[size - n - 1] = intermediate;

		++n;
	}
}

template <typename T, unsigned int tChannels>
void FrameChannels::reverseRowChannelOrder(const T* source, T* target, const size_t size, const void* /*options*/)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(source != target);
	ocean_assert(size >= 1);

#ifdef OCEAN_DEBUG
	const T* const debugSourceStart = source;
	const T* const debugSourceEnd = debugSourceStart + size * tChannels;

	const T* const debugTargetStart = target;
	const T* const debugTargetEnd = debugTargetStart + size * tChannels;
#endif

	if constexpr (tChannels == 1)
	{
		// we actually copy the one channel

		memcpy(target, source, sizeof(T) * size);
		return;
	}

	const T* const sourceEnd = source + size * tChannels;

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

	if ((std::is_same<typename TypeMapper<T>::Type, uint8_t>::value))
	{
		const size_t blocks16 = size / size_t(16);

		switch (tChannels)
		{
			case 1u:
				ocean_assert(false && "This should have been handled above!");
				break;

			case 2u:
			{
				for (size_t n = 0; n < blocks16; ++n)
				{
					SSE::reverseChannelOrder2Channel8Bit32Elements((const uint8_t*)source, (uint8_t*)target);

					source += 16u * tChannels;
					target += 16u * tChannels;
				}

				break;
			}

			case 3u:
			{
				for (size_t n = 0; n < blocks16; ++n)
				{
					SSE::reverseChannelOrder3Channel8Bit48Elements((const uint8_t*)source, (uint8_t*)target);

					source += 16u * tChannels;
					target += 16u * tChannels;
				}

				break;
			}

			case 4u:
			{
				for (size_t n = 0; n < blocks16; ++n)
				{
					SSE::reverseChannelOrder4Channel8Bit64Elements((const uint8_t*)source, (uint8_t*)target);

					source += 16u * tChannels;
					target += 16u * tChannels;
				}

				break;
			}

			default:
				break;
		}
	}

#elif defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	if ((std::is_same<typename TypeMapper<T>::Type, uint8_t>::value))
	{
		const size_t blocks16 = size / size_t(16);

		switch (tChannels)
		{
			case 1u:
				ocean_assert(false && "This should have been handled above!");
				break;

			case 2u:
			{
				for (size_t n = 0; n < blocks16; ++n)
				{
					ocean_assert(source >= debugSourceStart && source + 16u * tChannels <= debugSourceEnd);
					ocean_assert(target >= debugTargetStart && target + 16u * tChannels <= debugTargetEnd);

					const uint8x16_t sourceA_u_8x16 = vld1q_u8((const uint8_t*)source + 0);
					const uint8x16_t sourceB_u_8x16 = vld1q_u8((const uint8_t*)source + 16);

					const uint8x16_t revSourceA_u_8x16 = vrev16q_u8(sourceA_u_8x16);
					const uint8x16_t revSourceB_u_8x16 = vrev16q_u8(sourceB_u_8x16);

					vst1q_u8((uint8_t*)target + 0, revSourceA_u_8x16);
					vst1q_u8((uint8_t*)target + 16, revSourceB_u_8x16);

					source += 16u * tChannels;
					target += 16u * tChannels;
				}

				break;
			}

			case 3u:
			{
				for (size_t n = 0; n < blocks16; ++n)
				{
					ocean_assert(source >= debugSourceStart && source + 16u * tChannels <= debugSourceEnd);
					ocean_assert(target >= debugTargetStart && target + 16u * tChannels <= debugTargetEnd);

					const uint8x16x3_t source_u_8x16x3 = vld3q_u8((const uint8_t*)source);

					uint8x16x3_t revSource_u_8x16x3;
					revSource_u_8x16x3.val[0] = source_u_8x16x3.val[2];
					revSource_u_8x16x3.val[1] = source_u_8x16x3.val[1];
					revSource_u_8x16x3.val[2] = source_u_8x16x3.val[0];

					vst3q_u8((uint8_t*)target, revSource_u_8x16x3);

					source += 16u * tChannels;
					target += 16u * tChannels;
				}

				break;
			}

			case 4u:
			{
				for (size_t n = 0; n < blocks16; ++n)
				{
					ocean_assert(source >= debugSourceStart && source + 16u * tChannels <= debugSourceEnd);
					ocean_assert(target >= debugTargetStart && target + 16u * tChannels <= debugTargetEnd);

					const uint8x16_t sourceA_u_8x16 = vld1q_u8((const uint8_t*)source + 0);
					const uint8x16_t sourceB_u_8x16 = vld1q_u8((const uint8_t*)source + 16);
					const uint8x16_t sourceC_u_8x16 = vld1q_u8((const uint8_t*)source + 32);
					const uint8x16_t sourceD_u_8x16 = vld1q_u8((const uint8_t*)source + 48);

					const uint8x16_t revSourceA_u_8x16 = vrev32q_u8(sourceA_u_8x16);
					const uint8x16_t revSourceB_u_8x16 = vrev32q_u8(sourceB_u_8x16);
					const uint8x16_t revSourceC_u_8x16 = vrev32q_u8(sourceC_u_8x16);
					const uint8x16_t revSourceD_u_8x16 = vrev32q_u8(sourceD_u_8x16);

					vst1q_u8((uint8_t*)target + 0, revSourceA_u_8x16);
					vst1q_u8((uint8_t*)target + 16, revSourceB_u_8x16);
					vst1q_u8((uint8_t*)target + 32, revSourceC_u_8x16);
					vst1q_u8((uint8_t*)target + 48, revSourceD_u_8x16);

					source += 16u * tChannels;
					target += 16u * tChannels;
				}

				break;
			}

			default:
				break;
		}
	}

#endif // OCEAN_HARDWARE_NEON_VERSION

	while (source != sourceEnd)
	{
		ocean_assert(source < sourceEnd);

		ocean_assert(source >= debugSourceStart && source + tChannels <= debugSourceEnd);
		ocean_assert(target >= debugTargetStart && target + tChannels <= debugTargetEnd);

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			target[n] = source[tChannels - n - 1u];
		}

		source += tChannels;
		target += tChannels;
	}
}

template <typename T, unsigned int tSourceChannels, unsigned int tTargetChannels, unsigned int tShufflePattern>
inline void FrameChannels::shuffleRowChannels(const T* source, T* target, const size_t size, const void* /*unusedOptions*/)
{
	static_assert(tSourceChannels >= 1u && tSourceChannels <= 8u, "Invalid channel number!");
	static_assert(tTargetChannels >= 1u && tTargetChannels <= 8u, "Invalid channel number!");

	static_assert(tSourceChannels != 1u || tTargetChannels != 1u, "Invalid channel number!");

	static_assert(((tShufflePattern & 0x0000000Fu) >> 0u) < tSourceChannels, "Invalid shuffle pattern!");
	static_assert(((tShufflePattern & 0x000000F0u) >> 4u) < tSourceChannels, "Invalid shuffle pattern!");
	static_assert(((tShufflePattern & 0x00000F00u) >> 8u) < tSourceChannels, "Invalid shuffle pattern!");
	static_assert(((tShufflePattern & 0x0000F000u) >> 12u) < tSourceChannels, "Invalid shuffle pattern!");
	static_assert(((tShufflePattern & 0x000F0000u) >> 16u) < tSourceChannels, "Invalid shuffle pattern!");
	static_assert(((tShufflePattern & 0x00F00000u) >> 20u) < tSourceChannels, "Invalid shuffle pattern!");
	static_assert(((tShufflePattern & 0x0F000000u) >> 24u) < tSourceChannels, "Invalid shuffle pattern!");
	static_assert(((tShufflePattern & 0xF0000000u) >> 28u) < tSourceChannels, "Invalid shuffle pattern!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(size != 0);

	const T* const sourceEnd = source + size * tSourceChannels;

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

	if ((std::is_same<typename TypeMapper<T>::Type, uint8_t>::value))
	{
		const size_t blocks16 = size / size_t(16);

		switch (tSourceChannels | ((tTargetChannels) << 4u))
		{
			// 4 -> 4
			case (4u | (4u << 4u)):
			{
				// the following shuffle patterns are known during compile time

				constexpr unsigned int offset1 = 0x04040404u;
				constexpr unsigned int offset2 = 0x08080808u;
				constexpr unsigned int offset3 = 0x0C0C0C0Cu;

				// converting shufflePattern16 to shufflePattern16
				const unsigned int shufflePattern0 =  ((tShufflePattern & 0xF000u) << 12u) | ((tShufflePattern & 0x0F00u) << 8u) | ((tShufflePattern & 0x00F0u) << 4u) | ((tShufflePattern & 0x000Fu) << 0u);

				const unsigned int shufflePattern1 = shufflePattern0 + offset1;
				const unsigned int shufflePattern2 = shufflePattern0 + offset2;
				const unsigned int shufflePattern3 = shufflePattern0 + offset3;

				const __m128i shufflePattern128 = SSE::set128i((((unsigned long long)shufflePattern3) << 32ull) | (unsigned long long)shufflePattern2, (((unsigned long long)shufflePattern1) << 32ull) | (unsigned long long)shufflePattern0);

				for (size_t n = 0; n < blocks16; ++n)
				{
					SSE::store128i(_mm_shuffle_epi8(SSE::load128i((const uint8_t*)source + 0), shufflePattern128), (uint8_t*)target + 0);
					SSE::store128i(_mm_shuffle_epi8(SSE::load128i((const uint8_t*)source + 16), shufflePattern128), (uint8_t*)target + 16);
					SSE::store128i(_mm_shuffle_epi8(SSE::load128i((const uint8_t*)source + 32), shufflePattern128), (uint8_t*)target + 32);
					SSE::store128i(_mm_shuffle_epi8(SSE::load128i((const uint8_t*)source + 48), shufflePattern128), (uint8_t*)target + 48);

					source += 16u * tSourceChannels;
					target += 16u * tTargetChannels;
				}

				break;
			}

			default:
				// we do not have a NEON-based optimization
				break;
		}
	}

#elif defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	if ((std::is_same<typename TypeMapper<T>::Type, uint8_t>::value))
	{
		const size_t blocks16 = size / size_t(16);

		switch (tSourceChannels | ((tTargetChannels) << 4u))
		{
			// 1 -> 3
			case (1u | (3u << 4u)):
			{
				static_assert(tSourceChannels != 1u || tShufflePattern == 0u, "Invalid shuffle patter!");

				for (size_t n = 0; n < blocks16; ++n)
				{
					const uint8x16_t source_u_8x16 = vld1q_u8((const uint8_t*)source);

					uint8x16x3_t target_u_8x16x3;

					for (unsigned int nT = 0u; nT < tTargetChannels; ++nT)
					{
						target_u_8x16x3.val[nT] = source_u_8x16;
					}

					vst3q_u8((uint8_t*)target, target_u_8x16x3);

					source += 16u * tSourceChannels;
					target += 16u * tTargetChannels;
				}

				break;
			}

			// 2 -> 1
			case (2u | (1u << 4u)):
			{
				for (size_t n = 0; n < blocks16; ++n)
				{
					const uint8x16x2_t source_u_8x16x2 = vld2q_u8((const uint8_t*)source);

					constexpr unsigned int sourceChannel = tShufflePattern & 0x00000001u; // possible index values {0, 1}
					static_assert(sourceChannel <= 1u, "Invalid shuffle pattern!");
					ocean_assert(sourceChannel == (tShufflePattern & 0x0000000Fu));

					const uint8x16_t target_u_8x16 = source_u_8x16x2.val[sourceChannel];

					vst1q_u8((uint8_t*)target, target_u_8x16);

					source += 16u * tSourceChannels;
					target += 16u * tTargetChannels;
				}

				break;
			}

			// 2 -> 3
			case (2u | (3u << 4u)):
			{
				for (size_t n = 0; n < blocks16; ++n)
				{
					const uint8x16x2_t source_u_8x16x2 = vld2q_u8((const uint8_t*)source);

					uint8x16x3_t target_u_8x16x3;

					for (unsigned int nT = 0u; nT < tTargetChannels; ++nT)
					{
						ocean_assert(((tShufflePattern >> (nT * 4u)) & 0x00000001u) == ((tShufflePattern >> (nT * 4u)) & 0x0000000Fu));

						target_u_8x16x3.val[nT] = source_u_8x16x2.val[(tShufflePattern >> (nT * 4u)) & 0x00000001u]; // possible index values {0, 1}
					}

					vst3q_u8((uint8_t*)target, target_u_8x16x3);

					source += 16u * tSourceChannels;
					target += 16u * tTargetChannels;
				}

				break;
			}

			// 2 -> 4
			case (2u | (4u << 4u)):
			{
				for (size_t n = 0; n < blocks16; ++n)
				{
					const uint8x16x2_t source_u_8x16x2 = vld2q_u8((const uint8_t*)source);

					uint8x16x4_t target_u_8x16x4;

					for (unsigned int nT = 0u; nT < tTargetChannels; ++nT)
					{
						ocean_assert(((tShufflePattern >> (nT * 4u)) & 0x00000001u) == ((tShufflePattern >> (nT * 4u)) & 0x0000000Fu));

						target_u_8x16x4.val[nT] = source_u_8x16x2.val[(tShufflePattern >> (nT * 4u)) & 0x00000001u]; // possible index values {0, 1}
					}

					vst4q_u8((uint8_t*)target, target_u_8x16x4);

					source += 16u * tSourceChannels;
					target += 16u * tTargetChannels;
				}

				break;
			}

			// 3 -> 1
			case (3u | (1u << 4u)):
			{
				constexpr unsigned int sourceChannel = (tShufflePattern & 0x0000000Fu) <= 2u ? (tShufflePattern & 0x0000000Fu) : 2u; // possible index values {0, 1, 2}
				ocean_assert(sourceChannel == (tShufflePattern & 0x0000000Fu));

				for (size_t n = 0; n < blocks16; ++n)
				{
					const uint8x16x3_t source_u_8x16x3 = vld3q_u8((const uint8_t*)source);

					const uint8x16_t target_u_8x16 = source_u_8x16x3.val[sourceChannel];

					vst1q_u8((uint8_t*)target, target_u_8x16);

					source += 16u * tSourceChannels;
					target += 16u * tTargetChannels;
				}

				break;
			}

			// 3 -> 2
			case (3u | (2u << 4u)):
			{
				for (size_t n = 0; n < blocks16; ++n)
				{
					const uint8x16x3_t source_u_8x16x3 = vld3q_u8((const uint8_t*)source);

					uint8x16x2_t target_u_8x16x2;

					for (unsigned int nT = 0u; nT < tTargetChannels; ++nT)
					{
						target_u_8x16x2.val[nT] = source_u_8x16x3.val[std::min((tShufflePattern >> (nT * 4u)) & 0x0000000Fu, 2u)]; // possible index values {0, 1, 2}
					}

					vst2q_u8((uint8_t*)target, target_u_8x16x2);

					source += 16u * tSourceChannels;
					target += 16u * tTargetChannels;
				}

				break;
			}

			// 3 -> 3
			case (3u | (3u << 4u)):
			{
				for (size_t n = 0; n < blocks16; ++n)
				{
					const uint8x16x3_t source_u_8x16x3 = vld3q_u8((const uint8_t*)source);

					uint8x16x3_t target_u_8x16x3;

					for (unsigned int nT = 0u; nT < tTargetChannels; ++nT)
					{
						target_u_8x16x3.val[nT] = source_u_8x16x3.val[std::min((tShufflePattern >> (nT * 4u)) & 0x0000000Fu, 2u)]; // possible index values {0, 1, 2}
					}

					vst3q_u8((uint8_t*)target, target_u_8x16x3);

					source += 16u * tSourceChannels;
					target += 16u * tTargetChannels;
				}

				break;
			}

			// 4 -> 1
			case (4u | (1u << 4u)):
			{
				for (size_t n = 0; n < blocks16; ++n)
				{
					const uint8x16x4_t source_u_8x16x4 = vld4q_u8((const uint8_t*)source);

					constexpr unsigned int sourceChannel = tShufflePattern & 0x00000003u; // possible index values {0, 1, 2, 3}
					static_assert(sourceChannel <= 3u, "Invalid shuffle pattern!");

					ocean_assert(sourceChannel == (tShufflePattern & 0x0000000Fu));

					const uint8x16_t target_u_8x16 = source_u_8x16x4.val[sourceChannel];

					vst1q_u8((uint8_t*)target, target_u_8x16);

					source += 16u * tSourceChannels;
					target += 16u * tTargetChannels;
				}

				break;
			}

			// 4 -> 2
			case (4u | (2u << 4u)):
			{
				for (size_t n = 0; n < blocks16; ++n)
				{
					const uint8x16x4_t source_u_8x16x4 = vld4q_u8((const uint8_t*)source);

					uint8x16x2_t target_u_8x16x2;

					for (unsigned int nT = 0u; nT < tTargetChannels; ++nT)
					{
						ocean_assert(((tShufflePattern >> (nT * 4u)) & 0x00000003u) == ((tShufflePattern >> (nT * 4u)) & 0x0000000Fu));

						target_u_8x16x2.val[nT] = source_u_8x16x4.val[(tShufflePattern >> (nT * 4u)) & 0x00000003u]; // possible index values {0, 1, 2, 3}
					}

					vst2q_u8((uint8_t*)target, target_u_8x16x2);

					source += 16u * tSourceChannels;
					target += 16u * tTargetChannels;
				}

				break;
			}

			// 4 -> 3
			case (4u | (3u << 4u)):
			{
				for (size_t n = 0; n < blocks16; ++n)
				{
					const uint8x16x4_t source_u_8x16x4 = vld4q_u8((const uint8_t*)source);

					uint8x16x3_t target_u_8x16x3;

					for (unsigned int nT = 0u; nT < tTargetChannels; ++nT)
					{
						ocean_assert(((tShufflePattern >> (nT * 4u)) & 0x00000003u) == ((tShufflePattern >> (nT * 4u)) & 0x0000000Fu));

						target_u_8x16x3.val[nT] = source_u_8x16x4.val[(tShufflePattern >> (nT * 4u)) & 0x00000003u]; // possible index values {0, 1, 2, 3}
					}

					vst3q_u8((uint8_t*)target, target_u_8x16x3);

					source += 16u * tSourceChannels;
					target += 16u * tTargetChannels;
				}

				break;
			}

			// 4 -> 4
			case (4u | (4u << 4u)):
			{
				for (size_t n = 0; n < blocks16; ++n)
				{
					const uint8x16x4_t source_u_8x16x4 = vld4q_u8((const uint8_t*)source);

					uint8x16x4_t target_u_8x16x4;

					for (unsigned int nT = 0u; nT < tTargetChannels; ++nT)
					{
						ocean_assert(((tShufflePattern >> (nT * 4u)) & 0x00000003u) == ((tShufflePattern >> (nT * 4u)) & 0x0000000Fu));

						target_u_8x16x4.val[nT] = source_u_8x16x4.val[(tShufflePattern >> (nT * 4u)) & 0x00000003u]; // possible index values {0, 1, 2, 3}
					}

					vst4q_u8((uint8_t*)target, target_u_8x16x4);

					source += 16u * tSourceChannels;
					target += 16u * tTargetChannels;
				}

				break;
			}

			default:
				// we do not have a NEON-based optimization
				break;
		}
	}

#endif

	while (source != sourceEnd)
	{
		ocean_assert(source < sourceEnd);

		for (unsigned int n = 0u; n < tTargetChannels; ++n)
		{
			target[n] = source[(tShufflePattern >> (n * 4u)) & 0x0000000Fu];
		}

		source += tSourceChannels;
		target += tTargetChannels;
	}
}

template <typename T, unsigned int tSourceChannels, unsigned int tTargetChannels, unsigned int tShufflePattern>
inline void FrameChannels::shuffleRowChannelsAndSetLastChannelValue(const T* source, T* target, const size_t size, const void* options)
{
	static_assert(tSourceChannels >= 1u && tSourceChannels <= 8u, "Invalid channel number!");
	static_assert(tTargetChannels >= 2u && tTargetChannels <= 8u, "Invalid channel number!");

	static_assert(((tShufflePattern & 0x0000000Fu) >> 0u) < tSourceChannels, "Invalid shuffle pattern!");
	static_assert(((tShufflePattern & 0x000000F0u) >> 4u) < tSourceChannels, "Invalid shuffle pattern!");
	static_assert(((tShufflePattern & 0x00000F00u) >> 8u) < tSourceChannels, "Invalid shuffle pattern!");
	static_assert(((tShufflePattern & 0x0000F000u) >> 12u) < tSourceChannels, "Invalid shuffle pattern!");
	static_assert(((tShufflePattern & 0x000F0000u) >> 16u) < tSourceChannels, "Invalid shuffle pattern!");
	static_assert(((tShufflePattern & 0x00F00000u) >> 20u) < tSourceChannels, "Invalid shuffle pattern!");
	static_assert(((tShufflePattern & 0x0F000000u) >> 24u) < tSourceChannels, "Invalid shuffle pattern!");
	static_assert(((tShufflePattern & 0xF0000000u) >> 28u) < tSourceChannels, "Invalid shuffle pattern!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(size != 0);

	ocean_assert(options != nullptr);

	const T lastChannelValue = *(const T*)(options);

	const T* const sourceEnd = source + size * tSourceChannels;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	if ((std::is_same<typename TypeMapper<T>::Type, uint8_t>::value))
	{
		const size_t blocks16 = size / size_t(16);

		switch (tSourceChannels | ((tTargetChannels) << 4u))
		{
			// 1 -> 4
			case (1u | (4u << 4u)):
			{
				ocean_assert(tShufflePattern == 0u);

				const uint8x16_t lastChannelValue_u_8x16 = vmovq_n_u8(lastChannelValue);

				uint8x16x4_t target_u_8x16x4;
				target_u_8x16x4.val[3] = lastChannelValue_u_8x16;

				for (size_t n = 0; n < blocks16; ++n)
				{
					const uint8x16_t source_u_8x16 = vld1q_u8((const uint8_t*)source);

					for (unsigned int nT = 0u; nT < tTargetChannels - 1u; ++nT)
					{
						target_u_8x16x4.val[nT] = source_u_8x16;
					}

					vst4q_u8((uint8_t*)target, target_u_8x16x4);

					source += 16u * tSourceChannels;
					target += 16u * tTargetChannels;
				}

				break;
			}

			// 3 -> 4
			case (3u | (4u << 4u)):
			{
				const uint8x16_t lastChannelValue_u_8x16 = vmovq_n_u8(lastChannelValue);

				uint8x16x4_t target_u_8x16x4;
				target_u_8x16x4.val[3] = lastChannelValue_u_8x16;

				for (size_t n = 0; n < blocks16; ++n)
				{
					const uint8x16x3_t source_u_8x16x3 = vld3q_u8((const uint8_t*)source);

					for (unsigned int nT = 0u; nT < tTargetChannels - 1u; ++nT)
					{
						target_u_8x16x4.val[nT] = source_u_8x16x3.val[std::min((tShufflePattern >> (nT * 4u)) & 0x0000000Fu, 2u)]; // possible index values {0, 1, 2}
					}

					vst4q_u8((uint8_t*)target, target_u_8x16x4);

					source += 16u * tSourceChannels;
					target += 16u * tTargetChannels;
				}

				break;
			}

			// 4 -> 4
			case (4u | (4u << 4u)):
			{
				const uint8x16_t lastChannelValue_u_8x16 = vmovq_n_u8(lastChannelValue);

				uint8x16x4_t target_u_8x16x4;
				target_u_8x16x4.val[3] = lastChannelValue_u_8x16;

				for (size_t n = 0; n < blocks16; ++n)
				{
					const uint8x16x4_t source_u_8x16x4 = vld4q_u8((const uint8_t*)source);

					for (unsigned int nT = 0u; nT < tTargetChannels - 1u; ++nT)
					{
						target_u_8x16x4.val[nT] = source_u_8x16x4.val[std::min((tShufflePattern >> (nT * 4u)) & 0x0000000Fu, 3u)]; // possible index values {0, 1, 2, 3}
					}

					vst4q_u8((uint8_t*)target, target_u_8x16x4);

					source += 16u * tSourceChannels;
					target += 16u * tTargetChannels;
				}

				break;
			}

			default:
				// we do not have a NEON-based optimization
				break;
		}
	}

#endif

	while (source != sourceEnd)
	{
		ocean_assert(source < sourceEnd);

		for (unsigned int n = 0u; n < tTargetChannels - 1u; ++n)
		{
			target[n] = source[(tShufflePattern >> (n * 4u)) & 0x0000000Fu];
			target[tTargetChannels - 1u] = lastChannelValue;
		}

		source += tSourceChannels;
		target += tTargetChannels;
	}
}

template <typename T, unsigned int tSourceChannels, unsigned int tTargetChannels, unsigned int tShufflePattern>
inline void FrameChannels::shuffleChannels(const T* source, T* target, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(tSourceChannels >= 1u && tSourceChannels <= 8u, "Invalid channel number!");
	static_assert(tTargetChannels >= 1u && tTargetChannels <= 8u, "Invalid channel number!");

	static_assert(tSourceChannels != 1u || tTargetChannels != 1u, "Invalid channel number!");

	static_assert(((tShufflePattern & 0x0000000Fu) >> 0u) < tSourceChannels, "Invalid shuffle pattern!");
	static_assert(((tShufflePattern & 0x000000F0u) >> 4u) < tSourceChannels, "Invalid shuffle pattern!");
	static_assert(((tShufflePattern & 0x00000F00u) >> 8u) < tSourceChannels, "Invalid shuffle pattern!");
	static_assert(((tShufflePattern & 0x0000F000u) >> 12u) < tSourceChannels, "Invalid shuffle pattern!");
	static_assert(((tShufflePattern & 0x000F0000u) >> 16u) < tSourceChannels, "Invalid shuffle pattern!");
	static_assert(((tShufflePattern & 0x00F00000u) >> 20u) < tSourceChannels, "Invalid shuffle pattern!");
	static_assert(((tShufflePattern & 0x0F000000u) >> 24u) < tSourceChannels, "Invalid shuffle pattern!");
	static_assert(((tShufflePattern & 0xF0000000u) >> 28u) < tSourceChannels, "Invalid shuffle pattern!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int sourceStrideElements = width * tSourceChannels + sourcePaddingElements;
	const unsigned int targetStrideElements = width * tTargetChannels + targetPaddingElements;

	const bool areContinuous = sourcePaddingElements == 0u && targetPaddingElements == 0u;

	FrameConverter::convertGenericPixelFormat(source, target, width, height, sourceStrideElements, targetStrideElements, conversionFlag, FrameChannels::shuffleRowChannels<T, tSourceChannels, tTargetChannels, tShufflePattern>, FrameChannels::reverseRowPixelOrderInPlace<T, tTargetChannels>, areContinuous, nullptr, worker);
}

template <typename T, unsigned int tSourceChannels, unsigned int tTargetChannels, unsigned int tShufflePattern>
inline void FrameChannels::shuffleChannelsAndSetLastChannelValue(const T* source, const T newChannelValue, T* target, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(tSourceChannels >= 1u && tSourceChannels <= 8u, "Invalid channel number!");
	static_assert(tTargetChannels >= 2u && tTargetChannels <= 8u, "Invalid channel number!");

	static_assert(((tShufflePattern & 0x0000000Fu) >> 0u) < tSourceChannels, "Invalid shuffle pattern!");
	static_assert(((tShufflePattern & 0x000000F0u) >> 4u) < tSourceChannels, "Invalid shuffle pattern!");
	static_assert(((tShufflePattern & 0x00000F00u) >> 8u) < tSourceChannels, "Invalid shuffle pattern!");
	static_assert(((tShufflePattern & 0x0000F000u) >> 12u) < tSourceChannels, "Invalid shuffle pattern!");
	static_assert(((tShufflePattern & 0x000F0000u) >> 16u) < tSourceChannels, "Invalid shuffle pattern!");
	static_assert(((tShufflePattern & 0x00F00000u) >> 20u) < tSourceChannels, "Invalid shuffle pattern!");
	static_assert(((tShufflePattern & 0x0F000000u) >> 24u) < tSourceChannels, "Invalid shuffle pattern!");
	static_assert(((tShufflePattern & 0xF0000000u) >> 28u) < tSourceChannels, "Invalid shuffle pattern!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int sourceStrideElements = width * tSourceChannels + sourcePaddingElements;
	const unsigned int targetStrideElements = width * tTargetChannels + targetPaddingElements;

	const bool areContinuous = sourcePaddingElements == 0u && targetPaddingElements == 0u;

	const T options = newChannelValue;

	FrameConverter::convertGenericPixelFormat(source, target, width, height, sourceStrideElements, targetStrideElements, conversionFlag, FrameChannels::shuffleRowChannelsAndSetLastChannelValue<T, tSourceChannels, tTargetChannels, tShufflePattern>, FrameChannels::reverseRowPixelOrderInPlace<T, tTargetChannels>, areContinuous, &options, worker);
}

template <unsigned int tChannels>
inline void FrameChannels::narrow16BitPerChannelTo8BitPerChannel(const uint16_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int sourceStrideElements = width * tChannels + sourcePaddingElements;
	const unsigned int targetStrideElements = width * tChannels + targetPaddingElements;

	const bool areContinuous = sourcePaddingElements == 0u && targetPaddingElements == 0u;

	FrameConverter::convertGenericPixelFormat(source, target, width, height, sourceStrideElements, targetStrideElements, conversionFlag, FrameChannels::narrowRow16BitPerChannelTo8BitPerChannel<tChannels>, FrameChannels::reverseRowPixelOrderInPlace<uint8_t, tChannels>, areContinuous, nullptr, worker);
}

template <typename T, unsigned int tChannels, void (*tPixelFunction)(const T*, T*)>
void FrameChannels::applyPixelModifier(const T* source, T* target, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, Worker* worker)
{
	static_assert(tChannels > 0u, "Invalid channel number!");

	ocean_assert(source && target);
	ocean_assert(width != 0u && height != 0u);

	if (worker)
		worker->executeFunction(Worker::Function::createStatic(&FrameChannels::applyPixelModifierSubset<T, tChannels, tPixelFunction>, source, target, width, height, conversionFlag, 0u, 0u), 0u, height);
	else
		applyPixelModifierSubset<T, tChannels, tPixelFunction>(source, target, width, height, conversionFlag, 0u, height);
}

template <typename TSource, typename TTarget, unsigned int tSourceChannels, unsigned int tTargetChannels, void (*tPixelFunction)(const TSource*, TTarget*)>
void FrameChannels::applyAdvancedPixelModifier(const TSource* source, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const ConversionFlag conversionFlag, Worker* worker)
{
	static_assert(tSourceChannels > 0u, "Invalid source channel number!");
	static_assert(tTargetChannels > 0u, "Invalid target channel number!");

	ocean_assert(source && target);
	ocean_assert(width != 0u && height != 0u);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameChannels::applyAdvancedPixelModifierSubset<TSource, TTarget, tSourceChannels, tTargetChannels, tPixelFunction>, source, target, width, height, sourcePaddingElements, targetPaddingElements, conversionFlag, 0u, 0u), 0u, height);
	}
	else
	{
		applyAdvancedPixelModifierSubset<TSource, TTarget, tSourceChannels, tTargetChannels, tPixelFunction>(source, target, width, height, sourcePaddingElements, targetPaddingElements, conversionFlag, 0u, height);
	}
}

template <typename TSource0, typename TSource1, typename TTarget, typename TIntermediate, unsigned int tSourceChannels, unsigned int tTargetChannels, void (*tOperator)(const TSource0*, const TSource1*, TTarget*)>
void FrameChannels::applyBivariateOperator(const TSource0* source0, const TSource1* source1, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int source0PaddingElements, const unsigned int source1PaddingElements, const unsigned int targetPaddingElements, const ConversionFlag conversionFlag, Worker* worker)
{
	static_assert(tSourceChannels > 0u, "Invalid source channel number!");
	static_assert(tTargetChannels > 0u, "Invalid target channel number!");

	ocean_assert(source0 && source1 && target);
	ocean_assert(width != 0u && height != 0u);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameChannels::applyBivariateOperatorSubset<TSource0, TSource1, TTarget, TIntermediate, tSourceChannels, tTargetChannels, tOperator>, source0, source1, target, width, height, source0PaddingElements, source1PaddingElements, targetPaddingElements, conversionFlag, 0u, 0u), 0u, height);
	}
	else
	{
		FrameChannels::applyBivariateOperatorSubset<TSource0, TSource1, TTarget, TIntermediate, tSourceChannels, tTargetChannels, tOperator>(source0, source1, target, width, height, source0PaddingElements, source1PaddingElements, targetPaddingElements, conversionFlag, 0u, height);
	}
}

template <typename TSource, typename TTarget, unsigned int tSourceChannels, unsigned int tTargetChannels>
void FrameChannels::applyRowOperator(const TSource* source, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements,  const RowOperatorFunction<TSource, TTarget, tSourceChannels, tTargetChannels>& rowOperatorFunction, Worker* worker)
{
	static_assert(tSourceChannels > 0u, "Invalid source channel number!");
	static_assert(tTargetChannels > 0u, "Invalid target channel number!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width != 0u && height != 0u);

	const unsigned int sourceStrideElements = width * tSourceChannels + sourcePaddingElements;
	const unsigned int targetStrideElements = width * tTargetChannels + targetPaddingElements;

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameChannels::applyRowOperatorSubset<TSource, TTarget, tSourceChannels, tTargetChannels>, source, target, width, height, sourceStrideElements, targetStrideElements, rowOperatorFunction, 0u, 0u), 0u, height);
	}
	else
	{
		applyRowOperatorSubset<TSource, TTarget, tSourceChannels, tTargetChannels>(source, target, width, height, sourceStrideElements, targetStrideElements, rowOperatorFunction, 0u, height);
	}
}

template <typename T, unsigned int tChannels>
inline void FrameChannels::transformGeneric(const T* source, T* target, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int bytesPerRow = width * sizeof(T) * tChannels;

	const unsigned int sourceStrideBytes = width * sizeof(T) * tChannels + sizeof(T) * sourcePaddingElements;
	const unsigned int targetStrideBytes = width * sizeof(T) * tChannels + sizeof(T) * targetPaddingElements;

	typedef typename TypeMapper<T>::Type MappedType;

	const RowReversePixelOrderFunction<void> rowReversePixelOrderFunction = (const RowReversePixelOrderFunction<void>)(FrameChannels::reverseRowPixelOrder<MappedType, tChannels>);

	if (worker && height > 200u)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameChannels::transformGenericSubset, (const uint8_t*)(source), (uint8_t*)(target), width, height, conversionFlag, rowReversePixelOrderFunction, bytesPerRow, sourceStrideBytes, targetStrideBytes, 0u, 0u), 0u, height, 9u, 10u, 20u);
	}
	else
	{
		transformGenericSubset((const uint8_t*)(source), (uint8_t*)(target), width, height, conversionFlag, rowReversePixelOrderFunction, bytesPerRow, sourceStrideBytes, targetStrideBytes, 0u, height);
	}
}

template <unsigned int tChannels, unsigned int tAlphaChannelIndex>
void FrameChannels::premultipliedAlphaToStraightAlpha8BitPerChannel(uint8_t* const frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, Worker* worker)
{
	static_assert(tChannels >= 2u, "Invalid channel number!");
	static_assert(tAlphaChannelIndex < tChannels, "Invalid alpha channel index!");

	ocean_assert(frame != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	if (worker && height > 200u)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameChannels::premultipliedAlphaToStraightAlpha8BitPerChannelSubset<tChannels, tAlphaChannelIndex>, frame, width, framePaddingElements, 0u, 0u), 0u, height, 3u, 4u, 20u);
	}
	else
	{
		premultipliedAlphaToStraightAlpha8BitPerChannelSubset<tChannels, tAlphaChannelIndex>(frame, width, framePaddingElements, 0u, height);
	}
}

template <unsigned int tChannels, unsigned int tAlphaChannelIndex>
void FrameChannels::premultipliedAlphaToStraightAlpha8BitPerChannel(const uint8_t* const source, uint8_t* const target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(tChannels >= 2u, "Invalid channel number!");
	static_assert(tAlphaChannelIndex < tChannels, "Invalid alpha channel index!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	if (worker && height > 200u)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameChannels::premultipliedAlphaToStraightAlpha8BitPerChannelSubset<tChannels, tAlphaChannelIndex>, source, target, width, sourcePaddingElements, targetPaddingElements, 0u, 0u), 0u, height, 5u, 6u, 20u);
	}
	else
	{
		premultipliedAlphaToStraightAlpha8BitPerChannelSubset<tChannels, tAlphaChannelIndex>(source, target, width, sourcePaddingElements, targetPaddingElements, 0u, height);
	}
}

template <unsigned int tChannels, unsigned int tAlphaChannelIndex>
void FrameChannels::straightAlphaToPremultipliedAlpha8BitPerChannel(uint8_t* const frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, Worker* worker)
{
	static_assert(tChannels >= 2u, "Invalid channel number!");
	static_assert(tAlphaChannelIndex < tChannels, "Invalid alpha channel index!");

	ocean_assert(frame != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	if (worker && height > 200u)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameChannels::straightAlphaToPremultipliedAlpha8BitPerChannelSubset<tChannels, tAlphaChannelIndex>, frame, width, framePaddingElements, 0u, 0u), 0u, height, 3u, 4u, 20u);
	}
	else
	{
		straightAlphaToPremultipliedAlpha8BitPerChannelSubset<tChannels, tAlphaChannelIndex>(frame, width, framePaddingElements, 0u, height);
	}
}

template <unsigned int tChannels, unsigned int tAlphaChannelIndex>
void FrameChannels::straightAlphaToPremultipliedAlpha8BitPerChannel(const uint8_t* const source, uint8_t* const target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	static_assert(tChannels >= 2u, "Invalid channel number!");
	static_assert(tAlphaChannelIndex < tChannels, "Invalid alpha channel index!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	if (worker && height > 200u)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameChannels::straightAlphaToPremultipliedAlpha8BitPerChannelSubset<tChannels, tAlphaChannelIndex>, source, target, width, sourcePaddingElements, targetPaddingElements, 0u, 0u), 0u, height, 5u, 6u, 20u);
	}
	else
	{
		straightAlphaToPremultipliedAlpha8BitPerChannelSubset<tChannels, tAlphaChannelIndex>(source, target, width, sourcePaddingElements, targetPaddingElements, 0u, height);
	}
}

template <unsigned int tChannels>
void FrameChannels::narrowRow16BitPerChannelTo8BitPerChannel(const uint16_t* source, uint8_t* target, const size_t size, const void* /* unusedParameters */)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(size > 0);

	const uint16_t* const sourceEnd = source + size * tChannels;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	const size_t blocks8 = size / size_t(8);

	switch (tChannels)
	{
		case 4u:
		{
			for (size_t n = 0; n < blocks8; ++n)
			{
				const uint16x8_t sourceA_u_16x8 = vld1q_u16(source + 0);
				const uint16x8_t sourceB_u_16x8 = vld1q_u16(source + 8);
				const uint16x8_t sourceC_u_16x8 = vld1q_u16(source + 16);
				const uint16x8_t sourceD_u_16x8 = vld1q_u16(source + 24);

				const uint8x16_t targetAB_u_8x16 = vcombine_u8(vqrshrn_n_u16(sourceA_u_16x8, 8), vqrshrn_n_u16(sourceB_u_16x8, 8)); // narrowing rounded right shift: target = (source + 128) / 256
				const uint8x16_t targetCD_u_8x16 = vcombine_u8(vqrshrn_n_u16(sourceC_u_16x8, 8), vqrshrn_n_u16(sourceD_u_16x8, 8));

				vst1q_u8(target + 0, targetAB_u_8x16);
				vst1q_u8(target + 16, targetCD_u_8x16);

				source += 8u * tChannels;
				target += 8u * tChannels;
			}

			break;
		}

		default:
			break;
	}

#endif

	while (source != sourceEnd)
	{
		ocean_assert(source < sourceEnd);

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			ocean_assert((uint16_t)(source[n] >> 8u) <= 255u);
			target[n] = (uint8_t)(source[n] >> 8u);
		}

		source += tChannels;
		target += tChannels;
	}
}

template <typename T, unsigned int tSourceChannels, bool tAddToFront>
void FrameChannels::addChannelRow(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options)
{
	static_assert(tSourceChannels != 0u, "Invalid channel number!");
	static_assert(sizeof(size_t) == sizeof(const T*), "Invalid pointer size!");

	ocean_assert(sources != nullptr && targets != nullptr);
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(multipleRowIndex < height);
	ocean_assert(options != nullptr);

	const T* source = (const T*)(sources[0]);
	const T* sourceOneChannel = (const T*)(sources[1]);
	ocean_assert(source != nullptr && sourceOneChannel != nullptr);

	T* target = (T*)(targets[0]);
	ocean_assert(target != nullptr);

	const unsigned int* uintOptions = (const unsigned int*)options;
	ocean_assert(uintOptions != nullptr);

	const unsigned int sourcePaddingElements = uintOptions[0];
	const unsigned int sourceOneChannelPaddingElements = uintOptions[1];
	const unsigned int targetPaddingElements = uintOptions[2];

	const unsigned int targetChannels = tSourceChannels + 1u;

	const unsigned int sourceStrideElements = tSourceChannels * width + sourcePaddingElements;
	const unsigned int sourceOneChannelStrideElements = width + sourceOneChannelPaddingElements;
	const unsigned int targetStrideElements = targetChannels * width + targetPaddingElements;

	const bool flipTarget = conversionFlag == CONVERT_FLIPPED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;
	const bool mirrorTarget = conversionFlag == CONVERT_MIRRORED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;

	const T* sourceRow = source + sourceStrideElements * multipleRowIndex;
	const T* sourceOneChannelRow = sourceOneChannel + sourceOneChannelStrideElements * multipleRowIndex;
	T* targetRow = flipTarget ? target + targetStrideElements * (height - multipleRowIndex - 1u) : target + targetStrideElements * multipleRowIndex;

	if (mirrorTarget == false)
	{
		for (unsigned int n = 0u; n < width; ++n)
		{
			if constexpr (tAddToFront)
			{
				targetRow[0] = sourceOneChannelRow[0];

				for (unsigned int c = 0u; c < tSourceChannels; ++c)
				{
					targetRow[c + 1u] = sourceRow[c];
				}
			}
			else
			{
				for (unsigned int c = 0u; c < tSourceChannels; ++c)
				{
					targetRow[c] = sourceRow[c];
				}

				targetRow[tSourceChannels] = sourceOneChannelRow[0];
			}

			sourceRow += tSourceChannels;
			sourceOneChannelRow++;

			targetRow += targetChannels;
		}
	}
	else
	{
		targetRow += targetChannels * (width - 1u);

		for (unsigned int n = 0u; n < width; ++n)
		{
			if constexpr (tAddToFront)
			{
				targetRow[0] = sourceOneChannelRow[0];

				for (unsigned int c = 0u; c < tSourceChannels; ++c)
				{
					targetRow[c + 1u] = sourceRow[c];
				}
			}
			else
			{
				for (unsigned int c = 0u; c < tSourceChannels; ++c)
				{
					targetRow[c] = sourceRow[c];
				}

				targetRow[tSourceChannels] = sourceOneChannelRow[0];
			}

			sourceRow += tSourceChannels;
			sourceOneChannelRow++;

			targetRow -= targetChannels;
		}
	}
}

template <typename T, unsigned int tSourceChannels, bool tAddToFront>
void FrameChannels::addChannelValueRow(const T* source, T* target, const size_t size, const void* channelValueParameter)
{
	static_assert(tSourceChannels != 0u, "Invalid channel number!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(size > 0);
	ocean_assert(channelValueParameter != nullptr);

	const T& channelValue = *((const T*)channelValueParameter);

	const unsigned int targetChannels = tSourceChannels + 1u;

	for (size_t n = 0; n < size; ++n)
	{
		if constexpr (tAddToFront)
		{
			target[0] = channelValue;

			for (unsigned int c = 0u; c < tSourceChannels; ++c)
			{
				target[c + 1u] = source[c];
			}
		}
		else
		{
			for (unsigned int c = 0u; c < tSourceChannels; ++c)
			{
				target[c] = source[c];
			}

			target[tSourceChannels] = channelValue;
		}

		source += tSourceChannels;
		target += targetChannels;
	}
}

template <typename T, unsigned int tSourceChannels, unsigned int tTargetChannels, unsigned int tSourceChannelIndex, unsigned int tTargetChannelIndex>
void FrameChannels::copyChannelRow(const T* source, T* target, const size_t size, const void* /*unusedParameters*/)
{
	static_assert(tSourceChannels != 0u, "Invalid channel number!");
	static_assert(tTargetChannels != 0u, "Invalid channel number!");

	static_assert(tSourceChannelIndex < tSourceChannels, "Invalid channel number!");
	static_assert(tTargetChannelIndex < tTargetChannels, "Invalid channel number!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(size > 0);

	for (size_t n = 0; n < size; ++n)
	{
		target[tTargetChannelIndex] = source[tSourceChannelIndex];

		source += tSourceChannels;
		target += tTargetChannels;
	}
}

template <typename TSource, typename TTarget>
void FrameChannels::separateTo1ChannelRuntime(const TSource* const sourceFrame, TTarget* const* const targetFrames, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int sourceFramePaddingElements, const unsigned int* targetFramesPaddingElements)
{
	ocean_assert(sourceFrame != nullptr);
	ocean_assert(targetFrames != nullptr);

	ocean_assert(width != 0u && height != 0u);
	ocean_assert(channels != 0u);

#ifdef OCEAN_DEBUG
	for (unsigned int c = 0u; c < channels; ++c)
	{
		ocean_assert(targetFrames[c] != nullptr);
	}
#endif

	if (sourceFramePaddingElements == 0u && targetFramesPaddingElements == nullptr)
	{
		for (unsigned int n = 0u; n < width * height; ++n)
		{
			for (unsigned int c = 0u; c < channels; ++c)
			{
				targetFrames[c][n] = TTarget(sourceFrame[n * channels + c]);
			}
		}
	}
	else if (targetFramesPaddingElements == nullptr)
	{
		ocean_assert(sourceFramePaddingElements != 0u);

		const unsigned int sourceFrameStrideElements = width * channels + sourceFramePaddingElements;

		for (unsigned int y = 0u; y < height; ++y)
		{
			const TSource* const sourceRow = sourceFrame + y * sourceFrameStrideElements;

			const unsigned int targetRowOffset = y * width;

			for (unsigned int x = 0u; x < width; ++x)
			{
				for (unsigned int c = 0u; c < channels; ++c)
				{
					*(targetFrames[c] + targetRowOffset + x) = TTarget(*(sourceRow + x * channels + c));
				}
			}
		}
	}
	else
	{
		const unsigned int sourceFrameStrideElements = width * channels + sourceFramePaddingElements;

		Indices32 targetFrameStrideElements(channels);

		for (unsigned int c = 0u; c < channels; ++c)
		{
			targetFrameStrideElements[c] = width + targetFramesPaddingElements[c];
		}

		for (unsigned int y = 0u; y < height; ++y)
		{
			const TSource* const sourceRow = sourceFrame + y * sourceFrameStrideElements;

			for (unsigned int x = 0u; x < width; ++x)
			{
				for (unsigned int c = 0u; c < channels; ++c)
				{
					*(targetFrames[c] + y * targetFrameStrideElements[c] + x) = TTarget(*(sourceRow + x * channels + c));
				}
			}
		}
	}
}

template <typename TSource, typename TTarget>
void FrameChannels::zipChannelsRuntime(const TSource* const* sourceFrames, TTarget* const targetFrame, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int* sourceFramesPaddingElements, const unsigned int targetFramePaddingElements)
{
	ocean_assert(sourceFrames != nullptr);
	ocean_assert(targetFrame != nullptr);

	ocean_assert(width != 0u && height != 0u);
	ocean_assert(channels != 0u);

	bool allSourceFramesContinuous = true;

	if (sourceFramesPaddingElements != nullptr)
	{
		for (unsigned int n = 0u; n < channels; ++n)
		{
			if (sourceFramesPaddingElements[n] != 0u)
			{
				allSourceFramesContinuous = false;
				break;
			}
		}
	}

	if (allSourceFramesContinuous && targetFramePaddingElements == 0u)
	{
		for (unsigned int n = 0u; n < width * height; ++n)
		{
			for (unsigned int c = 0u; c < channels; ++c)
			{
				targetFrame[n * channels + c] = TTarget(sourceFrames[c][n]);
			}
		}
	}
	else
	{
		const unsigned int targetFrameStrideElements = width * channels + targetFramePaddingElements;

		Indices32 sourceFrameStrideElements(channels);

		for (unsigned int c = 0u; c < channels; ++c)
		{
			if (sourceFramesPaddingElements == nullptr)
			{
				sourceFrameStrideElements[c] = width;
			}
			else
			{
				sourceFrameStrideElements[c] = width + sourceFramesPaddingElements[c];
			}
		}

		for (unsigned int y = 0u; y < height; ++y)
		{
			TTarget* const targetRow = targetFrame + y * targetFrameStrideElements;

			for (unsigned int x = 0u; x < width; ++x)
			{
				for (unsigned int c = 0u; c < channels; ++c)
				{
					*(targetRow + x * channels + c) = TTarget(*(sourceFrames[c] + y * sourceFrameStrideElements[c] + x));
				}
			}
		}
	}
}

template <typename T, unsigned int tChannel, unsigned int tChannels>
void FrameChannels::setChannelSubset(T* frame, const unsigned int width, const T value, const unsigned int framePaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tChannel < tChannels, "Invalid channel index!");

	ocean_assert(frame != nullptr);

	const unsigned int frameStrideElements = width * tChannels + framePaddingElements;

	frame += firstRow * frameStrideElements + tChannel;

	for (unsigned int n = 0u; n < numberRows; ++n)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			frame[x * tChannels] = value;
		}

		frame += frameStrideElements;
	}
}

template <typename T, unsigned int tChannels, void (*tPixelFunction)(const T*, T*)>
void FrameChannels::applyPixelModifierSubset(const T* source, T* target, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number");

	ocean_assert(source && target);
	ocean_assert(source != target);

	ocean_assert(numberRows > 0u);
	ocean_assert(firstRow + numberRows <= height);

	const unsigned int widthElements = width * tChannels;
	const unsigned int targetBlockSize = widthElements * numberRows;

	switch (conversionFlag)
	{
		case CONVERT_NORMAL:
		{
			source += firstRow * widthElements;
			target += firstRow * widthElements;

			const T* const targetEnd = target + targetBlockSize;

			while (target != targetEnd)
			{
				tPixelFunction(source, target);

				source += tChannels;
				target += tChannels;
			}

			break;
		}

		case CONVERT_FLIPPED:
		{
			source += firstRow * widthElements;
			target += width * height * tChannels - (firstRow + 1u) * widthElements;

			const T* const targetEnd = target - targetBlockSize;

			while (target != targetEnd)
			{
				const T* const targetRowEnd = target + widthElements;

				while (target != targetRowEnd)
				{
					tPixelFunction(source, target);

					source += tChannels;
					target += tChannels;
				}

				target -= (widthElements << 1); // width * tChannels * 2
			}

			break;
		}

		case CONVERT_MIRRORED:
		{
			source += firstRow * widthElements;
			target += (firstRow + 1u) * widthElements;

			const T* const targetEnd = target + targetBlockSize;

			while (target != targetEnd)
			{
				const T* const targetRowEnd = target - widthElements;

				while (target != targetRowEnd)
				{
					tPixelFunction(source, target -= tChannels);

					source += tChannels;
				}

				target += widthElements << 1; // width * tChannels * 2;
			}

			break;
		}

		case CONVERT_FLIPPED_AND_MIRRORED:
		{
			source += firstRow * widthElements;
			target += width * height * tChannels - firstRow * widthElements;

			const T* const targetEnd = target - targetBlockSize;

			while (target != targetEnd)
			{
				tPixelFunction(source, target -= tChannels);

				source += tChannels;
			}

			break;
		}

		// default: this case is not handled
	}
}

template <typename TSource, typename TTarget, unsigned int tSourceChannels, unsigned int tTargetChannels, void (*tPixelFunction)(const TSource*, TTarget*)>
void FrameChannels::applyAdvancedPixelModifierSubset(const TSource* source, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const ConversionFlag conversionFlag, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tSourceChannels >= 1u, "Invalid source channel number");
	static_assert(tTargetChannels >= 1u, "Invalid target channel number");

	ocean_assert(source && target);
	ocean_assert((void*)source != (void*)target);

	ocean_assert(numberRows != 0u);
	ocean_assert(firstRow + numberRows <= height);

	const unsigned int sourceWidthElements = width * tSourceChannels;
	const unsigned int targetWidthElements = width * tTargetChannels;

	const unsigned int sourceStrideElements = sourceWidthElements + sourcePaddingElements;
	const unsigned int targetStrideElements = targetWidthElements + targetPaddingElements;

	switch (conversionFlag)
	{
		case CONVERT_NORMAL:
		{
			for (unsigned int rowIndex = firstRow; rowIndex < firstRow + numberRows; ++rowIndex)
			{
				const TSource* sourcePixel = source + rowIndex * sourceStrideElements;
				TTarget* targetPixel = target + rowIndex * targetStrideElements;

				for (unsigned int x = 0u; x < width; ++x)
				{
					tPixelFunction(sourcePixel, targetPixel);

					sourcePixel += tSourceChannels;
					targetPixel += tTargetChannels;
				}
			}

			break;
		}

		case CONVERT_FLIPPED:
		{
			for (unsigned int rowIndex = firstRow; rowIndex < firstRow + numberRows; ++rowIndex)
			{
				const TSource* sourcePixel = source + rowIndex * sourceStrideElements;
				TTarget* targetPixel = target + (height - rowIndex - 1u) * targetStrideElements;

				for (unsigned int x = 0u; x < width; ++x)
				{
					tPixelFunction(sourcePixel, targetPixel);

					sourcePixel += tSourceChannels;
					targetPixel += tTargetChannels;
				}
			}

			break;
		}

		case CONVERT_MIRRORED:
		{
			for (unsigned int rowIndex = firstRow; rowIndex < firstRow + numberRows; ++rowIndex)
			{
				const TSource* sourcePixel = source + rowIndex * sourceStrideElements;

				TTarget* const targetRowBegin = target + rowIndex * targetStrideElements;
				TTarget* targetPixel = targetRowBegin + targetWidthElements - tTargetChannels;

				for (unsigned int x = 0u; x < width; ++x)
				{
					ocean_assert(targetPixel >= targetRowBegin);
					tPixelFunction(sourcePixel, targetPixel);

					sourcePixel += tSourceChannels;
					targetPixel -= tTargetChannels;
				}
			}

			break;
		}

		case CONVERT_FLIPPED_AND_MIRRORED:
		{
			for (unsigned int rowIndex = firstRow; rowIndex < firstRow + numberRows; ++rowIndex)
			{
				const TSource* sourcePixel = source + rowIndex * sourceStrideElements;

				TTarget* const targetRowBegin = target + (height - rowIndex - 1u) * targetStrideElements;
				TTarget* targetPixel = targetRowBegin + targetWidthElements - tTargetChannels;

				for (unsigned int x = 0u; x < width; ++x)
				{
					ocean_assert(targetPixel >= targetRowBegin);
					tPixelFunction(sourcePixel, targetPixel);

					sourcePixel += tSourceChannels;
					targetPixel -= tTargetChannels;
				}
			}

			break;
		}

		// default: this case is not handled
	}
}

template <typename TSource0, typename TSource1, typename TTarget, typename TIntermediate, unsigned int tSourceChannels, unsigned int tTargetChannels, void (*tOperator)(const TSource0*, const TSource1*, TTarget*)>
void FrameChannels::applyBivariateOperatorSubset(const TSource0* source0, const TSource1* source1, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int source0PaddingElements, const unsigned int source1PaddingElements, const unsigned int targetPaddingElements, const ConversionFlag conversionFlag, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tSourceChannels >= 1u, "Invalid source channel number");
	static_assert(tTargetChannels >= 1u, "Invalid target channel number");
	static_assert(tOperator, "Invalid operator function");

	ocean_assert(source0 != nullptr && source1 != nullptr && target != nullptr);
	ocean_assert((const void*)(source0) != (const void*)(target));
	ocean_assert((const void*)(source1) != (const void*)(target));

	ocean_assert(numberRows != 0u);
	ocean_assert(firstRow + numberRows <= height);

	const unsigned int source0StrideElements = width * tSourceChannels + source0PaddingElements;
	const unsigned int source1StrideElements = width * tSourceChannels + source1PaddingElements;

	const unsigned int targetWidthElements = width * tTargetChannels;

	const unsigned int targetStrideElements = targetWidthElements + targetPaddingElements;

	switch (conversionFlag)
	{
		case CONVERT_NORMAL:
		{
			for (unsigned int rowIndex = firstRow; rowIndex < (firstRow + numberRows); ++rowIndex)
			{
				const TSource0* rowSource0 = source0 + rowIndex * source0StrideElements;
				const TSource1* rowSource1 = source1 + rowIndex * source1StrideElements;

				TTarget* rowTarget = target + rowIndex * targetStrideElements;
				const TTarget* const rowTargetEnd = rowTarget + targetWidthElements;

				while (rowTarget != rowTargetEnd)
				{
					ocean_assert(rowTarget < rowTargetEnd);

					tOperator(rowSource0, rowSource1, rowTarget);

					rowSource0 += tSourceChannels;
					rowSource1 += tSourceChannels;

					rowTarget += tTargetChannels;
				}
			}

			return;
		}

		case CONVERT_FLIPPED:
		{
			for (unsigned int rowIndex = firstRow; rowIndex < (firstRow + numberRows); ++rowIndex)
			{
				const TSource0* rowSource0 = source0 + rowIndex * source0StrideElements;
				const TSource1* rowSource1 = source1 + rowIndex * source1StrideElements;

				TTarget* rowTarget = target + (height - rowIndex - 1u) * targetStrideElements;
				const TTarget* const rowTargetEnd = rowTarget + targetWidthElements;

				while (rowTarget != rowTargetEnd)
				{
					ocean_assert(rowTarget < rowTargetEnd);

					tOperator(rowSource0, rowSource1, rowTarget);

					rowSource0 += tSourceChannels;
					rowSource1 += tSourceChannels;

					rowTarget += tTargetChannels;
				}
			}

			return;
		}

		case CONVERT_MIRRORED:
		{
			for (unsigned int rowIndex = firstRow; rowIndex < (firstRow + numberRows); ++rowIndex)
			{
				const TSource0* rowSource0 = source0 + rowIndex * source0StrideElements;
				const TSource1* rowSource1 = source1 + rowIndex * source1StrideElements;

				TTarget* rowTarget = target + rowIndex * targetStrideElements + targetWidthElements - tTargetChannels;
				const TTarget* const rowTargetEnd = rowTarget - targetWidthElements;

				while (rowTarget != rowTargetEnd)
				{
					ocean_assert(rowTarget > rowTargetEnd);

					tOperator(rowSource0, rowSource1, rowTarget);

					rowSource0 += tSourceChannels;
					rowSource1 += tSourceChannels;

					rowTarget -= tTargetChannels;
				}
			}

			return;
		}

		case CONVERT_FLIPPED_AND_MIRRORED:
		{
			for (unsigned int rowIndex = firstRow; rowIndex < (firstRow + numberRows); ++rowIndex)
			{
				const TSource0* rowSource0 = source0 + rowIndex * source0StrideElements;
				const TSource1* rowSource1 = source1 + rowIndex * source1StrideElements;

				TTarget* rowTarget = target + (height - rowIndex - 1u) * targetStrideElements + targetWidthElements - tTargetChannels;
				const TTarget* const rowTargetEnd = rowTarget - targetWidthElements;

				while (rowTarget != rowTargetEnd)
				{
					ocean_assert(rowTarget > rowTargetEnd);

					tOperator(rowSource0, rowSource1, rowTarget);

					rowSource0 += tSourceChannels;
					rowSource1 += tSourceChannels;

					rowTarget -= tTargetChannels;
				}
			}

			return;
		}

		default:
			ocean_assert(false && "This should never happen!");
			break;
	}
}

template <typename TSource, typename TTarget, unsigned int tSourceChannels, unsigned int tTargetChannels>
void FrameChannels::applyRowOperatorSubset(const TSource* source, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int sourceStrideElements, const unsigned int targetStrideElements, const RowOperatorFunction<TSource, TTarget, tSourceChannels, tTargetChannels> rowOperatorFunction, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tSourceChannels >= 1u, "Invalid source channel number");
	static_assert(tTargetChannels >= 1u, "Invalid target channel number");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert((const void*)source != (const void*)target);

	ocean_assert(width * tSourceChannels <= sourceStrideElements);
	ocean_assert(width * tTargetChannels <= targetStrideElements);

	ocean_assert(rowOperatorFunction != nullptr);

	ocean_assert(numberRows != 0u);
	ocean_assert(firstRow + numberRows <= height);

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		rowOperatorFunction(source + y * sourceStrideElements, target + y * targetStrideElements, width, height, y, sourceStrideElements, targetStrideElements);
	}
}

template <bool tUseFactorChannel0, bool tUseFactorChannel1, bool tUseFactorChannel2>
void FrameChannels::convertRow3ChannelsTo1Channel8BitPerChannel7BitPrecision(const uint8_t* source, uint8_t* target, const size_t size, const void* channelMultiplicationFactors_128)
{
	static_assert(tUseFactorChannel0 || tUseFactorChannel1 || tUseFactorChannel2, "Invalid channel factors!");

	ocean_assert(channelMultiplicationFactors_128 != nullptr);
	const unsigned int* channelFactors_128 = reinterpret_cast<const unsigned int*>(channelMultiplicationFactors_128);
	ocean_assert(channelFactors_128 != nullptr);

	const unsigned int factorChannel0_128 = channelFactors_128[0];
	const unsigned int factorChannel1_128 = channelFactors_128[1];
	const unsigned int factorChannel2_128 = channelFactors_128[2];

	ocean_assert(factorChannel0_128 <= 128u && factorChannel1_128 <= 128u && factorChannel2_128 <= 128u);
	ocean_assert(factorChannel0_128 + factorChannel1_128 + factorChannel2_128 == 128u);

	ocean_assert(tUseFactorChannel0 == (factorChannel0_128 != 0u));
	ocean_assert(tUseFactorChannel1 == (factorChannel1_128 != 0u));
	ocean_assert(tUseFactorChannel2 == (factorChannel2_128 != 0u));

	ocean_assert(source != nullptr && target != nullptr && size >= 1);

	const uint8_t* const targetEnd = target + size;

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

	constexpr size_t blockSize = 16;
	const size_t blocks = size / blockSize;

	const __m128i multiplicationFactors0_128_u_16x8 = _mm_set1_epi16(int16_t(factorChannel0_128));
	const __m128i multiplicationFactors1_128_u_16x8 = _mm_set1_epi16(int16_t(factorChannel1_128));
	const __m128i multiplicationFactors2_128_u_16x8 = _mm_set1_epi16(int16_t(factorChannel2_128));

	for (size_t n = 0; n < blocks; ++n)
	{
		convert3ChannelsTo1Channel16Pixels8BitPerChannel7BitPrecisionSSE(source, target, multiplicationFactors0_128_u_16x8, multiplicationFactors1_128_u_16x8, multiplicationFactors2_128_u_16x8);

		source += blockSize * size_t(3);
		target += blockSize;
	}

#elif defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	constexpr size_t blockSize = 8;
	const size_t blocks = size / blockSize;

	const uint8x8_t factorChannel0_128_u_8x8 = vdup_n_u8((uint8_t)factorChannel0_128);
	const uint8x8_t factorChannel1_128_u_8x8 = vdup_n_u8((uint8_t)factorChannel1_128);
	const uint8x8_t factorChannel2_128_u_8x8 = vdup_n_u8((uint8_t)factorChannel2_128);

	for (size_t n = 0; n < blocks; ++n)
	{
		convert3ChannelsTo1Channel8Pixels8BitPerChannel7BitPrecisionNEON<tUseFactorChannel0, tUseFactorChannel1, tUseFactorChannel2>(source, target, factorChannel0_128_u_8x8, factorChannel1_128_u_8x8, factorChannel2_128_u_8x8);

		source += blockSize * size_t(3);
		target += blockSize;
	}

#endif

	while (target != targetEnd)
	{
		ocean_assert(target < targetEnd);

		const unsigned int channel0 = tUseFactorChannel0 ? (source[0] * factorChannel0_128) : 0u;
		const unsigned int channel1 = tUseFactorChannel1 ? (source[1] * factorChannel1_128) : 0u;
		const unsigned int channel2 = tUseFactorChannel2 ? (source[2] * factorChannel2_128) : 0u;

		*target++ = (uint8_t)((channel0 + channel1 + channel2 + 64u) >> 7u);
		source += 3;
	}
}

template <bool tUseFactorChannel0, bool tUseFactorChannel1, bool tUseFactorChannel2, bool tUseFactorChannel3>
void FrameChannels::convertRow4ChannelsTo1Channel8BitPerChannel7BitPrecision(const uint8_t* source, uint8_t* target, const size_t size, const void* channelMultiplicationFactors_128)
{
	static_assert(tUseFactorChannel0 || tUseFactorChannel1 || tUseFactorChannel2 || tUseFactorChannel3, "Invalid channel factors!");

	ocean_assert(channelMultiplicationFactors_128 != nullptr);
	const unsigned int* channelFactors_128 = reinterpret_cast<const unsigned int*>(channelMultiplicationFactors_128);
	ocean_assert(channelFactors_128 != nullptr);

	const unsigned int factorChannel0_128 = channelFactors_128[0];
	const unsigned int factorChannel1_128 = channelFactors_128[1];
	const unsigned int factorChannel2_128 = channelFactors_128[2];
	const unsigned int factorChannel3_128 = channelFactors_128[3];

	ocean_assert(factorChannel0_128 <= 127u && factorChannel1_128 <= 127u && factorChannel2_128 <= 127u && factorChannel3_128 <= 127u);
	ocean_assert(factorChannel0_128 + factorChannel1_128 + factorChannel2_128 + factorChannel3_128 == 128u);

	ocean_assert(tUseFactorChannel0 == (factorChannel0_128 != 0u));
	ocean_assert(tUseFactorChannel1 == (factorChannel1_128 != 0u));
	ocean_assert(tUseFactorChannel2 == (factorChannel2_128 != 0u));
	ocean_assert(tUseFactorChannel3 == (factorChannel3_128 != 0u));

	ocean_assert(source != nullptr && target != nullptr && size >= 1);

	const uint8_t* const targetEnd = target + size;

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

	constexpr size_t blockSize = 16;
	const size_t blocks = size / blockSize;

	const __m128i m128_multiplicationFactors = _mm_set1_epi32(int(factorChannel0_128 | (factorChannel1_128 << 8u) | (factorChannel2_128 << 16u) | (factorChannel3_128 << 24u)));

	for (size_t n = 0; n < blocks; ++n)
	{
		convert4ChannelsTo1Channel16Pixels8BitPerChannel7BitPrecisionSSE(source, target, m128_multiplicationFactors);

		source += blockSize * size_t(4);
		target += blockSize;
	}

#elif defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	constexpr size_t blockSize = 8;
	const size_t blocks = size / blockSize;

	const uint8x8_t factorChannel0_128_u_8x8 = vdup_n_u8((uint8_t)factorChannel0_128);
	const uint8x8_t factorChannel1_128_u_8x8 = vdup_n_u8((uint8_t)factorChannel1_128);
	const uint8x8_t factorChannel2_128_u_8x8 = vdup_n_u8((uint8_t)factorChannel2_128);
	const uint8x8_t factorChannel3_128_u_8x8 = vdup_n_u8((uint8_t)factorChannel3_128);

	for (size_t n = 0; n < blocks; ++n)
	{
		convert4ChannelsTo1Channel8Pixels8BitPerChannel7BitPrecisionNEON<tUseFactorChannel0, tUseFactorChannel1, tUseFactorChannel2, tUseFactorChannel3>(source, target, factorChannel0_128_u_8x8, factorChannel1_128_u_8x8, factorChannel2_128_u_8x8, factorChannel3_128_u_8x8);

		source += blockSize * size_t(4);
		target += blockSize;
	}

#endif

	while (target != targetEnd)
	{
		ocean_assert(target < targetEnd);

		const unsigned int channel0 = tUseFactorChannel0 ? (source[0] * factorChannel0_128) : 0u;
		const unsigned int channel1 = tUseFactorChannel1 ? (source[1] * factorChannel1_128) : 0u;
		const unsigned int channel2 = tUseFactorChannel2 ? (source[2] * factorChannel2_128) : 0u;
		const unsigned int channel3 = tUseFactorChannel3 ? (source[3] * factorChannel3_128) : 0u;

		*target++ = (uint8_t)((channel0 + channel1 + channel2 + channel3 + 64u) >> 7u);
		source += 4;
	}
}

template <unsigned int tChannels, unsigned int tAlphaChannelIndex>
void FrameChannels::premultipliedAlphaToStraightAlpha8BitPerChannelSubset(uint8_t* const frame, const unsigned int width, const unsigned int framePaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannels >= 2u, "Invalid channel number!");
	static_assert(tAlphaChannelIndex < tChannels, "Invalid alpha channel index!");

	ocean_assert(frame != nullptr);
	ocean_assert(width >= 1u);

	const unsigned int frameStrideElements = width * tChannels + framePaddingElements;

	uint8_t* frameRow = frame + frameStrideElements * firstRow;

	for (unsigned int y = 0u; y < numberRows; ++y)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			if (frameRow[tAlphaChannelIndex])
			{
				const uint8_t alpha_2 = frameRow[tAlphaChannelIndex] / 2u;

				for (unsigned int channelIndex = 0u; channelIndex < tChannels; ++channelIndex)
				{
					if (channelIndex != tAlphaChannelIndex)
					{
						frameRow[channelIndex] = uint8_t(std::min((frameRow[channelIndex] * 255u + alpha_2) / frameRow[tAlphaChannelIndex], 255u));
					}
				}
			}

			frameRow += tChannels;
		}

		frameRow += framePaddingElements;
	}
}

template <unsigned int tChannels, unsigned int tAlphaChannelIndex>
void FrameChannels::premultipliedAlphaToStraightAlpha8BitPerChannelSubset(const uint8_t* const source, uint8_t* target, const unsigned int width, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannels >= 2u, "Invalid channel number!");
	static_assert(tAlphaChannelIndex < tChannels, "Invalid alpha channel index!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u);

	const unsigned int sourceStrideElements = width * tChannels + sourcePaddingElements;
	const unsigned int targetStrideElements = width * tChannels + targetPaddingElements;

	const uint8_t* sourceRow = source + sourceStrideElements * firstRow;
	uint8_t* targetRow = target + targetStrideElements * firstRow;

	for (unsigned int y = 0u; y < numberRows; ++y)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			if (sourceRow[tAlphaChannelIndex])
			{
				const uint8_t alpha_2 = sourceRow[tAlphaChannelIndex] / 2u;

				for (unsigned int channelIndex = 0u; channelIndex < tChannels; ++channelIndex)
				{
					if (channelIndex != tAlphaChannelIndex)
					{
						targetRow[channelIndex] = uint8_t(std::max((sourceRow[channelIndex] * 255u + alpha_2) / sourceRow[tAlphaChannelIndex], 255u));
					}
					else
					{
						targetRow[channelIndex] = sourceRow[channelIndex];
					}
				}
			}
			else
			{
				for (unsigned int channelIndex = 0u; channelIndex < tChannels; ++channelIndex)
				{
					targetRow[channelIndex] = sourceRow[channelIndex];
				}
			}

			sourceRow += tChannels;
			targetRow += tChannels;
		}

		sourceRow += sourcePaddingElements;
		targetRow += targetPaddingElements;
	}
}

template <unsigned int tChannels, unsigned int tAlphaChannelIndex>
void FrameChannels::straightAlphaToPremultipliedAlpha8BitPerChannelSubset(uint8_t* const frame, const unsigned int width, const unsigned int framePaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannels >= 2u, "Invalid channel number!");
	static_assert(tAlphaChannelIndex < tChannels, "Invalid alpha channel index!");

	ocean_assert(frame != nullptr);
	ocean_assert(width >= 1u);

	const unsigned int frameStrideElements = width * tChannels + framePaddingElements;

	uint8_t* frameRow = frame + frameStrideElements * firstRow;

	for (unsigned int y = 0u; y < numberRows; ++y)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			for (unsigned int channelIndex = 0u; channelIndex < tChannels; ++channelIndex)
			{
				if (channelIndex != tAlphaChannelIndex)
				{
					frameRow[channelIndex] = (frameRow[channelIndex] * frameRow[tAlphaChannelIndex] + 127u) / 255u;
				}
			}

			frameRow += tChannels;
		}

		frameRow += framePaddingElements;
	}
}

template <unsigned int tChannels, unsigned int tAlphaChannelIndex>
void FrameChannels::straightAlphaToPremultipliedAlpha8BitPerChannelSubset(const uint8_t* const source, uint8_t* const target, const unsigned int width, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	static_assert(tChannels >= 2u, "Invalid channel number!");
	static_assert(tAlphaChannelIndex < tChannels, "Invalid alpha channel index!");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u);

	const unsigned int sourceStrideElements = width * tChannels + sourcePaddingElements;
	const unsigned int targetStrideElements = width * tChannels + targetPaddingElements;

	const uint8_t* sourceRow = source + sourceStrideElements * firstRow;
	uint8_t* targetRow = target + targetStrideElements * firstRow;

	for (unsigned int y = 0u; y < numberRows; ++y)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			for (unsigned int channelIndex = 0u; channelIndex < tChannels; ++channelIndex)
			{
				if (channelIndex != tAlphaChannelIndex)
				{
					targetRow[channelIndex] = (sourceRow[channelIndex] * sourceRow[tAlphaChannelIndex] + 127u) / 255u;
				}
				else
				{
					targetRow[channelIndex] = sourceRow[channelIndex];
				}
			}

			sourceRow += tChannels;
			targetRow += tChannels;
		}

		sourceRow += sourcePaddingElements;
		targetRow += targetPaddingElements;
	}
}

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

OCEAN_FORCE_INLINE void FrameChannels::convert3ChannelsTo1Channel16Pixels8BitPerChannel7BitPrecisionSSE(const uint8_t* const source, uint8_t* const target, const __m128i& multiplicationFactors0_128_u_16x8, const __m128i& multiplicationFactors1_128_u_16x8, const __m128i& multiplicationFactors2_128_u_16x8)
{
	ocean_assert(source != nullptr && target != nullptr);

	// the documentation of this function is designed for RGB24 to Y8 conversion
	// however, in general this function can be used to apply a linear combination on the four source channels
	// to create one output channel

	// precise color space conversion:
	// Y = 0.299 * R + 0.587 * G + 0.114 * B

	// approximation:
	// Y = (38 * R + 75 * G + 15 * B) / 128

	// we expect the following input pattern (for here RGB24):
	// FEDC BA98 7654 3210  FEDC BA98 7654 3210  FEDC BA98 7654 3210
	// BGRB GRBG RBGR BGRB  GRBG RBGR BGRB GRBG  RBGR BGRB GRBG RBGR

	// we store eight 16 bit values holding 64 for rounding purpose:
	const __m128i constant64_u_16x8 = _mm_set1_epi32(0x00400040);

	const __m128i sourceA_u_8x16 = _mm_loadu_si128((const __m128i*)source + 0);
	const __m128i sourceB_u_8x16 = _mm_loadu_si128((const __m128i*)source + 1);
	const __m128i sourceC_u_8x16 = _mm_loadu_si128((const __m128i*)source + 2);

	__m128i channel0_u_8x16;
	__m128i channel1_u_8x16;
	__m128i channel2_u_8x16;
	SSE::deInterleave3Channel8Bit48Elements(sourceA_u_8x16, sourceB_u_8x16, sourceC_u_8x16, channel0_u_8x16, channel1_u_8x16, channel2_u_8x16);

	// now we need 16 bit values instead of 8 bit values

	const __m128i channel0_low_u_8x16 = SSE::removeHighBits16_8(channel0_u_8x16);
	const __m128i channel1_low_u_8x16 = SSE::removeHighBits16_8(channel1_u_8x16);
	const __m128i channel2_low_u_8x16 = SSE::removeHighBits16_8(channel2_u_8x16);

	const __m128i channel0_high_u_8x16 = _mm_srli_epi16(channel0_u_8x16, 8);
	const __m128i channel1_high_u_8x16 = _mm_srli_epi16(channel1_u_8x16, 8);
	const __m128i channel2_high_u_8x16 = _mm_srli_epi16(channel2_u_8x16, 8);

	// we multiply each channel with the corresponding multiplication factors

	const __m128i result0_low_u_8x16 = _mm_mullo_epi16(channel0_low_u_8x16, multiplicationFactors0_128_u_16x8);
	const __m128i result0_high_u_8x16 = _mm_mullo_epi16(channel0_high_u_8x16, multiplicationFactors0_128_u_16x8);

	const __m128i result1_low_u_8x16 = _mm_mullo_epi16(channel1_low_u_8x16, multiplicationFactors1_128_u_16x8);
	const __m128i result1_high_u_8x16 = _mm_mullo_epi16(channel1_high_u_8x16, multiplicationFactors1_128_u_16x8);

	const __m128i result2_low_u_8x16 = _mm_mullo_epi16(channel2_low_u_8x16, multiplicationFactors2_128_u_16x8);
	const __m128i result2_high_u_8x16 = _mm_mullo_epi16(channel2_high_u_8x16, multiplicationFactors2_128_u_16x8);

	// we sum up all results and add 64 for rounding purpose
	const __m128i result128_low_u_8x16 = _mm_adds_epu16(_mm_adds_epu16(result0_low_u_8x16, result1_low_u_8x16), _mm_adds_epu16(result2_low_u_8x16, constant64_u_16x8));
	const __m128i result128_high_u_8x16 = _mm_adds_epu16(_mm_adds_epu16(result0_high_u_8x16, result1_high_u_8x16), _mm_adds_epu16(result2_high_u_8x16, constant64_u_16x8));

	// we shift the multiplication results by 7 bits (= 128)
	const __m128i result_low_u_8x16 = _mm_srli_epi16(result128_low_u_8x16, 7);
	const __m128i result_high_u_8x16 = _mm_srli_epi16(result128_high_u_8x16, 7);

	// finally, we have to get rid of the upper zero bits by combining two 128 bit registers to one:
	const __m128i result_u_8x16 = _mm_or_si128(result_low_u_8x16, _mm_slli_epi16(result_high_u_8x16, 8));

	// and we can store the result
	_mm_storeu_si128((__m128i*)target, result_u_8x16);
}

OCEAN_FORCE_INLINE void FrameChannels::convert3ChannelsTo3Channels16Pixels8BitPerChannel7BitPrecisionSSE(const uint8_t* const source, uint8_t* const target, const __m128i& factorChannel00_128_s_16x8, const __m128i& factorChannel10_128_s_16x8, const __m128i& factorChannel20_128_s_16x8, const __m128i& factorChannel01_128_s_16x8, const __m128i& factorChannel11_128_s_16x8, const __m128i& factorChannel21_128_s_16x8, const __m128i& factorChannel02_128_s_16x8, const __m128i& factorChannel12_128_s_16x8, const __m128i& factorChannel22_128_s_16x8, const __m128i& biasChannel0_s_16x8, const __m128i& biasChannel1_s_16x8, const __m128i& biasChannel2_s_16x8)
{
	ocean_assert(source != nullptr && target != nullptr);

	// the documentation of this function designed for RGB24 to YUV24 conversion

	// precise color space conversion:
	// | Y |   |  0.2578125   0.5039063   0.09765625  16.0  |   | R |
	// | U | = | -0.1484375  -0.2890625   0.4375      128.0 | * | G |
	// | V |   |  0.4375     -0.3671875  -0.0703125   128.0 |   | B |
	//                                                          | 1 |

	// approximation:
	// Y = ( 33 * R + 64 * G + 13 * B) / 128 + 16
	// U = (-19 * R - 37 * G + 56 * B) / 128 + 128
	// V = ( 56 * R - 47 * G -  9 * B) / 128 + 128

	// we expect the following input pattern (for here RGB24):
	// FEDC BA98 7654 3210  FEDC BA98 7654 3210  FEDC BA98 7654 3210
	// BGRB GRBG RBGR BGRB  GRBG RBGR BGRB GRBG  RBGR BGRB GRBG RBGR

	const __m128i sourceA_u_8x16 = _mm_loadu_si128((const __m128i*)source + 0);
	const __m128i sourceB_u_8x16 = _mm_loadu_si128((const __m128i*)source + 1);
	const __m128i sourceC_u_8x16 = _mm_loadu_si128((const __m128i*)source + 2);

	__m128i channel0_u_8x16;
	__m128i channel1_u_8x16;
	__m128i channel2_u_8x16;
	SSE::deInterleave3Channel8Bit48Elements(sourceA_u_8x16, sourceB_u_8x16, sourceC_u_8x16, channel0_u_8x16, channel1_u_8x16, channel2_u_8x16);

	// now we need 16 bit values instead of 8 bit values

	const __m128i channel0_low_u_8x16 = SSE::removeHighBits16_8(channel0_u_8x16);
	const __m128i channel1_low_u_8x16 = SSE::removeHighBits16_8(channel1_u_8x16);
	const __m128i channel2_low_u_8x16 = SSE::removeHighBits16_8(channel2_u_8x16);

	const __m128i channel0_high_u_8x16 = _mm_srli_epi16(channel0_u_8x16, 8);
	const __m128i channel1_high_u_8x16 = _mm_srli_epi16(channel1_u_8x16, 8);
	const __m128i channel2_high_u_8x16 = _mm_srli_epi16(channel2_u_8x16, 8);

	// we multiply each channel with the corresponding multiplication factors

	__m128i result0_low_u_8x16 = _mm_add_epi16(_mm_add_epi16(_mm_mullo_epi16(channel0_low_u_8x16, factorChannel00_128_s_16x8), _mm_mullo_epi16(channel1_low_u_8x16, factorChannel01_128_s_16x8)), _mm_mullo_epi16(channel2_low_u_8x16, factorChannel02_128_s_16x8));
	__m128i result1_low_u_8x16 = _mm_add_epi16(_mm_add_epi16(_mm_mullo_epi16(channel0_low_u_8x16, factorChannel10_128_s_16x8), _mm_mullo_epi16(channel1_low_u_8x16, factorChannel11_128_s_16x8)), _mm_mullo_epi16(channel2_low_u_8x16, factorChannel12_128_s_16x8));
	__m128i result2_low_u_8x16 = _mm_add_epi16(_mm_add_epi16(_mm_mullo_epi16(channel0_low_u_8x16, factorChannel20_128_s_16x8), _mm_mullo_epi16(channel1_low_u_8x16, factorChannel21_128_s_16x8)), _mm_mullo_epi16(channel2_low_u_8x16, factorChannel22_128_s_16x8));

	__m128i result0_high_u_8x16 = _mm_add_epi16(_mm_add_epi16(_mm_mullo_epi16(channel0_high_u_8x16, factorChannel00_128_s_16x8), _mm_mullo_epi16(channel1_high_u_8x16, factorChannel01_128_s_16x8)), _mm_mullo_epi16(channel2_high_u_8x16, factorChannel02_128_s_16x8));
	__m128i result1_high_u_8x16 = _mm_add_epi16(_mm_add_epi16(_mm_mullo_epi16(channel0_high_u_8x16, factorChannel10_128_s_16x8), _mm_mullo_epi16(channel1_high_u_8x16, factorChannel11_128_s_16x8)), _mm_mullo_epi16(channel2_high_u_8x16, factorChannel12_128_s_16x8));
	__m128i result2_high_u_8x16 = _mm_add_epi16(_mm_add_epi16(_mm_mullo_epi16(channel0_high_u_8x16, factorChannel20_128_s_16x8), _mm_mullo_epi16(channel1_high_u_8x16, factorChannel21_128_s_16x8)), _mm_mullo_epi16(channel2_high_u_8x16, factorChannel22_128_s_16x8));

	// we normalize the result by 128 and add the bias

	result0_low_u_8x16 = _mm_add_epi16(SSE::divideByRightShiftSigned16Bit(result0_low_u_8x16, 7), biasChannel0_s_16x8);
	result1_low_u_8x16 = _mm_add_epi16(SSE::divideByRightShiftSigned16Bit(result1_low_u_8x16, 7), biasChannel1_s_16x8);
	result2_low_u_8x16 = _mm_add_epi16(SSE::divideByRightShiftSigned16Bit(result2_low_u_8x16, 7), biasChannel2_s_16x8);

	result0_high_u_8x16 = _mm_add_epi16(SSE::divideByRightShiftSigned16Bit(result0_high_u_8x16, 7), biasChannel0_s_16x8);
	result1_high_u_8x16 = _mm_add_epi16(SSE::divideByRightShiftSigned16Bit(result1_high_u_8x16, 7), biasChannel1_s_16x8);
	result2_high_u_8x16 = _mm_add_epi16(SSE::divideByRightShiftSigned16Bit(result2_high_u_8x16, 7), biasChannel2_s_16x8);

	// from here, we need values within the range [0, 255], so that we clamp the results

	const __m128i constant255_s_16x8 = _mm_set1_epi16(255);

	result0_low_u_8x16 = _mm_min_epi16(_mm_max_epi16(result0_low_u_8x16, _mm_setzero_si128()), constant255_s_16x8);
	result1_low_u_8x16 = _mm_min_epi16(_mm_max_epi16(result1_low_u_8x16, _mm_setzero_si128()), constant255_s_16x8);
	result2_low_u_8x16 = _mm_min_epi16(_mm_max_epi16(result2_low_u_8x16, _mm_setzero_si128()), constant255_s_16x8);

	result0_high_u_8x16 = _mm_min_epi16(_mm_max_epi16(result0_high_u_8x16, _mm_setzero_si128()), constant255_s_16x8);
	result1_high_u_8x16 = _mm_min_epi16(_mm_max_epi16(result1_high_u_8x16, _mm_setzero_si128()), constant255_s_16x8);
	result2_high_u_8x16 = _mm_min_epi16(_mm_max_epi16(result2_high_u_8x16, _mm_setzero_si128()), constant255_s_16x8);

	// finally, we have to get rid of the upper zero bits by combining two 128 bit registers to one:
	const __m128i result0_u_8x16 = _mm_or_si128(result0_low_u_8x16, _mm_slli_epi16(result0_high_u_8x16, 8));
	const __m128i result1_u_8x16 = _mm_or_si128(result1_low_u_8x16, _mm_slli_epi16(result1_high_u_8x16, 8));
	const __m128i result2_u_8x16 = _mm_or_si128(result2_low_u_8x16, _mm_slli_epi16(result2_high_u_8x16, 8));

	__m128i resultA_u_8x16;
	__m128i resultB_u_8x16;
	__m128i resultC_u_8x16;
	SSE::interleave3Channel8Bit48Elements(result0_u_8x16, result1_u_8x16, result2_u_8x16, resultA_u_8x16, resultB_u_8x16, resultC_u_8x16);

	// and we can store the result
	_mm_storeu_si128((__m128i*)target + 0, resultA_u_8x16);
	_mm_storeu_si128((__m128i*)target + 1, resultB_u_8x16);
	_mm_storeu_si128((__m128i*)target + 2, resultC_u_8x16);
}

OCEAN_FORCE_INLINE void FrameChannels::convert3ChannelsTo3Channels16Pixels8BitPerChannel10BitPrecisionSSE(const uint8_t* const source, uint8_t* const target, const __m128i& factorChannel00_1024_s_16x8, const __m128i& factorChannel10_1024_s_16x8, const __m128i& factorChannel20_1024_s_16x8, const __m128i& factorChannel01_1024_s_16x8, const __m128i& factorChannel11_1024_s_16x8, const __m128i& factorChannel21_1024_s_16x8, const __m128i& factorChannel02_1024_s_16x8, const __m128i& factorChannel12_1024_s_16x8, const __m128i& factorChannel22_1024_s_16x8, const __m128i& biasChannel0_1024_s_32x4, const __m128i& biasChannel1_1024_s_32x4, const __m128i& biasChannel2_1024_s_32x4)
{
	ocean_assert(source != nullptr && target != nullptr);

	// the documentation of this function designed for RGB24 to YUV24 conversion

	/// precise color space conversion:
	// | R |   | 1.1639404296875   0.0              1.595947265625  -222.904296875 |   | Y |
	// | G | = | 1.1639404296875  -0.3909912109375 -0.81298828125    135.486328125 | * | U |
	// | B |   | 1.1639404296875   2.0179443359375  0.0             -276.919921875 |   | V |
	//                                                                                 | 1 |

	// approximation:
	// | R |   | 1192    0       1634   -223 |   | Y |
	// | G | = | 1192   -400    -833     135 | * | U |
	// | B |   | 1192    2066    0      -277 |   | V |
	//                                           | 1 |

	// we expect the following input pattern (for here RGB24):
	// FEDC BA98 7654 3210  FEDC BA98 7654 3210  FEDC BA98 7654 3210
	// BGRB GRBG RBGR BGRB  GRBG RBGR BGRB GRBG  RBGR BGRB GRBG RBGR

	const __m128i sourceA_u_8x16 = _mm_loadu_si128((const __m128i*)source + 0);
	const __m128i sourceB_u_8x16 = _mm_loadu_si128((const __m128i*)source + 1);
	const __m128i sourceC_u_8x16 = _mm_loadu_si128((const __m128i*)source + 2);

	__m128i channel0_u_8x16;
	__m128i channel1_u_8x16;
	__m128i channel2_u_8x16;
	SSE::deInterleave3Channel8Bit48Elements(sourceA_u_8x16, sourceB_u_8x16, sourceC_u_8x16, channel0_u_8x16, channel1_u_8x16, channel2_u_8x16);


	// now we need 16 bit values instead of 8 bit values

	const __m128i channel0_low_u_16x8 = SSE::removeHighBits16_8(channel0_u_8x16);
	const __m128i channel1_low_u_16x8 = SSE::removeHighBits16_8(channel1_u_8x16);
	const __m128i channel2_low_u_16x8 = SSE::removeHighBits16_8(channel2_u_8x16);

	const __m128i channel0_high_u_16x8 = _mm_srli_epi16(channel0_u_8x16, 8);
	const __m128i channel1_high_u_16x8 = _mm_srli_epi16(channel1_u_8x16, 8);
	const __m128i channel2_high_u_16x8 = _mm_srli_epi16(channel2_u_8x16, 8);


	// we multiply each channel with the corresponding multiplication factors (int16_t * int16_t = int32_t), and we normalize the result by 1024

	__m128i result0_low_A_s_32x4;
	__m128i result0_low_B_s_32x4;
	__m128i result0_high_A_s_32x4;
	__m128i result0_high_B_s_32x4;

	SSE::multiplyInt8x16ToInt32x8(channel0_low_u_16x8, factorChannel00_1024_s_16x8, result0_low_A_s_32x4, result0_low_B_s_32x4);
	SSE::multiplyInt8x16ToInt32x8(channel0_high_u_16x8, factorChannel00_1024_s_16x8, result0_high_A_s_32x4, result0_high_B_s_32x4);

	SSE::multiplyInt8x16ToInt32x8AndAccumulate(channel1_low_u_16x8, factorChannel01_1024_s_16x8, result0_low_A_s_32x4, result0_low_B_s_32x4);
	SSE::multiplyInt8x16ToInt32x8AndAccumulate(channel1_high_u_16x8, factorChannel01_1024_s_16x8, result0_high_A_s_32x4, result0_high_B_s_32x4);

	SSE::multiplyInt8x16ToInt32x8AndAccumulate(channel2_low_u_16x8, factorChannel02_1024_s_16x8, result0_low_A_s_32x4, result0_low_B_s_32x4);
	SSE::multiplyInt8x16ToInt32x8AndAccumulate(channel2_high_u_16x8, factorChannel02_1024_s_16x8, result0_high_A_s_32x4, result0_high_B_s_32x4);

	result0_low_A_s_32x4 = SSE::divideByRightShiftSigned32Bit(_mm_add_epi32(result0_low_A_s_32x4, biasChannel0_1024_s_32x4), 10);
	result0_low_B_s_32x4 = SSE::divideByRightShiftSigned32Bit(_mm_add_epi32(result0_low_B_s_32x4, biasChannel0_1024_s_32x4), 10);
	result0_high_A_s_32x4 = SSE::divideByRightShiftSigned32Bit(_mm_add_epi32(result0_high_A_s_32x4, biasChannel0_1024_s_32x4), 10);
	result0_high_B_s_32x4 = SSE::divideByRightShiftSigned32Bit(_mm_add_epi32(result0_high_B_s_32x4, biasChannel0_1024_s_32x4), 10);


	__m128i result1_low_A_s_32x4;
	__m128i result1_low_B_s_32x4;
	__m128i result1_high_A_s_32x4;
	__m128i result1_high_B_s_32x4;

	SSE::multiplyInt8x16ToInt32x8(channel0_low_u_16x8, factorChannel10_1024_s_16x8, result1_low_A_s_32x4, result1_low_B_s_32x4);
	SSE::multiplyInt8x16ToInt32x8(channel0_high_u_16x8, factorChannel10_1024_s_16x8, result1_high_A_s_32x4, result1_high_B_s_32x4);

	SSE::multiplyInt8x16ToInt32x8AndAccumulate(channel1_low_u_16x8, factorChannel11_1024_s_16x8, result1_low_A_s_32x4, result1_low_B_s_32x4);
	SSE::multiplyInt8x16ToInt32x8AndAccumulate(channel1_high_u_16x8, factorChannel11_1024_s_16x8, result1_high_A_s_32x4, result1_high_B_s_32x4);

	SSE::multiplyInt8x16ToInt32x8AndAccumulate(channel2_low_u_16x8, factorChannel12_1024_s_16x8, result1_low_A_s_32x4, result1_low_B_s_32x4);
	SSE::multiplyInt8x16ToInt32x8AndAccumulate(channel2_high_u_16x8, factorChannel12_1024_s_16x8, result1_high_A_s_32x4, result1_high_B_s_32x4);

	result1_low_A_s_32x4 = SSE::divideByRightShiftSigned32Bit(_mm_add_epi32(result1_low_A_s_32x4, biasChannel1_1024_s_32x4), 10);
	result1_low_B_s_32x4 = SSE::divideByRightShiftSigned32Bit(_mm_add_epi32(result1_low_B_s_32x4, biasChannel1_1024_s_32x4), 10);
	result1_high_A_s_32x4 = SSE::divideByRightShiftSigned32Bit(_mm_add_epi32(result1_high_A_s_32x4, biasChannel1_1024_s_32x4), 10);
	result1_high_B_s_32x4 = SSE::divideByRightShiftSigned32Bit(_mm_add_epi32(result1_high_B_s_32x4, biasChannel1_1024_s_32x4), 10);


	__m128i result2_low_A_s_32x4;
	__m128i result2_low_B_s_32x4;
	__m128i result2_high_A_s_32x4;
	__m128i result2_high_B_s_32x4;

	SSE::multiplyInt8x16ToInt32x8(channel0_low_u_16x8, factorChannel20_1024_s_16x8, result2_low_A_s_32x4, result2_low_B_s_32x4);
	SSE::multiplyInt8x16ToInt32x8(channel0_high_u_16x8, factorChannel20_1024_s_16x8, result2_high_A_s_32x4, result2_high_B_s_32x4);

	SSE::multiplyInt8x16ToInt32x8AndAccumulate(channel1_low_u_16x8, factorChannel21_1024_s_16x8, result2_low_A_s_32x4, result2_low_B_s_32x4);
	SSE::multiplyInt8x16ToInt32x8AndAccumulate(channel1_high_u_16x8, factorChannel21_1024_s_16x8, result2_high_A_s_32x4, result2_high_B_s_32x4);

	SSE::multiplyInt8x16ToInt32x8AndAccumulate(channel2_low_u_16x8, factorChannel22_1024_s_16x8, result2_low_A_s_32x4, result2_low_B_s_32x4);
	SSE::multiplyInt8x16ToInt32x8AndAccumulate(channel2_high_u_16x8, factorChannel22_1024_s_16x8, result2_high_A_s_32x4, result2_high_B_s_32x4);

	result2_low_A_s_32x4 = SSE::divideByRightShiftSigned32Bit(_mm_add_epi32(result2_low_A_s_32x4, biasChannel2_1024_s_32x4), 10);
	result2_low_B_s_32x4 = SSE::divideByRightShiftSigned32Bit(_mm_add_epi32(result2_low_B_s_32x4, biasChannel2_1024_s_32x4), 10);
	result2_high_A_s_32x4 = SSE::divideByRightShiftSigned32Bit(_mm_add_epi32(result2_high_A_s_32x4, biasChannel2_1024_s_32x4), 10);
	result2_high_B_s_32x4 = SSE::divideByRightShiftSigned32Bit(_mm_add_epi32(result2_high_B_s_32x4, biasChannel2_1024_s_32x4), 10);


	// now we have int32_t values with 0x0000 or 0xFFFF in the high 16 bits
	// thus we can merge 8 int32_t values to 8 int16_t values

	const __m128i mask_0000FFFF_32x4 = _mm_set1_epi32(0x0000FFFF);

	__m128i result0_A_s_16x8 = _mm_or_si128(_mm_and_si128(result0_low_A_s_32x4, mask_0000FFFF_32x4), _mm_slli_epi32(result0_high_A_s_32x4, 16));
	__m128i result0_B_s_16x8 = _mm_or_si128(_mm_and_si128(result0_low_B_s_32x4, mask_0000FFFF_32x4), _mm_slli_epi32(result0_high_B_s_32x4, 16));

	__m128i result1_A_s_16x8 = _mm_or_si128(_mm_and_si128(result1_low_A_s_32x4, mask_0000FFFF_32x4), _mm_slli_epi32(result1_high_A_s_32x4, 16));
	__m128i result1_B_s_16x8 = _mm_or_si128(_mm_and_si128(result1_low_B_s_32x4, mask_0000FFFF_32x4), _mm_slli_epi32(result1_high_B_s_32x4, 16));

	__m128i result2_A_s_16x8 = _mm_or_si128(_mm_and_si128(result2_low_A_s_32x4, mask_0000FFFF_32x4), _mm_slli_epi32(result2_high_A_s_32x4, 16));
	__m128i result2_B_s_16x8 = _mm_or_si128(_mm_and_si128(result2_low_B_s_32x4, mask_0000FFFF_32x4), _mm_slli_epi32(result2_high_B_s_32x4, 16));


	// we combine 16 int16_t values to 16 uint8_t values (saturated)

	const __m128i result0_u_8x16 = _mm_packus_epi16(result0_A_s_16x8, result0_B_s_16x8);
	const __m128i result1_u_8x16 = _mm_packus_epi16(result1_A_s_16x8, result1_B_s_16x8);
	const __m128i result2_u_8x16 = _mm_packus_epi16(result2_A_s_16x8, result2_B_s_16x8);

	__m128i resultA_u_8x16;
	__m128i resultB_u_8x16;
	__m128i resultC_u_8x16;
	SSE::interleave3Channel8Bit48Elements(result0_u_8x16, result1_u_8x16, result2_u_8x16, resultA_u_8x16, resultB_u_8x16, resultC_u_8x16);

	// and we can store the result
	_mm_storeu_si128((__m128i*)target + 0, resultA_u_8x16);
	_mm_storeu_si128((__m128i*)target + 1, resultB_u_8x16);
	_mm_storeu_si128((__m128i*)target + 2, resultC_u_8x16);
}

OCEAN_FORCE_INLINE void FrameChannels::convert4ChannelsTo1Channel16Pixels8BitPerChannel7BitPrecisionSSE(const uint8_t* const source, uint8_t* const target, const __m128i& multiplicationFactors0123_128_s_32x4)
{
	ocean_assert(source != nullptr && target != nullptr);

	// the documentation of this function is designed for RGBA32 to Y8 conversion
	// however, in general this function can be used to apply a linear combination on the four source channels
	// to create one output channel

	// precise color space conversion:
	// Y = 0.299 * R + 0.587 * G + 0.114 * B

	// approximation:
	// Y = (38 * R + 75 * G + 15 * B) / 128

	// we expect the following input pattern (for here RGBA32):
	// FEDC BA98 7654 3210
	// ABGR ABGR ABGR ABGR

	// we calculate:
	// (int16_t)((uint8_t)R * (signed char)38) + (int16_t)((uint8_t)G * (signed char)75) for the first 16 bits
	// (int16_t)((uint8_t)B * (signed char)15) + (int16_t)((uint8_t)A * (signed char)0)  for the second 16 bits

	// we store eight 16 bit values holding 64 for rounding purpose:
	// FE DC BA 98 76 54 32 10
	// 64 64 64 64 64 64 64 64
	const __m128i constant64_u_8x16 = _mm_set1_epi32(0x00400040);

	const __m128i pixelsA_u_8x16 = _mm_loadu_si128((const __m128i*)source + 0);
	const __m128i pixelsB_u_8x16 = _mm_loadu_si128((const __m128i*)source + 1);
	const __m128i pixelsC_u_8x16 = _mm_loadu_si128((const __m128i*)source + 2);
	const __m128i pixelsD_u_8x16 = _mm_loadu_si128((const __m128i*)source + 3);

	// we get the following pattern
	// FE DC BA 98 76 54 32 10
	// 0b gr 0b gr 0b gr 0b gr
	const __m128i intermediateResults0_u_16x8 = _mm_maddubs_epi16(pixelsA_u_8x16, multiplicationFactors0123_128_s_32x4);
	const __m128i intermediateResults1_u_16x8 = _mm_maddubs_epi16(pixelsB_u_8x16, multiplicationFactors0123_128_s_32x4);
	const __m128i intermediateResults2_u_16x8 = _mm_maddubs_epi16(pixelsC_u_8x16, multiplicationFactors0123_128_s_32x4);
	const __m128i intermediateResults3_u_16x8 = _mm_maddubs_epi16(pixelsD_u_8x16, multiplicationFactors0123_128_s_32x4);

	// now we sum the pairs of neighboring 16 bit intermediate results
	__m128i grayA_u_16x8 = _mm_hadd_epi16(intermediateResults0_u_16x8, intermediateResults1_u_16x8);
	__m128i grayB_u_16x8 = _mm_hadd_epi16(intermediateResults2_u_16x8, intermediateResults3_u_16x8);

	// we add 64 for rounding purpose
	grayA_u_16x8 = _mm_add_epi16(grayA_u_16x8, constant64_u_8x16);
	grayB_u_16x8 = _mm_add_epi16(grayB_u_16x8, constant64_u_8x16);

	// we shift the multiplication results by 7 bits (= 128)
	grayA_u_16x8 = _mm_srli_epi16(grayA_u_16x8, 7);
	grayB_u_16x8 = _mm_srli_epi16(grayB_u_16x8, 7);

	// now we have the following pattern (in two 128 bit registers):
	// FEDCBA9876543210
	// 0Y0Y0Y0Y0Y0Y0Y0Y

	// finally, we have to get rid of the upper zero bits by combining two 128 bit registers to one:
	const __m128i gray_u_8x16 = _mm_packus_epi16(grayA_u_16x8, grayB_u_16x8);

	// and we can store the result
	_mm_storeu_si128((__m128i*)target, gray_u_8x16);
}

void FrameChannels::convert4ChannelsTo2Channels16Pixels8BitPerChannel7BitPrecisionSSE(const uint8_t* const source, uint8_t* const target, const __m128i& multiplicationFactorsChannel0_0123_128_s_16x8, const __m128i& multiplicationFactorsChannel1_0123_128_s_16x8)
{
	ocean_assert(source != nullptr && target != nullptr);

	// the documentation of this function is designed for RGBA32 to YA16 conversion
	// however, in general this function can be used to apply a linear combination on the four source channels
	// to create one output channel

	// precise color space conversion:
	// Y = 0.299 * R + 0.587 * G + 0.114 * B + 0.0 * A
	// A =   0.0 * R +   0.0 * G +   0.0 * B + 1.0 * A

	// approximation:
	// Y = (38 * R + 75 * G + 15 * B + 0 * A) / 128
	// A = (128 * A) / 128

	// we expect the following input pattern (for here RGBA32):
	// FEDC BA98 7654 3210
	// ABGR ABGR ABGR ABGR

	// we store eight 16 bit values holding 64 for rounding purpose:
	// FE DC BA 98 76 54 32 10
	// 64 64 64 64 64 64 64 64
	const __m128i constant64_u_8x16 = _mm_set1_epi32(0x00400040);

	const __m128i pixelsA_u_8x16 = _mm_loadu_si128((const __m128i*)source + 0);
	const __m128i pixelsB_u_8x16 = _mm_loadu_si128((const __m128i*)source + 1);
	const __m128i pixelsC_u_8x16 = _mm_loadu_si128((const __m128i*)source + 2);
	const __m128i pixelsD_u_8x16 = _mm_loadu_si128((const __m128i*)source + 3);

	// we convert the 8 bit values to 16 bit values

	const __m128i pixelsA_u_16x8 = _mm_unpacklo_epi8(pixelsA_u_8x16, _mm_setzero_si128());
	const __m128i pixelsB_u_16x8 = _mm_unpackhi_epi8(pixelsA_u_8x16, _mm_setzero_si128());

	const __m128i pixelsC_u_16x8 = _mm_unpacklo_epi8(pixelsB_u_8x16, _mm_setzero_si128());
	const __m128i pixelsD_u_16x8 = _mm_unpackhi_epi8(pixelsB_u_8x16, _mm_setzero_si128());

	const __m128i pixelsE_u_16x8 = _mm_unpacklo_epi8(pixelsC_u_8x16, _mm_setzero_si128());
	const __m128i pixelsF_u_16x8 = _mm_unpackhi_epi8(pixelsC_u_8x16, _mm_setzero_si128());

	const __m128i pixelsG_u_16x8 = _mm_unpacklo_epi8(pixelsD_u_8x16, _mm_setzero_si128());
	const __m128i pixelsH_u_16x8 = _mm_unpackhi_epi8(pixelsD_u_8x16, _mm_setzero_si128());

	// now we have the following pattern
	// FE DC BA 98 76 54 32 10
	// 0a 0b 0g 0r 0a 0b 0g 0r

	const __m128i intermediateResultsChannel0_0_u_32x4 = _mm_madd_epi16(pixelsA_u_16x8, multiplicationFactorsChannel0_0123_128_s_16x8); // r * f00 + g * f01 | b * f02 + a * f03 | ...
	const __m128i intermediateResultsChannel0_1_u_32x4 = _mm_madd_epi16(pixelsB_u_16x8, multiplicationFactorsChannel0_0123_128_s_16x8);
	const __m128i intermediateResultsChannel0_2_u_32x4 = _mm_madd_epi16(pixelsC_u_16x8, multiplicationFactorsChannel0_0123_128_s_16x8);
	const __m128i intermediateResultsChannel0_3_u_32x4 = _mm_madd_epi16(pixelsD_u_16x8, multiplicationFactorsChannel0_0123_128_s_16x8);
	const __m128i intermediateResultsChannel0_4_u_32x4 = _mm_madd_epi16(pixelsE_u_16x8, multiplicationFactorsChannel0_0123_128_s_16x8);
	const __m128i intermediateResultsChannel0_5_u_32x4 = _mm_madd_epi16(pixelsF_u_16x8, multiplicationFactorsChannel0_0123_128_s_16x8);
	const __m128i intermediateResultsChannel0_6_u_32x4 = _mm_madd_epi16(pixelsG_u_16x8, multiplicationFactorsChannel0_0123_128_s_16x8);
	const __m128i intermediateResultsChannel0_7_u_32x4 = _mm_madd_epi16(pixelsH_u_16x8, multiplicationFactorsChannel0_0123_128_s_16x8);

	const __m128i resultsChannel0_A_u_32x4 = _mm_hadd_epi32(intermediateResultsChannel0_0_u_32x4, intermediateResultsChannel0_1_u_32x4); // r * f00 + g * f01 + b * f02 + a * f03 | ...
	const __m128i resultsChannel0_B_u_32x4 = _mm_hadd_epi32(intermediateResultsChannel0_2_u_32x4, intermediateResultsChannel0_3_u_32x4);
	const __m128i resultsChannel0_C_u_32x4 = _mm_hadd_epi32(intermediateResultsChannel0_4_u_32x4, intermediateResultsChannel0_5_u_32x4);
	const __m128i resultsChannel0_D_u_32x4 = _mm_hadd_epi32(intermediateResultsChannel0_6_u_32x4, intermediateResultsChannel0_7_u_32x4);


	const __m128i intermediateResultsChannel1_0_u_32x4 = _mm_madd_epi16(pixelsA_u_16x8, multiplicationFactorsChannel1_0123_128_s_16x8); // r * f10 + g * f11 | b * f12 + a * f13 | ...
	const __m128i intermediateResultsChannel1_1_u_32x4 = _mm_madd_epi16(pixelsB_u_16x8, multiplicationFactorsChannel1_0123_128_s_16x8);
	const __m128i intermediateResultsChannel1_2_u_32x4 = _mm_madd_epi16(pixelsC_u_16x8, multiplicationFactorsChannel1_0123_128_s_16x8);
	const __m128i intermediateResultsChannel1_3_u_32x4 = _mm_madd_epi16(pixelsD_u_16x8, multiplicationFactorsChannel1_0123_128_s_16x8);
	const __m128i intermediateResultsChannel1_4_u_32x4 = _mm_madd_epi16(pixelsE_u_16x8, multiplicationFactorsChannel1_0123_128_s_16x8);
	const __m128i intermediateResultsChannel1_5_u_32x4 = _mm_madd_epi16(pixelsF_u_16x8, multiplicationFactorsChannel1_0123_128_s_16x8);
	const __m128i intermediateResultsChannel1_6_u_32x4 = _mm_madd_epi16(pixelsG_u_16x8, multiplicationFactorsChannel1_0123_128_s_16x8);
	const __m128i intermediateResultsChannel1_7_u_32x4 = _mm_madd_epi16(pixelsH_u_16x8, multiplicationFactorsChannel1_0123_128_s_16x8);

	const __m128i resultsChannel1_A_u_32x4 = _mm_hadd_epi32(intermediateResultsChannel1_0_u_32x4, intermediateResultsChannel1_1_u_32x4); // r * f10 + g * f11 + b * f12 + a * f13 | ...
	const __m128i resultsChannel1_B_u_32x4 = _mm_hadd_epi32(intermediateResultsChannel1_2_u_32x4, intermediateResultsChannel1_3_u_32x4);
	const __m128i resultsChannel1_C_u_32x4 = _mm_hadd_epi32(intermediateResultsChannel1_4_u_32x4, intermediateResultsChannel1_5_u_32x4);
	const __m128i resultsChannel1_D_u_32x4 = _mm_hadd_epi32(intermediateResultsChannel1_6_u_32x4, intermediateResultsChannel1_7_u_32x4);

	// now we interleave the results of first and second channel (as both results fit into 16 bit)

	__m128i resultA_u_16x8 = _mm_or_si128(resultsChannel0_A_u_32x4, _mm_slli_epi32(resultsChannel1_A_u_32x4, 16));
	__m128i resultB_u_16x8 = _mm_or_si128(resultsChannel0_B_u_32x4, _mm_slli_epi32(resultsChannel1_B_u_32x4, 16));
	__m128i resultC_u_16x8 = _mm_or_si128(resultsChannel0_C_u_32x4, _mm_slli_epi32(resultsChannel1_C_u_32x4, 16));
	__m128i resultD_u_16x8 = _mm_or_si128(resultsChannel0_D_u_32x4, _mm_slli_epi32(resultsChannel1_D_u_32x4, 16));

	// we add 64 for rounding purpose
	resultA_u_16x8 = _mm_add_epi16(resultA_u_16x8, constant64_u_8x16);
	resultB_u_16x8 = _mm_add_epi16(resultB_u_16x8, constant64_u_8x16);
	resultC_u_16x8 = _mm_add_epi16(resultC_u_16x8, constant64_u_8x16);
	resultD_u_16x8 = _mm_add_epi16(resultD_u_16x8, constant64_u_8x16);

	// we shift the multiplication results by 7 bits (= 128)
	resultA_u_16x8 = _mm_srli_epi16(resultA_u_16x8, 7);
	resultB_u_16x8 = _mm_srli_epi16(resultB_u_16x8, 7);
	resultC_u_16x8 = _mm_srli_epi16(resultC_u_16x8, 7);
	resultD_u_16x8 = _mm_srli_epi16(resultD_u_16x8, 7);

	// now we have the following pattern (in two 128 bit registers):
	// FEDCBA9876543210
	// 0A0Y0A0Y0A0Y0A0Y

	// finally, we have to get rid of the upper zero bits by combining two 128 bit registers to one:
	const __m128i resultAB_u_8x16 = _mm_packus_epi16(resultA_u_16x8, resultB_u_16x8);
	const __m128i resultCD_u_8x16 = _mm_packus_epi16(resultC_u_16x8, resultD_u_16x8);

	// and we can store the result
	_mm_storeu_si128((__m128i*)target + 0, resultAB_u_8x16);
	_mm_storeu_si128((__m128i*)target + 1, resultCD_u_8x16);
}

#endif // OCEAN_HARDWARE_SSE_VERSION

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

template <bool tUseFactorChannel0, bool tUseFactorChannel1, bool tUseFactorChannel2>
void FrameChannels::convert3ChannelsTo1Channel8Pixels8BitPerChannel7BitPrecisionNEON(const uint8_t* const source, uint8_t* const target, const uint8x8_t& factorChannel0_128_u_8x8, const uint8x8_t& factorChannel1_128_u_8x8, const uint8x8_t& factorChannel2_128_u_8x8)
{
	static_assert(tUseFactorChannel0 || tUseFactorChannel1 || tUseFactorChannel2, "Invalid multiplication factors!");

	ocean_assert(source != nullptr && target != nullptr);

	// the documentation of this function designed for RGB24 to Y8 conversion

	// precise color space conversion:
	// Y = 0.299 * R + 0.587 * G + 0.114 * B

	// approximation:
	// Y = (38 * R + 75 * G + 15 * B) / 128

	// we expect the following input pattern (for here RGB24):
	// FEDC BA98 7654 3210
	// RBGR BGRB GRBG RBGR

	// we load 8 pixels (= 3 * 8 values) and directly deinterleave the 3 channels so that we receive the following patterns:
	// source_u_8x8x3.val[0]: R R R R R R R R
	// source_u_8x8x3.val[1]: G G G G G G G G
	// source_u_8x8x3.val[2]: B B B B B B B B

	uint8x8x3_t source_u_8x8x3 = vld3_u8(source);

	uint16x8_t intermediateResults_u_16x8;

	// we multiply the first channel with the specified factor (unless zero)

	if constexpr (tUseFactorChannel0)
	{
		intermediateResults_u_16x8 = vmull_u8(source_u_8x8x3.val[0], factorChannel0_128_u_8x8);
	}
	else
	{
		intermediateResults_u_16x8 = vdupq_n_u16(0u);
	}

	// we multiply the second channel with the specified factor (unless zero) and accumulate the results

	if constexpr (tUseFactorChannel1)
	{
		intermediateResults_u_16x8 = vmlal_u8(intermediateResults_u_16x8, source_u_8x8x3.val[1], factorChannel1_128_u_8x8);
	}

	// we multiply the third channel with the specified factor (unless zero) and accumulate the results

	if constexpr (tUseFactorChannel2)
	{
		intermediateResults_u_16x8 = vmlal_u8(intermediateResults_u_16x8, source_u_8x8x3.val[2], factorChannel2_128_u_8x8);
	}

	// we shift the 16 bit values by 7 bits (= 128), apply rounding, and narrow the 16 bit integers to 8 bit integers within one operation
	uint8x8_t results_u_8x8 = vqrshrn_n_u16(intermediateResults_u_16x8, 7); // results_u_8x8 = (intermediateResults_u_16x8 + 2^6) >> 2^7

	// and we can store the result
	vst1_u8(target, results_u_8x8);
}

OCEAN_FORCE_INLINE void FrameChannels::convert3ChannelsTo3Channels8Pixels8BitPerChannel6BitPrecisionNEON(const uint8_t* const source, uint8_t* const target, const int16x8_t& factorChannel00_64_s_16x8, const int16x8_t& factorChannel10_64_s_16x8, const int16x8_t& factorChannel20_64_s_16x8, const int16x8_t& factorChannel01_64_s_16x8, const int16x8_t& factorChannel11_64_s_16x8, const int16x8_t& factorChannel21_64_s_16x8, const int16x8_t& factorChannel02_64_s_16x8, const int16x8_t& factorChannel12_64_s_16x8, const int16x8_t& factorChannel22_64_s_16x8, const uint8x8_t& biasChannel0_u_8x8, const uint8x8_t& biasChannel1_u_8x8, const uint8x8_t& biasChannel2_u_8x8)
{
	ocean_assert(source != nullptr && target != nullptr);

	// the documentation of this function designed for YUV24 to RGB24 conversion

	// precise color space conversion:
	// | R |   |  1    0.0          1.370705   -175.45024  |   | Y |
	// | G | = |  1   -0.3376335   -0.698001    132.561152 | * | U |
	// | B |   |  1    1.732446     0.0        -221.753088 |   | V |
	//                                                         | 1 |

	// approximation:
	// R = 64 * Y +   0 * (U - 128) + 88 * (V - 128)
	// G = 64 * Y -  22 * (U - 128) - 45 * (V - 128)
	// B = 64 * Y + 111 * (U - 128) +  0 * (V - 128)

	// we load 8 pixels (= 3 * 8 values) and directly deinterleave the 3 channels so that we receive the following patterns:
	// source_u_8x8x3.val[0]: R R R R R R R R
	// source_u_8x8x3.val[1]: G G G G G G G G
	// source_u_8x8x3.val[2]: B B B B B B B B

	const uint8x8x3_t source_u_8x8x3 = vld3_u8(source);

	// Y' = Y - bias0, U' = U - bias1, V' = V - bias2
	const int16x8_t source0_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(source_u_8x8x3.val[0], biasChannel0_u_8x8));
	const int16x8_t source1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(source_u_8x8x3.val[1], biasChannel1_u_8x8));
	const int16x8_t source2_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(source_u_8x8x3.val[2], biasChannel2_u_8x8));

	// now we apply the 3x3 matrix multiplication

	int16x8_t intermediateResults0_s_16x8 = vmulq_s16(source0_s_16x8, factorChannel00_64_s_16x8);
	int16x8_t intermediateResults1_s_16x8 = vmulq_s16(source0_s_16x8, factorChannel10_64_s_16x8);
	int16x8_t intermediateResults2_s_16x8 = vmulq_s16(source0_s_16x8, factorChannel20_64_s_16x8);

	intermediateResults0_s_16x8 = vqaddq_s16(intermediateResults0_s_16x8, vmulq_s16(source1_s_16x8, factorChannel01_64_s_16x8)); // intermediateResults0 = saturated(intermediateResults0 + source10_low * factorChannel01)
	intermediateResults1_s_16x8 = vqaddq_s16(intermediateResults1_s_16x8, vmulq_s16(source1_s_16x8, factorChannel11_64_s_16x8));
	intermediateResults2_s_16x8 = vqaddq_s16(intermediateResults2_s_16x8, vmulq_s16(source1_s_16x8, factorChannel21_64_s_16x8));

	intermediateResults0_s_16x8 = vqaddq_s16(intermediateResults0_s_16x8, vmulq_s16(source2_s_16x8, factorChannel02_64_s_16x8));
	intermediateResults1_s_16x8 = vqaddq_s16(intermediateResults1_s_16x8, vmulq_s16(source2_s_16x8, factorChannel12_64_s_16x8));
	intermediateResults2_s_16x8 = vqaddq_s16(intermediateResults2_s_16x8, vmulq_s16(source2_s_16x8, factorChannel22_64_s_16x8));

	uint8x8x3_t results_u_8x8x3;

	// saturated narrow signed to unsigned, normalized by 2^6
	results_u_8x8x3.val[0] = vqrshrun_n_s16(intermediateResults0_s_16x8, 6);
	results_u_8x8x3.val[1] = vqrshrun_n_s16(intermediateResults1_s_16x8, 6);
	results_u_8x8x3.val[2] = vqrshrun_n_s16(intermediateResults2_s_16x8, 6);

	// and we can store the result
	vst3_u8(target, results_u_8x8x3);
}

OCEAN_FORCE_INLINE void FrameChannels::convert3ChannelsTo3Channels16Pixels8BitPerChannel6BitPrecisionNEON(const uint8_t* const source, uint8_t* const target, const int16x8_t& factorChannel00_64_s_16x8, const int16x8_t& factorChannel10_64_s_16x8, const int16x8_t& factorChannel20_64_s_16x8, const int16x8_t& factorChannel01_64_s_16x8, const int16x8_t& factorChannel11_64_s_16x8, const int16x8_t& factorChannel21_64_s_16x8, const int16x8_t& factorChannel02_64_s_16x8, const int16x8_t& factorChannel12_64_s_16x8, const int16x8_t& factorChannel22_64_s_16x8, const uint8x8_t& biasChannel0_u_8x8, const uint8x8_t& biasChannel1_u_8x8, const uint8x8_t& biasChannel2_u_8x8)
{
	ocean_assert(source != nullptr && target != nullptr);

	// the documentation of this function designed for YUV24 to RGB24 conversion

	// precise color space conversion:
	// | R |   |  1    0.0          1.370705   -175.45024  |   | Y |
	// | G | = |  1   -0.3376335   -0.698001    132.561152 | * | U |
	// | B |   |  1    1.732446     0.0        -221.753088 |   | V |
	//                                                         | 1 |

	// approximation:
	// R = 64 * Y +   0 * (U - 128) + 88 * (V - 128)
	// G = 64 * Y -  22 * (U - 128) - 45 * (V - 128)
	// B = 64 * Y + 111 * (U - 128) +  0 * (V - 128)

	const uint8x16x3_t source_u_8x16x3 = vld3q_u8(source);

	// Y' = Y - bias0, U' = U - bias1, V' = V - bias2
	const int16x8_t source0_low_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(source_u_8x16x3.val[0]), biasChannel0_u_8x8));
	const int16x8_t source1_low_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(source_u_8x16x3.val[1]), biasChannel1_u_8x8));
	const int16x8_t source2_low_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(source_u_8x16x3.val[2]), biasChannel2_u_8x8));

	const int16x8_t source0_high_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(source_u_8x16x3.val[0]), biasChannel0_u_8x8));
	const int16x8_t source1_high_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(source_u_8x16x3.val[1]), biasChannel1_u_8x8));
	const int16x8_t source2_high_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(source_u_8x16x3.val[2]), biasChannel2_u_8x8));

	// now we mulitply apply the 3x3 matrix multiplication

	int16x8_t intermediateResults0_low_s_16x8 = vmulq_s16(source0_low_s_16x8, factorChannel00_64_s_16x8);
	int16x8_t intermediateResults1_low_s_16x8 = vmulq_s16(source0_low_s_16x8, factorChannel10_64_s_16x8);
	int16x8_t intermediateResults2_low_s_16x8 = vmulq_s16(source0_low_s_16x8, factorChannel20_64_s_16x8);

	int16x8_t intermediateResults0_high_s_16x8 = vmulq_s16(source0_high_s_16x8, factorChannel00_64_s_16x8);
	int16x8_t intermediateResults1_high_s_16x8 = vmulq_s16(source0_high_s_16x8, factorChannel10_64_s_16x8);
	int16x8_t intermediateResults2_high_s_16x8 = vmulq_s16(source0_high_s_16x8, factorChannel20_64_s_16x8);

	intermediateResults0_low_s_16x8 = vqaddq_s16(intermediateResults0_low_s_16x8, vmulq_s16(source1_low_s_16x8, factorChannel01_64_s_16x8)); // intermediateResults0 = saturated(intermediateResults0 + source10_low * factorChannel01)
	intermediateResults1_low_s_16x8 = vqaddq_s16(intermediateResults1_low_s_16x8, vmulq_s16(source1_low_s_16x8, factorChannel11_64_s_16x8));
	intermediateResults2_low_s_16x8 = vqaddq_s16(intermediateResults2_low_s_16x8, vmulq_s16(source1_low_s_16x8, factorChannel21_64_s_16x8));

	intermediateResults0_high_s_16x8 = vqaddq_s16(intermediateResults0_high_s_16x8, vmulq_s16(source1_high_s_16x8, factorChannel01_64_s_16x8));
	intermediateResults1_high_s_16x8 = vqaddq_s16(intermediateResults1_high_s_16x8, vmulq_s16(source1_high_s_16x8, factorChannel11_64_s_16x8));
	intermediateResults2_high_s_16x8 = vqaddq_s16(intermediateResults2_high_s_16x8, vmulq_s16(source1_high_s_16x8, factorChannel21_64_s_16x8));

	intermediateResults0_low_s_16x8 = vqaddq_s16(intermediateResults0_low_s_16x8, vmulq_s16(source2_low_s_16x8, factorChannel02_64_s_16x8));
	intermediateResults1_low_s_16x8 = vqaddq_s16(intermediateResults1_low_s_16x8, vmulq_s16(source2_low_s_16x8, factorChannel12_64_s_16x8));
	intermediateResults2_low_s_16x8 = vqaddq_s16(intermediateResults2_low_s_16x8, vmulq_s16(source2_low_s_16x8, factorChannel22_64_s_16x8));

	intermediateResults0_high_s_16x8 = vqaddq_s16(intermediateResults0_high_s_16x8, vmulq_s16(source2_high_s_16x8, factorChannel02_64_s_16x8));
	intermediateResults1_high_s_16x8 = vqaddq_s16(intermediateResults1_high_s_16x8, vmulq_s16(source2_high_s_16x8, factorChannel12_64_s_16x8));
	intermediateResults2_high_s_16x8 = vqaddq_s16(intermediateResults2_high_s_16x8, vmulq_s16(source2_high_s_16x8, factorChannel22_64_s_16x8));

	uint8x16x3_t results_u_8x16x3;

	// saturated narrow signed to unsigned, normalized by 2^6
	results_u_8x16x3.val[0] = vcombine_u8(vqrshrun_n_s16(intermediateResults0_low_s_16x8, 6), vqrshrun_n_s16(intermediateResults0_high_s_16x8, 6));
	results_u_8x16x3.val[1] = vcombine_u8(vqrshrun_n_s16(intermediateResults1_low_s_16x8, 6), vqrshrun_n_s16(intermediateResults1_high_s_16x8, 6));
	results_u_8x16x3.val[2] = vcombine_u8(vqrshrun_n_s16(intermediateResults2_low_s_16x8, 6), vqrshrun_n_s16(intermediateResults2_high_s_16x8, 6));

	// and we can store the result
	vst3q_u8(target, results_u_8x16x3);
}

OCEAN_FORCE_INLINE void FrameChannels::convert3ChannelsTo3Channels8Pixels8BitPerChannel7BitPrecisionNEON(const uint8_t* const source, uint8_t* const target, const int16x8_t& factorChannel00_128_s_16x8, const int16x8_t& factorChannel10_128_s_16x8, const int16x8_t& factorChannel20_128_s_16x8, const int16x8_t& factorChannel01_128_s_16x8, const int16x8_t& factorChannel11_128_s_16x8, const int16x8_t& factorChannel21_128_s_16x8, const int16x8_t& factorChannel02_128_s_16x8, const int16x8_t& factorChannel12_128_s_16x8, const int16x8_t& factorChannel22_128_s_16x8, const int16x8_t& biasChannel0_128_s_16x8, const int16x8_t& biasChannel1_128_s_16x8, const int16x8_t& biasChannel2_128_s_16x8)
{
	ocean_assert(source != nullptr && target != nullptr);

	// the documentation of this function designed for RGB24 to YUV24 conversion

	// precise color space conversion:
	// | Y |   |  0.2578125   0.5039063   0.09765625  16.0  |   | R |
	// | U | = | -0.1484375  -0.2890625   0.4375      128.0 | * | G |
	// | V |   |  0.4375     -0.3671875  -0.0703125   128.0 |   | B |
	//                                                          | 1 |

	// approximation:
	// Y = ( 33 * R + 64 * G + 13 * B) / 128 + 16
	// U = (-19 * R - 37 * G + 56 * B) / 128 + 128
	// V = ( 56 * R - 47 * G -  9 * B) / 128 + 128

	// we load 8 pixels (= 3 * 8 values) and directly deinterleave the 3 channels so that we receive the following patterns:
	// source_u_8x8x3.val[0]: R R R R R R R R
	// source_u_8x8x3.val[1]: G G G G G G G G
	// source_u_8x8x3.val[2]: B B B B B B B B

	const uint8x8x3_t source_u_8x8x3 = vld3_u8(source);

	const int16x8_t source0_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(source_u_8x8x3.val[0]));
	const int16x8_t source1_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(source_u_8x8x3.val[1]));
	const int16x8_t source2_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(source_u_8x8x3.val[2]));

	int16x8_t intermediateResults0_s_16x8 = vmulq_s16(source0_s_16x8, factorChannel00_128_s_16x8);
	int16x8_t intermediateResults1_s_16x8 = vmulq_s16(source0_s_16x8, factorChannel10_128_s_16x8);
	int16x8_t intermediateResults2_s_16x8 = vmulq_s16(source0_s_16x8, factorChannel20_128_s_16x8);

	intermediateResults0_s_16x8 = vmlaq_s16(intermediateResults0_s_16x8, source1_s_16x8, factorChannel01_128_s_16x8);
	intermediateResults1_s_16x8 = vmlaq_s16(intermediateResults1_s_16x8, source1_s_16x8, factorChannel11_128_s_16x8);
	intermediateResults2_s_16x8 = vmlaq_s16(intermediateResults2_s_16x8, source1_s_16x8, factorChannel21_128_s_16x8);

	intermediateResults0_s_16x8 = vmlaq_s16(intermediateResults0_s_16x8, source2_s_16x8, factorChannel02_128_s_16x8);
	intermediateResults1_s_16x8 = vmlaq_s16(intermediateResults1_s_16x8, source2_s_16x8, factorChannel12_128_s_16x8);
	intermediateResults2_s_16x8 = vmlaq_s16(intermediateResults2_s_16x8, source2_s_16x8, factorChannel22_128_s_16x8);

	// now we add the bias values (saturated)

	intermediateResults0_s_16x8 = vqaddq_s16(intermediateResults0_s_16x8, biasChannel0_128_s_16x8);
	intermediateResults1_s_16x8 = vqaddq_s16(intermediateResults1_s_16x8, biasChannel1_128_s_16x8);
	intermediateResults2_s_16x8 = vqaddq_s16(intermediateResults2_s_16x8, biasChannel2_128_s_16x8);

	uint8x8x3_t results_u_8x8x3;

	// saturated narrow signed to unsigned
	results_u_8x8x3.val[0] = vqrshrun_n_s16(intermediateResults0_s_16x8, 7);
	results_u_8x8x3.val[1] = vqrshrun_n_s16(intermediateResults1_s_16x8, 7);
	results_u_8x8x3.val[2] = vqrshrun_n_s16(intermediateResults2_s_16x8, 7);

	// and we can store the result
	vst3_u8(target, results_u_8x8x3);
}

OCEAN_FORCE_INLINE void FrameChannels::convert3ChannelsTo3Channels8Pixels8BitPerChannel10BitPrecisionNEON(const uint8_t* const source, uint8_t* const target, const int16x4_t& factorChannel00_1024_s_16x4, const int16x4_t& factorChannel10_1024_s_16x4, const int16x4_t& factorChannel20_1024_s_16x4, const int16x4_t& factorChannel01_1024_s_16x4, const int16x4_t& factorChannel11_1024_s_16x4, const int16x4_t& factorChannel21_1024_s_16x4, const int16x4_t& factorChannel02_1024_s_16x4, const int16x4_t& factorChannel12_1024_s_16x4, const int16x4_t& factorChannel22_1024_s_16x4, const int32x4_t& biasChannel0_1024_s_32x4, const int32x4_t& biasChannel1_1024_s_32x4, const int32x4_t& biasChannel2_1024_s_32x4)
{
	ocean_assert(source != nullptr && target != nullptr);

	// the documentation of this function designed for YUV24 to RGB24 conversion

	// precise color space conversion:
	// | R |   | 1.1639404296875   0.0              1.595947265625  -222.904296875 |   | Y |
	// | G | = | 1.1639404296875  -0.3909912109375 -0.81298828125    135.486328125 | * | U |
	// | B |   | 1.1639404296875   2.0179443359375  0.0             -276.919921875 |   | V |
	//                                                                                 | 1 |

	// approximation:
	// | R |   | 1192    0       1634   -223 |   | Y |
	// | G | = | 1192   -400    -833     135 | * | U |
	// | B |   | 1192    2066    0      -277 |   | V |
	//                                           | 1 |

	// we load 8 pixels (= 3 * 8 values) and directly deinterleave the 3 channels so that we receive the following patterns:
	// source_u_8x8x3.val[0]: R R R R R R R R
	// source_u_8x8x3.val[1]: G G G G G G G G
	// source_u_8x8x3.val[2]: B B B B B B B B

	const uint8x8x3_t source_u_8x8x3 = vld3_u8(source);

	const int16x8_t source0_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(source_u_8x8x3.val[0]));
	const int16x8_t source1_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(source_u_8x8x3.val[1]));
	const int16x8_t source2_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(source_u_8x8x3.val[2]));

	const int16x4_t source0_low_s_16x4 = vget_low_s16(source0_s_16x8);
	const int16x4_t source0_high_s_16x4 = vget_high_s16(source0_s_16x8);

	int32x4_t intermediateResults0_low_s_32x4 = vmull_s16(source0_low_s_16x4, factorChannel00_1024_s_16x4);
	int32x4_t intermediateResults0_high_s_32x4 = vmull_s16(source0_high_s_16x4, factorChannel00_1024_s_16x4);

	int32x4_t intermediateResults1_low_s_32x4 = vmull_s16(source0_low_s_16x4, factorChannel10_1024_s_16x4);
	int32x4_t intermediateResults1_high_s_32x4 = vmull_s16(source0_high_s_16x4, factorChannel10_1024_s_16x4);

	int32x4_t intermediateResults2_low_s_32x4 = vmull_s16(source0_low_s_16x4, factorChannel20_1024_s_16x4);
	int32x4_t intermediateResults2_high_s_32x4 = vmull_s16(source0_high_s_16x4, factorChannel20_1024_s_16x4);


	const int16x4_t source1_low_s_16x4 = vget_low_s16(source1_s_16x8);
	const int16x4_t source1_high_s_16x4 = vget_high_s16(source1_s_16x8);

	intermediateResults0_low_s_32x4 = vmlal_s16(intermediateResults0_low_s_32x4, source1_low_s_16x4, factorChannel01_1024_s_16x4);
	intermediateResults0_high_s_32x4 = vmlal_s16(intermediateResults0_high_s_32x4, source1_high_s_16x4, factorChannel01_1024_s_16x4);

	intermediateResults1_low_s_32x4 = vmlal_s16(intermediateResults1_low_s_32x4, source1_low_s_16x4, factorChannel11_1024_s_16x4);
	intermediateResults1_high_s_32x4 = vmlal_s16(intermediateResults1_high_s_32x4, source1_high_s_16x4, factorChannel11_1024_s_16x4);

	intermediateResults2_low_s_32x4 = vmlal_s16(intermediateResults2_low_s_32x4, source1_low_s_16x4, factorChannel21_1024_s_16x4);
	intermediateResults2_high_s_32x4 = vmlal_s16(intermediateResults2_high_s_32x4, source1_high_s_16x4, factorChannel21_1024_s_16x4);


	const int16x4_t source2_low_s_16x4 = vget_low_s16(source2_s_16x8);
	const int16x4_t source2_high_s_16x4 = vget_high_s16(source2_s_16x8);

	intermediateResults0_low_s_32x4 = vmlal_s16(intermediateResults0_low_s_32x4, source2_low_s_16x4, factorChannel02_1024_s_16x4);
	intermediateResults0_high_s_32x4 = vmlal_s16(intermediateResults0_high_s_32x4, source2_high_s_16x4, factorChannel02_1024_s_16x4);

	intermediateResults1_low_s_32x4 = vmlal_s16(intermediateResults1_low_s_32x4, source2_low_s_16x4, factorChannel12_1024_s_16x4);
	intermediateResults1_high_s_32x4 = vmlal_s16(intermediateResults1_high_s_32x4, source2_high_s_16x4, factorChannel12_1024_s_16x4);

	intermediateResults2_low_s_32x4 = vmlal_s16(intermediateResults2_low_s_32x4, source2_low_s_16x4, factorChannel22_1024_s_16x4);
	intermediateResults2_high_s_32x4 = vmlal_s16(intermediateResults2_high_s_32x4, source2_high_s_16x4, factorChannel22_1024_s_16x4);


	// now we add the bias values (saturated)

	intermediateResults0_low_s_32x4 = vaddq_s32(intermediateResults0_low_s_32x4, biasChannel0_1024_s_32x4);
	intermediateResults0_high_s_32x4 = vaddq_s32(intermediateResults0_high_s_32x4, biasChannel0_1024_s_32x4);

	intermediateResults1_low_s_32x4 = vaddq_s32(intermediateResults1_low_s_32x4, biasChannel1_1024_s_32x4);
	intermediateResults1_high_s_32x4 = vaddq_s32(intermediateResults1_high_s_32x4, biasChannel1_1024_s_32x4);

	intermediateResults2_low_s_32x4 = vaddq_s32(intermediateResults2_low_s_32x4, biasChannel2_1024_s_32x4);
	intermediateResults2_high_s_32x4 = vaddq_s32(intermediateResults2_high_s_32x4, biasChannel2_1024_s_32x4);


	uint8x8x3_t results_u_8x8x3;

	// saturated narrow signed to unsigned
	results_u_8x8x3.val[0] = vqmovn_u16(vcombine_u16(vqrshrun_n_s32(intermediateResults0_low_s_32x4, 10), vqrshrun_n_s32(intermediateResults0_high_s_32x4, 10)));
	results_u_8x8x3.val[1] = vqmovn_u16(vcombine_u16(vqrshrun_n_s32(intermediateResults1_low_s_32x4, 10), vqrshrun_n_s32(intermediateResults1_high_s_32x4, 10)));
	results_u_8x8x3.val[2] = vqmovn_u16(vcombine_u16(vqrshrun_n_s32(intermediateResults2_low_s_32x4, 10), vqrshrun_n_s32(intermediateResults2_high_s_32x4, 10)));

	// and we can store the result
	vst3_u8(target, results_u_8x8x3);
}

OCEAN_FORCE_INLINE void FrameChannels::convert3ChannelsTo3Channels16Pixels8BitPerChannel10BitPrecisionNEON(const uint8_t* const source, uint8_t* const target, const int16x4_t& factorChannel00_1024_s_16x4, const int16x4_t& factorChannel10_1024_s_16x4, const int16x4_t& factorChannel20_1024_s_16x4, const int16x4_t& factorChannel01_1024_s_16x4, const int16x4_t& factorChannel11_1024_s_16x4, const int16x4_t& factorChannel21_1024_s_16x4, const int16x4_t& factorChannel02_1024_s_16x4, const int16x4_t& factorChannel12_1024_s_16x4, const int16x4_t& factorChannel22_1024_s_16x4, const int32x4_t& biasChannel0_1024_s_32x4, const int32x4_t& biasChannel1_1024_s_32x4, const int32x4_t& biasChannel2_1024_s_32x4)
{
	ocean_assert(source != nullptr && target != nullptr);

	// the documentation of this function designed for YUV24 to RGB24 conversion

	// precise color space conversion:
	// | R |   | 1.1639404296875   0.0              1.595947265625  -222.904296875 |   | Y |
	// | G | = | 1.1639404296875  -0.3909912109375 -0.81298828125    135.486328125 | * | U |
	// | B |   | 1.1639404296875   2.0179443359375  0.0             -276.919921875 |   | V |
	//                                                                                 | 1 |

	// approximation:
	// | R |   | 1192    0       1634   -223 |   | Y |
	// | G | = | 1192   -400    -833     135 | * | U |
	// | B |   | 1192    2066    0      -277 |   | V |
	//                                           | 1 |

	// we load 8 pixels (= 3 * 8 values) and directly deinterleave the 3 channels so that we receive the following patterns:
	// source_u_8x8x3.val[0]: R R R R R R R R
	// source_u_8x8x3.val[1]: G G G G G G G G
	// source_u_8x8x3.val[2]: B B B B B B B B

	const uint8x16x3_t source_u_8x16x3 = vld3q_u8(source);

	const int16x8_t source0_low_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(vget_low_u8(source_u_8x16x3.val[0])));
	const int16x8_t source1_low_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(vget_low_u8(source_u_8x16x3.val[1])));
	const int16x8_t source2_low_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(vget_low_u8(source_u_8x16x3.val[2])));

	const int16x8_t source0_high_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(vget_high_u8(source_u_8x16x3.val[0])));
	const int16x8_t source1_high_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(vget_high_u8(source_u_8x16x3.val[1])));
	const int16x8_t source2_high_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(vget_high_u8(source_u_8x16x3.val[2])));

	const int16x4_t source0_A_s_16x4 = vget_low_s16(source0_low_s_16x8);
	const int16x4_t source0_B_s_16x4 = vget_high_s16(source0_low_s_16x8);
	const int16x4_t source0_C_s_16x4 = vget_low_s16(source0_high_s_16x8);
	const int16x4_t source0_D_s_16x4 = vget_high_s16(source0_high_s_16x8);

	int32x4_t intermediateResults0_A_s_32x4 = vmull_s16(source0_A_s_16x4, factorChannel00_1024_s_16x4);
	int32x4_t intermediateResults0_B_s_32x4 = vmull_s16(source0_B_s_16x4, factorChannel00_1024_s_16x4);
	int32x4_t intermediateResults0_C_s_32x4 = vmull_s16(source0_C_s_16x4, factorChannel00_1024_s_16x4);
	int32x4_t intermediateResults0_D_s_32x4 = vmull_s16(source0_D_s_16x4, factorChannel00_1024_s_16x4);

	int32x4_t intermediateResults1_A_s_32x4 = vmull_s16(source0_A_s_16x4, factorChannel10_1024_s_16x4);
	int32x4_t intermediateResults1_B_s_32x4 = vmull_s16(source0_B_s_16x4, factorChannel10_1024_s_16x4);
	int32x4_t intermediateResults1_C_s_32x4 = vmull_s16(source0_C_s_16x4, factorChannel10_1024_s_16x4);
	int32x4_t intermediateResults1_D_s_32x4 = vmull_s16(source0_D_s_16x4, factorChannel10_1024_s_16x4);

	int32x4_t intermediateResults2_A_s_32x4 = vmull_s16(source0_A_s_16x4, factorChannel20_1024_s_16x4);
	int32x4_t intermediateResults2_B_s_32x4 = vmull_s16(source0_B_s_16x4, factorChannel20_1024_s_16x4);
	int32x4_t intermediateResults2_C_s_32x4 = vmull_s16(source0_C_s_16x4, factorChannel20_1024_s_16x4);
	int32x4_t intermediateResults2_D_s_32x4 = vmull_s16(source0_D_s_16x4, factorChannel20_1024_s_16x4);


	const int16x4_t source1_A_s_16x4 = vget_low_s16(source1_low_s_16x8);
	const int16x4_t source1_B_s_16x4 = vget_high_s16(source1_low_s_16x8);
	const int16x4_t source1_C_s_16x4 = vget_low_s16(source1_high_s_16x8);
	const int16x4_t source1_D_s_16x4 = vget_high_s16(source1_high_s_16x8);

	intermediateResults0_A_s_32x4 = vmlal_s16(intermediateResults0_A_s_32x4, source1_A_s_16x4, factorChannel01_1024_s_16x4);
	intermediateResults0_B_s_32x4 = vmlal_s16(intermediateResults0_B_s_32x4, source1_B_s_16x4, factorChannel01_1024_s_16x4);
	intermediateResults0_C_s_32x4 = vmlal_s16(intermediateResults0_C_s_32x4, source1_C_s_16x4, factorChannel01_1024_s_16x4);
	intermediateResults0_D_s_32x4 = vmlal_s16(intermediateResults0_D_s_32x4, source1_D_s_16x4, factorChannel01_1024_s_16x4);

	intermediateResults1_A_s_32x4 = vmlal_s16(intermediateResults1_A_s_32x4, source1_A_s_16x4, factorChannel11_1024_s_16x4);
	intermediateResults1_B_s_32x4 = vmlal_s16(intermediateResults1_B_s_32x4, source1_B_s_16x4, factorChannel11_1024_s_16x4);
	intermediateResults1_C_s_32x4 = vmlal_s16(intermediateResults1_C_s_32x4, source1_C_s_16x4, factorChannel11_1024_s_16x4);
	intermediateResults1_D_s_32x4 = vmlal_s16(intermediateResults1_D_s_32x4, source1_D_s_16x4, factorChannel11_1024_s_16x4);

	intermediateResults2_A_s_32x4 = vmlal_s16(intermediateResults2_A_s_32x4, source1_A_s_16x4, factorChannel21_1024_s_16x4);
	intermediateResults2_B_s_32x4 = vmlal_s16(intermediateResults2_B_s_32x4, source1_B_s_16x4, factorChannel21_1024_s_16x4);
	intermediateResults2_C_s_32x4 = vmlal_s16(intermediateResults2_C_s_32x4, source1_C_s_16x4, factorChannel21_1024_s_16x4);
	intermediateResults2_D_s_32x4 = vmlal_s16(intermediateResults2_D_s_32x4, source1_D_s_16x4, factorChannel21_1024_s_16x4);


	const int16x4_t source2_A_s_16x4 = vget_low_s16(source2_low_s_16x8);
	const int16x4_t source2_B_s_16x4 = vget_high_s16(source2_low_s_16x8);
	const int16x4_t source2_C_s_16x4 = vget_low_s16(source2_high_s_16x8);
	const int16x4_t source2_D_s_16x4 = vget_high_s16(source2_high_s_16x8);

	intermediateResults0_A_s_32x4 = vmlal_s16(intermediateResults0_A_s_32x4, source2_A_s_16x4, factorChannel02_1024_s_16x4);
	intermediateResults0_B_s_32x4 = vmlal_s16(intermediateResults0_B_s_32x4, source2_B_s_16x4, factorChannel02_1024_s_16x4);
	intermediateResults0_C_s_32x4 = vmlal_s16(intermediateResults0_C_s_32x4, source2_C_s_16x4, factorChannel02_1024_s_16x4);
	intermediateResults0_D_s_32x4 = vmlal_s16(intermediateResults0_D_s_32x4, source2_D_s_16x4, factorChannel02_1024_s_16x4);

	intermediateResults1_A_s_32x4 = vmlal_s16(intermediateResults1_A_s_32x4, source2_A_s_16x4, factorChannel12_1024_s_16x4);
	intermediateResults1_B_s_32x4 = vmlal_s16(intermediateResults1_B_s_32x4, source2_B_s_16x4, factorChannel12_1024_s_16x4);
	intermediateResults1_C_s_32x4 = vmlal_s16(intermediateResults1_C_s_32x4, source2_C_s_16x4, factorChannel12_1024_s_16x4);
	intermediateResults1_D_s_32x4 = vmlal_s16(intermediateResults1_D_s_32x4, source2_D_s_16x4, factorChannel12_1024_s_16x4);

	intermediateResults2_A_s_32x4 = vmlal_s16(intermediateResults2_A_s_32x4, source2_A_s_16x4, factorChannel22_1024_s_16x4);
	intermediateResults2_B_s_32x4 = vmlal_s16(intermediateResults2_B_s_32x4, source2_B_s_16x4, factorChannel22_1024_s_16x4);
	intermediateResults2_C_s_32x4 = vmlal_s16(intermediateResults2_C_s_32x4, source2_C_s_16x4, factorChannel22_1024_s_16x4);
	intermediateResults2_D_s_32x4 = vmlal_s16(intermediateResults2_D_s_32x4, source2_D_s_16x4, factorChannel22_1024_s_16x4);


	// now we add the bias values (saturated)

	intermediateResults0_A_s_32x4 = vaddq_s32(intermediateResults0_A_s_32x4, biasChannel0_1024_s_32x4);
	intermediateResults0_B_s_32x4 = vaddq_s32(intermediateResults0_B_s_32x4, biasChannel0_1024_s_32x4);
	intermediateResults0_C_s_32x4 = vaddq_s32(intermediateResults0_C_s_32x4, biasChannel0_1024_s_32x4);
	intermediateResults0_D_s_32x4 = vaddq_s32(intermediateResults0_D_s_32x4, biasChannel0_1024_s_32x4);

	intermediateResults1_A_s_32x4 = vaddq_s32(intermediateResults1_A_s_32x4, biasChannel1_1024_s_32x4);
	intermediateResults1_B_s_32x4 = vaddq_s32(intermediateResults1_B_s_32x4, biasChannel1_1024_s_32x4);
	intermediateResults1_C_s_32x4 = vaddq_s32(intermediateResults1_C_s_32x4, biasChannel1_1024_s_32x4);
	intermediateResults1_D_s_32x4 = vaddq_s32(intermediateResults1_D_s_32x4, biasChannel1_1024_s_32x4);

	intermediateResults2_A_s_32x4 = vaddq_s32(intermediateResults2_A_s_32x4, biasChannel2_1024_s_32x4);
	intermediateResults2_B_s_32x4 = vaddq_s32(intermediateResults2_B_s_32x4, biasChannel2_1024_s_32x4);
	intermediateResults2_C_s_32x4 = vaddq_s32(intermediateResults2_C_s_32x4, biasChannel2_1024_s_32x4);
	intermediateResults2_D_s_32x4 = vaddq_s32(intermediateResults2_D_s_32x4, biasChannel2_1024_s_32x4);


	uint8x16x3_t results_u_8x16x3;

	// saturated narrow signed to unsigned
	results_u_8x16x3.val[0] = vcombine_u8(vqmovn_u16(vcombine_u16(vqrshrun_n_s32(intermediateResults0_A_s_32x4, 10), vqrshrun_n_s32(intermediateResults0_B_s_32x4, 10))), vqmovn_u16(vcombine_u16(vqrshrun_n_s32(intermediateResults0_C_s_32x4, 10), vqrshrun_n_s32(intermediateResults0_D_s_32x4, 10))));

	results_u_8x16x3.val[1] = vcombine_u8(vqmovn_u16(vcombine_u16(vqrshrun_n_s32(intermediateResults1_A_s_32x4, 10), vqrshrun_n_s32(intermediateResults1_B_s_32x4, 10))), vqmovn_u16(vcombine_u16(vqrshrun_n_s32(intermediateResults1_C_s_32x4, 10), vqrshrun_n_s32(intermediateResults1_D_s_32x4, 10))));
	results_u_8x16x3.val[2] = vcombine_u8(vqmovn_u16(vcombine_u16(vqrshrun_n_s32(intermediateResults2_A_s_32x4, 10), vqrshrun_n_s32(intermediateResults2_B_s_32x4, 10))), vqmovn_u16(vcombine_u16(vqrshrun_n_s32(intermediateResults2_C_s_32x4, 10), vqrshrun_n_s32(intermediateResults2_D_s_32x4, 10))));

	// and we can store the result
	vst3q_u8(target, results_u_8x16x3);
}

OCEAN_FORCE_INLINE void FrameChannels::convert3ChannelsTo3Channels16Pixels8BitPerChannel7BitPrecisionNEON(const uint8_t* const source, uint8_t* const target, const int16x8_t& factorChannel00_128_s_16x8, const int16x8_t& factorChannel10_128_s_16x8, const int16x8_t& factorChannel20_128_s_16x8, const int16x8_t& factorChannel01_128_s_16x8, const int16x8_t& factorChannel11_128_s_16x8, const int16x8_t& factorChannel21_128_s_16x8, const int16x8_t& factorChannel02_128_s_16x8, const int16x8_t& factorChannel12_128_s_16x8, const int16x8_t& factorChannel22_128_s_16x8, const int16x8_t& biasChannel0_128_s_16x8, const int16x8_t& biasChannel1_128_s_16x8, const int16x8_t& biasChannel2_128_s_16x8)
{
	ocean_assert(source != nullptr && target != nullptr);

	// the documentation of this function designed for RGB24 to YUV24 conversion

	// precise color space conversion:
	// | Y |   |  0.2578125   0.5039063   0.09765625  16.0  |   | R |
	// | U | = | -0.1484375  -0.2890625   0.4375      128.0 | * | G |
	// | V |   |  0.4375     -0.3671875  -0.0703125   128.0 |   | B |
	//                                                          | 1 |

	// approximation:
	// Y = ( 33 * R + 64 * G + 13 * B) / 128 + 16
	// U = (-19 * R - 37 * G + 56 * B) / 128 + 128
	// V = ( 56 * R - 47 * G -  9 * B) / 128 + 128

	// we load 8 pixels (= 3 * 8 values) and directly deinterleave the 3 channels so that we receive the following patterns:
	// source_u_8x8x3.val[0]: R R R R R R R R
	// source_u_8x8x3.val[1]: G G G G G G G G
	// source_u_8x8x3.val[2]: B B B B B B B B

	const uint8x16x3_t source_u_8x16x3 = vld3q_u8(source);

	const int16x8_t source0_low_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(vget_low_u8(source_u_8x16x3.val[0])));
	const int16x8_t source1_low_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(vget_low_u8(source_u_8x16x3.val[1])));
	const int16x8_t source2_low_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(vget_low_u8(source_u_8x16x3.val[2])));

	const int16x8_t source0_high_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(vget_high_u8(source_u_8x16x3.val[0])));
	const int16x8_t source1_high_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(vget_high_u8(source_u_8x16x3.val[1])));
	const int16x8_t source2_high_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(vget_high_u8(source_u_8x16x3.val[2])));


	int16x8_t intermediateResults0_low_s_16x8 = vmulq_s16(source0_low_s_16x8, factorChannel00_128_s_16x8);
	int16x8_t intermediateResults1_low_s_16x8 = vmulq_s16(source0_low_s_16x8, factorChannel10_128_s_16x8);
	int16x8_t intermediateResults2_low_s_16x8 = vmulq_s16(source0_low_s_16x8, factorChannel20_128_s_16x8);

	int16x8_t intermediateResults0_high_s_16x8 = vmulq_s16(source0_high_s_16x8, factorChannel00_128_s_16x8);
	int16x8_t intermediateResults1_high_s_16x8 = vmulq_s16(source0_high_s_16x8, factorChannel10_128_s_16x8);
	int16x8_t intermediateResults2_high_s_16x8 = vmulq_s16(source0_high_s_16x8, factorChannel20_128_s_16x8);


	intermediateResults0_low_s_16x8 = vmlaq_s16(intermediateResults0_low_s_16x8, source1_low_s_16x8, factorChannel01_128_s_16x8);
	intermediateResults1_low_s_16x8 = vmlaq_s16(intermediateResults1_low_s_16x8, source1_low_s_16x8, factorChannel11_128_s_16x8);
	intermediateResults2_low_s_16x8 = vmlaq_s16(intermediateResults2_low_s_16x8, source1_low_s_16x8, factorChannel21_128_s_16x8);

	intermediateResults0_high_s_16x8 = vmlaq_s16(intermediateResults0_high_s_16x8, source1_high_s_16x8, factorChannel01_128_s_16x8);
	intermediateResults1_high_s_16x8 = vmlaq_s16(intermediateResults1_high_s_16x8, source1_high_s_16x8, factorChannel11_128_s_16x8);
	intermediateResults2_high_s_16x8 = vmlaq_s16(intermediateResults2_high_s_16x8, source1_high_s_16x8, factorChannel21_128_s_16x8);


	intermediateResults0_low_s_16x8 = vmlaq_s16(intermediateResults0_low_s_16x8, source2_low_s_16x8, factorChannel02_128_s_16x8);
	intermediateResults1_low_s_16x8 = vmlaq_s16(intermediateResults1_low_s_16x8, source2_low_s_16x8, factorChannel12_128_s_16x8);
	intermediateResults2_low_s_16x8 = vmlaq_s16(intermediateResults2_low_s_16x8, source2_low_s_16x8, factorChannel22_128_s_16x8);

	intermediateResults0_high_s_16x8 = vmlaq_s16(intermediateResults0_high_s_16x8, source2_high_s_16x8, factorChannel02_128_s_16x8);
	intermediateResults1_high_s_16x8 = vmlaq_s16(intermediateResults1_high_s_16x8, source2_high_s_16x8, factorChannel12_128_s_16x8);
	intermediateResults2_high_s_16x8 = vmlaq_s16(intermediateResults2_high_s_16x8, source2_high_s_16x8, factorChannel22_128_s_16x8);

	// now we add the bias values (saturated)

	intermediateResults0_low_s_16x8 = vqaddq_s16(intermediateResults0_low_s_16x8, biasChannel0_128_s_16x8);
	intermediateResults0_high_s_16x8 = vqaddq_s16(intermediateResults0_high_s_16x8, biasChannel0_128_s_16x8);

	intermediateResults1_low_s_16x8 = vqaddq_s16(intermediateResults1_low_s_16x8, biasChannel1_128_s_16x8);
	intermediateResults1_high_s_16x8 = vqaddq_s16(intermediateResults1_high_s_16x8, biasChannel1_128_s_16x8);

	intermediateResults2_low_s_16x8 = vqaddq_s16(intermediateResults2_low_s_16x8, biasChannel2_128_s_16x8);
	intermediateResults2_high_s_16x8 = vqaddq_s16(intermediateResults2_high_s_16x8, biasChannel2_128_s_16x8);


	uint8x16x3_t results_u_8x16x3;

	// saturated narrow signed to unsigned shift with rounding
	results_u_8x16x3.val[0] = vcombine_u8(vqrshrun_n_s16(intermediateResults0_low_s_16x8, 7), vqrshrun_n_s16(intermediateResults0_high_s_16x8, 7));
	results_u_8x16x3.val[1] = vcombine_u8(vqrshrun_n_s16(intermediateResults1_low_s_16x8, 7), vqrshrun_n_s16(intermediateResults1_high_s_16x8, 7));
	results_u_8x16x3.val[2] = vcombine_u8(vqrshrun_n_s16(intermediateResults2_low_s_16x8, 7), vqrshrun_n_s16(intermediateResults2_high_s_16x8, 7));

	// and we can store the result
	vst3q_u8(target, results_u_8x16x3);
}

OCEAN_FORCE_INLINE void FrameChannels::convert3ChannelsTo4Channels16Pixels8BitPerChannel6BitPrecisionNEON(const uint8_t* const source, uint8_t* const target, const int16x8_t& factorChannel00_64_s_16x8, const int16x8_t& factorChannel10_64_s_16x8, const int16x8_t& factorChannel20_64_s_16x8, const int16x8_t& factorChannel01_64_s_16x8, const int16x8_t& factorChannel11_64_s_16x8, const int16x8_t& factorChannel21_64_s_16x8, const int16x8_t& factorChannel02_64_s_16x8, const int16x8_t& factorChannel12_64_s_16x8, const int16x8_t& factorChannel22_64_s_16x8, const uint8x8_t& biasChannel0_u_8x8, const uint8x8_t& biasChannel1_u_8x8, const uint8x8_t& biasChannel2_u_8x8, const uint8x16_t& channelValue3_u_8x16)
{
	ocean_assert(source != nullptr && target != nullptr);

	// the documentation of this function designed for YUV24 to RGB24 conversion

	// precise color space conversion:
	// | R |   |  1    0.0          1.370705   -175.45024  |   | Y |
	// | G | = |  1   -0.3376335   -0.698001    132.561152 | * | U |
	// | B |   |  1    1.732446     0.0        -221.753088 |   | V |
	//                                                         | 1 |

	// approximation:
	// R = 64 * Y +   0 * (U - 128) + 88 * (V - 128)
	// G = 64 * Y -  22 * (U - 128) - 45 * (V - 128)
	// B = 64 * Y + 111 * (U - 128) +  0 * (V - 128)

	const uint8x16x3_t source_u_8x16x3 = vld3q_u8(source);

	// Y' = Y - bias0, U' = U - bias1, V' = V - bias2
	const int16x8_t source0_low_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(source_u_8x16x3.val[0]), biasChannel0_u_8x8));
	const int16x8_t source1_low_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(source_u_8x16x3.val[1]), biasChannel1_u_8x8));
	const int16x8_t source2_low_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(source_u_8x16x3.val[2]), biasChannel2_u_8x8));

	const int16x8_t source0_high_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(source_u_8x16x3.val[0]), biasChannel0_u_8x8));
	const int16x8_t source1_high_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(source_u_8x16x3.val[1]), biasChannel1_u_8x8));
	const int16x8_t source2_high_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(source_u_8x16x3.val[2]), biasChannel2_u_8x8));

	// now we mulitply apply the 3x3 matrix multiplication

	int16x8_t intermediateResults0_low_s_16x8 = vmulq_s16(source0_low_s_16x8, factorChannel00_64_s_16x8);
	int16x8_t intermediateResults1_low_s_16x8 = vmulq_s16(source0_low_s_16x8, factorChannel10_64_s_16x8);
	int16x8_t intermediateResults2_low_s_16x8 = vmulq_s16(source0_low_s_16x8, factorChannel20_64_s_16x8);

	int16x8_t intermediateResults0_high_s_16x8 = vmulq_s16(source0_high_s_16x8, factorChannel00_64_s_16x8);
	int16x8_t intermediateResults1_high_s_16x8 = vmulq_s16(source0_high_s_16x8, factorChannel10_64_s_16x8);
	int16x8_t intermediateResults2_high_s_16x8 = vmulq_s16(source0_high_s_16x8, factorChannel20_64_s_16x8);

	intermediateResults0_low_s_16x8 = vqaddq_s16(intermediateResults0_low_s_16x8, vmulq_s16(source1_low_s_16x8, factorChannel01_64_s_16x8)); // intermediateResults0 = saturated(intermediateResults0 + source10_low * factorChannel01)
	intermediateResults1_low_s_16x8 = vqaddq_s16(intermediateResults1_low_s_16x8, vmulq_s16(source1_low_s_16x8, factorChannel11_64_s_16x8));
	intermediateResults2_low_s_16x8 = vqaddq_s16(intermediateResults2_low_s_16x8, vmulq_s16(source1_low_s_16x8, factorChannel21_64_s_16x8));

	intermediateResults0_high_s_16x8 = vqaddq_s16(intermediateResults0_high_s_16x8, vmulq_s16(source1_high_s_16x8, factorChannel01_64_s_16x8));
	intermediateResults1_high_s_16x8 = vqaddq_s16(intermediateResults1_high_s_16x8, vmulq_s16(source1_high_s_16x8, factorChannel11_64_s_16x8));
	intermediateResults2_high_s_16x8 = vqaddq_s16(intermediateResults2_high_s_16x8, vmulq_s16(source1_high_s_16x8, factorChannel21_64_s_16x8));

	intermediateResults0_low_s_16x8 = vqaddq_s16(intermediateResults0_low_s_16x8, vmulq_s16(source2_low_s_16x8, factorChannel02_64_s_16x8));
	intermediateResults1_low_s_16x8 = vqaddq_s16(intermediateResults1_low_s_16x8, vmulq_s16(source2_low_s_16x8, factorChannel12_64_s_16x8));
	intermediateResults2_low_s_16x8 = vqaddq_s16(intermediateResults2_low_s_16x8, vmulq_s16(source2_low_s_16x8, factorChannel22_64_s_16x8));

	intermediateResults0_high_s_16x8 = vqaddq_s16(intermediateResults0_high_s_16x8, vmulq_s16(source2_high_s_16x8, factorChannel02_64_s_16x8));
	intermediateResults1_high_s_16x8 = vqaddq_s16(intermediateResults1_high_s_16x8, vmulq_s16(source2_high_s_16x8, factorChannel12_64_s_16x8));
	intermediateResults2_high_s_16x8 = vqaddq_s16(intermediateResults2_high_s_16x8, vmulq_s16(source2_high_s_16x8, factorChannel22_64_s_16x8));

	uint8x16x4_t results_u_8x16x4;

	// saturated narrow signed to unsigned, normalized by 2^6
	results_u_8x16x4.val[0] = vcombine_u8(vqrshrun_n_s16(intermediateResults0_low_s_16x8, 6), vqrshrun_n_s16(intermediateResults0_high_s_16x8, 6));
	results_u_8x16x4.val[1] = vcombine_u8(vqrshrun_n_s16(intermediateResults1_low_s_16x8, 6), vqrshrun_n_s16(intermediateResults1_high_s_16x8, 6));
	results_u_8x16x4.val[2] = vcombine_u8(vqrshrun_n_s16(intermediateResults2_low_s_16x8, 6), vqrshrun_n_s16(intermediateResults2_high_s_16x8, 6));
	results_u_8x16x4.val[3] = channelValue3_u_8x16;

	// and we can store the result
	vst4q_u8(target, results_u_8x16x4);
}

template <bool tUseFactorChannel0, bool tUseFactorChannel1, bool tUseFactorChannel2, bool tUseFactorChannel3>
void FrameChannels::convert4ChannelsTo1Channel8Pixels8BitPerChannel7BitPrecisionNEON(const uint8_t* const source, uint8_t* const target, const uint8x8_t& factorChannel0_128_u_8x8, const uint8x8_t& factorChannel1_128_u_8x8, const uint8x8_t& factorChannel2_128_u_8x8, const uint8x8_t& factorChannel3_128_u_8x8)
{
	static_assert(tUseFactorChannel0 || tUseFactorChannel1 || tUseFactorChannel2 || tUseFactorChannel3, "Invalid multiplication factors!");

	ocean_assert(source != nullptr && target != nullptr);

	// the documentation of this function designed for RGBA32 to Y8 conversion

	// precise color space conversion:
	// Y = 0.299 * R + 0.587 * G + 0.114 * B

	// approximation:
	// Y = (38 * R + 75 * G + 15 * B) / 128

	// we expect the following input pattern (for here RGBA32):
	// FEDC BA98 7654 3210
	// ABGR ABGR ABGR ABGR

	// we load 8 pixels (= 4 * 8 values) and directly deinterleave the 4 channels so that we receive the following patterns:
	// m4_64_pixels.val[0]: R R R R R R R R
	// m4_64_pixels.val[1]: G G G G G G G G
	// m4_64_pixels.val[2]: B B B B B B B B
	// m4_64_pixels.val[3]: A A A A A A A A

	uint8x8x4_t pixels_u_8x8x4 = vld4_u8(source);

	uint16x8_t intermediateResults_16x8;

	// we multiply the first channel with the specified factor (unless zero)

	if constexpr (tUseFactorChannel0)
	{
		intermediateResults_16x8 = vmull_u8(pixels_u_8x8x4.val[0], factorChannel0_128_u_8x8);
	}
	else
	{
		intermediateResults_16x8 = vdupq_n_u16(0u);
	}

	// we multiply the second channel with the specified factor (unless zero) and accumulate the results

	if constexpr (tUseFactorChannel1)
	{
		intermediateResults_16x8 = vmlal_u8(intermediateResults_16x8, pixels_u_8x8x4.val[1], factorChannel1_128_u_8x8);
	}

	// we multiply the third channel with the specified factor (unless zero) and accumulate the results

	if constexpr (tUseFactorChannel2)
	{
		intermediateResults_16x8 = vmlal_u8(intermediateResults_16x8, pixels_u_8x8x4.val[2], factorChannel2_128_u_8x8);
	}

	// we multiply the fourth channel with the specified factor (unless zero) and accumulate the results

	if constexpr (tUseFactorChannel3)
	{
		intermediateResults_16x8 = vmlal_u8(intermediateResults_16x8, pixels_u_8x8x4.val[3], factorChannel3_128_u_8x8);
	}

	// we shift the 16 bit values by 7 bits (= 128), apply rounding, and narrow the 16 bit integers to 8 bit integers within one operation
	uint8x8_t results_u_8x8 = vqrshrn_n_u16(intermediateResults_16x8, 7); // pixels_u_8x8x4 = (intermediateResults_16x8 + 2^6) >> 2^7

	// and we can store the result
	vst1_u8(target, results_u_8x8);
}

OCEAN_FORCE_INLINE void FrameChannels::convert4ChannelsTo2Channels8Pixels8BitPerChannel7BitPrecisionNEON(const uint8_t* const source, uint8_t* const target, const uint8x8_t& factorChannel00_128_u_8x8, const uint8x8_t& factorChannel10_128_u_8x8, const uint8x8_t& factorChannel01_128_u_8x8, const uint8x8_t& factorChannel11_128_u_8x8, const uint8x8_t& factorChannel02_128_u_8x8, const uint8x8_t& factorChannel12_128_u_8x8, const uint8x8_t& factorChannel03_128_u_8x8, const uint8x8_t& factorChannel13_128_u_8x8)
{
	ocean_assert(source != nullptr && target != nullptr);

	// the documentation of this function designed for RGBA32 to YA16 conversion

	// precise color space conversion:
	// Y = 0.299 * R + 0.587 * G + 0.114 * B + 0.0 * A
	// A =   0.0 * R +   0.0 * G +   0.0 * B + 1.0 * A

	// approximation:
	// Y = (38 * R + 75 * G + 15 * B + 0 * A) / 128
	// A = (128 * A) / 128

	// we expect the following input pattern (for here RGBA32):
	// FEDC BA98 7654 3210
	// ABGR ABGR ABGR ABGR

	// we load 8 pixels (= 4 * 8 values) and directly deinterleave the 4 channels so that we receive the following patterns:
	// m4_64_pixels.val[0]: R R R R R R R R
	// m4_64_pixels.val[1]: G G G G G G G G
	// m4_64_pixels.val[2]: B B B B B B B B
	// m4_64_pixels.val[3]: A A A A A A A A

	uint8x8x4_t pixels_u_8x8x4 = vld4_u8(source);

	uint16x8_t intermediateResultsChannel0_16x8 = vmull_u8(pixels_u_8x8x4.val[0], factorChannel00_128_u_8x8);
	uint16x8_t intermediateResultsChannel1_16x8 = vmull_u8(pixels_u_8x8x4.val[0], factorChannel10_128_u_8x8);

	intermediateResultsChannel0_16x8 = vmlal_u8(intermediateResultsChannel0_16x8, pixels_u_8x8x4.val[1], factorChannel01_128_u_8x8);
	intermediateResultsChannel1_16x8 = vmlal_u8(intermediateResultsChannel1_16x8, pixels_u_8x8x4.val[1], factorChannel11_128_u_8x8);

	intermediateResultsChannel0_16x8 = vmlal_u8(intermediateResultsChannel0_16x8, pixels_u_8x8x4.val[2], factorChannel02_128_u_8x8);
	intermediateResultsChannel1_16x8 = vmlal_u8(intermediateResultsChannel1_16x8, pixels_u_8x8x4.val[2], factorChannel12_128_u_8x8);

	intermediateResultsChannel0_16x8 = vmlal_u8(intermediateResultsChannel0_16x8, pixels_u_8x8x4.val[3], factorChannel03_128_u_8x8);
	intermediateResultsChannel1_16x8 = vmlal_u8(intermediateResultsChannel1_16x8, pixels_u_8x8x4.val[3], factorChannel13_128_u_8x8);

	uint8x8x2_t results_u_8x8x2;

	// we shift the 16 bit values by 7 bits (= 128), apply rounding, and narrow the 16 bit integers to 8 bit integers within one operation

	results_u_8x8x2.val[0] = vqrshrn_n_u16(intermediateResultsChannel0_16x8, 7); // results_u_8x8x2.val[0] = (intermediateResultsChannel0_16x8 + 2^6) >> 2^7
	results_u_8x8x2.val[1] = vqrshrn_n_u16(intermediateResultsChannel1_16x8, 7);

	// and we can store the result
	vst2_u8(target, results_u_8x8x2);
}

#endif // OCEAN_HARDWARE_NEON_VERSION

}

}

#endif // META_OCEAN_CV_FRAME_CHANNELS_H
