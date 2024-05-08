/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_INTERACTION_JS_DEVICE_SAMPLE_H
#define META_OCEAN_INTERACTION_JS_DEVICE_SAMPLE_H

#include "ocean/interaction/javascript/JavaScript.h"
#include "ocean/interaction/javascript/JSObject.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

/**
 * This class implements a wrapper for a JavaScript DeviceSample object.
 * @ingroup interactionjs
 */
class OCEAN_INTERACTION_JS_EXPORT JSDeviceSample : public JSObject<JSDeviceSample, Devices::Measurement::SampleRef>
{
	friend class JSObject<JSDeviceSample, Devices::Measurement::SampleRef>;
	friend class JSLibrary;

	public:

		/**
		 * Definition of ids for individual accessors.
		 */
		enum AccessorId : unsigned int
		{
			/// The accessor for the size property, a number value.
			AI_TIMESTAMP,
			/// The accessor for the size property, an integer value.
			AI_SIZE
		};

		/**
		 * Definition of ids for individual functions.
		 */
		enum FunctionId : unsigned int
		{
			FI_IS_DEVICE_IN_OBJECT,
			FI_IS_OBJECT_IN_DEVICE,
			FI_IS_VALID,
			FI_IS_INVALID,
			FI_HAS_OBJECT,
			FI_POSITION,
			FI_ORIENTATION,
			FI_TRANSFORMATION,
			FI_OBJECTS
		};

	public:

		/**
		 * Returns the JavaScript name of this object.
		 * @return The object's JavaScript name
		 */
		static inline const char* objectName();

	protected:

		/**
		 * Creates the function template and object template for this object.
		 */
		static void createFunctionTemplate();
};

inline const char* JSDeviceSample::objectName()
{
	return "DeviceSample";
}

}

}

}

#endif // META_OCEAN_INTERACTION_JS_DEVICE_SAMPLE_H
