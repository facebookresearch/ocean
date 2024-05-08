/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_BOX_3_H
#define META_OCEAN_MATH_BOX_3_H

#include "ocean/math/Math.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Line3.h"
#include "ocean/math/Plane3.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

// Forward declaration.
template <typename T> class BoxT3;

/**
 * Definition of the Box3 object, depending on the OCEAN_MATH_USE_SINGLE_PRECISION either with single or double precision float data type.
 * @see BoxT3
 * @ingroup math
 */
typedef BoxT3<Scalar> Box3;

/**
 * Instantiation of the BoxT3 template class using a double precision float data type.
 * @see BoxT3
 * @ingroup math
 */
typedef BoxT3<double> BoxD3;

/**
 * Instantiation of the BoxT3 template class using a single precision float data type.
 * @see BoxT3
 * @ingroup math
 */
typedef BoxT3<float> BoxF3;

/**
 * Definition of a typename alias for vectors with BoxT3 objects.
 * @see BoxT3
 * @ingroup math
 */
template <typename T>
using BoxesT3 = std::vector<BoxT3<T>>;

/**
 * Definition of a vector holding Box3 objects.
 * @see Box3
 * @ingroup math
 */
typedef std::vector<Box3> Boxes3;

/**
 * This class implements an axis aligned 3D bounding box.
 * @tparam T The data type of the scalar to be used, either 'float' or 'double'
 * @see Box2, BoxF3, BoxD3, BoxT2.
 * @ingroup math
 */
template <typename T>
class BoxT3
{
	public:

		/**
		 * Creates an invalid bounding box.
		 */
		BoxT3() = default;

		/**
		 * Creates a new bounding box by two given corners.
		 * @param lower The lower corner
		 * @param higher The higher corner
		 */
		BoxT3(const VectorT3<T>& lower, const VectorT3<T>& higher);

		/**
		 * Creates a new bounding box enclosing a given set of 3D points.
		 * @param points The points to be enclosed by the bounding box
		 */
		explicit BoxT3(const VectorsT3<T>& points);

		/**
		 * Creates a new bounding box enclosing a given set of 3D points.
		 * @param points The points to be enclosed by the bounding box, must be valid
		 * @param number The number of points, with range [1, infinity)
		 */
		BoxT3(const VectorT3<T>* points, const size_t number);

		/**
		 * Creates a new bounding box with defined dimensions centered at a given 3D location.
		 * @param center The center position of the new bounding box, with range (-infinity, infinity)x(-infinity, infinity)x(-infinity, infinity)
		 * @param xSize The size of the bounding box in the x-axis, with range [0, infinity)
		 * @param ySize The size of the bounding box in the y-axis, with range [0, infinity)
		 * @param zSize The size of the bounding box in the z-axis, with range [0, infinity)
		 */
		BoxT3(const VectorT3<T>& center, const T xSize, const T ySize, const T zSize);

		/**
		 * Returns the center of the box.
		 * @return Box center
		 */
		VectorT3<T> center() const;

		/**
		 * Returns the square diagonal of this box.
		 * @return Square diagonal
		 */
		T sqrDiagonal() const;

		/**
		 * Returns the diagonal of this box.
		 * @return Box diagonal
		 */
		T diagonal() const;

		/**
		 * Returns the dimension in x axis, which could e.g. be the width of this box.
		 * Beware: The result is undefined for an invalid box.
		 * @return Dimension in x axis
		 */
		T xDimension() const;

		/**
		 * Returns the dimension in y axis, which could e.g. be the height of this box.
		 * Beware: The result is undefined for an invalid box.
		 * @return Dimension in y axis
		 */
		T yDimension() const;

		/**
		 * Returns the dimension in z axis, which could e.g. be the depth of this box.
		 * Beware: The result is undefined for an invalid box.
		 * @return Dimension in z axis
		 */
		T zDimension() const;

		/**
		 * Returns the dimension of this box for all three axis.
		 * @return The box's dimension
		 */
		VectorT3<T> dimension() const;

		/**
		 * Returns whether this box defines one single point only.
		 * @param point Optional resulting point defined by the box
		 * @return True, if so
		 */
		bool isPoint(VectorT3<T>* point = nullptr) const;

		/**
		 * Returns whether this box is planar aligned to one axis.
		 * A planar box has no expansion in one axis.
		 * @param plane The plane the box is part of
		 * @return True, if so
		 */
		bool isPlanar(PlaneT3<T>& plane) const;

		/**
		 * Returns the lower corner of the box.
		 * @return Lower corner
		 */
		const VectorT3<T>& lower() const;

		/**
		 * Returns the higher corner of the box.
		 * @return Higher corner
		 */
		const VectorT3<T>& higher() const;

		/**
		 * Returns whether a given point is inside this bounding box.
		 * @param point The point to check
		 * @param eps The optional epsilon adding an additional thin tolerance boundary, with range [0, infinity)
		 * @return True, if so
		 */
		bool isInside(const VectorT3<T>& point, const T eps = T(0)) const;

		/**
		 * Returns whether a given point lies on the surface of this box.
		 * @param point The point to be checked
		 * @param epsilon The accuracy value allowing some tolerance, with range [0, infinity)
		 * @return True, if so
		 */
		bool isOnSurface(const VectorT3<T>& point, const T epsilon = NumericT<T>::eps()) const;

		/**
		 * Returns whether a given ray has an intersection with this box.
		 * @param ray The ray to be tested, must be valid
		 * @return True, if so
		 */
		bool hasIntersection(const LineT3<T>& ray) const;

		/**
		 * Returns whether a given ray has an intersection with this box while applying a distance-dependent epsilon threshold.
		 * The larger the distance between the ray's origin and the box, the bigger the applied epsilon threshold.
		 * @param ray The ray to be tested, must be valid
		 * @param epsPerDistance The epsilon for distance 1 which will be multiplied with the (approximated) distance to determine the actual epsilon to be used, with range [0, infinity)
		 * @return True, if so
		 */
		bool hasIntersection(const LineT3<T>& ray, const T epsPerDistance) const;

		/**
		 * Returns whether a given ray has an intersection with this box.
		 * @param ray The ray to be tested, must be valid
		 * @param box_T_ray The transformation between ray and box, must be valid
		 * @return True, if so
		 */
		bool hasIntersection(const LineT3<T>& ray, const HomogenousMatrixT4<T>& box_T_ray) const;

		/**
		 * Returns the corner positions of this box.
		 * If this box is planar the four points are returned only.
		 * @param corners Resulting corners, must provide space for at least eight points
		 * @return Number of resulting corners
		 */
		unsigned int corners(VectorT3<T>* corners) const;

		/**
		 * Returns an expanded box of this box.
		 * Positive offsets will increase the box, negative offsets will decrease the box; offsets smaller than the dimension are clamped to zero.
		 * @param offsets The offsets along all three axis, an offset of +/-1 makes the box 1 unit larger/smaller, with range (-infinity, infinity)x(-infinity, infinity)x(-infinity, infinity)
		 * @return The new expanded box
		 */
		BoxT3<T> expanded(const VectorT3<T>& offsets) const;

		/**
		 * Expands this box.
		 * Positive offsets will increase the box, negative offsets will decrease the box; offsets smaller than the dimension are clamped to zero.
		 * @param offsets The offsets along all three axis, an offset of +/-1 makes the box 1 unit larger/smaller, with range (-infinity, infinity)x(-infinity, infinity)x(-infinity, infinity)
		 * @return Reference to this box
		 */
		BoxT3<T>& expand(const VectorT3<T>& offsets);

		/**
		 * Clears and resets the bounding box to an invalid box.
		 */
		void clear();

		/**
		 * Returns whether two box objects are equal up to an epsilon.
		 * @param box The Box to compare with, can be invalid
		 * @param epsilon The accuracy epsilon, with range [0, infinity)
		 * @return True, if so
		 */
		bool isEqual(const BoxT3<T>& box, const T epsilon = NumericT<T>::eps()) const;

		/**
		 * Returns whether the bounding box is valid.
		 * @return True, if so
		 */
		bool isValid() const;

		/**
		 * Returns an enlarged bounding box of this one.
		 * @param factor The factor to enlarge the bounding box in each dimension, with range (-infinity, infinity)
		 * @return New enlarged bounding box
		 */
		BoxT3<T> operator*(const T factor) const;

		/**
		 * Enlarges the bounding box by a given factor in each dimension.
		 * @param factor The factor to enlarge the bounding box in each dimension, with range (-infinity, infinity)
		 * @return Reference to this (enlarged) bounding box
		 */
		BoxT3<T>& operator*=(const T factor);

		/**
		 * Adds a new point to this bounding box and updates it's dimension.
		 * @param point The point to add to the bounding box
		 * @return Reference to this bounding box
		 */
		BoxT3<T>& operator+=(const VectorT3<T>& point);

		/**
		 * Returns the union of two bounding boxes.
		 * @param right The right bounding box
		 * @return New joined bounding box
		 */
		BoxT3<T> operator+(const BoxT3<T>& right) const;

		/**
		 * Joins to bounding boxes.
		 * @param right The right bounding box to join
		 * @return Reference to this (joined) bounding box
		 */
		BoxT3<T>& operator+=(const BoxT3<T>& right);

		/**
		 * Returns the (axis-aligned) world bounding box for a given transformation between the box and world.
		 * @param world_T_box The transformation between box and world, must be valid
		 * @return The new (still) axis-aligned bounding box, defined in world, most likely the new box will have a larger volume than this box
		 */
		BoxT3<T> operator*(const HomogenousMatrixT4<T>& world_T_box) const;

		/**
		 * Transforms this bounding box with a given transformation so that the this bounding box is defined in world afterwards.
		 * @param world_T_box The transformation between box and world, must be valid
		 * @return Reference to this new bounding box now defined in world
		 */
		BoxT3<T>& operator*=(const HomogenousMatrixT4<T>& world_T_box);

		/**
		 * Returns whether two boxes are identical.
		 * @param right Second box object
		 * @return True, if so
		 */
		bool operator==(const BoxT3<T>& right) const;

		/**
		 * Returns whether two boxes are not identical.
		 * @param right Second box object
		 * @return True, if so
		 */
		bool operator!=(const BoxT3<T>& right) const;

		/**
		 * Returns whether this box is not a default box.
		 * @return True, if so
		 */
		operator bool() const;

	protected:

		/// Lower corner of the bounding box.
		VectorT3<T> lower_ = VectorT3<T>(NumericT<T>::maxValue(), NumericT<T>::maxValue(), NumericT<T>::maxValue());

		/// Higher corner of the bounding box.
		VectorT3<T> higher_ = VectorT3<T>(NumericT<T>::minValue(), NumericT<T>::minValue(), NumericT<T>::minValue());
};

}

#endif // META_OCEAN_MATH_BOX_3_H
