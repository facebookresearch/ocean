/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_MESSENGER_H
#define META_OCEAN_BASE_MESSENGER_H

#include "ocean/base/Base.h"
#include "ocean/base/Lock.h"
#include "ocean/base/Singleton.h"
#include "ocean/base/String.h"

#include <fstream>
#include <iostream>
#include <queue>
#include <ostream>

namespace Ocean
{

/**
 * This class implements a messenger for information, warning or error messages.
 * Posted messages are stored in internal message queues or referred to a specified output file.<br>
 * Therefore, applications interested in live messaging have to pop the queued messages recurrently.
 *
 * There different message types are supported dividing all messages related to their magnitude:
 * Error messages hold critical informations which influence the major program progress directly.<br>
 * Warning messages hold informations which can influence minor program progresses, however they are not critical.<br>
 * Information messages hold interesting informations for the user only.<br>
 * Meaning that error messages should be informed to the user immediately, instead information messages can be informed to the user.<br>
 * Additionally, each message can provide a location specifying e.g. the sender module.
 *
 * Applications interested in messages use the Messenger object implemented as singleton to receive messages.
 * Modules use the MessageObject objects to post new messages.<br>
 * Three basic message types are defined: error, warning and info.
 * @see SpecificMessenger.
 *
 * Tutorial explaining the usage on application side:
 * @code
 * #include "ocean/base/Messenger.h"
 *
 * // Function called by e.g. a timer recurrently.
 * void timer()
 * {
 *     if (Messenger::get().empty() == false)
 *     {
 *         // Strings which will receive the popped informations
 *         std::string message, location;
 *
 *         // Boolean which will receive whether the information has been posted more than one time.
 *         bool isNew;
 *
 *         // Pop all error messages
 *         while (Messenger::get().popError(location, message, isNew))
 *         {
 *             // Writes the error message to any application output window.
 *             writeToErrorOutputWindow(location, message);
 *         }
 *
 *         // Pop all warning messages
 *         while (Messenger::get().popWarning(location, message, isNew))
 *         {
 *             // Writes the warning message to any application output window.
 *             writeToWarningOutputWindow(location, message);
 *         }
 *     }
 * }
 * @endcode
 *
 * Tutorial explaining the usage on module side:
 * @code
 * #include "ocean/base/Messenger.h"
 *
 * // Any function which will post a message.
 * void anyFunction()
 * {
 *     // A function calculating something
 *     int value = calculate();
 *
 *     // suppose a resulting value of '0' is a failure
 *     if (value == 0)
 *         Log::error() << "The calculation failed.";
 *     else
 *         Log::info() << "The calculation returned \"" << value << \" which is great!";
 * }
 * @endcode
 * @ingroup base
 */
class OCEAN_BASE_EXPORT Messenger : public Singleton<Messenger>
{
	/// Friend class
	friend class Singleton<Messenger>;

	public:

		/**
		 * Definition of different message types.
		 */
		enum MessageType : uint32_t
		{
			/// Invalid message type.
			TYPE_UNDEFINED = 0u,
			/// Debug message, which are not used on release builds (optimized out of the code).
			TYPE_DEBUG,
			/// Information message.
			TYPE_INFORMATION,
			/// Warning message.
			TYPE_WARNING,
			/// Error message.
			TYPE_ERROR
		};

		/**
		 * Definition of different message output types.
		 */
		enum MessageOutput : uint32_t
		{
			/// All messages will be discarded.
			OUTPUT_DISCARDED = 0u,
			/// All messages are directed to the standard output immediately.
			OUTPUT_STANDARD = 1u << 0u,
			/// All messages are queued and must be popped from the message stack explicitly.
			OUTPUT_QUEUED = 1u << 1u,
			/// All messages are directed to a debug window.
			OUTPUT_DEBUG_WINDOW = 1u << 2u,
			/// All messages are directed to a file immediately.
			OUTPUT_FILE = 1u << 3u,
			/// All messages are directed to an arbitrary stream immediately.
			OUTPUT_STREAM = 1u << 4u,
			/// All messages are directed to the maintenance manager.
			OUTPUT_MAINTENANCE = 1u << 5u
		};

	protected:

		/**
		 * Definition of a message.
		 */
		typedef std::pair<std::string, std::string> Message;

		/**
		 * Definition of a message queue.
		 */
		typedef std::queue<Message> MessageQueue;

	public:

		/**
		 * Pushes a new message into the message queue.
		 * @param type The type of the message
		 * @param location The location of the message
		 * @param message The text message
		 */
		void push(const MessageType type, std::string&& location, std::string&& message);

		/**
		 * Pops the first debug message from the message queue.
		 * @param location The resulting location of the message
		 * @param message The resulting text message
		 * @param isNew Optional returning flag determining whether this message is not identical to the last one
		 * @return True, if a message could be popped
		 */
		bool popDebug(std::string& location, std::string& message, bool* isNew = nullptr);

		/**
		 * Pops the first information message from the message queue.
		 * @param location The resulting location of the message
		 * @param message The resulting text message
		 * @param isNew Optional returning flag determining whether this message is not identical to the last one
		 * @return True, if a message could be popped
		 */
		bool popInformation(std::string& location, std::string& message, bool* isNew = nullptr);

		/**
		 * Pops the first warning message from the message queue.
		 * @param location The resulting location of the message
		 * @param message The resulting test message
		 * @param isNew Returning flag determining whether this message is not identical to the last one
		 * @return True, if a message could be popped
		 */
		bool popWarning(std::string& location, std::string& message, bool* isNew = nullptr);

		/**
		 * Pops the first error message from the message queue.
		 * @param location The resulting location of the message
		 * @param message The resulting text message
		 * @param isNew Returning flag determining whether this message is not identical to the last one
		 * @return True, if a message could be popped
		 */
		bool popError(std::string& location, std::string& message, bool* isNew = nullptr);

		/**
		 * Pops any first message available of a given type or of any type.<br>
		 * If several messages with different types are available and an undefined type is defined the pop order is: errors, warnings, informations.
		 * @param type The type of the message to pop, may be TYPE_UNDEFINED to return any message while 'type' will be set to the type of the message which is returned
		 * @param location The resulting location of the message
		 * @param message The resulting text message
		 * @param isNew Optional returning flag determining whether this message is not identical to the previous one
		 * @return True, if a message existed
		 */
		bool popMessage(MessageType& type, std::string& location, std::string& message, bool* isNew = nullptr);

		/**
		 * Pops the oldest message available of a specified type.
		 * @param type The type of the message to return, TYPE_UNDEFINED to return a message with any type
		 * @param isNew Optional resulting state determining whether the message is not identical to the previous one
		 * @return The resulting message with format "Type: Location, Message", an empty string if no message is available
		 */
		std::string popMessage(const MessageType type = TYPE_UNDEFINED, bool* isNew = nullptr);

		/**
		 * Sets the output type of the messenger.
		 * @param type Output type to set
		 * @return True, if succeeded
		 * @see setFileOutput(), setOutputStream().
		 */
		bool setOutputType(const MessageOutput type);

		/**
		 * Sets the message output to a file.
		 * All messages will be redirected to the specified file instead of inserted into the message queue.
		 * @param filename Name of the output file, use an empty filename to disable file output
		 * @return True, if succeeded
		 * @see setOutputType().
		 */
		bool setFileOutput(const std::string& filename);

		/**
		 * Sets the message output to an output stream.
		 * All messages will be redirected to the specified output stream instead of inserted into the message queue.
		 * @param stream Output stream
		 * @return True, if succeeded
		 * @see setOutputType().
		 */
		bool setOutputStream(std::ostream& stream);

		/**
		 * Enables or disables the integration of local date/time information into the location information of messages.
		 * @param state True, to enable the integration of the date/time information
		 * @see integrateDateTime().
		 */
		void setIntegrateDateTime(const bool state);

		/**
		 * Flushes the current message stack to a given output stream, the output type is unchanged.
		 * @param stream Output stream
		 */
		void flush(std::ostream& stream);

		/**
		 * Clears all message queues.
		 */
		void clear();

		/**
		 * Clears the information message queue.
		 */
		void clearInformations();

		/**
		 * Clears the warning message queue.
		 */
		void clearWarnings();

		/**
		 * Clears the error message queue.
		 */
		void clearErrors();

		/**
		 * Returns the output type of the messenger.
		 * @return Current output type
		 */
		inline MessageOutput outputType() const;

		/**
		 * Returns the number of waiting information messages.
		 * @return Number of messages
		 */
		inline size_t informations() const;

		/**
		 * Returns the number of waiting information messages.
		 * @return Number of messages
		 */
		inline size_t warnings() const;

		/**
		 * Returns the number of waiting information messages.
		 * @return Number of messages
		 */
		inline size_t errors() const;

		/**
		 * Returns whether the date/time integration is activated.
		 * By default the date time integration is deactivated.
		 * @return True, if so
		 * @see setIntegrateDateTime().
		 */
		inline bool integrateDateTime() const;

		/**
		 * Returns whether no message exists.
		 * @return True, if so
		 */
		inline bool empty() const;

		/**
		 * Writes a message to the most suitable debug output of the current platform.
		 * This function is just a simple way to create a debug message without needing to configure the Messenger.
		 * @param message The message to write
		 */
		static void writeToDebugOutput(const std::string& message);

		/**
		 * Returns whether the messenger is active on this build.
		 * @return True, if so; False, to strip away every message related information in the binary
		 */
		static constexpr bool isActive();

		/**
		 * Returns whether the messenger is used on a debug build.
		 * @return True, if so
		 */
		static constexpr bool isDebugBuild();

	protected:

		/**
		 * Creates a new messenger object queuing all messages as default.
		 */
		Messenger();

		/**
		 * Destructs a messenger object.
		 */
		~Messenger();

	protected:

		/// Message output type.
		MessageOutput outputType_ = OUTPUT_STANDARD;

#ifdef OCEAN_DEBUG
		/// Debug message queue.
		MessageQueue debugMessageQueue_;

		/// Last debug message.
		std::string lastDebugMessage_;
#endif

		/// Information message queue.
		MessageQueue informationMessageQueue_;

		/// Warning message queue.
		MessageQueue warningMessageQueue_;

		/// Error message queue.
		MessageQueue errorMessageQueue_;

		/// Last information message.
		std::string lastInformationMessage_;

		/// Last warning message.
		std::string lastWarningMessage_;

		/// Last error message.
		std::string lastErrorMessage_;

		/// File output stream
		std::ofstream fileOutputStream_;

		/// Explicit output stream, if any
		std::ostream* outputStream_ = nullptr;

		/// Date and time integration state.
		bool integrateDateTime_ = false;

		/// Messenger lock.
		mutable Lock lock_;

		/// Maximum number of messages.
		static constexpr unsigned int maxMessages_ = 5000u;

#if defined(__APPLE__)

		/**
		 * Write message to Apple-specific log facility.
		 * @sa push()
		 * @param message The message to be written into NSLog
		 */
		static void writeMessageToDebugWindowApple(const std::string& message);

#endif // defined(__APPLE__)
};

constexpr bool Messenger::isActive()
{
#ifdef OCEAN_DEACTIVATED_MESSENGER
	return false;
#else
	return true;
#endif
}

constexpr bool Messenger::isDebugBuild()
{
#ifdef OCEAN_DEBUG
	return true;
#else
	return false;
#endif
}

/**
 * Messenger object, one object for each message.
 * This class is a helper class only, therefore there is no need to use it directly.<br>
 * @tparam tActive True, if the message object is active; False, e.g., on builds in which messages must not appear in binary files
 * @see Messenger.
 * @ingroup base
 */
template <bool tActive>
class MessageObject
{
	public:

		/**
		 * Re-definition of MessageType
		 */
		using MessageType = Messenger::MessageType;

	public:

		/**
		 * Creates a new message object.
		 * @param type The type of the message object
		 */
		inline explicit MessageObject(const MessageType type);

		/**
		 * Creates a new message object.
		 * @param type The type of the message object
		 * @param location The location of the message
		 */
		inline MessageObject(const MessageType type, std::string&& location);

		/**
		 * Copy-constructs a new message object.
		 * @param messageObject Another instance that will be used to create this instance, must be valid
		 */
		MessageObject(const MessageObject& messageObject) = default;

		/**
		 * Destructs a message object.
		 */
		inline ~MessageObject();

		/**
		 * Adds a new line to this message object if a specified condition holds.
		 * @param condition True, to add the line; False, to do nothing
		 * @return Reference to this object
		 */
		inline MessageObject& newLine(const bool condition = true);

		/**
		 * Pushes another information message.
		 * @param message The message to push
		 * @return Reference to this object
		 */
		inline MessageObject& operator<<(std::string&& message);

		/**
		 * Pushes another information message.
		 * @param message The message to push
		 * @return Reference to this object
		 */
		inline MessageObject& operator<<(const std::string& message);

		/**
		 * Pushes another information message.
		 * @param message The message to push
		 * @return Reference to this object
		 */
		inline MessageObject& operator<<(const char* message);

		/**
		 * Pushes another information message.
		 * @param message The message to push
		 * @return Reference to this object
		 */
		inline MessageObject& operator<<(const double message);

		/**
		 * Pushes another information message.
		 * @param message The message to push
		 * @return Reference to this object
		 */
		inline MessageObject& operator<<(const float message);

		/**
		 * Pushes another information message.
		 * @param message The message to push
		 * @return Reference to this object
		 */
		inline MessageObject& operator<<(const int message);

		/**
		 * Pushes another information message.
		 * @param message The message to push
		 * @return Reference to this object
		 */
		inline MessageObject& operator<<(const unsigned int message);

		/**
		 * Pushes another information message.
		 * @param message The message to push
		 * @return Reference to this object
		 */
		inline MessageObject& operator<<(const long message);

		/**
		 * Pushes another information message.
		 * @param message The message to push
		 * @return Reference to this object
		 */
		inline MessageObject& operator<<(const long long message);

		/**
		 * Pushes another information message.
		 * @param message The message to push
		 * @return Reference to this object
		 */
		inline MessageObject& operator<<(const unsigned long long message);

		/**
		 * Pushes another information message.
		 * @param message The message to push
		 * @return Reference to this object
		 */
		inline MessageObject& operator<<(const long unsigned int message);

		/**
		 * Pushes another information message.
		 * @param message The message to push
		 */
		inline void operator<<(const MessageObject& message);

		/**
		 * Assign operator
		 * @param messageObject Another instance that will assigned to this instance, must be valid
		 */
		MessageObject& operator=(const MessageObject& messageObject) = default;

	protected:

		/**
		 * Creates a new message object.
		 * @param type The type of the message object
		 * @param location The location of the message object
		 * @param message The message of the object
		 */
		inline MessageObject(const MessageType type, std::string&& location, std::string&& message);

	protected:

		/// Entire message.
		std::string message_;

		/// Location of the message.
		std::string location_;

		/// Type of this messenger.
		Messenger::MessageType type_ = Messenger::TYPE_UNDEFINED;
};

/**
 * Specialization of MessageObject.
 * Inactive messenger object to strip away any messenger related information in binary.
 * @see Messenger.
 * @ingroup base
 */
template <>
class MessageObject<false>
{
	public:

		/**
		 * Re-definition of MessageType
		 */
		using MessageType = Messenger::MessageType;

	public:

		/**
		 * Creates a new message object.
		 */
		explicit inline MessageObject(const MessageType /*type*/)
		{
			// nothing to do here
		}

		/**
		 * Creates a new message object.
		 */
		inline MessageObject(const MessageType /*type*/, std::string&& /*location*/)
		{
			// nothing to do here
		}

		/**
		 * Copy-constructs a new message object.
		 * @param messageObject Another instance that will be used to create this instance, must be valid
		 */
		MessageObject(const MessageObject& messageObject) = default;

		/**
		 * Destructs a message object.
		 */
		~MessageObject() = default;

		/**
		 * Adds a new line to this message object if a specified condition holds.
		 * @return Reference to this object
		 */
		inline MessageObject& newLine()
		{
			return *this;
		}

		/**
		 * Adds a new line to this message object if a specified condition holds.
		 * @return Reference to this object
		 */
		inline MessageObject& newLine(const bool /*condition*/)
		{
			return *this;
		}

		/**
		 * Pushes another information message.
		 * @return Reference to this object
		 */
		inline MessageObject& operator<<(std::string&& /*message*/)
		{
			return *this;
		}

		/**
		 * Pushes another information message.
		 * @return Reference to this object
		 */
		inline MessageObject& operator<<(const std::string& /*message*/)
		{
			return *this;
		}

		/**
		 * Pushes another information message.
		 * @return Reference to this object
		 */
		inline MessageObject& operator<<(const char* /*message*/)
		{
			return *this;
		}

		/**
		 * Pushes another information message.
		 * @return Reference to this object
		 */
		inline MessageObject& operator<<(const double /*message*/)
		{
			return *this;
		}

		/**
		 * Pushes another information message.
		 * @return Reference to this object
		 */
		inline MessageObject& operator<<(const float /*message*/)
		{
			return *this;
		}

		/**
		 * Pushes another information message.
		 * @return Reference to this object
		 */
		inline MessageObject& operator<<(const int /*message*/)
		{
			return *this;
		}

		/**
		 * Pushes another information message.
		 * @return Reference to this object
		 */
		inline MessageObject& operator<<(const unsigned int /*message*/)
		{
			return *this;
		}

		/**
		 * Pushes another information message.
		 * @return Reference to this object
		 */
		inline MessageObject& operator<<(const long /*message*/)
		{
			return *this;
		}

		/**
		 * Pushes another information message.
		 * @return Reference to this object
		 */
		inline MessageObject& operator<<(const long long /*message*/)
		{
			return *this;
		}

		/**
		 * Pushes another information message.
		 * @return Reference to this object
		 */
		inline MessageObject& operator<<(const unsigned long long /*message*/)
		{
			return *this;
		}

		/**
		 * Pushes another information message.
		 * @return Reference to this object
		 */
		inline MessageObject& operator<<(const long unsigned int /*message*/)
		{
			return *this;
		}

		/**
		 * Pushes another information message.
		 */
		inline void operator<<(const MessageObject& /*message*/)
		{
			// nothing to do here
		}

		/**
		 * Assign operator
		 * @param messageObject Another instance that will assigned to this instance, must be valid
		 */
		MessageObject& operator=(const MessageObject& messageObject) = default;

	protected:

		/**
		 * Creates a new message object.
		 */
		inline MessageObject(const MessageType /*type*/, std::string&& /*location*/, std::string&& /*message*/)
		{
			// nothing to do here
		}
};

/**
 * This class provides access to three different Message objects, e.g., for regular information, warnings, or errors.
 * @ingroup base
 */
class OCEAN_BASE_EXPORT Log
{
	public:

		/**
		 * Definition of a default message object, only active if `Messenger::isActive() == true`.
		 */
		typedef Ocean::MessageObject<Messenger::isActive()> MessageObject;

		/**
		 * Definition of a debug message object, only active on debug builds and if `Messenger::isActive() == true`.
		 */
		typedef Ocean::MessageObject<Messenger::isActive() && Messenger::isDebugBuild()> DebugMessageObject;

	protected:

		/**
		 * Helper class allowing to specify a specific message object or stream type.
		 * @tparam TStream The data type of the message object (or stream) to be checked
		 */
		template <typename TStream>
		struct StreamHelper
		{
			/**
			 * Helper class to check whether a message object or a stream provides a shift operator for an object with specific data type.
			 * @tparam TObject The data type to check
			 */
			template <typename TObject, typename = void>
			struct ShiftOperatorChecker
			{
				/// False, if the message object or stream does not provide a shift operator for the data type 'TObject'.
				static constexpr bool value = false;
			};

			/**
			 * Helper class to check whether a message object or a stream provides a shift operator for an object with specific data type.
			 * Specialization of ShiftOperatorChecker.
			 * @tparam TObject The data type to check
			 */
			template <typename TObject>
			struct ShiftOperatorChecker<TObject, std::void_t<decltype( std::declval<TStream&>() << std::declval<TObject>())>>
			{
				/// True, if the message object or stream does provides a shift operator for the data type 'TObject'.
				static constexpr bool value = true;
			};
		};

	public:

		/**
		 * Returns the message for debug messages.
		 * Debug messages do not show up on release builds.<br>
		 * The debug log is intended to simplify code like this:
		 * @code
		 * #ifdef OCEAN_DEBUG
		 *     info() << "<debug> This message shows up on debug builds only."
		 * #endif
		 * @endcode
		 * @return The message object that will be stripped away on release builds
		 */
		static inline DebugMessageObject debug();

		/**
		 * Returns the message for information messages.
		 * @return The message object
		 */
		static inline MessageObject info();

		/**
		 * Returns the message for warning messages.
		 * @return The message object
		 */
		static inline MessageObject warning();

		/**
		 * Returns the message for error messages.
		 * @return The message object
		 */
		static inline MessageObject error();

		/**
		 * Returns whether a specific data type can be written to a message object (or an arbitrary stream).
		 * The function returns whether the shift operator of MessageObject (or of an arbitrary stream) supports the specified data type.
		 * Usage:
		 * @code
		 * void function()
		 * {
		 *     int value = 9;
		 *
		 *     if constexpr (Log::isSupported<int>())
		 *     {
		 *         Log::info() << "Value is: " << value;
		 *     }
		 *     else
		 *     {
		 *         Log::info() << "Value cannot be written to log.";
		 *     }
		 * }
		 * @endcode
		 * @return True, if so
		 * @tparam T The data type to be checked
		 * @tparam TStream The data type of the message object (or stream) to be checked
		 */
		template <typename T, typename TStream = MessageObject>
		static constexpr bool isSupported();
};

inline Messenger::MessageOutput Messenger::outputType() const
{
	const ScopedLock scopedLock(lock_);
	return outputType_;
}

inline size_t Messenger::informations() const
{
	const ScopedLock scopedLock(lock_);
	return informationMessageQueue_.size();
}

inline size_t Messenger::warnings() const
{
	const ScopedLock scopedLock(lock_);
	return warningMessageQueue_.size();
}

inline size_t Messenger::errors() const
{
	const ScopedLock scopedLock(lock_);
	return errorMessageQueue_.size();
}

inline bool Messenger::integrateDateTime() const
{
	const ScopedLock scopedLock(lock_);
	return integrateDateTime_;
}

inline bool Messenger::empty() const
{
	const ScopedLock scopedLock(lock_);
	return informationMessageQueue_.empty() && warningMessageQueue_.empty() && errorMessageQueue_.empty();
}

template <bool tActive>
inline MessageObject<tActive>::MessageObject(const MessageType type) :
	type_(type)
{
	// nothing to do here
}

template <bool tActive>
inline MessageObject<tActive>::MessageObject(const MessageType type, std::string&& location) :
	location_(std::move(location)),
	type_(type)
{
	// nothing to do here
}

template <bool tActive>
inline MessageObject<tActive>::MessageObject(const MessageType type, std::string&& location, std::string&& message) :
	message_(std::move(message)),
	location_(std::move(location)),
	type_(type)
{
	// nothing to do here
}

template <bool tActive>
inline MessageObject<tActive>::~MessageObject()
{
	if (!message_.empty())
	{
		Messenger::get().push(type_, std::move(location_), std::move(message_));
	}
}

template <bool tActive>
inline MessageObject<tActive>& MessageObject<tActive>::newLine(const bool condition)
{
	if (condition)
	{
		message_ += "\n";
	}

	return *this;
}

template <bool tActive>
inline MessageObject<tActive>& MessageObject<tActive>::operator<<(std::string&& message)
{
	if (message_.empty())
	{
		message_ = std::move(message);
	}
	else
	{
		message_ += message;
	}

	return *this;
}

template <bool tActive>
inline MessageObject<tActive>& MessageObject<tActive>::operator<<(const std::string& message)
{
	message_ += message;
	return *this;
}

template <bool tActive>
inline MessageObject<tActive>& MessageObject<tActive>::operator<<(const char* message)
{
	if (message != nullptr)
	{
		message_ += std::string(message);
	}

	return *this;
}

template <bool tActive>
inline MessageObject<tActive>& MessageObject<tActive>::operator<<(const double message)
{
	message_ += String::toAString(message);
	return *this;
}

template <bool tActive>
inline MessageObject<tActive>& MessageObject<tActive>::operator<<(const float message)
{
	message_ += String::toAString(message);
	return *this;
}

template <bool tActive>
inline MessageObject<tActive>& MessageObject<tActive>::operator<<(const int message)
{
	message_ += String::toAString(message);
	return *this;
}

template <bool tActive>
inline MessageObject<tActive>& MessageObject<tActive>::operator<<(const unsigned int message)
{
	message_ += String::toAString(message);
	return *this;
}

template <bool tActive>
inline MessageObject<tActive>& MessageObject<tActive>::operator<<(const long message)
{
	message_ += String::toAString(message);
	return *this;
}

template <bool tActive>
inline MessageObject<tActive>& MessageObject<tActive>::operator<<(const long long message)
{
	message_ += String::toAString(message);
	return *this;
}

template <bool tActive>
inline MessageObject<tActive>& MessageObject<tActive>::operator<<(const unsigned long long message)
{
	message_ += String::toAString(message);
	return *this;
}

template <bool tActive>
inline MessageObject<tActive>& MessageObject<tActive>::operator<<(const long unsigned int message)
{
	message_ += String::toAString(message);
	return *this;
}

template <bool tActive>
inline void MessageObject<tActive>::operator<<(const MessageObject<tActive>& messageObject)
{
	ocean_assert_and_suppress_unused(messageObject.type_ == Messenger::TYPE_UNDEFINED, messageObject);

	if (!message_.empty())
	{
		Messenger::get().push(type_, std::string(location_), std::move(message_));
	}

	message_.clear();
}

inline Log::DebugMessageObject Log::debug()
{
	return DebugMessageObject(Messenger::TYPE_DEBUG);
}

inline Log::MessageObject Log::info()
{
	return MessageObject(Messenger::TYPE_INFORMATION);
}

inline Log::MessageObject Log::warning()
{
	return MessageObject(Messenger::TYPE_WARNING);
}

inline Log::MessageObject Log::error()
{
	return MessageObject(Messenger::TYPE_ERROR);
}

template <typename T, typename TStream>
constexpr bool Log::isSupported()
{
	return StreamHelper<TStream>::template ShiftOperatorChecker<T>::value;
}

}

#endif // META_OCEAN_BASE_MESSENGER_H
