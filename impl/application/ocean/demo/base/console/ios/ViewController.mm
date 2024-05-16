/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#import "ViewController.h"

#include "ocean/base/Build.h"
#include "ocean/base/DateTime.h"
#include "ocean/base/Messenger.h"
#include "ocean/base/Processor.h"

#include "ocean/platform/apple/System.h"
#include "ocean/platform/apple/Utilities.h"

#include "ocean/platform/apple/ios/MessengerView.h"

using namespace Ocean;

@interface ViewController ()
{
	/// The view showing Ocean's log messages.
	@protected MessengerView* messengerView_;
}

@end

@implementation ViewController

- (void)viewDidLoad
{
	[super viewDidLoad];

	// writing the logs to standard output (e.g,. debug window) and queuing the messages to allow popping them
	Messenger::get().setOutputType(Messenger::MessageOutput(Messenger::OUTPUT_STANDARD | Messenger::OUTPUT_QUEUED));

	const CGRect rect = [[UIScreen mainScreen] applicationFrame];
	messengerView_ = [[MessengerView alloc] initWithFrame:rect];
	[self.view addSubview:messengerView_];

	Log::info() << "Console demo application";
	Log::info() << " ";

	Log::info() << "Platform: " << Build::buildString();
	Log::info() << "Build: " << Build::buildString();
	Log::info() << "Time: " << DateTime::localString();
	Log::info() << " ";

	Log::info() << "While the hardware supports the following SIMD instructions:";
	Log::info() << Processor::translateInstructions(Processor::get().instructions());
	Log::info() << " ";

	Log::info() << "Processor: " << Processor::brand();
	Log::info() << " ";

	unsigned int major = 0u;
	unsigned int minor = 0u;
	const double version = Platform::Apple::System::version(&major, &minor);

	Log::info() << "OS Version: " << version << ", " << major << "." << minor;
	Log::info() << " ";

	std::string bundleVersion;
	std::string bundleVersionKey;
	if (Platform::Apple::Utilities::bundleVersion(bundleVersion, &bundleVersionKey))
	{
		Log::info() << "Bundle version: " << bundleVersion << ", " << bundleVersionKey;
		Log::info() << " ";
	}

	Log::info() << "... do something here ...";
	Log::info() << " ";

	Log::info() << "Finished";
}

@end
