/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/interaction/javascript/JSSceneDescriptionManager.h"
#include "ocean/interaction/javascript/JSSceneDescriptionNode.h"

#include "ocean/scenedescription/Manager.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

v8::Persistent<v8::ObjectTemplate> JSSceneDescriptionManager::objectTemplate_;

void JSSceneDescriptionManager::createObjectTemplate()
{
	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	v8::Local<v8::ObjectTemplate> objectTemplate(v8::ObjectTemplate::New(isolate));

	objectTemplate->Set(newString("findNode", isolate), v8::FunctionTemplate::New(isolate, functionFindNode));
	objectTemplate->Set(newString("findNodes", isolate), v8::FunctionTemplate::New(isolate, functionFindNodes));
	objectTemplate->Set(newString("libraries", isolate), v8::FunctionTemplate::New(isolate, functionLibraries));

	objectTemplate_.Reset(isolate, objectTemplate);
}

void JSSceneDescriptionManager::functionFindNode(const v8::FunctionCallbackInfo<v8::Value>& info)
{
	std::string nodeName;
	if (hasValue(info, 0u, nodeName))
	{
		std::string libraryName;
		if (hasValue(info, 1u, libraryName))
		{
			info.GetReturnValue().Set(createObject<JSSceneDescriptionNode>(SceneDescription::Manager::get().node(libraryName, nodeName), JSContext::currentContext()));
			return;
		}

		info.GetReturnValue().Set(createObject<JSSceneDescriptionNode>(SceneDescription::Manager::get().node(nodeName), JSContext::currentContext()));
		return;
	}

	Log::error() << "SceneDescriptionManager::findNode() needs at least one String parameter.";
	info.GetReturnValue().Set(createObject<JSSceneDescriptionNode>(SceneDescription::NodeRef(), JSContext::currentContext()));
}

void JSSceneDescriptionManager::functionFindNodes(const v8::FunctionCallbackInfo<v8::Value>& info)
{
	std::string nodeName;
	if (hasValue(info, 0u, nodeName))
	{
		std::string libraryName;
		if (hasValue(info, 1u, libraryName))
		{
			info.GetReturnValue().Set(createObjects<JSSceneDescriptionNode>(SceneDescription::Manager::get().nodes(libraryName, nodeName), JSContext::currentContext()));
			return;
		}

		info.GetReturnValue().Set(createObjects<JSSceneDescriptionNode>(SceneDescription::Manager::get().nodes(nodeName), JSContext::currentContext()));
		return;
	}

	Log::error() << "SceneDescriptionManager::findNodes() needs at least one String parameter.";
	info.GetReturnValue().Set(createObjects<JSSceneDescriptionNode>(SceneDescription::NodeRefs(), JSContext::currentContext()));
}

void JSSceneDescriptionManager::functionLibraries(const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const SceneDescription::Manager::LibraryNames libraries(SceneDescription::Manager::get().libraries());

	info.GetReturnValue().Set(createValues(libraries, JSContext::currentContext()));
}

}

}

}
