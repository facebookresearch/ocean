/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/PluginManager.h"
#include "ocean/base/Messenger.h"

#ifndef _WINDOWS
	#include <dirent.h>
	#include <dlfcn.h>
#endif

namespace Ocean
{

PluginManager::Plugin::Plugin(const std::string& filename, const std::string& name, const std::string& description, const PluginType type, const PluginPriority priority, const PluginTypeSet& dependencySet, const std::string& thirdpartyDependences, const std::string& thirdpartyDescription) :
	filename_(filename),
	name_(name),
	description_(description),
	type_(type),
	dependencySet_(dependencySet),
	priority_(priority),
	thirdpartyDependences_(thirdpartyDependences),
	thirdpartyDescription_(thirdpartyDescription)
{
	ocean_assert(filename_.empty() == false);
	ocean_assert(name_.empty() == false);
}

bool PluginManager::Plugin::load() const
{
	if (handle_)
	{
		return true;
	}

	bool result = false;

#if defined(_WINDOWS)

	handle_ = LoadLibrary(String::toWString(filename_).c_str());

	if (handle_)
	{
		loadFunction_ = PluginLoadFunction(GetProcAddress((HMODULE)(handle_), "pluginLoad"));
		unloadFunction_ = PluginUnloadFunction(GetProcAddress((HMODULE)(handle_), "pluginUnload"));

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
		const DWORD errorCode = GetLastError();

		wchar_t* msg = nullptr;
		const DWORD formatResult = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (wchar_t*)(&msg), 0, nullptr);

		if (formatResult != 0)
		{
			std::wstring message(msg);
			if (msg[formatResult - 2] == 13 && msg[formatResult - 1] == 10)
			{
				message = message.substr(0, formatResult - 2);
			}

			Log::error() << "Error loading \"" << name_ << "\", the reason is error " << int(errorCode) << ": \"" << String::toAString(message) << "\".";

			if (errorCode == 126)
			{
				Log::error() << "This error mostly occurs if 3rd party dlls can not be accessed.";

				if (thirdpartyDependences_.empty() == false)
				{
					Log::error() << "This plugin needs the following 3rd party dlls: \"" << thirdpartyDependences_ << "\".";
				}
				if (thirdpartyDescription_.empty() == false)
				{
					Log::error() << thirdpartyDescription_;
				}
			}

			LocalFree(msg);
		}
		else
		{
			Log::error() << "Error loading \"" << name_ << "\", the reason is unknown.";
		}
	}

	if (result)
	{
		PluginVersionFunction pluginVersionFunction = PluginVersionFunction(GetProcAddress((HMODULE)handle_, "pluginVersion"));

		if (pluginVersionFunction)
		{
			Log::info() << "The plugin uses: " << pluginVersionFunction();
		}

		Log::info() << "Successfully loaded \"" << name_ << "\" plugin.";
	}
#elif defined(__APPLE__)

	result = loadApple();

#else // _WINDOWS

	handle_ = dlopen(filename_.c_str(), RTLD_LAZY);
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

#endif // _WINDOWS

	return result;
}

bool PluginManager::Plugin::unload() const
{
	bool result = false;

#ifdef _WINDOWS

	if (unloadFunction_)
	{
		if (unloadFunction_() == false)
		{
			Log::error() << "Could not unload plugin \"" << name_ << "\", check whether some resources are sill used.";
			return false;
		}
	}

	result = handle_ != nullptr || FreeLibrary((HMODULE)handle_) == TRUE;
	handle_ = nullptr;

#elif defined(__APPLE__)

	result = unloadApple();

#else

	if (unloadFunction_)
	{
		if (unloadFunction_() == false)
		{
			Log::error() << "Could not unload plugin \"" << name_ << "\", check whether some resources are sill used.";
			return false;
		}
	}

	result = handle_ == nullptr || dlclose(handle_) == 0;
	handle_ = nullptr;

#endif // _WINDOWS

	if (result)
	{
		Log::info() << "Successfully unloaded \"" << name_ << "\" plugin.";
	}

	return result;
}

PluginManager::Plugin::operator bool() const
{
	return handle_ != nullptr;
}

bool PluginManager::Plugin::operator<(const Plugin& right) const
{
	if (right.dependencySet_.find(type_) != right.dependencySet_.end())
	{
		if (dependencySet_.find(right.type_) != dependencySet_.end())
		{

#ifdef OCEAN_DEBUG
			Log::warning() << "Plugin manager: " << name_ << " and " << right.name_ << " have mutual plugin dependences.";
#endif

			return filename_ < right.filename_;
		}

		return true;
	}
	else if (dependencySet_.find(right.type_) != dependencySet_.end())
	{
		return false;
	}

	if (priority_ > right.priority_)
	{
		return true;
	}
	else if (priority_ < right.priority_)
	{
		return false;
	}

	return filename_ < right.filename_;
}

PluginManager::PluginManager() :
#ifdef _WINDOWS
	pluginFileExtension_("opf")
#else
	pluginFileExtension_("plugin")
#endif
{
	// nothing to do here
}

PluginManager::~PluginManager()
{
	release();
}

bool PluginManager::setPluginFileExtension(const std::string& extension)
{
	const ScopedLock scopedLock(lock_);

	if (!collectedPlugins_.empty() || loadedPlugins_.empty())
	{
		return false;
	}

	pluginFileExtension_ = extension;
	return true;
}

unsigned int PluginManager::collectPlugins(const std::string& directory, const bool removeAlreadyCollected)
{
	const ScopedLock scopedLock(lock_);

	if (removeAlreadyCollected)
	{
		collectedPlugins_.clear();
	}

	unsigned int foundPlugins = 0u;

#if defined(_WINDOWS)

	std::string closedDirectory;
	if (directory.rfind("/") == directory.size() - 1 || directory.rfind("\\") == directory.size() - 1)
	{
		closedDirectory = directory;
	}
	else
	{
		closedDirectory = directory + "/";
	}

	std::string searchFilename(closedDirectory + std::string("*.") + pluginFileExtension_);

	WIN32_FIND_DATA data;
	HANDLE handle = FindFirstFile(String::toWString(searchFilename).c_str(), &data);

	if (handle == INVALID_HANDLE_VALUE)
	{
		Log::warning() << "Invalid plugin directory \"" << directory << "\".";
		return 0u;
	}

	while (handle != nullptr)
	{
		if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 && data.cFileName[0] != '\0')
		{
			Plugin plugin;

			if (determinePlugin(closedDirectory + String::toAString(data.cFileName), plugin))
			{
				collectedPlugins_.push_back(plugin);
				++foundPlugins;
			}
		}

		if (FindNextFile(handle, &data) == FALSE)
		{
			break;
		}
	};

	if (foundPlugins > 0u)
	{
		SetDllDirectory(String::toWString(directory).c_str());
	}

#else

	std::string closedDirectory;
	if (directory.rfind("/") == directory.size() - 1 || directory.rfind("\\") == directory.size() - 1)
	{
		closedDirectory = directory;
	}
	else
	{
		closedDirectory = directory + "/";
	}

	DIR* handle = opendir(closedDirectory.c_str());
	if (handle == nullptr)
	{
		Log::warning() << "Invalid plugin directory \"" << directory << "\".";
		return 0u;
	}

	dirent* entry = readdir(handle);

	while (entry != nullptr)
	{
		const std::string filename(entry->d_name);

		// don't add the default files "." and ".." from a directory
		if (!filename.empty() && filename != std::string(".") && filename != std::string(".."))
		{
			const std::size_t pos = filename.rfind('.');

			if (pos != std::string::npos && filename.substr(pos + 1) == pluginFileExtension_)
			{
				Plugin plugin;

				if (determinePlugin(closedDirectory + filename, plugin))
				{
					collectedPlugins_.push_back(plugin);
					++foundPlugins;
				}
			}
		}

		entry = readdir(handle);
	}

	closedir(handle);

#endif // _WINDOWS

	return foundPlugins;
}

bool PluginManager::loadPlugin(const std::string& name)
{
	const ScopedLock scopedLock(lock_);

	for (Plugins::iterator i = collectedPlugins_.begin(); i != collectedPlugins_.end(); ++i)
	{
		if (i->name() == name)
		{
			if (i->load())
			{
				loadedPlugins_.push_back(*i);
				collectedPlugins_.erase(i);
				return true;
			}

			break;
		}
	}

	return false;
}

bool PluginManager::loadPlugins(const Names& names)
{
	const ScopedLock scopedLock(lock_);
	PluginSet pluginsToLoad;

	for (Names::const_iterator iN = names.begin(); iN != names.end(); ++iN)
	{
		for (Plugins::iterator iP = collectedPlugins_.begin(); iP != collectedPlugins_.end(); ++iP)
		{
			if (iP->name() == *iN)
			{
				pluginsToLoad.insert(*iP);
			}
		}
	}

	bool loaded = false;
	for (PluginSet::const_iterator i = pluginsToLoad.begin(); i != pluginsToLoad.end(); ++i)
	{
		const Plugin& plugin = *i;

		if (plugin.load())
		{
			loadedPlugins_.push_back(*i);
			loaded = true;
		}
	}

	return loaded;
}

bool PluginManager::loadPlugins(const PluginType type)
{
	const ScopedLock scopedLock(lock_);
	PluginSet pluginsToLoad;

	for (Plugins::iterator iP = collectedPlugins_.begin(); iP != collectedPlugins_.end(); ++iP)
	{
		if (iP->type() & type)
		{
			pluginsToLoad.insert(*iP);
		}
	}

	bool loaded = false;
	for (PluginSet::const_iterator i = pluginsToLoad.begin(); i != pluginsToLoad.end(); ++i)
	{
		const Plugin& plugin = *i;

		if (plugin.load())
		{
			loadedPlugins_.push_back(*i);
			loaded = true;
		}
	}

	return loaded;
}

bool PluginManager::loadAllPlugins()
{
	const ScopedLock scopedLock(lock_);
	PluginSet pluginsToLoad;

	for (Plugins::iterator iP = collectedPlugins_.begin(); iP != collectedPlugins_.end(); ++iP)
	{
		pluginsToLoad.insert(*iP);
	}

	bool loaded = false;
	for (PluginSet::const_iterator i = pluginsToLoad.begin(); i != pluginsToLoad.end(); ++i)
	{
		const Plugin& plugin = *i;

		if (plugin.load())
		{
			loadedPlugins_.push_back(*i);
			loaded = true;
		}
		else
		{
			Log::error() << "Could not loaded the \"" << i->name() << "\" plugin.";
		}
	}

	return loaded;
}

bool PluginManager::unloadAllPlugins()
{
	const ScopedLock scopedLock(lock_);

	bool result = true;

	for (Plugins::reverse_iterator i = loadedPlugins_.rbegin(); i != loadedPlugins_.rend(); ++i)
	{
		result = i->unload() && result;
	}

	for (size_t n = 0; n < loadedPlugins_.size(); /* noop */)
	{
		if (!loadedPlugins_[n])
		{
			collectedPlugins_.push_back(loadedPlugins_[n]);
			loadedPlugins_.erase(loadedPlugins_.begin() + n);
		}
		else
		{
			++n;
		}
	}

	return result;
}

PluginManager::Names PluginManager::plugins() const
{
	const ScopedLock scopedLock(lock_);

	Names result;

	for (Plugins::const_iterator i = collectedPlugins_.begin(); i != collectedPlugins_.end(); ++i)
	{
		result.push_back(i->name());
	}

	for (Plugins::const_iterator i = loadedPlugins_.begin(); i != loadedPlugins_.end(); i++)
	{
		result.push_back(i->name());
	}

	return result;
}

PluginManager::Names PluginManager::loadedPlugins() const
{
	const ScopedLock scopedLock(lock_);

	Names result;

	for (Plugins::const_iterator i = loadedPlugins_.begin(); i != loadedPlugins_.end(); i++)
	{
		result.push_back(i->name());
	}

	return result;
}

PluginManager::Names PluginManager::unloadedPlugins() const
{
	const ScopedLock scopedLock(lock_);

	Names result;

	for (Plugins::const_iterator i = collectedPlugins_.begin(); i != collectedPlugins_.end(); ++i)
	{
		result.push_back(i->name());
	}

	return result;
}

void PluginManager::release()
{
	unloadAllPlugins();
}

bool PluginManager::determinePlugin(const std::string& filename, Plugin& plugin)
{
	bool isValid = false;

#if defined(_WINDOWS)

	DWORD dummyValue = 0;
	int infoSize = GetFileVersionInfoSize((wchar_t*)String::toWString(filename).c_str(), &dummyValue);

	if (infoSize > 0)
	{
		std::vector<unsigned char> data(infoSize);

		if (GetFileVersionInfo((wchar_t*)String::toWString(filename).c_str(), 0, infoSize, data.data()))
		{
			void* value = nullptr;
			unsigned int valueLength = 0;

			std::wstring name;
			std::wstring description;
			std::wstring thirdpartyDependences;
			std::wstring thirdpartyDescription;

			PluginType type = TYPE_UNKNOWN;
			PluginPriority priority = PRIORITY_UNDEFINED;
			PluginTypeSet dependences;

			if (VerQueryValue(data.data(), L"\\OceanPlugin\\Name", &value, &valueLength) && valueLength > 0)
			{
				isValid = true;
				name = std::wstring((wchar_t*)value, valueLength - 1);

				if (VerQueryValue(data.data(), L"\\OceanPlugin\\Description", &value, &valueLength) && valueLength > 0)
				{
					description = std::wstring((wchar_t*)value, valueLength - 1);
				}

				if (VerQueryValue(data.data(), L"\\OceanPlugin\\Type", &value, &valueLength) && valueLength > 0)
				{
					type = translateType(String::toAString(std::wstring((wchar_t*)value, valueLength - 1)));
				}

				if (VerQueryValue(data.data(), L"\\OceanPlugin\\Priority", &value, &valueLength) && valueLength > 0)
				{
					if (valueLength == 1)
					{
						priority = PluginPriority(*(int8_t*)value);
					}
					else if (valueLength == 2)
					{
						priority = PluginPriority(*(int16_t*)value);
					}
					else if (valueLength == 4)
					{
						priority = PluginPriority(*(int32_t*)value);
					}
				}

				if (VerQueryValue(data.data(), L"\\OceanPlugin\\Dependences", &value, &valueLength) && valueLength > 0)
				{
					std::wstring values(std::wstring((wchar_t*)value, valueLength - 1));

					while (true)
					{
						std::wstring::size_type pos = values.find(L" ");
						std::wstring dependency = values.substr(0, pos);

						PluginType dependencyValue = translateType(String::toAString(dependency));
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

						values = values.substr(pos + 1);
					}

					if (VerQueryValue(data.data(), L"\\OceanPlugin\\Thirdpartydependences", &value, &valueLength) && valueLength > 0)
					{
						thirdpartyDependences = std::wstring((wchar_t*)value, valueLength - 1);
					}

					if (VerQueryValue(data.data(), L"\\OceanPlugin\\Thirdpartydescription", &value, &valueLength) && valueLength > 0)
					{
						thirdpartyDescription = std::wstring((wchar_t*)value, valueLength - 1);
					}
				}

				if (name.empty() == false && type != TYPE_UNKNOWN)
				{
					plugin = Plugin(filename, String::toAString(name), String::toAString(description), type, priority, dependences,
							String::toAString(thirdpartyDependences), String::toAString(thirdpartyDescription));
				}
				else
				{
					isValid = false;
				}
			}
		}
	}

#elif defined(__APPLE__)

	isValid = determinePluginApple(filename, plugin);

#else

	OCEAN_WARNING_MISSING_IMPLEMENTATION;
	OCEAN_SUPPRESS_UNUSED_WARNING(filename);
	OCEAN_SUPPRESS_UNUSED_WARNING(plugin);

#endif // _WINDOWS, __APPLE__

	return isValid;
}

PluginManager::PluginType PluginManager::translateType(const std::string& type)
{
	if (type == "DEVICE")
	{
		return TYPE_DEVICE;
	}

	if (type == "INTERACTION")
	{
		return TYPE_INTERACTION;
	}

	if (type == "MEDIA")
	{
		return TYPE_MEDIA;
	}

	if (type == "RENDERING")
	{
		return TYPE_RENDERING;
	}

	if (type == "SCENEDESCRIPTION")
	{
		return TYPE_SCENEDESCRIPTION;
	}

	if (type == "PHYSICS")
	{
		return TYPE_PHYSICS;
	}

	return TYPE_UNKNOWN;
}

} // namespace Ocean
