// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_TEST_TESTBASE_TEST_LEGACY_FRAME_H
#define META_OCEAN_TEST_TESTBASE_TEST_LEGACY_FRAME_H

#include "ocean/test/testbase/TestBase.h"

#include "ocean/base/Frame.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

/**
 * This class implements a test for the LegacyFrame class.
 * @ingroup testbase
 */
class OCEAN_TEST_BASE_EXPORT TestLegacyFrame : protected LegacyFrame
{
	public:

		/**
		 * Tests the frame functionalities.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True if the test succeeded; otherwise, false is returned.
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the data access functions row() and pixel().
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testRowPixel(const double testDuration);

		/**
		 * Tests the functionality of all Frame::set() variants.
		 * @return True if the test succeeded; otherwise, false is returned.
		 */
		static bool testSet();

		/**
		 * Tests the size function.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSize(const double testDuration);

		/**
		 * Tests several generic pixel format functions.
		 * @return True, if succeeded
		 */
		static bool testGenericPixelFormat();

		/**
		 * Tests the number of channel functionality.
		 * @return True, if succeeded
		 */
		static bool testChannelNumber();

		/**
		 * Tests the width multiple functionality.
		 * @return True, if succeeded
		 */
		static bool testWidthMultiple();

		/**
		 * Tests the height multiple functionality.
		 * @return True, if succeeded
		 */
		static bool testHeightMultiple();

		/**
		 * Tests the plane number functionality.
		 * @return True, if succeeded
		 */
		static bool testNumberPlanes();

		/**
		 * Tests the function checking whether two frames have an intersecting memory.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testHaveIntersectingMemory(const double testDuration);

		/**
		 * Tests that all defined pixel formats are composed of a unique value.
		 * @return True, if succeeded
		 */
		static bool testPixelFormatUniqueness();

		/**
		 * Tests the arePixelFormatsCompatible() function.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testArePixelFormatsCompatible(const double testDuration);

		/**
		 * Tests the translate pixel format functions.
		 * @return True, if succeeded
		 */
		static bool testTranslatePixelFormat();

		/**
		 * Tests the move constructor to a Frame objects.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testMoveConstructorFrame(const double testDuration);

		/**
		 * Tests the copy constructor to a Frame objects.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testCopyConstructorFrame(const double testDuration);

	private:

		/**
		 * Tests the functionality of Frame::set(const FrameType& type, const bool forceOwner, const bool forceWritable).
		 * @param templateFrame Template frame
		 * @param frameType Frame type passed to Frame::set()
		 * @param forceOwner Force owner flag passed to Frame::set()
		 * @param forceWritable Force writable flag passed to Frame::set()
		 * @param isValidExpected Specifies if the frame is expected to be valid after the call to Frame::set()
		 * @param isOwnerExpected Specifies if the frame is expected to owner of its frame data after the call to Frame::set()
		 * @param isReadonlyExpected Specifies if the frame is expected to be read-only after the call to Frame::set()
		 * @param timestampExpected Specifies the expected timestamp after the call to Frame::set()
		 * @param sizeExpected Specifies the expected size after the call to Frame::set()
		 * @return True if the test succeeded; otherwise, false is returned.
		 */
		static bool testSet(const LegacyFrame& templateFrame, const FrameType& frameType, const bool forceOwner, const bool forceWritable, const bool isValidExpected, const bool isOwnerExpected, const bool isReadonlyExpected, const Timestamp& timestampExpected,  const unsigned int sizeExpected);

		/**
		 * Tests the functionality of Frame::set(const FrameType& type).
		 * @param templateFrame Template frame
		 * @param frameType Frame type passed to Frame::set()
		 * @param isValidExpected Specifies if the frame is expected to be valid after the call to Frame::set()
		 * @param isOwnerExpected Specifies if the frame is expected to owner of its frame data after the call to Frame::set()
		 * @param isReadonlyExpected Specifies if the frame is expected to be read-only after the call to Frame::set()
		 * @param timestampExpected Specifies the expected timestamp after the call to Frame::set()
		 * @param sizeExpected Specifies the expected size after the call to Frame::set()
		 * @return True if the test succeeded; otherwise, false is returned.
		 */
		static bool testSet(const LegacyFrame& templateFrame, const FrameType& frameType, const bool isValidExpected, const bool isOwnerExpected, const bool isReadonlyExpected, const Timestamp& timestampExpected, const unsigned int sizeExpected);

		/**
		 * Tests the functionality of Frame::set(const FrameType& type, const Timestamp timestamp, const unsigned char* data, const bool copyData) and set(const FrameType& type, const Timestamp timestamp, unsigned char* data, const bool copyData).
		 * @param templateFrame Template frame
		 * @param frameType Frame type passed to Frame::set()
		 * @param timestamp Timestamp passed to Frame::set()
		 * @param dataFrame Frame data passed to Frame::set()
		 * @param passConstData Specifies if the frame data is passed as const or non-const pointer
		 * @param copyData Copy data flag passed to Frame::set()
		 * @param isValidExpected Specifies if the frame is expected to be valid after the call to Frame::set()
		 * @param isOwnerExpected Specifies if the frame is expected to owner of its frame data after the call to Frame::set()
		 * @param isReadonlyExpected Specifies if the frame is expected to be read-only after the call to Frame::set()
		 * @param sizeExpected Specifies the expected size after the call to Frame::set()
		 * @return True if the test succeeded; otherwise, false is returned.
		 */
		static bool testSet(const LegacyFrame& templateFrame, const FrameType& frameType, const Timestamp& timestamp, LegacyFrame& dataFrame, const bool passConstData, const bool copyData, const bool isValidExpected, const bool isOwnerExpected, const bool isReadonlyExpected, const unsigned int sizeExpected);

		/**
		 * Copies the specified frame and all of its properties.
		 * If the frame owns its frame data then the copied frame will own a copy of this frame data and frameCopy.data() will be not equal to frame.data().
		 * If the frame does not own its frame data, frameCopy will refer to the same frame data and frame.data() will be equal to frameCopy.data().
		 * @param frame The frame that is copied
		 * @param frameCopy Receives the frame copy
		 */
		static void copyFrame(const LegacyFrame& frame, LegacyFrame& frameCopy);

		/**
		 * Checks if the specified frame has the specified properties
		 * @param frame The frame to be validated
		 * @param isValid True if the frame should be valid, otherwise false
		 * @param isOwner True if the frame should own its data, otherwise false
		 * @param isReadonly True if the frame should be read-only, otherwise false
		 * @param timestamp Expected timestamp of the frame
		 * @param size Expected size of the frame
		 * @return True if the frame has the specified properties ; otherwise, false is returned.
		 */
		static bool validateFrame(const LegacyFrame& frame, const bool isValid, const bool isOwner, const bool isReadonly, const Timestamp& timestamp, const unsigned int size);

		/**
		 * Checks if the specified frame has the expected frame data
		 * @param frame The frame of which the frame data is validated
		 * @param expectedFrame Frame that contains the expected frame data
		 * @return True if the frame contains the expected frame data; otherwise, false is returned.
		 */
		static bool validateFrameData(const LegacyFrame& frame, const LegacyFrame& expectedFrame);

		/**
		 * Returns a list of specified pixel formats.
		 * @return All pixel formats specified in Ocean
		 */
		static std::vector<FrameType::PixelFormat> specifiedPixelFormats();

		/**
		 * Returns the average bits per pixel of a pixel format.
		 * Pixel formats with several planes may have planes with different bits per plane pixel so that the result is an average.
		 * @return The number of bits per pixel in average
		 */
		static unsigned int averageBitsPerPixel(const FrameType::PixelFormat pixelFormat);
};

}

}

}

#endif // META_OCEAN_TEST_TESTBASE_TEST_LEGACY_FRAME_H
