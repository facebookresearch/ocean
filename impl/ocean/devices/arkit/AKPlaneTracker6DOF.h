/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_ARKIT_AK_PLANE_TRACKER_6_DOF_H
#define META_OCEAN_DEVICES_ARKIT_AK_PLANE_TRACKER_6_DOF_H

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
 * This class implements a 6DOF plane tracker based on ARKit.
 * @ingroup devicesarkit
 */
class OCEAN_DEVICES_ARKIT_EXPORT AKPlaneTracker6DOF :
	virtual public AKDevice,
	virtual public SceneTracker6DOF,
	virtual public VisualTracker
{
	friend class AKFactory;

	protected:

		/**
		 * Definition of an unordered map mapping anchor identifier strings to unique ids.
		 */
		typedef std::unordered_map<std::string, Index32> IdentifierMap;

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
		 * @param arFrame The current ARFRame object containing additional data for the sample, must be valid
		 */
		API_AVAILABLE(ios(11.3))
		void onNewSample(const HomogenousMatrix4& world_T_camera, const Timestamp& timestamp, ARFrame* arFrame);

		/**
		 * Event function for a new 6DOF pose.
		 * @param world_T_camera The transformation between camera and world, invalid if unknown/lost
		 * @param sceneElement The scene element which is part of the sample, nullptr if no scene element is known
		 * @param timestamp The timestamp of the new transformation
		 * @param metadata The metadata of the sample
		 */
		void onNewSample(const HomogenousMatrix4& world_T_camera, SharedSceneElement&& sceneElement, const Timestamp& timestamp, Metadata&& metadata);

		/**
		 * Returns the name of this tracker.
		 * @return The trackers's name
		 */
		static inline std::string deviceNameAKPlaneTracker6DOF();

		/**
		 * Returns the device type of this tracker.
		 * @return The tracker's device type
		 */
		static inline DeviceType deviceTypeAKPlaneTracker6DOF();

	protected:

		/**
		 * Creates a new 6DOF plane tracker.
		 */
		explicit AKPlaneTracker6DOF();

		/**
		 * Destructs this 6DOF tracker.
		 */
		~AKPlaneTracker6DOF() override;

	protected:

		/// The unique id for the world object.
		ObjectId worldObjectId_ = invalidObjectId();

		/// True, if the tracker has been started.
		bool isStarted_ = false;

		/// True, if the world object is currently tracked.
		bool worldIsTracked_ = false;

		/// The map mapping unique plane identifier strings to plane ids.
		IdentifierMap identifierMap_;

		/// The counter for unique plane ids.
		unsigned int planeIdCounter_ = 0u;
};

inline std::string AKPlaneTracker6DOF::deviceNameAKPlaneTracker6DOF()
{
	return std::string("ARKit 6DOF Plane Tracker");
}

inline AKPlaneTracker6DOF::DeviceType AKPlaneTracker6DOF::deviceTypeAKPlaneTracker6DOF()
{
	return DeviceType(deviceTypeTracker6DOF(), TRACKER_VISUAL | SCENE_TRACKER_6DOF);
}

}

}

}

#endif // META_OCEAN_DEVICES_ARKIT_AK_PLANE_TRACKER_6_DOF_H
