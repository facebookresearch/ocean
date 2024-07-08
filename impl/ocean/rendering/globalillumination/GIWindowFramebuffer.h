/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_WINDOW_BITMAP_H
#define META_OCEAN_RENDERING_GI_WINDOW_BITMAP_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"
#include "ocean/rendering/globalillumination/GIFramebuffer.h"

#include "ocean/rendering/WindowFramebuffer.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

/**
 * This class is the base class for all GlobalIllumination window framebuffers.
 * @ingroup renderinggi
 */
class OCEAN_RENDERING_GI_EXPORT GIWindowFramebuffer :
	virtual public GIFramebuffer,
	virtual public WindowFramebuffer
{
	friend class GIEngine;

	public:

		/**
		 * Returns the platform dependent framebuffer base id or handle.
		 * @see Framebuffer::id().
		 */
		size_t id() const override;

		/**
		 * Sets the platform dependent framebuffer base id or handle.
		 * @see Frambuffer::initializeById().
		 */
		bool initializeById(const size_t id, const FramebufferRef& shareFramebuffer) override;

		/**
		 * Returns the global lighting mode of the entire framebuffer.
		 * @see Framebuffer::lightingMode().
		 */
		LightingMode lightingMode() const override;

		/**
		 * Returns the shadow technique of the entire framebuffer.
		 * @see Framebuffer::shadowTechnique().
		 */
		ShadowTechnique shadowTechnique() const override;

		/**
		 * Sets the global lighting mode of the entire framebuffer.
		 * @see Framebuffer::setLightingMode().
		 */
		void setLightingMode(const LightingMode lightingMode) override;

		/**
		 * Sets the shadow technique of the entire framebuffer.
		 * @see Framebuffer::setShadowTechnique().
		 */
		void setShadowTechnique(const ShadowTechnique technique) override;

		/**
		 * Renders the next frame into the framebuffer.
		 * @see Framebuffer::render().
		 */
		void render() override;

	protected:

		/**
		 * Creates a new Global Illumination window framebuffer.
		 * @param preferredGraphicAPI Preferred graphic API to be used for this framebuffer
		 */
		explicit GIWindowFramebuffer(const Engine::GraphicAPI preferredGraphicAPI = Engine::API_DEFAULT);

		/**
		 * Destructs a Global Illumination window framebuffer.
		 */
		~GIWindowFramebuffer() override;

		/**
		 * Releases the framebuffer.
		 * @see Framebuffer::release().
		 */
		void release() override;

	protected:

#if defined(_WINDOWS)

		/// Handle of this window.
		HWND windowHandle;

		/// Device context of this window.
		HDC windowDC;

#elif defined(__APPLE__)

		/// The view object of this window.
		void* windowView;

#endif

};

}

}

}

#endif // META_OCEAN_RENDERING_GI_WINDOW_BITMAP_H
