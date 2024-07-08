/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/apple/Utilities.h"

#include "ocean/base/StringApple.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"

#include <Foundation/Foundation.h>

#ifdef OCEAN_PLATFORM_BUILD_APPLE_MACOS
	#include <AppKit/AppKit.h>
#else
	#include <UIKit/UIKit.h>
#endif

namespace Ocean
{

namespace Platform
{

namespace Apple
{

ScopedCFStringRef Utilities::toCFString(const std::string& value)
{
	return ScopedCFStringRef(CFStringCreateWithCString(nullptr, value.c_str(), kCFStringEncodingUTF8));
}

ScopedCFStringRef Utilities::toCFString(const std::wstring& value)
{
	static_assert(sizeof(wchar_t) == 4, "Invalid data type!");
	return ScopedCFStringRef(CFStringCreateWithBytes(nullptr, (unsigned char*)(value.c_str()), value.length() * sizeof(wchar_t), kCFStringEncodingUTF32LE, false));
}

ScopedCGImageRef Utilities::toCGImage(const Frame& frame, bool copyData)
{
	if (!frame.isValid())
	{
		return ScopedCGImageRef();
	}

	constexpr unsigned int numberPixelFormats = 2u;

	FrameType::PixelFormat pixelFormats[numberPixelFormats];

	if (FrameType::formatHasAlphaChannel(frame.pixelFormat()))
	{
		pixelFormats[0] = FrameType::FORMAT_RGBA32;
		pixelFormats[1] = FrameType::FORMAT_YA16;
	}
	else
	{
		pixelFormats[0] = FrameType::FORMAT_RGB24;
		pixelFormats[1] = FrameType::FORMAT_Y8;
	};

	const bool makeCopy = copyData || !frame.isContinuous();

	for (unsigned int n = 0u; n < numberPixelFormats; ++n)
	{
		const FrameType::PixelFormat& pixelFormat = pixelFormats[n];

		Frame convertedFrame;
		if (CV::FrameConverter::Comfort::convert(frame, pixelFormat, FrameType::ORIGIN_UPPER_LEFT, convertedFrame, makeCopy, WorkerPool::get().conditionalScopedWorker(frame.pixels() < 400 * 400)()))
		{
			ocean_assert(convertedFrame.isContinuous());
			ocean_assert(convertedFrame.numberPlanes() == 1u);
			ocean_assert(FrameType::formatIsGeneric(convertedFrame.pixelFormat()));

			const bool forceCopy = makeCopy || convertedFrame.constdata<void>() != frame.constdata<void>(); // we need a force a copy if the frame has been converted

			const ScopedCFDataRef data = forceCopy ? ScopedCFDataRef(CFDataCreate(nil, convertedFrame.constdata<uint8_t>(), convertedFrame.size())) : ScopedCFDataRef(CFDataCreateWithBytesNoCopy(nil, convertedFrame.constdata<uint8_t>(), convertedFrame.size(), kCFAllocatorNull));
			const ScopedCGDataProviderRef dataProvider(CGDataProviderCreateWithCFData(data.object()));

			const size_t bitsPerComponent = convertedFrame.bytesPerDataType() * 8u;
			const size_t bitsPerPixel = bitsPerComponent * convertedFrame.channels();
			const size_t bytesPerRow = convertedFrame.planeWidthBytes(0u);

			ScopedCGColorSpaceRef colorSpace(CGColorSpaceCreateDeviceRGB());
			CGBitmapInfo bitmapInfo = kCGBitmapByteOrderDefault;

			switch (convertedFrame.pixelFormat())
			{
				case FrameType::FORMAT_RGBA32:
					colorSpace = ScopedCGColorSpaceRef(CGColorSpaceCreateDeviceRGB());
					bitmapInfo = CGBitmapInfo(uint32_t(kCGBitmapByteOrder32Big) | uint32_t(kCGImageAlphaLast));
					break;

				case FrameType::FORMAT_RGB24:
					colorSpace = ScopedCGColorSpaceRef(CGColorSpaceCreateDeviceRGB());
					bitmapInfo = kCGBitmapByteOrderDefault;
					break;

				case FrameType::FORMAT_YA16:
					colorSpace = ScopedCGColorSpaceRef(CGColorSpaceCreateDeviceGray());
					bitmapInfo = CGBitmapInfo(uint32_t(kCGBitmapByteOrder16Big) | uint32_t(kCGImageAlphaLast));
					break;

				case FrameType::FORMAT_Y8:
					colorSpace = ScopedCGColorSpaceRef(CGColorSpaceCreateDeviceGray());
					bitmapInfo = kCGBitmapByteOrderDefault;
					break;

				default:
					ocean_assert(false && "Invalid pixel format!");
			}

			return ScopedCGImageRef(CGImageCreate(convertedFrame.width(), convertedFrame.height(), bitsPerComponent, bitsPerPixel, bytesPerRow, colorSpace.object(), bitmapInfo, dataProvider.object(), nullptr, false, kCGRenderingIntentDefault));
		}
	}

	ocean_assert(false && "Not supported frame type!");

	return ScopedCGImageRef();
}

bool Utilities::asNSImageOrUIImage(const Frame& frame)
{
	const ScopedCGImageRef cgImageRef(toCGImage(frame));

	if (cgImageRef.object() != nullptr)
	{

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1
		UIImage* image = [[UIImage alloc] initWithCGImage:cgImageRef.object()];
#else
		const unsigned int width = (unsigned int)(CGImageGetWidth(cgImageRef.object()));
		const unsigned int height = (unsigned int)(CGImageGetHeight(cgImageRef.object()));

		NSImage* image = [[NSImage alloc] initWithCGImage:cgImageRef.object() size:NSMakeSize(width, height)];
#endif

		OCEAN_SUPPRESS_UNUSED_WARNING(image);
		// place your break point here to use Xcodes `Quick look` on 'image'

		return true;
	}

	return false;
}

void Utilities::showMessageBox(const std::string& title, const std::string& message)
{
#if !defined(TARGET_OS_IPHONE) || TARGET_OS_IPHONE == 0

	CFOptionFlags result;
	CFUserNotificationDisplayAlert(0, kCFUserNotificationNoteAlertLevel, nullptr, nullptr, nullptr, CFStringCreateWithCString(nullptr, title.c_str(), kCFStringEncodingUTF8), CFStringCreateWithCString(nullptr, message.c_str(), kCFStringEncodingUTF8), nullptr, nullptr, nullptr, &result);

#else

	UIAlertController* alertController = [UIAlertController alertControllerWithTitle:StringApple::toNSString(title) message:StringApple::toNSString(message) preferredStyle:UIAlertControllerStyleAlert];
	UIAlertAction* defaultAction = [UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:^(UIAlertAction * action) {}];

	[alertController addAction:defaultAction];
	[[[[UIApplication sharedApplication] keyWindow] rootViewController]  presentViewController:alertController animated:YES completion:nil];

#endif
}

std::string Utilities::documentDirectory()
{
	NSURL* directoryURL = [[[NSFileManager defaultManager] URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask] lastObject];
	NSString* urlString = [directoryURL absoluteString];

	const std::string cString(StringApple::toUTF8(urlString));
	ocean_assert(cString.find("file://") == 0);

	return cString.substr(7);
}

bool Utilities::hasFixedSupportedInterfaceOrientation(int& orientation)
{
#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

	UIApplication* application = [UIApplication sharedApplication];

	const UIInterfaceOrientationMask orientationMask = [application supportedInterfaceOrientationsForWindow:application.keyWindow];

	switch (orientationMask)
	{
		case UIInterfaceOrientationMaskPortrait:
			orientation = 0;
			return true;

		case UIInterfaceOrientationMaskLandscapeLeft:
			orientation = 90;
			return true;

		case UIInterfaceOrientationMaskLandscapeRight:
			orientation = -90;
			return true;

		case UIInterfaceOrientationMaskPortraitUpsideDown:
			orientation = 180;
			return true;

		default:
			break;
	}

#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

	return false;
}

bool Utilities::bundleVersion(std::string& bundleShortVersion, std::string* bundleVersionKey)
{
	NSBundle* mainBundle = [NSBundle mainBundle];

	if (mainBundle == nullptr)
	{
		return false;
	}

	NSString* bundleShortVersionString = [[mainBundle infoDictionary] objectForKey:@"CFBundleShortVersionString"];

	if (bundleShortVersionString == nullptr)
	{
		return false;
	}

	bundleShortVersion = StringApple::toUTF8(bundleShortVersionString);

	if (bundleVersionKey != nullptr)
	{
		NSString* bundleVersionKeyString = [[mainBundle infoDictionary] objectForKey:(NSString*)(kCFBundleVersionKey)];

		if (bundleVersionKeyString != nullptr)
		{
			*bundleVersionKey = StringApple::toUTF8(bundleVersionKeyString);
		}
		else
		{
			bundleVersionKey->clear();
		}
	}

	return true;
}

}

}

}
