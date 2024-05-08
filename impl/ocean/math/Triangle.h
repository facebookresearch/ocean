/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_TRIANGLE_H
#define META_OCEAN_MATH_TRIANGLE_H

#include "ocean/math/Math.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

// Forward declaration.
template <typename T> class TriangleT;

/**
 * Definition of the Triangle object, depending on the OCEAN_MATH_USE_SINGLE_PRECISION either with single or double precision float data type.
 * @see TriangleT
 * @ingroup math
 */
typedef TriangleT<Scalar> Triangle;

/**
 * Instantiation of the TriangleT template class using a double precision float data type.
 * @see TriangleT
 * @ingroup math
 */
typedef TriangleT<double> TriangleD;

/**
 * Instantiation of the TriangleT template class using a single precision float data type.
 * @see TriangleT
 * @ingroup math
 */
typedef TriangleT<float> TriangleF;

/**
 * This class implements a base class for all triangle classes.
 * @tparam T Data type used to represent coordinates
 * @see TriangleF, TriangleD, TriangleT2, TriangleT3.
 * @ingroup math
 */
template <typename T>
class TriangleT
{
	public:

		/**
		 * Returns whether a given point, specified as barycentric coordinate, lies inside a triangle.
		 * @param barycentricPoint Point to be checked
		 * @return True, if so
		 */
		static bool isBarycentricInside(const VectorT3<T>& barycentricPoint);

		/**
		 * Returns whether the a barycentric coordinate is valid.
		 * @param barycentric Barycentric coordinate to test
		 * @param epsilon Accuracy epsilon, with range [0, infinity)
		 * @return True, if so
		 */
		static bool isValidBarycentric(const VectorT3<T>& barycentric, const T& epsilon = NumericT<T>::eps());
};

template <typename T>
bool TriangleT<T>::isBarycentricInside(const VectorT3<T>& barycentricPoint)
{
	ocean_assert_accuracy((std::is_same<Scalar, float>::value) || isValidBarycentric(barycentricPoint));

	return barycentricPoint[0] >= -NumericT<T>::eps() && barycentricPoint[1] >= -NumericT<T>::eps() && barycentricPoint[2] >= -NumericT<T>::eps();
}

template <typename T>
bool TriangleT<T>::isValidBarycentric(const VectorT3<T>& barycentric, const T& epsilon)
{
	return NumericT<T>::isEqual(barycentric[0] + barycentric[1] + barycentric[2], T(1), epsilon);
}

}

#endif // META_OCEAN_MATH_TRIANGLE_H
