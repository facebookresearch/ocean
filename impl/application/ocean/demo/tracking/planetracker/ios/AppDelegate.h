// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_PLANETRACKER_IOS_APP_DELEGATE_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_PLANETRACKER_IOS_APP_DELEGATE_H

#include <UIKit/UIKit.h>

/**
 * @ingroup applicationdemotrackingplanetracker
 * @defgroup applicationdemotrackingplanetrackerios Plane Tracker (iOS)
 * @{
 * The demo application shows the capabilities of Ocean's plane tracker.<br>
 * However, this application here is intended for Apple iOS platforms only.
 * @}
 */

/**
 * The Application Delegate of the demo application.
 * @ingroup applicationdemotrackingplanetrackerios
 */
@interface AppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;

@end

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_PLANETRACKER_IOS_APP_DELEGATE_H
