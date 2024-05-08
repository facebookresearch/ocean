/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_BITMAP_FRAMEBUFFER_H
#define META_OCEAN_RENDERING_BITMAP_FRAMEBUFFER_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Framebuffer.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class BitmapFramebuffer;

/**
 * Definition of a smart object reference holding a bitmap framebuffer node.
 * @see SmartObjectRef, BitmapFramebuffer.
 * @ingroup rendering
 */
typedef SmartObjectRef<BitmapFramebuffer> BitmapFramebufferRef;

/**
 * This class is the base class for all bitmap framebuffers.
 * The framebuffer must be initialized before usage.
 * @see initialize().
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT BitmapFramebuffer : virtual public Framebuffer
{
	public:

		/**
		 * Initializes the framebuffer.
		 * @see Framebuffer::initialize().
		 */
		bool initialize(const FramebufferRef& shareFramebuffer = FramebufferRef()) override;

		/**
		 * Returns the type of this framebuffer.
		 * @see Framebuffer::framebufferType().
		 */
		FramebufferType framebufferType() const override;

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new bitmap framebuffer.
		 */
		BitmapFramebuffer();

		/**
		 * Destructs a bitmap framebuffer.
		 */
		~BitmapFramebuffer() override;
};

}

}

#endif // META_OCEAN_RENDERING__H
