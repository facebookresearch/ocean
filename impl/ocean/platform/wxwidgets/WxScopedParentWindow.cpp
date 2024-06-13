/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/wxwidgets/WxScopedParentWindow.h"

namespace Ocean
{

namespace Platform
{

namespace WxWidgets
{

WxScopedParentWindow::WxScopedParentWindow()
{
	window_ = new wxFrame(nullptr, -1, L"WxScopedParentWindow");
}

WxScopedParentWindow::WxScopedParentWindow(OCEAN_WXWIDGETS_HANDLE handle)
{
#if defined(OCEAN_PLATFORM_BUILD_WINDOWS)

	window_ = new wxWindow();
	window_->SetHWND(handle);
	assocated_ = true;

#elif defined(OCEAN_PLATFORM_BUILD_APPLE)

	ocean_assert(!handle && "Not supported for this platform!");
	window_ = new wxFrame(nullptr, -1, L"WxScopedParentWindow");

#else

	#warning Missing implementation
	ocean_assert(false && "Missing implementation!");

#endif
}

WxScopedParentWindow::WxScopedParentWindow(WxScopedParentWindow&& window) noexcept :
	assocated_(window.assocated_),
	window_(window.window_)
{
	window.window_ = nullptr;
	window.assocated_ = false;
}

WxScopedParentWindow::~WxScopedParentWindow()
{
	release();
}

void WxScopedParentWindow::release()
{
	if (window_ != nullptr)
	{
#ifdef OCEAN_PLATFORM_BUILD_WINDOWS
		if (assocated_)
		{
			window_->SetHWND(nullptr);
		}
#endif

		window_->Destroy();
		window_ = nullptr;
	}
}

WxScopedParentWindow& WxScopedParentWindow::operator=(WxScopedParentWindow&& window) noexcept
{
	if (this != &window)
	{
		release();

		assocated_ = window.assocated_;
		window.assocated_ = false;

		window_ = window.window_;
		window.window_ = nullptr;
	}

	return *this;
}

wxWindow& WxScopedParentWindow::operator*() const
{
	ocean_assert(window_);
	return *window_;
}

}

}

}
