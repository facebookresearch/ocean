/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/platform/wxwidgets/wxw/WxWidgetsApplication.h"
#include "application/ocean/demo/platform/wxwidgets/wxw/WxWidgetsMainWindow.h"

#include "ocean/base/Build.h"
#include "ocean/base/String.h"

// WxWidgets specific implementation of e.g. the application entry function
IMPLEMENT_APP(WxWidgetsApplication)

bool WxWidgetsApplication::OnInit()
{
	WxWidgetsMainWindow* mainWindow = new WxWidgetsMainWindow((std::wstring(L"Hello World, ") + Ocean::String::toWString(Ocean::Build::buildString())).c_str(), wxPoint(50, 50), wxSize(400, 400));
	mainWindow->Show(true);
	SetTopWindow(mainWindow);

	return true;
}
