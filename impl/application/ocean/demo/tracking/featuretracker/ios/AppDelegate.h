/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_FEATURETRACKER_IOS_APP_DELEGATE_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_FEATURETRACKER_IOS_APP_DELEGATE_H

#include <UIKit/UIKit.h>

/**
 * @ingroup applicationdemotrackingfeaturetracker
 * @defgroup applicationdemotrackingfeaturetrackerios Feature Tracker (iOS)
 * @{
 * The demo application demonstrates the usage of the "Pattern 6DOF Tracker" or the "ORB Feature Based 6DOF Tracker" tracker.<br>
 * The back facing camera of the iOS device is used as tracking input source.<br>
 * The application will visualize a bounding box and a coordinate system in every input frame for which a valid camera pose could be determined.<br>
 * This demo application uses the platform independent FeatureTracker class implementing most of the necessary code.
 * However, this application here is intended for Apple iOS platforms only.
 * @see FeatureTracker, applicationdemotrackingfeaturetrackerosx
 * @}
 */

/**
 * The Application Delegate of the Video Preview demo application.
 * @ingroup applicationdemomediavideopreviewios
 */
@interface AppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;

@end

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_FEATURETRACKER_IOS_APP_DELEGATE_H
