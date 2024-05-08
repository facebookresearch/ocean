/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/Normal.h"

#include "ocean/rendering/VertexSet.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

Normal::Normal(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DGeometricPropertyNode(environment),
	X3DNormalNode(environment)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;
}

Normal::NodeSpecification Normal::specifyNode()
{
	NodeSpecification specification("Normal");

	registerField(specification, "vector", vector_);

	X3DNormalNode::registerFields(specification);

	return specification;
}

void Normal::onFieldChanged(const std::string& fieldName)
{
	if (fieldName == "vector")
	{
		const NodeRefs parents(parentNodes());
		for (NodeRefs::const_iterator i = parents.begin(); i != parents.end(); ++i)
		{
			ocean_assert(false && "Missing implementation.");
		}

		return;
	}

	X3DNormalNode::onFieldChanged(fieldName);
}

size_t Normal::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
