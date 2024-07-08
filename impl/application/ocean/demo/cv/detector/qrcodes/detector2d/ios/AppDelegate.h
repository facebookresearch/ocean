/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <UIKit/UIKit.h>

/**
 * @ingroup applicationdemocvdetectorqrcodesdetector2d
 * @defgroup applicationdemocvdetectorqrcodesdetector2dios QR code detector demo on iOS
 * @{
 * The demo application demonstrates the 2D detector for QR codes<br>
 * This application is platform dependent and is implemented for iOS platforms.<br>
 * @}
 */

/**
 * The Application Delegate of the QR code demo application.
 * @ingroup applicationdemocvdetectorqrcodesdetector2dios
 */
@interface AppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;

@end
