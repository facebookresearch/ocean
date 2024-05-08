/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/LOD.h"

#include "ocean/rendering/LOD.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

LOD::LOD(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DChildNode(environment),
	X3DBoundedObject(environment),
	X3DGroupingNode(environment),
	levelChanged_(0),
	center_(Vector3(0, 0, 0)),
	forceTransitions_(false)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;

	renderingObject_ = engine()->factory().createLOD();
}

LOD::NodeSpecification LOD::specifyNode()
{
	NodeSpecification specification("LOD");

	registerField(specification, "level_changed", levelChanged_, ACCESS_GET);
	registerField(specification, "center", center_, ACCESS_NONE);
	registerField(specification, "forceTransitions", forceTransitions_, ACCESS_NONE);
	registerField(specification, "range", range_, ACCESS_NONE);

	X3DGroupingNode::registerFields(specification);

	return specification;
}

void LOD::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DGroupingNode::onInitialize(scene, timestamp);

	try
	{
		Rendering::LODRef renderingLOD(renderingObject_);
		if (renderingLOD)
		{
			renderingLOD->setDistanceRanges(range_.values());
		}
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}

	if (center_.timestamp().isInvalid() == false)
	{
		Log::warning() << "LOD::center is not supported.";
	}
	if (forceTransitions_.timestamp().isInvalid() == false)
	{
		Log::warning() << "LOD::forceTransitions is not supported.";
	}
}

void LOD::onFieldChanged(const std::string& fieldName)
{
	if (fieldName == "center")
	{
		Log::warning() << "LOD::center is not supported.";
	}
	if (fieldName == "forceTransitions")
	{
		Log::warning() << "LOD::forceTransitions is not supported.";
	}

	X3DGroupingNode::onFieldChanged(fieldName);
}

size_t LOD::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
