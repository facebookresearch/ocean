/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_INTERACTION_JS_QUATERNION_H
#define META_OCEAN_INTERACTION_JS_QUATERNION_H

#include "ocean/interaction/javascript/JavaScript.h"
#include "ocean/interaction/javascript/JSObject.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

/**
 * This class implements a wrapper for a JavaScript Quaternion object.
 * @ingroup interactionjs
 */
class OCEAN_INTERACTION_JS_EXPORT JSQuaternion : public JSObject<JSQuaternion, Quaternion>
{
	friend class JSObject<JSQuaternion, Quaternion>;
	friend class JSLibrary;

	public:

		/**
		 * Definition of ids for individual accessors.
		 */
		enum AccessorId : unsigned int
		{
			/// The accessor for the x property, a number value.
			AI_X,
			/// The accessor for the y property, a number value.
			AI_Y,
			/// The accessor for the z property, a number value.
			AI_Z,
			/// The accessor for the w property, a number value.
			AI_W
		};

		/**
		 * Definition of ids for individual functions.
		 */
		enum FunctionId : unsigned int
		{
			/**
			 * Inverts this quaternion.
			 * <pre>
			 * Quaternion.invert()
			 * </pre>
			 */
			FI_INVERT,

			/**
			 * Returns the inverted quaternion of this quaternion.
			 * <pre>
			 * Quaternion = Quaternion.inverted()
			 * </pre>
			 */
			FI_INVERTED,

			/**
			 * Returns whether two quaternion are equal up to some epsilon.
			 * <pre>
			 * Boolean = Quaternion.isEqual(Quaternion)
			 * </pre>
			 */
			FI_IS_EQUAL,

			/**
			 * Multiplies two quaternions, mutiplies this quaternion with a vector, or multiplies this quaternion with a quaternion.
			 * <pre>
			 * Quaternion = Quaternion.multiply(Quaternion)
			 * Vector3 = Quaternion.multiply(Vector3)
			 * Quaternion = Quaternion.multiply(Rotation)
			 * </pre>
			 */
			FI_MULTIPLY,

			/**
			 * Returns an interpolated quaternion.
			 * <pre>
			 * Quaternion = Quaternion.slerp(Quaternion, Number)
			 * </pre>
			 */
			FI_SLERP,

			/**
			 * Returns a string with the values of this matrix.
			 * <pre>
			 * String = Quaternion.string()
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

inline const char* JSQuaternion::objectName()
{
	return "Quaternion";
}

}

}

}

#endif // META_OCEAN_INTERACTION_JS_QUATERNION_H
