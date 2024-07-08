/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/Shape.h"

#include "ocean/rendering/AttributeSet.h"
#include "ocean/rendering/Geometry.h"
#include "ocean/rendering/PrimitiveAttribute.h"
#include "ocean/rendering/Renderable.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

Shape::Shape(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DBoundedObject(environment),
	X3DNode(environment),
	X3DChildNode(environment),
	X3DShapeNode(environment)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;

	renderingObject_ = engine()->factory().createGeometry();
}

Shape::NodeSpecification Shape::specifyNode()
{
	NodeSpecification specification("Shape");

	X3DShapeNode::registerFields(specification);

	return specification;
}

size_t Shape::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
