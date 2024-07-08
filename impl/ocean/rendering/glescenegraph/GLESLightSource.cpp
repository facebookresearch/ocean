/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESLightSource.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESLightSource::GLESLightSource() :
	GLESObject(),
	LightSource(),
	ambientColor_(0, 0, 0),
	diffuseColor_(1, 1, 1),
	specularColor_(1, 1, 1),
	intensity_(1),
	transformationType_(TT_PARENT),
	enabled_(true)
{
	//  nothing to do here
}

GLESLightSource::~GLESLightSource()
{
	//  nothing to do here
}

RGBAColor GLESLightSource::ambientColor() const
{
	const ScopedLock scopedLock(objectLock);
	return ambientColor_;
}

RGBAColor GLESLightSource::diffuseColor() const
{
	const ScopedLock scopedLock(objectLock);
	return diffuseColor_;
}

RGBAColor GLESLightSource::specularColor() const
{
	const ScopedLock scopedLock(objectLock);
	return specularColor_;
}

Scalar GLESLightSource::intensity() const
{
	const ScopedLock scopedLock(objectLock);
	return intensity_;
}

GLESLightSource::TransformationType GLESLightSource::transformationType() const
{
	return transformationType_;
}

bool GLESLightSource::enabled() const
{
	return enabled_;
}

void GLESLightSource::get(RGBAColor& ambient, RGBAColor& diffuse, RGBAColor& specular, Scalar& intensity)
{
	const ScopedLock scopedLock(objectLock);

	ambient = ambientColor_;
	diffuse = diffuseColor_;
	specular = specularColor_;
	intensity = intensity_;
}

bool GLESLightSource::setAmbientColor(const RGBAColor& color)
{
	if (!color.isValid())
	{
		return false;
	}

	const ScopedLock scopedLock(objectLock);

	ambientColor_ = color;
	return true;
}

bool GLESLightSource::setDiffuseColor(const RGBAColor& color)
{
	if (!color.isValid())
	{
		return false;
	}

	const ScopedLock scopedLock(objectLock);

	diffuseColor_ = color;
	return true;
}

bool GLESLightSource::setSpecularColor(const RGBAColor& color)
{
	if (!color.isValid())
	{
		return false;
	}

	const ScopedLock scopedLock(objectLock);

	specularColor_ = color;
	return true;
}

bool GLESLightSource::setIntensity(const Scalar intensity)
{
	if (intensity < 0 || intensity > 1)
	{
		return false;
	}

	const ScopedLock scopedLock(objectLock);

	intensity_ = intensity;
	return true;
}

void GLESLightSource::setTransformationType(const TransformationType transformationType)
{
	const ScopedLock scopedLock(objectLock);

	transformationType_ = transformationType;
}

bool GLESLightSource::set(const RGBAColor& ambient, const RGBAColor& diffuse, const RGBAColor& specular, const Scalar intensity)
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

	if (specular.isValid())
	{
		specularColor_ = specular;
		result = true;
	}

	if (intensity >= 0 && intensity <= 1)
	{
		intensity_ = intensity;
		result = true;
	}

	return result;
}

void GLESLightSource::setEnabled(const bool state)
{
	enabled_ = state;
}

void GLESLightSource::bindLight(GLESShaderProgram& shaderProgram, const HomogenousMatrix4& /*camera_T_model*/, const HomogenousMatrix4& /*camera_T_world*/, const SquareMatrix3& /*normalMatrix*/, const unsigned int index)
{
	ocean_assert(shaderProgram.isCompiled());
	ocean_assert(index < 8u);

	const ScopedLock scopedLock(objectLock);

	ocean_assert(enabled_);

	char ambientString[] = "lights[~].ambientColor";
	char diffuseString[] = "lights[~].diffuseColor";
	char specularString[] = "lights[~].specularColor";

	ocean_assert(ambientString[7] == '~');
	ambientString[7] = char('0' + index);

	ocean_assert(diffuseString[7] == '~');
	diffuseString[7] = char('0' + index);

	ocean_assert(specularString[7] == '~');
	specularString[7] = char('0' + index);

	ocean_assert(GL_NO_ERROR == glGetError());

	const GLint locationAmbientColor = glGetUniformLocation(shaderProgram.id(), ambientString);
	ocean_assert(GL_NO_ERROR == glGetError());
	ocean_assert(locationAmbientColor != -1);
	setUniform(locationAmbientColor, ambientColor_.damped(float(intensity_)));

	const GLint locationDiffiuseColor = glGetUniformLocation(shaderProgram.id(), diffuseString);
	ocean_assert(GL_NO_ERROR == glGetError());
	ocean_assert(locationDiffiuseColor != -1);
	setUniform(locationDiffiuseColor, diffuseColor_.damped(float(intensity_)));

	const GLint locationSpecularColor = glGetUniformLocation(shaderProgram.id(), specularString);
	ocean_assert(GL_NO_ERROR == glGetError());
	ocean_assert(locationSpecularColor != -1);
	setUniform(locationSpecularColor, specularColor_.damped(float(intensity_)));
}

}

}

}
