/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_INTERACTION_JS_VECTOR_2_H
#define META_OCEAN_INTERACTION_JS_VECTOR_2_H

#include "ocean/interaction/javascript/JavaScript.h"
#include "ocean/interaction/javascript/JSObject.h"

#include <v8.h>

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

/**
 * This class implements a wrapper for a JavaScript Vector2 object.
 * @ingroup interactionjs
 */
class OCEAN_INTERACTION_JS_EXPORT JSVector2 : public JSObject<JSVector2, Vector2>
{
	friend class JSObject<JSVector2, Vector2>;
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
			AI_Y
		};

		/**
		 * Definition of ids for individual functions.
		 */
		enum FunctionId : unsigned int
		{
			/**
			 * Adds two vectors or adds a scalar value to this vector.
			 * <pre>
			 * Vector2 = Vector2.add(Vector2)
			 * Vector2 = Veccto2.add(Number)
			 * </pre>
			 */
			FI_ADD,

			/**
			 * Determines the angle between two vectors in radian.
			 * <pre>
			 * Number = Vector2.angle(Vector2)
			 * </pre>
			 */
			FI_ANGLE,

			/**
			 * Inverts this vector.
			 * <pre>
			 * Vector2.invert()
			 * </pre>
			 */
			FI_INVERT,

			/**
			 * Returns the inverted vector of this vector.
			 * <pre>
			 * Vector2 = Vector2.inverted()
			 * </pre>
			 */
			FI_INVERTED,

			/**
			 * Returns whether two vectors are equal up to some epsilon.
			 * <pre>
			 * Boolean = Vector2.isEqual(Vector2)
			 * </pre>
			 */
			FI_IS_EQUAL,

			/**
			 * Returns the length of this vector.
			 * <pre>
			 * Number = Vector2.length()
			 * </pre>
			 */
			FI_LENGTH,

			/**
			 * Returns the distance between this vector and another vector.
			 * <pre>
			 * Number = Vector2.distance(Vector2)
			 * </pre>
			 */
			FI_DISTANCE,

			/**
			 * Multiplies two vectors to determine the scalar product or multiplies this vector with a scalar.
			 * <pre>
			 * Number = Vector2.multiply(Vector2)
			 * Vector2 = Vector2.multiply(Number)
			 * </pre>
			 */
			FI_MULTIPLY,

			/**
			 * Normalizes this vector so that the vector has unit length afterwards.
			 * <pre>
			 * Vector2.normalize()
			 * </pre>
			 */
			FI_NORMALIZE,

			/**
			 * Returns the normalized vector of this vector, so that the new vector has unit length.
			 * <pre>
			 * Vector2 = Vector2.normalized()
			 * </pre>
			 */
			FI_NORMALIZED,

			/**
			 * Returns a string with the values of this vector.
			 * <pre>
			 * String = Vector2.string()
			 * </pre>
			 */
			FI_STRING,

			/**
			 * Subtracts two vectors or subtracts a scalar value from this vector.
			 * <pre>
			 * Vector2 = Vector2.subtract(Vector2)
			 * Vector2 = Veccto2.subtract(Number)
			 * </pre>
			 */
			FI_SUBTRACT
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

inline const char* JSVector2::objectName()
{
	return "Vector2";
}

}

}

}

#endif // META_OCEAN_INTERACTION_JS_VECTOR_2_H
