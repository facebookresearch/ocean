// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/pattern/Pattern.h"
#include "ocean/devices/pattern/PatternFactory.h"

namespace Ocean
{

namespace Devices
{

namespace Pattern
{

std::string namePatternLibrary()
{
	return std::string("Pattern Tracker library");
}

#if defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

void registerPatternLibrary()
{
	PatternFactory::registerFactory();
}

bool unregisterPatternLibrary()
{
	return PatternFactory::unregisterFactory();
}

#endif // OCEAN_RUNTIME_STATIC

}

}

}
