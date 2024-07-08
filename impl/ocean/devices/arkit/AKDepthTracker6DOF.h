/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_ARKIT_AK_DEPTH_TRACKER_6_DOF_H
#define META_OCEAN_DEVICES_ARKIT_AK_DEPTH_TRACKER_6_DOF_H

#include "ocean/devices/arkit/ARKit.h"
#include "ocean/devices/arkit/AKDevice.h"

#include "ocean/devices/SceneTracker6DOF.h"
#include "ocean/devices/VisualTracker.h"

namespace Ocean
{

namespace Devices
{

namespace ARKit
{

/**
 * This class implements the 6DOF tracker also delivering depth images.
 * @ingroup devicesarkit
 */
class OCEAN_DEVICES_ARKIT_EXPORT AKDepthTracker6DOF :
	virtual public AKDevice,
	virtual public SceneTracker6DOF,
	virtual public VisualTracker
{
	friend class AKFactory;

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
		 * Returns whether a specific object is currently actively tracked by this tracker.
		 * @see Tracker::isObjectTracked().
		 */
		bool isObjectTracked(const ObjectId& objectId) const override;

		/**
		 * Event function for a new 6DOF pose.
		 * @param world_T_camera The transformation between camera and world, invalid if unknown/lost
		 * @param timestamp The timestamp of the new transformation
		 * @param camera The camera profile defining the projection of the depth information, can have a different resolution, must be valid
		 * @param device_T_depth The transformation between depth image and device, must be valid
		 * @param arFrame The current ARFRame object containing additional data for the sample, must be valid
		 */
		API_AVAILABLE(ios(14.0))
		void onNewSample(const HomogenousMatrix4& world_T_camera, const Timestamp& timestamp, const SharedAnyCamera& camera, const HomogenousMatrix4& device_T_depth, ARFrame* arFrame);

		/**
		 * Returns the name of this tracker.
		 * @return The trackers's name
		 */
		static inline std::string deviceNameAKDepthTracker6DOF();

		/**
		 * Returns the device type of this tracker.
		 * @return The tracker's device type
		 */
		static inline DeviceType deviceTypeAKDepthTracker6DOF();

	protected:

		/**
		 * Creates a new 6DOF depth tracker.
		 */
		explicit AKDepthTracker6DOF();

		/**
		 * Destructs this 6DOF tracker.
		 */
		~AKDepthTracker6DOF() override;

	protected:

		/// The unique id for the world object.
		ObjectId worldObjectId_ = invalidObjectId();

		/// True, if the tracker has been started.
		bool isStarted_ = false;

		/// True, if the world object is currently tracked.
		bool worldIsTracked_ = false;
};

inline std::string AKDepthTracker6DOF::deviceNameAKDepthTracker6DOF()
{
	return std::string("ARKit 6DOF Depth Tracker");
}

inline AKDepthTracker6DOF::DeviceType AKDepthTracker6DOF::deviceTypeAKDepthTracker6DOF()
{
	return DeviceType(deviceTypeTracker6DOF(), TRACKER_VISUAL | SCENE_TRACKER_6DOF);
}

}

}

}

#endif // META_OCEAN_DEVICES_ARKIT_AK_DEPTH_TRACKER_6_DOF_H
