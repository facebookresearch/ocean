// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/io/vrs/Reader.h"

#include <vrs/RecordFileReader.h>

namespace Ocean
{

namespace IO
{

namespace VRS
{

Reader::Reader()
{
	// nothing to do here
}

Reader::~Reader()
{
	// nothing to do here
}

bool Reader::addPlayable(vrs::RecordFormatStreamPlayer* playable, const std::string& name)
{
	ocean_assert(playable != nullptr);
	ocean_assert(!name.empty());

	if (playable == nullptr || name.empty())
	{
		return false;
	}

	if (playableNameReaderMap_.find(name) != playableNameReaderMap_.cend())
	{
		return false;
	}

	playableNameReaderMap_.insert(std::make_pair(name, playable));

	return true;
}

bool Reader::addPlayable(vrs::RecordFormatStreamPlayer* playable, const vrs::RecordableTypeId& recordableTypeId, const std::string& flavor)
{
	ocean_assert(playable != nullptr);
	ocean_assert(recordableTypeId != vrs::RecordableTypeId::Undefined);

	if (playable == nullptr || recordableTypeId == vrs::RecordableTypeId::Undefined)
	{
		return false;
	}

	if (playableRecordableTypeIdReaderMap_.find(std::make_pair(recordableTypeId, flavor)) != playableRecordableTypeIdReaderMap_.cend())
	{
		return false;
	}

	playableRecordableTypeIdReaderMap_.insert(std::make_pair(std::make_pair(recordableTypeId, flavor), playable));

	return true;
}

size_t Reader::read(const std::string& filename)
{
	ocean_assert(!filename.empty());
	if (filename.empty())
	{
		return 0;
	}

	ocean_assert(!playableNameReaderMap_.empty() || !playableRecordableTypeIdReaderMap_.empty());
	if (playableNameReaderMap_.empty() && playableRecordableTypeIdReaderMap_.empty())
	{
		return 0;
	}

	const ScopedLock scopedLock(lock_);

	vrs::RecordFileReader recordFileReader;

	if (recordFileReader.openFile(filename) != 0)
	{
		return 0;
	}

	const std::set<vrs::StreamId>& streamIds = recordFileReader.getStreams();

	size_t totalPlayablesRead = 0;

	for (const vrs::StreamId& streamId : streamIds)
	{
		const std::string name = streamId.getName();

		PlayableNameReaderMap::iterator iName = playableNameReaderMap_.find(name);

		vrs::RecordFormatStreamPlayer* playable = nullptr;

		if (iName != playableNameReaderMap_.cend())
		{
			playable = iName->second;
		}
		else
		{
			const std::string& flavor = recordFileReader.getFlavor(streamId);

			PlayableRecordableTypeIdReaderMap::iterator iRecordable = playableRecordableTypeIdReaderMap_.find(std::make_pair(streamId.getTypeId(), flavor));

			if (iRecordable != playableRecordableTypeIdReaderMap_.cend())
			{
				playable = iRecordable->second;
			}
		}

		if (playable == nullptr)
		{
			continue;
		}

		recordFileReader.setStreamPlayer(streamId, playable);

		const std::vector<const vrs::IndexRecord::RecordInfo*> recordInfos = recordFileReader.getIndex(streamId);

		bool readingRecordableSuccessful = true;

		for (size_t n = 0; n < recordInfos.size(); ++n)
		{
			if (recordFileReader.readRecord(*recordInfos[n], playable) != 0)
			{
				readingRecordableSuccessful = false;
				break;
			}
		}

		if (readingRecordableSuccessful)
		{
			++totalPlayablesRead;
		}
	}

	return totalPlayablesRead;
}

}

}

}
