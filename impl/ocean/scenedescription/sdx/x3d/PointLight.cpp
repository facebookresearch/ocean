/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/PointLight.h"

#include "ocean/rendering/Group.h"
#include "ocean/rendering/PointLight.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

PointLight::PointLight(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DChildNode(environment),
	SDXUpdateNode(environment),
	X3DLightNode(environment),
	attenuation_(Vector3(1, 0, 0)),
	location_(Vector3(0, 0, 0)),
	radius_(100)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;

	renderingObject_ = engine()->factory().createPointLight();
}

PointLight::NodeSpecification PointLight::specifyNode()
{
	NodeSpecification specification("PointLight");

	registerField(specification, "attenuation", attenuation_, ACCESS_GET_SET);
	registerField(specification, "location", location_, ACCESS_GET_SET);
	registerField(specification, "radius", radius_, ACCESS_GET_SET);

	X3DLightNode::registerFields(specification);

	return specification;
}

void PointLight::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DLightNode::onInitialize(scene, timestamp);

	try
	{
		const Rendering::PointLightRef renderingPointLight(renderingObject_);

		if (renderingPointLight)
		{
			renderingPointLight->setAttenuation(attenuation_.value());
			renderingPointLight->setPosition(location_.value());
		}
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}

	if (radius_.timestamp().isValid())
	{
		Log::warning() << "PointLight::radius is not supported.";
	}
}

void PointLight::onFieldChanged(const std::string& fieldName)
{
	try
	{
		const Rendering::PointLightRef renderingPointLight(renderingObject_);

		if (renderingPointLight)
		{
			if (fieldName == "attenuation")
			{
				renderingPointLight->setAttenuation(attenuation_.value());
			}
			if (fieldName == "location")
			{
				renderingPointLight->setPosition(location_.value());
			}
		}
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}

	if (fieldName == "radius")
	{
		Log::warning() << "PointLight::radius ist not supported";
	}

	X3DLightNode::onFieldChanged(fieldName);
}

void PointLight::onGlobalLight(const HomogenousMatrix4& world_T_light)
{
	ocean_assert(world_T_light.isValid());

	const Rendering::PointLightRef renderingPointLight(renderingObject_);

	if (renderingPointLight)
	{
		renderingPointLight->setPosition(world_T_light * location_.value());
	}
}

size_t PointLight::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
