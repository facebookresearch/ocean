/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/X3DViewpointNode.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

X3DViewpointNode::X3DViewpointNode(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DChildNode(environment),
	X3DBindableNode(environment),
	centerOfRotation_(Vector3(0, 0, 0)),
	description_(),
	jump_(true),
	orientation_(Rotation(0, 0, 1, 0)),
	position_(Vector3(0, 0, 10)),
	retainUserOffsets_(false)
{
	// nothing to do here
}

void X3DViewpointNode::registerFields(NodeSpecification& specification)
{
	registerField(specification, "centerOfRotation", centerOfRotation_, ACCESS_GET_SET);
	registerField(specification, "description", description_, ACCESS_GET_SET);
	registerField(specification, "jump", jump_, ACCESS_GET_SET);
	registerField(specification, "orientation", orientation_, ACCESS_GET_SET);
	registerField(specification, "position", position_, ACCESS_GET_SET);
	registerField(specification, "retainUserOffsets", retainUserOffsets_, ACCESS_GET_SET);

	X3DBindableNode::registerFields(specification);
}

}

}

}

}
