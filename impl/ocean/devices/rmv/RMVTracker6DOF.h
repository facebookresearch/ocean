/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_RMV_TRACKER_6DOF_H
#define META_OCEAN_DEVICES_RMV_TRACKER_6DOF_H

#include "ocean/devices/rmv/RMV.h"
#include "ocean/devices/rmv/RMVDevice.h"

#include "ocean/base/Thread.h"

#include "ocean/devices/ObjectTracker.h"
#include "ocean/devices/Tracker6DOF.h"
#include "ocean/devices/VisualTracker.h"

#include "ocean/geometry/Geometry.h"

#include "ocean/tracking/rmv/RMVFeatureTracker6DOF.h"

namespace Ocean
{

namespace Devices
{

namespace RMV
{

/**
 * This class implements an RMV feature based tracker.
 * @ingroup devicesrmv
 */
class OCEAN_DEVICES_RMV_EXPORT RMVTracker6DOF :
	virtual public RMVDevice,
	virtual public Tracker6DOF,
	virtual public ObjectTracker,
	virtual public VisualTracker,
	protected Thread
{
	friend class RMVFactory;

	public:

		/**
		 * Adds a new tracking pattern.
		 * For this RMV feature based tracker the pattern must be the url of an image.
		 * @see ObjectTracker::registerObject().
		 */
		ObjectId registerObject(const std::string& name, const Vector3& dimension) override;

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
		 * Returns whether a specific object is currently actively tracked by this tracker.
		 * @see Tracker::isObjectTracked().
		 */
		bool isObjectTracked(const ObjectId& objectId) const override;

		/**
		 * Returns the name of this tracker.
		 * @return Tracker name
		 */
		static inline std::string deviceNameRMVTracker6DOF();

		/**
		 * Returns the device type of this tracker.
		 * @return Device type
		 */
		static inline DeviceType deviceTypeRMVTracker6DOF();

	private:

		/**
		 * Creates a new RMV feature based 6DOF tracker object.
		 */
		RMVTracker6DOF();

		/**
		 * Destructs an RMV feature based 6DOF tracker object.
		 */
		~RMVTracker6DOF() override;

		/**
		 * Thread function.
		 * @see Thread::threadRun().
		 */
		void threadRun() override;

	private:

		/// Frame timestamp.
		Timestamp frameTimestamp_;

		/// Internal 6DOF tracker object.
		Tracking::RMV::RMVFeatureTracker6DOF featureTracker_;

		/// Random generator.
		RandomGenerator randomGenerator_;

		/// The unique object id of this tracker.
		ObjectId uniqueObjectId_ = invalidObjectId();
};

inline std::string RMVTracker6DOF::deviceNameRMVTracker6DOF()
{
	return std::string("RMV Feature Based 6DOF Tracker");
}

inline RMVTracker6DOF::DeviceType RMVTracker6DOF::deviceTypeRMVTracker6DOF()
{
	return DeviceType(RMVTracker6DOF::deviceTypeTracker6DOF(), TRACKER_VISUAL | TRACKER_OBJECT);
}

}

}

}

#endif // META_OCEAN_DEVICES_RMV_TRACKER_6DOF_H
