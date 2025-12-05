/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/io/serialization/OutputDataSerializer.h"

#include "ocean/io/serialization/VectorOutputStream.h"

namespace Ocean
{

namespace IO
{

namespace Serialization
{

bool OutputDataSerializer::start()
{
	const ScopedLock scopedLock(lock_);

	if (state_ >= S_STARTED)
	{
		return true;
	}

	if (stream_)
	{
		ocean_assert(false && "The output bitstream has already been created!");
		return false;
	}

	stream_ = createStream();

	if (!stream_)
	{
		ocean_assert(false && "Failed to create the output stream!");
		return false;
	}

	if (!writeHeader(stream_->outputBitstream()))
	{
		ocean_assert(false && "Failed to write the header!");
		stream_ = nullptr;
		return false;
	}

	ocean_assert(!startTimestamp_.isValid());
	startTimestamp_.toNow();

	state_ = S_STARTED;

	return startThread();
}

bool OutputDataSerializer::stop()
{
	const ScopedLock scopedLock(lock_);

	if (state_ < S_STARTED)
	{
		ocean_assert(false && "The serializer has not yet been started!");
		return false;
	}

	if (state_ >= S_STOPPING)
	{
		return true;
	}

	ocean_assert(state_ == S_STARTED);
	ocean_assert(stream_ != nullptr);

	state_ = S_STOPPING;

	return true;
}

bool OutputDataSerializer::isStarted() const
{
	const ScopedLock scopedLock(lock_);

	return state_ >= S_STARTED && state_ < S_STOPPED;
}

bool OutputDataSerializer::hasFinished() const
{
	const ScopedLock scopedLock(lock_);

	if (!hasStopped())
	{
		return false;
	}

	ocean_assert(sampleQueue_.empty());
	return true;
}

DataSerializer::ChannelId OutputDataSerializer::addChannel(const std::string& sampleType, const std::string& name, const std::string& contentType)
{
	ocean_assert(!sampleType.empty() && !name.empty() && !contentType.empty());

	if (sampleType.empty() || name.empty() || contentType.empty())
	{
		return invalidChannelId();
	}

	const ScopedLock scopedLock(lock_);

	const ChannelConfiguration channelConfiguration(sampleType, name, contentType);

	if (channelConfigurationMap_.contains(channelConfiguration))
	{
		return invalidChannelId();
	}

	const ChannelId channelId = nextChannelId_++;

	channelConfigurationMap_.emplace(channelConfiguration, channelId);

	return channelId;
}

bool OutputDataSerializer::addSample(const ChannelId channelId, UniqueDataSample&& sample)
{
	ocean_assert(channelId != invalidChannelId());
	ocean_assert(sample);

	const ScopedLock scopedLock(lock_);

	if (!succeeded_)
	{
		return false;
	}

	if (channelId >= nextChannelId_)
	{
		ocean_assert(false && "Invalid channel id!");
		return false;
	}

	sampleQueue_.emplace(channelId, std::move(sample));

	return true;
}

bool OutputDataSerializer::writeHeader(OutputBitstream& outputBitstream)
{
	ocean_assert(outputBitstream);

	if (!outputBitstream.write("OCEANDAT", 8u))
	{
		ocean_assert(false && "Failed to write the header!");
		return false;
	}

	constexpr uint32_t version = 0u;
	if (!outputBitstream.write<uint32_t>(version))
	{
		return false;
	}

	return true;
}

void OutputDataSerializer::threadRun()
{
	ocean_assert(stream_);
	if (!stream_)
	{
		return;
	}

	ocean_assert(succeeded_);
	if (!succeeded_)
	{
		return;
	}

	OutputBitstream& outputBitstream = stream_->outputBitstream();

	VectorOutputStream sampleStream(1024 * 1024);
	OutputBitstream sampleBitstream(sampleStream);

	UnorderedIndexSet32 activeChannelIds;

	while (!shouldThreadStop())
	{
		TemporaryScopedLock temporaryScopedLock(lock_);

			if (sampleQueue_.empty())
			{
				if (state_ >= S_STOPPING)
				{
					break;
				}

				temporaryScopedLock.release();

				sleep(1u);
				continue;
			}

			const ChannelId channelId = sampleQueue_.front().first;
			UniqueDataSample sample = std::move(sampleQueue_.front().second);
			sampleQueue_.pop();

		temporaryScopedLock.release();

		ocean_assert(channelId != invalidChannelId());
		ocean_assert(sample);

		sample->configurePlaybackTimestamp(startTimestamp_); // the sample holds a construction timestamp which is used to determine the playback timestamp

		if (activeChannelIds.emplace(channelId).second)
		{
			// the channel is new, so we first need to write the channel's configuration

			const ChannelConfiguration* channelConfiguration = nullptr;

			temporaryScopedLock.relock(lock_);

				for (const ChannelConfigurationMap::value_type& channelConfigurationPair : channelConfigurationMap_)
				{
					if (channelConfigurationPair.second == channelId)
					{
						channelConfiguration = &channelConfigurationPair.first;

						break;
					}
				}

			temporaryScopedLock.release();

			if (channelConfiguration == nullptr)
			{
				ocean_assert(false && "This should never happen!");
				succeeded_ = false;
				break;
			}

			const DataSampleChannelConfiguration dataSampleChannelConfiguration(*sample, *channelConfiguration);

			if (!dataSampleChannelConfiguration.writeSample(sampleBitstream))
			{
				succeeded_ = false;
				break;
			}

			const size_t payloadSize = sampleBitstream.size();

			if (!NumericT<uint32_t>::isInsideValueRange(payloadSize))
			{
				succeeded_ = false;
				break;
			}

			const uint32_t channelValue = makeConfigurationChannelId(channelId);

			if (!outputBitstream.write<uint32_t>(channelValue)
				|| !outputBitstream.write<uint32_t>(uint32_t(payloadSize))
				|| !outputBitstream.write(sampleStream.data(), payloadSize))
			{
				succeeded_ = false;
				break;
			}

			sampleStream.clear();
		}

		if (!sample->writeSample(sampleBitstream))
		{
			succeeded_ = false;
			break;
		}

		const size_t payloadSize = sampleBitstream.size();

		if (!NumericT<uint32_t>::isInsideValueRange(payloadSize))
		{
			succeeded_ = false;
			break;
		}

		if (!outputBitstream.write<uint32_t>(uint32_t(channelId))
				|| !outputBitstream.write<uint32_t>(uint32_t(payloadSize))
				|| !outputBitstream.write(sampleStream.data(), payloadSize))
		{
			succeeded_ = false;
			break;
		}

		sampleStream.clear();
	}

	// let's write a final invalid channel id to indicate the end of the stream

	if (!outputBitstream.write<uint32_t>(invalidChannelId()))
	{
		succeeded_ = false;
	}

	const ScopedLock scopedLock(lock_);

	stream_ = nullptr;
	state_ = S_STOPPED;
}

bool FileOutputDataSerializer::setFilename(const std::string& filename)
{
	if (filename.empty())
	{
		ocean_assert(false && "Invalid filename!");
		return false;
	}

	const ScopedLock scopedLock(lock_);

	if (stream_)
	{
		ocean_assert(false && "The output bitstream has already been created!");
		return false;
	}

	filename_ = filename;

	return true;
}

OutputDataSerializer::UniqueStream FileOutputDataSerializer::createStream() const
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
		Log::error() << "FileOutputDataSerializer: Failed to create the output stream for '" << filename_ << "'";
		return nullptr;
	}

	return stream;
}

}

}

}
