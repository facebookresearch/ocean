// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_NETWORK_VERTS_DRIVER_H
#define FACEBOOK_NETWORK_VERTS_DRIVER_H

#include "ocean/network/verts/Verts.h"
#include "ocean/network/verts/Entity.h"

#include "ocean/base/Lock.h"
#include "ocean/base/ScopedSubscription.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/RateCalculator.h"

#include <deque>

// Forward declaration
struct verts_component;
struct verts_connection;
struct verts_connection_options;
struct verts_entity;
struct verts_driver;
struct verts_string;

// Forward declaration
enum class VertsObjectState : int;

namespace Ocean
{

namespace Network
{

namespace Verts
{

/// Forward declaration.
class Driver;

/**
 * Definition of a shared ptr for a Driver object.
 * @see Driver.
 * @ingroup networkverts
 */
typedef std::shared_ptr<Driver> SharedDriver;

/**
 * Definition of a weak ptr for a Driver object.
 * @see Driver.
 * @ingroup networkverts
 */
typedef std::weak_ptr<Driver> WeakDriver;

/**
 * This class implements a driver for all entities and communication within a zone.
 * A driver mainly wrapps a VERTS driver.
 * @ingroup networkverts
 */
class OCEAN_NETWORK_VERTS_EXPORT Driver
{
	friend class Entity;
	friend class Manager;

	public:

		/**
		 * Definition of a callback function for changed (added and removed) users.
		 * @param driver The driver sending the event
		 * @param addedUsers The ids of all added users since the last event
		 * @param removedUsers The ids of all removed users since the last event
		 */
		typedef std::function<void(Driver& driver, const UnorderedIndexSet64& addedUsers, const UnorderedIndexSet64& removedUsers)> ChangedUsersCallbackFunction;

		/**
		 * Definition of a subscription object for changed users.
		 */
		typedef ScopedSubscriptionHandler::ScopedSubscriptionType ChangedUsersScopedSubscription;

		/**
		 * Definition of a callback function for new entities.
		 * @param driver The driver sending the event
		 * @param entity The new entity
		 */
		typedef std::function<void(Driver& driver, const SharedEntity& entity)> NewEntityCallbackFunction;

		/**
		 * Definition of a subscription object for new entities.
		 */
		typedef ScopedSubscription<unsigned int, Driver> NewEntityScopedSubscription;

		/**
		 * Definition of a callback function for voip samples.
		 * The data format is signed 16 bit integer 48khz mono.
		 * @param driver The driver sending the event
		 * @param elements The elements of the sample, must be valid
		 * @param size The number of elements in the sample, with range [1, infinity)
		 */
		typedef std::function<void(Driver& driver, const int16_t* elements, const size_t size)> VoipSamplesCallbackFunction;

		/**
		 * Definition of a subscription object for voip samples.
		 */
		typedef ScopedSubscriptionHandler::ScopedSubscriptionType VoipSamplesScopedSubscription;

		/**
		 * Definition of a callback function for receive on channel events.
		 * @param driver The driver sending the event
		 * @param channelIndex The index of the channel, with range [0, 7]
		 * @param buffer The received data, will be valid
		 * @param size The size of the received data, in bytes, with range [1, infinity)
		 */
		typedef std::function<void(Driver& driver, const unsigned int channelIndex, const void* buffer, const size_t size)> ReceiveOnChannelCallbackFunction;

		/**
		 * Definition of a subscription object for receive on channel events.
		 */
		typedef ScopedSubscriptionHandler::ScopedSubscriptionType ReceiveOnChannelScopedSubscription;

		/**
		 * Definition of a buffer.
		 */
		typedef std::vector<uint8_t> Buffer;

		/**
		 * Definition of a shared pointer holding a buffer.
		 */
		typedef std::shared_ptr<Buffer> SharedBuffer;

		/**
		 * Definition of a callback function for receive container events.
		 * @param driver The driver sending the event
		 * @param sessionId The id of the session sending the container
		 * @param userId The id of the user sending the container
		 * @param identifier The identifier of the container
		 * @param version The version of the container
		 * @param buffer The container's data
		 */
		typedef std::function<void(Driver& driver, const uint32_t sessionId, const uint64_t userId, const std::string& identifier, const unsigned int version, const SharedBuffer& buffer)> ContainerCallbackFunction;

		/**
		 * Definition of a subscription object for receive container events.
		 */
		typedef ScopedSubscriptionHandler::ScopedSubscriptionType ReceiveContainerScopedSubscription;

		/**
		 * Returns the maximal number of bytes which can be sent on a channel.
		 * @return The maximal size of a buffer which can be sent, in bytes, is 1024
		 */
		static constexpr size_t maximalChannelBufferSize();

	protected:

		/**
		 * Definition of a map mapping entity ids to entity objects.
		 */
		typedef std::unordered_map<Entity::EntityId, SharedEntity> EntityMap;

		/**
		 * Definition of a map mapping node ids to node objects.
		 */
		typedef std::unordered_map<Node::NodeId, SharedNode> NodeMap;

		/**
		 * Definition of an unordered map mapping subscription ids to new entity callback functions.
		 */
		typedef std::unordered_map<unsigned int, std::pair<std::string, NewEntityCallbackFunction>> NewEntityCallbackFunctionMap;

		/**
		 * Definition of a vector holding voip samples.
		 */
		typedef std::vector<int16_t> VoipSamples;

		/**
		 * This class holds mono voip data.
		 */
		class MonoVoipData
		{
			public:

				/**
				 * Creates a new object with reserved memory.
				 */
				inline MonoVoipData();

			public:

				/// The ambisonic voip samples.
				VoipSamples voipSamples_;

				/// The number of samples actually stored in the buffer, with range [0, voipSamples_.size()]
				size_t size_ = 0;

				/// The 3D position of the voip source.
				VectorF3 position_;
		};

		/**
		 * Definition of a vector holding voip data.
		 */
		typedef std::vector<MonoVoipData> MonoVoipDatas;

		/**
		 * Definition of a queue holding voip data.
		 */
		typedef std::queue<MonoVoipData> MonoVoipDataQueue;

		/**
		 * Definition of an unordered map mapping user ids to session ids.
		 */
		typedef std::unordered_map<uint64_t, UnorderedIndexSet32> UserMap;

		/**
		 * Definition of a set holding nodes.
		 */
		typedef std::unordered_set<SharedNode> SharedNodeSet;

		/**
		 * Definition of a subscription handler for changed users events.
		 */
		typedef ScopedSubscriptionCallbackHandler<ChangedUsersCallbackFunction, Driver, true> ChangedUsersCallbackHandler;

		/**
		 * Definition of a subscription handler for voip samples events.
		 */
		typedef ScopedSubscriptionCallbackHandler<VoipSamplesCallbackFunction, Driver, true> VoipSamplesCallbackHandler;

		/**
		 * Definition of a subscription handler for receive on channel events.
		 */
		typedef ScopedSubscriptionCallbackHandler<ReceiveOnChannelCallbackFunction, Driver, true> ReceiveOnChannelCallbackHandler;

		/**
		 * Definition of a pair combining an initialization statement with a ReceiveOnChannelCallbackHandler object.
		 */
		typedef std::pair<bool, std::unique_ptr<ReceiveOnChannelCallbackHandler>> ReceiveOnChannelCallbackHandlerPair;

		/**
		 * Definition of a vector holding receive on channel event handlers.
		 */
		typedef std::vector<ReceiveOnChannelCallbackHandlerPair> ReceiveOnChannelCallbackHandlers;

		/**
		 * Definition of a subscription handler for receive container events.
		 */
		typedef ScopedSubscriptionCallbackHandler<ContainerCallbackFunction, Driver, true> ReceiveContainerCallbackHandler;

		/**
		 * This class holds the relevant information of a buffer to be sent on a channel.
		 */
		class SendOnChannelObject
		{
			public:

				/**
				 * Creates a new object.
				 * @param channelIndex The index of the channel to be used, with range [0, 7]
				 * @param buffer The  buffer to be sent, must be valid
				 * @param lastObjectForContainerId Optional id of the container to which this buffer belongs and while beeing the last buffer of the container, -1 otherwise
				 */
				inline SendOnChannelObject(const unsigned int channelIndex, SharedBuffer buffer, const unsigned int lastObjectForContainerId = (unsigned int)(-1));

			public:

				/// The index of the channel to be used, with range [0, 7]
				unsigned int channelIndex_;

				/// The buffer to be sent.
				SharedBuffer buffer_;

				/// Optional id of the container to which this buffer belongs and while beeing the last buffer of the container, -1 otherwise.
				unsigned int lastObjectForContainerId_ = (unsigned int)(-1);
		};

		/**
		 * Definition of a double-ended queue holding buffers to be send on a channel.
		 */
		typedef std::deque<SendOnChannelObject> SendOnChannelBufferQueue;

		/**
		 * Definition of the data header of a container sequence.
		 */
		class ContainerSequenceDataHeader
		{
			public:

				/**
				 * Default constructor.
				 */
				ContainerSequenceDataHeader() = default;

				/**
				 * Creates a new header.
				 * @param sessionId The id of the session sending the container, must be valid
				 * @param containerId The id of the container, must be unique at the sending session
				 * @param sequenceIndex The index of the sequence, with range [0, numberSequences_ - 1]
				 * @param numberSequences The number of sequences the container is composed of, with range [0, infinity)
				 */
				inline ContainerSequenceDataHeader(const uint32_t sessionId, const uint32_t containerId, const uint32_t sequenceIndex, const uint32_t numberSequences);

				/**
				 * Returns the unique id containing session id and container id.
				 * @return The unique id
				 */
				inline uint64_t uniqueId() const;

				/**
				 * Returns whether this header contains valid data.
				 * @return True, if so
				 */
				inline bool isValid() const;

				/**
				 * Converts a unique id into session id and container id.
				 * @param uniqueId The unique id of container to convert
				 * @param sessionId The resulting session id
				 * @param containerId The resulting container id
				 */
				static inline void decomposeUniqueId(const uint64_t uniqueId, uint32_t& sessionId, uint32_t& containerId);

			public:

				/// The id of the session sending the container.
				uint32_t sessionId_ = 0u;

				/// The id of the container, unique at the sending session.
				uint32_t containerId_ = uint32_t(-1);

				/// The index of the sequence, with range [0, numberSequences_ - 1].
				uint32_t sequenceIndex_ = uint32_t(-1);

				/// The number of sequences the container is composed of, with range [0, infinity).
				uint32_t numberSequences_ = 0u;
		};

		/**
		 * Definition of the response header of a container response.
		 */
		class ContainerSequenceResponseHeader
		{
			public:

				/**
				 * Default constructor.
				 */
				ContainerSequenceResponseHeader() = default;

				/**
				 * Creates a new header.
				 * @param sessionId The id of the session sending the container, must be valid
				 * @param containerId The id of the container, must be unique at the sending session
				 */
				inline ContainerSequenceResponseHeader(const uint32_t sessionId, const uint32_t containerId);

			public:

				/// The id of the session sending the container.
				uint32_t sessionId_ = 0u;

				/// The id of the container, unique at the sending session.
				uint32_t containerId_ = uint32_t(-1);
		};

		/**
		 * This class is able to handle receiving a container.
		 */
		class ContainerReceiver
		{
			public:

				/**
				 * Creates a new receiver object.
				 * @param containerSequenceDataHeader The header of the first received sequence
				 * @param timestamp The timestamp when the identifier was set, must be valid
				 */
				ContainerReceiver(const ContainerSequenceDataHeader& containerSequenceDataHeader, const Timestamp& timestamp);

				/**
				 * Sets the identifier and version of this container.
				 * @param identifier The identifier to be set, must be valid
				 * @param version The version to be set, with range [0, infinity)
				 */
				inline void setIdentifier(std::string&& identifier, const unsigned int version);

				/**
				 * Sets a sequence.
				 * @param sequenceIndex The index of the sequence, with range [0, containerSequenceDataHeader.numberSequences_ - 1]
				 * @param data The data of the sequence, must be valid
				 * @param size The size of the sequence, with range [0, maximalPayloadSize() - 1]
				 * @param timestamp The timstamp when the sequence was received, must be valid
				 */
				bool setSequence(const uint32_t sequenceIndex, const void* data, const size_t size, const Timestamp& timestamp);

				/**
				 * Returns the sender's session id.
				 * @return Session id of the sender
				 */
				inline uint32_t sessionId() const;

				/**
				 * Returns whether it's time to request a re-sent of lost sequences.
				 * @param timestamp The current timestamp, must be valid
				 * @param lostSequenceIds The resulting ids of all lost sequences
				 * @param interval The interval between two re-sents, in seconds, with range [0, infinity)
				 * @param arrivalTimeout The timeout since the last sequence arrival to send all pending ids, in seconds, with range [0, infinity)
				 * @param maximalIds The maximal number of returning ids, with range [1, infinity)
				 * @return True, if so
				 */
				bool needResentLostSequences(const Timestamp& timestamp, Indices32& lostSequenceIds, const double interval = 0.5, const double arrivalTimeout = 5.0, const size_t maximalIds = 128);

				/**
				 * Returns the header for a sequence response.
				 * @return Header for a sequence response
				 */
				inline ContainerSequenceResponseHeader responseHeader() const;

				/**
				 * Returns whether this reciver is finished and holds a valid buffer.
				 * @return True, if so
				 */
				inline bool isFinished() const;

				/**
				 * Returns the finished buffer that has been received.
				 * The receiver must be finished, only call this function once.
				 * @param identifier The resulting identifier of the container
				 * @param version The resulting version of the container
				 * @param timestamp The current timestamp
				 * @return The finished buffer
				 */
				SharedBuffer finishedBuffer(std::string& identifier, unsigned int& version, const Timestamp& timestamp);

				/**
				 * Returns the maixmal size of a sequence.
				 * @return Maximal size, in bytes
				 */
				static constexpr size_t maximalSequenceSize();

				/**
				 * Returns the maixmal size of the payload.
				 * @return Maximal size, in bytes
				 */
				static constexpr size_t maximalPayloadSize();

			protected:

				/// The header of the first received sequence.
				ContainerSequenceDataHeader containerSequenceDataHeader_;

				/// The container's identifier.
				std::string identifier_;

				/// The container's version.
				int64_t version_ = -1;

				/// The indices of all pending sequences.
				IndexSet32 pendingSequenceIndices_;

				/// The index of the sequence with highest index which has been set.
				uint32_t highestFinishedSequenceIndex_ = uint32_t(-1);

				// Timestmap when the initial sequence has arrived/set.
				Timestamp initialArrivalTimestamp_ = Timestamp(false);

				/// Timestmap when the lastest sequence has arrived/set (or the sequence was created).
				Timestamp lastestArrivalTimestamp_ = Timestamp(false);

				/// The timestamp of the latest extraction of lost sequence indices.
				Timestamp lastLostSequenceIdsTimestamp_ = Timestamp(false);

				/// The receiver's buffer.
				Buffer buffer_;

				/// The actual size of the buffer once known, 0 until known.
				size_t actualSize_ = 0;

				/// The number of lost sequences.
				size_t lostSequences_ = 0;

				/// The number of sequences which have been received more than once.
				size_t redundantSequences_ = 0;
		};

		/**
		 * Definition of an unordered map mapping unique session/container ids to container receiver objects.
		 */
		typedef std::unordered_map<uint64_t, std::unique_ptr<ContainerReceiver>> ContainerReceiverMap;

		/**
		 * This class is able to handle receiving a container.
		 */
		class ContainerSender
		{
			protected:

				/**
				 * Definition of an unordered map mapping sequence indices to buffers.
				 */
				typedef std::unordered_map<uint32_t, SharedBuffer> BufferMap;

			public:

				/**
				 * Creates a new sender object.
				 * @param numberSequences The number of sequences the sender will hold, with range [1, infinity)
				 */
				explicit inline ContainerSender(const size_t numberSequences);

				/**
				 * Adds a new buffer to this sender.
				 * @param sequenceIndex The index of the sequence for which the buffer will be set, with range [0, numberSequences - 1]
				 * @param buffer The buffer to set, must be valid
				 */
				inline void addBuffer(const uint32_t sequenceIndex, SharedBuffer buffer);

				/**
				 * Adds indices of lost sequences to this sender.
				 * @param sequenceIndices The indices of the sequences, must be valid
				 * @param size The number of indices, with range [1, infinity)
				 * @param timestamp The current timestamp, must be valid
				 */
				void addLostSequenceIndices(const Index32* sequenceIndices, const size_t size, const Timestamp& timestamp);

				/**
				 * Checks whether the indices of lost sequences need to be re-sent.
				 * @param timestamp The current timestamp, must be valid
				 * @param lostSequenceIndices The resulting indices to resent
				 * @param interval The interval between two re-sents, in seconds, with range [0, infinity)
				 * @return True, if so
				 */
				bool needsResentLostSequences(const Timestamp& timestamp, UnorderedIndexSet64& lostSequenceIndices, const double interval);

				/**
				 * Returns a specific buffer of the sender.
				 * @param sequenceIndex The index of the sequence for which the buffer will be returned, with range [0, numberSequences - 1]
				 * @return The sequence's buffer, nullptr if the buffer does not exist
				 */
				inline SharedBuffer buffer(const uint32_t sequenceIndex) const;

				/**
				 * Informs this sender that the last sequence has been sent.
				 * @param timestamp The current timestamp, must be valid
				 */
				inline void informLastSequenceSent(const Timestamp& timestamp);

				/**
				 * Returns whether the sender is finished and can be removed.
				 * @param timestamp The current timestamp, must be valid
				 * @return True, if so
				 */
				bool isFinished(const Timestamp& timestamp);

			protected:

				/// The timestamp when this sender object was used the last time.
				Timestamp updateUsageTimestamp_;

				/// The timestamp when the lost sequences have been re-sent the last time.
				Timestamp lostSequencesResentTimestamp_;

				/// The map mapping sequence indices to buffers.
				BufferMap bufferMap_;

				/// The set holding the indices of all lost sequences.
				UnorderedIndexSet64 lostSequenceIndexSet_;

				/// The number of lost sequences.
				size_t lostSequences_ = 0;
		};

		/**
		 * Definition of an unordered map mapping container ids to container sender objects.
		 */
		typedef std::unordered_map<Index32, std::unique_ptr<ContainerSender>> ContainerSenderMap;

		/// Definition of the internal channel index which is used for container data.
		static constexpr unsigned int internalContainerDataChannelIndex_ = 6u;

		/// Definition of the internal channel index which is used for container responses.
		static constexpr unsigned int internalContainerResponseChannelIndex_ = 7u;

	public:

		/**
		 * Destructs a driver.
		 */
		~Driver();

		/**
		 * Returns the zone this driver is associated with.
		 * @return The driver's zone
		 */
		inline std::string zoneName() const;

		/**
		 * Returns the host which is used.
		 * @return The driver's host, empty if the default host is used
		 */
		inline std::string host() const;

		/**
		 * Returns the timeout of this driver.
		 * @return The driver's timeout in milliseconds, with range [1000, infinity)
		 */
		inline unsigned int timeout() const;

		/**
		 * Returns the id of the local user.
		 * @return The local user's id, 0 if the driver is not yet initialized
		 */
		inline uint64_t userId() const;

		/**
		 * Returns the id of the local session.
		 * @return The local user's session, 0 if the driver is not yet initialized
		 */
		inline uint32_t sessionId() const;

		/**
		 * Returns the ids of all remote users currently in the zone of this driver.
		 * @return The user ids
		 */
		UnorderedIndexSet64 remoteUserIds();

		/**
		 * Returns all entities of this driver.
		 * @return The driver's entities
		 */
		SharedEntities entities() const;

		/**
		 * Creates a new entity with only one node.
		 * @param nodeSpecification The specification of the one node which will be part of the resulting entity
		 * @return The new entity, nullptr if entity could not be created
		 */
		SharedEntity newEntity(const NodeSpecification& nodeSpecification);

		/**
		 * Creates a new entity with several nodes.
		 * @param nodeTypes The types of the nodes which will be part of the resulting entity
		 * @return The new entity, nullptr if entity could not be created
		 */
		SharedEntity newEntity(const std::vector<std::string>& nodeTypes);

		/**
		 * Sends voip samples with 16bit signed integer precision as mono channel and 48kHz.
		 * @param samples The samples to send, must be valid
		 * @param size The number of samples, with range [1, infinity)
		 * @param position The position of the source in 3D space
		 * @return True, if succeeded
		 */
		bool sendVoipSamples(const int16_t* samples, const size_t size, const VectorF3& position = VectorF3(0.0f, 0.0f, 0.0f));

		/**
		 * Sends data on a channel.
		 * There guarantee that the data will reach each client.
		 * @param channelIndex The index of the channel to be used, with range [0, 5]
		 * @param buffer The buffer to be sent, with size [1, maximalChannelBufferSize()] bytes
		 */
		void sendOnChannel(const unsigned int channelIndex, Buffer&& buffer);

		/**
		 * Sends a container with an arbitrary size.
		 * @param identifier The identifier of the container, must be valid
		 * @param version The version of the container, with range [0, infinity)
		 * @param data The data of the container to send, must be valid
		 * @param size The size of the container data, in bytes, with range [1, infinity)
		 */
		void sendContainer(const std::string& identifier, const unsigned int version, const void* data, const size_t size);

		/**
		 * Adds a new callback function for changed users events.
		 * @param changedUsersCallbackFunction The callback function to add, must be valid
		 * @return The subscription object, the callback function will exist as long as the subscription object exists
		 */
		[[nodiscard]] inline ChangedUsersScopedSubscription addChangedUsersCallback(ChangedUsersCallbackFunction changedUsersCallbackFunction);

		/**
		 * Adds a new callback function for new entity events.
		 * @param newEntityCallbackFunction The callback function to add, must be valid
		 * @param componentType Optional type of a component which the new entity must contain
		 * @return The subscription object, the callback function will exist as long as the subscription object exists
		 */
		[[nodiscard]] NewEntityScopedSubscription addNewEntityCallback(NewEntityCallbackFunction newEntityCallbackFunction, std::string componentType = std::string());

		/**
		 * Adds a new callback function for voip samples events.
		 * @param voipCallbackFunction The callback function to add, must be valid
		 * @return The subscription object, the callback function will exist as long as the subscription object exists
		 */
		[[nodiscard]] inline VoipSamplesScopedSubscription addVoipSamplesCallback(VoipSamplesCallbackFunction voipCallbackFunction);

		/**
		 * Adds a new callback function for receive on channels events.
		 * @param channelIndex The index of the channel from which the data will be received, with range [0, 5]
		 * @param receiveOnChannelCallbackFunction The callback function to add, must be valid
		 * @return The subscription object, the callback function will exist as long as the subscription object exists
		 */
		[[nodiscard]] ReceiveOnChannelScopedSubscription addReceiveOnChannelCallback(const unsigned int channelIndex, ReceiveOnChannelCallbackFunction receiveOnChannelCallbackFunction);

		/**
		 * Adds a new callback function for receive container events.
		 * @param containerCallbackFunction The callback function for containers, must be valid
		 * @return The subscription object, the callback function will exist as long as the subscription object exists
		 */
		[[nodiscard]] inline ReceiveContainerScopedSubscription addReceiveContainerCallback(ContainerCallbackFunction containerCallbackFunction);

		/**
		 * Returns whether this driver is initialized and ready to use.
		 * A driver will be initialized once a valid network connection could be established, this can take several seconds.
		 * @return True, if so
		 */
		inline bool isInitialized() const;

		/**
		 * Returns the number of voip samples in a mono chunk for 20ms.
		 * @return The number of samples
		 */
		static constexpr size_t voipMonoChunkSize();

		/**
		 * Returns the number of voip samples in a ambisonic chunk for 20ms.
		 * @return The number of samples
		 */
		static constexpr size_t voipAmbisonicChunkSize();

	protected:

		/**
		 * Creates a new driver for a specified zone.
		 * @param zoneName The name of the zone for which the driver will be created, must be valid
		 * @param host Explicit host to be used, empty to use the default host, e.g., 'graph.facebook.com' for Facebook universe, or 'graph.<OD-NUMBER>.od.facebook.com' for On-Demand server
		 * @param timeout The timeout in milliseconds, with range [1000, infinity)
		 */
		explicit Driver(std::string zoneName, std::string host = std::string(), const unsigned int timeout = 30000u);

		/**
		 * Returns whether this driver is valid.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Updates the driver and all related resources.
		 * @param timestamp The current timestamp, must be valid
		 */
		void update(const Timestamp& timestamp);

		/**
		 * Waits until the driver has been created and initialized.
		 * @return True, if the driver is ready to use
		 */
		bool waitForCreatedDriver();

		/**
		 * Handles changed users.
		 */
		void handleChangedUsers();

		/**
		 * Handles all queued voip samples for sending.
		 */
		void handleQueuedVoipSamples();

		/**
		 * Handles the latest received voip samples.
		 */
		void handleReceivedVoipSamples();

		/**
		 * Handles the queued buffers to be sent on a channel.
		 */
		void handleSendOnChannelQueue();

		/**
		 * Handles the pending container buffers to be sent.
		 */
		void handlePendingContainer();

		/**
		 * Removes a new entity callback function.
		 * @param subscriptionId The id of the callback function to remove, must be valid
		 */
		void removeNewEntityCallback(const unsigned int& subscriptionId);

		/**
		 * Releases the driver and all resources.
		 */
		void release();

		/**
		 * Event function for new internal nodes.
		 * @param node The new internal node
		 */
		void onNewInternalNode(const SharedNode& node);

		/**
		 * Event function for deleted internal nodes.
		 * @param node The deleted internal node
		 */
		void onDeletedInternalNode(const SharedNode& node);

		/**
		 * Event function for a new VERTS entity.
		 * @param vertsEntity The new entity, will be valid
		 */
		void onNewVertsEntity(verts_entity* vertsEntity);

		/**
		 * Event function for a deleted VERTS entity.
		 * @param vertsEntity The deleted entity, will be valid
		 */
		void onDeletedVertsEntity(verts_entity* vertsEntity);

		/**
		 * Event function for a new VERTS component.
		 * @param vertsComponent The new component, will be valid
		 */
		void onNewVertsComponent(verts_component* vertsComponent);

		/**
		 * Event function for a deleted VERTS component.
		 * @param vertsComponent The deleted component, will be valid
		 */
		void onDeletedVertsComponent(verts_component* vertsComponent);

		/**
		 * Event function for changed states of VERTS components.
		 * @param vertsComponent The impacted VERTS component, will be valid
		 * @param vertsObjectState The new state
		 */
		void onChangedComponentState(const verts_component* vertsComponent, VertsObjectState vertsObjectState);

		/**
		 * Event function for an updated VERTS component.
		 * @param vertsComponent The updated component, will be valid
		 * @param serverTime The server timestamp
		 * @param previousServerTime The previous server timestamp
		 */
		void onVertsComponentUpdated(verts_component* vertsComponent, uint64_t serverTime, uint64_t previousServerTime);

		/**
		 * Event function for an changed VERTS field.
		 * @param vertsComponent The component in which the field is located, will be valid
		 * @param name The name of the changed filed
		 * @param timestamp The timestamp
		 */
		void onVertsComponentFieldChanged(verts_component* vertsComponent, const verts_string name, uint64_t timestamp);

		/**
		 * Event function for data received on a channel.
		 * @param channelId The VERTS id of the channel, with range [1, 8]
		 * @param sequenceNumber
		 * @param buffer The received buffer, will be valid
		 * @param bufferSize The size of the buffer, in bytes, with range [1, infinity)
		 */
		void onReceiveOnChannel(uint8_t channelId, uint64_t sequenceNumber, const uint8_t* buffer, uint32_t bufferSize);

		/**
		 * Event function for internal container data receive events.
		 * @param driver The driver sending the event, will be this driver
		 * @param channelIndex The index of the channel receiving the data, will be 'internalContainerDataChannelIndex_'
		 * @param buffer The received data, will be valid
		 * @param size The size of the data, in bytes, will be in range [1, maximalChannelBufferSize()]
		 */
		void onReceiveOnContainerDataChannel(Driver& driver, const unsigned int channelIndex, const void* buffer, const size_t size);

		/**
		 * Event function for internal container response receive events.
		 * @param driver The driver sending the event, will be this driver
		 * @param channelIndex The index of the channel receiving the data, will be 'internalContainerResponseChannelIndex_'
		 * @param buffer The received data, will be valid
		 * @param size The size of the data, in bytes, will be in range [1, maximalChannelBufferSize()]
		 */
		void onReceiveOnContainerResponseChannel(Driver& driver, const unsigned int channelIndex, const void* buffer, const size_t size);

		/**
		 * Event function for a new VERTS entity.
		 * @param vertsEntity The new entity, will be valid
		 * @param context The event's context, will be valid
		 */
		static void onNewVertsEntity(verts_entity* vertsEntity, void* context);

		/**
		 * Event function for a deleted VERTS entity.
		 * @param vertsEntity The deleted entity, will be valid
		 * @param context The event's context, will be valid
		 */
		static void onDeletedVertsEntity(verts_entity* vertsEntity, void* context);

		/**
		 * Event function for a new VERTS component.
		 * @param vertsComponent The new component, will be valid
		 * @param context The event's context, will be valid
		 */
		static void onNewVertsComponent(verts_component* vertsComponent, void* context);

		/**
		 * Event function for a deleted VERTS component.
		 * @param vertsComponent The deleted component, will be valid
		 * @param context The event's context, will be valid
		 */
		static void onDeletedVertsComponent(verts_component* vertsComponent, void* context);

		/**
		 * Event function for changed states of VERTS components.
		 * @param vertsComponent The impacted VERTS component, will be valid
		 * @param vertsObjectState The new state
		 * @param context The event's context, will be valid
		 */
		static void onChangedComponentState(const verts_component* vertsComponent, VertsObjectState vertsObjectState, void* context);

		/**
		 * Event function for an updated VERTS component.
		 * @param vertsComponent The updated component, will be valid
		 * @param serverTime The server timestamp
		 * @param previousServerTime The previous server timestamp
		 * @param context The event's context, will be valid
		 */
		static void onVertsComponentUpdated(verts_component* vertsComponent, uint64_t serverTime, uint64_t previousServerTime, void* context);

		/**
		 * Event function for an changed VERTS field.
		 * @param vertsComponent The component in which the field is located, will be valid
		 * @param name The name of the changed filed
		 * @param timestamp The timestamp
		 * @param context The event's context, will be valid
		 */
		static void onVertsComponentFieldChanged(verts_component* vertsComponent, const verts_string name, uint64_t timestamp, void* context);

		/**
		 * Event function for data received on a channel.
		 * @param channelId The VERTS id of the channel, with range [1, 8]
		 * @param sequenceNumber
		 * @param buffer The received buffer, will be valid
		 * @param bufferSize The size of the buffer, in bytes, with range [1, infinity)
		 * @param context The event's context, will be valid
		 */
		static void onReceiveOnChannel(uint8_t channelId, uint64_t sequenceNumber, const uint8_t* buffer, uint32_t bufferSize, void* context);

	protected:

		/// The name of the zone to which the driver is associated.
		std::string zoneName_;

		/// The explicit host to be used, empty to use the default host.
		std::string host_;

		/// The driver's timeout in milliseconds.
		unsigned int timeout_ = 0u;

		/// The id of the local user, known once the driver is initialized.
		uint64_t userId_ = 0ull;

		/// The id of the local session, known once the driver is initialized.
		uint32_t sessionId_ = 0ul;

		/// True, if the driver is currently creating an entity locally.
		bool creatingEntityLocally_ = false;

		/// The map mapping entity ids to entity objects.
		EntityMap entityMap_;

		/// The map mapping node ids to node objects.
		NodeMap nodeMap_;

		/// The VERTS connection option which is used during establishing a connection.
		verts_connection_options* vertsOptions_ = nullptr;

		/// The VERTS connection.
		verts_connection* vertsConnection_ = nullptr;

		/// The VERTS driver which is valid once a connection has been established.
		verts_driver* vertsDriver_ = nullptr;

		/// The current timestamp.
		Timestamp currentTimestamp_;

		/// The set holding all remote users.
		UserMap userMap_;

		/// The set holding all remote users.
		UnorderedIndexSet64 userIdMap_;

		/// The VERTS subscription id for the new entity callback function.
		int onNewEntityCallbackFunctionId_ = -1;

		/// The VERTS subscription id for the deleted entity callback function.
		int onDeletedEntityCallbackFunctionId_ = -1;

		/// The VERTS subscription id for the new component callback function.
		int onNewVertsComponentCallbackFunctionId_ = -1;

		/// The VERTS subscription id for the deleted component callback function.
		int onDeletedVertsComponentCallbackFunctionId_ = -1;

		/// The VERTS subscription id for the component state changed callback function.
		int onComponentStateChangedCallbackFunctionId_ = -1;

		/// The VERTS subscription id for the updated component callback function.
		int onVertsComponentUpdatedCallbackFunctionId_ = -1;

		/// The VERTS subscription id for the changed field callback function.
		int onVertsComponentFieldChangedCallbackFunctionId_ = -1;

		/// The counter for unique subscription ids.
		unsigned int subscriptionIdCounter_ = 0u;

		/// The handler for changed user event subscriptions.
		ChangedUsersCallbackHandler changedUsersCallbackHandler_;

		/// The map mapping subscription ids to callback functions for new entities.
		NewEntityCallbackFunctionMap newEntityCallbackFunctionMap_;

		/// The handler for voip samples event subscriptions.
		VoipSamplesCallbackHandler voipSamplesCallbackHandler_;

		/// The handlers for on channel receive event subscriptions.
		ReceiveOnChannelCallbackHandlers receiveOnChannelCallbackHandlers_ = ReceiveOnChannelCallbackHandlers(8);

		/// The handler for receive container event subscription.
		ReceiveContainerCallbackHandler receiveContainerCallbackHandler_;

		/// The queue with mono voip samples to be sent.
		MonoVoipDataQueue monoVoipDataSendQueue_;

		/// Reusable mono voip samples.
		MonoVoipDatas reusableMonoVoipDatas_;

		/// Reusable buffer with ambisonic voip samples.
		VoipSamples reusableReceivedAmbisonicVoipSamples_;

		/// The internal session nodes.
		SharedNodeSet internalSessionNodes_;

		/// The internal container nodes.
		SharedNodeSet internalContainerNodes_;

		/// The ids of all users which have been added since the last update.
		UnorderedIndexSet64 addedUserIds_;

		/// The ids of all users which have been removed since the last update.
		UnorderedIndexSet64 removedUserIds_;

		/// The queue holding buffers to be send on a channel.
		SendOnChannelBufferQueue sendOnChannelBufferQueue_;

		/// The subscription object for internal container data channel events.
		ReceiveOnChannelScopedSubscription receiveOnContainerDataChannelScopedSubscription_;

		/// The subscription object for internal container response channel events.
		ReceiveOnChannelScopedSubscription receiveOnContainerResponseChannelScopedSubscription_;

		/// The map mapping unique ids to container receivers.
		ContainerReceiverMap pendingContainerReceiverMap_;

		/// The map mapping container ids container senders.
		ContainerSenderMap activeContainerSenderMap_;

		/// The set holding ids of all finished containers.
		UnorderedIndexSet64 finishedContainerIdSet_;

		// The throughput calculator for channels.
		RateCalculator channelsThroughputCalculator_;

		/// The maximal throughput for channels, in bytes.
		double maximalThroughputChannels_ = 2.5 * 1024.0 * 1024.0;

		/// The counter for unique container ids.
		Index32 containerSenderIdCounter_ = 0u;

		/// The driver's lock.
		mutable Lock lock_;

		/// The lock for voip samples.
		Lock voipSamplesLock_;

		/// The lock for the queue with send on channel buffers.
		Lock sendOnChannelQueueLock_;
};

constexpr size_t Driver::maximalChannelBufferSize()\
{
	// VERTS recommends to keep the buffer <= 1K
	// actual maximal packet size is VERTS_MAX_PACKET == 1200

	return 1024;
}

inline Driver::MonoVoipData::MonoVoipData() :
	voipSamples_(voipMonoChunkSize())
{
	// nothing to do here
}

inline Driver::SendOnChannelObject::SendOnChannelObject(const unsigned int channelIndex, SharedBuffer buffer, const unsigned int lastObjectForContainerId) :
	channelIndex_(channelIndex),
	buffer_(std::move(buffer)),
	lastObjectForContainerId_(lastObjectForContainerId)
{
	// nothing to do here
}

inline Driver::ContainerSequenceDataHeader::ContainerSequenceDataHeader(const uint32_t sessionId, const uint32_t containerId, const uint32_t sequenceIndex, const uint32_t numberSequences) :
	sessionId_(sessionId),
	containerId_(containerId),
	sequenceIndex_(sequenceIndex),
	numberSequences_(numberSequences)
{
	// nothing to do here
}

inline uint64_t Driver::ContainerSequenceDataHeader::uniqueId() const
{
	return uint64_t(sessionId_) | (uint64_t(containerId_) << 32ull);
}

inline bool Driver::ContainerSequenceDataHeader::isValid() const
{
	return sessionId_ != 0u && containerId_ != uint32_t(-1) && sequenceIndex_ < numberSequences_;
}

inline void Driver::ContainerSequenceDataHeader::decomposeUniqueId(const uint64_t uniqueId, uint32_t& sessionId, uint32_t& containerId)
{
	sessionId = uint32_t(uniqueId & 0xFFFFFFFFull);
	containerId = uint32_t(uniqueId >> 32ull);
}

inline Driver::ContainerSequenceResponseHeader::ContainerSequenceResponseHeader(const uint32_t sessionId, const uint32_t containerId) :
	sessionId_(sessionId),
	containerId_(containerId)
{
	// nothing to do here
}

inline void Driver::ContainerReceiver::setIdentifier(std::string&& identifier, const unsigned int version)
{
	identifier_ = std::move(identifier);
	version_ = int64_t(version);
}

inline uint32_t Driver::ContainerReceiver::sessionId() const
{
	ocean_assert(containerSequenceDataHeader_.isValid());
	return containerSequenceDataHeader_.sessionId_;
}

inline bool Driver::ContainerReceiver::isFinished() const
{
	return pendingSequenceIndices_.empty() && version_ >= 0;
}

inline Driver::ContainerSequenceResponseHeader Driver::ContainerReceiver::responseHeader() const
{
	return ContainerSequenceResponseHeader(containerSequenceDataHeader_.sessionId_, containerSequenceDataHeader_.containerId_);
}

constexpr size_t Driver::ContainerReceiver::maximalSequenceSize()
{
	return 1024 - sizeof(ContainerSequenceDataHeader);
}

constexpr size_t Driver::ContainerReceiver::maximalPayloadSize()
{
	return maximalSequenceSize() - sizeof(ContainerSequenceDataHeader);
}

inline Driver::ContainerSender::ContainerSender(const size_t numberSequences) :
	updateUsageTimestamp_(false)
{
	ocean_assert(numberSequences > 0);
	bufferMap_.reserve(numberSequences);
}

inline void Driver::ContainerSender::addBuffer(const uint32_t sequenceIndex, SharedBuffer buffer)
{
	ocean_assert(buffer);

	ocean_assert(bufferMap_.find(sequenceIndex) == bufferMap_.cend());
	bufferMap_.emplace(sequenceIndex, std::move(buffer));
}

inline Driver::SharedBuffer Driver::ContainerSender::buffer(const uint32_t sequenceIndex) const
{
	const BufferMap::const_iterator iBuffer = bufferMap_.find(sequenceIndex);

	if (iBuffer == bufferMap_.cend())
	{
		ocean_assert(false && "Invalid sequence index");
		return nullptr;
	}

	return iBuffer->second;
}

inline void Driver::ContainerSender::informLastSequenceSent(const Timestamp& timestamp)
{
	ocean_assert(updateUsageTimestamp_.isInvalid());
	ocean_assert(timestamp.isValid());

	updateUsageTimestamp_ = timestamp;
}

inline std::string Driver::zoneName() const
{
	const ScopedLock scopedLock(lock_);

	return zoneName_;
}

inline std::string Driver::host() const
{
	const ScopedLock scopedLock(lock_);

	return host_;
}

inline unsigned int Driver::timeout() const
{
	const ScopedLock scopedLock(lock_);

	return timeout_;
}

inline uint64_t Driver::userId() const
{
	const ScopedLock scopedLock(lock_);

	return userId_;
}

inline uint32_t Driver::sessionId() const
{
	const ScopedLock scopedLock(lock_);

	return sessionId_;
}

inline Driver::ChangedUsersScopedSubscription Driver::addChangedUsersCallback(ChangedUsersCallbackFunction changedUsersCallbackFunction)
{
	return changedUsersCallbackHandler_.addCallback(std::move(changedUsersCallbackFunction));
}

inline Driver::VoipSamplesScopedSubscription Driver::addVoipSamplesCallback(VoipSamplesCallbackFunction voipCallbackFunction)
{
	return voipSamplesCallbackHandler_.addCallback(std::move(voipCallbackFunction));
}

inline Driver::ReceiveContainerScopedSubscription Driver::addReceiveContainerCallback(ContainerCallbackFunction containerCallbackFunction)
{
	return receiveContainerCallbackHandler_.addCallback(std::move(containerCallbackFunction));
}

inline bool Driver::isInitialized() const
{
	const ScopedLock scopedLock(lock_);

	return vertsDriver_ != nullptr;
}

inline bool Driver::isValid() const
{
	const ScopedLock scopedLock(lock_);

	return vertsOptions_ != nullptr;
}

constexpr size_t Driver::voipMonoChunkSize()
{
	constexpr size_t sampleRate = 48000; // 48Khz
	constexpr size_t chunkSize = sampleRate / 50; // 20ms

	return chunkSize;
}

constexpr size_t Driver::voipAmbisonicChunkSize()
{
	return voipMonoChunkSize() * 4;
}

}

}

}

#endif // FACEBOOK_NETWORK_VERTS_DRIVER_H
