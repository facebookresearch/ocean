/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_MEDIA_IMAGEEXTRACTOR_IMAGE_EXTRACTOR_MAIN_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_MEDIA_IMAGEEXTRACTOR_IMAGE_EXTRACTOR_MAIN_H

#include "application/ocean/demo/media/ApplicationDemoMedia.h"

/**
 * @ingroup applicationdemomedia
 * @defgroup applicationdemomediaimageextractor Image Extractor
 * @{
 * The demo application shows how to extract individual images from a source medium e.g., a movie.<br>
 * This demo example shows to individual possibilities to extract frames from a video/movie.
 *
 * The first possibility is the application of the MovieFrameProvider (in combination with a Movie object).
 * While the Movie object delivers the actual video frames, the MovieFrameProvider can be used to access random frames.<br>
 * Thus, the MovieFrameProvider could be used to move forward and backward within the video stream,<br>
 * or to jump between arbitrary frames.
 *
 * The second possibility is the application of the Movie object directly without using the MovieFrameProvider.
 * In this case, we can configure the Movie object to deliver video frames as fast as possible,<br>
 * and we can define a callback function which is called whenever a new frame arrives.
 *
 * This application is almost platform independent and is available on desktop platforms like e.g., Windows or MacOS.
 * @}
 */

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_MEDIA_IMAGEEXTRACTOR_IMAGE_EXTRACTOR_MAIN_H
