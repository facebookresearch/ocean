/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_GL_GL_MEDIUM_TEXTURE_H
#define META_OCEAN_PLATFORM_GL_GL_MEDIUM_TEXTURE_H

#include "ocean/platform/gl/GL.h"
#include "ocean/platform/gl/Texture.h"

#include "ocean/media/FrameMedium.h"

namespace Ocean
{

namespace Platform
{

namespace GL
{

/**
 * This class implements a 2D OpenGL texture that receives the image content from a frame medium.
 * @ingroup platformgl
 */
class OCEAN_PLATFORM_GL_EXPORT MediumTexture : public Texture
{
	public:

		/**
		 * Creates a new OpenGL texture object.
		 */
		MediumTexture();

		/**
		 * Creates a new OpenGL texture object.
		 * @param frameMedium The frame medium for the texture
		 */
		explicit MediumTexture(const Media::FrameMediumRef& frameMedium);

		/**
		 * Updates this texture.
		 * @return True, if succeeded
		 */
		bool update();

		/**
		 * Returns the frame medium of this texture.
		 * @return The texture's medium
		 */
		inline const Media::FrameMediumRef& medium() const;

		/**
		 * Sets a new frame medium for this texture.
		 * @param medium Frame medium to set
		 */
		inline void setMedium(const Media::FrameMediumRef& medium);

	protected:

		/// Frame timestamp.
		Timestamp textureFrameTimestamp;

		/// Frame medium holding the image data.
		Media::FrameMediumRef textureFrameMedium;
};

inline MediumTexture::MediumTexture() :
	textureFrameTimestamp(false)
{
	// nothing to do here
}

inline MediumTexture::MediumTexture(const Media::FrameMediumRef& frameMedium) :
	textureFrameTimestamp(false),
	textureFrameMedium(frameMedium)
{
	// nothing to do here
}

inline const Media::FrameMediumRef& MediumTexture::medium() const
{
	return textureFrameMedium;
}

inline void MediumTexture::setMedium(const Media::FrameMediumRef& medium)
{
	textureFrameTimestamp.toInvalid();

	textureFrameMedium = medium;
}

}

}

}

#endif // META_OCEAN_PLATFORM_GL_GL_MEDIUM_TEXTURE_H
