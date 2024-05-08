/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
