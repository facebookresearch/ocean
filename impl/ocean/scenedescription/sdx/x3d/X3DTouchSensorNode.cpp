/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/X3DTouchSensorNode.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

X3DTouchSensorNode::X3DTouchSensorNode(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DChildNode(environment),
	X3DSensorNode(environment),
	SDXEventNode(environment),
	X3DPointingDeviceSensorNode(environment)
{
	// nothing to do here
}

void X3DTouchSensorNode::registerFields(NodeSpecification& specification)
{
	registerField(specification, "touchTime", touchTime_, ACCESS_GET);

	X3DPointingDeviceSensorNode::registerFields(specification);
}

}

}

}

}
