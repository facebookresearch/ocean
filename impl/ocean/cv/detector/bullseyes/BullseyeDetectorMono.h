/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef OCEAN_CV_DETECTOR_BULLSEYES_BULLSEYEDETECTORMONO_H
#define OCEAN_CV_DETECTOR_BULLSEYES_BULLSEYEDETECTORMONO_H

#include "ocean/cv/detector/bullseyes/Bullseyes.h"
#include "ocean/cv/detector/bullseyes/Bullseye.h"
#include "ocean/cv/detector/bullseyes/TransitionHistory.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Lock.h"
#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace Bullseyes
{

/**
 * Implementation of a monocular detector for the bullseye pattern.
 * @ingroup cvdetectorbullseyes
 */
class OCEAN_CV_DETECTOR_BULLSEYES_EXPORT BullseyeDetectorMono
{
	public:

		/**
		 * This class holds the most important parameters for the detector.
		 *
		 * Parameter Guide:
		 *
		 * framePyramidPixelThreshold:
		 *   Controls when to use multi-scale detection via image pyramids.
		 *   - Default: 640 * 480 = 307200 pixels (VGA resolution)
		 *   - For images larger than this threshold, pyramid layers are used to detect bullseyes at multiple scales
		 *   - Smaller values enable pyramid processing for smaller images (more thorough but slower)
		 *   - Larger values disable pyramid processing for more images (faster but may miss small bullseyes)
		 *
		 * framePyramidLayers:
		 *   Number of pyramid layers to use for multi-scale detection.
		 *   - Default: 3 layers
		 *   - More layers detect smaller bullseyes but increase computation time
		 *   - Typical range: 2-4 layers
		 *
		 * useAdaptiveRowSpacing:
		 *   Whether to skip rows during detection for better performance.
		 *   - Default: true (enabled)
		 *   - When enabled: Rows are skipped based on image height (height/150)
		 *   - When disabled: Every row is scanned (slower but more accurate)
		 *   - Recommended: true for real-time applications, false for offline/accuracy-critical applications
		 *
		 * minimumSegmentSize:
		 *   Minimum size in pixels for each of the 5 segments in the bullseye pattern.
		 *   - Default: 2 pixels
		 *   - Segments smaller than this are rejected as unreliable for threshold computation
		 *   - Helps filter out noise and false positives from tiny patterns at higher pyramid layers
		 */
		class OCEAN_CV_DETECTOR_BULLSEYES_EXPORT Parameters
		{
			public:

				/**
				 * Creates a new valid parameter object.
				 */
				Parameters() = default;

				/**
				 * Returns whether the parameters are valid.
				 * @return True if the parameters are valid
				 */
				bool isValid() const noexcept;

				/**
				 * Returns the pixel threshold for frame pyramid creation.
				 * @return The pixel threshold value, with range [0, infinity)
				 */
				unsigned int framePyramidPixelThreshold() const noexcept;

				/**
				 * Sets the pixel threshold for frame pyramid creation.
				 * @param framePyramidPixelThreshold The pixel threshold value, with range [0, infinity)
				 */
				void setFramePyramidPixelThreshold(unsigned int framePyramidPixelThreshold) noexcept;

				/**
				 * Returns the number of layers for the frame pyramid.
				 * @return The number of pyramid layers, with range [1, infinity)
				 */
				unsigned int framePyramidLayers() const noexcept;

				/**
				 * Sets the number of layers for the frame pyramid.
				 * @param framePyramidLayers The number of pyramid layers, with range [1, infinity)
				 */
				void setFramePyramidLayers(unsigned int framePyramidLayers) noexcept;

				/**
				 * Returns whether adaptive row spacing is enabled during bullseye detection.
				 * @return True if adaptive row spacing is used (performance optimization), false if every row is scanned (higher accuracy)
				 */
				bool useAdaptiveRowSpacing() const noexcept;

				/**
				 * Sets whether adaptive row spacing should be used during bullseye detection.
				 * When enabled (true), the detector uses adaptive row spacing based on frame height for better performance.
				 * When disabled (false), every row is scanned for higher accuracy but slower performance.
				 * @param useAdaptiveRowSpacing True to enable adaptive spacing, false to scan every row
				 */
				void setUseAdaptiveRowSpacing(bool useAdaptiveRowSpacing) noexcept;

				/**
				 * Returns the minimum segment size for a valid bullseye detection.
				 * @return The minimum segment size in pixels, with range [1, infinity)
				 */
				unsigned int minimumSegmentSize() const noexcept;

				/**
				 * Sets the minimum segment size for a valid bullseye detection.
				 * Segments smaller than this value are rejected as unreliable for threshold computation.
				 * @param minimumSegmentSize The minimum segment size in pixels, with range [1, infinity)
				 */
				void setMinimumSegmentSize(unsigned int minimumSegmentSize) noexcept;

				/**
				 * Returns the default parameters for the detector.
				 * @return The default parameters
				 */
				static Parameters defaultParameters() noexcept;

			protected:

				/// The pixel threshold for frame pyramid creation, with range [0, infinity)
				unsigned int framePyramidPixelThreshold_ = 640u * 480u;

				/// The number of layers for the frame pyramid, with range [1, infinity)
				unsigned int framePyramidLayers_ = 3u;

				/// Determines whether adaptive row spacing is used (true) or every row is scanned (false) during bullseye detection
				bool useAdaptiveRowSpacing_ = true;

				/// The minimum segment size in pixels for a valid bullseye detection, with range [1, infinity)
				unsigned int minimumSegmentSize_ = 2u;
		};

	public:

		/**
		 * Detects bullseyes in a given 8-bit grayscale image.
		 * @param yFrame The 8-bit grayscale frame in which the bullseyes will be detected, with origin in the upper left corner, must be valid
		 * @param bullseyes The resulting detected bullseyes, will be appended to the end of the vector
		 * @param parameters The parameters for the detector, must be valid
		 * @param worker Optional worker to distribute the computation
		 * @return True, if succeeded
		 */
		static bool detectBullseyes(const Frame& yFrame, Bullseyes& bullseyes, const Parameters& parameters = Parameters::defaultParameters(), Worker* worker = nullptr);

	protected:

		/**
		 * Detects bullseyes in a subset of a given 8-bit grayscale image.
		 * @param yFrame The 8-bit grayscale frame in which the bullseyes will be detected, must be valid
		 * @param bullseyes The resulting bullseyes, will be added to the end of the vector
		 * @param multiThreadLock Lock object in case this function is executed in multiple threads concurrently, otherwise nullptr
		 * @param useAdaptiveRowSpacing True to use adaptive row spacing, false to scan every row
		 * @param minimumSegmentSize The minimum segment size in pixels for a valid bullseye detection, with range [1, infinity)
		 * @param pyramidLayer The pyramid layer at which this detection is performed, with range [0, infinity)
		 * @param firstRow The first row to be handled, with range [0, yFrame.height())
		 * @param numberRows The number of rows to be handled, with range [1, yFrame.height() - firstRow]
		 */
		static void detectBullseyesSubset(const Frame* yFrame, Bullseyes* bullseyes, Lock* multiThreadLock, const bool useAdaptiveRowSpacing, const unsigned int minimumSegmentSize, const unsigned int pyramidLayer, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Detects bullseyes in a row of a grayscale image.
		 * @param yFrame The 8-bit grayscale frame in which the bullseyes will be detected, must be valid
		 * @param y The index of the row in which the bullseyes will be detected, with range [0, yFrame.height())
		 * @param bullseyes The resulting detected bullseyes, will be added to the end of the vector
		 * @param minimumSegmentSize The minimum segment size in pixels for a valid bullseye detection, with range [1, infinity)
		 * @param pyramidLayer The pyramid layer at which this detection is performed, with range [0, infinity)
		 */
		static void detectBullseyesInRow(const Frame& yFrame, const unsigned int y, Bullseyes& bullseyes, const unsigned int minimumSegmentSize, const unsigned int pyramidLayer = 0u);

		/**
		 * Finds either the next black or the next white pixel towards negative y direction (upwards in an image).
		 * @param yFrame The 8-bit grayscale frame in which the pixel will be searched, must be valid
		 * @param x The horizontal location within the frame at which the search will be performed, in pixels, with range [0, yFrame.width())
		 * @param y The current vertical location within the image, with range [0, yFrame.height())
		 * @param maximalRows The maximal number of rows (vertical pixels) to travel, with range [1, y]
		 * @param threshold The threshold separating a bright pixel from a dark pixel, with range [0, 255]
		 * @param rows The resulting number of rows that have been traveled until the black or white pixel has been found
		 * @return True, if the black or white pixel has been found within the specified range of [1, maximalRows]
		 * @tparam tFindBlackPixel True, to find the next black pixel; False, to find the next white pixel
		 */
		template <bool tFindBlackPixel>
		static bool findNextUpperPixel(const Frame& yFrame, const unsigned int x, const unsigned int y, const unsigned int maximalRows, const unsigned int threshold, unsigned int& rows);

		/**
		 * Finds either the next black or the next white pixel towards positive y direction (downwards in an image).
		 * @param yFrame The 8-bit grayscale frame in which the pixel will be searched, must be valid
		 * @param x The horizontal location within the frame at which the search will be performed, in pixels, with range [0, yFrame.width())
		 * @param y The current vertical location within the image, with range [0, yFrame.height())
		 * @param maximalRows The maximal number of rows (vertical pixels) to travel, with range [1, yFrame.height() - y]
		 * @param threshold The threshold separating a bright pixel from a dark pixel, with range [0, 255]
		 * @param rows The resulting number of rows that have been traveled until the black or white pixel has been found
		 * @return True, if the black or white pixel has been found within the specified range of [1, maximalRows]
		 * @tparam tFindBlackPixel True, to find the next black pixel; False, to find the next white pixel
		 */
		template <bool tFindBlackPixel>
		static bool findNextLowerPixel(const Frame& yFrame, const unsigned int x, const unsigned int y, const unsigned int maximalRows, const unsigned int threshold, unsigned int& rows);

		/**
		 * Determines the gray threshold separating bright pixels from dark pixels.
		 * The threshold is based on actual pixel values for which the association is known already.
		 * The provided position is a pointer to any pixel within the image frame.
		 * In addition to the pixels covered by the five segments, pixels neighboring the segments are also used for estimation of the threshold.
		 * @param yPosition The first pixel within an 8-bit grayscale image for which 5 connected segments are known with black, white, black, white, and black pixels, must be valid
		 * @param segmentSize1 The number of pixels covering dark pixels, with range [1, infinity)
		 * @param segmentSize2 The number of pixels covering bright pixels, with range [1, infinity)
		 * @param segmentSize3 The number of pixels covering dark pixels, with range [1, infinity)
		 * @param segmentSize4 The number of pixels covering bright pixels, with range [1, infinity)
		 * @param segmentSize5 The number of pixels covering dark pixels, with range [1, infinity)
		 * @return The threshold separating bright pixels from dark pixels, with range [0, 255], -1 if no valid threshold could be determined
		 */
		static unsigned int determineThreshold(const uint8_t* yPosition, const unsigned int segmentSize1, const unsigned int segmentSize2, const unsigned int segmentSize3, const unsigned int segmentSize4, const unsigned int segmentSize5);

		/**
		 * Checks whether a column contains a bullseye at a specified location.
		 * This function is simply checking for the same bullseye pattern in vertical direction (within a small window).
		 * @param yFrame The 8-bit grayscale frame in which the bullseyes will be detected, must be valid
		 * @param xCenter The horizontal location within the frame at which the existence of the bullseye will be checked, in pixels, with range [0, yFrame.width())
		 * @param yCenter The vertical location within the frame at which the existence of the bullseye will be checked, in pixels, with range [0, yFrame.height())
		 * @param threshold The grayscale threshold separating a bright pixel from a dark pixel, with range [0, 255]
		 * @param blackRingSegmentMin The minimal size (thickness) of the black ring, in pixels, with range [1, infinity)
		 * @param blackRingSegmentMax The maximal size (thickness) of the black ring, in pixels, with range [blackRingSegmentMin, infinity)
		 * @param whiteRingSegmentMin The minimal size (thickness) of the white ring, in pixels, with range [1, infinity)
		 * @param whiteRingSegmentMax The maximal size (thickness) of the white ring, in pixels, with range [whiteRingSegmentMin, infinity)
		 * @param dotSegmentMin The minimal size (thickness) of the black dot, in pixels, with range [1, infinity)
		 * @param dotSegmentMax The maximal size (thickness) of the black dot, in pixels, with range [dotSegmentMin, infinity)
		 * @return True, if the column contains a bullseye at the specified location
		 */
		static bool checkBullseyeInColumn(const Frame& yFrame, const unsigned int xCenter, const unsigned int yCenter, const unsigned int threshold, const unsigned int blackRingSegmentMin, const unsigned int blackRingSegmentMax, const unsigned int whiteRingSegmentMin, const unsigned int whiteRingSegmentMax, const unsigned int dotSegmentMin, const unsigned int dotSegmentMax);

		/**
		 * Checks whether the direct neighborhood contains a bullseye at a specified location.
		 * This function actually samples the neighborhood at sparse locations only instead of applying a dense check for the bullseye pattern.
		 * @param yFrame The 8-bit grayscale frame in which the bullseyes will be detected, must be valid
		 * @param xCenter The horizontal location within the frame at which the existence of the bullseye will be checked, in pixels, with range [0, yFrame.width())
		 * @param yCenter The vertical location within the frame at which the existence of the bullseye will be checked, in pixels, with range [0, yFrame.height())
		 * @param threshold The grayscale threshold separating a bright pixel from a dark pixel, with range [0, 255]
		 * @param whiteRingRadius The radius of the white ring (the center of this ring), in pixels, with range [1, infinity)
		 * @param blackRingRadius The radius of the black ring (the center of this ring), in pixels, with range [whiteRingRadius + 1, infinity)
		 * @param whiteBorderRadius The radius of the white border (the outer area around the black ring), in pixels, with range [blackRingRadius + 1, infinity)
		 * @return True, if the neighborhood contains a bullseye at the specified location
		 */
		static bool checkBullseyeInNeighborhood(const Frame& yFrame, const unsigned int xCenter, const unsigned int yCenter, const unsigned int threshold, const float whiteRingRadius, const float blackRingRadius, const float whiteBorderRadius);

		/**
		 * Determines the sub-pixel location of the center dot of a known bullseye.
		 * @param yFrame The 8-bit grayscale frame in which the bullseye is located, must be valid
		 * @param xBullseye The horizontal location of the bullseye (the center location), the pixel must be black, with range [0, yFrame.width())
		 * @param yBullseye The vertical location of the bullseye (the center location), the pixel must be black, with range [0, yFrame.height())
		 * @param threshold The threshold separating a bright pixel from a dark pixel, with range [0, 255]
		 * @param location The resulting sub-pixel location of the center of the bullseye
		 * @return True, if the sub-pixel location could be determined
		 */
		static bool determineAccurateBullseyeLocation(const Frame& yFrame, const unsigned int xBullseye, const unsigned int yBullseye, const unsigned int threshold, Vector2& location);
};

} // namespace Bullseyes

} // namespace Detector

} // namespace CV

} // namespace Ocean

#endif // OCEAN_CV_DETECTOR_BULLSEYES_BULLSEYEDETECTORMONO_H
