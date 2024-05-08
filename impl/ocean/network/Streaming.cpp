/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/network/Streaming.h"

#include "ocean/base/String.h"

namespace Ocean
{

namespace Network
{

const std::string& Streaming::connectCommand()
{
	static const std::string value("connect");
	return value;
}

const std::string& Streaming::connectResponseP()
{
	static const std::string value("connected");
	return value;
}

const std::string& Streaming::connectResponseN()
{
	static const std::string value("notconnected");
	return value;
}

const std::string& Streaming::disconnectCommand()
{
	static const std::string value("disconnect");
	return value;
}

const std::string& Streaming::disconnectResponseP()
{
	static const std::string value("disconnected");
	return value;
}

const std::string& Streaming::disconnectResponseN()
{
	static const std::string value("notdisconnected");
	return value;
}

const std::string& Streaming::channelSelectCommand()
{
	static const std::string value("select");
	return value;
}
const std::string& Streaming::channelSelectResponseP()
{
	static const std::string value("selected");
	return value;
}

const std::string& Streaming::channelSelectResponseN()
{
	static const std::string value("notselected");
	return value;
}

const std::string& Streaming::startCommand()
{
	static const std::string value("start");
	return value;
}

const std::string& Streaming::startResponseP()
{
	static const std::string value("started");
	return value;
}

const std::string& Streaming::startResponseN()
{
	static const std::string value("notstarted");
	return value;
}

const std::string& Streaming::pauseCommand()
{
	static const std::string value("pause");
	return value;
}

const std::string& Streaming::pauseResponseP()
{
	static const std::string value("paused");
	return value;
}

const std::string& Streaming::pauseResponseN()
{
	static const std::string value("notpaused");
	return value;
}

const std::string& Streaming::stopCommand()
{
	static const std::string value("stop");
	return value;
}

const std::string& Streaming::stopResponseP()
{
	static const std::string value("stopped");
	return value;
}

const std::string& Streaming::stopResponseN()
{
	static const std::string value("notstopped");
	return value;
}

const std::string& Streaming::clientPortCommand()
{
	static const std::string value("clientPort");
	return value;
}

const std::string& Streaming::clientPortResponseP()
{
	static const std::string value("accepted");
	return value;
}

const std::string& Streaming::clientPortResponseN()
{
	static const std::string value("notaccepted");
	return value;
}

const std::string& Streaming::serverPortCommand()
{
	static const std::string value("serverPort");
	return value;
}

const std::string& Streaming::serverPortResponseP()
{
	static const std::string value("accepted");
	return value;
}

const std::string& Streaming::serverPortResponseN()
{
	static const std::string value("notaccepted");
	return value;
}

const std::string& Streaming::channelRequestCommand()
{
	static const std::string value("channels");
	return value;
}

const std::string& Streaming::channelRequestResponseP()
{
	static const std::string value("channels");
	return value;
}

const std::string& Streaming::channelRequestResponseN()
{
	static const std::string value("nochannels");
	return value;
}

const std::string& Streaming::dataTypeRequestCommand()
{
	static const std::string value("datatype");
	return value;
}

const std::string& Streaming::dataTypeRequestResponseP()
{
	static const std::string value("datatype");
	return value;
}

const std::string& Streaming::dataTypeRequestResponseN()
{
	static const std::string value("nodatatype");
	return value;
}

const std::string& Streaming::changedDataTypeCommand()
{
	static const std::string value("changeddatatype");
	return value;
}

const std::string& Streaming::changedDataTypeResponseP()
{
	static const std::string value("accepted");
	return value;
}

const std::string& Streaming::changedDataTypeResponseN()
{
	static const std::string value("notaccepted");
	return value;
}

Streaming::Streaming()
{
	// nothing to do here
}

bool Streaming::setName(const std::string& name)
{
	name_ = name;
	return true;
}

bool Streaming::setDescription(const std::string& description)
{
	description_ = description;
	return true;
}

bool Streaming::setResponseTimeout(const double timeout)
{
	responseTimeout_ = timeout;
	return true;
}

bool Streaming::parse(const unsigned char* data, const size_t size, bool& isResponse, std::string& message, std::string& optionalValue, SessionId& sessionId)
{
	std::string value;
	if (data[size - 1] == '\0')
		value = std::string((char*)data);
	else
		value = std::string((char*)data, size);

	if (value.find("response:") == 0)
	{
		value = value.substr(9);
		isResponse = true;
	}
	else if (value.find("command:") == 0)
	{
		value = value.substr(8);
		isResponse = false;
	}
	else
	{
		return false;
	}

	std::string::size_type pos = value.find(",id:");
	if (pos == std::string::npos)
	{
		return false;
	}

	std::string idString = value.substr(pos + 4);
	sessionId = atoi(idString.c_str());
	if (sessionId == MessageQueue::invalidId())
	{
		Log::warning() << "Received an invalid message id.";
		return false;
	}

	message = value.substr(0, pos);

	pos = message.find("-");
	if (pos != std::string::npos)
	{
		optionalValue = message.substr(pos + 1);
		message = message.substr(0, pos);
	}

	return true;
}

std::string Streaming::createCommand(const std::string& command, const SessionId sessionId)
{
	ocean_assert(command.empty() == false && sessionId != MessageQueue::invalidId());

	return std::string("command:") + command + std::string(",id:") + String::toAString(sessionId);
}

std::string Streaming::createCommand(const std::string& command, const std::string& value, const SessionId sessionId)
{
	ocean_assert(command.empty() == false && sessionId != MessageQueue::invalidId());

	if (value.empty())
	{
		return createCommand(command, sessionId);
	}

	return std::string("command:") + command + std::string("-") + value + std::string(",id:") + String::toAString(sessionId);
}

std::string Streaming::createResponse(const std::string& response, const SessionId sessionId)
{
	ocean_assert(response.empty() == false && sessionId != MessageQueue::invalidId());

	return std::string("response:") + response + std::string(",id:") + String::toAString(sessionId);
}

std::string Streaming::createResponse(const std::string& response, const std::string& value, const SessionId sessionId)
{
	ocean_assert(response.empty() == false && sessionId != MessageQueue::invalidId());

	if (value.empty())
	{
		return createResponse(response, sessionId);
	}

	return std::string("response:") + response + std::string("-") + value + std::string(",id:") + String::toAString(sessionId);
}

}

}
