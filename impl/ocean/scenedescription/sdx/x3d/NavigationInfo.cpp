/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/NavigationInfo.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

NavigationInfo::NavigationInfo(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
    X3DChildNode(environment),
	X3DBindableNode(environment),
	avatarSize_({Scalar(0.25f), Scalar(1.6f), Scalar(0.75f)}),
	headlight_(true),
	speed_(1),
	transitionTime_(Timestamp(1.0)),
	transitionType_("LINEAR"),
	type_(std::vector<std::string>({"EXAMINE", "ANY"})),
	visibilityLimit_(0)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;
}

NavigationInfo::NodeSpecification NavigationInfo::specifyNode()
{
	NodeSpecification specification("NavigationInfo");

	registerField(specification, "avatarSize", avatarSize_, ACCESS_GET_SET);
	registerField(specification, "headlight", headlight_, ACCESS_GET_SET);
	registerField(specification, "speed", speed_, ACCESS_GET_SET);
	registerField(specification, "transitionTime", transitionTime_, ACCESS_GET_SET);
	registerField(specification, "transitionType", transitionType_, ACCESS_GET_SET);
	registerField(specification, "type", type_, ACCESS_GET_SET);
	registerField(specification, "visibilityLimit", visibilityLimit_, ACCESS_GET_SET);
    registerField(specification, "transitionComplete", transitionComplete_, ACCESS_GET);

	X3DBindableNode::registerFields(specification);

	return specification;
}

void NavigationInfo::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DBindableNode::onInitialize(scene, timestamp);

	try
	{
        scene->setUseHeadlight(headlight_.value());
    }
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}
}

void NavigationInfo::onFieldChanged(const std::string& fieldName)
{
	try
	{
        Log::warning() << "NavigationInfo does not allow to change any field.";
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}

	X3DBindableNode::onFieldChanged(fieldName);
}

size_t NavigationInfo::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
