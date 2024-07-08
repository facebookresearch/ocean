/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESPointLight.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESPointLight::GLESPointLight() :
	GLESLightSource(),
	PointLight(),
	position_(0, 0, 0),
	attenuationFactors_(Vector3(1, 0, 0))
{
	// nothing to do here
}

GLESPointLight::~GLESPointLight()
{
	// nothing to do here
}

Vector3 GLESPointLight::position() const
{
	return position_;
}

Vector3 GLESPointLight::attenuation() const
{
	return attenuationFactors_;
}

void GLESPointLight::setPosition(const Vector3& position)
{
	const ScopedLock scopedLock(objectLock);

	position_ = position;
}

bool GLESPointLight::setAttenuation(const Vector3& factors)
{
	if (factors.x() < 0 || factors.y() < 0 || factors.z() < 0)
	{
		return false;
	}

	const ScopedLock scopedLock(objectLock);

	attenuationFactors_ = factors;
	return true;
}

void GLESPointLight::bindLight(GLESShaderProgram& shaderProgram, const HomogenousMatrix4& camera_T_model, const HomogenousMatrix4& camera_T_world, const SquareMatrix3& normalMatrix, const unsigned int index)
{
	ocean_assert(shaderProgram.isCompiled());
	ocean_assert(camera_T_model.isValid());
	ocean_assert(camera_T_world.isValid());
	ocean_assert(!normalMatrix.isSingular());
	ocean_assert(index < 8u);

	const ScopedLock scopedLock(objectLock);

	GLESLightSource::bindLight(shaderProgram, camera_T_model, camera_T_world, normalMatrix, index);

	ocean_assert(enabled_);

	char positionString[] = "lights[~].positionOrDirection";
	char spotCutOffAngleString[] = "lights[~].spotCutOffAngle";
	char attenuationIsUsedString[] = "lights[~].attenuationIsUsed";
	char attenuationFactorsString[] = "lights[~].attenuationFactors";

	ocean_assert(positionString[7] == '~');
	positionString[7] = char('0' + index);

	ocean_assert(spotCutOffAngleString[7] == '~');
	spotCutOffAngleString[7] = char('0' + index);

	ocean_assert(attenuationIsUsedString[7] == '~');
	attenuationIsUsedString[7] = char('0' + index);

	ocean_assert(attenuationFactorsString[7] == '~');
	attenuationFactorsString[7] = char('0' + index);

	Vector3 position;

	if (transformationType_ == TT_PARENT)
	{
		position = camera_T_model * position_;
	}
	else
	{
		ocean_assert(transformationType_ == TT_WORLD);

		position = camera_T_world * position_;
	}

	ocean_assert(GL_NO_ERROR == glGetError());

	const GLint locationPosition = glGetUniformLocation(shaderProgram.id(), positionString);
	ocean_assert(GL_NO_ERROR == glGetError());
	ocean_assert(locationPosition != -1);
	setUniform(locationPosition, Vector4(position, 1));

	const GLint locationCutOffAngle = glGetUniformLocation(shaderProgram.id(), spotCutOffAngleString);
	ocean_assert(GL_NO_ERROR == glGetError());
	ocean_assert(locationCutOffAngle != -1);
	setUniform(locationCutOffAngle, 0.0f);

	const bool usingAttenuation = attenuationFactors_ != Vector3(1, 0, 0);

	const GLint locationAttenuationIsUsed = glGetUniformLocation(shaderProgram.id(), attenuationIsUsedString);
	ocean_assert(GL_NO_ERROR == glGetError());
	ocean_assert(locationAttenuationIsUsed != -1);
	setUniform(locationAttenuationIsUsed, usingAttenuation ? 1 : 0);

	if (usingAttenuation)
	{
		const GLint locationAttenuationFactors = glGetUniformLocation(shaderProgram.id(), attenuationFactorsString);
		ocean_assert(GL_NO_ERROR == glGetError());
		ocean_assert(locationAttenuationFactors != -1);
		setUniform(locationAttenuationFactors, attenuationFactors_);
	}
}

}

}

}
