/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestRandomI.h"

#include "ocean/base/RandomGenerator.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/Utilities.h"

#include <numeric>
#include <random>

namespace Ocean
{

namespace Test
{

namespace TestBase
{

bool TestRandomI::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   RandomI test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const unsigned int initialSeed = randomGenerator.initialSeed();

	if (initialSeed != randomGenerator.seed())
	{
		allSucceeded = false;
	}

	allSucceeded = testDistribution32(randomGenerator, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDistribution64(randomGenerator, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDistributionSmallRange(randomGenerator, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDistributionLargeRange(randomGenerator, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testOneParameter(randomGenerator) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testTwoParameter(randomGenerator) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testThreeParameter(randomGenerator) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSeveralParameter(randomGenerator) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testRandomPair(randomGenerator, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testRandomTriple(randomGenerator, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testRandomBoolean(randomGenerator, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testRandomElementsVector(randomGenerator, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testRandomElementsInitializerList(randomGenerator, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testExtremeValueRange(randomGenerator) && allSucceeded;

	Log::info() << " ";

	{
		Log::info() << "Initial seed test:";

		if (initialSeed == randomGenerator.initialSeed())
		{
			Log::info() << "Validation: succeeded.";
		}
		else
		{
			Log::info() << "Validation: FAILED!";

			allSucceeded = false;
		}
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

} // namespace TestBase

/**
 * This class implements a simple instance for the GTest ensuring that
 * all tests have access to the same random randomGenerator object.
 */
class TestRandomI : public ::testing::Test
{
	protected:

		/**
		 * Sets up the test.
		 */
		void SetUp() override
		{
			// nothing to do here
		}

		/**
		 * Tears down the test.
		 */
		void TearDown() override
		{
			// nothing to do here
		}

	protected:

		/// The random randomGenerator object for all tests.
		RandomGenerator randomGenerator_;
};

TEST_F(TestRandomI, Initialize)
{
	RandomI::initialize();
	EXPECT_TRUE(RandomI::random(1000u) <= 1000u);
}

TEST_F(TestRandomI, Distribution32)
{
	const unsigned int initialSeed = randomGenerator_.initialSeed();

	EXPECT_TRUE(TestBase::TestRandomI::testDistribution32(randomGenerator_, GTEST_TEST_DURATION));

	EXPECT_TRUE(initialSeed == randomGenerator_.initialSeed());
}

TEST_F(TestRandomI, Distribution64)
{
	EXPECT_TRUE(TestBase::TestRandomI::testDistribution64(randomGenerator_, GTEST_TEST_DURATION));
}

TEST_F(TestRandomI, DistributionSmallRange)
{
	EXPECT_TRUE(TestBase::TestRandomI::testDistributionSmallRange(randomGenerator_, GTEST_TEST_DURATION));
}

TEST_F(TestRandomI, DistributionLargeRange)
{
	EXPECT_TRUE(TestBase::TestRandomI::testDistributionLargeRange(randomGenerator_, GTEST_TEST_DURATION));
}

TEST_F(TestRandomI, OneParameter)
{
	EXPECT_TRUE(TestBase::TestRandomI::testOneParameter(randomGenerator_));
}

TEST_F(TestRandomI, TwoParameter)
{
	EXPECT_TRUE(TestBase::TestRandomI::testTwoParameter(randomGenerator_));
}

TEST_F(TestRandomI, ThreeParameter)
{
	EXPECT_TRUE(TestBase::TestRandomI::testThreeParameter(randomGenerator_));
}

TEST_F(TestRandomI, SeveralParameter)
{
	EXPECT_TRUE(TestBase::TestRandomI::testSeveralParameter(randomGenerator_));
}

TEST_F(TestRandomI, RandomPair)
{
	EXPECT_TRUE(TestBase::TestRandomI::testRandomPair(randomGenerator_, GTEST_TEST_DURATION));
}

TEST_F(TestRandomI, RandomTriple)
{
	EXPECT_TRUE(TestBase::TestRandomI::testRandomTriple(randomGenerator_, GTEST_TEST_DURATION));
}

TEST_F(TestRandomI, RandomBoolean)
{
	EXPECT_TRUE(TestBase::TestRandomI::testRandomBoolean(randomGenerator_, GTEST_TEST_DURATION));
}

TEST_F(TestRandomI, RandomElementsVector)
{
	EXPECT_TRUE(TestBase::TestRandomI::testRandomElementsVector(randomGenerator_, GTEST_TEST_DURATION));
}

TEST_F(TestRandomI, RandomElementsInitializerList)
{
	EXPECT_TRUE(TestBase::TestRandomI::testRandomElementsInitializerList(randomGenerator_, GTEST_TEST_DURATION));
}

TEST_F(TestRandomI, ExtremeValueRange)
{
	EXPECT_TRUE(TestBase::TestRandomI::testExtremeValueRange(randomGenerator_));
}

namespace TestBase
{

#endif // OCEAN_USE_GTEST

bool TestRandomI::testDistribution32(RandomGenerator& randomGenerator, const double testDuration)
{
	constexpr unsigned int numberBits = 32u;

	Log::info() << "Random " << numberBits << " bit distribution test:";
	Log::info() << " ";

	bool allSucceeded = true;

	const double threshold = 0.01; // 1%

	{
		Indices64 bits(numberBits, 0ull);

		uint64_t iterations = 0u;

		const Timestamp startTimestamp(true);

		do
		{
			for (unsigned int n = 0u; n < 10000u; ++n)
			{
				const uint32_t value = RandomI::random32();

				for (unsigned int nBit = 0u; nBit < numberBits; ++nBit)
				{
					if (value & (1u << nBit))
					{
						++bits[nBit];
					}
				}

				++iterations;
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);

		const uint64_t perfectAverage = iterations / 2ull;

		uint64_t actualAverage = 0ull;
		for (const uint64_t bit : bits)
		{
			actualAverage += bit;
		}
		actualAverage /= uint64_t(bits.size());

		uint64_t maxDifference = 0u;
		for (const uint64_t bit : bits)
		{
			maxDifference = std::max(maxDifference, uint64_t(std::abs(int64_t(actualAverage) - int64_t(bit))));
		}

		ocean_assert(actualAverage != 0ull && perfectAverage != 0ull);

		if (actualAverage != 0ull && perfectAverage != 0ull)
		{
			const double percentActual = double(maxDifference) / double(actualAverage);
			const double percentPerfect = double(maxDifference) / double(perfectAverage);

			Log::info() << "Default, created values: " << iterations;
			Log::info() << "Maximal bit error to average: " << percentActual * 100.0 << "% (actual), " << percentPerfect * 100.0 << "% (perfect)";

			if (percentActual > threshold || percentPerfect > threshold)
			{
				allSucceeded = false;
			}
		}
		else
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	{
		Indices64 bits(numberBits, 0ull);

		uint64_t iterations = 0u;

		const Timestamp startTimestamp(true);

		do
		{
			for (unsigned int n = 0u; n < 10000u; ++n)
			{
				const uint32_t value = RandomI::random32(randomGenerator);

				for (unsigned int nBit = 0u; nBit < numberBits; ++nBit)
				{
					if (value & (1u << nBit))
					{
						++bits[nBit];
					}
				}

				++iterations;
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);

		const uint64_t perfectAverage = iterations / 2ull;

		uint64_t actualAverage = 0ull;
		for (const uint64_t bit : bits)
		{
			actualAverage += bit;
		}
		actualAverage /= uint64_t(bits.size());

		uint64_t maxDifference = 0u;
		for (const uint64_t bit : bits)
		{
			maxDifference = std::max(maxDifference, uint64_t(std::abs(int64_t(actualAverage) - int64_t(bit))));
		}

		ocean_assert(actualAverage != 0ull && perfectAverage != 0ull);

		if (actualAverage != 0ull && perfectAverage != 0ull)
		{
			const double percentActual = double(maxDifference) / double(actualAverage);
			const double percentPerfect = double(maxDifference) / double(perfectAverage);

			Log::info() << "Generator object, created values: " << iterations;
			Log::info() << "Maximal bit error to average: " << percentActual * 100.0 << "% (actual), " << percentPerfect * 100.0 << "% (perfect)";

			if (percentActual > threshold || percentPerfect > threshold)
			{
				allSucceeded = false;
			}
		}
		else
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	{
		std::random_device randomDevice;
		std::mt19937 twisterEngine(randomDevice());
		std::uniform_int_distribution<uint32_t> distribution(0u, uint32_t(-1));

		Indices64 bits(numberBits, 0ull);

		uint64_t iterations = 0u;

		const Timestamp startTimestamp(true);

		do
		{
			for (unsigned int n = 0u; n < 10000u; ++n)
			{
				const uint32_t value = distribution(twisterEngine);

				for (unsigned int nBit = 0u; nBit < numberBits; ++nBit)
				{
					if (value & (1u << nBit))
					{
						++bits[nBit];
					}
				}

				++iterations;
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);

		const uint64_t perfectAverage = iterations / 2ull;

		uint64_t actualAverage = 0ull;
		for (const uint64_t bit : bits)
		{
			actualAverage += bit;
		}
		actualAverage /= uint64_t(bits.size());

		uint64_t maxDifference = 0u;
		for (const uint64_t bit : bits)
		{
			maxDifference = std::max(maxDifference, uint64_t(std::abs(int64_t(actualAverage) - int64_t(bit))));
		}

		ocean_assert(actualAverage != 0ull && perfectAverage != 0ull);

		if (actualAverage != 0ull && perfectAverage != 0ull)
		{
			const double percentActual = double(maxDifference) / double(actualAverage);
			const double percentPerfect = double(maxDifference) / double(perfectAverage);

			Log::info() << "C++ mt19937, created values: " << iterations;
			Log::info() << "Maximal bit error to average: " << percentActual * 100.0 << "% (actual), " << percentPerfect * 100.0 << "% (perfect)";

			if (percentActual > threshold || percentPerfect > threshold)
			{
				allSucceeded = false;
			}
		}
		else
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

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

bool TestRandomI::testDistribution64(RandomGenerator& randomGenerator, const double testDuration)
{
	constexpr unsigned int numberBits = 64u;

	Log::info() << "Random " << numberBits << " bit distribution test:";
	Log::info() << " ";

	bool allSucceeded = true;

	const double threshold = 0.01; // 1%

	{
		Indices64 bits(numberBits, 0ull);

		uint64_t iterations = 0u;

		const Timestamp startTimestamp(true);

		do
		{
			for (unsigned int n = 0u; n < 10000u; ++n)
			{
				const uint64_t value = RandomI::random64();

				for (unsigned int nBit = 0u; nBit < numberBits; ++nBit)
				{
					if (value & (1ull << nBit))
					{
						++bits[nBit];
					}
				}

				++iterations;
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);

		const uint64_t perfectAverage = iterations / 2ull;

		uint64_t actualAverage = 0ull;
		for (const uint64_t bit : bits)
		{
			actualAverage += bit;
		}
		actualAverage /= uint64_t(bits.size());

		uint64_t maxDifference = 0u;
		for (const uint64_t bit : bits)
		{
			maxDifference = std::max(maxDifference, uint64_t(std::abs(int64_t(actualAverage) - int64_t(bit))));
		}

		ocean_assert(actualAverage != 0ull && perfectAverage != 0ull);

		if (actualAverage != 0ull && perfectAverage != 0ull)
		{
			const double percentActual = double(maxDifference) / double(actualAverage);
			const double percentPerfect = double(maxDifference) / double(perfectAverage);

			Log::info() << "Default, created values: " << iterations;
			Log::info() << "Maximal bit error to average: " << percentActual * 100.0 << "% (actual), " << percentPerfect * 100.0 << "% (perfect)";

			if (percentActual > threshold || percentPerfect > threshold)
			{
				allSucceeded = false;
			}
		}
		else
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	{
		Indices64 bits(numberBits, 0ull);

		uint64_t iterations = 0u;

		const Timestamp startTimestamp(true);

		do
		{
			for (unsigned int n = 0u; n < 10000u; ++n)
			{
				const uint64_t value = RandomI::random64(randomGenerator);

				for (unsigned int nBit = 0u; nBit < numberBits; ++nBit)
				{
					if (value & (1ull << nBit))
					{
						++bits[nBit];
					}
				}

				++iterations;
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);

		const uint64_t perfectAverage = iterations / 2ull;

		uint64_t actualAverage = 0ull;
		for (const uint64_t bit : bits)
		{
			actualAverage += bit;
		}
		actualAverage /= uint64_t(bits.size());

		uint64_t maxDifference = 0u;
		for (const uint64_t bit : bits)
		{
			maxDifference = std::max(maxDifference, uint64_t(std::abs(int64_t(actualAverage) - int64_t(bit))));
		}

		ocean_assert(actualAverage != 0ull && perfectAverage != 0ull);

		if (actualAverage != 0ull && perfectAverage != 0ull)
		{
			const double percentActual = double(maxDifference) / double(actualAverage);
			const double percentPerfect = double(maxDifference) / double(perfectAverage);

			Log::info() << "Generator object, created values: " << iterations;
			Log::info() << "Maximal bit error to average: " << percentActual * 100.0 << "% (actual), " << percentPerfect * 100.0 << "% (perfect)";

			if (percentActual > threshold || percentPerfect > threshold)
			{
				allSucceeded = false;
			}
		}
		else
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	{
		std::random_device randomDevice;
		std::mt19937 twisterEngine(randomDevice());
		std::uniform_int_distribution<uint64_t> distribution(0u, uint64_t(-1));

		Indices64 bits(numberBits, 0ull);

		uint64_t iterations = 0u;

		const Timestamp startTimestamp(true);

		do
		{
			for (unsigned int n = 0u; n < 10000u; ++n)
			{
				const uint64_t value = distribution(twisterEngine);

				for (unsigned int nBit = 0u; nBit < numberBits; ++nBit)
				{
					if (value & (1ull << nBit))
					{
						++bits[nBit];
					}
				}

				++iterations;
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);

		const uint64_t perfectAverage = iterations / 2ull;

		uint64_t actualAverage = 0ull;
		for (const uint64_t bit : bits)
		{
			actualAverage += bit;
		}
		actualAverage /= uint64_t(bits.size());

		uint64_t maxDifference = 0u;
		for (const uint64_t bit : bits)
		{
			maxDifference = std::max(maxDifference, uint64_t(std::abs(int64_t(actualAverage) - int64_t(bit))));
		}

		ocean_assert(actualAverage != 0ull && perfectAverage != 0ull);

		if (actualAverage != 0ull && perfectAverage != 0ull)
		{
			const double percentActual = double(maxDifference) / double(actualAverage);
			const double percentPerfect = double(maxDifference) / double(perfectAverage);

			Log::info() << "C++ mt19937, created values: " << iterations;
			Log::info() << "Maximal bit error to average: " << percentActual * 100.0 << "% (actual), " << percentPerfect * 100.0 << "% (perfect)";

			if (percentActual > threshold || percentPerfect > threshold)
			{
				allSucceeded = false;
			}
		}
		else
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

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

bool TestRandomI::testDistributionSmallRange(RandomGenerator& randomGenerator, const double testDuration)
{
	Log::info() << "Random distribution test (small range):";
	Log::info() << " ";

	bool allSucceeded = true;

	const double threshold = 0.05; // 5%

	constexpr unsigned int valueRange = 100u;

	std::random_device randomDevice;
	std::mt19937 twisterEngine(randomDevice());

	Log::info() << "Range [0, valueRange - 1]";

	{
		// testing values [0, valueRange - 1]

		Indices64 occurrences(valueRange, 0ull);

		uint64_t iterations = 0u;

		const Timestamp startTimestamp(true);

		do
		{
			for (unsigned int n = 0u; n < valueRange * 1000u; ++n)
			{
				const unsigned int value = RandomI::random(valueRange - 1u);

				if (value < valueRange)
				{
					++occurrences[value];
				}
				else
				{
					allSucceeded = false;
				}

				++iterations;
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);

		const uint64_t perfectAverage = iterations / 2ull;

		uint64_t actualAverage = 0ull;
		for (const uint64_t occurrence : occurrences)
		{
			actualAverage += occurrence;
		}
		actualAverage /= uint64_t(occurrences.size());

		uint64_t maxDifference = 0u;
		for (const uint64_t occurrence : occurrences)
		{
			maxDifference = std::max(maxDifference, uint64_t(std::abs(int64_t(actualAverage) - int64_t(occurrence))));
		}

		ocean_assert(actualAverage != 0ull && perfectAverage != 0ull);

		if (actualAverage != 0ull && perfectAverage != 0ull)
		{
			const double percentActual = double(maxDifference) / double(actualAverage);
			const double percentPerfect = double(maxDifference) / double(perfectAverage);

			Log::info() << "Default, created values: " << iterations;
			Log::info() << "Maximal bit error to average: " << percentActual * 100.0 << "% (actual), " << percentPerfect * 100.0 << "% (perfect)";

			if (percentActual > threshold || percentPerfect > threshold)
			{
				allSucceeded = false;
			}
		}
		else
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	{
		// testing values [0, valueRange - 1], with RandomGenerator object

		Indices64 occurrences(valueRange, 0ull);

		uint64_t iterations = 0u;

		const Timestamp startTimestamp(true);

		do
		{
			for (unsigned int n = 0u; n < valueRange * 1000u; ++n)
			{
				const unsigned int value = RandomI::random(randomGenerator, valueRange - 1u);

				if (value < valueRange)
				{
					++occurrences[value];
				}
				else
				{
					allSucceeded = false;
				}

				++iterations;
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);

		const uint64_t perfectAverage = iterations / 2ull;

		uint64_t actualAverage = 0ull;
		for (const uint64_t occurrence : occurrences)
		{
			actualAverage += occurrence;
		}
		actualAverage /= uint64_t(occurrences.size());

		uint64_t maxDifference = 0u;
		for (const uint64_t occurrence : occurrences)
		{
			maxDifference = std::max(maxDifference, uint64_t(std::abs(int64_t(actualAverage) - int64_t(occurrence))));
		}

		ocean_assert(actualAverage != 0ull && perfectAverage != 0ull);

		if (actualAverage != 0ull && perfectAverage != 0ull)
		{
			const double percentActual = double(maxDifference) / double(actualAverage);
			const double percentPerfect = double(maxDifference) / double(perfectAverage);

			Log::info() << "Generator object, created values: " << iterations;
			Log::info() << "Maximal bit error to average: " << percentActual * 100.0 << "% (actual), " << percentPerfect * 100.0 << "% (perfect)";

			if (percentActual > threshold || percentPerfect > threshold)
			{
				allSucceeded = false;
			}
		}
		else
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	Log::info() << "Range [+minRange, +maxRange]:";

	{
		// testing values [+minRange, +maxRange]

		Indices64 occurrences(valueRange, 0ull);

		uint64_t iterations = 0u;

		const Timestamp startTimestamp(true);

		constexpr unsigned int maxMinRange = std::numeric_limits<unsigned int>::max() - valueRange;

		std::uniform_int_distribution<unsigned int> distribution(0u, maxMinRange);

		const unsigned int minRange = distribution(twisterEngine);
		const unsigned int maxRange = minRange + valueRange - 1u;
		ocean_assert(minRange < maxRange);

		do
		{
			for (unsigned int n = 0u; n < valueRange * 1000u; ++n)
			{
				const unsigned int value = RandomI::random(minRange, maxRange);

				if (value >= minRange && value <= maxRange)
				{
					ocean_assert(value - minRange < valueRange);
					++occurrences[value - minRange];
				}
				else
				{
					allSucceeded = false;
				}

				++iterations;
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);

		const uint64_t perfectAverage = iterations / 2ull;

		uint64_t actualAverage = 0ull;
		for (const uint64_t occurrence : occurrences)
		{
			actualAverage += occurrence;
		}
		actualAverage /= uint64_t(occurrences.size());

		uint64_t maxDifference = 0u;
		for (const uint64_t occurrence : occurrences)
		{
			maxDifference = std::max(maxDifference, uint64_t(std::abs(int64_t(actualAverage) - int64_t(occurrence))));
		}

		ocean_assert(actualAverage != 0ull && perfectAverage != 0ull);

		if (actualAverage != 0ull && perfectAverage != 0ull)
		{
			const double percentActual = double(maxDifference) / double(actualAverage);
			const double percentPerfect = double(maxDifference) / double(perfectAverage);

			Log::info() << "Default, created values: " << iterations;
			Log::info() << "Maximal bit error to average: " << percentActual * 100.0 << "% (actual), " << percentPerfect * 100.0 << "% (perfect)";

			if (percentActual > threshold || percentPerfect > threshold)
			{
				allSucceeded = false;
			}
		}
		else
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	{
		// testing values [+minRange, +maxRange], with RandomGenerator object

		Indices64 occurrences(valueRange, 0ull);

		uint64_t iterations = 0u;

		const Timestamp startTimestamp(true);

		constexpr unsigned int maxMinRange = std::numeric_limits<unsigned int>::max() - valueRange;

		std::uniform_int_distribution<unsigned int> distribution(0u, maxMinRange);

		const unsigned int minRange = distribution(twisterEngine);
		const unsigned int maxRange = minRange + valueRange - 1u;
		ocean_assert(minRange < maxRange);

		do
		{
			for (unsigned int n = 0u; n < valueRange * 1000u; ++n)
			{
				const unsigned int value = RandomI::random(randomGenerator, minRange, maxRange);

				if (value >= minRange && value <= maxRange)
				{
					ocean_assert(value - minRange < valueRange);
					++occurrences[value - minRange];
				}
				else
				{
					allSucceeded = false;
				}

				++iterations;
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);

		const uint64_t perfectAverage = iterations / 2ull;

		uint64_t actualAverage = 0ull;
		for (const uint64_t occurrence : occurrences)
		{
			actualAverage += occurrence;
		}
		actualAverage /= uint64_t(occurrences.size());

		uint64_t maxDifference = 0u;
		for (const uint64_t occurrence : occurrences)
		{
			maxDifference = std::max(maxDifference, uint64_t(std::abs(int64_t(actualAverage) - int64_t(occurrence))));
		}

		ocean_assert(actualAverage != 0ull && perfectAverage != 0ull);

		if (actualAverage != 0ull && perfectAverage != 0ull)
		{
			const double percentActual = double(maxDifference) / double(actualAverage);
			const double percentPerfect = double(maxDifference) / double(perfectAverage);

			Log::info() << "Generator object, created values: " << iterations;
			Log::info() << "Maximal bit error to average: " << percentActual * 100.0 << "% (actual), " << percentPerfect * 100.0 << "% (perfect)";

			if (percentActual > threshold || percentPerfect > threshold)
			{
				allSucceeded = false;
			}
		}
		else
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	Log::info() << "Range [+/-minRange, +/-maxRange]:";

	{
		// testing values [+/- minRange, +/- maxRange]

		Indices64 occurrences(valueRange, 0ull);

		uint64_t iterations = 0u;

		const Timestamp startTimestamp(true);

		constexpr int maxMinRange = std::numeric_limits<int>::max() - int(valueRange);

		std::uniform_int_distribution<int> distribution(std::numeric_limits<int>::lowest(), maxMinRange);

		const int minRange = distribution(twisterEngine);
		const int maxRange = minRange + valueRange - 1;
		ocean_assert(minRange < maxRange);

		do
		{
			for (unsigned int n = 0u; n < valueRange * 1000u; ++n)
			{
				const int value = RandomI::random(minRange, maxRange);

				if (value >= minRange && value <= maxRange)
				{
					ocean_assert(value - minRange < int(valueRange));
					++occurrences[value - minRange];
				}
				else
				{
					allSucceeded = false;
				}

				++iterations;
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);

		const uint64_t perfectAverage = iterations / 2ull;

		uint64_t actualAverage = 0ull;
		for (const uint64_t occurrence : occurrences)
		{
			actualAverage += occurrence;
		}
		actualAverage /= uint64_t(occurrences.size());

		uint64_t maxDifference = 0u;
		for (const uint64_t occurrence : occurrences)
		{
			maxDifference = std::max(maxDifference, uint64_t(std::abs(int64_t(actualAverage) - int64_t(occurrence))));
		}

		ocean_assert(actualAverage != 0ull && perfectAverage != 0ull);

		if (actualAverage != 0ull && perfectAverage != 0ull)
		{
			const double percentActual = double(maxDifference) / double(actualAverage);
			const double percentPerfect = double(maxDifference) / double(perfectAverage);

			Log::info() << "Default, created values: " << iterations;
			Log::info() << "Maximal bit error to average: " << percentActual * 100.0 << "% (actual), " << percentPerfect * 100.0 << "% (perfect)";

			if (percentActual > threshold || percentPerfect > threshold)
			{
				allSucceeded = false;
			}
		}
		else
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	{
		// testing values [+/- minRange, +/- maxRange], with RandomGenerator object

		Indices64 occurrences(valueRange, 0ull);

		uint64_t iterations = 0u;

		const Timestamp startTimestamp(true);

		constexpr int maxMinRange = std::numeric_limits<int>::max() - int(valueRange);

		std::uniform_int_distribution<int> distribution(std::numeric_limits<int>::lowest(), maxMinRange);

		const int minRange = distribution(twisterEngine);
		const int maxRange = minRange + valueRange - 1;
		ocean_assert(minRange < maxRange);

		do
		{
			for (unsigned int n = 0u; n < valueRange * 1000u; ++n)
			{
				const int value = RandomI::random(randomGenerator, minRange, maxRange);

				if (value >= minRange && value <= maxRange)
				{
					ocean_assert(value - minRange < int(valueRange));
					++occurrences[value - minRange];
				}
				else
				{
					allSucceeded = false;
				}

				++iterations;
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);

		const uint64_t perfectAverage = iterations / 2ull;

		uint64_t actualAverage = 0ull;
		for (const uint64_t occurrence : occurrences)
		{
			actualAverage += occurrence;
		}
		actualAverage /= uint64_t(occurrences.size());

		uint64_t maxDifference = 0u;
		for (const uint64_t occurrence : occurrences)
		{
			maxDifference = std::max(maxDifference, uint64_t(std::abs(int64_t(actualAverage) - int64_t(occurrence))));
		}

		ocean_assert(actualAverage != 0ull && perfectAverage != 0ull);

		if (actualAverage != 0ull && perfectAverage != 0ull)
		{
			const double percentActual = double(maxDifference) / double(actualAverage);
			const double percentPerfect = double(maxDifference) / double(perfectAverage);

			Log::info() << "Generator object, created values: " << iterations;
			Log::info() << "Maximal bit error to average: " << percentActual * 100.0 << "% (actual), " << percentPerfect * 100.0 << "% (perfect)";

			if (percentActual > threshold || percentPerfect > threshold)
			{
				allSucceeded = false;
			}
		}
		else
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	{
		std::uniform_int_distribution<unsigned int> distribution(0u, valueRange - 1u);

		Indices64 occurrences(valueRange, 0ull);

		uint64_t iterations = 0u;

		const Timestamp startTimestamp(true);

		do
		{
			for (unsigned int n = 0u; n < 10000u; ++n)
			{
				const uint32_t value = distribution(twisterEngine);

				if (value < valueRange)
				{
					++occurrences[value];
				}
				else
				{
					allSucceeded = false;
				}

				++iterations;
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);

		const uint64_t perfectAverage = iterations / 2ull;

		uint64_t actualAverage = 0ull;
		for (const uint64_t occurrence : occurrences)
		{
			actualAverage += occurrence;
		}
		actualAverage /= uint64_t(occurrences.size());

		uint64_t maxDifference = 0u;
		for (const uint64_t occurrence : occurrences)
		{
			maxDifference = std::max(maxDifference, uint64_t(std::abs(int64_t(actualAverage) - int64_t(occurrence))));
		}

		ocean_assert(actualAverage != 0ull && perfectAverage != 0ull);

		if (actualAverage != 0ull && perfectAverage != 0ull)
		{
			const double percentActual = double(maxDifference) / double(actualAverage);
			const double percentPerfect = double(maxDifference) / double(perfectAverage);

			Log::info() << "C++ mt19937, created values: " << iterations;
			Log::info() << "Maximal bit error to average: " << percentActual * 100.0 << "% (actual), " << percentPerfect * 100.0 << "% (perfect)";

			if (percentActual > threshold || percentPerfect > threshold)
			{
				allSucceeded = false;
			}
		}
		else
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

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

bool TestRandomI::testDistributionLargeRange(RandomGenerator& randomGenerator, const double testDuration)
{
	Log::info() << "Random distribution test (large range):";
	Log::info() << " ";

	bool allSucceeded = true;

	const double threshold = 0.05; // 5%

	constexpr unsigned int bins = 100u;
	constexpr unsigned int binSize = 1000000u;
	constexpr unsigned int largeValueRange = bins * binSize;

	std::random_device randomDevice;
	std::mt19937 twisterEngine(randomDevice());

	Log::info() << "Range [0, valueRange - 1]";

	{
		// testing values [0, largeValueRange - 1]

		Indices64 occurrences(bins, 0ull);

		uint64_t iterations = 0u;

		const Timestamp startTimestamp(true);

		do
		{
			for (unsigned int n = 0u; n < bins * 1000u; ++n)
			{
				const unsigned int value = RandomI::random(largeValueRange - 1u);

				if (value < largeValueRange)
				{
					ocean_assert(value / binSize < bins);
					++occurrences[value / binSize];
				}
				else
				{
					allSucceeded = false;
				}

				++iterations;
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);

		const uint64_t perfectAverage = iterations / 2ull;

		uint64_t actualAverage = 0ull;
		for (const uint64_t occurrence : occurrences)
		{
			actualAverage += occurrence;
		}
		actualAverage /= uint64_t(occurrences.size());

		uint64_t maxDifference = 0u;
		for (const uint64_t occurrence : occurrences)
		{
			maxDifference = std::max(maxDifference, uint64_t(std::abs(int64_t(actualAverage) - int64_t(occurrence))));
		}

		ocean_assert(actualAverage != 0ull && perfectAverage != 0ull);

		if (actualAverage != 0ull && perfectAverage != 0ull)
		{
			const double percentActual = double(maxDifference) / double(actualAverage);
			const double percentPerfect = double(maxDifference) / double(perfectAverage);

			Log::info() << "Default, created values: " << iterations;
			Log::info() << "Maximal bit error to average: " << percentActual * 100.0 << "% (actual), " << percentPerfect * 100.0 << "% (perfect)";

			if (percentActual > threshold || percentPerfect > threshold)
			{
				allSucceeded = false;
			}
		}
		else
		{
			allSucceeded = false;
		}
	}

	{
		// testing values [0, largeValueRange - 1], with RandomGenerator object

		Indices64 occurrences(bins, 0ull);

		uint64_t iterations = 0u;

		const Timestamp startTimestamp(true);

		do
		{
			for (unsigned int n = 0u; n < bins * 1000u; ++n)
			{
				const unsigned int value = RandomI::random(randomGenerator, largeValueRange - 1u);

				if (value < largeValueRange)
				{
					ocean_assert(value / binSize < bins);
					++occurrences[value / binSize];
				}
				else
				{
					allSucceeded = false;
				}

				++iterations;
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);

		const uint64_t perfectAverage = iterations / 2ull;

		uint64_t actualAverage = 0ull;
		for (const uint64_t occurrence : occurrences)
		{
			actualAverage += occurrence;
		}
		actualAverage /= uint64_t(occurrences.size());

		uint64_t maxDifference = 0u;
		for (const uint64_t occurrence : occurrences)
		{
			maxDifference = std::max(maxDifference, uint64_t(std::abs(int64_t(actualAverage) - int64_t(occurrence))));
		}

		ocean_assert(actualAverage != 0ull && perfectAverage != 0ull);

		if (actualAverage != 0ull && perfectAverage != 0ull)
		{
			const double percentActual = double(maxDifference) / double(actualAverage);
			const double percentPerfect = double(maxDifference) / double(perfectAverage);

			Log::info() << "Generator object, Created values: " << iterations;
			Log::info() << "Maximal bit error to average: " << percentActual * 100.0 << "% (actual), " << percentPerfect * 100.0 << "% (perfect)";

			if (percentActual > threshold || percentPerfect > threshold)
			{
				allSucceeded = false;
			}
		}
		else
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";
	Log::info() << "Range [+minRange, +maxRange]";

	{
		// testing values [+minRange, +maxRange]

		Indices64 occurrences(bins, 0ull);

		uint64_t iterations = 0u;

		const Timestamp startTimestamp(true);

		constexpr unsigned int maxMinRange = std::numeric_limits<unsigned int>::max() - largeValueRange;

		std::uniform_int_distribution<unsigned int> distribution(0u, maxMinRange);

		const unsigned int minRange = distribution(twisterEngine);
		const unsigned int maxRange = minRange + largeValueRange - 1u;
		ocean_assert(minRange < maxRange);

		do
		{
			for (unsigned int n = 0u; n < bins * 1000u; ++n)
			{
				const unsigned int value = RandomI::random(minRange, maxRange);

				if (value >= minRange && value <= maxRange)
				{
					const unsigned int bin = (value - minRange) / binSize;

					ocean_assert(bin < bins);
					++occurrences[bin];
				}
				else
				{
					allSucceeded = false;
				}

				++iterations;
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);

		const uint64_t perfectAverage = iterations / 2ull;

		uint64_t actualAverage = 0ull;
		for (const uint64_t occurrence : occurrences)
		{
			actualAverage += occurrence;
		}
		actualAverage /= uint64_t(occurrences.size());

		uint64_t maxDifference = 0u;
		for (const uint64_t occurrence : occurrences)
		{
			maxDifference = std::max(maxDifference, uint64_t(std::abs(int64_t(actualAverage) - int64_t(occurrence))));
		}

		ocean_assert(actualAverage != 0ull && perfectAverage != 0ull);

		if (actualAverage != 0ull && perfectAverage != 0ull)
		{
			const double percentActual = double(maxDifference) / double(actualAverage);
			const double percentPerfect = double(maxDifference) / double(perfectAverage);

			Log::info() << "Default, created values: " << iterations;
			Log::info() << "Maximal bit error to average: " << percentActual * 100.0 << "% (actual), " << percentPerfect * 100.0 << "% (perfect)";

			if (percentActual > threshold || percentPerfect > threshold)
			{
				allSucceeded = false;
			}
		}
		else
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	{
		// testing values [+minRange, +maxRange], with RandomGenerator object

		Indices64 occurrences(bins, 0ull);

		uint64_t iterations = 0u;

		const Timestamp startTimestamp(true);

		constexpr unsigned int maxMinRange = std::numeric_limits<unsigned int>::max() - largeValueRange;

		std::uniform_int_distribution<unsigned int> distribution(0u, maxMinRange);

		const unsigned int minRange = distribution(twisterEngine);
		const unsigned int maxRange = minRange + largeValueRange - 1u;
		ocean_assert(minRange < maxRange);

		do
		{
			for (unsigned int n = 0u; n < bins * 1000u; ++n)
			{
				const unsigned int value = RandomI::random(randomGenerator, minRange, maxRange);

				if (value >= minRange && value <= maxRange)
				{
					const unsigned int bin = (value - minRange) / binSize;

					ocean_assert(bin < bins);
					++occurrences[bin];
				}
				else
				{
					allSucceeded = false;
				}

				++iterations;
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);

		const uint64_t perfectAverage = iterations / 2ull;

		uint64_t actualAverage = 0ull;
		for (const uint64_t occurrence : occurrences)
		{
			actualAverage += occurrence;
		}
		actualAverage /= uint64_t(occurrences.size());

		uint64_t maxDifference = 0u;
		for (const uint64_t occurrence : occurrences)
		{
			maxDifference = std::max(maxDifference, uint64_t(std::abs(int64_t(actualAverage) - int64_t(occurrence))));
		}

		ocean_assert(actualAverage != 0ull && perfectAverage != 0ull);

		if (actualAverage != 0ull && perfectAverage != 0ull)
		{
			const double percentActual = double(maxDifference) / double(actualAverage);
			const double percentPerfect = double(maxDifference) / double(perfectAverage);

			Log::info() << "Generator object, created values: " << iterations;
			Log::info() << "Maximal bit error to average: " << percentActual * 100.0 << "% (actual), " << percentPerfect * 100.0 << "% (perfect)";

			if (percentActual > threshold || percentPerfect > threshold)
			{
				allSucceeded = false;
			}
		}
		else
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	Log::info() << "Range [+/-largeMinRange, +/-largeMaxRange]";

	{
		// testing values [+/- minRange, +/- maxRange]

		Indices64 occurrences(bins, 0ull);

		uint64_t iterations = 0u;

		const Timestamp startTimestamp(true);

		constexpr int maxMinRange = std::numeric_limits<int>::max() - int(largeValueRange);

		std::uniform_int_distribution<int> distribution(std::numeric_limits<int>::lowest(), maxMinRange);

		const int minRange = distribution(twisterEngine);
		const int maxRange = minRange + int(largeValueRange - 1u);
		ocean_assert(minRange < maxRange);

		do
		{
			for (unsigned int n = 0u; n < bins * 1000u; ++n)
			{
				const int value = RandomI::random(minRange, maxRange);

				if (value >= minRange && value <= maxRange)
				{
					const int bin = (value - minRange) / binSize;

					ocean_assert(bin < int(bins));
					++occurrences[bin];
				}
				else
				{
					allSucceeded = false;
				}

				++iterations;
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);

		const uint64_t perfectAverage = iterations / 2ull;

		uint64_t actualAverage = 0ull;
		for (const uint64_t occurrence : occurrences)
		{
			actualAverage += occurrence;
		}
		actualAverage /= uint64_t(occurrences.size());

		uint64_t maxDifference = 0u;
		for (const uint64_t occurrence : occurrences)
		{
			maxDifference = std::max(maxDifference, uint64_t(std::abs(int64_t(actualAverage) - int64_t(occurrence))));
		}

		ocean_assert(actualAverage != 0ull && perfectAverage != 0ull);

		if (actualAverage != 0ull && perfectAverage != 0ull)
		{
			const double percentActual = double(maxDifference) / double(actualAverage);
			const double percentPerfect = double(maxDifference) / double(perfectAverage);

			Log::info() << "Default, created values: " << iterations;
			Log::info() << "Maximal bit error to average: " << percentActual * 100.0 << "% (actual), " << percentPerfect * 100.0 << "% (perfect)";

			if (percentActual > threshold || percentPerfect > threshold)
			{
				allSucceeded = false;
			}
		}
		else
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	{
		// testing values [+/- minRange, +/- maxRange], with RandomGenerator object

		Indices64 occurrences(bins, 0ull);

		uint64_t iterations = 0u;

		const Timestamp startTimestamp(true);

		constexpr int maxMinRange = std::numeric_limits<int>::max() - int(largeValueRange);

		std::uniform_int_distribution<int> distribution(std::numeric_limits<int>::lowest(), maxMinRange);

		const int minRange = distribution(twisterEngine);
		const int maxRange = minRange + int(largeValueRange - 1u);
		ocean_assert(minRange < maxRange);

		do
		{
			for (unsigned int n = 0u; n < bins * 1000u; ++n)
			{
				const int value = RandomI::random(randomGenerator, minRange, maxRange);

				if (value >= minRange && value <= maxRange)
				{
					const int bin = (value - minRange) / binSize;

					ocean_assert(bin < int(bins));
					++occurrences[bin];
				}
				else
				{
					allSucceeded = false;
				}

				++iterations;
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);

		const uint64_t perfectAverage = iterations / 2ull;

		uint64_t actualAverage = 0ull;
		for (const uint64_t occurrence : occurrences)
		{
			actualAverage += occurrence;
		}
		actualAverage /= uint64_t(occurrences.size());

		uint64_t maxDifference = 0u;
		for (const uint64_t occurrence : occurrences)
		{
			maxDifference = std::max(maxDifference, uint64_t(std::abs(int64_t(actualAverage) - int64_t(occurrence))));
		}

		ocean_assert(actualAverage != 0ull && perfectAverage != 0ull);

		if (actualAverage != 0ull && perfectAverage != 0ull)
		{
			const double percentActual = double(maxDifference) / double(actualAverage);
			const double percentPerfect = double(maxDifference) / double(perfectAverage);

			Log::info() << "Generator object, created values: " << iterations;
			Log::info() << "Maximal bit error to average: " << percentActual * 100.0 << "% (actual), " << percentPerfect * 100.0 << "% (perfect)";

			if (percentActual > threshold || percentPerfect > threshold)
			{
				allSucceeded = false;
			}
		}
		else
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	{
		std::uniform_int_distribution<unsigned int> distribution(0u, largeValueRange - 1u);

		Indices64 occurrences(bins, 0ull);

		uint64_t iterations = 0u;

		const Timestamp startTimestamp(true);

		do
		{
			for (unsigned int n = 0u; n < 10000u; ++n)
			{
				const unsigned int value = distribution(twisterEngine);

				if (value < largeValueRange)
				{
					ocean_assert(value / binSize < bins);
					++occurrences[value / binSize];
				}
				else
				{
					allSucceeded = false;
				}

				++iterations;
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);

		const uint64_t perfectAverage = iterations / 2ull;

		uint64_t actualAverage = 0ull;
		for (const uint64_t occurrence : occurrences)
		{
			actualAverage += occurrence;
		}
		actualAverage /= uint64_t(occurrences.size());

		uint64_t maxDifference = 0u;
		for (const uint64_t occurrence : occurrences)
		{
			maxDifference = std::max(maxDifference, uint64_t(std::abs(int64_t(actualAverage) - int64_t(occurrence))));
		}

		ocean_assert(actualAverage != 0ull && perfectAverage != 0ull);

		if (actualAverage != 0ull && perfectAverage != 0ull)
		{
			const double percentActual = double(maxDifference) / double(actualAverage);
			const double percentPerfect = double(maxDifference) / double(perfectAverage);

			Log::info() << "C++ mt19937, created values: " << iterations;
			Log::info() << "Maximal bit error to average: " << percentActual * 100.0 << "% (actual), " << percentPerfect * 100.0 << "% (perfect)";

			if (percentActual > threshold || percentPerfect > threshold)
			{
				allSucceeded = false;
			}
		}
		else
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

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

bool TestRandomI::testOneParameter(RandomGenerator& randomGenerator)
{
	Log::info() << "One random parameter test:";

	bool allSucceeded = true;

	constexpr unsigned int iterations = 1000000u;

	for (unsigned int n = 0u; n < iterations; ++n)
	{
		const unsigned int value = RandomI::random(0u);

		if (value != 0u)
		{
			allSucceeded = false;
		}
	}

	for (unsigned int n = 0u; n < iterations; ++n)
	{
		const unsigned int value = RandomI::random(randomGenerator, 0u);

		if (value != 0u)
		{
			allSucceeded = false;
		}
	}

	for (unsigned int n = 0u; n < iterations; ++n)
	{
		const int value = RandomI::random(-5, -5);

		if (value != -5)
		{
			allSucceeded = false;
		}
	}

	for (unsigned int n = 0u; n < iterations; ++n)
	{
		const int value = RandomI::random(randomGenerator, -5, -5);

		if (value != -5)
		{
			allSucceeded = false;
		}
	}

	for (unsigned int n = 0u; n < iterations; ++n)
	{
		const int value = RandomI::random(5, 5);

		if (value != 5)
		{
			allSucceeded = false;
		}
	}

	for (unsigned int n = 0u; n < iterations; ++n)
	{
		const int value = RandomI::random(randomGenerator, 5, 5);

		if (value != 5)
		{
			allSucceeded = false;
		}
	}

	for (unsigned int n = 0u; n < iterations; ++n)
	{
		const unsigned int value = RandomI::random(5u, 5u);

		if (value != 5u)
		{
			allSucceeded = false;
		}
	}

	for (unsigned int n = 0u; n < iterations; ++n)
	{
		const unsigned int value = RandomI::random(randomGenerator, 5u, 5u);

		if (value != 5u)
		{
			allSucceeded = false;
		}
	}

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

bool TestRandomI::testTwoParameter(RandomGenerator& randomGenerator)
{
	Log::info() << "Two random parameter test:";

	bool allSucceeded = true;

	constexpr unsigned int iterations = 1000000u;

	{
		unsigned int first = 0u;
		unsigned int second = 0u;

		for (unsigned int n = 0u; n < iterations; ++n)
		{
			const unsigned int value = RandomI::random(1u);

			if (value == 0u)
			{
				first++;
			}
			else if (value == 1u)
			{
				second++;
			}
			else
			{
				allSucceeded = false;
			}
		}

		const unsigned int percentFirst = first * 100u / iterations;
		const unsigned int percentSecond = second * 100u / iterations;

		const unsigned int percentDifference = (unsigned int)(std::abs(int(percentFirst) - int(percentSecond)));

		if (percentDifference > 4u)
		{
			allSucceeded = false;
		}
	}

	{
		unsigned int first = 0u;
		unsigned int second = 0u;

		for (unsigned int n = 0u; n < iterations; ++n)
		{
			const unsigned int value = RandomI::random(randomGenerator, 1u);

			if (value == 0u)
			{
				first++;
			}
			else if (value == 1u)
			{
				second++;
			}
			else
			{
				allSucceeded = false;
			}
		}

		const unsigned int percentFirst = first * 100u / iterations;
		const unsigned int percentSecond = second * 100u / iterations;

		const unsigned int percentDifference = (unsigned int)(std::abs(int(percentFirst) - int(percentSecond)));

		if (percentDifference > 4u)
		{
			allSucceeded = false;
		}
	}

	{
		unsigned int first = 0u;
		unsigned int second = 0u;

		for (unsigned int n = 0u; n < iterations; ++n)
		{
			const int value = RandomI::random(-6, -5);

			if (value == -6)
			{
				first++;
			}
			else if (value == -5)
			{
				second++;
			}
			else
			{
				allSucceeded = false;
			}
		}

		const unsigned int percentFirst = first * 100u / iterations;
		const unsigned int percentSecond = second * 100u / iterations;

		const unsigned int percentDifference = (unsigned int)(std::abs(int(percentFirst) - int(percentSecond)));

		if (percentDifference > 4u)
		{
			allSucceeded = false;
		}
	}

	{
		unsigned int first = 0u;
		unsigned int second = 0u;

		for (unsigned int n = 0u; n < iterations; ++n)
		{
			const int value = RandomI::random(randomGenerator, -6, -5);

			if (value == -6)
			{
				first++;
			}
			else if (value == -5)
			{
				second++;
			}
			else
			{
				allSucceeded = false;
			}
		}


		const unsigned int percentFirst = first * 100u / iterations;
		const unsigned int percentSecond = second * 100u / iterations;

		const unsigned int percentDifference = (unsigned int)(std::abs(int(percentFirst) - int(percentSecond)));

		if (percentDifference > 4u)
		{
			allSucceeded = false;
		}
	}

	{
		unsigned int first = 0u;
		unsigned int second = 0u;

		for (unsigned int n = 0u; n < iterations; ++n)
		{
			const int value = RandomI::random(900, 901);

			if (value == 900)
			{
				first++;
			}
			else if (value == 901)
			{
				second++;
			}
			else
			{
				allSucceeded = false;
			}
		}

		const unsigned int percentFirst = first * 100u / iterations;
		const unsigned int percentSecond = second * 100u / iterations;

		const unsigned int percentDifference = (unsigned int)(std::abs(int(percentFirst) - int(percentSecond)));

		if (percentDifference > 4u)
		{
			allSucceeded = false;
		}
	}

	{
		unsigned int first = 0u;
		unsigned int second = 0u;

		for (unsigned int n = 0u; n < iterations; ++n)
		{
			const int value = RandomI::random(randomGenerator, 900, 901);

			if (value == 900)
			{
				first++;
			}
			else if (value == 901)
			{
				second++;
			}
			else
			{
				allSucceeded = false;
			}
		}

		const unsigned int percentFirst = first * 100u / iterations;
		const unsigned int percentSecond = second * 100u / iterations;

		const unsigned int percentDifference = (unsigned int)(std::abs(int(percentFirst) - int(percentSecond)));

		if (percentDifference > 4u)
		{
			allSucceeded = false;
		}
	}

	{
		unsigned int first = 0u;
		unsigned int second = 0u;

		for (unsigned int n = 0u; n < iterations; ++n)
		{
			const unsigned int value = RandomI::random(900u, 901u);

			if (value == 900u)
			{
				first++;
			}
			else if (value == 901u)
			{
				second++;
			}
			else
			{
				allSucceeded = false;
			}
		}

		const unsigned int percentFirst = first * 100u / iterations;
		const unsigned int percentSecond = second * 100u / iterations;

		const unsigned int percentDifference = (unsigned int)(std::abs(int(percentFirst) - int(percentSecond)));

		if (percentDifference > 4u)
		{
			allSucceeded = false;
		}
	}

	{
		unsigned int first = 0u;
		unsigned int second = 0u;

		for (unsigned int n = 0u; n < iterations; ++n)
		{
			const unsigned int value = RandomI::random(randomGenerator, 900u, 901u);

			if (value == 900u)
			{
				first++;
			}
			else if (value == 901u)
			{
				second++;
			}
			else
			{
				allSucceeded = false;
			}
		}

		const unsigned int percentFirst = first * 100u / iterations;
		const unsigned int percentSecond = second * 100u / iterations;

		const unsigned int percentDifference = (unsigned int)(std::abs(int(percentFirst) - int(percentSecond)));

		if (percentDifference > 4u)
		{
			allSucceeded = false;
		}
	}

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

bool TestRandomI::testThreeParameter(RandomGenerator& randomGenerator)
{
	Log::info() << "Three random parameter test:";

	bool allSucceeded = true;

	constexpr unsigned int iterations = 1000000u;

	{
		unsigned int first = 0u;
		unsigned int second = 0u;
		unsigned int third = 0u;

		for (unsigned int n = 0u; n < iterations; ++n)
		{
			const unsigned int value = RandomI::random(2u);

			if (value == 0u)
			{
				first++;
			}
			else if (value == 1u)
			{
				second++;
			}
			else if (value == 2u)
			{
				third++;
			}
			else
			{
				allSucceeded = false;
			}
		}

		const unsigned int percentFirst = first * 100u / iterations;
		const unsigned int percentSecond = second * 100u / iterations;
		const unsigned int percentThird = third * 100u / iterations;

		if ((unsigned int)(std::abs(int(percentFirst) - int(percentSecond))) > 4
				|| (unsigned int)(std::abs(int(percentSecond) - int(percentThird))) > 4
				|| (unsigned int)(std::abs(int(percentFirst) - int(percentThird))) > 4)
		{
			allSucceeded = false;
		}
	}

	{
		unsigned int first = 0u;
		unsigned int second = 0u;
		unsigned int third = 0u;

		for (unsigned int n = 0u; n < iterations; ++n)
		{
			const unsigned int value = RandomI::random(randomGenerator, 2u);

			if (value == 0u)
			{
				first++;
			}
			else if (value == 1u)
			{
				second++;
			}
			else if (value == 2u)
			{
				third++;
			}
			else
			{
				allSucceeded = false;
			}
		}

		const unsigned int percentFirst = first * 100u / iterations;
		const unsigned int percentSecond = second * 100u / iterations;
		const unsigned int percentThird = third * 100u / iterations;

		if ((unsigned int)(std::abs(int(percentFirst) - int(percentSecond))) > 4
				|| (unsigned int)(std::abs(int(percentSecond) - int(percentThird))) > 4
				|| (unsigned int)(std::abs(int(percentFirst) - int(percentThird))) > 4)
		{
			allSucceeded = false;
		}
	}

	{
		unsigned int first = 0u;
		unsigned int second = 0u;
		unsigned int third = 0u;

		for (unsigned int n = 0u; n < iterations; ++n)
		{
			const int value = RandomI::random(-7, -5);

			if (value == -7)
			{
				first++;
			}
			else if (value == -6)
			{
				second++;
			}
			else if (value == -5)
			{
				third++;
			}
			else
			{
				allSucceeded = false;
			}
		}

		const unsigned int percentFirst = first * 100u / iterations;
		const unsigned int percentSecond = second * 100u / iterations;
		const unsigned int percentThird = third * 100u / iterations;

		if ((unsigned int)(std::abs(int(percentFirst) - int(percentSecond))) > 4
				|| (unsigned int)(std::abs(int(percentSecond) - int(percentThird))) > 4
				|| (unsigned int)(std::abs(int(percentFirst) - int(percentThird))) > 4)
		{
			allSucceeded = false;
		}
	}

	{
		unsigned int first = 0u;
		unsigned int second = 0u;
		unsigned int third = 0u;

		for (unsigned int n = 0u; n < iterations; ++n)
		{
			const int value = RandomI::random(randomGenerator, -7, -5);

			if (value == -7)
			{
				first++;
			}
			else if (value == -6)
			{
				second++;
			}
			else if (value == -5)
			{
				third++;
			}
			else
			{
				allSucceeded = false;
			}
		}

		const unsigned int percentFirst = first * 100u / iterations;
		const unsigned int percentSecond = second * 100u / iterations;
		const unsigned int percentThird = third * 100u / iterations;

		if ((unsigned int)(std::abs(int(percentFirst) - int(percentSecond))) > 4
				|| (unsigned int)(std::abs(int(percentSecond) - int(percentThird))) > 4
				|| (unsigned int)(std::abs(int(percentFirst) - int(percentThird))) > 4)
		{
			allSucceeded = false;
		}
	}

	{
		unsigned int first = 0u;
		unsigned int second = 0u;
		unsigned int third = 0u;

		for (unsigned int n = 0u; n < iterations; ++n)
		{
			const int value = RandomI::random(900, 902);

			if (value == 900)
			{
				first++;
			}
			else if (value == 901)
			{
				second++;
			}
			else if (value == 902)
			{
				third++;
			}
			else
			{
				allSucceeded = false;
			}
		}

		const unsigned int percentFirst = first * 100u / iterations;
		const unsigned int percentSecond = second * 100u / iterations;
		const unsigned int percentThird = third * 100u / iterations;

		if ((unsigned int)(std::abs(int(percentFirst) - int(percentSecond))) > 4
				|| (unsigned int)(std::abs(int(percentSecond) - int(percentThird))) > 4
				|| (unsigned int)(std::abs(int(percentFirst) - int(percentThird))) > 4)
		{
			allSucceeded = false;
		}
	}

	{
		unsigned int first = 0u;
		unsigned int second = 0u;
		unsigned int third = 0u;

		for (unsigned int n = 0u; n < iterations; ++n)
		{
			const int value = RandomI::random(randomGenerator, 900, 902);

			if (value == 900)
			{
				first++;
			}
			else if (value == 901)
			{
				second++;
			}
			else if (value == 902)
			{
				third++;
			}
			else
			{
				allSucceeded = false;
			}
		}

		const unsigned int percentFirst = first * 100u / iterations;
		const unsigned int percentSecond = second * 100u / iterations;
		const unsigned int percentThird = third * 100u / iterations;

		if ((unsigned int)(std::abs(int(percentFirst) - int(percentSecond))) > 4
				|| (unsigned int)(std::abs(int(percentSecond) - int(percentThird))) > 4
				|| (unsigned int)(std::abs(int(percentFirst) - int(percentThird))) > 4)
		{
			allSucceeded = false;
		}
	}

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

bool TestRandomI::testSeveralParameter(RandomGenerator& randomGenerator)
{
	Log::info() << "n random parameter test:";

	bool allSucceeded = true;

	constexpr unsigned int iterations = 1000000u;

	{
		for (unsigned int i = 0u; i < 10u; ++i)
		{
			const int start = RandomI::random(-500, 500);
			const int number = RandomI::random(120, 1000);

			const int stop = start + number - 1;

			ocean_assert(start >= -500 && start <= 500);

			Indices32 values(number, 0u);

			for (unsigned int n = 0u; n < iterations; ++n)
			{
				const int value = RandomI::random(start, stop);

				if (value < start || value > stop)
				{
					allSucceeded = false;
				}
				else
				{
					const unsigned index = value - start;
					ocean_assert(index >= 0u && index < (unsigned int)number);

					values[index]++;
				}
			}

			unsigned int minValue = (unsigned int)(-1);
			unsigned int maxValue = 0u;

			for (unsigned int n = 0u; n < (unsigned int)number; ++n)
			{
				minValue = min(minValue, values[n]);
				maxValue = max(maxValue, values[n]);
			}

			if (std::abs(int(minValue * 100u / iterations) - int(maxValue * 100u / iterations)) > 1)
			{
				allSucceeded = false;
			}
		}
	}

	{
		for (unsigned int i = 0u; i < 10u; ++i)
		{
			const unsigned int start = RandomI::random(0u, 500u);
			const unsigned int number = RandomI::random(120u, 1000u);

			const unsigned int stop = start + number - 1u;

			ocean_assert(start >= 0u && start <= 500u);

			Indices32 values(number, 0u);

			for (unsigned int n = 0u; n < iterations; ++n)
			{
				const unsigned int value = RandomI::random(start, stop);

				if (value < start || value > stop)
				{
					allSucceeded = false;
				}
				else
				{
					const unsigned index = value - start;
					ocean_assert(index >= 0u && index < number);

					values[index]++;
				}
			}

			unsigned int minValue = (unsigned int)(-1);
			unsigned int maxValue = 0u;

			for (unsigned int n = 0u; n < number; ++n)
			{
				minValue = min(minValue, values[n]);
				maxValue = max(maxValue, values[n]);
			}

			if (std::abs(int(minValue * 100u / iterations) - int(maxValue * 100u / iterations)) > 1)
			{
				allSucceeded = false;
			}
		}
	}

	{
		for (unsigned int i = 0u; i < 10u; ++i)
		{
			const int start = RandomI::random(randomGenerator, -500, 500);
			const unsigned int number = RandomI::random(randomGenerator, 120, 1000);

			const int stop = start + int(number - 1);

			Indices32 values(number, 0u);

			for (unsigned int n = 0u; n < iterations; ++n)
			{
				const int value = RandomI::random(randomGenerator, start, stop);

				if (value < start || value > stop)
				{
					allSucceeded = false;
				}
				else
				{
					const unsigned index = value - start;
					ocean_assert(index >= 0u && index < number);

					values[index]++;
				}
			}

			unsigned int minValue = (unsigned int)(-1);
			unsigned int maxValue = 0u;

			for (unsigned int n = 0u; n < number; ++n)
			{
				minValue = min(minValue, values[n]);
				maxValue = max(maxValue, values[n]);
			}

			if (std::abs(int(minValue * 100u / iterations) - int(maxValue * 100u / iterations)) > 1)
			{
				allSucceeded = false;
			}
		}
	}

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

bool TestRandomI::testRandomPair(RandomGenerator& randomGenerator, const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Random pair test:";

	bool allSucceeded = true;

	constexpr unsigned int iterations = 1000u;

	const Timestamp startTimestamp(true);

	do
	{
		unsigned int first = 0u;
		unsigned int second = 0u;

		// first, we check for 1 as maximal value:

		RandomI::random(1u, first, second);

		Utilities::sortLowestToFront2(first, second);
		if (first != 0u || second != 1u)
		{
			allSucceeded = false;
		}

		first = 0u;
		second = 0u;

		RandomI::random(randomGenerator, 1u, first, second);

		Utilities::sortLowestToFront2(first, second);
		if (first != 0u || second != 1u)
		{
			allSucceeded = false;
		}

		// now, we check a random maximal value:

		const unsigned int maxValue = RandomI::random(1u, std::max(1u, RandomI::random32()));

		for (unsigned int n = 0u; n < iterations; ++n)
		{
			first = 0u;
			second = 0u;
			RandomI::random(maxValue, first, second);

			if (first == second || first > maxValue || second > maxValue)
			{
				allSucceeded = false;
			}
		}

		for (unsigned int n = 0u; n < iterations; ++n)
		{
			first = 0u;
			second = 0u;
			RandomI::random(randomGenerator, maxValue, first, second);

			if (first == second || first > maxValue || second > maxValue)
			{
				allSucceeded = false;
			}
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

bool TestRandomI::testRandomTriple(RandomGenerator& randomGenerator, const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Random triple test:";

	bool allSucceeded = true;

	constexpr unsigned int iterations = 1000u;

	const Timestamp startTimestamp(true);

	do
	{
		unsigned int first = 0u;
		unsigned int second = 0u;
		unsigned int third = 0u;

		// first, we check for 2 as maximal value:

		RandomI::random(2u, first, second, third);

		Utilities::sortLowestToFront3(first, second, third);
		if (first != 0u || second != 1u || third != 2u)
		{
			allSucceeded = false;
		}

		first = 0u;
		second = 0u;
		third = 0u;

		RandomI::random(randomGenerator, 2u, first, second, third);

		Utilities::sortLowestToFront3(first, second, third);
		if (first != 0u || second != 1u || third != 2u)
		{
			allSucceeded = false;
		}

		// now, we check a random maximal value:

		const unsigned int maxValue = RandomI::random(2u, std::max(2u, RandomI::random32(randomGenerator)));

		for (unsigned int n = 0u; n < iterations; ++n)
		{
			first = 0u;
			second = 0u;
			third = 0u;
			RandomI::random(maxValue, first, second, third);

			if (first == second || first == third || second == third || first > maxValue || second > maxValue || third > maxValue)
			{
				allSucceeded = false;
			}
		}

		for (unsigned int n = 0u; n < iterations; ++n)
		{
			first = 0u;
			second = 0u;
			third = 0u;
			RandomI::random(randomGenerator, maxValue, first, second, third);

			if (first == second || first == third || second == third || first > maxValue || second > maxValue || third > maxValue)
			{
				allSucceeded = false;
			}
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

bool TestRandomI::testRandomBoolean(RandomGenerator& randomGenerator, const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Random boolean test:";

	bool allSucceeded = true;

	constexpr unsigned int iterations = 100000u;

	const Timestamp startTimestamp(true);

	do
	{
		{
			unsigned int histogram[2] = {0u, 0u};

			for (unsigned int n = 0u; n < iterations; ++n)
			{
				const bool value = RandomI::boolean();

				if (value == false)
				{
					histogram[0]++;
				}
				else
				{
					histogram[1]++;
				}
			}

			const unsigned int difference = (unsigned int)(std::abs(int(histogram[0]) - int(histogram[1])));

			if (difference > iterations * 5u / 100u) // 5%
			{
				allSucceeded = false;
			}
		}

		{
			unsigned int histogram[2] = {0u, 0u};

			for (unsigned int n = 0u; n < iterations; ++n)
			{
				const bool value = RandomI::boolean(randomGenerator);

				if (value == false)
				{
					histogram[0]++;
				}
				else
				{
					histogram[1]++;
				}
			}

			const unsigned int difference = (unsigned int)(std::abs(int(histogram[0]) - int(histogram[1])));

			if (difference > iterations * 5u / 100u) // 5%
			{
				allSucceeded = false;
			}
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

bool TestRandomI::testRandomElementsVector(RandomGenerator& randomGenerator, const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Random elements per vector test:";

	bool allSucceeded = true;

	constexpr unsigned int iterations = 100000u;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int numberElements = RandomI::random(randomGenerator, 1u, 10u);

		std::vector<int> elements;
		for (unsigned int n = 0u; n < numberElements; ++n)
		{
			elements.push_back(int(n));
		}

		{
			Indices32 histogram(numberElements, 0u);

			for (size_t n = 0; n < iterations; ++n)
			{
				const int element = RandomI::random(elements);

				ocean_assert(element < int(histogram.size()));
				if (element < int(histogram.size()))
				{
					histogram[element]++;
				}
				else
				{
					allSucceeded = false;
				}
			}

			std::sort(histogram.begin(), histogram.end());

			const unsigned int difference = (unsigned int)(std::abs(int(histogram.front()) - int(histogram.back())));

			if (difference > iterations * 5u / 100u) // 5%
			{
				allSucceeded = false;
			}
		}

		{
			Indices32 histogram(numberElements, 0u);

			for (size_t n = 0; n < iterations; ++n)
			{
				const int element = RandomI::random(randomGenerator, elements);

				ocean_assert(element < int(histogram.size()));
				if (element < int(histogram.size()))
				{
					histogram[element]++;
				}
				else
				{
					allSucceeded = false;
				}
			}

			std::sort(histogram.begin(), histogram.end());

			const unsigned int difference = (unsigned int)(std::abs(int(histogram.front()) - int(histogram.back())));

			if (difference > iterations * 5u / 100u) // 5%
			{
				allSucceeded = false;
			}
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

bool TestRandomI::testRandomElementsInitializerList(RandomGenerator& randomGenerator, const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Random elements per initializer-list test:";

	bool allSucceeded = true;

	constexpr unsigned int iterations = 100000u;

	const Timestamp startTimestamp(true);

	do
	{
		const std::initializer_list<int> initializerList = {0, 1, 2, 3, 4};

		{
			Indices32 histogram(initializerList.size(), 0u);

			for (size_t n = 0; n < iterations; ++n)
			{
				const int element = RandomI::random(initializerList);

				ocean_assert(element < int(histogram.size()));
				if (element < int(histogram.size()))
				{
					histogram[element]++;
				}
				else
				{
					allSucceeded = false;
				}
			}

			std::sort(histogram.begin(), histogram.end());

			const unsigned int difference = (unsigned int)(std::abs(int(histogram.front()) - int(histogram.back())));

			if (difference > iterations * 5u / 100u) // 5%
			{
				allSucceeded = false;
			}
		}

		{
			Indices32 histogram(initializerList.size(), 0u);

			for (size_t n = 0; n < iterations; ++n)
			{
				const int element = RandomI::random(randomGenerator, initializerList);

				ocean_assert(element < int(histogram.size()));
				if (element < int(histogram.size()))
				{
					histogram[element]++;
				}
				else
				{
					allSucceeded = false;
				}
			}

			std::sort(histogram.begin(), histogram.end());

			const unsigned int difference = (unsigned int)(std::abs(int(histogram.front()) - int(histogram.back())));

			if (difference > iterations * 5u / 100u) // 5%
			{
				allSucceeded = false;
			}
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

bool TestRandomI::testExtremeValueRange(RandomGenerator& randomGenerator)
{
	Log::info() << "Testing extreme value range";

	bool allSucceeded = true;

	// just testing whether something unexpected happens - e.g., an assert

	const int offset = RandomI::random(randomGenerator, 1, 100000);

	{
		const unsigned int valueA = RandomI::random((unsigned int)(-1));
		const unsigned int valueB = RandomI::random(0u, (unsigned int)(-1));
		const int valueC = RandomI::random(0, 2147483647);
		const int valueD = RandomI::random(0 - offset, 2147483647 - offset);

		if (uint64_t(valueA) > uint64_t(uint32_t(-1)) // dummy test
			|| uint64_t(valueB) > uint64_t(uint32_t(-1))
			|| int64_t(valueC) > int64_t(uint32_t(-1))
			|| int64_t(valueD) > int64_t(uint32_t(-1)))
		{
			ocean_assert(false && " This should never happen!");
			allSucceeded = false;
		}
	}

	{
		const unsigned int valueA = RandomI::random(randomGenerator, (unsigned int)(-1));
		const unsigned int valueB = RandomI::random(randomGenerator, 0u, (unsigned int)(-1));
		const int valueC = RandomI::random(randomGenerator, 0, 2147483647);
		const int valueD = RandomI::random(randomGenerator, 0 - offset, 2147483647 - offset);

		if (uint64_t(valueA) > uint64_t(uint32_t(-1)) // dummy test
			|| uint64_t(valueB) > uint64_t(uint32_t(-1))
			|| int64_t(valueC) > int64_t(uint32_t(-1))
			|| int64_t(valueD) > int64_t(uint32_t(-1)))
		{
			ocean_assert(false && " This should never happen!");
			allSucceeded = false;
		}
	}

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

}

}

}
