/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/network/StreamingServer.h"

#include "ocean/base/String.h"

namespace Ocean
{

namespace Network
{

StreamingServer::Channel::Stream::Stream(const TCPServer::ConnectionId tcpConnectionId, const Address4& receiverAddress, const Port& receiverPort) :
	tcpConnectionId_(tcpConnectionId),
	address_(receiverAddress),
	port_(receiverPort)
{
	ocean_assert(address_.isNull() == false);
	ocean_assert(port_.isNull() == false);
}

bool StreamingServer::Channel::Stream::start()
{
	isStreaming_ = true;
	return true;
}

bool StreamingServer::Channel::Stream::pause()
{
	isStreaming_ = false;
	return true;
}

bool StreamingServer::Channel::Stream::stream(const void* data, const size_t size)
{
	if (isStreaming_)
	{
		ocean_assert(udpClient_.id() != Socket::invalidSocketId());
		return udpClient_.send(address_, port_, data, size) == PackagedUDPClient::SR_SUCCEEDED;
	}

	return false;
}

StreamingServer::Channel::Channel(const std::string& name, const std::string& dataType, const ChannelCallback& callback) :
	name_(name),
	dataType_(dataType),
	channelCallback_(callback)
{
	// nothing to do here
}

StreamingServer::Channel::StreamId StreamingServer::Channel::addStream(const TCPServer::ConnectionId tcpConnectionId, const Address4& address, const Port& port)
{
	if (address.isNull() || port.isNull())
	{
		return false;
	}

	ocean_assert(tcpConnectionId != TCPServer::invalidConnectionId());

	// ensure that this channel does not exist
	for (StreamMap::const_iterator i = streamMap_.begin(); i != streamMap_.end(); ++i)
	{
		ocean_assert(i->second->tcpConnectionId() != tcpConnectionId);
		ocean_assert(i->second->receiverAddress() != address || i->second->receiverPort() != port);
	}

	streamMap_[streamIdCounter_] = std::make_shared<Stream>(tcpConnectionId, address, port);
	return streamIdCounter_++;
}

bool StreamingServer::Channel::removeStream(const StreamId streamId)
{
	StreamMap::iterator i = streamMap_.find(streamId);
	if (i == streamMap_.end())
	{
		return false;
	}

	if (i->second->isStreaming())
	{
		ocean_assert(activeStreams_ != 0u);
		--activeStreams_;
	}

	streamMap_.erase(i);

	if (channelCallback_)
	{
		if (streamMap_.empty())
		{
			channelCallback_(STATE_STOP);
		}
		else if (activeStreams_ == 0u)
		{
			channelCallback_(STATE_PAUSE);
		}
	}

	return true;
}

bool StreamingServer::Channel::startStream(const StreamId streamId)
{
	StreamMap::iterator i = streamMap_.find(streamId);
	if (i == streamMap_.end())
	{
		return false;
	}

	// check whether this stream is streaming already
	if (i->second->isStreaming())
	{
		return true;
	}

	// start the stream
	if (i->second->start() == false)
	{
		return false;
	}

	// handle event callback function
	++activeStreams_;
	if (channelCallback_ && activeStreams_ == 1u)
	{
		channelCallback_(STATE_START);
	}

	return true;
}

bool StreamingServer::Channel::pauseStream(const StreamId streamId)
{
	StreamMap::iterator i = streamMap_.find(streamId);
	if (i == streamMap_.end())
	{
		return false;
	}

	// check whether the stream is paused already
	if (i->second->isStreaming() == false)
	{
		return true;
	}

	// pause the stream
	i->second->pause();

	// handle event callback function
	ocean_assert(activeStreams_ != 0u);
	--activeStreams_;
	if (channelCallback_ && activeStreams_ == 0u)
	{
		channelCallback_(STATE_PAUSE);
	}

	return true;
}

bool StreamingServer::Channel::stopStream(const StreamId streamId)
{
	StreamMap::iterator i = streamMap_.find(streamId);
	if (i == streamMap_.end())
	{
		return false;
	}

	// **TODO** handle pause and stop different inside the stream
	if (i->second->isStreaming())
	{
		ocean_assert(activeStreams_ != 0u);
		--activeStreams_;
	}
	i->second->pause();

	// handle event callback function
	if (channelCallback_ && activeStreams_ == 0u)
	{
		channelCallback_(STATE_PAUSE);
	}

	return true;
}

Port StreamingServer::Channel::streamSenderPort(const StreamId streamId) const
{
	StreamMap::const_iterator i = streamMap_.find(streamId);
	if (i == streamMap_.end())
	{
		return Port();
	}

	return i->second->senderPort();
}

bool StreamingServer::Channel::setDataType(TCPServer& configurationTCPServer, MessageQueue& messageQueue, const std::string& dataType)
{
	if (dataType == dataType_)
	{
		return true;
	}

	dataType_ = dataType;

	/// inform connected streaming clients on this change data type
	for (StreamMap::iterator i = streamMap_.begin(); i != streamMap_.end(); ++i)
	{
		const SessionId sessionId = messageQueue.uniqueId();
		if (configurationTCPServer.send(i->second->tcpConnectionId(), createCommand(changedDataTypeCommand(), dataType, sessionId)) != TCPServer::SR_SUCCEEDED)
		{
			Log::error() << "Could not send a change data type command to a streaming client.";
		}
		else
		{
			// **TODO** use user defined timeout
			if (messageQueue.pop(sessionId, 5) != changedDataTypeResponseP())
			{
				Log::error() << "A streaming client did not accept the change data type, however it is change on server side.";
			}
		}
	}

	return true;
}

bool StreamingServer::Channel::stream(const void* data, const size_t size)
{
	bool allSuccessfull = true;

	for (StreamMap::iterator i = streamMap_.begin(); i != streamMap_.end(); ++i)
	{
		if (i->second->isStreaming())
		{
			if (i->second->stream(data, size) == false)
			{
				allSuccessfull = false;
			}
		}
	}

	return allSuccessfull;
}

StreamingServer::StreamingServer()
{
	tcpServer_.setConnectionRequestCallback(TCPServer::ConnectionRequestCallback(*this, &StreamingServer::onTCPConnection));
	tcpServer_.setReceiveCallback(TCPServer::ReceiveCallback(*this, &StreamingServer::onTCPReceive));
}

StreamingServer::~StreamingServer()
{
	release();
	disable();
}

bool StreamingServer::setAddress(const Address4& address)
{
	const ScopedLock scopedLock(lock_);

	if (isEnabled_)
	{
		return false;
	}

	return tcpServer_.setAddress(address);
}

bool StreamingServer::setPort(const Port& port)
{
	const ScopedLock scopedLock(lock_);

	if (isEnabled_)
	{
		return false;
	}

	return tcpServer_.setPort(port);
}

Address4 StreamingServer::address() const
{
	const ScopedLock scopedLock(lock_);

	return tcpServer_.address();
}

Port StreamingServer::port() const
{
	const ScopedLock scopedLock(lock_);

	return tcpServer_.port();
}

bool StreamingServer::enable()
{
	const ScopedLock scopedLock(lock_);

	if (isEnabled_)
	{
		return true;
	}

	isEnabled_ = tcpServer_.start();

	return isEnabled_;
}

bool StreamingServer::disable()
{
	const ScopedLock scopedLock(lock_);

	if (isEnabled_ == false)
	{
		return true;
	}

	bool result = tcpServer_.stop();
	ocean_assert_and_suppress_unused(result, result);

	isEnabled_ = false;
	return true;
}

StreamingServer::ChannelId StreamingServer::registerChannel(const std::string& channel, const std::string& dataType, const ChannelCallback& callback)
{
	if (channel.empty())
	{
		return invalidChannelId();
	}

	const ScopedLock scopedLock(lock_);

	for (ChannelMap::const_iterator i = channelMap_.begin(); i != channelMap_.end(); ++i)
	{
		if (i->second.name() == channel)
		{
			return invalidChannelId();
		}
	}

	ocean_assert(channelMap_.find(channelIdCounter_) == channelMap_.end());

	channelMap_[channelIdCounter_] = Channel(channel, dataType, callback);
	return channelIdCounter_++;
}

bool StreamingServer::changeDataType(const ChannelId channelId, const std::string& dataType)
{
	if (channelId == invalidChannelId())
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	ChannelMap::iterator i = channelMap_.find(channelId);
	if (i == channelMap_.end())
	{
		return false;
	}

	return i->second.setDataType(tcpServer_, messageQueue_, dataType);
}

bool StreamingServer::unregisterChannel(const ChannelId channelId)
{
	if (channelId == invalidChannelId())
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	ChannelMap::iterator iChannel = channelMap_.find(channelId);
	if (iChannel == channelMap_.end())
	{
		return false;
	}

	for (ConnectionMap::iterator iConnection = connectionMap_.begin(); iConnection != connectionMap_.end(); /* noop */)
	{
		const Connection& streamConnection = iConnection->second;

		if (iConnection->second.channelId() == channelId)
		{
			iChannel->second.removeStream(streamConnection.channelStreamId());

			const SessionId sessionId = messageQueue_.uniqueId();
			if (tcpServer_.send(iConnection->first, createCommand(disconnectCommand(), sessionId)) != TCPServer::SR_SUCCEEDED)
			{
				Log::error() << "Could not send a disconnect command to the streaming client.";
			}
			else
			{
				if (messageQueue_.pop(sessionId, responseTimeout_) != disconnectResponseP())
				{
					Log::error() << "The client did not accept the disconnect command, however the server will close the connection anyway.";
				}
			}

			tcpServer_.disconnect(iConnection->first);

			iConnection = connectionMap_.erase(iConnection);
		}
		else
		{
			++iConnection;
		}
	}

	channelMap_.erase(iChannel);
	return true;
}

bool StreamingServer::hasChannel(const std::string& channel) const
{
	const ScopedLock scopedLock(lock_);

	for (ChannelMap::const_iterator i = channelMap_.begin(); i != channelMap_.end(); ++i)
	{
		if (i->second.name() == channel)
		{
			return true;
		}
	}

	return false;
}

void StreamingServer::release()
{
	const ScopedLock scopedLock(lock_);

	while (channelMap_.empty() == false)
	{
		unregisterChannel(channelMap_.begin()->first);
	}
}

bool StreamingServer::stream(const ChannelId channelId, const void* data, const size_t size)
{
	const ScopedLock scopedLock(lock_);

	if (isEnabled_ == false)
	{
		return false;
	}

	ChannelMap::iterator i = channelMap_.find(channelId);
	if (i == channelMap_.end())
	{
		return false;
	}

	return i->second.stream(data, size);
}

std::string StreamingServer::generateUniqueChannel() const
{
	const ScopedLock scopedLock(lock_);

	unsigned int counter = 0;
	while (counter < channelIdCounter_ + 1)
	{
		std::string channelName = std::string("channel") + String::toAString(counter);

		if (hasChannel(channelName) == false)
		{
			return channelName;
		}

		++counter;
	}

	ocean_assert(false && "Invalid channel name - this should never happen");
	return std::string("");
}

bool StreamingServer::onTCPConnection(const Address4& /*address*/, const Port& /*port*/, const TCPServer::ConnectionId /*connectionId*/)
{
	// always accepts raw TCP connections
	return true;
}

void StreamingServer::onCommand(const TCPServer::ConnectionId tcpConnectionId, const std::string& command, const std::string& value, const SessionId sessionId)
{
	const ScopedLock scopedLock(lock_);

	if (command == connectCommand())
	{
		onConnect(tcpConnectionId, value, sessionId);
	}
	else if (command == disconnectCommand())
	{
		onDisconnect(tcpConnectionId, value, sessionId);
	}
	else if (command == clientPortCommand())
	{
		onClientPort(tcpConnectionId, value, sessionId);
	}
	else if (command == serverPortCommand())
	{
		onServerPort(tcpConnectionId, value, sessionId);
	}
	else if (command == channelSelectCommand())
	{
		onChannelSelect(tcpConnectionId, value, sessionId);
	}
	else if (command == startCommand())
	{
		onStart(tcpConnectionId, value, sessionId);
	}
	else if (command == pauseCommand())
	{
		onPause(tcpConnectionId, value, sessionId);
	}
	else if (command == stopCommand())
	{
		onStop(tcpConnectionId, value, sessionId);
	}
	else if (command == channelRequestCommand())
	{
		onChannelRequest(tcpConnectionId, value, sessionId);
	}
	else if (command == dataTypeRequestCommand())
	{
		onDataTypeRequest(tcpConnectionId, value, sessionId);
	}
	else
	{
		Log::warning() << "Unknown streaming client command: \"" << command << "\" with value \"" << value << "\".";
	}
}

void StreamingServer::onConnect(const TCPServer::ConnectionId tcpConnectionId, const std::string& /*value*/, const SessionId sessionId)
{
	Address4 requestingAddress;
	Port requestingPort;
	if (tcpServer_.connectionProperties(tcpConnectionId, requestingAddress, requestingPort) == false)
	{
		ocean_assert(false && "This should never happen.");
	}

	if (connectionMap_.find(tcpConnectionId) == connectionMap_.end())
	{
		// we can use the address only, the port in not the stream data port
		connectionMap_[tcpConnectionId] = Connection(requestingAddress);
	}

	Log::info() << name_ << " got a connection request from " << requestingAddress.readable() << " and the server accepts it.";

	tcpServer_.send(tcpConnectionId, createResponse(connectResponseP(), sessionId));
}

void StreamingServer::onDisconnect(const TCPServer::ConnectionId tcpConnectionId, const std::string& /*value*/, const SessionId sessionId)
{
	ConnectionMap::iterator i = connectionMap_.find(tcpConnectionId);

	if (i == connectionMap_.end())
	{
		tcpServer_.send(tcpConnectionId, createResponse(disconnectResponseN(), sessionId));
		Log::warning() << name_ << " got a disconnection request from an unknown client.";
	}
	else
	{
		const Connection& streamConnection = i->second;

		ChannelMap::iterator iChannel = channelMap_.find(streamConnection.channelId());

		if (iChannel != channelMap_.end())
		{
			iChannel->second.stopStream(streamConnection.channelStreamId());
		}

		Log::info() << name_ << " got a disconnection request from " << streamConnection.address().readable() << " and the server accepts it.";

		tcpServer_.send(tcpConnectionId, createResponse(disconnectResponseP(), sessionId));
	}
}

void StreamingServer::onClientPort(const TCPServer::ConnectionId tcpConnectionId, const std::string& value, const SessionId sessionId)
{
	ConnectionMap::iterator i = connectionMap_.find(tcpConnectionId);

	if (i != connectionMap_.end())
	{
		Connection& streamConnection = i->second;

		/// the highest port can be 65535
		if (value.length() <= 5)
		{
			int readablePort = atoi(value.c_str());

			if (readablePort > 0 && readablePort < 65535)
			{
				if (streamConnection.setPort(Port((unsigned short)(readablePort), Port::TYPE_READABLE)))
				{
					Log::info() << name_ << " got a port change request with new port \"" << readablePort << "\" from " << i->second.address().readable() << " and the server accepts it.";

					tcpServer_.send(tcpConnectionId, createResponse(clientPortResponseP(), sessionId));

					if (streamConnection.channelId() != invalidChannelId())
					{
						ocean_assert(streamConnection.channelStreamId() == Channel::invalidStreamId());
						ocean_assert(streamConnection.address().isNull() == false);

						ChannelMap::iterator iChannel = channelMap_.find(streamConnection.channelId());
						ocean_assert(iChannel != channelMap_.end());

						if (iChannel != channelMap_.end())
						{
							Channel& channel = iChannel->second;
							Channel::StreamId streamId = channel.addStream(tcpConnectionId, streamConnection.address(), streamConnection.port());
							ocean_assert(streamId != Channel::invalidStreamId());

							streamConnection.setChannelStreamId(streamId);
						}
					}

					return;
				}
			}
		}

		Log::warning() << name_ << " got a port change request from " << i->second.address().readable() << ", however the port could not be accepted.";
	}

	tcpServer_.send(tcpConnectionId, createResponse(clientPortResponseN(), sessionId));
}

void StreamingServer::onServerPort(const TCPServer::ConnectionId tcpConnectionId, const std::string& /*value*/, const SessionId sessionId)
{
	ConnectionMap::iterator i = connectionMap_.find(tcpConnectionId);

	if (i != connectionMap_.end())
	{
		Connection& streamConnection = i->second;

		if (streamConnection.channelStreamId() != Channel::invalidStreamId())
		{
			ocean_assert(streamConnection.channelId() != invalidChannelId());

			ChannelMap::iterator iChannel = channelMap_.find(streamConnection.channelId());
			ocean_assert(iChannel != channelMap_.end());

			if (iChannel != channelMap_.end())
			{
				const Channel& channel = iChannel->second;
				const Port senderPort = channel.streamSenderPort(streamConnection.channelStreamId());

				Log::info() << name_ << " got a server port request from \"" << i->second.address().readable() << "\" and the server answered with \"" << senderPort.readable() << "\".";
				tcpServer_.send(tcpConnectionId, createResponse(serverPortResponseP(), String::toAString(senderPort.readable()), sessionId));
				return;
			}
		}

		Log::warning() << name_ << " got a server port request from " << i->second.address().readable() << ", however the request was invalid.";
	}

	tcpServer_.send(tcpConnectionId, createResponse(serverPortResponseN(), sessionId));
}

void StreamingServer::onChannelSelect(const TCPServer::ConnectionId tcpConnectionId, const std::string& value, const SessionId sessionId)
{
	ChannelMap::iterator iChannel;
	for (iChannel = channelMap_.begin(); iChannel != channelMap_.end(); ++iChannel)
	{
		if (iChannel->second.name() == value)
		{
			break;
		}
	}

	if (iChannel == channelMap_.end())
	{
		// unknown channel
		if (value.empty() == false)
		{
			tcpServer_.send(tcpConnectionId, createResponse(channelSelectResponseN(), "Channel unknown", sessionId));
			return;
		}

		iChannel = channelMap_.begin();
	}

	ocean_assert(iChannel != channelMap_.end());
	ConnectionMap::iterator iConnection = connectionMap_.find(tcpConnectionId);

	if (iConnection != connectionMap_.end())
	{
		Connection& streamConnection = iConnection->second;

		if (streamConnection.channelId() != invalidChannelId())
		{
			if (streamConnection.channelId() == iChannel->first)
			{
				Log::info() << name_ << " got a channel select request from " << iConnection->second.address().readable() << " and will send an accepting response.";
				tcpServer_.send(tcpConnectionId, createResponse(channelSelectResponseP(), sessionId));
			}
			else
			{
				Log::info() << name_ << " got a channel select request from " << iConnection->second.address().readable() << ", however the client has selected a different channel before.";
				tcpServer_.send(tcpConnectionId, createResponse(channelSelectResponseN(), "The client selected a different channel before", sessionId));
			}

			return;
		}

		streamConnection.setChannelId(iChannel->first);

		if (streamConnection.port().isNull() == false)
		{
			ocean_assert(streamConnection.channelStreamId() == Channel::invalidStreamId());
			ocean_assert(streamConnection.address().isNull() == false);

			Channel& channel = iChannel->second;
			Channel::StreamId streamId = channel.addStream(tcpConnectionId, streamConnection.address(), streamConnection.port());
			ocean_assert(streamId != Channel::invalidStreamId());

			streamConnection.setChannelStreamId(streamId);
		}

		Log::info() << name_ << " got a channel select request from " << iConnection->second.address().readable() << " and will send an accepting response.";
		tcpServer_.send(tcpConnectionId, createResponse(channelSelectResponseP(), sessionId));
		return;
	}

	tcpServer_.send(tcpConnectionId, createResponse(channelSelectResponseN(), sessionId));
}

void StreamingServer::onStart(const TCPServer::ConnectionId tcpConnectionId, const std::string& /*value*/, const SessionId sessionId)
{
	ConnectionMap::iterator iConnection = connectionMap_.find(tcpConnectionId);

	if (iConnection != connectionMap_.end())
	{
		Connection& streamConnection = iConnection->second;

		if (streamConnection.channelStreamId() == Channel::invalidStreamId())
		{
			tcpServer_.send(tcpConnectionId, createResponse(startResponseN(), "Channel is not selected", sessionId));
			return;
		}

		ChannelMap::iterator iChannel = channelMap_.find(streamConnection.channelId());
		ocean_assert(iChannel != channelMap_.end());

		if (iChannel == channelMap_.end())
		{
			tcpServer_.send(tcpConnectionId, createResponse(startResponseN(), "Channel is not selected", sessionId));
			return;
		}

		Channel& channel = iChannel->second;
		if (channel.startStream(streamConnection.channelStreamId()))
		{
			Log::info() << name_ << " got a start request from " << iConnection->second.address().readable() << " and will send an accepting response.";

			tcpServer_.send(tcpConnectionId, createResponse(startResponseP(), channel.dataType(), sessionId));
			return;
		}

		Log::warning() << name_ << " got a start request from " << iConnection->second.address().readable() << ", however the server didn't accept.";
	}
	else
	{
		Log::warning() << name_ << " got a start request from an unknown client.";
	}

	tcpServer_.send(tcpConnectionId, createResponse(startResponseN(), sessionId));
}

void StreamingServer::onPause(const TCPServer::ConnectionId tcpConnectionId, const std::string& /*value*/, const SessionId sessionId)
{
	ConnectionMap::iterator iConnection = connectionMap_.find(tcpConnectionId);

	if (iConnection != connectionMap_.end())
	{
		Connection& streamConnection = iConnection->second;
		ChannelMap::iterator iChannel = channelMap_.find(streamConnection.channelId());

		if (iChannel != channelMap_.end())
		{
			Channel& channel = iChannel->second;

			if (channel.pauseStream(streamConnection.channelStreamId()))
			{
				Log::info() << name_ << " got a pause request " << iConnection->second.address().readable() << " and will receive an accepting response.";

				tcpServer_.send(tcpConnectionId, createResponse(pauseResponseP(), sessionId));
				return;
			}
		}

		Log::warning() << name_ << " got a pause request from " << iConnection->second.address().readable() << ", however the server didn't accept.";
	}
	else
	{
		Log::warning() << name_ << " got a pause request from an unknown client.";
	}

	tcpServer_.send(tcpConnectionId, createResponse(pauseResponseN(), sessionId));
}

void StreamingServer::onStop(const TCPServer::ConnectionId tcpConnectionId, const std::string& /*value*/, const SessionId sessionId)
{
	ConnectionMap::iterator iConnection = connectionMap_.find(tcpConnectionId);

	// if no connection exists
	if (iConnection != connectionMap_.end())
	{
		Connection& streamConnection = iConnection->second;

		ChannelMap::iterator iChannel = channelMap_.find(streamConnection.channelId());
		ocean_assert(iChannel != channelMap_.end());

		if (iChannel->second.stopStream(streamConnection.channelStreamId()))
		{
			Log::info() << name_ << " got a stop request " << iConnection->second.address().readable() << " and will receive an accepting response.";

			tcpServer_.send(tcpConnectionId, createResponse(stopResponseP(), sessionId));
			return;
		}

		Log::warning() << name_ << " got a stop request from " << iConnection->second.address().readable() << ", however the server didn't accept.";
	}
	else
	{
		Log::warning() << name_ << " got a stop request from an unknown client.";
	}

	tcpServer_.send(tcpConnectionId, createResponse(stopResponseN(), sessionId));
}

void StreamingServer::onChannelRequest(const TCPServer::ConnectionId tcpConnectionId, const std::string& /*value*/, const SessionId sessionId)
{
	std::string channels;

	for (ChannelMap::const_iterator i = channelMap_.begin(); i != channelMap_.end(); ++i)
	{
		if (i != channelMap_.begin())
		{
			channels += ";";
		}

		channels += i->second.name();
	}

	Address4 requestingAddress;
	Port requestingPort;
	if (tcpServer_.connectionProperties(tcpConnectionId, requestingAddress, requestingPort) == false)
	{
		ocean_assert(false && "This should never happen.");
	}

	Log::info() << name_ << " got a channel request from " << requestingAddress.readable() << " and send \"" << channels << "\" back.";

	tcpServer_.send(tcpConnectionId, createResponse(channelRequestResponseP(), channels, sessionId));
}

void StreamingServer::onDataTypeRequest(const TCPServer::ConnectionId tcpConnectionId, const std::string& value, const SessionId sessionId)
{
	ConnectionMap::iterator iConnection = connectionMap_.find(tcpConnectionId);

	if (iConnection != connectionMap_.end())
	{
		if (channelMap_.empty() == false)
		{
			ChannelMap::iterator iChannel = channelMap_.end();

			for (iChannel = channelMap_.begin(); iChannel != channelMap_.end(); ++iChannel)
			{
				if (iChannel->second.name() == value)
				{
					break;
				}
			}

			if (iChannel == channelMap_.end())
			{
				iChannel = channelMap_.begin();
			}

			ocean_assert(iChannel != channelMap_.end());

			Channel& channel = iChannel->second;

			Log::info() << name_ << " got a data type request for channel \"" << channel.name() << "\" from " << iConnection->second.address().readable() << " and send \"" << channel.dataType() << "\" back.";

			tcpServer_.send(tcpConnectionId, createResponse(dataTypeRequestResponseP(), channel.dataType(), sessionId));
			return;
		}
	}
	else
	{
		Log::warning() << name_ << " got a data type request from an unknown client.";
	}

	tcpServer_.send(tcpConnectionId, createResponse(dataTypeRequestResponseN(), sessionId));
}

void StreamingServer::onTCPReceive(const TCPServer::ConnectionId tcpConnectionId, const void* data, const size_t size)
{
	if (data == nullptr || size == 0)
	{
		return;
	}

	bool isResponse = false;
	std::string message, value;
	SessionId sessionId;

	if (parse((unsigned char*)data, size, isResponse, message, value, sessionId) == false)
	{
		return;
	}

	if (isResponse)
	{
		messageQueue_.push(sessionId, message, value);
	}
	else
	{
		onCommand(tcpConnectionId, message, value, sessionId);
	}
}

}

}
