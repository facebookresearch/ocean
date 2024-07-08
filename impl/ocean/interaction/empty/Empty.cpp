/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/interaction/empty/Empty.h"
#include "ocean/interaction/empty/EPYLibrary.h"

namespace Ocean
{

namespace Interaction
{

namespace Empty
{

#ifndef _DLL

void registerEmptyLibrary()
{
	EPYLibrary::registerLibrary();
}

bool unregisterEmptyLibrary()
{
	return EPYLibrary::unregisterLibrary();
}

#endif // _DLL

} // namespace Empty

} // namespace Interaction

} // namespace Ocean
