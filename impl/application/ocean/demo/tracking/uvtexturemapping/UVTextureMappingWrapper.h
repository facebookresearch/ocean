// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_UVTEXTUREMAPPING_UV_TEXTURE_MAPPING_WRAPPER_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_UVTEXTUREMAPPING_UV_TEXTURE_MAPPING_WRAPPER_H

#include "application/ocean/demo/tracking/ApplicationDemoTracking.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/devices/OrientationTracker3DOF.h"

#include "ocean/math/Cone3.h"
#include "ocean/math/Cylinder3.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/Vector2.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/tracking/VisualTracker.h"

/**
 * @ingroup applicationdemotracking
 * @defgroup applicationdemotrackinguvtexturemapping UV Texture Mapping
 * @{
 * TODO jtprice Add a description of this demo here.
 * The implementation of the basic functionality is platform independent, thus is can be used on any platform.
 * @}
 */

using namespace Ocean;

/**
 * This class implements the platform independent UV texture mapping functionality which will be used/shared by/across platform specific applications.
 * @ingroup applicationdemotrackinguvtexturemapping
 */
class UVTextureMappingWrapper
{
	public:

		/**
		 * Creates an invalid UV texture mapping object.
		 */
		UVTextureMappingWrapper();

		/**
		 * Move constructor.
		 * @param uvTextureMappingWrapper The UV texture mapping wrapper object to be moved
		 */
		inline UVTextureMappingWrapper(UVTextureMappingWrapper&& uvTextureMappingWrapper) noexcept;

		/**
		 * Creates a new UV texture mapping object by a given set of command arguments.
		 * The command arguments can be used to specify the behavior/mode of the tracker:<br>
		 *
		 * 1. Parameter (optional): The name or filename of the input source e.g.:
		 * "LiveVideoId:0", or "directory/trackingMovie.mp4", or "singleImage.png"
		 *
		 * 2. Parameter (optional): The filename of the tracking pattern e.g.:
		 * "pattern.png", or "/absolute/path/to/image.jpg", or "ocean/res/application/ocean/demo/tracking/uvtexturemapping/sift640x512.bmp"
		 *
		 * 3. Parameter (optional): The preferred frame dimension of the input medium in pixel:
		 * "640x480", or "1280x720", or "1920x1080"
		 *
		 * 4. Parameter: The tracker type to be applied:
		 * "Pattern 6DOF Tracker [for {cones, cylinders}]", or "ORB Feature Based 6DOF Tracker", or "Blob Feature Based 6DOF Tracker [for {cones, cylinders, cubes, meshes}]"
		 *
		 * 5. Parameter: The filename of the camera calibration file containing the calibration for the input source e.g.,:
		 * "ocean/res/application/ocean/demo/tracking/uvtexturemapping/cameracalibration.occ"
		 *
		 * 6. Parameter (required for cones, cylinders, and meshes): Space-delimited list of shape parameters as a string, or a filename containing ASCII data for such a string
		 * - Cone parameters: [0] coneHeight, [1] largerDiameter, [2] smallerDiameter,
		 *     [3] largerArcEnd.x(), [4] largerArcEnd.y(), [5] smallerArcEnd.x(), [6] smallerArcEnd.y(),
		 *     [7] yAxisIntersection.x(), [8] yAxisIntersection.y(), [9] originOnSurface as 0 or 1
		 * - Cylinder parameters: [0] patternCropWidth, [1] cylinderMetricHeight, [2] xAxisOffset,
		 *     [3] yAxisIntersection.x(), [4] yAxisIntersection.y()
		 * - Meshes: The input must be an OBJ filepath. Only "v", "vt", and "f" OBJ entries are used.
		 *     Texture coordinates are assumed to be relative to the bottom left of the texture image
		 *     and specified in normalized [0,1]x[0,1] coordinates. Face winding order should be
		 *     specified so that the surface normal points outward.
		 *
		 * @param commandArguments The command arguments that configure the properties of the UV texture mapping
		 */
		explicit UVTextureMappingWrapper(const std::vector<std::wstring>& commandArguments);

		/**
		 * Destructs a UV texture mapping wrapper object.
		 */
		~UVTextureMappingWrapper();

		/**
		 * Explicitly releases this UV texture mapping object.
		 */
		void release();

		/**
		 * Checks if the medium holds a new frame and if so applies the tracking for the frame.
		 * @param frame The resulting frame showing a virtual box (with coordinate system) around the pattern (if detected)
		 * @param time The time the tracker needed to process the frame, averaged over 20 iterations, in seconds, with range [0, infinity), negative if the pattern has not been detected yet
		 * @return True, if succeeded
		 */
		bool trackNewFrame(Frame& frame, double& time);

		/**
		 * Move operator.
		 * @param uvTextureMappingWrapper The object to be moved
		 * @return Reference to this object
		 */
		inline UVTextureMappingWrapper& operator=(UVTextureMappingWrapper&& uvTextureMappingWrapper) noexcept;

	protected:

		/// The frame medium providing the visual information for this object.
		Media::FrameMediumRef trackerFrameMedium;

		/// The bounding box  of the tracking pattern defined in the world coordinate system, with range (0, infinity)x(0, infinity)
		Box3 trackerObjectDimension;

		/// The tracked cone being used, if any.
		Cone3 trackerObjectCone;

		/// The tracked cylinder being used, if any.
		Cylinder3 trackerObjectCylinder;

		/// The tracked mesh being used, if any.
		Triangles3 trackerObjectTriangles;

		/// The actual tracker to be used for demo.
		Tracking::VisualTrackerRef visualTracker;

		/// The camera profile defining the project and the camera distortion.
		PinholeCamera trackerCamera;

		/// The timestamp of the last frame that has been handled.
		Timestamp trackerFrameTimestamp;

		/// The performance measurement object.
		HighPerformanceStatistic trackerPerformance;

		/// The 3DOF orientation tracker which is used to support the tracker.
		Devices::OrientationTracker3DOFRef orientationTracker3DOF_;
};

inline UVTextureMappingWrapper::UVTextureMappingWrapper(UVTextureMappingWrapper&& uvTextureMappingWrapper) noexcept
{
	*this = std::move(uvTextureMappingWrapper);
}

inline UVTextureMappingWrapper& UVTextureMappingWrapper::operator=(UVTextureMappingWrapper&& uvTextureMappingWrapper) noexcept
{
	if (this != &uvTextureMappingWrapper)
	{
		trackerFrameMedium = std::move(uvTextureMappingWrapper.trackerFrameMedium);
		trackerObjectDimension = std::move(uvTextureMappingWrapper.trackerObjectDimension);
		trackerObjectCone = std::move(uvTextureMappingWrapper.trackerObjectCone);
		trackerObjectCylinder = std::move(uvTextureMappingWrapper.trackerObjectCylinder);
		trackerObjectTriangles = std::move(uvTextureMappingWrapper.trackerObjectTriangles);

		visualTracker = std::move(uvTextureMappingWrapper.visualTracker);
		trackerCamera = uvTextureMappingWrapper.trackerCamera;

		trackerFrameTimestamp = uvTextureMappingWrapper.trackerFrameTimestamp;
		trackerPerformance = uvTextureMappingWrapper.trackerPerformance;

		orientationTracker3DOF_ = std::move(uvTextureMappingWrapper.orientationTracker3DOF_);
	}

	return *this;
}

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_UVTEXTUREMAPPING_UV_TEXTURE_MAPPING_WRAPPER_H
