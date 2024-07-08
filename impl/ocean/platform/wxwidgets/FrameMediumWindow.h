/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_WXWIDGETS_FRAME_MEDIUM_WINDOW_H
#define META_OCEAN_PLATFORM_WXWIDGETS_FRAME_MEDIUM_WINDOW_H

#include "ocean/platform/wxwidgets/FrameWindow.h"

#include "ocean/base/Callback.h"
#include "ocean/base/Lock.h"
#include "ocean/base/Thread.h"

#include "ocean/media/FrameMedium.h"

namespace Ocean
{

namespace Platform
{

namespace WxWidgets
{

/**
 * This class implements the main window.
 * @ingroup platformwxw
 */
class OCEAN_PLATFORM_WXWIDGETS_EXPORT FrameMediumWindow :
	public FrameWindow,
	protected Thread
{
	public:

		/**
		 * Definition of a callback for medium finished events.
		 */
		typedef Ocean::Callback<void> Callback;

	public:

		/**
		 * Creates a new main window object.
		 * @param title Tile of the frame window to be created
		 * @param parent Parent window
		 * @param pos Position of the frame window
		 * @param size Size of the frame window
		 */
		FrameMediumWindow(const wxString& title, wxWindow* parent = nullptr, const wxPoint& pos = wxPoint(-1, -1), const wxSize& size = wxSize(-1, -1));

		/**
		 * Destructs this frame medium window.
		 */
		~FrameMediumWindow() override;

		/**
		 * Sets or changes the frame medium of this window.
		 * @param medium Frame medium to be set
		 */
		void setFrameMedium(const Media::FrameMediumRef& medium);

		/**
		 * Sets the callback function for medium finished events.
		 * @param callback Callback function to set
		 */
		inline void setFinishedCallback(const Callback& callback);

	protected:

		/**
		 * Thread run function.
		 * @see Thread::threadRun().
		 */
		void threadRun() override;

	protected:

		/// Frame medium to be displed.
		Media::FrameMediumRef frameMedium_;

		/// Timestamp of the current frame.
		Timestamp frameTimestamp_;

		/// Medium finished callback function.
		Callback callback_;

		/// True, if the medium has finished.
		bool hasFinished_;

		/// Frame lock.
		Ocean::Lock lock_;
};

inline void FrameMediumWindow::setFinishedCallback(const Callback& callback)
{
	callback_ = callback;
}

}

}

}

#endif // META_OCEAN_PLATFORM_WXWIDGETS_FRAME_MEDIUM_WINDOW_H
