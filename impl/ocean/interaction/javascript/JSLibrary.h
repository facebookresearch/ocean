/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_INTERACTION_JS_LIBRARY_H
#define META_OCEAN_INTERACTION_JS_LIBRARY_H

#include "ocean/interaction/javascript/JavaScript.h"
#include "ocean/interaction/javascript/JSContext.h"

#include "ocean/interaction/Library.h"

#include <vector>

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

/**
 * This class implements the java script interaction library object.
 * @ingroup interactionjs
 */
class OCEAN_INTERACTION_JS_EXPORT JSLibrary : public Library
{
	friend class JSContext;

	protected:

		/**
		 * Definition of a vector holding JavaScript contexts.
		 */
		typedef std::vector<std::shared_ptr<JSContext>> JSContexts;

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

	protected:

		/**
		 * Creates a new java script library object.
		 */
		JSLibrary();

		/**
		 * Destructs a java script library object.
		 */
		~JSLibrary() override;

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
		 * Unloads all loaded scripts.
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
		 * If a JavaScript interaction file returns a different timestmap this timestamp is used for the next interaction file and so on.<br>
		 * Therefore, the last registered JavaScript interaction file is able to force the preferred timestamp.<br>
		 * However, different interaction libraries / modules can change the timestamp if they are executed after later.<br>
		 * @see Library::preUpdate().
		 */
		Timestamp preUpdate(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp) override;

		/**
		 * Post update interaction function.
		 * @see Library::postUpdate().
		 */
		void postUpdate(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp) override;

		/**
		 * Mouse press event function.
		 * @see Library::onMousePress().
		 */
		void onMousePress(const UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const Rendering::ObjectId pickedObject, const Vector3& pickedPosition, const Timestamp timestamp) override;

		/**
		 * Mouse move event function.
		 * @see Library::onMouseMove().
		 */
		void onMouseMove(const UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const Rendering::ObjectId pickedObject, const Vector3& pickedPosition, const Timestamp timestamp) override;

		/**
		 * Mouse release event function.
		 * @see Library::onMouseRelease().
		 */
		void onMouseRelease(const UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const Rendering::ObjectId pickedObject, const Vector3& pickedPosition, const Timestamp timestamp) override;

		/**
		 * Key press function.
		 * @see Library::onKeyPress().
		 */
		void onKeyPress(const UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp) override;

		/**
		 * Key release function.
		 * @see Library::onKeyRelease().
		 */
		void onKeyRelease(const UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp) override;

		/**
		 * Returns the global template object holding definitions of all global custom JavaScript object templates (e.g. constructors of custom JavaScript object).
		 * @return The global template object
		 */
		v8::Local<v8::ObjectTemplate> globalTemplate();

		/**
		 * Releases the global template object.
		 */
		void releaseGlobalTemplate();

		/**
		 * Load function for additional java script file.
		 * @param info The function callback info
		 */
		static void functionLoad(const v8::FunctionCallbackInfo<v8::Value>& info);

		/**
		 * Translates a given rendering object id into a named picking object.
		 * @param engine Rendering engine
		 * @param objectId Id of the rendering object
		 * @return Resulting name of the picking object
		 */
		std::string translatePickingObject(const Rendering::EngineRef& engine, const Rendering::ObjectId objectId);

	protected:

		/// The platform for all contexts.
		std::unique_ptr<v8::Platform> platform_;

		/// The create parameters for the isolate.
		v8::Isolate::CreateParams createParams_;

		/// The isolate for all contexts.
		v8::Isolate* isolate_ = nullptr;

		/// Global template object holding definitions of all global custom JavaScript object templates (e.g. constructors of custom JavaScript object).
		v8::Persistent<v8::ObjectTemplate> globalTemplate_;

		/// All JavaScript context objects.
		JSContexts jsContexts_;

		/// The library's lock.
		Lock lock_;
};

}

}

}

#endif // META_OCEAN_INTERACTION_JS_LIBRARY_H
