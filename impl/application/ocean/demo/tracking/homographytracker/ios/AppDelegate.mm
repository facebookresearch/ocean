/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "AppDelegate.h"

#include "application/ocean/demo/tracking/homographytracker/HomographyTrackerWrapper.h"

#include "ocean/base/Messenger.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/String.h"
#include "ocean/base/StringApple.h"

#include "ocean/media/Manager.h"
#include "ocean/media/PixelImage.h"

#include "ocean/platform/apple/ios/OpenGLFrameMediumViewController.h"

#ifdef OCEAN_RUNTIME_STATIC
	#include "ocean/rendering/glescenegraph/apple/Apple.h"
#endif

/**
 * The ViewController adding support for user interaction.
 */
@interface CustomViewController : OpenGLFrameMediumViewController
{
	@public

		/// Position of the most recent user interaction.
		Vector2 recentTouchPosition_;
}
@end

@implementation CustomViewController

- (id)init
{
	if ([super init])
	{
		recentTouchPosition_ = Vector2(Numeric::minValue(), Numeric::maxValue());
	}

	return self;
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	if ([touches count] != 1)
	{
		return;
	}

	const CGPoint viewPoint = [[touches anyObject] locationInView:[self view]];
	const CGPoint mediumPoint = [self view2medium:viewPoint];

	recentTouchPosition_ = Vector2(mediumPoint.x, mediumPoint.y);
}

@end

@interface AppDelegate ()
{
	/// The platform independent wrapper for the tracker.
	HomographyTrackerWrapper homographyTrackerWrapper_;

	/// The custom view controller of this application.
	CustomViewController* viewController_;

	/// A text label showing the performance of the aligner.
	UILabel* textLabelPerformance_;

	/// The pixel image that will forward the image result from the feature tracker to the renderer.
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

	viewController_ = [[CustomViewController alloc] init];

	self.window.rootViewController = viewController_;
	[self.window makeKeyAndVisible];

	textLabelPerformance_ = [[UILabel alloc] initWithFrame:CGRectMake(10, 10, 800, 20)];
	textLabelPerformance_.textColor = [UIColor blackColor];
	textLabelPerformance_.shadowColor = [UIColor whiteColor];
	[viewController_.view addSubview:textLabelPerformance_];

	std::vector<std::wstring> commandLines;
	commandLines.push_back(L"LiveVideoId:0"); // video source
	commandLines.push_back(L"1280x720"); // video resolution

	homographyTrackerWrapper_ = HomographyTrackerWrapper(commandLines);

	// now we create a PixelImage that simply wrapps a Frame object
	// as the OpenGLES renderer uses Media objects for textures only
	// we will update this pixel image each time we receive an update from the tracker

	pixelImage_ = Media::Manager::get().newMedium("PixelImageForRenderer", Media::Medium::PIXEL_IMAGE);

	if (pixelImage_)
	{
		pixelImage_->setDevice_T_camera(homographyTrackerWrapper_.frameMedium()->device_T_camera());
		pixelImage_->start();

		[viewController_ setFrameMedium:pixelImage_];
	}

	// we create a timer that invokes our tracker every 10ms

	[NSTimer scheduledTimerWithTimeInterval:0.01 target:self selector:@selector(timerTicked:) userInfo:nil repeats:YES];

	return YES;
}

- (void)timerTicked:(NSTimer*)timer
{
	if (pixelImage_.isNull())
		return;

	Frame resultingFrame;
	double resultingPerformance;

	const Vector2 recentTouchPosition = viewController_->recentTouchPosition_;

	// we check whether the platform independent tracker has some new image to process

	if (homographyTrackerWrapper_.trackNewFrame(resultingFrame, resultingPerformance, recentTouchPosition, nullptr) && resultingFrame.isValid())
	{
		// we received an augmented frame from the tracker
		// so we forward the result to the render by updating the visual content of the pixel image

		// **NOTE** copying the resulting RGB frame and forwarding the frame to the renderer costs some performance
		// however, this demo application focuses on the usage of platform independent code and not on performance
		// @see ocean_app_shark for a high performance implementation of an Augmented Realty application (even more powerful)

		pixelImage_->setPixelImage(std::move(resultingFrame));

		if (resultingPerformance >= 0.0)
		{
			textLabelPerformance_.text = StringApple::toNSString(String::toAString(resultingPerformance * 1000.0) + " ms");
		}
		else
		{
			textLabelPerformance_.text = StringApple::toNSString("Select a region to track.");
		}

		viewController_->recentTouchPosition_ = Vector2(Numeric::minValue(), Numeric::minValue());
	}
}

- (void)applicationWillTerminate:(UIApplication *)application
{
	homographyTrackerWrapper_.release();
}

@end
