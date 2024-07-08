/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/GPSTracker.h"

#include "ocean/math/Sphere3.h"

namespace Ocean
{

namespace Devices
{

GPSTracker::GPSTrackerSample::GPSTrackerSample(const Timestamp& timestamp, const ReferenceSystem referenceSystem, const ObjectIds& objectIds, const Locations& locations, const Metadata& metadata) :
	Sample(timestamp, objectIds, metadata),
	TrackerSample(timestamp, referenceSystem, objectIds, metadata),
	locations_(locations)
{
	ocean_assert(objectIds_.size() == locations_.size());
}

GPSTracker::GPSTrackerSample::GPSTrackerSample(const Timestamp& timestamp, const ReferenceSystem referenceSystem, ObjectIds&& objectIds, Locations&& locations, Metadata&& metadata) :
	Sample(timestamp, std::move(objectIds), std::move(metadata)),
	TrackerSample(timestamp, referenceSystem, std::move(objectIds), std::move(metadata)),
	locations_(std::move(locations))
{
	ocean_assert(objectIds_.size() == locations_.size());
}

GPSTracker::GPSTracker(const std::string& name) :
	Device(name, deviceTypeGPSTracker()),
	Measurement(name, deviceTypeGPSTracker()),
	Tracker(name, deviceTypeGPSTracker())
{
	// nothing to do here
}

GPSTracker::~GPSTracker()
{
	// nothing to do here
}

bool GPSTracker::parseGPSLocation(const std::string& gpsString, double& latitude, double& longitude, double* altitude)
{
	// description has the following pattern "GPS Location <latitude>, <longitude>"

	constexpr std::string::size_type startTagLength = 13;
	ocean_assert(std::string("GPS Location ").size() == startTagLength);

	if (gpsString.find("GPS Location ") != 0)
	{
		return false;
	}

	const std::string::size_type firstCommaPos = gpsString.find(',', startTagLength);

	if (firstCommaPos == std::string::npos)
	{
		return false;
	}

	if (altitude)
	{
		const std::string::size_type secondCommaPos = gpsString.find(',', firstCommaPos + 1);

		if (secondCommaPos != std::string::npos)
		{
			double localLatitude = NumericD::minValue();
			double localLongitude = NumericD::minValue();
			double localAltitude = NumericD::minValue();

			if (String::isNumber(String::trim(gpsString.substr(startTagLength, firstCommaPos - startTagLength)), true, &localLatitude)
					&& String::isNumber(String::trim(gpsString.substr(firstCommaPos + 1, secondCommaPos - firstCommaPos -1)), true, &localLongitude)
					&& String::isNumber(String::trim(gpsString.substr(secondCommaPos + 1)), true, &localAltitude))
			{
				ocean_assert(localLatitude != NumericD::minValue() && localLongitude != NumericD::minValue() && localAltitude != NumericD::minValue());

				latitude = localLatitude;
				longitude = localLongitude;
				*altitude = localAltitude;

				return true;
			}

			return false;
		}
	}

	// no altitude value allowed

	double localLatitude = NumericD::minValue();
	double localLongitude = NumericD::minValue();

	if (String::isNumber(String::trim(gpsString.substr(startTagLength, firstCommaPos - startTagLength)), true, &localLatitude)
			&& String::isNumber(String::trim(gpsString.substr(firstCommaPos + 1)), true, &localLongitude))
	{
		ocean_assert(localLatitude != NumericD::minValue() && localLongitude != NumericD::minValue());

		latitude = localLatitude;
		longitude = localLongitude;

		return true;
	}

	return false;
}

bool GPSTracker::decodePolyline(const std::string& polyline, const unsigned int precision, Locations& locations, const bool unescapeBackslash)
{
	ocean_assert(!polyline.empty());
	ocean_assert(precision >= 1u && precision <= 8u);

	locations.clear();

	const double normalization = NumericD::pow(10.0, double(precision));

	size_t position = 0;

	while (position < polyline.length())
	{
		int32_t integerValue;
		if (!decodePolylineValue(polyline, position, integerValue, unescapeBackslash))
		{
			return false;
		}

		double latitude = double(integerValue) / normalization;

		if (position >= polyline.length() || !NumericD::isInsideRange(-90.0, latitude, 90.0))
		{
			return false;
		}

		if (!decodePolylineValue(polyline, position, integerValue, unescapeBackslash))
		{
			return false;
		}

		double longitude = double(integerValue) / normalization;

		if (!NumericD::isInsideRange(-180.0, longitude, 180.0))
		{
			return false;
		}

		if (!locations.empty())
		{
			// every coordinate is an offset vector (not the very first location)

			latitude += locations.back().latitude();
			longitude += locations.back().longitude();
		}

		locations.emplace_back(latitude, longitude);
	}

	return true;
}

bool GPSTracker::decodePolylineValue(const std::string& polyline, size_t& position, int32_t& integerValue, const bool unescapeBackslash)
{
	if (position >= polyline.length())
	{
		return false;
	}

	constexpr int32_t asciiOffset = 63;
	constexpr int32_t chunkSize = 5;
	constexpr int32_t bitMaskChunkSize = 0b11111;

	int32_t result = 0;
	int32_t chunkShift = 0;

	while (true)
	{
		ocean_assert(position < polyline.length());
		const char& character = polyline[position++];

		if (unescapeBackslash && character == '\\')
		{
			if (position >= polyline.length())
			{
				return false;
			}

			if (polyline[position] != '\\')
			{
				return false;
			}

			++position;
		}

		const int32_t chunk = ((int32_t(character) - asciiOffset) & bitMaskChunkSize);

		result = result | (chunk << chunkShift);
		chunkShift += chunkSize;

		if (character < 0b100000 + asciiOffset)
		{
			break;
		}

		while (position >= polyline.length())
		{
			return false;
		}
	}

	// reverse two's complement

	if (result & 0x00000001)
	{
		result = ~result;
	}

	integerValue = result >> 1;

	return true;
}

double GPSTracker::approximatedDistanceBetweenLocations(const Location& locationA, const Location& locationB, const double earthRadius)
{
	ocean_assert(locationA.isValid() && locationB.isValid());
	ocean_assert(earthRadius > 0.0);

	return SphereD3::shortestDistance(NumericD::deg2rad(locationA.latitude()), NumericD::deg2rad(locationA.longitude()), NumericD::deg2rad(locationB.latitude()), NumericD::deg2rad(locationB.longitude())) * earthRadius;
}

}

}
