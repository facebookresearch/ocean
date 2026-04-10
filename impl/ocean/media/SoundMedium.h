/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_SOUND_MEDIUM_H
#define META_OCEAN_MEDIA_SOUND_MEDIUM_H

#include "ocean/media/Media.h"
#include "ocean/media/Medium.h"
#include "ocean/media/MediumRef.h"

#include "ocean/base/Timestamp.h"

namespace Ocean
{

namespace Media
{

// Forward declaration.
class SoundMedium;

/**
 * Definition of a smart medium reference holding a sound medium object.
 * @see SmartMediumRef, SoundMedium.
 * @ingroup media
 */
using SoundMediumRef = SmartMediumRef<SoundMedium>;

/**
 * This class is the base class for all sound mediums.
 * @ingroup media
 */
class OCEAN_MEDIA_EXPORT SoundMedium : public virtual Medium
{
	public:

		/**
		 * Definition of a sound frequency defined in Hz.
		 */
		using SoundFrequency = float;

		/**
		 * Definition of a sound type composed by the sound frequency and channels.
		 */
		class OCEAN_MEDIA_EXPORT SoundType
		{
			public:

				/**
				 * Creates a new sound type with invalid parameters.
				 */
				inline SoundType();

				/**
				 * Creates a new sound type.
				 * @param frequency Sound frequency
				 * @param channels Sound channels
				 * @param bitsPerSample Bits per sound sample
				 */
				inline SoundType(const SoundFrequency frequency, const unsigned int channels, const unsigned int bitsPerSample);

				/**
				 * Returns the frequency of the sound format in Hz.
				 * @return Sound frequency
				 */
				inline SoundFrequency frequency() const;

				/**
				 * Returns the channels of the sound format.
				 * @return Sound channels
				 */
				inline unsigned int channels() const;

				/**
				 * Returns the bits per sound sample.
				 * @return Bits per sample
				 */
				inline unsigned int bitsPerSample() const;

				/**
				 * Sets the frequency of the sound format in Hz.
				 * @param frequency Sound frequency
				 */
				inline void setFrequency(const SoundFrequency frequency);

				/**
				 * Sets the channels of the sound format.
				 * @param channels Sound channels
				 */
				inline void setChannels(const unsigned int channels);

				/**
				 * Sets the bits per sound sample.
				 * @param bits Bits per sound sample
				 */
				inline void setBitsPerSoundSample(const unsigned int bits);

				/**
				 * Returns whether two sound types are equal.
				 * @param right Right sound type
				 * @return True, if so
				 */
				inline bool operator==(const SoundType& right) const;

				/**
				 * Returns whether the left sound type is 'smaller' than the right one.
				 * @param right Right sound type
				 * @return True, if so
				 */
				inline bool operator<(const SoundType& right) const;

			private:

				/// Sound frequency in Hz.
				SoundFrequency frequency_;

				/// Sound channels.
				unsigned int channels_;

				/// Bits per sound sample.
				unsigned int bitsPerSample_;
		};

	protected:

		/**
		 * Class allowing the sorting of several sound media types according their popularity.
		 */
		class OCEAN_MEDIA_EXPORT SortableSoundType
		{
			public:

				/**
				 * Creates a new sortable sound media type.
				 * @param soundType Preferable sound type
				 */
				SortableSoundType(const SoundType& soundType);

				/**
				 * Creates a new sortable sound type.
				 * @param frequency Preferable reference frequency
				 * @param channels Preferable reference channels
				 * @param bitsPerSample Preferable reference bits per sample
				 */
				SortableSoundType(const SoundFrequency frequency, const unsigned int channels, const unsigned int bitsPerSample);

				/**
				 * Returns whether the left sortable sound type is more prefered than the right one.
				 * @param right Right sortable sound type
				 * @return True, if so
				 */
				bool operator<(const SortableSoundType& right) const;

			protected:

				/// Actual sound type.
				SoundType actualSoundType_;

				/// Preferable sound type.
				SoundType preferableSoundType_;
		};

	public:

		/**
		 * Returns whether the object holds a sound.
		 * @return True, if so
		 */
		inline bool hasSound() const;

		/**
		 * Returns the number of sound channels.
		 * @return Number of channels
		 */
		inline unsigned int soundChannels() const;

		/**
		 * Returns the frequency of the sound in Hz.
		 * @return Sound frequency in Hz
		 */
		inline SoundFrequency soundFrequency() const;

		/**
		 * Returns the number of bits per sample.
		 * @return Number of bits per sample
		 */
		inline unsigned int soundBitsPerSample() const;

		/**
		 * Returns the volume of the sound in db.
		 * @return Sound volume in db.
		 * @see setSoundVolume().
		 */
		virtual float soundVolume() const = 0;

		/**
		 * Returns whether the sound medium is in a mute state.
		 * @return True, if so
		 * @see setSoundMute().
		 */
		virtual bool soundMute() const = 0;

		/**
		 * Returns the preferred number of sound channels.
		 * @return Preferred sound channels
		 */
		inline unsigned int preferredSoundChannels() const;

		/**
		 * Returns the preferred sound frequency in Hz.
		 * @return Preferred sound frequency
		 */
		inline SoundFrequency preferredSoundFrequency() const;

		/**
		 * Returns the preferred bits per sound sample.
		 * @return Preferred bits per sound sample
		 */
		inline unsigned int preferredSoundBitsPerSample() const;

		/**
		 * Sets the volume of the sound in db.
		 * The volume must be in range [-100db, 0db]
		 * @param volume New volume in db
		 * @return True, if succeeded
		 * @see soundVolume().
		 */
		virtual bool setSoundVolume(const float volume) = 0;

		/**
		 * Sets or unsets the sound medium to a mute state.
		 * @param mute True, to mute the sound
		 * @return True, if succeeded
		 * @see soundMute().
		 */
		virtual bool setSoundMute(const bool mute) = 0;

		/**
		 * Sets the preferred number of sound channels.
		 * @param channels Preferred sound channels
		 * @return True, if succeeded
		 */
		virtual bool setPreferredSoundChannels(const unsigned int channels);

		/**
		 * Sets the preferred sound frequency in Hz.
		 * @param frequency Preferred sound frequency
		 * @return True, if succeeded
		 */
		virtual bool setPreferredSoundFrequency(const SoundFrequency frequency);

		/**
		 * Sets the preferred bits per sound sample.
		 * @param bits Preferred bits per sound sample
		 * @return True, if succeeded
		 */
		virtual bool setPreferredSoundBitsPerSample(const unsigned int bits);

	protected:

		/**
		 * Creates a new sound medium by a given url.
		 * @param url Url of the sound medium
		 */
		explicit SoundMedium(const std::string& url);

	protected:

		/// Actual sound type.
		SoundType soundType_;

		/// Preferred sound type.
		SoundType preferredSoundType_;

		/// Timestamp of the recent sound frame.
		Timestamp soundTimestamp_;

		/// Timestamp of the recent sound frame type.
		Timestamp soundTypeTimestamp_;
};

inline SoundMedium::SoundType::SoundType() :
	frequency_(0),
	channels_(0),
	bitsPerSample_(0)
{
	// nothing to do here
}

inline SoundMedium::SoundType::SoundType(const SoundFrequency frequency, const unsigned int channels, const unsigned int bitsPerSample) :
	frequency_(frequency),
	channels_(channels),
	bitsPerSample_(bitsPerSample)
{
	// nothing to do here
}

inline SoundMedium::SoundFrequency SoundMedium::SoundType::frequency() const
{
	return frequency_;
}

inline unsigned int SoundMedium::SoundType::channels() const
{
	return channels_;
}

inline unsigned int SoundMedium::SoundType::bitsPerSample() const
{
	return bitsPerSample_;
}

inline void SoundMedium::SoundType::setFrequency(const SoundFrequency frequency)
{
	frequency_ = frequency;
}

inline void SoundMedium::SoundType::setChannels(const unsigned int channels)
{
	channels_ = channels;
}

inline void SoundMedium::SoundType::setBitsPerSoundSample(const unsigned int bits)
{
	bitsPerSample_ = bits;
}

inline bool SoundMedium::SoundType::operator==(const SoundType& right) const
{
	return channels_ == right.channels_ && frequency_ == right.frequency_
		&& bitsPerSample_ == right.bitsPerSample_;
}

inline bool SoundMedium::SoundType::operator<(const SoundType& right) const
{
	if (channels_ < right.channels_)
	{
		return true;
	}

	if (channels_ > right.channels_)
	{
		return false;
	}

	if (frequency_ < right.frequency_)
	{
		return true;
	}

	if (frequency_ > right.frequency_)
	{
		return false;
	}

	return bitsPerSample_ < right.bitsPerSample_;
}

inline bool SoundMedium::hasSound() const
{
	return soundTimestamp_.isInvalid() == false;
}

inline unsigned int SoundMedium::soundChannels() const
{
	return soundType_.channels();
}

inline SoundMedium::SoundFrequency SoundMedium::soundFrequency() const
{
	return soundType_.frequency();
}

inline unsigned int SoundMedium::soundBitsPerSample() const
{
	return soundType_.bitsPerSample();
}

inline unsigned int SoundMedium::preferredSoundChannels() const
{
	return preferredSoundType_.channels();
}

inline SoundMedium::SoundFrequency SoundMedium::preferredSoundFrequency() const
{
	return preferredSoundType_.frequency();
}

inline unsigned int SoundMedium::preferredSoundBitsPerSample() const
{
	return preferredSoundType_.bitsPerSample();
}

}

}

#endif // META_OCEAN_MEDIA_SOUND_MEDIUM_H
