// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/Microphone.h"

namespace Ocean
{

namespace Media
{

Microphone::Microphone(const std::string& url) :
	Medium(url),
	ConfigMedium(url),
	LiveMedium(url)
{
	type_ = Type(type_ | MICROPHONE);
}

Microphone::MicrophoneTypes Microphone::microphoneTypes() const
{
	const ScopedLock scopedLock(lock_);

	return microphoneTypes_;
}

Microphone::MicrophoneConfigurations Microphone::microphoneConfigurations() const
{
	const ScopedLock scopedLock(lock_);

	return microphoneConfigurations_;
}

Microphone::SamplesScopedSubscription Microphone::addSamplesCallback(SamplesCallbackFunction samplesCallbackFunction)
{
	ocean_assert(samplesCallbackFunction);

	return samplesCallbackHandler_.addCallback(std::move(samplesCallbackFunction)); // samplesCallbackHandler_ is already thread-safe
}

}

}
