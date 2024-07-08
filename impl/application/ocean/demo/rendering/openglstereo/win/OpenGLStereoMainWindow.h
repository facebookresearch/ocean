/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_RENDERING_OPENGL_STEREO_WIN_OPENGL_STEREO_MAINWINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_RENDERING_OPENGL_STEREO_WIN_OPENGL_STEREO_MAINWINDOW_H

#include "application/ocean/demo/rendering/openglstereo/win/Background.h"

#include "ocean/platform/win/ApplicationWindow.h"

using namespace Ocean;

/**
 * This class implements a main window.
 * @ingroup applicationdemorenderingopenglstereowin
 */
class OpenGLStereoMainWindow : public Platform::Win::ApplicationWindow
{
	public:

		/**
		 * Creates a new main window.
		 * @param instance Application instance
		 * @param name The name of the main window
		 */
		OpenGLStereoMainWindow(HINSTANCE instance, const std::wstring& name);

		/**
		 * Destructs the main window.
		 */
		~OpenGLStereoMainWindow() override;

		/**
		 * Initializes the application.
		 * @see MainWindow::initialize().
		 */
		bool initialize(const HICON icon = nullptr, const std::string& windowClass = std::string("window")) override;

		/**
		 * Releases the OpenGL environment.
		 */
		void releaseOpenGL();

	protected:

		/**
		 * Initializes the OpenGL environment.
		 */
		void initializeOpenGL();

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
		 * Function for keyboard button down events.
		 * @see ApplicationWindow::onKeyDown().
		 */
		void onKeyDown(const int key) override;

		/**
		 * Function for mouse button down events.
		 * @see ApplicationWindow::onMouseDown().
		 */
		void onMouseDown(const MouseButton button, const int x, const int y) override;

		/**
		 * Function for keyboard button up events.
		 * @see ApplicationWindow::onKeyUp().
		 */
		void onKeyUp(const int key) override;

		/**
		 * Function for mouse button up events.
		 * @see ApplicationWindow::onMouseUp().
		 */
		void onMouseUp(const MouseButton button, const int x, const int y) override;

		/**
		 * Toggles the full screen stater.
		 */
		void onToggleFullscreen();

		/**
		 * Allows the modification of the window class before the registration.
		 * @see ApplicationWindow::modifyWindowClass().
		 */
		void modifyWindowClass(WNDCLASS& windowClass) override;

	protected:

		/// OpenGL render context.
		HGLRC windowRC_ = nullptr;

		/// Left background object.
		Background leftBackground_;

		/// Right background object.
		Background rightBackground_;

		/// Horizontal field of view in deg.
		Scalar horizontalFieldOfView_ = 30;

		/// Swap eye flag.
		bool swapBackground_ = false;

		/// Full-screen flag.
		bool isFullscreen_ = false;

		/// Non full-screen main window style.
		int mainWindowNonFullScreenStyle_ = 0;

		/// Left window position before full-screen mode.
		int mainWindowLastLeft_ = 0;

		/// Top window position before full-screen mode.
		int mainMindowLastTop_ = 0;

		/// Window width before full-screen mode.
		int mainWindowLastWidth_ = 0;

		/// Window height before full-screen mode.
		int mainWindowLastHeight_ = 0;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_RENDERING_OPENGL_STEREO_WIN_OPENGL_STEREO_MAINWINDOW_H
