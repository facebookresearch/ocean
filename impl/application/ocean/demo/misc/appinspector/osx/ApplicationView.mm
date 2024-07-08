/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/misc/appinspector/osx/ApplicationView.h"

#include "ocean/io/Scanner.h"

ApplicationView::ApplicationView() :
	SymbolView()
{
	// nothing to do here
}

ApplicationView::ApplicationView(const NSRect& rect, const BinaryAnalyzerRef& binaryAnalyzer) :
	SymbolView(rect, binaryAnalyzer, Strings{"Property", "Size", "Name"})
{
	update();
}

ApplicationView::ApplicationView(ApplicationView&& view)
{
	*this = std::move(view);
}

ApplicationView::~ApplicationView()
{
	// nothing to do here
}

void ApplicationView::setSymbolFilterText(const std::string& text)
{
	if (symbolFilterText_ == text)
		return;

	symbolFilterText_ = text;

	update();
}

void ApplicationView::setSymbolFilterCaseSensitive(const bool caseSensitive)
{
	if (symbolFilterCaseSensitive_ == caseSensitive)
		return;

	symbolFilterCaseSensitive_ = caseSensitive;

	update();
}

void ApplicationView::setShowRootSymbolsOnly(const bool showRootSymbolsOnly)
{
	if (showRootSymbolsOnly_ == showRootSymbolsOnly)
		return;

	showRootSymbolsOnly_ = showRootSymbolsOnly;

	update();
}

void ApplicationView::setShowChildSymbols(const bool showChildSymbols)
{
	if (showChildSymbols_ == showChildSymbols)
		return;

	showChildSymbols_ = showChildSymbols;

	update();
}

void ApplicationView::update()
{
	removeItems();

	if (binaryAnalyzer_)
	{
		if (binaryAnalyzer_->isState(BinaryAnalyzer::AS_WORKING))
		{
			createItem({"", "", "Analyzing in progress, please wait until the analyzer has finished!"}, rootItemId());
			createItem({"", "", "This may need several minutes..."}, rootItemId());

			return;
		}

		addSymbols();
	}
}

ApplicationView& ApplicationView::operator=(ApplicationView&& view)
{
	if (this != &view)
	{
		SymbolView::operator=(std::move(view));

		orderMode_ = view.orderMode_;
		view.orderMode_ = OM_ID_ASCENDING;

		symbolFilterText_ = std::move(view.symbolFilterText_);
		symbolFilterCaseSensitive_ = view.symbolFilterCaseSensitive_;
		view.symbolFilterCaseSensitive_ = false;

		showChildSymbols_ = view.showChildSymbols_;
		view.showChildSymbols_ = false;
	}

	return *this;
}

bool ApplicationView::addSymbols()
{
	ocean_assert(binaryAnalyzer_);
	if (!binaryAnalyzer_)
		return false;

	setColumnWidth(0u, 80);
	setColumnWidth(1u, 50);
	setColumnWidth(2u, 1000);

	const BinaryAnalyzer::Symbols& symbols = binaryAnalyzer_->symbols();

	const std::string propertyText[2] =
	{
		std::string("root"),
		std::string("local")
	};

	BinaryAnalyzer::SymbolIds subsetSymbolIds;
	subsetSymbolIds.reserve(symbols.size());

	// we check whether we have separate symbol filters

	const std::string adjustedSymbolFilterText = symbolFilterCaseSensitive_ ? symbolFilterText_ : String::toLower(symbolFilterText_);

	std::vector<std::string> symbolFilterTexts;

	size_t tokenStart = 0;
	size_t tokenLength = 0;

	while (!adjustedSymbolFilterText.empty())
	{
		tokenLength = 0;
		IO::Scanner::findNextToken(adjustedSymbolFilterText.c_str(), adjustedSymbolFilterText.size(), tokenStart, tokenStart, tokenLength);

		if (tokenLength != 0)
		{
			symbolFilterTexts.push_back(adjustedSymbolFilterText.substr(tokenStart, tokenLength));
			tokenStart += tokenLength;
		}
		else
		{
			break;
		}
	}

	// now let's apply our filter

	if (symbolFilterCaseSensitive_)
	{
		if (symbolFilterTexts.size() > 1)
		{
			for (size_t n = 0; n < symbols.size(); ++n)
			{
				const BinaryAnalyzer::Symbol& symbol = symbols[n];

				if (!showRootSymbolsOnly_ || symbol.isRootSymbol())
				{
					bool matchAll = true;

					for (size_t i = 0; matchAll && i < symbolFilterTexts.size(); ++i)
					{
						matchAll = symbol.readableName().find(symbolFilterTexts[i]) != std::string::npos;
					}

					if (matchAll)
					{
						subsetSymbolIds.emplace_back(BinaryAnalyzer::SymbolId(n));
					}
				}
			}
		}
		else
		{
			for (size_t n = 0; n < symbols.size(); ++n)
			{
				const BinaryAnalyzer::Symbol& symbol = symbols[n];

				if (!showRootSymbolsOnly_ || symbol.isRootSymbol())
				{
					if (symbolFilterText_.empty() || symbol.readableName().find(symbolFilterText_) != std::string::npos)
					{
						subsetSymbolIds.emplace_back(BinaryAnalyzer::SymbolId(n));
					}
				}
			}
		}
	}
	else
	{
		if (symbolFilterTexts.size() > 1)
		{
			for (size_t n = 0; n < symbols.size(); ++n)
			{
				const BinaryAnalyzer::Symbol& symbol = symbols[n];

				if (!showRootSymbolsOnly_ || symbol.isRootSymbol())
				{
					const std::string lowerReadableName = String::toLower(symbol.readableName());

					bool matchAll = true;
					for (size_t i = 0; matchAll && i < symbolFilterTexts.size(); ++i)
					{
						matchAll = lowerReadableName.find(symbolFilterTexts[i]) != std::string::npos;
					}

					if (matchAll)
					{
						subsetSymbolIds.emplace_back(BinaryAnalyzer::SymbolId(n));
					}
				}
			}
		}
		else
		{
			for (size_t n = 0; n < symbols.size(); ++n)
			{
				const BinaryAnalyzer::Symbol& symbol = symbols[n];

				if (!showRootSymbolsOnly_ || symbol.isRootSymbol())
				{
					if (adjustedSymbolFilterText.empty() || String::toLower(symbol.readableName()).find(adjustedSymbolFilterText) != std::string::npos)
					{
						subsetSymbolIds.emplace_back(BinaryAnalyzer::SymbolId(n));
					}
				}
			}
		}
	}

	sortSymbols(symbols, subsetSymbolIds, orderMode_);

	for (BinaryAnalyzer::SymbolIds::const_iterator i = subsetSymbolIds.begin(); i != subsetSymbolIds.end(); ++i)
	{
		const BinaryAnalyzer::Symbol& symbol = symbols[*i];

		const std::string aliasString = symbol.readableAliasNames().empty() ? std::string() : (std::string("(") + String::toAString(symbol.readableAliasNames().size()) + std::string(" alias) "));

		std::vector<std::string> texts = {propertyText[symbol.isRootSymbol() ? 0 : 1], String::toAString(symbol.size()), aliasString + String::toAString(symbol.readableName())};
		const TreeItemRef treeItem = createItem(std::move(texts), rootItemId(), TreeView::invalidTreeItemId, false);
		registerTreeItem(treeItem->id(), *i);

		if (showChildSymbols_)
		{
			// we do not apply name filters or orderings for child symbols

			const BinaryAnalyzer::SymbolIdSet& childSymbolIds = symbol.childSymbols();

			for (BinaryAnalyzer::SymbolIdSet::const_iterator iC = childSymbolIds.cbegin(); iC != childSymbolIds.cend(); ++iC)
			{
				const BinaryAnalyzer::Symbol& childSymbol = symbols[*iC];

				std::vector<std::string> childTexts = {std::string(), std::string(), std::string("  ->  ") + String::toAString(childSymbol.readableName())};
				const TreeItemRef childItem = createItem(std::move(childTexts), treeItem->id(), TreeView::invalidTreeItemId, false);
				registerTreeItem(childItem->id(), *iC);
			}
		}
	}

	refreshItems();

	return true;
}

void ApplicationView::onMouseDownInHeader(const unsigned int columnIndex)
{
	ocean_assert(binaryAnalyzer_);
	if (!binaryAnalyzer_)
		return;

	OrderMode previousOrderMode = orderMode_;

	if (columnIndex == 0u)
	{
		orderMode_ = OM_ID_ASCENDING;
	}
	else if (columnIndex == 1u)
	{
		// we sort all entries based on the size of the symbols

		if (orderMode_ == OM_SIZE_ASCENDING)
		{
			orderMode_ = OM_SIZE_DESCENDING;
		}
		else
		{
			orderMode_ = OM_SIZE_ASCENDING;
		}
	}
	else if (columnIndex == 2u)
	{
		// we sort all entries based on the name of the symbols

		if (orderMode_ == OM_NAME_ASCENDING)
		{
			orderMode_ = OM_NAME_DESCENDING;
		}
		else
		{
			orderMode_ = OM_NAME_ASCENDING;
		}
	}

	if (orderMode_ == previousOrderMode)
		return;

	update();
}

void ApplicationView::onItemSelected(const TreeItemId itemId)
{
	AppDelegate* appDelegate = [[NSApplication sharedApplication] delegate];
	[appDelegate onChangedSelectionSymbolView];
}

int ApplicationView::showContextMenu(Platform::Apple::MacOS::ContextMenu& contextMenu)
{
	ocean_assert(binaryAnalyzer_);

	if (!binaryAnalyzer_ || binaryAnalyzer_->state() != BinaryAnalyzer::AS_SUCCEEDED)
	{
		return SymbolView::showContextMenu(contextMenu);
	}

	const TreeItemIds treeItems = selectedItemIds();

	contextMenu.addItem("-");
	//const int menuEntryShowRootTraces = contextMenu.addItem("Show root traces", treeItems.size() == 1);
	const int menuEntryShowRootGraph = contextMenu.addItem("Show graph to root", treeItems.size() == 1);
	const int menuEntryShowChildGraph = contextMenu.addItem("Show graph to children", treeItems.size() == 1);

	const int userSelection = SymbolView::showContextMenu(contextMenu);

	/*if (userSelection == menuEntryShowRootTraces)
	{
		const BinaryAnalyzer::SymbolId id = symbolId(treeItems.front());

		ocean_assert(id != BinaryAnalyzer::invalidSymbolId);
		if (id != BinaryAnalyzer::invalidSymbolId)
		{
			AppDelegate* appDelegate = [[NSApplication sharedApplication] delegate];
			[appDelegate showCallTracesForSymbol:id];
		}
	}
	else*/ if (userSelection == menuEntryShowChildGraph || userSelection == menuEntryShowRootGraph)
	{
		const BinaryAnalyzer::SymbolId id = objectId(treeItems.front());

		ocean_assert(id != BinaryAnalyzer::invalidSymbolId);
		if (id != BinaryAnalyzer::invalidSymbolId)
		{
			AppDelegate* appDelegate = [[NSApplication sharedApplication] delegate];
			[appDelegate showCallGraphForSymbol:id forChildren:userSelection == menuEntryShowChildGraph];
		}
	}

	return userSelection;
}
