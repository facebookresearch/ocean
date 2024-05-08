/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/interaction/javascript/JSColor.h"
#include "ocean/interaction/javascript/JSContext.h"

#include "ocean/base/String.h"

#include "ocean/math/HSVAColor.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

void JSColor::createFunctionTemplate()
{
	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	v8::Local<v8::FunctionTemplate> functionTemplate(v8::FunctionTemplate::New(isolate, JSColor::constructor<RGBAColor>));
	functionTemplate->SetClassName(newString(objectName(), isolate));

	v8::Local<v8::ObjectTemplate> objectTemplate(v8::Local<v8::ObjectTemplate>::New(isolate, functionTemplate->InstanceTemplate()));
	objectTemplate->SetInternalFieldCount(1);

	objectTemplate->SetAccessor(newString("red", isolate), propertyGetter<NativeType, AI_RED>, propertySetter<NativeType, AI_RED>);
	objectTemplate->SetAccessor(newString("green", isolate), propertyGetter<NativeType, AI_GREEN>, propertySetter<NativeType, AI_GREEN>);
	objectTemplate->SetAccessor(newString("blue", isolate), propertyGetter<NativeType, AI_BLUE>, propertySetter<NativeType, AI_BLUE>);
	objectTemplate->SetAccessor(newString("alpha", isolate), propertyGetter<NativeType, AI_ALPHA>, propertySetter<NativeType, AI_ALPHA>);

	objectTemplate->Set(newString("clamp", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_CLAMP>));
	objectTemplate->Set(newString("Clamped", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_CLAMPED>));
	objectTemplate->Set(newString("interpolate", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_INTERPOLATE>));
	objectTemplate->Set(newString("invert", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_INVERT>));
	objectTemplate->Set(newString("inverted", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_INVERTED>));
	objectTemplate->Set(newString("isEqual", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_IS_EQUAL>));
	objectTemplate->Set(newString("isNormalized", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_IS_NORMALIZED>));
	objectTemplate->Set(newString("multiply", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_MULTIPLY>));
	objectTemplate->Set(newString("normalize", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_NORMALIZE>));
	objectTemplate->Set(newString("normalized", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_NORMALIZED>));
	objectTemplate->Set(newString("string", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_STRING>));

	functionTemplate_.Reset(isolate, functionTemplate);
	objectTemplate_.Reset(isolate, objectTemplate);
}

template <>
void JSBase::constructor<RGBAColor>(RGBAColor& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	if (info.Length() == 0)
	{
		return;
	}

	if (hasValue(info, 0u, thisValue))
	{
		return;
	}

	if (info[0]->IsObject())
	{
		Vector3 vector3;
		Vector4 vector4;

		if (hasValue(info, 0u, vector3))
		{
			float alpha = 0.0f;
			hasValue(info, 1u, alpha);

			thisValue = RGBAColor(float(vector3.x()), float(vector3.y()), float(vector3.z()), alpha);
		}
		else if (hasValue(info, 0u, vector4))
		{
			thisValue = RGBAColor(float(vector4.x()), float(vector4.y()), float(vector4.z()), float(vector4.w()));
		}
	}
	else if (info[0]->IsArray())
	{
		std::vector<float> values;
		hasValue(info, 0u, values);

		if (values.size() == 3)
		{
			thisValue = RGBAColor(values[0], values[1], values[2]);
		}
		else if (values.size() == 4)
		{
			thisValue = RGBAColor(values.data());
		}
	}
	else
	{
		float values[4] = {0.0f, 0.0f, 0.0f, 1.0f};
		const unsigned int numberValues = hasValues<float>(info, 0u, 4u, values);

		if (numberValues >= 3u)
		{
			thisValue = RGBAColor(values);
		}
	}

	if (!thisValue.isValid())
	{
		Log::error() << "Invalid color initialization, values must not be negative, alpha must be in range [0, 1].";
	}
}

template <>
void JSBase::propertyGetter<RGBAColor, JSColor::AI_RED>(RGBAColor& thisValue, v8::Local<v8::String>& /*property*/, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(double(thisValue.red()));
}

template <>
void JSBase::propertyGetter<RGBAColor, JSColor::AI_GREEN>(RGBAColor& thisValue, v8::Local<v8::String>& /*property*/, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(double(thisValue.green()));
}

template <>
void JSBase::propertyGetter<RGBAColor, JSColor::AI_BLUE>(RGBAColor& thisValue, v8::Local<v8::String>& /*property*/, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(double(thisValue.blue()));
}

template <>
void JSBase::propertyGetter<RGBAColor, JSColor::AI_ALPHA>(RGBAColor& thisValue, v8::Local<v8::String>& /*property*/, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(double(thisValue.alpha()));
}

template <>
void JSBase::propertySetter<RGBAColor, JSColor::AI_RED>(RGBAColor& thisValue, v8::Local<v8::String>& /*property*/, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& /*info*/)
{
	float numberValue;
	if (isValue(value, numberValue) && numberValue >= 0.0f)
	{
		thisValue.setRed(numberValue);
	}
	else
	{
		Log::error() << "The red property of a color object accepts positive number values only.";
	}
}

template <>
void JSBase::propertySetter<RGBAColor, JSColor::AI_GREEN>(RGBAColor& thisValue, v8::Local<v8::String>& /*property*/, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& /*info*/)
{
	float numberValue;
	if (isValue(value, numberValue) && numberValue >= 0.0f)
	{
		thisValue.setGreen(numberValue);
	}
	else
	{
		Log::error() << "The green property of a color object accepts positive number values only.";
	}
}

template <>
void JSBase::propertySetter<RGBAColor, JSColor::AI_BLUE>(RGBAColor& thisValue, v8::Local<v8::String>& /*property*/, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& /*info*/)
{
	float numberValue;
	if (isValue(value, numberValue) && numberValue >= 0.0f)
	{
		thisValue.setBlue(numberValue);
	}
	else
	{
		Log::error() << "The blue property of a color object accepts positive number values only.";
	}
}

template <>
void JSBase::propertySetter<RGBAColor, JSColor::AI_ALPHA>(RGBAColor& thisValue, v8::Local<v8::String>& /*property*/, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& /*info*/)
{
	float numberValue;
	if (isValue(value, numberValue) && numberValue >= 0.0f)
	{
		thisValue.setAlpha(numberValue);
	}
	else
	{
		Log::error() << "The alpha property of a color object accepts positive number values only.";
	}
}

template <>
void JSBase::function<RGBAColor, JSColor::FI_CLAMP>(RGBAColor& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	if (info.Length() != 0)
	{
		Log::warning() << "Color::clamp() does not take any parameters.";
	}

	thisValue.clamp();
}

template <>
void JSBase::function<RGBAColor, JSColor::FI_CLAMPED>(RGBAColor& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	if (info.Length() != 0)
	{
		Log::warning() << "Color::clamped() does not take any parameters.";
	}

	info.GetReturnValue().Set(createObject<JSColor>(thisValue.clamped(), JSContext::currentContext()));
}

template <>
void JSBase::function<RGBAColor, JSColor::FI_INTERPOLATE>(RGBAColor& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	RGBAColor result(thisValue);

	RGBAColor value;
	float factor;

	if (hasValue<RGBAColor>(info, 0u, value) && hasValue<float>(info, 1u, factor))
	{
		if (factor >= 0.0f && factor <= 1.0f)
		{
			const HSVAColor firstColor(thisValue);
			const HSVAColor secondColor(value);

			result = RGBAColor(firstColor.interpolate(secondColor, factor));
		}
		else
		{
			Log::error() << "Invalid interpolation parameter: must be inside the range [0.0, 1.0]";
		}
	}
	else
	{
		Log::error() << "Color::interpolate() needs a second color and a number parameter.";
	}

	info.GetReturnValue().Set(createObject<JSColor>(result, JSContext::currentContext()));
}

template <>
void JSBase::function<RGBAColor, JSColor::FI_INVERT>(RGBAColor& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	if (info.Length() != 0)
	{
		Log::warning() << "Color::invert() does not take any parameters.";
	}

	if (thisValue.isNormalized())
	{
		thisValue = -thisValue;
	}
	else
	{
		Log::error() << "Color inverting failed: the color object is not normalized.";
	}
}

template <>
void JSBase::function<RGBAColor, JSColor::FI_INVERTED>(RGBAColor& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	if (info.Length() != 0)
	{
		Log::warning() << "Color::invert() does not take any parameters.";
	}

	if (thisValue.isNormalized())
	{
		info.GetReturnValue().Set(createObject<JSColor>(-thisValue, JSContext::currentContext()));
	}

	Log::error() << "Color inverting failed: the color object is not normalized.";
	return info.GetReturnValue().Set(createObject<JSColor>(thisValue, JSContext::currentContext()));
}

template <>
void JSBase::function<RGBAColor, JSColor::FI_IS_EQUAL>(RGBAColor& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	RGBAColor value;
	if (hasValue<RGBAColor>(info, 0u, value))
	{
		info.GetReturnValue().Set(thisValue == value);
	}

	Log::error() << "'Color::isEqual() accepts one Color object only.";
	return info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<RGBAColor, JSColor::FI_IS_NORMALIZED>(RGBAColor& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	return info.GetReturnValue().Set(thisValue.isNormalized());
}

template <>
void JSBase::function<RGBAColor, JSColor::FI_MULTIPLY>(RGBAColor& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	float factor;
	if (hasValue<float>(info, 0u, factor) && factor >= 0.0f)
	{
		info.GetReturnValue().Set(createObject<JSColor>(thisValue.damped(factor, true), JSContext::currentContext()));
	}
	else
	{
		Log::error() << "Color::multiply() accepts one positive scalar value parameter only.";
		info.GetReturnValue().Set(createObject<JSColor>(thisValue, JSContext::currentContext()));
	}
}

template <>
void JSBase::function<RGBAColor, JSColor::FI_NORMALIZE>(RGBAColor& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	if (info.Length() != 0)
	{
		Log::warning() << "Color::normalize() does not take any parameters.";
	}

	thisValue.normalize();
}

template <>
void JSBase::function<RGBAColor, JSColor::FI_NORMALIZED>(RGBAColor& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	if (info.Length() != 0)
	{
		Log::warning() << "Color::normalized() does not take any parameters.";
	}

	info.GetReturnValue().Set(createObject<JSColor>(thisValue.normalized(), JSContext::currentContext()));
}

template <>
void JSBase::function<RGBAColor, JSColor::FI_STRING>(RGBAColor& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	if (info.Length() != 0)
	{
		Log::warning() << "Color::string() does not take any parameters.";
	}

	const std::string stringValue = String::toAString(thisValue.red(), 2u) + std::string(", ") + String::toAString(thisValue.green(), 2u) + std::string(", ") + String::toAString(thisValue.blue(), 2u) + std::string(", ") + String::toAString(thisValue.alpha(), 2u);

	info.GetReturnValue().Set(newString(stringValue, v8::Isolate::GetCurrent()));
}

}

}

}
