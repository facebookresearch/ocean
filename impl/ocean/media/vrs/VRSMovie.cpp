// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/vrs/VRSMovie.h"
#include "ocean/media/vrs/VRSLibrary.h"

namespace Ocean
{

namespace Media
{

namespace VRS
{

VRSMovie::VRSMovie(const std::string& url) :
	Medium(url),
	FiniteMedium(url),
	VRSMedium(url),
	VRSFiniteMedium(url),
	FrameMedium(url),
	VRSFrameMedium(url),
	SoundMedium(url),
	Movie(url)
{
	if (isValid_)
	{
		ocean_assert(recordFileReader_);
		recordFileReader_->setStreamPlayer(streamId_, &imagePlayable_);
	}
}

VRSMovie::~VRSMovie()
{
	stopThreadExplicitly();
}

bool VRSMovie::start()
{
	const ScopedLock scopedLock(lock_);

	if (startTimestamp_.isValid())
	{
		return true;
	}

	startTimestamp_.toNow();
	pauseTimestamp_.toInvalid();
	stopTimestamp_.toInvalid();

	previousUnixFrameTimestamp_.toInvalid();
	previousVRSFrameTimestamp_.toInvalid();

	startThread();

	return true;
}

bool VRSMovie::pause()
{
	return false;
}

bool VRSMovie::stop()
{
	const ScopedLock scopedLock(lock_);

	if (stopTimestamp_.isValid())
	{
		return true;
	}

	stopThread();

	return true;
}

MediumRef VRSMovie::clone() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid_);
	if (isValid_)
	{
		return VRSLibrary::newMovie(url_, true);
	}

	return MediumRef();
}

float VRSMovie::soundVolume() const
{
	return 0.0f;
}

bool VRSMovie::soundMute() const
{
	return true;
}

bool VRSMovie::setSoundVolume(const float volume)
{
	return false;
}

bool VRSMovie::setSoundMute(const bool mute)
{
	return false;
}

void VRSMovie::threadRun()
{
	ocean_assert(recordFileReader_);
	ocean_assert(streamId_.getInstanceId() != uint16_t(0));

	const std::vector<const vrs::IndexRecord::RecordInfo*> recordInfos = recordFileReader_->getIndex(streamId_);

	// first we need to read all configuration records

	for (size_t n = 0; n < recordInfos.size(); ++n)
	{
		if (recordInfos[n]->recordType == vrs::Record::Type::CONFIGURATION)
		{
			recordFileReader_->readRecord(*recordInfos[n], &imagePlayable_);
		}
	}

	size_t index = size_t(-1);

	{
		const ScopedLock scopedLock(lock_);

		ocean_assert(startTimestamp_.isValid());
		ocean_assert(stopTimestamp_.isInvalid());

		if (newDesiredPosition_ >= 0.0)
		{
			position_ = newDesiredPosition_;
			newDesiredPosition_ = -1.0;
		}

		index = findDataRecordInfoForTimestamp(recordInfos, Timestamp(position_ + double(firstFrameTimestamp_)));
		ocean_assert(index != size_t(-1));
	}

	while (index != size_t(-1) && shouldThreadStop() == false)
	{
		if (index >= recordInfos.size())
		{
			if (loop_)
			{
				position_ = 0.0;
				previousVRSFrameTimestamp_.toInvalid();

				index = findDataRecordInfoForTimestamp(recordInfos, Timestamp(position_ + double(firstFrameTimestamp_)));
			}
			else
			{
				break;
			}
		}

		recordFileReader_->readRecord(*recordInfos[index], &imagePlayable_);

		index++;

		const ScopedLock scopedLock(lock_);

		if (newDesiredPosition_ >= 0.0)
		{
			position_ = newDesiredPosition_;
			newDesiredPosition_ = -1.0;

			index = findDataRecordInfoForTimestamp(recordInfos, Timestamp(position_ + double(firstFrameTimestamp_)));
		}
		else if (index < recordInfos.size())
		{
			position_ = recordInfos[index]->timestamp - double(firstFrameTimestamp_);
			ocean_assert(position_ >= 0.0 && position_ < normalDuration_);
		}
	}

	const ScopedLock scopedLock(lock_);

	stopTimestamp_.toNow();
	startTimestamp_.toInvalid();
}

}

}

}
