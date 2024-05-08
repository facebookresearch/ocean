/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/interaction/javascript/JSSquareMatrix3.h"
#include "ocean/interaction/javascript/JSVector2.h"
#include "ocean/interaction/javascript/JSVector3.h"

#include "ocean/base/String.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

void JSSquareMatrix3::createFunctionTemplate()
{
	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	v8::Local<v8::FunctionTemplate> functionTemplate(v8::FunctionTemplate::New(isolate, constructor<SquareMatrix3>));
	functionTemplate->SetClassName(newString(objectName(), isolate));

	v8::Local<v8::ObjectTemplate> objectTemplate(v8::Local<v8::ObjectTemplate>::New(isolate, functionTemplate->InstanceTemplate()));
	objectTemplate->SetInternalFieldCount(1);

	objectTemplate->Set(newString("add", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_ADD>));
	objectTemplate->Set(newString("determinant", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_DETERMINANT>));
	objectTemplate->Set(newString("invert", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_INVERT>));
	objectTemplate->Set(newString("inverted", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_INVERTED>));
	objectTemplate->Set(newString("isEqual", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_IS_EQUAL>));
	objectTemplate->Set(newString("multiply", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_MULTIPLY>));
	objectTemplate->Set(newString("string", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_STRING>));
	objectTemplate->Set(newString("subtract", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SUBTRACT>));
	objectTemplate->Set(newString("trace", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_TRACE>));
	objectTemplate->Set(newString("transpose", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_TRANSPOSE>));
	objectTemplate->Set(newString("transposed", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_TRANSPOSED>));
	objectTemplate->Set(newString("xAxis", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_X_AXIS>));
	objectTemplate->Set(newString("yAxis", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_Y_AXIS>));
	objectTemplate->Set(newString("zAxis", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_Z_AXIS>));

	functionTemplate_.Reset(isolate, functionTemplate);
	objectTemplate_.Reset(isolate, objectTemplate);
}

template <>
void JSBase::constructor<SquareMatrix3>(SquareMatrix3& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	thisValue.toIdentity();

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
		Euler euler;
		if (hasValue(info, 0u, euler))
		{
			thisValue = SquareMatrix3(euler);
			return;
		}

		Quaternion quaternion;
		if (hasValue(info, 0u, quaternion))
		{
			thisValue = SquareMatrix3(quaternion);
			return;
		}

		Rotation rotation;
		if (hasValue(info, 0u, rotation))
		{
			thisValue = SquareMatrix3(rotation);
			return;
		}

		Vector3 axis[3];
		if (hasValues(info, 0u, 3u, axis) == 3u)
		{
			thisValue = SquareMatrix3(axis[0], axis[1], axis[2]);
		}
	}
	else if (info[0]->IsArray())
	{
		std::vector<Scalar> values;
		if (hasValue(info, 0u, values) && values.size() == 9u)
		{
			thisValue = SquareMatrix3(values.data());
		}
	}
	else
	{
		Scalar values[9];
		if (hasValues<Scalar>(info, 0u, 9u, values) == 9u)
		{
			thisValue = SquareMatrix3(values);
			return;
		}
	}
}

template <>
void JSBase::function<SquareMatrix3, JSSquareMatrix3::FI_ADD>(SquareMatrix3& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	SquareMatrix3 value;
	if (hasValue<SquareMatrix3>(info, 0u, value))
	{
		info.GetReturnValue().Set(createObject<JSSquareMatrix3>(thisValue + value, JSContext::currentContext()));
		return;
	}

	Log::error() << "SquareMatrix3::add() accepts one SquareMatrix3 object as parameter only.";
}

template <>
void JSBase::function<SquareMatrix3, JSSquareMatrix3::FI_DETERMINANT>(SquareMatrix3& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(double(thisValue.determinant()));
}

template <>
void JSBase::function<SquareMatrix3, JSSquareMatrix3::FI_INVERT>(SquareMatrix3& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(thisValue.invert());
}

template <>
void JSBase::function<SquareMatrix3, JSSquareMatrix3::FI_INVERTED>(SquareMatrix3& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	SquareMatrix3 invertedMatrix(thisValue);
	invertedMatrix.invert();

	info.GetReturnValue().Set(createObject<JSSquareMatrix3>(invertedMatrix, JSContext::currentContext()));
}

template <>
void JSBase::function<SquareMatrix3, JSSquareMatrix3::FI_IS_EQUAL>(SquareMatrix3& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	SquareMatrix3 value;
	if (hasValue<SquareMatrix3>(info, 0u, value))
	{
		info.GetReturnValue().Set(thisValue == value);
		return;
	}

	Log::error() << "SquareMatrix3::isEqual() accepts one SquareMatrix3 object as parameter only.";
}

template <>
void JSBase::function<SquareMatrix3, JSSquareMatrix3::FI_MULTIPLY>(SquareMatrix3& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	SquareMatrix3 matrixValue;
	if (hasValue<SquareMatrix3>(info, 0u, matrixValue))
	{
		info.GetReturnValue().Set(createObject<JSSquareMatrix3>(thisValue * matrixValue, JSContext::currentContext()));
		return;
	}

	Vector3 vectorValue3;
	if (hasValue<Vector3>(info, 0u, vectorValue3))
	{
		info.GetReturnValue().Set(createObject<JSVector3>(thisValue * vectorValue3, JSContext::currentContext()));
		return;
	}

	Vector2 vectorValue2;
	if (hasValue<Vector2>(info, 0u, vectorValue2))
	{
		info.GetReturnValue().Set(createObject<JSVector2>(thisValue * vectorValue2, JSContext::currentContext()));
		return;
	}

	Scalar scalarValue;
	if (hasValue<Scalar>(info, 0u, scalarValue))
	{
		info.GetReturnValue().Set(createObject<JSSquareMatrix3>(thisValue * scalarValue, JSContext::currentContext()));
		return;
	}

	Log::error() << "SquareMatrix3::multiply() must have one parameter, a SquareMatrix3 object, a Vector3 object, a Vector2 object, or a Number.";
}

template <>
void JSBase::function<SquareMatrix3, JSSquareMatrix3::FI_STRING>(SquareMatrix3& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	if (info.Length() != 0)
	{
		Log::warning() << "JSSquareMatrix3::string() does not take any parameters.";
	}

	const std::string stringValue = String::toAString(thisValue[0], 4u) + std::string(", ") + String::toAString(thisValue[1], 4u) + std::string(", ") + String::toAString(thisValue[2], 4u) + std::string(", ")
									+ String::toAString(thisValue[3], 4u) + std::string(", ") + String::toAString(thisValue[4], 4u) + std::string(", ") + String::toAString(thisValue[5], 4u) + std::string(", ")
									+ String::toAString(thisValue[6], 4u) + std::string(", ") + String::toAString(thisValue[7], 4u) + std::string(", ") +	String::toAString(thisValue[8], 4u);

	info.GetReturnValue().Set(newString(stringValue, v8::Isolate::GetCurrent()));
}

template <>
void JSBase::function<SquareMatrix3, JSSquareMatrix3::FI_SUBTRACT>(SquareMatrix3& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	SquareMatrix3 value;
	if (hasValue<SquareMatrix3>(info, 0u, value))
	{
		info.GetReturnValue().Set(createObject<JSSquareMatrix3>(thisValue - value, JSContext::currentContext()));
		return;
	}

	Log::error() << "SquareMatrix3::subtract() accepts one SquareMatrix3 object as parameter only.";
}

template <>
void JSBase::function<SquareMatrix3, JSSquareMatrix3::FI_TRACE>(SquareMatrix3& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(double(thisValue.trace()));
}

template <>
void JSBase::function<SquareMatrix3, JSSquareMatrix3::FI_TRANSPOSE>(SquareMatrix3& thisValue, const v8::FunctionCallbackInfo<v8::Value>& /*info*/)
{
	thisValue.transpose();
}

template <>
void JSBase::function<SquareMatrix3, JSSquareMatrix3::FI_TRANSPOSED>(SquareMatrix3& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(createObject<JSSquareMatrix3>(thisValue.transposed(), JSContext::currentContext()));
}

template <>
void JSBase::function<SquareMatrix3, JSSquareMatrix3::FI_X_AXIS>(SquareMatrix3& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(createObject<JSVector3>(thisValue.xAxis(), JSContext::currentContext()));
}

template <>
void JSBase::function<SquareMatrix3, JSSquareMatrix3::FI_Y_AXIS>(SquareMatrix3& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(createObject<JSVector3>(thisValue.yAxis(), JSContext::currentContext()));
}

template <>
void JSBase::function<SquareMatrix3, JSSquareMatrix3::FI_Z_AXIS>(SquareMatrix3& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(createObject<JSVector3>(thisValue.zAxis(), JSContext::currentContext()));
}

}

}

}
