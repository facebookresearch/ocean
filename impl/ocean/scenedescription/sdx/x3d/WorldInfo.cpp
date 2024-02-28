// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/scenedescription/sdx/x3d/WorldInfo.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

WorldInfo::WorldInfo(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DChildNode(environment),
	X3DInfoNode(environment)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;
}

WorldInfo::NodeSpecification WorldInfo::specifyNode()
{
	NodeSpecification specification("WorldInfo");

	registerField(specification, "info", info_, ACCESS_NONE);
	registerField(specification, "title", title_, ACCESS_NONE);

	X3DInfoNode::registerFields(specification);

	return specification;
}

size_t WorldInfo::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
