/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_APPLICATION_OCEAN_DEMO_TRACKING_SLAM_REGRESSION_REGRESSION_MAIN_H
#define META_OCEAN_APPLICATION_OCEAN_DEMO_TRACKING_SLAM_REGRESSION_REGRESSION_MAIN_H

#include "application/ocean/demo/tracking/slam/ApplicationDemoTrackingSLAM.h"

/**
 * @ingroup applicationdemotrackingslam
 * @defgroup applicationdemotrackingslamregression SLAM Regression Testing
 * @{
 * The demo application runs regression tests on SLAM tracker recordings (.osn files).<br>
 * It replays recordings and compares the resulting tracking statistics with saved baselines to detect performance regressions.
 *
 * The application uses Ocean's SerializerDevicePlayer to replay frame mediums along with associated sensor data like gravity and orientation from .osn files.
 *
 * For each .osn file in the specified directory, the application:<br>
 * - Replays the recording through TrackerMono<br>
 * - Compares statistics (valid poses, correspondences, Bundle Adjustment iterations) with baseline<br>
 * - Reports regressions or improvements beyond configured tolerances<br>
 * - Creates baseline files for new recordings
 *
 * This application is platform independent and is available on desktop platforms like e.g., Windows or MacOS.
 * @}
 */

using namespace Ocean;

/**
 * Just a helper class to ensure that all media plugins are unregistered when this object is disposed.
 * @ingroup applicationdemotrackingslamregression
 */
class ScopedPlugin
{
	public:

		/**
		 * Creates a new object and registers all plugins.
		 */
		ScopedPlugin();

		/**
		 * Destructs this object and unregisters all plugins.
		 */
		~ScopedPlugin();
};

#endif // META_OCEAN_APPLICATION_OCEAN_DEMO_TRACKING_SLAM_REGRESSION_REGRESSION_MAIN_H
