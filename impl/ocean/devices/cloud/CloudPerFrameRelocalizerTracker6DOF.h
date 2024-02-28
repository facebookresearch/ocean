// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_DEVICES_CLOUD_CLOUD_PER_FRAME_RELOCALIZER_TRACKER_6_DOF_H
#define META_OCEAN_DEVICES_CLOUD_CLOUD_PER_FRAME_RELOCALIZER_TRACKER_6_DOF_H

#include "ocean/devices/cloud/Cloud.h"
#include "ocean/devices/cloud/CloudDevice.h"

#include "ocean/base/Thread.h"

#include "ocean/devices/ObjectTracker.h"
#include "ocean/devices/SceneTracker6DOF.h"
#include "ocean/devices/VisualTracker.h"

namespace Ocean
{

namespace Devices
{

namespace Cloud
{

/**
 * This class implements a 6-DOF tracker based on Cloud relocalization which applies the cloud relocalization on a per-frame bases.
 * The tracker is intended for debugging the precision of the pure relocalization approach not applying any additional tracker like e.g., SLAM/GPS for production usage.
 * @ingroup devicescloud
 */
class CloudPerFrameRelocalizerTracker6DOF final :
	virtual public SceneTracker6DOF,
	virtual public ObjectTracker,
	virtual public VisualTracker,
	virtual public CloudDevice,
	virtual protected Thread
{
	friend class CloudFactory;

	public:

		/**
		 * Adds a new tracking pattern.
		 * For this pattern feature based tracker the pattern must be the url of an image.
		 * @see ObjectTracker::registerObject().
		 */
		ObjectId registerObject(const std::string& description, const Vector3& dimension) override;

		/**
		 * Returns whether a specific object is currently actively tracked by this tracker.
		 * @see Tracker::isObjectTracked().
		 */
		bool isObjectTracked(const ObjectId& objectId) const override;

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
		 * Returns the name of this tracker.
		 * @return Tracker name
		 */
		static inline std::string deviceNameCloudPerFrameRelocalizerTracker6DOF();

		/**
		 * Returns the type of this tracker.
		 * @return Tracker type
		 */
		static inline DeviceType deviceTypeCloudPerFrameRelocalizerTracker6DOF();

	protected:

		/**
		 * Creates a new tracker.
		 */
		CloudPerFrameRelocalizerTracker6DOF();

		/**
		 * Destructs a tracker and releases all resources.
		 */
		~CloudPerFrameRelocalizerTracker6DOF() override;

		/**
		 * The thread function in which the relocalization service is invoked on a regular bases.
		 */
		void threadRun() override;

	protected:

		/// The id of the cloud map.
		ObjectId objectId_ = invalidObjectId();

		/// The description of the cloud map, which is the map's label.
		std::string objectDescription_;

		/// True, if the cloud map object is actively tracked.
		bool objectIsTracked_ = false;
};

inline std::string CloudPerFrameRelocalizerTracker6DOF::deviceNameCloudPerFrameRelocalizerTracker6DOF()
{
	return std::string("Cloud Per-Frame Relocalizer 6DOF Tracker");
}

inline CloudPerFrameRelocalizerTracker6DOF::DeviceType CloudPerFrameRelocalizerTracker6DOF::deviceTypeCloudPerFrameRelocalizerTracker6DOF()
{
	return DeviceType(DEVICE_TRACKER, SCENE_TRACKER_6DOF | TRACKER_VISUAL | TRACKER_OBJECT);
}

}

}

}

#endif // META_OCEAN_DEVICES_CLOUD_CLOUD_PER_FRAME_RELOCALIZER_TRACKER_6_DOF_H
