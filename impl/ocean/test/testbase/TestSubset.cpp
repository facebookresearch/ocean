/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestSubset.h"

#include "ocean/base/DataType.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Subset.h"
#include "ocean/base/Timestamp.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

bool TestSubset::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Subset test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testSubset(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testInvertedSubset(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = hasIntersectingElement(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Subset test succeeded.";
	}
	else
	{
		Log::info() << "Subset test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestSubset, Subset)
{
	EXPECT_TRUE(TestSubset::testSubset(GTEST_TEST_DURATION));
}

TEST(TestSubset, InvertedSubset)
{
	EXPECT_TRUE(TestSubset::testInvertedSubset(GTEST_TEST_DURATION));
}

TEST(TestSubset, IntersectingElement)
{
	EXPECT_TRUE(TestSubset::hasIntersectingElement(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestSubset::testSubset(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Normal subset test:";

	bool allSucceeded = true;

	allSucceeded = testSubset<unsigned short>(testDuration) && allSucceeded;
	allSucceeded = testSubset<unsigned int>(testDuration) && allSucceeded;
	allSucceeded = testSubset<unsigned long long>(testDuration) && allSucceeded;

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

bool TestSubset::testInvertedSubset(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Inverted subset test:";

	bool allSucceeded = true;

	allSucceeded = testInvertedSubset<unsigned short>(testDuration) && allSucceeded;
	allSucceeded = testInvertedSubset<unsigned int>(testDuration) && allSucceeded;
	allSucceeded = testInvertedSubset<unsigned long long>(testDuration) && allSucceeded;

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

template <typename TIndex>
bool TestSubset::testSubset(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for \"" << TypeNamer::name<TIndex>() << "\" indices:";

	typedef std::set<TIndex> IndexSet;
	typedef std::vector<TIndex> Indices;

	Indices32 setInteger32;
	Indices64 setInteger64;
	std::vector<std::string> setString;

	bool allSucceeded = true;
	bool firstIteration = true;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int setSize = RandomI::random(1u, 5000u);

		setInteger32.resize(setSize);
		setInteger64.resize(setSize);
		setString.resize(setSize);

		for (unsigned int n = 0u; n < setSize; ++n)
		{
			setInteger32[n] = n;
			setInteger64[n] = n;
			setString[n] = String::toAString(n);
		}

		ocean_assert(setSize >= 1u);
		const unsigned int subsetSize = firstIteration ? setSize : RandomI::random(setSize - 1u);

		IndexSet subsetIndicesSet;
		while (subsetIndicesSet.size() < subsetSize)
		{
			subsetIndicesSet.insert(TIndex(RandomI::random(setSize - 1u)));
		}

		const Indices subsetIndicesArray(subsetIndicesSet.begin(), subsetIndicesSet.end());

		{
			const Indices32 subset32 = Subset::subset(setInteger32, subsetIndicesSet);

			if (subset32.size() != subsetIndicesSet.size())
			{
				allSucceeded = false;
			}

			for (Indices32::const_iterator i = subset32.begin(); i != subset32.end(); ++i)
			{
				if (subsetIndicesSet.find(TIndex(*i)) == subsetIndicesSet.end())
				{
					allSucceeded = false;
				}
			}
		}
		{
			const Indices32 subset32 = Subset::subset(setInteger32.data(), setInteger32.size(), subsetIndicesSet);

			if (subset32.size() != subsetIndicesSet.size())
			{
				allSucceeded = false;
			}

			for (Indices32::const_iterator i = subset32.begin(); i != subset32.end(); ++i)
			{
				if (subsetIndicesSet.find(TIndex(*i)) == subsetIndicesSet.end())
				{
					allSucceeded = false;
				}
			}
		}
		{
			const Indices32 subset32 = Subset::subset(setInteger32, subsetIndicesArray);

			if (subset32.size() != subsetIndicesArray.size())
			{
				allSucceeded = false;
			}

			for (Indices32::const_iterator i = subset32.begin(); i != subset32.end(); ++i)
			{
				if (subsetIndicesSet.find(TIndex(*i)) == subsetIndicesSet.end())
				{
					allSucceeded = false;
				}
			}
		}
		{
			const Indices32 subset32 = Subset::subset(setInteger32.data(), setInteger32.size(), subsetIndicesArray);

			if (subset32.size() != subsetIndicesArray.size())
			{
				allSucceeded = false;
			}

			for (Indices32::const_iterator i = subset32.begin(); i != subset32.end(); ++i)
			{
				if (subsetIndicesSet.find(TIndex(*i)) == subsetIndicesSet.end())
				{
					allSucceeded = false;
				}
			}
		}
		{
			const Indices32 subset32 = Subset::subset(setInteger32.data(), setInteger32.size(), subsetIndicesArray.data(), subsetIndicesArray.size());

			if (subset32.size() != subsetIndicesArray.size())
			{
				allSucceeded = false;
			}

			for (Indices32::const_iterator i = subset32.begin(); i != subset32.end(); ++i)
			{
				if (subsetIndicesSet.find(TIndex(*i)) == subsetIndicesSet.end())
				{
					allSucceeded = false;
				}
			}
		}



		{
			const Indices64 subset64 = Subset::subset(setInteger64, subsetIndicesSet);

			if (subset64.size() != subsetIndicesSet.size())
			{
				allSucceeded = false;
			}

			for (Indices64::const_iterator i = subset64.begin(); i != subset64.end(); ++i)
			{
				if (subsetIndicesSet.find(TIndex(*i)) == subsetIndicesSet.end())
				{
					allSucceeded = false;
				}
			}
		}
		{
			const Indices64 subset64 = Subset::subset(setInteger64.data(), setInteger64.size(), subsetIndicesSet);

			if (subset64.size() != subsetIndicesSet.size())
			{
				allSucceeded = false;
			}

			for (Indices64::const_iterator i = subset64.begin(); i != subset64.end(); ++i)
			{
				if (subsetIndicesSet.find(TIndex(*i)) == subsetIndicesSet.end())
				{
					allSucceeded = false;
				}
			}
		}
		{
			const Indices64 subset64 = Subset::subset(setInteger64, subsetIndicesArray);

			if (subset64.size() != subsetIndicesArray.size())
			{
				allSucceeded = false;
			}

			for (Indices64::const_iterator i = subset64.begin(); i != subset64.end(); ++i)
			{
				if (subsetIndicesSet.find(TIndex(*i)) == subsetIndicesSet.end())
				{
					allSucceeded = false;
				}
			}
		}
		{
			const Indices64 subset64 = Subset::subset(setInteger64.data(), setInteger64.size(), subsetIndicesArray);

			if (subset64.size() != subsetIndicesArray.size())
			{
				allSucceeded = false;
			}

			for (Indices64::const_iterator i = subset64.begin(); i != subset64.end(); ++i)
			{
				if (subsetIndicesSet.find(TIndex(*i)) == subsetIndicesSet.end())
				{
					allSucceeded = false;
				}
			}
		}
		{
			const Indices64 subset64 = Subset::subset(setInteger64.data(), setInteger64.size(), subsetIndicesArray.data(), subsetIndicesArray.size());

			if (subset64.size() != subsetIndicesArray.size())
			{
				allSucceeded = false;
			}

			for (Indices64::const_iterator i = subset64.begin(); i != subset64.end(); ++i)
			{
				if (subsetIndicesSet.find(TIndex(*i)) == subsetIndicesSet.end())
				{
					allSucceeded = false;
				}
			}
		}



		{
			const std::vector<std::string> strings = Subset::subset(setString, subsetIndicesSet);

			if (strings.size() != subsetIndicesSet.size())
			{
				allSucceeded = false;
			}

			int value = -1;

			for (std::vector<std::string>::const_iterator i = strings.begin(); i != strings.end(); ++i)
			{
				if (!String::isInteger32(*i, &value) || value < 0 || subsetIndicesSet.find(TIndex(value)) == subsetIndicesSet.end())
				{
					allSucceeded = false;
				}
			}
		}
		{
			const std::vector<std::string> strings = Subset::subset(setString.data(), setString.size(), subsetIndicesSet);

			if (strings.size() != subsetIndicesSet.size())
			{
				allSucceeded = false;
			}

			int value = -1;

			for (std::vector<std::string>::const_iterator i = strings.begin(); i != strings.end(); ++i)
			{
				if (!String::isInteger32(*i, &value) || value < 0 || subsetIndicesSet.find(TIndex(value)) == subsetIndicesSet.end())
				{
					allSucceeded = false;
				}
			}
		}
		{
			const std::vector<std::string> strings = Subset::subset(setString, subsetIndicesArray);

			if (strings.size() != subsetIndicesSet.size())
			{
				allSucceeded = false;
			}

			int value = -1;

			for (std::vector<std::string>::const_iterator i = strings.begin(); i != strings.end(); ++i)
			{
				if (!String::isInteger32(*i, &value) || value < 0 || subsetIndicesSet.find(TIndex(value)) == subsetIndicesSet.end())
				{
					allSucceeded = false;
				}
			}
		}
		{
			const std::vector<std::string> strings = Subset::subset(setString.data(), setString.size(), subsetIndicesArray);

			if (strings.size() != subsetIndicesSet.size())
			{
				allSucceeded = false;
			}

			int value = -1;

			for (std::vector<std::string>::const_iterator i = strings.begin(); i != strings.end(); ++i)
			{
				if (!String::isInteger32(*i, &value) || value < 0 || subsetIndicesSet.find(TIndex(value)) == subsetIndicesSet.end())
				{
					allSucceeded = false;
				}
			}
		}
		{
			const std::vector<std::string> strings = Subset::subset(setString.data(), setString.size(), subsetIndicesArray.data(), subsetIndicesArray.size());

			if (strings.size() != subsetIndicesSet.size())
			{
				allSucceeded = false;
			}

			int value = -1;

			for (std::vector<std::string>::const_iterator i = strings.begin(); i != strings.end(); ++i)
			{
				if (!String::isInteger32(*i, &value) || value < 0 || subsetIndicesSet.find(TIndex(value)) == subsetIndicesSet.end())
				{
					allSucceeded = false;
				}
			}
		}

		firstIteration = false;
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

template <typename TIndex>
bool TestSubset::testInvertedSubset(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for \"" << TypeNamer::name<TIndex>() << "\" indices:";

	typedef std::set<TIndex> IndexSet;
	typedef std::vector<TIndex> Indices;

	Indices32 setInteger32;
	Indices64 setInteger64;
	std::vector<std::string> setString;

	bool allSucceeded = true;
	bool firstIteration = true;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int setSize = RandomI::random(1u, 5000u);

		setInteger32.resize(setSize);
		setInteger64.resize(setSize);
		setString.resize(setSize);

		for (unsigned int n = 0u; n < setSize; ++n)
		{
			setInteger32[n] = n;
			setInteger64[n] = n;
			setString[n] = String::toAString(n);
		}

		ocean_assert(setSize >= 1u);
		const unsigned int subsetSize = firstIteration ? setSize : RandomI::random(setSize - 1u);

		IndexSet subsetIndicesSet;
		while (subsetIndicesSet.size() < subsetSize)
		{
			subsetIndicesSet.insert(TIndex(RandomI::random(setSize - 1u)));
		}

		const Indices subsetIndicesArray(subsetIndicesSet.begin(), subsetIndicesSet.end());

		IndexSet invertedSubsetIndicesSet;
		for (unsigned int n = 0u; n < setSize; ++n)
		{
			if (subsetIndicesSet.find(TIndex(n)) == subsetIndicesSet.end())
			{
				invertedSubsetIndicesSet.insert(TIndex(n));
			}
		}

		{
			const IndexSet invertedSet = Subset::invertedIndices(subsetIndicesSet, setSize);

			if (invertedSet.size() != invertedSubsetIndicesSet.size())
			{
				allSucceeded = false;
			}

			for (typename IndexSet::const_iterator i = invertedSet.begin(); i != invertedSet.end(); ++i)
			{
				if (invertedSubsetIndicesSet.find(*i) == invertedSubsetIndicesSet.end())
				{
					allSucceeded = false;
				}
			}
		}

		{
			const Indices invertedArray = Subset::invertedIndices(subsetIndicesArray, setSize);

			if (invertedArray.size() != invertedSubsetIndicesSet.size())
			{
				allSucceeded = false;
			}

			for (typename Indices::const_iterator i = invertedArray.begin(); i != invertedArray.end(); ++i)
			{
				if (invertedSubsetIndicesSet.find(*i) == invertedSubsetIndicesSet.end())
				{
					allSucceeded = false;
				}
			}
		}

		{
			const Indices32 integers32 = Subset::invertedSubset(setInteger32, subsetIndicesSet);

			if (integers32.size() != invertedSubsetIndicesSet.size())
			{
				allSucceeded = false;
			}

			for (Indices32::const_iterator i = integers32.begin(); i != integers32.end(); ++i)
			{
				if (invertedSubsetIndicesSet.find(TIndex(*i)) == invertedSubsetIndicesSet.end())
				{
					allSucceeded = false;
				}
			}
		}

		{
			const Indices32 integers32 = Subset::invertedSubset(setInteger32.data(), setInteger32.size(), subsetIndicesSet);

			if (integers32.size() != invertedSubsetIndicesSet.size())
			{
				allSucceeded = false;
			}

			for (Indices32::const_iterator i = integers32.begin(); i != integers32.end(); ++i)
			{
				if (invertedSubsetIndicesSet.find(TIndex(*i)) == invertedSubsetIndicesSet.end())
				{
					allSucceeded = false;
				}
			}
		}

		{
			const Indices64 integers64 = Subset::invertedSubset(setInteger64, subsetIndicesSet);

			if (integers64.size() != invertedSubsetIndicesSet.size())
			{
				allSucceeded = false;
			}

			for (Indices64::const_iterator i = integers64.begin(); i != integers64.end(); ++i)
			{
				if (invertedSubsetIndicesSet.find(TIndex(*i)) == invertedSubsetIndicesSet.end())
				{
					allSucceeded = false;
				}
			}
		}

		{
			const Indices64 integers64 = Subset::invertedSubset(setInteger64.data(), setInteger64.size(), subsetIndicesSet);

			if (integers64.size() != invertedSubsetIndicesSet.size())
			{
				allSucceeded = false;
			}

			for (Indices64::const_iterator i = integers64.begin(); i != integers64.end(); ++i)
			{
				if (invertedSubsetIndicesSet.find(TIndex(*i)) == invertedSubsetIndicesSet.end())
				{
					allSucceeded = false;
				}
			}
		}

		{
			const std::vector<std::string> strings = Subset::invertedSubset(setString, subsetIndicesSet);

			if (strings.size() != invertedSubsetIndicesSet.size())
			{
				allSucceeded = false;
			}

			int value = -1;

			for (std::vector<std::string>::const_iterator i = strings.begin(); i != strings.end(); ++i)
			{
				if (!String::isInteger32(*i, &value) || value < 0 || invertedSubsetIndicesSet.find(TIndex(value)) == invertedSubsetIndicesSet.end())
				{
					allSucceeded = false;
				}
			}
		}

		{
			const std::vector<std::string> strings = Subset::invertedSubset(setString.data(), setString.size(), subsetIndicesSet);

			if (strings.size() != invertedSubsetIndicesSet.size())
			{
				allSucceeded = false;
			}

			int value = -1;

			for (std::vector<std::string>::const_iterator i = strings.begin(); i != strings.end(); ++i)
			{
				if (!String::isInteger32(*i, &value) || value < 0 || invertedSubsetIndicesSet.find(TIndex(value)) == invertedSubsetIndicesSet.end())
				{
					allSucceeded = false;
				}
			}
		}

		firstIteration = false;
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

bool TestSubset::hasIntersectingElement(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test hasIntersectingElement():";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		for (const bool hasIntersectionIteration : {false, true})
		{
			std::unordered_set<uint32_t> unionIndices;

			const unsigned int numberUnionIndices = RandomI::random(randomGenerator, 1u, 1000u);
			while (unionIndices.size() < numberUnionIndices)
			{
				unionIndices.emplace(RandomI::random(randomGenerator, numberUnionIndices * 10u));
			}

			std::vector<uint32_t> vectorA;
			std::vector<uint32_t> vectorB;

			std::set<uint32_t> setA;
			std::set<uint32_t> setB;

			const unsigned int half = RandomI::random(randomGenerator, (unsigned int)(unionIndices.size()) - 1u);

			std::unordered_set<uint32_t>::const_iterator iUnion = unionIndices.cbegin();

			const unsigned int halfLast = hasIntersectionIteration ? half + 1u : half;

			for (unsigned int n = 0u; n < halfLast; ++n)
			{
				ocean_assert(iUnion != unionIndices.cend());

				vectorA.emplace_back(*iUnion);
				setA.emplace(*iUnion);

				if (n != half)
				{
					++iUnion;
				}
			}

			ocean_assert(iUnion != unionIndices.cend());

			while (iUnion != unionIndices.cend())
			{
				vectorB.emplace_back(*iUnion);
				setB.emplace(*iUnion);

				++iUnion;
			}

			std::sort(vectorA.begin(), vectorA.end());
			std::sort(vectorB.begin(), vectorB.end());

			if (Subset::hasIntersectingElement(vectorA, vectorB) != hasIntersectionIteration)
			{
				allSucceeded = false;
			}

			if (Subset::hasIntersectingElement(setA, setB) != hasIntersectionIteration)
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

}

}

}
