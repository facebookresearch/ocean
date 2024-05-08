/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/win/TextWindow.h"

namespace Ocean
{

namespace Platform
{

namespace Win
{

TextWindow::TextWindow(HINSTANCE instance, const std::wstring& name, const bool isChild) :
	Window(instance, name, nullptr, isChild)
{
	// nothing to do here
}

TextWindow::~TextWindow()
{
	// nothing to do here
}

std::string TextWindow::text() const
{
	if (handle_ == nullptr)
		return std::string();

	const int textLength = GetWindowTextLengthA(handle_);

	std::vector<char> buffer(textLength);
	GetWindowTextA(handle_, buffer.data(), int(buffer.size()));

	return std::string(buffer.data(), size_t(buffer.size()));
}

bool TextWindow::setText(const std::string& text)
{
	if (handle_ == nullptr)
		return false;

	return SetWindowTextA(handle_, text.c_str()) == TRUE;
}

bool TextWindow::appendText(const std::string& text)
{
	if (text.empty())
		return true;

	if (handle_ == nullptr)
		return false;

	const int textLength = GetWindowTextLengthA(handle_);

	SendMessageA(handle_, EM_SETSEL, textLength, textLength);
	SendMessageA(handle_, EM_REPLACESEL, FALSE, (LPARAM)text.c_str());

	return true;
}

bool TextWindow::clearText()
{
	if (handle_ == nullptr)
		return false;

	SendMessageA(handle_, WM_CLEAR, 0, 0);
	return true;
}

bool TextWindow::createWindow()
{
	ocean_assert(handle_ == nullptr);
	if (handle_ != nullptr)
		return true;

	DWORD windowStyle = ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | WS_VSCROLL;

	if (isChild_)
		windowStyle |= WS_CHILD;
	else
		windowStyle |= WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

	int windowX = CW_USEDEFAULT;
	int windowY = CW_USEDEFAULT;
	int windowWidth = CW_USEDEFAULT;
	int windowHeight = CW_USEDEFAULT;

	modifyWindowStyle(windowStyle, windowX, windowY, windowWidth, windowHeight);

	handle_ = CreateWindowW(L"EDIT", name_.c_str(), windowStyle, windowX, windowY, windowWidth, windowHeight, parentHandle_, nullptr, applicationInstance_, nullptr);
	dc_ = GetDC(handle_);

	SetWindowLongPtr(handle_, GWLP_USERDATA, (LONG_PTR)this);

	if (handle_ == nullptr)
		return false;

	// sets the largest possible text limit
	SendMessageA(handle_, EM_LIMITTEXT, 0, 0);

	return true;
}

}

}

}
