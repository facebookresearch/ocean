/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testgeometry/TestOctree.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/geometry/Octree.h"

#include "ocean/math/Random.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/Validation.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

bool TestOctree::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0);

	TestResult testResult("Octree test");

	if (selector.shouldRun("constructor"))
	{
		testResult = testConstructor(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("closestpoints"))
	{
		testResult = testClosestPoints(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("intersectingleavesforrays"))
	{
		testResult = testIntersectingLeavesForRays(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("edgecases"))
	{
		testResult = testEdgeCases(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestOctree, Constructor)
{
	EXPECT_TRUE(TestOctree::testConstructor(GTEST_TEST_DURATION));
}

TEST(TestOctree, ClosestPoints)
{
	EXPECT_TRUE(TestOctree::testClosestPoints(GTEST_TEST_DURATION));
}

TEST(TestOctree, IntersectingLeavesForRays)
{
	EXPECT_TRUE(TestOctree::testIntersectingLeavesForRays(GTEST_TEST_DURATION));
}

TEST(TestOctree, EdgeCases)
{
	EXPECT_TRUE(TestOctree::testEdgeCases(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestOctree::testConstructor(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

#ifdef OCEAN_DEBUG
	constexpr unsigned int benchmarkPointNumber = 50000u;
#else
	constexpr unsigned int benchmarkPointNumber = 500000u;
#endif

	Log::info() << "Test Constructor with " << benchmarkPointNumber << " points:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	HighPerformanceStatistic performanceBisectedBoundingBoxes;
	HighPerformanceStatistic performanceTightBoundingBoxes;

	const Timestamp startTimestamp(true);

	do
	{
		for (const bool benchmarkIteration : {false, true})
		{
			const unsigned int numberPoints = benchmarkIteration ? benchmarkPointNumber : RandomI::random(randomGenerator, 100000u) + 1000u;

			Vectors3 points(numberPoints);

			for (Vector3& point : points)
			{
				point = Random::vector3(randomGenerator) * Random::scalar(randomGenerator, -1000, 1000);
			}

			for (const bool useTightBoundingBoxes : {false, true})
			{
				const Geometry::Octree::Parameters parameters(40u, useTightBoundingBoxes);

				HighPerformanceStatistic& performance = useTightBoundingBoxes ? performanceTightBoundingBoxes : performanceBisectedBoundingBoxes;

				performance.startIf(benchmarkIteration);
					const Geometry::Octree octree(points.data(), points.size());
				performance.stopIf(benchmarkIteration);

				UnorderedIndexSet32 pointIndexSet;
				pointIndexSet.reserve(numberPoints);

				std::vector<const Geometry::Octree*> nodes(1, &octree);

				while (!nodes.empty())
				{
					const Geometry::Octree* node = nodes.back();
					nodes.pop_back();

					bool isLeafNode = true;

					if (node->childNodes() != nullptr)
					{
						isLeafNode = false;

						for (size_t n = 0; n < 8; ++n)
						{
							if (node->childNodes()[n] != nullptr)
							{
								nodes.emplace_back(node->childNodes()[n]);
							}
						}
					}

					if (isLeafNode)
					{
						for (const Index32& index : node->pointIndices())
						{
							if (pointIndexSet.find(index) != pointIndexSet.cend())
							{
								OCEAN_SET_FAILED(validation);
							}

							pointIndexSet.emplace(index);
						}
					}
					else
					{
						// non-leaf nodes must not contain indices

						if (!node->pointIndices().empty())
						{
							OCEAN_SET_FAILED(validation);
						}
					}
				}

				if (pointIndexSet.size() != numberPoints)
				{
					// not all descriptors are represented in the tree
					OCEAN_SET_FAILED(validation);
				}
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	ocean_assert(performanceBisectedBoundingBoxes.measurements() >= 1u);
	ocean_assert(performanceTightBoundingBoxes.measurements() >= 1u);

	Log::info() << "Performance bisected bounding boxes: " << performanceBisectedBoundingBoxes;
	Log::info() << "Performance tight bounding boxes: " << performanceTightBoundingBoxes;

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestOctree::testClosestPoints(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

#ifdef OCEAN_DEBUG
	constexpr unsigned int benchmarkTreePointNumber = 50000u;
	constexpr unsigned int benchmarkQueryPointNumber = 100u;
#else
	constexpr unsigned int benchmarkTreePointNumber = 500000u;
	constexpr unsigned int benchmarkQueryPointNumber = 1000u;
#endif

	Log::info() << "Test closestPoints() with " << benchmarkTreePointNumber << " tree points, and " << benchmarkQueryPointNumber << " query points:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	HighPerformanceStatistic performanceBruteForce;
	HighPerformanceStatistic performanceTightBoundingBoxes;
	HighPerformanceStatistic performanceBisectedBoundingBoxes;

	const Timestamp startTimestamp(true);

	do
	{
		for (const bool benchmarkIteration : {false, true})
		{
			const unsigned int numberTreePoints = benchmarkIteration ? benchmarkTreePointNumber : RandomI::random(randomGenerator, 100000u) + 1u;
			const unsigned int numberQueryPoints = benchmarkIteration ? benchmarkQueryPointNumber : RandomI::random(randomGenerator, 1u, benchmarkQueryPointNumber);

			Vectors3 treePoints(numberTreePoints);
			for (Vector3& treePoint : treePoints)
			{
				treePoint = Random::vector3(randomGenerator) * Random::scalar(randomGenerator, -1000, 1000);
			}

			Vectors3 queryPoints(numberQueryPoints);
			for (Vector3& queryPoint : queryPoints)
			{
				queryPoint = Random::vector3(randomGenerator) * Random::scalar(randomGenerator, -1000, 1000);
			}

			const Scalar maximalDistance = benchmarkIteration ? Scalar(5) : Random::scalar(randomGenerator, Scalar(0.1), 20);

			std::vector<std::pair<Index32, bool>> bruteForceResult(numberQueryPoints, std::make_pair(Index32(-1), false));

			performanceBruteForce.startIf(benchmarkIteration);
				for (unsigned int nQuery = 0u; nQuery < numberQueryPoints; ++nQuery)
				{
					const Vector3& queryPoint = queryPoints[nQuery];

					Index32 bestTreeIndex = Index32(-1);
					Scalar bestSqrDistance = Numeric::maxValue();

					bool severalIdenticalCandidates = false;

					for (unsigned int nTree = 0u; nTree < numberTreePoints; ++nTree)
					{
						const Scalar sqrDistance = queryPoint.sqrDistance(treePoints[nTree]);

						if (sqrDistance < bestSqrDistance)
						{
							bestSqrDistance = sqrDistance;
							bestTreeIndex = nTree;

							severalIdenticalCandidates = false;
						}
						else if (sqrDistance == bestSqrDistance)
						{
							severalIdenticalCandidates = true;
						}
					}

					if (bestSqrDistance <= maximalDistance * maximalDistance)
					{
						bruteForceResult[nQuery] = std::make_pair(bestTreeIndex, severalIdenticalCandidates);
					}
				}
			performanceBruteForce.stopIf(benchmarkIteration);

			for (const bool useTightBoundingBoxes : {false, true})
			{
				const Geometry::Octree::Parameters parameters(40u, useTightBoundingBoxes);

				HighPerformanceStatistic& performance = useTightBoundingBoxes ? performanceTightBoundingBoxes : performanceBisectedBoundingBoxes;

				const Geometry::Octree octree(treePoints.data(), treePoints.size(), parameters);

				Indices32 octreeResult(numberQueryPoints, Index32(-1));

				performance.startIf(benchmarkIteration);

					Indices32 pointIndices;
					pointIndices.reserve(32);

					Geometry::Octree::ReusableData reusableData;

					for (unsigned int nQuery = 0u; nQuery < numberQueryPoints; ++nQuery)
					{
						const Vector3& queryPoint = queryPoints[nQuery];

						pointIndices.clear();
						octree.closestPoints(treePoints.data(), queryPoint, maximalDistance, pointIndices, nullptr, reusableData);

						Index32 bestTreeIndex = Index32(-1);
						Scalar bestDistance = Numeric::maxValue();

						for (const Index32& treeIndex : pointIndices)
						{
							const Scalar distance = queryPoint.sqrDistance(treePoints[treeIndex]);

							if (distance < bestDistance)
							{
								bestDistance = distance;
								bestTreeIndex = treeIndex;
							}
						}

						octreeResult[nQuery] = bestTreeIndex;
					}
				performance.stopIf(benchmarkIteration);

				for (unsigned int nQuery = 0u; nQuery < numberQueryPoints; ++nQuery)
				{
					if (bruteForceResult[nQuery].second == false)
					{
						if (bruteForceResult[nQuery].first != octreeResult[nQuery])
						{
							OCEAN_SET_FAILED(validation);
						}
					}
				}
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	ocean_assert(performanceBruteForce.measurements() >= 1u);
	ocean_assert(performanceBisectedBoundingBoxes.measurements() >= 1u);
	ocean_assert(performanceTightBoundingBoxes.measurements() >= 1u);

	Log::info() << "Performance brute-force: " << performanceBruteForce;
	Log::info() << "Performance bisected bounding boxes: " << performanceBisectedBoundingBoxes;
	Log::info() << "Performance tight bounding boxes: " << performanceTightBoundingBoxes;

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestOctree::testIntersectingLeavesForRays(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

#ifdef OCEAN_DEBUG
	constexpr unsigned int benchmarkTreePointNumber = 50000u;
	constexpr unsigned int benchamrkQueryRaysNumber = 100u;
#else
	constexpr unsigned int benchmarkTreePointNumber = 500000u;
	constexpr unsigned int benchamrkQueryRaysNumber = 1000u;
#endif

	Log::info() << "Test intersectingLeaves() for rays with " << benchmarkTreePointNumber << " tree points, and " << benchamrkQueryRaysNumber << " query rays:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	HighPerformanceStatistic performanceBruteForce;
	HighPerformanceStatistic performanceBisectedBoundingBoxes;
	HighPerformanceStatistic performanceTightBoundingBoxes;

	const Timestamp startTimestamp(true);

	do
	{
		for (const bool benchmarkIteration : {false, true})
		{
			const unsigned int numberTreePoints = benchmarkIteration ? benchmarkTreePointNumber : RandomI::random(randomGenerator, 100000u) + 1u;
			const unsigned int numberQueryRays = benchmarkIteration ? benchamrkQueryRaysNumber : RandomI::random(randomGenerator, 1u, benchamrkQueryRaysNumber);

			Vectors3 treePoints(numberTreePoints);
			for (Vector3& treePoint : treePoints)
			{
				treePoint = Random::vector3(randomGenerator) * Random::scalar(randomGenerator, -1000, 1000);
			}

			Lines3 queryRays;
			queryRays.reserve(numberQueryRays);

			while (queryRays.size() < numberQueryRays)
			{
				const Vector3& treePoint = treePoints[RandomI::random(randomGenerator, numberTreePoints - 1u)];

				queryRays.emplace_back(treePoint, Random::vector3(randomGenerator));
			}

			const Scalar maximalDistance = benchmarkIteration ? Scalar(5) : Random::scalar(randomGenerator, Scalar(0.1), 20);

			std::vector<std::pair<Index32, bool>> bruteForceResult(numberQueryRays, std::make_pair(Index32(-1), false));

			performanceBruteForce.startIf(benchmarkIteration);
				for (unsigned int nQuery = 0u; nQuery < numberQueryRays; ++nQuery)
				{
					const Line3& queryRay = queryRays[nQuery];

					Index32 bestTreeIndex = Index32(-1);
					Scalar bestSqrDistance = Numeric::maxValue();

					bool severalIdenticalCandidates = false;

					for (unsigned int nTree = 0u; nTree < numberTreePoints; ++nTree)
					{
						const Scalar sqrDistance = queryRay.sqrDistance(treePoints[nTree]);

						if (sqrDistance < bestSqrDistance)
						{
							bestSqrDistance = sqrDistance;
							bestTreeIndex = nTree;

							severalIdenticalCandidates = false;
						}
						else if (sqrDistance == bestSqrDistance)
						{
							severalIdenticalCandidates = true;
						}
					}

					if (bestSqrDistance <= maximalDistance * maximalDistance)
					{
						bruteForceResult[nQuery] = std::make_pair(bestTreeIndex, severalIdenticalCandidates);
					}
				}
			performanceBruteForce.stopIf(benchmarkIteration);

			for (const bool useTightBoundingBoxes : {false, true})
			{
				const Geometry::Octree::Parameters parameters(40u, useTightBoundingBoxes);

				HighPerformanceStatistic& performance = useTightBoundingBoxes ? performanceTightBoundingBoxes : performanceBisectedBoundingBoxes;

				const Geometry::Octree octree(treePoints.data(), treePoints.size(), parameters);

				Indices32 octreeResult(numberQueryRays, Index32(-1));

				performance.startIf(benchmarkIteration);

					std::vector<const Indices32*> leaves;
					leaves.reserve(32);

					Geometry::Octree::ReusableData reusableData;

					for (unsigned int nQuery = 0u; nQuery < numberQueryRays; ++nQuery)
					{
						const Line3& queryRay = queryRays[nQuery];

						leaves.clear();
						octree.intersectingLeaves(queryRay, leaves);

						Index32 bestTreeIndex = Index32(-1);
						Scalar bestSqrDistance = Numeric::maxValue();

						for (const Indices32* leaf : leaves)
						{
							for (const Index32& treeIndex : *leaf)
							{
								const Scalar sqrDistance = queryRay.sqrDistance(treePoints[treeIndex]);

								if (sqrDistance < bestSqrDistance)
								{
									bestSqrDistance = sqrDistance;
									bestTreeIndex = treeIndex;
								}
							}
						}

						octreeResult[nQuery] = bestTreeIndex;
					}
				performance.stopIf(benchmarkIteration);

				for (unsigned int nQuery = 0u; nQuery < numberQueryRays; ++nQuery)
				{
					if (bruteForceResult[nQuery].second == false)
					{
						if (bruteForceResult[nQuery].first != octreeResult[nQuery])
						{
							OCEAN_SET_FAILED(validation);
						}
					}
				}
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	ocean_assert(performanceBruteForce.measurements() >= 1u);
	ocean_assert(performanceBisectedBoundingBoxes.measurements() >= 1u);
	ocean_assert(performanceTightBoundingBoxes.measurements() >= 1u);

	Log::info() << "Performance brute-force: " << performanceBruteForce;
	Log::info() << "Performance bisected bounding boxes: " << performanceBisectedBoundingBoxes;
	Log::info() << "Performance tight bounding boxes: " << performanceTightBoundingBoxes;

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestOctree::testEdgeCases(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test edge cases:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int maximalPointsPerLeaf = RandomI::random(randomGenerator, 1u, 100u);
		const unsigned int numberGroupsOfPoints = RandomI::random(randomGenerator, 1u, 10u);
		const unsigned int numberPointsPerGroup = RandomI::random(randomGenerator, maximalPointsPerLeaf, 1000u);

		Vectors3 groupPoints;

		for (unsigned int n = 0u; n < numberGroupsOfPoints; ++n)
		{
			groupPoints.emplace_back(Random::vector3(randomGenerator, Scalar(-1000), Scalar(1000)));
		}

		Vectors3 points;

		for (const Vector3& groupPoint : groupPoints)
		{
			points.insert(points.end(), numberPointsPerGroup, groupPoint);
		}

		OCEAN_EXPECT_EQUAL(validation, points.size(), size_t(numberGroupsOfPoints * numberPointsPerGroup));

		const bool useTightBoundingBoxes = RandomI::boolean(randomGenerator);

		const Geometry::Octree octree(points.data(), points.size(), Geometry::Octree::Parameters(maximalPointsPerLeaf, useTightBoundingBoxes));

		for (const Vector3& groupPoint : groupPoints)
		{
			Indices32 pointIndices;

			octree.closestPoints(points.data(), groupPoint, 1.0, pointIndices);

			OCEAN_EXPECT_GREATER_EQUAL(validation, pointIndices.size(), size_t(numberPointsPerGroup));

			for (const Index32& pointIndex : pointIndices)
			{
				if (pointIndex < points.size())
				{
					const Vector3& point = points[pointIndex];

					const Scalar distance = groupPoint.distance(point);

					OCEAN_EXPECT_LESS_EQUAL(validation, distance, Scalar(1.01));
				}
				else
				{
					OCEAN_SET_FAILED(validation);
				}
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
