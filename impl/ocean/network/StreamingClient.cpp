/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/network/StreamingClient.h"

#include "ocean/base/String.h"
#include "ocean/base/Thread.h"

#include "ocean/io/Base64.h"

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

	// retry TCP connection with ~1 second total timeout for transient network errors
	constexpr unsigned int maxRetries = 5u;
	constexpr unsigned int retryDelayMs = 200u;

	for (unsigned int retry = 0u; retry < maxRetries; ++retry)
	{
		if (tcpClient_.connect(address, port))
		{
			Log::info() << name_ << " established a TCP connection with the streaming server \"" << address.readable() << "\" with port \"" << port.readable() << "\".";
			break;
		}

		if (retry + 1u < maxRetries)
		{
			Log::debug() << name_ << " connection attempt " << (retry + 1u) << " failed, retrying...";
			Thread::sleep(retryDelayMs);
		}
		else
		{
			Log::error() << name_ << " could not establish a TCP connection with the streaming server \"" << address.readable() << "\" with port \"" << port.readable() << "\" after " << maxRetries << " attempts.";
			noError = false;
		}
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
		std::string::size_type pos = value.find(';');
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

StreamingClient::Buffer StreamingClient::channelExtraData(const std::string& channel)
{
	const ScopedLock scopedLock(lock_);

	if (tcpClient_.isConnected() == false)
	{
		Log::error() << name_ << " failed to receive channel extra data, because the client is not connected.";
		return Buffer();
	}

	const SessionId sessionId = messageQueue_.uniqueId();

	if (tcpClient_.send(createCommand(extraDataRequestCommand(), channel, sessionId)) != TCPClient::SR_SUCCEEDED)
	{
		Log::error() << name_ << " could not send an extra data request message.";
		return Buffer();
	}
	else
	{
		Log::info() << " sent an extra data request message.";
	}

	std::string response, value;

	if (messageQueue_.pop(sessionId, responseTimeout_, response, value) == false || response != extraDataRequestResponseP())
	{
		Log::error() << name_ << " received no answer for the extra data request.";
		return Buffer();
	}

	// decode the Base64 encoded extra data
	Buffer extraData;
	if (!value.empty())
	{
		if (!IO::Base64::decode(reinterpret_cast<const uint8_t*>(value.data()), value.size(), extraData))
		{
			Log::error() << name_ << " failed to decode Base64 extra data.";
			return Buffer();
		}
	}

	Log::info() << name_ << " received " << extraData.size() << " bytes of extra data.";

	return extraData;
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

	// request extra data (optional - failure is not fatal)
	if (tcpClient_.send(createCommand(extraDataRequestCommand(), sessionId)) != TCPClient::SR_SUCCEEDED)
	{
		Log::warning() << name_ << " could not send an extra data request.";
	}
	else
	{
		Log::info() << name_ << " sent an extra data request.";

		if (messageQueue_.pop(sessionId, responseTimeout_, response, value) == false || response != extraDataRequestResponseP())
		{
			Log::warning() << name_ << " received no positive extra data response.";
		}
		else
		{
			Log::info() << name_ << " received an extra data response.";

			// decode the Base64 encoded extra data
			if (!value.empty())
			{
				if (IO::Base64::decode(reinterpret_cast<const uint8_t*>(value.data()), value.size(), extraData_))
				{
					Log::info() << name_ << " decoded " << extraData_.size() << " bytes of extra data.";
				}
				else
				{
					Log::warning() << name_ << " failed to decode Base64 extra data.";
					extraData_.clear();
				}
			}
			else
			{
				extraData_.clear();
			}
		}
	}


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
	if (readablePort < 0 || readablePort > 65535)
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
	const ScopedLock scopedLock(lock_);

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
	// parse the combined value: "dataType|base64EncodedExtraData"
	std::string newDataType;
	Buffer newExtraData;

	const std::string::size_type delimiterPos = value.find('|');
	if (delimiterPos != std::string::npos)
	{
		newDataType = value.substr(0, delimiterPos);
		const std::string encodedExtraData = value.substr(delimiterPos + 1);

		if (!encodedExtraData.empty())
		{
			if (!IO::Base64::decode(reinterpret_cast<const uint8_t*>(encodedExtraData.data()), encodedExtraData.size(), newExtraData))
			{
				Log::error() << name_ << " failed to decode Base64 extra data from server.";
				tcpClient_.send(createResponse(changedDataTypeResponseN(), sessionId));
				return;
			}
		}
	}
	else
	{
		ocean_assert(false && "Invalid changed data type command format, expected 'dataType|base64EncodedExtraData'");
		tcpClient_.send(createResponse(changedDataTypeResponseN(), sessionId));
		return;
	}

	// store old values for potential rollback
	std::string oldClientDataType = dataType_;
	Buffer oldClientExtraData = extraData_;

	dataType_ = newDataType;
	extraData_ = newExtraData;

	bool accepted = true;
	if (dataTypeChangedCallback_)
	{
		accepted = dataTypeChangedCallback_(newDataType, newExtraData);
	}

	if (accepted)
	{
		Log::info() << name_ << " receives a change data type request \"" << newDataType << "\" with " << newExtraData.size() << " bytes extra data from the streaming server and accepts it.";
		tcpClient_.send(createResponse(changedDataTypeResponseP(), sessionId));
	}
	else
	{
		Log::info() << name_ << " receives a change data type request \"" << newDataType << "\" from the streaming server, however it's not accepted.";
		tcpClient_.send(createResponse(changedDataTypeResponseN(), sessionId));
		dataType_ = oldClientDataType;
		extraData_ = oldClientExtraData;
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
