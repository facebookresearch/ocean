/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestSampleMap.h"

#include "ocean/base/Timestamp.h"

#include "ocean/math/Quaternion.h"
#include "ocean/math/Random.h"
#include "ocean/math/SampleMap.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestSampleMap::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "---   SampleMap test:   ---";
	Log::info() << " ";

	allSucceeded = testSampleMostRecent(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSampleSpecific(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSampleInterpolation(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testStressTest(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "SampleMap test succeeded.";
	}
	else
	{
		Log::info() << "SampleMap test FAILED";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestSampleMap, SampleMostRecent)
{
	EXPECT_TRUE(TestSampleMap::testSampleMostRecent(GTEST_TEST_DURATION));
}

TEST(TestSampleMap, SampleSpecific)
{
	EXPECT_TRUE(TestSampleMap::testSampleSpecific(GTEST_TEST_DURATION));
}

TEST(TestSampleMap, SampleInterpolation)
{
	EXPECT_TRUE(TestSampleMap::testSampleInterpolation(GTEST_TEST_DURATION));
}

TEST(TestSampleMap, StressTest)
{
	EXPECT_TRUE(TestSampleMap::testStressTest(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestSampleMap::testSampleMostRecent(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Sample test with most recent element:";

	RandomGenerator randomGenerator;
	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int capacity = RandomI::random(randomGenerator, 1u, 100u);
		const unsigned int iterations = RandomI::random(1u, capacity);

		Vector2 mostRecentValue(0, 0);
		double mostRecentTimestamp = NumericD::minValue();

		SampleMap<Vector2> sampleMap(capacity);

		for (unsigned int n = 0u; n < iterations; ++n)
		{
			const double timestamp = RandomD::scalar(randomGenerator, -1000, 1000);
			const Vector2 value = Random::vector2();

			if (timestamp >= mostRecentTimestamp)
			{
				mostRecentTimestamp = timestamp;
				mostRecentValue = value;
			}

			sampleMap.insert(value, timestamp);
		}

		Vector2 value;
		if (!sampleMap.sample(value) || value != mostRecentValue)
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

bool TestSampleMap::testSampleSpecific(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "sample test with specific element:";

	RandomGenerator randomGenerator;
	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int capacity = RandomI::random(randomGenerator, 1u, 100u);
		const unsigned int iterations = RandomI::random(1u, capacity);

		std::map<double, Vector2> testMap;

		SampleMap<Vector2> sampleMap(capacity);

		for (unsigned int n = 0u; n < iterations; ++n)
		{
			const double timestamp = RandomD::scalar(randomGenerator, -1000, 1000);
			const Vector2 value = Random::vector2();

			testMap[timestamp] = value; // not using insert to ensure that identical timestamps overwrite previous entries
			sampleMap.insert(value, timestamp);
		}

		// let's ensure that we can access each sample

		for (std::map<double, Vector2>::const_iterator i = testMap.cbegin(); i != testMap.cend(); ++i)
		{

			Vector2 value;
			if (!sampleMap.sample(i->first, value) || value != i->second)
			{
				allSucceeded = false;
			}
		}

		// let's ensure that an arbitrary timestamp does not produce a corresponding sample

		for (unsigned int n = 0u; n < 10u; ++n)
		{
			const double timestamp = RandomD::scalar(randomGenerator, -1000, 1000);

			if (testMap.find(timestamp) == testMap.cend())
			{
				Vector2 value;
				if (sampleMap.sample(timestamp, value))
				{
					allSucceeded = false;
				}
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

bool TestSampleMap::testSampleInterpolation(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Sample test with interpolation:";

	RandomGenerator randomGenerator;
	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		for (const SampleMap<float>::InterpolationStrategy interpolationStrategy : {SampleMap<float>::IS_TIMESTAMP_NEAREST, SampleMap<float>::IS_TIMESTAMP_INTERPOLATE})
		{
			SampleMap<float> sampleMap(RandomI::random(randomGenerator, 2u, 100u));

			const double lowerTimestamp = RandomD::scalar(randomGenerator, -1000, 1000);
			const float lowerValue = RandomF::scalar(randomGenerator, -10.0f, 10.0f);

			const double higherTimestamp = lowerTimestamp + RandomD::scalar(randomGenerator, 0.0001, 1000);
			const float higherValue = RandomF::scalar(randomGenerator, -10.0f, 10.0f);

			ocean_assert(higherTimestamp > lowerTimestamp);

			sampleMap.insert(lowerValue, lowerTimestamp);
			sampleMap.insert(higherValue, higherTimestamp);

			{
				// timestamp below lower timestamp

				const double timestamp = RandomD::scalar(randomGenerator, -2000, lowerTimestamp - Numeric::weakEps());

				float interpolatedValue = NumericF::minValue();

				double timestampDistance = NumericD::maxValue();
				if (!sampleMap.sample(timestamp, interpolationStrategy, interpolatedValue, &timestampDistance))
				{
					allSucceeded = false;
				}

				if (NumericF::isNotWeakEqual(interpolatedValue, lowerValue))
				{
					allSucceeded = false;
				}

				if (NumericD::isNotEqual(timestampDistance, NumericD::abs(timestamp - lowerTimestamp)))
				{
					allSucceeded = false;
				}
			}

			{
				// timestamp between sample values

				const double timestamp = RandomD::scalar(randomGenerator, lowerTimestamp, higherTimestamp);

				float interpolatedValue = NumericF::minValue();

				double timestampDistance = NumericD::maxValue();
				if (!sampleMap.sample(timestamp, interpolationStrategy, interpolatedValue, &timestampDistance))
				{
					allSucceeded = false;
				}

				if (interpolationStrategy == SampleMap<float>::IS_TIMESTAMP_NEAREST)
				{
					if (interpolatedValue != lowerValue && interpolatedValue != higherValue)
					{
						allSucceeded = false;
					}

					const double lowerDistance = timestamp - lowerTimestamp;
					const double higherDistance = higherTimestamp - timestamp;

					ocean_assert(lowerDistance >= 0.0 && higherDistance >= 0.0);

					if (NumericD::isNotWeakEqual(lowerDistance, higherDistance))
					{
						if (lowerDistance > higherDistance)
						{
							if (interpolatedValue != higherValue)
							{
								allSucceeded = false;
							}
						}
						else
						{
							ocean_assert(higherDistance > lowerDistance);

							if (interpolatedValue != lowerValue)
							{
								allSucceeded = false;
							}
						}
					}
				}
				else
				{
					const double interpolationFactor = (timestamp - lowerTimestamp) / (higherTimestamp - lowerTimestamp);
					ocean_assert(interpolationFactor >= 0.0 && interpolationFactor <= 1.0);

					const float testInterpolatedValue = float(interpolationFactor * double(higherValue) + (1.0 - interpolationFactor) * double(lowerValue));

					if (NumericF::isNotWeakEqual(interpolatedValue, testInterpolatedValue))
					{
						allSucceeded = false;
					}
				}

				if (NumericD::isNotEqual(timestampDistance, std::min(NumericD::abs(higherTimestamp - timestamp), NumericD::abs(timestamp - lowerTimestamp))))
				{
					allSucceeded = false;
				}
			}

			{
				// timestamp above higher timestamp

				const double timestamp = RandomD::scalar(randomGenerator, higherTimestamp + Numeric::weakEps(), 2000);

				float interpolatedValue = NumericF::minValue();

				double timestampDistance = NumericD::maxValue();
				if (!sampleMap.sample(timestamp, interpolationStrategy, interpolatedValue, &timestampDistance))
				{
					allSucceeded = false;
				}

				if (NumericF::isNotWeakEqual(interpolatedValue, higherValue))
				{
					allSucceeded = false;
				}

				if (NumericD::isNotEqual(timestampDistance, NumericD::abs(timestamp - higherTimestamp)))
				{
					allSucceeded = false;
				}
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

bool TestSampleMap::testStressTest(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Stress test:";

	// this test is not testing correctness or benchmarking performance
	// we simply ensure that the SampleMap does not crash

	RandomGenerator randomGenerator;
	bool allSucceeded = true;

	unsigned long long dummyValue = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		const size_t capacity = RandomI::random(randomGenerator, 1u, 1000u);

		SampleMap<Quaternion> sampleMap(capacity);

		const unsigned int iterations = RandomI::random(randomGenerator, 1u, 100u);

		for (unsigned int n = 0u; n < iterations; ++n)
		{
			sampleMap.insert(Random::quaternion(randomGenerator), RandomD::scalar(randomGenerator, -1000, 1000));

			switch (RandomI::random(randomGenerator, 10u))
			{
				case 0u:
				{
					Quaternion value;

					if (sampleMap.sample(value))
					{
						dummyValue++;
					}

					break;
				}

				case 1u:
				{
					Quaternion value;

					if (sampleMap.sample(RandomD::scalar(-1000, 1000), value))
					{
						dummyValue++;
					}

					break;
				}

				case 2u:
				{
					Quaternion value;

					if (sampleMap.sample(RandomD::scalar(-1000, 1000), SampleMap<Quaternion>::IS_TIMESTAMP_NEAREST, value))
					{
						dummyValue++;
					}

					break;
				}

				case 3u:
				{
					Quaternion value;

					if (sampleMap.sample(RandomD::scalar(-1000, 1000), SampleMap<Quaternion>::IS_TIMESTAMP_INTERPOLATE, value))
					{
						dummyValue++;
					}

					break;
				}

				default:
					// we do nothing in case 4 - 9
					break;
			}

			if (RandomI::random(randomGenerator, 20u) == 0u)
			{
				sampleMap.clear();
			}

			if (sampleMap.size() > capacity)
			{
				allSucceeded = false;
			}

			if (sampleMap.capacity() != capacity)
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		if (dummyValue == 0u)
		{
			Log::info() << "Validation: succeeded.";
		}
		else
		{
			Log::info() << "Validation: succeeded.";
		}
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
