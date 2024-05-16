/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testgeometry/TestSpatialDistribution.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/geometry/SpatialDistribution.h"

#include "ocean/math/Random.h"

#include "ocean/test/Validation.h"
#include "ocean/test/ValidationPrecision.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

bool TestSpatialDistribution::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Spatial distribution test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testIdealBins(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testIdealBinsNeighborhood9(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMinimalSqrDistances(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDistribute(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDistributeAndFilter(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDistributeAndFilterIndices(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testCopyConstructorWithNeighborhood8(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Spatial distribution test succeeded.";
	}
	else
	{
		Log::info() << "Spatial distribution test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestSpatialDistribution, IdealBins)
{
	EXPECT_TRUE(TestSpatialDistribution::testIdealBins(GTEST_TEST_DURATION));
}

TEST(TestSpatialDistribution, IdealBinsNeighborhood9)
{
	EXPECT_TRUE(TestSpatialDistribution::testIdealBinsNeighborhood9(GTEST_TEST_DURATION));
}

TEST(TestSpatialDistribution, MinimalSqrDistances)
{
	EXPECT_TRUE(TestSpatialDistribution::testMinimalSqrDistances(GTEST_TEST_DURATION));
}

TEST(TestSpatialDistribution, Distribute)
{
	EXPECT_TRUE(TestSpatialDistribution::testDistribute(GTEST_TEST_DURATION));
}

TEST(TestSpatialDistribution, DistributeAndFilter)
{
	EXPECT_TRUE(TestSpatialDistribution::testDistributeAndFilter(GTEST_TEST_DURATION));
}

TEST(TestSpatialDistribution, DistributeAndFilterIndices)
{
	EXPECT_TRUE(TestSpatialDistribution::testDistributeAndFilterIndices(GTEST_TEST_DURATION));
}

TEST(TestSpatialDistribution, CopyConstructorWithNeighborhood8)
{
	EXPECT_TRUE(TestSpatialDistribution::testCopyConstructorWithNeighborhood8(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestSpatialDistribution::testIdealBins(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing ideal number of bins:";

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 10u, 1920u * 2u);
		const unsigned int height = RandomI::random(randomGenerator, 10u, 1080u * 2u);

		const size_t numberPoints = RandomI::random(randomGenerator, 1u, 1000u * 1000u);

		const unsigned int minimalHorizontalBins = RandomI::random(randomGenerator, 1u, min(20u, width));
		const unsigned int minimalVerticalBins = RandomI::random(randomGenerator, 1u, min(20u, height));

		unsigned int horizontalBins = (unsigned int)(-1);
		unsigned int verticalBins = (unsigned int)(-1);

		Geometry::SpatialDistribution::idealBins(width, height, numberPoints, horizontalBins, verticalBins, minimalHorizontalBins, minimalVerticalBins);

		ocean_assert(horizontalBins != (unsigned int)(-1));
		ocean_assert(verticalBins != (unsigned int)(-1));

		OCEAN_EXPECT_GREATER_EQUAL(validation, horizontalBins, minimalHorizontalBins);
		OCEAN_EXPECT_LESS_EQUAL(validation, horizontalBins, width);

		OCEAN_EXPECT_GREATER_EQUAL(validation, verticalBins, minimalVerticalBins);
		OCEAN_EXPECT_LESS_EQUAL(validation, verticalBins, height);
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestSpatialDistribution::testIdealBinsNeighborhood9(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing ideal number of bins with 9 neighborhood guarantee:";

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 10u, 1920u * 2u);
		const unsigned int height = RandomI::random(randomGenerator, 10u, 1080u * 2u);

		const Scalar distance = Random::scalar(randomGenerator, Scalar(1), Scalar(2u * max(width, height)));

		const unsigned int minimalHorizontalBins = RandomI::random(randomGenerator, 1u, min(20u, width));
		const unsigned int minimalVerticalBins = RandomI::random(randomGenerator, 1u, min(20u, height));

		const unsigned int maximalHorizontalBins = RandomI::random(randomGenerator, minimalHorizontalBins, width);
		const unsigned int maximalVerticalBins = RandomI::random(randomGenerator, minimalVerticalBins, height);

		ocean_assert(minimalHorizontalBins != 0u && minimalHorizontalBins <= maximalHorizontalBins && maximalHorizontalBins <= width);
		ocean_assert(minimalVerticalBins != 0u && minimalVerticalBins <= maximalVerticalBins && maximalVerticalBins <= height);

		unsigned int horizontalBins = (unsigned int)(-1);
		unsigned int verticalBins = (unsigned int)(-1);

		Geometry::SpatialDistribution::idealBinsNeighborhood9(width, height, distance, horizontalBins, verticalBins, minimalHorizontalBins, minimalVerticalBins, maximalHorizontalBins, maximalVerticalBins);

		ocean_assert(horizontalBins != (unsigned int)(-1));
		ocean_assert(verticalBins != (unsigned int)(-1));

		OCEAN_EXPECT_GREATER_EQUAL(validation, horizontalBins, minimalHorizontalBins);
		OCEAN_EXPECT_LESS_EQUAL(validation, horizontalBins, maximalHorizontalBins);

		OCEAN_EXPECT_GREATER_EQUAL(validation, verticalBins, minimalVerticalBins);
		OCEAN_EXPECT_LESS_EQUAL(validation, verticalBins, maximalVerticalBins);
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestSpatialDistribution::testMinimalSqrDistances(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int numberImagePoints = 50u;
	constexpr unsigned int numberCandidates = 300u;

	Log::info() << "Testing minimal square distances for " << numberImagePoints << " points to " << numberCandidates << " candidates:";

	constexpr unsigned int width = 640u;
	constexpr unsigned int height = 480u;

	HighPerformanceStatistic performanceDistributionCreation;
	HighPerformanceStatistic performanceDistributionSearch;
	HighPerformanceStatistic performanceBruteForce;

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		Vectors2 imagePoints;
		Vectors2 candidates;

		for (unsigned int n = 0u; n < numberImagePoints; ++n)
		{
			const unsigned int x = Random::random(randomGenerator, width - 1u);
			const unsigned int y = Random::random(randomGenerator, height - 1u);

			imagePoints.emplace_back(Scalar(x), Scalar(y));
		}

		for (unsigned int n = 0u; n < numberCandidates; ++n)
		{
			const unsigned int x = Random::random(randomGenerator, width - 1u);
			const unsigned int y = Random::random(randomGenerator, height - 1u);

			candidates.emplace_back(Scalar(x), Scalar(y));
		}

		{
			// first we test the performance

			Scalars minimalSqrDistances(numberImagePoints);

			performanceDistributionCreation.start();
				const Geometry::SpatialDistribution::DistributionArray indexArray(Geometry::SpatialDistribution::distributeToArray(candidates.data(), candidates.size(), Scalar(0), Scalar(0), Scalar(width), Scalar(height), 10u, 10u));
			performanceDistributionCreation.stop();

			performanceDistributionSearch.start();
				Geometry::SpatialDistribution::determineMinimalSqrDistances(imagePoints.data(), imagePoints.size(), candidates.data(), numberCandidates, indexArray, minimalSqrDistances.data());
			performanceDistributionSearch.stop();

			performanceBruteForce.start();

				for (unsigned int n = 0u; n < numberImagePoints; ++n)
				{
					const Vector2& imagePoint = imagePoints[n];

					Scalar minimalSqrDistance = Numeric::maxValue();

					for (unsigned int i = 0u; i < numberCandidates; ++i)
					{
						const Scalar sqrDistance = imagePoint.sqrDistance(candidates[i]);

						if (sqrDistance < minimalSqrDistance)
						{
							minimalSqrDistance = sqrDistance;
						}
					}

					minimalSqrDistances[n] = minimalSqrDistance;
				}

			performanceBruteForce.stop();
		}

		{
			// now we validate the result as the result

			Scalars minimalSqrDistances(numberImagePoints);
			Geometry::SpatialDistribution::determineMinimalSqrDistances(imagePoints.data(), imagePoints.size(), candidates.data(), candidates.size(), width, height, 10u, minimalSqrDistances.data());

			const Geometry::SpatialDistribution::DistributionArray distributionArray(Geometry::SpatialDistribution::distributeToArray(candidates.data(), candidates.size(), Scalar(0), Scalar(0), Scalar(width), Scalar(height), 10u, 10u));

			Scalars testMinimalSqrDistances;
			testMinimalSqrDistances.reserve(numberImagePoints);

			for (unsigned int n = 0u; n < numberImagePoints; ++n)
			{
				const Vector2& imagePoint = imagePoints[n];

				const int binX = distributionArray.horizontalBin(imagePoint.x());
				const int binY = distributionArray.verticalBin(imagePoint.y());

				Scalar testMinSqrDistance = Numeric::maxValue();

				for (unsigned int c = 0u; c < numberCandidates; ++c)
				{
					const Vector2& candidate = candidates[c];

					const int candidateX = distributionArray.horizontalBin(candidate.x());
					const int candidateY = distributionArray.verticalBin(candidate.y());

					if (abs(candidateX - binX) <= 1 && abs(candidateY - binY) <= 1)
					{
						testMinSqrDistance = min(testMinSqrDistance, imagePoint.sqrDistance(candidate));
					}
				}

				testMinimalSqrDistances.push_back(testMinSqrDistance);
			}

			ocean_assert(minimalSqrDistances.size() == testMinimalSqrDistances.size());

			for (unsigned int n = 0u; n < numberImagePoints; ++n)
			{
				if (minimalSqrDistances[n] != Numeric::maxValue())
				{
					OCEAN_EXPECT_EQUAL(validation, minimalSqrDistances[n], testMinimalSqrDistances[n]);
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Brute force performance: " << performanceBruteForce.averageMseconds() << "ms";
	Log::info() << "Distribution performance: " << performanceDistributionCreation.averageMseconds() + performanceDistributionSearch.averageMseconds() << "ms (creation: " << performanceDistributionCreation.averageMseconds() << "ms + search: " << performanceDistributionSearch.averageMseconds() << "ms)";

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestSpatialDistribution::testDistribute(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int numberPoints = 500u;

	Log::info() << "Testing distribute function for " << numberPoints << " points:";

	RandomGenerator randomGenerator;

	constexpr double threshold = std::is_same<float, Scalar>::value ? 0.95 : 0.99;
	ValidationPrecision validation(threshold, randomGenerator);

	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		Vectors2 imagePoints;

		for (unsigned int n = 0u; n < numberPoints; ++n)
		{
			imagePoints.emplace_back(Random::vector2(randomGenerator, Scalar(-100), Scalar(100)));
		}

		const Scalar left = Random::scalar(randomGenerator, -200, 200);
		const Scalar top = Random::scalar(randomGenerator, -200, 200);

		const Scalar width = Random::scalar(randomGenerator, Scalar(0.001), 300);
		const Scalar height = Random::scalar(randomGenerator, Scalar(0.001), 300);

		const unsigned int horizontalBins = RandomI::random(1, max(1, int(width * 5)));
		const unsigned int verticalBins = RandomI::random(1, max(1, int(height * 5)));

		performance.start();
			const Geometry::SpatialDistribution::DistributionArray distribution(Geometry::SpatialDistribution::distributeToArray(imagePoints.data(), imagePoints.size(), left, top, width, height, horizontalBins, verticalBins));
		performance.stop();

		ocean_assert(Numeric::isNotEqualEps(width) && Numeric::isNotEqualEps(height));

		size_t numberDistributed = 0;

		for (size_t n = 0; n < imagePoints.size(); ++n)
		{
			const Vector2& imagePoint = imagePoints[n];

			if (imagePoint.x() >= left && imagePoint.x() <= left + width && imagePoint.y() >= top && imagePoint.y() <= top + height)
			{
				const Scalar floatBinX = (imagePoint.x() - left) * Scalar(horizontalBins) / Scalar(width);
				const Scalar floatBinY = (imagePoint.y() - top) * Scalar(verticalBins) / Scalar(height);

				ocean_assert(floatBinX >= Scalar(0) && floatBinY >= Scalar(0));

				const unsigned int binX = (unsigned int)(floatBinX);
				const unsigned int binY = (unsigned int)(floatBinY);

				if (hasElement(distribution(binX, binY), (unsigned int)(n)))
				{
					++numberDistributed;
				}
				else
				{
					scopedIteration.setInaccurate();
				}
			}
		}

		size_t testNumberDistributed = 0;
		for (unsigned int n = 0u; n < distribution.bins(); ++n)
		{
			testNumberDistributed += distribution[n].size();
		}

		if (numberDistributed != testNumberDistributed)
		{
			scopedIteration.setInaccurate();
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance: " << performance;
	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestSpatialDistribution::testDistributeAndFilter(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int numberPoints = 500u;

	Log::info() << "Testing distribute and filter function for " << numberPoints << " points:";

	HighPerformanceStatistic performance;

	RandomGenerator randomGenerator;

	ValidationPrecision validation(0.99, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		Vectors2 imagePoints;

		for (unsigned int n = 0u; n < numberPoints; ++n)
		{
			imagePoints.emplace_back(Random::vector2(randomGenerator, -100, 100));
		}

		const Scalar left = Random::scalar(randomGenerator, -200, 200);
		const Scalar top = Random::scalar(randomGenerator, -200, 200);

		const Scalar width = Random::scalar(randomGenerator, Scalar(0.001), 300);
		const Scalar height = Random::scalar(randomGenerator, Scalar(0.001), 300);

		const unsigned int horizontalBins = RandomI::random(randomGenerator, 1, max(1, int(width * 5)));
		const unsigned int verticalBins = RandomI::random(randomGenerator, 1, max(1, int(height * 5)));

		performance.start();
			const Vectors2 filteredImagePoints = Geometry::SpatialDistribution::distributeAndFilter(imagePoints.data(), imagePoints.size(), left, top, width, height, horizontalBins, verticalBins);
		performance.stop();

		const std::unordered_set<Vector2, Vector2> filteredImagePointsSet(filteredImagePoints.cbegin(), filteredImagePoints.cend());

		Indices32 occupiedBins(horizontalBins * verticalBins, 0u);

		ocean_assert(Numeric::isNotEqualEps(width) && Numeric::isNotEqualEps(height));
		const Scalar invWidth = Scalar(1) / width;
		const Scalar invHeight = Scalar(1) / height;

		size_t numberFiltered = 0;

		for (size_t n = 0; n < imagePoints.size(); ++n)
		{
			const Vector2& imagePoint = imagePoints[n];

			if (imagePoint.x() >= left && imagePoint.x() <= left + width && imagePoint.y() >= top && imagePoint.y() <= top + height)
			{
				const Scalar floatBinX = (imagePoint.x() - left) * Scalar(horizontalBins) * invWidth;
				const Scalar floatBinY = (imagePoint.y() - top) * Scalar(verticalBins) * invHeight;

				ocean_assert(floatBinX >= 0 && floatBinY >= 0);

				const unsigned int binX = (unsigned int)floatBinX;
				const unsigned int binY = (unsigned int)floatBinY;

				if (occupiedBins[binY * horizontalBins + binX] == 0u)
				{
					occupiedBins[binY * horizontalBins + binX] = 1u;

					if (filteredImagePointsSet.find(imagePoint) != filteredImagePointsSet.cend())
					{
						++numberFiltered;
					}
					else
					{
						scopedIteration.setInaccurate();
					}
				}
			}
		}

		if (filteredImagePoints.size() != numberFiltered)
		{
			scopedIteration.setInaccurate();
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance: " << performance;
	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestSpatialDistribution::testDistributeAndFilterIndices(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int numberPoints = 500u;

	Log::info() << "Testing distribute and filter indices function for " << numberPoints << " points:";

	HighPerformanceStatistic performance;

	RandomGenerator randomGenerator;

	ValidationPrecision validation(0.99, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		Vectors2 imagePoints;

		for (unsigned int n = 0u; n < numberPoints; ++n)
		{
			imagePoints.emplace_back(Random::vector2(randomGenerator, -100, 100));
		}

		const Scalar left = Random::scalar(randomGenerator, -200, 200);
		const Scalar top = Random::scalar(randomGenerator, -200, 200);

		const Scalar width = Random::scalar(randomGenerator, Scalar(0.001), 300);
		const Scalar height = Random::scalar(randomGenerator, Scalar(0.001), 300);

		const unsigned int horizontalBins = RandomI::random(randomGenerator, 1, max(1, int(width * 5)));
		const unsigned int verticalBins = RandomI::random(randomGenerator, 1, max(1, int(height * 5)));

		performance.start();
			const Indices32 filteredIndices = Geometry::SpatialDistribution::distributeAndFilterIndices<Index32>(imagePoints.data(), imagePoints.size(), left, top, width, height, horizontalBins, verticalBins);
		performance.stop();

		UnorderedIndexSet64 filteredIndicesSet(filteredIndices.cbegin(), filteredIndices.cend());

		Indices32 occupiedBins(horizontalBins * verticalBins, 0u);

		size_t numberFiltered = 0;

		for (size_t n = 0; n < imagePoints.size(); ++n)
		{
			const Vector2& imagePoint = imagePoints[n];

			if (imagePoint.x() >= left && imagePoint.x() <= left + width && imagePoint.y() >= top && imagePoint.y() <= top + height)
			{
				ocean_assert(Numeric::isNotEqualEps(width) && Numeric::isNotEqualEps(height));

				const Scalar floatBinX = (imagePoint.x() - left) * Scalar(horizontalBins) / Scalar(width);
				const Scalar floatBinY = (imagePoint.y() - top) * Scalar(verticalBins) / Scalar(height);

				ocean_assert(floatBinX >= 0 && floatBinY >= 0);

				const unsigned int binX = (unsigned int)floatBinX;
				const unsigned int binY = (unsigned int)floatBinY;

				if (occupiedBins[binY * horizontalBins + binX] == 0u)
				{
					occupiedBins[binY * horizontalBins + binX] = 1u;

					if (filteredIndicesSet.find(Index32(n)) != filteredIndicesSet.cend())
					{
						++numberFiltered;
					}
					else
					{
						scopedIteration.setInaccurate();
					}
				}
			}
		}

		if (filteredIndices.size() != numberFiltered)
		{
			scopedIteration.setInaccurate();
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance: " << performance;
	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestSpatialDistribution::testCopyConstructorWithNeighborhood8(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing copy constructor with 8-neighborhood:";

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int elements = RandomI::random(randomGenerator, 1u, 5000u);

		const Scalar left = Random::scalar(randomGenerator, -200, 200);
		const Scalar top = Random::scalar(randomGenerator, -200, 200);

		const Scalar width = Random::scalar(randomGenerator, 1, 300);
		const Scalar height = Random::scalar(randomGenerator, 1, 300);

		const unsigned int horizontalBins = RandomI::random(randomGenerator, 1u, 200u);
		const unsigned int verticalBins = RandomI::random(randomGenerator, 1u, 200u);

		Geometry::SpatialDistribution::DistributionArray distributionArray(left, top, width, height, horizontalBins, verticalBins);

		for (unsigned int n = 0u; n < elements; ++n)
		{
			const unsigned int binX = RandomI::random(0u, horizontalBins - 1u);
			const unsigned int binY = RandomI::random(0u, verticalBins - 1u);

			distributionArray(binX, binY).push_back(n);
		}

		const Geometry::SpatialDistribution::DistributionArray distributionArray8(distributionArray, true);

		for (unsigned int verticalBin = 0u; verticalBin < verticalBins; ++verticalBin)
		{
			for (unsigned int horizontalBin = 0u; horizontalBin < horizontalBins; ++horizontalBin)
			{
				const Indices32& indices = distributionArray8(horizontalBin, verticalBin);

				for (const Index32& index : indices)
				{
					bool indexFound = false;

					size_t neighborhoodIndices = 0;

					for (int yBin = max(0, int(verticalBin) - 1); yBin < min(int(verticalBin) + 2, int(verticalBins)); ++yBin)
					{
						for (int xBin = max(0, int(horizontalBin) - 1); xBin < min(int(horizontalBin) + 2, int(horizontalBins)); ++xBin)
						{
							const Indices32& localIndices = distributionArray(xBin, yBin);

							neighborhoodIndices += localIndices.size();

							for (const Index32& localIndex : localIndices)
							{
								if (localIndex == index)
								{
									// the index must not exist in two individual bins (in the original distribution array)
									OCEAN_EXPECT_FALSE(validation, indexFound);

									indexFound = true;
								}
							}
						}
					}

					OCEAN_EXPECT_TRUE(validation, indexFound);

					OCEAN_EXPECT_EQUAL(validation, neighborhoodIndices, indices.size());
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
