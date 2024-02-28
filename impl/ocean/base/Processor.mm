// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/base/Processor.h"
#include "ocean/base/StringApple.h"

#include <Foundation/Foundation.h>

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE==1
  #include <UIKit/UIKit.h>
#endif

namespace Ocean
{

unsigned int Processor::realCoresApple()
{
	const NSInteger cores = [[NSProcessInfo processInfo] processorCount];

	static_assert(sizeof(NSInteger) >= sizeof(unsigned int), "Invalid data type!");
	ocean_assert(cores > 0 && cores < (NSInteger)std::numeric_limits<unsigned int>::max());

	return (unsigned int)cores;
}

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE==1

std::string Processor::deviceModelAppleIOS()
{
	return StringApple::toUTF8([[UIDevice currentDevice] model]);
}

#endif

} // namespace Ocean
