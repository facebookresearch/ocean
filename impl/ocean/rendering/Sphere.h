/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_SPHERE_H
#define META_OCEAN_RENDERING_SPHERE_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Shape.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class Sphere;

/**
 * Definition of a smart object reference holding a sphere.
 * @see SmartObjectRef, Sphere.
 * @ingroup rendering
 */
typedef SmartObjectRef<Sphere> SphereRef;

/**
 * This class is the base class for all spheres.<br>
 * A default sphere has a radius of 1<br>
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT Sphere : virtual public Shape
{
	public:

		/**
		 * Returns the radius of this sphere.
		 * @return Radius of the sphere
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual Scalar radius() const;

		/**
		 * Sets the radius of this sphere.
		 * @param radius Radius to set
		 * @return True, if the specified radius was valid and could be set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual bool setRadius(const Scalar radius);

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new sphere object.
		 */
		Sphere();

		/**
		 * Destructs a sphere object.
		 */
		~Sphere() override;

		/**
		 * Calculates the coordinate for given latitude and longitude values.
		 * The resulting position is for a sphere with radius 1.
		 * @param latitude The latitude value, in radian
		 * @param longitude The longitude value, in radian
		 * @return Resulting position on the sphere surface
		 */
		static Vertex vertex(const Scalar latitude, const Scalar longitude);
};

}

}

#endif // META_OCEAN_RENDERING_SPHERE_H
