// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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
