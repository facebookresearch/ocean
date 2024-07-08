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

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestNonMaximumSuppression::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Non maximum suppression test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testSuppressionInFrame(width, height, width, height, true, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSuppressionInFrame(width, height, width, height, false, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSuppressionInFrame(width, height, width * 75u / 100u, height * 75u / 100u, true, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSuppressionInFrame(width, height, width * 75u / 100u, height * 75u / 100u, false, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSuppressionInStrengthPositions(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDeterminePrecisePeakLocation1<float>() && allSucceeded;
	Log::info() << " ";
	allSucceeded = testDeterminePrecisePeakLocation1<double>() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDeterminePrecisePeakLocation2<float>() && allSucceeded;
	Log::info() << " ";
	allSucceeded = testDeterminePrecisePeakLocation2<double>() && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Non maximum suppression test succeeded.";
	}
	else
	{
		Log::info() << "Non maximum suppression test FAILED!";
	}

	return allSucceeded;
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


TEST(TestNonMaximumSuppression, testSuppressionInStrengthPositions_uint32_float)
{
	EXPECT_TRUE((TestNonMaximumSuppression::testSuppressionInStrengthPositions<uint32_t, float>(GTEST_TEST_DURATION)));
}

TEST(TestNonMaximumSuppression, testSuppressionInStrengthPositions_double_double)
{
	EXPECT_TRUE((TestNonMaximumSuppression::testSuppressionInStrengthPositions<double, double>(GTEST_TEST_DURATION)));
}


TEST(TestNonMaximumSuppression, DeterminePrecisePeakLocation1_Float)
{
	EXPECT_TRUE(TestNonMaximumSuppression::testDeterminePrecisePeakLocation1<float>());
}

TEST(TestNonMaximumSuppression, DeterminePrecisePeakLocation1_Double)
{
	EXPECT_TRUE(TestNonMaximumSuppression::testDeterminePrecisePeakLocation1<double>());
}


TEST(TestNonMaximumSuppression, DeterminePrecisePeakLocation2_Float)
{
	EXPECT_TRUE(TestNonMaximumSuppression::testDeterminePrecisePeakLocation2<float>());
}

TEST(TestNonMaximumSuppression, DeterminePrecisePeakLocation2_Double)
{
	EXPECT_TRUE(TestNonMaximumSuppression::testDeterminePrecisePeakLocation2<double>());
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

	bool allSucceeded = true;

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
			const uint8_t minimalThreshold = uint8_t(RandomI::random(40u, 150u));

			const unsigned int paddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);

			Frame yFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), paddingElements);
			createFeaturePoints(yFrame, features);

			const unsigned int subFrameLeft = RandomI::random(0u, width - subFrameWidth);
			const unsigned int subFrameTop = RandomI::random(0u, height - subFrameHeight);

			const unsigned int subFrameExtraBorderLeft = (unsigned int)(std::max(0, int(subFrameLeft) - 1)); // extra border is necessary to get same results as in test function
			const unsigned int subFrameExtraBorderTop = (unsigned int)(std::max(0, int(subFrameTop) - 1));
			const unsigned int subFrameExtraBorderRight = std::min(subFrameLeft + subFrameWidth + 1u, width);
			const unsigned int subFrameExtraBorderBottom = std::min(subFrameTop + subFrameHeight + 1u, height);

			performanceCreation.start();
				CV::NonMaximumSuppression<uint8_t> nonMaximumSuppression(width, height, 0u);
				nonMaximumSuppression.addCandidates(yFrame.constdata<uint8_t>(), yFrame.paddingElements(), subFrameExtraBorderLeft, subFrameExtraBorderRight - subFrameExtraBorderLeft, subFrameExtraBorderTop, subFrameExtraBorderBottom - subFrameExtraBorderTop, minimalThreshold, useWorker);
			performanceCreation.stop();

			StrengthPositions locations;

			performanceDetermination.start();
				if (strictMaximum)
				{
					locations = nonMaximumSuppression.suppressNonMaximum<int, uint8_t, true>(subFrameLeft, subFrameWidth, subFrameTop, subFrameHeight, useWorker, nullptr);
				}
				else
				{
					locations = nonMaximumSuppression.suppressNonMaximum<int, uint8_t, false>(subFrameLeft, subFrameWidth, subFrameTop, subFrameHeight, useWorker, nullptr);
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

			if (locationSet != testLocationSet || testLocationSet.size() != naiveLocations.size())
			{
				allSucceeded = false;
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));
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

bool TestNonMaximumSuppression::testSuppressionInStrengthPositions(const double testDuration)
{
	Log::info() << "Test non maximum suppression of strength positions:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testSuppressionInStrengthPositions<uint32_t, float>(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testSuppressionInStrengthPositions<double, double>(testDuration) && allSucceeded;

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

template <typename TCoordinate, typename TStrength>
bool TestNonMaximumSuppression::testSuppressionInStrengthPositions(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... with coordinate data type '" << TypeNamer::name<TCoordinate>() << "' and strength parameter '" << TypeNamer::name<TStrength>() << "':";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 10u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 10u, 1920u);

		const unsigned int size = RandomI::random(randomGenerator, 1u, 10000u);

		CV::NonMaximumSuppression<double>::StrengthPositions<TCoordinate, TStrength> strengthPositions;
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
		CV::NonMaximumSuppression<double>::StrengthPositions<TCoordinate, TStrength> remainingStrengthPositions;

		if (strictMaximum)
		{
			remainingStrengthPositions = CV::NonMaximumSuppression<double>::suppressNonMaximum<TCoordinate, TStrength, true>(width, height, strengthPositions, radius, &validIndices);
		}
		else
		{
			remainingStrengthPositions = CV::NonMaximumSuppression<double>::suppressNonMaximum<TCoordinate, TStrength, false>(width, height, strengthPositions, radius, &validIndices);
		}

		const UnorderedIndexSet32 debugValidIndexSet(validIndices.cbegin(), validIndices.cend());
		ocean_assert_and_suppress_unused(debugValidIndexSet.size() == validIndices.size(), debugValidIndexSet);

		Indices32 statementsStrengthPositions(strengthPositions.size(), 1u);

		for (size_t nOuter = 0; nOuter < strengthPositions.size(); ++nOuter)
		{
			const CV::NonMaximumSuppression<double>::StrengthPosition<TCoordinate, TStrength>& outer = strengthPositions[nOuter];

			for (size_t nInner = 0; nInner < strengthPositions.size(); ++nInner)
			{
				if (nInner == nOuter)
				{
					continue;
				}

				const CV::NonMaximumSuppression<double>::StrengthPosition<TCoordinate, TStrength>& inner = strengthPositions[nInner];

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

		CV::NonMaximumSuppression<double>::StrengthPositions<TCoordinate, TStrength> testStrengthPositions;
		testStrengthPositions.reserve(remainingStrengthPositions.size());

		for (size_t n = 0; n < statementsStrengthPositions.size(); ++n)
		{
			if (statementsStrengthPositions[n])
			{
				testStrengthPositions.emplace_back(strengthPositions[n]);
			}
		}

		if (remainingStrengthPositions != testStrengthPositions)
		{
			allSucceeded = false;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	return allSucceeded;
}

template <typename T>
bool TestNonMaximumSuppression::testDeterminePrecisePeakLocation1()
{
	Log::info() << "Test 1D precise peak location, with " << TypeNamer::name<T>() << ":";

	bool allSucceeded = true;

	T precisePeak = NumericT<T>::minValue();
	if (!CV::NonMaximumSuppression<T>::determinePrecisePeakLocation1(0, 0, 0, precisePeak) || precisePeak != 0)
	{
		allSucceeded = false;
	}

	precisePeak = NumericT<T>::minValue();
	if (!CV::NonMaximumSuppression<T>::determinePrecisePeakLocation1(0, 1, 0, precisePeak) || precisePeak != 0)
	{
		allSucceeded = false;
	}

	precisePeak = NumericT<T>::minValue();
	if (!CV::NonMaximumSuppression<T>::determinePrecisePeakLocation1(1, 2, 0, precisePeak) || precisePeak < T(-0.5) || precisePeak >= 0)
	{
		allSucceeded = false;
	}

	precisePeak = NumericT<T>::minValue();
	if (!CV::NonMaximumSuppression<T>::determinePrecisePeakLocation1(0, 2, 1, precisePeak) || precisePeak > T(0.5) || precisePeak <= 0)
	{
		allSucceeded = false;
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
			const T sigma = T(1);

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

			precisePeak = NumericT<T>::minValue();
			if (CV::NonMaximumSuppression<T>::determinePrecisePeakLocation1(values[0], values[1], values[2], precisePeak))
			{
				if (NumericT<T>::isNotEqual(precisePeak, offset, T(0.01)))
				{
					allSucceeded = false;
				}
			}
			else
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

template <typename T>
bool TestNonMaximumSuppression::testDeterminePrecisePeakLocation2()
{
	Log::info() << "Test 2D precise peak location, with " << TypeNamer::name<T>() << ":";

	bool allSucceeded = true;

	{
		const T    topValues[3] = {0, 0, 0};
		const T centerValues[3] = {0, 0, 0};
		const T bottomValues[3] = {0, 0, 0};

		VectorT2<T> precisePeak(NumericT<T>::minValue(), NumericT<T>::minValue());
		if (!CV::NonMaximumSuppression<T>::determinePrecisePeakLocation2(topValues, centerValues, bottomValues, precisePeak) || precisePeak != VectorT2<T>(0, 0))
		{
			allSucceeded = false;
		}
	}

	{
		const T    topValues[3] = {0, 0, 0};
		const T centerValues[3] = {0, 1, 0};
		const T bottomValues[3] = {0, 0, 0};

		VectorT2<T> precisePeak(NumericT<T>::minValue(), NumericT<T>::minValue());
		if (!CV::NonMaximumSuppression<T>::determinePrecisePeakLocation2(topValues, centerValues, bottomValues, precisePeak) || precisePeak != VectorT2<T>(0, 0))
		{
			allSucceeded = false;
		}
	}

	{
		const T    topValues[3] = {0, 0, 0};
		const T centerValues[3] = {1, 1, 1};
		const T bottomValues[3] = {0, 0, 0};

		VectorT2<T> precisePeak(NumericT<T>::minValue(), NumericT<T>::minValue());
		if (!CV::NonMaximumSuppression<T>::determinePrecisePeakLocation2(topValues, centerValues, bottomValues, precisePeak) || precisePeak != VectorT2<T>(0, 0))
		{
			allSucceeded = false;
		}
	}

	{
		const T    topValues[3] = {0, 1, 0};
		const T centerValues[3] = {0, 1, 0};
		const T bottomValues[3] = {0, 1, 0};

		VectorT2<T> precisePeak(NumericT<T>::minValue(), NumericT<T>::minValue());
		if (!CV::NonMaximumSuppression<T>::determinePrecisePeakLocation2(topValues, centerValues, bottomValues, precisePeak) || precisePeak != VectorT2<T>(0, 0))
		{
			allSucceeded = false;
		}
	}

	{
		const T    topValues[3] = {1, 2, 0};
		const T centerValues[3] = {1, 2, 0};
		const T bottomValues[3] = {1, 2, 0};

		VectorT2<T> precisePeak(NumericT<T>::minValue(), NumericT<T>::minValue());
		if (!CV::NonMaximumSuppression<T>::determinePrecisePeakLocation2(topValues, centerValues, bottomValues, precisePeak) || precisePeak != VectorT2<T>(0, 0))
		{
			allSucceeded = false;
		}
	}

	{
		const T    topValues[3] = {1, 1, 1};
		const T centerValues[3] = {2, 2, 2};
		const T bottomValues[3] = {0, 0, 0};

		VectorT2<T> precisePeak(NumericT<T>::minValue(), NumericT<T>::minValue());
		if (!CV::NonMaximumSuppression<T>::determinePrecisePeakLocation2(topValues, centerValues, bottomValues, precisePeak) || precisePeak != VectorT2<T>(0, 0))
		{
			allSucceeded = false;
		}
	}

	{
		const T    topValues[3] = {1, 1, 2};
		const T centerValues[3] = {1, 2, 0};
		const T bottomValues[3] = {2, 0, 0};

		VectorT2<T> precisePeak(NumericT<T>::minValue(), NumericT<T>::minValue());
		if (!CV::NonMaximumSuppression<T>::determinePrecisePeakLocation2(topValues, centerValues, bottomValues, precisePeak) || precisePeak.x() < T(-0.5) || precisePeak.x() >= T(0) || precisePeak.y() < T(-0.5) || precisePeak.y() >= T(0))
		{
			allSucceeded = false;
		}
	}

	{
		const T    topValues[3] = {0, 0, 2};
		const T centerValues[3] = {0, 2, 1};
		const T bottomValues[3] = {2, 1, 1};

		VectorT2<T> precisePeak(NumericT<T>::minValue(), NumericT<T>::minValue());
		if (!CV::NonMaximumSuppression<T>::determinePrecisePeakLocation2(topValues, centerValues, bottomValues, precisePeak) || precisePeak.x() > T(0.5) || precisePeak.x() < T(0) || precisePeak.y() > T(0.5) || precisePeak.y() < T(0))
		{
			allSucceeded = false;
		}
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
			const T sigma = T(1);

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

			for (T& value : values) // normalizing the values
			{
				value /= sum;
			}

			VectorT2<T> precisePeak(NumericT<T>::minValue(), NumericT<T>::minValue());
			if (CV::NonMaximumSuppression<T>::determinePrecisePeakLocation2(values.data() + 0, values.data() + 3, values.data() + 6, precisePeak))
			{
				const T distance = precisePeak.distance(offset);

				if (distance > T(0.25))
				{
					allSucceeded = false;
				}
			}
			else
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

}

}

}
