/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESMediaTexture2D.h"
#include "ocean/rendering/glescenegraph/GLESAttributeSet.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/media/FrameMedium.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESMediaTexture2D::GLESMediaTexture2D() :
	GLESTexture(),
	Texture2D(),
	DynamicObject(),
	GLESTexture2D(),
	MediaTexture2D()
{
	// nothing to do here
}

GLESMediaTexture2D::~GLESMediaTexture2D()
{
	// nothing to do here
}

bool GLESMediaTexture2D::isValid() const
{
	return primaryTextureId_ != 0u;
}

std::string GLESMediaTexture2D::descriptiveInformation() const
{
	const ScopedLock scopedLock(objectLock);

	std::string result = Object::descriptiveInformation();

	if (frameMedium_)
	{
		result += ", with medium '" + frameMedium_->url() + "'";
	}

	return result;
}

void GLESMediaTexture2D::onDynamicUpdate(const ViewRef& /*view*/, const Timestamp timestamp)
{
	const ScopedLock scopedLock(objectLock);

	if (frameMedium_.isNull())
	{
		return;
	}

	if (timestamp != renderTimestamp_)
	{
		const FrameRef frame = frameMedium_->frame(timestamp);
		if (frame.isNull())
		{
			return;
		}

		renderTimestamp_ = timestamp;

		if (frame->isValid() && frame->timestamp() != frameTimestamp_)
		{
			updateTexture(*frame);
		}
	}
}

}

}

}
