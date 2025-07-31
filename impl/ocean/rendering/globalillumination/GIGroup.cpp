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

	for (const NodeRef& groupNode : groupNodes)
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
		if (groupLights.empty())
		{
			for (Nodes::const_iterator i = groupNodes.begin(); i != groupNodes.end(); ++i)
			{
				SmartObjectRef<GINode> node(*i);
				ocean_assert(node);

				node->buildTracing(group, modelTransform, lightSources);
			}
		}
		else
		{
			LightSources newLightSources(lightSources);

			for (LightSet::const_iterator i = groupLights.begin(); i != groupLights.end(); ++i)
				newLightSources.push_back(LightPair(*i, modelTransform));

			for (Nodes::const_iterator i = groupNodes.begin(); i != groupNodes.end(); ++i)
			{
				SmartObjectRef<GINode> node(*i);
				ocean_assert(node);

				node->buildTracing(group, modelTransform, newLightSources);
			}
		}
	}
}

}

}

}
