/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/globalillumination/GILightSource.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

GILightSource::GILightSource() :
	GIObject(),
	LightSource(),
	lightAmbientColor(0, 0, 0),
	lightDiffuseColor(1, 1, 1),
	lightSpecularColor(1, 1, 1),
	lightIntensity(1),
	lightEnabled(true)
{
	//  nothing to do here
}

GILightSource::~GILightSource()
{
	//  nothing to do here
}

RGBAColor GILightSource::ambientColor() const
{
	const ScopedLock scopedLock(objectLock);
	return lightAmbientColor;
}

RGBAColor GILightSource::diffuseColor() const
{
	const ScopedLock scopedLock(objectLock);
	return lightDiffuseColor;
}

RGBAColor GILightSource::specularColor() const
{
	const ScopedLock scopedLock(objectLock);
	return lightSpecularColor;
}

Scalar GILightSource::intensity() const
{
	const ScopedLock scopedLock(objectLock);
	return lightIntensity;
}

bool GILightSource::enabled() const
{
	return lightEnabled;
}

void GILightSource::get(RGBAColor& ambient, RGBAColor& diffuse, RGBAColor& specular, Scalar& intensity)
{
	const ScopedLock scopedLock(objectLock);

	ambient = lightAmbientColor;
	diffuse = lightDiffuseColor;
	specular = lightSpecularColor;
	intensity = lightIntensity;
}

bool GILightSource::setAmbientColor(const RGBAColor& color)
{
	if (!color.isValid())
		return false;

	const ScopedLock scopedLock(objectLock);

	lightAmbientColor = color;
	return true;
}

bool GILightSource::setDiffuseColor(const RGBAColor& color)
{
	if (!color.isValid())
		return false;

	const ScopedLock scopedLock(objectLock);

	lightDiffuseColor = color;
	return true;
}

bool GILightSource::setSpecularColor(const RGBAColor& color)
{
	if (!color.isValid())
		return false;

	const ScopedLock scopedLock(objectLock);

	lightSpecularColor = color;
	return true;
}

bool GILightSource::setIntensity(const Scalar intensity)
{
	if (intensity < 0 || intensity > 1)
		return false;

	const ScopedLock scopedLock(objectLock);

	lightIntensity = intensity;
	return true;
}

bool GILightSource::set(const RGBAColor& ambient, const RGBAColor& diffuse, const RGBAColor& specular, const Scalar intensity)
{
	const ScopedLock scopedLock(objectLock);

	bool result = false;

	if (ambient.isValid())
	{
		lightAmbientColor = ambient;
		result = true;
	}

	if (diffuse.isValid())
	{
		lightDiffuseColor = diffuse;
		result = true;
	}

	if (specular.isValid())
	{
		lightSpecularColor = specular;
		result = true;
	}

	if (intensity >= 0 && intensity <= 1)
	{
		lightIntensity = intensity;
		result = true;
	}

	return result;
}

void GILightSource::setEnabled(const bool state)
{
	lightEnabled = state;
}

}

}

}
