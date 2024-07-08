/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "AppDelegate.h"

#include "application/ocean/demo/tracking/homographyimagealigner/HomographyImageAligner.h"

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

@interface AppDelegate ()
{
	/// The platform independent wrapper for the aligner.
	HomographyImageAligner homographyImageAligner_;

	/// A text label showing the performance of the aligner.
	UILabel* textLabel_;

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

	OpenGLFrameMediumViewController* viewController = [[OpenGLFrameMediumViewController alloc] init];

	self.window.rootViewController = viewController;
	[self.window makeKeyAndVisible];

	textLabel_ = [[UILabel alloc] initWithFrame:CGRectMake(10, 10, 500, 30)];
	textLabel_.textColor = [UIColor blackColor];
	textLabel_.shadowColor = [UIColor whiteColor];
	[viewController.view addSubview:textLabel_];


	std::vector<std::wstring> commandLines;
	commandLines.emplace_back(L"LiveVideoId:0"); // video source
	commandLines.emplace_back(L"1280x720"); // video resolution
	commandLines.emplace_back(L"150"); // number feature points
	commandLines.emplace_back(L"7"); // path size
	commandLines.emplace_back(L"2"); // sub-pixel iterations/precision
	commandLines.emplace_back(L"128"); // maximal offset between corresponding feature points
	commandLines.emplace_back(L"2"); // search radius on coarsest pyramid layer
	commandLines.emplace_back(L"3"); // RANSAC threshold
	commandLines.emplace_back(L"Y8"); // pixel format to be used
	commandLines.emplace_back(L"nozeromean"); // we not not apply a zero-mean SSD

	homographyImageAligner_ = HomographyImageAligner(commandLines);


	// now we create a PixelImage that simply wrapps a Frame object
	// as the OpenGLES renderer uses Media objects for textures only
	// we will update this pixel image each time we receive an update from the tracker

	pixelImage_ = Media::Manager::get().newMedium("PixelImageForRenderer", Media::Medium::PIXEL_IMAGE);

	if (pixelImage_)
	{
		pixelImage_->setDevice_T_camera(homographyImageAligner_.frameMedium()->device_T_camera());
		pixelImage_->start();

		[viewController setFrameMedium:pixelImage_];
	}


	// we create a timer that invokes our tracker every 10ms

	[NSTimer scheduledTimerWithTimeInterval:0.01 target:self selector:@selector(timerTicked:) userInfo:nil repeats:YES];

	return YES;
}

- (void)timerTicked:(NSTimer*)timer
{
	if (pixelImage_.isNull())
	{
		return;
	}

	Frame resultingAlignerFrame;
	double resultingAlignerPerformance;

	// we check whether the platform independent tracker has some new image to process

	if (homographyImageAligner_.alignNewFrame(resultingAlignerFrame, resultingAlignerPerformance) && resultingAlignerFrame.isValid())
	{
		// we received an augmented frame from the tracker
		// so we forward the result to the render by updating the visual content of the pixel image

		// **NOTE** copying the resulting RGB frame and forwarding the frame to the renderer costs some performance
		// however, this demo application focuses on the usage of platform independent code and not on performance
		// @see ocean_app_shark for a high performance implementation of an Augmented Realty application (even more powerful)

		pixelImage_->setPixelImage(std::move(resultingAlignerFrame));

		textLabel_.text = StringApple::toNSString(String::toAString(resultingAlignerPerformance * 1000.0) + " ms");
	}
}

- (void)applicationWillTerminate:(UIApplication *)application
{
	homographyImageAligner_.release();
}

@end
