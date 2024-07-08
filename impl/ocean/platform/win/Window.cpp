/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/win/Window.h"

#include "ocean/base/String.h"

#include <shellapi.h>

namespace Ocean
{

namespace Platform
{

namespace Win
{

Window::Window(HINSTANCE applicationInstance, const std::wstring& name, const HWND parent, const bool isChild) :
	parentHandle_(parent),
	className_(L"window"),
	applicationInstance_(applicationInstance),
	name_(name),
	isChild_(isChild)
{
	ocean_assert(applicationInstance_);
	ocean_assert(name_.empty() == false);
}

Window::~Window()
{
	if (handle())
	{
		const ScopedLock scopedLock(classMapLock());

		ClassMap& windowClassMap = classMap();

		ClassMap::iterator iClass = windowClassMap.find(className_);
		ocean_assert(iClass != windowClassMap.end());

		ocean_assert(iClass->second != 0u);
		--iClass->second;

		if (iClass->second == 0u)
		{
			UnregisterClass(className_.c_str(), applicationInstance_);
			windowClassMap.erase(iClass);
		}
	}

	ReleaseDC(handle_, dc_);
}

bool Window::initialize(const HICON icon, const std::string& windowClass)
{
	if (!windowClass.empty())
	{
		className_ = String::toWString(windowClass);
	}

	if (!className_.empty())
	{
		const ScopedLock scopedLock(classMapLock());

		ClassMap& windowClassMap = classMap();

		ClassMap::iterator iClass = windowClassMap.find(className_);
		if (iClass == windowClassMap.end())
		{
			if (registerWindowClass(icon) == false)
			{
				return false;
			}

			iClass = windowClassMap.emplace(className_, 0u).first;
		}

		++iClass->second;
	}

	if (createWindow() == false)
	{
		return false;
	}

	if (handle())
	{
		onInitialized();
		return true;
	}

	return false;
}

bool Window::setParent(const HWND parent)
{
	if (handle())
	{
		return false;
	}

	parentHandle_ = parent;
	return true;
}

void Window::setText(const std::wstring& text)
{
	ocean_assert(handle_ != HWND(0));

	SetWindowTextW(handle_, text.c_str());
}

void Window::setEnableDropAndDrop(const bool state)
{
	ocean_assert(handle_ != HWND(0));

	DragAcceptFiles(handle_, state ? TRUE : FALSE);
}

void Window::show()
{
	ShowWindow(handle(), SW_SHOW);
}

void Window::hide()
{
	ShowWindow(handle(), SW_HIDE);
}

void Window::update()
{
	UpdateWindow(handle());
}

void Window::repaint(const bool eraseBackground)
{
	InvalidateRect(handle(), nullptr, eraseBackground ? TRUE : FALSE);
}

bool Window::move(const int x, const int y)
{
	if (handle() == nullptr)
	{
		return false;
	}

	SetWindowPos(handle(), nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	return true;
}

bool Window::resize(const unsigned int width, const unsigned int height)
{
	if (handle() == nullptr || width == 0 || height == 0)
	{
		return false;
	}

	SetWindowPos(handle(), nullptr, 0, 0, int(width), int(height), SWP_NOMOVE | SWP_NOZORDER);
	return true;
}

unsigned int Window::width() const
{
	RECT rect;
	if (GetWindowRect(handle_, &rect) == TRUE)
	{
		return rect.right - rect.left;
	}

	return 0;
}

unsigned int Window::clientWidth() const
{
	RECT rect;
	if (GetClientRect(handle_, &rect) == TRUE)
	{
		return rect.right - rect.left;
	}

	return 0;
}

unsigned int Window::height() const
{
	RECT rect;
	if (GetWindowRect(handle_, &rect) == TRUE)
	{
		return rect.bottom - rect.top;
	}

	return 0;
}

unsigned int Window::clientHeight() const
{
	RECT rect;
	if (GetClientRect(handle_, &rect) == TRUE)
	{
		return rect.bottom - rect.top;
	}

	return 0;
}

bool Window::registerWindowClass(const HICON icon)
{
	WNDCLASSW windowClass;

	windowClass.style            = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	windowClass.lpfnWndProc      = windowProc;
	windowClass.cbClsExtra       = 0;
	windowClass.cbWndExtra       = 0;
	windowClass.hInstance        = applicationInstance_;
	windowClass.hIcon            = icon;
	windowClass.hCursor          = LoadCursor(nullptr, IDC_ARROW);
	windowClass.hbrBackground    = (HBRUSH)(COLOR_WINDOWFRAME);
	windowClass.lpszMenuName     = 0;

	modifyWindowClass(windowClass);

	windowClass.lpszClassName = className_.c_str();

	return RegisterClassW(&windowClass) != 0;
}

bool Window::createWindow()
{
	ocean_assert(handle_ == nullptr);
	if (handle_ != nullptr)
	{
		return true;
	}

	DWORD windowStyle = 0;

	if (isChild_)
	{
		windowStyle |= WS_CHILD;
	}
	else
	{
		windowStyle |= WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
	}

	int windowX = CW_USEDEFAULT;
	int windowY = CW_USEDEFAULT;
	int windowWidth = CW_USEDEFAULT;
	int windowHeight = CW_USEDEFAULT;

	modifyWindowStyle(windowStyle, windowX, windowY, windowWidth, windowHeight);

	handle_ = CreateWindow(className_.c_str(), name_.c_str(), windowStyle, windowX, windowY, windowWidth, windowHeight, parentHandle_, nullptr, applicationInstance_, nullptr);
	dc_ = GetDC(handle_);

	SetWindowLongPtr(handle_, GWLP_USERDATA, (LONG_PTR)this);

	if (handle_ == nullptr)
	{
		return false;
	}

	return true;
}

void Window::modifyWindowClass(WNDCLASSW& /*windowClass*/)
{
	// nothing to do here
}

void Window::modifyWindowStyle(DWORD& /*windowStyle*/, int& /*windowLeft*/, int& /*windowTop*/, int& /*windowWidth*/, int& /*windowHeight*/)
{
	// nothing to do here
}

void Window::onInitialized()
{
	// nothing to do here
}

void Window::onActivate(const bool /*active*/)
{
	// nothing to do here
}

void Window::onPaint()
{
	// nothing to do here
}

void Window::onIdle()
{
	Sleep(1);
}

void Window::onShow(const bool /*visible*/)
{
	// nothing to do here
}

void Window::onDestroy()
{
	// nothing to do here
}

void Window::onResize(const unsigned int /*clientWidth*/, const unsigned int /*clientHeight*/)
{
	// nothing to do here
}

void Window::onKeyDown(const int /*key*/)
{
	// nothing to do here
}

void Window::onMouseDoubleClick(const MouseButton /*button*/, const int /*x*/, const int /*y*/)
{
	// nothing to do here
}

void Window::onMouseDown(const MouseButton /*button*/, const int /*x*/, const int /*y*/)
{
	// nothing to do here
}

void Window::onMouseMove(const MouseButton /*buttons*/, const int /*x*/, const int /*y*/)
{
	// nothing to do here
}

void Window::onKeyUp(const int /*key*/)
{
	// nothing to do here
}

void Window::onMouseUp(const MouseButton /*button*/, const int /*x*/, const int /*y*/)
{
	// nothing to do here
}

void Window::onMouseWheel(const MouseButton /*buttons*/, const int /*wheel*/, const int /*x*/, const int /*y*/)
{
	// nothing to do here
}

void Window::onMinimize()
{
	// nothing to do here
}

void Window::onDragAndDrop(const Files& /*files*/)
{
	// nothing to do here
}

LRESULT CALLBACK Window::windowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	const LONG_PTR value = GetWindowLongPtr(hWnd, GWLP_USERDATA);

	/// Default message handling
	if (uMsg == WM_COMMAND || value == 0)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	Window* window = reinterpret_cast<Window*>(value);

	switch (uMsg)
	{
		case WM_ACTIVATE:
			window->onActivate(LOWORD(wParam) == WA_ACTIVE || LOWORD(wParam) == WA_CLICKACTIVE);
			break;

		case WM_PAINT:
		{
			LRESULT result = DefWindowProc(hWnd, uMsg, wParam, lParam);
			window->onPaint();
			return result;
		}

		case WM_SHOWWINDOW:
			window->onShow(wParam == TRUE);
			break;

		case WM_SIZE:
			ocean_assert(LOWORD(lParam) >= 0);
			ocean_assert(HIWORD(lParam) >= 0);
			window->onResize(LOWORD(lParam), HIWORD(lParam));
			break;

		case WM_KEYDOWN:
			window->onKeyDown(int(wParam));
			break;

		case WM_KEYUP:
			window->onKeyUp(int(wParam));
			break;

		case WM_LBUTTONDBLCLK:
			window->onMouseDoubleClick(BUTTON_LEFT, LOWORD(lParam), HIWORD(lParam));
			break;

		case WM_LBUTTONDOWN:
			window->onMouseDown(BUTTON_LEFT, LOWORD(lParam), HIWORD(lParam));
			break;

		case WM_LBUTTONUP:
			window->onMouseUp(BUTTON_LEFT, LOWORD(lParam), HIWORD(lParam));
			break;

		case WM_MBUTTONDOWN:
			window->onMouseDown(BUTTON_MIDDLE, LOWORD(lParam), HIWORD(lParam));
			break;

		case WM_MBUTTONUP:
			window->onMouseUp(BUTTON_MIDDLE, LOWORD(lParam), HIWORD(lParam));
			break;

		case WM_RBUTTONDBLCLK:
			window->onMouseDoubleClick(BUTTON_RIGHT, LOWORD(lParam), HIWORD(lParam));
			break;

		case WM_RBUTTONDOWN:
			window->onMouseDown(BUTTON_RIGHT, LOWORD(lParam), HIWORD(lParam));
			break;

		case WM_RBUTTONUP:
			window->onMouseUp(BUTTON_RIGHT, LOWORD(lParam), HIWORD(lParam));
			break;

		case WM_MOUSEMOVE:
		{
			MouseButton mouseButtons = BUTTON_NONE;

			if (wParam & MK_LBUTTON)
				mouseButtons = MouseButton(mouseButtons | BUTTON_LEFT);
			if (wParam & MK_MBUTTON)
				mouseButtons = MouseButton(mouseButtons | BUTTON_MIDDLE);
			if (wParam & MK_RBUTTON)
				mouseButtons = MouseButton(mouseButtons | BUTTON_RIGHT);

			window->onMouseMove(mouseButtons, LOWORD(lParam), HIWORD(lParam));
			break;
		}

		case WM_MOUSEWHEEL:
		{
			MouseButton mouseButtons = BUTTON_NONE;

			unsigned int buttonState = LOWORD(wParam);
			if (buttonState & MK_LBUTTON)
				mouseButtons = MouseButton(mouseButtons | BUTTON_LEFT);
			if (buttonState & MK_MBUTTON)
				mouseButtons = MouseButton(mouseButtons | BUTTON_MIDDLE);
			if (buttonState & MK_RBUTTON)
				mouseButtons = MouseButton(mouseButtons | BUTTON_RIGHT);

			window->onMouseWheel(mouseButtons, short(HIWORD(wParam)), LOWORD(lParam), HIWORD(lParam));
			break;
		}

		case WM_CANCELMODE:
			window->onMinimize();
			break;

		case WM_DESTROY:
			window->onDestroy();
			return 0;

		case WM_DROPFILES:
		{
			const UINT numberFiles = DragQueryFileW(HDROP(wParam), 0xFFFFFFFF, nullptr, 0);

			std::vector<std::wstring> files;

			for (UINT n = 0u; n < numberFiles; ++n)
			{
				const UINT pathSize = DragQueryFileW(HDROP(wParam), n, nullptr, 0);

				if (pathSize != 0u)
				{
					std::vector<wchar_t> buffer(pathSize + 1, wchar_t(0));
					if (DragQueryFileW(HDROP(wParam), n, buffer.data(), UINT(buffer.size())) != 0 && buffer.back() == wchar_t(0))
					{
						files.emplace_back(buffer.data());
					}
				}
			}

			DragFinish(HDROP(wParam));

			window->onDragAndDrop(files);

			return 0;
		}
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

Window::ClassMap& Window::classMap()
{
	static ClassMap classMap;
	return classMap;
}

Lock& Window::classMapLock()
{
	static Lock lock;
	return lock;
}

}

}

}
