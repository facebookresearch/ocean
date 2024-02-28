// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/scenedescription/sdx/x3d/X3DBoundedObject.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

X3DBoundedObject::X3DBoundedObject(const SDXEnvironment* environment) :
	SDXNode(environment),
	bboxCenter_(Vector3(0, 0, 0)),
	bboxSize_(Vector3(-1, -1, -1))
{
	// nothing to do here
}

void X3DBoundedObject::registerFields(NodeSpecification& specification)
{
	registerField(specification, "bboxCenter", bboxCenter_, ACCESS_NONE);
	registerField(specification, "bboxSize", bboxSize_, ACCESS_NONE);
}

}

}

}

}
