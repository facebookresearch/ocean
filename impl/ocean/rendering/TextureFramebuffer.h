/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_TEXTURE_FRAMEBUFFER_H
#define META_OCEAN_RENDERING_TEXTURE_FRAMEBUFFER_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Texture.h"

#include "ocean/base/Frame.h"

#include "ocean/cv/PixelBoundingBox.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class TextureFramebuffer;

/**
 * Definition of a smart object reference holding a texture framebuffer.
 * @see SmartObjectRef, TextureFramebuffer.
 * @ingroup rendering
 */
typedef SmartObjectRef<TextureFramebuffer> TextureFramebufferRef;

/**
 * This class is the base class for all textures based on a framebuffer.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT TextureFramebuffer : virtual public Texture
{
	public:

		/**
		 * Resizes the texture framebuffer.
		 * @param width The width of the framebuffer in pixel, with range [1, infinity)
		 * @param height The height of the framebuffer in pixel, with range [1, infinity)
		 */
		virtual bool resize(const unsigned int width, const unsigned int height) = 0;

		/**
		 * Returns the texture framebuffer's internal pixel format.
		 * @return The pixel format of the texture framebuffer
		 */
		virtual FrameType::PixelFormat pixelFormat() const = 0;

		/**
		 * Returns the texture wrap type in s direction.
		 * The default is WRAP_CLAMP.
		 * @return Wrap type in s direction
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see setWrapTypeS(), wrapTypeT().
		 */
		virtual WrapType wrapTypeS() const;

		/**
		 * Returns the texture wrap type in t direction.
		 * The default is WRAP_CLAMP.
		 * @return Wrap type in t direction
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see setWrapTypeT(), wrapTypeS().
		 */
		virtual WrapType wrapTypeT() const;

		/**
		 * Sets the texture framebuffer's internal pixel format.
		 * @param pixelFormat The pixel format of the texture framebuffer
		 * @return True, if succeeded
		 */
		virtual bool setPixelFormat(const FrameType::PixelFormat pixelFormat) = 0;

		/**
		 * Sets the texture wrap type in s direction.
		 * @param type Wrap type to set
		 * @return True, if succeeded
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see wrapTypeS(), setWrapTypeT().
		 */
		virtual bool setWrapTypeS(const WrapType type);

		/**
		 * Sets the texture wrap type in t direction.
		 * @param type Wrap type to set
		 * @return True, if succeeded
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see wrapTypeT(), setWrapTypeS().
		 */
		virtual bool setWrapTypeT(const WrapType type);

		/**
		 * Copies the image content of the color texture to a given frame.
		 * @param frame The frame to which the color texture will be copied, the frame will be adjusted if the pixel format does match
		 * @param subRegion Optional sub-region within the framebuffer to copy; an invalid bounding box to copy the entire framebuffer
		 * @return True, if succeeded
		 */
		virtual bool copyColorTextureToFrame(Frame& frame, const CV::PixelBoundingBox& subRegion = CV::PixelBoundingBox());

		/**
		 * Copies the image content of the depth texture to a given frame.
		 * @param frame The frame to which the depth texture will be copied, the frame will be adjusted if the pixel format does match
		 * @param subRegion Optional sub-region within the framebuffer to copy; an invalid bounding box to copy the entire framebuffer
		 * @return True, if succeeded
		 */
		virtual bool copyDepthTextureToFrame(Frame& frame, const CV::PixelBoundingBox& subRegion = CV::PixelBoundingBox());

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		inline ObjectType type() const override;

	protected:

		/**
		 * Creates a new texture object.
		 */
		 TextureFramebuffer() = default;

		/**
		 * Destructs a texture object.
		 */
		~TextureFramebuffer() override;
};

inline TextureFramebuffer::ObjectType TextureFramebuffer::type() const
{
	return TYPE_TEXTURE_FRAMEBUFFER;
}

}

}

#endif // META_OCEAN_RENDERING_TEXTURE_FRAMEBUFFER_H
