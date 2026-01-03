/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_SLAM_TRACKER_H
#define META_OCEAN_TRACKING_SLAM_TRACKER_H

#include "ocean/tracking/slam/SLAM.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Numeric.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace Tracking
{

namespace SLAM
{

/**
 * This class implements the base class for all SLAM trackers.
 * @ingroup trackingslam
 */
class OCEAN_TRACKING_SLAM_EXPORT Tracker
{
	public:

		/**
		 * Definition of individual tracker states.
		 */
		enum TrackerState : uint32_t
		{
			/// The tracker is in an unknown state (e.g., not yet started).
			TS_UNKNOWN = 0u,
			/// The tracker is currently initializing (e.g., building initial map).
			TS_INITIALIZING,
			/// The tracker is currently tracking (e.g., has a valid map and pose).
			TS_TRACKING
		};

		/**
		 * This class implements a configuration object for the tracker.
		 * The configuration holds all parameters that control the behavior of the tracker, including feature detection thresholds, tracking distances, and gravity constraints.
		 */
		class Configuration
		{
			public:

				/**
				 * Creates a new default configuration object.
				 */
				inline Configuration();

				/**
				 * Returns the mean Harris threshold.
				 * The mean threshold is calculated as the average of the minimum and maximum Harris thresholds.
				 * @return The mean Harris threshold value, with range [harrisThresholdMin_, harrisThresholdMax_]
				 */
				inline unsigned int harrisThresholdMean() const;

				/**
				 * Returns whether the time interval between the previous and current frame is within the expected range.
				 * This function checks if the frame rate is consistent with the expected frames per second.
				 * @param previousFrameTimestamp The timestamp of the previous frame, must be valid
				 * @param currentFrameTimestamp The timestamp of the current frame, must be valid and >= previousFrameTimestamp
				 * @return True, if the interval is within the expected range; False otherwise
				 */
				bool isInsideExpectedFrameInterval(const Timestamp& previousFrameTimestamp, const Timestamp& currentFrameTimestamp) const;

				/**
				 * Returns whether this configuration object holds valid setting values.
				 * A configuration is valid if all parameters are within their valid ranges.
				 * @return True, if so; False otherwise
				 */
				inline bool isValid() const;

			public:

				/// The number of spatial bins used for feature distribution, with range [1, 10000].
				unsigned int numberBins_ = 200u;

				/// The minimal threshold value for Harris corners, with range [1, harrisThresholdMax_].
				unsigned int harrisThresholdMin_ = 5u;

				/// The maximal threshold value for Harris corners, with range [harrisThresholdMin_, 254].
				unsigned int harrisThresholdMax_ = 40u;

				/// The size of the image patches to be used for 2D/2D tracking in pixels, possible values are {7, 15, 31}.
				unsigned int patchSize_ = 7u;

				/// The maximal projection error for pose estimation in pixels, with range [0, infinity).
				Scalar maximalProjectionError_ = Scalar(3.5);

				/// The maximal tracking distance for unguided tracking as a fraction of image diagonal, with range (0, 1].
				float maximalTrackingDistanceUnguided_ = 0.025f;

				/// The maximal tracking distance for IMU-guided tracking as a fraction of image diagonal, with range (0, maximalTrackingDistanceUnguided_].
				float maximalTrackingDistanceGuidedIMU_ = 0.015f;

				/// The maximal tracking distance for object point-guided tracking as a fraction of image diagonal, with range (0, maximalTrackingDistanceGuidedIMU_].
				float maximalTrackingDistanceGuidedObjectPoint_ = 0.01f;

				/// The gravity direction in the world coordinate system (pointing towards ground), must be a unit vector.
				Vector3 worldGravity_ = Vector3(0, -1, 0);

				/// The weight factor for gravity constraints in optimization, with range [0, infinity).
				Scalar gravityWeightFactor_ = Scalar(0.001);

				/// The maximal angle between measured and expected gravity direction in radians, with range (0, pi/2].
				Scalar gravityMaximalAngle_ = Numeric::deg2rad(15);

				/// The expected number of frames per second, with range (0, infinity).
				double expectedFramesPerSecond_ = 30.0;
		};

		/**
		 * This class holds a pair of tracking parameters defining the pyramid configuration.
		 * The parameters specify the number of pyramid layers and the search radius in the coarsest layer, which together determine the tracking behavior for feature point tracking.
		 */
		class TrackingParameterPair
		{
			public:

				/**
				 * Creates an invalid tracking parameter pair.
				 */
				TrackingParameterPair() = default;

				/**
				 * Creates a new tracking parameter pair.
				 * @param layers The number of pyramid layers to use, with range [1, infinity)
				 * @param coarsestLayerRadius The search radius in the coarsest pyramid layer in pixels, with range [1, infinity)
				 */
				inline TrackingParameterPair(const unsigned int layers, const unsigned int coarsestLayerRadius);

				/**
				 * Returns whether this parameter pair is valid.
				 * A parameter pair is valid if both the number of layers and the coarsest layer radius are non-zero.
				 * @return True, if so; False otherwise
				 */
				inline bool isValid() const;

			public:

				/// The number of pyramid layers, with range [1, infinity), 0 if invalid.
				unsigned int layers_ = 0u;

				/// The search radius in the coarsest pyramid layer in pixels, with range [1, infinity), 0 if invalid.
				unsigned int coarsestLayerRadius_ = 0u;
		};

		/**
		 * This class holds tracking parameters for different tracking modes.
		 * The class provides separate parameter sets for unguided tracking, IMU-guided tracking, and object point-guided tracking, allowing the tracker to adapt its behavior based on available information.
		 */
		class TrackingParameters
		{
			public:

				/**
				 * Creates invalid tracking parameters.
				 */
				TrackingParameters() = default;

				/**
				 * Creates new tracking parameters based on frame dimensions and configuration.
				 * @param width The width of the frame in pixels, with range [1, infinity)
				 * @param height The height of the frame in pixels, with range [1, infinity)
				 * @param configuration The configuration object containing tracking settings, must be valid
				 */
				TrackingParameters(const unsigned int width, const unsigned int height, const Tracker::Configuration& configuration);

				/**
				 * Returns the appropriate tracking parameter pair based on camera motion and available guidance.
				 * @param world_T_previousCamera The transformation of the previous camera in world coordinates, must be valid
				 * @param previousCamera_Q_currentCamera The rotation from the previous camera to the current camera, invalid if no IMU data is available
				 * @param strongMotionAngle The angle threshold for detecting strong motion in radians, with range [0, pi], default is 1 degree
				 * @return The tracking parameter pair to use for the current tracking situation
				 */
				const TrackingParameterPair& parameterPair(const HomogenousMatrix4& world_T_previousCamera, const Quaternion& previousCamera_Q_currentCamera, const Scalar strongMotionAngle = Numeric::deg2rad(1)) const;

				/**
				 * Returns whether these tracking parameters are valid.
				 * Tracking parameters are valid if the patch size is non-zero and all parameter pairs are valid.
				 * @return True, if so; False otherwise
				 */
				inline bool isValid() const;

			public:

				/// The patch size in pixels, possible values are {7, 15, 31}, 0 if invalid.
				unsigned int patchSize_ = 0u;

				/// Tracking parameters for unguided tracking (no IMU, no prior object points).
				TrackingParameterPair trackingParametersUnguided_;

				/// Tracking parameters for IMU-guided tracking (IMU rotation available).
				TrackingParameterPair trackingParametersGuidedIMU_;

				/// Tracking parameters for object point-guided tracking (3D object points available for prediction).
				TrackingParameterPair trackingParametersGuidedObjectPoint_;
		};

		/// True, to enabled logging (for debugging purposes); False, to disable logging.
		static constexpr bool loggingEnabled_ = false;

	protected:

		/**
		 * This class implements a delay debugger for performance monitoring.
		 * In release builds, the debugger logs warnings when delays exceed the specified threshold.
		 */
		class DelayDebugger
		{
			public:

				/**
				 * Reports a delay and logs a warning if it exceeds the threshold.
				 * In debug builds, this function does nothing.
				 * In release builds, this function measures the elapsed time since the debugger was created and logs a warning if it exceeds the specified threshold.
				 * @param description A human-readable description of the operation being measured, must be valid
				 * @param maxDelayMs The maximum acceptable delay in milliseconds, with range [0, infinity), default is 1.0ms
				 */
				inline void reportDelay(const char* description, const double maxDelayMs = 1.0);

#ifndef OCEAN_DEBUG
			protected:

				/// The high performance timer for measuring elapsed time (release builds only).
				HighPerformanceTimer timer_;
#endif // OCEAN_DEBUG
		};

	public:

		/**
		 * Translates a tracker state to a human-readable string.
		 * @param trackerState The tracker state to translate
		 * @return The string representation of the tracker state, 'Invalid' for unknown values
		 */
		static std::string translateTrackerState(const TrackerState trackerState);

	protected:

		/**
		 * Creates a new tracker object.
		 */
		Tracker() = default;

		/**
		 * Destructs this tracker object.
		 */
		virtual ~Tracker() = default;
};

inline Tracker::Configuration::Configuration()
{
	ocean_assert(isValid());
}

inline unsigned int Tracker::Configuration::harrisThresholdMean() const
{
	return (harrisThresholdMin_ + harrisThresholdMax_ + 1u) / 2u;
}

inline bool Tracker::Configuration::isValid() const
{
	if (numberBins_ <= 1u || numberBins_ > 10000u)
	{
		return false;
	}

	if (harrisThresholdMin_ <= 0u || harrisThresholdMax_ >= 255u || harrisThresholdMin_ > harrisThresholdMax_)
	{
		return false;
	}

	if (patchSize_ != 7u && patchSize_ != 15u && patchSize_ != 31u)
	{
		return false;
	}

	if (maximalProjectionError_ < Scalar(0))
	{
		return false;
	}

	if (maximalTrackingDistanceUnguided_ <= 0.0f || maximalTrackingDistanceUnguided_ > 1.0f)
	{
		return false;
	}

	if (maximalTrackingDistanceGuidedIMU_ <= 0.0f || maximalTrackingDistanceGuidedIMU_ > maximalTrackingDistanceUnguided_)
	{
		return false;
	}

	if (maximalTrackingDistanceGuidedObjectPoint_ <= 0.0f || maximalTrackingDistanceGuidedObjectPoint_ > maximalTrackingDistanceGuidedIMU_)
	{
		return false;
	}

	if (!worldGravity_.isUnit())
	{
		return false;
	}

	if (gravityMaximalAngle_ <= 0 || gravityMaximalAngle_ > Numeric::pi_2())
	{
		return false;
	}

	if (expectedFramesPerSecond_ <= 0.0)
	{
		return false;
	}

	return true;
}

inline Tracker::TrackingParameterPair::TrackingParameterPair(const unsigned int layers, const unsigned int coarsestLayerRadius) :
	layers_(layers),
	coarsestLayerRadius_(coarsestLayerRadius)
{
	// nothing to do here
}

inline bool Tracker::TrackingParameterPair::isValid() const
{
	return layers_ != 0u && coarsestLayerRadius_ != 0u;
}

inline bool Tracker::TrackingParameters::isValid() const
{
	return patchSize_ != 0u && trackingParametersUnguided_.isValid() && trackingParametersGuidedIMU_.isValid() && trackingParametersGuidedObjectPoint_.isValid();
}

inline void Tracker::DelayDebugger::reportDelay([[maybe_unused]] const char* description, [[maybe_unused]] const double maxDelayMs)
{
#ifdef OCEAN_DEBUG
	// no-op in debug builds
#else
	const double delay = timer_.mseconds();

	if (delay > maxDelayMs)
	{
		Log::warning() << "Delay: " << description << ": " << delay << "ms";
	}
#endif // OCEAN_DEBUG
}

inline std::string Tracker::translateTrackerState(const TrackerState trackerState)
{
	switch (trackerState)
	{
		case TS_UNKNOWN:
			return "Unknown";

		case TS_INITIALIZING:
			return "Initializing";

		case TS_TRACKING:
			return "Tracking";
	}

	ocean_assert(false && "Invalid tracker state");
	return "Invalid";
}

}

}

}

#endif // META_OCEAN_TRACKING_SLAM_TRACKER_H
