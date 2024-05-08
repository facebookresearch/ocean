/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_VISUAL_TRACKER_H
#define META_OCEAN_TRACKING_VISUAL_TRACKER_H

#include "ocean/tracking/Tracking.h"
#include "ocean/tracking/MotionModel.h"
#include "ocean/tracking/Tracker.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"

#include <vector>

namespace Ocean
{

namespace Tracking
{

// Forward declaration.
class VisualTracker;

/**
 * Definition of an object reference covering a visual tracker object.
 * @see VisualTracker.
 * @ingroup tracking
 */
typedef ObjectRef<VisualTracker> VisualTrackerRef;

/**
 * This class implements a base class for all visual tracker objects.
 * @ingroup tracking
 */
class OCEAN_TRACKING_EXPORT VisualTracker : public Tracker
{
	public:

		/**
		 * Definition of an object id.
		 */
		typedef unsigned int ObjectId;

		/**
		 * Definition of a simple tracking sample combining a tracking object id with a transformation.
		 */
		class TransformationSample
		{
			public:

				/**
				 * Creates a new sample object with in valid parameters.
				 */
				TransformationSample() = default;

				/**
				 * Creates a new sample object.
				 * @param transformation The transformation of the new sample
				 * @param id The object id of the new sample
				 */
				inline TransformationSample(const HomogenousMatrix4& transformation, const ObjectId id);

			public:

				/**
				 * Returns the transformation of this sample.
				 * @return Sample transformation
				 */
				inline const HomogenousMatrix4& transformation() const;

				/**
				 * Returns the id of this sample.
				 * @return Sample id
				 */
				inline unsigned int id() const;

			protected:

				/// The sample's transformation.
				HomogenousMatrix4 transformation_ = HomogenousMatrix4(false);

				/// The sample's object id.
				ObjectId id_ = ObjectId(-1);
		};

		/**
		 * Definition of a vector holding a transformation sample object.
		 */
		typedef std::vector<TransformationSample> TransformationSamples;

	public:

		/**
		 * Returns the maximal expected pose position offset between two successive frames for this tracker.
		 * @return Maximal pose position offset separately for each axis
		 */
		inline const Vector3& maximalPositionOffset() const;

		/**
		 * Returns the maximal expected pose orientation offset between two successive frames for this tracker.
		 * @return Maximal pose orientation offset in radian
		 */
		inline Scalar maximalOrientationOffset() const;

		/**
		 * Sets or changes the maximal expected pose position offset between two successive frames for this tracker.
		 * @param positionOffset New position offset to be set for each axis separately, with range (0, infinity)
		 * @return True, if succeeded
		 */
		virtual bool setMaxPositionOffset(const Vector3& positionOffset);

		/**
		 * Sets or changes the maximal expected pose orientation offset between two successive frames for this tracker.
		 * @param orientationOffset New orientation offset to be set in radian, with range (0, PI)
		 * @return True, if succeeded
		 */
		virtual bool setMaxOrientationOffset(const Scalar orientationOffset);

		/**
		 * Executes the tracking step for a collection of frames and corresponding cameras
		 * This function allows to specify an absolute orientation 'absoluteOrientation' provided by e.g., an IMU sensor.<br>
		 * This orientation can be defined in relation to an independent coordinate system not related with the tracking objects (as long as this coordinate system does not change between successive calls).<br>
		 * The tracker can use the provided orientation to improve tracking robustness.
		 * @note The base implementation will only accept a single frame and camera and will return false for multiple frames and cameras. If the camera type is not a pinhole camera, the input frame will be undistorted and the camera will be converted to a pinhole camera, which is an expensive operation. For customization this function needs to be overriden.
		 * @note Once the deprecated function below has been removed, this function will become purely virtual. For any derived class, it is strongly suggested to provide an override of this function.
		 * @param frames The frames to be used for tracking, must have at least one element and have same number of elements as `anyCameras`, all elements must be valid
		 * @param anyCameras The camera objects associated with the frames, with width and height must match that of each corresponding frame, must have same number of elements as `frames`, all elements must be valid
		 * @param transformations The resulting 6DOF poses combined with the tracking IDs
		 * @param world_R_camera An optional absolute orientation of the camera in the moment the frame was taken, defined in a coordinate system (e.g., world) not related with the tracking objects, an invalid object otherwise
		 * @param worker An optional worker object
		 * @return True, if succeeded
		 */
		virtual bool determinePoses(const Frames& frames, const SharedAnyCameras& anyCameras, TransformationSamples& transformations, const Quaternion& world_R_camera = Quaternion(false), Worker* worker = nullptr);

		/**
		 * Deprecated.
		 *
		 * Executes the tracking for a given frame.
		 * This function allows to specify an absolute orientation 'absoluteOrientation' provided by e.g., an IMU sensor.<br>
		 * This orientation can be defined in relation to an independent coordinate system not related with the tracking objects (as long as this coordinate system does not change between successive calls).<br>
		 * The tracker can use the provided orientation to improve tracking robustness.
		 * @param frame The frame to be used for tracking, must be valid
		 * @param pinholeCamera The pinhole camera object associated with the frame, with width and height matching with the frame's resolution
		 * @param frameIsUndistorted True, if the original input frame is undistorted and thus feature must not be undistorted explicitly
		 * @param transformations Resulting 6DOF poses combined with the tracking ids
		 * @param world_R_camera Optional absolute orientation of the camera in the moment the frame was taken, defined in a coordinate system (e.g., world) not related with the tracking objects, an invalid object otherwise
		 * @param worker Optional worker object
		 * @return True, if succeeded
		 */
		virtual bool determinePoses(const Frame& frame, const PinholeCamera& pinholeCamera, const bool frameIsUndistorted, TransformationSamples& transformations, const Quaternion& world_R_camera = Quaternion(false), Worker* worker = nullptr) = 0;

	protected:

		/**
		 * Creates a new visual tracker object.
		 */
		VisualTracker() = default;

	protected:

		/// Pose motion model to predict the pose of the next frame.
		MotionModel motionModel_;

		/// Maximal pose position offset between two frames.
		Vector3 maxPositionOffset_ = Vector3(Scalar(0.08), Scalar(0.08), Scalar(0.08));

		/// Maximal pose orientation offset between two frames, in radian angle.
		Scalar maxOrientationOffset_ = Numeric::deg2rad(15);

		/// Tracker lock object.
		mutable Lock lock_;
};

inline VisualTracker::TransformationSample::TransformationSample(const HomogenousMatrix4& transformation, const ObjectId id) :
	transformation_(transformation),
	id_(id)
{
	// nothing to do here
}

inline const HomogenousMatrix4& VisualTracker::TransformationSample::transformation() const
{
	return transformation_;
}

inline VisualTracker::ObjectId VisualTracker::TransformationSample::id() const
{
	return id_;
}

inline const Vector3& VisualTracker::maximalPositionOffset() const
{
	return maxPositionOffset_;
}

inline Scalar VisualTracker::maximalOrientationOffset() const
{
	return maxOrientationOffset_;
}

}

}

#endif // META_OCEAN_TRACKING_VISUAL_TRACKER_H
