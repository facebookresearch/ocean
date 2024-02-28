// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/scenedescription/sdx/x3d/Group.h"

#include "ocean/rendering/Group.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

Group::Group(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DChildNode(environment),
	X3DBoundedObject(environment),
	X3DGroupingNode(environment)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;

	renderingObject_ = engine()->factory().createGroup();
}

Group::NodeSpecification Group::specifyNode()
{
	NodeSpecification specification("Group");

	X3DGroupingNode::registerFields(specification);

	return specification;
}

size_t Group::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
