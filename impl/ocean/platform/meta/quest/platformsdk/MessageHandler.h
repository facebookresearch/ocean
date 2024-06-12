/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_MESSAGE_HANDLER_H
#define META_OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_MESSAGE_HANDLER_H

#include "ocean/platform/meta/quest/platformsdk/PlatformSDK.h"

#include "ocean/base/ScopedSubscription.h"

#include <OVR_Platform.h>

#include <future>

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
 * This class is the base class for all objects needing to handle with messages.
 * The class is mainly forwarding all calls of invokeRequest() to Manager::get().invokeRequest().
 * @ingroup platformmetaquestplatformsdk
 */
class OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_EXPORT MessageHandler
{
	friend class Manager;

	public:

		/**
		 * This class implements a scoped handle for options.
		 * The class is a helper class to ensure that options are always destroyed after usage.
		 * @tparam T The data type of the option
		 */
		template <typename T>
		class ScopedOptions
		{
			public:

				/**
				 * Definition of a function pointer to a function destroying the object.
				 */
				using DestroyFunction = void(*)(T object);

			public:

				/**
				 * Creates a new scoped options object.
				 * @param options The actual options which will be destroyed once this object is disposed, must be valid
				 * @param destoryFunction The destroy function for the given options, must be valid
				 */
				inline ScopedOptions(const T& options, const DestroyFunction& destoryFunction);

				/**
				 * Destructs the object and destroys the options.
				 */
				inline ~ScopedOptions();

				/**
				 * Returns the actual options.
				 * @return The options of this object
				 */
				inline operator T() const;

			protected:

				/**
				 * Disabled copy constructor.
				 */
				ScopedOptions(const ScopedOptions&) = delete;

				/**
				 * Disabled move constructor.
				 */
				ScopedOptions(ScopedOptions&&) = delete;

				/**
				 * Disabled assign operator.
				 */
				ScopedOptions& operator=(const ScopedOptions&) = delete;

				/**
				 * Disabled move operator.
				 */
				ScopedOptions& operator=(ScopedOptions&&) = delete;

			protected:

				/// The actual options.
				T options_ = nullptr;

				/// The destroy function.
				DestroyFunction destroyFunction_ = nullptr;
		};

		/**
		 * This class implements a helper object allowing to store responses for requests.
		 * @tparam T the data type of the response
		 */
		template <typename T>
		class RequestObject
		{
			public:

				/**
				 * Default constructor creating a new object without response.
				 */
				RequestObject() = default;

				/**
				 * Sets the latest response.
				 * @param response The response to set
				 */
				void setResponse(T&& response);

				/**
				 * Sets the latest response.
				 * @param response The response to set
				 */
				void setResponse(const T& response);

				/**
				 * Returns whether this object holds a latest response.
				 * In case a latest response was available, the object is reset so that no latest response is available anymore.
				 * @param response The resulting latest response, if any
				 * @return True, if a latest response was available
				 */
				bool latestResponse(T& response);

			protected:

				/// The latest response.
				T latestResponse_;

				/// True, if this object holds a latest response.
				bool hasLatestResponse_ = false;

				/// The object's lock.
				mutable Lock lock_;
		};

		/**
		 * This class implements a helper object allowing to queue responses of requests.
		 */
		template <typename T>
		class RequestQueue : public RequestObject<T>
		{
			public:

				/**
				 * Default constructor.
				 */
				RequestQueue() = default;

				/**
				 * Adds a new request and returns the corresponding future.
				 * @return The future of the new request
				 */
				std::future<T> addRequest();

				/**
				 * Sets the latest response.
				 * The response will be forwarded to the oldest queued future as well.
				 * @param response The response to set
				 */
				void setResponse(T&& response);

				/**
				 * Sets the latest response.
				 * The response will be forwarded to the oldest queued future as well.
				 * @param response The response to set
				 */
				void setResponse(const T& response);

			protected:

				/// The queue holding all active promises for the futures of all requests.
				std::queue<std::promise<T>> promises_;
		};

		/**
		 * Definition of a callback function for message responses.
		 * @param message The response message, must be valid
		 * @param succeeded True, if the message succeeded; False, if an error happened
		 */
		using ResponseCallback = std::function<void(ovrMessage* message, bool succeeded)>;

		/**
		 * Definition of a member callback function for message responses.
		 * @param message The response message, must be valid
		 * @param succeeded True, if the message succeeded; False, if an error happened
		 * @tparam T The data type of the class of the callback function
		 */
		template <typename T>
		using ResponseCallbackFunctionT = void (T::*)(ovrMessage* message, const bool succeeded);

		/**
		 * Definition of a scoped subscription object for messages.
		 */
		using MessageScopedSubscription = ScopedSubscriptionT<unsigned int, MessageHandler>;

		/**
		 * Definition of a vector holding MessageScopedSubscription objects.
		 */
		using MessageScopedSubscriptions = std::vector<MessageScopedSubscription>;

	public:

		/**
		 * Invokes a new request.
		 * @param requestId The id of the new request
		 * @param responseCallback The callback member function which will be called once the response has arrived
		 * @return True, if succeeded
		 * @tparam T The data type of the class of the callback function
		 */
		template <typename T>
		bool invokeRequest(const ovrRequest requestId, const ResponseCallbackFunctionT<T>& responseCallback);

		/**
		 * Subscribes a callback function for response messages with specific type.
		 * @param messageType The type of the message for wich the callback function will be subscibed, ovrMessage_Unknown to subscribe to all messages
		 * @param responseCallback The callback member function which will be called whenever a response message with the specified type arrives
		 * @return The subscription object, the subscription is active as long as the subscription object exists
		 * @see unsubscribeForMessageResponse().
		 */
		template <typename T>
		[[nodiscard]] MessageScopedSubscription subscribeForMessageResponse(const ovrMessageType& messageType, const ResponseCallbackFunctionT<T>& responseCallback);

		/**
		 * Invokes a new request.
		 * @param requestId The id of the new request
		 * @param responseCallback The callback function which will be called once the response has arrived
		 * @return True, if succeeded
		 */
		static bool invokeRequest(const ovrRequest requestId, ResponseCallback responseCallback);

		/**
		 * Subscribes a callback function for response messages with specific type.
		 * @param messageType The type of the message for wich the callback function will be subscibed, ovrMessage_Unknown to subscribe to all messages
		 * @param responseCallback The callback function which will be called whenever a response message with the specified type arrives
		 * @return The subscription object, the subscription is active as long as the subscription object exists
		 * @see unsubscribeForMessageResponse().
		 */
		[[nodiscard]] static MessageScopedSubscription subscribeForMessageResponse(const ovrMessageType& messageType, ResponseCallback responseCallback);

		/**
		 * Converts a string with exactly four characters to a unique tag value.
		 * @param tagString The string with four characters to be converted
		 * @return The resulting tag value
		 */
		static constexpr uint32_t string2tag(const char tagString[4]);

	protected:

		/**
		 * Creates a new subscription object for a given subscription id.
		 * @param subscriptionId The subscription id, must be valid
		 * @return The resulting subscription object
		 */
		[[nodiscard]] static MessageScopedSubscription createMessageScopedSubscription(const unsigned int subscriptionId);

		/**
		 * Removes a subscription for response messages with specific message type.
		 * @param subscriptionId The subscription id to unsubscribe, must be valid
		 * @see subscribeForMessageResponse().
		 */
		static void unsubscribeForMessageResponse(const unsigned int& subscriptionId);
};

template <typename T>
bool MessageHandler::invokeRequest(const ovrRequest requestId, const ResponseCallbackFunctionT<T>& responseCallback)
{
	return invokeRequest(requestId, std::bind(responseCallback, (T*)(this), std::placeholders::_1, std::placeholders::_2));
}

template <typename T>
MessageHandler::MessageScopedSubscription MessageHandler::subscribeForMessageResponse(const ovrMessageType& messageType, const ResponseCallbackFunctionT<T>& responseCallback)
{
	return subscribeForMessageResponse(messageType, std::bind(responseCallback, (T*)(this), std::placeholders::_1, std::placeholders::_2));
}

template <typename T>
inline MessageHandler::ScopedOptions<T>::ScopedOptions(const T& options, const DestroyFunction& destroyFunction) :
	options_(options),
	destroyFunction_(destroyFunction)
{
	ocean_assert(options_ != nullptr);
	ocean_assert(destroyFunction_ != nullptr);
}

template <typename T>
inline MessageHandler::ScopedOptions<T>::~ScopedOptions<T>()
{
	ocean_assert(options_ != nullptr);
	ocean_assert(destroyFunction_ != nullptr);

	destroyFunction_(options_);
}

template <typename T>
inline MessageHandler::ScopedOptions<T>::operator T() const
{
	return options_;
}

template <typename T>
void MessageHandler::RequestObject<T>::setResponse(T&& response)
{
	const ScopedLock scopedLock(lock_);

	latestResponse_ = std::move(response);
	hasLatestResponse_ = true;
}

template <typename T>
void MessageHandler::RequestObject<T>::setResponse(const T& response)
{
	const ScopedLock scopedLock(lock_);

	latestResponse_ = response;
	hasLatestResponse_ = true;
}

template <typename T>
bool MessageHandler::RequestObject<T>::latestResponse(T& value)
{
	const ScopedLock scopedLock(lock_);

	if (hasLatestResponse_)
	{
		value = std::move(latestResponse_);
		hasLatestResponse_ = false;

		return true;
	}

	return false;
}

template <typename T>
std::future<T> MessageHandler::RequestQueue<T>::addRequest()
{
	const ScopedLock scopedLock(this->lock_);

	std::promise<T> promise;
	std::future future = promise.get_future();

	promises_.push(std::move(promise));

	return future;
}

template <typename T>
void MessageHandler::RequestQueue<T>::setResponse(T&& response)
{
	const ScopedLock scopedLock(this->lock_);

	ocean_assert(!promises_.empty());

	std::promise<T>& promise = promises_.front();
	promise.set_value(response);

	RequestObject<T>::setResponse(std::move(response));

	promises_.pop();
}

template <typename T>
void MessageHandler::RequestQueue<T>::setResponse(const T& response)
{
	const ScopedLock scopedLock(this->lock_);

	ocean_assert(!promises_.empty());

	std::promise<T>& promise = promises_.front();
	promise.set_value(response);

	RequestObject<T>::setResponse(response);

	promises_.pop();
}

constexpr uint32_t MessageHandler::string2tag(const char tagString[4])
{
	ocean_assert(tagString[0] != 0 && tagString[1] != 0 && tagString[2] != 0 && tagString[3] != 0);

	return (uint32_t(tagString[0]) << 0ull)
				| (uint32_t(tagString[1]) << 8ull)
				| (uint32_t(tagString[2]) << 16ull)
				| (uint32_t(tagString[3]) << 24ull);
}

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_MESSAGE_HANDLER_H
