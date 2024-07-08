/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_LINE_EVALUATOR_H
#define META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_LINE_EVALUATOR_H

#include "ocean/test/testcv/testdetector/TestCVDetector.h"

#include "ocean/cv/detector/LineEvaluator.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

/**
 * This class implements tests for the LineEvaluator.
 * @ingroup testcvdetector
 */
class OCEAN_TEST_CV_DETECTOR_EXPORT TestLineEvaluator
{
	public:

		/**
		 * Invokes all test for the LineEvaluator.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Tests the match determination.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type of a scalar value, e.g,. 'float' or 'double'
		 */
		template <typename T>
		static bool testMatchDetermination(const double testDuration);

	protected:

		/**
		 * Verifies a given match.
		 * @param match The match to verify, must be valid
		 * @param matchType The expected type of the match
		 * @param targetIds The expected target ids of the match, at least one
		 * @return True, if the match is correct
		 */
		static bool verifyMatch(const CV::Detector::LineEvaluator::LineMatchRef& match, const CV::Detector::LineEvaluator::LineMatch::MatchType matchType, const CV::Detector::LineEvaluator::IdSet& targetIds);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_LINE_EVALUATOR_H
