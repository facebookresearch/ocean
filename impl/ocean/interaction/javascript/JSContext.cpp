/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/interaction/javascript/JSContext.h"

#include "ocean/base/Exception.h"
#include "ocean/base/String.h"
#include "ocean/base/Thread.h"

#include "ocean/interaction/javascript/JSLibrary.h"
#include "ocean/interaction/javascript/JSLine3.h"
#include "ocean/interaction/javascript/JSRenderingEngine.h"
#include "ocean/interaction/javascript/JSVector2.h"
#include "ocean/interaction/javascript/JSVector3.h"

#include "ocean/interaction/Manager.h"

#include <istream>
#include <fstream>

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

v8::Local<v8::Context> JSContext::currentContext_;

std::weak_ptr<JSContext> JSContext::currentJSContext_;

unsigned int JSContext::mouseEventCounter_ = 0u;

JSContext::~JSContext()
{
	const ScopedLock scopedLock(lock_);

	makeCurrent();

	v8::Context::Scope contextScope(context());

	if (hasMousePressEventFunction_ || hasMouseMoveEventFunction_ || hasMouseReleaseEventFunction_)
	{
		ocean_assert(mouseEventCounter_ > 0u);
		--mouseEventCounter_;

		if (mouseEventCounter_ == 0u)
		{
			Manager::get().unregisterMouseEventLibrary(nameJavaScriptLibrary());
		}
	}

	scripts_.clear();

	v8::Isolate::GetCurrent()->AdjustAmountOfExternalAllocatedMemory(512 * 1024 * 1024); // forcing GC to run (at least a good hint)

	while (!v8::Isolate::GetCurrent()->IdleNotificationDeadline(0.1))
	{
		Thread::sleep(1u);
	}

	currentContext_.Clear();
	currentJSContext_.reset();
}

void JSContext::initialize(const std::shared_ptr<JSContext>& thisContext, const v8::Local<v8::ObjectTemplate>& globalTemplate)
{
	ocean_assert(thisContext.get() == this);

	thisJSContext_ = thisContext;

	makeCurrent();

	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	context_ = v8::UniquePersistent<v8::Context>(isolate, v8::Context::New(isolate, nullptr /*extensions*/, globalTemplate));
}

bool JSContext::addScriptCode(const std::string& code)
{
	if (code.empty())
	{
		ocean_assert(false && "Invalid code!");
		return false;
	}

	const ScopedLock scopedLock(lock_);

	ocean_assert(!context_.IsEmpty());
	if (context_.IsEmpty())
	{
		return false;
	}

	makeCurrent();

	v8::Context::Scope contextScope(context());

	std::unique_ptr<JSScript> newScript(std::make_unique<JSScript>());
	ocean_assert(newScript);

	if (!newScript)
	{
		return false;
	}

	try
	{
		std::string errorMessage;
		if (newScript->compileAndRun(code, errorMessage) == false)
		{
			Log::error() << "Failed to compile the script file:";
			Log::error() << errorMessage;

			return false;
		}

		scripts_.emplace_back(std::move(newScript));
	}
	catch(...)
	{
		ocean_assert(false && "This exception should be caught earlier!");
	}

	return true;
}

bool JSContext::addScriptFile(const std::string& filename)
{
	if (filename.empty())
	{
		ocean_assert(false && "Invalid filename!");
		return false;
	}

	std::ifstream stream;
	stream.open(filename.c_str(), std::ios::binary);

	if (stream.fail())
	{
		Log::error() << "Failed to load script file \"" << filename << "\".";
		return false;
	}

	stream.seekg(0, std::ios_base::end);
	const std::istream::pos_type fileSize = stream.tellg();
	stream.seekg(0, std::ios_base::beg);

	if (fileSize == std::istream::pos_type(0))
	{
		Log::error() << "The script file is empty \"" << filename << "\".";
		return false;
	}

	filenames_.emplace_back(filename);

	const std::string::size_type codeSize = std::string::size_type(fileSize);
	std::string code(codeSize, '\0');

	stream.read(&code[0], fileSize);
	ocean_assert(stream.good());

	return addScriptCode(code);
}

void JSContext::onInitialize(const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	ocean_assert(engine.isNull() == false);

	const ScopedLock scopedLock(lock_);

	ocean_assert(!context_.IsEmpty());

	makeCurrent();

	const v8::Context::Scope contextScope(context());

	hasPreUpdateFunction_ = hasFunction("onOceanPreUpdate");
	hasPostUpdateFunction_ = hasFunction("onOceanPostUpdate");

	hasKeyPressFunction_ = hasFunction("onOceanKeyPress");
	hasKeyReleaseFunction_ = hasFunction("onOceanKeyRelease");

	hasMousePressEventFunction_ = hasFunction("onOceanMousePress");
	hasMouseMoveEventFunction_ = hasFunction("onOceanMouseMove");
	hasMouseReleaseEventFunction_ = hasFunction("onOceanMouseRelease");

	if (hasMousePressEventFunction_ || hasMouseMoveEventFunction_ || hasMouseReleaseEventFunction_)
	{
		++mouseEventCounter_;

		if (mouseEventCounter_ == 1u)
		{
			Manager::get().registerMouseEventLibrary(nameJavaScriptLibrary());
		}
	}

	if (hasFunction("onOceanInitialize"))
	{
		v8::Local<v8::Value> returnValue;
		std::string errorMessage;

		Values parameters =
		{
			JSRenderingEngine::create(Rendering::EngineRef(engine), currentContext()),
			v8::Number::New(v8::Isolate::GetCurrent(), double(timestamp))
		};

		if (callFunction(std::string("onOceanInitialize"), returnValue, parameters, errorMessage) == false)
		{
			Log::error() << "Failed to run \"onOceanInitialize()\" function: " << errorMessage;
		}
	}
}

void JSContext::onRelease(const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	ocean_assert(engine.isNull() == false);

	const ScopedLock scopedLock(lock_);

	ocean_assert(!context_.IsEmpty());

	makeCurrent();

	const v8::Context::Scope contextScope(context());

	if (hasFunction("onOceanRelease"))
	{
		v8::Local<v8::Value> returnValue;
		std::string errorMessage;

		Values parameters =
		{
			JSRenderingEngine::create(Rendering::EngineRef(engine), currentContext()),
			v8::Number::New(v8::Isolate::GetCurrent(), double(timestamp))
		};

		if (callFunction(std::string("onOceanRelease"), returnValue, parameters, errorMessage) == false)
		{
			Log::error() << "Failed to run \"onOceanRelease()\" function: " << errorMessage;
		}
	}
}

bool JSContext::callFunction(v8::Local<v8::Function> function, v8::Local<v8::Value>& returnValue, Values& parameters, std::string& errorMessage)
{
	ocean_assert(!function.IsEmpty());

	const ScopedLock scopedLock(lock_);

	ocean_assert(!context_.IsEmpty());

	makeCurrent();

	const v8::Context::Scope contextScope(context());

	try
	{
		const v8::TryCatch tryCatch(v8::Isolate::GetCurrent());

		v8::MaybeLocal<v8::Value> result;

		if (parameters.empty())
		{
			result = function->Call(currentContext(), function, 0, nullptr);
		}
		else
		{
			result = function->Call(currentContext(), function, int(parameters.size()), parameters.data());
		}

		if (!result.IsEmpty())
		{
			returnValue = result.ToLocalChecked();
		}

		if (tryCatch.HasCaught())
		{
			errorMessage = JSScript::extractErrorMessage(tryCatch);
			return false;
		}

		return true;
	}
	catch(...)
	{
		ocean_assert(false && "This exception should be caught earlier!");
	}

	return false;
}

bool JSContext::callFunction(const std::string& function, v8::Local<v8::Value>& returnValue, Values& parameters, std::string& errorMessage)
{
	ocean_assert(!function.empty());

	const ScopedLock scopedLock(lock_);

	ocean_assert(!context_.IsEmpty());

	makeCurrent();

	const v8::Context::Scope contextScope(context());

	try
	{
		v8::MaybeLocal<v8::Value> maybeFunction = context()->Global()->Get(currentContext(), JSBase::newString(function.c_str(), v8::Isolate::GetCurrent()));

		if (maybeFunction.IsEmpty())
		{
			return false;
		}

		v8::Local<v8::Function> functionHandle(v8::Local<v8::Function>::Cast(maybeFunction.ToLocalChecked()));

		if (functionHandle.IsEmpty())
		{
			return false;
		}

		const v8::TryCatch tryCatch(v8::Isolate::GetCurrent());

		v8::MaybeLocal<v8::Value> result;

		if (parameters.empty())
		{
			result = functionHandle->Call(currentContext(), functionHandle, 0, nullptr);
		}
		else
		{
			result = functionHandle->Call(currentContext(), functionHandle, int(parameters.size()), parameters.data());
		}

		if (!result.IsEmpty())
		{
			returnValue = result.ToLocalChecked();
		}

		if (tryCatch.HasCaught())
		{
			errorMessage = JSScript::extractErrorMessage(tryCatch);
			return false;
		}

		return true;
	}
	catch(...)
	{
		ocean_assert(false && "This exception should be caught earlier!");
	}

	return false;
}

void JSContext::addExplicitFunctionCall(const std::string& function, const FunctionParameterCaller& parameterCaller)
{
	const ScopedLock scopedLock(lock_);

	explicitFunctions_.emplace_back(function, parameterCaller);
}

IO::Files JSContext::resolveFile(const IO::File& file)
{
	std::set<IO::File> resolvedFiles;

	if (filenames_.empty())
	{
		const IO::Files resolved(IO::FileResolver::get().resolve(file));
		resolvedFiles.insert(resolved.cbegin(), resolved.cend());
	}
	else
	{
		for (const std::string& filename : filenames_)
		{
			const IO::Files resolved(IO::FileResolver::get().resolve(file, IO::File(filename)));
			resolvedFiles.insert(resolved.cbegin(), resolved.cend());
		}
	}

	return IO::Files(resolvedFiles.cbegin(), resolvedFiles.cend());
}

bool JSContext::resolveFile(const IO::File& file, std::string& resolved)
{
	const IO::Files resolvedFiles(resolveFile(file));

	for (const IO::File& fileFile : resolvedFiles)
	{
		if (fileFile.exists())
		{
			resolved = fileFile();
			return true;
		}
	}

	return false;
}

void JSContext::preFileLoad(const std::string& /*filename*/)
{
	// nothing to do here
}

void JSContext::postFileLoad(const std::string& /*filename*/, const bool /*succeeded*/)
{
	// nothing to do here
}

Timestamp JSContext::preUpdate(const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	ocean_assert(engine);

	const ScopedLock scopedLock(lock_);

	ocean_assert(!context_.IsEmpty());

	makeCurrent();

	const v8::Context::Scope contextScope(context());

	for (const FunctionPair& explicitFunction : explicitFunctions_)
	{
		v8::Local<v8::Value> returnValue;
		std::string errorMessage;

		Values parameters(explicitFunction.second());

		if (callFunction(explicitFunction.first, returnValue, parameters, errorMessage) == false)
		{
			Log::error() << errorMessage;
		}
	}

	explicitFunctions_.clear();

	if (hasPreUpdateFunction_)
	{
		ocean_assert(engine.isNull() == false);

		ocean_assert(hasFunction("onOceanPreUpdate"));

		v8::Local<v8::Value> returnValue;
		std::string errorMessage;

		Values parameters =
		{
			JSRenderingEngine::create(Rendering::EngineRef(engine), currentContext()),
			v8::Number::New(v8::Isolate::GetCurrent(), double(timestamp))
		};

		if (callFunction(std::string("onOceanPreUpdate"), returnValue, parameters, errorMessage) == false)
		{
			Log::error() << "Failed to run \"onOceanPreUpdate()\" function: " << errorMessage;
		}

		if (returnValue.IsEmpty() == false && returnValue->IsNumber())
		{
			const Timestamp newTimestamp(returnValue->NumberValue(currentContext()).FromJust());

			if (double(newTimestamp) > 0.0)
			{
				return newTimestamp;
			}
		}
	}

	return timestamp;
}

void JSContext::postUpdate(const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	ocean_assert(engine);

	const ScopedLock scopedLock(lock_);

	ocean_assert(!context_.IsEmpty());

	if (hasPostUpdateFunction_)
	{
		makeCurrent();

		const v8::Context::Scope contextScope(context());

		ocean_assert(hasFunction("onOceanPostUpdate"));

		v8::Local<v8::Value> returnValue;
		std::string errorMessage;

		Values parameters =
		{
			JSRenderingEngine::create(Rendering::EngineRef(engine), currentContext()),
			v8::Number::New(v8::Isolate::GetCurrent(), double(timestamp))
		};

		if (callFunction(std::string("onOceanPostUpdate"), returnValue, parameters, errorMessage) == false)
		{
			Log::error() << "Failed to run \"onOceanPostUpdate()\" function: " << errorMessage;
		}
	}
}

void JSContext::onMousePress(const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const std::string& pickedObject, const Vector3& pickedPosition, const Timestamp timestamp)
{
	ocean_assert(engine);
	ocean_assert(!button.empty());

	const ScopedLock scopedLock(lock_);

	ocean_assert(!context_.IsEmpty());

	if (hasMousePressEventFunction_)
	{
		makeCurrent();

		const v8::Context::Scope contextScope(context());

		ocean_assert(hasFunction("onOceanMousePress"));

		v8::Local<v8::Value> returnValue;
		std::string errorMessage;

		Values parameters =
		{
			JSRenderingEngine::create(Rendering::EngineRef(engine), currentContext()),
			JSBase::newString(button, v8::Isolate::GetCurrent()),
			JSVector2::create(Vector2(screenPosition), currentContext()),
			JSLine3::create(Line3(ray), currentContext()),
			JSBase::newString(pickedObject, v8::Isolate::GetCurrent()),
			JSVector3::create(Vector3(pickedPosition), currentContext()),
			v8::Number::New(v8::Isolate::GetCurrent(), double(timestamp))
		};

		if (callFunction(std::string("onOceanMousePress"), returnValue, parameters, errorMessage) == false)
		{
			Log::error() << "Failed to run \"onOceanMousePress()\" function: " << errorMessage;
		}
	}
}

void JSContext::onMouseMove(const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const std::string& pickedObject, const Vector3& pickedPosition, const Timestamp timestamp)
{
	ocean_assert(engine);
	ocean_assert(!button.empty());

	const ScopedLock scopedLock(lock_);

	ocean_assert(!context_.IsEmpty());

	if (hasMouseMoveEventFunction_)
	{
		makeCurrent();

		const v8::Context::Scope contextScope(context());

		ocean_assert(hasFunction("onOceanMouseMove"));

		v8::Local<v8::Value> returnValue;
		std::string errorMessage;

		Values parameters =
		{
			JSRenderingEngine::create(Rendering::EngineRef(engine), currentContext()),
			JSBase::newString(button, v8::Isolate::GetCurrent()),
			JSVector2::create(Vector2(screenPosition), currentContext()),
			JSLine3::create(Line3(ray), currentContext()),
			JSBase::newString(pickedObject, v8::Isolate::GetCurrent()),
			JSVector3::create(Vector3(pickedPosition), currentContext()),
			v8::Number::New(v8::Isolate::GetCurrent(), double(timestamp))
		};

		if (callFunction(std::string("onOceanMouseMove"), returnValue, parameters, errorMessage) == false)
		{
			Log::error() << "Failed to run \"onOceanMouseMove()\" function: " << errorMessage;
		}
	}
}

void JSContext::onMouseRelease(const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const std::string& pickedObject, const Vector3& pickedPosition, const Timestamp timestamp)
{
	ocean_assert(engine);
	ocean_assert(!button.empty());

	const ScopedLock scopedLock(lock_);

	ocean_assert(!context_.IsEmpty());

	if (hasMouseReleaseEventFunction_)
	{
		makeCurrent();

		const v8::Context::Scope contextScope(context());

		ocean_assert(hasFunction("onOceanMouseRelease"));

		v8::Local<v8::Value> returnValue;
		std::string errorMessage;

		Values parameters =
		{
			JSRenderingEngine::create(Rendering::EngineRef(engine), currentContext()),
			JSBase::newString(button, v8::Isolate::GetCurrent()),
			JSVector2::create(Vector2(screenPosition), currentContext()),
			JSLine3::create(Line3(ray), currentContext()),
			JSBase::newString(pickedObject, v8::Isolate::GetCurrent()),
			JSVector3::create(Vector3(pickedPosition), currentContext()),
			v8::Number::New(v8::Isolate::GetCurrent(), double(timestamp))
		};

		if (callFunction(std::string("onOceanMouseRelease"), returnValue, parameters, errorMessage) == false)
		{
			Log::error() << "Failed to run \"onOceanMouseRelease()\" function: " << errorMessage;
		}
	}
}

void JSContext::onKeyPress(const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp)
{
	ocean_assert(engine);
	ocean_assert(!key.empty());

	const ScopedLock scopedLock(lock_);

	ocean_assert(!context_.IsEmpty());

	if (hasKeyPressFunction_)
	{
		makeCurrent();

		const v8::Context::Scope contextScope(context());

		ocean_assert(hasFunction("onOceanKeyPress"));

		v8::Local<v8::Value> returnValue;
		std::string errorMessage;

		Values parameters =
		{
			JSRenderingEngine::create(Rendering::EngineRef(engine), currentContext()),
			JSBase::newString(key, v8::Isolate::GetCurrent()),
			v8::Number::New(v8::Isolate::GetCurrent(), double(timestamp))
		};

		if (callFunction(std::string("onOceanKeyPress"), returnValue, parameters, errorMessage) == false)
		{
			Log::error() << "Failed to run \"onOceanKeyPress()\" function: " << errorMessage;
		}
	}
}

void JSContext::onKeyRelease(const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp)
{
	ocean_assert(engine);
	ocean_assert(!key.empty());

	const ScopedLock scopedLock(lock_);

	ocean_assert(!context_.IsEmpty());

	if (hasKeyReleaseFunction_)
	{
		makeCurrent();

		const v8::Context::Scope contextScope(context());

		ocean_assert(hasFunction("onOceanKeyRelease"));

		v8::Local<v8::Value> returnValue;
		std::string errorMessage;

		Values parameters =
		{
			JSRenderingEngine::create(Rendering::EngineRef(engine), currentContext()),
			JSBase::newString(key, v8::Isolate::GetCurrent()),
			v8::Number::New(v8::Isolate::GetCurrent(), double(timestamp))
		};

		if (callFunction(std::string("onOceanKeyRelease"), returnValue, parameters, errorMessage) == false)
		{
			Log::error() << "Failed to run \"onOceanKeyRelease()\" function: " << errorMessage;
		}
	}
}

void JSContext::makeCurrent()
{
	currentContext_ = context();
	currentJSContext_ = thisJSContext_;
}

bool JSContext::hasFunction(const std::string& function) const
{
	// Context scoped must have been created already

	ocean_assert(!function.empty());
	ocean_assert(!context_.IsEmpty());

	v8::MaybeLocal<v8::Value> scriptObject(context()->Global()->Get(currentContext(), JSBase::newString(function.c_str(), v8::Isolate::GetCurrent())));

	return !scriptObject.IsEmpty() && scriptObject.ToLocalChecked()->IsFunction();
}

bool JSContext::hasObject(const std::string& object) const
{
	// Context scoped must have been created already

	ocean_assert(!object.empty());
	ocean_assert(!context_.IsEmpty());

	v8::MaybeLocal<v8::Value> scriptObject(context()->Global()->Get(currentContext(), JSBase::newString(object.c_str(), v8::Isolate::GetCurrent())));

	return !scriptObject.IsEmpty() && !scriptObject.ToLocalChecked()->IsFunction();
}

}

}

}
