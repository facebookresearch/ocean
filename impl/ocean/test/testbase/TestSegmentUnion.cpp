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

#include <stddef.h>

namespace Ocean
{

namespace Test
{

namespace TestBase
{

bool TestSegmentUnion::test(const double testDuration)
{
	ocean_assert(testDuration > 0);

	Log::info() << "---   SegmentUnion test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testUnionSize<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testUnionSize<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testIntersection<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testIntersection<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMaximalGap<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testMaximalGap<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testBoolCastOperator<float>() && allSucceeded;
	Log::info() << " ";
	allSucceeded = testBoolCastOperator<double>() && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "SegmentUnion test succeeded.";
	}
	else
	{
		Log::info() << "SegmentUnion test FAILED!";
	}

	return allSucceeded;
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

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		ShiftVector<unsigned char> testElements;
		SegmentUnion<T> segmentUnion;

		// we select a random amount of segments
		const unsigned int segments = RandomI::random(25u);

		for (unsigned int n = 0u; n < segments; ++n)
		{
			// we select a random segment [startPosition, stopPosition] and we fill the corresponding elements

			const int startPosition = RandomI::random(-1000, 999);
			const int stopPosition = RandomI::random(startPosition + 1, 1000);

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

		if (unionSize != T(activeElements))
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
bool TestSegmentUnion::testIntersection(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Intersection test for '" << TypeNamer::name<T>() << "':";

	// we test the correctness with pixel accurate locations
	// thus, we can simply use an array and set elements of an array to 1 if they are covered by a segment
	// finally, we count the elements with 1 and have the length

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		ShiftVector<unsigned char> testElements;
		SegmentUnion<T> segmentUnion;

		// we select a random amount of segments
		const unsigned int segments = RandomI::random(25u);

		for (unsigned int n = 0u; n < segments; ++n)
		{
			// we select a random segment [startPosition, stopPosition] and we fill the corresponding elements

			const int startPosition = RandomI::random(-1000, 999);
			const int stopPosition = RandomI::random(startPosition + 1, 1000);

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

		const int rangeStartPosition = RandomI::random(-1000, 999);
		const int rangeStopPosition = RandomI::random(rangeStartPosition + 1, 1000);

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
					allSucceeded = false;
					break;
				}
			
				ptrdiff_t segmentStop = segmentStart + 1;
				while (segmentStop < testElements.endIndex() && testElements[segmentStop] != 0u)
				{
					segmentStop++;
				}

				if (i->first != T(segmentStart) || i->second != T(segmentStop))
				{
					allSucceeded = false;
					break;
				}

				segmentStart = segmentStop;
			}

			for (ptrdiff_t i = segmentStart; i < testElements.endIndex(); ++i)
			{
				if (testElements[i] != 0u)
				{
					allSucceeded = false;
				}
			}
		}
		else
		{
			if (intersectionUnion)
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

template <typename T>
bool TestSegmentUnion::testMaximalGap(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Maximal gap test for '" << TypeNamer::name<T>() << "':";

	// we test the correctness with pixel accurate locations
	// thus, we can simply use an array and set elements of an array to 1 if they are covered by a segment
	// finally, we count the elements with 1 and have the length

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		ShiftVector<unsigned char> testElements;
		SegmentUnion<T> segmentUnion;

		// we select a random amount of segments
		const unsigned int segments = RandomI::random(25u);

		for (unsigned int n = 0u; n < segments; ++n)
		{
			// we select a random segment [startPosition, stopPosition] and we fill the corresponding elements

			const int startPosition = RandomI::random(-1000, 999);
			const int stopPosition = RandomI::random(startPosition + 1, 1000);

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
						break;
				}

				const ptrdiff_t nonActiveBlockEnd = i; // exclusive

				const unsigned int blockSize = (unsigned int)(nonActiveBlockEnd - nonActiveBlockStart);
				
				maximalNonActiveElements = std::max(maximalNonActiveElements, blockSize);
			}
		}

		const T maximalGap = segmentUnion.maximalGap();

		if (T(maximalNonActiveElements) != maximalGap)
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
bool TestSegmentUnion::testBoolCastOperator()
{
	Log::info() << "Bool cast operator '" << TypeNamer::name<T>() << "':";

	bool allSucceeded = true;

	SegmentUnion<T> segmentUnion;

	if (segmentUnion)
	{
		allSucceeded = false;
	}

	segmentUnion.addSegment(T(5), T(10));

	if (!segmentUnion)
	{
		allSucceeded = false;
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
