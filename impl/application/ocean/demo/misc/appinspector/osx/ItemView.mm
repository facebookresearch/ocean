/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/misc/appinspector/osx/ItemView.h"

#include "ocean/base/StringApple.h"

#include "ocean/platform/apple/macos/ContextMenu.h"

ItemView::ItemView() :
	Platform::Apple::MacOS::TreeView()
{
	// nothing to do here
}

ItemView::ItemView(const NSRect& rect, const BinaryAnalyzerRef& binaryAnalyzer, const Strings& columnNames) :
	Platform::Apple::MacOS::TreeView(rect, columnNames),
	binaryAnalyzer_(binaryAnalyzer)
{
	enableMultiRowSelection(true);
}

ItemView::ItemView(ItemView&& itemView) :
	Platform::Apple::MacOS::TreeView(std::move(itemView)),
	binaryAnalyzer_(std::move(itemView.binaryAnalyzer_))
{
	// nothing to do here
}

ItemView::~ItemView()
{
	// nothing to do here
}

ItemView::ObjectIdSet ItemView::selectedObjectIds() const
{
	const TreeItemIds treeItemIds = selectedItemIds();

	ObjectIdSet objectIds;

	for (const TreeItemId& treeItemId : treeItemIds)
	{
		const size_t id = objectId(treeItemId);

		if (id != invalidObjectId)
		{
			objectIds.insert(id);
		}
	}

	return objectIds;
}

ItemView& ItemView::operator=(ItemView&& itemView)
{
	if (this != &itemView)
	{
		Platform::Apple::MacOS::TreeView::operator=(std::move(itemView));

		binaryAnalyzer_ = std::move(itemView.binaryAnalyzer_);
		treeItemId2ObjectIdMap_ = std::move(itemView.treeItemId2ObjectIdMap_);
	}

	return *this;
}

void ItemView::onMouseUpRight()
{
	const TreeItemIds ids = selectedItemIds();

	Platform::Apple::MacOS::ContextMenu contextMenu;

	const int copyEntriesIndex = contextMenu.addItem("Copy entries", !ids.empty());
	const int selectAllIndex = contextMenu.addItem("Select all entries");

	const int index = showContextMenu(contextMenu);

	if (index == copyEntriesIndex)
	{
		ocean_assert(!ids.empty());

		NSPasteboard* pasteBoard = [NSPasteboard generalPasteboard];

		[pasteBoard declareTypes:[NSArray arrayWithObject:NSStringPboardType] owner:nil];

		std::string text;

		for (TreeItemIds::const_iterator i = ids.cbegin(); i != ids.cend(); ++i)
		{
			const TreeItemRef itemObject = item(*i);

			ocean_assert(itemObject);
			if (!itemObject)
			{
				continue;
			}

			const Strings& texts = itemObject->texts();

			if (texts.size() == 0)
			{
				continue;
			}

			if (texts.size() == 1)
			{
				[pasteBoard setString:StringApple::toNSString(texts.front()) forType:NSStringPboardType];
			}
			else
			{
				ocean_assert(texts.size() >= 2);

				if (!text.empty())
				{
					text.append("\n");
				}

				text.append(texts.front());

				for (size_t n = 1; n < texts.size(); ++n)
				{
					text.append("\t");
					text.append(texts[n]);
				}
			}
		}

		[pasteBoard setString:StringApple::toNSString(text) forType:NSStringPboardType];
	}
	else if (index == selectAllIndex)
	{
		[outlineView_ selectAll:outlineView_];
	}
}

int ItemView::showContextMenu(Platform::Apple::MacOS::ContextMenu& contextMenu)
{
	return contextMenu.popup();
}
