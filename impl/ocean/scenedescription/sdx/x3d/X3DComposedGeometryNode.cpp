/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/X3DComposedGeometryNode.h"

#include "ocean/rendering/Factory.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

X3DComposedGeometryNode::X3DComposedGeometryNode(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DGeometryNode(environment),
	ccw_(true),
	colorPerVertex_(true),
	normalPerVertex_(true),
	solid_(true)
{
	renderingVertexSet_ = engine()->factory().createVertexSet();
}

X3DComposedGeometryNode::~X3DComposedGeometryNode()
{
	if (initialized_)
	{
		for (MultiNode::Values::const_iterator i = attrib_.values().begin(); i != attrib_.values().end(); ++i)
		{
			unregisterThisNodeAsParent(*i);
		}

		unregisterThisNodeAsParent(color_.value());
		unregisterThisNodeAsParent(coord_.value());
		unregisterThisNodeAsParent(fogCoord_.value());
		unregisterThisNodeAsParent(normal_.value());
		unregisterThisNodeAsParent(texCoord_.value());
	}
}

void X3DComposedGeometryNode::registerFields(NodeSpecification& specification)
{
	registerField(specification, "attrib", attrib_, FieldAccessType(ACCESS_GET_SET | ACCESS_EXPLICIT_NOTIFICATION));
	registerField(specification, "color", color_, FieldAccessType(ACCESS_GET_SET | ACCESS_EXPLICIT_NOTIFICATION));
	registerField(specification, "coord", coord_, FieldAccessType(ACCESS_GET_SET | ACCESS_EXPLICIT_NOTIFICATION));
	registerField(specification, "fogCoord", fogCoord_, FieldAccessType(ACCESS_GET_SET | ACCESS_EXPLICIT_NOTIFICATION));
	registerField(specification, "normal", normal_, FieldAccessType(ACCESS_GET_SET | ACCESS_EXPLICIT_NOTIFICATION));
	registerField(specification, "texCoord", texCoord_, FieldAccessType(ACCESS_GET_SET | ACCESS_EXPLICIT_NOTIFICATION));
	registerField(specification, "ccw", ccw_, ACCESS_NONE);
	registerField(specification, "colorPerVertex", colorPerVertex_, ACCESS_NONE);
	registerField(specification, "normalPerVertex", normalPerVertex_, ACCESS_NONE);
	registerField(specification, "solid", solid_, ACCESS_NONE);

	X3DGeometryNode::registerFields(specification);
}

void X3DComposedGeometryNode::onInitialize(const Rendering::SceneRef& /*scene*/, const Timestamp /*timestamp*/)
{
	for (MultiNode::Values::const_iterator i = attrib_.values().begin(); i != attrib_.values().end(); ++i)
	{
		registerThisNodeAsParent(*i);
	}

	registerThisNodeAsParent(color_.value());
	registerThisNodeAsParent(coord_.value());
	registerThisNodeAsParent(fogCoord_.value());
	registerThisNodeAsParent(normal_.value());
	registerThisNodeAsParent(texCoord_.value());
}

}

}

}

}
