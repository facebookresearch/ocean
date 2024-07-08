/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/mediafoundation/MediaFoundation.h"
#include "ocean/media/mediafoundation/MFLibrary.h"

namespace Ocean
{

namespace Media
{

namespace MediaFoundation
{

std::string nameMediaFoundationLibrary()
{
	return std::string("MediaFoundation");
}

#ifdef OCEAN_RUNTIME_STATIC

void registerMediaFoundationLibrary()
{
	MFLibrary::registerLibrary();
}

bool unregisterMediaFoundationLibrary()
{
	return MFLibrary::unregisterLibrary();
}

#endif // OCEAN_RUNTIME_STATIC

} // namespace MediaFoundation

} // namespace Media

} // namespace Ocean
