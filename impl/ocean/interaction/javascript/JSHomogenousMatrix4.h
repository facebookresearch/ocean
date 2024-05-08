/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_INTERACTION_JS_HOMOGENOUS_MATRIX_4_H
#define META_OCEAN_INTERACTION_JS_HOMOGENOUS_MATRIX_4_H

#include "ocean/interaction/javascript/JavaScript.h"
#include "ocean/interaction/javascript/JSObject.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

/**
 * This class implements a wrapper for a JavaScript HomogenousMatrix4 object.
 * @ingroup interactionjs
 */
class OCEAN_INTERACTION_JS_EXPORT JSHomogenousMatrix4 : public JSObject<JSHomogenousMatrix4, HomogenousMatrix4>
{
	friend class JSObject<JSHomogenousMatrix4, HomogenousMatrix4>;
	friend class JSLibrary;

	public:

		/**
		 * Definition of ids for individual functions.
		 */
		enum FunctionId : unsigned int
		{
			/**
			 * Determines the matrix's determinant.
			 * <pre>
			 * Number = HomogenousMatrix4.determinant()
			 * </pre>
			 */
			FI_DETERMINANT,

			/**
			 * Inverts this matrix.
			 * <pre>
			 * Boolean = HomogenousMatrix4.invert()
			 * </pre>
			 */
			FI_INVERT,

			/**
			 * Returns the inverted matrix of this matrix.
			 * <pre>
			 * HomogenousMatrix4 = HomogenousMatrix4.inverted()
			 * </pre>
			 */
			FI_INVERTED,

			/**
			 * Returns an interpolated matrix.
			 * <pre>
			 * HomogenousMatrix4 = HomogenousMatrix4.interpolate(HomogenousMatrix4, Number)
			 * </pre>
			 */
			FI_INTERPOLATE,

			/**
			 * Returns whether two matrices are equal up to some epsilon.
			 * <pre>
			 * Boolean = HomogenousMatrix4.isEqual(HomogenousMatrix4)
			 * </pre>
			 */
			FI_IS_EQUAL,

			/**
			 * Multiplies two matrices, mutiplies this matrix with a vector, or multiplies this matrix with a scalar.
			 * <pre>
			 * HomogenousMatrix4 = HomogenousMatrix4.multiply(HomogenousMatrix4)
			 * Vector3 = HomogenousMatrix4.multiply(Vector3)
			 * Vector4 = HomogenousMatrix4.multiply(Vector4)
			 * HomogenousMatrix4 = HomogenousMatrix4.multiply(Rotation)
			 * HomogenousMatrix4 = HomogenousMatrix4.multiply(Quaternion)
			 * HomogenousMatrix4 = HomogenousMatrix4.multiply(Euler)
			 * </pre>
			 */
			FI_MULTIPLY,

			/**
			 * Returns the rotation of this matrix.
			 * <pre>
			 * Rotation = HomogenousMatrix4.rotation()
			 * </pre>
			 */
			FI_ROTATION,

			/**
			 * Returns the scale of this matrix.
			 * <pre>
			 * Vector3 = HomogenousMatrix4.scale()
			 * </pre>
			 */
			FI_SCALE,

			/**
			 * Sets the rotation of this matrix matrix.
			 * <pre>
			 * HomogenousMatrix4.setRotation(Rotation)
			 * HomogenousMatrix4.setRotation(Quaternion)
			 * HomogenousMatrix4.setRotation(Euler)
			 * HomogenousMatrix4.setRotation(SquareMatrix3)
			 * HomogenousMatrix4.setRotation(HomogenousMatrix4)
			 * </pre>
			 */
			FI_SET_ROTATION,

			/**
			 * Sets the translation of this matrix matrix.
			 * <pre>
			 * HomogenousMatrix4.setTranslation(Vector3)
			 * HomogenousMatrix4.setTranslation(HomogenousMatrix4)
			 * </pre>
			 */
			FI_SET_TRANSLATION,

			/**
			 * Returns a string with the values of this matrix.
			 * <pre>
			 * String = HomogenousMatrix4.string()
			 * </pre>
			 */
			FI_STRING,

			/**
			 * Returns the trace of this matrix.
			 * <pre>
			 * Number = HomogenousMatrix4.trace()
			 * </pre>
			 */
			FI_TRACE,

			/**
			 * Returns the translation of this matrix.
			 * <pre>
			 * Vector3 = HomogenousMatrix4.translation()
			 * </pre>
			 */
			FI_TRANSLATION,

			/**
			 * Returns the transposed matrix of this matrix.
			 * <pre>
			 * SquareMatrix4 = HomogenousMatrix4.transposed()
			 * </pre>
			 */
			FI_TRANSPOSED,

			/**
			 * Returns the x-axis of this matrix.
			 * <pre>
			 * Vector3 = HomogenousMatrix4.xAxis()
			 * </pre>
			 */
			FI_X_AXIS,

			/**
			 * Returns the y-axis of this matrix.
			 * <pre>
			 * Vector3 = HomogenousMatrix4.yAxis()
			 * </pre>
			 */
			FI_Y_AXIS,

			/**
			 * Returns the z-axis of this matrix.
			 * <pre>
			 * Vector3 = HomogenousMatrix4.zAxis()
			 * </pre>
			 */
			FI_Z_AXIS
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

inline const char* JSHomogenousMatrix4::objectName()
{
	return "HomogenousMatrix4";
}

}

}

}

#endif // META_OCEAN_INTERACTION_JS_HOMOGENOUS_MATRIX_4_H
