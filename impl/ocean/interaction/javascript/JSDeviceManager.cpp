/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/interaction/javascript/JSDeviceManager.h"
#include "ocean/interaction/javascript/JSDeviceObject.h"

#include "ocean/base/String.h"

#include "ocean/devices/Manager.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

v8::Persistent<v8::ObjectTemplate> JSDeviceManager::objectTemplate_;

void JSDeviceManager::createObjectTemplate()
{
	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	v8::Local<v8::ObjectTemplate> objectTemplate(v8::ObjectTemplate::New(isolate));

	objectTemplate->Set(newString("create", isolate), v8::FunctionTemplate::New(isolate, functionCreate));
	objectTemplate->Set(newString("devices", isolate), v8::FunctionTemplate::New(isolate, functionDevices));

	objectTemplate_.Reset(isolate, objectTemplate);
}

void JSDeviceManager::functionCreate(const v8::FunctionCallbackInfo<v8::Value>& info)
{
	std::string name;
	if (hasValue(info, 0u, name))
	{
		bool exclusive = false;
		hasValue(info, 1u, exclusive);

		info.GetReturnValue().Set(createObject<JSDeviceObject>(JSDevice(Devices::Manager::get().device(name, exclusive)), JSContext::currentContext()));
		return;
	}

	Log::error() << "DeviceManager::create() needs one String parameter.";
	info.GetReturnValue().Set(createObject<JSDeviceObject>(JSDevice(Devices::DeviceRef()), JSContext::currentContext()));
}

void JSDeviceManager::functionDevices(const v8::FunctionCallbackInfo<v8::Value>& info)
{
	Devices::Device::MajorType majorType = Devices::Device::DEVICE_INVALID;
	int minorType = Devices::Device::MINOR_INVALID;

	std::string valueMajor;
	if (hasValue(info, 0u, valueMajor))
	{
		majorType = JSDeviceObject::translateMajorType(valueMajor);

		std::string valueMinor;
		if (hasValue(info, 1u, valueMinor))
		{
			minorType = JSDeviceObject::translateMinorType(majorType, valueMinor);
		}
	}

	Strings deviceNames;
	if (majorType != Devices::Device::DEVICE_INVALID)
	{
		deviceNames = Devices::Manager::get().devices(Devices::Device::DeviceType(majorType, minorType));
	}
	else
	{
		deviceNames = Devices::Manager::get().devices();
	}

	info.GetReturnValue().Set(createValues(deviceNames, JSContext::currentContext()));
}

}

}

}
