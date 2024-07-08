/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTBASE_TEST_FRAME_H
#define META_OCEAN_TEST_TESTBASE_TEST_FRAME_H

#include "ocean/test/testbase/TestBase.h"

#include "ocean/base/Frame.h"
#include "ocean/base/RandomGenerator.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

/**
 * This class implements tests for the Frame class.
 * @ingroup testbase
 */
class OCEAN_TEST_BASE_EXPORT TestFrame
{
	public:

		/**
		 * Invokes all tests.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Test ensuring that the function returning all data types is correct.
		 * @return True, if so
		 */
		static bool testDefinedDataTypes();

		/**
		 * Test ensuring that the function returning all defined pixel formats is correct.
		 * @return True, if so
		 */
		static bool testDefinedPixelFormats();

		/**
		 * Tests the Plane constructors.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPlaneContructors(const double testDuration);

		/**
		 * Tests the Plane copy constructors.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPlaneCopyContructors(const double testDuration);

		/**
		 * Tests the whether the frame specifications for generic pixel formats are correct.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testFrameSpecificationGenericPixelFormats(const double testDuration);

		/**
		 * Tests the whether the frame specifications for non-generic pixel formats are correct.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testFrameSpecificationNonGenericPixelFormats(const double testDuration);

		/**
		 * Tests all constructors.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testConstructor(const double testDuration);

		/**
		 * Tests all copy constructors.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testCopyConstructor(const double testDuration);

		/**
		 * Tests all move constructors.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testMoveConstructor(const double testDuration);

		/**
		 * Tests all copy operators.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testCopyOperator(const double testDuration);

		/**
		 * Tests the initializer for planes.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPlaneInitializer(const double testDuration);

		/**
		 * Tests the plane layout function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPlaneLayout(const double testDuration);

		/**
		 * Tests the release function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testRelease(const double testDuration);

		/**
		 * Tests the extraction of a sub-frame.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSubFrame(const double testDuration);

		/**
		 * Tests timestamps.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testTimestamp(const double testDuration);

		/**
		 * Tests the row and pixel accessor function based on data types.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testAccessorsDataTypes(const double testDuration);

		/**
		 * Tests the row and pixel accessor function based on pixel formats.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testAccessorsPixelFormats(const double testDuration);

		/**
		 * Tests the set function for frame types.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSetFrameType(const double testDuration);

		/**
		 * Tests the legacy copy function of the entire image.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testLegacyCopy(const double testDuration);

		/**
		 * Tests the copy function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testCopy(const double testDuration);

		/**
		 * Tests the make continuous function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testMakeContinuous(const double testDuration);

		/**
		 * Tests the plane bits per pixel function.
		 * @return True, if succeeded
		 */
		static bool testPlaneBytesPerPixel();

		/**
		 * Tests the setValue functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSetValue(const double testDuration);

		/**
		 * Tests the containsValue functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testContainsValue(const double testDuration);

		/**
		 * Tests the hasTransparentPixel function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testHasTransparentPixel(const double testDuration);

		/**
		 * Tests the function calculating padding elements based on stride bytes.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testStrideBytes2paddingElements(const double testDuration);

		/**
		 * Tests several generic pixel format functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testHaveIntersectingMemory(const double testDuration);

		/**
		 * Tests the update memory functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testUpdateMemory(const double testDuration);

		/**
		 * Tests the formatIsPacked() function.
		 * @return True, if succeeded
		 */
		static bool testFormatIsPacked();

		/**
		 * Tests the translate data type functions.
		 * @return True, if succeeded
		 */
		static bool testTranslateDataType();

		/**
		 * Tests the translate pixel format functions.
		 * @return True, if succeeded
		 */
		static bool testTranslatePixelFormat();

	protected:

		/**
		 * Tests the copy function.
		 * @param pixelFormat The pixel format to be tested, must be valid
		 * @param randomGenerator The random generator to be used
		 * @return True, if succeeded
		 * @tparam T The data type of each pixel element
		 */
		template <typename T>
		static bool testCopy(const FrameType::PixelFormat pixelFormat, RandomGenerator& randomGenerator);

		/**
		 * Tests the Plane constructors for a specified resolution, channel number and element type.
		 * @param width The width of the plane in pixels, with range [1, infinity)
		 * @param height The height of the plane in pixels, with range [1, infinity)
		 * @param channels The of channels the plane has, with range [1, infinity)
		 * @param paddingElements The number of padding elements at the end of each row, in elements, with range [0, infinity)
		 * @return True, if succeeded
		 * @tparam T The element data type
		 */
		template <typename T>
		static bool testPlaneContructors(const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int paddingElements);

		/**
		 * Tests the Plane copy constructors for a specified resolution, channel number and element type.
		 * @param width The width of the plane in pixels, with range [1, infinity)
		 * @param height The height of the plane in pixels, with range [1, infinity)
		 * @param channels The of channels the plane has, with range [1, infinity)
		 * @param paddingElements The number of padding elements at the end of each row, in elements, with range [0, infinity)
		 * @return True, if succeeded
		 * @tparam T The element data type
		 */
		template <typename T>
		static bool testPlaneCopyContructors(const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int paddingElements);

		/**
		 * Validates the initializer for one plane.
		 * @param frameType The frame type of the image to be used, must be valid
		 * @param randomGenerator The random generator object to be used
		 * @return True, if succeeded
		 * @tparam T The element data type
		 */
		template <typename T>
		static bool validatePlaneInitializer(const FrameType& frameType, RandomGenerator& randomGenerator);

		/**
		 * Validates whether a given frame with multiple planes has the correct specification.
		 * @param frame The frame to be checked, must be valid
		 * @param frameType The type of the frame to check, must be valid
		 * @param paddingElementsPerPlane The number of padding elements for each individual plane, with ranges [0, infinity)
		 * @param bytesPerElement The number of bytes per elements, with range [1, infinity)
		 * @param isOwner True, if the frame should be the owner of the memory; False, otherwise
		 * @param isReadOnly True, if the frame should have access to read-only memory; False, if the frame has should have access to writable memory
		 * @return True, if the frame matches the given specification
		 */
		static bool validateFrameSpecification(const Frame& frame, const FrameType& frameType, const Indices32& paddingElementsPerPlane, const unsigned int bytesPerElement, const bool isOwner, const bool isReadOnly);

		/**
		 * Validates whether a given frame with one plane has the correct specification.
		 * @param frame The frame to be checked, must be valid
		 * @param frameType The type of the frame to check, must be valid
		 * @param paddingElements The number of padding elements for the one and only plane, with range [0, infinity)
		 * @param bytesPerElement The number of bytes per elements, with range [1, infinity)
		 * @param isOwner True, if the frame should be the owner of the memory; False, otherwise
		 * @param isReadOnly True, if the frame should have access to read-only memory; False, if the frame has should have access to writable memory
		 * @return True, if the frame matches the given specification
		 */
		static inline bool validateFrameSpecification(const Frame& frame, const FrameType& frameType, const unsigned int paddingElements, const unsigned int bytesPerElement, const bool isOwner, const bool isReadOnly);

		/**
		 * Tests the setValue functions.
		 * @param frame The frame to be used for testing, must be valid
		 * @param randomGenerator The random generator to be used
		 * @return True, if succeeded
		 * @tparam T The data type of the frame's elements
		 */
		template <typename T>
		static bool testSetValue(const Frame& frame, RandomGenerator& randomGenerator);

		/**
		 * Returns all defined pixel formats.
		 * @param genericPixelFormats Optional custom generic pixel formats which will be added to the resulting pixel formats
		 * @return Ocean's defined pixel formats
		 */
		static FrameType::PixelFormats definedPixelFormats(const FrameType::PixelFormats& genericPixelFormats = FrameType::PixelFormats());

		/**
		 * Returns a random frame type.
		 * @param pixelFormats The pixel formats which can be used for the frame type, at least one
		 * @param randomGenerator Optional random generator object to be used
		 */
		static FrameType randomizedFrameType(const FrameType::PixelFormats& pixelFormats, RandomGenerator* randomGenerator = nullptr);
};

inline bool TestFrame::validateFrameSpecification(const Frame& frame, const FrameType& frameType, const unsigned int paddingElements, const unsigned int bytesPerElement, const bool isOwner, const bool isReadOnly)
{
	ocean_assert(frame.numberPlanes() == 1u);

	return validateFrameSpecification(frame, frameType, Indices32(1u, paddingElements), bytesPerElement, isOwner, isReadOnly);
}

}

}

}

#endif // META_OCEAN_TEST_TESTBASE_TEST_FRAME_H
