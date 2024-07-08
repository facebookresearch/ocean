/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_INTERACTION_JS_SQUARE_MATRIX_3_H
#define META_OCEAN_INTERACTION_JS_SQUARE_MATRIX_3_H

#include "ocean/interaction/javascript/JavaScript.h"
#include "ocean/interaction/javascript/JSObject.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

/**
 * This class implements a wrapper for a JavaScript SquareMatrix3 object.
 * @ingroup interactionjs
 */
class OCEAN_INTERACTION_JS_EXPORT JSSquareMatrix3 : public JSObject<JSSquareMatrix3, SquareMatrix3>
{
	friend class JSObject<JSSquareMatrix3, SquareMatrix3>;
	friend class JSLibrary;

	public:

		/**
		 * Definition of ids for individual functions.
		 */
		enum FunctionId : unsigned int
		{
			/**
			 * Adds two matrices.
			 * <pre>
			 * SquareMatrix3 = SquareMatrix3.add(SquareMatrix3)
			 * </pre>
			 */
			FI_ADD,

			/**
			 * Determines the matrix's determinant.
			 * <pre>
			 * Number = SquareMatrix3.determinant()
			 * </pre>
			 */
			FI_DETERMINANT,

			/**
			 * Inverts this matrix.
			 * <pre>
			 * Boolean = SquareMatrix3.invert()
			 * </pre>
			 */
			FI_INVERT,

			/**
			 * Returns the inverted matrix of this matrix.
			 * <pre>
			 * SquareMatrix3 = SquareMatrix3.inverted()
			 * </pre>
			 */
			FI_INVERTED,

			/**
			 * Returns whether two matrices are equal up to some epsilon.
			 * <pre>
			 * Boolean = SquareMatrix3.isEqual(SquareMatrix3)
			 * </pre>
			 */
			FI_IS_EQUAL,

			/**
			 * Multiplies two matrices, mutiplies this matrix with a vector, or multiplies this matrix with a scalar.
			 * <pre>
			 * SquareMatrix3 = SquareMatrix3.multiply(SquareMatrix3)
			 * Vector3 = SquareMatrix3.multiply(Vector3)
			 * Vector2 = SquareMatrix3.multiply(Vector2)
			 * SquareMatrix3 = SquareMatrix3.multiply(Number)
			 * </pre>
			 */
			FI_MULTIPLY,

			/**
			 * Returns a string with the values of this matrix.
			 * <pre>
			 * String = SquareMatrix3.string()
			 * </pre>
			 */
			FI_STRING,

			/**
			 * Subtracts two matrices.
			 * <pre>
			 * SquareMatrix3 = SquareMatrix3.subtract(SquareMatrix3)
			 * </pre>
			 */
			FI_SUBTRACT,

			/**
			 * Returns the trace of this matrix.
			 * <pre>
			 * Number = SquareMatrix3.trace()
			 * </pre>
			 */
			FI_TRACE,

			/**
			 * Transposes this matrix.
			 * <pre>
			 * SquareMatrix3.transpose()
			 * </pre>
			 */
			FI_TRANSPOSE,

			/**
			 * Returns the transposed matrix of this matrix.
			 * <pre>
			 * SquareMatrix3 = SquareMatrix3.transposed()
			 * </pre>
			 */
			FI_TRANSPOSED,

			/**
			 * Returns the x-axis of this matrix.
			 * <pre>
			 * Vector3 = SquareMatrix3.xAxis()
			 * </pre>
			 */
			FI_X_AXIS,

			/**
			 * Returns the y-axis of this matrix.
			 * <pre>
			 * Vector3 = SquareMatrix3.yAxis()
			 * </pre>
			 */
			FI_Y_AXIS,

			/**
			 * Returns the z-axis of this matrix.
			 * <pre>
			 * Vector3 = SquareMatrix3.zAxis()
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

inline const char* JSSquareMatrix3::objectName()
{
	return "SquareMatrix3";
}

}

}

}

#endif // META_OCEAN_INTERACTION_JS_SQUARE_MATRIX_3_H
