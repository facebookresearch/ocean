/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/interaction/javascript/JSLibrary.h"
#include "ocean/interaction/javascript/JSContext.h"
#include "ocean/interaction/javascript/JSColor.h"
//#include "ocean/interaction/javascript/JSData.h"
#include "ocean/interaction/javascript/JSDeviceObject.h"
#include "ocean/interaction/javascript/JSDeviceSample.h"
#include "ocean/interaction/javascript/JSEuler.h"
#include "ocean/interaction/javascript/JSHomogenousMatrix4.h"
#include "ocean/interaction/javascript/JSLine3.h"
#include "ocean/interaction/javascript/JSMediaObject.h"
#include "ocean/interaction/javascript/JSPlane3.h"
#include "ocean/interaction/javascript/JSQuaternion.h"
#include "ocean/interaction/javascript/JSOcean.h"
#include "ocean/interaction/javascript/JSRenderingEngine.h"
#include "ocean/interaction/javascript/JSRenderingObject.h"
#include "ocean/interaction/javascript/JSRotation.h"
#include "ocean/interaction/javascript/JSSceneDescriptionNode.h"
#include "ocean/interaction/javascript/JSSquareMatrix3.h"
#include "ocean/interaction/javascript/JSSquareMatrix4.h"
#include "ocean/interaction/javascript/JSVector2.h"
#include "ocean/interaction/javascript/JSVector3.h"
#include "ocean/interaction/javascript/JSVector4.h"

#include "ocean/base/Exception.h"
#include "ocean/base/Thread.h"

#include <v8.h>
#include <libplatform/libplatform.h>

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

using namespace v8;

JSLibrary::JSLibrary() :
	Library(nameJavaScriptLibrary()),
	isolate_(nullptr)
{
	registerFileExtension("js", "JavaScript interaction scripting file");

#if defined(OCEAN_V8_VERSION) && OCEAN_V8_VERSION > 70000
	platform_ = v8::platform::NewDefaultPlatform();
	v8::V8::InitializePlatform(platform_.get());
#else
	platform_ = std::unique_ptr<v8::Platform>(v8::platform::CreateDefaultPlatform());
	v8::V8::InitializePlatform(platform_.get());
#endif

	v8::V8::Initialize();

	createParams_.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();

	isolate_ = v8::Isolate::New(createParams_);
}

JSLibrary::~JSLibrary()
{
	ocean_assert(isolate_ != nullptr);

	while (!isolate_->IdleNotificationDeadline(0.1))
	{
		Thread::sleep(1u);
	}

	{
		const v8::Isolate::Scope isolateScope(isolate_);
		const v8::HandleScope handleScope(isolate_);

		jsContexts_.clear();

		while (!isolate_->IdleNotificationDeadline(0.1))
		{
			Thread::sleep(1u);
		}

		releaseGlobalTemplate();
	}

	isolate_->Dispose();
	isolate_ = nullptr;

	v8::V8::Dispose();

#if defined(OCEAN_V8_VERSION) && OCEAN_V8_VERSION >= 90900
	v8::V8::DisposePlatform();
#else
	v8::V8::ShutdownPlatform();
#endif

	delete createParams_.array_buffer_allocator;
}

void JSLibrary::registerLibrary()
{
	JSLibrary* library = new JSLibrary();
	if (library == nullptr)
	{
		throw OutOfMemoryException("Not enough memory to create a new java script interaction library.");
	}

	if (Library::registerFactory(*library) == false)
	{
		ocean_assert(false && "The library has been registered before!");
		delete library;
	}
}

bool JSLibrary::unregisterLibrary()
{
	return Library::unregisterLibrary(nameJavaScriptLibrary());
}

bool JSLibrary::load(const UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& filename)
{
	if (filename.empty())
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	ocean_assert(isolate_ != nullptr);

	const v8::Isolate::Scope isolateScope(isolate_);
	const v8::HandleScope handleScope(isolate_);

	std::shared_ptr<JSContext> context(std::make_shared<JSContext>());

	if (!context)
	{
		throw OutOfMemoryException("Not enough memory to create a new context.");
	}

	context->initialize(context, globalTemplate());

	if (context->addScriptFile(filename))
	{
		context->onInitialize(engine, timestamp);

		jsContexts_.emplace_back(std::move(context));

		return true;
	}

	return false;
}

bool JSLibrary::unload(const UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& filename)
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isolate_ != nullptr);

	const v8::Isolate::Scope isolateScope(isolate_);
	const v8::HandleScope handleScope(isolate_);

	for (JSContexts::iterator i = jsContexts_.begin(); i != jsContexts_.end(); ++i)
	{
		ocean_assert(*i);

		JSContext& jsContext = *i->get();

		for (const std::string& contextFilename : jsContext.filenames())
		{
			if (contextFilename == filename)
			{
				jsContext.onRelease(engine, timestamp);

				jsContexts_.erase(i);
				return true;
			}
		}
	}

	return false;
}

void JSLibrary::unload(const UserInterface& /*userInterface*/, const Rendering::EngineRef& /*engine*/, const Timestamp /*timestamp*/)
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isolate_ != nullptr);

	const v8::Isolate::Scope isolateScope(isolate_);
	const v8::HandleScope handleScope(isolate_);

	jsContexts_.clear();
}

void JSLibrary::preFileLoad(const UserInterface& /*userInterface*/, const std::string& filename)
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isolate_ != nullptr);

	const v8::Isolate::Scope isolateScope(isolate_);
	const v8::HandleScope handleScope(isolate_);

	for (std::shared_ptr<JSContext>& jsContext : jsContexts_)
	{
		ocean_assert(jsContext);
		jsContext->preFileLoad(filename);
	}
}

void JSLibrary::postFileLoad(const UserInterface& /*userInterface*/, const std::string& filename, const bool succeeded)
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isolate_ != nullptr);

	const v8::Isolate::Scope isolateScope(isolate_);
	const v8::HandleScope handleScope(isolate_);

	for (std::shared_ptr<JSContext>& jsContext : jsContexts_)
	{
		ocean_assert(jsContext);
		jsContext->postFileLoad(filename, succeeded);
	}
}

Timestamp JSLibrary::preUpdate(const UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Rendering::ViewRef& /*view*/, const Timestamp timestamp)
{
	ocean_assert(engine.isNull() == false);

	const ScopedLock scopedLock(lock_);

	ocean_assert(isolate_ != nullptr);

	const v8::Isolate::Scope isolateScope(isolate_);
	const v8::HandleScope handleScope(isolate_);

	Timestamp changedTimestamp = timestamp;

	for (std::shared_ptr<JSContext>& jsContext : jsContexts_)
	{
		ocean_assert(jsContext);
		changedTimestamp = jsContext->preUpdate(engine, changedTimestamp);
	}

	isolate_->AdjustAmountOfExternalAllocatedMemory(512 * 1024 * 1024); // forcing GC to run (at least a good hint)

	return changedTimestamp;
}

void JSLibrary::postUpdate(const UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Rendering::ViewRef& /*view*/, const Timestamp timestamp)
{
	ocean_assert(engine.isNull() == false);

	const ScopedLock scopedLock(lock_);

	ocean_assert(isolate_ != nullptr);

	const v8::Isolate::Scope isolateScope(isolate_);
	const v8::HandleScope handleScope(isolate_);

	for (std::shared_ptr<JSContext>& jsContext : jsContexts_)
	{
		ocean_assert(jsContext);
		jsContext->postUpdate(engine, timestamp);
	}

	isolate_->AdjustAmountOfExternalAllocatedMemory(512 * 1024 * 1024); // forcing GC to run (at least a good hint)
}

void JSLibrary::onMousePress(const UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const Rendering::ObjectId pickedObject, const Vector3& pickedPosition, const Timestamp timestamp)
{
	ocean_assert(engine.isNull() == false);

	const ScopedLock scopedLock(lock_);

	const std::string objectName(translatePickingObject(engine, pickedObject));

	ocean_assert(isolate_ != nullptr);

	const v8::Isolate::Scope isolateScope(isolate_);
	const v8::HandleScope handleScope(isolate_);

	for (std::shared_ptr<JSContext>& jsContext : jsContexts_)
	{
		ocean_assert(jsContext);
		jsContext->onMousePress(engine, button, screenPosition, ray, objectName, pickedPosition, timestamp);
	}
}

void JSLibrary::onMouseMove(const UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const Rendering::ObjectId pickedObject, const Vector3& pickedPosition, const Timestamp timestamp)
{
	ocean_assert(engine.isNull() == false);

	const ScopedLock scopedLock(lock_);

	const std::string objectName(translatePickingObject(engine, pickedObject));

	ocean_assert(isolate_ != nullptr);

	const v8::Isolate::Scope isolateScope(isolate_);
	const v8::HandleScope handleScope(isolate_);

	for (std::shared_ptr<JSContext>& jsContext : jsContexts_)
	{
		ocean_assert(jsContext);
		jsContext->onMouseMove(engine, button, screenPosition, ray, objectName, pickedPosition, timestamp);
	}
}

void JSLibrary::onMouseRelease(const UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const Rendering::ObjectId pickedObject, const Vector3& pickedPosition, const Timestamp timestamp)
{
	ocean_assert(engine.isNull() == false);

	const ScopedLock scopedLock(lock_);

	const std::string objectName(translatePickingObject(engine, pickedObject));

	ocean_assert(isolate_ != nullptr);

	const v8::Isolate::Scope isolateScope(isolate_);
	const v8::HandleScope handleScope(isolate_);

	for (std::shared_ptr<JSContext>& jsContext : jsContexts_)
	{
		ocean_assert(jsContext);
		jsContext->onMouseRelease(engine, button, screenPosition, ray, objectName, pickedPosition, timestamp);
	}
}

void JSLibrary::onKeyPress(const UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp)
{
	ocean_assert(key.length() != 0);

	const ScopedLock scopedLock(lock_);

	ocean_assert(isolate_ != nullptr);

	const v8::Isolate::Scope isolateScope(isolate_);
	const v8::HandleScope handleScope(isolate_);

	for (std::shared_ptr<JSContext>& jsContext : jsContexts_)
	{
		ocean_assert(jsContext);
		jsContext->onKeyPress(engine, key, timestamp);
	}
}

void JSLibrary::onKeyRelease(const UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp)
{
	ocean_assert(key.length() != 0);

	const ScopedLock scopedLock(lock_);

	ocean_assert(isolate_ != nullptr);

	const v8::Isolate::Scope isolateScope(isolate_);
	const v8::HandleScope handleScope(isolate_);

	for (std::shared_ptr<JSContext>& jsContext : jsContexts_)
	{
		ocean_assert(jsContext);
		jsContext->onKeyRelease(engine, key, timestamp);
	}
}

v8::Local<v8::ObjectTemplate> JSLibrary::globalTemplate()
{
	if (globalTemplate_.IsEmpty())
	{
		v8::Isolate* isolate = v8::Isolate::GetCurrent();

		v8::Local<v8::ObjectTemplate> globalTemplate = v8::ObjectTemplate::New(isolate);

		// Global object templates
		globalTemplate->Set(JSBase::newString(JSOcean::objectName(), isolate), JSOcean::objectTemplate());

		// Global function templates
		globalTemplate->Set(JSBase::newString(JSColor::objectName(), isolate), JSColor::functionTemplate());
		globalTemplate->Set(JSBase::newString(JSDeviceObject::objectName(), isolate), JSDeviceObject::functionTemplate());
		globalTemplate->Set(JSBase::newString(JSDeviceSample::objectName(), isolate), JSDeviceSample::functionTemplate());
		globalTemplate->Set(JSBase::newString(JSEuler::objectName(), isolate), JSEuler::functionTemplate());
		globalTemplate->Set(JSBase::newString(JSHomogenousMatrix4::objectName(), isolate), JSHomogenousMatrix4::functionTemplate());
		globalTemplate->Set(JSBase::newString(JSLine3::objectName(), isolate), JSLine3::functionTemplate());
		globalTemplate->Set(JSBase::newString(JSMediaObject::objectName(), isolate), JSMediaObject::functionTemplate());
		globalTemplate->Set(JSBase::newString(JSPlane3::objectName(), isolate), JSPlane3::functionTemplate());
		globalTemplate->Set(JSBase::newString(JSQuaternion::objectName(), isolate), JSQuaternion::functionTemplate());
		globalTemplate->Set(JSBase::newString(JSRenderingEngine::objectName(), isolate), JSRenderingEngine::functionTemplate());
		globalTemplate->Set(JSBase::newString(JSRenderingObject::objectName(), isolate), JSRenderingObject::functionTemplate());
		globalTemplate->Set(JSBase::newString(JSRotation::objectName(), isolate), JSRotation::functionTemplate());
		globalTemplate->Set(JSBase::newString(JSSceneDescriptionNode::objectName(), isolate), JSSceneDescriptionNode::functionTemplate());
		globalTemplate->Set(JSBase::newString(JSSquareMatrix3::objectName(), isolate), JSSquareMatrix3::functionTemplate());
		globalTemplate->Set(JSBase::newString(JSSquareMatrix4::objectName(), isolate), JSSquareMatrix4::functionTemplate());
		globalTemplate->Set(JSBase::newString(JSVector2::objectName(), isolate), JSVector2::functionTemplate());
		globalTemplate->Set(JSBase::newString(JSVector3::objectName(), isolate), JSVector3::functionTemplate());
		globalTemplate->Set(JSBase::newString(JSVector4::objectName(), isolate), JSVector4::functionTemplate());

		globalTemplate->Set(JSBase::newString("include", isolate), FunctionTemplate::New(isolate, functionLoad));
		globalTemplate->Set(JSBase::newString("load", isolate), FunctionTemplate::New(isolate, functionLoad));

		globalTemplate_.Reset(isolate, globalTemplate);
	}

	ocean_assert(globalTemplate_.IsEmpty() == false);
	return globalTemplate_.Get(v8::Isolate::GetCurrent());
}

void JSLibrary::releaseGlobalTemplate()
{
	if (globalTemplate_.IsEmpty())
	{
		return;
	}

	JSVector4::release();
	JSVector3::release();
	JSVector2::release();
	JSSquareMatrix4::release();
	JSSquareMatrix3::release();
	JSSceneDescriptionNode::release();
	JSRotation::release();
	JSRenderingObject::release();
	JSRenderingEngine::release();
	JSQuaternion::release();
	JSPlane3::release();
	JSMediaObject::release();
	JSLine3::release();
	JSHomogenousMatrix4::release();
	JSEuler::release();
	JSDeviceSample::release();
	JSDeviceObject::release();
	JSColor::release();

	JSOcean::release();

	globalTemplate_.Reset();
}

void JSLibrary::functionLoad(const v8::FunctionCallbackInfo<v8::Value>& info)
{
	if (info.Length() == 0)
	{
		info.GetReturnValue().Set(false);
		return;
	}

	ocean_assert(false && "TODO");

	/*

	v8::Local<v8::Value> uniqueContextId = v8::Context::GetCurrent()->Global()->Get(v8::String::New("__UNIQUE_CONTEXT_ID__"));
	ocean_assert(uniqueContextId.IsEmpty() == false && uniqueContextId->IsInt32());

	JSContext* contextObject = JSContext::context((unsigned int)(uniqueContextId->Int32Value()));
	ocean_assert(contextObject != nullptr);

	bool fileLoaded = false;
	for (int n = 0; n < args.Length(); n++)
	{
		if (args[n]->IsString())
		{
			v8::String::Utf8Value value(args[n]);
			const std::string filename(*value);

			const IO::Files resolvedFiles(JSBase::resolveFile(IO::File(filename)));

			for (IO::Files::const_iterator i = resolvedFiles.begin(); i != resolvedFiles.end(); ++i)
			{
				if (i->exists() && contextObject->addScriptFile((*i)()))
				{
					fileLoaded = true;
					break;
				}
			}
		}

		return v8::Boolean::New(fileLoaded);
	}
	*/
}

std::string JSLibrary::translatePickingObject(const Rendering::EngineRef& engine, const Rendering::ObjectId objectId)
{
	ocean_assert(engine);

	Rendering::ObjectRef object(engine->object(objectId));

	if (!object)
	{
		return std::string();
	}

	if (!object->name().empty())
	{
		return object->name();
	}

	while (object)
	{
		const Rendering::ObjectRefSet parents(object->parentNodes());

		for (Rendering::ObjectRefSet::const_iterator i = parents.begin(); i != parents.end(); ++i)
		{
			if (!(*i)->name().empty())
			{
				return (*i)->name();
			}
		}

		if (parents.empty())
		{
			object.release();
		}
		else
		{
			object = *parents.begin();
		}
	}

	return std::string();
}

}

}

}
