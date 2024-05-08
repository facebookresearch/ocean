/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/gl/VertexBuffer.h"

namespace Ocean
{

namespace Platform
{

namespace GL
{

bool VertexBuffer::setBufferData(const double* data, const size_t size)
{
	std::vector<float> floatData(size);

	for (size_t n = 0; n < size; ++n)
		floatData[n] = float(data[n]);

	return setBufferData(floatData.data(), size);
}

bool VertexBuffer::setBufferData(const float* data, const size_t size)
{
	static_assert(sizeof(float) == sizeof(GLfloat), "Invalid data type!");

	ocean_assert(associatedContext_);
	if (!associatedContext_)
		return false;

	ocean_assert(glGetError() == GL_NO_ERROR);

	if (bufferId == 0u)
	{
		associatedContext_->glGenBuffers(1, &bufferId);
		ocean_assert(GL_NO_ERROR == glGetError());
	}

	if (bufferId == 0u)
		return false;

	associatedContext_->glBindBuffer(GL_ARRAY_BUFFER, bufferId);
	ocean_assert(GL_NO_ERROR == glGetError());

	associatedContext_->glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(size * sizeof(GLfloat)), data, GL_STATIC_DRAW);
	ocean_assert(GL_NO_ERROR == glGetError());

	bufferSize = size;

	return true;
}

bool VertexBuffer::bindToProgram(const GLuint programId, const std::string& attributeName, const unsigned int elements) const
{
	ocean_assert(associatedContext_);
	if (!associatedContext_)
		return false;

	ocean_assert(programId != 0u && !attributeName.empty() && elements != 0u);

	if (bufferId == 0u)
		return false;

	ocean_assert(GL_NO_ERROR == glGetError());

	associatedContext_->glBindBuffer(GL_ARRAY_BUFFER, bufferId);
	ocean_assert(GL_NO_ERROR == glGetError());

	ocean_assert(associatedContext_->glIsProgram(programId));
	ocean_assert(GL_NO_ERROR == glGetError());

	const GLint location = associatedContext_->glGetAttribLocation(programId, attributeName.c_str());
	ocean_assert(GL_NO_ERROR == glGetError());

	ocean_assert(location != -1);
	if (location == -1)
		return false;

	associatedContext_->glEnableVertexAttribArray(location);
	ocean_assert(GL_NO_ERROR == glGetError());

	associatedContext_->glVertexAttribPointer(location, elements, GL_FLOAT, GL_FALSE, 0, 0);
	ocean_assert(GL_NO_ERROR == glGetError());

	return true;
}

VertexBuffer& VertexBuffer::operator=(const VertexBuffer& buffer)
{
	if (this != &buffer)
	{
		release();

		bufferId = buffer.bufferId;
		bufferSize = buffer.bufferSize;
	}

	return *this;
}

}

}

}
