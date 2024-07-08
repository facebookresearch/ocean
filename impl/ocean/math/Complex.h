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
typedef std::complex<double> ComplexD;

/**
 * Definition of a complex number based on the single precision floating point data type.
 * @ingroup math
 */
typedef std::complex<float> ComplexF;

/**
 * Definition of a complex number based on the default floating point precision data type.
 * @ingroup math
 */
typedef std::complex<Scalar> Complex;

/**
 * Definition of a vector holding complex objects.
 * @ingroup math
 */
typedef std::vector<Complex> Complexes;

}

#endif // META_OCEAN_MATH_COMPLEX_H
