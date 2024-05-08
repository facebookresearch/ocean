/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_OCEAN_APPLICATION_OCEAN_DEMO_MISC_APPINSPECTOR_SYMBOL_VIEW_H
#define FACEBOOK_OCEAN_APPLICATION_OCEAN_DEMO_MISC_APPINSPECTOR_SYMBOL_VIEW_H

#include "application/ocean/demo/misc/appinspector/osx/AppDelegate.h"
#include "application/ocean/demo/misc/appinspector/osx/ItemView.h"

/**
 * This class implements the base view for all symbols.
 * In addition to the ItemView functionality, this view extends the context menu allowing to copy the name of a symbol.
 * @ingroup applicationdemomiscappinspectorosx
 */
class SymbolView : public ItemView
{
	public:

		/**
		 * Creates an invalid view object.
		 */
		SymbolView();

		/**
		 * Creates a new view with specified size and dimension.
		 * @param rect The rect defining the size and dimension
		 * @param binaryAnalyzer The binary analyzer providing the data that will be visualized
		 * @param columnNames The names of the columns, at least one
		 */
		SymbolView(const NSRect& rect, const BinaryAnalyzerRef& binaryAnalyzer, const Strings& columnNames);

		/**
		 * Move constructor.
		 * @param view The view object to be moved
		 */
		SymbolView(SymbolView&& view);

		/**
		 * Destructs a view object.
		 */
		virtual ~SymbolView();

		/**
		 * Move operator.
		 * @param view The view object to be moved
		 * @return The reference to this object
		 */
		SymbolView& operator=(SymbolView&& view);

	protected:

		/**
		 * Disabled copy constructor.
		 * @param view The view object to be copied
		 */
		SymbolView(const SymbolView& view) = delete;

		/**
		 * Disabled assign operator.
		 * @param view The view object to be assigned
		 * @return The reference to this objec
		 */
		SymbolView& operator=(const SymbolView& view) = delete;

		/**
		 * Shows a context menu, may extend the menu with new entries and returns the user's selection.
		 * @see ItemView::showContextMenu().
		 */
		int showContextMenu(Platform::Apple::MacOS::ContextMenu& contextMenu) override;
};

#endif // FACEBOOK_OCEAN_APPLICATION_OCEAN_DEMO_MISC_APPINSPECTOR_SYMBOL_VIEW_H
