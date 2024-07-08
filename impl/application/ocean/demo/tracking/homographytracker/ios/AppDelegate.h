/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_HOMOGRAPHYTRACKER_IOS_APP_DELEGATE_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_HOMOGRAPHYTRACKER_IOS_APP_DELEGATE_H

#include "application/ocean/demo/tracking/homographytracker/HomographyTrackerWrapper.h"

#include <UIKit/UIKit.h>

/**
 * @ingroup applicationdemotrackinghomographytracker
 * @defgroup applicationdemotrackinghomographytrackerios Homography Tracker (iOS)
 * @{
 * The demo application shows the capabilities of Ocean's homography tracker.<br>
 * This application here is intended for Apple iOS platforms only.
 * @}
 */

/**
 * The Application Delegate of the demo application.
 * @ingroup applicationdemotrackinghomographytrackerios
 */
@interface AppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;

@end

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_HOMOGRAPHYTRACKER_IOS_APP_DELEGATE_H
