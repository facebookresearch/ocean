/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_APPLICATION_OCEAN_DEMO_RENDERING_OPENGL_WIN_BACKGROUND_H
#define META_OCEAN_APPLICATION_OCEAN_DEMO_RENDERING_OPENGL_WIN_BACKGROUND_H

#include "application/ocean/demo/rendering/openglstereo/win/OpenGLStereo.h"
#include "application/ocean/demo/rendering/openglstereo/win/Texture.h"

#include "ocean/base/Frame.h"

#include <gl\gl.h>

/**
 * This class implements a background geometry.
 * @ingroup applicationdemorenderingopenglstereowin
 */
class Background
{
	public:

		/**
		 * Creates a new background geometry.
		 */
		Background() = default;

		/**
		 * Destructs a background geometry.
		 */
		~Background();

		/**
		 * Updates the background.
		 */
		void update();

		/**
		 * Draws the background.
		 */
		void draw();

		/**
		 * Releases the background.
		 */
		void release();

		/**
		 * Sets a new frame medium for this texture.
		 * @param medium Frame medium to set
		 */
		void setMedium(const Media::FrameMediumRef& medium);

		/**
		 * Configures the frame medium if possible.
		 * @param index Configuration index
		 */
		void configurateMedium(const unsigned int index);

	protected:

		/// Geometry display list.
		GLuint displayList_ = 0u;

		/// Texture object.
		Texture texture_;

		/// Frame type.
		FrameType frameType_;

		/// Frame camera.
		SharedAnyCamera frameCamera_;

		/// Medium url.
		std::string mediumUrl_;
};

#endif // META_OCEAN_APPLICATION_OCEAN_DEMO_RENDERING_OPENGL_WIN_BACKGROUND_H
