// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/platform/gl/Buffer.h"

namespace Ocean
{

namespace Platform
{

namespace GL
{

Buffer::~Buffer()
{
	release();
}

bool Buffer::release()
{
	if (bufferId != 0u)
	{
		ocean_assert(associatedContext_);
		if (!associatedContext_)
			return false;

		ocean_assert(glGetError() == GL_NO_ERROR);
		associatedContext_->glDeleteBuffers(1, &bufferId);
		ocean_assert(glGetError() == GL_NO_ERROR);

		bufferId = 0u;
	}

	return true;
}

}

}

}
