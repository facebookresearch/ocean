/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_COMPLEX_H
#define META_OCEAN_MATH_COMPLEX_H

#include "ocean/math/Math.h"

#include <complex>

namespace Ocean
{

/**
 * Definition of a complex number based on the double precision floating point data type.
 * @ingroup math
 */
using ComplexD = std::complex<double>;

/**
 * Definition of a complex number based on the single precision floating point data type.
 * @ingroup math
 */
using ComplexF = std::complex<float>;

/**
 * Definition of a complex number based on the default floating point precision data type.
 * @ingroup math
 */
using Complex = std::complex<Scalar>;

/**
 * Definition of a vector holding complex objects.
 * @ingroup math
 */
using Complexes = std::vector<Complex>;

}

#endif // META_OCEAN_MATH_COMPLEX_H
