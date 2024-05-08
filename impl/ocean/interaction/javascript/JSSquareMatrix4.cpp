/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/interaction/javascript/JSSquareMatrix4.h"
#include "ocean/interaction/javascript/JSVector3.h"
#include "ocean/interaction/javascript/JSVector4.h"

#include "ocean/base/String.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

void JSSquareMatrix4::createFunctionTemplate()
{
	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	v8::Local<v8::FunctionTemplate> functionTemplate(v8::FunctionTemplate::New(isolate, constructor<SquareMatrix4>));
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

	functionTemplate_.Reset(isolate, functionTemplate);
	objectTemplate_.Reset(isolate, objectTemplate);
}

template <>
void JSBase::constructor<SquareMatrix4>(SquareMatrix4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
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
		HomogenousMatrix4 homogenousMatrix;
		if (hasValue(info, 0u, homogenousMatrix))
		{
			thisValue = SquareMatrix4(homogenousMatrix);
			return;
		}
	}
	else if (info[0]->IsArray())
	{
		std::vector<Scalar> values;
		if (hasValue(info, 0u, values) && values.size() == 16u)
		{
			thisValue = SquareMatrix4(values.data());
			return;
		}

		if (values.size() == 4)
		{
			thisValue = SquareMatrix4::projectionMatrix(values[0], values[1], values[2], values[3]);
			return;
		}

		if (values.size() == 6)
		{
			thisValue = SquareMatrix4::frustumMatrix(values[0], values[1], values[2], values[3], values[4], values[5]);
			return;
		}
	}
	else
	{
		Scalar values[16];
		const unsigned int size = hasValues<Scalar>(info, 0u, 16u, values);

		if (size == 16u)
		{
			thisValue = SquareMatrix4(values);
			return;
		}

		if (size == 4u)
		{
			thisValue = SquareMatrix4::projectionMatrix(values[0], values[1], values[2], values[3]);
			return;
		}

		if (size == 6u)
		{
			thisValue = SquareMatrix4::frustumMatrix(values[0], values[1], values[2], values[3], values[4], values[5]);
			return;
		}
	}
}

template <>
void JSBase::function<SquareMatrix4, JSSquareMatrix4::FI_ADD>(SquareMatrix4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	SquareMatrix4 value;
	if (hasValue<SquareMatrix4>(info, 0u, value))
	{
		info.GetReturnValue().Set(createObject<JSSquareMatrix4>(thisValue + value, JSContext::currentContext()));
		return;
	}

	Log::error() << "SquareMatrix4::add() accepts one SquareMatrix4 object as parameter only.";
}

template <>
void JSBase::function<SquareMatrix4, JSSquareMatrix4::FI_DETERMINANT>(SquareMatrix4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(double(thisValue.determinant()));
}

template <>
void JSBase::function<SquareMatrix4, JSSquareMatrix4::FI_INVERT>(SquareMatrix4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(thisValue.invert());
}

template <>
void JSBase::function<SquareMatrix4, JSSquareMatrix4::FI_INVERTED>(SquareMatrix4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	SquareMatrix4 invertedMatrix(thisValue);
	invertedMatrix.invert();

	info.GetReturnValue().Set(createObject<JSSquareMatrix4>(invertedMatrix, JSContext::currentContext()));
}

template <>
void JSBase::function<SquareMatrix4, JSSquareMatrix4::FI_IS_EQUAL>(SquareMatrix4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	SquareMatrix4 value;
	if (hasValue<SquareMatrix4>(info, 0u, value))
	{
		info.GetReturnValue().Set(thisValue == value);
		return;
	}

	Log::error() << "SquareMatrix4::isEqual() accepts one SquareMatrix4 object as parameter only.";
}

template <>
void JSBase::function<SquareMatrix4, JSSquareMatrix4::FI_MULTIPLY>(SquareMatrix4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	SquareMatrix4 squareMatrixValue;
	if (hasValue<SquareMatrix4>(info, 0u, squareMatrixValue))
	{
		info.GetReturnValue().Set(createObject<JSSquareMatrix4>(thisValue * squareMatrixValue, JSContext::currentContext()));
		return;
	}

	HomogenousMatrix4 homogenousMatrixValue;
	if (hasValue<HomogenousMatrix4>(info, 0u, homogenousMatrixValue))
	{
		info.GetReturnValue().Set(createObject<JSSquareMatrix4>(thisValue * homogenousMatrixValue, JSContext::currentContext()));
		return;
	}

	Vector4 vectorValue4;
	if (hasValue<Vector4>(info, 0u, vectorValue4))
	{
		info.GetReturnValue().Set(createObject<JSVector4>(thisValue * vectorValue4, JSContext::currentContext()));
		return;
	}

	Vector3 vectorValue3;
	if (hasValue<Vector3>(info, 0u, vectorValue3))
	{
		info.GetReturnValue().Set(createObject<JSVector3>(thisValue * vectorValue3, JSContext::currentContext()));
		return;
	}

	Scalar scalarValue;
	if (hasValue<Scalar>(info, 0u, scalarValue))
	{
		info.GetReturnValue().Set(createObject<JSSquareMatrix4>(thisValue * scalarValue, JSContext::currentContext()));
		return;
	}

	Log::error() << "SquareMatrix4::multiply() must have one parameter, a SquareMatrix4 object, a HomogenousMatrix4 object, a Vector4 object, a Vector3 object, or a Number.";
}

template <>
void JSBase::function<SquareMatrix4, JSSquareMatrix4::FI_STRING>(SquareMatrix4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	if (info.Length() != 0)
	{
		Log::warning() << "JSSquareMatrix4::string() does not take any parameters.";
	}

	const std::string stringValue = String::toAString(thisValue[0], 4u) + std::string(", ") + String::toAString(thisValue[1], 4u) + std::string(", ") + String::toAString(thisValue[2], 4u) + std::string(", ") + String::toAString(thisValue[3], 4u) + std::string(", ")
										+ String::toAString(thisValue[4], 4u) + std::string(", ") + String::toAString(thisValue[5], 4u) + std::string(", ") + String::toAString(thisValue[6], 4u) + std::string(", ") + String::toAString(thisValue[7], 4u) + std::string(", ")
										+ String::toAString(thisValue[8], 4u) + std::string(", ") + String::toAString(thisValue[9], 4u) + std::string(", ") + String::toAString(thisValue[10], 4u) + std::string(", ") + String::toAString(thisValue[11], 4u) + std::string(", ")
										+ String::toAString(thisValue[12], 4u) + std::string(", ") + String::toAString(thisValue[13], 4u) + std::string(", ") + String::toAString(thisValue[14], 4u) + std::string(", ") + String::toAString(thisValue[15], 4u);

	info.GetReturnValue().Set(newString(stringValue, v8::Isolate::GetCurrent()));
}

template <>
void JSBase::function<SquareMatrix4, JSSquareMatrix4::FI_SUBTRACT>(SquareMatrix4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	SquareMatrix4 value;
	if (hasValue<SquareMatrix4>(info, 0u, value))
	{
		info.GetReturnValue().Set(createObject<JSSquareMatrix4>(thisValue - value, JSContext::currentContext()));
		return;
	}

	Log::error() << "SquareMatrix4::subtract() accepts one SquareMatrix4 object as parameter only.";
}

template <>
void JSBase::function<SquareMatrix4, JSSquareMatrix4::FI_TRACE>(SquareMatrix4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(double(thisValue.trace()));
}

template <>
void JSBase::function<SquareMatrix4, JSSquareMatrix4::FI_TRANSPOSE>(SquareMatrix4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& /*info*/)
{
	thisValue.transpose();
}

template <>
void JSBase::function<SquareMatrix4, JSSquareMatrix4::FI_TRANSPOSED>(SquareMatrix4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(createObject<JSSquareMatrix4>(thisValue.transposed(), JSContext::currentContext()));
}

}

}

}
