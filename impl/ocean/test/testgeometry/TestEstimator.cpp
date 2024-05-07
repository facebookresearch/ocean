// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "ocean/test/testgeometry/TestEstimator.h"
#include "ocean/test/testgeometry/Utilities.h"

#include "ocean/base/Timestamp.h"

#include "ocean/geometry/Estimator.h"

#include "ocean/math/Random.h"

namespace Ocean
{

using namespace Geometry;

namespace Test
{

namespace TestGeometry
{

bool TestEstimator::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Estimator test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testNeedSigma() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testIsStandardEstimator() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testRobustError(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testRobustWeight(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testTranslateEstimatorType() && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Estimator test succeeded.";
	}
	else
	{
		Log::info() << "Estimator test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestEstimator, NeedSigma)
{
	EXPECT_TRUE(TestEstimator::testNeedSigma());
}

TEST(TestEstimator, IsStandardEstimator)
{
	EXPECT_TRUE(TestEstimator::testIsStandardEstimator());
}

TEST(TestEstimator, RobustError)
{
	EXPECT_TRUE(TestEstimator::testRobustError(GTEST_TEST_DURATION));
}

TEST(TestEstimator, RobustWeight)
{
	EXPECT_TRUE(TestEstimator::testRobustWeight(GTEST_TEST_DURATION));
}

TEST(TestEstimator, TranslateEstimatorType)
{
	EXPECT_TRUE(TestEstimator::testTranslateEstimatorType());
}

#endif // OCEAN_USE_GTEST

bool TestEstimator::testNeedSigma()
{
	Log::info() << "Need sigma test:";

	bool allSucceeded = true;

	if (Estimator::needSigma<Estimator::ET_SQUARE>() || Estimator::needSigma(Estimator::ET_SQUARE))
	{
		allSucceeded = false;
	}

	if (Estimator::needSigma<Estimator::ET_LINEAR>() || Estimator::needSigma(Estimator::ET_LINEAR))
	{
		allSucceeded = false;
	}

	if (!Estimator::needSigma<Estimator::ET_HUBER>() || !Estimator::needSigma(Estimator::ET_HUBER))
	{
		allSucceeded = false;
	}

	if (!Estimator::needSigma<Estimator::ET_TUKEY>() || !Estimator::needSigma(Estimator::ET_TUKEY))
	{
		allSucceeded = false;
	}

	if (!Estimator::needSigma<Estimator::ET_CAUCHY>() || !Estimator::needSigma(Estimator::ET_CAUCHY))
	{
		allSucceeded = false;
	}

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestEstimator::testIsStandardEstimator()
{
	Log::info() << "Is standard estimator test:";

	bool allSucceeded = true;

	if (!Estimator::isStandardEstimator<Estimator::ET_SQUARE>() || !Estimator::isStandardEstimator(Estimator::ET_SQUARE))
	{
		allSucceeded = false;
	}

	if (Estimator::isStandardEstimator<Estimator::ET_LINEAR>() || Estimator::isStandardEstimator(Estimator::ET_LINEAR))
	{
		allSucceeded = false;
	}

	if (Estimator::isStandardEstimator<Estimator::ET_HUBER>() || Estimator::isStandardEstimator(Estimator::ET_HUBER))
	{
		allSucceeded = false;
	}

	if (Estimator::isStandardEstimator<Estimator::ET_TUKEY>() || Estimator::isStandardEstimator(Estimator::ET_TUKEY))
	{
		allSucceeded = false;
	}

	if (Estimator::isStandardEstimator<Estimator::ET_CAUCHY>() || Estimator::isStandardEstimator(Estimator::ET_CAUCHY))
	{
		allSucceeded = false;
	}

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestEstimator::testRobustError(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing robust error function:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		for (const Estimator::EstimatorType& estimatorType : Estimator::estimatorTypes())
		{
			constexpr size_t numberValues = 100;

			Scalars values;
			values.reserve(numberValues);

			for (size_t n = 0; n < numberValues; ++n)
			{
				constexpr Scalar range = std::is_same<double, Scalar>::value ? Scalar(100) : Scalar(10);
				values.emplace_back(Random::scalar(randomGenerator, -range , range));
			}

			std::sort(values.begin(), values.end(), compareAbsolute);

			Scalar previousRobustError = Numeric::minValue();

			const Scalar sigma = Estimator::needSigma(estimatorType) ? Random::scalar(Numeric::weakEps(), 100) : Scalar(0);

			for (const Scalar& value : values)
			{
				const Scalar robustError = Estimator::robustError(value, sigma, estimatorType);

				if (robustError < previousRobustError)
				{
					allSucceeded = false;
				}

				previousRobustError = robustError;

				if (Numeric::isNotWeakEqual(robustError, Estimator::robustErrorSquare(value * value, sigma * sigma, estimatorType)))
				{
					allSucceeded = false;
				}

				switch (estimatorType)
				{
					case Estimator::ET_SQUARE:
					{
						if (Numeric::isNotEqual(robustError, value * value / Scalar(2)))
						{
							allSucceeded = false;
						}

						break;
					}

					case Estimator::ET_LINEAR:
					{
						if (Numeric::isNotEqual(robustError, Numeric::abs(value)))
						{
							allSucceeded = false;
						}

						break;
					}

					case Estimator::ET_HUBER:
					{
						ocean_assert(sigma > Scalar(0));

						Scalar huberValue = Numeric::minValue();

						if (Numeric::abs(value) <= sigma)
						{
							huberValue = value * value / Scalar(2);
						}
						else
						{
							huberValue = sigma * (Numeric::abs(value) - sigma / Scalar(2));
						}

						if (Numeric::isNotEqual(robustError, huberValue))
						{
							allSucceeded = false;
						}

						break;
					}

					case Estimator::ET_TUKEY:
					{
						ocean_assert(sigma > Scalar(0));

						Scalar tukeyValue = Numeric::minValue();

						if (Numeric::abs(value) <= sigma)
						{
							tukeyValue = Numeric::sqr(sigma) / Scalar(6) * (Scalar(1) - Numeric::pow(Scalar(1) - Numeric::sqr(value / sigma), Scalar(3)));
						}
						else
						{
							tukeyValue = sigma * sigma / Scalar(6);
						}

						if (Numeric::isNotWeakEqual(robustError, tukeyValue))
						{
							allSucceeded = false;
						}

						break;
					}

					case Estimator::ET_CAUCHY:
					{
						ocean_assert(sigma > Scalar(0));

						const Scalar cauchyValue = (sigma * sigma) / Scalar(2.0) * Numeric::log(Scalar(1) + Numeric::sqr(value / sigma));

						if (Numeric::isNotEqual(robustError, cauchyValue))
						{
							allSucceeded = false;
						}

						break;
					}

					case Estimator::ET_INVALID:
						ocean_assert(false && "This should never happen!");
						allSucceeded = false;
						break;
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestEstimator::testRobustWeight(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing robust weight function:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		for (const Estimator::EstimatorType& estimatorType : Estimator::estimatorTypes())
		{
			constexpr size_t numberValues = 100;

			Scalars values;
			values.reserve(numberValues);

			for (size_t n = 0; n < numberValues; ++n)
			{
				values.emplace_back(Random::scalar(randomGenerator, -100, 100));
			}

			std::sort(values.begin(), values.end(), compareAbsolute);

			Scalar previousRobustWeight = Numeric::maxValue();

			const Scalar sigma = Estimator::needSigma(estimatorType) ? Random::scalar(Numeric::weakEps(), 100) : Scalar(0);

			for (const Scalar& value : values)
			{
				const Scalar robustWeight = Estimator::robustWeight(value, sigma, estimatorType);

				if (robustWeight > previousRobustWeight)
				{
					allSucceeded = false;
				}

				previousRobustWeight = robustWeight;

				if (Numeric::isNotWeakEqual(robustWeight, Estimator::robustWeightSquare(value * value, sigma * sigma, estimatorType)))
				{
					allSucceeded = false;
				}

				switch (estimatorType)
				{
					case Estimator::ET_SQUARE:
					{
						if (Numeric::isNotEqual(robustWeight, Scalar(1)))
						{
							allSucceeded = false;
						}

						break;
					}

					case Estimator::ET_LINEAR:
					{
						Scalar linearWeight = Numeric::ratio(Scalar(1), Numeric::abs(value), Numeric::maxValue());

						if (linearWeight > Estimator::maximalWeight())
						{
							linearWeight = Estimator::maximalWeight();
						}

						if (Numeric::isNotEqual(robustWeight, linearWeight))
						{
							allSucceeded = false;
						}

						break;
					}

					case Estimator::ET_HUBER:
					{
						ocean_assert(sigma > Scalar(0));

						Scalar huberWeight = Numeric::minValue();

						if (Numeric::abs(value) <= sigma)
						{
							huberWeight = Scalar(1);
						}
						else
						{
							huberWeight = Scalar(sigma) / Numeric::abs(value);
						}

						if (huberWeight > Estimator::maximalWeight())
						{
							huberWeight = Estimator::maximalWeight();
						}

						if (Numeric::isNotEqual(robustWeight, huberWeight))
						{
							allSucceeded = false;
						}

						break;
					}

					case Estimator::ET_TUKEY:
					{
						ocean_assert(sigma > Scalar(0));

						Scalar tukeyWeight = Numeric::minValue();

						if (Numeric::abs(value) <= sigma)
						{

							tukeyWeight = Numeric::sqr(Scalar(1) - Numeric::sqr(value / sigma));
						}
						else
						{
							tukeyWeight = Scalar(0);
						}

						if (tukeyWeight > Estimator::maximalWeight())
						{
							tukeyWeight = Estimator::maximalWeight();
						}

						if (Numeric::isNotEqual(robustWeight, tukeyWeight))
						{
							allSucceeded = false;
						}

						break;
					}

					case Estimator::ET_CAUCHY:
					{
						ocean_assert(sigma > Scalar(0));

						Scalar cauchyWeight = Scalar(1) / (Scalar(1) + Numeric::sqr(value / sigma));

						if (cauchyWeight > Estimator::maximalWeight())
						{
							ocean_assert(false && "This should never happen!");

							cauchyWeight = Estimator::maximalWeight();
						}

						if (Numeric::isNotEqual(robustWeight, cauchyWeight))
						{
							allSucceeded = false;
						}

						break;
					}

					case Estimator::ET_INVALID:
						ocean_assert(false && "This should never happen!");
						allSucceeded = false;
						break;
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestEstimator::testTranslateEstimatorType()
{
	Log::info() << "Testing translate estimator type:";

	bool allSucceeded = true;

	const Estimator::EstimatorTypes& estimatorTypes = Estimator::estimatorTypes();

	if (estimatorTypes.size() != 5)
	{
		allSucceeded = false;
	}

	for (const Estimator::EstimatorType estimatorType : estimatorTypes)
	{
		const std::string name = Estimator::translateEstimatorType(estimatorType);

		if (name == "Invalid")
		{
			allSucceeded = false;
		}

		if (Estimator::translateEstimatorType(name) != estimatorType)
		{
			allSucceeded = false;
		}
	}

	if (Estimator::translateEstimatorType(Estimator::ET_INVALID) != "Invalid")
	{
		allSucceeded = false;
	}

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded";
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
