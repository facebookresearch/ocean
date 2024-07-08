/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_DETECTOR_LINE_DETECTOR_ULF_H
#define META_OCEAN_CV_DETECTOR_LINE_DETECTOR_ULF_H

#include "ocean/cv/detector/Detector.h"

#include "ocean/base/Memory.h"

#include "ocean/cv/FrameTransposer.h"
#include "ocean/cv/PixelPosition.h"

#include "ocean/math/FiniteLine2.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

/**
 * This class implements a line detector optimized for urban lines (Urban Line Finder).
 * @ingroup cvdetector
 */
class OCEAN_CV_DETECTOR_EXPORT LineDetectorULF
{
	public:

		/**
		 * Definition of individual edge types.
		 */
		enum EdgeType : uint8_t
		{
			/// No edge type.
			ET_NONE = 0u,
			/// A bar edge.
			ET_BAR = 1u,
			/// A step edge.
			ET_STEP = 2u,
			/// Positive sign edge; e.g., a bright bar edge.
			ET_SIGN_POSITIVE = 4u,
			/// Negative sign edge; e.g., a dark bar edge.
			ET_SIGN_NEGATIVE = 8u,
			/// A bar edge or a step edge.
			ET_BAR_OR_STEP = ET_BAR | ET_STEP
		};

		/**
		 * Definition of scan direction of the line detection
		 */
		enum ScanDirection : uint8_t
		{
			/// Scan only for vertical edges
			SD_VERTICAL = 1u,
			/// Scan only for horizontal edges
			SD_HORIZONTAL = 2u,
			/// Scan for vertical as well as horizontal edges
			SD_VERTICAL_AND_HORIZONTAL = SD_VERTICAL | SD_HORIZONTAL
		};

		/**
		 * Definition of a vector holding edge types.
		 */
		typedef std::vector<EdgeType> EdgeTypes;

	public:

		/**
		 * This class implements the almost abstract base class for all edge detectors.
		 * @see EdgeDetector::invoke().
		 */
		class OCEAN_CV_DETECTOR_EXPORT EdgeDetector
		{
			public:

				/**
				 * Destructor of an edge detector
				 */
				virtual ~EdgeDetector() = default;

				/**
				 * Returns the width of the sliding window in pixel, with range [1, infinity)
				 * @return The sliding window's width, in pixel, with range [1, infinity)
				 */
				inline unsigned int window() const;

				/**
				 * Returns the type of the edges this detector detects.
				 */
				inline EdgeType edgeType() const;

				/**
				 * Invokes the vertical edge detection for the entire frame.
				 * This function is guaranteed to exist in any EdgeDetector object.
				 * @param frame The 8bit grayscale frame on which the edge detection will be applied, must be valid
				 * @param width The width the given frame in pixel, with range [1, infinity)
				 * @param height The height of the given frame in pixel, with range [1, infinity)
				 * @param responses The pointer to the resulting response values one for each pixel, must be valid
				 * @param paddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
				 * @see invokeHorizontal().
				 */
				virtual void invokeVertical(const uint8_t* frame, const unsigned int width, const unsigned int height, int16_t* responses, const unsigned int paddingElements) const = 0;

				/**
				 * Invokes the horizontal edge detection for the entire frame.
				 * Beware: Not every EdgeDetector may implement this function, check whether the function is implemented before calling it.
				 * If this function is not implemented, transpose the input image and call invokeVertical() instead.
				 * @param frame The 8bit grayscale frame on which the edge detection will be applied, must be valid
				 * @param width The width the given frame in pixel, with range [1, infinity)
				 * @param height The height of the given frame in pixel, with range [1, infinity)
				 * @param responses The pointer to the resulting response values one for each pixel, must be valid
				 * @param paddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
				 * @return True, if succeeded
				 * @see hasInvokeHorizontal(), invokeVertical().
				 */
				virtual bool invokeHorizontal(const uint8_t* frame, const unsigned int width, const unsigned int height, int16_t* responses, const unsigned int paddingElements) const;

				/**
				 * Returns whether this EdgeDetector object has an implementation for invokeHorizontal().
				 * If the detector does not provide an implementation for horizontal edges, transpose the input image and call invokeVertical() instead of invokeHorizontal().
				 * @param width The width of the image to be handled, with range [0, infinity)
				 * @param height The height of the image to be handled, with range [0, infinity)
				 * @return True, if so; False, if no or if e.g., the image resolution is not suitable for the implementation
				 */
				virtual bool hasInvokeHorizontal(const unsigned int width, const unsigned int height) const;

				/**
				 * Adjusts the edge detection threshold (which is specified independently of the applied edge detection algorithm) so that it matches with the detection algorithm of this detector.
				 * This function allows to specify one threshold parameter for any of the implemented detection algorithms.
				 * @param threshold The threshold to be adjusted, with range [0, infinity)
				 * @return The adjusted threshold matching with this detection algorithm
				 */
				virtual unsigned int adjustThreshold(const unsigned int threshold) const;

				/**
				 * Determines the sums of pixel intensities of sliding windows within a row of a frame.
				 * @param row The row for which the sums will be determined, must be valid
				 * @param width The width of the given row in pixel, with range [7 + window, infinity)
				 * @param window The width of the sliding window in pixel, with range [1, width]
				 * @param windowSums The resulting (width - window + 1) sums of intensities of the sliding windows, must be valid
				 */
				static void determineRowSums(const uint8_t* row, const unsigned int width, const unsigned int window, uint32_t* windowSums);

				/**
				 * Determines the sums of pixel intensities of sliding windows within a row of a frame.
				 * @param row The row for which the sums will be determined, must be valid
				 * @param width The width of the given row in pixel, with range [7 + window, infinity)
				 * @param window The width of the sliding window in pixel, with range [1, width]
				 * @param windowSums The resulting (width - window + 1) sums of intensities of the sliding windows, must be valid
				 */
				static void determineRowSums(const uint8_t* row, const unsigned int width, const unsigned int window, uint16_t* windowSums);

				/**
				 * Determines the sums of pixel intensities (and sums of squared pixel intensities) of sliding windows within a row of a frame.
				 * @param row The row for which the sums will be determined, must be valid
				 * @param width The width of the given row in pixel, with range [7 + window, infinity)
				 * @param window The width of the sliding window in pixel, with range [1, width]
				 * @param windowSums The resulting (width - window + 1) sums of intensities of the sliding windows, must be valid
				 * @param windowSqrSums The resulting (width - window + 1) sums of squared intensities of the sliding windows, must be valid
				 */
				static void determineRowSums(const uint8_t* row, const unsigned int width, const unsigned int window, uint32_t* windowSums, uint32_t* windowSqrSums);

				/**
				 * Determines the sums of pixel intensities (and sums of squared pixel intensities) of sliding windows within a row of a frame.
				 * @param row The row for which the sums will be determined, must be valid
				 * @param width The width of the given row in pixel, with range [7 + window, infinity)
				 * @param window The width of the sliding window in pixel, with range [1, min(width, 255)]
				 * @param windowSums The resulting (width - window + 1) sums of intensities of the sliding windows, must be valid
				 * @param windowSqrSums The resulting (width - window + 1) sums of squared intensities of the sliding windows, must be valid
				 */
				static void determineRowSums(const uint8_t* row, const unsigned int width, const unsigned int window, uint16_t* windowSums, uint32_t* windowSqrSums);

				/**
				 * Either adds or subtracts one row from the sum and square sum buffers.
				 * @param row The row which will be added or subtracted, must be valid
				 * @param width The width of the row, in pixel, with range [1, infinity)
				 * @param sum The sum values to which the row will be added or from which the row will be subtracted, must be valid
				 * @tparam tAdd True, to add the row to the sum values; False, to subtract the row values from the sum values
				 */
				template <bool tAdd>
				static void applyRowSum(const uint8_t* row, const unsigned int width, uint16_t* sum);

				/**
				 * Either adds or subtracts one row from the sum and square sum buffers.
				 * @param row The row which will be added or subtracted, must be valid
				 * @param width The width of the row, in pixel, with range [1, infinity)
				 * @param sum The sum values to which the row will be added or from which the row will be subtracted, must be valid
				 * @param sqrSum The square sum values, to which the row will be added or from which the row will be subtracted, must be valid
				 * @tparam tAdd True, to add the row to the sum values; False, to subtract the row values from the sum values
				 */
				template <bool tAdd>
				static void applyRowSum(const uint8_t* row, const unsigned int width, uint16_t* sum, uint32_t* sqrSum);

			protected:

				/**
				 * Protected default constructor.
				 * @param window The width of the sliding window in pixel, with range [1, infinity)
				 * @param edgeType The type of the edge detector
				 */
				inline EdgeDetector(const unsigned int window, const EdgeType edgeType);

			protected:

				/// The width of the sliding window in pixel, with range [1, infinity)
				const unsigned int window_;

				// The type of the edges this detector detects.
				const EdgeType edgeType_;
		};

		/**
		 * Definition of a vector holding edge detectors.
		 */
		typedef std::vector<std::shared_ptr<EdgeDetector>> EdgeDetectors;

		/**
		 * This class implements an integer-based bar edge detector based on root mean square residuals.
		 * @see RMSBarEdgeDetectorF.
		 */
		class OCEAN_CV_DETECTOR_EXPORT RMSBarEdgeDetectorI : public EdgeDetector
		{
			protected:

				/// The bar size of this detector.
				static constexpr unsigned int barSize_ = 3u;

			public:

				/**
				 * Create a new edge detector object.
				 * @param window The window size which will be applied for detection, with range [1, width]
				 * @param minimalDelta The minimal intensity delta between average and center pixel, with range [0, 255]
				 */
				explicit RMSBarEdgeDetectorI(const unsigned int window = 4u, const unsigned int minimalDelta = barDetectorMinimalDelta());

				/**
				 * Invokes the vertical edge detection for the entire frame.
				 * @param frame The frame for which the edge detection will be applied, must be valid
				 * @param width The width of the given row in pixel, with range [8 + (window + 1) * 2, infinity)
				 * @param height The height of the given image in pixel, with range [1, infinity)
				 * @param responses The resulting response values, one for each row pixel, must be valid
				 * @param framePaddingElements The number of padding elements at the end of each image row, in elements, with range [0, infinity)
				 * @see EdgeDetector::invokeVertical().
				 */
				void invokeVertical(const uint8_t* frame, const unsigned int width, const unsigned int height, int16_t* responses, const unsigned int framePaddingElements) const override;

				/**
				 * Invokes the horizontal edge detection for the entire frame.
				 * Beware: Not every EdgeDetector may implement this function, check whether the function is implemented before calling it.
				 * If this function is not implemented, transpose the input image and call invokeVertical() instead.
				 * @param frame The 8bit grayscale frame on which the edge detection will be applied, must be valid
				 * @param width The width the given frame in pixel, with range [1, infinity)
				 * @param height The height of the given frame in pixel, with range [1, infinity)
				 * @param responses The pointer to the resulting response values one for each pixel, must be valid
				 * @param paddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
				 * @return True, if succeeded
				 * @see EdgeDetector::invokeHorizontal().
				 */
				bool invokeHorizontal(const uint8_t* frame, const unsigned int width, const unsigned int height, int16_t* responses, const unsigned int paddingElements) const override;

				/**
				 * Returns whether this EdgeDetector object has an implementation for invokeHorizontal().
				 * @see EdgeDetector::hasInvokeHorizontal().
				 */
				bool hasInvokeHorizontal(const unsigned int width, const unsigned int height) const override;

				/**
				 * Adjusts the edge detection threshold (which is specified independently of the applied edge detection algorithm) so that it matches with the detection algorithm of this detector.
				 * @see EdgeDetection::adjustThreshold().
				 */
				unsigned int adjustThreshold(const unsigned int threshold) const override;

				/**
				 * Adjusts the edge detection threshold (which is specified independently of the applied edge detection algorithm) so that it matches with the detection algorithm of this detector.
				 * This function is just the static version of adjustThreshold().
				 * @see EdgeDetection::adjustThreshold().
				 */
				static inline unsigned int staticAdjustThreshold(const unsigned int threshold);

				/**
				 * Invokes the vertical edge detection in one row of the input frame.
				 * @param row The row for which the edge detection will be applied, must be valid
				 * @param width The width of the given row in pixel, with range [8 + (window + 1) * 2, infinity)
				 * @param window The window size which will be applied for detection, with range [1, 11]
				 * @param minimalDelta The minimal intensity delta between average and center pixel, with range [0, 255]
				 * @param windowSums The (width - window + 1) sums of pixel intensities of the sliding window, must be valid
				 * @param windowSqrSums The (width - window + 1) sums of squared pixel intensities of the sliding window, must be valid
				 * @param sqrResponses The resulting (squared) response values, one for each row pixel, must be valid
				 */
				static void invokeRowVertical(const uint8_t* const row, const unsigned int width, const unsigned int window, const unsigned int minimalDelta, const uint16_t* const windowSums, const uint32_t* const windowSqrSums, int16_t* sqrResponses);

				/**
				 * Returns a vector containing just this edge detector with shared pointer (to simplify the usage with detectLines()).
				 * @param window The window size which will be applied for detection, with range [1, width]
				 * @param minimalDelta The minimal intensity delta between average and center pixel, with range [0, 255]
				 * @return This edge detector in a vector
				 */
				static inline EdgeDetectors asEdgeDetectors(const unsigned int window = 4u, const unsigned int minimalDelta = barDetectorMinimalDelta());

			protected:

				/// The minimal intensity delta between average and center pixel, with range [0, 255].
				const unsigned int minimalDelta_;
		};

		/**
		 * This class implements an integer-based bar step detector based on root mean square residuals.
		 */
		class OCEAN_CV_DETECTOR_EXPORT RMSStepEdgeDetectorI : public EdgeDetector
		{
			protected:

				/// The step size of this detector.
				static constexpr unsigned int stepSize_ = 1u;

			public:

				/**
				 * Create a new edge detector object.
				 * @param window The window size which will be applied for detection, with range [1, width]
				 */
				explicit RMSStepEdgeDetectorI(const unsigned int window = 4u);

				/**
				 * Invokes the vertical edge detection for the entire frame.
				 * @param frame The frame for which the edge detection will be applied, must be valid
				 * @param width The width of the given row in pixel, with range [8 + (window + 1) * 2, infinity)
				 * @param height The height of the given image in pixel, with range [1, infinity)
				 * @param responses The resulting response values, one for each row pixel, must be valid
				 * @param framePaddingElements The number of padding elements at the end of each image row, in elements, with range [0, infinity)
				 * @see EdgeDetector::invokeVertical().
				 */
				void invokeVertical(const uint8_t* frame, const unsigned int width, const unsigned int height, int16_t* responses, const unsigned int framePaddingElements) const override;

				/**
				 * Invokes the horizontal edge detection for the entire frame.
				 * Beware: Not every EdgeDetector may implement this function, check whether the function is implemented before calling it.
				 * If this function is not implemented, transpose the input image and call invokeVertical() instead.
				 * @param frame The 8bit grayscale frame on which the edge detection will be applied, must be valid
				 * @param width The width the given frame in pixel, with range [1, infinity)
				 * @param height The height of the given frame in pixel, with range [1, infinity)
				 * @param responses The pointer to the resulting response values one for each pixel, must be valid
				 * @param paddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
				 * @return True, if succeeded
				 * @see hasInvokeHorizontal(), invokeVertical().
				 */
				bool invokeHorizontal(const uint8_t* frame, const unsigned int width, const unsigned int height, int16_t* responses, const unsigned int paddingElements) const override;

				/**
				 * Returns whether this EdgeDetector object has an implementation for invokeHorizontal().
				 * @see EdgeDetector::hasInvokeHorizontal().
				 */
				bool hasInvokeHorizontal(const unsigned int width, const unsigned int height) const override;

				/**
				 * Adjusts the edge detection threshold (which is specified independently of the applied edge detection algorithm) so that it matches with the detection algorithm of this detector.
				 * @see EdgeDetection::adjustThreshold().
				 */
				unsigned int adjustThreshold(const unsigned int threshold) const override;

				/**
				 * Adjusts the edge detection threshold (which is specified independently of the applied edge detection algorithm) so that it matches with the detection algorithm of this detector.
				 * This function is just the static version of adjustThreshold().
				 * @see EdgeDetection::adjustThreshold().
				 */
				static inline unsigned int staticAdjustThreshold(const unsigned int threshold);

				/**
				 * Invokes the vertical edge detection in one row of the input frame.
				 * @param row The row for which the edge detection will be applied, must be valid
				 * @param width The width of the given row in pixel, with range [8 + window * 2, infinity)
				 * @param window The window size which will be applied for detection, with range [1, min(width, 8)]
				 * @param windowSums The (width - window + 1) sums of pixel intensities of the sliding window, must be valid
				 * @param windowSqrSums The (width - window + 1) sums of squared pixel intensities of the sliding window, must be valid
				 * @param sqrResponses The resulting (squared) response values, one for each row pixel, must be valid
				 */
				static void invokeRowVertical(const uint8_t* const row, const unsigned int width, const unsigned int window, const uint16_t* const windowSums, const uint32_t* const windowSqrSums, int16_t* sqrResponses);

				/**
				 * Returns a vector containing just this edge detector with shared pointer (to simplify the usage with detectLines()).
				 * @param window The window size which will be applied for detection, with range [1, width]
				 * @return This edge detector in a vector
				 */
				static inline EdgeDetectors asEdgeDetectors(const unsigned int window = 4u);
		};

		/**
		 * This class implements a floating-point-based bar edge detector based on root mean square residuals.
		 * The edge response is defined by:
		 * <pre>
		 * (peakValue - mean) / rms
		 * rms = sqrt(1/n * sum[(mean - yi)^2])
		 * </pre>
		 * Actually, this class mainly covers the original implementation of ULF's bar edge detector.
		 * @see RMSBarEdgeDetectorI.
		 */
		class OCEAN_CV_DETECTOR_EXPORT RMSBarEdgeDetectorF : public EdgeDetector
		{
			public:

				/**
				 * Create a new edge detector object.
				 * @param window The window size which will be applied for detection, with range [1, width]
				 * @param minimalDelta The minimal intensity delta between average and center pixel, with range [0, 255]
				 */
				explicit RMSBarEdgeDetectorF(const unsigned int window = 4u, const unsigned int minimalDelta = barDetectorMinimalDelta());

				/**
				 * Invokes the vertical edge detection for the entire frame.
				 * @see EdgeDetector::invokeVertical().
				 */
				void invokeVertical(const uint8_t* frame, const unsigned int width, const unsigned int height, int16_t* responses, const unsigned int paddingElements) const override;

				/**
				 * Adjusts the edge detection threshold (which is specified independently of the applied edge detection algorithm) so that it matches with the detection algorithm of this detector.
				 * @see EdgeDetection::adjustThreshold().
				 */
				unsigned int adjustThreshold(const unsigned int threshold) const override;

				/**
				 * Adjusts the edge detection threshold (which is specified independently of the applied edge detection algorithm) so that it matches with the detection algorithm of this detector.
				 * This function is just the static version of adjustThreshold().
				 * @see EdgeDetection::adjustThreshold().
				 */
				static inline unsigned int staticAdjustThreshold(const unsigned int threshold);

				/**
				 * Invokes the vertical edge detection in one row of the input frame.
				 * @param row The row for which the edge detection will be applied, must be valid
				 * @param width The width of the given row in pixel, with range [1, infinity)
				 * @param window The window size which will be applied for detection, with range [1, width]
				 * @param minimalDelta The minimal intensity delta between average and center pixel, with range [0, 255]
				 * @param windowSums The (width - window + 1) sums of pixel intensities of the sliding window, must be valid
				 * @param windowSqrSums The (width - window + 1) sums of squared pixel intensities of the sliding window, must be valid
				 * @param sqrResponses The resulting (squared) response values, one for each row pixel, must be valid
				 */
				static void invokeRowVertical(const uint8_t* const row, const unsigned int width, const unsigned int window, const unsigned int minimalDelta, const uint32_t* const windowSums, const uint32_t* const windowSqrSums, int16_t* sqrResponses);

			protected:

				/// The minimal intensity delta between average and center pixel, with range [0, 255].
				const unsigned int minimalDelta_;
		};

		/**
		 * This class implements a floating-point-based bar step detector based on root mean square residuals.
		 */
		class OCEAN_CV_DETECTOR_EXPORT RMSStepEdgeDetectorF : public EdgeDetector
		{
			public:

				/**
				 * Create a new edge detector object.
				 * @param window The window size which will be applied for detection, with range [1, width]
				 */
				explicit RMSStepEdgeDetectorF(const unsigned int window = 4u);

				/**
				 * Invokes the vertical edge detection for the entire frame.
				 * @see EdgeDetector::invokeVertical().
				 */
				void invokeVertical(const uint8_t* frame, const unsigned int width, const unsigned int height, int16_t* responses, const unsigned int paddingElements) const override;

				/**
				 * Adjusts the edge detection threshold (which is specified independently of the applied edge detection algorithm) so that it matches with the detection algorithm of this detector.
				 * @see EdgeDetection::adjustThreshold().
				 */
				unsigned int adjustThreshold(const unsigned int threshold) const override;

				/**
				 * Adjusts the edge detection threshold (which is specified independently of the applied edge detection algorithm) so that it matches with the detection algorithm of this detector.
				 * This function is just the static version of adjustThreshold().
				 * @see EdgeDetection::adjustThreshold().
				 */
				static inline unsigned int staticAdjustThreshold(const unsigned int threshold);

				/**
				 * Invokes the vertical edge detection in one row of the input frame.
				 * @param row The row for which the edge detection will be applied, must be valid
				 * @param width The width of the given row in pixel, with range [1, infinity)
				 * @param window The window size which will be applied for detection, with range [1, width]
				 * @param windowSums The (width - window + 1) sums of pixel intensities of the sliding window, must be valid
				 * @param windowSqrSums The (width - window + 1) sums of squared pixel intensities of the sliding window, must be valid
				 * @param sqrResponses The resulting (squared) response values, one for each row pixel, must be valid
				 */
				static void invokeRowVertical(const uint8_t* const row, const unsigned int width, const unsigned int window, const uint32_t* const windowSums, const uint32_t* const windowSqrSums, int16_t* sqrResponses);
		};

		/**
		 * This class implements an integer-based bar edge detector based on averaged differences.
		 */
		class OCEAN_CV_DETECTOR_EXPORT ADBarEdgeDetectorI : public EdgeDetector
		{
			public:

				/**
				 * Create a new edge detector object.
				 * @param window The window size which will be applied for detection, with range [1, width]
				 */
				explicit ADBarEdgeDetectorI(const unsigned int window = 4u);

				/**
				 * Invokes the vertical edge detection for the entire frame.
				 * @param frame The frame for which the edge detection will be applied, must be valid
				 * @param width The width of the given row in pixel, with range [8 + (window + 1) * 2, infinity)
				 * @param height The height of the given image in pixel, with range [1, infinity)
				 * @param responses The resulting response values, one for each row pixel, must be valid
				 * @param framePaddingElements The number of padding elements at the end of each image row, in elements, with range [0, infinity)
				 * @see EdgeDetector::invokeVertical().
				 */
				void invokeVertical(const uint8_t* frame, const unsigned int width, const unsigned int height, int16_t* responses, const unsigned int framePaddingElements) const override;

				/**
				 * Adjusts the edge detection threshold (which is specified independently of the applied edge detection algorithm) so that it matches with the detection algorithm of this detector.
				 * @see EdgeDetection::adjustThreshold().
				 */
				unsigned int adjustThreshold(const unsigned int threshold) const override;

				/**
				 * Adjusts the edge detection threshold (which is specified independently of the applied edge detection algorithm) so that it matches with the detection algorithm of this detector.
				 * This function is just the static version of adjustThreshold().
				 * @see EdgeDetection::adjustThreshold().
				 */
				static inline unsigned int staticAdjustThreshold(const unsigned int threshold);

				/**
				 * Invokes the vertical edge detection in one row of the input frame.
				 * @param row The row for which the edge detection will be applied, must be valid
				 * @param width The width of the given row in pixel, with range [8 + (window + 1) * 2, infinity)
				 * @param window The window size which will be applied for detection, with range [1, width]
				 * @param windowSums The (width - window + 1) sums of pixel intensities of the sliding window, must be valid
				 * @param responses The resulting response values, one for each row pixel, must be valid
				 */
				static void invokeRowVertical(const uint8_t* const row, const unsigned int width, const unsigned int window, const uint32_t* const windowSums, int16_t* responses);

				/**
				 * Returns a vector containing just this edge detector with shared pointer (to simplify the usage with detectLines()).
				 * @param window The window size which will be applied for detection, with range [1, width]
				 * @return This edge detector in a vector
				 */
				static inline EdgeDetectors asEdgeDetectors(const unsigned int window = 4u);
		};

		/**
		 * This class implements an integer-based (sum difference) step edge detector that computes the difference of two fixed-size sliding window of width 2.
		 * The edge response is defined by:
		 * <pre>
		 * peakValue = leftWindow - rightWindow
		 * </pre>
		 */
		class OCEAN_CV_DETECTOR_EXPORT SDStepEdgeDetectorI : public EdgeDetector
		{
			public:

				/**
				 * Create a new edge detector object.
				 * @param window The window size which will be applied for detection, with range [1, min(width, 127)]
				 * @param stepSize The number of pixels between both windows sums, "should be odd", "should" have range [1, infinity)
				 */
				explicit SDStepEdgeDetectorI(const unsigned int window = 2u, const unsigned int stepSize = 1u);

				/**
				 * Invokes the vertical edge detection for the entire frame.
				 * @param frame The frame for which the edge detection will be applied, must be valid
				 * @param width The width of the given row in pixel, with range [8 + (window + 1) * 2, infinity)
				 * @param height The height of the given image in pixel, with range [1, infinity)
				 * @param responses The resulting response values, one for each row pixel, must be valid
				 * @param framePaddingElements The number of padding elements at the end of each image row, in elements, with range [0, infinity)
				 * @see EdgeDetector::invokeVertical().
				 */
				void invokeVertical(const uint8_t* frame, const unsigned int width, const unsigned int height, int16_t* responses, const unsigned int framePaddingElements) const override;

				/**
				 * Invokes the horizontal edge detection for the entire frame.
				 * @see EdgeDetector::invokeHorizontal().
				 */
				bool invokeHorizontal(const uint8_t* frame, const unsigned int width, const unsigned int height, int16_t* responses, const unsigned int paddingElements) const override;

				/**
				 * Returns whether this EdgeDetector object has an implementation for invokeHorizontal().
				 * @see EdgeDetector::hasInvokeHorizontal().
				 */
				bool hasInvokeHorizontal(const unsigned int width, const unsigned int height) const override;

				/**
				 * Adjusts the edge detection threshold (which is specified independently of the applied edge detection algorithm) so that it matches with the detection algorithm of this detector.
				 * @see EdgeDetection::adjustThreshold().
				 */
				unsigned int adjustThreshold(const unsigned int threshold) const override;

				/**
				 * Returns a vector containing just this edge detector with shared pointer (to simplify the usage with detectLines()).
				 * @param window The window size which will be applied for detection, with range [1, width]
				 * @param stepSize The number of pixels between both windows sums, "should be odd", "should" have range [1, infinity)
				 * @return This edge detector in a vector
				 */
				static inline EdgeDetectors asEdgeDetectors(const unsigned int window = 2u, const unsigned int stepSize = 1u);

			protected:

				/**
				 * Invokes the vertical edge detection in one row of the input image with a fixed window width of 2 pixels
				 * Note this function only computes the difference between a left and a right window.
				 * @param row The row for which the edge detection will be applied, must be valid
				 * @param width The width of the given row in pixel, with range [8 + window * 2, infinity)
				 * @param stepSize The number of pixels between both windows sums, "should be odd", "should" have range [1, infinity)
				 * @param window The window size which will be applied for detection, with range [1, min(width, 127)]
				 * @param windowSums The (width - window + 1) sums of pixel intensities of the sliding window, must be valid for window >= 3
				 * @param responses The resulting response values (not squared!), one for each row pixel, must be valid
				 */
				static void invokeRowVertical(const uint8_t* const row, const unsigned int width, const unsigned int stepSize, const unsigned int window, const uint16_t* const windowSums, int16_t* responses);

			protected:

				/// The number of pixels between both windows sums, "should be odd", "should" have range [1, infinity).
				const unsigned int stepSize_;
		};

	public:

		/**
		 * Returns ULF's two default edge detectors.
		 * The two default edge detectors are: RMSBarEdgeDetectorI, RMSStepEdgeDetectorI.
		 * @param window The window size which will be applied for detection, with range [1, width]
		 * @return The two default edge detectors
		 * @see performanceEdgeDetectors().
		 */
		static inline EdgeDetectors defaultEdgeDetectors(const unsigned int window = 4u);

		/**
		 * Returns ULF's two high performance edge detectors.
		 * The two performance edge detectors are: ADBarEdgeDetectorI, SDStepEdgeDetectorI.
		 * Compared to ULF's defaultEdgeDetectors() the performance detectors are significantly faster, but will detect less lines.
		 * @param window The window size which will be applied for detection, with range [1, width]
		 * @return The two default edge detectors
		 * see defaultEdgeDetectors().
		 */
		static inline EdgeDetectors performanceEdgeDetectors(const unsigned int window = 4u);

		/**
		 * Detects finite lines within a given 8bit grayscale image.
		 * @param yFrame The image in which the lines will be detected, must be valid
		 * @param width The width of the given image in pixel, with range [20, infinity)
		 * @param height The height of the given image in pixel, with range [20, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each image row, in elements, with range [0, infinity)
		 * @param edgeDetectors The edge detectors which will be used to detect the lines, at least one
		 * @param threshold The detection threshold to be used to distinguish between weak and strong lines, the higher the threshold, the stronger the detected lines, with range [0, infinity)
		 * @param minimalLength The minimal length an extracted line must have in pixel, with range [2, infinity)
		 * @param maximalStraightLineDistance The maximal distance between the ideal line and every pixel on actual extracted line in pixel, with range [0, infinity)
		 * @param types Optional resulting types of the individual resulting lines, one type for each line
		 * @param scanDirection The scan direction(s) to be applied
		 * @return The detected lines
		 * @see defaultEdgeDetectors().
		 */
		static FiniteLines2 detectLines(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const EdgeDetectors& edgeDetectors = defaultEdgeDetectors(), const unsigned int threshold = 50u, const unsigned int minimalLength = 20u, const float maximalStraightLineDistance = 1.6f, EdgeTypes* types = nullptr, const ScanDirection scanDirection = SD_VERTICAL_AND_HORIZONTAL);

	protected:

		/**
		 * Extracts straight vertical (+/- 45 degree) finite lines from a given frame with edge responses.
		 * This function supports positive and negative response values and extracts individual lines for negative and positive responses.<br>
		 * For negative response values, the negative thresholds are applied.<br>
		 * This function follows strong responses in vertical direction and is converted the determined seam into one or several finite lines.<br>
		 * The seam is separated into several individual finite lines in case the distance of a seam pixel to the ideal line exceed a threshold.<br>
		 * Note: This function can extract horizontal finite lines if the given input response frame is transposed.
		 * @param responses The frame with edge responses from which the lines will be extracted, must be valid
		 * @param width The width of the given frame in pixels, with range [1, infinity)
		 * @param height The height of the given frame in pixels, with range [1, infinity)
		 * @param paddingElements The number of padding elements at the end of each response row, in elements, with range [0, infinity)
		 * @param transposed True, to create transposes line coordinates (e.g., if the given response frame is transposed)
		 * @param lines The resulting finite lines all fitting within the range [0, width - 1]x[0, height - 1]
		 * @param minimalStartThreshold The minimal (positive) threshold a response value must have to start the line extraction at this location, with range [1, infinity)
		 * @param minimalIntermediateThreshold The minimal (positive) threshold a response value must have once the line extraction has been started, with range [1, minimalStartThreshold]
		 * @param minimalLength The minimal length an extracted line must have in pixel, with range [2, infinity)
		 * @param maximalStraightLineDistance The maximal distance between the ideal line and every pixel on actual extracted line in pixel, with range [0, infinity)
		 * @param types Optional resulting types of the individual resulting lines, one type for each line
		 */
		template <typename T>
		static void extractVerticalLines(T* const responses, const unsigned int width, const unsigned int height, const unsigned int paddingElements, const bool transposed, FiniteLines2& lines, const unsigned int minimalStartThreshold, const unsigned int minimalIntermediateThreshold, const unsigned int minimalLength = 20u, const float maximalStraightLineDistance = 1.6f, EdgeTypes* types = nullptr);

		/**
		 * Extracts straight horizontal (+/- 45 degree) finite lines from a given frame with edge responses.
		 * This function supports positive and negative response values and extracts individual lines for negative and positive responses.<br>
		 * For negative response values, the negative thresholds are applied.<br>
		 * This function follows strong responses in horizontal direction and is converted the determined seam into one or several finite lines.<br>
		 * The seam is separated into several individual finite lines in case the distance of a seam pixel to the ideal line exceed a threshold.<br>
		 * Note: This function can extract horizontal finite lines if the given input response frame is transposed.
		 * @param responses The frame with edge responses from which the lines will be extracted, must be valid
		 * @param width The width of the given frame in pixels, with range [1, infinity)
		 * @param height The height of the given frame in pixels, with range [1, infinity)
		 * @param paddingElements The number of padding elements at the end of each response row, in elements, with range [0, infinity)
		 * @param lines The resulting finite lines all fitting within the range [0, width - 1]x[0, height - 1]
		 * @param minimalStartThreshold The minimal (positive) threshold a response value must have to start the line extraction at this location, with range [1, infinity)
		 * @param minimalIntermediateThreshold The minimal (positive) threshold a response value must have once the line extraction has been started, with range [1, minimalStartThreshold]
		 * @param minimalLength The minimal length an extracted line must have in pixel, with range [2, infinity)
		 * @param maximalStraightLineDistance The maximal distance between the ideal line and every pixel on actual extracted line in pixel, with range [0, infinity)
		 * @param types Optional resulting types of the individual resulting lines, one type for each line
		 */
		template <typename T>
		static void extractHorizontalLines(T* const responses, const unsigned int width, const unsigned int height, const unsigned int paddingElements, FiniteLines2& lines, const unsigned int minimalStartThreshold, const unsigned int minimalIntermediateThreshold, const unsigned int minimalLength = 20u, const float maximalStraightLineDistance = 1.6f, EdgeTypes* types = nullptr);

		/**
		 * Follows an edge in vertical direction while applying a horizontal search radius with one pixel (-1, 0, +1), to determine a seam of edge pixels.
		 * Each edge response value must exceed a specified threshold so that the edge continues.<br>
		 * Visited response values will be set to zero ensuring that we do not use the response again for another edge.<br>
		 * The function will return a connected list of pixel coordinates representing the found edge.<br>
		 * The scheme of the function is depicted below:
		 * <pre>
		 * Previous row n-1: - - - - - ? ? ? - - - -  (for tVerticalDirection = -1)
		 *  Current row n    - - - - - - x - - - - -
		 *     Next row n+1  - - - - - ? ? ? - - - -  (for tVerticalDirection = +1)
		 *
		 * with '?' candidate response values
		 * </pre>
		 * In case two or move response candidates exceed the threshold and have an equal value, the selection order is: center (0), left (-1), right (+1).
		 * @param data The frame with edge responses in which the edge will be followed, must be valid
		 * @param width The width of the given frame in pixels, with range [1, infinity)
		 * @param height The height of the given frame in pixels, with range [1, infinity)
		 * @param x The horizontal location at which the following process will start, with range [0, width - 1]
		 * @param y The vertical location at which the following process will start, with range [0, height - 1]
		 * @param threshold The threshold every edge response must exceed to count as edge
		 * @param pixelPositionsX The buffer of possible horizontal pixel locations for the resulting edge, the buffer must provide 'height' pixel locations
		 * @param paddingElements The number of padding elements at the end of each response row, in elements, with range [0, infinity)
		 * @return The last valid vertical pixel position, with range [0, height - 1]
		 * @tparam T The data type of the response value (and threshold value)
		 * @tparam tPositiveThreshold True, if the threshold is positive; False, if the threshold is negative
		 * @tparam tVerticalDirection 1 in case the edge should be followed towards positive y direction; -1 in case the edge should be followed towards negative y direction
		 * @see followEdgeVerticalBranchFree().
		 */
		template <typename T, const bool tPositiveThreshold, const int tVerticalDirection>
		static unsigned int followEdgeVertical(T* data, const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y, const T& threshold, unsigned int* pixelPositionsX, const unsigned int paddingElements);

		/**
		 * Follows an edge in horizontal direction while applying a vertical search radius with one pixel (-1, 0, +1), to determine a seam of edge pixels.
		 * Each edge response value must exceed a specified threshold so that the edge continues.<br>
		 * Visited response values will be set to zero ensuring that we do not use the response again for another edge.<br>
		 * The function will return a connected list of pixel coordinates representing the found edge.<br>
		 * In case two or move response candidates exceed the threshold and have an equal value, the selection order is: center (0), left (-1), right (+1).
		 * @param data The frame with edge responses in which the edge will be followed, must be valid
		 * @param width The width of the given frame in pixels, with range [1, infinity)
		 * @param height The height of the given frame in pixels, with range [1, infinity)
		 * @param x The horizontal location at which the following process will start, with range [0, width - 1]
		 * @param y The vertical location at which the following process will start, with range [0, height - 1]
		 * @param threshold The threshold every edge response must exceed to count as edge
		 * @param pixelPositionsY The buffer of possible vertical pixel locations for the resulting edge, the buffer must provide 'width' pixel locations
		 * @param paddingElements The number of padding elements at the end of each response row, in elements, with range [0, infinity)
		 * @return The last valid horizontal pixel position, with range [0, width - 1]
		 * @tparam T The data type of the response value (and threshold value)
		 * @tparam tPositiveThreshold True, if the threshold is positive; False, if the threshold is negative
		 * @tparam tHorizontalDirection 1 in case the edge should be followed towards positive x direction; -1 in case the edge should be followed towards negative x direction
		 */
		template <typename T, const bool tPositiveThreshold, const int tHorizontalDirection>
		static unsigned int followEdgeHorizontal(T* data, const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y, const T& threshold, unsigned int* pixelPositionsY, const unsigned int paddingElements);

		/**
		 * Follows an edge in vertical direction while applying a horizontal search radius with one pixel (-1, 0, +1).
		 * This function is similar to 'followEdgeVertical' while avoiding branches as best as possible.<br>
		 * In general, this function should be faster than `followEdgeVertical`.
		 * @param data The frame with edge responses in which the edge will be followed, must be valid
		 * @param width The width of the given frame in pixels, with range [1, infinity)
		 * @param height The height of the given frame in pixels, with range [1, infinity)
		 * @param x The horizontal location at which the following process will start, with range [0, width - 1]
		 * @param y The vertical location at which the following process will start, with range [0, height - 1]
		 * @param threshold The threshold every edge response must exceed to count as edge
		 * @param pixelPositionsX The buffer of possible pixel positions for the resulting edge, the buffer must provide 'height' pixel positions
		 * @param paddingElements The number of padding elements at the end of each response row, in elements, with range [0, infinity)
		 * @tparam T The data type of the response value (and threshold value)
		 * @tparam tPositiveThreshold True, if the threshold is positive; False, if the threshold is negative
		 * @tparam tVerticalDirection 1 in case the edge should be followed towards positive y direction; -1 in case the edge should be followed towards negative y direction
		 * @see followEdgeVertical().
		 */
		template <typename T, const bool tPositiveThreshold, const int tVerticalDirection>
		static unsigned int followEdgeVerticalBranchFree(T* data, const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y, const T& threshold, unsigned int* pixelPositionsX, const unsigned int paddingElements);

		/**
		 * Separates a set of connected pixels (almost defining a straight line) into individual perfect straight lines.
		 * @param pixelPositionsMajor The buffer of major pixel positions defining the connected pixels, must be valid
		 * @param firstPositionIndex The index within the given buffer 'pixelPositions' at which the set of connected pixels start, with range [0, infinity)
		 * @param lastPositionIndex The index within the given buffer 'pixelPositions' at which the set of connected pixels end (including position), with range [firstPositionIndex, infinity)
		 * @param lines The resulting separated straight lines, all separated lines will be added to the end of the given vector
		 * @param minimalLength The minimal length a separated straight line must have to be added, with range [2, infinity)
		 * @param maximalOffset The maximal distance/offset between a perfect line and the actual pixel so that a pixel counts as inlier for the line, in pixels, with range [0, infinity)
		 * @param majorIsY True, if the y-axis is the major axis; False, if the x-axis is the major axis
		 * @param refine True, to apply a non linear least square fit for the resulting lines; False, to use the pixel locations as actual end points of the resulting lines
		 */
		static void separateStraightLines(const unsigned int* pixelPositionsMajor, const unsigned int firstPositionIndex, const unsigned int lastPositionIndex, FiniteLines2& lines, const unsigned int minimalLength, const float maximalOffset, const bool majorIsY, const bool refine);

		/**
		 * Detects lines by applying a given edge detector for an image in horizontal and vertical direction.
		 * In case, the transposed image is not provided, horizontal lines will not be detected.
		 * @param yFrame The 8 bit grayscale frame in which the lines will be detected, which will be used to detect vertical lines, must be valid
		 * @param yFrameTransposedMemory The memory for the transposed frame, will be used if valid, otherwise will be created if necessary
		 * @param width The width of the given (normal) frame in pixels, with range [1, infinity)
		 * @param height The height of the given (normal) frame in pixels, with range [1, infinity)
		 * @param yFramePaddingElements The number of padding elements at the end of each row of the normal frame, with range [0, infinity)
		 * @param yFrameTransposedMemoryPaddingElements The number of padding elements at the end of each row of the transposed frame, with range [0, infinity)
		 * @param edgeDetector The edge detector to be used for line detection
		 * @param detectedLines The resulting lines that are detected, all detected lines will be added to the end of the given vector
		 * @param scanDirection The scan directions to be applied
		 * @param threshold The threshold all detected lines will exceed (at least at one location in the image), with range [1, infinity)
		 * @param reusableResponseBuffer Optional response buffer that can be used within this function, with one response value for each frame pixel, otherwise nullptr so that the function will create an own temporary buffer
		 * @param minimalLength The minimal length an extracted line must have in pixel, with range [2, infinity)
		 * @param maximalStraightLineDistance The maximal distance between the ideal line and every pixel on actual extracted line in pixel, with range [0, infinity)
		 * @param types Optional resulting types of the individual resulting lines, one type for each line
		 * @return True, if succeeded
		 */
		static bool detectLines(const uint8_t* const yFrame, Memory& yFrameTransposedMemory, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, unsigned int& yFrameTransposedMemoryPaddingElements, const EdgeDetector& edgeDetector, FiniteLines2& detectedLines, const ScanDirection scanDirection, const unsigned int threshold = 50u, int16_t* reusableResponseBuffer = nullptr, const unsigned int minimalLength = 20u, const float maximalStraightLineDistance = 1.6f, EdgeTypes* types = nullptr);

		/**
		 * Returns whether a given value is larger than or equal to a given threshold (or smaller than or equal to a given threshold).
		 * @param value The value to compare
		 * @param threshold The threshold to be used
		 * @return True, if value >= threshold && tPositiveThreshold, or value <= tPositiveThreshold && !tPositiveThreshold
		 * @tparam T The data type of the value and threshold
		 * @tparam tPositiveThreshold True, if the threshold is positive; False, if the threshold is negative
		 */
		template <typename T, const bool tPositiveThreshold>
		static inline bool valueMatchesThreshold(const T& value, const T& threshold);

		/**
		 * The threshold for the minimal delta for bar detectors.
		 * @return The threshold value
		 */
		static constexpr unsigned int barDetectorMinimalDelta();

};

inline LineDetectorULF::EdgeDetector::EdgeDetector(const unsigned int window, const EdgeType edgeType) :
	window_(window),
	edgeType_(edgeType)
{
	// nothing to do here
}

inline unsigned int LineDetectorULF::EdgeDetector::window() const
{
	return window_;
}

inline LineDetectorULF::EdgeType LineDetectorULF::EdgeDetector::edgeType() const
{
	return edgeType_;
}

inline unsigned int LineDetectorULF::RMSBarEdgeDetectorI::staticAdjustThreshold(const unsigned int threshold)
{
	/**
	 * The floating point response is:
	 * response = 16 * (peakValue - average) / residual
	 *
	 * sqrResponse = [64 * (2 * area) * (peakValue - average)]^2 / [area * residual]^2
	 *             = [64 * 2 * (peakValue - average) / residual]^2
	 *             = 16^ * [(peakValue - average) / residual]^2
	 *             **TODO** 128^2 ???
	 */

	return threshold * threshold;
}

inline LineDetectorULF::EdgeDetectors LineDetectorULF::RMSBarEdgeDetectorI::asEdgeDetectors(const unsigned int window, const unsigned int minimalDelta)
{
	return {std::make_shared<RMSBarEdgeDetectorI>(window, minimalDelta)};
}

unsigned int LineDetectorULF::RMSStepEdgeDetectorI::staticAdjustThreshold(const unsigned int threshold)
{
	/**
	 * The floating point response is:
	 * response = 4 * (averageL - averageR) / [(residualL + residualR) / 2]
	 *          = 8 * (averageL - averageR) / (residualL + residualR)
	 *
	 * sqrResponse = window^2 * (averageL - averageR)^2 / [window^2 * residualL^2 + window^2 + residualR^2)]
	 *             = 8^2 * (averageL - averageR)^2 / (residualL^2 + residualR^2)
	 *             **TODO** 32 ???
	 *
	 * note this is just an approximation as: (residualL + residualR)^2  !=  (residualL^2 + residualR^2)
	 */

	return threshold * threshold;
}

inline LineDetectorULF::EdgeDetectors LineDetectorULF::RMSStepEdgeDetectorI::asEdgeDetectors(const unsigned int window)
{
	return {std::make_shared<RMSStepEdgeDetectorI>(window)};
}

unsigned int LineDetectorULF::RMSBarEdgeDetectorF::staticAdjustThreshold(const unsigned int threshold)
{
	/**
	 * The response is:
	 * 16 * (peakValue - average) / residual
	 */

	return threshold;
}

unsigned int LineDetectorULF::RMSStepEdgeDetectorF::staticAdjustThreshold(const unsigned int threshold)
{
	/**
	 * The response is:
	 * 4 * (averageL - averageR) / [(residualL + residualR) / 2]
	 */

	 return threshold;
}

unsigned int LineDetectorULF::ADBarEdgeDetectorI::staticAdjustThreshold(const unsigned int threshold)
{
	return threshold; // **TODO**
}

inline LineDetectorULF::EdgeDetectors LineDetectorULF::ADBarEdgeDetectorI::asEdgeDetectors(const unsigned int window)
{
	return {std::make_shared<ADBarEdgeDetectorI>(window)};
}

inline LineDetectorULF::EdgeDetectors LineDetectorULF::SDStepEdgeDetectorI::asEdgeDetectors(const unsigned int window, const unsigned int stepSize)
{
	return {std::make_shared<SDStepEdgeDetectorI>(window, stepSize)};
}

inline LineDetectorULF::EdgeDetectors LineDetectorULF::defaultEdgeDetectors(const unsigned int window)
{
	return {std::make_shared<RMSBarEdgeDetectorI>(window, barDetectorMinimalDelta()), std::make_shared<RMSStepEdgeDetectorI>(window)};
}

inline LineDetectorULF::EdgeDetectors LineDetectorULF::performanceEdgeDetectors(const unsigned int window)
{
	return {std::make_shared<ADBarEdgeDetectorI>(window), std::make_shared<SDStepEdgeDetectorI>(window)};
}

template <typename T>
void LineDetectorULF::extractVerticalLines(T* const responses, const unsigned int width, const unsigned int height, const unsigned int paddingElements, const bool transposed, FiniteLines2& lines, const unsigned int minimalStartThreshold, const unsigned int minimalIntermediateThreshold, const unsigned int minimalLength, const float maximalStraightLineDistance, EdgeTypes* types)
{
	ocean_assert(responses != nullptr);
	ocean_assert(width != 0u && height != 0u);

	ocean_assert(minimalStartThreshold >= minimalIntermediateThreshold);
	ocean_assert(minimalStartThreshold >= 0u && minimalIntermediateThreshold >= 0u);

	ocean_assert(double(minimalStartThreshold) <= double(NumericT<T>::maxValue()));

	ocean_assert(maximalStraightLineDistance >= 0.0f);

	ocean_assert(types == nullptr || types->size() == lines.size());

	Memory memoryPixelPositionsX = Memory::create<unsigned int>(height);
	unsigned int* pixelPositionsX = memoryPixelPositionsX.data<unsigned int>();

	const T* responsePixel = responses;

	for (unsigned int y = 0u; y < height; ++y)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			if ((unsigned int)abs(*responsePixel) >= minimalStartThreshold)
			{
				unsigned int firstValidPixelPosition = y;
				unsigned int lastValidPixelPosition = y;

				const EdgeType edgeTypeSign = *responsePixel >= T(minimalStartThreshold) ? ET_SIGN_POSITIVE : ET_SIGN_NEGATIVE;

				if (*responsePixel >= T(minimalStartThreshold))
				{
					constexpr bool positiveThreshold = true;

					lastValidPixelPosition = LineDetectorULF::followEdgeVertical<T, positiveThreshold, 1>(responses, width, height, x, y, T(minimalIntermediateThreshold), pixelPositionsX, paddingElements);
					firstValidPixelPosition = LineDetectorULF::followEdgeVertical<T, positiveThreshold, -1>(responses, width, height, x, y, T(minimalIntermediateThreshold), pixelPositionsX, paddingElements);
				}
				else if (*responsePixel <= -T(minimalStartThreshold))
				{
					constexpr bool positiveThreshold = false;

					lastValidPixelPosition = LineDetectorULF::followEdgeVertical<T, positiveThreshold, 1>(responses, width, height, x, y, -T(minimalIntermediateThreshold), pixelPositionsX, paddingElements);
					firstValidPixelPosition = LineDetectorULF::followEdgeVertical<T, positiveThreshold, -1>(responses, width, height, x, y, -T(minimalIntermediateThreshold), pixelPositionsX, paddingElements);
				}

				ocean_assert(lastValidPixelPosition >= firstValidPixelPosition);
				const unsigned int length = lastValidPixelPosition - firstValidPixelPosition + 1u;

				if (length > minimalLength)
				{
					const size_t previousNumberLines = lines.size();

					LineDetectorULF::separateStraightLines(pixelPositionsX, firstValidPixelPosition, lastValidPixelPosition, lines, minimalLength, maximalStraightLineDistance, transposed, true);

					const size_t numberNewLines = lines.size() - previousNumberLines;

					if (types && numberNewLines > 0)
					{
						// we set the sign of all new lines which have been created within separateStraightLines() - all new lines have the same sign
						types->insert(types->end(), numberNewLines, edgeTypeSign);
					}
				}
			}

			++responsePixel;
		}

		responsePixel += paddingElements;
	}
}

template <typename T>
void LineDetectorULF::extractHorizontalLines(T* const responses, const unsigned int width, const unsigned int height, const unsigned int paddingElements, FiniteLines2& lines, const unsigned int minimalStartThreshold, const unsigned int minimalIntermediateThreshold, const unsigned int minimalLength, const float maximalStraightLineDistance, EdgeTypes* types)
{
	ocean_assert(responses != nullptr);
	ocean_assert(width != 0u && height != 0u);

	ocean_assert(minimalStartThreshold >= minimalIntermediateThreshold);
	ocean_assert(minimalStartThreshold >= 0u && minimalIntermediateThreshold >= 0u);

	ocean_assert(double(minimalStartThreshold) <= double(NumericT<T>::maxValue()));

	ocean_assert(maximalStraightLineDistance >= 0.0f);

	ocean_assert(types == nullptr || types->size() == lines.size());

	Memory memoryPixelPositionsY = Memory::create<unsigned int>(width);
	unsigned int* pixelPositionsY = memoryPixelPositionsY.data<unsigned int>();

	const T* responsePixel = responses;

	for (unsigned int y = 0u; y < height; ++y)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			if ((unsigned int)abs(*responsePixel) >= minimalStartThreshold)
			{
				unsigned int firstValidPixelPosition = x;
				unsigned int lastValidPixelPosition = x;

				const EdgeType edgeTypeSign = *responsePixel >= T(minimalStartThreshold) ? ET_SIGN_POSITIVE : ET_SIGN_NEGATIVE;

				if (*responsePixel >= T(minimalStartThreshold))
				{
					constexpr bool positiveThreshold = true;

					lastValidPixelPosition = LineDetectorULF::followEdgeHorizontal<T, positiveThreshold, 1>(responses, width, height, x, y, T(minimalIntermediateThreshold), pixelPositionsY, paddingElements);
					firstValidPixelPosition = LineDetectorULF::followEdgeHorizontal<T, positiveThreshold, -1>(responses, width, height, x, y, T(minimalIntermediateThreshold), pixelPositionsY, paddingElements);
				}
				else if (*responsePixel <= -T(minimalStartThreshold))
				{
					constexpr bool positiveThreshold = false;

					lastValidPixelPosition = LineDetectorULF::followEdgeHorizontal<T, positiveThreshold, 1>(responses, width, height, x, y, -T(minimalIntermediateThreshold), pixelPositionsY, paddingElements);
					firstValidPixelPosition = LineDetectorULF::followEdgeHorizontal<T, positiveThreshold, -1>(responses, width, height, x, y, -T(minimalIntermediateThreshold), pixelPositionsY, paddingElements);
				}

				ocean_assert(lastValidPixelPosition >= firstValidPixelPosition);
				const unsigned int length = lastValidPixelPosition - firstValidPixelPosition + 1u;

				if (length > minimalLength)
				{
					const size_t previousNumberLines = lines.size();

					LineDetectorULF::separateStraightLines(pixelPositionsY, firstValidPixelPosition, lastValidPixelPosition, lines, minimalLength, maximalStraightLineDistance, true /*majorIsY*/, true);

					const size_t numberNewLines = lines.size() - previousNumberLines;

					if (types && numberNewLines > 0)
					{
						// we set the sign of all new lines which have been created within separateStraightLines() - all new lines have the same sign
						types->insert(types->end(), numberNewLines, edgeTypeSign);
					}
				}
			}

			++responsePixel;
		}

		responsePixel += paddingElements;
	}
}

template <typename T, const bool tPositiveThreshold, const int tVerticalDirection>
unsigned int LineDetectorULF::followEdgeVertical(T* data, const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y, const T& threshold, unsigned int* pixelPositionsX, const unsigned int paddingElements)
{
	static_assert(tVerticalDirection == 1 || tVerticalDirection == -1, "Invalid direction value!");

	ocean_assert(data != nullptr);
	ocean_assert(x < width && y < height);
	ocean_assert(threshold != T(0));
	ocean_assert(pixelPositionsX != nullptr);

	const unsigned int dataStrideElements = width + paddingElements;

	data += y * dataStrideElements + x;
	*data = T(0); // visited

	pixelPositionsX[y] = x;

	unsigned int nextX = x;
	unsigned int nextY = (unsigned int)(int(y) + tVerticalDirection);

	data += tVerticalDirection * int(dataStrideElements);

	while (nextY < height) // this test includes negative coordinates: (unsigned int)(-1) >= height
	{
		ocean_assert(nextX < width);

		T bestValue = threshold;
		int bestOffset = NumericT<int>::minValue();

		// right pixel
		if (nextX < width - 1u && valueMatchesThreshold<T, tPositiveThreshold>(*(data + 1), threshold /* = bestValue */))
		{
			bestValue = *(data + 1);
			bestOffset = 1;
		}

		// left pixel
		if (nextX >= 1u && valueMatchesThreshold<T, tPositiveThreshold>(*(data - 1), bestValue))
		{
			bestValue = *(data - 1);
			bestOffset = -1;
		}

		// center pixel
		if (valueMatchesThreshold<T, tPositiveThreshold>(*data, bestValue))
		{
			// bestValue = *(data + 0); not used below anymore
			bestOffset = 0;
		}

		if (bestOffset == NumericT<int>::minValue())
		{
			break;
		}

		data += bestOffset;
		*data = 0; // visited

		nextX += bestOffset;
		pixelPositionsX[nextY] = nextX;

		data += tVerticalDirection * int(dataStrideElements);

		nextY = (unsigned int)(int(nextY) + tVerticalDirection);
	}

	ocean_assert(pixelPositionsX[int(nextY) - tVerticalDirection] < width);

	ocean_assert((unsigned int)(int(nextY) - tVerticalDirection) < height);

	// the previous y value
	return (unsigned int)(int(nextY) - tVerticalDirection);
}

template <typename T, const bool tPositiveThreshold, const int tHorizontalDirection>
unsigned int LineDetectorULF::followEdgeHorizontal(T* data, const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y, const T& threshold, unsigned int* pixelPositionsY, const unsigned int paddingElements)
{
	static_assert(tHorizontalDirection == 1 || tHorizontalDirection == -1, "Invalid direction value!");

	ocean_assert(data != nullptr);
	ocean_assert(x < width && y < height);
	ocean_assert(threshold != T(0));
	ocean_assert(pixelPositionsY != nullptr);

	const unsigned int dataStrideElements = width + paddingElements;

	data += y * dataStrideElements + x;
	*data = T(0); // visited

	pixelPositionsY[x] = y;

	unsigned int nextX = (unsigned int)(int(x) + tHorizontalDirection);
	unsigned int nextY = y;

	data += tHorizontalDirection;

	while (nextX < width) // this test includes negative coordinates: (unsigned int)(-1) >= width
	{
		ocean_assert(nextY < height);

		T bestValue = threshold;
		int bestOffset = NumericT<int>::minValue();

		// right pixel
		if (nextY < height - 1u && valueMatchesThreshold<T, tPositiveThreshold>(*(data + dataStrideElements), threshold /* = bestValue */))
		{
			bestValue = *(data + dataStrideElements);
			bestOffset = 1;
		}

		// left pixel
		if (nextY >= 1u && valueMatchesThreshold<T, tPositiveThreshold>(*(data - dataStrideElements), bestValue))
		{
			bestValue = *(data - dataStrideElements);
			bestOffset = -1;
		}

		// center pixel
		if (valueMatchesThreshold<T, tPositiveThreshold>(*data, bestValue))
		{
			// bestValue = *(data + 0); not used below anymore
			bestOffset = 0;
		}

		if (bestOffset == NumericT<int>::minValue())
		{
			break;
		}

		data += bestOffset * int(dataStrideElements);
		*data = 0; // visited

		nextY += bestOffset;
		pixelPositionsY[nextX] = nextY;

		data += tHorizontalDirection;

		nextX = (unsigned int)(int(nextX) + tHorizontalDirection);
	}

	ocean_assert(pixelPositionsY[int(nextX) - tHorizontalDirection] < height);

	ocean_assert((unsigned int)(int(nextX) - tHorizontalDirection) < width);

	// the previous x value
	return (unsigned int)(int(nextX) - tHorizontalDirection);
}

template <typename T, const bool tPositiveThreshold, const int tVerticalDirection>
unsigned int LineDetectorULF::followEdgeVerticalBranchFree(T* data, const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y, const T& threshold, unsigned int* pixelPositionsX, const unsigned int paddingElements)
{
	static_assert(tVerticalDirection == 1 || tVerticalDirection == -1, "Invalid direction value!");

	ocean_assert(data != nullptr);
	ocean_assert(x < width && y < height);
	ocean_assert(threshold != T(0));
	ocean_assert(pixelPositionsX != nullptr);

	const unsigned int dataStrideElements = width + paddingElements;

	data += y * dataStrideElements + x;
	*data = T(0); // visited

	pixelPositionsX[y] = x;

	const int intThreshold = int(threshold);

	unsigned int nextX = x;
	unsigned int nextY = (unsigned int)(int(y) + tVerticalDirection);

	data += tVerticalDirection * int(dataStrideElements);

	while (nextY < height) // this test includes negative coordinates: (unsigned int)(-1) >= height
	{
		ocean_assert(nextX < width);

		const int leftOrCenterValue = *(data - int(nextX - 1u < width));
		const int centerValue = *data;
		const int rightOrCenterValue = *(data + int(nextX < width - 1u));

#ifdef OCEAN_DEBUG

		if (nextX >= 1u)
			ocean_assert(leftOrCenterValue == *(data - 1));
		else
			ocean_assert(leftOrCenterValue == *data);

		if (nextX + 1u < width)
			ocean_assert(rightOrCenterValue == *(data + 1));
		else
			ocean_assert(rightOrCenterValue == *data);

#endif

		// priority if equal: center, right, left

		// for positive threshold: center >= threshold && center >= left && center >= right
		const int useCenterValue = tPositiveThreshold ? (centerValue >= intThreshold && centerValue >= leftOrCenterValue && centerValue >= rightOrCenterValue) : centerValue <= intThreshold && centerValue <= leftOrCenterValue && centerValue <= rightOrCenterValue;
		ocean_assert(useCenterValue == 0 || useCenterValue == 1);

		// for positive threshold: right >= threshold && right > center && right > left
		const int useRightValue = tPositiveThreshold ? (rightOrCenterValue >= intThreshold && rightOrCenterValue > centerValue && rightOrCenterValue > leftOrCenterValue) : (rightOrCenterValue <= intThreshold && rightOrCenterValue < centerValue && rightOrCenterValue < leftOrCenterValue);
		ocean_assert(useRightValue == 0 || useRightValue == 1);

		// for positive threshold: left >= threshold && left > center && left >= right
		const int useLeftValue = tPositiveThreshold ? (leftOrCenterValue >= intThreshold && leftOrCenterValue > centerValue && leftOrCenterValue >= rightOrCenterValue) : (leftOrCenterValue <= intThreshold && leftOrCenterValue < centerValue && leftOrCenterValue <= rightOrCenterValue);
		ocean_assert(useLeftValue == 0 || useLeftValue == 1);

		ocean_assert(useCenterValue + useLeftValue + useRightValue == 0 || useCenterValue + useLeftValue + useRightValue == 1);

		const int useNoValue = 1 - useCenterValue - useLeftValue - useRightValue;
		ocean_assert(useNoValue == 0 || useNoValue == 1);

		if (useNoValue != 0)
		{
			// we have no valid next value

#ifdef OCEAN_DEBUG

			ocean_assert(useLeftValue == 0);
			ocean_assert(useCenterValue == 0);
			ocean_assert(useRightValue == 0);

			if constexpr (tPositiveThreshold)
			{
				ocean_assert(*data < intThreshold);
				ocean_assert(nextX == 0u || *(data - 1) < intThreshold);
				ocean_assert(nextX + 1u >= width || *(data + 1) < intThreshold);
			}
			else
			{
				ocean_assert(*data > intThreshold);
				ocean_assert(nextX == 0u || *(data - 1) > intThreshold);
				ocean_assert(nextX + 1u >= width || *(data + 1) > intThreshold);
			}

#endif // OCEAN_DEBUG

			break;
		}

		const int nextOffsetX = -useLeftValue /* + useCenterValue * 0 */ + useRightValue;

#ifdef OCEAN_DEBUG

		if (useNoValue == 0)
		{
			// we have a valid next value

			if (nextOffsetX == -1)
			{
				ocean_assert(useLeftValue == 1);
				ocean_assert(nextX >= 1u);

				if constexpr (tPositiveThreshold)
				{
					ocean_assert(*(data - 1) >= intThreshold);
					ocean_assert(*(data - 1) > *data);
					ocean_assert(nextX + 1u >= width || *(data - 1) >= *(data + 1));
				}
				else
				{
					ocean_assert(*(data - 1) <= intThreshold);
					ocean_assert(*(data - 1) < *data);
					ocean_assert(nextX + 1u >= width || *(data - 1) <= *(data + 1));
				}
			}
			else if (nextOffsetX == 0)
			{
				ocean_assert(useCenterValue == 1);

				if constexpr (tPositiveThreshold)
				{
					ocean_assert(*data >= intThreshold);
					ocean_assert(nextX >= 1u || *data >= *(data - 1));
					ocean_assert(nextX + 1u >= width || *data >= *(data + 1));
				}
				else
				{
					ocean_assert(*data <= intThreshold);
					ocean_assert(nextX >= 1u || *data <= *(data - 1));
					ocean_assert(nextX + 1u >= width || *data <= *(data + 1));
				}
			}
			else
			{
				ocean_assert(nextOffsetX == 1);

				ocean_assert(useRightValue == 1);
				ocean_assert(nextX + 1u < width);

				if constexpr (tPositiveThreshold)
				{
					ocean_assert(*(data + 1) >= intThreshold);
					ocean_assert(*(data + 1) > *data);
					ocean_assert(nextX == 0u || *(data + 1) > *(data - 1));
				}
				else
				{
					ocean_assert(*(data + 1) <= intThreshold);
					ocean_assert(*(data + 1) < *data);
					ocean_assert(nextX == 0u || *(data + 1) < *(data - 1));
				}
			}

			ocean_assert(int(nextX) + nextOffsetX >= 0 && nextX + nextOffsetX < int(width));
		}

#endif

		data += nextOffsetX;
		*data = 0; // visited

		nextX += nextOffsetX; // nextX + nextOffsetX may actually be wrong
		pixelPositionsX[nextY] = nextX;

		data += tVerticalDirection * int(dataStrideElements);

		nextY += tVerticalDirection;
	}

	ocean_assert(pixelPositionsX[int(nextY) - tVerticalDirection] < width);

	// the previous y value
	return (unsigned int)(int(nextY) - tVerticalDirection);
}

template <typename T, const bool tPositiveThreshold>
inline bool LineDetectorULF::valueMatchesThreshold(const T& value, const T& threshold)
{
	if constexpr (tPositiveThreshold)
	{
		return value >= threshold;
	}
	else
	{
		return value <= threshold;
	}
}

constexpr unsigned int LineDetectorULF::barDetectorMinimalDelta()
{
	return 2u;
};

}

}

}

#endif // META_OCEAN_CV_DETECTOR_LINE_DETECTOR_ULF_H
