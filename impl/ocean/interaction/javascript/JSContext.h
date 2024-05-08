/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_INTERACTION_JS_CONTEXT_H
#define META_OCEAN_INTERACTION_JS_CONTEXT_H

#include "ocean/interaction/javascript/JavaScript.h"
#include "ocean/interaction/javascript/JSScript.h"

#include "ocean/base/Caller.h"
#include "ocean/base/Timestamp.h"

#include "ocean/io/File.h"

#include "ocean/rendering/Engine.h"

#include <v8.h>

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

/**
 * This class implements a wrapper for a JavaScript context.
 * @ingroup interactionjs
 */
class OCEAN_INTERACTION_JS_EXPORT JSContext
{
	public:

		/**
		 * Definition of a vector holding JavaScript code objects.
		 */
		typedef std::vector<std::unique_ptr<JSScript>> Scripts;

		/**
		 * Definition of a vector holding value handles.
		 */
		typedef std::vector<v8::Handle<v8::Value>> Values;

		/**
		 * Definition of a caller object returning JavaScript parameters.
		 */
		typedef Caller<Values> FunctionParameterCaller;

	protected:

		/**
		 * Defines a pair holding function names and function parameters.
		 */
		typedef std::pair<std::string, FunctionParameterCaller> FunctionPair;

		/**
		 * Definition of a vector holding function pairs.
		 */
		typedef std::vector<FunctionPair> FunctionPairs;

	public:

		/**
		 * Creates a new context object.
		 * @see initialize().
		 */
		JSContext() = default;

		/**
		 * Destructs a context object.
		 */
		~JSContext();

		/**
		 * Initializes this context.
		 * This function must be called after the context has been constructed.
		 * @param thisContext The shared pointer to this context, must be valid
		 * @param globalTemplate The global template object holding the definition of all wrapper classes and global objects not part of java script natively
		 */
		void initialize(const std::shared_ptr<JSContext>& thisContext, const v8::Local<v8::ObjectTemplate>& globalTemplate);

		/**
		 * Returns the JavaScript context object.
		 * @return Context object
		 */
		inline v8::Local<v8::Context> context() const;

		/**
		 * Returns a corresponding filenames of the scripts managed by this context object
		 * @return Names of all script files
		 */
		inline const std::vector<std::string>& filenames() const;

		/**
		 * Adds a new script to this context.
		 * @param code Code of the new script
		 * @return True, if succeeded to compile and to add the code
		 */
		bool addScriptCode(const std::string& code);

		/**
		 * Adds a new script to this context.
		 * @param filename Name of the script file to add
		 * @return True, if succeeded to compile and to add the code
		 */
		bool addScriptFile(const std::string& filename);

		/**
		 * Initialize function.
		 * @param engine Current rendering engine
		 * @param timestamp Recent initialization timestamp
		 */
		void onInitialize(const Rendering::EngineRef& engine, const Timestamp timestamp);

		/**
		 * Release function.
		 * @param engine Current rendering engine
		 * @param timestamp Recent release timestamp
		 */
		void onRelease(const Rendering::EngineRef& engine, const Timestamp timestamp);

		/**
		 * Calls a given JavaScript function.
		 * @param function JavaScript function to call
		 * @param returnValue Optional JavaScript function return value
		 * @param parameters Optional function parameters
		 * @param errorMessage Optional resulting error message
		 * @return True, if succeeded
		 */
		bool callFunction(v8::Local<v8::Function> function, v8::Local<v8::Value>& returnValue, Values& parameters, std::string& errorMessage);

		/**
		 * Executes a spcific script function.
		 * @param function Name of the function to execute
		 * @param returnValue Resulting return value, if any
		 * @param parameters Optional function parameter
		 * @param errorMessage Resulting error message, if any
		 * @return True, if succeeded
		 */
		bool callFunction(const std::string& function, v8::Local<v8::Value>& returnValue, Values& parameters, std::string& errorMessage);

		/**
		 * Adds a new function to the queue of explicit functions.
		 * @param function Name of the function to be executed
		 * @param parameterCreator Caller function creating the parameters
		 */
		void addExplicitFunctionCall(const std::string& function, const FunctionParameterCaller& parameterCreator);

		/**
		 * Resolves the entire path of a given file.
		 * @param file File to resolve
		 * @return All possible resolved files
		 */
		IO::Files resolveFile(const IO::File& file);

		/**
		 * Resolves the entire path of a given file to the first unique existing file
		 * @param file File to resolve
		 * @param resolved Path of the unique existing file
		 * @return True, if the file could be resolved
		 */
		bool resolveFile(const IO::File& file, std::string& resolved);

		/**
		 * Pre file load interaction function.
		 * @see JSLibrary::preFileLoad().
		 */
		void preFileLoad(const std::string& filename);

		/**
		 * Post file load interaction function.
		 * @see JSLibrary::postFileLoad().
		 */
		void postFileLoad(const std::string& filename, const bool succeeded);

		/**
		 * Pre update interaction function.
		 * The returned timestamp is used for the next JavaScript interaction file.
		 * @see JSLibrary::preUpdate().
		 */
		Timestamp preUpdate(const Rendering::EngineRef& engine, const Timestamp timestamp);

		/**
		 * Post update interaction function.
		 * @see JSLibrary::postUpdate().
		 */
		void postUpdate(const Rendering::EngineRef& engine, const Timestamp timestamp);

		/**
		 * Mouse press event function.
		 * @see JSLibrary::onMousePress().
		 */
		void onMousePress(const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const std::string& pickedObject, const Vector3& pickedPosition, const Timestamp timestamp);

		/**
		 * Mouse move event function.
		 * @see JSLibrary::onMouseMove().
		 */
		void onMouseMove(const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const std::string& pickedObject, const Vector3& pickedPosition, const Timestamp timestamp);

		/**
		 * Mouse release event function.
		 * @see JSLibrary::onMouseRelease().
		 */
		void onMouseRelease(const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const std::string& pickedObject, const Vector3& pickedPosition, const Timestamp timestamp);

		/**
		 * Key press function.
		 * @see JSLibrary::onKeyPress().
		 */
		void onKeyPress(const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp);

		/**
		 * Key release function.
		 * @see JSLibrary::onKeyRelease().
		 */
		void onKeyRelease(const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp);

		/**
		 * Returns the current JavaScript context object.
		 * @return JavaScript current context
		 */
		static inline const v8::Local<v8::Context>& currentContext();

		/**
		 * Returns the current JSContext object.
		 * @return The current JSContext object
		 */
		static inline std::shared_ptr<JSContext> currentJSContext();

	protected:

		/**
		 * Disabled copy constructor.
		 * @param context Object which would be copied
		 */
		JSContext(const JSContext& context) = delete;

		/**
		 * Makes this context the current context.
		 */
		void makeCurrent();

		/**
		 * Returns whether the script contains a specific function.
		 * @param function Name of the function to check
		 * @return True, if so
		 */
		bool hasFunction(const std::string& function) const;

		/**
		 * Returns whether the script contains a specific global object.
		 * @param object Name of the object to check
		 * @return True, if so
		 */
		bool hasObject(const std::string& object) const;

		/**
		 * Disabled copy operator.
		 * @param context Object which would be copied
		 * @return Reference to this object
		 */
		JSContext& operator=(const JSContext& context) = delete;

	protected:

		/// Script code objects.
		Scripts scripts_;

		/// JavaScript context object handle.
		v8::UniquePersistent<v8::Context> context_;

		/// The reference to this object.
		std::weak_ptr<JSContext> thisJSContext_;

		/// Name of the file the initial script is defined inside.
		std::vector<std::string> filenames_;

		/// True, if the context contains a preUpdate function.
		bool hasPreUpdateFunction_ = false;

		/// True, if the context contains a postUpdate function.
		bool hasPostUpdateFunction_ = false;

		/// True, if the context contains a mousePress event function.
		bool hasMousePressEventFunction_ = false;

		/// True, if the context contains a mouseMove event function.
		bool hasMouseMoveEventFunction_ = false;

		/// True, if the context contains a mouseRelease event function.
		bool hasMouseReleaseEventFunction_ = false;

		/// True, if the context contains a keyPress event function.
		bool hasKeyPressFunction_ = false;

		/// True, if the context contains a keyRelease event function.
		bool hasKeyReleaseFunction_ = false;

		/// Functions to be called in the next update step explicitly.
		FunctionPairs explicitFunctions_;

		/// The context's lock.
		Lock lock_;

		/// The current JavaScript context.
		static v8::Local<v8::Context> currentContext_;

		/// The current context object.
		static std::weak_ptr<JSContext> currentJSContext_;

		/// Global counter for context objects interested in mouse events.
		static unsigned int mouseEventCounter_;
};

inline v8::Local<v8::Context> JSContext::context() const
{
	return context_.Get(v8::Isolate::GetCurrent());
}

inline std::shared_ptr<JSContext> JSContext::currentJSContext()
{
	return currentJSContext_.lock();
}

inline const std::vector<std::string>& JSContext::filenames() const
{
	return filenames_;
}

inline const v8::Local<v8::Context>& JSContext::currentContext()
{
	return currentContext_;
}

}

}

}

#endif // META_OCEAN_INTERACTION_JS_CONTEXT_H
