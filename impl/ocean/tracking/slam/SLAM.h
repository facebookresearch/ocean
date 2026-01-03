/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_SLAM_SLAM_H
#define META_OCEAN_TRACKING_SLAM_SLAM_H

#include "ocean/tracking/Tracking.h"

namespace Ocean
{

namespace Tracking
{

namespace SLAM
{

/**
 * @ingroup tracking
 * @defgroup trackingslam Ocean SLAM Tracking Library
 * @{
 * The Ocean SLAM Tracking Library provides a monocular visual SLAM implementation for real-time camera tracking and 3D environment reconstruction, optimized for mobile AR/VR applications.
 * The pipeline detects and tracks Harris corner features across image pyramids, estimates camera poses using P3P+RANSAC with optional IMU gravity constraints, and builds a 3D map through multi-view triangulation with background bundle adjustment.
 * Core classes include TrackerMono (main tracker), LocalizedObjectPoint (3D map points with FREAK descriptors), CameraPoses (thread-safe pose storage), and TrackingCorrespondences (frame-to-frame feature tracking).
 * @see Tracker, TrackerMono, CameraPose, LocalizedObjectPoint
 * @}
 */

/**
 * @namespace Ocean::Tracking::SLAM Namespace of the SLAM Tracking library.<p>
 * The Namespace Ocean::Tracking::SLAM is used in the entire Ocean SLAM Tracking Library.
 */

// Defines OCEAN_TRACKING_SLAM_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_TRACKING_SLAM_EXPORT
		#define OCEAN_TRACKING_SLAM_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_TRACKING_SLAM_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_TRACKING_SLAM_EXPORT
#endif

}

}

}

#endif // META_OCEAN_TRACKING_SLAM_SLAM_H
