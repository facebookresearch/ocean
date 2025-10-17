/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_CV_CALIBRATION_PICTURETAKER_IOS_APP_DELEGATE_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_CV_CALIBRATION_PICTURETAKER_IOS_APP_DELEGATE_H

#include "application/ocean/demo/cv/calibration/ApplicationDemoCVCalibration.h"

#include <UIKit/UIKit.h>

/**
 * @ingroup applicationdemocvcalibrationpicturetaker
 * @defgroup applicationdemocvcalibrationpicturetakerios PictureTaker (iOS)
 * @{
 * The demo application allows to take pictures from a live video stream and to store the pictures in a file.<br>
 * This application here is intended for Apple iOS platforms only.
 * @}
 */

/**
 * The Application Delegate of the demo application.
 * @ingroup applicationdemocvcalibrationpicturetakerios
 */
@interface AppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;

@end

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_CV_CALIBRATION_PICTURETAKER_IOS_APP_DELEGATE_H
