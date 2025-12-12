/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_APPLICATION_OCEAN_DEMO_CV_DETECTOR_BULLSEYES_TRIANGULATION_SIMULATOR_QT_MAIN_WINDOW_H
#define META_APPLICATION_OCEAN_DEMO_CV_DETECTOR_BULLSEYES_TRIANGULATION_SIMULATOR_QT_MAIN_WINDOW_H

#include "application/ocean/demo/cv/detector/bullseyes/triangulationsimulatorqt/Scene.h"

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/WindowFramebuffer.h"

#include <QtWidgets/QMainWindow>

class QTimer;

namespace Ocean
{

namespace Demo
{

namespace CV
{

namespace Detector
{

namespace Bullseyes
{

namespace TriangulationSimulatorQt
{

/**
 * This class implements the main window of the triangulation simulator application.
 */
class MainWindow : public QMainWindow
{
	public:

		/**
		 * Creates a new main window.
		 * @param parent Optional parent widget
		 */
		explicit MainWindow(QWidget* parent = nullptr);

		/**
		 * Destructs the main window.
		 */
		~MainWindow() override;

	protected:

		/**
		 * Event function for mouse press events.
		 * @see QWidget::mousePressEvent().
		 */
		void mousePressEvent(QMouseEvent* event) override;

		/**
		 * Event function for mouse move events.
		 * @see QWidget::mouseMoveEvent().
		 */
		void mouseMoveEvent(QMouseEvent* event) override;

		/**
		 * Event function for mouse release events.
		 * @see QWidget::mouseReleaseEvent().
		 */
		void mouseReleaseEvent(QMouseEvent* event) override;

		/**
		 * Event function for mouse wheel events.
		 * @see QWidget::wheelEvent().
		 */
		void wheelEvent(QWheelEvent* event) override;

		/**
		 * Event function for resize events.
		 * @see QWidget::resizeEvent().
		 */
		void resizeEvent(QResizeEvent* event) override;

		/**
		 * Requests a render on the next frame.
		 * Call this whenever the scene needs to be redrawn.
		 */
		void requestRender();

		/**
		 * Slot for render timer timeout.
		 */
		void onRender();

	protected:

		/// The rendering engine
		Rendering::EngineRef engine_;

		/// The window framebuffer
		Rendering::WindowFramebufferRef framebuffer_;

		/// The scene
		Scene scene_;

		/// Render timer
		QTimer* renderTimer_ = nullptr;

		/// Flag to track if scene has been initialized
		bool sceneInitialized_ = false;

		/// Flag to track if a render is pending
		bool renderPending_ = false;
};

}

}

}

}

}

}

#endif // META_APPLICATION_OCEAN_DEMO_CV_DETECTOR_BULLSEYES_TRIANGULATION_SIMULATOR_QT_MAIN_WINDOW_H
