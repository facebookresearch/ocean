// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#import "ViewController.h"

#include "ocean/base/Messenger.h"
#include "ocean/base/StringApple.h"

#include "ocean/test/testcv/testlibyuv/TestLibyuv.h"

using namespace Ocean;

@interface ViewController ()
@end

@implementation ViewController

- (void)viewDidLoad
{
	[super viewDidLoad];

#if 0
	// Enable this block to redirect the output to Xcode
	Messenger::get().setOutputType(Messenger::OUTPUT_QUEUED);
#endif

	// define the number of seconds each test is applied
	const double testDurations = 5.0;

	// True, to skip the validation and to apply just the benchmarking
	const bool skipValidation = false;

	// define the subset of functions which will be invoked, an empty set invokes all functions
	const std::string testFunctions = "";

	Test::TestCV::TestLibyuv::testCVLibyuvAsynchron(testDurations, skipValidation, testFunctions);

	[NSTimer scheduledTimerWithTimeInterval:0.05 target:self selector:@selector(timerTicked:) userInfo:nil repeats:YES];
}

- (void)timerTicked:(NSTimer*)timer
{
	const std::string message(Messenger::get().popMessage());

	if (!message.empty())
		[_controllerTextView setText:[[_controllerTextView text] stringByAppendingString:StringApple::toNSString(message + std::string("\n"))]];
}

- (void)didReceiveMemoryWarning
{
	[super didReceiveMemoryWarning];
}

@end
