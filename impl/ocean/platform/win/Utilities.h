/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_WIN_UTILITIES_H
#define META_OCEAN_PLATFORM_WIN_UTILITIES_H

#include "ocean/platform/win/Win.h"
#include "ocean/platform/win/Bitmap.h"

#include "ocean/base/Frame.h"

#include "ocean/cv/PixelBoundingBox.h"

#ifdef OCEAN_COMPILER_MSC
	#pragma managed(push, off)
#endif

#include "ocean/base/String.h"

#include <string>

#ifdef OCEAN_COMPILER_MSC
	#pragma managed(pop)
#endif

namespace Ocean
{

namespace Platform
{

namespace Win
{

/**
 * This class implements utilities and helper functions.
 * @ingroup platformwin
 */
class OCEAN_PLATFORM_WIN_EXPORT Utilities
{
	public:

		/**
		 * Prints a text on the given device context.
		 * @param dc Device context receiving the text
		 * @param x Horizontal output position
		 * @param y Vertical output position
		 * @param text Text to be printed
		 */
		static void textOutput(HDC dc, const int x, const int y, const std::string& text);

		/**
		 * Prints a text on the desktop.
		 * @param x Horizontal output position
		 * @param y Vertical output position
		 * @param text Text to be printed
		 */
		static void desktopTextOutput(const int x, const int y, const std::string& text);

		/**
		 * Prints a frame on the given device context.
		 * @param dc Device context receiving the frame
		 * @param x Horizontal output position
		 * @param y Vertical output position
		 * @param frame The frame to be printed
		 */
		static void frameOutput(HDC dc, const int x, const int y, const Frame& frame);

		/**
		 * Prints a frame on the given device context.
		 * @param dc Device context receiving the text
		 * @param x Horizontal output position
		 * @param y Vertical output position
		 * @param width Output width in pixel
		 * @param height Output height in pixel
		 * @param frame The frame to be printed
		 */
		static void frameOutput(HDC dc, const int x, const int y, const unsigned int width, const unsigned int height, const Frame& frame);

		/**
		 * Prints a given frame on the Windows' main desktop at a specified location.
		 * You do not need to have a GUI application to use this function.<br>
		 * Call this function from any application and from any thread.<br>
		 * The painted frame will disappear immediately when Windows receives any repaint event.<br>
		 * This function is intended for debugging purposes only.
		 * @param x Horizontal output position, with range (-infinity, infinity)
		 * @param y Vertical output position, with range (-infinity, infinity)
		 * @param frame The frame to be printed
		 */
		static void desktopFrameOutput(const int x, const int y, const Frame& frame);

		/**
		 * Prints a given frame on the Windows' main desktop at a specified location.
		 * You do not need to have a GUI application to use this function.<br>
		 * Call this function from any application and from any thread.<br>
		 * The painted frame will disappear immediately when Windows receives any repaint event.<br>
		 * This function is intended for debugging purposes only.
		 * @param x Horizontal output position, with range (-infinity, infinity)
		 * @param y Vertical output position, with range (-infinity, infinity)
		 * @param scale The scale that is applied to the frame, a factor of 2 will increase the painted frame by two, with range [1, infinity)
		 * @param frame The frame to be printed
		 */
		static inline void desktopFrameOutput(const int x, const int y, const unsigned int scale, const Frame& frame);

		/**
		 * Prints a given frame on the Windows' main desktop at a specified location.
		 * You do not need to have a GUI application to use this function.<br>
		 * Call this function from any application and from any thread.<br>
		 * The painted frame will disappear immediately when Windows receives any repaint event.<br>
		 * This function is intended for debugging purposes only.
		 * @param x Horizontal output position, with range (-infinity, infinity)
		 * @param y Vertical output position, with range (-infinity, infinity)
		 * @param width The width of the painted frame in the domain of the desktop, in pixel, with range [1, infinity)
		 * @param height The height of the painted frame in the domain of the desktop, in pixel, with range [1, infinity)
		 * @param frame The frame to be printed
		 */
		static void desktopFrameOutput(const int x, const int y, const unsigned int width, const unsigned int height, const Frame& frame);

		/**
		 * Prints a bitmap on the given device context.
		 * @param dc Device context receiving the bitmap
		 * @param x Horizontal output position
		 * @param y Vertical output position
		 * @param bitmap bitmap to be printed
		 */
		static void bitmapOutput(HDC dc, const int x, const int y, const Bitmap& bitmap);

		/**
		 * Prints a bitmap on the desktop.
		 * @param x Horizontal output position
		 * @param y Vertical output position
		 * @param scale Scale of the output bitmap
		 * @param bitmap Bitmap to be printed
		 */
		static inline void desktopBitmapOutput(const int x, const int y, const unsigned int scale, const Bitmap& bitmap);

		/**
		 * Prints a bitmap on the given device context.
		 * @param dc Device context receiving the text
		 * @param x Horizontal output position
		 * @param y Vertical output position
		 * @param width Output width in pixel
		 * @param height Output height in pixel
		 * @param bitmap bitmap to be printed
		 */
		static void bitmapOutput(HDC dc, const int x, const int y, const unsigned int width, const unsigned int height, const Bitmap& bitmap);

		/**
		 * Prints a bitmap on the desktop.
		 * @param x Horizontal output position
		 * @param y Vertical output position
		 * @param bitmap bitmap to be printed
		 */
		static void desktopBitmapOutput(const int x, const int y, const Bitmap& bitmap);

		/**
		 * Prints a bitmap on the desktop.
		 * @param x Horizontal output position
		 * @param y Vertical output position
		 * @param width Output width in pixel
		 * @param height Output height in pixel
		 * @param bitmap bitmap to be printed
		 */
		static void desktopBitmapOutput(const int x, const int y, const unsigned int width, const unsigned int height, const Bitmap& bitmap);

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
};

/**
 * This class implements a nested scoped object which disables a window object until the scope of all nested elements ends (or until all nested object are released explicitly).
 * The creation of the first nested object disables the window.<br>
 * Following nested objects only increase the internal counter while do not have any further consequence.<br>
 * @ingroup platformwin
 */
class ScopedDisableWindow
{
	protected:

		/**
		 * This class implements a simple counter for nested disable window objects.
		 */
		class DisableWindowCounter : public Singleton<DisableWindowCounter>
		{
			friend class Singleton<DisableWindowCounter>;
			friend class ScopedDisableWindow;

			protected:

				/**
				 * Definition of a map mapping window handles to counters.
				 */
				typedef std::unordered_map<HWND, unsigned int> CounterMap;

			protected:

				/**
				 * Creates a new DisableWindowCounter object.
				 */
				inline DisableWindowCounter();

				/**
				 * Destructs a DisableWindowCounter object.
				 */
				inline ~DisableWindowCounter();

				/**
				 * Increases the counter of this object.
				 * @return True, if the counter was zero before the counter has been increased
				 */
				inline bool increase(const HWND windowHandle);

				/**
				 * Decreases the counter of this object.
				 * @return True, if the counter is zero after it has been decreased
				 */
				inline bool decrease(const HWND windowHandle);

				/**
				 * Returns whether the counter is zero.
				 * @return True, if so
				 */
				inline bool isZero(const HWND windowHandle);

				/**
				 * Returns the lock object.
				 * @return Lock object
				 */
				inline Lock& lock();

			protected:

				/// Disable window counter.
				CounterMap counterMap_;

				/// Disable window lock.
				Lock windowLock_;
		};

	public:

		/**
		 * Creates a new scoped object and disables the associated window.
		 * @param windowHandle Handle of the window to disable, this window must not be disposed before this scoped object is disposed
		 */
		explicit ScopedDisableWindow(const HWND windowHandle);

		/**
		 * Destructs this scoped object and enables the associated window.
		 */
		inline ~ScopedDisableWindow();

		/**
		 * Explicitly releases the scoped object and enables the associated window (already before the scope ends).
		 */
		void release();

	protected:

		/**
		 * Disabled copy constructor.
		 * @param window The window that would be copied
		 */
		ScopedDisableWindow(const ScopedDisableWindow& window) = delete;

		/**
		 * Disabled assign operator.
		 * @param window The window that would be assigned
		 */
		ScopedDisableWindow& operator=(const ScopedDisableWindow& window) = delete;

	protected:

		/// The associated window, nullptr if the object has been released already.
		HWND handle_ = nullptr;
};

#if defined(OCEAN_COMPILER_MSC) && defined(_MANAGED)

inline System::String^ toString(const std::string& value)
{
	return gcnew System::String(value.c_str());
}

inline System::String^ toString(const char* value)
{
	return gcnew System::String(value);
}

inline System::String^ toString(const std::wstring& value)
{
	return gcnew System::String(value.c_str());
}

inline System::String^ toString(const wchar_t* value)
{
	return gcnew System::String(value);
}

inline std::string toAString(System::String^ value)
{
	char* str = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(value);
	std::string result(str);

	System::Runtime::InteropServices::Marshal::FreeHGlobal(System::IntPtr(str));

	return result;
}

inline std::wstring toWString(System::String^ value)
{
	char* str = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(value);
	std::string result(str);

	System::Runtime::InteropServices::Marshal::FreeHGlobal(System::IntPtr(str));

	return Ocean::String::toWString(result);
}

#endif // OCEAN_COMPILER_MSC && _MANAGED

inline void Utilities::desktopFrameOutput(const int x, const int y, const unsigned int scale, const Frame& frame)
{
	desktopFrameOutput(x, y, frame.width() * scale, frame.height() * scale, frame);
}

inline void Utilities::desktopBitmapOutput(const int x, const int y, const unsigned int scale, const Bitmap& bitmap)
{
	desktopBitmapOutput(x, y, bitmap.width() * scale, bitmap.height() * scale, bitmap);
}

inline ScopedDisableWindow::DisableWindowCounter::DisableWindowCounter()
{
	// nothing to do here
}

inline ScopedDisableWindow::DisableWindowCounter::~DisableWindowCounter()
{
#ifdef OCEAN_DEBUG

	for (CounterMap::const_iterator it = counterMap_.begin(); it != counterMap_.end(); ++it)
		ocean_assert(it->second == 0u);

#endif
}

inline bool ScopedDisableWindow::DisableWindowCounter::increase(const HWND windowHandle)
{
	counterMap_[windowHandle]++;

	return counterMap_[windowHandle] == 1u;
}

inline bool ScopedDisableWindow::DisableWindowCounter::decrease(const HWND windowHandle)
{
	ocean_assert(counterMap_.find(windowHandle) != counterMap_.end());
	ocean_assert(counterMap_[windowHandle] >= 1u);

	counterMap_[windowHandle]--;

	return counterMap_[windowHandle] == 0u;
}

inline bool ScopedDisableWindow::DisableWindowCounter::isZero(const HWND windowHandle)
{
	return counterMap_[windowHandle] == 0u;
}

inline Lock& ScopedDisableWindow::DisableWindowCounter::lock()
{
	return windowLock_;
}

inline ScopedDisableWindow::~ScopedDisableWindow()
{
	release();
}

}

}

}

#endif // META_OCEAN_PLATFORM_WIN_UTILITIES_H
