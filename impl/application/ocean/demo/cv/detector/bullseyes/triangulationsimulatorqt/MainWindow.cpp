/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/detector/bullseyes/triangulationsimulatorqt/MainWindow.h"

#include "ocean/math/Numeric.h"

#include "ocean/rendering/Manager.h"

#include <QtCore/QTimer>

#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>

#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QColorDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QScrollArea>

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

MainWindow::MainWindow(QWidget* parent) :
	QMainWindow(parent)
{
	setWindowTitle("SSG Bullseye Triangulation Simulator");

	// Create central widget with horizontal layout
	QWidget* centralWidget = new QWidget(this);
	setCentralWidget(centralWidget);

	QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
	mainLayout->setContentsMargins(0, 0, 0, 0);

	// Add configuration panel on left
	QScrollArea* scrollArea = new QScrollArea();
	scrollArea->setWidget(createConfigPanel());
	scrollArea->setWidgetResizable(true);
	scrollArea->setFixedWidth(300);
	mainLayout->addWidget(scrollArea);

	// Get rendering engine
	engine_ = Rendering::Manager::get().engine();
	if (engine_.isNull())
	{
		return;
	}

	// Create window framebuffer
	framebuffer_ = engine_->createFramebuffer(Rendering::Framebuffer::FRAMEBUFFER_WINDOW);
	if (framebuffer_.isNull())
	{
		return;
	}

	// Initialize framebuffer with this window
	framebuffer_->initializeById(size_t(centralWidget));

	// Add framebuffer widget to layout
	QWidget* glWidget = dynamic_cast<QWidget*>(&*framebuffer_);
	if (glWidget)
	{
		glWidget->setMinimumSize(400, 400);
		mainLayout->addWidget(glWidget, 1);  // Stretch factor 1
	}

	// Note: Scene initialization is deferred until first render when OpenGL is ready

	// Create menu bar
	createMenuBar();

	// Set up render timer (single-shot for on-demand rendering)
	renderTimer_ = new QTimer(this);
	renderTimer_->setSingleShot(true);
	connect(renderTimer_, &QTimer::timeout, this, &MainWindow::onRender);

	// Initial update
	updateStatisticsDisplay();

	// Request initial render
	requestRender();
}

MainWindow::~MainWindow()
{
	if (renderTimer_)
	{
		renderTimer_->stop();
	}
}

QWidget* MainWindow::createConfigPanel()
{
	QWidget* panel = new QWidget();
	QVBoxLayout* layout = new QVBoxLayout(panel);

	// Depth Range group
	{
		QGroupBox* group = new QGroupBox("Depth Range");
		QFormLayout* form = new QFormLayout(group);

		frontDepthSpinBox_ = new QDoubleSpinBox();
		frontDepthSpinBox_->setRange(0.1, 5.0);
		frontDepthSpinBox_->setSingleStep(0.05);
		frontDepthSpinBox_->setValue(0.25);
		frontDepthSpinBox_->setSuffix(" m");
		form->addRow("Front Depth:", frontDepthSpinBox_);

		backDepthSpinBox_ = new QDoubleSpinBox();
		backDepthSpinBox_->setRange(0.1, 5.0);
		backDepthSpinBox_->setSingleStep(0.05);
		backDepthSpinBox_->setValue(1.0);
		backDepthSpinBox_->setSuffix(" m");
		form->addRow("Back Depth:", backDepthSpinBox_);

		spacingSpinBox_ = new QDoubleSpinBox();
		spacingSpinBox_->setRange(0.005, 0.2);
		spacingSpinBox_->setSingleStep(0.01);
		spacingSpinBox_->setValue(0.150);
		spacingSpinBox_->setDecimals(3);
		spacingSpinBox_->setSuffix(" m");
		form->addRow("Grid Spacing:", spacingSpinBox_);

		layout->addWidget(group);

		connect(frontDepthSpinBox_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onConfigChanged);
		connect(backDepthSpinBox_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onConfigChanged);
		connect(spacingSpinBox_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onConfigChanged);
	}

	// Camera Baseline group
	{
		QGroupBox* group = new QGroupBox("Camera Baseline");
		QFormLayout* form = new QFormLayout(group);

		baselineSpinBox_ = new QDoubleSpinBox();
		baselineSpinBox_->setRange(0.05, 0.30);
		baselineSpinBox_->setSingleStep(0.01);
		baselineSpinBox_->setValue(0.14);
		baselineSpinBox_->setDecimals(3);
		baselineSpinBox_->setSuffix(" m");
		form->addRow("Baseline:", baselineSpinBox_);

		layout->addWidget(group);

		connect(baselineSpinBox_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onConfigChanged);
	}

	// Cone Filter group
	{
		QGroupBox* group = new QGroupBox("Cone Filter");
		QFormLayout* form = new QFormLayout(group);

		useConeCheckBox_ = new QCheckBox();
		useConeCheckBox_->setChecked(false);
		form->addRow("Enable:", useConeCheckBox_);

		coneAngleSpinBox_ = new QDoubleSpinBox();
		coneAngleSpinBox_->setRange(0.0, 180.0);
		coneAngleSpinBox_->setSingleStep(1.0);
		coneAngleSpinBox_->setValue(25.0);
		coneAngleSpinBox_->setSuffix(" deg");
		form->addRow("Half-Angle:", coneAngleSpinBox_);

		layout->addWidget(group);

		connect(useConeCheckBox_, &QCheckBox::toggled, this, &MainWindow::onConfigChanged);
		connect(coneAngleSpinBox_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onConfigChanged);
	}

	// RGB Camera group
	{
		QGroupBox* group = new QGroupBox("Left Camera (Yellow)");
		QFormLayout* form = new QFormLayout(group);

		leftCameraTypeCombo_ = new QComboBox();
		leftCameraTypeCombo_->addItem("Pinhole", QVariant::fromValue((int)AnyCameraType::PINHOLE));
		leftCameraTypeCombo_->addItem("Fisheye", QVariant::fromValue((int)AnyCameraType::FISHEYE));
		leftCameraTypeCombo_->setCurrentIndex(0);  // Default: Pinhole
		form->addRow("Type:", leftCameraTypeCombo_);

		leftWidthSpinBox_ = new QSpinBox();
		leftWidthSpinBox_->setRange(100, 8000);
		leftWidthSpinBox_->setValue(2016);  // RGB camera default
		leftWidthSpinBox_->setSuffix(" px");
		form->addRow("Width:", leftWidthSpinBox_);

		leftHeightSpinBox_ = new QSpinBox();
		leftHeightSpinBox_->setRange(100, 8000);
		leftHeightSpinBox_->setValue(1512);  // RGB camera default
		leftHeightSpinBox_->setSuffix(" px");
		form->addRow("Height:", leftHeightSpinBox_);

		leftHfovSpinBox_ = new QDoubleSpinBox();
		leftHfovSpinBox_->setRange(1.0, 180.0);
		leftHfovSpinBox_->setSingleStep(1.0);
		leftHfovSpinBox_->setValue(100.0);  // RGB camera default (IMX681)
		leftHfovSpinBox_->setSuffix(" deg");
		form->addRow("HFOV:", leftHfovSpinBox_);

		layout->addWidget(group);

		connect(leftCameraTypeCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onConfigChanged);
		connect(leftWidthSpinBox_, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onConfigChanged);
		connect(leftHeightSpinBox_, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onConfigChanged);
		connect(leftHfovSpinBox_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onConfigChanged);
	}

	// CV Camera group
	{
		QGroupBox* group = new QGroupBox("Right Camera (Cyan)");
		QFormLayout* form = new QFormLayout(group);

		rightCameraTypeCombo_ = new QComboBox();
		rightCameraTypeCombo_->addItem("Pinhole", QVariant::fromValue((int)AnyCameraType::PINHOLE));
		rightCameraTypeCombo_->addItem("Fisheye", QVariant::fromValue((int)AnyCameraType::FISHEYE));
		rightCameraTypeCombo_->setCurrentIndex(1);  // Default: Fisheye (CV camera)
		form->addRow("Type:", rightCameraTypeCombo_);

		rightWidthSpinBox_ = new QSpinBox();
		rightWidthSpinBox_->setRange(100, 8000);
		rightWidthSpinBox_->setValue(400);  // CV camera default
		rightWidthSpinBox_->setSuffix(" px");
		form->addRow("Width:", rightWidthSpinBox_);

		rightHeightSpinBox_ = new QSpinBox();
		rightHeightSpinBox_->setRange(100, 8000);
		rightHeightSpinBox_->setValue(400);  // CV camera default
		rightHeightSpinBox_->setSuffix(" px");
		form->addRow("Height:", rightHeightSpinBox_);

		rightHfovSpinBox_ = new QDoubleSpinBox();
		rightHfovSpinBox_->setRange(1.0, 180.0);
		rightHfovSpinBox_->setSingleStep(1.0);
		rightHfovSpinBox_->setValue(119.0);  // CV camera default (OG0TD1B)
		rightHfovSpinBox_->setSuffix(" deg");
		form->addRow("HFOV:", rightHfovSpinBox_);

		layout->addWidget(group);

		connect(rightCameraTypeCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onConfigChanged);
		connect(rightWidthSpinBox_, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onConfigChanged);
		connect(rightHeightSpinBox_, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onConfigChanged);
		connect(rightHfovSpinBox_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onConfigChanged);
	}

	// Simulation group
	{
		QGroupBox* group = new QGroupBox("Simulation");
		QFormLayout* form = new QFormLayout(group);

		randomizeCheckBox_ = new QCheckBox();
		randomizeCheckBox_->setChecked(false);  // Default: fixed mode
		form->addRow("Randomize:", randomizeCheckBox_);

		perturbationModeCombo_ = new QComboBox();
		perturbationModeCombo_->addItem("Both Cameras", QVariant::fromValue(0));
		perturbationModeCombo_->addItem("Left Only", QVariant::fromValue(1));
		perturbationModeCombo_->addItem("Right Only", QVariant::fromValue(2));
		perturbationModeCombo_->setCurrentIndex(2);
		form->addRow("Perturb:", perturbationModeCombo_);

		repetitionsSpinBox_ = new QSpinBox();
		repetitionsSpinBox_->setRange(1, 1000);
		repetitionsSpinBox_->setSingleStep(10);
		repetitionsSpinBox_->setValue(100);
		form->addRow("Repetitions:", repetitionsSpinBox_);
		repetitionsLabel_ = (QLabel*)form->labelForField(repetitionsSpinBox_);

		deltaXSpinBox_ = new QDoubleSpinBox();
		deltaXSpinBox_->setRange(-100.0, 100.0);
		deltaXSpinBox_->setSingleStep(0.1);
		deltaXSpinBox_->setValue(0.0);
		deltaXSpinBox_->setSuffix(" px");
		form->addRow(QString::fromUtf8("\316\224x:"), deltaXSpinBox_);  // Δx (Greek uppercase delta)
		deltaXLabel_ = (QLabel*)form->labelForField(deltaXSpinBox_);

		deltaYSpinBox_ = new QDoubleSpinBox();
		deltaYSpinBox_->setRange(-100.0, 100.0);
		deltaYSpinBox_->setSingleStep(0.1);
		deltaYSpinBox_->setValue(0.0);
		deltaYSpinBox_->setSuffix(" px");
		form->addRow(QString::fromUtf8("\316\224y:"), deltaYSpinBox_);  // Δy (Greek uppercase delta)
		deltaYLabel_ = (QLabel*)form->labelForField(deltaYSpinBox_);

		layout->addWidget(group);

		// Lambda to update UI based on randomize state
		auto updateRandomizeUI = [this]()
		{
			const bool randomize = randomizeCheckBox_->isChecked();

			// Repetitions only visible in random mode
			repetitionsSpinBox_->setVisible(randomize);
			if (repetitionsLabel_)
			{
				repetitionsLabel_->setVisible(randomize);
			}

			// Update delta ranges and labels based on mode
			if (randomize)
			{
				deltaXSpinBox_->setRange(0.0, 100.0);
				deltaYSpinBox_->setRange(0.0, 100.0);
				if (deltaXLabel_)
				{
					deltaXLabel_->setText(QString::fromUtf8("\316\224x (\302\261):"));  // Δx (±):
				}
				if (deltaYLabel_)
				{
					deltaYLabel_->setText(QString::fromUtf8("\316\224y (\302\261):"));  // Δy (±):
				}
			}
			else
			{
				deltaXSpinBox_->setRange(-100.0, 100.0);
				deltaYSpinBox_->setRange(-100.0, 100.0);
				if (deltaXLabel_)
				{
					deltaXLabel_->setText(QString::fromUtf8("\316\224x:"));  // Δx:
				}
				if (deltaYLabel_)
				{
					deltaYLabel_->setText(QString::fromUtf8("\316\224y:"));  // Δy:
				}
			}
		};

		connect(randomizeCheckBox_, &QCheckBox::toggled, this, [this, updateRandomizeUI]()
		{
			updateRandomizeUI();
			onConfigChanged();
		});
		connect(perturbationModeCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onConfigChanged);
		connect(repetitionsSpinBox_, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onConfigChanged);
		connect(deltaXSpinBox_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onConfigChanged);
		connect(deltaYSpinBox_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onConfigChanged);

		// Apply initial UI state
		updateRandomizeUI();
	}

	// Statistics group
	{
		QGroupBox* group = new QGroupBox("Statistics (Angular Errors)");
		QFormLayout* form = new QFormLayout(group);

		meanErrorLabel_ = new QLabel("--");
		form->addRow("Mean Error:", meanErrorLabel_);

		p50ErrorLabel_ = new QLabel("--");
		form->addRow("P50 Error:", p50ErrorLabel_);

		p90ErrorLabel_ = new QLabel("--");
		form->addRow("P90 Error:", p90ErrorLabel_);

		p95ErrorLabel_ = new QLabel("--");
		form->addRow("P95 Error:", p95ErrorLabel_);

		numPointsLabel_ = new QLabel("--");
		form->addRow("Valid Points:", numPointsLabel_);

		layout->addWidget(group);
	}

	// Colorization group
	{
		QGroupBox* group = new QGroupBox("Colorization");
		QFormLayout* form = new QFormLayout(group);

		// Min angle spin box with low color button
		minAngleSpinBox_ = new QDoubleSpinBox();
		minAngleSpinBox_->setRange(0.0, 90.0);
		minAngleSpinBox_->setSingleStep(0.1);
		minAngleSpinBox_->setDecimals(2);
		minAngleSpinBox_->setSuffix(QString::fromUtf8("\u00B0"));  // degree symbol
		minAngleSpinBox_->setValue(0.0);
		connect(minAngleSpinBox_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onColorizationChanged);

		lowColorButton_ = new QPushButton();
		lowColorButton_->setStyleSheet(QString("background-color: %1").arg(lowColor_.name()));
		lowColorButton_->setFixedWidth(40);
		connect(lowColorButton_, &QPushButton::clicked, this, &MainWindow::onChooseLowColor);

		QHBoxLayout* minAngleLayout = new QHBoxLayout();
		minAngleLayout->setContentsMargins(0, 0, 0, 0);
		minAngleLayout->addWidget(minAngleSpinBox_);
		minAngleLayout->addWidget(lowColorButton_);
		QWidget* minAngleWidget = new QWidget();
		minAngleWidget->setLayout(minAngleLayout);
		form->addRow(minAngleWidget);

		// Max angle spin box with high color button
		maxAngleSpinBox_ = new QDoubleSpinBox();
		maxAngleSpinBox_->setRange(0.01, 90.0);
		maxAngleSpinBox_->setSingleStep(0.1);
		maxAngleSpinBox_->setDecimals(2);
		maxAngleSpinBox_->setSuffix(QString::fromUtf8("\u00B0"));  // degree symbol
		maxAngleSpinBox_->setValue(0.5);
		connect(maxAngleSpinBox_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onColorizationChanged);

		highColorButton_ = new QPushButton();
		highColorButton_->setStyleSheet(QString("background-color: %1").arg(highColor_.name()));
		highColorButton_->setFixedWidth(40);
		connect(highColorButton_, &QPushButton::clicked, this, &MainWindow::onChooseHighColor);

		QHBoxLayout* maxAngleLayout = new QHBoxLayout();
		maxAngleLayout->setContentsMargins(0, 0, 0, 0);
		maxAngleLayout->addWidget(maxAngleSpinBox_);
		maxAngleLayout->addWidget(highColorButton_);
		QWidget* maxAngleWidget = new QWidget();
		maxAngleWidget->setLayout(maxAngleLayout);
		form->addRow(maxAngleWidget);

		layout->addWidget(group);
	}

	layout->addStretch();

	return panel;
}

void MainWindow::createMenuBar()
{
	QMenuBar* menuBar = new QMenuBar(this);
	setMenuBar(menuBar);

	// View menu
	QMenu* viewMenu = menuBar->addMenu("View");

	// Camera submenu
	QMenu* cameraMenu = viewMenu->addMenu("Camera");
	cameraMenu->addAction("Reset", this, &MainWindow::onResetCamera, tr("Ctrl+0"));
	cameraMenu->addSeparator();
	cameraMenu->addAction("Front", this, &MainWindow::onCameraFront, tr("Ctrl+1"));
	cameraMenu->addAction("Back", this, &MainWindow::onCameraBack, tr("Ctrl+2"));
	cameraMenu->addSeparator();
	cameraMenu->addAction("Left", this, &MainWindow::onCameraLeft, tr("Ctrl+3"));
	cameraMenu->addAction("Right", this, &MainWindow::onCameraRight, tr("Ctrl+4"));
	cameraMenu->addSeparator();
	cameraMenu->addAction("Top", this, &MainWindow::onCameraTop, tr("Ctrl+5"));
	cameraMenu->addAction("Bottom", this, &MainWindow::onCameraBottom, tr("Ctrl+6"));
	cameraMenu->addSeparator();
	orbitingAction_ = cameraMenu->addAction("Orbiting", this, &MainWindow::onCameraOrbiting, tr("Ctrl+7"));
	orbitingAction_->setCheckable(true);

	// Projection submenu
	QMenu* projectionMenu = viewMenu->addMenu("Projection");
	perspectiveAction_ = projectionMenu->addAction("Perspective", this, &MainWindow::onProjectionPerspective, tr("Ctrl+8"));
	perspectiveAction_->setCheckable(true);
	perspectiveAction_->setChecked(true);  // Default to perspective
	orthogonalAction_ = projectionMenu->addAction("Orthogonal", this, &MainWindow::onProjectionOrthogonal, tr("Ctrl+9"));
	orthogonalAction_->setCheckable(true);
}

SimulationConfig MainWindow::collectConfiguration()
{
	SimulationConfig config;

	config.frontDepthMeters = Scalar(frontDepthSpinBox_->value());
	config.backDepthMeters = Scalar(backDepthSpinBox_->value());
	config.spacingMeters = Scalar(spacingSpinBox_->value());
	config.baselineMeters = Scalar(baselineSpinBox_->value());

	// Left camera configuration
	config.leftCamera.type = (AnyCameraType)leftCameraTypeCombo_->currentData().toInt();
	config.leftCamera.width = (unsigned int)leftWidthSpinBox_->value();
	config.leftCamera.height = (unsigned int)leftHeightSpinBox_->value();
	config.leftCamera.hfovDegrees = Scalar(leftHfovSpinBox_->value());

	// Right camera configuration
	config.rightCamera.type = (AnyCameraType)rightCameraTypeCombo_->currentData().toInt();
	config.rightCamera.width = (unsigned int)rightWidthSpinBox_->value();
	config.rightCamera.height = (unsigned int)rightHeightSpinBox_->value();
	config.rightCamera.hfovDegrees = Scalar(rightHfovSpinBox_->value());

	config.randomize = randomizeCheckBox_->isChecked();
	config.perturbationMode = (PerturbationMode)perturbationModeCombo_->currentIndex();
	config.numRepetitions = (unsigned int)repetitionsSpinBox_->value();
	config.deltaX = Scalar(deltaXSpinBox_->value());
	config.deltaY = Scalar(deltaYSpinBox_->value());

	config.useConeFilter = useConeCheckBox_->isChecked();
	config.coneHalfAngleDegrees = Scalar(coneAngleSpinBox_->value());

	return config;
}

void MainWindow::updateStatisticsDisplay()
{
	const SimulationStats& stats = scene_.statistics();

	// Error values are in radians, convert to degrees for display
	meanErrorLabel_->setText(QString("%1 deg").arg(Numeric::rad2deg(stats.meanError), 0, 'f', 4));
	p50ErrorLabel_->setText(QString("%1 deg").arg(Numeric::rad2deg(stats.p50Error), 0, 'f', 4));
	p90ErrorLabel_->setText(QString("%1 deg").arg(Numeric::rad2deg(stats.p90Error), 0, 'f', 4));
	p95ErrorLabel_->setText(QString("%1 deg").arg(Numeric::rad2deg(stats.p95Error), 0, 'f', 4));
	numPointsLabel_->setText(QString::number(stats.numValidPoints));
}

void MainWindow::mousePressEvent(QMouseEvent* event)
{
	if (!sceneInitialized_)
	{
		return;
	}

	// Only handle mouse events over the OpenGL widget
	QWidget* glWidget = dynamic_cast<QWidget*>(&*framebuffer_);
	if (glWidget)
	{
		QPoint globalPos = event->globalPos();
		QRect glRect = glWidget->geometry();
		glRect.moveTopLeft(glWidget->parentWidget()->mapToGlobal(glRect.topLeft()));

		if (!glRect.contains(globalPos))
		{
			event->ignore();
			return;
		}
	}

	int button = 0;
	if (event->button() == Qt::LeftButton)
	{
		button = 1;
	}
	else if (event->button() == Qt::RightButton)
	{
		button = 2;
	}

	scene_.handleMousePress(button, Vector2(Scalar(event->pos().x()), Scalar(event->pos().y())));
	requestRender();
}

void MainWindow::mouseMoveEvent(QMouseEvent* event)
{
	if (!sceneInitialized_)
	{
		return;
	}

	// Only handle mouse events over the OpenGL widget
	QWidget* glWidget = dynamic_cast<QWidget*>(&*framebuffer_);
	if (glWidget)
	{
		QPoint globalPos = event->globalPos();
		QRect glRect = glWidget->geometry();
		glRect.moveTopLeft(glWidget->parentWidget()->mapToGlobal(glRect.topLeft()));

		if (!glRect.contains(globalPos))
		{
			event->ignore();
			return;
		}
	}

	int buttons = 0;
	if (event->buttons() & Qt::LeftButton)
	{
		buttons |= 1;
	}
	if (event->buttons() & Qt::RightButton)
	{
		buttons |= 2;
	}

	scene_.handleMouseMove(Vector2(Scalar(event->pos().x()), Scalar(event->pos().y())), buttons);

	// Only request render if buttons are pressed (dragging)
	if (buttons != 0)
	{
		requestRender();
	}
}

void MainWindow::mouseReleaseEvent(QMouseEvent* /*event*/)
{
	// Nothing to do
}

void MainWindow::wheelEvent(QWheelEvent* event)
{
	if (!sceneInitialized_)
	{
		return;
	}

	// Only handle wheel events over the OpenGL widget
	QWidget* glWidget = dynamic_cast<QWidget*>(&*framebuffer_);
	if (glWidget)
	{
		// Check if the event position is within the GL widget's geometry
		QPoint globalPos = event->globalPos();
		QRect glRect = glWidget->geometry();
		glRect.moveTopLeft(glWidget->parentWidget()->mapToGlobal(glRect.topLeft()));

		if (!glRect.contains(globalPos))
		{
			// Let the event propagate to child widgets (e.g., spinboxes)
			event->ignore();
			return;
		}
	}

	scene_.handleMouseWheel(event->angleDelta().y());
	requestRender();
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
	QMainWindow::resizeEvent(event);
	requestRender();
}

void MainWindow::requestRender()
{
	if (!renderPending_ && renderTimer_)
	{
		renderPending_ = true;
		renderTimer_->start(0);  // Schedule render on next event loop iteration
	}
}

void MainWindow::onConfigChanged()
{
	if (updatingConfig_ || !sceneInitialized_)
	{
		return;
	}

	const SimulationConfig config = collectConfiguration();
	scene_.updateConfiguration(config);
	updateStatisticsDisplay();
	requestRender();
}

void MainWindow::onRender()
{
	renderPending_ = false;

	if (framebuffer_)
	{
		// Initialize scene on first render when OpenGL context is ready
		if (!sceneInitialized_)
		{
			sceneInitialized_ = true;
			scene_.initialize(engine_, framebuffer_);

			// Apply initial configuration from UI (struct defaults may differ)
			const SimulationConfig config = collectConfiguration();
			scene_.updateConfiguration(config);

			updateStatisticsDisplay();
		}

		// Update orbiting animation if active
		if (scene_.isOrbiting())
		{
			scene_.updateOrbiting();
			// Request another render for continuous orbiting animation
			requestRender();
		}

		// Update orbiting action checkmark
		if (orbitingAction_ && orbitingAction_->isChecked() != scene_.isOrbiting())
		{
			orbitingAction_->setChecked(scene_.isOrbiting());
		}

		framebuffer_->render();
	}
}

void MainWindow::onResetCamera()
{
	if (sceneInitialized_)
	{
		scene_.resetCamera();
		requestRender();
	}
}

void MainWindow::onCameraTop()
{
	if (sceneInitialized_)
	{
		scene_.setCameraTop();
		requestRender();
	}
}

void MainWindow::onCameraBottom()
{
	if (sceneInitialized_)
	{
		scene_.setCameraBottom();
		requestRender();
	}
}

void MainWindow::onCameraLeft()
{
	if (sceneInitialized_)
	{
		scene_.setCameraLeft();
		requestRender();
	}
}

void MainWindow::onCameraRight()
{
	if (sceneInitialized_)
	{
		scene_.setCameraRight();
		requestRender();
	}
}

void MainWindow::onCameraFront()
{
	if (sceneInitialized_)
	{
		scene_.setCameraFront();
		requestRender();
	}
}

void MainWindow::onCameraBack()
{
	if (sceneInitialized_)
	{
		scene_.setCameraBack();
		requestRender();
	}
}

void MainWindow::onCameraOrbiting()
{
	if (sceneInitialized_)
	{
		scene_.setOrbiting(!scene_.isOrbiting());
		requestRender();
	}
}

void MainWindow::onProjectionPerspective()
{
	if (sceneInitialized_)
	{
		scene_.setProjectionMode(ProjectionMode::PERSPECTIVE);
		perspectiveAction_->setChecked(true);
		orthogonalAction_->setChecked(false);
		requestRender();
	}
}

void MainWindow::onProjectionOrthogonal()
{
	if (sceneInitialized_)
	{
		scene_.setProjectionMode(ProjectionMode::ORTHOGONAL);
		perspectiveAction_->setChecked(false);
		orthogonalAction_->setChecked(true);
		requestRender();
	}
}

void MainWindow::onColorizationChanged()
{
	if (updatingConfig_ || !sceneInitialized_)
	{
		return;
	}

	const ColorizationConfig config = collectColorizationConfig();
	scene_.updateColorization(config);
	requestRender();
}

void MainWindow::onChooseLowColor()
{
	QColor color = QColorDialog::getColor(lowColor_, this, "Choose Low Color");
	if (color.isValid())
	{
		lowColor_ = color;
		lowColorButton_->setStyleSheet(QString("background-color: %1").arg(lowColor_.name()));
		onColorizationChanged();
	}
}

void MainWindow::onChooseHighColor()
{
	QColor color = QColorDialog::getColor(highColor_, this, "Choose High Color");
	if (color.isValid())
	{
		highColor_ = color;
		highColorButton_->setStyleSheet(QString("background-color: %1").arg(highColor_.name()));
		onColorizationChanged();
	}
}

ColorizationConfig MainWindow::collectColorizationConfig()
{
	ColorizationConfig config;

	config.minAngleDegrees = Scalar(minAngleSpinBox_->value());
	config.maxAngleDegrees = Scalar(maxAngleSpinBox_->value());
	config.lowColor = RGBAColor(lowColor_.redF(), lowColor_.greenF(), lowColor_.blueF());
	config.highColor = RGBAColor(highColor_.redF(), highColor_.greenF(), highColor_.blueF());

	return config;
}

}

}

}

}

}

}
