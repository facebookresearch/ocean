/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/interaction/javascript/JSRenderingObject.h"
#include "ocean/interaction/javascript/JSColor.h"
#include "ocean/interaction/javascript/JSHomogenousMatrix4.h"
#include "ocean/interaction/javascript/JSMediaObject.h"
#include "ocean/interaction/javascript/JSQuaternion.h"
#include "ocean/interaction/javascript/JSVector2.h"
#include "ocean/interaction/javascript/JSVector3.h"

#include "ocean/base/String.h"

#include "ocean/media/Manager.h"

#include "ocean/rendering/AttributeSet.h"
#include "ocean/rendering/Box.h"
#include "ocean/rendering/Cone.h"
#include "ocean/rendering/Cylinder.h"
#include "ocean/rendering/DirectionalLight.h"
#include "ocean/rendering/Geometry.h"
#include "ocean/rendering/Group.h"
#include "ocean/rendering/LightSource.h"
#include "ocean/rendering/LOD.h"
#include "ocean/rendering/Material.h"
#include "ocean/rendering/MediaTexture2D.h"
#include "ocean/rendering/PointLight.h"
#include "ocean/rendering/PerspectiveView.h"
#include "ocean/rendering/PrimitiveAttribute.h"
#include "ocean/rendering/Quads.h"
#include "ocean/rendering/QuadStrips.h"
#include "ocean/rendering/ShaderProgram.h"
#include "ocean/rendering/Sphere.h"
#include "ocean/rendering/SpotLight.h"
#include "ocean/rendering/StereoView.h"
#include "ocean/rendering/StripPrimitive.h"
#include "ocean/rendering/Switch.h"
#include "ocean/rendering/Texture.h"
#include "ocean/rendering/Transform.h"
#include "ocean/rendering/Triangles.h"
#include "ocean/rendering/UndistortedBackground.h"
#include "ocean/rendering/VertexSet.h"
#include "ocean/rendering/View.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

void JSRenderingObject::createFunctionTemplate()
{
	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	v8::Local<v8::FunctionTemplate> functionTemplate(v8::FunctionTemplate::New(isolate, constructor<Rendering::ObjectRef>));
	functionTemplate->SetClassName(newString(objectName(), isolate));

	v8::Local<v8::ObjectTemplate> objectTemplate(v8::Local<v8::ObjectTemplate>::New(isolate, functionTemplate->InstanceTemplate()));
	objectTemplate->SetInternalFieldCount(1);

	objectTemplate->SetAccessor(newString("name", isolate), propertyGetter<NativeType, AI_NAME>, propertySetter<NativeType, AI_NAME>);
	objectTemplate->SetAccessor(newString("type", isolate), propertyGetter<NativeType, AI_TYPE>);

	objectTemplate->Set(newString("isValid", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_IS_VALID>));
	objectTemplate->Set(newString("isInvalid", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_IS_INVALID>));

	objectTemplate->Set(newString("add", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_ADD>));
	objectTemplate->Set(newString("remove", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_REMOVE>));

	objectTemplate->Set(newString("parent", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_PARENT>));
	objectTemplate->Set(newString("parents", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_PARENTS>));

	objectTemplate->Set(newString("activeNode", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_ACTIVE_NODE>));
	objectTemplate->Set(newString("ambientColor", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_AMBIENT_COLOR>));
	objectTemplate->Set(newString("aspectRatio", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_ASPECT_RATIO>));
	objectTemplate->Set(newString("attenuation", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_ATTENUATION>));
	objectTemplate->Set(newString("attribute", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_ATTRIBUTE>));
	objectTemplate->Set(newString("background", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_BACKGROUND>));
	objectTemplate->Set(newString("backgroundColor", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_BACKGROUND_COLOR>));
	objectTemplate->Set(newString("child", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_CHILD>));
	objectTemplate->Set(newString("colors", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_COLORS>));
	objectTemplate->Set(newString("coneAngle", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_CONE_ANGLE>));
	objectTemplate->Set(newString("cullingMode", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_CULLING_MODE>));
	objectTemplate->Set(newString("depth", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_DEPTH>));
	objectTemplate->Set(newString("direction", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_DIRECTION>));
	objectTemplate->Set(newString("diffuseColor", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_DIFFUSE_COLOR>));
	objectTemplate->Set(newString("emissiveColor", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_EMISSIVE_COLOR>));
	objectTemplate->Set(newString("enabled", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_ENABLED>));
	objectTemplate->Set(newString("environmentMode", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_ENVIRONMENT_MODE>));
	objectTemplate->Set(newString("existParameter", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_EXIST_PARAMETER>));
	objectTemplate->Set(newString("faceMode", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_FACE_MODE>));
	objectTemplate->Set(newString("farDistance", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_FAR_DISTANCE>));
	objectTemplate->Set(newString("height", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_HEIGHT>));
	objectTemplate->Set(newString("intensity", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_INTENSITY>));
	objectTemplate->Set(newString("isCompiled", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_IS_COMPILED>));
	objectTemplate->Set(newString("magnificationFilterMode", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_MAGNIFICATION_FILTER_MODE>));
	objectTemplate->Set(newString("medium", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_MEDIUM>));
	objectTemplate->Set(newString("minificationFilterMode", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_MINIFICATION_FILTER_MODE>));
	objectTemplate->Set(newString("nearDistance", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_NEAR_DISTANCE>));
	objectTemplate->Set(newString("normals", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_NORMALS>));
	objectTemplate->Set(newString("numberAttributes", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_NUMBER_ATTRIBUTES>));
	objectTemplate->Set(newString("numberChildren", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_NUMBER_CHILDREN>));
	objectTemplate->Set(newString("parameter", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_PARAMETER>));
	objectTemplate->Set(newString("parameterElements", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_PARAMETER_ELEMENTS>));
	objectTemplate->Set(newString("parameterType", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_PARAMETER_TYPE>));
	objectTemplate->Set(newString("position", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_POSITION>));
	objectTemplate->Set(newString("radius", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_RADIUS>));
	objectTemplate->Set(newString("reflectivity", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_REFLECTIVITY>));
	objectTemplate->Set(newString("rotation", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_ROTATION>));
	objectTemplate->Set(newString("scale", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SCALE>));
	objectTemplate->Set(newString("size", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SIZE>));
	objectTemplate->Set(newString("specularColor", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SPECULAR_COLOR>));
	objectTemplate->Set(newString("specularExponent", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SPECULAR_EXPONENT>));
	objectTemplate->Set(newString("spotExponent", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SPOT_EXPONENT>));
	objectTemplate->Set(newString("textureCoordinates", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_TEXTURE_COORDINATES>));
	objectTemplate->Set(newString("translation", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_TRANSLATION>));
	objectTemplate->Set(newString("transformation", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_TRANSFORMATION>));
	objectTemplate->Set(newString("transparency", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_TRANSPARENCY>));
	objectTemplate->Set(newString("headlight", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_USE_HEADLIGHT>));
	objectTemplate->Set(newString("vertices", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_VERTICES>));
	objectTemplate->Set(newString("vertexSet", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_VERTEX_SET>));
	objectTemplate->Set(newString("visible", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_VISIBLE>));
	objectTemplate->Set(newString("width", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_WIDTH>));
	objectTemplate->Set(newString("worldTransformation", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_WORLD_TRANSFORMATION>));

	objectTemplate->Set(newString("setActiveNode", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_ACTIVE_NODE>));
	objectTemplate->Set(newString("setAmbientColor", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_AMBIENT_COLOR>));
	objectTemplate->Set(newString("setAspectRatio", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_ASPECT_RATIO>));
	objectTemplate->Set(newString("setAttenuation", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_ATTENUATION>));
	objectTemplate->Set(newString("addBackground", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_ADD_BACKGROUND>));
	objectTemplate->Set(newString("removeBackground", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_REMOVE_BACKGROUND>));
	objectTemplate->Set(newString("setBackgroundColor", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_BACKGROUND_COLOR>));
	objectTemplate->Set(newString("setColors", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_COLORS>));
	objectTemplate->Set(newString("setConeAngle", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_CONE_ANGLE>));
	objectTemplate->Set(newString("setCode", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_CODE>));
	objectTemplate->Set(newString("setCodeFile", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_CODE_FILE>));
	objectTemplate->Set(newString("setCullingMode", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_CULLING_MODE>));
	objectTemplate->Set(newString("setDepth", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_DEPTH>));
	objectTemplate->Set(newString("setDirection", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_DIRECTION>));
	objectTemplate->Set(newString("setDiffuseColor", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_DIFFUSE_COLOR>));
	objectTemplate->Set(newString("setEmissiveColor", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_EMISSIVE_COLOR>));
	objectTemplate->Set(newString("setEnabled", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_ENABLED>));
	objectTemplate->Set(newString("setEnvironmentMode", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_ENVIRONMENT_MODE>));
	objectTemplate->Set(newString("setFaceMode", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_FACE_MODE>));
	objectTemplate->Set(newString("setFarDistance", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_FAR_DISTANCE>));
	objectTemplate->Set(newString("setUseHeadlight", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_USE_HEADLIGHT>));
	objectTemplate->Set(newString("setHeight", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_HEIGHT>));
	objectTemplate->Set(newString("setIntensity", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_INTENSITY>));
	objectTemplate->Set(newString("setLeftProjectionMatrix", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_LEFT_PROJECTION_MATRIX>));
	objectTemplate->Set(newString("setLeftTransformation", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_LEFT_TRANSFORMATION>));
	objectTemplate->Set(newString("setMagnificationFilterMode", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_MAGNIFICATION_FILTER_MODE>));
	objectTemplate->Set(newString("setMedium", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_MEDIUM>));
	objectTemplate->Set(newString("setMinificationFilterMode", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_MINIFICATION_FILTER_MODE>));
	objectTemplate->Set(newString("setNearDistance", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_NEAR_DISTANCE>));
	objectTemplate->Set(newString("setNormals", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_NORMALS>));
	objectTemplate->Set(newString("setParameter", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_PARAMETER>));
	objectTemplate->Set(newString("setPosition", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_POSITION>));
	objectTemplate->Set(newString("setProjectionMatrix", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_PROJECTION_MATRIX>));
	objectTemplate->Set(newString("setRadius", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_RADIUS>));
	objectTemplate->Set(newString("setReflectivity", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_REFLECTIVITY>));
	objectTemplate->Set(newString("setRightProjectionMatrix", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_RIGHT_PROJECTION_MATRIX>));
	objectTemplate->Set(newString("setRightTransformation", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_RIGHT_TRANSFORMATION>));
	objectTemplate->Set(newString("setRotation", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_ROTATION>));
	objectTemplate->Set(newString("setSampler", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_SAMPLER>));
	objectTemplate->Set(newString("setSize", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_SIZE>));
	objectTemplate->Set(newString("setSpecularColor", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_SPECULAR_COLOR>));
	objectTemplate->Set(newString("setSpecularExponent", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_SPECULAR_EXPONENT>));
	objectTemplate->Set(newString("setSpotExponent", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_SPOT_EXPONENT>));
	objectTemplate->Set(newString("setTextureCoordinates", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_TEXTURE_COORDINATES>));
	objectTemplate->Set(newString("setTranslation", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_TRANSLATION>));
	objectTemplate->Set(newString("setTransformation", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_TRANSFORMATION>));
	objectTemplate->Set(newString("setTransparency", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_TRANSPARENCY>));
	objectTemplate->Set(newString("setVertices", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_VERTICES>));
	objectTemplate->Set(newString("setVertexSet", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_VERTEX_SET>));
	objectTemplate->Set(newString("setVisible", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_VISIBLE>));
	objectTemplate->Set(newString("setWidth", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_WIDTH>));

	functionTemplate_.Reset(isolate, functionTemplate);
	objectTemplate_.Reset(isolate, objectTemplate);
}

template <>
void JSBase::constructor<Rendering::ObjectRef>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
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
void JSBase::propertyGetter<Rendering::ObjectRef, JSRenderingObject::AI_NAME>(Rendering::ObjectRef& thisValue, v8::Local<v8::String>& /*property*/, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if (thisValue.isNull())
	{
		Log::warning() << "The rendering object does not hold any valid reference.";
	}
	else
	{
		info.GetReturnValue().Set(newString(thisValue->name(), v8::Isolate::GetCurrent()));
	}
}

template <>
void JSBase::propertyGetter<Rendering::ObjectRef, JSRenderingObject::AI_TYPE>(Rendering::ObjectRef& thisValue, v8::Local<v8::String>& /*property*/, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if (thisValue.isNull())
	{
		Log::warning() << "The rendering object does not hold any valid reference.";
		return;
	}

	std::string typeString = "None";

	if (thisValue)
	{
		switch (thisValue->type())
		{
			case Rendering::Object::TYPE_UNKNOWN:
				ocean_assert(false && "Unknown type!");
				typeString = "Unknown";
				break;

			case Rendering::Object::TYPE_ABSOLUTE_TRANSFORM:
				typeString = "AbsoluteTransform";
				break;

			case Rendering::Object::TYPE_ATTRIBUTE:
				ocean_assert(false && "This should never happen!");
				break;

			case Rendering::Object::TYPE_ATTRIBUTE_SET:
				typeString = "AttributeSet";
				break;

			case Rendering::Object::TYPE_BACKGROUND:
				ocean_assert(false && "This should never happen!");
				break;

			case Rendering::Object::TYPE_BILLBOARD:
				typeString = "Billboard";
				break;

			case Rendering::Object::TYPE_BITMAP_FRAMEBUFFER:
				typeString = "BitmapFramebuffer";
				break;

			case Rendering::Object::TYPE_BLEND_ATTRIBUTE:
				typeString = "BlendAttribute";
				break;

			case Rendering::Object::TYPE_BOX:
				typeString = "Box";
				break;

			case Rendering::Object::TYPE_CONE:
				typeString = "Cone";
				break;

			case Rendering::Object::TYPE_CYLINDER:
				typeString = "Cylinder";
				break;

			case Rendering::Object::TYPE_DEPTH_ATTRIBUTE:
				typeString = "DepthAttribute";
				break;

			case Rendering::Object::TYPE_DIRECTIONAL_LIGHT:
				typeString = "DirectionalLight";
				break;

			case Rendering::Object::TYPE_FRAMEBUFFER:
				ocean_assert(false && "This should never happen!");
				break;

			case Rendering::Object::TYPE_FRAME_TEXTURE_2D:
				typeString = "FrameTexture2D";
				break;

			case Rendering::Object::TYPE_GEOMETRY:
				typeString = "Geometry";
				break;

			case Rendering::Object::TYPE_GROUP:
				typeString = "Group";
				break;

			case Rendering::Object::TYPE_INDEPENDENT_PRIMITIVE:
				ocean_assert(false && "This should never happen!");
				break;

			case Rendering::Object::TYPE_LIGHT_SOURCE:
				ocean_assert(false && "This should never happen!");
				break;

			case Rendering::Object::TYPE_LINES:
				typeString = "Lines";
				break;

			case Rendering::Object::TYPE_LINE_STRIPS:
				typeString = "LineStrips";
				break;

			case Rendering::Object::TYPE_LOD:
				typeString = "LOD";
				break;

			case Rendering::Object::TYPE_MATERIAL:
				typeString = "Material";
				break;

			case Rendering::Object::TYPE_MEDIA_TEXTURE_2D:
				typeString = "MediaTexture2D";
				break;

			case Rendering::Object::TYPE_NODE:
				ocean_assert(false && "This should never happen!");
				break;

			case Rendering::Object::TYPE_OBJECT:
				ocean_assert(false && "This should never happen!");
				break;

			case Rendering::Object::TYPE_PARALLEL_VIEW:
				typeString = "ParallelView";
				break;

			case Rendering::Object::TYPE_PERSPECTIVE_VIEW:
				typeString = "PerspectiveView";
				break;

			case Rendering::Object::TYPE_PHANTOM_ATTRIBUTE:
				typeString = "PhantomAttribute";
				break;

			case Rendering::Object::TYPE_POINTS:
				typeString = "Points";
				break;

			case Rendering::Object::TYPE_POINT_LIGHT:
				typeString = "PointLight";
				break;

			case Rendering::Object::TYPE_PRIMITIVE:
				ocean_assert(false && "This should never happen!");
				break;

			case Rendering::Object::TYPE_PRIMITIVE_ATTRIBUTE:
				typeString = "PrimitiveAttribute";
				break;

			case Rendering::Object::TYPE_QUAD_STRIPS:
				typeString = "QuadStrips";
				break;

			case Rendering::Object::TYPE_QUADS:
				typeString = "Quads";
				break;

			case Rendering::Object::TYPE_RENDERABLE:
				ocean_assert(false && "This should never happen!");
				break;

			case Rendering::Object::TYPE_SCENE:
				typeString = "Scene";
				break;

			case Rendering::Object::TYPE_SKY_BACKGROUND:
				typeString = "SkyBackground";
				break;

			case Rendering::Object::TYPE_SHADER_PROGRAM:
				typeString = "ShaderProgram";
				break;

			case Rendering::Object::TYPE_SHAPE:
				ocean_assert(false && "This should never happen!");
				break;

			case Rendering::Object::TYPE_SPHERE:
				typeString = "Sphere";
				break;

			case Rendering::Object::TYPE_SPOT_LIGHT:
				typeString = "SpotLight";
				break;

			case Rendering::Object::TYPE_STEREO_ATTRIBUTE:
				typeString = "StereoAttribute";
				break;

			case Rendering::Object::TYPE_STEREO_VIEW:
				typeString = "StereoView";
				break;

			case Rendering::Object::TYPE_STRIP_PRIMITIVE:
				ocean_assert(false && "This should never happen!");
				break;

			case Rendering::Object::TYPE_SWITCH:
				typeString = "Switch";
				break;

			case Rendering::Object::TYPE_TEXT:
				typeString = "Text";
				break;

			case Rendering::Object::TYPE_TEXTURE:
				ocean_assert(false && "This should never happen!");
				break;

			case Rendering::Object::TYPE_TEXTURE_2D:
				ocean_assert(false && "This should never happen!");
				break;

			case Rendering::Object::TYPE_TEXTURE_FRAMEBUFFER:
				typeString = "TextureFramebuffer";
				break;

			case Rendering::Object::TYPE_TEXTURES:
				typeString = "Textures";
				break;

			case Rendering::Object::TYPE_TRANSFORM:
				typeString = "Transform";
				break;

			case Rendering::Object::TYPE_TRIANGLE_FANS:
				typeString = "TriangleFans";
				break;

			case Rendering::Object::TYPE_TRIANGLES:
				typeString = "Triangles";
				break;

			case Rendering::Object::TYPE_TRIANGLE_STRIPS:
				typeString = "TriangleStrips";
				break;

			case Rendering::Object::TYPE_UNDISTORTED_BACKGROUND:
				typeString = "UndistortedBackground";
				break;

			case Rendering::Object::TYPE_VERTEX_SET:
				typeString = "VertexSet";
				break;

			case Rendering::Object::TYPE_VIEW:
				ocean_assert(false && "This should never happen!");
				break;

			case Rendering::Object::TYPE_WINDOW_FRAMEBUFFER:
				typeString = "WindowFramebuffer";
				break;
		}
	}

	info.GetReturnValue().Set(newString(typeString, v8::Isolate::GetCurrent()));
}

template <>
void JSBase::propertySetter<Rendering::ObjectRef, JSRenderingObject::AI_NAME>(Rendering::ObjectRef& thisValue, v8::Local<v8::String>& /*property*/, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& /*info*/)
{
	if (thisValue.isNull())
	{
		Log::warning() << "The rendering object does not hold any valid reference.";
		return;
	}

	std::string stringValue;
	if (isValue(value, stringValue))
	{
		thisValue->setName(stringValue);
	}
	else
	{
		Log::error() << "The name property of a RenderingObject object accepts String values only.";
	}
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_IS_VALID>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(!thisValue.isNull());
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_IS_INVALID>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(thisValue.isNull());
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_ADD>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		Rendering::ObjectRef object;
		if (hasValue<Rendering::ObjectRef>(info, 0u, object))
		{
			const Rendering::GroupRef group(thisValue);
			if (group)
			{
				const Rendering::NodeRef nodeObject(object);
				if (nodeObject)
				{
					group->addChild(nodeObject);
					info.GetReturnValue().Set(true);
					return;
				}

				const Rendering::LightSourceRef lightSource(object);
				if (lightSource)
				{
					group->registerLight(lightSource);
					info.GetReturnValue().Set(true);
					return;
				}
			}

			const Rendering::AttributeSetRef attributeSet(thisValue);
			if (attributeSet)
			{
				attributeSet->addAttribute(object);
				info.GetReturnValue().Set(true);
				return;
			}

			const Rendering::FramebufferRef framebuffer(thisValue);
			if (framebuffer)
			{
				framebuffer->addScene(object);
				info.GetReturnValue().Set(true);
				return;
			}
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'add()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_REMOVE>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		Rendering::ObjectRef object;
		if (hasValue<Rendering::ObjectRef>(info, 0u, object))
		{
			const Rendering::GroupRef group(thisValue);
			if (group)
			{
				const Rendering::NodeRef nodeObject(object);
				if (nodeObject)
				{
					group->removeChild(nodeObject);
					info.GetReturnValue().Set(true);
					return;
				}

				const Rendering::LightSourceRef lightSource(object);
				if (lightSource)
				{
					group->unregisterLight(lightSource);
					info.GetReturnValue().Set(true);
					return;
				}
			}

			const Rendering::AttributeSetRef attributeSet(thisValue);
			if (attributeSet)
			{
				attributeSet->removeAttribute(object);
				info.GetReturnValue().Set(true);
				return;
			}

			const Rendering::FramebufferRef framebuffer(thisValue);
			if (framebuffer)
			{
				framebuffer->removeScene(object);
				info.GetReturnValue().Set(true);
				return;
			}

			const Rendering::GeometryRef geometry(thisValue);
			if (geometry)
			{
				geometry->removeRenderable(object);
				info.GetReturnValue().Set(true);
				return;
			}
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'remove()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_PARENT>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		if (thisValue)
		{
			const Rendering::ObjectRefs parents(thisValue->parentObjects());

			if (!parents.empty())
			{
				info.GetReturnValue().Set(createObject<JSRenderingObject>(parents.front(), JSContext::currentContext()));
				return;
			}
		}

		info.GetReturnValue().Set(createObject<JSRenderingObject>(Rendering::ObjectRef(), JSContext::currentContext()));
		return;
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'parent()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_PARENTS>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		if (thisValue)
		{
			info.GetReturnValue().Set(createObjects<JSRenderingObject>(thisValue->parentObjects(), JSContext::currentContext()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'parents()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_ACTIVE_NODE>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::SwitchRef switchNode(thisValue);

		if (switchNode)
		{
			info.GetReturnValue().Set(int(switchNode->activeNode()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'activeNode()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_AMBIENT_COLOR>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::MaterialRef materialNode(thisValue);

		if (materialNode)
		{
			info.GetReturnValue().Set(createObject<JSColor>(materialNode->ambientColor(), JSContext::currentContext()));
			return;
		}

		const Rendering::LightSourceRef lightSourceNode(thisValue);

		if (lightSourceNode)
		{
			info.GetReturnValue().Set(createObject<JSColor>(lightSourceNode->ambientColor(), JSContext::currentContext()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'ambientColor()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_ASPECT_RATIO>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::ViewRef view(thisValue);

		if (view)
		{
			info.GetReturnValue().Set(double(view->aspectRatio()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'aspectRatio()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_ATTENUATION>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::PointLightRef pointLight(thisValue);

		if (pointLight)
		{
			info.GetReturnValue().Set(createObject<JSVector3>(pointLight->attenuation(), JSContext::currentContext()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'attenuation()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_ATTRIBUTE>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::AttributeSetRef attributeSet(thisValue);

		if (attributeSet)
		{
			int index;
			if (hasValue<int>(info, 0u, index) && index >= 0)
			{
				info.GetReturnValue().Set(createObject<JSRenderingObject>(attributeSet->attribute((unsigned int)(index)), JSContext::currentContext()));
				return;
			}
			else
			{
				Log::warning() << "RenderingObject::attribute() needs a positive Integer value as first parameter.";
				return;
			}
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'attenuation()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_BACKGROUND>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::ViewRef view(thisValue);

		if (view)
		{
			info.GetReturnValue().Set(createObject<JSRenderingObject>(view->background(), JSContext::currentContext()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'background()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_BACKGROUND_COLOR>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::ViewRef view(thisValue);

		if (view)
		{
			info.GetReturnValue().Set(createObject<JSColor>(view->backgroundColor(), JSContext::currentContext()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'backgroundColor()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_CHILD>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::GroupRef group(thisValue);

		if (group)
		{
			int index;
			if (hasValue<int>(info, 0u, index) && index >= 0)
			{
				info.GetReturnValue().Set(createObject<JSRenderingObject>(group->child((unsigned int)(index)), JSContext::currentContext()));
				return;
			}
			else
			{
				Log::warning() << "RenderingObject::child() needs a positive Integer value as first parameter.";
				return;
			}
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'child()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_COLORS>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::VertexSetRef vertexSet(thisValue);

		if (vertexSet)
		{
			info.GetReturnValue().Set(createObjects<JSColor>(vertexSet->colors(), JSContext::currentContext()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'colors()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_CONE_ANGLE>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::SpotLightRef spotLight(thisValue);

		if (spotLight)
		{
			info.GetReturnValue().Set(double(spotLight->coneAngle()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'coneAngle()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_CULLING_MODE>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::PrimitiveAttributeRef primitiveAttribute(thisValue);

		if (primitiveAttribute)
		{
			std::string value;

			switch (primitiveAttribute->cullingMode())
			{
				case Rendering::PrimitiveAttribute::CULLING_BACK:
					value = "BACK";
					break;

				case Rendering::PrimitiveAttribute::CULLING_NONE:
					value = "NONE";
					break;

				case Rendering::PrimitiveAttribute::CULLING_FRONT:
					value = "FRONT";
					break;

				case Rendering::PrimitiveAttribute::CULLING_BOTH:
					value = "BOTH";
					break;

				case Rendering::PrimitiveAttribute::CULLING_DEFAULT:
					value = "Default";
					break;
			}

			ocean_assert(!value.empty());

			info.GetReturnValue().Set(newString(value, v8::Isolate::GetCurrent()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'cullingMode()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_DEPTH>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::BoxRef box(thisValue);

		if (box)
		{
			info.GetReturnValue().Set(double(box->size().z()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'depth()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_DIRECTION>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::DirectionalLightRef directionalLight(thisValue);

		if (directionalLight)
		{
			info.GetReturnValue().Set(createObject<JSVector3>(directionalLight->direction(), JSContext::currentContext()));
			return;
		}

		const Rendering::SpotLightRef spotLight(thisValue);

		if (spotLight)
		{
			info.GetReturnValue().Set(createObject<JSVector3>(spotLight->direction(), JSContext::currentContext()));
			return;
		}

		const Rendering::ViewRef view(thisValue);

		if (view)
		{
			info.GetReturnValue().Set(createObject<JSVector3>(view->transformation().rotation() * Vector3(0, 0, -1), JSContext::currentContext()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'direction()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_DIFFUSE_COLOR>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::MaterialRef material(thisValue);

		if (material)
		{
			info.GetReturnValue().Set(createObject<JSColor>(material->diffuseColor(), JSContext::currentContext()));
			return;
		}

		const Rendering::LightSourceRef lightSource(thisValue);

		if (lightSource)
		{
			info.GetReturnValue().Set(createObject<JSColor>(lightSource->diffuseColor(), JSContext::currentContext()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'diffuseColor()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_EMISSIVE_COLOR>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::MaterialRef material(thisValue);

		if (material)
		{
			info.GetReturnValue().Set(createObject<JSColor>(material->emissiveColor(), JSContext::currentContext()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'emissiveColor()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_ENABLED>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::LightSourceRef lightSource(thisValue);

		if (lightSource)
		{
			info.GetReturnValue().Set(lightSource->enabled());
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the `enabled()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_ENVIRONMENT_MODE>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::TextureRef texture(thisValue);

		if (texture)
		{
			std::string value;

			switch (texture->environmentMode())
			{
				case Rendering::Texture::MODE_INVALID:
					value = "INVALID";
					break;

				case Rendering::Texture::MODE_ADD:
					value = "ADD";
					break;

				case Rendering::Texture::MODE_ADD_SIGNED:
					value = "ADD SIGNED";
					break;

				case Rendering::Texture::MODE_BLEND:
					value = "BLEND";
					break;

				case Rendering::Texture::MODE_MODULATE:
					value = "MODULATE";
					break;

				case Rendering::Texture::MODE_REPLACE:
					value = "REPLACE";
					break;

				case Rendering::Texture::MODE_SUBTRACT:
					value = "SUBTRACT";
					break;
			}

			ocean_assert(!value.empty());

			info.GetReturnValue().Set(newString(value, v8::Isolate::GetCurrent()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the `environmentMode()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_EXIST_PARAMETER>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::ShaderProgramRef shaderProgram(thisValue);

		if (shaderProgram)
		{
			std::string value;
			if (hasValue<std::string>(info, 0u, value))
			{
				info.GetReturnValue().Set(shaderProgram->existParameter(value));
				return;
			}

			Log::warning() << "RenderingObject::existParameter() needs a String as value parameter.";
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the `existParameter()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_FACE_MODE>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::PrimitiveAttributeRef primitiveAttribute(thisValue);

		if (primitiveAttribute)
		{
			std::string value;

			switch (primitiveAttribute->faceMode())
			{
				case Rendering::PrimitiveAttribute::MODE_DEFAULT:
					value = "DEFAULT";
					break;

				case Rendering::PrimitiveAttribute::MODE_FACE:
					value = "FACE";
					break;

				case Rendering::PrimitiveAttribute::MODE_LINE:
					value = "LINE";
					break;

				case Rendering::PrimitiveAttribute::MODE_POINT:
					value = "POINT";
					break;
			}

			ocean_assert(!value.empty());

			info.GetReturnValue().Set(newString(value, v8::Isolate::GetCurrent()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the `faceMode()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_FAR_DISTANCE>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::ViewRef view(thisValue);

		if (view)
		{
			info.GetReturnValue().Set(double(view->farDistance()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the `farDistance()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_USE_HEADLIGHT>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::ViewRef view(thisValue);

		if (view)
		{
			info.GetReturnValue().Set(view->useHeadlight());
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the `useHeadlight()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_HEIGHT>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::BoxRef box(thisValue);

		if (box)
		{
			info.GetReturnValue().Set(double(box->size().y()));
			return;
		}

		const Rendering::ConeRef cone(thisValue);

		if (cone)
		{
			info.GetReturnValue().Set(double(cone->height()));
			return;
		}

		const Rendering::CylinderRef cylinder(thisValue);

		if (cylinder)
		{
			info.GetReturnValue().Set(double(cylinder->height()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the `height()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_INTENSITY>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::LightSourceRef lightSource(thisValue);

		if (lightSource)
		{
			info.GetReturnValue().Set(double(lightSource->intensity()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the `intensity()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_IS_COMPILED>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::ShaderProgramRef shaderProgram(thisValue);

		if (shaderProgram)
		{
			info.GetReturnValue().Set(shaderProgram->isCompiled());
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the `isCompiled()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_MAGNIFICATION_FILTER_MODE>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::TextureRef texture(thisValue);

		if (texture)
		{
			std::string value;

			switch (texture->magnificationFilterMode())
			{
				case Rendering::Texture::MAG_MODE_INVALID:
					value = "INVALID";
					break;

				case Rendering::Texture::MAG_MODE_LINEAR:
					value = "LINEAR";
					break;

				case Rendering::Texture::MAG_MODE_NEAREST:
					value = "NEAREST";
					break;
			}

			ocean_assert(!value.empty());

			info.GetReturnValue().Set(newString(value, v8::Isolate::GetCurrent()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the `magnificationFilterMode()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_MEDIUM>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::MediaTexture2DRef texture(thisValue);

		if (texture)
		{
			info.GetReturnValue().Set(createObject<JSMediaObject>(texture->medium(), JSContext::currentContext()));
			return;
		}

		const Rendering::UndistortedBackgroundRef background(thisValue);

		if (background)
		{
			info.GetReturnValue().Set(createObject<JSMediaObject>(background->medium(), JSContext::currentContext()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the `medium()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_MINIFICATION_FILTER_MODE>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::TextureRef texture(thisValue);

		if (texture)
		{
			std::string value;

			switch (texture->minificationFilterMode())
			{
				case Rendering::Texture::MIN_MODE_INVALID:
					value = "INVALID";
					break;

				case Rendering::Texture::MIN_MODE_LINEAR:
					value = "LINEAR";
					break;

				case Rendering::Texture::MIN_MODE_NEAREST:
					value = "NEAREST";
					break;

				case Rendering::Texture::MIN_MODE_NEAREST_MIPMAP_NEAREST:
					value = "NEAREST MIPMAP NEAREST";
					break;

				case Rendering::Texture::MIN_MODE_LINEAR_MIPMAP_NEAREST:
					value = "LINEAR MIPMAP NEAREST";
					break;

				case Rendering::Texture::MIN_MODE_NEAREST_MIPMAP_LINEAR:
					value = "NEAREST MIPMAP LINEAR";
					break;

				case Rendering::Texture::MIN_MODE_LINEAR_MIPMAP_LINEAR:
					value = "LINEAR MIPMAP LINEAR";
					break;
			}

			ocean_assert(!value.empty());

			info.GetReturnValue().Set(newString(value, v8::Isolate::GetCurrent()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the `minificationFilterMode()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_NEAR_DISTANCE>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::ViewRef view(thisValue);

		if (view)
		{
			info.GetReturnValue().Set(double(view->nearDistance()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the `nearDistance()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_NORMALS>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::VertexSetRef vertexSet(thisValue);

		if (vertexSet)
		{
			info.GetReturnValue().Set(createObjects<JSVector3>(vertexSet->normals(), JSContext::currentContext()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the `normals()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_NUMBER_ATTRIBUTES>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::AttributeSetRef attributeSet(thisValue);

		if (attributeSet)
		{
			info.GetReturnValue().Set(int(attributeSet->numberAttributes()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the `numberAttributes()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_NUMBER_CHILDREN>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::GroupRef group(thisValue);

		if (group)
		{
			info.GetReturnValue().Set(int(group->numberChildren()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the `numberChildren()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_PARAMETER>(Rendering::ObjectRef& /*thisValue*/, const v8::FunctionCallbackInfo<v8::Value>& /*info*/)
{
	ocean_assert(false && "Missing implementation!");
	Log::error() << "Missing implementation!";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_PARAMETER_ELEMENTS>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::ShaderProgramRef shaderProgram(thisValue);

		if (shaderProgram)
		{
			std::string value;
			if (hasValue<std::string>(info, 0u, value))
			{
				info.GetReturnValue().Set(int(shaderProgram->parameterElements(value)));
				return;
			}

			Log::warning() << "RenderingObject::parameterElements() needs a String as value parameter.";
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the `parameterElements()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_PARAMETER_TYPE>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::ShaderProgramRef shaderProgram(thisValue);

		if (shaderProgram)
		{
			std::string parameterName;
			if (hasValue<std::string>(info, 0u, parameterName))
			{
				std::string value;

				switch (shaderProgram->parameterType(parameterName))
				{
					case Rendering::ShaderProgram::TYPE_INVALID:
						value = "INVALID";
						break;

					case Rendering::ShaderProgram::TYPE_UNSUPPORTED:
						value = "UNSUPPORTED";
						break;

					case Rendering::ShaderProgram::TYPE_BOOL:
						value = "BOOL";
						break;

					case Rendering::ShaderProgram::TYPE_FLOAT:
						value = "FLOAT";
						break;

					case Rendering::ShaderProgram::TYPE_INTEGER:
						value = "INTEGER";
						break;

					case Rendering::ShaderProgram::TYPE_MATRIX3:
						value = "MATRIX3";
						break;

					case Rendering::ShaderProgram::TYPE_MATRIX4:
						value = "MATRIX4";
						break;

					case Rendering::ShaderProgram::TYPE_SAMPLE1:
						value = "SAMPLE1";
						break;

					case Rendering::ShaderProgram::TYPE_SAMPLE2:
						value = "SAMPLE2";
						break;

					case Rendering::ShaderProgram::TYPE_SAMPLE3:
						value = "SAMPLE3";
						break;

					case Rendering::ShaderProgram::TYPE_STRUCT:
						value = "STRUCT";
						break;

					case Rendering::ShaderProgram::TYPE_VECTOR2:
						value = "VECTOR2";
						break;

					case Rendering::ShaderProgram::TYPE_VECTOR3:
						value = "VECTOR3";
						break;

					case Rendering::ShaderProgram::TYPE_VECTOR4:
						value = "VECTOR4";
						break;
				}

				ocean_assert(!value.empty());

				info.GetReturnValue().Set(newString(value, v8::Isolate::GetCurrent()));
				return;
			}

			Log::warning() << "RenderingObject::parameterType() needs a String as value parameter.";
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the `parameterType()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_POSITION>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::PointLightRef pointLight(thisValue);

		if (pointLight)
		{
			info.GetReturnValue().Set(createObject<JSVector3>(pointLight->position(), JSContext::currentContext()));
			return;
		}

		const Rendering::ViewRef view(thisValue);

		if (view)
		{
			info.GetReturnValue().Set(createObject<JSVector3>(view->transformation().translation(), JSContext::currentContext()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the `position()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_RADIUS>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::ConeRef cone(thisValue);

		if (cone)
		{
			info.GetReturnValue().Set(double(cone->radius()));
			return;
		}

		const Rendering::CylinderRef cylinder(thisValue);

		if (cylinder)
		{
			info.GetReturnValue().Set(double(cylinder->radius()));
			return;
		}

		const Rendering::SphereRef sphere(thisValue);

		if (sphere)
		{
			info.GetReturnValue().Set(double(sphere->radius()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the `cone()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_REFLECTIVITY>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::MaterialRef material(thisValue);

		if (material)
		{
			info.GetReturnValue().Set(double(material->reflectivity()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the `reflectivity()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_ROTATION>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::TransformRef transform(thisValue);
		if (transform)
		{
			info.GetReturnValue().Set(createObject<JSQuaternion>(transform->transformation().rotation(), JSContext::currentContext()));
			return;
		}

		const Rendering::TextureRef texture(thisValue);
		if (texture)
		{
			info.GetReturnValue().Set(createObject<JSQuaternion>(texture->transformation().rotation(), JSContext::currentContext()));
			return;
		}

		const Rendering::ViewRef view(thisValue);
		if (view)
		{
			info.GetReturnValue().Set(createObject<JSQuaternion>(view->transformation().rotation(), JSContext::currentContext()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the `rotation()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SCALE>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::TransformRef transform(thisValue);
		if (transform)
		{
			info.GetReturnValue().Set(createObject<JSVector3>(transform->transformation().scale(), JSContext::currentContext()));
			return;
		}

		const Rendering::TextureRef texture(thisValue);
		if (texture)
		{
			info.GetReturnValue().Set(createObject<JSVector3>(texture->transformation().scale(), JSContext::currentContext()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the `scale()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SIZE>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::BoxRef box(thisValue);
		if (box)
		{
			info.GetReturnValue().Set(createObject<JSVector3>(box->size(), JSContext::currentContext()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the `size()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SPECULAR_COLOR>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::MaterialRef material(thisValue);
		if (material)
		{
			info.GetReturnValue().Set(createObject<JSColor>(material->specularColor(), JSContext::currentContext()));
			return;
		}

		const Rendering::LightSourceRef lightSource(thisValue);
		if (lightSource)
		{
			info.GetReturnValue().Set(createObject<JSColor>(lightSource->specularColor(), JSContext::currentContext()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the `specularColor()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SPECULAR_EXPONENT>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::MaterialRef material(thisValue);
		if (material)
		{
			info.GetReturnValue().Set(double(material->specularExponent()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the `specularExponent()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SPOT_EXPONENT>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::SpotLightRef spotLight(thisValue);
		if (spotLight)
		{
			info.GetReturnValue().Set(double(spotLight->spotExponent()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the `spotExponent()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_TRANSPARENCY>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::MaterialRef material(thisValue);
		if (material)
		{
			info.GetReturnValue().Set(double(material->transparency()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the `transparency()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_TRANSFORMATION>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::TransformRef transform(thisValue);
		if (transform)
		{
			info.GetReturnValue().Set(createObject<JSHomogenousMatrix4>(transform->transformation(), JSContext::currentContext()));
			return;
		}

		const Rendering::TextureRef texture(thisValue);
		if (texture)
		{
			info.GetReturnValue().Set(createObject<JSHomogenousMatrix4>(texture->transformation(), JSContext::currentContext()));
			return;
		}

		const Rendering::ViewRef view(thisValue);
		if (view)
		{
			info.GetReturnValue().Set(createObject<JSHomogenousMatrix4>(view->transformation(), JSContext::currentContext()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the `transformation()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_TEXTURE_COORDINATES>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::VertexSetRef vertexSet(thisValue);
		if (vertexSet)
		{
			constexpr unsigned int layerIndex = 0u;

			info.GetReturnValue().Set(createObjects<JSVector2>(vertexSet->textureCoordinates(layerIndex), JSContext::currentContext()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the `textureCoordinates()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_TRANSLATION>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::TransformRef transform(thisValue);
		if (transform)
		{
			info.GetReturnValue().Set(createObject<JSVector3>(transform->transformation().translation(), JSContext::currentContext()));
			return;
		}

		const Rendering::TextureRef texture(thisValue);
		if (texture)
		{
			info.GetReturnValue().Set(createObject<JSVector3>(texture->transformation().translation(), JSContext::currentContext()));
			return;
		}

		const Rendering::ViewRef view(thisValue);
		if (view)
		{
			info.GetReturnValue().Set(createObject<JSVector3>(view->transformation().translation(), JSContext::currentContext()));
			return;
		}

		const Rendering::PointLightRef pointLight(thisValue);
		if (pointLight)
		{
			info.GetReturnValue().Set(createObject<JSVector3>(pointLight->position(), JSContext::currentContext()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the `translation()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_VERTICES>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::VertexSetRef vertexSet(thisValue);
		if (vertexSet)
		{
			info.GetReturnValue().Set(createObjects<JSVector3>(vertexSet->vertices(), JSContext::currentContext()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the `vertices()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_VERTEX_SET>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::PrimitiveRef primitive(thisValue);
		if (primitive)
		{
			info.GetReturnValue().Set(createObject<JSRenderingObject>(primitive->vertexSet(), JSContext::currentContext()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the `vertexSet()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_VISIBLE>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::NodeRef node(thisValue);
		if (node)
		{
			info.GetReturnValue().Set(node->visible());
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the `visible()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_WIDTH>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::BoxRef box(thisValue);
		if (box)
		{
			info.GetReturnValue().Set(double(box->size().x()));
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the `width()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_WORLD_TRANSFORMATION>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::NodeRef node(thisValue);
		if (node)
		{
			const HomogenousMatrices4 worldTransformations(node->worldTransformations());

			if (worldTransformations.empty())
			{
				info.GetReturnValue().Set(createObject<JSHomogenousMatrix4>(HomogenousMatrix4(true), JSContext::currentContext()));
				return;
			}
			else
			{
				info.GetReturnValue().Set(createObject<JSHomogenousMatrix4>(worldTransformations.front(), JSContext::currentContext()));
				return;
			}
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the `worldTransformation()` function.";
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_ACTIVE_NODE>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		const Rendering::SwitchRef switchNode(thisValue);

		if (switchNode)
		{
			int index;
			if (hasValue<int>(info, 0u, index))
			{
				const Index32 nodeIndex = index < 0 ? Rendering::Switch::invalidIndex : Index32(index);

				switchNode->setActiveNode(nodeIndex);
				info.GetReturnValue().Set(true);
				return;
			}
			else
			{
				Log::warning() << "RenderingObject::setActiveNode() needs an Integer value as first parameter.";
				info.GetReturnValue().Set(false);
				return;
			}
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setActiveNode()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_AMBIENT_COLOR>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		RGBAColor color;
		if (hasValue<RGBAColor>(info, 0u, color))
		{
			const Rendering::MaterialRef material(thisValue);
			if (material)
			{
				material->setAmbientColor(color);
				info.GetReturnValue().Set(true);
				return;
			}

			const Rendering::LightSourceRef lightSource(thisValue);
			if (lightSource)
			{
				lightSource->setAmbientColor(color);
				info.GetReturnValue().Set(true);
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setAmbientColor() needs a Color object as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setAmbientColor()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_ASPECT_RATIO>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		Scalar value;
		if (hasValue<Scalar>(info, 0u, value))
		{
			const Rendering::ViewRef view(thisValue);
			if (view)
			{
				view->setAspectRatio(value);
				info.GetReturnValue().Set(true);
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setAspectRatio() needs a Number value as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setAspectRatio()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_ATTENUATION>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		Vector3 value;
		if (hasValue<Vector3>(info, 0u, value))
		{
			const Rendering::PointLightRef pointLight(thisValue);
			if (pointLight)
			{
				pointLight->setAttenuation(value);
				info.GetReturnValue().Set(true);
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setAttenuation() needs a Vector3 object as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setAttenuation()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_ADD_BACKGROUND>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		Rendering::ObjectRef background;
		if (hasValue<Rendering::ObjectRef>(info, 0u, background))
		{
			const Rendering::ViewRef view(thisValue);
			if (view)
			{
				view->addBackground(background);
				info.GetReturnValue().Set(true);
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::addBackground() needs a RenderingObject object as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'addBackground()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_REMOVE_BACKGROUND>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		Rendering::ObjectRef background;
		if (hasValue<Rendering::ObjectRef>(info, 0u, background))
		{
			const Rendering::ViewRef view(thisValue);
			if (view)
			{
				view->removeBackground(background);
				info.GetReturnValue().Set(true);
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::removeBackground() needs a RenderingObject object as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'removeBackground()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_BACKGROUND_COLOR>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		RGBAColor backgroundColor;
		if (hasValue(info, 0u, backgroundColor))
		{
			const Rendering::ViewRef view(thisValue);
			if (view)
			{
				view->setBackgroundColor(backgroundColor);
				info.GetReturnValue().Set(true);
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setBackgroundColor() needs a Color object as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setBackgroundColor()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_COLORS>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		RGBAColors colors;
		if (hasValue(info, 0u, colors))
		{
			const Rendering::VertexSetRef vertexSet(thisValue);
			if (vertexSet)
			{
				vertexSet->setColors(colors);
				info.GetReturnValue().Set(true);
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setColors() needs a Color objects as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setColors()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_CONE_ANGLE>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		Scalar value;
		if (hasValue(info, 0u, value))
		{
			const Rendering::SpotLightRef spotLight(thisValue);
			if (spotLight)
			{
				spotLight->setConeAngle(value);
				info.GetReturnValue().Set(true);
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setConeAngle() needs a Number value as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setConeAngle()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_CODE>(Rendering::ObjectRef& /*thisValue*/, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	Log::warning() << "The function 'setCode()` is currently not supported";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_CODE_FILE>(Rendering::ObjectRef& /*thisValue*/, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	Log::warning() << "The function 'setCodeFile()` is currently not supported";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_CULLING_MODE>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		std::string value;
		if (hasValue(info, 0u, value))
		{
			const Rendering::PrimitiveAttributeRef primitiveAttribute(thisValue);
			if (primitiveAttribute)
			{
				Rendering::PrimitiveAttribute::CullingMode cullingMode = Rendering::PrimitiveAttribute::CULLING_DEFAULT;

				if (value == "BACK")
				{
					cullingMode = Rendering::PrimitiveAttribute::CULLING_BACK;
				}
				else if (value == "NONE")
				{
					cullingMode = Rendering::PrimitiveAttribute::CULLING_NONE;
				}
				else if (value == "FRONT")
				{
					cullingMode = Rendering::PrimitiveAttribute::CULLING_FRONT;
				}
				else if (value == "BOTH")
				{
					cullingMode = Rendering::PrimitiveAttribute::CULLING_BOTH;
				}
				else
				{
					Log::warning() << "Invalid culling mode: \"" << value << "\".";
				}

				primitiveAttribute->setCullingMode(cullingMode);
				info.GetReturnValue().Set(true);
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setCullingMode() needs a String value as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setCullingMode()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_DEPTH>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		Scalar depth;
		if (hasValue(info, 0u, depth))
		{
			const Rendering::BoxRef box(thisValue);
			if (box)
			{
				const Vector3 size(box->size());

				box->setSize(Vector3(size.x(), size.y(), depth));
				info.GetReturnValue().Set(true);
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setDepth() needs a Number value as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setDepth()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_DIFFUSE_COLOR>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		RGBAColor color;
		if (hasValue<RGBAColor>(info, 0u, color))
		{
			const Rendering::MaterialRef material(thisValue);
			if (material)
			{
				material->setDiffuseColor(color);
				info.GetReturnValue().Set(true);
				return;
			}

			const Rendering::LightSourceRef lightSource(thisValue);
			if (lightSource)
			{
				lightSource->setDiffuseColor(color);
				info.GetReturnValue().Set(true);
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setDiffuseColor() needs a Color object as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setDiffuseColor()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_DIRECTION>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		Vector3 direction;
		if (hasValue(info, 0u, direction))
		{
			const Rendering::DirectionalLightRef directionalLight(thisValue);
			if (directionalLight)
			{
				directionalLight->setDirection(direction);
				info.GetReturnValue().Set(true);
			}

			const Rendering::SpotLightRef spotLight(thisValue);
			if (spotLight)
			{
				spotLight->setDirection(direction);
				info.GetReturnValue().Set(true);
			}

			const Rendering::ViewRef view(thisValue);
			if (view)
			{
				HomogenousMatrix4 transformation(view->transformation());
				transformation.setRotation(Quaternion(Vector3(0, 0, -1), direction));

				view->setTransformation(transformation);
				info.GetReturnValue().Set(true);
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setDirection() needs an Vector3 object as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setDirection()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_EMISSIVE_COLOR>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		RGBAColor color;
		if (hasValue<RGBAColor>(info, 0u, color))
		{
			const Rendering::MaterialRef material(thisValue);
			if (material)
			{
				material->setEmissiveColor(color);
				info.GetReturnValue().Set(true);
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setEmissiveColor() needs a Color object as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setEmissiveColor()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_ENABLED>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		bool value;
		if (hasValue(info, 0u, value))
		{
			const Rendering::LightSourceRef lightSource(thisValue);
			if (lightSource)
			{
				lightSource->setEnabled(value);
				info.GetReturnValue().Set(true);
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setEnabled() needs an Boolean value as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setEnabled()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_ENVIRONMENT_MODE>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		std::string mode;
		if (hasValue(info, 0u, mode))
		{
			const Rendering::TextureRef texture(thisValue);
			if (texture)
			{
				Rendering::Texture::EnvironmentMode environmentMode = Rendering::Texture::MODE_REPLACE;

				if (mode == "ADD")
				{
					environmentMode = Rendering::Texture::MODE_ADD;
				}
				else if (mode == "ADD SIGNED")
				{
					environmentMode = Rendering::Texture::MODE_ADD_SIGNED;
				}
				else if (mode == "BLEND")
				{
					environmentMode = Rendering::Texture::MODE_BLEND;
				}
				else if (mode == "MODULATE")
				{
					environmentMode = Rendering::Texture::MODE_MODULATE;
				}
				else if (mode == "REPLACE")
				{
					environmentMode = Rendering::Texture::MODE_REPLACE;
				}
				else if (mode == "SUBTRACT")
				{
					environmentMode = Rendering::Texture::MODE_SUBTRACT;
				}
				else
				{
					Log::warning() << "Invalid environment mode: \"" << mode << "\".";
				}

				texture->setEnvironmentMode(environmentMode);
				info.GetReturnValue().Set(true);
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setEnvironmentMode() needs a String value as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setEnvironmentMode()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_FACE_MODE>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		std::string mode;
		if (hasValue(info, 0u, mode))
		{
			const Rendering::PrimitiveAttributeRef primitiveAttribute(thisValue);
			if (primitiveAttribute)
			{
				Rendering::PrimitiveAttribute::FaceMode faceMode = Rendering::PrimitiveAttribute::MODE_FACE;

				if (mode == "FACE")
				{
					faceMode = Rendering::PrimitiveAttribute::MODE_FACE;
				}
				else if (mode == "LINE")
				{
					faceMode = Rendering::PrimitiveAttribute::MODE_LINE;
				}
				else if (mode == "POINT")
				{
					faceMode = Rendering::PrimitiveAttribute::MODE_POINT;
				}
				else
				{
					Log::warning() << "Invalid face mode: \"" << mode << "\".";
				}

				primitiveAttribute->setFaceMode(faceMode);
				info.GetReturnValue().Set(true);
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setFaceMode() needs a String value as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setFaceMode()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_FAR_DISTANCE>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		Scalar value;
		if (hasValue(info, 0u, value))
		{
			const Rendering::ViewRef view(thisValue);
			if (view)
			{
				view->setFarDistance(value);
				info.GetReturnValue().Set(true);
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setFarDistance() needs a Number value as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setFarDistance()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_USE_HEADLIGHT>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		bool value;
		if (hasValue(info, 0u, value))
		{
			const Rendering::ViewRef view(thisValue);
			if (view)
			{
				view->setUseHeadlight(value);
				info.GetReturnValue().Set(true);
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setUseHeadlight() needs a Boolean value as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setUseHeadlight()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_HEIGHT>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		Scalar height;
		if (hasValue(info, 0u, height))
		{
			const Rendering::BoxRef box(thisValue);
			if (box)
			{
				const Vector3 size(box->size());
				info.GetReturnValue().Set(box->setSize(Vector3(size.x(), height, size.z())));
				return;
			}

			const Rendering::ConeRef cone(thisValue);
			if (cone)
			{
				info.GetReturnValue().Set(cone->setHeight(height));
				return;
			}

			const Rendering::CylinderRef cylinder(thisValue);
			if (cylinder)
			{
				info.GetReturnValue().Set(cylinder->setHeight(height));
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setHeight() needs a Number value as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setHeight()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_INTENSITY>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		Scalar intensity;
		if (hasValue(info, 0u, intensity))
		{
			const Rendering::LightSourceRef lightSource(thisValue);
			if (lightSource)
			{
				info.GetReturnValue().Set(lightSource->setIntensity(intensity));
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setIntensity() needs a Number value as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setIntensity()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_LEFT_PROJECTION_MATRIX>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		SquareMatrix4 matrix;
		if (hasValue(info, 0u, matrix))
		{
			const Rendering::StereoViewRef stereoView(thisValue);
			if (stereoView)
			{
				stereoView->setLeftProjectionMatrix(matrix);
				info.GetReturnValue().Set(true);
				return;
			}

			const Rendering::PerspectiveViewRef perspectiveView(thisValue);
			if (perspectiveView)
			{
				perspectiveView->setProjectionMatrix(matrix);
				info.GetReturnValue().Set(true);
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setLeftProjectionMatrix() needs a SquareMatrix4 object as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setLeftProjectionMatrix()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_LEFT_TRANSFORMATION>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		HomogenousMatrix4 world_T_leftView;
		if (hasValue(info, 0u, world_T_leftView))
		{
			const Rendering::StereoViewRef stereoView(thisValue);
			if (stereoView)
			{
				stereoView->setLeftTransformation(world_T_leftView);
				info.GetReturnValue().Set(true);
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setLeftTransformation() needs a HomogenousMatrix4 object as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setLeftTransformation()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_MAGNIFICATION_FILTER_MODE>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		std::string mode;
		if (hasValue(info, 0u, mode))
		{
			const Rendering::TextureRef texture(thisValue);
			if (texture)
			{
				Rendering::Texture::MagFilterMode magFilterModel = Rendering::Texture::MAG_MODE_LINEAR;

				if (mode == "LINEAR")
				{
					magFilterModel = Rendering::Texture::MAG_MODE_LINEAR;
				}
				else if (mode == "NEAREST")
				{
					magFilterModel = Rendering::Texture::MAG_MODE_NEAREST;
				}
				else
				{
					Log::warning() << "Invalid magnification filter mode: \"" << mode << "\".";
				}

				texture->setMagnificationFilterMode(magFilterModel);
				info.GetReturnValue().Set(true);
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setMagnificationFilterMode() needs a String value as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setMagnificationFilterMode()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_MEDIUM>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		Media::MediumRef medium;
		if (hasValue(info, 0u, medium))
		{
			const Rendering::MediaTexture2DRef texture(thisValue);
			if (texture)
			{
				texture->setMedium(medium);
				info.GetReturnValue().Set(true);
				return;
			}

			const Rendering::UndistortedBackgroundRef background(thisValue);
			if (background)
			{
				background->setMedium(medium);
				info.GetReturnValue().Set(true);
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setMedium() needs a MediumObject object as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setMedium()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_MINIFICATION_FILTER_MODE>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		std::string mode;
		if (hasValue(info, 0u, mode))
		{
			const Rendering::TextureRef texture(thisValue);
			if (texture)
			{
				Rendering::Texture::MinFilterMode minFilterModel = Rendering::Texture::MIN_MODE_LINEAR;

				if (mode == "LINEAR")
				{
					minFilterModel = Rendering::Texture::MIN_MODE_LINEAR;
				}
				else if (mode == "NEAREST")
				{
					minFilterModel = Rendering::Texture::MIN_MODE_NEAREST;
				}
				else if (mode == "NEAREST MIPMAP NEAREST")
				{
					minFilterModel = Rendering::Texture::MIN_MODE_NEAREST_MIPMAP_NEAREST;
				}
				else if (mode == "LINEAR MIPMAP NEAREST")
				{
					minFilterModel = Rendering::Texture::MIN_MODE_LINEAR_MIPMAP_NEAREST;
				}
				else if (mode == "NEAREST MIPMAP LINEAR")
				{
					minFilterModel = Rendering::Texture::MIN_MODE_NEAREST_MIPMAP_LINEAR;
				}
				else if (mode == "LINEAR MIPMAP LINEAR")
				{
					minFilterModel = Rendering::Texture::MIN_MODE_LINEAR_MIPMAP_LINEAR;
				}
				else
				{
					Log::warning() << "Invalid minification filter mode: \"" << mode << "\".";
				}

				texture->setMinificationFilterMode(minFilterModel);
				info.GetReturnValue().Set(true);
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setMinificationFilterMode() needs a String value as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setMinificationFilterMode()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_NEAR_DISTANCE>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		Scalar value;
		if (hasValue(info, 0u, value))
		{
			const Rendering::ViewRef view(thisValue);
			if (view)
			{
				view->setNearDistance(value);
				info.GetReturnValue().Set(true);
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setNearDistance() needs a Number value as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setNearDistance()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_NORMALS>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		Vectors3 normals;
		if (hasValue(info, 0u, normals))
		{
			const Rendering::VertexSetRef vertexSet(thisValue);
			if (vertexSet)
			{
				vertexSet->setNormals(normals);
				info.GetReturnValue().Set(true);
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setNormals() needs Vector3 objects as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setNormals()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_PARAMETER>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		std::string parameterName;
		if (hasValue(info, 0u, parameterName))
		{
			const Rendering::ShaderProgramRef shaderProgram(thisValue);
			if (shaderProgram)
			{
				Scalar scalarValue;
				if (hasValue(info, 1u, scalarValue))
				{
					info.GetReturnValue().Set(shaderProgram->setParameter(parameterName, scalarValue));
					return;
				}

				int intValue;
				if (hasValue(info, 1u, intValue))
				{
					info.GetReturnValue().Set(shaderProgram->setParameter(parameterName, intValue));
					return;
				}

				bool boolValue;
				if (hasValue(info, 1u, boolValue))
				{
					info.GetReturnValue().Set(shaderProgram->setParameter(parameterName, boolValue ? 1 : 0));
					return;
				}

				std::vector<float> floatsValue;
				if (hasValue(info, 1u, floatsValue))
				{
					info.GetReturnValue().Set(shaderProgram->setParameter(parameterName, floatsValue.data(), (unsigned int)(floatsValue.size())));
					return;
				}

				RGBAColor colorValue;
				if (hasValue(info, 1u, colorValue))
				{
					info.GetReturnValue().Set(shaderProgram->setParameter(parameterName, colorValue.data(), 4));
					return;
				}

				HomogenousMatrix4 homogenousMatrixValue;
				if (hasValue(info, 1u, homogenousMatrixValue))
				{
					info.GetReturnValue().Set(shaderProgram->setParameter(parameterName, homogenousMatrixValue));
					return;
				}

				SquareMatrix3 squareMatrixValue3;
				if (hasValue(info, 1u, squareMatrixValue3))
				{
					info.GetReturnValue().Set(shaderProgram->setParameter(parameterName, squareMatrixValue3));
					return;
				}

				SquareMatrix4 squareMatrixValue4;
				if (hasValue(info, 1u, squareMatrixValue4))
				{
					info.GetReturnValue().Set(shaderProgram->setParameter(parameterName, squareMatrixValue4));
					return;
				}

				Vector2 vectorValue2;
				if (hasValue(info, 1u, vectorValue2))
				{
					info.GetReturnValue().Set(shaderProgram->setParameter(parameterName, vectorValue2));
					return;
				}

				Vector3 vectorValue3;
				if (hasValue(info, 1u, vectorValue3))
				{
					info.GetReturnValue().Set(shaderProgram->setParameter(parameterName, vectorValue3));
					return;
				}

				Vector4 vectorValue4;
				if (hasValue(info, 1u, vectorValue4))
				{
					info.GetReturnValue().Set(shaderProgram->setParameter(parameterName, vectorValue4));
					return;
				}

				Log::warning() << "RenderingObject::setParameter() does not accept the given parameter.";
				info.GetReturnValue().Set(false);
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setParameter() needs a String value as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setNormals()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_POSITION>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		Vector3 position;
		if (hasValue(info, 0u, position))
		{
			const Rendering::PointLightRef pointLight(thisValue);
			if (pointLight)
			{
				pointLight->setPosition(position);
				info.GetReturnValue().Set(true);
				return;
			}

			const Rendering::ViewRef view(thisValue);
			if (view)
			{
				HomogenousMatrix4 transformation = view->transformation();
				transformation.setTranslation(position);

				view->setTransformation(transformation);
				info.GetReturnValue().Set(true);
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setPosition() needs a Vector3 object as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setPosition()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_PROJECTION_MATRIX>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		SquareMatrix4 matrix;
		if (hasValue(info, 0u, matrix))
		{
			const Rendering::PerspectiveViewRef perspectiveView(thisValue);
			if (perspectiveView)
			{
				perspectiveView->setProjectionMatrix(matrix);
				info.GetReturnValue().Set(true);
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setProjectionMatrix() needs a SquareMatrix4 object as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setProjectionMatrix()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_RADIUS>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		Scalar radius;
		if (hasValue(info, 0u, radius))
		{
			const Rendering::ConeRef cone(thisValue);
			if (cone)
			{
				info.GetReturnValue().Set(cone->setRadius(radius));
				return;
			}

			const Rendering::CylinderRef cylinder(thisValue);
			if (cylinder)
			{
				info.GetReturnValue().Set(cylinder->setRadius(radius));
				return;
			}

			const Rendering::SphereRef sphere(thisValue);
			if (sphere)
			{
				info.GetReturnValue().Set(sphere->setRadius(radius));
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setRadius() needs a Number value as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setRadius()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_REFLECTIVITY>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		float reflectivity;
		if (hasValue(info, 0u, reflectivity))
		{
			const Rendering::MaterialRef material(thisValue);
			if (material)
			{
				material->setReflectivity(reflectivity);
				info.GetReturnValue().Set(true);
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setReflectivity() needs a Number value as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setReflectivity()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_RIGHT_PROJECTION_MATRIX>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		SquareMatrix4 matrix;
		if (hasValue(info, 0u, matrix))
		{
			const Rendering::StereoViewRef stereoView(thisValue);
			if (stereoView)
			{
				stereoView->setRightProjectionMatrix(matrix);
				info.GetReturnValue().Set(true);
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setRightProjectionMatrix() needs a SquareMatrix4 object as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setRightProjectionMatrix()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_RIGHT_TRANSFORMATION>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		HomogenousMatrix4 world_T_rightView;
		if (hasValue(info, 0u, world_T_rightView))
		{
			const Rendering::StereoViewRef stereoView(thisValue);
			if (stereoView)
			{
				stereoView->setRightTransformation(world_T_rightView);
				info.GetReturnValue().Set(true);
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setRightTransformation() needs a HomogenousMatrix4 object as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setRightTransformation()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_ROTATION>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		Quaternion quaternion;
		if (!hasValue(info, 0u, quaternion))
		{
			Euler euler;
			if (hasValue(info, 0u, euler))
			{
				quaternion = Quaternion(euler);
			}
			else
			{
				Rotation rotation;
				if (hasValue(info, 0u, rotation))
				{
					quaternion = Quaternion(rotation);
				}
				else
				{
					SquareMatrix3 squareMatrix3;
					if (hasValue(info, 0u, squareMatrix3))
					{
						quaternion = Quaternion(rotation);
					}
					else
					{
						HomogenousMatrix4 homogeousMatrix4;
						if (hasValue(info, 0u, homogeousMatrix4))
						{
							quaternion = homogeousMatrix4.rotation();
						}
						else
						{
							Log::warning() << "RenderingObject::setRotation() needs a Quaternion, Rotation, Euler, SquareMatrix3, or a HomogenousMatrix4 object as first parameter.";
							info.GetReturnValue().Set(false);
							return;
						}
					}
				}
			}
		}

		const Rendering::TransformRef transform(thisValue);
		if (transform)
		{
			HomogenousMatrix4 transformation = transform->transformation();
			transformation.setRotation(quaternion);

			transform->setTransformation(transformation);

			info.GetReturnValue().Set(true);
			return;
		}

		const Rendering::TextureRef texture(thisValue);
		if (texture)
		{
			HomogenousMatrix4 transformation = texture->transformation();
			transformation.setRotation(quaternion);

			texture->setTransformation(transformation);

			info.GetReturnValue().Set(true);
			return;
		}

		const Rendering::ViewRef view(thisValue);
		if (view)
		{
			HomogenousMatrix4 transformation(view->transformation());
			transformation.setRotation(quaternion);

			view->setTransformation(transformation);

			info.GetReturnValue().Set(true);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setRotation()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_SAMPLER>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		int samplerId;
		Rendering::ObjectRef renderingObject;

		if (hasValue(info, 0u, samplerId) && hasValue(info, 1u, renderingObject))
		{
			const Rendering::ShaderProgramRef shaderProgram(thisValue);
			const Rendering::TextureRef texture(renderingObject);

			if (shaderProgram && texture)
			{
				if (samplerId >= 0 && samplerId < 1024)
				{
					shaderProgram->setSampler((unsigned int)(samplerId), texture);
				}

				info.GetReturnValue().Set(true);
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setSampler() needs an Integer value as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setSampler()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_SIZE>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		Vector3 size;
		if (hasValue(info, 0u, size))
		{
			const Rendering::BoxRef box(thisValue);
			if (box)
			{
				info.GetReturnValue().Set(box->setSize(size));
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setSize() needs a Vector3 object as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setSize()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_SPECULAR_COLOR>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		RGBAColor color;
		if (hasValue<RGBAColor>(info, 0u, color))
		{
			const Rendering::MaterialRef material(thisValue);
			if (material)
			{
				material->setSpecularColor(color);
				info.GetReturnValue().Set(true);
				return;
			}

			const Rendering::LightSourceRef lightSource(thisValue);
			if (lightSource)
			{
				lightSource->setSpecularColor(color);
				info.GetReturnValue().Set(true);
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setSpecularColor() needs a Color object as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setSpecularColor()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_SPECULAR_EXPONENT>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		float specularExponent;
		if (hasValue(info, 0u, specularExponent))
		{
			const Rendering::MaterialRef material(thisValue);
			if (material)
			{
				material->setSpecularExponent(specularExponent);
				info.GetReturnValue().Set(true);
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setSpecularExponent() needs a Number value as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setSpecularExponent()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_SPOT_EXPONENT>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		Scalar specularExponent;
		if (hasValue(info, 0u, specularExponent))
		{
			const Rendering::SpotLightRef spotLight(thisValue);
			if (spotLight)
			{
				spotLight->setSpotExponent(specularExponent);
				info.GetReturnValue().Set(true);
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setSpotExponent() needs a Number value as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setSpotExponent()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_TEXTURE_COORDINATES>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		Vectors2 textureCoordinates;
		if (hasValue(info, 0u, textureCoordinates))
		{
			const Rendering::VertexSetRef vertexSet(thisValue);
			if (vertexSet)
			{
				constexpr unsigned int layerIndex = 0u;

				vertexSet->setTextureCoordinates(textureCoordinates, layerIndex);
				info.GetReturnValue().Set(true);
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setTextureCoordinates() needs Vector2 objects as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setTextureCoordinates()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_TRANSLATION>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		Vector3 translation;
		if (!hasValue(info, 0u, translation))
		{
			HomogenousMatrix4 homogeousMatrix4;
			if (hasValue(info, 0u, homogeousMatrix4))
			{
				translation = homogeousMatrix4.translation();
			}
			else
			{
				Log::warning() << "RenderingObject::setTranslation() needs a Vector3 or a HomogenousMatrix4 object as first parameter.";
				info.GetReturnValue().Set(false);
				return;
			}
		}

		const Rendering::TransformRef transform(thisValue);
		if (transform)
		{
			HomogenousMatrix4 transformation = transform->transformation();
			transformation.setTranslation(translation);

			transform->setTransformation(transformation);

			info.GetReturnValue().Set(true);
			return;
		}

		const Rendering::TextureRef texture(thisValue);
		if (texture)
		{
			HomogenousMatrix4 transformation = texture->transformation();
			transformation.setTranslation(translation);

			texture->setTransformation(transformation);

			info.GetReturnValue().Set(true);
			return;
		}

		const Rendering::ViewRef view(thisValue);
		if (view)
		{
			HomogenousMatrix4 transformation(view->transformation());
			transformation.setTranslation(translation);

			view->setTransformation(transformation);

			info.GetReturnValue().Set(true);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setTranslation()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_TRANSFORMATION>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		HomogenousMatrix4 transformation;
		if (!hasValue(info, 0u, transformation))
		{
			Log::warning() << "RenderingObject::setTransformation() needs a HomogenousMatrix4 as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}

		const Rendering::TransformRef transform(thisValue);
		if (transform)
		{
			transform->setTransformation(transformation);

			info.GetReturnValue().Set(true);
			return;
		}

		const Rendering::TextureRef texture(thisValue);
		if (texture)
		{
			texture->setTransformation(transformation);

			info.GetReturnValue().Set(true);
			return;
		}

		const Rendering::ViewRef view(thisValue);
		if (view)
		{
			view->setTransformation(transformation);

			info.GetReturnValue().Set(true);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setTransformation()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_TRANSPARENCY>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		float transparency;
		if (hasValue(info, 0u, transparency))
		{
			const Rendering::MaterialRef material(thisValue);
			if (material)
			{
				material->setTransparency(transparency);
				info.GetReturnValue().Set(true);
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setTransparency() needs a Number value as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setTransparency()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_VERTICES>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		Vectors3 vertices;
		if (hasValue(info, 0u, vertices))
		{
			const Rendering::VertexSetRef vertexSet(thisValue);
			if (vertexSet)
			{
				vertexSet->setVertices(vertices);
				info.GetReturnValue().Set(true);
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setVertices() needs Vector3 objects as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setVertices()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_VERTEX_SET>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		Rendering::ObjectRef renderingObject;
		if (hasValue(info, 0u, renderingObject))
		{
			const Rendering::PrimitiveRef primitive(thisValue);
			const Rendering::VertexSetRef vertexSet(renderingObject);

			if (primitive && vertexSet)
			{
				primitive->setVertexSet(vertexSet);
				info.GetReturnValue().Set(true);
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setVertexSet() needs a RenderingObject object as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setVertexSet()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_VISIBLE>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		bool visible;
		if (hasValue(info, 0u, visible))
		{
			const Rendering::NodeRef node(thisValue);
			if (node)
			{
				node->setVisible(visible);
				info.GetReturnValue().Set(true);
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setVisible() needs a Boolean value as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setVisible()` function.";
	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Rendering::ObjectRef, JSRenderingObject::FI_SET_WIDTH>(Rendering::ObjectRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	try
	{
		Scalar width;
		if (hasValue(info, 0u, width))
		{
			const Rendering::BoxRef box(thisValue);
			if (box)
			{
				const Vector3 size(box->size());
				info.GetReturnValue().Set(box->setSize(Vector3(width, size.y(), size.z())));
				return;
			}
		}
		else
		{
			Log::warning() << "RenderingObject::setWidth() needs a Number value as first parameter.";
			info.GetReturnValue().Set(false);
			return;
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	Log::warning() << "The rendering object does not support the 'setWidth()` function.";
	info.GetReturnValue().Set(false);
}

}

}

}
