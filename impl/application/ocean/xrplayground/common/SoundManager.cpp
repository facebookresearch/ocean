// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/SoundManager.h"

#include "ocean/io/File.h"
#include "ocean/io/FileResolver.h"

#include "ocean/math/Numeric.h"

#include "ocean/media/Manager.h"

namespace Ocean
{

namespace XRPlayground
{

SoundManager::Sound::Sound(const std::string& soundFile) :
	soundFile_(soundFile),
	usageCounter_(1u)
{
	Media::AudioRef audio(Media::Manager::get().newMedium(soundFile_, Media::Medium::AUDIO));

	if (audio)
	{
		audios_.emplace_back(std::move(audio));
	}
}

bool SoundManager::Sound::play(const float intensity)
{
	ocean_assert(!audios_.empty());

	for (const Media::AudioRef& audio : audios_)
	{
		ocean_assert(audio);

		if (!audio->isStarted())
		{
			if (play(audio, intensity))
			{
				return true;
			}
		}
	}

	if (audios_.size() > 10)
	{
		Log::warning() << "Too many sounds playing at the same time";
		return false;
	}

	ocean_assert(!soundFile_.empty());
	Media::AudioRef audio(Media::Manager::get().newMedium(soundFile_, Media::Medium::AUDIO));

	if (audio.isNull())
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	if (!play(audio, intensity))
	{
		return false;
	}

	audios_.emplace_back(std::move(audio));

	return true;
}

void SoundManager::Sound::incrementUsage()
{
	++usageCounter_;
}

bool SoundManager::Sound::decrementUsage()
{
	ocean_assert(usageCounter_ >= 1u);

	if (--usageCounter_ == 0u)
	{
		return true;
	}

	return false;
}

bool SoundManager::Sound::isValid() const
{
	return !audios_.empty();
}

bool SoundManager::Sound::play(const Media::AudioRef& audio, const float intensity)
{
	ocean_assert(audio);
	ocean_assert(intensity >= 0.0f && intensity <= 1.0f);

	const float decibel = 20.0f * NumericF::log10(intensity);

	audio->setSoundVolume(decibel);

	return audio->start();
}

[[nodiscard]] SoundManager::SoundScopedSubscription SoundManager::subscribe(const std::string& soundFile, const SoundId& soundId)
{
	ocean_assert(!soundId.empty());

	if (soundId.empty())
	{
		return SoundScopedSubscription();
	}

	const IO::Files resolvedFiles = IO::FileResolver::get().resolve(IO::File(soundFile), true);

	if (resolvedFiles.empty())
	{
		Log::info() << "Failed to load sound '" << soundFile << "'";
		return SoundScopedSubscription();
	}

	const IO::File& file = resolvedFiles.front();

	const ScopedLock scopedLock(lock_);

	SoundMap::iterator iSound = soundMap_.end();

	const FileMap::const_iterator iFile = fileMap_.find(file());
	if (iFile != fileMap_.cend())
	{
		if (iFile->second != soundId)
		{
			Log::error() << "The sound id '" << soundId << "' is already used for a different sound";

			ocean_assert(false && "Invalid sound id");
			return SoundScopedSubscription();
		}

		iSound = soundMap_.find(iFile->second);

		ocean_assert(iSound != soundMap_.cend());
		if (iSound == soundMap_.cend())
		{
			return SoundScopedSubscription();
		}

		iSound->second.incrementUsage();
	}
	else
	{
		Sound sound(file());

		if (!sound.isValid())
		{
			return SoundScopedSubscription();
		}

		fileMap_.emplace(file(), soundId);

		ocean_assert(soundMap_.find(soundId) == soundMap_.cend());
		iSound = soundMap_.emplace(soundId, std::move(sound)).first;
	}

	return SoundScopedSubscription(iSound->first, std::bind(&SoundManager::unsubscribe, this, std::placeholders::_1));
}

bool SoundManager::play(const SoundId& soundId, const float intensity)
{
	ocean_assert(!soundId.empty());
	ocean_assert(intensity >= 0.0f && intensity <= 1.0f);

	const ScopedLock scopedLock(lock_);

	const SoundMap::iterator iAudio = soundMap_.find(soundId);
	if (iAudio == soundMap_.cend())
	{
		Log::error() << "Unknown sound id '" << soundId << "'";

		ocean_assert(false && "Unknown sound id");
		return false;
	}

	return iAudio->second.play(intensity);
}

void SoundManager::unsubscribe(const SoundId& soundId)
{
	const ScopedLock scopedLock(lock_);

	SoundMap::iterator iAudio = soundMap_.find(soundId);
	if (iAudio == soundMap_.cend())
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	if (iAudio->second.decrementUsage())
	{
		const FileMap::const_iterator iFile = fileMap_.find(iAudio->second.soundFile());

		ocean_assert(iFile != fileMap_.cend());
		if (iFile != fileMap_.cend())
		{
			fileMap_.erase(iFile);
		}

		soundMap_.erase(iAudio);
	}
}

}

}
