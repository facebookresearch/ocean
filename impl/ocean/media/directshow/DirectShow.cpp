/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/directshow/DirectShow.h"
#include "ocean/media/directshow/DSLibrary.h"

namespace Ocean
{

namespace Media
{

namespace DirectShow
{

std::string nameDirectShowLibrary()
{
	return std::string("DirectShow");
}

#ifdef OCEAN_RUNTIME_STATIC

void registerDirectShowLibrary()
{
	DSLibrary::registerLibrary();
}

bool unregisterDirectShowLibrary()
{
	return DSLibrary::unregisterLibrary();
}

#endif // OCEAN_RUNTIME_STATIC

} // namespace DirectShow

} // namespace Media

} // namespace Ocean
