/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_IO_SERIALIZATION_DATA_SERIALIZER_H
#define META_OCEAN_IO_SERIALIZATION_DATA_SERIALIZER_H

#include "ocean/io/serialization/Serialization.h"

#include "ocean/base/Timestamp.h"
#include "ocean/base/Thread.h"

#include "ocean/io/Bitstream.h"
#include "ocean/io/serialization/DataTimestamp.h"
#include "ocean/io/serialization/DataSample.h"

namespace Ocean
{

namespace IO
{

namespace Serialization
{

/**
 * This class implements the base class for data serializers.
 * Data serializers are used to serialize and deserialize data samples across multiple channels.<br>
 * Each channel can have a different sample type, name, and content type, allowing heterogeneous data to be organized and streamed together.<br>
 * The class provides a thread-based architecture where the actual serialization/deserialization happens asynchronously in a background thread.<br>
 * Derived classes (InputDataSerializer and OutputDataSerializer) implement the specific read or write functionality.
 * @ingroup ioserialization
 */
class OCEAN_IO_SERIALIZATION_EXPORT DataSerializer : protected Thread
{
	public:

		/// Definition of a channel id.
		using ChannelId = uint32_t;

		/**
		 * Returns an invalid channel id.
		 * @return The invalid channel id
		 */
		static constexpr ChannelId invalidChannelId();

		/**
		 * This class holds channel configuration (sample type, name, and content type).
		 * A channel configuration uniquely identifies the type and purpose of a data channel without including the runtime channel id.<br>
		 * The sample type describes the class type of data samples (e.g., "DataSampleFrame"), the name provides a user-friendly identifier (e.g., "camera_left"), and the content type describes the semantic meaning of the data (e.g., "RGB_FRAME").
		 */
		class ChannelConfiguration
		{
			public:

				/**
				 * Definition of a hash function for ChannelConfiguration objects.
				 */
				struct Hash
				{
					/**
					 * Calculates the hash value for a given channel configuration.
					 * @param channelConfiguration The channel configuration for which the hash will be calculated
					 * @return The hash value
					 */
					inline size_t operator()(const ChannelConfiguration& channelConfiguration) const;
				};

			public:

				/**
				 * Creates a new invalid channel configuration.
				 */
				ChannelConfiguration() = default;

				/**
				 * Creates a new channel configuration with given sample type, name, and content type.
				 * @param sampleType The sample type
				 * @param name The name of the channel
				 * @param contentType The content type
				 */
				inline ChannelConfiguration(const std::string& sampleType, const std::string& name, const std::string& contentType);

				/**
				 * Returns the sample type.
				 * @return The sample type
				 */
				inline const std::string& sampleType() const;

				/**
				 * Returns the name of the channel.
				 * @return The channel name
				 */
				inline const std::string& name() const;

				/**
				 * Returns the content type.
				 * @return The content type
				 */
				inline const std::string& contentType() const;

				/**
				 * Returns whether this channel configuration is valid.
				 * @return True, if all fields (sample type, name, and content type) are non-empty
				 */
				inline bool isValid() const;

				/**
				 * Returns whether two channel configurations are equal.
				 * @param channelConfiguration The second channel configuration to compare
				 * @return True, if both configurations are equal
				 */
				inline bool operator==(const ChannelConfiguration& channelConfiguration) const;

			protected:

				/// The sample type.
				std::string sampleType_;

				/// The name of the channel.
				std::string name_;

				/// The content type.
				std::string contentType_;
		};

		/**
		 * This class implements a channel with configuration and channel id.
		 * A channel extends ChannelConfiguration by adding a unique channel id that is assigned at runtime during serialization.<br>
		 * The channel id is used to efficiently identify and route data samples within the serialization stream.
		 */
		class OCEAN_IO_SERIALIZATION_EXPORT Channel : public ChannelConfiguration
		{
			public:

				/**
				 * Definition of a hash function for Channel objects.
				 */
				struct Hash
				{
					/**
					 * Calculates the hash value for a given channel.
					 * @param channel The channel for which the hash will be calculated
					 * @return The hash value
					 */
					inline size_t operator()(const Channel& channel) const;
				};

			public:

				/**
				 * Creates a new invalid channel.
				 */
				Channel() = default;

				/**
				 * Creates a new channel with given configuration and channel id.
				 * @param channelConfiguration The channel configuration
				 * @param channelId The channel id
				 */
				inline Channel(const ChannelConfiguration& channelConfiguration, const ChannelId channelId);

				/**
				 * Returns the channel id.
				 * @return The channel id
				 */
				inline ChannelId channelId() const;

				/**
				 * Returns whether this channel is valid.
				 * @return True, if the channel id is valid and the configuration is valid
				 */
				inline bool isValid() const;

				/**
				 * Returns whether two channels are equal.
				 * @param channel The second channel to compare
				 * @return True, if both channels are equal
				 */
				inline bool operator==(const Channel& channel) const;

			protected:

				/// The channel id.
				ChannelId channelId_ = invalidChannelId();
		};

		/// Definition of a vector holding channels.
		using Channels = std::vector<Channel>;

	protected:

		/**
		 * Definition of individual states the serializer can have.
		 */
		enum State
		{
			/// The serializer has not yet been initialized or started.
			S_IDLE,
			/// The serializer has been initialized and is ready to start.
			S_INITIALIZED,
			/// The serializer has been started and is currently actively processing data samples.
			S_STARTED,
			/// The serializer is currently stopping but may still process remaining data samples.
			S_STOPPING,
			/// The serializer has been stopped and all active processing of data samples has finished. However, there may still be samples left which could be requested by the user.
			S_STOPPED
		};

		/// Definition of a map mapping channel configurations to channel ids.
		using ChannelConfigurationMap = std::unordered_map<ChannelConfiguration, ChannelId, ChannelConfiguration::Hash>;

		/// Definition of a pair holding a channel id and a unique data sample.
		using SamplePair = std::pair<ChannelId, UniqueDataSample>;

		/**
		 * This class implements a data sample holding channel configuration information.
		 * This internal class is used to serialize channel configuration as a special sample type, allowing the configuration to be embedded in the data stream alongside regular samples.<br>
		 * When a new channel is encountered during playback, the configuration sample is read first to establish the channel's metadata.
		 */
		class DataSampleChannelConfiguration :
			public DataSample,
			public ChannelConfiguration
		{
			public:

				/**
				 * Creates a new invalid data sample channel configuration.
				 */
				DataSampleChannelConfiguration() = default;

				/**
				 * Creates a new data sample channel configuration from a data sample and channel configuration.
				 * @param sample The data sample
				 * @param channelConfiguration The channel configuration
				 */
				inline explicit DataSampleChannelConfiguration(const DataSample& sample, const ChannelConfiguration& channelConfiguration);

				/**
				 * Creates a new data sample channel configuration from a data timestamp and channel configuration.
				 * @param dataTimestamp The data timestamp
				 * @param channelConfiguration The channel configuration
				 * @param sampleCreationTimestamp The timestamp when the sample was created, this timestamp is not serialized and is only used to automatically determine the playback timestamp
				 */
				inline explicit DataSampleChannelConfiguration(const DataTimestamp& dataTimestamp, const ChannelConfiguration& channelConfiguration, const Timestamp sampleCreationTimestamp = Timestamp(true));

				/**
				 * Reads the sample from an input bitstream.
				 * @param inputBitstream The input bitstream from which the sample will be read
				 * @return True, if succeeded
				 */
				inline bool readSample(InputBitstream& inputBitstream) override;

				/**
				 * Writes the sample to an output bitstream.
				 * @param outputBitstream The output bitstream to which the sample will be written
				 * @return True, if succeeded
				 */
				inline bool writeSample(OutputBitstream& outputBitstream) const override;

				/**
				 * Returns the type of the sample.
				 * @return The sample type
				 */
				inline const std::string& type() const override;
		};

	public:

		/**
		 * Destructs the data serializer.
		 */
		virtual ~DataSerializer() = default;

		/**
		 * Starts the serializer.
		 * @return True, if succeeded
		 */
		virtual bool start() = 0;

		/**
		 * Stops the serializer.
		 * This function sets a stopping flag and returns immediately; it does not wait for the serializer to actually stop.
		 * The background thread will check the stopping flag and terminate asynchronously.
		 * Use stopAndWait() to wait until the serializer has fully stopped, or poll hasStopped() to check if it has stopped.
		 * @return True, if the stop request was accepted; False, if the serializer was not started
		 * @see stopAndWait(), hasStopped().
		 */
		virtual bool stop() = 0;

		/**
		 * Stops the serializer and waits until the serializer has stopped.
		 * @param timeout The timeout in seconds, with range (0, infinity)
		 * @return True, if the serializer stopped within the given timeout
		 */
		virtual bool stopAndWait(const double timeout = 60.0);

		/**
		 * Returns whether the serializer has been started.
		 * @return True, if so
		 */
		[[nodiscard]] virtual bool isStarted() const = 0;

		/**
		 * Returns whether the serializer has stopped but does not check wehther remaining samples have not yet been processed or retrieved.
		 * @return True, if so
		 */
		[[nodiscard]] virtual bool hasStopped() const;

		/**
		 * Returns whether the serializer has stopped and all remaining samples have been processed or retrieved.
		 * @return True, if so
		 * @see DataSerializer::hasFinished().
		 */
		[[nodiscard]] virtual bool hasFinished() const = 0;

	protected:

		/**
		 * Returns whether the given channel value has the configuration bit set.
		 * @param channelValue The channel value to check
		 * @return True, if the channel value is a configuration channel id
		 */
		[[nodiscard]] static constexpr bool isConfigurationChannelId(const uint32_t channelValue);

		/**
		 * Creates a configuration channel id from a regular channel id by setting the highest bit.
		 * @param channelId The channel id
		 * @return The configuration channel id
		 */
		[[nodiscard]] static constexpr uint32_t makeConfigurationChannelId(const ChannelId channelId);

		/**
		 * Extracts the channel id from a channel value by clearing the configuration bit.
		 * @param channelValue The channel value
		 * @return The channel id
		 */
		[[nodiscard]] static constexpr ChannelId extractChannelId(const uint32_t channelValue);

	protected:

		/// The timestamp when the serializer was started.
		Timestamp startTimestamp_;

		/// The current state of the serializer.
		State state_ = S_IDLE;

		/// True, if the serializer succeeded; False, if an error occurred.
		bool succeeded_ = true;

		/// The lock for thread-safe access.
		mutable Lock lock_;
};

constexpr DataSerializer::ChannelId DataSerializer::invalidChannelId()
{
	return ChannelId(-1);
}

inline size_t DataSerializer::ChannelConfiguration::Hash::operator()(const ChannelConfiguration& channelConfiguration) const
{
	size_t seed = std::hash<std::string>()(channelConfiguration.sampleType_);
	seed ^= std::hash<std::string>{}(channelConfiguration.name_) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	seed ^= std::hash<std::string>{}(channelConfiguration.contentType_) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

	return seed;
}

inline DataSerializer::ChannelConfiguration::ChannelConfiguration(const std::string& sampleType, const std::string& name, const std::string& contentType) :
	sampleType_(sampleType),
	name_(name),
	contentType_(contentType)
{
	// nothing to do here
}

inline const std::string& DataSerializer::ChannelConfiguration::sampleType() const
{
	return sampleType_;
}

inline const std::string& DataSerializer::ChannelConfiguration::name() const
{
	return name_;
}

inline const std::string& DataSerializer::ChannelConfiguration::contentType() const
{
	return contentType_;
}

inline bool DataSerializer::ChannelConfiguration::isValid() const
{
	return !sampleType_.empty() && !name_.empty() && !contentType_.empty();
}

inline bool DataSerializer::ChannelConfiguration::operator==(const ChannelConfiguration& channelConfiguration) const
{
	return sampleType_ == channelConfiguration.sampleType_ && name_ == channelConfiguration.name_ && contentType_ == channelConfiguration.contentType_;
}

inline size_t DataSerializer::Channel::Hash::operator()(const Channel& channel) const
{
	size_t seed = std::hash<uint32_t>()(channel.channelId_);
	seed ^= ChannelConfiguration::Hash{}(channel) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

	return seed;
}

inline DataSerializer::Channel::Channel(const ChannelConfiguration& channelConfiguration, const ChannelId channelId) :
	ChannelConfiguration(channelConfiguration),
	channelId_(channelId)
{
	// nothing to do here
}

inline DataSerializer::ChannelId DataSerializer::Channel::channelId() const
{
	return channelId_;
}

inline bool DataSerializer::Channel::isValid() const
{
	return channelId_ != invalidChannelId() && ChannelConfiguration::isValid();
}

inline bool DataSerializer::Channel::operator==(const Channel& channel) const
{
	return channelId_ == channel.channelId_ && ChannelConfiguration::operator==(channel);
}

constexpr bool DataSerializer::isConfigurationChannelId(const uint32_t channelValue)
{
	constexpr uint32_t highestBit = 1u << 31u;

	return channelValue & highestBit;
}

constexpr uint32_t DataSerializer::makeConfigurationChannelId(const ChannelId channelId)
{
	ocean_assert(channelId != invalidChannelId());
	ocean_assert(!isConfigurationChannelId(channelId));

	constexpr uint32_t highestBit = 1u << 31u;

	return channelId | highestBit;
}

constexpr DataSerializer::ChannelId DataSerializer::extractChannelId(const uint32_t channelValue)
{
	constexpr uint32_t highestBit = 1u << 31u;

	return ChannelId(channelValue & ~highestBit);
}

inline DataSerializer::DataSampleChannelConfiguration::DataSampleChannelConfiguration(const DataSample& sample, const ChannelConfiguration& channelConfiguration) :
	DataSample(sample),
	ChannelConfiguration(channelConfiguration)
{
	// nothing to do here
}

inline DataSerializer::DataSampleChannelConfiguration::DataSampleChannelConfiguration(const DataTimestamp& dataTimestamp, const ChannelConfiguration& channelConfiguration, const Timestamp sampleCreationTimestamp) :
	DataSample(dataTimestamp, sampleCreationTimestamp),
	ChannelConfiguration(channelConfiguration)
{
	// nothing to do here
}

inline bool DataSerializer::DataSampleChannelConfiguration::readSample(InputBitstream& inputBitstream)
{
	if (!DataSample::readSample(inputBitstream))
	{
		return false;
	}

	return inputBitstream.read<std::string>(sampleType_) && inputBitstream.read<std::string>(name_) && inputBitstream.read<std::string>(contentType_);
}

inline bool DataSerializer::DataSampleChannelConfiguration::writeSample(OutputBitstream& outputBitstream) const
{
	if (!DataSample::writeSample(outputBitstream))
	{
		return false;
	}

	return outputBitstream.write<std::string>(sampleType_) && outputBitstream.write<std::string>(name_) && outputBitstream.write<std::string>(contentType_);
}

inline const std::string& DataSerializer::DataSampleChannelConfiguration::type() const
{
	return sampleType_;
}

}

}

}

#endif // META_OCEAN_IO_SERIALIZATION_DATA_SERIALIZER_H
