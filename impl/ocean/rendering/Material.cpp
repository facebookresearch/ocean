/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/Material.h"

#include "ocean/base/Exception.h"

namespace Ocean
{

namespace Rendering
{

Material::Material() :
	Attribute()
{
	// nothing to do here
}

Material::~Material()
{
	// nothing to do here
}

RGBAColor Material::ambientColor() const
{
	throw NotSupportedException("Material::ambientColor() is not supported.");
}

RGBAColor Material::diffuseColor() const
{
	throw NotSupportedException("Material::diffuseColor() is not supported.");
}

RGBAColor Material::emissiveColor() const
{
	throw NotSupportedException("Material::emissiveColor() is not supported.");
}

RGBAColor Material::specularColor() const
{
	throw NotSupportedException("Material::specularColor() is not supported.");
}

float Material::transparency() const
{
	throw NotSupportedException("Material::transparency() is not supported.");
}

float Material::specularExponent() const
{
	throw NotSupportedException("Material::specularExponent() is not supported.");
}

float Material::reflectivity() const
{
	throw NotSupportedException("Material::reflectivity() is not supported.");
}

float Material::refractionIndex() const
{
	throw NotSupportedException("Material::refractionIndex() is not supported.");
}

void Material::get(RGBAColor& /*ambient*/, RGBAColor& /*diffuse*/, RGBAColor& /*emissive*/, RGBAColor& /*specular*/, float& /*specularExponent*/, float& /*transparency*/)
{
	throw NotSupportedException("Material::get() is not supported.");
}

bool Material::setAmbientColor(const RGBAColor& /*color*/)
{
	throw NotSupportedException("Material::setAmbientColor() is not supported.");
}

bool Material::setDiffuseColor(const RGBAColor& /*color*/)
{
	throw NotSupportedException("Material::setDiffuseColor() is not supported.");
}

bool Material::setEmissiveColor(const RGBAColor& /*color*/)
{
	throw NotSupportedException("Material::setEmissiveColor() is not supported.");
}

bool Material::setSpecularColor(const RGBAColor& /*color*/)
{
	throw NotSupportedException("Material::setSpecularColor() is not supported.");
}

bool Material::setSpecularExponent(const float /*specularExponent*/)
{
	throw NotSupportedException("Material::setSpecularExponent() is not supported.");
}

bool Material::setTransparency(const float /*transparency*/)
{
	throw NotSupportedException("Material::setTransparency() is not supported.");
}

bool Material::setReflectivity(const float /*reflectivity*/)
{
	throw NotSupportedException("Material::setReflectivity() is not supported.");
}

bool Material::setRefractionIndex(const float /*index*/)
{
	throw NotSupportedException("Material::setRefractionIndex() is not supported.");
}

bool Material::set(const RGBAColor& /*ambient*/, const RGBAColor& /*diffuse*/, const RGBAColor& /*emissive*/, const RGBAColor& /*specular*/, const float /*specularExponent*/, const float /*transparency*/)
{
	throw NotSupportedException("Material::set() is not supported.");
}

Material::ObjectType Material::type() const
{
	return TYPE_MATERIAL;
}

}

}
