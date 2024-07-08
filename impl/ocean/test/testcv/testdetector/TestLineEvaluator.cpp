/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testdetector/TestLineEvaluator.h"

#include "ocean/base/DataType.h"
#include "ocean/base/RandomI.h"

#include "ocean/math/FiniteLine2.h"

#include "ocean/cv/detector/LineEvaluator.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

using namespace Ocean::CV::Detector;

bool TestLineEvaluator::test(const double testDuration, Worker& /*worker*/)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Line evaluator test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testMatchDetermination<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testMatchDetermination<double>(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Line evaluator test succeeded.";
	}
	else
	{
		Log::info() << "Line evaluator test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestLineEvaluator, MatchDeterminationFloat)
{
	EXPECT_TRUE(TestLineEvaluator::testMatchDetermination<float>(GTEST_TEST_DURATION));
}

TEST(TestLineEvaluator, MatchDeterminationDouble)
{
	EXPECT_TRUE(TestLineEvaluator::testMatchDetermination<double>(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

template <typename T>
bool TestLineEvaluator::testMatchDetermination(const double /*testDuration*/)
{
	Log::info() << "Match determination test for " << TypeNamer::name<T>() << "':";

	typedef std::unordered_map<LineEvaluator::Id, FiniteLineT2<T>> LineMap;

	bool allSucceeded = true;

	// for now we test static data, however we need to extend the test to use random data

	LineMap linesGroundTruth;
	LineMap linesEvaluation;

	/*
	 * some perfect matches:
	 *
	 * ground truth lines: ++++++++++++++
	 *   evaluation lines: --------------
	 */

	linesGroundTruth[0] = FiniteLineT2<T>(VectorT2<T>(T(5), T(5)), VectorT2<T>(T(10), T(10)));
	linesEvaluation[0] = FiniteLineT2<T>(VectorT2<T>(T(5.05), T(4.95)), VectorT2<T>(T(9.98), T(10.21)));

	linesGroundTruth[2] = FiniteLineT2<T>(VectorT2<T>(T(205), T(-405)), VectorT2<T>(T(-1000), T(39)));
	linesEvaluation[2] = FiniteLineT2<T>(VectorT2<T>(T(205.7), T(-404.79)), VectorT2<T>(T(-1000), T(39.2)));


	/*
	 * some partial matches:
	 *
	 * ground truth lines:  ++++++++++++++++++++++
	 *   evaluation lines: -------- -------- ----
	 */

	linesGroundTruth[4] = FiniteLineT2<T>(VectorT2<T>(T(500), T(500)), VectorT2<T>(T(1000), T(1000)));
	linesEvaluation[4] = FiniteLineT2<T>(VectorT2<T>(T(701), T(699)), VectorT2<T>(T(850.5), T(850.2)));

	linesGroundTruth[6] = FiniteLineT2<T>(VectorT2<T>(T(18), T(-21)), VectorT2<T>(T(211), T(-21)));
	linesEvaluation[6] = FiniteLineT2<T>(VectorT2<T>(T(26), T(-21.7)), VectorT2<T>(T(100), T(-20.9)));
	linesEvaluation[7] = FiniteLineT2<T>(VectorT2<T>(T(78), T(-20.85)), VectorT2<T>(T(200), T(-21.1)));
	linesEvaluation[8] = FiniteLineT2<T>(VectorT2<T>(T(202), T(-21.18)), VectorT2<T>(T(212.8), T(-21.8)));


	/*
	 * some complex matches:
	 *
	 * ground truth lines:  ++++++++++++++++++++++ +++++++++++++ ++++++++++
	 *   evaluation lines: -------- ----------------------- -------- -----
	 */

	linesGroundTruth[10] = FiniteLineT2<T>(VectorT2<T>(T(200), T(100)), VectorT2<T>(T(-600), T(100)));
	linesGroundTruth[11] = FiniteLineT2<T>(VectorT2<T>(T(202), T(100)), VectorT2<T>(T(500), T(100)));
	linesEvaluation[10] = FiniteLineT2<T>(VectorT2<T>(T(-100), T(101)), VectorT2<T>(T(350), T(99)));

	const LineEvaluator::LineMatchMap lineMatches = LineEvaluator::evaluateLineSegments(linesGroundTruth, linesEvaluation);

	// now we verify the found matches

	// perfect match with id 0:
	LineEvaluator::LineMatchMap::const_iterator iMatch = lineMatches.find(0);
	if (iMatch == lineMatches.end() || !verifyMatch(iMatch->second, LineEvaluator::LineMatch::MT_PERFECT, {0}))
	{
		allSucceeded = false;
	}

	// perfect math with id 2:
	iMatch = lineMatches.find(2);
	if (iMatch == lineMatches.end() || !verifyMatch(iMatch->second, LineEvaluator::LineMatch::MT_PERFECT, {2}))
	{
		allSucceeded = false;
	}

	// partial math with id 4:
	iMatch = lineMatches.find(4);
	if (iMatch == lineMatches.end() || !verifyMatch(iMatch->second, LineEvaluator::LineMatch::MT_PARTIAL, {4}))
	{
		allSucceeded = false;
	}

	// partial math with id 6:
	iMatch = lineMatches.find(6);
	if (iMatch == lineMatches.end() || !verifyMatch(iMatch->second, LineEvaluator::LineMatch::MT_PARTIAL, {6, 7, 8}))
	{
		allSucceeded = false;
	}

	// complex math with id 10:
	iMatch = lineMatches.find(10);
	if (iMatch == lineMatches.end() || !verifyMatch(iMatch->second, LineEvaluator::LineMatch::MT_COMPLEX, {10}))
	{
		allSucceeded = false;
	}

	// complex math with id 11:
	iMatch = lineMatches.find(11);
	if (iMatch == lineMatches.end() || !verifyMatch(iMatch->second, LineEvaluator::LineMatch::MT_COMPLEX, {10}))
	{
		allSucceeded = false;
	}

	// we must have 6 matches

	if (lineMatches.size() != 6)
	{
		allSucceeded = false;
	}

	if (allSucceeded)
	{
		Log::info() << "Validation succeeded.";
	}
	else
	{
		Log::info() << "Validation FAILED!";
	}

	return allSucceeded;
}

bool TestLineEvaluator::verifyMatch(const CV::Detector::LineEvaluator::LineMatchRef& match, const CV::Detector::LineEvaluator::LineMatch::MatchType matchType, const CV::Detector::LineEvaluator::IdSet& targetIds)
{
	ocean_assert(match);
	ocean_assert(targetIds.empty() == false);

	if (match->matchType() != matchType)
	{
		return false;
	}

	switch (matchType)
	{
		case LineEvaluator::LineMatch::MT_PERFECT:
		{
			ocean_assert(targetIds.size() == 1);
			if (targetIds.size() != 1)
			{
				return false;
			}

			const LineEvaluator::PerfectLineMatch& perfectMatch = dynamic_cast<const LineEvaluator::PerfectLineMatch&>(*match);

			if (perfectMatch.targetId() != *targetIds.begin())
			{
				return false;
			}

			return true;
		}

		case LineEvaluator::LineMatch::MT_PARTIAL:
		{
			ocean_assert(targetIds.empty() == false);

			const LineEvaluator::PartialLineMatch& partialMatch = dynamic_cast<const LineEvaluator::PartialLineMatch&>(*match);

			if (partialMatch.targetIds() != targetIds)
			{
				return false;
			}

			return true;
		}

		case LineEvaluator::LineMatch::MT_COMPLEX:
		{
			ocean_assert(targetIds.empty() == false);

			const LineEvaluator::ComplexLineMatch& complexMatch = dynamic_cast<const LineEvaluator::ComplexLineMatch&>(*match);

			if (complexMatch.targetIds() != targetIds)
			{
				return false;
			}

			return true;
		}

		default:
			ocean_assert(false && "Unknown match type!");
			break;
	}

	return false;
}

}

}

}

}
