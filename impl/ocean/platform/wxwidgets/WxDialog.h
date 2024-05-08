/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_WXWIDGETS_WX_DIALOG_H
#define META_OCEAN_PLATFORM_WXWIDGETS_WX_DIALOG_H

#include "ocean/platform/wxwidgets/WxWidgets.h"

#include "ocean/base/Lock.h"
#include "ocean/base/ObjectRef.h"
#include "ocean/base/Timestamp.h"

#include <wx/dialog.h>
#include <wx/evtloop.h>

namespace Ocean
{

namespace Platform
{

namespace WxWidgets
{

/**
 * This class implements a modal dialog for WxWidgets which in addition to the standard wxDialog implementation forces the modal-event-loop to stop if the dialog ends (the modal state).
 * @ingroup platformwxwidgets
 */
class OCEAN_PLATFORM_WXWIDGETS_EXPORT WxDialog : public wxDialog
{
	protected:

#ifdef _WINDOWS

		/**
		 * This class implements an event loop.
		 */
		class OCEAN_PLATFORM_WXWIDGETS_EXPORT EventLoop : public wxModalEventLoop
		{
			public:

				/**
				 * Creates a new event loop object.
				 * @param winModal The window which will be owner of the event loop
				 */
				explicit EventLoop(wxWindow *winModal);

				/**
				 * Returns whether any event is available.
				 * @return True, if so
				 */
				virtual bool Pending() const;

				/**
				 * Exits this currently running loop with the given exit code.
				 * @param returnCode The return code
				 */
				virtual void Exit(int returnCode = 0);

			protected:

				/// The timestamp this event loop has been exited.
				Timestamp exitTimestamp_;
		};

		/**
		 * Definition of an object reference for event loop objects.
		 */
		typedef ObjectRef<EventLoop> EventLoopRef;

#endif

	public:

		/**
		 * Creates a new dialog object.
		 * @param parent The parent window, may be nullptr
		 * @param id The id of the new dialog
		 * @param title The title of the new dialog
		 * @param pos Horizontal and vertical position of the new dialog, in pixel with range (-infinity, infinity)x(-infinity, infinity)
		 * @param size The width and height of the new dialog, in pixel with range [0, infinity)x[0, infinity)
		 * @param style The style of the new dialog
		 * @param name The optional name of the new dialog
		 */
		explicit WxDialog(wxWindow *parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE, const wxString& name = wxDialogNameStr);

		/**
		 * Destructs a dialog object.
		 */
		~WxDialog() override;

#ifdef OCEAN_PLATFORM_BUILD_WINDOWS

		/**
		 * Returns whether this dialog is currently showing modally.
		 * @return True, if so
		 */
		virtual bool IsModal() const;

		/**
		 * Shows or hides this dialog (not modally).
		 * @param show True, to show the dialog; False, to hide the dialog
		 * @return True, if succeeded
		 */
		virtual bool Show(bool show = true);

		/**
		 * Shows this dialog modally.
		 * @return The return code of the modal call
		 */
		virtual int ShowModal();

		/**
		 * Ends the modally shown dialog.
		 * @param retCode The return code of the modal call
		 */
		virtual void EndModal(int retCode);

	protected:

		/// The event loop of this dialog for modal executions.
		EventLoopRef eventLoop_;

		/// The lock for the event loop.
		Lock eventLoopLock_;

#endif // OCEAN_PLATFORM_BUILD_WINDOWS

		/// True, if the modal execution of this dialog ends.
		bool endModalCalled_ = false;
};

}

}

}

#endif // META_OCEAN_PLATFORM_WXWIDGETS_WX_DIALOG_H
