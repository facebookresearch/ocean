/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/interaction/javascript/JSOcean.h"
#include "ocean/interaction/javascript/JSContext.h"
#include "ocean/interaction/javascript/JSDeviceManager.h"
#include "ocean/interaction/javascript/JSMediaManager.h"
#include "ocean/interaction/javascript/JSSceneDescriptionManager.h"

#include "ocean/base/Build.h"
#include "ocean/base/String.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

v8::Persistent<v8::ObjectTemplate> JSOcean::objectTemplate_;

JSOcean::OceanContainer::~OceanContainer()
{
	release();
}

void JSOcean::OceanContainer::addContentAddedEventFunction(const v8::Local<v8::Function>& function)
{
	ocean_assert(!function.IsEmpty());

	std::string functionName = toAString(function->GetName()->ToString(JSContext::currentContext()));
	ocean_assert(!functionName.empty());

	const ScopedLock scopedLock(lock_);

	if (contentAddedEventFunctions_.empty() && contentRemovedEventFunctions_.empty())
	{
		ApplicationInterface::get().addEventCallbackFunction(ApplicationInterface::EventCallback::create(*this, &OceanContainer::onContentEvent));
	}

	contentAddedEventFunctions_.emplace_back(JSContext::currentJSContext(), std::move(functionName));
}

void JSOcean::OceanContainer::addContentRemovedEventFunction(const v8::Local<v8::Function>& function)
{
	ocean_assert(!function.IsEmpty());

	std::string functionName = toAString(function->GetName()->ToString(JSContext::currentContext()));
	ocean_assert(!functionName.empty());

	const ScopedLock scopedLock(lock_);

	if (contentAddedEventFunctions_.empty() && contentRemovedEventFunctions_.empty())
	{
		ApplicationInterface::get().addEventCallbackFunction(ApplicationInterface::EventCallback::create(*this, &OceanContainer::onContentEvent));
	}

	contentRemovedEventFunctions_.emplace_back(JSContext::currentJSContext(), std::move(functionName));
}

void JSOcean::OceanContainer::removeContentAddedEventFunction(const v8::Local<v8::Function>& function)
{
	const std::string functionName = toAString(function->GetName()->ToString(JSContext::currentContext()));
	ocean_assert(!functionName.empty());

	const std::shared_ptr<JSContext> currentContext(JSContext::currentJSContext());
	ocean_assert(currentContext);

	const ScopedLock scopedLock(lock_);

	for (ContextFunctionPairs::iterator i = contentAddedEventFunctions_.begin(); i != contentAddedEventFunctions_.end(); ++i)
	{
		if (functionName == i->second)
		{
			const std::shared_ptr<JSContext> functionContext(i->first);
			ocean_assert(functionContext);

			if (currentContext.get() == functionContext.get())
			{
				contentAddedEventFunctions_.erase(i);

				if (contentAddedEventFunctions_.empty() && contentRemovedEventFunctions_.empty())
				{
					ApplicationInterface::get().removeEventCallbackFunction(ApplicationInterface::EventCallback::create(*this, &OceanContainer::onContentEvent));
				}

				return;
			}
		}
	}

	ocean_assert(false && "Unknown function.");
}

void JSOcean::OceanContainer::removeContentRemovedEventFunction(const v8::Local<v8::Function>& function)
{
	const std::string functionName = toAString(function->GetName()->ToString(JSContext::currentContext()));
	ocean_assert(!functionName.empty());

	const std::shared_ptr<JSContext> currentContext(JSContext::currentJSContext());
	ocean_assert(currentContext);

	const ScopedLock scopedLock(lock_);

	for (ContextFunctionPairs::iterator i = contentRemovedEventFunctions_.begin(); i != contentRemovedEventFunctions_.end(); ++i)
	{
		if (functionName == i->second)
		{
			const std::shared_ptr<JSContext> functionContext(i->first);
			ocean_assert(functionContext);

			if (currentContext.get() == functionContext.get())
			{
				contentRemovedEventFunctions_.erase(i);

				if (contentAddedEventFunctions_.empty() && contentRemovedEventFunctions_.empty())
				{
					ApplicationInterface::get().removeEventCallbackFunction(ApplicationInterface::EventCallback::create(*this, &OceanContainer::onContentEvent));
				}

				return;
			}
		}
	}

	ocean_assert(false && "Unknown function.");
}

void JSOcean::OceanContainer::release()
{
	const ScopedLock scopedLock(lock_);

	if (!contentAddedEventFunctions_.empty() || !contentRemovedEventFunctions_.empty())
	{
		ApplicationInterface::get().removeEventCallbackFunction(ApplicationInterface::EventCallback::create(*this, &OceanContainer::onContentEvent));
	}

	contentAddedEventFunctions_.clear();
	contentRemovedEventFunctions_.clear();
}

void JSOcean::OceanContainer::onContentEvent(const ApplicationInterface::EventId eventId, const bool value, const bool state)
{
	const ScopedLock scopedLock(lock_);

	if (value)
	{
		// content loaded
		for (ContextFunctionPairs::iterator i = contentAddedEventFunctions_.begin(); i != contentAddedEventFunctions_.end(); ++i)
		{
			std::shared_ptr<JSContext> functionContext(i->first);
			ocean_assert(functionContext);

			if (functionContext)
			{
				const v8::Context::Scope contextScope(functionContext->context());

				functionContext->addExplicitFunctionCall(i->second, JSContext::FunctionParameterCaller::createStatic(&OceanContainer::parameter, eventId, state));
			}
		}
	}
	else
	{
		// content unloaded
		for (ContextFunctionPairs::iterator i = contentRemovedEventFunctions_.begin(); i != contentRemovedEventFunctions_.end(); ++i)
		{
			std::shared_ptr<JSContext> functionContext(i->first);
			ocean_assert(functionContext);

			if (functionContext)
			{
				const v8::Context::Scope contextScope(functionContext->context());

				functionContext->addExplicitFunctionCall(i->second, JSContext::FunctionParameterCaller::createStatic(&OceanContainer::parameter, eventId, state));
			}
		}
	}
}

std::vector< v8::Handle<v8::Value> > JSOcean::OceanContainer::parameter(const ApplicationInterface::EventId eventId, const bool state)
{
	JSContext::Values values;
	values.reserve(2);

	values.push_back(v8::Int32::New(v8::Isolate::GetCurrent(), int(eventId)));
	values.push_back(v8::Boolean::New(v8::Isolate::GetCurrent(), state));

	return values;
}

void JSOcean::createObjectTemplate()
{
	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	v8::Local<v8::ObjectTemplate> objectTemplate(v8::ObjectTemplate::New(isolate));

	objectTemplate->Set(newString("device", isolate), JSDeviceManager::objectTemplate());
	objectTemplate->Set(newString("media", isolate), JSMediaManager::objectTemplate());
	objectTemplate->Set(newString("sceneDescription", isolate), JSSceneDescriptionManager::objectTemplate());

	objectTemplate->Set(newString("debug", isolate), v8::FunctionTemplate::New(isolate, functionDebug));
	objectTemplate->Set(newString("info", isolate), v8::FunctionTemplate::New(isolate, functionInfo));
	objectTemplate->Set(newString("warning", isolate), v8::FunctionTemplate::New(isolate, functionWarning));
	objectTemplate->Set(newString("error", isolate), v8::FunctionTemplate::New(isolate, functionError));

	objectTemplate->Set(newString("platformType", isolate), v8::FunctionTemplate::New(isolate, functionPlatformType));
	objectTemplate->Set(newString("architectureType", isolate), v8::FunctionTemplate::New(isolate, functionArchitectureType));
	objectTemplate->Set(newString("releaseType", isolate), v8::FunctionTemplate::New(isolate, functionReleaseType));
	objectTemplate->Set(newString("compilerVersion", isolate), v8::FunctionTemplate::New(isolate, functionCompilerVersion));

	objectTemplate->Set(newString("addContent", isolate), v8::FunctionTemplate::New(isolate, functionAddContent));
	objectTemplate->Set(newString("removeContent", isolate), v8::FunctionTemplate::New(isolate, functionRemoveContent));

	objectTemplate->Set(newString("setContentAddedEventFunction", isolate), v8::FunctionTemplate::New(isolate, functionSetContentAddedEventFunction));
	objectTemplate->Set(newString("setContentRemovedEventFunction", isolate), v8::FunctionTemplate::New(isolate, functionSetContentRemovedEventFunction));

	objectTemplate->Set(newString("timestamp", isolate), v8::FunctionTemplate::New(isolate, functionTimestamp));

	objectTemplate_.Reset(isolate, objectTemplate);
}

void JSOcean::functionDebug(const v8::FunctionCallbackInfo<v8::Value>& info)
{
	if (info.Length() > 0)
	{
		Log::DebugMessageObject messageObject(Log::debug());
		pushMessage(messageObject, info);
	}
}

void JSOcean::functionInfo(const v8::FunctionCallbackInfo<v8::Value>& info)
{
	if (info.Length() > 0)
	{
		Log::MessageObject messageObject(Log::info());
		pushMessage(messageObject, info);
	}
}

void JSOcean::functionWarning(const v8::FunctionCallbackInfo<v8::Value>& info)
{
	if (info.Length() > 0)
	{
		Log::MessageObject messageObject(Log::warning());
		pushMessage(messageObject, info);
	}
}

void JSOcean::functionError(const v8::FunctionCallbackInfo<v8::Value>& info)
{
	if (info.Length() > 0)
	{
		Log::MessageObject messageObject(Log::error());
		pushMessage(messageObject, info);
	}
}

void JSOcean::functionPlatformType(const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(newString(Build::platformType(), v8::Isolate::GetCurrent()));
}

void JSOcean::functionArchitectureType(const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(newString(Build::architectureType(), v8::Isolate::GetCurrent()));
}

void JSOcean::functionReleaseType(const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(newString(Build::releaseType(), v8::Isolate::GetCurrent()));
}

void JSOcean::functionCompilerVersion(const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(newString(Build::compilerVersion(), v8::Isolate::GetCurrent()));
}

void JSOcean::functionAddContent(const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const ApplicationInterface::StringVector files(resolveFilenames(info));

	if (files.empty() && info.Length() > 0)
	{
		Log::warning() << "Ocean::addContent() could not resolve any content information.";
		info.GetReturnValue().Set(-1);
		return;
	}

	if (files.empty())
	{
		info.GetReturnValue().Set(-1);
		return;
	}

	if (files.size() > 1)
	{
		Log::warning() << "Ocean::addContent() returns the event id for the first content only.";
	}

	const ApplicationInterface::EventIds eventIds(ApplicationInterface::get().addContent(files));

	if (eventIds.empty())
	{
		info.GetReturnValue().Set(-1);
		return;
	}

	info.GetReturnValue().Set(int(eventIds.front()));
}

void JSOcean::functionRemoveContent(const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const ApplicationInterface::StringVector files(resolveFilenames(info));

	if (info.Length() == 0)
	{
		ApplicationInterface::get().removeContent(ApplicationInterface::StringVector());
		info.GetReturnValue().Set(true);
		return;
	}

	if (files.empty())
	{
		Log::warning() << "Ocean::removeContent() could not resolve any content information.";
		info.GetReturnValue().Set(-1);
		return;
	}

	if (files.size() > 1)
	{
		Log::warning() << "Ocean::reomveContent() returns the event id for the first content only.";
	}

	const ApplicationInterface::EventIds eventIds(ApplicationInterface::get().removeContent(files));

	if (eventIds.empty())
	{
		info.GetReturnValue().Set(-1);
		return;
	}

	info.GetReturnValue().Set(int(eventIds.front()));
}

void JSOcean::functionSetContentAddedEventFunction(const v8::FunctionCallbackInfo<v8::Value>& info)
{
	if (info.Length() > 0 && info[0]->IsFunction())
	{
		OceanContainer::get().addContentAddedEventFunction(v8::Local<v8::Function>::Cast(info[0]));
	}
}

void JSOcean::functionSetContentRemovedEventFunction(const v8::FunctionCallbackInfo<v8::Value>& info)
{
	if (info.Length() > 0 && info[0]->IsFunction())
	{
		OceanContainer::get().addContentRemovedEventFunction(v8::Local<v8::Function>::Cast(info[0]));
	}
}

void JSOcean::functionTimestamp(const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(double(Timestamp(true)));
}

ApplicationInterface::StringVector JSOcean::resolveFilenames(const v8::FunctionCallbackInfo<v8::Value>& info)
{
	ApplicationInterface::StringVector files;

	for (int n = 0; n < info.Length(); ++n)
	{
		const std::shared_ptr<JSContext> context(JSContext::currentJSContext());
		ocean_assert(context);

		if (context)
		{
			std::string filename;
			if (hasValue(info, (unsigned int)(n), filename))
			{
				std::string resolvedFile;
				if (context->resolveFile(IO::File(filename), resolvedFile))
				{
					files.push_back(resolvedFile);
				}
				else
				{
					Log::warning() << "Could not find file to load: \"" << filename << "\".";
				}

				continue;
			}

			std::vector<std::string> filenames;
			if (hasValue(info, (unsigned int)(n), filenames))
			{
				for (const std::string& internalFilename : filenames)
				{
					std::string resolvedFile;
					if (context->resolveFile(IO::File(internalFilename), resolvedFile))
					{
						files.push_back(resolvedFile);
					}
					else
					{
						Log::warning() << "Could not find file to load: \"" << internalFilename << "\".";
					}
				}
			}
		}
	}

	return files;
}

}

}

}
