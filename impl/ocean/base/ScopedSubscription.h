/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_SCOPED_SUBSCRIPTION_H
#define META_OCEAN_BASE_SCOPED_SUBSCRIPTION_H

#include "ocean/base/Base.h"
#include "ocean/base/Lock.h"

#include <functional>

namespace Ocean
{

/**
 * This class implements a subscription object which can be used unique subscriptions to e.g., callback functions.
 * The subscription exists as long as the object exists.
 * @tparam T The data type of the subscription id
 * @tparam TOwner The data type of the class in which this subscription object will be defined
 * @ingroup base
 */
template <typename T, typename TOwner>
class ScopedSubscriptionT
{
	friend TOwner;

	protected:

		/**
		 * Definition of a callback function for release requests.
		 * @param subscriptionId The id of the subscription to be released
		 */
		using ReleaseCallbackFunction = std::function<void(const T& subscriptionId)>;

	public:

		/**
		 * Creates an invalid (unsubscribed) subscription object.
		 */
		ScopedSubscriptionT() = default;

		/**
		 * Move constructor.
		 * @param scopedSubscription The object to be moved
		 */
		inline ScopedSubscriptionT(ScopedSubscriptionT<T, TOwner>&& scopedSubscription);

		/**
		 * Destructs the object and releases the subscription if any.
		 */
		~ScopedSubscriptionT();

		/**
		 * Explicitly releases the subscription before this object is disposes.
		 */
		void release();

		/**
		 * Returns whether this object holds a valid subscription.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Returns whether this object holds a valid subscription.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

		/**
		 * Move operator.
		 * @param scopedSubscription The object to be moved
		 * @return Reference to this object
		 */
		inline ScopedSubscriptionT<T, TOwner>& operator=(ScopedSubscriptionT<T, TOwner>&& scopedSubscription);

		/**
		 * Returns whether two subscription objects are identical.
		 * @param scopedSubscription The second subscription object to compare
		 * @return True, if so
		 */
		inline bool operator==(const ScopedSubscriptionT<T, TOwner>& scopedSubscription) const;

		/**
		 * Returns whether two subscription objects are not identical.
		 * @param scopedSubscription The second subscription object to compare
		 * @return True, if so
		 */
		inline bool operator!=(const ScopedSubscriptionT<T, TOwner>& scopedSubscription) const;

		/**
		 * Hash function.
		 * @param scopedSubscription The object for which the hash value will be determined
		 * @return The resulting hash value
		 */
		inline size_t operator()(const ScopedSubscriptionT<T, TOwner>& scopedSubscription) const;

	protected:

		/**
		 * Disabled copy constructor.
		 * @param scopedSubscription The object which would be copied
		 */
		ScopedSubscriptionT(const ScopedSubscriptionT<T, TOwner>& scopedSubscription) = delete;

		/**
		 * Creates a new subscription object for a valid subscription id.
		 * @param subscriptionId The subscription id, must be valid
		 * @param releaseCallbackFunction The callback function which will be used when the subscription needs to be released, must be valid
		 */
		explicit inline ScopedSubscriptionT(const T& subscriptionId, ReleaseCallbackFunction releaseCallbackFunction);

		/**
		 * The disabled assign operator.
		 * @param scopedSubscription The object which would be assigned
		 * @return The reference to this object
		 */
		ScopedSubscriptionT<T, TOwner>& operator=(const ScopedSubscriptionT<T, TOwner>& scopedSubscription) = delete;

	protected:

		/// The subscription id, nullptr if invalid.
		std::unique_ptr<T> subscriptionId_ = nullptr;

		/// The callback function which will be used when the subscription needs to be released.
		ReleaseCallbackFunction releaseCallbackFunction_ = nullptr;
};

/**
 * This class implements the base class for all subscription handlers.
 * @ingroup base
 */
class ScopedSubscriptionHandler
{
	public:

		/**
		 * Definition of a scoped subscription object
		 */
		using ScopedSubscriptionType = ScopedSubscriptionT<unsigned int, ScopedSubscriptionHandler>;

	protected:

		/**
		 * Returns the subscription object for a given subscription id.
		 * @param subscriptionId The subscription id of the resulting subscription object
		 * @param releaseCallbackFunction The release function of the subscription, must be valid
		 * @return The subscription object
		 */
		inline ScopedSubscriptionType scopedSubscription(const unsigned int& subscriptionId, ScopedSubscriptionType::ReleaseCallbackFunction releaseCallbackFunction);
};

/**
 * This class implements a handler for scoped subscriptions to callback functions.
 * The class mainly wraps a counter for subscription ids and a map holding the callback functions.
 * @tparam TCallbackFunction The data type of the callback function
 * @tparam TOwner The owner of this handler (the class in which the handler will be located)
 * @tparam tThreadSafe True, to make the handler thread-safe
 * @ingroup base
 */
template <typename TCallbackFunction, typename TOwner, bool tThreadSafe>
class ScopedSubscriptionCallbackHandlerT : public ScopedSubscriptionHandler
{
	friend TOwner;

	public:

		/**
		 * Definition of the data type of the callback function.
		 */
		using CallbackFunctionType = TCallbackFunction;

		/// True, if this handler is thread-safe.
		static constexpr bool isThreadSafe_ = tThreadSafe;

	protected:

		/**
		 * Definition of an unordered map mapping subscription ids to callback functions.
		 */
		using CallbackMap = std::unordered_map<unsigned int, TCallbackFunction>;

	public:

		/**
		 * Adds a new callback function to this handler.
		 * @param callbackFunction The callback function to add, must be valid
		 * @return The resulting scoped subscription object, the callback function will be in use until the resulting subscription object exists
		 */
		[[nodiscard]] ScopedSubscriptionType addCallback(TCallbackFunction callbackFunction);

		/**
		 * Returns the number of subscriptions.
		 * @return The handler's subscriptions
		 */
		size_t subscriptions() const;

		/**
		 * Returns whether no subscription exists.
		 * @return True, if so
		 */
		bool isEmpty() const;

	protected:

		/**
		 * Calls all callback functions of this handler.
		 * @param args The arguments of the callback function
		 * @return The resulting of any of the callback functions
		 * @tparam TArgs The data types of the callback functions
		 */
		template <class... TArgs>
		typename TCallbackFunction::result_type callCallbacks(TArgs&& ... args);

		/**
		 * Removes a callback function from the handler.
		 * @param subscriptionId The id of the subscription to be removed
		 */
		void removeCallback(const unsigned int& subscriptionId);

	protected:

		/// The map mapping subscription ids to callback functions.
		CallbackMap callbackMap_;

		/// The counter for subscription ids.
		unsigned int subscriptionIdCounter_ = 0u;

		/// The optional lock object.
		mutable TemplatedLock<tThreadSafe> lock_;
};

template <typename T, typename TOwner>
inline ScopedSubscriptionT<T, TOwner>::ScopedSubscriptionT(ScopedSubscriptionT<T, TOwner>&& scopedSubscription)
{
	*this = std::move(scopedSubscription);
}

template <typename T, typename TOwner>
inline ScopedSubscriptionT<T, TOwner>::ScopedSubscriptionT(const T& subscriptionId, ReleaseCallbackFunction releaseCallbackFunction) :
	subscriptionId_(std::make_unique<T>(subscriptionId)),
	releaseCallbackFunction_(std::move(releaseCallbackFunction))
{
	ocean_assert(isValid());
}

template <typename T, typename TOwner>
ScopedSubscriptionT<T, TOwner>::~ScopedSubscriptionT()
{
	release();
}

template <typename T, typename TOwner>
void ScopedSubscriptionT<T, TOwner>::release()
{
	if (subscriptionId_)
	{
		ocean_assert(releaseCallbackFunction_);
		releaseCallbackFunction_(*subscriptionId_);

		releaseCallbackFunction_ = nullptr;
		subscriptionId_ = nullptr;
	}
}

template <typename T, typename TOwner>
inline bool ScopedSubscriptionT<T, TOwner>::isValid() const
{
	return subscriptionId_ != nullptr;
}

template <typename T, typename TOwner>
inline ScopedSubscriptionT<T, TOwner>::operator bool() const
{
	return isValid();
}

template <typename T, typename TOwner>
inline ScopedSubscriptionT<T, TOwner>& ScopedSubscriptionT<T, TOwner>::operator=(ScopedSubscriptionT<T, TOwner>&& scopedSubscription)
{
	if (this != &scopedSubscription)
	{
		release();

		subscriptionId_ = std::move(scopedSubscription.subscriptionId_);
		releaseCallbackFunction_ = std::move(scopedSubscription.releaseCallbackFunction_);
	}

	return *this;
}

template <typename T, typename TOwner>
inline bool ScopedSubscriptionT<T, TOwner>::operator==(const ScopedSubscriptionT<T, TOwner>& scopedSubscription) const
{
	return subscriptionId_ == scopedSubscription.subscriptionId_ && releaseCallbackFunction_ == scopedSubscription.releaseCallbackFunction_;
}

template <typename T, typename TOwner>
inline bool ScopedSubscriptionT<T, TOwner>::operator!=(const ScopedSubscriptionT<T, TOwner>& scopedSubscription) const
{
	return !(*this == scopedSubscription);
}

template <typename T, typename TOwner>
inline size_t ScopedSubscriptionT<T, TOwner>::operator()(const ScopedSubscriptionT<T, TOwner>& scopedSubscription) const
{
	return std::hash<std::unique_ptr<T>>()(scopedSubscription.subscriptionId_);
}

inline ScopedSubscriptionHandler::ScopedSubscriptionType ScopedSubscriptionHandler::scopedSubscription(const unsigned int& subscriptionId, ScopedSubscriptionType::ReleaseCallbackFunction releaseCallbackFunction)
{
	ocean_assert(releaseCallbackFunction);
	return ScopedSubscriptionType(subscriptionId, std::move(releaseCallbackFunction));
}

template <typename TCallbackFunction, typename TOwner, bool tThreadSafe>
typename ScopedSubscriptionHandler::ScopedSubscriptionType ScopedSubscriptionCallbackHandlerT<TCallbackFunction, TOwner, tThreadSafe>::addCallback(TCallbackFunction callbackFunction)
{
	const TemplatedScopedLock<tThreadSafe> scopedLock(lock_);

	const unsigned int subscriptionId = ++subscriptionIdCounter_;

	ocean_assert(callbackMap_.find(subscriptionId) == callbackMap_.cend());
	callbackMap_.emplace(subscriptionId, std::move(callbackFunction));

	return scopedSubscription(subscriptionId, std::bind(&ScopedSubscriptionCallbackHandlerT<TCallbackFunction, TOwner, tThreadSafe>::removeCallback, this, std::placeholders::_1));
}

template <typename TCallbackFunction, typename TOwner, bool tThreadSafe>
size_t ScopedSubscriptionCallbackHandlerT<TCallbackFunction, TOwner, tThreadSafe>::subscriptions() const
{
	const TemplatedScopedLock<tThreadSafe> scopedLock(lock_);

	return callbackMap_.size();
}

template <typename TCallbackFunction, typename TOwner, bool tThreadSafe>
bool ScopedSubscriptionCallbackHandlerT<TCallbackFunction, TOwner, tThreadSafe>::isEmpty() const
{
	const TemplatedScopedLock<tThreadSafe> scopedLock(lock_);

	return callbackMap_.empty();
}

template <typename TCallbackFunction, typename TOwner, bool tThreadSafe>
template <class... TArgs>
typename TCallbackFunction::result_type ScopedSubscriptionCallbackHandlerT<TCallbackFunction, TOwner, tThreadSafe>::callCallbacks(TArgs&& ... args)
{
	const TemplatedScopedLock<tThreadSafe> scopedLock(lock_);

	for (typename CallbackMap::const_iterator iCallback = callbackMap_.cbegin(); iCallback != callbackMap_.cend(); ++iCallback)
	{
		const TCallbackFunction& callbackFunction = iCallback->second;

		typename CallbackMap::const_iterator iNextCallback(iCallback);

		if (++iNextCallback == callbackMap_.cend())
		{
			return callbackFunction(std::forward<TArgs>(args)...);
		}
		else
		{
			callbackFunction(std::forward<TArgs>(args)...);
		}
	}
}

template <typename TCallbackFunction, typename TOwner, bool tThreadSafe>
void ScopedSubscriptionCallbackHandlerT<TCallbackFunction, TOwner, tThreadSafe>::removeCallback(const unsigned int& subscriptionId)
{
	const TemplatedScopedLock<tThreadSafe> scopedLock(lock_);

	ocean_assert(callbackMap_.find(subscriptionId) != callbackMap_.cend());

	callbackMap_.erase(subscriptionId);
}

}

#endif // META_OCEAN_BASE_SCOPED_SUBSCRIPTION_H
