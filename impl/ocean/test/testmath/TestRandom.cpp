/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestRandom.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/Worker.h"

#include "ocean/math/Euler.h"
#include "ocean/math/Random.h"

#include <ctime>

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestRandom::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "---   Random test:   ---";
	Log::info() << " ";

	allSucceeded = testStandardRandomSingleThreaded(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testOceanRandomSingleThreaded(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testStandardRandomMultiThreaded(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testOceanRandomMultiThreaded(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testStandardRandomTriple(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testOceanRandomTriple(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testStandardRandomVector3(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testOceanRandomVector3(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testStandardRandomEuler(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testOceanRandomEuler(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
		Log::info() << "Random test succeeded.";
	else
		Log::info() << "Random test FAILED!";

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestRandom, StandardRandomSingleThreaded) {
	EXPECT_TRUE(TestRandom::testStandardRandomSingleThreaded(GTEST_TEST_DURATION));
}

TEST(TestRandom, OceanRandomSingleThreaded) {
	EXPECT_TRUE(TestRandom::testOceanRandomSingleThreaded(GTEST_TEST_DURATION));
}

TEST(TestRandom, StandardRandomMultiThreaded) {
	EXPECT_TRUE(TestRandom::testStandardRandomMultiThreaded(GTEST_TEST_DURATION));
}

TEST(TestRandom, OceanRandomMultiThreaded) {
	EXPECT_TRUE(TestRandom::testOceanRandomMultiThreaded(GTEST_TEST_DURATION));
}

TEST(TestRandom, StandardRandomTriple) {
	EXPECT_TRUE(TestRandom::testStandardRandomTriple(GTEST_TEST_DURATION));
}

TEST(TestRandom, OceanRandomTriple) {
	EXPECT_TRUE(TestRandom::testOceanRandomTriple(GTEST_TEST_DURATION));
}

TEST(TestRandom, StandardRandomVector3) {
	EXPECT_TRUE(TestRandom::testStandardRandomVector3(GTEST_TEST_DURATION));
}

TEST(TestRandom, OceanRandomVector3) {
	EXPECT_TRUE(TestRandom::testOceanRandomVector3(GTEST_TEST_DURATION));
}

TEST(TestRandom, StandardRandomEuler) {
	EXPECT_TRUE(TestRandom::testStandardRandomEuler(GTEST_TEST_DURATION));
}

TEST(TestRandom, OceanRandomEuler) {
	EXPECT_TRUE(TestRandom::testOceanRandomEuler(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestRandom::testStandardRandomSingleThreaded(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Standard rand() test single threaded:";

	const unsigned int number = 1000u * 1000u * 10u;

	std::vector<int> randomValues(number);
	unsigned long long iterations = 0ul;

	const Timestamp startTimestamp(true);
	HighPerformanceTimer timer;

	do
	{
		calculateStandardRandomValues(randomValues.data(), 0u, number);
		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);

	const double ys = timer.yseconds() / (double(iterations) * double(number));
	Log::info() << "Performance: " << ys << "mys";

	return true;
}

bool TestRandom::testStandardRandomMultiThreaded(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Standard rand() test multi threaded:";

	const unsigned int number = 1000u * 1000u * 10u;

	std::vector<int> randomValues(number);
	unsigned long long iterations = 0ull;

	Worker worker;

	const Timestamp startTimestamp(true);
	HighPerformanceTimer timer;

	do
	{
		worker.executeFunction(Worker::Function::createStatic(&TestRandom::calculateStandardRandomValues, randomValues.data(), 0u, 0u), 0u, number);
		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double ys = timer.yseconds() / (double(iterations) * double(number));

	Log::info() << "Performance: " << ys << "mys";
	Log::info() << "Comparable single threaded performance: " << ys * double(worker.threads()) << "mys";

	return true;
}

bool TestRandom::testOceanRandomSingleThreaded(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Ocean rand() test single threaded:";

	const unsigned int number = 1000u * 1000u * 10u;

	std::vector<int> randomValues(number);
	unsigned long long iterations = 0ull;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);
	HighPerformanceTimer timer;

	do
	{
		calculateOceanRandomValues(&randomGenerator, randomValues.data(), 0u, number);
		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);

	const double ys = timer.yseconds() / (double(iterations) * double(number));
	Log::info() << "Performance: " << ys << "mys";

	return true;
}

bool TestRandom::testOceanRandomMultiThreaded(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Ocean rand() test multi threaded:";

	const unsigned int number = 1000u * 1000u * 10u;

	std::vector<int> randomValues(number);
	unsigned long long iterations = 0ull;

	Worker worker;
	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);
	HighPerformanceTimer timer;

	do
	{
		worker.executeFunction(Worker::Function::createStatic(&TestRandom::calculateOceanRandomValues, &randomGenerator, randomValues.data(), 0u, 0u), 0u, number);
		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double ys = timer.yseconds() / (double(iterations) * double(number));

	Log::info() << "Performance: " << ys << "mys";
	Log::info() << "Comparable single threaded performance: " << ys * double(worker.threads()) << "mys";

	return true;
}

bool TestRandom::testStandardRandomTriple(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Standard triple rand():";

	const unsigned int constIterations = 100000u;
	unsigned int a, b, c;

	unsigned long long iterations = 0ull;

	const Timestamp startTimestamp(true);
	HighPerformanceTimer timer;

	do
	{
		for (unsigned int n = 0u; n < constIterations; ++n)
			Random::random(2000, a, b, c);

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);

	const double ys = timer.yseconds() / (double(iterations) * double(constIterations));
	Log::info() << "Performance: " << ys << "mys";

	return true;
}

bool TestRandom::testOceanRandomTriple(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Ocean triple rand():";

	const unsigned int constIterations = 100000u;
	unsigned int a, b, c;

	RandomGenerator randomGenerator;

	unsigned long long iterations = 0ull;

	const Timestamp startTimestamp(true);
	HighPerformanceTimer timer;

	do
	{
		for (unsigned int n = 0u; n < constIterations; ++n)
			Random::random(randomGenerator, 2000, a, b, c);

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);

	const double ys = timer.yseconds() / (double(iterations) * double(constIterations));
	Log::info() << "Performance: " << ys << "mys";

	return true;
}

bool TestRandom::testStandardRandomVector3(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Standard vector3 rand() with three ranges:";

	unsigned long long succeeded = 0ull;
	unsigned long long iterations = 0ull;

	HighPerformanceTimer timer;
	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const Vector3 range(Random::scalar(0, 1000), Random::scalar(0, 1000), Random::scalar(0, 1000));
			const Vector3 vector = Random::vector3(range);

			if (vector.x() >= -range.x() && vector.x() <= range.x() && vector.y() >= -range.y() && vector.y() <= range.y() && vector.z() >= -range.z() && vector.z() <= range.z())
				succeeded++;

			iterations++;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

bool TestRandom::testOceanRandomVector3(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Ocean vector3 rand() with three ranges:";

	RandomGenerator randomGenerator;

	unsigned long long succeeded = 0ull;
	unsigned long long iterations = 0ull;

	HighPerformanceTimer timer;
	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const Vector3 range(Random::scalar(randomGenerator, 0, 1000), Random::scalar(randomGenerator, 0, 1000), Random::scalar(randomGenerator, 0, 1000));
			const Vector3 vector = Random::vector3(randomGenerator, range);

			if (vector.x() >= -range.x() && vector.x() <= range.x() && vector.y() >= -range.y() && vector.y() <= range.y() && vector.z() >= -range.z() && vector.z() <= range.z())
				succeeded++;

			iterations++;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

bool TestRandom::testStandardRandomEuler(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Standard euler rand() with one range:";

	unsigned long long succeeded = 0ull;
	unsigned long long iterations = 0ull;

	HighPerformanceTimer timer;
	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const Scalar range = Random::scalar(0, Numeric::pi_2() - Numeric::eps());
			const Euler euler = Random::euler(range);

			if (euler.yaw() >= -range && euler.yaw() <= range && euler.pitch() >= -range && euler.pitch() <= range && euler.roll() >= -range && euler.roll() <= range)
				succeeded++;

			iterations++;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

bool TestRandom::testOceanRandomEuler(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Ocean euler rand() with one range:";

	RandomGenerator randomGenerator;

	unsigned long long succeeded = 0ull;
	unsigned long long iterations = 0ull;

	HighPerformanceTimer timer;
	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const Scalar range = Random::scalar(randomGenerator, 0, Numeric::pi_2() - Numeric::eps());
			const Euler euler = Random::euler(randomGenerator, range);

			if (euler.yaw() >= -range && euler.yaw() <= range && euler.pitch() >= -range && euler.pitch() <= range && euler.roll() >= -range && euler.roll() <= range)
				succeeded++;

			iterations++;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

void TestRandom::calculateStandardRandomValues(int* values, const unsigned int firstValue, const unsigned int numberValues)
{
	ocean_assert(values);

	srand((unsigned int)(time(nullptr)));

	for (unsigned int n = firstValue; n < firstValue + numberValues; ++n)
	{
		values[n] = rand();
	}
}

void TestRandom::calculateOceanRandomValues(RandomGenerator* generator, int* values, const unsigned int firstValue, const unsigned int numberValues)
{
	ocean_assert(generator && values);

	RandomGenerator localGenerator(*generator);

	for (unsigned int n = firstValue; n < firstValue + numberValues; ++n)
	{
		values[n] = localGenerator.rand();
	}
}

}

}

}
