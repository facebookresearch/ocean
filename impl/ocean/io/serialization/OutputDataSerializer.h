/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_IO_SERIALIZATION_OUTPUT_DATA_SERIALIZER_H
#define META_OCEAN_IO_SERIALIZATION_OUTPUT_DATA_SERIALIZER_H

#include "ocean/io/serialization/Serialization.h"
#include "ocean/io/serialization/DataSerializer.h"

namespace Ocean
{

namespace IO
{

namespace Serialization
{

/**
 * This class implements an output data serializer.
 * The output data serializer serializes data samples to a stream (e.g., file or network) for recording purposes.<br>
 * Before adding samples, channels must be created using addChannel() which assigns a unique channel id for each distinct sample type, name, and content type combination.<br>
 * Samples are added via addSample() and are written to the stream asynchronously by a background thread, allowing the caller to continue without blocking.<br>
 * When stopping, all queued samples are written before the serializer terminates, ensuring no data is lost.
 * @ingroup ioserialization
 */
class OCEAN_IO_SERIALIZATION_EXPORT OutputDataSerializer : public DataSerializer
{
	protected:

		/**
		 * This class implements an abstract stream for output data serializers.
		 */
		class Stream
		{
			public:

				/**
				 * Destructs the stream.
				 */
				virtual ~Stream() = default;

				/**
				 * Returns the output bitstream.
				 * @return The output bitstream
				 */
				virtual OutputBitstream& outputBitstream() = 0;

				/**
				 * Returns whether this stream is valid.
				 * @return True, if so
				 */
				virtual bool isValid() const = 0;
		};

		/// Definition of a unique pointer holding a stream.
		using UniqueStream = std::unique_ptr<Stream>;

		/// Definition of a FIFO queue holding sample pairs.
		using SampleQueue = std::queue<SamplePair>;

	public:

		/**
		 * Adds a new channel to the serializer.
		 * @param sampleType The sample type, must be non-empty
		 * @param name The name of the channel, must be non-empty
		 * @param contentType The content type, must be non-empty
		 * @return The channel id, invalidChannelId() if the channel could not be added (e.g., if a channel with the same configuration already exists)
		 */
		[[nodiscard]] DataSerializer::ChannelId addChannel(const std::string& sampleType, const std::string& name, const std::string& contentType);

		/**
		 * Adds a new sample to the serializer.
		 * The sample will be added to the internal queue and will be written to the stream asynchronously.
		 * @param channelId The channel id
		 * @param sample The sample to add, will be moved
		 * @return True, if succeeded
		 */
		bool addSample(const DataSerializer::ChannelId channelId, UniqueDataSample&& sample);

		/**
		 * Starts the serializer.
		 * @return True, if succeeded
		 * @see DataSerializer::start().
		 */
		bool start() override;

		/**
		 * Stops the serializer.
		 * This function sets a stopping flag and returns immediately; it does not wait for the serializer to actually stop.
		 * The background thread will finish writing all pending samples in the queue before checking the stopping flag and terminating.
		 * This ensures that all queued samples are written to the output stream before the serializer stops.
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
		 * Returns whether the serializer has stopped and all remaining samples have been processed.
		 * @return True, if so
		 * @see DataSerializer::hasFinished().
		 */
		[[nodiscard]] bool hasFinished() const override;

	protected:

		/**
		 * Creates the output stream.
		 * @return The output stream, nullptr if the stream could not be created
		 */
		virtual UniqueStream createStream() const = 0;

		/**
		 * Writes the header to the output bitstream.
		 * @param outputBitstream The output bitstream to which the header will be written
		 * @return True, if succeeded
		 */
		virtual bool writeHeader(OutputBitstream& outputBitstream);

		/**
		 * The thread run function.
		 * @see Thread::threadRun().
		 */
		void threadRun() override;

	protected:

		/// The output stream.
		UniqueStream stream_;

		/// The next channel id to be assigned.
		ChannelId nextChannelId_ = ChannelId(0);

		/// The map mapping channel configurations to channel ids.
		ChannelConfigurationMap channelConfigurationMap_;

		/// The queue holding samples which are pending to be written.
		SampleQueue sampleQueue_;
};

/**
 * This class implements a file-based output data serializer.
 * This specialization of OutputDataSerializer writes serialized data to a binary file on disk.<br>
 * Use setFilename() to configure the output file path before calling start().
 * @ingroup ioserialization
 */
class OCEAN_IO_SERIALIZATION_EXPORT FileOutputDataSerializer : public OutputDataSerializer
{
	protected:

		/**
		 * This class implements a file stream for file output data serializers.
		 */
		class FileStream : public Stream
		{
			public:

				/**
				 * Creates a new file stream with given filename.
				 * @param filename The filename of the file to write, must be valid
				 */
				inline explicit FileStream(const std::string& filename);

				/**
				 * Destructs the file stream.
				 */
				inline ~FileStream() override;

				/**
				 * Returns the output bitstream.
				 * @return The output bitstream
				 * @see Stream::outputBitstream().
				 */
				inline OutputBitstream& outputBitstream() override;

				/**
				 * Returns whether this stream is valid.
				 * @return True, if so
				 * @see Stream::isValid().
				 */
				inline bool isValid() const override;

			protected:

				/// The file stream.
				std::ofstream stream_;

				/// The output bitstream.
				OutputBitstream outputBitstream_;
		};

	public:

		/**
		 * Sets the filename of the file to write.
		 * @param filename The filename of the file to write, must be non-empty
		 * @return True, if succeeded
		 */
		virtual bool setFilename(const std::string& filename);

	protected:

		/**
		 * Creates the output stream.
		 * @return The output stream, nullptr if the stream could not be created
		 * @see OutputDataSerializer::createStream().
		 */
		UniqueStream createStream() const override;

	protected:

		/// The filename of the file to write.
		std::string filename_;
};

inline FileOutputDataSerializer::FileStream::FileStream(const std::string& filename) :
	stream_(filename.c_str(), std::ios::binary),
	outputBitstream_(stream_)

{
	// nothing to do here
}

inline FileOutputDataSerializer::FileStream::~FileStream()
{
	stream_.close();
}

inline OutputBitstream& FileOutputDataSerializer::FileStream::outputBitstream()
{
	return outputBitstream_;
}

inline bool FileOutputDataSerializer::FileStream::isValid() const
{
	return stream_.is_open() && !stream_.fail();
}

}

}

}

#endif // META_OCEAN_IO_SERIALIZATION_OUTPUT_DATA_SERIALIZER_H
