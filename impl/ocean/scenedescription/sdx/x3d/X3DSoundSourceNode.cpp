// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/scenedescription/sdx/x3d/X3DSoundSourceNode.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

X3DSoundSourceNode::X3DSoundSourceNode(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DChildNode(environment),
	SDXUpdateNode(environment),
	X3DTimeDependentNode(environment),
	description_(),
	pitch_(1),
	durationChanged_()
{
	// nothing to do here
}

void X3DSoundSourceNode::registerFields(NodeSpecification& specification)
{
	registerField(specification, "description", description_);
	registerField(specification, "pitch", pitch_);
	registerField(specification, "duration_changed", durationChanged_);

	X3DTimeDependentNode::registerFields(specification);
}

}

}

}

}
