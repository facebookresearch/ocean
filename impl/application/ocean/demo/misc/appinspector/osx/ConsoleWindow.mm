/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/misc/appinspector/osx/ConsoleWindow.h"

#include "ocean/base/Messenger.h"
#include "ocean/base/StringApple.h"

/**
 * Definition of a window mainly holding a CallGraphView
 */
@interface ConsoleWindow ()
{
	/// The text view showing all console messages.
	NSTextView* consoleTextView_;
}

@end

@implementation ConsoleWindow

-(id)initWithContentRect:(NSRect)contentRect styleMask:(NSWindowStyleMask)style backing:(NSBackingStoreType)backingStoreType defer:(BOOL)flag
{
	if (![super initWithContentRect:contentRect styleMask:style backing:backingStoreType defer:flag])
	{
		return nil;
	}

	[self setTitle:@"Console"];

	NSScrollView* scrollView = [[NSScrollView alloc] initWithFrame:[[self contentView] frame]];
	NSSize contentSize = [scrollView contentSize];
	[scrollView setHasVerticalScroller:YES];
	[scrollView setHasHorizontalScroller:YES];
	[scrollView setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];

	consoleTextView_ = [[NSTextView alloc] initWithFrame:NSMakeRect(0, 0, contentSize.width, contentSize.height)];

	[scrollView setDocumentView:consoleTextView_];
	[self setContentView:scrollView];

	[NSTimer scheduledTimerWithTimeInterval:0.05 target:self selector:@selector(timerTicked:) userInfo:nil repeats:YES];

	return self;
}

- (void)timerTicked:(NSTimer*)timer
{
	while (true)
	{
		const std::string message(Messenger::get().popMessage());

		if (message.empty())
		{
			return;
		}

		NSAttributedString* attributedString = [[NSAttributedString alloc] initWithString:StringApple::toNSString(message + std::string("\n"))];

		[[consoleTextView_ textStorage] appendAttributedString:attributedString];
		[consoleTextView_ scrollRangeToVisible:NSMakeRange([[consoleTextView_ string] length], 0)];
	}
}

@end
