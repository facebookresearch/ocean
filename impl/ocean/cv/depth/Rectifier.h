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
		 * @param useTangentMapping Whether to use tangent mapping to better preserve the original resolution
		 * @param worker Optional worker object to distribute the computation, nullptr to execute the code on one core only
		 * @return True, if succeeded
		 */
		static bool rectify(const AnyCamera& cameraA, const AnyCamera& cameraB, const HomogenousMatrix4& world_T_cameraA, const HomogenousMatrix4& world_T_cameraB, const Frame& frameA, const Frame& frameB, const PinholeCamera& pinholeCamera, Frame& rectifiedFrameA, Frame& rectifiedFrameB, HomogenousMatrix4& world_T_rectifiedA, HomogenousMatrix4& world_T_rectifiedB, const bool useTangentMapping, Worker* worker = nullptr);

		/**
		 * Returns the rotation between the rectified pinhole camera(s) and world for two camera poses.
		 * @param world_T_cameraA The transformation between the first camera and world, with default camera pointing towards the negative z-space with y-axis up
		 * @param world_T_cameraB The transformation between the second camera and world, with default camera pointing towards the negative z-space with y-axis up
		 * @param world_R_rectified The resulting transformation between rectified pinhole camera(s) and world, with default camera pointing towards the negative z-space with y-axis up
		 * @return True, if succeeded
		 */
		static bool detemineRectificationRotation(const HomogenousMatrix4& world_T_cameraA, const HomogenousMatrix4& world_T_cameraB, Quaternion& world_R_rectified);

		/**
		 * Re-samples a camera image which has been captured with a camera profile as if the image would have been captured with a second camera profile.
		 * Optionally applies tangent mapping to the target image to better preserve the original resolution.
		 * The function can be used e.g., to rectify a fisheye camera image into a pinhole camera image.
		 * @param sourceFrame The source image captured with the source camera profile, must be valid
		 * @param sourceCamera The source camera profile which has been used to capture the source image, with resolution sourceFrame.width() x sourceFrame.height(), must be valid
		 * @param source_R_target The rotation transforming 3D points defined in the coordinate system of the target camera image to 3D points defined in the coordinate system of the source camera image, must be valid
		 * @param targetCamera The camera profile of the target frame, must be valid
		 * @param targetFrame The resulting target image, with resolution targetCamera.width() x targetCamera.height(), must be valid
		 * @param source_OLT_target Optional resulting offset lookup table between target image points and source image points
		 * @param worker Optional worker object to distribute the computational load
		 * @param binSizeInPixel The size in pixel of the interpolation bins used for building the lookup table, with range [1, infinity)
		 * @param borderColor The color of border pixels for which now visual content exists, provide one value for each channel, nullptr to use ElementType(0) for each channel
		 * @return True, if succeeded
		 * @see FrameInterpolatorBilinear::Comfort::resampleCameraImage().
		 */
		static bool resampleCameraImageWithOptionalTangentMapping(const Frame& sourceFrame, const AnyCamera& sourceCamera, const SquareMatrix3& source_R_target, const AnyCamera& targetCamera, Frame& targetFrame, LookupCorner2<Vector2>* source_OLT_target = nullptr, Worker* worker = nullptr, const unsigned int binSizeInPixel = 8u, const void* borderColor = nullptr, const bool useTangentMapping = false);

};

}

}

}

#endif // META_OCEAN_CV_DEPTH_RECTIFIER_H
