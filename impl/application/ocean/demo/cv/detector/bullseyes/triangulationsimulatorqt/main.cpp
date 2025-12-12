/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/detector/bullseyes/triangulationsimulatorqt/MainWindow.h"

#include "ocean/base/Messenger.h"

#include "ocean/rendering/glescenegraph/qt/QT.h"

#include <QtPlugin>
#include <QtWidgets/QApplication>

// Import static Qt platform plugins
#if defined(Q_OS_MACOS)
Q_IMPORT_PLUGIN(QCocoaIntegrationPlugin)
#elif defined(Q_OS_WIN)
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
#endif

using namespace Ocean;
using namespace Ocean::Demo::CV::Detector::Bullseyes::TriangulationSimulatorQt;

int main(int argc, char* argv[])
{
	// Create Qt application
	QApplication app(argc, argv);

	// Set messenger output
	Messenger::get().setOutputType(Messenger::OUTPUT_STANDARD);

	// Register Ocean GLES scene graph engine for Qt
	Rendering::GLESceneGraph::QT::registerGLESceneGraphEngine();

	int result = 0;

	{
		// Create and show main window
		MainWindow mainWindow;
		mainWindow.resize(1400, 800);
		mainWindow.show();

		result = app.exec();
	}

	// Unregister rendering engine
	Rendering::GLESceneGraph::unregisterGLESceneGraphEngine();

	return result;
}
