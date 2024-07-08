/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/interaction/UserInterface.h"

namespace Ocean
{

namespace Interaction
{

#if defined(OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY) && defined(__OBJC__)

UserInterface::UserInterface(UIWindow* window, UIViewController* viewController) :
	window_(window),
	viewController_(viewController)
{
	isIOS_ = window_ != nullptr && viewController_ != nullptr;
}

UIWindow* UserInterface::window() const
{
	return window_;
}

UIViewController* UserInterface::viewController() const
{
	return viewController_;
}

#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

}

}
