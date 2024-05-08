/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/android/Android.h"
#include "ocean/devices/android/AndroidFactory.h"

namespace Ocean
{

namespace Devices
{

namespace Android
{

std::string nameAndroidLibrary()
{
	return std::string("Android sensor library");
}

#if defined(OCEAN_RUNTIME_STATIC)

bool registerAndroidLibrary()
{
	return AndroidFactory::registerFactory();
}

bool unregisterAndroidLibrary()
{
	return AndroidFactory::unregisterFactory();
}

#endif // defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

}

}

}
