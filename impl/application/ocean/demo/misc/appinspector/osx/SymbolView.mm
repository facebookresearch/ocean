/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/misc/appinspector/osx/SymbolView.h"

#include "ocean/base/StringApple.h"

#include "ocean/platform/apple/Utilities.h"

SymbolView::SymbolView() :
	ItemView()
{
	// nothing to do here
}

SymbolView::SymbolView(const NSRect& rect, const BinaryAnalyzerRef& binaryAnalyzer, const Strings& columnNames) :
	ItemView(rect, binaryAnalyzer, columnNames)
{
	// nothing to do here
}

SymbolView::SymbolView(SymbolView&& view)
{
	*this = std::move(view);
}

SymbolView::~SymbolView()
{
	// nothing to do here
}

SymbolView& SymbolView::operator=(SymbolView&& view)
{
	if (this != &view)
	{
		ItemView::operator=(std::move(view));
	}

	return *this;
}

int SymbolView::showContextMenu(Platform::Apple::MacOS::ContextMenu& contextMenu)
{
	ocean_assert(binaryAnalyzer_);

	if (!binaryAnalyzer_ || binaryAnalyzer_->state() != BinaryAnalyzer::AS_SUCCEEDED)
	{
		return ItemView::showContextMenu(contextMenu);
	}

	const TreeItemIds treeItems = selectedItemIds();

	contextMenu.addItem("-");
	const int menuEntryCopyReadableName = contextMenu.addItem("Copy readable name", treeItems.size() == 1);
	const int menuEntryCopyMangledName = contextMenu.addItem("Copy mangled name", treeItems.size() == 1);
	contextMenu.addItem("-");
	const int menuEntryShowAliasNames = contextMenu.addItem("Show alias names", treeItems.size() == 1);

	const int userSelection = contextMenu.popup();

	std::string nameToCopy;

	if (userSelection == menuEntryCopyReadableName)
	{
		ocean_assert(!treeItems.empty());

		const BinaryAnalyzer::SymbolId symbolId = BinaryAnalyzer::SymbolId(objectId(treeItems.front()));
		nameToCopy = binaryAnalyzer_->symbol(symbolId).readableName();
	}
	else if (userSelection == menuEntryCopyMangledName)
	{
		ocean_assert(!treeItems.empty());

		const BinaryAnalyzer::SymbolId symbolId = BinaryAnalyzer::SymbolId(objectId(treeItems.front()));
		nameToCopy = binaryAnalyzer_->symbol(symbolId).name();
	}
	else if (userSelection == menuEntryShowAliasNames)
	{
		const BinaryAnalyzer::SymbolId id = objectId(treeItems.front());

		ocean_assert(id != BinaryAnalyzer::invalidSymbolId);
		if (id != BinaryAnalyzer::invalidSymbolId)
		{
			const BinaryAnalyzer::Symbol& symbol = binaryAnalyzer_->symbol(id);

			if (symbol.readableAliasNames().empty())
			{
				Platform::Apple::Utilities::showMessageBox("Alias names", "The symbol does not have any alias names");
			}
			else
			{
				std::string message;

				for (const std::string& readableAliasName : symbol.readableAliasNames())
				{
					message += readableAliasName + "\n";
				}

				Platform::Apple::Utilities::showMessageBox("Alias names", message);
			}
		}
	}

	if (!nameToCopy.empty())
	{
		NSPasteboard* pasteBoard = [NSPasteboard generalPasteboard];

		[pasteBoard declareTypes:[NSArray arrayWithObject:NSStringPboardType] owner:nil];
		[pasteBoard setString:StringApple::toNSString(nameToCopy) forType:NSStringPboardType];
	}

	return userSelection;
}
