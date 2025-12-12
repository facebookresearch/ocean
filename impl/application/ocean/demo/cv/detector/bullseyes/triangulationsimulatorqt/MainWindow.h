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

#include <QtGui/QColor>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>

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
		 * Creates the configuration panel widget.
		 * @return The configuration panel widget
		 */
		QWidget* createConfigPanel();

		/**
		 * Collects current values from UI controls into a configuration.
		 * @return The current configuration
		 */
		SimulationConfig collectConfiguration();

		/**
		 * Updates the statistics display labels.
		 */
		void updateStatisticsDisplay();

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
		 * Slot called when any configuration parameter changes.
		 */
		void onConfigChanged();

		/**
		 * Slot for render timer timeout.
		 */
		void onRender();

		/**
		 * Slot called when any colorization parameter changes.
		 */
		void onColorizationChanged();

		/**
		 * Slot for choosing the low color.
		 */
		void onChooseLowColor();

		/**
		 * Slot for choosing the high color.
		 */
		void onChooseHighColor();

		/**
		 * Collects current colorization values from UI controls.
		 * @return The current colorization configuration
		 */
		ColorizationConfig collectColorizationConfig();

	protected:

		/// The rendering engine
		Rendering::EngineRef engine_;

		/// The window framebuffer
		Rendering::WindowFramebufferRef framebuffer_;

		/// The scene
		Scene scene_;

		/// Render timer
		QTimer* renderTimer_ = nullptr;

		// Depth range controls
		QDoubleSpinBox* frontDepthSpinBox_ = nullptr;
		QDoubleSpinBox* backDepthSpinBox_ = nullptr;
		QDoubleSpinBox* spacingSpinBox_ = nullptr;

		// Baseline control
		QDoubleSpinBox* baselineSpinBox_ = nullptr;

		// Left camera controls
		QComboBox* leftCameraTypeCombo_ = nullptr;
		QSpinBox* leftWidthSpinBox_ = nullptr;
		QSpinBox* leftHeightSpinBox_ = nullptr;
		QDoubleSpinBox* leftHfovSpinBox_ = nullptr;

		// Right camera controls
		QComboBox* rightCameraTypeCombo_ = nullptr;
		QSpinBox* rightWidthSpinBox_ = nullptr;
		QSpinBox* rightHeightSpinBox_ = nullptr;
		QDoubleSpinBox* rightHfovSpinBox_ = nullptr;

		// Simulation controls
		QCheckBox* randomizeCheckBox_ = nullptr;
		QComboBox* perturbationModeCombo_ = nullptr;
		QSpinBox* repetitionsSpinBox_ = nullptr;
		QDoubleSpinBox* deltaXSpinBox_ = nullptr;
		QDoubleSpinBox* deltaYSpinBox_ = nullptr;
		QLabel* repetitionsLabel_ = nullptr;
		QLabel* deltaXLabel_ = nullptr;
		QLabel* deltaYLabel_ = nullptr;

		// Statistics labels
		QLabel* meanErrorLabel_ = nullptr;
		QLabel* p50ErrorLabel_ = nullptr;
		QLabel* p90ErrorLabel_ = nullptr;
		QLabel* p95ErrorLabel_ = nullptr;
		QLabel* numPointsLabel_ = nullptr;

		// Colorization controls
		QDoubleSpinBox* minAngleSpinBox_ = nullptr;
		QDoubleSpinBox* maxAngleSpinBox_ = nullptr;
		QPushButton* lowColorButton_ = nullptr;
		QPushButton* highColorButton_ = nullptr;
		QColor lowColor_ = QColor(0, 255, 0);  // Green
		QColor highColor_ = QColor(255, 0, 0);  // Red

		/// Flag to prevent recursive updates
		bool updatingConfig_ = false;

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
