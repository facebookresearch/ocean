/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/io/serialization/preview/osx/AppDelegate.h"

#include "ocean/base/Build.h"
#include "ocean/base/Messenger.h"
#include "ocean/base/PluginManager.h"

#include "ocean/io/File.h"

#include "ocean/io/serialization/InputDataSerializer.h"
#include "ocean/io/serialization/MediaSerializer.h"

#include "ocean/media/imageio/ImageIO.h"

#include "ocean/platform/apple/System.h"

#include "ocean/platform/apple/macos/FrameView.h"

@interface AppDelegate ()
{
	/// The view displaying the frames
	Platform::Apple::MacOS::FrameView frameView_;

	/// The input serializer.
	IO::Serialization::FileInputDataSerializer serializer_;

	/// Timer for updating the view
	NSTimer* updateTimer_;
}

/// The window object.
@property (weak) IBOutlet NSWindow* mainWindow;

/// The view object.
@property (weak) IBOutlet NSView* mainWindowView;

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification*)aNotification
{
	// Register the ImageIO library for image decoding
#ifdef OCEAN_RUNTIME_SHARED
	const std::string frameworkPath(Platform::Apple::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));

	PluginManager::get().collectPlugins(frameworkPath + std::string("/bin/plugins/") + Build::buildString());
	PluginManager::get().loadPlugins(PluginManager::TYPE_MEDIA);
#else
	Media::ImageIO::registerImageIOLibrary();
#endif

	// Open file dialog
	NSOpenPanel* openPanel = [NSOpenPanel openPanel];
	[openPanel setTitle:@"Open Serialization File"];
	[openPanel setCanChooseFiles:YES];
	[openPanel setCanChooseDirectories:NO];
	[openPanel setAllowsMultipleSelection:NO];
	[openPanel setAllowedFileTypes:@[@"osn"]];

	NSInteger result = [openPanel runModal];

	if (result != NSModalResponseOK)
	{
		return;
	}

	NSURL* fileURL = [[openPanel URLs] firstObject];
	if (fileURL == nil)
	{
		return;
	}

	const std::string filename = std::string([[fileURL path] UTF8String]);
	const IO::File file(filename);

	if (!file.exists())
	{
		Log::error() << "The input file does not exist: '" << file() << "'";
		return;
	}

	Log::info() << "Opening serialization file: '" << file() << "'";

	if (!serializer_.setFilename(file()))
	{
		Log::error() << "Failed to set the filename";
		return;
	}

	if (!serializer_.registerSample<IO::Serialization::MediaSerializer::DataSampleFrame>())
	{
		Log::error() << "Failed to register factory function";
		return;
	}

	IO::Serialization::DataSerializer::Channels channels;
	if (!serializer_.initialize(&channels))
	{
		Log::error() << "Failed to initialize the serializer";
		return;
	}

	Log::info() << "Found " << channels.size() << " channel(s)";

	for (size_t n = 0; n < channels.size(); ++n)
	{
		const IO::Serialization::DataSerializer::Channel& channel = channels[n];
		Log::info() << "Channel #" << (n + 1) << ": " << channel.name() << " (" << channel.sampleType() << ")";
	}

	// Create the frame view
	frameView_ = Platform::Apple::MacOS::FrameView([_mainWindowView frame]);
	frameView_.nsView().autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;

	[_mainWindowView addSubview:frameView_.nsView()];

	// Start the serializer
	if (!serializer_.start())
	{
		Log::error() << "Failed to start the serializer";
		return;
	}

	// Set up timer to update frames
	updateTimer_ = [NSTimer scheduledTimerWithTimeInterval:1.0 / 60.0
		target:self
		selector:@selector(updateFrame)
		userInfo:nil
		repeats:YES];
}

- (void)updateFrame
{
	if (!serializer_.isStarted())
	{
		return;
	}

	IO::Serialization::DataSerializer::ChannelId channelId = IO::Serialization::DataSerializer::invalidChannelId();
	IO::Serialization::UniqueDataSample sample = serializer_.sample(channelId, 1.0);

	if (sample)
	{
		IO::Serialization::MediaSerializer::DataSampleFrame* frameSample = dynamic_cast<IO::Serialization::MediaSerializer::DataSampleFrame*>(sample.get());

		if (frameSample != nullptr)
		{
			Frame frame = frameSample->frame();

			if (frame.isValid())
			{
				frameView_.setFrame(frame);
			}
		}
	}
}

/**
 * The release event function.
 */
- (void)applicationWillTerminate:(NSNotification*)aNotification
{
	[updateTimer_ invalidate];
	updateTimer_ = nil;

	if (serializer_.isStarted())
	{
		serializer_.stopAndWait();
	}

	frameView_ = Platform::Apple::MacOS::FrameView();

#ifdef OCEAN_RUNTIME_SHARED
	PluginManager::get().release();
#else
	Media::ImageIO::unregisterImageIOLibrary();
#endif
}

/**
 * Checks whether the application should terminate when the last windows is closed.
 * @return True, if so
 */
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender
{
	return YES;
}

@end
