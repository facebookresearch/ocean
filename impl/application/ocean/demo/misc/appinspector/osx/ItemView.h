/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_OCEAN_APPLICATION_OCEAN_DEMO_MISC_APPINSPECTOR_ITEM_VIEW_H
#define FACEBOOK_OCEAN_APPLICATION_OCEAN_DEMO_MISC_APPINSPECTOR_ITEM_VIEW_H

#include "application/ocean/demo/misc/appinspector/osx/AppDelegate.h"

#include "application/ocean/demo/misc/appinspector/BinaryAnalyzer.h"

#include "ocean/base/Callback.h"

#include "ocean/platform/apple/macos/ContextMenu.h"
#include "ocean/platform/apple/macos/TreeView.h"

#include <unordered_map>

/**
 * This class implements the base class for all items.
 * Items are either symbols, strings or data elements.<br>
 * In general, this view is able to connect view items with objects from the binary analyzer.<br>
 * Further, the view provides a basic context menu.
 * @ingroup applicationdemomiscappinspectorosx
 */
class ItemView : public Platform::Apple::MacOS::TreeView
{
	public:

		/**
		 * Defintion of a set holding object ids.
		 */
		typedef std::unordered_set<size_t> ObjectIdSet;

		/// Definition of an invalid object id.
		const static size_t invalidObjectId = size_t(-1);

	protected:

		/**
		 * Definition of a map mapping tree item ids to object ids.
		 */
		typedef std::unordered_map<TreeItemId, size_t> TreeItemId2ObjectIdMap;

	public:

		/**
		 * Creates an invalid view object.
		 */
		ItemView();

		/**
		 * Creates a new view with specified size and dimension.
		 * @param rect The rect defining the size and dimension
		 * @param binaryAnalyzer The binary analyzer providing the data that will be visualized
		 * @param columnNames The names of the columns, at least one
		 */
		explicit ItemView(const NSRect& rect, const BinaryAnalyzerRef& binaryAnalyzer, const Strings& columnNames);

		/**
		 * Move constructor.
		 * @param itemView The view object to be moved
		 */
		ItemView(ItemView&& itemView);

		/**
		 * Disabled copy constructor.
		 * @param itemView The view object to be moved
		 */
		ItemView(const ItemView& itemView) = delete;

		/**
		 * Destructs a view object.
		 */
		virtual ~ItemView();

		/**
		 * Returns the ids of all currently selected objects (either a symbol, a string, o a data element).
		 * @return The object ids
		 */
		ObjectIdSet selectedObjectIds() const;

		/**
		 * Move operator.
		 * @param itemView The view object to be moved
		 * @return Reference to this object
		 */
		ItemView& operator=(ItemView&& itemView);

	protected:

		/**
		 * The function for right mouse button up events.
		 * @see TreeView::onMouseUpRight().
		 */
		void onMouseUpRight() override;

		/**
		 * Shows a context menu, may extend the menu with new entries and returns the user's selection.
		 * @param contextMenu The context menu to be shown, contains entries already
		 * @return The user's selection
		 */
		virtual int showContextMenu(Platform::Apple::MacOS::ContextMenu& contextMenu);

		/**
		 * Registers a new tree item with an object id.
		 * @param treeItemId The id of the tree item which will be registered with a specified object id
		 * @param objectId The id of the object belonging to the tree item
		 */
		inline void registerTreeItem(const TreeItemId treeItemId, const size_t& objectId);

		/**
		 * Returns the object id for a specified tree item id.
		 * @param treeItemId The id of the tree item for which the corresponding object id will be returned
		 * @return The object id corresponding to the given tree item id, invalidObjectId if the tree item does not exist
		 */
		inline size_t objectId(const TreeItemId& treeItemId) const;

		/**
		 * Disabled assign operator.
		 * @param itemView The item view object to be assigned
		 * @return Reference to this object
		 */
		ItemView& operator=(const ItemView& itemView) = delete;

	protected:

		/// The binary analyzer providing the data for this item view.
		BinaryAnalyzerRef binaryAnalyzer_;

		/// The map mapping tree item ids to object ids.
		TreeItemId2ObjectIdMap treeItemId2ObjectIdMap_;
};

inline void ItemView::registerTreeItem(const TreeItemId treeItemId, const size_t& objectId)
{
	ocean_assert(treeItemId2ObjectIdMap_.find(treeItemId) == treeItemId2ObjectIdMap_.end());
	treeItemId2ObjectIdMap_.insert(std::make_pair(treeItemId, objectId));
}

inline size_t ItemView::objectId(const TreeItemId& treeItemId) const
{
	const TreeItemId2ObjectIdMap::const_iterator i = treeItemId2ObjectIdMap_.find(treeItemId);

	if (i == treeItemId2ObjectIdMap_.end())
		return invalidObjectId;

	return i->second;
}

#endif // FACEBOOK_OCEAN_APPLICATION_OCEAN_DEMO_MISC_APPINSPECTOR_ITEM_VIEW_H
