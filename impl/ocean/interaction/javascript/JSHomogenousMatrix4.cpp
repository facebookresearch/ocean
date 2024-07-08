/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/interaction/javascript/JSHomogenousMatrix4.h"
#include "ocean/interaction/javascript/JSEuler.h"
#include "ocean/interaction/javascript/JSRotation.h"
#include "ocean/interaction/javascript/JSQuaternion.h"
#include "ocean/interaction/javascript/JSSquareMatrix4.h"
#include "ocean/interaction/javascript/JSVector3.h"
#include "ocean/interaction/javascript/JSVector4.h"

#include "ocean/base/String.h"

#include "ocean/math/Interpolation.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

void JSHomogenousMatrix4::createFunctionTemplate()
{
	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	v8::Local<v8::FunctionTemplate> functionTemplate(v8::FunctionTemplate::New(isolate, constructor<HomogenousMatrix4>));
	functionTemplate->SetClassName(newString(objectName(), isolate));

	v8::Local<v8::ObjectTemplate> objectTemplate(v8::Local<v8::ObjectTemplate>::New(isolate, functionTemplate->InstanceTemplate()));
	objectTemplate->SetInternalFieldCount(1);

	objectTemplate->Set(newString("determinant", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_DETERMINANT>));
	objectTemplate->Set(newString("invert", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_INVERT>));
	objectTemplate->Set(newString("inverted", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_INVERTED>));
	objectTemplate->Set(newString("interpolate", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_INTERPOLATE>));
	objectTemplate->Set(newString("isEqual", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_IS_EQUAL>));
	objectTemplate->Set(newString("multiply", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_MULTIPLY>));
	objectTemplate->Set(newString("rotation", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_ROTATION>));
	objectTemplate->Set(newString("scale", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SCALE>));
	objectTemplate->Set(newString("setRotation", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_ROTATION>));
	objectTemplate->Set(newString("setTranslation", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_TRANSLATION>));
	objectTemplate->Set(newString("string", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_STRING>));
	objectTemplate->Set(newString("trace", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_TRACE>));
	objectTemplate->Set(newString("translation", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_TRANSLATION>));
	objectTemplate->Set(newString("transposed", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_TRANSPOSED>));
	objectTemplate->Set(newString("xAxis", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_X_AXIS>));
	objectTemplate->Set(newString("yAxis", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_Y_AXIS>));
	objectTemplate->Set(newString("zAxis", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_Z_AXIS>));

	functionTemplate_.Reset(isolate, functionTemplate);
	objectTemplate_.Reset(isolate, objectTemplate);
}

template <>
void JSBase::constructor<HomogenousMatrix4>(HomogenousMatrix4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
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
		Vector3 vectors[4];
		const unsigned int numberVectors = hasValues(info, 0u, 4u, vectors);

		if (numberVectors == 4u)
		{
			thisValue = HomogenousMatrix4(vectors[0], vectors[1], vectors[2], vectors[3]);
			return;
		}

		if (numberVectors == 3u)
		{
			thisValue = HomogenousMatrix4(vectors[0], vectors[1], vectors[2]);
			return;
		}

		if (numberVectors == 2u)
		{
			thisValue = HomogenousMatrix4(vectors[0], vectors[1]);
			return;
		}

		if (numberVectors == 1u)
		{
			Euler euler;
			if (hasValue(info, 1u, euler))
			{
				Vector3 scale;
				if (hasValue(info, 2u, scale))
				{
					thisValue = HomogenousMatrix4(vectors[0], Quaternion(euler), scale);
				}
				else
				{
					thisValue = HomogenousMatrix4(vectors[0], euler);
				}

				return;
			}

			Quaternion quaternion;
			if (hasValue(info, 1u, quaternion))
			{
				Vector3 scale;
				if (hasValue(info, 2u, scale))
				{
					thisValue = HomogenousMatrix4(vectors[0], quaternion, scale);
				}
				else
				{
					thisValue = HomogenousMatrix4(vectors[0], quaternion);
				}

				return;
			}

			Rotation rotation;
			if (hasValue(info, 1u, rotation))
			{
				Vector3 scale;
				if (hasValue(info, 2u, scale))
				{
					thisValue = HomogenousMatrix4(vectors[0], rotation, scale);
				}
				else
				{
					thisValue = HomogenousMatrix4(vectors[0], rotation);
				}

				return;
			}

			SquareMatrix3 squareMatrix3;
			if (hasValue(info, 1u, squareMatrix3))
			{
				Vector3 scale;
				if (hasValue(info, 2u, scale))
				{
					thisValue = HomogenousMatrix4(vectors[0], Quaternion(squareMatrix3), scale);
				}
				else
				{
					thisValue = HomogenousMatrix4(vectors[0], squareMatrix3);
				}

				return;
			}

			thisValue = HomogenousMatrix4(vectors[0]);
			return;
		}
		else
		{
			ocean_assert(numberVectors == 0u);

			Euler euler;
			if (hasValue(info, 0u, euler))
			{
				thisValue = HomogenousMatrix4(euler);
				return;
			}

			Quaternion quaternion;
			if (hasValue(info, 0u, quaternion))
			{
				thisValue = HomogenousMatrix4(quaternion);
				return;
			}

			Rotation rotation;
			if (hasValue(info, 0u, rotation))
			{
				thisValue = HomogenousMatrix4(rotation);
				return;
			}

			SquareMatrix3 squareMatrix3;
			if (hasValue(info, 0u, squareMatrix3))
			{
				thisValue = HomogenousMatrix4(squareMatrix3);
				return;
			}
		}
	}
	else if (info[0]->IsArray())
	{
		std::vector<Scalar> values;
		if (hasValue(info, 0u, values) && values.size() == 16u)
		{
			thisValue = HomogenousMatrix4(values.data());
		}
	}
	else
	{
		Scalar values[16];
		if (hasValues<Scalar>(info, 0u, 16u, values) == 16u)
		{
			thisValue = HomogenousMatrix4(values);
		}
	}

	if (!thisValue.isValid())
	{
		Log::warning() << "The povided values created an invalid HomogenousMatrix4 object";
	}
}

template <>
void JSBase::function<HomogenousMatrix4, JSHomogenousMatrix4::FI_DETERMINANT>(HomogenousMatrix4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(double(thisValue.determinant()));
}

template <>
void JSBase::function<HomogenousMatrix4, JSHomogenousMatrix4::FI_INVERT>(HomogenousMatrix4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(thisValue.invert());
}

template <>
void JSBase::function<HomogenousMatrix4, JSHomogenousMatrix4::FI_INVERTED>(HomogenousMatrix4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	HomogenousMatrix4 invertedMatrix(thisValue);
	invertedMatrix.invert();

	info.GetReturnValue().Set(createObject<JSHomogenousMatrix4>(invertedMatrix, JSContext::currentContext()));
}

template <>
void JSBase::function<HomogenousMatrix4, JSHomogenousMatrix4::FI_INTERPOLATE>(HomogenousMatrix4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	HomogenousMatrix4 matrixValue;
	Scalar scalarValue;

	if (hasValue<HomogenousMatrix4>(info, 0u, matrixValue) && hasValue<Scalar>(info, 1u, scalarValue))
	{
		if (scalarValue >= Scalar(0) && scalarValue <= Scalar(1))
		{
			info.GetReturnValue().Set(createObject<JSHomogenousMatrix4>(Interpolation::linear<HomogenousMatrix4>(thisValue, matrixValue, scalarValue), JSContext::currentContext()));
		}
		else
		{
			Log::warning() << "HomogenousMatrix4::interpolate() accepts interpolation factors with value range [0.0, 1.0] only.";
		}
	}
	else
	{
		Log::error() << "HomogenousMatrix4::interpolate() accepts a HomogenousMatrix4 object as first parameter and an interpolation Number value as second parameter only.";
	}
}

template <>
void JSBase::function<HomogenousMatrix4, JSHomogenousMatrix4::FI_IS_EQUAL>(HomogenousMatrix4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	HomogenousMatrix4 value;
	if (hasValue<HomogenousMatrix4>(info, 0u, value))
	{
		info.GetReturnValue().Set(thisValue == value);
		return;
	}

	Log::error() << "HomogenousMatrix4::isEqual() accepts one HomogenousMatrix4 object as parameter only.";
}

template <>
void JSBase::function<HomogenousMatrix4, JSHomogenousMatrix4::FI_MULTIPLY>(HomogenousMatrix4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	HomogenousMatrix4 matrixValue;
	if (hasValue<HomogenousMatrix4>(info, 0u, matrixValue))
	{
		info.GetReturnValue().Set(createObject<JSHomogenousMatrix4>(thisValue * matrixValue, JSContext::currentContext()));
		return;
	}

	Vector3 vectorValue3;
	if (hasValue<Vector3>(info, 0u, vectorValue3))
	{
		info.GetReturnValue().Set(createObject<JSVector3>(thisValue * vectorValue3, JSContext::currentContext()));
		return;
	}

	Vector4 vectorValue4;
	if (hasValue<Vector4>(info, 0u, vectorValue4))
	{
		info.GetReturnValue().Set(createObject<JSVector4>(thisValue * vectorValue4, JSContext::currentContext()));
		return;
	}

	Rotation rotationValue;
	if (hasValue<Rotation>(info, 0u, rotationValue))
	{
		info.GetReturnValue().Set(createObject<JSHomogenousMatrix4>(thisValue * rotationValue, JSContext::currentContext()));
		return;
	}

	Quaternion quaternionValue;
	if (hasValue<Quaternion>(info, 0u, quaternionValue))
	{
		info.GetReturnValue().Set(createObject<JSHomogenousMatrix4>(thisValue * quaternionValue, JSContext::currentContext()));
		return;
	}

	Euler eulerValue;
	if (hasValue<Euler>(info, 0u, eulerValue))
	{
		info.GetReturnValue().Set(createObject<JSHomogenousMatrix4>(thisValue * HomogenousMatrix4(eulerValue), JSContext::currentContext()));
		return;
	}

	Log::error() << "HomogenousMatrix4::multiply() must have one parameter, a HomogenousMatrix4 object, a Vector3 object, a Vector3 object, a Rotation object, a Quaternion object, or an Euler object.";
}

template <>
void JSBase::function<HomogenousMatrix4, JSHomogenousMatrix4::FI_ROTATION>(HomogenousMatrix4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(createObject<JSRotation>(Rotation(thisValue.rotation()), JSContext::currentContext()));
}

template <>
void JSBase::function<HomogenousMatrix4, JSHomogenousMatrix4::FI_SCALE>(HomogenousMatrix4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(createObject<JSVector3>(thisValue.scale(), JSContext::currentContext()));
}

template <>
void JSBase::function<HomogenousMatrix4, JSHomogenousMatrix4::FI_SET_ROTATION>(HomogenousMatrix4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	Rotation rotationValue;
	if (hasValue<Rotation>(info, 0u, rotationValue))
	{
		thisValue.setRotation(rotationValue);
		return;
	}

	Quaternion quaternionValue;
	if (hasValue<Quaternion>(info, 0u, quaternionValue))
	{
		thisValue.setRotation(quaternionValue);
		return;
	}

	Euler eulerValue;
	if (hasValue<Euler>(info, 0u, eulerValue))
	{
		thisValue.setRotation(Quaternion(eulerValue));
		return;
	}

	SquareMatrix3 squareMatrixValue;
	if (hasValue<SquareMatrix3>(info, 0u, squareMatrixValue))
	{
		thisValue.setRotation(squareMatrixValue);
		return;
	}

	HomogenousMatrix4 homogenousMatrixValue;
	if (hasValue<HomogenousMatrix4>(info, 0u, homogenousMatrixValue))
	{
		thisValue.setRotation(homogenousMatrixValue.rotation());
		return;
	}

	Log::error() << "HomogenousMatrix4::setRotation() must have one parameter, a Rotation object, a Quaternion object, an Euler object, a SquareMatrix3 object, or a HomogenousMatrix4 object.";
}

template <>
void JSBase::function<HomogenousMatrix4, JSHomogenousMatrix4::FI_SET_TRANSLATION>(HomogenousMatrix4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	Vector3 vectorValue;
	if (hasValue<Vector3>(info, 0u, vectorValue))
	{
		thisValue.setTranslation(vectorValue);
		return;
	}

	HomogenousMatrix4 homogenousMatrixValue;
	if (hasValue<HomogenousMatrix4>(info, 0u, homogenousMatrixValue))
	{
		thisValue.setTranslation(homogenousMatrixValue.translation());
		return;
	}

	Log::error() << "HomogenousMatrix4::setTranslation() must have one parameter, a Vector3 object, or a HomogenousMatrix4 object.";
}

template <>
void JSBase::function<HomogenousMatrix4, JSHomogenousMatrix4::FI_STRING>(HomogenousMatrix4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	if (info.Length() != 0)
	{
		Log::warning() << "JSHomogenousMatrix4::string() does not take any parameters.";
	}

	const std::string stringValue = String::toAString(thisValue[0], 4u) + std::string(", ") + String::toAString(thisValue[1], 4u) + std::string(", ") + String::toAString(thisValue[2], 4u) + std::string(", ") + String::toAString(thisValue[3], 4u) + std::string(", ")
										+ String::toAString(thisValue[4], 4u) + std::string(", ") + String::toAString(thisValue[5], 4u) + std::string(", ") + String::toAString(thisValue[6], 4u) + std::string(", ") + String::toAString(thisValue[7], 4u) + std::string(", ")
										+ String::toAString(thisValue[8], 4u) + std::string(", ") + String::toAString(thisValue[9], 4u) + std::string(", ") + String::toAString(thisValue[10], 4u) + std::string(", ") + String::toAString(thisValue[11], 4u) + std::string(", ")
										+ String::toAString(thisValue[12], 4u) + std::string(", ") + String::toAString(thisValue[13], 4u) + std::string(", ") + String::toAString(thisValue[14], 4u) + std::string(", ") + String::toAString(thisValue[15], 4u);

	info.GetReturnValue().Set(newString(stringValue, v8::Isolate::GetCurrent()));
}

template <>
void JSBase::function<HomogenousMatrix4, JSHomogenousMatrix4::FI_TRACE>(HomogenousMatrix4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(double(thisValue.trace()));
}

template <>
void JSBase::function<HomogenousMatrix4, JSHomogenousMatrix4::FI_TRANSLATION>(HomogenousMatrix4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(createObject<JSVector3>(thisValue.translation(), JSContext::currentContext()));
}

template <>
void JSBase::function<HomogenousMatrix4, JSHomogenousMatrix4::FI_TRANSPOSED>(HomogenousMatrix4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(createObject<JSSquareMatrix4>(thisValue.transposed(), JSContext::currentContext()));
}

template <>
void JSBase::function<HomogenousMatrix4, JSHomogenousMatrix4::FI_X_AXIS>(HomogenousMatrix4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(createObject<JSVector3>(thisValue.xAxis(), JSContext::currentContext()));
}

template <>
void JSBase::function<HomogenousMatrix4, JSHomogenousMatrix4::FI_Y_AXIS>(HomogenousMatrix4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(createObject<JSVector3>(thisValue.yAxis(), JSContext::currentContext()));
}

template <>
void JSBase::function<HomogenousMatrix4, JSHomogenousMatrix4::FI_Z_AXIS>(HomogenousMatrix4& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(createObject<JSVector3>(thisValue.zAxis(), JSContext::currentContext()));
}

}

}

}
