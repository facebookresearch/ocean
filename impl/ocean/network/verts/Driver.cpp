// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/network/verts/Driver.h"

#include "ocean/base/ScopedValue.h"
#include "ocean/base/Utilities.h"

#include "ocean/math/RateCalculator.h"

#include <verts/client/capi.h>

#include "verts/shared/ctypes.h"

namespace Ocean
{

namespace Network
{

namespace Verts
{

Driver::ContainerReceiver::ContainerReceiver(const ContainerSequenceDataHeader& containerSequenceDataHeader, const Timestamp& timestamp) :
	containerSequenceDataHeader_(containerSequenceDataHeader)
{
	ocean_assert(containerSequenceDataHeader_.isValid());
	ocean_assert(timestamp.isValid());

	buffer_.resize(maximalSequenceSize() * size_t(containerSequenceDataHeader_.numberSequences_));

	for (uint32_t index = 0u; index < containerSequenceDataHeader_.numberSequences_; ++index)
	{
		pendingSequenceIndices_.emplace(index);
	}

	highestFinishedSequenceIndex_ = containerSequenceDataHeader_.sequenceIndex_;

	lastestArrivalTimestamp_ = timestamp;
}

bool Driver::ContainerReceiver::setSequence(const uint32_t sequenceIndex, const void* data, const size_t size, const Timestamp& timestamp)
{
	ocean_assert(containerSequenceDataHeader_.isValid());

	ocean_assert(data != nullptr && size > 0 && size <= maximalPayloadSize());
	ocean_assert(timestamp.isValid());

	ocean_assert(!pendingSequenceIndices_.empty());

	if (initialArrivalTimestamp_.isInvalid())
	{
		initialArrivalTimestamp_ = timestamp;
	}

	lastestArrivalTimestamp_ = timestamp;

	ocean_assert(sequenceIndex < containerSequenceDataHeader_.numberSequences_);
	if (sequenceIndex < containerSequenceDataHeader_.numberSequences_)
	{
		ocean_assert(highestFinishedSequenceIndex_ != uint32_t(-1));
		if (sequenceIndex > highestFinishedSequenceIndex_)
		{
			highestFinishedSequenceIndex_ = sequenceIndex;
		}

		const IndexSet32::iterator iPending = pendingSequenceIndices_.find(sequenceIndex);

		if (iPending == pendingSequenceIndices_.cend())
		{
			// we have received the sequence already
			ocean_assert(memcmp(buffer_.data() + sequenceIndex * maximalPayloadSize(), data, size) == 0);

			++redundantSequences_;

			return true;
		}

		ocean_assert(sequenceIndex * maximalPayloadSize() + size <= buffer_.size());
		memcpy(buffer_.data() + sequenceIndex * maximalPayloadSize(), data, size);

		pendingSequenceIndices_.erase(iPending);

		if (sequenceIndex + 1u == containerSequenceDataHeader_.numberSequences_)
		{
			// we have received the last sequence, now we know the actual size

			ocean_assert(containerSequenceDataHeader_.numberSequences_ >= 1u);
			actualSize_ = (containerSequenceDataHeader_.numberSequences_ - 1u) * maximalPayloadSize() + size;

			buffer_.resize(actualSize_);

			Log::debug() << "Actual size for container " << containerSequenceDataHeader_.uniqueId() << ": " << actualSize_ << "bytes";
		}
		else
		{
			ocean_assert(size == maximalPayloadSize());
		}

		if (pendingSequenceIndices_.size() % 200 == 0)
		{
			Log::debug() << "Pending sequences for " << containerSequenceDataHeader_.uniqueId() << ": " << pendingSequenceIndices_.size();
		}

		return true;
	}

	return false;
}

bool Driver::ContainerReceiver::needResentLostSequences(const Timestamp& timestamp, Indices32& lostSequenceIds, const double interval, const double arrivalTimeout, const size_t maximalIds)
{
	ocean_assert(lastestArrivalTimestamp_.isValid());

	ocean_assert(timestamp.isValid());
	ocean_assert(interval >= 0.0 && arrivalTimeout >= 0.0);
	ocean_assert(maximalIds > 0);

	if (timestamp < lastLostSequenceIdsTimestamp_ + interval)
	{
		return false;
	}

	lastLostSequenceIdsTimestamp_ = timestamp;

	lostSequenceIds.clear();

	if (timestamp > lastestArrivalTimestamp_ + arrivalTimeout)
	{
		// we have not received any new sequences in a long time - therefore, we report all pending sequences as lost

		const size_t size = std::min(pendingSequenceIndices_.size(), maximalIds);

		lostSequenceIds.reserve(size);

		for (IndexSet32::const_iterator iPending = pendingSequenceIndices_.cbegin(); lostSequenceIds.size() < size && iPending != pendingSequenceIndices_.cend(); ++iPending)
		{
			lostSequenceIds.emplace_back(*iPending);
		}

		if (!lostSequenceIds.empty())
		{
			Log::debug() << "Container receiver with identifier '" << identifier_ << "' and version " << version_ << " reported all " << pendingSequenceIndices_.size() << " pending sequences to be lost, elapsed time " << double(timestamp - lastestArrivalTimestamp_);
		}
	}
	else
	{
		if (containerSequenceDataHeader_.numberSequences_ * 5u / 100u > 50u && highestFinishedSequenceIndex_ < containerSequenceDataHeader_.numberSequences_ * 95u / 100u)
		{
			// we do not start sending lost sequences before we are close to the end
			return false;
		}

		// we report all sequences as lost which indices are smaller than the sequence number of the higest finished sequence

		lostSequenceIds.reserve(std::min(pendingSequenceIndices_.size() * 6 / 128, maximalIds)); // ~5% or maximalIds

		for (const Index32 pendingId : pendingSequenceIndices_) // pendingSequenceIndices is sorted
		{
			if (pendingId < highestFinishedSequenceIndex_)
			{
				lostSequenceIds.emplace_back(pendingId);

				if (lostSequenceIds.size() >= maximalIds)
				{
					break;
				}
			}
			else
			{
				ocean_assert(pendingId != highestFinishedSequenceIndex_);

				break;
			}
		}

		if (!lostSequenceIds.empty())
		{
			Log::debug() << "Container receiver with identifier '" << identifier_ << "' and version " << version_ << " reported " << lostSequenceIds.size() << " pending sequences to be lost";
		}
	}

	lostSequences_ += lostSequenceIds.size();

	return !lostSequenceIds.empty();
}

Driver::SharedBuffer Driver::ContainerReceiver::finishedBuffer(std::string& identifier, unsigned int& version, const Timestamp& timestamp)
{
	ocean_assert(isFinished());
	ocean_assert(!buffer_.empty());

	identifier = std::move(identifier_);
	version = (unsigned int)(version_);

#ifdef OCEAN_DEBUG
	if (containerSequenceDataHeader_.numberSequences_ > 0u)
	{
		ocean_assert(initialArrivalTimestamp_.isValid());
		const double time = double(timestamp - initialArrivalTimestamp_);

		const double throughput = time > 0.0 ? double(buffer_.size()) / double(time * 1024.0 * 1024.0) : 0.0;

		Log::debug() << "Container '" << identifier << "' with version " << version << " arrived with " << throughput << " MB/s, reported "
					<< String::toAString(Scalar(lostSequences_) / Scalar(containerSequenceDataHeader_.numberSequences_) * Scalar(100), 1u) << "% lost sequences, and "
					<< String::toAString(Scalar(redundantSequences_) / Scalar(containerSequenceDataHeader_.numberSequences_) * Scalar(100), 1u) << "% redundant sequences";
	}
#endif

	return std::make_shared<Buffer>(std::move(buffer_));
}

void Driver::ContainerSender::addLostSequenceIndices(const Index32* sequenceIndices, const size_t size, const Timestamp& timestamp)
{
	ocean_assert(sequenceIndices != nullptr && size > 0);

	for (size_t n = 0; n < size; ++n)
	{
		lostSequenceIndexSet_.emplace(sequenceIndices[n]);
	}

	ocean_assert(timestamp.isValid());

	if (updateUsageTimestamp_.isValid() && timestamp > updateUsageTimestamp_)
	{
		updateUsageTimestamp_ = timestamp;
	}

	if (lostSequencesResentTimestamp_.isInvalid())
	{
		lostSequencesResentTimestamp_ = timestamp;
	}
}

bool Driver::ContainerSender::needsResentLostSequences(const Timestamp& timestamp, UnorderedIndexSet64& lostSequenceIndices, const double interval)
{
	/*if (updateUsageTimestamp_.isInvalid())
	{
		// we have not yet sent the last sequence of the container
		return false;
	}*/

	if (lostSequenceIndexSet_.empty())
	{
		lostSequencesResentTimestamp_ = timestamp;
		return false;
	}

	if (lostSequencesResentTimestamp_ + interval <= timestamp)
	{
		lostSequencesResentTimestamp_ = timestamp;

		lostSequences_ += lostSequenceIndexSet_.size();

		lostSequenceIndices = std::move(lostSequenceIndexSet_);
		return true;
	}

	return false;
}

bool Driver::ContainerSender::isFinished(const Timestamp& timestamp)
{
	ocean_assert(timestamp.isValid());

	if (updateUsageTimestamp_.isInvalid())
	{
		// we are still sending the individual sequences
		return false;
	}

	// we have sent the last sequences, now we wait whether some got lost

	if (timestamp > updateUsageTimestamp_ + 10.0)
	{
		if (!bufferMap_.empty())
		{
			Log::debug() << "Sender container is finished, had " << String::toAString(Scalar(lostSequences_) / Scalar(bufferMap_.size()) * Scalar(100), 1u) << "% lost sequences";
		}

		return true;
	}

	return false;
}

Driver::Driver(std::string zoneName, std::string host, const unsigned int timeout) :
	zoneName_(std::move(zoneName)),
	host_(std::move(host))
{
	ocean_assert(!zoneName_.empty());

	reusableReceivedAmbisonicVoipSamples_.resize(voipAmbisonicChunkSize());

	vertsOptions_ = verts_connection_options_create(verts_make_string(zoneName_.c_str()));

	if (!host_.empty())
	{
		// 'graph.oculus.com' for applications providing an Oculus user access token
		// 'graph.facebook.com' for applications providing an Facebook user access token
		// 'graph.<OD-NUMBER>.od.facebook.com' for On-Demand servers

		verts_connection_options_set_host_override(vertsOptions_, verts_make_string(host_.c_str()));
	}

	ocean_assert(timeout >= 1000u);

	const uint16_t timeoutSeconds = uint16_t(minmax(1u, timeout / 1000u, 120u));

	verts_connection_options_set_inactive_timeout_sec(vertsOptions_, timeoutSeconds);

	if (vertsOptions_ != nullptr)
	{
		vertsConnection_ = verts_connection_create(vertsOptions_);

		if (vertsConnection_ != nullptr)
		{
			timeout_ = (unsigned int)(timeoutSeconds) * 1000u;

			return;
		}
	}

	Log::error() << "VERTS: Failed to initiate driver for zone '" << zoneName_ << "'";

	release();
}

Driver::~Driver()
{
	release();
}

UnorderedIndexSet64 Driver::remoteUserIds()
{
	const ScopedLock scopedLock(lock_);

	UnorderedIndexSet64 userIds;
	userIds.reserve(userMap_.size());

	for (UserMap::const_iterator iUser = userMap_.cbegin(); iUser != userMap_.cend(); ++iUser)
	{
		userIds.emplace(iUser->first);
	}

	return userIds;
}

SharedEntities Driver::entities() const
{
	const ScopedLock scopedLock(lock_);

	SharedEntities entities;
	entities.reserve(entityMap_.size());

	for (EntityMap::const_iterator iEntity = entityMap_.cbegin(); iEntity != entityMap_.cend(); ++iEntity)
	{
		entities.emplace_back(iEntity->second);
	}

	return entities;
}

SharedEntity Driver::newEntity(const NodeSpecification& nodeSpecification)
{
	return newEntity({nodeSpecification.name()});
}

SharedEntity Driver::newEntity(const std::vector<std::string>& nodeTypes)
{
	const ScopedLock scopedLock(lock_);

	if (vertsDriver_ == nullptr)
	{
		ocean_assert(false && "VERTS driver not yet initialized");
		return nullptr;
	}

	const ScopedValue<bool> scopedValue(creatingEntityLocally_, false, true);

	SharedEntity newEntity(new Entity(vertsDriver_, nodeTypes));

	ocean_assert(entityMap_.find(newEntity->entityId()) == entityMap_.cend());
	entityMap_.emplace(newEntity->entityId(), newEntity);

	for (const SharedNode& node : newEntity->nodes())
	{
		ocean_assert(nodeMap_.find(node->nodeId()) == nodeMap_.cend());
		nodeMap_.emplace(node->nodeId(), node);

		node->informHasBeenInitialized(); // local nodes are always initialized
	}

	return newEntity;
}

bool Driver::sendVoipSamples(const int16_t* samples, const size_t size, const VectorF3& position)
{
	ocean_assert(samples != nullptr && size > 0);

	const ScopedLock scopedLock(voipSamplesLock_);

	if (vertsDriver_ == nullptr)
	{
		return false;
	}

	if (monoVoipDataSendQueue_.size() > 20)
	{
		static Timestamp nextWarningTimestamp(true);

		if (Timestamp(true) >= nextWarningTimestamp)
		{
			Log::warning() << "Large voip send queue: " << monoVoipDataSendQueue_.size();

			nextWarningTimestamp = Timestamp(true) + 5.0;
		}

		if (monoVoipDataSendQueue_.size() > 40)
		{
			return false;
		}
	}

	size_t remainingSize = size;

	while (remainingSize != 0)
	{
		if (!monoVoipDataSendQueue_.empty())
		{
			// first, we check whether the latest data is not yet full
			MonoVoipData& lastMonoData = monoVoipDataSendQueue_.back();

			if (lastMonoData.size_ < lastMonoData.voipSamples_.size())
			{
				const size_t samplesToCopy = std::min(size, lastMonoData.voipSamples_.size() - lastMonoData.size_);
				ocean_assert(samplesToCopy > 0 && samplesToCopy <= size && samplesToCopy <= lastMonoData.voipSamples_.size());

				memcpy(lastMonoData.voipSamples_.data() + lastMonoData.size_, samples, samplesToCopy * sizeof(int16_t));
				remainingSize -= samplesToCopy;

				lastMonoData.position_ = position;
				lastMonoData.size_ += samplesToCopy;

				if (remainingSize == 0)
				{
					break;
				}
			}
		}

		ocean_assert(remainingSize != 0);

		if (!reusableMonoVoipDatas_.empty())
		{
			monoVoipDataSendQueue_.emplace(std::move(reusableMonoVoipDatas_.back()));

			reusableMonoVoipDatas_.pop_back();
		}
		else
		{
			monoVoipDataSendQueue_.emplace(MonoVoipData());
		}

		// filling the buffer immediately in the next while iteration
	}

	return true;
}

void Driver::sendOnChannel(const unsigned int channelIndex, Buffer&& buffer)
{
	if (channelIndex >= 6u)
	{
		ocean_assert(false && "Invalid hannel index");
		return;
	}

	ocean_assert(!buffer.empty());
	if (!buffer.empty())
	{
		ocean_assert(buffer.size() <= maximalChannelBufferSize() && "Invalid buffer size");
		if (buffer.size() <= maximalChannelBufferSize())
		{
			const ScopedLock scopedLock(sendOnChannelQueueLock_);

			sendOnChannelBufferQueue_.emplace_back(SendOnChannelObject(channelIndex, std::make_shared<Buffer>(std::move(buffer))));
		}
	}
}

void Driver::sendContainer(const std::string& identifier, const unsigned int version, const void* data, const size_t size)
{
	ocean_assert(data != nullptr && size > 0);
	if (data == nullptr || size == 0)
	{
		return;
	}

	const ScopedLock scopedLock(sendOnChannelQueueLock_);

	constexpr size_t maximalPayloadSize = ContainerReceiver::maximalSequenceSize() - sizeof(ContainerSequenceDataHeader);

	const size_t necessarySequences = (size + maximalPayloadSize - 1) / maximalPayloadSize;
	ocean_assert(necessarySequences < size_t(NumericT<uint32_t>::maxValue()));

	const uint32_t containerSenderId = ++containerSenderIdCounter_;

	ocean_assert(activeContainerSenderMap_.find(containerSenderId) == activeContainerSenderMap_.cend());

	ContainerSender& containerSender = *activeContainerSenderMap_.emplace(containerSenderId, std::make_unique<ContainerSender>(necessarySequences)).first->second;

	ContainerSequenceDataHeader containerSequenceDataHeader(sessionId_, containerSenderId, 0u, uint32_t(necessarySequences));

	const SharedEntity entity = newEntity({NodeSpecification::internalNodeTypeContainer_});
	ocean_assert(entity);
	const SharedNode node = entity->node(NodeSpecification::internalNodeTypeContainer_);
	ocean_assert(node);
	node->setField<uint64_t>(0u, containerSequenceDataHeader.uniqueId());
	node->setField<std::string>(1u, identifier);
	node->setField<uint64_t>(2u, uint64_t(version));
	node->setField<uint64_t>(3u, uint64_t(necessarySequences));

	size_t remainingSize = size;
	const uint8_t* remainingData = (const uint8_t*)(data);

	while (remainingSize != 0)
	{
		const size_t payloadSize = std::min(remainingSize, maximalPayloadSize);
		ocean_assert(payloadSize > 0);

		Buffer sequenceBuffer(payloadSize + sizeof(ContainerSequenceDataHeader));

		ocean_assert(containerSequenceDataHeader.isValid());
		memcpy(sequenceBuffer.data(), &containerSequenceDataHeader, sizeof(ContainerSequenceDataHeader));

		memcpy(sequenceBuffer.data() + sizeof(ContainerSequenceDataHeader), remainingData, payloadSize);

		SharedBuffer sharedBuffer(std::make_shared<Buffer>(std::move(sequenceBuffer)));

		containerSender.addBuffer(containerSequenceDataHeader.sequenceIndex_, sharedBuffer);

		remainingData += payloadSize;

		ocean_assert(payloadSize <= remainingSize);
		remainingSize -= payloadSize;

		const unsigned int containerId = remainingSize == 0 ? containerSenderId : (unsigned int)(-1); // the very last sequence is attached with the container's id

		sendOnChannelBufferQueue_.emplace_back(SendOnChannelObject(internalContainerDataChannelIndex_, std::move(sharedBuffer), containerId));

		++containerSequenceDataHeader.sequenceIndex_;
	}
}

Driver::NewEntityScopedSubscription Driver::addNewEntityCallback(NewEntityCallbackFunction newEntityCallbackFunction, std::string componentType)
{
	ocean_assert(newEntityCallbackFunction);

	const ScopedLock scopedLock(lock_);

	const unsigned int subscriptionId = ++subscriptionIdCounter_;

	ocean_assert(newEntityCallbackFunctionMap_.find(subscriptionId) == newEntityCallbackFunctionMap_.cend());
	newEntityCallbackFunctionMap_.emplace(subscriptionId, std::make_pair(std::move(componentType), std::move(newEntityCallbackFunction)));

	return NewEntityScopedSubscription(subscriptionId, std::bind(&Driver::removeNewEntityCallback, this, std::placeholders::_1));
}

Driver::ReceiveOnChannelScopedSubscription Driver::addReceiveOnChannelCallback(const unsigned int channelIndex, ReceiveOnChannelCallbackFunction receiveOnChannelCallbackFunction)
{
	if (channelIndex >= 6u)
	{
		ocean_assert(false && "Invalid channel index");
		return ReceiveOnChannelScopedSubscription();
	}

	const ScopedLock scopedLock(lock_);

	ocean_assert(channelIndex < receiveOnChannelCallbackHandlers_.size());

	if (!receiveOnChannelCallbackHandlers_[channelIndex].second)
	{
		receiveOnChannelCallbackHandlers_[channelIndex] = std::make_pair(false /*not yet registered*/, std::make_unique<ReceiveOnChannelCallbackHandler>());
	}

	return receiveOnChannelCallbackHandlers_[channelIndex].second->addCallback(std::move(receiveOnChannelCallbackFunction));
}

void Driver::update(const Timestamp& timestamp)
{
	const ScopedLock scopedLock(lock_);

	if (vertsConnection_ != nullptr)
	{
		waitForCreatedDriver();
	}

	if (vertsDriver_ == nullptr)
	{
		return;
	}

	currentTimestamp_ = timestamp;

	verts_driver_loop(vertsDriver_);

	handleChangedUsers();
	handleQueuedVoipSamples();
	handleReceivedVoipSamples();
	handleSendOnChannelQueue();
	handlePendingContainer();
}

bool Driver::waitForCreatedDriver()
{
	ocean_assert(vertsConnection_ != nullptr);

	const VertsConnectionState state = verts_connection_get_status(vertsConnection_);

	switch (state)
	{
		case VertsConnectionState::VertsConnectionState_Initial:
		case VertsConnectionState::VertsConnectionState_HttpRequestSent:
		{
			static Timestamp reportTimestamp(true);

			if (Timestamp(true) > reportTimestamp + 1.0)
			{
				Log::info() << "VERTS: Waiting for connection...";

				reportTimestamp.toNow();
			}

			break;
		}

		case VertsConnectionState::VertsConnectionState_HttpRequestComplete:
		{
			if (vertsDriver_ == nullptr)
			{
				ocean_assert(vertsOptions_ != nullptr);
				verts_connection_options_delete(vertsOptions_);
				vertsOptions_ = nullptr;

				verts_client_options* vertsClientOptions = verts_get_default_client_options();

				vertsDriver_ = verts_connection_create_driver(vertsConnection_, vertsClientOptions, verts_make_string("debug_tag"));
				verts_client_options_destroy(vertsClientOptions);

				if (vertsDriver_ == nullptr)
				{
					Log::error() << "VERTS: Failed to create driver";
					return false;
				}

#ifdef OCEAN_DEBUG

#endif // OCEAN_DEBUG

				Log::info() << "VERTS: Successfully created driver";

				userId_ = verts_driver_get_local_user_id(vertsDriver_);
				sessionId_ = verts_driver_get_local_session(vertsDriver_);

				// registration is normally done automatically, however node specification with RPC fields may be registered too late
				NodeSpecification::registerAllNodeSpecificationsWithDriver(vertsDriver_);

				ocean_assert(onNewEntityCallbackFunctionId_ == -1);
				onNewEntityCallbackFunctionId_ = verts_driver_on_new_entity(vertsDriver_, onNewVertsEntity, this);
				ocean_assert(onNewEntityCallbackFunctionId_ != -1);

				ocean_assert(onDeletedEntityCallbackFunctionId_ == -1);
				onDeletedEntityCallbackFunctionId_ = verts_driver_on_delete_entity(vertsDriver_, onDeletedVertsEntity, this);
				ocean_assert(onDeletedEntityCallbackFunctionId_ != -1);

				ocean_assert(onNewVertsComponentCallbackFunctionId_ == -1);
				onNewVertsComponentCallbackFunctionId_ = verts_driver_on_new_component(vertsDriver_, onNewVertsComponent, this);
				ocean_assert(onNewVertsComponentCallbackFunctionId_ != -1);

				ocean_assert(onDeletedVertsComponentCallbackFunctionId_ == -1);
				onDeletedVertsComponentCallbackFunctionId_ = verts_driver_on_delete_component(vertsDriver_, onDeletedVertsComponent, this);
				ocean_assert(onDeletedVertsComponentCallbackFunctionId_ != -1);

				ocean_assert(onComponentStateChangedCallbackFunctionId_ == -1);
				onComponentStateChangedCallbackFunctionId_ = verts_driver_on_component_state_change(vertsDriver_, onChangedComponentState, this);
				ocean_assert(onComponentStateChangedCallbackFunctionId_ != -1);

				ocean_assert(onVertsComponentUpdatedCallbackFunctionId_ == -1);
				onVertsComponentUpdatedCallbackFunctionId_ = verts_driver_on_component_update(vertsDriver_, onVertsComponentUpdated, this);
				ocean_assert(onVertsComponentUpdatedCallbackFunctionId_ != -1);

				ocean_assert(onVertsComponentFieldChangedCallbackFunctionId_ == -1);
				onVertsComponentFieldChangedCallbackFunctionId_ = verts_driver_on_field_change(vertsDriver_, onVertsComponentFieldChanged, this);
				ocean_assert(onVertsComponentFieldChangedCallbackFunctionId_ != -1);

				const SharedEntity internalSessionEntity = newEntity({NodeSpecification::internalNodeTypeSession_});
				ocean_assert(internalSessionEntity);

				if (internalSessionEntity)
				{
					const SharedNode sessionNode = internalSessionEntity->node(NodeSpecification::internalNodeTypeSession_);
					ocean_assert(sessionNode);

					const uint32_t sessionId = verts_driver_get_local_session(vertsDriver_);
					const uint64_t userId = verts_driver_get_local_user_id(vertsDriver_);
					const uint64_t appId = verts_driver_get_local_app_id(vertsDriver_);

					sessionNode->setField<uint64_t>(0u, sessionId);
					sessionNode->setField<uint64_t>(1u, userId);
					sessionNode->setField<uint64_t>(2u, appId);
				}

				ocean_assert(!receiveOnChannelCallbackHandlers_[internalContainerDataChannelIndex_].second);
				receiveOnChannelCallbackHandlers_[internalContainerDataChannelIndex_] = std::make_pair(false /*not yet registered*/, std::make_unique<ReceiveOnChannelCallbackHandler>());
				receiveOnContainerDataChannelScopedSubscription_ = receiveOnChannelCallbackHandlers_[internalContainerDataChannelIndex_].second->addCallback(std::bind(&Driver::onReceiveOnContainerDataChannel, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

				ocean_assert(!receiveOnChannelCallbackHandlers_[internalContainerResponseChannelIndex_].second);
				receiveOnChannelCallbackHandlers_[internalContainerResponseChannelIndex_] = std::make_pair(false /*not yet registered*/, std::make_unique<ReceiveOnChannelCallbackHandler>());
				receiveOnContainerResponseChannelScopedSubscription_ = receiveOnChannelCallbackHandlers_[internalContainerResponseChannelIndex_].second->addCallback(std::bind(&Driver::onReceiveOnContainerResponseChannel, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
			}

			break;
		}

		case VertsConnectionState::VertsConnectionState_VertsDriverConnected:
		{
			verts_connection_delete(vertsConnection_);
			vertsConnection_ = nullptr;


			Log::debug() << "VERTS: Driver connected for zone '" << verts_driver_get_local_zone_name(vertsDriver_).content << "'";

			break;
		}

		case VertsConnectionState::VertsConnectionState_Error:
		{
			return false;
		}
	}

	return true;
}

void Driver::handleChangedUsers()
{
	// this->lock_ is locked

	addedUserIds_.clear();
	removedUserIds_.clear();

	for (SharedNodeSet::iterator iNode = internalSessionNodes_.begin(); iNode != internalSessionNodes_.end(); /*noop*/)
	{
		Node& node = **iNode;

		if (node.hasBeenDeleted())
		{
			ocean_assert(node.field<uint64_t>(0u) < uint64_t(NumericT<uint32_t>::maxValue()));
			const uint32_t sessionId = node.field<uint64_t>(0u);
			const uint64_t userId = node.field<uint64_t>(1u);

			UserMap::iterator iUser = userMap_.find(userId);

			ocean_assert(iUser != userMap_.cend());
			if (iUser != userMap_.cend())
			{
				UnorderedIndexSet32& sessionIds = iUser->second;

				ocean_assert(sessionIds.find(sessionId) != sessionIds.cend());
				sessionIds.erase(sessionId);

				if (sessionIds.empty())
				{
					removedUserIds_.emplace(userId);
					userMap_.erase(iUser);
				}
			}

			iNode = internalSessionNodes_.erase(iNode);
		}
		else
		{
			if (node.hasChanged())
			{
				ocean_assert(node.field<uint64_t>(0u) < uint64_t(NumericT<uint32_t>::maxValue()));
				const uint32_t sessionId = node.field<uint64_t>(0u);
				const uint64_t userId = node.field<uint64_t>(1u);

				if (userId != 0ull)
				{
					UnorderedIndexSet32& sessionIds = userMap_[userId];

					if (sessionIds.emplace(sessionId).second)
					{
						addedUserIds_.emplace(userId);
					}
				}
			}

			++iNode;
		}
	}

	if (!addedUserIds_.empty() || !removedUserIds_.empty())
	{
		changedUsersCallbackHandler_.callCallbacks(*this, addedUserIds_, removedUserIds_);
	}
}

void Driver::handleQueuedVoipSamples()
{
	// this->lock_ is locked

	const ScopedLock scopedLock(voipSamplesLock_);

	while (!monoVoipDataSendQueue_.empty())
	{
		if (monoVoipDataSendQueue_.front().size_ != monoVoipDataSendQueue_.front().voipSamples_.size())
		{
			// the voip data is not yet full

			ocean_assert(monoVoipDataSendQueue_.size() == 1);
			break;
		}

		MonoVoipData monoVoipData = std::move(monoVoipDataSendQueue_.front());
		monoVoipDataSendQueue_.pop();

		if (monoVoipData.position_.x() != NumericF::minValue())
		{
			const VectorF3& position = monoVoipData.position_;
			verts_voip_component_set_pos(vertsDriver_, verts_vec3{position.x(), position.y(), position.z()});
		}

		const int result = verts_voip_send_pcm(vertsDriver_, monoVoipData.voipSamples_.data(), uint32_t(monoVoipData.voipSamples_.size()));

		if (result != int(monoVoipData.voipSamples_.size()))
		{
			Log::error() << "Failed to send all voip samples";
		}

		monoVoipData.size_ = 0;
		monoVoipData.position_.x() = NumericF::minValue();
		reusableMonoVoipDatas_.emplace_back(std::move(monoVoipData));
	}
}

void Driver::handleReceivedVoipSamples()
{
	// this->lock_ is locked

	ocean_assert(vertsDriver_ != nullptr);

	ocean_assert(!reusableReceivedAmbisonicVoipSamples_.empty());

	while (true)
	{
		const int receivedSamples = verts_voip_get_pcm_ambisonic(vertsDriver_, reusableReceivedAmbisonicVoipSamples_.data(), int(reusableReceivedAmbisonicVoipSamples_.size()));

		if (receivedSamples == 0)
		{
			break;
		}

		if (receivedSamples > 0 && receivedSamples % 4 == 0)
		{
			ocean_assert(receivedSamples <= int(reusableReceivedAmbisonicVoipSamples_.size()));

			const size_t monoReceivedSamples = size_t(receivedSamples) / 4;
			ocean_assert(monoReceivedSamples > 0);

			voipSamplesCallbackHandler_.callCallbacks(*this, reusableReceivedAmbisonicVoipSamples_.data(), monoReceivedSamples);
		}
		else
		{
			Log::error() << "Invalid return value";
			ocean_assert(false && "Invalid return value");
			break;
		}
	}
}

void Driver::handleSendOnChannelQueue()
{
	// this->lock_ is locked

	ocean_assert(vertsDriver_ != nullptr);
	ocean_assert(currentTimestamp_.isValid());

	for (size_t channelIndex = 0; channelIndex < receiveOnChannelCallbackHandlers_.size(); ++channelIndex)
	{
		ReceiveOnChannelCallbackHandlerPair& handlerPair = receiveOnChannelCallbackHandlers_[channelIndex];

		if (handlerPair.second && !handlerPair.first)
		{
			// the channel is not yet registered

			const uint8_t vertsChannelId = uint8_t(channelIndex + 1u);
			ocean_assert(vertsChannelId >= 1u && vertsChannelId <= 8u);

			verts_driver_register_channel_handler(vertsDriver_, vertsChannelId, onReceiveOnChannel, this); // there is no un-register function

			handlerPair.first = true;
		}
	}

	const ScopedLock scopedLock(sendOnChannelQueueLock_);

	unsigned int sentBuffers_ = 0u;

	while (!sendOnChannelBufferQueue_.empty())
	{
		const SendOnChannelObject& object = sendOnChannelBufferQueue_.front();
		ocean_assert(object.buffer_);

		const uint8_t vertsChannelId = object.channelIndex_ + 1u;
		ocean_assert(vertsChannelId >= 1u && vertsChannelId <= 8u);
		ocean_assert(object.buffer_->size() <= size_t(NumericT<uint32_t>::maxValue()));

		const size_t bufferSize = object.buffer_->size();

		verts_driver_send_on_channel(vertsDriver_, vertsChannelId, object.buffer_->data(), uint32_t(object.buffer_->size()));

		if (object.lastObjectForContainerId_ != (unsigned int)(-1))
		{
			// this was the last sequence of the entire container

			ContainerSenderMap::iterator iContainer = activeContainerSenderMap_.find(object.lastObjectForContainerId_);
			ocean_assert(iContainer != activeContainerSenderMap_.cend());

			if (iContainer != activeContainerSenderMap_.cend())
			{
				iContainer->second->informLastSequenceSent(currentTimestamp_);
			}
		}

		sendOnChannelBufferQueue_.pop_front();

		const Timestamp currentSystemTimestamp(true);

		channelsThroughputCalculator_.addOccurance(currentSystemTimestamp, double(bufferSize));

		if (++sentBuffers_ >= 50u || channelsThroughputCalculator_.rate(currentSystemTimestamp) > maximalThroughputChannels_)
		{
			break;
		}

#ifdef OCEAN_DEBUG
		if (sendOnChannelBufferQueue_.size() > 10)
		{
			double currentRate;
			if (channelsThroughputCalculator_.rateEveryNSeconds(Timestamp(true), currentRate, 1.0))
			{
				Log::debug() << "Send on channel throughput: " << currentRate / (1024.0 * 1024.0) << "MB/s";
			}
		}
#endif
	}
}

void Driver::handlePendingContainer()
{
	// this->lock_ is locked

	ocean_assert(vertsDriver_ != nullptr);
	ocean_assert(currentTimestamp_.isValid());

	constexpr size_t maximalPerResponseIds = 250;
	static_assert(maximalPerResponseIds * sizeof(Index32) + sizeof(ContainerSequenceResponseHeader) <= maximalChannelBufferSize());
	constexpr size_t maximalIds = maximalPerResponseIds * 4;

	Indices32 lostSessionIds;

	for (ContainerReceiverMap::iterator iContainer = pendingContainerReceiverMap_.begin(); iContainer != pendingContainerReceiverMap_.end(); ++iContainer)
	{
		ocean_assert(iContainer->second);
		ContainerReceiver& containerReceiver = *iContainer->second;

		if (containerReceiver.needResentLostSequences(currentTimestamp_, lostSessionIds, 0.25, 5.0, maximalIds))
		{
			const ContainerSequenceResponseHeader responseHeader = containerReceiver.responseHeader();

			Index32* lostIds = lostSessionIds.data();
			size_t remainingIds = lostSessionIds.size();

			const ScopedLock scopedLock(sendOnChannelQueueLock_);

			while (remainingIds != 0)
			{
				const size_t size = std::min(remainingIds, maximalPerResponseIds);

				Buffer buffer(sizeof(ContainerSequenceResponseHeader) + size * sizeof(Index32));
				memcpy(buffer.data(), &responseHeader, sizeof(ContainerSequenceResponseHeader));

				memcpy(buffer.data() + sizeof(ContainerSequenceResponseHeader), lostIds, size * sizeof(Index32));

				ocean_assert(buffer.size() <= maximalChannelBufferSize() && "Invalid buffer size");

				// we move the response to the front of the buffer queue

				sendOnChannelBufferQueue_.emplace_front(SendOnChannelObject(internalContainerResponseChannelIndex_, std::make_shared<Buffer>(std::move(buffer))));

				Log::debug() << "Sent resent request for " << size << " sequences, first one is " << *lostIds;

				lostIds += size;
				remainingIds -= size;
			}
		}
	}

	UnorderedIndexSet64 lostSequenceIndices;

	for (ContainerSenderMap::iterator iContainer = activeContainerSenderMap_.begin(); iContainer != activeContainerSenderMap_.end(); /*noop*/)
	{
		ContainerSender& containerSender = *iContainer->second;

		if (containerSender.isFinished(currentTimestamp_))
		{
			Log::debug() << "Removed sender container";

			iContainer = activeContainerSenderMap_.erase(iContainer);
		}
		else
		{
			if (containerSender.needsResentLostSequences(currentTimestamp_, lostSequenceIndices, 0.25))
			{
				ocean_assert(!lostSequenceIndices.empty());

				const ScopedLock scopedLock(sendOnChannelQueueLock_);

				const IndexSet32 orderedIndices(lostSequenceIndices.cbegin(), lostSequenceIndices.cend());

				for (IndexSet32::const_reverse_iterator iIndex = orderedIndices.crbegin(); iIndex != orderedIndices.crend(); ++iIndex)
				{
					SharedBuffer sequenceBuffer = containerSender.buffer(*iIndex);

					if (sequenceBuffer)
					{
						// we move the sequence to the front of the buffer queue

						sendOnChannelBufferQueue_.emplace_front(SendOnChannelObject(internalContainerDataChannelIndex_, std::move(sequenceBuffer)));
					}
					else
					{
						ocean_assert(false && "This should never happen!");
						break;
					}
				}

				Log::debug() << "Re-sent " << lostSequenceIndices.size() << " sequence due to reported losts";
			}

			++iContainer;
		}
	}

	for (SharedNodeSet::iterator iNode = internalContainerNodes_.begin(); iNode != internalContainerNodes_.end(); /*noop*/)
	{
		Node& node = **iNode;

		if (node.isInitialized())
		{
			const uint64_t uniqueId = node.field<uint64_t>(0u);
			ocean_assert(uniqueId != 0ull);

			std::string identifier = node.field<std::string>(1u);
			const uint64_t version = node.field<uint64_t>(2u);
			const uint64_t numberSequences = node.field<uint64_t>(3u);

			ocean_assert(finishedContainerIdSet_.find(uniqueId) == finishedContainerIdSet_.cend()); // can not be finished as the identifier was unknown

			ContainerReceiverMap::iterator iReceiver = pendingContainerReceiverMap_.find(uniqueId);

			if (iReceiver == pendingContainerReceiverMap_.cend())
			{
				// we have not received a sequence for the container, so we add the container here

				Log::debug() << "Added container '" << identifier << "', " << version << " without container";

				uint32_t sessionId = 0u;
				uint32_t containerId = 0u;
				ContainerSequenceDataHeader::decomposeUniqueId(uniqueId, sessionId, containerId);

				constexpr uint32_t sequenceIndex = 0ul;

				const ContainerSequenceDataHeader containerSequenceDataHeader(sessionId, containerId, sequenceIndex, uint32_t(numberSequences));

				iReceiver = pendingContainerReceiverMap_.emplace(uniqueId, std::make_unique<ContainerReceiver>(containerSequenceDataHeader, currentTimestamp_)).first;
			}

			ContainerReceiver& containerReceiver = *iReceiver->second;

			containerReceiver.setIdentifier(std::move(identifier), (unsigned int)(version));

			if (containerReceiver.isFinished())
			{
				const uint64_t userId = verts_driver_get_user_for_session(vertsDriver_, containerReceiver.sessionId());

				std::string internalIdentifier;
				unsigned int internalVersion = 0u;
				const SharedBuffer containerBuffer = containerReceiver.finishedBuffer(internalIdentifier, internalVersion, currentTimestamp_);

				receiveContainerCallbackHandler_.callCallbacks(*this, containerReceiver.sessionId(), userId, internalIdentifier, internalVersion, containerBuffer);

				pendingContainerReceiverMap_.erase(iReceiver);

				finishedContainerIdSet_.emplace(uniqueId);
			}

			iNode = internalContainerNodes_.erase(iNode);
			continue;
		}

		++iNode;
	}
}

void Driver::removeNewEntityCallback(const unsigned int& subscriptionId)
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(newEntityCallbackFunctionMap_.find(subscriptionId) != newEntityCallbackFunctionMap_.cend());
	newEntityCallbackFunctionMap_.erase(subscriptionId);
}

void Driver::release()
{
	nodeMap_.clear();
	entityMap_.clear();

	if (vertsOptions_ != nullptr)
	{
		verts_connection_options_delete(vertsOptions_);
		vertsOptions_ = nullptr;
	}

	if (vertsConnection_ != nullptr)
	{
		verts_connection_delete(vertsConnection_);
		vertsConnection_ = nullptr;
	}

	receiveOnContainerDataChannelScopedSubscription_.release();
	receiveOnContainerResponseChannelScopedSubscription_.release();

	if (vertsDriver_ != nullptr)
	{
		if (onNewEntityCallbackFunctionId_ != -1)
		{
			verts_driver_on_new_entity_remove(vertsDriver_, onNewEntityCallbackFunctionId_);
			onNewEntityCallbackFunctionId_ = -1;
		}

		if (onDeletedEntityCallbackFunctionId_ != -1)
		{
			verts_driver_on_delete_entity_remove(vertsDriver_, onDeletedEntityCallbackFunctionId_);
			onDeletedEntityCallbackFunctionId_ = -1;
		}

		if (onNewVertsComponentCallbackFunctionId_ != -1)
		{
			verts_driver_on_new_component_remove(vertsDriver_, onNewVertsComponentCallbackFunctionId_);
			onNewVertsComponentCallbackFunctionId_ = -1;
		}

		if (onDeletedVertsComponentCallbackFunctionId_ != -1)
		{
			verts_driver_on_delete_component_remove(vertsDriver_, onDeletedVertsComponentCallbackFunctionId_);
			onDeletedVertsComponentCallbackFunctionId_ = -1;
		}

		if (onComponentStateChangedCallbackFunctionId_ != -1)
		{
			verts_driver_on_component_state_change_remove(vertsDriver_, onComponentStateChangedCallbackFunctionId_);
			onComponentStateChangedCallbackFunctionId_ = -1;
		}

		if (onVertsComponentUpdatedCallbackFunctionId_ != -1)
		{
			verts_driver_on_component_update_remove(vertsDriver_, onVertsComponentUpdatedCallbackFunctionId_);
			onVertsComponentUpdatedCallbackFunctionId_ = -1;
		}

		if (onVertsComponentFieldChangedCallbackFunctionId_ != -1)
		{
			verts_driver_on_field_change_remove(vertsDriver_, uint32_t(onVertsComponentFieldChangedCallbackFunctionId_));
			onVertsComponentFieldChangedCallbackFunctionId_ = -1;
		}

		verts_driver_delete(vertsDriver_);
		vertsDriver_ = nullptr;
	}

	ocean_assert(onNewEntityCallbackFunctionId_ == -1);
	ocean_assert(onDeletedEntityCallbackFunctionId_ == -1);
	ocean_assert(onNewVertsComponentCallbackFunctionId_ == -1);
	ocean_assert(onDeletedVertsComponentCallbackFunctionId_ == -1);
	ocean_assert(onVertsComponentUpdatedCallbackFunctionId_ == -1);
	ocean_assert(onVertsComponentFieldChangedCallbackFunctionId_ == -1);
}

void Driver::onNewInternalNode(const SharedNode& node)
{
	// this->lock_ is locked

	if (node->nodeType() == NodeSpecification::internalNodeTypeSession_)
	{
		ocean_assert(internalSessionNodes_.find(node) == internalSessionNodes_.cend());
		internalSessionNodes_.emplace(node);
	}
	else if (node->nodeType() == NodeSpecification::internalNodeTypeContainer_)
	{
		ocean_assert(internalContainerNodes_.find(node) == internalContainerNodes_.cend());
		internalContainerNodes_.emplace(node);
	}
}

void Driver::onDeletedInternalNode(const SharedNode& node)
{
	// this->lock_ is locked
}

void Driver::onNewVertsEntity(verts_entity* vertsEntity)
{
	ocean_assert(vertsEntity != nullptr);

	// this->lock_ is locked

	if (creatingEntityLocally_)
	{
		return;
	}

	const uint64_t vertsEntitytId = verts_entity_get_id(vertsEntity);

	if (entityMap_.find(vertsEntitytId) == entityMap_.cend())
	{
		// the entity is an external entity, so we need to create a cooresponding data structure

#ifdef OCEAN_DEBUG
		// new entities will be empty when first created, the corresponding components will be created immediately afterwards
		const int numberComponents = verts_entity_get_num_components(vertsEntity);
		ocean_assert(numberComponents == 0);
#endif

		SharedEntity newEntity(new Entity(vertsEntity));

		auto&& entityId = newEntity->entityId();
		entityMap_.emplace(entityId, std::move(newEntity));

		Log::debug() << "VERTS: Added new external entity";
	}
	else
	{
		ocean_assert(false && "This should never happen!");
	}
}

void Driver::onDeletedVertsEntity(verts_entity* vertsEntity)
{
	ocean_assert(vertsEntity != nullptr);

	// this->lock_ is locked

	const uint64_t vertsEntitytId = verts_entity_get_id(vertsEntity);

	EntityMap::iterator iEntity = entityMap_.find(vertsEntitytId);

	if (iEntity != entityMap_.cend())
	{
		iEntity->second->informHasBeenDeleted();

		entityMap_.erase(iEntity);
	}
}

void Driver::onNewVertsComponent(verts_component* vertsComponent)
{
	ocean_assert(vertsComponent != nullptr);

	// this->lock_ is locked

	if (creatingEntityLocally_)
	{
		return;
	}

	if (!verts_component_is_locally_owned(vertsComponent))
	{
		const uint64_t vertsEntityId = verts_component_get_entity_id(vertsComponent);

		// we are not the owner, so we need to ensure that we know the corresponding NodeSpecification (field set)

		const verts_string typeName = verts_component_get_type_name(vertsComponent);
		ocean_assert(typeName.length != 0 && typeName.content != nullptr);

		verts_field_set* vertsFieldSet = verts_driver_get_fieldset_cstruct(vertsDriver_, typeName);
		const NodeSpecification* nodeSpecification = NodeSpecification::newNodeSpecification(vertsDriver_, vertsComponent, vertsFieldSet);

		// now, let's add a Node data structure to the corresponding entity to represent the new component

		EntityMap::iterator iEntity = entityMap_.find(vertsEntityId);

		if (iEntity != entityMap_.cend() && nodeSpecification != nullptr)
		{
			SharedNode node = iEntity->second->addNode(vertsComponent, *nodeSpecification);

			if (NodeSpecification::isInternalNodeType(node->nodeType()))
			{
				onNewInternalNode(node);
			}

			ocean_assert(nodeMap_.find(node->nodeId()) == nodeMap_.cend());
			auto&& nodeId = node->nodeId();
			nodeMap_.emplace(nodeId, std::move(node));

			Log::debug() << "VERTS: Added new node '" << nodeSpecification->name() << "' to external entity";

			// now, we inform everyone interested in the new entity

			for (NewEntityCallbackFunctionMap::const_iterator iCallback = newEntityCallbackFunctionMap_.cbegin(); iCallback != newEntityCallbackFunctionMap_.cend(); ++iCallback)
			{
				if (iCallback->second.first.empty() || iCallback->second.first == nodeSpecification->name())
				{
					iCallback->second.second(*this, iEntity->second);
				}
			}
		}
		else
		{
			ocean_assert(false && "This should never happen");
		}
	}
}

void Driver::onDeletedVertsComponent(verts_component* vertsComponent)
{
	ocean_assert(vertsComponent != nullptr);

	// this->lock_ is locked

	const uint64_t vertsComponentId = verts_component_get_id(vertsComponent);

	NodeMap::iterator iNode = nodeMap_.find(vertsComponentId);

	if (iNode != nodeMap_.cend())
	{
		SharedNode& node = iNode->second;

		node->informHasBeenDeleted();

		if (NodeSpecification::isInternalNodeType(node->nodeType()))
		{
			onDeletedInternalNode(node);
		}

		nodeMap_.erase(iNode);
	}
}

void Driver::onChangedComponentState(const verts_component* vertsComponent, VertsObjectState vertsObjectState)
{
	ocean_assert(vertsComponent != nullptr);

	// this->lock_ is locked

	const uint64_t vertsComponentId = verts_component_get_id(vertsComponent);

	NodeMap::iterator iNode = nodeMap_.find(vertsComponentId);

	if (iNode != nodeMap_.cend())
	{
		SharedNode& node = iNode->second;

		if (VertsObjectState(vertsObjectState) == VertsObjectState::VertsObjectState_Confirmed)
		{
			node->informHasBeenInitialized();
		}
	}
}

void Driver::onVertsComponentUpdated(verts_component* vertsComponent, uint64_t serverTime, uint64_t previousServerTime)
{
	ocean_assert(vertsComponent != nullptr);

	// this->lock_ is locked
}

void Driver::onVertsComponentFieldChanged(verts_component* vertsComponent, const verts_string name, uint64_t timestamp)
{
	ocean_assert(vertsComponent != nullptr);
	ocean_assert(name.length != 0);

	// this->lock_ is locked

	if (name.content[0] == '_')
	{
		// skipping internal fields
		return;
	}

	std::string fieldName(name.content);

	const uint64_t vertsComponentId = verts_component_get_id(vertsComponent);

	NodeMap::iterator iNode = nodeMap_.find(vertsComponentId);

	if (iNode != nodeMap_.cend())
	{
		iNode->second->informChangedField(fieldName);
	}
}

void Driver::onReceiveOnChannel(uint8_t channelId, uint64_t sequenceNumber, const uint8_t* buffer, uint32_t bufferSize)
{
	ocean_assert(channelId >= 1 && channelId <= 8u);
	ocean_assert(buffer != nullptr);
	ocean_assert(bufferSize >= 1u);

	// this->lock_ is locked

	static_assert(VERTS_CHANNEL_USER_MIN == 1u && VERTS_CHANNEL_USER_MAX == 8u, "Invalid user channel layout");

	const unsigned int channelIndex = (unsigned int)(channelId) - 1u;
	ocean_assert(channelIndex < receiveOnChannelCallbackHandlers_.size());

	ocean_assert(receiveOnChannelCallbackHandlers_[channelIndex].second);
	receiveOnChannelCallbackHandlers_[channelIndex].second->callCallbacks(*this, channelIndex, buffer, bufferSize);
}

void Driver::onReceiveOnContainerDataChannel(Driver& driver, const unsigned int channelIndex, const void* buffer, const size_t size)
{
	ocean_assert(channelIndex == internalContainerDataChannelIndex_);
	ocean_assert(buffer != nullptr);
	ocean_assert(size != 0);

	// this->lock_ is locked

	static_assert(sizeof(ContainerSequenceDataHeader) == sizeof(uint32_t) * 4, "Invalid data type!");

	if (size <= sizeof(ContainerSequenceDataHeader))
	{
		Log::error() << "Invalid container data sequence with size " << size << " bytes";
		ocean_assert(false && "Invalid container data sequence");
		return;
	}

	ContainerSequenceDataHeader containerSequenceDataHeader;
	memcpy(&containerSequenceDataHeader, buffer, sizeof(ContainerSequenceDataHeader));

	if (!containerSequenceDataHeader.isValid())
	{
		Log::error() << "Invalid container header";
		ocean_assert(false && "Invalid container header");
		return;
	}

	const uint64_t uniqueContainerId = containerSequenceDataHeader.uniqueId();

	ContainerReceiverMap::iterator iContainer = pendingContainerReceiverMap_.find(uniqueContainerId);

	if (iContainer == pendingContainerReceiverMap_.cend())
	{
		if (finishedContainerIdSet_.find(uniqueContainerId) != finishedContainerIdSet_.cend())
		{
			// the container is already handled, we can drop the buffer
			return;
		}

		iContainer = pendingContainerReceiverMap_.emplace(uniqueContainerId, std::make_unique<ContainerReceiver>(containerSequenceDataHeader, currentTimestamp_)).first;
	}

	ocean_assert(iContainer != pendingContainerReceiverMap_.cend());

	ContainerReceiver& containerReceiver = *iContainer->second;

	const void* payloadBuffer = (const uint8_t*)(buffer) + sizeof(ContainerSequenceDataHeader);
	const size_t payloadSize = size - sizeof(ContainerSequenceDataHeader);
	ocean_assert(payloadSize < size);

	if (containerReceiver.setSequence(containerSequenceDataHeader.sequenceIndex_, payloadBuffer, payloadSize, currentTimestamp_))
	{
		if (containerReceiver.isFinished())
		{
			const uint64_t userId = verts_driver_get_user_for_session(vertsDriver_, containerReceiver.sessionId());

			std::string identifier;
			unsigned int version = 0u;
			const SharedBuffer containerBuffer = containerReceiver.finishedBuffer(identifier, version, currentTimestamp_);

			receiveContainerCallbackHandler_.callCallbacks(*this, containerReceiver.sessionId(), userId, identifier, version, containerBuffer);

			pendingContainerReceiverMap_.erase(iContainer);

			finishedContainerIdSet_.emplace(uniqueContainerId);
		}
	}
	else
	{
		// the buffer was invalid, we will drop the entire container
		finishedContainerIdSet_.emplace(uniqueContainerId);
	}
}

void Driver::onReceiveOnContainerResponseChannel(Driver& driver, const unsigned int channelIndex, const void* buffer, const size_t size)
{
	ocean_assert(channelIndex == internalContainerResponseChannelIndex_);
	ocean_assert(buffer != nullptr);
	ocean_assert(size != 0);

	// this->lock_ is locked

	static_assert(sizeof(ContainerSequenceResponseHeader) == sizeof(uint32_t) * 2, "Invalid data type!");

	if (size <= sizeof(ContainerSequenceResponseHeader))
	{
		Log::error() << "Invalid container response with size " << size << " bytes";
		ocean_assert(false && "Invalid container response");
		return;
	}

	ContainerSequenceResponseHeader containerSequenceResponseHeader;
	memcpy(&containerSequenceResponseHeader, buffer, sizeof(ContainerSequenceResponseHeader));

	if (containerSequenceResponseHeader.sessionId_ != sessionId_)
	{
		return;
	}

	const size_t payloadSize = size - sizeof(ContainerSequenceResponseHeader);
	ocean_assert(payloadSize > 0);

	if (payloadSize % 4 != 0)
	{
		Log::error() << "Invalid payload size of container response";
		ocean_assert(false && "Invalid payload size of container response");
		return;
	}

	const void* payload = (const uint8_t*)(buffer) + sizeof(ContainerSequenceResponseHeader);

	const size_t numberSessionIds = payloadSize / 4;

	const Index32* ids = nullptr;
	Indices32 lostSessionIds;

	if (size_t(buffer) % sizeof(Index32) == 0)
	{
		ids = (const Index32*)(payload);
	}
	else
	{
		lostSessionIds.resize(numberSessionIds);
		memcpy(lostSessionIds.data(), payload, lostSessionIds.size() * sizeof(Index32));

		ids = lostSessionIds.data();
	}

	const ScopedLock scopedLock(sendOnChannelQueueLock_);

	ContainerSenderMap::iterator iContainer = activeContainerSenderMap_.find(containerSequenceResponseHeader.containerId_);

	if (iContainer == activeContainerSenderMap_.cend())
	{
		Log::warning() << "The container with id " << containerSequenceResponseHeader.containerId_ << " does not exist anymore, we cannot send the lost sequences again";
		return;
	}

	ocean_assert(currentTimestamp_.isValid());
	iContainer->second->addLostSequenceIndices(ids, numberSessionIds, currentTimestamp_);
}

void Driver::onNewVertsEntity(verts_entity* vertsEntity, void* context)
{
	ocean_assert(context != nullptr);
	Driver* driver = static_cast<Driver*>(context);

	driver->onNewVertsEntity(vertsEntity);
}

void Driver::onDeletedVertsEntity(verts_entity* vertsEntity, void* context)
{
	ocean_assert(context != nullptr);
	Driver* driver = static_cast<Driver*>(context);

	driver->onDeletedVertsEntity(vertsEntity);
}

void Driver::onNewVertsComponent(verts_component* vertsComponent, void* context)
{
	ocean_assert(context != nullptr);
	Driver* driver = static_cast<Driver*>(context);

	driver->onNewVertsComponent(vertsComponent);
}

void Driver::onDeletedVertsComponent(verts_component* vertsComponent, void* context)
{
	ocean_assert(context != nullptr);
	Driver* driver = static_cast<Driver*>(context);

	driver->onDeletedVertsComponent(vertsComponent);
}

void Driver::onChangedComponentState(const verts_component* vertsComponent, VertsObjectState vertsObjectState, void* context)
{
	static_assert(std::is_same<int, std::underlying_type<VertsObjectState>::type>::value, "Invalid data type!");

	ocean_assert(context != nullptr);
	Driver* driver = static_cast<Driver*>(context);

	driver->onChangedComponentState(vertsComponent, vertsObjectState);
}

void Driver::onVertsComponentUpdated(verts_component* vertsComponent, uint64_t serverTime, uint64_t previousServerTime, void* context)
{
	ocean_assert(context != nullptr);
	Driver* driver = static_cast<Driver*>(context);

	driver->onVertsComponentUpdated(vertsComponent, serverTime, previousServerTime);
}

void Driver::onVertsComponentFieldChanged(verts_component* vertsComponent, const verts_string name, uint64_t timestamp, void* context)
{
	ocean_assert(context != nullptr);
	Driver* driver = static_cast<Driver*>(context);

	driver->onVertsComponentFieldChanged(vertsComponent, name, timestamp);
}

void Driver::onReceiveOnChannel(uint8_t channelId, uint64_t sequenceNumber, const uint8_t* buffer, uint32_t bufferSize, void* context)
{
	ocean_assert(context != nullptr);
	Driver* driver = static_cast<Driver*>(context);

#ifdef OCEAN_DEBUG
	{
		static RateCalculator receiveRateCalculator;

		const Timestamp currentSystemTimestamp(true);

		receiveRateCalculator.addOccurance(currentSystemTimestamp, double(bufferSize));

		double currentRate;
		if (receiveRateCalculator.rateEveryNSeconds(currentSystemTimestamp, currentRate, 1.0))
		{
			Log::debug() << "Receive on channel throughput: " << currentRate / (1024.0 * 1024.0) << "MB/s";
		}
	}
#endif

	driver->onReceiveOnChannel(channelId, sequenceNumber, buffer, bufferSize);
}

}

}

}
