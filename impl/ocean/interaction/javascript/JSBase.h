/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_INTERACTION_JS_BASE_H
#define META_OCEAN_INTERACTION_JS_BASE_H

#include "ocean/interaction/javascript/JavaScript.h"
#include "ocean/interaction/javascript/JSContext.h"
#include "ocean/interaction/javascript/JSExternal.h"

#include "ocean/io/File.h"
#include "ocean/io/FileResolver.h"

#include <v8.h>

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

/**
 * This class implements the base class for all JavaScript wrappers.
 * @ingroup interactionjs
 */
class OCEAN_INTERACTION_JS_EXPORT JSBase
{
	public:

		/**
		 * Returns the std string of a given JavaScript string object.
		 * @param value JavaScript value holding a string
		 * @return Readable string value
		 */
		static std::string toAString(const v8::Local<v8::String>& value);

		/**
		 * Returns the std string of a given JavaScript string object.
		 * @param value JavaScript value holding a string
		 * @return Readable string value
		 */
		static std::string toAString(v8::MaybeLocal<v8::String> value);

		/**
		 * Creates a JavaScript string object from a given std string.
		 * @param value The string for which the JavaScript object will be created
		 * @param isolate The current v8 isolate, must be valid
		 * @return The JavaScript string object
		 */
		static inline v8::Local<v8::String> newString(const std::string& value, v8::Isolate* isolate);

		/**
		 * Creates a JavaScript string object from a given std string.
		 * @param value The string for which the JavaScript object will be created
		 * @param isolate The current v8 isolate, must be valid
		 * @return The JavaScript string object
		 */
		static inline v8::Local<v8::String> newString(const char* value, v8::Isolate* isolate);

	protected:

		/**
		 * Creates an JavaScript array value for a given C++ array.
		 * @param values The C++ array for which the JavaScript value will be created
		 * @param context The current JavaScript context
		 * @return The resulting JavaScript value
		 * @tparam TNative The native C++ data type of all array elements
		 */
		template <typename TNative>
		static inline v8::Local<v8::Value> createValues(const std::vector<TNative>& values, const v8::Local<v8::Context>& context);

		/**
		 * Creates an JavaScript array value for a given C++ array.
		 * @param values The C++ array for which the JavaScript value will be created
		 * @param size The number of elements int he array, with range [0, infinity)
		 * @param context The current JavaScript context
		 * @return The resulting JavaScript value
		 * @tparam TNative The native C++ data type of all array elements
		 */
		template <typename TNative>
		static v8::Local<v8::Value> createValues(const TNative* values, const size_t size, const v8::Local<v8::Context>& context);

		/**
		 * Creates an JavaScript value for a given C++ wrapper object.
		 * @param value The C++ wrapper object for which the JavaScript value will be created, will be moved
		 * @param context The current JavaScript context
		 * @return The resulting JavaScript value
		 * @tparam T The data type of the C++ wrapper object
		 */
		template <typename T>
		static v8::Local<v8::Value> createObject(typename T::NativeType&& value, const v8::Local<v8::Context>& context);

		/**
		 * Creates an JavaScript value for a given C++ wrapper object.
		 * @param value The C++ wrapper object for which the JavaScript value will be created
		 * @param context The current JavaScript context
		 * @return The resulting JavaScript value
		 * @tparam T The data type of the C++ wrapper object
		 */
		template <typename T>
		static v8::Local<v8::Value> createObject(const typename T::NativeType& value, const v8::Local<v8::Context>& context);

		/**
		 * Creates an JavaScript array value for given C++ wrapper objects.
		 * @param values The C++ wrapper objects for which the JavaScript value will be created
		 * @param context The current JavaScript context
		 * @return The resulting JavaScript value
		 * @tparam T The data type of the C++ wrapper object
		 */
		template <typename T>
		static v8::Local<v8::Array> createObjects(const std::vector<typename T::NativeType>& values, const v8::Local<v8::Context>& context);

		/**
		 * Returns whether a JavaScript function call holds a specific native type as parameter.
		 * @param info The function callback info
		 * @param index The index of the function parameter, with range [0, infinity)
		 * @param value The resulting native value of the JavaScript value in case the parameter exists
		 * @return True, if the function has at least `index + 1 parameters and if the parameter type fits with `TNative`
		 * @tparam TNative The data type of the native C++ wrapper object
		 */
		template <typename TNative>
		static bool hasValue(const v8::FunctionCallbackInfo<v8::Value>& info, const unsigned int index, TNative& value);

		/**
		 * Returns whether a JavaScript function call holds a specific native type as parameter.
		 * @param info The function callback info
		 * @param index The index of the function parameter, with range [0, infinity)
		 * @param value The resulting native value of the JavaScript value in case the parameter exists
		 * @return True, if the function has at least `index + 1 parameters and if the parameter type fits with `TNative`
		 * @tparam TNative The data type of the native C++ wrapper object
		 */
		template <typename TNative>
		static bool hasValue(const v8::FunctionCallbackInfo<v8::Value>& info, const unsigned int index, std::vector<TNative>& value);

		/**
		 * Returns whether a JavaScript function call holds a sequence of specific native type as parameters.
		 * @param info The function callback info
		 * @param startIndex The start index of the function parameter to be checked, with range [0, infinity)
		 * @param endIndex The end index (exclusive) of the function parameter to be checked, with range [startIndex + 1, infinity)
		 * @param values The resulting native values of the JavaScript value in case several consecutive parameter exist
		 * @return The number of consecutive values with native data type `TNative`, with range [0, endIndex - startIndex]
		 * @tparam TNative The data type of the native C++ wrapper object
		 */
		template <typename TNative>
		static unsigned int hasValues(const v8::FunctionCallbackInfo<v8::Value>& info, const unsigned int startIndex, const unsigned int endIndex, TNative* values);

		/**
		 * Returns whether a JavaScript value wraps a specified native type.
		 * @param localValue The JavaScript value to check
		 * @param value The resulting native value of the JavaScript value in case the parameter exists
		 * @tparam TNative The data type of the native C++ wrapper object
		 * @return True, if the function has at least `index + 1 parameters and if the parameter type fits with `TNative`
		 */
		template <typename TNative>
		static bool isValue(v8::Local<v8::Value> localValue, TNative& value);

		/**
		 * The callback function for all JavaScript constructors of wrapped C++ objects.
		 * @param info The function callback info
		 * @tparam TNative The data type of the native C++ wrapper object
		 */
		template <typename TNative>
		static void constructor(const v8::FunctionCallbackInfo<v8::Value>& info);

		/**
		 * The specialized callback function for all JavaScript constructors of wrapped C++ objects.
		 * @param thisValue The wrapped native C++ object which can be changed in the constructor
		 * @param info The function callback info
		 * @tparam TNative The data type of the native C++ wrapper object
		 */
		template <typename TNative>
		static void constructor(TNative& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info);

		/**
		 * The callback function for all JavaScript getters of wrapped C++ objects.
		 * @param property The property of the getter
		 * @param info The function callback info
		 * @tparam T The data type of the C++ wrapper object
		 * @tparam tGetterId The unique id of the getter, unique in combination with `T`
		 */
		template <typename T, unsigned int tGetterId>
		static inline void propertyGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);

		/**
		 * The specialized callback function for all JavaScript getters of wrapped C++ objects.
		 * This function will be template-specialized in the individual wrapper classes.
		 * @param thisValue The wrapped native C++ object
		 * @param property The property of the getter
		 * @param info The function callback info
		 * @tparam TNative The data type of the native C++ wrapper object
		 * @tparam tGetterId The unique id of the getter
		 */
		template <typename TNative, unsigned int tGetterId>
		static void propertyGetter(TNative& thisValue, v8::Local<v8::String>& property, const v8::PropertyCallbackInfo<v8::Value>& info);

		/**
		 * The callback function for all JavaScript setters of wrapped C++ objects.
		 * @param property The property of the setter
		 * @param value The value to be set
		 * @param info The function callback info
		 * @tparam T The data type of the C++ wrapper object
		 * @tparam tSetterId The unique id of the setter, unique in combination with `T`
		 */
		template <typename T, unsigned int tSetterId>
		static inline void propertySetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);

		/**
		 * The specialized callback function for all JavaScript setters of wrapped C++ objects.
		 * This function will be template-specialized in the individual wrapper classes.
		 * @param thisValue The wrapped native C++ object
		 * @param property The property of the setter
		 * @param value The value to be set
		 * @param info The function callback info
		 * @tparam TNative The data type of the native C++ wrapper object
		 * @tparam tSetterId The unique id of the setter
		 */
		template <typename TNative, unsigned int tSetterId>
		static void propertySetter(TNative& thisValue, v8::Local<v8::String>& property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);

		/**
		 * The callback function for all JavaScript functions of wrapped C++ objects.
		 * @param info The function callback info
		 * @tparam T The data type of the C++ wrapper object
		 * @tparam tFunctionId The unique id of the function, unique in combination with `T`
		 */
		template <typename T, unsigned int tFunctionId>
		static inline void function(const v8::FunctionCallbackInfo<v8::Value>& info);

		/**
		 * The specialized callback function for all JavaScript functions of wrapped C++ objects.
		 * This function will be template-specialized in the individual wrapper classes.
		 * @param thisValue The wrapped native C++ object
		 * @param info The function callback info
		 * @tparam TNative The data type of the native C++ wrapper object
		 * @tparam tFunctionId The unique id of the function
		 */
		template <typename T, unsigned int tFunctionId>
		static void function(T& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info);
};

template <>
inline v8::Local<v8::Value> JSBase::createValues(const std::vector<bool>& values, const v8::Local<v8::Context>& context)
{
	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	v8::Local<v8::Array> result = v8::Array::New(isolate, int(values.size()));

	for (size_t n = 0; n < values.size(); ++n)
	{
		if (!result->Set(context, v8::Int32::New(isolate, int(n)), v8::Boolean::New(isolate, values[n])).IsJust())
		{
			ocean_assert(false && "Invalid value!");
		}
	}

	return result;
}

template <typename TNative>
v8::Local<v8::Value> JSBase::createValues(const std::vector<TNative>& values, const v8::Local<v8::Context>& context)
{
	return createValues<TNative>(values.data(), values.size(), context);
}

template <>
inline v8::Local<v8::Value> JSBase::createValues(const int* values, const size_t size, const v8::Local<v8::Context>& context)
{
	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	v8::Local<v8::Array> result = v8::Array::New(isolate, int(size));

	for (size_t n = 0; n < size; ++n)
	{
		if (!result->Set(context, v8::Int32::New(isolate, int(n)), v8::Int32::New(isolate, values[n])).IsJust())
		{
			ocean_assert(false && "Invalid value!");
		}
	}

	return result;
}

template <>
inline v8::Local<v8::Value> JSBase::createValues(const float* values, const size_t size, const v8::Local<v8::Context>& context)
{
	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	v8::Local<v8::Array> result = v8::Array::New(isolate, int(size));

	for (size_t n = 0; n < size; ++n)
	{
		if (!result->Set(context, v8::Int32::New(isolate, int(n)), v8::Number::New(isolate, double(values[n]))).IsJust())
		{
			ocean_assert(false && "Invalid value!");
		}
	}

	return result;
}

template <>
inline v8::Local<v8::Value> JSBase::createValues(const double* values, const size_t size, const v8::Local<v8::Context>& context)
{
	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	v8::Local<v8::Array> result = v8::Array::New(isolate, int(size));

	for (size_t n = 0; n < size; ++n)
	{
		if (!result->Set(context, v8::Int32::New(isolate, int(n)), v8::Number::New(isolate, values[n])).IsJust())
		{
			ocean_assert(false && "Invalid value!");
		}
	}

	return result;
}

template <>
inline v8::Local<v8::Value> JSBase::createValues(const std::string* values, const size_t size, const v8::Local<v8::Context>& context)
{
	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	v8::Local<v8::Array> result = v8::Array::New(isolate, int(size));

	for (size_t n = 0; n < size; ++n)
	{
		if (!result->Set(context, v8::Int32::New(isolate, int(n)), newString(values[n], isolate)).IsJust())
		{
			ocean_assert(false && "Invalid value!");
		}
	}

	return result;
}

template <typename TNative>
inline v8::Local<v8::Value> JSBase::createValues(const TNative* values, const size_t size, const v8::Local<v8::Context>& context)
{
	static_assert(oceanFalse<TNative>(), "This function is not defined for the data type");
}

template <typename T>
v8::Local<v8::Value> JSBase::createObject(typename T::NativeType&& value, const v8::Local<v8::Context>& context)
{
	v8::MaybeLocal<v8::Object> maybeResult(T::objectTemplate()->NewInstance(context));

	if (maybeResult.IsEmpty())
	{
		ocean_assert(false && "Invalid object!");
		return v8::Local<v8::Value>();
	}

	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	v8::Local<v8::Object> result(maybeResult.ToLocalChecked());

	JSExternal* newExternal = JSExternal::create(new typename T::NativeType(std::move(value)), result, isolate);
	result->SetInternalField(0, v8::External::New(isolate, newExternal));

	return result;
}

template <typename T>
v8::Local<v8::Value> JSBase::createObject(const typename T::NativeType& value, const v8::Local<v8::Context>& context)
{
	v8::MaybeLocal<v8::Object> maybeResult(T::objectTemplate()->NewInstance(context));

	if (maybeResult.IsEmpty())
	{
		ocean_assert(false && "Invalid object!");
		return v8::Local<v8::Value>();
	}

	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	v8::Local<v8::Object> result(maybeResult.ToLocalChecked());

	JSExternal* newExternal = JSExternal::create(new typename T::NativeType(value), result, isolate);
	result->SetInternalField(0, v8::External::New(isolate, newExternal));

	return result;
}

template <typename T>
v8::Local<v8::Array> JSBase::createObjects(const std::vector<typename T::NativeType>& values, const v8::Local<v8::Context>& context)
{
	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	v8::Local<v8::Array> result = v8::Array::New(isolate, int(values.size()));

	for (size_t n = 0; n < values.size(); ++n)
	{
		if (!result->Set(context, v8::Int32::New(isolate, int(n)), JSBase::createObject<T>(values[n], context)).IsJust())
		{
			ocean_assert(false && "Invalid value!");
		}
	}

	return result;
}

inline v8::Local<v8::String> JSBase::newString(const std::string& value, v8::Isolate* isolate)
{
	ocean_assert(isolate != nullptr);

	if (value.empty())
	{
		return v8::Local<v8::String>();
	}

	v8::MaybeLocal<v8::String> maybeString = v8::String::NewFromUtf8(isolate, value.c_str());
	ocean_assert(!maybeString.IsEmpty());

	return maybeString.ToLocalChecked();
}

inline v8::Local<v8::String> JSBase::newString(const char* value, v8::Isolate* isolate)
{
	ocean_assert(isolate != nullptr);

	if (value == nullptr)
	{
		return v8::Local<v8::String>();
	}

	v8::MaybeLocal<v8::String> maybeString = v8::String::NewFromUtf8(isolate, value);
	ocean_assert(!maybeString.IsEmpty());

	return maybeString.ToLocalChecked();
}

template <typename TNative>
inline void JSBase::constructor(const v8::FunctionCallbackInfo<v8::Value>& info)
{
	ocean_assert(info.This()->InternalFieldCount() == 1);

	TNative* newValue = new TNative();

	constructor<TNative>(*newValue, info);

	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	v8::Local<v8::Object> ob = info.This();
	JSExternal* newExternal = JSExternal::create(newValue, ob, isolate);

	info.This()->SetInternalField(0,  v8::External::New(isolate, newExternal));
	info.GetReturnValue().Set(info.This());
}

template <typename T, unsigned int tFunctionId>
inline void JSBase::propertyGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	ocean_assert(info.Holder()->InternalFieldCount() == 1);
	const v8::Local<v8::External> thisWrapper(v8::Local<v8::External>::Cast(info.Holder()->GetInternalField(0)));

	JSExternal* thisExternal = static_cast<JSExternal*>(thisWrapper->Value());
	ocean_assert(thisExternal->type() == JSExternal::type<T>());

	propertyGetter<T, tFunctionId>(thisExternal->value<T>(), property, info);
}

template <typename T, unsigned int tFunctionId>
inline void JSBase::propertySetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
	ocean_assert(value.IsEmpty() == false);
	ocean_assert(info.Holder()->InternalFieldCount() == 1);

	const v8::Local<v8::External> thisWrapper(v8::Local<v8::External>::Cast(info.Holder()->GetInternalField(0)));

	JSExternal* thisExternal = static_cast<JSExternal*>(thisWrapper->Value());
	ocean_assert(thisExternal->type() == JSExternal::type<T>());

	propertySetter<T, tFunctionId>(thisExternal->value<T>(), property, value, info);
}

template <typename T, unsigned int tFunctionId>
inline void JSBase::function(const v8::FunctionCallbackInfo<v8::Value>& info)
{
	ocean_assert(info.This()->InternalFieldCount() == 1);
	v8::Local<v8::External> thisWrapper(v8::Local<v8::External>::Cast(info.This()->GetInternalField(0)));

	JSExternal* thisExternal = static_cast<JSExternal*>(thisWrapper->Value());
	ocean_assert(thisExternal->type() == JSExternal::type<T>());

	function<T, tFunctionId>(thisExternal->value<T>(), info);
}

template <>
inline bool JSBase::hasValue(const v8::FunctionCallbackInfo<v8::Value>& info, const unsigned int index, bool& value)
{
	if (info.Length() >= int(index + 1u))
	{
		if (info[index]->IsBoolean())
		{
#if defined(OCEAN_V8_VERSION) && OCEAN_V8_VERSION > 70000
			value = info[index]->BooleanValue(v8::Isolate::GetCurrent());
#else
			value = info[index]->BooleanValue(JSContext::currentContext()).FromJust();
#endif

			return true;
		}
	}

	return false;
}

template <>
inline bool JSBase::hasValue(const v8::FunctionCallbackInfo<v8::Value>& info, const unsigned int index, int& value)
{
	if (info.Length() >= int(index + 1u))
	{
		if (info[index]->IsInt32())
		{
			value = info[index]->Int32Value(JSContext::currentContext()).FromJust();

			return true;
		}
	}

	return false;
}

template <>
inline bool JSBase::hasValue(const v8::FunctionCallbackInfo<v8::Value>& info, const unsigned int index, double& value)
{
	if (info.Length() >= int(index + 1u))
	{
		if (info[index]->IsNumber())
		{
			value = info[index]->NumberValue(JSContext::currentContext()).FromJust();

			return true;
		}
	}

	return false;
}

template <>
inline bool JSBase::hasValue(const v8::FunctionCallbackInfo<v8::Value>& info, const unsigned int index, float& value)
{
	if (info.Length() >= int(index + 1u))
	{
		if (info[index]->IsNumber())
		{
			value = float(info[index]->NumberValue(JSContext::currentContext()).FromJust());

			return true;
		}
	}

	return false;
}

template <>
inline bool JSBase::hasValue(const v8::FunctionCallbackInfo<v8::Value>& info, const unsigned int index, std::string& value)
{
	if (info.Length() >= int(index + 1u))
	{
		if (info[index]->IsString())
		{
			value = toAString(info[index]->ToString(JSContext::currentContext()));

			return true;
		}
	}

	return false;
}

template <typename TNative>
inline bool JSBase::hasValue(const v8::FunctionCallbackInfo<v8::Value>& info, const unsigned int index, TNative& value)
{
	if (info.Length() >= int(index + 1u))
	{
		JSExternal* externalParameter = JSExternal::external(info[index]);

		if (externalParameter != nullptr && externalParameter->type() == JSExternal::type<TNative>())
		{
			value = externalParameter->value<TNative>();

			return true;
		}
	}

	return false;
}

template <>
inline bool JSBase::hasValue(const v8::FunctionCallbackInfo<v8::Value>& info, const unsigned int index, std::vector<bool>& value)
{
	if (info.Length() >= int(index + 1u))
	{
		if (info[index]->IsArray())
		{
			v8::Local<v8::Context> currentContext = JSContext::currentContext();
#if defined(OCEAN_V8_VERSION) && OCEAN_V8_VERSION > 70000
			v8::Isolate* isolate = v8::Isolate::GetCurrent();
#endif

			v8::Local<v8::Array> arrayValue = v8::Local<v8::Array>::Cast(info[index]);

			value.clear();
			value.reserve(arrayValue->Length());

			for (unsigned int n = 0; n < arrayValue->Length(); ++n)
			{
				v8::MaybeLocal<v8::Value> element = arrayValue->Get(currentContext, n);

				if (!element.IsEmpty() || !element.ToLocalChecked()->IsBoolean())
				{
					return false;
				}

#if defined(OCEAN_V8_VERSION) && OCEAN_V8_VERSION > 70000
				value.emplace_back(element.ToLocalChecked()->BooleanValue(isolate));
#else
				value.emplace_back(element.ToLocalChecked()->BooleanValue(currentContext).FromJust());
#endif
			}

			return true;
		}
	}

	return false;
}

template <>
inline bool JSBase::hasValue(const v8::FunctionCallbackInfo<v8::Value>& info, const unsigned int index, std::vector<int>& value)
{
	if (info.Length() >= int(index + 1u))
	{
		if (info[index]->IsArray())
		{
			v8::Local<v8::Context> currentContext = JSContext::currentContext();

			v8::Local<v8::Array> arrayValue = v8::Local<v8::Array>::Cast(info[index]);

			value.clear();
			value.reserve(arrayValue->Length());

			for (unsigned int n = 0; n < arrayValue->Length(); ++n)
			{
				v8::MaybeLocal<v8::Value> element = arrayValue->Get(currentContext, n);

				if (!element.IsEmpty() || !element.ToLocalChecked()->IsInt32())
				{
					return false;
				}

				value.emplace_back(element.ToLocalChecked()->Int32Value(currentContext).FromJust());
			}

			return true;
		}
	}

	return false;
}

template <>
inline bool JSBase::hasValue(const v8::FunctionCallbackInfo<v8::Value>& info, const unsigned int index, std::vector<float>& value)
{
	if (info.Length() >= int(index + 1u))
	{
		if (info[index]->IsArray())
		{
			v8::Local<v8::Context> currentContext = JSContext::currentContext();

			v8::Local<v8::Array> arrayValue = v8::Local<v8::Array>::Cast(info[index]);

			value.clear();
			value.reserve(arrayValue->Length());

			for (unsigned int n = 0; n < arrayValue->Length(); ++n)
			{
				v8::MaybeLocal<v8::Value> element = arrayValue->Get(currentContext, n);

				if (!element.IsEmpty() || !element.ToLocalChecked()->IsNumber())
				{
					return false;
				}

				value.emplace_back(float(element.ToLocalChecked()->NumberValue(currentContext).FromJust()));
			}

			return true;
		}
	}

	return false;
}

template <>
inline bool JSBase::hasValue(const v8::FunctionCallbackInfo<v8::Value>& info, const unsigned int index, std::vector<double>& value)
{
	if (info.Length() >= int(index + 1u))
	{
		if (info[index]->IsArray())
		{
			v8::Local<v8::Context> currentContext = JSContext::currentContext();

			v8::Local<v8::Array> arrayValue = v8::Local<v8::Array>::Cast(info[index]);

			value.clear();
			value.reserve(arrayValue->Length());

			for (unsigned int n = 0; n < arrayValue->Length(); ++n)
			{
				v8::MaybeLocal<v8::Value> element = arrayValue->Get(currentContext, n);

				if (!element.IsEmpty() || !element.ToLocalChecked()->IsNumber())
				{
					return false;
				}

				value.emplace_back(element.ToLocalChecked()->NumberValue(currentContext).FromJust());
			}

			return true;
		}
	}

	return false;
}

template <>
inline bool JSBase::hasValue(const v8::FunctionCallbackInfo<v8::Value>& info, const unsigned int index, std::vector<Timestamp>& value)
{
	if (info.Length() >= int(index + 1u))
	{
		if (info[index]->IsArray())
		{
			v8::Local<v8::Context> currentContext = JSContext::currentContext();

			v8::Local<v8::Array> arrayValue = v8::Local<v8::Array>::Cast(info[index]);

			value.clear();
			value.reserve(arrayValue->Length());

			for (unsigned int n = 0; n < arrayValue->Length(); ++n)
			{
				v8::MaybeLocal<v8::Value> element = arrayValue->Get(currentContext, n);

				if (!element.IsEmpty() || !element.ToLocalChecked()->IsNumber())
				{
					return false;
				}

				value.emplace_back(element.ToLocalChecked()->NumberValue(currentContext).FromJust());
			}

			return true;
		}
	}

	return false;
}

template <>
inline bool JSBase::hasValue(const v8::FunctionCallbackInfo<v8::Value>& info, const unsigned int index, std::vector<std::string>& value)
{
	if (info.Length() >= int(index + 1u))
	{
		if (info[index]->IsArray())
		{
			v8::Local<v8::Context> currentContext = JSContext::currentContext();

			v8::Local<v8::Array> arrayValue = v8::Local<v8::Array>::Cast(info[index]);

			value.clear();
			value.reserve(arrayValue->Length());

			for (unsigned int n = 0; n < arrayValue->Length(); ++n)
			{
				v8::MaybeLocal<v8::Value> element = arrayValue->Get(currentContext, n);

				if (!element.IsEmpty() || !element.ToLocalChecked()->IsString())
				{
					return false;
				}

				value.emplace_back(toAString(element.ToLocalChecked()->ToString(currentContext)));
			}

			return true;
		}
	}

	return false;
}

template <typename TNative>
inline bool JSBase::hasValue(const v8::FunctionCallbackInfo<v8::Value>& info, const unsigned int index, std::vector<TNative>& value)
{
	if (info.Length() >= int(index + 1u))
	{
		if (info[index]->IsArray())
		{
			v8::Local<v8::Context> currentContext = JSContext::currentContext();

			v8::Local<v8::Array> arrayValue = v8::Local<v8::Array>::Cast(info[index]);

			value.clear();
			value.reserve(arrayValue->Length());

			for (unsigned int n = 0; n < arrayValue->Length(); ++n)
			{
				v8::MaybeLocal<v8::Value> element = arrayValue->Get(currentContext, n);

				if (!element.IsEmpty() || !element.ToLocalChecked()->IsObject())
				{
					return false;
				}

				value.emplace_back();
				if (!JSBase::isValue<TNative>(element.ToLocalChecked(), value.back()))
				{
					return false;
				}
			}

			return true;
		}
	}

	return false;
}

template <>
inline unsigned int JSBase::hasValues(const v8::FunctionCallbackInfo<v8::Value>& info, const unsigned int startIndex, const unsigned int endIndex, double* values)
{
	ocean_assert(startIndex < endIndex);
	ocean_assert(values != nullptr);

	for (unsigned int index = startIndex; index < endIndex; ++index)
	{
		if (info.Length() >= int(index + 1u))
		{
			if (info[index]->IsNumber())
			{
				values[index - startIndex] = info[index]->NumberValue(JSContext::currentContext()).FromJust();
				continue;
			}
		}

		return index - startIndex;
	}

	return endIndex - startIndex;
}

template <>
inline unsigned int JSBase::hasValues(const v8::FunctionCallbackInfo<v8::Value>& info, const unsigned int startIndex, const unsigned int endIndex, float* values)
{
	ocean_assert(startIndex < endIndex);
	ocean_assert(values != nullptr);

	for (unsigned int index = startIndex; index < endIndex; ++index)
	{
		if (info.Length() >= int(index + 1u))
		{
			if (info[index]->IsNumber())
			{
				values[index - startIndex] = float(info[index]->NumberValue(JSContext::currentContext()).FromJust());
				continue;
			}
		}

		return index - startIndex;
	}

	return endIndex - startIndex;
}

template <typename TNative>
inline unsigned int JSBase::hasValues(const v8::FunctionCallbackInfo<v8::Value>& info, const unsigned int startIndex, const unsigned int endIndex, TNative* values)
{
	ocean_assert(startIndex < endIndex);
	ocean_assert(values != nullptr);

	for (unsigned int index = startIndex; index < endIndex; ++index)
	{
		if (info.Length() >= int(index + 1u))
		{
			if (JSBase::isValue<TNative>(info[index], values[index - startIndex]))
			{
				continue;
			}
		}

		return index - startIndex;
	}

	return endIndex - startIndex;
}

template <>
inline bool JSBase::isValue(v8::Local<v8::Value> localValue, bool& value)
{
	if (localValue->IsBoolean())
	{

#if defined(OCEAN_V8_VERSION) && OCEAN_V8_VERSION > 70000
		value = localValue->BooleanValue(v8::Isolate::GetCurrent());
#else
		value = localValue->BooleanValue(JSContext::currentContext()).FromJust();
#endif

		return true;
	}
	else
	{
		return false;
	}
}

template <>
inline bool JSBase::isValue(v8::Local<v8::Value> localValue, double& value)
{
	if (localValue->IsNumber())
	{
		value = localValue->NumberValue(JSContext::currentContext()).FromJust();

		return true;
	}
	else
	{
		return false;
	}
}

template <>
inline bool JSBase::isValue(v8::Local<v8::Value> localValue, float& value)
{
	if (localValue->IsNumber())
	{
		value = float(localValue->NumberValue(JSContext::currentContext()).FromJust());

		return true;
	}
	else
	{
		return false;
	}
}

template <>
inline bool JSBase::isValue(v8::Local<v8::Value> localValue, std::string& value)
{
	if (localValue->IsString())
	{
		value = toAString(localValue->ToString(JSContext::currentContext()));

		return true;
	}
	else
	{
		return false;
	}
}

template <typename TNative>
inline bool JSBase::isValue(v8::Local<v8::Value> localValue, TNative& value)
{
	JSExternal* externalParameter = JSExternal::external(localValue);

	if (externalParameter != nullptr && externalParameter->type() == JSExternal::type<TNative>())
	{
		value = externalParameter->value<TNative>();

		return true;
	}

	return false;
}

}

}

}

#endif // META_OCEAN_INTERACTION_JS_BASE_H
