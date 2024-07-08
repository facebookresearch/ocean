/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/Cylinder.h"

#include "ocean/rendering/Cylinder.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

Cylinder::Cylinder(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DGeometryNode(environment),
	bottom_(true),
	height_(2),
	radius_(1),
	side_(true),
	solid_(true),
	top_(true)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;

	renderingObject_ = engine()->factory().createCylinder();
}

Cylinder::NodeSpecification Cylinder::specifyNode()
{
	NodeSpecification specification("Cylinder");

	registerField(specification, "bottom", bottom_, ACCESS_NONE);
	registerField(specification, "height", height_, ACCESS_NONE);
	registerField(specification, "radius", radius_, ACCESS_NONE);
	registerField(specification, "side", side_, ACCESS_NONE);
	registerField(specification, "solid", solid_, ACCESS_NONE);
	registerField(specification, "top", top_, ACCESS_NONE);

	X3DGeometryNode::registerFields(specification);

	return specification;
}

void Cylinder::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DGeometryNode::onInitialize(scene, timestamp);

	try
	{
		const Rendering::CylinderRef cylinder(renderingObject_);

		if (cylinder)
		{
			cylinder->setHeight(height_.value());
			cylinder->setRadius(radius_.value());

			cylinder->setTop(top_.value());
			cylinder->setSide(side_.value());
			cylinder->setBottom(bottom_.value());
		}
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}
}

size_t Cylinder::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
