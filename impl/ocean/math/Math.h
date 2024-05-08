/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_MATH_H
#define META_OCEAN_MATH_MATH_H

#include "ocean/base/Base.h"
#include "ocean/base/Messenger.h"

#include <vector>

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41
	// SSE2 include files
	#include <emmintrin.h>
	#include <mmintrin.h>

	// SSE3 include files
	#include <pmmintrin.h>
	#include <mmintrin.h>

	// SSE4 include files
	#include <smmintrin.h>

	#if defined(OCEAN_HARDWARE_AVX_VERSION) && OCEAN_HARDWARE_AVX_VERSION > 0
		// AVX include files
		#include <immintrin.h>
	#endif
#endif

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
	#if defined(__ARM_NEON__) || defined(__ARM_NEON)
		#include <arm_neon.h>
	#endif // __ARM_NEON__
#endif

namespace Ocean
{

/**
 * @defgroup math Ocean Math Library
 * @{
 * The Ocean Math Library provides basic numerical and algebraic functionalities.
 * The library is platform independent.<br>
 *
 * The Math Library supports floating point numbers with single (32bit) and double (64bit) precision.<br>
 * The library defines an own type for floating point numbers allowing to compile the entire library for a specific default type of floating point numbers (either float or double).<br>
 * Thus, use the Scalar type to implement code for either 32bit or 64bit floating point values.<br>
 * The preprocessor define 'OCEAN_MATH_USE_SINGLE_PRECISION' distinguishes whether single or double precision is used as default floating point numbers.<br>
 * @see Scalar.
 *
 * The library implements template-based classes allowing the explicit application of single or double precision floating point numbers.<br>
 * The template-based classes are necessary to store or convert mathematical data between the Ocean framework and 3rd party libraries which rely either on 32bit or 64bit floating point numbers.<br>
 * Further, the library implements classes that consistently applies the Scalar default floating point type avoiding a template-based implementation.<br>
 * All template-based class names end with a 'T' identifying the template implementation.<br>
 * However, for each template class three typedef definitions exist: a single,a double and a default precision version.<br>
 * The single precision version ends with a 'F' (for float), the double precision version ends with a 'D' (for double).<br>
 * The name of the default precision version is the normal class name without 'T', 'F' or 'D'.<br>
 * @see VectorT3, VectorF3, VectorD3, Vector3.
 * @see PlaneT3, PlaneF3, PlaneD3, Plane3.
 * @see VarianceT, VarianceF, VarianceD, Variance.
 *
 * The NumericT class provides template-based access to numerical functions for single (NumericF) and double (NumericD) precision floating point numbers.<br>
 * Use the default Numeric class to access the numerical functions with default floating point number type.<br>
 * @see NumericT, NumericF, NumericD, Numeric.
 *
 * The Math Library provides 2D, 3D and 4D vector classes with a wide variety of functions necessary for linear algebra tasks.<br>
 * @see VectorT2, VectorT3, VectorT4.
 *
 * The SquareMatrix2, SquareMatrix3 and SquareMatrix4 classes can be used for e.g., matrix-matrix or matrix-vector multipliations.<br>
 * Each of the three matrix classes implement a square matrix with either 2, 3 or 4 rows and columns.<br>
 * @see SquareMatrixT2, SquareMatrixT3, SquareMatrixT4.
 *
 * The HomogenousMatrix4 is comparable to the SquareMatrix4 class.<br>
 * However, the homogenous matrix is designed to store homogenous transformations in 3D space like e.g., camera poses or coordinate system transformations.<br>
 * @see HomogenousMatrix4.
 *
 * Matrix object with arbitrary (dynamic or static dimensions) are implemented by the classes Matrix and StaticMatrix.<br>
 * A sparse matrix is realized in the SparseMatrix class.<br>
 * @see Matrix, StaticMatrix, SparseMatrix.<br>
 *
 * Rotation in 3D space can be expressed by the Euler, ExponentialMap, Quaternion, Rotation, SquareMatrix3 classes.<br>
 * @see EulerT, ExponentialMap, QuaternionT, RotationT, SquareMatrixT3.
 *
 * An extrinsic camera matrix (a camera pose) can be represented by an HomogenousMatrix4 object or by a Pose object.<br>
 * An intrinsic camera matrix (camera profile) is implemented by the PinholeCamera class further supporting camera distortion profiles.<br>
 * @see HomogenousMatrix4, Pose.
 *
 * Further, the library supports 2D and 3D finite and infinite lines (rays) and 2D and 3D triangles.<br>
 * @see Line2, Line3, FiniteLine2, FiniteLine3, Triangle2, Triangle3.<br>
 *
 * The Plane3 class implements an infinite 3D plane.<br>
 * @see Plane3.<br>
 *
 * 2D rectangles, 3D boxes and sphere as well as bounding volumes are realized by the Box2, Box3, Sphere3, BoundingBox and BoundingSphere classes.<br>
 * @see Box2, Box3, Sphere3, BoundingBox, BoundingSphere.
 *
 * A Fourier transformation and a discrete cosine transformation can be accessed by the FourierTransformation and DiscreteCosineTransformation classes.<br>
 * @see FourierTransformation, DiscreteCosineTransformation.<br>
 *
 * However, the Math Library provides several further classes and functions.
 *
 * In general, the Math library (but also the remaining entire framework) is using a right-handed coordinate system (as long as not explicitly documented differently).<br>
 * Please refer to the Geometry library regarding a more detailed overview of the individual coordinate systems and how the framework handles transformations by default.
 * @}
 */

// @cond
// Selection of the scalar floating precision.
// Define OCEAN_MATH_USE_SINGLE_PRECISION to use 32bit floating point values.
#ifdef OCEAN_HARDWARE_REDUCED_PERFORMANCE
	#ifndef OCEAN_MATH_USE_SINGLE_PRECISION
		#define OCEAN_MATH_USE_SINGLE_PRECISION
	#endif
#else
	//#define OCEAN_MATH_USE_SINGLE_PRECISION
#endif
// @endcond

#ifdef OCEAN_MATH_USE_SINGLE_PRECISION

	/**
	 * Definition of a scalar type.
	 * @ingroup math
	 */
	typedef float Scalar;

#else

	/**
	 * Definition of a scalar type.
	 * @ingroup math
	 */
	typedef double Scalar;

#endif

/**
 * Definition of a vector holding Scalar objects.
 * @ingroup math
 */
typedef std::vector<Scalar> Scalars;

// Defines OCEAN_MATH_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_MATH_EXPORT
		#define OCEAN_MATH_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_MATH_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_MATH_EXPORT
#endif

}

#endif // META_OCEAN_MATH_MATH_H
