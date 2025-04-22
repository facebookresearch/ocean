/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/Processor.h"
#include "ocean/base/StringApple.h"

#include <Foundation/Foundation.h>

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE==1
  #include <UIKit/UIKit.h>
#endif

namespace Ocean
{

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE==1

std::string Processor::deviceModelAppleIOS()
{
	return StringApple::toUTF8([[UIDevice currentDevice] model]);
}

#endif

} // namespace Ocean
