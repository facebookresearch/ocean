/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESDirectionalLight.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESDirectionalLight::GLESDirectionalLight() :
	GLESLightSource(),
	DirectionalLight(),
	direction_(0, 0, 0)
{
	// nothing to do here
}

GLESDirectionalLight::~GLESDirectionalLight()
{
	// nothing to do here
}

Vector3 GLESDirectionalLight::direction() const
{
	return direction_;
}

void GLESDirectionalLight::setDirection(const Vector3& direction)
{
	ocean_assert(direction.isUnit(Numeric::weakEps()));
	direction_ = direction;
}

void GLESDirectionalLight::bindLight(GLESShaderProgram& shaderProgram, const HomogenousMatrix4& camera_T_model, const HomogenousMatrix4& camera_T_world, const SquareMatrix3& normalMatrix, const unsigned int index)
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
	positionString[7] = char('0' + index);

	Vector3 direction;

	if (transformationType_ == TT_PARENT)
	{
		direction = (normalMatrix * direction_).normalizedOrZero();
	}
	else
	{
		ocean_assert(transformationType_ == TT_WORLD);

		const SquareMatrix3 worldNormalMatrix(camera_T_world.rotationMatrix().inverted().transposed());
		direction = (worldNormalMatrix * direction_).normalizedOrZero();
	}

	ocean_assert(direction.isUnit());
	ocean_assert(GL_NO_ERROR == glGetError());

	const GLint locationPosition = glGetUniformLocation(shaderProgram.id(), positionString);
	ocean_assert(GL_NO_ERROR == glGetError());
	ocean_assert(locationPosition != -1);
	setUniform(locationPosition, Vector4(direction, 0));
}

}

}

}
