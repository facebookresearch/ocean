/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_NETWORK_STREAMING_H
#define FACEBOOK_NETWORK_STREAMING_H

#include "ocean/network/Network.h"
#include "ocean/network/MessageQueue.h"

namespace Ocean
{

namespace Network
{

/**
 * This class is the base class for all streaming objects.
 * @ingroup network
 */
class OCEAN_NETWORK_EXPORT Streaming
{
	public:

		/**
		 * Definition of different streaming states.
		 */
		enum State
		{
			/// Start state.
			STATE_START,
			/// Pause state.
			STATE_PAUSE,
			/// Stop state.
			STATE_STOP,
			/// Type change state.
			STATE_TYPE_CHANGED
		};

		/**
		 * Definition of a session id.
		 */
		typedef MessageQueue::Id SessionId;

	protected:

		/// Definition of a connect command.
		static const std::string& connectCommand();
		/// Definition of a positive connect response.
		static const std::string& connectResponseP();
		/// Definition of a negative connect response.
		static const std::string& connectResponseN();

		/// Definition of a disconnect command.
		static const std::string& disconnectCommand();
		/// Definition of a positive disconnect response.
		static const std::string& disconnectResponseP();
		/// Definition of a negative disconnect response.
		static const std::string& disconnectResponseN();

		/// Definition of a channel select command.
		static const std::string& channelSelectCommand();
		/// Definition of a positive channel select response.
		static const std::string& channelSelectResponseP();
		/// Definition of a negative channel select response.
		static const std::string& channelSelectResponseN();

		/// Definition of a start command.
		static const std::string& startCommand();
		/// Definition of a positiv start response.
		static const std::string& startResponseP();
		/// Definition fo a negative start response.
		static const std::string& startResponseN();

		/// Definition of a pause command.
		static const std::string& pauseCommand();
		/// Definition of a positive pause response.
		static const std::string& pauseResponseP();
		/// Definition of a negative pause response.
		static const std::string& pauseResponseN();

		/// Definition of a stop command.
		static const std::string& stopCommand();
		/// Definition of a positive stop response.
		static const std::string& stopResponseP();
		/// Definition of a negative stop response.
		static const std::string& stopResponseN();

		/// Definition of a client port command.
		static const std::string& clientPortCommand();
		/// Definition of a positive client port response.
		static const std::string& clientPortResponseP();
		/// Definition of a negative client port response.
		static const std::string& clientPortResponseN();

		/// Definition of a server port command.
		static const std::string& serverPortCommand();
		/// Definition of a positive server port response.
		static const std::string& serverPortResponseP();
		/// Definition of a negative server port response.
		static const std::string& serverPortResponseN();

		/// Definition of a channel request command.
		static const std::string& channelRequestCommand();
		/// Definition of a positive channel request response.
		static const std::string& channelRequestResponseP();
		/// Definition of a negative channel request response.
		static const std::string& channelRequestResponseN();

		/// Definition of a changed data type command.
		static const std::string& changedDataTypeCommand();
		/// Definition of a positive changed data type request.
		static const std::string& changedDataTypeResponseP();
		/// Definition of a negative change data type request.
		static const std::string& changedDataTypeResponseN();

		/// Definition of a data type request command.
		static const std::string& dataTypeRequestCommand();
		/// Definition of a positive data type request response.
		static const std::string& dataTypeRequestResponseP();
		/// Definition of a negative data type request response.
		static const std::string& dataTypeRequestResponseN();

	public:

		/**
		 * Returns the name of this streaming object.
		 * @return Streaming object name
		 */
		inline const std::string& name() const;

		/**
		 * Returns the description of this streaming object.
		 * @return Streaming object description
		 */
		inline const std::string& description() const;

		/**
		 * Returns the timeout value the streaming object waits for response messages.
		 * @return timeout The timeout value in seconds
		 */
		inline double responseTimeout() const;

		/**
		 * Sets the name of this streaming object.
		 * @param name Streaming object name to set
		 * @return True, if succeeded
		 */
		virtual bool setName(const std::string& name);

		/**
		 * Sets the description of this streaming object.
		 * @param description Streaming object description to set
		 * @return True, if succeeded
		 */
		virtual bool setDescription(const std::string& description);

		/**
		 * Sets the timeout value the streaming object waits for response messages.
		 * @param timeout The timeout value in seconds
		 * @return True, if succeeded
		 */
		bool setResponseTimeout(const double timeout);

	protected:

		/**
		 * Creates a new streaming object.
		 */
		Streaming();

		/**
		 * Disabled copy constructor.
		 * @param streaming Object which would be copied
		 */
		Streaming(const Streaming& streaming) = delete;

		/**
		 * Destructs an object.
		 */
		virtual ~Streaming() = default;

		/**
		 * Parses a command or a response.
		 * @param data The data to parse, must be valid
		 * @param size The size of the data to parse in bytes
		 * @param isResponse True, if the parse string was a command, a response otherwise
		 * @param message Command or response message
		 * @param value Optional command or response value
		 * @param sessionId Unique session id
		 * @return True, if succeeded
		 */
		bool parse(const unsigned char* data, const size_t size, bool& isResponse, std::string& message, std::string& value, SessionId& sessionId);

		/**
		 * Disabled copy operator.
		 * @param streaming Object which would be copied
		 */
		Streaming& operator=(const Streaming& streaming) = delete;

		/**
		 * Creates a new command message.
		 * @param command The command
		 * @param sessionId Unique session id connected to this command
		 * @return Command message
		 */
		static std::string createCommand(const std::string& command, const SessionId sessionId);

		/**
		 * Creates a new command message with a value.
		 * @param command The command
		 * @param value Command value
		 * @param sessionId Unique session id connected to this command
		 * @return Command message
		 */
		static std::string createCommand(const std::string& command, const std::string& value, const SessionId sessionId);

		/**
		 * Creates a new response message.
		 * @param response The response
		 * @param sessionId Unique session id connected to this response
		 * @return Response message
		 */
		static std::string createResponse(const std::string& response, const SessionId sessionId);

		/**
		 * Creates a new response message with a value.
		 * @param response The response
		 * @param value Response value
		 * @param sessionId Unique session id connected to this response
		 * @return Response message
		 */
		static std::string createResponse(const std::string& response, const std::string& value, const SessionId sessionId);

	protected:

		/// Name of this streaming object.
		std::string name_;

		/// Description of this streaming object.
		std::string description_;

		/// Timeout value for response messages.
		double responseTimeout_ = 2.0;

		/// Message queue.
		MessageQueue messageQueue_ = MessageQueue(100);
};

inline const std::string& Streaming::name() const
{
	return name_;
}

inline const std::string& Streaming::description() const
{
	return description_;
}

inline double Streaming::responseTimeout() const
{
	return responseTimeout_;
}

}

}

#endif // FACEBOOK_NETWORK_STREAMING_H
