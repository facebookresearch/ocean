/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/interaction/javascript/JSExternal.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

JSExternal::JSExternal(void* data, const Type type, v8::Local<v8::Object>& owner, v8::Isolate* isolate) :
	data_(data),
	type_(type),
	persistent_(isolate, owner)
{
	ocean_assert(data_ != nullptr);
	ocean_assert(type_ != TYPE_UNDEFINED);

	persistent_.SetWeak(this, destructorCallback, v8::WeakCallbackType::kParameter);
}

JSExternal* JSExternal::external(const v8::Local<v8::Value>& value)
{
	if (value.IsEmpty())
	{
		return nullptr;
	}

	if (value->IsObject() == false)
	{
		return nullptr;
	}

	const v8::Local<v8::Object> object(v8::Local<v8::Object>::Cast(value));
	if (object->InternalFieldCount() != 1)
	{
		return nullptr;
	}

	const v8::Local<v8::Value> internalField(object->GetInternalField(0));
	if (internalField->IsObject() == false)
	{
		return nullptr;
	}

	const v8::Local<v8::External> wrapper(v8::Local<v8::External>::Cast(internalField));

	return static_cast<JSExternal*>(wrapper->Value());
}

JSExternal* JSExternal::external(const v8::Local<v8::Object>& object)
{
	if (object.IsEmpty())
	{
		return nullptr;
	}

	if (object->InternalFieldCount() != 1)
	{
		return nullptr;
	}

	const v8::Local<v8::Value> internalField(object->GetInternalField(0));
	if (internalField->IsObject() == false)
	{
		return nullptr;
	}

	const v8::Local<v8::External> wrapper(v8::Local<v8::External>::Cast(internalField));

	return static_cast<JSExternal*>(wrapper->Value());
}

JSExternal::~JSExternal()
{
	switch (type_)
	{
		case TYPE_COLOR:
			delete &value<RGBAColor>();
			break;

		case TYPE_DEVICE_OBJECT:
			delete &value<JSDevice>();
			break;

		case TYPE_DEVICE_SAMPLE:
			delete &value<Devices::Measurement::SampleRef>();
			break;

		case TYPE_EULER:
			delete &value<Euler>();
			break;

		case TYPE_HOMOGENOUS_MATRIX_4:
			delete &value<HomogenousMatrix4>();
			break;

		case TYPE_LINE_3:
			delete &value<Line3>();
			break;

		case TYPE_MEDIA_OBJECT:
			delete &value<Media::MediumRef>();
			break;

		case TYPE_PLANE_3:
			delete &value<Plane3>();
			break;

		case TYPE_QUATERNION:
			delete &value<Quaternion>();
			break;

		case TYPE_RENDERING_ENGINE:
			delete &value<Rendering::EngineRef>();
			break;

		case TYPE_RENDERING_OBJECT:
			delete &value<Rendering::ObjectRef>();
			break;

		case TYPE_ROTATION:
			delete &value<Rotation>();
			break;

		case TYPE_SCENE_DESCRIPTION_NODE:
			delete &value<SceneDescription::NodeRef>();
			break;

		case TYPE_SQUARE_MATRIX_3:
			delete &value<SquareMatrix3>();
			break;

		case TYPE_SQUARE_MATRIX_4:
			delete &value<SquareMatrix4>();
			break;

		case TYPE_VECTOR_2:
			delete &value<Vector2>();
			break;

		case TYPE_VECTOR_3:
			delete &value<Vector3>();
			break;

		case TYPE_VECTOR_4:
			delete &value<Vector4>();
			break;

		default:
			ocean_assert(false && "Unknown external data object");
	}
}

void JSExternal::destructorCallback(const v8::WeakCallbackInfo<JSExternal>& info)
{
	info.GetParameter()->persistent_.Reset();

	JSExternal* jsExternal = info.GetParameter();
	ocean_assert(jsExternal != nullptr);

	delete jsExternal;
}



}

}

}
