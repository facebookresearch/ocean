/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#import <Cocoa/Cocoa.h>

/**
 * @ingroup applicationdemocvdetectormessengercode
 * @defgroup applicationdemocvdetectormessengercodeosx Messenger Code (OSX)
 * @{
 * This application demonstrates the detection and decoding of Messenger code from video frames (e.g., from a live video/webcam).
 * The user has the possibility to define the video input source (with preferred frame dimension) via the commando line parameter.
 * This application will run the original implementation of the detection and decoding of the Messenger code and display the decoded payload as well as the runtime performance.
 * This demo application uses the platform independent MessengerCode class implementing most of the necessary code.
 * This application here is intended for Apple iOS platforms only.
 * @see MessengerCode, applicationdemocvdetectormessengercode
 *
 * The application can be started with several optional command arguments to configurate the behavior of the application.
 * 1. parameter: Name or filename of the input medium to be used for tracking e.g., "LiveVideoId:0", or "directory/trackingMovie.mp4", or "imageSequence0001.png".<br>
 * 2. parameter: Preferred frame dimenion of the input medium in pixel e.g., "640x480", or "1280x720", or "1920x1080".<br>
 * 3. parameter: Pixel format to be used, e.g., "Y8"<br>
 * 4. parameter: "loop" - input medium will be looped (if the medium is finite), "noloop" - input medium will be read until it ends (finite medium) or until interrupted, default: "loop". Other values will cause an error.
 *
 * Examples:
 * open democvdetectormessengercode.app
 *
 * open democvdetectormessengercode.app -- args LiveVideoId:0 1280x720
 * @}
 */

@interface AppDelegate : NSObject <NSApplicationDelegate>

@end
