/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testdevices/TestPositionTracker3DOF.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/Validation.h"

#include "ocean/devices/PositionTracker3DOF.h"

#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestDevices
{

/**
 * This class implements a test position tracker for testing interpolation.
 */
class TestablePositionTracker3DOF : public Devices::PositionTracker3DOF
{
	public:

		/**
		 * Creates a new test position tracker.
		 * @param name The name of the tracker
		 */
		explicit TestablePositionTracker3DOF(const std::string& name) :
			Devices::Device(name, deviceTypePositionTracker3DOF()),
			Devices::Measurement(name, deviceTypePositionTracker3DOF()),
			Devices::Tracker(name, deviceTypePositionTracker3DOF()),
			Devices::PositionTracker3DOF(name)
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
		 * @param positions The position measurements
		 * @param referenceSystem The reference system
		 */
		void addSample(const Timestamp& timestamp, const PositionTracker3DOFSample::Positions& positions, const ReferenceSystem referenceSystem = RS_DEVICE_IN_OBJECT)
		{
			ObjectIds objectIds;
			objectIds.reserve(positions.size());

			for (size_t n = 0; n < positions.size(); ++n)
			{
				objectIds.emplaceBack(ObjectId(n));
			}

			postNewSample(SampleRef(new PositionTracker3DOFSample(timestamp, referenceSystem, std::move(objectIds), positions)));
		}
};

bool TestPositionTracker3DOF::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("PositionTracker3DOF test");
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

TEST(TestPositionTracker3DOF, SampleInterpolation)
{
	EXPECT_TRUE(TestPositionTracker3DOF::testSampleInterpolation(GTEST_TEST_DURATION));
}

#endif

bool TestPositionTracker3DOF::testSampleInterpolation(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing sample() function with all interpolation strategies:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		TestablePositionTracker3DOF tracker("Test Position Tracker");

		const unsigned int measurements = RandomI::random(randomGenerator, 1u, 10u);

		Timestamps timestamps;
		Vectors3 positions;

		double currentTime = RandomD::scalar(randomGenerator, 1000.0, 2000.0);

		for (unsigned int n = 0u; n < measurements; ++n)
		{
			timestamps.emplace_back(currentTime);

			const Vector3 position = Random::vector3(randomGenerator, Scalar(-100), Scalar(100));

			positions.push_back(position);

			tracker.addSample(timestamps.back(), Devices::PositionTracker3DOF::PositionTracker3DOFSample::Positions(1, position));

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

				const Devices::PositionTracker3DOF::PositionTracker3DOFSampleRef positionSample = sampleRef;

				if (!positionSample)
				{
					OCEAN_SET_FAILED(validation);
					continue;
				}

				if (positionSample->positions().size() != 1)
				{
					OCEAN_SET_FAILED(validation);
					continue;
				}

				const Vector3& actualPosition = positionSample->positions()[0];

				OCEAN_EXPECT_TRUE(validation, positionSample->timestamp().isValid());

				const double returnedTime = double(positionSample->timestamp());

				if (strategy == Devices::Measurement::IS_TIMESTAMP_INTERPOLATE)
				{
					if (queryTime <= firstTime)
					{
						OCEAN_EXPECT_EQUAL(validation, returnedTime, firstTime);
						OCEAN_EXPECT_EQUAL(validation, actualPosition, positions.front());
					}
					else if (queryTime >= lastTime)
					{
						OCEAN_EXPECT_EQUAL(validation, returnedTime, lastTime);
						OCEAN_EXPECT_EQUAL(validation, actualPosition, positions.back());
					}
					else
					{
						OCEAN_EXPECT_TRUE(validation, NumericD::isWeakEqual(returnedTime, queryTime));

						const Vector3 expectedPosition = TestPositionTracker3DOF::expectedInterpolatedPosition(queryTime, timestamps, positions);

						const Scalar distance = (actualPosition - expectedPosition).length();
						OCEAN_EXPECT_LESS_EQUAL(validation, distance, Scalar(0.01));
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

					OCEAN_EXPECT_GREATER_EQUAL(validation, closestIndices.size(), size_t(1));

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
						OCEAN_EXPECT_EQUAL(validation, actualPosition, positions[matchedIndex]);
					}
				}
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

Vector3 TestPositionTracker3DOF::expectedInterpolatedPosition(const double queryTime, const Timestamps& timestamps, const Vectors3& positions)
{
	ocean_assert(timestamps.size() == positions.size());
	ocean_assert(!timestamps.empty());

	if (timestamps.size() == 1)
	{
		return positions.front();
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

	const Vector3& lowerPosition = positions[lowerIndex];
	const Vector3& upperPosition = positions[upperIndex];

	const double lowerTime = double(timestamps[lowerIndex]);
	const double upperTime = double(timestamps[upperIndex]);

	const double duration = upperTime - lowerTime;

	if (NumericD::isEqualEps(duration))
	{
		return lowerPosition;
	}

	const double factor = (queryTime - lowerTime) / duration;

	return lowerPosition + (upperPosition - lowerPosition) * Scalar(factor);
}

} // namespace TestDevices

} // namespace Test

} // namespace Ocean
