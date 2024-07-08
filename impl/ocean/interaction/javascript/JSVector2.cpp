/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/interaction/javascript/JSVector2.h"

#include "ocean/base/String.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

void JSVector2::createFunctionTemplate()
{
	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	v8::Local<v8::FunctionTemplate> functionTemplate(v8::FunctionTemplate::New(isolate, constructor<Vector2>));
	functionTemplate->SetClassName(newString(objectName(), isolate));

	v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->InstanceTemplate();
	objectTemplate->SetInternalFieldCount(1);

	objectTemplate->SetAccessor(newString("x", isolate), propertyGetter<NativeType, AI_X>, propertySetter<NativeType, AI_X>);
	objectTemplate->SetAccessor(newString("y", isolate), propertyGetter<NativeType, AI_Y>, propertySetter<NativeType, AI_Y>);

	objectTemplate->Set(newString("add", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_ADD>));
	objectTemplate->Set(newString("angle", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_ANGLE>));
	objectTemplate->Set(newString("invert", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_INVERT>));
	objectTemplate->Set(newString("inverted", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_INVERTED>));
	objectTemplate->Set(newString("isEqual", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_IS_EQUAL>));
	objectTemplate->Set(newString("length", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_LENGTH>));
	objectTemplate->Set(newString("distance", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_DISTANCE>));
	objectTemplate->Set(newString("multiply", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_MULTIPLY>));
	objectTemplate->Set(newString("normalize", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_NORMALIZE>));
	objectTemplate->Set(newString("normalized", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_NORMALIZED>));
	objectTemplate->Set(newString("string", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_STRING>));
	objectTemplate->Set(newString("subtract", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SUBTRACT>));

	functionTemplate_.Reset(isolate, functionTemplate);
	objectTemplate_.Reset(isolate, objectTemplate);
}

template <>
void JSBase::constructor<Vector2>(Vector2& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	thisValue = Vector2();

	if (!hasValues<Scalar>(info, 0u, 2u, thisValue.data()))
	{
		hasValue<Vector2>(info, 0u, thisValue);
	}
}

template <>
void JSBase::propertyGetter<Vector2, JSVector2::AI_X>(Vector2& thisValue, v8::Local<v8::String>& /*property*/, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(double(thisValue.x()));
}

template <>
void JSBase::propertyGetter<Vector2, JSVector2::AI_Y>(Vector2& thisValue, v8::Local<v8::String>& /*property*/, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(double(thisValue.y()));
}

template <>
void JSBase::propertySetter<Vector2, JSVector2::AI_X>(Vector2& thisValue, v8::Local<v8::String>& /*property*/, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& /*info*/)
{
	Scalar numberValue;
	if (isValue(value, numberValue))
	{
		thisValue.x() = numberValue;
	}
	else
	{
		Log::error() << "The x property of a Vector2 object accepts number values only.";
	}
}

template <>
void JSBase::propertySetter<Vector2, JSVector2::AI_Y>(Vector2& thisValue, v8::Local<v8::String>& /*property*/, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& /*info*/)
{
	Scalar numberValue;
	if (isValue(value, numberValue))
	{
		thisValue.y() = numberValue;
	}
	else
	{
		Log::error() << "The y property of a Vector2 object accepts number values only.";
	}
}

template <>
void JSBase::function<Vector2, JSVector2::FI_ADD>(Vector2& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	Vector2 valueVector;
	if (hasValue<Vector2>(info, 0u, valueVector))
	{
		info.GetReturnValue().Set(createObject<JSVector2>(thisValue + valueVector, JSContext::currentContext()));
		return;
	}

	Scalar valueScalar;
	if (hasValue<Scalar>(info, 0u, valueScalar))
	{
		info.GetReturnValue().Set(createObject<JSVector2>(thisValue + Vector2(valueScalar, valueScalar), JSContext::currentContext()));
		return;
	}

	Log::error() << "Vector2::add() accepts one parameter only, either a Vector2 object or a Number value.";
}

template <>
void JSBase::function<Vector2, JSVector2::FI_ANGLE>(Vector2& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	Vector2 value;
	if (hasValue<Vector2>(info, 0u, value))
	{
		info.GetReturnValue().Set(double(thisValue.angle(value)));
		return;
	}

	Log::error() << "Vector2::angle() must have one Vector2 object as parameter.";
}

template <>
void JSBase::function<Vector2, JSVector2::FI_INVERT>(Vector2& thisValue, const v8::FunctionCallbackInfo<v8::Value>& /*info*/)
{
	thisValue = -thisValue;
}

template <>
void JSBase::function<Vector2, JSVector2::FI_INVERTED>(Vector2& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(createObject<JSVector2>(-thisValue, JSContext::currentContext()));
}

template <>
void JSBase::function<Vector2, JSVector2::FI_IS_EQUAL>(Vector2& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	Vector2 value;
	if (hasValue<Vector2>(info, 0u, value))
	{
		info.GetReturnValue().Set(thisValue == value);
	}

	Log::error() << "'Vector2::isEqual() accepts one Vector2 object only.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Vector2, JSVector2::FI_LENGTH>(Vector2& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	if (info.Length() != 0)
	{
		Log::warning() << "Vector2::length() does not take any parameters.";
	}

	info.GetReturnValue().Set(double(thisValue.length()));
}

template <>
void JSBase::function<Vector2, JSVector2::FI_DISTANCE>(Vector2& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	Vector2 value;
	if (hasValue<Vector2>(info, 0u, value))
	{
		info.GetReturnValue().Set(double(thisValue.distance(value)));
	}

	Log::error() << "'Vector2::distance() accepts one Vector2 object only.";
	info.GetReturnValue().Set(0.0);
}

template <>
void JSBase::function<Vector2, JSVector2::FI_MULTIPLY>(Vector2& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	Vector2 valueVector;
	if (hasValue<Vector2>(info, 0u, valueVector))
	{
		info.GetReturnValue().Set(double(thisValue * valueVector));
		return;
	}

	Scalar valueScalar;
	if (hasValue<Scalar>(info, 0u, valueScalar))
	{
		info.GetReturnValue().Set(createObject<JSVector2>(thisValue * valueScalar, JSContext::currentContext()));
		return;
	}

	Log::error() << "Vector2::multiply() accepts one parameter only, either a Vector2 object or a Number value.";
}

template <>
void JSBase::function<Vector2, JSVector2::FI_NORMALIZE>(Vector2& thisValue, const v8::FunctionCallbackInfo<v8::Value>& /*info*/)
{
	thisValue.normalize();
}

template <>
void JSBase::function<Vector2, JSVector2::FI_NORMALIZED>(Vector2& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(createObject<JSVector2>(thisValue.normalizedOrZero(), JSContext::currentContext()));
}

template <>
void JSBase::function<Vector2, JSVector2::FI_STRING>(Vector2& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	if (info.Length() != 0)
	{
		Log::warning() << "Vector2::string() does not take any parameters.";
	}

	const std::string stringValue = String::toAString(thisValue.x(), 4u) + std::string(", ") + String::toAString(thisValue.y(), 4u);

	info.GetReturnValue().Set(newString(stringValue, v8::Isolate::GetCurrent()));
}

template <>
void JSBase::function<Vector2, JSVector2::FI_SUBTRACT>(Vector2& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	Vector2 valueVector;
	if (hasValue<Vector2>(info, 0u, valueVector))
	{
		info.GetReturnValue().Set(createObject<JSVector2>(thisValue - valueVector, JSContext::currentContext()));
		return;
	}

	Scalar valueScalar;
	if (hasValue<Scalar>(info, 0u, valueScalar))
	{
		info.GetReturnValue().Set(createObject<JSVector2>(thisValue - Vector2(valueScalar, valueScalar), JSContext::currentContext()));
		return;
	}

	Log::error() << "Vector2::subtract() accepts one parameter only, either a Vector2 object or a Number value.";
}

}

}

}
