/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESLOD.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESLOD::GLESLOD() :
	GLESGroup(),
	LOD()
{
	// nothing to do here
}

GLESLOD::~GLESLOD()
{
	// nothing to do here
}

GLESLOD::DistanceRanges GLESLOD::distanceRanges() const
{
	const ScopedLock scopedLock(objectLock);

	return distanceRanges_;
}

void GLESLOD::setDistanceRanges(const DistanceRanges& ranges)
{
	const ScopedLock scopedLock(objectLock);

	distanceRanges_ = ranges;
}

void GLESLOD::addToTraverser(const GLESFramebuffer& framebuffer, const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& camera_T_object, const Lights& lights, GLESTraverser& traverser) const
{
	const ScopedLock scopedLock(objectLock);

	if (!visible_ || groupNodes.empty())
	{
		return;
	}

	const Scalar distance = camera_T_object.translation().length();

	// n+1 range values for n objects

	for (size_t n = 1; n < distanceRanges_.size() && n <= groupNodes.size(); ++n)
	{
		if (distanceRanges_[n - 1] <= distance && distance <= distanceRanges_[n])
		{
			const SmartObjectRef<GLESNode> node(groupNodes[n - 1]);
			ocean_assert(node);

			if (groupLights.empty())
			{
				node->addToTraverser(framebuffer, projectionMatrix, camera_T_object, lights, traverser);
			}
			else
			{
				Lights newLights(lights);
				newLights.reserve(newLights.size() + groupLights.size());

				for (const LightSourceRef& light : groupLights)
				{
					ocean_assert(light);

					if (light->enabled() && light->intensity() > 0)
					{
						newLights.emplace_back(light, camera_T_object);
					}
				}

				node->addToTraverser(framebuffer, projectionMatrix, camera_T_object, newLights, traverser);
			}

			break;
		}
	}
}

}

}

}
