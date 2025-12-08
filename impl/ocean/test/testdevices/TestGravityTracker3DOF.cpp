/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testdevices/TestGravityTracker3DOF.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/Validation.h"

#include "ocean/devices/GravityTracker3DOF.h"

#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestDevices
{

/**
 * This class implements a test gravity tracker for testing interpolation.
 */
class TestableGravityTracker3DOF : public Devices::GravityTracker3DOF
{
	public:

		/**
		 * Creates a new test gravity tracker.
		 * @param name The name of the tracker
		 */
		explicit TestableGravityTracker3DOF(const std::string& name) :
			Devices::Device(name, deviceTypeGravityTracker3DOF()),
			Devices::Measurement(name, deviceTypeGravityTracker3DOF()),
			Devices::Tracker(name, deviceTypeGravityTracker3DOF()),
			Devices::GravityTracker3DOF(name)
		{
			// nothing to do here
		}

		/**
		 * Returns the name of the owner library.
		 * @return The library name
		 */
		const std::string& library() const override
		{
			static const std::string libraryName("TestLibrary");
			return libraryName;
		}

		/**
		 * Adds a new sample to the tracker.
		 * @param timestamp The timestamp of the sample
		 * @param gravities The gravity measurements
		 * @param referenceSystem The reference system
		 */
		void addSample(const Timestamp& timestamp, const GravityTracker3DOFSample::Gravities& gravities, const ReferenceSystem referenceSystem = RS_OBJECT_IN_DEVICE)
		{
			ObjectIds objectIds;
			objectIds.reserve(gravities.size());

			for (size_t n = 0; n < gravities.size(); ++n)
			{
				objectIds.emplaceBack(ObjectId(n));
			}

			postNewSample(SampleRef(new GravityTracker3DOFSample(timestamp, referenceSystem, std::move(objectIds), gravities)));
		}
};

bool TestGravityTracker3DOF::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("GravityTracker3DOF test");
	Log::info() << " ";

	if (selector.shouldRun("sampleinterpolation"))
	{
		testResult = testSampleInterpolation(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestGravityTracker3DOF, SampleInterpolation)
{
	EXPECT_TRUE(TestGravityTracker3DOF::testSampleInterpolation(GTEST_TEST_DURATION));
}

#endif

bool TestGravityTracker3DOF::testSampleInterpolation(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing sample() function with all interpolation strategies:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		TestableGravityTracker3DOF tracker("Test Gravity Tracker");

		const unsigned int measurements = RandomI::random(randomGenerator, 1u, 10u);

		Timestamps timestamps;
		Vectors3 gravities;

		double currentTime = RandomD::scalar(randomGenerator, 1000.0, 2000.0);

		for (unsigned int n = 0u; n < measurements; ++n)
		{
			timestamps.emplace_back(currentTime);

			const Vector3 gravity = Random::vector3(randomGenerator);
			ocean_assert(gravity.isUnit());

			gravities.push_back(gravity);

			tracker.addSample(timestamps.back(), Devices::GravityTracker3DOF::GravityTracker3DOFSample::Gravities(1, gravity));

			currentTime += RandomD::scalar(randomGenerator, 0.001, 1.0);
		}

		const double firstTime = double(timestamps.front());
		const double lastTime = double(timestamps.back());

		const double eps = RandomD::scalar(randomGenerator, 0.1, 1.0);

		for (const Devices::Measurement::InterpolationStrategy strategy : {Devices::Measurement::IS_TIMESTAMP_INTERPOLATE, Devices::Measurement::IS_TIMESTAMP_NEAREST})
		{
			for (unsigned int i = 0u; i < 20u; ++i)
			{
				const double queryTime = RandomD::scalar(randomGenerator, firstTime - eps, lastTime + eps);

				const Timestamp queryTimestamp(queryTime);

				const Devices::Measurement::SampleRef sampleRef = tracker.sample(queryTimestamp, strategy);

				if (!sampleRef)
				{
					OCEAN_SET_FAILED(validation);
					continue;
				}

				const Devices::GravityTracker3DOF::GravityTracker3DOFSampleRef gravitySample = sampleRef;

				if (!gravitySample)
				{
					OCEAN_SET_FAILED(validation);
					continue;
				}

				if (gravitySample->gravities().size() != 1)
				{
					OCEAN_SET_FAILED(validation);
					continue;
				}

				const Vector3& actualGravity = gravitySample->gravities()[0];

				OCEAN_EXPECT_TRUE(validation, actualGravity.isUnit());

				OCEAN_EXPECT_TRUE(validation, gravitySample->timestamp().isValid());

				const double returnedTime = double(gravitySample->timestamp());

				if (strategy == Devices::Measurement::IS_TIMESTAMP_INTERPOLATE)
				{
					if (queryTime <= firstTime)
					{
						OCEAN_EXPECT_EQUAL(validation, returnedTime, firstTime);
						OCEAN_EXPECT_EQUAL(validation, actualGravity, gravities.front());
					}
					else if (queryTime >= lastTime)
					{
						OCEAN_EXPECT_EQUAL(validation, returnedTime, lastTime);
						OCEAN_EXPECT_EQUAL(validation, actualGravity, gravities.back());
					}
					else
					{
						OCEAN_EXPECT_TRUE(validation, NumericD::isWeakEqual(returnedTime, queryTime));

						const Vector3 expectedGravity = TestGravityTracker3DOF::expectedInterpolatedGravity(queryTime, timestamps, gravities);

						const Scalar angleError = actualGravity.angle(expectedGravity);
						OCEAN_EXPECT_LESS_EQUAL(validation, angleError, Numeric::deg2rad(Scalar(0.5)));
					}
				}
				else if (strategy == Devices::Measurement::IS_TIMESTAMP_NEAREST)
				{
					std::vector<size_t> closestIndices;
					double closestDistance = NumericD::maxValue();

					for (size_t n = 0; n < timestamps.size(); ++n)
					{
						const double distance = NumericD::abs(queryTime - double(timestamps[n]));

						if (distance < closestDistance)
						{
							closestDistance = distance;
							closestIndices = {n};
						}
						else if (distance == closestDistance)
						{
							closestIndices.push_back(n);
						}
					}

					OCEAN_EXPECT_NOT_EQUAL(validation, closestDistance, NumericD::maxValue());

					OCEAN_EXPECT_GREATER_EQUAL(validation, closestIndices.size() , size_t(1));

					size_t matchedIndex = size_t(-1);

					for (const size_t closestIndex : closestIndices)
					{
						if (NumericD::isEqual(double(timestamps[closestIndex]), returnedTime))
						{
							matchedIndex = closestIndex;
							break;
						}
					}

					OCEAN_EXPECT_NOT_EQUAL(validation, matchedIndex, size_t(-1));

					if (matchedIndex != size_t(-1))
					{
						OCEAN_EXPECT_EQUAL(validation, actualGravity, gravities[matchedIndex]);
					}
				}
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

Vector3 TestGravityTracker3DOF::expectedInterpolatedGravity(const double queryTime, const Timestamps& timestamps, const Vectors3& gravities)
{
	ocean_assert(timestamps.size() == gravities.size());
	ocean_assert(!timestamps.empty());

	if (timestamps.size() == 1)
	{
		return gravities.front();
	}

	size_t lowerIndex = 0;
	size_t upperIndex = 1;

	for (size_t n = 0; n < timestamps.size() - 1; ++n)
	{
		if (double(timestamps[n]) <= queryTime && queryTime <= double(timestamps[n + 1]))
		{
			lowerIndex = n;
			upperIndex = n + 1;
			break;
		}
	}

	const Vector3& lowerGravity = gravities[lowerIndex];
	const Vector3& upperGravity = gravities[upperIndex];

	const double lowerTime = double(timestamps[lowerIndex]);
	const double upperTime = double(timestamps[upperIndex]);

	const double duration = upperTime - lowerTime;

	if (NumericD::isEqualEps(duration))
	{
		return lowerGravity;
	}

	const double factor = (queryTime - lowerTime) / duration;

	const Scalar dotProduct = lowerGravity * upperGravity;
	const Scalar clampedDot = std::min(Scalar(1), std::max(Scalar(-1), dotProduct));
	const Scalar angle = Numeric::acos(clampedDot);

	if (Numeric::abs(angle) < Numeric::eps())
	{
		return lowerGravity;
	}

	const Scalar sinAngle = Numeric::sin(angle);
	const Scalar factorLower = Numeric::sin((Scalar(1) - Scalar(factor)) * angle) / sinAngle;
	const Scalar factorUpper = Numeric::sin(Scalar(factor) * angle) / sinAngle;

	Vector3 result = lowerGravity * factorLower + upperGravity * factorUpper;

	if (!result.normalize())
	{
		return lowerGravity;
	}

	return result;
}

} // namespace TestDevices

} // namespace Test

} // namespace Ocean
