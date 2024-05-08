/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/misc/appinspector/osx/CallGraphWindow.h"
#include "application/ocean/demo/misc/appinspector/osx/CallGraphView.h"

#include "ocean/base/StringApple.h"

/**
 * Definition of a window mainly holding a CallGraphView
 */
@interface CallGraphWindow ()
{
	/// The binary analyzer providing the information.
	BinaryAnalyzerRef binaryAnalyzer_;

	/// The view showing the call graph.
	CallGraphView* callGraphView_;

	/// The label showing the binary impact.
	NSTextField* textField_;
}

@end

@implementation CallGraphWindow

-(id)initWithContentRect:(NSRect)contentRect styleMask:(NSWindowStyleMask)style backing:(NSBackingStoreType)backingStoreType defer:(BOOL)flag screen:(NSScreen *)screen
{
	if (![super initWithContentRect:contentRect styleMask:style backing:backingStoreType defer:flag screen:screen])
	{
		return nil;
	}

	callGraphView_ = nullptr;
	textField_ = nil;

	return self;
}

-(void)showGraph:(BinaryAnalyzerRef)binaryAnalyzer forSymbol:(BinaryAnalyzer::SymbolId)symbolId forChildren:(bool)children
{
	ocean_assert(binaryAnalyzer);

	ocean_assert(!callGraphView_);
	if (callGraphView_)
	{
		return;
	}

	binaryAnalyzer_ = binaryAnalyzer;

	const BinaryAnalyzer::Symbol& symbol = binaryAnalyzer_->symbol(symbolId);

	BinaryAnalyzer::GraphNodeRefs graphNodes;

	if (children)
	{
		const BinaryAnalyzer::GraphNodeRef rootNode = binaryAnalyzer_->determineReducedCallGraphForChildren(symbol.id());
		graphNodes.push_back(rootNode);
	}
	else
	{
		const BinaryAnalyzer::SymbolIdGroups callTraces = binaryAnalyzer_->determineCallTraces(symbol.id(), true);
		graphNodes = binaryAnalyzer_->callTraces2callNodes(callTraces);
	}

	[self cascadeTopLeftFromPoint:NSMakePoint(20,20)];
	[self setTitle:StringApple::toNSString("Call graph for \"" + symbol.readableName() + "\"")];
	[self makeKeyAndOrderFront:nil];
	self.minSize = NSMakeSize(300, 200);

	const NSRect windowRect = [self frame];

	// we add the actual graph view

	const NSRect callGraphViewRect = NSMakeRect(10, 40, windowRect.size.width - 20, windowRect.size.height - 80);
	callGraphView_ = new CallGraphView(callGraphViewRect, binaryAnalyzer_, std::move(graphNodes));
	[[self contentView] addSubview:callGraphView_->nsView()];

	// we add the text field displaying the binary size information

	const NSRect textFieldRect = NSMakeRect(10, 10, windowRect.size.width - 20, 20);
	textField_ = [[NSTextField alloc] initWithFrame:textFieldRect];
	textField_.autoresizingMask = NSViewWidthSizable;
	textField_.selectable = false;
	textField_.drawsBackground = false;
	textField_.bezeled = false;
	textField_.stringValue = @"Select a symbol to calculate the impact of the binary size";
	[[self contentView] addSubview:textField_];

	[self setReleasedWhenClosed:NO];
}

-(void)onSelectionChanged
{
	ocean_assert(binaryAnalyzer_ && callGraphView_ != nullptr && textField_ != nullptr);
	if (!binaryAnalyzer_ || callGraphView_ == nullptr || textField_ == nullptr)
		return;

	const std::string text = binaryAnalyzer_->determineSizeImpactString(callGraphView_->selectedObjectIds());

	if (text.empty())
	{
		textField_.stringValue = @"Select a symbol to calculate the impact of the binary size";
	}
	else
	{
		textField_.stringValue = StringApple::toNSString(text);
	}
}

@end
