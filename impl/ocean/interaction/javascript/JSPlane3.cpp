/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/interaction/javascript/JSPlane3.h"
#include "ocean/interaction/javascript/JSLine3.h"
#include "ocean/interaction/javascript/JSVector3.h"

#include "ocean/base/String.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

void JSPlane3::createFunctionTemplate()
{
	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	v8::Local<v8::FunctionTemplate> functionTemplate(v8::FunctionTemplate::New(isolate, constructor<Plane3>));
	functionTemplate->SetClassName(newString(objectName(), isolate));

	v8::Local<v8::ObjectTemplate> objectTemplate(v8::Local<v8::ObjectTemplate>::New(isolate, functionTemplate->InstanceTemplate()));
	objectTemplate->SetInternalFieldCount(1);

	objectTemplate->SetAccessor(newString("normal", isolate), propertyGetter<NativeType, AI_NORMAL>, propertySetter<NativeType, AI_NORMAL>);
	objectTemplate->SetAccessor(newString("distance", isolate), propertyGetter<NativeType, AI_DISTANCE>, propertySetter<NativeType, AI_DISTANCE>);

	objectTemplate->Set(newString("intersection", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_INTERSECTION>));
	objectTemplate->Set(newString("isEqual", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_IS_EQUAL>));
	objectTemplate->Set(newString("isParallel", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_IS_PARALLEL>));
	objectTemplate->Set(newString("string", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_STRING>));

	functionTemplate_.Reset(isolate, functionTemplate);
	objectTemplate_.Reset(isolate, objectTemplate);
}


template <>
void JSBase::constructor<Plane3>(Plane3& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
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
		Vector3 vectors[3];
		const unsigned int size = hasValues(info, 0u, 3u, vectors);

		if (size == 3u)
		{
			thisValue = Plane3(vectors[0], vectors[1], vectors[2]);
			return;
		}

		if (size == 2u)
		{
			if (!vectors[1].isUnit())
			{
				Log::warning() << "The provided plane normal is not a unit vector.";
			}

			thisValue = Plane3(vectors[0], vectors[1]);
			return;
		}
		else if (size == 1u)
		{
			Scalar distance;
			if (hasValue(info, 1u, distance))
			{
				if (!vectors[0].isUnit())
				{
					Log::warning() << "The provided plane normal is not a unit vector.";
				}

				thisValue = Plane3(vectors[0], distance);
				return;
			}
		}
	}
}

template <>
void JSBase::propertyGetter<Plane3, JSPlane3::AI_NORMAL>(Plane3& thisValue, v8::Local<v8::String>& /*property*/, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(createObject<JSVector3>(thisValue.normal(), JSContext::currentContext()));
}

template <>
void JSBase::propertyGetter<Plane3, JSPlane3::AI_DISTANCE>(Plane3& thisValue, v8::Local<v8::String>& /*property*/, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(double(thisValue.distance()));
}

template <>
void JSBase::propertySetter<Plane3, JSPlane3::AI_NORMAL>(Plane3& thisValue, v8::Local<v8::String>& /*property*/, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& /*info*/)
{
	Vector3 vectorValue;
	if (isValue(value, vectorValue))
	{
		if (vectorValue.isUnit())
		{
			thisValue.setNormal(vectorValue);
		}
		else
		{
			Log::error() << "Plane3::normal property accepts unit vectors only.";
		}
	}
	else
	{
		Log::error() << "The normal property of a Plane3 object accepts a Vector3 object only.";
	}
}

template <>
void JSBase::propertySetter<Plane3, JSPlane3::AI_DISTANCE>(Plane3& thisValue, v8::Local<v8::String>& /*property*/, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& /*info*/)
{
	Scalar scalarValue;
	if (isValue(value, scalarValue))
	{
		thisValue.setDistance(scalarValue);
	}
	else
	{
		Log::error() << "The distance property of a Plane3 object accepts a Number value only.";
	}
}

template <>
void JSBase::function<Plane3, JSPlane3::FI_INTERSECTION>(Plane3& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	Plane3 planeValue;
	if (hasValue<Plane3>(info, 0u, planeValue))
	{
		Line3 intersectionLine;

		if (thisValue.intersection(planeValue, intersectionLine))
		{
			info.GetReturnValue().Set(createObject<JSLine3>(intersectionLine, JSContext::currentContext()));
		}
		else
		{
			Log::warning() << "Plane3::intersection() did not provide an intersection with the given plane, as both planes are parallel.";
		}

		return;
	}

	Line3 lineValue;
	if (hasValue<Line3>(info, 0u, lineValue))
	{
		Vector3 intersectionPoint;

		if (thisValue.intersection(lineValue, intersectionPoint))
		{
			info.GetReturnValue().Set(createObject<JSVector3>(intersectionPoint, JSContext::currentContext()));
		}
		else
		{
			Log::warning() << "Plane3::intersection() did not provide an intersection with the given ray, as plane and ray are parallel.";
		}

		return;
	}

	Log::error() << "Plane3::intersection() accepts one Plane3 object, or one Line3 object as parameter only.";
}

template <>
void JSBase::function<Plane3, JSPlane3::FI_IS_EQUAL>(Plane3& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	Plane3 value;
	if (hasValue<Plane3>(info, 0u, value))
	{
		info.GetReturnValue().Set(thisValue == value);
		return;
	}

	Log::error() << "Plane3::isEqual() accepts one Plane3 object as parameter only.";
}

template <>
void JSBase::function<Plane3, JSPlane3::FI_IS_PARALLEL>(Plane3& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	Plane3 planeValue;
	if (hasValue<Plane3>(info, 0u, planeValue))
	{
		info.GetReturnValue().Set(thisValue.normal().isParallel(planeValue.normal()));
		return;
	}

	Line3 lineValue;
	if (hasValue<Line3>(info, 0u, lineValue))
	{
		info.GetReturnValue().Set(thisValue.normal().isOrthogonal(lineValue.direction()));
		return;
	}

	Log::error() << "Plane3::isParallel() accepts one Plane3 object or one Line3 object as parameter only.";
}

template <>
void JSBase::function<Plane3, JSPlane3::FI_STRING>(Plane3& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	if (info.Length() != 0)
	{
		Log::warning() << "Plane3::string() does not take any parameters.";
	}

	const std::string stringValue = String::toAString(thisValue.normal().x(), 4u) + std::string(", ") + String::toAString(thisValue.normal().y(), 4u) + std::string(", ") + String::toAString(thisValue.normal().z(), 4u)  + std::string(", ") + String::toAString(thisValue.distance(), 4u);

	info.GetReturnValue().Set(newString(stringValue, v8::Isolate::GetCurrent()));
}

}

}

}
