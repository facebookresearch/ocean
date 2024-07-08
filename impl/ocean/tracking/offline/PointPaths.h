/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_OFFLINE_POINT_PATHS_H
#define META_OCEAN_TRACKING_OFFLINE_POINT_PATHS_H

#include "ocean/tracking/offline/Offline.h"
#include "ocean/tracking/offline/OfflineTracker.h"

#include "ocean/cv/FrameProviderInterface.h"
#include "ocean/cv/FramePyramid.h"
#include "ocean/cv/SubRegion.h"

#include "ocean/math/Vector2.h"

#include "ocean/tracking/Database.h"

namespace Ocean
{

namespace Tracking
{

namespace Offline
{

/**
 * This class implements point path tracker allowing to track image points from frame to frame within a stream of several successive frames.
 * @ingroup trackingoffline
 */
class OCEAN_TRACKING_OFFLINE_EXPORT PointPaths
{
	public:

		/**
		 * Definition of individual camera motion speeds.
		 */
		enum MotionSpeed
		{
			/// A slow motion of the camera.
			MS_SLOW,
			/// A moderate motion of the camera.
			MS_MODERATE,
			/// A fast motion of the camera.
			MS_FAST
		};

		/**
		 * Definition of individual tracking methods.
		 */
		enum TrackingMethod
		{
			/// Invalid tracking method.
			TM_INVALID,
			/// Using a patch with size 7.
			TM_FIXED_PATCH_SIZE_7,
			/// Using a patch with size 15.
			TM_FIXED_PATCH_SIZE_15,
			/// Using a patch with size 31.
			TM_FIXED_PATCH_SIZE_31
		};

		/**
		 * Definition of a class holding a point tracking configuration.
		 */
		class TrackingConfiguration
		{
			public:

				/**
				 * Creates an invalid tracking configuration object.
				 */
				TrackingConfiguration() = default;

				/**
				 * Creates a new tracking configuration object.
				 * @param trackingMethod The tracking method to be used
				 * @param frameWidth The width of the frame in pixel, with range [1, infinity)
				 * @param frameHeight The height of the frame in pixel, with range [1, infinity)
				 * @param numberBins The number of the bins in horizontal or vertical direction depending on the larger value, with range [1, infinity)
				 * @param strength The minimal strength parameter for tracking points, with range [0, 256]
				 * @param coarsestLayerRadius The search radius on the coarsest pyramid layer in pixels, with range [2, infinity)
				 * @param pyramidLayers The number of pyramid layers, with range [1, infinity)
				 */
				inline TrackingConfiguration(const TrackingMethod trackingMethod, const unsigned int frameWidth, const unsigned int frameHeight, const unsigned int numberBins, const unsigned int strength, const unsigned int coarsestLayerRadius, const unsigned int pyramidLayers);

				/**
				 * Creates a new tracking configuration object.
				 * @param trackingMethod The tracking method to be used
				 * @param horizontalBinSize The number of pixels per horizontal bin filtering tracking points before they are tracked, 0 to avoid any filtering
				 * @param verticalBinSize The number of pixels per vertical bin filtering tracking points before they are tracked, 0 to avoid any filtering
				 * @param strength The minimal strength parameter for tracking points, with range [0, 256]
				 * @param coarsestLayerRadius The search radius on the coarsest pyramid layer in pixels, with range [2, infinity)
				 * @param pyramidLayers The number of pyramid layers, with range [1, infinity)
				 */
				inline TrackingConfiguration(const TrackingMethod trackingMethod, const unsigned int horizontalBinSize, const unsigned int verticalBinSize, const unsigned int strength, const unsigned int coarsestLayerRadius, const unsigned int pyramidLayers);

				/**
				 * Returns the tracking method of this configuration.
				 * @return The tracking method
				 */
				inline TrackingMethod trackingMethod() const;

				/**
				 * Returns the horizontal bin size of this configuration.
				 * A tracking area with width 100 pixel and horizontal bin size 50 pixel will be tracked by application of two horizontal bins.
				 * @return The horizontal bin size, with range [1, infinity), 0 to avoid the application of bins
				 */
				inline unsigned int horizontalBinSize() const;

				/**
				 * Returns the vertical bin size of this configuration.
				 * A tracking area with height 100 pixel and vertical bin size 50 pixel will be tracked by application of two vertical bins.
				 * @return The vertical bin size in pixel, with range [1, infinity), 0 to avoid the application of bins
				 */
				inline unsigned int verticalBinSize() const;

				/**
				 * Returns the minimal strength parameter of this configuration.
				 * @return The minimal strength parameter, with range [0, infinity)
				 */
				inline unsigned int strength() const;

				/**
				 * Returns the search radius on the coarsest pyramid layer in pixel.
				 * @return The radius on the coarsest pyramid layer, with range [2, infinity) if valid
				 */
				inline unsigned int coarsestLayerRadius() const;

				/**
				 * Returns the number of pyramid layers of this configuration.
				 * @return The number of pyramid layers, with range [1, infinity) if valid
				 */
				inline unsigned int pyramidLayers() const;

				/**
				 * Returns the number of horizontal bins that are necessary if this configuration is applied to a given frame or frame area.
				 * @param areaWidth The width of the area to which this configuration is applied, may be the width of an entire frame or the width of a sub-region, with range [1, infinity)
				 * @param minimalBins The minimal number of bins that will be returned (if bin-filtering is intended by this configuration)
				 * @return The number of horizontal bins, 0 if the application of bin-filtering is not intended by this configuration
				 */
				inline unsigned int horizontalBins(const unsigned int areaWidth, const unsigned int minimalBins) const;

				/**
				 * Returns the number of vertical bins that are necessary if this configuration is applied to a given frame or frame area.
				 * @param areaHeight The height of the area to which this configuration is applied, may be the height of an entire frame or the height of a sub-region, with range [1, infinity)
				 * @param minimalBins The minimal number of bins that will be returned (if bin-filtering is intended by this configuration)
				 * @return The number of vertical bins, 0 if the application of bin-filtering is not intended by this configuration
				 */
				inline unsigned int verticalBins(const unsigned int areaHeight, const unsigned int minimalBins) const;

				/**
				 * Weakens the tracking configurations so that more feature points will be used for tracking while the tracking will take longer.
				 * If the provided factor are larger than 1 the configuration will get stricter.
				 * @param binSizeFactor The factor which will be multiplied to the current horizontal and vertical bin sizes, with range (0, infinity)
				 * @param strengthFactor The factor which will be multiplied to the current strength value, with range (0, infinity)
				 * @param minimalBinSize Optional minimal bin size value ensuring that the weakened value does not drop below this threshold, with range [0, infinity)
				 * @param minimalStrength Optional minimal strength value ensuring that the weakened value does not drop below this threshold, with range [0, infinity)
				 * @return True, if the configuration value have been changed
				 */
				bool weakenConfiguration(const Scalar binSizeFactor = Scalar(0.5), const Scalar strengthFactor = Scalar(0.5), const unsigned int minimalBinSize = 0u, const unsigned int minimalStrength = 0u);

				/**
				 * Returns whether this configuration object is valid.
				 * @return True, if so
				 */
				inline bool isValid() const;

			private:

				/// The tracking method of this configuration.
				TrackingMethod trackingMethod_ = TM_INVALID;

				/// The horizontal bin size of this configuration.
				unsigned int horizontalBinSize_ = 0u;

				/// The vertical bin size of this configuration.
				unsigned int verticalBinSize_ = 0u;

				/// The strength parameter of this configuration.
				unsigned int strength_ = 0u;

				/// The number of pyramid layers of this configuration, with range [2, infinity) if valid
				unsigned int pyramidLayers_ = 0u;

				/// The search radius on the coarsest pyramid layers, with range [1, infinity) if valid
				unsigned int coarsestLayerRadius_ = 0u;
		};

	protected:

		/**
		 * Definition of a pair combining a tracker configuration and a ratio value.
		 */
		using TrackingConfigurationPair = std::pair<TrackingConfiguration, unsigned int>;

		/**
		 * Definition of a vector holding TrackingConfigurationPair objects.
		 */
		using TrackingConfigurationPairs = std::vector<TrackingConfigurationPair>;

		/**
		 * Definition of a vector holding strength parameters.
		 */
		using Strengths = std::vector<int>;

	public:

		/**
		 * Tracks reliable points between successive frames and joins points paths to a common/shared object points.
		 * Further, camera poses are registered for each camera frame (without the actual pose determination).<br>
		 * @param frameProviderInterface The frame provider interface which is used to extract the individual frames, must be valid and must be initialized
		 * @param pixelFormat The pixel format which is used for each frame
		 * @param pixelOrigin The pixel origin which is used for each frame
		 * @param trackingConfiguration The tracking configuration that is applied to track the points
		 * @param lowerFrameIndex The index of the lower frame which will be used for tracking
		 * @param startFrameIndex The index of the frame at which the determination of the point paths will start, with range [lowerFrameIndex, upperFrameIndex]
		 * @param upperFrameIndex The index of the upper frame which will be used for tracking, with range [lowerFrame, infinity)
		 * @param invalidBorderSize The border size at the outer frame border in which tracked points will count as invalid, in pixel, with range [0, min(width / 2, height / 2))
		 * @param onlyNewObjectPoints True, to add and track only object points which are not part of the database yet
		 * @param database The resulting database holding the object points, image points and camera poses after tracking, must be empty
		 * @param worker Optional worker object to distribute the computation
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @param progress Optional resulting progress with range [0, 1]
		 * @return True, if succeeded
		 */
		static bool determinePointPaths(CV::FrameProviderInterface& frameProviderInterface, const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin, const TrackingConfiguration& trackingConfiguration, const unsigned int lowerFrameIndex, const unsigned int startFrameIndex, const unsigned int upperFrameIndex, const unsigned int invalidBorderSize, const bool onlyNewObjectPoints, Database& database, Worker* worker = nullptr, bool* abort = nullptr, Scalar* progress = nullptr);

		/**
		 * Tracks reliable points between successive frames starting at a specific frame in a specific sub-region.
		 * Successive tracked points will be joined to a path representing the same object point.
		 * @param frameProviderInterface The frame provider interface which is used to extract the individual frames, must be valid and must be initialized
		 * @param pixelFormat The pixel format which is used for each frame
		 * @param pixelOrigin The pixel origin which is used for each frame
		 * @param trackingConfiguration The tracking configuration that is applied to track the points
		 * @param lowerFrameIndex The index of the lower frame which will be used for tracking
		 * @param subRegion The sub-region defining a specific frame area in the start frame in which all reliable points will be tracked
		 * @param subRegionFrameIndex The index of the frame in which the sub-region is defined
		 * @param upperFrameIndex The index of the upper frame which will be used for tracking, with range [lowerFrame, infinity)
		 * @param invalidBorderSize The border size at the outer frame border in which tracked points will count as invalid, in pixel, with range [0, min(width / 2, height / 2))
		 * @param onlyNewObjectPoints True, to add and track only object points which are not part of the database yet
		 * @param database The resulting database holding the object points, image points and camera poses after tracking, must be empty
		 * @param worker Optional worker object to distribute the computation
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @param progress Optional resulting progress with range [0, 1]
		 * @return True, if succeeded
		 */
		static bool determinePointPaths(CV::FrameProviderInterface& frameProviderInterface, const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin, const TrackingConfiguration& trackingConfiguration, const unsigned int lowerFrameIndex, const CV::SubRegion& subRegion, const unsigned int subRegionFrameIndex, const unsigned int upperFrameIndex, const unsigned int invalidBorderSize, const bool onlyNewObjectPoints, Database& database, Worker* worker = nullptr, bool* abort = nullptr, Scalar* progress = nullptr);

		/**
		 * Determines the tracking configuration for an explicit specified tracking quality.
		 * @param frameProviderInterface The frame provider interface providing the frame access
		 * @param regionOfInterest The optional region of interest for which the specific region-of-interest-configuration may be determined, an invalid region otherwise
		 * @param trackingQuality The tracking quality for which the tracking configuration will be determined
		 * @param motionSpeed The expected motion speed of the image content
		 * @param frameTrackingConfiguration The resulting tracking configuration for the entire frame, if defined
		 * @param regionOfInterestTrackingConfiguration The resulting tracking configuration for the specified region of interest, if defined and if a region of interest is defined
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @return True, if succeeded
		 */
		static bool determineTrackingConfiguration(CV::FrameProviderInterface& frameProviderInterface, const CV::SubRegion& regionOfInterest, const OfflineTracker::TrackingQuality trackingQuality, const MotionSpeed motionSpeed, TrackingConfiguration* frameTrackingConfiguration, TrackingConfiguration* regionOfInterestTrackingConfiguration, bool* abort = nullptr);

		/**
		 * Determines the best matching tracking configuration for the point tracker starting at a specific frame.
		 * The most suitable configuration can either be determined for the entire frame, for a specified region of interest or for both areas.
		 * @param frameProviderInterface The frame provider interface providing the frame access
		 * @param pixelOrigin The origin of the frame for which the configuration will be determined
		 * @param motionSpeed The expected motion speed of the image content
		 * @param frameIndex The index of the frame for which the configuration will be determined
		 * @param regionOfInterest The optional region of interest for which the specific region-of-interest-configuration may be determined, an invalid region otherwise
		 * @param frameTrackingConfiguration The resulting tracking configuration for the entire frame, if defined
		 * @param regionOfInterestTrackingConfiguration The resulting tracking configuration for the specified region of interest, if defined and if a region of interest is defined
		 * @param worker Optional worker object to distribute the computation
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @return True, if succeeded
		 */
		static bool determineAutomaticTrackingConfiguration(CV::FrameProviderInterface& frameProviderInterface, const FrameType::PixelOrigin pixelOrigin, const MotionSpeed motionSpeed, const unsigned int frameIndex, const CV::SubRegion& regionOfInterest, TrackingConfiguration* frameTrackingConfiguration, TrackingConfiguration* regionOfInterestTrackingConfiguration, Worker* worker = nullptr, bool* abort = nullptr);

		/**
		 * Determines the best matching tracking configuration for the point tracker tracking frames within a specified frame range.
		 * @param frameProviderInterface The frame provider interface providing the frame access
		 * @param pixelOrigin The origin of the frame for which the configuration will be determined
		 * @param motionSpeed The expected motion speed of the image content
		 * @param lowerFrameIndex The index of the frame defining the lower frame range, with range [0, infinity)
		 * @param upperFrameIndex The index of the frame defining the upper frame range, with range [lowerFrameIndex, infinity)
		 * @param frameTrackingConfiguration The resulting tracking configuration for the entire frame
		 * @param intermediateFrames The number of intermediate frames which will be distributed within the specified frame range to determine the best matching configuration
		 * @param worker Optional worker object to distribute the computation
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @return True, if succeeded
		 */
		static bool determineAutomaticTrackingConfiguration(CV::FrameProviderInterface& frameProviderInterface, const FrameType::PixelOrigin pixelOrigin, const MotionSpeed motionSpeed, const unsigned int lowerFrameIndex, const unsigned int upperFrameIndex, TrackingConfiguration& frameTrackingConfiguration, const unsigned int intermediateFrames = 5u, Worker* worker = nullptr, bool* abort = nullptr);

		/**
		 * Determines the number of necessary pyramid layers and coarsest layer radius for a specified frame dimension and motion speed.
		 * @param width The width of the frame (the width of the finest pyramid layer) in pixel, with range [1, infinity)
		 * @param height The height of the frame (the height of the finest pyramid layer) in pixel, with range [1, infinity)
		 * @param motionSpeed The motion speed for which the minimal number of pyramid layers will be determined
		 * @param coarsestLayerRadius Resulting search radius for the coarsest pyramid layer, with range [2, infinity)
		 * @param layers Resulting number of pyramid layers necessary for the defined parameters, with range [3, infinity)
		 * @param maximalCoarsestLayerRadius The maximal (largest) search radius on the coarsest pyramid layers which can be accepted, with range [2, infinity)
		 * @param maximalLayers The maximal number of pyramid layers which can be accepted, with range [1, infinity)
		 */
		static void idealPyramidParameters(const unsigned int width, const unsigned int height, const MotionSpeed motionSpeed, unsigned int& coarsestLayerRadius, unsigned int& layers, const unsigned int maximalCoarsestLayerRadius = 26u, const unsigned int maximalLayers = (unsigned int)(-1));

	protected:

		/**
		 * Applies a bidirectional tracking of points between to frames.
		 * @param previousFramePyramid The frame pyramid of the previous frame, must be valid
		 * @param currentFramePyramid The frame pyramid of the current frame, must be valid and must have the same number of layers as 'previousFramePyramid'
		 * @param coarsestLayerRadius The search radius on the coarsest layer, with range [2, infinity)
		 * @param previousFeatureStrengths The strength values for each individual previous image point
		 * @param trackingMethod The tracking method to be used to track the points
		 * @param previousFeaturePoints The image points located in the previous frame
		 * @param currentFeaturePoints The resulting tracked image points located in the current frame
		 * @param validIndices The indices of all image points that could be tracked reliably
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool trackPoints(const CV::FramePyramid& previousFramePyramid, const CV::FramePyramid& currentFramePyramid, const unsigned int coarsestLayerRadius, const Strengths& previousFeatureStrengths, const TrackingMethod trackingMethod, Vectors2& previousFeaturePoints, Vectors2& currentFeaturePoints, Indices32& validIndices, Worker* worker);
};

inline PointPaths::TrackingConfiguration::TrackingConfiguration(const TrackingMethod trackingMethod, const unsigned int frameWidth, const unsigned int frameHeight, const unsigned int numberBins, const unsigned int strength, const unsigned int coarsestLayerRadius, const unsigned int pyramidLayers) :
	trackingMethod_(trackingMethod),
	horizontalBinSize_(0u),
	verticalBinSize_(0u),
	strength_(strength),
	pyramidLayers_(pyramidLayers),
	coarsestLayerRadius_(coarsestLayerRadius)
{
	const unsigned int maxValue = max(frameWidth, frameHeight);

	ocean_assert(numberBins != 0u);

	horizontalBinSize_ = maxValue / numberBins;
	ocean_assert(horizontalBinSize_ >= 0u && horizontalBinSize_ <= maxValue);

	verticalBinSize_ = horizontalBinSize_;

	ocean_assert((coarsestLayerRadius_ == 0u && pyramidLayers_ == 0u) || (coarsestLayerRadius_ >= 2u && pyramidLayers_ >= 1u));
}

inline PointPaths::TrackingConfiguration::TrackingConfiguration(const TrackingMethod trackingMethod, const unsigned int horizontalBinSize, const unsigned int verticalBinSize, const unsigned int strength, const unsigned int coarsestLayerRadius, const unsigned int pyramidLayers) :
	trackingMethod_(trackingMethod),
	horizontalBinSize_(horizontalBinSize),
	verticalBinSize_(verticalBinSize),
	strength_(strength),
	pyramidLayers_(pyramidLayers),
	coarsestLayerRadius_(coarsestLayerRadius)
{
	ocean_assert((coarsestLayerRadius_ == 0u && pyramidLayers_ == 0u) || (coarsestLayerRadius_ >= 2u && pyramidLayers_ >= 1u));
}

inline PointPaths::TrackingMethod PointPaths::TrackingConfiguration::trackingMethod() const
{
	return trackingMethod_;
}

inline unsigned int PointPaths::TrackingConfiguration::horizontalBinSize() const
{
	return horizontalBinSize_;
}

inline unsigned int PointPaths::TrackingConfiguration::verticalBinSize() const
{
	return verticalBinSize_;
}

inline unsigned int PointPaths::TrackingConfiguration::strength() const
{
	return strength_;
}

inline unsigned int PointPaths::TrackingConfiguration::coarsestLayerRadius() const
{
	return coarsestLayerRadius_;
}

inline unsigned int PointPaths::TrackingConfiguration::pyramidLayers() const
{
	return pyramidLayers_;
}

inline unsigned int PointPaths::TrackingConfiguration::horizontalBins(const unsigned int areaWidth, const unsigned int minimalBins) const
{
	ocean_assert(areaWidth >= 1u);

	if (horizontalBinSize_ == 0u)
	{
		return 0u;
	}

	return max(minimalBins, (areaWidth + horizontalBinSize_ / 2u) / horizontalBinSize_);
}

inline unsigned int PointPaths::TrackingConfiguration::verticalBins(const unsigned int areaHeight, const unsigned int minimalBins) const
{
	ocean_assert(areaHeight >= 1u);

	if (verticalBinSize_ == 0u)
	{
		return 0u;
	}

	return max(minimalBins, (areaHeight + verticalBinSize_ / 2u) / verticalBinSize_);
}

inline bool PointPaths::TrackingConfiguration::isValid() const
{
	return trackingMethod_ != TM_INVALID && coarsestLayerRadius_ != 0u && pyramidLayers_ != 0u;
}

}

}

}

#endif // META_OCEAN_TRACKING_OFFLINE_POINT_PATHS_H
