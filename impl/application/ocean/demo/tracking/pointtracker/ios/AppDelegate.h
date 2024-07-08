/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_POINTTRACKER_IOS_APP_DELEGATE_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_POINTTRACKER_IOS_APP_DELEGATE_H

#include "ocean/base/Base.h"

#include <UIKit/UIKit.h>

/**
 * @ingroup applicationdemotrackingpointtracker
 * @defgroup applicationdemotrackingpointtrackerios Point Tracker (iOS)
 * @{
 * The demo application demonstrates the usage of the PointTracker able to track points from one frame to another while keeping the history from previous frames.<br>
 * The back facing camera of the iOS device is used as tracking input source.<br>
 * This demo application uses the platform independent PointTracker class implementing most of the necessary code.
 * However, this application here is intended for Apple iOS platforms only.
 * @see PointTracker, applicationdemotrackingpointtrackerosx
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

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_POINTTRACKER_IOS_APP_DELEGATE_H
