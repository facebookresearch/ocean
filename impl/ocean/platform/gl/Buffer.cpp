/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
