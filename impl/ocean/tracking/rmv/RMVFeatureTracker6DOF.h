/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_RMV_RMV_TRACKER_6DOF_H
#define META_OCEAN_TRACKING_RMV_RMV_TRACKER_6DOF_H

#include "ocean/tracking/rmv/RMV.h"
#include "ocean/tracking/rmv/RMVFeatureDetector.h"
#include "ocean/tracking/rmv/RMVFeatureMap.h"

#include "ocean/base/Accessor.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/Thread.h"

#include "ocean/geometry/Geometry.h"

#include "ocean/math/AnyCamera.h"

#include "ocean/tracking/MotionModel.h"
#include "ocean/tracking/PoseProjection.h"
#include "ocean/tracking/VisualTracker.h"

namespace Ocean
{

namespace Tracking
{

namespace RMV
{

/**
 * This class implements a RMV feature tracker.
 * @ingroup trackingrmv
 */
class OCEAN_TRACKING_RMV_EXPORT RMVFeatureTracker6DOF :
	public VisualTracker,
	private Thread
{
	public:

		/**
		 * Creates a new RMV feature tracker object.
		 * @param detectorType The type of the feature detector to be sued
		 */
		explicit RMVFeatureTracker6DOF(const RMVFeatureDetector::DetectorType detectorType);

		/**
		 * Destructs a RMV feature tracker object.
		 */
		~RMVFeatureTracker6DOF();

		/**
		 * Returns the feature detector type of this tracker.
		 * @return Detector type
		 */
		inline RMVFeatureDetector::DetectorType detectorType() const;

		/**
		 * Returns the feature map of this tracker.
		 * @return Tracker feature map
		 */
		inline const RMVFeatureMap& featureMap() const;

		/**
		 * Sets or changes the feature map for this tracker.
		 * @param featureMap Feature map to be set
		 * @param randomGenerator Random number generator to be used
		 * @param autoUpdateMaxPositionOffset State determining whether the maximal position offset between two frame will be adjusted to the feature map size automatically
		 */
		void setFeatureMap(const RMVFeatureMap& featureMap, RandomGenerator& randomGenerator, const bool autoUpdateMaxPositionOffset = true);

		/**
		 * Executes the tracking step for a collection of frames and corresponding cameras
		 * @see VisualTracker::determinePoses().
		 */
		bool determinePoses(const Frames& frames, const SharedAnyCameras& anyCameras, TransformationSamples& transformations, const Quaternion& world_R_camera = Quaternion(false), Worker* worker = nullptr) override;

		/**
		 * Deprecated.
		 *
		 * Executes the tracking for a given frame.
		 * @see VisualTracker::determinePoses().
		 */
		bool determinePoses(const Frame& frame, const PinholeCamera& camera, const bool frameIsUndistorted, TransformationSamples& transformations, const Quaternion& previousCamera_R_camera = Quaternion(false), Worker* worker = nullptr) override;

	private:

		/**
		 * Determines the camera pose for a given frame (should be the current frame).
		 * @param frame The frame which will be used for pose determination, must be valid
		 * @param camera The camera profile defining the projection, with same dimension as the given frame
		 * @param frameIsUndistorted True, if the original input frame is undistorted and thus feature must not be undistorted explicitly
		 * @param world_T_camera The resulting camera pose
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		bool internDeterminePose(const Frame& frame, const AnyCamera& camera, const bool frameIsUndistorted, HomogenousMatrix4& world_T_camera, Worker* worker);

		/**
		 * Determines the camera pose if no reliable knowledge is given from any previous camera frame.
		 * This function applies a multi-scale/pyramid approach for pose estimation.
		 * @param frame The frame which will be used for pose determination, must be valid
		 * @param camera The camera profile defining the projection, must be valid
		 * @param world_T_camera The resulting camera pose if the pose could be determined
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		bool determinePoseWithoutKnowledgePyramid(const Frame& frame, const AnyCamera& camera, HomogenousMatrix4& world_T_camera, Worker* worker);

		/**
		 * Determines the camera pose if no reliable knowledge is given from any previous camera frame.
		 * This function does not apply any multi-scale/pyramid approach for pose estimation.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param imagePoints The feature points that have been determined in the current camera frame, should be sorted according to the strength of the features, at least 20
		 * @param world_T_camera The resulting camera pose if the pose could be determined
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		bool determinePoseWithoutKnowledgeDefault(const AnyCamera& camera, const Vectors2& imagePoints, HomogenousMatrix4& world_T_camera, Worker* worker);

		/**
		 * Determines a precise pose based on a rough camera pose and mainly based on strong feature correspondences from the previous frame.
		 * @param world_T_roughCamera The already known rough camera pose for the current frame, must be valid
		 * @param camera The camera profile defining the projection, must be valid
		 * @param imagePoints The image points (strong feature points) that has been determined in the current camera frame, at least 10, will be empty if this function succeeds
		 * @param world_T_camera The resulting precise pose, which will be very accurate
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 * @see determinePoseWithAnyPreviousCorrespondences().
		 */
		bool determinePoseWithStrongPreviousCorrespondences(const HomogenousMatrix4& world_T_roughCamera, const AnyCamera& camera, Vectors2& imagePoints, HomogenousMatrix4& world_T_camera, Worker* worker);

		/**
		 * Determines a precise pose based on a rough camera pose and mainly based on any feature correspondences from the previous frame.
		 * This function does not take strong correspondences (from the previous frame) but any as this function should be the backup solution for the strong-correspondence-solution.
		 * @param world_T_roughCamera The already known rough camera pose for the current frame, must be valid
		 * @param camera The camera profile defining the projection, must be valid
		 * @param imagePoints The image points (strong feature points) that has been determined in the current camera frame, at least 10, will be empty if this function succeeds
		 * @param world_T_camera The resulting precise pose, which will be very accurate
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 * @see determinePoseWithStrongPreviousCorrespondences().
		 */
		bool determinePoseWithAnyPreviousCorrespondences(const HomogenousMatrix4& world_T_roughCamera, const AnyCamera& camera, Vectors2& imagePoints, HomogenousMatrix4& world_T_camera, Worker* worker);

		/**
		 * Determines the precise camera pose based on an already known rough camera pose.
		 * @param world_T_roughCamera The already known rough camera pose, must be valid
		 * @param camera The camera profile defining the projection, must be valid
		 * @param imagePoints The image points (strong feature points) for which the precise camera pose will be determined, at least 5, will be empty if this function succeeds
		 * @param world_T_camera The resulting precise pose
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		bool determinePoseWithRoughPose(const HomogenousMatrix4& world_T_roughCamera, const AnyCamera& camera, Vectors2& imagePoints, HomogenousMatrix4& world_T_camera, Worker* worker);

		/**
		 * Refines a rough camera pose by application of strong feature correspondences from the previous frame.
		 * This function tries to re-find/determine strong and reliable 2D/3D feature correspondences from the previous frame within two stages.<br>
		 * The resulting pose will be significantly better/more precise than provided rough pose but still need improvements.
		 * @param roughFlippedCamera_T_world The already known rough inverted and flipped camera pose for the current frame, must be valid
		 * @param camera The camera profile defining the projection, must be valid
		 * @param imagePoints The image points (strong feature points) that has been determined in the current camera frame, at least 10
		 * @param flippedCamera_T_world The resulting improved inverted and flipped pose, which may still not be very accurate
		 * @return True, if succeeded
		 */
		bool refinePoseWithStrongPreviousCorrespondencesIF(const HomogenousMatrix4& roughFlippedCamera_T_world, const AnyCamera& camera, const Vectors2& imagePoints, HomogenousMatrix4& flippedCamera_T_world);

		/**
		 * Detects feature points in a given frame optional within a defined sub-region.
		 * @param yFrame The frame in which the feature points will be detected, must have pixel format FORMAT_Y8
		 * @param frameIsUndistorted True, if the original input frame is undistorted and thus feature must not be undistorted explicitly
		 * @param boundingBox The bounding box defining the sub-region inside the frame in which the feature points will be detected, with range (-infinity, infinity)x(-infinity, infinity), and invalid bounding box to detect features in the entire frame
		 * @param worker Optional worker object to distribute the computation
		 */
		Vectors2 detectFeatures(const Frame& yFrame, const bool frameIsUndistorted, const Box2& boundingBox = Box2(), Worker* worker = nullptr);

		/**
		 * Returns whether the data that is processed asynchronously (between two successive camera frame - directly after a camera pose could be determined) is available already.
		 * @return True, if so
		 * @see startAsynchronousDataProcessingIF().
		 */
		bool asynchronousDataProcessed();

		/**
		 * Starts the asynchronous data processing for precise (inverted and flipped) pose an the corresponding image points.
		 * The process is asynchronous as the resulting information is necessary for the next camera frame (to simplify the tracking).<br>
		 * Due to performance reasons, the processing is not done in the moment the next frame is handled but in the meantime.
		 * @param flippedCamera_T_world The precise inverted and flipped camera pose to be used for the data processing
		 * @param camera The camera profile defining the projection
		 * @param imagePoints The image points to be used for the data processing, will be moved
		 * @see asynchronousDataProcessed().
		 */
		void startAsynchronousDataProcessingIF(const HomogenousMatrix4& flippedCamera_T_world, const AnyCamera& camera, Vectors2&& imagePoints);

		/**
		 * Thread run function.
		 */
		void threadRun() override;

		/**
		 * Determines the used 3D object points using the final most accurate pose possible.
		 * @param fineFlippedCamera_T_world The precise inverted and flipped camera pose to be used
		 * @param camera The camera object to be used
		 * @param imagePoints Current detected image points
		 * @param minimalStrongObjectPoints The minimal number of strong object points that need to be found to avoid the determination of semi-strong features, with range [1, infinity)
		 * @param strongObjectPointIndices Resulting indices of valid and strong object points from the feature map (strongly matching to the image points and the given pose), with ascending order
		 * @param moderateObjectPointIndices Optional resulting indices of valid and moderate/semi-strong object points from the feature map (only in the case not enough strong features could be found regarding to the specified minimal number), with ascending order
		 * @param usedObjectPointIndices Resulting indices of valid object points from the feature map (weakly matching to the image points and the given pose), with ascending order
		 * @return True, if succeeded
		 */
		bool determineUsedFeaturesIF(const HomogenousMatrix4& fineFlippedCamera_T_world, const AnyCamera& camera, const Vectors2& imagePoints, const size_t minimalStrongObjectPoints, Indices32& strongObjectPointIndices, Indices32& moderateObjectPointIndices, Indices32& usedObjectPointIndices);

		/**
		 * Adds unique and reliable 2D/3D correspondences based on known reliable object points (e.g., from the previous frame) and a uniqueness term.
		 * @param roughFlippedCamera_T_world The already known rough inverted and flipped camera pose for the current frame, must be valid
		 * @param camera The camera profile defining the projection, must be valid
		 * @param imagePoints The image points (strong feature points) that has been determined in the current camera frame, must be valid
		 * @param numberImagePoints The number of provided image points, with range [1, infinity)
		 * @param objectPointAccessor The accessor providing all reliable object points for which the corresponding image points will be determined, at least one
		 * @param searchWindow Size of the search window (as 'radius') in which possible candidate points will be investigated, in pixel, with range (0, infinity)
		 * @param uniquenessSqrFactor Factor to distinguish between a unique and non-unique correspondence, with range (0, infinity)
		 * @param resultingObjectPoints The object points to which the new determined object points (from unique correspondences) will be added
		 * @param resultingImagePoints the image points to which the new determined image points (from unique correspondences) will be added
		 */
		static void addUniqueCorrespondencesIF(const HomogenousMatrix4& roughFlippedCamera_T_world, const AnyCamera& camera, const Vector2* imagePoints, const size_t numberImagePoints, const ConstIndexedAccessor<Vector3>& objectPointAccessor, const Scalar searchWindow, const Scalar uniquenessSqrFactor, Vectors3& resultingObjectPoints, Vectors2& resultingImagePoints);

		/**
		 * Refines a rough camera pose by application of guessed feature correspondences between projected object points and image points visible in the current camera frame.
		 * The feature correspondences a guessed due to uniqueness terms.
		 * @param roughFlippedCamera_T_world The already known rough inverted flipped camera pose for the current frame, must be valid
		 * @param camera The camera profile defining the projection, must be valid
		 * @param imagePoints The image points (strong feature points) that has been determined in the current camera frame, at least 10
		 * @param objectPoints The object points that will be used for pose determination, can be the entire set of object points from the feature map or a subset with special properties (e.g., very strong ones), at least 10
		 * @param flippedCamera_T_world The resulting inverted flipped precise camera pose
		 * @param useNumberImagePoints The maximal number of image points that will be used for tracking - so that the (first) strongest image points will be used only, with range [10, infinity)
		 * @param useNumberObjectPoints The maximal number of object points that will be used for tracking, so that the (first) strong object points will be used only, with range [10, infinity)
		 * @param searchWindow Size of the search window (as 'radius') in which possible candidate points will be investigated, in pixel, with range (0, infinity)
		 * @param uniquenessSqrFactor Factor to distinguish between a unique and non-unique correspondence, with range (0, infinity)
		 * @param maxSqrDistance The maximal square distance for a unique correspondence, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return The number of points correspondences that have been used during the pose refinement, with range [5, infinity), 0 if the pose refinement failed
		 */
		static unsigned int refinePoseIF(const HomogenousMatrix4& roughFlippedCamera_T_world, const AnyCamera& camera, const Vectors2& imagePoints, const Vectors3& objectPoints, HomogenousMatrix4& flippedCamera_T_world, const unsigned int useNumberImagePoints, const unsigned int useNumberObjectPoints, const Scalar searchWindow, const Scalar uniquenessSqrFactor, const Scalar maxSqrDistance = Numeric::maxValue(), Worker* worker = nullptr);

		/**
		 * Returns a size factor that reflects the size of the current camera resolution.
		 * The larger the camera dimension the larger the factor, the default camera dimension is 640x480 with a resulting factor of 1.
		 * @return The size factor, with range (0, infinity)
		 */
		static inline Scalar cameraResolutionFactor(const AnyCamera& camera);

	private:

		/// Feature map of this tracker.
		RMVFeatureMap trackerFeatureMap_;

		/// Pose projection set.
		PoseProjectionSet trackerPoseProjectionSet_;

		/// The type of the feature detector to be used.
		RMVFeatureDetector::DetectorType trackerDetectorType_;

		/// The current strength threshold for the feature tracker, will be adjusted as time goes by, with range [0, infinity)
		Scalar trackerFeatureDetectorStrength_;

		/// Maximal number of feature points to be used for the pose projections, with range [10, infinity)
		size_t trackerMaximalPoseProjectionFeatureNumber_;

		/// The radius defining the circle around each strong feature point not containing more than one projected object point so that a point correspondence counts as strong, in pixels, with range (trackerSemiStrongCorrespondencesEmptyAreaRadius_, infinity)
		Scalar trackerStrongCorrespondencesEmptyAreaRadius_;

		/// The radius defining the circle around each semi-strong feature point not containing more than one projected object point so that a point correspondence counts as semi-strong, in pixels, with range [0, trackerStrongCorrespondencesEmptyAreaRadius_)
		Scalar trackerSemiStrongCorrespondencesEmptyAreaRadius_;

		/// The random number generator object to be used.
		RandomGenerator trackerRandomGenerator_;

		/// True, if the asynchronous data processing function is currently active.
		bool trackerAsynchronousDataProcessingActive_;

		/// The inverted and flipped camera pose to be used during the asynchronous data processing function.
		HomogenousMatrix4 trackerAsynchronousDataProcessingFlippedCamera_T_world_;

		/// The camera profile to be used during the asynchronous data processing function.
		SharedAnyCamera trackerAsynchronousDataProcessingCamera_;

		/// The image points to be used during the asynchronous data processing function.
		Vectors2 trackerAsynchronousDataProcessingImagePoints_;

		/// The signal used to invoke the asynchronous data processing function.
		Signal trackerAsynchronousSignal_;

		/// The lock for the asynchronous data processing function.
		Lock trackerAsynchronousDataProcessingLock_;
};

inline RMVFeatureDetector::DetectorType RMVFeatureTracker6DOF::detectorType() const
{
	return trackerDetectorType_;
}

inline const RMVFeatureMap& RMVFeatureTracker6DOF::featureMap() const
{
	return trackerFeatureMap_;
}

inline Scalar RMVFeatureTracker6DOF::cameraResolutionFactor(const AnyCamera& camera)
{
	ocean_assert(camera.isValid());

	return Vector2(Scalar(camera.width()), Scalar(camera.height())).length() * Scalar(0.00125); // 1/800 = sqrt(640^2 + 480^2)
}

}

}

}

#endif // META_OCEAN_TRACKING_RMV_RMV_TRACKER_6DOF_H
