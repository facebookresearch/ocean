/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/interaction/javascript/JSSceneDescriptionNode.h"
#include "ocean/interaction/javascript/JSColor.h"
#include "ocean/interaction/javascript/JSHomogenousMatrix4.h"
#include "ocean/interaction/javascript/JSMediaObject.h"
#include "ocean/interaction/javascript/JSQuaternion.h"
#include "ocean/interaction/javascript/JSRotation.h"
#include "ocean/interaction/javascript/JSSquareMatrix3.h"
#include "ocean/interaction/javascript/JSSquareMatrix4.h"
#include "ocean/interaction/javascript/JSVector2.h"
#include "ocean/interaction/javascript/JSVector3.h"
#include "ocean/interaction/javascript/JSVector4.h"

#include "ocean/base/String.h"

#include "ocean/scenedescription/Field0D.h"
#include "ocean/scenedescription/Field1D.h"
#include "ocean/scenedescription/SDXNode.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

void JSSceneDescriptionNode::createFunctionTemplate()
{
	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	v8::Local<v8::FunctionTemplate> functionTemplate(v8::FunctionTemplate::New(isolate, constructor<SceneDescription::NodeRef>));
	functionTemplate->SetClassName(newString(objectName(), isolate));

	v8::Local<v8::ObjectTemplate> objectTemplate(v8::Local<v8::ObjectTemplate>::New(isolate, functionTemplate->InstanceTemplate()));
	objectTemplate->SetInternalFieldCount(1);

	objectTemplate->SetAccessor(newString("name", isolate), propertyGetter<NativeType, AI_NAME>, propertySetter<NativeType, AI_NAME>);
	objectTemplate->SetAccessor(newString("type", isolate), propertyGetter<NativeType, AI_TYPE>);

	objectTemplate->Set(newString("field", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_FIELD>));
	objectTemplate->Set(newString("setField", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_FIELD>));

	objectTemplate->Set(newString("hasField", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_HAS_FIELD>));
	objectTemplate->Set(newString("fieldType", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_FIELD_TYPE>));
	objectTemplate->Set(newString("fieldDimension", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_FIELD_DIMENSION>));

	objectTemplate->Set(newString("isValid", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_IS_VALID>));
	objectTemplate->Set(newString("isInvalid", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_IS_INVALID>));

	objectTemplate->Set(newString("parent", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_PARENT>));
	objectTemplate->Set(newString("parents", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_PARENTS>));

	functionTemplate_.Reset(isolate, functionTemplate);
	objectTemplate_.Reset(isolate, objectTemplate);
}

template <>
void JSBase::constructor<SceneDescription::NodeRef>(SceneDescription::NodeRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	if (info.Length() == 0)
	{
		return;
	}

	if (hasValue(info, 0u, thisValue))
	{
		return;
	}
}

template <>
void JSBase::propertyGetter<SceneDescription::NodeRef, JSSceneDescriptionNode::AI_NAME>(SceneDescription::NodeRef& thisValue, v8::Local<v8::String>& /*property*/, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if (thisValue)
	{
		info.GetReturnValue().Set(newString(thisValue->name(), v8::Isolate::GetCurrent()));
		return;
	}

	Log::warning() << "The SceneDescription object is invalid.";
}

template <>
void JSBase::propertyGetter<SceneDescription::NodeRef, JSSceneDescriptionNode::AI_TYPE>(SceneDescription::NodeRef& thisValue, v8::Local<v8::String>& /*property*/, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if (thisValue)
	{
		info.GetReturnValue().Set(newString(thisValue->type(), v8::Isolate::GetCurrent()));
		return;
	}

	Log::warning() << "The SceneDescription object is invalid.";
	info.GetReturnValue().Set(newString("None", v8::Isolate::GetCurrent()));
}

template <>
void JSBase::propertySetter<SceneDescription::NodeRef, JSSceneDescriptionNode::AI_NAME>(SceneDescription::NodeRef& thisValue, v8::Local<v8::String>& /*property*/, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& /*info*/)
{
	if (thisValue)
	{
		std::string name;
		if (isValue(value, name))
		{
			thisValue->setName(name);
			return;
		}
		else
		{
			Log::warning() << "This name property of a SceneDescription object accepts String values only.";
			return;
		}
	}

	Log::warning() << "The SceneDescription object is invalid.";
}

template <>
void JSBase::function<SceneDescription::NodeRef, JSSceneDescriptionNode::FI_FIELD>(SceneDescription::NodeRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	std::string fieldName;
	if (hasValue(info, 0u, fieldName))
	{
		const SceneDescription::SDXNodeRef node(thisValue);
		if (node)
		{
			try
			{
				if (!node->hasAnyField(fieldName))
				{
					Log::error() << "The SceneDescription object does not have a field \"" << fieldName << "\".";
					return;
				}

				const SceneDescription::Field& field = node->anyField(fieldName);

				if (field.is0D())
				{
					switch (field.type())
					{
						case SceneDescription::Field::TYPE_BOOLEAN:
						{
							const SceneDescription::SingleBool& singleBool = SceneDescription::Field::cast<SceneDescription::SingleBool>(field);
							info.GetReturnValue().Set(singleBool.value());
							return;
						}

						case SceneDescription::Field::TYPE_COLOR:
						{
							const SceneDescription::SingleColor& singleColor = SceneDescription::Field::cast<SceneDescription::SingleColor>(field);
							info.GetReturnValue().Set(createObject<JSColor>(singleColor.value(), JSContext::currentContext()));
							return;
						}

						case SceneDescription::Field::TYPE_FLOAT:
						{
							const SceneDescription::SingleFloat& singleFloat = SceneDescription::Field::cast<SceneDescription::SingleFloat>(field);
							info.GetReturnValue().Set(double(singleFloat.value()));
							return;
						}

						case SceneDescription::Field::TYPE_INT:
						{
							const SceneDescription::SingleInt& singleInt = SceneDescription::Field::cast<SceneDescription::SingleInt>(field);
							info.GetReturnValue().Set(singleInt.value());
							return;
						}

						case SceneDescription::Field::TYPE_MATRIX3:
						{
							const SceneDescription::SingleMatrix3& singleMatrix = SceneDescription::Field::cast<SceneDescription::SingleMatrix3>(field);
							info.GetReturnValue().Set(createObject<JSSquareMatrix3>(singleMatrix.value(), JSContext::currentContext()));
							return;
						}

						case SceneDescription::Field::TYPE_MATRIX4:
						{
							const SceneDescription::SingleMatrix4& singleMatrix = SceneDescription::Field::cast<SceneDescription::SingleMatrix4>(field);
							info.GetReturnValue().Set(createObject<JSSquareMatrix4>(singleMatrix.value(), JSContext::currentContext()));
							return;
						}

						case SceneDescription::Field::TYPE_NODE:
						{
							const SceneDescription::SingleNode& singleNode = SceneDescription::Field::cast<SceneDescription::SingleNode>(field);
							info.GetReturnValue().Set(createObject<JSSceneDescriptionNode>(singleNode.value(), JSContext::currentContext()));
							return;
						}

						case SceneDescription::Field::TYPE_ROTATION:
						{
							const SceneDescription::SingleRotation& singleRotation = SceneDescription::Field::cast<SceneDescription::SingleRotation>(field);
							info.GetReturnValue().Set(createObject<JSRotation>(singleRotation.value(), JSContext::currentContext()));
							return;
						}

						case SceneDescription::Field::TYPE_STRING:
						{
							const SceneDescription::SingleString& singleString = SceneDescription::Field::cast<SceneDescription::SingleString>(field);
							info.GetReturnValue().Set(newString(singleString.value(), v8::Isolate::GetCurrent()));
							return;
						}

						case SceneDescription::Field::TYPE_TIME:
						{
							const SceneDescription::SingleTime& singleTime = SceneDescription::Field::cast<SceneDescription::SingleTime>(field);
							info.GetReturnValue().Set(double(singleTime.value()));
							return;
						}

						case SceneDescription::Field::TYPE_VECTOR2:
						{
							const SceneDescription::SingleVector2& singleVector = SceneDescription::Field::cast<SceneDescription::SingleVector2>(field);
							info.GetReturnValue().Set(createObject<JSVector2>(singleVector.value(), JSContext::currentContext()));
							return;
						}

						case SceneDescription::Field::TYPE_VECTOR3:
						{
							const SceneDescription::SingleVector3& singleVector = SceneDescription::Field::cast<SceneDescription::SingleVector3>(field);
							info.GetReturnValue().Set(createObject<JSVector3>(singleVector.value(), JSContext::currentContext()));
							return;
						}

						case SceneDescription::Field::TYPE_VECTOR4:
						{
							const SceneDescription::SingleVector4& singleVector = SceneDescription::Field::cast<SceneDescription::SingleVector4>(field);
							info.GetReturnValue().Set(createObject<JSVector4>(singleVector.value(), JSContext::currentContext()));
							return;
						}

						case SceneDescription::Field::TYPE_INVALID:
						{
							ocean_assert(false && "Invalid type!");
							break;
						}
					}
				}
				else if (field.is1D())
				{
					switch (field.type())
					{
						case SceneDescription::Field::TYPE_BOOLEAN:
						{
							const SceneDescription::MultiBool& multiBool = SceneDescription::Field::cast<SceneDescription::MultiBool>(field);
							info.GetReturnValue().Set(createValues<bool>(multiBool.values(), JSContext::currentContext()));
							return;
						}

						case SceneDescription::Field::TYPE_COLOR:
						{
							const SceneDescription::MultiColor& multiColor = SceneDescription::Field::cast<SceneDescription::MultiColor>(field);
							info.GetReturnValue().Set(createObjects<JSColor>(multiColor.values(), JSContext::currentContext()));
							return;
						}

						case SceneDescription::Field::TYPE_FLOAT:
						{
							const SceneDescription::MultiFloat& multiFloat = SceneDescription::Field::cast<SceneDescription::MultiFloat>(field);
							info.GetReturnValue().Set(createValues<Scalar>(multiFloat.values(), JSContext::currentContext()));
							return;
						}

						case SceneDescription::Field::TYPE_INT:
						{
							const SceneDescription::MultiInt& multiInt = SceneDescription::Field::cast<SceneDescription::MultiInt>(field);
							info.GetReturnValue().Set(createValues<int>(multiInt.values(), JSContext::currentContext()));
							return;
						}

						case SceneDescription::Field::TYPE_MATRIX3:
						{
							const SceneDescription::MultiMatrix3& multiMatrix = SceneDescription::Field::cast<SceneDescription::MultiMatrix3>(field);
							info.GetReturnValue().Set(createObjects<JSSquareMatrix3>(multiMatrix.values(), JSContext::currentContext()));
							return;
						}

						case SceneDescription::Field::TYPE_MATRIX4:
						{
							const SceneDescription::MultiMatrix4& multiMatrix = SceneDescription::Field::cast<SceneDescription::MultiMatrix4>(field);
							info.GetReturnValue().Set(createObjects<JSSquareMatrix4>(multiMatrix.values(), JSContext::currentContext()));
							return;
						}

						case SceneDescription::Field::TYPE_NODE:
						{
							const SceneDescription::MultiNode& multiNode = SceneDescription::Field::cast<SceneDescription::MultiNode>(field);
							info.GetReturnValue().Set(createObjects<JSSceneDescriptionNode>(multiNode.values(), JSContext::currentContext()));
							return;
						}

						case SceneDescription::Field::TYPE_ROTATION:
						{
							const SceneDescription::MultiRotation& multiRotation = SceneDescription::Field::cast<SceneDescription::MultiRotation>(field);
							info.GetReturnValue().Set(createObjects<JSRotation>(multiRotation.values(), JSContext::currentContext()));
							return;
						}

						case SceneDescription::Field::TYPE_STRING:
						{
							const SceneDescription::MultiString& multiString = SceneDescription::Field::cast<SceneDescription::MultiString>(field);
							info.GetReturnValue().Set(createValues<std::string>(multiString.values(), JSContext::currentContext()));
							return;
						}

						case SceneDescription::Field::TYPE_TIME:
						{
							const SceneDescription::MultiTime& multiTime = SceneDescription::Field::cast<SceneDescription::MultiTime>(field);

							static_assert(sizeof(Timestamp) == sizeof(double), "Invalid data type!");

							info.GetReturnValue().Set(createValues<double>((const double*)(multiTime.values().data()), multiTime.values().size(), JSContext::currentContext()));
							return;
						}

						case SceneDescription::Field::TYPE_VECTOR2:
						{
							const SceneDescription::MultiVector2& multiVector = SceneDescription::Field::cast<SceneDescription::MultiVector2>(field);
							info.GetReturnValue().Set(createObjects<JSVector2>(multiVector.values(), JSContext::currentContext()));
							return;
						}

						case SceneDescription::Field::TYPE_VECTOR3:
						{
							const SceneDescription::MultiVector3& multiVector = SceneDescription::Field::cast<SceneDescription::MultiVector3>(field);
							info.GetReturnValue().Set(createObjects<JSVector3>(multiVector.values(), JSContext::currentContext()));
							return;
						}

						case SceneDescription::Field::TYPE_VECTOR4:
						{
							const SceneDescription::MultiVector4& multiVector = SceneDescription::Field::cast<SceneDescription::MultiVector4>(field);
							info.GetReturnValue().Set(createObjects<JSVector4>(multiVector.values(), JSContext::currentContext()));
							return;
						}

						case SceneDescription::Field::TYPE_INVALID:
						{
							ocean_assert(false && "Invalid type!");
							break;
						}
					}
				}
			}
			catch(const Exception& exception)
			{
				Log::error() << "SceneDescriptionNode::setField() failed: " << exception.what();
			}
		}
	}
	else
	{
		Log::error() << "SceneDescription::hasField() accepts a String value as first parameter only.";
		return;
	}

	Log::error() << "The SceneDescription object is invalid or does not have a field with name \"" << fieldName << "\".";
}

template <>
void JSBase::function<SceneDescription::NodeRef, JSSceneDescriptionNode::FI_SET_FIELD>(SceneDescription::NodeRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	std::string fieldName;
	if (hasValue(info, 0u, fieldName))
	{
		const SceneDescription::SDXNodeRef node(thisValue);

		if (node)
		{
			try
			{
				if (!node->hasAnyField(fieldName))
				{
					Log::error() << "The SceneDescription object does not have a field \"" << fieldName << "\".";
					return;
				}

				Timestamp timestamp;

				double timestampValue;
				if (hasValue(info, 2u, timestampValue))
				{
					timestamp = Timestamp(timestampValue);
				}
				else
				{
					timestamp.toNow();
				}

				const SceneDescription::Field& field = node->anyField(fieldName);

				if (field.is0D())
				{
					switch (field.type())
					{
						case SceneDescription::Field::TYPE_BOOLEAN:
						{
							bool value;
							if (hasValue(info, 1u, value))
							{
								info.GetReturnValue().Set(node->setAnyField(fieldName, SceneDescription::SingleBool(value, timestamp)));
								return;
							}

							Log::warning() << "SceneDescriptionNode::setField() of Boolean field accepts a boolean value as second parameter only.";
							info.GetReturnValue().Set(false);
							return;
						}

						case SceneDescription::Field::TYPE_COLOR:
						{
							RGBAColor value;
							if (hasValue(info, 1u, value))
							{
								info.GetReturnValue().Set(node->setAnyField(fieldName, SceneDescription::SingleColor(value, timestamp)));
								return;
							}

							Log::warning() << "SceneDescriptionNode::setField() of a color field accepts a Color object as second parameter only.";
							info.GetReturnValue().Set(false);
							return;
						}

						case SceneDescription::Field::TYPE_FLOAT:
						{
							Scalar value;
							if (hasValue(info, 1u, value))
							{
								info.GetReturnValue().Set(node->setAnyField(fieldName, SceneDescription::SingleFloat(value, timestamp)));
								return;
							}

							Log::warning() << "SceneDescriptionNode::setField() of a float field accepts a Number value as second parameter only.";
							info.GetReturnValue().Set(false);
							return;
						}

						case SceneDescription::Field::TYPE_INT:
						{
							int value;
							if (hasValue(info, 1u, value))
							{
								info.GetReturnValue().Set(node->setAnyField(fieldName, SceneDescription::SingleInt(value, timestamp)));
								return;
							}

							Log::warning() << "SceneDescriptionNode::setField() of an integer field accepts an Integer value as second parameter only.";
							info.GetReturnValue().Set(false);
							return;
						}

						case SceneDescription::Field::TYPE_MATRIX3:
						{
							SquareMatrix3 value;
							if (hasValue(info, 1u, value))
							{
								info.GetReturnValue().Set(node->setAnyField(fieldName, SceneDescription::SingleMatrix3(value, timestamp)));
								return;
							}

							Log::warning() << "SceneDescriptionNode::setField() of a 3x3 square matrix field accepts a SquareMatrix3 object as second parameter only.";
							info.GetReturnValue().Set(false);
							return;
						}

						case SceneDescription::Field::TYPE_MATRIX4:
						{
							SquareMatrix4 value;
							if (hasValue(info, 1u, value))
							{
								info.GetReturnValue().Set(node->setAnyField(fieldName, SceneDescription::SingleMatrix4(value, timestamp)));
								return;
							}

							Log::warning() << "SceneDescriptionNode::setField() of a 4x4 square matrix field accepts a SquareMatrix4 object as second parameter only.";
							info.GetReturnValue().Set(false);
							return;
						}

						case SceneDescription::Field::TYPE_NODE:
						{
							SceneDescription::NodeRef value;
							if (hasValue(info, 1u, value))
							{
								info.GetReturnValue().Set(node->setAnyField(fieldName, SceneDescription::SingleNode(value, timestamp)));
								return;
							}

							Log::warning() << "SceneDescriptionNode::setField() of a scene description object field accepts a SceneDescriptionNode object as second parameter only.";
							info.GetReturnValue().Set(false);
							return;
						}

						case SceneDescription::Field::TYPE_ROTATION:
						{
							Rotation rotationValue;
							if (hasValue(info, 1u, rotationValue))
							{
								info.GetReturnValue().Set(node->setAnyField(fieldName, SceneDescription::SingleRotation(rotationValue, timestamp)));
								return;
							}

							Quaternion quaternionValue;
							if (hasValue(info, 1u, quaternionValue))
							{
								info.GetReturnValue().Set(node->setAnyField(fieldName, SceneDescription::SingleRotation(Rotation(quaternionValue), timestamp)));
								return;
							}

							SquareMatrix3 matrixValue;
							if (hasValue(info, 1u, matrixValue))
							{
								info.GetReturnValue().Set(node->setField(fieldName, SceneDescription::SingleRotation(Rotation(matrixValue), timestamp)));
								return;
							}

							Euler eulerValue;
							if (hasValue(info, 1u, eulerValue))
							{
								info.GetReturnValue().Set(node->setAnyField(fieldName, SceneDescription::SingleRotation(Rotation(eulerValue), timestamp)));
								return;
							}

							Log::warning() << "SceneDescriptionNode::setField() of a rotation field accepts a Rotation, Quaternion, SquareMatrix3, or Euler value as second parameter only.";
							info.GetReturnValue().Set(false);
							return;
						}

						case SceneDescription::Field::TYPE_STRING:
						{
							std::string value;
							if (hasValue(info, 1u, value))
							{
								info.GetReturnValue().Set(node->setAnyField(fieldName, SceneDescription::SingleString(value, timestamp)));
								return;
							}

							Log::warning() << "SceneDescriptionNode::setField() of a string field accepts a String value as second parameter only.";
							info.GetReturnValue().Set(false);
							return;
						}

						case SceneDescription::Field::TYPE_TIME:
						{
							double value;
							if (hasValue(info, 1u, value))
							{
								info.GetReturnValue().Set(node->setAnyField(fieldName, SceneDescription::SingleTime(Timestamp(value), timestamp)));
								return;
							}

							Log::warning() << "SceneDescriptionNode::setField() of a time field accepts a Number value as second parameter only.";
							info.GetReturnValue().Set(false);
							return;
						}

						case SceneDescription::Field::TYPE_VECTOR2:
						{
							Vector2 value;
							if (hasValue(info, 1u, value))
							{
								info.GetReturnValue().Set(node->setAnyField(fieldName, SceneDescription::SingleVector2(value, timestamp)));
								return;
							}

							Log::warning() << "SceneDescriptionNode::setField() of a 2D vector field accepts a Vector2 object as second parameter only.";
							info.GetReturnValue().Set(false);
							return;
						}

						case SceneDescription::Field::TYPE_VECTOR3:
						{
							Vector3 value3;
							if (hasValue(info, 1u, value3))
							{
								info.GetReturnValue().Set(node->setAnyField(fieldName, SceneDescription::SingleVector3(value3, timestamp)));
								return;
							}

							Vector4 value4;
							if (hasValue(info, 1u, value4))
							{
								info.GetReturnValue().Set(node->setAnyField(fieldName, SceneDescription::SingleVector3(value4.xyz(), timestamp)));
								return;
							}

							Log::warning() << "SceneDescriptionNode::setField() of a 3D vector field accepts a Vector3 or Vector4 object as second parameter only.";
							info.GetReturnValue().Set(false);
							return;
						}

						case SceneDescription::Field::TYPE_VECTOR4:
						{
							Vector4 value;
							if (hasValue(info, 1u, value))
							{
								info.GetReturnValue().Set(node->setAnyField(fieldName, SceneDescription::SingleVector4(value, timestamp)));
								return;
							}

							Log::warning() << "SceneDescriptionNode::setField() of a 4D vector field accepts a Vector4 value as second parameter only.";
							info.GetReturnValue().Set(false);
							return;
						}

						case SceneDescription::Field::TYPE_INVALID:
						{
							ocean_assert(false && "Invalid type!");
							break;
						}
					}

					ocean_assert(false && "Missing implementation.");
					Log::warning() << "Unsupported field type of a SceneDescription node.";
				}
				else if (field.is1D())
				{
					switch (field.type())
					{
						case SceneDescription::Field::TYPE_BOOLEAN:
						{
							SceneDescription::MultiBool::Values values;

							if (hasValue(info, 1u, values))
							{
								info.GetReturnValue().Set(node->setAnyField(fieldName, SceneDescription::MultiBool(values, timestamp)));
								return;
							}

							info.GetReturnValue().Set(false);
							return;
						}

						case SceneDescription::Field::TYPE_COLOR:
						{
							SceneDescription::MultiColor::Values values;

							if (hasValue(info, 1u, values))
							{
								info.GetReturnValue().Set(node->setAnyField(fieldName, SceneDescription::MultiColor(values, timestamp)));
								return;
							}

							info.GetReturnValue().Set(false);
							return;
						}

						case SceneDescription::Field::TYPE_FLOAT:
						{
							SceneDescription::MultiFloat::Values values;

							if (hasValue(info, 1u, values))
							{
								info.GetReturnValue().Set(node->setAnyField(fieldName, SceneDescription::MultiFloat(values, timestamp)));
								return;
							}

							info.GetReturnValue().Set(false);
							return;
						}

						case SceneDescription::Field::TYPE_INT:
						{
							SceneDescription::MultiInt::Values values;

							if (hasValue(info, 1u, values))
							{
								info.GetReturnValue().Set(node->setAnyField(fieldName, SceneDescription::MultiInt(values, timestamp)));
								return;
							}

							info.GetReturnValue().Set(false);
							return;
						}

						case SceneDescription::Field::TYPE_MATRIX3:
						{
							SceneDescription::MultiMatrix3::Values values;

							if (hasValue(info, 1u, values))
							{
								info.GetReturnValue().Set(node->setAnyField(fieldName, SceneDescription::MultiMatrix3(values, timestamp)));
								return;
							}

							info.GetReturnValue().Set(false);
							return;
						}

						case SceneDescription::Field::TYPE_MATRIX4:
						{
							SceneDescription::MultiMatrix4::Values values;

							if (hasValue(info, 1u, values))
							{
								info.GetReturnValue().Set(node->setAnyField(fieldName, SceneDescription::MultiMatrix4(values, timestamp)));
								return;
							}

							info.GetReturnValue().Set(false);
							return;
						}

						case SceneDescription::Field::TYPE_NODE:
						{
							SceneDescription::MultiNode::Values nodes;

							if (hasValue(info, 1u, nodes))
							{
								info.GetReturnValue().Set(node->setAnyField(fieldName, SceneDescription::MultiNode(nodes, timestamp)));
								return;
							}

							info.GetReturnValue().Set(false);
							return;
						}

						case SceneDescription::Field::TYPE_ROTATION:
						{
							SceneDescription::MultiRotation::Values values;

							if (hasValue(info, 1u, values))
							{
								info.GetReturnValue().Set(node->setAnyField(fieldName, SceneDescription::MultiRotation(values, timestamp)));
								return;
							}

							info.GetReturnValue().Set(false);
							return;
						}

						case SceneDescription::Field::TYPE_STRING:
						{
							SceneDescription::MultiString::Values values;

							if (hasValue(info, 1u, values))
							{
								info.GetReturnValue().Set(node->setAnyField(fieldName, SceneDescription::MultiString(values, timestamp)));
								return;
							}

							info.GetReturnValue().Set(false);
							return;
						}

						case SceneDescription::Field::TYPE_TIME:
						{
							SceneDescription::MultiTime::Values values;

							if (hasValue(info, 1u, values))
							{
								info.GetReturnValue().Set(node->setAnyField(fieldName, SceneDescription::MultiTime(values, timestamp)));
								return;
							}

							info.GetReturnValue().Set(false);
							return;
						}

						case SceneDescription::Field::TYPE_VECTOR2:
						{
							SceneDescription::MultiVector2::Values values;

							if (hasValue(info, 1u, values))
							{
								info.GetReturnValue().Set(node->setAnyField(fieldName, SceneDescription::MultiVector2(values, timestamp)));
								return;
							}

							info.GetReturnValue().Set(false);
							return;
						}

						case SceneDescription::Field::TYPE_VECTOR3:
						{
							SceneDescription::MultiVector3::Values values;

							if (hasValue(info, 1u, values))
							{
								info.GetReturnValue().Set(node->setAnyField(fieldName, SceneDescription::MultiVector3(values, timestamp)));
								return;
							}

							info.GetReturnValue().Set(false);
							return;
						}

						case SceneDescription::Field::TYPE_VECTOR4:
						{
							SceneDescription::MultiVector4::Values values;

							if (hasValue(info, 1u, values))
							{
								info.GetReturnValue().Set(node->setAnyField(fieldName, SceneDescription::MultiVector4(values, timestamp)));
								return;
							}

							info.GetReturnValue().Set(false);
							return;
						}

						case SceneDescription::Field::TYPE_INVALID:
						{
							ocean_assert(false && "Invalid type!");
							break;
						}
					}

					ocean_assert(false && "Missing implementation.");
					Log::warning() << "Unsupported field type of a SceneDescription node.";
				}
			}
			catch(const Exception& exception)
			{
				Log::error() << "SceneDescriptionNode::setField() failed: " << exception.what();
			}
		}
	}
	else
	{
		Log::error() << "SceneDescriptionNode::setField() needs a String as first parameter.";
	}

	return info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<SceneDescription::NodeRef, JSSceneDescriptionNode::FI_IS_VALID>(SceneDescription::NodeRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(!thisValue.isNull());
}

template <>
void JSBase::function<SceneDescription::NodeRef, JSSceneDescriptionNode::FI_IS_INVALID>(SceneDescription::NodeRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(thisValue.isNull());
}

template <>
void JSBase::function<SceneDescription::NodeRef, JSSceneDescriptionNode::FI_HAS_FIELD>(SceneDescription::NodeRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	std::string fieldName;
	if (hasValue(info, 0u, fieldName))
	{
		const SceneDescription::SDXNodeRef node(thisValue);
		if (node)
		{
			info.GetReturnValue().Set(node->hasAnyField(fieldName));
			return;
		}
	}
	else
	{
		Log::error() << "SceneDescription::hasField() accepts a String value as first parameter only.";
	}

	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<SceneDescription::NodeRef, JSSceneDescriptionNode::FI_FIELD_TYPE>(SceneDescription::NodeRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	std::string type = "INVALID";

	std::string fieldName;
	if (hasValue(info, 0u, fieldName))
	{
		const SceneDescription::SDXNodeRef node(thisValue);

		if (node)
		{
			if (node->hasAnyField(fieldName))
			{
				switch (node->fieldType(fieldName))
				{
					case SceneDescription::Field::TYPE_INVALID:
						type = "INVALID";
						break;

					case SceneDescription::Field::TYPE_BOOLEAN:
						type = "BOOLEAN";
						break;

					case SceneDescription::Field::TYPE_COLOR:
						type = "COLOR";
						break;

					case SceneDescription::Field::TYPE_FLOAT:
						type = "NUMBER";
						break;

					case SceneDescription::Field::TYPE_INT:
						type = "INTEGER";
						break;

					case SceneDescription::Field::TYPE_MATRIX3:
						type = "MATRIX3";
						break;

					case SceneDescription::Field::TYPE_MATRIX4:
						type = "MATRIX4";
						break;

					case SceneDescription::Field::TYPE_NODE:
						type = "SCNEDESCRIPTIONNODE";
						break;

					case SceneDescription::Field::TYPE_ROTATION:
						type = "ROTATION";
						break;

					case SceneDescription::Field::TYPE_STRING:
						type = "STRING";
						break;

					case SceneDescription::Field::TYPE_TIME:
						type = "TIME";
						break;

					case SceneDescription::Field::TYPE_VECTOR2:
						type = "VECTOR2";
						break;

					case SceneDescription::Field::TYPE_VECTOR3:
						type = "VECTOR3";
						break;

					case SceneDescription::Field::TYPE_VECTOR4:
						type = "VECTOR4";
						break;

					default:
						ocean_assert(false && "Invalid type!");
						break;
				}
			}
			else
			{
				Log::error() << "The SceneDescription node does not have a field \"" << fieldName << "\".";
			}
		}
	}
	else
	{
		Log::error() << "SceneDescription::fieldType() accepts a String value as first parameter only.";
	}

	info.GetReturnValue().Set(newString(type, v8::Isolate::GetCurrent()));
}

template <>
void JSBase::function<SceneDescription::NodeRef, JSSceneDescriptionNode::FI_FIELD_DIMENSION>(SceneDescription::NodeRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	std::string fieldName;
	if (hasValue(info, 0u, fieldName))
	{
		const SceneDescription::SDXNodeRef node(thisValue);

		if (node)
		{
			if (!node->hasAnyField(fieldName))
			{
				Log::error() << "The SceneDescription object does not have a field \"" << fieldName << "\".";
				info.GetReturnValue().Set(-1);
				return;
			}

			info.GetReturnValue().Set(node->fieldDimension(fieldName));
			return;
		}
	}
	else
	{
		Log::error() << "SceneDescription::fieldDimension() accepts a String value as first parameter only.";
	}

	info.GetReturnValue().Set(-1);
}

template <>
void JSBase::function<SceneDescription::NodeRef, JSSceneDescriptionNode::FI_PARENT>(SceneDescription::NodeRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const SceneDescription::SDXNodeRef node(thisValue);
	if (node)
	{
		const SceneDescription::NodeRefs parents(node->parentNodes());

		if (!parents.empty())
		{
			info.GetReturnValue().Set(createObject<JSSceneDescriptionNode>(parents.front(), JSContext::currentContext()));
			return;
		}
	}
	else
	{
		Log::warning() << "The SceneDescription object does not support the 'parent()` function.";
	}

	info.GetReturnValue().Set(createObject<JSSceneDescriptionNode>(SceneDescription::NodeRef(), JSContext::currentContext()));
}

template <>
void JSBase::function<SceneDescription::NodeRef, JSSceneDescriptionNode::FI_PARENTS>(SceneDescription::NodeRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const SceneDescription::SDXNodeRef node(thisValue);
	if (node)
	{
		info.GetReturnValue().Set(createObjects<JSSceneDescriptionNode>(node->parentNodes(), JSContext::currentContext()));
		return;
	}
	else
	{
		Log::warning() << "The SceneDescription object does not support the 'parents()` function.";
	}

	info.GetReturnValue().Set(createObjects<JSSceneDescriptionNode>(SceneDescription::NodeRefs(), JSContext::currentContext()));
}

}

}

}
