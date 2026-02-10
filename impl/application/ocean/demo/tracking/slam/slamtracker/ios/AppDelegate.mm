/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "AppDelegate.h"

#include "application/ocean/demo/tracking/slam/slamtracker/SLAMTrackerWrapper.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Messenger.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/Processor.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/String.h"
#include "ocean/base/StringApple.h"
#include "ocean/base/Thread.h"

#include "ocean/io/CameraCalibrationManager.h"

#include "ocean/media/Manager.h"
#include "ocean/media/PixelImage.h"

#include "ocean/platform/apple/Resource.h"
#include "ocean/platform/apple/ios/GLFrameViewController.h"

#ifdef OCEAN_RUNTIME_STATIC
	#include "ocean/rendering/glescenegraph/apple/Apple.h"
#endif

@interface AppDelegate ()
{
	/// The platform independent wrapper for the tracker.
	SLAMTrackerWrapper slamTrackerWrapper_;

	/// A text label showing the performance of the aligner.
	UILabel* textLabelPerformance_;

#ifdef OCEAN_DEBUG
	/// A text label showing debug mode indicator.
	UILabel* textLabelDebug_;
#endif

	/// The pixel image that will forward the image result from the feature tracker to the renderer.
	Media::PixelImageRef pixelImage_;

	/// Button to start/stop recording.
	UIButton* recordingButton_;

	/// State whether recording is active.
	bool isRecording_;
}
@end

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
	// we forward all messages to the debug window (of e.g., Xcode)
	Messenger::get().setOutputType(Messenger::OUTPUT_DEBUG_WINDOW);

	// for the static runtime we explicitly register the rendering plugin, media plugins will be registered
	Rendering::GLESceneGraph::Apple::registerGLESceneGraphEngine();

	self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
	self.window.backgroundColor = [UIColor whiteColor];

	GLFrameViewController* viewController = [[GLFrameViewController alloc] init];

	self.window.rootViewController = viewController;
	[self.window makeKeyAndVisible];

	textLabelPerformance_ = [[UILabel alloc] initWithFrame:CGRectMake(10, 10, 800, 20)];
	textLabelPerformance_.textColor = [UIColor blackColor];
	textLabelPerformance_.shadowColor = [UIColor whiteColor];
	[viewController.view addSubview:textLabelPerformance_];

#ifdef OCEAN_DEBUG
	// Add debug indicator in top right corner
	textLabelDebug_ = [[UILabel alloc] initWithFrame:CGRectMake(0, 10, 0, 0)];
	textLabelDebug_.text = @"(debug)";
	textLabelDebug_.textColor = [UIColor blackColor];
	textLabelDebug_.shadowColor = [UIColor whiteColor];
	textLabelDebug_.shadowOffset = CGSizeMake(1, 1);
	textLabelDebug_.font = [UIFont systemFontOfSize:10];
	[textLabelDebug_ sizeToFit];

	// Position in top right corner
	CGRect frame = textLabelDebug_.frame;
	frame.origin.x = viewController.view.bounds.size.width - frame.size.width - 30;
	textLabelDebug_.frame = frame;
	textLabelDebug_.autoresizingMask = UIViewAutoresizingFlexibleLeftMargin;

	[viewController.view addSubview:textLabelDebug_];
#endif

	isRecording_ = false;

#ifdef ALLOW_RECORDING

	// Create recording button
	recordingButton_ = [UIButton buttonWithType:UIButtonTypeSystem];
	recordingButton_.frame = CGRectMake(0, 0, 200, 50);
	recordingButton_.center = CGPointMake(viewController.view.bounds.size.width / 2, viewController.view.bounds.size.height - 80);
	[recordingButton_ setTitle:@"Start Recording" forState:UIControlStateNormal];
	recordingButton_.titleLabel.font = [UIFont boldSystemFontOfSize:18];
	recordingButton_.backgroundColor = [[UIColor greenColor] colorWithAlphaComponent:0.8];
	recordingButton_.layer.cornerRadius = 10;
	[recordingButton_ setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
	[recordingButton_ addTarget:self action:@selector(recordingButtonTapped:) forControlEvents:UIControlEventTouchUpInside];
	recordingButton_.autoresizingMask = UIViewAutoresizingFlexibleLeftMargin | UIViewAutoresizingFlexibleRightMargin | UIViewAutoresizingFlexibleTopMargin;
	[viewController.view addSubview:recordingButton_];

#endif // ALLOW_RECORDING

	std::wstring resourcePath = Platform::Apple::Resource::resourcePath(L"camera_calibration", L"json");

	if (!resourcePath.empty())
	{
		std::string resourcePathString = String::toAString(resourcePath);

		if (IO::CameraCalibrationManager::get().registerCalibrations(resourcePathString))
		{
			Log::info() << "Successfully loaded camera calibrations from resource file";

			IO::CameraCalibrationManager::get().setDeviceVersion(Processor::brand());
		}
		else
		{
			Log::warning() << "Failed to register camera calibrations from resource file";
		}
	}
	else
	{
		Log::warning() << "Failed to find camera_calibration.json in app bundle";
	}

	std::vector<std::wstring> commandLines;
	commandLines.emplace_back(L"--input"); // video source
	commandLines.emplace_back(L"LiveVideoId:2"); // video source
	commandLines.emplace_back(L"--resolution"); // video resolution
	commandLines.emplace_back(L"640x480"); // video resolution

	slamTrackerWrapper_ = SLAMTrackerWrapper(commandLines);

	// now we create a PixelImage that simply wrapps a Frame object
	// as the OpenGLES renderer uses Media objects for textures only
	// we will update this pixel image each time we receive an update from the tracker

	pixelImage_ = Media::Manager::get().newMedium("PixelImageForRenderer", Media::Medium::PIXEL_IMAGE);

	if (pixelImage_)
	{
		pixelImage_->setDevice_T_camera(slamTrackerWrapper_.frameMedium()->device_T_camera());
		pixelImage_->start();

		[viewController setFrameMedium:pixelImage_];
	}

	// we create a timer that invokes our tracker every 10ms

	[NSTimer scheduledTimerWithTimeInterval:0.01 target:self selector:@selector(timerTicked:) userInfo:nil repeats:YES];

	Thread::setThreadPriority(Thread::PRIORTY_ABOVE_NORMAL);

	return YES;
}

- (void)timerTicked:(NSTimer*)timer
{
	if (pixelImage_.isNull())
	{
		return;
	}

	Frame outputFrame;

	if (slamTrackerWrapper_.trackNewFrame(outputFrame))
	{
		pixelImage_->setPixelImage(std::move(outputFrame));
	}
}

- (void)recordingButtonTapped:(UIButton*)sender
{
	if (!isRecording_)
	{
		if (slamTrackerWrapper_.startRecording())
		{
			isRecording_ = true;

			[recordingButton_ setTitle:@"Stop Recording" forState:UIControlStateNormal];
			recordingButton_.backgroundColor = [[UIColor redColor] colorWithAlphaComponent:0.8];

			Log::info() << "Recording started";
		}
		else
		{
			Log::error() << "Failed to start recording";
		}
	}
	else
	{
		if (slamTrackerWrapper_.stopRecording())
		{
			isRecording_ = false;

			[recordingButton_ setTitle:@"Start Recording" forState:UIControlStateNormal];
			recordingButton_.backgroundColor = [[UIColor greenColor] colorWithAlphaComponent:0.8];

			Log::info() << "Recording stopped";
		}
		else
		{
			Log::error() << "Failed to stop recording";
		}
	}
}

- (void)applicationWillTerminate:(UIApplication *)application
{
	slamTrackerWrapper_.release();
}

@end
