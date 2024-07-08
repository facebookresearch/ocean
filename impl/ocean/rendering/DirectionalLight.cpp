/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
