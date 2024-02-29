// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include <UIKit/UIKit.h>

/**
 * @ingroup applicationdemocvdetectorqrcodesdetector
 * @defgroup applicationdemocvdetectorqrcodesdetectorios QR code detector demo on iOS
 * @{
 * The demo application demonstrates the 2D detector for QR codes<br>
 * This application is platform dependent and is implemented for iOS platforms.<br>
 * @}
 */

/**
 * The Application Delegate of the QR code demo application.
 * @ingroup applicationdemocvdetectorqrcodesdetectorios
 */
@interface AppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;

@end
