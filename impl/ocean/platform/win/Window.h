/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_WIN_WINDOW_H
#define META_OCEAN_PLATFORM_WIN_WINDOW_H

#include "ocean/platform/win/Win.h"

#include "ocean/base/Lock.h"

#include <map>

namespace Ocean
{

namespace Platform
{

namespace Win
{

/**
 * This class is the base class for all windows.
 * @ingroup platformwin
 */
class OCEAN_PLATFORM_WIN_EXPORT Window
{
	public:

		/**
		 * Definition of several mouse buttons.
		 */
		enum MouseButton
		{
			/// No mouse button
			BUTTON_NONE = 0,
			/// Left mouse button
			BUTTON_LEFT = 1,
			/// Middle mouse button
			BUTTON_MIDDLE = 2,
			/// Right mouse button
			BUTTON_RIGHT = 4
		};

		/**
		 * Definition of a vector holding files.
		 */
		typedef std::vector<std::wstring> Files;

	protected:

		/**
		 * Map mapping class names to an reference counter.
		 */
		typedef std::unordered_map<std::wstring, unsigned int> ClassMap;

	public:

		/**
		 * Initializes the window.
		 * @param icon The optional handle of the icon to be used, nullptr to use default icon
		 * @param windowClass The name of the window class
		 * @return True, if succeeded
		 */
		virtual bool initialize(const HICON icon = nullptr, const std::string& windowClass = "window");

		/**
		 * Initializes the window.
		 * @param applicationInstance The instance of the application, must be valid
		 * @param iconId The id of the icon as specified in the resources, with range [0, infinity)
		 * @param windowClass The name of the window class
		 * @return True, if succeeded
		 */
		inline bool initialize(const HINSTANCE applicationInstance, const int iconId, const std::string& windowClass = "window");

		/**
		 * Shows the window.
		 */
		virtual void show();

		/**
		 * Hides the window.
		 */
		virtual void hide();

		/**
		 * Updates the window.
		 */
		virtual void update();

		/**
		 * Repaints the window.
		 * @param eraseBackground True, to erase the background
		 */
		virtual void repaint(const bool eraseBackground = false);

		/**
		 * Moves the window.
		 * @param x The new horizontal position
		 * @param y The new vertical position
		 */
		bool move(const int x, const int y);

		/**
		 * Resizes the window.
		 * @param width New window width
		 * @param height New window height
		 * @return True, if succeeded
		 */
		bool resize(const unsigned int width, const unsigned int height);

		/**
		 * Returns the application instance the window belongs to.
		 * @return Application instance
		 */
		inline HINSTANCE applicationInstance() const;

		/**
		 * Returns the name of this window.
		 * @return Window name
		 */
		inline const std::wstring& name() const;

		/**
		 * Returns the handle of this window.
		 * @return Window handle
		 */
		inline HWND handle() const;

		/**
		 * Returns the handle of a possible parent window.
		 * @return Parent window handle, nullptr otherwise
		 */
		inline HWND parentHandle() const;

		/**
		 * Returns the device context of this window.
		 * @return Device context
		 */
		inline HDC dc() const;

		/**
		 * Returns the width of the entire window.
		 * @return Width in pixel
		 */
		unsigned int width() const;

		/**
		 * Returns the height of the window.
		 * @return Height in pixel
		 */
		unsigned int height() const;

		/**
		 * Returns the width of the client window.
		 * @return Width in pixel
		 */
		unsigned int clientWidth() const;

		/**
		 * Returns the height of the client window.
		 * @return Height in pixel
		 */
		unsigned int clientHeight() const;

		/**
		 * Sets the parent window handle.
		 * @return True, if succeeded
		 */
		bool setParent(const HWND parent);

		/**
		 * Sets or changes the text of this windows.
		 * @param text The text to be set
		 */
		void setText(const std::wstring& text);

		/**
		 * Enables or disables drag&drop support for files.
		 * @param state True, to enable support for drag&drop of file for this window; False, to disable drag&drop.
		 * @see onDragAndDrop().
		 */
		void setEnableDropAndDrop(const bool state = true);

	protected:

		/**
		 * Creates a new window.
		 * Beware: If a derived window class will use a different window class as the default one you have to change the window class name in the constructor.
		 * @param applicationInstance The instance of the application, must be valid
		 * @param name The name of the application window
		 * @param parent Possible parent window making this window to a child window
		 * @param isChild True, if the window is intended to be a child window
		 */
		Window(HINSTANCE applicationInstance, const std::wstring& name, const HWND parent = nullptr, const bool isChild = false);

		/**
		 * Destructs a window.
		 */
		virtual ~Window();

		/**
		 * Registers a new windows class for the application window.
		 * @param icon The optional handle of the icon to be used, nullptr to use default icon
		 * @return True, if succeeded
		 */
		virtual bool registerWindowClass(const HICON icon = nullptr);

		/**
		 * Creates the window itself using the registered window class.
		 * @return True, if succeeded
		 */
		virtual bool createWindow();

		/**
		 * Allows the modification of the window class before registration.<br>
		 * Overload this function to change the window type.<br>
		 * However do not change the class name.<br>
		 * @param windowClass Window class to modify
		 */
		virtual void modifyWindowClass(WNDCLASSW& windowClass);

		/**
		 * Allows the modification of the window style before creation.<br>
		 * Overload this function to change the window style.
		 * @param windowStyle Window style to modify
		 * @param windowLeft Left position of the window
		 * @param windowTop Top position of the window
		 * @param windowWidth Width of the window
		 * @param windowHeight Height of the window
		 */
		virtual void modifyWindowStyle(DWORD& windowStyle, int& windowLeft, int& windowTop, int& windowWidth, int& windowHeight);

		/**
		 * Event function if the windows has been initialized successfully.
		 */
		virtual void onInitialized();

		/**
		 * Event function for an activate event.
		 * @param active True, if the window is activated, false if the window is inactivated
		 */
		virtual void onActivate(const bool active);

		/**
		 * Event function to repaint the window.
		 */
		virtual void onPaint();

		/**
		 * Function called by the windows message loop if the process is idling.
		 */
		virtual void onIdle();

		/**
		 * Function for a show event.
		 * @param visible True, if the window is being shown, false if the window is being hidden
		 */
		virtual void onShow(const bool visible);

		/**
		 * Function for a destroy event.
		 */
		virtual void onDestroy();

		/**
		 * Function for window resize event.
		 * @param clientWidth New client width
		 * @param clientHeight New client height
		 */
		virtual void onResize(const unsigned int clientWidth, const unsigned int clientHeight);

		/**
		 * Function for keyboard button down events.
		 * @param key Specifies the key button
		 */
		virtual void onKeyDown(const int key);

		/**
		 * Function for mouse double click events.
		 * @param button Specifies the mouse button
		 * @param x Specifies the x position of the cursor
		 * @param y Specifies the y position of the cursor
		 */
		virtual void onMouseDoubleClick(const MouseButton button, const int x, const int y);

		/**
		 * Function for mouse button down events.
		 * @param button Specifies the mouse button
		 * @param x Specifies the x position of the cursor
		 * @param y Specifies the y position of the cursor
		 */
		virtual void onMouseDown(const MouseButton button, const int x, const int y);

		/**
		 * Function for mouse move event.
		 * @param buttons Holds all pushed mouse buttons
		 * @param x Specifies the x position of the cursor
		 * @param y Specifies the y position of the cursor
		 */
		virtual void onMouseMove(const MouseButton buttons, const int x, const int y);

		/**
		 * Function for keyboard button up events.
		 * @param key Specifies the key button
		 */
		virtual void onKeyUp(const int key);

		/**
		 * Function for mouse button up events.
		 * @param button Specifies the mouse button
		 * @param x Specifies the x position of the cursor
		 * @param y Specifies the y position of the cursor
		 */
		virtual void onMouseUp(const MouseButton button, const int x, const int y);

		/**
		 * Functions for mouse wheel events.
		 * @param buttons Holds all pushed mouse buttons
		 * @param wheel Specifies the wheel delta value
		 * @param x Specifies the x position of the cursor
		 * @param y Specifies the y position of the cursor
		 */
		virtual void onMouseWheel(const MouseButton buttons, const int wheel, const int x, const int y);

		/**
		 * Functions for window minimize event.
		 */
		virtual void onMinimize();

		/**
		 * Event functions for drag&drop events for files.
		 * @param files The files which have been dropped
		 * @see setEnableDropAndDrop().
		 */
		virtual void onDragAndDrop(const Files& files);

		/**
		 * Window message / event procedure.
		 * @param hwnd Window handle
		 * @param uMsg Message id
		 * @param wParam High message parameter
		 * @param lParam Low message parameter
		 */
		static LRESULT CALLBACK windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		/**
		 * Returns the class map for all windows.
		 * @return The window class map
		 */
		static ClassMap& classMap();

		/**
		 * Returns the lock of the window class map.
		 * @return The window class map's lock
		 */
		static Lock& classMapLock();

	protected:

		/// Window handle of a possible parent window, making this window to a child window.
		HWND parentHandle_ = nullptr;

		/// Window class name.
		std::wstring className_;

		/// Application instance.
		HINSTANCE applicationInstance_ = nullptr;

		/// Name of the window.
		std::wstring name_;

		/// Window handle.
		HWND handle_ = nullptr;

		/// Window device context.
		HDC dc_ = nullptr;

		/// True, if the window is a child window.
		bool isChild_ = false;
};

inline bool Window::initialize(const HINSTANCE applicationInstance, const int iconId, const std::string& windowClass)
{
	ocean_assert(applicationInstance != nullptr);
	ocean_assert(iconId >= 0);

	const HICON hIcon = LoadIcon(applicationInstance, MAKEINTRESOURCE(iconId));

	return initialize(hIcon, windowClass);
}

inline HINSTANCE Window::applicationInstance() const
{
	return applicationInstance_;
}

inline const std::wstring& Window::name() const
{
	return name_;
}

inline HWND Window::handle() const
{
	return handle_;
}

inline HWND Window::parentHandle() const
{
	return parentHandle_;
}

inline HDC Window::dc() const
{
	return dc_;
}

}

}

}

#endif // META_OCEAN_PLATFORM_WIN_WINDOW_H
