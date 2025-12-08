/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testdevices/TestTracker6DOF.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/Validation.h"

#include "ocean/devices/Tracker6DOF.h"

#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestDevices
{

/**
 * This class implements a test 6DOF tracker for testing interpolation.
 */
class TestableTracker6DOF : public Devices::Tracker6DOF
{
	public:

		/**
		 * Creates a new test 6DOF tracker.
		 * @param name The name of the tracker
		 */
		explicit TestableTracker6DOF(const std::string& name) :
			Devices::Device(name, deviceTypeTracker6DOF()),
			Devices::Measurement(name, deviceTypeTracker6DOF()),
			Devices::Tracker(name, deviceTypeTracker6DOF()),
			Devices::OrientationTracker3DOF(name),
			Devices::PositionTracker3DOF(name),
			Devices::Tracker6DOF(name)
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
		 * @param orientations The orientation measurements
		 * @param positions The position measurements
		 * @param referenceSystem The reference system
		 */
		void addSample(const Timestamp& timestamp, const Tracker6DOFSample::Orientations& orientations, const Tracker6DOFSample::Positions& positions, const ReferenceSystem referenceSystem = RS_DEVICE_IN_OBJECT)
		{
			ocean_assert(orientations.size() == positions.size());

			ObjectIds objectIds;
			objectIds.reserve(orientations.size());

			for (size_t n = 0; n < orientations.size(); ++n)
			{
				objectIds.emplaceBack(ObjectId(n));
			}

			postNewSample(SampleRef(new Tracker6DOFSample(timestamp, referenceSystem, std::move(objectIds), orientations, positions)));
		}
};

bool TestTracker6DOF::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("Tracker6DOF test");
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

TEST(TestTracker6DOF, SampleInterpolation)
{
	EXPECT_TRUE(TestTracker6DOF::testSampleInterpolation(GTEST_TEST_DURATION));
}

#endif

bool TestTracker6DOF::testSampleInterpolation(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing sample() function with all interpolation strategies:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		TestableTracker6DOF tracker("Test 6DOF Tracker");

		const unsigned int measurements = RandomI::random(randomGenerator, 1u, 10u);

		Timestamps timestamps;
		Quaternions orientations;
		Vectors3 positions;

		double currentTime = RandomD::scalar(randomGenerator, 1000.0, 2000.0);

		for (unsigned int n = 0u; n < measurements; ++n)
		{
			timestamps.emplace_back(currentTime);

			const Quaternion orientation = Random::quaternion(randomGenerator);
			ocean_assert(orientation.isValid());

			orientations.push_back(orientation);

			const Vector3 position = Random::vector3(randomGenerator, Scalar(-100), Scalar(100));

			positions.push_back(position);

			tracker.addSample(timestamps.back(), Devices::Tracker6DOF::Tracker6DOFSample::Orientations(1, orientation), Devices::Tracker6DOF::Tracker6DOFSample::Positions(1, position));

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

				const Devices::Tracker6DOF::Tracker6DOFSampleRef tracker6DOFSample = sampleRef;

				if (!tracker6DOFSample)
				{
					OCEAN_SET_FAILED(validation);
					continue;
				}

				if (tracker6DOFSample->orientations().size() != 1 || tracker6DOFSample->positions().size() != 1)
				{
					OCEAN_SET_FAILED(validation);
					continue;
				}

				const Quaternion& actualOrientation = tracker6DOFSample->orientations()[0];
				const Vector3& actualPosition = tracker6DOFSample->positions()[0];

				OCEAN_EXPECT_TRUE(validation, actualOrientation.isValid());

				OCEAN_EXPECT_TRUE(validation, tracker6DOFSample->timestamp().isValid());

				const double returnedTime = double(tracker6DOFSample->timestamp());

				if (strategy == Devices::Measurement::IS_TIMESTAMP_INTERPOLATE)
				{
					if (queryTime <= firstTime)
					{
						OCEAN_EXPECT_EQUAL(validation, returnedTime, firstTime);

						const Scalar dot = Numeric::abs(actualOrientation.dot(orientations.front()));
						OCEAN_EXPECT_GREATER_EQUAL(validation, dot, Scalar(0.999));

						OCEAN_EXPECT_EQUAL(validation, actualPosition, positions.front());
					}
					else if (queryTime >= lastTime)
					{
						OCEAN_EXPECT_EQUAL(validation, returnedTime, lastTime);

						const Scalar dot = Numeric::abs(actualOrientation.dot(orientations.back()));
						OCEAN_EXPECT_GREATER_EQUAL(validation, dot, Scalar(0.999));

						OCEAN_EXPECT_EQUAL(validation, actualPosition, positions.back());
					}
					else
					{
						OCEAN_EXPECT_TRUE(validation, NumericD::isWeakEqual(returnedTime, queryTime));

						const Quaternion expectedOrientation = TestTracker6DOF::expectedInterpolatedOrientation(queryTime, timestamps, orientations);
						const Vector3 expectedPosition = TestTracker6DOF::expectedInterpolatedPosition(queryTime, timestamps, positions);

						const Scalar angleError = Numeric::abs(actualOrientation.angle(expectedOrientation));
						OCEAN_EXPECT_LESS_EQUAL(validation, angleError, Numeric::deg2rad(Scalar(0.5)));

						const Scalar positionDistance = (actualPosition - expectedPosition).length();
						OCEAN_EXPECT_LESS_EQUAL(validation, positionDistance, Scalar(0.01));
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
						// For quaternions, check considering double-cover
						const Scalar dot = Numeric::abs(actualOrientation.dot(orientations[matchedIndex]));
						OCEAN_EXPECT_GREATER_EQUAL(validation, dot, Scalar(0.999));

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

Quaternion TestTracker6DOF::expectedInterpolatedOrientation(const double queryTime, const Timestamps& timestamps, const Quaternions& orientations)
{
	ocean_assert(timestamps.size() == orientations.size());
	ocean_assert(!timestamps.empty());

	if (timestamps.size() == 1)
	{
		return orientations.front();
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

	const Quaternion& lowerOrientation = orientations[lowerIndex];
	const Quaternion& upperOrientation = orientations[upperIndex];

	const double lowerTime = double(timestamps[lowerIndex]);
	const double upperTime = double(timestamps[upperIndex]);

	const double duration = upperTime - lowerTime;

	if (NumericD::isEqualEps(duration))
	{
		return lowerOrientation;
	}

	const double factor = (queryTime - lowerTime) / duration;

	return lowerOrientation.slerp(upperOrientation, Scalar(factor));
}

Vector3 TestTracker6DOF::expectedInterpolatedPosition(const double queryTime, const Timestamps& timestamps, const Vectors3& positions)
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
