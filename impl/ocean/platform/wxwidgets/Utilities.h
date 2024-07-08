/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_WXWIDGETS_UTILITIES_H
#define META_OCEAN_PLATFORM_WXWIDGETS_UTILITIES_H

#include "ocean/platform/wxwidgets/WxWidgets.h"

#include "ocean/base/Frame.h"

#include "ocean/cv/PixelBoundingBox.h"

namespace Ocean
{

namespace Platform
{

namespace WxWidgets
{

/**
 * This class implemnets utility functions for WxWidgets.
 * @ingroup platformwxw
 */
class OCEAN_PLATFORM_WXWIDGETS_EXPORT Utilities
{
	public:

		/**
		 * Prints a text on the given device.
		 * @param dc Device receiving the text
		 * @param x Horizontal output position
		 * @param y Vertical output position
		 * @param text Text to be printed
		 */
		static void textOutput(wxDC& dc, const int x, const int y, const wxString& text);

		/**
		 * Prints a text on the given device.
		 * @param dc Device receiving the text
		 * @param x Horizontal output position
		 * @param y Vertical output position
		 * @param text Text to be printed
		 */
		static void textOutput(wxDC& dc, const int x, const int y, const std::string& text);

		/**
		 * Prints a text on the desktop.
		 * @param x Horizontal output position
		 * @param y Vertical output position
		 * @param text Text to be printed
		 */
		static void desktopTextOutput(const int x, const int y, const wxString& text);

		/**
		 * Prints a text on the desktop.
		 * @param x Horizontal output position
		 * @param y Vertical output position
		 * @param text Text to be printed
		 */
		static void desktopTextOutput(const int x, const int y, const std::string& text);

		/**
		 * Prints a bitmap on the given device.
		 * @param dc Device receiving the bitmap
		 * @param x Horizontal output position
		 * @param y Vertical output position
		 * @param bitmap Bitmap to be printed
		 */
		static void bitmapOutput(wxDC& dc, const int x, const int y, const wxBitmap& bitmap);

		/**
		 * Prints a frame on the given device.
		 * @param dc Device receiving the bitmap
		 * @param x Horizontal output position
		 * @param y Vertical output position
		 * @param frame The frame to be printed
		 */
		static void frameOutput(wxDC& dc, const int x, const int y, const Frame& frame);

		/**
		 * Prints a bitmap on the desktop.
		 * @param x Horizontal output position
		 * @param y Vertical output position
		 * @param bitmap Bitmap to be printed
		 */
		static void desktopBitmapOutput(const int x, const int y, const wxBitmap& bitmap);

		/**
		 * Prints a frame on the desktop.
		 * @param x Horizontal output position
		 * @param y Vertical output position
		 * @param frame The frame to be printed
		 */
		static void desktopFrameOutput(const int x, const int y, const Frame& frame);

		/**
		 * Prints a bitmap on the given device.
		 * @param dc Device receiving the bitmap
		 * @param xOutput Horizontal output position
		 * @param yOutput Vertical output position
		 * @param widthOutput Width of the output in pixel
		 * @param heightOutput Height of the output in pixel
		 * @param xInput Horizontal input position
		 * @param yInput Vertical input position
		 * @param widthInput Width of the input in pixel
		 * @param heightInput Height of the input in pixe
		 * @param bitmap Bitmap to be printed
		 */
		static void bitmapOutput(wxDC& dc, const int xOutput, const int yOutput, const int widthOutput, const int heightOutput, const int xInput, const int yInput, const int widthInput, const int heightInput, const wxBitmap& bitmap);

		/**
		 * Determines the bounding box of a given string with specified font and font size.
		 * @param value The string for which the bounding box will be determined
		 * @param font The name of the font which will be applied
		 * @param size The size of the font
		 * @return The bounding box of the given string
		 */
		static CV::PixelBoundingBox textBoundingBox(const std::string& value, const std::string& font = std::string(), const unsigned int size = 0u);

		/**
		 * Determines the bounding box of a given string with specified font and font size.
		 * @param value The string for which the bounding box will be determined
		 * @param font The name of the font which will be applied
		 * @param size The size of the font
		 * @return The bounding box of the given string
		 */
		static CV::PixelBoundingBox textBoundingBox(const std::wstring& value, const std::wstring& font = std::wstring(), const unsigned int size = 0u);

		/**
		 * Converts a WxWidgets string value to an 8bit string.
		 * @param value Value to convert
		 * @return Converted string
		 */
		static inline std::string toAString(const wxString& value);

		/**
		 * Converts a WxWidgets string value to a wstring.
		 * @param value Value to convert
		 * @return Converted string
		 */
		static inline std::wstring toWString(const wxString& value);
};

/**
 * This class implements a scoped busy cursor object.
 * @ingroup platformwxw
 */
class WxScopedBusyCursor
{
	public:

		/**
		 * Creates a new scoped cursor object.
		 */
		inline WxScopedBusyCursor();

		/**
		 * Destructs a scoped cursor.
		 */
		inline ~WxScopedBusyCursor();

		/**
		 * Explicitly releases the busy cursor (already before the scope ends).
		 */
		inline void release();

	protected:

		/**
		 * Disabled copy constructor.
		 * @param cursor The cursor that would be copied
		 */
		WxScopedBusyCursor(const WxScopedBusyCursor& cursor) = delete;

		/**
		 * Disabled assign operator.
		 * @param cursor The cursor that would be assigned
		 */
		WxScopedBusyCursor& operator=(const WxScopedBusyCursor& cursor) = delete;

	protected:

		/// True, if the cursor is still busy.
		bool isBusy_ = true;
};

/**
 * This class implements a scoped object which disables a window object until the scope ends (or until the object is released explicitly).
 * @ingroup platformwxw
 */
class WxScopedDisableWindow
{
	public:

		/**
		 * Creates a new scoped object and disables the associated window.
		 * @param window The window to disable, this window must not be disposed before this scoped object is disposed
		 */
		explicit inline WxScopedDisableWindow(wxWindow& window);

		/**
		 * Destructs this scoped object and enables the associated window.
		 */
		inline ~WxScopedDisableWindow();

		/**
		 * Explicitly releases the scoped object and enables the associated window (already before the scope ends).
		 */
		inline void release();

	protected:

		/**
		 * Disabled copy constructor.
		 * @param window The window that would be copied
		 */
		WxScopedDisableWindow(const WxScopedDisableWindow& window) = delete;

		/**
		 * Disabled assign operator.
		 * @param window The window that would be assigned
		 */
		WxScopedDisableWindow& operator=(const WxScopedDisableWindow& window) = delete;

	protected:

		/// The associated window, nullptr if the object has been released already.
		wxWindow* window_ = nullptr;
};

/**
 * This class implements a scoped object which removes a given style from a given window until the scope ends (or until the object is released explicitly).
 * @ingroup platformwxw
 */
class WxScopedRemoveStyle
{
	public:

		/**
		 * Creates a new scoped object and removes the given style from the given window if the window is not nullptr.
		 * @param window The window to remove the style from, this window must not be disposed before this scoped object is disposed, may be nullptr so that nothing will be done
		 * @param style The style to remove from the window
		 */
		inline WxScopedRemoveStyle(wxWindow* window, const long style);

		/**
		 * Destructs this scoped object and assign the old style to the associated window.
		 */
		inline ~WxScopedRemoveStyle();

		/**
		 * Explicitly releases the scoped object and assign the old style to the associated window (already before the scope ends).
		 */
		inline void release();

	protected:

		/**
		 * Disabled copy constructor.
		 * @param style The style object that would be copied
		 */
		WxScopedRemoveStyle(const WxScopedRemoveStyle& style) = delete;

		/**
		 * Disabled assign operator.
		 * @param style The style object that would be assigned
		 */
		WxScopedRemoveStyle& operator=(const WxScopedRemoveStyle& style) = delete;

	protected:

		/// The associated window, nullptr if the object has been released already
		wxWindow* window_ = nullptr;

		/// The old style of the associated window before the removing
		long oldStyle_ = 0;
};

inline std::string Utilities::toAString(const wxString& value)
{
	if (value.IsEmpty())
	{
		return std::string();
	}

#if defined(wxABI_VERSION) && wxABI_VERSION <= 20899
	return String::toAString(value);
#else
	return std::string(value.c_str().AsChar());
#endif
}

inline std::wstring Utilities::toWString(const wxString& value)
{
	if (value.IsEmpty())
	{
		return std::wstring();
}

#if defined(wxABI_VERSION) && wxABI_VERSION <= 20899
	return std::wstring(value);
#else
	return std::wstring(value.c_str().AsWChar());
#endif
}

inline WxScopedBusyCursor::WxScopedBusyCursor()
{
	wxBeginBusyCursor();
}

inline WxScopedBusyCursor::~WxScopedBusyCursor()
{
	release();
}

inline void WxScopedBusyCursor::release()
{
	if (isBusy_)
	{
		wxEndBusyCursor();
		isBusy_ = false;
	}
}

inline WxScopedDisableWindow::WxScopedDisableWindow(wxWindow& window) :
	window_(&window)
{
	ocean_assert(window_->IsEnabled());
	window_->Enable(false);
}

inline WxScopedDisableWindow::~WxScopedDisableWindow()
{
	release();
}

inline void WxScopedDisableWindow::release()
{
	if (window_)
	{
		ocean_assert(!window_->IsEnabled());
		window_->Enable(true);
		window_ = nullptr;
	}
}

inline WxScopedRemoveStyle::WxScopedRemoveStyle(wxWindow* window, const long style) :
	window_(window),
	oldStyle_(0)
{
	if (window_)
	{
		oldStyle_ = window_->GetWindowStyleFlag();
		window_->SetWindowStyleFlag(oldStyle_ & ~style);
	}
}

inline WxScopedRemoveStyle::~WxScopedRemoveStyle()
{
	release();
}

inline void WxScopedRemoveStyle::release()
{
	if (window_)
	{
		window_->SetWindowStyleFlag(oldStyle_);
		window_ = nullptr;
	}
}

}

}

}

#endif // META_OCEAN_PLATFORM_WXWIDGETS_UTILITIES_H
