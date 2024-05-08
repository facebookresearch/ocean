/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_RAY_INTERSECTION_H
#define META_OCEAN_RENDERING_GI_RAY_INTERSECTION_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"
#include "ocean/rendering/globalillumination/GILightSource.h"

#include "ocean/math/Line3.h"

#include "ocean/rendering/AttributeSet.h"
#include "ocean/rendering/Renderable.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

// Forward declaration.
class TracingObject;

// Forward declaration.
class RayIntersection;

/**
 * Definition of a vector holding ray intersection objects.
 */
typedef std::vector<RayIntersection> RayIntersections;

/**
 * This class implements a ray intersection object.
 * @ingroup renderinggi
 */
class RayIntersection
{
	public:

		/**
		 * Creates an empty ray intersection object.
		 */
		inline RayIntersection();

		/**
		 * Creates a new ray intersection object.
		 * @param position Intersection position
		 * @param direction Intersection direction
		 * @param normal Intersection normal
		 * @param textureCoordinate Intersection texture coordinate
		 * @param distance Intersection distance
		 * @param tracingObject Tracing object pointer
		 * @param lightSources Light sources
		 */
		inline RayIntersection(const Vector3& position, const Vector3& direction, const Vector3& normal, const TextureCoordinate& textureCoordinate, const Scalar distance, const TracingObject* tracingObject, const LightSources& lightSources);

		/**
		 * Returns the intersection position.
		 * @return Intersection position
		 */
		inline const Vector3& position() const;

		/**
		 * Returns the intersection direction.
		 * @return Intersection direction
		 */
		inline const Vector3& direction() const;

		/**
		 * Returns the normal.
		 * @return Intersection normal
		 */
		inline const Normal& normal() const;

		/**
		 * Returns the intersection texture coordinate.
		 * @return Intersection texture coordinate
		 */
		inline const TextureCoordinate& textureCoordinate() const;

		/**
		 * Returns the distance.
		 * @return Intersection distance
		 */
		inline Scalar distance() const;

		/**
		 * Returns the renderable object.
		 * @return Renderable object
		 */
		inline const TracingObject* tracingObject() const;

		/**
		 * Returns the light sources.
		 * @return Light sources
		 */
		inline const LightSources& lightSources() const;

		/**
		 * Compares two objects.
		 * @param intersection Second object to compare
		 * @return True, if the second object has a small intersection distance
		 */
		inline bool operator<(const RayIntersection& intersection) const;

		/**
		 * Returns whether this object holds a valid intersection.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

	private:

		/// Intersection position.
		Vector3 intersectionPosition;

		/// Intersection direction.
		Vector3 intersectionDirection;

		/// Intersection normal.
		Normal intersectionNormal;

		/// Intersection texture coordinate.
		TextureCoordinate intersectionTextureCoordinate;

		/// Intersection distance.
		Scalar intersectionDistance;

		/// Renderable object.
		const TracingObject* intersectionTracingObject;

		/// Light sources.
		LightSources intersectionLightSources;
};

inline RayIntersection::RayIntersection() :
	intersectionPosition(0, 0, 0),
	intersectionDirection(0, 0, 0),
	intersectionNormal(0, 0, 0),
	intersectionTextureCoordinate(0, 0),
	intersectionDistance(Numeric::maxValue()),
	intersectionTracingObject(nullptr)
{
	// nothing to do here
}

inline RayIntersection::RayIntersection(const Vector3& position, const Vector3& direction, const Vector3& normal, const TextureCoordinate& textureCoordinate, const Scalar distance, const TracingObject* tracingObject, const LightSources& lightSources) :
	intersectionPosition(position),
	intersectionDirection(direction),
	intersectionNormal(normal),
	intersectionTextureCoordinate(textureCoordinate),
	intersectionDistance(distance),
	intersectionTracingObject(tracingObject),
	intersectionLightSources(lightSources)
{
	ocean_assert(Numeric::isEqual(normal.length(), 1));
	ocean_assert(Numeric::isEqual(direction.length(), 1));
}

inline const Vector3& RayIntersection::position() const
{
	return intersectionPosition;
}

inline const Vector3& RayIntersection::direction() const
{
	return intersectionDirection;
}

inline const Normal& RayIntersection::normal() const
{
	return intersectionNormal;
}

inline const TextureCoordinate& RayIntersection::textureCoordinate() const
{
	return intersectionTextureCoordinate;
}

inline Scalar RayIntersection::distance() const
{
	return intersectionDistance;
}

inline const TracingObject* RayIntersection::tracingObject() const
{
	ocean_assert(intersectionTracingObject);
	return intersectionTracingObject;
}

inline const LightSources& RayIntersection::lightSources() const
{
	return intersectionLightSources;
}

inline bool RayIntersection::operator<(const RayIntersection& intersection) const
{
	return intersectionDistance < intersection.intersectionDistance;
}

inline RayIntersection::operator bool() const
{
	return intersectionDistance < Numeric::maxValue();
}

}

}

}

#endif // META_OCEAN_RENDERING_GI_RAY_INTERSECTION_H
