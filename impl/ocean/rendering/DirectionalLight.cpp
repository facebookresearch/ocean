// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/rendering/DirectionalLight.h"

namespace Ocean
{

namespace Rendering
{

DirectionalLight::DirectionalLight() :
	LightSource()
{
	// nothing to do here
}

DirectionalLight::~DirectionalLight()
{
	// nothing to do here
}

Vector3 DirectionalLight::direction() const
{
	throw NotSupportedException("Directional light direction is not supported.");
}

void DirectionalLight::setDirection(const Vector3& /*direction*/)
{
	throw NotSupportedException("Direction light direction is not supported.");
}

DirectionalLight::ObjectType DirectionalLight::type() const
{
	return TYPE_DIRECTIONAL_LIGHT;
}

}

}
