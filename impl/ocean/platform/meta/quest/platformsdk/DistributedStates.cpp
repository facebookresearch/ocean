// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/platform/meta/quest/platformsdk/DistributedStates.h"

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

DistributedStates::DistributedStates(const double decisionDelay) :
	decisionDelay_(decisionDelay)
{
	receiveScopedSubscription_ = PlatformSDK::Network::get().addReceiveCallback(PlatformSDK::Network::CI_DISTRIBUTED_STATES, std::bind(&DistributedStates::onNetworkReceive, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
}

bool DistributedStates::state(const ElementId elementId, Value& value, Timestamp& timestamp, const double customDecisionDelay, const Timestamp& currentTimestamp)
{
	ocean_assert(elementId != 0u);
	ocean_assert(currentTimestamp.isValid());

	const ScopedLock scopedLock(lock_);

	ElementMap::iterator iElement = elementMap_.find(elementId);

	if (iElement == elementMap_.cend())
	{
		return false;
	}

	State& state = iElement->second;

	const double decisionDelay = customDecisionDelay >= 0.0 ? customDecisionDelay : decisionDelay_;
	ocean_assert(decisionDelay >= 0.0);

	if (currentTimestamp >= state.timestamp_ - decisionDelay)
	{
		state.hasChanged_ = false;

		value = state.value_;
		timestamp = state.timestamp_;

		return true;
	}

	return false;
}

bool DistributedStates::changedState(const ElementId elementId, Value& value, Timestamp& timestamp, const double customDecisionDelay, const Timestamp& currentTimestamp)
{
	ocean_assert(elementId != 0u);
	ocean_assert(currentTimestamp.isValid());

	const ScopedLock scopedLock(lock_);

	ElementMap::iterator iElement = elementMap_.find(elementId);

	if (iElement == elementMap_.cend())
	{
		return false;
	}

	State& state = iElement->second;

	if (state.hasChanged_)
	{
		const double decisionDelay = customDecisionDelay >= 0.0 ? customDecisionDelay : decisionDelay_;
		ocean_assert(decisionDelay >= 0.0);

		if (currentTimestamp >= state.timestamp_ - decisionDelay)
		{
			state.hasChanged_ = false;

			value = state.value_;
			timestamp = state.timestamp_;

			return true;
		}
	}

	return false;
}

void DistributedStates::updateState(const bool fromLocal, const ElementId elementId, Value&& value, const Timestamp& timestamp)
{
	const ScopedLock scopedLock(lock_);

	ElementMap::iterator iElement = elementMap_.find(elementId);

	if (iElement == elementMap_.cend())
	{
		iElement = elementMap_.emplace(elementId, State(std::move(value), timestamp)).first;
	}
	else
	{
		State& state = iElement->second;

		if (timestamp > state.timestamp_)
		{
			state.hasChanged_ = true;
			state.value_ = std::move(value);
			state.timestamp_ = timestamp;
		}
	}

	ocean_assert(iElement != elementMap_.cend());

	if (fromLocal)
	{
		// we need to forward the local information
		sendState(elementId, iElement->second.timestamp_, iElement->second.value_);
	}
}

void DistributedStates::onNetworkReceive(const uint64_t senderUserId, const Network::ComponentId componentId, const void* data, const size_t size, const Network::ConnectionType connectionType)
{
	static_assert(sizeof(ElementId) + sizeof(Timestamp) == 12, "Invalid data type!");

	constexpr size_t headerSize = sizeof(ElementId) + sizeof(double);

	if (size >= headerSize)
	{
		if (connectionType == Network::CT_UDP)
		{
			ElementId elementId;
			double timestamp;

			const uint8_t* data8 = (const uint8_t*)(data);

			memcpy(&elementId, data8, sizeof(elementId));
			data8 += sizeof(elementId);

			memcpy(&timestamp, data8, sizeof(timestamp));
			data8 += sizeof(timestamp);

			ocean_assert(size >= headerSize);

			Value value = Value::readFromBuffer(data8, size - headerSize);

			double adjustedTimestamp = double(Timestamp(true)); // **TODO**

			updateState(false /*fromLocal*/, elementId, std::move(value), Timestamp(adjustedTimestamp));
		}
	}
}

bool DistributedStates::sendState(const ElementId elementId, const Timestamp& timestamp, const Value& value)
{
	ocean_assert(value);

	static_assert(sizeof(ElementId) + sizeof(Timestamp) == 12, "Invalid data type!");

	std::vector<uint8_t> buffer;
	buffer.reserve(4 + 8 + 128);

	buffer.resize(12);

	memcpy(buffer.data(), &elementId, sizeof(elementId));
	memcpy(buffer.data() + sizeof(elementId), &timestamp, sizeof(timestamp));

	if (!Value::writeToBuffer(value, buffer, 12))
	{
		ocean_assert(!"This should never happen!");
		return false;
	}

	if (Network::get().sendToRoomUDP(Network::CI_DISTRIBUTED_STATES, buffer.data(), buffer.size()))
	{
		return true;
	}

	return false;
}

} // namespace PlatformSDK

} // namespace Quest

} // namespace Meta

} // namespace Platform

} // namespace Ocean
