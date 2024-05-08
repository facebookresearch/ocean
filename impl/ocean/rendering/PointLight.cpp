/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/PointLight.h"

namespace Ocean
{

namespace Rendering
{

PointLight::PointLight() :
	LightSource()
{
	// nothing to do here
}

PointLight::~PointLight()
{
	// nothing to do here
}

Vector3 PointLight::position() const
{
	throw NotSupportedException("PointLight::position() is not supported.");
}

Vector3 PointLight::attenuation() const
{
	throw NotSupportedException("PointLight::attenuation() is not supported.");
}

void PointLight::setPosition(const Vector3& /*position*/)
{
	throw NotSupportedException("PointLight::setPosition() is not supported.");
}

bool PointLight::setAttenuation(const Vector3& /*factors*/)
{
	throw NotSupportedException("PointLight::setAttenuation() is not supported.");
}

PointLight::ObjectType PointLight::type() const
{
	return TYPE_POINT_LIGHT;
}

}

}
