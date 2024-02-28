// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/platform/meta/quest/vrapi/application/NativeApplication.h"

#include "ocean/platform/android/NativeInterfaceManager.h"
#include "ocean/platform/android/Permission.h"
#include "ocean/platform/android/Resource.h"

#include "ocean/platform/linux/Utilities.h"

#include <VrApi_Helpers.h>
#include <VrApi_SystemUtils.h>

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

namespace Application
{

NativeApplication::NativeApplication(struct android_app* androidApp) :
	androidApp_(androidApp),
	deviceType_(Device::deviceType())
{
	ocean_assert(androidApp_ != nullptr);

	ocean_assert(deviceType_ != Device::DT_UNKNOWN);
	Log::debug() << "Running on device: " << Device::deviceName(deviceType_);

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

NativeApplication::~NativeApplication()
{
	// nothing to do here
}

bool NativeApplication::run()
{
	ocean_assert(androidApp_ != nullptr);
	if (androidApp_ == nullptr)
	{
		return false;
	}

	// start up event function e.g., to set window flags
	onStartup();

	ovrJava_.Vm = androidApp_->activity->vm;
	ovrJava_.Vm->AttachCurrentThread(&ovrJava_.Env, nullptr);
	ovrJava_.ActivityObject = androidApp_->activity->clazz;

	ovrInitParms initParms = vrapi_DefaultInitParms(&ovrJava_);

	onPreVrApiInitialize(initParms);

	const int32_t ovrInitResult = vrapi_Initialize(&initParms);

	if (ovrInitResult != VRAPI_INITIALIZE_SUCCESS)
	{
		return false;
	}

	if (!Platform::Android::ResourceManager::get().initialize(androidApp_->activity->vm, androidApp_->activity->clazz, androidApp_->activity->assetManager))
	{
		Log::error() << "Failed to initialize the Android resource manager";

		return false;
	}

	Platform::Android::NativeInterfaceManager::get().setCurrentActivity(androidApp_->activity->clazz);

	ocean_assert(Platform::Android::ResourceManager::get().isValid());

	onVrApiInitialized();

	applicationLoop();

	vrapi_Shutdown();

	ovrJava_.Vm->DetachCurrentThread();

	// we explicitly exit the application here (instead of returning),
	// as the Oculus app does not seem to `terminate` although the application terminates
	// as a result, static variables would stay initialized during two individual run calls

	exit(0);

	// will never happen
	return true;
}

void NativeApplication::applicationLoop()
{
	Log::debug() << "NativeApplication::applicationLoop() started";

	ocean_assert(androidApp_ != nullptr);

	AndroidPermissionMap missingPermissions;
	for (const std::string& permission : androidPermissionsToRequest())
	{
		missingPermissions.emplace(permission, -1);
	}

	bool firstPermissionCheck = true;
	while (androidApp_->destroyRequested == 0)
	{
		// first we process all pending Android events
		processAndroidEvents();

		// check the remaining permissions
		updateGrantedPermissions(missingPermissions, firstPermissionCheck);
		firstPermissionCheck = false;

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
		const int timeoutMilliseconds = (androidApp_->destroyRequested == 0 && !isVRModeEntered()) ? 10 : 0;

		int events = 0;
		struct android_poll_source* eventSource;
		if (ALooper_pollAll(timeoutMilliseconds, nullptr, &events, (void**)&eventSource) < 0)
		{
			break;
		}

		// process event
		if (eventSource != nullptr)
		{
			eventSource->process(androidApp_, eventSource);
		}
	}
}

bool NativeApplication::isVRModeEntered() const
{
	// needs to be implemented in derived classes
	return false;
}

void NativeApplication::showSystemConfirmQuitMenu()
{
	vrapi_ShowSystemUI(&ovrJava_, VRAPI_SYS_UI_CONFIRM_QUIT_MENU);
}

NativeApplication::AndroidPermissionsSet NativeApplication::androidPermissionsToRequest()
{
	return AndroidPermissionsSet();
}

void NativeApplication::onPermissionGranted(const std::string& permission)
{
	Log::debug() << "NativeApplication::onPermissionGranted() with permission '" << permission << "'";
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

void NativeApplication::onPreVrApiInitialize(ovrInitParms& /*initParams*/)
{
	// can be implememted in derived classes

	Log::debug() << "NativeApplication::onPreVrApiInitialize()";
}

void NativeApplication::onVrApiInitialized()
{
	if (!requestPermissions(androidPermissionsToRequest()))
	{
		Log::warning() << "Failed to request some permissions";
	}
}

void NativeApplication::onIdle()
{
	// can be implememted in derived classes
}

bool NativeApplication::hasPermission(const std::string& permission, const bool translate)
{
	ocean_assert(androidApp_ != nullptr && androidApp_->activity != nullptr && androidApp_->activity->vm != nullptr && androidApp_->activity->clazz != nullptr);

	if (permission.empty())
	{
		ocean_assert(false && "Permission string must not be empty");
		return false;
	}

	ocean_assert((permission.rfind("com.oculus.permission.") == std::string::npos || !translate) && "For Oculus permissions `translate` must be `false`");

	bool hasPermission = false;

	return Platform::Android::Permission::hasPermission(androidApp_->activity->vm, androidApp_->activity->clazz, permission, hasPermission, translate) && hasPermission;
}

bool NativeApplication::requestPermissions(const AndroidPermissionsSet& permissions)
{
	std::vector<std::string> missingPermissions;

	for (const AndroidPermissionsSet::key_type& permission : permissions)
	{
		if (permission.empty())
		{
			ocean_assert(false && "This should never happen");
			continue;
		}

		if (!hasPermission(permission, /* translate */ false))
		{
			missingPermissions.emplace_back(permission);
		}
	}

	if (!missingPermissions.empty())
	{
		return Platform::Android::Permission::requestPermissions(androidApp_->activity->vm, androidApp_->activity->clazz, missingPermissions, /* translate */ false);
	}

	return true;
}

size_t NativeApplication::updateGrantedPermissions(AndroidPermissionMap& missingPermissions, const bool firstCheck)
{
	if (missingPermissions.empty())
	{
		return 0u;
	}

	AndroidPermissionMap::iterator permissionIter = missingPermissions.begin();

	while (permissionIter != missingPermissions.end())
	{
		ocean_assert(!permissionIter->first.empty());

		if (hasPermission(permissionIter->first, /* translate */ false))
		{
			if (permissionIter->second < 0 && !firstCheck)
			{
				// Apply a delay of a few iterations
				permissionIter->second = 5;
			}

			if (permissionIter->second <= 0)
			{
				onPermissionGranted(permissionIter->first);
				permissionIter = missingPermissions.erase(permissionIter);

				continue;
			}
			else
			{
				--permissionIter->second;
			}
		}

		++permissionIter;
	}

	return missingPermissions.size();
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
