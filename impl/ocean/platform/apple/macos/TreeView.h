/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_APPLE_MACOS_TREE_VIEW_H
#define META_OCEAN_PLATFORM_APPLE_MACOS_TREE_VIEW_H

#include "ocean/platform/apple/macos/MacOS.h"

#ifndef __OBJC__
	#error Platform::Apple::MacOS::TreeView.h needs to be included from an ObjectiveC++ file
#endif

#include "ocean/platform/apple/macos/ImageList.h"

#include <memory>
#include <unordered_map>

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
 * This class implements tree view control for MacOS platforms.
 * @ingroup platformapplemacos
 */
class TreeView
{
	public:

		/**
		 * Definition of a vector holding strings.
		 */
		typedef std::vector<std::string> Strings;

		/**
		 * Definition of a unique tree item id.
		 */
		typedef uint64_t TreeItemId;

		/**
		 * Definition of a vector holding unique tree item ids.
		 */
		typedef std::vector<TreeItemId> TreeItemIds;

		/**
		 * Definition of an (unordered) map mapping item ids to item ids.
		 */
		typedef std::unordered_map<TreeItemId, TreeItemId> TreeItemResortingMap;

		/**
		 * Definition of an invalid tree item id.
		 */
		static const TreeItemId invalidTreeItemId = TreeItemId(-2);

		/**
		 * Definition of an invalid icon index.
		 */
		static const unsigned int invalidIconIndex = (unsigned int)(-1);

		/**
		 * This class implements a tree item.
		 */
		class TreeItem
		{
			friend class TreeView;

			public:

				/**
				 * Destructs a tree item.
				 */
				virtual ~TreeItem();

				/**
				 * Returns the unique id of this tree item (unique for the owning tree view).
				 * @return The item's unique id
				 */
				inline TreeItemId id() const;

				/**
				 * Returns the texts of this item.
				 * @return The item's texts, one for each column
				 */
				inline const Strings& texts() const;

				/**
				 * Returns the id of the parent item.
				 * @return THe parent's item id, an invalid id if this item is currently not shown in the tree view.
				 */
				inline TreeItemId parentId() const;

				/**
				 * Returns all children of this item.
				 * @return The ids of all children of this item
				 */
				inline const TreeItemIds& children() const;

				/**
				 * Returns all children of this item recursively.
				 * @return The ids of all children and of the children's children a.s.o.
				 */
				TreeItemIds recursiveChildren() const;

				/**
				 * Returns the index of the icon of this item.
				 * @return The item's icon index, an invalid index if the item has no icon
				 */
				inline unsigned int iconIndex() const;

				/**
				 * Returns whether this item has a specific child item.
				 * @param childId The id of the child to check
				 * @return True, if so
				 */
				bool hasChild(const TreeItemId childId) const;

				/**
				 * Removes a child from this item.
				 * @param childId The id of the child to remove, must be a valid child
				 * @return True, if succeeded
				 * @see hasChild().
				 */
				bool removeChild(const TreeItemId childId);

				/**
				 * Sets the texts of this item.
				 * @param texts The texts to set, one for each column
				 */
				void setTexts(const Strings& texts);

				/**
				 * Sets or changes the index of the icon of this item.
				 * @param index The index of the icon to set
				 */
				void setIconIndex(const unsigned int index);

				/**
				 * Sets or changes the parent item of this item.
				 * @param parentId The id of the new parent item
				 * @param followingId Optional id of the (already existing) child item of the new parent item which will become the next sibling item, an invalid id to add this item as last child item
				 * @param refreshView True, to refresh the view afterwards; False, to skip refreshing the view
				 * @return True, if succeeded
				 */
				bool setParent(const TreeItemId parentId, const TreeItemId followingId = invalidTreeItemId, const bool refreshView = true);

				/**
				 * Expands this item.
				 * @param allChildItems True, to expand also all child items
				 */
				void expand(const bool allChildItems = false) const;

			protected:

				/**
				 * Creates a new tree item object.
				 * @param owner The owner of the item
				 * @param texts The texts of the item, one for each column
				 * @param id The unique id of the item, any unique id which does not exist already, or and invalid id to automatically create an unique id
				 * @param iconIndex The item's icon index, an invalid index if the item has no icon
				 */
				explicit TreeItem(TreeView& owner, const Strings& texts, const TreeItemId id = invalidTreeItemId, const unsigned int iconIndex = invalidIconIndex);

				/**
				 * Creates a new tree item object.
				 * @param owner The owner of the item
				 * @param texts The texts of the item, one for each column, will be moved
				 * @param id The unique id of the item, any unique id which does not exist already, or and invalid id to automatically create an unique id
				 * @param iconIndex The item's icon index, an invalid index if the item has no icon
				 */
				explicit TreeItem(TreeView& owner, Strings&& texts, const TreeItemId id = invalidTreeItemId, const unsigned int iconIndex = invalidIconIndex);

			protected:

				/// The owner of this item.
				TreeView& owner_;

				/// The id of the parent item, an invalid id if this item is currently not registered in the tree view.
				TreeItemId parentId_;

				/// The ids of all child items.
				TreeItemIds childItemIds_;

				/// The unique id of this item.
				TreeItemId id_;

				/// The text of this item.
				Strings texts_;

				/// The index of the item's icon defined for the image list of the owner.
				unsigned int iconIndex_ = (unsigned int)(-1);
		};

		/**
		 * Definition of an (not thread safe) object reference holding a tree item.
		 */
		typedef std::shared_ptr<TreeItem> TreeItemRef;

	protected:

		/**
		 * Definition of a map mapping tree item ids to tree items.
		 */
		typedef std::map<TreeItemId, TreeItemRef> TreeItemMap;

	public:

		/**
		 * Creates an invalid tree view object.
		 */
		TreeView();

		/**
		 * Creates a new view object with specified size and dimension.
		 * @param rect The rect defining the size and dimension
		 * @param columnNames The names of the columns the tree view will have, at least one (optional empty) string
		 */
		explicit TreeView(const NSRect& rect, const Strings& columnNames = Strings(1, std::string()));

		/**
		 * Creates a new tree view object with specified size and dimension.
		 * @param left The horizontal start position of the control, with range (-infinity, infinity)
		 * @param top The vertical start position of the control in pixel, with range (-infinity, infinity)
		 * @param width The width of the control in pixel, with range [0, infinity)
		 * @param height The height of the control in pixel, with range [0, infinity)
		 * @param columnNames The names of the columns the tree view will have, at least one (optional empty) string
		 */
		TreeView(const double left, const double top, const double width, const double height, const Strings& columnNames = Strings(1, std::string()));

		/**
		 * Move constructor.
		 * @param treeView Tree view object to be moved
		 */
		TreeView(TreeView&& treeView) noexcept;

		/**
		 * Disabled copy constructor.
		 * @param treeView Tree view object to be moved
		 */
		TreeView(const TreeView& treeView) = delete;

		/**
		 * Destructs a tree view object.
		 */
		virtual ~TreeView();

		/**
		 * Returns the number of columns this view has.
		 * @return The views number of columns, with range [1, infinity)
		 */
		inline size_t columns() const;

		/**
		 * Enables or disables multi row selection.
		 * @param enable True, to enabled multi row selection; False, to disable multi row selection
		 */
		inline void enableMultiRowSelection(const bool enable);

		/**
		 * Returns the id of the invisible root item of this tree view.
		 * @return The tree view's root item id, which is -1
		 */
		inline TreeItemId rootItemId() const;

		/**
		 * Returns the invisible root item of this tree view.
		 * @return The tree view's root item, with item id -1
		 */
		inline const TreeItem& rootItem() const;

		/**
		 * Returns whether this tree view holds a specified item.
		 * @param itemId The id of the item to check
		 * @return True, if so
		 */
		bool hasItem(const TreeItemId itemId) const;

		/**
		 * Returns a specific item of this tree view.
		 * @param itemId The id of the requested item
		 * @return The requested item, an invalid object if the item does not exist
		 */
		TreeItemRef item(const TreeItemId itemId) const;

		/**
		 * Creates a new item for this tree view and registeres it.
		 * @param text The text of the first column of the item to be created
		 * @param parentId Optional id of the parent item, and invalid id so that the item will not directly show up in the tree view
		 * @param itemId Optional the unique id of the item, any unique id which does not exist already, or and invalid id to automatically create an unique id, while the root item has item id -1
		 * @param refreshView True, to refresh the view afterwards; False, to skip refreshing the view
		 */
		virtual TreeItemRef createItem(const std::string& text, const TreeItemId parentId = invalidTreeItemId, const TreeItemId itemId = invalidTreeItemId, const bool refreshView = true);

		/**
		 * Creates a new item for this tree view and registeres it.
		 * @param texts The texts of the item to be created, one text for each column
		 * @param parentId Optional id of the parent item, and invalid id so that the item will not directly show up in the tree view
		 * @param itemId Optional the unique id of the item, any unique id which does not exist already, or and invalid id to automatically create an unique id, while the root item has item id -1
		 * @param refreshView True, to refresh the view afterwards; False, to skip refreshing the view
		 */
		virtual TreeItemRef createItem(const Strings& texts, const TreeItemId parentId = invalidTreeItemId, const TreeItemId itemId = invalidTreeItemId, const bool refreshView = true);

		/**
		 * Creates a new item for this tree view and registeres it.
		 * @param texts The texts of the item to be created, one text for each column, will be moved
		 * @param parentId Optional id of the parent item, and invalid id so that the item will not directly show up in the tree view
		 * @param itemId Optional the unique id of the item, any unique id which does not exist already, or and invalid id to automatically create an unique id, while the root item has item id -1
		 * @param refreshView True, to refresh the view afterwards; False, to skip refreshing the view
		 */
		virtual TreeItemRef createItem(Strings&& texts, const TreeItemId parentId = invalidTreeItemId, const TreeItemId itemId = invalidTreeItemId, const bool refreshView = true);

		/**
		 * Removes an item from this tree view.
		 * @param itemId The id of the item to remove
		 * @return True, if succeeded
		 */
		virtual bool removeItem(const TreeItemId itemId);

		/**
		 * Removes all items from this tree view.
		 */
		virtual void removeItems();

		/**
		 * Selects an item of this tree view.
		 * @param itemId The id of the item to select
		 * @param extendSelection True, to leave currently selected item unchanged; False, to select the defined item only
		 * @return True, if the item could be selected
		 */
		virtual bool selectItem(const TreeItemId itemId, const bool extendSelection = false);

		/**
		 * Selects items of this tree view.
		 * @param itemIds The ids of the item to select
		 * @param extendSelection True, to leave currently selected item unchanged; False, to select the defined items only
		 * @return True, if all items could be selected; False, if some items could not be selected
		 */
		virtual bool selectItems(const TreeItemIds& itemIds, const bool extendSelection = false);

		/**
		 * Unselects all items.
		 */
		virtual void unselectItems();

		/**
		 * Returns the id of the currently (first) selected item.
		 * @return The id of the first item which is currently selected, an invalid id if no item is selected
		 */
		TreeItemId selectedItemId() const;

		/**
		 * Returns the ids of all items that are currently selected.
		 * @return Ids of all selected items
		 */
		TreeItemIds selectedItemIds() const;

		/**
		 * Sets the image list for this tree view object (by moving the object).
		 * @param imageList The image list object to be moved
		 */
		void setImageList(ImageList&& imageList);

		/**
		 * Returns the image list object of this tree view.
		 * @return The image list object
		 */
		inline ImageList& imageList();

		/**
		 * Sets the width of a column.
		 * @param columnIndex The index of the column to be set, with range [0, columns())
		 * @param width The width of the column, with range [0, infinity)
		 */
		void setColumnWidth(const unsigned int columnIndex, const double width);

		/**
		 * Fits the width of the all columns or of one specific column so that the width of the column(s) match with the content.
		 * @param columnIndex The index of the column to be adjusted, -1 to adjust all columns
		 */
		void fitColumnSize(const unsigned int columnIndex = (unsigned int)(-1));

		/**
		 * Returns the MacOS specific object of this tree view.
		 * @return The view object
		 */
		inline NSView* nsView();

		/**
		 * Returns whether this object is enabled.
		 * @return True, if so
		 */
		inline bool isEnabled() const;

		/**
		 * Returns whether this object is valid and wrapps a valid MacOS object.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Forces the panel to be repainted.
		 */
		void repaint();

		/**
		 * View resize event.
		 * @param x The horizontal start location of this view, with range (-infinity, infinity)
		 * @param y The vertical start location of this view, with range (-infinity, infinity)
		 * @param width The width of this view, with range [0, infinity)
		 * @param height The height of this view, with range [0, infinity)
		 */
		virtual void onResize(const double x, const double y, const double width, const double height);

		/**
		 * The item selected event function.
		 * @param itemId The id of the item which has been selected, an invalid id if an item has been unselected
		 */
		virtual void onItemSelected(const TreeItemId itemId);

		/**
		 * The function for left mouse button down events.
		 */
		virtual void onMouseDownLeft();

		/**
		 * The function for left mouse button up events.
		 */
		virtual void onMouseUpLeft();

		/**
		 * The function for right mouse button down events.
		 */
		virtual void onMouseDownRight();

		/**
		 * The function for right mouse button up events.
		 */
		virtual void onMouseUpRight();

		/**
		 * The function for header click events.
		 * @param columnIndex The index of the header column that has been clicked
		 */
		virtual void onMouseDownInHeader(const unsigned int columnIndex);

		/**
		 * The function for background draw events.
		 */
		virtual void onDrawBackground();

		/**
		 * The function for post-foreground draw events.
		 */
		virtual void onDrawForeground();

		/**
		 * Translates an unsorted item (a tree view entry) to a sorted item.
		 * @param unsortedItem The unsorted item to be translated
		 * @return The sorted item
		 */
		TreeItemRef unsortedItem2sortedItem(id unsortedItem) const;

		/**
		 * Move operator.
		 * @param treeView Tree view object to be moved
		 * @return Reference to this object
		 */
		TreeView& operator=(TreeView&& treeView) noexcept;

		/**
		 * Disabled copy operator.
		 * @param treeView Tree view object to be copied
		 * @return Reference to this object
		 */
		TreeView& operator=(const TreeView& treeView) = delete;

		/**
		 * Returns whether this object is valid and wraps a valid MacOS object.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

	protected:

		/**
		 * Creates (mainly registers) an item for this tree view.
		 * @param itemObject The already existing but not registered tree item which must not be released by the caller, must be valid
		 * @param parentId Optional id of the parent item, and invalid id so that the item will not directly show up in the tree view
		 * @param refreshView True, to refresh the view afterwards; False, to skip refreshing the view
		 */
		virtual TreeItemRef createItem(TreeItem* itemObject, const TreeItemId parentId = invalidTreeItemId, const bool refreshView = true);

		/**
		 * Returns a unique tree item id for the current state of the tree view.
		 * @return The unique item id
		 */
		TreeItemId uniqueItemId();

		/**
		 * Refreshes (repaints) a specified item.
		 * @param itemId The id of the item which has to be refreshed (repainted).
		 */
		void refreshItem(const TreeItemId itemId);

		/**
		 * Refreshes (repaints) all items.
		 */
		void refreshItems();

		/**
		 * Sets the sorting map for all items.
		 * @param unsorted2SortedItemMap The map mapping unsorted item ids to sorted item ids, will be moved
		 */
		void setItemSortMap(TreeItemResortingMap&& unsorted2SortedItemMap);

		/**
		 * Translates an unsorted item id to a sorted item id.
		 * @param unsortedItemId The id of the unsorted item to be translated
		 * @return The sorted item id
		 */
		inline TreeItemId unsortedItemId2sortedItemId(const TreeItemId unsortedItemId) const;

		/**
		 * Translates a sorted item id to an unsorted item id.
		 * @param sortedItemId The id of the sorted item to be translated
		 * @return The unsorted item id
		 */
		inline TreeItemId sortedItemId2unsortedItemId(const TreeItemId sortedItemId) const;

	protected:

		/// The MacOS specific object providing scroll functionality.
		NSScrollView* scrollView_ = nullptr;

		/// The MacOS specific object providing the tree functionality.
		NSOutlineView* outlineView_ = nullptr;

		/// The MacOS specific data source for the outline view.
		NSObject<NSOutlineViewDataSource>* outlineViewDataSource_ = nullptr;

		/// The invisible root item of this tree view.
		TreeItemRef rootItem_;

		/// The map mapping tree item ids to tree items.
		TreeItemMap treeItemMap_;

		/// The counter for unique tree item ids.
		TreeItemId treeItemIdCounter_ = TreeItemId(0);

		/// The image list of this tree view.
		ImageList viewImageList_;

		/// The names of the columns of this tree.
		Strings columnNames_;

	private:

		/// Optional map mapping unsorted item ids to sorted item ids.
		TreeItemResortingMap unsorted2SortedItemMap_;

		/// Optional map mapping sorted item ids to unsorted item ids.
		TreeItemResortingMap sorted2UnsortedItemMap_;
};

inline TreeView::TreeItemId TreeView::TreeItem::id() const
{
	return id_;
}

inline const TreeView::Strings& TreeView::TreeItem::texts() const
{
	return texts_;
}

inline TreeView::TreeItemId TreeView::TreeItem::parentId() const
{
	return parentId_;
}

inline const TreeView::TreeItemIds& TreeView::TreeItem::children() const
{
	return childItemIds_;
}

inline unsigned int TreeView::TreeItem::iconIndex() const
{
	return iconIndex_;
}

inline size_t TreeView::columns() const
{
	return columnNames_.size();
}

inline void TreeView::enableMultiRowSelection(const bool enable)
{
	[outlineView_ setAllowsMultipleSelection:enable];
}

inline TreeView::TreeItemId TreeView::rootItemId() const
{
	return TreeItemId(-1);
}

inline const TreeView::TreeItem& TreeView::rootItem() const
{
	ocean_assert(rootItem_);
	return *rootItem_;
}

inline ImageList& TreeView::imageList()
{
	return viewImageList_;
}

inline NSView* TreeView::nsView()
{
	ocean_assert(scrollView_);
	return scrollView_;
}

inline bool TreeView::isEnabled() const
{
	return [outlineView_ isEnabled];
}

inline bool TreeView::isValid() const
{
	ocean_assert((scrollView_ != nullptr && outlineView_ != nullptr && outlineViewDataSource_ != nullptr)
			|| (scrollView_ == nullptr && outlineView_ == nullptr && outlineViewDataSource_ == nullptr));

	return scrollView_ != nullptr;
}

inline TreeView::operator bool() const
{
	return isValid();
}

inline TreeView::TreeItemId TreeView::unsortedItemId2sortedItemId(const TreeItemId unsortedItemId) const
{
	ocean_assert(unsorted2SortedItemMap_.size() == sorted2UnsortedItemMap_.size());

	if (unsorted2SortedItemMap_.empty())
	{
		return unsortedItemId;
	}
	else
	{
		const TreeItemResortingMap::const_iterator i = unsorted2SortedItemMap_.find(unsortedItemId);
		ocean_assert(i != unsorted2SortedItemMap_.end());

		return i->second;
	}
}

inline TreeView::TreeItemId TreeView::sortedItemId2unsortedItemId(const TreeItemId sortedItemId) const
{
	ocean_assert(unsorted2SortedItemMap_.size() == sorted2UnsortedItemMap_.size());

	if (sorted2UnsortedItemMap_.empty())
	{
		return sortedItemId;
	}
	else
	{
		const TreeItemResortingMap::const_iterator i = sorted2UnsortedItemMap_.find(sortedItemId);
		ocean_assert(i != sorted2UnsortedItemMap_.end());

		return i->second;
	}
}

}

}

}

}

#endif // META_OCEAN_PLATFORM_APPLE_MACOS_TREE_VIEW_H
