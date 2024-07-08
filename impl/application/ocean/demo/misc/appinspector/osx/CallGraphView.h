/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_OCEAN_APPLICATION_OCEAN_DEMO_MISC_APPINSPECTOR_CALL_GRAPH_VIEW_H
#define FACEBOOK_OCEAN_APPLICATION_OCEAN_DEMO_MISC_APPINSPECTOR_CALL_GRAPH_VIEW_H

#include "application/ocean/demo/misc/appinspector/osx/AppDelegate.h"
#include "application/ocean/demo/misc/appinspector/osx/SymbolView.h"

/**
 * This view implements the visualization of a call graph for all symbols analyzed.
 * In addition, the view extends the context menu of SymbolView allowing to show call grpahs for selected symbols.
 * @ingroup applicationdemomiscappinspectorosx
 */
class CallGraphView : public SymbolView
{
	public:

		/**
		 * Creates an invalid view object.
		 */
		CallGraphView();

		/**
		 * Creates a new view with specified size and dimension.
		 * @param rect The rect defining the size and dimension
		 * @param binaryAnalyzer The binary analyzer providing the data that will be visualized
		 * @param callGraphRootNodes The root nodes of the call graph to be displayed, will be moved
		 */
		CallGraphView(const NSRect& rect, const BinaryAnalyzerRef& binaryAnalyzer, BinaryAnalyzer::GraphNodeRefs&& callGraphRootNodes);

		/**
		 * Move constructor.
		 * @param callGraphView The view object to be moved
		 */
		CallGraphView(CallGraphView&& callGraphView);

		/**
		 * Destructs a view object.
		 */
		virtual ~CallGraphView();

		/**
		 * Updates the view, call this function if the binary analyzer holds new data.
		 */
		void update();

		/**
		 * Move operator.
		 * @param callGraphView The view object to be moved
		 * @return The reference to this object
		 */
		CallGraphView& operator=(CallGraphView&& callGraphView);

	protected:

		/**
		 * Disabled copy constructor.
		 * @param callGraphView The view object to be copied
		 */
		CallGraphView(const CallGraphView& callGraphView) = delete;

		/**
		 * Disabled assign operator.
		 * @param callGraphView The view object to be assigned
		 * @return The reference to this objec
		 */
		CallGraphView& operator=(const CallGraphView& callGraphView) = delete;

		/**
		 * Adds the symbols of the call graph.
		 * @return True, if succeeded
		 */
		bool addGraph();

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

	protected:

		/// The nodes of the call graph.
		BinaryAnalyzer::GraphNodeRefs callGraphRootNodes_;
};

#endif // FACEBOOK_OCEAN_APPLICATION_OCEAN_DEMO_MISC_APPINSPECTOR_CALL_GRAPH_VIEW_H
