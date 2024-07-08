/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_ORCA_WIN_MESSAGEWINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_ORCA_WIN_MESSAGEWINDOW_H

#include "application/ocean/orca/win/OrcaWin.h"

#include "ocean/base/Messenger.h"
#include "ocean/base/Timestamp.h"

namespace Ocean
{

namespace Orca
{

namespace Win
{

/**
 * This class implements a message window providing different categories of messages for the user.
 * @ingroup orcawin
 */
class MessageWindow : public CDockablePane
{
	protected:

		/**
		 * This class implements a message list box.
		 */
		class MessageList : public CListBox
		{
			public:

				/**
				 * Creates a new message list box.
				 * @param type Message type
				 */
				explicit MessageList(const Messenger::MessageType type);

				/**
				 * Destructs a message list box.
				 */
				~MessageList() override;

				/**
				 * Checks whether no message are available and adds them to the list box if so
				 */
				void checkForNewMessages();

				/**
				 * Returns the current number of messages.
				 * @return Number of messages
				 */
				inline unsigned int messages() const;

			protected:

				/**
				 * Runtime event message function declaration.
				 */
				DECLARE_MESSAGE_MAP();

			private:

				/// Message type handling by this list box.
				Messenger::MessageType messageType_;

				/// Most recent message.
				std::string recentMessage_;

				/// Number of identical most recent messages.
				unsigned int numberRecentMessages_ = 1u;

				/// Number of messages.
				unsigned int numberMessages_ = 0u;

				/// Timestamp of the last message.
				Timestamp timestamp_;

				/// Horizontal text extent.
				int horizontalExtent_ = 1;
		};

	public:

		/**
		 * Creates a new message window.
		 */
		MessageWindow();

		/**
		 * Destructs a message window.
		 */
		~MessageWindow() override;

		/**
		 * Checks whether no message are available and adds them to the output window if so
		 */
		void checkForNewMessages();

		/**
		 * Loads and sets configuration parameters like e.g. position and dimension.
		 */
		void applyConfiguration();

	protected:

		/**
		 * The GUI framework calls this function when an application requests a window creation.
		 * @param createStruct Create structure
		 * @return Creation result
		 */
		afx_msg int OnCreate(LPCREATESTRUCT createStruct);

		/**
		 * The GUI framework calls this function when the CWnd object is about to be hidden or shown.
		 * @param show Specifies whether a window is being shown or not.
		 * @param status Specifies the status of the window being shown.
		 */
		afx_msg void OnShowWindow(BOOL show, UINT status);

		/**
		 * The GUI framework calls this function after the CWnd object has been moved.
		 * @param left New left window position
		 * @param top New top window position
		 */
		afx_msg void OnMove(int left, int top);

		/**
		 * The GUI framework calls this function after the window's size has changed.
		 * @param type Of the size change
		 * @param width New client width
		 * @param height New client height
		 */
		afx_msg void OnSize(UINT type, int width, int height);

		/**
		 * Called by the GUI framework when the user presses the close button on the caption for the pane.
		 */
		virtual void OnPressCloseButton();

		/**
		 * Called by the GUI framework after a pane floats.
		 */
		virtual void OnAfterFloat();

		/**
		 * Called by the GUI framework when a pane has been docked.
		 * @param bar Unused parameter
		 * @param rect Unused parameter
		 * @param method Unused parameter
		 */
		virtual void OnAfterDock(CBasePane* bar, LPCRECT rect, AFX_DOCK_METHOD method);

		/**
		 * Runtime event message function declaration.
		 */
		DECLARE_MESSAGE_MAP();

	protected:

		/// List box for error messages.
		MessageList errors_;

		/// List box for warning messages.
		MessageList warnings_;

		/// List box for information messages.
		MessageList informations_;

		/// Number of error messages.
		unsigned int numberErrors_ = 0u;

		/// Number of warning messages.
		unsigned int numberWarnings_ = 0u;

		/// Number of information messages.
		unsigned int numberInformations_ = 0u;

		/// Text font for all message windows.
		CFont font_;

		/// Message tabs.
		CMFCTabCtrl	tabs_;

		/// Flag determines whether the configuration has been applied.
		bool configurationApplied_ = false;

		/// Definition of an invalid window value.
		static constexpr int invalidWindowValue_ = 2147483647;
};

inline unsigned int MessageWindow::MessageList::messages() const
{
	return numberMessages_;
}

}

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_ORCA_WIN_MESSAGEWINDOW_H
