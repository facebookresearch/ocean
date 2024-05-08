/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/X3DBindableNode.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

X3DBindableNode::X3DBindableNode(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DChildNode(environment),
	setBind_(false),
	bindTime_(Timestamp()),
	isBound_(false)
{
	// nothing to do here
}

void X3DBindableNode::registerFields(NodeSpecification& specification)
{
	registerField(specification, "set_bind", setBind_, ACCESS_SET);
	registerField(specification, "bindTime", bindTime_, ACCESS_GET);
	registerField(specification, "isBound", isBound_, ACCESS_GET);

	X3DChildNode::registerFields(specification);
}

}

}

}

}
