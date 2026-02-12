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
#include "ocean/network/PackagedTCPServer.h"
#include "ocean/network/Resolver.h"

#include "ocean/io/image/Image.h"

#include "ocean/platform/apple/macos/Utilities.h"

@interface AppDelegate ()
{
	/// The view displaying the medium
	Platform::Apple::MacOS::FrameMediumView frameMediumView_;

	/// The network client for receiving data (used in client mode).
	std::unique_ptr<Network::PackagedTCPClient> networkClient_;

	/// The network server for receiving data (used in server mode).
	std::unique_ptr<Network::PackagedTCPServer> networkServer_;

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
 * Event function which is called whenever new data has been received (client mode).
 * @param data The received data
 * @param size The size of the received data, in bytes
 */
static void onReceive(const void* data, const size_t size)
{
	ocean_assert(appDelegate != nullptr);

	[appDelegate onReceive:data size:size];
}

/**
 * Event function which is called whenever new data has been received (server mode).
 * @param connectionId The connection ID
 * @param data The received data
 * @param size The size of the received data, in bytes
 */
static void onReceiveServer(const Network::PackagedTCPServer::ConnectionId /*connectionId*/, const void* data, const size_t size)
{
	ocean_assert(appDelegate != nullptr);

	[appDelegate onReceive:data size:size];
}

/**
 * Event function which is called when a connection request is received (server mode).
 * @param address The address of the client
 * @param port The port of the client
 * @param connectionId The connection ID
 * @return True to accept the connection
 */
static bool onConnection(const Network::Address4& /*address*/, const Network::Port& /*port*/, const Network::PackagedTCPServer::ConnectionId /*connectionId*/)
{
	return true;
}

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification*)aNotification
{
	Platform::Apple::MacOS::Utilities::Commands commandLines = Platform::Apple::MacOS::Utilities::commandArguments();

	CommandArguments commandArguments("An image receiver reciving images over network and displaying the images");
	commandArguments.registerParameter("help", "h", "Showing this help output.");
	commandArguments.registerParameter("server", "s", "When specified, the application acts as a server waiting for connections. Otherwise, it acts as a client.");
	commandArguments.registerParameter("address", "a", "The address of the server to connect to (e.g., 192.168.178.10) - only used in client mode");
	commandArguments.registerParameter("port", "p", "The port of the server to connect to (client mode) or listen on (server mode)", Value(6666));
	commandArguments.registerParameter("rotation", "r", "The rotation angle to be applied to the image clockwise, in degree, must be multiple of 90", Value(0));

	commandArguments.parse(commandLines);

	if (commandArguments.hasValue("help"))
	{
		Log::info() << commandArguments.makeSummary();
		[[NSApplication sharedApplication] terminate:self];
	}

	const bool isServerMode = commandArguments.hasValue("server");

	int port = 0;
	if (!commandArguments.hasValue("port", port) || port < 0 || port > 65535)
	{
		Log::error() << "Invalid port: " << port;
		[[NSApplication sharedApplication] terminate:self];
	}

	if (!commandArguments.hasValue("rotation", rotationAngle_) || rotationAngle_ % 90 != 0)
	{
		Log::error() << "Invalid rotation angle: " << rotationAngle_;
		[[NSApplication sharedApplication] terminate:self];
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

	if (isServerMode)
	{
		Log::info() << "Starting in server mode on port " << port;

		networkServer_ = std::make_unique<Network::PackagedTCPServer>();

		networkServer_->setConnectionRequestCallback(Network::PackagedTCPServer::ConnectionRequestCallback::createStatic(&onConnection));
		networkServer_->setReceiveCallback(Network::PackagedTCPServer::ReceiveCallback::createStatic(&onReceiveServer));

		networkServer_->setPort(Network::Port(port, Network::Port::TYPE_READABLE));

		if (!networkServer_->start())
		{
			Log::error() << "Failed to start server on port " << port;
			[[NSApplication sharedApplication] terminate:self];
		}

		const Network::Resolver::Addresses4 addresses = Network::Resolver::get().localAddresses();

		if (!addresses.empty())
		{
			Log::info() << "Server listening on:";
			for (const Network::Address4& localAddress : addresses)
			{
				Log::info() << "  " << localAddress.readable() << ":" << port;
			}
		}
	}
	else
	{
		std::string addressString;
		if (!commandArguments.hasValue("address", addressString))
		{
			Log::error() << "No address specified (required in client mode)";
			[[NSApplication sharedApplication] terminate:self];
		}

		const Network::Address4 address = Network::Resolver::resolveFirstIp4(addressString);

		if (!address.isValid())
		{
			Log::error() << "Invalid address: " << addressString;
			[[NSApplication sharedApplication] terminate:self];
		}

		Log::info() << "Starting in client mode, connecting to " << address.readable() << ":" << port;

		networkClient_ = std::make_unique<Network::PackagedTCPClient>();
		networkClient_->setReceiveCallback(Network::PackagedTCPClient::ReceiveCallback::createStatic(&onReceive));

		if (!networkClient_->connect(address, Network::Port(port, Network::Port::TYPE_READABLE)))
		{
			Log::error() << "Failed to connect to " << address.readable() << ":" << port;
			[[NSApplication sharedApplication] terminate:self];
		}

		Log::info() << "Connected successfully";
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
	networkClient_ = nullptr;
	networkServer_ = nullptr;

	pixelImage_.release();

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
