/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/CommandArguments.h"
#include "ocean/base/Messenger.h"
#include "ocean/base/Thread.h"

#include "ocean/io/File.h"

#include "ocean/io/serialization/InputDataSerializer.h"

using namespace Ocean;

/**
 * This class collects channel information from the serialization file.
 * The class is thread-safe and can be used as a callback handler during serialization parsing.
 * Channels are stored internally and can be retrieved via the channels() method.
 */
class ChannelCollector
{
	public:

		/**
		 * Event function that is called when a new channel is parsed.
		 * @param channel The channel information to be collected
		 */
		void onNewChannel(const IO::Serialization::DataSerializer::Channel& channel)
		{
			const ScopedLock scopedLock(lock_);

			channels_.push_back(channel);
		}

		/**
		 * Returns the collected channels.
		 * @return The collected channels
		 */
		IO::Serialization::DataSerializer::Channels channels() const
		{
			const ScopedLock scopedLock(lock_);

			return channels_;
		}

	protected:

		/// The collected channels.
		IO::Serialization::DataSerializer::Channels channels_;

		/// The lock.
		mutable Lock lock_;
};

#if defined(_WINDOWS)
	// main function on Windows platforms
	int wmain(int argc, wchar_t* argv[])
#elif defined(__APPLE__) || defined(__linux__)
	// main function on macOS and Linux platforms
	int main(int argc, char* argv[])
#else
	#error Missing implementation.
#endif
{
#ifdef OCEAN_COMPILER_MSC
	// prevent the debugger to abort the application after an assert has been caught
	_set_error_mode(_OUT_TO_MSGBOX);
#endif

	Messenger::get().setOutputType(Messenger::OUTPUT_STANDARD);

	CommandArguments commandArguments;
	commandArguments.registerParameter("input", "i", "The input serialization file to inspect");
	commandArguments.registerParameter("help", "h", "Show this help output");

	commandArguments.parse(argv, size_t(argc));

	if (commandArguments.hasValue("help", nullptr, false))
	{
		Log::info() << "Ocean Serialization Inspector Demo:";
		Log::info() << " ";
		Log::info() << commandArguments.makeSummary();

		return 0;
	}

	const std::string inputValue = commandArguments.value("input", std::string(), false, 0);

	if (inputValue.empty())
	{
		Log::error() << "Need 'input' parameter";
		return 1;
	}

	const IO::File file(inputValue);

	if (!file.exists())
	{
		Log::error() << "The input file does not exist: '" << file() << "'";
		return 1;
	}

	Log::info() << "Opening serialization file: '" << file() << "'";
	Log::info() << " ";

	IO::Serialization::FileInputDataSerializer serializer;

	if (!serializer.setFilename(file()))
	{
		Log::error() << "Failed to set the filename";
		return 1;
	}

	IO::Serialization::DataSerializer::Channels channels;
	if (!serializer.initialize(&channels))
	{
		Log::error() << "Failed to initialize the serializer";
		return 1;
	}

	Log::info() << "Total channels found: " << channels.size();
	Log::info() << " ";

	for (size_t n = 0; n < channels.size(); ++n)
	{
		const IO::Serialization::DataSerializer::Channel& channel = channels[n];

		Log::info() << "Channel #" << (n + 1) << ":";
		Log::info() << "  Channel id:   " << channel.channelId();
		Log::info() << "  Sample type:  " << channel.sampleType();
		Log::info() << "  Name:         " << channel.name();
		Log::info() << "  Content Type: " << channel.contentType();
		Log::info() << " ";
	}

	if (!commandArguments.hasValue("input"))
	{
		// The input file was specified as a nameless argument (e.g., "inspector file.txt") rather than a named argument (e.g., "inspector --input file.txt").
		// In this case, wait for a key press before exiting to allow the user to view the output.

		Log::info() << "Press a key to exit.";
		getchar();
	}

	return 0;
}
