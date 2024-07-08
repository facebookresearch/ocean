/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_MISC_MAINTENANCE_VIEWER_WIN_MV_MAINTENANCE_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_MISC_MAINTENANCE_VIEWER_WIN_MV_MAINTENANCE_H

#include "application/ocean/demo/misc/maintenanceviewer/win/MVApplication.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Maintenance.h"

#include "ocean/math/HomogenousMatrix4.h"

/**
 * This class provides maintenance data helper functions.
 * @ingroup applicationdemomiscmaintenanceviewerwin
 */
class MVMaintenance
{
	public:

		/**
		 * Decodes a tracking environment composed of a frame mesh (a frame with correspondences of 2D image points and 3D object points), a camera pose from which the frame has been captured and independent set of 3D object points.
		 * @param buffer The buffer to decode
		 * @param frame The resulting decoded frame
		 * @param frameImagePoints The resulting decoded frame image points
		 * @param frameObjectPoints The resulting decoded frame object points, one object point for each frame image points
		 * @param framePose The resulting pose of the frame
		 * @param objectPoints The resulting independent set of 3D object points
		 * @return True, if succeeded
		 */
		static bool decodeEnvironment(const Maintenance::Buffer& buffer, Frame& frame, Vectors2& frameImagePoints, Vectors3& frameObjectPoints, HomogenousMatrix4& framePose, Vectors3& objectPoints);

		/**
		 * Decodes a frame from a maintenance data buffer.
		 * @param data The buffer providing the frame, will be shifted by the number of consumed bytes afterwards
		 * @param size The size of the entire buffer, will be reduced by the number of consumed bytes afterwards
		 * @param frame The resulting frame
		 * @return True, if succeeded
		 */
		static bool decodeFrame(const uint8_t*& data, size_t& size, Frame& frame);
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_MISC_MAINTENANCE_VIEWER_WIN_MV_MAINTENANCE_H
