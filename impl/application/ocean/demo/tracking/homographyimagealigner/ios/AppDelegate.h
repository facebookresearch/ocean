/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_HOMOGRAPHYIMAGEALIGNER_IOS_APP_DELEGATE_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_HOMOGRAPHYIMAGEALIGNER_IOS_APP_DELEGATE_H

#include <UIKit/UIKit.h>

/**
 * @ingroup applicationdemotrackinghomographyimagealigner
 * @defgroup applicationdemotrackinghomographyimagealignerios Homography Image Aligner (iOS)
 * @{
 * The demo application shows how successive video frames (e.g., from a live video/webcam) can be aligned w.r.t. a homography.<br>
 * The user has the possibility to define the video input source (with preferred frame dimension) via the commando line parameter.<br>
 * The application will blend the current video frame with the previous video frame (while the previous frame is transformed by application of the determined homography).<br>
 * This demo application uses the platform independent HomographyImageAligner class implementing most of the necessary code.
 * However, this application here is intended for Apple iOS platforms only.
 * @see HomographyImageAligner, applicationdemotrackinghomographyimagealignerosx
 * @}
 */

/**
 * The Application Delegate of the Video Preview demo application.
 * @ingroup applicationdemotrackinghomographyimagealignerios
 */
@interface AppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;

@end

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_HOMOGRAPHYIMAGEALIGNER_IOS_APP_DELEGATE_H
