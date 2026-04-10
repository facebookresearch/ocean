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
	preferableSoundType_(soundType)
{
	// nothing to do here
}

SoundMedium::SortableSoundType::SortableSoundType(const SoundFrequency frequency, const unsigned int channels, const unsigned int bitsPerSample) :
	preferableSoundType_(frequency, channels, bitsPerSample)
{
	// nothing to do here
}

bool SoundMedium::SortableSoundType::operator<(const SortableSoundType& right) const
{
	ocean_assert(preferableSoundType_ == right.preferableSoundType_);

	if (preferableSoundType_.channels() != 0)
	{
		int leftDifference = abs(int(preferableSoundType_.channels()) - int(actualSoundType_.channels()));
		int rightDifference = abs(int(preferableSoundType_.channels()) - int(right.actualSoundType_.channels()));

		if (leftDifference < rightDifference)
			return true;
		if (rightDifference < leftDifference)
			return false;
	}

	if (preferableSoundType_.frequency() != 0)
	{
		SoundFrequency leftDifference = NumericT<SoundFrequency>::abs(preferableSoundType_.frequency() - actualSoundType_.frequency());
		SoundFrequency rightDifference = NumericT<SoundFrequency>::abs(preferableSoundType_.frequency() - right.actualSoundType_.frequency());

		if (leftDifference < rightDifference)
			return true;
		if (rightDifference < leftDifference)
			return false;
	}

	return false;
}

SoundMedium::SoundMedium(const std::string& url) :
	Medium(url),
	soundType_(),
	preferredSoundType_(),
	soundTimestamp_(),
	soundTypeTimestamp_()
{
	type_ = Type(type_ | SOUND_MEDIUM);
}

bool SoundMedium::setPreferredSoundChannels(const unsigned int channels)
{
	preferredSoundType_.setChannels(channels);
	return true;
}

bool SoundMedium::setPreferredSoundFrequency(const SoundFrequency frequency)
{
	if (frequency < 0 || frequency > 1000000)
		return false;

	preferredSoundType_.setFrequency(frequency);
	return true;
}

bool SoundMedium::setPreferredSoundBitsPerSample(const unsigned int bits)
{
	preferredSoundType_.setBitsPerSoundSample(bits);
	return true;
}

}

}
