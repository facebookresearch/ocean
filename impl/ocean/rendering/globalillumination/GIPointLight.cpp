/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/globalillumination/GIPointLight.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

GIPointLight::GIPointLight() :
	lightPosition(0, 0, 0),
	lightAttenuationFactors(0, 0, 0)
{
	// nothing to do here
}

GIPointLight::~GIPointLight()
{
	// nothing to do here
}

Vector3 GIPointLight::position() const
{
	return lightPosition;
}

Vector3 GIPointLight::attenuation() const
{
	return lightAttenuationFactors;
}

void GIPointLight::setPosition(const Vector3& position)
{
	const ScopedLock scopedLock(objectLock);
	lightPosition = position;
}

bool GIPointLight::setAttenuation(const Vector3& factors)
{
	if (factors.x() < 0 || factors.y() < 0 || factors.z() < 0)
		return false;

	const ScopedLock scopedLock(objectLock);

	lightAttenuationFactors = factors;
	return true;
}

}

}

}
