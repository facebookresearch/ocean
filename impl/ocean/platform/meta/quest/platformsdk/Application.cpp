/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/meta/quest/platformsdk/Application.h"
#include "ocean/platform/meta/quest/platformsdk/Manager.h"
#include "ocean/platform/meta/quest/platformsdk/Utilities.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace PlatformSDK
{

Application::Application()
{
	messageScopedSubscriptions_.emplace_back(subscribeForMessageResponse(ovrMessage_Notification_ApplicationLifecycle_LaunchIntentChanged, &Application::onLaunchIntentChangedNotification));
}

void Application::ensureInitialization()
{
	// nothing to do here, just ensuring that the instance exists
}

bool Application::hasLaunchDeepLink(std::string& deepLink, std::string* sessionId) const
{
	const ScopedLock scopedLock(lock_);

	if (!Manager::get().isInitialized())
	{
		ocean_assert(false && "Manager not yet initialized!");
		return false;
	}

	if (launchDeepLink_.empty())
	{
		return false;
	}

	deepLink = launchDeepLink_;

	if (sessionId != nullptr)
	{
		*sessionId = launchSessionId_;
	}

	return true;
}

bool Application::hasChangedLaunchDeepLink(std::string& changedDeepLink, std::string* sessionId)
{
	const ScopedLock scopedLock(lock_);

	if (changedLaunchDeepLink_.empty())
	{
		return false;
	}

	changedDeepLink = std::move(changedLaunchDeepLink_);
	changedLaunchDeepLink_ = std::string();

	if (sessionId != nullptr)
	{
		*sessionId = std::move(changedLaunchSessionId_);
		changedLaunchSessionId_ = std::string();
	}

	return true;
}

std::future<bool> Application::launchAnotherApplication(const uint64_t appId, const std::string& deepLink)
{
	ocean_assert(appId != 0ull);

	const ScopedOptions<ovrApplicationOptionsHandle> options(ovr_ApplicationOptions_Create(), ovr_ApplicationOptions_Destroy);

	if (!deepLink.empty())
	{
		ovr_ApplicationOptions_SetDeeplinkMessage(options, deepLink.c_str());
	}

	if (!invokeRequest(ovr_Application_LaunchOtherApp(appId, options), &Application::onLaunchedAnotherApplication))
	{
		return std::future<bool>();
	}

	return launchAnotherApplicationRequestQueue_.addRequest();
}

void Application::checkLaunchDeepLink()
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(Manager::get().isInitialized());

	ovrLaunchDetailsHandle launchDetailsHandle = ovr_ApplicationLifecycle_GetLaunchDetails();
	const char* deepLink = ovr_LaunchDetails_GetDeeplinkMessage(launchDetailsHandle);
	const char* sessionId = ovr_LaunchDetails_GetLobbySessionID(launchDetailsHandle);

	if (deepLink != nullptr)
	{
		launchDeepLink_ = deepLink;

		if (sessionId != nullptr)
		{
			launchSessionId_ = sessionId;
		}
		else
		{
			launchSessionId_.clear();
		}

		ocean_assert(launchDeepLinkTimestamp_.isInvalid());
		launchDeepLinkTimestamp_.toNow();
	}
}

void Application::onLaunchIntentChangedNotification(ovrMessage* message, const bool succeeded)
{
	if (succeeded)
	{
		const Timestamp currentTimestamp(true);

		const ScopedLock scopedLock(lock_);

		if (currentTimestamp >= launchDeepLinkTimestamp_ + 0.5)
		{
			// the new (re-)launch deep link has arrived long after the initial launch deep link

			ovrLaunchDetailsHandle launchDetailsHandle = ovr_ApplicationLifecycle_GetLaunchDetails();
			const char* deepLink = ovr_LaunchDetails_GetDeeplinkMessage(launchDetailsHandle);
			const char* sessionId = ovr_LaunchDetails_GetLobbySessionID(launchDetailsHandle);

			if (deepLink != nullptr)
			{
				changedLaunchDeepLink_ = deepLink;

				if (sessionId != nullptr)
				{
					changedLaunchSessionId_ = sessionId;
				}
				else
				{
					changedLaunchSessionId_.clear();
				}
			}
		}
	}
}

void Application::onLaunchedAnotherApplication(ovrMessage* message, const bool succeeded)
{
	if (succeeded)
	{
		Log::debug() << "Succeeded to launch another application";
	}
	else
	{
		Log::error() << "Failed to launch another application: " << Utilities::errorMessage(message);
	}

	launchAnotherApplicationRequestQueue_.setResponse(succeeded);
}

} // namespace PlatformSDK

} // namespace Quest

} // namespace Meta

} // namespace Platform

} // namespace Ocean
