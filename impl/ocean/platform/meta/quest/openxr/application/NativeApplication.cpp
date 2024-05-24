/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/meta/quest/openxr/application/NativeApplication.h"

#include "ocean/platform/android/Permission.h"
#include "ocean/platform/android/ResourceManager.h"

#include "ocean/platform/linux/Utilities.h"

#include "ocean/platform/openxr/Utilities.h"

#ifdef OCEAN_PLATFORM_BUILD_ANDROID
	#ifndef XR_USE_PLATFORM_ANDROID
		#define XR_USE_PLATFORM_ANDROID
	#endif
#endif

#include <openxr/openxr_platform.h>

namespace Ocean
{

namespace Platform
{

using namespace OpenXR;

namespace Meta
{

namespace Quest
{

namespace OpenXR
{

namespace Application
{

#ifdef OCEAN_PLATFORM_BUILD_ANDROID

NativeApplication::NativeApplication(struct android_app* androidApp) :
	androidApp_(androidApp),
	deviceType_(Device::deviceType())
{
	ocean_assert(androidApp_ != nullptr);

	ocean_assert(deviceType_ != Platform::Meta::Quest::Device::DT_UNKNOWN);
	Log::debug() << "Running on device: " << Platform::Meta::Quest::Device::deviceName(deviceType_);

	ocean_assert(androidApp_->userData == nullptr);
	androidApp_->userData = this;

	// set the event function for Android Activity events
	androidApp_->onAppCmd = onAndroidCommand;

#ifdef OCEAN_DEBUG
	if (Platform::Linux::Utilities::checkSecurityEnhancedLinuxStateIsPermissive())
	{
		Log::debug() << "The SELinux state is 'permissive'";
	}
	else
	{
		// permissive can be set with 'adb root && adb shell setenforce 1'
		Log::debug() << "The SELinux state is not 'permissive', thus 'enforcing'";
	}
#endif
}

#endif // OCEAN_PLATFORM_BUILD_ANDROID

NativeApplication::~NativeApplication()
{
	// nothing to do here
}

bool NativeApplication::run()
{

#ifdef OCEAN_PLATFORM_BUILD_ANDROID

	ocean_assert(androidApp_ != nullptr);
	if (androidApp_ == nullptr)
	{
		return false;
	}

#endif // OCEAN_PLATFORM_BUILD_ANDROID

	// start up event function e.g., to set window flags
	onStartup();

#ifdef OCEAN_PLATFORM_BUILD_ANDROID

	ocean_assert(androidApp_ != nullptr);

	ocean_assert(jniEnv_ == nullptr);
	androidApp_->activity->vm->AttachCurrentThread(&jniEnv_, nullptr);

	PFN_xrInitializeLoaderKHR xrInitializeLoaderKHR = nullptr;
	XrResult xrResult = xrGetInstanceProcAddr(XR_NULL_HANDLE, "xrInitializeLoaderKHR", (PFN_xrVoidFunction*)(&xrInitializeLoaderKHR));

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "Failed to get xrInitializeLoaderKHR function: " << int(xrResult);
		return false;
	}

	ocean_assert(xrInitializeLoaderKHR != nullptr);

	XrLoaderInitInfoAndroidKHR loaderInitializeInfoAndroid;
	memset(&loaderInitializeInfoAndroid, 0, sizeof(loaderInitializeInfoAndroid));
	loaderInitializeInfoAndroid.type = XR_TYPE_LOADER_INIT_INFO_ANDROID_KHR;
	loaderInitializeInfoAndroid.next = nullptr;
	loaderInitializeInfoAndroid.applicationVM = androidApp_->activity->vm;
	loaderInitializeInfoAndroid.applicationContext = androidApp_->activity->clazz;

	xrResult = xrInitializeLoaderKHR((XrLoaderInitInfoBaseHeaderKHR*)(&loaderInitializeInfoAndroid));

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "Failed to initialize loader KHR: " << int(xrResult);
		return false;
	}

#endif // defined(ANDROID)

	Platform::OpenXR::Instance::determineApiLayers();

	if (!xrInstance_.initialize(necessaryOpenXRExtensionNames()))
	{
		return false;
	}

	Log::debug() << "OpenXR instance initialized";

	onOpenXRInstanceInitialized();

	XrViewConfigurationViews xrViewConfigurationViews;
	if (!xrInstance_.determineViewConfigurations(xrViewConfigurationType_, xrViewConfigurationViews) || xrViewConfigurationViews.empty())
	{
		Log::error() << "OpenXR: Failed to determine matching view configuration";
		return false;
	}

	Log::debug() << "Determined valid view configuration";

	if (!createOpenXRSession(xrViewConfigurationViews))
	{
		Log::error() << "Failed to initialize OpenXR session";

		return false;
	}

	if (!Platform::Android::ResourceManager::get().initialize(androidApp_->activity->vm, androidApp_->activity->clazz, androidApp_->activity->assetManager))
	{
		Log::error() << "Failed to initialize the Android resource manager";

		return false;
	}

	ocean_assert(Platform::Android::ResourceManager::get().isValid());

	applicationLoop();

	xrInstance_.release();

#ifdef OCEAN_PLATFORM_BUILD_ANDROID

	ocean_assert(androidApp_ != nullptr);
	androidApp_->activity->vm->DetachCurrentThread();

#endif

	// we explicitly exit the application here (instead of returning),
	// as the Oculus app does not seem to `terminate` although the application terminates
	// as a result, static variables would stay initialized during two individual run calls

	exit(0);

	// will never happen
	return true;
}

void NativeApplication::requestAndroidPermission(std::string&& permission)
{
	ocean_assert(!permission.empty());

	if (!permission.empty())
	{
		const ScopedLock scopedLock(androidPermissionLock_);

		if (grantedAndroidPermissionSet_.find(permission) == grantedAndroidPermissionSet_.cend())
		{
			if (pendingAndroidPermissionMap_.find(permission) == pendingAndroidPermissionMap_.cend())
			{
				pendingAndroidPermissionMap_.emplace(std::move(permission), permissionNotYetRequested_);
			}
		}
	}
}

NativeApplication::StringSet NativeApplication::grantedAndroidPermissions() const
{
	const ScopedLock scopedLock(androidPermissionLock_);

	return grantedAndroidPermissionSet_;
}

NativeApplication::StringSet NativeApplication::necessaryOpenXRExtensionNames() const
{
	StringSet extensionNames =
	{
#ifdef OCEAN_PLATFORM_BUILD_ANDROID
		XR_EXT_PERFORMANCE_SETTINGS_EXTENSION_NAME,
		XR_KHR_ANDROID_THREAD_SETTINGS_EXTENSION_NAME,
#endif

		XR_KHR_COMPOSITION_LAYER_COLOR_SCALE_BIAS_EXTENSION_NAME,
		XR_KHR_COMPOSITION_LAYER_CUBE_EXTENSION_NAME,
		XR_KHR_COMPOSITION_LAYER_CYLINDER_EXTENSION_NAME
	};

	return extensionNames;
}

void NativeApplication::applicationLoop()
{
	Log::debug() << "NativeApplication::applicationLoop() started";

	ocean_assert(androidApp_ != nullptr);

	bool firstPermissionCheck = true;
	while (androidApp_->destroyRequested == 0)
	{
		// first we process all pending Android events
		processAndroidEvents();

		// now, we handle pending Android permissions
		handlePendingAndroidPermissions(firstPermissionCheck);
		firstPermissionCheck = false;

		// we process all pending OpenXR events
		processOpenXREvents();

		onIdle();
	}

	Log::debug() << "NativeApplication::applicationLoop() ended";
}

void NativeApplication::processAndroidEvents()
{
	ocean_assert(androidApp_ != nullptr);

	// read all pending events.
	while (true)
	{
		const int timeoutMilliseconds = (androidApp_->destroyRequested == 0 && !xrSessionIsReady_) ? 10 : 0;

		int events = 0;
		struct android_poll_source* eventSource = nullptr;

		int pollResult;
		do
		{
			pollResult = ALooper_pollOnce(timeoutMilliseconds, nullptr, &events, (void**)(&eventSource));
		}
		while (ALOOPER_POLL_CALLBACK == pollResult);

		if (pollResult < 0)
		{
			// No event data needs to be processed or error occurred (pollResult == ALOOPER_POLL_ERROR)
			break;
		}

		// process event
		if (eventSource != nullptr)
		{
			eventSource->process(androidApp_, eventSource);
		}
	}
}

void NativeApplication::processOpenXREvents()
{
	ocean_assert(xrInstance_.isValid());

	XrEventDataBuffer xrEventDataBuffer = {};

	while (true)
	{
		XrEventDataBaseHeader& xrEventDataBaseHeader = (XrEventDataBaseHeader&)(xrEventDataBuffer);
		xrEventDataBaseHeader.type = XR_TYPE_EVENT_DATA_BUFFER;
		xrEventDataBaseHeader.next = nullptr;

		XrResult xrResult = xrPollEvent(xrInstance_, &xrEventDataBuffer);

		if (xrResult != XR_SUCCESS)
		{
			ocean_assert(xrResult == XR_EVENT_UNAVAILABLE);
			break;
		}

		switch (xrEventDataBaseHeader.type)
		{
			case XR_TYPE_EVENT_DATA_EVENTS_LOST:
				// the event queue overflowed
				Log::debug() << "OpenXR: Received XR_TYPE_EVENT_DATA_EVENTS_LOST event";
				break;

			case XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING:
				// the application is about to lose the indicated XrInstance at the indicated lossTime in the future
				Log::debug() << "OpenXR: Received XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING event";
				break;

			case XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED:
				// the active input form factor for one or more top level user paths has changed.
				Log::debug() << "OpenXR: Received XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED event";
				break;

			case XR_TYPE_EVENT_DATA_PERF_SETTINGS_EXT:
				Log::debug() << "OpenXR: Received XR_TYPE_EVENT_DATA_PERF_SETTINGS_EXT event";
				break;

			case XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING:
			{
				// the origin (and perhaps the bounds) of a reference space is changing
				// this may occur due to the user re-centering the space explicitly, or the runtime otherwise switching to a different space definition.
				Log::debug() << "OpenXR: Received XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING event";

				const XrEventDataReferenceSpaceChangePending& xrEventDataReferenceSpaceChangePending = (XrEventDataReferenceSpaceChangePending&)(xrEventDataBaseHeader);

				HomogenousMatrix4 previous_T_changed(false);

				if (xrEventDataReferenceSpaceChangePending.poseValid)
				{
					previous_T_changed = Platform::OpenXR::Utilities::toHomogenousMatrix4(xrEventDataReferenceSpaceChangePending.poseInPreviousSpace);
				}

				onChangedReferenceSpace(xrEventDataReferenceSpaceChangePending.referenceSpaceType, previous_T_changed, xrEventDataReferenceSpaceChangePending.changeTime);

				break;
			}

			case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED:
			{
				const XrEventDataSessionStateChanged& xrEventDataSessionStateChanged = (XrEventDataSessionStateChanged&)(xrEventDataBaseHeader);

				Log::debug() << "OpenXR: Received XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED event, new state is " << Utilities::translateSessionState(xrEventDataSessionStateChanged.state);

				onOpenXRSessionChanged(xrEventDataSessionStateChanged);
				break;
			}

			default:
				Log::debug() << "OpenXR Received unknown event type: " << int(xrEventDataBaseHeader.type);
				break;
		}
	}
}

void NativeApplication::onChangedReferenceSpace(const XrReferenceSpaceType xrReferenceSpaceType, const HomogenousMatrix4& previous_T_changed, const XrTime& changeTime)
{
	Log::debug() << "NativeApplication::onChangedReferenceSpace() for reference space type " << int(xrReferenceSpaceType) << ":";
	Log::debug() << previous_T_changed;
}

void NativeApplication::onOpenXRInstanceInitialized()
{
	Log::debug() << "NativeApplication::onOpenXRInstanceInitialized()";
}

void NativeApplication::onOpenXRSessionChanged(const XrEventDataSessionStateChanged& xrEventDataSessionStateChanged)
{
	xrSessionState_ = xrEventDataSessionStateChanged.state;

	switch (xrSessionState_)
	{
		case XR_SESSION_STATE_UNKNOWN:
			Log::error() << "OpenXR: Unknown session state";
			ocean_assert(false && "This should never happen!");
			break;

		case XR_SESSION_STATE_IDLE:
			// The initial state after calling xrCreateSession or returned to after calling xrEndSession
			break;

		case XR_SESSION_STATE_READY:
			// The application is ready to call xrBeginSession and sync its frame loop with the runtime
		 	xrSessionIsReady_ = true;
			onOpenXRSessionReady();
			break;

		case XR_SESSION_STATE_SYNCHRONIZED:
			// The application has synced its frame loop with the runtime but is not visible to the user
			break;

		case XR_SESSION_STATE_VISIBLE:
			// The application has synced its frame loop with the runtime and is visible to the user but cannot receive XR input (e.g., because an external menu/session is in focus)
			break;

		case XR_SESSION_STATE_FOCUSED:
			// The application has synced its frame loop with the runtime, is visible to the user and can receive XR input
			break;

		case XR_SESSION_STATE_STOPPING:
			// The application should exit its frame loop and call xrEndSession
			onOpenXRSessionStopping();
		 	xrSessionIsReady_ = false;
			break;

		case XR_SESSION_STATE_LOSS_PENDING:
			// The session is in the process of being lost. The application should destroy the current session and can optionally recreate it.
			break;

		case XR_SESSION_STATE_EXITING:
			// The application should end its XR experience and not automatically restart it
			break;

		case XR_SESSION_STATE_MAX_ENUM:
			break;
	}
}

void NativeApplication::onOpenXRSessionReady()
{
	// needs to be implemented in a derived class

	Log::debug() << "NativeApplication::onOpenXRSessionReady()";
}

void NativeApplication::onOpenXRSessionStopping()
{
	// needs to be implemented in a derived class

	Log::debug() << "NativeApplication::onOpenXRSessionStopping()";
}

void NativeApplication::onAndroidPermissionGranted(const std::string& permission)
{
	Log::debug() << "NativeApplication::onAndroidPermissionGranted() with permission '" << permission << "'";
}

void NativeApplication::onActivityStart()
{
	// can be implemented in derived classes

	Log::debug() << "NativeApplication::onActivityStart()";
}

void NativeApplication::onActivityResume()
{
	applicationResumed_ = true;

	Log::debug() << "NativeApplication::onActivityResume()";
}

void NativeApplication::onActivityPause()
{
	applicationResumed_ = false;

	Log::debug() << "NativeApplication::onActivityPause()";
}

void NativeApplication::onActivityStop()
{
	// can be implemented in derived classes

	Log::debug() << "NativeApplication::onActivityStop()";
}

void NativeApplication::onActivityDestroy()
{
	releaseOpenXRSession();

	androidNativeWindow_ = nullptr;

	Log::debug() << "NativeApplication::onActivityDestroy()";
}

void NativeApplication::onActivityInitWindow()
{
	ocean_assert(androidApp_ != nullptr);
	ocean_assert(androidApp_->window != nullptr);

	ocean_assert(androidNativeWindow_ == nullptr);
	androidNativeWindow_ = androidApp_->window;

	Log::debug() << "NativeApplication::onActivityInitWindow()";
}

void NativeApplication::onActivityTermWindow()
{
	androidNativeWindow_ = nullptr;

	Log::debug() << "NativeApplication::onActivityTermWindow()";
}

void NativeApplication::onStartup()
{
	ocean_assert(androidApp_ != nullptr);
	ANativeActivity_setWindowFlags(androidApp_->activity, AWINDOW_FLAG_KEEP_SCREEN_ON, 0);

	Log::debug() << "NativeApplication::onStartup()";
}

bool NativeApplication::isAndroidPermissionGranted(const std::string& permission, const bool translate) const
{
	ocean_assert(androidApp_ != nullptr && androidApp_->activity != nullptr && androidApp_->activity->vm != nullptr && androidApp_->activity->clazz != nullptr);

	if (permission.empty())
	{
		ocean_assert(false && "Permission string must not be empty");
		return false;
	}

	ocean_assert((permission.rfind("com.oculus.permission.") == std::string::npos || !translate) && "For Oculus permissions `translate` must be `false`");

	bool isGranted = false;

	return Platform::Android::Permission::hasPermission(androidApp_->activity->vm, androidApp_->activity->clazz, permission, isGranted, translate) && isGranted;
}

void NativeApplication::handlePendingAndroidPermissions(const bool firstCheck)
{
	const ScopedLock scopedLock(androidPermissionLock_);

	if (pendingAndroidPermissionMap_.empty())
	{
		return;
	}

	std::vector<std::string> notYetRequestedPermissions;

	constexpr int permissionDelay = 5;

	for (AndroidPermissionMap::iterator iPermission = pendingAndroidPermissionMap_.begin(); iPermission != pendingAndroidPermissionMap_.end(); /*noop*/)
	{
		if (iPermission->second == permissionNotYetRequested_)
		{
			if (isAndroidPermissionGranted(iPermission->first, false /*translate*/))
			{
				iPermission->second = 0;
			}
			else
			{
				notYetRequestedPermissions.emplace_back(iPermission->first);

				iPermission->second = permissionNotYetGranted_;
			}
		}
		else if (iPermission->second == permissionNotYetGranted_)
		{
			if (isAndroidPermissionGranted(iPermission->first, false /*translate*/))
			{
				iPermission->second = 0;
			}
		}

		if (iPermission->second >= 0)
		{
			if (iPermission->second >= permissionDelay || firstCheck)
			{
				grantedAndroidPermissionSet_.emplace(iPermission->first);

				onAndroidPermissionGranted(iPermission->first);
				iPermission = pendingAndroidPermissionMap_.erase(iPermission);

				continue;
			}
			else
			{
				iPermission->second++;
			}
		}

		++iPermission;
	}

	if (!notYetRequestedPermissions.empty())
	{
		if (!Platform::Android::Permission::requestPermissions(androidApp_->activity->vm, androidApp_->activity->clazz, notYetRequestedPermissions, /* translate */ false))
		{
			Log::warning() << "Failed to request some Android permissions";
		}
	}
}

void NativeApplication::onAndroidCommand(struct android_app* androidApp, int32_t cmd)
{
	ocean_assert(androidApp != nullptr && androidApp->userData != nullptr);
	if (androidApp == nullptr || androidApp->userData == nullptr)
	{
		return;
	}

	NativeApplication* application = static_cast<NativeApplication*>(androidApp->userData);
	ocean_assert(application != nullptr);

	switch (cmd)
	{
		case APP_CMD_START:
		{
			application->onActivityStart();
			break;
		}

		case APP_CMD_RESUME:
		{
			application->onActivityResume();
			break;
		}

		case APP_CMD_PAUSE:
		{
			application->onActivityPause();
			break;
		}

		case APP_CMD_STOP:
		{
			application->onActivityStop();
			break;
		}

		case APP_CMD_DESTROY:
		{
			application->onActivityDestroy();
			break;
		}

		case APP_CMD_INIT_WINDOW:
		{
			application->onActivityInitWindow();
			break;
		}

		case APP_CMD_TERM_WINDOW:
		{
			application->onActivityTermWindow();
			break;
		}
	}
}

}

}

}

}

}

}
