/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_APPLE_MACOS_CONTEXT_MENU_H
#define META_OCEAN_PLATFORM_APPLE_MACOS_CONTEXT_MENU_H

#include "ocean/platform/apple/macos/MacOS.h"

#ifndef __OBJC__
	#error Platform::Apple::MacOS::ContextMenu.h needs to be included from an ObjectiveC++ file
#endif

#include <AppKit/AppKit.h>

namespace Ocean
{

namespace Platform
{

namespace Apple
{

namespace MacOS
{

/**
 * This class implements a context menu for MacOS platforms.
 * Mainly, this class wraps a NSMenu object.
 * @ingroup platformapplemacos
 */
class ContextMenu
{
	protected:

		/**
		 * Definition of a vector holding a string.
		 */
		typedef std::vector<std::string> Strings;

	public:

		/**
		 * Creates a new context menu object.
		 */
		ContextMenu();

		/**
		 * Disabled copy constructor preventing to copy a ContextMenu object.
		 * @param contextMenu The context menu to be copied
		 */
		ContextMenu(const ContextMenu& contextMenu) = delete;

		/**
		 * Creates a new context menu object.
		 * @param contextMenu The context menu to be moved
		 */
		inline ContextMenu(ContextMenu&& contextMenu) noexcept;

		/**
		 * Adds a new item to the context menu.
		 * @param text The text of the item to be added, "-" for a separator item
		 * @param enabled True, to enabled the item; False, to disable the item
		 * @return The index of the new item, with range [0, infinity)
		 */
		int addItem(const std::string& text, const bool enabled = true);

		/**
		 * Popups the context menu, blocks the main event queue, and does not return unti the user has made a decision.
		 * @return The index of the item the user selected, -1 if the user did not select any item
		 * @see addItem().
		 */
		int popup();

		/**
		 * Internal event functions for a selected menu item, should never be called from outside.
		 * @param item The item that has been selected
		 */
		void onItemSelected(NSMenuItem* item);

		/**
		 * Disabled assign operator preventing to assign a context menu.
		 * @param contextMenu The context menu to be assigned
		 * @return Reference to this object
		 */
		ContextMenu& operator=(const ContextMenu& contextMenu) = delete;

		/**
		 * Move operator.
		 * @param contextMenu The context menu to be moved
		 * @return Reference to this object
		 */
		ContextMenu& operator=(ContextMenu&& contextMenu) noexcept;

	protected:

		/// The actual MacOS context menu wrapped by this class.
		NSMenu* contextMenuNSMenu;

		/// The items of the context menus.
		Strings contextMenuItemTexts;

		/// The index of the item that has been selected by the user, -1 if no item has been selected.
		int contextSelectedIndex;
};

ContextMenu::ContextMenu(ContextMenu&& contextMenu) noexcept :
	ContextMenu()
{
	*this = std::move(contextMenu);
}

}

}

}

}

#endif // META_OCEAN_PLATFORM_APPLE_MACOS_CONTEXT_MENU_H
