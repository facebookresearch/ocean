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

#include "ocean/cv/FrameTransposer.h"

#include "ocean/media/imageio/ImageIO.h"

#include "ocean/media/imageio/Image.h"

#include "ocean/platform/apple/System.h"

#include "ocean/platform/apple/macos/FrameView.h"

/**
 * A custom view that accepts drag and drop of files.
 */
@interface DragDropView : NSView

/// The app delegate to notify when a file is dropped.
@property (weak) AppDelegate* appDelegate;

@end

@interface AppDelegate ()
{
	/// The view displaying the frames
	Platform::Apple::MacOS::FrameView frameView_;

	/// The input serializer.
	std::unique_ptr<IO::Serialization::FileInputDataSerializer> serializer_;

	/// Timer for updating the view
	NSTimer* updateTimer_;

	/// The current rotation angle in degrees (0, 90, 180, 270)
	int rotationAngle_;

	/// The local event monitor for key events.
	id keyEventMonitor_;

	/// The drag and drop overlay view.
	DragDropView* dragDropView_;

	/// True if the next frame should be saved to disk.
	bool saveNextFrame_;

	/// Counter for saved frames.
	unsigned int savedFrameCounter_;
}

/// The window object.
@property (weak) IBOutlet NSWindow* mainWindow;

/// The view object.
@property (weak) IBOutlet NSView* mainWindowView;

/**
 * Loads and starts playing a serialization file.
 * @param filename The filename of the serialization file to load
 * @return True, if succeeded
 */
- (bool)loadFile:(const std::string&)filename;

/**
 * Stops the serializer.
 */
- (void)stopSerializer;

@end

@implementation DragDropView

- (instancetype)initWithFrame:(NSRect)frameRect
{
	self = [super initWithFrame:frameRect];
	if (self)
	{
		[self registerForDraggedTypes:@[NSPasteboardTypeFileURL]];
	}
	return self;
}

- (NSDragOperation)draggingEntered:(id<NSDraggingInfo>)sender
{
	NSPasteboard* pasteboard = [sender draggingPasteboard];

	if ([[pasteboard types] containsObject:NSPasteboardTypeFileURL])
	{
		return NSDragOperationCopy;
	}

	return NSDragOperationNone;
}

- (BOOL)performDragOperation:(id<NSDraggingInfo>)sender
{
	NSPasteboard* pasteboard = [sender draggingPasteboard];

	if ([[pasteboard types] containsObject:NSPasteboardTypeFileURL])
	{
		NSURL* fileURL = [NSURL URLFromPasteboard:pasteboard];
		if (fileURL != nil && _appDelegate != nil)
		{
			const std::string filename = std::string([[fileURL path] UTF8String]);
			const IO::File file(filename);

			if (file.extension() != "osn")
			{
				Log::warning() << "Dropped file is not an .osn file";
				return NO;
			}

			[_appDelegate stopSerializer];

			return [_appDelegate loadFile:filename] ? YES : NO;
		}
	}

	return NO;
}

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

	// Create the frame view
	frameView_ = Platform::Apple::MacOS::FrameView([_mainWindowView frame]);
	frameView_.nsView().autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;

	[_mainWindowView addSubview:frameView_.nsView()];

	// Create and add the drag and drop view
	dragDropView_ = [[DragDropView alloc] initWithFrame:[_mainWindowView bounds]];
	dragDropView_.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
	dragDropView_.appDelegate = self;
	[_mainWindowView addSubview:dragDropView_];

	// Open file dialog
	NSOpenPanel* openPanel = [NSOpenPanel openPanel];
	[openPanel setTitle:@"Open Serialization File"];
	[openPanel setCanChooseFiles:YES];
	[openPanel setCanChooseDirectories:NO];
	[openPanel setAllowsMultipleSelection:NO];
	[openPanel setAllowedFileTypes:@[@"osn"]];

	NSInteger result = [openPanel runModal];

	if (result == NSModalResponseOK)
	{
		NSURL* fileURL = [[openPanel URLs] firstObject];
		if (fileURL != nil)
		{
			const std::string filename = std::string([[fileURL path] UTF8String]);
			[self loadFile:filename];
		}
	}

	// Set up timer to update frames
	updateTimer_ = [NSTimer scheduledTimerWithTimeInterval:1.0 / 60.0 target:self selector:@selector(updateFrame) userInfo:nil repeats:YES];

	rotationAngle_ = 0;
	saveNextFrame_ = false;
	savedFrameCounter_ = 0u;

	keyEventMonitor_ = [NSEvent addLocalMonitorForEventsMatchingMask:NSEventMaskKeyDown handler:^NSEvent*(NSEvent* event)
	{
		NSString* characters = [event charactersIgnoringModifiers];
		if ([characters length] == 1)
		{
			unichar character = [characters characterAtIndex:0];
			if (character == 'r')
			{
				self->rotationAngle_ = (self->rotationAngle_ + 90) % 360;

				Log::info() << "Rotation angle: " << self->rotationAngle_ << " degrees";
			}
			else if (character == 'l')
			{
				self->rotationAngle_ = (self->rotationAngle_ - 90 + 360) % 360;

				Log::info() << "Rotation angle: " << self->rotationAngle_ << " degrees";
			}
			else if (character == 's')
			{
				self->saveNextFrame_ = true;
			}
		}
		return event;
	}];
}

- (void)updateFrame
{
	if (!serializer_ || !serializer_->isStarted())
	{
		return;
	}

	IO::Serialization::DataSerializer::ChannelId channelId = IO::Serialization::DataSerializer::invalidChannelId();
	IO::Serialization::UniqueDataSample sample = serializer_->sample(channelId, 1.0);

	if (sample)
	{
		IO::Serialization::MediaSerializer::DataSampleFrame* frameSample = dynamic_cast<IO::Serialization::MediaSerializer::DataSampleFrame*>(sample.get());

		if (frameSample != nullptr)
		{
			Frame frame = frameSample->frame();

			if (frame.isValid())
			{
				if (rotationAngle_ != 0)
				{
					CV::FrameTransposer::Comfort::rotate(frame, rotationAngle_);
				}

				if (saveNextFrame_)
				{
					saveNextFrame_ = false;

					const std::string filename = std::string(std::getenv("HOME")) + "/Desktop/frame_" + std::to_string(savedFrameCounter_++) + ".png";

					if (Media::ImageIO::Image::writeImage(frame, filename, true))
					{
						Log::info() << "Saved frame to: " << filename;
					}
					else
					{
						Log::error() << "Failed to save frame to: " << filename;
					}
				}

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
	if (keyEventMonitor_ != nil)
	{
		[NSEvent removeMonitor:keyEventMonitor_];
		keyEventMonitor_ = nil;
	}

	[updateTimer_ invalidate];
	updateTimer_ = nil;

	if (serializer_ && serializer_->isStarted())
	{
		serializer_->stopAndWait();
	}

	serializer_.reset();

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

- (bool)loadFile:(const std::string&)filename
{
	const IO::File file(filename);

	if (!file.exists())
	{
		Log::error() << "The input file does not exist: '" << file() << "'";
		return false;
	}

	Log::info() << "Opening serialization file: '" << file() << "'";

	// Create a fresh serializer instance
	serializer_ = std::make_unique<IO::Serialization::FileInputDataSerializer>();

	if (!serializer_->setFilename(file()))
	{
		Log::error() << "Failed to set the filename";
		return false;
	}

	if (!serializer_->registerSample<IO::Serialization::MediaSerializer::DataSampleFrame>())
	{
		Log::error() << "Failed to register factory function";
		return false;
	}

	IO::Serialization::DataSerializer::Channels channels;
	if (!serializer_->initialize(&channels))
	{
		Log::error() << "Failed to initialize the serializer";
		return false;
	}

	Log::info() << "Found " << channels.size() << " channel(s)";

	for (size_t n = 0; n < channels.size(); ++n)
	{
		const IO::Serialization::DataSerializer::Channel& channel = channels[n];
		Log::info() << "Channel #" << (n + 1) << ": " << channel.name() << " (" << channel.sampleType() << ")";
	}

	if (!serializer_->start())
	{
		Log::error() << "Failed to start the serializer";
		return false;
	}

	return true;
}

- (void)stopSerializer
{
	if (serializer_ && serializer_->isStarted())
	{
		serializer_->stopAndWait();
	}
}

@end
