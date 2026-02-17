/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testgeometry/TestNonLinearOptimizationLine.h"
#include "ocean/test/testgeometry/Utilities.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/Validation.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/geometry/NonLinearOptimizationLine.h"

#include "ocean/math/Random.h"
#include "ocean/math/Rotation.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

bool TestNonLinearOptimizationLine::test(const double testDuration, Worker* /*worker*/, const TestSelector& selector)
{
	TestResult testResult("Line non linear optimization test");

	Log::info() << " ";

	if (selector.shouldRun("nonlinearoptimizationline"))
	{
		testResult = testNonLinearOptimizationLine(testDuration);

		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

bool TestNonLinearOptimizationLine::testNonLinearOptimizationLine(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Optimization of 2D line:";
	Log::info() << " ";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	OCEAN_EXPECT_TRUE(validation, testOptimizeLineIdeal(5u, testDuration, Geometry::Estimator::ET_SQUARE, 0u));
	Log::info() << " ";
	OCEAN_EXPECT_TRUE(validation, testOptimizeLineIdeal(15u, testDuration, Geometry::Estimator::ET_SQUARE, 0u));
	Log::info() << " ";
	OCEAN_EXPECT_TRUE(validation, testOptimizeLineIdeal(50u, testDuration, Geometry::Estimator::ET_SQUARE, 0u));
	Log::info() << " ";
	OCEAN_EXPECT_TRUE(validation, testOptimizeLineIdeal(500u, testDuration, Geometry::Estimator::ET_SQUARE, 0u));

	Log::info() << " ";
	Log::info() << " ";

	OCEAN_EXPECT_TRUE(validation, testOptimizeLineIdeal(5u, testDuration, Geometry::Estimator::ET_TUKEY, 1u));
	Log::info() << " ";
	OCEAN_EXPECT_TRUE(validation, testOptimizeLineIdeal(15u, testDuration, Geometry::Estimator::ET_TUKEY, 3u));
	Log::info() << " ";
	OCEAN_EXPECT_TRUE(validation, testOptimizeLineIdeal(50u, testDuration, Geometry::Estimator::ET_TUKEY, 15u));
	Log::info() << " ";
	OCEAN_EXPECT_TRUE(validation, testOptimizeLineIdeal(500u, testDuration, Geometry::Estimator::ET_TUKEY, 100u));

	Log::info() << " ";
	Log::info() << " ";

	OCEAN_EXPECT_TRUE(validation, testOptimizeLineNoisy(10u, testDuration, Geometry::Estimator::ET_TUKEY, Scalar(0.05), 3u));
	Log::info() << " ";
	OCEAN_EXPECT_TRUE(validation, testOptimizeLineNoisy(20u, testDuration, Geometry::Estimator::ET_TUKEY, Scalar(0.05), 6u));
	Log::info() << " ";
	OCEAN_EXPECT_TRUE(validation, testOptimizeLineNoisy(50u, testDuration, Geometry::Estimator::ET_TUKEY, Scalar(0.05), 10u));
	Log::info() << " ";
	OCEAN_EXPECT_TRUE(validation, testOptimizeLineNoisy(500u, testDuration, Geometry::Estimator::ET_TUKEY, Scalar(0.05), 100u));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestNonLinearOptimizationLine::testOptimizeLineIdeal(const unsigned int numberPoints, const double testDuration, const Geometry::Estimator::EstimatorType type, const unsigned int outliers)
{
	ocean_assert(numberPoints >= 2u);

	Log::info() << "Testing " << numberPoints << " ideal points with " << outliers << " outliers using estimator " << Geometry::Estimator::translateEstimatorType(type) << ":";

	Scalar averageInitialError = 0;
	Scalar averageOptimizedError = 0;

	Scalars medianInitialErrors;
	Scalars medianOptimizedErrors;

	unsigned long long iterations = 0ull;

	HighPerformanceStatistic performance;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);
	do
	{
		const Scalar environmentRadius = Random::scalar(randomGenerator, Scalar(0.01), 10);

		const Vector2 linePosition = Random::vector2(randomGenerator, -environmentRadius * 10, environmentRadius * 10);
		const Vector2 lineDirection = Random::vector2(randomGenerator);

		const Line2 line(linePosition, lineDirection);

		Vectors2 linePoints;

		for (unsigned int n = 0u; n < numberPoints; ++n)
		{
			const Vector2 point = linePosition + Random::vector2(randomGenerator, -environmentRadius, environmentRadius);
			const Vector2 projectedPoint = line.nearestPoint(point);
			ocean_assert(projectedPoint.distance(linePosition) < environmentRadius * 2);

			linePoints.push_back(projectedPoint);
		}

		IndexSet32 outlierIndices;
		while (outlierIndices.size() < outliers)
		{
			outlierIndices.insert(RandomI::random(randomGenerator, numberPoints - 1u));
		}

		for (IndexSet32::const_iterator i = outlierIndices.begin(); i != outlierIndices.end(); ++i)
		{
			const Scalar outlierScale = Random::scalar(randomGenerator, Scalar(0.5), 1000);
			const Scalar outlierSign = Random::sign(randomGenerator);

			linePoints[*i] = line.nearestPoint(linePoints[*i]) + line.normal() * environmentRadius * outlierScale * outlierSign;
		}

		const Scalar offsetAngle = Random::scalar(randomGenerator, Numeric::deg2rad(5), Numeric::deg2rad(20));
		const Scalar offsetSign = Random::sign(randomGenerator);

		const Rotation offsetRotation(0, 0, 1, offsetAngle * offsetSign);
		const Vector3 faultyLineDirection3(offsetRotation * Vector3(lineDirection, 0));

		const Vector2 faultyLineDirection(faultyLineDirection3.x(), faultyLineDirection3.y());
		const Vector2 faultyLinePosition(linePosition + Random::vector2(randomGenerator, -environmentRadius * Scalar(0.1), environmentRadius * Scalar(0.1)));

		const Line2 faultyLine(faultyLinePosition, faultyLineDirection);

		performance.start();

		Line2 optimizedLine;
		Scalar initialError, finalError;
		if (Geometry::NonLinearOptimizationLine::optimizeLine(faultyLine, ConstArrayAccessor<Vector2>(linePoints), optimizedLine, 50u, type, Scalar(0.001), Scalar(5), &initialError, &finalError))
		{
			performance.stop();

			averageInitialError += initialError;
			averageOptimizedError += finalError;

			medianInitialErrors.push_back(initialError);
			medianOptimizedErrors.push_back(finalError);
		}
		else
		{
			performance.skip();
		}

		iterations++;
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	ocean_assert(iterations != 0ull);
	averageInitialError /= Scalar(iterations);
	averageOptimizedError /= Scalar(iterations);

	Log::info() << "Average error: " << String::toAString(averageInitialError, 1u) << " -> " << String::toAString(averageOptimizedError, 1u);
	Log::info() << "Median error: " << String::toAString(Median::constMedian(medianInitialErrors.data(), medianInitialErrors.size()), 1u) << " -> " << String::toAString(Median::constMedian(medianOptimizedErrors.data(), medianOptimizedErrors.size()), 1u);
	Log::info() << "Performance: Best: " << String::toAString(performance.bestMseconds(), 1u) << "ms, worst: " << String::toAString(performance.worstMseconds(), 1u) << "ms, average: " << String::toAString(performance.averageMseconds(), 1u) << "ms, first: " << String::toAString(performance.firstMseconds(), 1u) << "ms";

	return true;
}

bool TestNonLinearOptimizationLine::testOptimizeLineNoisy(const unsigned int numberPoints, const double testDuration, const Geometry::Estimator::EstimatorType type, const Scalar standardDeviation, const unsigned int outliers)
{
	ocean_assert(numberPoints >= 2u);
	ocean_assert(standardDeviation >= 0);

	Log::info() << "Testing " << numberPoints << " noisy points with " << outliers << " outliers and " << String::toAString(standardDeviation, 1u) << " px noise using estimator " << Geometry::Estimator::translateEstimatorType(type) << ":";

	Scalar averageIdealNoisyError = 0;
	Scalar averageOptimizedError = 0;

	Scalars medianIdealNoisyErrors;
	Scalars medianOptimizedErrors;

	unsigned long long iterations = 0ull;

	HighPerformanceStatistic performance;
	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const Scalar environmentRadius = Random::scalar(randomGenerator, Scalar(0.01), 10);

		const Vector2 linePosition = Random::vector2(randomGenerator, -environmentRadius * 10, environmentRadius * 10);
		const Vector2 lineDirection = Random::vector2(randomGenerator);

		const Line2 line(linePosition, lineDirection);

		Vectors2 linePoints;

		for (unsigned int n = 0u; n < numberPoints; ++n)
		{
			const Vector2 point = linePosition + Random::vector2(randomGenerator, -environmentRadius, environmentRadius);
			const Vector2 projectedPoint = line.nearestPoint(point);
			ocean_assert(projectedPoint.distance(linePosition) < environmentRadius * 2);

			Vector2 linePoint = projectedPoint;

			if (standardDeviation > 0)
			{
				linePoint += line.normal() * Random::gaussianNoise(randomGenerator, environmentRadius * standardDeviation);
			}

			linePoints.push_back(linePoint);
		}

		IndexSet32 outlierIndices;
		while (outlierIndices.size() < outliers)
		{
			outlierIndices.insert(RandomI::random(randomGenerator, numberPoints - 1u));
		}

		for (IndexSet32::const_iterator i = outlierIndices.begin(); i != outlierIndices.end(); ++i)
		{
			const Scalar outlierScale = Random::scalar(randomGenerator, Scalar(0.5), 1000);
			const Scalar outlierSign = Random::sign(randomGenerator);

			linePoints[*i] = line.nearestPoint(linePoints[*i]) + line.normal() * environmentRadius * outlierScale * outlierSign;
		}

		const Scalar offsetAngle = Random::scalar(randomGenerator, Numeric::deg2rad(5), Numeric::deg2rad(20));
		const Scalar offsetSign = Random::sign(randomGenerator);

		const Rotation offsetRotation(0, 0, 1, offsetAngle * offsetSign);
		const Vector3 faultyLineDirection3(offsetRotation * Vector3(lineDirection, 0));

		const Vector2 faultyLineDirection(faultyLineDirection3.x(), faultyLineDirection3.y());
		const Vector2 faultyLinePosition(linePosition + Random::vector2(randomGenerator, -environmentRadius * Scalar(0.1), environmentRadius * Scalar(0.1)));

		const Line2 faultyLine(faultyLinePosition, faultyLineDirection);

		performance.start();

		Line2 optimizedLine;
		Scalar finalError;
		if (Geometry::NonLinearOptimizationLine::optimizeLine(faultyLine, ConstArrayAccessor<Vector2>(linePoints), optimizedLine, 50u, type, Scalar(0.001), Scalar(5), nullptr, &finalError))
		{
			performance.stop();

			Scalar idealNoisyError = 0;
			for (unsigned int n = 0u; n < linePoints.size(); ++n)
			{
				if (outlierIndices.find(n) == outlierIndices.end())
				{
					idealNoisyError += Numeric::sqr(line.distance(linePoints[n]));
				}
			}

			averageIdealNoisyError += idealNoisyError;
			averageOptimizedError += finalError;

			medianIdealNoisyErrors.push_back(idealNoisyError);
			medianOptimizedErrors.push_back(finalError);
		}
		else
		{
			performance.skip();
		}

		iterations++;
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	ocean_assert(iterations != 0ull);
	averageIdealNoisyError /= Scalar(iterations);
	averageOptimizedError /= Scalar(iterations);

	Log::info() << "Average error: " << String::toAString(averageIdealNoisyError, 1u) << " -> " << String::toAString(averageOptimizedError, 1u);
	Log::info() << "Median error: " << String::toAString(Median::constMedian(medianIdealNoisyErrors.data(), medianIdealNoisyErrors.size()), 1u) << " -> " << String::toAString(Median::constMedian(medianOptimizedErrors.data(), medianOptimizedErrors.size()), 1u);
	Log::info() << "Performance: Best: " << String::toAString(performance.bestMseconds(), 1u) << "ms, worst: " << String::toAString(performance.worstMseconds(), 1u) << "ms, average: " << String::toAString(performance.averageMseconds()) << "ms, first: " << String::toAString(performance.firstMseconds()) << "ms";

	return true;
}

}

}

}
