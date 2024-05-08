/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_BOUNDING_BOX_H
#define META_OCEAN_MATH_BOUNDING_BOX_H

#include "ocean/math/Math.h"
#include "ocean/math/Box3.h"
#include "ocean/math/SquareMatrix3.h"

namespace Ocean
{

/**
 * This class implements a 3D bounding box.
 * @ingroup math
 */
class OCEAN_MATH_EXPORT BoundingBox : public Box3
{
	public:

		/**
		 * Creates an invalid bounding box.
		 */
		inline BoundingBox();

		/**
		 * Creates an invalid bounding box.
		 */
		inline BoundingBox(const Box3& box);

		/**
		 * Creates a new bounding box by two given corners.
		 * @param lower Lower corner
		 * @param higher Higher corner
		 */
		inline BoundingBox(const Vector3& lower, const Vector3& higher);

		/**
		 * Creates a new bounding box enclosing a given set of 3D points.
		 * @param points Points to be enclosed by the bounding box
		 */
		inline explicit BoundingBox(const Vectors3& points);

		/**
		 * Creates a new bounding box enclosing a given set of 3D points.
		 * @param points Points to be enclosed by the bounding box
		 * @param number Number of points
		 */
		inline BoundingBox(const Vector3* points, const unsigned int number);

		/**
		 * Returns the front intersection point between a given ray and this box whenever the distance is positive.
		 * The dot product between the ray direction and the intersection normal will be negative.<br>
		 * @param ray Ray to determine the intersection point for
		 * @param position Resulting intersection position
		 * @param distance Resulting intersection distance
		 * @return True, if so
		 */
		bool positiveFrontIntersection(const Line3& ray, Vector3& position, Scalar& distance) const;

		/**
		 * Returns the front intersection point between a given ray and this box whenever the distance is positive.
		 * The dot product between the ray direction and the intersection normal will be negative.<br>
		 * @param ray Ray to determine the intersection point for
		 * @param position Resulting intersection position
		 * @param distance Resulting intersection distance
		 * @param normal Resulting intersection normal
		 * @return True, if so
		 */
		bool positiveFrontIntersection(const Line3& ray, Vector3& position, Scalar& distance, Vector3& normal) const;

		/**
		 * Returns the front intersection point between a given ray and this box whenever the distance is positive.
		 * The dot product between the ray direction and the intersection normal will be negative.<br>
		 * @param ray Ray to determine the intersection point for
		 * @param position Resulting intersection position
		 * @param distance Resulting intersection distance
		 * @param normal Resulting intersection normal
		 * @param textureCoordinate Resulting texture coordinate
		 * @return True, if so
		 */
		bool positiveFrontIntersection(const Line3& ray, Vector3& position, Scalar& distance, Vector3& normal, Vector2& textureCoordinate) const;

		/**
		 * Returns whether a given ray has an intersection with this box.
		 * @param ray Ray to be tested
		 * @param transformation Transformation of the given world transformation (for the box)
		 * @param invertedTransformation Inverted transformation of the given world transformation (for this box)
		 * @param position Resulting nearest intersection position
		 * @param distance Resulting intersection distance
		 * @return True, if so
		 */
		inline bool positiveFrontIntersection(const Line3& ray, const HomogenousMatrix4& transformation, const HomogenousMatrix4& invertedTransformation, Vector3& position, Scalar& distance) const;

		/**
		 * Returns whether a given ray has an intersection with this box.
		 * @param ray Ray to be tested
		 * @param transformation Transformation of the given world transformation (for the box)
		 * @param invertedTransformation Inverted transformation of the given world transformation (for this box)
		 * @param position Resulting nearest intersection position
		 * @param distance Resulting intersection distance
		 * @param normal Resulting intersection normal
		 * @return True, if so
		 */
		inline bool positiveFrontIntersection(const Line3& ray, const HomogenousMatrix4& transformation, const HomogenousMatrix4& invertedTransformation, Vector3& position, Scalar& distance, Vector3& normal) const;

		/**
		 * Returns the front intersection point between a given ray and this box whenever the distance is positive.
		 * The dot product between the ray direction and the intersection normal will be negative.<br>
		 * @param ray Ray to determine the intersection point for
		 * @param transformation Transformation of the given world transformation (for the box)
		 * @param invertedTransformation Inverted transformation of the given world transformation (for this box)
		 * @param position Resulting intersection position
		 * @param distance Resulting intersection distance
		 * @param normal Resulting intersection normal
		 * @param textureCoordinate Resulting texture coordinate
		 * @return True, if so
		 */
		inline bool positiveFrontIntersection(const Line3& ray, const HomogenousMatrix4& transformation, const HomogenousMatrix4& invertedTransformation, Vector3& position, Scalar& distance, Vector3& normal, Vector2& textureCoordinate) const;

		/**
		 * Returns the back intersection point between a given ray and this box whenever the distance is positive.
		 * The dot product between the ray direction and the intersection normal will be positive.<br>
		 * @param ray Ray to determine the intersection point for
		 * @param position Resulting intersection position
		 * @param distance Resulting intersection distance
		 * @return True, if so
		 */
		bool positiveBackIntersection(const Line3& ray, Vector3& position, Scalar& distance) const;

		/**
		 * Returns the back intersection point between a given ray and this box whenever the distance is positive.
		 * The dot product between the ray direction and the intersection normal will be positive.<br>
		 * @param ray Ray to determine the intersection point for
		 * @param position Resulting intersection position
		 * @param distance Resulting intersection distance
		 * @param normal Resulting intersection normal
		 * @return True, if so
		 */
		bool positiveBackIntersection(const Line3& ray, Vector3& position, Scalar& distance, Vector3& normal) const;

		/**
		 * Returns the back intersection point between a given ray and this box whenever the distance is positive.
		 * The dot product between the ray direction and the intersection normal will be positive.<br>
		 * @param ray Ray to determine the intersection point for
		 * @param position Resulting intersection position
		 * @param distance Resulting intersection distance
		 * @param normal Resulting intersection normal
		 * @param textureCoordinate Resulting texture coordinate
		 * @return True, if so
		 */
		bool positiveBackIntersection(const Line3& ray, Vector3& position, Scalar& distance, Vector3& normal, Vector2& textureCoordinate) const;

		/**
		 * Returns the back intersection point between a given ray and this box whenever the distance is positive.
		 * The dot product between the ray direction and the intersection normal will be positive.<br>
		 * @param ray Ray to determine the intersection point for
		 * @param transformation Transformation of the given world transformation (for the sphere)
		 * @param invertedTransformation Inverted transformation of the given world transformation (for this box)
		 * @param position Resulting intersection position
		 * @param distance Resulting intersection distance
		 * @return True, if so
		 */
		inline bool positiveBackIntersection(const Line3& ray, const HomogenousMatrix4& transformation, const HomogenousMatrix4& invertedTransformation, Vector3& position, Scalar& distance) const;

		/**
		 * Returns the back intersection point between a given ray and this box whenever the distance is positive.
		 * The dot product between the ray direction and the intersection normal will be positive.<br>
		 * @param ray Ray to determine the intersection point for
		 * @param transformation Transformation of the given world transformation (for the sphere)
		 * @param invertedTransformation Inverted transformation of the given world transformation (for this box)
		 * @param position Resulting intersection position
		 * @param distance Resulting intersection distance
		 * @param normal Resulting intersection normal
		 * @return True, if so
		 */
		inline bool positiveBackIntersection(const Line3& ray, const HomogenousMatrix4& transformation, const HomogenousMatrix4& invertedTransformation, Vector3& position, Scalar& distance, Vector3& normal) const;

		/**
		 * Returns the back intersection point between a given ray and this box whenever the distance is positive.
		 * The dot product between the ray direction and the intersection normal will be positive.<br>
		 * @param ray Ray to determine the intersection point for
		 * @param transformation Transformation of the given world transformation (for the sphere)
		 * @param invertedTransformation Inverted transformation of the given world transformation (for this box)
		 * @param position Resulting intersection position
		 * @param distance Resulting intersection distance
		 * @param normal Resulting intersection normal
		 * @param textureCoordinate Resulting texture coordinate
		 * @return True, if so
		 */
		inline bool positiveBackIntersection(const Line3& ray, const HomogenousMatrix4& transformation, const HomogenousMatrix4& invertedTransformation, Vector3& position, Scalar& distance, Vector3& normal, Vector2& textureCoordinate) const;
};

inline BoundingBox::BoundingBox() :
	Box3()
{
	// nothing to do here
}

inline BoundingBox::BoundingBox(const Box3& box) :
	Box3(box)
{
	// nothing to do here
}

inline BoundingBox::BoundingBox(const Vector3& lower, const Vector3& higher) :
	Box3(lower, higher)
{
	// nothing to do here
}

inline BoundingBox::BoundingBox(const Vectors3& points) :
	Box3(points)
{
	// nothing to do here
}

inline BoundingBox::BoundingBox(const Vector3* points, const unsigned int number) :
	Box3(points, number)
{
	// nothing to do here
}

inline bool BoundingBox::positiveFrontIntersection(const Line3& ray, const HomogenousMatrix4& transformation, const HomogenousMatrix4& invertedTransformation, Vector3& position, Scalar& distance) const
{
	ocean_assert(invertedTransformation.isValid());
	ocean_assert(ray.isValid());

	if (positiveFrontIntersection(Line3(invertedTransformation * ray.point(), invertedTransformation.rotationMatrix(ray.direction())), position, distance))
	{
		position = transformation * position;
		return true;
	}

	return false;
}

inline bool BoundingBox::positiveFrontIntersection(const Line3& ray, const HomogenousMatrix4& transformation, const HomogenousMatrix4& invertedTransformation, Vector3& position, Scalar& distance, Vector3& normal) const
{
	ocean_assert(invertedTransformation.isValid());
	ocean_assert(ray.isValid());

	if (positiveFrontIntersection(Line3(invertedTransformation * ray.point(), invertedTransformation.rotationMatrix(ray.direction())), position, distance, normal))
	{
		position = transformation * position;
		normal = invertedTransformation.transposedRotationMatrix(normal).normalizedOrZero();
		return true;
	}

	return false;
}

inline bool BoundingBox::positiveFrontIntersection(const Line3& ray, const HomogenousMatrix4& transformation, const HomogenousMatrix4& invertedTransformation, Vector3& position, Scalar& distance, Vector3& normal, Vector2& textureCoordinate) const
{
	ocean_assert(invertedTransformation.isValid());
	ocean_assert(ray.isValid());

	if (positiveFrontIntersection(Line3(invertedTransformation * ray.point(), invertedTransformation.rotationMatrix(ray.direction())), position, distance, normal, textureCoordinate))
	{
		position = transformation * position;
		normal = invertedTransformation.transposedRotationMatrix(normal).normalizedOrZero();
		return true;
	}

	return false;
}

inline bool BoundingBox::positiveBackIntersection(const Line3& ray, const HomogenousMatrix4& transformation, const HomogenousMatrix4& invertedTransformation, Vector3& position, Scalar& distance) const
{
	ocean_assert(invertedTransformation.isValid());
	ocean_assert(ray.isValid());

	if (positiveBackIntersection(Line3(invertedTransformation * ray.point(), invertedTransformation.rotationMatrix(ray.direction())), position, distance))
	{
		position = transformation * position;
		return true;
	}

	return false;
}

inline bool BoundingBox::positiveBackIntersection(const Line3& ray, const HomogenousMatrix4& transformation, const HomogenousMatrix4& invertedTransformation, Vector3& position, Scalar& distance, Vector3& normal) const
{
	ocean_assert(invertedTransformation.isValid());
	ocean_assert(ray.isValid());

	if (positiveBackIntersection(Line3(invertedTransformation * ray.point(), invertedTransformation.rotationMatrix(ray.direction())), position, distance, normal))
	{
		position = transformation * position;
		normal = invertedTransformation.transposedRotationMatrix(normal).normalizedOrZero();
		return true;
	}

	return false;
}

inline bool BoundingBox::positiveBackIntersection(const Line3& ray, const HomogenousMatrix4& transformation, const HomogenousMatrix4& invertedTransformation, Vector3& position, Scalar& distance, Vector3& normal, Vector2& textureCoordinate) const
{
	ocean_assert(invertedTransformation.isValid());
	ocean_assert(ray.isValid());

	if (positiveBackIntersection(Line3(invertedTransformation * ray.point(), invertedTransformation.rotationMatrix(ray.direction())), position, distance, normal, textureCoordinate))
	{
		position = transformation * position;
		normal = invertedTransformation.transposedRotationMatrix(normal).normalizedOrZero();
		return true;
	}

	return false;
}

}

#endif // META_OCEAN_MATH_BOUNDING_BOX_H
