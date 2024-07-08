/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestLookup2.h"

#include "ocean/base/Frame.h"
#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/Lookup2.h"
#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestLookup2::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Lookup2 test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testCenterLookupBinPositions(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testCenterLookupClampedValues(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testAdvancedCenterLookupClampedValues(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testCornerLookupNearestNeighbor(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testCornerLookupBilinear(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testCornerLookupBilinearValues(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testCornerLookupBilinearSubsetValues(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testCornerLookupClampedValues(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Lookup2 test succeeded.";
	}
	else
	{
		Log::info() << "Lookup2 test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestLookup2, CenterLookupBinPositions)
{
	EXPECT_TRUE(TestLookup2::testCenterLookupBinPositions(GTEST_TEST_DURATION));
}

TEST(TestLookup2, CenterLookupClampedValues)
{
	EXPECT_TRUE(TestLookup2::testCenterLookupClampedValues(GTEST_TEST_DURATION));
}

TEST(TestLookup2, AdvancedCenterLookupClampedValues)
{
	EXPECT_TRUE(TestLookup2::testAdvancedCenterLookupClampedValues(GTEST_TEST_DURATION));
}

TEST(TestLookup2, CornerLookupNearestNeighbor)
{
	EXPECT_TRUE(TestLookup2::testCornerLookupNearestNeighbor(GTEST_TEST_DURATION));
}

TEST(TestLookup2, CornerLookupBilinear)
{
	EXPECT_TRUE(TestLookup2::testCornerLookupBilinear(GTEST_TEST_DURATION));
}

TEST(TestLookup2, CornerLookupBilinearValues)
{
	EXPECT_TRUE(TestLookup2::testCornerLookupBilinearValues(GTEST_TEST_DURATION));
}

TEST(TestLookup2, CornerLookupBilinearSubsetValues)
{
	EXPECT_TRUE(TestLookup2::testCornerLookupBilinearSubsetValues(GTEST_TEST_DURATION));
}

TEST(TestLookup2, CornerLookupClampedValues)
{
	EXPECT_TRUE(TestLookup2::testCornerLookupClampedValues(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestLookup2::testCenterLookupBinPositions(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Center lookup object bin position test:";

	const Timestamp startTimestamp(true);

	bool allSucceeded = true;

	do
	{
		const unsigned int sizeX = RandomI::random(7u, std::is_same<Scalar, float>::value ? 2048u : 8192u);
		const unsigned int sizeY = RandomI::random(7u, std::is_same<Scalar, float>::value ? 2048u : 8192u);

		const unsigned int binsX = RandomI::random(1u, sizeX);
		const unsigned int binsY = RandomI::random(1u, sizeY);

		LookupCenter2<Scalar> lookupObject(sizeX, sizeY, binsX, binsY);
		Frame indexFrame(FrameType(sizeX, sizeY, FrameType::FORMAT_Y32, FrameType::ORIGIN_UPPER_LEFT));

		for (unsigned int y = 0u; y < sizeY; ++y)
		{
			for (unsigned int x = 0u; x < sizeX; ++x)
			{
				const unsigned int xBin = (unsigned int)lookupObject.binX(Scalar(x));
				const unsigned int yBin = (unsigned int)lookupObject.binY(Scalar(y));

				const unsigned int binIndex = yBin * (unsigned int)lookupObject.binsX() + xBin;

				indexFrame.pixel<uint32_t>(x, y)[0] = binIndex;
			}
		}

		Frame visitedFrame(FrameType(sizeX, sizeY, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
		visitedFrame.setValue(0x00);

		for (unsigned int yBin = 0u; yBin < lookupObject.binsY(); ++yBin)
		{
			for (unsigned int xBin = 0u; xBin < lookupObject.binsX(); ++xBin)
			{
				const unsigned int binIndex = yBin * (unsigned int)lookupObject.binsX() + xBin;

				const unsigned int left = (unsigned int)lookupObject.binTopLeftX(xBin);
				const unsigned int right = (unsigned int)lookupObject.binBottomRightX(xBin);
				const unsigned int top = (unsigned int)lookupObject.binTopLeftY(yBin);
				const unsigned int bottom = (unsigned int)lookupObject.binBottomRightY(yBin);

				if (left >= sizeX || right >= sizeX || top >= sizeY || bottom >= sizeY)
				{
					allSucceeded = false;
				}
				else
				{
					for (unsigned int y = top; y <= bottom; ++y)
					{
						for (unsigned int x = left; x <= right; ++x)
						{
							if (visitedFrame.constpixel<uint8_t>(x, y)[0] != 0u)
							{
								allSucceeded = false;
							}

							visitedFrame.pixel<uint8_t>(x, y)[0] = 1u;

							if (indexFrame.constpixel<uint32_t>(x, y)[0u] != binIndex)
							{
								allSucceeded = false;
							}
						}
					}

					const Vector2 testCenter((Scalar(right) + Scalar(left)) * Scalar(0.5), (Scalar(top) + Scalar(bottom)) * Scalar(0.5));
					const Vector2 center = lookupObject.binCenterPosition(xBin, yBin);

					if (testCenter.sqrDistance(center) >= Numeric::sqr(Scalar(0.001)))
					{
						allSucceeded = false;
					}
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

bool TestLookup2::testCenterLookupClampedValues(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Center lookup object clamped value test:";

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int sizeX = RandomI::random(1u, 500u);
		const unsigned int sizeY = RandomI::random(1u, 500u);

		const unsigned int binsX = RandomI::random(1u, sizeX);
		const unsigned int binsY = RandomI::random(1u, sizeY);

		LookupCenter2<Scalar> lookupObject(sizeX, sizeY, binsX, binsY);

		for (unsigned int yBin = 0u; yBin < lookupObject.binsY(); ++yBin)
		{
			for (unsigned int xBin = 0u; xBin < lookupObject.binsX(); ++xBin)
			{
				lookupObject.setBinCenterValue(xBin, yBin, Random::scalar(-100, 100));
			}
		}

		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			bool localSucceeded = true;

			const Scalar x = Random::scalar(-10, Scalar(lookupObject.sizeX()) + 10);
			const Scalar y = Random::scalar(-10, Scalar(lookupObject.sizeY()) + 10);

			if (x >= 0 && y >= 0 && x <= Scalar(lookupObject.sizeX() - 1) && y <= Scalar(lookupObject.sizeY() - 1))
			{
				if (lookupObject.nearestValue(x, y) != lookupObject.clampedNearestValue(x, y))
				{
					localSucceeded = false;
				}

				if (lookupObject.bilinearValue(x, y) != lookupObject.clampedBilinearValue(x, y))
				{
					localSucceeded = false;
				}

				if (lookupObject.bicubicValue(x, y) != lookupObject.clampedBicubicValue(x, y))
				{
					localSucceeded = false;
				}
			}
			else
			{
				Scalar clampedX = x;
				Scalar clampedY = y;

				if (clampedX < 0)
				{
					clampedX = 0;
				}
				if (clampedX > Scalar(lookupObject.sizeX() - 1))
				{
					clampedX = Scalar(lookupObject.sizeX() - 1);
				}

				if (clampedY < 0)
				{
					clampedY = 0;
				}
				if (clampedY > Scalar(lookupObject.sizeY() - 1))
				{
					clampedY = Scalar(lookupObject.sizeY() - 1);
				}

				if (lookupObject.nearestValue(clampedX, clampedY) != lookupObject.clampedNearestValue(x, y))
				{
					localSucceeded = false;
				}

				if (lookupObject.bilinearValue(clampedX, clampedY) != lookupObject.clampedBilinearValue(x, y))
				{
					localSucceeded = false;
				}

				if (lookupObject.bicubicValue(clampedX, clampedY) != lookupObject.clampedBicubicValue(x, y))
				{
					localSucceeded = false;
				}
			}

			if (localSucceeded)
			{
				validIterations++;
			}

			iterations++;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

bool TestLookup2::testAdvancedCenterLookupClampedValues(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Advanced center lookup object clamped value test:";

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int sizeX = RandomI::random(1u, 500u);
		const unsigned int sizeY = RandomI::random(1u, 500u);

		const unsigned int binsX = RandomI::random(1u, sizeX);
		const unsigned int binsY = RandomI::random(1u, sizeY);

		AdvancedLookupCenter2<Scalar> lookupObject(sizeX, sizeY, binsX, binsY);

		for (unsigned int yBin = 0u; yBin < lookupObject.binsY(); ++yBin)
		{
			for (unsigned int xBin = 0u; xBin < lookupObject.binsX(); ++xBin)
			{
				lookupObject.setBinCenterValue(xBin, yBin, Random::scalar(-100, 100), RandomI::random(1u) == 0u ? false : true);
			}
		}

		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			bool localSucceeded = true;

			const Scalar x = Random::scalar(-10, Scalar(lookupObject.sizeX()) + 10);
			const Scalar y = Random::scalar(-10, Scalar(lookupObject.sizeY()) + 10);

			Scalar value = Numeric::maxValue();
			Scalar clampedValue = Numeric::maxValue();

			if (x >= 0 && y >= 0 && x <= Scalar(lookupObject.sizeX() - 1) && y <= Scalar(lookupObject.sizeY() - 1))
			{
				if (lookupObject.nearestValue(x, y, value) != lookupObject.clampedNearestValue(x, y, clampedValue) || value != clampedValue)
				{
					localSucceeded = false;
				}

				if (lookupObject.bilinearValue(x, y, value) != lookupObject.clampedBilinearValue(x, y, clampedValue) || value != clampedValue)
				{
					localSucceeded = false;
				}
			}
			else
			{
				Scalar clampedX = x;
				Scalar clampedY = y;

				if (clampedX < 0)
				{
					clampedX = 0;
				}
				if (clampedX > Scalar(lookupObject.sizeX() - 1))
				{
					clampedX = Scalar(lookupObject.sizeX() - 1);
				}

				if (clampedY < 0)
				{
					clampedY = 0;
				}
				if (clampedY > Scalar(lookupObject.sizeY() - 1))
				{
					clampedY = Scalar(lookupObject.sizeY() - 1);
				}

				if (lookupObject.nearestValue(clampedX, clampedY, value) != lookupObject.clampedNearestValue(x, y, clampedValue) || value != clampedValue)
				{
					localSucceeded = false;
				}

				if (lookupObject.bilinearValue(clampedX, clampedY, value) != lookupObject.clampedBilinearValue(x, y, clampedValue) || value != clampedValue)
				{
					localSucceeded = false;
				}
			}

			if (localSucceeded)
			{
				validIterations++;
			}

			iterations++;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

bool TestLookup2::testCornerLookupNearestNeighbor(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Corner lookup object nearest value test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(1u, 100u);
		const unsigned int height = RandomI::random(1u, 100u);

		LookupCorner2<Vector2> lookupObject(width, height, 1, 1);
		lookupObject.setBinTopLeftCornerValue(0, 0, Vector2(0, 0));
		lookupObject.setBinTopLeftCornerValue(1, 0, Vector2(Scalar(width), 0));
		lookupObject.setBinTopLeftCornerValue(0, 1, Vector2(0, Scalar(height)));
		lookupObject.setBinTopLeftCornerValue(1, 1, Vector2(Scalar(width), Scalar(height)));

		for (unsigned int y = 0u; y <= lookupObject.sizeY(); ++y)
		{
			for (unsigned int x = 0u; x <= lookupObject.sizeX(); ++x)
			{
				Scalar distance = Numeric::maxValue();
				Vectors2 values;

				for (unsigned int yBin = 0u; yBin <= lookupObject.binsY(); ++yBin)
				{
					for (unsigned int xBin = 0u; xBin <= lookupObject.binsX(); ++xBin)
					{
						const Vector2 value = lookupObject.binTopLeftCornerPosition(xBin, yBin);
						const Scalar d = Vector2(Scalar(x), Scalar(y)).sqrDistance(value);

						if (d < distance)
						{
							distance = d;
							values = Vectors2(1, value);
						}
						else if (d == distance)
						{
							values.push_back(value);
						}
					}
				}

				const Vector2 value = lookupObject.nearestValue(Scalar(x), Scalar(y));

				bool found = false;
				for (unsigned int n = 0u; !found && n < values.size(); ++n)
				{
					found = values[n] == value;
				}

				if (!found)
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

bool TestLookup2::testCornerLookupBilinear(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Corner lookup object bilinear value test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(1u, 100u);
		const unsigned int height = RandomI::random(1u, 100u);

		LookupCorner2<Vector2> lookupObject(width, height, 1, 1);
		lookupObject.setBinTopLeftCornerValue(0, 0, Vector2(0, 0));
		lookupObject.setBinTopLeftCornerValue(1, 0, Vector2(Scalar(width), 0));
		lookupObject.setBinTopLeftCornerValue(0, 1, Vector2(0, Scalar(height)));
		lookupObject.setBinTopLeftCornerValue(1, 1, Vector2(Scalar(width), Scalar(height)));

		for (unsigned int y = 0u; y <= lookupObject.sizeY(); ++y)
		{
			for (unsigned int x = 0u; x <= lookupObject.sizeX(); ++x)
			{
				const Vector2 value = lookupObject.bilinearValue(Scalar(x), Scalar(y));

				if (std::is_same<Scalar, float>::value)
				{
					if (Numeric::isNotWeakEqual(value.x(), Scalar(x)) || Numeric::isNotWeakEqual(value.y(), Scalar(y)))
					{
						allSucceeded = false;
					}
				}
				else
				{
					if (Numeric::isNotEqual(value.x(), Scalar(x)) || Numeric::isNotEqual(value.y(), Scalar(y)))
					{
						allSucceeded = false;
					}
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

bool TestLookup2::testCornerLookupBilinearValues(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Corner lookup object bilinear values test:";

	bool allSucceeded = true;

	HighPerformanceStatistic performanceRows;
	HighPerformanceStatistic performanceIndividuals;

	const Timestamp startTimestamp(true);

	do
	{
		{
			// testing accuracy

			const size_t sizeX = RandomI::random(20u, 100u);
			const size_t sizeY = RandomI::random(20u, 100u);

			const size_t binsX = RandomI::random(1u, (unsigned int)(sizeX) / 4u);
			const size_t binsY = RandomI::random(1u, (unsigned int)(sizeY) / 4u);

			LookupCorner2<Vector2> lookupObject(sizeX, sizeY, binsX, binsY);

			for (size_t bY = 0; bY <= binsY; ++bY)
			{
				for (size_t bX = 0; bX <= binsX; ++bX)
				{
					const Vector2 value = Random::vector2(-10, 10);
					lookupObject.setBinTopLeftCornerValue(bX, bY, value);
				}
			}

			Vectors2 valuesIndividuals(sizeX * sizeY);

			for (size_t y = 0; y < sizeY; ++y)
			{
				Vector2* const values = valuesIndividuals.data() + y * sizeX;

				for (size_t x = 0; x < sizeX; ++x)
				{
					values[x] = lookupObject.bilinearValue(Scalar(x), Scalar(y));
				}
			}


			Vectors2 valuesRows(sizeX * sizeY);

			for (size_t y = 0; y < sizeY; ++y)
			{
				lookupObject.bilinearValues(y, valuesRows.data() + y * sizeX);
			}

			ocean_assert(valuesRows.size() == valuesIndividuals.size());

			for (size_t n = 0u; n < valuesRows.size(); ++n)
			{
				const Scalar error = valuesRows[n].distance(valuesIndividuals[n]);

				if (error >= Scalar(0.001))
				{
					allSucceeded = false;
				}
			}
		}

		{
			// testing performance

			const size_t sizeX = 640;
			const size_t sizeY = 480;

			const size_t binsX = 64;
			const size_t binsY = 48;

			LookupCorner2<Vector2> lookupObject(sizeX, sizeY, binsX, binsY);

			for (size_t bY = 0; bY <= binsY; ++bY)
			{
				for (size_t bX = 0; bX <= binsX; ++bX)
				{
					const Vector2 value = Random::vector2(-10, 10);
					lookupObject.setBinTopLeftCornerValue(bX, bY, value);
				}
			}

			Vectors2 valuesIndividuals(sizeX * sizeY);

			performanceIndividuals.start();
			for (size_t y = 0; y < sizeY; ++y)
			{
				Vector2* const values = valuesIndividuals.data() + y * sizeX;

				for (size_t x = 0; x < sizeX; ++x)
				{
					values[x] = lookupObject.bilinearValue(Scalar(x), Scalar(y));
				}
			}
			performanceIndividuals.stop();


			Vectors2 valuesRows(sizeX * sizeY);

			performanceRows.start();
			for (size_t y = 0; y < sizeY; ++y)
			{
				lookupObject.bilinearValues(y, valuesRows.data() + y * sizeX);
			}
			performanceRows.stop();

			for (size_t n = 0u; n < valuesRows.size(); ++n)
			{
				const Scalar error = valuesRows[n].distance(valuesIndividuals[n]);

				if (error >= Scalar(0.001))
				{
					allSucceeded = false;
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance individuals: " << performanceIndividuals.medianMseconds() << "ms";
	Log::info() << "Performance rows: " << performanceRows.medianMseconds() << "ms, " << String::toAString(performanceIndividuals.median() / performanceRows.median(), 2u) << "x";

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

bool TestLookup2::testCornerLookupBilinearSubsetValues(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Corner lookup object bilinear subset values test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		{
			const size_t sizeX = RandomI::random(randomGenerator, 20u, 100u);
			const size_t sizeY = RandomI::random(randomGenerator, 20u, 100u);

			const size_t binsX = RandomI::random(randomGenerator, 1u, (unsigned int)(sizeX) / 4u);
			const size_t binsY = RandomI::random(randomGenerator, 1u, (unsigned int)(sizeY) / 4u);

			LookupCorner2<Vector2> lookupObject(sizeX, sizeY, binsX, binsY);

			for (size_t bY = 0; bY <= binsY; ++bY)
			{
				for (size_t bX = 0; bX <= binsX; ++bX)
				{
					const Vector2 value = Random::vector2(randomGenerator, -10, 10);
					lookupObject.setBinTopLeftCornerValue(bX, bY, value);
				}
			}

			Vectors2 valuesIndividuals(sizeX * sizeY);

			for (size_t y = 0; y < sizeY; ++y)
			{
				Vector2* const values = valuesIndividuals.data() + y * sizeX;

				for (size_t x = 0; x < sizeX; ++x)
				{
					values[x] = lookupObject.bilinearValue(Scalar(x), Scalar(y));
				}
			}

			const size_t size = size_t(RandomI::random(randomGenerator, 1u, (unsigned int)(sizeX)));

			Vectors2 valuesRows(size);

			for (size_t y = 0; y < sizeY; ++y)
			{
				const size_t x = size_t(Random::random(randomGenerator, 0u, (unsigned int)(sizeX - size)));

				lookupObject.bilinearValues(x, y, size, valuesRows.data());

				for (size_t n = x; n < x + size; ++n)
				{
					const Scalar error = valuesRows[n - x].distance(valuesIndividuals[y * sizeX + n]);

					if (error >= Scalar(0.001))
					{
						allSucceeded = false;
					}
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

bool TestLookup2::testCornerLookupClampedValues(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Corner lookup object clamped value test:";

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int sizeX = RandomI::random(1u, 500u);
		const unsigned int sizeY = RandomI::random(1u, 500u);

		const unsigned int binsX = RandomI::random(1u, max(1u, sizeX / 2u));
		const unsigned int binsY = RandomI::random(1u, max(1u, sizeY / 2u));

		LookupCorner2<Scalar> lookupObject(sizeX, sizeY, binsX, binsY);

		for (unsigned int yBin = 0u; yBin <= lookupObject.binsY(); ++yBin)
		{
			for (unsigned int xBin = 0u; xBin <= lookupObject.binsX(); ++xBin)
			{
				lookupObject.setBinTopLeftCornerValue(xBin, yBin, Random::scalar(-100, 100));
			}
		}

		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			bool localSucceeded = true;

			const Scalar x = Random::scalar(-10, Scalar(lookupObject.sizeX()) + 10);
			const Scalar y = Random::scalar(-10, Scalar(lookupObject.sizeY()) + 10);

			if (x >= 0 && y >= 0 && x <= Scalar(lookupObject.sizeX()) && y <= Scalar(lookupObject.sizeY()))
			{
				if (lookupObject.nearestValue(x, y) != lookupObject.clampedNearestValue(x, y))
				{
					localSucceeded = false;
				}

				if (lookupObject.bilinearValue(x, y) != lookupObject.clampedBilinearValue(x, y))
				{
					localSucceeded = false;
				}

				if (lookupObject.bicubicValue(x, y) != lookupObject.clampedBicubicValue(x, y))
				{
					localSucceeded = false;
				}
			}
			else
			{
				Scalar clampedX = x;
				Scalar clampedY = y;

				if (clampedX < 0)
				{
					clampedX = 0;
				}
				if (clampedX > Scalar(lookupObject.sizeX()))
				{
					clampedX = Scalar(lookupObject.sizeX());
				}

				if (clampedY < 0)
				{
					clampedY = 0;
				}
				if (clampedY > Scalar(lookupObject.sizeY()))
				{
					clampedY = Scalar(lookupObject.sizeY());
				}

				if (lookupObject.nearestValue(clampedX, clampedY) != lookupObject.clampedNearestValue(x, y))
				{
					localSucceeded = false;
				}

				if (lookupObject.bilinearValue(clampedX, clampedY) != lookupObject.clampedBilinearValue(x, y))
				{
					localSucceeded = false;
				}

				if (lookupObject.bicubicValue(clampedX, clampedY) != lookupObject.clampedBicubicValue(x, y))
				{
					localSucceeded = false;
				}
			}

			if (localSucceeded)
			{
				validIterations++;
			}

			iterations++;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

}

}

}
