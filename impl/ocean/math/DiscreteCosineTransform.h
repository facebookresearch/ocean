/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_DISCRETE_COSINE_TRANSFORM_H
#define META_OCEAN_MATH_DISCRETE_COSINE_TRANSFORM_H

#include "ocean/math/Math.h"

namespace Ocean
{

/**
 * This class implements discrete cosine transform functions.
 * @ingroup math
 */
class OCEAN_MATH_EXPORT DiscreteCosineTransform
{
	public:

		/**
		 * Applies a 2D discrete cosine transform for a given 2D data block.
		 * The resulting coefficients are stored in an row aligned order.<br>
		 * @param data Data block for that the transform is applied
		 * @param xElements Number of horizontal data elements, with range [1, infinity)
		 * @param yElements Number of vertical data elements, with range [1, infinity)
		 * @param coefficients Resulting number of coefficients, an array with xElements * yElements values must be provided
		 */
		static inline void transform2(const Scalar* data, const unsigned int xElements, const unsigned int yElements, Scalar* coefficients);

		/**
		 * Applies a 2D discrete cosine transform for a given 2D data block that might be larger than the actual DCT data block.
		 * The resulting coefficients are stored in an row aligned order.<br>
		 * @param data Data block for that the transform is applied (pointer to the first element)
		 * @param xElements Number of horizontal data elements, with range [1, infinity)
		 * @param yElements Number of vertical data elements, with range [1, infinity)
		 * @param stride Width of the entire data information including the xElements, with range [xElements, infinity)
		 * @param coefficients Resulting number of coefficients, an array with xElements * yElements values must be provided
		 */
		static void transform2(const Scalar* data, const unsigned int xElements, const unsigned int yElements, const unsigned int stride, Scalar* coefficients);

		/**
		 * Applies a inverse 2D discrete cosine transform for a given set of DCT coefficients.
		 * @param coefficients DCT coefficients from that the data information will be recovered
		 * @param xElements Number of horizontal data elements, with range [1, infinity)
		 * @param yElements Number of vertical data elements, with range [1, infinity)
		 * @param data Resulting data block, an array with xElements * yElements values must be provided
		 */
		static inline void iTransform2(const Scalar* coefficients, const unsigned int xElements, const unsigned int yElements, Scalar* data);

		/**
		 * Applies a inverse 2D discrete cosine transform for a given set of DCT coefficients.
		 * @param coefficients DCT coefficients from that the data information will be recovered
		 * @param xElements Number of horizontal data elements, with range [1, infinity)
		 * @param yElements Number of vertical data elements, with range [1, infinity)
		 * @param stride Width of the data information including the xElements, with range [xElements, infinity)
		 * @param data Resulting data block, a data block that might be larger than the actual number of DCT coefficients as an explicit stride parameter can be defined
		 */
		static void iTransform2(const Scalar* coefficients, const unsigned int xElements, const unsigned int yElements, const unsigned int stride, Scalar* data);
};

inline void DiscreteCosineTransform::transform2(const Scalar* data, const unsigned int xElements, const unsigned int yElements, Scalar* coefficients)
{
	transform2(data, xElements, yElements, xElements, coefficients);
}

inline void DiscreteCosineTransform::iTransform2(const Scalar* coefficients, const unsigned int xElements, const unsigned int yElements, Scalar* data)
{
	iTransform2(coefficients, xElements, yElements, xElements, data);
}

}

#endif // META_OCEAN_MATH_DISCRETE_COSINE_TRANSFORM_H
