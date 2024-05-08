/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
	CFUserNotificationDisplayAlert(0, kCFUserNotificationNoteAlertLevel, nullptr, nullptr, nullptr, CFStringCreateWithCString(nullptr, title.c_str(), kCFStringEncodingUTF8), CFStringCreateWithCString(nullptr, message.c_str(), kCFStringEncodingUTF8), nullptr, nullptr, nullptr, &result);

#else

	UIAlertController* alertController = [UIAlertController alertControllerWithTitle:StringApple::toNSString(title) message:StringApple::toNSString(message) preferredStyle:UIAlertControllerStyleAlert];
	UIAlertAction* defaultAction = [UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:^(UIAlertAction * action) {}];

	[alertController addAction:defaultAction];
	[[[[UIApplication sharedApplication] keyWindow] rootViewController]  presentViewController:alertController animated:YES completion:nil];

#endif
}

}

}
