/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_APPLICATION_OCEAN_DEMO_RENDERING_OPENGL_WIN_TEXTURE_H
#define META_OCEAN_APPLICATION_OCEAN_DEMO_RENDERING_OPENGL_WIN_TEXTURE_H

#include "application/ocean/demo/rendering/openglstereo/win/OpenGLStereo.h"

#include "ocean/base/Frame.h"

#include "ocean/media/FrameMedium.h"
#include "ocean/media/Manager.h"

#include <gl\gl.h>

/**
 * This class implements an OpenGL texture.
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
		 * @param camera Optional resulting camera profile associated with the recent frame; nullptr, if not of interest
		 * @return Frame type of the used frame
		 */
		FrameType update(SharedAnyCamera* camera = nullptr);

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
		 * @return The texture's frame medium
		 */
		Media::FrameMediumRef medium();

		/**
		 * Sets a new frame medium for this texture.
		 * @param medium Frame medium to set
		 */
		void setMedium(const Media::FrameMediumRef& medium);

	protected:

		/// OpenGL texture id.
		GLuint textureId_ = 0u;

		/// OpenGL texture pixel format.
		GLuint texturePixelFormat_ = 0u;

		/// Frame timestamp.
		Timestamp frameTimestamp_;

		/// Frame type.
		FrameType frameType_;

		/// Frame medium holding the image data.
		Media::FrameMediumRef frameMedium_;
};

#endif // META_OCEAN_APPLICATION_OCEAN_DEMO_RENDERING_OPENGL_WIN_TEXTURE_H
