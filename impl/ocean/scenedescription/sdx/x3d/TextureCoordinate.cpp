/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/TextureCoordinate.h"

#include "ocean/rendering/VertexSet.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

TextureCoordinate::TextureCoordinate(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DGeometricPropertyNode(environment),
	X3DTextureCoordinateNode(environment)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;
}

TextureCoordinate::NodeSpecification TextureCoordinate::specifyNode()
{
	NodeSpecification specification("TextureCoordinate");

	registerField(specification, "point", point_);

	X3DTextureCoordinateNode::registerFields(specification);

	return specification;
}

void TextureCoordinate::onFieldChanged(const std::string& fieldName)
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

	X3DTextureCoordinateNode::onFieldChanged(fieldName);
}

size_t TextureCoordinate::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
