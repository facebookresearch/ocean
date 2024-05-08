/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_WXWIDGETS_FRAME_WINDOW_H
#define META_OCEAN_PLATFORM_WXWIDGETS_FRAME_WINDOW_H

#include "ocean/platform/wxwidgets/BitmapWindow.h"

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
class OCEAN_PLATFORM_WXWIDGETS_EXPORT FrameWindow : public BitmapWindow
{
	public:

		/**
		 * Creates a new main window object.
		 * @param title Tile of the frame window to be created
		 * @param parent Parent window
		 * @param pos Position of the frame window
		 * @param size Size of the frame window
		 */
		FrameWindow(const wxString& title, wxWindow* parent = nullptr, const wxPoint& pos = wxPoint(-1, -1), const wxSize& size = wxSize(-1, -1));

		/**
		 * Sets of changes the frame of this frame window.
		 * @param frame The frame to be converted into a bitmap and then changed as display bitmap
		 * @return True, if succeeded
		 */
		virtual bool setFrame(const Frame& frame);

	protected:

		/// Frame medium to be displayed.
		Frame frame_;

		/// Timestamp of the current frame.
		Timestamp frameTimestamp_;
};

}

}

}

#endif // META_OCEAN_PLATFORM_WXWIDGETS_FRAME_WINDOW_H
