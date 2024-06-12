/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_MICROPHONE_H
#define META_OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_MICROPHONE_H

#include "ocean/platform/meta/quest/platformsdk/PlatformSDK.h"

#include "ocean/base/Lock.h"
#include "ocean/base/Singleton.h"
#include "ocean/base/ScopedSubscription.h"
#include "ocean/base/Thread.h"

#include <OVR_Voip_LowLevel.h>

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
 * This class provides access to the device's microphone.
 * @ingroup platformmetaquestplatformsdk
 */
class OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_EXPORT Microphone :
	public Singleton<Microphone>,
	protected Thread
{
	friend class Singleton<Microphone>;

	public:

		/**
		 * Definition of a callback function for microphone samples.
		 * The data format is signed 16 bit integer 48khz mono.
		 * @param elements The elements of the sample, must be valid
		 * @param size The number of elements in the sample, with range [1, infinity)
		 */
		using SampleCallback = std::function<void(const int16_t* elements, const size_t size)>;

		/**
		 * Definition of a subscription object for microphone samples.
		 * The subscription exists as long as this object exists.<br>
		 * The microphone will be stopped once the last subscription object is disposed.
		 */
		using ScopedSubscription = ScopedSubscriptionT<unsigned int, Microphone>;

	protected:

		/**
		 * Definition of a map mapping subscription ids to callback functions for microphone samples.
		 */
		using SampleCallbackMap = std::unordered_map<unsigned int, SampleCallback>;

	public:

		/**
		 * Starts the microphone.
		 * This function can be called several times for several users.<br>
		 * The microphone will be recording as long as at least one ScopedSubscription object exists.
		 * @param sampleCallback The callback function for microphone samples, must be valid
		 * @return The subscription object, invalid if the microphone could not be started
		 */
		[[nodiscard]] ScopedSubscription start(SampleCallback sampleCallback);

	protected:

		/**
		 * Default constructor.
		 */
		Microphone() = default;

		/**
		 * Stops the microphone.
		 * @param subscriptionId The subscription id for which the microphone will be stopped, must be valid
		 */
		void stop(const unsigned int& subscriptionId);

		/**
		 * The internal thread function.
		 */
		void threadRun() override;

	protected:

		/// The counter for subscription ids.
		unsigned int subscriptionIdCounter_ = 1u;

		/// The handle for the microphone.
		ovrMicrophoneHandle microphoneHandle_ = nullptr;

		/// The callback functions for microphone samples.
		SampleCallbackMap sampleCallbackMap_;

		/// The microphone's lock.
		Lock lock_;
};

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_MICROPHONE_H
