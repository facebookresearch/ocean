// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/PlatformSpecific.h"

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
	#include "ocean/platform/meta/quest/vrapi/HeadsetPoses.h"
#endif

#ifdef OCEAN_PLATFORM_BUILD_ANDROID
	#include "ocean/platform/android/NativeInterfaceManager.h"
#endif

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_PHONE_ANDROID
	#include "ocean/platform/android/Utilities.h"
#endif

namespace Ocean
{

namespace XRPlayground
{

PlatformSpecific::PlatformSpecific()
{
	// nothing to do here
}

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

HomogenousMatrix4 PlatformSpecific::world_T_device(const Timestamp& hostTimestamp) const
{
	return Platform::Meta::Quest::VrApi::HeadsetPoses::world_T_device(ovrMobile_, hostTimestamp);
}

HomogenousMatrix4 PlatformSpecific::world_T_floor() const
{
	return Platform::Meta::Quest::VrApi::HeadsetPoses::world_T_floor(ovrMobile_);
}

Platform::Meta::Quest::VrApi::HandPoses& PlatformSpecific::handPoses()
{
	ocean_assert(handPoses_ != nullptr);
	return *handPoses_;
}

Platform::Meta::Quest::VrApi::TrackedRemoteDevice& PlatformSpecific::trackedRemoteDevice()
{
	ocean_assert(trackedRemoteDevice_ != nullptr);
	return *trackedRemoteDevice_;
}

Platform::Meta::Quest::VrApi::Application::VRControllerVisualizer& PlatformSpecific::vrControllerVisualizer()
{
	ocean_assert(vrControllerVisualizer_ != nullptr);
	return *vrControllerVisualizer_;
}

Platform::Meta::Quest::VrApi::Application::VRHandVisualizer& PlatformSpecific::vrHandVisualizer()
{
	ocean_assert(vrHandVisualizer_ != nullptr);
	return *vrHandVisualizer_;
}

Platform::Meta::Quest::VrApi::Application::MRPassthroughVisualizer& PlatformSpecific::mrPassthroughVisualizer()
{
	ocean_assert(mrPassthroughVisualizer_ != nullptr);
	return *mrPassthroughVisualizer_;
}

ovrMobile* PlatformSpecific::ovr() const
{
	return ovrMobile_;
}

void PlatformSpecific::setHandPoses(Platform::Meta::Quest::VrApi::HandPoses& handPoses)
{
	handPoses_ = &handPoses;
}

void PlatformSpecific::setTrackedRemoteDevice(Platform::Meta::Quest::VrApi::TrackedRemoteDevice& device)
{
	trackedRemoteDevice_ = &device;
}

void PlatformSpecific::setVRControllerVisualizer(Platform::Meta::Quest::VrApi::Application::VRControllerVisualizer& visualizer)
{
	vrControllerVisualizer_ = &visualizer;
}

void PlatformSpecific::setVRHandVisualizer(Platform::Meta::Quest::VrApi::Application::VRHandVisualizer& visualizer)
{
	vrHandVisualizer_ = &visualizer;
}

void PlatformSpecific::setMRPassthroughVisualizer(Platform::Meta::Quest::VrApi::Application::MRPassthroughVisualizer& visualizer)
{
	mrPassthroughVisualizer_ = &visualizer;
}

void PlatformSpecific::setOVR(ovrMobile* ovr)
{
	ovrMobile_ = ovr;
}

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

#ifdef OCEAN_PLATFORM_BUILD_ANDROID

JNIEnv* PlatformSpecific::environment()
{
	return Platform::Android::NativeInterfaceManager::get().environment();
}

jobject PlatformSpecific::currentActivity()
{
	return Platform::Android::NativeInterfaceManager::get().currentActivity();
}

#endif // OCEAN_PLATFORM_BUILD_ANDROID

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_PHONE_ANDROID

bool PlatformSpecific::startActivity(const std::string& activityClassName, const std::string& package)
{
	ocean_assert(!activityClassName.empty());
	ocean_assert(!package.empty() && package.back() == '/');

	JNIEnv* jniEnv = environment();
	jobject rootActivity = currentActivity();

	if (jniEnv == nullptr || rootActivity == nullptr)
	{
		return false;
	}

	return Platform::Android::Utilities::startActivity(jniEnv, rootActivity, package + activityClassName);
}

#endif // XR_PLAYGROUND_PLATFORM_BUILD_PHONE_ANDROID

}

}
