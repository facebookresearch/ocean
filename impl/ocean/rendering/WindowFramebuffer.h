/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_WINDOW_FRAMEBUFFER_H
#define META_OCEAN_RENDERING_WINDOW_FRAMEBUFFER_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Framebuffer.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class WindowFramebuffer;

/**
 * Definition of a smart object reference holding a window framebuffer node.
 * @see SmartObjectRef, WindowFramebuffer.
 * @ingroup rendering
 */
typedef SmartObjectRef<WindowFramebuffer> WindowFramebufferRef;

/**
 * This class is the base class for all window framebuffers.
 * To initialize the framebuffer set the id of the window receiving the framebuffer.<br>
 * All userdefined framebuffer properties must be set before the id is set.<br>
 *
 * Sample code for windows platforms:
 * @code
 * void newFramebuffer(const EngineRef engine, HWND window)
 * {
 *     WindowFramebufferRef framebuffer = engine.createFramebuffer(Framebuffer::FRAMEBUFFER_WINDOW);
 *
 *     if (!framebuffer)
 *         throw OceanException("Failed to create a new framebuffer");
 *
 *     framebuffer->setSupportAntialiasing(4);
 *
 *     if (!framebuffer->setId(size_t(window)))
 *         throw OceanException("Failed to initialize the framebuffer");
 *
 *     // ...
 * }
 * @endcode
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT WindowFramebuffer : virtual public Framebuffer
{
	public:

		/**
		 * Returns the type of this framebuffer.
		 * @see Framebuffer::framebufferType().
		 */
		FramebufferType framebufferType() const override;

		/**
		 * Returns the platform dependent framebuffer base id or handle.
		 * @return Id or handle of the framebuffer base
		 * @see initializeById().
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual size_t id() const;

		/**
		 * Sets the platform dependent framebuffer base id or handle and initializes the framebuffer.
		 * This id can be a handle of a window.<br>
		 * E.g. on windows platforms this id must be the window handle (HWND).<br>
		 * The definition of the id will initialize the rendering context for this framebuffer.
		 * @param id Id or handle of the framebuffer base
		 * @param shareFramebuffer Optional framebuffer to share resources with
		 * @return True, if succeeded
		 * @see id().
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual bool initializeById(const size_t id, const FramebufferRef& shareFramebuffer = FramebufferRef());

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new window framebuffer.
		 */
		WindowFramebuffer();

		/**
		 * Destructs a window framebuffer.
		 */
		~WindowFramebuffer() override;
};

}

}

#endif // META_OCEAN_RENDERING_WINDOW_FRAMEBUFFER_H
