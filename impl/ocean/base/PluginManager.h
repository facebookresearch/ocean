/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_PLUGIN_MANAGER_H
#define META_OCEAN_BASE_PLUGIN_MANAGER_H

#include "ocean/base/Base.h"
#include "ocean/base/ObjectRef.h"
#include "ocean/base/Singleton.h"

#include <set>
#include <vector>

namespace Ocean
{

/**
 * This class implements a manager for all plugins available for the Ocean framework.
 * @ingroup base
 */
class OCEAN_BASE_EXPORT PluginManager : public Singleton<PluginManager>
{
	friend class Singleton<PluginManager>;

	public:

		/**
		 * Definition of different plugin types.
		 */
		enum PluginType
		{
			/// Unknown plugin type.
			TYPE_UNKNOWN = 0,
			/// Device plugin.
			TYPE_DEVICE = 1 << 0,
			/// Interaction plugin.
			TYPE_INTERACTION = 1 << 1,
			/// Media plugin.
			TYPE_MEDIA = 1 << 2,
			/// Rendering plugin.
			TYPE_RENDERING = 1 << 3,
			/// Scene description plugin.
			TYPE_SCENEDESCRIPTION = 1 << 4,
			/// Physics plugin.
			TYPE_PHYSICS = 1 << 5,
			/// Any plugin
			TYPE_ANY = TYPE_DEVICE | TYPE_INTERACTION | TYPE_MEDIA | TYPE_RENDERING | TYPE_SCENEDESCRIPTION | TYPE_PHYSICS
		};

		/**
		 * Definition of different plugin load priority values.
		 * Higher priority means that the plugin will be loaded earlier than plugins with lower priority.
		 */
		enum PluginPriority
		{
			/// Undefined plugin load priority.
			PRIORITY_UNDEFINED,
			/// Low plugin load priority.
			PRIORITY_LOW,
			/// Medium plugin load priority.
			PRIORITY_MEDIUM,
			/// High plugin load priority.
			PRIORITY_HIGH,
			/// Very high plugin load priority.
			PRIORITY_VERY_HIGH
		};

		/**
		 * Definition of a set holding plugin types.
		 */
		typedef std::set<PluginType> PluginTypeSet;

		/**
		 * Definition of a vector holding plugin names.
		 */
		typedef std::vector<std::string> Names;

	private:

		/**
		 * This class implements a plugin interface.
		 */
		class OCEAN_BASE_EXPORT Plugin
		{
			protected:

				/**
				 * Definition of a function pointer for plugin load functions.
				 */
				typedef bool (*PluginLoadFunction)();

				/**
				 * Definition of a function pointer for plugin unload functions.
				 */
				typedef bool (*PluginUnloadFunction)();

				/**
				 * Definition of a function pointer for plugin version information functions.
				 */
				typedef const char* (*PluginVersionFunction)();

			public:

				/**
				 * Creates an empty plugin object.
				 */
				Plugin() = default;

				/**
				 * Creates a new plugin object.
				 * @param filename Plugin filename
				 * @param name Plugin name
				 * @param description Plugin description
				 * @param type Plugin type
				 * @param priority Plugin load priority, the higher the priority the earlier the plugin will be loaded
				 * @param dependencySet Set of ocean plugin types this plugin depends on.
				 * @param thirdpartyDependences 3rd party dependences
				 * @param thirdpartyDescription 3rd party description
				 */
				Plugin(const std::string& filename, const std::string& name, const std::string& description, const PluginType type, const PluginPriority priority, const PluginTypeSet& dependencySet, const std::string& thirdpartyDependences, const std::string& thirdpartyDescription);

				/**
				 * Returns the filename of the plugin.
				 * @return Plugin filename
				 */
				inline const std::string& filename() const;

				/**
				 * Returns the name of the plugin.
				 * @return Plugin name
				 */
				inline const std::string& name() const;

				/**
				 * Returns the description of the plugin.
				 * @return Plugin description
				 */
				inline const std::string& description() const;

				/**
				 * Returns the version information of the used 3rd party libraries.
				 * @return Version information
				 */
				inline const std::string& thirdpartyInformation() const;

				/**
				 * Returns the type of this plugin.
				 * @return Plugin type
				 */
				inline PluginType type() const;

				/**
				 * Returns the 3rd party dependences of this plugin.
				 * @return 3rd party dependences
				 */
				inline const std::string& thirdpartyDependences() const;

				/**
				 * Returns the description about the 3rd party dependences of this plugin.
				 * @return 3rd party description
				 */
				inline const std::string& thirdpartyDescription() const;

				/**
				 * Loads the plugin.
				 * @return True, if succeeded
				 */
				bool load() const;

#if defined(__APPLE__)

				/**
				 * Loads the plugin (specialization for Apple platforms)
				 * @sa load()
				 * @return True, if succeeded
				 */
				bool loadApple() const;

#endif // defined(__APPLE__)

				/**
				 * Tries to unload the plugin.
				 * All resources using this plugin have to be released to unload the plugin.
				 * @return True, if succeeded
				 */
				bool unload() const;

#if defined(__APPLE__)

				/**
				 * Tries to unload the plugin. (specialization for Apple platforms)
				 * All resources using this plugin have to be released to unload the plugin.
				 * @sa unload()
				 * @return True, if succeeded
				 */
				bool unloadApple() const;

#endif // defined(__APPLE__)

				/**
				 * Returns whether the plugin is successfully loaded.
				 * @return True, if so
				 */
				explicit operator bool() const;

				/**
				 * Returns whether the left plugin has to be loaded before the right one.
				 * @param right Right plugin
				 * @return True, if so
				 */
				bool operator<(const Plugin& right) const;

			protected:

				/// Filename of the plugin.
				std::string filename_;

				/// Name of the plugin.
				std::string name_;

				/// Description of the plugin.
				std::string description_;

				/// 3rd party library version information.
				std::string thirdpartyInformation_;

				/// Platform specific plugin handle.
				mutable void* handle_ = nullptr;

				/// Plugin type.
				PluginType type_ = TYPE_UNKNOWN;

				/// Set of plugin types this plugin depends on.
				PluginTypeSet dependencySet_;

				/// Load priority of this plugin.
				PluginPriority priority_ = PRIORITY_UNDEFINED;

				/// 3rd party dependences.
				std::string thirdpartyDependences_;

				/// 3rd party description.
				std::string thirdpartyDescription_;

				/// Plugin load function.
				mutable PluginLoadFunction loadFunction_ = nullptr;

				/// Plugin unload function.
				mutable PluginUnloadFunction unloadFunction_ = nullptr;
		};

		/**
		 * Definition of a vector holding plugin objects.
		 */
		typedef std::vector<Plugin> Plugins;

		/**
		 * Definition of a set holding plugin objects.
		 */
		typedef std::set<Plugin> PluginSet;

	public:

		/**
		 * Returns the file extension for plugin files.
		 * @return File extension of plugins
		 */
		inline const std::string& fileExtension() const;

		/**
		 * Sets the file extension for plugin files.
		 * @param extension File extension to be set
		 * @return True, if succeeded
		 */
		bool setPluginFileExtension(const std::string& extension);

		/**
		 * Collects all Ocean supported plugins available in a given directory.
		 * @param directory Plugin directory
		 * @param removeAlreadyCollected State determining whether previous collected (but not loaded) plugins will be remove before
		 * @return Number of collected plugins
		 */
		unsigned int collectPlugins(const std::string& directory, const bool removeAlreadyCollected = true);

		/**
		 * Loads a specific plugin.
		 * @param name The name of the plugin to load.
		 * @return True, if succeeded
		 */
		bool loadPlugin(const std::string& name);

		/**
		 * Loads several plugins and uses the internal dependency order.
		 * @param names Names of the plugins to load
		 * @return True, if at least one plugin has been loaded
		 */
		bool loadPlugins(const Names& names);

		/**
		 * Loads all plugins with a specified type.
		 * @param type Plugin type to load, can be a combination of all defined plugin types
		 * @return True, if at least one plugin has been loaded
		 */
		bool loadPlugins(const PluginType type);

		/**
		 * Loads all available plugins.
		 * @return True, if at least one plugin has been loaded
		 */
		bool loadAllPlugins();

		/**
		 * Unloads all loaded plugins.
		 * @return True, if all plugins could be unloaded successfully
		 */
		bool unloadAllPlugins();

		/**
		 * Returns all available plugins.
		 * @return Plugin names
		 */
		Names plugins() const;

		/**
		 * Returns all loaded plugins.
		 * @return Plugin names
		 */
		Names loadedPlugins() const;

		/**
		 * Returns all not loaded plugins.
		 * @return Plugin names
		 */
		Names unloadedPlugins() const;

		/**
		 * Releases the plugin manager and unloads all plugins.
		 */
		void release();

	private:

		/**
		 * Creates a new plugin manager object.
		 */
		PluginManager();

		/**
		 * Destructs a plugin manager object.
		 */
		~PluginManager();

		/**
		 * Determines whether a given file is a plugin.
		 * @param filename Filename of the potential plugin
		 * @param plugin Resulting plugin object, if succeeded
		 * @return True, if succeeded
		 */
		static bool determinePlugin(const std::string& filename, Plugin& plugin);

	#if defined(__APPLE__)

		/**
		 * Determines whether a given file is a plugin (specialization for Apple platforms)
		 * @sa determinePlugin()
		 * @param filename Filename of the potential plugin
		 * @param plugin Resulting plugin object, if succeeded
		 * @return True, if succeeded
		 */
		static bool determinePluginApple(const std::string& filename, Plugin& plugin);

	#endif // defined(__APPLE__)

		/**
		 * Translates a plugin type string to a plugin type id.
		 * @param type Plugin type string to translate
		 * @return Plugin type id
		 */
		static PluginType translateType(const std::string& type);

	private:

		/// Vector holding all collected plugins.
		Plugins collectedPlugins_;

		/// Vector holding all loaded plugins.
		Plugins loadedPlugins_;

		/// File extension for plugins.
		std::string pluginFileExtension_;

		/// Lock for the manager.
		mutable Lock lock_;
};

inline const std::string& PluginManager::Plugin::filename() const
{
	return filename_;
}

inline const std::string& PluginManager::Plugin::name() const
{
	return name_;
}

inline const std::string& PluginManager::Plugin::description() const
{
	return description_;
}

inline const std::string& PluginManager::Plugin::thirdpartyInformation() const
{
	return thirdpartyInformation_;
}

inline PluginManager::PluginType PluginManager::Plugin::type() const
{
	return type_;
}

inline const std::string& PluginManager::Plugin::thirdpartyDependences() const
{
	return thirdpartyDependences_;
}

inline const std::string& PluginManager::Plugin::thirdpartyDescription() const
{
	return thirdpartyDescription_;
}

inline const std::string& PluginManager::fileExtension() const
{
	return pluginFileExtension_;
}

}

#endif // META_OCEAN_BASE_PLUGIN_MANAGER_H
