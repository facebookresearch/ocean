/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_INTERACTION_JS_SCENE_DESCRIPTION_MANAGER_H
#define META_OCEAN_INTERACTION_JS_SCENE_DESCRIPTION_MANAGER_H

#include "ocean/interaction/javascript/JavaScript.h"
#include "ocean/interaction/javascript/JSBase.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

/**
 * This class implements a wrapper for a JavaScript SceneDescriptionManager object.
 * @ingroup interactionjs
 */
class OCEAN_INTERACTION_JS_EXPORT JSSceneDescriptionManager : public JSBase
{
	friend class JSLibrary;

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
		 * Callback function for 'findNode()' function.
		 * @param info The function callback info
		 */
		static void functionFindNode(const v8::FunctionCallbackInfo<v8::Value>& info);

		/**
		 * Callback function for 'findNodes()' function.
		 * @param info The function callback info
		 */
		static void functionFindNodes(const v8::FunctionCallbackInfo<v8::Value>& info);

		/**
		 * Callback function for 'libraries()' function.
		 * @param info The function callback info
		 */
		static void functionLibraries(const v8::FunctionCallbackInfo<v8::Value>& info);

	protected:

		/// Object template for the Vector2 object.
		static v8::Persistent<v8::ObjectTemplate> objectTemplate_;
};

inline const char* JSSceneDescriptionManager::objectName()
{
	return "SceneDescriptionManager";
}

inline v8::Local<v8::ObjectTemplate> JSSceneDescriptionManager::objectTemplate()
{
	if (objectTemplate_.IsEmpty())
	{
		createObjectTemplate();
	}

	ocean_assert(objectTemplate_.IsEmpty() == false);
	return objectTemplate_.Get(v8::Isolate::GetCurrent());
}

inline void JSSceneDescriptionManager::release()
{
	ocean_assert(!objectTemplate_.IsEmpty());
	objectTemplate_.Reset();
}

}

}

}

#endif // META_OCEAN_INTERACTION_JS_SCENE_DESCRIPTION_MANAGER_H
