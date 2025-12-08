/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testdevices/TestOrientationTracker3DOF.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/Validation.h"

#include "ocean/devices/OrientationTracker3DOF.h"

#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestDevices
{

/**
 * This class implements a test orientation tracker for testing interpolation.
 */
class TestableOrientationTracker3DOF : public Devices::OrientationTracker3DOF
{
	public:

		/**
		 * Creates a new test orientation tracker.
		 * @param name The name of the tracker
		 */
		explicit TestableOrientationTracker3DOF(const std::string& name) :
			Devices::Device(name, deviceTypeOrientationTracker3DOF()),
			Devices::Measurement(name, deviceTypeOrientationTracker3DOF()),
			Devices::Tracker(name, deviceTypeOrientationTracker3DOF()),
			Devices::OrientationTracker3DOF(name)
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
		 * @param referenceSystem The reference system
		 */
		void addSample(const Timestamp& timestamp, const OrientationTracker3DOFSample::Orientations& orientations, const ReferenceSystem referenceSystem = RS_DEVICE_IN_OBJECT)
		{
			ObjectIds objectIds;
			objectIds.reserve(orientations.size());

			for (size_t n = 0; n < orientations.size(); ++n)
			{
				objectIds.emplaceBack(ObjectId(n));
			}

			postNewSample(SampleRef(new OrientationTracker3DOFSample(timestamp, referenceSystem, std::move(objectIds), orientations)));
		}
};

bool TestOrientationTracker3DOF::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("OrientationTracker3DOF test");
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

TEST(TestOrientationTracker3DOF, SampleInterpolation)
{
	EXPECT_TRUE(TestOrientationTracker3DOF::testSampleInterpolation(GTEST_TEST_DURATION));
}

#endif

bool TestOrientationTracker3DOF::testSampleInterpolation(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing sample() function with all interpolation strategies:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		TestableOrientationTracker3DOF tracker("Test Orientation Tracker");

		const unsigned int measurements = RandomI::random(randomGenerator, 1u, 10u);

		Timestamps timestamps;
		Quaternions orientations;

		double currentTime = RandomD::scalar(randomGenerator, 1000.0, 2000.0);

		for (unsigned int n = 0u; n < measurements; ++n)
		{
			timestamps.emplace_back(currentTime);

			const Quaternion orientation = Random::quaternion(randomGenerator);
			ocean_assert(orientation.isValid());

			orientations.push_back(orientation);

			tracker.addSample(timestamps.back(), Devices::OrientationTracker3DOF::OrientationTracker3DOFSample::Orientations(1, orientation));

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

				const Devices::OrientationTracker3DOF::OrientationTracker3DOFSampleRef orientationSample = sampleRef;

				if (!orientationSample)
				{
					OCEAN_SET_FAILED(validation);
					continue;
				}

				if (orientationSample->orientations().size() != 1)
				{
					OCEAN_SET_FAILED(validation);
					continue;
				}

				const Quaternion& actualOrientation = orientationSample->orientations()[0];

				OCEAN_EXPECT_TRUE(validation, actualOrientation.isValid());

				OCEAN_EXPECT_TRUE(validation, orientationSample->timestamp().isValid());

				const double returnedTime = double(orientationSample->timestamp());

				if (strategy == Devices::Measurement::IS_TIMESTAMP_INTERPOLATE)
				{
					if (queryTime <= firstTime)
					{
						OCEAN_EXPECT_EQUAL(validation, returnedTime, firstTime);

						const Scalar dot = Numeric::abs(actualOrientation.dot(orientations.front()));
						OCEAN_EXPECT_GREATER_EQUAL(validation, dot, Scalar(0.999));
					}
					else if (queryTime >= lastTime)
					{
						OCEAN_EXPECT_EQUAL(validation, returnedTime, lastTime);

						const Scalar dot = Numeric::abs(actualOrientation.dot(orientations.back()));
						OCEAN_EXPECT_GREATER_EQUAL(validation, dot, Scalar(0.999));
					}
					else
					{
						OCEAN_EXPECT_TRUE(validation, NumericD::isWeakEqual(returnedTime, queryTime));

						const Quaternion expectedOrientation = TestOrientationTracker3DOF::expectedInterpolatedOrientation(queryTime, timestamps, orientations);

						const Scalar angleError = Numeric::abs(actualOrientation.angle(expectedOrientation));
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
					}
				}
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

Quaternion TestOrientationTracker3DOF::expectedInterpolatedOrientation(const double queryTime, const Timestamps& timestamps, const Quaternions& orientations)
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

} // namespace TestDevices

} // namespace Test

} // namespace Ocean
