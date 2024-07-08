/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_APPLICATION_OCEAN_CV_DETECTOR_MESSENGER_CODE_IOS_APP_DELEGATE_H
#define META_OCEAN_APPLICATION_OCEAN_CV_DETECTOR_MESSENGER_CODE_IOS_APP_DELEGATE_H

#include "MessengerCodeWrapper.h"

#include <UIKit/UIKit.h>

/**
 * @ingroup applicationdemocvdetectormessengercode
 * @defgroup applicationdemocvdetectormessengercodeios Messenger code (iOS)
 * @{
 * This application demonstrates the detection and decoding of Messenger code from video frames (e.g., from a live video/webcam).
 * The user has the possibility to define the video input source (with preferred frame dimension) via the commando line parameter.
 * This application will run the original implementation of the detection and decoding of the Messenger code and display the decoded payload as well as the runtime performance.
 * This demo application uses the platform independent MessengerCode class implementing most of the necessary code.
 * This application here is intended for Apple iOS platforms only.
 * @see MessengerCode, applicationdemocvdetectormessengercode
 * @}
 */

/**
 * The Application Delegate of the Messenger code demo application.
 * @ingroup applicationcvdetectormessengercodeios
 */
@interface AppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;

@end

#endif // META_OCEAN_APPLICATION_OCEAN_CV_DETECTOR_MESSENGER_CODE_IOS_APP_DELEGATE_H
