/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/network/StreamingClient.h"

#include "ocean/base/String.h"
#include "ocean/base/Thread.h"
#include "ocean/base/Timestamp.h"

namespace Ocean
{

namespace Network
{

StreamingClient::StreamingClient()
{
	tcpClient_.setReceiveCallback(TCPClient::ReceiveCallback(*this, &StreamingClient::onTCPReceiveData));
	udpClient_.setReceiveCallback(PackagedUDPServer::ReceiveCallback(*this, &StreamingClient::onUDPReceiveData));
}

StreamingClient::~StreamingClient()
{
	disconnect();
}

bool StreamingClient::connect(const Address4& address, const Port& port)
{
	const ScopedLock scopedLock(lock_);

	if (isReceiving_)
	{
		return false;
	}

	if (!tcpClient_ || !udpClient_)
	{
		return false;
	}

	if (udpClient_.port().isNull())
	{
		return false;
	}

	if (tcpClient_.isConnected())
	{
		if (address == serverAddress_ && port == serverPort_)
		{
			return true;
		}

		Log::error() << "The TCP client is connected already.";
		return false;
	}

	bool noError = true;

	if (tcpClient_.connect(address, port) == false)
	{
		Log::error() << name_ << " could not establish a TCP connection with the streaming server \"" << address.readable() << "\" with port \"" << port.readable() << "\".";
		noError = false;
	}
	else
	{
		Log::info() << name_ << " established a TCP connection with the streaming server \"" << address.readable() << "\" with port \"" << port.readable() << "\".";
	}

	const SessionId sessionId = messageQueue_.uniqueId();

	if (noError)
	{
		if (tcpClient_.send(createCommand((connectCommand()), sessionId)) != TCPClient::SR_SUCCEEDED)
		{
			Log::error() << name_ << " could not send a connect request.";
			noError = false;
		}
		else
		{
			Log::info() << name_ << " sent a connection request.";
		}
	}

	if (noError)
	{
		if (messageQueue_.pop(sessionId, responseTimeout_) != connectResponseP())
		{
			Log::error() << name_ << " received no connection accept response by the streaming server.";
			noError = false;
		}
		else
		{
			Log::info() << name_ << " holds a valid connection with the streaming server.";
		}
	}

	if (noError)
	{
		if (tcpClient_.send(createCommand(clientPortCommand(), String::toAString(udpClient_.port().readable()), sessionId)) != TCPClient::SR_SUCCEEDED)
		{
			Log::error() << name_ << " could not send a streaming port.";
			noError = false;
		}
		else
		{
			Log::info() << name_ << " sends a streaming port \"" << udpClient_.port().readable() << "\".";
		}
	}

	if (noError && messageQueue_.pop(sessionId, responseTimeout_) != clientPortResponseP())
	{
		Log::error() << name_ << " received no port accept response by the streaming server.";
		noError = false;
	}

	if (noError == false)
	{
		tcpClient_.disconnect();
	}
	else
	{
		serverAddress_ = address;
		serverPort_ = port;
	}

	return noError;
}

bool StreamingClient::disconnect()
{
	const ScopedLock scopedLock(lock_);

	stop();

	if (!tcpClient_)
	{
		return false;
	}

	if (tcpClient_.isConnected() == false)
	{
		return false;
	}

	bool noError = true;

	const SessionId sessionId = messageQueue_.uniqueId();

	if (noError)
	{
		if (tcpClient_.send(createCommand(disconnectCommand(), sessionId)) != TCPClient::SR_SUCCEEDED)
		{
			Log::error() << name_ << " could not send a disconnect request.";
			noError = false;
		}
		else
		{
			Log::info() << name_ << " sent a disconnect request.";
		}
	}

	if (noError)
	{
		if (messageQueue_.pop(sessionId, responseTimeout_) != disconnectResponseP())
		{
			Log::error() << name_ << " received no disconnection accept response, however the client will stop receiving.";
			noError = false;
		}
		else
		{
			Log::info() << name_ << " disconnected the connection with the streaming server gracefully.";
		}
	}

	if (tcpClient_.disconnect() == false)
	{
		noError = false;
	}

	return noError;
}

bool StreamingClient::isConnected() const
{
	const ScopedLock scopedLock(lock_);

	return tcpClient_.isConnected();
}

StreamingClient::Channels StreamingClient::selectableChannels()
{
	const ScopedLock scopedLock(lock_);

	Channels channels;

	if (tcpClient_.isConnected() == false)
	{
		return channels;
	}

	const SessionId sessionId = messageQueue_.uniqueId();

	if (tcpClient_.send(createCommand(channelRequestCommand(), sessionId)) != TCPClient::SR_SUCCEEDED)
	{
		Log::error() << name_ << " could not send a channel request command.";
		return channels;
	}
	else
	{
		Log::info() << name_ << " sent a channel request command.";
	}

	std::string response, value;

	if (messageQueue_.pop(sessionId, responseTimeout_, response, value) == false || response != channelRequestResponseP())
	{
		Log::error() << name_ << " received no channel request answer.";
		return channels;
	}
	else
	{
		Log::info() << name_ << " received following channels \"" << value << "\".";
	}

	while (true)
	{
		std::string::size_type pos = value.find(";");
		if (pos == std::string::npos)
		{
			if (value.empty() == false)
			{
				channels.push_back(value);
			}

			break;
		}

		channels.push_back(value.substr(0, pos));
		value = value.substr(pos + 1);
	}

	return channels;
}

std::string StreamingClient::channelDataType(const std::string& channel)
{
	const ScopedLock scopedLock(lock_);

	if (tcpClient_.isConnected() == false)
	{
		Log::error() << name_ << " failed to receive a channel data type, because the client is not connected.";
		return std::string();
	}

	const SessionId sessionId = messageQueue_.uniqueId();

	if (tcpClient_.send(createCommand(dataTypeRequestCommand(), channel, sessionId)) != TCPClient::SR_SUCCEEDED)
	{
		Log::error() << name_ << " could not send a data type request message.";
		return std::string();
	}
	else
	{
		Log::info() << " sent a data type request message.";
	}

	std::string response, value;

	if (messageQueue_.pop(sessionId, responseTimeout_, response, value) == false || response != dataTypeRequestResponseP())
	{
		Log::error() << name_ << " received no answer for the data type request.";
	}

	Log::info() << name_ << " received the following data type \"" << value << "\".";

	return value;
}

bool StreamingClient::start(const std::string& channel)
{
	const ScopedLock scopedLock(lock_);

	if (tcpClient_.isConnected() == false)
	{
		return false;
	}

	const SessionId sessionId = messageQueue_.uniqueId();
	std::string response, value;

	if (tcpClient_.send(createCommand(channelSelectCommand(), channel, sessionId)) != TCPClient::SR_SUCCEEDED)
	{
		Log::error() << name_ << " could not send a channel select request.";
		return false;
	}
	else
	{
		Log::info() << name_ << " sent a channel select request.";
	}

	if (messageQueue_.pop(sessionId, responseTimeout_, response, value) == false || response != channelSelectResponseP())
	{
		Log::error() << name_ << " received no positive channel select response.";
		return false;
	}
	else
	{
		Log::info() << name_ << " received a channel select response.";
	}

	if (tcpClient_.send(createCommand(dataTypeRequestCommand(), sessionId)) != TCPClient::SR_SUCCEEDED)
	{
		Log::error() << name_ << " could not send a data type request.";
		return false;
	}
	else
	{
		Log::info() << name_ << " sent a data type request.";
	}

	if (messageQueue_.pop(sessionId, responseTimeout_, response, value) == false || response != dataTypeRequestResponseP())
	{
		Log::error() << name_ << " received no positive data type request response.";
		return false;
	}
	else
	{
		Log::info() << name_ << " received a data type response.";
	}
	dataType_ = value;


	if (tcpClient_.send(createCommand(serverPortCommand(), sessionId)) != TCPClient::SR_SUCCEEDED)
	{
		Log::error() << name_ << " could not send a server port request.";
		return false;
	}
	else
	{
		Log::info() << name_ << " sent a server port request.";
	}

	if (messageQueue_.pop(sessionId, responseTimeout_, response, value) == false || response != serverPortResponseP())
	{
		Log::error() << name_ << " received no positive server port response.";
		return false;
	}
	else
	{
		Log::info() << name_ << " received a server port response.";
	}

	/// the highest port can be 65535
	if (value.length() > 5)
	{
		Log::error() << name_ << " received an invalid server port.";
		return false;
	}

	const int readablePort = atoi(value.c_str());
	if (readablePort < 0 || readablePort >= 65535)
	{
		Log::error() << name_ << " received an invalid server port.";
		return false;
	}

	const Port serverPort((unsigned short)(readablePort), Port::TYPE_READABLE);
	// send a dummy message to receive an routing entry in the local NAT table
	if (serverPort.isNull() == false)
	{
		udpClient_.send(tcpClient_.receiverAddress(), serverPort, "NAT entry message");
		Log::info() << name_ << " send a UDP dummy message from UDP client port: " << udpClient_.port().readable() << " to UDP server port: " << serverPort.readable() << " create an entry in NAT.";
	}


	if (tcpClient_.send(createCommand(startCommand(), sessionId)) != TCPClient::SR_SUCCEEDED)
	{
		Log::error() << name_ << " could not send a start request.";
		return false;
	}
	else
	{
		Log::info() << name_ << " sent a start request.";
	}

	if (messageQueue_.pop(sessionId, responseTimeout_, response, value) == false || response != startResponseP())
	{
		Log::error() << name_ << " received no positive start response.";
		return false;
	}
	else
	{
		Log::info() << name_ << " received a start response.";
	}

	if (udpClient_.start() == false)
	{
		Log::error() << "The UDP client is not able to receive streaming data.";
		stop();
		return false;
	}

	channel_ = channel;

	isPaused_ = false;
	isReceiving_ = true;
	return true;
}

bool StreamingClient::pause()
{
	const ScopedLock scopedLock(lock_);

	if (isReceiving_ == false)
	{
		return false;
	}

	const SessionId sessionId = messageQueue_.uniqueId();

	if (tcpClient_.send(createCommand(pauseCommand(), sessionId)) != TCPClient::SR_SUCCEEDED)
	{
		Log::error() << name_ << " could not send a pause request.";
		return false;
	}
	else
	{
		Log::info() << name_ << " sent a pause request.";
	}

	if (messageQueue_.pop(sessionId, responseTimeout_) != pauseResponseP())
	{
		Log::error() << name_ << " received no positive pause response.";
		return false;
	}
	else
	{
		Log::info() << name_ << " received a pause response.";
	}

	udpClient_.stop();
	isPaused_ = true;
	return true;
}

bool StreamingClient::stop()
{
	const ScopedLock scopedLock(lock_);

	if (isReceiving_ == false)
	{
		return true;
	}

	if (tcpClient_.isConnected() == false)
	{
		return false;
	}

	const SessionId sessionId = messageQueue_.uniqueId();

	if (tcpClient_.send(createCommand(stopCommand(), sessionId)) != TCPClient::SR_SUCCEEDED)
	{
		Log::error() << name_ << " could not send a stop request.";
		return false;
	}
	else
	{
		Log::info() << name_ << " sent a stop request.";
	}

	if (messageQueue_.pop(sessionId, responseTimeout_) != stopResponseP())
	{
		Log::error() << "Server did not stop the stream, however the client will be stopped.";
	}
	else
	{
		Log::info() << name_ << " received a stop response.";
	}

	udpClient_.stop();

	isReceiving_ = false;
	isPaused_ = false;

	return true;
}

void StreamingClient::onCommand(const std::string& command, const std::string& value, const SessionId sessionId)
{
	if (command == startCommand())
	{
		onStart(value, sessionId);
	}
	else if (command == pauseCommand())
	{
		onPause(value, sessionId);
	}
	else if (command == stopCommand())
	{
		onStop(value, sessionId);
	}
	else if (command == changedDataTypeCommand())
	{
		onChangedDataType(value, sessionId);
	}
	else
	{
		Log::warning() << "Unknown streaming server command: \"" << command << "\" with value \"" << value << "\".";
	}
}

void StreamingClient::onStart(const std::string& value, const SessionId sessionId)
{
	bool accepted = true;

	if (requestCallback_)
	{
		accepted = requestCallback_(STATE_START, value);
	}

	if (accepted)
	{
		dataType_ = value;

		Log::info() << name_ << " receives a start request from the streaming server and accepts it.";
		tcpClient_.send(createResponse(startResponseP(), sessionId));
	}
	else
	{
		Log::info() << name_ << "receives a start request from the streaming server, however it's not accepted.";
		tcpClient_.send(createResponse(startResponseN(), sessionId));
	}
}

void StreamingClient::onPause(const std::string& value, const SessionId sessionId)
{
	bool accepted = true;

	if (requestCallback_)
	{
		accepted = requestCallback_(STATE_PAUSE, value);
	}

	if (accepted)
	{
		Log::info() << name_ << " receives a pause request from the streaming server and accepts it.";
		tcpClient_.send(createResponse(pauseResponseP(), sessionId));
	}
	else
	{
		Log::info() << name_ << "receives a pause request from the streaming server, however it's not accepted.";
		tcpClient_.send(createResponse(pauseResponseN(), sessionId));
	}
}

void StreamingClient::onStop(const std::string& value, const SessionId sessionId)
{
	bool accepted = true;

	if (requestCallback_)
	{
		accepted = requestCallback_(STATE_STOP, value);
	}

	if (accepted)
	{
		Log::info() << name_ << " receives a stop request from the streaming server and accepts it.";
		tcpClient_.send(createResponse(stopResponseP(), sessionId));
	}
	else
	{
		Log::info() << name_ << "receives a stop request from the streaming server, however it's not accepted.";
		tcpClient_.send(createResponse(stopResponseN(), sessionId));
	}
}

void StreamingClient::onChangedDataType(const std::string& value, const SessionId sessionId)
{
	bool accepted = true;

	// independent from the
	std::string oldClientDataType = dataType_;
	dataType_ = value;

	if (requestCallback_)
	{
		accepted = requestCallback_(STATE_TYPE_CHANGED, value);
	}

	if (accepted)
	{
		Log::info() << name_ << " receives a change data type request \"" << value << "\" from the streaming server and accepts it.";
		tcpClient_.send(createResponse(changedDataTypeResponseP(), sessionId));
	}
	else
	{
		Log::info() << name_ << "receives a change data type request \"" << value << "\" from the streaming server, however it's not accepted.";
		tcpClient_.send(createResponse(changedDataTypeResponseN(), sessionId));
		dataType_ = oldClientDataType;
	}
}

void StreamingClient::onTCPReceiveData(const void* data, const size_t size)
{
	ocean_assert(data != nullptr && size != 0);

	bool isResponse = false;
	std::string message, value;
	SessionId sessionId;

	if (parse((unsigned char*)(data), size, isResponse, message, value, sessionId))
	{
		if (isResponse)
		{
			messageQueue_.push(sessionId, message, value);
		}
		else
		{
			onCommand(message, value, sessionId);
		}
	}
}

void StreamingClient::onUDPReceiveData(const Address4&, const Port&, const void* data, const size_t size, const PackagedSocket::MessageId messageId)
{
	if (data == nullptr)
	{
		Log::warning() << "We lost a UDP message: " << messageId;
		return;
	}

	if (receiveCallback_)
	{
		receiveCallback_(data, size);
	}
}

}

}
