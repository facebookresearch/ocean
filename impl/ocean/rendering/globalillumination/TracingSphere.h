/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_TRACING_SPHERE_H
#define META_OCEAN_RENDERING_GI_TRACING_SPHERE_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"
#include "ocean/rendering/globalillumination/GITextures.h"
#include "ocean/rendering/globalillumination/TracingObject.h"

#include "ocean/math/BoundingBox.h"
#include "ocean/math/BoundingSphere.h"

#include "ocean/rendering/Material.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

/**
 * This class implements the tracing object for a sphere.
 * @ingroup renderinggi
 */
class OCEAN_RENDERING_GI_EXPORT TracingSphere : public TracingObject
{
	public:

		/**
		 * Creates a new tracing object.
		 */
		TracingSphere();

		/**
		 * Destructs this tracing object.
		 */
		~TracingSphere() override;

		/**
		 * Sets the main characteristics of the sphere by providing the object-world transformation and the radius.
		 * @param objectTransformation The transformation transforming points located in the coordinate system of the spehere to points located in the world coordinate system, must be valid
		 * @param localBoundingSphere The bounding sphere object defining the radius of the sphere, must be valid
		 */
		void setSphere(const HomogenousMatrix4& objectTransformation, const BoundingSphere& localBoundingSphere);

		/**
		 * Determines the nearest intersection with between this tracing object and a given 3D ray.
		 * @see TracingObject::findNearestIntersection().
		 */
		void findNearestIntersection(const Line3& ray, RayIntersection& intersection, const bool frontFace, const Scalar eps, const TracingObject* excludedObject = nullptr) const override;

		/**
		 * Determines whether this tracing object has an intersection with a provided 3D ray.
		 * @see TracingObject::hasIntersection().
		 */
		bool hasIntersection(const Line3& ray, const Scalar maximalDistance = Numeric::maxValue(), const TracingObject* excludedObject = nullptr) const override;

		/**
		 * Determines the amount of light that transmits trough this object in the case e.g., this object is transparent.
		 * @see TracingObject::determineDampingColor().
		 */
		bool determineDampingColor(const Line3& ray, RGBAColor& color, const Scalar maximalDistance = Numeric::maxValue()) const override;

		/**
		 * Determines the light (the color) for a specified viewing ray this object does reflect.
		 * @see TracingObject::determineColor().
		 */
		bool determineColor(const Vector3& viewPosition, const Vector3& viewObjectDirection, const RayIntersection& intersection, const TracingGroup& group, const unsigned int bounces, const TracingObject* excludedObject, const Lighting::LightingModes lightingModes, RGBAColor& color) const override;

	private:

		/// The bounding sphere object providing the radius of the sphere.
		BoundingSphere tracingLocalBoundingSphere;
};

}

}

}

#endif // META_OCEAN_RENDERING_GI_TRACING_SPHERE_H
