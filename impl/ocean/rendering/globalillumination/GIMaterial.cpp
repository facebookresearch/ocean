/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/globalillumination/GIMaterial.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

GIMaterial::GIMaterial() :
	GIAttribute(),
	Material(),
	materialAmbientColor(0.2f, 0.2f, 0.2f),
	materialDiffuseColor(0.8f, 0.8f, 0.8f),
	materialEmissiveColor(0, 0, 0),
	materialSpecularColor(0, 0, 0),
	materialSpecularExponent(0),
	materialTransparency(0),
	materialReflectivity(0),
	materialRefractionIndex(1)
{
	// nothing to do here
}

GIMaterial::~GIMaterial()
{
	// nothing to do here
}

RGBAColor GIMaterial::ambientColor() const
{
	return materialAmbientColor;
}

RGBAColor GIMaterial::diffuseColor() const
{
	return materialDiffuseColor;
}

RGBAColor GIMaterial::emissiveColor() const
{
	return materialEmissiveColor;
}

RGBAColor GIMaterial::specularColor() const
{
	return materialSpecularColor;
}

float GIMaterial::specularExponent() const
{
	return materialSpecularExponent;
}

float GIMaterial::transparency() const
{
	return materialTransparency;
}

float GIMaterial::reflectivity() const
{
	return materialReflectivity;
}

float GIMaterial::refractionIndex() const
{
	return materialRefractionIndex;
}

void GIMaterial::get(RGBAColor& ambient, RGBAColor& diffuse, RGBAColor& emissive, RGBAColor& specular, float& specularExponent, float& transparency)
{
	const ScopedLock scopedLock(objectLock);

	ambient = materialAmbientColor;
	diffuse = materialDiffuseColor;
	emissive = materialEmissiveColor;
	specular = materialSpecularColor;
	specularExponent = materialSpecularExponent;
	transparency = materialTransparency;
}

bool GIMaterial::setAmbientColor(const RGBAColor& color)
{
	if (!color.isValid())
		return false;

	const ScopedLock scopedLock(objectLock);

	materialAmbientColor = color;
	return true;
}

bool GIMaterial::setDiffuseColor(const RGBAColor& color)
{
	if (!color.isValid())
		return false;

	const ScopedLock scopedLock(objectLock);

	materialDiffuseColor = color;
	return true;
}

bool GIMaterial::setEmissiveColor(const RGBAColor& color)
{
	if (!color.isValid())
		return false;

	const ScopedLock scopedLock(objectLock);

	materialEmissiveColor = color;
	return true;
}

bool GIMaterial::setSpecularColor(const RGBAColor& color)
{
	if (!color.isValid())
		return false;

	const ScopedLock scopedLock(objectLock);

	materialSpecularColor = color;
	return true;
}

bool GIMaterial::setSpecularExponent(const float specularExponent)
{
	if (specularExponent < 0)
		return false;

	const ScopedLock scopedLock(objectLock);

	materialSpecularExponent = specularExponent;
	return true;
}

bool GIMaterial::setTransparency(const float transparency)
{
	if (transparency < 0 || transparency > 1)
		return false;

	const ScopedLock scopedLock(objectLock);

	materialTransparency = transparency;
	return true;
}

bool GIMaterial::setReflectivity(const float reflectivity)
{
	if (reflectivity < 0 || reflectivity > 1)
		return false;

	const ScopedLock scopedLock(objectLock);

	materialReflectivity = reflectivity;
	return true;
}

bool GIMaterial::setRefractionIndex(const float index)
{
	if (index < 0)
		return false;

	const ScopedLock scopedLock(objectLock);

	materialRefractionIndex = index;
	return true;
}

bool GIMaterial::set(const RGBAColor& ambient, const RGBAColor& diffuse, const RGBAColor& emissive, const RGBAColor& specular, const float specularExponent, const float transparency)
{
	const ScopedLock scopedLock(objectLock);

	bool result = false;

	if (ambient.isValid())
	{
		materialAmbientColor = ambient;
		result = true;
	}

	if (diffuse.isValid())
	{
		materialDiffuseColor = diffuse;
		result = true;
	}

	if (emissive.isValid())
	{
		materialEmissiveColor = emissive;
		result = true;
	}

	if (specular.isValid())
	{
		materialSpecularColor = specular;
		result = true;
	}

	if (specularExponent >= 0)
	{
		materialSpecularExponent = specularExponent;
		result = true;
	}

	if (transparency >= 0 && transparency <= 1)
	{
		materialTransparency = transparency;
		result = true;
	}

	return result;
}

}

}

}
