/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_OCEAN_APPLICATION_OCEAN_DEMO_MISC_APPINSPECTOR_APP_DELEGATE_H
#define FACEBOOK_OCEAN_APPLICATION_OCEAN_DEMO_MISC_APPINSPECTOR_APP_DELEGATE_H

#include "application/ocean/demo/misc/ApplicationDemoMisc.h"

#include "application/ocean/demo/misc/appinspector/BinaryAnalyzer.h"

#import <Cocoa/Cocoa.h>

/**
 * @ingroup applicationdemomisc
 * @defgroup applicationdemomiscappinspectorosx App Inspector (OSX)
 * @{
 * The demo application demonstrates the implementation of a simple analyzer for binary sizes, symbol/function hierarchies, and data elements for iOS and Android apps.<br>
 * This application is available for OSX platforms only.
 * @}
 */

using namespace Ocean;

/**
 * The delegate of this application.
 * @ingroup applicationdemomiscappinspectorosx
 */
@interface AppDelegate : NSObject <NSApplicationDelegate>

-(void)showCallGraphForSymbol:(BinaryAnalyzer::SymbolId)symbolId forChildren:(bool)children;

/**
 * Event function for changed selections in the symbol view.
 */
-(void)onChangedSelectionSymbolView;

@end

#endif // FACEBOOK_OCEAN_APPLICATION_OCEAN_DEMO_MISC_APPINSPECTOR_APP_DELEGATE_H
