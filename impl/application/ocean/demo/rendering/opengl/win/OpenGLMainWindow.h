/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_RENDERING_OPENGL_WIN_MAIN_WINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_RENDERING_OPENGL_WIN_MAIN_WINDOW_H

#include "application/ocean/demo/rendering/opengl/win/OpenGL.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/math/Math.h"

#include "ocean/platform/win/ApplicationWindow.h"

#include "ocean/platform/gl/MediumTexture.h"
#include "ocean/platform/gl/WindowFramebuffer.h"

#include "ocean/platform/gl/win/WindowContext.h"

/**
 * This class implements a main window.
 * @ingroup applicationdemorenderingopenglwin
 */
class OpenGLMainWindow : public Platform::Win::ApplicationWindow
{
	public:

		/**
		 * Creates a new main window.
		 * @param instance Application instance
		 * @param name The name of the main window
		 */
		OpenGLMainWindow(HINSTANCE instance, const std::wstring& name);

		/**
		 * Destructs the main window.
		 */
		~OpenGLMainWindow() override;

	protected:

		/**
		 * Event function if the windows has been initialized successfully.
		 * @see Window::onInitialized().
		 */
		void onInitialized() override;

		/**
		 * Renders the scene.
		 */
		void render();

		/**
		 * Repaints the window.
		 * @see ApplicationWindow::onPaint().
		 */
		void onPaint() override;

		/**
		 * Function called by the windows message loop if the process is idling.
		 * @see ApplicationWindow::onIdle().
		 */
		void onIdle() override;

		/**
		 * Function for window resize event.
		 * @see ApplicationWindow::onResize().
		 */
		void onResize(const unsigned int clientWidth, const unsigned int clientHeight) override;

		/**
		 * Allows the modification of the window class before the registration.
		 * @see ApplicationWindow::modifyWindowClass().
		 */
		void modifyWindowClass(WNDCLASS& windowClass) override;

	protected:

		/// The OpenGL child window.
		Platform::GL::Win::WindowContext windowContext_;

		/// The OpenGL framebuffer.
		Platform::GL::WindowFramebuffer windowFramebuffer_;

		/// Texture object.
		Platform::GL::MediumTexture texture_;

		/// Horizontal field of view in deg.
		Scalar horizontalFieldOfView_ = Scalar(45);

		/// High performance timer.
		HighPerformanceTimer highPerformanceTimer_;

		/// Frame counter.
		unsigned int renderFrameCounter_ = 0u;

		/// Frame counter timestamp.
		double renderFrameCounterTimestamp_ = -1.0f;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_RENDERING_OPENGL_WIN_MAIN_WINDOW_H
