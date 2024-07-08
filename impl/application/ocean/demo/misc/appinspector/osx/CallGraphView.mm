/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/misc/appinspector/osx/CallGraphView.h"
#include "application/ocean/demo/misc/appinspector/osx/CallGraphWindow.h"

CallGraphView::CallGraphView() :
	SymbolView()
{
	// nothing to do here
}

CallGraphView::CallGraphView(const NSRect& rect, const BinaryAnalyzerRef& binaryAnalyzer, BinaryAnalyzer::GraphNodeRefs&& callGraphRootNodes) :
	SymbolView(rect, binaryAnalyzer, Strings{"Symbol"}),
	callGraphRootNodes_(std::move(callGraphRootNodes))
{
	update();
}

CallGraphView::CallGraphView(CallGraphView&& callGraphView)
{
	*this = std::move(callGraphView);
}

CallGraphView::~CallGraphView()
{
	// nothing to do here
}

void CallGraphView::update()
{
	removeItems();
	addGraph();
}

CallGraphView& CallGraphView::operator=(CallGraphView&& callGraphView)
{
	if (this != &callGraphView)
	{
		SymbolView::operator=(std::move(callGraphView));

		callGraphRootNodes_ = std::move(callGraphView.callGraphRootNodes_);
	}

	return *this;
}

bool CallGraphView::addGraph()
{
	ocean_assert(binaryAnalyzer_);
	if (!binaryAnalyzer_)
		return false;

	setColumnWidth(0u, 1000);

	typedef std::pair<TreeItemId, BinaryAnalyzer::GraphNodeRef> NodePair;

	std::vector<NodePair> pairStack;
	pairStack.reserve(128);

	for (BinaryAnalyzer::GraphNodeRefs::const_iterator i = callGraphRootNodes_.cbegin(); i != callGraphRootNodes_.cend(); ++i)
	{
		pairStack.emplace_back(std::make_pair(rootItemId(), *i));
	}

	while (!pairStack.empty())
	{
		const TreeItemId treeItemId = pairStack.back().first;
		const BinaryAnalyzer::GraphNodeRef node = pairStack.back().second;
		ocean_assert(node);

		pairStack.pop_back();

		const BinaryAnalyzer::Symbol& symbol = binaryAnalyzer_->symbol(node->symbolId());

		const std::string linkSuffix = node->childNodes().empty() && !symbol.childSymbols().empty() ? "  ----> Additional children" : "";

		const std::string aliasString = symbol.readableAliasNames().empty() ? std::string() : (std::string(" (") + String::toAString(symbol.readableAliasNames().size()) + std::string(" alias) "));

		std::vector<std::string> texts = {"[" + String::toAString(symbol.size()) + "] " + aliasString + String::toAString(symbol.readableName()) + linkSuffix};
		const TreeItemRef rootTreeItem = createItem(std::move(texts), treeItemId);
		registerTreeItem(rootTreeItem->id(), symbol.id());

		for (BinaryAnalyzer::GraphNodeRefs::const_iterator i = node->childNodes().cbegin(); i != node->childNodes().cend(); ++i)
		{
			pairStack.emplace_back(std::make_pair(rootTreeItem->id(), *i));
		}
	}

	rootItem().expand(true);

	return true;
}

void CallGraphView::onItemSelected(const TreeItemId itemId)
{
	// tell our parent window that the selection has changed

	[(CallGraphWindow*)[nsView() window] onSelectionChanged];
}

int CallGraphView::showContextMenu(Platform::Apple::MacOS::ContextMenu& contextMenu)
{
	ocean_assert(binaryAnalyzer_);
	if (!binaryAnalyzer_ || binaryAnalyzer_->state() != BinaryAnalyzer::AS_SUCCEEDED)
	{
		return SymbolView::showContextMenu(contextMenu);
	}

	const TreeItemIds treeItems = selectedItemIds();

	contextMenu.addItem("-");
	const int menuEntryShowRootGraph = contextMenu.addItem("Show graph to root", treeItems.size() == 1);
	const int menuEntryShowChildGraph = contextMenu.addItem("Show graph to children", treeItems.size() == 1);

	const int userSelection = SymbolView::showContextMenu(contextMenu);

	if (userSelection == menuEntryShowChildGraph || userSelection == menuEntryShowRootGraph)
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
