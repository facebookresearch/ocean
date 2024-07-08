/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/cv/detector/barcodes/Barcodes.h"
#include "ocean/cv/detector/barcodes/Barcode.h"

#include "ocean/base/DataType.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/PixelPosition.h"

#include "ocean/math/FiniteLine2.h"
#include "ocean/math/Line2.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace Barcodes
{

/**
 * This class converts raw pixel data into binary segments.
 *
 * The process starts by searching for an intensity jump from background intensity to foreground intensity values.
 * Once a transition has been found, it will determine a gray value that's used to threshold the following pixels.
 * When requested, the segmenter can prepare the N segments in advance. Once done, the segmenter will advance to
 * the next intensity jump. This process continues until all raw pixels have been processed.
 *
 * Suggested use:
 * <pre>
 * const bool isNormalReflectance = ...;
 * const uint8_t gradientThreshold = 20u;
 * const uint8_t* buffer = ...;
 * const size_t bufferSize = ...;
 *
 * RowSegmenter rowSegmenter(buffer, bufferSize, gradientThreshold, isNormalReflectance);
 *
 * while (rowSegmenter.findNextTransitionToForeground())
 * {
 *     rowSegmenter.prepareSegments(maxNumberRequiredSegments);
 *
 *     if (rowSegmenter.size() < minNumberRequiredSegments)
 *     {
 *         // There aren't enough segments.
 *         continue;
 *     }
 *
 *     // work with the segments ...
 * }
 * </pre>
 * @ingroup cvdetectorbarcodes
 * @tparam TPixel The type of the raw pixels that will be processed, e.g. `uint8_t`.
 */
template <typename TPixel>
class RowSegmenter
{
	public:

		// The type that is used for the image gradient.
		typedef typename DifferenceValueTyper<TPixel>::Type TGradient;

	protected:

		/**
		 * This class implements a simple history for previous pixel transitions (a sliding window of pixel transitions).
		 */
		class TransitionHistory
		{
			public:

				/**
				* Creates a new history object.
				*/
				TransitionHistory() = default;

				/**
				* Returns the history with window size N.
				* @return The sum of the most recent delta
				*/
				inline TGradient history1();

				/**
				* Returns the history with window size N.
				* @return The sum of the most recent delta
				*/
				inline TGradient history2();

				/**
				* Returns the history with window size N.
				* @return The sum of the most recent delta
				*/
				inline TGradient history3();

				/**
				* Adds a new delta object as most recent history.
				* Existing history objects will be moved by one pixel.
				* @param newDelta The new delta object to be added
				*/
				inline void push(const TGradient newDelta);

				/**
				* Resets the history object.
				*/
				inline void reset();

			protected:

				/// The most recent deltas.
				TGradient deltas_[3] = {0, 0, 0};
		};

		/// Definition of a function pointer for function that determine intensity transitions between back- and foreground pixels.
		typedef bool (*IsTransitionFunc)(const TPixel*, const TGradient, TransitionHistory&);

	public:

		/**
		 * Creates a segmenter object for a buffer of raw pixel data.
		 * @param pixelData The pointer to the raw pixel data that will be processed, must be valid
		 * @param pixelDataSize The size of the raw pixel data, range: [1, infinity)
		 * @param minimumGradient The minimum value of the pixel gradient required to count as a transition, range: (-infinity, infinity)
		 * @param isNormalReflectance Indicates whether the segmenter should look for transitions with normal or inverted reflectance
		 */
		RowSegmenter(const TPixel* pixelData, const size_t pixelDataSize, const TGradient minimumGradient, const bool isNormalReflectance);

		/**
		 * Returns if this segmenter is valid
		 * @return True if this segmenter is valid, otherwise false
		 */
		bool isValid() const;

		/**
		 * Finds the next transition from background to foreground in the raw pixel data
		 * @return True if a transition has been found, otherwise false
		 */
		bool findNextTransitionToForeground();

		/**
		 * Prepares a batch of segments
		 * @note: Calling this function without having `findNextTransitionToForeground()` first (with `true` as return value) results in undefined behavior!
		 * @param numberSegments The number of segments that should be prepared, range: (1, infinity)
		 * @return True if the number of requested segments are available, otherwise false
		 */
		bool prepareSegments(const size_t numberSegments);

		/**
		 * Returns the current segment data
		 * @return The current segment data
		 */
		const SegmentData& segmentData() const;

		/**
		 * Returns the size of the raw pixel data that is handled by this object
		 * @return The size of the raw pixel data in elements
		 */
		size_t size() const;

		/**
		 * Returns the current position of the segmenter in the raw pixel data
		 * @return The current position
		 */
		size_t position() const;

		/**
		 * Sets the position of the segmenter in the raw pixel data
		 * @param position The index position to where the segmenter should be set, range: [0, size())
		 */
		bool setPosition(const size_t position);

	protected:

		/**
		 * Checks if the specified pixel is a transition from light to dark pixels
		 * @param pixel The pointer to the pixel that will be checked, must be valid
		 * @param minimumGradient The minimum value of the pixel gradient that must be exceed for it count as a intensity transition, range: (-infinity, infinity)
		 * @param history The object holding the recent pixel history, must be valid
		 */
		static bool isTransitionLightToDark(const TPixel* pixel, const TGradient minimumGradient, TransitionHistory& history);

		/**
		 * Checks if the specified pixel is a transition from dark to light pixels
		 * @param pixel The pointer to the pixel that will be checked, must be valid
		 * @param minimumGradient The minimum value of the pixel gradient that must be exceed for it count as a intensity transition, range: (-infinity, infinity)
		 * @param history The object holding the recent pixel history, must be valid
		 */
		static bool isTransitionDarkToLight(const TPixel* pixel, const TGradient minimumGradient, TransitionHistory& history);

	protected:

		/// A pointer to the function that checks if there is a transition from background to foreground (this depends on the reflectance type)
		IsTransitionFunc isTransitionToForeground_ = nullptr;

		/// A pointer to the function that checks if there is a transition from foreground to background (this depends on the reflectance type)
		IsTransitionFunc isTransitionToBackground_ = nullptr;

		/// The pointer to the raw pixel data that will be processed by this object.
		const TPixel* pixelData_ = nullptr;

		/// The size of the raw pixel data in elements.
		size_t size_ = 0;

		/// The minimum value of the pixel gradient that must be exceed for it count as a intensity transition.
		TGradient minimumGradient_ = TGradient(0);

		/// The current position of the segmenter in the raw pixel data.
		size_t position_ = 0;

		/// The position of the segmenter in the raw pixel data when creating new segments (thresholding), this is `segmentPosition_ = position_ + X`
		size_t segmentPosition_ = 0;

		/// The memory holding the current segments
		SegmentData segmentData_;

		/// The object that holds the recent pixel history.
		TransitionHistory transitionHistory_;
};

template <typename TPixel>
inline typename RowSegmenter<TPixel>::TGradient RowSegmenter<TPixel>::TransitionHistory::history1()
{
	return deltas_[0];
}

template <typename TPixel>
inline typename RowSegmenter<TPixel>::TGradient RowSegmenter<TPixel>::TransitionHistory::history2()
{
	return deltas_[0] + deltas_[1];
}

template <typename TPixel>
inline typename RowSegmenter<TPixel>::TGradient RowSegmenter<TPixel>::TransitionHistory::history3()
{
	return deltas_[0] + deltas_[1] + deltas_[2];
}

template <typename TPixel>
inline void RowSegmenter<TPixel>::TransitionHistory::push(const TGradient newDelta)
{
	deltas_[2] = deltas_[1];
	deltas_[1] = deltas_[0];
	deltas_[0] = newDelta;
}

template <typename TPixel>
inline void RowSegmenter<TPixel>::TransitionHistory::reset()
{
	deltas_[0] = 0;
	deltas_[1] = 0;
	deltas_[2] = 0;
}

template <typename TPixel>
RowSegmenter<TPixel>::RowSegmenter(const TPixel* pixelData, const size_t pixelDataSize, const TGradient minimumGradient, const bool isNormalReflectance) :
	pixelData_(pixelData),
	size_(pixelDataSize),
	minimumGradient_(minimumGradient)
{
	if (isNormalReflectance)
	{
		isTransitionToForeground_ = isTransitionLightToDark;
		isTransitionToBackground_ = isTransitionDarkToLight;
	}
	else
	{
		isTransitionToForeground_ = isTransitionDarkToLight;
		isTransitionToBackground_ = isTransitionLightToDark;
	}

	ocean_assert(isValid());
}

template <typename TPixel>
bool RowSegmenter<TPixel>::isValid() const
{
	return pixelData_ != nullptr && size_ != 0 && minimumGradient_ > TGradient(0) && isTransitionToForeground_ != nullptr && isTransitionToBackground_ != nullptr;
}

template <typename TPixel>
bool RowSegmenter<TPixel>::findNextTransitionToForeground()
{
	if (!isValid())
	{
		return false;
	}

	// Delete any previous segments
	segmentData_.clear();

	transitionHistory_.reset();
	++position_;

	ocean_assert(position_ != 0);

	while (position_ < size_ && !isTransitionToForeground_(pixelData_ + position_, minimumGradient_, transitionHistory_))
	{
		++position_;
	}

	if (position_ < size_)
	{
		segmentPosition_ = position_;

		return true;
	}

	return false;
}

template <typename TPixel>
bool RowSegmenter<TPixel>::prepareSegments(const size_t numberSegments)
{
	if (!isValid())
	{
		return false;
	}

	if (numberSegments <= segmentData_.size())
	{
		// There are sufficient segments already, no need to find additional ones
		return true;
	}

#if 1
	// Use a gray threshold to determine the next segments.

	// Use the midpoint between the previous (background) and the current (foreground) pixel as gray threshold.
	const TPixel grayThreshold_ = (pixelData_[position_ - 1] + pixelData_[position_]) / 2u;

	while (segmentPosition_ < size_ && numberSegments > segmentData_.size())
	{
		const bool atForeground = segmentData_.size() % 2 == 0;

		size_t nextSegmentPosition = segmentPosition_ + 1;

		if (atForeground)
		{
			while (nextSegmentPosition < size_ && pixelData_[nextSegmentPosition] < grayThreshold_)
			{
				nextSegmentPosition++;
			}
		}
		else
		{
			while (nextSegmentPosition < size_  && pixelData_[nextSegmentPosition] >= grayThreshold_)
			{
				nextSegmentPosition++;
			}
		}

		ocean_assert(nextSegmentPosition >= segmentPosition_);
		const uint32_t segmentSize = uint32_t(nextSegmentPosition - segmentPosition_); // nextSegmentPosition is the first element of the next segment, so no +1 necessary

		if (segmentSize == 0)
		{
			return false;
		}

		segmentData_.emplace_back(segmentSize);
		segmentPosition_ = nextSegmentPosition;
	}
#else
	// Determine the next segments by searching for intensity transitions, i.e. locations where the gradient exceeds a certain threshold.

	// Note: after experimentation, this seems to be easily affected by pixel noise. Using a longer transition history (smoothing) to reduce noise increases the minimum number of pixels per module.

	while (segmentPosition_ < pixelDataSize_ && numberSegments > segmentData_.size())
	{
		// Segment data alternates between foreground and background data. The first element is a foreground segment.
		const bool atForeground = segmentData_.size() % 2 == 0;

		IsTransitionFunc isNextTransition = nullptr;

		if (atForeground)
		{
			isNextTransition = isTransitionToBackground_;
		}
		else
		{
			isNextTransition = isTransitionToForeground_;
		}

		size_t nextSegmentPosition = segmentPosition_ + 1u;

		while (nextSegmentPosition < pixelDataSize_ && !isNextTransition(pixelData_ + nextSegmentPosition, minimumGradient_, transitionHistory_))
		{
			++nextSegmentPosition;
		}

		ocean_assert(nextSegmentPosition >= segmentPosition_);
		const uint32_t segmentSize = uint32_t(nextSegmentPosition - segmentPosition_); // nextSegmentPosition is the first element of the next segment, so no +1 necessary

		if (segmentSize == 0)
		{
			return false;
		}

		segmentData_.emplace_back(segmentSize);
		segmentPosition_ = nextSegmentPosition;
	}
#endif

	if (numberSegments <= segmentData_.size())
	{
		return true;
	}

	return false;
}

/**
 * This class implements a detector for barcodes.
 * @ingroup cvdetectorbarcodes
 */
class OCEAN_CV_DETECTOR_BARCODES_EXPORT BarcodeDetector2D
{
	public:

		/**
		 * Definition of optional detection features.
		 * @note Enabling additional features will reduce the runtime performance of the detector.
		 */
		enum DetectionFeatures : uint32_t
		{
			/// Standard features that should be sufficient for most cases (excluding all the cases below).
			DF_STANDARD = 0u,
			/// Enables additional scan line directions, i.e. besides horizontal lines, there will also be scan lines at 45, 90, and 135 degrees around the image center.
			DF_ENABLE_MULTIPLE_SCANLINE_DIRECTIONS = 1u << 0u,
			/// Enables the search for barcodes that use inverted reflectance.
			DF_ENABLE_INVERTED_REFLECTANCE = 1u << 1u,
			/// Enables the detection of barcodes which are mirrored (e.g. when held up-side-down).
			DF_ENABLE_SCANLINE_MIRRORING = 1u << 2u,
			/// Enable the detection of multiple codes, otherwise the detection will stop after the first detected barcode.
			DF_ENABLE_MULTI_CODE_DETECTION = 1u << 3u,
			/// Enable the detection of duplicate codes; this will also enable the detection of multiple codes.
			DF_ENABLE_MULTI_CODE_DETECTION_WITH_DUPLICATES = 1u << 4u | DF_ENABLE_MULTI_CODE_DETECTION,
			/// Enable all of the available extra features.
			DF_ENABLE_EVERYTHING = 0xFFFFFFFFu
		};

		/**
		 * Definition of an observation of a barcode in 2D.
		 */
		class OCEAN_CV_DETECTOR_BARCODES_EXPORT Observation
		{
			public:

				/**
				 * Creates an invalid observation.
				 */
				Observation() = default;

				/**
				 * Create an observation from points.
				 */
				Observation(const Vector2& startPoint, const Vector2& endPoint);

				/**
				 * Returns the location of the observation.
				 * @return The location of the observation.
				 */
				const FiniteLine2& location() const;

			protected:

				/// The location of the observation.
				FiniteLine2 location_;
		};

		/// Definition of a vector of observations.
		typedef std::vector<Observation> Observations;

		/// Definition of a function pointer for parser functions which detect the actual barcodes.
		typedef bool (*ParserFunction)(const uint32_t* segmentData, const size_t size, Barcode& barcode, IndexPair32& xCoordinates);

		/// Definition of a set of parser functions.
		typedef std::unordered_set<ParserFunction> ParserFunctionSet;

	public:

		/**
		 * Detects barcodes in an 8-bit grayscale image.
		 * @param yFrame The frame in which barcodes will be detected, must be valid, have its origin in the upper left corner, and have a pixel format that is compatible with Y8, minimum size is 70 x 70 pixels.
		 * @param detectionFeatures Optional flag to enable certain additional detection features.
		 * @param enabledBarcodeTypes A set of barcode types that will be detected; if empty, every supported barcode will be detected.
		 * @param scanlineSpacing The spacing between parallel scan lines in pixels, range: [1, infinity).
		 * @param observations Optional observations of the detected barcodes that will be returned, will be ignored if `nullptr`.
		 * @param scanlines Optionally resulting scan lines that were used during the detection, will be ignored if `nullptr`.
		 * @return The list of detected barcodes.
		 */
		static Barcodes detectBarcodes(const Frame& yFrame, const uint32_t detectionFeatures = DF_STANDARD, const BarcodeTypeSet& enabledBarcodeTypes = BarcodeTypeSet(), const unsigned int scanlineSpacing = 25u, Observations* observations = nullptr, FiniteLines2* scanlines = nullptr);

	protected:

		/**
		 * Computes a vector pointing at a specific angle on a unit circle.
		 * @param angle The angle on the unit circle for which a corresponding vector is computed, range: [0, 2*PI].
		 * @param length The length that the resulting vector will have, range: (0, infinity).
		 * @return The vector
		 */
		static Vector2 computeDirectionVector(const Scalar angle, const Scalar length = Scalar(1));

		/**
		 * Computes the intersection points of a frame and an intersecting infinite line.
		 * @param frameWidth The width of the frame that is intersected by the infinite line, range: [1, infinity).
		 * @param frameHeight The height of the frame that is intersected by the infinite line, range: [1, infinity).
		 * @param frameBorder The border on the inside of the frame that should be enforced between the frame and the intersection points, range: [0, min(frameWidth, frameHeight) / 2).
		 * @param line The infinite line to intersect with the frame, must be valid.
		 * @param point0 The resulting first intersection point.
		 * @param point1 The resulting second intersection point.
		 * @return True if an intersection has been found, otherwise false.
		 */
		static bool computeFrameIntersection(const unsigned int frameWidth, const unsigned frameHeight, const unsigned int frameBorder, const Line2& line, CV::PixelPositionI& point0, CV::PixelPositionI& point1);

		/**
		 * Computes the locations of the scan lines for a given direction.
		 * The first scan line will intersect the frame center. All other scan lines will then be added alternatingly above and below the first scan line with increasing distance (`scanlineSpacing`) until they are outside the frame or below a minimum size.
		 * @param frameWidth The width of the frame that is intersected by the infinite line, range: [1, infinity).
		 * @param frameHeight The height of the frame that is intersected by the infinite line, range: [1, infinity).
		 * @param scanlineDirection The direction for which scan lines should be extracted, must be valid.
		 * @param scanlineSpacing The spacing between parallel scan lines in pixels, range: [1, infinity).
		 * @param frameBorder The border on the inside of the frame that should be enforced between the frame and the intersection points, range: [0, min(frameWidth, frameHeight) / 2).
		 * @param minimumScanlineLength The minimum length of scan lines that will be accepted, range: [1, infinity).
		 * @return The locations of the scan lines in the image, each defined by its end points in pixel coordinates.
		 */
		static FiniteLines2 computeScanlines(const unsigned int frameWidth, const unsigned frameHeight, const Vector2& scanlineDirection, const unsigned int scanlineSpacing, const unsigned int frameBorder, const unsigned int minimumScanlineLength);

		/**
		 * Extracts the data of scan line specified by two points.
		 * Uses the Bresenham algorithm to extract the data between two points (scan line).
		 * @param yFrame The frame from which a scan line will be extracted, must be valid, have its origin in the upper left corner, and have a pixel format that is compatible with Y8.
		 * @param scanline The scan line for which image data will be extracted, must be inside the image boundary.
		 * @param scanlineData The resulting scan line data; it is suggested to reserve its memory before calling this function.
		 * @param scanlinePositions The resulting pixel positions of the elements of the scan line, will have the same size as `scanline`.
		 * @param minimumScanlineLength An optional minimum value of the size of the scan line; scan lines with fewer elements will be discarded and the function will return false; will be ignored if set to 0.
		 * @return True if a scan line has been successfully extracted and it has at least the minimum number of elements, otherwise false.
		 */
		static bool extractScanlineData(const Frame& yFrame, const FiniteLine2& scanline, ScanlineData& scanlineData, CV::PixelPositionsI& scanlinePositions, const unsigned int minimumScanlineLength = 0u);

		/**
		 * Checks if a given pixel is a foreground pixel.
		 * @param pixelValue The pixel value that will be checked.
		 * @param grayThreshold The value of the gray threshold that is used to determine if the pixel is a foreground pixel.
		 * @return True if the pixel a is a foreground pixel, otherwise false.
		 * @tparam tIsNormalReflectance Indicates whether to consider foreground for normal or inverted reflectance.
		 */
		template <bool tIsNormalReflectance>
		static bool isForegroundPixel(const uint8_t pixelValue, const uint8_t grayThreshold);

		/**
		 * Returns the set of all available parser function pointers.
		 * @return The set of all available parser function pointers.
		 */
		static ParserFunctionSet getParserFunctions(const BarcodeTypeSet& barcodeTypeSet);
};

template <typename TPixel>
const SegmentData& RowSegmenter<TPixel>::segmentData() const
{
	return segmentData_;
}

template <typename TPixel>
size_t RowSegmenter<TPixel>::size() const
{
	return size_;
}

template <typename TPixel>
size_t RowSegmenter<TPixel>::position() const
{
	return position_;
}

template <typename TPixel>
bool RowSegmenter<TPixel>::setPosition(const size_t position)
{
	if (position >= size_)
	{
		ocean_assert(false && "Invalid position value");
		return false;
	}

	position_ = position;
	segmentPosition_ = position_;
	transitionHistory_.reset();

	return true;
}

template <typename TPixel>
bool RowSegmenter<TPixel>::isTransitionLightToDark(const TPixel* pixel, const TGradient gradientThreshold, TransitionHistory& history)
{
	ocean_assert(pixel != nullptr);
	ocean_assert(gradientThreshold > TGradient(0));

	const TGradient gradient = TGradient(*pixel) - TGradient(*(pixel - 1));

	bool isTransition = false;

	if (gradient < -gradientThreshold)
	{
		isTransition = true;
	}
	else
	{
		if (gradient + history.history1() < -gradientThreshold ||
			gradient + history.history2() < -(gradientThreshold * 5 / 4)||
			gradient + history.history3() < -(gradientThreshold * 6 / 4))
		{
			isTransition = true;
		}
	}

	history.push(gradient);

	return isTransition;
}

template <typename TPixel>
bool RowSegmenter<TPixel>::isTransitionDarkToLight(const TPixel* pixel, const TGradient gradientThreshold, TransitionHistory& history)
{
	ocean_assert(pixel != nullptr);
	ocean_assert(gradientThreshold > TGradient(0));

	const TGradient gradient = TGradient(*pixel) - TGradient(*(pixel - 1));

	bool isTransition = false;

	if (gradient > gradientThreshold)
	{
		isTransition = true;
	}
	else
	{
		if (gradient + history.history1() > gradientThreshold ||
			gradient + history.history2() > (gradientThreshold * 5 / 4)||
			gradient + history.history3() > (gradientThreshold * 6 / 4))
		{
			isTransition = true;
		}
	}

	history.push(gradient);

	return isTransition;
}

} // namespace Barcodes

} // namespace Detector

} // namespace CV

} // namespace Ocean
