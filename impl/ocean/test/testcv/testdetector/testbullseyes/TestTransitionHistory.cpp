/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testdetector/testbullseyes/TestTransitionHistory.h"

#include "ocean/base/RandomI.h"

#include "ocean/cv/detector/bullseyes/TransitionHistory.h"

#include "ocean/math/Random.h"

#include <vector>

using Ocean::CV::Detector::Bullseyes::TransitionHistory;

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

namespace TestBullseyes
{

using namespace CV::Detector::Bullseyes;

bool TestTransitionHistory::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Test for TransitionHistory:   ---";
	Log::info() << " ";

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	allSucceeded = testHistoryAndPush(testDuration, randomGenerator) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testReset(testDuration, randomGenerator) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testIsTransitionToBlack(testDuration, randomGenerator) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testIsTransitionToWhite(testDuration, randomGenerator) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "TransitionHistory test succeeded.";
	}
	else
	{
		Log::info() << "TransitionHistory test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

} // namespace TestBullseyes

TEST(TestTransitionHistory, HistoryAndPush)
{
	RandomGenerator randomGenerator;
	EXPECT_TRUE(TestDetector::TestBullseyes::TestTransitionHistory::testHistoryAndPush(GTEST_TEST_DURATION, randomGenerator));
}

TEST(TestTransitionHistory, Reset)
{
	RandomGenerator randomGenerator;
	EXPECT_TRUE(TestDetector::TestBullseyes::TestTransitionHistory::testReset(GTEST_TEST_DURATION, randomGenerator));
}

TEST(TestTransitionHistory, IsTransitionToBlack)
{
	RandomGenerator randomGenerator;
	EXPECT_TRUE(TestDetector::TestBullseyes::TestTransitionHistory::testIsTransitionToBlack(GTEST_TEST_DURATION, randomGenerator));
}

TEST(TestTransitionHistory, IsTransitionToWhite)
{
	RandomGenerator randomGenerator;
	EXPECT_TRUE(TestDetector::TestBullseyes::TestTransitionHistory::testIsTransitionToWhite(GTEST_TEST_DURATION, randomGenerator));
}

namespace TestBullseyes
{

#endif // OCEAN_USE_GTEST

bool TestTransitionHistory::testHistoryAndPush(const double testDuration, RandomGenerator& randomGenerator)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "TransitionHistory::history1(), history2(), history3(), and push() test:";

	bool allSucceeded = true;

	Timestamp start(true);

	do
	{
		TransitionHistory history;

		const int delta1 = RandomI::random(randomGenerator, -100, 100);
		const int delta2 = RandomI::random(randomGenerator, -100, 100);
		const int delta3 = RandomI::random(randomGenerator, -100, 100);
		const int delta4 = RandomI::random(randomGenerator, -100, 100);

		// Test history1
		history.push(delta1);

		if (history.history1() != delta1)
		{
			allSucceeded = false;
		}

		// Test history2
		history.push(delta2);

		if (history.history1() != delta2)
		{
			allSucceeded = false;
		}

		if (history.history2() != delta1 + delta2)
		{
			allSucceeded = false;
		}

		// Test history3
		history.push(delta3);

		if (history.history1() != delta3)
		{
			allSucceeded = false;
		}

		if (history.history2() != delta2 + delta3)
		{
			allSucceeded = false;
		}

		if (history.history3() != delta1 + delta2 + delta3)
		{
			allSucceeded = false;
		}

		// Test push with 4 values - verify sliding window behavior
		history.push(delta4);

		if (history.history1() != delta4)
		{
			allSucceeded = false;
		}

		if (history.history2() != delta3 + delta4)
		{
			allSucceeded = false;
		}

		if (history.history3() != delta2 + delta3 + delta4)
		{
			allSucceeded = false;
		}
	}
	while (Timestamp(true) < start + testDuration);

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

bool TestTransitionHistory::testReset(const double testDuration, RandomGenerator& randomGenerator)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "TransitionHistory::reset() test:";

	bool allSucceeded = true;

	Timestamp start(true);

	do
	{
		TransitionHistory history;

		const int delta1 = RandomI::random(randomGenerator, -100, 100);
		const int delta2 = RandomI::random(randomGenerator, -100, 100);
		const int delta3 = RandomI::random(randomGenerator, -100, 100);

		history.push(delta1);
		history.push(delta2);
		history.push(delta3);

		history.reset();

		// After reset, all history values should be 0
		if (history.history1() != 0)
		{
			allSucceeded = false;
		}

		if (history.history2() != 0)
		{
			allSucceeded = false;
		}

		if (history.history3() != 0)
		{
			allSucceeded = false;
		}
	}
	while (Timestamp(true) < start + testDuration);

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

bool TestTransitionHistory::testIsTransitionToBlack(const double testDuration, RandomGenerator& randomGenerator)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "TransitionHistory::isTransitionToBlack() test:";

	bool allSucceeded = true;

	// Test with hard-coded clear transition from white to black
	{
		TransitionHistory history;
		uint8_t pixels[2] = {255, 0};

		const bool isTransition = TransitionHistory::isTransitionToBlack(&pixels[1], history);

		if (!isTransition)
		{
			allSucceeded = false;
		}
	}

	// Test with hard-coded steep gradient (monotonic decreasing)
	{
		constexpr unsigned int pixelCount = 32u;
		uint8_t pixels[pixelCount];

		// Create steep gradient from 255 to 0
		for (unsigned int n = 0u; n < pixelCount; ++n)
		{
			pixels[n] = uint8_t(255u - (n * 255u) / (pixelCount - 1u));
		}

		TransitionHistory history;
		std::vector<unsigned int> transitionIndices;

		for (unsigned int n = 1u; n < pixelCount; ++n)
		{
			const bool isTransition = TransitionHistory::isTransitionToBlack(&pixels[n], history);

			if (isTransition)
			{
				transitionIndices.push_back(n);
			}
		}

		if (transitionIndices.empty())
		{
			allSucceeded = false;
		}
	}

	// Test with gentle gradient where history matters
	{
		constexpr unsigned int pixelCount = 64u;
		uint8_t pixels[pixelCount];

		// Create gentle gradient with delta < 20 per pixel but large enough for history accumulation
		// With delta=-10: currentDelta (-10) + history3 (-30) = -40, which exceeds -30 threshold
		const int startValue = 200;
		const int deltaPerPixel = -10; // Small delta where history will accumulate

		for (unsigned int n = 0u; n < pixelCount; ++n)
		{
			pixels[n] = uint8_t(std::max(0, std::min(255, startValue + int(n) * deltaPerPixel)));
		}

		TransitionHistory history;
		std::vector<unsigned int> transitionIndices;

		for (unsigned int n = 1u; n < pixelCount; ++n)
		{
			const bool isTransition = TransitionHistory::isTransitionToBlack(&pixels[n], history);

			if (isTransition)
			{
				transitionIndices.push_back(n);
			}
		}

		if (transitionIndices.empty())
		{
			allSucceeded = false;
		}
	}

	// Now run randomized stress tests for the remaining test duration
	Timestamp start(true);

	do
	{
		// Test with random monotonic decreasing values
		{
			constexpr unsigned int pixelCount = 32u;
			uint8_t pixels[pixelCount];

			const unsigned int startValue = RandomI::random(randomGenerator, 150u, 255u);
			const unsigned int endValue = RandomI::random(randomGenerator, 0u, 50u);
			const int totalDelta = int(endValue) - int(startValue);

			for (unsigned int n = 0u; n < pixelCount; ++n)
			{
				pixels[n] = uint8_t(startValue + (totalDelta * int(n)) / int(pixelCount - 1u));
			}

			TransitionHistory history;
			unsigned int transitionCount = 0u;

			for (unsigned int n = 1u; n < pixelCount; ++n)
			{
				const bool isTransition = TransitionHistory::isTransitionToBlack(&pixels[n], history);

				if (isTransition)
				{
					++transitionCount;
				}
			}

			// For a steep enough monotonic decreasing gradient, we expect at least one transition
			// With 32 pixels and to trigger via history3: need currentDelta + 3*currentDelta < -30
			// So currentDelta < -7.5, meaning totalDelta < -7.5 * 31 ≈ -233
			if (totalDelta < -240 && transitionCount == 0u)
			{
				allSucceeded = false;
				Log::info() << "Random test failed: startValue=" << startValue << ", endValue=" << endValue << ", totalDelta=" << totalDelta << ", transitions=" << transitionCount;
				break;
			}
		}

		// Test with no transition (constant values)
		{
			constexpr unsigned int pixelCount = 16u;
			uint8_t pixels[pixelCount];

			const uint8_t constantValue = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
			for (unsigned int n = 0u; n < pixelCount; ++n)
			{
				pixels[n] = constantValue;
			}

			TransitionHistory history;

			for (unsigned int n = 1u; n < pixelCount; ++n)
			{
				const bool isTransition = TransitionHistory::isTransitionToBlack(&pixels[n], history);

				if (isTransition)
				{
					allSucceeded = false;
					Log::info() << "Constant value test failed: detected false positive transition at index " << n << " with value " << int(constantValue);
					break;
				}
			}
		}

		if (!allSucceeded)
		{
			break;
		}
	}
	while (Timestamp(true) < start + testDuration);

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

bool TestTransitionHistory::testIsTransitionToWhite(const double testDuration, RandomGenerator& randomGenerator)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "TransitionHistory::isTransitionToWhite() test:";

	bool allSucceeded = true;

	// Test with hard-coded clear transition from black to white
	{
		TransitionHistory history;
		uint8_t pixels[2] = {0, 255};

		const bool isTransition = TransitionHistory::isTransitionToWhite(&pixels[1], history);

		if (!isTransition)
		{
			allSucceeded = false;
		}
	}

	// Test with hard-coded steep gradient (monotonic increasing)
	{
		constexpr unsigned int pixelCount = 32u;
		uint8_t pixels[pixelCount];

		// Create steep gradient from 0 to 255
		for (unsigned int n = 0u; n < pixelCount; ++n)
		{
			pixels[n] = uint8_t((n * 255u) / (pixelCount - 1u));
		}

		TransitionHistory history;
		std::vector<unsigned int> transitionIndices;

		for (unsigned int n = 1u; n < pixelCount; ++n)
		{
			const bool isTransition = TransitionHistory::isTransitionToWhite(&pixels[n], history);

			if (isTransition)
			{
				transitionIndices.push_back(n);
			}
		}

		if (transitionIndices.empty())
		{
			allSucceeded = false;
		}
	}

	// Test with gentle gradient where history matters
	{
		constexpr unsigned int pixelCount = 64u;
		uint8_t pixels[pixelCount];

		// Create gentle gradient with delta < 20 per pixel but large enough for history accumulation
		// With delta=+10: currentDelta (+10) + history3 (+30) = +40, which exceeds +30 threshold
		const int startValue = 50;
		const int deltaPerPixel = 10; // Small delta where history will accumulate

		for (unsigned int n = 0u; n < pixelCount; ++n)
		{
			pixels[n] = uint8_t(std::max(0, std::min(255, startValue + int(n) * deltaPerPixel)));
		}

		TransitionHistory history;
		std::vector<unsigned int> transitionIndices;

		for (unsigned int n = 1u; n < pixelCount; ++n)
		{
			const bool isTransition = TransitionHistory::isTransitionToWhite(&pixels[n], history);

			if (isTransition)
			{
				transitionIndices.push_back(n);
			}
		}

		if (transitionIndices.empty())
		{
			allSucceeded = false;
		}
	}

	// Now run randomized stress tests for the remaining test duration
	Timestamp start(true);

	do
	{
		// Test with random monotonic increasing values
		{
			constexpr unsigned int pixelCount = 32u;
			uint8_t pixels[pixelCount];

			const unsigned int startValue = RandomI::random(randomGenerator, 0u, 50u);
			const unsigned int endValue = RandomI::random(randomGenerator, 150u, 255u);
			const int totalDelta = int(endValue) - int(startValue);

			for (unsigned int n = 0u; n < pixelCount; ++n)
			{
				pixels[n] = uint8_t(startValue + (totalDelta * int(n)) / int(pixelCount - 1u));
			}

			TransitionHistory history;
			unsigned int transitionCount = 0u;

			for (unsigned int n = 1u; n < pixelCount; ++n)
			{
				const bool isTransition = TransitionHistory::isTransitionToWhite(&pixels[n], history);

				if (isTransition)
				{
					++transitionCount;
				}
			}

			// For a steep enough monotonic increasing gradient, we expect at least one transition
			// With 32 pixels and to trigger via history3: need currentDelta + 3*currentDelta > 30
			// So currentDelta > 7.5, meaning totalDelta > 7.5 * 31 ≈ 233
			if (totalDelta > 240 && transitionCount == 0u)
			{
				allSucceeded = false;
				Log::info() << "Random test failed: startValue=" << startValue << ", endValue=" << endValue << ", totalDelta=" << totalDelta << ", transitions=" << transitionCount;
				break;
			}
		}

		// Test with no transition (constant values)
		{
			constexpr unsigned int pixelCount = 16u;
			uint8_t pixels[pixelCount];

			const uint8_t constantValue = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
			for (unsigned int n = 0u; n < pixelCount; ++n)
			{
				pixels[n] = constantValue;
			}

			TransitionHistory history;

			for (unsigned int n = 1u; n < pixelCount; ++n)
			{
				const bool isTransition = TransitionHistory::isTransitionToWhite(&pixels[n], history);

				if (isTransition)
				{
					allSucceeded = false;
					Log::info() << "Constant value test failed: detected false positive transition at index " << n << " with value " << int(constantValue);
					break;
				}
			}
		}

		if (!allSucceeded)
		{
			break;
		}
	}
	while (Timestamp(true) < start + testDuration);

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

} // namespace TestBullseyes

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namespace Ocean
