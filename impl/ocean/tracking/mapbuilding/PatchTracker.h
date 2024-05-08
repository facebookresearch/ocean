/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_MAPBUILDING_PATCH_TRACKER_H
#define META_OCEAN_TRACKING_MAPBUILDING_PATCH_TRACKER_H

#include "ocean/tracking/mapbuilding/MapBuilding.h"
#include "ocean/tracking/mapbuilding/DescriptorHandling.h"
#include "ocean/tracking/mapbuilding/Unified.h"

#include "ocean/base/RandomGenerator.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/FramePyramid.h"

#include "ocean/cv/detector/HarrisCorner.h"

#include "ocean/devices/DevicePlayer.h"

#include "ocean/geometry/Estimator.h"

#include "ocean/math/AnyCamera.h"

#include "ocean/tracking/Database.h"

namespace Ocean
{

namespace Tracking
{

namespace MapBuilding
{

/**
 * This class implements a tracker for 3D feature points in an image sequence (offline) or a live video with SLAM (online).
 * Feature points are tracked with a patch tracking approach.<br>
 * The tracker relies on a precise 6-DOF camera pose for each individual frame.
 *
 * Two tracking modes exist:<br>
 * 1) 2D features are tracked from frame-to-frame and their corresponding 3D location is eventually determined<br>
 * 2) 3D features are tracked from frame-to-frame while using the projected image location as prediction to reduce the search radius
 *
 * The tracker creates a database with the topology of all detected and tracked features.
 * @ingroup trackingmapbuilding
 */
class OCEAN_TRACKING_MAPBUILDING_EXPORT PatchTracker : public DescriptorHandling
{
	public:

		/**
		 * Definition of a shared FramePyramid.
		 */
		typedef std::shared_ptr<CV::FramePyramid> SharedFramePyramid;

		/**
		 * This class implement a container holding options for the tracker.
		 */
		class Options
		{
			public:

				/**
				 * Creates new options.
				 * @param maximalFeaturesPerFrame The maximal number of features which will be managed in each frame, with range [1, infinity)
				 * @param newFeaturesInterval The time interval between frames in which new feature points will be added to the tracker, in seconds, with range [0, infinity)
				 * @param keepUnlocatedFeatures True, to keep unlocated features in the database; False, to keep only located features (with known 3D location)
				 * @param minimalNumberObservationsPerFeature The minimal number of observations a feature must have, with range [2, infinity)
				 * @param minimalBoxDiagonalForLocatedFeature The minimal diagonal of the bounding box of all camera poses observing an object point necessary so that the point will be located, with range [0, infinity)
				 */
				inline Options(const size_t maximalFeaturesPerFrame, const double newFeaturesInterval, const bool keepUnlocatedFeatures, const size_t minimalNumberObservationsPerFeature, const Scalar minimalBoxDiagonalForLocatedFeature);

				/**
				 * Creates default options for realtime execution.
				 * @return Options with realtime execution purpose
				 */
				static inline Options realtimeOptions();

				/**
				 * Creates default options for offline execution.
				 * @return Options with offline execution purpose
				 */
				static inline Options offlineOptions();

			public:

				/// The maximal number of features which will be managed in each frame.
				size_t maximalFeaturesPerFrame_ = 0;

				/// The time interval between frames in which new feature points will be added to the tracker, in seconds.
				double newFeaturesInterval_ = -1.0;

				/// True, to keep unlocated features in the database; False, to keep only located features (with known 3D location).
				bool keepUnlocatedFeatures_ = false;

				/// The minimal number of observations a feature must have, with range [2, infinity).
				size_t minimalNumberObservationsPerFeature_ = 0;

				/// The minimal diagonal of the bounding box of all camera poses observing an object point necessary so that the point will be located.
				Scalar minimalBoxDiagonalForLocatedFeature_ = 0;
		};

	protected:

		/**
		 * Definition of individual location results.
		 */
		enum LocationResult : uint32_t
		{
			/// The location could not yet be determined.
			LR_NOT_YET,
			/// The location is flaky and thus the object point should not be used.
			LR_FLAKY,
			/// The location is precise.
			LR_PRECISE
		};

		/**
		 * This class implements the base class for all tracking data objects.
		 */
		class OCEAN_TRACKING_MAPBUILDING_EXPORT TrackingData
		{
			public:

				/**
				 * Destructs the object.
				 */
				virtual ~TrackingData() = default;

				/**
				 * Returns whether this object holds valid data.
				 * @return True, if so
				 */
				virtual bool isValid() const;

			protected:

				/// The image pyramid associated with this tracking data.
				SharedFramePyramid yPreviousFramePyramid_;

				/// The ids of the object points associated with this tracking data.
				Indices32 objectPointIds_;

				/// the 2D image points associated with this tracking data.
				Vectors2 previousImagePoints_;
		};

		/**
		 * This class holds the tracking data for feature points for which the 3D location is not yet known.
		 */
		class UnlocatedTrackingData : public TrackingData
		{
			friend class PatchTracker;

			protected:

				/// A vector with Harris corners, can be re-used for each new frame.
				CV::Detector::HarrisCorners reusableHarrisCorners_;

				/// The timestamp when the last time new unlocated features have been added.
				Timestamp lastNewFeaturesAddedTimestamp_;
		};

		/**
		 * This class holds the tracking data for feature points for which the 3D location is know.
		 */
		class LocatedTrackingData : public TrackingData
		{
			friend class PatchTracker;

			public:

				/**
				 * Returns whether this object holds valid data.
				 * @return True, if so
				 */
				bool isValid() const override;

			protected:

				/// The 3D locations of all feature points.
				Vectors3 objectPoints_;

				/// The translational part of the camera pose for each 3D object point in the moment the initial location of the 3D object point was determined.
				Vectors3 poseTranslationsWhenDeterminedObjectPoints_;

				/// A vector with 2D image points, can be re-used for each new frame.
				Vectors2 reusablePredictedCurrentImagePoints_;

				/// The number of consecutive frames without good tracking (e.g., because of quick camera movements).
				unsigned int framesWithoutGoodTracking_ = 0u;
		};

	public:

		/**
		 * Creates a new tracker object with specified descriptor extractor.
		 * @param unifiedDescriptorExtractor The feature extractor to be used, must be valid
		 * @param options The options to be used
		 */
		explicit inline PatchTracker(const std::shared_ptr<UnifiedDescriptorExtractor>& unifiedDescriptorExtractor, const Options& options = Options::offlineOptions());

		/**
		 * Tracks the features from the previous frame to the current frame.
		 * @param frameIndex The index of the current frame, with range [0, infinity)
		 * @param anyCamera The camera profile of the current image, must be valid
		 * @param world_T_camera The known precise camera pose for the current frame, with default viewing direction towards the negative z-space with y-axis up, must be valid
		 * @param yCurrentFramePyramid The image pyramid of the current image, with pixel FORMAT_Y8
		 * @param frameTimestamp The timestamp of the current frame, must be valid
		 * @param worker Optional worker to distribute the computation
		 * @param debugFrame Optional resulting debug frame visualizing the current state, nullptr if not of interest
		 * @return True, if succeeded
		 */
		bool trackFrame(const Index32 frameIndex, const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_camera, const SharedFramePyramid& yCurrentFramePyramid, const Timestamp& frameTimestamp, Worker* worker = nullptr, Frame* debugFrame = nullptr);

		/**
		 * Returns the current database of the tracker.
		 * @return The tracker's current database
		 */
		inline const Database& database() const;

		/**
		 * Returns the current descriptor map of the tracker.
		 * @return The tracker's current descriptor map
		 */
		inline std::shared_ptr<UnifiedDescriptorMap> unifiedDescriptorMap() const;

		/**
		 * Returns the 3D locations of all currently known located 3D object points.
		 * @return All 3D object points
		 */
		Vectors3 latestObjectPoints(Indices32* objectPointIds = nullptr) const;

		/**
		 * Resets the tracker so that it can be used for a new tracking session.
		 * @param database Optional resulting database of the tracker
		 * @param unifiedDescriptorMap Optional resulting descriptor map of the tracker
		 */
		void reset(Database* database = nullptr, std::shared_ptr<UnifiedDescriptorMap>* unifiedDescriptorMap = nullptr);

		/**
		 * Runs the tracker on a recording provided through a device player.
		 * @param devicePlayer The device player providing the recording information, must be valid
		 * @param worldTrackerNames The names of world tracker which may be available in the recording and which will be used, at least one
		 * @param database The resulting database holding the topology of the tracked features
		 * @param anyCamera The resulting camera profile for the entire sequence
		 * @param descriptorMap The resulting map mapping object point ids to descriptors
		 * @param unifiedDescriptorExtractor The feature extractor to be used, must be valid
		 * @return True, if succeeded
		 */
		static bool trackRecording(Devices::DevicePlayer& devicePlayer, const std::vector<std::string>& worldTrackerNames, Database& database, SharedAnyCamera& anyCamera, std::shared_ptr<UnifiedDescriptorMap>& descriptorMap, const std::shared_ptr<UnifiedDescriptorExtractor>& unifiedDescriptorExtractor);

		/**
		 * Removes flaky object points from the database.
		 * @param database The database from which the flaky object points will be removed
		 * @param minimalNumberObservations The minimal number of observations each object point must have to count as not-flaky, with range [1, infinity)
		 * @param mimimalBoxDiagonal The minimal diagonal of the bounding box of all camera poses (in which the object point is visible) so that the object point does not count as flaky, with range (0, infinity)
		 * @param removedObjectPointIds Optional resulting ids of all object points which have been removed, nullptr if not of interest
		 * @return The number of removed object points
		 */
		static size_t removeFlakyObjectPoints(Database& database, const size_t minimalNumberObservations, const Scalar mimimalBoxDiagonal, Indices32* removedObjectPointIds = nullptr);

	protected:

		/**
		 * Tracks the unlocated feature points from the previous frame to the current frame.
		 * @param frameIndex The index of the current frame, with range [0, infinity)
		 * @param yCurrentFramePyramid The frame pyramid of the current frame, with format FORMAT_Y8
		 * @param occupancyArray The occupancy array containing all currently tracked 2D feature points
		 * @param worker Optional worker to distribute the computation
		 */
		void trackUnlocatedPoints(const Index32 frameIndex, const SharedFramePyramid& yCurrentFramePyramid, Geometry::SpatialDistribution::OccupancyArray& occupancyArray, Worker* worker);

		/**
		 * Tracks the located feature points from the previous frame to the current frame.
		 * @param frameIndex The index of the current frame, with range [0, infinity)
		 * @param currentAnyCamera The current camera profile defining the projection, must be valid
		 * @param world_T_currentCamera The current camera pose, transforming camera to world, with default viewing direction into the negative z-space and y-axis up, must be valid
		 * @param yCurrentFramePyramid The frame pyramid of the current frame, with format FORMAT_Y8
		 * @param occupancyArray The occupancy array containing all currently tracked 2D feature points
		 * @param worker Optional worker to distribute the computation
		 */
		void trackLocatedPoints(const Index32 frameIndex, const AnyCamera& currentAnyCamera, const HomogenousMatrix4& world_T_currentCamera, const SharedFramePyramid& yCurrentFramePyramid, Geometry::SpatialDistribution::OccupancyArray& occupancyArray, Worker* worker);

		/**
		 * Adds new unlocated 2D feature points into image regions without feature points.
		 * @param frameIndex The index of the current frame, with range [0, infinity)
		 * @param currentAnyCamera The current camera profile defining the projection, must be valid
		 * @param yCurrentFramePyramid The frame pyramid of the current frame, with format FORMAT_Y8
		 * @param frameTimestamp The timestamp of the frame, must be valid
		 * @param occupancyArray The occupancy array containing all currently tracked 2D feature points
		 * @param worker Optional worker to distribute the computation
		 */
		void addUnlocatedPoints(const Index32 frameIndex, const AnyCamera& currentAnyCamera, const CV::FramePyramid& yCurrentFramePyramid, const Timestamp& frameTimestamp, Geometry::SpatialDistribution::OccupancyArray& occupancyArray, Worker* worker);

		/**
		 * Converts unlocated 2D feature points to located 3D feature points.
		 * @param currentAnyCamera The current camera profile defining the projection, must be valid
		 * @param world_T_currentCamera The current camera pose, transforming camera to world, with default viewing direction into the negative z-space and y-axis up, must be valid
		 */
		void convertUnlocatedPointsToLocatedPoints(const AnyCamera& currentAnyCamera, const HomogenousMatrix4& world_T_currentCamera);

		/**
		 * Determines the location of a 3D feature point.
		 * @param currentAnyCamera The current camera profile defining the projection, must be valid
		 * @param objectPointId The id of the object point for which the location will be determined, must be valid
		 * @param estimatorType The robust estimator to be used when optimizing the 3D location
		 * @param objectPoint The resulting 3D location of the object point
		 * @return The location result
		 */
		LocationResult determineObjectPointLocation(const AnyCamera& currentAnyCamera, const Index32 objectPointId, const Geometry::Estimator::EstimatorType estimatorType, Vector3& objectPoint);

	protected:

		/// The tracker's options.
		Options options_;

		/// The database holding the topology of the tracked features.
		Database database_;

		/// The tracking data for all unlocated feature points.
		UnlocatedTrackingData unlocatedTrackingData_;

		/// The tracking data for all located feature points (for which a 3D location is known).
		LocatedTrackingData locatedTrackingData_;

		/// An occupancy array, can be reused for each new frame.
		Geometry::SpatialDistribution::OccupancyArray reusableOccupancyArray_;

		/// True, if the image pyramid from which descriptors will be extracted needs to be updated; False, if no update is necessary.
		bool needToUpdateFramePyramidForDescriptors_ = true;

		/// The image pyramid for descriptor extraction, can be reused for each new frame.
		CV::FramePyramid yReusableFramePyramidForDescriptors_;

		/// The map mapping object point ids to feature descriptors.
		std::shared_ptr<UnifiedDescriptorMap> unifiedDescriptorMap_;

		/// The feature extractor to be used.
		std::shared_ptr<UnifiedDescriptorExtractor> unifiedDescriptorExtractor_;

		/// The random generator to be used.
		RandomGenerator randomGenerator_;

		/// Reusable indices.
		Indices32 reusableValidIndices_;

		/// Reusable 2D points.
		Vectors2 reusableCurrentImagePoints_;

		/// Reusable pose indices.
		Indices32 reusabledPoseIds_;

		/// Reusable point indices.
		Indices32 reusableImagePointIds_;

		/// Reusable 2D points.
		Vectors2 reusableImagePoints_;
};

inline PatchTracker::Options::Options(const size_t maximalFeaturesPerFrame, const double newFeaturesInterval, const bool keepUnlocatedFeatures, const size_t minimalNumberObservationsPerFeature, const Scalar minimalBoxDiagonalForLocatedFeature) :
	maximalFeaturesPerFrame_(maximalFeaturesPerFrame),
	newFeaturesInterval_(newFeaturesInterval),
	keepUnlocatedFeatures_(keepUnlocatedFeatures),
	minimalNumberObservationsPerFeature_(minimalNumberObservationsPerFeature),
	minimalBoxDiagonalForLocatedFeature_(minimalBoxDiagonalForLocatedFeature)
{
	ocean_assert(maximalFeaturesPerFrame_ >= 1);
	ocean_assert(newFeaturesInterval_ >= 0.0);
	ocean_assert(minimalNumberObservationsPerFeature_ >= 2);
	ocean_assert(minimalBoxDiagonalForLocatedFeature_ >= 0);
}

inline PatchTracker::Options PatchTracker::Options::realtimeOptions()
{
	constexpr size_t maximalFeaturesPerFrame_ = 400;
	constexpr double newFeaturesInterval = 0.25;

	constexpr bool keepUnlocatedFeatures = false;
	constexpr size_t minimalNumberObservationsPerFeature = 30;
	constexpr Scalar minimalBoxDiagonalForLocatedFeature = Scalar(0.1); // 10cm

	return Options(maximalFeaturesPerFrame_, newFeaturesInterval, keepUnlocatedFeatures, minimalNumberObservationsPerFeature, minimalBoxDiagonalForLocatedFeature);
}

inline PatchTracker::Options PatchTracker::Options::offlineOptions()
{
	constexpr size_t maximalFeaturesPerFrame_ = 1200;
	constexpr double newFeaturesInterval = 0.0;

	constexpr bool keepUnlocatedFeatures = true;
	constexpr size_t minimalNumberObservationsPerFeature = 30;
	constexpr Scalar minimalBoxDiagonalForLocatedFeature = Scalar(0.05); // 5cm

	return Options(maximalFeaturesPerFrame_, newFeaturesInterval, keepUnlocatedFeatures, minimalNumberObservationsPerFeature, minimalBoxDiagonalForLocatedFeature);
}

inline PatchTracker::PatchTracker(const std::shared_ptr<UnifiedDescriptorExtractor>& unifiedDescriptorExtractor, const Options& options) :
	options_(options),
	unifiedDescriptorExtractor_(std::move(unifiedDescriptorExtractor))
{
	unifiedDescriptorMap_ = unifiedDescriptorExtractor_->createUnifiedDescriptorMap();
}

inline const Database& PatchTracker::database() const
{
	return database_;
}

inline std::shared_ptr<UnifiedDescriptorMap> PatchTracker::unifiedDescriptorMap() const
{
	return unifiedDescriptorMap_;
}

}

}

}

#endif // META_OCEAN_TRACKING_MAPBUILDING_PATCH_TRACKER_H
