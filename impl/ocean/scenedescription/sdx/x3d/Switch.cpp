/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/Switch.h"

#include "ocean/rendering/Switch.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

Switch::Switch(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DChildNode(environment),
	X3DBoundedObject(environment),
	X3DGroupingNode(environment),
	whichChoice_(-1)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;

	renderingObject_ = engine()->factory().createSwitch();
}

Switch::NodeSpecification Switch::specifyNode()
{
	NodeSpecification specification("Switch");

	registerField(specification, "whichChoice", whichChoice_, ACCESS_GET_SET);

	// VMRL 97 compatibility:
	registerField(specification, "choice", children_, FieldAccessType(ACCESS_GET_SET | ACCESS_EXPLICIT_NOTIFICATION));

	X3DGroupingNode::registerFields(specification);

	return specification;
}

void Switch::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DGroupingNode::onInitialize(scene, timestamp);

	try
	{
		Rendering::SwitchRef renderingSwitch(renderingObject_);
		if (renderingSwitch)
		{
			Index32 index = Rendering::Switch::invalidIndex;

			if (whichChoice_.value() >= 0 && whichChoice_.value() < int(children_.values().size()))
			{
				index = Index32(whichChoice_.value());
			}

			renderingSwitch->setActiveNode(index);
		}
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}
}

bool Switch::onFieldChanging(const std::string& fieldName, const Field& field)
{
	if (fieldName == "choice")
	{
		X3DGroupingNode::onFieldChanging("children", field);
	}

	return X3DGroupingNode::onFieldChanging(fieldName, field);
}

void Switch::onFieldChanged(const std::string& fieldName)
{
	try
	{
		Rendering::SwitchRef renderingSwitch(renderingObject_);
		if (renderingSwitch)
		{
			if (fieldName == "whichChoice")
			{
				Index32 index = Rendering::Switch::invalidIndex;

				if (whichChoice_.value() >= 0 && whichChoice_.value() < int(children_.values().size()))
				{
					index = Index32(whichChoice_.value());
				}

				renderingSwitch->setActiveNode(index);
			}
		}
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}

	X3DGroupingNode::onFieldChanged(fieldName);
}

size_t Switch::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
