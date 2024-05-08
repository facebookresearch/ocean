/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/globalillumination/TracingGroup.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

TracingGroup::~TracingGroup()
{
	for (TracingObjects::iterator i = tracingObjects.begin(); i != tracingObjects.end(); ++i)
	{
		delete *i;
	}
}

void TracingGroup::findNearestIntersection(const Line3& ray, RayIntersection& intersection, const bool frontFace, const Scalar eps, const TracingObject* excludedObject) const
{
	for (TracingObjects::const_iterator i = tracingObjects.begin(); i != tracingObjects.end(); ++i)
	{
		if (*i != excludedObject)
		{
			(*i)->findNearestIntersection(ray, intersection, frontFace, eps);
		}
	}
}

bool TracingGroup::hasIntersection(const Line3& ray, const Scalar maximalDistance, const TracingObject* excludedObject) const
{
	for (TracingObjects::const_iterator i = tracingObjects.begin(); i != tracingObjects.end(); ++i)
	{
		if (*i != excludedObject && (*i)->hasIntersection(ray, maximalDistance))
		{
				return true;
		}
	}

	return false;
}

bool TracingGroup::determineDampingColor(const Line3& ray, RGBAColor& color, const Scalar maximalDistance) const
{
	for (TracingObjects::const_iterator i = tracingObjects.begin(); i != tracingObjects.end(); ++i)
	{
		if (!(*i)->determineDampingColor(ray, color, maximalDistance))
		{
			return false;
		}
	}

	return true;
}

bool TracingGroup::determineColor(const Vector3& /*viewPosition*/, const Vector3& /*viewObjectDirection*/, const RayIntersection& /*intersection*/, const TracingGroup& /*group*/, const unsigned int /*bounces*/, const TracingObject* /*excludedObject*/, const Lighting::LightingModes /*lightingModes*/, RGBAColor& /*color*/) const
{
	ocean_assert(false && "This should never happen!");
	return false;
}

}

}

}
