/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_GPS_TRACKER_H
#define META_OCEAN_DEVICES_GPS_TRACKER_H

#include "ocean/devices/Devices.h"
#include "ocean/devices/Tracker.h"

#include "ocean/math/Vector2.h"

namespace Ocean
{

namespace Devices
{

// Forward declaration.
class GPSTracker;

/**
 * Definition of a smart object reference for a GPS tracker.
 * @see GPSTracker.
 * @ingroup devices
 */
typedef SmartDeviceRef<GPSTracker> GPSTrackerRef;

/**
 * This class is the base class for all gps tracker objects.
 * @ingroup devices
 */
class OCEAN_DEVICES_EXPORT GPSTracker : virtual public Tracker
{
	public:

		/**
		 * This class implements a container for a GPS location.
		 */
		class Location
		{
			public:

				/**
				 * Creates an invalid location.
				 */
				inline Location();

				/**
				 * Creates a new GPS location object.
				 * @param latitude The position's latitude, in degree, always valid, with range [-90, 90]
				 * @param longitude The position's longitude, in degree, must be valid, with range [-180, 180]
				 * @param altitude The position's altitude, in meter, NumericF::minValue() if unknown
				 * @param direction The travel direction of the device, relative to north, in degree, north is 0 degree, east is 90 degree, with range [0, 360], -1 if unknown
				 * @param speed The device's speed, in meter per second, with range [0, infinity), -1 if unknown
				 * @param accuracy The horizontal accuracy as radius, in meter, with range [0, infinity), -1 if unknown
				 * @param altitudeAccuracy The vertical accuracy in meter, with range [0, infinity), -1 if unknown
				 * @param directionAccuracy The direction accuracy, in degree, with range [0, 360], -1 if unknown
				 * @param speedAccuracy The speed accuracy, in meter per second, with range [0, infinity), -1 if unknown
				 */
				inline Location(const double latitude, const double longitude, const float altitude = NumericF::minValue(), const float direction = -1.0f, const float speed = -1.0f, const float accuracy = -1.0f, const float altitudeAccuracy = -1.0f, const float directionAccuracy = -1.0f, const float speedAccuracy = -1.0f);

				/**
				 * Returns the latitude of the GPS position.
				 * @return The position's latitude, in degree, always valid, with range [-90, 90].
				 */
				inline double latitude() const;

				/**
				 * Returns the longitude of the GPS position.
				 * @return The position's longitude, in degree, always valid, with range [-180, 180].
				 */
				inline double longitude() const;

				/**
				 * Returns the altitude of the GPS position.
				 * @return The position's altitude, in meter, NumericF::minValue() if unknown
				 */
				inline float altitude() const;

				/**
				 * Returns the device's travel direction not the orientation of the device.
				 * @return The travel direction of the device, relative to north, in degree, north is 0 degree, east is 90 degree, with range [0, 360], -1 if unknown.
				 */
				inline float direction() const;

				/**
				 * Returns the speed of the device.
				 * @return The device's speed, in meter per second, with range [0, infinity), -1 if unknown.
				 */
				inline float speed() const;

				/**
				 * Returns the accuracy of latitude and longitude.
				 * @return The horizontal accuracy as radius, in meter, with range [0, infinity), -1 if unknown.
				 */
				inline float accuracy() const;

				/**
				 * Returns the accuracy of the altitude.
				 * @return The vertical accuracy in meter, with range [0, infinity), -1 if unknown
				 */
				inline float altitudeAccuracy() const;

				/**
				 * Returns the accuracy of the direction.
				 * @return The direction accuracy, in degree, with range [0, 180], -1 if unknown
				 */
				inline float directionAccuracy() const;

				/**
				 * Returns the accuracy of the speed value.
				 * @return The speed accuracy, in meter per second, with range [0, infinity), -1 if unknown.
				 */
				inline float speedAccuracy() const;

				/**
				 * Returns whether this location holds valid data.
				 * @return True, if so
				 */
				inline bool isValid() const;

			protected:

				/// The latitude of the GPS position, in degree, always valid, with range [-90, 90], NumericD::minValue() if invalid
				double latitude_ = NumericD::minValue();

				/// The longitude of the GPS position, in degree, always valid, with range [-180, 180], NumericD::minValue() if invalid
				double longitude_ = NumericD::minValue();

				/// The altitude/height of the GPS position, in meter, NumericF::minValue() if unknown
				float altitude_ = NumericF::minValue();

				/// The device's travel direction not the orientation of the device, relative to north, in degree, north is 0 degree, east is 90 degree, with range [0, 360], -1 if unknown.
				float direction_ = -1.0f;

				/// The speed of the device, in meter per second, with range [0, infinity), -1 if unknown.
				float speed_ = -1.0f;

				/// The horizontal position accuracy as radius, in meter, with range [0, infinity), -1 if unknown.
				float accuracy_ = -1.0f;

				/// The altitude accuracy, in meter, with range [0, infinity), -1 if unknown
				float altitudeAccuracy_ = -1.0f;

				/// The direction accuracy, in degree, with range [0, 180], -1 if unknown
				float directionAccuracy_ = -1.0f;

				/// The accuracy of the speed value, in meter per second, with range [0, infinity), -1 if unknown.
				float speedAccuracy_ = -1.0f;
		};

		/**
			* Definition of a vector holding GPS data values.
			*/
		typedef std::vector<Location> Locations;

		/**
		 * Definition of a sample holding GPS measurements.
		 */
		class OCEAN_DEVICES_EXPORT GPSTrackerSample : virtual public Tracker::TrackerSample
		{
			public:

				/**
				 * Creates a new GPS sample.
				 * @param timestamp Sample timestamp
				 * @param referenceSystem Tracking reference system used by the underlying tracker
				 * @param objectIds The ids of the individual GPS measurements
				 * @param locations The GPS locations, one for each object id
				 * @param metadata Optional metadata of the new sample
				 */
				GPSTrackerSample(const Timestamp& timestamp, const ReferenceSystem referenceSystem, const ObjectIds& objectIds, const Locations& locations, const Metadata& metadata = Metadata());

				/**
				 * Creates a new GPS sample.
				 * @param timestamp Sample timestamp
				 * @param referenceSystem Tracking reference system used by the underlying tracker
				 * @param objectIds The ids of the individual GPS measurements
				 * @param locations The GPS locations, one for each object id
				 * @param metadata Optional metadata of the new sample
				 */
				GPSTrackerSample(const Timestamp& timestamp, const ReferenceSystem referenceSystem, ObjectIds&& objectIds, Locations&& locations, Metadata&& metadata = Metadata());

				/**
				 * Returns the GPS locations.
				 * @return The samples GPS locations, one for each object id.
				 */
				inline const Locations& locations() const;

			protected:

				/// The GPS locations, one for each object id.
				Locations locations_;
		};

		/**
		 * Definition of a smart object reference for GPS tracker samples.
		 */
		typedef SmartObjectRef<GPSTrackerSample, Sample> GPSTrackerSampleRef;

	public:

		/**
		 * Returns the device type of this tracker.
		 * @return Device type
		 */
		static inline DeviceType deviceTypeGPSTracker();

		/**
		 * Parses a string with a GPS location.
		 * The given string must have one of the following patterns:
		 * <pre>
		 * GPS Location {latitude}, {longitude}
		 * GPS Location {latitude}, {longitude}, {altitude}
		 * e.g.,
		 * GPS Location 37.48507, -122.14829
		 * GPS Location 37.48507, -122.14829, 10.0
		 * </pre>
		 * @param gpsString The string to be parsed
		 * @param latitude The resulting latitude value
		 * @param longitude The resulting longitude value
		 * @param altitude Optional resulting altitude; nullptr to not allow an altitude value in a valid input string
		 * @return True, if succeeded; Also True, if the 'altitude' parameter is not nullptr but no altitude value is present in 'gpsString'
		 */
		static bool parseGPSLocation(const std::string& gpsString, double& latitude, double& longitude, double* altitude = nullptr);

		/**
		 * Decodes all GPS locations from a polyline.
		 * Polyline encoding is a lossy compression algorithm that allows you to store a series of coordinates as a single string.<br>
		 * More details: https://developers.google.com/maps/documentation/utilities/polylinealgorithm
		 * @param polyline The polyline to decode, must be valid
		 * @param precision The precision of decimal places the given polyline has, with range [1, 8]
		 * @param locations The resulting individual locations
		 * @param unescapeBackslash True, to unescape two consecutive backslash in the polyline to one backslash; False, to use the polyline as it is
		 * @return True, if succeeded
		 */
		static bool decodePolyline(const std::string& polyline, const unsigned int precision, Locations& locations, const bool unescapeBackslash = false);

		/**
		 * Calculates the distance between two GPS locations in meters, the location's latitude is not considered.
		 * The resulting distance is an approximation based on a perfect sphere.
		 * @param locationA The first GPS location, must be valid
		 * @param locationB The second GPS location, must be valid
		 * @param earthRadius The radius of the earth in meter, with range (0, infinity)
		 * @return The approximated distance between both locations, in meter, with range [0, earthRadius * PI]
		 */
		static double approximatedDistanceBetweenLocations(const Location& locationA, const Location& locationB, const double earthRadius = 6378135.0);

	protected:

		/**
		 * Creates a new GPS tracker object.
		 * @param name The name of the gps tracker
		 */
		explicit GPSTracker(const std::string& name);

		/**
		 * Destructs a GPS tracker object.
		 */
		~GPSTracker() override;

		/**
		 * Decodes one value from a polyline.
		 * @param polyline The polyline from which the value will be decoded, must be valid
		 * @param position The current position within the polyline, will be moved to the next value position, with range [0, polyline.length() - 1]
		 * @param integerValue The resulting value with integer precision (not yet normalized)
		 * @param unescapeBackslash True, to unescape two consecutive backslash in the polyline to one backslash; False, to use the polyline as it is
		 * @return True, if succeeded
		 * @see decodePolyline().
		 */
		static bool decodePolylineValue(const std::string& polyline, size_t& position, int32_t& integerValue, const bool unescapeBackslash);
};

inline GPSTracker::Location::Location()
{
	ocean_assert(!isValid());
}

inline GPSTracker::Location::Location(const double latitude, const double longitude, const float altitude, const float direction, const float speed, const float accuracy, const float altitudeAccuracy, const float directionAccuracy, const float speedAccuracy) :
	latitude_(latitude),
	longitude_(longitude),
	altitude_(altitude),
	direction_(direction),
	speed_(speed),
	accuracy_(accuracy),
	altitudeAccuracy_(altitudeAccuracy),
	directionAccuracy_(directionAccuracy),
	speedAccuracy_(speedAccuracy)
{
	ocean_assert(latitude_ >= -90.0 && latitude_ <= 90.0);
	ocean_assert(longitude_ >= -180.0 && longitude_ <= 180.0);
	ocean_assert((direction_ >= 0.0f && direction_ <= 360.0f) || direction_ == -1.0f);
	ocean_assert((directionAccuracy_ >= 0.0f && directionAccuracy_ <= 180.0f) || directionAccuracy_ == -1.0f);

	ocean_assert(isValid());
}

inline double GPSTracker::Location::latitude() const
{
	return latitude_;
}

inline double GPSTracker::Location::longitude() const
{
	return longitude_;
}

inline float GPSTracker::Location::altitude() const
{
	return altitude_;
}

inline float GPSTracker::Location::direction() const
{
	return direction_;
}

inline float GPSTracker::Location::speed() const
{
	return speed_;
}

inline float GPSTracker::Location::accuracy() const
{
	return accuracy_;
}

inline float GPSTracker::Location::altitudeAccuracy() const
{
	return altitudeAccuracy_;
}

inline float GPSTracker::Location::directionAccuracy() const
{
	return directionAccuracy_;
}

inline float GPSTracker::Location::speedAccuracy() const
{
	return speedAccuracy_;
}

inline bool GPSTracker::Location::isValid() const
{
	ocean_assert((direction_ >= 0.0f && direction_ <= 360.0f) || direction_ == -1.0f);
	ocean_assert((directionAccuracy_ >= 0.0f && directionAccuracy_ <= 180.0f) || directionAccuracy_ == -1.0f);

	return NumericD::isInsideRange(-90.0, latitude_, 90.0) && NumericD::isInsideRange(-180.0, longitude_, 180.0);
}

inline const GPSTracker::Locations& GPSTracker::GPSTrackerSample::locations() const
{
	return locations_;
}

inline GPSTracker::DeviceType GPSTracker::deviceTypeGPSTracker()
{
	return DeviceType(DEVICE_TRACKER, TRACKER_GPS);
}

}

}

#endif // META_OCEAN_DEVICES_GPS_TRACKER_H
