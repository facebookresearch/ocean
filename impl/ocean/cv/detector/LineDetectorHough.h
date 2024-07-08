/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_DETECTOR_LINE_DETECTOR_HOUGH_H
#define META_OCEAN_CV_DETECTOR_LINE_DETECTOR_HOUGH_H

#include "ocean/cv/detector/Detector.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Singleton.h"
#include "ocean/base/Worker.h"

#include "ocean/math/FiniteLine2.h"
#include "ocean/math/Line2.h"
#include "ocean/math/Vector2.h"

#include <algorithm>
#include <tuple>

namespace Ocean
{

// Forward declaration for test library.
namespace Test { namespace TestCV { namespace TestDetector { class TestLineDetectorHough; } } }

namespace CV
{

namespace Detector
{

/**
 * This class implements a line detector mainly based on the Hough transformation.
 * Beware: The origin of the resulting infinite line's coordinate system is located in the center of the frame.
 * @see InfiniteLine::cornerAlignedLine().
 * @ingroup cvdetector
 */
class OCEAN_CV_DETECTOR_EXPORT LineDetectorHough
{
	friend class Test::TestCV::TestDetector::TestLineDetectorHough;

	public:

		/**
		 * This class defines an infinite 2D line.
		 * Beware: The origin of the line's coordinate system is located in the center of the frame.
		 * @see cornerAlignedLine().
		 */
		class OCEAN_CV_DETECTOR_EXPORT InfiniteLine : public Line2
		{
			public:

				/**
				 * Creates an empty line object.
				 */
				InfiniteLine() = default;

				/**
				 * Creates a new line object.
				 * @param normal Line normal, must be valid
				 * @param angle The angle of the line (matching with the normal) in radian, with range [-PI, PI]
				 * @param distance Line distance between to the origin (the center of the frame - not the corner of the frame), with range (-infinity, infinity)
				 * @param strength The strength value, with range (0, infinity)
				 */
				inline InfiniteLine(const Vector2& normal, const Scalar angle, const Scalar distance, const Scalar strength);

				/**
				 * Converts this line (with origin defined in the center of the frame) to a line with origin defined in the upper left (or lower left) corner of the frame (depending on the pixel origin of the original frame).
				 * @param width The width of the frame which has been used to detect the line in pixel, with range [3, infinity)
				 * @param height The height of the frame which has been used to detect the line in pixel, with range [3, infinity)
				 * @return The line defined in the coordinate of the frame, with origin at one of the frame's corners
				 * @see cornerAlignedLines().
				 */
				inline Line2 cornerAlignedLine(const unsigned int width, const unsigned int height) const;

				/**
				 * Returns the normal of this line.
				 * @return Line normal
				 */
				inline const Vector2& normal() const;

				/**
				 * Returns the angle of this line.
				 * @return The line's angle in radian with range [-PI, PI]
				 */
				inline Scalar angle() const;

				/**
				 * Returns the distance of this line.
				 * @return The lines's distance to the origin (the center of the frame - not the corner of the frame), with range (-infinity, infinity)
				 * @see cornerAlignedLine().
				 */
				inline Scalar distance() const;

				/**
				 * Returns the strength of this line.
				 * @return Line's strength, with range (0, infinity)
				 */
				inline Scalar strength() const;

				/**
				 * Returns whether two lines are parallel up to a given angle precision.
				 * @param line The second line to check
				 * @param cosAngle Cosine value of the maximal angle to count as parallel, with range [0, 1]
				 * @return True, if so
				 */
				bool isParallel(const InfiniteLine& line, const Scalar cosAngle) const;

				/**
				 * Returns whether two lines are similar up to a given distance and angle precision.
				 * @param line Second line to check
				 * @param distance Maximal distance to count as similar
				 * @param cosAngle Cosine value of the maximal angle to count as similar
				 * @param halfOrientationPrecision True, to handle flipped lines (e.g. with angle 45 deg and -135) as identical lines
				 * @return True, if so
				 */
				bool isSimilar(const InfiniteLine& line, const Scalar distance, const Scalar cosAngle, const bool halfOrientationPrecision) const;

				/**
				 * Returns whether this line object has a lower strength value than the second one.
				 * @param second The second line object
				 * @return True, if so
				 */
				inline bool operator<(const InfiniteLine& second) const;

				/**
				 * Converts lines (with origin defined in the center of the frame) to a lines with origin defined in the upper left (or lower left) corner of the frame (depending on the pixel origin of the original frame).
				 * @param lines The lines to be converted, may be nullptr if size is 0
				 * @param size The number of lines to be converted, with range [0, infinity)
				 * @param width The width of the frame which has been used to detect the line in pixel, with range [3, infinity)
				 * @param height The height of the frame which has been used to detect the line in pixel, with range [3, infinity)
				 * @param strengths Optional resulting strength values of the converted lines, one strength value for each line
				 * @return The lines defined in the coordinate of the frame, with origin at one of the frame's corners
				 * @see cornerAlignedLine().
				 */
				static inline Lines2 cornerAlignedLines(const InfiniteLine* lines, const size_t size, const unsigned int width, const unsigned int height, Scalar* strengths = nullptr);

			protected:

				/// Line normal.
				Vector2 normal_ = Vector2(0, 0);

				/// Line angle.
				Scalar angle_ = Scalar(0);

				/// Line distance.
				Scalar distance_ = Scalar(0);

				/// Line strength.
				Scalar strength_ = Scalar(0);
		};

		/**
		 * Definition of a vector holding infinite lines.
		 */
		typedef std::vector<InfiniteLine> InfiniteLines;

		/**
		 * Definition of a vector holding infinite lines.
		 */
		typedef std::vector<InfiniteLines> InfiniteLineGroups;

	protected:

		/**
		 * Vote accumulator array.
		 */
		class OCEAN_CV_DETECTOR_EXPORT Accumulator
		{
			friend class Test::TestCV::TestDetector::TestLineDetectorHough;

			public:

				/**
				 * This class holds angle lookup data.
				 */
				class AngleLookupData
				{
					public:

						/**
						 * Creates an empty lookup object.
						 */
						AngleLookupData() = default;

						/**
						 * Creates a new lookup object.
						 * @param angleBin The angle bin to be stored
						 * @param weight The weight to be stored
						 */
						inline AngleLookupData(const unsigned int angleBin, const unsigned int weight);

					public:

						/// Bin of the associated angle.
						unsigned int angleBin_ = (unsigned int)(-1);

						/// Weight value.
						unsigned int weight_ = 0u;
				};

				/**
				 * This class holds distance lookup data.
				 */
				class DirectionLookupData
				{
					public:

						/**
						 * Creates an empty lookup object.
						 */
						DirectionLookupData() = default;

						/**
						 * Creates a new lookup object.
						 * @param dx Direction for the x-axis
						 * @param dy Direction for the y-axis
						 */
						inline DirectionLookupData(const int dx, const int dy);

					public:

						/// Direction of the normal for the x axis.
						int directionX_ = 0;

						/// Direction of the normal for the y axis.
						int directionY_ = 0;
				};

				/**
				 * This class defines a data lookup manager defined as singleton.
				 */
				class OCEAN_CV_DETECTOR_EXPORT LookupManager : public Singleton<LookupManager>
				{
					friend class Singleton<LookupManager>;
					friend class Accumulator;

					private:

						/**
						 * Definition of a pair combining an unsigned integer with a boolean state.
						 */
						typedef std::pair<unsigned int, bool> MapPair;

						/**
						 * Definition of a tuple combining angleBins, distanceBins and halfOrientationPrecision.
						 */
						typedef std::tuple<unsigned int, unsigned int, bool> MapTriple;

						/**
						 * Definition of a map mapping precision values to angle lookup data.
						 */
						typedef std::map<MapPair, AngleLookupData*> AngleLookupMap;

						/**
						 * Definition of a map mapping precision values to direction lookup data.
						 */
						typedef std::map<MapTriple, DirectionLookupData*> DirectionLookupMap;

					public:

						/**
						 * Returns the angle lookup data for 8 bit horizontal and vertical response values.
						 * The value request is done by an 16 bit index composed of the horizontal and vertical response.
						 * @param angleBins Number of angle bins to return the lookup data for
						 * @param halfOrientationPrecision True, to handle flipped lines (e.g. with angle 45 deg and -135) as identical lines
						 * @return Array holding the lookup data
						 */
						const AngleLookupData* angleLookupData8BitResponse16BitRequest(const unsigned int angleBins, const bool halfOrientationPrecision);

						/**
						 * Returns the angle lookup data for 8 bit diagonal (45 and 135 degree) response values.
						 * The value request is done by an 16 bit index composed of the horizontal and vertical response.
						 * @param angleBins Number of angle bins to return the lookup data for
						 * @param halfOrientationPrecision True, to handle flipped lines (e.g. with angle 45 deg and -135) as identical lines
						 * @return Array holding the lookup data
						 */
						const AngleLookupData* angleLookupDataDiagonal8BitResponse16BitRequest(const unsigned int angleBins, const bool halfOrientationPrecision);

						/**
						 * Returns the direction lookup data for an angle request.
						 * The size of the entire lookup buffer depends on the angle precision (on the number of used angle bins).
						 * @param angleBins Number of angle bins to return the lookup data for
						 * @param distanceBins Number of distance bins to return the lookup data for
						 * @param halfOrientationPrecision True, to handle flipped lines (e.g. with angle 45 deg and -135) as identical lines
						 * @return Array holding the lookup data
						 */
						const DirectionLookupData* directionLookupData(const unsigned int angleBins, const unsigned int distanceBins, const bool halfOrientationPrecision);

					private:

						/**
						 * Destructs a manager.
						 */
						~LookupManager();

					private:

						/// Lookup map for angles, horizontal and vertical.
						AngleLookupMap angleLookupMap_;

						/// Lookup map for angles, diagonal.
						AngleLookupMap angleLookupMapDiagonal_;

						/// Lookup map for directions.
						DirectionLookupMap directionLookupMap_;

						/// The manager's look.
						Lock lock_;
				};

			public:

				/**
				 * Creates a new vote element by the given precisions for distance and angle.
				 * The specified number of additional border bins is internally multiplied by two to cover additional top and bottom bins.
				 * @param width The width of the original image in pixel, with range [3, infinity)
				 * @param height The height of the original image in pixel, with range [3, infinity)
				 * @param distanceBins Number of distance bins used for line extraction, must be odd [-maxDistance | 0 | +maxDistance]
				 * @param angleBins Number of difference angle values can be considered, must be even (-90 deg, +90 deg]
				 * @param mirroredAngleBins Number of additional angle bins (at the top and bottom of the core accumulator) to simplify border handling, with range [0, angleBins / 2)
				 * @param halfOrientationPrecision True, to handle flipped lines (e.g. with angle 45 deg and -135) as identical lines
				 */
				Accumulator(const unsigned int width, const unsigned int height, const unsigned int distanceBins, const unsigned int angleBins, const unsigned int mirroredAngleBins, const bool halfOrientationPrecision);

				/**
				 * Adds a new horizontal and vertical edge filter response to accumulate the corresponding vote.
				 * @param x Horizontal filter position, with range [0, width())
				 * @param y Vertical filter position, with range [0, height())
				 * @param responses Two responses values (first horizontal, second vertical in address space) to perform a lookup for
				 * @param angleNeigbors Number of neighbors (in each angle direction - so it is more a less a radius) additionally receiving a less weighted vote, with range [0, angleBins)
				 */
				void accumulate(const unsigned int x, const unsigned int y, const int8_t* responses, const unsigned int angleNeigbors = 3u);

				/**
				 * Adds a new diagonal (45 degree and 135 degree) filter edge filter response to accumulate the corresponding vote.
				 * @param x Horizontal filter position, with range [0, width())
				 * @param y Vertical filter position, with range [0, height())
				 * @param responsesDiagonal Two responses values (first 45, second 135 in address space) to perform a lookup for
				 * @param angleNeigbors Number of neighbors (in each angle direction - so it is more a less a radius) additionally receiving a less weighted vote, with range [0, angleBins)
				 */
				void accumulateDiagonal(const unsigned int x, const unsigned int y, const int8_t* responsesDiagonal, const unsigned int angleNeigbors = 3u);

				/**
				 * Clears the accumulation buffer.
				 */
				void clear();

				/**
				 * Returns the width of the original image in pixel.
				 * @return Original image width, with range [3, infinity)
				 */
				inline unsigned int width() const;

				/**
				 * Returns the height of the original image in pixel.
				 * @return Original image height, with range [3, infinity)
				 */
				inline unsigned int height() const;

				/**
				 * Returns the distance precision of this accumulator.
				 * @return Number of bins used to represent all distance values
				 */
				inline unsigned int distanceBins() const;

				/**
				 * Returns the number of bins this accumulator stores for angle votes (including the additional bins for border operations).
				 * @return Number of bins used to represent all angle values
				 */
				inline unsigned int angleBins() const;

				/**
				 * Returns the angle precision of this accumulator.
				 * @return Number of bins used to represent all angle values
				 */
				inline unsigned int angleBinsCore() const;

				/**
				 * Returns the additional angle bins of this accumulator.
				 * @return Number of additional angle bins
				 */
				inline unsigned int mirroredAngleBins() const;

				/**
				 * Returns the vote buffer stored for this accumulator.
				 * @return Vote buffer
				 */
				inline const uint32_t* votes() const;

				/**
				 * Creates the additional mirrored angle bins at the top and bottom of the accumulator frame.
				 * These additional rows simplify the border operations.
				 */
				void createMirroredAngleBins();

				/**
				 * Detects peaks inside the accumulator votes.
				 * A vote is accepted as candidate (with following a non-maximum-suppression check) if the following holds: vote >= voteThreshold.
				 * @param lines Resulting lines
				 * @param voteThreshold The threshold of a vote so that a vote is accepted as candidate, with range [1, infinity)
				 * @param determineExactPeakMaximum True, to invoke the determination of the exact peak maximum by interpolation
				 * @param worker Optional worker object to distribute the computation
				 * @param smoothAccumulator True, to apply a smoothing filter (Gaussian) before peaks are detected
				 */
				void detectPeaks(InfiniteLines& lines, const unsigned int voteThreshold, const bool determineExactPeakMaximum, Worker* worker = nullptr, const bool smoothAccumulator = false);

				/**
				 * Detects peaks inside a subset of the accumulator votes using a surrounding window to determine the threshold for each pixel individually.
				 * A vote is accepted as candidate (with following a non-maximum-suppression check) if the following holds: vote >= adaptiveVoteThresholdFactor * averagedNeighborVotes.
				 * @param lines Resulting lines
				 * @param adaptiveVoteThresholdFactor The minimal factor between the actual vote and the averaged votes inside the surrounding window area so that a vote is accepted as candidate, with range (0, infinity)
				 * @param windowHalf Half of the size of the window for the adaptive threshold in pixel (window size = 2 * windowHalf + 1), with range [1, infinity)
				 * @param determineExactPeakMaximum True, to invoke the determination of the exact peak maximum by interpolation
				 * @param worker Optional worker object to distribute the computation
				 * @param smoothAccumulator True, to apply a smoothing filter (Gaussian) before peaks are detected
				 */
				void detectAdaptivePeaks(InfiniteLines& lines, const Scalar adaptiveVoteThresholdFactor, const unsigned int windowHalf, const bool determineExactPeakMaximum, Worker* worker = nullptr, const bool smoothAccumulator = false);

				/**
				 * Joins two accumulator objects.
				 * The result will be stored in the first accumulator.
				 * @param accumulators Two accumulators to join, must be valid
				 * @param worker Optional worker object to distribute the computation
				 */
				static void joinTwo(Accumulator* accumulators, Worker* worker = nullptr);

				/**
				 * Joins four accumulator objects.
				 * The result will be stored in the first accumulator.
				 * @param accumulators Four accumulators to join, must be valid
				 * @param worker Optional worker object to distribute the computation
				 */
				static void joinFour(Accumulator* accumulators, Worker* worker = nullptr);

				/**
				 * Joins an arbitrary number of accumulator objects.
				 * The result will be stored in the first accumulator.
				 * @param accumulators the accumulators to join, must be valid
				 * @param number The number of accumulators to join, with range [1, infinity)
				 * @param worker Optional worker object to distribute the computation
				 */
				static void join(Accumulator* accumulators, const unsigned int number, Worker* worker = nullptr);

				/**
				 * Returns whether this vote accumulator is valid.
				 * @return True, if so
				 */
				explicit inline operator bool() const;

			protected:

				/**
				 * Detects peaks inside a subset of the accumulator votes.
				 * A vote is accepted as candidate (with following a non-maximum-suppression check) if the following holds: vote >= voteThreshold.
				 * @param voteThreshold The threshold of a vote so that a vote is accepted as candidate, with range [1, infinity)
				 * @param determineExactPeakMaximum True, to invoke the determination of the exact peak maximum by interpolation
				 * @param lock Optional lock if this function is executed distributed within several threads
				 * @param lines The resulting lines
				 * @param firstAngleBin First angle bin to be checked, with range [0, angleBinsCore())
				 * @param numberAngleBins Number of angle bins to be checked, with range [1, angleBinsCore()]
				 */
				void detectPeaksSubset(const unsigned int voteThreshold, const bool determineExactPeakMaximum, Lock* lock, InfiniteLines* lines, const unsigned int firstAngleBin, const unsigned int numberAngleBins);

				/**
				 * Detects peaks inside a subset of the accumulator votes using a surrounding window to determine the threshold for each pixel individually.
				 * A vote is accepted as candidate (with following a non-maximum-suppression check) if the following holds: vote >= adaptiveVoteThresholdFactor * averagedNeighborVotes.
				 * @param borderedIntegralAccumulator Buffer of the bordered integral accumulator, must be valid
				 * @param adaptiveVoteThresholdFactor The minimal factor between the actual vote and the averaged votes inside the surrounding window area so that a vote is accepted as candidate, with range (0, infinity)
				 * @param windowHalf Half of the size of the window for the adaptive threshold in pixel (window size = 2 * windowHalf + 1), with range [1, infinity)
				 * @param determineExactPeakMaximum True, to invoke the determination of the exact peak maximum by interpolation
				 * @param lock Optional lock if this function is executed distributed within several threads
				 * @param lines The resulting lines
				 * @param firstAngleBin First angle bin to be checked, with range [0, angleBinsCore())
				 * @param numberAngleBins Number of angle bins to be checked, with range [1, angleBinsCore()]
				 */
				void detectAdaptivePeaksSubset(const uint32_t* borderedIntegralAccumulator, const Scalar adaptiveVoteThresholdFactor, const unsigned int windowHalf, const bool determineExactPeakMaximum, Lock* lock, InfiniteLines* lines, const unsigned int firstAngleBin, const unsigned int numberAngleBins);

				/**
				 * Joins a subset of two accumulator objects.
				 * The result will be stored in the first accumulator.
				 * @param accumulators Two accumulators to join
				 * @param firstAngleBin First angle bin to be joined
				 * @param numberAngleBins Number of angle bins to be joined
				 */
				static void joinTwo(Accumulator* accumulators, const unsigned int firstAngleBin, const unsigned int numberAngleBins);

				/**
				 * Joins a subset of four accumulator objects.
				 * The result will be stored in the first accumulator.
				 * @param accumulators Four accumulators to join, must be valid
				 * @param firstAngleBin First angle bin to be joined
				 * @param numberAngleBins Number of angle bins to be joined
				 */
				static void joinFour(Accumulator* accumulators, const unsigned int firstAngleBin, const unsigned int numberAngleBins);

				/**
				 * Joins a subset of an arbitrary number of accumulator objects.
				 * The result will be stored in the first accumulator.
				 * @param accumulators The accumulators to join, must be valid
				 * @param number The number of accumulators to join, with range [1, infinity)
				 * @param firstAngleBin First angle bin to be joined
				 * @param numberAngleBins Number of angle bins to be joined
				 */
				static void join(Accumulator* accumulators, const unsigned int number, const unsigned int firstAngleBin, const unsigned int numberAngleBins);

			protected:

				/// Array holding the individual votes.
				Frame accumulatorFrame_;

				/// Maximal line distance in pixel.
				int accumulatorMaximalDistance_ = 0;

				/// Half distance bins.
				int accumulatorDistanceBinsHalf_ = 0;

				/// Additional angle bins simplifying border operations.
				unsigned int accumulatorMirroredAngleBins_ = 0u;

				/// Width of the original image in pixel.
				const unsigned int accumulatorImageWidth_ = 0u;

				/// Height of the original image in pixel.
				const unsigned int accumulatorImageHeight_ = 0u;

				/// Half width of the original image in pixel.
				const unsigned int accumulatorImageWidthHalf_ = 0u;

				/// Half height of the original image in pixel.
				const unsigned int accumulatorImageHeightHalf_ = 0u;

				/// True, to handle flipped lines (e.g. with angle 45 deg and -135) as identical lines
				bool accumulatorHalfOrientationPrecision_ = false;

				/// Lookup table for angle data, horizontal and vertical.
				const AngleLookupData* angleLookupTable_ = nullptr;

				/// Lookup table for angle data, diagonal.
				const AngleLookupData* angleLookupTableDiagonal_ = nullptr;

				/// Lookup table for direction data.
				const DirectionLookupData* directionLookupTable_ = nullptr;

#ifdef OCEAN_DEBUG
				/// State to check whether the mirrored angle bins has been created before line detection
				bool debugMirroredAngleBinsCreated_ = false;
#endif
		};

		/**
		 * Definition of a vector holding index sets.
		 */
		typedef std::vector<IndexSet32> IndexSetVector;

	public:

		/**
		 * Definition of different edge detector filters.
		 */
		enum FilterType : uint32_t
		{
			// Invalid filter type.
			FT_INVALID = 0u,
			// Scharr filter.
			FT_SCHARR,
			// Sobel filter.
			FT_SOBEL
		};

		/**
		 * Definition of usage of different filter responses.
		 */
		enum FilterResponse : uint32_t
		{
			//  Invalid filter response.
			FR_INVALID = 0u,
			// Horizontal and vertical filter response (0 and 90 degrees).
			FR_HORIZONTAL_VERTICAL = 1u,
			// Diagonal filter response (45 and 135 degrees).
			FR_DIAGONAL = 2u,
			/// Horizontal, vertical (0 and 90 degrees) and diagonal (45 and 135 degrees).
			FR_HORIZONTAL_VERTICAL_DIAGONAL = FR_HORIZONTAL_VERTICAL | FR_DIAGONAL
		};

	public:

		/**
		 * Detects lines inside a given frame using a threshold ensuring that detected lines have a specific strength.
		 * Lines are detected by searching for all votes exceeding the given threshold.<br>
		 * All data channels of the frame will be used during the determination.
		 * @param frame The frame in which the lines will be detected, can be any zipped pixel format, all data channels will be used during the line detection, must be valid
		 * @param filterType Filter to be used for edge detection
		 * @param filterResponse Filter responses to be used
		 * @param infiniteLines Resulting infinite lines
		 * @param finiteLines Optional resulting finite lines, if specified
		 * @param optimizeLines True, to apply a fine adjustment of the lines after detection
		 * @param accumulatorThreshold Minimal threshold that an accumulated vote counts as line
		 * @param voteThreshold Minimal threshold a vote must exceed to forward a vote to the accumulator, with range [1, infinity)
		 * @param angleNeigbors Number of neighbors in angle direction receiving an less-weighted vote, with range [0, 11
		 * @param determineExactPeakMaximum True, to invoke the determination of the exact peak maximum by interpolation
		 * @param worker Optional worker object to distribute the computational load
		 * @param anglePrecision Number of detectable line angles, the higher the more precise the angle of the line, with range [1, 36000]
		 * @param distancePrecision Number of detectable distance values, the higher the more precise the distance of the line, use -1 to receive a distance precision of 1 pixel, with range (0, infinity) | [-1]
		 * @param halfOrientationPrecision True, to handle flipped lines (e.g. with angle 45 deg and -135) as identical lines
		 * @param similarDistance Maximal pixel distance for two lines to count as similar for filtering, 0 for non filtering
		 * @param similarAngle Maximal angle in radian for two lines to count as similar for filtering, 0 for non filtering
		 * @return True, if succeeded
		 */
		static inline bool detectLines(const Frame& frame, const FilterType filterType, const FilterResponse filterResponse, InfiniteLines& infiniteLines, FiniteLines2* finiteLines = nullptr, const bool optimizeLines = true, const unsigned int accumulatorThreshold = 100u, const unsigned int voteThreshold = 16u, const unsigned int angleNeigbors = 2u, const bool determineExactPeakMaximum = true, Worker* worker = nullptr, const unsigned int anglePrecision = 360u, const unsigned int distancePrecision = (unsigned int)(-1), const bool halfOrientationPrecision = true, const Scalar similarDistance = Scalar(10), const Scalar similarAngle = Numeric::deg2rad(5));

		/**
		 * Detects lines inside a given frame using an adaptive threshold in combination with a surrounding window.
		 * Lines are detected by searching for all votes exceeding a threshold relative to the surrounding vote area.<br>
		 * All data channels of the frame will be used during the determination.
		 * @param frame The frame in which the lines will be detected, can be any zipped pixel format, all data channels will be used during the line detection, must be valid
		 * @param filterType Filter to be used for edge detection
		 * @param filterResponse Filter responses to be used
		 * @param infiniteLines Resulting infinite lines
		 * @param finiteLines Optional resulting finite lines, if specified
		 * @param optimizeLines True, to apply a fine adjustment of the lines after detection
		 * @param adaptiveVoteThresholdFactor The minimal factor between the actual vote and the averaged votes inside the surrounding window area so that a vote is accepted as candidate, with range (0, infinity)
		 * @param thresholdWindow Size of the surrounding vote area considering for line detection, with range [5, infinity), must be odd
		 * @param voteThreshold Minimal threshold a vote must exceed to forward a vote to the accumulator
		 * @param angleNeigbors Number of neighbors in angle direction receiving an less-weighted vote
		 * @param determineExactPeakMaximum True, to invoke the determination of the exact peak maximum by interpolation
		 * @param worker Optional worker object to distribute the computational load
		 * @param anglePrecision Number of detectable line angles, the higher the more precise the angle of the line, with range [1, 36000]
		 * @param distancePrecision Number of detectable distance values, the higher the more precise the distance of the line, use -1 to receive a distance precision of 1 pixel, with range (0, infinity) | [-1]
		 * @param halfOrientationPrecision True, to handle flipped lines (e.g. with angle 45 deg and -135) as identical lines
		 * @param similarDistance Maximal pixel distance for two lines to count as similar for filtering, 0 for non filtering
		 * @param similarAngle Maximal angle in radian for two lines to count as similar for filtering, 0 for non filtering
		 * @return True, if succeeded
		 */
		static inline bool detectLinesWithAdaptiveThreshold(const Frame& frame, const FilterType filterType, const FilterResponse filterResponse, InfiniteLines& infiniteLines, FiniteLines2* finiteLines = nullptr, const bool optimizeLines = true, const Scalar adaptiveVoteThresholdFactor = Scalar(8), const unsigned int thresholdWindow = 61u, const unsigned int voteThreshold = 16, const unsigned int angleNeigbors = 2u, const bool determineExactPeakMaximum = true, Worker* worker = nullptr, const unsigned int anglePrecision = 360u, const unsigned int distancePrecision = (unsigned int)(-1), const bool halfOrientationPrecision = true, const Scalar similarDistance = Scalar(10), const Scalar similarAngle = Numeric::deg2rad(5));

		/**
		 * Filters a set of similar detected lines so that the strongest and unique lines are returned only.
		 * @param lines Detected lines that have to be filtered
		 * @param minDistance Minimal distance between two lines so that they do not count as identical
		 * @param minAngle Minimal angle between two lines so that they do not count as identical
		 * @param filteredLines Resulting subset of the original lines, filtered by the minimal distance and angle
		 * @param halfOrientationPrecision True, to handle flipped lines (e.g. with angle 45 deg and -135) as identical lines
		 */
		static void filterLines(const InfiniteLines& lines, const Scalar minDistance, const Scalar minAngle, InfiniteLines& filteredLines, const bool halfOrientationPrecision);

		/**
		 * Separates given lines in sets of almost parallel lines.
		 * @param lines The given lines to be separated
		 * @param maxAngle The maximal angle between two lines so that they count as parallel, with range [0, PI/2]
		 * @param parallelGroups The resulting groups of lines which are parallel
		 * @param minimalSetSize The minimal size of a group of parallel lines, with range [0, infinity)
		 * @param noDuplicates True, to avoid duplicates in the resulting groups
		 */
		static void parallelLines(const InfiniteLines& lines, const Scalar maxAngle, InfiniteLineGroups& parallelGroups, const unsigned int minimalSetSize = 0u, const bool noDuplicates = true);

		/**
		 * Filters the biggest set of parallel lines from a given set of lines.
		 * Further the resulting lines are sorted by their distance parameter.<br>
		 * @param lines Set of lines to be filtered for one set of parallel lines.
		 * @param minAngle Minimal angle between two successive lines in radian
		 * @param parallels Resulting parallel lines
		 * @return True, if succeeded
		 */
		static bool parallelLines(const InfiniteLines& lines, const Scalar minAngle, InfiniteLines& parallels);

		/**
		 * Sorts lines according to their distance values.
		 * @param lines The lines to be sorted
		 */
		static inline void sortLinesAccordingDistance(InfiniteLines& lines);

		/**
		 * Sorts groups of elements (of e.g., infinite lines) according to their number of elements in descending order.
		 * @param groups The groups to be ordered
		 * @tparam T The data type of the elements
		 */
		template <typename T>
		static inline void sortGroupsDescendingAccordingElements(std::vector<std::vector<T>>& groups);

	private:

		/**
		 * Internal line detection function to detects lines inside an 8 bit gray scale image.
		 * Beware: The origin of the pixel data is expected to be in the upper left corner.
		 * @param frame The frame in which the lines will be detected, can be any zipped pixel format, all data channels will be used during the line detection, must be valid
		 * @param filterType Filter to be used for edge detection
		 * @param filterResponse Filter responses to be used
		 * @param infiniteLines Resulting infinite lines
		 * @param finiteLines Optional resulting finite lines, if defined
		 * @param optimizeLines True, to apply a fine adjustment of the lines after detection
		 * @param thresholdParameter Depending on the defined border this threshold is interpreted as a unique vote threshold or an adaptive threshold ratio
		 * @param adaptiveThresholdWindowHalf The half size of the surrounding window for the adaptive threshold in pixel, 0 to avoid the adaptive thresholding, with range [0, infinity)
		 * @param voteThreshold Minimal threshold a vote must exceed to forward a vote to the accumulator
		 * @param angleNeigbors Number of neighbors in angle direction receiving an less-weighted vote
		 * @param determineExactPeakMaximum True, to invoke the determination of the exact peak maximum by interpolation
		 * @param worker Optional worker object to distribute the computational load
		 * @param anglePrecision Number of detectable line angles, the higher the more precise the angle of the line, with range [1, 36000]
		 * @param distancePrecision Number of detectable distance values, the higher the more precise the distance of the line, use -1 to receive a distance precision of 1 pixel, with range (0, infinity) | [-1]
		 * @param halfOrientationPrecision True, to handle flipped lines (e.g. with angle 45 deg and -135) as identical lines
		 * @param similarDistance Maximal pixel distance for two lines to count as similar for filtering, 0 for non filtering
		 * @param similarAngle Maximal angle in radian for two lines to count as similar for filtering, 0 for non filtering
		 * @return True, if succeeded
		 */
		static bool internalDetectLines(const Frame& frame, const FilterType filterType, const FilterResponse filterResponse, InfiniteLines& infiniteLines, FiniteLines2* finiteLines, const bool optimizeLines, const Scalar thresholdParameter, const unsigned int adaptiveThresholdWindowHalf, const unsigned int voteThreshold, const unsigned int angleNeigbors, const bool determineExactPeakMaximum, Worker* worker, const unsigned int anglePrecision, const unsigned int distancePrecision, const bool halfOrientationPrecision, const Scalar similarDistance, const Scalar similarAngle);

		/**
		 * Creates line votes for horizontal and vertical (0 and 90 degree) filter responses inside a given accumulator object.
		 * @param response Buffer holding the filter responses for the original frame
		 * @param accumulator The accumulator object receiving the votes
		 * @param angleNeigbors Number of neighbors in angle direction receiving an less-weighted vote
		 * @param voteThreshold Threshold a filter response must exceed to count as vote, with range [1, 127]
		 * @param firstRow First row of the responses buffer to be handled
		 * @param numberRows Number of rows of the response buffer to be handled
		 */
		static void createVotesHorizontalVerticalSubset(const int8_t* response, Accumulator* accumulator, const unsigned int angleNeigbors, const unsigned int voteThreshold, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Creates line votes for diagonal (45 and 135 degree) filter responses inside a given accumulator object.
		 * @param response Buffer holding the filter responses for the original frame
		 * @param accumulator The accumulator object receiving the votes
		 * @param angleNeigbors Number of neighbors in angle direction receiving an less-weighted vote
		 * @param voteThreshold Threshold a filter response must exceed to count as vote, with range [1, 127]
		 * @param firstRow First row of the responses buffer to be handled
		 * @param numberRows Number of rows of the response buffer to be handled
		 */
		static void createVotesDiagonalSubset(const int8_t* response, Accumulator* accumulator, const unsigned int angleNeigbors, const unsigned int voteThreshold, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Creates line votes for horizontal, vertical and diagonal (0, 90 and 45, 135 degree) filter responses inside a given accumulator object.
		 * @param response Buffer holding the filter responses for the original frame
		 * @param accumulator The accumulator object receiving the votes
		 * @param angleNeigbors Number of neighbors in angle direction receiving an less-weighted vote
		 * @param voteThreshold Threshold a filter response must exceed to count as vote, with range [1, 127]
		 * @param firstRow First row of the responses buffer to be handled
		 * @param numberRows Number of rows of the response buffer to be handled
		 */
		static void createVotesHorizontalVerticalDiagonalSubset(const int8_t* response, Accumulator* accumulator, const unsigned int angleNeigbors, const unsigned int voteThreshold, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Detects finite lines from a subset of already detected infinite lines additionally using the frame filter responses.
		 * @param infiniteLines Infinite lines to detect finite lines from
		 * @param response Buffer holding the filter responses for the original frame
		 * @param width The width of the original frame in pixel
		 * @param height The height of the original frame in pixel
		 * @param filterResponse Filter responses type which has been used to create the filter response
		 * @param angleBins Number of angle bins
		 * @param horizontalAngleLookup Horizontal angle lookup table
		 * @param diagonalAngleLookup Diagonal angle lookup table
		 * @param halfOrientationPrecision True, to handle flipped lines (e.g. with angle 45 deg and -135) as identical lines
		 * @param lock Optional lock if this function is executed distributed within several threads
		 * @param finiteLines Resulting finite lines
		 * @param firstLine First infinite line to be handled, with range [0, infiniteLines->size())
		 * @param numberLines Number of infinite lines to be handled, with range [1, infiniteLines->size() - firstLine]
		 */
		static void detectFiniteLinesSubset(const InfiniteLines* infiniteLines, const int8_t* response, const unsigned int width, const unsigned int height, const FilterResponse filterResponse, const unsigned int angleBins, const Accumulator::AngleLookupData* horizontalAngleLookup, const Accumulator::AngleLookupData* diagonalAngleLookup, const bool halfOrientationPrecision, Lock* lock, FiniteLines2* finiteLines, const unsigned int firstLine, const unsigned int numberLines);

		/**
		 * Detects finite lines from one infinite line additionally using the frame filter responses.
		 * @param infiniteLine Infinite line to detect finite lines from
		 * @param response Buffer holding the filter responses for the original frame
		 * @param width The width of the original frame in pixel
		 * @param height The height of the original frame in pixel
		 * @param filterResponse Filter responses type which has been used to create the filter response
		 * @param angleBins Number of angle bins
		 * @param horizontalAngleLookup Horizontal angle lookup table
		 * @param diagonalAngleLookup Diagonal angle lookup table
		 * @param halfOrientationPrecision True, to handle flipped lines (e.g. with angle 45 deg and -135) as identical lines
		 * @param finiteLines Resulting finite lines
		 */
		static void detectFiniteLines(const InfiniteLine& infiniteLine, const int8_t* response, const unsigned int width, const unsigned int height, const FilterResponse filterResponse, const unsigned int angleBins, const Accumulator::AngleLookupData* horizontalAngleLookup, const Accumulator::AngleLookupData* diagonalAngleLookup, const bool halfOrientationPrecision, FiniteLines2& finiteLines);

		/**
		 * Refines lines by adjusting the line with the filter responses.
		 * @param infiniteLines Lines to be adjusted
		 * @param number The number of given lines
		 * @param response Buffer holding the filter responses for the original frame
		 * @param width The width of the original frame in pixel
		 * @param height The height of the original frame in pixel
		 * @param filterResponse Filter responses type which has been used to create the filter response
		 * @param radius Search radius along the original line, in pixel
		 * @param accumulator Response accumulator that has been applied to fine the lines
		 * @param halfOrientationPrecision True, to handle flipped lines (e.g. with angle 45 deg and -135) as identical lines
		 * @param optimizedLines Resulting optimized lines
		 * @param firstLine First line to be optimized
		 * @param numberLines Number of lines to be optimized
		 */
		static void optimizeInfiniteLinesSubset(const InfiniteLine* infiniteLines, const size_t number, const int8_t* response, const unsigned int width, const unsigned int height, const FilterResponse filterResponse, const unsigned int radius, const Accumulator* accumulator, const bool halfOrientationPrecision, InfiniteLine* optimizedLines, const unsigned int firstLine, const unsigned int numberLines);

		/**
		 * Compares two index sets.
		 * @param first The first index set to compare
		 * @param second The second index set to compare
		 * @return True, if the first one holds lesser elements than the second one
		 */
		static inline bool compare(const IndexSet32& first, const IndexSet32& second);

		/**
		 * Compares to lines according to their distance.
		 * @param first The first line to compare
		 * @param second The second line to compare
		 * @return True, if the distance of the first one is lesser than that of the second one
		 */
		static inline bool compareDistance(const InfiniteLine& first, const InfiniteLine& second);

		/**
		 * Compares to groups of elements according to their size.
		 * @param first The first group of lines
		 * @param second The second groups of lines
		 * @return True, if the first group holds more elements than the second one
		 * @tparam T The data type of the elements
		 */
		template <typename T>
		static inline bool compareElements(const std::vector<T>& first, const std::vector<T>& second);
};

inline LineDetectorHough::InfiniteLine::InfiniteLine(const Vector2& normal, const Scalar angle, const Scalar distance, const Scalar strength) :
	Line2(normal * distance, normal.perpendicular()),
	normal_(normal),
	angle_(angle),
	distance_(distance),
	strength_(strength)
{
	ocean_assert(Numeric::isEqual(normal_.length(), 1));
	ocean_assert(Numeric::isInsideRange(-Numeric::pi(), angle, Numeric::pi()));
	ocean_assert(strength_ > Numeric::eps() );
}

inline Line2 LineDetectorHough::InfiniteLine::cornerAlignedLine(const unsigned int width, const unsigned int height) const
{
	ocean_assert(width >= 3u && height >= 3u);

	const Vector2 frameCenter(Scalar(width) * Scalar(0.5), Scalar(height) * Scalar(0.5));

	return Line2(frameCenter + point(), direction());
}

inline Lines2 LineDetectorHough::InfiniteLine::cornerAlignedLines(const InfiniteLine* lines, const size_t size, const unsigned int width, const unsigned int height, Scalar* strengths)
{
	ocean_assert(width >= 3u && height >= 3u);

	if (size == 0)
	{
		return Lines2();
	}

	ocean_assert(lines);

	Lines2 result;
	result.reserve(size);

	const Vector2 frameCenter(Scalar(width) * Scalar(0.5), Scalar(height) * Scalar(0.5));

	if (strengths)
	{
		for (size_t n = 0; n < size; ++n)
		{
			result.push_back(Line2(frameCenter + lines[n].point(), lines[n].direction()));
			strengths[n] = lines[n].strength();
		}
	}
	else
	{
		for (size_t n = 0; n < size; ++n)
		{
			result.push_back(Line2(frameCenter + lines[n].point(), lines[n].direction()));
		}
	}

	return result;
}

inline const Vector2& LineDetectorHough::InfiniteLine::normal() const
{
	return normal_;
}

inline Scalar LineDetectorHough::InfiniteLine::angle() const
{
	return angle_;
}

inline Scalar LineDetectorHough::InfiniteLine::distance() const
{
	return distance_;
}

inline Scalar LineDetectorHough::InfiniteLine::strength() const
{
	return strength_;
}

inline bool LineDetectorHough::InfiniteLine::operator<(const InfiniteLine& second) const
{
	return strength_ < second.strength_;
}

inline LineDetectorHough::Accumulator::AngleLookupData::AngleLookupData(const unsigned int angleBin, const unsigned int weight) :
	angleBin_(angleBin),
	weight_(weight)
{
	// nothing to do here
};

inline LineDetectorHough::Accumulator::DirectionLookupData::DirectionLookupData(const int dx, const int dy) :
	directionX_(dx),
	directionY_(dy)
{
	// nothing to do here
};

inline unsigned int LineDetectorHough::Accumulator::width() const
{
	return accumulatorImageWidth_;
}

inline unsigned int LineDetectorHough::Accumulator::height() const
{
	return accumulatorImageHeight_;
}

inline unsigned int LineDetectorHough::Accumulator::distanceBins() const
{
	return accumulatorFrame_.width();
}

inline unsigned int LineDetectorHough::Accumulator::angleBins() const
{
	return accumulatorFrame_.height();
}

inline unsigned int LineDetectorHough::Accumulator::angleBinsCore() const
{
	return accumulatorFrame_.height() - 2 * accumulatorMirroredAngleBins_;
}

inline unsigned int LineDetectorHough::Accumulator::mirroredAngleBins() const
{
	return accumulatorMirroredAngleBins_;
}

inline const uint32_t* LineDetectorHough::Accumulator::votes() const
{
	return accumulatorFrame_.constdata<unsigned int>();
}

inline LineDetectorHough::Accumulator::operator bool() const
{
	return accumulatorFrame_.width() >= 1 && accumulatorFrame_.height() >= 1;
}

inline void LineDetectorHough::sortLinesAccordingDistance(InfiniteLines& lines)
{
	std::sort(lines.begin(), lines.end(), compareDistance);
}

template <typename T>
inline void LineDetectorHough::sortGroupsDescendingAccordingElements(std::vector<std::vector<T>>& groups)
{
	std::sort(groups.begin(), groups.end(), compareElements<T>);
}

inline bool LineDetectorHough::compare(const IndexSet32& first, const IndexSet32& second)
{
	return first.size() > second.size();
}

inline bool LineDetectorHough::compareDistance(const InfiniteLine& first, const InfiniteLine& second)
{
	return first.distance() < second.distance();
}

template <typename T>
inline bool LineDetectorHough::compareElements(const std::vector<T>& first, const std::vector<T>& second)
{
	return first.size() > second.size();
}

inline bool LineDetectorHough::detectLines(const Frame& frame, const FilterType filterType, const FilterResponse filterResponse, InfiniteLines& infiniteLines, FiniteLines2* finiteLines, const bool optimizeLines, const unsigned int accumulatorThreshold, const unsigned int voteThreshold, const unsigned int angleNeigbors, const bool determineExactPeakMaximum, Worker* worker, const unsigned int anglePrecision, const unsigned int distancePrecision, const bool halfOrientationPrecision, const Scalar similarDistance, const Scalar similarAngle)
{
	ocean_assert(accumulatorThreshold > 0);
	ocean_assert(similarDistance >= 0);
	ocean_assert(similarAngle >= 0);

	return internalDetectLines(frame, filterType, filterResponse, infiniteLines, finiteLines, optimizeLines, Scalar(accumulatorThreshold), 0u, voteThreshold, angleNeigbors, determineExactPeakMaximum, worker, anglePrecision, distancePrecision, halfOrientationPrecision, similarDistance, similarAngle);
}

inline bool LineDetectorHough::detectLinesWithAdaptiveThreshold(const Frame& frame, const FilterType filterType, const FilterResponse filterResponse, InfiniteLines& infiniteLines, FiniteLines2* finiteLines, const bool optimizeLines, const Scalar thresholdRatio, const unsigned int thresholdWindow, const unsigned int voteThreshold, const unsigned int angleNeigbors, const bool determineExactPeakMaximum, Worker* worker, const unsigned int anglePrecision, const unsigned int distancePrecision, const bool halfOrientationPrecision, const Scalar similarDistance, const Scalar similarAngle)
{
	ocean_assert(thresholdRatio > 0 && thresholdWindow > 0u);
	ocean_assert(similarDistance >= 0);
	ocean_assert(similarAngle >= 0);

	ocean_assert(thresholdWindow >= 5u && (thresholdWindow % 2u) == 1u); // 5 as 3 is the non-suppression-area already

	const unsigned int adaptiveThresholdWindowHalf = thresholdWindow / 2u;

	return internalDetectLines(frame, filterType, filterResponse, infiniteLines, finiteLines, optimizeLines, thresholdRatio, adaptiveThresholdWindowHalf, voteThreshold, angleNeigbors, determineExactPeakMaximum, worker, anglePrecision, distancePrecision, halfOrientationPrecision, similarDistance, similarAngle);
}

}

}

}

#endif // META_OCEAN_CV_DETECTOR_LINE_DETECTOR_HOUGH_H
