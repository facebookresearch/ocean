/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/interaction/javascript/JSDevice.h"
#include "ocean/interaction/javascript/JSContext.h"
#include "ocean/interaction/javascript/JSDeviceSample.h"
#include "ocean/interaction/javascript/JSLibrary.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

JSDevice::JSDevice()
{
	// nothing to do here
}

JSDevice::JSDevice(JSDevice&& jsDevice) :
	JSDevice()
{
	*this = std::move(jsDevice);
}

JSDevice::JSDevice(const Devices::DeviceRef& device) :
	device_(device)
{
	// nothing to do here
}

JSDevice::~JSDevice()
{
	release();
}

void JSDevice::setDevice(const Devices::DeviceRef& device)
{
	release();
	device_ = device;
}

void JSDevice::setFoundObjectEventFunction(v8::Local<v8::Function> function)
{
	if (function.IsEmpty())
	{
		if (trackerObjectEventSubscription_ && lostObjectEventFunctionName_.empty())
		{
			trackerObjectEventSubscription_.release();
		}

		foundObjectEventFunctionName_.clear();
	}
	else
	{
		if (!trackerObjectEventSubscription_)
		{
			const Devices::TrackerRef tracker(device_);

			if (tracker)
			{
				trackerObjectEventSubscription_ = tracker->subscribeTrackerObjectEvent(Devices::Tracker::TrackerObjectCallback(*this, &JSDevice::onObjectEvent));

				ownerContext_ = JSContext::currentJSContext();

				const v8::Handle<v8::Value> name = function->GetName();
				ocean_assert(name->IsString());

				foundObjectEventFunctionName_ = JSBase::toAString(name->ToString(JSContext::currentContext()));
			}
		}
	}
}

void JSDevice::setLostObjectEventFunction(v8::Local<v8::Function> function)
{
	if (function.IsEmpty())
	{
		if (trackerObjectEventSubscription_ && foundObjectEventFunctionName_.empty())
		{
			trackerObjectEventSubscription_.release();
		}

		lostObjectEventFunctionName_.clear();
	}
	else
	{
		if (!trackerObjectEventSubscription_)
		{
			const Devices::TrackerRef tracker(device_);

			if (tracker)
			{
				trackerObjectEventSubscription_ = tracker->subscribeTrackerObjectEvent(Devices::Tracker::TrackerObjectCallback(*this, &JSDevice::onObjectEvent));

				ownerContext_ = JSContext::currentJSContext();

				const v8::Handle<v8::Value> name = function->GetName();
				ocean_assert(name->IsString());

				lostObjectEventFunctionName_ = JSBase::toAString(name->ToString(JSContext::currentContext()));
			}
		}
	}
}

JSDevice& JSDevice::operator=(JSDevice&& jsDevice)
{
	if (this != &jsDevice)
	{
		release();

		device_ = std::move(jsDevice.device_);

		sampleEventSubscription_ = std::move(jsDevice.sampleEventSubscription_);
		trackerObjectEventSubscription_ = std::move(jsDevice.trackerObjectEventSubscription_);

		sampleEventFunctionName_ = std::move(jsDevice.sampleEventFunctionName_);
		foundObjectEventFunctionName_ = std::move(jsDevice.foundObjectEventFunctionName_);
		lostObjectEventFunctionName_ = std::move(jsDevice.lostObjectEventFunctionName_);

		ownerContext_ = std::move(jsDevice.ownerContext_);
	}

	return *this;
}

void JSDevice::release()
{
	sampleEventSubscription_.release();
	trackerObjectEventSubscription_.release();

	sampleEventFunctionName_.clear();
	foundObjectEventFunctionName_.clear();
	lostObjectEventFunctionName_.clear();

	ownerContext_.reset();

	device_ = Devices::DeviceRef();
}

void JSDevice::onObjectEvent(const Devices::Tracker* /*tracker*/, const bool found, const Devices::Tracker::ObjectIdSet& objectIds, const Timestamp& timestamp)
{
	if (found)
	{
		if (foundObjectEventFunctionName_.empty())
		{
			return;
		}

		const std::shared_ptr<JSContext> context(ownerContext_);

		if (context)
		{
			for (const Devices::Tracker::ObjectId& objectId : objectIds)
			{
				context->addExplicitFunctionCall(foundObjectEventFunctionName_, JSContext::FunctionParameterCaller::createStatic(&JSDevice::parameter, objectId, timestamp));
			}
		}
		else
		{
#ifdef OCEAN_DEBUG
			Log::warning() << "JSDevice::onObjectEvent(): Unknown JSContext";
#endif
		}
	}
	else
	{
		if (lostObjectEventFunctionName_.empty())
		{
			return;
		}

		const std::shared_ptr<JSContext> context(ownerContext_);

		if (context)
		{
			for (const Devices::Tracker::ObjectId& objectId : objectIds)
			{
				context->addExplicitFunctionCall(lostObjectEventFunctionName_, JSContext::FunctionParameterCaller::createStatic(&JSDevice::parameter, objectId, timestamp));
			}
		}
		else
		{
#ifdef OCEAN_DEBUG
			Log::warning() << "JSDevice::onObjectEvent(): Unknown JSContext";
#endif
		}
	}
}

std::vector<v8::Handle<v8::Value>> JSDevice::parameter(const Devices::Tracker::ObjectId object, const Timestamp timestamp)
{
	JSContext::Values values;
	values.reserve(2);

	values.push_back(v8::Uint32::New(v8::Isolate::GetCurrent(), int(object)));
	values.push_back(v8::Number::New(v8::Isolate::GetCurrent(), double(timestamp)));

	return values;
}

}

}

}
