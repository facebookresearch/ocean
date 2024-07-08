/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/apple/macos/TreeView.h"

#include "ocean/base/StringApple.h"

#include "ocean/platform/apple/System.h"

using namespace Ocean::Platform::Apple::MacOS;

/**
 * Definition of our own outline view class.
 */
@interface NSOceanPlatformAppleMacOSOutlineView : NSOutlineView<NSOutlineViewDelegate>
{
	/// The c++ owner of this outline view.
	TreeView* viewOwner;

	/// The text color this outline view.
	NSColor* viewTextColor;
}

/**
 * Initializes the outline view by a given rectangle and corresponding c++ class instance.
 * @param frameRect The rect of the outline view
 * @param owner The c++ owner of the outline view
 * @param columnNames The names of the columns the outline view will have, at least one
 * @return The instance of the new outline view
 */
-(id)initWithFrame:(NSRect)frameRect andOwner:(TreeView*)owner andNamesOfColumns:(const std::vector<std::string>&)columnNames;

/**
 * Sets or changes the owner of this outline view object.
 * @param owner The owner to set
 */
-(void)setOwner:(TreeView*)owner;

@end


@interface NSOceanPlatformAppleMacOSOutlineViewImageAndTextCell : NSTextFieldCell
{
	/// The image of this cell.
	NSImage* cellImage;

	/// The scale factor of the cell's image, with range (0, infinity).
	double cellImageScaleFactor;
}

@end

@implementation NSOceanPlatformAppleMacOSOutlineViewImageAndTextCell

-(id)initTextCell:(NSString *)aString
{
	cellImage = nullptr;
	cellImageScaleFactor = 1.0;

	if (self = [super initTextCell:aString])
	{
		// nothing to do here
	}

	return self;
}

-(void)setImage:(NSImage *)image withScaleFactor:(double)scaleFactor
{
	cellImage = [image copy];

	ocean_assert(scaleFactor > 0.0);
	cellImageScaleFactor = scaleFactor;
}

-(void)drawWithFrame:(NSRect)cellFrame inView:(NSView *)controlView
{
	if (cellImage == nil)
		[super drawWithFrame:cellFrame inView:controlView];
	else
	{
		NSSize imageSize = [cellImage size];
		const double factor = (cellImageScaleFactor != 0.0) ? (1.0 / cellImageScaleFactor) : 1.0;

		imageSize.width *= factor;
		imageSize.height *= factor;

		NSRect imageFrame, textFrame;
		NSDivideRect(cellFrame, &imageFrame, &textFrame, imageSize.width + 5, NSMinXEdge);

		imageFrame.origin.x += 5;
		imageFrame.size = imageSize;

		if (textFrame.size.width >= 2)
		{
			textFrame.origin.x += 2;
			textFrame.size.width -= 2;
		}

		[cellImage drawInRect:imageFrame fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0 respectFlipped:NO hints:nil];
		[super drawWithFrame:textFrame inView:controlView];
	}
}

-(NSColor*)highlightColorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView
{
	return [NSColor selectedTextBackgroundColor];
}

@end

@implementation NSOceanPlatformAppleMacOSOutlineView

-(id)initWithFrame:(NSRect)frameRect andOwner:(TreeView*)owner andNamesOfColumns:(const std::vector<std::string>&)columnNames
{
	ocean_assert(owner && columnNames.size() >= 1);
	viewOwner = nullptr;

	viewTextColor = [NSColor textColor];

	if (self = [super initWithFrame:frameRect])
	{
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(selectionChanged:) name:NSOutlineViewSelectionDidChangeNotification object:self];

		[self setDelegate:self];

		for (size_t n = 0; n < columnNames.size(); ++n)
		{
			NSTableColumn* column = [[NSTableColumn alloc] initWithIdentifier:Ocean::StringApple::toNSString(std::string("Column_") + Ocean::String::toAString(n))];
			[column setEditable:NO];
			[column setTitle:Ocean::StringApple::toNSString(columnNames[n])];

			[column setMinWidth:50];
			[column setWidth:100];
			[column setDataCell:[[NSOceanPlatformAppleMacOSOutlineViewImageAndTextCell alloc] init]];
			[column setResizingMask:(NSTableColumnAutoresizingMask | NSTableColumnUserResizingMask)];

			[self addTableColumn:column];

			if (n == 0u)
				[self setOutlineTableColumn:column];
		}

		[self setColumnAutoresizingStyle:NSTableViewSequentialColumnAutoresizingStyle];

		viewOwner = owner;
	}

	return self;
}

-(void)drawBackgroundInClipRect:(NSRect)clipRect
{
	[super drawBackgroundInClipRect:clipRect];

	ocean_assert(viewOwner);
	viewOwner->onDrawBackground();
}

-(void)drawRect:(NSRect)dirtyRect
{
	[super drawRect:dirtyRect];

	ocean_assert(viewOwner);
	viewOwner->onDrawForeground();
}

-(void)setOwner:(TreeView*)owner
{
	ocean_assert(viewOwner != owner && "Should be set already during initialization!");
	viewOwner = owner;
}

-(void)mouseDown:(NSEvent *)theEvent
{
	[super mouseDown:theEvent];

	if ([super isEnabled])
	{
		ocean_assert(viewOwner);
		viewOwner->onMouseDownLeft();
	}
}

-(void)mouseUp:(NSEvent *)theEvent
{
	[super mouseUp:theEvent];

	if ([super isEnabled])
	{
		ocean_assert(viewOwner);
		viewOwner->onMouseUpLeft();
	}
}

-(void)rightMouseDown:(NSEvent *)theEvent
{
	[super rightMouseDown:theEvent];

	if ([super isEnabled])
	{
		ocean_assert(viewOwner);
		viewOwner->onMouseDownRight();
	}
}

- (void)rightMouseUp:(NSEvent *)theEvent
{
	[super rightMouseUp:theEvent];

	if ([super isEnabled])
	{
		ocean_assert(viewOwner);
		viewOwner->onMouseUpRight();
	}
}

-(void)outlineView:(NSOutlineView *)outlineView mouseDownInHeaderOfTableColumn:(NSTableColumn *)tableColumn
{
	if (viewOwner && [super isEnabled])
	{
		const std::string identifier = Ocean::StringApple::toUTF8([tableColumn identifier]);
		ocean_assert(identifier.find("Column_") == 0);

		int value = -1;
		if (Ocean::String::isInteger32(identifier.substr(7), &value) && value >= 0)
		{
			viewOwner->onMouseDownInHeader((unsigned int)value);
		}
	}
}

-(void)selectionChanged:(NSNotification*)notification
{
	ocean_assert(viewOwner);

	id selectedItem = [self itemAtRow:[self selectedRow]];

	const TreeView::TreeItemId itemId = selectedItem ? ((NSNumber*)selectedItem).integerValue : TreeView::invalidTreeItemId;

	viewOwner->onItemSelected(itemId);
}

-(void)highlightSelectionInClipRect:(NSRect)clipRect
{
	NSRange visibleRowIndexes = [self rowsInRect:clipRect];
	NSIndexSet* selectedRowIndexes = [self selectedRowIndexes];

	for (size_t n = visibleRowIndexes.location; n < visibleRowIndexes.location + visibleRowIndexes.length; n++)
	{
		if ([selectedRowIndexes containsIndex:n])
		{
			NSBezierPath* path = [NSBezierPath bezierPathWithRect:[self rectOfRow:n]];

			[[NSColor selectedTextBackgroundColor] set];
			[path fill];
		}
	}
}

-(void)outlineView:(NSOutlineView *)outlineView willDisplayCell:(id)cell forTableColumn:(NSTableColumn *)tableColumn item:(id)item
{
	const TreeView::TreeItemRef treeItem = viewOwner->unsortedItem2sortedItem(item);
	ocean_assert(treeItem);

	if (treeItem && treeItem->iconIndex())
	{
		ImageList& imageList = viewOwner->imageList();

		if (treeItem->iconIndex() < imageList.size())
		{
			Image& image = imageList[treeItem->iconIndex()];

			if (image.isValid())
			{
				[cell setImage:image.nsImage() withScaleFactor:image.scaleFactor()];
			}
		}
	}

	ocean_assert(viewTextColor);
	[cell setTextColor:viewTextColor];
	[cell setBackgroundStyle:NSBackgroundStyleLight];
}

@end


/**
 * Definition of our own scroll view class.
 */
@interface NSOceanPlatformMacOSScrollView : NSScrollView
{
	/// The c++ owner of the scroll view
	TreeView* viewOwner;
}

/**
 * Initializes the scroll view by a given rectangle and corresponding c++ class instance.
 * @param frameRect The rect of the scroll view
 * @param owner The c++ owner of the scroll view
 * @return The instance of the new scroll view
 */
-(id)initWithFrame:(NSRect)frameRect andOwner:(TreeView*)owner;

/**
 * Sets or changes the owner of this scroll view object.
 * @param owner The owner to set
 */
-(void)setOwner:(TreeView*)owner;

/**
 * Destructs the object.
 */
-(void)dealloc;

@end

@implementation NSOceanPlatformMacOSScrollView

-(id)initWithFrame:(NSRect)frameRect andOwner:(TreeView*)owner
{
	ocean_assert(owner);
	viewOwner = nullptr;

	if (self = [super initWithFrame:frameRect])
	{
		viewOwner = owner;
	}

	return self;
}

-(void)setOwner:(TreeView*)owner
{
	ocean_assert(viewOwner != owner && "Should be set already during initialization!");
	viewOwner = owner;
}

-(void)setFrame:(NSRect)frame
{
	[super setFrame:frame];

	if (viewOwner)
		viewOwner->onResize(frame.origin.x, frame.origin.y, frame.size.width, frame.size.height);
}

-(void)dealloc
{
	ocean_assert(viewOwner);
	viewOwner = nullptr;
}

@end

/**
 * Definition of the data source protocoll for the tree view.
 */
@interface NSOceanPlatformMacOSTreeViewDataSource : NSObject<NSOutlineViewDataSource>
{
	/// The c++ owner of the tree view.
	TreeView* sourceOwner;
}

/**
 * Initializes the data source with a given corresponding c++ class instance.
 * @param owner The c++ owner of the data source
 * @return The instance of the new scroll view
 */
-(id)init:(TreeView*)owner;

/**
 * Sets or changes the owner of this source protocoll object.
 * @param owner The owner to set
 */
-(void)setOwner:(TreeView*)owner;

@end

@implementation NSOceanPlatformMacOSTreeViewDataSource

-(id)init:(TreeView*)owner
{
	ocean_assert(owner);
	sourceOwner = nullptr;

	if (self = [super init])
	{
		sourceOwner = owner;
	}

	return self;
}

-(void)setOwner:(TreeView*)owner
{
	ocean_assert(sourceOwner != owner && "Should be set already during initialization!");
	sourceOwner = owner;
}

-(NSInteger)outlineView:(NSOutlineView *)outlineView numberOfChildrenOfItem:(id)item
{
	ocean_assert(sourceOwner);

	if (item == nullptr)
	{
		return NSInteger(sourceOwner->rootItem().children().size());
	}

	const TreeView::TreeItemRef treeItem = sourceOwner->unsortedItem2sortedItem(item);

	ocean_assert(treeItem);
	if (!treeItem)
	{
		return 0;
	}

	return NSInteger(treeItem->children().size());
}

-(BOOL)outlineView:(NSOutlineView *)outlineView isItemExpandable:(id)item
{
	ocean_assert(sourceOwner);

	if (item == nullptr)
	{
		return !sourceOwner->rootItem().children().empty();
	}

	const TreeView::TreeItemRef treeItem = sourceOwner->unsortedItem2sortedItem(item);

	ocean_assert(treeItem);
	if (!treeItem)
	{
		return false;
	}

	return !treeItem->children().empty();
}

-(id)outlineView:(NSOutlineView *)outlineView child:(NSInteger)index ofItem:(id)item
{
	if (sourceOwner == nullptr)
	{
		return [[NSNumber alloc] initWithInteger:TreeView::invalidTreeItemId];
	}

	if (item == nullptr)
	{
		ocean_assert(index < sourceOwner->rootItem().children().size());
		if (index < sourceOwner->rootItem().children().size())
		{
			return [[NSNumber alloc] initWithInteger:sourceOwner->rootItem().children()[index]];
		}
	}

	const TreeView::TreeItemId itemId = ((NSNumber*)item).integerValue;
	const TreeView::TreeItemRef treeItem = sourceOwner->item(itemId);

	ocean_assert(treeItem && index < treeItem->children().size());
	if (!treeItem || index >= treeItem->children().size())
	{
		return [[NSNumber alloc] initWithInteger:TreeView::invalidTreeItemId];
	}

	return [[NSNumber alloc] initWithInteger:treeItem->children()[index]];
}

-(id)outlineView:(NSOutlineView *)outlineView objectValueForTableColumn:(NSTableColumn *)tableColumn byItem:(id)item
{
	const TreeView::TreeItemRef treeItem = sourceOwner->unsortedItem2sortedItem(item);

	ocean_assert(tableColumn != nullptr);
	NSTableColumn* _tableColumn = tableColumn;
	NSUInteger columnIndex = [[outlineView tableColumns] indexOfObject:_tableColumn];

	ocean_assert(treeItem);
	if (!treeItem)
	{
		return @"Invalid item!";
	}

	if ((unsigned int)columnIndex >= treeItem->texts().size())
	{
		return @"";
	}

	return Ocean::StringApple::toNSString(treeItem->texts()[(unsigned int)columnIndex]);
}

@end

namespace Ocean
{

namespace Platform
{

namespace Apple
{

namespace MacOS
{

TreeView::TreeItem::TreeItem(TreeView& owner, const Strings& texts, const TreeItemId id, const unsigned int iconIndex) :
	owner_(owner),
	parentId_(invalidTreeItemId),
	id_(id),
	texts_(texts),
	iconIndex_(iconIndex)
{
	if (id_ == invalidTreeItemId)
	{
		id_ = owner_.uniqueItemId();
	}

	ocean_assert(id_ != invalidTreeItemId);
	ocean_assert(!owner_.hasItem(id_));
}

TreeView::TreeItem::TreeItem(TreeView& owner, Strings&& texts, const TreeItemId id, const unsigned int iconIndex) :
	owner_(owner),
	parentId_(invalidTreeItemId),
	id_(id),
	texts_(std::move(texts)),
	iconIndex_(iconIndex)
{
	if (id_ == invalidTreeItemId)
	{
		id_ = owner_.uniqueItemId();
	}

	ocean_assert(id_ != invalidTreeItemId);
	ocean_assert(!owner_.hasItem(id_));
}

TreeView::TreeItem::~TreeItem()
{
	// nothing to do here
}

TreeView::TreeItemIds TreeView::TreeItem::recursiveChildren() const
{
	TreeItemIds result(childItemIds_);

	size_t index = 0;

	while (index < result.size())
	{
		const TreeItemRef item = owner_.item(result[index]);
		ocean_assert(item);

		if (item)
		{
			result.insert(result.end(), item->children().begin(), item->children().end());
		}

		index++;
	}

	return result;
}

bool TreeView::TreeItem::hasChild(const TreeItemId childId) const
{
	if (childId == invalidTreeItemId)
	{
		return false;
	}

	for (TreeItemIds::const_iterator i = childItemIds_.begin(); i != childItemIds_.end(); ++i)
	{
		if (*i == childId)
		{
			return true;
		}
	}

	return false;
}

bool TreeView::TreeItem::removeChild(const TreeItemId childId)
{
	if (childId == invalidTreeItemId)
	{
		return true;
	}

	TreeItemRef childItem(owner_.item(childId));

	ocean_assert(childItem);
	if (!childItem)
	{
		return false;
	}

	for (TreeItemIds::iterator i = childItemIds_.begin(); i != childItemIds_.end(); ++i)
	{
		if (*i == childId)
		{
			childItemIds_.erase(i);
			childItem->parentId_ = invalidTreeItemId;

			owner_.refreshItem(id_);

			return true;
		}
	}

	ocean_assert(false && "Invalid item id!");
	return false;
}

void TreeView::TreeItem::setTexts(const Strings& texts)
{
	if (texts_ == texts)
	{
		return;
	}

	texts_ = texts;
	owner_.refreshItem(id_);
}

void TreeView::TreeItem::setIconIndex(const unsigned int index)
{
	if (iconIndex_ == index)
	{
		return;
	}

	iconIndex_ = index;
	owner_.refreshItem(id_);
}

bool TreeView::TreeItem::setParent(const TreeItemId parentId, const TreeItemId followingId, const bool refreshView)
{
	if (parentId_ == parentId)
	{
		return true;
	}

	const TreeItemId previousParentId = parentId;

	if (parentId_ != invalidTreeItemId)
	{
		const TreeItemRef previousParent = owner_.item(parentId_);

		ocean_assert(previousParent);
		if (previousParent)
		{
			bool found = false;

			for (TreeItemIds::iterator i = previousParent->childItemIds_.begin(); i != previousParent->childItemIds_.end(); ++i)
			{
				if (*i == id_)
				{
					previousParent->childItemIds_.erase(i);
					found = true;
				}
			}

			ocean_assert(found);
			parentId_ = invalidTreeItemId;
		}
	}

	if (parentId != invalidTreeItemId)
	{
		const TreeItemRef newParent = owner_.item(parentId);

		ocean_assert(newParent);
		if (newParent)
		{
			for (TreeItemIds::iterator i = newParent->childItemIds_.begin(); i != newParent->childItemIds_.end(); ++i)
			{
				ocean_assert(*i != id_);
			}

			bool hasBeenInserted = false;

			if (followingId != invalidTreeItemId)
			{
				for (TreeItemIds::iterator i = newParent->childItemIds_.begin(); i != newParent->childItemIds_.end(); ++i)
				{
					if (*i == followingId)
					{
						newParent->childItemIds_.insert(i, id_);
						hasBeenInserted = true;
						break;
					}
				}

				ocean_assert(hasBeenInserted && "The specified followingId is not a child item of the new parent item");
			}

			if (!hasBeenInserted)
			{
				newParent->childItemIds_.push_back(id_);
			}
		}

		parentId_ = parentId;
	}

        if (refreshView)
	{
		if (previousParentId != invalidTreeItemId)
		{
			owner_.refreshItem(previousParentId);
		}

		if (parentId_ != invalidTreeItemId)
		{
			owner_.refreshItem(parentId_);
		}
	}

        return true;
}

void TreeView::TreeItem::expand(const bool allChildItems) const
{
	if (this == owner_.rootItem_.get())
	{
		[owner_.outlineView_ expandItem:nil expandChildren:allChildItems];
	}
	else
	{
		TreeItemIds treeItemIds(1, id_);

		while (true)
		{
			const TreeItemRef treeItem = owner_.item(treeItemIds.back());
			ocean_assert(treeItem);

			NSNumber* nsItemId = [[NSNumber alloc] initWithInteger:id_];

			if ([owner_.outlineView_ isItemExpanded:nsItemId])
			{
				break;
			}

			const TreeItemId parentId = treeItem->parentId();
			if (parentId == invalidTreeItemId)
			{
				break;
			}

			treeItemIds.push_back(parentId);
		}

		for (TreeItemIds::const_reverse_iterator i = treeItemIds.rbegin(); i != treeItemIds.rend(); ++i)
		{
			NSNumber* item = [[NSNumber alloc] initWithInteger:*i];
			[owner_.outlineView_ expandItem:item expandChildren:allChildItems];
		}
	}
}

TreeView::TreeView()
{
	rootItem_ = TreeItemRef(new TreeItem(*this, Strings(1, "Invisible Root Item"), rootItemId()));
	treeItemMap_[rootItem_->id()] = rootItem_;
}

TreeView::TreeView(const NSRect& rect, const Strings& columnNames) :
	TreeView(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height, columnNames)
{
	// nothing to do here
}

TreeView::TreeView(const double left, const double top, const double width, const double height, const Strings& columnNames) :
	scrollView_(nullptr),
	outlineView_(nullptr),
	outlineViewDataSource_(nullptr),
	treeItemIdCounter_(TreeItemId(0)),
	columnNames_(columnNames)
{
	ocean_assert(!columnNames.empty());

	rootItem_ = TreeItemRef(new TreeItem(*this, Strings(1, "Invisible Root Item"), rootItemId()));
	treeItemMap_[rootItem_->id()] = rootItem_;

	scrollView_ = [[NSOceanPlatformMacOSScrollView alloc] initWithFrame:NSMakeRect(CGFloat(left), CGFloat(top), CGFloat(width), CGFloat(height)) andOwner:this];

	outlineView_ = [[NSOceanPlatformAppleMacOSOutlineView alloc] initWithFrame:[scrollView_ bounds] andOwner:this andNamesOfColumns:columnNames_];

	NSTableHeaderView* headerView = [[NSTableHeaderView alloc] init];
	[outlineView_ setHeaderView:headerView];

	[scrollView_ setHasVerticalScroller:YES];
	[scrollView_ setHasHorizontalScroller:YES];
	[scrollView_ setAutohidesScrollers:YES];

	[scrollView_ setScrollerStyle:NSScrollerStyleLegacy];

	[scrollView_ setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];

	[scrollView_ setDocumentView:outlineView_];

	outlineViewDataSource_ = [[NSOceanPlatformMacOSTreeViewDataSource alloc] init:this];
	[outlineView_ setDataSource:outlineViewDataSource_];

	[outlineView_ reloadData];
}

TreeView::TreeView(TreeView&& treeView) noexcept :
	scrollView_(nullptr),
	outlineView_(nullptr),
	outlineViewDataSource_(nullptr),
	treeItemIdCounter_(TreeItemId(0))
{
	*this = std::move(treeView);
}

TreeView::~TreeView()
{
	// we do not release the MacOS specific objects as this will be done by the OS itself
}

bool TreeView::hasItem(const TreeItemId itemId) const
{
	return treeItemMap_.find(itemId) != treeItemMap_.end();
}

TreeView::TreeItemRef TreeView::item(const TreeItemId itemId) const
{
	const TreeItemMap::const_iterator i = treeItemMap_.find(itemId);

	if (i == treeItemMap_.end())
	{
		return TreeItemRef();
	}

	return i->second;
}

TreeView::TreeItemRef TreeView::createItem(const std::string& text, const TreeItemId parentId, const TreeItemId itemId, const bool refreshView)
{
	return createItem(Strings(1, text), parentId, itemId, refreshView);
}

TreeView::TreeItemRef TreeView::createItem(const Strings& texts, const TreeItemId parentId, const TreeItemId itemId, const bool refreshView)
{
	TreeItemId id = itemId;
	if (id == invalidTreeItemId)
	{
		id = uniqueItemId();
	}

	return createItem(new TreeItem(*this, texts, id), parentId, refreshView);
}

TreeView::TreeItemRef TreeView::createItem(Strings&& texts, const TreeItemId parentId, const TreeItemId itemId, const bool refreshView)
{
	TreeItemId id = itemId;
	if (id == invalidTreeItemId)
	{
		id = uniqueItemId();
	}

	return createItem(new TreeItem(*this, std::move(texts), id), parentId, refreshView);
}

bool TreeView::removeItem(const TreeItemId itemId)
{
	if (itemId == invalidTreeItemId)
	{
		return true;
	}

	const TreeItemRef itemObject(item(itemId));

	if (!itemObject)
	{
		return false;
	}

	ocean_assert(itemObject->id() == itemId);

	const TreeItemRef parentItem(item(itemObject->parentId()));
	if (parentItem)
	{
		parentItem->removeChild(itemObject->id());
	}

	treeItemMap_.erase(itemId);

	fitColumnSize();

	return true;
}

void TreeView::removeItems()
{
	treeItemMap_.clear();
	rootItem_->childItemIds_.clear();

	treeItemMap_[rootItem_->id()] = rootItem_;

	[outlineView_ reloadData];
}

bool TreeView::selectItem(const TreeItemId itemId, const bool extendSelection)
{
	if (!isEnabled())
	{
		return false;
	}

	TreeItemId unsortedItemId = sortedItemId2unsortedItemId(itemId);

	NSInteger row = [outlineView_ rowForItem:[[NSNumber alloc] initWithInteger:unsortedItemId]];

	if (row == -1)
	{
		return false;
	}

	[outlineView_ selectRowIndexes: [NSIndexSet indexSetWithIndex:row] byExtendingSelection:extendSelection];

	return true;
}

bool TreeView::selectItems(const TreeItemIds& itemIds, const bool extendSelection)
{
	if (!isEnabled())
	{
		return false;
	}

        if (!extendSelection)
	{
		[outlineView_ deselectAll:outlineView_];
	}

        for (TreeItemIds::const_iterator i = itemIds.cbegin(); i != itemIds.cend(); ++i)
	{
		TreeItemId unsortedItemId = sortedItemId2unsortedItemId(*i);

                NSInteger row = [outlineView_ rowForItem:[[NSNumber alloc] initWithInteger:unsortedItemId]];

                if (row == -1)
		{
			return false;
		}

                [outlineView_ selectRowIndexes: [NSIndexSet indexSetWithIndex:row] byExtendingSelection:YES];
	}

        return true;
}

void TreeView::unselectItems()
{
	[outlineView_ deselectAll:nil];
}

TreeView::TreeItemId TreeView::selectedItemId() const
{
	id selectedItem = [outlineView_ itemAtRow:[outlineView_ selectedRow]];

	if (selectedItem == nullptr)
	{
		return invalidTreeItemId;
	}

	const TreeView::TreeItemId itemId = unsortedItemId2sortedItemId(((NSNumber*)selectedItem).integerValue);
	ocean_assert(hasItem(itemId));

	return itemId;
}

TreeView::TreeItemIds TreeView::selectedItemIds() const
{
	NSIndexSet* indices = [outlineView_ selectedRowIndexes];

        TreeItemIds itemIds;
	itemIds.reserve(indices.count);

        NSUInteger index = [indices firstIndex];

        while (index != NSNotFound)
	{
		id selectedItem = [outlineView_ itemAtRow:index];

                if (selectedItem != nullptr)
		{
			const TreeView::TreeItemId itemId = ((NSNumber*)selectedItem).integerValue;
			ocean_assert(hasItem(itemId));

                        itemIds.push_back(itemId);
		}

                index = [indices indexGreaterThanIndex:index];
	}

        return itemIds;
}

void TreeView::setImageList(ImageList&& imageList)
{
	viewImageList_ = std::move(imageList);
}

void TreeView::repaint()
{
	[outlineView_ setNeedsDisplayInRect:[outlineView_ bounds]];
}

void TreeView::onResize(const double x, const double y, const double width, const double height)
{
	// should be implemented in a derived class
}

void TreeView::onItemSelected(const TreeItemId itemId)
{
	// should be implemented in a derived class
}

void TreeView::onMouseDownLeft()
{
	// should be implemented in a derived class
}

void TreeView::onMouseUpLeft()
{
	// should be implemented in a derived class
}

void TreeView::onMouseDownRight()
{
	// should be implemented in a derived class
}

void TreeView::onMouseUpRight()
{
	// should be implemented in a derived class
}

void TreeView::onMouseDownInHeader(const unsigned int columnHeader)
{
	// should be implemented in a derived class
}

void TreeView::onDrawBackground()
{
	// should be implemented in a derived class
}

void TreeView::onDrawForeground()
{
	// should be implemented in a derived class
}

TreeView::TreeItemRef TreeView::unsortedItem2sortedItem(id unsortedItem) const
{
	const TreeView::TreeItemId unsortedItemId = ((NSNumber*)unsortedItem).integerValue;

	if (unsortedItemId == rootItemId() || unsorted2SortedItemMap_.empty())
	{
		return item(unsortedItemId);
	}
	else
	{
		const TreeItemResortingMap::const_iterator i = unsorted2SortedItemMap_.find(unsortedItemId);

		if (i == unsorted2SortedItemMap_.end())
		{
			ocean_assert(false && "The provided resorting map is invalid!");
			return TreeView::TreeItemRef();
		}

		return item(i->second);
	}
}

TreeView& TreeView::operator=(TreeView&& treeView) noexcept
{
	if (this != &treeView)
	{
		scrollView_ = treeView.scrollView_;
		if (scrollView_)
		{
			[(NSOceanPlatformMacOSScrollView*)scrollView_ setOwner:this];
		}
		treeView.scrollView_ = nullptr;

		outlineView_ = treeView.outlineView_;
		if (outlineView_)
		{
			[(NSOceanPlatformAppleMacOSOutlineView*)outlineView_ setOwner:this];
		}
		treeView.outlineView_ = nullptr;

		outlineViewDataSource_ = treeView.outlineViewDataSource_;
		if (outlineViewDataSource_)
		{
			[(NSOceanPlatformMacOSTreeViewDataSource*)outlineViewDataSource_ setOwner:this];
		}
		treeView.outlineViewDataSource_ = nullptr;

		rootItem_ = std::move(treeView.rootItem_);
		ocean_assert(!treeView.rootItem_);

		treeItemMap_ = std::move(treeView.treeItemMap_);

		treeItemIdCounter_ = treeView.treeItemIdCounter_;
		treeView.treeItemIdCounter_ = TreeItemId(0);

		viewImageList_ = std::move(treeView.viewImageList_);
		ocean_assert(treeView.viewImageList_.isEmpty());

		columnNames_ = std::move(treeView.columnNames_);

		unsorted2SortedItemMap_ = std::move(treeView.unsorted2SortedItemMap_);
		sorted2UnsortedItemMap_ = std::move(treeView.sorted2UnsortedItemMap_);
	}

	return *this;
}

TreeView::TreeItemRef TreeView::createItem(TreeItem* itemObject, const TreeItemId parentId, const bool refreshView)
{
	ocean_assert(itemObject);

	const TreeItemRef item(itemObject);

	ocean_assert(treeItemMap_.find(item->id()) == treeItemMap_.end());

	treeItemMap_[item->id()] = item;

	if (parentId != invalidTreeItemId)
	{
		item->setParent(parentId, invalidTreeItemId, refreshView);
	}

	return item;
}

TreeView::TreeItemId TreeView::uniqueItemId()
{
	while (treeItemMap_.find(treeItemIdCounter_) != treeItemMap_.end())
	{
		treeItemIdCounter_++;
	}

	ocean_assert(treeItemMap_.find(treeItemIdCounter_) == treeItemMap_.end());
	return treeItemIdCounter_++;
}

void TreeView::setColumnWidth(const unsigned int columnIndex, const double width)
{
	ocean_assert(columnIndex < columns());

	NSTableColumn* column = outlineView_.tableColumns[columnIndex];
	[column setWidth:width];
}

void TreeView::fitColumnSize(const unsigned int columnIndex)
{
	const unsigned int firstColumn = columnIndex < outlineView_.tableColumns.count ? columnIndex : 0u;
	const unsigned int endColumn = columnIndex < outlineView_.tableColumns.count ? columnIndex + 1u : (unsigned int)outlineView_.tableColumns.count;

	for (unsigned int c = firstColumn; c < endColumn; ++c)
	{
		NSTableColumn* column = outlineView_.tableColumns[c];

		CGFloat maxWidth = 0;

		for (NSInteger n = 0; n < outlineView_.numberOfRows; ++n)
		{
			NSCell* cell = [column dataCellForRow:n];
			NSSize size = [cell cellSizeForBounds:NSMakeRect(0, 0, INFINITY, INFINITY)];

			maxWidth = max(maxWidth, size.width);
		}

		// as the cellSizeForBounds function seems not to respect the tree-depth we explicitly add 150 to handle this issue
		[column setWidth:maxWidth + 30];
	}
}

void TreeView::refreshItem(const TreeItemId itemId)
{
	// **TODO** do it more restricted
	refreshItems();
}

void TreeView::refreshItems()
{
	[outlineView_ reloadData];
}

void TreeView::setItemSortMap(TreeItemResortingMap&& unsorted2SortedItemMap)
{
	unsorted2SortedItemMap_ = std::move(unsorted2SortedItemMap);

	sorted2UnsortedItemMap_.clear();

	for (TreeItemResortingMap::const_iterator i = unsorted2SortedItemMap_.cbegin(); i != unsorted2SortedItemMap_.cend(); ++i)
		sorted2UnsortedItemMap_.insert(std::make_pair(i->second, i->first));

	ocean_assert(unsorted2SortedItemMap_.size() == sorted2UnsortedItemMap_.size());
}

}
}

}

}
