/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/StaticGroup.h"

#include "ocean/rendering/Group.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

StaticGroup::StaticGroup(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DChildNode(environment),
	X3DBoundedObject(environment)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;

	renderingObject_ = engine()->factory().createGroup();
}

StaticGroup::~StaticGroup()
{
	if (initialized_)
	{
		for (MultiNode::Values::const_iterator i = children_.values().begin(); i != children_.values().end(); ++i)
		{
			unregisterThisNodeAsParent(*i);
		}
	}
}

StaticGroup::NodeSpecification StaticGroup::specifyNode()
{
	NodeSpecification specification("StaticGroup");

	registerField(specification, "children", children_);

	X3DChildNode::registerFields(specification);
	X3DBoundedObject::registerFields(specification);

	return specification;
}

void StaticGroup::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DBoundedObject::onInitialize(scene, timestamp);
	X3DChildNode::onInitialize(scene, timestamp);

	try
	{
		Rendering::GroupRef renderingGroup(renderingObject_);
		if (renderingGroup)
		{
			for (MultiNode::Values::const_iterator i = children_.values().begin(); i != children_.values().end(); ++i)
			{
				const SDXNodeRef childNode(*i);

				registerThisNodeAsParent(childNode);
				childNode->initialize(scene, timestamp);

				renderingGroup->addChild(childNode->renderingObject());
			}
		}
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}
}

size_t StaticGroup::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
