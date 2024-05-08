/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/ConfigMedium.h"

namespace Ocean
{

namespace Media
{

ConfigMedium::ConfigMedium(const std::string& url) :
	Medium(url)
{
	type_ = Type(type_ | CONFIG_MEDIUM);
}

bool ConfigMedium::configuration(const std::string& /*name*/, long long /*data*/)
{
	// must be implemented in a derived class

	return false;
}

ConfigMedium::ConfigNames ConfigMedium::configs() const
{
	// must be implemented in a derived class

	return ConfigNames();
}

}

}
