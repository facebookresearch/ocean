/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/HeadUpTransform.h"

#include "ocean/rendering/AbsoluteTransform.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

HeadUpTransform::HeadUpTransform(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DChildNode(environment),
	X3DBoundedObject(environment),
	X3DGroupingNode(environment),
	screenPosition_(Vector2(-1, -1))
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;

	renderingObject_ = engine()->factory().createAbsoluteTransform();
	if (renderingObject_)
	{
		const Rendering::AbsoluteTransformRef absoluteTransform(renderingObject_);
		ocean_assert(absoluteTransform);

		absoluteTransform->setTransformationType(Rendering::AbsoluteTransform::TT_HEAD_UP);
	}
}

HeadUpTransform::~HeadUpTransform()
{
	// nothing to do here
}

HeadUpTransform::NodeSpecification HeadUpTransform::specifyNode()
{
	NodeSpecification specification("HeadUpTransform");

	registerField(specification, "screenPosition", screenPosition_, ACCESS_GET_SET);

	X3DGroupingNode::registerFields(specification);

	return specification;
}

void HeadUpTransform::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DGroupingNode::onInitialize(scene, timestamp);

	try
	{
		const Rendering::AbsoluteTransformRef absoluteTransform(renderingObject_);
		if (absoluteTransform)
		{
			absoluteTransform->setHeadUpRelativePosition(screenPosition_.value());
		}
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}
}

void HeadUpTransform::onFieldChanged(const std::string& fieldName)
{
	try
	{
		const Rendering::AbsoluteTransformRef absoluteTransform(renderingObject_);

		if (fieldName == "screenPosition")
		{
			if (absoluteTransform)
			{
				absoluteTransform->setHeadUpRelativePosition(screenPosition_.value());
				return;
			}
		}
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}

	X3DGroupingNode::onFieldChanged(fieldName);
}

size_t HeadUpTransform::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
