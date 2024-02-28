// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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
