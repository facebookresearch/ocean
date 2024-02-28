// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_PLATFORM_META_QUEST_VRAPI_HEADSET_POSES_H
#define META_OCEAN_PLATFORM_META_QUEST_VRAPI_HEADSET_POSES_H

#include "ocean/platform/meta/quest/vrapi/VrApi.h"

#include "ocean/base/Thread.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/SampleMap.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace VrApi
{

/**
 * This class implements a lookup for most recent headset poses.
 * The class uses the VrAPI function vrapi_GetPredictedTracking2() to determine the most recent tracking samples.
 * @ingroup platformmetaquestvrapi
 */
class HeadsetPoses : protected Thread
{
	protected:

		/**
		 * Definition of a sample map for 6-DOF poses.
		 */
		typedef SampleMap<HomogenousMatrix4> PosesMap;

	public:

		/**
		 * Default constructor.
		 */
		HeadsetPoses();

		/**
		 * Initializes the object and starts the acquisition of poses from the OVR API.
		 * Beware: The given OVR object must be up-to-date (based on a valid most recent vrapi_EnterVrMode() call).
		 * Whenever the VrMode is left, this object needs to be stopped, and can be restarted based on a new OVR object afterwards.
		 * @param ovr The OVR SDK object to be used, must be valid
		 * @return True, if succeeded; False, if the given OVR object was invalid, or if this object is started already
		 * @see stop(), isStarted().
		 */
		bool start(ovrMobile* ovr);

		/**
		 * Stops collecting headset poses and releases all resources.
		 * This function should be called whenever the VrMode is left.
		 * @see start().
		 */
		void stop();

		/**
		 * Returns the headset pose for a spcific timestamp.
		 * The resulting headset pose will be interpolated between both nearest existing headset poses
		 * @param hostTimestamp The host's timestamp in seconds for which the heaset pose is requested
		 * @param world_T_device The resulting headset pose
		 * @return True, if succeeded
		 * @see isStarted().
		 */
		bool interpolatedPose(const Timestamp& hostTimestamp, HomogenousMatrix4& world_T_device) const;

		/**
		 * Returns whether this object is started and ready to be used.
		 * @return True, if so
		 */
		inline bool isStarted() const;

		/**
		 * Returns the transformation between floor and world.
		 * The floor is defined by Guardian (and the user).
		 * @param ovr The OVR SDK object to be used, must be valid
		 * @return The 6-DOF transformation between floor and world
		 */
		static HomogenousMatrix4 world_T_floor(ovrMobile* ovr);

		/**
		 * Returns the transformation between devices and world.
		 * This function is using the same VrApi function as `interpoatedPose()` but does not interpolate or store any poses to improve pose accuracy.<br>
		 * @param ovr The OVR SDK object to be used, must be valid
		 * @param hostTimestamp The host's timestamp in seconds for which the heaset pose is requested
		 * @return The 6-DOF transformation between device and world, invalid if the device is not tracked
		 */
		static HomogenousMatrix4 world_T_device(ovrMobile* ovr, const Timestamp& hostTimestamp);

	protected:

		/**
		 * Internal thread loop.
		 */
		void threadRun() override;

	protected:

		/// The sample map holding the most recent poses.
		PosesMap sampleMap_world_T_device_;

		/// The timestamp of the most recent pose.
		Timestamp sampleMapPreviousTimestamp_;

		/// The OVR SDK object.
		ovrMobile* ovrMobile_;

		/// The lock of this object.
		mutable Lock lock_;
};

inline bool HeadsetPoses::isStarted() const
{
	const ScopedLock scopedLock(lock_);

	return ovrMobile_ != nullptr;
}

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_VRAPI_HEADSET_POSES_H
