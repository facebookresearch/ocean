// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/io/vrs/FlexibleRecorder.h"

#include <vrs/TagConventions.h>

namespace Ocean
{

namespace IO
{

namespace VRS
{

FlexibleRecorder::FlexibleRecorder(const std::string& name) :
	name_(name)
{
	// nothing to do here
}

FlexibleRecorder::~FlexibleRecorder()
{
	stop();
}

bool FlexibleRecorder::onConfigureWriter()
{
	if (!name_.empty())
	{
		vrs::tag_conventions::addTagSet(recordFileWriter_, {name_});
	}

	vrs::tag_conventions::addCaptureTime(recordFileWriter_);
	vrs::tag_conventions::addOsFingerprint(recordFileWriter_);

	return true;
}

void FlexibleRecorder::release()
{
	Recorder::release();

	for (RecordableMap::iterator i = recordableMap_.begin(); i != recordableMap_.end(); ++i)
	{
		i->second->release();
	}

	recordableMap_.clear();
}

}

}

}
