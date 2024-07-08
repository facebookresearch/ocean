/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/Cone.h"

#include "ocean/rendering/Cone.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

Cone::Cone(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DGeometryNode(environment),
	bottom_(true),
	bottomRadius_(1),
	height_(2),
	side_(true),
	solid_(true)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;

	renderingObject_ = engine()->factory().createCone();
}

Cone::NodeSpecification Cone::specifyNode()
{
	NodeSpecification specification("Cone");

	registerField(specification, "bottom", bottom_, ACCESS_NONE);
	registerField(specification, "bottomRadius", bottomRadius_, ACCESS_NONE);
	registerField(specification, "height", height_, ACCESS_NONE);
	registerField(specification, "side", side_, ACCESS_NONE);
	registerField(specification, "solid", solid_, ACCESS_NONE);

	X3DGeometryNode::registerFields(specification);

	return specification;
}

void Cone::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DGeometryNode::onInitialize(scene, timestamp);

	try
	{
		const Rendering::ConeRef cone(renderingObject_);

		if (cone)
		{
			cone->setHeight(height_.value());
			cone->setRadius(bottomRadius_.value());
			cone->setSide(side_.value());
			cone->setBottom(bottom_.value());
		}
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}
}

size_t Cone::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
