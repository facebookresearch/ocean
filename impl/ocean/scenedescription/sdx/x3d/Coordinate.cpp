/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/Coordinate.h"

#include "ocean/rendering/VertexSet.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

Coordinate::Coordinate(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DGeometricPropertyNode(environment),
	X3DCoordinateNode(environment)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;
}

Coordinate::NodeSpecification Coordinate::specifyNode()
{
	NodeSpecification specification("Coordinate");

	registerField(specification, "point", point_);

	X3DCoordinateNode::registerFields(specification);

	return specification;
}

void Coordinate::onFieldChanged(const std::string& fieldName)
{
	if (fieldName == "point")
	{
		const NodeRefs parents(parentNodes());
		for (NodeRefs::const_iterator i = parents.begin(); i != parents.end(); ++i)
		{
			ocean_assert(false && "Missing implementation.");
		}

		return;
	}

	X3DCoordinateNode::onFieldChanged(fieldName);
}

size_t Coordinate::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
