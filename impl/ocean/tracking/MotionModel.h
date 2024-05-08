/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_MOTION_MODEL_H
#define META_OCEAN_TRACKING_MOTION_MODEL_H

#include "ocean/tracking/Tracker.h"

#include "ocean/math/Interpolation.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Pose.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace Tracking
{

/**
 * This class implements a 6DOF pose with internal motion model.
 * @ingroup tracking
 */
class OCEAN_TRACKING_EXPORT MotionModel
{
	public:

		/**
		 * Creates a new but invalid motion model.
		 */
		MotionModel();

		/**
		 * Creates a new pose with a given homogeneous matrix.
		 * The given transformation must be scale and shear free.
		 * @param transformation Matrix defining the pose
		 */
		explicit MotionModel(const HomogenousMatrix4& transformation);

		/**
		 * Creates a new pose by a given translation and orientation.
		 * @param translation Translation of the pose
		 * @param orientation Orientation of the pose
		 */
		MotionModel(const Vector3& translation, const Quaternion& orientation);

		/**
		 * Creates a new pose by a given translation and orientation.
		 * @param translation Translation of the pose
		 * @param orientation Orientation of the pose
		 */
		MotionModel(const Vector3& translation, const Rotation& orientation);

		/**
		 * Updates the model with a new precise pose.
		 * @param pose New pose to updating the model and creating a new prediction
		 */
		void update(const HomogenousMatrix4& pose);

		/**
		 * Returns the current pose of this motion model as transformation matrix.
		 * @return Current pose
		 */
		inline const HomogenousMatrix4& pose() const;

		/**
		 * Returns the predicted pose of this motion model as transformation matrix.
		 * @return Predicted pose
		 */
		inline const HomogenousMatrix4& predictedPose() const;

		/**
		 * Returns the current velocity of this motion model.
		 * @return Current velocity
		 */
		inline const Pose& velocity() const;

		/**
		 * Returns the current velocity of this motion model.
		 * @return Current velocity
		 */
		inline const Pose& predictedVelocity() const;

		/**
		 * Resets the motion model.
		 * All gathered information will be lost.
		 */
		void reset();

		/**
		 * Returns whether no pose has been registered.
		 * @return True, if so
		 */
		inline bool isNull() const;

		/**
		 * Returns whether at least one pose has been registered.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

		/**
		 * Applies a liner (and spherical linear) interpolation between two camera poses by application of an interpolation factor.
		 * @param pose0 The first camera pose that will be returned if an interpolation factor of 0.0 is applied
		 * @param pose1 The second camera pose that will be returned if an interpolation factor of 1.0 is applied
		 * @param factor The interpolation factor with range [0.0, 1.0]
		 * @return The resulting interpolated camera pose
		 */
		static inline HomogenousMatrix4 interpolate(const HomogenousMatrix4& pose0, const HomogenousMatrix4& pose1, const Scalar factor);

		/**
		 * Predicts (extrapolates) the camera pose for a current camera frame, if poses for the previous frames are known.
		 * This function predicts a pose for each two pairs of concurrent poses of the given set of previous poses.<br>
		 * The prediction of the most recent pose pairs is interpolated with the precision of the next pose pairs and so on.<br>
		 * Thus, the influence of the previous poses can be adjusted with the interpolation factor.<br>
		 * A given factor of of 0.4 means that the youngest poses have an influence of 60% while each older pose has an influence of 40% (recursively).<br>
		 * @param previousPoses A concurrent set of valid poses for the previous frames, at least two poses must be provided.
		 * @param factor The interpolation factor that defines the influence of the most recent poses, with range [0.0, 1.0]
		 */
		static HomogenousMatrix4 predictPose(const HomogenousMatrices4& previousPoses, const Scalar factor = Scalar(0.4));

	private:

		/// Current pose.
		HomogenousMatrix4 modelPose;

		/// Predicted pose.
		HomogenousMatrix4 modelPredictedPose;

		/// Current velocity.
		Pose modelVelocity;

		/// Predicted velocity.
		Pose modelPredictedVelocity;

		/// State determining whether at least one pose has been registered
		bool modelHasPose;
};

inline const HomogenousMatrix4& MotionModel::pose() const
{
	return modelPose;
}

inline const HomogenousMatrix4& MotionModel::predictedPose() const
{
	return modelPredictedPose;
}

inline const Pose& MotionModel::velocity() const
{
	return modelVelocity;
}

inline const Pose& MotionModel::predictedVelocity() const
{
	return modelPredictedVelocity;
}

inline bool MotionModel::isNull() const
{
	return !modelHasPose;
}

inline MotionModel::operator bool() const
{
	return modelHasPose;
}

inline HomogenousMatrix4 MotionModel::interpolate(const HomogenousMatrix4& pose0, const HomogenousMatrix4& pose1, const Scalar factor)
{
	return HomogenousMatrix4(Interpolation::linear(std::make_pair(pose0.translation(), pose0.rotation()), std::make_pair(pose1.translation(), pose1.rotation()), factor));
}

}

}

#endif // META_OCEAN_TRACKING_MOTION_MODEL_H
