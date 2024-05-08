/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/Sphere.h"

#include "ocean/rendering/Sphere.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

Sphere::Sphere(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DGeometryNode(environment),
	radius_(1),
	solid_(true)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;

	renderingObject_ = engine()->factory().createSphere();
}

Sphere::NodeSpecification Sphere::specifyNode()
{
	NodeSpecification specification("Sphere");

	registerField(specification, "radius", radius_, ACCESS_NONE);
	registerField(specification, "solid", solid_, ACCESS_NONE);

	X3DGeometryNode::registerFields(specification);

	return specification;
}

void Sphere::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DGeometryNode::onInitialize(scene, timestamp);

	try
	{
		const Rendering::SphereRef sphere(renderingObject_);

		if (sphere)
		{
			sphere->setRadius(radius_.value());
		}
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}
}

size_t Sphere::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
