/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/X3DPointingDeviceSensorNode.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

X3DPointingDeviceSensorNode::X3DPointingDeviceSensorNode(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DChildNode(environment),
	X3DSensorNode(environment),
	SDXEventNode(environment),
	isOver_(false),
	isActive_(false)
{
	// nothing to do here
}

void X3DPointingDeviceSensorNode::registerFields(NodeSpecification& specification)
{
	registerField(specification, "description", description_, ACCESS_GET_SET);
	registerField(specification, "isOver", isOver_, ACCESS_GET);
	registerField(specification, "isActive", isActive_, ACCESS_GET);

	X3DSensorNode::registerFields(specification);
}

}

}

}

}
