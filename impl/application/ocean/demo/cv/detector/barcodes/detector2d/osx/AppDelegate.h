// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#import <Cocoa/Cocoa.h>

/**
 * @ingroup applicationdemocvdetectorbarcodes
 * @defgroup applicationdemocvdetectorbarcodesosx Base Codes (OSX)
 * @{
 * This application demonstrates the detection and decoding of bar codes from video frames (e.g., from a live video/webcam).
 * The user has the possibility to define the video input source (with preferred frame dimension) via the commando line parameter.
 * This application will run the original implementation of the detection and decoding of the bar codes and display the decoded payload as well as the runtime performance.
 * This demo application uses the platform independent QRCode class implementing most of the necessary code.
 * This application here is intended for Apple iOS platforms only.
 * @see QRCode, applicationdemocvdetectorqrcode
 * @}
 */

@interface AppDelegate : NSObject <NSApplicationDelegate>

@end
