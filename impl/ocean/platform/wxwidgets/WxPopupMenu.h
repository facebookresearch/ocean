/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_WXWIDGETS_WX_POPUP_MENU_H
#define META_OCEAN_PLATFORM_WXWIDGETS_WX_POPUP_MENU_H

#include "ocean/platform/wxwidgets/WxWidgets.h"

namespace Ocean
{

namespace Platform
{

namespace WxWidgets
{

/**
 * This class implements a popup menu.
 * @ingroup platformwxwidgets
 */
class OCEAN_PLATFORM_WXWIDGETS_EXPORT WxPopupMenu : public wxMenu
{
	public:

		/**
		 * Creates a new popup menu.
		 */
		WxPopupMenu() = default;

		/**
		 * Pops this menu up.
		 * @param parent The parent window, must be valid
		 * @param x The horizontal popup position
		 * @param y The vertical popup position
		 * @param invalidId The resulting id if no menu entry has been selected
		 * @return The id of the selected menu entry, or invalidId
		 */
		int popup(wxWindow& parent, const int x = -1, const int y = -1, const int invalidId = -1);

		/**
		 * Pops this menu up if no associated parent window is known.
		 * @param x The horizontal popup position
		 * @param y The vertical popup position
		 * @param invalidId The resulting id if no menu entry has been selected
		 * @return The id of the selected menu entry, or invalidId
		 */
		int popup(const int x = -1, const int y = -1, const int invalidId = -1);

	protected:

		/**
		 * Exchanges the default event handler of all sub menus by the event handler from the root menu.
		 * @param rootMenu The root menu providing the event handler
		 * @param menu The menu which's event handler will be exchanged, may be the rootMenu
		 */
		static void exchangeEventHandler(wxMenu& rootMenu, wxMenu& menu);

		/**
		 * Menu click event function.
		 * @param event The event object
		 */
		void onPopupMenuClick(wxCommandEvent& event);

	protected:

		/// The selected id.
		int menuId_ = -1;
};

}

}

}

#endif // META_OCEAN_PLATFORM_WXWIDGETS_WX_POPUP_MENU_H
