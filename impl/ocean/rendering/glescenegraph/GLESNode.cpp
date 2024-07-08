/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESNode.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESNode::GLESNode() :
	GLESObject(),
	Node(),
	visible_(true)
{
	// nothing to do here
}

GLESNode::~GLESNode()
{
	// nothing to do here
}

bool GLESNode::visible() const
{
	const ScopedLock scopedLock(objectLock);

	return visible_;
}

void GLESNode::setVisible(const bool visible)
{
	const ScopedLock scopedLock(objectLock);

	visible_ = visible;
}

}

}

}
