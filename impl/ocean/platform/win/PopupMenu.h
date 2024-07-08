/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_WIN_POPUP_MENU_H
#define META_OCEAN_PLATFORM_WIN_POPUP_MENU_H

#include "ocean/platform/win/Win.h"

#include <vector>

namespace Ocean
{

namespace Platform
{

namespace Win
{

/**
 * This class implements a popup menu.
 * @ingroup platformwin
 */
class OCEAN_PLATFORM_WIN_EXPORT PopupMenu
{
	protected:

		/**
		 * Definition of a vector holding popup menus.
		 */
		typedef std::vector<PopupMenu*> PopupMenus;

	public:

		/**
		 * Creates a new popup menu object.
		 */
		PopupMenu();

		/**
		 * Destructs a popup menu object.
		 */
		~PopupMenu();

		/**
		 * Returns the number of menu entries (not any sub-entries).
		 * @return Number of menu entries
		 */
		size_t size() const;

		/**
		 * Adds a new menu entry.
		 * @param text The text of the menu entry
		 * @param id the unique entry id that is returned if this entry is selected, must not be zero
		 * @param checked True, if the entry is checked
		 * @return True, if succeeded
		 */
		bool addEntry(const std::wstring& text, const unsigned int id, const bool checked = false);

		/**
		 * Adds a seperator menu entry.
		 * @return True, if succeeded
		 */
		bool addSeparator();

		/**
		 * Adds a new sub-menu entry.
		 * @param text The text of the sub-menu entry
		 * @return The reference of the new sub-menu
		 */
		PopupMenu& addMenu(const std::wstring& text);

		/**
		 * Shows the menu at a specified screen position and returns the selected entry id.
		 * @param x The horizontal display position in screen coordinates
		 * @param y The vertical display position in screen coordinates
		 * @param parent The parent window handle, must not be zero
		 * @return The id of the selected menu entry, zero if no entry has been selected
		 */
		unsigned int show(const int x, const int y, const HWND parent);

		/**
		 * Shows the menu at the current cursor (mouse) position and returns the selected entry id.
		 * @param parent The parent window handle, must not be zero
		 * @return The id of the selected menu entry, zero if no entry has been selected
		 */
		unsigned int show(const HWND parent);

	protected:

		/// The handle of this menu.
		HMENU handle_ = nullptr;

		/// The sub-menus of this menu.
		PopupMenus subMenus_;
};

}

}

}

#endif // META_OCEAN_PLATFORM_WIN_POPUP_MENU_H
