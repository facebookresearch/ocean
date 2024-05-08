// Copyright (c) Meta Platforms, Inc. and affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#import <Cocoa/Cocoa.h>

/**
 * @ingroup applicationdemotrackingfeaturetracker
 * @defgroup applicationdemotrackingfeaturetrackerosx Feature Tracker (OSX)
 * @{
 * The demo application demonstrates the usage of the "Blob Feature Based 6DOF Tracker", "ORB Feature Based 6DOF Tracker", or "Pattern 6DOF Tracker" tracker.<br>
 * The user has the possibility to define the video input source (with preferred frame dimension), the pattern, and the used tracker via the commando line parameter.<br>
 * The application will visualize a bounding box and a coordinate system in every input frame for which a valid camera pose could be determined.<br>
 * This demo application uses the platform independent FeatureTracker class implementing most of the necessary code.
 * However, this application here is intended for Apple OSX platforms only.
 * @see FeatureTracker, applicationdemotrackingfeaturetrackerwin
 *
 * The application should be started with two command line arguments/parameters:<br>
 * (if no command arguments are given a default configuration is used taking the first live camera and the 'sift' image as pattern)
 * (Recommended) First parameter: The name or filename of the input medium to be used for tracking e.g., "LiveVideoId:0", or "directory/trackingMovie.mp4", or "singleImage.png".<br>
 * (Recommended) Second parameter: The filename of the tracking pattern (the reference pattern) which will be detected e.g., "pattern.png", or "ocean/data/tracking/featuremaps/sift640x512.bmp".<br>
 * (Optional) Third parameter: The preferred frame dimenion of the input medium in pixel e.g., "640x480", or "1280x720", or "1920x1080".<br>
 * (Optional) Fourth parameter: The tracker type to be applied either "Pattern 6DOF Tracker", or "ORB Feature Based 6DOF Tracker", or "Blob Feature Based 6DOF Tracker".<br>
 * (Optional) Fifth parameter: The filename of the camera calibration file containing the calibration for the input medium e.g., "ocean/data/cameracalibration/cameracalibration.occ".<br>
 *
 * Examples:
 * open demotrackingfeaturetracker.app --args LiveVideoId:0 sift800x640.bmp 1920x1080 "Pattern 6DOF Tracker" <br>
 *
 * open demotrackingfeaturetracker.app --args movie.mp4 pattern.png <br>
 *
 * open demotrackingfeaturetracker.app --args LiveVideoId:1 sift800x640.bmp 1920x1080 "Pattern 6DOF Tracker" directory/cameracalibration.occ <br>
 *
 * @}
 */

@interface AppDelegate : NSObject <NSApplicationDelegate>

@end
