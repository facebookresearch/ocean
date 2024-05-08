/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_DETECTOR_MESSENGER_CODE_DETECTOR_H
#define META_OCEAN_CV_DETECTOR_MESSENGER_CODE_DETECTOR_H

#include "ocean/cv/detector/Detector.h"

#include "ocean/base/Memory.h"

#include "ocean/cv/FrameInverter.h"

#include "ocean/math/Box2.h"
#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/Vector2.h"

#include <array>
#include <bitset>
#include <unordered_set>

namespace Ocean
{

namespace CV
{

namespace Detector
{

/**
 * This class implements a detector for circular Messenger Codes.
 * @ingroup cvdetector
 */
class OCEAN_CV_DETECTOR_EXPORT MessengerCodeDetector
{
	friend class QRCodeDetector;

	public:

		/**
		 * Definition of the number of bits the Messenger Code provides.
		 */
		static constexpr size_t numberCodeBits = 260;

		/**
		 * Definition of a bitset containing the information of a Messenger Code
		 */
		typedef std::bitset<numberCodeBits> CodeBits;

		/**
		 * Definition of a vector holding codes.
		 */
		typedef std::vector<CodeBits> Codes;

		/**
		 * Definition of an index quartet (an array with exactly four indices).
		 */
		typedef std::array<unsigned int, 4> IndexQuartet;

		/**
		 * Definition of a vector holding index quartets
		 */
		typedef std::vector<IndexQuartet> IndexQuartets;

		/**
		 * Definition of a class holding a bullseye composed of a location and a radius.
		 */
		class Bullseye
		{
			public:

				/**
				 * Creates an invalid bullseye object.
				 */
				inline Bullseye();

				/**
				 * Creates a new bullseye object by a given position and radius.
				 * @param position The (center) position of the bullseye within the camera frame
				 * @param radius The radius of the bullseye in pixels, with range (0, infinity)
				 * @param grayThreshold Threshold that was used during the detection, range [0, 255]
				 */
				inline Bullseye(const Vector2& position, const Scalar& radius, const unsigned int grayThreshold);

				/**
				 * Returns the (center) position of the bullseye.
				 * @return The Bullseye's position within the camera frame
				 */
				inline const Vector2& position() const;

				/**
				 * Returns the radius of the bullseye.
				 * @return The Bullseye's radius, with range (0, infinity), 0 for an invalid object
				 */
				inline Scalar radius() const;

				/**
				 * Returns the threshold that was used for the detection of this bullseye
				 * @return The threshold value
				 */
				inline unsigned int grayThreshold() const;

			protected:

				/// The (center) position of the bullseye within the camera frame.
				Vector2 position_;

				/// The radius of the bullseye in pixels, with range (0, infinity).
				Scalar radius_;

				/// The threshold that was used during the detection of this bullseye
				unsigned int grayThreshold_;
		};

		/**
		 * Definition of a vector holding bullseyes.
		 */
		typedef std::vector<Bullseye> Bullseyes;

		/**
		 * Data storing debug information.
		 */
		class OCEAN_CV_DETECTOR_EXPORT DebugInformation
		{
			public:

				/**
				 * Data of one detected code.
				 */
				struct DetectedCode
				{
					/// The four bullseyes
					Bullseyes bullseyes_;

					/// Location of the bits in frame coordinates
					Vectors2 codeBitsLocationFrame_;

					/// Extracted code bits
					CodeBits codebits_;
				};

				/**
				 * Definition of a vector holding DetectedCode object.
				 */
				typedef std::vector<DetectedCode> DetectedCodes;

			public:

				/**
				 * Draws the entire debug information into a given frame.
				 * @param frame The frame in which the debug information will be painted, must be valid
				 */
				void draw(Frame& frame) const;

				/**
				 * Draws the location of a bullseye into a given frame.
				 * @param frame The frame in which the bullseye will be painted, must be valid
				 * @param bullseye The bullseye to be painted
				 * @param color The color to be used, one value for each frame channel, must be valid
				 */
				static void drawBullseye(Frame& frame, const Bullseye& bullseye, const uint8_t* color);

			public:

				/// Contains information about all detected codes
				DetectedCodes detectedCodes_;

				/// Contains all detected bullseyes
				Bullseyes detectedBullseyes_;
		};

	protected:

		/// The intensity threshold between two successive pixels to count as a transition from black to white (or vice versa).
		static constexpr int deltaThreshold = 20;

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
				 * Returns the history with window size 1.
				 * @return The previous delta
				 */
				inline int history1();

				/**
				 * Returns the history with window size 2.
				 * @return The sum of the previous two deltas
				 */
				inline int history2();

				/**
				 * Returns the history with window size 3.
				 * @return The sum of the previous three deltas
				 */
				inline int history3();

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

				/// The previous delta.
				int deltaMinus1;

				/// The second previous delta.
				int deltaMinus2;

				/// The third previous delta.
				int deltaMinus3;
		};

	public:

		/**
		 * Detects Messenger Codes in a given 8 bit grayscale image.
		 * @param yFrame The 8 bit grayscale frame in which the Messenger code will be detected, with origin in the upper left corner, must be valid
		 * @param width The width of the given grayscale frame in pixel, with range [21, infinity)
		 * @param height The height of the given grayscale frame in pixel, with range [21, infinity)
		 * @param paddingElements Optional number of padding elements at the end of each image row, in elements, with range [0, infinity)
		 * @param worker Optional worker to distribute the computation
		 * @return The detected messenger codes
		 */
		static Codes detectMessengerCodes(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int paddingElements, Worker* worker = nullptr);

		/**
		 * Detects Messenger Codes in a given 8 bit grayscale image and returns debug information.
		 * @param yFrame The 8 bit grayscale frame in which the Messenger code will be detected, with origin in the upper left corner, must be valid
		 * @param width The width of the given grayscale frame in pixel, with range [21, infinity)
		 * @param height The height of the given grayscale frame in pixel, with range [21, infinity)
		 * @param debugInformation The resulting debug information for the given frame
		 * @param yFramePaddingElements The number of padding elements at the end of each image row, in elements, with range [0, infinity)
		 * @param worker Optional worker to distribute the computation
		 * @return The detected messenger codes
		 */
		static Codes detectMessengerCodesWithDebugInformation(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, DebugInformation& debugInformation, const unsigned int yFramePaddingElements, Worker* worker = nullptr);

	protected:

		/**
		 * Detects Messenger Codes in a given 8 bit grayscale image.
		 * @param yFrame The 8 bit grayscale frame in which the Messenger code will be detected, must be valid
		 * @param width The width of the given grayscale frame in pixel, with range [21, infinity)
		 * @param height The height of the given grayscale frame in pixel, with range [21, infinity)
		 * @param yFramePaddingElements The number of padding elements at the end of each image row, in elements, with range [0, infinity)
		 * @param debugInformation If specified, debug information will be stored here otherwise it will be ignored (note: `tCreateDebugInformation` must be set to `true` as well)
		 * @param worker Optional worker to distribute the computation
		 * @return The detected messenger codes
		 * @tparam tCreateDebugInformation If true, debug information will be created and returned in 'debugInformation' (note: `debugInformation` must be specified as well)
		 */
		template <bool tCreateDebugInformation>
		static Codes detectMessengerCodes(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, DebugInformation* debugInformation = nullptr, Worker* worker = nullptr);

		/**
		 * Detects Messenger Code bullseyes in a given 8 bit grayscale image.
		 * @param yFrame The 8 bit grayscale frame in which the bullseyes will be detected, must be valid
		 * @param width The width of the given grayscale frame in pixel, with range [21, infinity)
		 * @param height The height of the given grayscale frame in pixel, with range [21, infinity)
		 * @param yFramePaddingElements The number of padding elements at the end of each image row, in elements, with range [0, infinity)
		 * @param worker Optional worker to distribute the computation
		 * @return The detected bullseyes
		 */
		static inline Bullseyes detectBullseyes(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, Worker* worker = nullptr);

		/**
		 * Detects Messenger Code bullseyes in subset of a given 8 bit grayscale image.
		 * @param yFrame The 8 bit grayscale frame in which the bullseyes will be detected, must be valid
		 * @param width The width of the given grayscale frame in pixel, with range [21, infinity)
		 * @param height The height of the given grayscale frame in pixel, with range [21, infinity)
		 * @param bullseyes The resulting bullseyes, will be added to the end of the vector
		 * @param multiThreadLock Lock object in case this function is executed in multiple threads concurrently, otherwise nullptr
		 * @param yFramePaddingElements The number of padding elements at the end of each image row, in elements, with range [0, infinity)
		 * @param firstRow The first row to be handled, with range [10, height - 10)
		 * @param numberRows The number of rows to be handled, with range [1, height - 10 - firstRow]
		 */
		static void detectBullseyesSubset(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, Bullseyes* bullseyes, Lock* multiThreadLock, const unsigned int yFramePaddingElements, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Extracts quartets of code bullseyes from a given set of bullseyes.
		 * The indices of a resulting quartet provide the bullseyes in a counter-clock-wise order.
		 * Based on the given bullseyes, any combination of four bullseyes is returned which potentially can define a Messenger Code.
		 * @param bullseyes The center locations of the bullseyes that have been detected in one image, must be valid
		 * @param radii The radii of each individual bullseye, one for each bullseye, in pixel, must be valid
		 * @param size The number of given bullseyes, with range [4, infinity)
		 * @param radiusScaleTolerance The tolerance between radii of matching bullseyes in percent, with range [0, 0.5]
		 * @param distanceScaleTolerance The tolerance between radii of matching bullseyes in percent, with range [0, 3 - (2 * sqrt(2))]
		 * @return The quartets of indices of bullseyes valid candidates for a code
		 */
		static IndexQuartets extractCodeCandidates(const Vector2* bullseyes, const Scalar* radii, const size_t size, const Scalar radiusScaleTolerance = Scalar(0.35), const Scalar distanceScaleTolerance = Scalar(0.17));

		/**
		 * Calculates the homography rectifying the image content covered (and defined) by four bullseyes.
		 * The orientation of the resulting homography is undefined (in the rectified image, the Messenger Icon can be in any of the four quadrants).
		 * @param bullseyes The four bullseyes for which the homography will be determined, defined in counter-clockwise order, must provide four valid and individual locations of bullseyes, must be valid
		 * @param homography The resulting homography transforming image points defined in the rectified image to image points defined in the camera image: bullseyePoint = homography * rectifiedBullseyePoint
		 * @param codeSize The resulting size of the Messenger Code - the width (and height) of the rectified (sub-)frame containing the Messenger Code only, in pixel, with range (0, infinity)
		 * @return True, if the homography could be calculated
		 * @see correctRotation().
		 */
		static bool determineHomographyForBullseyeQuartet(const Vector2* bullseyes, SquareMatrix3& homography, Scalar& codeSize);

		/**
		 * Corrects the orientation of a given homography already rectifying the content of a Messenger Code.
		 * The homography will be rotated around the center point of the rectified sub-image so that the Messenger Icon is located in the lower right quadrant of the Messenger Code.<br>
		 * Depending on the input homography, this function either applies a 0-degree, 90-degree, 180-degree, or 270-degree orientation.
		 * @param yFrame The 8 bit gray image in which the Messenger Code is located, must be valid
		 * @param width The width of the given image in pixel, with range [1, infinity)
		 * @param height The height of the given image in pixel, with range [1, infinity)
		 * @param bullseyes The four bullseyes defining the Messenger Code, with counter-clock-wise order, all located inside the given image, must provide four valid and individual locations of bullseyes, must be valid
		 * @param codeSize The size of the rectified image containing the Messenger Code, in pixel, with range (0, infinity)
		 * @param homography The already known homography rectifying the image content, which will be adjusted if necessary, with: bullseyePoint = homography * rectifiedBullseyePoint, must not be singular
		 * @param yFramePaddingElements The number of padding elements at the end of each image row, in elements, with range [0, infinity)
		 * @return True, if succeeded
		 * @see determineHomographyForBullseyeQuartet().
		 */
		static bool correctRotation(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const Vector2* bullseyes, const Scalar codeSize, SquareMatrix3& homography, const unsigned int yFramePaddingElements);

		/**
		 * Returns whether a Messenger Code, defined by a rectifying homography and code size, is entirely visible in a camera frame.
		 * The bullseyes of a Messenger Code are slightly inside the Code so that an extract border is necessary to see the entire Code.
		 * @param width The width of the camera frame in pixel, with range [1, infinity)
		 * @param height The height of the camera frame in pixel, with range [1, infinity)
		 * @param homography The homography rectifying the image content of the Messenger Code, with transformation: bullseyePoint = homography * rectifiedBullseyePoint, must not be singular
		 * @param codeSize The size of the Messenger Code - the width (and height) of the rectified (sub-)frame containing the Messenger Code only, in pixel, with range (0, infinity)
		 * @return True, if the entire information (e.g., all bit rings) is visible in the camera frame
		 */
		static bool isCodeInsideFrame(const unsigned int width, const unsigned int height, const SquareMatrix3& homography, const Scalar codeSize);

		/**
		 * Extracts the Messenger Code's bit information.
		 * @param yFrame The 8 bit gray image in which the Messenger Code is located, must be valid
		 * @param width The width of the given image in pixel, with range [1, infinity)
		 * @param height The height of the given image in pixel, with range [1, infinity)
		 * @param codeSize The size of the Messenger Code - the width (and height) of the rectified (sub-)frame containing the Messenger Code only, in pixel, with range (0, infinity)
		 * @param homography The homography rectifying the image content of the Messenger Code, with transformation: bullseyePoint = homography * rectifiedBullseyePoint, must not be singular
		 * @param codeBits The resulting extracted bits of the Messenger Code
		 * @param grayThreshold Threshold that is used to determine if a bit is 0 or 1, range: [0, 255]
		 * @param yFramePaddingElements The number of padding elements at the end of each image row, in elements, with range [0, infinity)
		 * @param codeBitsLocationFrame If specified, the locations in frame coordinates of the code bits are written to this vector, otherwise it will be ignored (note: `tCreateDebugInformation` must be set to `true` as well)
		 * @return True, if succeeded
		 * @tparam tCreateDebugInformation If true, debug information will be created and returned in 'debugInformation' (note: `codeBitsLocationFrame` must be specified as well)
		 */
		template <bool tCreateDebugInformation>
		static bool extractCodeBits(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const Scalar codeSize, const SquareMatrix3& homography, CodeBits& codeBits, const unsigned int grayThreshold, const unsigned int yFramePaddingElements, Vectors2* codeBitsLocationFrame = nullptr);

		/**
		 * Detects Messenger Code bullseyes in a row of an grayscale image.
		 * @param yFrame The 8 bit grayscale frame in which the bullseyes will be detected, must be valid
		 * @param width The width of the given grayscale frame in pixel, with range [21, infinity)
		 * @param height The height of the given grayscale frame in pixel, with range [21, infinity)
		 * @param y The index of the row in which the bullseyes will be detected, with range [10, height - 11]
		 * @param bullseyes The resulting detected bullseyes, will be added to the end of the vector
		 * @param yFramePaddingElements The number of padding elements at the end of each image row, in elements, with range [0, infinity)
		 */
		static void detectBullseyesInRow(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int y, Bullseyes& bullseyes, const unsigned int yFramePaddingElements);

		/**
		 * Checks whether a column contains a bullseye at a specified location.
		 * This function is simply checking for the same bullseye pattern in vertical direction (within a small window).
		 * @param yFrame The 8 bit grayscale frame in which the bullseyes will be detected, must be valid
		 * @param frameStrideElements The number of stride elements in the given image (width + padding elements), in elements, with range [width, infinity)
		 * @param height The height of the given grayscale frame in pixel, with range [21, infinity)
		 * @param xCenter The horizontal location within the frame at which the existence of the bullseye will be checked, in pixels, with range [0, width - 1]
		 * @param yCenter The vertical location within the frame at which the existence of the bullseye will be checked, in pixels, with range [0, height - 1]
		 * @param threshold The grayscale threshold separating a bright pixel from a dark pixel, with range [0, 255]
		 * @param blackRingSegmentMin The minimal size (thickness) of the black ring, in pixel, with range [1, infinity)
		 * @param blackRingSegmentMax The maximal size (thickness) of the black ring, in pixel, with range [blackRingSegmentMin, infinity)
		 * @param whiteRingSegmentMin The minimal size (thickness) of the white ring, in pixel, with range [1, infinity)
		 * @param whiteRingSegmentMax The maximal size (thickness) of the white ring, in pixel, with range [whiteRingSegmentMin, infinity)
		 * @param dotSegmentMin The minimal size (thickness) of the black dot, in pixel, with range [1, infinity)
		 * @param dotSegmentMax The maximal size (thickness) of the black dot, in pixel, with range [dotSegmentMin, infinity)
		 * @return True, if the column contains a bullseye at the specified location
		 */
		static bool checkBullseyeInColumn(const uint8_t* const yFrame, const unsigned int frameStrideElements, const unsigned int height, const unsigned int xCenter, const unsigned int yCenter, const unsigned int threshold, const unsigned int blackRingSegmentMin, const unsigned int blackRingSegmentMax, const unsigned int whiteRingSegmentMin, const unsigned int whiteRingSegmentMax, const unsigned int dotSegmentMin, const unsigned int dotSegmentMax);

		/**
		 * Checks whether the direction neighborhood contains a bullseye at a specified location.
		 * This function actually samples the neighborhood at sparse locations only instead of applying a dense check for the bullseye pattern.
		 * @param yFrame The 8 bit grayscale frame in which the bullseyes will be detected, must be valid
		 * @param width The width oft he given frame in pixels, with range [21, infinity)
		 * @param height The height of the given frame in pixels, with range [21, infinity)
		 * @param xCenter The horizontal location within the frame at which the existence of the bullseye will be checked, in pixels, with range [0, width - 1]
		 * @param yCenter The vertical location within the frame at which the existence of the bullseye will be checked, in pixels, with range [0, height - 1]
		 * @param threshold The grayscale threshold separating a bright pixel from a dark pixel, with range [0, 255]
		 * @param whiteRingRadius The radius of the white ring (the center of this ring), in pixel, with range [1, infinity)
		 * @param blackRingRadius The radius of the black ring (the center of this ring), in pixel, with range [whiteRingRadius + 1, infinity)
		 * @param whiteBorderRadius The radius of the white border (the outer area around the black ring), in pixel, with range [blackRingRadius + 1u, infinity)
		 * @param yFramePaddingElements The number of padding elements at the end of each image row, in elements, with range [0, infinity)
		 * @return True, if the neighborhood contains a bullseye at the specified location
		 */
		static bool checkBullseyeInNeighborhood(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int xCenter, const unsigned int yCenter, const unsigned int threshold, const float whiteRingRadius, const float blackRingRadius, const float whiteBorderRadius, const unsigned int yFramePaddingElements);

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
		static unsigned int determineThreshold(const uint8_t* yPosition, const unsigned int segmentSize1, const unsigned int segmentSize2, const unsigned int segmentSize3, const unsigned int segmentSize4, const unsigned int segmentSize5);

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
		 * Finds either the next black or the next white pixel towards negative y direction (upwards in an image).
		 * @param yPointer The pointer to the location within the image at which the search will start, must be valid
		 * @param y The current vertical location within the image, with range [1, height - 1]
		 * @param maximalRows The maximal number of rows (vertical pixels) to travel, with range [1, height - y]
		 * @param threshold The threshold separating a bright pixel from a dark pixel, with range [0, 255]
		 * @param frameStrideElements The number of horizontal stride elements in the image, which is width + paddingElements, with range [width, infinity)
		 * @param rows The resulting number of rows that have been traveled until the black or white pixel has been found
		 * @return True, if the black or white pixel have been found within the specified range of [1, maximalRows]
		 * @tparam tFindBlackPixel True, to find the next black pixel; False, to find the next white pixel
		 */
		template <bool tFindBlackPixel>
		static bool findNextUpperPixel(const uint8_t* yPointer, const unsigned int y, const unsigned int maximalRows, const unsigned int threshold, const unsigned int frameStrideElements, unsigned int& rows);

		/**
		 * Finds either the next black or the next white pixel towards positive y direction (downwards in an image).
		 * @param yPointer The pointer to the location within the image at which the search will start, must be valid
		 * @param y The current vertical location within the image, with range [1, height - 1]
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param maximalRows The maximal number of rows (vertical pixels) to travel, with range [1, height - y]
		 * @param threshold The threshold separating a bright pixel from a dark pixel, with range [0, 255]
		 * @param frameStrideElements The number of horizontal stride elements in the image, which is width + paddingElements, with range [width, infinity)
		 * @param rows The resulting number of rows that have been traveled until the black or white pixel has been found
		 * @return True, if the black or white pixel have been found within the specified range of [1, maximalRows]
		 * @tparam tFindBlackPixel True, to find the next black pixel; False, to find the next white pixel
		 */
		template <bool tFindBlackPixel>
		static bool findNextLowerPixel(const uint8_t* yPointer, const unsigned int y, const unsigned int height, const unsigned int maximalRows, const unsigned int threshold, const unsigned int frameStrideElements, unsigned int& rows);

		/**
		 * Determines the sub-pixel location of the center dot of a known bullseye.
		 * @param yFrame The 8 bit grayscale frame in which the bullseyes will be detected, must be valid
		 * @param width The width of the given frame in pixel, with range [21, infinity)
		 * @param height The height of the given frame in pixel, with range [21, infinity)
		 * @param xBullseye The horizontal location of the bullseye (the center location), the pixel must be black, with range [0, width - 1]
		 * @param yBullseye The vertical location of the bullseye (the center location), the pixel must be black, with range [0, height - 1]
		 * @param threshold The threshold separating a bright pixel from a dark pixel, with range [0, 255]
		 * @param location The resulting sub-pixel location of the center of the bullseye
		 * @param framePaddingElements The number of padding elements at the end of each image row, in elements, with range [0, infinity)
		 * @return True, if the sub-pixel location could be determined
		 */
		static bool determineAccurateBullseyeLocation(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int xBullseye, const unsigned int yBullseye, const unsigned int threshold, Vector2& location, const unsigned int framePaddingElements);

		/**
		 * Returns the short distance between two bullseyes of the same Messenger Code (for neighboring bullseyes) based on the radius of a bullseye.
		 * @param radius The radius of the bullseye for which the corresponding distance will be calculated, with range (0, infinity)
		 * @return The distance between two neighboring bullseyes, with range (0, infinity)
		 */
		static inline Scalar radius2bullseyesDistanceShort(const Scalar radius);

		/**
		 * Returns the long distance between two bullseyes of the same Messenger Code (for opposite bullseyes) based on the radius of a bullseye.
		 * @param radius The radius of the bullseye for which the corresponding distance will be calculated, with range (0, infinity)
		 * @return The distance between opposite bullseyes, with range (0, infinity)
		 */
		static inline Scalar radius2bullseyesDistanceLong(const Scalar radius);

		/**
		 * Returns the reference to 260 coordinates of the Messenger Code's bit elements origin in the center of the Code and normalized to a radius of 1.
		 * @return The coordinates at which the Code's bit elements are located
		 */
		static const Vectors2& codeBitCoordinates();

		/**
		 * Returns 260 coordinates of the Messenger Code's bit elements origin in the center of the Code and normalized to a radius of 1.
		 * @return The coordinates at which the Code's bit elements are located
		 */
		static Vectors2 calculateBitCoordiantes();

		/**
		 * Calculates the bit coordinates of a ring of the Messenger Code.
		 * @param bits The number of bit for which the coordinates will be calculated, with range [1, infinity)
		 * @param bitsToSkip The indices of all bits for which no coordinate will be calculated, with range [0, bits - 1]
		 * @param radius The radius of the ring with range (0, infinity)
		 * @param coordinates The resulting coordinates, will be added to the end of the vector
		 */
		static void calculateRingBitCoordinates(const unsigned int bits, const IndexSet32& bitsToSkip, const Scalar radius, Vectors2& coordinates);
};

inline MessengerCodeDetector::Bullseye::Bullseye() :
	position_(-1, -1),
	radius_(0),
	grayThreshold_(0u)
{
	// nothing to do here
}

inline MessengerCodeDetector::Bullseye::Bullseye(const Vector2& position, const Scalar& radius, const unsigned int grayThreshold) :
	position_(position),
	radius_(radius),
	grayThreshold_(grayThreshold)
{
	// nothing to do here
}

inline const Vector2& MessengerCodeDetector::Bullseye::position() const
{
	return position_;
}

inline Scalar MessengerCodeDetector::Bullseye::radius() const
{
	return radius_;
}

inline unsigned int MessengerCodeDetector::Bullseye::grayThreshold() const
{
	return grayThreshold_;
}

inline MessengerCodeDetector::TransitionHistory::TransitionHistory() :
	deltaMinus1(0),
	deltaMinus2(0),
	deltaMinus3(0)
{
	// nothing to do here
}

inline int MessengerCodeDetector::TransitionHistory::history1()
{
	return deltaMinus1;
}

inline int MessengerCodeDetector::TransitionHistory::history2()
{
	return deltaMinus1 + deltaMinus2;
}

inline int MessengerCodeDetector::TransitionHistory::history3()
{
	return deltaMinus1 + deltaMinus2 + deltaMinus3;
}

inline void MessengerCodeDetector::TransitionHistory::push(const int newDelta)
{
	deltaMinus3 = deltaMinus2;
	deltaMinus2 = deltaMinus1;
	deltaMinus1 = newDelta;
}

inline void MessengerCodeDetector::TransitionHistory::reset()
{
	deltaMinus1 = 0;
	deltaMinus2 = 0;
	deltaMinus3 = 0;
}

inline MessengerCodeDetector::Bullseyes MessengerCodeDetector::detectBullseyes(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, Worker* worker)
{
	ocean_assert(yFrame != nullptr);
	ocean_assert(width >= 21u && height >= 21u);

	Bullseyes bullseyes;
	bullseyes.reserve(16);

	if (worker && height >= 600u)
	{
		Lock multiThreadLock;
		worker->executeFunction(Worker::Function::createStatic(&MessengerCodeDetector::detectBullseyesSubset, yFrame, width, height, &bullseyes, &multiThreadLock, yFramePaddingElements, 0u, 0u), 10u, height - 20u);
	}
	else
	{
		detectBullseyesSubset(yFrame, width, height, &bullseyes, nullptr, yFramePaddingElements, 10u, height - 20u);
	}

	return bullseyes;
}

inline bool MessengerCodeDetector::isTransitionToBlack(const uint8_t* pixel, TransitionHistory& history)
{
	const int currentDelta = int(*(pixel + 0) - *(pixel - 1));

	bool result = false;

	if (currentDelta < -deltaThreshold)
	{
		result = true;
	}
	else if ((currentDelta + history.history1() < -(deltaThreshold * 5 / 4))
		|| (currentDelta + history.history2() < -(deltaThreshold * 3 / 2))
		|| (currentDelta + history.history3() < -(deltaThreshold * 3 / 2)))
	{
		result = true;
	}

	history.push(currentDelta);

	return result;
}

inline bool MessengerCodeDetector::isTransitionToWhite(const uint8_t* pixel, TransitionHistory& history)
{
	const int currentDelta = int(*(pixel + 0) - *(pixel - 1));

	bool result = false;

	if (currentDelta > deltaThreshold)
	{
		result = true;
	}
	else if ((currentDelta + history.history1() > (deltaThreshold * 5 / 4))
		|| (currentDelta + history.history2() > (deltaThreshold * 3 / 2))
		|| (currentDelta + history.history3() > (deltaThreshold * 3 / 2)))
	{
		result = true;
	}

	history.push(currentDelta);

	return result;
}

template <bool tFindBlackPixel>
bool MessengerCodeDetector::findNextUpperPixel(const uint8_t* yPointer, unsigned int y, const unsigned int maximalRows, const unsigned int threshold, const unsigned int frameStrideElements, unsigned int& rows)
{
	ocean_assert(yPointer != nullptr);
	ocean_assert(maximalRows != 0u);
	ocean_assert(frameStrideElements != 0u);

	if (y == 0u)
	{
		return false;
	}

	rows = 0u;

	while (int(--y) >= 0 && ++rows <= maximalRows && (tFindBlackPixel ? (int(*(yPointer - frameStrideElements)) > int(threshold)) : (int(*(yPointer - frameStrideElements)) < int(threshold))))
	{
		yPointer -= frameStrideElements;
	}

	return int(y) >= 0 && rows <= maximalRows;
}

template <bool tFindBlackPixel>
bool MessengerCodeDetector::findNextLowerPixel(const uint8_t* yPointer, unsigned int y, const unsigned int height, const unsigned int maximalRows, const unsigned int threshold, const unsigned int frameStrideElements, unsigned int& rows)
{
	ocean_assert(yPointer != nullptr);
	ocean_assert(maximalRows != 0u);
	ocean_assert(y < height);
	ocean_assert(frameStrideElements != 0u);

	if (y >= height - 1u)
	{
		return false;
	}

	rows = 0u;

	while (++y < height && ++rows <= maximalRows && (tFindBlackPixel ? (int(*(yPointer + frameStrideElements)) > int(threshold)) : (int(*(yPointer + frameStrideElements)) < int(threshold))))
	{
		yPointer += frameStrideElements;
	}

	return y < height && rows <= maximalRows;
}

inline Scalar MessengerCodeDetector::radius2bullseyesDistanceShort(const Scalar radius)
{
	/*
	 * example:
	 * bullseyes radius: 27px
	 * bounding box size: 512px
	 * half bounding box size: 256px
	 * short edge (diagonal): sqrt(2) * 256px
	 */

	return Scalar(1.4142135623730950488016887242097) * radius2bullseyesDistanceLong(radius) * Scalar(0.5);
}

inline Scalar MessengerCodeDetector::radius2bullseyesDistanceLong(const Scalar radius)
{
	/*
	 * example:
	 * bullseyes radius: 27px
	 * bounding box size: 512px
	 */

	return radius * Scalar(512.0 / 27.0); // **TODO**
}

}

}

}

#endif // META_OCEAN_CV_DETECTOR_MESSENGER_CODE_DETECTOR_H
