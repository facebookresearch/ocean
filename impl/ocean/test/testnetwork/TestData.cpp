/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testnetwork/TestData.h"

#include "ocean/base/RandomGenerator.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/network/Data.h"

namespace Ocean
{

namespace Test
{

namespace TestNetwork
{

bool TestData::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "---   Data test:   ---";
	Log::info() << " ";

	allSucceeded = testEndian(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Data test succeeded.";
	}
	else
	{
		Log::info() << "Data test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestData, Endian)
{
	EXPECT_TRUE(TestData::testEndian(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestData::testEndian(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Endian conversion test:";

	bool allSucceeded = true;
	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		if (!testToBigEndian<int16_t>(randomGenerator))
		{
			allSucceeded = false;
		}

		if (!testToBigEndian<uint16_t>(randomGenerator))
		{
			allSucceeded = false;
		}

		if (!testToBigEndian<int32_t>(randomGenerator))
		{
			allSucceeded = false;
		}

		if (!testToBigEndian<uint32_t>(randomGenerator))
		{
			allSucceeded = false;
		}


		if (!testFromBigEndian<int16_t>(randomGenerator))
		{
			allSucceeded = false;
		}

		if (!testFromBigEndian<uint16_t>(randomGenerator))
		{
			allSucceeded = false;
		}

		if (!testFromBigEndian<int32_t>(randomGenerator))
		{
			allSucceeded = false;
		}

		if (!testFromBigEndian<uint32_t>(randomGenerator))
		{
			allSucceeded = false;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

template <typename T>
bool TestData::testToBigEndian(RandomGenerator& randomGenerator)
{
#ifdef OCEAN_LITTLE_ENDIAN

	const T value = T(RandomI::random32(randomGenerator));

	const T bigEndianValue = Network::Data::toBigEndian(value);

	if (bigEndianValue != flipByteOrder(value))
	{
		return false;
	}

	return true;

#else

	const T value = T(RandomI::random32(randomGenerator));

	const T bigEndianValue = Network::Data::toBigEndian(value);

	if (bigEndianValue != value)
	{
		return false;
	}

	return true;

#endif // OCEAN_LITTLE_ENDIAN
}

template <typename T>
bool TestData::testFromBigEndian(RandomGenerator& randomGenerator)
{
#ifdef OCEAN_LITTLE_ENDIAN

	const T bigEndianValue = T(RandomI::random32(randomGenerator));

	const T value = Network::Data::fromBigEndian(bigEndianValue);

	if (value != flipByteOrder(bigEndianValue))
	{
		return false;
	}

	return true;

#else

	const T bigEndianValue = T(RandomI::random32(randomGenerator));

	const T value = Network::Data::fromBigEndian(value);

	if (value != bigEndianValue)
	{
		return false;
	}

	return true;

#endif // OCEAN_LITTLE_ENDIAN
}

template <typename T>
T TestData::flipByteOrder(const T& value)
{
	constexpr size_t size = sizeof(T);

	uint8_t byteArray[size];
	memcpy(byteArray, &value, size);

	for (size_t n = 0; n < size / 2; ++n)
	{
		std::swap(byteArray[n], byteArray[size - n - 1]);
	}

	T flippedValue;
	memcpy(&flippedValue, byteArray, size);

	return flippedValue;
}

}

}

}
