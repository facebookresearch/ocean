/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESScene.h"
#include "ocean/rendering/glescenegraph/GLESFramebuffer.h"
#include "ocean/rendering/glescenegraph/GLESView.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESScene::GLESScene() :
	GLESTransform(),
	Scene()
{
	// nothing to do here
}

GLESScene::~GLESScene()
{
	// nothing to do here
}

void GLESScene::addToTraverser(const GLESFramebuffer& framebuffer, const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& camera_T_parent, const Lights& lights, GLESTraverser& traverser) const
{
	const ScopedLock scopedLock(objectLock);

	if (!visible_ || groupNodes.empty() || parent_T_object_.rotationMatrix().isNull())
	{
		return;
	}

	Lights newLights;

	if (useHeadlight())
	{
		newLights = lights;
	}
	else
	{
		const SmartObjectRef<GLESView> glesView(framebuffer.view());

		for (const LightPair& lightPair : lights)
		{
			if (lightPair.first != glesView->headlight())
			{
				newLights.emplace_back(lightPair);
			}
		}
	}

	const HomogenousMatrix4 camera_T_object = transformModifier_ ? (camera_T_parent * parent_T_object_ * transformModifier_->transformation()) : (camera_T_parent * parent_T_object_);

	if (groupLights.empty())
	{
		for (Nodes::const_iterator i = groupNodes.begin(); i != groupNodes.end(); ++i)
		{
			const SmartObjectRef<GLESNode> node(*i);
			ocean_assert(node);

			node->addToTraverser(framebuffer, projectionMatrix, camera_T_object, newLights, traverser);
		}
	}
	else
	{
		for (LightSet::const_iterator i = groupLights.begin(); i != groupLights.end(); ++i)
		{
			if ((*i)->enabled())
			{
				newLights.emplace_back(*i, camera_T_object);
			}
		}

		for (Nodes::const_iterator i = groupNodes.begin(); i != groupNodes.end(); ++i)
		{
			const SmartObjectRef<GLESNode> node(*i);
			ocean_assert(node);

			node->addToTraverser(framebuffer, projectionMatrix, camera_T_object, newLights, traverser);
		}
	}
}

}

}

}
