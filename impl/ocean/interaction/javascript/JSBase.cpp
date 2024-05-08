/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/interaction/javascript/JSBase.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

std::string JSBase::toAString(const v8::Local<v8::String>& value)
{
#if defined(OCEAN_V8_VERSION) && OCEAN_V8_VERSION > 70000
	const v8::String::Utf8Value utf8Value(v8::Isolate::GetCurrent(), value);
#else
	const v8::String::Utf8Value utf8Value(value);
#endif

	if (utf8Value.length() == 0)
	{
		return std::string();
	}

	return std::string(*utf8Value);
}

std::string JSBase::toAString(v8::MaybeLocal<v8::String> value)
{
	if (value.IsEmpty())
	{
		return std::string();
	}

	return toAString(value.ToLocalChecked());
}

}

}

}
