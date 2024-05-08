/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/PluginManager.h"
#include "ocean/base/Messenger.h"
#include "ocean/base/StringApple.h"

#include <dlfcn.h>

#include <Foundation/Foundation.h>

#ifndef __APPLE__
    #error This files contains Apple specific functions only, see PluginManager.cpp
#endif

namespace Ocean
{

bool PluginManager::Plugin::loadApple() const
{
	if (handle_)
	{
		return true;
	}

	bool result = false;

	// first we need to determine the filename of the executable inside the bundle

	std::string file = filename_;
	size_t pos = file.rfind('/');
	if (pos != std::string::npos)
	{
		file = file.substr(pos + 1);
	}

	pos = file.rfind('.');
	if (pos != std::string::npos)
	{
		file = file.substr(0, pos);
	}

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1

	handle_ = dlopen((filename_ + std::string("/") + file).c_str(), RTLD_NOW);

#else // TARGET_OS_IPHONE

	handle_ = dlopen((filename_ + std::string("/Contents/MacOS/") + file).c_str(), RTLD_NOW);

#endif // TARGET_OS_IPHONE

	if (handle_)
	{
		loadFunction_ = PluginLoadFunction(dlsym(handle_, "pluginLoad"));
		unloadFunction_ = PluginUnloadFunction(dlsym(handle_, "pluginUnload"));

		if (loadFunction_ != nullptr)
		{
			result = loadFunction_();
		}
		else
		{
			Log::error() << "The plugin \"" << name_ << "\" supports no valid load function.";
		}
	}
	else
	{
		const std::string errorMessage(String::toAString(dlerror()));
		ocean_assert(!errorMessage.empty());

		if (errorMessage.empty())
		{
			Log::error() << "Error loading \"" << name_ << "\", the reason is unknown.";
		}
		else
		{
			Log::error() << "Error loading \"" << name_ << "\", the reason is: \"" << errorMessage << "\".";
		}
	}

	return result;
}

bool PluginManager::Plugin::unloadApple() const
{
	bool result = false;

	if (unloadFunction_)
	{
		if (unloadFunction_() == false)
		{
			Log::error() << "Could not unload plugin \"" << name_ << "\", check whether some resources are sill used.";
			return false;
		}
	}

	result = handle_ != nullptr || dlclose(handle_) == 0;
	handle_ = nullptr;

	if (result)
	{
		Log::info() << "Successfully unloaded \"" << name_ << "\" plugin.";
	}

	return result;
}


bool PluginManager::determinePluginApple(const std::string& filename, Plugin& plugin)
{
	NSString* nsFilename = [[NSString alloc] initWithCString:filename.c_str() encoding:NSASCIIStringEncoding];
	NSBundle* nsBundle = [NSBundle bundleWithPath:nsFilename];

	ocean_assert(nsBundle);
	if (nsBundle == nullptr)
	{
		return false;
	}

	NSDictionary* nsDirectory = [nsBundle infoDictionary];

	ocean_assert(nsDirectory);
	if (nsDirectory == nullptr)
	{
		return false;
	}

	const std::string name(StringApple::toUTF8([nsDirectory objectForKey:@"OceanPluginName"]));
	const std::string description(StringApple::toUTF8([nsDirectory objectForKey:@"OceanPluginDescription"]));
	const std::string thirdpartyDependences(StringApple::toUTF8([nsDirectory objectForKey:@"OceanPluginThirdpartydependences"]));
	const std::string thirdpartyDescription(StringApple::toUTF8([nsDirectory objectForKey:@"OceanPluginThirdpartydescription"]));

	const PluginType type = translateType(StringApple::toUTF8([nsDirectory objectForKey:@"OceanPluginType"]));

	NSNumber* nsPriorityValue = [nsDirectory objectForKey:@"OceanPluginPriority"];
	PluginPriority priority = nsPriorityValue ? PluginPriority([nsPriorityValue intValue]) : PRIORITY_UNDEFINED;

	PluginTypeSet dependences;
	std::string dependencesValue(StringApple::toUTF8([nsDirectory objectForKey:@"OceanPluginDependences"]));

	while (!dependencesValue.empty())
	{
		const std::string::size_type pos = dependencesValue.find(" ");
		const std::string dependency = dependencesValue.substr(0, pos);

		const PluginType dependencyValue = translateType(String::toAString(dependency));

		if (dependencyValue == TYPE_UNKNOWN)
		{
#ifdef OCEAN_DEBUG
			Log::error() << "Plugin " << String::toAString(name) << " has an invalid dependency value \"" << String::toAString(dependency) << "\".";
#endif
		}
		else
		{
			dependences.insert(dependencyValue);
		}

		if (pos == std::string::npos)
		{
			break;
		}

		dependencesValue = dependencesValue.substr(pos + 1);
	}

	if (name.empty() == false && type != TYPE_UNKNOWN)
	{
		plugin = Plugin(filename, String::toAString(name), String::toAString(description), type, priority, dependences, String::toAString(thirdpartyDependences), String::toAString(thirdpartyDescription));
	}
	else
	{
		return false;
	}

	return true;
}

} // namespace Ocean
