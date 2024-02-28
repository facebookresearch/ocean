// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/directshow/DSMediaType.h"

DISABLE_WARNINGS_BEGIN
	#include <Streams.h>
DISABLE_WARNINGS_END

namespace Ocean
{

namespace Media
{

namespace DirectShow
{

DSMediaType::DSMediaType(const AM_MEDIA_TYPE& mediaType)
{
	CopyMediaType(&type_, &mediaType);
}

DSMediaType::DSMediaType(DSMediaType&& dsMediaType)
{
	*this = std::move(dsMediaType);
}

DSMediaType::~DSMediaType()
{
	release();
}

void DSMediaType::release()
{
	FreeMediaType(type_);
}

DSMediaType& DSMediaType::operator=(DSMediaType&& dsMediaType)
{
	if (this != &dsMediaType)
	{
		release();

		type_ = dsMediaType.type_;
		dsMediaType.type_ = {};
	}

	return *this;
}

}

}

}
