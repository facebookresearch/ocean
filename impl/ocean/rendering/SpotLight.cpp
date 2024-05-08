/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/SpotLight.h"

namespace Ocean
{

namespace Rendering
{

SpotLight::SpotLight() :
	LightSource()
{
	// nothing to do here
}

SpotLight::~SpotLight()
{
	// nothing to do here
}

Vector3 SpotLight::direction() const
{
	throw NotSupportedException("SpotLight::direction() is not supported.");
}

Scalar SpotLight::coneAngle() const
{
	throw NotSupportedException("SpotLight::coneAngle() is not supported.");
}

Scalar SpotLight::spotExponent() const
{
	throw NotSupportedException("SpotLight::spotExponent() is not supported.");
}

void SpotLight::setDirection(const Vector3& /*direction*/)
{
	throw NotSupportedException("SpotLight::setDirection() is not supported.");
}

bool SpotLight::setConeAngle(const Scalar /*angle*/)
{
	throw NotSupportedException("SpotLight::setConeAngle() is not supported.");
}

bool SpotLight::setSpotExponent(const Scalar /*exponent*/)
{
	throw NotSupportedException("SpotLight::setSpotExponent is not supported.");
}

SpotLight::ObjectType SpotLight::type() const
{
	return TYPE_SPOT_LIGHT;
}

}

}
