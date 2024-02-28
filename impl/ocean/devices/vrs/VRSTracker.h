// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_DEVICES_VRS_VRS_TRACKER_H
#define META_OCEAN_DEVICES_VRS_VRS_TRACKER_H

#include "ocean/devices/vrs/VRS.h"

#include "ocean/devices/Tracker.h"

namespace Ocean
{

namespace Devices
{

namespace VRS
{

// Forward declaration.
class VRSTracker;

/**
 * Definition of a smart object reference for a VRS tracker.
 * @see VRSTracker.
 * @ingroup devices
 */
typedef SmartDeviceRef<VRSTracker> VRSTrackerRef;

/**
 * This class implements a VRS tracker.
 * @ingroup devicesvrs
 */
class OCEAN_DEVICES_VRS_EXPORT VRSTracker : virtual public Tracker
{
	friend class VRSFactory;

	public:

		/**
		 * Definition of an ordered map mapping descriptions of objects to their internal ids.
		 */
		typedef std::map<std::string, unsigned int> VRSObjectDescriptionMap;

	public:

		/**
		 * Returns whether this device is active.
		 * @see Devices::isStarted().
		 */
		bool isStarted() const override;

		/**
		 * Starts the device.
		 * @see Device::start().
		 */
		bool start() override;

		/**
		 * Stops the device.
		 * @see Device::stop().
		 */
		bool stop() override;

		/**
		 * Returns the name of the owner library.
		 * @see Device::library().
		 */
		const std::string& library() const override;

		/**
		 * Returns whether a specific object is currently actively tracked by this tracker.
		 * @see Tracker::isObjectTracked().
		 */
		bool isObjectTracked(const ObjectId& objectId) const override;

		/**
		 * Updates the description of objects as provided by the VRS recording.
		 * @param vrsObjectDescriptionMap The mappings from object descriptions to internal object ids of all known objects
		 */
		void updateVRSObjects(const VRSObjectDescriptionMap& vrsObjectDescriptionMap);

		/**
		 * Forwards lost and found events.
		 * @param internalFoundObjects The ids of all found objects (specified by internal ids), can be empty
		 * @param internalLostObjects The ids of all lost objects (specified by internal ids), can be empty
		 * @param timestamp The timestamp of the event
		 */
		void forwardObjectEvent(const ObjectIdSet& internalFoundObjects, const ObjectIdSet& internalLostObjects, const Timestamp& timestamp);

	protected:

		/**
		 * Creates a new tracker object.
		 * @param name The name of the tracker
		 * @param type Major and minor device type of the device
		 */
		VRSTracker(const std::string& name, const DeviceType& type);

		/**
		 * Destructs a new VRS tracker object.
		 */
		~VRSTracker() override;

	protected:

		/// True, if the tracker is started.
		bool isStarted_;

		/// The name of the library of this device.
		std::string library_;

		/// The mapper between internal (VRS) object ids and external (unique in the current session) object ids.
		ObjectMapper<unsigned int> objectMapper_;

		/// The set of all currently found objects, specified by external object ids.
		ObjectIdSet currentlyFoundExternalObjectIds_;
};

}

}

}

#endif // META_OCEAN_DEVICES_VRS_VRS_TRACKER_H
