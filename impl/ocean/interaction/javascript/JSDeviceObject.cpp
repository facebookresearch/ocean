/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/interaction/javascript/JSDeviceObject.h"
#include "ocean/interaction/javascript/JSDeviceSample.h"
#include "ocean/interaction/javascript/JSMediaObject.h"

#include "ocean/base/String.h"

#include "ocean/devices/MagneticTracker.h"
#include "ocean/devices/Sensor.h"
#include "ocean/devices/Tracker.h"
#include "ocean/devices/ObjectTracker.h"
#include "ocean/devices/VisualTracker.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

Devices::Device::MajorType JSDeviceObject::translateMajorType(const std::string& majorType)
{
	const std::string upperType(String::toUpper(majorType));

	if (upperType == "MEASUREMENT")
	{
		return Devices::Device::DEVICE_MEASUREMENT;
	}

	if (upperType == "SENSOR")
	{
		return Devices::Device::DEVICE_SENSOR;
	}

	if (upperType == "TRACKER")
	{
		return Devices::Device::DEVICE_TRACKER;
	}

	return Devices::Device::DEVICE_INVALID;
}

std::string JSDeviceObject::translateMajorType(const Devices::Device::MajorType majorType)
{
	switch (majorType)
	{
		case Devices::Device::DEVICE_MEASUREMENT:
			return "MEASUREMENT";

		case Devices::Device::DEVICE_SENSOR:
			return "SENSOR";

		case Devices::Device::DEVICE_TRACKER:
			return "TRACKER";

		case Devices::Device::DEVICE_INVALID:
			break;
	}

	return "INVALID";
}

int JSDeviceObject::translateMinorType(const Devices::Device::MajorType majorType, const std::string& minorType)
{
	const std::string upperType(String::toUpper(minorType));

	switch (majorType)
	{
		case Devices::Device::DEVICE_MEASUREMENT:
		{
			return Devices::Device::MINOR_INVALID;
		}

		case Devices::Device::DEVICE_SENSOR:
		{
			if (upperType == "ACCELERATION 3DOF")
			{
				return Devices::Sensor::SENSOR_ACCELERATION_3DOF;
			}

			return Devices::Device::MINOR_INVALID;
		}

		case Devices::Device::DEVICE_TRACKER:
		{
			if (upperType == "MAGNETIC")
			{
				return Devices::Tracker::TRACKER_MAGNETIC;
			}
			if (upperType == "ORIENTATION 3DOF")
			{
				return Devices::Tracker::TRACKER_ORIENTATION_3DOF;
			}
			if (upperType == "POSITION 3DOF")
			{
				return Devices::Tracker::TRACKER_POSITION_3DOF;
			}
			if (upperType == "TRACKER 6DOF")
			{
				return Devices::Tracker::TRACKER_6DOF;
			}

			return Devices::Device::MINOR_INVALID;
		}

		case Devices::Device::DEVICE_INVALID:
		{
			break;
		}
	}

	return Devices::Device::MINOR_INVALID;
}

std::string JSDeviceObject::translateMinorType(const Devices::Device::MajorType majorType, const int minorType)
{
	switch (majorType)
	{
		case Devices::Device::DEVICE_MEASUREMENT:
		{
			return "INVALID";
		}

		case Devices::Device::DEVICE_SENSOR:
		{
			if (minorType == Devices::Sensor::SENSOR_ACCELERATION_3DOF)
			{
				return "ACCELERATION 3DOF";
			}

			return "INVALID";
		}

		case Devices::Device::DEVICE_TRACKER:
		{
			switch (minorType)
			{
				case Devices::Tracker::TRACKER_MAGNETIC:
					return "MAGNETIC";

				case Devices::Tracker::TRACKER_ORIENTATION_3DOF:
					return "ORIENTATION 3DOF";

				case Devices::Tracker::TRACKER_POSITION_3DOF:
					return "POSITION 3DOF";

				case Devices::Tracker::TRACKER_6DOF:
					return "TRACKER 6DOF";
			}

			return "INVALID";
		}

		case Devices::Device::DEVICE_INVALID:
		{
			break;
		}
	}

	return "INVALID";
}

void JSDeviceObject::createFunctionTemplate()
{
	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	v8::Local<v8::FunctionTemplate> functionTemplate(v8::FunctionTemplate::New(isolate, constructor<JSDevice>));
	functionTemplate->SetClassName(newString(objectName(), isolate));

	v8::Local<v8::ObjectTemplate> objectTemplate(v8::Local<v8::ObjectTemplate>::New(isolate, functionTemplate->InstanceTemplate()));
	objectTemplate->SetInternalFieldCount(1);

	objectTemplate->SetAccessor(newString("name", isolate), propertyGetter<NativeType, AI_NAME>);
	objectTemplate->SetAccessor(newString("library", isolate), propertyGetter<NativeType, AI_LIBRARY>);

	objectTemplate->Set(newString("frequency", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_FREQUENCY>));
	objectTemplate->Set(newString("input", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_INPUT>));
	objectTemplate->Set(newString("sample", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SAMPLE>));

	objectTemplate->Set(newString("setFoundObjectEventFunction", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_FOUND_OBJECT_EVENT_FUNCTION>));
	objectTemplate->Set(newString("setLostObjectEventFunction", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_LOST_OBJECT_EVENT_FUNCTION>));
	objectTemplate->Set(newString("setInput", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_INPUT>));

	objectTemplate->Set(newString("isValid", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_IS_VALID>));
	objectTemplate->Set(newString("isInvalid", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_IS_INVALID>));

	objectTemplate->Set(newString("start", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_START>));
	objectTemplate->Set(newString("pause", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_PAUSE>));

	objectTemplate->Set(newString("registerObject", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_REGISTER_OBJECT>));
	objectTemplate->Set(newString("registerPattern", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_REGISTER_OBJECT>)); // alias
	objectTemplate->Set(newString("registerMarker", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_REGISTER_OBJECT>)); // alias

	objectTemplate->Set(newString("objectDescriptions", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_OBJECT_DESCRIPTIONS>));
	objectTemplate->Set(newString("objectId", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_OBJECT_ID>));

	functionTemplate_.Reset(isolate, functionTemplate);
	objectTemplate_.Reset(isolate, objectTemplate);
}

template <>
void JSBase::constructor<JSDevice>(JSDevice& /*thisValue*/, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	if (info.Length() == 0)
	{
		return;
	}
}

template <>
void JSBase::propertyGetter<JSDevice, JSDeviceObject::AI_NAME>(JSDevice& thisValue, v8::Local<v8::String>& /*property*/, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if (thisValue.device().isNull())
	{
		Log::warning() << "The DeviceObject is invalid.";
	}
	else
	{
		info.GetReturnValue().Set(newString(thisValue.device()->name(), v8::Isolate::GetCurrent()));
	}
}

template <>
void JSBase::propertyGetter<JSDevice, JSDeviceObject::AI_LIBRARY>(JSDevice& thisValue, v8::Local<v8::String>& /*property*/, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if (thisValue.device().isNull())
	{
		Log::warning() << "The DeviceObject is invalid.";
	}
	else
	{
		info.GetReturnValue().Set(newString(thisValue.device()->library(), v8::Isolate::GetCurrent()));
	}
}

template <>
void JSBase::function<JSDevice, JSDeviceObject::FI_IS_VALID>(JSDevice& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(!thisValue.device().isNull());
}

template <>
void JSBase::function<JSDevice, JSDeviceObject::FI_IS_INVALID>(JSDevice& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(thisValue.device().isNull());
}

template <>
void JSBase::function<JSDevice, JSDeviceObject::FI_FREQUENCY>(JSDevice& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Devices::TrackerRef tracker(thisValue.device());

	if (tracker)
	{
		info.GetReturnValue().Set(double(tracker->frequency()));
		return;
	}

	Log::warning() << "The DeviceObject does not support the 'frequency()` function.";
}

template <>
void JSBase::function<JSDevice, JSDeviceObject::FI_INPUT>(JSDevice& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Devices::VisualTrackerRef visualTracker(thisValue.device());

	if (visualTracker)
	{
		const Media::FrameMediumRefs frameMediums(visualTracker->input());

		if (frameMediums.empty())
		{
			info.GetReturnValue().Set(createObject<JSMediaObject>(Media::MediumRef(), JSContext::currentContext()));
		}
		else if (frameMediums.size() == 1)
		{
			info.GetReturnValue().Set(createObject<JSMediaObject>(frameMediums.front(), JSContext::currentContext()));
		}
		else
		{
			std::vector<Media::MediumRef> mediums;
			mediums.reserve(frameMediums.size());

			for (const Media::FrameMediumRef& frameMedium : frameMediums)
			{
				mediums.emplace_back(frameMedium);
			}

			info.GetReturnValue().Set(createObjects<JSMediaObject>(mediums, JSContext::currentContext()));
		}

		return;
	}

	Log::warning() << "The DeviceObject does not support the 'input()` function.";
	info.GetReturnValue().Set(createObject<JSMediaObject>(Media::MediumRef(), JSContext::currentContext()));
}

template <>
void JSBase::function<JSDevice, JSDeviceObject::FI_SAMPLE>(JSDevice& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Devices::MeasurementRef measurement(thisValue.device());

	if (measurement)
	{
		double timestamp;
		if (hasValue(info, 0u, timestamp))
		{
			info.GetReturnValue().Set(createObject<JSDeviceSample>(measurement->sample(Timestamp(timestamp)), JSContext::currentContext()));
			return;
		}
		else
		{
			info.GetReturnValue().Set(createObject<JSDeviceSample>(measurement->sample(), JSContext::currentContext()));
			return;
		}
	}

	Log::warning() << "The DeviceObject does not support the 'sample()` function.";
	info.GetReturnValue().Set(createObject<JSDeviceSample>(Devices::Measurement::SampleRef(), JSContext::currentContext()));
}

template <>
void JSBase::function<JSDevice, JSDeviceObject::FI_SET_FOUND_OBJECT_EVENT_FUNCTION>(JSDevice& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Devices::MeasurementRef measurement(thisValue.device());

	if (measurement)
	{
		if (info.Length() >= 1 && info[0]->IsFunction())
		{
			thisValue.setFoundObjectEventFunction(v8::Local<v8::Function>::Cast(info[0]));

			info.GetReturnValue().Set(true);
			return;
		}
	}
	else
	{
		Log::warning() << "The DeviceObject does not support the 'setFoundObjectEventFunction()` function.";
	}

	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<JSDevice, JSDeviceObject::FI_SET_LOST_OBJECT_EVENT_FUNCTION>(JSDevice& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Devices::MeasurementRef measurement(thisValue.device());

	if (measurement)
	{
		if (info.Length() >= 1 && info[0]->IsFunction())
		{
			thisValue.setLostObjectEventFunction(v8::Local<v8::Function>::Cast(info[0]));

			info.GetReturnValue().Set(true);
			return;
		}
	}
	else
	{
		Log::warning() << "The DeviceObject does not support the 'setLostObjectEventFunction()` function.";
	}

	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<JSDevice, JSDeviceObject::FI_SET_INPUT>(JSDevice& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Devices::VisualTrackerRef visualTracker(thisValue.device());

	if (visualTracker)
	{
		Media::MediumRef inputMedium;
		if (hasValue(info, 0u, inputMedium))
		{
			const Media::FrameMediumRef frameInputMedium(inputMedium);

			if (frameInputMedium)
			{
				visualTracker->setInput(frameInputMedium);
				info.GetReturnValue().Set(true);
				return;
			}
		}
	}
	else
	{
		Log::warning() << "The DeviceObject does not support the 'setInput()` function.";
	}

	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<JSDevice, JSDeviceObject::FI_START>(JSDevice& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Devices::DeviceRef& device(thisValue.device());

	if (device)
	{
		info.GetReturnValue().Set(device->start());
		return;
	}
	else
	{
		Log::warning() << "The DeviceObject does not support the 'start()` function.";
	}

	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<JSDevice, JSDeviceObject::FI_PAUSE>(JSDevice& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Devices::DeviceRef& device(thisValue.device());

	if (device)
	{
		info.GetReturnValue().Set(device->pause());
		return;
	}
	else
	{
		Log::warning() << "The DeviceObject does not support the 'pause()` function.";
	}

	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<JSDevice, JSDeviceObject::FI_STOP>(JSDevice& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Devices::DeviceRef& device(thisValue.device());

	if (device)
	{
		info.GetReturnValue().Set(device->stop());
		return;
	}
	else
	{
		Log::warning() << "The DeviceObject does not support the 'stop()` function.";
	}

	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<JSDevice, JSDeviceObject::FI_REGISTER_OBJECT>(JSDevice& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Devices::ObjectTrackerRef objectTracker(thisValue.device());

	if (objectTracker)
	{
		std::string description;

		if (!hasValue(info, 0u, description))
		{
			int descriptionId;
			if (hasValue(info, 0u, descriptionId))
			{
				description = String::toAString(descriptionId);
			}
		}

		Vector3 dimension(0, 0, 0);
		if (!hasValue(info, 1u, dimension))
		{
			Scalar xDimension;
			if (hasValue(info, 1u, xDimension))
			{
				dimension.x() = xDimension;
			}
		}

		const IO::File objectFile(description);

		if (objectFile.exists())
		{
			const Devices::ObjectTracker::ObjectId objectId = objectTracker->registerObject(objectFile(), dimension);

			if (objectId == Devices::ObjectTracker::invalidObjectId())
			{
				Log::warning() << "Failed to register object '" << objectFile() << "'";
			}

			info.GetReturnValue().Set(int(objectId));
			return;
		}

		Devices::ObjectTracker::ObjectId objectId = Devices::ObjectTracker::invalidObjectId();

		const std::shared_ptr<JSContext> context(JSContext::currentJSContext());
		ocean_assert(context);

		if (context)
		{
			const IO::Files resolvedFiles = context->resolveFile(objectFile);

			for (const IO::File& resolveFile : resolvedFiles)
			{
				if (resolveFile.exists())
				{
					const Devices::ObjectTracker::ObjectId internalObjectId = objectTracker->registerObject(resolveFile(), dimension);

					if (internalObjectId == Devices::ObjectTracker::invalidObjectId())
					{
						Log::warning() << "Failed to register object '" << objectFile() << "'";
					}

					info.GetReturnValue().Set(int(internalObjectId));
					return;
				}
			}

			objectId = objectTracker->registerObject(description, dimension);
		}

		if (objectId == Devices::ObjectTracker::invalidObjectId())
		{
			Log::warning() << "Failed to register object '" << description << "'";
		}

		info.GetReturnValue().Set(int(objectId));
		return;
	}
	else
	{
		Log::warning() << "The DeviceObject does not support the 'registerObject()` function.";
	}

	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<JSDevice, JSDeviceObject::FI_OBJECT_DESCRIPTIONS>(JSDevice& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Devices::MeasurementRef measurement(thisValue.device());

	if (measurement)
	{
		const Strings objectDescriptions(measurement->objectDescriptions());

		v8::Local<v8::Value> result = JSBase::createValues<std::string>(objectDescriptions.data(), objectDescriptions.size(), JSContext::currentContext());

		info.GetReturnValue().Set(result);
		return;
	}
}

template <>
void JSBase::function<JSDevice, JSDeviceObject::FI_OBJECT_ID>(JSDevice& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	Devices::Measurement::ObjectId objectId = Devices::Measurement::invalidObjectId();

	const Devices::MeasurementRef measurement(thisValue.device());

	if (measurement)
	{
		std::string description;
		if (hasValue(info, 0u, description))
		{
			objectId = measurement->objectId(description);
		}
	}

	info.GetReturnValue().Set(objectId);
}

template <>
void JSBase::function<JSDevice, JSDeviceObject::FI_INVALID_OBJECT_ID>(JSDevice& /*thisValue*/, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(Devices::Measurement::invalidObjectId());
}

}

}

}
