/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESGroup.h"
#include "ocean/rendering/glescenegraph/GLESLightSource.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESGroup::GLESGroup() :
	GLESNode(),
	Group()
{
	// nothing to do here
}

GLESGroup::~GLESGroup()
{
	// nothing to do here
}

BoundingBox GLESGroup::boundingBox(const bool /*involveLocalTransformation*/) const
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

void GLESGroup::addChild(const NodeRef& node)
{
	if (node.isNull())
	{
		return;
	}

	const ScopedLock scopedLock(objectLock);

	Group::addChild(node);
}

void GLESGroup::registerLight(const LightSourceRef& light)
{
	if (light.isNull())
	{
		return;
	}

	const ScopedLock scopedLock(objectLock);

	Group::registerLight(light);
}

void GLESGroup::removeChild(const NodeRef& node)
{
	if (node.isNull())
	{
		return;
	}

	const ScopedLock scopedLock(objectLock);

	Group::removeChild(node);
}

void GLESGroup::unregisterLight(const LightSourceRef& light)
{
	if (light.isNull())
	{
		return;
	}

	const ScopedLock scopedLock(objectLock);

	Group::unregisterLight(light);
}

void GLESGroup::clear()
{
	const ScopedLock scopedLock(objectLock);

	Group::clear();
}

void GLESGroup::addToTraverser(const GLESFramebuffer& framebuffer, const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& camera_T_object, const Lights& lights, GLESTraverser& traverser) const
{
	const ScopedLock scopedLock(objectLock);

	if (!visible_ || groupNodes.empty())
	{
		return;
	}

	if (groupLights.empty())
	{
		for (const NodeRef& groupNode : groupNodes)
		{
			const SmartObjectRef<GLESNode> node(groupNode);
			ocean_assert(node);

			node->addToTraverser(framebuffer, projectionMatrix, camera_T_object, lights, traverser);
		}
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

		for (const NodeRef& groupNode : groupNodes)
		{
			const SmartObjectRef<GLESNode> node(groupNode);
			ocean_assert(node);

			node->addToTraverser(framebuffer, projectionMatrix, camera_T_object, newLights, traverser);
		}
	}
}

}

}

}
