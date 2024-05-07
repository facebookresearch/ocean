// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#ifndef META_OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_DISTRIBUTED_STATES_H
#define META_OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_DISTRIBUTED_STATES_H

#include "ocean/platform/meta/quest/platformsdk/PlatformSDK.h"
#include "ocean/platform/meta/quest/platformsdk/Network.h"

#include "ocean/base/Lock.h"
#include "ocean/base/Value.h"
#include "ocean/base/Timestamp.h"

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
 * This class implements TODO
 * @ingroup platformmetaquestplatformsdk
 */
class OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_EXPORT DistributedStates
{
	public:

		typedef uint32_t ElementId;

	protected:

		class State
		{
			public:

				inline State(Value&& value, const Timestamp& timestamp);

			public:

				bool hasChanged_ = false;

				Value value_;

				Timestamp timestamp_;
		};

		typedef std::unordered_map<ElementId, State> ElementMap;

	public:

		explicit DistributedStates(const double decisionDelay = 0.01);

		inline void updateState(const ElementId elementId, Value&& value, const Timestamp& timestamp);

		bool state(const ElementId elementId, Value& value, Timestamp& timestamp, const double customDecisionDelay = -1.0, const Timestamp& currentTimestamp = Timestamp(true));

		bool changedState(const ElementId elementId, Value& value, Timestamp& timestamp, const double customDecisionDelay = -1.0, const Timestamp& currentTimestamp = Timestamp(true));

	protected:

		void updateState(const bool fromLocal, const ElementId elementId, Value&& value, const Timestamp& timestamp);

		/**
		 * The event function for new data received via network.
		 * @param senderUserId The id of the user sending the data, must be valid
		 * @param componentId The id of the component sending the data
		 * @param data The actual data, must be valid
		 * @param size The size of the data, in bytes, with range [1, infinity)
		 * @param connectionType The type of the connecition which has been used to receive the data
		 */
		void onNetworkReceive(const uint64_t senderUserId, const Network::ComponentId componentId, const void* data, const size_t size, const Network::ConnectionType connectionType);

		static bool sendState(const ElementId elementId, const Timestamp& timetamp, const Value& value);

	protected:

		ElementMap elementMap_;

		double decisionDelay_ = 0.01;

		/// The subscription object to receive network data.
		Network::ReceiveScopedSubscription receiveScopedSubscription_;

		uint32_t timestampSynchronizationIterationCounter_ = 0u;

		mutable Lock lock_;
};

inline DistributedStates::State::State(Value&& value, const Timestamp& timestamp) :
	hasChanged_(true),
	value_(std::move(value)),
	timestamp_(timestamp)
{
	// nothing to do here
}

inline void DistributedStates::updateState(const ElementId elementId, Value&& value, const Timestamp& timestamp)
{
	updateState(true /*fromLocal*/, elementId, std::move(value), timestamp);
}

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_DISTRIBUTED_STATES_H
