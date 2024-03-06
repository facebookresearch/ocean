// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#import <Cocoa/Cocoa.h>

/**
 * @ingroup applicationdemocvdetectorfbmessengercodes
 * @defgroup applicationdemocvdetectorfbmessengercodesosx FB Messenger Codes (OSX)
 * @{
 * This application demonstrates the detection and decoding of FB Messenger codes from video frames (e.g., from a live video/webcam).
 * The user has the possibility to define the video input source (with preferred frame dimension) via the commando line parameter.
 * This application will run the original implementation of the detection and decoding of the FB Messenger codes and display the decoded payload as well as the runtime performance.
 * This demo application uses the platform independent FBMessengerCodes class implementing most of the necessary code.
 * This application here is intended for Apple iOS platforms only.
 * @see FBMessengerCodes, applicationdemocvdetectorfbmessengercodes
 *
 * The application can be started with several optional command arguments to configurate the behavior of the application.
 * 1. parameter: Name or filename of the input medium to be used for tracking e.g., "LiveVideoId:0", or "directory/trackingMovie.mp4", or "imageSequence0001.png".<br>
 * 2. parameter: Preferred frame dimenion of the input medium in pixel e.g., "640x480", or "1280x720", or "1920x1080".<br>
 * 3. parameter: Pixel format to be used, e.g., "Y8"<br>
 * 4. parameter: File path of the badge image (BMP), e.g. `/path/to/messenger_code_badge_template.bmp` (required by the original implementation of the FB Messenger code detection) <br>
 * 5. parameter: File path of the bullseye image (BMP), e.g. `/path/to/messenger_code_bullseye_template.bmp` (required by the original implementation of the FB Messenger code detection) <br>
 * 6. parameter: Filepath of the classifier configuration, e.g., `/path/to/MessengerCodeDetectClassifier1.xml` (required by the original implementation of the FB Messenger code detection)<BR>
 * 7. parameter: "loop" - input medium will be looped (if the medium is finite), "noloop" - input medium will be read until it ends (finite medium) or until interrupted, default: "loop". Other values will cause an error.
 *
 * Examples:
 * open democvdetectorfbmessengercodes.app
 *
 * open democvdetectorfbmessengercodes.app -- args LiveVideoId:0 1280x720
 * @}
 */

@interface AppDelegate : NSObject <NSApplicationDelegate>

@end
