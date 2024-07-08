/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_DETECTOR_FRAME_CHANGE_DETECTOR_H
#define META_OCEAN_CV_DETECTOR_FRAME_CHANGE_DETECTOR_H

#include "ocean/cv/detector/Detector.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/math/Matrix.h"
#include "ocean/math/Quaternion.h"

#include <array>
#include <vector>

namespace Ocean
{

namespace CV
{

namespace Detector
{

/**
 * This class implements a simple detection algorithm to compute whether a camera's image content has significantly changed between a given frame and a registered keyframe.
 * The implementation uses intensity histogram comparison over local image tiles and scores the overall difference across all tiles in the image space.
 * The class also allows for accelerometer and gyroscope readings to be fed to the detector, to avoid keyframe selection when the image is blurry.
 * @ingroup cvdetector
 */
class OCEAN_CV_DETECTOR_EXPORT FrameChangeDetector
{
	public:

		/// Number of histogram bins to use for intensity values.
		static constexpr unsigned int kNumberIntensityBins = 16u;

		/// Number of values in the range [0, 255] covered by each intensity bin in the histogram.
		static constexpr unsigned int kIntensityBinWidth = 16u;

		/// Histogram type for a one-channel image.
		typedef std::array<uint32_t, kNumberIntensityBins> TileHistogram;

		/// A vector of histograms for tiles in the image.
		typedef std::vector<TileHistogram> TileHistograms;

		/**
		 * Different possible results for processFrame().
		 */
		enum class FrameChangeResult
		{
			INVALID_INPUT = 0,
			NO_CHANGE_DETECTED,
			CHANGE_DETECTED
		};

		/*
		 * Options for the detector.
		 */
		struct Options
		{
			/// Target frame width in pixels, with range (0, infinity). Input frames will be resized to this resolution.
			unsigned int targetFrameWidth = 0u;

			/// Target frame height in pixels, with range (0, infinity). Input frames will be resized to this resolution.
			unsigned int targetFrameHeight = 0u;

			/// Side length, in pixels, of each spatial bin used for local intensity histogram computation, with range (4, infinity).
			unsigned int spatialBinSize = 40u;

			/// Threshold on the (vector magnitude of the) linear acceleration reading from the device's accelerometer, in m/s^2 with range [0, infinity).
			/// If a value greater than this has been observed over the last two frames, we consider the device motion too large to make the current frame a good keyframe.
			Scalar largeMotionAccelerationThreshold = Numeric::maxValue();

			/// Threshold on the (vector magnitude of the) unbiased rotation rate read from the device's gyroscope, in rad/s with range [0, infinity).
			/// If a value greater than this has been observed over the last two frames, we consider the device motion too large to make the current frame a good keyframe.
			Scalar largeMotionRotationRateThreshold = Numeric::maxValue();

			/// Threshold on maximum total device rotation since the last keyframe, based on IMU, in radians with range (0, pi).
			/// If the device pose difference is more than this amount, then the current frame will be set as the keyframe.
			Scalar rotationThreshold = Numeric::pi();

			/// Threshold on the minimum amount of time between keyframes, in seconds, with range [0, preferredMaximumTimeBetweenKeyframes).
			double minimumTimeBetweenKeyframes = 0.0;

			/// Preferred threshold on the maximum amount of time between keyframes, in seconds, with range (minimumTimeBetweenKeyframes, absoluteMaximumTimeBetweenKeyframes].
			/// Keyframes will be set at least at this rate, regardless of whether a significant change in content has occurred.
			/// This value will be ignored, however, if the current frame contains very strong motion.
			double preferredMaximumTimeBetweenKeyframes = NumericD::maxValue();

			/// Absolute threshold on the maximum amount of time between keyframes, in seconds, with range [preferredMaximumTimeBetweenKeyframes, NumericD::maxValue()].
			/// Keyframes will be set at most at this rate, regardless of whether a significant change in content has occurred, and regardless of whether the current frame contains very strong motion.
			double absoluteMaximumTimeBetweenKeyframes = NumericD::maxValue();

			/// Minimum histogram distance between the keyframe and the current frame for a tile to be considered as having significant content change, with range [0, histogramDistanceThreshold).
			Scalar minimumHistogramDistance = Scalar(25.0);

			/// Sets the maximum change considered when scoring a specific tile in the current image, with range (minimumHistogramDistance, infinity).
			Scalar histogramDistanceThreshold = Scalar(100.0);

			/// Minimum "change detection" score for the current frame to be regarded as significantly different from the keyframe.
			/// This score is computed as the weighted proportion of tiles having significant change.
			/// Tile weights are computed as min(tileHistogramDistance / histogramDistanceThreshold, 1), or as 0 if tileHistogramDistance < minimumHistogramDistance.
			Scalar changeDetectionThreshold = Scalar(0.05);

			/**
			 * Checks whether the specified options are valid for processing.
			 * @return True if the spatial bin size is nonzero and no larger than the frame dimensions, otherwise false
			 */
			inline bool isValid() const;
		};

		/**
		 * Creates an invalid frame change detector.
		 */
		inline FrameChangeDetector();

		/**
		 * Creates a new frame change detector with the given parameters.
		 * @param options Set of parameters for the detector
		 */
		explicit FrameChangeDetector(const Options& options);

		/**
		 * Copy constructor.
		 * @param other Frame change detector to copy
		 */
		FrameChangeDetector(const FrameChangeDetector& other);

		/**
		 * Move constructor.
		 * @param other Frame change detector to move to this object
		 */
		inline FrameChangeDetector(FrameChangeDetector&& other) noexcept;

		/**
		 * Records a new acceleration reading from an accelerometer.
		 * @param acceleration 3DOF acceleration values in the device's local frame, in units of m/s^2
		 * @param timestamp Timestamp of the accelerometer sample
		 */
		void addAccelerationSample(const Vector3& acceleration, const Timestamp& timestamp);

		/**
		 * Records a new rotational motion reading from a gyroscope.
		 * @param rotationRate 3DOF rotation rate values in the device's local frame, in units of rad/s
		 * @param timestamp Timestamp of the gyroscope sample
		 */
		void addGyroSample(const Vector3& rotationRate, const Timestamp& timestamp);

		/**
		 * Handles one frame of input and determines whether a significant change in visual content has occurred.
		 * @param yFrame Input frame; must be valid with 8-bit grayscale format; the frame size must not be smaller than (options().targetFrameWidth)x(options().targetFrameHeight).
		 * @param world_R_camera Optional prior on the device's 3DOF orientation as provided by the device's internal sensor fusion algorithm, may be invalid
		 * @param worker Optional worker to distribute the computation
		 * @return Indicator of whether a frame change occurred, or an error value if invalid input was provided
		 */
		FrameChangeResult detectFrameChange(const Frame& yFrame, const Quaternion& world_R_camera, Worker* worker = nullptr);

		/**
		 * Returns the set of options that were specified when this detector was created.
		 * @return Options for the detector
		 */
		inline const Options& options() const;

		/**
		 * Returns the number of rows in the associated tile matrix.
		 * @return Number of rows, equal to options_.targetFrameHeight / options_.spatialBinSize, or zero if the detector is invalid
		 */
		inline unsigned int tileRows() const;

		/**
		 * Returns the number of columns in the associated tile matrix.
		 * @return Number of columns, equal to options_.targetFrameWidth / options_.spatialBinSize, or zero if the detector is invalid
		 */
		inline unsigned int tileColumns() const;

		/**
		 * Returns the most recently computed set of histogram distances for this detector. These distances may not have been computed in the most recently processed frame.
		 * @return tileRows() x tileColumns() matrix of histogram distances, each associated with a specific tile in the image
		 */
		inline const Matrix& tileScores() const;

		/**
		 * Checks whether the detector was created with valid parameters.
		 * @return True, if the detector was created successfully; otherwise, false
		 */
		inline bool isValid() const;

		/**
		 * Move operator.
		 * @param other The detector to be moved
		 * @return Reference to this object
		 */
		inline FrameChangeDetector& operator=(FrameChangeDetector&& other) noexcept;

	private:

		/**
		 * Computes the local intensity histograms for all tiles in the image.
		 * @param yFrame Uint8 grayscale image to process
		 * @param shouldComputeHistogramDistances If true, the histogram difference with the keyframe will be additionally computed for each tile; if false, this step is skipped
		 * @param worker Optional worker to distribute the computation
		 */
		void computeTileHistograms(const Frame& yFrame, bool shouldComputeHistogramDistances, Worker* worker);

		/**
		 * Computes the local intensity histograms for a subset of image tiles.
		 * @param yFrame Pointer to uint8 grayscale image data
		 * @param yFrameStride Stride of the yFrame rows in bytes (i.e., 4-byte-aligned)
		 * @param shouldComputeHistogramDistances If true, the histogram difference with the keyframe will be additionally computed for each tile; if false, this step is skipped
		 * @param tileIndexStart Row-major linear index for the first tile to process
		 * @param numTilesToProcess Number of tiles that will be sequentially processed starting with tileIndex
		 */
		void computeTileHistogramsSubset(const uint8_t* yFrame, const unsigned int yFrameStride, bool shouldComputeHistogramDistances, unsigned int tileIndexStart, unsigned int numTilesToProcess);

		/**
		 * Computes a distance score between two histograms.
		 * @param tileHistogram1 First histogram to compare
		 * @param tileHistogram2 Second histogram to compare
		 * @return Score reflecting the distance between the histograms, with lower values indicating higher similarity
		 */
		static Scalar computeHistogramDistance(const TileHistogram& tileHistogram1, const TileHistogram& tileHistogram2);

	private:

		/// Set of options for the detector.
		Options options_;

		/// Number of tile blocks in the vertical dimension.
		unsigned int tileRows_;

		/// Number of tile blocks in the horizontal dimension.
		unsigned int tileColumns_;

		/// Flattened grid of 2D histograms for the most recently processed frame, stored row-major.
		TileHistograms tileHistograms_;

		/// Flattened grid of 2D histograms for the current keyframe.
		TileHistograms keyframeTileHistograms_;

		/// Timestamp of the last frame that was processed.
		Timestamp lastTimestamp_;

		/// Timestamp of frame prior to the last frame that was processed.
		Timestamp priorLastTimestamp_;

		/// Timestamp of the current keyframe.
		Timestamp keyframeTimestamp_;

		/// Last timestamp at which large motion was detected from the accelerometer or gyro (if any).
		Timestamp lastLargeMotionTimestamp_;

		/// Flattened grid of 2D histogram distances between the most recently processed frame and the current keyframe.
		Matrix histogramDistances_;

		/// 3DOF rotation of the last keyframe relative to a world coordinate frame. If rotations are not available or if no frames have been processed, this will be set to invalid.
		Quaternion world_R_keyframe_;
};

inline bool FrameChangeDetector::Options::isValid() const
{
	return targetFrameWidth > 0u && targetFrameHeight > 0u && spatialBinSize >= 4u && rotationThreshold > Scalar(0.0) && rotationThreshold <= Numeric::pi() && minimumTimeBetweenKeyframes >= Scalar(0.0) && preferredMaximumTimeBetweenKeyframes > minimumTimeBetweenKeyframes && absoluteMaximumTimeBetweenKeyframes >= preferredMaximumTimeBetweenKeyframes && minimumHistogramDistance >= Scalar(0.0) && minimumHistogramDistance < histogramDistanceThreshold;
}

inline FrameChangeDetector::FrameChangeDetector() :
	tileRows_(0u),
	tileColumns_(0u),
	lastTimestamp_(false),
	priorLastTimestamp_(false),
	keyframeTimestamp_(false),
	lastLargeMotionTimestamp_(false),
	world_R_keyframe_(false)
{
	// nothing to do, here
}

inline FrameChangeDetector::FrameChangeDetector(FrameChangeDetector&& other) noexcept :
	options_(other.options_),
	tileRows_(other.tileRows_),
	tileColumns_(other.tileColumns_),
	tileHistograms_(std::move(other.tileHistograms_)),
	keyframeTileHistograms_(std::move(other.keyframeTileHistograms_)),
	lastTimestamp_(other.lastTimestamp_),
	priorLastTimestamp_(other.priorLastTimestamp_),
	keyframeTimestamp_(other.keyframeTimestamp_),
	lastLargeMotionTimestamp_(other.lastLargeMotionTimestamp_),
	histogramDistances_(std::move(other.histogramDistances_)),
	world_R_keyframe_(std::move(other.world_R_keyframe_))
{
	// nothing to do, here
}

inline const FrameChangeDetector::Options& FrameChangeDetector::options() const
{
	return options_;
}

inline unsigned int FrameChangeDetector::tileRows() const
{
	return tileRows_;
}

inline unsigned int FrameChangeDetector::tileColumns() const
{
	return tileColumns_;
}

inline const Matrix& FrameChangeDetector::tileScores() const
{
	return histogramDistances_;
}

inline bool FrameChangeDetector::isValid() const
{
	return tileRows_ > 0u && tileColumns_ > 0u;
}

inline FrameChangeDetector& FrameChangeDetector::operator=(FrameChangeDetector&& other) noexcept
{
	if (&other != this)
	{
		options_ = other.options_;
		tileRows_ = other.tileRows_;
		tileColumns_ = other.tileColumns_;
		tileHistograms_ = std::move(other.tileHistograms_);
		keyframeTileHistograms_ = std::move(other.keyframeTileHistograms_);
		lastTimestamp_ = other.lastTimestamp_;
		priorLastTimestamp_ = other.priorLastTimestamp_;
		keyframeTimestamp_ = other.keyframeTimestamp_;
		lastLargeMotionTimestamp_ = other.lastLargeMotionTimestamp_;
		histogramDistances_ = std::move(other.histogramDistances_);
		world_R_keyframe_ = std::move(other.world_R_keyframe_);
	}

	return *this;
}

} // namespace Detector

} // namespace CV

} // namespace Ocean

#endif // META_OCEAN_CV_DETECTOR_FRAME_CHANGE_DETECTOR_H
