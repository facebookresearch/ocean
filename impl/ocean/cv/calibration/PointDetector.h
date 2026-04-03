/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_CALIBRATION_POINT_DETECTOR_H
#define META_OCEAN_CV_CALIBRATION_POINT_DETECTOR_H

#include "ocean/cv/calibration/Calibration.h"
#include "ocean/cv/calibration/Point.h"

#include "ocean/base/Frame.h"
#include "ocean/base/StaticVector.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/NonMaximumSuppression.h"
#include "ocean/cv/PixelPosition.h"

#include "ocean/geometry/SpatialDistribution.h"

namespace Ocean
{

namespace CV
{

namespace Calibration
{

/**
 * This class implements a point detector for marker points.
 * The detector identifies both dark points on bright backgrounds and bright points on dark backgrounds.<br>
 * It uses a border-based filter that compares the intensity of a center pixel with the intensities of surrounding border pixels.<br>
 * The filter response is the sum of clamped squared differences between center and border pixels.<br>
 * Detected candidates undergo non-maximum suppression followed by sub-pixel refinement.
 * @ingroup cvcalibration
 */
class OCEAN_CV_CALIBRATION_EXPORT PointDetector
{
	friend class CalibrationDebugElements;

	public:

		/**
		 * Definition of a pair combining an index with a distance.
		 */
		using IndexDistancePair = std::pair<Index32, Scalar>;

		/**
		 * Definition of a static vector holding IndexDistancePair objects.
		 * @tparam tNumber The number of elements the vector can hold at most, with range [1, infinity)
		 */
		template <unsigned int tNumber>
		using IndexDistancePairs = StaticVector<IndexDistancePair, tNumber>;

		/**
		 * Definition of the border shape of the detection filter.
		 */
		enum BorderShape : uint32_t
		{
			/// Invalid border shape.
			BS_INVALID = 0u,
			/// Square-shaped border (all pixels along the perimeter of a square).
			BS_SQUARE,
			/// Circle-shaped border (all pixels on a Bresenham circle).
			BS_CIRCLE
		};

	protected:

		/**
		 * This class manages the border pixel offsets used by the point detection filter.
		 * The offsets define the relative positions of the border pixels around a center pixel.<br>
		 * Offsets are cached and reused as long as the filter parameters and frame layout remain unchanged.
		 */
		class PointBorderOffsets
		{
			public:

				/**
				 * Updates the border offsets for a given filter configuration and frame layout.
				 * If the parameters match the cached state, no recalculation is performed.
				 * @param filterSize The size of the filter, in pixels, must be odd, with range [3, infinity)
				 * @param borderShape The shape of the border, must not be BS_INVALID
				 * @param width The width of the frame in which the filter will be applied, in pixels, with range [filterSize, infinity)
				 * @param paddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
				 * @return True, if the offsets were successfully updated or already up to date
				 */
				bool update(const unsigned int filterSize, const BorderShape borderShape, const unsigned int width, const unsigned int paddingElements);

				/**
				 * Returns the 1D border offsets for the current filter configuration.
				 * @return The border offsets, empty if not yet initialized
				 */
				inline const Indices32& offsets() const;

				/**
				 * Determines the 2D border offsets for a given filter size and border shape.
				 * The resulting offsets are relative to the center pixel of the filter.
				 * @param filterSize The size of the filter, in pixels, must be odd, with range [3, infinity)
				 * @param borderShape The shape of the border, must not be BS_INVALID
				 * @param borderOffsets The resulting 2D border offsets, must be empty
				 * @return True, if succeeded
				 */
				static bool determineBorderOffsets(const unsigned int filterSize, const BorderShape borderShape, CV::PixelPositionsI& borderOffsets);

				/**
				 * Determines the 1D border offsets for a given filter size, border shape, and frame stride.
				 * The first offset is the negative offset from the center pixel to the top-left starting position.<br>
				 * All subsequent offsets are positive, each relative to the previous position.
				 * @param strideElements The stride of the frame, in elements (width + padding), with range [filterSize, infinity)
				 * @param filterSize The size of the filter, in pixels, must be odd, with range [3, infinity)
				 * @param borderShape The shape of the border, must not be BS_INVALID
				 * @param unsignedBorderOffsets The resulting 1D border offsets, must be empty
				 * @return True, if succeeded
				 */
				static bool determineBorderOffsets(const unsigned int strideElements, const unsigned int filterSize, const BorderShape borderShape, Indices32& unsignedBorderOffsets);

			protected:

				/// The cached filter size, 0 if not yet initialized.
				unsigned int filterSize_ = 0u;

				/// The cached border shape.
				BorderShape borderShape_ = BS_INVALID;

				/// The cached frame width, in pixels.
				unsigned int width_ = 0u;

				/// The cached number of padding elements.
				unsigned int paddingElements_ = 0u;

				/// The border offsets for the current filter configuration.
				Indices32 offsets_;
		};

		/**
		 * This class implements a peak detector that refines the sub-pixel position of a detected point.
		 * The refinement uses iterative gradient-based optimization on the grayscale image.
		 */
		class PointPeakDetector
		{
			public:

				/**
				 * Creates a new peak detector for a given grayscale frame.
				 * @param yFrame The grayscale frame in which peaks will be refined, with pixel format FORMAT_Y8, must be valid and must exist as long as this object exists
				 */
				explicit PointPeakDetector(const Frame& yFrame);

				/**
				 * Determines the precise sub-pixel position of a detected point.
				 * @param x The horizontal pixel position of the detected point, with range [1, width - 2]
				 * @param y The vertical pixel position of the detected point, with range [1, height - 2]
				 * @param strength The integer strength of the detected point (positive for dark points, negative for bright points)
				 * @param preciseX The resulting precise horizontal position
				 * @param preciseY The resulting precise vertical position
				 * @param preciseStrength The resulting precise strength (currently set to the input strength)
				 * @return True, if succeeded
				 */
				bool determinePrecisePosition(const unsigned int x, const unsigned int y, const int32_t strength, Scalar& preciseX, Scalar& preciseY, int32_t& preciseStrength) const;

			protected:

				/// The grayscale frame used for peak refinement with pixel format FORMAT_Y8.
				const Frame& yFrame_;
		};

		/**
		 * Definition of the non-maximum suppression type used by this detector.
		 * Uses int32_t votes to support both positive (dark point) and negative (bright point) responses.
		 */
		using NonMaximumSuppressionVote = NonMaximumSuppressionT<int32_t>;

	public:

		/**
		 * Creates a new point detector.
		 */
		PointDetector() = default;

		/**
		 * Detects points in a new frame.
		 * Previously detected points will be replaced.
		 * @param yFrame The frame in which the points will be detected, with pixel format FORMAT_Y8, must be valid
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		bool detectPoints(const Frame& yFrame, Worker* worker = nullptr);

		/**
		 * Returns the points detected in the latest frame.
		 * @return The latest's frame points
		 */
		inline const Points& points() const;

		/**
		 * Returns the spatial distribution array of the points detected in the latest frame.
		 * @return The latest's frame points distribution array
		 */
		inline const Geometry::SpatialDistribution::DistributionArray& pointsDistributionArray() const;

		/**
		 * Detects points in a grayscale frame using a specified filter size and border shape.
		 * @param yFrame The grayscale frame in which the points will be detected, with pixel format FORMAT_Y8, must be valid
		 * @param filterSize The size of the detection filter, in pixels, must be odd, with range [3, infinity)
		 * @param points The resulting detected points
		 * @param borderShape The shape of the detection filter border
		 * @param minimalDifference The minimal intensity difference between the center pixel and each border pixel to accept a candidate, with range [1, infinity)
		 * @param maximalDifference The maximal intensity difference used for clamping the squared differences, with range [minimalDifference, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool detectPoints(const Frame& yFrame, const unsigned int filterSize, Points& points, const BorderShape borderShape, const int32_t minimalDifference = 25, const int32_t maximalDifference = 25 * 4, Worker* worker = nullptr);

		/**
		 * Detects points in a grayscale frame using pre-computed border offsets.
		 * @param yFrame The grayscale frame in which the points will be detected, with pixel format FORMAT_Y8, must be valid
		 * @param points The resulting detected points
		 * @param borderOffsets The pre-computed 1D border offsets, must be valid
		 * @param numberBorderOffsets The number of border offsets, with range [1, infinity)
		 * @param filterSize The size of the detection filter, in pixels, must be odd, with range [3, infinity)
		 * @param minimalDifference The minimal intensity difference between the center pixel and each border pixel to accept a candidate, with range [1, infinity)
		 * @param maximalDifference The maximal intensity difference used for clamping the squared differences, with range [minimalDifference, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool detectPoints(const Frame& yFrame, Points& points, const uint32_t* borderOffsets, const size_t numberBorderOffsets, const unsigned int filterSize, const int32_t minimalDifference = 25, const int32_t maximalDifference = 25 * 4, Worker* worker = nullptr);

		/**
		 * Creates a filter response frame for visualization or debugging.
		 * Each pixel in the response frame contains the signed filter response: positive for dark points, negative for bright points, zero for non-detections.
		 * @param yFrame The grayscale input frame, with pixel format FORMAT_Y8, must be valid
		 * @param responseFrame The resulting filter response frame with pixel format genericPixelFormat<int32_t, 1u>
		 * @param filterSize The size of the detection filter, in pixels, must be odd, with range [3, infinity)
		 * @param borderShape The shape of the detection filter border
		 * @param minimalDifference The minimal intensity difference between the center pixel and each border pixel, with range [1, infinity)
		 * @param maximalDifference The maximal intensity difference used for clamping the squared differences, with range [minimalDifference, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool createFilterResponseFrame(const Frame& yFrame, Frame& responseFrame, const unsigned int filterSize, const BorderShape borderShape, const int32_t minimalDifference = 25, const int32_t maximalDifference = 25 * 4, Worker* worker = nullptr);

		/**
		 * Returns the closest point to a given point.
		 * @param queryPoint The query point for which the closest point will be determined
		 * @param sign The sign of the point the closest point must have, true for dark points, false for bright points
		 * @param pointsDistributionArray The distribution array of the points to be used
		 * @param points The points from which the closest point will be determined
		 * @param maxSqrDistance The maximal square distance between the given point and the closest point, with range [0, infinity)
		 * @return The index of the closest point, -1 if no point could be found
		 */
		static size_t closestPoint(const Vector2& queryPoint, const bool sign, const Geometry::SpatialDistribution::DistributionArray& pointsDistributionArray, const Points& points, const Scalar maxSqrDistance);

		/**
		 * Returns the closest points to a given point.
		 * @param pointsDistributionArray The distribution array of the points to be used
		 * @param queryPointIndex The index of the query point for which the closest points will be determined, with range [0, points.size() - 1]
		 * @param points The points from which the closest points will be determined
		 * @param indexDistancePairs The resulting index/distance pairs of the closest points
		 * @param maxSqrDistance The maximal square distance between the given point and the closest point, with range [0, infinity)
		 * @tparam tNumber The number of closest points to be determined, with range [1, infinity)
		 * @tparam tMatchSign True, to determine closest points with same sign as the query point; False, to determine closest points with any sign
		 */
		template <unsigned int tNumber, bool tMatchSign>
		static void closestPoints(const Geometry::SpatialDistribution::DistributionArray& pointsDistributionArray, const size_t queryPointIndex, const Points& points, IndexDistancePairs<tNumber>& indexDistancePairs, const Scalar maxSqrDistance);

		/**
		 * Returns the two closest points to a given point.
		 * @param queryPoint The query point for which the closest points will be determined
		 * @param pointsDistributionArray The distribution array of the points to be used
		 * @param points The points from which the closest points will be determined
		 * @param closestPointIndex The resulting index of the closest point, with range [0, points.size() - 1], -1 if no point could be found
		 * @param secondClosestPointIndex The resulting index of the second closest point, with range [0, points.size() - 1], -1 if no second closest point could be found
		 * @param closestSqrDistance The resulting square distance between the query point and the closest point, with range [0, infinity)
		 * @param secondClosestSqrDistance The resulting square distance between the query point and the second closest point, with range [0, infinity)
		 * @return True, if at least one closest point could be found
		 */
		static bool closestPoints(const Vector2& queryPoint, const Geometry::SpatialDistribution::DistributionArray& pointsDistributionArray, const Points& points, Index32& closestPointIndex, Index32& secondClosestPointIndex, Scalar& closestSqrDistance, Scalar& secondClosestSqrDistance);

	protected:

		/**
		 * Detects point candidates in a subset of rows and adds them to a non-maximum suppression object.
		 * This is a worker function for parallel execution.
		 * @param yFrame The grayscale frame, with pixel format FORMAT_Y8, must be valid
		 * @param nonMaximumSuppression The non-maximum suppression object to which detected candidates will be added, must be valid
		 * @param borderOffsets The pre-computed 1D border offsets, must be valid
		 * @param numberBorderOffsets The number of border offsets, with range [1, infinity)
		 * @param filterSize The size of the detection filter, in pixels, must be odd, with range [3, infinity)
		 * @param minimalDifference The minimal intensity difference between the center pixel and each border pixel, with range [1, infinity)
		 * @param maximalDifference The maximal intensity difference used for clamping, with range [minimalDifference, infinity)
		 * @param firstRow The first row to process, with range [filterSize / 2, height - filterSize / 2)
		 * @param numberRows The number of rows to process, with range [1, height - filterSize / 2 - firstRow]
		 */
		static void detectPointCandidatesSubset(const Frame* yFrame, NonMaximumSuppressionVote* nonMaximumSuppression, const uint32_t* borderOffsets, const size_t numberBorderOffsets, const unsigned int filterSize, const int32_t minimalDifference, const int32_t maximalDifference, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Creates filter responses for a subset of rows and writes them into the response frame.
		 * This is a worker function for parallel execution.
		 * @param yFrame The grayscale frame, with pixel format FORMAT_Y8, must be valid
		 * @param responseFrame The response frame with pixel format genericPixelFormat<int32_t, 1u>, must be valid
		 * @param borderOffsets The pre-computed 1D border offsets, must be valid
		 * @param numberBorderOffsets The number of border offsets, with range [1, infinity)
		 * @param filterSize The size of the detection filter, in pixels, must be odd, with range [3, infinity)
		 * @param minimalDifference The minimal intensity difference between the center pixel and each border pixel, with range [1, infinity)
		 * @param maximalDifference The maximal intensity difference used for clamping, with range [minimalDifference, infinity)
		 * @param firstRow The first row to process, with range [0, height)
		 * @param numberRows The number of rows to process, with range [1, height - firstRow]
		 */
		static void createFilterResponseFrameSubset(const Frame* yFrame, Frame* responseFrame, const uint32_t* borderOffsets, const size_t numberBorderOffsets, const unsigned int filterSize, const int32_t minimalDifference, const int32_t maximalDifference, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Determines point candidates for a single row (single-sign: dark points only).
		 * @param y The row index, with range [filterSize / 2, height - filterSize / 2)
		 * @param yRow Pointer to the beginning of the row in the grayscale frame, must be valid
		 * @param nonMaximumSuppression The non-maximum suppression object to which detected candidates will be added, must be valid
		 * @param borderOffsets The pre-computed 1D border offsets, must be valid
		 * @param numberBorderOffsets The number of border offsets, with range [1, infinity)
		 * @param filterSize The size of the detection filter, in pixels, must be odd, with range [3, infinity)
		 * @param minimalDifference The minimal intensity difference between the center pixel and each border pixel, with range [1, infinity)
		 * @param maximalDifference The maximal intensity difference used for clamping, with range [minimalDifference, infinity)
		 */
		static void determinePointCandidatesRow(const unsigned int y, const uint8_t* yRow, NonMaximumSuppressionVote* nonMaximumSuppression, const uint32_t* borderOffsets, const size_t numberBorderOffsets, const unsigned int filterSize, const int32_t minimalDifference, const int32_t maximalDifference);

		/**
		 * Determines point candidates for a single row (dual-sign: both dark and bright points).
		 * Dark points produce a positive response; bright points produce a negative response.
		 * @param y The row index, with range [filterSize / 2, height - filterSize / 2)
		 * @param yRow Pointer to the beginning of the row in the grayscale frame, must be valid
		 * @param nonMaximumSuppression The non-maximum suppression object to which detected candidates will be added, must be valid
		 * @param borderOffsets The pre-computed 1D border offsets, must be valid
		 * @param numberBorderOffsets The number of border offsets, with range [1, infinity)
		 * @param filterSize The size of the detection filter, in pixels, must be odd, with range [3, infinity)
		 * @param minimalDifference The minimal intensity difference between the center pixel and each border pixel, with range [1, infinity)
		 * @param maximalDifference The maximal intensity difference used for clamping, with range [minimalDifference, infinity)
		 */
		static void determinePointCandidatesRowDual(const unsigned int y, const uint8_t* yRow, NonMaximumSuppressionVote* nonMaximumSuppression, const uint32_t* borderOffsets, const size_t numberBorderOffsets, const unsigned int filterSize, const int32_t minimalDifference, const int32_t maximalDifference);

		/**
		 * Creates a filter response row (single-sign: dark points only).
		 * @param yRow Pointer to the beginning of the row in the grayscale frame, must be valid
		 * @param responseRow The resulting response row, must be valid
		 * @param width The width of the row, in pixels, with range [filterSize, infinity)
		 * @param borderOffsets The pre-computed 1D border offsets, must be valid
		 * @param numberBorderOffsets The number of border offsets, with range [1, infinity)
		 * @param filterSize The size of the detection filter, in pixels, must be odd, with range [3, infinity)
		 * @param minimalDifference The minimal intensity difference between the center pixel and each border pixel, with range [1, infinity)
		 * @param maximalDifference The maximal intensity difference used for clamping the squared differences, with range [minimalDifference, infinity)
		 */
		static void createFilterResponseRow(const uint8_t* yRow, int32_t* responseRow, const unsigned int width, const uint32_t* borderOffsets, const size_t numberBorderOffsets, const unsigned int filterSize, const int32_t minimalDifference, const int32_t maximalDifference);

		/**
		 * Creates a filter response row (dual-sign: both dark and bright points).
		 * Dark points produce a positive response; bright points produce a negative response.
		 * @param yRow Pointer to the beginning of the row in the grayscale frame, must be valid
		 * @param responseRow The resulting response row, must be valid
		 * @param width The width of the row, in pixels, with range [filterSize, infinity)
		 * @param borderOffsets The pre-computed 1D border offsets, must be valid
		 * @param numberBorderOffsets The number of border offsets, with range [1, infinity)
		 * @param filterSize The size of the detection filter, in pixels, must be odd, with range [3, infinity)
		 * @param minimalDifference The minimal intensity difference between the center pixel and each border pixel, with range [1, infinity)
		 * @param maximalDifference The maximal intensity difference used for clamping the squared differences, with range [minimalDifference, infinity)
		 */
		static void createFilterResponseRowDual(const uint8_t* yRow, int32_t* responseRow, const unsigned int width, const uint32_t* borderOffsets, const size_t numberBorderOffsets, const unsigned int filterSize, const int32_t minimalDifference, const int32_t maximalDifference);

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

		/**
		 * Creates a filter response row using SSE4.1 SIMD instructions (single-sign: dark points only).
		 * This is an optimized implementation that processes 8 pixels per iteration.
		 * @param yRow Pointer to the beginning of the row in the grayscale frame, must be valid
		 * @param responseRow The resulting response row, must be valid
		 * @param width The width of the row, in pixels, with range [filterSize / 2 + 8 + filterSize / 2, infinity)
		 * @param borderOffsets The pre-computed 1D border offsets, must be valid
		 * @param numberBorderOffsets The number of border offsets, with range [1, infinity)
		 * @param filterSize The size of the detection filter, in pixels, must be odd, with range [3, infinity)
		 * @param minimalDifference The minimal intensity difference between the center pixel and each border pixel, with range [1, infinity)
		 * @param maximalDifference The maximal intensity difference used for clamping the squared differences, with range [minimalDifference, infinity)
		 */
		static void createFilterResponseRowSSE(const uint8_t* yRow, int32_t* responseRow, const unsigned int width, const uint32_t* borderOffsets, const size_t numberBorderOffsets, const unsigned int filterSize, const int32_t minimalDifference, const int32_t maximalDifference);

		/**
		 * Creates a filter response row using SSE4.1 SIMD instructions (dual-sign: both dark and bright points).
		 * Detects both dark points on bright backgrounds (positive response) and bright points on dark backgrounds (negative response).
		 * This is an optimized implementation that processes 8 pixels per iteration.
		 * @param yRow Pointer to the beginning of the row in the grayscale frame, must be valid
		 * @param responseRow The resulting response row, must be valid
		 * @param width The width of the row, in pixels, with range [filterSize / 2 + 8 + filterSize / 2, infinity)
		 * @param borderOffsets The pre-computed 1D border offsets, must be valid
		 * @param numberBorderOffsets The number of border offsets, with range [1, infinity)
		 * @param filterSize The size of the detection filter, in pixels, must be odd, with range [3, infinity)
		 * @param minimalDifference The minimal intensity difference between the center pixel and each border pixel, with range [1, infinity)
		 * @param maximalDifference The maximal intensity difference used for clamping the squared differences, with range [minimalDifference, infinity)
		 */
		static void createFilterResponseRowSSEDual(const uint8_t* yRow, int32_t* responseRow, const unsigned int width, const uint32_t* borderOffsets, const size_t numberBorderOffsets, const unsigned int filterSize, const int32_t minimalDifference, const int32_t maximalDifference);

		/**
		 * Determines point candidates for a single row using SSE4.1 SIMD instructions (single-sign: dark points only).
		 * This is an optimized implementation that processes 8 pixels per iteration.
		 * @param y The row index, with range [filterSize / 2, height - filterSize / 2)
		 * @param yRow Pointer to the beginning of the row in the grayscale frame, must be valid
		 * @param nonMaximumSuppression The non-maximum suppression object to which detected candidates will be added, must be valid
		 * @param borderOffsets The pre-computed 1D border offsets, must be valid
		 * @param numberBorderOffsets The number of border offsets, with range [1, infinity)
		 * @param filterSize The size of the detection filter, in pixels, must be odd, with range [3, infinity)
		 * @param minimalDifference The minimal intensity difference between the center pixel and each border pixel, with range [1, infinity)
		 * @param maximalDifference The maximal intensity difference used for clamping the squared differences, with range [minimalDifference, infinity)
		 */
		static void determinePointCandidatesRowSSE(const unsigned int y, const uint8_t* yRow, NonMaximumSuppressionVote* nonMaximumSuppression, const uint32_t* borderOffsets, const size_t numberBorderOffsets, const unsigned int filterSize, const int32_t minimalDifference, const int32_t maximalDifference);

		/**
		 * Determines point candidates for a single row using SSE4.1 SIMD instructions (dual-sign: both dark and bright points).
		 * This is an optimized implementation that processes 8 pixels per iteration.
		 * @param y The row index, with range [filterSize / 2, height - filterSize / 2)
		 * @param yRow Pointer to the beginning of the row in the grayscale frame, must be valid
		 * @param nonMaximumSuppression The non-maximum suppression object to which detected candidates will be added, must be valid
		 * @param borderOffsets The pre-computed 1D border offsets, must be valid
		 * @param numberBorderOffsets The number of border offsets, with range [1, infinity)
		 * @param filterSize The size of the detection filter, in pixels, must be odd, with range [3, infinity)
		 * @param minimalDifference The minimal intensity difference between the center pixel and each border pixel, with range [1, infinity)
		 * @param maximalDifference The maximal intensity difference used for clamping the squared differences, with range [minimalDifference, infinity)
		 */
		static void determinePointCandidatesRowSSEDual(const unsigned int y, const uint8_t* yRow, NonMaximumSuppressionVote* nonMaximumSuppression, const uint32_t* borderOffsets, const size_t numberBorderOffsets, const unsigned int filterSize, const int32_t minimalDifference, const int32_t maximalDifference);

#endif // OCEAN_HARDWARE_SSE_VERSION >= 41

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

		/**
		 * Creates a filter response row using NEON SIMD instructions (single-sign: dark points only).
		 * This is an optimized implementation that processes 8 pixels per iteration.
		 * @param yRow Pointer to the beginning of the row in the grayscale frame, must be valid
		 * @param responseRow The resulting response row, must be valid
		 * @param width The width of the row, in pixels, with range [filterSize / 2 + 8 + filterSize / 2, infinity)
		 * @param borderOffsets The pre-computed 1D border offsets, must be valid
		 * @param numberBorderOffsets The number of border offsets, with range [1, infinity)
		 * @param filterSize The size of the detection filter, in pixels, must be odd, with range [3, infinity)
		 * @param minimalDifference The minimal intensity difference between the center pixel and each border pixel, with range [1, infinity)
		 * @param maximalDifference The maximal intensity difference used for clamping the squared differences, with range [minimalDifference, infinity)
		 */
		static void createFilterResponseRowNEON(const uint8_t* yRow, int32_t* responseRow, const unsigned int width, const uint32_t* borderOffsets, const size_t numberBorderOffsets, const unsigned int filterSize, const int32_t minimalDifference, const int32_t maximalDifference);

		/**
		 * Creates a filter response row using NEON SIMD instructions (dual-sign: both dark and bright points).
		 * Detects both dark points on bright backgrounds (positive response) and bright points on dark backgrounds (negative response).
		 * This is an optimized implementation that processes 8 pixels per iteration.
		 * @param yRow Pointer to the beginning of the row in the grayscale frame, must be valid
		 * @param responseRow The resulting response row, must be valid
		 * @param width The width of the row, in pixels, with range [filterSize / 2 + 8 + filterSize / 2, infinity)
		 * @param borderOffsets The pre-computed 1D border offsets, must be valid
		 * @param numberBorderOffsets The number of border offsets, with range [1, infinity)
		 * @param filterSize The size of the detection filter, in pixels, must be odd, with range [3, infinity)
		 * @param minimalDifference The minimal intensity difference between the center pixel and each border pixel, with range [1, infinity)
		 * @param maximalDifference The maximal intensity difference used for clamping the squared differences, with range [minimalDifference, infinity)
		 */
		static void createFilterResponseRowNEONDual(const uint8_t* yRow, int32_t* responseRow, const unsigned int width, const uint32_t* borderOffsets, const size_t numberBorderOffsets, const unsigned int filterSize, const int32_t minimalDifference, const int32_t maximalDifference);

		/**
		 * Determines point candidates for a single row using NEON SIMD instructions (single-sign: dark points only).
		 * This is an optimized implementation that processes 8 pixels per iteration.
		 * @param y The row index, with range [filterSize / 2, height - filterSize / 2)
		 * @param yRow Pointer to the beginning of the row in the grayscale frame, must be valid
		 * @param nonMaximumSuppression The non-maximum suppression object to which detected candidates will be added, must be valid
		 * @param borderOffsets The pre-computed 1D border offsets, must be valid
		 * @param numberBorderOffsets The number of border offsets, with range [1, infinity)
		 * @param filterSize The size of the detection filter, in pixels, must be odd, with range [3, infinity)
		 * @param minimalDifference The minimal intensity difference between the center pixel and each border pixel, with range [1, infinity)
		 * @param maximalDifference The maximal intensity difference used for clamping the squared differences, with range [minimalDifference, infinity)
		 */
		static void determinePointCandidatesRowNEON(const unsigned int y, const uint8_t* yRow, NonMaximumSuppressionVote* nonMaximumSuppression, const uint32_t* borderOffsets, const size_t numberBorderOffsets, const unsigned int filterSize, const int32_t minimalDifference, const int32_t maximalDifference);

		/**
		 * Determines point candidates for a single row using NEON SIMD instructions (dual-sign: both dark and bright points).
		 * This is an optimized implementation that processes 8 pixels per iteration.
		 * @param y The row index, with range [filterSize / 2, height - filterSize / 2)
		 * @param yRow Pointer to the beginning of the row in the grayscale frame, must be valid
		 * @param nonMaximumSuppression The non-maximum suppression object to which detected candidates will be added, must be valid
		 * @param borderOffsets The pre-computed 1D border offsets, must be valid
		 * @param numberBorderOffsets The number of border offsets, with range [1, infinity)
		 * @param filterSize The size of the detection filter, in pixels, must be odd, with range [3, infinity)
		 * @param minimalDifference The minimal intensity difference between the center pixel and each border pixel, with range [1, infinity)
		 * @param maximalDifference The maximal intensity difference used for clamping the squared differences, with range [minimalDifference, infinity)
		 */
		static void determinePointCandidatesRowNEONDual(const unsigned int y, const uint8_t* yRow, NonMaximumSuppressionVote* nonMaximumSuppression, const uint32_t* borderOffsets, const size_t numberBorderOffsets, const unsigned int filterSize, const int32_t minimalDifference, const int32_t maximalDifference);

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

	protected:

		/**
		 * Removes duplicated points from a given set of points.
		 * @param width The width of the frame in which the points have been detected, in pixel, with range [1, infinity)
		 * @param height The height of the frame in which the points have been detected, in pixel, with range [1, infinity)
		 * @param points The points from which the duplicated points will be removed
		 * @param maxDistance The maximal distance between two points to be considered as duplicated, with range [0, infinity)
		 */
		static void removeDuplicatedPoints(const unsigned int width, const unsigned int height, Points& points, const Scalar maxDistance);

		/**
		 * Returns whether a query point is close to another point.
		 * @param queryPoint The query point to check
		 * @param pointsDistributionArray The distribution array of the points to be used
		 * @param points The points from which the closest point will be determined
		 * @param maxSqrDistance The maximal square distance between the given point and the closest point, with range [0, infinity)
		 * @return True, if so
		 */
		static bool hasClosePoint(const Vector2& queryPoint, const Geometry::SpatialDistribution::DistributionArray& pointsDistributionArray, const Points& points, const Scalar maxSqrDistance);

		/**
		 * Returns the closest points to a query point.
		 * @param queryPoint The query point for which the closest points will be determined
		 * @param pointsDistributionArray The distribution array of the points to be used
		 * @param points The points from which the closest point will be determined
		 * @param maxSqrDistance The maximal square distance between the given point and the closest point, with range [0, infinity)
		 * @param pointIndices The resulting indices of the closest points
		 * @return True, if at least one closest point could be found
		 */
		static bool closestPoints(const Vector2& queryPoint, const Geometry::SpatialDistribution::DistributionArray& pointsDistributionArray, const Points& points, const Scalar maxSqrDistance, Indices32& pointIndices);

	protected:

		/// The size of the detection filter, in pixels, must be odd, with range [3, infinity).
		unsigned int filterSize_ = 7u;

		/// The minimal intensity difference between the center pixel and each border pixel, with range [1, infinity).
		int32_t minimalDifference_ = 25;

		/// The maximal intensity difference used for clamping the squared differences, with range [minimalDifference_, infinity).
		int32_t maximalDifference_ = 25 * 4;

		/// The cached border offsets for the current filter configuration.
		PointBorderOffsets pointBorderOffsets_;

		/// The maximal distance between two points to be considered as duplicated, with range [0, infinity).
		Scalar maxDistanceBetweenDuplicatePoints_ = Scalar(2);

		/// The precise points detected in the latest frame.
		Points points_;

		/// The spatial distribution array of the points detected in the latest frame.
		Geometry::SpatialDistribution::DistributionArray pointsDistributionArray_;
};

inline const Indices32& PointDetector::PointBorderOffsets::offsets() const
{
	return offsets_;
}

template <unsigned int tNumber, bool tMatchSign>
void PointDetector::closestPoints(const Geometry::SpatialDistribution::DistributionArray& pointsDistributionArray, const size_t queryPointIndex, const Points& points, IndexDistancePairs<tNumber>& indexDistancePairs, const Scalar maxSqrDistance)
{
	static_assert(tNumber >= 1u, "Invalid number of points!");

	ocean_assert(indexDistancePairs.empty());
	ocean_assert(queryPointIndex < points.size());

	const Vector2& point = points[queryPointIndex].observation();

	const unsigned int xBinCenter = pointsDistributionArray.horizontalBin(point.x());
	const unsigned int yBinCenter = pointsDistributionArray.verticalBin(point.y());

	for (unsigned int xBin = pointsDistributionArray.beginBinHorizontal<1u>(xBinCenter); xBin < pointsDistributionArray.endBinHorizontal<1u>(xBinCenter); ++xBin)
	{
		for (unsigned int yBin = pointsDistributionArray.beginBinVertical<1u>(yBinCenter); yBin < pointsDistributionArray.endBinVertical<1u>(yBinCenter); ++yBin)
		{
			const Indices32& indices = pointsDistributionArray(xBin, yBin);

			for (const Index32& index : indices)
			{
				if (index == Index32(queryPointIndex))
				{
					continue;
				}

				if constexpr (tMatchSign)
				{
					if (points[queryPointIndex].sign() != points[index].sign())
					{
						continue;
					}
				}

				const Scalar sqrDistance = point.sqrDistance(points[index].observation());

				if (sqrDistance > maxSqrDistance)
				{
					continue;
				}

				bool inserted = false;

				if (!indexDistancePairs.empty() && sqrDistance < indexDistancePairs.back().second)
				{
					for (size_t nCandidate = 0; nCandidate < indexDistancePairs.size(); ++nCandidate)
					{
						if (sqrDistance < indexDistancePairs[nCandidate].second)
						{
							if (indexDistancePairs.size() != tNumber)
							{
								indexDistancePairs.weakResize(indexDistancePairs.size() + 1);
							}

							for (size_t n = indexDistancePairs.size() - 1; n > nCandidate; --n)
							{
								indexDistancePairs[n] = indexDistancePairs[n - 1];
							}

							indexDistancePairs[nCandidate] = IndexDistancePair(index, sqrDistance);

							inserted = true;
							break;

						}
					}
				}

				if (!inserted && indexDistancePairs.size() < tNumber)
				{
					indexDistancePairs.pushBack(IndexDistancePair(index, sqrDistance));
				}
			}
		}
	}
}

inline const Points& PointDetector::points() const
{
	return points_;
}

inline const Geometry::SpatialDistribution::DistributionArray& PointDetector::pointsDistributionArray() const
{
	return pointsDistributionArray_;
}

}

}

}

#endif // META_OCEAN_CV_CALIBRATION_POINT_DETECTOR_H
