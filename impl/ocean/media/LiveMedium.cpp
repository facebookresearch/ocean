/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
