/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_SPHERE_3_H
#define META_OCEAN_MATH_SPHERE_3_H

#include "ocean/math/Math.h"
#include "ocean/math/Box3.h"
#include "ocean/math/Line3.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

// Forward declaration.
template <typename T>
class SphereT3;

/**
 * Definition of the Sphere3 object, depending on the OCEAN_MATH_USE_SINGLE_PRECISION either with single or double precision float data type.
 * @see SphereT3
 * @ingroup math
 */
typedef SphereT3<Scalar> Sphere3;

/**
 * Definition of a 3D sphere with double values.
 * @see SphereT3
 * @ingroup math
 */
typedef SphereT3<double> SphereD3;

/**
 * Definition of a 3D sphere with float values.
 * @see SphereT3
 * @ingroup math
 */
typedef SphereT3<float> SphereF3;

/**
 * Definition of a vector holding Shpere3 objects.
 * @see Sphere3
 * @ingroup math
 */
typedef std::vector<Sphere3> Spheres3;

/**
 * This class implements a 3D sphere.
 * @ingroup math
 */
template <typename T>
class SphereT3
{
	public:

		/**
		 * Creates an invalid sphere.
		 */
		SphereT3() = default;

		/**
		 * Creates a new sphere by a center point and a radius.
		 * @param center The center of the sphere
		 * @param radius The radius of the sphere, with range [0, infinity), negative to create an invalid sphere
		 */
		inline SphereT3(const VectorT3<T>& center, const T radius);

		/**
		 * Creates a new sphere entirely containing a given 3D box.
		 * @param boundingBox The box which will be contained by the new sphere, must be valid
		 */
		explicit inline SphereT3(const BoxT3<T>& boundingBox);

		/**
		 * Returns the center of the sphere.
		 * @return Sphere center
		 */
		inline const VectorT3<T>& center() const;

		/**
		 * Returns the radius of the sphere.
		 * @return Sphere radius, with range [0, infinity), negative if invalid
		 */
		inline const T& radius() const;

		/**
		 * Returns whether a given point is inside this sphere.
		 * @param point The point to check
		 * @return True, if so
		 */
		inline bool isInside(const VectorT3<T>& point) const;

		/**
		 * Returns whether a given point is inside this sphere including a thin epsilon boundary.
		 * @param point The point to check
		 * @param eps Epsilon to be used, with range [0, infinity)
		 * @return True, if so
		 */
		inline bool isInsideEps(const VectorT3<T>& point, const T eps = NumericT<T>::eps()) const;

		/**
		 * Returns whether a given ray has an intersection with this sphere.
		 * @param ray The ray to be tested
		 * @return True, if so
		 */
		bool hasIntersection(const LineT3<T>& ray) const;

		/**
		 * Returns whether a given ray has an intersection with this sphere.
		 * @param ray Ray to be tested, must be valid
		 * @param sphere_T_ray The transformation between ray and this sphere, must be valid
		 * @return True, if so
		 */
		inline bool hasIntersection(const LineT3<T>& ray, const HomogenousMatrixT4<T>& sphere_T_ray) const;

		/**
		 * Returns whether two spheres have an intersection.
		 * @param sphere Second sphere to test
		 * @return True, if so
		 */
		inline bool hasIntersection(const SphereT3<T>& sphere) const;

		/**
		 * Returns whether this radius of this sphere is not negative and thus the sphere is valid.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Returns a new sphere with an enlarged radius (the center of the sphere stays constant).
		 * @param factor The factor to be multiplied with the radius of this sphere, with range [0, infinity)
		 * @return New resulting sphere
		 */
		inline SphereT3<T> operator*(const T factor) const;

		/**
		 * Multiplies the radius of this sphere with a given factor.
		 * @param factor The factor to be multiplied with the radius, with range [0, infinity)
		 * @return Reference to this sphere object
		 */
		inline SphereT3<T>& operator*=(const T factor);

		/**
		 * Converts a 2D location coordinate on the surface of a unit sphere to a vector with unit length.
		 * The north/south axis of the sphere is parallel to the y-axis, the z-axis points towards the longitude 0 at the equator, the x-axis points towards the longitude PI/2 at the equator.
		 * @param latitude The latitude coordinate of the location, in radian, with range [-PI/2, PI/2]
		 * @param longitude The longitude coordinate of the location, in radian, with range [-PI, PI]
		 * @return The corresponding 3D vector with unit length
		 */
		static VectorT3<T> coordinateToVector(const T latitude, const T longitude);

		/**
		 * Converts a vector pointing to the surface of a unit sphere to a 2D location coordinate.
		 * @param coordinateVector The vector pointing to the location on the unit sphere, must have unit length
		 * @param latitude The resulting latitude coordinate of the location, in radian, with range range [-PI/2, PI/2]
		 * @param longitude The resulting longitude coordinate of the location, in radian, with range range [-PI, PI]
		 */
		static void vectorToCoordinate(const VectorT3<T>& coordinateVector, T& latitude, T& longitude);

		/**
		 * Calculates the shortest distance between two 2D location coordinates on the surface of a unit sphere.
		 * The shortest distance is identical to the shortest angle (in radian) in a unit sphere.<br>
		 * This function applies the Haversine formula.
		 * @param latitudeA The latitude coordinate of the first location, in radian, with range [-PI/2, PI/2]
		 * @param longitudeA The longitude coordinate of the first location, in radian, with range [-PI, PI]
		 * @param latitudeB  The latitude coordinate of the second location, in radian, with range [-PI/2, PI/2]
		 * @param longitudeB The longitude coordinate of the second location, in radian, with range [-PI, PI]
		 * @return The shortest distance on the unit sphere, with range [0, PI]
		 */
		static T shortestDistance(const T latitudeA, const T longitudeA, const T latitudeB, const T longitudeB);

	protected:

		/// Sphere center.
		VectorT3<T> center_ = VectorT3<T>(0, 0, 0);

		/// Sphere radius.
		T radius_ = T(-1);
};

template <typename T>
inline SphereT3<T>::SphereT3(const VectorT3<T>& center, const T radius) :
	center_(center),
	radius_(radius)
{
	// nothing to do here
}

template <typename T>
inline SphereT3<T>::SphereT3(const BoxT3<T>& boundingBox) :
	center_(boundingBox.center()),
	radius_(boundingBox.diagonal() * T(0.5))
{
	ocean_assert(boundingBox.isValid());
	ocean_assert(isValid());
}

template <typename T>
inline const VectorT3<T>& SphereT3<T>::center() const
{
	return center_;
}

template <typename T>
inline const T& SphereT3<T>::radius() const
{
	return radius_;
}

template <typename T>
inline bool SphereT3<T>::isInside(const VectorT3<T>& point) const
{
	ocean_assert(isValid());

	// a point is inside this sphere if:
	// |center - point| <= radius

	return center_.sqrDistance(point) <= NumericT<T>::sqr(radius_);
}

template <typename T>
bool SphereT3<T>::isInsideEps(const VectorT3<T>& point, const T eps) const
{
	ocean_assert(isValid());
	ocean_assert(eps >= T(0));

	// a point is inside this sphere if:
	// |center - point| <= radius + eps

	return center_.sqrDistance(point) <= NumericT<T>::sqr(radius_ + eps);
}

template <typename T>
bool SphereT3<T>::hasIntersection(const LineT3<T>& ray) const
{
	ocean_assert(isValid());
	ocean_assert(ray.isValid());

	// (d * d) * t^2 + (2 * d * (p - o)) * t + (p - o)*(p - o) - r^2 == 0
	//       a * t^2 +                 b * t +                     c == 0

	const VectorT3<T>& d = ray.direction();
	const VectorT3<T>& p = ray.point();
	const VectorT3<T>& o = center_;

	const VectorT3<T> po(p - o);

	const T a = d * d;
	const T b = (d * po) * 2;
	const T c = po * po - radius_ * radius_;

	return b * b >= 4 * a * c;
}

template <typename T>
inline bool SphereT3<T>::hasIntersection(const LineT3<T>& ray, const HomogenousMatrixT4<T>& sphere_T_ray) const
{
	ocean_assert(sphere_T_ray.isValid());
	ocean_assert(ray.isValid());

	return hasIntersection(LineT3<T>(sphere_T_ray * ray.point(), sphere_T_ray.rotationMatrix(ray.direction())));
}

template <typename T>
inline bool SphereT3<T>::hasIntersection(const SphereT3<T>& sphere) const
{
	ocean_assert(isValid());
	ocean_assert(sphere.isValid());

	// two spheres have an intersection if:
	// |center0 - center1| <= radius0 + radius1

	return center_.sqrDistance(sphere.center_) <= NumericT<T>::sqr(radius_ + sphere.radius_);
}

template <typename T>
inline bool SphereT3<T>::isValid() const
{
	return radius_ >= T(0);
}

template <typename T>
inline SphereT3<T> SphereT3<T>::operator*(const T factor) const
{
	ocean_assert(isValid());
	ocean_assert(factor >= 0);

	return SphereT3<T>(center_, radius_ * factor);
}

template <typename T>
inline SphereT3<T>& SphereT3<T>::operator*=(const T factor)
{
	ocean_assert(isValid());
	ocean_assert(factor >= T(0));

	radius_ *= factor;
	return *this;
}

template <typename T>
VectorT3<T> SphereT3<T>::coordinateToVector(const T latitude, const T longitude)
{
	ocean_assert(NumericT<T>::isInsideRange(-NumericT<T>::pi_2(), latitude, NumericT<T>::pi_2()));
	ocean_assert(NumericT<T>::isInsideRange(-NumericT<T>::pi(), longitude, NumericT<T>::pi()));

	const T d = NumericT<T>::cos(latitude);

	const VectorT3<T> coordinateVector(NumericT<T>::sin(longitude) * d, NumericT<T>::sin(latitude), NumericT<T>::cos(longitude) * d);
	ocean_assert(coordinateVector.isUnit());

	return coordinateVector;
}

template <typename T>
void SphereT3<T>::vectorToCoordinate(const VectorT3<T>& coordinateVector, T& latitude, T& longitude)
{
	ocean_assert(coordinateVector.isUnit());

	latitude = NumericT<T>::asin(coordinateVector.y());

	if (NumericT<T>::isEqualEps(coordinateVector.x()) && NumericT<T>::isEqualEps(coordinateVector.z()))
	{
		longitude = 0; // we are at one of both poles, any value is correct
	}
	else
	{
		longitude = NumericT<T>::atan2(coordinateVector.x(), coordinateVector.z());
	}
}

template <typename T>
T SphereT3<T>::shortestDistance(const T latitudeA, const T longitudeA, const T latitudeB, const T longitudeB)
{
	ocean_assert(NumericT<T>::isInsideRange(-NumericT<T>::pi_2(), latitudeA, NumericT<T>::pi_2()) && NumericT<T>::isInsideRange(-NumericT<T>::pi_2(), latitudeB, NumericT<T>::pi_2()));
	ocean_assert(NumericT<T>::isInsideRange(-NumericT<T>::pi(), longitudeA, NumericT<T>::pi()) && NumericT<T>::isInsideRange(-NumericT<T>::pi(), longitudeB, NumericT<T>::pi()));

	// great-circle distance between two points (shortest distance between two points on a sphere) using Haversine formula

	const T latitudeDelta_2 = (latitudeB - latitudeA) * T(0.5);
	const T longitudeDelta_2 = (longitudeB - longitudeA) * T(0.5);

	const T inner = NumericT<T>::sqr(NumericT<T>::sin(latitudeDelta_2)) + NumericT<T>::cos(latitudeA) * NumericT<T>::cos(latitudeB) * NumericT<T>::sqr(NumericT<T>::sin(longitudeDelta_2));
	ocean_assert(inner >= T(0));

	return T(2) * NumericT<T>::asin(NumericT<T>::sqrt(inner));
}

}

#endif // META_OCEAN_MATH_SPHERE_3_H
