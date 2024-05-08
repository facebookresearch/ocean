/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/win/ApplicationWindow.h"

#include "ocean/base/String.h"

namespace Ocean
{

namespace Platform
{

namespace Win
{

ApplicationWindow::ApplicationWindow(HINSTANCE instance, const std::wstring& name) :
	Window(instance, name)
{
	// nothing to do here
}

ApplicationWindow::~ApplicationWindow()
{
	// nothing to do here
}

int ApplicationWindow::start()
{
	if (handle() == nullptr)
	{
		return 0;
	}

	show();
	update();

	MSG msg;

	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if (applicationAboutToClosed_ == false)
		{
			onIdle();
		}
	}

	return int(msg.wParam);
}

void ApplicationWindow::close()
{
	applicationAboutToClosed_ = true;
	PostQuitMessage(0);
}

void ApplicationWindow::onDestroy()
{
	applicationAboutToClosed_ = true;
	PostQuitMessage(0);
}

}

}

}
