/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/ImageFileSequence.h"

#include "ocean/io/File.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace Media
{

ImageFileSequence::ImageFileSequence(const std::string& url) :
	Medium(url),
	FiniteMedium(url),
	FrameMedium(url),
	ImageSequence(url),
	Thread("ImageFileSequence Thread")
{
	// nothing to do here
}

ImageFileSequence::~ImageFileSequence()
{
	stopThreadExplicitly();
}

double ImageFileSequence::duration() const
{
	const ScopedLock scopedLock(lock_);

	return double(images() * double(preferredFrameType_.frequency()));
}

double ImageFileSequence::normalDuration() const
{
	const ScopedLock scopedLock(lock_);

	return double(images());
}

double ImageFileSequence::position() const
{
	const ScopedLock scopedLock(lock_);

	if (NumericD::isEqualEps(preferredFrameType_.frequency()))
	{
		return double(mediumFrameIndex - mediumFrameStartIndex);
	}

	return double(mediumFrameIndex - mediumFrameStartIndex) / double(preferredFrameType_.frequency());
}

float ImageFileSequence::speed() const
{
	const ScopedLock scopedLock(lock_);

	return float(preferredFrameType_.frequency());
}

std::string ImageFileSequence::currentUrl() const
{
	return imageFilename(index());
}

unsigned int ImageFileSequence::images() const
{
	const ScopedLock scopedLock(lock_);

	if (isValid_)
	{
		if (mediumImages == (unsigned int)(-1))
		{
			unsigned int index = mediumFrameStartIndex;

			while (IO::File(imageFilename(index)).exists())
			{
				++index;
			}

			mediumImages = index - mediumFrameStartIndex;

			return mediumImages;
		}

		return mediumImages;
	}

	return 0u;
}

bool ImageFileSequence::start()
{
	const ScopedLock scopedLock(lock_);

	if (!isValid_)
	{
		return false;
	}

	if (mediumStartTimestamp.isValid())
	{
		return false;
	}

	if (mediumSequenceMode == SM_EXPLICIT)
	{
		mediumExplicitSequenceModeStarted = true;

		// try to load the next frame
		IO::File nextFile(imageFilename(mediumFrameIndex));

		if (!nextFile.exists())
		{
			return false;
		}

		if (!loadImage(nextFile(), Timestamp(true), &mediumNextFrame))
		{
			return false;
		}

		deliverNewFrame(std::move(mediumNextFrame));

		mediumStartTimestamp.toNow();
		mediumPauseTimestamp.toInvalid();
		mediumStopTimestamp.toInvalid();

		return true;
	}

	if (isThreadActive())
	{
		return true;
	}

	if (startThread())
	{
		mediumStartTimestamp.toNow();
		mediumPauseTimestamp.toInvalid();
		mediumStopTimestamp.toInvalid();

		return true;
	}

	return false;
}

bool ImageFileSequence::pause()
{
	const ScopedLock scopedLock(lock_);

	if (mediumSequenceMode == SM_EXPLICIT)
	{
		mediumExplicitSequenceModeStarted = false;
		return true;
	}

	stopThread();
	return true;
}

bool ImageFileSequence::stop()
{
	const ScopedLock scopedLock(lock_);

	if (mediumSequenceMode == SM_EXPLICIT)
	{
		mediumExplicitSequenceModeStarted = false;
		return true;
	}

	stopThread();

	mediumFrameIndex = mediumFrameStartIndex;
	mediumStartTimestamp.toInvalid();
	mediumPauseTimestamp.toInvalid();
	mediumStopTimestamp.toNow();

	mediumNextFrame.release();

	return true;
}

bool ImageFileSequence::isStarted() const
{
	return mediumExplicitSequenceModeStarted || isThreadActive();
}

Timestamp ImageFileSequence::startTimestamp() const
{
	return mediumStartTimestamp;
}

Timestamp ImageFileSequence::pauseTimestamp() const
{
	return mediumPauseTimestamp;
}

Timestamp ImageFileSequence::stopTimestamp() const
{
	return mediumStopTimestamp;
}

bool ImageFileSequence::setPosition(const double position)
{
	const ScopedLock scopedLock(lock_);

	int frameIndex = NumericD::round32(position * preferredFrameType_.frequency());
	if (preferredFrameType_.frequency() == 0)
	{
		frameIndex = NumericD::round32(position);
	}

	if (frameIndex < 0 && frameIndex >= int(images()))
	{
		return false;
	}

	mediumFrameIndex = (unsigned int)(frameIndex);
	return true;
}

bool ImageFileSequence::setSpeed(const float speed)
{
	return setPreferredFrameFrequency(double(speed));
}

bool ImageFileSequence::forceNextFrame()
{
	if (mediumSequenceMode != SM_EXPLICIT)
	{
		return false;
	}

	if (!mediumExplicitSequenceModeStarted)
	{
		return false;
	}

	if (!isValid_)
	{
		return false;
	}

	// try to load the next frame
	++mediumFrameIndex;
	IO::File nextFile(imageFilename(mediumFrameIndex));

	if (!nextFile.exists())
	{
		if (!loop_)
		{
			mediumStartTimestamp.toInvalid();
			mediumPauseTimestamp.toInvalid();
			mediumStopTimestamp.toNow();
			return false;
		}

		mediumFrameIndex = mediumFrameStartIndex;
		nextFile = IO::File(imageFilename(mediumFrameIndex));
	}

	if (!loadImage(nextFile(), Timestamp(true), &mediumNextFrame))
	{
		return false;
	}

	return deliverNewFrame(std::move(mediumNextFrame));
}

void ImageFileSequence::threadRun()
{
	Timestamp nextFrameTimestamp;

	while (!shouldThreadStop())
	{
		Timestamp timestamp(true);

		// wait for the next frame
		if (nextFrameTimestamp.isValid() && timestamp < nextFrameTimestamp)
		{
			while (timestamp < nextFrameTimestamp && !shouldThreadStop())
			{
				const double waitTime = double(nextFrameTimestamp - timestamp);

				if (waitTime > 0.01)
				{
					sleep(1u);
				}
				else
				{
					sleep(0u);
				}

				timestamp.toNow();
			}
		}

		if (shouldThreadStop())
		{
			break;
		}

		const ScopedLock scopedLock(lock_);

		// use already loaded frame or load the next frame explicitly
		if (mediumNextFrame)
		{
			deliverNewFrame(std::move(mediumNextFrame));
		}
		else
		{
			if (!loadImage(imageFilename(mediumFrameIndex), timestamp))
			{
				break;
			}
		}

		// try to load the next frame
		++mediumFrameIndex;
		const IO::File nextFile(imageFilename(mediumFrameIndex));

		if (nextFile.exists())
		{
			if (!loadImage(nextFile(), timestamp, &mediumNextFrame))
			{
				break;
			}
		}
		else
		{
			mediumFrameIndex = mediumFrameStartIndex;

			if (!loadImage(imageFilename(mediumFrameIndex), timestamp, &mediumNextFrame))
			{
				break;
			}

			if (!loop_)
			{
				break;
			}
		}

		if (preferredFrameType_.frequency() <= NumericD::eps())
		{
			mediumPauseTimestamp = timestamp;
		}
		else
		{
			nextFrameTimestamp = timestamp + 1.0 / preferredFrameType_.frequency();
		}
	}

	mediumStopTimestamp.toNow();
}

bool ImageFileSequence::determineSequence()
{
	const IO::File file(url_);

	if (!file.exists())
	{
		return false;
	}

	const std::string fileBase = file.base();
	if (fileBase.empty())
	{
		return false;
	}

	ocean_assert(fileBase.length() > 0u);
	std::string::size_type pos = fileBase.length() - 1u;

	while (isdigit((unsigned char)fileBase[pos]) && pos > 0u)
	{
		--pos;
	}

	if (!isdigit((unsigned char)fileBase[pos]))
	{
		++pos;
	}

	const std::string digits = fileBase.substr(pos);
	if (digits.empty())
	{
		return false;
	}

	mediumFilenameIndexLength = (unsigned int)digits.length();
	mediumFrameStartIndex = atoi(digits.c_str());
	mediumFrameIndex = mediumFrameStartIndex;

	mediumFilenamePrefix = fileBase.substr(0u, pos);
	mediumFilenameType = file.extension();

	return true;
}

std::string ImageFileSequence::imageFilename(const unsigned int index) const
{
	const std::string numberString(String::toAString(index));
	const std::string filename(mediumFilenamePrefix + std::string(max(0, int(mediumFilenameIndexLength - numberString.size())), '0') + numberString + std::string(".") + mediumFilenameType);

	return filename;
}

}

}
