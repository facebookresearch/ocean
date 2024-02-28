// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_PLATFORM_SPECIFIC_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_PLATFORM_SPECIFIC_H

#include "application/ocean/xrplayground/XRPlayground.h"

#include "ocean/base/Singleton.h"

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
	#include "ocean/platform/meta/quest/vrapi/HandPoses.h"
	#include "ocean/platform/meta/quest/vrapi/TrackedRemoteDevice.h"

	#include "ocean/platform/meta/quest/vrapi/application/MRPassthroughVisualizer.h"
	#include "ocean/platform/meta/quest/vrapi/application/VRControllerVisualizer.h"
	#include "ocean/platform/meta/quest/vrapi/application/VRHandVisualizer.h"
#endif

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_PHONE_ANDROID
	#include <jni.h>
#endif

namespace Ocean
{

namespace XRPlayground
{

/**
 * This class provides access to some platform specific functionalities.
 * @ingroup xrplayground
 */
class PlatformSpecific : public Singleton<PlatformSpecific>
{
	friend class Singleton<PlatformSpecific>;

	public:

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

		/**
		 * Returns the transformation between devices and world.
		 * @param hostTimestamp The host's timestamp in seconds for which the heaset pose is requested
		 * @return The 6-DOF transformation between device and world
		 */
		HomogenousMatrix4 world_T_device(const Timestamp& hostTimestamp) const;

		/**
		 * Returns the transformation between floor and world.
		 * @return The 6-DOF transformation between floor and world
		 */
		HomogenousMatrix4 world_T_floor() const;

		/**
		 * Returns the HandPoses object on VrApi platforms.
		 * @return The HandPoses object
		 */
		Platform::Meta::Quest::VrApi::HandPoses& handPoses();

		/**
		 * Returns the TrackedRemoteDevice object on VrApi platforms.
		 * @return The TrackedRemoteDevice object
		 */
		Platform::Meta::Quest::VrApi::TrackedRemoteDevice& trackedRemoteDevice();

		/**
		 * Returns the VRControllerVisualizer object on Quest platforms with VrApi.
		 * @return The VRControllerVisualizer object
		 */
		Platform::Meta::Quest::VrApi::Application::VRControllerVisualizer& vrControllerVisualizer();

		/**
		 * Returns the VRHandVisualizer object on Quest platforms with VrApi.
		 * @return The VRHandVisualizer object
		 */
		Platform::Meta::Quest::VrApi::Application::VRHandVisualizer& vrHandVisualizer();

		/**
		 * Returns the MRPassthroughVisualizer object on Quest platforms with VrApi.
		 * @return The MRPassthroughVisualizer object
		 */
		Platform::Meta::Quest::VrApi::Application::MRPassthroughVisualizer& mrPassthroughVisualizer();

		/**
		 * Returns the OVR SDK object.
		 * @return The OVR SDK object
		 */
		ovrMobile* ovr() const;

		/**
		 * Sets the TrackedRemoteDevice object on VrApi platforms.
		 * @param device The object to set
		 */
		void setTrackedRemoteDevice(Platform::Meta::Quest::VrApi::TrackedRemoteDevice& device);

		/**
		 * Sets the HandPoses object on VrApi platforms.
		 * @param device The object to set
		 */
		void setHandPoses(Platform::Meta::Quest::VrApi::HandPoses& handPoses);

		/**
		 * Sets the VRControllerVisualizer object on Quest platforms with VrApi.
		 * @param visualizer The VRControllerVisualizer object to set
		 */
		void setVRControllerVisualizer(Platform::Meta::Quest::VrApi::Application::VRControllerVisualizer& visualizer);

		/**
		 * Sets the VRHandVisualizer object on Quest platforms with VrApi.
		 * @param visualizer The VRControllerVisualizer object to set
		 */
		void setVRHandVisualizer(Platform::Meta::Quest::VrApi::Application::VRHandVisualizer& visualizer);

		/**
		 * Sets the MRPassthroughVisualizer object on Quest platforms with VrApi.
		 * @param visualizer The MRPassthroughVisualizer object to set
		 */
		void setMRPassthroughVisualizer(Platform::Meta::Quest::VrApi::Application::MRPassthroughVisualizer& visualizer);

		/**
		 * Sets the OVR SDK object.
		 * @param ovr The OVR SDK object to set
		 */
		void setOVR(ovrMobile* ovr);

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

#ifdef OCEAN_PLATFORM_BUILD_ANDROID

		/**
		 * Returns the java native interface environment object for the current calling thread.
		 * @return JNI environment object.
		 */
		JNIEnv* environment();

		/**
		 * Returns the current activity.
		 * @return The current activity
		 */
		jobject currentActivity();
#endif // OCEAN_PLATFORM_BUILD_ANDROID

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_PHONE_ANDROID
		/**
		 * Starts an activity.
		 * @param activityClassName The name of the activity class to start, must be valid
		 * @param package The package in which the activity is defined, must be valid and end with '/'
		 * @return True, if succeeded
		 */
		bool startActivity(const std::string& activityClassName, const std::string& package = "com/facebook/ocean/app/xrplayground/android/");

#endif // XR_PLAYGROUND_PLATFORM_BUILD_PHONE_ANDROID

	protected:

		/**
		 * Creates a new platform object.
		 */
		PlatformSpecific();

	protected:

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

		/// The HandPoses object on Quest platforms with VrApi.
		Platform::Meta::Quest::VrApi::HandPoses* handPoses_ = nullptr;

		/// The TrackedRemoteDevice object on Quest platforms with VrApi.
		Platform::Meta::Quest::VrApi::TrackedRemoteDevice* trackedRemoteDevice_ = nullptr;

		/// The VRControllerVisualizer object on Quest platforms with VrApi.
		Platform::Meta::Quest::VrApi::Application::VRControllerVisualizer* vrControllerVisualizer_ = nullptr;

		/// The VRHandVisualizer object on Quest platforms with VrApi.
		Platform::Meta::Quest::VrApi::Application::VRHandVisualizer* vrHandVisualizer_ = nullptr;

		/// The MRPassthroughVisualizer object on Quest platforms with VrApi.
		Platform::Meta::Quest::VrApi::Application::MRPassthroughVisualizer* mrPassthroughVisualizer_ = nullptr;

		/// The OVR SDK object
		ovrMobile* ovrMobile_ = nullptr;

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
};

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_PLATFORM_SPECIFIC_H
