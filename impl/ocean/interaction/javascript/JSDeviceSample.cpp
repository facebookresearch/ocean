/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/interaction/javascript/JSDeviceSample.h"
#include "ocean/interaction/javascript/JSHomogenousMatrix4.h"
#include "ocean/interaction/javascript/JSQuaternion.h"
#include "ocean/interaction/javascript/JSVector3.h"

#include "ocean/base/String.h"

#include "ocean/devices/Tracker.h"
#include "ocean/devices/Tracker6DOF.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

void JSDeviceSample::createFunctionTemplate()
{
	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	v8::Local<v8::FunctionTemplate> functionTemplate(v8::FunctionTemplate::New(isolate, constructor<Devices::Measurement::SampleRef>));
	functionTemplate->SetClassName(newString(objectName(), isolate));

	v8::Local<v8::ObjectTemplate> objectTemplate(v8::Local<v8::ObjectTemplate>::New(isolate, functionTemplate->InstanceTemplate()));
	objectTemplate->SetInternalFieldCount(1);

	objectTemplate->SetAccessor(newString("timestamp", isolate), propertyGetter<NativeType, AI_TIMESTAMP>);
	objectTemplate->SetAccessor(newString("size", isolate), propertyGetter<NativeType, AI_SIZE>);

	objectTemplate->Set(newString("isDeviceInObject", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_IS_DEVICE_IN_OBJECT>));
	objectTemplate->Set(newString("isObjectInDevice", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_IS_OBJECT_IN_DEVICE>));
	objectTemplate->Set(newString("isValid", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_IS_VALID>));
	objectTemplate->Set(newString("isInvalid", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_IS_INVALID>));
	objectTemplate->Set(newString("hasObject", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_HAS_OBJECT>));
	objectTemplate->Set(newString("position", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_POSITION>));
	objectTemplate->Set(newString("orientation", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_ORIENTATION>));
	objectTemplate->Set(newString("transformation", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_TRANSFORMATION>));
	objectTemplate->Set(newString("objects", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_OBJECTS>));

	functionTemplate_.Reset(isolate, functionTemplate);
	objectTemplate_.Reset(isolate, objectTemplate);
}

template <>
void JSBase::constructor<Devices::Measurement::SampleRef>(Devices::Measurement::SampleRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
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
void JSBase::propertyGetter<Devices::Measurement::SampleRef, JSDeviceSample::AI_TIMESTAMP>(Devices::Measurement::SampleRef& thisValue, v8::Local<v8::String>& /*property*/, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if (thisValue)
	{
		info.GetReturnValue().Set(double(thisValue->timestamp()));
	}
	else
	{
		Log::warning() << "The DeviceSample is invalid";
	}
}

template <>
void JSBase::propertyGetter<Devices::Measurement::SampleRef, JSDeviceSample::AI_SIZE>(Devices::Measurement::SampleRef& thisValue, v8::Local<v8::String>& /*property*/, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if (thisValue)
	{
		info.GetReturnValue().Set(int(thisValue->objectIds().size()));
	}
	else
	{
		Log::warning() << "The DeviceSample is invalid";
	}
}

template <>
void JSBase::function<Devices::Measurement::SampleRef, JSDeviceSample::FI_IS_DEVICE_IN_OBJECT>(Devices::Measurement::SampleRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Devices::Tracker::TrackerSampleRef trackerSample(thisValue);

	if (trackerSample)
	{
		info.GetReturnValue().Set(trackerSample->referenceSystem() == Devices::Tracker::RS_DEVICE_IN_OBJECT);
	}
	else
	{
		Log::info() << "The DeviceSample does not support the function 'isDeviceInObject()'";
		info.GetReturnValue().Set(false);
	}
}

template <>
void JSBase::function<Devices::Measurement::SampleRef, JSDeviceSample::FI_IS_OBJECT_IN_DEVICE>(Devices::Measurement::SampleRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Devices::Tracker::TrackerSampleRef trackerSample(thisValue);

	if (trackerSample)
	{
		info.GetReturnValue().Set(trackerSample->referenceSystem() == Devices::Tracker::RS_OBJECT_IN_DEVICE);
	}
	else
	{
		Log::info() << "The DeviceSample does not support the function 'isObjectInDevice()'";
		info.GetReturnValue().Set(false);
	}
}

template <>
void JSBase::function<Devices::Measurement::SampleRef, JSDeviceSample::FI_IS_VALID>(Devices::Measurement::SampleRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(!thisValue.isNull());
}

template <>
void JSBase::function<Devices::Measurement::SampleRef, JSDeviceSample::FI_IS_INVALID>(Devices::Measurement::SampleRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(thisValue.isNull());
}

template <>
void JSBase::function<Devices::Measurement::SampleRef, JSDeviceSample::FI_HAS_OBJECT>(Devices::Measurement::SampleRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	int objectId;
	if (hasValue<int>(info, 0u, objectId))
	{
		const Devices::Tracker::TrackerSampleRef trackerSample(thisValue);

		if (trackerSample)
		{
			for (const Devices::Tracker::ObjectId& id : trackerSample->objectIds())
			{
				if (int(id) == objectId)
				{
					info.GetReturnValue().Set(true);
					return;
				}
			}
		}

	}

	info.GetReturnValue().Set(false);
}

template <>
void JSBase::function<Devices::Measurement::SampleRef, JSDeviceSample::FI_POSITION>(Devices::Measurement::SampleRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Devices::PositionTracker3DOF::PositionTracker3DOFSampleRef positionTrackerSample(thisValue);

	if (positionTrackerSample)
	{
		const Devices::PositionTracker3DOF::PositionTracker3DOFSample::Positions& positions = positionTrackerSample->positions();

		int objectId;
		if (hasValue<int>(info, 0u, objectId))
		{
			const Devices::Tracker::ObjectIds& objectIds = positionTrackerSample->objectIds();
			ocean_assert(positions.size() == objectIds.size());

			for (size_t index = 0; index < objectIds.size(); ++index)
			{
				if (int(objectIds[index]) == objectId)
				{
					info.GetReturnValue().Set(createObject<JSVector3>(positions[index], JSContext::currentContext()));
					return;
				}
			}

			Log::warning() << "Unknown id provided in DeviceSample::position()";
		}
		else
		{
			if (!positions.empty())
			{
				info.GetReturnValue().Set(createObject<JSVector3>(positions.front(), JSContext::currentContext()));
			}
		}
	}
}

template <>
void JSBase::function<Devices::Measurement::SampleRef, JSDeviceSample::FI_ORIENTATION>(Devices::Measurement::SampleRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Devices::OrientationTracker3DOF::OrientationTracker3DOFSampleRef orientationTrackerSample(thisValue);

	if (orientationTrackerSample)
	{
		const Devices::OrientationTracker3DOF::OrientationTracker3DOFSample::Orientations& orientations = orientationTrackerSample->orientations();

		int objectId;
		if (hasValue<int>(info, 0u, objectId))
		{
			const Devices::Tracker::ObjectIds& objectIds = orientationTrackerSample->objectIds();
			ocean_assert(orientations.size() == objectIds.size());

			for (size_t index = 0; index < objectIds.size(); ++index)
			{
				if (int(objectIds[index]) == objectId)
				{
					info.GetReturnValue().Set(createObject<JSQuaternion>(orientations[index], JSContext::currentContext()));
					return;
				}
			}

			Log::warning() << "Unknown id provided in DeviceSample::orientation()";
		}
		else
		{
			if (!orientations.empty())
			{
				info.GetReturnValue().Set(createObject<JSQuaternion>(orientations.front(), JSContext::currentContext()));
			}
		}
	}
}

template <>
void JSBase::function<Devices::Measurement::SampleRef, JSDeviceSample::FI_TRANSFORMATION>(Devices::Measurement::SampleRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Devices::Tracker6DOF::Tracker6DOFSampleRef tracker6DOFSample(thisValue);

	if (tracker6DOFSample)
	{
		const Devices::PositionTracker3DOF::PositionTracker3DOFSample::Positions& positions = tracker6DOFSample->positions();
		const Devices::OrientationTracker3DOF::OrientationTracker3DOFSample::Orientations& orientations = tracker6DOFSample->orientations();
		ocean_assert(positions.size() == orientations.size());

		int objectId;
		if (hasValue<int>(info, 0u, objectId))
		{
			const Devices::Tracker::ObjectIds& objectIds = tracker6DOFSample->objectIds();
			ocean_assert(positions.size() == objectIds.size());

			for (size_t index = 0; index < objectIds.size(); ++index)
			{
				if (int(objectIds[index]) == objectId)
				{
					info.GetReturnValue().Set(createObject<JSHomogenousMatrix4>(HomogenousMatrix4(positions[index], orientations[index]), JSContext::currentContext()));
					return;
				}
			}

			Log::warning() << "Unknown id provided in DeviceSample::transformation()";
		}
		else
		{
			if (!positions.empty())
			{
				info.GetReturnValue().Set(createObject<JSHomogenousMatrix4>(HomogenousMatrix4(positions.front(), orientations.front()), JSContext::currentContext()));
			}
		}
	}
}

template <>
void JSBase::function<Devices::Measurement::SampleRef, JSDeviceSample::FI_OBJECTS>(Devices::Measurement::SampleRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Devices::Tracker::TrackerSampleRef trackerSample(thisValue);

	if (trackerSample)
	{
		const Devices::Tracker::ObjectIds& objectIds = trackerSample->objectIds();

		v8::Local<v8::Array> result(v8::Array::New(v8::Isolate::GetCurrent(), int(objectIds.size())));

		for (size_t index = 0; index < objectIds.size(); ++index)
		{
			if (!result->Set(JSContext::currentContext(), v8::Int32::New(v8::Isolate::GetCurrent(), int(index)), v8::Int32::New(v8::Isolate::GetCurrent(), int(objectIds[index]))).IsJust())
			{
				ocean_assert(false && "Invalid value!");
			}
		}

		info.GetReturnValue().Set(result);
	}
}

}

}

}
