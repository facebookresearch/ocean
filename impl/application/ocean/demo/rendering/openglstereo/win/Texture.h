/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_RENDERING_OPENGL_WIN_TEXTURE_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_RENDERING_OPENGL_WIN_TEXTURE_H

#include "ocean/base/Frame.h"

#include "ocean/media/FrameMedium.h"
#include "ocean/media/Manager.h"

#include <gl\gl.h>

/**
 * This class implements an opengl texture.
 * @ingroup applicationdemorenderingopenglstereowin
 */
class Texture
{
	public:

		/**
		 * Creates a new texture.
		 */
		Texture() = default;

		/**
		 * Destructs a texture.
		 */
		~Texture();

		/**
		 * Updates this texture.
		 * @return Frame type of the used frame
		 */
		Ocean::FrameType update();

		/**
		 * Bind this texture.
		 */
		void bind();

		/**
		 * Unbinds this texture.
		 */
		void unbind();

		/**
		 * Releases the texture.
		 */
		void release();

		/**
		 * Returns the medium.
		 */
		Ocean::Media::FrameMediumRef medium();

		/**
		 * Sets a new frame medium for this texture.
		 * @param medium Frame medium to set
		 */
		void setMedium(const Ocean::Media::FrameMediumRef& medium);

	protected:

		/// OpenGL texture id.
		GLuint textureId_ = 0u;

		/// OpenGL texture pixel format.
		GLuint texturePixelFormat_ = 0u;

		/// Frame timestamp.
		Ocean::Timestamp frameTimestamp_;

		/// Frame type.
		Ocean::FrameType frameType_;

		/// Frame medium holding the image data.
		Ocean::Media::FrameMediumRef frameMedium_;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_RENDERING_OPENGL_WIN_TEXTURE_H
