/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/gl/VertexArray.h"

namespace Ocean
{

namespace Platform
{

namespace GL
{

VertexArray::~VertexArray()
{
	release();
}

bool VertexArray::bind()
{
	ocean_assert(associatedContext_);
	if (!associatedContext_)
		return false;

	if (arrayId == 0u)
	{
		ocean_assert(glGetError() == GL_NO_ERROR);
		associatedContext_->glGenVertexArrays(1, &arrayId);
		ocean_assert(glGetError() == GL_NO_ERROR);
	}

	if (arrayId == 0u)
		return false;

	ocean_assert(GL_NO_ERROR == glGetError());
	associatedContext_->glBindVertexArray(arrayId);
	ocean_assert(GL_NO_ERROR == glGetError());

	return true;
}

bool VertexArray::unbind()
{
	ocean_assert(associatedContext_);
	if (!associatedContext_)
		return false;

	ocean_assert(GL_NO_ERROR == glGetError());
	associatedContext_->glBindVertexArray(0u);
	ocean_assert(GL_NO_ERROR == glGetError());

	return true;
}

bool VertexArray::release()
{
	if (arrayId != 0u)
	{
		ocean_assert(associatedContext_);
		if (!associatedContext_)
			return false;

		ocean_assert(glGetError() == GL_NO_ERROR);
		associatedContext_->glDeleteVertexArrays(1, &arrayId);
		ocean_assert(glGetError() == GL_NO_ERROR);

		arrayId = 0u;
	}

	return true;
}

VertexArray& VertexArray::operator=(const VertexArray& object)
{
	if (this != &object)
	{
		release();
		arrayId = object.arrayId;
	}

	return *this;
}

}

}

}
