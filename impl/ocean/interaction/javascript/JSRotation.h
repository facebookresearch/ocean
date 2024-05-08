/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_INTERACTION_JS_ROTATION_H
#define META_OCEAN_INTERACTION_JS_ROTATION_H

#include "ocean/interaction/javascript/JavaScript.h"
#include "ocean/interaction/javascript/JSObject.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

/**
 * This class implements a wrapper for a JavaScript Rotation object.
 * @ingroup interactionjs
 */
class OCEAN_INTERACTION_JS_EXPORT JSRotation : public JSObject<JSRotation, Rotation>
{
	friend class JSObject<JSRotation, Rotation>;
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
			/// The accessor for the axis property, a Vector3 object.
			AI_AXIS,
			/// The accessor for the a property, a number value.
			AI_A,
			/// The accessor for the angle property, a number value.
			AI_ANGLE
		};

		/**
		 * Definition of ids for individual functions.
		 */
		enum FunctionId : unsigned int
		{
			/**
			 * Inverts this rotation.
			 * <pre>
			 * Rotation.invert()
			 * </pre>
			 */
			FI_INVERT,

			/**
			 * Returns the inverted rotation of this rotation.
			 * <pre>
			 * Rotation = Rotation.inverted()
			 * </pre>
			 */
			FI_INVERTED,

			/**
			 * Returns whether two rotation are equal up to some epsilon.
			 * <pre>
			 * Boolean = Rotation.isEqual(Rotation)
			 * </pre>
			 */
			FI_IS_EQUAL,

			/**
			 * Multiplies two rotations, mutiplies this rotation with a vector, or multiplies this rotation with a quaternion.
			 * <pre>
			 * Rotation = Rotation.multiply(Rotation)
			 * Vector3 = Rotation.multiply(Vector3)
			 * Rotation = Rotation.multiply(Quaternion)
			 * </pre>
			 */
			FI_MULTIPLY,

			/**
			 * Returns a string with the values of this rotation.
			 * <pre>
			 * String = Rotation.string()
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

inline const char* JSRotation::objectName()
{
	return "Rotation";
}

}

}

}

#endif // META_OCEAN_INTERACTION_JS_ROTATION_H
