// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_UVTEXTUREMAPPING_WIN_UV_TEXTURE_MAPPING_MAIN_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_UVTEXTUREMAPPING_WIN_UV_TEXTURE_MAPPING_MAIN_H

#include "application/ocean/demo/tracking/ApplicationDemoTracking.h"

 /**
 * @ingroup applicationdemotrackinguvtexturemapping
 * @defgroup applicationdemotrackinguvtexturemappingwin UV Texture Mapping (Windows)
 * @{
 * TODO jtprice Add a description of the demo here.
 * This demo application uses the platform independent UVTextureMappingWrapper class implementing most of the necessary code.
 * However, this application here is intended for Windows platforms only.
 * @see UVTextureMappingWrapper, applicationdemotrackinguvtexturemappingosx
 *
 * The application should be started with two command line arguments/parameters:<br>
 * (if no command arguments are given a default configuration is used taking the first live camera and the 'sift' image as pattern)
 * (Recommended) First parameter: The name or filename of the input medium to be used for tracking e.g., "LiveVideoId:0", or "directory/trackingMovie.mp4", or "singleImage.png".<br>
 * (Recommended) Second parameter: The filename of the tracking pattern (the reference pattern) which will be detected e.g., "pattern.png", or "ocean/data/tracking/featuremaps/sift640x512.bmp".<br>
 * (Optional) Third parameter: The preferred frame dimension of the input medium in pixel e.g., "640x480", or "1280x720", or "1920x1080".<br>
 * (Optional) Fourth parameter: The tracker type to be applied either "Pattern 6DOF Tracker", or "ORB Feature Based 6DOF Tracker", or "Blob Feature Based 6DOF Tracker".<br>
 * (Optional) Fifth parameter: The filename of the camera calibration file containing the calibration for the input medium e.g., "ocean/data/cameracalibration/cameracalibration.occ".<br>
 *
 * Examples:
 * demotrackinguvtexturemapping.exe LiveVideoId:0 sift800x640.bmp 1920x1080 "Pattern 6DOF Tracker" <br>
 *
 * demotrackinguvtexturemapping.exe movie.mp4 pattern.png <br>
 *
 * demotrackinguvtexturemapping.exe LiveVideoId:1 sift800x640.bmp 1920x1080 "Pattern 6DOF Tracker" directory/cameracalibration.occ <br>
 *
 * @}
 */
#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_UVTEXTUREMAPPING_WIN_UV_TEXTURE_MAPPING_MAIN_H
