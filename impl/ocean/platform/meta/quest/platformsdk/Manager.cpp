/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/meta/quest/platformsdk/Manager.h"
#include "ocean/platform/meta/quest/platformsdk/Application.h"
#include "ocean/platform/meta/quest/platformsdk/Multiplayer.h"
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

bool Manager::initialize(jobject activityObject, JNIEnv* jniEnv, const std::string& appId)
{
	ocean_assert(activityObject != nullptr && jniEnv != nullptr && !appId.empty());

	const ScopedLock scopedLock(lock_);

	if (isInitialized_ || initializationPending_)
	{
		return false;
	}

	// we ensure that multiplayer is initialized to be able to catch group presence messages immediately after initialization
	Multiplayer::get().ensureInitialization();

	ocean_assert(!initializationPending_);
	initializationPending_ = true;

	if (!MessageHandler::invokeRequest(ovr_PlatformInitializeAndroidAsynchronous(appId.c_str(), activityObject, jniEnv), &Manager::onInitialized))
	{
		initializationPending_ = false;

		Log::error() << "Failed to initialize platform SDK";

		return false;
	}

	return true;
}

Manager::~Manager()
{
	// nothing to do here
}

void Manager::update(const double timestamp)
{
	const ScopedLock scopedLock(lock_);

	if (!isInitialized_ && !initializationPending_)
	{
		return;
	}

	ovrMessage* message = ovr_PopMessage();

	while (message != nullptr)
	{
		const ovrRequest requestId = ovr_Message_GetRequestID(message);
		const bool succeeded = ovr_Message_IsError(message) == false;

		if (requestId != invalidRequestID)
		{
			// first, we forward the response to the callback which has been provided when the request was invoked

			TemporaryScopedLock temporaryScopedLock(requestMapLock_);

			RequestMap::iterator iRequest = requestMap_.find(requestId);
			if (iRequest != requestMap_.cend())
			{
				const ResponseCallback responseCallback(std::move(iRequest->second));
				requestMap_.erase(iRequest);

				temporaryScopedLock.release();

				ocean_assert(responseCallback);
				responseCallback(message, succeeded);
			}
		}

		{
			// now, we forward the response to the callbacks which have subscribed for the message type

			const ovrMessageType messageType = ovr_Message_GetType(message);

#ifdef OCEAN_INTENSIVE_DEBUG
			Log::debug() << "Received message with type: " << ovrMessageType_ToString(messageType);
#endif

			const ScopedLock innerScopedLock(requestMapLock_);

			if (messageType != ovrMessage_Unknown)
			{
				const std::pair<ResponseTypeSubscriptionMap::iterator, ResponseTypeSubscriptionMap::iterator> range = responseTypeSubscriptionMap_.equal_range(messageType);
				for (ResponseTypeSubscriptionMap::iterator i = range.first; i != range.second; ++i)
				{
					i->second.first(message, succeeded);
				}
			}

			const std::pair<ResponseTypeSubscriptionMap::iterator, ResponseTypeSubscriptionMap::iterator> range = responseTypeSubscriptionMap_.equal_range(ovrMessage_Unknown);
			for (ResponseTypeSubscriptionMap::iterator i = range.first; i != range.second; ++i)
			{
				i->second.first(message, succeeded);
			}
		}

		ovr_FreeMessage(message);
		message = nullptr;

		message = ovr_PopMessage();
	}
}

bool Manager::invokeRequest(const ovrRequest requestId, ResponseCallback responseCallback)
{
	if (requestId == invalidRequestID)
	{
		return false;
	}

	const ScopedLock scopedLock(requestMapLock_);

	if (requestMap_.find(requestId) != requestMap_.cend())
	{
		ocean_assert(false && "Request id exists already");
		return false;
	}

	requestMap_.emplace(requestId, std::move(responseCallback));

	return true;
}

Manager::MessageScopedSubscription Manager::subscribeForMessageResponse(const ovrMessageType& messageType, ResponseCallback responseCallback)
{
	ocean_assert(responseCallback);

	const ScopedLock scopedLock(requestMapLock_);

	const unsigned int subscriptionId = subscriptionIdCounter_++;

	responseTypeSubscriptionMap_.emplace(messageType, ResponseSubscriptionPair(std::move(responseCallback), subscriptionId));

	return createMessageScopedSubscription(subscriptionId);
}

void Manager::unsubscribeForMessageResponse(const unsigned int subscriptionId)
{
	ocean_assert(subscriptionId != 0u);

	const ScopedLock scopedLock(requestMapLock_);

	for (ResponseTypeSubscriptionMap::iterator i = responseTypeSubscriptionMap_.begin(); i != responseTypeSubscriptionMap_.end(); ++i)
	{
		if (i->second.second == subscriptionId)
		{
			responseTypeSubscriptionMap_.erase(i);
			return;
		}
	}

	ocean_assert(false && "The subscription did not exist");
}

void Manager::onInitialized(ovrMessage* message, const bool succeeded)
{
	ocean_assert(isInitialized_ == false);

	if (succeeded)
	{
		Log::info() << "Platform SDK initialized";

		isInitialized_ = true;

		Application::get().checkLaunchDeepLink();

		MessageHandler::invokeRequest(ovr_Entitlement_GetIsViewerEntitled(), &Manager::onGetIsViewerEntitled);
	}
	else
	{
		Log::error() << "Failed to initialize platform SDK";
	}

	ocean_assert(initializationPending_);
	initializationPending_ = false;
}

void Manager::onGetIsViewerEntitled(ovrMessage* message, const bool succeeded)
{
	if (succeeded)
	{
		Log::debug() << "User is entitled";

		MessageHandler::invokeRequest(ovr_User_GetLoggedInUser(), &Manager::onGetLoggedInUser);
		MessageHandler::invokeRequest(ovr_User_GetAccessToken(), &Manager::onGetAccessToken);

		const ScopedLock scopedLock(lock_);

		entitlementType_ = ET_ENTITLED;
	}
	else
	{
		Log::debug() << "User is not entitled: " << Utilities::errorMessage(message);

		const ScopedLock scopedLock(lock_);

		entitlementType_ = ET_NOT_ENTITLED;
	}
}

void Manager::onGetLoggedInUser(ovrMessage* message, const bool succeeded)
{
	if (succeeded)
	{
		const ovrUserHandle userHandle = ovr_Message_GetUser(message);
		const ovrID userId = ovr_User_GetID(userHandle);

		userId_ = userId;

		Log::debug() << "User id: " << userId_;

#ifdef OCEAN_DEBUG
		const char* oculusId = ovr_User_GetOculusID(userHandle);
		if (oculusId != nullptr && oculusId[0] != '\0')
		{
			Log::debug() << "Oculus id: " << oculusId;
		}

		const char* userDisplayName = ovr_User_GetDisplayName(userHandle);
		if (userDisplayName != nullptr && userDisplayName[0] != '\0')
		{
			Log::debug() << "User display name: " << userDisplayName;
		}
#endif
	}
	else
	{
		Log::error() << "Failed to get user id: " << Utilities::errorMessage(message);
	}
}

void Manager::onGetAccessToken(ovrMessage* message, const bool succeeded)
{
	if (succeeded)
	{
		const char* messageString = ovr_Message_GetString(message);
		ocean_assert(messageString != nullptr);

		if (messageString != nullptr)
		{
			accessToken_ = messageString;
		}

		Log::debug() << "Accessed user token: " << accessToken_;
	}
	else
	{
		Log::error() << "Failed to get access token: " << Utilities::errorMessage(message);
	}
}

} // namespace PlatformSDK

} // namespace Quest

} // namespace Meta

} // namespace Platform

} // namespace Ocean
