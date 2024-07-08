/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_MEDIA_VIDEOPREVIEW_APP_DELEGATE_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_MEDIA_VIDEOPREVIEW_APP_DELEGATE_H

#include "ocean/base/Base.h"

#include <UIKit/UIKit.h>

/**
 * @ingroup applicationdemomediavideopreview
 * @defgroup applicationdemomediavideopreviewios Video Preview (iOS)
 * @{
 * The demo application implements a basic video inpainter.
 * This application is available for iOS platforms only (but sibling applications exist for other platforms).
 * @see applicationdemomediavideopreviewosx, applicationdemomediavideopreviewosxwin
 * @}
 */

using namespace Ocean;

/**
 * The Application Delegate of the Video Preview demo application.
 * @ingroup applicationdemomediavideopreviewios
 */
@interface AppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;

@end

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_MEDIA_VIDEOPREVIEW_APP_DELEGATE_H
