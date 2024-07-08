/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_INTERACTION_JS_EULER_H
#define META_OCEAN_INTERACTION_JS_EULER_H

#include "ocean/interaction/javascript/JavaScript.h"
#include "ocean/interaction/javascript/JSObject.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

/**
 * This class implements a wrapper for a JavaScript Euler object.
 * @ingroup interactionjs
 */
class OCEAN_INTERACTION_JS_EXPORT JSEuler : public JSObject<JSEuler, Euler>
{
	friend class JSObject<JSEuler, Euler>;
	friend class JSLibrary;

	public:

		/**
		 * Definition of ids for individual accessors.
		 */
		enum AccessorId : unsigned int
		{
			/// The accessor for the yaw property, a number value.
			AI_YAW,
			/// The accessor for the roll property, a number value.
			AI_PITCH,
			/// The accessor for the pitch property, a number value.
			AI_ROLL
		};

		/**
		 * Definition of ids for individual functions.
		 */
		enum FunctionId : unsigned int
		{
			/**
			 * Returns whether two euler objects are equal up to some epsilon.
			 * <pre>
			 * Boolean = Euler.isEqual(Euler)
			 * </pre>
			 */
			FI_IS_EQUAL,

			/**
			 * Returns a string with the values of this euler object.
			 * <pre>
			 * String = Euler.string()
			 * </pre>
			 */
			FI_STRING
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

inline const char* JSEuler::objectName()
{
	return "Euler";
}

}

}

}

#endif // META_OCEAN_INTERACTION_JS_EULER_H
