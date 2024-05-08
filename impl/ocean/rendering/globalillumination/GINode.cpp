/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/globalillumination/GINode.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

GINode::GINode() :
	GIObject(),
	Node(),
	nodeVisible(true)
{
	// nothing to do here
}

GINode::~GINode()
{
	// nothing to do here
}

bool GINode::visible() const
{
	return nodeVisible;
}

void GINode::setVisible(const bool visible)
{
	nodeVisible = visible;
}

}

}

}
