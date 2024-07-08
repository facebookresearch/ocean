/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/Box.h"

#include "ocean/rendering/Box.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

Box::Box(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DGeometryNode(environment),
	size_(Vector3(2, 2, 2)),
	solid_(true)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;

	renderingObject_ = engine()->factory().createBox();
}

Box::NodeSpecification Box::specifyNode()
{
	NodeSpecification specification("Box");

	registerField(specification, "size", size_, ACCESS_NONE);
	registerField(specification, "solid", solid_, ACCESS_NONE);

	X3DGeometryNode::registerFields(specification);

	return specification;
}

void Box::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DGeometryNode::onInitialize(scene, timestamp);

	try
	{
		const Rendering::BoxRef box(renderingObject_);

		if (box)
		{
			box->setSize(size_.value());
		}
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}
}

size_t Box::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
