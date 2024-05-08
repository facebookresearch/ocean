/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/Color.h"

#include "ocean/rendering/VertexSet.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

Color::Color(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DGeometricPropertyNode(environment),
	X3DColorNode(environment)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;
}

Color::NodeSpecification Color::specifyNode()
{
	NodeSpecification specification("Color");

	registerField(specification, "color", color_);

	X3DColorNode::registerFields(specification);

	return specification;
}

void Color::onFieldChanged(const std::string& fieldName)
{
	if (fieldName == "color")
	{
		const NodeRefs parents(parentNodes());
		for (NodeRefs::const_iterator i = parents.begin(); i != parents.end(); ++i)
		{
			ocean_assert(false && "Missing implementation.");
		}

		return;
	}

	X3DColorNode::onFieldChanged(fieldName);
}

size_t Color::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
