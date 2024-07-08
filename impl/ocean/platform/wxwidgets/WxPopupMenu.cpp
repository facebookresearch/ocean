/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/wxwidgets/WxPopupMenu.h"

namespace Ocean
{

namespace Platform
{

namespace WxWidgets
{

int WxPopupMenu::popup(wxWindow& parent, const int x, const int y, const int invalidId)
{
	menuId_ = invalidId;

	int adjustedX = x;
	int adjustedY = y;

#ifdef __APPLE__
	// here we replace the default event handler of all sub menus by the event handler of the root menu as under macOS events of sub menus seem to be not forwarded
	exchangeEventHandler(*this, *this);

	if (adjustedX == -1 && adjustedY == -1)
	{
		wxPoint mouse = wxGetMousePosition();
		adjustedX = mouse.x + 10;
		adjustedY = mouse.y + 10;

		parent.ScreenToClient(&adjustedX, &adjustedY);
	}

#endif

	Connect(wxEVT_COMMAND_MENU_SELECTED, wxObjectEventFunction(wxCommandEventFunction(&WxPopupMenu::onPopupMenuClick)), nullptr, this);
	parent.PopupMenu(this, adjustedX, adjustedY);

	return menuId_;
}

int WxPopupMenu::popup(const int x, const int y, const int invalidId)
{
	wxFrame dummyFrame(nullptr, wxID_ANY, L"");

	return popup(dummyFrame, x, y, invalidId);
}

void WxPopupMenu::exchangeEventHandler(wxMenu& rootMenu, wxMenu& menu)
{
	if (&rootMenu != &menu)
	{
		menu.SetEventHandler(&rootMenu);
	}

	wxMenuItemList& items = menu.GetMenuItems();

	for (size_t n = 0; n < items.size(); ++n)
	{
		wxMenuItem* item = items[n];

		ocean_assert(item);
		wxMenu* subMenu = item ? item->GetSubMenu() : nullptr;

		if (subMenu)
		{
			exchangeEventHandler(rootMenu, *subMenu);
		}
	}
}

void WxPopupMenu::onPopupMenuClick(wxCommandEvent& event)
{
	menuId_ = event.GetId();
}

}

}

}
