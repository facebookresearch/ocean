/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/Messenger.h"
#include "ocean/base/DateTime.h"
#include "ocean/base/Maintenance.h"

#include <fstream>
#include <iostream>

#if defined(_ANDROID)
	#include <android/log.h>
#endif

namespace Ocean
{

Messenger::Messenger()
{
#ifdef OCEAN_INTENSIVE_DEBUG
	writeToDebugOutput("Messenger::Messenger()");
#endif
}

Messenger::~Messenger()
{
#ifdef OCEAN_INTENSIVE_DEBUG
	writeToDebugOutput("Messenger::~Messenger()");
#endif
}

void Messenger::push(const MessageType type, std::string&& location, std::string&& message)
{
#ifndef OCEAN_DEBUG
	if (type == TYPE_DEBUG)
	{
		ocean_assert(false && "This should never happen!");
		return;
	}
#endif

	const ScopedLock scopedLock(lock_);

	std::string messagePrefix;
	std::string locationAndTime = location;

	if (integrateDateTime_)
	{
		if (location.empty())
		{
			locationAndTime = DateTime::localStringDate() + std::string(", ") + DateTime::localStringTime(true);
		}
		else
		{
			locationAndTime = DateTime::localStringDate() + std::string(", ") + DateTime::localStringTime(true) + std::string(": ") + location;
		}
	}

	switch (type)
	{
		case TYPE_UNDEFINED:
			ocean_assert(false && "Unknown message type.");
			break;

		case TYPE_DEBUG:
			messagePrefix = std::string("Debug: ");
			break;

		case TYPE_INFORMATION:
			break;

		case TYPE_WARNING:
			messagePrefix = std::string("Warning: ");
			break;

		case TYPE_ERROR:
			messagePrefix = std::string("Error: ");
			break;
	}

	// we can stop here if the output type is none of the remaining output types
	if ((outputType_ | (~OUTPUT_QUEUED)) != 0)
	{
		std::string augmentedMessage;

		if (locationAndTime.empty())
		{
			if (message == "\n" || message == "\r\n")
			{
				augmentedMessage = messagePrefix;
			}
			else
			{
				augmentedMessage = messagePrefix + message;
			}
		}
		else
		{
			if (message == "\n" || message == "\r\n")
			{
				augmentedMessage = messagePrefix + locationAndTime;
			}
			else
			{
				augmentedMessage = messagePrefix + locationAndTime + std::string(", ") + message;
			}
		}

		if (outputType_ & OUTPUT_FILE)
		{
			fileOutputStream_ << augmentedMessage.c_str() << std::endl;
		}

		if (outputType_ & OUTPUT_STREAM)
		{
			if (outputStream_)
			{
				*outputStream_ << augmentedMessage.c_str() << std::endl;
			}
		}

		if (outputType_ & OUTPUT_MAINTENANCE)
		{
			Maintenance::get().send("OCEAN_MESSENGER", augmentedMessage.c_str(), augmentedMessage.length());
		}

#ifdef _ANDROID
		if ((outputType_ & OUTPUT_STANDARD) || (outputType_ & OUTPUT_DEBUG_WINDOW))
		{
			// on Android platforms both output types end in the special Log system of Android
			const char logcatTag[] = "Ocean";
			augmentedMessage = locationAndTime + (locationAndTime.empty() ? "" : ": ") + message;

			switch (type)
			{
				case TYPE_ERROR:
					__android_log_write(ANDROID_LOG_ERROR, logcatTag, augmentedMessage.c_str());
					break;

				case TYPE_WARNING:
					__android_log_write(ANDROID_LOG_WARN, logcatTag, augmentedMessage.c_str());
					break;

#ifdef OCEAN_DEBUG
				case TYPE_DEBUG:
					__android_log_write(ANDROID_LOG_DEBUG, logcatTag, augmentedMessage.c_str());
					break;
#endif

				default:
					__android_log_write(ANDROID_LOG_INFO, logcatTag, augmentedMessage.c_str());
					break;
			}
		}
#else // _ANDROID

		if (outputType_ & OUTPUT_DEBUG_WINDOW)
		{
#if defined(_WINDOWS)
			OutputDebugStringA((std::string("Ocean, ") + augmentedMessage + std::string("\n")).c_str());
#elif defined(__APPLE__)
			writeMessageToDebugWindowApple(augmentedMessage);
#else
			std::cout << augmentedMessage.c_str() << std::endl;
#endif
		}

		if (outputType_ & OUTPUT_STANDARD)
		{
			std::cout << augmentedMessage.c_str() << std::endl;
		}

#endif // _ANDROID
	}

	if ((outputType_ & OUTPUT_QUEUED) != OUTPUT_QUEUED)
	{
		return;
	}

	switch (type)
	{
		case TYPE_UNDEFINED:
			break;

		case TYPE_DEBUG:
		{
#ifdef OCEAN_DEBUG
			if (debugMessageQueue_.size() >= maxMessages_)
			{
				debugMessageQueue_.pop();
			}

			debugMessageQueue_.emplace(std::move(locationAndTime), std::move(message));
#endif

			return;
		}

		case TYPE_INFORMATION:
		{
			if (informationMessageQueue_.size() >= maxMessages_)
			{
				informationMessageQueue_.pop();
			}

			informationMessageQueue_.emplace(std::move(locationAndTime), std::move(message));
			return;
		}

		case TYPE_WARNING:
		{
			if (warningMessageQueue_.size() >= maxMessages_)
			{
				warningMessageQueue_.pop();
			}

			warningMessageQueue_.emplace(std::move(locationAndTime), std::move(message));
			return;
		}

		case TYPE_ERROR:
		{
			if (errorMessageQueue_.size() >= maxMessages_)
			{
				errorMessageQueue_.pop();
			}

			errorMessageQueue_.emplace(std::move(locationAndTime), std::move(message));
			return;
		}
	}

	ocean_assert(false && "Unknown message type.");
}

void Messenger::writeToDebugOutput(const std::string& message)
{
#if defined(_WINDOWS)

	OutputDebugStringA((std::string("Ocean, ") + message + std::string("\n")).c_str());

#elif defined(_ANDROID)

	__android_log_write(ANDROID_LOG_INFO, "Ocean", message.c_str());

#elif defined(__APPLE__)

	writeMessageToDebugWindowApple(message);

#else

	std::cout << "Ocean, " << message.c_str() << std::endl;

#endif
}

bool Messenger::popDebug(std::string& location, std::string& message, bool* isNew)
{
#ifdef OCEAN_DEBUG

	const ScopedLock scopedLock(lock_);

	if (debugMessageQueue_.empty())
	{
		return false;
	}

	location = std::move(debugMessageQueue_.front().first);
	message = std::move(debugMessageQueue_.front().second);

	if (isNew)
	{
		*isNew = message != lastDebugMessage_;
	}

	lastDebugMessage_ = message;

	debugMessageQueue_.pop();
	return true;

#else

	OCEAN_SUPPRESS_UNUSED_WARNING(location);
	OCEAN_SUPPRESS_UNUSED_WARNING(message);
	OCEAN_SUPPRESS_UNUSED_WARNING(isNew);

	return false;

#endif // OCEAN_DEBUG
}

bool Messenger::popInformation(std::string& location, std::string& message, bool* isNew)
{
	const ScopedLock scopedLock(lock_);

	if (informationMessageQueue_.empty())
	{
		return false;
	}

	location = std::move(informationMessageQueue_.front().first);
	message = std::move(informationMessageQueue_.front().second);

	if (isNew)
	{
		*isNew = message != lastInformationMessage_;
	}

	lastInformationMessage_ = message;

	informationMessageQueue_.pop();
	return true;
}

bool Messenger::popWarning(std::string& location, std::string& message, bool* isNew)
{
	const ScopedLock scopedLock(lock_);

	if (warningMessageQueue_.empty())
	{
		return false;
	}

	location = std::move(warningMessageQueue_.front().first);
	message = std::move(warningMessageQueue_.front().second);

	if (isNew)
	{
		*isNew = message != lastWarningMessage_;
	}

	lastWarningMessage_ = message;

	warningMessageQueue_.pop();
	return true;
}

bool Messenger::popError(std::string& location, std::string& message, bool* isNew)
{
	const ScopedLock scopedLock(lock_);

	if (errorMessageQueue_.empty())
	{
		return false;
	}

	location = std::move(errorMessageQueue_.front().first);
	message = std::move(errorMessageQueue_.front().second);

	if (isNew)
	{
		*isNew = message != lastErrorMessage_;
	}

	lastErrorMessage_ = message;

	errorMessageQueue_.pop();
	return true;
}

bool Messenger::popMessage(MessageType& type, std::string& location, std::string& message, bool* isNew)
{
	const ScopedLock scopedLock(lock_);

	if ((type == TYPE_UNDEFINED || type == TYPE_ERROR) && !errorMessageQueue_.empty())
	{
		type = TYPE_ERROR;
		return popError(location, message, isNew);
	}

	if ((type == TYPE_UNDEFINED || type == TYPE_WARNING) && !warningMessageQueue_.empty())
	{
		type = TYPE_WARNING;
		return popWarning(location, message, isNew);
	}

	if ((type == TYPE_UNDEFINED || type == TYPE_INFORMATION) && !informationMessageQueue_.empty())
	{
		type = TYPE_INFORMATION;

		return popInformation(location, message, isNew);
	}

	if (type == TYPE_UNDEFINED || type == TYPE_DEBUG)
	{
#ifdef OCEAN_DEBUG
		if (!debugMessageQueue_.empty())
		{
			type = TYPE_DEBUG;

			return popDebug(location, message, isNew);
		}
#endif
	}

	return false;
}

std::string Messenger::popMessage(const MessageType type, bool* isNew)
{
	MessageType messageType(type);
	std::string location, message;

	if (!popMessage(messageType, location, message, isNew))
	{
		return std::string();
	}

	switch (messageType)
	{
		case TYPE_UNDEFINED:
			break;

		case TYPE_ERROR:
			return std::string("Error: ") + (location.empty() ? message : (location + std::string(", ") + message));

		case TYPE_WARNING:
			return std::string("Warning: ") + (location.empty() ? message : (location + std::string(", ") + message));

		case TYPE_INFORMATION:
		{
			if (location.empty())
			{
				return message;
			}
			else
			{
				return location + std::string(", ") + message;
			}

			break;
		}

		case TYPE_DEBUG:
		{
#ifdef OCEAN_DEBUG
			return std::string("Debug: ") + (location.empty() ? message : (location + std::string(", ") + message));
#else
			ocean_assert(false && "This should never happen!");
#endif
			break;
		}
	}

	ocean_assert(false && "Invalid message type!");
	return std::string();
}

bool Messenger::setOutputType(const MessageOutput type)
{
	const ScopedLock scopedLock(lock_);

	outputType_ = type;
	return true;
}

bool Messenger::setFileOutput(const std::string& filename)
{
#ifdef OCEAN_DEACTIVATED_MESSENGER

	// If the messenger is deactivated, we do not want the output files to be created:
	OCEAN_SUPPRESS_UNUSED_WARNING(filename);
	return true;

#else

	const ScopedLock scopedLock(lock_);

	if (fileOutputStream_.is_open())
	{
		fileOutputStream_.close();
	}

	fileOutputStream_.open(filename.c_str(), std::ios::binary);
	return fileOutputStream_.good();

#endif
}

bool Messenger::setOutputStream(std::ostream& stream)
{
	const ScopedLock scopedLock(lock_);

	outputStream_ = &stream;
	return true;
}

void Messenger::setIntegrateDateTime(const bool state)
{
	const ScopedLock scopedLock(lock_);
	integrateDateTime_ = state;
}

void Messenger::flush(std::ostream& stream)
{
	const ScopedLock scopedLock(lock_);

	std::string message;
	bool isNew = false;

	while (true)
	{
		message = popMessage(TYPE_UNDEFINED, &isNew);

		if (message.empty())
		{
			break;
		}

		stream << message << std::endl;
	}
}

void Messenger::clear()
{
	const ScopedLock scopedLock(lock_);

	informationMessageQueue_ = MessageQueue();
	warningMessageQueue_ = MessageQueue();
	errorMessageQueue_ = MessageQueue();
}

void Messenger::clearInformations()
{
	const ScopedLock scopedLock(lock_);
	informationMessageQueue_ = MessageQueue();
}

void Messenger::clearWarnings()
{
	const ScopedLock scopedLock(lock_);
	warningMessageQueue_ = MessageQueue();
}

void Messenger::clearErrors()
{
	const ScopedLock scopedLock(lock_);
	errorMessageQueue_ = MessageQueue();
}

} // namespace Ocean
