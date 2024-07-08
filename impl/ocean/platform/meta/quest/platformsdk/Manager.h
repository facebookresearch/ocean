/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_MANAGER_H
#define META_OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_MANAGER_H

#include "ocean/platform/meta/quest/platformsdk/PlatformSDK.h"
#include "ocean/platform/meta/quest/platformsdk/MessageHandler.h"

#include "ocean/base/Lock.h"
#include "ocean/base/Singleton.h"

#include <OVR_Platform.h>

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

/**
 * This class implements a manager that handles the central PlatformSDK functionalities.
 * @ingroup platformmetaquestplatformsdk
 */
class OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_EXPORT Manager :
	public Singleton<Manager>,
	protected MessageHandler
{
	friend class Singleton<Manager>;
	friend class MessageHandler;

	public:

		/**
		 * Definition of individual entitlement types.
		 */
		enum EntitlementType : uint32_t
		{
			/// The entitlement state is not yet known.
			ET_NOT_YET_KNOWN = 0u,
			/// The user is not entitled.
			ET_NOT_ENTITLED,
			/// The user is entitled.
			ET_ENTITLED,
		};

	protected:

		/**
		 * Definition of an unordered map mapping message requests to response callback functions.
		 */
		typedef std::unordered_map<ovrRequest, ResponseCallback> RequestMap;

		/**
		 * Definition of a pair combining a response callback with a subscription id.
		 */
		typedef std::pair<ResponseCallback, unsigned int> ResponseSubscriptionPair;

		/**
		 * Definition of an unordered multi-map mapping message types to response subscription pairs.
		 */
		typedef std::unordered_multimap<ovrMessageType, ResponseSubscriptionPair> ResponseTypeSubscriptionMap;

	public:

		/**
		 * Initializes the manager.
		 * The manager must be initialized once at application start.
		 * @param activityObject The native activity object, must be valid
		 * @param jniEnv The JNI environment, must be valid
		 * @param appId The application id which is used (provided when registering a new developer app), must be valid
		 * @return True, if succeeded
		 */
		bool initialize(jobject activityObject, JNIEnv* jniEnv, const std::string& appId);

		/**
		 * Updates the manager and the underlying PlatformSDK functionalities.
		 * This function can be called even if the manager is not yet initialized.
		 * @param timestamp The current timestamp, with range (-infinity, infinity)
		 */
		void update(const double timestamp);

		/**
		 * Returns whether the manager has been fully initialized.
		 * Call update() before calling this function.
		 * @return True, if so
		 */
		inline bool isInitialized() const;

		/**
		 * Returns the id of the currently logged-in user.
		 * @param accessToken Optional resulting access token for the logged-in user, empty if unknown or not yet known
		 * @return The user's id, 0 if unknown or not yet known
		 */
		inline uint64_t userId(std::string* accessToken = nullptr) const;

		/**
		 * Returns the entitlement type.
		 * One of the requirements to sell an app in the Meta Quest Store is that you verify the user purchased or obtained your app legitimately.
		 * This check is called the entitlement check. You should make the entitlement check within 10 seconds of the user launching your app.
		 * @return The user's entitelement type
		 * @see https://developer.oculus.com/documentation/native/ps-entitlement-check/
		 */
		inline EntitlementType entitlementType() const;

		/**
		 * Invokes a new request.
		 * @param requestId The id of the new request
		 * @param responseCallback The callback function which will be called once the response has arrived
		 * @return True, if succeeded
		 */
		bool invokeRequest(const ovrRequest requestId, ResponseCallback responseCallback);

		/**
		 * Subscribes a callback function for response messages with specific type.
		 * @param messageType The type of the message for wich the callback function will be subscibed, ovrMessage_Unknown to subscribe to all messages
		 * @param responseCallback The callback function which will be called whenever a response message with the specified type arrives
		 * @return The subscription object, the subscription is active as long as the subscription object exists
		 * @see unsubscribeForMessageResponse().
		 */
		MessageScopedSubscription subscribeForMessageResponse(const ovrMessageType& messageType, ResponseCallback responseCallback);

	protected:

		/**
		 * Protected default constructor.
		 */
		Manager() = default;

		/**
		 * Destructs the manager and releases all resources.
		 */
		~Manager();

		/**
		 * Removes a subscription for response messages with specific message type.
		 * @param subscriptionId The subscription id to unsubscribe, must be valid
		 * @see subscribeForMessageResponse().
		 */
		void unsubscribeForMessageResponse(const unsigned int subscriptionId);

		/**
		 * The response function for ovr_PlatformInitializeAndroidAsynchronous().
		 * @param message The response message, must be valid
		 * @param succeeded True, if the request succeeded
		 */
		void onInitialized(ovrMessage* message, const bool succeeded);

		/**
		 * The response function for ovr_Entitlement_GetIsViewerEntitled().
		 * @param message The response message, must be valid
		 * @param succeeded True, if the request succeeded
		 */
		void onGetIsViewerEntitled(ovrMessage* message, const bool succeeded);

		/**
		 * The response function for ovr_User_GetLoggedInUser().
		 * @param message The response message, must be valid
		 * @param succeeded True, if the request succeeded
		 */
		void onGetLoggedInUser(ovrMessage* message, const bool succeeded);

		/**
		 * The response function for ovr_User_GetAccessToken().
		 * @param message The response message, must be valid
		 * @param succeeded True, if the request succeeded
		 */
		void onGetAccessToken(ovrMessage* message, const bool succeeded);

	protected:

		/// True, if the manager's initialization is pending.
		bool initializationPending_ = false;

		/// True, if the manager has been initialized.
		bool isInitialized_  = false;

		/// The entitlement type of the user.
		EntitlementType entitlementType_ = ET_NOT_YET_KNOWN;

		/// The deep link which has been used to launch the app, empty if the app was launched normally.
		std::string launchDeepLink_;

		/// The counter for subscription ids.
		unsigned int subscriptionIdCounter_ = 1u;

		/// The map mapping request ids to response callback functions.
		RequestMap requestMap_;

		/// The map mapping response message types to subscription pairs.
		ResponseTypeSubscriptionMap responseTypeSubscriptionMap_;

		/// The lock for the request map.
		mutable Lock requestMapLock_;

		/// The id of the logged-in user.
		ovrID userId_ = ovrID(0);

		/// The access token of the logged-in user.
		std::string accessToken_;

		/// The result queue for launchAnotherApplication().
		RequestQueue<bool> setGroupPresenceRequestQueue_;

		/// The manager's lock.
		mutable Lock lock_;
};

inline bool Manager::isInitialized() const
{
	const ScopedLock scopedLock(lock_);

	return isInitialized_;
}

inline uint64_t Manager::userId(std::string* accessToken) const
{
	static_assert(sizeof(ovrID) == sizeof(uint64_t), "Invalid data type!");

	const ScopedLock scopedLock(lock_);

	if (accessToken != nullptr)
	{
		*accessToken = accessToken_;
	}

	return userId_;
}

inline Manager::EntitlementType Manager::entitlementType() const
{
	const ScopedLock scopedLock(lock_);

	return entitlementType_;
}

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_MANAGER_H
