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

#include "ocean/math/Variance.h"

namespace Ocean
{

namespace CV
{

namespace Calibration
{

/**
 * This class implements a point detector for marker points.
 * @ingroup cvcalibration
 */
class OCEAN_CV_CALIBRATION_EXPORT PointDetector
{
	friend class CalibrationDebugElements;

	public:

		/**
		 * This class implements a pattern for fast point detection.
		 * The pattern is defined by a radius and an inner radius defining a ring area around the center pixel.<br>
		 * Points can be detected by comparing the color intensity of the center pixel with the color intensities of the surrounding pixels in the ring area.
		 */
		class PointPattern
		{
			public:

				/**
				 * Creates a new point pattern with same properties as a given point pattern but with different frame stride.
				 * @param pointPattern The point pattern to be copied, must be valid
				 * @param frameStrideElements The stride of the frame in which the point pattern will be used, in elements, with range [frame.width(), infinity)
				 */
				PointPattern(const PointPattern& pointPattern, const unsigned int frameStrideElements);

				/**
				 * Creates a new point pattern.
				 * @param radius The radius of the point pattern, with range [1, infinity)
				 * @param innerRadius The innerRadius defining a circular area around the center pixel where pixel data is excluded from use, with range [0, radius - 1]
				 * @param frameStrideElements The stride of the frame in which the point pattern is used, in elements, with range [frame
				 * @param useCircle True, if the outer shape of the point pattern is a circle; False, if the outer shape is a rectangle
				 */
				PointPattern(const unsigned int radius, const unsigned int innerRadius, const unsigned int frameStrideElements, const bool useCircle = true);

				/**
				 * Returns the radius of the point this pattern is able to detect.
				 * @return The point's radius, in pixel, with range [1, infinity)
				 */
				inline unsigned int radius() const;

				/**
				 * Returns the diameter of the point this pattern is able to detect.
				 * @return The point's diameter, in pixel, with range [3, infinity)
				 */
				inline unsigned int diameter() const;

				/**
				 * Returns the inner radius of this pattern.
				 * @return The pattern's inner radius, in pixel, with range [0, radius - 1]
				 */
				inline unsigned int innerRadius() const;

				/**
				 * Returns the stride of the frame which which this pattern has been created.
				 * @return The pattern's stride, in elements, with range [frame.width(), infinity)
				 */
				inline unsigned int frameStrideElements() const;

				/**
				 * Returns whether the outer shape of this pattern is a circle or a rectangle.
				 * @return True, if the outer shape is a circle; False, if the outer shape is a rectangle
				 */
				inline bool isCircle() const;

				/**
				 * Returns the negative offset for the most top-left pixel of the surrounding pixels.
				 * @return The pattern's negative offset, with range [0, infinity), needs to be negated
				 */
				inline unsigned int negativeOffset() const;

				/**
				 * Returns the positive offsets for all surrounding pixels starting at the top-left pixel, all in relation to the previous pixel.
				 * @return The pattern's positive offsets
				 */
				inline const Indices32& positiveOffsets() const;

				/**
				 * Returns the 2D offsets for all surrounding pixels.
				 * @return The pattern's offsets for surrounding pixels
				 */
				inline const CV::PixelPositionsI& offsets() const;

				/**
				 * Returns the normalized strength of a determined strength for this pattern.
				 * The strength is normalized due to the number of pixels this pattern uses.
				 * @param strength The strength to be normalized, with range [0, infinity)
				 * @return The normalized strength
				 */
				inline float normalizedStrength(const unsigned int strength) const;

				/**
				 * Determines the strength of a dark point candidate.
				 * A dark point has a dark center pixel surrounded by brighter pixels.
				 * @param centerPixelValue The color intensity of the center pixel, with range [0, 255]
				 * @param firstSurroundingPixel Pointer to the first surrounding pixel (top-left of the pattern), must be valid
				 * @param minDifference The minimal color intensity difference between the center pixel and the surrounding pixels, with range [0, 255]
				 * @param maxVariance The maximal variance of the color intensities of the surrounding pixels, 0 to disable checking the variance
				 * @return The strength of the point as sum of squared differences, 0 if the point is not a valid dark point
				 * @tparam tMaxCenterColorFixed The maximal color intensity of the center pixel to be considered as a dark point, with range [0, 255]
				 * @tparam tMinSurroundingColorFixed The minimal color intensity of the surrounding pixels, with range [0, tMaxCenterColorFixed)
				 */
				template <uint8_t tMaxCenterColorFixed, uint8_t tMinSurroundingColorFixed>
				inline uint32_t determineDarkPointStrength(const uint8_t centerPixelValue, const uint8_t* firstSurroundingPixel, const unsigned int minDifference, const unsigned int maxVariance = 0u) const;

				/**
				 * Determines the strength of a dark point candidate.
				 * A dark point has a dark center pixel surrounded by brighter pixels.
				 * @param yPoint Pointer to the center pixel in the frame, must be valid
				 * @param minDifference The minimal color intensity difference between the center pixel and the surrounding pixels, with range [0, 255]
				 * @param maxVariance The maximal variance of the color intensities of the surrounding pixels, 0 to disable checking the variance
				 * @return The strength of the point as sum of squared differences, 0 if the point is not a valid dark point
				 * @tparam tMaxCenterColorFixed The maximal color intensity of the center pixel to be considered as a dark point, with range [0, 255]
				 * @tparam tMinSurroundingColorFixed The minimal color intensity of the surrounding pixels, with range [0, tMaxCenterColorFixed)
				 */
				template <uint8_t tMaxCenterColorFixed, uint8_t tMinSurroundingColorFixed>
				inline uint32_t determineDarkPointStrength(const uint8_t* yPoint, const unsigned int minDifference, const unsigned int maxVariance = 0u) const;

				/**
				 * Determines the strength of a bright point candidate.
				 * A bright point has a bright center pixel surrounded by darker pixels.
				 * @param centerPixelValue The color intensity of the center pixel, with range [0, 255]
				 * @param firstSurroundingPixel Pointer to the first surrounding pixel (top-left of the pattern), must be valid
				 * @param minDifference The minimal color intensity difference between the center pixel and the surrounding pixels, with range [0, 255]
				 * @param maxVariance The maximal variance of the color intensities of the surrounding pixels, 0 to disable checking the variance
				 * @return The strength of the point as sum of squared differences, 0 if the point is not a valid bright point
				 * @tparam tMinCenterColorFixed The minimal color intensity of the center pixel to be considered as a bright point, with range [0, 255]
				 * @tparam tMaxSurroundingColorFixed The maximal color intensity of the surrounding pixels, with range (tMinCenterColorFixed, 255]
				 */
				template <uint8_t tMinCenterColorFixed, uint8_t tMaxSurroundingColorFixed>
				inline uint32_t determineBrightPointStrength(const uint8_t centerPixelValue, const uint8_t* firstSurroundingPixel, const unsigned int minDifference, const unsigned int maxVariance = 0u) const;

				/**
				 * Determines the strength of a bright point candidate.
				 * A bright point has a bright center pixel surrounded by darker pixels.
				 * @param yPoint Pointer to the center pixel in the frame, must be valid
				 * @param minDifference The minimal color intensity difference between the center pixel and the surrounding pixels, with range [0, 255]
				 * @param maxVariance The maximal variance of the color intensities of the surrounding pixels, 0 to disable checking the variance
				 * @return The strength of the point as sum of squared differences, 0 if the point is not a valid bright point
				 * @tparam tMinCenterColorFixed The minimal color intensity of the center pixel to be considered as a bright point, with range [0, 255]
				 * @tparam tMaxSurroundingColorFixed The maximal color intensity of the surrounding pixels, with range (tMinCenterColorFixed, 255]
				 */
				template <uint8_t tMinCenterColorFixed, uint8_t tMaxSurroundingColorFixed>
				inline uint32_t determineBrightPointStrength(const uint8_t* yPoint, const unsigned int minDifference, const unsigned int maxVariance = 0u) const;

				/**
				 * Determines the strength of a point at a sub-pixel position using bilinear interpolation.
				 * The strength is signed: positive for dark points (dark center, bright surrounding), negative for bright points.
				 * @param yFrame The frame in which the point strength will be determined, with pixel format FORMAT_Y8, must be valid
				 * @param observation The sub-pixel position of the point in the frame
				 * @param strength The resulting signed strength of the point (positive for dark points, negative for bright points)
				 * @param strict True, if all surrounding pixels have the same sign relationship to the center pixel; False otherwise
				 * @return True, if the strength could be determined successfully
				 */
				bool determinePointStrength(const Frame& yFrame, const Vector2& observation, int32_t& strength, bool& strict) const;

				/**
				 * Returns whether this pattern is valid.
				 * @return True, if so
				 */
				inline bool isValid() const;

			protected:

				/**
				 * Determines the offsets for all surrounding pixels.
				 * @param radius The radius of the point pattern, with range [1, infinity)
				 * @param innerRadius The innerRadius defining a circular area around the center pixel where pixel data is excluded from use, with range [0, radius - 1]
				 * @param frameStrideElements The stride of the frame in which the point pattern is used, in elements, with range [frame.width(), infinity)
				 * @param useCircle True, if the outer shape of the point pattern is a circle; False, if the outer shape is a rectangle
				 * @param negativeOffset The resulting negative offset for the most top-left pixel of the surrounding pixels, needs to be negated
				 * @param positiveOffsets The resulting positive offsets for all surrounding pixels, starting at the top-left pixel, all in relation to the previous pixel
				 * @param offsets Optional resulting 2D offsets for all surrounding pixels
				 */
				static bool determineOffsets(const unsigned int radius, const unsigned int innerRadius, const unsigned int frameStrideElements, const bool useCircle, Index32& negativeOffset, Indices32& positiveOffsets, CV::PixelPositionsI* offsets = nullptr);

			protected:

				/// The radius of the point this pattern is able to detect.
				unsigned int radius_ = 0u;

				/// The inner radius specifies a circular area around the center pixel where pixel data is excluded from use.
				unsigned int innerRadius_ = 0u;

				/// The stride of the frame which which this pattern has been created.
				unsigned int frameStrideElements_ = 0u;

				/// True, if the outer shape of this pattern is a circle; False, if the outer shape is a rectangle.
				bool isCircle_ = false;

				/// The negative offset for the most top-left pixel of the surrounding pixels.
				Index32 negativeOffset_ = 0u;

				/// The positive offsets for all surrounding pixels starting at the top-left pixel, all in relation to the previous pixel
				Indices32 positiveOffsets_;

				/// The normalization factor for the strength of this pattern based on the number of pixels this pattern uses, with range (0, infinity).
				float strengthNormalization_ = 0.0f;

				/// The offsets of this pattern for all surrounding pixels.
				CV::PixelPositionsI offsets_;
		};

		/**
		 * Definition of a vector holding point patterns.
		 */
		using PointPatterns = std::vector<PointPattern>;

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
		 * Returns the closest point to a given point.
		 * @param queryPoint The query point for which the closest point will be determined
		 * @param sign The sign of the point the closest point must have
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
		 * @param secondClosestPointIndex The resulting index of the second closest point, with range [0, points.size() - 1], if no second closest point could be found
		 * @param closestSqrDistance The resulting square distance between the query given point and the closest point, with range [0, infinity)
		 * @param secondClosestSqrDistance The resulting square distance between the query given point and the second closest point, with range [0, infinity)
		 * @return True, if at least one closest point could be found
		 */
		static bool closestPoints(const Vector2& queryPoint, const Geometry::SpatialDistribution::DistributionArray& pointsDistributionArray, const Points& points, Index32& closestPointIndex, Index32& secondClosestPointIndex, Scalar& closestSqrDistance, Scalar& secondClosestSqrDistance);

	protected:

		/**
		 * Optimizes the position of detected points and removes outliers.
		 * @param yFrame The frame in which the points have been detected, with pixel format FORMAT_Y8, must be valid
		 * @param points The points to be optimized, must be valid
		 * @param pointPatterns The point patterns which were used to detect the points, must be valid
		 * @param optimizedPoints The resulting optimized points
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		bool optimizePoints(const Frame& yFrame, const Points& points, const PointPatterns& pointPatterns, Points& optimizedPoints, Worker* worker = nullptr) const;

		/**
		 * Detects points with several point patterns.
		 * @param yFrame The frame in which the points will be detected, with pixel format FORMAT_Y8, must be valid
		 * @param pointPatterns The point patterns to be used for point detection, at least one
		 * @param minDifference The minimal color intensity difference between the center pixel and the surrounding pixels, with range [0, 255]
		 * @param maxVariance The maximal deviation of the color intensities of the surrounding pixels, 0 to disable checking the deviation/variance
		 * @param points The resulting points
		 * @param suppressNonMaximum True, to apply a non-maximum suppression after the detection of points
		 * @param detectionScaleSteps The number of steps to be used for detection the scape, with range [1, infinity
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool detectPoints(const Frame& yFrame, const PointPatterns& pointPatterns, const unsigned int minDifference, const unsigned int maxVariance, Points& points, const bool suppressNonMaximum, const unsigned int detectionScaleSteps = 2u, Worker* worker = nullptr);

		/**
		 * Detects point candidates in a frame and adds them to a non-maximum suppression object.
		 * @param yFrame Pointer to the frame data, with pixel format FORMAT_Y8, must be valid
		 * @param yFramePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param mask Optional pointer to a mask frame where non-zero values indicate pixels to process, nullptr to process all pixels
		 * @param pointPattern The point pattern to be used for detection, must be valid
		 * @param minDifference The minimal color intensity difference between the center pixel and the surrounding pixels, with range [0, 255]
		 * @param maxVariance The maximal variance of the color intensities of the surrounding pixels, 0 to disable checking the variance
		 * @param nonMaximumSuppression The non-maximum suppression object to which detected candidates will be added
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tDarkPoint True, to detect dark points (dark center, bright surrounding); False, to detect bright points
		 */
		template <bool tDarkPoint>
		static void detectPointCandidates(const uint8_t* yFrame, const unsigned int yFramePaddingElements, const uint8_t* mask, const PointPattern& pointPattern, const uint8_t minDifference, const unsigned int maxVariance, CV::NonMaximumSuppressionT<uint32_t>& nonMaximumSuppression, Worker* worker = nullptr);

		/**
		 * Detects point candidates in a subset of rows (worker function for parallel execution).
		 * @param yFrame Pointer to the frame data, with pixel format FORMAT_Y8, must be valid
		 * @param yFramePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param mask Optional pointer to a mask frame where non-zero values indicate pixels to process, nullptr if tUseMask is false
		 * @param pointPatterns Pointer to the point pattern to be used for detection, must be valid
		 * @param minDifference The minimal color intensity difference between the center pixel and the surrounding pixels, with range [0, 255]
		 * @param maxVariance The maximal variance of the color intensities of the surrounding pixels, 0 to disable checking the variance
		 * @param nonMaximumSuppression Pointer to the non-maximum suppression object to which detected candidates will be added, must be valid
		 * @param firstColumn The first column to be processed, with range [0, width - 1]
		 * @param numberColumns The number of columns to be processed, with range [1, width - firstColumn]
		 * @param firstRow The first row to be processed, with range [0, height - 1]
		 * @param numberRows The number of rows to be processed, with range [1, height - firstRow]
		 * @tparam tDarkPoint True, to detect dark points (dark center, bright surrounding); False, to detect bright points
		 * @tparam tUseMask True, if a mask is used; False, if no mask is used
		 */
		template <bool tDarkPoint, bool tUseMask>
		static void detectPointCandidatesSubset(const uint8_t* yFrame, const unsigned int yFramePaddingElements, const uint8_t* mask, const PointPattern* pointPatterns, const uint8_t minDifference, const unsigned int maxVariance, CV::NonMaximumSuppressionT<uint32_t>* nonMaximumSuppression, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Determines the best matching radius for a detected point by testing smaller point patterns.
		 * @param yFrame Pointer to the frame data, with pixel format FORMAT_Y8, must be valid
		 * @param width The width of the frame, in pixels, with range [1, infinity)
		 * @param height The height of the frame, in pixels, with range [1, infinity)
		 * @param yFramePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param pixelPosition The pixel position of the detected point, must be inside the frame
		 * @param currentRadius The current radius of the detected point, with range [1, infinity)
		 * @param pointPatterns Pointer to an array of point patterns with increasing radii, must be valid
		 * @param numberPointPatterns The number of point patterns in the array, with range [2, infinity)
		 * @param minDifference The minimal color intensity difference between the center pixel and the surrounding pixels, with range [0, 255]
		 * @param maxVariance The maximal variance of the color intensities of the surrounding pixels, 0 to disable checking the variance
		 * @param radius The resulting best matching radius for the point
		 * @param strength The resulting strength of the point at the best matching radius
		 * @return True, if a smaller radius was found; False, if the current radius is the best match
		 * @tparam tDarkPoint True, to test for dark points (dark center, bright surrounding); False, to test for bright points
		 */
		template <bool tDarkPoint>
		static bool determinePointRadius(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const CV::PixelPosition& pixelPosition, const unsigned int currentRadius, const PointPattern* pointPatterns, const size_t numberPointPatterns, const uint8_t minDifference, const unsigned int maxVariance, unsigned int& radius, unsigned int& strength);

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

		/**
		 * Creates the point patterns with increasing radii.
		 * @param radius The maximal radius of the point patterns, with range [1, infinity)
		 * @param innerRadius The innerRadius defining a circular area around the center pixel where pixel data is excluded from use, with range [0, radius - 1]
		 * @param useCircle True, if the outer shape of the point pattern is a circle; False, if the outer shape is a rectangle
		 * @param frameStrideElements The stride of the frame in which the point patterns will be used, in elements, with range [frame.width(), infinity)
		 * @return The resulting point patterns
		 */
		static PointPatterns createPointPatterns(const unsigned int radius, const unsigned int innerRadius, const bool useCircle, const unsigned int frameStrideElements);

		/**
		 * Updates the point patterns for a specified frame stride.
		 * In case the frame stride is identical to the frame stride of the point patterns, the point patterns will not change.
		 * @param pointPatterns The point patterns to be updated, must be valid
		 * @param frameStrideElements The stride of the frame in which the point patterns will be used, in elements, with range [frame.width(), infinity)
		 */
		static void updatePointPatterns(PointPatterns& pointPatterns, const unsigned int frameStrideElements);

		/**
		 * Paints a point pattern into a frame.
		 * @param yFrame The frame in which the point pattern will be painted, with pixel format FORMAT_Y8, must be valid
		 * @param radius The radius of the point pattern, with range [1, infinity)
		 * @param pointColor The color of the point pattern, with range [0, 255]
		 */
		static bool paintPointPattern(Frame& yFrame, const unsigned int radius, const uint8_t pointColor = 0x00u);

	protected:

		/// The minimal color intensity difference between the center pixel and the surrounding pixels, with range [0, 255].
		unsigned int minDifference_ = 5u;

		/// The maximal deviation of the color intensities of the surrounding pixels, 0 to disable checking the deviation/variance.
		unsigned int maxDeviation_ = 30u;

		/// The maximal distance between two points to be considered as duplicated, with range [0, infinity).
		Scalar maxDistanceBetweenDuplicatePoints_ = Scalar(2);

		/// The point patterns to be used for point detection.
		PointDetector::PointPatterns pointPatterns_;

		/// Rough intermediate points.
		Points roughPoints_;

		/// The precise points detected in the latest frame.
		Points points_;

		/// The spatial distribution array of the points detected in the latest frame.
		Geometry::SpatialDistribution::DistributionArray pointsDistributionArray_;

		/// The frame with all images of point pattern with individual radii.
		Frame yPointPatternImages_;

		/// The width and height of the point pattern images.
		static constexpr unsigned int pointPatternImageSize_ = 31u;
};

inline unsigned int PointDetector::PointPattern::radius() const
{
	return radius_;
}

inline unsigned int PointDetector::PointPattern::diameter() const
{
	ocean_assert(isValid());
	return radius_ * 2u + 1u;
}

inline unsigned int PointDetector::PointPattern::innerRadius() const
{
	return innerRadius_;
}

inline unsigned int PointDetector::PointPattern::frameStrideElements() const
{
	return frameStrideElements_;
}

inline bool PointDetector::PointPattern::isCircle() const
{
	return isCircle_;
}

inline unsigned int PointDetector::PointPattern::negativeOffset() const
{
	return negativeOffset_;
}

inline const Indices32& PointDetector::PointPattern::positiveOffsets() const
{
	return positiveOffsets_;
}

inline const CV::PixelPositionsI& PointDetector::PointPattern::offsets() const
{
	return offsets_;
}

inline float PointDetector::PointPattern::normalizedStrength(const unsigned int strength) const
{
	ocean_assert(strengthNormalization_ != 0.0f);

	return float(strength) * strengthNormalization_;
}

template <uint8_t tMaxCenterColorFixed, uint8_t tMinSurroundingColorFixed>
inline uint32_t PointDetector::PointPattern::determineDarkPointStrength(const uint8_t centerPixelValue, const uint8_t* firstSurroundingPixel, const unsigned int minDifference, const unsigned int maxVariance) const
{
	ocean_assert(isValid());
	ocean_assert(firstSurroundingPixel != nullptr);

	static_assert(tMinSurroundingColorFixed < tMaxCenterColorFixed);

	if (centerPixelValue > tMaxCenterColorFixed) // the center pixel should have a certain amount of darkness
	{
		return 0u;
	}

	ocean_assert(centerPixelValue + minDifference <= 0xFFu);
	const uint8_t minSurroundingColor = std::max(tMinSurroundingColorFixed, uint8_t(centerPixelValue + minDifference)); // the surrounding pixels should be brighter than the center pixel and brighter than certain threshold in general

	uint32_t sumSqrDifferences = 0u;

	const uint8_t* surroundingPixel = firstSurroundingPixel;

	VarianceT<uint32_t> variance;

	for (const Index32& positiveOffset : positiveOffsets_)
	{
		surroundingPixel += positiveOffset;

		const uint8_t surroundingPixelValue = *surroundingPixel;

		if (surroundingPixelValue < minSurroundingColor)
		{
			return 0u;
		}

		variance.add(uint32_t(surroundingPixelValue));

		ocean_assert(surroundingPixelValue > centerPixelValue);

		const uint32_t difference = surroundingPixelValue - centerPixelValue;

		sumSqrDifferences += difference * difference;
	}

	if (sumSqrDifferences != 0u && (maxVariance == 0u || variance.variance() <= maxVariance))
	{
		return sumSqrDifferences;
	}

	return 0u;
}

template <uint8_t tMaxCenterColorFixed, uint8_t tMinSurroundingColorFixed>
inline uint32_t PointDetector::PointPattern::determineDarkPointStrength(const uint8_t* yPoint, const unsigned int minDifference, const unsigned int maxVariance) const
{
	ocean_assert(yPoint != nullptr);

	const uint8_t centerPixelValue = *yPoint;

	return determineDarkPointStrength<tMaxCenterColorFixed, tMinSurroundingColorFixed>(centerPixelValue, yPoint - negativeOffset_, minDifference, maxVariance);
}

template <uint8_t tMinCenterColorFixed, uint8_t tMaxSurroundingColorFixed>
inline uint32_t PointDetector::PointPattern::determineBrightPointStrength(const uint8_t centerPixelValue, const uint8_t* firstSurroundingPixel, const unsigned int minDifference, const unsigned int maxVariance) const
{
	ocean_assert(isValid());
	ocean_assert(firstSurroundingPixel != nullptr);

	static_assert(tMinCenterColorFixed < tMaxSurroundingColorFixed);

	if (centerPixelValue < tMinCenterColorFixed) // the center pixel should have a certain amount of brightness
	{
		return 0u;
	}

	ocean_assert(int32_t(centerPixelValue) - int32_t(minDifference) >= 0);
	const uint8_t maxSurroundingColor = std::min(tMaxSurroundingColorFixed, uint8_t(centerPixelValue - minDifference)); // the surrounding pixels should be darker than the center pixel and darker than certain threshold in general

	uint32_t sumSqrDifferences = 0u;

	const uint8_t* surroundingPixel = firstSurroundingPixel;

	VarianceT<uint32_t> variance;

	for (const Index32& positiveOffset : positiveOffsets_)
	{
		surroundingPixel += positiveOffset;

		const uint8_t surroundingPixelValue = *surroundingPixel;

		if (surroundingPixelValue > maxSurroundingColor)
		{
			return 0u;
		}

		variance.add(uint32_t(surroundingPixelValue));

		ocean_assert(surroundingPixelValue < centerPixelValue);

		const uint32_t difference = centerPixelValue - surroundingPixelValue;

		sumSqrDifferences += difference * difference;
	}

	if (sumSqrDifferences != 0u && (maxVariance == 0u || variance.variance() <= maxVariance))
	{
		return sumSqrDifferences;
	}

	return 0u;
}

template <uint8_t tMinCenterColorFixed, uint8_t tMaxSurroundingColorFixed>
inline uint32_t PointDetector::PointPattern::determineBrightPointStrength(const uint8_t* yPoint, const unsigned int minDifference, const unsigned int maxVariance) const
{
	ocean_assert(yPoint != nullptr);

	const uint8_t centerPixelValue = *yPoint;

	return determineBrightPointStrength<tMinCenterColorFixed, tMaxSurroundingColorFixed>(centerPixelValue, yPoint - negativeOffset_, minDifference, maxVariance);
}

inline bool PointDetector::PointPattern::isValid() const
{
	ocean_assert(radius_ == 0u || innerRadius_ < radius_);
	ocean_assert(radius_ == 0u || frameStrideElements_ != 0u);

	return radius_ >= 1u;
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

	for (unsigned int xBin = (unsigned int)(std::max(0, int(xBinCenter) - 1)); xBin < std::min(xBinCenter + 2u, pointsDistributionArray.horizontalBins()); ++xBin)
	{
		for (unsigned int yBin = (unsigned int)(std::max(0, int(yBinCenter) - 1)); yBin < std::min(yBinCenter + 2u, pointsDistributionArray.verticalBins()); ++yBin)
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
