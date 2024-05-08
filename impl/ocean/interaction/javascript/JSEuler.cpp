/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/interaction/javascript/JSEuler.h"

#include "ocean/base/String.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

void JSEuler::createFunctionTemplate()
{
	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	v8::Local<v8::FunctionTemplate> functionTemplate(v8::FunctionTemplate::New(isolate, constructor<Euler>));
	functionTemplate->SetClassName(newString(objectName(), isolate));

	v8::Local<v8::ObjectTemplate> objectTemplate(v8::Local<v8::ObjectTemplate>::New(isolate, functionTemplate->InstanceTemplate()));
	objectTemplate->SetInternalFieldCount(1);

	objectTemplate->SetAccessor(newString("yaw", isolate), propertyGetter<NativeType, AI_YAW>, propertySetter<NativeType, AI_YAW>);
	objectTemplate->SetAccessor(newString("pitch", isolate), propertyGetter<NativeType, AI_PITCH>, propertySetter<NativeType, AI_PITCH>);
	objectTemplate->SetAccessor(newString("roll", isolate), propertyGetter<NativeType, AI_ROLL>, propertySetter<NativeType, AI_ROLL>);

	objectTemplate->Set(newString("isEqual", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_IS_EQUAL>));
	objectTemplate->Set(newString("string", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_STRING>));

	functionTemplate_.Reset(isolate, functionTemplate);
	objectTemplate_.Reset(isolate, objectTemplate);
}

template <>
void JSBase::constructor<Euler>(Euler& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
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
		if (hasValue(info, 0u, vector3))
		{
			thisValue = Euler(vector3.x(), vector3.y(), vector3.z());
			return;
		}

		Quaternion quaternion;
		if (hasValue(info, 0u, quaternion))
		{
			thisValue = Euler(quaternion);
			return;
		}

		Rotation rotation;
		if (hasValue(info, 0u, rotation))
		{
			thisValue = Euler(rotation);
			return;
		}

		HomogenousMatrix4 homogenousMatrix;
		if (hasValue(info, 0u, homogenousMatrix))
		{
			thisValue = Euler(homogenousMatrix);
			return;
		}

		SquareMatrix3 squareMatrix3;
		if (hasValue(info, 0u, squareMatrix3))
		{
			thisValue = Euler(squareMatrix3);
			return;
		}
	}
	else if (info[0]->IsArray())
	{
		std::vector<Scalar> values;
		if (hasValue(info, 0u, values) && values.size() == 3u)
		{
			thisValue = Euler(values.data());
			return;
		}
	}
	else
	{
		Scalar values[3];
		if (hasValues<Scalar>(info, 0u, 3u, values) == 3)
		{
			Scalar yaw = values[0];
			Scalar pitch = values[1];
			Scalar roll = values[2];

			Euler::adjustAngles(yaw, pitch, roll);

			thisValue = Euler(yaw, pitch, roll);
			ocean_assert(thisValue.isValid());

			return;
		}
	}
}

template <>
void JSBase::propertyGetter<Euler, JSEuler::AI_YAW>(Euler& thisValue, v8::Local<v8::String>& /*property*/, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(double(thisValue.yaw()));
}

template <>
void JSBase::propertyGetter<Euler, JSEuler::AI_PITCH>(Euler& thisValue, v8::Local<v8::String>& /*property*/, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(double(thisValue.pitch()));
}

template <>
void JSBase::propertyGetter<Euler, JSEuler::AI_ROLL>(Euler& thisValue, v8::Local<v8::String>& /*property*/, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(double(thisValue.roll()));
}

template <>
void JSBase::propertySetter<Euler, JSEuler::AI_YAW>(Euler& thisValue, v8::Local<v8::String>& /*property*/, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& /*info*/)
{
	Scalar numberValue;
	if (isValue(value, numberValue))
	{
		thisValue.yaw() = numberValue;
	}
	else
	{
		Log::error() << "The yaw property of a Euler object accepts number values only.";
	}
}

template <>
void JSBase::propertySetter<Euler, JSEuler::AI_PITCH>(Euler& thisValue, v8::Local<v8::String>& /*property*/, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& /*info*/)
{
	Scalar numberValue;
	if (isValue(value, numberValue))
	{
		thisValue.pitch() = numberValue;
	}
	else
	{
		Log::error() << "The pitch property of a Euler object accepts number values only.";
	}
}

template <>
void JSBase::propertySetter<Euler, JSEuler::AI_ROLL>(Euler& thisValue, v8::Local<v8::String>& /*property*/, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& /*info*/)
{
	Scalar numberValue;
	if (isValue(value, numberValue))
	{
		thisValue.roll() = numberValue;
	}
	else
	{
		Log::error() << "The roll property of a Euler object accepts number values only.";
	}
}

template <>
void JSBase::function<Euler, JSEuler::FI_IS_EQUAL>(Euler& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	Euler value;
	if (hasValue<Euler>(info, 0u, value))
	{
		info.GetReturnValue().Set(thisValue == value);
		return;
	}

	Log::error() << "Euler::isEqual() accepts one Euler object as parameter only.";
}

template <>
void JSBase::function<Euler, JSEuler::FI_STRING>(Euler& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	if (info.Length() != 0)
	{
		Log::warning() << "Euler::string() does not take any parameters.";
	}

	const std::string stringValue = String::toAString(thisValue.yaw(), 4u) + std::string(", ") + String::toAString(thisValue.pitch(), 4u) + std::string(", ") + String::toAString(thisValue.roll(), 4u);

	info.GetReturnValue().Set(newString(stringValue, v8::Isolate::GetCurrent()));
}

}

}

}
