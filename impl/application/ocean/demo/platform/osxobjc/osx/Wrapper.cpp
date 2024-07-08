/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/platform/osxobjc/osx/Wrapper.h"

#include "ocean/base/Build.h"
#include "ocean/base/Timestamp.h"

using namespace Ocean;

const char* string2buffer(const std::string& value)
{
	char* result = (char*)malloc(value.length() + 1);

	ocean_assert(result != nullptr);

	memcpy(result, value.c_str(), value.length());
	result[value.length()] = '\0';

	return result;
}

const char* oceanBaseBuildStringStatic()
{
	static const std::string value(Build::buildString());

	return value.c_str();
}

const char* oceanBaseDateTimeString(const char* selection)
{
	ocean_assert(selection);

	if (selection == nullptr)
	{
		return string2buffer("invalid selection");
	}

	if (std::string(selection) == std::string("date"))
	{
		return string2buffer(Build::buildDate(__DATE__));
	}

	if (std::string(selection) == std::string("time"))
	{
		return string2buffer(Build::buildTime(__TIME__));
	}

	return string2buffer("invalid seleciton");
}

double oceanBaseTimestamp()
{
	return double(Timestamp(true));
}

Struct oceanBaseStructObject()
{
	const Struct value = {42, 42.1f};
	return value;
}

Struct* oceanBaseStructStatic()
{
	static Struct value = {43, 43.2f};
	return &value;
}

void oceanReleaseString(const char* value)
{
	ocean_assert(value);
	free((void*)value);
}
