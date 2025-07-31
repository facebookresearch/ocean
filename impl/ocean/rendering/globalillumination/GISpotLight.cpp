/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/globalillumination/GISpotLight.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

GISpotLight::GISpotLight() :
	GIPointLight(),
	SpotLight(),
	lightDirection(Vector3(0, 0, -1)),
	lightConeAngle(Numeric::deg2rad(45)),
	lightConeAngleCos(Numeric::cos(Numeric::deg2rad(45))),
	lightSpotExponent(0)
{
	// nothing to do here
}

GISpotLight::~GISpotLight()
{
	// nothing to do here
}

Vector3 GISpotLight::direction() const
{
	return lightDirection;
}

Scalar GISpotLight::coneAngle() const
{
	return lightConeAngle;
}

Scalar GISpotLight::spotExponent() const
{
	return lightSpotExponent;
}

void GISpotLight::setDirection(const Vector3& direction)
{
	const ScopedLock scopedLock(objectLock);

	ocean_assert(Numeric::isEqual(direction.length(), 1));
	lightDirection = direction;
}

bool GISpotLight::setConeAngle(const Scalar angle)
{
	if (angle < 0 || angle > Numeric::pi_2())
		return false;

	const ScopedLock scopedLock(objectLock);
	lightConeAngle = angle;
	lightConeAngleCos = Numeric::cos(lightConeAngle);

	return true;
}

bool GISpotLight::setSpotExponent(const Scalar exponent)
{
	if (exponent < 0 || exponent > 1)
		return false;

	const ScopedLock scopedLock(objectLock);
	lightSpotExponent = exponent;

	return true;
}

}

}

}
