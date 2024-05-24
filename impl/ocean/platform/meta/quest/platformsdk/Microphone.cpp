/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/meta/quest/platformsdk/Microphone.h"
#include "ocean/platform/meta/quest/platformsdk/Manager.h"

#include <OVR_Microphone.h>

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace PlatformSDK
{

Microphone::ScopedSubscription Microphone::start(SampleCallback sampleCallback)
{
	const ScopedLock scopedLock(lock_);

	if (sampleCallbackMap_.empty())
	{
		ocean_assert(microphoneHandle_ == nullptr);

		if (!Manager::get().isInitialized())
		{
			Log::error() << "Microphone::start() failed, Platform SDK is not initialized";
			return Microphone::ScopedSubscription();
		}

		microphoneHandle_ = ovr_Microphone_Create();

		if (microphoneHandle_ == nullptr)
		{
			Log::error() << "Failed to create microphone";
			return Microphone::ScopedSubscription();
		}

		ovr_Microphone_Start(microphoneHandle_);

		startThread();
	}

	const unsigned int subscriptionId = subscriptionIdCounter_++;

	ocean_assert(subscriptionId != 0u);
	sampleCallbackMap_.emplace(subscriptionId, std::move(sampleCallback));

	return ScopedSubscription(subscriptionId, std::bind(&Microphone::stop, this, std::placeholders::_1));
}

void Microphone::stop(const unsigned int& subscriptionId)
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(microphoneHandle_ != nullptr);

	ocean_assert(!sampleCallbackMap_.empty());
	ocean_assert(sampleCallbackMap_.find(subscriptionId) != sampleCallbackMap_.cend());

	sampleCallbackMap_.erase(subscriptionId);

	if (sampleCallbackMap_.empty())
	{
		stopThread();

		ovr_Microphone_Stop(microphoneHandle_);
		ovr_Microphone_Destroy(microphoneHandle_);

		microphoneHandle_ = nullptr;
	}
}

void Microphone::threadRun()
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
			const ScopedLock scopedLock(lock_);

			for (SampleCallbackMap::const_iterator iSampleCallback = sampleCallbackMap_.cbegin(); iSampleCallback != sampleCallbackMap_.cend(); ++iSampleCallback)
			{
				iSampleCallback->second(buffer.data(), buffer.size());
			}

			positionInChunk = 0;
		}
	}
}

} // namespace PlatformSDK

} // namespace Quest

} // namespace Meta

} // namespace Platform

} // namespace Ocean
