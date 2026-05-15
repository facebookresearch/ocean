/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/ImageFileSequence.h"

#include "ocean/io/Directory.h"
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
		return double(frameIndex_ - frameStartIndex_);
	}

	return double(frameIndex_ - frameStartIndex_) / double(preferredFrameType_.frequency());
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
		if (images_ == (unsigned int)(-1))
		{
			unsigned int index = frameStartIndex_;

			while (IO::File(imageFilename(index)).exists())
			{
				++index;
			}

			images_ = index - frameStartIndex_;

			return images_;
		}

		return images_;
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

	if (startTimestamp_.isValid())
	{
		return false;
	}

	if (sequenceMode_ == SM_EXPLICIT)
	{
		explicitSequenceModeStarted_ = true;

		// try to load the next frame
		IO::File nextFile(imageFilename(frameIndex_));

		if (!nextFile.exists())
		{
			return false;
		}

		if (!loadImage(nextFile(), Timestamp(true), &nextFrame_))
		{
			return false;
		}

		deliverNewFrame(std::move(nextFrame_), SharedAnyCamera(camera_));

		startTimestamp_.toNow();
		pauseTimestamp_.toInvalid();
		stopTimestamp_.toInvalid();

		return true;
	}

	if (isThreadActive())
	{
		return true;
	}

	if (startThread())
	{
		startTimestamp_.toNow();
		pauseTimestamp_.toInvalid();
		stopTimestamp_.toInvalid();

		return true;
	}

	return false;
}

bool ImageFileSequence::pause()
{
	const ScopedLock scopedLock(lock_);

	if (sequenceMode_ == SM_EXPLICIT)
	{
		explicitSequenceModeStarted_ = false;
		return true;
	}

	stopThread();
	return true;
}

bool ImageFileSequence::stop()
{
	const ScopedLock scopedLock(lock_);

	if (sequenceMode_ == SM_EXPLICIT)
	{
		explicitSequenceModeStarted_ = false;
		return true;
	}

	stopThread();

	frameIndex_ = frameStartIndex_;
	startTimestamp_.toInvalid();
	pauseTimestamp_.toInvalid();
	stopTimestamp_.toNow();

	nextFrame_.release();

	return true;
}

bool ImageFileSequence::isStarted() const
{
	return explicitSequenceModeStarted_ || isThreadActive();
}

Timestamp ImageFileSequence::startTimestamp() const
{
	return startTimestamp_;
}

Timestamp ImageFileSequence::pauseTimestamp() const
{
	return pauseTimestamp_;
}

Timestamp ImageFileSequence::stopTimestamp() const
{
	return stopTimestamp_;
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

	frameIndex_ = (unsigned int)(frameIndex);
	return true;
}

bool ImageFileSequence::setSpeed(const float speed)
{
	return setPreferredFrameFrequency(double(speed));
}

bool ImageFileSequence::setCamera(SharedAnyCamera&& camera)
{
	const ScopedLock scopedLock(lock_);

	camera_ = std::move(camera);

	return true;
}

bool ImageFileSequence::forceNextFrame()
{
	const ScopedLock scopedLock(lock_);

	if (sequenceMode_ != SM_EXPLICIT)
	{
		return false;
	}

	if (!explicitSequenceModeStarted_)
	{
		return false;
	}

	if (!isValid_)
	{
		return false;
	}

	// try to load the next frame
	++frameIndex_;
	IO::File nextFile(imageFilename(frameIndex_));

	if (!nextFile.exists())
	{
		if (!loop_)
		{
			startTimestamp_.toInvalid();
			pauseTimestamp_.toInvalid();
			stopTimestamp_.toNow();
			return false;
		}

		frameIndex_ = frameStartIndex_;
		nextFile = IO::File(imageFilename(frameIndex_));
	}

	if (!loadImage(nextFile(), Timestamp(true), &nextFrame_))
	{
		return false;
	}

	return deliverNewFrame(std::move(nextFrame_), SharedAnyCamera(camera_));
}

bool ImageFileSequence::isFileSequence(const std::string& filename, bool* isIndividualImage)
{
	if (isIndividualImage != nullptr)
	{
		*isIndividualImage = false;
	}

	const IO::File file(filename);

	if (!file.exists())
	{
		return false;
	}

	const std::string baseName = file.baseName();
	if (baseName.empty())
	{
		return false;
	}

	ocean_assert(baseName.length() > 0u);
	std::string::size_type pos = baseName.length() - 1u;

	while (std::isdigit((unsigned char)(baseName[pos])) && pos > 0u)
	{
		--pos;
	}

	if (!std::isdigit((unsigned char)(baseName[pos])))
	{
		++pos;
	}

	const std::string digits = baseName.substr(pos);
	if (digits.empty())
	{
		if (isIndividualImage != nullptr)
		{
			*isIndividualImage = true;
		}

		return false;
	}

	const unsigned int startIndex = (unsigned int)(atoi(digits.c_str()));
	const unsigned int nextIndex = startIndex + 1u;

	const unsigned int indexLength = (unsigned int)(digits.length());

	const std::string nextNumberString(String::toAString(nextIndex, indexLength));

	const std::string filenamePrefix = baseName.substr(0u, pos);
	const std::string filenameType = file.extension();

	IO::File nextFilename = IO::Directory(file) + IO::File(filenamePrefix + nextNumberString + std::string(".") + filenameType);

	if (IO::File(nextFilename).exists())
	{
		return true;
	}

	if (isIndividualImage != nullptr)
	{
		*isIndividualImage = true;
	}

	return false;
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
		if (nextFrame_)
		{
			deliverNewFrame(std::move(nextFrame_), SharedAnyCamera(camera_));
		}
		else
		{
			if (!loadImage(imageFilename(frameIndex_), timestamp))
			{
				break;
			}
		}

		// try to load the next frame
		++frameIndex_;
		const IO::File nextFile(imageFilename(frameIndex_));

		if (nextFile.exists())
		{
			if (!loadImage(nextFile(), timestamp, &nextFrame_))
			{
				break;
			}
		}
		else
		{
			frameIndex_ = frameStartIndex_;

			if (!loadImage(imageFilename(frameIndex_), timestamp, &nextFrame_))
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
			pauseTimestamp_ = timestamp;
		}
		else
		{
			nextFrameTimestamp = timestamp + 1.0 / preferredFrameType_.frequency();
		}
	}

	stopTimestamp_.toNow();
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

	filenameIndexLength_ = (unsigned int)digits.length();
	frameStartIndex_ = atoi(digits.c_str());
	frameIndex_ = frameStartIndex_;

	filenamePrefix_ = fileBase.substr(0u, pos);
	filenameType_ = file.extension();

	return true;
}

std::string ImageFileSequence::imageFilename(const unsigned int index) const
{
	const std::string numberString(String::toAString(index));
	const std::string filename(filenamePrefix_ + std::string(max(0, int(filenameIndexLength_ - numberString.size())), '0') + numberString + std::string(".") + filenameType_);

	return filename;
}

}

}
