/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_WXWIDGETS_FRAME_PREVIEW_WINDOW_H
#define META_OCEAN_PLATFORM_WXWIDGETS_FRAME_PREVIEW_WINDOW_H

#include "ocean/platform/wxwidgets/WxWidgets.h"

#include "ocean/media/MovieFrameProvider.h"

namespace Ocean
{

namespace Platform
{

namespace WxWidgets
{

/**
 * This class implements a frame preview window.
 * @ingroup platformwxw
 */
class OCEAN_PLATFORM_WXWIDGETS_EXPORT FramePreviewWindow : public wxPanel
{
	public:

		/**
		 * Definition of a selection callback function.
		 * The first parameter defines the index of the selection.<br>
		 */
		typedef Callback<void, const unsigned int> SelectionCallback;

	private:

		/**
		 * This class implements a preview element.
		 */
		class Element
		{
			public:

				/**
				 * Creates a default element.
				 */
				Element() = default;

				/**
				 * Creates a new element.
				 * @param index Index of the preview frame
				 * @param virtualPositionStart Virtual horizontal start position of this element
				 * @param displayId Display id
				 */
				inline Element(const unsigned int index, const int virtualPositionStart, const unsigned int displayId);

				/**
				 * Returns the frame index of this element.
				 * @return Frame index
				 */
				inline unsigned int index() const;

				/**
				 * Returns the virtual horizontal start position.
				 * @return Virtual horizotnal start position
				 */
				inline int virtualPositionStart() const;

				/**
				 * Returns the display id of this element.
				 * @return Display id
				 */
				inline unsigned int displayId() const;

				/**
				 * Returns the horizontal display start position of this element.
				 * @param windowVirtualPositionStart Virtual horizontal start position of the display window
				 * @param zoom Zoom of the display window
				 * @return Resulting horizontal display start position
				 */
				inline int displayPositionStart(const int windowVirtualPositionStart, const unsigned int zoom) const;

				/**
				 * Returns the horizontal display stop position (inclusive position) of this element.
				 * @param windowVirtualPositionStart Virtual horizontal start position of the display window
				 * @param previewWidth Width of the preview frame
				 * @param zoom Zoom of the display window
				 * @return Resulting horizontal display stop position (including)
				 */
				inline int displayPositionStop(const int windowVirtualPositionStart, const unsigned int previewWidth, const unsigned int zoom) const;

				/**
				 * Returns the horizontal display end position (exclusive position) of this element.
				 * @param windowVirtualPositionStart Virtual horizontal start position of the display window
				 * @param previewWidth Width of the preview frame
				 * @param zoom Zoom of the display window
				 * @return Resulting horizontal display end position (exclusive)
				 */
				inline int displayPositionEnd(const int windowVirtualPositionStart, const unsigned int previewWidth, const unsigned int zoom) const;

				/**
				 * Returns whether this element stores a valid preview frame.
				 * @return True, if so
				 */
				explicit inline operator bool() const;

			private:

				/// Index of the preview frame.
				unsigned int index_ = (unsigned int)(-1);

				/// Virtual horizontal start position.
				int virtualPositionStart_ = 0;

				/// Display id.
				unsigned int displayId_ = 0u;
		};

		/**
		 * Definition of a vector holding elements.
		 */
		typedef std::vector<Element> Elements;

	public:

		/**
		 * Creates a new main window object.
		 * @param title Tile of the bitmap window to be created
		 * @param parent Parent window
		 * @param pos Position of the bitmap window
		 * @param size Size of the bitmap window
		 */
		FramePreviewWindow(const wxString& title, wxWindow* parent = nullptr, const wxPoint& pos = wxPoint(-1, -1), const wxSize& size = wxSize(-1, 100));

		/**
		 * Destructs the window.
		 */
		~FramePreviewWindow() override;

		/**
		 * Sets a frame provider and exchanges the previous one.
		 * @param frameProvider Frame provider to be set
		 */
		void setFrameProvider(const Media::MovieFrameProviderRef& frameProvider);

		/**
		 * Returns the current selection index.
		 * @return Selection index
		 */
		inline unsigned int selectionIndex() const;

		/**
		 * Returns whether the frame index is displayed.
		 * @return True, if so (default)
		 */
		inline bool displayFrameIndex() const;

		/**
		 * Sets whether the frame index will be displayed.
		 * @param display True, to display the frame index, false otherwise
		 */
		inline void setDisplayFrameIndex(const bool display);

		/**
		 * Sets or changes the selection callback function.
		 * Set an empty callback function to remove the callback that has been set before.<br>
		 * Remove the callback function before the window is disposed.<br>
		 * @param selection Selection callback function to be set
		 */
		inline void setSelectionCallback(const SelectionCallback& selection);

	protected:

		/// WxWidgets event table.
		DECLARE_EVENT_TABLE();

	protected:

		/**
		 * Paint event function.
		 * @param event Event command
		 */
		virtual void onPaint(wxPaintEvent& event);

		/**
		 * Size event function.
		 * @param event Event command
		 */
		virtual void onSize(wxSizeEvent& event);

		/**
		 * Left mouse click down event function.
		 * @param event Event command
		 */
		virtual void onMouseLeftDown(wxMouseEvent& event);

		/**
		 * Left mouse click up event function.
		 * @param event Event command
		 */
		virtual void onMouseLeftUp(wxMouseEvent& event);

		/**
		 * Left mouse double click down event function.
		 * @param event Event command
		 */
		virtual void onMouseLeftDblClick(wxMouseEvent& event);

		/**
		 * Middle mouse click down event function.
		 * @param event Event command
		 */
		virtual void onMouseMiddleDown(wxMouseEvent& event);

		/**
		 * Middle mouse click up event function.
		 * @param event Event command
		 */
		virtual void onMouseMiddleUp(wxMouseEvent& event);

		/**
		 * Right mouse click down event function.
		 * @param event Event command
		 */
		virtual void onMouseRightDown(wxMouseEvent& event);

		/**
		 * Right mouse click up event function.
		 * @param event Event command
		 */
		virtual void onMouseRightUp(wxMouseEvent& event);

		/**
		 * Right mouse double click down event function.
		 * @param event Event command
		 */
		virtual void onMouseRightDblClick(wxMouseEvent& event);

		/**
		 * Mouse wheel event function.
		 * @param event Event command
		 */
		virtual void onMouseWheel(wxMouseEvent& event);

		/**
		 * Mouse move event function.
		 * @param event Event command
		 */
		virtual void onMouseMove(wxMouseEvent& event);

		/**
		 * Background erase event function.
		 * @param event Event command
		 */
		void onEraseBackground(wxEraseEvent& event);

		/**
		 * Event function for new frames.
		 * @param index Index of the frame
		 */
		void onPreviewFrame(const unsigned int index);

		/**
		 * Frame provider event function.
		 * @param eventType Type of the event
		 * @param value Parameter of the event
		 */
		void onEvent(const Media::MovieFrameProvider::EventType eventType, const unsigned long long value);

		/**
		 * Updates the display parameters.
		 */
		void updateDispalyParameter();

		/**
		 * Calculates the first visible preview frame.
		 * @param windowVirtualPositionStart Virtual horizontal start position of the display window
		 * @param previewWidth Width of the preview frame in pixels
		 * @param separatorWidth Width of the seperator between preview frames
		 * @param zoom Preview frame zooming factor
		 * @param adjust2zoom True, if the resulting index is adjusted to the zooming factor
		 * @param index Resulting index of the first visible preview frame
		 * @return True, if succeeded
		 */
		static bool calculateFirstVisibleElement(const int windowVirtualPositionStart, const unsigned int previewWidth, const unsigned int separatorWidth, const unsigned int zoom, const bool adjust2zoom, int& index);

		/**
		 * Calculates the last visible preview frame.
		 * @param windowVirtualPositionStart Virtual horizontal start position of the display window
		 * @param displayWidth Width of the display window in pixels
		 * @param previewWidth Width of the preview frame in pixels
		 * @param separatorWidth Width of the seperator between preview frames
		 * @param zoom Preview frame zooming factor
		 * @param adjust2zoom True, if the resulting index is adjusted to the zooming factor
		 * @param index Resulting index of the last visible preview frame
		 * @return True, if succeeded
		 */
		static bool calculateLastVisibleElement(const int windowVirtualPositionStart, const unsigned int displayWidth, const unsigned int previewWidth, const unsigned int separatorWidth, const unsigned int zoom, const bool adjust2zoom, int& index);

		/**
		 * Calculates the virtual horizontal start position of a preview frame.
		 * @param index Index of the preview frame
		 * @param previewWidth Width of the preview frame in pixel
		 * @param separatorWidth Width of the separator between preview frames
		 * @param zoom Preview frame zooming factor
		 * @param position Resulting virtual horizontal start position
		 * @return True, if succeeded
		 */
		static bool calculateVirtualPositionStart(const unsigned int index, const unsigned int previewWidth, const unsigned int separatorWidth, const unsigned int zoom, int& position);

		/**
		 * Draws a frame into a given device context.
		 * @param dc Device context in that the frame will be drawn
		 * @param frame The frame to be drawn
		 * @param frameIndex Index of frame
		 * @param displayPositionLeft Horizontal display position
		 * @param selection True, if the frame is selected
		 */
		void drawFrame(wxDC& dc, const Frame& frame, const unsigned int frameIndex, const int displayPositionLeft, const bool selection);

	protected:

		/// Frame provider of this window.
		Media::MovieFrameProviderRef frameProvider_;

		/// Current, estimated, or actual frame number.
		unsigned int frameNumber_ = 0u;

		/// Width of the separator between preview frame, in pixel.
		unsigned int separatorWidth_ = 2u;

		/// Virtual width of the window, in pixel.
		unsigned int virtualWidth_ = 0u;

		/// Virtual position, in pixel.
		int virtualPositionStart_ = 0;

		/// Width of a preview frame in pixel.
		unsigned int previewWidth_ = 0u;

		/// Height of the preview frame in pixel.
		unsigned int previewHeight_ = 0u;

		/// Preview zoom factor.
		unsigned int previewZoom_ = 50u;

		/// Preview elements.
		Elements elements_;

		/// Preview frame selection index.
		unsigned int selectionIndex_ = (unsigned int)(-1);

		/// True to display the frame index; False, to hide it
		bool displayFrameIndex_ = true;

		/// Selection changed callback function.
		SelectionCallback selectionCallback_;

		/// True, if a new previous frame has arrived.
		bool newPreviewFrame_ = false;
};

inline FramePreviewWindow::Element::Element(const unsigned int index, const int virtualPositionStart, const unsigned int displayId) :
	index_(index),
	virtualPositionStart_(virtualPositionStart),
	displayId_(displayId)
{
	// nothing to do here
}

inline unsigned int FramePreviewWindow::Element::index() const
{
	return index_;
}

inline int FramePreviewWindow::Element::virtualPositionStart() const
{
	return virtualPositionStart_;
}

inline unsigned int FramePreviewWindow::Element::displayId() const
{
	return displayId_;
}

inline int FramePreviewWindow::Element::displayPositionStart(const int windowVirtualPositionStart, const unsigned int zoom) const
{
	return virtualPositionStart_ / int(zoom) - windowVirtualPositionStart / int(zoom);
}

inline int FramePreviewWindow::Element::displayPositionStop(const int windowVirtualPositionStart, const unsigned int previewWidth, const unsigned int zoom) const
{
	return virtualPositionStart_ / int(zoom) - windowVirtualPositionStart / int(zoom) + int(previewWidth) - 1;
}

inline int FramePreviewWindow::Element::displayPositionEnd(const int windowVirtualPositionStart, const unsigned int previewWidth, const unsigned int zoom) const
{
	return virtualPositionStart_ / int(zoom) - windowVirtualPositionStart / int(zoom) + int(previewWidth);
}

inline FramePreviewWindow::Element::operator bool() const
{
	return index_ != (unsigned int)-1;
}

inline unsigned int FramePreviewWindow::selectionIndex() const
{
	return selectionIndex_;
}

inline void FramePreviewWindow::setSelectionCallback(const SelectionCallback& selectionCallback)
{
	selectionCallback_ = selectionCallback;
}

inline bool FramePreviewWindow::displayFrameIndex() const
{
	return displayFrameIndex_;
}

inline void FramePreviewWindow::setDisplayFrameIndex(const bool display)
{
	displayFrameIndex_ = display;
}

}

}

}

#endif // META_OCEAN_PLATFORM_WXWIDGETS_FRAME_PREVIEW_WINDOW_H
