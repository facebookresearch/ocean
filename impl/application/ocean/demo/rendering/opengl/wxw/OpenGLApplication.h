// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_RENDERING_OPENGL_WXW_OPENGL_APPLICATION_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_RENDERING_OPENGL_WXW_OPENGL_APPLICATION_H

#include "application/ocean/demo/rendering/opengl/wxw/OpenGL.h"

/**
 * This class implements the application.
 * @ingroup applicationdemorenderingopenglwxw
 */
class OpenGLApplication : public wxApp
{
	private:

		/**
		 * Initialization event function.
		 * @return True, if succeeded
		 */
		bool OnInit() override;

		/**
		 * Exit event function.
		 * @return The return value of the base class implementation.
		 */
		int OnExit() override;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_RENDERING_OPENGL_WXW_OPENGL_APPLICATION_H
