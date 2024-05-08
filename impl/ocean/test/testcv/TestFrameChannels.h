/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_FRAME_CHANNELS_H
#define META_OCEAN_TEST_TESTCV_TEST_FRAME_CHANNELS_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/base/DataType.h"
#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/FrameChannels.h"
#include "ocean/cv/FrameConverter.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements a frame channels test.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameChannels
{
	public:

		/**
		 * Tests all frame channels functions.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if so
		 */
		static bool test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests frame separation to one channel function.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSeparateTo1Channel(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests frame zip channels function.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testZipChannels(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the add channel function that adds a channel as new first channel.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if so
		 */
		static bool testAddFirstChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the add channel value function that adds a channel as new first channel.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if so
		 */
		static bool testAddFirstChannelValue(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the add channel function that adds a channel as new last channel.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if so
		 */
		static bool testAddLastChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the add channel value function that adds a channel as new last channel.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if so
		 */
		static bool testAddLastChannelValue(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the remove channel function that removes the first channel.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if so
		 */
		static bool testRemoveFirstChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the remove channel function that removes the last channel.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if so
		 */
		static bool testRemoveLastChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the copy channel function.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if so
		 */
		static bool testCopyChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the set channel function.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if so
		 */
		static bool testSetChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the advanced pixel modifier function
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if the test was successful
		 */
		static bool testApplyAdvancedPixelModifier(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the advanced pixel modifier function for specifc element type and number of channels
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if the test was successful
		 * @tparam TElement The type that the test data will use, must be an integral type
		 * @tparam tChannels The number of channels that the test data will use, range: [1, 4]
		 * @tparam tFunction The pixel function that will be used for testing, must be valid
		 */
		template <typename TElement, unsigned int tChannels, void (*tPixelFunction)(const TElement*, TElement*)>
		static bool testApplyAdvancedPixelModifier(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Test the pixel operator for two inputs and one output.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if the test was successful, otherwise false
		 * @tparam TPrecision The type that the test data will use, must be an integral type
		 * @tparam tChannels The number of channels that the test data will use, range: [1, 4]
		 */
		template <typename TPrecision, unsigned int tChannels>
		static bool testApplyBivariateOperator(const double testDuration, Worker& worker);

		/**
		 * Test the image transformation function for generic pixel formats.
		 * @param testDuration The number of seconds for each test, range: (0, infinity)
		 * @param worker A worker instance to test the parallelized version of the test function
		 * @return True if the test was successful, otherwise false
		 */
		static bool testTransformGeneric(const double testDuration, Worker& worker);

		/**
		 * Tests the image transformation function for a specific generic pixel format.
		 * @param width The width of the images to be tested, range: [1, infinity)
		 * @param height The height of the images to be tested, range: [1, infinity)
		 * @param testDuration The number of seconds for each test, range: (0, infinity)
		 * @param worker A worker instance to test the parallelized version of the test function
		 * @return True if the test was successful, otherwise false
		 * @tparam TElementType Type used for data of the images (will be used internally to generate random test data)
		 * @tparam tChannels The number of channels of the test images (will be used internally to generate random test data), range: [1, infinity)
		 */
		template <typename TElementType, unsigned int tChannels>
		static bool testTransformGeneric(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the conversion function from premultiplied alpha to straight alpha images.
		 * @param testDuration The number of seconds for each test, range: (0, infinity)
		 * @param worker A worker instance to test the parallelized version of the test function
		 * @return True if the test was successful, otherwise false
		 */
		static bool testPremultipliedAlphaToStraightAlpha(const double testDuration, Worker& worker);

		/**
		 * Tests the conversion function from straight alpha to premultiplied alpha images.
		 * @param testDuration The number of seconds for each test, range: (0, infinity)
		 * @param worker A worker instance to test the parallelized version of the test function
		 * @return True if the test was successful, otherwise false
		 */
		static bool testStraightAlphaToPremultipliedAlpha(const double testDuration, Worker& worker);

		/**
		 * Tests the image conversion function reversing the order of pixel channels.
		 * @param testDuration The number of seconds for each test, range: (0, infinity)
		 * @param worker A worker instance to test the parallelized version of the test function
		 * @return True if the test was successful, otherwise false
		 */
		static bool testReverseChannelOrder(const double testDuration, Worker& worker);

		/**
		 * Tests the image conversion function reversing the order of pixel channels for a specific data type and channel number.
		 * @param width The width of the images to be tested, with range [1, infinity)
		 * @param height The height of the images to be tested, with range [1, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker A worker instance to test the parallelized version of the test function
		 * @return True if the test was successful, otherwise false
		 * @tparam T The data type of each pixel channel, e.g., 'unsigned char' or 'float'
		 * @tparam tChannels The number of channels of the test images, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static bool testReverseChannelOrder(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Test the row-based pixel conversion from 3 channels to 1 channel.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testRowPixelConversion3ChannelsTo1Channel(const double testDuration);

		/**
		 * Test the row-based pixel conversion from 3 channels to 3 channels with 6 bit precision.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testRowPixelConversion3ChannelsTo3Channels6BitPrecision(const double testDuration);

		/**
		 * Test the row-based pixel conversion from 3 channels to 3 channels with 7 bit precision.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testRowPixelConversion3ChannelsTo3Channels7BitPrecision(const double testDuration);

		/**
		 * Test the row-based pixel conversion from 3 channels to 3 channels with 10 bit precision.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testRowPixelConversion3ChannelsTo3Channels10BitPrecision(const double testDuration);

		/**
		 * Test the row-based pixel conversion from 4 channels to 1 channel.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testRowPixelConversion4ChannelsTo1Channel(const double testDuration);

		/**
		 * Test the row-based pixel conversion from 4 channels to 2 channel.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testRowPixelConversion4ChannelsTo2Channels(const double testDuration);

		/**
		 * Test the row-based pixel conversion from 4 channels to 3 channel.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testRowPixelConversion4ChannelsTo3Channels(const double testDuration);

		/**
		 * Test the row-based pixel reverse order function.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testReverseRowPixelOrder(const double testDuration);

		/**
		 * Test the row-based pixel reverse order function.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testReverseRowChannelOrder(const double testDuration);

		/**
		 * Test the row-based shuffling of pixel channels.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testShuffleRowChannels(const double testDuration);

		/**
		 * Test the row-based shuffling of pixel channels and setting the last channel with constant value.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testShuffleRowChannelsAndSetLastChannelValue(const double testDuration);

		/**
		 * Test the row-based narrowing of pixel with 16 bit channels.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testNarrowRow16BitPerChannels(const double testDuration);

		/**
		 * Test function to be used with the test for the advanced pixel modifier
		 * @param source The pointer to the source pixel, must be valid
		 * @param target The pointer to the target pixel, must be valid
		 * @tparam TSource The type of the source
		 * @tparam TTarget The type of the target
		 */
		template <typename TSource, typename TTarget, unsigned int tChannels>
		static void testFunctionApplyAdvancedModifier(const TSource* source, TTarget* target);

	protected:

		/**
		 * Tests frame separation to one channel function.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param channels The number of channels the test frame has, with range [2, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam TSource The data type of the source elements
		 * @tparam TTarget The data type of the target elements
		 */
		template <typename TSource, typename TTarget>
		static bool testSeparateTo1Channel(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration);

		/**
		 * Tests zip channels function.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param channels The number of channels the test frame has, with range [2, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam TSource The data type of the source elements
		 * @tparam TTarget The data type of the target elements
		 */
		template <typename TSource, typename TTarget>
		static bool testZipChannels(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration);

		/**
		 * Tests the add channel function that adds a channel as new first channel.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if so
		 * @tparam T The data type of each pixel element, e.g., 'unsigned char' or 'float'
		 * @tparam tSourceChannels The number of channels of the source frame, with range [1, infinity)
		 */
		template <typename T, unsigned int tSourceChannels>
		static bool testAddFirstChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the add channel value function that adds a channel as new first channel.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if so
		 * @tparam T The data type of each pixel element, e.g., 'unsigned char' or 'float'
		 * @tparam tSourceChannels The number of channels of the source frame, with range [1, infinity)
		 */
		template <typename T, unsigned int tSourceChannels>
		static bool testAddFirstChannelValue(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the add channel function that adds a channel as new last channel.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if so
		 * @tparam T The data type of each pixel element, e.g., 'unsigned char' or 'float'
		 * @tparam tSourceChannels The number of channels of the source frame, with range [1, infinity)
		 */
		template <typename T, unsigned int tSourceChannels>
		static bool testAddLastChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the add channel value function that adds a channel as new last channel.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if so
		 * @tparam T The data type of each pixel element, e.g., 'unsigned char' or 'float'
		 * @tparam tSourceChannels The number of channels of the source frame, with range [1, infinity)
		 */
		template <typename T, unsigned int tSourceChannels>
		static bool testAddLastChannelValue(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the remove channel function that removes the first channel.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if so
		 * @tparam T The data type of each pixel element, e.g., 'unsigned char' or 'float'
		 * @tparam tSourceChannels The number of channels of the source frame, with range [2, infinity)
		 */
		template <typename T, unsigned int tSourceChannels>
		static bool testRemoveFirstChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the remove channel function that removes the last channel.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if so
		 * @tparam T The data type of each pixel element, e.g., 'unsigned char' or 'float'
		 * @tparam tSourceChannels The number of channels of the source frame, with range [2, infinity)
		 */
		template <typename T, unsigned int tSourceChannels>
		static bool testRemoveLastChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the copy channel function.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if so
		 * @tparam T The data type of each pixel element, e.g., 'unsigned char' or 'float'
		 * @tparam tSourceChannels The number of channels of the source frame, with range [1, infinity)
		 * @tparam tTargetChannels The number of channels of the source frame, with range [1, infinity)
		 * @tparam tSourceChannelIndex The number of channels of the source frame, with range [1, tSourceChannels - 1]
		 * @tparam tTargetChannelIndex The number of channels of the source frame, with range [1, tTargetchannels - 1]
		 */
		template <typename T, unsigned int tSourceChannels, unsigned int tTargetChannels, unsigned int tSourceChannelIndex, unsigned int tTargetChannelIndex>
		static bool testCopyChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the set channel function.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if so
		 * @tparam T The data type of each pixel element, e.g., 'unsigned char' or 'float'
		 * @tparam tChannel The number of channels of the source frame, with range [1, tChannels - 1]
		 * @tparam tChannels The number of channels of the frame, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannel, unsigned int tChannels>
		static bool testSetChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Validates the add channel function that add a new channel as first channel.
		 * @param source The source frame to which the new channel will be added, must be valid
		 * @param sourceNewChannel The new channel to be added, must be valid
		 * @param target The target frame with added channel, must be valid
		 * @param sourceChannels The number of channels the source frame has, with range [1, infinity)
		 * @param width The width of both frames frame in pixel, with range [1, infinity)
		 * @param height The height of both frames frame in pixel, with range [1, infinity)
		 * @param flag The conversion type to be applied
		 * @param sourcePaddingElements Optional number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param sourceNewChannelPaddingElements Optional number of padding elements at the end of each new-channel-source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @tparam T The data type of each pixel element, e.g., 'unsigned char' or 'float'
		 */
		template <typename T>
		static bool validateAddFirstChannel(const T* source, const T* sourceNewChannel, const T* target, const unsigned int sourceChannels, const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int sourceNewChannelPaddingElements, const unsigned int targetPaddingElements);

		/**
		 * Validates the add channel value function that adds a new channel as first channel.
		 * @param source The source frame to which the new channel will be added, must be valid
		 * @param newChannelValue The value of the new channel
		 * @param target The target frame with added channel, must be valid
		 * @param sourceChannels The number of channels the source frame has, with range [1, infinity)
		 * @param width The width of both frames frame in pixel, with range [1, infinity)
		 * @param height The height of both frames frame in pixel, with range [1, infinity)
		 * @param flag The conversion type to be applied
		 * @param sourcePaddingElements Optional number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @tparam T The data type of each pixel element, e.g., 'unsigned char' or 'float'
		 */
		template <typename T>
		static bool validateAddFirstChannelValue(const T* source, const T& newChannelValue, const T* target, const unsigned int sourceChannels, const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements);

		/**
		 * Validates the add channel function that add a new channel as last channel.
		 * @param source The source frame to which the new channel will be added, must be valid
		 * @param sourceNewChannel The new channel to be added, must be valid
		 * @param target The target frame with added channel, must be valid
		 * @param sourceChannels The number of channels the source frame has, with range [1, infinity)
		 * @param width The width of both frames frame in pixel, with range [1, infinity)
		 * @param height The height of both frames frame in pixel, with range [1, infinity)
		 * @param flag The conversion type to be applied
		 * @param sourcePaddingElements Optional number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param sourceNewChannelPaddingElements Optional number of padding elements at the end of each new-channel-source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @tparam T The data type of each pixel element, e.g., 'unsigned char' or 'float'
		 */
		template <typename T>
		static bool validateAddLastChannel(const T* source, const T* sourceNewChannel, const T* target, const unsigned int sourceChannels, const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int sourceNewChannelPaddingElements, const unsigned int targetPaddingElements);

		/**
		 * Validates the add channel value function that adds a new channel as last channel.
		 * @param source The source frame to which the new channel will be added, must be valid
		 * @param newChannelValue The value of the new channel
		 * @param target The target frame with added channel, must be valid
		 * @param sourceChannels The number of channels the source frame has, with range [1, infinity)
		 * @param width The width of both frames frame in pixel, with range [1, infinity)
		 * @param height The height of both frames frame in pixel, with range [1, infinity)
		 * @param flag The conversion type to be applied
		 * @param sourcePaddingElements Optional number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @tparam T The data type of each pixel element, e.g., 'unsigned char' or 'float'
		 */
		template <typename T>
		static bool validateAddLastChannelValue(const T* source, const T& newChannelValue, const T* target, const unsigned int sourceChannels, const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements);

		/**
		 * Validates the advanced pixel modification function
		 * @param source The source frame that will be used for the validation, must be valid
		 * @param validationTarget The target frame that will be validated, must be valid
		 * @param conversionFlag The conversion flag that will be used for the validation
		 * @tparam TElement The type that the test data will use, must be an integral type
		 * @tparam tChannels The number of channels that the test data will use, range: [1, 4]
		 * @tparam tFunction The pixel function that will be used for testing, must be valid
		 */
		template <typename TElement, unsigned int tChannels, void (*tPixelFunction)(const TElement*, TElement*)>
		static bool validateApplyAdvancedPixelModifier(const Frame& source, const Frame& validationTarget, const CV::FrameConverter::ConversionFlag conversionFlag);

		/**
		 * Validates the bivariate operator using subtraction
		 * @param source0 The first source frame, must be valid
		 * @param source1 The second source frame, must be valid
		 * @param target The target frame that will be validated, must be valid
		 * @param flag The conversion flag
		 * @tparam TSource0 Type of the first source
		 * @tparam TSource1 Type of the second source
		 * @tparam TTarget Type of the target
		 * @return True on successful validation, otherwise false
		 */
		template <typename TSource0, typename TSource1, typename TTarget>
		static bool validateApplyBivariateOperatorSubtract(const Frame& source0, const Frame& source1, const Frame& target, const CV::FrameChannels::ConversionFlag flag);

		/**
		 * Validation function for image transformations like flipping and mirroring for a specific generic pixel format.
		 * @param source Pointer to the source data, must be valid
		 * @param target Pointer to the data to be tested, must be valid
		 * @param width The width of the images to be tested, range: [1, infinity)
		 * @param height The height of the images to be tested, range: [1, infinity)
		 * @param sourcePaddingElements The number of padding elements in the source image, range: [0, infinity)
		 * @param targetPaddingElements The number of padding elements in the test image, range: [0, infinity)
		 * @param flag Conversion flag, cf. @c CV::FrameConverter::ConversionFlag
		 * @return True if the test was successful, otherwise false
		 * @tparam TElementType Type used for data of the images (will be used internally to generate random test data)
		 * @tparam tChannels The number of channels of the test images (will be used internally to generate random test data), range: [1, infinity)
		 */
		template <typename TElementType, unsigned int tChannels>
		static bool validateTransformGeneric(const TElementType* source, const TElementType* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const CV::FrameConverter::ConversionFlag flag);

		/**
		 * Validates the reversion of channels of frame pixels.
		 * @param source The source frame for which the channels will be reversed, must be valid
		 * @param target The target frame with reversed channels, must be valid
		 * @param width The width of the images to be tested, with range [1, infinity)
		 * @param height The height of the images to be tested, with range [1, infinity)
		 * @param channels The number of channels of the test images, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements in the source image, witgh range [0, infinity)
		 * @param targetPaddingElements The number of padding elements in the test image, with range [0, infinity)
		 * @param flag The conversion flag which has been applied
		 * @return True if the test was successful, otherwise false
		 * @tparam T The data type of each pixel channel, e.g., 'unsigned char' or 'float'
		 */
		template <typename T>
		static bool validateReverseChannelOrder(const T* source, const T* target, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const CV::FrameConverter::ConversionFlag flag);

		/**
		 * Validates the remove channel function that removes the first channel.
		 * @param source The source frame from which the channel will be removed, must be valid
		 * @param target The target frame with removed channel, must be valid
		 * @param sourceChannels The number of channels of the source frame, with range [2, infinity)
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param flag Conversion flag
		 * @param sourcePaddingElements The number of padding elements at the end of each row of the source image, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each row of the target image, in elements, with range [0, infinity)
		 * @tparam T The data type of each pixel channel, e.g., 'unsigned char' or 'float'
		 */
		template <typename T>
		static bool validateRemoveFirstChannel(const T* source, const T* target, const unsigned int sourceChannels, const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements);

		/**
		 * Validates the remove channel function that removes the last channel.
		 * @param source The source frame from which the channel will be removed, must be valid
		 * @param target The target frame with removed channel, must be valid
		 * @param sourceChannels The number of channels of the source frame, with range [2, infinity)
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param flag Conversion flag
		 * @param sourcePaddingElements The number of padding elements at the end of each row of the source image, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each row of the target image, in elements, with range [0, infinity)
		 */
		template <typename T>
		static bool validateRemoveLastChannel(const T* source, const T* target, const unsigned int sourceChannels, const unsigned int width, const unsigned int height, const CV::FrameConverter::ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements);

		/**
		 * Validates the copy channel function.
		 * @param source The source frame from which one channel will be copied, must be valid
		 * @param originalTarget The target frame before the channel has been copied, must be valid
		 * @param target The target frame to which the channel has been copied, must be valid
		 * @param sourceChannels The number of channels of the source frame, with range [1, infinity)
		 * @param targetChannels The number of channels of the target frame, with range [1, infinity)
		 * @param sourceChannelIndex The index of the source channel to be copied, with range [0, sourceChannels - 1]
		 * @param targetChannelIndex The index of the target channel which has received the copied channel, with range [0, targetChannels - 1]
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each row of the source image, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each row of the target image, in elements, with range [0, infinity)
		 */
		template <typename T>
		static bool validateCopyChannel(const T* source, const T* originalTarget, const T* target, const unsigned int sourceChannels, const unsigned int targetChannels, const unsigned int sourceChannelIndex, const unsigned int targetChannelIndex, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements);

		/**
		 * Validates the set channel function.
		 * @param originalFrame The frame before the channel has been set, must be valid
		 * @param frame The frame with modified channel, must be valid
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param value The channel value that has been set
		 * @param channelIndex The index of the channel which has been set, with range [0, channels - 1]
		 * @param channels The number of channels of the frame, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each row of the image, in elements, with range [0, infinity)
		 */
		template <typename T>
		static bool validateSetChannel(const T* originalFrame, const T* frame, const unsigned int width, const unsigned int height, const T value, const unsigned int channelIndex, const unsigned int channels, const unsigned int framePaddingElements);

		/**
		 * Validates the row-based pixel reverse order function for a specific data type and channel number.
		 * @param randomGenerator The random generator object to be used
		 * @return True, if succeeded
		 * @tparam T The data type of the each pixel element
		 * @tparam tChannels The number of channels per pixel, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static bool validateReverseRowPixelOrder(RandomGenerator& randomGenerator);

		/**
		 * Validates the row-based channel reverse order function for a specific data type and channel number.
		 * @param randomGenerator The random generator object to be used
		 * @return True, if succeeded
		 * @tparam T The data type of the each pixel element
		 * @tparam tChannels The number of channels per pixel, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static bool validateReverseRowChannelOrder(RandomGenerator& randomGenerator);

		/**
		 * Validates the row-based shuffle function for channels.
		 * @param randomGenerator The random generator object to be used
		 * @return True, if succeeded
		 * @tparam T Data type of each channel pixel value, e.g, 'unsigned char' or 'float'
		 * @tparam tSourceChannels The number of source data channels, with range [1, 8u]
		 * @tparam tTargetChannels The number of target data channels, with range [1, 8u]
		 * @tparam tShufflePattern Groups of four bits define the source channel, e.g., 0x76543210 defines the identity transformation, 0x01234567 defines the reverse transformation
		 */
		template <typename T, unsigned int tSourceChannels, unsigned int tTargetChannels, unsigned int tShufflePattern>
		static bool validateShuffleRowChannels(RandomGenerator& randomGenerator);

		/**
		 * Validates the row-based shuffle and setting last channel with constant value function for channels.
		 * @param randomGenerator The random generator object to be used
		 * @return True, if succeeded
		 * @tparam T Data type of each channel pixel value, e.g, 'unsigned char' or 'float'
		 * @tparam tSourceChannels The number of source data channels, with range [1, 8u]
		 * @tparam tTargetChannels The number of target data channels, with range [2, 8u]
		 * @tparam tShufflePattern Groups of four bits define the source channel, e.g., 0x76543210 defines the identity transformation, 0x01234567 defines the reverse transformation
		 */
		template <typename T, unsigned int tSourceChannels, unsigned int tTargetChannels, unsigned int tShufflePattern>
		static bool validateShuffleRowAndSetLastChannelValueChannels(RandomGenerator& randomGenerator);

		/**
		 * Validates the row-based narrow function for 16 bit channels.
		 * @param randomGenerator The random generator object to be used
		 * @return True, if succeeded
		 * @tparam tChannels The number of channels, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static bool validateNarrowRow16BitPerChannels(RandomGenerator& randomGenerator);

		/**
		 * Collection of test operations for pixel operators.
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		struct TestOperations
		{
			/**
			 * Facilitates subtraction between two sources and stores the result to another destination
			 * Essentially, target = source0 - source1
			 * @param source0 The first source pixel, must be valid
			 * @param source1 The second source pixel must be valid
			 * @param target The location where the result will be stored (of the targer pixel), must be valid
			 */
			template <typename TSource0, typename TSource1, typename TTarget>
			static void subtract(const TSource0* source0, const TSource1* source1, TTarget* target);
		};
};

template <unsigned int tChannels>
template <typename TSource0, typename TSource1, typename TTarget>
void TestFrameChannels::TestOperations<tChannels>::subtract(const TSource0* source0, const TSource1* source1, TTarget* target)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(source0 != nullptr && source1 != nullptr && target != nullptr);

	for (unsigned int c = 0u; c < tChannels; ++c)
	{
		target[c] = (TTarget)(source0[c] - source1[c]);
	}
};

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_FRAME_CHANNELS_H
