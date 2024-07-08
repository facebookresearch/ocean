/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/cv/detector/qrcodes/QRCodes.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Memory.h"

#include "ocean/cv/Bresenham.h"

#include "ocean/math/Box2.h"
#include "ocean/math/Vector2.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace QRCodes
{

/**
 * This class implements an alignment pattern inside QR codes.
 * @ingroup cvdetectorqrcodes
 */
class AlignmentPattern
{
	public:

		/**
		 * Default constructor that creates an invalid alignment pattern
		 */
		AlignmentPattern() = default;

		/**
		 * Constructor.
		 * @param center The center location at which this alignment pattern was detected
		 * @param averageSegmentSize The average segment size in pixels that has been measured during the detection, range: [1, infinity)
		 */
		inline AlignmentPattern(const Vector2& center, const unsigned int averageSegmentSize);

		/**
		 * Returns the center location at which this alignment pattern was detected
		 * @return The location of the center
		 */
		inline const Vector2& center() const;

		/**
		 * Returns the average segment size in pixels that has been measured during the detection
		 * @return The average segment size
		 */
		inline unsigned int averageSegmentSize() const;

		/**
		 * Returns true if this alignment pattern is valid
		 * @return True if this alignment pattern is valid, otherwise false
		 */
		inline bool isValid() const;

	protected:

		/// The center location of the alignment pattern
		Vector2 center_ = Vector2(Numeric::minValue(), Numeric::minValue());

		/// The average segment size in pixels that has been measured during the detection
		unsigned int averageSegmentSize_ = 0u;
};

/// Definition of a vector of alignment patterns
typedef std::vector<AlignmentPattern> AlignmentPatterns;

/**
 * This class implements a detector for alignment patterns inside QR codes.
 * @ingroup cvdetectorqrcodes
 */
class OCEAN_CV_DETECTOR_QRCODES_EXPORT AlignmentPatternDetector
{
	public:

		/**
		 * Detects alignment patterns in the specified search region
		 * @param yFrame Pointer to the 8-bit grayscale input image in which alignment patterns will be searched, must be valid, origin must be in the upper left
		 * @param width The width of the input image, range: [searchX + searchWidth, infinity)
		 * @param height The height of the input image, range: [searchY + searchHeight, infinity)
		 * @param paddingElements The padding elements of the input image, range: [0, infinity)
		 * @param searchX The x-coordinate of the top-left corner of the search region, range: [0, width - searchWidth]
		 * @param searchY The y-coordinate of the top-left corner of the search region, range: [0, height - searchHeight]
		 * @param searchWidth The width of the search region, range: [1, width - searchX]
		 * @param searchHeight The height of the search region, range: [1, height - searchY]
		 * @param isNormalReflectance Indicates whether alignment patterns with normal or inverted reflectance are searched
		 * @param grayThreshold The gray value that has been determined as the separation between foreground and background modules (cf. `FinderPattern::grayThreshold()`)
		 * @return The list of detected alignment patterns
		 */
		static AlignmentPatterns detectAlignmentPatterns(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int paddingElements, const unsigned int searchX, const unsigned int searchY, const unsigned int searchWidth, const unsigned int searchHeight, const bool isNormalReflectance, const unsigned int grayThreshold);

	protected:

		/**
		 * Detects alignment patterns in a row of an image
		 * @param yFrame Pointer to the 8-bit grayscale input image in which alignment patterns will be searched, must be valid, origin must be in the upper left
		 * @param width The width of the input image, range: [searchX + searchWidth, infinity)
		 * @param height The height of the input image, range: [searchY + searchHeight, infinity)
		 * @param paddingElements The padding elements of the input image, range: [0, infinity)
		 * @param row The row of the input image in which alignment patterns will be searched, range: [0, height)
		 * @param offsetX The offset in x-direction that will be added to the location of detected alignment patterns, range: [0, infinity)
		 * @param offsetY The offset in y-direction that will be added to the location of detected alignment patterns, range: [0, infinity)
		 * @param isNormalReflectance Indicates whether alignment patterns with normal or inverted reflectance are searched
		 * @param grayThreshold The gray value that has been determined as the separation between foreground and background modules (cf. `FinderPattern::grayThreshold()`)
		 * @param alignmentPatterns The resulting list of alignment patterns that have been found in the selected row
		 */
		static void detectAlignmentPatternsInRow(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int paddingElements, const unsigned int row, const unsigned int offsetX, const unsigned int offsetY, const bool isNormalReflectance, const unsigned int grayThreshold, AlignmentPatterns& alignmentPatterns);

		/**
		 * Performs a circular check around a candidate location to confirm a detection in a row
		 * @param yFrame Pointer to the 8-bit grayscale input image in which alignment patterns will be searched, must be valid, origin must be in the upper left
		 * @param width The width of the input image, range: [searchX + searchWidth, infinity)
		 * @param height The height of the input image, range: [searchY + searchHeight, infinity)
		 * @param paddingElements The padding elements of the input image, range: [0, infinity)
		 * @param xCenter The x-coordinate of the candidate location that will be checked vertically, range: [0, width)
		 * @param yCenter The y-coordinate of the candidate location that will be checked vertically, range: [0, height)
		 * @param minLength The minimum length in pixels of the segments that are expected, range: [1, infinity)
		 * @param maxLength The maximum length in pixels of the segments that are expected, range: [1, infinity)
		 * @param isNormalReflectance Indicates whether alignment patterns with normal or inverted reflectance are searched
		 * @param grayThreshold The gray value that has been determined as the separation between foreground and background modules, range: [0, 255]
		 * @return True, if an acceptable sequence of fore- and background segments has been detected in all directions, otherwise false
		 */
		static bool checkInCircle(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int paddingElements, const unsigned int xCenter, const unsigned int yCenter, const unsigned int minLength, const unsigned int maxLength, const bool isNormalReflectance, const unsigned int grayThreshold);

		/**
		 * Performs a check to confirm a detection in a specified direction
		 * @param yFrame Pointer to the 8-bit grayscale input image in which alignment patterns will be searched, must be valid, origin must be in the upper left
		 * @param width The width of the input image, range: [searchX + searchWidth, infinity)
		 * @param height The height of the input image, range: [searchY + searchHeight, infinity)
		 * @param paddingElements The padding elements of the input image, range: [0, infinity)
		 * @param xCenter The x-coordinate of the candidate location that will be checked vertically, range: [0, width)
		 * @param yCenter The y-coordinate of the candidate location that will be checked vertically, range: [0, height)
		 * @param minLength The minimum length in pixels of the segments that are expected, range: [1, infinity)
		 * @param maxLength The maximum length in pixels of the segments that are expected, range: [1, infinity)
		 * @param isNormalReflectance Indicates whether alignment patterns with normal or inverted reflectance are searched
		 * @param grayThreshold The gray value that has been determined as the separation between foreground and background modules, range: [0, 255]
		 * @param angle The angle (in RAD) of the search direction, range: [0, PI)
		 * @return True, if an acceptable sequence of fore- and background segments has been detected in all directions, otherwise false
		 */
		static bool checkInDirection(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int paddingElements, const unsigned int xCenter, const unsigned int yCenter, const unsigned int minLength, const unsigned int maxLength, const bool isNormalReflectance, const unsigned int grayThreshold, const Scalar angle);
};

inline AlignmentPattern::AlignmentPattern(const Vector2& center, const unsigned int averageSegmentSize) :
	center_(center),
	averageSegmentSize_(averageSegmentSize)
{
	ocean_assert(isValid());
}

inline const Vector2& AlignmentPattern::center() const
{
	return center_;
}

inline unsigned int AlignmentPattern::averageSegmentSize() const
{
	return averageSegmentSize_;
}

inline bool AlignmentPattern::isValid() const
{
	return center_.x() >= Scalar(0) && center_.y() >= Scalar(0) && averageSegmentSize_ != 0u;
}

} // namespace QRCodes

} // namespace Detector

} // namespace CV

} // namespace Ocean
