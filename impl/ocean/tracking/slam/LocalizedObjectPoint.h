/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_SLAM_LOCALIZED_OBJECT_POINT_H
#define META_OCEAN_TRACKING_SLAM_LOCALIZED_OBJECT_POINT_H

#include "ocean/tracking/slam/SLAM.h"
#include "ocean/tracking/slam/CameraPoses.h"
#include "ocean/tracking/slam/Observation.h"
#include "ocean/tracking/slam/PointTrack.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/cv/detector/FREAKDescriptor.h"

#include "ocean/io/Bitstream.h"

#include "ocean/geometry/Estimator.h"
#include "ocean/geometry/GravityConstraints.h"

#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace Tracking
{

namespace SLAM
{

// Forward declaration.
class LocalizedObjectPoint;

/**
 * Definition of an unordered map mapping object point ids to localized object points.
 * @ingroup trackingslam
 */
using LocalizedObjectPointMap = std::unordered_map<Index32, LocalizedObjectPoint>;

/**
 * This class implements a localized 3D object point.
 * In contrast to an unlocalized object point, a localized object point has a known 3D position and holds visual descriptors for observations of the object point.
 * A localized object point may (not yet) have a valid known 3D location - e.g., because there have not been enough observation from individual viewing angles.
 * @ingroup trackingslam
 */
class OCEAN_TRACKING_SLAM_EXPORT LocalizedObjectPoint
{
	public:

		/**
		 * Definition of possible localization precisions.
		 */
		enum LocalizationPrecision : uint8_t
		{
			/// An invalid localization precision.
			LP_INVALID = 0u,
			/// The localization precision could not yet be decided (e.g., because of too few observations).
			LP_UNKNOWN,
			/// The localization precision is low, because the object point has not been observed from enough different viewing angles.
			LP_LOW,
			/// The localization precision is medium, because the object point has been observed from quite narrow viewing angles.
			LP_MEDIUM,
			/// The localization precision is high, because the object point has been observed from several different viewing angles.
			LP_HIGH
		};

		/**
		 * Definition of a vector holding localization precisions.
		 */
		using LocalizationPrecisions = std::vector<LocalizationPrecision>;

		/**
		 * Definition of individual optimization results.
		 */
		enum OptimizationResult : uint32_t
		{
			/// The optimization failed because the object point does not have enough observations.
			OR_NOT_ENOUGH_OBSERVATIONS,
			/// The optimization failed because the object point location does not fit to all observations.
			OR_INACCURATE,
			/// The optimization succeeded.
			OR_SUCCEEDED
		};

		/**
		 * This class implements a container for correspondences between object points and image points which can be reused to reduce memory re-allocations.
		 */
		class OCEAN_TRACKING_SLAM_EXPORT CorrespondenceData
		{
			public:

				/**
				 * Applies the subset of used indices to filter the correspondence data.
				 * This function removes correspondences that are not in the used indices and populates the bad object point ids.
				 */
				void applySubset();

				/**
				 * Resets this data object so that it can be re-used.
				 */
				void reset();

				/**
				 * Returns whether the subset has already been applied.
				 * @return True, if so
				 */
				inline bool isSubsetApplied() const;

				/**
				 * Returns whether this correspondence data object is empty.
				 * @return True, if so
				 */
				inline bool isEmpty() const;

			public:

				/// The 3D object points.
				Vectors3 objectPoints_;

				/// The 2D image points corresponding to the object points.
				Vectors2 imagePoints_;

				/// The ids of the object points.
				Indices32 objectPointIds_;

				/// The localization precisions of the object points.
				LocalizationPrecisions localizationPrecisions_;

				/// The individual squared distances between previous and current image points, one for each observation, in squared pixel; empty if correspondences are not based on frame-to-frame tracking.
				Scalars imagePointSqrDistances_;

				/// The indices of the used correspondences after pose determination.
				Indices32 usedIndices_;

				/// The ids of object points that were rejected during pose determination.
				Indices32 badObjectPointIds_;

				/// The ids of object points with precise localization used for the pose, for debugging and visualization.
				UnorderedIndexSet32 posePreciseObjectPointIds_;

				/// The ids of object points with imprecise localization used for the pose, for debugging and visualization.
				UnorderedIndexSet32 poseNotPreciseObjectPointIds_;
		};

		/**
		 * This class implements a thread-safe container for object point ids.
		 * The container allows adding object point ids from multiple threads and retrieving them atomically.
		 */
		class ObjectPointIdSet
		{
			public:

				/**
				 * Adds object point ids to this container.
				 * This function is thread-safe.
				 * @param objectPointIds The object point ids to add
				 */
				inline void add(const Indices32& objectPointIds);

				/**
				 * Returns and clears all object point ids from this container.
				 * This function is thread-safe.
				 * @return The object point ids that were stored in this container
				 */
				inline UnorderedIndexSet32 objectPointIds();

				/**
				 * Clears all object point ids from this container.
				 * This function is thread-safe.
				 */
				inline void clear();

			protected:

				/// The set of object point ids.
				UnorderedIndexSet32 objectPointIdSet_;

				/// The lock for thread-safe access.
				Lock lock_;
		};

	protected:

		/**
		 * Definition of an unordered map mapping camera indices to 2D observations.
		 */
		using ObservationMap = std::unordered_map<Index32, Vector2>;

	public:

		/**
		 * Creates a new localized object point from an unlocalized object point.
		 * The resulting localized object point will not yet contain a valid position for the object point.
		 * @param pointTrack The unlocalized point track containing the observations, must be valid
		 */
		explicit LocalizedObjectPoint(const PointTrack& pointTrack);

		/**
		 * Creates a new localized object point from an unlocalized object point for which the 3D position is already known.
		 * @param pointTrack The unlocalized point track containing the observations, must be valid
		 * @param position The known 3D position of the object point, must be valid
		 * @param localizationPrecision The localization precision of the object point
		 * @param isBundleAdjusted True, if the position has been determined during a bundle adjustment; False, otherwise
		 */
		inline LocalizedObjectPoint(const PointTrack& pointTrack, const Vector3& position, const LocalizationPrecision localizationPrecision, const bool isBundleAdjusted);

		/**
		 * Adds a new observation of this object point for a given frame index.
		 * Complexity: O(1).
		 * @param frameIndex The index of the frame for which the observation will be added
		 * @param imagePoint The 2D image point of the observation
		 */
		inline void addObservation(const Index32 frameIndex, const Vector2& imagePoint);

		/**
		 * Adds all observations from a point track to this localized object point.
		 * @param pointTrack The point track containing the observations to add, must be valid
		 */
		void addObservations(const PointTrack& pointTrack);

		/**
		 * Removes the observation of this object point for a given frame index.
		 * Complexity: O(1).
		 * @param frameIndex The index of the frame for which the observation will be removed
		 */
		inline void removeObservation(const Index32 frameIndex);

		/**
		 * Returns whether this object point has an observation for a given frame index.
		 * Complexity: O(1).
		 * @param frameIndex The index of the frame for which the observation will be checked
		 * @param imagePoint Optional resulting 2D image point of the observation, nullptr if not of interest
		 * @return True, if so
		 */
		inline bool hasObservation(const Index32& frameIndex, Vector2* imagePoint = nullptr) const;

		/**
		 * Returns the observation of this object point for a given frame index.
		 * Only call this function if hasObservation() returns true.<br>
		 * Complexity: O(1).
		 * @param frameIndex The index of the frame for which the observation will be returned
		 * @return The object point's observation for the given frame index
		 */
		inline Vector2 observation(const Index32 frameIndex) const;

		/**
		 * Returns the last observation of this object point.
		 * @return The object point's last observation
		 */
		inline Observation lastObservation() const;

		/**
		 * Returns the frame index of the last observation of this object point.
		 * Complexity: O(1).
		 * @return The object point's last observation frame index, -1 if no observation exists
		 */
		inline Index32 lastObservationFrameIndex() const;

		/**
		 * Returns the number of observations of this object point.
		 * Complexity: O(1).
		 * @return The object point's number of observations, with range [1, infinity)
		 */
		inline size_t numberObservations() const;

		/**
		 * Returns the position of this object point.
		 * @return The object point's position, Vector3::minValue() if the position is (not yet) known
		 */
		inline const Vector3& position() const;

		/**
		 * Sets or updates the position of this object point.
		 * @param objectPoint The new position of this object point, must not be Vector3::minValue()
		 * @param isBundleAdjusted True, if the position has been determined during a bundle adjustment; False, otherwise
		 */
		inline void setPosition(const Vector3& objectPoint, const bool isBundleAdjusted);

		/**
		 * Returns whether the position of this object point has been determined during a bundle adjustment.
		 * @return True, if the position has been determined during a bundle adjustment; False, if the position has been determined by lower precision methods
		 */
		inline bool isBundleAdjusted() const;

		/**
		 * Returns the localization precision of this object point.
		 * @return The object point's localization precision
		 */
		inline LocalizationPrecision localizationPrecision() const;

		/**
		 * Sets or updates the localization precision of this object point.
		 * @param localizationPrecision The new localization precision of this object point
		 */
		inline void setLocalizationPrecision(const LocalizationPrecision localizationPrecision);

		/**
		 * Returns the visual descriptors of this object point.
		 * An object point may have several visual descriptors, e.g., one for individual observations.
		 * @return The object point's visual descriptors
		 */
		inline const CV::Detector::FREAKDescriptors32& descriptors() const;

		/**
		 * Adds a new visual descriptor to this object point.
		 * @param frameIndex The index of the frame from which the descriptor has been extracted, with range [0, infinity)
		 * @param descriptor The new visual descriptor to be added, must be valid
		 */
		inline void addDescriptors(const Index32 frameIndex, const CV::Detector::FREAKDescriptor32& descriptor);

		/**
		 * Returns whether this object point needs a visual descriptor.
		 * @param frameIndex The index of the current frame, with range [0, infinity)
		 * @return True, if so
		 */
		inline bool needDescriptor(const Index32 frameIndex) const;

		/**
		 * Optimizes this localized 3D object point which is visible in the current camera frame and in several previous camera frames.
		 * This function should be called for object points which are not precise enough anymore (e.g., because they are based on a low number of observations).
		 * @param mapVersion The version of the map to use for the optimization
		 * @param camera The camera profile defining the projection, must be valid
		 * @param cameraPoses The camera poses which have been determined so far, must be valid
		 * @param currentFrameIndex The index of the current frame in which the object point is visible, with range [0, infinity)
		 * @param minimalNumberObservations The minimal number of observations which are necessary to optimize the object point, with range [2, infinity)
		 * @param maximalProjectionError The maximal projection error in pixels, with range [0, infinity)
		 * @param estimatorType The estimator type to be used, must be valid
		 * @return The optimization result, OR_SUCCEEDED if the position was updated
		 */
		inline OptimizationResult optimizeObjectPoint(const Index32 mapVersion, const AnyCamera& camera, const CameraPoses& cameraPoses, const Index32 currentFrameIndex, const size_t minimalNumberObservations, const Scalar maximalProjectionError, const Geometry::Estimator::EstimatorType estimatorType);

		/**
		 * Determines the optimized 3D position of this localized object point visible in the current camera frame and in several previous camera frames.
		 * This function computes an optimized position without modifying the internal state of this object.
		 * @param mapVersion The version of the map to use for the optimization
		 * @param camera The camera profile defining the projection, must be valid
		 * @param cameraPoses The camera poses which have been determined so far, must be valid
		 * @param currentFrameIndex The index of the current frame in which the object point is visible, with range [0, infinity)
		 * @param minimalNumberObservations The minimal number of observations which are necessary to optimize the object point, with range [2, infinity)
		 * @param maximalProjectionError The maximal projection error in pixels, with range [0, infinity)
		 * @param estimatorType The estimator type to be used, must be valid
		 * @param optimizedPosition The resulting optimized 3D position of this object point
		 * @return The optimization result, OR_SUCCEEDED if the position was successfully optimized
		 */
		OptimizationResult optimizedObjectPoint(const Index32 mapVersion, const AnyCamera& camera, const CameraPoses& cameraPoses, const Index32 currentFrameIndex, const size_t minimalNumberObservations, const Scalar maximalProjectionError, const Geometry::Estimator::EstimatorType estimatorType, Vector3& optimizedPosition) const;

		/**
		 * Updates the localization precision of this object point based on its observations and the camera poses.
		 * The precision is determined from the covariance matrix of the object point's 3D position estimate.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param cameraPose The camera poses which have been determined so far, must not be empty
		 * @return True, if the precision has changed; False, if the precision has not changed
		 */
		bool updateLocalizedObjectPointUncertainty(const AnyCamera& camera, const CameraPoses& cameraPose);

		/**
		 * Determines the median viewing angle of this object point.
		 * @param cameraPoses The camera poses which have been determined so far, must be valid
		 * @return The median viewing angle in radian, 0 if the angle could not be determined
		 */
		Scalar determineMedianViewingAngle(const CameraPoses& cameraPoses) const;

		/**
		 * Determines the quality of a camera pose for a specific frame by computing projection errors.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param cameraPoses The camera poses which have been determined so far, must be valid
		 * @param frameIndex The index of the frame for which the camera pose quality will be determined, with range [0, infinity)
		 * @param localizedObjectPointMap The map of localized object points
		 * @param minError The resulting minimum projection error in pixels
		 * @param averageError The resulting average projection error in pixels
		 * @param maxError The resulting maximum projection error in pixels
		 * @return The number of observations used to determine the quality, 0 if the pose could not be evaluated
		 */
		static size_t determineCameraPoseQuality(const AnyCamera& camera, const CameraPoses& cameraPoses, const Index32 frameIndex, const LocalizedObjectPointMap& localizedObjectPointMap, Scalar& minError, Scalar& averageError, Scalar& maxError);

		/**
		 * Determines the quality of a camera pose for a specific frame by computing projection errors using an inverted flipped camera transformation.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param flippedCamera_T_world The inverted flipped camera transformation, must be valid
		 * @param frameIndex The index of the frame for which the camera pose quality will be determined, with range [0, infinity)
		 * @param localizedObjectPointMap The map of localized object points
		 * @param minError The resulting minimum projection error in pixels
		 * @param averageError The resulting average projection error in pixels
		 * @param maxError The resulting maximum projection error in pixels
		 * @return The number of observations used to determine the quality, 0 if the pose could not be evaluated
		 */
		static size_t determineCameraPoseQualityIF(const AnyCamera& camera, const HomogenousMatrix4& flippedCamera_T_world, const Index32 frameIndex, const LocalizedObjectPointMap& localizedObjectPointMap, Scalar& minError, Scalar& averageError, Scalar& maxError);

		/**
		 * Determines the quality of object points for a specific frame by computing projection errors using an inverted flipped camera transformation.
		 * Object points are classified as valid or invalid based on whether their projection error is below the maximal threshold.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param flippedCamera_T_world The inverted flipped camera transformation, must be valid
		 * @param frameIndex The index of the frame for which the object point quality will be determined, with range [0, infinity)
		 * @param localizedObjectPointMap The map of localized object points
		 * @param maximalProjectionError The maximal projection error in pixels, with range [0, infinity)
		 * @param validObjectPointIds The resulting ids of object points with projection errors below the threshold
		 * @param invalidObjectPointIds The resulting ids of object points with projection errors above the threshold
		 */
		static void determineObjectPointQualityIF(const AnyCamera& camera, const HomogenousMatrix4& flippedCamera_T_world, const Index32 frameIndex, const LocalizedObjectPointMap& localizedObjectPointMap, const Scalar maximalProjectionError, Indices32& validObjectPointIds, Indices32& invalidObjectPointIds);

		/**
		 * Determines the quality of bundle adjustment for a specific frame.
		 * @param frameIndex The index of the frame for which the bundle adjustment quality will be determined, with range [0, infinity)
		 * @param localizedObjectPointMap The map of localized object points
		 * @param bundleAdjustedObjectPoints The resulting number of object points that have been bundle adjusted
		 * @return The total number of object points visible in the frame
		 */
		static size_t determineBundleAdjustmentQuality(const Index32 frameIndex, const LocalizedObjectPointMap& localizedObjectPointMap, size_t& bundleAdjustedObjectPoints);

		/**
		 * Determines the median distance from the camera to the localized object points.
		 * @param cameraPoses The camera poses which have been determined so far, must be valid
		 * @param frameIndex The index of the frame for which the median distance will be determined, with range [0, infinity)
		 * @param localizedObjectPointMap The map of localized object points
		 * @param onlyTrackedObjectPoints True, to only consider object points that have an observation in the current frame; False, to consider all object points in front of the camera
		 * @return The median distance in world units, -1 if the median could not be determined
		 */
		static Scalar determineMedianLocalizedObjectDistances(const CameraPoses& cameraPoses, const Index32 frameIndex, const LocalizedObjectPointMap& localizedObjectPointMap, const bool onlyTrackedObjectPoints);

		/**
		 * Determines the number of tracked object points for a specific frame.
		 * @param cameraPoses The camera poses which have been determined so far, must be valid
		 * @param frameIndex The index of the frame for which the number of tracked object points will be determined, with range [0, infinity)
		 * @param localizedObjectPointMap The map of localized object points
		 * @return The number of tracked object points
		 */
		static size_t determineNumberTrackedObjectPoints(const CameraPoses& cameraPoses, const Index32 frameIndex, const LocalizedObjectPointMap& localizedObjectPointMap);

		/**
		 * Determines the number of tracked object points for a specific frame from a given set of object point ids.
		 * @param cameraPoses The camera poses which have been determined so far, must be valid
		 * @param frameIndex The index of the frame for which the number of tracked object points will be determined, with range [0, infinity)
		 * @param localizedObjectPointMap The map of localized object points
		 * @param objectPointIds The set of object point ids to check
		 * @return The number of tracked object points
		 */
		static size_t determineNumberTrackedObjectPoints(const CameraPoses& cameraPoses, const Index32 frameIndex, const LocalizedObjectPointMap& localizedObjectPointMap, const UnorderedIndexSet32& objectPointIds);

		/**
		 * Determines the localization precision of an object point based on camera poses using an inverted flipped camera transformation.
		 * The precision is estimated from the covariance matrix of the triangulated 3D point.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param flippedCameras_T_world The inverted flipped camera transformations for each observation, with at least 2 elements
		 * @param objectPoint The 3D position of the object point
		 * @return The determined localization precision
		 */
		static LocalizationPrecision determineLocalizedObjectPointUncertaintyIF(const AnyCamera& camera, const HomogenousMatrices4& flippedCameras_T_world, const Vector3& objectPoint);

		/**
		 * Determines the localization precision of an object point based on its covariance matrix.
		 * The precision is determined by analyzing the eigenvalues of the covariance matrix.
		 * @param covarianceMatrix The covariance matrix of the object point's 3D position, must not be singular
		 * @return The determined localization precision
		 */
		static LocalizationPrecision determineLocalizedObjectPointUncertainty(const SquareMatrix3& covarianceMatrix);

		/**
		 * Determines the camera pose for a specific frame using the localized object points.
		 * This function uses pose estimation with an optional prior pose from the previous frame.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param cameraPoses The camera poses which have been determined so far, must be valid
		 * @param frameIndex The index of the frame for which the camera pose will be determined, with range [0, infinity)
		 * @param localizedObjectPointMap The map of localized object points, with at least 4 elements
		 * @param randomGenerator The random generator to use for RANSAC
		 * @param estimatorType The estimator type to be used, must be valid
		 * @param correspondenceData The reusable correspondence data container, will be modified
		 * @param gravityConstraints Optional gravity constraints to apply during pose optimization, nullptr if not used
		 * @return The determined camera pose, nullptr if the pose could not be determined
		 */
		static SharedCameraPose determineCameraPose(const AnyCamera& camera, const CameraPoses& cameraPoses, const Index32 frameIndex, const LocalizedObjectPointMap& localizedObjectPointMap, RandomGenerator& randomGenerator, const Geometry::Estimator::EstimatorType estimatorType, CorrespondenceData& correspondenceData, const Geometry::GravityConstraints* gravityConstraints);

		/**
		 * Determines the camera pose for a specific frame using RANSAC-based P3P pose estimation.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param frameIndex The index of the frame for which the camera pose will be determined, with range [0, infinity)
		 * @param localizedObjectPointMap The map of localized object points
		 * @param world_T_camera The resulting camera-to-world transformation
		 * @param randomGenerator The random generator to use for RANSAC
		 * @param minimalCorrespondences The minimal number of correspondences required, with range [4, infinity)
		 * @param maximalProjectionError The maximal projection error in pixels for inlier classification, with range [0, infinity)
		 * @param usedObjectPointIds The resulting ids of object points used as inliers
		 * @param sqrError The resulting squared average projection error
		 * @param gravityConstraints Optional gravity constraints to apply during pose optimization, nullptr if not used
		 * @return True, if a valid pose was determined; False, otherwise
		 */
		static bool determineCameraPose(const AnyCamera& camera, const Index32 frameIndex, const LocalizedObjectPointMap& localizedObjectPointMap, HomogenousMatrix4& world_T_camera, RandomGenerator& randomGenerator, const size_t minimalCorrespondences, const Scalar maximalProjectionError, Indices32& usedObjectPointIds, Scalar& sqrError, const Geometry::GravityConstraints* gravityConstraints);

		/**
		 * Translates a localization precision enum value to a human-readable string.
		 * @param localizationPrecision The localization precision to translate
		 * @return The human-readable string representation of the localization precision
		 */
		static std::string translateLocalizationPrecision(const LocalizationPrecision localizationPrecision);

		/**
		 * Serializes a map of localized object points to a bitstream.
		 * @param localizedObjectPointMap The map of localized object points to serialize
		 * @param outputBitstream The output bitstream to write to
		 * @return True, if the serialization succeeded; False, otherwise
		 */
		static bool serialize(const LocalizedObjectPointMap& localizedObjectPointMap, IO::OutputBitstream& outputBitstream);

		/**
		 * Serializes a single localized object point to a bitstream.
		 * @param localizedObjectPoint The localized object point to serialize
		 * @param outputBitstream The output bitstream to write to
		 * @return True, if the serialization succeeded; False, otherwise
		 */
		static bool serialize(const LocalizedObjectPoint& localizedObjectPoint, IO::OutputBitstream& outputBitstream);

	protected:

		/// The 3D position of the object point.
		Vector3 position_ = Vector3::minValue();

		/// True, if the position of the object point has been determined during a Bundle Adjustment; False, if the position has been determine by lower precision methods
		bool isBundleAdjusted_ = false;

		/// The map mapping camera indices to 2D observations.
		ObservationMap observationMap_;

		/// The index of the last observation, -1 if no observation exists.
		Index32 lastObservationFrameIndex_ = Index32(-1);

		/// The localization precision of the object point.
		LocalizationPrecision localizationPrecision_ = LP_INVALID;

		/// The index of the frame in which the last descriptor was added, -1 if no descriptor has been added yet.
		Index32 lastDescriptorFrameIndex_ = Index32(-1);

		/// The visual descriptors of the object point.
		CV::Detector::FREAKDescriptors32 descriptors_;

		/// TODO, we need to separate visibile from invisible localized object points to optimize performance
};

inline LocalizedObjectPoint::LocalizedObjectPoint(const PointTrack& pointTrack, const Vector3& position, const LocalizationPrecision localizationPrecision, const bool isBundleAdjusted) :
	LocalizedObjectPoint(pointTrack)
{
	position_ = position;
	isBundleAdjusted_ = isBundleAdjusted;
	localizationPrecision_ = localizationPrecision;
}

inline void LocalizedObjectPoint::addObservation(const Index32 frameIndex, const Vector2& imagePoint)
{
	ocean_assert(observationMap_.find(frameIndex) == observationMap_.cend());
	observationMap_.emplace(frameIndex, imagePoint);

	ocean_assert(lastObservationFrameIndex_ != Index32(-1));
	ocean_assert(frameIndex > lastObservationFrameIndex_);
	lastObservationFrameIndex_ = frameIndex;
}

inline void LocalizedObjectPoint::removeObservation(const Index32 frameIndex)
{
	ocean_assert(observationMap_.find(frameIndex) != observationMap_.cend());
	observationMap_.erase(frameIndex);

	if (frameIndex != lastObservationFrameIndex_)
	{
		return;
	}

	// we removed the last observation, so we have to find the new latest frame index

	lastObservationFrameIndex_ = Index32(-1);

	if (observationMap_.empty())
	{
		return;
	}

	for (const ObservationMap::value_type& observationPair : observationMap_)
	{
		const Index32 candidateFrameIndex = observationPair.first;

		if (lastObservationFrameIndex_ == Index32(-1) || candidateFrameIndex > lastObservationFrameIndex_)
		{
			lastObservationFrameIndex_ = candidateFrameIndex;
		}
	}
}

inline bool LocalizedObjectPoint::hasObservation(const Index32& frameIndex, Vector2* imagePoint) const
{
	const ObservationMap::const_iterator iObservation = observationMap_.find(frameIndex);

	if (iObservation == observationMap_.cend())
	{
		return false;
	}

	if (imagePoint != nullptr)
	{
		*imagePoint = iObservation->second;
	}

	return true;
}

inline Vector2 LocalizedObjectPoint::observation(const Index32 frameIndex) const
{
	const ObservationMap::const_iterator iObservation = observationMap_.find(frameIndex);

	ocean_assert(iObservation != observationMap_.cend());

	return iObservation->second;
}

inline Observation LocalizedObjectPoint::lastObservation() const
{
	ocean_assert(lastObservationFrameIndex_ != Index32(-1));

	const ObservationMap::const_iterator iObservation = observationMap_.find(lastObservationFrameIndex_);
	ocean_assert(iObservation != observationMap_.cend());

	return Observation(iObservation->first, iObservation->second);
}

inline Index32 LocalizedObjectPoint::lastObservationFrameIndex() const
{
	return lastObservationFrameIndex_;
}

inline size_t LocalizedObjectPoint::numberObservations() const
{
	return observationMap_.size();
}

inline const Vector3& LocalizedObjectPoint::position() const
{
	return position_;
}

inline void LocalizedObjectPoint::setPosition(const Vector3& objectPoint, const bool isBundleAdjusted)
{
	ocean_assert(objectPoint != Vector3::minValue());

	position_ = objectPoint;
	isBundleAdjusted_ = isBundleAdjusted;
}

inline LocalizedObjectPoint::LocalizationPrecision LocalizedObjectPoint::localizationPrecision() const
{
	return localizationPrecision_;
}

inline void LocalizedObjectPoint::setLocalizationPrecision(const LocalizationPrecision localizationPrecision)
{
	localizationPrecision_ = localizationPrecision;
}

inline const CV::Detector::FREAKDescriptors32& LocalizedObjectPoint::descriptors() const
{
	return descriptors_;
}

inline void LocalizedObjectPoint::addDescriptors(const Index32 frameIndex, const CV::Detector::FREAKDescriptor32& descriptor)
{
	ocean_assert(lastDescriptorFrameIndex_ == Index32(-1) || lastDescriptorFrameIndex_ < frameIndex);
	ocean_assert(descriptor.isValid());

	lastDescriptorFrameIndex_ = frameIndex;

	descriptors_.push_back(descriptor);
}

inline bool LocalizedObjectPoint::needDescriptor(const Index32 frameIndex) const
{
	if (localizationPrecision_ < LP_MEDIUM)
	{
		return false;
	}

	if (descriptors_.empty())
	{
		return true;
	}

	if (descriptors_.size() >= 10)
	{
		return false;
	}

	ocean_assert(lastDescriptorFrameIndex_ != Index32(-1));

	// intervals between descriptors = basicInterval, 2 * basicInterval, 4 * basicInterval, ...

	const unsigned int basicInterval = 30u; // 30 frames

	const unsigned int intervalFactor = (unsigned int)(1u << (unsigned int)(descriptors_.size() - 1u));

	const unsigned int nextFrameIndex = lastDescriptorFrameIndex_ + basicInterval * intervalFactor;

	return frameIndex >= nextFrameIndex;
}

inline LocalizedObjectPoint::OptimizationResult LocalizedObjectPoint::optimizeObjectPoint(const Index32 mapVersion, const AnyCamera& camera, const CameraPoses& cameraPoses, const Index32 currentFrameIndex, const size_t minimalNumberObservations, const Scalar maximalProjectionError, const Geometry::Estimator::EstimatorType estimatorType)
{
	Vector3 optimizedPosition;
	const OptimizationResult optimizationResult = optimizedObjectPoint(mapVersion, camera, cameraPoses, currentFrameIndex, minimalNumberObservations, maximalProjectionError, estimatorType, optimizedPosition);

	if (optimizationResult == OR_SUCCEEDED)
	{
		position_ = optimizedPosition;
	}

	return optimizationResult;
}

inline bool LocalizedObjectPoint::isBundleAdjusted() const
{
	return isBundleAdjusted_;
}

inline bool LocalizedObjectPoint::CorrespondenceData::isSubsetApplied() const
{
	return usedIndices_.empty();
}

inline bool LocalizedObjectPoint::CorrespondenceData::isEmpty() const
{
	ocean_assert(!objectPoints_.empty() || (imagePoints_.empty() && objectPointIds_.empty() && localizationPrecisions_.empty() && imagePointSqrDistances_.empty() && usedIndices_.empty() && badObjectPointIds_.empty()));

	return objectPoints_.empty();
}

inline void LocalizedObjectPoint::ObjectPointIdSet::add(const Indices32& objectPointIds)
{
	const ScopedLock scopedLock(lock_);

	objectPointIdSet_.insert(objectPointIds.begin(), objectPointIds.end());
}

inline UnorderedIndexSet32 LocalizedObjectPoint::ObjectPointIdSet::objectPointIds()
{
	const ScopedLock scopedLock(lock_);

	UnorderedIndexSet32 result(std::move(objectPointIdSet_));
	objectPointIdSet_.clear();

	return result;
}

inline void LocalizedObjectPoint::ObjectPointIdSet::clear()
{
	const ScopedLock scopedLock(lock_);

	objectPointIdSet_.clear();
}

}

}

}

#endif // META_OCEAN_TRACKING_SLAM_LOCALIZED_OBJECT_POINT_H
