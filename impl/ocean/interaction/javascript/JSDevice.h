/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_INTERACTION_JS_DEVICE_H
#define META_OCEAN_INTERACTION_JS_DEVICE_H

#include "ocean/interaction/javascript/JavaScript.h"

#include "ocean/devices/DeviceRef.h"
#include "ocean/devices/Measurement.h"
#include "ocean/devices/Tracker.h"

#include <v8.h>

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

class JSContext;

/**
 * This class is a lightweight helper class for device objects to allow event callback handling.
 * @ingroup interactionjs
 */
class OCEAN_INTERACTION_JS_EXPORT JSDevice
{
	public:

		/**
		 * Creates a new device helper object.
		 */
		JSDevice();

		/**
		 * Move constructor.
		 * @param jsDevice The device object to be moved
		 */
		JSDevice(JSDevice&& jsDevice);

		/**
		 * Creates a new device helper object.
		 */
		explicit JSDevice(const Devices::DeviceRef& device);

		/**
		 * Destructs a device helper object.
		 */
		~JSDevice();

		/**
		 * Returns the internal device object.
		 * @return Device object
		 */
		inline const Devices::DeviceRef& device() const;

		/**
		 * Sets the internal device object.
		 * Beware: If the internal device has been defined once, you can not change it
		 * @param device Internal device to set
		 */
		void setDevice(const Devices::DeviceRef& device);

		/**
		 * Sets the JavaScript function for found object events.
		 * @param function JavaScript function to be called on new found object events
		 */
		void setFoundObjectEventFunction(v8::Local<v8::Function> function);

		/**
		 * Sets the JavaScript function for lost object events.
		 * @param function JavaScript function to be called on new lost object events
		 */
		void setLostObjectEventFunction(v8::Local<v8::Function> function);

		/**
		 * Move operator.
		 * @param jsDevice The device object to be moved
		 * @return The reference to this object
		 */
		JSDevice& operator=(JSDevice&& jsDevice);

	protected:

		/**
		 * Disabled copy constructor.
		 * @param device Object which would be copied
		 */
		JSDevice(const JSDevice& device) = delete;

		/**
		 * Releases all event subscriptions.
		 */
		void release();

		/**
		 * Callback function for tracker object events.
		 * @param tracker Event sender
		 * @param found State determining whether an object has been found or lost
		 * @param objectIds The ids of all objects which have been found or lost recently
		 * @param timestamp Event timestamp
		 */
		void onObjectEvent(const Devices::Tracker* tracker, const bool found, const Devices::Tracker::ObjectIdSet& objectIds, const Timestamp& timestamp);

		/**
		 * Disabled copy operator.
		 * @param device Object which would be copied
		 * @return Reference to this object
		 */
		JSDevice& operator=(const JSDevice& device) = delete;

		/**
		 * Creates JavaScript parameters of one given values.
		 * @param object Object id
		 * @param timestamp Timestamp
		 */
		static std::vector<v8::Handle<v8::Value>> parameter(const Devices::Tracker::ObjectId object, const Timestamp timestamp);

	protected:

		/// Internal device object.
		Devices::DeviceRef device_;

		/// Sample event subscription id.
		Devices::Tracker::SampleEventSubscription sampleEventSubscription_;

		/// Tracker object event subscription id.
		Devices::Tracker::TrackerObjectEventSubscription trackerObjectEventSubscription_;

		/// JavaScript function name for sample events.
		std::string sampleEventFunctionName_;

		/// JavaScript function name for found object events.
		std::string foundObjectEventFunctionName_;

		// JavaScript function name for lost object events.
		std::string lostObjectEventFunctionName_;

		/// The context owning this device.
		std::weak_ptr<JSContext> ownerContext_;
};

const Devices::DeviceRef& JSDevice::device() const
{
	return device_;
}

}

}

}

#endif // META_OCEAN_INTERACTION_JS_DEVICE_H
