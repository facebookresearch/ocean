/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/globalillumination/GIGroup.h"
#include "ocean/rendering/globalillumination/GILightSource.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

GIGroup::GIGroup() :
	GINode(),
	Group()
{
	// nothing to do here
}

GIGroup::~GIGroup()
{
	// nothing to do here
}

void GIGroup::addChild(const NodeRef& node)
{
	Group::addChild(node);
}

void GIGroup::registerLight(const LightSourceRef& light)
{
	Group::registerLight(light);
}

void GIGroup::removeChild(const NodeRef& node)
{
	Group::removeChild(node);
}

void GIGroup::unregisterLight(const LightSourceRef& light)
{
	Group::unregisterLight(light);
}

BoundingBox GIGroup::boundingBox(const bool /*involveLocalTransformation*/) const
{
	const ScopedLock scopedLock(objectLock);

	BoundingBox result;

	for (const NodeRef& groupNode : nodes_)
	{
		ocean_assert(groupNode);

		const BoundingBox groupBoundingBox = groupNode->boundingBox(true /*involveLocalTransformation*/);

		if (groupBoundingBox.isValid())
		{
			result += groupBoundingBox;
		}
	}

	return result;
}

void GIGroup::clear()
{
	Group::clear();
}

void GIGroup::buildTracing(TracingGroup& group, const HomogenousMatrix4& modelTransform, const LightSources& lightSources) const
{
	ocean_assert(modelTransform.isValid());

	if (nodeVisible)
	{
		if (lights_.empty())
		{
			for (const NodeRef& groupNode : nodes_)
			{
				const SmartObjectRef<GINode> node(groupNode);
				ocean_assert(node);

				node->buildTracing(group, modelTransform, lightSources);
			}
		}
		else
		{
			LightSources newLightSources(lightSources);

			for (const LightSourceRef& light : lights_)
			{
				ocean_assert(light);

				if (light->enabled() && light->intensity() > 0)
				{
					newLightSources.emplace_back(light, modelTransform);
				}
			}

			for (const NodeRef& groupNode : nodes_)
			{
				const SmartObjectRef<GINode> node(groupNode);
				ocean_assert(node);

				node->buildTracing(group, modelTransform, newLightSources);
			}
		}
	}
}

}

}

}
