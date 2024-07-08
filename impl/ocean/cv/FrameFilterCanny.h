/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_FILTER_CANNY_H
#define META_OCEAN_CV_FRAME_FILTER_CANNY_H

#include "ocean/cv/CV.h"

#include "ocean/base/Worker.h"

#include "ocean/cv/PixelPosition.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements a canny edge filter based on Sobel or Scharr filter operators and their responses respectively.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameFilterCanny
{
	protected:

		/**
		 * Definition of individual edge filter operators.
		 */
		enum EdgeFilter
		{
			/// Sobel operator
			EF_SOBEL,
			/// Scharr operator
			EF_SCHARR
		};

		/**
		 * Definition of individual edge directions.
		 */
		enum EdgeDirection
		{
			/// Unknown edge direction.
			ED_UNKNOWN = 0,
			/// Horizontal edge with 0 degree.
			ED_HORIZONTAL,
			/// Diagonal edge with 45 degree
			ED_DIAGONAL_45,
			/// Vertical edge with 90 degree.
			ED_VERTICAL,
			/// Diagonal edige with 135 degree.
			ED_DIAGONAL_135,
			/// No edge.
			ED_NO_EDGE
		};

	public:

		/**
		 * Canny edge detector for 8 bit grayscale images using a normalized Sobel operator<br>
		 * Applies a normalized Sobel filter in four directions: 0, 90, 45, and 135 degrees, value range: [-127, 127]. For thresholding the absolute value of the filter response will be used.<br>
		 * Pixels along the border of the source image are never considered for edges (first/last column/row).
		 * @param source The source frame (8-bit, 1 channel image), must be a valid
		 * @param target The frame that will contain extracted Canny edges (8-bit, 1 channel image, same size as `source`), must be valid
		 * @param width The width of the source frame in pixels, range: [3, infinity)
		 * @param height The height of the source frame in pixels, range: [3, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param lowThreshold After filtering the source image (cf. `tEdgeFilter`), pixels with an absolute filter response below this threshold will be immediately rejected as edges, range: [0, 127], default: 20
		 * @param highThreshold After filtering the source image (cf. `tEdgeFilter`), pixels with an absolute filter response below this threshold will be immediately accepted as edges if they are local maxima (non-maximum suppression), range: (lowThreshold, 127], default: 50
		 * @param worker An optional worker object for parallelization
		 */
		static void filterCannySobelNormalized(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const int8_t lowThreshold = 20, const int8_t highThreshold = 50, Worker* worker = nullptr);

		/**
		 * Canny edge detector for 8 bit grayscale images using a Sobel operator<br>
		 * Applies an (unnormalized) Sobel filter in four directions: 0, 90, 45, and 135 degrees, value range: [-1020, 1020]. For thresholding the absolute value of the filter response will be used.<br>
		 * Pixels along the border of the source image are never considered for edges (first/last column/row).
		 * @param source The source frame (8-bit, 1 channel image), must be a valid
		 * @param target The frame that will contain extracted Canny edges (8-bit, 1 channel image, same size as `source`), must be valid
		 * @param width The width of the source frame in pixels, range: [3, infinity)
		 * @param height The height of the source frame in pixels, range: [3, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param lowThreshold After filtering the source image (cf. `tEdgeFilter`), pixels with an absolute filter response below this threshold will be immediately rejected as edges, range: [0, 1020], default: 160
		 * @param highThreshold After filtering the source image (cf. `tEdgeFilter`), pixels with an absolute filter response below this threshold will be immediately accepted as edges if they are local maxima (non-maximum suppression), range: (lowThreshold, 1020], default: 400
		 * @param worker An optional worker object for parallelization
		 */
		static void filterCannySobel(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const int16_t lowThreshold = 160, const int16_t highThreshold = 400, Worker* worker = nullptr);

		/**
		 * Canny edge detector for 8 bit grayscale images using a normalized Scharr operator<br>
		 * Applies a normalized Scharr filter in four directions: 0, 90, 45, and 135 degrees, value range: [-127, 127]. For thresholding the absolute value of the filter response will be used.<br>
		 * Pixels along the border of the source image are never considered for edges (first/last column/row).
		 * @param source The source frame (8-bit, 1 channel image), must be a valid
		 * @param target The frame that will contain extracted Canny edges (8-bit, 1 channel image, same size as `source`), must be valid
		 * @param width The width of the source frame in pixels, range: [3, infinity)
		 * @param height The height of the source frame in pixels, range: [3, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param lowThreshold After filtering the source image (cf. `tEdgeFilter`), pixels with an absolute filter response below this threshold will be immediately rejected as edges, range: [0, 127], default: 20
		 * @param highThreshold After filtering the source image (cf. `tEdgeFilter`), pixels with an absolute filter response below this threshold will be immediately accepted as edges if they are local maxima (non-maximum suppression), range: (lowThreshold, 127], default: 50
		 * @param worker An optional worker object for parallelization
		 */
		static void filterCannyScharrNormalized(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const int8_t lowThreshold = 20, const int8_t highThreshold = 50, Worker* worker = nullptr);

		/**
		 * Canny edge detector for 8 bit grayscale images using a Scharr operator<br>
		 * Applies an (unnormalized) Scharr filter in four directions: 0, 90, 45, and 135 degrees, value range: [-4080, 4080]. For thresholding the absolute value of the filter response will be used.<br>
		 * Pixels along the border of the source image are never considered for edges (first/last column/row).
		 * @param source The source frame (8-bit, 1 channel image), must be a valid
		 * @param target The frame that will contain extracted Canny edges (8-bit, 1 channel image, same size as `source`), must be valid
		 * @param width The width of the source frame in pixels, range: [3, infinity)
		 * @param height The height of the source frame in pixels, range: [3, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param lowThreshold After filtering the source image (cf. `tEdgeFilter`), pixels with an absolute filter response below this threshold will be immediately rejected as edges, range: [0, 4080], default: 640
		 * @param highThreshold After filtering the source image (cf. `tEdgeFilter`), pixels with an absolute filter response below this threshold will be immediately accepted as edges if they are local maxima (non-maximum suppression), range: (lowThreshold, 4080], default: 1600
		 * @param worker An optional worker object for parallelization
		 */
		static void filterCannyScharr(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const int16_t lowThreshold = 640, const int16_t highThreshold = 1600, Worker* worker = nullptr);

	private:

		/**
		 * Canny edge filter for 8 bit gray scale images for 0, 90, 45, and 135 degrees.<br>
		 * The border pixels are set to unknown edge directions.
		 * @param source The source frame to filter, must be valid
		 * @param target The target frame receiving the filtered frame, must be valid
		 * @param width The width of the frame in pixel, with range [3, infinity)
		 * @param height The height of the frame in pixel, with range [3, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param lowThreshold The threshold for not being edges [0, 127]
		 * @param highThreshold The threshold for surely being edges (lowThreshold, 127]
		 * @param worker An optional worker object to distribute the computational load
		 * @tparam TFilterOutputElementType The element type of the filter output, cf. parameter `filterFunction`, e.g., `int8_t` or `int16_t`
		 * @tparam tEdgeFilter The filter function that is used to determine the horizontal, vertical, diagonal directions (0, 90, 45, 135 degree), must be valid.
		 */
		template <typename TFilterOutputElementType, EdgeFilter tEdgeFilter>
		static void filterCanny(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const TFilterOutputElementType lowThreshold, const TFilterOutputElementType highThreshold, Worker* worker = nullptr);

		/**
		 * Calculates edge directions (quantized to 0, 90, 45, and 135 degrees) and their corresponding magnitues (based on Sobel or Scharr filter responses).
		 * @param egdeFilterResults Source: filter response of scharr filter (zipped frame for each degree)
		 * @param direction The target receiving the quantized edge directions.
		 * @param magnitude The target receiving the edge magnitudes.
		 * @param lowThreshold The threshold for not being edges
		 * @param width The width of the frame in pixel, with range [3, infinity)
		 * @param height The height of the frame in pixel, with range [3, infinity)
		 * @param worker An optional worker object to distribute the computational load
		 * @tparam T The data type of the edge operator responses
		 */
		template <typename T>
		static inline void extractGradientDirectionsAndMagnitudes(const T* egdeFilterResults, uint8_t* direction, T* magnitude, const T lowThreshold, const unsigned int width, const unsigned int height, Worker* worker = nullptr);

		/**
		 * Calculates a subset of edge directions (quantized to 0, 90, 45, and 135 degrees) and their corresponding magnitues (based on Sobel or Scharr filter responses).
		 * @param egdeFilterResults Source: filter response of scharr filter (zipped frame for each degree)
		 * @param direction The target receiving the quantized edge directions.
		 * @param magnitude The target receiving the edge magnitudes.
		 * @param lowThreshold The threshold for not being edges
		 * @param width The width of the frame in pixel, with range [3, infinity)
		 * @param height The height of the frame in pixel, with range [3, infinity)
		 * @param firstRow First row to be filtered
		 * @param numberRows Number of rows to be filtered
		 * @tparam T The data type of the edge operator responses
		 */
		template <typename T>
		static void extractGradientDirectionsAndMagnitudesSubset(const T* egdeFilterResults, uint8_t* direction, T* magnitude, const T lowThreshold, const unsigned int width, const unsigned int height, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Extract edges from the gradient magnitudes and directions
		 * @param gradientDirections The frame containing the binned directions of the gradient (0, 90, 45, 135), must be valid
		 * @param gradientMagnitudes The frame containing the absolute gradient magnitudes, must be valid
		 * @param target The frame that will contain extracted Canny edges, must be valid (8-bit, 1 channel image, size: width x height)
		 * @param width The width of the frames in pixels, range: [3, infinity)
		 * @param height The height of the frames in pixels, range: [3, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param lowThreshold After filtering the source image (cf. `tEdgeFilter`), pixels with an absolute filter response below this threshold will be immediately rejected as edges, range: 8-bit Sobel/Scharr: [0, 126], 16-bit Sobel [0, 1019], 16-bit Scharr: [0, 4079]
		 * @param highThreshold After filtering the source image (cf. `tEdgeFilter`), pixels with an absolute filter response below this threshold will be immediately accepted as edges if they are local maxima (non-maximum suppression), range: 8-bit Sobel/Scharr: [lowThreshold, 127], 16-bit Sobel [0, 1020], 16-bit Scharr: [0, 4080]
		 * @param worker An optional worker object for parallelization
		 */
		template <typename TFilterOutputElementType>
		static void extractEdgePixels(const uint8_t* const gradientDirections, const TFilterOutputElementType* const gradientMagnitudes, uint8_t* const target, const unsigned int width, const unsigned int height, const unsigned int targetPaddingElements, const TFilterOutputElementType lowThreshold, const TFilterOutputElementType highThreshold, Worker* worker = nullptr);

		/**
		 * Helper function to extract edges from the gradient magnitudes and directions
		 * @param gradientDirections The frame containing the binned directions of the gradient (0, 90, 45, 135), must be valid
		 * @param gradientMagnitudes The frame containing the absolute gradient magnitudes, must be valid
		 * @param target The frame that will contain extracted Canny edges, must be valid (8-bit, 1 channel image, size: width x height)
		 * @param edgeCandidateMap The map of edge candidates
		 * @param edgeCandidateStack The resulting edge candidates, must be valid
		 * @param edgeCandidateStackLock The lock for the stack of edge candidates
		 * @param width The width of the frames in pixels, range: [3, infinity)
		 * @param height The height of the frames in pixels, range: [3, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param lowThreshold After filtering the source image (cf. `tEdgeFilter`), pixels with an absolute filter response below this threshold will be immediately rejected as edges, range: 8-bit Sobel/Scharr: [0, 126], 16-bit Sobel [0, 1019], 16-bit Scharr: [0, 4079]
		 * @param highThreshold After filtering the source image (cf. `tEdgeFilter`), pixels with an absolute filter response below this threshold will be immediately accepted as edges if they are local maxima (non-maximum suppression), range: 8-bit Sobel/Scharr: [lowThreshold, 127], 16-bit Sobel [0, 1020], 16-bit Scharr: [0, 4080]
		 * @param firstRow The first row to be handled, with range [0, height - 1]
		 * @param numberRows The number of rows to be handled, with range [1, height - firstRow]
		 */
		template <typename TFilterOutputElementType>
		static void extractEdgePixelsSubset(const uint8_t* const gradientDirections, const TFilterOutputElementType* const gradientMagnitudes, uint8_t* const target, uint8_t* const edgeCandidateMap, CV::PixelPositions* edgeCandidateStack, Lock* edgeCandidateStackLock, const unsigned int width, const unsigned int height, const unsigned int targetPaddingElements, const TFilterOutputElementType lowThreshold, const TFilterOutputElementType highThreshold, const unsigned int firstRow, const unsigned numberRows);
};

template <typename T>
inline void FrameFilterCanny::extractGradientDirectionsAndMagnitudes(const T* egdeFilterResults, uint8_t* direction, T* magnitude, const T lowThreshold, const unsigned int width, const unsigned int height, Worker* worker)
{
	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&extractGradientDirectionsAndMagnitudesSubset, egdeFilterResults, direction, magnitude, lowThreshold, width, height, 0u, 0u), 0, height, 6u, 7u, 20u);
	}
	else
	{
		extractGradientDirectionsAndMagnitudesSubset(egdeFilterResults, direction, magnitude, lowThreshold, width, height, 0u, height);
	}
}

} // namespace CV

} // namespace Ocean

#endif // META_OCEAN_CV_FRAME_FILTER_CANNY_H
