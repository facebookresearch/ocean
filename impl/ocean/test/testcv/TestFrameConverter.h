/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_FRAME_CONVERTER_H
#define META_OCEAN_TEST_TESTCV_TEST_FRAME_CONVERTER_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/base/Frame.h"
#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Memory.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameConverter.h"

#include <numeric>

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements a frame converter test for the function of the basic frame converter.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameConverter : protected CV::FrameConverter
{
	public:

		/**
		 * Tests all frame converter functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if all tests succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Tests the comfort convert function for a Frame.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testComfortConvert(const double testDuration);

		/**
		 * Tests the comfort convert and copy function for a Frame.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testComfortConvertAndCopy(const double testDuration);

		/**
		 * Tests the comfort change function for a Frame.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testComfortChange(const double testDuration);

		/**
		 * Tests the cast function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testCast(const double testDuration);

		/**
		 * Tests the normalized cast function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testNormalizedCast(const double testDuration);

		/**
		 * Tests the sub frame function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSubFrame(const double testDuration);

		/**
		 * Tests the sub frame function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSubFrameMask(const double testDuration);

		/**
		 * Tests the patch creator.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPatchFrame(const double testDuration);

		/**
		 * Tests the patch creator with mirrored border.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPatchFrameMirroredBorder(const double testDuration);

		/**
		 * Test the 1-row-based converter for pixel formats with 3 channels, 2 planes and a 2x2 downsampling of channel 2 and 3, with 6 bit precision.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testConvertOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision6Bit(const double testDuration);

		/**
		 * Test the 1-row-based converter for pixel formats with 3 channels, 2 planes and a 2x2 downsampling of channel 2 and 3, with 10 bit precision.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testConvertOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision10Bit(const double testDuration);

		/**
		 * Test the 2-row-based converter for pixel formats with 3 channels, 2 planes and a 2x2 downsampling of channel 2 and 3, with 10 bit precision.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testConvertTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision6Bit(const double testDuration);

		/**
		 * Test the 2-row-based converter for pixel formats with 3 channels, 2 planes and a 2x2 downsampling of channel 2 and 3, with 10 bit precision.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testConvertTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision10Bit(const double testDuration);

		/**
		 * Test the 2-row-based converter for pixel formats with 3 channels, 2 planes and a 2x2 downsampling of channel 2 and 3, with 7 bit precision.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testConvertTwoRows_1Plane3Channels_To_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_8BitPerChannel_Precision7Bit(const double testDuration);

		/**
		 * Test the 2-row-based converter for pixel formats with 3 channels, 3 planes and a 2x2 downsampling of channel 2 and 3, with 7 bit precision.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testConvertTwoRows_1Plane3Channels_To_1Plane1ChannelAnd2Planes1ChannelsDownsampled2x2_8BitPerChannel_Precision7Bit(const double testDuration);

		/**
		 * Test the 1-row-based mapper for pixel formats with 3 planes and 1 channel to 1 plane and 3 channels.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testMapOneRow_3Plane1Channel_To_1Plane3Channels_8BitPerChannel(const double testDuration);

		/**
		 * Test the 1-row-based mapper for pixel formats with 1 plane and 3 channels to 3 planes and 1 channel.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testMapOneRow_1Plane3Channels_To_3Plane1Channel_8BitPerChannel(const double testDuration);

		/**
		 * Test the 1-row-based mapper for pixel formats with 3 channels, 1 planes and a 2x1 downsampling of channel 2 and 3.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testMapOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1BackIsDownsampled_To_1Plane3Channels_8BitPerChannel(const double testDuration);

		/**
		 * Test the 1-row-based mapper for pixel formats with 3 channels, 1 planes and a 2x1 downsampling of channel 1 and 3.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testMapOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1FrontIsDownsampled_To_1Plane3Channels_8BitPerChannel(const double testDuration);

		/**
		 * Test the 1-row-based mapper for pixel formats with 3 channels, 2 planes and a 2x2 downsampling of channel 2 and 3.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testMapOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel(const double testDuration);

		/**
		 * Test the 2-row-based mapper for pixel formats with 3 channels, 2 planes and a 2x2 downsampling of channel 2 and 3.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testMapTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel(const double testDuration);

		/**
		 * Test the 1-row-based converter for pixel formats with 3 channels, 3 planes and a 2x2 downsampling of channel 2 and 3.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testConvertOneRow_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision10Bit(const double testDuration);

		/**
		 * Test the 2-row-based converter for pixel formats with 3 channels, 3 planes and a 2x2 downsampling of channel 2 and 3.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testConvertTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision6Bit(const double testDuration);

		/**
		 * Test the 2-row-based converter for pixel formats with 3 channels, 3 planes and a 2x2 downsampling of channel 2 and 3 adding a new target channel.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testConvertTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane4Channels_8BitPerChannel_Precision6Bit(const double testDuration);

		/**
		 * Test the 2-row-based converter for pixel formats with 3 channels, 3 planes and a 2x2 downsampling of channel 2 and 3.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testConvertTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision10Bit(const double testDuration);

		/**
		 * Test the 1-row-based mapper for pixel formats with 3 channels, 2 planes and a 2x2 downsampling of channel 2 and 3.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testMapOneRow_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel(const double testDuration);

		/**
		 * Test the 2-row-based mapper for pixel formats with 3 channels, 2 planes and a 2x2 downsampling of channel 2 and 3.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testMapTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel(const double testDuration);

		/**
		 * Test the 1-row-based convert 3-plane to zipped 3-channel function, with 6 bit precision.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testConvertOneRow_3Planes1Channel_To_1Plane3Channels_8BitPerChannel_Precision6Bit(const double testDuration);

		/**
		 * Test the 1-row-based convert zipped 3-channel with 2x1 downsampling to 3-channel function, with 10 bit precision.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testConvertOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1BackIsDownsampled_To_1Plane3Channels_8BitPerChannel_Precision10Bit(const double testDuration);

		/**
		 * Test the 1-row-based convert zipped 3-channel with 2x1 downsampling to 3-channel function, with 10 bit precision.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if the test was successful, otherwise false
		 */
		static bool testConvertOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1FrontIsDownsampled_To_1Plane3Channels_8BitPerChannel_Precision10Bit(const double testDuration);

		/**
		 * Tests the color space conversion matrices.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testConversionMatrices(const double testDuration);

	protected:

		/**
		 * Tests the sub frame function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of the elements to be used
		 */
		template <typename T>
		static bool testSubFrame(const double testDuration);

		/**
		 * Tests the cast function for from 'unsigned char' to a specified data type.
		 * @param width The width of the frame to be tested, with range [1, infinity)
		 * @param height The height of the frame to be tested, with range [1, infinity)
		 * @param channels The number of channels the frame has, with range [1, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of the target frame
		 */
		template <typename T>
		static bool testCast(const unsigned int width, const unsigned int height, const unsigned int channels);

		/**
		 * Tests the cast function for from 'unsigned char' to a specified data type.
		 * @param width The width of the frame to be tested, with range [1, infinity)
		 * @param height The height of the frame to be tested, with range [1, infinity)
		 * @param channels The number of channels the frame has, with range [1, infinity)
		 * @param normalization The normalization parameter that is applied
		 * @param offset The offset that is added to all values
		 * @return True, if succeeded
		 * @tparam T The data type of the target frame
		 */
		template <typename T>
		static bool testNormalizedCast(const unsigned int width, const unsigned int height, const unsigned int channels, const T normalization, const T offset);

		/**
		 * Tests the patch creator.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The element type to be used
		 */
		template <typename T>
		static bool testPatchFrame(const double testDuration);

		/**
		 * Tests the patch creator with mirrored border.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The element type to be used
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static bool testPatchFrameMirroredBorder(const double testDuration);

		/**
		 * Validates the sub-frame function.
		 * @param channels Number of data channels of both frames, with range [1, infinity)
		 * @param source The source frame from which the image content has been copied, must be valid
		 * @param sourceWidth Width of the source frame in pixel, with range [1, infinity)
		 * @param sourceHeight Height of the source frame in pixel, with range [1, infinity)
		 * @param target The target frame to which the image content has been copied, must be valid
		 * @param targetWidth Width of the target frame in pixel, with range [1, infinity)
		 * @param targetHeight Height of the target frame in pixel, with range [1, infinity)
		 * @param sourceLeft Horizontal start position of the sub-frame within the source frame, with range [0, sourceWidth - 1]
		 * @param sourceTop Vertical start position of the sub-frame within the source frame, with range [0, sourceHeight - 1]
		 * @param targetLeft Horizontal start position of the sub-frame within the target frame, with range [0, targetWidth - 1]
		 * @param targetTop Vertical start position of the sub-frame within the target frame, with range [0, targetHeight - 1]
		 * @param width The width of the sub-frame in pixel, with range [1, min(sourceWidth - sourceLeft, targetWidth - targetLeft)]
		 * @param height The height of the sub-frame in pixel, with range [1, min(sourceHeight - sourceTop, targetHeight - targetTop)]
		 * @param sourcePaddingElements Optional number of padding elements at the end of each source row, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, with range [0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of each element
		 */
		template <typename T>
		static bool validateSubFrame(const unsigned int channels, const T* source, const unsigned int sourceWidth, const unsigned int sourceHeight, const T* target, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int sourceLeft, const unsigned int sourceTop, const unsigned int targetLeft, const unsigned int targetTop, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements);

		/**
		 * Validates the sub-frame function supporting a mask.
		 * @param channels Number of data channels of both frames, with range [1, infinity)
		 * @param source The source frame from which the image content has been copied, must be valid
		 * @param sourceWidth Width of the source frame in pixel, with range [1, infinity)
		 * @param sourceHeight Height of the source frame in pixel, with range [1, infinity)
		 * @param originalTarget The copy of the target frame BEFORE the image content has been copied to it, must be valid
		 * @param target The target frame to which the image content has been copied, must be valid
		 * @param targetWidth Width of the target frame in pixel, with range [1, infinity)
		 * @param targetHeight Height of the target frame in pixel, with range [1, infinity)
		 * @param mask The binary mask that is used to indicate which source pixels to copy to the target frame, must be valid, have one channel, and have the same size as the source frame
		 * @param sourceLeft Horizontal start position of the sub-frame within the source frame, with range [0, sourceWidth - 1]
		 * @param sourceTop Vertical start position of the sub-frame within the source frame, with range [0, sourceHeight - 1]
		 * @param targetLeft Horizontal start position of the sub-frame within the target frame, with range [0, targetWidth - 1]
		 * @param targetTop Vertical start position of the sub-frame within the target frame, with range [0, targetHeight - 1]
		 * @param subFrameWidth Width of the sub-frame in pixel, with range [1, min(sourceWidth - sourceLeft, targetWidth - targetLeft)]
		 * @param subFrameHeight Height of the sub-frame in pixel, with range [1, min(sourceHeight - sourceTop, targetHeight - targetTop)]
		 * @param sourcePaddingElements Optional number of padding elements at the end of each source row, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, with range [0, infinity)
		 * @param maskPaddingElements Optional number of padding elements at the end of each source mask row, with range [0, infinity)
		 * @param maskValue Optional value which indicates which pixel value should be interpreted as the foreground (and copied)
		 * @return True, if succeeded
		 * @tparam T The data type of each element
		 */
		template <typename T>
		static bool validateSubFrameMask(const unsigned int channels, const T* source, const unsigned int sourceWidth, const unsigned int sourceHeight, const T* originalTarget, const T* target, const unsigned int targetWidth, const unsigned int targetHeight, const uint8_t* mask, const unsigned int sourceLeft, const unsigned int sourceTop, const unsigned int targetLeft, const unsigned int targetTop, const unsigned int subFrameWidth, const unsigned int subFrameHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int maskPaddingElements, const uint8_t maskValue);

		/**
		 * Returns whether an image type can be converted to another image type without needing to create a copy of the image.
		 * @param sourceType The source image type, must be valid
		 * @param targetType The target image type, must be valid
		 * @return True, if soc
		 */
		static bool canBeConvertedWithoutCopy(const FrameType& sourceFrameType, const FrameType& targetFrameType);

		/**
		 * Returns whether a given pixel format contains a grayscale channel.
		 * @param pixelFormat The pixel format to be checked, must be valid
		 * @return True, if so
		 */
		static bool containsGrayscaleChannel(const FrameType::PixelFormat pixelFormat);
};

} // namespace TestCV

} // namespace Test

} // namespace Ocean

#endif // META_OCEAN_TEST_TESTCV_TEST_FRAME_CONVERTER_H
