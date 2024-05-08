/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_ARKIT_ROOMPLAN_NATIVE_AK_ROOM_PLAN_TRACKER_6_DOF_BASE_H
#define META_OCEAN_DEVICES_ARKIT_ROOMPLAN_NATIVE_AK_ROOM_PLAN_TRACKER_6_DOF_BASE_H

#include "ocean/devices/Devices.h"
#include "ocean/devices/SceneTracker6DOF.h"

#include "ocean/base/Timestamp.h"

#include "ocean/math/HomogenousMatrix4.h"

#include <ARKit/ARKit.h>
#include <Foundation/Foundation.h>

namespace Ocean
{

namespace Devices
{

namespace ARKit
{

/**
 * This class implements the base class for the 6DOF RoomPlan tracker.
 * @ingroup devicesarkit
 */
class AKRoomPlanTracker6DOFBase
{
	public:

		/**
		 * Definition of individual instruction values.
		 */
		enum InstructionValue : uint32_t
		{
			/// Unknown instruction.
			IV_UNKNOWN = 0u,
			/// Guidance instruction is: Move closer to wall.
			IV_MOVE_CLOSE_TO_WALL,
			/// Guidance instruction is: Move away from wall.
			IV_MOVE_AWAY_FROM_WALL,
			/// Guidance instruction is: Slow down.
			IV_SLOW_DOWN,
			/// Guidance instruction is: Tun on light.
			IV_TURN_ON_LIGHT,
			/// Guidance instruction is: Normal.
			IV_NORMAL,
			/// Guidance instruction is: Low texture.
			IV_LOW_TEXTURE
		};

		/**
		 * Re-definitino of PlanarRoomObject.
		 */
		using PlanarRoomObject = SceneTracker6DOF::SceneElementRoom::PlanarRoomObject;

		/**
		 * Re-definition of VolumetricRoomObject.
		 */
		using VolumetricRoomObject = SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject;

		/**
		 * Re-definitino of SharedPlanarRoomObject.
		 */
		using SharedPlanarRoomObject = SceneTracker6DOF::SceneElementRoom::SharedPlanarRoomObject;

		/**
		 * Re-definition of SharedVolumetricRoomObject.
		 */
		using SharedVolumetricRoomObject = SceneTracker6DOF::SceneElementRoom::SharedVolumetricRoomObject;

		/**
		 * Re-definition of SharedPlanarRoomObjects.
		 */
		using SharedPlanarRoomObjects = SceneTracker6DOF::SceneElementRoom::SharedPlanarRoomObjects;

		/**
		 * Re-definition of SharedVolumetricRoomObjects.
		 */
		using SharedVolumetricRoomObjects = SceneTracker6DOF::SceneElementRoom::SharedVolumetricRoomObjects;

	public:

		/**
		 * Event function for a new 6DOF pose.
		 * @param world_T_camera The transformation between camera and world, invalid if unknown/lost
		 * @param timestamp The timestamp of the new transformation
		 * @param arFrame The current ARFRame object containing additional data for the sample, must be valid
		 */
		API_AVAILABLE(ios(11.0))
		virtual void onNewSample(const HomogenousMatrix4& world_T_camera, const Timestamp& timestamp, ARFrame* arFrame) = 0;

		/**
		 * Event function that the capture session has started.
		 */
		virtual void onCaptureSessionStarted() = 0;

		/**
		 * Event function for added room objects.
		 * @param planarRoomObjects The new planar objects
		 * @param volumetricRoomObjects The new volumetric objects
		 */
		virtual void onCaptureSessionAdded(const SharedPlanarRoomObjects& planarRoomObjects, const SharedVolumetricRoomObjects& volumetricRoomObjects) = 0;

		/**
		 * Event function for removed room objects.
		 * @param planarRoomObjects The removed planar objects
		 * @param volumetricRoomObjects The removed volumetric objects
		 */
		virtual void onCaptureSessionRemoved(const SharedPlanarRoomObjects& planarRoomObjects, const SharedVolumetricRoomObjects& volumetricRoomObjects) = 0;

		/**
		 * Event function for changed room objects.
		 * @param planarRoomObjects The changed planar objects
		 * @param volumetricRoomObjects The changed volumetric objects
		 */
		virtual void onCaptureSessionChanged(const SharedPlanarRoomObjects& planarRoomObjects, const SharedVolumetricRoomObjects& volumetricRoomObjects) = 0;

		/**
		 * Event function for updated room objects.
		 * @param planarRoomObjects The updated planar objects
		 * @param volumetricRoomObjects The updated volumetric objects
		 */
		virtual void onCaptureSessionUpdated(const SharedPlanarRoomObjects& planarRoomObjects, const SharedVolumetricRoomObjects& volumetricRoomObjects) = 0;

		/**
		 * Event function for instructions.
		 * @param instructionValue The instruction which can be used to improve the capture result
		 */
		virtual void onCaptureSessionInstruction(const InstructionValue instructionValue) = 0;

		/**
		 * Event function that the capture session has stopped.
		 */
		virtual void onCaptureSessionStopped() = 0;

	protected:

		/**
		 * Translates the instruction value.
		 * @return The translated instruction
		 */
		static std::string translateInstruction(const InstructionValue instructionValue);
};

}

}

}

#endif // META_OCEAN_DEVICES_ARKIT_ROOMPLAN_NATIVE_AK_ROOM_PLAN_TRACKER_6_DOF_BASE_H
