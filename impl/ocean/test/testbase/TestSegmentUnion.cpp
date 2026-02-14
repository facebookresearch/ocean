/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestSegmentUnion.h"

#include "ocean/base/DataType.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/SegmentUnion.h"
#include "ocean/base/ShiftVector.h"
#include "ocean/base/Timestamp.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/TestSelector.h"
#include "ocean/test/Validation.h"

#include <stddef.h>

namespace Ocean
{

namespace Test
{

namespace TestBase
{

bool TestSegmentUnion::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0);

	TestResult testResult("SegmentUnion test");
	Log::info() << " ";

	if (selector.shouldRun("unionsize"))
	{
		testResult = testUnionSize<float>(testDuration);
		Log::info() << " ";
		testResult = testUnionSize<double>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("intersection"))
	{
		testResult = testIntersection<float>(testDuration);
		Log::info() << " ";
		testResult = testIntersection<double>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("maximalgap"))
	{
		testResult = testMaximalGap<float>(testDuration);
		Log::info() << " ";
		testResult = testMaximalGap<double>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("boolcastoperator"))
	{
		testResult = testBoolCastOperator<float>();
		Log::info() << " ";
		testResult = testBoolCastOperator<double>();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestSegmentUnion, UnionSizeFloat)
{
	EXPECT_TRUE(TestSegmentUnion::testUnionSize<float>(GTEST_TEST_DURATION));
}

TEST(TestSegmentUnion, UnionSizeDouble)
{
	EXPECT_TRUE(TestSegmentUnion::testUnionSize<double>(GTEST_TEST_DURATION));
}

TEST(TestSegmentUnion, IntersectionFloat)
{
	EXPECT_TRUE(TestSegmentUnion::testIntersection<float>(GTEST_TEST_DURATION));
}

TEST(TestSegmentUnion, IntersectionDouble)
{
	EXPECT_TRUE(TestSegmentUnion::testIntersection<double>(GTEST_TEST_DURATION));
}

TEST(TestSegmentUnion, MaximalGapFloat)
{
	EXPECT_TRUE(TestSegmentUnion::testMaximalGap<float>(GTEST_TEST_DURATION));
}

TEST(TestSegmentUnion, MaximalGapDouble)
{
	EXPECT_TRUE(TestSegmentUnion::testMaximalGap<double>(GTEST_TEST_DURATION));
}

TEST(TestSegmentUnion, BoolCastOperatorFloat)
{
	EXPECT_TRUE(TestSegmentUnion::testBoolCastOperator<float>());
}

TEST(TestSegmentUnion, BoolCastOperatorDouble)
{
	EXPECT_TRUE(TestSegmentUnion::testBoolCastOperator<double>());
}

#endif // OCEAN_USE_GTEST

template <typename T>
bool TestSegmentUnion::testUnionSize(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Union size test for '" << TypeNamer::name<T>() << "':";

	// we test the correctness with pixel accurate locations
	// thus, we can simply use an array and set elements of an array to 1 if they are covered by a segment
	// finally, we count the elements with 1 and have the length

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		ShiftVector<unsigned char> testElements;
		SegmentUnion<T> segmentUnion;

		// we select a random amount of segments
		const unsigned int segments = RandomI::random(randomGenerator, 25u);

		for (unsigned int n = 0u; n < segments; ++n)
		{
			// we select a random segment [startPosition, stopPosition] and we fill the corresponding elements

			const int startPosition = RandomI::random(randomGenerator, -1000, 999);
			const int stopPosition = RandomI::random(randomGenerator, startPosition + 1, 1000);

			if (testElements.isEmpty())
			{
				testElements.setFirstIndex(startPosition);
			}

			for (int i = startPosition; i < stopPosition; ++i)
			{
				testElements.insert(i, 1u);
			}

			segmentUnion.addSegment(T(startPosition), T(stopPosition));
		}

		// we count the active elements to determine the overall length

		unsigned int activeElements = 0u;

		if (!testElements.isEmpty())
		{
			for (ptrdiff_t i = testElements.firstIndex(); i < testElements.endIndex(); ++i)
			{
				if (testElements[i] != 0u)
				{
					activeElements++;
				}
			}
		}

		const T unionSize = segmentUnion.unionSize();

		OCEAN_EXPECT_EQUAL(validation, unionSize, T(activeElements));
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestSegmentUnion::testIntersection(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Intersection test for '" << TypeNamer::name<T>() << "':";

	// we test the correctness with pixel accurate locations
	// thus, we can simply use an array and set elements of an array to 1 if they are covered by a segment
	// finally, we count the elements with 1 and have the length

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		ShiftVector<unsigned char> testElements;
		SegmentUnion<T> segmentUnion;

		// we select a random amount of segments
		const unsigned int segments = RandomI::random(randomGenerator, 25u);

		for (unsigned int n = 0u; n < segments; ++n)
		{
			// we select a random segment [startPosition, stopPosition] and we fill the corresponding elements

			const int startPosition = RandomI::random(randomGenerator, -1000, 999);
			const int stopPosition = RandomI::random(randomGenerator, startPosition + 1, 1000);

			if (testElements.isEmpty())
			{
				testElements.setFirstIndex(startPosition);
			}

			for (int i = startPosition; i < stopPosition; ++i)
			{
				testElements.insert(i, 1u);
			}

			segmentUnion.addSegment(T(startPosition), T(stopPosition));
		}

		// now we select a range randomly

		const int rangeStartPosition = RandomI::random(randomGenerator, -1000, 999);
		const int rangeStopPosition = RandomI::random(randomGenerator, rangeStartPosition + 1, 1000);

		// we invalidate every pixel outside the range

		if (!testElements.isEmpty())
		{
			for (ptrdiff_t i = testElements.firstIndex(); i < testElements.endIndex(); ++i)
			{
				if (i < int(rangeStartPosition))
				{
					testElements[i] = 0u;
				}
				else if (i >= int(rangeStopPosition))
				{
					testElements[i] = 0u;
				}
			}
		}

		const SegmentUnion<T> intersectionUnion = segmentUnion.intersection(T(rangeStartPosition), T(rangeStopPosition));

		if (!testElements.isEmpty())
		{
			const typename SegmentUnion<T>::SegmentMap& segmentMap = intersectionUnion.segments();

			ptrdiff_t segmentStart = testElements.firstIndex();

			for (typename SegmentUnion<T>::SegmentMap::const_iterator i = segmentMap.cbegin(); i != segmentMap.cend(); ++i)
			{
				while (segmentStart < testElements.endIndex() && testElements[segmentStart] == 0u)
				{
					segmentStart++;
				}

				if (segmentStart == testElements.endIndex())
				{
					OCEAN_SET_FAILED(validation);
					break;
				}

				ptrdiff_t segmentStop = segmentStart + 1;
				while (segmentStop < testElements.endIndex() && testElements[segmentStop] != 0u)
				{
					segmentStop++;
				}

				if (i->first != T(segmentStart) || i->second != T(segmentStop))
				{
					OCEAN_SET_FAILED(validation);
					break;
				}

				segmentStart = segmentStop;
			}

			for (ptrdiff_t i = segmentStart; i < testElements.endIndex(); ++i)
			{
				if (testElements[i] != 0u)
				{
					OCEAN_SET_FAILED(validation);
				}
			}
		}
		else
		{
			OCEAN_EXPECT_TRUE(validation, !intersectionUnion);
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestSegmentUnion::testMaximalGap(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Maximal gap test for '" << TypeNamer::name<T>() << "':";

	// we test the correctness with pixel accurate locations
	// thus, we can simply use an array and set elements of an array to 1 if they are covered by a segment
	// finally, we count the elements with 1 and have the length

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		ShiftVector<unsigned char> testElements;
		SegmentUnion<T> segmentUnion;

		// we select a random amount of segments
		const unsigned int segments = RandomI::random(randomGenerator, 25u);

		for (unsigned int n = 0u; n < segments; ++n)
		{
			// we select a random segment [startPosition, stopPosition] and we fill the corresponding elements

			const int startPosition = RandomI::random(randomGenerator, -1000, 999);
			const int stopPosition = RandomI::random(randomGenerator, startPosition + 1, 1000);

			if (testElements.isEmpty())
			{
				testElements.setFirstIndex(startPosition);
			}

			for (int i = startPosition; i < stopPosition; ++i)
			{
				testElements.insert(i, 1u);
			}

			segmentUnion.addSegment(T(startPosition), T(stopPosition));
		}

		// we determine the longest block of non-active elements

		unsigned int maximalNonActiveElements = 0u;

		if (!testElements.isEmpty())
		{
			ptrdiff_t i = testElements.firstIndex();

			ocean_assert(testElements.front() != 0u);
			ocean_assert(testElements.back() != 0u);

			while (++i < testElements.endIndex())
			{
				if (testElements[i] != 0u)
				{
					continue;
				}

				const ptrdiff_t nonActiveBlockStart = i; // inclusive

				while (++i < testElements.endIndex())
				{
					if (testElements[i] != 0u)
					{
						break;
					}
				}

				const ptrdiff_t nonActiveBlockEnd = i; // exclusive

				const unsigned int blockSize = (unsigned int)(nonActiveBlockEnd - nonActiveBlockStart);

				maximalNonActiveElements = std::max(maximalNonActiveElements, blockSize);
			}
		}

		const T maximalGap = segmentUnion.maximalGap();

		OCEAN_EXPECT_EQUAL(validation, T(maximalNonActiveElements), maximalGap);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestSegmentUnion::testBoolCastOperator()
{
	Log::info() << "Bool cast operator '" << TypeNamer::name<T>() << "':";

	Validation validation;

	SegmentUnion<T> segmentUnion;

	OCEAN_EXPECT_TRUE(validation, !segmentUnion);

	segmentUnion.addSegment(T(5), T(10));

	OCEAN_EXPECT_TRUE(validation, bool(segmentUnion));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
