// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#import "ViewController.h"

#include "ocean/base/Messenger.h"
#include "ocean/base/StringApple.h"

#include "ocean/platform/apple/Resource.h"

#include "ocean/test/testcv/testopencv/TestOpenCV.h"

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
	const double testDurations = 2.0;

	// define the subset of functions which will be invoked, an empty set invokes all functions
	const std::string testFunctions = "";

	// we seek for the resource file containing the media file
	const std::wstring testMediaFilename = Ocean::Platform::Apple::Resource::resourcePath(L"sift800x640", L"bmp");

	Test::TestCV::TestOpenCV::testCVOpenCVAsynchron(testDurations, String::toAString(testMediaFilename), testFunctions);

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
