/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/interaction/javascript/JavaScript.h"
#include "ocean/interaction/javascript/JSLibrary.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

std::string nameJavaScriptLibrary()
{
	return std::string("JavaScript interaction library");
}

#ifdef OCEAN_RUNTIME_STATIC

void registerJavaScriptLibrary()
{
	JSLibrary::registerLibrary();
}

bool unregisterJavaScriptLibrary()
{
	return JSLibrary::unregisterLibrary();
}

#endif // OCEAN_RUNTIME_STATIC

} // namespace JavaScript

} // namespace Interaction

} // namespace Ocean
