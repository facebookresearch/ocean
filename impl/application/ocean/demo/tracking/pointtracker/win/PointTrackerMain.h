/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_POINTTRACKER_WIN_POINT_TRACKER_MAIN_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_POINTTRACKER_WIN_POINT_TRACKER_MAIN_H

#include "application/ocean/demo/tracking/ApplicationDemoTracking.h"

/**
 * @ingroup applicationdemotrackingpointtracker
 * @defgroup applicationdemotrackingpointtrackerwin Point Tracker (Windows)
 * @{
 * The demo application demonstrates the usage of the PointTracker able to track points from one frame to another while keeping the history from previous frames.<br>
 * The back facing camera of the iOS device is used as tracking input source.<br>
 * This demo application uses the platform independent PointTracker class implementing most of the necessary code.
 * However, this application here is intended for Windows platforms only.
 * @see PointTracker, applicationdemotrackingpointtrackerosx
 *
 * The application should be started with two command line arguments/parameters:<br>
 * (if no command arguments are given a default configuration is used taking the first live camera and the 'sift' image as pattern)
 * (Recommended) First parameter: The name or filename of the input medium to be used for tracking e.g., "LiveVideoId:0", or "directory/trackingMovie.mp4", or "singleImage.png".<br>
 * (Optional) Second parameter: The preferred frame dimenion of the input medium in pixel e.g., "640x480", or "1280x720", or "1920x1080".<br>
 *
 * Examples:
 * demotrackingpointtracker.exe LiveVideoId:0 1920x1080 <br>
 *
 * demotrackingpointtracker.exe movie.mp4 <br>
 *
 * demotrackingpointtracker.exe LiveVideoId:1 <br>
 *
 * @}
 */
#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_POINTTRACKER_WIN_POINT_TRACKER_MAIN_H
