/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_INTERACTION_JS_VECTOR_3_H
#define META_OCEAN_INTERACTION_JS_VECTOR_3_H

#include "ocean/interaction/javascript/JavaScript.h"
#include "ocean/interaction/javascript/JSObject.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

/**
 * This class implements a wrapper for a JavaScript Vector3 object.
 * @ingroup interactionjs
 */
class OCEAN_INTERACTION_JS_EXPORT JSVector3 : public JSObject<JSVector3, Vector3>
{
	friend class JSObject<JSVector3, Vector3>;
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
			AI_Z
		};

		/**
		 * Definition of ids for individual functions.
		 */
		enum FunctionId : unsigned int
		{
			/**
			 * Adds two vectors or adds a scalar value to this vector.
			 * <pre>
			 * Vector3 = Vector3.add(Vector3)
			 * Vector3 = Vector3.add(Number)
			 * </pre>
			 */
			FI_ADD,

			/**
			 * Determines the angle between two vectors in radian.
			 * <pre>
			 * Number = Vector3.angle(Vector3)
			 * </pre>
			 */
			FI_ANGLE,

			/**
			 * Determines the cross product between two vectors.
			 * <pre>
			 * Vector3 = Vector3.cross(Vector3)
			 * </pre>
			 */
			FI_CROSS,

			/**
			 * Inverts this vector.
			 * <pre>
			 * Vector3.invert()
			 * </pre>
			 */
			FI_INVERT,

			/**
			 * Returns the inverted vector of this vector.
			 * <pre>
			 * Vector3 = Vector3.inverted()
			 * </pre>
			 */
			FI_INVERTED,

			/**
			 * Returns whether two vectors are equal up to some epsilon.
			 * <pre>
			 * Boolean = Vector3.isEqual(Vector3)
			 * </pre>
			 */
			FI_IS_EQUAL,

			/**
			 * Returns the length of this vector.
			 * <pre>
			 * Number = Vector3.length()
			 * </pre>
			 */
			FI_LENGTH,

			/**
			 * Returns the distance between this vector and another vector.
			 * <pre>
			 * Number = Vector3.distance(Vector3)
			 * </pre>
			 */
			FI_DISTANCE,

			/**
			 * Multiplies two vectors to determine the scalar product or multiplies this vector with a scalar.
			 * <pre>
			 * Number = Vector3.multiply(Vector3)
			 * Vector3 = Vector3.multiply(Number)
			 * </pre>
			 */
			FI_MULTIPLY,

			/**
			 * Normalizes this vector so that the vector has unit length afterwards.
			 * <pre>
			 * Vector3.normalize()
			 * </pre>
			 */
			FI_NORMALIZE,

			/**
			 * Returns the normalized vector of this vector, so that the new vector has unit length.
			 * <pre>
			 * Vector3 = Vector3.normalized()
			 * </pre>
			 */
			FI_NORMALIZED,

			/**
			 * Returns a string with the values of this vector.
			 * <pre>
			 * String = Vector3.string()
			 * </pre>
			 */
			FI_STRING,

			/**
			 * Subtracts two vectors or subtracts a scalar value from this vector.
			 * <pre>
			 * Vector3 = Vector3.subtract(Vector3)
			 * Vector3 = Veccto3.subtract(Number)
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

inline const char* JSVector3::objectName()
{
	return "Vector3";
}

}

}

}

#endif // META_OCEAN_INTERACTION_JS_VECTOR_3_H
