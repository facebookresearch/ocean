// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/network/verts/Voip.h"

#include "ocean/media/Manager.h"

#include "ocean/network/verts/Manager.h"

namespace Ocean
{

namespace Network
{

namespace Verts
{

Voip::Zone::Zone(SharedDriver vertsDriver) :
	usageCounter_(1u),
	vertsDriver_(std::move(vertsDriver))
{
	ocean_assert(vertsDriver_);

	voipSamplesScopedSubscription_ = vertsDriver_->addVoipSamplesCallback(std::bind(&Zone::onVoipSamples, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	ocean_assert(voipSamplesScopedSubscription_.isValid());

	liveAudio_ = Media::Manager::get().newMedium("Voip live audio for zone", Media::Medium::LIVE_AUDIO, true /*useExclusive*/);

	if (liveAudio_)
	{
		if (!liveAudio_->start())
		{
			Log::error() << "Failed to start live audio";
		}
	}
	else
	{
		Log::error() << "Failed to create live audio";
	}
}

void Voip::Zone::incrementUsage()
{
	ocean_assert(usageCounter_ >= 1u);
	++usageCounter_;
}

bool Voip::Zone::decrementUsage()
{
	ocean_assert(usageCounter_ >= 1u);
	--usageCounter_;

	return usageCounter_ != 0u;
}

void Voip::Zone::onVoipSamples(Driver& driver, const int16_t* samples, const size_t size)
{
	ocean_assert(samples != nullptr && size > 0);
	ocean_assert(liveAudio_);

	liveAudio_->addSamples(Media::LiveAudio::ST_INTEGER_16_MONO_48, samples, size * sizeof(uint16_t));
}

Voip::Voip()
{
	// nothing to do here
}

Voip::VoipScopedSubscription Voip::startVoip(const std::string& zoneName)
{
	if (!Manager::get().isInitialized())
	{
		Log::error() << "Voip: VERTS component is not initialized";
		return VoipScopedSubscription();
	}

	const ScopedLock scopedLock(lock_);

	ZoneMap::iterator iZone = zoneMap_.find(zoneName);

	if (iZone != zoneMap_.cend())
	{
		iZone->second->incrementUsage();

		return VoipScopedSubscription(zoneName, std::bind(&Voip::stopVoip, this, std::placeholders::_1));
	}

	if (microphone_.isNull())
	{
		microphone_ = Media::Manager::get().newMedium("Gained Voice Microphone", Media::Medium::MICROPHONE);

		if (microphone_.isNull())
		{
			Log::error() << "Voip: Failed to access the microphone";
			return VoipScopedSubscription();
		}
	}

	SharedDriver vertsDriver = Manager::get().driver(zoneName);

	if (vertsDriver == nullptr)
	{
		return VoipScopedSubscription();
	}

	zoneMap_.emplace(zoneName, std::make_unique<Zone>(std::move(vertsDriver)));

	if (zoneMap_.size() == 1)
	{
		ocean_assert(microphone_);

		ocean_assert(!microphoneSubscription_.isValid());
		microphoneSubscription_ = microphone_->addSamplesCallback(std::bind(&Voip::onMicrophoneSample, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		ocean_assert(microphoneSubscription_.isValid());

		microphone_->start();
	}

	return VoipScopedSubscription(zoneName, std::bind(&Voip::stopVoip, this, std::placeholders::_1));
}

void Voip::updatePosition(const Vector3& position)
{
	const ScopedLock scopedLock(lock_);

	position_ = position;
}

void Voip::stopVoip(const std::string& zoneName)
{
	TemporaryScopedLock scopedLock(lock_);

	ZoneMap::iterator iZone = zoneMap_.find(zoneName);

	if (iZone == zoneMap_.cend())
	{
		ocean_assert(false && "The voip zone does not exist");
	}

	std::unique_ptr<Zone> deferredCleanupZone;

	if (!iZone->second->decrementUsage())
	{
		deferredCleanupZone = std::move(iZone->second);

		zoneMap_.erase(iZone);
	}

	if (zoneMap_.empty())
	{
		ocean_assert(microphoneSubscription_.isValid());
		microphoneSubscription_.release();
	}

	scopedLock.release();
}

void Voip::onMicrophoneSample(const Media::Microphone::SamplesType samplesType, const void* data, const size_t size)
{
	if (samplesType != Media::Microphone::ST_INTEGER_16_MONO_48)
	{
		Log::error() << "Voip: The samples type is not supported";
		return;
	}

	ocean_assert(size % sizeof(int16_t) == 0);
	ocean_assert(size_t(data) % sizeof(int16_t) == 0);

	const int16_t* elements = (const int16_t*)(data);
	const unsigned int numberElements = size / sizeof(int16_t);

	const ScopedLock scopedLock(lock_);

	for (ZoneMap::const_iterator iZone = zoneMap_.cbegin(); iZone != zoneMap_.cend(); ++iZone)
	{
		iZone->second->vertsDriver_->sendVoipSamples(elements, numberElements, VectorF3(position_));
	}
}

}

}

}
