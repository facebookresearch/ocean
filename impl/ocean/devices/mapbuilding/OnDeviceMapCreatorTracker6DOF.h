/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_MAPBUILDING_ON_DEVICE_MAP_CREATOR_TRACKER_6DOF_H
#define META_OCEAN_DEVICES_MAPBUILDING_ON_DEVICE_MAP_CREATOR_TRACKER_6DOF_H

#include "ocean/devices/mapbuilding/MapBuilding.h"
#include "ocean/devices/mapbuilding/MapBuildingDevice.h"

#include "ocean/base/Thread.h"

#include "ocean/devices/SceneTracker6DOF.h"
#include "ocean/devices/Tracker6DOF.h"
#include "ocean/devices/VisualTracker.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/HomogenousMatrix4.h"

#include "ocean/tracking/mapbuilding/Unified.h"

namespace Ocean
{

namespace Devices
{

namespace MapBuilding
{

/**
 * This class implements an On-Device map creator.
 * @ingroup devicesmapbuilding
 */
class OCEAN_DEVICES_MAPBUILDING_EXPORT OnDeviceMapCreatorTracker6DOF :
	virtual public MapBuildingDevice,
	virtual public SceneTracker6DOF,
	virtual public VisualTracker,
	protected Thread
{
	friend class MapBuildingFactory;

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
		 * Returns whether a specific object is currently actively tracked by this tracker.
		 * @see Tracker::isObjectTracked().
		 */
		bool isObjectTracked(const ObjectId& objectId) const override;

		/**
		 * Exports the determined scene elements.
		 * Supported formats: 'ocean_map'
		 * Supported options: empty or 'optimized'
		 * @see SceneTracker6DOF::exportSceneElements().
		 */
		bool exportSceneElements(const std::string& format, std::ostream& outputStream, const std::string& options = std::string()) const override;

		/**
		 * Returns the name of this tracker.
		 * @return Tracker name
		 */
		static inline std::string deviceNameOnDeviceMapCreatorTracker6DOF();

		/**
		 * Returns the type of this tracker.
		 * @return Tracker type
		 */
		static inline DeviceType deviceTypeOnDeviceMapCreatorTracker6DOF();

	private:

		/**
		 * Creates a new 6DOF tracker object.
		 */
		OnDeviceMapCreatorTracker6DOF();

		/**
		 * Destructs a 6DOF tracker object.
		 */
		~OnDeviceMapCreatorTracker6DOF() override;

		/**
		 * Thread function.
		 * @see Thread::threadRun().
		 */
		void threadRun() override;

		/**
		 * Event function for new tracking samples from the world tracker.
		 * @param measurement The measurement object sending the sample
		 * @param sample The new samples with resulting from world tracker
		 */
		void onWorldTrackerSample(const Measurement* measurement, const SampleRef& sample);

	private:

		/// The object tracking id of the map.
		ObjectId mapObjectId_ = invalidObjectId();

		/// True, if the map is currently tracked; False, if e.g., the SLAM tracker is failing.
		bool isMapTracked_ = false;

		/// The world tracker to be used.
		Devices::Tracker6DOFRef worldTracker_;

		/// The subscription for world tracker sample events.
		SampleEventSubscription worldTrackerSampleSubscription_;

		/// The camera profile from the tracker's last execution.
		mutable SharedAnyCamera lastAnyCamera_;

		/// The tracking database from the tracker's last execution.
		mutable Tracking::Database lastDatabase_;

		/// The descriptor map form the tracker's last execution.
		mutable std::shared_ptr<Tracking::MapBuilding::UnifiedDescriptorMap> lastUnifiedDescriptorMap_;

		/// The recent 3D object points of the current state of the created map.
		Vectors3 recentObjectPoints_;

		/// The recent object point ids of the current state of the created map.
		Indices64 recentObjectPointIds_;

		/// The lock for the recent points.
		Lock pointLock_;
};

inline std::string OnDeviceMapCreatorTracker6DOF::deviceNameOnDeviceMapCreatorTracker6DOF()
{
	return std::string("On-Device Map Creator 6DOF Tracker");
}

inline OnDeviceMapCreatorTracker6DOF::DeviceType OnDeviceMapCreatorTracker6DOF::deviceTypeOnDeviceMapCreatorTracker6DOF()
{
	return DeviceType(deviceTypeTracker6DOF(), SCENE_TRACKER_6DOF | TRACKER_VISUAL);
}

}

}

}

#endif // META_OCEAN_DEVICES_MAPBUILDING_ON_DEVICE_MAP_CREATOR_TRACKER_6DOF_H
