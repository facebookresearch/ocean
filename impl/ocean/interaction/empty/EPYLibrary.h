/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_INTERACTION_EPY_LIBRARY_H
#define META_OCEAN_INTERACTION_EPY_LIBRARY_H

#include "ocean/interaction/empty/Empty.h"

#include "ocean/interaction/Library.h"

#include <vector>

namespace Ocean
{

namespace Interaction
{

namespace Empty
{

/**
 * This class implements the empty demo interaction library object.
 * @ingroup interactionepy
 */
class OCEAN_INTERACTION_EPY_EXPORT EPYLibrary : public Library
{
	public:

		/**
		 * Creates this library and registeres it at the global interaction manager.<br>
		 * Do not register this library if using it as plugin, because it's done by the plugin itself.<br>
		 * However, if you are not using the plugin mechanism you have to initialize this library once at program initialization.<br>
		 * If the library is not used anymore unregister it using the unregister function.<br>
		 * Beware: This registration must not be done more than once!<br>
		 * @see Manager, unregisterLibrary()
		 */
		static void registerLibrary();

		/**
		 * Unregisters this library at the global interaction manager.
		 * Do not unregister this library if using it as plugin, because it's done by the plugin itself when the plugin is unloaded.<br>
		 * Beware: This registration must not be done more than once and must not be done without previous registration!<br>
		 * @return True, if succeeded
		 * @see Manager, registerLibrary()
		 */
		static bool unregisterLibrary();

		/**
		 * Loads a new interaction file.
		 * @see Library::load().
		 */
		bool load(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& filename) override;

		/**
		 * Unloads one specific loaded interaction files.
		 * @see Library::unload().
		 */
		bool unload(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& filename) override;

		/**
		 * Unloads all loaded interaction files.
		 * @see Library::unload
		 */
		void unload(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp) override;

		/**
		 * Pre file load interaction function.
		 * @see Library::preFileLoad().
		 */
		void preFileLoad(const UserInterface& userInterface, const std::string& filename) override;

		/**
		 * Post file load interaction function.
		 * @see Library::postFileLoad().
		 */
		void postFileLoad(const UserInterface& userInterface, const std::string& filename, const bool succeeded) override;

		/**
		 * Pre update interaction function.
		 * @see Library::preUpdate().
		 */
		Timestamp preUpdate(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp) override;

		/**
		 * Post update interaction function.
		 * @see Library::postUpdate().
		 */
		void postUpdate(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp) override;

		/**
		 * Key press function.
		 * @see Library::onKeyPress().
		 */
		void onKeyPress(const UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp) override;

		/**
		 * Key press function.
		 * @see Library::onKeyRelease().
		 */
		void onKeyRelease(const UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp) override;

	protected:

		/**
		 * Creates a new Empty Demo Interaction Library object.
		 */
		EPYLibrary();

		/**
		 * Destructs an Empty Demo Interaction Library object.
		 */
		~EPYLibrary() override;

	protected:

		/// Interaction library lock.
		Lock lock_;
};

}

}

}

#endif // META_OCEAN_INTERACTION_EPY_LIBRARY_H
