/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_BITMAP_FRAMEBUFFER_H
#define META_OCEAN_RENDERING_GI_BITMAP_FRAMEBUFFER_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"
#include "ocean/rendering/globalillumination/GIFramebuffer.h"

#include "ocean/rendering/BitmapFramebuffer.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

/**
 * This class is the base class for all GlobalIllumination bitmap framebuffers.
 * @ingroup renderinggi
 */
class OCEAN_RENDERING_GI_EXPORT GIBitmapFramebuffer :
	virtual public GIFramebuffer,
	virtual public BitmapFramebuffer
{
	friend class GIEngine;

	public:

		/**
		 * Initializes the framebuffer.
		 * @see Framebuffer::initialize().
		 */
		bool initialize(const FramebufferRef& shareFramebuffer = FramebufferRef()) override;

	protected:

		/**
		 * Creates a new bitmap frambuffer.
		 * @param preferredGraphicAPI Preferred graphic API to be used for this framebuffer
		 */
		GIBitmapFramebuffer(const Engine::GraphicAPI preferredGraphicAPI);

		/**
		 * Destructs a bitmap framebuffer.
		 */
		~GIBitmapFramebuffer() override;
};

}

}

}

#endif // META_OCEAN_RENDERING_GI_BITMAP_FRAMEBUFFER_H
