/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_WIN_BITMAP_WINDOW_H
#define META_OCEAN_PLATFORM_WIN_BITMAP_WINDOW_H

#include "ocean/platform/win/Win.h"
#include "ocean/platform/win/Bitmap.h"
#include "ocean/platform/win/Window.h"

namespace Ocean
{

namespace Platform
{

namespace Win
{

/**
 * This class implements a main window.
 * @ingroup platformwin
 */
class OCEAN_PLATFORM_WIN_EXPORT BitmapWindow : virtual public Window
{
	public:

		/**
		 * Definition of several display modes.
		 */
		enum DisplayMode
		{
			/// An invalid display mode.
			DM_INVALID = 0,
			// The bitmap is stretch to cover the entire window, the aspect ratio is not preserved.
			DM_STRETCH_TO_SCREEN,
			// The bitmap is resized (enlarged or reduced) to fit into the window, the aspect ratio is preserved.
			DM_FIT_TO_SCREEN
		};

	public:

		/**
		 * Creates a new bitmap window.
		 * @param instance Application instance
		 * @param name The name of the window
		 */
		BitmapWindow(HINSTANCE instance, const std::wstring& name);

		/**
		 * Destructs the bitmap window.
		 */
		~BitmapWindow() override;

		/**
		 * Returns the bitmap of this windows.
		 * @return Window bitmap
		 */
		inline const Bitmap& bitmap() const;

		/**
		 * Sets the bitmap of this window.
		 * @param bitmap The bitmap to set, an invalid bitmap to remove a previously set bitmap
		 */
		void setBitmap(const Bitmap& bitmap);

		/**
		 * Sets the frame for this window.
		 * @param frame The frame to set, an invalid frame to remove a previously set frame
		 */
		void setFrame(const Frame& frame);

		/**
		 * Sets the display mode of this bitmap window.
		 * @param mode The mode to be set
		 */
		void setDisplayMode(const DisplayMode mode);

		/**
		 * Adjusts the windows size to the bitmap size.
		 */
		void adjustToBitmapSize();

	protected:

		/**
		 * Allows the modification of the window class before registration.
		 * @see Window::modifyWindowClass().
		 */
		void modifyWindowClass(WNDCLASSW& windowClass) override;

		/**
		 * Function for window resize event.
		 * @see Window::onResize().
		 */
		void onResize(const unsigned int clientWidth, const unsigned int clientHeight) override;

		/**
		 * Repaints the window.
		 * @see Window::onPaint().
		 */
		void onPaint() override;

		/**
		 * Function for mouse double click events.
		 * @see Window::onMouseDoubleClick().
		 */
		void onMouseDoubleClick(const MouseButton button, const int x, const int y) override;

		/**
		 * Converts window positions to bitmap positions.
		 * @param windowX Horizontal window position
		 * @param windowY Vertical window position
		 * @param bitmapX Resulting horizontal bitmap position
		 * @param bitmapY Resulting vertical bitmap position
		 * @return True, if succeeded
		 */
		bool window2bitmap(const int windowX, const int windowY, int& bitmapX, int& bitmapY);

	protected:

		/// Window bitmap
		Bitmap bitmap_;

		/// Width of the stretched bitmap.
		unsigned int stretchWidth_ = 0u;

		/// Height of the stretched bitmap.
		unsigned int stretchHeight_ = 0u;

		/// Horizontal position of the stretched bitmap.
		unsigned int stretchLeft_ = 0u;

		/// Vertical position of the stretched bitmap.
		unsigned int stretchTop_ = 0u;

		/// The display mode which is used to display the bitmap within the window.
		DisplayMode displayMode_ = DM_FIT_TO_SCREEN;

		/// Window lock.
		Lock lock_;
};

inline const Bitmap& BitmapWindow::bitmap() const
{
	return bitmap_;
}

}

}

}

#endif // META_OCEAN_PLATFORM_WIN_BITMAP_WINDOW_H
