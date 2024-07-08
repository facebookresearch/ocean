/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/wic/WIC.h"
#include "ocean/media/wic/WICLibrary.h"

namespace Ocean
{

namespace Media
{

namespace WIC
{

std::string nameWICLibrary()
{
	return std::string("WIC");
}

#if defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

void registerWICLibrary()
{
	WICLibrary::registerLibrary();
}

bool unregisterWICLibrary()
{
	return WICLibrary::unregisterLibrary();
}

#endif // OCEAN_RUNTIME_STATIC

} // namespace WIC

} // namespace Media

} // namespace Ocean
