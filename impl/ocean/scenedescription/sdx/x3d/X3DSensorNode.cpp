/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/X3DSensorNode.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

X3DSensorNode::X3DSensorNode(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DChildNode(environment),
	enabled_(true)
{
	// nothing to do here
}

void X3DSensorNode::registerFields(NodeSpecification& specification)
{
	registerField(specification, "enabled", enabled_, ACCESS_GET_SET);

	X3DChildNode::registerFields(specification);
}

}

}

}

}
