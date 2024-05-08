/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_MAPBUILDING_ON_DEVICE_RELOCALIZER_TRACKER_6DOF_H
#define META_OCEAN_DEVICES_MAPBUILDING_ON_DEVICE_RELOCALIZER_TRACKER_6DOF_H

#include "ocean/devices/mapbuilding/MapBuilding.h"
#include "ocean/devices/mapbuilding/MapBuildingDevice.h"

#include "ocean/base/Thread.h"

#include "ocean/devices/ObjectTracker.h"
#include "ocean/devices/SceneTracker6DOF.h"
#include "ocean/devices/VisualTracker.h"

#include "ocean/tracking/mapbuilding/RelocalizerMono.h"

namespace Ocean
{

namespace Devices
{

namespace MapBuilding
{

/**
 * This class implements an On-Device Relocalizer based on offline created feature maps.
 * @ingroup devicesmapbuilding
 */
class OCEAN_DEVICES_MAPBUILDING_EXPORT OnDeviceRelocalizerTracker6DOF :
	virtual public MapBuildingDevice,
	virtual public SceneTracker6DOF,
	virtual public ObjectTracker,
	virtual public VisualTracker,
	protected Thread
{
	friend class MapBuildingFactory;

	public:

		/**
		 * Adds a new tracking object.
		 * The description must be the url of a file containing the feature map
		 * @see ObjectTracker::registerObject().
		 */
		ObjectId registerObject(const std::string& description, const Vector3& dimension) override;

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
		static inline std::string deviceNameOnDeviceRelocalizerTracker6DOF();

		/**
		 * Returns the type of this tracker.
		 * @return Tracker type
		 */
		static inline DeviceType deviceTypeOnDeviceRelocalizerTracker6DOF();

	private:

		/**
		 * Creates a new 6DOF tracker object.
		 */
		OnDeviceRelocalizerTracker6DOF();

		/**
		 * Destructs a 6DOF tracker object.
		 */
		~OnDeviceRelocalizerTracker6DOF() override;

		/**
		 * Thread function.
		 * @see Thread::threadRun().
		 */
		void threadRun() override;

	private:

		/// The map's unique id.
		ObjectId mapObjectId_ = invalidObjectId();

		/// True, if the map is currently tracked.
		bool isMapTracked_ = false;

		/// The reusable frame.
		Frame yFrame_;

		/// The relocalizer to be used to determine the 6-DOF pose.
		Tracking::MapBuilding::RelocalizerMono relocalizer_;

		/// The 3D object points of the relocalizer.
		Vectors3 objectPoints_;

		/// The ids of the object points of the relocalizer.
		Indices64 objectPointIds_;
};

inline std::string OnDeviceRelocalizerTracker6DOF::deviceNameOnDeviceRelocalizerTracker6DOF()
{
	return std::string("On-Device Relocalizer 6DOF Tracker");
}

inline OnDeviceRelocalizerTracker6DOF::DeviceType OnDeviceRelocalizerTracker6DOF::deviceTypeOnDeviceRelocalizerTracker6DOF()
{
	return DeviceType(deviceTypeTracker6DOF(), TRACKER_VISUAL | TRACKER_OBJECT);
}

}

}

}

#endif // META_OCEAN_DEVICES_MAPBUILDING_ON_DEVICE_RELOCALIZER_TRACKER_6DOF_H
