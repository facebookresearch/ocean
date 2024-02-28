// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_NETWORK_H
#define META_OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_NETWORK_H

#include "ocean/platform/meta/quest/platformsdk/PlatformSDK.h"
#include "ocean/platform/meta/quest/platformsdk/MessageHandler.h"

#include "ocean/base/ScopedSubscription.h"
#include "ocean/base/Singleton.h"
#include "ocean/base/Thread.h"

#include "ocean/math/RateCalculator.h"

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
 * This class implements network functionalities.
 * @ingroup platformmetaquestplatformsdk
 */
class OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_EXPORT Network :
	public Singleton<Network>,
	protected MessageHandler,
	protected Thread
{
	friend class Singleton<Network>;

	public:

		/**
		 * Definition of connection types.
		 */
		enum ConnectionType : uint32_t
		{
			/// Invalid connection type.
			CT_INVALID = 0u,
			/// The connection is using UDP, package size must be below 1196 bytes.
			CT_UDP,
			/// The connection is using TCP.
			CT_TCP
		};

		/**
		 * Definition of individual component ids.
		 * Component ids are used to distinguish the customer of a message received via network.
		 */
		enum ComponentId : uint32_t
		{
			/// The component is unknown.
			CI_UNKNOWN = 0u,
			/// The message belongs to the Avatars component.
			CI_AVATARS = string2tag("AVTS"),
			/// The message belongs to the DistributedStates component.
			CI_DISTRIBUTED_STATES = string2tag("DRSA"),
			/// The message is a custom message not associated with a component inside PlatformSDK.
			CI_CUSTOM = string2tag("CSTM")
		};

		/**
		 * Definition of a callback function for connection requests.
		 */
		using ConnectionCallbackFunction = std::function<bool(const uint64_t senderUserId)>;

		/**
		 * Definition of a callback function for received data.
		 */
		using ReceiveCallbackFunction = std::function<void(const uint64_t senderUserId, const ComponentId componentId, const void* data, const size_t size, const ConnectionType connectionType)>;

		/**
		 * Definition of a subscription object for connection requests.
		 */
		typedef ScopedSubscription<unsigned int, Network> ConnectionScopedSubscription;

		/**
		 * Definition of a subscription object for a received data.
		 */
		typedef ScopedSubscription<unsigned int, Network> ReceiveScopedSubscription;

	protected:

		/**
		 * Definition of an unordered map mapping subscription ids to connection callback functions.
		 */
		typedef std::unordered_map<unsigned int, ConnectionCallbackFunction> ConnectionCallbackFunctionMap;

		/**
		 * Definition of individual data purpose types.
		 */
		enum PurposeType : uint32_t
		{
			/// Unknown purpose.
			PT_UNKNOWN  = 0u,
			/// The data contains a package.
			PT_PACKAGE,
			/// The data contains an acknowledgement.
			PT_ACKNOWLEDGEMENT
		};

		/**
		 * Definition of individual distribution types.
		 */
		enum DistributionType : uint32_t
		{
			/// Unknown distribution type.
			DT_UNKNOWN = 0u,
			/// The data has been sent to a unique user only.
			DT_UNIQUE_USER,
			/// The data has been sent to all users in a room.
			DT_ROOM
		};

		/**
		 * Definition of an unordered map mapping component ids to paris of subscription ids and receive data callback functions.
		 */
		typedef std::unordered_map<ComponentId, std::pair<unsigned int, ReceiveCallbackFunction>> ReceiveCallbackFunctionMap;

		/// Definition of the maximal package size for TCP connections.
		static constexpr size_t maximalPackageSize_ = 65000;

		/// Definition of the maximal payload size for TCP connections.
		static constexpr size_t maximalPayloadSize_ = maximalPackageSize_ - sizeof(uint32_t) * 4;

		/// Definition of the number of bytes which can be sent per second.
		static constexpr size_t bytesPerSecond_ = 1024 * 1024;

		/// Definition of the number of seconds needed per byte.
		static constexpr double secondsPerByte_ = 1.0 / double(bytesPerSecond_);

		/// The maximal number of pending packages to avoid flooding the network
		static constexpr unsigned int maximalPendingPackages_ = 12000000 / maximalPackageSize_;

		/**
		 * This class allows to separate a lage buffer (a package) into smaller chunks so that the individual chunks can be transmitted via TCP and re-assembled on the receiver side.
		 */
		class SenderPackage
		{
			public:

				/**
				 * Creates a new package object.
				 * @param receiverUserId The id of the user receiving the data, 0 to send the data to the current room
				 * @param packageId The unique id of the package, must be valid
				 * @param componentId The id of the component sending the data, must be valid
				 * @param data The data package to send with arbitrary size
				 */
				SenderPackage(const uint64_t receiverUserId, const uint32_t packageId, const ComponentId componentId, std::vector<uint8_t>&& data);

				/**
				 * Returns the id of the user receiving the data.
				 * @return The user id of the receiver, 0 to send the data to the current room
				 */
				uint64_t receiverUserId() const;

				/**
				 * Returns the pointer to the current chunk data.
				 * @return The current cunk data
				 */
				const void* currentChunkData() const;

				/**
				 * Returns the size of the current chunk data.
				 * @return The current chunk's size, in bytes
				 */
				size_t currentChunkSize() const;

				/**
				 * Checks whether this package contains another chunk which needs to be sent.
				 * @return True, if so; False, if all chunks have been sent
				 */
				bool nextChunk();

			protected:

				/// The user id of the receiver, 0 to send the data to the current room.
				uint64_t receiverUserId_ = uint64_t(0);

				/// The unique id of this package.
				uint32_t packageId_ = 0u;

				/// The id of the component sending the data.
				ComponentId componentId_ = CI_UNKNOWN;

				/// The number of chunks in this package.
				size_t numberChunks_ = 0;

				/// The index of the current chunk.
				size_t currentChunkIndex_ = size_t(-1);

				/// The size of the current chunk, including configuration data and payload, in bytes.
				size_t currentChunkSize_ = 0;

				/// The size of the payload in the current chunk, in bytes.
				size_t payloadInCurrentChunk_ = 0;

				/// The current index in the package data.
				size_t indexInPackageData_ = 0;

				/// The data of the package.
				std::vector<uint8_t> packageData_;

				/// The data of the current chunk.
				std::vector<uint8_t> chunkData_;
		};

		/**
		 * This class allows to re-assembled individual message chunks which have been received via TCP into a larger data package.
		 */
		class ReceiverPackage
		{
			public:

				/**
				 * Default constructor.
				 */
				ReceiverPackage() = default;

				/**
				 * Adds a new chunk to this package.
				 * @param data The data of the chunk, must be valid
				 * @param size The size of the chunk, in bytes, with range [16, infinity)
				 * @return True, if succeeded; False, if the chunk data was damaged
				 */
				bool addChunk(const uint8_t* data, const size_t size);

				/**
				 * Returns the id of the component to which this package belongs.
				 * @return The package's component id
				 */
				ComponentId componentId() const;

				/**
				 * Returns the pointer to the memory of the completed package.
				 * @return The package's data
				 * @see isCompleted().
				 */
				const void* data() const;

				/**
				 * Returns the size of the completed package.
				 * @return The package's size in bytes
				 * @see isCompleted().
				 */
				size_t dataSize() const;

				/**
				 * Returns whether the package has been completed and whether the data can be accessed.
				 * @return True, if so
				 */
				bool isCompleted() const;

			protected:

				/// The data of the package.
				std::vector<uint8_t> packageData_;

				/// The the id of the component to which this package belongs.
				ComponentId componentId_ = CI_UNKNOWN;

				/// Returns the size of the completed package.
				size_t dataSize_ = 0;

				/// The number of chunks of this package.
				size_t numberChunks_ = 0;

				/// The number of chunks which have been read already.
				size_t readChunks_ = 0;
		};

		/**
		 * Definition of a queue holding sender packages.
		 */
		typedef std::queue<SenderPackage> SenderPackageQueue;

		/**
		 * Definition of a pair combining a user id with a package id.
		 */
		typedef std::pair<uint64_t, uint32_t> SenderPackagePair;

		/**
		 * Definition of a map mapping pairs of user ids and package ids to receiver packages.
		 */
		typedef std::map<SenderPackagePair, ReceiverPackage> ReceiverPackageMap;

		/**
		 * Definition of an unordered map mapping user ids to pendering package counters.
		 */
		typedef std::unordered_map<uint64_t, unsigned int> PendingPackageCounterMap;

	public:

		/**
		 * Activates the automatic connection acceptance for all connections comming from users in the current room.
		 * @return True, if succeeded
		 */
		bool acceptAllConnectionsForCurrentRoom();

		/**
		 * Sets the id of a user for which a connection request will be accepted automatically.
		 * @param userId The id of the user for which the connection will be accepted automatically, must be valid
		 */
		void acceptConnectionForUser(const uint64_t userId);

		/**
		 * Connects to a remote user.
		 * @param receiverUserId The id of the remote user to which a connection will be established, must be valid
		 * @return True, if succeeded
		 */
		bool connect(const uint64_t receiverUserId);

		/**
		 * Returns whether a connection exists to a remote user.
		 * @param receiverUserId The id of the remote user to check, must be valid
		 * @return True, if so
		 */
		bool isConnected(const uint64_t receiverUserId) const;

		/**
		 * Sends data to all users in the current room via UDP.
		 * @param componentId The id of the component from which the data was sent
		 * @param data The data to send, must be valid
		 * @param size The size of the data to send, in bytes, with range [1, 1195]
		 * @return True, if succeeded
		 */
		bool sendToRoomUDP(const ComponentId componentId, const void* data, const size_t size);

		/**
		 * Sends data to all users in the current room via TCP.
		 * @param componentId The id of the component from which the data was sent
		 * @param data The data to send, can have arbitary size
		 * @return True, if succeeded
		 */
		bool sendToRoomTCP(const ComponentId componentId, std::vector<uint8_t>&& data);

		/**
		 * Sends data to a user via UDP.
		 * @param receiverUserId The id of the remote user to which the data will be sent, must be valid
		 * @param componentId The id of the component from which the data was sent
		 * @param data The data to send, must be valid
		 * @param size The size of the data to send, in bytes, with range [1, 1195]
		 * @return True, if succeeded
		 */
		bool sendToUserUDP(const uint64_t receiverUserId, const ComponentId componentId, const void* data, const size_t size);

		/**
		 * Sends data to a user via TCP.
		 * @param receiverUserId The id of the remote user to which the data will be sent, must be valid
		 * @param componentId The id of the component from which the data was sent
		 * @param data The data to send, can have arbitary size
		 * @return True, if succeeded
		 */
		bool sendToUserTCP(const uint64_t receiverUserId, const ComponentId componentId, std::vector<uint8_t>&& data);

		/**
		 * Returns the size of the send queue for TCP connections.
		 * @param userId Optional the user id for which the size of the queue is returned; 0 to return the size of the queue for any user
		 * @return The size of the queue
		 */
		size_t sendQueueSizeTCP(const uint64_t userId = 0ull) const;

		/**
		 * Returns the size of the receive queue for TCP connections.
		 * @return The size of the queue
		 */
		size_t receiveQueueSizeTCP() const;

		/**
		 * Returns the current send throughput.
		 * @param tcp True, to determine the throughput for TCP connections; False, for UDP connections
		 * @return The current send throughput in bytes.
		 */
		inline size_t currentSendThroughput(const bool tcp) const;

		/**
		 * Returns the current receive throughput.
		 * @param tcp True, to determine the throughput for TCP connections; False, for UDP connections
		 * @return The current receive throughput in bytes.
		 */
		inline size_t currentReceiveThroughput(const bool tcp) const;

		/**
		 * Adds a new callback function for connection requests.
		 * Connection callback functions can be used to get a more precise/powerful connection handling compared to acceptAllConnectionsForCurrentRoom() or acceptConnectionForUser().
		 * @param connectionCallbackFunction The callback function to set, must be valid
		 * @return The subscription object, the callback function will exist as long as the subscription object exists
		 * @see acceptAllConnectionsForCurrentRoom(), acceptConnectionForUser().
		 */
		ConnectionScopedSubscription addConnectionCallback(ConnectionCallbackFunction connectionCallbackFunction);

		/**
		 * Adds a new callback function for data receive events.
		 * @param componentId The id of the component to which the callback belongs
		 * @param receiveCallbackFunction The callback function to set, must be valid
		 * @return The subscription object, the callback function will exist as long as the subscription object exists
		 */
		ReceiveScopedSubscription addReceiveCallback(const ComponentId componentId, ReceiveCallbackFunction receiveCallbackFunction);

	protected:

		/**
		 * Creates a new network object.
		 */
		Network();

		/**
		 * Destructs the network object.
		 */
		~Network() override;

		/**
		 * Removes a conneciton callback function.
		 * @param subscriptionId The id of the callback function to remove, must be valid
		 */
		void removeConnectionCallback(const unsigned int& subscriptionId);

		/**
		 * Removes a receive callback function.
		 * @param subscriptionId The id of the callback function to remove, must be valid
		 */
		void removeReceiveCallback(const unsigned int& subscriptionId);

		/**
		 * Event notification function for changed connection states.
		 * @param message The response message, must be valid
		 * @param succeeded True, if the request succeeded
		 */
		void onNotificationConnectionStateChange(ovrMessage* message, const bool succeeded);

		/**
		 * Event notification function for connection requests.
		 * @param message The response message, must be valid
		 * @param succeeded True, if the request succeeded
		 */
		void onNotificationPeerConnectRequest(ovrMessage* message, const bool succeeded);

		/**
		 * The internal thread function.
		 */
		void threadRun() override;

	protected:

		/// The subscription objects for all messages.
		MessageScopedSubscriptions messageScopedSubscriptions_;

		/// The ids of all remote users from which connection requests will be accepted automatically.
		IndexSet64 acceptConnectionUserIdSet_;

		/// The map mapping subscription ids to connection callback functions.
		ConnectionCallbackFunctionMap connectionCallbackFunctionMap_;

		/// The map mapping component ids to receive callback functions.
		ReceiveCallbackFunctionMap receiveCallbackFunctionMap_;

		/// The counter for unique subscription ids.
		unsigned int subscriptionIdCounter_ = 0u;

		/// The counter for unique sender package ids.
		uint32_t senderPackageIdCounter_ = 0u;

		/// The queue with sender packages.
		SenderPackageQueue senderPackageQueue_;

		/// The lock for sender packages.
		mutable Lock senderPackageLock_;

		/// The map mapping pairs of user ids and package ids to receiver packages.
		ReceiverPackageMap receiverPackageMap_;

		/// The map mapping user ids to counters of pending packages.
		PendingPackageCounterMap pendingPackageCounterMap_;

		/// The rate calculator for the send throughput of TCP connections.
		RateCalculator sendRateCalculatorTCP_;

		/// The rate calculator for the send throughput of UDP connections.
		RateCalculator sendRateCalculatorUDP_;

		/// The rate calculator for the receive throughput of TCP connections.
		RateCalculator receiveRateCalculatorTCP_;

		/// The rate calculator for the receive throughput of TCP connections.
		RateCalculator receiveRateCalculatorUDP_;

		/// The lock.
		Lock lock_;
};

inline size_t Network::currentSendThroughput(const bool tcp) const
{
	if (tcp)
	{
		return size_t(sendRateCalculatorTCP_.rate(Timestamp(true)) + 0.5);
	}
	else
	{
		return size_t(sendRateCalculatorUDP_.rate(Timestamp(true)) + 0.5);
	}
}

inline size_t Network::currentReceiveThroughput(const bool tcp) const
{
	if (tcp)
	{
		return size_t(receiveRateCalculatorTCP_.rate(Timestamp(true)) + 0.5);
	}
	else
	{
		return size_t(receiveRateCalculatorUDP_.rate(Timestamp(true)) + 0.5);
	}
}

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_NETWORK_H
