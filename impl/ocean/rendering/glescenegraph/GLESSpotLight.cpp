/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESSpotLight.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESSpotLight::GLESSpotLight() :
	GLESPointLight(),
	SpotLight(),
	direction_(Vector3(0, 0, -1)),
	coneAngle_(Numeric::deg2rad(45)),
	spotExponent_(0)
{
	// nothing to do here
}

GLESSpotLight::~GLESSpotLight()
{
	// nothing to do here
}

Vector3 GLESSpotLight::direction() const
{
	return direction_;
}

Scalar GLESSpotLight::coneAngle() const
{
	return coneAngle_;
}

Scalar GLESSpotLight::spotExponent() const
{
	return spotExponent_;
}

void GLESSpotLight::setDirection(const Vector3& direction)
{
	const ScopedLock scopedLock(objectLock);

	ocean_assert(direction.isUnit(Numeric::weakEps()));
	direction_ = direction;
}

bool GLESSpotLight::setConeAngle(const Scalar angle)
{
	if (angle < 0 || angle > Numeric::pi_2())
	{
		return false;
	}

	const ScopedLock scopedLock(objectLock);
	coneAngle_ = angle;

	return true;
}

bool GLESSpotLight::setSpotExponent(const Scalar exponent)
{
	if (exponent < 0 || exponent > 1)
	{
		return false;
	}

	const ScopedLock scopedLock(objectLock);
	spotExponent_ = exponent;

	return true;
}

void GLESSpotLight::bindLight(GLESShaderProgram& shaderProgram, const HomogenousMatrix4& camera_T_model, const HomogenousMatrix4& camera_T_world, const SquareMatrix3& normalMatrix, const unsigned int index)
{
	ocean_assert(shaderProgram.isCompiled());
	ocean_assert(camera_T_model.isValid());
	ocean_assert(camera_T_world.isValid());
	ocean_assert(!normalMatrix.isSingular());
	ocean_assert(index < 8u);

	const ScopedLock scopedLock(objectLock);

	GLESLightSource::bindLight(shaderProgram, camera_T_model, camera_T_world, normalMatrix, index); // The bind light function of the point light object is not used

	ocean_assert(enabled_);

	char positionString[] = "lights[~].positionOrDirection";
	char spotDirectionString[] = "lights[~].spotDirection";
	char spotCutOffAngleString[] = "lights[~].spotCutOffAngle";
	char spotExponentString[] = "lights[~].spotExponent";
	char attenuationIsUsedString[] = "lights[~].attenuationIsUsed";
	char attenuationFactorsString[] = "lights[~].attenuationFactors";

	positionString[7] = char('0' + index);
	spotDirectionString[7] = char('0' + index);
	spotCutOffAngleString[7] = char('0' + index);
	spotExponentString[7] = char('0' + index);
	attenuationIsUsedString[7] = char('0' + index);
	attenuationFactorsString[7] = char('0' + index);

	Vector3 position;
	Vector3 direction;

	if (transformationType_ == TT_PARENT)
	{
		position = camera_T_model * position_;

		direction = (normalMatrix * direction_).normalizedOrZero();
	}
	else
	{
		ocean_assert(transformationType_ == TT_WORLD);

		position = camera_T_world * position_;

		const SquareMatrix3 worldNormalMatrix(camera_T_world.rotationMatrix().inverted().transposed());
		direction = (worldNormalMatrix * direction_).normalizedOrZero();
	}

	ocean_assert(direction.isUnit());

	ocean_assert(GL_NO_ERROR == glGetError());

	const GLint locationPosition = glGetUniformLocation(shaderProgram.id(), positionString);
	ocean_assert(GL_NO_ERROR == glGetError());
	ocean_assert(locationPosition != -1);
	setUniform(locationPosition, Vector4(position, 1));

	const GLint locationDirection = glGetUniformLocation(shaderProgram.id(), spotDirectionString);
	ocean_assert(GL_NO_ERROR == glGetError());
	ocean_assert(locationDirection != -1);
	setUniform(locationDirection, direction);

	const GLint locationCutOffAngle = glGetUniformLocation(shaderProgram.id(), spotCutOffAngleString);
	ocean_assert(GL_NO_ERROR == glGetError());
	ocean_assert(locationCutOffAngle != -1);
	setUniform(locationCutOffAngle, coneAngle_);

	const GLint locationSpotExponent = glGetUniformLocation(shaderProgram.id(), spotExponentString);
	ocean_assert(GL_NO_ERROR == glGetError());
	ocean_assert(locationSpotExponent != -1);
	setUniform(locationSpotExponent, spotExponent_);

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
