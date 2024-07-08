/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testdevices/TestGPSTracker.h"

#include "ocean/devices/GPSTracker.h"

#include "ocean/math/Random.h"
#include "ocean/math/Sphere3.h"

namespace Ocean
{

namespace Test
{

namespace TestDevices
{

bool TestGPSTracker::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Test GPSTracker:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testParseGPSLocation(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDecodePolyline() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testApproximatedDistanceBetweenLocations(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "GPSTracker test succeeded.";
	}
	else
	{
		Log::info() << "GPSTracker test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestGPSTracker, ParseGPSLocation)
{
	EXPECT_TRUE(TestGPSTracker::testParseGPSLocation(GTEST_TEST_DURATION));
}

TEST(TestGPSTracker, DecodePolyline)
{
	EXPECT_TRUE(TestGPSTracker::testDecodePolyline());
}

TEST(TestGPSTracker, ApproximatedDistanceBetweenLocations)
{
	EXPECT_TRUE(TestGPSTracker::testApproximatedDistanceBetweenLocations(GTEST_TEST_DURATION));
}

#endif

bool TestGPSTracker::testParseGPSLocation(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing parseGPSLocation():";

	RandomGenerator randomGenerator;
	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	{
		// some fixed inputs

		double parsedLatitude;
		double parsedLongitude;
		double parsedAltitude;

		if (Devices::GPSTracker::parseGPSLocation("GPS Location ,", parsedLatitude, parsedLongitude)
				|| Devices::GPSTracker::parseGPSLocation("GPS Location ,", parsedLatitude, parsedLongitude, &parsedAltitude))
		{
			allSucceeded = false;
		}

		if (Devices::GPSTracker::parseGPSLocation("GPS Location ,,", parsedLatitude, parsedLongitude)
				|| Devices::GPSTracker::parseGPSLocation("GPS Location ,,", parsedLatitude, parsedLongitude, &parsedAltitude))
		{
			allSucceeded = false;
		}

		if (Devices::GPSTracker::parseGPSLocation("GPS Location , ,", parsedLatitude, parsedLongitude)
				|| Devices::GPSTracker::parseGPSLocation("GPS Location , ,", parsedLatitude, parsedLongitude, &parsedAltitude))
		{
			allSucceeded = false;
		}

		if (Devices::GPSTracker::parseGPSLocation("GPS Location 0.1234, ", parsedLatitude, parsedLongitude)
				|| Devices::GPSTracker::parseGPSLocation("GPS Location 0.1234, ", parsedLatitude, parsedLongitude, &parsedAltitude))
		{
			allSucceeded = false;
		}

		if (Devices::GPSTracker::parseGPSLocation("GPS Location 0.1234, 0.5678, ", parsedLatitude, parsedLongitude)
				|| Devices::GPSTracker::parseGPSLocation("GPS Location 0.1234, 0.5678, ", parsedLatitude, parsedLongitude, &parsedAltitude))
		{
			allSucceeded = false;
		}
	}

	do
	{
		const double latitude = RandomD::scalar(randomGenerator, -90.0, 90.0);
		const double longitude = RandomD::scalar(randomGenerator, -180.0, 180.0);
		const double altitude = RandomD::scalar(randomGenerator, -50.0, 8000.0);

		{
			// string without altitude

			std::string locationString = "GPS Location " + String::toAString(latitude, 10u) + ", " + String::toAString(longitude, 10u);

			double parsedLatitude = RandomD::scalar(-1000.0, 1000.0);
			double parsedLongitude = RandomD::scalar(-1000.0, 1000.0);
			double parsedAltitude = NumericD::maxValue();

			double* usedParsedAltitude = Random::random(randomGenerator, 1u) == 0u ? &parsedAltitude : nullptr;

			if (Devices::GPSTracker::parseGPSLocation(locationString, parsedLatitude, parsedLongitude, usedParsedAltitude))
			{
				if (!NumericD::isEqual(latitude, parsedLatitude, 0.000001) || !NumericD::isEqual(longitude, parsedLongitude, 0.000001) || parsedAltitude != NumericD::maxValue())
				{
					allSucceeded = false;
				}
			}
			else
			{
				allSucceeded = false;
			}

			// invalid string

			parsedLatitude = RandomD::scalar(-1000.0, 1000.0);
			parsedLongitude = RandomD::scalar(-1000.0, 1000.0);
			parsedAltitude = NumericD::maxValue();

			const unsigned int randomIndex = RandomI::random(randomGenerator, (unsigned int)(locationString.size()) - 1u);
			const char randomChar = char(RandomI::random(randomGenerator, 255));

			if (locationString[randomIndex] != randomChar && (randomChar < '0' || randomChar > '9') && randomChar != '+' && randomChar != '-' && randomChar != 'e' && randomChar != 'E' && randomChar != ' ' && randomChar != ',')
			{
				locationString[randomIndex] = randomChar;

				if (Devices::GPSTracker::parseGPSLocation(locationString, parsedLatitude, parsedLongitude, usedParsedAltitude))
				{
					allSucceeded = false;
				}
			}
		}

		{
			// valid string with altitude

			std::string locationString = "GPS Location " + String::toAString(latitude, 10u) + ", " + String::toAString(longitude, 10u)  + ", " + String::toAString(altitude, 10u);

			double parsedLatitude = RandomD::scalar(-1000.0, 1000.0);
			double parsedLongitude = RandomD::scalar(-1000.0, 1000.0);
			double parsedAltitude = RandomD::scalar(-1000.0, 1000.0);

			double* usedParsedAltitude = Random::random(randomGenerator, 1u) == 0u ? &parsedAltitude : nullptr;

			if (Devices::GPSTracker::parseGPSLocation(locationString, parsedLatitude, parsedLongitude, usedParsedAltitude))
			{
				if (usedParsedAltitude == nullptr)
				{
					allSucceeded = false;
				}
				else if (!NumericD::isEqual(latitude, parsedLatitude, 0.000001) || !NumericD::isEqual(longitude, parsedLongitude, 0.000001) || !NumericD::isEqual(altitude, parsedAltitude, 0.000001))
				{
					allSucceeded = false;
				}
			}
			else
			{
				if (usedParsedAltitude != nullptr)
				{
					allSucceeded = false;
				}
			}

			// invalid string

			parsedLatitude = RandomD::scalar(-1000.0, 1000.0);
			parsedLongitude = RandomD::scalar(-1000.0, 1000.0);
			parsedAltitude = RandomD::scalar(-1000.0, 1000.0);

			const unsigned int randomIndex = RandomI::random(randomGenerator, (unsigned int)(locationString.size()) - 1u);
			const char randomChar = char(RandomI::random(randomGenerator, 255));

			if (locationString[randomIndex] != randomChar && (randomChar < '0' || randomChar > '9') && randomChar != '+' && randomChar != '-' && randomChar != 'e' && randomChar != 'E' && randomChar != ' ')
			{
				locationString[randomIndex] = randomChar;

				if (Devices::GPSTracker::parseGPSLocation(locationString, parsedLatitude, parsedLongitude, usedParsedAltitude))
				{
					allSucceeded = false;
				}
			}
		}
	}
	while(startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation succeeded.";
	}
	else
	{
		Log::info() << "Validation FAILED!";
	}

	return allSucceeded;
}

bool TestGPSTracker::testDecodePolyline()
{
	Log::info() << "Testing decodePolyline():";

	constexpr double threshold = 0.0001;

	bool allSucceeded = true;

	Devices::GPSTracker::Locations locations;
	if (Devices::GPSTracker::decodePolyline("_p~iF~ps|U_ulLnnqC_mqNvxq`@", 5u, locations))
	{
		const VectorsD2 coordinates =
		{
			VectorD2(38.5, -120.2),
			VectorD2(40.7, -120.95),
			VectorD2(43.252, -126.453),
		};

		if (coordinates.size() == locations.size())
		{
			for (size_t n = 0; n < coordinates.size(); ++n)
			{
				if (NumericD::isNotEqual(locations[n].latitude(), coordinates[n][0], threshold) || NumericD::isNotEqual(locations[n].longitude(), coordinates[n][1], threshold))
				{
					allSucceeded = false;
				}
			}
		}
		else
		{

			allSucceeded = false;
		}
	}
	else
	{
		allSucceeded = false;
	}

	locations.clear();
	if (Devices::GPSTracker::decodePolyline("gqxcFnqphVjLFvJGKwM{Kl@aLOSyJxLw@~KUSwHqLh@iKP", 5u, locations))
	{
		const VectorsD2 coordinates =
		{
			VectorD2(37.48644, -122.15080),
			VectorD2(37.48430, -122.15084),
			VectorD2(37.48242, -122.15080),
			VectorD2(37.48248, -122.14844),
			VectorD2(37.48454, -122.14867),
			VectorD2(37.48663, -122.14859),
			VectorD2(37.48673, -122.14670),
			VectorD2(37.48452, -122.14642),
			VectorD2(37.48244, -122.14631),
			VectorD2(37.48254, -122.14475),
			VectorD2(37.48471, -122.14496),
			VectorD2(37.48668, -122.14505)
		};

		if (coordinates.size() == locations.size())
		{
			for (size_t n = 0; n < coordinates.size(); ++n)
			{
				if (NumericD::isNotEqual(locations[n].latitude(), coordinates[n][0], threshold) || NumericD::isNotEqual(locations[n].longitude(), coordinates[n][1], threshold))
				{
					allSucceeded = false;
				}
			}
		}
		else
		{

			allSucceeded = false;
		}
	}
	else
	{
		allSucceeded = false;
	}

	if (allSucceeded)
	{
		Log::info() << "Validation succeeded.";
	}
	else
	{
		Log::info() << "Validation FAILED!";
	}

	return allSucceeded;
}

bool TestGPSTracker::testApproximatedDistanceBetweenLocations(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing approximatedDistanceBetweenLocations():";

	constexpr double earthRadius = 6378135.0; // in meter
	constexpr double earthCircumference = earthRadius * NumericD::pi2();

	RandomGenerator randomGenerator;

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		const double latitudeA = RandomD::scalar(randomGenerator, -90.0, 90.0);
		const double longitudeA = RandomD::scalar(randomGenerator, -180.0, 180.0);

		const Devices::GPSTracker::Location locationA(latitudeA, longitudeA);

		const double randomDistance = RandomD::scalar(randomGenerator, 50.0, 100000.0);
		const double randomAngle = randomDistance / earthCircumference * NumericD::pi2();

		const VectorD3 vectorA = SphereD3::coordinateToVector(NumericD::deg2rad(latitudeA), NumericD::deg2rad(longitudeA));

		// we determine a rotation which rotates 'vectorA' by 'randomAngle'

		const VectorD3 perpendicularVector = QuaternionD(vectorA, RandomD::scalar(randomGenerator, 0, NumericD::pi2())) * vectorA.perpendicular().normalized();
		ocean_assert(vectorA.isOrthogonal(perpendicularVector));
		ocean_assert(perpendicularVector.isUnit());

		const QuaternionD rotation(perpendicularVector, randomAngle);

		const VectorD3 vectorB = rotation * vectorA;

		ocean_assert(NumericD::angleIsEqual(randomAngle, rotation.angle(), NumericD::deg2rad(0.01)));

		double latitudeB = NumericD::minValue();
		double longitudeB = NumericD::minValue();
		SphereD3::vectorToCoordinate(vectorB, latitudeB, longitudeB);

		const Devices::GPSTracker::Location locationB(NumericD::rad2deg(latitudeB), NumericD::rad2deg(longitudeB));

		const double approximatedDistance = Devices::GPSTracker::approximatedDistanceBetweenLocations(locationA, locationB, earthRadius);

		if (NumericD::isEqual(randomDistance, approximatedDistance, 2.0))
		{
			++validIterations;
		}

		++iterations;
	}
	while(startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

} // namespace TestDevices

} // namespace Test

} // namespace Ocean
