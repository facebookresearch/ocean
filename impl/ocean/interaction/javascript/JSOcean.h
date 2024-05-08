/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_INTERACTION_JS_OCEAN_H
#define META_OCEAN_INTERACTION_JS_OCEAN_H

#include "ocean/interaction/javascript/JavaScript.h"
#include "ocean/interaction/javascript/JSBase.h"
#include "ocean/interaction/javascript/JSExternal.h"

#include "ocean/base/ApplicationInterface.h"
#include "ocean/base/Singleton.h"

#include "ocean/io/FileConfig.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

/**
 * This class implements the JavaScript Ocean objevct.
 * @ingroup interactionjs
 */
class OCEAN_INTERACTION_JS_EXPORT JSOcean : public JSBase
{
	friend class JSLibrary;

	public:

		/**
		 * This class holds a global ocean object.
		 */
		class OceanContainer : public Singleton<OceanContainer>
		{
			friend class Singleton<OceanContainer>;

			private:

				/**
				 * Definition of a pair combining a JSContext object with a function name.
				 */
				typedef std::pair<std::weak_ptr<JSContext>, std::string> ContextFunctionPair;

				/**
				 * Definition of a vector holding ContextFunctionPair objects.
				 */
				typedef std::vector<ContextFunctionPair> ContextFunctionPairs;

			public:

				/**
				 * Destructs the global ocean object.
				 */
				virtual ~OceanContainer();

				/**
				 * Adds a new content added event function.
				 * @param function The function to be added
				 */
				void addContentAddedEventFunction(const v8::Local<v8::Function>& function);

				/**
				 * Adds a new content removed event function.
				 * @param function The function to be added
				 */
				void addContentRemovedEventFunction(const v8::Local<v8::Function>& function);

				/**
				 * Removes a new content added event function.
				 * @param function The function to be removed
				 */
				void removeContentAddedEventFunction(const v8::Local<v8::Function>& function);

				/**
				 * Removes a new content removed event function.
				 * @param function The function to be removed
				 */
				void removeContentRemovedEventFunction(const v8::Local<v8::Function>& function);

				/**
				 * Releases all event callback functions.
				 */
				void release();

			private:

				/**
				 * On content event function.
				 * @param eventId Id of the event
				 * @param value Event value
				 * @param state Event state
				 */
				void onContentEvent(const ApplicationInterface::EventId eventId, const bool value, const bool state);

				/**
				 * Creates JavaScript parameters of one given values.
				 * @param eventId Id of the event
				 * @param state State value
				 */
				static std::vector<v8::Handle<v8::Value>> parameter(const ApplicationInterface::EventId eventId, const bool state);

			private:

				/// Functions for content added events.
				ContextFunctionPairs contentAddedEventFunctions_;

				/// Functions for content removed events.
				ContextFunctionPairs contentRemovedEventFunctions_;

				/// File config object.
				IO::FileConfig containerFileConfig;

				/// Lock object.
				Lock lock_;
		};

	public:

		/**
		 * Returns the JavaScript name of this object.
		 * @return The object's JavaScript name
		 */
		static inline const char* objectName();

		/**
		 * Returns the object template for this object.
		 * @return The object template
		 */
		static inline v8::Local<v8::ObjectTemplate> objectTemplate();

		/**
		 * Releases the function and object template for this object.
		 */
		static void release();

	protected:

		/**
		 * Creates the function object template for this object.
		 */
		static void createObjectTemplate();

		/**
		 * Callback function for 'debug()' function.
		 * @param info The function callback info
		 */
		static void functionDebug(const v8::FunctionCallbackInfo<v8::Value>& info);

		/**
		 * Callback function for 'info()' function.
		 * @param info The function callback info
		 */
		static void functionInfo(const v8::FunctionCallbackInfo<v8::Value>& info);

		/**
		 * Callback function for 'warning()' function.
		 * @param info The function callback info
		 */
		static void functionWarning(const v8::FunctionCallbackInfo<v8::Value>& info);

		/**
		 * Callback function for 'error()' function.
		 * @param info The function callback info
		 */
		static void functionError(const v8::FunctionCallbackInfo<v8::Value>& info);

		/**
		 * Callback function for 'platformType()' function.
		 * @param info The function callback info
		 */
		static void functionPlatformType(const v8::FunctionCallbackInfo<v8::Value>& info);

		/**
		 * Callback function for 'architectureType()' function.
		 * @param info The function callback info
		 */
		static void functionArchitectureType(const v8::FunctionCallbackInfo<v8::Value>& info);

		/**
		 * Callback function for 'releaseType()' function.
		 * @param info The function callback info
		 */
		static void functionReleaseType(const v8::FunctionCallbackInfo<v8::Value>& info);

		/**
		 * Callback function for 'compilerVersion()' function.
		 * @param info The function callback info
		 */
		static void functionCompilerVersion(const v8::FunctionCallbackInfo<v8::Value>& info);

		/**
		 * Callback function for 'addContent()' function.
		 * @param info The function callback info
		 */
		static void functionAddContent(const v8::FunctionCallbackInfo<v8::Value>& info);

		/**
		 * Callback function for 'removeContent()' function.
		 * @param info The function callback info
		 */
		static void functionRemoveContent(const v8::FunctionCallbackInfo<v8::Value>& info);

		/**
		 * Callback function for 'setContentAddedEventFunction()' function.
		 * @param info The function callback info
		 */
		static void functionSetContentAddedEventFunction(const v8::FunctionCallbackInfo<v8::Value>& info);

		/**
		 * Callback function for 'setContentRemovedEventFunction()' function.
		 * @param info The function callback info
		 */
		static void functionSetContentRemovedEventFunction(const v8::FunctionCallbackInfo<v8::Value>& info);

		/**
		 * Callback function for 'timestamp()' function.
		 * @param info The function callback info
		 */
		static void functionTimestamp(const v8::FunctionCallbackInfo<v8::Value>& info);

		/**
		 * Pushes information into a message object.
		 * @param messageObject The message object to which the information will be pushed
		 * @param info The function callback info
		 */
		template <bool tActive>
		static void pushMessage(MessageObject<tActive>& messageObject, const v8::FunctionCallbackInfo<v8::Value>& info);

		/**
		 * Resolves the filenames from a given java script argument.
		 * @param info The function callback info
		 * @return Resolved filenames
		 */
		static ApplicationInterface::StringVector resolveFilenames(const v8::FunctionCallbackInfo<v8::Value>& info);

	protected:

		/// Object template for the Vector2 object.
		static v8::Persistent<v8::ObjectTemplate> objectTemplate_;
};

inline const char* JSOcean::objectName()
{
	return "ocean";
}

inline v8::Local<v8::ObjectTemplate> JSOcean::objectTemplate()
{
	if (objectTemplate_.IsEmpty())
	{
		createObjectTemplate();
	}

	ocean_assert(objectTemplate_.IsEmpty() == false);
	return objectTemplate_.Get(v8::Isolate::GetCurrent());
}

inline void JSOcean::release()
{
	ocean_assert(!objectTemplate_.IsEmpty());
	objectTemplate_.Reset();
}

template <bool tActive>
void JSOcean::pushMessage(MessageObject<tActive>& messageObject, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	ocean_assert(info.Length() > 0);

	std::string message;

	for (int n = 0; n < info.Length(); n++)
	{
		message += toAString(info[n]->ToDetailString(JSContext::currentContext()));
	}

	messageObject << message;
}

}

}

}

#endif // META_OCEAN_INTERACTION_JS_OCEAN_H
