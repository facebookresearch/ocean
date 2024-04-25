// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/vrs/VRSMedium.h"

#include "ocean/base/String.h"

#include "ocean/io/Directory.h"
#include "ocean/io/File.h"

#include "ocean/io/vrs/Utilities.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace Media
{

namespace VRS
{

VRSMedium::VRSMedium(const std::string& url) :
	Medium(url)
{
	libraryName_ = nameVRSLibrary();

	// we need to separate the given url into a VRS file and the corresponding streamId (both are separated by a slash/backslash: <vrsFile>/<streamId>

	const IO::File vrsFileAndStreamId(url);

	if (vrsFileAndStreamId.extension() == "vrs")
	{
		// the given url does not contain a specific stream id, so that we select the first suitable

		const IO::VRS::Utilities::RecordableTypeIdSet recordableTypeIds =
		{
			vrs::RecordableTypeId::SlamCameraData,
			vrs::RecordableTypeId::ForwardCameraRecordableClass,
			vrs::RecordableTypeId::ImageStream,
			vrs::RecordableTypeId::FacebookARCamera
		};

		const std::vector<std::string> streamIds = IO::VRS::Utilities::availableRecordablesInFile(url, recordableTypeIds);

		if (streamIds.empty())
		{
			return;
		}

		streamIdName_ = streamIds.front();
		vrsFilename_ = url;
	}
	else
	{
		streamIdName_ = vrsFileAndStreamId.name();

		if (streamIdName_.empty())
		{
			return;
		}

		IO::Directory vrsFileAsDirectory(vrsFileAndStreamId);
		vrsFilename_ = vrsFileAsDirectory();

		if (vrsFilename_.size() <= 1 || !IO::Directory::isSeparator(vrsFilename_.back()))
		{
			return;
		}

		vrsFilename_.pop_back();
	}

	const IO::File vrsFile(vrsFilename_);

	recordFileReader_ = std::make_unique<vrs::RecordFileReader>();

	if (recordFileReader_->openFile(vrsFile()) == 0)
	{
		streamId_ = findStreamId(*recordFileReader_, streamIdName_);

		isValid_ = streamId_.getInstanceId() != uint16_t(0);
	}
}

VRSMedium::~VRSMedium()
{
	// nothing to do here
}

bool VRSMedium::isStarted() const
{
	return startTimestamp_.isValid();
}

Timestamp VRSMedium::startTimestamp() const
{
	return startTimestamp_;
}

Timestamp VRSMedium::pauseTimestamp() const
{
	return pauseTimestamp_;
}

Timestamp VRSMedium::stopTimestamp() const
{
	return stopTimestamp_;
}

bool VRSMedium::setRespectPlaybackTime(const bool respectPlaybackTime)
{
	if (!respectPlaybackTime)
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	return true;
}

vrs::StreamId VRSMedium::findStreamId(const vrs::RecordFileReader& recordFileReader, const std::string& name)
{
	const std::set<vrs::StreamId>& streamIds = recordFileReader.getStreams();

	for (const vrs::StreamId& streamId : streamIds)
	{
		if (streamId.getName() == name)
		{
			return streamId;
		}
	}

	return vrs::StreamId();
}

Timestamp VRSMedium::determineFirstFrameTimestamp(const vrs::RecordFileReader& recordFileReader, const vrs::StreamId& streamId)
{
	const std::vector<const vrs::IndexRecord::RecordInfo*>& recordInfos = recordFileReader.getIndex(streamId);

#ifdef OCEAN_DEBUG
	double debugPreviousTimestamp = NumericD::minValue();
	for (size_t n = 0; n < recordInfos.size(); ++n)
	{
		ocean_assert(debugPreviousTimestamp <= recordInfos[n]->timestamp);
		debugPreviousTimestamp = recordInfos[n]->timestamp;
	}
#endif

	for (size_t n = 0; n < recordInfos.size(); ++n)
	{
		if (recordInfos[n]->recordType == vrs::Record::Type::DATA)
		{
			return Timestamp(recordInfos[n]->timestamp);
		}
	}

	return Timestamp(false);
}

Timestamp VRSMedium::determineLastFrameTimestamp(const vrs::RecordFileReader& recordFileReader, const vrs::StreamId& streamId)
{
	const std::vector<const vrs::IndexRecord::RecordInfo*>& recordInfos = recordFileReader.getIndex(streamId);

#ifdef OCEAN_DEBUG
	double debugPreviousTimestamp = NumericD::minValue();
	for (size_t n = 0; n < recordInfos.size(); ++n)
	{
		ocean_assert(debugPreviousTimestamp <= recordInfos[n]->timestamp);
		debugPreviousTimestamp = recordInfos[n]->timestamp;
	}
#endif

	for (size_t n = recordInfos.size() - 1; n < recordInfos.size(); --n)
	{
		if (recordInfos[n]->recordType == vrs::Record::Type::DATA)
		{
			return Timestamp(recordInfos[n]->timestamp);
		}
	}

	return Timestamp(false);
}

size_t VRSMedium::determineNumberFrames(const vrs::RecordFileReader& recordFileReader, const vrs::StreamId& streamId)
{
	const std::vector<const vrs::IndexRecord::RecordInfo*>& recordInfos = recordFileReader.getIndex(streamId);

	size_t numberFrames = 0;

	for (size_t n = 0; n < recordInfos.size(); ++n)
	{
		if (recordInfos[n]->recordType == vrs::Record::Type::DATA)
		{
			numberFrames++;
		}
	}

	return numberFrames;
}

size_t VRSMedium::findDataRecordInfoForTimestamp(const std::vector<const vrs::IndexRecord::RecordInfo*>& recordInfos, const Timestamp& timestamp, bool* perfectMatch)
{
	ocean_assert(timestamp.isValid());

	if (recordInfos.empty())
	{
		return size_t(-1);
	}

	// **TODO** apply binary search

	for (size_t n = 0; n < recordInfos.size(); ++n)
	{
		if (recordInfos[n]->recordType != vrs::Record::Type::DATA)
		{
			continue;
		}

		if (NumericD::isWeakEqual(recordInfos[n]->timestamp, double(timestamp)))
		{
			if (perfectMatch)
			{
				*perfectMatch = true;
			}

			return n;
		}

		if (recordInfos[n]->timestamp > double(timestamp))
		{
			if (perfectMatch)
			{
				*perfectMatch = false;
			}

			return n;
		}
	}

	if (perfectMatch)
	{
		*perfectMatch = false;
	}

	return recordInfos.size() - 1;
}

}

}

}
