/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/Base.h"
#include "ocean/base/Messenger.h"

namespace Ocean
{

#if defined(OCEAN_DEBUG) && defined(OCEAN_REDIRECT_ASSERT_TO_MESSENGER)

void assertErrorMessage(const char* file, const int line, const char* message)
{
	Log::error() << "Assert file: " << file << ", file: " << line << ", \"" << message << "\"";
}

#endif

}
