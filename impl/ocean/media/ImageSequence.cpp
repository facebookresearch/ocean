/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/ImageSequence.h"

#include "ocean/io/File.h"

#include "ocean/math/Numeric.h"

#include "ocean/system/Performance.h"

namespace Ocean
{

namespace Media
{

ImageSequence::ImageSequence(const std::string& url) :
	Medium(url),
	FiniteMedium(url),
	FrameMedium(url),
	mediumFrameStartIndex(0u),
	mediumFrameIndex(0u),
	mediumImages((unsigned int)(-1)),
	mediumFilenameIndexLength(0u),
	mediumSequenceMode(SM_AUTOMATIC),
	mediumExplicitSequenceModeStarted(false)
{
	type_ = Type(type_ | IMAGE_SEQUENCE);

	if (System::Performance::get().performanceLevel() > System::Performance::LEVEL_MEDIUM)
	{
		frameCollection_ = FrameCollection(10u);
	}
	else
	{
		frameCollection_ = FrameCollection(5u);
	}

	setPreferredFrameFrequency(1.0);
}

ImageSequence::~ImageSequence()
{
	// nothing to do here
}

ImageSequence::SequenceMode ImageSequence::mode() const
{
	return mediumSequenceMode;
}

unsigned int ImageSequence::index() const
{
	return mediumFrameIndex;
}

bool ImageSequence::setMode(const SequenceMode mode)
{
	if (isStarted())
	{
		ocean_assert(!isStarted());
		return false;
	}

	mediumSequenceMode = mode;
	return true;
}

bool ImageSequence::setPreferredFramePixelFormat(const FrameType::PixelFormat format)
{
	if (format == preferredFrameType_.pixelFormat())
	{
		return true;
	}

	const ScopedLock scopedLock(lock_);

	preferredFrameType_ = MediaFrameType(preferredFrameType_, format);

	return true;
}

bool ImageSequence::setPreferredFrameFrequency(const FrameFrequency frequency)
{
	if (frequency < 0 || frequency > 10000)
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	preferredFrameType_.setFrequency(frequency);
	recentFrameFrequency_ = frequency;

	return true;
}

}

}
