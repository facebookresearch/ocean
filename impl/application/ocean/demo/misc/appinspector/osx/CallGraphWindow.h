/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_OCEAN_APPLICATION_OCEAN_DEMO_MISC_APPINSPECTOR_CALL_GRAPH_WINDOW_H
#define FACEBOOK_OCEAN_APPLICATION_OCEAN_DEMO_MISC_APPINSPECTOR_CALL_GRAPH_WINDOW_H

#include "application/ocean/demo/misc/appinspector/osx/AppDelegate.h"

#include "application/ocean/demo/misc/appinspector/BinaryAnalyzer.h"

#include <AppKit/AppKit.h>

/**
 * Definition of a window mainly holding a CallGraphView.
 * ingroup applicationdemomiscappinspectorosx
 */
@interface CallGraphWindow : NSWindow<NSWindowDelegate>

-(void)showGraph:(BinaryAnalyzerRef)binaryAnalyzer forSymbol:(BinaryAnalyzer::SymbolId)symbolId forChildren:(bool)children;

/**
 * Event function for changed selections in the symbol view.
 */
-(void)onSelectionChanged;

@end

#endif // FACEBOOK_OCEAN_APPLICATION_OCEAN_DEMO_MISC_APPINSPECTOR_CALL_GRAPH_WINDOW_H
