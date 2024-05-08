/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_NETWORK_STREAMING_SERVER_H
#define FACEBOOK_NETWORK_STREAMING_SERVER_H

#include "ocean/network/Network.h"
#include "ocean/network/Port.h"
#include "ocean/network/Streaming.h"
#include "ocean/network/TCPServer.h"
#include "ocean/network/PackagedUDPClient.h"

#include "ocean/base/Callback.h"
#include "ocean/base/Lock.h"

namespace Ocean
{

namespace Network
{

/**
 * This class implements a streaming server.
 * @ingroup network
 */
class OCEAN_NETWORK_EXPORT StreamingServer : public Streaming
{
	public:

		/**
		 * Definition of a callback function on channel start, stop or pause requests.
		 */
		typedef Callback<void, const State> ChannelCallback;

		/**
		 * Definition of a channel id.
		 */
		typedef unsigned int ChannelId;

		/**
		 * Returns an invalid channel id.
		 * @return Invalid channel id
		 */
		static constexpr ChannelId invalidChannelId();

	protected:

		/**
		 * This class implements a channel.
		 */
		class Channel
		{
			public:

				/**
				 * Definition of a stream id.
				 */
				typedef unsigned int StreamId;

				/**
				 * Returns an invalid stream id.
				 * @return Invalid stream id
				 */
				static constexpr StreamId invalidStreamId();

			public:

				/**
				 * This class implements a stream.
				 */
				class Stream
				{
					public:

						/**
						 * Creates a new stream object.<br>
						 * A stream is in stopped mode by default.
						 * @param tcpConnectionId The id of the TCP configuration
						 * @param receiverAddress Address4 of the stream receiver
						 * @param receiverPort Port of the stream receiver
						 */
						Stream(const TCPServer::ConnectionId tcpConnectionId, const Address4& receiverAddress, const Port& receiverPort);

						/**
						 * (Re-)Starts the stream.
						 * @return True, if succeeded
						 */
						bool start();

						/**
						 * Pauses the stream.
						 * @return True, if succeeded
						 */
						bool pause();

						/**
						 * Returns whether this stream is currently streaming.
						 * @return True, if so
						 */
						inline bool isStreaming() const;

						/**
						 * Returns the id of the TCP configuration connection associated with this stream.
						 * @return Configuration connection
						 */
						inline TCPServer::ConnectionId tcpConnectionId() const;

						/**
						 * Returns the address of the used sender UDP client.
						 * @return Sender address
						 */
						inline Address4 senderAddress() const;

						/**
						 * Returns the port the used sender UDP client.
						 * @return Sender port
						 */
						inline Port senderPort() const;

						/**
						 * Returns the address of the receiver.
						 * @return Receiver address
						 */
						inline const Address4& receiverAddress() const;

						/**
						 * Returns the port of the receiver.
						 * @return Receiver port
						 */
						inline const Port& receiverPort() const;

						/**
						 * Streams new data using the given UDP connections.
						 * @param data The data to stream
						 * @param size The size of the data to stream in bytes
						 * @return True, if succeeded
						 */
						bool stream(const void* data, const size_t size);

					protected:

						/**
						 * Disabled copy constructor.
						 * @param stream Object which would be copied
						 */
						Stream(const Stream& stream) = delete;

						/**
						 * Disabled copy operator.
						 * @param stream Object which would be copied
						 * @return Reference to this object
						 */
						Stream& operator=(const Stream& stream) = delete;

					protected:

						/// The id of the TCP configuration connection.
						TCPServer::ConnectionId tcpConnectionId_ = TCPServer::invalidConnectionId();

						/// UDP client used for stream data transfer.
						PackagedUDPClient udpClient_;

						/// Stream receiver address.
						Address4 address_;

						/// Stream receiver port.
						Port port_;

						/// Determines whether data should be streamed.
						bool isStreaming_ = false;
				};

				/**
				 * Definition of a map mapping stream ids to streams.
				 */
				typedef std::map<StreamId, std::shared_ptr<Stream>> StreamMap;

			public:

				/**
				 * Creates a new default channel.
				 */
				Channel() = default;

				/**
				 * Creates a new channel.
				 * @param name Unique channel name
				 * @param dataType Data type of the channel
				 * @param callback Channel callback function.
				 */
				Channel(const std::string& name, const std::string& dataType, const ChannelCallback& callback);

				/**
				 * Adds a new stream to this channel.
				 * @param tcpConnectionId The id of the TCP configuration connection
				 * @param address Address4 of the stream receiver
				 * @param port Port of the stream receiver
				 * @return A valid stream id, if succeeded
				 */
				StreamId addStream(const TCPServer::ConnectionId tcpConnectionId, const Address4& address, const Port& port);

				/**
				 * Removes a stream from this channel.
				 * @param streamId Id of the stream to remove
				 * @return True, if the stream existed
				 */
				bool removeStream(const StreamId streamId);

				/**
				 * (Re-)starts a stream of this channel.
				 * @param streamId Id of the stream to start
				 * @return True, if succeeded
				 */
				bool startStream(const StreamId streamId);

				/**
				 * Pauses a stream.
				 * @param streamId Id of the stream to pause
				 * @return True, if succeeded
				 */
				bool pauseStream(const StreamId streamId);

				/**
				 * Stops and removes a stream of this channel.
				 * @param streamId Id of the stream to stop
				 * @return True, if succeeded
				 */
				bool stopStream(const StreamId streamId);

				/**
				 * Returns the name of this channel.
				 * @return Unique channel name
				 */
				inline const std::string& name() const;

				/**
				 * Returns the data type of this channel.
				 * @return Channel data type
				 */
				inline const std::string& dataType() const;

				/**
				 * Returns the UDP client sender port of a given stream.
				 * @param streamId Id of the stream to return the UDP sender port for
				 * @return UDP server sender port
				 */
				Port streamSenderPort(const StreamId streamId) const;

				/**
				 * Sets or changes the data type of this channel.
				 * @param configurationTCPServer TCP server of the streaming server used for stream configurations
				 * @param messageQueue Message queue of the streaming server used for message identification
				 * @param dataType New data type to set
				 * @return True, if succeeded
				 */
				bool setDataType(TCPServer& configurationTCPServer, MessageQueue& messageQueue, const std::string& dataType);

				/**
				 * Streams new data using the given UDP connections.
				 * @param data Data to stream
				 * @param size Size of the data to stream in bytes
				 * @return True, if succeeded
				 */
				bool stream(const void* data, const size_t size);

			protected:

				/// Unique Channel name.
				std::string name_;

				/// Data type of the channel.
				std::string dataType_;

				/// Determines the number of active streaming streams.
				unsigned int activeStreams_ = 0u;

				/// Streams used for this channel.
				StreamMap streamMap_;

				/// Stream id counter.
				StreamId streamIdCounter_ = StreamId(0);

				/// Channel request callback function.
				ChannelCallback channelCallback_;
		};

		/**
		 * Definition of a map mapping channel ids to channels.
		 */
		 typedef std::map<ChannelId, Channel> ChannelMap;

		/**
		 * This class holds some information connected with a TCP connection.
		 * @ingroup network
		 */
		class Connection
		{
			public:

				/**
				 * Creates an empty connection object.
				 */
				inline Connection() = default;

				/**
				 * Creates a new connection object.
				 * @param receiver The receiver address of the stream data
				 */
				inline Connection(const Address4& receiver);

				/**
				 * Returns the channel id.
				 * @return Channel id
				 */
				inline ChannelId channelId() const;

				/**
				 * Returns the stream id inside the channel.
				 * @return Channel stream id
				 */
				inline Channel::StreamId channelStreamId() const;

				/**
				 * Returns the stream receiver address of this connection.
				 * @return Receiver address
				 */
				inline const Address4& address() const;

				/**
				 * Returns the stream receiver port of this connection.
				 * @return Receiver port
				 */
				inline const Port& port() const;

				/**
				 * Sets the channel id of this connection.
				 * @param channelId Channel id to set
				 * @return True, if succeeded
				 */
				inline bool setChannelId(const ChannelId channelId);

				/**
				 * Sets the channel stream id of this connection.
				 * @param streamId Stream id to set
				 * @return True, if succeeded
				 */
				inline bool setChannelStreamId(const Channel::StreamId streamId);

				/**
				 * Sets the port of the stream data.
				 * @param port Port to set
				 * @return True, if succeeded
				 */
				inline bool setPort(const Port& port);

			private:

				/// Id of the channel.
				ChannelId channelId_ = invalidChannelId();

				/// Id of the stream.
				Channel::StreamId channelStreamId_ = Channel::invalidStreamId();

				/// Receiver address of the streaming data.
				Address4 address_;

				/// Receiver port of the streaming data.
				Port port_;
		};

		/**
		 * Definition of a map mapping TCP connection ids to server stream connections.
		 */
		typedef std::unordered_map<TCPServer::ConnectionId, Connection> ConnectionMap;

	public:

		/**
		 * Creates a new streaming server.
		 */
		StreamingServer();

		/**
		 * Destructs a streaming server.
		 */
		~StreamingServer() override;

		/**
		 * Sets the server address.
		 * If the systems supports more than one network address use this function to define which address to use for this server.<br>
		 * However, normally is not necessary to define the local address.
		 * @param address Address to use for this streaming server
		 * @return True, if succeeded
		 */
		bool setAddress(const Address4& address);

		/**
		 * Sets the server port.
		 * @param port Server port to set
		 * @return True, if succeeded
		 */
		bool setPort(const Port& port);

		/**
		 * Returns the server address.
		 * @return Server address
		 */
		Address4 address() const;

		/**
		 * Returns the server port.
		 * @return Server port
		 */
		Port port() const;

		/**
		 * Enables the streaming server.
		 * @return True, if succeeded
		 */
		bool enable();

		/**
		 * Disables the streaming server.
		 * @return True, if succeeded
		 */
		bool disable();

		/**
		 * Returns whether the server is enabled.
		 * @return True, if so
		 */
		inline bool isEnabled() const;

		/**
		 * Registers a new channel.
		 * @param channel Unique name of the new channel to register
		 * @param dataType Data type provides by the channel
		 * @param callback Channel request callback
		 * @return Valid channel id if succeeded
		 */
		ChannelId registerChannel(const std::string& channel, const std::string& dataType, const ChannelCallback& callback);

		/**
		 * Changes the data type of a channel.
		 * @param channelId SessionId of the channel to change the data type for
		 * @param dataType New data type to set
		 * @return True, if succeeded
		 */
		bool changeDataType(const ChannelId channelId, const std::string& dataType);

		/**
		 * Unregister a channel.
		 * @param channelId SessionId of the channel to unregister
		 * @return True, if succeeded
		 */
		bool unregisterChannel(const ChannelId channelId);

		/**
		 * Returns whether this server holds a specified channel.
		 * @return True, if the channel exists already
		 */
		bool hasChannel(const std::string& channel) const;

		/**
		 * Releases all channels.
		 */
		void release();

		/**
		 * Sets new streaming data for a specified channel.
		 * @param channelId SessionId of the streaming channel
		 * @param data Streaming data
		 * @param size Streaming data size in bytes
		 * @return True, if the data was accepted
		 */
		bool stream(const ChannelId channelId, const void* data, const size_t size);

		/**
		 * Returns the number of registered channels.
		 * @return Channels
		 */
		inline size_t channels() const;

		/**
		 * Returns a generated but unique channel name.
		 * @return Unique channel name
		 */
		std::string generateUniqueChannel() const;

	protected:

		/**
		 * New TCP connection request function.
		 * @param address Sender address
		 * @param port Sender port
		 * @param connectionId TCP server specific connection id
		 */
		bool onTCPConnection(const Address4& address, const Port& port, const TCPServer::ConnectionId connectionId);

		/**
		 * New command function.
		 * @param tcpConnectionId TCP server connection id
		 * @param command New command to handle
		 * @param value Command value
		 * @param sessionId Unique id of the server which is not unique on the client side
		 */
		void onCommand(const TCPServer::ConnectionId tcpConnectionId, const std::string& command, const std::string& value, const SessionId sessionId);

		/**
		 * Function handling connect commands.
		 * @param tcpConnectionId TCP server connection id
		 * @param value Command value
		 * @param sessionId Unique id of the server which is not unique on the client side
		 */
		void onConnect(const TCPServer::ConnectionId tcpConnectionId, const std::string& value, const SessionId sessionId);

		/**
		 * Function handling disconnect commands.
		 * @param tcpConnectionId TCP server connection id
		 * @param value Command value
		 * @param sessionId Unique id of the server which is not unique on the client side
		 */
		void onDisconnect(const TCPServer::ConnectionId tcpConnectionId, const std::string& value, const SessionId sessionId);

		/**
		 * Function handling client port commands.
		 * @param tcpConnectionId TCP server connection id
		 * @param value Command value
		 * @param sessionId Unique id of the server which is not unique on the client side
		 */
		void onClientPort(const TCPServer::ConnectionId tcpConnectionId, const std::string& value, const SessionId sessionId);

		/**
		 * Function handling server port commands.
		 * @param tcpConnectionId TCP server connection id
		 * @param value Command value
		 * @param sessionId Unique id of the server which is not unique on the client side
		 */
		void onServerPort(const TCPServer::ConnectionId tcpConnectionId, const std::string& value, const SessionId sessionId);

		/**
		 * Function handling channel select commands.
		 * @param tcpConnectionId TCP server connection id
		 * @param value Command value
		 * @param sessionId Unique id of the server which is not unique on the client side
		 */
		void onChannelSelect(const TCPServer::ConnectionId tcpConnectionId, const std::string& value, const SessionId sessionId);

		/**
		 * Function handling start commands.
		 * @param tcpConnectionId TCP server connection id
		 * @param value Command value
		 * @param sessionId Unique id of the server which is not unique on the client side
		 */
		void onStart(const TCPServer::ConnectionId tcpConnectionId, const std::string& value, const SessionId sessionId);

		/**
		 * Function handling pause commands.
		 * @param tcpConnectionId TCP server connection id
		 * @param value Command value
		 * @param sessionId Unique id of the server which is not unique on the client side
		 */
		void onPause(const TCPServer::ConnectionId tcpConnectionId, const std::string& value, const SessionId sessionId);

		/**
		 * Function handling stop commands.
		 * @param tcpConnectionId TCP server connection id
		 * @param value Command value
		 * @param sessionId Unique id of the server which is not unique on the client side
		 */
		void onStop(const TCPServer::ConnectionId tcpConnectionId, const std::string& value, const SessionId sessionId);

		/**
		 * Function handling channel commands.
		 * @param tcpConnectionId TCP server connection id
		 * @param value Command value
		 * @param sessionId Unique id of the server which is not unique on the client side
		 */
		void onChannelRequest(const TCPServer::ConnectionId tcpConnectionId, const std::string& value, const SessionId sessionId);

		/**
		 * Function handling data type commands.
		 * @param tcpConnectionId TCP server connection id
		 * @param value Command value
		 * @param sessionId Unique id of the server which is not unique on the client side
		 */
		void onDataTypeRequest(const TCPServer::ConnectionId tcpConnectionId, const std::string& value, const SessionId sessionId);

		/**
		 * Callback function for TCP receive message.
		 * @param tcpConnectionId TCP server connection id
		 * @param data Received data
		 * @param size The size of the received data
		 */
		void onTCPReceive(const TCPServer::ConnectionId tcpConnectionId, const void* data, const size_t size);

	protected:

		/// Determines whether the server is enabled.
		bool isEnabled_ = false;

		/// TCP server used for configuration tasks.
		TCPServer tcpServer_;

		/// Registered channels.
		ChannelMap channelMap_;

		/// Channel id counter.
		ChannelId channelIdCounter_ = ChannelId(0);

		/// Map mapping TCP connection ids to subscribed channels.
		ConnectionMap connectionMap_;

		/// Server lock.
		mutable Lock lock_;
};

constexpr StreamingServer::ChannelId StreamingServer::invalidChannelId()
{
	return ChannelId(-1);
}

constexpr StreamingServer::Channel::StreamId StreamingServer::Channel::invalidStreamId()
{
	return StreamId(-1);
}

inline bool StreamingServer::Channel::Stream::isStreaming() const
{
	return isStreaming_;
}

inline TCPServer::ConnectionId StreamingServer::Channel::Stream::tcpConnectionId() const
{
	return tcpConnectionId_;
}

inline Address4 StreamingServer::Channel::Stream::senderAddress() const
{
	return udpClient_.address();
}

inline Port StreamingServer::Channel::Stream::senderPort() const
{
	return udpClient_.port();
}

inline const Address4& StreamingServer::Channel::Stream::receiverAddress() const
{
	return address_;
}

inline const Port& StreamingServer::Channel::Stream::receiverPort() const
{
	return port_;
}

inline const std::string& StreamingServer::Channel::name() const
{
	return name_;
}

inline const std::string& StreamingServer::Channel::dataType() const
{
	return dataType_;
}

inline StreamingServer::Connection::Connection(const Address4& receiver) :
	address_(receiver)
{
	// nothing to do here
}

inline StreamingServer::ChannelId StreamingServer::Connection::channelId() const
{
	return channelId_;
}

inline StreamingServer::Channel::StreamId StreamingServer::Connection::channelStreamId() const
{
	return channelStreamId_;
}

inline const Address4& StreamingServer::Connection::address() const
{
	return address_;
}

inline const Port& StreamingServer::Connection::port() const
{
	return port_;
}

inline bool StreamingServer::Connection::setChannelId(const ChannelId channelId)
{
	if (channelId_ != invalidChannelId())
	{
		return false;
	}

	channelId_ = channelId;
	return true;
}

inline bool StreamingServer::Connection::setChannelStreamId(const Channel::StreamId streamId)
{
	if (channelStreamId_ != Channel::invalidStreamId())
	{
		return false;
	}

	channelStreamId_ = streamId;
	return true;
}

inline bool StreamingServer::Connection::setPort(const Port& port)
{
	if (port_.isNull() == false || port.isNull())
	{
		return false;
	}

	port_ = port;
	return true;
}

inline bool StreamingServer::isEnabled() const
{
	return isEnabled_;
}

inline size_t StreamingServer::channels() const
{
	const ScopedLock scopedLock(lock_);

	return channelMap_.size();
}

}

}

#endif // FACEBOOK_NETWORK_STREAMING_SERVER_H
