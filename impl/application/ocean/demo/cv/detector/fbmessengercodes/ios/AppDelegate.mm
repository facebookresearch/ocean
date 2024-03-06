// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "AppDelegate.h"

#include "FBMessengerCodesWrapper.h"

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
	/// The platform independent wrapper for the detection of FB Messenger codes
	FBMessengerCodesWrapper applicationFBMessengerCodesWrapper;

	/// A text label showing the performance of the original implementation of FB Messenger codes
	UILabel* runtimePerformanceLabel;

	/// A text label to display the decoded FB Messenger code
	UILabel* decodedFBMessengerCodeLabel;

	/// The pixel image that will hold the image result from the FB Messenger code detection and forward it to the renderer
	Media::PixelImageRef applicationPixelImage;
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

	runtimePerformanceLabel = [[UILabel alloc] initWithFrame:CGRectMake(10, 10, 500, 30)];
	runtimePerformanceLabel.textColor = [UIColor blackColor];
	runtimePerformanceLabel.shadowColor = [UIColor whiteColor];
	[viewController.view addSubview:runtimePerformanceLabel];

	decodedFBMessengerCodeLabel = [[UILabel alloc] initWithFrame:CGRectMake(10, 30, 500, 30)];
	decodedFBMessengerCodeLabel.textColor = [UIColor blackColor];
	decodedFBMessengerCodeLabel.shadowColor = [UIColor whiteColor];
	decodedFBMessengerCodeLabel.lineBreakMode = NSLineBreakByWordWrapping;
	decodedFBMessengerCodeLabel.numberOfLines = 0;
	[viewController.view addSubview:decodedFBMessengerCodeLabel];

	std::vector<std::wstring> commandLines;
	commandLines.push_back(L"LiveVideoId:0"); // 0. Video source
	commandLines.push_back(L"1280x720"); // 1. Video resolution
	commandLines.push_back(L"Y8"); // 2. Pixel format to be used

	applicationFBMessengerCodesWrapper = FBMessengerCodesWrapper(commandLines);


	// Now we create a PixelImage that simply wrapps a Frame object
	// as the OpenGLES renderer uses Media objects for textures only
	// we will update this pixel image each time we receive an update from the tracker

	applicationPixelImage = Media::Manager::get().newMedium("PixelImageForRenderer", Media::Medium::PIXEL_IMAGE);
	[viewController setFrameMedium:applicationPixelImage];

	if (applicationPixelImage)
	{
		applicationPixelImage->setDevice_T_camera(applicationFBMessengerCodesWrapper.frameMedium()->device_T_camera());
		applicationPixelImage->start();

		[viewController setFrameMedium:applicationPixelImage];
	}


	// we create a timer that invokes our tracker every 10ms

	[NSTimer scheduledTimerWithTimeInterval:0.01 target:self selector:@selector(timerTicked:) userInfo:nil repeats:YES];

	return YES;
}

- (void)timerTicked:(NSTimer*)timer
{
	if (applicationPixelImage.isNull())
		return;

	double resultingPerformance = -1.0;

	// we check whether the platform independent tracker has some new image to process

	Frame resultingFrame;
	std::vector<std::string> messages;
	const bool detectedMessengerCode = applicationFBMessengerCodesWrapper.detectAndDecode(resultingFrame, resultingPerformance, messages) && resultingFrame.isValid();

	// **NOTE** copying the resulting RGB frame and forwarding the frame to the renderer costs some performance
	// however, this demo application focuses on the usage of platform independent code and not on performance
	// @see ocean_app_shark for a high performance implementation of an Augmented Realty application (even more powerful)
	if (resultingFrame.isValid())
	{
		applicationPixelImage->setPixelImage(resultingFrame);

		runtimePerformanceLabel.text = StringApple::toNSString(String::toAString(resultingPerformance * 1000.0) + " ms");

		if (detectedMessengerCode)
		{
			std::ostringstream oss;

			for (size_t i = 0; i < messages.size(); ++i)
			{
				oss << i + 1 << ": " << messages[i].substr(0, 15) << "..." << (i + 1 < messages.size() ? " \n " : "");
			}

			Log::info() << oss.str();
			decodedFBMessengerCodeLabel.text = StringApple::toNSString(oss.str());
		}
		else
		{
			decodedFBMessengerCodeLabel.text = StringApple::toNSString("---");
		}
	}
}

- (void)applicationWillTerminate:(UIApplication *)application
{
	applicationFBMessengerCodesWrapper.release();
}

@end
