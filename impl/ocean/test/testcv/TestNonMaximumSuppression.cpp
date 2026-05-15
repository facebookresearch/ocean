/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestNonMaximumSuppression.h"

#include "ocean/base/DataType.h"
#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/cv/Canvas.h"

#include "ocean/math/Random.h"

#include "ocean/test/Validation.h"
#include "ocean/test/TestResult.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestNonMaximumSuppression::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker, const TestSelector& selector)
{
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(testDuration > 0.0);

	TestResult testResult("Non maximum suppression test");

	Log::info() << " ";

	if (selector.shouldRun("suppressioninframe"))
	{
		testResult = testSuppressionInFrame(width, height, width, height, true, testDuration, worker);
		Log::info() << " ";
		testResult = testSuppressionInFrame(width, height, width, height, false, testDuration, worker);
		Log::info() << " ";
		testResult = testSuppressionInFrame(width, height, width * 75u / 100u, height * 75u / 100u, true, testDuration, worker);
		Log::info() << " ";
		testResult = testSuppressionInFrame(width, height, width * 75u / 100u, height * 75u / 100u, false, testDuration, worker);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("suppressioninframeminimum"))
	{
		testResult = testSuppressionInFrameMinimum(width, height, true, testDuration, worker);
		Log::info() << " ";
		testResult = testSuppressionInFrameMinimum(width, height, false, testDuration, worker);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("suppressioninstrengthpositions"))
	{
		testResult = testSuppressionInStrengthPositions(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("determineprecisepeaklocation1"))
	{
		testResult = testDeterminePrecisePeakLocation1<float>();
		Log::info() << " ";
		testResult = testDeterminePrecisePeakLocation1<double>();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("determineprecisepeaklocation2"))
	{
		testResult = testDeterminePrecisePeakLocation2<float>();
		Log::info() << " ";
		testResult = testDeterminePrecisePeakLocation2<double>();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("determineprecisepeaklocationnxn"))
	{
		testResult = testDeterminePrecisePeakLocationNxN<float>();
		Log::info() << " ";
		testResult = testDeterminePrecisePeakLocationNxN<double>();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("determineprecisepeaklocationiterativenxn"))
	{
		testResult = testDeterminePrecisePeakLocationIterativeNxN<float>();
		Log::info() << " ";
		testResult = testDeterminePrecisePeakLocationIterativeNxN<double>();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("candidate"))
	{
		testResult = testCandidate(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << " ";

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestNonMaximumSuppression, SuppressionInFrame_1920x1080_Strict)
{
	Worker worker;
	EXPECT_TRUE(TestNonMaximumSuppression::testSuppressionInFrame(1920u, 1080u, 1920u, 1080u, true, GTEST_TEST_DURATION, worker));
}

TEST(TestNonMaximumSuppression, SuppressionInFrame_1920x1080_NonStrict)
{
	Worker worker;
	EXPECT_TRUE(TestNonMaximumSuppression::testSuppressionInFrame(1920u, 1080u, 1920u, 1080u, false, GTEST_TEST_DURATION, worker));
}

TEST(TestNonMaximumSuppression, SuppressionInSubFrame_1920x1080_Strict)
{
	Worker worker;
	EXPECT_TRUE(TestNonMaximumSuppression::testSuppressionInFrame(1920u, 1080u, 1920u * 75u / 100u, 1080u * 75u / 100u, true, GTEST_TEST_DURATION, worker));
}

TEST(TestNonMaximumSuppression, SuppressionInSubFrame_1920x1080_NonStrict)
{
	Worker worker;
	EXPECT_TRUE(TestNonMaximumSuppression::testSuppressionInFrame(1920u, 1080u, 1920u * 75u / 100u, 1080u * 75u / 100u, false, GTEST_TEST_DURATION, worker));
}


TEST(TestNonMaximumSuppression, SuppressionInFrameMinimum_1920x1080_Strict)
{
	Worker worker;
	EXPECT_TRUE(TestNonMaximumSuppression::testSuppressionInFrameMinimum(1920u, 1080u, true, GTEST_TEST_DURATION, worker));
}

TEST(TestNonMaximumSuppression, SuppressionInFrameMinimum_1920x1080_NonStrict)
{
	Worker worker;
	EXPECT_TRUE(TestNonMaximumSuppression::testSuppressionInFrameMinimum(1920u, 1080u, false, GTEST_TEST_DURATION, worker));
}


TEST(TestNonMaximumSuppression, testSuppressionInStrengthPositions_uint32_float)
{
	EXPECT_TRUE((TestNonMaximumSuppression::testSuppressionInStrengthPositions<uint32_t, float>(GTEST_TEST_DURATION)));
}

TEST(TestNonMaximumSuppression, testSuppressionInStrengthPositions_double_double)
{
	EXPECT_TRUE((TestNonMaximumSuppression::testSuppressionInStrengthPositions<double, double>(GTEST_TEST_DURATION)));
}


TEST(TestNonMaximumSuppression, DeterminePrecisePeakLocation1_float)
{
	EXPECT_TRUE(TestNonMaximumSuppression::testDeterminePrecisePeakLocation1<float>());
}

TEST(TestNonMaximumSuppression, DeterminePrecisePeakLocation1_double)
{
	EXPECT_TRUE(TestNonMaximumSuppression::testDeterminePrecisePeakLocation1<double>());
}


TEST(TestNonMaximumSuppression, DeterminePrecisePeakLocation2_float)
{
	EXPECT_TRUE(TestNonMaximumSuppression::testDeterminePrecisePeakLocation2<float>());
}

TEST(TestNonMaximumSuppression, DeterminePrecisePeakLocation2_double)
{
	EXPECT_TRUE(TestNonMaximumSuppression::testDeterminePrecisePeakLocation2<double>());
}

TEST(TestNonMaximumSuppression, DeterminePrecisePeakLocationNxN_float)
{
	EXPECT_TRUE(TestNonMaximumSuppression::testDeterminePrecisePeakLocationNxN<float>());
}

TEST(TestNonMaximumSuppression, DeterminePrecisePeakLocationNxN_double)
{
	EXPECT_TRUE(TestNonMaximumSuppression::testDeterminePrecisePeakLocationNxN<double>());
}

TEST(TestNonMaximumSuppression, DeterminePrecisePeakLocationIterativeNxN_float)
{
	EXPECT_TRUE(TestNonMaximumSuppression::testDeterminePrecisePeakLocationIterativeNxN<float>());
}

TEST(TestNonMaximumSuppression, DeterminePrecisePeakLocationIterativeNxN_double)
{
	EXPECT_TRUE(TestNonMaximumSuppression::testDeterminePrecisePeakLocationIterativeNxN<double>());
}

TEST(TestNonMaximumSuppression, Candidate)
{
	EXPECT_TRUE(TestNonMaximumSuppression::testCandidate(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestNonMaximumSuppression::testSuppressionInFrame(const unsigned int width, const unsigned int height, const unsigned int subFrameWidth, const unsigned int subFrameHeight, const bool strictMaximum, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 3u && height >= 3u);
	ocean_assert(subFrameWidth >= 2u && subFrameHeight >= 2u);
	ocean_assert(subFrameWidth <= width && subFrameHeight <= height);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test non maximum suppression (" << (strictMaximum ? "strict" : "non-strict") << ") in " << subFrameWidth << "x" << subFrameHeight << " area within " << width << "x" << height << " frame:";
	Log::info() << " ";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const unsigned int features = 1000u;

	HighPerformanceStatistic performanceSinglecoreCreation;
	HighPerformanceStatistic performanceSinglecoreDetermination;
	HighPerformanceStatistic performanceSinglecoreDeterminationNaive;

	HighPerformanceStatistic performanceMulticoreCreation;
	HighPerformanceStatistic performanceMulticoreDetermination;
	HighPerformanceStatistic performanceMulticoreDeterminationNaive;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++ workerIteration)
	{
		// we use the worker (multi-core execution in the second iteration)
		HighPerformanceStatistic& performanceCreation = workerIteration == 0u ? performanceSinglecoreCreation : performanceMulticoreCreation;
		HighPerformanceStatistic& performanceDetermination = workerIteration == 0u ? performanceSinglecoreDetermination : performanceMulticoreDetermination;
		HighPerformanceStatistic& performanceDeterminationNaive = workerIteration == 0u ? performanceSinglecoreDeterminationNaive : performanceMulticoreDeterminationNaive;

		Worker* useWorker = workerIteration == 0u ? nullptr : &worker;

		const Timestamp startTimestamp(true);

		do
		{
			const uint8_t minimalThreshold = uint8_t(RandomI::random(randomGenerator, 40u, 150u));

			unsigned int paddingElements = 0u;
			if (RandomI::boolean(randomGenerator))
			{
				paddingElements = RandomI::random(randomGenerator, 1u, 100u);
			}

			Frame yFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), paddingElements);
			createFeaturePoints(yFrame, features);

			const unsigned int subFrameLeft = RandomI::random(randomGenerator, 0u, width - subFrameWidth);
			const unsigned int subFrameTop = RandomI::random(randomGenerator, 0u, height - subFrameHeight);

			const unsigned int subFrameExtraBorderLeft = (unsigned int)(std::max(0, int(subFrameLeft) - 1)); // extra border is necessary to get same results as in test function
			const unsigned int subFrameExtraBorderTop = (unsigned int)(std::max(0, int(subFrameTop) - 1));
			const unsigned int subFrameExtraBorderRight = std::min(subFrameLeft + subFrameWidth + 1u, width);
			const unsigned int subFrameExtraBorderBottom = std::min(subFrameTop + subFrameHeight + 1u, height);

			performanceCreation.start();
				CV::NonMaximumSuppressionT<uint8_t> nonMaximumSuppression(width, height, 0u);
				nonMaximumSuppression.addCandidates(yFrame.constdata<uint8_t>(), yFrame.paddingElements(), subFrameExtraBorderLeft, subFrameExtraBorderRight - subFrameExtraBorderLeft, subFrameExtraBorderTop, subFrameExtraBorderBottom - subFrameExtraBorderTop, minimalThreshold, useWorker);
			performanceCreation.stop();

			StrengthPositions locations;

			performanceDetermination.start();
				if (strictMaximum)
				{
					if (!nonMaximumSuppression.suppressNonMaximum<int, uint8_t, true>(subFrameLeft, subFrameWidth, subFrameTop, subFrameHeight, locations, useWorker, nullptr))
					{
						OCEAN_SET_FAILED(validation);
					}
				}
				else
				{
					if (!nonMaximumSuppression.suppressNonMaximum<int, uint8_t, false>(subFrameLeft, subFrameWidth, subFrameTop, subFrameHeight, locations, useWorker, nullptr))
					{
						OCEAN_SET_FAILED(validation);
					}
				}
			performanceDetermination.stop();

			performanceDeterminationNaive.start();
				const StrengthPositions naiveLocations(determineFeaturePoints(yFrame, subFrameLeft, subFrameTop, subFrameWidth, subFrameHeight, minimalThreshold, strictMaximum, useWorker));
			performanceDeterminationNaive.stop();

			const StrengthPositions testLocations(determineFeaturePoints(yFrame, subFrameLeft, subFrameTop, subFrameWidth, subFrameHeight, minimalThreshold, strictMaximum, nullptr /* no worker */));

			const StrengthPositionSet locationSet(locations.begin(), locations.end());
			ocean_assert(locationSet.size() == locations.size());

			const StrengthPositionSet testLocationSet(testLocations.begin(), testLocations.end());
			ocean_assert(testLocationSet.size() == testLocations.size());

			OCEAN_EXPECT_EQUAL(validation, locationSet, testLocationSet);

			OCEAN_EXPECT_EQUAL(validation, testLocationSet.size(), naiveLocations.size());
		}
		while (!startTimestamp.hasTimePassed(testDuration));
	}

	Log::info() << "Singlecore creation: " << performanceSinglecoreCreation.averageMseconds() << "ms";
	Log::info() << "Singlecore determination: " << performanceSinglecoreDetermination.averageMseconds() << "ms, boost: " << String::toAString(performanceSinglecoreDeterminationNaive.average() / performanceSinglecoreDetermination.average(), 2u) << "x";
	Log::info() << "Singlecore determination naive: " << performanceSinglecoreDeterminationNaive.averageMseconds() << "ms";

	if (performanceMulticoreCreation.measurements() != 0u)
	{
		Log::info() << " ";
		Log::info() << "Multicore creation: " << performanceMulticoreCreation.averageMseconds() << "ms, boost: " << String::toAString(performanceSinglecoreCreation.average() / performanceMulticoreCreation.average(), 2u) << "x";
		Log::info() << "Multicore determination: " << performanceMulticoreDetermination.averageMseconds() << "ms, boost: " << String::toAString(performanceSinglecoreDetermination.average() / performanceMulticoreDetermination.average(), 2u) << "x";
		Log::info() << "Multicore determination naive: " << performanceMulticoreDeterminationNaive.averageMseconds() << "ms";
	}

	Log::info() << " ";

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestNonMaximumSuppression::testSuppressionInFrameMinimum(const unsigned int width, const unsigned int height, const bool strictMaximum, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 3u && height >= 3u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test non minimum suppression (" << (strictMaximum ? "strict" : "non-strict") << ") in " << width << "x" << height << " frame:";

	using SignedNonMaximumSuppression = CV::NonMaximumSuppressionT<int32_t>;
	using SignedStrengthPosition = SignedNonMaximumSuppression::StrengthPosition<int, int32_t>;
	using SignedStrengthPositions = std::vector<SignedStrengthPosition>;

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const unsigned int features = 1000u;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = workerIteration == 0u ? nullptr : &worker;

		const Timestamp startTimestamp(true);

		do
		{
			const int32_t negativeThreshold = -int32_t(RandomI::random(randomGenerator, 40u, 150u));

			Frame yFrame(FrameType(width, height, FrameType::genericPixelFormat<int32_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT));

			{
				Frame yFrame8(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
				createFeaturePoints(yFrame8, features);

				for (unsigned int y = 0u; y < height; ++y)
				{
					const uint8_t* row8 = yFrame8.constrow<uint8_t>(y);
					int32_t* row32 = yFrame.row<int32_t>(y);

					for (unsigned int x = 0u; x < width; ++x)
					{
						row32[x] = -int32_t(row8[x]);
					}
				}
			}

			SignedNonMaximumSuppression nonMaximumSuppression(width, height, 0u);

			for (unsigned int y = 0u; y < height; ++y)
			{
				const int32_t* row = yFrame.constrow<int32_t>(y);

				for (unsigned int x = 0u; x < width; ++x)
				{
					if (row[x] <= negativeThreshold)
					{
						nonMaximumSuppression.addCandidate(x, y, row[x]);
					}
				}
			}

			SignedStrengthPositions locations;

			if (strictMaximum)
			{
				if (!nonMaximumSuppression.suppressNonMaximum<int, int32_t, true, CV::NonMaximumSuppression::SM_MINIMUM>(0u, width, 0u, height, locations, useWorker, nullptr))
				{
					OCEAN_SET_FAILED(validation);
				}
			}
			else
			{
				if (!nonMaximumSuppression.suppressNonMaximum<int, int32_t, false, CV::NonMaximumSuppression::SM_MINIMUM>(0u, width, 0u, height, locations, useWorker, nullptr))
				{
					OCEAN_SET_FAILED(validation);
				}
			}

			// naive brute-force reference: find local minima in the negated frame

			SignedStrengthPositions naiveLocations;

			for (unsigned int y = 1u; y < height - 1u; ++y)
			{
				for (unsigned int x = 1u; x < width - 1u; ++x)
				{
					const int32_t centerValue = yFrame.constpixel<int32_t>(x, y)[0];

					if (centerValue > negativeThreshold)
					{
						continue;
					}

					const int32_t northWest  = yFrame.constpixel<int32_t>(x - 1u, y - 1u)[0];
					const int32_t north      = yFrame.constpixel<int32_t>(x + 0u, y - 1u)[0];
					const int32_t northEast  = yFrame.constpixel<int32_t>(x + 1u, y - 1u)[0];
					const int32_t west       = yFrame.constpixel<int32_t>(x - 1u, y + 0u)[0];
					const int32_t east       = yFrame.constpixel<int32_t>(x + 1u, y + 0u)[0];
					const int32_t southWest  = yFrame.constpixel<int32_t>(x - 1u, y + 1u)[0];
					const int32_t south      = yFrame.constpixel<int32_t>(x + 0u, y + 1u)[0];
					const int32_t southEast  = yFrame.constpixel<int32_t>(x + 1u, y + 1u)[0];

					if (strictMaximum)
					{
						// strict minimum: center must be strictly less than all 8 neighbors
						if (centerValue < northWest && centerValue < north && centerValue < northEast
							&& centerValue < west && centerValue < east
							&& centerValue < southWest && centerValue < south && centerValue < southEast)
						{
							naiveLocations.emplace_back(int(x), int(y), centerValue);
						}
					}
					else
					{
						// non-strict minimum: center is less-or-equal in upper-left neighborhood, strictly less in lower-right
						if (centerValue <= northWest && centerValue <= north && centerValue <= northEast
							&& centerValue <= west && centerValue < east
							&& centerValue <= southWest && centerValue < south && centerValue < southEast)
						{
							naiveLocations.emplace_back(int(x), int(y), centerValue);
						}
					}
				}
			}

			std::set<SignedStrengthPosition> locationSet(locations.begin(), locations.end());
			std::set<SignedStrengthPosition> naiveLocationSet(naiveLocations.begin(), naiveLocations.end());

			OCEAN_EXPECT_EQUAL(validation, locationSet, naiveLocationSet);
		}
		while (!startTimestamp.hasTimePassed(testDuration));
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestNonMaximumSuppression::testSuppressionInStrengthPositions(const double testDuration)
{
	Log::info() << "Test non maximum suppression of strength positions:";
	Log::info() << " ";

	TestResult testResult;

	testResult = testSuppressionInStrengthPositions<uint32_t, float>(testDuration);

	Log::info() << " ";

	testResult = testSuppressionInStrengthPositions<double, double>(testDuration);

	Log::info() << " ";
	Log::info() << "Validation: " << testResult;

	return testResult.succeeded();
}

template <typename TCoordinate, typename TStrength>
bool TestNonMaximumSuppression::testSuppressionInStrengthPositions(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... with coordinate data type '" << TypeNamer::name<TCoordinate>() << "' and strength parameter '" << TypeNamer::name<TStrength>() << "':";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 10u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 10u, 1920u);

		const unsigned int size = RandomI::random(randomGenerator, 1u, 10000u);

		CV::NonMaximumSuppressionT<double>::StrengthPositions<TCoordinate, TStrength> strengthPositions;
		strengthPositions.reserve(size);

		while (strengthPositions.size() < size_t(size))
		{
			const Scalar xPosition = Random::scalar(randomGenerator, Scalar(0), Scalar(width) - Numeric::weakEps());
			const Scalar yPosition = Random::scalar(randomGenerator, Scalar(0), Scalar(height) - Numeric::weakEps());

			const Scalar strength = Random::scalar(randomGenerator, Scalar(0), Scalar(1000));

			strengthPositions.emplace_back(TCoordinate(xPosition), TCoordinate(yPosition), TStrength(strength));
		}

		const TCoordinate radius = TCoordinate(Random::scalar(randomGenerator, Scalar(1), Scalar(std::max(width, height) / 2u)));
		ocean_assert(radius >= TCoordinate(1));

		const TCoordinate sqrRadius = radius * radius;

		const bool strictMaximum = RandomI::boolean(randomGenerator);

		Indices32 validIndices;
		CV::NonMaximumSuppressionT<double>::StrengthPositions<TCoordinate, TStrength> remainingStrengthPositions;

		if (strictMaximum)
		{
			remainingStrengthPositions = CV::NonMaximumSuppressionT<double>::suppressNonMaximum<TCoordinate, TStrength, true>(width, height, strengthPositions, radius, &validIndices);
		}
		else
		{
			remainingStrengthPositions = CV::NonMaximumSuppressionT<double>::suppressNonMaximum<TCoordinate, TStrength, false>(width, height, strengthPositions, radius, &validIndices);
		}

		const UnorderedIndexSet32 debugValidIndexSet(validIndices.cbegin(), validIndices.cend());
		ocean_assert_and_suppress_unused(debugValidIndexSet.size() == validIndices.size(), debugValidIndexSet);

		Indices32 statementsStrengthPositions(strengthPositions.size(), 1u);

		for (size_t nOuter = 0; nOuter < strengthPositions.size(); ++nOuter)
		{
			const CV::NonMaximumSuppressionT<double>::StrengthPosition<TCoordinate, TStrength>& outer = strengthPositions[nOuter];

			for (size_t nInner = 0; nInner < strengthPositions.size(); ++nInner)
			{
				if (nInner == nOuter)
				{
					continue;
				}

				const CV::NonMaximumSuppressionT<double>::StrengthPosition<TCoordinate, TStrength>& inner = strengthPositions[nInner];

				if (outer.sqrDistance(inner) <= sqrRadius)
				{
					if (outer.strength() < inner.strength())
					{
						statementsStrengthPositions[nOuter] = 0u;

						ocean_assert(debugValidIndexSet.find(Index32(nOuter)) == debugValidIndexSet.cend());
					}
					else if (outer.strength() == inner.strength())
					{
						if (strictMaximum)
						{
							statementsStrengthPositions[nOuter] = 0u;

							ocean_assert(debugValidIndexSet.find(Index32(nOuter)) == debugValidIndexSet.cend());
						}
						else
						{
							// if both positions have the same strength, we remove the upper left position

							if (outer.y() < inner.y() || (outer.y() == inner.y() && outer.x() < inner.x()))
							{
								statementsStrengthPositions[nOuter] = 0u;
							}
						}
					}
				}
			}
		}

		CV::NonMaximumSuppressionT<double>::StrengthPositions<TCoordinate, TStrength> testStrengthPositions;
		testStrengthPositions.reserve(remainingStrengthPositions.size());

		for (size_t n = 0; n < statementsStrengthPositions.size(); ++n)
		{
			if (statementsStrengthPositions[n])
			{
				testStrengthPositions.emplace_back(strengthPositions[n]);
			}
		}

		OCEAN_EXPECT_EQUAL(validation, remainingStrengthPositions, testStrengthPositions);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestNonMaximumSuppression::testDeterminePrecisePeakLocation1()
{
	Log::info() << "Test determinePrecisePeakLocation1, with " << TypeNamer::name<T>() << ":";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	{
		T precisePeak = NumericT<T>::minValue();
		const bool result = CV::NonMaximumSuppressionT<T>::determinePrecisePeakLocation1(0, 0, 0, precisePeak);
		OCEAN_EXPECT_TRUE(validation, result);
		OCEAN_EXPECT_EQUAL(validation, precisePeak, T(0));
	}

	{
		T precisePeak = NumericT<T>::minValue();
		const bool result = CV::NonMaximumSuppressionT<T>::determinePrecisePeakLocation1(0, 1, 0, precisePeak);
		OCEAN_EXPECT_TRUE(validation, result);
		OCEAN_EXPECT_EQUAL(validation, precisePeak, T(0));
	}

	{
		T precisePeak = NumericT<T>::minValue();
		const bool result = CV::NonMaximumSuppressionT<T>::determinePrecisePeakLocation1(1, 2, 0, precisePeak);
		OCEAN_EXPECT_TRUE(validation, result);
		OCEAN_EXPECT_GREATER_EQUAL(validation, precisePeak, T(-0.5));
		OCEAN_EXPECT_LESS(validation, precisePeak, T(0));
	}

	{
		T precisePeak = NumericT<T>::minValue();
		const bool result = CV::NonMaximumSuppressionT<T>::determinePrecisePeakLocation1(0, 2, 1, precisePeak);
		OCEAN_EXPECT_TRUE(validation, result);
		OCEAN_EXPECT_LESS_EQUAL(validation, precisePeak, T(0.5));
		OCEAN_EXPECT_GREATER(validation, precisePeak, T(0));
	}

	{
		// testing a 1D Gaussian distribution (bell)

		const std::vector<T> offsets =
		{
			T(0),
			T(-0.5),
			T(0.5)
		};

		for (const T offset : offsets)
		{
			constexpr T sigma = T(1);

			std::vector<T> values;
			T sum = T(0);

			for (int x = -1; x <= 1; ++x)
			{
				const T value = T(1) / (sigma * NumericT<T>::sqr(2 * NumericT<T>::pi())) * NumericT<T>::exp(T(-0.5) * NumericT<T>::sqr((x - offset) / sigma));

				values.push_back(value);
				sum += value;
			}

			ocean_assert(NumericT<T>::isNotEqualEps(sum));

			for (T& value : values) // normalizing the values
			{
				value /= sum;
			}

			T precisePeak = NumericT<T>::minValue();
			if (CV::NonMaximumSuppressionT<T>::determinePrecisePeakLocation1(values[0], values[1], values[2], precisePeak))
			{
				OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(precisePeak, offset, T(0.01)));
			}
			else
			{
				OCEAN_SET_FAILED(validation);
			}
		}
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestNonMaximumSuppression::testDeterminePrecisePeakLocation2()
{
	Log::info() << "Test determinePrecisePeakLocation2, with " << TypeNamer::name<T>() << ":";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	{
		constexpr T    topValues[3] = {0, 0, 0};
		constexpr T centerValues[3] = {0, 0, 0};
		constexpr T bottomValues[3] = {0, 0, 0};

		VectorT2<T> precisePeak(NumericT<T>::minValue(), NumericT<T>::minValue());
		const bool result = CV::NonMaximumSuppressionT<T>::determinePrecisePeakLocation2(topValues, centerValues, bottomValues, precisePeak);
		OCEAN_EXPECT_TRUE(validation, result);
		OCEAN_EXPECT_EQUAL(validation, precisePeak, VectorT2<T>(0, 0));
	}

	{
		constexpr T    topValues[3] = {0, 0, 0};
		constexpr T centerValues[3] = {0, 1, 0};
		constexpr T bottomValues[3] = {0, 0, 0};

		VectorT2<T> precisePeak(NumericT<T>::minValue(), NumericT<T>::minValue());
		const bool result = CV::NonMaximumSuppressionT<T>::determinePrecisePeakLocation2(topValues, centerValues, bottomValues, precisePeak);
		OCEAN_EXPECT_TRUE(validation, result);
		OCEAN_EXPECT_EQUAL(validation, precisePeak, VectorT2<T>(0, 0));
	}

	{
		constexpr T    topValues[3] = {0, 0, 0};
		constexpr T centerValues[3] = {1, 1, 1};
		constexpr T bottomValues[3] = {0, 0, 0};

		VectorT2<T> precisePeak(NumericT<T>::minValue(), NumericT<T>::minValue());
		const bool result = CV::NonMaximumSuppressionT<T>::determinePrecisePeakLocation2(topValues, centerValues, bottomValues, precisePeak);
		OCEAN_EXPECT_TRUE(validation, result);
		OCEAN_EXPECT_EQUAL(validation, precisePeak, VectorT2<T>(0, 0));
	}

	{
		constexpr T    topValues[3] = {0, 1, 0};
		constexpr T centerValues[3] = {0, 1, 0};
		constexpr T bottomValues[3] = {0, 1, 0};

		VectorT2<T> precisePeak(NumericT<T>::minValue(), NumericT<T>::minValue());
		const bool result = CV::NonMaximumSuppressionT<T>::determinePrecisePeakLocation2(topValues, centerValues, bottomValues, precisePeak);
		OCEAN_EXPECT_TRUE(validation, result);
		OCEAN_EXPECT_EQUAL(validation, precisePeak, VectorT2<T>(0, 0));
	}

	{
		constexpr T    topValues[3] = {1, 2, 0};
		constexpr T centerValues[3] = {1, 2, 0};
		constexpr T bottomValues[3] = {1, 2, 0};

		VectorT2<T> precisePeak(NumericT<T>::minValue(), NumericT<T>::minValue());
		const bool result = CV::NonMaximumSuppressionT<T>::determinePrecisePeakLocation2(topValues, centerValues, bottomValues, precisePeak);
		OCEAN_EXPECT_TRUE(validation, result);
		OCEAN_EXPECT_EQUAL(validation, precisePeak, VectorT2<T>(0, 0));
	}

	{
		constexpr T    topValues[3] = {1, 1, 1};
		constexpr T centerValues[3] = {2, 2, 2};
		constexpr T bottomValues[3] = {0, 0, 0};

		VectorT2<T> precisePeak(NumericT<T>::minValue(), NumericT<T>::minValue());
		const bool result = CV::NonMaximumSuppressionT<T>::determinePrecisePeakLocation2(topValues, centerValues, bottomValues, precisePeak);
		OCEAN_EXPECT_TRUE(validation, result);
		OCEAN_EXPECT_EQUAL(validation, precisePeak, VectorT2<T>(0, 0));
	}

	{
		constexpr T    topValues[3] = {1, 1, 2};
		constexpr T centerValues[3] = {1, 2, 0};
		constexpr T bottomValues[3] = {2, 0, 0};

		VectorT2<T> precisePeak(NumericT<T>::minValue(), NumericT<T>::minValue());
		const bool result = CV::NonMaximumSuppressionT<T>::determinePrecisePeakLocation2(topValues, centerValues, bottomValues, precisePeak);
		OCEAN_EXPECT_TRUE(validation, result);
		OCEAN_EXPECT_GREATER_EQUAL(validation, precisePeak.x(), T(-0.5));
		OCEAN_EXPECT_LESS(validation, precisePeak.x(), T(0));
		OCEAN_EXPECT_GREATER_EQUAL(validation, precisePeak.y(), T(-0.5));
		OCEAN_EXPECT_LESS(validation, precisePeak.y(), T(0));
	}

	{
		constexpr T    topValues[3] = {0, 0, 2};
		constexpr T centerValues[3] = {0, 2, 1};
		constexpr T bottomValues[3] = {2, 1, 1};

		VectorT2<T> precisePeak(NumericT<T>::minValue(), NumericT<T>::minValue());
		const bool result = CV::NonMaximumSuppressionT<T>::determinePrecisePeakLocation2(topValues, centerValues, bottomValues, precisePeak);
		OCEAN_EXPECT_TRUE(validation, result);
		OCEAN_EXPECT_LESS_EQUAL(validation, precisePeak.x(), T(0.5));
		OCEAN_EXPECT_GREATER(validation, precisePeak.x(), T(0));
		OCEAN_EXPECT_LESS_EQUAL(validation, precisePeak.y(), T(0.5));
		OCEAN_EXPECT_GREATER(validation, precisePeak.y(), T(0));
	}

	{
		// testing a 2D Gaussian distribution (bell)

		const VectorsT2<T> offsets =
		{
			VectorT2<T>(0, 0),
			VectorT2<T>(-1, -1) * T(0.5),
			VectorT2<T>(-1, 1) * T(0.5),
			VectorT2<T>(1, 1) * T(0.5),
			VectorT2<T>(1, -1) * T(0.5)
		};

		for (const VectorT2<T>& offset : offsets)
		{
			constexpr T sigma = T(1);

			std::vector<T> values;
			T sum = 0;

			for (int y = -1; y <= 1; ++y)
			{
				const T yValue = T(1) / (sigma * NumericT<T>::sqr(2 * NumericT<T>::pi())) * NumericT<T>::exp(T(-0.5) * NumericT<T>::sqr((y - offset.y()) / sigma));

				for (int x = -1; x <= 1; ++x)
				{
					const T xValue = T(1) / (sigma * NumericT<T>::sqr(2 * NumericT<T>::pi())) * NumericT<T>::exp(T(-0.5) * NumericT<T>::sqr((x - offset.x()) / sigma));

					const T value = xValue * yValue;

					values.push_back(value);

					sum += value;
				}
			}

			ocean_assert(NumericT<T>::isNotEqualEps(sum));

			for (T& value : values)
			{
				value /= sum;
			}

			VectorT2<T> precisePeak(NumericT<T>::minValue(), NumericT<T>::minValue());
			if (CV::NonMaximumSuppressionT<T>::determinePrecisePeakLocation2(values.data() + 0, values.data() + 3, values.data() + 6, precisePeak))
			{
				const T distance = precisePeak.distance(offset);

				OCEAN_EXPECT_LESS_EQUAL(validation, distance, T(0.25));
			}
			else
			{
				OCEAN_SET_FAILED(validation);
			}
		}
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestNonMaximumSuppression::testDeterminePrecisePeakLocationNxN()
{
	Log::info() << "Test determinePrecisePeakLocationNxN, with " << TypeNamer::name<T>() << ":";

	Validation validation;

	{
		// flat surface — should return (0, 0)

		constexpr T values3x3[9] = {T(5), T(5), T(5), T(5), T(5), T(5), T(5), T(5), T(5)};

		T offsetX = NumericT<T>::minValue();
		T offsetY = NumericT<T>::minValue();

		const bool result = CV::NonMaximumSuppressionT<int32_t>::determinePrecisePeakLocationNxN<T, 3u>(values3x3, 3u, offsetX, offsetY);
		OCEAN_EXPECT_TRUE(validation, result);
		OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(offsetX, T(0), T(0.001)));
		OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(offsetY, T(0), T(0.001)));
	}

	{
		// symmetric peak centered — should return (0, 0)

		constexpr T values3x3[9] = {T(1), T(2), T(1), T(2), T(4), T(2), T(1), T(2), T(1)};

		T offsetX = NumericT<T>::minValue();
		T offsetY = NumericT<T>::minValue();

		const bool result = CV::NonMaximumSuppressionT<int32_t>::determinePrecisePeakLocationNxN<T, 3u>(values3x3, 3u, offsetX, offsetY);
		OCEAN_EXPECT_TRUE(validation, result);
		OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(offsetX, T(0), T(0.001)));
		OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(offsetY, T(0), T(0.001)));
	}

	{
		// peak shifted to the right — offset should be positive in x

		constexpr T values3x3[9] = {T(1), T(2), T(1), T(2), T(4), T(3), T(1), T(2), T(1)};

		T offsetX = NumericT<T>::minValue();
		T offsetY = NumericT<T>::minValue();

		const bool result = CV::NonMaximumSuppressionT<int32_t>::determinePrecisePeakLocationNxN<T, 3u>(values3x3, 3u, offsetX, offsetY);
		OCEAN_EXPECT_TRUE(validation, result);
		OCEAN_EXPECT_GREATER(validation, offsetX, T(0));
		OCEAN_EXPECT_LESS_EQUAL(validation, offsetX, T(1));
	}

	{
		// peak shifted down — offset should be positive in y

		constexpr T values3x3[9] = {T(1), T(2), T(1), T(2), T(4), T(2), T(1), T(3), T(1)};

		T offsetX = NumericT<T>::minValue();
		T offsetY = NumericT<T>::minValue();

		const bool result = CV::NonMaximumSuppressionT<int32_t>::determinePrecisePeakLocationNxN<T, 3u>(values3x3, 3u, offsetX, offsetY);
		OCEAN_EXPECT_TRUE(validation, result);
		OCEAN_EXPECT_GREATER(validation, offsetY, T(0));
		OCEAN_EXPECT_LESS_EQUAL(validation, offsetY, T(1));
	}

	{
		// 2D Gaussian on a 3x3 grid with known sub-pixel offset

		const VectorsT2<T> offsets =
		{
			VectorT2<T>(T(0), T(0)),
			VectorT2<T>(T(-0.3), T(0.2)),
			VectorT2<T>(T(0.4), T(-0.1)),
		};

		for (const VectorT2<T>& offset : offsets)
		{
			constexpr T sigma = T(1);

			T values3x3[9];

			for (int y = -1; y <= 1; ++y)
			{
				for (int x = -1; x <= 1; ++x)
				{
					const T xValue = NumericT<T>::exp(T(-0.5) * NumericT<T>::sqr(T(x) - offset.x()) / (sigma * sigma));
					const T yValue = NumericT<T>::exp(T(-0.5) * NumericT<T>::sqr(T(y) - offset.y()) / (sigma * sigma));
					values3x3[(y + 1) * 3 + (x + 1)] = xValue * yValue;
				}
			}

			T offsetX = NumericT<T>::minValue();
			T offsetY = NumericT<T>::minValue();

			if (CV::NonMaximumSuppressionT<int32_t>::determinePrecisePeakLocationNxN<T, 3u>(values3x3, 3u, offsetX, offsetY))
			{
				OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(offsetX, offset.x(), T(0.15)));
				OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(offsetY, offset.y(), T(0.15)));
			}
			else
			{
				OCEAN_SET_FAILED(validation);
			}
		}
	}

	{
		// 5x5 grid with Gaussian

		constexpr T sigma = T(1.5);

		const VectorT2<T> offset(T(0.25), T(-0.15));

		T values5x5[25];

		for (int y = -2; y <= 2; ++y)
		{
			for (int x = -2; x <= 2; ++x)
			{
				const T xValue = NumericT<T>::exp(T(-0.5) * NumericT<T>::sqr(T(x) - offset.x()) / (sigma * sigma));
				const T yValue = NumericT<T>::exp(T(-0.5) * NumericT<T>::sqr(T(y) - offset.y()) / (sigma * sigma));
				values5x5[(y + 2) * 5 + (x + 2)] = xValue * yValue;
			}
		}

		T offsetX = NumericT<T>::minValue();
		T offsetY = NumericT<T>::minValue();

		if (CV::NonMaximumSuppressionT<int32_t>::determinePrecisePeakLocationNxN<T, 5u>(values5x5, 5u, offsetX, offsetY))
		{
			OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(offsetX, offset.x(), T(0.15)));
			OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(offsetY, offset.y(), T(0.15)));
		}
		else
		{
			OCEAN_SET_FAILED(validation);
		}
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestNonMaximumSuppression::testDeterminePrecisePeakLocationIterativeNxN()
{
	Log::info() << "Test iterative precise peak location, with " << TypeNamer::name<T>() << ":";

	Validation validation;

	constexpr unsigned int frameWidth = 100u;
	constexpr unsigned int frameHeight = 100u;

	{
		// bright blob (find maximum), grid size 3

		T truePeakX = T(50.3);
		T truePeakY = T(40.7);
		const T sigma = T(3);

		const Frame frame = createGaussianFrame(frameWidth, frameHeight, truePeakX, truePeakY, sigma, false);

		T preciseX = NumericT<T>::minValue();
		T preciseY = NumericT<T>::minValue();

		const CV::NonMaximumSuppression::RefinementStatus resultTopLeft = CV::NonMaximumSuppressionT<int32_t>::determinePrecisePeakLocationIterativeNxN<T, 3u, true>(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), T(50), T(41), preciseX, preciseY);

		OCEAN_EXPECT_EQUAL(validation, resultTopLeft, CV::NonMaximumSuppression::RS_CONVERGED);

		const T errorTopLeft = NumericT<T>::sqrt(NumericT<T>::sqr(preciseX - truePeakX) + NumericT<T>::sqr(preciseY - truePeakY));
		OCEAN_EXPECT_LESS(validation, errorTopLeft, T(0.3));

		truePeakX += T(0.5);
		truePeakY += T(0.5);

		preciseX = NumericT<T>::minValue();
		preciseY = NumericT<T>::minValue();

		const CV::NonMaximumSuppression::RefinementStatus resultCenter = CV::NonMaximumSuppressionT<int32_t>::determinePrecisePeakLocationIterativeNxN<T, 3u, true, CV::PC_CENTER>(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), T(50) + T(0.5), T(41) + T(0.5), preciseX, preciseY);

		OCEAN_EXPECT_EQUAL(validation, resultCenter, CV::NonMaximumSuppression::RS_CONVERGED);

		const T errorCenter = NumericT<T>::sqrt(NumericT<T>::sqr(preciseX - truePeakX) + NumericT<T>::sqr(preciseY - truePeakY));
		OCEAN_EXPECT_LESS(validation, errorCenter, T(0.3));
	}

	{
		// dark blob (find minimum), grid size 3

		T truePeakX = T(60.4);
		T truePeakY = T(30.2);
		const T sigma = T(3);

		const Frame frame = createGaussianFrame(frameWidth, frameHeight, truePeakX, truePeakY, sigma, true);

		T preciseX = NumericT<T>::minValue();
		T preciseY = NumericT<T>::minValue();

		const CV::NonMaximumSuppression::RefinementStatus resultTopLeft = CV::NonMaximumSuppressionT<int32_t>::determinePrecisePeakLocationIterativeNxN<T, 3u, false>(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), T(60), T(30), preciseX, preciseY);

		OCEAN_EXPECT_EQUAL(validation, resultTopLeft, CV::NonMaximumSuppression::RS_CONVERGED);

		const T errorTopLeft = NumericT<T>::sqrt(NumericT<T>::sqr(preciseX - truePeakX) + NumericT<T>::sqr(preciseY - truePeakY));
		OCEAN_EXPECT_LESS(validation, errorTopLeft, T(0.3));

		truePeakX += T(0.5);
		truePeakY += T(0.5);

		preciseX = NumericT<T>::minValue();
		preciseY = NumericT<T>::minValue();

		const CV::NonMaximumSuppression::RefinementStatus resultCenter = CV::NonMaximumSuppressionT<int32_t>::determinePrecisePeakLocationIterativeNxN<T, 3u, false, CV::PC_CENTER>(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), T(60) + T(0.5), T(30) + T(0.5), preciseX, preciseY);

		OCEAN_EXPECT_EQUAL(validation, resultCenter, CV::NonMaximumSuppression::RS_CONVERGED);

		const T errorCenter = NumericT<T>::sqrt(NumericT<T>::sqr(preciseX - truePeakX) + NumericT<T>::sqr(preciseY - truePeakY));
		OCEAN_EXPECT_LESS(validation, errorCenter, T(0.3));
	}

	{
		// bright blob, grid size 5

		T truePeakX = T(45.6);
		T truePeakY = T(55.8);
		const T sigma = T(4);

		const Frame frame = createGaussianFrame(frameWidth, frameHeight, truePeakX, truePeakY, sigma, false);

		T preciseX = NumericT<T>::minValue();
		T preciseY = NumericT<T>::minValue();

		const CV::NonMaximumSuppression::RefinementStatus resultTopLeft = CV::NonMaximumSuppressionT<int32_t>::determinePrecisePeakLocationIterativeNxN<T, 5u, true>(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), T(46), T(56), preciseX, preciseY);

		OCEAN_EXPECT_EQUAL(validation, resultTopLeft, CV::NonMaximumSuppression::RS_CONVERGED);

		const T errorTopLeft = NumericT<T>::sqrt(NumericT<T>::sqr(preciseX - truePeakX) + NumericT<T>::sqr(preciseY - truePeakY));
		OCEAN_EXPECT_LESS(validation, errorTopLeft, T(0.3));

		truePeakX += T(0.5);
		truePeakY += T(0.5);

		preciseX = NumericT<T>::minValue();
		preciseY = NumericT<T>::minValue();

		const CV::NonMaximumSuppression::RefinementStatus resultCenter = CV::NonMaximumSuppressionT<int32_t>::determinePrecisePeakLocationIterativeNxN<T, 5u, true, CV::PC_CENTER>(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), T(46) + T(0.5), T(56) + T(0.5), preciseX, preciseY);

		OCEAN_EXPECT_EQUAL(validation, resultCenter, CV::NonMaximumSuppression::RS_CONVERGED);

		const T errorCenter = NumericT<T>::sqrt(NumericT<T>::sqr(preciseX - truePeakX) + NumericT<T>::sqr(preciseY - truePeakY));
		OCEAN_EXPECT_LESS(validation, errorCenter, T(0.3));
	}

	{
		// peak at border -> returns false with RS_BORDER

		const T truePeakX = T(0.5);
		const T truePeakY = T(0.5);
		const T sigma = T(3);

		const Frame frame = createGaussianFrame(frameWidth, frameHeight, truePeakX, truePeakY, sigma, false);

		T preciseX = NumericT<T>::minValue();
		T preciseY = NumericT<T>::minValue();
		const CV::NonMaximumSuppression::RefinementStatus result = CV::NonMaximumSuppressionT<int32_t>::determinePrecisePeakLocationIterativeNxN<T, 3u, true>(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), T(0), T(0), preciseX, preciseY);

		OCEAN_EXPECT_EQUAL(validation, result, CV::NonMaximumSuppression::RS_BORDER);
	}

	{
		// flat region -> quadratic fit returns (0, 0), position is valid

		Frame frame(FrameType(frameWidth, frameHeight, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
		frame.setValue(128u);

		T preciseX = NumericT<T>::minValue();
		T preciseY = NumericT<T>::minValue();
		const CV::NonMaximumSuppression::RefinementStatus result = CV::NonMaximumSuppressionT<int32_t>::determinePrecisePeakLocationIterativeNxN<T, 3u, true>(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), T(50), T(50), preciseX, preciseY);

		OCEAN_EXPECT_EQUAL(validation, result, CV::NonMaximumSuppression::RS_CONVERGED);
		OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(preciseX, T(50), T(0.01)));
		OCEAN_EXPECT_TRUE(validation, NumericT<T>::isEqual(preciseY, T(50), T(0.01)));
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestNonMaximumSuppression::testCandidate(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test candidate lookup:";

	using GroundTruthMap = std::map<IndexPair32, uint32_t, bool(*)(const IndexPair32&, const IndexPair32&)>;

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 10u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 10u, 1080u);

		const unsigned int yOffset = RandomI::random(randomGenerator, 0u, 100u);

		CV::NonMaximumSuppressionT<uint32_t> nonMaximumSuppression(width, height, yOffset);

		GroundTruthMap groundTruth(comparePositionYX);

		const unsigned int numberCandidates = RandomI::random(randomGenerator, 1u, 500u);

		for (unsigned int n = 0u; n < numberCandidates; ++n)
		{
			const unsigned int y = RandomI::random(randomGenerator, yOffset, yOffset + height - 1u);
			const unsigned int x = RandomI::random(randomGenerator, 0u, width - 1u);
			const uint32_t strength = RandomI::random32(randomGenerator);

			const IndexPair32 key(x, y);

			if (groundTruth.find(key) != groundTruth.cend())
			{
				continue;
			}

			groundTruth[key] = strength;
		}

		// the map is sorted by (y, x), so iterating adds candidates row by row in ascending x order

		for (GroundTruthMap::const_iterator i = groundTruth.cbegin(); i != groundTruth.cend(); ++i)
		{
			nonMaximumSuppression.addCandidate(i->first.first, i->first.second, i->second);
		}

		// verify that every inserted candidate can be found with the correct strength

		for (GroundTruthMap::const_iterator i = groundTruth.cbegin(); i != groundTruth.cend(); ++i)
		{
			uint32_t strength = 0u;
			const bool found = nonMaximumSuppression.candidate(i->first.first, i->first.second, strength);

			OCEAN_EXPECT_TRUE(validation, found);

			if (found)
			{
				OCEAN_EXPECT_EQUAL(validation, strength, i->second);
			}
		}

		// verify that positions without candidates return false

		for (unsigned int n = 0u; n < 100u; ++n)
		{
			const unsigned int y = RandomI::random(randomGenerator, yOffset, yOffset + height - 1u);
			const unsigned int x = RandomI::random(randomGenerator, 0u, width - 1u);

			const IndexPair32 key(x, y);

			if (groundTruth.find(key) != groundTruth.cend())
			{
				continue;
			}

			uint32_t strength = 0u;
			const bool found = nonMaximumSuppression.candidate(x, y, strength);

			OCEAN_EXPECT_FALSE(validation, found);
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

void TestNonMaximumSuppression::createFeaturePoints(Frame& yFrame, const unsigned int features, const uint8_t featurePointStrength)
{
	ocean_assert(yFrame.isValid() && yFrame.pixelFormat() == FrameType::FORMAT_Y8);

	constexpr uint8_t backgroundColor = 0x00;
	ocean_assert(backgroundColor != featurePointStrength);

	yFrame.setValue(backgroundColor);

	for (unsigned int n = 0u; n < features; ++n)
	{
		const Scalar positionX = Random::scalar(-5, Scalar(yFrame.width() + 4u));
		const Scalar positionY = Random::scalar(-5, Scalar(yFrame.height() + 4u));

		CV::Canvas::point<5u>(yFrame, Vector2(positionX, positionY), &featurePointStrength);
	}
}

void TestNonMaximumSuppression::determineFeaturePointsSubset(const Frame* yFrame, const uint8_t minimalThreshold, const bool strict, Lock* lock, StrengthPositions* locations, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(yFrame != nullptr);
	ocean_assert(yFrame->pixelFormat() == FrameType::FORMAT_Y8);
	ocean_assert(yFrame->width() >= 3u && yFrame->height() >= 3u);
	ocean_assert(firstColumn >= 1u && firstColumn + numberColumns <= yFrame->width() - 1u);
	ocean_assert(firstRow >= 1u && firstRow + numberRows <= yFrame->height() - 1u);

	StrengthPositions localLocations;

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		const uint8_t* data0 = yFrame->constpixel<uint8_t>(firstColumn, y - 1u);
		const uint8_t* data1 = yFrame->constpixel<uint8_t>(firstColumn, y);
		const uint8_t* data2 = yFrame->constpixel<uint8_t>(firstColumn, y + 1u);

		for (unsigned int x = firstColumn; x < firstColumn + numberColumns; ++x)
		{
			if (*data1 >= minimalThreshold)
			{
				if (strict)
				{
					// the value of the center pixel is higher than all values in the 8 neighborhood

					if (*data1 > *(data0 - 1) && *data1 > *data0 && *data1 > *(data0 + 1)
							&& *data1 > *(data1 - 1) && *data1 > *(data1 + 1)
							&& *data1 > *(data2 - 1) && *data1 > *data2 && *data1 > *(data2 + 1))
					{
						localLocations.emplace_back(x, y, *data1);
					}
				}
				else
				{
					// the value of the center pixel is higher or equal compared to the upper row and left column,
					// the value of the center pixel is higher than the values in the lower right triangle

					if (*data1 >= *(data0 - 1) && *data1 >= *data0 && *data1 >= *(data0 + 1)
							&& *data1 >= *(data1 - 1) && *data1 > *(data1 + 1)
							&& *data1 >= *(data2 - 1) && *data1 > *data2 && *data1 > *(data2 + 1))
					{
						localLocations.emplace_back(x, y, *data1);
					}
				}
			}

			++data0;
			++data1;
			++data2;
		}
	}

	const OptionalScopedLock scopedLock(lock);

	locations->insert(locations->end(), localLocations.begin(), localLocations.end());
}

template <typename T>
Frame TestNonMaximumSuppression::createGaussianFrame(const unsigned int frameWidth, const unsigned int frameHeight, const T centerX, const T centerY, const T sigma, const bool darkBlob)
{
	Frame frame(FrameType(frameWidth, frameHeight, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));

	for (unsigned int y = 0u; y < frameHeight; ++y)
	{
		uint8_t* row = frame.row<uint8_t>(y);

		for (unsigned int x = 0u; x < frameWidth; ++x)
		{
			const T dx = T(x) - centerX;
			const T dy = T(y) - centerY;
			const T gaussValue = NumericT<T>::exp(T(-0.5) * (dx * dx + dy * dy) / (sigma * sigma));

			if (darkBlob)
			{
				row[x] = uint8_t(T(255) * (T(1) - gaussValue));
			}
			else
			{
				row[x] = uint8_t(T(255) * gaussValue);
			}
		}
	}

	return frame;
}

}

}

}
