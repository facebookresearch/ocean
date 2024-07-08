/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESSwitch.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESSwitch::GLESSwitch() :
	GLESGroup(),
	Switch()
{
	// nothing to do here
}

GLESSwitch::~GLESSwitch()
{
	// nothing to do here
}

Index32 GLESSwitch::activeNode() const
{
	const ScopedLock scopedLock(objectLock);

	return activeNodeIndex_;
}

void GLESSwitch::setActiveNode(const Index32 index)
{
	const ScopedLock scopedLock(objectLock);

	if (index >= numberChildren() && index != invalidIndex)
	{
		ocean_assert(false && "Invalid index!");
		return;
	}

	activeNodeIndex_ = index;
}

void GLESSwitch::addToTraverser(const GLESFramebuffer& framebuffer, const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& camera_T_parent, const Lights& lights, GLESTraverser& traverser) const
{
	const ScopedLock scopedLock(objectLock);

	if (!visible_ || groupNodes.empty())
	{
		return;
	}

	if (activeNodeIndex_ != invalidIndex && activeNodeIndex_ < numberChildren())
	{
		const SmartObjectRef<GLESNode> node(groupNodes[activeNodeIndex_]);
		ocean_assert(node);

		node->addToTraverser(framebuffer, projectionMatrix, camera_T_parent, lights, traverser);
	}
}

}

}

}
