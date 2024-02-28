// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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
