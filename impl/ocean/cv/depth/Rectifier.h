/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_DEPTH_RECTIFIER_H
#define META_OCEAN_CV_DEPTH_RECTIFIER_H

#include "ocean/cv/depth/Depth.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Quaternion.h"

namespace Ocean
{

namespace CV
{

namespace Depth
{

/**
 * This class implements frame rectification functions.
 * @ingroup cvdepth
 */
class OCEAN_CV_DEPTH_EXPORT Rectifier
{
	public:

		/**
		 * Rectifies two images with given camera profile and camera pose.
		 * @param cameraA The camera profile of the first camera, must be valid
		 * @param cameraB The camera profile of the second camera, must be valid
		 * @param world_T_cameraA The transformation between the first camera and world, with default camera pointing towards the negative z-space with y-axis up
		 * @param world_T_cameraB The transformation between the first camera and world, with default camera pointing towards the negative z-space with y-axis up
		 * @param frameA The first camera image, must be valid
		 * @param frameB The second camera image, must be valid
		 * @param pinholeCamera The camera profile of the pinhole camera to be used when rectifying the images, must be valid
		 * @param rectifiedFrameA The resulting rectified image of the first camera, must be valid
		 * @param rectifiedFrameB The resulting rectified image of the second camera, must be valid
		 * @param world_T_rectifiedA The resulting transformation between the first rectified camera and world, with default camera pointing towards the negative z-space with y-axis up
		 * @param world_T_rectifiedB The resulting transformation between the second rectified camera and world, with default camera pointing towards the negative z-space with y-axis up
		 * @param worker Optional worker object to distribute the computation, nullptr to execute the code on one core only
		 * @return True, if succeeded
		 */
		static bool rectify(const AnyCamera& cameraA, const AnyCamera& cameraB, const HomogenousMatrix4& world_T_cameraA, const HomogenousMatrix4& world_T_cameraB, const Frame& frameA, const Frame& frameB, const PinholeCamera& pinholeCamera, Frame& rectifiedFrameA, Frame& rectifiedFrameB, HomogenousMatrix4& world_T_rectifiedA, HomogenousMatrix4& world_T_rectifiedB, Worker* worker = nullptr);

		/**
		 * Returns the rotation between the rectified pinhole camera(s) and world for two camera poses.
		 * @param world_T_cameraA The transformation between the first camera and world, with default camera pointing towards the negative z-space with y-axis up
		 * @param world_T_cameraB The transformation between the second camera and world, with default camera pointing towards the negative z-space with y-axis up
		 * @param world_R_rectified The resulting transformation between rectified pinhole camera(s) and world, with default camera pointing towards the negative z-space with y-axis up
		 * @return True, if succeeded
		 */
		static bool detemineRectificationRotation(const HomogenousMatrix4& world_T_cameraA, const HomogenousMatrix4& world_T_cameraB, Quaternion& world_R_rectified);
};

}

}

}

#endif // META_OCEAN_CV_DEPTH_RECTIFIER_H
