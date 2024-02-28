// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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

WxScopedParentWindow::WxScopedParentWindow(WXHWND handle)
{
#ifdef _WINDOWS

	window_ = new wxWindow();
	window_->SetHWND(handle);
	assocated_ = true;

#elif defined(__APPLE__)

	ocean_assert(!handle && "Not supported for this platform!");
	window_ = new wxFrame(nullptr, -1, L"WxScopedParentWindow");

#else

	#warning Missing implementation

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
	if (window_)
	{
#ifdef _WINDOWS
		if (assocated_)
			window_->SetHWND(nullptr);
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
