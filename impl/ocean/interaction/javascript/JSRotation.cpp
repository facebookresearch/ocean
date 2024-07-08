/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/interaction/javascript/JSRotation.h"
#include "ocean/interaction/javascript/JSVector3.h"

#include "ocean/base/String.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

void JSRotation::createFunctionTemplate()
{
	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	v8::Local<v8::FunctionTemplate> functionTemplate(v8::FunctionTemplate::New(isolate, constructor<Rotation>));
	functionTemplate->SetClassName(newString(objectName(), isolate));

	v8::Local<v8::ObjectTemplate> objectTemplate(v8::Local<v8::ObjectTemplate>::New(isolate, functionTemplate->InstanceTemplate()));
	objectTemplate->SetInternalFieldCount(1);

	objectTemplate->SetAccessor(newString("x", isolate), propertyGetter<NativeType, AI_X>);
	objectTemplate->SetAccessor(newString("y", isolate), propertyGetter<NativeType, AI_Y>);
	objectTemplate->SetAccessor(newString("z", isolate), propertyGetter<NativeType, AI_Z>);
	objectTemplate->SetAccessor(newString("axis", isolate), propertyGetter<NativeType, AI_AXIS>);
	objectTemplate->SetAccessor(newString("a", isolate), propertyGetter<NativeType, AI_A>);
	objectTemplate->SetAccessor(newString("angle", isolate), propertyGetter<NativeType, AI_ANGLE>);

	objectTemplate->Set(newString("invert", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_INVERT>));
	objectTemplate->Set(newString("inverted", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_INVERTED>));
	objectTemplate->Set(newString("isEqual", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_IS_EQUAL>));
	objectTemplate->Set(newString("multiply", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_MULTIPLY>));
	objectTemplate->Set(newString("string", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_STRING>));

	functionTemplate_.Reset(isolate, functionTemplate);
	objectTemplate_.Reset(isolate, objectTemplate);
}

template <>
void JSBase::constructor<Rotation>(Rotation& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
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

			thisValue = Rotation(vector3, scalar);
			return;
		}

		Quaternion quaternion;
		if (hasValue(info, 0u, quaternion))
		{
			thisValue = Rotation(quaternion);
			return;
		}

		Euler euler;
		if (hasValue(info, 0u, euler))
		{
			thisValue = Rotation(euler);
			return;
		}

		HomogenousMatrix4 homogenousMatrix;
		if (hasValue(info, 0u, homogenousMatrix))
		{
			thisValue = Rotation(homogenousMatrix.rotation());
			return;
		}
	}
	else if (info[0]->IsArray())
	{
		std::vector<Scalar> values;
		if (hasValue(info, 0u, values) && values.size() == 4u)
		{
			thisValue = Rotation(values.data());

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
			thisValue = Rotation(values[0], values[1], values[2], values[3]);

			if (!thisValue.isValid())
			{
				Log::warning() << "The rotation axis must be a unit vector";
			}

			return;
		}
	}
}

template <>
void JSBase::propertyGetter<Rotation, JSRotation::AI_X>(Rotation& thisValue, v8::Local<v8::String>& /*property*/, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(double(thisValue.axis().x()));
}

template <>
void JSBase::propertyGetter<Rotation, JSRotation::AI_Y>(Rotation& thisValue, v8::Local<v8::String>& /*property*/, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(double(thisValue.axis().y()));
}

template <>
void JSBase::propertyGetter<Rotation, JSRotation::AI_Z>(Rotation& thisValue, v8::Local<v8::String>& /*property*/, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(double(thisValue.axis().z()));
}

template <>
void JSBase::propertyGetter<Rotation, JSRotation::AI_AXIS>(Rotation& thisValue, v8::Local<v8::String>& /*property*/, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(createObject<JSVector3>(thisValue.axis(), JSContext::currentContext()));
}

template <>
void JSBase::propertyGetter<Rotation, JSRotation::AI_A>(Rotation& thisValue, v8::Local<v8::String>& /*property*/, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(double(thisValue.angle()));
}

template <>
void JSBase::propertyGetter<Rotation, JSRotation::AI_ANGLE>(Rotation& thisValue, v8::Local<v8::String>& /*property*/, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(double(thisValue.angle()));
}

template <>
void JSBase::function<Rotation, JSRotation::FI_INVERT>(Rotation& thisValue, const v8::FunctionCallbackInfo<v8::Value>& /*info*/)
{
	thisValue.invert();
}

template <>
void JSBase::function<Rotation, JSRotation::FI_INVERTED>(Rotation& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(createObject<JSRotation>(thisValue.inverted(), JSContext::currentContext()));
}

template <>
void JSBase::function<Rotation, JSRotation::FI_IS_EQUAL>(Rotation& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	Rotation value;
	if (hasValue<Rotation>(info, 0u, value))
	{
		info.GetReturnValue().Set(thisValue == value);
		return;
	}

	Log::error() << "Rotation::isEqual() accepts one Rotation object as parameter only.";
}

template <>
void JSBase::function<Rotation, JSRotation::FI_MULTIPLY>(Rotation& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	Rotation rotationValue;
	if (hasValue<Rotation>(info, 0u, rotationValue))
	{
		info.GetReturnValue().Set(createObject<JSRotation>(thisValue * rotationValue, JSContext::currentContext()));
		return;
	}

	Quaternion quaternionValue;
	if (hasValue<Quaternion>(info, 0u, quaternionValue))
	{
		info.GetReturnValue().Set(createObject<JSRotation>(thisValue * quaternionValue, JSContext::currentContext()));
		return;
	}

	Vector3 vectorValue3;
	if (hasValue<Vector3>(info, 0u, vectorValue3))
	{
		info.GetReturnValue().Set(createObject<JSVector3>(thisValue * vectorValue3, JSContext::currentContext()));
		return;
	}

	Log::error() << "Rotation::multiply() must have one parameter, a Rotation object, a Quaternion object, or a Vector3 object.";
}

template <>
void JSBase::function<Rotation, JSRotation::FI_STRING>(Rotation& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	if (info.Length() != 0)
	{
		Log::warning() << "Rotation::string() does not take any parameters.";
	}

	const std::string stringValue = String::toAString(thisValue.axis().x(), 4u) + std::string(", ") + String::toAString(thisValue.axis().y(), 4u) + std::string(", ") + String::toAString(thisValue.axis().z(), 4u) + std::string(", ") + String::toAString(thisValue.angle(), 4u);

	info.GetReturnValue().Set(newString(stringValue, v8::Isolate::GetCurrent()));
}

}

}

}
