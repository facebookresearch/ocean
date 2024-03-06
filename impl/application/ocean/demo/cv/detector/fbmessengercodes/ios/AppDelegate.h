// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_CV_DETECTOR_FB_MESSENGER_CODES_IOS_APP_DELEGATE_H
#define FACEBOOK_APPLICATION_OCEAN_CV_DETECTOR_FB_MESSENGER_CODES_IOS_APP_DELEGATE_H

#include "FBMessengerCodesWrapper.h"

#include <UIKit/UIKit.h>

/**
 * @ingroup applicationdemocvdetectorfbmessengercodes
 * @defgroup applicationdemocvdetectorfbmessengercodesios FB Messenger codes (iOS)
 * @{
 * This application demonstrates the detection and decoding of FB Messenger codes from video frames (e.g., from a live video/webcam).
 * The user has the possibility to define the video input source (with preferred frame dimension) via the commando line parameter.
 * This application will run the original implementation of the detection and decoding of the FB Messenger codes and display the decoded payload as well as the runtime performance.
 * This demo application uses the platform independent FBMessengerCodes class implementing most of the necessary code.
 * This application here is intended for Apple iOS platforms only.
 * @see FBMessengerCodes, applicationdemocvdetectorfbmessengercodes
 * @}
 */

/**
 * The Application Delegate of the FB Messenger codes demo application.
 * @ingroup applicationcvdetectorfbmessengercodesios
 */
@interface AppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;

@end

#endif // FACEBOOK_APPLICATION_OCEAN_CV_DETECTOR_FB_MESSENGER_CODES_IOS_APP_DELEGATE_H
