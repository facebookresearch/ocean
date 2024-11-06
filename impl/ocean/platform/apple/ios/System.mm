/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/apple/ios/System.h"

#include <UIKit/UIKit.h>

#import <sys/utsname.h>

namespace Ocean
{

namespace Platform
{

namespace Apple
{

namespace IOS
{

bool System::iphoneModelName(std::string& name)
{
  struct utsname systemInfo;

  if (uname(&systemInfo) == 0)
  {
    name = systemInfo.machine;
    return true;
  }

  name.clear();

  return false;
}

bool System::iosVersion(std::string& version)
{
	version = [[[UIDevice currentDevice] systemVersion] UTF8String];
	return true;
}

}

}

}

}
