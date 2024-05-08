/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_THING_H
#define META_OCEAN_RENDERING_GI_THING_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"
#include "ocean/rendering/globalillumination/GILightSource.h"
#include "ocean/rendering/globalillumination/RayIntersection.h"
#include "ocean/rendering/globalillumination/TracingObject.h"
#include "ocean/rendering/globalillumination/Lighting.h"

#include "ocean/math/HomogenousMatrix4.h"

#include "ocean/rendering/AttributeSet.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

// Forward declaration.
class TracingGroup;

/**
 * This class is the abstract base class for all tracing objects.
 * A tracing object represents the geometry and appearance of a renderable and holds all data necessary during the actual ray-tracing process.
 * @ingroup renderinggi
 */
class OCEAN_RENDERING_GI_EXPORT TracingObject
{
	public:

		/**
		 * Creates a new tracing object.
		 */
		TracingObject() = default;

		/**
		 * Destructs this tracing object.
		 */
		virtual ~TracingObject();

		/**
		 * Sets the light sources that will be used for rendering.
		 * @param lightSources The light sources
		 */
		void setLightSources(const LightSources& lightSources);

		/**
		 * Sets the attributes that will defined the appearance of the object.
		 * @param attributes The set of attributes
		 */
		void setAttributes(const AttributeSetRef& attributes);

		/**
		 * Determines the nearest intersection with between this tracing object and a given 3D ray.
		 * @param ray The 3D ray for which the nearest intersection will be determined, must be valid
		 * @param intersection The resulting nearest intersection with the provided ray
		 * @param frontFace True, to determine intersections with front faces; False, to determine intersections with back faces
		 * @param eps The maximal distance between the ray and the object so that a not perfectly hitting ray still counts as intersecting
		 * @param excludedObject An optional tracing object that will be excluded from the determination
		 */
		virtual void findNearestIntersection(const Line3& ray, RayIntersection& intersection, const bool frontFace, const Scalar eps, const TracingObject* excludedObject = nullptr) const = 0;

		/**
		 * Determines whether this tracing object has an intersection with a provided 3D ray.
		 * @param ray The 3D ray for which the nearest intersection will be determined, must be valid
		 * @param maximalDistance The maximal distance between the object and the starting point of the ray, with range [0, infinity)
		 * @param excludedObject An optional tracing object that will be excluded from the determination
		 * @return True, if the provided ray has an intersection with this object within the specified maximal distance
		 */
		virtual bool hasIntersection(const Line3& ray, const Scalar maximalDistance = Numeric::maxValue(), const TracingObject* excludedObject = nullptr) const = 0;

		/**
		 * Determines the amount of light that transmits trough this object in the case e.g., this object is transparent.
		 * @param ray The ray of the light for which the damped color/light is determined
		 * @param color The resulting color/light
		 * @param maximalDistance The maximal distance between the start position of the ray and the intersection of the object, with rang (0, infinity)
		 * @return True, if succeeded
		 */
		virtual bool determineDampingColor(const Line3& ray, RGBAColor& color, const Scalar maximalDistance = Numeric::maxValue()) const = 0;

		/**
		 * Determines the light (the color) for a specified viewing ray this object does reflect.
		 * @param viewPosition The start position of the viewing ray
		 * @param viewObjectDirection The direction of the viewing ray
		 * @param intersection The already known intersection of the viewing ray with this object
		 * @param group All tracing objects of the entire scene
		 * @param bounces The number of reflection bounces to be used, with range [0, infinity)
		 * @param excludedObject An optional tracking object to be excluded during this iteration, nullptr to use every tracing object
		 * @param lightingModes The light lightingModes which will be used for the lighting
		 * @param color The resulting color/light for the defined viewing ray
		 * @return True, if succeeded
		 */
		virtual bool determineColor(const Vector3& viewPosition, const Vector3& viewObjectDirection, const RayIntersection& intersection, const TracingGroup& group, const unsigned int bounces, const TracingObject* excludedObject, const Lighting::LightingModes lightingModes, RGBAColor& color) const = 0;

	protected:

		/**
		 * Sets the transformation transforming points located in the coordinate system of this tracing object to points located in the world coordinate system.
		 * @param objectTransformation The transformation to be set
		 */
		inline void setObjectTransformation(const HomogenousMatrix4& objectTransformation);

	protected:

		/// The transformation transforming points located in the coordinate system of this tracing object to points located in the world coordinate system.
		HomogenousMatrix4 objectTransformation_ = HomogenousMatrix4(false);

		/// The transformation transforming points located in the world coordinate system to points located in this tracing object's coordinate system.
		HomogenousMatrix4 invertedObjectTransformation_ = HomogenousMatrix4(false);

		/// The material of the sphere, if any.
		GIMaterial* material_;

		/// The textures of the sphere, if any.
		GITextures* textures_;

		/// The light sources which will be used for rendering.
		LightSources lightSources_;

		/// The set of attributes defining the appearance of the sphere.
		AttributeSetRef attributes_;
};

inline void TracingObject::setObjectTransformation(const HomogenousMatrix4& objectTransformation)
{
	ocean_assert(objectTransformation.isValid());

	objectTransformation_ = objectTransformation;
	invertedObjectTransformation_ = objectTransformation.inverted();
}

}

}

}

#endif // META_OCEAN_RENDERING_GI_THING_H
