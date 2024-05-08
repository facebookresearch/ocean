// Copyright (c) Meta Platforms, Inc. and affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#import "ViewController.h"

#include "ocean/base/Messenger.h"
#include "ocean/base/StringApple.h"

#include "ocean/test/testcv/testadvanced/TestCVAdvanced.h"

using namespace Ocean;

@interface ViewController ()
@end

@implementation ViewController

- (void)viewDidLoad
{
	[super viewDidLoad];

	Messenger::get().setOutputType(Messenger::OUTPUT_QUEUED);

	// define the number of seconds each test will be executed
	const double testDurations = 2.0;

	// define the dimension of the default test frame in pixel
	const unsigned int testWidth = 1920u;
	const unsigned int testHeight = 1080u;

	// define the subset of functions which will be invoked, an empty set invokes all functions
	const std::string testFunctions;

	Test::TestCV::TestAdvanced::testCVAdvancedAsynchron(testDurations, testWidth, testHeight, testFunctions);

	[NSTimer scheduledTimerWithTimeInterval:0.05 target:self selector:@selector(timerTicked:) userInfo:nil repeats:YES];
}

- (void)timerTicked:(NSTimer*)timer
{
	const std::string message(Messenger::get().popMessage());

	if (!message.empty())
	{
		[_controllerTextView setText:[[_controllerTextView text] stringByAppendingString:StringApple::toNSString(message + std::string("\n"))]];
	}
}

- (void)didReceiveMemoryWarning
{
	[super didReceiveMemoryWarning];
}

@end
