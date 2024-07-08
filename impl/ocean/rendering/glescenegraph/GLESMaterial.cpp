/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESMaterial.h"
#include "ocean/rendering/glescenegraph/GLESShaderProgram.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESMaterial::GLESMaterial() :
	GLESAttribute(),
	Material(),
	ambientColor_(0.2f, 0.2f, 0.2f),
	diffuseColor_(0.8f, 0.8f, 0.8f),
	emissiveColor_(0, 0, 0),
	specularColor_(0, 0, 0),
	specularExponent_(0.0f),
	transparency_(0.0f)
{
	// nothing to do here
}

GLESMaterial::~GLESMaterial()
{
	// nothing to do here
}

RGBAColor GLESMaterial::ambientColor() const
{
	const ScopedLock scopedLock(objectLock);
	return ambientColor_;
}

RGBAColor GLESMaterial::diffuseColor() const
{
	const ScopedLock scopedLock(objectLock);
	return diffuseColor_;
}

RGBAColor GLESMaterial::emissiveColor() const
{
	const ScopedLock scopedLock(objectLock);
	return emissiveColor_;
}

RGBAColor GLESMaterial::specularColor() const
{
	const ScopedLock scopedLock(objectLock);
	return specularColor_;
}

float GLESMaterial::specularExponent() const
{
	const ScopedLock scopedLock(objectLock);
	return specularExponent_;
}

float GLESMaterial::transparency() const
{
	const ScopedLock scopedLock(objectLock);
	return transparency_;
}

void GLESMaterial::get(RGBAColor& ambient, RGBAColor& diffuse, RGBAColor& emissive, RGBAColor& specular, float& specularExponent, float& transparency)
{
	const ScopedLock scopedLock(objectLock);

	ambient = ambientColor_;
	diffuse = diffuseColor_;
	emissive = emissiveColor_;
	specular = specularColor_;
	specularExponent = specularExponent_;
	transparency = transparency_;
}

bool GLESMaterial::setAmbientColor(const RGBAColor& color)
{
	if (!color.isValid())
	{
		return false;
	}

	const ScopedLock scopedLock(objectLock);

	ambientColor_ = color;
	return true;
}

bool GLESMaterial::setDiffuseColor(const RGBAColor& color)
{
	if (!color.isValid())
	{
		return false;
	}

	const ScopedLock scopedLock(objectLock);

	diffuseColor_ = color;
	return true;
}

bool GLESMaterial::setEmissiveColor(const RGBAColor& color)
{
	if (!color.isValid())
	{
		return false;
	}

	const ScopedLock scopedLock(objectLock);

	emissiveColor_ = color;
	return true;
}

bool GLESMaterial::setSpecularColor(const RGBAColor& color)
{
	if (!color.isValid())
	{
		return false;
	}

	const ScopedLock scopedLock(objectLock);

	specularColor_ = color;
	return true;
}

bool GLESMaterial::setSpecularExponent(const float specularExponent)
{
	if (specularExponent < 0.0f)
	{
		return false;
	}

	const ScopedLock scopedLock(objectLock);

	specularExponent_ = specularExponent;
	return true;
}

bool GLESMaterial::setTransparency(const float transparency)
{
	if (transparency < 0.0f || transparency > 1.0f)
	{
		return false;
	}

	const ScopedLock scopedLock(objectLock);

	transparency_ = transparency;
	return true;
}

bool GLESMaterial::set(const RGBAColor& ambient, const RGBAColor& diffuse, const RGBAColor& emissive, const RGBAColor& specular, const float specularExponent, const float transparency)
{
	const ScopedLock scopedLock(objectLock);

	bool result = false;

	if (ambient.isValid())
	{
		ambientColor_ = ambient;
		result = true;
	}

	if (diffuse.isValid())
	{
		diffuseColor_ = diffuse;
		result = true;
	}

	if (emissive.isValid())
	{
		emissiveColor_ = emissive;
		result = true;
	}

	if (specular.isValid())
	{
		specularColor_ = specular;
		result = true;
	}

	if (specularExponent >= 0.0f)
	{
		specularExponent_ = specularExponent;
		result = true;
	}

	if (transparency >= 0.0f && transparency <= 1.0f)
	{
		transparency_ = transparency;
		result = true;
	}

	return result;
}

void GLESMaterial::bindAttribute(const GLESFramebuffer& framebuffer, GLESShaderProgram& shaderProgram)
{
	bindMaterial(framebuffer, shaderProgram, "material");
}

void GLESMaterial::bindMaterial(const GLESFramebuffer& /*framebuffer*/, GLESShaderProgram& shaderProgram, const std::string& materialName)
{
	ocean_assert(!materialName.empty());

	const ScopedLock scopedLock(objectLock);

	ocean_assert(GL_NO_ERROR == glGetError());

	const GLint locationAmbientColor = glGetUniformLocation(shaderProgram.id(), (materialName + ".ambientColor").c_str());
	if (locationAmbientColor != -1)
	{
		setUniform(locationAmbientColor, ambientColor_);
	}

	const GLint locationDiffuseColor = glGetUniformLocation(shaderProgram.id(), (materialName + ".diffuseColor").c_str());
	if (locationDiffuseColor != -1)
	{
		ocean_assert(transparency_ >= 0.0f && transparency_ <= 1.0f);
		setUniform(locationDiffuseColor, RGBAColor(diffuseColor_.red(), diffuseColor_.green(), diffuseColor_.blue(), 1.0f - transparency_));
	}

	const GLint locationSpecularColor = glGetUniformLocation(shaderProgram.id(), (materialName + ".specularColor").c_str());
	if (locationSpecularColor != -1)
	{
		setUniform(locationSpecularColor, specularColor_);
	}

	const GLint locationEmissiveColor = glGetUniformLocation(shaderProgram.id(), (materialName + ".emissiveColor").c_str());
	if (locationEmissiveColor != -1)
	{
		setUniform(locationEmissiveColor, emissiveColor_);
	}

	const GLint locationSpecularExponent = glGetUniformLocation(shaderProgram.id(), (materialName + ".specularExponent").c_str());
	if (locationSpecularExponent != -1)
	{
		setUniform(locationSpecularExponent, specularExponent_);
	}
}

GLESAttribute::ProgramType GLESMaterial::necessaryShader() const
{
	return GLESAttribute::PT_MATERIAL;
}

}

}

}
