/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/detector/bullseyes/triangulationsimulatorqt/MainWindow.h"

#include "ocean/rendering/Manager.h"

#include <QtCore/QTimer>

#include <QtWidgets/QBoxLayout>

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

	// Set up render timer (single-shot for on-demand rendering)
	renderTimer_ = new QTimer(this);
	renderTimer_->setSingleShot(true);
	connect(renderTimer_, &QTimer::timeout, this, &MainWindow::onRender);

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

void MainWindow::requestRender()
{
	if (!renderPending_)
	{
		renderPending_ = true;
		renderTimer_->start(0);  // Schedule for next event loop iteration
	}
}

void MainWindow::onRender()
{
	renderPending_ = false;

	if (framebuffer_.isNull())
	{
		return;
	}

	// Initialize scene on first render (when OpenGL context is ready)
	if (!sceneInitialized_)
	{
		sceneInitialized_ = scene_.initialize(engine_, framebuffer_);
	}

	// Render the scene
	framebuffer_->render();
}

}

}

}

}

}

}
