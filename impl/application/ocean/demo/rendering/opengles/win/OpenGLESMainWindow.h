/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_RENDERING_OPENGLES_WIN_MAIN_WINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_RENDERING_OPENGLES_WIN_MAIN_WINDOW_H

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/platform/win/ApplicationWindow.h"

#include <GLES3/gl3.h>
#include <egl/egl.h>

using namespace Ocean;

/**
 * This class implements a main window.
 * @ingroup applicationdemorenderingopengleswin
 */
class OpenGLESMainWindow : public Platform::Win::ApplicationWindow
{
	public:

		/**
		 * Creates a new main window.
		 * @param instance Application instance
		 * @param name name of the main window
		 */
		OpenGLESMainWindow(HINSTANCE instance, const std::wstring& name);

		/**
		 * Destructs the main window.
		 */
		virtual ~OpenGLESMainWindow();

	private:

		/**
		 * Renders the next frame.
		 */
		void render();

		/**
		 * Event function if the windows has been initialized successfully.
		 * @see Window::onInitialized().
		 */
		virtual void onInitialized();

		/**
		 * Function for a destroy event.
		 * @see ApplicationWindow::onDestroy().
		 */
		virtual void onDestroy();

		/**
		 * Repaints the window.
		 * @see ApplicationWindow::onPaint().
		 */
		virtual void onPaint();

		/**
		 * Function called by the windows message loop if the process is idling.
		 * @see ApplicationWindow::onIdle().
		 */
		virtual void onIdle();

		/**
		 * Function for window resize event.
		 * @see ApplicationWindow::onResize().
		 */
		virtual void onResize(const unsigned int clientWidth, const unsigned int clientHeight);

		/**
		 * Allows the modification of the window class before the registration.
		 * @see ApplicationWindow::modifyWindowClass().
		 */
		virtual void modifyWindowClass(WNDCLASS& windowClass);

		/**
		 * Initializes the egl context.
		 * @return True, if succeeded
		 */
		bool initializeContext();

		/**
		 * Releases the egl context.
		 */
		void releaseContext();

		/**
		 * Initializes opengl es.
		 */
		void initializeGL();

		/**
		 * Creates a shader program.
		 */
		void createShaderProgram();

		/**
		 * Releases the shader program.
		 */
		void releaseShaderProgram();

		/**
		 * Creates a vertex mesh.
		 */
		void createVertexMesh();

		/**
		 * Releases the vertex mesh.
		 */
		void releaseVertexMesh();

		/**
		 * Creates a texture.
		 */
		void createTexture();

		/**
		 * Releases the texture.
		 */
		void releaseTexture();

		/**
		 * Renders one triangle with self contained code.
		 */
		static void renderTriangle();

	private:

		/// EGL display object.
		EGLDisplay eglDisplay_;

		/// EGL config object.
		EGLConfig eglConfig_;

		/// EGL surface object.
		EGLSurface eglSurface_;

		/// EGL context object.
		EGLContext eglContext_;

		/// GLES shader program.
		GLuint glesShaderProgram_;

		/// GLES vertex shader.
		GLuint glesVertexShader_;

		/// GLES fragment shader.
		GLuint glesFragmentShader_;

		/// GLES vertex buffer object.
		GLuint glesVertexBufferObject_;

		/// GLES vertex buffer indices object.
		GLuint glesVertexBufferIndicesObject_;

		/// GLES vertex attribute location.
		GLuint glesVertexAttributeLocation_;

		/// GLES texture id.
		GLuint glesTextureId_;

		/// The performance object to determine the rendering execution time.
		HighPerformanceStatistic performance_;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_RENDERING_OPENGLES_WIN_MAIN_WINDOW_H
