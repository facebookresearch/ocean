/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/LightSource.h"

namespace Ocean
{

namespace Rendering
{

LightSource::LightSource()
{
	// nothing to do here
}

LightSource::~LightSource()
{
	// nothing to do here
}

RGBAColor LightSource::ambientColor() const
{
	throw NotSupportedException("Ambient color is not supported.");
}

RGBAColor LightSource::diffuseColor() const
{
	throw NotSupportedException("Diffuse color is not supported.");
}

RGBAColor LightSource::specularColor() const
{
	throw NotSupportedException("Specular color is not supported.");
}

Scalar LightSource::intensity() const
{
	throw NotSupportedException("Intensity is not supported.");
}

LightSource::TransformationType LightSource::transformationType() const
{
	throw NotSupportedException("PointLight::transformationType() is not supported.");
}

bool LightSource::enabled() const
{
	throw NotSupportedException("Enabled is not supported.");
}

void LightSource::get(RGBAColor& /*ambient*/, RGBAColor& /*diffuse*/, RGBAColor& /*specular*/, Scalar& /*intensity*/)
{
	throw NotSupportedException("LightSource::get() is not supported.");
}

bool LightSource::setAmbientColor(const RGBAColor& /*color*/)
{
	throw NotSupportedException("LightSource::setAmbientColor is not supported.");
}

bool LightSource::setDiffuseColor(const RGBAColor& /*color*/)
{
	throw NotSupportedException("LightSource::setDiffuseColor is not supported.");
}

bool LightSource::setSpecularColor(const RGBAColor& /*color*/)
{
	throw NotSupportedException("LightSource::setSpecularColor is not supported.");
}

bool LightSource::setIntensity(const Scalar /*intensity*/)
{
	throw NotSupportedException("LightSource::setIntensity is not supported.");
}

bool LightSource::set(const RGBAColor& /*ambient*/, const RGBAColor& /*diffuse*/, const RGBAColor& /*specular*/, const Scalar /*intensity*/)
{
	throw NotSupportedException("LightSource::set() is not supported.");
}

void LightSource::setTransformationType(const TransformationType /*transformationType*/)
{
	throw NotSupportedException("PointLight::setTransformationType() is not supported.");
}

void LightSource::setEnabled(const bool /*state*/)
{
	throw NotSupportedException("LightSource::setEnabled() is not supported.");
}

LightSource::ObjectType LightSource::type() const
{
	return TYPE_LIGHT_SOURCE;
}

}

}
