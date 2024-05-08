/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESObject.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESObject::GLESObject()
{
	// nothing to do here
}

GLESObject::~GLESObject()
{
	// nothing to do here
}

const std::string& GLESObject::engineName() const
{
	return nameGLESceneGraphEngine();
}

void GLESObject::setUniform(const GLint location, const int32_t value)
{
	ocean_assert(location != -1);

	ocean_assert(GL_NO_ERROR == glGetError());
	glUniform1i(location, value);
	ocean_assert(GL_NO_ERROR == glGetError());
}

void GLESObject::setUniform(const GLint location, const uint32_t value)
{
	ocean_assert(location != -1);

	ocean_assert(GL_NO_ERROR == glGetError());
	glUniform1ui(location, value);
	ocean_assert(GL_NO_ERROR == glGetError());
}

void GLESObject::setUniform(const GLint location, const float value)
{
	ocean_assert(location != -1);

	ocean_assert(GL_NO_ERROR == glGetError());
	glUniform1f(location, value);
	ocean_assert(GL_NO_ERROR == glGetError());
}

void GLESObject::setUniform(const GLint location, const double value)
{
	ocean_assert(location != -1);

	ocean_assert(GL_NO_ERROR == glGetError());
	glUniform1f(location, float(value));
	ocean_assert(GL_NO_ERROR == glGetError());
}

void GLESObject::setUniform(const GLint location, const RGBAColor& value)
{
	static_assert(sizeof(RGBAColor::Type) == 4, "Invalid data type!");

	ocean_assert(location != -1);

	ocean_assert(GL_NO_ERROR == glGetError());
	glUniform4fv(location, 1, value.data());
	ocean_assert(GL_NO_ERROR == glGetError());
}

void GLESObject::setUniform(const GLint location, const SquareMatrixF3& value)
{
	ocean_assert(location != -1);
	ocean_assert(GL_NO_ERROR == glGetError());

	ocean_assert(sizeof(GLfloat) == sizeof(SquareMatrix4::Type));
	glUniformMatrix3fv(location, 1, GL_FALSE, value.data());
	ocean_assert(GL_NO_ERROR == glGetError());
}

void GLESObject::setUniform(const GLint location, const SquareMatrixD3& value)
{
	ocean_assert(location != -1);
	ocean_assert(GL_NO_ERROR == glGetError());

	float matrixValues[9];

	value.copyElements(matrixValues);
	glUniformMatrix3fv(location, 1, GL_FALSE, matrixValues);
	ocean_assert(GL_NO_ERROR == glGetError());
}

void GLESObject::setUniform(const GLint location, const SquareMatrixF4& value)
{
	ocean_assert(location != -1);
	ocean_assert(GL_NO_ERROR == glGetError());

	glUniformMatrix4fv(location, 1, GL_FALSE, value.data());
	ocean_assert(GL_NO_ERROR == glGetError());
}

void GLESObject::setUniform(const GLint location, const SquareMatrixD4& value)
{
	ocean_assert(location != -1);
	ocean_assert(GL_NO_ERROR == glGetError());

	float matrixValues[16];

	value.copyElements(matrixValues);
	glUniformMatrix4fv(location, 1, GL_FALSE, matrixValues);
	ocean_assert(GL_NO_ERROR == glGetError());
}

void GLESObject::setUniform(const GLint location, const HomogenousMatrixF4& value)
{
	ocean_assert(location != -1);
	ocean_assert(GL_NO_ERROR == glGetError());

	glUniformMatrix4fv(location, 1, GL_FALSE, value.data());
	ocean_assert(GL_NO_ERROR == glGetError());
}

void GLESObject::setUniform(const GLint location, const HomogenousMatrixD4& value)
{
	ocean_assert(location != -1);
	ocean_assert(GL_NO_ERROR == glGetError());

	float matrixValues[16];

	value.copyElements(matrixValues);
	glUniformMatrix4fv(location, 1, GL_FALSE, matrixValues);
	ocean_assert(GL_NO_ERROR == glGetError());
}

void GLESObject::setUniform(const GLint location, const VectorF2& value)
{
	ocean_assert(location != -1);
	ocean_assert(GL_NO_ERROR == glGetError());
	glUniform2f(location, value[0], value[1]);
	ocean_assert(GL_NO_ERROR == glGetError());
}

void GLESObject::setUniform(const GLint location, const VectorD2& value)
{
	ocean_assert(location != -1);
	ocean_assert(GL_NO_ERROR == glGetError());
	glUniform2f(location, float(value[0]), float(value[1]));
	ocean_assert(GL_NO_ERROR == glGetError());
}

void GLESObject::setUniform(const GLint location, const VectorF3& value)
{
	ocean_assert(location != -1);
	ocean_assert(GL_NO_ERROR == glGetError());
	glUniform3f(location, value[0], value[1], value[2]);
	ocean_assert(GL_NO_ERROR == glGetError());
}

void GLESObject::setUniform(const GLint location, const VectorD3& value)
{
	ocean_assert(location != -1);
	ocean_assert(GL_NO_ERROR == glGetError());
	glUniform3f(location, float(value[0]), float(value[1]), float(value[2]));
	ocean_assert(GL_NO_ERROR == glGetError());
}

void GLESObject::setUniform(const GLint location, const VectorF4& value)
{
	ocean_assert(location != -1);
	ocean_assert(GL_NO_ERROR == glGetError());
	glUniform4f(location, value[0], value[1], value[2], value[3]);
	ocean_assert(GL_NO_ERROR == glGetError());
}

void GLESObject::setUniform(const GLint location, const VectorD4& value)
{
	ocean_assert(location != -1);
	ocean_assert(GL_NO_ERROR == glGetError());
	glUniform4f(location, float(value[0]), float(value[1]), float(value[2]), float(value[3]));
	ocean_assert(GL_NO_ERROR == glGetError());
}

void GLESObject::setUniform(const GLint location, const SquareMatricesT3<float>& value)
{
	ocean_assert(location != -1);
	ocean_assert(GL_NO_ERROR == glGetError());
	glUniformMatrix3fv(location, GLsizei(value.size()), GL_FALSE, (const GLfloat*)(value.data()));
	ocean_assert(GL_NO_ERROR == glGetError());
}

void GLESObject::setUniform(const GLint location, const SquareMatricesT3<double>& value)
{
	ocean_assert(location != -1);
	ocean_assert(GL_NO_ERROR == glGetError());

	std::vector<float> matrixValues(value.size() * 9);
	for (size_t n = 0; n < value.size(); ++n)
	{
		value[n].copyElements(matrixValues.data() + 9 * n);
	}

	glUniformMatrix3fv(location, GLsizei(value.size()), GL_FALSE, matrixValues.data());
	ocean_assert(GL_NO_ERROR == glGetError());
}

void GLESObject::setUniform(const GLint location, const SquareMatricesT4<float>& value)
{
	ocean_assert(location != -1);
	ocean_assert(GL_NO_ERROR == glGetError());
	glUniformMatrix4fv(location, GLsizei(value.size()), GL_FALSE, (const GLfloat*)(value.data()));
	ocean_assert(GL_NO_ERROR == glGetError());
}

void GLESObject::setUniform(const GLint location, const SquareMatricesT4<double>& value)
{
	ocean_assert(location != -1);
	ocean_assert(GL_NO_ERROR == glGetError());

	std::vector<float> matrixValues(value.size() * 16);
	for (size_t n = 0; n < value.size(); ++n)
	{
		value[n].copyElements(matrixValues.data() + 16 * n);
	}

	glUniformMatrix4fv(location, GLsizei(value.size()), GL_FALSE, matrixValues.data());
	ocean_assert(GL_NO_ERROR == glGetError());
}

void GLESObject::setUniform(const GLint location, const HomogenousMatricesF4& value)
{
	ocean_assert(location != -1);
	ocean_assert(GL_NO_ERROR == glGetError());
	glUniformMatrix4fv(location, GLsizei(value.size()), GL_FALSE, (const GLfloat*)(value.data()));
	ocean_assert(GL_NO_ERROR == glGetError());
}

void GLESObject::setUniform(const GLint location, const HomogenousMatricesD4& value)
{
	ocean_assert(location != -1);
	ocean_assert(GL_NO_ERROR == glGetError());

	std::vector<float> matrixValues(value.size() * 16);
	for (size_t n = 0; n < value.size(); ++n)
	{
		value[n].copyElements(matrixValues.data() + 16 * n);
	}

	glUniformMatrix4fv(location, GLsizei(value.size()), GL_FALSE, matrixValues.data());
	ocean_assert(GL_NO_ERROR == glGetError());
}

void GLESObject::setUniform(const GLint location, const VectorsF2& value)
{
	ocean_assert(location != -1);
	ocean_assert(GL_NO_ERROR == glGetError());
	glUniform2fv(location, GLsizei(value.size()), (const GLfloat*)(value.data()));
	ocean_assert(GL_NO_ERROR == glGetError());
}

void GLESObject::setUniform(const GLint location, const VectorsD2& value)
{
	VectorsF2 valueF;
	valueF.reserve(value.size());
	for (const VectorD2& element : value)
	{
		valueF.emplace_back(float(element.x()), float(element.y()));
	}

	return setUniform(location, valueF);
}

void GLESObject::setUniform(const GLint location, const VectorsF3& value)
{
	ocean_assert(location != -1);
	ocean_assert(GL_NO_ERROR == glGetError());
	glUniform3fv(location, GLsizei(value.size()), (const GLfloat*)(value.data()));
	ocean_assert(GL_NO_ERROR == glGetError());
}

void GLESObject::setUniform(const GLint location, const VectorsD3& value)
{
	VectorsF3 valueF;
	valueF.reserve(value.size());
	for (const VectorD3& element : value)
	{
		valueF.emplace_back(float(element.x()), float(element.y()), float(element.z()));
	}

	return setUniform(location, valueF);
}

void GLESObject::setUniform(const GLint location, const VectorsF4& value)
{
	ocean_assert(location != -1);
	ocean_assert(GL_NO_ERROR == glGetError());
	glUniform4fv(location, GLsizei(value.size()), (const GLfloat*)(value.data()));
	ocean_assert(GL_NO_ERROR == glGetError());
}

void GLESObject::setUniform(const GLint location, const VectorsD4& value)
{
	VectorsF4 valueF;
	valueF.reserve(value.size());
	for (const VectorD4& element : value)
	{
		valueF.emplace_back(float(element.x()), float(element.y()), float(element.z()), float(element.w()));
	}

	return setUniform(location, valueF);
}

void GLESObject::setUniform(const GLint location, const float* values, const size_t size)
{
	ocean_assert(location != -1);
	ocean_assert(values != nullptr && size >= 1);
	ocean_assert(GL_NO_ERROR == glGetError());
	glUniform1fv(location, (unsigned int)(size), values);
	ocean_assert(GL_NO_ERROR == glGetError());
}

}

}

}
