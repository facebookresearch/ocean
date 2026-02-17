/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testnetwork/TestData.h"

#include "ocean/test/TestResult.h"

#include "ocean/base/RandomGenerator.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/network/Data.h"

#include "ocean/test/Validation.h"

namespace Ocean
{

namespace Test
{

namespace TestNetwork
{

bool TestData::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("Data test");
	Log::info() << " ";

	if (selector.shouldRun("endian"))
	{
		testResult = testEndian(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
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

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		testToBigEndian<int16_t>(validation, randomGenerator);
		testToBigEndian<uint16_t>(validation, randomGenerator);
		testToBigEndian<int32_t>(validation, randomGenerator);
		testToBigEndian<uint32_t>(validation, randomGenerator);

		testFromBigEndian<int16_t>(validation, randomGenerator);
		testFromBigEndian<uint16_t>(validation, randomGenerator);
		testFromBigEndian<int32_t>(validation, randomGenerator);
		testFromBigEndian<uint32_t>(validation, randomGenerator);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
void TestData::testToBigEndian(Validation& validation, RandomGenerator& randomGenerator)
{
#ifdef OCEAN_LITTLE_ENDIAN

	const T value = T(RandomI::random32(randomGenerator));

	const T bigEndianValue = Network::Data::toBigEndian(value);

	OCEAN_EXPECT_EQUAL(validation, bigEndianValue, flipByteOrder(value));

#else

	const T value = T(RandomI::random32(randomGenerator));

	const T bigEndianValue = Network::Data::toBigEndian(value);

	OCEAN_EXPECT_EQUAL(validation, bigEndianValue, value);

#endif // OCEAN_LITTLE_ENDIAN
}

template <typename T>
void TestData::testFromBigEndian(Validation& validation, RandomGenerator& randomGenerator)
{
#ifdef OCEAN_LITTLE_ENDIAN

	const T bigEndianValue = T(RandomI::random32(randomGenerator));

	const T value = Network::Data::fromBigEndian(bigEndianValue);

	OCEAN_EXPECT_EQUAL(validation, value, flipByteOrder(bigEndianValue));

#else

	const T bigEndianValue = T(RandomI::random32(randomGenerator));

	const T value = Network::Data::fromBigEndian(value);

	OCEAN_EXPECT_EQUAL(validation, value, bigEndianValue);

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
