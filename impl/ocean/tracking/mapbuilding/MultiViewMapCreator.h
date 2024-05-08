/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_MAPBUILDING_MULTI_VIEW_MAP_CREATOR_H
#define META_OCEAN_TRACKING_MAPBUILDING_MULTI_VIEW_MAP_CREATOR_H

#include "ocean/tracking/mapbuilding/MapBuilding.h"
#include "ocean/tracking/mapbuilding/DescriptorHandling.h"
#include "ocean/tracking/mapbuilding/Unified.h"

#include "ocean/base/RandomGenerator.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/detector/FREAKDescriptor.h"

#include "ocean/geometry/SpatialDistribution.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/HomogenousMatrix4.h"

namespace Ocean
{

namespace Tracking
{

namespace MapBuilding
{

/**
 * This class implements a creator for real-time feature maps based on multiple views (e.g., an HMD).
 * The lower two stereo cameras will be used to detect new features while the remaining cameras will be used to add observations of existing features.
 * @ingroup trackingmapbuilding
 */
class OCEAN_TRACKING_MAPBUILDING_EXPORT MultiViewMapCreator : public DescriptorHandling
{
	public:

		/**
		 * Definition of a pair combining a frame index with a camera index.
		 */
		class ObservationPair
		{
			public:

				/**
				 * Creates a new observation pair.
				 * @param frameIndex The index of the (multi-view) frame to which the observation belongs, with range [0, infinity)
				 * @param cameraIndex The index of the camera (within the multi-views) to which the observation belongs, with range [0, infinity)
				 */
				inline ObservationPair(const Index32 frameIndex, const Index32 cameraIndex);

			public:

				/// The index of the (multi-view) frame to which the observation belongs.
				Index32 frameIndex_ = Index32(-1);

				/// The index of the camera (within the multi-views) to which the observation belongs.
				Index32 cameraIndex_ = Index32(-1);
		};

	protected:

		/**
		 * This class holds the relevant information for one 3D feature point.
		 */
		class OCEAN_TRACKING_MAPBUILDING_EXPORT Feature
		{
			public:

				/**
				 * Definition of individual localization results.
				 */
				enum LocalizationResult : uint32_t
				{
					/// The localization failed.
					LR_FAILED = 0u,
					/// The localization has been skipped.
					LR_SKIPPED,
					/// The localization succeeded.
					LR_SUCCEEDED
				};

				/**
				 * This class holds the relevant information for one observation of a 3D feature.
				 */
				class Observation
				{
					public:

						/**
						 * Creates a new observation object.
						 * @param imagePoint The 2D image point of the observation within the camera image
						 * @param observationPair
						 * @param descriptor The descriptor of the observation
						 */
						inline Observation(const Vector2& imagePoint, const ObservationPair& observationPair, const CV::Detector::FREAKDescriptor32& descriptor);

					public:

						/// The 2D location of the observation within one camera image.
						Vector2 imagePoint_;

						/// The observation pair defining to which cameras/images the observation belongs.
						ObservationPair observationPair_;

						/// The descriptor of the observation.
						CV::Detector::FREAKDescriptor32 descriptor_;
				};

				/**
				 * Definition of a vector holding observations.
				 */
				typedef std::vector<Observation> Observations;

			public:

				/**
				 * Creates a new feature object which has been determined in a mono view.
				 * @param imagePoint The 2D observation of the 3D feature point in the camera
				 * @param observationPair The observation pair defining in which camera image the feature point was observed
				 * @param descriptor The descriptor of the feature point
				 */
				Feature(const Vector2& imagePoint, const ObservationPair& observationPair, const CV::Detector::FREAKDescriptor32& descriptor);

				/**
				 * Creates a new feature object which has been determined in a stereo view.
				 * @param imagePointA The 2D observation of the 3D feature point in the first camera
				 * @param imagePointB The 2D observation of the 3D feature point in the second camera
				 * @param observationPairA The observation pair defining in which camera image the feature point was observed in the first camera
				 * @param observationPairB The observation pair defining in which camera image the feature point was observed in the second camera
				 * @param descriptorA The descriptor of the feature point in the first image
				 * @param descriptorB The descriptor of the feature point in the second image
				 * @param world_T_cameraA The transformation between first camera and world, must be valid
				 * @param world_T_cameraB The transformation between second camera and world, must be valid
				 * @param objectPoint The 3D location of the feature point
				 */
				Feature(const Vector2& imagePointA, const Vector2& imagePointB, const ObservationPair& observationPairA, const ObservationPair& observationPairB, const CV::Detector::FREAKDescriptor32& descriptorA, const CV::Detector::FREAKDescriptor32& descriptorB, const HomogenousMatrix4& world_T_cameraA, const HomogenousMatrix4& world_T_cameraB, const Vector3& objectPoint);

				/**
				 * Returns all observations of this feature point.
				 * @return The feature's observations
				 */
				inline const Observations& observations() const;

				/**
				 * Returns the 3D object point of this feature point, if known already
				 * @return The feature's 3D object point location
				 * @see isLocalized().
				 */
				inline const Vector3& objectPoint() const;

				/**
				 * Returns whether this feature point is localized in 3D space.
				 * In case the feature is localized, a valid 3D location is known based on the feature's observations.
				 * @return True, if so
				 */
				inline bool isLocalized() const;

				/**
				 * Adds a new observation for the feature.
				 * @param imagePoint The 2D image point within the image at which the feature was observed
				 * @param observationPair The observation pair defining in which image the image point is defined
				 * @param descriptor The descriptor of the 2D observation
				 * @param randomGenerator Random generator object
				 */
				void addObservation(const Vector2& imagePoint, const ObservationPair& observationPair, const CV::Detector::FREAKDescriptor32& descriptor, RandomGenerator& randomGenerator);

				/**
				 * (Re-)localizes this feature.
				 * @param world_T_cameraGroups The groups of transformations between cameras and world, one group for each multi-view camera
				 * @param cameraGroups The groups of cameras defining the projection, one group for each group of transformations
				 * @return The localization result
				 */
				LocalizationResult localizeObjectPoint(const std::vector<HomogenousMatrices4>& world_T_cameraGroups, const std::vector<SharedAnyCameras>& cameraGroups);

				/**
				 * Informs the feature that is has not been observed.
				 * @param sqrDistance The square distance between camera and feature point, with range (0, infinity)
				 * @param secondsPerFrame The seconds since the last frame, with range (0, infinity)
				 * @return True, if the feature could have been observed based on the distance between camera and feature point; False, if the feature was outside visibility range
				 */
				bool failedObservation(const Scalar sqrDistance, const double secondsPerFrame);

				/**
				 * Copies the observations from a second feature e.g., to join two features.
				 * @param feature The second feature from which the observation will be copied
				 */
				void copyObservations(const Feature& feature);

				/**
				 * Returns the stability factor of this feature.
				 * @return The feature's stability factor; 0 means not stable, 1 means stable, with range [0, 1]
				 */
				inline Scalar stabilityFactor() const;

				/**
				 * Returns whether this feature is not stable anymore and should be removed.
				 * @return True, if so
				 */
				inline bool isInstable() const;

			protected:

				/// The overall number of observations for this feature.
				size_t observationIterations_ = 0;

				/// The observations of this feature.
				Observations observations_;

				/// The 3D location of this feature, defined in world.
				Vector3 objectPoint_ = Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue());

				/// The minimal distance between 3D object point and camera at which the feature has been observed, adjusted with a generous threshold.
				Scalar minimalObservationSqrDistance_ = Numeric::maxValue();

				/// The maximal distance between 3D object point and camera at which the feature has bee observed, adjusted with a generous threshold.
				Scalar maximalObservationSqrDistance_ = Numeric::minValue();

				/// The minimal number of observation iterations necessary when the next re-localization will be invoked.
				size_t nextLocalizationObservationIterations_ = 3;

				/// The squared diagonal size of the bounding box in which all camera poses are located.
				Scalar sqrBaseline_ = 0;

				/// The time since the feature has been observed the last time.
				double timeSinceLastObservation_ = 0.0;

				/// The maximal number of observations that will be kept.
				static constexpr size_t maxObservations_ = 100;

				/// The adjustment threshold for the observation distance, with range (0, 1), e.g., 0.25 = 75% and 125%
				static constexpr Scalar observationThreshold_ = Scalar(0.25);

				/// The maximal time a feature exists without any re-observation, in seconds.
				static constexpr double maxTimeWithoutObservation_ = 5.0;
		};

		/**
		 * Definition of a vector holding features.
		 */
		typedef std::vector<Feature> Features;

	public:

		/**
		 * Processes multi-frames to extend the current feature map.
		 * @param yFrames The camera images to be used, with pixel format FORMAT_Y8, at least two
		 * @param cameras The camera profiles of the given camera images, one for each camera image
		 * @param world_T_device The transformation between the device and the world, must be valid
		 * @param device_T_cameras The transformations between cameras and the device, with each camera pointing towards the negative z-space with y-axis pointing upwards, one for each camera image
		 * @return True, if succeeded
		 */
		bool processFrame(const Frames& yFrames, const SharedAnyCameras& cameras, const HomogenousMatrix4& world_T_device, const HomogenousMatrices4& device_T_cameras);

		/**
		 * Extracts the latest feature map from this creator.
		 * @param objectPoints The resulting 3D object points of all features
		 * @param multiDescriptors Optional resulting multi-descriptors for the 3D feature points, each feature point can be associated with several descriptors, one for each 3D object point
		 * @param stabilityFactors Optional resulting stability factors for feature points; 0 means instable, 1 means stable; one for each 3D object point
		 * @param minimalObservations The minimal number of observations each resulting feature point must have, with range [2, infinity)
		 * @param maximalDescriptorsPerFeaturePoint The maximal number of feature descriptors that will be returned per feature point, with range [1, infinity)
		 * @return True, if succeeded
		 */
		bool latestFeatureMap(Vectors3& objectPoints, std::vector<CV::Detector::FREAKDescriptors32>* multiDescriptors = nullptr, Scalars* stabilityFactors = nullptr, const size_t minimalObservations = 10, const size_t maximalDescriptorsPerFeaturePoint = 10) const;

		/**
		 * Determines the indices of the two lower (hopefully overlapping) stereo cameras.
		 * @param device_T_cameras The transformations between cameras and device, with each camera pointing towards the negative z-space with y-axis pointing upwards
		 * @param stereoCameraIndices The resulting indices of the stereo cameras
		 * @return True, if succeeded
		 */
		static bool determineLowerStereoCameras(const HomogenousMatrices4& device_T_cameras, IndexPair32& stereoCameraIndices);

	protected:

		/**
		 * Determines observations for existing localized 3D feature points.
		 * Further, features which have not been observed for a while will be removed.
		 * @param currentFrameIndex The index of the current frame, with range [0, infinity)
		 * @param currentTimestamp The timestamp of the current multi frames, must be valid
		 * @param world_T_device The transformation between the device and the world, must be valid
		 * @param world_T_currentCameras The transformations between cameras and world, one for each multi frame
		 * @param currentCameras The camera profiles defining the projection, one for each multi frame
		 * @param distributionArrays The distribution arrays for all image points, one for each multi frame
		 */
		void determineObservations(const Index32 currentFrameIndex, const Timestamp& currentTimestamp, const HomogenousMatrix4& world_T_device, const HomogenousMatrices4& world_T_currentCameras, const SharedAnyCameras& currentCameras, std::vector<Geometry::SpatialDistribution::DistributionArray>& distributionArrays);

		/**
		 * Determines features in all frames.
		 * @param yFrames The camera frames in which the features will be determined, with pixel format FORMAT_Y8, at least one
		 * @param cameras The camera profiles defining the projection, one for each camera frame
		 * @param imagePointGroups The resulting groups of image points (detected features), one group for each camera frame
		 * @param descriptorGroups The resulting groups of feature descriptors, one group for each camera frame, on descriptor for each image point
		 * @param cornerPyramidLevelGroups The resulting groups of pyramid levels in which the individual features were detected, on group for each camera frame
		 * @param distributionArrays The resulting distribution arrays for all image points, one for each camera frame
		 * @param worker Optional worker to distribute the computation
		 */
		static void determineImageFeatures(const Frames& yFrames, const SharedAnyCameras& cameras, std::vector<Vectors2>& imagePointGroups, std::vector<CV::Detector::FREAKDescriptors32>& descriptorGroups, std::vector<Indices32>& cornerPyramidLevelGroups, std::vector<Geometry::SpatialDistribution::DistributionArray>& distributionArrays, Worker* worker);

		/**
		 * Determines features in a subset of all frames.
		 * @param yFrames The camera frames in which the features will be determined, with pixel format FORMAT_Y8, at least one
		 * @param cameras The camera profiles defining the projection, one for each camera frame
		 * @param imagePointGroups The resulting groups of image points (detected features), one group for each camera frame
		 * @param descriptorGroups The resulting groups of feature descriptors, one group for each camera frame, on descriptor for each image point
		 * @param cornerPyramidLevelGroups The resulting groups of pyramid levels in which the individual features were detected, on group for each camera frame
		 * @param distributionArrays The resulting distribution arrays for all image points, one for each camera frame
		 * @param firstGroup The first group to be handled, with range [0, yFrames.size() - 1]
		 * @param numberGroups The first group to be handled, with range [1, yFrames.size() - firstGroup]
		 */
		static void determineImageFeaturesSubset(const Frame* yFrames, const SharedAnyCamera* cameras, Vectors2* imagePointGroups, CV::Detector::FREAKDescriptors32* descriptorGroups, Indices32* cornerPyramidLevelGroups, Geometry::SpatialDistribution::DistributionArray* distributionArrays, const unsigned int firstGroup, const unsigned int numberGroups);

		/**
		 * Determines feature matches between two stereo images.
		 * @param cameraA The camera profile of the first camera, must be valid
		 * @param cameraB The camera profile of the second camera, must be valid
		 * @param world_T_cameraA The transformations between first camera and world, with camera pointing towards negative z-space and y-axis upwards, must be valid
		 * @param world_T_cameraB The transformations between second camera and world, with camera pointing towards negative z-space and y-axis upwards, must be valid
		 * @param imagePointsA The image points of all features in the first camera frame
		 * @param imagePointsB The image points of all features in the second camera frame
		 * @param descriptorsA The descriptors for all image points in the first camera frame, one for each image point
		 * @param descriptorsB The descriptors for all image points in the second camera frame, one for each image point
		 * @param pyramidLevelsA The pyramid levels in which the first image points have been detected, one for each image point
		 * @param pyramidLevelsB The pyramid levels in which the second image points have been detected, one for each image point
		 * @param objectPoints The resulting 3D object points of all matched stereo features
		 * @return The index pairs of all matched features
		 */
		static IndexPairs32 matchStereoFeatures(const AnyCamera& cameraA, const AnyCamera& cameraB, const HomogenousMatrix4& world_T_cameraA, const HomogenousMatrix4& world_T_cameraB, const Vectors2& imagePointsA, const Vectors2& imagePointsB, const CV::Detector::FREAKDescriptors32& descriptorsA, const CV::Detector::FREAKDescriptors32 descriptorsB, const Indices32& pyramidLevelsA, const Indices32& pyramidLevelsB, Vectors3& objectPoints);

		/**
		 * Determines feature matches between two mono images (using the same camera at different moments in time).
		 * Features matched in mono frames cannot be localized immediately due to their smaller baseline.
		 * @param cameraA The camera profile of the first camera, must be valid
		 * @param cameraB The camera profile of the second camera, must be valid
		 * @param world_T_cameraA The transformations between first camera and world, with camera pointing towards negative z-space and y-axis upwards, must be valid
		 * @param world_T_cameraB The transformations between second camera and world, with camera pointing towards negative z-space and y-axis upwards, must be valid
		 * @param imagePointsA The image points of all features in the first camera frame
		 * @param imagePointsB The image points of all features in the second camera frame
		 * @param descriptorsA The descriptors for all image points in the first camera frame, one for each image point
		 * @param descriptorsB The descriptors for all image points in the second camera frame, one for each image point
		 * @param pyramidLevelsA The pyramid levels in which the first image points have been detected, one for each image point
		 * @param pyramidLevelsB The pyramid levels in which the second image points have been detected, one for each image point
		 * @return The index pairs of all matched features
		 */
		static IndexPairs32 matchMonoFeatures(const AnyCamera& cameraA, const AnyCamera& cameraB, const HomogenousMatrix4& world_T_cameraA, const HomogenousMatrix4& world_T_cameraB, const Vectors2& imagePointsA, const Vectors2& imagePointsB, const CV::Detector::FREAKDescriptors32& descriptorsA, const CV::Detector::FREAKDescriptors32 descriptorsB, const Indices32& pyramidLevelsA, const Indices32& pyramidLevelsB);

	protected:

		/// The groups of transformations between cameras and world, one group for each multi-frame, one transformation for each frame index.
		std::vector<HomogenousMatrices4> world_T_cameraGroups_;

		/// The groups of camera profiles, one group for each multi-frame, one profile for reach frame index.
		std::vector<SharedAnyCameras> cameraGroups_;

		/// The groups of image points detected in the current multi-frames.
		std::vector<Vectors2> currentImagePointGroups_;

		/// The groups of descriptors for the detected image points in the current multi-frames.
		std::vector<CV::Detector::FREAKDescriptors32> currentDescriptorGroups_;

		/// The groups of pyramid levels in which the image points have been detected in the current multi-frames.
		std::vector<Indices32> currentCornerPyramidLevelGroups_;

		/// The timestamp of the last frame.
		Timestamp lastTimestamp_ = Timestamp(false);

		/// The features which have been detected so far.
		Features features_;

		/// The creator's random generator object.
		RandomGenerator randomGenerator_;

		/// The creator's lock.
		mutable Lock lock_;

		/// The maximal expected projection distance between two corresponding feature points per pixel.
		static constexpr Scalar maxPixelDistanceFactor_ = Scalar(0.0625); // 40px for 640px
};

inline MultiViewMapCreator::ObservationPair::ObservationPair(const Index32 frameIndex, const Index32 cameraIndex) :
	frameIndex_(frameIndex),
	cameraIndex_(cameraIndex)
{
	// nothing to do here
}

inline MultiViewMapCreator::Feature::Observation::Observation(const Vector2& imagePoint, const ObservationPair& observationPair, const CV::Detector::FREAKDescriptor32& descriptor) :
	imagePoint_(imagePoint),
	observationPair_(observationPair),
	descriptor_(descriptor)
{
	// nothing to do here
}

inline const MultiViewMapCreator::Feature::Observations& MultiViewMapCreator::Feature::observations() const
{
	return observations_;
}

inline const Vector3& MultiViewMapCreator::Feature::objectPoint() const
{
	return objectPoint_;
}

inline bool MultiViewMapCreator::Feature::isLocalized() const
{
	ocean_assert(objectPoint_.x() == Numeric::minValue() || minimalObservationSqrDistance_ <= maximalObservationSqrDistance_);

	return objectPoint_.x() != Numeric::minValue();
}

inline Scalar MultiViewMapCreator::Feature::stabilityFactor() const
{
	static_assert(maxTimeWithoutObservation_ > 0.0, "Invalid parameter!");

	return Scalar(1.0 - std::min(timeSinceLastObservation_ / maxTimeWithoutObservation_, 1.0));
}

inline bool MultiViewMapCreator::Feature::isInstable() const
{
	return timeSinceLastObservation_ >= maxTimeWithoutObservation_;
}

}

}

}

#endif // META_OCEAN_TRACKING_MAPBUILDING_MULTI_VIEW_MAP_CREATOR_H
