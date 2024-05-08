/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/interaction/empty/Plugin.h"
#include "ocean/interaction/empty/EPYLibrary.h"

bool pluginLoad()
{
	Ocean::Interaction::Empty::EPYLibrary::registerLibrary();

	return true;
}

bool pluginUnload()
{
	return Ocean::Interaction::Empty::EPYLibrary::unregisterLibrary();
}

const char* pluginVersion()
{
	static std::string versionString(std::string("Empty demo version: 0.1"));
	return versionString.c_str();
}
