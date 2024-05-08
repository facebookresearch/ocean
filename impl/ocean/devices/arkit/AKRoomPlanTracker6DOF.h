/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_ARKIT_AK_ROOM_PLAN_TRACKER_6_DOF_H
#define META_OCEAN_DEVICES_ARKIT_AK_ROOM_PLAN_TRACKER_6_DOF_H

#include "ocean/devices/arkit/ARKit.h"

#include "ocean/devices/SceneTracker6DOF.h"
#include "ocean/devices/VisualTracker.h"

#include "ocean/devices/arkit/roomplan/native/AKRoomPlanTracker6DOFBase.h"

#include "ocean/devices/arkit/roomplan/swift/AKRoomPlanTracker6DOF_Objc.h"

namespace Ocean
{

namespace Devices
{

namespace ARKit
{

/**
 * This class implements the 6DOF RoomPlan tracker.
 * @ingroup devicesarkit
 */
class OCEAN_DEVICES_ARKIT_EXPORT AKRoomPlanTracker6DOF :
	virtual public AKRoomPlanTracker6DOFBase,
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
		 * Returns the name of the owner library.
		 * @see Device::library().
		 */
		const std::string& library() const override;

		/**
		 * Returns the name of this tracker.
		 * @return The trackers's name
		 */
		static inline std::string deviceNameAKRoomPlanTracker6DOF();

		/**
		 * Returns the device type of this tracker.
		 * @return The tracker's device type
		 */
		static inline DeviceType deviceTypeAKRoomPlanTracker6DOF();

		/**
		 * Returns whether the tracker is supported on the platform.
		 * @return True, if so
		 */
		static bool isSupported();

	protected:

		/**
		 * Creates a new 6DOF room plan tracker.
		 */
		explicit AKRoomPlanTracker6DOF();

		/**
		 * Destructs this 6DOF tracker.
		 */
		~AKRoomPlanTracker6DOF() override;

		/**
		 * Event function for a new 6DOF pose.
		 * @param world_T_camera The transformation between camera and world, invalid if unknown/lost
		 * @param timestamp The timestamp of the new transformation
		 * @param arFrame The current ARFRame object containing additional data for the sample, must be valid
		 */
		API_AVAILABLE(ios(11.0))
		void onNewSample(const HomogenousMatrix4& world_T_camera, const Timestamp& timestamp, ARFrame* arFrame) override;

		/**
		 * Event function that the capture session has started.
		 */
		void onCaptureSessionStarted() override;

		/**
		 * Event function for added room objects.
		 * @param planarRoomObjects The new planar objects
		 * @param volumetricRoomObjects The new volumetric objects
		 */
		void onCaptureSessionAdded(const SharedPlanarRoomObjects& planarRoomObjects, const SharedVolumetricRoomObjects& volumetricRoomObjects) override;

		/**
		 * Event function for removed room objects.
		 * @param planarRoomObjects The removed planar objects
		 * @param volumetricRoomObjects The removed volumetric objects
		 */
		void onCaptureSessionRemoved(const SharedPlanarRoomObjects& planarRoomObjects, const SharedVolumetricRoomObjects& volumetricRoomObjects) override;

		/**
		 * Event function for changed room objects.
		 * @param planarRoomObjects The changed planar objects
		 * @param volumetricRoomObjects The changed volumetric objects
		 */
		void onCaptureSessionChanged(const SharedPlanarRoomObjects& planarRoomObjects, const SharedVolumetricRoomObjects& volumetricRoomObjects) override;

		/**
		 * Event function for updated room objects.
		 * @param planarRoomObjects The updated planar objects
		 * @param volumetricRoomObjects The updated volumetric objects
		 */
		void onCaptureSessionUpdated(const SharedPlanarRoomObjects& planarRoomObjects, const SharedVolumetricRoomObjects& volumetricRoomObjects) override;

		/**
		 * Event function for instructions.
		 * @param instructionValue The instruction
		 */
		void onCaptureSessionInstruction(const InstructionValue instructionValue) override;

		/**
		 * Event function that the capture session has stopped.
		 */
		void onCaptureSessionStopped() override;

	protected:

		/// The unique id for the world object.
		ObjectId worldObjectId_ = invalidObjectId();

		/// True, if the tracker has been started.
		bool isStarted_ = false;

		/// True, if the world object is currently tracked.
		bool worldIsTracked_ = false;

		/// The Objc wrapper of the actual RoomPlan tracker.
		AKRoomPlanTracker6DOF_Objc* akRoomPlanTracker_ = nullptr;

		/// The latest room objects.
		SceneElementRoom::RoomObjectMap roomObjectMap_;

		/// The identifiers of all added room objects.
		SceneElementRoom::RoomObjectIdentifierSet addedRoomObjects_;

		/// The identifiers of all removed room objects.
		SceneElementRoom::RoomObjectIdentifierSet removedRoomObjects_;

		/// The identifiers of all changed room objects.
		SceneElementRoom::RoomObjectIdentifierSet changedRoomObjects_;

		/// The identifiers of all updated room objects.
		SceneElementRoom::RoomObjectIdentifierSet updatedRoomObjects_;

		/// The latest instructions.
		InstructionValue instructionValue_ = IV_UNKNOWN;
};

inline std::string AKRoomPlanTracker6DOF::deviceNameAKRoomPlanTracker6DOF()
{
	return std::string("ARKit 6DOF Room Plan Tracker");
}

inline AKRoomPlanTracker6DOF::DeviceType AKRoomPlanTracker6DOF::deviceTypeAKRoomPlanTracker6DOF()
{
	return DeviceType(deviceTypeTracker6DOF(), TRACKER_VISUAL);
}

}

}

}

#endif // META_OCEAN_DEVICES_ARKIT_AK_ROOM_PLAN_TRACKER_6_DOF_H
