/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_GEOMETRY_CONE_H
#define META_OCEAN_GEOMETRY_CONE_H

#include "ocean/geometry/Geometry.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/SquareMatrix4.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace Geometry
{

/**
 * This class implements the base for all quadric object.
 * @ingroup geometry
 */
class OCEAN_GEOMETRY_EXPORT Quadric
{
	public:

		/**
		 * Creates an invalid quadric.
		 */
		Quadric();

		/**
		 * Returns whether a given point lies inside or on the surface of the cone.
		 * @param point Point to check
		 * @return True, if so
		 */
		bool isInside(const Vector3& point) const;

		/**
		 * Returns whether a given point lies on the surface of the cone.
		 * @param point Point to check
		 * @return True, if so
		 */
		bool isOnSurface(const Vector3& point) const;

		/**
		 * Returns the quadric value for a given point.
		 * @param point Point to check
		 * @return Quadric value
		 */
		Scalar value(const Vector3& point) const;

	protected:

		/**
		 * Creates the combination of quadric and transformation matrix.
		 * @param quadric Quadric matrix
		 * @param transformation Transformation matrix to be combined with the quadric
		 */
		void createCombinedQuadric(const SquareMatrix4& quadric, const HomogenousMatrix4 &transformation);

	protected:

		/// Holds the combined quadric and transformation matrix.
		SquareMatrix4 combinedQuadric;
};

/**
 * This class implements a cone quadric.
 * Quadric for cone Q:
 * Cone apex at: (m_x, m_y, m_z)
 * Cone angle: a
 *
 * [   1            0             0                      -m_x                 ]<br>
 * [   0            1             0                      -m_y                 ]<br>
 * [   0            0          -tan^2(a)              m_z tan^2               ]<br>
 * [  -m_x         -m_y        m_z tan^2(a)  (m_x^2 + m_y^2 - m_z^2 tan^2(a)) ]<br>
 *
 * Inside test: p^T * Q * p <= 0.0
 * @ingroup geometry
 */
class OCEAN_GEOMETRY_EXPORT Cone : public Quadric
{
	public:

		/**
		 * Creates an invalid cone.
		 */
		Cone();

		/**
		 * Creates a cone by a given dihedral angle and a transformation.
		 * @param angle Dihedral angle of the cone in radian, which is half of the aperture (aperture = 2 * angle)
		 * @param transformation Transformation to transform the default cone defined about the z axis
		 */
		Cone(const Scalar angle, const HomogenousMatrix4 &transformation);
};

/**
 * This class implements a sphere quadric.
 * Quadric for sphere Q:
 * Sphere midpoint at: (m_x, m_y, m_z)
 * Sphere radius: r
 *
 * [   1         0        0                 -m_x              ]<br>
 * [   0         1        0                 -m_y              ]<br>
 * [   0         0        1                 -m_z              ]<br>
 * [  -m_x     -m_y     -m_z    (m_x^2 + m_y^2 - m_z^2 - r^2) ]<br>
 *
 * Inside test: p^T * Q * p <= 0.0
 * @ingroup geometry
 */
class OCEAN_GEOMETRY_EXPORT Sphere : public Quadric
{
	public:

		/**
		 * Creates an invalid sphere.
		 */
		Sphere();

		/**
		 * Creates a sphere by a given radius and a transformation.
		 * @param radius Radius of the sphere
		 * @param transformation Transformation to transform the default sphere defined with midpoint at (0, 0, 0)
		 */
		Sphere(const Scalar radius, const HomogenousMatrix4 &transformation);
};

}

}

#endif // META_OCEAN_GEOMETRY_CONE_H
