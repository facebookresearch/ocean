/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_RENDERING_RENDERER_WIN_RENDERER_MAINWINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_RENDERING_RENDERER_WIN_RENDERER_MAINWINDOW_H

#include "application/ocean/demo/rendering/renderer/win/RendererWin.h"

#include "ocean/platform/win/ApplicationWindow.h"

#include "ocean/rendering/Engine.h"

using namespace Ocean;

/**
 * This class implements a main window.
 * @ingroup applicationdemorenderingrendererwin
 */
class RendererWinMainWindow : public Platform::Win::ApplicationWindow
{
	public:

		/**
		 * Creates a new main window.
		 * @param instance Application instance
		 * @param name The name of the main window
		 */
		RendererWinMainWindow(HINSTANCE instance, const std::wstring& name);

		/**
		 * Destructs the main window.
		 */
		~RendererWinMainWindow() override;

		/**
		 * Initializes the application.
		 * @see MainWindow::initialize().
		 */
		bool initialize(const HICON icon = nullptr, const std::string& windowClass = std::string("window")) override;

	protected:

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
		 * Function for mouse move event.
		 * @see ApplicationWindow::onMouseMove().
		 */
		void onMouseMove(const MouseButton buttons, const int x, const int y) override;

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
		 * Functions for mouse wheel events.
		 * @see ApplicationWindow::onMouseWheel().
		 */
		void onMouseWheel(const MouseButton buttons, const int wheel, const int x, const int y) override;

		/**
		 * Allows the modification of the window class before the registration.
		 * @see ApplicationWindow::modifyWindowClass().
		 */
		void modifyWindowClass(WNDCLASS& windowClass) override;

	protected:

		/// Rendering engine.
		Rendering::EngineRef engine_;

		/// Rendering framebuffer.
		Rendering::WindowFramebufferRef framebuffer_;

		/// Old horizontal mouse cursor position.
		int oldCursorX_ = NumericT<int>::minValue();

		/// Old vertical mouse cursor position.
		int oldCursorY_ = NumericT<int>::minValue();
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_RENDERING_RENDERER_WIN_RENDERER_MAINWINDOW_H
