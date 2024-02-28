// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/quest/QuestMicrophone.h"

#include <OVR_Microphone.h>

namespace Ocean
{

namespace Media
{

namespace Quest
{

QuestMicrophone::QuestMicrophone(const std::string& url) :
	Medium(url),
	ConfigMedium(url),
	LiveMedium(url),
	Microphone(url)
{
	libraryName_ = nameQuestLibrary();

	isValid_ = false;

	if (String::toLower(url).find("microphone") == std::string::npos)
	{
		return;
	}

	microphoneHandle_ = ovr_Microphone_Create();

	if (microphoneHandle_ != nullptr)
	{
		isValid_ = true;
	}
	else
	{
		Log::error() << "Failed to access OVR microphone";
	}
}

QuestMicrophone::~QuestMicrophone()
{
	stop();

	stopThreadExplicitly();

	if (microphoneHandle_ != nullptr)
	{
		ovr_Microphone_Destroy(microphoneHandle_);
	}
}

bool QuestMicrophone::isStarted() const
{
	const ScopedLock scopedLock(lock_);

	return startTimestamp_.isValid();
}

Timestamp QuestMicrophone::startTimestamp() const
{
	const ScopedLock scopedLock(lock_);

	Timestamp timestamp(startTimestamp_);

	return timestamp;
}

Timestamp QuestMicrophone::pauseTimestamp() const
{
	const ScopedLock scopedLock(lock_);

	Timestamp timestamp(pauseTimestamp_);

	return timestamp;
}

Timestamp QuestMicrophone::stopTimestamp() const
{
	const ScopedLock scopedLock(lock_);

	Timestamp timestamp(stopTimestamp_);

	return timestamp;
}

bool QuestMicrophone::start()
{
	const ScopedLock scopedLock(lock_);

	if (startTimestamp_.isValid())
	{
		return true;
	}

	startTimestamp_.toNow();
	pauseTimestamp_.toInvalid();
	stopTimestamp_.toInvalid();

	ocean_assert(microphoneHandle_ != nullptr);

	ovr_Microphone_Start(microphoneHandle_);

	startThread();

	return true;
}

bool QuestMicrophone::pause()
{
	const ScopedLock scopedLock(lock_);

	if (stopTimestamp_.isValid())
	{
		return false;
	}

	if (pauseTimestamp_.isValid() || startTimestamp_.isInvalid())
	{
		return true;
	}

	startTimestamp_.toInvalid();
	pauseTimestamp_.toNow();
	stopTimestamp_.toInvalid();

	stopThread();

	ovr_Microphone_Stop(microphoneHandle_);

	return true;
}

bool QuestMicrophone::stop()
{
	const ScopedLock scopedLock(lock_);

	if (stopTimestamp_.isValid() || (startTimestamp_.isInvalid() && pauseTimestamp_.isInvalid()))
	{
		return true;
	}

	pauseTimestamp_.toInvalid();
	startTimestamp_.toInvalid();
	stopTimestamp_.toNow();

	stopThread();

	ovr_Microphone_Stop(microphoneHandle_);

	return true;
}

void QuestMicrophone::threadRun()
{
	ocean_assert(microphoneHandle_ != nullptr);

	const size_t maxBufferSize = ovr_Microphone_GetOutputBufferMaxSize(microphoneHandle_);
	if (maxBufferSize == 0)
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	// we ensure that every chunk has a length of 20ms

	constexpr size_t sampleRate = 48000; // 48Khz
	constexpr size_t chunkSize = sampleRate / 50; // 20ms

	std::vector<int16_t> buffer(chunkSize);
	size_t positionInChunk = 0;

	while (!shouldThreadStop())
	{
		const size_t elements = ovr_Microphone_GetPCM(microphoneHandle_, buffer.data() + positionInChunk, buffer.size() - positionInChunk);

		positionInChunk += elements;
		ocean_assert(positionInChunk <= buffer.size());

		if (elements == 0)
		{
			sleep(1u);
			continue;
		}
		else if (positionInChunk == buffer.size())
		{
			sendSamples(ST_INTEGER_16_MONO_48, buffer.data(), buffer.size() * sizeof(int16_t));
			positionInChunk = 0;
		}
	}
}

}

}

}
