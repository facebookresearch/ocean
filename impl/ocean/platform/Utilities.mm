// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/platform/Utilities.h"

#ifndef __APPLE__
	#error This files contains Apple specific functions only, see Utilities.cpp
#endif

#include "ocean/base/StringApple.h"

#include <Foundation/Foundation.h>

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE==1
  #include <UIKit/UIKit.h>
#else
  #include <CoreFoundation/CoreFoundation.h>
#endif

namespace Ocean
{

namespace Platform
{

void Utilities::showMessageBoxApple(const std::string& title, const std::string& message)
{
#if !defined(TARGET_OS_IPHONE) || TARGET_OS_IPHONE == 0

	CFOptionFlags result;
	CFUserNotificationDisplayAlert(0, kCFUserNotificationNoteAlertLevel, NULL, NULL, NULL, CFStringCreateWithCString(NULL, title.c_str(), kCFStringEncodingUTF8), CFStringCreateWithCString(NULL, message.c_str(), kCFStringEncodingUTF8), NULL, NULL, NULL, &result);

#else

	UIAlertController* alertController = [UIAlertController alertControllerWithTitle:StringApple::toNSString(title) message:StringApple::toNSString(message) preferredStyle:UIAlertControllerStyleAlert];
	UIAlertAction* defaultAction = [UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:^(UIAlertAction * action) {}];

	[alertController addAction:defaultAction];
	[[[[UIApplication sharedApplication] keyWindow] rootViewController]  presentViewController:alertController animated:YES completion:nil];

#endif
}

}

}
