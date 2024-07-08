/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/SpotLight.h"

#include "ocean/rendering/Group.h"
#include "ocean/rendering/SpotLight.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

SpotLight::SpotLight(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DChildNode(environment),
	SDXUpdateNode(environment),
	X3DLightNode(environment),
	attenuation_(Vector3(1, 0, 0)),
	beamWidth_(Numeric::pi_2()),
	cutOffAngle_(Numeric::pi_4()),
	direction_(Vector3(0, 0, -1)),
	location_(Vector3(0, 0, 0)),
	radius_(100)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;

	renderingObject_ = engine()->factory().createSpotLight();
}

SpotLight::NodeSpecification SpotLight::specifyNode()
{
	NodeSpecification specification("SpotLight");

	registerField(specification, "attenuation", attenuation_, ACCESS_GET_SET);
	registerField(specification, "beamWidth", beamWidth_, ACCESS_GET_SET);
	registerField(specification, "cutOffAngle", cutOffAngle_, ACCESS_GET_SET);
	registerField(specification, "direction", direction_, ACCESS_GET_SET);
	registerField(specification, "location", location_, ACCESS_GET_SET);
	registerField(specification, "radius", radius_, ACCESS_GET_SET);

	X3DLightNode::registerFields(specification);

	return specification;
}

void SpotLight::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DLightNode::onInitialize(scene, timestamp);

	try
	{
		const Rendering::SpotLightRef renderingSpotLight(renderingObject_);

		if (renderingSpotLight)
		{
			renderingSpotLight->setAttenuation(attenuation_.value());

			const Scalar directionLength = direction_.value().length();

			if (Numeric::isNotEqualEps(directionLength))
			{
				renderingSpotLight->setDirection(direction_.value() / directionLength);
			}

			renderingSpotLight->setPosition(location_.value());

			const Scalar cutOffAngleValue = max(Scalar(0), min(Numeric::pi_2(), cutOffAngle_.value()));
			renderingSpotLight->setConeAngle(cutOffAngleValue);

			Scalar exponent = 0;
			if (cutOffAngle_.value() > Numeric::eps())
			{
				exponent = max(Scalar(0), min(Scalar(1), beamWidth_.value() / cutOffAngle_.value()));
			}

			// this spot exponent is not the correct mapping, however it's the only way to approximate the beamWidth behavior
			renderingSpotLight->setSpotExponent(exponent);
		}
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}

	if (radius_.timestamp().isInvalid() == false)
	{
		Log::warning() << "SpotLight::radius ist not supported";
	}
}

void SpotLight::onFieldChanged(const std::string& fieldName)
{
	try
	{
		const Rendering::SpotLightRef renderingSpotLight(renderingObject_);

		if (renderingSpotLight)
		{
			if (fieldName == "attenuation")
			{
				renderingSpotLight->setAttenuation(attenuation_.value());
			}
			if (fieldName == "direction")
			{
				const Scalar directionLength = direction_.value().length();

				if (Numeric::isNotEqualEps(directionLength))
				{
					renderingSpotLight->setDirection(direction_.value() / directionLength);
				}
			}
			if (fieldName == "location")
			{
				renderingSpotLight->setPosition(location_.value());
			}

			if (fieldName == "cutOffAngle")
			{
				const Scalar cutOffAngleValue = max(Scalar(0), min(Numeric::pi_2(), cutOffAngle_.value()));
				renderingSpotLight->setConeAngle(cutOffAngleValue);
			}

			if (fieldName == "beamWidth" || fieldName == "cutOffAngle")
			{
				Scalar exponent = 0;
				if (cutOffAngle_.value() > Numeric::eps())
				{
					exponent = 128 * (1 - max(Scalar(0), min(Scalar(1), beamWidth_.value() / cutOffAngle_.value())));
				}

				// this spot exponent is not the correct mapping, however it's the only way to approximate the beamWidth behavior
				renderingSpotLight->setSpotExponent(exponent);
			}
		}
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}

	if (fieldName == "radius")
	{
		Log::warning() << "SpotLight::radius ist not supported";
	}

	X3DLightNode::onFieldChanged(fieldName);
}

void SpotLight::onGlobalLight(const HomogenousMatrix4& world_T_light)
{
	ocean_assert(world_T_light.isValid());

	const Rendering::SpotLightRef renderingSpotLight(renderingObject_);

	if (renderingSpotLight)
	{
		const Scalar directionLength = direction_.value().length();

		if (Numeric::isNotEqualEps(directionLength))
		{
			const Vector3 normalizedDirection(direction_.value() / directionLength);
			renderingSpotLight->setDirection(world_T_light.rotation() * normalizedDirection);
			renderingSpotLight->setPosition(world_T_light * location_.value());
		}
	}
}

size_t SpotLight::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
