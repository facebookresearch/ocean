/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/Messenger.h"

#include <Foundation/Foundation.h>

#ifndef __APPLE__
    #error This files contains Apple specific functions only, see Messenger.cpp
#endif

namespace Ocean
{

void Messenger::writeMessageToDebugWindowApple(const std::string& message)
{
	const std::string text(std::string("Ocean, ") + message);
	NSLog(@"%@", [NSString stringWithCString:text.c_str() encoding:NSASCIIStringEncoding]);
}

} // namespace Ocean
