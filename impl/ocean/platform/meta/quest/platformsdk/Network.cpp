// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/platform/meta/quest/platformsdk/Network.h"

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

Network::SenderPackage::SenderPackage(const uint64_t receiverUserId, const unsigned int packageId, const ComponentId componentId, std::vector<uint8_t>&& data) :
	receiverUserId_(receiverUserId),
	packageId_(packageId),
	componentId_(componentId),
	packageData_(std::move(data))
{
	ocean_assert(!packageData_.empty());

	numberChunks_ = (packageData_.size() + maximalPayloadSize_ - 1) / maximalPayloadSize_;
	chunkData_.resize(maximalPackageSize_);

	nextChunk();
}

uint64_t Network::SenderPackage::receiverUserId() const
{
	return receiverUserId_;
}

const void* Network::SenderPackage::currentChunkData() const
{
	return chunkData_.data();
}

size_t Network::SenderPackage::currentChunkSize() const
{
	ocean_assert(currentChunkSize_ == maximalPackageSize_ || currentChunkIndex_ + 1 == numberChunks_);

	return currentChunkSize_;
}

bool Network::SenderPackage::nextChunk()
{
	ocean_assert(currentChunkIndex_ == size_t(-1) || currentChunkIndex_ < numberChunks_);

	++currentChunkIndex_;

	if (currentChunkIndex_ >= numberChunks_)
	{
		return false;
	}

	indexInPackageData_ += payloadInCurrentChunk_;
	ocean_assert(indexInPackageData_ < packageData_.size());

	ocean_assert(chunkData_.size() == maximalPackageSize_);

	// the chunk layout
	// 4 bytes: version
	// 4 bytes: purpose type
	// 4 bytes: distribution type
	// 4 bytes: packageId
	// 4 bytes: componentId
	// 4 bytes: chunks
	// 4 bytes: chunkId
	// remaining: payload

	const uint32_t chunks = uint32_t(numberChunks_);
	const uint32_t chunkId = uint32_t(currentChunkIndex_);

	ocean_assert(chunks >= 1u && chunkId < chunks);

	constexpr PurposeType purposeType = PT_PACKAGE;

	const DistributionType distributionType = receiverUserId_ == uint64_t(0) ? DT_ROOM : DT_UNIQUE_USER;

	static_assert(sizeof(purposeType) == sizeof(uint32_t), "Invalid data type!");
	static_assert(sizeof(distributionType) == sizeof(uint32_t), "Invalid data type!");
	static_assert(sizeof(componentId_) == sizeof(uint32_t), "Invalid data type!");

	constexpr uint32_t version = 1u;

	memcpy(chunkData_.data() + sizeof(uint32_t) * 0, &version, sizeof(uint32_t));
	memcpy(chunkData_.data() + sizeof(uint32_t) * 1, &purposeType, sizeof(uint32_t));
	memcpy(chunkData_.data() + sizeof(uint32_t) * 2, &distributionType, sizeof(uint32_t));
	memcpy(chunkData_.data() + sizeof(uint32_t) * 3, &packageId_, sizeof(uint32_t));
	memcpy(chunkData_.data() + sizeof(uint32_t) * 4, &componentId_, sizeof(uint32_t));
	memcpy(chunkData_.data() + sizeof(uint32_t) * 5, &chunks, sizeof(uint32_t));
	memcpy(chunkData_.data() + sizeof(uint32_t) * 6, &chunkId, sizeof(uint32_t));

	ocean_assert(indexInPackageData_ < packageData_.size());
	payloadInCurrentChunk_ = std::min(packageData_.size() - indexInPackageData_, maximalPayloadSize_);
	ocean_assert(payloadInCurrentChunk_ >= 1 && payloadInCurrentChunk_ <= maximalPayloadSize_);

	memcpy(chunkData_.data() + sizeof(uint32_t) * 7, packageData_.data() + indexInPackageData_, payloadInCurrentChunk_);

	currentChunkSize_ = payloadInCurrentChunk_ + sizeof(uint32_t) * 7;

	return true;
}

bool Network::ReceiverPackage::addChunk(const uint8_t* data, const size_t size)
{
	if (size <= sizeof(uint32_t) * 7)
	{
		return false;
	}

	// we skip the verion, purpose type, distribution type, and package id

	static_assert(sizeof(componentId_) == sizeof(uint32_t), "Invalid data type!");

	ComponentId componentId;
	memcpy(&componentId, data + sizeof(uint32_t) * 4, sizeof(componentId));

	uint32_t chunks;
	memcpy(&chunks, data + sizeof(uint32_t) * 5, sizeof(uint32_t));

	uint32_t chunkId;
	memcpy(&chunkId, data + sizeof(uint32_t) * 6, sizeof(uint32_t));

	if (componentId_ == CI_UNKNOWN)
	{
		componentId_ = componentId;
	}
	else if (componentId_ != componentId)
	{
		ocean_assert(false && "The component id has changed");
		return false;
	}

	if (chunks == 0u || chunkId >= chunks)
	{
		ocean_assert(false && "Invalid chunk id!");
		return false;
	}

	if (numberChunks_ == 0u)
	{
		numberChunks_ = size_t(chunks);

		packageData_.resize(numberChunks_ * maximalPayloadSize_);
	}
	else if (numberChunks_ != size_t(chunks))
	{
		ocean_assert(false && "The number of chunks has changed!");
		return false;
	}

	if (readChunks_ == numberChunks_)
	{
		ocean_assert(false && "we have read all chunks already");
		return false;
	}

	const size_t palyloadSize = size - sizeof(uint32_t) * 7;
	ocean_assert(palyloadSize >= 1 && palyloadSize <= maximalPayloadSize_);

	const size_t offsetInPackageData = chunkId * maximalPayloadSize_;

	memcpy(packageData_.data() + offsetInPackageData, data + sizeof(uint32_t) * 7, palyloadSize);

	if (chunkId + 1u == numberChunks_)
	{
		// we have the last chunk, now we know the size of the package data

		if (dataSize_ != 0)
		{
			ocean_assert(false && "We have received the last chunk twice!");
			return false;
		}

		dataSize_ = offsetInPackageData + palyloadSize;
	}

	++readChunks_;

	return true;
}

Network::ComponentId Network::ReceiverPackage::componentId() const
{
	return componentId_;
}

const void* Network::ReceiverPackage::data() const
{
	ocean_assert(isCompleted());
	return packageData_.data();
}

size_t Network::ReceiverPackage::dataSize() const
{
	return dataSize_;
}

bool Network::ReceiverPackage::isCompleted() const
{
	return readChunks_ == numberChunks_;
}

Network::Network()
{
	messageScopedSubscriptions_.emplace_back(subscribeForMessageResponse(ovrMessage_Notification_Networking_ConnectionStateChange, &Network::onNotificationConnectionStateChange));
	messageScopedSubscriptions_.emplace_back(subscribeForMessageResponse(ovrMessage_Notification_Networking_PeerConnectRequest, &Network::onNotificationPeerConnectRequest));

	startThread();
}

Network::~Network()
{
	messageScopedSubscriptions_.clear();

	stopThreadExplicitly();
}

bool Network::acceptAllConnectionsForCurrentRoom()
{
	return ovr_Net_AcceptForCurrentRoom();
}

void Network::acceptConnectionForUser(const uint64_t userId)
{
	ocean_assert(userId != 0ull);

	const ScopedLock scopedLock(lock_);

	acceptConnectionUserIdSet_.emplace(userId);
}

bool Network::connect(const uint64_t receiverUserId)
{
	ocean_assert(receiverUserId != 0ull);

	ovr_Net_Connect(receiverUserId);
	return true;
}

bool Network::isConnected(const uint64_t receiverUserId) const
{
	ocean_assert(receiverUserId != 0ull);

	return ovr_Net_IsConnected(receiverUserId);
}

bool Network::sendToRoomUDP(const ComponentId componentId, const void* data, const size_t size)
{
	ocean_assert(data != nullptr && size != 0);

	static_assert(sizeof(ComponentId) == sizeof(uint32_t), "Invalid data type!");
	const size_t totalSize = size + sizeof(uint32_t);

	if (totalSize >= 1200)
	{
		ocean_assert(false && "UDP package is too large");
		return false;
	}

	std::vector<uint8_t> buffer(totalSize);

	memcpy(buffer.data(), &componentId, sizeof(uint32_t));
	memcpy(buffer.data() + sizeof(uint32_t), data, size);

	sendRateCalculatorUDP_.addOccurance(Timestamp(true), double(buffer.size()));

	return ovr_Net_SendPacketToCurrentRoom(buffer.size(), buffer.data(), ovrSend_Unreliable);
}

bool Network::sendToRoomTCP(const ComponentId componentId, std::vector<uint8_t>&& data)
{
	ocean_assert(!data.empty());

	if (data.empty())
	{
		return false;
	}

	const ScopedLock scopedLock(senderPackageLock_);

	constexpr uint64_t receiverUserId = 0ull; // sending to the room instead of a unique user

	senderPackageQueue_.emplace(receiverUserId, ++senderPackageIdCounter_, componentId, std::move(data));

	return true;
}

bool Network::sendToUserUDP(const uint64_t receiverUserId, const ComponentId componentId, const void* data, const size_t size)
{
	ocean_assert(receiverUserId != 0ull);
	ocean_assert(data != nullptr && size != 0);

	static_assert(sizeof(ComponentId) == sizeof(uint32_t), "Invalid data type!");
	const size_t totalSize = size + sizeof(uint32_t);

	if (totalSize >= 1200)
	{
		ocean_assert(false && "UDP package is too large");
		return false;
	}

	std::vector<uint8_t> buffer(totalSize);

	memcpy(buffer.data(), &componentId, sizeof(uint32_t));
	memcpy(buffer.data() + sizeof(uint32_t), data, size);

	sendRateCalculatorUDP_.addOccurance(Timestamp(true), double(buffer.size()));

	return ovr_Net_SendPacket(receiverUserId, buffer.size(), buffer.data(), ovrSend_Unreliable);
}

bool Network::sendToUserTCP(const uint64_t receiverUserId, const ComponentId componentId, std::vector<uint8_t>&& data)
{
	ocean_assert(receiverUserId != 0ull);
	ocean_assert(!data.empty());

	if (data.empty())
	{
		return false;
	}

	const ScopedLock scopedLock(senderPackageLock_);

	senderPackageQueue_.emplace(receiverUserId, ++senderPackageIdCounter_, componentId, std::move(data));

	return true;
}

size_t Network::sendQueueSizeTCP(const uint64_t userId) const
{
	const ScopedLock scopedLock(senderPackageLock_);

	if (userId == 0ull)
	{
		return senderPackageQueue_.size();
	}
	else
	{
		const PendingPackageCounterMap::const_iterator iCounter = pendingPackageCounterMap_.find(userId);

		if (iCounter == pendingPackageCounterMap_.cend())
		{
			return 0;
		}

		return iCounter->second;
	}
}

size_t Network::receiveQueueSizeTCP() const
{
	const ScopedLock scopedLock(senderPackageLock_);

	return receiverPackageMap_.size();
}

Network::ConnectionScopedSubscription Network::addConnectionCallback(ConnectionCallbackFunction connectionCallbackFunction)
{
	ocean_assert(connectionCallbackFunction);

	const ScopedLock scopedLock(lock_);

	const unsigned int subscriptionId = ++subscriptionIdCounter_;

	ocean_assert(connectionCallbackFunctionMap_.find(subscriptionId) == connectionCallbackFunctionMap_.cend());
	connectionCallbackFunctionMap_.emplace(subscriptionId, std::move(connectionCallbackFunction));

	return ConnectionScopedSubscription(subscriptionId, std::bind(&Network::removeConnectionCallback, this, std::placeholders::_1));
}

Network::ReceiveScopedSubscription Network::addReceiveCallback(const ComponentId componentId, ReceiveCallbackFunction receiveCallbackFunction)
{
	ocean_assert(receiveCallbackFunction);

	const ScopedLock scopedLock(lock_);

	const unsigned int subscriptionId = ++subscriptionIdCounter_;

	receiveCallbackFunctionMap_.emplace(componentId, std::make_pair(subscriptionId, std::move(receiveCallbackFunction)));

	return ConnectionScopedSubscription(subscriptionId, std::bind(&Network::removeReceiveCallback, this, std::placeholders::_1));
}

void Network::removeConnectionCallback(const unsigned int& subscriptionId)
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(connectionCallbackFunctionMap_.find(subscriptionId) != connectionCallbackFunctionMap_.cend());
	connectionCallbackFunctionMap_.erase(subscriptionId);
}

void Network::removeReceiveCallback(const unsigned int& subscriptionId)
{
	const ScopedLock scopedLock(lock_);

	for (ReceiveCallbackFunctionMap::iterator iCallback = receiveCallbackFunctionMap_.begin(); iCallback != receiveCallbackFunctionMap_.end(); ++iCallback)
	{
		if (iCallback->second.first == subscriptionId)
		{
			receiveCallbackFunctionMap_.erase(iCallback);
			return;
		}
	}

	ocean_assert(false && "Invalid subscription id");
}

void Network::onNotificationConnectionStateChange(ovrMessage* message, const bool succeeded)
{
#ifdef OCEAN_DEBUG
	const ovrMessageType messageType = ovr_Message_GetType(message);
	ocean_assert(messageType == ovrMessage_Notification_Networking_ConnectionStateChange);
#endif

	ovrNetworkingPeerHandle peerHandle = ovr_Message_GetNetworkingPeer(message);

	const uint64_t remoteUserId = ovr_NetworkingPeer_GetID(peerHandle);
	const ovrPeerConnectionState connectionState = ovr_NetworkingPeer_GetState(peerHandle);

	std::string stateString = "Unknown";

	switch (connectionState)
	{
		case ovrPeerState_Unknown:
			break;

		case ovrPeerState_Connected:
			stateString = "Connected";
			break;

		case ovrPeerState_Timeout:
			stateString = "Timeout";
			break;

		case ovrPeerState_Closed:
			stateString = "Closed";
			break;
	}

	Log::info() << "Network connection state change: " << remoteUserId << ", " << stateString;
}

void Network::onNotificationPeerConnectRequest(ovrMessage* message, const bool succeeded)
{
#ifdef OCEAN_DEBUG
	const ovrMessageType messageType = ovr_Message_GetType(message);
	ocean_assert(messageType == ovrMessage_Notification_Networking_PeerConnectRequest);
#endif

	ovrNetworkingPeerHandle peerHandle = ovr_Message_GetNetworkingPeer(message);

	const uint64_t senderUserId = ovr_NetworkingPeer_GetID(peerHandle);

	TemporaryScopedLock scopedLock(lock_);

		bool acceptConnection = acceptConnectionUserIdSet_.find(senderUserId) != acceptConnectionUserIdSet_.cend();

		for (ConnectionCallbackFunctionMap::const_iterator iCallback = connectionCallbackFunctionMap_.cbegin(); iCallback != connectionCallbackFunctionMap_.cend(); ++iCallback)
		{
			acceptConnection = iCallback->second(senderUserId) || acceptConnection;
		}

	scopedLock.release();

	if (acceptConnection)
	{
		ovr_Net_Accept(senderUserId);

		Log::info() << "Accepted network connection from remote user: " << senderUserId;
	}
	else
	{
		Log::info() << "Rejected network connection from remote user: " << senderUserId;
	}
}

void Network::threadRun()
{
	Timestamp nextSendTimestamp(false);
	Timestamp lastSendFailureTimestamp(false);

	while (!shouldThreadStop())
	{
		bool wasBusy = false;

		const ovrPacketHandle packetHandle = ovr_Net_ReadPacket();

		if (packetHandle != nullptr)
		{
			const uint64_t senderUserId = ovr_Packet_GetSenderID(packetHandle);

			const void* data = ovr_Packet_GetBytes(packetHandle);
			ocean_assert(data != nullptr);

			const size_t size = ovr_Packet_GetSize(packetHandle);
			ocean_assert(size != 0);

			const ovrSendPolicy policy = ovr_Packet_GetSendPolicy(packetHandle);
			ocean_assert(policy == ovrSend_Unreliable || policy == ovrSend_Reliable);

			const ConnectionType connectionType = policy == ovrSend_Unreliable ? CT_UDP : CT_TCP;

			if (connectionType == CT_UDP)
			{
				receiveRateCalculatorUDP_.addOccurance(Timestamp(true), double(size));

				ComponentId componentId = CI_UNKNOWN;

				static_assert(sizeof(ComponentId) == sizeof(uint32_t), "Invalid data type!");
				if (size >= sizeof(uint32_t))
				{
					memcpy(&componentId, data, sizeof(uint32_t));

					const ScopedLock scopedLock(lock_);

					const ReceiveCallbackFunctionMap::const_iterator iConnection = receiveCallbackFunctionMap_.find(componentId);
					if (iConnection != receiveCallbackFunctionMap_.cend())
					{
						iConnection->second.second(senderUserId, componentId, (const uint8_t*)(data) + sizeof(uint32_t), size - sizeof(uint32_t), CT_UDP);
					}
				}
				else
				{
					Log::error() << "Received invalid package via UDP from user " << senderUserId;
				}
			}
			else
			{
				ocean_assert(connectionType == CT_TCP);

				receiveRateCalculatorTCP_.addOccurance(Timestamp(true), double(size));

				if (size >= sizeof(uint32_t) * 2)
				{
					uint32_t version;
					memcpy(&version, (const uint8_t*)(data) + 0, sizeof(uint32_t));

					if (version == 1u)
					{
						PurposeType purposeType;
						memcpy(&purposeType, (const uint8_t*)(data) + 4, sizeof(uint32_t));

						if (purposeType == PT_ACKNOWLEDGEMENT)
						{
							const ScopedLock scopedLock(senderPackageLock_);

							unsigned int& counter = pendingPackageCounterMap_[senderUserId];
							ocean_assert(counter > 0u);

							if (counter > 0u)
							{
								--counter;
							}
						}
						else if (purposeType == PT_PACKAGE)
						{
							if (size >= sizeof(uint32_t) * 7)
							{
								DistributionType distributionType;
								memcpy(&distributionType, (const uint8_t*)(data) + 8, sizeof(uint32_t));

								// we can only manage pending packages for packages sent to a unique user (not to all users in a room)
								if (distributionType == DT_UNIQUE_USER)
								{
									uint32_t innerData[2] =
									{
										1u, // version
										PT_ACKNOWLEDGEMENT
									};

									ovr_Net_SendPacket(senderUserId, sizeof(innerData), innerData, ovrSend_Reliable);
								}

								uint32_t packageId;
								memcpy(&packageId, (const uint8_t*)(data) + 12, sizeof(uint32_t));

								const SenderPackagePair senderPackagePair = SenderPackagePair(senderUserId, packageId);

								ReceiverPackage& receiverPackage = receiverPackageMap_[senderPackagePair];

								if (receiverPackage.addChunk((const uint8_t*)(data), size))
								{
									if (receiverPackage.isCompleted())
									{
										const ScopedLock scopedLock(lock_);

										const ReceiveCallbackFunctionMap::const_iterator iConnection = receiveCallbackFunctionMap_.find(receiverPackage.componentId());
										if (iConnection != receiveCallbackFunctionMap_.cend())
										{
											iConnection->second.second(senderUserId, receiverPackage.componentId(), receiverPackage.data(), receiverPackage.dataSize(), CT_TCP);
										}

										receiverPackageMap_.erase(senderPackagePair);
									}
								}
								else
								{
									Log::error() << "Received invalid package via TCP from user " << senderUserId;
								}
							}
						}
						else
						{
							Log::error() << "Invalid purpose type: " << int(purposeType);
						}
					}
					else
					{
						Log::error() << "Invalid package version: " << version;
					}
				}
				else
				{
					Log::error() << "Received invalid package via TCP from user " << senderUserId;
				}
			}

			ovr_Packet_Free(packetHandle);

			wasBusy = true;
		}

		{
			// let's send the queue TCP packages

			const Timestamp currentTimestamp(true);

			if (nextSendTimestamp.isInvalid() || currentTimestamp >= nextSendTimestamp)
			{
				const ScopedLock scopedLock(senderPackageLock_);

				if (!senderPackageQueue_.empty())
				{
					SenderPackage& senderPackage = senderPackageQueue_.front();

					ocean_assert(senderPackage.currentChunkData() != nullptr && senderPackage.currentChunkSize() != 0);

					if (senderPackage.receiverUserId() == 0ull || pendingPackageCounterMap_[senderPackage.receiverUserId()] < maximalPendingPackages_)
					{
						bool succeeded = false;

						if (senderPackage.receiverUserId() == 0ull)
						{
							succeeded = ovr_Net_SendPacketToCurrentRoom(senderPackage.currentChunkSize(), senderPackage.currentChunkData(), ovrSend_Reliable);
						}
						else
						{
							succeeded = ovr_Net_SendPacket(senderPackage.receiverUserId(), senderPackage.currentChunkSize(), senderPackage.currentChunkData(), ovrSend_Reliable);

							pendingPackageCounterMap_[senderPackage.receiverUserId()]++;
						}

						if (succeeded)
						{
							sendRateCalculatorTCP_.addOccurance(currentTimestamp, double(senderPackage.currentChunkSize()));

							lastSendFailureTimestamp.toInvalid();

							if (senderPackage.receiverUserId() == 0ull)
							{
								// for room network communication, we limite the bandwidth to avoid putting too much preasure onto the PlatformSDK
								nextSendTimestamp = currentTimestamp + double(senderPackage.currentChunkSize()) * secondsPerByte_;
							}

							static size_t size = 0;
							static Timestamp timestamp(true);

							size += senderPackage.currentChunkSize();

							if (Timestamp(true) > timestamp + 5.0)
							{
								Log::info() << double(size / 1024) / double(Timestamp(true) - timestamp) << "KB/s";
								timestamp.toNow();
								size = 0;
							}
						}
						else
						{
							if (lastSendFailureTimestamp.isInvalid())
							{
								lastSendFailureTimestamp = currentTimestamp;
							}

							if (currentTimestamp < lastSendFailureTimestamp + 5.0) // we retry for 5 seconds
							{
								Log::warning() << "Failed to send TCP chunk, retrying ...";

								Thread::sleep(1u);
								continue;
							}
						}

						if (!succeeded || !senderPackage.nextChunk())
						{
							if (!succeeded)
							{
								Log::error() << "Failed to send TCP chunk";
							}

							// we have reached the last chunk or we failed to send the data
							senderPackageQueue_.pop();
						}
					}

					wasBusy = true;
				}
			}
		}

		if (!wasBusy)
		{
			Thread::sleep(1u);
		}
	}
}

} // namespace PlatformSDK

} // namespace Quest

} // namespace Meta

} // namespace Platform

} // namespace Ocean
