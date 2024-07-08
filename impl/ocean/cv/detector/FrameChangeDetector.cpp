/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/FrameChangeDetector.h"

#include "ocean/cv/FrameInterpolatorNearestPixel.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

FrameChangeDetector::FrameChangeDetector(const Options& options) :
	options_(options),
	tileRows_(0u),
	tileColumns_(0u),
	lastTimestamp_(false),
	priorLastTimestamp_(false),
	keyframeTimestamp_(false),
	lastLargeMotionTimestamp_(false),
	world_R_keyframe_(false)
{
	static_assert(kNumberIntensityBins * kIntensityBinWidth >= 256u, "Histogram bins do not cover the entire [0,255] range!");

	if (options_.isValid())
	{
		// We'll include edge bins if at least 50% of their area covers the image content.
		tileRows_ = std::max(1u, (options_.targetFrameHeight + options_.spatialBinSize / 2u) / options_.spatialBinSize);
		tileColumns_ = std::max(1u, (options_.targetFrameWidth + options_.spatialBinSize / 2u) / options_.spatialBinSize);

		tileHistograms_.resize(tileRows_ * tileColumns_);
		keyframeTileHistograms_.resize(tileRows_ * tileColumns_);
		histogramDistances_ = Matrix(tileRows_, tileColumns_);
	}
}

FrameChangeDetector::FrameChangeDetector(const FrameChangeDetector& other) :
	options_(other.options_),
	tileRows_(other.tileRows_),
	tileColumns_(other.tileColumns_),
	tileHistograms_(other.tileHistograms_),
	keyframeTileHistograms_(other.keyframeTileHistograms_),
	lastTimestamp_(other.lastTimestamp_),
	priorLastTimestamp_(other.priorLastTimestamp_),
	keyframeTimestamp_(other.keyframeTimestamp_),
	lastLargeMotionTimestamp_(other.lastLargeMotionTimestamp_),
	histogramDistances_(other.histogramDistances_),
	world_R_keyframe_(other.world_R_keyframe_)
{
	// nothing to do, here
}

void FrameChangeDetector::addAccelerationSample(const Vector3& acceleration, const Timestamp& timestamp)
{
	ocean_assert(timestamp.isValid());

	if (acceleration.sqr() >= options_.largeMotionAccelerationThreshold * options_.largeMotionAccelerationThreshold)
	{
		lastLargeMotionTimestamp_ = timestamp;
	}
}

void FrameChangeDetector::addGyroSample(const Vector3& rotationRate, const Timestamp& timestamp)
{
	ocean_assert(timestamp.isValid());

	if (rotationRate.sqr() >= options_.largeMotionRotationRateThreshold * options_.largeMotionRotationRateThreshold)
	{
		lastLargeMotionTimestamp_ = timestamp;
	}
}

FrameChangeDetector::FrameChangeResult FrameChangeDetector::detectFrameChange(const Frame& yFrame, const Quaternion& world_R_camera, Worker* worker)
{
	if (!isValid() || !yFrame || yFrame.width() < options_.targetFrameWidth || yFrame.height() < options_.targetFrameHeight || !FrameType::formatIsGeneric(yFrame.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 1u))
	{
		ocean_assert(isValid() && "Frame detector not initialized correctly!");
		ocean_assert(yFrame && "Input frame is invalid!");
		ocean_assert(yFrame.width() >= options_.targetFrameWidth && yFrame.height() >= options_.targetFrameHeight && "Input dimensions are smaller than the target dimensions!");
		ocean_assert(FrameType::formatIsGeneric(yFrame.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 1u) && "Input frame must be an 8-bit grayscale image!");

		return FrameChangeResult::INVALID_INPUT;
	}

	// The first frame is used to initialize our timestamps.
	if (!lastTimestamp_.isValid())
	{
		priorLastTimestamp_ = yFrame.timestamp();
		lastTimestamp_ = yFrame.timestamp();
		lastLargeMotionTimestamp_ = yFrame.timestamp();
	}

	// We can skip this frame if there hasn't been enough time since the last keyframe.
	const double timeSinceKeyframe = keyframeTimestamp_.isValid() ? double(yFrame.timestamp() - keyframeTimestamp_) : NumericD::maxValue();

	if (timeSinceKeyframe < options_.minimumTimeBetweenKeyframes)
	{
		priorLastTimestamp_ = lastTimestamp_;
		lastTimestamp_ = yFrame.timestamp();
		return FrameChangeResult::NO_CHANGE_DETECTED;
	}

	// We can skip this frame if (1) we haven't hit the absolute maximum time between keyframes and
	// (2) there's been a period of high motion over the course of the last two frames. Note that the
	// first frame is always set as a keyframe.

	bool setAsKeyframe = (timeSinceKeyframe >= options_.absoluteMaximumTimeBetweenKeyframes);

	if (!setAsKeyframe && lastLargeMotionTimestamp_ > priorLastTimestamp_)
	{
		priorLastTimestamp_ = lastTimestamp_;
		lastTimestamp_ = yFrame.timestamp();
		return FrameChangeResult::NO_CHANGE_DETECTED;
	}

	setAsKeyframe = (timeSinceKeyframe >= options_.preferredMaximumTimeBetweenKeyframes);

	// If we haven't already obtained a keyframe, or if we've had substantial device rotation since
	// the last keyframe, set the current frame as the keyframe. If the current frame is known to be
	// a keyframe, we won't compute histogram differences with the previous keyframe, since these are
	// only used to determine whether enough change has occurred to warrant using a keyframe.
	if (!setAsKeyframe)
	{
		const Scalar totalRotationSinceKeyframe = (world_R_camera.isValid() && world_R_keyframe_.isValid()) ? world_R_keyframe_.smallestAngle(world_R_camera) : Scalar(0.0);

		setAsKeyframe = (totalRotationSinceKeyframe > options_.rotationThreshold);
	}

	// If the desired frame size for processing is smaller than the input frame size, directly resize to that smaller frame size.
	// It's most efficient to simply perform nearest-neighbor sampling; since we don't need a high-quality resampling, we'll go with this.

	// Resample to the desired size and create a frame with 4-byte-aligned data.
	Frame yFrameResized(yFrame, Frame::ACM_USE_KEEP_LAYOUT);
	const unsigned int paddingElements = (4u - options_.targetFrameWidth % 4u) % 4u;

	if (yFrameResized.width() != options_.targetFrameWidth || yFrameResized.height() != options_.targetFrameHeight || paddingElements != 0u)
	{
		Frame yFrameResampled(FrameType(yFrameResized, options_.targetFrameWidth, options_.targetFrameHeight), paddingElements);
		CV::FrameInterpolatorNearestPixel::resize<uint8_t, 1u>(yFrameResized.constdata<uint8_t>(), yFrameResampled.data<uint8_t>(), yFrameResized.width(), yFrameResized.height(), yFrameResampled.width(), yFrameResampled.height(), yFrameResized.paddingElements(), yFrameResampled.paddingElements(), worker);
		std::swap(yFrameResized, yFrameResampled);
	}

	// Actually compute histograms for the tiles.
	computeTileHistograms(yFrameResized, !setAsKeyframe, worker);

	// Score the current frame difference and update the keyframe if a relevant change in visual content has occurred.
	// The "difference score" between frames is computed as
	//     score = 1/T * \sum_t min(d_t, b) / b, if d_t > a else 0
	// where `d_t` is the histogram difference score for tile `t` and `T` is the total number of tiles.
	// Differences scores less than `a` (default=25) are ignored, and scores are saturated at `b` (default=100).
	// A significant change is identified if `score` is greater than a threshold (default=0.05).
	// This score roughly translates to a weighted proportion of tiles exhibiting significant change.

	if (!setAsKeyframe)
	{
		Scalar score = Scalar(0.0);

		ocean_assert(options_.histogramDistanceThreshold > Scalar(0.0));

		const Scalar invHistogramDistanceThreshold = Scalar(1.0) / options_.histogramDistanceThreshold;

		for (unsigned int r = 0u; r < histogramDistances_.rows(); ++r)
		{
			for (unsigned int c = 0u; c < histogramDistances_.columns(); ++c)
			{
				Scalar tileScore = histogramDistances_(r, c);
				if (tileScore >= options_.minimumHistogramDistance)
				{
					tileScore = std::min(tileScore * invHistogramDistanceThreshold, Scalar(1.0));
					score += tileScore;
				}
			}
		}

		ocean_assert(histogramDistances_.rows() > 0u && histogramDistances_.columns() > 0u);
		score /= Scalar(histogramDistances_.rows() * histogramDistances_.columns());

		setAsKeyframe = (score >= options_.changeDetectionThreshold);
	}

	priorLastTimestamp_ = lastTimestamp_;
	lastTimestamp_ = yFrame.timestamp();

	if (setAsKeyframe)
	{
		// It's faster just to swap the underlying arrays.
		keyframeTileHistograms_.swap(tileHistograms_);

		keyframeTimestamp_ = yFrame.timestamp();

		world_R_keyframe_ = world_R_camera.isValid() ? world_R_camera : Quaternion(false);

		return FrameChangeResult::CHANGE_DETECTED;
	}

	return FrameChangeResult::NO_CHANGE_DETECTED;
}

void FrameChangeDetector::computeTileHistograms(const Frame& yFrame, bool shouldComputeHistogramDistance, Worker* worker)
{
	ocean_assert(isValid());

	if (worker)
	{
		worker->executeFunction(Worker::Function::create(*this, &FrameChangeDetector::computeTileHistogramsSubset, yFrame.constdata<uint8_t>(), yFrame.strideBytes(), shouldComputeHistogramDistance, 0u, 0u), 0u, (unsigned int)(tileHistograms_.size()));
	}
	else
	{
		computeTileHistogramsSubset(yFrame.constdata<uint8_t>(), yFrame.strideBytes(), shouldComputeHistogramDistance, 0u, (unsigned int)(tileHistograms_.size()));
	}
}

void FrameChangeDetector::computeTileHistogramsSubset(const uint8_t* yFrame, const unsigned int yFrameStride, bool shouldComputeHistogramDistance, unsigned int tileIndexStart, unsigned int numTilesToProcess)
{
	static_assert(kNumberIntensityBins != 0u, "Number of histogram bins is set to zero!");

	const unsigned int tileIndexEnd = tileIndexStart + numTilesToProcess;

	// These invariants would only arise due to a bug in initialization.
	ocean_assert(tileHistograms_.size() == tileRows_ * tileColumns_);
	ocean_assert(tileHistograms_.size() == keyframeTileHistograms_.size());
	ocean_assert(tileIndexStart < tileHistograms_.size());
	ocean_assert(tileIndexEnd <= tileHistograms_.size());
	ocean_assert(tileIndexStart < tileIndexEnd);

	// The yFrame should have been internally allocated with a 4-byte-aligned row stride.
	ocean_assert(yFrameStride % 4u == 0u);
	ocean_assert(yFrameStride >= options_.targetFrameWidth);
	ocean_assert(yFrameStride - options_.targetFrameWidth < 4u);

	for (unsigned int tileIndex = tileIndexStart; tileIndex < tileIndexEnd; ++tileIndex)
	{
		TileHistogram& tileHistogram = tileHistograms_[tileIndex];
		tileHistogram.fill(0u);

		ocean_assert(tileColumns_ != 0u);
		const unsigned int tileRow = tileIndex / tileColumns_;
		const unsigned int tileColumn = tileIndex % tileColumns_;

		const unsigned int startRow = std::min(tileRow * options_.spatialBinSize, options_.targetFrameHeight);
		const unsigned int endRow = std::min((tileRow + 1u) * options_.spatialBinSize, options_.targetFrameHeight);

		const unsigned int startColumn = std::min(tileColumn * options_.spatialBinSize, options_.targetFrameWidth);
		const unsigned int endColumn = std::min((tileColumn + 1u) * options_.spatialBinSize, options_.targetFrameWidth);

		for (unsigned int r = startRow; r < endRow; ++r)
		{
			const uint8_t* framePtr = yFrame + r * yFrameStride + startColumn;

			// Encourage vectorized computation by processing 4 pixels at a time.
			for (unsigned int c = startColumn; c + 4u <= endColumn; c += 4u, framePtr += 4u)
			{
				++tileHistogram[uint32_t(framePtr[0]) / kIntensityBinWidth];
				++tileHistogram[uint32_t(framePtr[1]) / kIntensityBinWidth];
				++tileHistogram[uint32_t(framePtr[2]) / kIntensityBinWidth];
				++tileHistogram[uint32_t(framePtr[3]) / kIntensityBinWidth];
			}
		}

		// Compute the histogram difference versus the keyframe, if applicable.
		Scalar histogramDistance = Scalar(0.0);
		if (shouldComputeHistogramDistance)
		{
			const TileHistogram& keyframeTileHistogram = keyframeTileHistograms_[tileIndex];

			histogramDistance = computeHistogramDistance(tileHistogram, keyframeTileHistogram);
		}

		histogramDistances_(tileRow, tileColumn) = histogramDistance;
	}
}

Scalar FrameChangeDetector::computeHistogramDistance(const FrameChangeDetector::TileHistogram& histogram1, const FrameChangeDetector::TileHistogram& histogram2)
{
	static_assert(kNumberIntensityBins != 0u, "Number of histogram bins is set to zero!");

	// TODO Will remove these once the final approach is decided.

	// Sum of absolute differences.
	//Scalar histogramDistance = Scalar(0.0);
	//for (unsigned int bin = 0u; bin < FrameChangeDetector::kNumberIntensityBins; ++bin)
	//{
	//	histogramDistance += Numeric::abs(
	//		static_cast<Scalar>(histogram1[bin]) - static_cast<Scalar>(histogram2[bin]));
	//}

	// Earth-mover's distance.
	//Scalar histogramDistance = Scalar(0.0);
	//Scalar e = Scalar(0.0);
	//for (unsigned int bin = 0u; bin < FrameChangeDetector::kNumberIntensityBins; ++bin)
	//{
	//	e += static_cast<Scalar>(histogram1[bin]) - static_cast<Scalar>(histogram2[bin]);
	//	histogramDistance += Numeric::abs(e);
	//}

	// Thresholded Hellinger Distance.
	//constexpr Scalar kDifferenceThreshold = Scalar(20.0);
	//
	//Scalar histogramDistance = Scalar(0.);
	//for (unsigned int bin = 0u; bin < FrameChangeDetector::kNumberIntensityBins; ++bin)
	//{
	//	if (Numeric::abs(static_cast<Scalar>(histogram1[bin]) - static_cast<Scalar>(histogram2[bin])) > kDifferenceThreshold)
	//	{
	//		++histogramDistance;
	//	}
	//}

	// Chi-squared.
	//Scalar histogramDistance = Scalar(0.0);
	//for (unsigned int bin = 0u; bin < FrameChangeDetector::kNumberIntensityBins; ++bin)
	//{
	//	const Scalar h1 = static_cast<Scalar>(histogram1[bin]);
	//	const Scalar h2 = static_cast<Scalar>(histogram2[bin]);
	//	if (h1 > Scalar(0.0) || h2 > Scalar(0.0))
	//	{
	//		histogramDistance += Numeric::sqr(h1 - h2) / (h1 + h2);
	//	}
	//}

	// Windowed approach.
	Scalar histogramDistance = Scalar(0.0);
	for (unsigned int bin = 1u; bin < FrameChangeDetector::kNumberIntensityBins - 1u; ++bin)
	{
		const Scalar h1 = static_cast<Scalar>(histogram1[bin - 1u] + histogram1[bin] + histogram1[bin + 1u]);
		const Scalar h2 = static_cast<Scalar>(histogram2[bin - 1u] + histogram2[bin] + histogram2[bin + 1u]);
		histogramDistance += Numeric::abs(h1 - h2); // / Scalar(3.0);
	}

	histogramDistance = Numeric::sqrt(histogramDistance / Scalar(3.0));

	return histogramDistance;
}

} // namespace Detector

} // namespace CV

} // namespace Ocean
