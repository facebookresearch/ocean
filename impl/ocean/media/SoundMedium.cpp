/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/SoundMedium.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace Media
{

SoundMedium::SortableSoundType::SortableSoundType(const SoundType& soundType) :
	preferableSoundType(soundType)
{
	// nothing to do here
}

SoundMedium::SortableSoundType::SortableSoundType(const SoundFrequency frequency, const unsigned int channels, const unsigned int bitsPerSample) :
	preferableSoundType(frequency, channels, bitsPerSample)
{
	// nothing to do here
}

bool SoundMedium::SortableSoundType::operator<(const SortableSoundType& right) const
{
	ocean_assert(preferableSoundType == right.preferableSoundType);

	if (preferableSoundType.channels() != 0)
	{
		int leftDifference = abs(int(preferableSoundType.channels()) - int(actualSoundType.channels()));
		int rightDifference = abs(int(preferableSoundType.channels()) - int(right.actualSoundType.channels()));

		if (leftDifference < rightDifference)
			return true;
		if (rightDifference < leftDifference)
			return false;
	}

	if (preferableSoundType.frequency() != 0)
	{
		SoundFrequency leftDifference = NumericT<SoundFrequency>::abs(preferableSoundType.frequency() - actualSoundType.frequency());
		SoundFrequency rightDifference = NumericT<SoundFrequency>::abs(preferableSoundType.frequency() - right.actualSoundType.frequency());

		if (leftDifference < rightDifference)
			return true;
		if (rightDifference < leftDifference)
			return false;
	}

	return false;
}

SoundMedium::SoundMedium(const std::string& url) :
	Medium(url),
	mediumSoundType(),
	mediumPreferredSoundType(),
	mediumSoundTimestamp(),
	mediumSoundTypeTimestamp()
{
	type_ = Type(type_ | SOUND_MEDIUM);
}

bool SoundMedium::setPreferredSoundChannels(const unsigned int channels)
{
	mediumPreferredSoundType.setChannels(channels);
	return true;
}

bool SoundMedium::setPreferredSoundFrequency(const SoundFrequency frequency)
{
	if (frequency < 0 || frequency > 1000000)
		return false;

	mediumPreferredSoundType.setFrequency(frequency);
	return true;
}

bool SoundMedium::setPreferredSoundBitsPerSample(const unsigned int bits)
{
	mediumPreferredSoundType.setBitsPerSoundSample(bits);
	return true;
}

}

}
