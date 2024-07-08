/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_OCEAN_APPLICATION_OCEAN_DEMO_MISC_APPINSPECTOR_APPLICATION_VIEW_H
#define FACEBOOK_OCEAN_APPLICATION_OCEAN_DEMO_MISC_APPINSPECTOR_APPLICATION_VIEW_H

#include "application/ocean/demo/misc/appinspector/osx/AppDelegate.h"
#include "application/ocean/demo/misc/appinspector/osx/SymbolView.h"

/**
 * This class implements the main view for the analyzed symbols.
 * The view does not show a call graph but shows a list of all existing symbols,<br>
 * or a subset based on filter parameters.
 * ingroup applicationdemomiscappinspectorosx
 */
class ApplicationView : public SymbolView
{
	public:

		/**
		 * Definition of individual ordering modes.
		 */
		enum OrderMode
		{
			/// Symbols are ordered ascending by their id, i.e. as their appear in the binary.
			OM_ID_ASCENDING,
			/// Symbols are ordered ascending by their binary sizes.
			OM_SIZE_ASCENDING,
			/// Symbols are ordered descending by their binary sizes.
			OM_SIZE_DESCENDING,
			/// Symbols are ordered ascending by their names.
			OM_NAME_ASCENDING,
			/// Symbols are ordered descending by their names.
			OM_NAME_DESCENDING
		};

	public:

		/**
		 * Creates an invalid view object.
		 */
		ApplicationView();

		/**
		 * Creates a new view with specified size and dimension.
		 * @param rect The rect defining the size and dimension
		 * @param binaryAnalyzer The binary analyzer providing the data that will be visualized
		 */
		ApplicationView(const NSRect& rect, const BinaryAnalyzerRef& binaryAnalyzer);

		/**
		 * Move constructor.
		 * @param symbolView The view object to be moved
		 */
		ApplicationView(ApplicationView&& symbolView);

		/**
		 * Destructs a view object.
		 */
		virtual ~ApplicationView();

		/**
		 * Sets a filter for symbol names.
		 * @param text The string that must be part of a symbol
		 */
		void setSymbolFilterText(const std::string& text);

		/**
		 * Sets whether the symbol filter is case sensitive.
		 * @param caseSensitive True, if the filter for the symbol names is case sensitive
		 */
		void setSymbolFilterCaseSensitive(const bool caseSensitive);

		/**
		 * Sets whether only root symbols are displayed or whether all symbols are displayed.
		 * @param showRootSymbolsOnly True, to show root symbols only; False, to show any symbols
		 */
		void setShowRootSymbolsOnly(const bool showRootSymbolsOnly);

		/**
		 * Sets whether child symbols are displayed or hided.
		 * @param showChildSymbols True, to show child symbols; False, to hide child symbols
		 */
		void setShowChildSymbols(const bool showChildSymbols);

		/**
		 * Updates the view, call this function if the binary analyzer holds new data.
		 */
		void update();

		/**
		 * Move operator.
		 * @param view The view object to be moved
		 * @return The reference to this object
		 */
		ApplicationView& operator=(ApplicationView&& view);

	protected:

		/**
		 * Disabled copy constructor.
		 * @param view The view object to be copied
		 */
		ApplicationView(const ApplicationView& view) = delete;

		/**
		 * Disabled assign operator.
		 * @param view The view object to be assigned
		 * @return The reference to this objec
		 */
		ApplicationView& operator=(const ApplicationView& view) = delete;

		/**
		 * Adds all symbols from the analyzer.
		 * @return True, if succeeded
		 */
		bool addSymbols();

		/**
		 * The function for header click events.
		 * @see TreeView::onMouseDownInHeader().
		 */
		void onMouseDownInHeader(const unsigned int columnIndex) override;

		/**
		 * The item selected event function.
		 * @see TreeView::onItemSelected().
		 */
		void onItemSelected(const TreeItemId itemId) override;

		/**
		 * Shows a context menu, may extend the menu with new entries and returns the user's selection.
		 * @see ItemView::showContextMenu().
		 */
		int showContextMenu(Platform::Apple::MacOS::ContextMenu& contextMenu) override;

		/**
		 * Sorts symbol ids based on a specified order mode.
		 * @param symbols All existing symbolds
		 * @param subsetSymbolIds The ids of the symbolds to be sorted, can be a subset of all possible symbol ids
		 * @param orderMode The order mode to be used
		 * @tparam T The container data type of the given symbol ids
		 */
		template <typename T>
		static void sortSymbols(const BinaryAnalyzer::Symbols& symbols, T& subsetSymbolIds, const OrderMode orderMode);

	protected:

		/// The current order mode.
		OrderMode orderMode_ = OM_ID_ASCENDING;

		/// The filter for the symbol names.
		std::string symbolFilterText_;

		/// True, if the filter for the symbol names is case sensitive.
		bool symbolFilterCaseSensitive_ = false;

		/// True, to show root symbols only; False, to show any symbols.
		bool showRootSymbolsOnly_ = false;

		/// True, to show child symbols; False, to hide child symbols.
		bool showChildSymbols_ = false;
};

template <typename T>
void ApplicationView::sortSymbols(const BinaryAnalyzer::Symbols& symbols, T& subsetSymbolIds, const OrderMode orderMode)
{
	switch (orderMode)
	{
		case OM_ID_ASCENDING:
			// nothing to do here as we exect the ids to be sorted already
			break;

		case OM_SIZE_ASCENDING:
			BinaryAnalyzer::sortSymboldsBySize(symbols, subsetSymbolIds, true);
			break;

		case OM_SIZE_DESCENDING:
			BinaryAnalyzer::sortSymboldsBySize(symbols, subsetSymbolIds, false);
			break;

		case OM_NAME_ASCENDING:
			BinaryAnalyzer::sortSymboldsByReadableName(symbols, subsetSymbolIds, true);
			break;

		case OM_NAME_DESCENDING:
			BinaryAnalyzer::sortSymboldsByReadableName(symbols, subsetSymbolIds, false);
			break;
	}
}

#endif // FACEBOOK_OCEAN_APPLICATION_OCEAN_DEMO_MISC_APPINSPECTOR_APPLICATION_VIEW_H
