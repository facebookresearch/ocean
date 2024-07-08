/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_INTERACTION_JS_OBJECT_H
#define META_OCEAN_INTERACTION_JS_OBJECT_H

#include "ocean/interaction/javascript/JavaScript.h"
#include "ocean/interaction/javascript/JSBase.h"

#include <v8.h>

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

/**
 * This class implements the base class for all native wrapper of JavaScript objects.
 * @ingroup interactionjs
 */
template <typename T, typename TNative>
class JSObject : public JSBase
{
	public:

		/**
		 * Definition of the native data type which is wrapped by this Java Script object.
		 */
		typedef TNative NativeType;

	public:

		/**
		 * Returns the function template for this object.
		 * @return The function template
		 */
		static inline v8::Local<v8::FunctionTemplate> functionTemplate();

		/**
		 * Returns the object template for this object.
		 * @return The object template
		 */
		static inline v8::Local<v8::ObjectTemplate> objectTemplate();

		/**
		 * Creates an new wrapped object.
		 * @param value The value of the new object, will be moved
		 * @param context The context in which the object will be created
		 * @return The new JavaScript object
		 */
		static inline v8::Local<v8::Value> create(NativeType&& value, const v8::Local<v8::Context>& context);

		/**
		 * Releases the function and object template for this object.
		 */
		static void release();

	protected:

		/// Function template for the Vector2 object.
		static v8::Persistent<v8::FunctionTemplate> functionTemplate_;

		/// Object template for the Vector2 object.
		static v8::Persistent<v8::ObjectTemplate> objectTemplate_;
};

template <typename T, typename TNative>
v8::Persistent<v8::FunctionTemplate> JSObject<T, TNative>::functionTemplate_;

template <typename T, typename TNative>
v8::Persistent<v8::ObjectTemplate> JSObject<T, TNative>::objectTemplate_;

template <typename T, typename TNative>
inline v8::Local<v8::FunctionTemplate> JSObject<T, TNative>::functionTemplate()
{
	if (functionTemplate_.IsEmpty())
	{
		T::createFunctionTemplate();
	}

	ocean_assert(!functionTemplate_.IsEmpty());

	return functionTemplate_.Get(v8::Isolate::GetCurrent());
}

template <typename T, typename TNative>
inline v8::Local<v8::ObjectTemplate> JSObject<T, TNative>::objectTemplate()
{
	ocean_assert(!objectTemplate_.IsEmpty());
	return objectTemplate_.Get(v8::Isolate::GetCurrent());
}

template <typename T, typename TNative>
inline v8::Local<v8::Value> JSObject<T, TNative>::create(NativeType&& value, const v8::Local<v8::Context>& context)
{
	return createObject<T>(std::move(value), context);
}

template <typename T, typename TNative>
void JSObject<T, TNative>::release()
{
	ocean_assert(!functionTemplate_.IsEmpty());
	functionTemplate_.Reset();

	ocean_assert(!objectTemplate_.IsEmpty());
	objectTemplate_.Reset();
}

}

}

}

#endif // META_OCEAN_INTERACTION_JS_OBJECT_H
