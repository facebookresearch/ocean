/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/io/serialization/InputDataSerializer.h"

namespace Ocean
{

namespace IO
{

namespace Serialization
{

bool InputDataSerializer::initialize(Channels* preparsedChannels, bool* isStreamCorrupted)
{
	if (preparsedChannels != nullptr)
	{
		preparsedChannels->clear();
	}

	if (isStreamCorrupted != nullptr)
	{
		*isStreamCorrupted = false;
	}

	const ScopedLock scopedLock(lock_);

	if (stream_)
	{
		ocean_assert(false && "The input bitstream has already been created!");
		return false;
	}

	stream_ = createStream();

	if (!stream_)
	{
		ocean_assert(false && "Failed to create the input stream!");
		return false;
	}

	InputBitstream& inputBitstream = stream_->inputBitstream();
	ocean_assert(inputBitstream);

	if (!readHeader(inputBitstream))
	{
		ocean_assert(false && "Failed to read the header!");
		stream_ = nullptr;
		return false;
	}

	bool correctEndOfStreamIndication = false;

	if (preparsedChannels != nullptr)
	{
		// we pre-parse the channels to ensure that we can register the channels before the first sample arrives

		Channels channels;
		channels.reserve(16);

		UnorderedIndexSet32 channelIdSet;

		while (true)
		{
			uint32_t channelValue = 0u;
			if (!inputBitstream.read<uint32_t>(channelValue))
			{
				if (inputBitstream.isEndOfFile())
				{
					Log::debug() << "InputDataSerializer: The input seems to be corrupted, end of stream indication is missing";
					break;
				}

				return false;
			}

			if (channelValue == invalidChannelId())
			{
				// we have reached the end of the stream, indicated by an invalid channel id

				uint8_t lastReadAttempt;
				if (!inputBitstream.read<uint8_t>(lastReadAttempt) && inputBitstream.isEndOfFile())
				{
					correctEndOfStreamIndication = true;
				}
				else
				{
					Log::debug() << "InputDataSerializer: The input seems to be corrupted, we read and end of stream indication without being at the end of the stream";
				}

				break;
			}

			uint32_t payloadSize = 0u;
			if (!inputBitstream.read<uint32_t>(payloadSize))
			{
				return false;
			}

			if (isConfigurationChannelId(channelValue))
			{
				const ChannelId channelId = extractChannelId(channelValue);

				if (!channelIdSet.emplace(channelId).second)
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

#ifdef OCEAN_DEBUG
				const uint64_t debugStartPosition = inputBitstream.position();
#endif

				DataSampleChannelConfiguration dataSampleChannelConfiguration;
				if (!dataSampleChannelConfiguration.readSample(inputBitstream) || !dataSampleChannelConfiguration.isValid())
				{
					return false;
				}

#ifdef OCEAN_DEBUG
				const uint64_t debugBytesRead = inputBitstream.position() - debugStartPosition;

				if (debugBytesRead != uint64_t(payloadSize))
				{
					ocean_assert(false && "Payload size mismatch!");
				}
#endif // OCEAN_DEBUG

				channels.emplace_back(dataSampleChannelConfiguration, channelId);
			}
			else
			{
				if (!inputBitstream.skip(uint64_t(payloadSize)))
				{
					return false;
				}
			}
		}

		inputBitstream.reset();
		ocean_assert(inputBitstream);

		if (!readHeader(inputBitstream))
		{
			ocean_assert(false && "Failed to read the header!");
			stream_ = nullptr;
			return false;
		}

		*preparsedChannels = std::move(channels);
	}

	if (isStreamCorrupted != nullptr)
	{
		*isStreamCorrupted = !correctEndOfStreamIndication;
	}

	return true;
}

bool InputDataSerializer::start()
{
	const ScopedLock scopedLock(lock_);

	if (!stream_)
	{
		ocean_assert(false && "The serializer has not yet been initialized!");
		return false;
	}

	ocean_assert(!startTimestamp_.isValid());
	startTimestamp_.toNow();

	return startThread();
}

bool InputDataSerializer::stop()
{
	const ScopedLock scopedLock(lock_);

	if (stream_ == nullptr)
	{
		return false;
	}

	stopping_ = true;

	return true;
}

bool InputDataSerializer::isStarted() const
{
	const ScopedLock scopedLock(lock_);

	return stream_ != nullptr;
}

bool InputDataSerializer::hasStopped() const
{
	const ScopedLock scopedLock(lock_);

	if (stream_ != nullptr)
	{
		return false;
	}

	if (!sampleQueue_.empty())
	{
		// the stream has already parsed all samples, however the queue is still not empty yet
		return false;
	}

	return true;
}

bool InputDataSerializer::registerFactoryFunction(const std::string& sampleType, const FactoryFunction& factoryFunction)
{
	ocean_assert(!sampleType.empty());
	ocean_assert(factoryFunction);

	const ScopedLock scopedLock(lock_);

	if (startTimestamp_.isValid())
	{
		ocean_assert(false && "The serializer has been started already!");
		return false;
	}

	if (!factoryFunctionMap_.emplace(sampleType, factoryFunction).second)
	{
		ocean_assert(false && "The factory function has already been registered!");
		return false;
	}

	return true;
}

bool InputDataSerializer::registerChannelEventFunction(const ChannelEventFunction& channelEventFunction)
{
	ocean_assert(channelEventFunction);

	const ScopedLock scopedLock(lock_);

	if (startTimestamp_.isValid())
	{
		ocean_assert(false && "The serializer has been started already!");
		return false;
	}

	if (channelEventFunction_)
	{
		ocean_assert(false && "The channel event function has already been registered!");
		return false;
	}

	channelEventFunction_ = channelEventFunction;

	return true;
}

UniqueDataSample InputDataSerializer::sample(ChannelId& channelId, const double speed)
{
	const Timestamp currentTimestamp(true);

	const ScopedLock scopedLock(lock_);

	if (sampleQueue_.empty())
	{
		return nullptr;
	}

	if (speed > 0.0)
	{
		ocean_assert(startTimestamp_.isValid());
		const double playbackTimestamp = double(currentTimestamp - startTimestamp_);

		const double samplePlaybackTimestamp = sampleQueue_.front().second->playbackTimestamp();

		if (samplePlaybackTimestamp > playbackTimestamp * speed)
		{
			return nullptr;
		}
	}

	channelId = sampleQueue_.front().first;
	UniqueDataSample sample = std::move(sampleQueue_.front().second);
	sampleQueue_.pop();

	return sample;
}

DataSerializer::ChannelConfiguration InputDataSerializer::channelConfiguration(const ChannelId channelId) const
{
	const ScopedLock scopedLock(lock_);

	const ExtendedChannelMap::const_iterator iChannel = extendedChannelMap_.find(channelId);

	if (iChannel != extendedChannelMap_.cend())
	{
		return ChannelConfiguration(iChannel->second.sampleType(), iChannel->second.name(), iChannel->second.contentType());
	}

	return ChannelConfiguration(); // return invalid/default channel
}

DataSerializer::Channels InputDataSerializer::channels() const
{
	const ScopedLock scopedLock(lock_);

	Channels result;
	result.reserve(extendedChannelMap_.size());

	for (const ExtendedChannelMap::value_type& channelPair : extendedChannelMap_)
	{
		result.emplace_back(channelPair.second);
	}

	return result;
}

bool InputDataSerializer::readHeader(InputBitstream& inputBitstream)
{
	ocean_assert(inputBitstream);

	std::array<char, 9> formatString = {};
	if (!inputBitstream.read(formatString.data(), formatString.size() - 1))
	{
		ocean_assert(false && "Failed to read the header!");
		return false;
	}

	// expected: OCEANDAT
	if (formatString[0] != 'O' || formatString[1] != 'C' || formatString[2] != 'E' || formatString[3] != 'A' || formatString[4] != 'N' || formatString[5] != 'D' || formatString[6] != 'A' || formatString[7] != 'T')
	{
		ocean_assert(false && "Invalid header!");
		return false;
	}

	uint32_t version = uint32_t(-1);
	if (!inputBitstream.read<uint32_t>(version))
	{
		ocean_assert(false && "Failed to read the header!");
		return false;
	}

	if (version != 0u)
	{
		ocean_assert(false && "Invalid version!");
		return false;
	}

	return true;
}

void InputDataSerializer::threadRun()
{
	ocean_assert(stream_);

	ExtendedChannelMap extendedChannelMap; // local channel map for performance (no lock needed), will be synchronized with member extendedChannelMap_ under lock (whenever updated)
	extendedChannelMap.reserve(32);

	InputBitstream& inputBitstream = stream_->inputBitstream();

	while (!shouldThreadStop())
	{
		TemporaryScopedLock scopedTemporaryLock(lock_);

			if (stopping_)
			{
				break;
			}

			if (sampleQueue_.size() > maxPendingSampleQueueSize_)
			{
				scopedTemporaryLock.release();

				Thread::sleep(1u);
				continue;
			}

		scopedTemporaryLock.release();

		uint32_t channelValue = 0u;
		if (!inputBitstream.read<uint32_t>(channelValue))
		{
			if (inputBitstream.isEndOfFile())
			{
				Log::debug() << "InputDataSerializer: The input seems to be corrupted, end of stream indication is missing";
				break;
			}

			succeeded_ = false;
			break;
		}

		if (channelValue == invalidChannelId())
		{
			// we have reached the end of the stream, indicated by an invalid channel id

			uint8_t lastReadAttempt;
			if (inputBitstream.read<uint8_t>(lastReadAttempt) || !inputBitstream.isEndOfFile())
			{
				Log::debug() << "InputDataSerializer: The input seems to be corrupted, we read and end of stream indication without being at the end of the stream";
				succeeded_ = false;
			}

			break;
		}

		uint32_t payloadSize = 0u;
		if (!inputBitstream.read<uint32_t>(payloadSize))
		{
			succeeded_ = false;
			break;
		}

		if (isConfigurationChannelId(channelValue))
		{
			const ChannelId channelId = extractChannelId(channelValue);

			if (extendedChannelMap.contains(channelId))
			{
				ocean_assert(false && "The channel has already been registered!");

				succeeded_ = false;
				break;
			}

#ifdef OCEAN_DEBUG
			const uint64_t debugStartPosition = inputBitstream.position();
#endif

			DataSampleChannelConfiguration dataSampleChannelConfiguration;
			if (!dataSampleChannelConfiguration.readSample(inputBitstream) || !dataSampleChannelConfiguration.isValid())
			{
				succeeded_ = false;
				break;
			}

#ifdef OCEAN_DEBUG
			const uint64_t debugBytesRead = inputBitstream.position() - debugStartPosition;

			if (debugBytesRead != uint64_t(payloadSize))
			{
				ocean_assert(false && "Payload size mismatch!");
				succeeded_ = false;
				break;
			}
#endif // OCEAN_DEBUG

			ChannelEventFunction channelEventFunction;

			const Channel channel(dataSampleChannelConfiguration, channelId);

			TemporaryScopedLock temporaryScopedLock(lock_);

				FactoryFunctionMap::const_iterator iFactoryFunction = factoryFunctionMap_.find(dataSampleChannelConfiguration.sampleType());

				if (iFactoryFunction != factoryFunctionMap_.cend())
				{
					const ExtendedChannel extendedChannel(channel, iFactoryFunction->second);

					extendedChannelMap.emplace(channelId, extendedChannel); // local channel map for lock-free lookup

					extendedChannelMap_.emplace(channelId, extendedChannel); // global channel map for lock-based lookup

				}
				else
				{
					Log::debug() << "FileInputDataSerializer: The sample type '" << dataSampleChannelConfiguration.sampleType() << "' is not registered, skipping";
				}

				channelEventFunction = channelEventFunction_;

			temporaryScopedLock.release();

			if (channelEventFunction)
			{
				channelEventFunction(channel);
			}
		}
		else
		{
			const ChannelId channelId = extractChannelId(channelValue);

			const ExtendedChannelMap::const_iterator iExtendedChannel = extendedChannelMap.find(channelId);

			if (iExtendedChannel != extendedChannelMap.cend())
			{
				const ExtendedChannel& channel = iExtendedChannel->second;

				const FactoryFunction& factoryFunction = channel.factoryFunction_;
				ocean_assert(factoryFunction);

				UniqueDataSample sample = factoryFunction(channel.sampleType());

				ocean_assert(sample);
				if (!sample)
				{
					succeeded_ = false;
					break;
				}

				if (sample->readSample(inputBitstream))
				{
					const ScopedLock scopedLock(lock_);

					sampleQueue_.emplace(channelId, std::move(sample));
				}
				else
				{
					ocean_assert(false && "Failed to read the sample!");
					succeeded_ = false;
					break;
				}
			}
			else
			{
				if (!inputBitstream.skip(uint64_t(payloadSize)))
				{
					succeeded_ = false;
					break;
				}
			}
		}
	}

	const ScopedLock scopedLock(lock_);

	stream_ = nullptr;
}

bool FileInputDataSerializer::setFilename(const std::string& filename)
{
	if (filename.empty())
	{
		ocean_assert(false && "Invalid filename!");
		return false;
	}

	const ScopedLock scopedLock(lock_);

	if (stream_)
	{
		ocean_assert(false && "The input bitstream has already been created!");
		return false;
	}

	filename_ = filename;

	return true;
}

InputDataSerializer::UniqueStream FileInputDataSerializer::createStream() const
{
	const ScopedLock scopedLock(lock_);

	if (filename_.empty())
	{
		ocean_assert(false && "Invalid filename!");
		return nullptr;
	}

	UniqueStream stream = std::make_unique<FileStream>(filename_);

	if (!stream->isValid())
	{
		Log::error() << "FileInputDataSerializer: Failed to create the input stream for '" << filename_ << "'";
		return nullptr;
	}

	return stream;
}

}

}

}
