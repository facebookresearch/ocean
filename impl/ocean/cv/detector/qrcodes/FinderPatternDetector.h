/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/cv/detector/qrcodes/QRCodes.h"
#include "ocean/cv/detector/qrcodes/QRCode.h"
#include "ocean/cv/detector/qrcodes/QRCodeEncoder.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Memory.h"

#include "ocean/cv/Bresenham.h"

#include "ocean/geometry/Homography.h"

#include "ocean/math/Box2.h"
#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/Vector2.h"

#include <array>
#include <cstdint>

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace QRCodes
{

/**
 * Definition of a triplet of indices
 * @ingroup cvdetectorqrcodes
 */
typedef std::array<unsigned int, 3> IndexTriplet;

/**
 * Definition of a vector index triplets
 * @ingroup cvdetectorqrcodes
 */
typedef std::vector<IndexTriplet> IndexTriplets;

/**
 * Definition of a class for finder patterns of QR codes (squares in the top-left, top-right and bottom-left corners)
 * @ingroup cvdetectorqrcodes
 */
class FinderPattern
{
	public:

		/**
		 * Creates an invalid finder pattern object.
		 */
		inline FinderPattern();

		/**
		 * Creates a new finder pattern object by a given position and edge length.
		 * @param position The (center) position of the finder pattern within the camera frame
		 * @param length The edge length of the finder pattern in pixels, with range (0, infinity)
		 * @param centerIntensity The intensity that has been measured in the center of the finder pattern, range: [0, 255]
		 * @param grayThreshold Threshold that was used during the detection, range [0, 255]
		 * @param symmetryScore Symmetry score of this finder pattern, range: [0, infinity) (lower value = higher symmetry)
		 */
		inline FinderPattern(const Vector2& position, const Scalar length, const unsigned int centerIntensity, const unsigned int grayThreshold, const Scalar symmetryScore);

		/**
	 	 * Creates a new finder pattern object by a given position and edge length.
		 * @param position The (center) position of the finder pattern within the camera frame
		 * @param length The edge length of the finder pattern in pixels, with range (0, infinity)
		 * @param centerIntensity The intensity that has been measured in the center of the finder pattern, range: [0, 255]
		 * @param grayThreshold Threshold that was used during the detection, range [0, 255]
		 * @param symmetryScore Symmetry score of this finder pattern, range: [0, infinity) (lower value = higher symmetry)
		 * @param corners The locations of the four corners of this finder pattern, must be valid and have 4 elements
		 * @param orientation Dominant orientation of the finder pattern
		 * @param moduleSize The size of modules (=bits) in pixels
		 */
		inline FinderPattern(const Vector2& position, const Scalar length, const unsigned int centerIntensity, const unsigned int grayThreshold, const Scalar symmetryScore, const Vector2* corners, const Vector2& orientation, const Scalar moduleSize);

		/**
		 * Returns the (center) position of the finder pattern.
		 * @return The finder pattern's position within the camera frame
		 */
		inline const Vector2& position() const;

		/**
		 * Returns the radius of the finder pattern.
		 * @return The finder pattern's radius, with range (0, infinity), 0 for an invalid object
		 */
		inline Scalar length() const;

		/**
		 * Returns the intensity value that was measured in the center of the finder pattern
		 * @return The intensity value, range: [0, 255]
		 */
		inline unsigned int centerIntensity() const;

		/**
		 * Returns the threshold that was used for the detection of this finder pattern
		 * @return The threshold value, range: [0, 255]
		 */
		inline unsigned int grayThreshold() const;

		/**
		 * Returns the width of a module (= bit) in pixels
		 * @return The module width
		 */
		inline Scalar moduleSize() const;

		/**
		 * Returns the symmetry score that was determined when this finder pattern was detected
		 * @return The symmetry score of this finder pattern
		 */
		inline Scalar symmetryScore() const;

		/**
		 * Returns true if the four corners of this finder pattern are known, otherwise false
		 * @return True, if the four corners are known, otherwise false
		 */
		inline bool cornersKnown() const;

		/**
		 * Returns a pointer to the four corners of this finder pattern.
		 * @return A constant pointer to the four corners of this finder pattern. These values are undefined if `cornersKnown()` returns false.
		 */
		inline const Vector2* corners() const;

		/**
		 * Returns the dominant orientation of this finder pattern
		 * @return The vector defining the orientation (will be (1, 0) by default, i.e. if it's not set)
		 */
		inline const Vector2& orientation() const;

		/**
		 * Returns whether this finder pattern is of normal reflectance
		 * @return True if so, otherwise false
		 */
		inline bool isNormalReflectance() const;

		/**
		 * Comparator to sort finder patterns based on their location in an image
		 * Pattern `a` comes before pattern `b` if (pseudo-code) `a.y * imageWidth + a.x < b.y * imageWidth + b.x`
		 * @param first The first finder pattern to compare
		 * @param second The second finder pattern to compare
		 * @return True if the first pattern comes before the second pattern, otherwise false
		 */
		static inline bool comesBefore(const FinderPattern& first, const FinderPattern& second);

	protected:

		/// The (center) position of the finder pattern within the camera frame.
		Vector2 position_;

		/// The edge length of the finder pattern in pixels, range: (0, infinity).
		Scalar length_;

		/// The intensity value that has been measured in the center of the finder pattern
		unsigned int centerIntensity_;

		/// The threshold that was used during the detection of this finder pattern.
		unsigned int grayThreshold_;

		/// The symmetry score of this finder pattern, range: [0, infinity) (lower score = higher symmetry)
		Scalar symmetryScore_;

		/// True if the four corners of this finder pattern are known, otherwise false
		bool cornersKnown_;

		/// The four corners of this finder pattern; points are stored in counter-clockwise order but no guarantee on which corner is the first; if `cornersDetected_` is false these values will be undefined
		Vector2 corners_[4];

		/// Dominant orientation of this finder pattern
		Vector2 orientation_;

		/// Module width (bit width) in pixels
		Scalar moduleSize_;
};

/**
 * Definition of a vector holding finder pattern.
 * @ingroup cvdetectorqrcodes
 */
typedef std::vector<FinderPattern> FinderPatterns;

/**
 * Definition of a 3-tuple of finder patterns
 * @ingroup cvdetectorqrcodes
 */
typedef std::array<FinderPattern, 3> FinderPatternTriplet;

/**
 * This class implements a detector for finder patterns which are part of QR Codes.
 * @ingroup cvdetectorqrcodes
 */
class OCEAN_CV_DETECTOR_QRCODES_EXPORT FinderPatternDetector
{
	protected:

		/// The intensity threshold between two successive pixels to count as a transition from dark to light (or vice versa).
		static constexpr int deltaThreshold = 30;

		/**
		 * This class implements a simple history for previous pixel transitions (a sliding window of pixel transitions).
		 */
		class TransitionHistory
		{
			public:

				/**
				* Creates a new history object.
				*/
				inline TransitionHistory();

				/**
				* Returns the history with window size N.
				* @return The sum of the most recent delta
				*/
				inline int history1();

				/**
				* Returns the history with window size N.
				* @return The sum of the most recent delta
				*/
				inline int history2();

				/**
				* Returns the history with window size N.
				* @return The sum of the most recent delta
				*/
				inline int history3();

				/**
				* Returns the history with window size N.
				* @return The sum of the most recent delta
				*/
				inline int history4();

				/**
				* Returns the history with window size N.
				* @return The sum of the most recent delta
				*/
				inline int history5();

				/**
				* Adds a new delta object as most recent history.
				* Existing history objects will be moved by one pixel.
				* @param newDelta The new delta object to be added
				*/
				inline void push(const int newDelta);

				/**
				* Resets the history object.
				*/
				inline void reset();

			protected:

				/// The most recent deltas.
				int deltas_[5] = { 0, 0, 0, 0, 0 };
		};

	public:

		/**
		 * Detects finder patterns of a QR code in a 8 bit grayscale image.
		 * @param yFrame The 8 bit grayscale frame in which the finder patterns will be detected, must be valid
		 * @param width The width of the given grayscale frame in pixel, with range [29, infinity)
		 * @param height The height of the given grayscale frame in pixel, with range [29, infinity)
		 * @param minimumDistance The minimum distance in pixels that is enforced between any pair of finder patterns, range: [0, infinity), default: 10
		 * @param paddingElements Optional number of padding elements at the end of each image row, in elements, with range [0, infinity), default: 0
		 * @param worker Optional worker to distribute the computation
		 * @return The detected finder patterns
		 */
		static FinderPatterns detectFinderPatterns(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int minimumDistance = 10u, const unsigned int paddingElements = 0u, Worker* worker = nullptr);

		/**
		 * Extract 3-tuples of finder patterns that form good (plausible) candidates for QR code symbols
		 * @param finderPatterns The list finder patterns in which 3-tuples forming potential QR code symbols are sought, must be valid, minimum size: 3
		 * @param distanceScaleTolerance Scale factor that define how much corners of one finder pattern may deviate from the parallel lines of another finder pattern, range: [0, infinity), default: 0.05
		 * @param moduleSizeScaleTolerance Defines the maximum difference scale of the module size between pairs of finder patterns in order to be considered a match, range: [0, 1], default: 0.35
		 * @param angleTolerance Defines the maximum difference of the dominant orientation of pairs of finder patterns in order to be considered a match, measured in radian, range: [0, PI/2), default: deg2rad(9)
		 * @return A list of 3-tuples of finder patterns, will be empty on failure (or if nothing was found)
		 */
		static IndexTriplets extractIndexTriplets(const FinderPatterns& finderPatterns, const Scalar distanceScaleTolerance = Scalar(0.175), const Scalar moduleSizeScaleTolerance = Scalar(0.35), const Scalar angleTolerance = Numeric::deg2rad(Scalar(9)));

	protected:

		/**
		 * Detects finder patterns of QR codes in subregion of a given 8 bit grayscale image.
		 * @param yFrame The 8 bit grayscale frame in which the finder patterns will be detected, must be valid
		 * @param width The width of the given grayscale frame in pixel, with range [29, infinity)
		 * @param height The height of the given grayscale frame in pixel, with range [29, infinity)
		 * @param finderPatterns The resulting finderPatterns, will be added to the end of the vector
		 * @param multiThreadLock Lock object in case this function is executed in multiple threads concurrently, otherwise nullptr
		 * @param paddingElements Optional number of padding elements at the end of each image row, in elements, with range [0, infinity)
		 * @param firstRow The first row to be handled, with range [10, height - 10)
		 * @param numberRows The number of rows to be handled, with range [1, height - 10 - firstRow]
		 */
		static void detectFinderPatternsSubset(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, FinderPatterns* finderPatterns, Lock* multiThreadLock, const unsigned int paddingElements, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Detects finder patterns of QR codes in a single row of an grayscale image.
		 * @param yFrame The 8 bit grayscale frame in which the finder patterns will be detected, must be valid
		 * @param width The width of the given grayscale frame in pixel, with range [29, infinity)
		 * @param height The height of the given grayscale frame in pixel, with range [29, infinity)
		 * @param y The index of the row in which the finder patterns will be detected, with range [10, height - 11]
		 * @param finderPatterns The resulting detected finder patterns, will be added to the end of the vector
		 * @param paddingElements Optional number of padding elements at the end of each image row, in elements, with range [0, infinity)
		 */
		static void detectFinderPatternInRow(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int y, FinderPatterns& finderPatterns, const unsigned int paddingElements);

		/**
		 * Estimates the locations of the corners of finder pattern and computes the dominant orientation of the finder pattern from those corners
		 * @param xCenter The pixel-accurate x-coordinate of the candidate location
		 * @param yCenter The pixel-accurate y-coordinate of the candidate location
		 * @param edgePoints The edge points which will be used to determine the corners of the finder pattern, must be valid and have `edgePointsSize` elements
		 * @param edgePointsSize Number of edge points that are available, range: [2, infinity) must be even
		 * @param location The resulting center location of the finder pattern that is determined from the four corners that will be determined by this function
		 * @param corners The resulting four corners of the finder pattern that will be determined, must be valid, in counter-clockwise order, and must have size of at least 4 elements
		 * @param orientation The resulting main orientation of the finder pattern that will be determined
		 * @param moduleSize The resulting size of the modules in this finder pattern candidate
		 * @param edgePointDistanceTolerance The factor that defines the maximum deviation from the distance between the center and the edge point closest to the center, range: [0, 1]
		 * @param maxEdgeLineDistance The maximum distance (in pixel) that new edge points may have in order to be accepted as "on the edge line", range: [0, infinity)
		 * @return True on success, otherwise false
		 */
		static bool estimateFinderPatternCorners(const unsigned int xCenter, const unsigned int yCenter, const Vector2* edgePoints, const unsigned int edgePointsSize, Vector2& location, Vector2* corners, Vector2& orientation, Scalar& moduleSize, const Scalar edgePointDistanceTolerance = 2.25, const Scalar maxEdgeLineDistance = 1.5);

		/**
		 * Refine the location and corners of a finder pattern
		 * @param yFrame Pointer to the input grayscale image, must be valid
		 * @param width The width of the input grayscale image, range: [1, infinity)
		 * @param height The height of the input grayscale image, range:[1, infinity)
		 * @param finderPattern The resulting finder pattern of which its position and corners will be refined
		 * @param yFramePaddingElements The number of padding elements of the input grayscale image, range: [0, infinity)
		 * @return True if the refinement was successful, otherwise false
		 * @sa estimateFinderPatternCorners()
		 */
		static bool refineFinderPatternLocation(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, FinderPattern& finderPattern, const unsigned int yFramePaddingElements = 0u);

		/**
		 * Performs a check around a given candidate location looking for a correct configuration of light and dark pixels (testing 8 angles each yielding 2 edge points)
		 * @param yFrame The 8 bit grayscale frame in which the finder pattern candidate will be tested, must be valid
		 * @param width The width of the given grayscale frame in pixels, range: [29, infinity)
		 * @param height The height of the given grayscale frame in pixels, range: [29, infinity)
		 * @param paddingElements The number of padding elements in the given image with range [0, infinity)
		 * @param xCenter The horizontal location within the frame at which the existence of the finder pattern will be checked, in pixels, with range [0, width - 1]
		 * @param yCenter The vertical location within the frame at which the existence of the finder pattern will be checked, in pixels, with range [0, height - 1]
		 * @param threshold The grayscale threshold separating a bright pixel from a dark pixel, with range [0, 255]
		 * @param blackSquareSegmentMin Minimum diameter of the outer black square in pixels, range: [1, infinity)
		 * @param blackSquareSegmentMax Maximum diameter of the outer black square in pixels, range: [blackSquareSegmentMin, infinity)
		 * @param whiteSquareSegmentMin Minimum diameter of the inner white square in pixels, range: [1, infinity)
		 * @param whiteSquareSegmentMax Maximum diameter of the inner white square in pixels, range: [whiteSquareSegmentMin, infinity)
		 * @param centerSegmentMin Minimum diameter of the center black square in pixels, range: [1, infinity)
		 * @param centerSegmentMax Maximum diameter of the center black square in pixels, range: [centerSegmentMin, infinity)
		 * @param symmetryScore The resulting symmetry score that is computed for the current candidate location `(xCenter, yCenter)`; this score is based on distances so the lower the score, the better. Range: [0, infinity)
		 * @param edgePoints If specified, will hold the resulting points detected during the directional checks on the outside border of the finder pattern candidate. Must be valid, expected size: `2 * angles`
		 * @return True if all edge points of the finder pattern are found in all scanline directions, otherwise false
		 */
		static bool checkFinderPatternInNeighborhood(const uint8_t* const yFrame, const unsigned width, const unsigned height, const unsigned int paddingElements, const unsigned int xCenter, const unsigned int yCenter, const unsigned int threshold, const unsigned int blackSquareSegmentMin, const unsigned int blackSquareSegmentMax, const unsigned int whiteSquareSegmentMin, const unsigned int whiteSquareSegmentMax, const unsigned int centerSegmentMin, const unsigned int centerSegmentMax, Scalar& symmetryScore, Vector2* edgePoints);

		/**
		 * Performs a check for a given candidate location in a specified direction (yielding 2 edge points)
		 * @param yFrame The 8 bit grayscale frame in which the finder pattern candidate will be tested, must be valid
		 * @param width The width of the given grayscale frame in pixels, range: [29, infinity)
		 * @param height The height of the given grayscale frame in pixels, range: [29, infinity)
		 * @param paddingElements The number of padding elements in the given image with range [0, infinity)
		 * @param xCenter The horizontal location within the frame at which the existence of the finder pattern will be checked, in pixels, with range [0, width - 1]
		 * @param yCenter The vertical location within the frame at which the existence of the finder pattern will be checked, in pixels, with range [0, height - 1]
		 * @param angle The angle in Radian defining the directions in which edge points will be searched, range: [0, pi)
		 * @param threshold The grayscale threshold separating a bright pixel from a dark pixel, with range [0, 255]
		 * @param blackSquareSegmentMin Minimum diameter of the outer black square in pixels, range: [1, infinity)
		 * @param blackSquareSegmentMax Maximum diameter of the outer black square in pixels, range: [blackSquareSegmentMin, infinity)
		 * @param whiteSquareSegmentMin Minimum diameter of the inner white square in pixels, range: [1, infinity)
		 * @param whiteSquareSegmentMax Maximum diameter of the inner white square in pixels, range: [whiteSquareSegmentMin, infinity)
		 * @param centerSegmentMin Minimum diameter of the center black square in pixels, range: [1, infinity)
		 * @param centerSegmentMax Maximum diameter of the center black square in pixels, range: [centerSegmentMin, infinity)
		 * @param topBorder The resulting location of the last pixel on the current finder pattern in the specified direction of the scanline
		 * @param bottomBorder The resulting location of the last pixel on the current finder pattern in the opposite direction (`angle + pi`) of the specified direction of the scanline
		 * @return True if the two edge points of the finder pattern are found in the specified scanline direction, otherwise false
		 */
		static bool checkFinderPatternDirectional(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int paddingElements, const unsigned int xCenter, const unsigned int yCenter, const Scalar angle, const unsigned int threshold, const unsigned int blackSquareSegmentMin, const unsigned int blackSquareSegmentMax, const unsigned int whiteSquareSegmentMin, const unsigned int whiteSquareSegmentMax, const unsigned int centerSegmentMin, const unsigned int centerSegmentMax, Vector2& topBorder, Vector2& bottomBorder);

		/**
		 * Checks whether the given pixel is a transition-to-black pixel (whether the direct left neighbor is a bright pixel).
		 * @param pixel The pixel to be checked, must be valid
		 * @param history The history object containing information about previous pixels
		 * @return True, if so
		 */
		static inline bool isTransitionToBlack(const uint8_t* pixel, TransitionHistory& history);

		/**
		 * Checks whether the given pixel is a transition-to-white pixel (whether the direct left neighbor is a dark pixel).
		 * @param pixel The pixel to be checked, must be valid
		 * @param history The history object containing information about previous pixels
		 * @return True, if so
		 */
		static inline bool isTransitionToWhite(const uint8_t* pixel, TransitionHistory& history);

		/**
		 * Determines the gray threshold separating bright pixels form dark pixels.
		 * The threshold is based on already actual pixel values for which the association is known already.<br>
		 * The provided start position is a pointer to any pixel within the image, with horizontal range [1, width - segmentSize1 - segmentSize2 - segmentSize3 - segmentSize4 - segmentSize5 - 2].
		 * In addition to the pixels covered by the five segments, the fist pixel left of the segments and the last pixel right of the segments are also used for estimation of the threshold.
		 * @param yPosition The first pixel within an 8 bit grayscale image for which 5 connected segments are known with black, white, black, white, and black pixels, must be valid
		 * @param segmentSize1 The number of pixels covering dark pixels, with range [1, width - ...)
		 * @param segmentSize2 The number of pixels covering bright pixels, with range [1, width - ...)
		 * @param segmentSize3 The number of pixels covering dark pixels, with range [1, width - ...)
		 * @param segmentSize4 The number of pixels covering bright pixels, with range [1, width - ...)
		 * @param segmentSize5 The number of pixels covering dark pixels, with range [1, width - segmentSize1 - segmentSize2 - segmentSize3 - segmentSize4 - 2]
		 * @return The threshold separating bright pixels from dark pixels, with range [0, 255], -1 if no valid threshold could be determined
		 */
		static inline unsigned int determineThreshold(const uint8_t* yPosition, const unsigned int segmentSize1, const unsigned int segmentSize2, const unsigned int segmentSize3, const unsigned int segmentSize4, const unsigned int segmentSize5);

		/**
		 * Returns true if a pair of finder patterns is in parallel configuration, i.e., if one is above/below/left of/right of the other (and vice versa)
		 * @param finderPatternA The first finder pattern that will be used
		 * @param finderPatternB The second finder pattern that will be used
		 * @param distanceTolerance A scaling factor that defines how much the configuration may deviate from perfect parallelism, range: [0, infinity), default: 0.05
		 * @return True if the two finder patterns are in a parallel configuration, otherwise false
		 */
		static inline bool isParallel(const FinderPattern& finderPatternA, const FinderPattern& finderPatternB, const Scalar distanceTolerance = Scalar(0.05));

		/**
		 * Returns true if a pair of finder patterns is in a diagonal configuration, i.e. the center of one pattern lies on one of the two diagonal (infinite) lines of the other finder pattern (and vice versa)
		 * @param finderPatternA The first finder pattern that will be used
		 * @param finderPatternB The second finder pattern that will be used
		 * @param angleTolerance Defines the angle that the centers of the finder pattern may deviate from the actual diagonal infinite lines (in radian), range: [0, PI/2), default: deg2rad(9)
		 * @return True if the two finder patterns are in a diagonal configuration, otherwise false
		 */
		static inline bool isDiagonal(const FinderPattern& finderPatternA, const FinderPattern& finderPatternB, const Scalar angleTolerance = Numeric::deg2rad(9));
};

inline FinderPattern::FinderPattern() :
	FinderPattern::FinderPattern(Vector2(-1, -1), Scalar(0), 0u, 0u, Numeric::maxValue())
{
	// nothing to do here
}

inline FinderPattern::FinderPattern(const Vector2& position, const Scalar length, const unsigned int centerIntensity, const unsigned int grayThreshold, const Scalar symmetryScore) :
	position_(position),
	length_(length),
	centerIntensity_(centerIntensity),
	grayThreshold_(grayThreshold),
	symmetryScore_(symmetryScore),
	cornersKnown_(false),
	orientation_(1, 0),
	moduleSize_(length / Scalar(7))
{
	ocean_assert(centerIntensity_ <= 255u);
	ocean_assert(grayThreshold_ <= 255u);

	corners_[0] = Vector2(-1, -1);
	corners_[1] = Vector2(-1, -1);
	corners_[2] = Vector2(-1, -1);
	corners_[3] = Vector2(-1, -1);
}

inline FinderPattern::FinderPattern(const Vector2& position, const Scalar length, const unsigned int centerIntensity, const unsigned int grayThreshold, const Scalar symmetryScore, const Vector2* corners, const Vector2& orientation, const Scalar moduleSize) :
	position_(position),
	length_(length),
	centerIntensity_(centerIntensity),
	grayThreshold_(grayThreshold),
	symmetryScore_(symmetryScore),
	cornersKnown_(true),
	orientation_(orientation),
	moduleSize_(moduleSize)
{
	ocean_assert(centerIntensity_ <= 255u);
	ocean_assert(grayThreshold_ <= 255u);

	ocean_assert(corners != nullptr);

	// Expect a counter-clockwise order for the corners

	ocean_assert((corners[1] - corners[0]).cross(corners[3] - corners[0]) <= 0);
	ocean_assert((corners[2] - corners[1]).cross(corners[0] - corners[1]) <= 0);
	ocean_assert((corners[3] - corners[2]).cross(corners[1] - corners[2]) <= 0);
	ocean_assert((corners[0] - corners[3]).cross(corners[2] - corners[3]) <= 0);

	corners_[0] = corners[0];
	corners_[1] = corners[1];
	corners_[2] = corners[2];
	corners_[3] = corners[3];
}

inline const Vector2& FinderPattern::position() const
{
	return position_;
}

inline Scalar FinderPattern::length() const
{
	return length_;
}

inline unsigned int FinderPattern::centerIntensity() const
{
	return centerIntensity_;
}

inline unsigned int FinderPattern::grayThreshold() const
{
	return grayThreshold_;
}

inline Scalar FinderPattern::moduleSize() const
{
	return moduleSize_;
}

inline Scalar FinderPattern::symmetryScore() const
{
	return symmetryScore_;
}

inline bool FinderPattern::cornersKnown() const
{
	return cornersKnown_;
}

inline const Vector2* FinderPattern::corners() const
{
	// Expect a counter-clockwise order for the corners, if the corners are known

	ocean_assert(cornersKnown() == false || (corners_[1] - corners_[0]).cross(corners_[3] - corners_[0]) <= 0);
	ocean_assert(cornersKnown() == false || (corners_[2] - corners_[1]).cross(corners_[0] - corners_[1]) <= 0);
	ocean_assert(cornersKnown() == false || (corners_[3] - corners_[2]).cross(corners_[1] - corners_[2]) <= 0);
	ocean_assert(cornersKnown() == false || (corners_[0] - corners_[3]).cross(corners_[2] - corners_[3]) <= 0);

	return corners_;
}

inline const Vector2& FinderPattern::orientation() const
{
	ocean_assert(Numeric::isEqualEps(orientation_.length() - Scalar(1)));
	return orientation_;
}

inline bool FinderPattern::isNormalReflectance() const
{
	return centerIntensity_ <= grayThreshold_;
}

inline bool FinderPattern::comesBefore(const FinderPattern& first, const FinderPattern& second)
{
	return first.position().y() > second.position().y() || (first.position().y() == second.position().y() && first.position().x() > second.position().x());
};

inline FinderPatternDetector::TransitionHistory::TransitionHistory()
	: deltas_{ 0, 0, 0, 0, 0 }
{
	// Nothing else to do.
}

inline int FinderPatternDetector::TransitionHistory::history1()
{
	return deltas_[0];
}

inline int FinderPatternDetector::TransitionHistory::history2()
{
	return deltas_[0] + deltas_[1];
}

inline int FinderPatternDetector::TransitionHistory::history3()
{
	return deltas_[0] + deltas_[1] + deltas_[2];
}

inline int FinderPatternDetector::TransitionHistory::history4()
{
	return deltas_[0] + deltas_[1] + deltas_[2] + deltas_[3];
}

inline int FinderPatternDetector::TransitionHistory::history5()
{
	return deltas_[0] + deltas_[1] + deltas_[2] + deltas_[3] + deltas_[4];
}

inline void FinderPatternDetector::TransitionHistory::push(const int newDelta)
{
	deltas_[4] = deltas_[3];
	deltas_[3] = deltas_[2];
	deltas_[2] = deltas_[1];
	deltas_[1] = deltas_[0];
	deltas_[0] = newDelta;
}

inline void FinderPatternDetector::TransitionHistory::reset()
{
	deltas_[0] = 0;
	deltas_[1] = 0;
	deltas_[2] = 0;
	deltas_[3] = 0;
	deltas_[4] = 0;
}

inline bool FinderPatternDetector::isTransitionToBlack(const uint8_t* pixel, TransitionHistory& history)
{
	const int currentDelta = int(*(pixel + 0) - *(pixel - 1));

	bool result = false;

	if (currentDelta < -deltaThreshold)
	{
		result = true;
	}
	else if ((currentDelta + history.history1() < -deltaThreshold)
		|| (currentDelta + history.history2() < -(deltaThreshold * 5 / 4))
		|| (currentDelta + history.history3() < -(deltaThreshold * 6 / 4))
		|| (currentDelta + history.history4() < -(deltaThreshold * 7 / 4))
		|| (currentDelta + history.history5() < -(deltaThreshold * 8 / 4)))
	{
		result = true;
	}

	history.push(currentDelta);

	return result;
}

inline bool FinderPatternDetector::isTransitionToWhite(const uint8_t* pixel, TransitionHistory& history)
{
	const int currentDelta = int(*(pixel + 0) - *(pixel - 1));

	bool result = false;

	if (currentDelta > deltaThreshold)
	{
		result = true;
	}
	else if ((currentDelta + history.history1() > deltaThreshold)
		|| (currentDelta + history.history2() > (deltaThreshold * 5 / 4))
		|| (currentDelta + history.history3() > (deltaThreshold * 6 / 4))
		|| (currentDelta + history.history4() > (deltaThreshold * 7 / 4))
		|| (currentDelta + history.history5() > (deltaThreshold * 8 / 4)))
	{
		result = true;
	}

	history.push(currentDelta);

	return result;
}

inline unsigned int FinderPatternDetector::determineThreshold(const uint8_t* yPosition, const unsigned int segmentSize1, const unsigned int segmentSize2, const unsigned int segmentSize3, const unsigned int segmentSize4, const unsigned int segmentSize5)
{
	unsigned int sumBlack = 0u;
	unsigned int sumWhite = 0u;

	sumWhite += *(yPosition - 1);

	for (unsigned int n = 0u; n < segmentSize1; ++n)
	{
		sumBlack += *yPosition++;
	}

	for (unsigned int n = 0u; n < segmentSize2; ++n)
	{
		sumWhite += *yPosition++;
	}

	for (unsigned int n = 0u; n < segmentSize3; ++n)
	{
		sumBlack += *yPosition++;
	}

	for (unsigned int n = 0u; n < segmentSize4; ++n)
	{
		sumWhite += *yPosition++;
	}

	for (unsigned int n = 0u; n < segmentSize5; ++n)
	{
		sumBlack += *yPosition++;
	}

	sumWhite += *yPosition;

	const unsigned int averageBlack = sumBlack / (segmentSize1 + segmentSize3 + segmentSize5);
	const unsigned int averageWhite = sumWhite / (segmentSize2 + segmentSize4 + 2u);

	if (averageBlack + 2u >= averageWhite)
	{
		// the separate between bright and dark pixels is not strong enough
		return (unsigned int)(-1);
	}

	return (averageBlack + averageWhite + 1u) / 2u;
}

inline bool FinderPatternDetector::isParallel(const FinderPattern& finderPatternA, const FinderPattern& finderPatternB, const Scalar distanceTolerance)
{
	ocean_assert(finderPatternA.cornersKnown() && finderPatternB.cornersKnown());
	ocean_assert(finderPatternB.corners() != nullptr && finderPatternA.corners() != nullptr);
	ocean_assert(distanceTolerance >= 0);

	const Line2 linesB[4] =
	{
		Line2(finderPatternB.corners()[1], (finderPatternB.corners()[0] - finderPatternB.corners()[1]).normalized()),
		Line2(finderPatternB.corners()[2], (finderPatternB.corners()[1] - finderPatternB.corners()[2]).normalized()),
		Line2(finderPatternB.corners()[3], (finderPatternB.corners()[2] - finderPatternB.corners()[3]).normalized()),
		Line2(finderPatternB.corners()[0], (finderPatternB.corners()[3] - finderPatternB.corners()[0]).normalized())
	};

	const Vector2 lineAB = finderPatternB.position() - finderPatternA.position();
	const Vector2 directionAB = lineAB.normalizedOrZero();

	const Scalar squareDistanceThreshold = (lineAB.length() * distanceTolerance) * (lineAB.length() * distanceTolerance);

	for (unsigned int n = 0u; n < 4u; ++n)
	{
		// Reject pairs of lines diverge too much
		if (Numeric::abs(directionAB * linesB[n].direction()) <= Numeric::cos(Numeric::deg2rad(35)))
		{
			continue;
		}

		// Check if:
		//
		// * the corners `i` and `(i+1)` of finder pattern a are both "close enough" to the n-th line of finder pattern b, i.e., is the line between corners `i` and `(i+1)` roughly parallel to the n-th line of finder pattern b.
		// * the opposite corners in finder pattern a, `(i+2) % 4` and `(i+3) % 4`, and line opposite to the n-th line in finder pattern b (n + 2 % 4) are roughly parallel as well.
		//
		// If both is true, finder patterns a and b are considered parallel.

		const Scalar sqrDistanceCornerA0 = linesB[n].sqrDistance(finderPatternA.corners()[0]);
		const Scalar sqrDistanceCornerA1 = linesB[n].sqrDistance(finderPatternA.corners()[1]);

		if (sqrDistanceCornerA0 < squareDistanceThreshold && sqrDistanceCornerA1 < squareDistanceThreshold)
		{
			if (linesB[(n + 2u) & 0b0011u].sqrDistance(finderPatternA.corners()[2]) < squareDistanceThreshold && linesB[(n + 2u) & 0b0011u].sqrDistance(finderPatternA.corners()[3]) < squareDistanceThreshold)  // (n + 2u) & 0b0011u == (n + 2u) % 4
			{
				return true;
			}
		}

		const Scalar sqrDistanceCornerA2 = linesB[n].sqrDistance(finderPatternA.corners()[2]);

		if (sqrDistanceCornerA1 < squareDistanceThreshold && sqrDistanceCornerA2 < squareDistanceThreshold)
		{
			if (linesB[(n + 2u) & 0b0011u].sqrDistance(finderPatternA.corners()[3]) < squareDistanceThreshold && linesB[(n + 2u) & 0b0011u].sqrDistance(finderPatternA.corners()[0]) < squareDistanceThreshold) // (n + 2u) & 0b0011u == (n + 2u) % 4
			{
				return true;
			}
		}

		const Scalar sqrDistanceCornerA3 = linesB[n].sqrDistance(finderPatternA.corners()[3]);

		if (sqrDistanceCornerA2 < squareDistanceThreshold && sqrDistanceCornerA3 < squareDistanceThreshold)
		{
			if (linesB[(n + 2u) & 0b0011u].sqrDistance(finderPatternA.corners()[0]) < squareDistanceThreshold && linesB[(n + 2u) & 0b0011u].sqrDistance(finderPatternA.corners()[1]) < squareDistanceThreshold) // (n + 2u) & 0b0011u == (n + 2u) % 4
			{
				return true;
			}
		}

		if (sqrDistanceCornerA3 < squareDistanceThreshold && sqrDistanceCornerA0 < squareDistanceThreshold)
		{
			if (linesB[(n + 2u) & 0b0011u].sqrDistance(finderPatternA.corners()[1]) < squareDistanceThreshold && linesB[(n + 2u) & 0b0011u].sqrDistance(finderPatternA.corners()[2]) < squareDistanceThreshold) // (n + 2u) & 0b0011u == (n + 2u) % 4
			{
				return true;
			}
		}
	}

	return false;
}

inline bool FinderPatternDetector::isDiagonal(const FinderPattern& finderPatternA, const FinderPattern& finderPatternB, const Scalar angleTolerance)
{
	ocean_assert(finderPatternA.cornersKnown() && finderPatternB.cornersKnown());
	ocean_assert(finderPatternB.corners() != nullptr && finderPatternA.corners() != nullptr);
	ocean_assert(angleTolerance >= 0 && angleTolerance < Numeric::deg2rad(90));

	const Vector2 directionAB = (finderPatternB.position() - finderPatternA.position()).normalizedOrZero();
	const Scalar angleThreshold = Numeric::abs(Numeric::cos(angleTolerance));

	const Vector2 diagonalsA[2] =
	{
		(finderPatternA.corners()[2] - finderPatternA.corners()[0]).normalizedOrZero(),
		(finderPatternA.corners()[3] - finderPatternA.corners()[1]).normalizedOrZero()
	};

	unsigned int diagonalEdgeA = (unsigned int)(-1);
	unsigned int diagonalEdgeB = (unsigned int)(-1);

	if (Numeric::abs(diagonalsA[0] * directionAB) >= angleThreshold)
	{
		diagonalEdgeA = 0u;
	}
	else if (Numeric::abs(diagonalsA[1] * directionAB) >= angleThreshold)
	{
		diagonalEdgeA = 1u;
	}

	if (diagonalEdgeA >= 2u)
	{
		return false;
	}

	const Vector2 diagonalsB[2] =
	{
		(finderPatternB.corners()[2] - finderPatternB.corners()[0]).normalizedOrZero(),
		(finderPatternB.corners()[3] - finderPatternB.corners()[1]).normalizedOrZero()
	};

	if (Numeric::abs(diagonalsB[0] * directionAB) >= angleThreshold)
	{
		diagonalEdgeB = 0u;
	}
	else if (Numeric::abs(diagonalsB[1] * directionAB) >= angleThreshold)
	{
		diagonalEdgeB = 1u;
	}

	return diagonalEdgeA < 2u && diagonalEdgeB < 2u;
}

} // namespace QRCodes

} // namespace Detector

} // namespace CV

} // namespace Ocean
