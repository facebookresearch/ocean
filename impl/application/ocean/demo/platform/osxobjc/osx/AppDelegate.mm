/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/platform/osxobjc/osx/AppDelegate.h"
#include "application/ocean/demo/platform/osxobjc/osx/Wrapper.h"

#include "ocean/base/Build.h"
#include "ocean/base/StringApple.h"

using namespace Ocean;

@interface AppDelegate ()

/// The window object.
@property (weak) IBOutlet NSWindow* mainWindow;

/// The view object.
@property (weak) IBOutlet NSView* mainWindowView;

/// The label object of the build string.
@property (weak) IBOutlet NSTextField* mainWindowLabelBuildString;

/// The label object of the build date.
@property (weak) IBOutlet NSTextField* mainWindowLabelBuildDate;

/// The label object of the build time.
@property (weak) IBOutlet NSTextField* mainWindowLabelBuildTime;

/// The label object of the timestamp.
@property (weak) IBOutlet NSTextField* mainWindowLabelTimestamp;

/// The label object of the first struct.
@property (weak) IBOutlet NSTextField* mainWindowLabelStruct0;

/// The label object of the second struct.
@property (weak) IBOutlet NSTextField* mainWindowLabelStruct1;

/// The label object of the third struct.
@property (weak) IBOutlet NSTextField* mainWindowLabelStruct2;

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification*)aNotification
{
	// the following build string is not determined by a wrapper function but by the direct call of the corresponding C++ function
	[_mainWindowLabelBuildString setStringValue:StringApple::toNSString(Build::buildString())];

	// the following date string is determined by an explicit c-wrapper function
	const char* dateString = oceanBaseDateTimeString("date");

	[_mainWindowLabelBuildDate setStringValue:(NSString * _Nonnull)[[NSString alloc] initWithCString:dateString encoding:NSASCIIStringEncoding]];
	[_mainWindowLabelBuildTime setStringValue:StringApple::toNSString(Build::buildTime(__TIME__))];

	// we have to release the external string (however, we could also call free(dateString) here)
	oceanReleaseString(dateString);

	// in the following we use only the explicit c-wrapper functions (although the entire functionality could be realized without the wrapper)

	double timestamp = oceanBaseTimestamp();
	NSNumber* timestampNumber = [NSNumber numberWithDouble:timestamp];
	[_mainWindowLabelTimestamp setStringValue:[[timestampNumber stringValue] stringByAppendingString:@" seconds"]];


	const struct Struct structObject = oceanBaseStructObject();
	[_mainWindowLabelStruct0 setStringValue:[[[[NSNumber numberWithInt:structObject.valueInteger] stringValue] stringByAppendingString:@", "] stringByAppendingString:[[NSNumber numberWithFloat:structObject.valueFloat] stringValue]]];

	struct Struct* structPointer0 = oceanBaseStructStatic();
	[_mainWindowLabelStruct1 setStringValue:[[[[NSNumber numberWithInt:structPointer0->valueInteger] stringValue] stringByAppendingString:@", "] stringByAppendingString:[[NSNumber numberWithFloat:structPointer0->valueFloat] stringValue]]];

	structPointer0->valueInteger++;
	structPointer0->valueFloat += 0.64f;

	struct Struct* structPointer1 = oceanBaseStructStatic();
	[_mainWindowLabelStruct2 setStringValue:[[[[NSNumber numberWithInt:structPointer1->valueInteger] stringValue] stringByAppendingString:@", "] stringByAppendingString:[[NSNumber numberWithFloat:structPointer1->valueFloat] stringValue]]];

	[_mainWindowView addSubview:[[NSTextField alloc] initWithFrame:NSMakeRect(20, 20, 200, 40)]];
}

/**
 * The release event function.
 */
- (void)applicationWillTerminate:(NSNotification*)aNotification
{
	// insert code here to tear down your application
}

- (IBAction)onButtonPressed:(NSButton*)sender
{
	[_mainWindow setTitle:@"The button has been pressed!"];
	[sender setTitle:@"Pressed!"];
}

/**
 * The event function for the selected new menu.
 * @param sender The sender object
 */
- (IBAction)onMenuNew:(NSMenuItem*)sender
{
	[_mainWindow setTitle:@"The 'New' menu has been pressed!"];
	[sender setTitle:@"New Pressed!"];
}

@end
