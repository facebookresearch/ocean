/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/Utilities.h"


#include "ocean/base/CommandArguments.h"

namespace Ocean
{

namespace Platform
{

Utilities::Commands Utilities::parseCommandLine(const wchar_t* commandLine)
{
	return CommandArguments::separateArguments(commandLine);
}

void Utilities::showMessageBox(const std::string& title, const std::string& message)
{
#if defined(_WINDOWS)

	MessageBoxA(nullptr, message.c_str(), title.c_str(), 0);

#elif defined(__APPLE__)

	showMessageBoxApple(title, message);

#else

	// missing implementation e.g., for Android platforms
	Log::info() << "MessageBox: " << title << ", " << message;

#endif
}

}

}
