// Copyright (c) Meta Platforms, Inc. and affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_UVTEXTUREMAPPING_IOS_APP_DELEGATE_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_UVTEXTUREMAPPING_IOS_APP_DELEGATE_H

#include "ocean/base/Base.h"

#include <UIKit/UIKit.h>

/**
 * @ingroup applicationdemotrackinguvtexturemapping
 * @defgroup applicationdemotrackinguvtexturemappingios UV Texture Mapping (iOS)
 * @{
 * TODO jtprice Add a description of this demo here
 * This demo application uses the platform independent UVTextureMappingWrapper class implementing most of the necessary code.
 * However, this application here is intended for Apple iOS platforms only.
 * @see UVTextureMappingWrapper, applicationdemotrackinguvtexturemappingosx
 * @}
 */

using namespace Ocean;

/**
 * The Application Delegate of the UV texture mapping demo application.
 * @ingroup applicationdemotrackinguvtexturemappingios
 */
@interface AppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;

@end

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_UVTEXTUREMAPPING_IOS_APP_DELEGATE_H
