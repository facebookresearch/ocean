/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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

#if defined(OCEAN_RUNTIME_STATIC)

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
