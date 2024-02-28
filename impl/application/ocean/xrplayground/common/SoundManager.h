// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_SOUND_MANAGER_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_SOUND_MANAGER_H

#include "application/ocean/xrplayground/XRPlayground.h"

#include "ocean/base/Lock.h"
#include "ocean/base/ScopedSubscription.h"
#include "ocean/base/Singleton.h"

#include "ocean/media/Audio.h"

namespace Ocean
{

namespace XRPlayground
{

/**
 * This class implements a manager for sounds simplifying the usage of sounds in an experience.
 * @ingroup xrplayground
 */
class SoundManager : public Singleton<SoundManager>
{
	friend class Singleton<SoundManager>;

	public:

		/**
		 * Definition of a unique sound id.
		 */
		typedef std::string SoundId;

		/**
		 * Definition of a scoped subscription object for sounds.
		 */
		typedef ScopedSubscription<SoundId, SoundManager> SoundScopedSubscription;

		/**
		 * Definition of a vector holding SoundScopedSubscription objects.
		 */
		typedef std::vector<SoundScopedSubscription> SoundScopedSubscriptions;

	protected:

		/**
		 * Definition of a map mapping file names to sound ids.
		 */
		typedef std::unordered_map<std::string, SoundId> FileMap;

		/**
		 * This class holds the relevant data to play a sound.
		 */
		class Sound
		{
			protected:

				/**
				 * Definition of a vector holding audios.
				 */
				typedef std::vector<Media::AudioRef> Audios;

			public:

				/**
				 * Creates a new sound object and sets the usage counter to 1.
				 * @param soundFile The file of the sound, must be valid
				 * @see isValid().
				 */
				explicit Sound(const std::string& soundFile);

				/**
				 * Plays the sound.
				 * @param intensity The sound/volume intensity, with range [0, 1]
				 * @return True, if succeeded
				 */
				bool play(const float intensity = 1.0f);

				/**
				 * Returns the sound file of the sound.
				 * @return The sound's file
				 */
				inline const std::string& soundFile() const;

				/**
				 * Increments the usage counter.
				 */
				void incrementUsage();

				/**
				 * Decrements the usage counter.
				 * @return True, if the usage counter is 0
				 */
				bool decrementUsage();

				/**
				 * Returns whether this object is valid
				 * @return True, if so
				 */
				bool isValid() const;

			protected:

				/**
				 * Plays an audio object with given intensity.
				 * @param audio The audio object, must be valid
				 * @param intensity The intensity to be used, with range [0, 1]
				 * @return True, if succeeded
				 */
				static bool play(const Media::AudioRef& audio, const float intensity);

			protected:

				/// The sound file.
				std::string soundFile_;

				/// The audio objects of the same sound file.
				Audios audios_;

				/// The usage counter.
				unsigned int usageCounter_ = 0u;
		};

		/// Definition of an unordered map mapping sound ids to sound objects.
		typedef std::unordered_map<SoundId, Sound> SoundMap;

	public:

		/**
		 * Registers/subscribes a new sound.
		 * @param soundFile The file of the sound to register, will be resolved internally
		 * @param soundId A unique id to identify the sound
		 * @return The sound subscription object, the sound is registered/subscribed as long as this object exist, invalid in case of a failure
		 */
		[[nodiscard]] SoundScopedSubscription subscribe(const std::string& soundFile, const SoundId& soundId);

		/**
		 * Plays a specified sound.
		 * @param soundId The identifier of the sound to play
		 * @param intensity The sound intensity/volume to be used, with range [0, 1]
		 * @return True, if succeeded
		 */
		bool play(const SoundId& soundId, const float intensity = 1.0f);

	protected:

		/**
		 * Unsubscribes a sound.
		 * @param soundId The id of the audio to unsubscribe
		 */
		void unsubscribe(const SoundId& soundId);

	protected:

		/// The map mapping sound files to sound identifiers.
		SoundMap soundMap_;

		/// The map mapping sound identifiers to sound objects.
		FileMap fileMap_;

		/// The manager's lock.
		Lock lock_;
};

inline const std::string& SoundManager::Sound::soundFile() const
{
	return soundFile_;
}

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_SOUND_MANAGER_H
