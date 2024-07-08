/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/interaction/javascript/JSVector4.h"

#include "ocean/base/String.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

void JSVector4::createFunctionTemplate()
{
	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	v8::Local<v8::FunctionTemplate> functionTemplate(v8::FunctionTemplate::New(isolate, constructor<Vector4>));
	functionTemplate->SetClassName(newString(objectName(), isolate));

	v8::Local<v8::ObjectTemplate> objectTemplate(v8::Local<v8::ObjectTemplate>::New(isolate, functionTemplate->InstanceTemplate()));
	objectTemplate->SetInternalFieldCount(1);

	objectTemplate->SetAccessor(newString("x", isolate), propertyGetter<NativeType, AI_X>, propertySetter<NativeType, AI_X>);
	objectTemplate->SetAccessor(newString("y", isolate), propertyGetter<NativeType, AI_Y>, propertySetter<NativeType, AI_Y>);
	objectTemplate->SetAccessor(newString("z", isolate), propertyGetter<NativeType, AI_Z>, propertySetter<NativeType, AI_Z>);
	objectTemplate->SetAccessor(newString("w", isolate), propertyGetter<NativeType, AI_W>, propertySetter<NativeType, AI_W>);

	objectTemplate->Set(newString("add", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_ADD>));
	objectTemplate->Set(newString("angle", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_ANGLE>));
	objectTemplate->Set(newString("invert", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_INVERT>));
	objectTemplate->Set(newString("inverted", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_INVERTED>));
	objectTemplate->Set(newString("isEqual", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_IS_EQUAL>));
	objectTemplate->Set(newString("length", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_LENGTH>));
	objectTemplate->Set(newString("multiply", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_MULTIPLY>));
	objectTemplate->Set(newString("normalize", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_NORMALIZE>));
	objectTemplate->Set(newString("normalized", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_NORMALIZED>));
	objectTemplate->Set(newString("string", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_STRING>));
	objectTemplate->Set(newString("subtract", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SUBTRACT>));

	functionTemplate_.Reset(isolate, functionTemplate);
	objectTemplate_.Reset(isolate, objectTemplate);
}

template <>
void JSBase::constructor<Vector4>(Vector4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	thisValue = Vector4();

	if (hasValues<Scalar>(info, 0u, 4u, thisValue.data()))
	{
		return;
	}

	if (hasValue<Vector4>(info, 0u, thisValue))
	{
		return;
	}

	Vector3 vector3;
	if (hasValue<Vector3>(info, 0u, vector3))
	{
		Scalar w;

		if (hasValue<Scalar>(info, 1u, w))
		{
			thisValue = Vector4(vector3, w);
		}
		else
		{
			thisValue = Vector4(vector3);
		}
	}
}

template <>
void JSBase::propertyGetter<Vector4, JSVector4::AI_X>(Vector4& thisValue, v8::Local<v8::String>& /*property*/, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(double(thisValue.x()));
}

template <>
void JSBase::propertyGetter<Vector4, JSVector4::AI_Y>(Vector4& thisValue, v8::Local<v8::String>& /*property*/, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(double(thisValue.y()));
}

template <>
void JSBase::propertyGetter<Vector4, JSVector4::AI_Z>(Vector4& thisValue, v8::Local<v8::String>& /*property*/, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(double(thisValue.z()));
}

template <>
void JSBase::propertyGetter<Vector4, JSVector4::AI_W>(Vector4& thisValue, v8::Local<v8::String>& /*property*/, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(double(thisValue.w()));
}

template <>
void JSBase::propertySetter<Vector4, JSVector4::AI_X>(Vector4& thisValue, v8::Local<v8::String>& /*property*/, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& /*info*/)
{
	Scalar numberValue;
	if (isValue(value, numberValue))
	{
		thisValue.x() = numberValue;
	}
	else
	{
		Log::error() << "The x property of a Vector4 object accepts number values only.";
	}
}

template <>
void JSBase::propertySetter<Vector4, JSVector4::AI_Y>(Vector4& thisValue, v8::Local<v8::String>& /*property*/, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& /*info*/)
{
	Scalar numberValue;
	if (isValue(value, numberValue))
	{
		thisValue.y() = numberValue;
	}
	else
	{
		Log::error() << "The y property of a Vector4 object accepts number values only.";
	}
}

template <>
void JSBase::propertySetter<Vector4, JSVector4::AI_Z>(Vector4& thisValue, v8::Local<v8::String>& /*property*/, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& /*info*/)
{
	Scalar numberValue;
	if (isValue(value, numberValue))
	{
		thisValue.z() = numberValue;
	}
	else
	{
		Log::error() << "The z property of a Vector4 object accepts number values only.";
	}
}

template <>
void JSBase::propertySetter<Vector4, JSVector4::AI_W>(Vector4& thisValue, v8::Local<v8::String>& /*property*/, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& /*info*/)
{
	Scalar numberValue;
	if (isValue(value, numberValue))
	{
		thisValue.w() = numberValue;
	}
	else
	{
		Log::error() << "The w property of a Vector4 object accepts number values only.";
	}
}

template <>
void JSBase::function<Vector4, JSVector4::FI_ADD>(Vector4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	Vector4 valueVector;
	if (hasValue<Vector4>(info, 0u, valueVector))
	{
		info.GetReturnValue().Set(createObject<JSVector4>(thisValue + valueVector, JSContext::currentContext()));
		return;
	}

	Scalar valueScalar;
	if (hasValue<Scalar>(info, 0u, valueScalar))
	{
		info.GetReturnValue().Set(createObject<JSVector4>(thisValue + Vector4(valueScalar, valueScalar, valueScalar, valueScalar), JSContext::currentContext()));
		return;
	}

	Log::error() << "Vector4::add() accepts one parameter only, either a Vector4 object or a Number value.";
}

template <>
void JSBase::function<Vector4, JSVector4::FI_ANGLE>(Vector4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	Vector4 value;
	if (hasValue<Vector4>(info, 0u, value))
	{
		info.GetReturnValue().Set(double(thisValue.angle(value)));
		return;
	}

	Log::error() << "Vector4::angle() must have one Vector4 object as parameter.";
}

template <>
void JSBase::function<Vector4, JSVector4::FI_INVERT>(Vector4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& /*info*/)
{
	thisValue = -thisValue;
}

template <>
void JSBase::function<Vector4, JSVector4::FI_INVERTED>(Vector4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(createObject<JSVector4>(-thisValue, JSContext::currentContext()));
}

template <>
void JSBase::function<Vector4, JSVector4::FI_IS_EQUAL>(Vector4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	Vector4 value;
	if (hasValue<Vector4>(info, 0u, value))
	{
		info.GetReturnValue().Set(thisValue == value);
	}

	Log::error() << "'Vector4::isEqual() accepts one Vector4 object only.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Vector4, JSVector4::FI_LENGTH>(Vector4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	if (info.Length() != 0)
	{
		Log::warning() << "Vector4::length() does not take any parameters.";
	}

	info.GetReturnValue().Set(double(thisValue.length()));
}

template <>
void JSBase::function<Vector4, JSVector4::FI_MULTIPLY>(Vector4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	Vector4 valueVector;
	if (hasValue<Vector4>(info, 0u, valueVector))
	{
		info.GetReturnValue().Set(double(thisValue * valueVector));
		return;
	}

	Scalar valueScalar;
	if (hasValue<Scalar>(info, 0u, valueScalar))
	{
		info.GetReturnValue().Set(createObject<JSVector4>(thisValue * valueScalar, JSContext::currentContext()));
		return;
	}

	Log::error() << "Vector4::multiply() accepts one parameter only, either a Vector4 object or a Number value.";
}

template <>
void JSBase::function<Vector4, JSVector4::FI_NORMALIZE>(Vector4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& /*info*/)
{
	thisValue.normalize();
}

template <>
void JSBase::function<Vector4, JSVector4::FI_NORMALIZED>(Vector4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(createObject<JSVector4>(thisValue.normalized(), JSContext::currentContext()));
}

template <>
void JSBase::function<Vector4, JSVector4::FI_STRING>(Vector4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	if (info.Length() != 0)
	{
		Log::warning() << "Vector4::string() does not take any parameters.";
	}

	const std::string stringValue = String::toAString(thisValue.x(), 4u) + std::string(", ") + String::toAString(thisValue.y(), 4u) + std::string(", ") + String::toAString(thisValue.z(), 4u);

	info.GetReturnValue().Set(newString(stringValue, v8::Isolate::GetCurrent()));
}

template <>
void JSBase::function<Vector4, JSVector4::FI_SUBTRACT>(Vector4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	Vector4 valueVector;
	if (hasValue<Vector4>(info, 0u, valueVector))
	{
		info.GetReturnValue().Set(createObject<JSVector4>(thisValue - valueVector, JSContext::currentContext()));
		return;
	}

	Scalar valueScalar;
	if (hasValue<Scalar>(info, 0u, valueScalar))
	{
		info.GetReturnValue().Set(createObject<JSVector4>(thisValue - Vector4(valueScalar, valueScalar, valueScalar, valueScalar), JSContext::currentContext()));
		return;
	}

	Log::error() << "Vector4::subtract() accepts one parameter only, either a Vector4 object or a Number value.";
}

}

}

}
