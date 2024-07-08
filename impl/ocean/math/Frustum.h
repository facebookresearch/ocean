/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_FRUSTUM_H
#define META_OCEAN_MATH_FRUSTUM_H

#include "ocean/math/Math.h"
#include "ocean/math/AnyCamera.h"
#include "ocean/math/Box3.h"
#include "ocean/math/Numeric.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Plane3.h"
#include "ocean/math/Sphere3.h"

namespace Ocean
{

// Forward declaration.
template <typename T> class FrustumT;

/**
 * Definition of a frustum.
 * @see FrustumT
 * @ingroup math
 */
typedef FrustumT<Scalar> Frustum;

/**
 * Definition of a frustum with double values.
 * @see FrustumT
 * @ingroup math
 */
typedef FrustumT<double> FrustumD;

/**
 * Definition of a frustum vector with float values.
 * @see FrustumT
 * @ingroup math
 */
typedef FrustumT<float> FrustumF;

/**
 * Definition of a typename alias for vectors with FrustumT objects.
 * @see FrustumT
 * @ingroup math
 */
template <typename T>
using FrustumsT = std::vector<FrustumT<T>>;

/**
 * Definition of a vector holding Frustum objects.
 * @see Frustum
 * @ingroup math
 */
typedef std::vector<Frustum> Frustums;

/**
 * Definition of a vector holding FrustumD objects.
 * @see FrustumD
 * @ingroup math
 */
typedef std::vector<FrustumD> FrustumsD;

/**
 * Definition of a vector holding FrustumF objects.
 * @see FrustumF
 * @ingroup math
 */
typedef std::vector<FrustumF> FrustumsF;

/**
 * This class implements a viewing frustum.
 * The viewing frustum points towards the negative z-space with x-axis pointing towards the right plane of the frustum, and y-axis pointing towards the top plane of the frustum.
 * @tparam T Data type of the frustum elements.
 * @see Frustum.
 * @ingroup math
 */
template <typename T>
class FrustumT
{
	public:

		/**
		 * Definition of ids for the individual planes of the frustum.
		 */
		enum PlaneIds : unsigned int
		{
			/// The front plane, with normal pointing into the frustum.
			PI_FRONT = 0u,
			/// The back plane, with normal pointing into the frustum.
			PI_BACK = 1u,
			/// The left plane, with normal pointing into the frustum.
			PI_LEFT = 2u,
			/// The right plane, with normal pointing into the frustum.
			PI_RIGHT = 3u,
			/// The top plane, with normal pointing into the frustum.
			PI_TOP = 4u,
			/// The bottom plane, with normal pointing into the frustum.
			PI_BOTTOM = 5u,
			/// The number of planes.
			PI_END = 6u
		};

	public:

		/**
		 * Definition of the used data type.
		 */
		typedef T Type;

	public:

		/**
		 * Default constructor creating an invalid frustum.
		 */
		FrustumT() = default;

		/**
		 * Creates a new viewing frustum, pointing towards the negative z-space with y-axis up.
		 * @param width The width of the camera image, in pixel, with range (0, infinity)
		 * @param height The height of the camera image, in pixel, with range (0, infinity)
		 * @param focalLengthX The focal length of the camera in horizontal direction, in pixel domain, with range (0, infinity)
		 * @param focalLengthY The focal length of the camera in vertical direction, in pixel domain, with range (0, infinity)
		 * @param principalPointX The horizontal location of the camera's principal point, in pixel, with range (0, width)
		 * @param principalPointY The vertical location of the camera's principal point, in pixel, with range (0, height)
		 * @param nearDistance The distance to the frustum's front plane, with range [0, infinity)
		 * @param farDistance The distance to the frustum's back plane, with range (nearDistance, infinity)
		 */
		FrustumT(const T width, const T height, const T focalLengthX, const T focalLengthY, const T principalPointX, const T principalPointY, const T nearDistance, const T farDistance);

		/**
		 * Creates a new viewing frustum from a camera, pointing towards the negative z-space with y-axis up.
		 * @param pinholeCamera The pinhole camera profile for which the frustum will be created, must be valid
		 * @param nearDistance The distance to the frustum's front plane, with range [0, infinity)
		 * @param farDistance The distance to the frustum's back plane, with range (nearDistance, infinity)
		 */
		inline FrustumT(const PinholeCamera& pinholeCamera, const T nearDistance, const T farDistance);

		/**
		 * Creates a new viewing frustum from a camera, pointing towards the negative z-space with y-axis up.
		 * @param pinholeCamera The pinhole camera profile for which the frustum will be created, must be a pinhole camera, must be valid
		 * @param nearDistance The distance to the frustum's front plane, with range [0, infinity)
		 * @param farDistance The distance to the frustum's back plane, with range (nearDistance, infinity)
		 */
		inline FrustumT(const AnyCameraT<T>& pinholeCamera, const T nearDistance, const T farDistance);

		/**
		 * Creates a new viewing frustum located anywhere in space.
		 * @param world_T_camera The transformation transforming the camera (pointing towards negative z-space with y-axis up) to world, must be valid
		 * @param width The width of the camera image, in pixel, with range (0, infinity)
		 * @param height The height of the camera image, in pixel, with range (0, infinity)
		 * @param focalLengthX The focal length of the camera in horizontal direction, in pixel domain, with range (0, infinity)
		 * @param focalLengthY The focal length of the camera in vertical direction, in pixel domain, with range (0, infinity)
		 * @param principalPointX The horizontal location of the camera's principal point, in pixel, with range (0, width)
		 * @param principalPointY The vertical location of the camera's principal point, in pixel, with range (0, height)
		 * @param nearDistance The distance to the frustum's front plane, with range [0, infinity)
		 * @param farDistance The distance to the frustum's back plane, with range (nearDistance, infinity)
		 */
		FrustumT(const HomogenousMatrixT4<T>& world_T_camera, const T width, const T height, const T focalLengthX, const T focalLengthY, const T principalPointX, const T principalPointY, const T nearDistance, const T farDistance);

		/**
		 * Creates a new viewing frustum from a camera located anywhere in space.
		 * @param world_T_camera The transformation transforming the camera (pointing towards negative z-space with y-axis up) to world, must be valid
		 * @param pinholeCamera The pinhole camera profile for which the frustum will be created, must be valid
		 * @param nearDistance The distance to the frustum's front plane, with range [0, infinity)
		 * @param farDistance The distance to the frustum's back plane, with range (nearDistance, infinity)
		 */
		inline FrustumT(const HomogenousMatrixT4<T>& world_T_camera, const PinholeCamera& pinholeCamera, const T nearDistance, const T farDistance);

		/**
		 * Creates a new viewing frustum from a camera located anywhere in space.
		 * @param world_T_camera The transformation transforming the camera (pointing towards negative z-space with y-axis up) to world, must be valid
		 * @param pinholeCamera The pinhole camera profile for which the frustum will be created, must be a pinhole camera, must be valid
		 * @param nearDistance The distance to the frustum's front plane, with range [0, infinity)
		 * @param farDistance The distance to the frustum's back plane, with range (nearDistance, infinity)
		 */
		inline FrustumT(const HomogenousMatrixT4<T>& world_T_camera, const AnyCamera& pinholeCamera, const T nearDistance, const T farDistance);

		/**
		 * Returns the six planes of the frustum, with order as defined in PlaneIds.
		 * @return The frustum's planes
		 */
		inline const PlaneT3<T>* planes() const;

		/**
		 * Returns whether a 3D object point is inside this frustum.
		 * @param objectPoint The 3D object point to check
		 * @return True, if so
		 */
		bool isInside(const VectorT3<T>& objectPoint) const;

		/**
		 * Returns whether a 3D sphere is located entirely inside this frustum.
		 * @param sphere The sphere to check, must be valid
		 * @return True, if so
		 */
		bool isInside(const SphereT3<T>& sphere) const;

		/**
		 * Returns whether a 3D box is located entirely inside this frustum.
		 * @param box The box to check, must be valid
		 * @return True, if so
		 */
		bool isInside(const Box3& box) const;

		/**
		 * Returns whether a 3D object is located entirely inside this frustum.
		 * @param vertices The 3D vertices of the object to check, must be valid
		 * @param size The number of vertices, with range [1, infinity)
		 * @return True, if so
		 */
		bool isInside(const Vector3* vertices, const size_t size) const;

		/**
		 * Returns whether a 3D sphere has an interaction with this frustum or is entirely inside this frustum.
		 * @param sphere The sphere to check, must be valid
		 * @return True, if so
		 */
		bool hasIntersection(const SphereT3<T>& sphere) const;

		/**
		 * Returns whether a 3D box has an interaction with this frustum or is entirely inside this frustum.
		 * @param box The box to check, must be valid
		 * @return True, if so
		 */
		bool hasIntersection(const Box3& box) const;

		/**
		 * Returns whether a 3D object has an interaction with this frustum or is entirely inside this frustum.
		 * @param vertices The 3D vertices of the object to check, must be valid
		 * @param size The number of vertices, with range [1, infinity)
		 * @return True, if so
		 */
		bool hasIntersection(const Vector3* vertices, const size_t size) const;

		/**
		 * Returns whether this frustum object is valid.
		 */
		inline bool isValid() const;

		/**
		 * Returns whether two frustum objects are equal up to a small epsilon.
		 * @param frustum The second frustum to check
		 * @param eps The epsilon to be used, with range [0, infinity)
		 * @return True, if so
		 */
		bool isEqual(const FrustumT<T>& frustum, const T eps = NumericT<T>::eps()) const;

		/**
		 * Returns whether two frustum objects are identical.
		 * @param frustum The second frustum to check
		 * @return True, if so
		 */
		inline bool operator==(const FrustumT<T>& frustum) const;

		/**
		 * Returns whether two frustum objects are not identical.
		 * @param frustum The second frustum to check
		 * @return True, if so
		 */
		inline bool operator!=(const FrustumT<T>& frustum) const;

	protected:

		/// The six planes of the frustum.
		PlaneT3<T> planes_[PI_END];
};

template <typename T>
FrustumT<T>::FrustumT(const T width, const T height, const T focalLengthX, const T focalLengthY, const T principalPointX, const T principalPointY, const T nearDistance, const T farDistance)
{
	ocean_assert(width > NumericT<T>::eps() && height > NumericT<T>::eps());

	ocean_assert(focalLengthX > NumericT<T>::eps() && focalLengthY > NumericT<T>::eps());

	ocean_assert(principalPointX > 0 && principalPointX < width);
	ocean_assert(principalPointY > 0 && principalPointY < height);

	ocean_assert(nearDistance >= 0 && nearDistance < farDistance);

	planes_[PI_FRONT] = PlaneT3<T>(VectorT3<T>(0, 0, -1), nearDistance);
	planes_[PI_BACK] = PlaneT3<T>(VectorT3<T>(0, 0, 1), -farDistance);

	const T& leftPx = principalPointX;
	const T rightPx = width - principalPointX;

	planes_[PI_LEFT] = PlaneT3<T>(VectorT3<T>(focalLengthX, 0, -leftPx).normalized(), 0);
	planes_[PI_RIGHT] = PlaneT3<T>(VectorT3<T>(-focalLengthX, 0, -rightPx).normalized(), 0);

	const T& topPy = principalPointY;
	const T bottomPy = height - principalPointY;

	planes_[PI_TOP] = PlaneT3<T>(VectorT3<T>(0, -focalLengthY, -topPy).normalized(), 0);
	planes_[PI_BOTTOM] = PlaneT3<T>(VectorT3<T>(0, focalLengthY, -bottomPy).normalized(), 0);

	ocean_assert(isValid());
}

template <typename T>
inline FrustumT<T>::FrustumT(const PinholeCamera& pinholeCamera, const T nearDistance, const T farDistance) :
	FrustumT<T>(T(pinholeCamera.width()), T(pinholeCamera.height()), T(pinholeCamera.focalLengthX()), T(pinholeCamera.focalLengthY()), T(pinholeCamera.principalPointX()), T(pinholeCamera.principalPointY()), nearDistance, farDistance)
{
	ocean_assert(pinholeCamera.isValid());
}

template <typename T>
inline FrustumT<T>::FrustumT(const AnyCameraT<T>& pinholeCamera, const T nearDistance, const T farDistance) :
	FrustumT<T>(T(pinholeCamera.width()), T(pinholeCamera.height()), T(pinholeCamera.focalLengthX()), T(pinholeCamera.focalLengthY()), T(pinholeCamera.principalPointX()), T(pinholeCamera.principalPointY()), nearDistance, farDistance)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(pinholeCamera.anyCameraType() == AnyCameraType::PINHOLE);
}

template <typename T>
FrustumT<T>::FrustumT(const HomogenousMatrixT4<T>& world_T_camera, const T width, const T height, const T focalLengthX, const T focalLengthY, const T principalPointX, const T principalPointY, const T nearDistance, const T farDistance)
{
	ocean_assert(world_T_camera.isValid() && world_T_camera.rotationMatrix().isOrthonormal(NumericT<T>::weakEps()));

	ocean_assert(width > NumericT<T>::eps() && height > NumericT<T>::eps());

	ocean_assert(focalLengthX > NumericT<T>::eps() && focalLengthY > NumericT<T>::eps());

	ocean_assert(principalPointX > 0 && principalPointX < width);
	ocean_assert(principalPointY > 0 && principalPointY < height);

	ocean_assert(nearDistance >= 0 && nearDistance < farDistance);

	const VectorT3<T> translation = world_T_camera.translation();

	const VectorT3<T> zAxisNormalized = world_T_camera.zAxis().normalizedOrZero();

	planes_[PI_FRONT] = PlaneT3<T>(world_T_camera * VectorT3<T>(0, 0, -nearDistance), -zAxisNormalized);
	planes_[PI_BACK] = PlaneT3<T>(world_T_camera * VectorT3<T>(0, 0, -farDistance), zAxisNormalized);

	const T& leftPx = principalPointX;
	const T rightPx = width - principalPointX;

	const VectorT3<T> leftNormal = (world_T_camera * VectorT3<T>(focalLengthX, 0, -leftPx) - translation).normalizedOrZero();
	const VectorT3<T> rightNormal = (world_T_camera * VectorT3<T>(-focalLengthX, 0, -rightPx) - translation).normalizedOrZero();

	planes_[PI_LEFT] = PlaneT3<T>(translation, leftNormal);
	planes_[PI_RIGHT] = PlaneT3<T>(translation, rightNormal);

	const T& topPy = principalPointY;
	const T bottomPy = height - principalPointY;

	const VectorT3<T> topNormal = (world_T_camera * VectorT3<T>(0, -focalLengthY, -topPy) - translation).normalizedOrZero();
	const VectorT3<T> bottomNormal = (world_T_camera * VectorT3<T>(0, focalLengthY, -bottomPy) - translation).normalizedOrZero();

	planes_[PI_TOP] = PlaneT3<T>(translation, topNormal);
	planes_[PI_BOTTOM] = PlaneT3<T>(translation, bottomNormal);

	ocean_assert(isValid());
}

template <typename T>
FrustumT<T>::FrustumT(const HomogenousMatrixT4<T>& world_T_camera, const PinholeCamera& pinholeCamera, const T nearDistance, const T farDistance) :
	FrustumT(world_T_camera, T(pinholeCamera.width()), T(pinholeCamera.height()), T(pinholeCamera.focalLengthX()), T(pinholeCamera.focalLengthY()), T(pinholeCamera.principalPointX()), T(pinholeCamera.principalPointY()), nearDistance, farDistance)
{
	ocean_assert(pinholeCamera.isValid());
}

template <typename T>
FrustumT<T>::FrustumT(const HomogenousMatrixT4<T>& world_T_camera, const AnyCamera& pinholeCamera, const T nearDistance, const T farDistance) :
	FrustumT(world_T_camera, T(pinholeCamera.width()), T(pinholeCamera.height()), T(pinholeCamera.focalLengthX()), T(pinholeCamera.focalLengthY()), T(pinholeCamera.principalPointX()), T(pinholeCamera.principalPointY()), nearDistance, farDistance)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(pinholeCamera.anyCameraType() == AnyCameraType::PINHOLE);
}

template <typename T>
inline const PlaneT3<T>* FrustumT<T>::planes() const
{
	return planes_;
}

template <typename T>
inline bool FrustumT<T>::isInside(const VectorT3<T>& objectPoint) const
{
	ocean_assert(isValid());

	// the point is inside the frustum if the point is in front of all frustum planes

	for (unsigned int n = 0u; n < PI_END; ++n)
	{
		if (planes_[n].signedDistance(objectPoint) < T(0))
		{
			return false;
		}
	}

	return true;
}

template <typename T>
bool FrustumT<T>::isInside(const SphereT3<T>& sphere) const
{
	ocean_assert(isValid());
	ocean_assert(sphere.isValid());

	// the sphere is entirely inside the frustum if the signed distance is bigger/equal than the radius of the sphere for all planes

	for (unsigned int n = 0u; n < PI_END; ++n)
	{
		if (planes_[n].signedDistance(sphere.center()) < sphere.radius())
		{
			return false;
		}
	}

	return true;
}

template <typename T>
bool FrustumT<T>::isInside(const Box3& box) const
{
	ocean_assert(isValid());
	ocean_assert(box.isValid());

	// the box is entirely inside of the frustum if all 8 corners of the box are in front of all planes

	for (unsigned int n = 0u; n < PI_END; ++n)
	{
		const PlaneT3<T>& plane = planes_[n];

		if (plane.signedDistance(box.lower()) < T(0)
			|| plane.signedDistance(VectorT3<T>(box.lower().x(), box.lower().y(), box.higher().z())) < T(0)
			|| plane.signedDistance(VectorT3<T>(box.lower().x(), box.higher().y(), box.lower().z())) < T(0)
			|| plane.signedDistance(VectorT3<T>(box.lower().x(), box.higher().y(), box.higher().z())) < T(0)
			|| plane.signedDistance(VectorT3<T>(box.higher().x(), box.lower().y(), box.lower().z())) < T(0)
			|| plane.signedDistance(VectorT3<T>(box.higher().x(), box.lower().y(), box.higher().z())) < T(0)
			|| plane.signedDistance(VectorT3<T>(box.higher().x(), box.higher().y(), box.lower().z())) < T(0)
			|| plane.signedDistance(box.higher()) < T(0))
		{
			return false;
		}
	}

	return true;
}

template <typename T>
bool FrustumT<T>::isInside(const Vector3* vertices, const size_t size) const
{
	ocean_assert(isValid());
	ocean_assert(vertices != nullptr && size != 0);

	// the object is entirely inside of the frustum if all vertices of the object are in front of all planes

	for (unsigned int n = 0u; n < PI_END; ++n)
	{
		const PlaneT3<T>& plane = planes_[n];

		for (size_t nVertex = 0; nVertex < size; ++nVertex)
		{
			if (plane.signedDistance(vertices[nVertex]) < T(0))
			{
				return false;
			}
		}
	}

	return true;
}

template <typename T>
bool FrustumT<T>::hasIntersection(const SphereT3<T>& sphere) const
{
	ocean_assert(isValid());
	ocean_assert(sphere.isValid());

	// the sphere is partially inside the frustum if the signed distance is bigger than the (negative) radius of the sphere for all planes

	for (unsigned int n = 0u; n < PI_END; ++n)
	{
		if (planes_[n].signedDistance(sphere.center()) < -sphere.radius())
		{
			return false;
		}
	}

	return true;
}

template <typename T>
bool FrustumT<T>::hasIntersection(const Box3& box) const
{
	ocean_assert(isValid());
	ocean_assert(box.isValid());

	// the box is outside of the frustum if all 8 corners of the box are outside of one plane

	for (unsigned int n = 0u; n < PI_END; ++n)
	{
		const PlaneT3<T>& plane = planes_[n];

		if (plane.signedDistance(box.lower()) < T(0)
			&& plane.signedDistance(VectorT3<T>(box.lower().x(), box.lower().y(), box.higher().z())) < T(0)
			&& plane.signedDistance(VectorT3<T>(box.lower().x(), box.higher().y(), box.lower().z())) < T(0)
			&& plane.signedDistance(VectorT3<T>(box.lower().x(), box.higher().y(), box.higher().z())) < T(0)
			&& plane.signedDistance(VectorT3<T>(box.higher().x(), box.lower().y(), box.lower().z())) < T(0)
			&& plane.signedDistance(VectorT3<T>(box.higher().x(), box.lower().y(), box.higher().z())) < T(0)
			&& plane.signedDistance(VectorT3<T>(box.higher().x(), box.higher().y(), box.lower().z())) < T(0)
			&& plane.signedDistance(box.higher()) < T(0))
		{
			return false;
		}
	}

	return true;
}

template <typename T>
bool FrustumT<T>::hasIntersection(const Vector3* vertices, const size_t size) const
{
	ocean_assert(isValid());
	ocean_assert(vertices != nullptr && size != 0);

	// the object is outside of the frustum if all vertices of the object are outside of at least one plane

	for (unsigned int n = 0u; n < PI_END; ++n)
	{
		const PlaneT3<T>& plane = planes_[n];

		bool allOutside = true;

		for (size_t nVertex = 0; allOutside && nVertex < size; ++nVertex)
		{
			allOutside = plane.signedDistance(vertices[nVertex]) < T(0);
		}

		if (allOutside)
		{
			return false;
		}
	}

	return true;
}

template <typename T>
inline bool FrustumT<T>::isValid() const
{
	return planes_[PI_FRONT].isValid();
}

template <typename T>
bool FrustumT<T>::isEqual(const FrustumT<T>& frustum, const T eps) const
{
	for (unsigned int n = 0u; n < PI_END; ++n)
	{
		if (!planes_[n].isEqual(frustum.planes_[n], eps))
		{
			return false;
		}
	}

	return true;
}

template <typename T>
inline bool FrustumT<T>::operator==(const FrustumT<T>& frustum) const
{
	return isEqual(frustum);
}

template <typename T>
inline bool FrustumT<T>::operator!=(const FrustumT<T>& frustum) const
{
	return !isEqual(frustum);
}

}

#endif // META_OCEAN_MATH_FRUSTUM_H
