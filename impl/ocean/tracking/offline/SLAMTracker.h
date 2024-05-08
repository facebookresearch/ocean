/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_OFFLINE_SLAM_TRACKER_H
#define META_OCEAN_TRACKING_OFFLINE_SLAM_TRACKER_H

#include "ocean/tracking/offline/Offline.h"
#include "ocean/tracking/offline/FrameTracker.h"
#include "ocean/tracking/offline/PointPaths.h"

#include "ocean/base/RandomGenerator.h"
#include "ocean/base/Scheduler.h"

#include "ocean/math/PinholeCamera.h"

#include "ocean/tracking/Database.h"
#include "ocean/tracking/Solver3.h"

namespace Ocean
{

namespace Tracking
{

namespace Offline
{

// Forward declaration.
class SLAMTracker;

/**
 * Definition of an object reference holding a PlanarRectangleTracker object.
 * @see PlanarRectangleTracker.
 * @ingroup trackingoffline
 */
typedef SmartObjectRef<SLAMTracker, OfflineTracker> SLAMTrackerRef;

/**
 * This class implements a SLAM (Simultaneous Localization and Mapping) tracker for arbitrary environments and camera movements.
 * The tracker determines camera poses and the location of 3D object points concurrently only due to the positions of corresponding image points in individual camera frames.<br>
 * The tracker mainly extracts all necessary information from a database providing valid image point positions and a valid topology between image points, object points and camera poses.<br>
 * However, neither the locations of the 3D object points nor the camera poses are necessary for this tracker.
 * @ingroup trackingoffline
 */
class OCEAN_TRACKING_OFFLINE_EXPORT SLAMTracker : public FrameTracker
{
	public:

		/**
		 * Definition of a map mapping frame indices to transformations e.g., camera poses or object transformations.
		 */
		typedef std::map<unsigned int, HomogenousMatrix4> TransformationMap;

	protected:

		/**
		 * This class implements a pair of thresholds.
		 */
		class ReliabilityPair
		{
			public:

				/**
				 * Creates a new pair object.
				 * @param maximalCosine The maximal cosine value for an object point, with range (0, 1)
				 * @param minimalObservationRatio The minimal number of frames in which an object point must be visible, defined as ratio of an external frame range, with range (0, 1]
				 */
				inline ReliabilityPair(const Scalar maximalCosine, const Scalar minimalObservationRatio);

				/**
				 * Returns the maximal cosine value.
				 * @return Maximal cosine value
				 */
				inline Scalar maximalCosine() const;

				/**
				 * Returns the minimal observation ratio.
				 * @return Observation ratio
				 */
				inline Scalar minimalObservationRatio() const;

				/**
				 * Returns the minimal number of observations in relation to a given number of maximal possible observations.
				 * @param overallObservation The maximal possible number of observation
				 * @return max(min(tLowerBoundary, overallObservation), overallObservation * ratio)
				 * @tparam tLowerBoundary Explicit lower boundary
				 */
				template <unsigned int tLowerBoundary>
				inline unsigned int minimalObservations(const unsigned int overallObservation) const;

			protected:

				/// The maximal cosine value.
				const Scalar maximalCosineValue_ = Scalar(0);

				/// The minimal observation ratio.
				const Scalar minimalObservationRatio_ = Scalar(0);
		};

	public:

		/**
		 * Creates a new SLAM Tracker object.
		 */
		SLAMTracker() = default;

		/**
		 * Destructs a SLAM Tracker object.
		 */
		~SLAMTracker() override;

		/**
		 * Starts the offline tracker.
		 * @see OfflineTracker::start().
		 */
		bool start() override;

		/**
		 * Stops the offline tracker.
		 * @see OfflineTracker::stop().
		 */
		bool stop(const unsigned int timeout = 0u) override;

		/**
		 * Returns the tracking quality of this tracker.
		 * @return The tracker's quality, TQ_AUTOMATIC by default
		 */
		virtual TrackingQuality trackingQuality() const;

		/**
		 * Returns the used-defined horizontal field of view of the camera.
		 * @return The horizontal field of view that explicitly has been set, in radian with range (0, PI), -1 if the field of view is not known, -1 by default
		 */
		virtual Scalar cameraFieldOfView() const;

		/**
		 * Returns the camera profile optimization strategy of this tracker.
		 * @return The tracker's optimization strategy for the camera profile, OS_INTRINSIC_PARAMETERS_DISTORTIONS by default
		 */
		virtual PinholeCamera::OptimizationStrategy cameraOptimizationStrategy() const;

		/**
		 * Returns the abstract motion type of this tracker.
		 * @return The tracker's abstract motion type
		 */
		virtual AbstractMotionType abstractMotionType() const;

		/**
		 * Returns the motion speed of this tracker (the speed of the visual information in the frames on which the tracker relies).
		 * @return The tracker's motion speed
		 */
		virtual PointPaths::MotionSpeed motionSpeed() const;

		/**
		 * Returns the specific region of interest which covers image content in the start frame of this tracker.
		 * @return The tracker's region of interest
		 */
		inline const CV::SubRegion& regionOfInterest() const;

		/**
		 * Sets the tracking quality of the tracker.
		 * The quality must be set before the tracker starts.
		 * @param trackingQuality The quality that specifies the performance/accuracy of the tracker.
		 * @return True, if succeeded
		 */
		virtual bool setTrackingQuality(const TrackingQuality trackingQuality);

		/**
		 * Sets the optimization strategy for the camera profile of this tracker.
		 * The strategy must be set before the tracker starts.
		 * @param optimizationStrategy The optimization strategy to set
		 * @return True, if succeeded
		 */
		virtual bool setCameraOptimizationStrategy(const PinholeCamera::OptimizationStrategy optimizationStrategy);

		/**
		 * Explicitly sets the horizontal field of view of the camera.
		 * The field of view must be set before the tracker starts.
		 * @param fovX The horizontal field of view that will be used by the tracker, in radian with range (0, PI), -1 if the field of view is not known
		 * @return True, if succeeded
		 */
		virtual bool setCameraFieldOfView(const Scalar fovX);

		/**
		 * Sets the abstract motion type of this tracker, if known before that tracker starts.
		 * The motion type must be set before the tracker starts.
		 * @param abstractMotionType The abstract motion type to set
		 * @return True, if succeeded
		 */
		virtual bool setAbstractMotionType(const AbstractMotionType abstractMotionType);

		/**
		 * Sets the motion speed of this tracker.
		 * The motion speed must be set before the tracker starts.
		 * @return motionSpeed The motion speed to set
		 * @return True, if succeeded
		 */
		virtual bool setMotionSpeed(const PointPaths::MotionSpeed motionSpeed);

		/**
		 * Sets a specific region of interest which covers image content in the start frame of this tracker.
		 * The roi cannot be set if the tracker is active
		 * @param regionOfInterest The region of interest to set, an invalid roi removes an already existing one
		 * @param soleApplication True, if the tracker uses only the region of interest and not the remaining frame information for tracking
		 * @return True, if succeeded
		 * @see setTrackingFrameRange(), setFrameProviderInterface().
		 */
		bool setRegionOfInterest(const CV::SubRegion& regionOfInterest, const bool soleApplication);

		/**
		 * Extracts the poses from this tracker for a specified frame range not considering any specific region of interest.
		 * Beware: The tracker must have finished before calling this function!<br>
		 * @param lowerFrameIndex The index of the lower frame, with range [0, infinity)
		 * @param upperFrameIndex The index of the upper frame, with range [lowerFrameIndex, infinity)
		 * @param offlinePoses The resulting poses for the specified frame range, not existing poses will be invalid
		 * @param minimalCorrespondences The number of minimal valid 2D/3D point correspondences so that a pose will be provided
		 * @param estimator The robust estimator which is applied for the non-linear pose optimization
		 * @param minimalValidCorrespondenceRatio The ratio of the minimal number of valid correspondences (the valid correspondences will be determined from a RANSAC iteration), with range [0, 1]
		 * @param ransacMaximalSqrError The maximal squared pixel error between image point and projected object points for RANSAC iterations, with range (0, infinity)
		 * @param maximalRobustError The maximal average robust pixel error between image point and projected object points so that a pose counts as valid, with range (0, infinity)
		 * @param finalAverageError Optional resulting average final error for all valid poses, the error depends on the selected robust estimator
		 * @param worker Optional worker object to distribute the computation
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @return True, if all poses have been determined (some poses may be invalid)
		 */
		bool extractPoses(const unsigned int lowerFrameIndex, const unsigned int upperFrameIndex, OfflinePoses& offlinePoses, const unsigned int minimalCorrespondences = 5u, const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const Scalar minimalValidCorrespondenceRatio = Scalar(1), const Scalar ransacMaximalSqrError = Scalar(3.5 * 3.5), const Scalar maximalRobustError = Scalar(3.5 * 3.5), Scalar* finalAverageError = nullptr, Worker* worker = nullptr, bool* abort = nullptr) const;

		/**
		 * Extracts the poses from this tracker for a specific region of interest.
		 * The region of interest must cover a planar area (a plane).<br>
		 * Beware: The tracker must have finished before calling this function!<br>
		 * @param lowerFrameIndex The index of the lower frame, with range [0, infinity)
		 * @param regionOfInterestFrameIndex The index of the frame in which the region of interest is defined, with range [lowerFrameIndex, upperFrameIndex]
		 * @param upperFrameIndex The index of the upper frame, with range [lowerFrameIndex, infinity)
		 * @param regionOfInterest The region of interest for which the resulting poses will be optimized
		 * @param offlinePoses The resulting poses for the specified frame range, not existing poses will be invalid
		 * @param planeTransformation The resulting transformation of the plane representing the geometry for the region of interest, with z-axis defining the normal of the plane
		 * @param minimalCorrespondences The minimal number of valid 2D/3D point correspondences so that a pose will be provided, with range [3, infinity)
		 * @param minimalKeyFrames The minimal number of key frames that will be applied to optimized the object points located in the region of interest, with range [2, infinity)
		 * @param estimator The robust estimator which is applied for the non-linear pose optimization
		 * @param minimalValidCorrespondenceRatio The ratio of the minimal number of valid correspondences (the valid correspondences will be determined from a RANSAC iteration), with range [0, 1]
		 * @param ransacMaximalSqrError The maximal squared pixel error between image point and projected object points for RANSAC iterations, with range (0, infinity)
		 * @param maximalRobustError The maximal average robust pixel error between image point and projected object points so that a pose counts as valid, with range (0, infinity)
		 * @param finalAverageError Optional resulting average final error for all valid poses, the error depends on the selected robust estimator
		 * @param worker Optional worker object to distribute the computation
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @return True, if all poses have been determined (some poses may be invalid)
		 */
		bool extractPoses(const unsigned int lowerFrameIndex, const unsigned int regionOfInterestFrameIndex, const unsigned int upperFrameIndex, const CV::SubRegion& regionOfInterest, OfflinePoses& offlinePoses, HomogenousMatrix4& planeTransformation, const unsigned int minimalCorrespondences = 5u, const unsigned int minimalKeyFrames = 50u, const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const Scalar minimalValidCorrespondenceRatio = Scalar(1), const Scalar ransacMaximalSqrError = Scalar(3.5 * 3.5), const Scalar maximalRobustError = Scalar(3.5 * 3.5), Scalar* finalAverageError = nullptr, Worker* worker = nullptr, bool* abort = nullptr) const;

		/**
		 * Determines the location of some initial 3D object points which are all visible within a defined frame range (range of camera frame without known camera pose values).
		 * The accuracy and reliability of the resulting initial 3D object points can vary due to the given database (which is based on camera motions etc.).<br>
		 * Thus, initial object point locations should never be used directly, instead the locations should be verified or stabilized etc.<br>
		 * This function is based on a database providing image points (projections of the 3D object points) for several camera frames.<br>
		 * The database must provide valid image point positions and the topology between the image points and their corresponding object points and their observation information (visibility of object points in individual camera frames/poses).<br>
		 * This functions tries to add initial information regarding some 3D object point locations and some camera pose values.<br>
		 * The given database will be modified so that the database finally stores the information determined by this function.<br>
		 * The origin of all image points provided by the database must be the upper left frame corner.<br>
		 * This functions has the capability to identify/separate static and dynamic 3D object points and relies only on static object points for initial pose calculations.<br>
		 * @param pinholeCamera The pinhole camera profile providing e.g., the dimension of the camera frame and the projection model
		 * @param database The database providing only the image point positions and the topology information, not the 3D object point locations and not the camera pose values
		 * @param randomGenerator A random generator object
		 * @param lowerFrame The index of the lower frame border of the range of camera frames which will be investigated, with range [0, upperFrame]
		 * @param startFrame Optional index of the start frame from which the object point determination starts, with range [lowerFrame, upperFrame], otherwise nullptr
		 * @param upperFrame The index of the upper frame border of the range of camera frames which will be investigated, with range [lowerFrame, infinity)
		 * @param regionOfInterest Optional region of interest defining a specific area in the start frame so that the object points lying in the region are handled with higher priority, an invalid region to avoid any special region of interest handling, a valid region needs a valid start frame
		 * @param soleRegionOfInterest True, if the tracking must rely on the region of interest only and if no remaining information must be used
		 * @param finalLowerValidPoseRange Optional resulting index of the lower frame within the resulting valid range of camera poses, with range [lowerFrame, upperFrame]
		 * @param finalUpperValidPoseRange Optional resulting index of the upper frame within the resulting valid range of camera poses, with range [lowerValidPose, upperFrame]
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @param progress Optional resulting progress with range [0, 1]
		 * @return True, if at least one initial 3D object point and one camera pose could be determined
		 * @see extendInitialObjectPoints().
		 */
		static bool determineInitialObjectPoints(const PinholeCamera& pinholeCamera, Database& database, RandomGenerator& randomGenerator, const unsigned int lowerFrame, const unsigned int* startFrame, const unsigned int upperFrame, const CV::SubRegion& regionOfInterest, const bool soleRegionOfInterest, unsigned int* finalLowerValidPoseRange = nullptr, unsigned int* finalUpperValidPoseRange = nullptr, bool* abort = nullptr, Scalar* progress = nullptr);

		/**
		 * Determines the locations of further (stable and reliable) 3D object points which are visible within a defined frame range and for which the 3D location is not known yet.
		 * Thus, this function extends a set of already known 3D object point locations with locations of further 3D object points which are predicted to be very reliable.<br>
		 * The number of new object point locations can vary but in general will be not very large as in this step the concentration lies on quality rather than quantity.<br>
		 * The given database must provide locations of some already known 3D object points and further the camera pose values for some camera frames (within the defined frame range) in which the 3D object points are visible.<br>
		 * The database will be modified as the database will receive the locations of new 3D object points and camera pose values.<br>
		 * If this functions succeeds the database will hold reliable and stable 3D object point locations and a set of valid camera pose values.<br>
		 * Thus, the database will provide reliable information within the defined frame range.
		 * @param pinholeCamera The pinhole camera profile providing e.g., the dimension of the camera frame and the projection model
		 * @param database The database providing the image point positions, the topology information and some 3D object point locations and camera pose values
		 * @param lowerFrame The index of the lower frame border of the range of camera frames which will be investigated, with range [0, upperFrame]
		 * @param upperFrame The index of the upper frame border of the range of camera frames which will be investigated, with range [lowerFrame, infinity)
		 * @param correspondenceThreshold The threshold of point correspondences which must be visible in each camera frame so that a valid pose will be determined
		 * @param finalLowerValidPoseRange Optional resulting index of the lower frame within the resulting valid range of camera poses, with range [lowerFrame, upperFrame]
		 * @param finalUpperValidPoseRange Optional resulting index of the upper frame within the resulting valid range of camera poses, with range [lowerValidPose, upperFrame]
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @return True, if at least one 3D object point location and one camera pose are finally in the database
		 * @see extendStableObjectPoints(), determineInitialObjectPoints().
		 */
		static bool extendInitialObjectPoints(const PinholeCamera& pinholeCamera, Database& database, const unsigned int lowerFrame, const unsigned int upperFrame, const Solver3::RelativeThreshold& correspondenceThreshold = Solver3::RelativeThreshold(10u, Scalar(0.5), 25u), unsigned int* finalLowerValidPoseRange = nullptr, unsigned int* finalUpperValidPoseRange = nullptr, bool* abort = nullptr);

		/**
		 * Optimizes the camera profile for a database with stable initial object points.
		 * Beware: Do not optimize the camera profile 'too early', the database must hold enough object point locations and camera poses so that the true camera profile can be determined.
		 * @param pinholeCamera The pinhole camera profile to be optimized
		 * @param database The database providing locations of initial but stable object points (with corresponding valid camera poses)
		 * @param lowerFrame The index of the lower frame border of the range of camera frames which will be investigated, with range [0, upperFrame]
		 * @param upperFrame The index of the upper frame border of the range of camera frames which will be investigated, with range [lowerFrame, infinity)
		 * @param findInitialFieldOfView True, to apply a determination of the initial field of view of the camera (should be done if the camera's field of view is not known)
		 * @param optimizationStrategy The optimization strategy for the camera parameters which will be applied, OS_INVALID to avoid any optimization of the camera parameters
		 * @param maximalKeyFrames The maximal number of key frames that are used to optimize the profile of the camera, the higher the number the better the final accuracy but the longer the optimization will take, with range [1, upperFrame - lowerFrame + 1u]
		 * @param optimizedCamera The optimized camera profile
		 * @param optimizedDatabase The database with new object points and camera poses matching to the new camera profile
		 * @param cameraMotion Optional resulting motion of the camera that could be identified during the optimization of the camera profile, CM_UNKNOWN if the motion could not be determined
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @param finalMeanSqrError Optional resulting final mean squared projection error for the optimized camera profile
		 * @return True, if the camera profile could be optimized or changed due to the specified properties; False, if neither the camera profile nor the database has been modified so that the caller should use the initial camera profile and database for further calculations
		 */
		static bool optimizeCamera(const PinholeCamera& pinholeCamera, const Database& database, const unsigned int lowerFrame, const unsigned int upperFrame, const bool findInitialFieldOfView, const PinholeCamera::OptimizationStrategy optimizationStrategy, const unsigned int maximalKeyFrames, PinholeCamera& optimizedCamera, Database& optimizedDatabase, Solver3::CameraMotion* cameraMotion = nullptr, bool* abort = nullptr, Scalar* finalMeanSqrError = nullptr);

		/**
		 * This function extends a database providing stable/accurate and reliable locations of 3D object points visible within the defined frame range by additional reliable 3D object points.
		 * The resulting database will provide as much stable 3D object point location as necessary (if possible) to guarantee stable poses.<br>
		 * However, the database may hold several additional robust object points which have not been investigated/determined yet.
		 * @param pinholeCamera The pinhole camera profile providing e.g., the dimension of the camera frame and the projection model
		 * @param database The database providing the image point positions, the topology information and some stable/reliable 3D object point locations and camera pose values
		 * @param randomGenerator Random generator object
		 * @param lowerFrame The index of the lower frame border of the range of camera frames which will be investigated, with range [0, upperFrame]
		 * @param upperFrame The index of the upper frame border of the range of camera frames which will be investigated, with range [lowerFrame, infinity)
		 * @param cameraMotion The camera motion (if known) for which stable object points will be extended/added, CM_UNKNOWN if the motion is not known
		 * @param correspondenceThreshold The threshold of point correspondences which must be visible in each camera frame so that a valid pose will be determined
		 * @param finalLowerValidPoseRange Optional resulting index of the lower frame within the resulting valid range of camera poses, with range [lowerFrame, upperFrame]
		 * @param finalUpperValidPoseRange Optional resulting index of the upper frame within the resulting valid range of camera poses, with range [lowerValidPose, upperFrame]
		 * @param finalCameraMotion Optional resulting motion of the camera
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @param progress Optional resulting progress with range [0, 1]
		 * @return True, if at least one 3D object point location and one camera pose are finally in the database
		 * @see extendStableObjectPoints(), determineInitialObjectPoints().
		 */
		static bool extendStableObjectPoints(const PinholeCamera& pinholeCamera, Database& database, RandomGenerator& randomGenerator, const unsigned int lowerFrame, const unsigned int upperFrame, const Solver3::CameraMotion cameraMotion = Solver3::CM_UNKNOWN, const Solver3::RelativeThreshold& correspondenceThreshold = Solver3::RelativeThreshold(10u, Scalar(0.5), 25u), unsigned int* finalLowerValidPoseRange = nullptr, unsigned int* finalUpperValidPoseRange = nullptr, Solver3::CameraMotion* finalCameraMotion = nullptr, bool* abort = nullptr, Scalar* progress = nullptr);

		/**
		 * Removes all inaccurate locations of 3D object points from a given database.
		 * The topology will be left unchanged but the location is invalidated.<br>
		 * A location of a 3D object point is inaccurate if the projection error extends a specified threshold in at least one camera frame.<br>
		 * Whenever locations of 3D object points are removed from the database all camera poses will be updated as the pose value can slightly change due to the missing object points.<br>
		 * Therefore, several iterations of object-remove and pose-update steps can be applied so that the database stored only reliable and accurate object points and camera poses
		 * @param pinholeCamera The pinhole camera profile providing e.g., the dimension of the camera frame and the projection model
		 * @param cameraMotion The motion of the camera, use CM_UNKNOWN if the motion is not known and may contain translation
		 * @param database The database providing the image point positions, the topology information and the locations of the 3D object points and valid camera pose values within the specified frame range
		 * @param randomGenerator Random generator object
		 * @param lowerFrame The index of the lower frame border of the range of camera frames which will be investigated, with range [0, upperFrame]
		 * @param upperFrame The index of the upper frame border of the range of camera frames which will be investigated, with range [lowerFrame, infinity)
		 * @param minimalCorrespondences The minimal number of point correspondences which must be visible in each frame so that a valid pose will be determined
		 * @param minimalValidCorrespondenceRatio The ratio of the minimal number of valid correspondences (the valid correspondences will be determined from a RANSAC iteration), with range [0, 1]
		 * @param maximalAverageSqrError The maximal averaged squared error between the projected 3D object points and their corresponding image point positions to count as valid
		 * @param maximalWorstSqrError The maximal worst squared error between the projected 3D object points and their corresponding image point positions to count as valid
		 * @param iterations The maximal number of object-remove and pose-update iterations; however, the function stop in anyway if no object point has been invalidated
		 * @param finalLowerValidPoseRange Optional resulting index of the lower frame within the resulting valid range of camera poses, with range [lowerFrame, upperFrame]
		 * @param finalUpperValidPoseRange Optional resulting index of the upper frame within the resulting valid range of camera poses, with range [lowerValidPose, upperFrame]
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @return True, if at least one 3D object point location and one camera pose are finally in the database
		 */
		static bool removeInaccurateObjectPoints(const PinholeCamera& pinholeCamera, const Solver3::CameraMotion cameraMotion, Database& database, RandomGenerator& randomGenerator, const unsigned int lowerFrame, const unsigned int upperFrame, const unsigned int minimalCorrespondences, const Scalar minimalValidCorrespondenceRatio = Scalar(1), const Scalar maximalAverageSqrError = Scalar(3.5 * 3.5), const Scalar maximalWorstSqrError = Scalar(5.5 * 5.5), const unsigned int iterations = (unsigned int)(-1), unsigned int* finalLowerValidPoseRange = nullptr, unsigned int* finalUpperValidPoseRange = nullptr, bool* abort = nullptr);

		/**
		 * Optimizes a set of given 3D object point locations and all camera poses within a specified frame range iteratively.
		 * The optimization is not done by a bundle adjustment but by a separated optimization (first the optimization of object points with static camera poses, followed by an optimization of the camera poses with static object points).
		 * @param pinholeCamera The pinhole camera profile providing e.g., the dimension of the camera frame and the projection model
		 * @param cameraMotion The motion of the camera, use CM_UNKNOWN if the motion is not known and may contain translation
		 * @param database The database providing the image point positions, the topology information and some stable/reliable 3D object point locations and camera pose values, the database will received the optimized camera poses and object point locations
		 * @param randomGenerator Random generator object
		 * @param lowerFrame The index of the lower frame border of the range of camera frames which will be investigated, with range [0, upperFrame]
		 * @param upperFrame The index of the upper frame border of the range of camera frames which will be investigated, with range [lowerFrame, infinity)
		 * @param objectPointIds The ids of all object point having already a valid 3D location which will be optimized, object points which are not accurate enough will be removed from this set
		 * @param minimalCorrespondences The minimal number of point correspondences in a frame so that a valid pose will be determined
		 * @param beginWithPoseUpdate True, to start with one initial optimization of all camera poses before the object points will be optimized
		 * @param iterations The number of successive optimization iteration steps
		 * @param estimator The robust estimator which will be applied for pose determination
		 * @param minimalValidCorrespondenceRatio The ratio of the minimal number of valid correspondences (the valid correspondences will be determined from a RANSAC iteration), with range [0, 1]
		 * @param maximalRobustError The maximal robust error so that a determined pose counts as valid
		 * @param initialAverageError Optional resulting averaged initial error before the optimization has been invoked
		 * @param finalAverageError Optional resulting averaged final error after the optimization has finished
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @return True, if succeeded
		 */
		static bool optimizeObjectPointsAndPosesIndividuallyIteratively(const PinholeCamera& pinholeCamera, const Solver3::CameraMotion cameraMotion, Database& database, RandomGenerator& randomGenerator, const unsigned int lowerFrame, const unsigned int upperFrame, Indices32& objectPointIds, const unsigned int minimalCorrespondences, const bool beginWithPoseUpdate, const unsigned int iterations, const Geometry::Estimator::EstimatorType estimator, const Scalar minimalValidCorrespondenceRatio, const Scalar maximalRobustError, Scalar* initialAverageError, Scalar* finalAverageError, bool* abort);

		/**
		 * This functions extends an already reliable database with precise 3D object point locations and camera poses with new 3D object point locations visible in a specified region of interest in a specified camera frame.
		 * As the region of interest may cover any geometry structure this function tries to determine the locations of object points only that are visible in the specified frame for which the region of interest is defined.<br>
		 * @param pinholeCamera The pinhole camera profile providing e.g., the dimension of the camera frame and the projection model
		 * @param database The database providing the image point positions, the topology information and some stable/reliable 3D object point locations and camera pose values
		 * @param randomGenerator Random generator object
		 * @param lowerFrame The index of the lower frame border of the range of camera frames which will be investigated, with range [0, upperFrame]
		 * @param regionOfInterestFrame The index of the frame in which the region of interest is defined, with range [lowerFrame, upperFrame]
		 * @param upperFrame The index of the upper frame border of the range of camera frames which will be investigated, with range [lowerFrame, infinity)
		 * @param regionOfInterest The region of interest defining a sub-region in a specific frame in which new object points will be investigated
		 * @param cameraMotion The camera motion (if known) for the given database
		 * @param minimalObservations The minimal number of observations a new object point must have (in arbitrary sibling camera pose)
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @return True, if at least one 3D object point location and one camera pose are finally in the database
		 * @see addObjectPointsInPlanarRegionOfInterest().
		 */
		static bool addUnknownObjectPointsInRegionOfInterest(const PinholeCamera& pinholeCamera, Database& database, RandomGenerator& randomGenerator, const unsigned int lowerFrame, const unsigned int regionOfInterestFrame, const unsigned int upperFrame, const CV::SubRegion& regionOfInterest, const Solver3::CameraMotion cameraMotion = Solver3::CM_UNKNOWN, const unsigned int minimalObservations = 10u, bool* abort = nullptr);

		/**
		 * This functions extends an already reliable database with precise 3D object point locations and camera poses with new 3D object point locations visible in a specified region of interest in a specified camera frame which covers a planar area (a plane).
		 * This function determines the plane which is covered by the region of interest and uses this plane to determine the locations of 3D object points which are not visible in the frame for which the region of interest is defined.<br>
		 * @param pinholeCamera The pinhole camera profile providing e.g., the dimension of the camera frame and the projection model
		 * @param database The database providing the image point positions, the topology information and some stable/reliable 3D object point locations and camera pose values
		 * @param randomGenerator Random generator object
		 * @param lowerFrame The index of the lower frame border of the range of camera frames which will be investigated, with range [0, upperFrame]
		 * @param regionOfInterestFrame The index of the frame in which the region of interest is defined, with range [lowerFrame, upperFrame]
		 * @param upperFrame The index of the upper frame border of the range of camera frames which will be investigated, with range [lowerFrame, infinity)
		 * @param regionOfInterest The region of interest defining a sub-region in a specific frame in which new object points will be investigated
		 * @param cameraMotion The camera motion (if known) for the given database
		 * @param minimalObservations The minimal number of observations a new object point must have (in arbitrary sibling camera pose)
		 * @param plane Optional resulting plane that has been determined during the process
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @return True, if at least one 3D object point location and one camera pose are finally in the database
		 * @see addObjectPointsInRegionOfInterest().
		 */
		static bool addUnknownObjectPointsInPlanarRegionOfInterest(const PinholeCamera& pinholeCamera, Database& database, RandomGenerator& randomGenerator, const unsigned int lowerFrame, const unsigned int regionOfInterestFrame, const unsigned int upperFrame, const CV::SubRegion& regionOfInterest, const Solver3::CameraMotion cameraMotion = Solver3::CM_UNKNOWN, const unsigned int minimalObservations = 10u, Plane3* plane = nullptr, bool* abort = nullptr);

		/**
		 * Determines the number of valid correspondences (2D/3D correspondences which are used for pose determination) for image points which are visible in a planar region of interest for a defined range of frames.
		 * @param pinholeCamera The pinhole camera profile defining the projection, must be valid
		 * @param database The database providing the image point positions, the valid 3D object point locations, the valid camera poses and the topology information
		 * @param lowerFrame The index of the lower frame border of the range of camera frames which will be investigated, with range [0, upperFrame]
		 * @param regionOfInterestFrame The index of the frame in which the region of interest is defined, with range [lowerFrame, upperFrame]
		 * @param upperFrame The index of the upper frame border of the range of camera frames which will be investigated, with range [lowerFrame, infinity)
		 * @param regionOfInterest The region of interest defining a sub-region in a specific frame in which new object points will be investigated
		 * @param plane The plane of the region of interest, must be valid
		 * @param validCorrespondences Optional resulting number of valid correspondences for each frame in the defined frame range, the first number represents the lower frame
		 * @param meanCorrespondences Optional resulting mean number of valid correspondences
		 * @param medianCorrespondences Optional resulting median number of valid correspondences
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @return True, if succeeded
		 */
		static bool determineValidCorrespondencesInPlanarRegionOfInterest(const PinholeCamera& pinholeCamera, const Database& database, const unsigned int lowerFrame, const unsigned int regionOfInterestFrame, const unsigned int upperFrame, const CV::SubRegion& regionOfInterest, const Plane3& plane, Indices32* validCorrespondences = nullptr, Scalar* meanCorrespondences = nullptr, unsigned int* medianCorrespondences = nullptr, bool* abort = nullptr);

		/**
		 * Determines the number of visible image points in a planar region of interest for a defined range of frames.
		 * @param pinholeCamera The pinhole camera profile defining the projection, must be valid
		 * @param database The database providing the image point positions, the valid 3D object point locations, the valid camera poses and the topology information
		 * @param lowerFrame The index of the lower frame border of the range of camera frames which will be investigated, with range [0, upperFrame]
		 * @param regionOfInterestFrame The index of the frame in which the region of interest is defined, with range [lowerFrame, upperFrame]
		 * @param upperFrame The index of the upper frame border of the range of camera frames which will be investigated, with range [lowerFrame, infinity)
		 * @param regionOfInterest The region of interest defining a sub-region in a specific frame in which new object points will be investigated
		 * @param plane The plane of the region of interest, must be valid
		 * @param numberImagePoints Optional resulting number of visible image points for each frame in the defined frame range, the first number represents the lower frame
		 * @param meanNumberImagePoints Optional resulting mean number of visible image points
		 * @param medianNumberImagePoints Optional resulting median number of visible image points
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @return True, if succeeded
		 */
		static bool determineNumberImagePointsInPlanarRegionOfInterest(const PinholeCamera& pinholeCamera, const Database& database, const unsigned int lowerFrame, const unsigned int regionOfInterestFrame, const unsigned int upperFrame, const CV::SubRegion& regionOfInterest, const Plane3& plane, Indices32* numberImagePoints = nullptr, Scalar* meanNumberImagePoints = nullptr, unsigned int* medianNumberImagePoints = nullptr, bool* abort = nullptr);

		/**
		 * Adjusts camera poses specified for a specific camera profile without distortion parameters to a new camera profile without distortion parameters.
		 * @param oldCamera The old camera profile matching to the old camera poses, without distortion parameters
		 * @param oldPoses The old camera poses matching to the old camera profile
		 * @param newCamera The new camera profile for which the new camera poses will be determined, without distortion parameters
		 * @param newPoses The new camera poses matching to the provided new camera profile
		 * @param newObjectTransformations The new object transformations matching to the new camera poses, may contain scale and shear factors
		 * @return True, if succeeded
		 * @see adjustPosesAndPlaneToCamera().
		 */
		static bool adjustPosesToCameraWithoutDistortion(const PinholeCamera& oldCamera, const TransformationMap& oldPoses, const PinholeCamera& newCamera, TransformationMap& newPoses, TransformationMap& newObjectTransformations);

		/**
		 * Adjusts camera poses specified for a specific camera profile (which may include distortion) to a new camera profile (which may include distortion).
		 * @param oldCamera The old camera profile matching to the old camera poses
		 * @param oldPoses The old camera poses matching to the old camera profile
		 * @param oldPlaneTransformation The transformation of the plane that has been determined together with the old camera poses
		 * @param selectionFrameIndex The index of the selection frame in which a region of interest has been defined to determine the track (the camera profile and camera poses)
		 * @param newCamera The new camera profile for which the new camera poses and plane transformation will be determined
		 * @param newPoses The new camera poses matching to the provided new camera profile
		 * @param newPlaneTransformation The transformation of the plane that matches to the new poses
		 * @param iterations The number of optimization iterations, with range [1, infinity), use 1 iteration for rotational camera motion to avoid skewed optimization results (as the scene is not based on stereo information)
		 * @param finalError Optional resulting final pixel error between projected 3D object points and their corresponding image points
		 * @return True, if succeeded
		 * @see adjustPosesToCameraWithoutDistortion().
		 */
		static bool adjustPosesAndPlaneToCamera(const PinholeCamera& oldCamera, const OfflinePoses& oldPoses, const HomogenousMatrix4& oldPlaneTransformation, const unsigned int selectionFrameIndex, const PinholeCamera& newCamera, OfflinePoses& newPoses, HomogenousMatrix4& newPlaneTransformation, const unsigned int iterations = 30u, Scalar* finalError = nullptr);

	protected:

		/**
		 * Frame tracker run function.
		 * @param frameType Frame type of the tracking frames
		 * @return True, if the tracking succeeded
		 */
		bool applyFrameTracking(const FrameType& frameType) override;

		/**
		 * The event function for the scheduler.
		 */
		void onScheduler();

		/**
		 * This function extends a database providing stable/accurate and reliable locations of 3D object points visible within the defined frame range by additional reliable 3D object points.
		 * The given database already provides valid camera poses for all frames within the defined frame range.
		 * @param pinholeCamera The pinhole camera profile providing e.g., the dimension of the camera frame and the projection model
		 * @param database The database providing the image point positions, the topology information and some stable/reliable 3D object point locations and camera pose values
		 * @param cameraMotion The camera motion within the defined frame range
		 * @param lowerFrame The index of the lower frame border of the range of camera frames which will be investigated, with range [0, upperFrame]
		 * @param upperFrame The index of the upper frame border of the range of camera frames which will be investigated, with range [lowerFrame, infinity)
		 * @param correspondenceThreshold The threshold of point correspondences which must be visible in each camera frame so that a valid pose will be determined
		 * @param finalLowerValidPoseRange Optional resulting index of the lower frame within the resulting valid range of camera poses, with range [lowerFrame, upperFrame]
		 * @param finalUpperValidPoseRange Optional resulting index of the upper frame within the resulting valid range of camera poses, with range [lowerValidPose, upperFrame]
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @param progress Optional resulting progress with range [0, 1]
		 * @return True, if at least one 3D object point location and one camera pose are finally in the database
		 * @see extendStableObjectPoints().
		 */
		static bool extendStableObjectPointsFull(const PinholeCamera& pinholeCamera, Database& database, const Solver3::CameraMotion cameraMotion, const unsigned int lowerFrame, const unsigned int upperFrame, const Solver3::RelativeThreshold& correspondenceThreshold = Solver3::RelativeThreshold(10u, Scalar(0.5), 25u), unsigned int* finalLowerValidPoseRange = nullptr, unsigned int* finalUpperValidPoseRange = nullptr, bool* abort = nullptr, Scalar* progress = nullptr);

		/**
		 * This function extends a database providing stable/accurate and reliable locations of 3D object points visible within the defined frame range by additional reliable 3D object points.
		 * The given database provides valid camera poses only for a subset of the frames within the defined frame range.
		 * @param pinholeCamera The pinhole camera profile providing e.g., the dimension of the camera frame and the projection model
		 * @param database The database providing the image point positions, the topology information and some stable/reliable 3D object point locations and camera pose values
		 * @param cameraMotion The camera motion within the defined frame range
		 * @param lowerFrame The index of the lower frame border of the range of camera frames which will be investigated, with range [0, upperFrame]
		 * @param upperFrame The index of the upper frame border of the range of camera frames which will be investigated, with range [lowerFrame, infinity)
		 * @param correspondenceThreshold The threshold of point correspondences which must be visible in each camera frame so that a valid pose will be determined
		 * @param finalLowerValidPoseRange Optional resulting index of the lower frame within the resulting valid range of camera poses, with range [lowerFrame, upperFrame]
		 * @param finalUpperValidPoseRange Optional resulting index of the upper frame within the resulting valid range of camera poses, with range [lowerValidPose, upperFrame]
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @param progress Optional resulting progress with range [0, 1]
		 * @return True, if at least one 3D object point location and one camera pose are finally in the database
		 * @see extendStableObjectPoints().
		 */
		static bool extendStableObjectPointsPartially(const PinholeCamera& pinholeCamera, Database& database, const Solver3::CameraMotion cameraMotion, const unsigned int lowerFrame, const unsigned int upperFrame, const Solver3::RelativeThreshold& correspondenceThreshold = Solver3::RelativeThreshold(10u, Scalar(0.5), 25u), unsigned int* finalLowerValidPoseRange = nullptr, unsigned int* finalUpperValidPoseRange = nullptr, bool* abort = nullptr, Scalar* progress = nullptr);

		/**
		 * This function extends a database providing stable/accurate and reliable locations of 3D object points visible within the defined frame range by additional reliable 3D object points.
		 * The camera has a rotational motion or is static within the defined frame range.<br>
		 * The given database already provides valid camera poses for all frames within the defined frame range.
		 * @param pinholeCamera The pinhole camera profile providing e.g., the dimension of the camera frame and the projection model
		 * @param database The database providing the image point positions, the topology information and some stable/reliable 3D object point locations and camera pose values
		 * @param lowerFrame The index of the lower frame border of the range of camera frames which will be investigated, with range [0, upperFrame]
		 * @param upperFrame The index of the upper frame border of the range of camera frames which will be investigated, with range [lowerFrame, infinity)
		 * @param correspondenceThreshold The threshold of point correspondences which must be visible in each camera frame so that a valid pose will be determined
		 * @param finalLowerValidPoseRange Optional resulting index of the lower frame within the resulting valid range of camera poses, with range [lowerFrame, upperFrame]
		 * @param finalUpperValidPoseRange Optional resulting index of the upper frame within the resulting valid range of camera poses, with range [lowerValidPose, upperFrame]
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @param progress Optional resulting progress with range [0, 1]
		 * @return True, if at least one 3D object point location and one camera pose are finally in the database
		 * @see extendStableObjectPointsFull().
		 */
		static bool extendStableObjectPointsFullRotational(const PinholeCamera& pinholeCamera, Database& database, const unsigned int lowerFrame, const unsigned int upperFrame, const Solver3::RelativeThreshold& correspondenceThreshold = Solver3::RelativeThreshold(10u, Scalar(0.5), 25u), unsigned int* finalLowerValidPoseRange = nullptr, unsigned int* finalUpperValidPoseRange = nullptr, bool* abort = nullptr, Scalar* progress = nullptr);

		/**
		 * This function extends a database providing stable/accurate and reliable locations of 3D object points visible within the defined frame range by additional reliable 3D object points.
		 * The camera has a translational motion within the defined frame range.<br>
		 * The given database already provides valid camera poses for all frames within the defined frame range.
		 * @param pinholeCamera The pinhole camera profile providing e.g., the dimension of the camera frame and the projection model
		 * @param database The database providing the image point positions, the topology information and some stable/reliable 3D object point locations and camera pose values
		 * @param lowerFrame The index of the lower frame border of the range of camera frames which will be investigated, with range [0, upperFrame]
		 * @param upperFrame The index of the upper frame border of the range of camera frames which will be investigated, with range [lowerFrame, infinity)
		 * @param correspondenceThreshold The threshold of point correspondences which must be visible in each camera frame so that a valid pose will be determined
		 * @param finalLowerValidPoseRange Optional resulting index of the lower frame within the resulting valid range of camera poses, with range [lowerFrame, upperFrame]
		 * @param finalUpperValidPoseRange Optional resulting index of the upper frame within the resulting valid range of camera poses, with range [lowerValidPose, upperFrame]
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @param progress Optional resulting progress with range [0, 1]
		 * @return True, if at least one 3D object point location and one camera pose are finally in the database
		 * @see extendStableObjectPointsFull(), stabilizeStableObjectPointsPartiallyTranslational(), extendStableObjectPointsPartiallyTranslational().
		 */
		static bool extendStableObjectPointsFullTranslational(const PinholeCamera& pinholeCamera, Database& database, const unsigned int lowerFrame, const unsigned int upperFrame, const Solver3::RelativeThreshold& correspondenceThreshold = Solver3::RelativeThreshold(10u, Scalar(0.5), 25u), unsigned int* finalLowerValidPoseRange = nullptr, unsigned int* finalUpperValidPoseRange = nullptr, bool* abort = nullptr, Scalar* progress = nullptr);

		/**
		 * This function extends a database providing stable/accurate and reliable locations of 3D object points visible within the defined frame range by additional reliable 3D object points.
		 * In detail, this function adds new object points mainly visible within the frame range with valid poses.<br>
		 * The camera has a rotational motion or is static within the defined frame range.<br>
		 * The given database provides valid camera poses only for a subset of the frames within the defined frame range.
		 * @param pinholeCamera The pinhole camera profile providing e.g., the dimension of the camera frame and the projection model
		 * @param database The database providing the image point positions, the topology information and some stable/reliable 3D object point locations and camera pose values
		 * @param lowerFrame The index of the lower frame border of the range of camera frames which will be investigated, with range [0, upperFrame]
		 * @param upperFrame The index of the upper frame border of the range of camera frames which will be investigated, with range [lowerFrame, infinity)
		 * @param correspondenceThreshold The threshold of point correspondences which must be visible in each camera frame so that a valid pose will be determined
		 * @param finalLowerValidPoseRange Optional resulting index of the lower frame within the resulting valid range of camera poses, with range [lowerFrame, upperFrame]
		 * @param finalUpperValidPoseRange Optional resulting index of the upper frame within the resulting valid range of camera poses, with range [lowerValidPose, upperFrame]
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @param progress Optional resulting progress with range [0, 1]
		 * @return True, if at least one 3D object point location and one camera pose are finally in the database
		 * @see extendStableObjectPointsFull().
		 */
		static bool extendStableObjectPointsPartiallyRotational(const PinholeCamera& pinholeCamera, Database& database, const unsigned int lowerFrame, const unsigned int upperFrame, const Solver3::RelativeThreshold& correspondenceThreshold = Solver3::RelativeThreshold(10u, Scalar(0.5), 25u), unsigned int* finalLowerValidPoseRange = nullptr, unsigned int* finalUpperValidPoseRange = nullptr, bool* abort = nullptr, Scalar* progress = nullptr);

		/**
		 * This function stabilizes a database already providing stable/accurate and reliable locations of 3D object points visible within the defined frame range by additional reliable 3D object points.
		 * In detail, this function adds new object points mainly visible within the frame range with valid poses.<br>
		 * The camera has a translational motion within the defined frame range.<br>
		 * The given database provides valid camera poses only for a subset of the frames within the defined frame range.
		 * @param pinholeCamera The pinhole camera profile providing e.g., the dimension of the camera frame and the projection model
		 * @param database The database providing the image point positions, the topology information and some stable/reliable 3D object point locations and camera pose values
		 * @param lowerFrame The index of the lower frame border of the range of camera frames which will be investigated, with range [0, upperFrame]
		 * @param upperFrame The index of the upper frame border of the range of camera frames which will be investigated, with range [lowerFrame, infinity)
		 * @param correspondenceThreshold The threshold of point correspondences which must be visible in each camera frame so that a valid pose will be determined
		 * @param finalLowerValidPoseRange Optional resulting index of the lower frame within the resulting valid range of camera poses, with range [lowerFrame, upperFrame]
		 * @param finalUpperValidPoseRange Optional resulting index of the upper frame within the resulting valid range of camera poses, with range [lowerValidPose, upperFrame]
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @param progress Optional resulting progress with range [0, 1]
		 * @return True, if at least one 3D object point location and one camera pose are finally in the database
		 * @see extendStableObjectPointsPartiallyTranslational().
		 */
		static bool stabilizeStableObjectPointsPartiallyTranslational(const PinholeCamera& pinholeCamera, Database& database, const unsigned int lowerFrame, const unsigned int upperFrame, const Solver3::RelativeThreshold& correspondenceThreshold = Solver3::RelativeThreshold(10u, Scalar(0.5), 25u), unsigned int* finalLowerValidPoseRange = nullptr, unsigned int* finalUpperValidPoseRange = nullptr, bool* abort = nullptr, Scalar* progress = nullptr);

		/**
		 * This function extends a database providing stable/accurate and reliable locations of 3D object points visible within the defined frame range by additional reliable 3D object points.
		 * In detail, this function adds new object points to a database mainly visible at the 'borders' of the frame range with valid poses.<br>
		 * This function should be applied if a database holds valid poses for some (connected) frames within the entire range of frames.<br>
		 * The algorithm explicitly tries to extend the range of valid frame poses by adding object points visible at these border iteratively.<br>
		 * The camera has a translational motion within the defined frame range.<br>
		 * The given database provides valid camera poses only for a subset of the frames within the defined frame range.
		 * @param pinholeCamera The pinhole camera profile providing e.g., the dimension of the camera frame and the projection model
		 * @param database The database providing the image point positions, the topology information and some stable/reliable 3D object point locations and camera pose values
		 * @param lowerFrame The index of the lower frame border of the range of camera frames which will be investigated, with range [0, upperFrame]
		 * @param upperFrame The index of the upper frame border of the range of camera frames which will be investigated, with range [lowerFrame, infinity)
		 * @param correspondenceThreshold The threshold of point correspondences which must be visible in each camera frame so that a valid pose will be determined
		 * @param finalLowerValidPoseRange Optional resulting index of the lower frame within the resulting valid range of camera poses, with range [lowerFrame, upperFrame]
		 * @param finalUpperValidPoseRange Optional resulting index of the upper frame within the resulting valid range of camera poses, with range [lowerValidPose, upperFrame]
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @param progress Optional resulting progress with range [0, 1]
		 * @return True, if at least one 3D object point location and one camera pose are finally in the database
		 * @see stabilizeStableObjectPointsPartiallyTranslational().
		 */
		static bool extendStableObjectPointsPartiallyTranslational(const PinholeCamera& pinholeCamera, Database& database, const unsigned int lowerFrame, const unsigned int upperFrame, const Solver3::RelativeThreshold& correspondenceThreshold = Solver3::RelativeThreshold(10u, Scalar(0.5), 25u), unsigned int* finalLowerValidPoseRange = nullptr, unsigned int* finalUpperValidPoseRange = nullptr, bool* abort = nullptr, Scalar* progress = nullptr);

		/**
		 * Extracts a subset of object point ids and object points from a large set of object point ids, object points and their corresponding number of observations so that the subset contains object points with most observations.
		 * This functions sorts the number of observations so that the object points with most observations (hopefully the most stable object points) can be returned.
		 * @param objectPointIds The ids of all given object points
		 * @param objectPoints The locations of the object points, one location for each given id
		 * @param objectPointObservations The number of observations of all object points, one observation number for each given id
		 * @param subsetSize The number of resulting extracted object points, the actual resulting number of object points can be larger as several object points can have the same number of observations
		 * @param bestObjectPointIds The resulting ids of the subset of extracted object points
		 * @param bestObjectPoints The resulting locations of the subset of extracted object points
		 */
		static void extractObjectPointsWithMostObservations(const Indices32& objectPointIds, const Vectors3& objectPoints, const Indices32& objectPointObservations, const size_t subsetSize, Indices32& bestObjectPointIds, Vectors3& bestObjectPoints);

		/**
		 * Sets an optional progress value to a fixed value.
		 * @param progress The optional progress value, nullptr if no progress value is provided
		 * @param value The fixed progress value to set, with range [0, 1]
		 */
		static inline void setProgress(Scalar* progress, const Scalar value);

		/**
		 * Adjusts a transformation of a plane representing the geometry for a region of interest, with z-axis defining the normal of the plane.
		 * If the region is composed of two triangles defining a rectangle the x-axis of the transformation will be adjusted so that the projected axis fits to the most horizontal edge of the rectangle.<br>
		 * Otherwise, the x-axis of the transformation is adjusted so that the projected axis fits to the horizontal edge of the bounding box of the region of interest.<br>
		 * Further, the origin of the plane will be located at the center of the region of interest.
		 * @param pinholeCamera The pinhole camera profile defining e.g., the dimension of the camera frame and the projection model
		 * @param pose The camera pose of the frame in which the region of interest is defined
		 * @param regionOfInterest The region of interest to which the transformation will be adjusted to
		 * @param planeTransformation The transformation of the plane representing the geometry of the region of interest, with z-axis defining the normal of the plane
		 */
		static bool adjustPlaneTransformationToRegionOfInterest(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose, const CV::SubRegion& regionOfInterest, HomogenousMatrix4& planeTransformation);

		/**
		 * Sends environment information to the maintenance manager.
		 */
		void maintenanceSendEnvironment();

	protected:

		/// The database of this tracker.
		Database database_;

		/// The tracking quality of this tracker.
		TrackingQuality trackingQuality_ = TQ_AUTOMATIC;

		/// The horizontal field of view of the camera of this tracker in radian, with range (0, PI), -1 if the field of view is not known
		Scalar cameraFieldOfView_ = Scalar(-1);

		/// The optimization strategy for the camera profile of this tracker.
		PinholeCamera::OptimizationStrategy cameraOptimizationStrategy_ = PinholeCamera::OS_INTRINSIC_PARAMETERS_DISTORTIONS;

		/// The abstract motion type of this tracker (the abstract motion type is a user-defined motion type which can be defined to force a specific kind of camera motion and is updated to the tracker determined camera motion).
		AbstractMotionType abstractMotionType_ = AMT_UNKNOWN;

		/// The motion speed of this tracker (the visual information of the video frames respectively).
		PointPaths::MotionSpeed motionSpeed_ = PointPaths::MS_MODERATE;

		/// The region of interest of this tracker, if any.
		CV::SubRegion regionOfInterest_;

		/// The motion of the camera which has been detected by this tracker.
		Solver3::CameraMotion cameraMotion_ = Solver3::CM_INVALID;

		// True, if the tracker uses only the region of interest and not the remaining frame information for tracking
		bool soleRegionOfInterestApplication_ = false;

		/// The progress of this tracker for the current sub-task, with range [0, 1], -1 if undefined.
		Scalar localProgress_ = Scalar(-1);

		/// The callback function that has been registered at the scheduler.
		Scheduler::Callback schedulerCallback_;
};

inline SLAMTracker::ReliabilityPair::ReliabilityPair(const Scalar maximalCosine, const Scalar minimalObservationRatio) :
	maximalCosineValue_(maximalCosine),
	minimalObservationRatio_(minimalObservationRatio)
{
	ocean_assert(maximalCosineValue_ > 0 && maximalCosineValue_ < 1);
	ocean_assert(minimalObservationRatio_ > 0 && minimalObservationRatio_ <= 1);
}

inline Scalar SLAMTracker::ReliabilityPair::maximalCosine() const
{
	return maximalCosineValue_;
}

inline Scalar SLAMTracker::ReliabilityPair::minimalObservationRatio() const
{
	return minimalObservationRatio_;
}

template <unsigned int tLowerBoundary>
inline unsigned int SLAMTracker::ReliabilityPair::minimalObservations(const unsigned int overallObservation) const
{
	return max(min(tLowerBoundary, overallObservation), (unsigned int)(Scalar(overallObservation) * minimalObservationRatio_));
}

inline const CV::SubRegion& SLAMTracker::regionOfInterest() const
{
	return regionOfInterest_;
}

inline void SLAMTracker::setProgress(Scalar* progress, const Scalar value)
{
	ocean_assert(value >= 0 && value <= 1);

	if (progress)
	{
		*progress = value;
	}
}

}

}

}

#endif // META_OCEAN_TRACKING_OFFLINE_SLAM_TRACKER_H
