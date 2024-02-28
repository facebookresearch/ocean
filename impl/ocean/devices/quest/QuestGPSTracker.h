// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_DEVICES_QUEST_QUEST_GPS_TRACKER_H
#define META_OCEAN_DEVICES_QUEST_QUEST_GPS_TRACKER_H

#include "ocean/devices/quest/Quest.h"

#include "ocean/devices/GPSTracker.h"

#include <vros/sys/location/LocationManager.h>
#include <vros/sys/location/LocationCallback.h>

namespace Ocean
{

namespace Devices
{

namespace Quest
{

/**
 * This class implements a GPS tracker for Quest platforms.
 * @ingroup devicesquest
 */
class OCEAN_DEVICES_QUEST_EXPORT QuestGPSTracker :
	virtual public GPSTracker,
	virtual protected OSSDK::Location::v2::LocationCallback
{
	friend class QuestFactory;

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
		 * Returns the name of the owner library.
		 * @see Device::library().
		 */
		const std::string& library() const override;

		/**
		 * Returns the name of this tracker.
		 * @return The tracker's name
		 */
		static inline std::string deviceNameQuestGPSTracker();

		/**
		 * Returns the device type of this tracker.
		 * @return The tracker's device type
		 */
		static inline DeviceType deviceTypeQuestGPSTracker();

	protected:

		/**
		 * Creates a new GPS tracker device.
		 */
		QuestGPSTracker();

		/**
		 * Destructs a GPS tracker device.
		 */
		~QuestGPSTracker() override;

		/**
		 * Event callback function for new location events.
		 * @param location The new location
		 */
		void onLocation(OSSDK::Location::v2::Location& location) override;

	protected:

		/// The Sensor location manager.
		std::shared_ptr<OSSDK::Location::v2::ILocationManager> locationManager_;

		/// The timestamp of the last GPS signal;
		Timestamp lastTimestamp_ = Timestamp(false);

		/// The unique id for the world object.
		ObjectId gpsObjectId_ = invalidObjectId();
};

inline std::string QuestGPSTracker::deviceNameQuestGPSTracker()
{
	return std::string("Quest GPS Tracker");
}

inline QuestGPSTracker::DeviceType QuestGPSTracker::deviceTypeQuestGPSTracker()
{
	return deviceTypeGPSTracker();
}

}

}

}

#endif // META_OCEAN_DEVICES_QUEST_QUEST_GPS_TRACKER_H
