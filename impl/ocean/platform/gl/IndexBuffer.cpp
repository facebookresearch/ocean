/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/gl/IndexBuffer.h"

namespace Ocean
{

namespace Platform
{

namespace GL
{

bool IndexBuffer::setBufferData(const unsigned int* data, const size_t size)
{
	static_assert(sizeof(unsigned int) == sizeof(GLuint), "Invalid data type!");

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

	associatedContext_->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId);
	ocean_assert(GL_NO_ERROR == glGetError());

	associatedContext_->glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(size * sizeof(unsigned int)), data, GL_STATIC_DRAW);
	ocean_assert(GL_NO_ERROR == glGetError());

	bufferSize = size;

	return true;
}

bool IndexBuffer::bind() const
{
	ocean_assert(associatedContext_);
	if (!associatedContext_)
		return false;

	if (bufferId == 0u)
		return false;

	ocean_assert(GL_NO_ERROR == glGetError());

	associatedContext_->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId);
	ocean_assert(GL_NO_ERROR == glGetError());

	return true;
}

IndexBuffer& IndexBuffer::operator=(const IndexBuffer& buffer)
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
