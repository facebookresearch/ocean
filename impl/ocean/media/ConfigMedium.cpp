// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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
