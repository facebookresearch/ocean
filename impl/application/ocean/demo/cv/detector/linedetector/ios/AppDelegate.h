/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_CV_DETECTOR_LINEDETECTOR_IOS_APP_DELEGATE_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_CV_DETECTOR_LINEDETECTOR_IOS_APP_DELEGATE_H

#include "ocean/base/Base.h"

#include <UIKit/UIKit.h>
 
/**
 * @ingroup applicationdemocvdetectorlinedetector
 * @defgroup applicationdemocvdetectorlinedetectorios Line Detector (iOS)
 * @{
 * The demo application demonstrates the usage of a Line Detector able to detect lines in frames.<br>
 * The back facing camera of the iOS device is used as tracking input source.<br>
 * This application here is intended for Apple iOS platforms only.
 * @see applicationdemotrackingpointtrackerosx, applicationdemotrackingpointtrackerandroid.
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

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_CV_DETECTOR_LINEDETECTOR_IOS_APP_DELEGATE_H
