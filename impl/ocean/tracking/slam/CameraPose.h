/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_SLAM_CAMERA_POSE_H
#define META_OCEAN_TRACKING_SLAM_CAMERA_POSE_H

#include "ocean/tracking/slam/SLAM.h"

#include "ocean/math/Camera.h"
#include "ocean/math/HomogenousMatrix4.h"

namespace Ocean
{

namespace Tracking
{

namespace SLAM
{

// Forward declaration.
class LocalizedObjectPoint;

// Forward declaration.
class CameraPose;

/**
 * Definition of a shared pointer holding a CameraPose object.
 * @see CameraPose
 * @ingroup trackingslam
 */
using SharedCameraPose = std::shared_ptr<CameraPose>;

/**
 * This class holds the camera pose of a camera in relation to the world.
 * The pose includes both the standard camera-to-world transformation and the flipped camera-to-world transformation.<br>
 * Additionally, each pose stores a quality indicator reflecting the reliability of the pose estimation, and an estimated motion type indicating whether the camera is stationary or moving.<br>
 * Each pose is tagged with a map version to ensure consistency with the SLAM feature map it was computed against.
 * @ingroup trackingslam
 */
class OCEAN_TRACKING_SLAM_EXPORT CameraPose
{
	friend class CameraPoses;

	public:

		/**
		 * Definition of the quality of a camera pose.
		 */
		enum PoseQuality : uint32_t
		{
			/// The pose is invalid.
			PQ_INVALID = 0u,
			/// The pose has a low quality as it is based on few feature correspondences.
			PQ_LOW,
			/// The pose has a medium quality as it is based on a medium number of feature correspondences.
			PQ_MEDIUM,
			/// The pose is accurate as it is based on a full bundle adjustment.
			PQ_HIGH
		};

		/**
		 * Definition of different types of estimated motion.
		 */
		enum EstimatedMotion : uint32_t
		{
			/// The motion is unknown.
			EM_UNKNOWN = 0u,
			/// The camera is stationary (not moving).
			EM_STATIONARY,
			/// The camera is undergoing translational motion.
			EM_TRANSLATIONAL
		};

	public:

		/**
		 * Creates a new invalid camera pose.
		 */
		CameraPose() = default;

		/**
		 * Creates a new camera pose.
		 * @param world_T_camera The transformation between camera and world, with default camera pointing towards the negative z-space and y-axis upwards, must be valid
		 * @param flippedCamera_T_world The transformation between world and flipped camera, with default flipped camera pointing towards the positive z-space and y-axis downwards, must be valid
		 * @param poseQuality The quality of the pose, must be valid
		 * @param estimatedMotion The estimated motion type of the camera, EM_UNKNOWN if unknown
		 */
		inline CameraPose(const HomogenousMatrix4& world_T_camera, const HomogenousMatrix4& flippedCamera_T_world, const PoseQuality poseQuality, const EstimatedMotion estimatedMotion = EM_UNKNOWN);

		/**
		 * Creates a new camera pose.
		 * This constructor calculates the transformation between world and flipped camera.
		 * @param world_T_camera The transformation between camera and world, with default camera pointing towards the negative z-space and y-axis upwards, must be valid
		 * @param poseQuality The quality of the pose, must be valid
		 * @param estimatedMotion The estimated motion type of the camera, EM_UNKNOWN if unknown
		 */
		inline CameraPose(const HomogenousMatrix4& world_T_camera, const PoseQuality poseQuality, const EstimatedMotion estimatedMotion = EM_UNKNOWN);

		/**
		 * Returns the transformation between camera and world.
		 * The default camera points towards the negative z-space with y-axis upwards.
		 * @return The camera-to-world transformation
		 */
		inline const HomogenousMatrix4& world_T_camera() const;

		/**
		 * Returns the transformation between world and flipped camera.
		 * The default flipped camera points towards the positive z-space with y-axis downwards.
		 * @return The world-to-flipped-camera transformation
		 */
		inline const HomogenousMatrix4& flippedCamera_T_world() const;

		/**
		 * Returns the quality of this pose.
		 * @return The pose quality
		 */
		inline PoseQuality poseQuality() const;

		/**
		 * Returns the estimated motion type of the camera.
		 * @return The estimated motion type
		 */
		inline EstimatedMotion estimatedMotion() const;

		/**
		 * Returns the version of the map used when this pose was computed.
		 * The map version is used to ensure consistency between poses and the feature map they were computed against.
		 * @return The map version, with range [0, infinity), -1 if not yet set
		 */
		inline Index32 mapVersion() const;

		/**
		 * Returns whether this camera pose is valid.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Returns whether two camera poses are identical.
		 * @param right The right camera pose to compare
		 * @return True, if so
		 */
		bool operator==(const CameraPose& right) const = default;

		/**
		 * Translates the given pose quality to a string.
		 * @param poseQuality The pose quality to translate
		 * @return The translated string, 'Invalid' if invalid or unknown
		 */
		static std::string translatePoseQuality(const PoseQuality poseQuality);

		/**
		 * Determines the estimated motion type from optical flow data.
		 * @param imagePointSqrDistances The squared distances of optical flow vectors, must be valid
		 * @param size The number of elements in imagePointSqrDistances, with range [1, infinity)
		 * @param width The width of the image in pixels, with range [1, infinity)
		 * @param height The height of the image in pixels, with range [1, infinity)
		 * @return The estimated motion type based on the optical flow analysis
		 */
		static EstimatedMotion motionFromOpticalFlow(const Scalar* imagePointSqrDistances, const size_t size, const unsigned int width, const unsigned int height);

	protected:

		/**
		 * Sets the version of the map used when this pose was computed.
		 * @param mapVersion The map version to set, with range [0, infinity)
		 */
		inline void setMapVersion(const Index32 mapVersion);

	protected:

		/// The transformation between camera and world, with default camera pointing towards the negative z-space and y-axis upwards.
		HomogenousMatrix4 world_T_camera_ = HomogenousMatrix4(false);

		/// The transformation between world and flipped camera, with default flipped camera pointing towards the positive z-space and y-axis downwards.
		HomogenousMatrix4 flippedCamera_T_world_ = HomogenousMatrix4(false);

		/// The quality of the pose.
		PoseQuality poseQuality_ = PQ_INVALID;

		/// The estimated motion type of the camera.
		EstimatedMotion estimatedMotion_ = EM_UNKNOWN;

		/// The version of the map used when this pose was computed.
		Index32 mapVersion_ = Index32(-1);
};

inline CameraPose::CameraPose(const HomogenousMatrix4& world_T_camera, const HomogenousMatrix4& flippedCamera_T_world, const PoseQuality poseQuality, const EstimatedMotion estimatedMotion) :
	world_T_camera_(world_T_camera),
	flippedCamera_T_world_(flippedCamera_T_world),
	poseQuality_(poseQuality),
	estimatedMotion_(estimatedMotion)
{
	ocean_assert(isValid());
}

inline CameraPose::CameraPose(const HomogenousMatrix4& world_T_camera, const PoseQuality poseQuality, const EstimatedMotion estimatedMotion) :
	CameraPose(world_T_camera, Camera::standard2InvertedFlipped(world_T_camera), poseQuality, estimatedMotion)
{
	// nothing to do here
}

inline const HomogenousMatrix4& CameraPose::world_T_camera() const
{
	return world_T_camera_;
}

inline const HomogenousMatrix4& CameraPose::flippedCamera_T_world() const
{
	return flippedCamera_T_world_;
}

inline CameraPose::PoseQuality CameraPose::poseQuality() const
{
	return poseQuality_;
}

inline CameraPose::EstimatedMotion CameraPose::estimatedMotion() const
{
	return estimatedMotion_;
}

inline Index32 CameraPose::mapVersion() const
{
	return mapVersion_;
}

inline void CameraPose::setMapVersion(const Index32 mapVersion)
{
	mapVersion_ = mapVersion;
}

inline bool CameraPose::isValid() const
{
	ocean_assert((poseQuality_ == PQ_INVALID && !world_T_camera_.isValid()) || (poseQuality_ != PQ_INVALID && world_T_camera_.isValid()));

	if (poseQuality_ == PQ_INVALID)
	{
		return false;
	}

	ocean_assert(world_T_camera_.isEqual(Camera::invertedFlipped2Standard(flippedCamera_T_world_), Numeric::weakEps()));

	return true;
}

}

}

}

#endif // META_OCEAN_TRACKING_SLAM_CAMERA_POSE_H
