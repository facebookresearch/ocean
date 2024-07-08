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
typedef SmartMediumRef<SoundMedium> SoundMediumRef;

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
		typedef float SoundFrequency;

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
				SoundFrequency typeFrequency;

				/// Sound channels.
				unsigned int typeChannels;

				/// Bits per sound sample.
				unsigned int typeBitsPerSample;
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
				SoundType actualSoundType;

				/// Preferable sound type.
				SoundType preferableSoundType;
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
		SoundType mediumSoundType;

		/// Preferred sound type.
		SoundType mediumPreferredSoundType;

		/// Timestamp of the recent sound frame.
		Timestamp mediumSoundTimestamp;

		/// Timestamp of the recent sound frame type.
		Timestamp mediumSoundTypeTimestamp;
};

inline SoundMedium::SoundType::SoundType() :
	typeFrequency(0),
	typeChannels(0),
	typeBitsPerSample(0)
{
	// nothing to do here
}

inline SoundMedium::SoundType::SoundType(const SoundFrequency frequency, const unsigned int channels, const unsigned int bitsPerSample) :
	typeFrequency(frequency),
	typeChannels(channels),
	typeBitsPerSample(bitsPerSample)
{
	// nothing to do here
}

inline SoundMedium::SoundFrequency SoundMedium::SoundType::frequency() const
{
	return typeFrequency;
}

inline unsigned int SoundMedium::SoundType::channels() const
{
	return typeChannels;
}

inline unsigned int SoundMedium::SoundType::bitsPerSample() const
{
	return typeBitsPerSample;
}

inline void SoundMedium::SoundType::setFrequency(const SoundFrequency frequency)
{
	typeFrequency = frequency;
}

inline void SoundMedium::SoundType::setChannels(const unsigned int channels)
{
	typeChannels = channels;
}

inline void SoundMedium::SoundType::setBitsPerSoundSample(const unsigned int bits)
{
	typeBitsPerSample = bits;
}

inline bool SoundMedium::SoundType::operator==(const SoundType& right) const
{
	return typeChannels == right.typeChannels && typeFrequency == right.typeFrequency
		&& typeBitsPerSample == right.typeBitsPerSample;
}

inline bool SoundMedium::SoundType::operator<(const SoundType& right) const
{
	if (typeChannels < right.typeChannels)
		return true;
	if (typeChannels > right.typeChannels)
		return false;

	if (typeFrequency < right.typeFrequency)
		return true;
	if (typeFrequency > right.typeFrequency)
		return false;

	return typeBitsPerSample < right.typeBitsPerSample;
}

inline bool SoundMedium::hasSound() const
{
	return mediumSoundTimestamp.isInvalid() == false;
}

inline unsigned int SoundMedium::soundChannels() const
{
	return mediumSoundType.channels();
}

inline SoundMedium::SoundFrequency SoundMedium::soundFrequency() const
{
	return mediumSoundType.frequency();
}

inline unsigned int SoundMedium::soundBitsPerSample() const
{
	return mediumSoundType.bitsPerSample();
}

inline unsigned int SoundMedium::preferredSoundChannels() const
{
	return mediumPreferredSoundType.channels();
}

inline SoundMedium::SoundFrequency SoundMedium::preferredSoundFrequency() const
{
	return mediumPreferredSoundType.frequency();
}

inline unsigned int SoundMedium::preferredSoundBitsPerSample() const
{
	return mediumPreferredSoundType.bitsPerSample();
}

}

}

#endif // META_OCEAN_MEDIA_SOUND_MEDIUM_H
