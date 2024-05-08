/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestKdTree.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/KdTree.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/Utilities.h"
#include "ocean/math/Numeric.h"
#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

bool TestKdTree::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Kd tree tests:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testNearestNeighborInteger<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testNearestNeighborInteger<float>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testRadiusSearchInteger<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testRadiusSearchInteger<float>(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
		Log::info() << "Kd tree test succeeded.";
	else
		Log::info() << "Kd tree test FAILED!";

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestKdTree, NearestNeighborInteger_Double) {
	EXPECT_TRUE(TestKdTree::testNearestNeighborInteger<double>(GTEST_TEST_DURATION));
}

TEST(TestKdTree, NearestNeighborInteger_Float) {
	EXPECT_TRUE(TestKdTree::testNearestNeighborInteger<float>(GTEST_TEST_DURATION));
}

TEST(TestKdTree, RadiusSearchInteger_Double) {
	EXPECT_TRUE(TestKdTree::testRadiusSearchInteger<double>(GTEST_TEST_DURATION));
}

TEST(TestKdTree, RadiusSearchInteger_Float) {
	EXPECT_TRUE(TestKdTree::testRadiusSearchInteger<float>(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

template<typename T>
bool TestKdTree::testNearestNeighborInteger(const double testDuration)
{
	static_assert(std::is_same<float, T>::value || std::is_same<double, T>::value, "T must be float or double");
	ocean_assert(testDuration > 0.0);

	Log::info() << "Nearest neighbor test:";
	Log::info() << " ";

#if defined(OCEAN_USE_GTEST)
	const std::vector<unsigned int> elements = {10u, 50u};
	const std::vector<unsigned int> dimensions = {2u, 3u, 5u};
#else
	const std::vector<unsigned int> elements = {10u, 100u, 1000u, 100000u};
	const std::vector<unsigned int> dimensions = {2u, 3u, 5u, 10u, 32u, 64u};
#endif

	bool allSucceeded = true;

	for (unsigned int e = 0u; e < static_cast<unsigned int>(elements.size()); ++e)
	{
		if (e != 0u)
		{
			Log::info() << " ";
			Log::info() << " ";
		}

		for (unsigned int d = 0u; d < static_cast<unsigned int>(dimensions.size()); ++d)
		{
			Log::info().newLine(d != 0u);
			allSucceeded = testNearestNeighborInteger<T>(elements[e], dimensions[d], testDuration) && allSucceeded;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED!";

	return allSucceeded;
}

template<typename T>
bool TestKdTree::testNearestNeighborInteger(const unsigned int number, const unsigned int dimension, const double testDuration)
{
	static_assert(std::is_same<float, T>::value || std::is_same<double, T>::value, "T must be float or double");
	ocean_assert(testDuration > 0.0);

	const unsigned int iterations = 100u;

	Log::info() << "... with " << String::insertCharacter(String::toAString(number), ',', 3, false) << " elements and " << dimension << " dimensions, (" << iterations << " times):";

	RandomGenerator randomGenerator;

	std::vector<T> elements;

	for (unsigned int n = 0u; n < number; ++n)
	{
		for (unsigned int d = 0u; d < dimension; ++d)
			elements.push_back(RandomT<T>::scalar(randomGenerator, T(-1.0), T(1.0)));
	}

	std::vector<const T*> pointers;
	for (unsigned int n = 0u; n < number; ++n)
		pointers.push_back(elements.data() + n * dimension);

	bool allSucceeded = true;
	std::vector<std::vector<T>> randomsSet(iterations, std::vector<T>(dimension));

	T dummyValue = 0.0;

	HighPerformanceStatistic performance, performanceNearestNeighbor, ssdPerformanceNearestNeighbor;
	Timestamp startTimestamp(true);

	do
	{
		KdTree<T> kdTree(dimension);

		performance.start();
			kdTree.insert(pointers.data(), pointers.size());
		performance.stop();

		for (size_t i = 0; i < randomsSet.size(); ++i)
		{
			for (unsigned int n = 0u; n < dimension; ++n)
			{
				randomsSet[i][n] = RandomT<T>::scalar(randomGenerator, T(-1.0), T(1.0));
			}
		}

		performanceNearestNeighbor.start();

		for (size_t i = 0; i < randomsSet.size(); ++i)
		{
			T dummyDistance = -1; // necessary to ensure that the optimizer does not skip the code
			if (kdTree.nearestNeighbor(randomsSet[i].data(), dummyDistance) == nullptr)
			{
				dummyValue += 0.01f;
			}
		}

		performanceNearestNeighbor.stop();

		ssdPerformanceNearestNeighbor.start();

		for (size_t i = 0; i < randomsSet.size(); ++i)
		{
			T ssdBest = NumericT<T>::maxValue();
			const T* valueBest = nullptr;

			for (unsigned int n = 0u; n < number; ++n)
			{
				T ssd = 0;

				for (unsigned int d = 0u; d < dimension; ++d)
				{
					ssd += sqr(pointers[n][d] - randomsSet[i][d]);
				}

				if (ssd < ssdBest)
				{
					ssdBest = ssd;
					valueBest = pointers[n];
				}
			}

			dummyValue += valueBest[0];
		}

		ssdPerformanceNearestNeighbor.stop();
	}
	while (startTimestamp + testDuration > Timestamp(true));

	startTimestamp.toNow();

	do
	{
		// validation

		std::vector<T> randoms(dimension);

		KdTree<T> kdTree(dimension);
		kdTree.insert(pointers.data(), pointers.size());

		// first we validate that nearest distance function for node values (the distance to existing nodes must be zero)
		for (unsigned int n = 0; n < number; ++n)
		{
			const T* value = pointers[n];

			T distance = -1;
			const T* nearest = kdTree.nearestNeighbor(value, distance);

			ocean_assert(nearest);

			if (distance != 0)
			{
				allSucceeded = false;
			}

			for (unsigned int d = 0u; d < dimension; ++d)
			{
				if (nearest[d] != value[d])
				{
					allSucceeded = false;
				}
			}
		}

		// now we validate the nearest distance function for random values
		for (unsigned int i = 0u; i < 1000u; ++i)
		{
			for (unsigned int d = 0u; d < dimension; ++d)
			{
				randoms[d] = RandomT<T>::scalar(randomGenerator, T(-1.0), T(1.0));
			}

			const T* value = randoms.data();

			T distance = NumericT<T>::maxValue();
			const T* nearest = kdTree.nearestNeighbor(value, distance);
			ocean_assert(nearest);

			T ssdBest = NumericT<T>::maxValue();
			const T* valueBest = nullptr;

			for (unsigned int n = 0u; n < number; ++n)
			{
				T ssd = 0;

				for (unsigned int d = 0u; d < dimension; ++d)
				{
					ssd += sqr(pointers[n][d] - value[d]);
				}

				if (ssd < ssdBest)
				{
					ssdBest = ssd;
					valueBest = pointers[n];
				}
			}

			if (ssdBest != distance)
			{
				if (std::fabs(ssdBest - distance) > 1e-12f)
				{
					allSucceeded = false;
				}
			}

			for (unsigned int d = 0u; d < dimension; ++d)
			{
				if (nearest[d] != valueBest[d])
				{
					// there may be more than one nearest value (so we have to ensure that both distances are identical)

					T ssdTest = 0;

					for (unsigned int innerD = 0u; innerD < dimension; ++innerD)
					{
						ssdTest += sqr(nearest[innerD] - value[innerD]);
					}

					if (std::fabs(ssdTest - ssdBest) > 1e-12f)
					{
						allSucceeded = false;
					}

					break;
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (dummyValue >= 0.0)
	{
		Log::info() << "Create performance: Best: " << performance.bestMseconds() << "ms, worst: " << performance.worstMseconds() << "ms, average: " << performance.averageMseconds() << "ms";
	}
	else
	{
		Log::info() << "Create performance: Best: " << performance.bestMseconds() << "ms, worst: " << performance.worstMseconds() << "ms, average: " << performance.averageMseconds() << "ms";
	}

	Log::info() << "KD Nearest Neighbor performance: Best: " << performanceNearestNeighbor.bestMseconds() << "ms, worst: " << performanceNearestNeighbor.worstMseconds() << "ms, average: " << performanceNearestNeighbor.averageMseconds() << "ms";
	Log::info() << "Brute Force performance: Best: " << ssdPerformanceNearestNeighbor.bestMseconds() << "ms, worst: " << ssdPerformanceNearestNeighbor.worstMseconds() << "ms, average: " << ssdPerformanceNearestNeighbor.averageMseconds() << "ms";

	if (performanceNearestNeighbor.averageMseconds() > 0)
	{
		Log::info() << "KD boost factor: Average: " << String::toAString(ssdPerformanceNearestNeighbor.averageMseconds() / performanceNearestNeighbor.averageMseconds(), 2u) << "x";
	}

	return allSucceeded;
}

template<typename T>
bool TestKdTree::testRadiusSearchInteger(const double testDuration)
{
	static_assert(std::is_same<float, T>::value || std::is_same<double, T>::value, "T must be float or double");
	ocean_assert(testDuration > 0.0);

	Log::info() << "Radius search test:";
	Log::info() << " ";

#if defined(OCEAN_USE_GTEST)
	const std::vector<unsigned int> elements = {10u, 50u};
	const std::vector<unsigned int> dimensions = {2u, 3u, 5u};
#else
	const std::vector<unsigned int> elements = {10u, 100u, 1000u, 100000u};
	const std::vector<unsigned int> dimensions = {2u, 3u, 5u, 10u, 32u, 64u};
#endif

	bool allSucceeded = true;

	for (unsigned int e = 0u; e < static_cast<unsigned int>(elements.size()); ++e)
	{
		if (e != 0u)
		{
			Log::info() << " ";
			Log::info() << " ";
		}

		for (unsigned int d = 0u; d < static_cast<unsigned int>(dimensions.size()); ++d)
		{
			Log::info().newLine(d != 0u);
			allSucceeded = testRadiusSearchInteger<T>(elements[e], dimensions[d], testDuration) && allSucceeded;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED!";

	return allSucceeded;
}

template <typename T>
bool TestKdTree::testRadiusSearchInteger(const unsigned int number, const unsigned int dimension, const double testDuration)
{
	static_assert(std::is_same<float, T>::value || std::is_same<double, T>::value, "T must be float or double");
	ocean_assert(testDuration > 0.0);

	const unsigned int iterations = 100u;

	Log::info() << "... with " << String::insertCharacter(String::toAString(number), ',', 3, false) << " elements and " << dimension << " dimensions, (" << iterations << " times):";

	RandomGenerator randomGenerator;

	std::vector<T> elements;

	for (unsigned int n = 0u; n < number; ++n)
	{
		for (unsigned int d = 0u; d < dimension; ++d)
			elements.push_back(RandomT<T>::scalar(randomGenerator, T(-1.0), T(1.0)));
	}

	std::vector<const T*> pointers;
	for (unsigned int n = 0u; n < number; ++n)
		pointers.push_back(elements.data() + n * dimension);

	bool allSucceeded = true;
	std::vector<std::vector<T>> randomsSet(iterations, std::vector<T>(dimension));

	T dummyValue = 0.0;

	HighPerformanceStatistic performance, performanceRadiusSearch, ssdPerformanceRadiusSearch;
	Timestamp startTimestamp(true);

	std::vector<const T *> neighbors(number, nullptr);

	do
	{
		KdTree<T> kdTree(dimension);

		performance.start();
		kdTree.insert(pointers.data(), pointers.size());
		performance.stop();

		for (size_t i = 0; i < randomsSet.size(); ++i)
			for (unsigned int n = 0u; n < dimension; ++n)
				randomsSet[i][n] = RandomT<T>::scalar(randomGenerator, T(-1.0), T(1.0));

		// Performance boost is greater when fewer items returned, so use small radius
		const T radius = T(0.01);

		performanceRadiusSearch.start();

		for (size_t i = 0; i < randomsSet.size(); ++i)
		{
			const size_t foundNeighbors = kdTree.radiusSearch(randomsSet[i].data(), radius, neighbors.data(), neighbors.size());
			dummyValue += T(0.01f) * T(foundNeighbors);
		}

		performanceRadiusSearch.stop();

		ssdPerformanceRadiusSearch.start();

		for (size_t i = 0; i < randomsSet.size(); ++i)
		{
			unsigned int foundNeighbors = 0;

			for (unsigned int n = 0u; n < number; ++n)
			{
				T ssd = 0;

				for (unsigned int d = 0u; d < dimension; ++d)
					ssd += sqr(pointers[n][d] - randomsSet[i][d]);

				if (ssd <= radius)
					neighbors[foundNeighbors ++] = pointers[n];
			}

			dummyValue += T(0.01f) * T(foundNeighbors);
		}

		ssdPerformanceRadiusSearch.stop();
	}
	while (startTimestamp + testDuration > Timestamp(true));

	startTimestamp.toNow();

	do
	{
		// validation

		std::vector<T> randoms(dimension);

		KdTree<T> kdTree(dimension);
		kdTree.insert(pointers.data(), pointers.size());

		// first we validate that radius search for node values always returns the node itself
		for (unsigned int n = 0u; n < number; ++n)
		{
			const T* value = pointers[n];

			size_t foundNeighbors = kdTree.radiusSearch(value, typename SquareValueTyper<T>::Type(0.01), neighbors.data(), neighbors.size());

			bool foundSelf = false;
			for (unsigned int i = 0u; i < foundNeighbors; ++i) {
				if (neighbors[i] == value) {
					foundSelf = true;
					break;
				}
			}
			if (!foundSelf)
				allSucceeded = false;
		}

		// then we validate that resulting neighborhoods are complete
		for (unsigned int i = 0u; i < 1000u; ++i)
		{
			for (unsigned int d = 0u; d < dimension; ++d)
				randoms[d] = RandomT<T>::scalar(randomGenerator, T(-1.0), T(1.0));

			const T* value = randoms.data();

			const T* randomValue = pointers[RandomI::random(number - 1)];

			T radius = 0.0;
			for (unsigned int d = 0u; d < dimension; ++d)
				radius += sqr(value[d] - randomValue[d]);

			const size_t foundNeighbors = kdTree.radiusSearch(value, radius, neighbors.data(), neighbors.size());
			std::unordered_set<const T *> seenNeighbors;

			// check for false positives
			for (unsigned int n = 0u; n < foundNeighbors; ++n)
			{
				const T *neighborValue = neighbors[n];

				// make sure all pointers are valid
				if (neighborValue < elements.data() || neighborValue >= elements.data() + number * dimension || ((neighborValue - elements.data()) % dimension) != 0)
					allSucceeded = false;

				// make sure no pointer is returned twice
				if (seenNeighbors.find(neighborValue) == seenNeighbors.end())
					seenNeighbors.insert(neighborValue);
				else
					allSucceeded = false;

				T ssd = 0;
				for (unsigned int d = 0u; d < dimension; ++d)
					ssd += sqr(neighborValue[d] - value[d]);
				if (!NumericT<T>::isBelow(ssd, radius))
					allSucceeded = false;
			}

			// check for false negatives
			for (unsigned int n = 0u; n < number; ++n)
			{
				const T* neighborValue = pointers[n];

				if (seenNeighbors.find(neighborValue) != seenNeighbors.end())
					continue;

				T ssd = 0;
				for (unsigned int d = 0u; d < dimension; ++d)
					ssd += sqr(neighborValue[d] - value[d]);
				if (!NumericT<T>::isAbove(ssd, radius))
					allSucceeded = false;
			}
		}

		// finally validate that output buffer size is always respected
		for (unsigned int i = 0u; i < 1000u; ++i)
		{
			for (unsigned int d = 0u; d < dimension; ++d)
				randoms[d] = RandomT<T>::scalar(randomGenerator, T(-1.0), T(1.0));

			const T* value = randoms.data();

			const T* randomValue = pointers[RandomI::random(number - 1)];

			T radius = 0.0;
			for (unsigned int d = 0u; d < dimension; ++d)
				radius += sqr(value[d] - randomValue[d]);

			const size_t maxNeighbors = RandomI::random((unsigned int)(neighbors.size()) - 1u);
			const size_t foundNeighbors = kdTree.radiusSearch(value, radius, neighbors.data(), maxNeighbors);

			if (foundNeighbors > maxNeighbors)
				allSucceeded = false;

			for (unsigned int n = 0u; n < foundNeighbors; ++n)
			{
				const T *neighborValue = neighbors[n];

				// make sure all pointers are valid
				if (neighborValue < elements.data() || neighborValue >= elements.data() + number * dimension || ((neighborValue - elements.data()) % dimension) != 0)
					allSucceeded = false;

				T ssd = 0;
				for (unsigned int d = 0u; d < dimension; ++d)
					ssd += sqr(neighborValue[d] - value[d]);
				if (!NumericT<T>::isBelow(ssd, radius))
					allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "KD Radius Search performance: Best: " << performanceRadiusSearch.bestMseconds() << "ms, worst: " << performanceRadiusSearch.worstMseconds() << "ms, average: " << performanceRadiusSearch.averageMseconds() << "ms";
	Log::info() << "Brute Force performance: Best: " << ssdPerformanceRadiusSearch.bestMseconds() << "ms, worst: " << ssdPerformanceRadiusSearch.worstMseconds() << "ms, average: " << ssdPerformanceRadiusSearch.averageMseconds() << "ms";

	if (performanceRadiusSearch.averageMseconds() > 0)
		Log::info() << "KD boost factor: Average: " << String::toAString(ssdPerformanceRadiusSearch.averageMseconds() / performanceRadiusSearch.averageMseconds(), 2u) << "x";

	return allSucceeded;
}

}

}

}
