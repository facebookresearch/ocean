/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "AppDelegate.h"

#include "application/ocean/demo/cv/detector/qrcodes/detector2d/Detector2DWrapper.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Messenger.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/String.h"
#include "ocean/base/StringApple.h"

#include "ocean/media/Manager.h"
#include "ocean/media/PixelImage.h"

#include "ocean/platform/apple/ios/OpenGLFrameMediumViewController.h"
#include "ocean/platform/apple/Resource.h"

#ifdef OCEAN_RUNTIME_STATIC
	#include "ocean/rendering/glescenegraph/apple/Apple.h"
#endif

@interface AppDelegate ()
{
	/// The platform independent wrapper for the detection of QR codes
	Detector2DWrapper detectorWrapper_;

	/// A text label showing the performance of the original implementation of QR code
	UILabel* runtimePerformanceLabel_;

	/// Text labels to display the messages of the detected QR codes
	std::vector<UILabel*> messageLabels_;

	/// The pixel image that will hold the image result from the QR code detection and forward it to the renderer
	Media::PixelImageRef pixelImage_;
}
@end

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
	// we forward all messages to the debug window (of e.g., Xcode)
	Messenger::get().setOutputType(Messenger::OUTPUT_DEBUG_WINDOW);

#ifdef OCEAN_RUNTIME_STATIC

	// for the static runtime we explicitly register the rendering plugin, media plugins will be registered
	Rendering::GLESceneGraph::Apple::registerGLESceneGraphEngine();

#else

	// for the shared runtime the plugin we will load all rendering plugins

	PluginManager::get().collectPlugins(StringApple::toUTF8([[NSBundle mainBundle] resourcePath]));
	PluginManager::get().loadPlugins(PluginManager::TYPE_RENDERING);

#endif

	// as the Storyboard does not define the layout of this application,
	// we simply need to set up the view controller programmatically

	self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
	self.window.backgroundColor = [UIColor whiteColor];

	OpenGLFrameMediumViewController* viewController = [[OpenGLFrameMediumViewController alloc] init];

	self.window.rootViewController = viewController;
	[self.window makeKeyAndVisible];

	runtimePerformanceLabel_ = [[UILabel alloc] initWithFrame:CGRectMake(10, 10, 500, 30)];
	runtimePerformanceLabel_.textColor = [UIColor blackColor];
	runtimePerformanceLabel_.shadowColor = [UIColor whiteColor];
	[viewController.view addSubview:runtimePerformanceLabel_];

	// Add one entry that will show the standard text in case no QR code were detected. Additional elements will be added as needed.
	messageLabels_.resize(1);
	messageLabels_[0] = [[UILabel alloc] initWithFrame:CGRectMake(10, 30, 500, 30)];
	messageLabels_[0].textColor = [UIColor blackColor];
	messageLabels_[0].shadowColor = [UIColor whiteColor];
	[viewController.view addSubview:messageLabels_[0]];

	std::vector<std::wstring> commandLines;
	commandLines.push_back(L"LiveVideoId:0"); // 0. Video source
	commandLines.push_back(L"1280x720"); // 1. Video resolution

	detectorWrapper_ = Detector2DWrapper(commandLines);

	// Now we create a PixelImage that simply wrapps a Frame object
	// as the OpenGLES renderer uses Media objects for textures only
	// we will update this pixel image each time we receive an update from the tracker

	pixelImage_ = Media::Manager::get().newMedium("PixelImageForRenderer", Media::Medium::PIXEL_IMAGE);

	const Media::FrameMediumRef inputFrameMedium = detectorWrapper_.frameMedium();
	if (pixelImage_ && inputFrameMedium)
	{
		pixelImage_->setDevice_T_camera(inputFrameMedium->device_T_camera());
	}

	[viewController setFrameMedium:pixelImage_];

	if (pixelImage_)
	{
		pixelImage_->start();
	}

	// we create a timer that invokes our tracker every 10ms

	[NSTimer scheduledTimerWithTimeInterval:0.01 target:self selector:@selector(timerTicked:) userInfo:nil repeats:YES];

	return YES;
}

- (void)timerTicked:(NSTimer*)timer
{
	if (pixelImage_.isNull())
		return;

	double resultingPerformance = -1.0;

	// we check whether the platform independent tracker has some new image to process

	std::vector<std::string> messages;

	Frame resultingFrame;
	const bool detectedCode = detectorWrapper_.detectAndDecode(resultingFrame, resultingPerformance, messages) && resultingFrame.isValid();

	// **NOTE** copying the resulting RGB frame and forwarding the frame to the renderer costs some performance
	// however, this demo application focuses on the usage of platform independent code and not on performance
	// @see ocean_app_shark for a high performance implementation of an Augmented Realty application (even more powerful)
	if (resultingFrame.isValid())
	{
		pixelImage_->setPixelImage(std::move(resultingFrame));

		runtimePerformanceLabel_.text = StringApple::toNSString(String::toAString(resultingPerformance * 1000.0) + " ms");

		if (messageLabels_.empty() || messageLabels_.size() < messages.size())
		{
			const size_t yOffset = messageLabels_.size();
			const size_t addNewCount = messages.size() - messageLabels_.size();
			for (size_t i = 0; i < addNewCount; ++i)
			{
				UILabel* newMessageLabel = [[UILabel alloc] initWithFrame:CGRectMake(10, 30 + 20 * int(yOffset + i), 500, 30)];
				newMessageLabel.textColor = [UIColor blackColor];
				newMessageLabel.shadowColor = [UIColor whiteColor];

				[self.window.rootViewController.view addSubview:newMessageLabel];
				messageLabels_.push_back(newMessageLabel);
			}
			ocean_assert(messageLabels_.size() >= messages.size());
		}

		for (size_t i = messages.size(); i < messageLabels_.size(); ++i)
		{
			messageLabels_[i].text = StringApple::toNSString(std::string(""));
		}

		if (detectedCode)
		{
			for (size_t i = 0; i < messages.size(); ++i)
			{
				messageLabels_[i].text = StringApple::toNSString(String::toAString(i + 1) + ": " + messages[i]);
			}
		}
		else
		{
			ocean_assert(messageLabels_.empty() == false);
			messageLabels_[0].text = StringApple::toNSString("Point the camera at a QR code");
		}
	}
}

- (void)applicationWillTerminate:(UIApplication *)application
{
	detectorWrapper_.release();
}

@end
