/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/interaction/javascript/JSQuaternion.h"
#include "ocean/interaction/javascript/JSVector3.h"

#include "ocean/base/String.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

void JSQuaternion::createFunctionTemplate()
{
	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	v8::Local<v8::FunctionTemplate> functionTemplate(v8::FunctionTemplate::New(isolate, constructor<Quaternion>));
	functionTemplate->SetClassName(newString(objectName(), isolate));

	v8::Local<v8::ObjectTemplate> objectTemplate(v8::Local<v8::ObjectTemplate>::New(isolate, functionTemplate->InstanceTemplate()));
	objectTemplate->SetInternalFieldCount(1);

	objectTemplate->SetAccessor(newString("x", isolate), propertyGetter<NativeType, AI_X>, propertySetter<NativeType, AI_X>);
	objectTemplate->SetAccessor(newString("y", isolate), propertyGetter<NativeType, AI_Y>, propertySetter<NativeType, AI_X>);
	objectTemplate->SetAccessor(newString("z", isolate), propertyGetter<NativeType, AI_Z>, propertySetter<NativeType, AI_X>);
	objectTemplate->SetAccessor(newString("w", isolate), propertyGetter<NativeType, AI_W>, propertySetter<NativeType, AI_X>);

	objectTemplate->Set(newString("invert", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_INVERT>));
	objectTemplate->Set(newString("inverted", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_INVERTED>));
	objectTemplate->Set(newString("isEqual", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_IS_EQUAL>));
	objectTemplate->Set(newString("multiply", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_MULTIPLY>));
	objectTemplate->Set(newString("slerp", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SLERP>));
	objectTemplate->Set(newString("string", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_STRING>));

	functionTemplate_.Reset(isolate, functionTemplate);
	objectTemplate_.Reset(isolate, objectTemplate);
}

template <>
void JSBase::constructor<Quaternion>(Quaternion& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
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
		Scalar scalar;

		if (hasValue(info, 0u, vector3) && hasValue(info, 1u, scalar))
		{
			if (vector3.isUnit())
			{
				Log::warning() << "The rotation axis must be a unit vector";
			}

			thisValue = Quaternion(vector3, scalar);
			return;
		}

		Rotation rotation;
		if (hasValue(info, 0u, rotation))
		{
			thisValue = Quaternion(rotation);
			return;
		}

		Euler euler;
		if (hasValue(info, 0u, euler))
		{
			thisValue = Quaternion(euler);
			return;
		}

		HomogenousMatrix4 homogenousMatrix;
		if (hasValue(info, 0u, homogenousMatrix))
		{
			thisValue = homogenousMatrix.rotation();
			return;
		}
	}
	else if (info[0]->IsArray())
	{
		std::vector<Scalar> values;
		if (hasValue(info, 0u, values) && values.size() == 4u)
		{
			thisValue = Quaternion(values.data());

			if (!thisValue.isValid())
			{
				Log::warning() << "The rotation axis must be a unit vector";
			}

			return;
		}
	}
	else
	{
		Scalar values[4];
		if (hasValues<Scalar>(info, 0u, 4u, values) == 4)
		{
			thisValue = Quaternion(values[0], values[1], values[2], values[3]);

			if (!thisValue.isValid())
			{
				Log::warning() << "The rotation axis must be a unit vector";
			}

			return;
		}
	}
}

template <>
void JSBase::propertyGetter<Quaternion, JSQuaternion::AI_X>(Quaternion& thisValue, v8::Local<v8::String>& /*property*/, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(double(thisValue.x()));
}

template <>
void JSBase::propertyGetter<Quaternion, JSQuaternion::AI_Y>(Quaternion& thisValue, v8::Local<v8::String>& /*property*/, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(double(thisValue.y()));
}

template <>
void JSBase::propertyGetter<Quaternion, JSQuaternion::AI_Z>(Quaternion& thisValue, v8::Local<v8::String>& /*property*/, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(double(thisValue.z()));
}

template <>
void JSBase::propertyGetter<Quaternion, JSQuaternion::AI_W>(Quaternion& thisValue, v8::Local<v8::String>& /*property*/, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(double(thisValue.w()));
}

template <>
void JSBase::propertySetter<Quaternion, JSQuaternion::AI_X>(Quaternion& thisValue, v8::Local<v8::String>& /*property*/, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& /*info*/)
{
	Scalar numberValue;
	if (isValue(value, numberValue))
	{
		thisValue.x() = numberValue;
	}
	else
	{
		Log::error() << "The x property of a Quaternion object accepts number values only.";
	}
}

template <>
void JSBase::propertySetter<Quaternion, JSQuaternion::AI_Y>(Quaternion& thisValue, v8::Local<v8::String>& /*property*/, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& /*info*/)
{
	Scalar numberValue;
	if (isValue(value, numberValue))
	{
		thisValue.y() = numberValue;
	}
	else
	{
		Log::error() << "The y property of a Quaternion object accepts number values only.";
	}
}

template <>
void JSBase::propertySetter<Quaternion, JSQuaternion::AI_Z>(Quaternion& thisValue, v8::Local<v8::String>& /*property*/, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& /*info*/)
{
	Scalar numberValue;
	if (isValue(value, numberValue))
	{
		thisValue.z() = numberValue;
	}
	else
	{
		Log::error() << "The z property of a Quaternion object accepts number values only.";
	}
}

template <>
void JSBase::propertySetter<Quaternion, JSQuaternion::AI_W>(Quaternion& thisValue, v8::Local<v8::String>& /*property*/, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& /*info*/)
{
	Scalar numberValue;
	if (isValue(value, numberValue))
	{
		thisValue.w() = numberValue;
	}
	else
	{
		Log::error() << "The w property of a Quaternion object accepts number values only.";
	}
}

template <>
void JSBase::function<Quaternion, JSQuaternion::FI_INVERT>(Quaternion& thisValue, const v8::FunctionCallbackInfo<v8::Value>& /*info*/)
{
	thisValue.invert();
}

template <>
void JSBase::function<Quaternion, JSQuaternion::FI_INVERTED>(Quaternion& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(createObject<JSQuaternion>(thisValue.inverted(), JSContext::currentContext()));
}

template <>
void JSBase::function<Quaternion, JSQuaternion::FI_IS_EQUAL>(Quaternion& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	Quaternion value;
	if (hasValue<Quaternion>(info, 0u, value))
	{
		info.GetReturnValue().Set(thisValue == value);
		return;
	}

	Log::error() << "Quaternion::isEqual() accepts one Quaternion object as parameter only.";
}

template <>
void JSBase::function<Quaternion, JSQuaternion::FI_MULTIPLY>(Quaternion& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	Quaternion quaternionValue;
	if (hasValue<Quaternion>(info, 0u, quaternionValue))
	{
		info.GetReturnValue().Set(createObject<JSQuaternion>(thisValue * quaternionValue, JSContext::currentContext()));
		return;
	}

	Rotation rotationValue;
	if (hasValue<Rotation>(info, 0u, rotationValue))
	{
		info.GetReturnValue().Set(createObject<JSQuaternion>(thisValue * rotationValue, JSContext::currentContext()));
		return;
	}

	Vector3 vectorValue3;
	if (hasValue<Vector3>(info, 0u, vectorValue3))
	{
		info.GetReturnValue().Set(createObject<JSVector3>(thisValue * vectorValue3, JSContext::currentContext()));
		return;
	}

	Log::error() << "Quaternion::multiply() must have one parameter, a Quaterion object, a Rotation object, or a Vector3 object.";
}

template <>
void JSBase::function<Quaternion, JSQuaternion::FI_SLERP>(Quaternion& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	Quaternion quaternionValue;
	Scalar scalarValue;

	if (hasValue<Quaternion>(info, 0u, quaternionValue) && hasValue<Scalar>(info, 1u, scalarValue))
	{
		info.GetReturnValue().Set(createObject<JSQuaternion>(thisValue.slerp(quaternionValue, scalarValue), JSContext::currentContext()));
		return;
	}

	Log::error() << "Quaternion::slerp() must have two parameters, a Quaternion object and a Number value.";
}

template <>
void JSBase::function<Quaternion, JSQuaternion::FI_STRING>(Quaternion& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	if (info.Length() != 0)
	{
		Log::warning() << "Quaternion::string() does not take any parameters.";
	}

	const std::string stringValue = String::toAString(thisValue.w(), 4u) + std::string(", ") + String::toAString(thisValue.x(), 4u) + std::string(", ") + String::toAString(thisValue.y(), 4u) + std::string(", ") + String::toAString(thisValue.z(), 4u);

	info.GetReturnValue().Set(newString(stringValue, v8::Isolate::GetCurrent()));
}

}

}

}
