/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_APPLICATION_OCEAN_DEMO_TRACKING_SLAM_SLAMTRACKER_IOS_APP_DELEGATE_H
#define META_OCEAN_APPLICATION_OCEAN_DEMO_TRACKING_SLAM_SLAMTRACKER_IOS_APP_DELEGATE_H

#include "application/ocean/demo/tracking/slam/slamtracker/SLAMTrackerWrapper.h"

#include <UIKit/UIKit.h>

/**
 * @ingroup applicationdemotrackingslamslamtracker
 * @defgroup applicationdemotrackingslamslamtrackerios SLAM Tracker (iOS)
 * @{
 * The demo application shows the capabilities of Ocean's SLAM tracker.<br>
 * This application here is intended for Apple iOS platforms only.
 * @}
 */

/**
 * The Application Delegate of the demo application.
 * @ingroup applicationdemotrackingslamslamtrackerios
 */
@interface AppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;

@end

#endif // META_OCEAN_APPLICATION_OCEAN_DEMO_TRACKING_SLAM_SLAMTRACKER_IOS_APP_DELEGATE_H
