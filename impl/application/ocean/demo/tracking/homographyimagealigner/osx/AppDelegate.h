/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#import <Cocoa/Cocoa.h>

/**
 * @ingroup applicationdemotrackinghomographyimagealigner
 * @defgroup applicationdemotrackinghomographyimagealignerosx Homography Image Aligner (OSX)
 * @{
 * The demo application shows how successive video frsames (e.g., from a live video/webcam) can be aligned w.r.t. a homography.<br>
 * This application is mainly a platform specific wrapper (and GUI creator) for the platform independent HomographyImageAligner class.<br>
 * This demo application uses the platform independent HomographyImageAligner class implementing most of the necessary code.<br>
 * However, this application here is intended for Apple OSX platforms only.
 * @see HomographyImageAligner, applicationdemotrackinghomographyimagealignerwin
 *
 * The application can be started with several optional command arguments to configurate the behavior of the application.
 * First parameter: The name or filename of the input medium to be used for tracking e.g., "LiveVideoId:0", or "directory/trackingMovie.mp4", or "imageSequence0001.png".<br>
 * Second parameter: The preferred frame dimenion of the input medium in pixel e.g., "640x480", or "1280x720", or "1920x1080".<br>
 * Third parameter: The number of sparse feature points that should be used for homography determination, 150 by default.<br>
 * Fourth parameter: The size of the image patches that are used for point tracking (in pixel), possible values are 5, 7, 15, 31.<br>
 * Fifth parameter: The number of sub-pixel iterations that will be applied to determine the location of a tracked feature point, providing a tracking precision of 0.5 ^ iterations pixels, 4 by default ~ 0.0625 pixels.<br>
 * Sixth parameter: The maximal offset between two corresponding feature points in two successive frame (in pixel), 128 by default.<br>
 * Seventh parameter: The search radius of image patches on the coarsest pyramid layer (in pixel), 4 by default.<br>
 * Eighth parameter: The pixel format to be used for tracking, "RGB24" by default, possible values are "RBG24", "YUV24", "Y8".<br>
 * Ninth parameter: The statement whether a normal SSD or a Zero-Mean-SSD measure is applied for patch tracking, "zeromean" by default, possible values are "zeromean", "nozeromean".
 * Tenth parameter: The statement whether the input medium will be looped (if the medium is finite), "loop" by default, possible values are "loop", "noloop".
 *
 * Examples:
 * open demotrackinghomographyimagealigner.app
 *
 * open demotrackinghomographyimagealigner.app -- args LiveVideoId:0 1280x720
 *
 * open demotrackinghomographyimagealigner.app -- args anyMove.mp4 "" 80 15 4 128 8 RGB24 zeromean
 *
 * open demotrackinghomographyimagealigner.app -- args imageSequence1.jpg
 * @}
 */

@interface AppDelegate : NSObject <NSApplicationDelegate>

@end
