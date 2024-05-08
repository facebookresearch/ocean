/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_WXWIDGETS_BITMAP_WINDOW_H
#define META_OCEAN_PLATFORM_WXWIDGETS_BITMAP_WINDOW_H

#include "ocean/platform/wxwidgets/WxWidgets.h"

#include "ocean/base/Frame.h"

#include "ocean/math/Numeric.h"

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
class OCEAN_PLATFORM_WXWIDGETS_EXPORT BitmapWindow : public wxScrolledWindow
{
	public:

		/**
		 * Definition of several display modes.
		 */
		enum DisplayMode
		{
			// User display mode.
			DM_USER,
			// Stretch display mode.
			DM_STRETCH,
			// Enlarge display mode.
			DM_ENLARGE_TO_SCREEN,
			/// Fullscreen zoom display mode.
			DM_ZOOM_TO_FULLSCREEN,
			/// Shrink display mode.
			DM_SHRINK_TO_SCREEN
		};

	public:

		/**
		 * Creates a new main window object.
		 * @param title Tile of the bitmap window to be created
		 * @param parent Parent window
		 * @param pos Position of the bitmap window
		 * @param size Size of the bitmap window
		 */
		BitmapWindow(const wxString& title, wxWindow* parent = nullptr, const wxPoint& pos = wxPoint(-1, -1), const wxSize& size = wxSize(-1, -1));

		/**
		 * Sets or changes the bitmap of this bitmap window.
		 * @param bitmap Bitmap to be set
		 * @param resetZoom True, to reset the zoom; False, to keep the zoom as it is
		 */
		void setBitmap(const wxBitmap& bitmap, const bool resetZoom = true);

		/**
		 * Sets or changes the bitmap of this bitmap window by a given frame.
		 * @param frame The frame to be set
		 * @param resetZoom True, to reset the zoom; False, to keep the zoom as it is
		 * @return True, if succeeded
		 */
		bool setFrame(const Frame& frame, const bool resetZoom = true);

		/**
		 * Sets the display mode of this bitmap window.
		 * @param mode Mode to be set
		 */
		void setDisplayMode(const DisplayMode mode);

		/**
		 * Sets the zoom of the bitmap window.
		 * @param zoom Zoom to be set
		 */
		void setZoom(const Scalar zoom);

		/**
		 * Enables scroll bars if necessary.
		 * @param enable True, to enable the scrollbars
		 */
		void setEnableScrollbars(const bool enable);

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
		 * Paint event function.
		 * @param event Event command
		 * @param dc Device context
		 */
		virtual void onPaintOverlay(wxPaintEvent& event, wxPaintDC& dc);

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
		 * Paints the bitmap of this window at the center of this window.
		 * @param dc Device context in which the bitmap will be painted
		 * @param clientWidth Client width of this window
		 * @param clientHeight Client height of this window
		 * @param bitmapWidth The display width of the bitmap
		 * @param bitmapHeight The display height of the bitmap
		 */
		void paintCenter(wxDC& dc, const int clientWidth, const int clientHeight, const int bitmapWidth, const int bitmapHeight);

		/**
		 * Paints the bitmap of this window centered in the window with a zoom of 1.0.
		 * The bitmap of this window must have a valid dimension, with range [1, infinity)x[1, infinity)
		 * @param dc Device context in which the bitmap will be painted
		 * @param clientWidth Client width of this window, with range [1, infinity)
		 * @param clientHeight Client height of this window, with range [1, infininty)
		 */
		void paintSubset(wxDC& dc, const int clientWidth, const int clientHeight);

		/**
		 * Paints the bitmap of this window centered in the window while the size of the bitmap is scaled to the larges size (with correct aspect ratio) so that the entire bitmap is still visible.
		 * The bitmap of this window must have a valid dimension, with range [1, infinity)x[1, infinity)
		 * @param dc Device context in which the bitmap will be painted
		 * @param clientWidth Client width of this window, with range [1, infinity)
		 * @param clientHeight Client height of this window, with range [1, infininty)
		 */
		void paintFit2Client(wxDC& dc, const int clientWidth, const int clientHeight);

		/**
		 * Paints the bitmap of this window centered in the window while the size of the bitmap is scaled (with correct aspect ratio) so that the entire window is filled with the bitmap.
		 * The bitmap of this window must have a valid dimension, with range [1, infinity)x[1, infinity)
		 * @param dc Device context in which the bitmap will be painted
		 * @param clientWidth Client width of this window, with range [1, infinity)
		 * @param clientHeight Client height of this window, with range [1, infininty)
		 */
		void paintFit2Fullscreen(wxDC& dc, const int clientWidth, const int clientHeight);

		/**
		 * Returns the width of the zoomed bitmap in pixel.
		 * @return Zoomed bitmap width
		 */
		int zoomedWidth() const;

		/**
		 * Returns the height of the zoomed bitmap in pixel.
		 * @return Zoomed bitmap height
		 */
		int zoomedHeight() const;

		/**
		 * Calculates the bitmap position for a given window position.
		 * @param x Horizontal window position
		 * @param y Vertical window position
		 * @param xBitmap Resulting horizontal bitmap position
		 * @param yBitmap Resulting vertical bitmap position
		 * @param allowPositionOutsideBitmap True, to allow output positions lying outside the actual bitmap; False, to fail when a given bitmap position is outside the actual bitmap
		 * @return True, if the given point lies inside the bitmap
		 */
		bool window2bitmap(const int x, const int y, Scalar& xBitmap, Scalar& yBitmap, const bool allowPositionOutsideBitmap = false);

		/**
		 * Calculates the bitmap position for a given virtual window position.
		 * @param xVirtual Horizontal window position
		 * @param yVirtual Vertical window position
		 * @param xBitmap Resulting horizontal bitmap position
		 * @param yBitmap Resulting vertical bitmap position
		 * @return True, if the given point lies inside the bitmap
		 */
		bool virtualWindow2bitmap(const int xVirtual, const int yVirtual, Scalar& xBitmap, Scalar& yBitmap);

		/**
		 * Calculates the virtual window position for a given bitmap position.
		 * @param xBitmap Horizontal bitmap position
		 * @param yBitmap Vertical bitmap position
		 * @param xVirtual Resulting horizontal virtual position
		 * @param yVirtual Resulting vertical virtual position
		 * @param allowPositionOutsideBitmap True, to allow input positions lying outside the actual bitmap; False, to fail when a given bitmap position is outside the actual bitmap
		 * @return True, if the given point could be converted to a point located in the virtual window; False, if no point could be converted
		 */
		bool bitmap2virtualWindow(const Scalar xBitmap, const Scalar yBitmap, Scalar& xVirtual, Scalar& yVirtual, const bool allowPositionOutsideBitmap = false);

		/**
		 * Calculates the window position for a given bitmap position.
		 * @param xBitmap Horizontal bitmap position
		 * @param yBitmap Vertical bitmap position
		 * @param x Resulting horizontal window position
		 * @param y Resulting vertical window position
		 * @return True, if the given point lies inside the bitmap
		 */
		bool bitmap2window(const int xBitmap, const int yBitmap, Scalar& x, Scalar& y);

		/**
		 * Calculates the window position for a given bitmap position.
		 * @param xBitmap Horizontal bitmap position
		 * @param yBitmap Vertical bitmap position
		 * @param x Resulting horizontal window position
		 * @param y Resulting vertical window position
		 * @return True, if the given point lies inside the bitmap
		 */
		bool bitmap2window(const Scalar xBitmap, const Scalar yBitmap, Scalar& x, Scalar& y);

	protected:

		/// Bitmap of the window.
		wxBitmap bitmap_;

		/// Bitmap horizontal display position.
		int displayBitmapLeft_ = NumericT<int>::minValue();

		/// Bitmap vertical display position.
		int displayBitmapTop_ = NumericT<int>::minValue();

		/// Bitmap display width.
		int displayBitmapWidth_ = NumericT<int>::minValue();

		/// Bitmap display height.
		int displayBitmapHeight_ = NumericT<int>::minValue();

		/// Display mode.
		DisplayMode displayMode_ = DM_SHRINK_TO_SCREEN;

		/// Scrollbar enable state.
		bool scrollbarsEnabled_ = true;

		/// Zoom factor for user display mode.
		double zoom_ = 1.0;

		/// Previous horizontal middle mouse position.
		double previousMouseMiddleBitmapX_ = NumericD::minValue();

		/// Previous vertical middle mouse position.
		double previousMouseMiddleBitmapY = NumericD::minValue();

		/// Previous horizontal mouse position.
		int previousMouseX_ = -1;

		/// Previous vertical mouse position.
		int previousMouseY_ = -1;

		/// Bitmap lock.
		mutable Ocean::Lock bitmapLock_;
};

}

}

}

#endif // META_OCEAN_PLATFORM_WXWIDGETS_BITMAP_WINDOW_H
