/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTADVANCED_TEST_FRAME_COLOR_ADJUSTMENT_H
#define META_OCEAN_TEST_TESTCV_TESTADVANCED_TEST_FRAME_COLOR_ADJUSTMENT_H

#include "ocean/test/testcv/testadvanced/TestCVAdvanced.h"

#include "ocean/base/RandomGenerator.h"
#include "ocean/base/StaticBuffer.h"
#include "ocean/base/Worker.h"

#include "ocean/math/Lookup2.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestAdvanced
{

/**
 * This class implements a frame color adjustment test.
 * @ingroup testcvadvanced
 */
class OCEAN_TEST_CV_ADVANCED_EXPORT TestFrameColorAdjustment
{
	public:

		/**
		 * This class extends the static buffer class by mathematical operators.
		 */
		template <size_t tCapacity>
		class Object : public StaticBuffer<Scalar, tCapacity>
		{
			public:

				/**
				 * Element-wise add operator.
				 * @param object The object to be added
				 * @return Sum result
				 */
				inline Object operator+(const Object& object) const;

				/**
				 * Element-wise subtract operator.
				 * @param object The object to be subtracted
				 * @return Subtraction result
				 */
				inline Object operator-(const Object& object) const;

				/**
				 * Scalar multiplication operator.
				 * @param factor Multiplication factor
				 * @return Multiplication result
				 */
				inline Object operator*(const Scalar factor) const;
		};

		/**
		 * Definition of a lookup table holding objects.
		 */
		template <unsigned int tChannels>
		using ObjectLookupCenter2 = LookupCenter2<Object<tChannels>>;

	public:

		/**
		 * Tests all frame color adjustment functions.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the default adjustment function without mask pixels.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 * @tparam tChannels The number of data channels for the frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static bool testAdjustmentNoMask(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the adjustment function with mask pixels.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 * @tparam tChannels The number of data channels for the frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static bool testAdjustmentWithMask(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

	protected:

		/**
		 * Creates a modification table with given size and maximal offset value.
		 * @param sizeX The horizontal size of the table, with range [1, infinity)
		 * @param sizeY The vertical size of the table, with range [1, infinity)
		 * @param binsX The number of horizontal bins, with range [1, sizeX]
		 * @param binsY The number of vertical bins, with range [1, sizeY]
		 * @param minimalOffset The minimal offset for each channel, with range [-255, 255)
		 * @param maximalOffset The maximal offset for each channel, with range (minimalOffset, 255]
		 * @param randomGenerator The random generator to be used
		 * @return The resulting lookup table
		 * @tparam tChannels The number of data channels, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static ObjectLookupCenter2<tChannels> modificationTable(const unsigned int sizeX, const unsigned int sizeY, const unsigned int binsX, const unsigned int binsY, const Scalar minimalOffset, const Scalar maximalOffset, RandomGenerator& randomGenerator);

		/**
		 * Modifies a given frame by adding component-wise values from a given lookup table.
		 * @param source The source frame providing the image content, must be valid
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param lookupTable The lookup table providing the offset values which will be added
		 * @param target The target frame receiving the modified source frame, must be valid
		 * @param targetPaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @tparam tChannels The number of data channels for the frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void modifyFrame8BitPerChannel(const uint8_t* source, const unsigned int sourcePaddingElements, const ObjectLookupCenter2<tChannels>& lookupTable, uint8_t* target, const unsigned int targetPaddingElements);

		/**
		 * Returns the average color difference between two frames.
		 * @param frame0 The first frame, must be valid
		 * @param mask0 Optional mask defining valid and invalid pixels in the first frame
		 * @param frame1 The second frame, with same frame type as the first frame, must be valid
		 * @param mask1 Optional mask defining valid and invalid pixels in the second frame
		 * @param maskValue Mask value defining valid pixels, must be valid
		 * @return The average pixel difference, with range [0, infinity)
		 * @tparam tChannels The number of data channels for the frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static Scalar averageDifference(const Frame& frame0, const Frame& mask0, const Frame& frame1, const Frame& mask1, const uint8_t maskValue = 0xFFu);

		/**
		 * Sets random mask values and adds pepper at the same positions in a corresponding frame.
		 * @param frame The frame to which the pepper will be added
		 * @param mask The mask receiving the mask pixels
		 * @param number The number of mask pixels which will be set
		 * @param randomGenerator The random generator to be used
		 * @param value The value which will be set to the mask
		 * @tparam tChannels The number of data channels for the frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void randomMask(Frame& frame, Frame& mask, const unsigned int number, RandomGenerator& randomGenerator, const uint8_t value = 0x00u);
};

template <size_t tCapacity>
TestFrameColorAdjustment::Object<tCapacity> TestFrameColorAdjustment::Object<tCapacity>::operator+(const Object& object) const
{
	TestFrameColorAdjustment::Object<tCapacity> result;

	for (size_t n = 0; n < tCapacity; ++n)
	{
		result[n] = (*this)[n] + object[n];
	}

	return result;
}

template <size_t tCapacity>
TestFrameColorAdjustment::Object<tCapacity> TestFrameColorAdjustment::Object<tCapacity>::operator-(const Object& object) const
{
	TestFrameColorAdjustment::Object<tCapacity> result;

	for (size_t n = 0; n < tCapacity; ++n)
	{
		result[n] = (*this)[n] - object[n];
	}

	return result;
}

template <size_t tCapacity>
TestFrameColorAdjustment::Object<tCapacity> TestFrameColorAdjustment::Object<tCapacity>::operator*(const Scalar factor) const
{
	TestFrameColorAdjustment::Object<tCapacity> result;

	for (size_t n = 0; n < tCapacity; ++n)
	{
		result[n] = (*this)[n] * factor;
	}

	return result;
}

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTADVANCED_TEST_FRAME_COLOR_ADJUSTMENT_H
