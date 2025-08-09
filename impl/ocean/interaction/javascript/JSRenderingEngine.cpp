/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/interaction/javascript/JSRenderingEngine.h"
#include "ocean/interaction/javascript/JSRenderingObject.h"

#include "ocean/rendering/Factory.h"
#include "ocean/rendering/View.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

void JSRenderingEngine::createFunctionTemplate()
{
	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	v8::Local<v8::FunctionTemplate> functionTemplate(v8::FunctionTemplate::New(isolate, constructor<Rendering::EngineRef>));
	functionTemplate->SetClassName(newString(objectName(), isolate));

	v8::Local<v8::ObjectTemplate> objectTemplate(v8::Local<v8::ObjectTemplate>::New(isolate, functionTemplate->InstanceTemplate()));
	objectTemplate->SetInternalFieldCount(1);

	objectTemplate->SetAccessor(newString("name", isolate), propertyGetter<NativeType, AI_NAME>);

	objectTemplate->Set(newString("createObject", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_CREATE_OBJECT>));
	objectTemplate->Set(newString("framebuffer", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_FRAMEBUFFER>));
	objectTemplate->Set(newString("isValid", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_IS_VALID>));
	objectTemplate->Set(newString("isInvalid", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_IS_INVALID>));
	objectTemplate->Set(newString("findObject", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_FIND_OBJECT>));
	objectTemplate->Set(newString("findObjects", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_FIND_OBJECTS>));
	objectTemplate->Set(newString("view", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_VIEW>));

	functionTemplate_.Reset(isolate, functionTemplate);
	objectTemplate_.Reset(isolate, objectTemplate);
}

template <>
void JSBase::constructor<Rendering::EngineRef>(Rendering::EngineRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
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
void JSBase::propertyGetter<Rendering::EngineRef, JSRenderingEngine::AI_NAME>(Rendering::EngineRef& thisValue, v8::Local<v8::String>& /*property*/, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if (thisValue.isNull())
	{
		Log::warning() << "The rendering engine does not hold any valid reference.";
	}
	else
	{
		info.GetReturnValue().Set(newString(thisValue->engineName(), v8::Isolate::GetCurrent()));
	}
}

template <>
void JSBase::function<Rendering::EngineRef, JSRenderingEngine::FI_CREATE_OBJECT>(Rendering::EngineRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	if (thisValue.isNull())
	{
		Log::warning() << "The rendering engine does not hold any valid reference.";
		return;
	}

	std::string type;
	if (hasValue<std::string>(info, 0u, type))
	{
		Rendering::ObjectRef object;

		try
		{
			const Rendering::Factory& factory = thisValue->factory();

			if (type == "AttributeSet")
				object = factory.createAttributeSet();
			else if (type == "Billboard")
				object = factory.createBillboard();
			else if (type == "BlendAttribute")
				object = factory.createBlendAttribute();
			else if (type == "Box")
				object = factory.createBox();
			else if (type == "Cone")
				object = factory.createCone();
			else if (type == "Cylinder")
				object = factory.createCylinder();
			else if (type == "DepthAttribute")
				object = factory.createDepthAttribute();
			else if (type == "DirectionalLight")
				object = factory.createDirectionalLight();
			else if (type == "Geometry")
				object = factory.createGeometry();
			else if (type == "Group")
				object = factory.createGroup();
			else if (type == "LOD")
				object = factory.createLOD();
			else if (type == "Material")
				object = factory.createMaterial();
			else if (type == "ParallelView")
				object = factory.createParallelView();
			else if (type == "PerspectiveView")
				object = factory.createPerspectiveView();
			else if (type == "PointLight")
				object = factory.createPointLight();
			else if (type == "PrimitiveAttribute")
				object = factory.createPrimitiveAttribute();
			else if (type == "Quads")
				object = factory.createQuads();
			else if (type == "QuadStrips")
				object = factory.createQuadStrips();
			else if (type == "Scene")
				object = factory.createScene();
			else if (type == "ShaderProgram")
				object = factory.createShaderProgram();
			else if (type == "Sphere")
				object = factory.createSphere();
			else if (type == "SpotLight")
				object = factory.createSpotLight();
			else if (type == "StereoView")
				object = factory.createStereoView();
			else if (type == "Switch")
				object = factory.createSwitch();
			else if (type == "MediaTexture2D")
				object = factory.createMediaTexture2D();
			else if (type == "Textures")
				object = factory.createTextures();
			else if (type == "Transform")
				object = factory.createTransform();
			else if (type == "Triangles")
				object = factory.createTriangles();
			else if (type == "TriangleFans")
				object = factory.createTriangleFans();
			else if (type == "TriangleStrips")
				object = factory.createTriangleStrips();
			else if (type == "UndistortedBackground")
				object = factory.createUndistortedBackground();
			else if (type == "VertexSet")
				object = factory.createVertexSet();
		}
		catch (...)
		{
			// nothing to do here
		}

		if (object.isNull())
		{
			Log::warning() << "Failed to create a rendering object \"" << type << "\"";
		}
		else
		{
			info.GetReturnValue().Set(createObject<JSRenderingObject>(object, JSContext::currentContext()));
		}
	}
	else
	{
		Log::error() << "RenderingEngine::createObject() needs a string value as first parameter.";
	}
}

template <>
void JSBase::function<Rendering::EngineRef, JSRenderingEngine::FI_FRAMEBUFFER>(Rendering::EngineRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	if (thisValue.isNull())
	{
		Log::warning() << "The rendering engine does not hold any valid reference.";
		return;
	}

	const Rendering::Engine::Framebuffers framebuffers(thisValue->framebuffers());

	if (framebuffers.empty() == false)
	{
		info.GetReturnValue().Set(createObject<JSRenderingObject>(framebuffers.front(), JSContext::currentContext()));
	}
	else
	{
		Log::error() << "RenderingEngine::framebuffer() failed as the rendering engine does not hold a framebuffer.";
	}
}

template <>
void JSBase::function<Rendering::EngineRef, JSRenderingEngine::FI_IS_VALID>(Rendering::EngineRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(!thisValue.isNull());
}

template <>
void JSBase::function<Rendering::EngineRef, JSRenderingEngine::FI_IS_INVALID>(Rendering::EngineRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(thisValue.isNull());
}

template <>
void JSBase::function<Rendering::EngineRef, JSRenderingEngine::FI_FIND_OBJECT>(Rendering::EngineRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	if (thisValue.isNull())
	{
		Log::warning() << "The rendering engine does not hold any valid reference.";
		return;
	}

	std::string name;
	if (hasValue<std::string>(info, 0u, name))
	{
		info.GetReturnValue().Set(createObject<JSRenderingObject>(thisValue->object(name), JSContext::currentContext()));
	}
	else
	{
		Log::error() << "RenderingObject::findObject() needs a String value as parameter.";
	}
}

template <>
void JSBase::function<Rendering::EngineRef, JSRenderingEngine::FI_FIND_OBJECTS>(Rendering::EngineRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	if (thisValue.isNull())
	{
		Log::warning() << "The rendering engine does not hold any valid reference.";
		return;
	}

	std::string name;
	if (hasValue<std::string>(info, 0u, name))
	{
		info.GetReturnValue().Set(createObjects<JSRenderingObject>(thisValue->objects(name), JSContext::currentContext()));
	}
	else
	{
		Log::error() << "RenderingObject::findObjects() needs a String value as parameter.";
	}
}

template <>
void JSBase::function<Rendering::EngineRef, JSRenderingEngine::FI_VIEW>(Rendering::EngineRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	if (thisValue.isNull())
	{
		Log::warning() << "The rendering engine does not hold any valid reference.";
		return;
	}

	const Rendering::Engine::Framebuffers framebuffers(thisValue->framebuffers());

	if (framebuffers.empty() == false)
	{
		if (framebuffers.front())
		{
			info.GetReturnValue().Set(createObject<JSRenderingObject>(framebuffers.front()->view(), JSContext::currentContext()));
		}
	}
	else
	{
		Log::error() << "RenderingEngine::view() failed as the rendering engine does not hold a framebuffer or view.";
	}
}

}

}

}
