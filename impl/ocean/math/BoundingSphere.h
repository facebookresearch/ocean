/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_BOUNDING_SPHERE_H
#define META_OCEAN_MATH_BOUNDING_SPHERE_H

#include "ocean/math/Math.h"
#include "ocean/math/BoundingBox.h"
#include "ocean/math/Sphere3.h"

namespace Ocean
{

/**
 * This class implements a bounding sphere.
 * @ingroup math
 */
class OCEAN_MATH_EXPORT BoundingSphere : public SphereT3<Scalar>
{
	public:

		/**
		 * Creates an invalid bounding sphere.
		 */
		inline BoundingSphere();

		/**
		 * Creates a new bounding sphere by a center point and a radius.
		 * @param center Center of the bounding sphere
		 * @param radius Radius of the bounding sphere
		 */
		inline BoundingSphere(const Vector3& center, const Scalar radius);

		/**
		 * Creates a new bounding sphere by a given bounding box.
		 * @param boundingBox Bounding box to be converted
		 */
		inline explicit BoundingSphere(const BoundingBox& boundingBox);

		/**
		 * Returns both intersection points between a given ray and this sphere.
		 * The intersection points are determined without consideration of the ray direction<br>.
		 * @param ray Ray to determine the nearest intersection point for
		 * @param position0 Resulting nearest intersection position
		 * @param distance0 Resulting nearest intersection distance, regarding to the length of the ray direction
		 * @param position1 Resulting farthest intersection position
		 * @param distance1 Resulting farthest intersection distance, regarding to the length of the ray direction
		 * @return True, if so
		 */
		bool intersections(const Line3& ray, Vector3& position0, Scalar& distance0, Vector3& position1, Scalar& distance1) const;

		/**
		 * Returns both intersection points between a given ray and this sphere.
		 * The intersection points are determined without consideration of the ray direction<br>.
		 * @param ray Ray to determine the nearest intersection point for
		 * @param transformation Transformation of the given world transformation (for the sphere)
		 * @param invertedTransformation Inverted transformation of the given world transformation (for the sphere)
		 * @param position0 Resulting nearest intersection position
		 * @param distance0 Resulting nearest intersection distance, regarding to the length of the ray direction
		 * @param position1 Resulting farthest intersection position
		 * @param distance1 Resulting farthest intersection distance, regarding to the length of the ray direction
		 * @return True, if so
		 */
		inline bool intersections(const Line3& ray, const HomogenousMatrix4& transformation, const HomogenousMatrix4& invertedTransformation, Vector3& position0, Scalar& distance0, Vector3& position1, Scalar& distance1) const;

		/**
		 * Returns both intersection points between a given ray and this sphere.
		 * The intersection points are determined without consideration of the ray direction<br>.
		 * @param ray Ray to determine the nearest intersection point for
		 * @param position0 Resulting nearest intersection position
		 * @param distance0 Resulting nearest intersection distance, regarding to the length of the ray direction
		 * @param normal0 Resulting nearest intersection normal
		 * @param position1 Resulting farthest intersection position
		 * @param distance1 Resulting farthest intersection distance, regarding to the length of the ray direction
		 * @param normal1 Resulting farthest intersection normal
		 * @return True, if so
		 */
		inline bool intersections(const Line3& ray, Vector3& position0, Scalar& distance0, Vector3& normal0, Vector3& position1, Scalar& distance1, Vector3& normal1) const;

		/**
		 * Returns both intersection points between a given ray and this sphere.
		 * The intersection points are determined without consideration of the ray direction<br>.
		 * @param ray Ray to determine the nearest intersection point for
		 * @param transformation Transformation of the given world transformation (for the sphere)
		 * @param invertedTransformation Inverted transformation of the given world transformation (for the sphere)
		 * @param position0 Resulting nearest intersection position
		 * @param distance0 Resulting nearest intersection distance, regarding to the length of the ray direction
		 * @param normal0 Resulting nearest intersection normal
		 * @param position1 Resulting farthest intersection position
		 * @param distance1 Resulting farthest intersection distance, regarding to the length of the ray direction
		 * @param normal1 Resulting farthest intersection normal
		 * @return True, if so
		 */
		inline bool intersections(const Line3& ray, const HomogenousMatrix4& transformation, const HomogenousMatrix4& invertedTransformation, Vector3& position0, Scalar& distance0, Vector3& normal0, Vector3& position1, Scalar& distance1, Vector3& normal1) const;

		/**
		 * Returns the front intersection point between a given ray and this sphere whenever the distance is positive.
		 * The dot product between the ray direction and the intersection normal will be negative.<br>
		 * @param ray Ray to determine the intersection point for
		 * @param position Resulting intersection position
		 * @param distance Resulting intersection distance
		 * @return True, if so
		 */
		bool positiveFrontIntersection(const Line3& ray, Vector3& position, Scalar& distance) const;

		/**
		 * Returns the front intersection point between a given ray and this sphere whenever the distance is positive.
		 * The dot product between the ray direction and the intersection normal will be negative.<br>
		 * @param ray Ray to determine the intersection point for
		 * @param position Resulting intersection position
		 * @param distance Resulting intersection distance
		 * @param normal Resulting intersection normal
		 * @return True, if so
		 */
		inline bool positiveFrontIntersection(const Line3& ray, Vector3& position, Scalar& distance, Vector3& normal) const;

		/**
		 * Returns the front intersection point between a given ray and this sphere whenever the distance is positive.
		 * The dot product between the ray direction and the intersection normal will be negative.<br>
		 * @param ray Ray to determine the intersection point for
		 * @param transformation Transformation of the given world transformation (for the sphere)
		 * @param invertedTransformation Inverted transformation of the given world transformation (for the sphere)
		 * @param position Resulting intersection position
		 * @param distance Resulting intersection distance
		 * @return True, if so
		 */
		inline bool positiveFrontIntersection(const Line3& ray, const HomogenousMatrix4& transformation, const HomogenousMatrix4& invertedTransformation, Vector3& position, Scalar& distance) const;

		/**
		 * Returns the front intersection point between a given ray and this sphere whenever the distance is positive.
		 * The dot product between the ray direction and the intersection normal will be negative.<br>
		 * @param ray Ray to determine the intersection point for
		 * @param transformation Transformation of the given world transformation (for the sphere)
		 * @param invertedTransformation Inverted transformation of the given world transformation (for the sphere)
		 * @param position Resulting intersection position
		 * @param distance Resulting intersection distance
		 * @param normal Resulting intersection normal
		 * @return True, if so
		 */
		inline bool positiveFrontIntersection(const Line3& ray, const HomogenousMatrix4& transformation, const HomogenousMatrix4& invertedTransformation, Vector3& position, Scalar& distance, Vector3& normal) const;

		/**
		 * Returns the back intersection point between a given ray and this sphere whenever the distance is positive.
		 * The dot product between the ray direction and the intersection normal will be positive.<br>
		 * @param ray Ray to determine the intersection point for
		 * @param position Resulting intersection position
		 * @param distance Resulting intersection distance
		 * @return True, if so
		 */
		bool positiveBackIntersection(const Line3& ray, Vector3& position, Scalar& distance) const;

		/**
		 * Returns the back intersection point between a given ray and this sphere whenever the distance is positive.
		 * The dot product between the ray direction and the intersection normal will be positive.<br>
		 * @param ray Ray to determine the intersection point for
		 * @param position Resulting intersection position
		 * @param distance Resulting intersection distance
		 * @param normal Resulting intersection normal
		 * @return True, if so
		 */
		inline bool positiveBackIntersection(const Line3& ray, Vector3& position, Scalar& distance, Vector3& normal) const;

		/**
		 * Returns the back intersection point between a given ray and this sphere whenever the distance is positive.
		 * The dot product between the ray direction and the intersection normal will be positive.<br>
		 * @param ray Ray to determine the intersection point for
		 * @param transformation Transformation of the given world transformation (for the sphere)
		 * @param invertedTransformation Inverted transformation of the given world transformation (for the sphere)
		 * @param position Resulting intersection position
		 * @param distance Resulting intersection distance
		 * @return True, if so
		 */
		inline bool positiveBackIntersection(const Line3& ray, const HomogenousMatrix4& transformation, const HomogenousMatrix4& invertedTransformation, Vector3& position, Scalar& distance) const;

		/**
		 * Returns the back intersection point between a given ray and this sphere whenever the distance is positive.
		 * The dot product between the ray direction and the intersection normal will be positive.<br>
		 * @param ray Ray to determine the intersection point for
		 * @param transformation Transformation of the given world transformation (for the sphere)
		 * @param invertedTransformation Inverted transformation of the given world transformation (for the sphere)
		 * @param position Resulting intersection position
		 * @param distance Resulting intersection distance
		 * @param normal Resulting intersection normal
		 * @return True, if so
		 */
		inline bool positiveBackIntersection(const Line3& ray, const HomogenousMatrix4& transformation, const HomogenousMatrix4& invertedTransformation, Vector3& position, Scalar& distance, Vector3& normal) const;

	private:

		/// Inverse of the sphere radius.
		Scalar inverseRadius_;
};

inline BoundingSphere::BoundingSphere() :
	Sphere3(),
	inverseRadius_(0)
{
	// nothing to do here
}

inline BoundingSphere::BoundingSphere(const Vector3& center, const Scalar radius) :
	Sphere3(center, radius),
	inverseRadius_((radius > 0) ? (Scalar(1) / radius) : Scalar(0))
{
	// nothing to do here
}

inline BoundingSphere::BoundingSphere(const BoundingBox& boundingBox) :
	Sphere3(boundingBox)
{
	inverseRadius_ = (radius_ > 0) ? (Scalar(1) / radius_) : Scalar(0);
}

inline bool BoundingSphere::intersections(const Line3& ray, const HomogenousMatrix4& transformation, const HomogenousMatrix4& invertedTransformation, Vector3& position0, Scalar& distance0, Vector3& position1, Scalar& distance1) const
{
	ocean_assert(isValid());
	ocean_assert(transformation.isValid());
	ocean_assert(invertedTransformation.isValid());
	ocean_assert(ray.isValid());

	if (intersections(Line3(invertedTransformation * ray.point(), invertedTransformation.rotationMatrix(ray.direction())), position0, distance0, position1, distance1))
	{
		position0 = transformation * position0;
		position1 = transformation * position1;
		return true;
	}

	return false;
}

inline bool BoundingSphere::intersections(const Line3& ray, Vector3& position0, Scalar& distance0, Vector3& normal0, Vector3& position1, Scalar& distance1, Vector3& normal1) const
{
	ocean_assert(isValid());
	ocean_assert(ray.isValid());

	if (intersections(ray, position0, distance0, position1, distance1))
	{
		normal0 = (position0 - center_) * inverseRadius_;
		normal1 = (position1 - center_) * inverseRadius_;

		return true;
	}

	return false;
}

inline bool BoundingSphere::intersections(const Line3& ray, const HomogenousMatrix4& transformation, const HomogenousMatrix4& invertedTransformation, Vector3& position0, Scalar& distance0, Vector3& normal0, Vector3& position1, Scalar& distance1, Vector3& normal1) const
{
	ocean_assert(isValid());
	ocean_assert(transformation.isValid());
	ocean_assert(invertedTransformation.isValid());
	ocean_assert(ray.isValid());

	if (intersections(Line3(invertedTransformation * ray.point(), invertedTransformation.rotationMatrix(ray.direction())), position0, distance0, normal0, position1, distance1, normal1))
	{
		position0 = transformation * position0;
		normal0 = invertedTransformation.transposedRotationMatrix(normal0).normalizedOrZero();

		position1 = transformation * position1;
		normal1 = invertedTransformation.transposedRotationMatrix(normal1).normalizedOrZero();
		return true;
	}

	return false;
}

inline bool BoundingSphere::positiveFrontIntersection(const Line3& ray, Vector3& position, Scalar& distance, Vector3& normal) const
{
	ocean_assert(isValid());
	ocean_assert(ray.isValid());

	if (positiveFrontIntersection(ray, position, distance))
	{
		normal = (position - center_) * inverseRadius_;
		ocean_assert(ray.direction() * normal <= 0);

		return true;
	}

	return false;
}

inline bool BoundingSphere::positiveFrontIntersection(const Line3& ray, const HomogenousMatrix4& transformation, const HomogenousMatrix4& invertedTransformation, Vector3& position, Scalar& distance) const
{
	ocean_assert(isValid());
	ocean_assert(transformation.isValid());
	ocean_assert(invertedTransformation.isValid());
	ocean_assert(ray.isValid());

	if (positiveFrontIntersection(Line3(invertedTransformation * ray.point(), invertedTransformation.rotationMatrix(ray.direction())), position, distance))
	{
		position = transformation * position;
		return true;
	}

	return false;
}

inline bool BoundingSphere::positiveFrontIntersection(const Line3& ray, const HomogenousMatrix4& transformation, const HomogenousMatrix4& invertedTransformation, Vector3& position, Scalar& distance, Vector3& normal) const
{
	ocean_assert(isValid());
	ocean_assert(transformation.isValid());
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

inline bool BoundingSphere::positiveBackIntersection(const Line3& ray, Vector3& position, Scalar& distance, Vector3& normal) const
{
	ocean_assert(isValid());
	ocean_assert(ray.isValid());

	if (positiveBackIntersection(ray, position, distance))
	{
		normal = (position - center_) * inverseRadius_;
		ocean_assert(ray.direction() * normal >= 0);

		return true;
	}

	return false;
}

inline bool BoundingSphere::positiveBackIntersection(const Line3& ray, const HomogenousMatrix4& transformation, const HomogenousMatrix4& invertedTransformation, Vector3& position, Scalar& distance) const
{
	ocean_assert(isValid());
	ocean_assert(transformation.isValid());
	ocean_assert(invertedTransformation.isValid());
	ocean_assert(ray.isValid());

	if (positiveBackIntersection(Line3(invertedTransformation * ray.point(), invertedTransformation.rotationMatrix(ray.direction())), position, distance))
	{
		position = transformation * position;
		return true;
	}

	return false;
}

inline bool BoundingSphere::positiveBackIntersection(const Line3& ray, const HomogenousMatrix4& transformation, const HomogenousMatrix4& invertedTransformation, Vector3& position, Scalar& distance, Vector3& normal) const
{
	ocean_assert(isValid());
	ocean_assert(transformation.isValid());
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

}

#endif // META_OCEAN_MATH_BOUNDING_SPHERE_H
