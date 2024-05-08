/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/DirectionalLight.h"

#include "ocean/rendering/DirectionalLight.h"
#include "ocean/rendering/Group.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

DirectionalLight::DirectionalLight(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DChildNode(environment),
	SDXUpdateNode(environment),
	X3DLightNode(environment),
	direction_(Vector3(0, 0, -1))
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;

	renderingObject_ = engine()->factory().createDirectionalLight();
}

DirectionalLight::NodeSpecification DirectionalLight::specifyNode()
{
	NodeSpecification specification("DirectionalLight");

	registerField(specification, "direction", direction_, ACCESS_GET_SET);

	X3DLightNode::registerFields(specification);

	return specification;
}

void DirectionalLight::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DLightNode::onInitialize(scene, timestamp);

	try
	{
		const Rendering::DirectionalLightRef renderingDirectionalLight(renderingObject_);

		if (renderingDirectionalLight)
		{
			const Scalar directionLength = direction_.value().length();

			if (Numeric::isNotEqualEps(directionLength))
			{
				renderingDirectionalLight->setDirection(direction_.value() / directionLength);
			}
		}
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}
}

void DirectionalLight::onFieldChanged(const std::string& fieldName)
{
	try
	{
		const Rendering::DirectionalLightRef renderingDirectionalLight(renderingObject_);

		if (renderingDirectionalLight)
		{
			if (fieldName == "direction")
			{
				const Scalar directionLength = direction_.value().length();

				if (Numeric::isNotEqualEps(directionLength))
				{
					renderingDirectionalLight->setDirection(direction_.value() / directionLength);
				}
			}
		}
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}

	X3DLightNode::onFieldChanged(fieldName);
}

void DirectionalLight::onGlobalLight(const HomogenousMatrix4& world_T_light)
{
	ocean_assert(world_T_light.isValid());

	const Rendering::DirectionalLightRef renderingDirectionalLight(renderingObject_);

	if (renderingDirectionalLight)
	{
		const Scalar directionLength = direction_.value().length();

		if (Numeric::isNotEqualEps(directionLength))
		{
			const Vector3 normalizedDirection(direction_.value() / directionLength);
			renderingDirectionalLight->setDirection(world_T_light.rotation() * normalizedDirection);
		}
	}
}

size_t DirectionalLight::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
