/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testdevices/TestAccelerationSensor3DOF.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/Validation.h"

#include "ocean/devices/AccelerationSensor3DOF.h"

#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestDevices
{

/**
 * This class implements a test acceleration sensor for testing interpolation.
 */
class TestableAccelerationSensor3DOF : public Devices::AccelerationSensor3DOF
{
	public:

		/**
		 * Creates a new test acceleration sensor.
		 * @param name The name of the sensor
		 */
		explicit TestableAccelerationSensor3DOF(const std::string& name) :
			Devices::Device(name, deviceTypeAccelerationSensor3DOF(Devices::Sensor::SENSOR_ACCELERATION_3DOF)),
			Devices::Measurement(name, deviceTypeAccelerationSensor3DOF(Devices::Sensor::SENSOR_ACCELERATION_3DOF)),
			Devices::Sensor(name, deviceTypeAccelerationSensor3DOF(Devices::Sensor::SENSOR_ACCELERATION_3DOF)),
			Devices::AccelerationSensor3DOF(name, Devices::Sensor::SENSOR_ACCELERATION_3DOF)
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
		 * Adds a new sample to the sensor.
		 * @param timestamp The timestamp of the sample
		 * @param measurements The acceleration measurements
		 */
		void addSample(const Timestamp& timestamp, const Acceleration3DOFSample::Measurements& measurements)
		{
			ObjectIds objectIds;
			objectIds.reserve(measurements.size());

			for (size_t n = 0; n < measurements.size(); ++n)
			{
				objectIds.emplaceBack(ObjectId(n));
			}

			postNewSample(SampleRef(new Acceleration3DOFSample(timestamp, std::move(objectIds), measurements)));
		}
};

bool TestAccelerationSensor3DOF::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("AccelerationSensor3DOF test");
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

TEST(TestAccelerationSensor3DOF, SampleInterpolation)
{
	EXPECT_TRUE(TestAccelerationSensor3DOF::testSampleInterpolation(GTEST_TEST_DURATION));
}

#endif

bool TestAccelerationSensor3DOF::testSampleInterpolation(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing sample() function with all interpolation strategies:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		TestableAccelerationSensor3DOF sensor("Test Acceleration Sensor");

		const unsigned int measurements = RandomI::random(randomGenerator, 1u, 10u);

		Timestamps timestamps;
		Vectors3 accelerations;

		double currentTime = RandomD::scalar(randomGenerator, 1000.0, 2000.0);

		for (unsigned int n = 0u; n < measurements; ++n)
		{
			timestamps.emplace_back(currentTime);

			const Vector3 acceleration = Random::vector3(randomGenerator, Scalar(-20), Scalar(20));

			accelerations.push_back(acceleration);

			sensor.addSample(timestamps.back(), Devices::AccelerationSensor3DOF::Acceleration3DOFSample::Measurements(1, acceleration));

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

				const Devices::Measurement::SampleRef sampleRef = sensor.sample(queryTimestamp, strategy);

				if (!sampleRef)
				{
					OCEAN_SET_FAILED(validation);
					continue;
				}

				const Devices::AccelerationSensor3DOF::Acceleration3DOFSampleRef accelerationSample = sampleRef;

				if (!accelerationSample)
				{
					OCEAN_SET_FAILED(validation);
					continue;
				}

				if (accelerationSample->measurements().size() != 1)
				{
					OCEAN_SET_FAILED(validation);
					continue;
				}

				const Vector3& actualAcceleration = accelerationSample->measurements()[0];

				OCEAN_EXPECT_TRUE(validation, accelerationSample->timestamp().isValid());

				const double returnedTime = double(accelerationSample->timestamp());

				if (strategy == Devices::Measurement::IS_TIMESTAMP_INTERPOLATE)
				{
					if (queryTime <= firstTime)
					{
						OCEAN_EXPECT_EQUAL(validation, returnedTime, firstTime);
						OCEAN_EXPECT_EQUAL(validation, actualAcceleration, accelerations.front());
					}
					else if (queryTime >= lastTime)
					{
						OCEAN_EXPECT_EQUAL(validation, returnedTime, lastTime);
						OCEAN_EXPECT_EQUAL(validation, actualAcceleration, accelerations.back());
					}
					else
					{
						OCEAN_EXPECT_TRUE(validation, NumericD::isWeakEqual(returnedTime, queryTime));

						const Vector3 expectedAcceleration = TestAccelerationSensor3DOF::expectedInterpolatedAcceleration(queryTime, timestamps, accelerations);

						OCEAN_EXPECT_TRUE(validation, actualAcceleration.isEqual(expectedAcceleration, Scalar(0.01)));
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
						OCEAN_EXPECT_EQUAL(validation, actualAcceleration, accelerations[matchedIndex]);
					}
				}
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

Vector3 TestAccelerationSensor3DOF::expectedInterpolatedAcceleration(const double queryTime, const Timestamps& timestamps, const Vectors3& accelerations)
{
	ocean_assert(timestamps.size() == accelerations.size());
	ocean_assert(!timestamps.empty());

	if (timestamps.size() == 1)
	{
		return accelerations.front();
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

	const Vector3& lowerAcceleration = accelerations[lowerIndex];
	const Vector3& upperAcceleration = accelerations[upperIndex];

	const double lowerTime = double(timestamps[lowerIndex]);
	const double upperTime = double(timestamps[upperIndex]);

	const double duration = upperTime - lowerTime;

	if (NumericD::isEqualEps(duration))
	{
		return lowerAcceleration;
	}

	const double factor = (queryTime - lowerTime) / duration;

	return lowerAcceleration * Scalar(1.0 - factor) + upperAcceleration * Scalar(factor);
}

} // namespace TestDevices

} // namespace Test

} // namespace Ocean
