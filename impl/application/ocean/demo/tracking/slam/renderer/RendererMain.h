/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_APPLICATION_OCEAN_DEMO_TRACKING_SLAM_RENDERER_RENDERER_MAIN_H
#define META_OCEAN_APPLICATION_OCEAN_DEMO_TRACKING_SLAM_RENDERER_RENDERER_MAIN_H

#include "application/ocean/demo/tracking/slam/ApplicationDemoTrackingSLAM.h"

#include "ocean/devices/GravityTracker3DOF.h"
#include "ocean/devices/OrientationTracker3DOF.h"

/**
 * @ingroup applicationdemotrackingslam
 * @defgroup applicationdemotrackingslamrenderer SLAM Renderer
 * @{
 * The demo application replays a serialized recording (.osn file) and runs the SLAM tracker on it.<br>
 * The application uses a rendering engine to visualize AR content (virtual rendered content) and<br>
 * outputs the visualization to a video file.
 *
 * The application uses Ocean's SerializerDevicePlayer to replay frame mediums along with associated sensor data<br>
 * like gravity and orientation from an .osn file.
 *
 * The SLAM tracker processes each frame and the rendering engine can display:<br>
 * - 3D scenes loaded from scene files (OBJ, X3D, etc.)<br>
 * - Virtual content overlaid on the camera frames
 *
 * This application is platform independent and is available on desktop platforms like e.g., Windows or MacOS.
 * @}
 */

using namespace Ocean;

/**
 * Just a helper class to ensure that all media and rendering plugins are unregistered when this object is disposed.
 * @ingroup applicationdemotrackingslamrenderer
 */
class ScopedPlugins
{
	public:

		/**
		 * Creates a new object and registers all plugins.
		 */
		ScopedPlugins();

		/**
		 * Destructs this object and unregisters all plugins.
		 */
		~ScopedPlugins();
};

/**
 * Helper class providing access to gravity and orientation sensor data.
 * This class manages GravityTracker3DOF and OrientationTracker3DOF devices and provides convenience functions to retrieve sensor measurements transformed into the camera coordinate system.
 * @ingroup applicationdemotrackingslamrenderer
 */
class SensorAccessor
{
	public:

		/**
		 * Creates a new sensor accessor object.
		 */
		SensorAccessor() = default;

		/**
		 * Returns the gravity vector in the camera coordinate system.
		 * On first call, this function will initialize and start the gravity tracker device.
		 * @param device_Q_camera The rotation transforming points in camera coordinates to device coordinates
		 * @param timestamp The timestamp for which the gravity measurement is requested
		 * @return The gravity vector in camera coordinates, a null vector if unavailable
		 */
		Vector3 cameraGravity(const Quaternion& device_Q_camera, const Timestamp& timestamp);

		/**
		 * Returns the camera's orientation in world coordinates.
		 * On first call, this function will initialize and start the orientation tracker device.
		 * @param device_Q_camera The rotation transforming points in camera coordinates to device coordinates
		 * @param timestamp The timestamp for which the orientation measurement is requested
		 * @return The rotation transforming points in camera coordinates to world coordinates, invalid if unavailable
		 */
		Quaternion anyWorld_Q_camera(const Quaternion& device_Q_camera, const Timestamp& timestamp);

		/**
		 * Releases all device references held by this object.
		 */
		void release();

	protected:

		/// The gravity tracker device providing gravity measurements.
		Devices::GravityTracker3DOFRef gravityTracker_;

		/// The orientation tracker device providing orientation measurements.
		Devices::OrientationTracker3DOFRef orientationTracker_;
};

#endif // META_OCEAN_APPLICATION_OCEAN_DEMO_TRACKING_SLAM_RENDERER_RENDERER_MAIN_H
