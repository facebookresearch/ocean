/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/misc/appinspector/osx/AppDelegate.h"
#include "application/ocean/demo/misc/appinspector/osx/ApplicationView.h"
#include "application/ocean/demo/misc/appinspector/osx/CallGraphView.h"
#include "application/ocean/demo/misc/appinspector/osx/CallGraphWindow.h"
#include "application/ocean/demo/misc/appinspector/osx/ConsoleWindow.h"

#include "application/ocean/demo/misc/appinspector/BinaryAnalyzer.h"

#include "ocean/base/StringApple.h"

#include "ocean/io/File.h"
#include "ocean/io/Directory.h"

#include "ocean/platform/apple/System.h"
#include "ocean/platform/apple/Utilities.h"

@interface AppDelegate ()
{
	/// The main view of this application.
	ApplicationView applicationView_;

	/// The console window showing information messages.
	ConsoleWindow* consoleWindow_;

	/// The binary analyzer that actually holds the information of the binary.
	BinaryAnalyzerRef binaryAnalyzer_;

	/// The previous state of the analyzer, used to handle updates.
	BinaryAnalyzer::AnalyzerState previousAnalyzerState;
}

/// The window object.
@property (weak) IBOutlet NSWindow* mainWindow;

/// The view object.
@property (weak) IBOutlet NSView* mainWindowView;

/// The text field used for filtering.
@property (weak) IBOutlet NSTextField *mainWindowTextField;

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification*)aNotification
{
	// writing the logs to standard output (e.g,. debug window) and queuing the messages to allow popping them
	Messenger::get().setOutputType(Messenger::MessageOutput(Messenger::OUTPUT_STANDARD | Messenger::OUTPUT_QUEUED));

	binaryAnalyzer_ = std::make_shared<BinaryAnalyzer>();
	previousAnalyzerState = BinaryAnalyzer::AS_EMPTY;

	NSRect rect = [_mainWindowView frame];
	rect = NSMakeRect(rect.origin.x + 10, rect.origin.y + 30, rect.size.width - 20, rect.size.height - 90);

	applicationView_ = ApplicationView(rect, binaryAnalyzer_);
	applicationView_.nsView().autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
	[_mainWindowView addSubview:applicationView_.nsView()];

	// Now, we create a window with scrollable text view as our consoel window
	consoleWindow_ = [[ConsoleWindow alloc] initWithContentRect:NSMakeRect(0, 0, 400, 600) styleMask:(NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask) backing:NSBackingStoreBuffered defer:NO];
	[consoleWindow_ cascadeTopLeftFromPoint:NSMakePoint(20,20)];
	[consoleWindow_ makeKeyAndOrderFront:nil];
	[consoleWindow_ setReleasedWhenClosed:NO];

	[NSTimer scheduledTimerWithTimeInterval:0.05 target:self selector:@selector(timerTicked:) userInfo:nil repeats:YES];

	[_mainWindow makeKeyAndOrderFront:nil];
}

/**
 * The release event function.
 */
- (void)applicationWillTerminate:(NSNotification*)aNotification
{
	// nothing to do here
}

/**
 * Checks whether the application should terminate when the last windows is closed.
 * @return True, if so
 */
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender
{
	return YES;
}

- (void)timerTicked:(NSTimer*)timer
{
	if (previousAnalyzerState != binaryAnalyzer_->state())
	{
		previousAnalyzerState = binaryAnalyzer_->state();
		applicationView_.update();
	}
}

- (IBAction)onMenuFileOpen:(id)sender
{
	ocean_assert(binaryAnalyzer_);
	if (!binaryAnalyzer_)
		return;

	if (binaryAnalyzer_->isState(BinaryAnalyzer::AS_WORKING))
	{
		Ocean::Platform::Apple::Utilities::showMessageBox("Error", "Analyzing still in progress!");
		return;
	}

	NSOpenPanel* panel = [NSOpenPanel openPanel];

	[panel beginWithCompletionHandler:^(NSInteger result)
	{
		if (result == NSFileHandlingPanelOKButton)
		{
			NSURL* fileURL = [[panel URLs] objectAtIndex:0];

			std::string filename = Ocean::StringApple::toUTF8([fileURL path]);

			// we check whether the user has selected an app bundle
			// in this case, we need to get the actual binary within the bundle

			const IO::Directory bundleDirectory(filename);

			if (bundleDirectory.exists())
			{
				Log::info() << "The selected file is a directory e.g., an .app";
				Log::info() << "We will search for the actual binary within the directory";

				std::string appName = bundleDirectory.name();

				const std::string::size_type pos = appName.find(".app");
				if (pos != std::string::npos)
				{
					appName.resize(pos);
				}

				const IO::File bundleBinary(bundleDirectory + IO::File(appName));

				if (bundleBinary.exists())
				{
					filename = bundleBinary();

					Log::info() << "We could find the following binary: \"" << filename << "\"";
				}
				else
				{
					Ocean::Platform::Apple::Utilities::showMessageBox("Error", "You cannot load a directoy without binary file");
					return;
				}
			}

			if (!self->binaryAnalyzer_->analyzeBinaryAsynchron(filename))
			{
				Ocean::Platform::Apple::Utilities::showMessageBox("Error", "Could not parse the binary/data file");
				return;
			}
		}
	}];
}

- (IBAction)onMenuFileSave:(id)sender
{
	ocean_assert(binaryAnalyzer_);
	if (!binaryAnalyzer_)
		return;

	if (!self->binaryAnalyzer_->isState(BinaryAnalyzer::AS_SUCCEEDED))
	{
		Ocean::Platform::Apple::Utilities::showMessageBox("Error", "Nothing to save!");
		return;
	}

	NSSavePanel* panel = [NSSavePanel savePanel];
	[panel setAllowedFileTypes:[NSArray arrayWithObjects:@"asa", @"json", nil]];

	[panel beginSheetModalForWindow:_mainWindow completionHandler:^(NSInteger result)
	{
		 if (result == NSFileHandlingPanelOKButton)
		 {
			 NSURL* fileURL =  [panel URL];

			 const std::string filename = Ocean::StringApple::toUTF8([fileURL path]);

			 // **TODO** show progress bar

			 NSString* extension = [fileURL pathExtension];
			 BOOL successfullySaved = NO;
			 if (extension != nil && [@"json" isEqualToString:extension])
			 {
				 successfullySaved = self->binaryAnalyzer_->writeToJsonFile(filename);
			 }
			 else
			 {
				 successfullySaved = self->binaryAnalyzer_->writeToDataFile(filename);
			 }

			 if (!successfullySaved)
			 {
				 Ocean::Platform::Apple::Utilities::showMessageBox("Error", "Could not write the information");
				 return;
			 }
		 }
	 }];
}

- (IBAction)onTextFieldEvent:(id)sender
{
	NSTextField* textField = (NSTextField*)sender;

	applicationView_.setSymbolFilterText(Ocean::StringApple::toUTF8([textField stringValue]));
}

- (IBAction)onFilterCaseSensitiveClicked:(id)sender
{
	applicationView_.setSymbolFilterCaseSensitive([(NSButton*)sender state] == NSOnState);
}

- (IBAction)onShowRootSymbolsOnlyClicked:(id)sender
{
	applicationView_.setShowRootSymbolsOnly([(NSButton*)sender state] == NSOnState);
}

- (IBAction)onShowChildSymbolsClicked:(id)sender
{
	applicationView_.setShowChildSymbols([(NSButton*)sender state] == NSOnState);
}

-(void)onChangedSelectionSymbolView
{
	ocean_assert(binaryAnalyzer_);
	if (!binaryAnalyzer_)
		return;

	const std::string text = binaryAnalyzer_->determineSizeImpactString(applicationView_.selectedObjectIds());

	_mainWindowTextField.stringValue = StringApple::toNSString(text);
}

-(void)showCallGraphForSymbol:(BinaryAnalyzer::SymbolId)symbolId forChildren:(bool)children;
{
	CallGraphWindow* callGraphWindow = [[CallGraphWindow alloc] initWithContentRect:NSMakeRect(0, 0, 800, 600) styleMask:(NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask) backing:NSBackingStoreBuffered defer:NO];

	[callGraphWindow showGraph:binaryAnalyzer_ forSymbol:symbolId forChildren:children];

	[[self mainWindow] addChildWindow:callGraphWindow ordered:NSWindowAbove];
}

@end
