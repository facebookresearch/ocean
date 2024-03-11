// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_QT_MAIN_WIDGET_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_QT_MAIN_WIDGET_H

#include "application/ocean/xrplayground/qt/XRPlaygroundQT.h"
#include "application/ocean/xrplayground/qt/LogDialog.h"

#include "application/ocean/xrplayground/common/XRPlaygroundCommon.h"

#include "metaonly/ocean/devices/vrs/DevicePlayer.h"

#include <QtWidgets/QMenuBar>
#include <QtWidgets/QWidget>

namespace Ocean
{

namespace XRPlayground
{

namespace QT
{

/**
 * This class implements the main widget of the XRPlayground app.
 * The main widget handles scene loading, interaction, rendering etc.
 * @ingroup xrplaygroundqt
 */
class MainWidget : public QWidget
{
	public:

		/**
		 * Creates a new main widget.
		 * @param content The initial scene file or experience to be loaded, can be empty
		 * @param vrsRecording The VRS recording to be loaded, can be empty
		 */
 		MainWidget(const std::string& content, const std::string& vrsRecording);

		/**
		 * Destructs this main widget and releases all resources.
		 */
		~MainWidget() override;

	protected:

		/**
		 * Sets up the menu bar.
		 * @return The resulting menu bar
		 */
		QMenuBar* setupMenu();

		/**
		 * Loads a new scene file.
		 * All previously loaded scene files are unloaded before.
		 * @param filename The filename of the new scene file to be loaded, must be valid
		 * @return True, if succeeded
		 */
		bool loadScene(const std::string& filename);

		/**
		 * Loads a VRS recording.
		 * @param filename The filename of the VRS recording to be loaded, must be valid
		 * @return True, if succeeded
		 */
		bool loadVRSRecording(const std::string& filename);

		/**
		 * Event function for mouse press events.
		 * @see QWidget::mousePressEvent().
		 */
		void mousePressEvent(QMouseEvent* qMouseEvent) override;

		/**
		 * Event function for mouse move events.
		 * @see QWidget::mouseMoveEvent().
		 */
		void mouseMoveEvent(QMouseEvent* qMouseEvent) override;

		/**
		 * Event function for mouse release events.
		 * @see QWidget::mouseReleaseEvent().
		 */
		void mouseReleaseEvent(QMouseEvent* qMouseEvent) override;

		/**
		 * Event function for mouse wheel events.
		 * @see QWidget::wheelEvent().
		 */
		void wheelEvent(QWheelEvent* qWheelEvent) override;

		/**
		 * This event handler is called when a drag is in progress and the mouse enters this widget.
		 * @see QWidget::dragEnterEvent().
		 */
		void dragEnterEvent(QDragEnterEvent* qDragEnterEvent) override;

		/**
		 * This event handler is called when the drag is dropped on this widget.
		 * @see QWidget::dropEvent().
		 */
		void dropEvent(QDropEvent* qDropEvent) override;

		/**
		 * This event handler is called when a key is pressed.
		 * @see QWidget::keyPressEvent().
		 */
		void keyPressEvent(QKeyEvent* qKeyEvent) override;

		/**
		 * This event handler is called when a key is released.
		 * @see QWidget::keyReleaseEvent().
		 */
		void keyReleaseEvent(QKeyEvent* qKeyEvent) override;

		/**
		 * Event function for the menu entry File, LoadScene.
		 */
		void onMenuLoadScene();

		/**
		 * Event function for the menu entry File, ReloadScene.
		 */
		void onMenuReloadScene();

		/**
		 * Event function for the menu entry File, CloseScene.
		 */
		void onMenuCloseScene();

		/**
		 * Event function for the menu entry Show Log.
		 */
		void onMenuShowLog();

		/**
		 * Event function for the menu entry Reset View.
		 */
		void onMenuResetView();

		/**
		 * Event function for the menu entry Fit View.
		 */
		void onMenuFitView();

		/**
		 * Event function for the menu entry Background.
		 */
		void onMenuBackground();

		/**
		 * Event function for the menu entry Background, Individual entry.
		 */
		void onMenuBackgroundEntryVRSRecording();

		/**
		 * Event function for the menu entry Background, Individual entry.
		 * @param checked True, if the meny entry is checked
		 */
		void onMenuBackgroundEntryTriggered(bool checked);

		/**
		 * Event function for the menu entry Recording, StartRecording.
		 */
		void onMenuRecordingStartRecording();

		/**
		 * Event function for the menu entry Recording, StopRecording.
		 */
		void onMenuRecordingStopRecording();

		/**
		 * Event function for the menu entry Screen, Rotate.
		 */
		void onMenuScreenRotate();

		/**
		 * Event function for the experience menu entries.
		 */
		void onMenuExperience();

		/**
		 * Event function to draw a new frame.
		 */
		void onRender();

		/**
		 * Returns the default view: in front of the origin looking towards negative Z.
		 */
		static HomogenousMatrix4 defaultView();

    protected:

		/// The platform independent code of the XRPlayground app.
    	XRPlaygroundCommon xrPlaygroundCommon_;

		/// The timer for render events.
		QTimer* renderTimer_;

		/// The last position of the mouse.
		Vector2 lastMousePosition_;

		/// The menu action to start a new recording.
		QAction* qActionStartRecording_;

		/// The menu action to stop a recording.
		QAction* qActionStopRecording_;

		/// The player for VRS recordings.
		Devices::VRS::DevicePlayer devicePlayer_;

		/// The log window.
		LogDialog* logDialog_;

		/// The last scene loaded
		std::string lastFilename_;
};

}

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_QT_MAIN_WIDGET_H
