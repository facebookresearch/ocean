/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/apple/ios/MessengerView.h"

#include "ocean/base/Messenger.h"
#include "ocean/base/StringApple.h"

#import <Foundation/Foundation.h>

#import <UIKit/UIKit.h>

@interface MessengerView ()
{
}

@end

using namespace Ocean;

@implementation MessengerView

- (id)initWithFrame:(CGRect)frame
{
	if (self = [super initWithFrame:frame])
	{
		self.editable = NO;

		[NSTimer scheduledTimerWithTimeInterval:0.05 target:self selector:@selector(timerTicked:) userInfo:nil repeats:YES];
	}

	return self;
}

- (void)timerTicked:(NSTimer*)timer
{
	Messenger::MessageType messageType = Messenger::TYPE_UNDEFINED;

	std::string location;
	std::string message;

	size_t remainingMessages = 50;

	while (remainingMessages-- != 0 && Messenger::get().popMessage(messageType, location, message))
	{
		if (messageType == Messenger::TYPE_ERROR)
		{
			message = "Error: " + message;
		}
		else if (messageType == Messenger::TYPE_WARNING)
		{
			message = "Warning: " + message;
		}

		[self setText:[[self text] stringByAppendingString:StringApple::toNSString(message + std::string("\n"))]];
	}
}

@end
