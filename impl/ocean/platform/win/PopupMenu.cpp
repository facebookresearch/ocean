/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/win/PopupMenu.h"

#include "ocean/platform/win/Mouse.h"

namespace Ocean
{

namespace Platform
{

namespace Win
{

PopupMenu::PopupMenu()
{
	handle_ = CreatePopupMenu();
}

PopupMenu::~PopupMenu()
{
	for (PopupMenus::iterator i = subMenus_.begin(); i != subMenus_.end(); ++i)
	{
		ocean_assert(*i != nullptr);
		delete *i;
	}

	if (handle_)
		DestroyMenu(handle_);
}

size_t PopupMenu::size() const
{
	ocean_assert(handle_);

	const int number = GetMenuItemCount(handle_);

	ocean_assert(number >= 0);
	if (number < 0)
		return 0;

	return size_t(number);
}

bool PopupMenu::addEntry(const std::wstring& text, const unsigned int id, const bool checked)
{
	ocean_assert(handle_);

	if (checked)
		return AppendMenuW(handle_, MF_STRING | MF_CHECKED, id, text.c_str()) == TRUE;
	else
		return AppendMenuW(handle_, MF_STRING, id, text.c_str()) == TRUE;
}

bool PopupMenu::addSeparator()
{
	ocean_assert(handle_);

	return AppendMenuW(handle_, MF_SEPARATOR, 0, nullptr) == TRUE;
}

PopupMenu& PopupMenu::addMenu(const std::wstring& text)
{
	ocean_assert(handle_);

	PopupMenu* menu = new PopupMenu();
	ocean_assert(menu != nullptr);

	const bool result = AppendMenuW(handle_, MF_POPUP, UINT_PTR(menu->handle_), text.c_str()) == TRUE;
	ocean_assert_and_suppress_unused(result, result);

	subMenus_.push_back(menu);
	return *menu;
}

unsigned int PopupMenu::show(const int x, const int y, const HWND parent)
{
	ocean_assert(handle_);
	ocean_assert(parent);

	return (unsigned int)TrackPopupMenu(handle_, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, x, y, 0, parent, nullptr);
}

unsigned int PopupMenu::show(const HWND parent)
{
	ocean_assert(parent);

	const VectorI2 cursorPosition(Mouse::screenPosition());
	return show(cursorPosition.x(), cursorPosition.y(), parent);
}

}

}

}
