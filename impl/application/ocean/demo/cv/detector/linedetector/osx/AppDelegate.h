/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_CV_DETECTOR_LINEDETECTOR_OSX_APP_DELEGATE_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_CV_DETECTOR_LINEDETECTOR_OSX_APP_DELEGATE_H

#import <Cocoa/Cocoa.h>

/**
 * @ingroup applicationdemocvdetectorlinedetector
 * @defgroup applicationdemocvdetectorlinedetectorosx Line Detector (macOS)
 * @{
 * The demo application demonstrates the usage of a Line Detector able to detect lines in frames.<br>
 * This application here is intended for Apple macOS platforms only.
 *
 * The application should be started with two command line arguments/parameters:<br>
 * (if no command arguments are given a default configuration is used)
 * (Recommended) First parameter: The name or filename of the input medium to be used as input e.g., "LiveVideoId:0", or "directory/trackingMovie.mp4", or "singleImage.png".<br>
 * (Optional) Second parameter: The preferred frame dimenion of the input medium in pixel e.g., "640x480", or "1280x720", or "1920x1080".<br>
 *
 * Examples:
 * open democvdetectorlinedetector.app --args LiveVideoId:0 <br>
 *
 * open democvdetectorlinedetector.app --args movie.mp4 <br>
 *
 * open democvdetectorlinedetector.app --args LiveVideoId:1 1920x1080 <br>
 *
 * @see applicationdemocvdetectorlinedetectorios, applicationdemocvdetectorlinedetectorandroid.
 * @}
 */

@interface AppDelegate : NSObject <NSApplicationDelegate>

@end

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_CV_DETECTOR_LINEDETECTOR_OSX_APP_DELEGATE_H
