/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_INTERACTION_JS_SQUARE_MATRIX_4_H
#define META_OCEAN_INTERACTION_JS_SQUARE_MATRIX_4_H

#include "ocean/interaction/javascript/JavaScript.h"
#include "ocean/interaction/javascript/JSObject.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

/**
 * This class implements a wrapper for a JavaScript SquareMatrix4 object.
 * @ingroup interactionjs
 */
class OCEAN_INTERACTION_JS_EXPORT JSSquareMatrix4 : public JSObject<JSSquareMatrix4, SquareMatrix4>
{
	friend class JSObject<JSSquareMatrix4, SquareMatrix4>;
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
			 * SquareMatrix4 = SquareMatrix4.add(SquareMatrix4)
			 * </pre>
			 */
			FI_ADD,

			/**
			 * Determines the matrix's determinant.
			 * <pre>
			 * Number = SquareMatrix4.determinant()
			 * </pre>
			 */
			FI_DETERMINANT,

			/**
			 * Inverts this matrix.
			 * <pre>
			 * Boolean = SquareMatrix4.invert()
			 * </pre>
			 */
			FI_INVERT,

			/**
			 * Returns the inverted matrix of this matrix.
			 * <pre>
			 * SquareMatrix4 = SquareMatrix4.inverted()
			 * </pre>
			 */
			FI_INVERTED,

			/**
			 * Returns whether two matrices are equal up to some epsilon.
			 * <pre>
			 * Boolean = SquareMatrix4.isEqual(SquareMatrix4)
			 * </pre>
			 */
			FI_IS_EQUAL,

			/**
			 * Multiplies two matrices, mutiplies this matrix with a vector, or multiplies this matrix with a scalar.
			 * <pre>
			 * SquareMatrix4 = SquareMatrix4.multiply(SquareMatrix4)
			 * SquareMatrix4 = SquareMatrix4.multiply(HomogenousMatrix4)
			 * Vector3 = SquareMatrix4.multiply(Vector3)
			 * Vector4 = SquareMatrix4.multiply(Vector4)
			 * SquareMatrix4 = SquareMatrix4.multiply(Number)
			 * </pre>
			 */
			FI_MULTIPLY,

			/**
			 * Returns a string with the values of this matrix.
			 * <pre>
			 * String = SquareMatrix4.string()
			 * </pre>
			 */
			FI_STRING,

			/**
			 * Subtracts two matrices.
			 * <pre>
			 * SquareMatrix4 = SquareMatrix4.subtract(SquareMatrix4)
			 * </pre>
			 */
			FI_SUBTRACT,

			/**
			 * Returns the trace of this matrix.
			 * <pre>
			 * Number = SquareMatrix4.trace()
			 * </pre>
			 */
			FI_TRACE,

			/**
			 * Transposes this matrix.
			 * <pre>
			 * SquareMatrix4.transpose()
			 * </pre>
			 */
			FI_TRANSPOSE,

			/**
			 * Returns the transposed matrix of this matrix.
			 * <pre>
			 * SquareMatrix4 = SquareMatrix4.transposed()
			 * </pre>
			 */
			FI_TRANSPOSED
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

inline const char* JSSquareMatrix4::objectName()
{
	return "SquareMatrix4";
}

}

}

}

#endif // META_OCEAN_INTERACTION_JS_SQUARE_MATRIX_3_H
