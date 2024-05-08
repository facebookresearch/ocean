/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_INTERACTION_JS_DEVICE_OBJECT_H
#define META_OCEAN_INTERACTION_JS_DEVICE_OBJECT_H

#include "ocean/interaction/javascript/JavaScript.h"
#include "ocean/interaction/javascript/JSDevice.h"
#include "ocean/interaction/javascript/JSObject.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

/**
 * This class implements a wrapper for a JavaScript DeviceObject object.
 * @ingroup interactionjs
 */
class OCEAN_INTERACTION_JS_EXPORT JSDeviceObject : public JSObject<JSDeviceObject, JSDevice>
{
	friend class JSObject<JSDeviceObject, JSDevice>;
	friend class JSLibrary;

	public:

		/**
		 * Definition of ids for individual accessors.
		 */
		enum AccessorId : unsigned int
		{
			AI_NAME,
			AI_LIBRARY
		};

		/**
		 * Definition of ids for individual functions.
		 */
		enum FunctionId : unsigned int
		{
			FI_FREQUENCY,
			FI_INPUT,
			FI_SAMPLE,

			FI_SET_FOUND_OBJECT_EVENT_FUNCTION,
			FI_SET_LOST_OBJECT_EVENT_FUNCTION,
			FI_SET_INPUT,

			FI_IS_VALID,
			FI_IS_INVALID,

			FI_START,
			FI_PAUSE,
			FI_STOP,

			FI_REGISTER_OBJECT,
			FI_OBJECT_DESCRIPTIONS,
			FI_OBJECT_ID,
			FI_INVALID_OBJECT_ID
		};

	public:

		/**
		 * Returns the JavaScript name of this object.
		 * @return The object's JavaScript name
		 */
		static inline const char* objectName();

		/**
		 * Translates a string holding a device major type into the major type value.
		 * @param majorType Major type to translate
		 * @return Major type as value
		 */
		static Devices::Device::MajorType translateMajorType(const std::string& majorType);

		/**
		 * Translates a major type value into a string holding the major type.
		 * @param majorType Major type to translate
		 * @return Major type as string
		 */
		static std::string translateMajorType(const Devices::Device::MajorType majorType);

		/**
		 * Translates a string holding a device minor type into the minor type value.
		 * @param majorType Major type of the minor type to translate
		 * @param minorType Mintor type to translate
		 * @return Minor type as value
		 */
		static int translateMinorType(const Devices::Device::MajorType majorType, const std::string& minorType);

		/**
		 * Translates a minor type into a string holding the minor type.
		 * @param majorType Major type of the minor type to translate
		 * @param minorType Minor type to translate
		 * @return Minor type as string
		 */
		static std::string translateMinorType(const Devices::Device::MajorType majorType, const int minorType);

	protected:

		/**
		 * Creates the function template and object template for this object.
		 */
		static void createFunctionTemplate();
};

inline const char* JSDeviceObject::objectName()
{
	return "DeviceObject";
}

}

}

}

#endif // META_OCEAN_INTERACTION_JS_DEVICE_OBJECT_H
