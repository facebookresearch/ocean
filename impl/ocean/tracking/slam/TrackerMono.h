/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_SLAM_TRACKER_MONO_H
#define META_OCEAN_TRACKING_SLAM_TRACKER_MONO_H

#include "ocean/tracking/slam/SLAM.h"
#include "ocean/tracking/slam/Tracker.h"
#include "ocean/tracking/slam/BackgroundTask.h"
#include "ocean/tracking/slam/CameraPose.h"
#include "ocean/tracking/slam/CameraPoses.h"
#include "ocean/tracking/slam/FramePyramidManager.h"
#include "ocean/tracking/slam/Gravities.h"
#include "ocean/tracking/slam/LocalizedObjectPoint.h"
#include "ocean/tracking/slam/Mutex.h"
#include "ocean/tracking/slam/OccupancyArray.h"
#include "ocean/tracking/slam/PointTrack.h"
#include "ocean/tracking/slam/PoseCorrespondences.h"
#include "ocean/tracking/slam/TrackingCorrespondences.h"

#include "ocean/base/Frame.h"
#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/Thread.h"

#include "ocean/cv/FramePyramid.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/RateCalculator.h"

namespace Ocean
{

namespace Tracking
{

namespace SLAM
{

/**
 * This class implements a monocular SLAM tracker.
 * @ingroup trackingslam
 */
class OCEAN_TRACKING_SLAM_EXPORT TrackerMono :
	public Tracker,
	protected Thread
{
	public:

		/**
		 * This class holds per-frame tracking statistics for debugging and analysis.
		 * Statistics include tracking counts, pose estimation data, projection errors, and the resulting camera pose.
		 */
		class FrameStatistics
		{
			public:

				/**
				 * Creates a new frame statistics object for a specific frame.
				 * @param frameIndex The index of the frame, with range [0, infinity)
				 */
				explicit inline FrameStatistics(const Index32 frameIndex);

				/**
				 * Returns whether the frame statistics contain valid data.
				 * @return True if valid; false otherwise
				 */
				bool isValid() const;

			public:

				/// The index of the frame these statistics belong to.
				Index32 frameIndex_ = Index32(-1);

				/// The number of feature points that could potentially be tracked from the previous frame.
				size_t frameToFrameTrackingPossible_ = 0;

				/// The number of feature points that were actually tracked successfully from the previous frame.
				size_t frameToFrameTrackingActual_ = 0;

				/// The number of 2D-3D correspondences used for pose estimation.
				size_t poseEstimationCorrespondences_ = 0;

				/// The average projection error of the estimated pose, negative if not computed.
				Scalar projectionError_ = Scalar(-1);

				/// The estimated camera pose, invalid if pose estimation failed.
				HomogenousMatrix4 world_T_camera_ = HomogenousMatrix4(false);

				/// The map version at the time of pose estimation.
				unsigned int mapVersion_ = 0u;
		};

		/// Definition of a vector holding frame statistics.
		using FramesStatistics = std::vector<FrameStatistics>;

	protected:

		/**
		 * Background object:
		 *
		 * This class implements an optimization for 3D object points that were not included in the main Bundle Adjustment.
		 * The class collects object points visible in at least two keyframes and optimizes their 3D positions using non-linear optimization with the already-optimized camera poses from Bundle Adjustment.
		 * @note The keyFrameIndices reference must remain valid for the lifetime of this object.
		 */
		class ObjectPointOptimization
		{
			protected:

				/**
				 * This class holds data for a single object point to be optimized.
				 */
				class OptimizationObject
				{
					public:

						/// The 3D position of the object point.
						Vector3 objectPoint_;

						/// The indices into the keyframe subset where this object point is observed.
						std::vector<size_t> keyFrameSubsetIndices_;

						/// The 2D image point observations corresponding to keyFrameSubsetIndices_.
						Vectors2 imagePoints_;
				};

				/// Definition of a map mapping object point ids to their optimization data.
				using OptimizationObjectMap = std::unordered_map<Index32, OptimizationObject>;

			public:

				/**
				 * Creates a new object point optimization object.
				 * @param keyFrameIndices The indices of keyframes to consider for optimization, must remain valid for the lifetime of this object
				 */
				explicit inline ObjectPointOptimization(const Indices32& keyFrameIndices);

				/**
				 * Collects object points that are visible in at least one keyframe and were not part of the previous Bundle Adjustment.
				 * For each collected object point, the function gathers all 2D observations across the keyframes.
				 * @param localizedObjectPointMap The map of all localized 3D object points with their observations
				 * @param previousBundleAdjustmentObjectPointIdSet The set of object point ids that were already optimized in Bundle Adjustment
				 */
				void collectObjectPoints(const LocalizedObjectPointMap& localizedObjectPointMap, const UnorderedIndexSet32& previousBundleAdjustmentObjectPointIdSet);

				/**
				 * Optimizes the collected object points using non-linear optimization with fixed camera poses.
				 * Object points visible in at least two keyframes are optimized. Successfully optimized points (with projection error below threshold) are added to the output vectors; others are marked as inaccurate.
				 * @param camera The camera profile used for projection, must be valid
				 * @param optimizedFlippedCameras_T_world The optimized flipped camera poses from Bundle Adjustment, one for each keyframe
				 * @param estimatorType The robust estimator type to use for optimization
				 * @param maximalProjectionError The maximum allowed projection error for a point to be considered accurate, in pixels
				 * @param currentBundleAdjustmentObjectPointIdSet The set of object point ids from Bundle Adjustment, will be extended with newly optimized points
				 * @param currentObjectPointIds The vector of optimized object point ids, will be extended with newly optimized points
				 * @param currentObjectPointPositions The vector of optimized object point positions, will be extended with newly optimized points
				 * @param inaccurateObjectPointIds The resulting ids of object points that could not be optimized accurately
				 */
				void optimizeObjectPointsIF(const AnyCamera& camera, const HomogenousMatrices4& optimizedFlippedCameras_T_world, const Geometry::Estimator::EstimatorType estimatorType, const Scalar maximalProjectionError, UnorderedIndexSet32& currentBundleAdjustmentObjectPointIdSet, Indices32& currentObjectPointIds, Vectors3& currentObjectPointPositions, Indices32& inaccurateObjectPointIds);

			protected:

				/// Reference to the keyframe indices to consider for optimization.
				const Indices32& keyFrameIndices_;

				/// The map of object points to be optimized.
				OptimizationObjectMap optimizationObjectMap_;
		};

		/**
		 * Definition of a pair combining a frame index with a 2D observation.
		 */
		using PoseIndexToImagePointPair = std::pair<unsigned int, Vector2>;

		/**
		 * Definition of a vector holding pairs of frame indices and 2D observations.
		 */
		using PoseIndexToImagePointPairs = std::vector<PoseIndexToImagePointPair>;

		/**
		 * Definition of an unordered map mapping object point ids to observation pairs.
		 */
		using ObjectPointToObservations = std::unordered_map<Index32, PoseIndexToImagePointPairs>;

		/**
		 * This class encapsulates all performance measurement logic for the TrackerMono.
		 * The class can be enabled or disabled at compile time via the isEnabled_ flag.<br>
		 * When disabled, all performance measurements become no-ops.
		 */
		class PerformanceStatistics
		{
			public:

				/// True, to enable performance measurements; False, to disable.
				static constexpr bool isEnabled_ = true;

				/**
				 * This class defines a scoped high performance statistic module.
				 * When isEnabled_ is false, this becomes a no-op.
				 */
				class ScopedStatistic
				{
					public:

						/**
						 * Creates a new scoped statistic object and starts a new measurement.
						 * @param statistic The specific statistic to measure
						 */
						inline explicit ScopedStatistic(HighPerformanceStatistic& statistic);

						/**
						 * Destructs a scoped statistic object and stops the measurement.
						 */
						inline ~ScopedStatistic();

					protected:

						/**
						 * Disabled copy constructor.
						 */
						ScopedStatistic(const ScopedStatistic&) = delete;

					protected:

						/// Pointer to the statistic object, nullptr if disabled.
						HighPerformanceStatistic* statistic_ = nullptr;
				};

				/**
				 * Starts a specific performance measurement.
				 * @param statistic The statistic to start
				 */
				inline void start(HighPerformanceStatistic& statistic) const;

				/**
				 * Stops a specific performance measurement.
				 * @param statistic The statistic to stop
				 */
				inline void stop(HighPerformanceStatistic& statistic) const;

				/**
				 * Returns a string with the relevant performance information.
				 * @return The formatted performance report
				 */
				std::string toString() const;

			public:

				/// Performance statistic for the handleFrame() function.
				HighPerformanceStatistic handleFrame_;

				/// Performance statistic for tracking image points.
				HighPerformanceStatistic trackImagePoints_;

				/// Performance statistic for updating the tracking database.
				HighPerformanceStatistic trackImagePointsDatabase_;

				/// Performance statistic for determining the camera pose.
				HighPerformanceStatistic determineCameraPose_;

				/// Performance statistic for optimizing bad object points.
				HighPerformanceStatistic optimizeBadObjectPoints_;

				/// Performance statistic for determining initial object points.
				HighPerformanceStatistic determineInitialObjectPoints_;

				/// Performance statistic for re-localization.
				HighPerformanceStatistic relocalize_;

				/// Performance statistic for Bundle Adjustment.
				HighPerformanceStatistic bundleAdjustment_;

				/// Performance statistic for detecting new image points.
				HighPerformanceStatistic detectNewImagePoints_;

				/// Performance statistic for matching unlocalized object points.
				HighPerformanceStatistic matchCornersToLocalizedObjectPoints_;

				/// Performance statistic for matching unlocalized object points.
				HighPerformanceStatistic matchLocalizedObjectPointsToCorners_;
		};

		/**
		 * Definition of a pair combining object point ids and object point positions.
		 */
		using ObjectPointIdPositionPair = std::pair<Index32, Vector3>;

		/**
		 * Definition of a vector holding object point id and position pairs.
		 */
		using ObjectPointIdPositionPairs = std::vector<ObjectPointIdPositionPair>;

	public:


		/**
		 * This class holds debug data for visualization and analysis purposes.
		 * The class maintains 2D point tracks across frames and their associated 3D object points.
		 */
		class OCEAN_TRACKING_SLAM_EXPORT DebugData
		{
			public:

				/// Definition of a pair combining the last frame index with a sequence of 2D image points.
				using TrackPair = std::pair<Index32, Vectors2>;

				/// Definition of a map mapping object point ids to their track data.
				using TracksMap = std::unordered_map<Index32, TrackPair>;

				/**
				 * This class holds information about a 3D object point for visualization purposes.
				 */
				class Point
				{
					public:

						/**
						 * Creates a default point object.
						 */
						Point() = default;

						/**
						 * Creates a new point object.
						 * @param position The 3D position of the object point
						 * @param precision The localization precision of the object point
						 */
						inline Point(const Vector3& position, const LocalizedObjectPoint::LocalizationPrecision precision);

					public:

						/// The 3D position of the object point.
						Vector3 position_ = Vector3::minValue();

						/// The localization precision of the object point.
						LocalizedObjectPoint::LocalizationPrecision precision_ = LocalizedObjectPoint::LP_INVALID;

						/// True if this object point has been optimized by Bundle Adjustment.
						bool isBundleAdjusted_ = false;
				};

				/// Definition of a map mapping object point ids to their 3D positions and precisions.
				using PointMap = std::unordered_map<Index32, Point>;

			public:

				/**
				 * Updates the tracking data with new frame correspondences.
				 * This function updates 2D tracks with new observations and removes invalid tracks.
				 * @param frameIndex The index of the current frame, with range [0, infinity)
				 * @param trackingCorrespondences The tracking correspondences containing point ids and image points
				 * @param poseCorrespondences The pose correspondences containing 3D object points and outlier information
				 */
				void update(const Index32 frameIndex, const TrackingCorrespondences& trackingCorrespondences, const PoseCorrespondences& poseCorrespondences);

				/**
				 * Updates the debug data by merging from another DebugData object.
				 * The tracksMap is replaced entirely while pointMap entries are merged.
				 * Object points marked as inaccurate in the source are removed from pointMap.
				 * @param debugData The source debug data to merge from, will be moved
				 */
				void update(DebugData&& debugData);

				/**
				 * Clears all debug data.
				 * Resets the object to its initial empty state.
				 */
				void clear();

			public:

				/// The map of 2D point tracks, mapping object point ids to their last frame index and sequence of 2D observations.
				TracksMap tracksMap_;

				/// The map of 3D object points, mapping object point ids to their 3D positions and localization precisions.
				PointMap pointMap_;

				/// The set of object point ids with precise localization used for pose estimation.
				UnorderedIndexSet32 posePreciseObjectPointIds_;

				/// The set of object point ids with imprecise localization used for pose estimation.
				UnorderedIndexSet32 poseNotPreciseObjectPointIds_;

				/// The set of object point ids that were identified as outliers during pose estimation.
				UnorderedIndexSet32 inaccurateObjectPointIdSet_;
		};

		/**
		 * Creates a new tracker object.
		 */
		TrackerMono();

		/**
		 * Destructs this tracker object.
		 */
		~TrackerMono() override;

		/**
		 * Configures the tracker with the specified settings.
		 * This function must be called before the first frame is processed.
		 * @param configuration The configuration object containing all tracker settings, must be valid
		 * @return True if configuration was successful
		 */
		bool configure(const Configuration& configuration);

		/**
		 * Processes a new camera frame and determines the camera pose.
		 * This is the main entry point for the tracker. The function tracks feature points from the previous frame, estimates the 6-DOF camera pose, and triggers background processing for map maintenance.<br>
		 * The tracker transitions from TS_INITIALIZING to TS_TRACKING once sufficient object points are tracked with adequate quality.
		 * @param camera The camera profile defining the projection, must be valid and consistent across all frames
		 * @param yFrame The current grayscale frame (FORMAT_Y8), will be moved, must be valid with matching dimensions
		 * @param world_T_camera The resulting camera pose transforming camera to world coordinates, invalid if pose could not be determined
		 * @param cameraGravity Optional gravity vector in camera coordinates (unit vector), can be null if unavailable
		 * @param anyWorld_Q_camera Optional orientation from an external source (e.g., IMU), can be invalid if unavailable
		 * @param debugData Optional pointer to receive debug data for visualization/analysis, nullptr to skip
		 * @return True if the frame was processed successfully; false on error
		 */
		bool handleFrame(const AnyCamera& camera, Frame&& yFrame, HomogenousMatrix4& world_T_camera, const Vector3& cameraGravity = Vector3(0, 0, 0), const Quaternion& anyWorld_Q_camera = Quaternion(false), DebugData* debugData = nullptr);

		/**
		 * Returns the index of the current frame which the tracker has just processed.
		 * @return The tracker's current frame index, with range [0, infinity)
		 */
		inline Index32 frameIndex() const;

		/**
		 * Returns a string with performance statistics for the tracker.
		 * Call only after the tracker has finished.
		 * @return The performance statistics as a formatted string
		 */
		std::string performance() const;

		/**
		 * Returns the collected per-frame statistics.
		 * Call only after the tracker has finished.
		 * @return The vector of frame statistics
		 */
		FramesStatistics framesStatistics() const;

	protected:

		/**
		 * Checks whether frame statistics collection is enabled and retrieves a pointer to the statistics for the current frame.
		 * This function is used to optionally gather per-frame tracking metrics (e.g., number of tracked points, projection errors) when statistics collection is enabled.
		 * @param frameIndex The index of the current frame to check, must match the frame index of the last entry in framesStatistics_
		 * @param frameStatistics The resulting pointer to the frame statistics object for the given frame, nullptr if statistics are disabled or unavailable
		 * @return True if frame statistics are enabled and available for the specified frame; false otherwise
		 */
		bool needsFrameStatistics(const Index32 frameIndex, FrameStatistics*& frameStatistics);

		/**
		 * Tracks image points from the previous frame to the current frame and determines the camera pose.
		 * This function performs frame-to-frame 2D point tracking using the frame pyramids, then estimates the 6-DOF camera pose using the tracked 2D-3D correspondences via a robust estimation approach.
		 * @param camera The camera profile to be used, must be valid
		 * @param currentFrameIndex The index of the current frame, with range [1, infinity)
		 * @param randomGenerator A random generator for the RANSAC-based pose estimation
		 * @param previousCamera_Q_currentCamera The rotation from the current camera frame to the previous camera frame (e.g., from IMU), can be invalid if not available
		 * @return The shared camera pose if pose estimation succeeded, nullptr otherwise
		 */
		SharedCameraPose trackImagePointsAndDeterminePose(const AnyCamera& camera, const Index32 currentFrameIndex, RandomGenerator& randomGenerator, const Quaternion& previousCamera_Q_currentCamera);

		/**
		 * Resets all localized 3D object points and related state during re-initialization.
		 * This function clears the map when localized object points are not precise enough and the tracker needs to restart initialization.<br>
		 * The observations of currently visible object points are preserved as unlocalized points for potential re-triangulation.
		 */
		void resetLocalizedObjectPoints();

		/**
		 * Post-processing function:
		 *
		 * Performs post-processing after a frame has been handled by the main tracking pipeline.
		 * This function initializes or clears the occupancy array, processes tracking results to update point tracks
		 * and localized object points, detects new feature points, and updates tracking correspondences for the next frame.
		 */
		void postHandleFrame();

		/**
		 * Post-processing function:
		 *
		 * Processes tracking results by updating point tracks and localized object points with new observations.
		 * @param currentFrameIndex The index of the current frame being processed, with range [0, infinity)
		 * @param trackingCorrespondences The tracking correspondences containing previous/current image points, point ids, and validity flags
		 */
		void processTrackingResults(const Index32 currentFrameIndex, const TrackingCorrespondences& trackingCorrespondences);

		/**
		 * Post-processing function:
		 *
		 * Detects new Harris corner features and adds them as unlocalized object points for tracking.
		 * This function first checks if the occupancy array needs more points.<br>
		 * Further, the function may match new feature points with existing localized object points.
		 * @param camera The camera model used for projection, must be valid
		 * @param currentFrameIndex The index of the current frame, with range [0, infinity)
		 * @param yFramePyramid The grayscale frame pyramid for corner detection, must be valid and match camera dimensions
		 * @param tryMatchCornersToLocalizedObjectPoints True to attempt matching corners to existing localized object points before adding as new tracks
		 * @return True if detection succeeded or was skipped due to sufficient coverage; false if corner detection failed
		 */
		bool detectNewImagePoints(const AnyCamera& camera, const Index32 currentFrameIndex, const CV::FramePyramid& yFramePyramid, const bool tryMatchCornersToLocalizedObjectPoints);

		/**
		 * Post-processing function:
		 *
		 * Matches newly detected Harris corners to existing localized 3D object points that are not currently being tracked.
		 * This function attempts to re-observe localized object points by projecting them into the current frame and matching nearby corners using FREAK descriptor comparison.<br>
		 * Successfully matched corners are removed from the input vector.
		 * @param camera The camera model used for projection, must be valid
		 * @param currentFrameIndex The index of the current frame, with range [0, infinity)
		 * @param cameraPose The current camera pose, must be valid
		 * @param yFramePyramid The grayscale frame pyramid for descriptor computation, must be valid and match camera dimensions
		 * @param corners The detected Harris corners to match; matched corners will be removed from this vector
		 */
		void matchCornersToLocalizedObjectPoints(const AnyCamera& camera, const Index32 currentFrameIndex, const CameraPose& cameraPose, const CV::FramePyramid& yFramePyramid, CV::Detector::HarrisCorners& corners);

		/**
		 * Post-processing function:
		 *
		 * Matches localized 3D object points to locally detected Harris corners.
		 * This function projects each untracked localized object point into the current frame, detects Harris corners in a small region around the projection, and matches using FREAK descriptor comparison.<br>
		 * Successfully matched points are added to the occupancy array.
		 * @param camera The camera model used for projection, must be valid
		 * @param currentFrameIndex The index of the current frame, with range [0, infinity)
		 * @param cameraPose The current camera pose, must be valid
		 * @param yFramePyramid The grayscale frame pyramid for corner detection and descriptor computation, must be valid and match camera dimensions
		 */
		void matchLocalizedObjectPointsToCorners(const AnyCamera& camera, const Index32 currentFrameIndex, const CameraPose& cameraPose, const CV::FramePyramid& yFramePyramid);

		/**
		 * The main loop of the background processing thread.
		 * @see Thread::threadRun()
		 */
		void threadRun() override;

		/**
		 * Background function:
		 *
		 * Determines initial 3D object points from 2D point correspondences during tracker initialization.
		 * This function uses stereoscopic geometry to triangulate initial object points from tracked 2D features between a suitable earlier frame and the latest frame.<br>
		 * @param camera The camera model used for projection, must be valid
		 * @param latestFrameIndex The index of the most recent frame
		 * @param randomGenerator A random generator for RANSAC-based pose estimation
		 * @return True if initialization succeeded and initial object points were determined; false otherwise
		 */
		bool determineInitialObjectPoints(const AnyCamera& camera, const Index32 latestFrameIndex, RandomGenerator& randomGenerator);

		/**
		 * Background function:
		 *
		 * Re-optimizes object points that have been marked as inaccurate.
		 * For each inaccurate object point that is visible in the current frame, the function tries to re-triangulate/optimize its 3D position.<br>
		 * Successfully optimized points are updated in the map; points that remain inaccurate are kept for potential recovery during the next Bundle Adjustment.
		 * @param camera The camera model used for projection, must be valid
		 * @param currentFrameIndex The index of the current/latest frame
		 * @param inaccurateObjectPointIdSet The set of object point ids known to be inaccurate, must not be empty
		 */
		void updateInaccurateObjectPoints(const AnyCamera& camera, const Index32 currentFrameIndex, const UnorderedIndexSet32& inaccurateObjectPointIdSet);

		/**
		 * Background function:
		 *
		 * Performs bundle adjustment optimization on camera poses and 3D object points.
		 * @param camera The camera model used for projection, must be valid
		 * @param currentFrameIndex The index of the current frame
		 */
		void bundleAdjustment(const AnyCamera& camera, const Index32 currentFrameIndex);

		/**
		 * Background function:
		 *
		 * Localizes unlocalized object points by triangulating their 3D positions from multiple observations.
		 * @param camera The camera profile used for triangulation, must be valid
		 * @param currentFrameIndex The index of the current frame up to which observations are considered
		 */
		void localizeUnlocalizedObjectPoints(const AnyCamera& camera, const Index32 currentFrameIndex);

		/**
		 * Background function:
		 *
		 * Attempts to re-localize the tracker by matching unlocalized point tracks to known localized 3D object points.
		 * The function computes FREAK descriptors for currently tracked 2D points and matches them against descriptors of localized object points.<br>
		 * Using RANSAC-based pose estimation, it determines the camera pose and merges successfully matched point tracks into their corresponding localized object points.
		 * @param camera The camera model used for projection, must be valid
		 * @param latestFrameIndex The index of the latest frame to use for re-localization
		 * @param yFramePyramid The frame pyramid of the latest frame for descriptor computation, must be valid
		 * @return True if re-localization succeeded; False otherwise
		 */
		bool relocalize(const AnyCamera& camera, const Index32 latestFrameIndex, const CV::FramePyramid& yFramePyramid);

		/**
		 * Post-processing function:
		 *
		 * Returns a new unique object point id.
		 * @return The unique object point id
		 */
		inline Index32 uniqueObjectPointId();

		/**
		 * Background function:
		 *
		 * Determines camera poses for frames that are missing valid poses.
		 * The function performs both a backward pass (from startFrameIndex towards earlier frames) and a forward pass (towards later frames).<br>
		 * For each frame without a pose, it uses the localized object points to estimate the camera pose via RANSAC-based PnP with optional gravity constraints.
		 * @param camera The camera model used for projection, must be valid
		 * @param startFrameIndex The frame index to start from (excluding), must be valid
		 * @param skipFrameIndex Optional frame index to skip during processing, Index32(-1) to skip none
		 * @param stopAtValidPose True to stop each pass when encountering a frame with a valid pose; False to continue processing all frames
		 * @tparam tUseReadLock True to acquire a read lock on the mutex; False if the caller already holds the lock
		 */
		template <bool tUseReadLock = true>
		void determineCameraPoses(const AnyCamera& camera, const Index32 startFrameIndex, const Index32 skipFrameIndex = Index32(-1), const bool stopAtValidPose = false);

		/**
		 * Background function:
		 *
		 * Determines whether bundle adjustment optimization should be triggered.
		 * The function checks three conditions and returns true if any is met:
		 * 1. The average reprojection error exceeds the specified threshold
		 * 2. Less than 50% of visible object points have been previously bundle-adjusted
		 * 3. The camera has moved significantly from all existing keyframe positions (baseline exceeds threshold)
		 * @param camera The camera model used for projection, must be valid
		 * @param currentCameraPose The current camera pose to evaluate
		 * @param currentFrameIndex The index of the current frame
		 * @param maximalProjectionError The maximum allowed average projection error before triggering bundle adjustment
		 * @param necessaryMapVersion The required map version for consistency checks (used in debug assertions)
		 * @return True if bundle adjustment is needed; false otherwise
		 */
		bool isBundleAdjustmentNeeded(const AnyCamera& camera, const CameraPose& currentCameraPose, const Index32 currentFrameIndex, const Scalar maximalProjectionError, const unsigned int necessaryMapVersion) const;

		/**
		 * Background function:
		 *
		 * Determines the topology for the bundle adjustment.
		 * The function selects a subset of keyframes to be used in the bundle adjustment.
		 * The selection strategy tries to maximize the spatial distribution of keyframes while ensuring sufficient feature overlap.
		 * @param necessaryDataVersion The required map version; only frames with camera poses matching this version are considered
		 * @param cameraPoses The camera poses of all frames, must be valid
		 * @param localizedObjectPointMap The map of localized object points, must be valid
		 * @param maximalNumberNewKeyFrames The maximal number of new keyframes to add in this call, with range [1, infinity)
		 * @param maximalNumberKeyFrames The maximal number of keyframes to be selected, with range [2, infinity)
		 * @param keyFrameIndices The indices of the selected keyframes, will be updated with new keyframes and may have old ones removed
		 * @param objectPointToObservations The resulting map mapping object point ids to their observations in the selected keyframes
		 * @param minimalNumberKeyFrames The minimal number of keyframes to be selected, with range [2, maximalNumberKeyFrames]
		 * @param maximalFrameHistory The maximal frame history to consider when selecting keyframes, with range [1, infinity)
		 * @return True if the topology was successfully determined with at least minimalNumberKeyFrames; false otherwise
		 */
		static bool determineBundleAdjustmentTopology(const Index32 necessaryDataVersion, const CameraPoses& cameraPoses, const LocalizedObjectPointMap& localizedObjectPointMap, const size_t maximalNumberNewKeyFrames, const size_t maximalNumberKeyFrames, Indices32& keyFrameIndices, ObjectPointToObservations& objectPointToObservations, const size_t minimalNumberKeyFrames = 3, const size_t maximalFrameHistory = 300);

		/**
		 * Background function:
		 *
		 * Computes and adds FREAK visual descriptors to localized object points that need descriptors.
		 * Descriptors are used for re-matching object points when they lose frame-to-frame tracking.
		 * @param camera The camera model used for descriptor computation, must be valid
		 * @param currentFrameIndex The index of the current frame, with range [0, infinity)
		 * @param yFramePyramid The grayscale frame pyramid for descriptor computation, must be valid and match camera dimensions
		 */
		void describeObjectPoints(const AnyCamera& camera, const Index32 currentFrameIndex, const CV::FramePyramid& yFramePyramid);

		/**
		 * Determines the frame index with the most visible localized object points.
		 * This function iterates through all frames with valid camera poses and counts how many
		 * localized object points have observations in each frame, returning the frame with the highest count.
		 * Only frames whose camera pose has a matching map version are considered.
		 * @param necessaryDataVersion The required map version; only frames with camera poses matching this version are considered
		 * @param cameraPoses The camera poses for all frames, must not be empty
		 * @param localizedObjectPointMap The map of localized 3D object points with their observations
		 * @param minimalNumberObjectPoints The minimal number of object points that must be visible in a frame for it to be considered valid, with range [1, infinity)
		 * @param ignoreFrameIndices Optional set of frame indices to skip during the search, nullptr to consider all frames
		 * @return The index of the frame with the most visible object points (meeting the threshold), or Index32(-1) if no valid frame was found
		 */
		static Index32 frameIndexWithMostLocalizedObjectPoints(const Index32 necessaryDataVersion, const CameraPoses& cameraPoses, const LocalizedObjectPointMap& localizedObjectPointMap, const size_t minimalNumberObjectPoints, const UnorderedIndexSet32* ignoreFrameIndices = nullptr);

		/**
		 * Returns the maximal distance between two descriptors so that they are considered a match (35% of descriptor size).
		 * @return The descriptor matching threshold
		 */
		static constexpr unsigned int descriptorThreshold();

	protected:

		/// The current operational state of the tracker, modified only by the foreground thread but read by both foreground and background threads.
		std::atomic<TrackerState> trackerState_ = TS_UNKNOWN;

		/// The configuration of the tracker.
		Configuration configuration_;

		/// The tracking parameters defining pyramid configuration for feature tracking.
		TrackingParameters trackingParameters_;

		/// The occupancy array for spatial distribution of feature points across the image.
		OccupancyArray occupancyArray_;

		/// The map of 2D point tracks, mapping object point ids to their tracked 2D observations across frames.
		PointTrackMap pointTrackMap_;

		/// The map of localized 3D object points, mapping object point ids to their 3D positions and observation history.
		LocalizedObjectPointMap localizedObjectPointMap_;

		/// The counter for generating unique object point ids.
		Index32 objectPointIdCounter_ = 0u;

		/// The manager for frame pyramids providing thread-safe access to image pyramids for foreground and background processing.
		FramePyramidManager framePyramidManager_;

		/// The adaptive Harris corner detection threshold, adjusted dynamically based on feature coverage.
		unsigned int harrisThreshold_ = 0u;

		/// The frame pyramid of the previous frame used for frame-to-frame feature tracking.
		FramePyramidManager::ScopedPyramid previousPyramid_;

		/// The frame pyramid of the current frame used for frame-to-frame feature tracking.
		FramePyramidManager::ScopedPyramid currentPyramid_;

		/// The orientation of the previous camera in an external/arbitrary world coordinate system (e.g., from IMU), used to compute frame-to-frame rotation.
		Quaternion anyWorld_Q_previousCamera_ = Quaternion(false);

		/// Frame-to-frame tracking correspondences.
		TrackingCorrespondences trackingCorrespondences_;

		/// Pose estimation correspondences.
		PoseCorrespondences poseCorrespondences_;

		/// The history of camera poses for all processed frames.
		CameraPoses cameraPoses_;

		/// The history of gravity vectors in camera coordinates for processed frames.
		Gravities gravities_;

		/// The camera profile used for projection, cloned from the first frame's camera.
		SharedAnyCamera camera_;

		/// The random generator for the foreground thread.
		RandomGenerator randomGenerator_;

		/// The random generator for the background thread.
		RandomGenerator randomGeneratorBackground_;

		/// The frame indices of keyframes used in the most recent Bundle Adjustment.
		Indices32 bundleAdjustmentKeyFrameIndices_;

		/// The squared baseline (camera translation distance) from the most recent Bundle Adjustment, used to trigger new optimizations.
		Scalar bundleAdjustmentSqrBaseline_ = Numeric::minValue();

		/// The ids of object points which have been used during the previous Bundle Adjustment.
		UnorderedIndexSet32 bundleAdjustmentObjectPointIdSet_;

		/// The performance statistics for this tracker.
		PerformanceStatistics performanceStatistics_;

		/// The set of object point ids whose 3D positions are considered inaccurate and need re-optimization.
		LocalizedObjectPoint::ObjectPointIdSet inaccurateObjectPointIdSet_;

		/// The background task which will execute the post processing for the handleFrame() function.
		BackgroundTask postHandleFrameTask_;

		/// The rate calculator for measuring the frame processing rate.
		RateCalculator handleFrameRateCalculator_;

		/// The debug data for visualization and analysis purposes.
		DebugData debugData_;

		/// The mutex for synchronizing access to shared data between foreground and background threads.
		mutable Mutex mutex_;

		/// True, if the background thread needs to determine initial object points. Only the foreground thread can set this task, only the background thread can clear the task.
		std::atomic_bool taskDetermineInitialObjectPoints_ = false;

		/// The index of the frame in which post estimation failed the first time, -1 if a valid pose is known
		Index32 poseEstimationFailedFrameIndex_ = Index32(-1);

		/// The version counter for the map, incremented after each Bundle Adjustment to track pose and object point consistency.
		Index32 mapVersion_ = 0u;

		/// The minimal localization precision for projecting object points; points below this threshold use the previous 2D position instead.
		static constexpr LocalizedObjectPoint::LocalizationPrecision minimalFrontPrecision_ = LocalizedObjectPoint::LP_LOW;

		/// True to enable collecting per-frame statistics; false to disable.
		bool frameStatisticsEnabled_ = true;

		/// The collected statistics for each processed frame.
		FramesStatistics framesStatistics_;

	private:

		/// Reusable pairs of object point ids and object point positions, used in updateInaccurateObjectPoints().
		ObjectPointIdPositionPairs reusableObjectPointIdPositionPairs_;
};

inline TrackerMono::FrameStatistics::FrameStatistics(const Index32 frameIndex) :
	frameIndex_(frameIndex)
{
	// nothing to do here
}

inline TrackerMono::ObjectPointOptimization::ObjectPointOptimization(const Indices32& keyFrameIndices) :
	keyFrameIndices_(keyFrameIndices)
{
	optimizationObjectMap_.reserve(256);
}

inline TrackerMono::DebugData::Point::Point(const Vector3& position, const LocalizedObjectPoint::LocalizationPrecision precision) :
	position_(position),
	precision_(precision)
{
	// nothing to do here
}

inline TrackerMono::PerformanceStatistics::ScopedStatistic::ScopedStatistic(HighPerformanceStatistic& statistic)
{
	if constexpr (isEnabled_)
	{
		statistic_ = &statistic;
		statistic_->start();
	}
}

inline TrackerMono::PerformanceStatistics::ScopedStatistic::~ScopedStatistic()
{
	if constexpr (isEnabled_)
	{
		if (statistic_ != nullptr)
		{
			statistic_->stop();
		}
	}
}

inline void TrackerMono::PerformanceStatistics::start(HighPerformanceStatistic& statistic) const
{
	if constexpr (isEnabled_)
	{
		statistic.start();
	}
}

inline void TrackerMono::PerformanceStatistics::stop(HighPerformanceStatistic& statistic) const
{
	if constexpr (isEnabled_)
	{
		statistic.stop();
	}
}

inline Index32 TrackerMono::frameIndex() const
{
	return cameraPoses_.frameIndex();
}

inline Index32 TrackerMono::uniqueObjectPointId()
{
	// now thread-safty necessary as the function is only called from one place

	const Index32 id = ++objectPointIdCounter_;

	return id;
}

constexpr unsigned int TrackerMono::descriptorThreshold()
{
	return CV::Detector::FREAKDescriptor32::descriptorMatchingThreshold(35u);
}

}

}

}

#endif // META_OCEAN_TRACKING_SLAM_TRACKER_MONO_H
