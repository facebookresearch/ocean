/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifdef OCEAN_USE_GTEST

#include <gtest/gtest.h>

#include "ocean/base/Frame.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/Timestamp.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameFilterMin.h"

#include "ocean/math/Numeric.h"

#include <algorithm>
#include <cstdint>
#include <tuple>

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace
{

// Naive reference: computes the minimum over the square patch of side `filterSize`
// centered at (x, y), clamped to the frame extents (matching the production behavior
// which processes only in-bounds source pixels).
template <typename T>
T referenceMinAt(const Frame& source, const unsigned int x, const unsigned int y, const unsigned int channel, const unsigned int filterSize)
{
	const int filterRadius = int(filterSize / 2u);
	T result = NumericT<T>::maxValue();

	for (int yy = -filterRadius; yy <= filterRadius; ++yy)
	{
		const int yPos = int(y) + yy;
		if (yPos < 0 || yPos >= int(source.height()))
		{
			continue;
		}

		for (int xx = -filterRadius; xx <= filterRadius; ++xx)
		{
			const int xPos = int(x) + xx;
			if (xPos < 0 || xPos >= int(source.width()))
			{
				continue;
			}

			const T value = source.constpixel<T>(static_cast<unsigned int>(xPos), static_cast<unsigned int>(yPos))[channel];
			result = std::min(result, value);
		}
	}

	return result;
}

template <typename T>
::testing::AssertionResult verifyMinAgainstReference(const Frame& source, const Frame& target, const unsigned int filterSize)
{
	if (!source.isFrameTypeCompatible(target, false))
	{
		return ::testing::AssertionFailure() << "source and target frame types differ";
	}

	for (unsigned int y = 0u; y < source.height(); ++y)
	{
		for (unsigned int x = 0u; x < source.width(); ++x)
		{
			for (unsigned int n = 0u; n < source.channels(); ++n)
			{
				const T expected = referenceMinAt<T>(source, x, y, n, filterSize);
				const T actual = target.constpixel<T>(x, y)[n];

				if (expected != actual)
				{
					return ::testing::AssertionFailure()
						<< "mismatch at (" << x << ", " << y << ", ch " << n << "): expected "
						<< double(expected) << ", got " << double(actual);
				}
			}
		}
	}

	return ::testing::AssertionSuccess();
}

constexpr unsigned int kTestWidth = 10u;
constexpr unsigned int kTestHeight = 10u;
constexpr unsigned int kFilterSize = 3u;

} // namespace

TEST(FrameFilterMinComfort, InvalidTargetIsAutoAllocated)
{
	RandomGenerator randomGenerator;
	const Frame source = CV::CVUtilities::randomizedFrame(
		FrameType(kTestWidth, kTestHeight, FrameType::genericPixelFormat<uint8_t, 3u>(), FrameType::ORIGIN_UPPER_LEFT),
		&randomGenerator);
	ASSERT_TRUE(source.isValid());

	Frame target;
	ASSERT_FALSE(target.isValid());

	EXPECT_TRUE(CV::FrameFilterMin::Comfort::filter(source, target, kFilterSize, nullptr));

	ASSERT_TRUE(target.isValid());
	EXPECT_TRUE(source.isFrameTypeCompatible(target, false));
	EXPECT_TRUE(verifyMinAgainstReference<uint8_t>(source, target, kFilterSize));
}

class FrameFilterMinComfortChannelsFixture
    : public ::testing::TestWithParam<std::tuple<FrameType::DataType, unsigned int>>
{
};

TEST_P(FrameFilterMinComfortChannelsFixture, ComputesMinimumMatchingNaiveReference)
{
	const auto [dataType, channels] = GetParam();

	RandomGenerator randomGenerator;
	const Frame source = CV::CVUtilities::randomizedFrame(
		FrameType(kTestWidth, kTestHeight, FrameType::genericPixelFormat(dataType, channels), FrameType::ORIGIN_UPPER_LEFT),
		&randomGenerator);
	Frame target = CV::CVUtilities::randomizedFrame(source.frameType(), &randomGenerator);

	EXPECT_TRUE(CV::FrameFilterMin::Comfort::filter(source, target, kFilterSize, nullptr));

	if (dataType == FrameType::DT_UNSIGNED_INTEGER_8)
	{
		EXPECT_TRUE(verifyMinAgainstReference<uint8_t>(source, target, kFilterSize));
	}
	else
	{
		ASSERT_EQ(dataType, FrameType::DT_SIGNED_FLOAT_32);
		EXPECT_TRUE(verifyMinAgainstReference<float>(source, target, kFilterSize));
	}
}

INSTANTIATE_TEST_SUITE_P(
	SupportedTypesAndChannels,
	FrameFilterMinComfortChannelsFixture,
	::testing::Values(
		std::make_tuple(FrameType::DT_UNSIGNED_INTEGER_8, 2u),
		std::make_tuple(FrameType::DT_UNSIGNED_INTEGER_8, 4u),
		std::make_tuple(FrameType::DT_SIGNED_FLOAT_32, 2u),
		std::make_tuple(FrameType::DT_SIGNED_FLOAT_32, 4u)));

TEST(FrameFilterMinComfort, InPlaceOverloadMatchesOutOfPlaceAndPreservesTimestamps)
{
	RandomGenerator randomGenerator;
	Frame source = CV::CVUtilities::randomizedFrame(
		FrameType(kTestWidth, kTestHeight, FrameType::genericPixelFormat<uint8_t, 3u>(), FrameType::ORIGIN_UPPER_LEFT),
		&randomGenerator);

	const Timestamp expectedTimestamp(123.5);
	const Timestamp expectedRelativeTimestamp(4.25);
	source.setTimestamp(expectedTimestamp);
	source.setRelativeTimestamp(expectedRelativeTimestamp);

	// Keep an untouched copy of the source pixels to feed into the reference implementation.
	const Frame originalSource(source, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

	EXPECT_TRUE(CV::FrameFilterMin::Comfort::filter(source, kFilterSize, nullptr));

	EXPECT_EQ(source.timestamp(), expectedTimestamp);
	EXPECT_EQ(source.relativeTimestamp(), expectedRelativeTimestamp);
	EXPECT_TRUE(verifyMinAgainstReference<uint8_t>(originalSource, source, kFilterSize));
}

// The remaining tests trigger the intentional `ocean_assert(false)` branches inside
// `Comfort::filter`. Those assertions abort in debug builds, so we guard the tests to
// release builds where the function returns `false` gracefully.
#ifndef OCEAN_DEBUG

TEST(FrameFilterMinComfort, InvalidSourceReturnsFalse)
{
	const Frame invalidSource;
	ASSERT_FALSE(invalidSource.isValid());

	Frame target;
	EXPECT_FALSE(CV::FrameFilterMin::Comfort::filter(invalidSource, target, kFilterSize, nullptr));
	EXPECT_FALSE(target.isValid());
}

TEST(FrameFilterMinComfort, IncompatibleTargetReturnsFalse)
{
	RandomGenerator randomGenerator;
	const Frame source = CV::CVUtilities::randomizedFrame(
		FrameType(kTestWidth, kTestHeight, FrameType::genericPixelFormat<uint8_t, 3u>(), FrameType::ORIGIN_UPPER_LEFT),
		&randomGenerator);

	// Different channel count -> incompatible.
	Frame target = CV::CVUtilities::randomizedFrame(
		FrameType(kTestWidth, kTestHeight, FrameType::genericPixelFormat<uint8_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT),
		&randomGenerator);

	EXPECT_FALSE(CV::FrameFilterMin::Comfort::filter(source, target, kFilterSize, nullptr));
}

TEST(FrameFilterMinComfort, UnsupportedDataTypeReturnsFalse)
{
	RandomGenerator randomGenerator;
	// `uint16_t` frames fall into the `default` branch of the data-type switch.
	const Frame source = CV::CVUtilities::randomizedFrame(
		FrameType(kTestWidth, kTestHeight, FrameType::genericPixelFormat<uint16_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT),
		&randomGenerator);
	Frame target;

	EXPECT_FALSE(CV::FrameFilterMin::Comfort::filter(source, target, kFilterSize, nullptr));
}

class FrameFilterMinComfortUnsupportedChannelsFixture
    : public ::testing::TestWithParam<FrameType::DataType>
{
};

TEST_P(FrameFilterMinComfortUnsupportedChannelsFixture, ReturnsFalse)
{
	const FrameType::DataType dataType = GetParam();

	RandomGenerator randomGenerator;
	// 5 channels falls into the `default` branch of the channel-count switch.
	const Frame source = CV::CVUtilities::randomizedFrame(
		FrameType(kTestWidth, kTestHeight, FrameType::genericPixelFormat(dataType, 5u), FrameType::ORIGIN_UPPER_LEFT),
		&randomGenerator);
	Frame target;

	EXPECT_FALSE(CV::FrameFilterMin::Comfort::filter(source, target, kFilterSize, nullptr));
}

INSTANTIATE_TEST_SUITE_P(
	AllSupportedDataTypes,
	FrameFilterMinComfortUnsupportedChannelsFixture,
	::testing::Values(FrameType::DT_UNSIGNED_INTEGER_8, FrameType::DT_SIGNED_FLOAT_32));

TEST(FrameFilterMinComfort, InPlaceOverloadReturnsFalseForInvalidFrame)
{
	// Verifies the failure-propagation branch of the in-place overload:
	// when the inner two-argument `filter` fails, the in-place overload must
	// propagate the failure without mutating the frame.
	Frame invalidFrame;
	ASSERT_FALSE(invalidFrame.isValid());

	EXPECT_FALSE(CV::FrameFilterMin::Comfort::filter(invalidFrame, kFilterSize, nullptr));
	EXPECT_FALSE(invalidFrame.isValid());
}

#endif // OCEAN_DEBUG

} // namespace TestCV

} // namespace Test

} // namespace Ocean

#endif // OCEAN_USE_GTEST
