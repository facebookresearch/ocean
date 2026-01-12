/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_IO_SERIALIZATION_INPUT_DATA_SERIALIZER_H
#define META_OCEAN_IO_SERIALIZATION_INPUT_DATA_SERIALIZER_H

#include "ocean/io/serialization/Serialization.h"
#include "ocean/io/serialization/DataSerializer.h"

#include <functional>

namespace Ocean
{

namespace IO
{

namespace Serialization
{

/**
 * This class implements an input data serializer.
 * The input data serializer deserializes data samples from a stream (e.g., file or network) and provides playback functionality with configurable speed.<br>
 * Before starting playback, factory functions must be registered for each expected sample type so that the serializer can construct the appropriate sample objects when reading from the stream.<br>
 * Samples for which no factory function is registered are simply skipped during playback.<br>
 * Samples are returned through the sample() function in playback order, with optional speed control for real-time or accelerated playback.<br>
 * The class uses a background thread to continuously read and buffer samples, ensuring smooth playback without blocking.
 * @ingroup ioserialization
 */
class InputDataSerializer : public DataSerializer
{
	public:

		/// Definition of a factory function which creates a data sample based on a sample type.
		using FactoryFunction = std::function<UniqueDataSample(const std::string& sampleType)>;

		/**
		 * Definition of a callback function which is invoked whenever a new channel is parsed.
		 * @param channel The channel information
		 */
		using ChannelEventFunction = std::function<void(const Channel& channel)>;

	protected:

		/**
		 * This class implements an abstract stream for input data serializers.
		 */
		class Stream
		{
			public:

				/**
				 * Destructs the stream.
				 */
				virtual ~Stream() = default;

				/**
				 * Returns the input bitstream.
				 * @return The input bitstream
				 */
				virtual InputBitstream& inputBitstream() = 0;

				/**
				 * Returns whether this stream is valid.
				 * @return True, if so
				 */
				virtual bool isValid() const = 0;
		};

		/// Definition of a unique pointer holding a stream.
		using UniqueStream = std::unique_ptr<Stream>;

		/// Definition of a map mapping sample types to factory functions.
		using FactoryFunctionMap = std::unordered_map<std::string, FactoryFunction>;

		/**
		 * This class implements an extended channel with factory function.
		 */
		class ExtendedChannel : public Channel
		{
			public:

				/**
				 * Creates a new invalid extended channel.
				 */
				ExtendedChannel() = default;

				/**
				 * Creates a new extended channel with given channel and factory function.
				 * @param channel The channel
				 * @param factoryFunction The factory function
				 */
				inline ExtendedChannel(const Channel& channel, const FactoryFunction& factoryFunction);

			public:

				/// The factory function for creating data samples.
				FactoryFunction factoryFunction_;
		};

		/// Definition of a map mapping channel ids to extended channels.
		using ExtendedChannelMap = std::unordered_map<ChannelId, ExtendedChannel>;

		/**
		 * Comparator for SamplePair that orders by playback timestamp (min-heap).
		 * Samples with smaller playback timestamps have higher priority.
		 */
		struct SamplePairComparator
		{
			/**
			 * Compares two sample pairs by their playback timestamps.
			 * @param sampleA The first sample pair
			 * @param sampleB The second sample pair
			 * @return True if 'sampleA' should come after 'sampleB' (i.e., 'sampleA' has a larger timestamp)
			 */
			inline bool operator()(const SamplePair& sampleA, const SamplePair& sampleB) const;
		};

		/**
		 * A priority queue that allows moving elements out.
		 * This extends std::priority_queue to provide access to the underlying container
		 * for proper move semantics when popping elements.
		 */
		class SampleQueue : public std::priority_queue<SamplePair, std::vector<SamplePair>, SamplePairComparator>
		{
			public:

				/**
				 * Pops the top element from the queue.
				 * @return The top element, moved out of the queue
				 */
				inline SamplePair popTop();
		};

	public:

		/**
		 * Initializes the input data serializer.
		 * The serializer will create the input stream and read the header.
		 * Optionally, the serializer can pre-parse all channels before returning.
		 * @param preparsedChannels Optional output parameter to receive all channels that were pre-parsed, nullptr if not of interest
		 * @param isStreamCorrupted Optional output parameter to receive whether the stream is corrupted (missing end-of-stream indication), nullptr if not of interest
		 * @return True, if succeeded
		 */
		bool initialize(Channels* preparsedChannels = nullptr, bool* isStreamCorrupted = nullptr);

		/**
		 * Starts the serializer.
		 * @return True, if succeeded
		 * @see DataSerializer::start().
		 */
		bool start() override;

		/**
		 * Stops the serializer.
		 * This function sets a stopping flag and returns immediately; it does not wait for the serializer to actually stop.
		 * The background thread will check the stopping flag at the beginning of each iteration and terminate.
		 * @return True, if the stop request was accepted; False, if the serializer was not started
		 * @see DataSerializer::stop(), stopAndWait(), hasStopped().
		 */
		bool stop() override;

		/**
		 * Returns whether the serializer has been started.
		 * @return True, if so
		 * @see DataSerializer::isStarted().
		 */
		[[nodiscard]] bool isStarted() const override;

		/**
		 * Returns whether the serializer has stopped and all remaining samples have been retrieved.
		 * @return True, if so
		 * @see DataSerializer::hasFinished().
		 */
		[[nodiscard]] bool hasFinished() const override;

		/**
		 * Registers a factory function for a given sample type.
		 * The factory function will be used to create data samples when reading from the stream.
		 * @param sampleType The sample type for which the factory function will be registered, must be non-empty
		 * @param factoryFunction The factory function to register, must be valid
		 * @return True, if succeeded
		 */
		bool registerFactoryFunction(const std::string& sampleType, const FactoryFunction& factoryFunction);

		/**
		 * Registers a factory function for a sample type T.
		 * The sample type T must provide static functions `sampleType()` and `createSample()`.
		 * This is a convenience function that calls `registerFactoryFunction(T::sampleType(), T::createSample)`.
		 * @tparam T The sample type which must provide `static const std::string& sampleType()` and `static UniqueDataSample createSample(const std::string&)`
		 * @return True, if succeeded
		 */
		template <typename T>
		bool registerSample();

		/**
		 * Registers a callback function that will be invoked whenever a new channel is parsed.
		 * @param channelEventFunction The callback function to be invoked, must be valid
		 * @return True if the callback was successfully registered, false otherwise
		 */
		bool registerChannelEventFunction(const ChannelEventFunction& channelEventFunction);

		/**
		 * Returns the next sample from the stream.
		 * The sample will be removed from the internal queue and returned to the caller.
		 * @param channelId The resulting channel id of the sample
		 * @param speed The playback speed, with range (0, infinity), 0 to return samples as fast as possible, 1 for real-time playback
		 * @return The sample, nullptr if no sample is available yet or if the stream has ended
		 */
		[[nodiscard]] UniqueDataSample sample(ChannelId& channelId, const double speed = 1.0);

		/**
		 * Returns the channel information for a given channel.
		 * @param channelId The channel id to query
		 * @return The channel information, an invalid channel if the channel doesn't exist
		 */
		[[nodiscard]] ChannelConfiguration channelConfiguration(const ChannelId channelId) const;

		/**
		 * Returns all channels that have been parsed so far.
		 * @return The channels
		 */
		[[nodiscard]] Channels channels() const;

	protected:

		/**
		 * Creates the input stream.
		 * @return The input stream, nullptr if the stream could not be created
		 */
		virtual UniqueStream createStream() const = 0;

		/**
		 * Reads the header from the input bitstream.
		 * @param inputBitstream The input bitstream from which the header will be read
		 * @return True, if succeeded
		 */
		virtual bool readHeader(InputBitstream& inputBitstream);

		/**
		 * The thread run function.
		 * @see Thread::threadRun().
		 */
		void threadRun() override;

	protected:

		/// The input stream.
		UniqueStream stream_;

		/// The map mapping sample types to factory functions.
		FactoryFunctionMap factoryFunctionMap_;

		/// The map mapping channel ids to extended channels (with factory function).
		ExtendedChannelMap extendedChannelMap_;

		/// The callback function which is invoked whenever a new channel is parsed.
		ChannelEventFunction channelEventFunction_;

		/// The priority queue holding samples which are pending to be retrieved, ordered by playback timestamp (smallest first).
		SampleQueue sampleQueue_;

		/// The maximum number of pending samples in the queue.
		static constexpr size_t maxPendingSampleQueueSize_ = 100;
};

/**
 * This class implements a file-based input data serializer.
 * This specialization of InputDataSerializer reads serialized data from a binary file on disk.<br>
 * Use setFilename() to configure the input file path before calling initialize() and start().
 * @ingroup ioserialization
 */
class FileInputDataSerializer : public InputDataSerializer
{
	protected:

		/**
		 * This class implements a file stream for file input data serializers.
		 */
		class FileStream : public Stream
		{
			public:

				/**
				 * Creates a new file stream with given filename.
				 * @param filename The filename of the file to read, must be valid
				 */
				inline explicit FileStream(const std::string& filename);

				/**
				 * Destructs the file stream.
				 */
				inline ~FileStream() override;

				/**
				 * Returns the input bitstream.
				 * @return The input bitstream
				 * @see Stream::inputBitstream().
				 */
				inline InputBitstream& inputBitstream() override;

				/**
				 * Returns whether this stream is valid.
				 * @return True, if so
				 * @see Stream::isValid().
				 */
				inline bool isValid() const override;

			protected:

				/// The file stream.
				std::ifstream stream_;

				/// The input bitstream.
				InputBitstream inputBitstream_;
		};

	public:

		/**
		 * Sets the filename of the file to read.
		 * @param filename The filename of the file to read, must be non-empty
		 * @return True, if succeeded
		 */
		virtual bool setFilename(const std::string& filename);

	protected:

		/**
		 * Creates the input stream.
		 * @return The input stream, nullptr if the stream could not be created
		 * @see InputDataSerializer::createStream().
		 */
		UniqueStream createStream() const override;

	protected:

		/// The filename of the file to read.
		std::string filename_;
};

inline InputDataSerializer::ExtendedChannel::ExtendedChannel(const Channel& channel, const FactoryFunction& factoryFunction) :
	Channel(channel),
	factoryFunction_(factoryFunction)
{
	// nothing to do here
}

inline bool InputDataSerializer::SamplePairComparator::operator()(const SamplePair& sampleA, const SamplePair& sampleB) const
{
	return sampleA.second->playbackTimestamp() > sampleB.second->playbackTimestamp();
}

inline InputDataSerializer::SamplePair InputDataSerializer::SampleQueue::popTop()
{
	ocean_assert(!empty());

	std::pop_heap(c.begin(), c.end(), comp);

	SamplePair result = std::move(c.back());
	c.pop_back();

	return result;
}

inline FileInputDataSerializer::FileStream::FileStream(const std::string& filename) :
	stream_(filename.c_str(), std::ios::binary),
	inputBitstream_(stream_)

{
	// nothing to do here
}

inline FileInputDataSerializer::FileStream::~FileStream()
{
	stream_.close();
}

inline InputBitstream& FileInputDataSerializer::FileStream::inputBitstream()
{
	return inputBitstream_;
}

inline bool FileInputDataSerializer::FileStream::isValid() const
{
	return stream_.is_open() && !stream_.fail();
}

template <typename T>
bool InputDataSerializer::registerSample()
{
	return registerFactoryFunction(T::sampleType(), T::createSample);
}

}

}

}

#endif // META_OCEAN_IO_SERIALIZATION_INPUT_DATA_SERIALIZER_H
