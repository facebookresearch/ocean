/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/interaction/javascript/JSLine3.h"
#include "ocean/interaction/javascript/JSVector3.h"

#include "ocean/base/String.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

void JSLine3::createFunctionTemplate()
{
	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	v8::Local<v8::FunctionTemplate> functionTemplate(v8::FunctionTemplate::New(isolate, constructor<Line3>));
	functionTemplate->SetClassName(newString(objectName(), isolate));

	v8::Local<v8::ObjectTemplate> objectTemplate(v8::Local<v8::ObjectTemplate>::New(isolate, functionTemplate->InstanceTemplate()));
	objectTemplate->SetInternalFieldCount(1);

	objectTemplate->SetAccessor(newString("point", isolate), propertyGetter<NativeType, AI_POINT>, propertySetter<NativeType, AI_POINT>);
	objectTemplate->SetAccessor(newString("direction", isolate), propertyGetter<NativeType, AI_DIRECTION>, propertySetter<NativeType, AI_DIRECTION>);

	objectTemplate->Set(newString("isEqual", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_IS_EQUAL>));
	objectTemplate->Set(newString("distance", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_DISTANCE>));
	objectTemplate->Set(newString("nearestPoint", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_NEAREST_POINT>));
	objectTemplate->Set(newString("string", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_STRING>));

	functionTemplate_.Reset(isolate, functionTemplate);
	objectTemplate_.Reset(isolate, objectTemplate);
}

template <>
void JSBase::constructor<Line3>(Line3& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	if (info.Length() == 0)
	{
		return;
	}

	if (hasValue(info, 0u, thisValue))
	{
		return;
	}

	Vector3 point;
	Vector3 direction;

	if (hasValue(info, 0u, point) && hasValue(info, 1u, direction))
	{
		thisValue = Line3(point, direction);
	}
}

template <>
void JSBase::propertyGetter<Line3, JSLine3::AI_POINT>(Line3& thisValue, v8::Local<v8::String>& /*property*/, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(createObject<JSVector3>(thisValue.point(), JSContext::currentContext()));
}

template <>
void JSBase::propertyGetter<Line3, JSLine3::AI_DIRECTION>(Line3& thisValue, v8::Local<v8::String>& /*property*/, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(createObject<JSVector3>(thisValue.direction(), JSContext::currentContext()));
}

template <>
void JSBase::propertySetter<Line3, JSLine3::AI_POINT>(Line3& thisValue, v8::Local<v8::String>& /*property*/, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& /*info*/)
{
	Vector3 vectorValue;
	if (isValue(value, vectorValue))
	{
		thisValue.setPoint(vectorValue);
	}
	else
	{
		Log::error() << "The point property of a Line3 object accepts a Vector3 object only.";
	}
}

template <>
void JSBase::propertySetter<Line3, JSLine3::AI_DIRECTION>(Line3& thisValue, v8::Local<v8::String>& /*property*/, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& /*info*/)
{
	Vector3 vectorValue;
	if (isValue(value, vectorValue))
	{
		if (vectorValue.isUnit())
		{
			thisValue.setDirection(vectorValue);
		}
		else
		{
			Log::error() << "Line3::direction property accepts unit vectors only.";
		}
	}
	else
	{
		Log::error() << "The direction property of a Line3 object accepts a Vector3 object only.";
	}
}

template <>
void JSBase::function<Line3, JSLine3::FI_IS_EQUAL>(Line3& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	Line3 value;
	if (hasValue<Line3>(info, 0u, value))
	{
		info.GetReturnValue().Set(thisValue == value);
		return;
	}

	Log::error() << "Line3::isEqual() accepts one Line3 object as parameter only.";
}

template <>
void JSBase::function<Line3, JSLine3::FI_DISTANCE>(Line3& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	Line3 lineValue;
	if (hasValue<Line3>(info, 0u, lineValue))
	{
		info.GetReturnValue().Set(double(thisValue.distance(lineValue)));
		return;
	}

	Vector3 vectorValue;
	if (hasValue<Vector3>(info, 0u, vectorValue))
	{
		info.GetReturnValue().Set(double(thisValue.distance(vectorValue)));
		return;
	}

	Log::error() << "Line3::distance() accepts one Line3 object or a Vector3 object sas parameter only.";
}

template <>
void JSBase::function<Line3, JSLine3::FI_NEAREST_POINT>(Line3& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	Vector3 value;
	if (hasValue<Vector3>(info, 0u, value))
	{
		info.GetReturnValue().Set(JSVector3::create(thisValue.nearestPoint(value), JSContext::currentContext()));
		return;
	}

	Log::error() << "Line3::nearestPoint() accepts one Vector3 object as parameter only.";
}

template <>
void JSBase::function<Line3, JSLine3::FI_STRING>(Line3& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	if (info.Length() != 0)
	{
		Log::warning() << "Line3::string() does not take any parameters.";
	}

	const std::string stringValue = String::toAString(thisValue.point().x(), 4u) + std::string(", ") + String::toAString(thisValue.point().y(), 4u) + std::string(", ") + String::toAString(thisValue.point().z(), 4u)
										+ std::string("; ") + String::toAString(thisValue.direction().x(), 4u) + std::string(", ") + String::toAString(thisValue.direction().y(), 4u) + std::string(", ") + String::toAString(thisValue.direction().z(), 4u);

	info.GetReturnValue().Set(newString(stringValue, v8::Isolate::GetCurrent()));
}

}

}

}
