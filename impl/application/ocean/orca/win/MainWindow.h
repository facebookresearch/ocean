/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_ORCA_WIN_MAINWINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_ORCA_WIN_MAINWINDOW_H

#include "application/ocean/orca/win/OrcaWin.h"
#include "application/ocean/orca/win/MessageWindow.h"
#include "application/ocean/orca/win/PropertiesWindow.h"
#include "application/ocean/orca/win/View.h"

#include "application/ocean/orca/ContentManager.h"

#include "ocean/base/ApplicationInterface.h"
#include "ocean/base/Caller.h"

#include "ocean/math/Quaternion.h"
#include "ocean/math/Vector3.h"

#include <queue>
#include <vector>

namespace Ocean
{

namespace Orca
{

namespace Win
{

/**
 * This class implements the main window of the ocean orca viewer.
 * @ingroup orcawin
 */
class MainWindow : public CFrameWndEx
{
	public:

		/**
		 * Definition of a vector holding filenames.
		 */
		typedef std::vector<std::string> Filenames;

	private:

		/**
		 * Definition of a caller without return type.
		 */
		typedef Caller<void> ApplicationEventCaller;

		/**
		 * Definition of a queue holding application event caller objects.
		 */
		typedef std::queue<ApplicationEventCaller> ApplicationEventCallers;

		/**
		 * Definition of a vector holding pairs of content ids and event ids.
		 */
		typedef std::vector< std::pair<ContentManager::ContentId, ApplicationInterface::EventId> > ContentEventIds;

		/// Definition of an invalid window value.
		static constexpr int invalidWindowValue_ = 2147483647;

	public:

		/**
		 * Creates a new main window object.
		 */
		MainWindow();

		/**
		 * Destructs a main window object.
		 */
		~MainWindow() override;

		/**
		 * Returns the main view of this main windows.
		 * @return Main view
		 */
		inline View& mainView();

		/**
		 * Loads or adds scene files.<br>
		 * It can be selected whether the first file will replace existing scenes or not.<br>
		 * However, following files will be added only and therefore will not replay any scenes.
		 * @param filenames Names of the file to load or add
		 * @param add True, if the first file has to be added, False if the first file will replace old scenes
		 * @return Content ids, one for each file
		 */
		ContentManager::ContentIds loadFiles(const Filenames& filenames, const bool add = false);

		/**
		 * Sets the dimension column of the status bar.
		 * @param width Window width in pixel
		 * @param height Window height in pixel
		 */
		void setStatusBarSize(const unsigned int width, const unsigned int height);

		/**
		 * Sets the position and orientation column of the status bar.
		 * @param position Current position
		 * @param orientation Current orientation
		 */
		void setStatusBarPosition(const Ocean::Vector3& position, const Ocean::Quaternion& orientation);

		/**
		 * Sets the progress column of the status bar.
		 * @param progress The progress state to set with range [0, 1]
		 */
		void setStatusBarProgress(const float progress);

		/**
		 * Sets the framerate column of the status bar.
		 * @param fps Framerate in Hz
		 */
		 void setStatusBarFramerate(const float fps);

		/**
		 * Called if the application is in idle state.
		 * @return False, to stop idle handling
		 */
		bool onIdle();

		/**
		 * The GUI framework calls this function to toggle the fullscreen state of this main window.
		 */
		void onToggleFullscreen();

		/**
		 * Returns whether the main window is in fullscreen mode.
		 * @return True, if so
		 */
		inline bool fullscreen() const;

		/**
		 * Loads and applies configuration parameters like e.g. position and dimension.
		 */
		void applyConfiguration();

		/**
		 * Stores configuration parameters.
		 */
		void storeConfiguration();

		/**
		 * Returns the properties window of the main window.
		 * @return Property window
		 */
		inline PropertiesWindow& propertiesWindow();

		/**
		 * Returns the main window.
		 * @return Main window
		 */
		static MainWindow& mainWindow();

	private:

		/**
		 * Called by the GUI framework before the creation of the Windows window attached to this CWnd object.
		 * @param createStruct Structure holding informations about the behavior and appearance of the window to create
		 * @return True, if succeeded
		 */
		BOOL PreCreateWindow(CREATESTRUCT& createStruct) override;

		/**
		 * Called by the GUI framework to route and dispatch command messages and to handle the update of command user-interface objects.
		 * @param id Command id
		 * @param code Command notification code
		 * @param extra The extra data according to the value of code
		 * @param handlerInfo Optional handling information which is normally not used and therefore nullptr
		 * @return True, if succeeded
		 */
		BOOL OnCmdMsg(UINT id, int code, void* extra, AFX_CMDHANDLERINFO* handlerInfo) override;

		/**
		 * The GUI framework calls this function when an application requests a window creation.
		 * @param createStruct Create structure
		 * @return Creation result
		 */
		afx_msg int OnCreate(LPCREATESTRUCT createStruct);

		/**
		 * The GUI framework calls this function after the CWnd object has been moved.
		 * @param left New left window position
		 * @param top New top window position
		 */
		afx_msg void OnMove(int left, int top);

		/**
		 * The GUI framework calls this function after the window's size has changed.
		 * @param type Window size change type
		 * @param width New window width
		 * @param height New window height
		 */
		afx_msg void OnSize(UINT type, int width, int height);

		/**
		 * The GUI framework calls this function as a signal that the CWnd or an application is to terminate.
		 */
		afx_msg void OnClose();

		/**
		 * The GUI framework calls this function after gaining the input focus.
		 * @param lastWindow Window that loses the focus state
		 */
		afx_msg void OnSetFocus(CWnd* lastWindow);

		/**
		 * The GUI framework calls this function when the user releases the left mouse button over a window that has registered itself as the recipient of dropped files.
		 * @param info Internal data structure that describes the dropped files
		 */
		afx_msg void OnDropFiles(HDROP info);

		/**
		 * The GUI framework calls this function to toggle the properties window.
		 */
		void onTogglePropertiesWindow();

		/**
		 * Toggles the visible state of the message output window.
		 */
		void onToggleMessageWindow();

		/**
		 * Toggles the visible state of the status bar.
		 */
		void onToggleStatusBar();

		/**
		 * Event function for loaded content events.
		 * @param contentId Id of the content loaded
		 * @param state The state determining whether the content has been loaded successfully
		 */
		void onContentLoaded(const ContentManager::ContentId contentId, const bool state);

		/**
		 * Event function for unloaded content events.
		 * @param contentId Id of the content unloaded
		 * @param state The state determining whether the content has been unloaded successfully
		 */
		void onContentUnloaded(const ContentManager::ContentId contentId, const bool state);

		/**
		 * Returns the file menu toolbar object.
		 * @return File menu object
		 */
		CMFCToolBarMenuButton* fileMenu();

		/**
		 * Return the window menu toolbar object.
		 * @return Window menu object
		 */
		CMFCToolBarMenuButton* windowMenu();

		/**
		 * Sets the checked state of the menu message window entry.
		 * @param state Checked state to set
		 */
		void setMenuMessageWindowChecked(const bool state);

		/**
		 * Sets the checked state of the menu status bar entry.
		 * @param state Checked state to set
		 */
		void setMenuStatusBarChecked(const bool state);

		/**
		 * Application interface event function for content add callbacks.
		 * @param content The content to be loaded
		 * @return Corresponding content event id
		 */
		ApplicationInterface::EventIds onApplicationInterfaceContentAdd(const ApplicationInterface::StringVector& content);

		/**
		 * Application interface event function for content remove callbacks.
		 * @param content The content to be removed
		 * @return Corresponding content event id
		 */
		ApplicationInterface::EventIds onApplicationInterfaceContentRemove(const ApplicationInterface::StringVector& content);

		/**
		 * Application interface function for content add events.
		 * @param content The content to be added
		 * @param eventIds Corresponding content event ids
		 */
		void applicationInterfaceContentAdd(const ApplicationInterface::StringVector content, const ApplicationInterface::EventIds eventIds);

		/**
		 * Application interface function for content remove events.
		 * @param content The content to be removed
		 * @param eventIds Corresponding content event ids
		 */
		void applicationInterfaceContentRemove(const ApplicationInterface::StringVector content, const ApplicationInterface::EventIds eventIds);

		/**
		 * Application interface function for content added events.
		 * @param contentId Id of the added content
		 * @param state Added state
		 */
		void applicationInterfaceContentAdded(const ContentManager::ContentId contentId, const bool state);

		/**
		 * Application interface function for content removed events.
		 * @param contentId Id of the removed content
		 * @param state Removed state
		 */
		void applicationInterfaceContentRemoved(const ContentManager::ContentId contentId, const bool state);

		/**
		 * Runtime event message function declaration.
		 */
		DECLARE_MESSAGE_MAP();

	private:

		/// Dockable message window providing error, warning or information message.
		MessageWindow messageWindow_;

		/// Main window view.
		View view_;

		/// Properties window.
		PropertiesWindow propertiesWindow_;

		/// Main window menu bar.
		CMFCMenuBar nenuBar_;

		/// Main window status bar.
		CMFCStatusBar statusBar_;

		/// Flag determines whether the main window is in full screen state.
		bool isFullscreen_ = false;

		/// Non fullscreen main window style.
		int nonFullScreenStyle_ = 0;

		/// Flag determines whether the configuration has been applied.
		bool configurationApplied_ = false;;

		/// State determining whether this window has been started to release.
		bool released_ = false;

		/// Application interface events.
		ApplicationEventCallers applicationEvents_;

		/// Vector holding the content ids and event ids.
		ContentEventIds contentEventIds_;

		/// Counter of content events.
		unsigned int contentEventCounter_ = 0;

		/// Application interface events lock.
		Lock applicationEventLock_;
};

inline PropertiesWindow& MainWindow::propertiesWindow()
{
	return propertiesWindow_;
}

inline View& MainWindow::mainView()
{
	return view_;
}

inline bool MainWindow::fullscreen() const
{
	return isFullscreen_;
}

}

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_ORCA_WIN_MAINWINDOW_H
