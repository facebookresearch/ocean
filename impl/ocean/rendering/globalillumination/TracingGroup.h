/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_TRACING_GROUP_H
#define META_OCEAN_RENDERING_GI_TRACING_GROUP_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"
#include "ocean/rendering/globalillumination/TracingObject.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

/**
 * This class implements a group of tracing objects.
 * @ingroup renderinggi
 */
class OCEAN_RENDERING_GI_EXPORT TracingGroup : public TracingObject
{
	private:

		/**
		 * Definition of a vector holding tracing objects.
		 */
		typedef std::vector<const TracingObject*> TracingObjects;

	public:

		/**
		 * Destructs this tracking object.
		 */
		~TracingGroup() override;

		/**
		 * Adds a new tracing object to this group object.
		 * @param object The new object to be added, must be valid
		 */
		inline void addObject(const TracingObject* object);

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

		/// The group of tracing objects.
		TracingObjects tracingObjects;
};

inline void TracingGroup::addObject(const TracingObject* object)
{
	ocean_assert(object);
	tracingObjects.push_back(object);
}

}

}

}

#endif // META_OCEAN_RENDERING_GI_TRACING_GROUP_H
