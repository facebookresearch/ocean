/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/globalillumination/GIBitmapFramebuffer.h"

#include "ocean/cv/FrameConverter.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

GIBitmapFramebuffer::GIBitmapFramebuffer(const Engine::GraphicAPI preferredGraphicAPI) :
		GIFramebuffer(preferredGraphicAPI)
{
	// nothing to do here
}

GIBitmapFramebuffer::~GIBitmapFramebuffer()
{
	// nothing to do here
}

bool GIBitmapFramebuffer::initialize(const FramebufferRef& shareFramebuffer)
{
	ocean_assert_and_suppress_unused(shareFramebuffer.isNull(), shareFramebuffer);

	release();

	return true;
}

}

}

}
