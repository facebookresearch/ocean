/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/media/imagereceiver/osx/AppDelegate.h"

#include "ocean/base/Build.h"
#include "ocean/base/CommandArguments.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/RandomI.h"

#include "ocean/cv/FrameTransposer.h"

#include "ocean/media/Manager.h"
#include "ocean/media/PixelImage.h"

#include "ocean/media/avfoundation/AVFoundation.h"

#include "ocean/platform/apple/System.h"

#include "ocean/platform/apple/macos/FrameMediumView.h"

#include "ocean/network/PackagedTCPClient.h"
#include "ocean/network/Resolver.h"

#include "ocean/io/image/Image.h"

#include "ocean/platform/apple/macos/Utilities.h"

@interface AppDelegate ()
{
	/// The view displaying the medium
	Platform::Apple::MacOS::FrameMediumView frameMediumView_;

	/// The network client for receiving data.
	Network::PackagedTCPClient networkClient_;

	/// The pixel image medium.
	Media::PixelImageRef pixelImage_;

	/// The rotation angle of the image in clockwise direction, in degree.
	int rotationAngle_;
}

/**
 * Event function which is called whenever new data has been received.
 * @param data The received data
 * @param size The size of the received data, in bytes
 */
- (void)onReceive:(const void*)data size:(size_t)size;

/// The window object.
@property (weak) IBOutlet NSWindow* mainWindow;

/// The view object.
@property (weak) IBOutlet NSView* mainWindowView;

@end

static AppDelegate* appDelegate = nullptr;

/**
 * Event function which is called whenever new data has been received.
 * @param data The received data
 * @param size The size of the received data, in bytes
 */
static void onReceive(const void* data, const size_t size)
{
	ocean_assert(appDelegate != nullptr);

	[appDelegate onReceive:data size:size];
}

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification*)aNotification
{
	Platform::Apple::MacOS::Utilities::Commands commandLines = Platform::Apple::MacOS::Utilities::commandArguments();

	CommandArguments commandArguments("An image receiver reciving images over network and displaying the images");
	commandArguments.registerParameter("help", "h", "Showing this help output.");
	commandArguments.registerParameter("address", "a", "The address of the server to connect to (e.g., 192.168.178.10)");
	commandArguments.registerParameter("port", "p", "The port of the server to connect to if other than 6666", Value(6666));
	commandArguments.registerParameter("rotation", "r", "The rotation angle to be applied to the image clockwise, in degree, must be multiple of 90", Value(0));

	commandArguments.parse(commandLines);

	if (commandArguments.hasValue("help"))
	{
		Log::info() << commandArguments.makeSummary();
		exit(0);
	}

	std::string addressString;
	if (!commandArguments.hasValue("address", addressString))
	{
		Log::error() << "No address specified";
		exit(1);
	}

	const Network::Address4 address = Network::Resolver::resolveFirstIp4(addressString);

	if (!address.isValid())
	{
		Log::error() << "Invalid address: " << addressString;
		exit(1);
	}

	int port = 0;
	if (!commandArguments.hasValue("port", port) || port < 0 || port > 65535)
	{
		Log::error() << "Invalid port: " << port;
		exit(1);
	}

	if (!commandArguments.hasValue("rotation", rotationAngle_) || rotationAngle_ % 90 != 0)
	{
		Log::error() << "Invalid rotation angle: " << rotationAngle_;
		exit(1);
	}

	appDelegate = self;

	frameMediumView_ = Platform::Apple::MacOS::FrameMediumView([_mainWindowView frame]);
	frameMediumView_.nsView().autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;

	// we start the medium so that medium will deliver frames

	pixelImage_ = Media::Manager::get().newMedium("PIXEL_IMAGE", Media::Medium::PIXEL_IMAGE);

	pixelImage_->start();

	// and we tell the view which medium to display

	frameMediumView_.setFrameMedium(pixelImage_);

	[_mainWindowView addSubview:frameMediumView_.nsView()];

	networkClient_.setReceiveCallback(Network::PackagedTCPClient::ReceiveCallback::createStatic(&onReceive));

	if (!networkClient_.connect(address, Network::Port(port, Network::Port::TYPE_READABLE)))
	{
		Log::error() << "Failed to connect to " << address.readable() << ":" << port;
		exit(1);
	}
}

- (void)onReceive:(const void*)data size:(size_t)size
{
	Frame frame = IO::Image::decodeImage(data, size);

	if (frame.isValid())
	{
		CV::FrameTransposer::Comfort::rotate(frame, rotationAngle_);

		pixelImage_->setPixelImage(std::move(frame));
	}
}

/**
 * The release event function.
 */
- (void)applicationWillTerminate:(NSNotification*)aNotification
{
	frameMediumView_ = Platform::Apple::MacOS::FrameMediumView();
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
