// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/vrs/VRSImageSequence.h"
#include "ocean/media/vrs/VRSLibrary.h"

namespace Ocean
{

namespace Media
{

namespace VRS
{

VRSImageSequence::VRSImageSequence(const std::string& url) :
	Medium(url),
	FiniteMedium(url),
	VRSMedium(url),
	VRSFiniteMedium(url),
	FrameMedium(url),
	VRSFrameMedium(url),
	ImageSequence(url)
{
	if (isValid_)
	{
		ocean_assert(recordFileReader_);
		recordFileReader_->setStreamPlayer(streamId_, &imagePlayable_);
	}
}

VRSImageSequence::~VRSImageSequence()
{
	stopThreadExplicitly();
}

std::string VRSImageSequence::currentUrl() const
{
	// as the VRS file contains all the images, we have the same url for all images
	return url();
}

unsigned int VRSImageSequence::images() const
{
	const Timestamp startTimestamp(true);

	while (startTimestamp + 5.0 > Timestamp(true))
	{
		const ScopedLock scopedLock(lock_);

		if (images_ != 0u)
		{
			return images_;
		}
	}

	return 0u;
}

bool VRSImageSequence::start()
{
	const ScopedLock scopedLock(lock_);

	if (startTimestamp_.isValid())
	{
		return true;
	}

	frameCollection_.release();
	images_ = 0u;

	startTimestamp_.toNow();
	pauseTimestamp_.toInvalid();
	stopTimestamp_.toInvalid();

	previousUnixFrameTimestamp_.toInvalid();
	previousVRSFrameTimestamp_.toInvalid();

	startThread();

	return true;
}

bool VRSImageSequence::pause()
{
	return false;
}

bool VRSImageSequence::stop()
{
	const ScopedLock scopedLock(lock_);

	if (stopTimestamp_.isValid())
	{
		return true;
	}

	stopThread();

	return true;
}

bool VRSImageSequence::setMode(const SequenceMode mode)
{
	const ScopedLock scopedLock(lock_);

	respectPlaybackTime_ = mode == SM_AUTOMATIC;
	mediumSequenceMode = mode;

	return true;
}

MediumRef VRSImageSequence::clone() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid_);
	if (isValid_)
	{
		return VRSLibrary::newImageSequence(url_, true);
	}

	return MediumRef();
}

bool VRSImageSequence::forceNextFrame()
{
	unsigned int nextFrameIndex = (unsigned int)(-1);

	{
		const ScopedLock scopedLock(lock_);

		if (mediumSequenceMode != SM_EXPLICIT)
		{
			ocean_assert(false && "The image sequence is not in explicit mode!");
			return false;
		}

		nextFrameIndex = mediumFrameIndex + 1u;

		if (nextFrameIndex >= images_)
		{
			if (loop_)
			{
				nextFrameIndex = 0u;
			}
			else
			{
				return false;
			}
		}

		forceNextFrame_ = true;
	}

	const Timestamp startTimestamp(true);

	while (true)
	{
		ocean_assert(nextFrameIndex != (unsigned int)(-1));

		sleep(1u);

		if (startTimestamp + 5.0 < Timestamp(true))
		{
			ocean_assert(false && "Timeout!");
			return false;
		}

		const ScopedLock scopedLock(lock_);

		if (mediumFrameIndex == nextFrameIndex)
		{
			break;
		}
	}

	return true;
}

void VRSImageSequence::threadRun()
{
	ocean_assert(recordFileReader_);
	ocean_assert(streamId_.getInstanceId() != uint16_t(0));

	ocean_assert(firstFrameTimestamp_.isValid());

	std::vector<const vrs::IndexRecord::RecordInfo*> recordInfos;

	Indices32 imageIndicesFromRecordIndices_;
	Indices32 configurationRecordIndexFromImageIndex_;

	unsigned int images = 0u;

	size_t nextFrameRecordIndex = size_t(-1);

	{
		const ScopedLock scopedLock(lock_);

		mediumFrameIndex = 0u;

		recordInfos = recordFileReader_->getIndex(streamId_);

		// first we need to separate configuration and data records

		imageIndicesFromRecordIndices_.reserve(recordInfos.size());
		configurationRecordIndexFromImageIndex_.reserve(recordInfos.size());

		unsigned int lastConfigurationRecordIndex = (unsigned int)(-1);

		for (size_t recordIndex = 0; recordIndex < recordInfos.size(); ++recordIndex)
		{
			ocean_assert(recordInfos[recordIndex] != nullptr);
			const vrs::IndexRecord::RecordInfo& recordInfo = *recordInfos[recordIndex];

			unsigned int imageIndex = (unsigned int)(-1);

			if (recordInfo.recordType == vrs::Record::Type::CONFIGURATION)
			{
				recordFileReader_->readRecord(recordInfo, &imagePlayable_);

				lastConfigurationRecordIndex = (unsigned int)(recordIndex);
			}
			else if (recordInfo.recordType == vrs::Record::Type::DATA)
			{
				if (lastConfigurationRecordIndex == (unsigned int)(-1))
				{
					ocean_assert(false && "Image without configuration");
				}
				else
				{
					imageIndex = images++;
					configurationRecordIndexFromImageIndex_.push_back(lastConfigurationRecordIndex);
				}
			}

			imageIndicesFromRecordIndices_.push_back(imageIndex);
		}

		ocean_assert(startTimestamp_.isValid());
		ocean_assert(stopTimestamp_.isInvalid());

		if (newDesiredPosition_ >= 0.0)
		{
			position_ = newDesiredPosition_;
			newDesiredPosition_ = -1.0;
		}

		nextFrameRecordIndex = findDataRecordInfoForTimestamp(recordInfos, Timestamp(position_ + double(firstFrameTimestamp_)));
		ocean_assert(nextFrameRecordIndex != size_t(-1));
	}

	unsigned int lastConfigurationRecordIndex = (unsigned int)(-1);

	ocean_assert(nextFrameRecordIndex < imageIndicesFromRecordIndices_.size());
	unsigned int nextFrameIndex = imageIndicesFromRecordIndices_[nextFrameRecordIndex];

	while (nextFrameRecordIndex != size_t(-1) && shouldThreadStop() == false)
	{
		if (nextFrameRecordIndex >= recordInfos.size())
		{
			if (loop_)
			{
				position_ = 0.0;
				previousVRSFrameTimestamp_.toInvalid();

				nextFrameRecordIndex = findDataRecordInfoForTimestamp(recordInfos, Timestamp(position_ + double(firstFrameTimestamp_)));

				ocean_assert(nextFrameRecordIndex < imageIndicesFromRecordIndices_.size());
				nextFrameIndex = imageIndicesFromRecordIndices_[nextFrameRecordIndex];
			}
			else
			{
				break;
			}
		}

		const unsigned int currentConfigurationRecordIndex = configurationRecordIndexFromImageIndex_[nextFrameIndex];

		if (currentConfigurationRecordIndex != lastConfigurationRecordIndex)
		{
			recordFileReader_->readRecord(*recordInfos[currentConfigurationRecordIndex], &imagePlayable_); // will internally call FrameMedium::onNewCamera()

			lastConfigurationRecordIndex = currentConfigurationRecordIndex;
		}

		recordFileReader_->readRecord(*recordInfos[nextFrameRecordIndex], &imagePlayable_); // will internally call FrameMedium::onNewFrame()

		{
			const ScopedLock scopedLock(lock_);

			position_ = recordInfos[nextFrameRecordIndex]->timestamp - double(firstFrameTimestamp_);
			ocean_assert(position_ >= 0.0 && position_ < normalDuration_);

			if (images_ == 0u)
			{
				images_ = images;
			}

			mediumFrameIndex = nextFrameIndex;

			if (mediumFrameIndex + 1u >= images_ && !loop_)
			{
				// we reached the last frame
				break;
			}
		}

		// if the image sequence is in explicit mode, we have to wait until the next frame is forced
		while (shouldThreadStop() == false)
		{
			TemporaryScopedLock temporaryScopedLock(lock_);

			if (mediumSequenceMode != SM_EXPLICIT || forceNextFrame_)
			{
				forceNextFrame_ = false;
				break;
			}

			temporaryScopedLock.release();
			Thread::sleep(1u);
		}

		const ScopedLock scopedLock(lock_);

		if (newDesiredPosition_ >= 0.0)
		{
			position_ = newDesiredPosition_;
			newDesiredPosition_ = -1.0;

			nextFrameRecordIndex = findDataRecordInfoForTimestamp(recordInfos, Timestamp(position_ + double(firstFrameTimestamp_)));
		}
		else
		{
			while (++nextFrameRecordIndex < imageIndicesFromRecordIndices_.size() && imageIndicesFromRecordIndices_[nextFrameRecordIndex] == (unsigned int)(-1))
			{
				// nothing to do here
			}
		}

		if (nextFrameRecordIndex < imageIndicesFromRecordIndices_.size())
		{
			nextFrameIndex = imageIndicesFromRecordIndices_[nextFrameRecordIndex];
		}
	}

	const ScopedLock scopedLock(lock_);

	stopTimestamp_.toNow();
	startTimestamp_.toInvalid();
}

}

}

}
