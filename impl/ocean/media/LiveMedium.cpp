// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/LiveMedium.h"

namespace Ocean
{

namespace Media
{

LiveMedium::LiveMedium(const std::string& url) :
	Medium(url),
	ConfigMedium(url)
{
	type_ = Type(type_ | LIVE_MEDIUM);
}

}

}
