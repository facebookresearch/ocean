/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_NETWORK_STREAMING_CLIENT_H
#define FACEBOOK_NETWORK_STREAMING_CLIENT_H

#include "ocean/network/Network.h"
#include "ocean/network/Address4.h"
#include "ocean/network/Port.h"
#include "ocean/network/Streaming.h"
#include "ocean/network/TCPClient.h"
#include "ocean/network/PackagedUDPServer.h"

#include "ocean/base/Callback.h"
#include "ocean/base/Lock.h"

#include <queue>

namespace Ocean
{

namespace Network
{

/**
 * This class implements a streaming client.<br>
 * The client uses a TCP connection for configuration tasks and a UDP connection for the data transfer.
 * @ingroup network
 */
class OCEAN_NETWORK_EXPORT StreamingClient : public Streaming
{
	public:

		/**
		 * Definition of a callback function for streaming server requests.
		 */
		typedef Callback<bool, const State, const std::string&> RequestCallback;

		/**
		 * Definition of a callback function for streaming data.
		 * Parameter 0 provides the buffer that has been received
		 * Parameter 1 provides the size of the buffer, in bytes
		 */
		typedef Callback<void, const void*, const size_t> ReceiveCallback;

		/**
		 * Definition of a vector holding channels.
		 */
		typedef std::vector<std::string> Channels;

	public:

		/**
		 * Creates a new streaming client.
		 */
		StreamingClient();

		/**
		 * Destructs a streaming client.
		 */
		~StreamingClient();

		/**
		 * Connects the streaming client with a remote streaming server.
		 * @param address Address4 of the streaming server
		 * @param port Port of the streaming server
		 * @return True, if succeeded
		 */
		bool connect(const Address4& address, const Port& port);

		/**
		 * Disconnects the streaming client.
		 * @return True, if the client was connected before
		 */
		bool disconnect();

		/**
		 * Returns whether this client is currently connected with a streaming server.
		 * @return True, if so
		 */
		bool isConnected() const;

		/**
		 * Returns a list of selectable channels provides by the streaming server.
		 * @return Selectable channels
		 */
		Channels selectableChannels();

		/**
		 * Returns a data type of a specified channel provides by the streaming server.
		 * @param channel Channel to return the data type for
		 * @return Data type
		 */
		std::string channelDataType(const std::string& channel);

		/**
		 * Returns the selected channel of the streaming server.
		 * @return Selected channel
		 */
		inline const std::string& channel() const;

		/**
		 * Returns the type of the streaming data.
		 * @return Data type
		 */
		inline const std::string& dataType() const;

		/**
		 * Returns the address of the connected streaming server.
		 * @return Streaming server address
		 */
		inline const Address4& serverAddress() const;

		/**
		 * Returns the port of the connected streaming server.
		 * @return Streaming server port
		 */
		inline const Port& serverPort() const;

		/**
		 * (Re-)Starts the streaming.
		 * @param channel Streaming channel, if any
		 * @return True, if succeeded
		 */
		bool start(const std::string& channel = std::string());

		/**
		 * Pauses the streaming.
		 * @return True, if succeeded
		 */
		bool pause();

		/**
		 * Stops the streaming.
		 * @return True, if succeeded
		 */
		bool stop();

		/**
		 * Returns whether this client is currently receiving streaming data.<br>
		 * Beware: If the client is paused the client still is handled as in receiving mode.
		 * @return True, if so
		 */
		inline bool isReceiving();

		/**
		 * Returns whether this client is currently paused.
		 * @return True, if so
		 */
		inline bool isPaused();

		/**
		 * Sets the callback function for streaming server requests (like e.g. start or stop requests).
		 * @param callback Callback function to set
		 */
		inline void setRequestCallback(const RequestCallback& callback);

		/**
		 * Sets the callback function for streaming data received from the streaming server.
		 * @param callback Callback function to set
		 */
		inline void setReceiveCallback(const ReceiveCallback& callback);

	protected:

		/**
		 * New command function.
		 * @param command New command to handle
		 * @param value Command value
		 * @param sessionId Unique id of the server which is not unique on the client side
		 */
		void onCommand(const std::string& command, const std::string& value, const SessionId sessionId);

		/**
		 * Function handling start commands.
		 * @param value Command value
		 * @param sessionId Unique id of the server which is not unique on the client side
		 */
		void onStart(const std::string& value, const SessionId sessionId);

		/**
		 * Function handling pause commands.
		 * @param value Command value
		 * @param sessionId Unique id of the server which is not unique on the client side
		 */
		void onPause(const std::string& value, const SessionId sessionId);

		/**
		 * Function handling stop commands.
		 * @param value Command value
		 * @param sessionId Unique id of the server which is not unique on the client side
		 */
		void onStop(const std::string& value, const SessionId sessionId);

		/**
		 * Function handling changed data type commands.
		 * @param value Command value
		 * @param sessionId Unique id of the server which is not unique on the client side
		 */
		void onChangedDataType(const std::string& value, const SessionId sessionId);

		/**
		 * Callback function for TCP receive message.
		 * @param data Received data
		 * @param size Size of the received data, in bytes
		 */
		void onTCPReceiveData(const void* data, const size_t size);

		/**
		 * Callback function for UDP receive message.
		 * @param address Address4 of the sender
		 * @param port Port of the sender
		 * @param data Received data
		 * @param size Size of the received data
		 * @param messageId Unique message id for the received message
		 */
		void onUDPReceiveData(const Address4& address, const Port& port, const void* data, const size_t size, const PackagedSocket::MessageId messageId);

	protected:

		/// Stream channel.
		std::string channel_;

		/// Stream data type.
		std::string dataType_;

		/// Determines whether the client is currently receiving.
		bool isReceiving_ = false;

		/// Determines whether the client is pause.
		bool isPaused_ = false;

		/// UDP client.
		PackagedUDPServer udpClient_;

		/// TCP client.
		TCPClient tcpClient_;

		/// Address4 of the streaming server.
		Address4 serverAddress_;

		/// Port of the streaming server.
		Port serverPort_;

		/// Streaming server request callback function.
		RequestCallback requestCallback_;

		/// Streaming data receive callback function.
		ReceiveCallback receiveCallback_;

		/// Client lock.
		mutable Lock lock_;
};

inline const std::string& StreamingClient::channel() const
{
	return channel_;
}

inline const std::string& StreamingClient::dataType() const
{
	return dataType_;
}

inline bool StreamingClient::isReceiving()
{
	return isReceiving_;
}

inline bool StreamingClient::isPaused()
{
	return isPaused_;
}

inline const Address4& StreamingClient::serverAddress() const
{
	return serverAddress_;
}

inline const Port& StreamingClient::serverPort() const
{
	return serverPort_;
}

inline void StreamingClient::setRequestCallback(const RequestCallback& callback)
{
	requestCallback_ = callback;
}

inline void StreamingClient::setReceiveCallback(const ReceiveCallback& callback)
{
	receiveCallback_ = callback;
}

}

}

#endif // FACEBOOK_NETWORK_STREAMING_CLIENT_H
