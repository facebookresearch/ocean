/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_OFFLINE_STRUCTURE_FROM_MOTION_WIN_SFM_MAINTENANCE_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_OFFLINE_STRUCTURE_FROM_MOTION_WIN_SFM_MAINTENANCE_H

#include "application/ocean/demo/tracking/offline/structurefrommotion/win/SFMApplication.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Maintenance.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Vector2.h"
#include "ocean/math/Vector3.h"

/**
 * This class provides maintenance data helper functions.
 * @ingroup applicationdemotrackingofflinestructurefrommotionwin
 */
class SFMMaintenance
{
	public:

		/**
		 * Definition of a vector holding 8 bit elements.
		 */
		typedef std::vector<uint8_t> Buffer;

	public:

		/**
		 * Encodes the tracking environment composes of a frame mesh (a frame with correspondences of 2D image points and 3D object points), a camera pose from which the frame has been captured and an independent set of 3D object points.<br>
		 * @param frame The frame to encode
		 * @param frameImagePoints The image points located in the given frame
		 * @param frameObjectPoints The object points, one object point for each image point
		 * @param framePose The camera pose to encode
		 * @param objectPoints The independent object points to encode
		 * @return The encoded buffer
		 */
		static Maintenance::Buffer encodeEnvironment(const Frame& frame, const Vectors2& frameImagePoints, const Vectors3& frameObjectPoints, const HomogenousMatrix4& framePose, const Vectors3& objectPoints);

		/**
		 * Encodes a frame to a buffer and adds further control parameters to the buffer.
		 * @param frame The frame to encode
		 * @param imageType Optional type of the image, e.g. bmp, jpg, png
		 * @param buffer The resulting buffer
		 */
		static void encodeFrame(const Frame& frame, const std::string& imageType, Buffer& buffer);
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_OFFLINE_STRUCTURE_FROM_MOTION_WIN_SFM_MAINTENANCE_H
