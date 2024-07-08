/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/interaction/javascript/Plugin.h"
#include "ocean/interaction/javascript/JSLibrary.h"

#include <v8.h>

#if defined(OCEAN_RUNTIME_SHARED)

bool pluginLoad()
{
	Ocean::Interaction::JavaScript::JSLibrary::registerLibrary();

	return true;
}

bool pluginUnload()
{
	return Ocean::Interaction::JavaScript::JSLibrary::unregisterLibrary();
}

const char* pluginVersion()
{
	static std::string versionString(std::string("V8 version: ") + std::string(v8::V8::GetVersion()));
	return versionString.c_str();
}

#endif // #if defined(OCEAN_RUNTIME_SHARED)
