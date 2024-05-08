/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_ARKIT_AK_GEO_ANCHORS_TRACKER_6_DOF_H
#define META_OCEAN_DEVICES_ARKIT_AK_GEO_ANCHORS_TRACKER_6_DOF_H

#include "ocean/devices/arkit/ARKit.h"
#include "ocean/devices/arkit/AKDevice.h"

#include "ocean/devices/ObjectTracker.h"
#include "ocean/devices/Tracker6DOF.h"
#include "ocean/devices/VisualTracker.h"

namespace Ocean
{

namespace Devices
{

namespace ARKit
{

/**
 * This class implements ARKit's 6-DOF Geo Anchors tracker.
 * @ingroup devicesarkit
 */
class OCEAN_DEVICES_ARKIT_EXPORT AKGeoAnchorsTracker6DOF :
	virtual public AKDevice,
	virtual public Tracker6DOF,
	virtual public ObjectTracker,
	virtual public VisualTracker
{
	friend class AKFactory;

	public:

		/**
		 * Definition of an unordered map mapping object ids to transformations.
		 */
		typedef std::unordered_map<ObjectId, HomogenousMatrix4> TransformationMap;

	protected:

		/**
		 * This class stores GPS locations.
		 */
		class Location
		{
			public:

				/**
				 * Creates a new location object.
				 * @param objectId The object id of the location
				 * @param latitude The location's latitude, in degree, with range [-90, 90]
				 * @param longitude The location's longitude, in degree, with range [-180, 180]
				 * @param altitude The location's altitude, in meters, with range (-infinity, infinity), NumericD::minVaue() if unknown
				 */
				inline Location(const ObjectId objectId, const double latitude, const double longitude, const double altitude);

			public:

				/// The location's latitude, in degree, with range [-90, 90].
				double latitude_ = NumericD::minValue();

				/// The location's longitude, in degree, with range [-180, 180].
				double longitude_ = NumericD::minValue();

				/// The location's altitude, in meters, with range (-infinity, infinity), NumericD::minVaue() if unknown.
				double altitude_ = NumericD::minValue();

				/// The object id of the location.
				ObjectId objectId_ = invalidObjectId();
		};

		/**
		 * Definition of a vector holding locations.
		 */
		typedef std::vector<Location> Locations;

	public:

		/**
		 * Starts the device.
		 * @see Device::start().
		 */
		bool start() override;

		/**
		 * Pauses the device.
		 * @see Device::pause().
		 */
		bool pause() override;

		/**
		 * Stops the device.
		 * @see Device::stop().
		 */
		bool stop() override;

		/**
		 * Sets the multi-view visual input of this tracker.
		 * @see VisualTracker::setInput().
		 */
		void setInput(Media::FrameMediumRefs&& frameMediums) override;

		/**
		 * Registers (adds) a new object to be tracked.
		 * @see ObjectTracker::registerObject().
		 */
		ObjectId registerObject(const std::string& description, const Vector3& dimension) override;

		/**
		 * Unregisters (removes) a previously registered object.
		 * @see ObjectTracker::unregisterObject().
		 */
		bool unregisterObject(const ObjectId objectId) override;

		/**
		 * Returns whether a specific object is currently actively tracked by this tracker.
		 * @see Tracker::isObjectTracked().
		 */
		bool isObjectTracked(const ObjectId& objectId) const override;

		/**
		 * Event function for a new 6DOF pose.
		 * @param world_T_camera The transformation between camera and world, invalid if unknown/lost
		 * @param world_T_rotatedWorld The optional transformation between ARKit's rotated world and the standard world, should only be identity or the flip matrix around y-axis
		 * @param timestamp The timestamp of the new transformation
		 * @param arFrame The current ARFRame object containing additional data for the sample, must be valid
		 */
		API_AVAILABLE(ios(14.0))
		void onNewSample(const HomogenousMatrix4& world_T_camera, const HomogenousMatrix4& world_T_rotatedWorld, const Timestamp& timestamp, ARFrame* arFrame);

		/**
		 * Event function for new 6DOF transformations.
		 * @param anchors_T_camera The transformations of all currently known anchors
		 * @param timestamp The timestamp of the new transformation
		 * @param metadata The metadata of the sample
		 */
		void onNewSample(const TransformationMap& anchors_T_camera, const Timestamp& timestamp, Metadata&& metadata);

		/**
		 * Returns the name of this tracker.
		 * @return The trackers's name
		 */
		static inline std::string deviceNameAKGeoAnchorsTracker6DOF();

		/**
		 * Returns the device type of this tracker.
		 * @return The tracker's device type
		 */
		static inline DeviceType deviceTypeAKGeoAnchorsTracker6DOF();

	protected:

		/**
		 * Creates a new 6DOF Geo Anchor tracker.
		 */
		explicit AKGeoAnchorsTracker6DOF();

		/**
		 * Destructs this tracker.
		 */
		~AKGeoAnchorsTracker6DOF() override;

	protected:

		/// The ids of all objects (including the world object) which are currently tracked.
		ObjectIdSet trackedObjectIds_;

		/// True, if the tracker has been started.
		bool isStarted_ = false;

		/// Intermediate locations which have been registered before the device have been started.
		Locations intermediateLocations_;
};

inline AKGeoAnchorsTracker6DOF::Location::Location(const ObjectId objectId, const double latitude, const double longitude, const double altitude) :
	latitude_(latitude),
	longitude_(longitude),
	altitude_(altitude),
	objectId_(objectId)
{
	// nothing to do here
}

inline std::string AKGeoAnchorsTracker6DOF::deviceNameAKGeoAnchorsTracker6DOF()
{
	return std::string("ARKit 6DOF Geo Anchors Tracker");
}

inline AKGeoAnchorsTracker6DOF::DeviceType AKGeoAnchorsTracker6DOF::deviceTypeAKGeoAnchorsTracker6DOF()
{
	return DeviceType(deviceTypeTracker6DOF(), TRACKER_VISUAL);
}

}

}

}

#endif // META_OCEAN_DEVICES_ARKIT_AK_GEO_ANCHORS_TRACKER_6_DOF_H
