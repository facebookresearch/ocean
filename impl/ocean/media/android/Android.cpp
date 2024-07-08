/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/android/Android.h"
#include "ocean/media/android/ALibrary.h"

namespace Ocean
{

namespace Media
{

namespace Android
{

std::string nameAndroidLibrary()
{
	return std::string("Android");
}

#ifdef OCEAN_RUNTIME_STATIC

void registerAndroidLibrary()
{
	ALibrary::registerLibrary();
}

bool unregisterAndroidLibrary()
{
	return ALibrary::unregisterLibrary();
}

#endif // OCEAN_RUNTIME_STATIC

}

}

}
