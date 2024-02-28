// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/Image.h"

namespace Ocean
{

namespace Media
{

Image::Image(const std::string& url) :
	Medium(url),
	FrameMedium(url)
{
	type_ = Type(type_ | IMAGE);
}

}

}
