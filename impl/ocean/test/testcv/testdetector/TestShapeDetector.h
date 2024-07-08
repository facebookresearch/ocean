/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_SHAPE_DETECTOR_H
#define META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_SHAPE_DETECTOR_H

#include "ocean/test/testcv/testdetector/TestCVDetector.h"

#include "ocean/base/Frame.h"

#include "ocean/cv/detector/ShapeDetector.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

/**
 * This class implements tests for the Shape detector.
 * @ingroup testcvdetector
 */
class OCEAN_TEST_CV_DETECTOR_EXPORT TestShapeDetector : protected CV::Detector::ShapeDetector::PatternDetectorGradientVarianceBased
{
	protected:

		using LShape = CV::Detector::ShapeDetector::LShape;
		using TShape = CV::Detector::ShapeDetector::TShape;
		using XShape = CV::Detector::ShapeDetector::XShape;

		using LShapes = CV::Detector::ShapeDetector::LShapes;
		using TShapes = CV::Detector::ShapeDetector::TShapes;
		using XShapes = CV::Detector::ShapeDetector::XShapes;

	public:

		/**
		 * Definition of indivdual response types.
		 */
		enum ResponseType : uint32_t
		{
			/// The response is composed of the horizontal and vertical response.
			RT_HORIZONTAL_AND_VERTICAL = 0u,
			/// The response is just based on the horizontal response.
			RT_HORIZONTAL = 1u,
			/// The response is just based on the vertical response.
			RT_VERTICAL = 2u,
		};

		/**
		 * Definition of individual response visualization types.
		 */
		enum ResponseVisualization : uint32_t
		{
			/// The response values are visualized with linear normalization.
			RV_LINEAR = 0u,
			/// The response values are visualized with a logarithmic normalization.
			RV_LOGARITHMIC = 1u
		};

		/**
		 * This class provides the ground truth implementation of the gradient-based T-detector.
		 */
		class OCEAN_TEST_CV_DETECTOR_EXPORT GradientBasedDetector
		{
			public:

				/**
				 * Definition of individual stategies to determine edge response.
				 */
				enum EdgeResponseStrategy : uint32_t
				{
					/// The edge gradient (vertical and horizontal) is determined between the edge pixel and the direct neighboring pixel.
					ERS_GRADIENT_TO_NEIGHBOR = 0u,
					/// The edge gradient (vertical and horizontal) is determine between the edge pixel and the center pixel.
					ERS_GRADIENT_TO_CENTER = 1u
				};

				/**
				 * Definition of individual stategies to handle a minimal edge response.
				 */
				enum MinimalResponseStrategy : uint32_t
				{
					/// The minimal response is ignored.
					MRS_IGNORE = 0u,
					/// The minimal response is checked separately across the horizontal and vertical edge responses.
					MRS_SEPARATE_HORIZONTAL_VERTICAL = 1u,
					/// The minimal response is checked separately across opposite edges.
					MRS_SEPARATE_OPPOSITE_SIDE = 2u
				};

				/**
				 * Definition of individual penalty usages.
				 */
				enum PenaltyUsage : uint32_t
				{
					/// The penalty is subtracted from the edge response.
					PU_SUBTRACT = 0u,
					/// The penalty is used to normalize the edge response (by division).
					PU_DIVISION = 1u
				};

			public:

				/**
				 * Detects shapes in a given image.
				 * @param yFrame The frame in which the shapes will be detected, must be valid
				 * @param threshold The minimal threshold for a detected shape, with range (0, infinity)
				 * @param responseMultiplicationFactor An explicit post-processing multiplicatio factor for detector responses, with range (0, infinity)
				 * @param lShapes The resulting detected L-shapes
				 * @param tShapes The resulting detected T-shapes
				 * @param xShapes The resulting detected X-shapes
				 * @param sign The sign of the shape to be detected, -1 for shapes with dark edges and bright environment, 1 for shapes with bright edges and dark environment
				 * @param shapeWidth The width of the shapes, in pixel, with range [shapeBottomBand * 2 + shapeStepSize, infinity), must be odd
				 * @param shapeHeight The height of the shapes, in pixel, with range [shapeBottomBand + shapeStepSize, infinity)
				 * @param shapeStepSize The step size of the shapes, in pixel, with range [1, infinity), must be odd
				 * @param shapeTopBand The top band size of the shapes, in pixel, with range [1, infinity)
				 * @param shapeBottomBand The bottom band size of the shapes, in pixel, with range [1, infinity)
				 * @param responseType The response type to be used for shape detection
				 * @param penaltyFactor The multiplication factor for the penalty value, with range (0, infinity)
				 * @param minimalEdgeResponse The minimal edge response the detector must create to accept the response as candidate, with range [0, infinity)
				 * @param nonMaximumSupressionRadius The radius which is applied during non-maximum-suppression, with range [1, infinity)
				 * @param edgeResponseStrategy The response strategy for edges which will be applied
				 * @param minimalResponseStrategy The strategy for minimal responses to be applied
				 * @param penaltyUsage The penalty mode to be applied
				 * @param fResponseTopDown Optional resulting detector response image for the top-down detector (not rotated)
				 * @param fResponseBottomUp Optional resulting detector response image for the bottom-up detector (rotated by 180 degree)
				 */
				static void detectShapes(const Frame& yFrame, const double threshold, const double responseMultiplicationFactor, LShapes& lShapes, TShapes& tShapes, XShapes& xShapes, const int sign, const unsigned int shapeWidth, const unsigned int shapeHeight, const unsigned int shapeStepSize, const unsigned int shapeTopBand, const unsigned int shapeBottomBand, const ResponseType responseType, const double penaltyFactor, const unsigned int minimalEdgeResponse, const double nonMaximumSupressionRadius, const EdgeResponseStrategy edgeResponseStrategy, const MinimalResponseStrategy minimalResponseStrategy, const PenaltyUsage penaltyUsage, Frame* fResponseTopDown = nullptr, Frame* fResponseBottomUp = nullptr);

				/**
				 * Determines the gradient-based T-shape detector response.
				 * @param yFrame The frame in which the detector will be applied, must have pixel format FORMAT_Y8
				 * @param x The horizontal position of the T-shape within the frame, with range [0, yFrame.width())
				 * @param y The vertical position of the T-shape wihtin the frame, with range [0, yFrame.height())
				 * @param sign The sign of the shape to be detected, -1 for shapes with dark edges and bright environment, 1 for shapes with bright edges and dark environment
				 * @param shapeWidth The width of the shapes, in pixel, with range [shapeBottomBand * 2 + shapeStepSize, infinity), must be odd
				 * @param shapeHeight The height of the shapes, in pixel, with range [shapeBottomBand + shapeStepSize, infinity)
				 * @param shapeStepSize The step size of the shapes, in pixel, with range [1, infinity), must be odd
				 * @param shapeTopBand The top band size of the shapes, in pixel, with range [1, infinity)
				 * @param shapeBottomBand The bottom band size of the shapes, in pixel, with range [1, infinity)
				 * @param responseType The response type to be used for shape detection
				 * @param penaltyFactor The multiplication factor for the penalty value, with range (0, infinity)
				 * @param minimalEdgeResponse The minimal edge response the detector must create to accept the response as candidate, with range [0, infinity)
				 * @param edgeResponseStrategy The response strategy for edges which will be applied
				 * @param minimalResponseStrategy The strategy for minimal responses to be applied
				 * @param penaltyUsage The penalty mode to be applied
				 * @return The reponse of the T-shape detector
				 */
				static double tShapeDetectorResponse(const Frame& yFrame, const unsigned int x, const unsigned int y, const int sign, const unsigned int shapeWidth, const unsigned int shapeHeight, const unsigned int shapeStepSize, const unsigned int shapeTopBand, const unsigned int shapeBottomBand, const ResponseType responseType, const double penaltyFactor, const unsigned int minimalEdgeResponse, const EdgeResponseStrategy edgeResponseStrategy, const MinimalResponseStrategy minimalResponseStrategy, const PenaltyUsage penaltyUsage);
		};

		/**
		 * This class provides the ground truth implementation of the variance-based T-detector.
		 */
		class OCEAN_TEST_CV_DETECTOR_EXPORT VarianceBasedDetector
		{
			public:

				/**
				 * Definition of individual stategies to determine the gradient response.
				 */
				enum GradientResponseStrategy : uint32_t
				{
					/// The foreground response is based on the maximum absolute difference.
					GRS_MAX_ABSOLUTE_DIFFERENCE = 0u,
					/// The foreground response is based on the sum of absolute differences.
					GRS_SUM_ABSOLUTE_DIFFERENCES = 1u,
				};

				/**
				 * Definition of individual band strategies.
				 */
				enum BandStrategy : uint32_t
				{
					/// The band variance is determined based on a joining all band blocks.
					BS_JOINED = 0u,
					/// The band variance is determined based on the average variance of all band blocks.
					BS_SEPARATE_AVERAGE = 1u,
					/// The band variance is determined based on the maximum variance of all band blocks.
					BS_SEPARATE_MAX = 2u,
					/// The band variance is not used.
					BS_SKIP = 3u
				};

				/**
				 * Definition of individual threshold strategies.
				 */
				enum ThresholdStrategy : uint32_t
				{
					/// The threshold value is used as defined.
					TS_VALUE = 0u,
					/// The threshold is based on 65% of the sorted top 100 shapes.
					TS_BASED_ON_TOP_100_65 = 1u,
					/// The threshold is based on 55% of the sorted top 75 shapes.
					TS_BASED_ON_TOP_75_55 = 2u,
				};

			public:

				/**
				 * Detects shapes in a given image.
				 * @param yFrame The frame in which the shapes will be detected, must be valid
				 * @param threshold The minimal threshold for a detected shape, with range (0, infinity)
				 * @param responseMultiplicationFactor An explicit post-processing multiplicatio factor for detector responses, with range (0, infinity)
				 * @param lShapes The resulting detected L-shapes
				 * @param tShapes The resulting detected T-shapes
				 * @param xShapes The resulting detected X-shapes
				 * @param shapeWidth The width of the shapes, in pixel, with range [shapeBottomBand * 2 + shapeStepSize, infinity), must be odd
				 * @param shapeHeight The height of the shapes, in pixel, with range [shapeBottomBand + shapeStepSize, infinity)
				 * @param shapeStepSize The step size of the shapes, in pixel, with range [1, infinity), must be odd
				 * @param shapeTopBand The top band size of the shapes, in pixel, with range [1, infinity)
				 * @param shapeBottomBand The bottom band size of the shapes, in pixel, with range [1, infinity)
				 * @param responseType The response type to be used for shape detection
				 * @param minimalGradient The minimal gradient which is necessary to accept a response, with range [0, 256)
				 * @param varianceFactor The multiplication factor for the variance, with range (0, infinity)
				 * @param minimalVariance The minimal variance value used as lower boundary, with range (0, infinity)
				 * @param maximalRatio The maximal ratio between horizontal and vertical responses to accept a response, with range [1, infinity)
				 * @param nonMaximumSupressionRadius The radius which is applied during non-maximum-suppression, with range [1, infinity)
				 * @param thresholdStrategy The strategy for thresholding the shapes
				 * @param gradientResponseStrategy The strategy for gradient responses to be applied
				 * @param bandStrategy The strategy how to determine the variances for the bands
				 * @param fResponseTopDown Optional resulting detector response image for the top-down detector (not rotated)
				 * @param fResponseBottomUp Optional resulting detector response image for the bottom-up detector (rotated by 180 degree)
				 */
				static void detectShapes(const Frame& yFrame, const double threshold, const double responseMultiplicationFactor, LShapes& lShapes, TShapes& tShapes, XShapes& xShapes, const unsigned int shapeWidth, const unsigned int shapeHeight, const unsigned int shapeStepSize, const unsigned int shapeTopBand, const unsigned int shapeBottomBand, const ResponseType responseType, const double minimalGradient, const double varianceFactor, const double minimalVariance, const double maximalRatio, const double nonMaximumSupressionRadius, const ThresholdStrategy thresholdStrategy, const GradientResponseStrategy gradientResponseStrategy, const BandStrategy bandStrategy, Frame* fResponseTopDown = nullptr, Frame* fResponseBottomUp = nullptr);

				/**
				 * Determines the gradient-based T-shape detector response.
				 * @param linedIntegral The lined integral image of the original yFrame, must be valid
				 * @param linedIntegralSquared The lined integral squared image of the original yFrame, must be valid
				 * @param width The width of the original yFrame, in pixel, with range [1, infinity)
				 * @param height The height of the original yFrame, in pixel, with range [1, infinity)
				 * @param x The horizontal location for which the T-shape response will be determined, in pixel, with range [0, width - 1]
				 * @param y The vertical location for which the T-shape response will be determined, in pixel, with range [0, height - 1]
				 * @param shapeWidth The width of the shapes, in pixel, with range [shapeBottomBand * 2 + shapeStepSize, infinity), must be odd
				 * @param shapeHeight The height of the shapes, in pixel, with range [shapeBottomBand + shapeStepSize, infinity)
				 * @param shapeStepSize The step size of the shapes, in pixel, with range [1, infinity), must be odd
				 * @param shapeTopBand The top band size of the shapes, in pixel, with range [1, infinity)
				 * @param shapeBottomBand The bottom band size of the shapes, in pixel, with range [1, infinity)
				 * @param responseType The response type to be used for shape detection
				 * @param minimalGradient The minimal gradient which is necessary to accept a response, with range [0, 256)
				 * @param varianceFactor The multiplication factor for the variance, with range (0, infinity)
				 * @param minimalVariance The minimal variance value used as lower boundary, with range (0, infinity)
				 * @param maximalRatio The maximal ratio between horizontal and vertical responses to accept a response, with range [1, infinity)
				 * @param gradientResponseStrategy The strategy for gradient responses to be applied
				 * @param bandStrategy The strategy how to determine the variances for the bands
				 */
				static double tShapeDetectorResponse(const uint32_t* linedIntegral, const uint64_t* linedIntegralSquared, const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y, const unsigned int shapeWidth, const unsigned int shapeHeight, const unsigned int shapeStepSize, const unsigned int shapeTopBand, const unsigned int shapeBottomBand, const ResponseType responseType, const double minimalGradient, const double varianceFactor, const double minimalVariance, const double maximalRatio, const GradientResponseStrategy gradientResponseStrategy, const BandStrategy bandStrategy);
		};

		/**
		 * This class provides the ground truth implementation of the gradient&variance-based T-detector.
		 * The shape detector has the following geometry:
		 * <pre>
		 *                                T-shape width
		 *                  <--------------------------------------->
		 *
		 *                   ---------------------------------------         ^
		 *                  |                                       |        |  top band
		 *                  |                                       |        V
		 *              ^   |#######################################|    ^
		 *              |   |                                       |    |
		 *              |   |                   X                   |    |  shapeStepSize
		 *              |   |                                       |    |
		 *              |   |################       ################|    V
		 *    T-shape   |   |               #       #               |        ^
		 *    height    |   |               #       #               |        |  bottom band
		 *              |    --------       #       #       --------         V
		 *              |            |      #       #      |
		 *              |            |      #       #      |
		 *              |            |      #       #      |
		 *              |            |      #       #      |
		 *              |            |      #       #      |
		 *              |            |      #       #      |
		 *              |            |      #       #      |
		 *              |            |      #       #      |
		 *              V             ---------------------
		 *
		 * X: position of the T-shape
		 * #: edges of the T-shape
		 * </pre>
		 */
		class OCEAN_TEST_CV_DETECTOR_EXPORT GradientVarianceBasedDetector
		{
			public:

				/**
				 * Definition of individual band strategies.
				 */
				enum BandStrategy : uint32_t
				{
					/// The band variance is not used.
					BS_SKIP = 0u,
					/// The band variance is divided.
					BS_DIVIDE,
					/// The band variance is divided and subtracted.
					BS_SUBTRACT_AND_DIVIDE,
					/// Last (exclusive) band strategy value.
					BS_END
				};

				/**
				 * Definition of individual optimization strategies.
				 */
				enum OptimizationStrategy : uint32_t
				{
					/// No optimization.
					OS_NONE = 0u,
					/// Applying symmetric responses to allow response recycling, top and bottom band need to be identical, four horizontal response (two top and bottom blocks), vertical blocks have same height.
					OS_SYMMETRIC_RESPONSES_FOUR_HORIZONTAL_SAME_VERTICAL,
					/// Applying symmetric responses to allow response recycling, top and bottom band need to be identical, four horizontal response (two top and bottom blocks), vertical blocks have different height.
					OS_SYMMETRIC_RESPONSES_FOUR_HORIZONTAL_DIFFERENT_VERTICAL,
					/// Applying symmetric responses to allow response recycling, top and bottom band need to be identical, two horizontal repsonse (one top and one bottom block), vertical blocks have same height.
					OS_SYMMETRIC_RESPONSES_TWO_HORIZONTAL_SAME_VERTICAL,
					/// Applying symmetric responses to allow response recycling, top and bottom band need to be identical, two horizontal repsonse (one top and one bottom block), vertical blocks have different height.
					OS_SYMMETRIC_RESPONSES_TWO_HORIZONTAL_DIFFERENT_VERTICAL,
					/// Last (exclusive) optimization strategy value.
					OS_END
				};

			public:

				/**
				 * Detects standard shapes in a given image.
				 * @param yFrame The frame in which the shapes will be detected, must be valid
				 * @param threshold The minimal threshold for a detected shape, with range (0, infinity)
				 * @param responseMultiplicationFactor An explicit post-processing multiplication factor for detector responses, with range (0, infinity)
				 * @param lShapes The resulting detected L-shapes
				 * @param tShapes The resulting detected T-shapes
				 * @param xShapes The resulting detected X-shapes
				 * @param sign The sign of the shape to be detected, -1 for shapes with dark edges and bright environment, 1 for shapes with bright edges and dark environment, 0 to accept shapes with both signs
				 * @param shapeWidth The width of the shapes, in pixel, with range [shapeBottomBand * 2 + shapeStepSize, infinity), must be odd
				 * @param shapeHeight The height of the shapes, in pixel, with range [shapeBottomBand + shapeStepSize, infinity)
				 * @param shapeStepSize The step size of the shapes, in pixel, with range [1, infinity), must be odd
				 * @param shapeTopBand The top band size of the shapes, in pixel, with range [1, infinity)
				 * @param shapeBottomBand The bottom band size of the shapes, in pixel, with range [1, infinity)
				 * @param responseType The response type to be used for shape detection
				 * @param minimalGradient The minimal gradient which is necessary to accept a response, with range [0, 256)
				 * @param maximalResponseRatio The maximal ratio between horizontal and vertical responses to accept a response, with range [1, infinity)
				 * @param bandStrategy The band strategy to be applied
				 * @param optimizationStrategy The optimization strategy to be applied
				 * @param nonMaximumSupressionRadius The radius which is applied during non-maximum-suppression, with range [1, infinity)
				 * @param fResponseTopDown Optional resulting detector response image for the top-down detector (not rotated)
				 * @param fResponseBottomUp Optional resulting detector response image for the bottom-up detector (rotated by 180 degree)
				 */
				static void detectShapes(const Frame& yFrame, const double threshold, const double responseMultiplicationFactor, LShapes& lShapes, TShapes& tShapes, XShapes& xShapes, const int sign, const unsigned int shapeWidth, const unsigned int shapeHeight, const unsigned int shapeStepSize, const unsigned int shapeTopBand, const unsigned int shapeBottomBand, const ResponseType responseType, const double minimalGradient, const double maximalResponseRatio, const BandStrategy bandStrategy, const OptimizationStrategy optimizationStrategy, const double nonMaximumSupressionRadius, Frame* fResponseTopDown = nullptr, Frame* fResponseBottomUp = nullptr);

				/**
				 * Detects modified shapes in a given image.
				 * The modified shapes do not have a bar-edge roof, but a step-edge roof.
				 * @param yFrame The frame in which the shapes will be detected, must be valid
				 * @param threshold The minimal threshold for a detected shape, with range (0, infinity)
				 * @param responseMultiplicationFactor An explicit post-processing multiplication factor for detector responses, with range (0, infinity)
				 * @param lShapes The resulting detected L-shapes
				 * @param tShapes The resulting detected T-shapes
				 * @param xShapes The resulting detected X-shapes
				 * @param sign The sign of the shape to be detected, -1 for shapes with dark edges and bright environment, 1 for shapes with bright edges and dark environment, 0 to accept shapes with both signs
				 * @param shapeWidth The width of the shapes, in pixel, with range [shapeBottomBand * 2 + shapeStepSize, infinity), must be odd
				 * @param shapeHeight The height of the shapes, in pixel, with range [shapeBottomBand + shapeStepSize, infinity)
				 * @param shapeStepSize The step size of the shapes, in pixel, with range [1, infinity), must be odd
				 * @param shapeTopBand The top band size of the shapes, in pixel, with range [1, infinity)
				 * @param shapeBottomBand The bottom band size of the shapes, in pixel, with range [1, infinity)
				 * @param responseType The response type to be used for shape detection
				 * @param minimalGradient The minimal gradient which is necessary to accept a response, with range [0, 256)
				 * @param maximalResponseRatio The maximal ratio between horizontal and vertical responses to accept a response, with range [1, infinity)
				 * @param bandStrategy The band strategy to be applied
				 * @param nonMaximumSupressionRadius The radius which is applied during non-maximum-suppression, with range [1, infinity)
				 * @param fResponseTopDown Optional resulting detector response image for the top-down detector (not rotated)
				 * @param fResponseBottomUp Optional resulting detector response image for the bottom-up detector (rotated by 180 degree)
				 */
				static void detectShapesModified(const Frame& yFrame, const double threshold, const double responseMultiplicationFactor, LShapes& lShapes, TShapes& tShapes, XShapes& xShapes, const int sign, const unsigned int shapeWidth, const unsigned int shapeHeight, const unsigned int shapeStepSize, const unsigned int shapeTopBand, const unsigned int shapeBottomBand, const ResponseType responseType, const double minimalGradient, const double maximalResponseRatio, const BandStrategy bandStrategy, const double nonMaximumSupressionRadius, Frame* fResponseTopDown = nullptr, Frame* fResponseBottomUp = nullptr);

				/**
				 * Determines the gradient&variance-based T-shape detector response.
				 * @param linedIntegral The lined integral image of the original yFrame, must be valid
				 * @param linedIntegralSquared The lined integral squared image of the original yFrame, must be valid
				 * @param width The width of the original yFrame, in pixel, with range [1, infinity)
				 * @param height The height of the original yFrame, in pixel, with range [1, infinity)
				 * @param x The horizontal location for which the T-shape response will be determined, in pixel, with range [0, width - 1]
				 * @param y The vertical location for which the T-shape response will be determined, in pixel, with range [0, height - 1]
				 * @param sign The sign of the shape to be detected, -1 for shapes with dark edges and bright environment, 1 for shapes with bright edges and dark environment, 0 to accept shapes with both signs
				 * @param shapeWidth The width of the shapes, in pixel, with range [shapeBottomBand * 2 + shapeStepSize, infinity), must be odd
				 * @param shapeHeight The height of the shapes, in pixel, with range [shapeBottomBand + shapeStepSize, infinity)
				 * @param shapeStepSize The step size of the shapes, in pixel, with range [1, infinity), must be odd
				 * @param shapeTopBand The top band size of the shapes, in pixel, with range [1, infinity)
				 * @param shapeBottomBand The bottom band size of the shapes, in pixel, with range [1, infinity)
				 * @param responseType The response type to be used for shape detection
				 * @param minimalGradient The minimal gradient which is necessary to accept a response, with range [0, 256)
				 * @param maximalResponseRatio The maximal ratio between horizontal and vertical responses to accept a response, with range [1, infinity)
				 * @param bandStrategy The band strategy to be applied
				 * @param optimizationStrategy The optimization strategy to be applied
				 * @param horizontalResponse Optional resulting horizontal response
				 * @param verticalResponse Optional vertical horizontal response
				 */
				static double tShapeDetectorResponse(const uint32_t* linedIntegral, const uint64_t* linedIntegralSquared, const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y, const int sign, const unsigned int shapeWidth, const unsigned int shapeHeight, const unsigned int shapeStepSize, const unsigned int shapeTopBand, const unsigned int shapeBottomBand, const ResponseType responseType, const double minimalGradient, const double maximalResponseRatio, const BandStrategy bandStrategy, const OptimizationStrategy optimizationStrategy, double* horizontalResponse = nullptr, double* verticalResponse = nullptr);

				/**
				 * Determines the gradient&variance-based T-shape detector response.
				 * The modified T-shapes do not have a bar-edge roof, but a step-edge roof.
				 * @param linedIntegral The lined integral image of the original yFrame, must be valid
				 * @param linedIntegralSquared The lined integral squared image of the original yFrame, must be valid
				 * @param width The width of the original yFrame, in pixel, with range [1, infinity)
				 * @param height The height of the original yFrame, in pixel, with range [1, infinity)
				 * @param x The horizontal location for which the T-shape response will be determined, in pixel, with range [0, width - 1]
				 * @param y The vertical location for which the T-shape response will be determined, in pixel, with range [0, height - 1]
				 * @param sign The sign of the shape to be detected, -1 for shapes with dark edges and bright environment, 1 for shapes with bright edges and dark environment, 0 to accept shapes with both signs
				 * @param shapeWidth The width of the shapes, in pixel, with range [shapeBottomBand * 2 + shapeStepSize, infinity), must be odd
				 * @param shapeHeight The height of the shapes, in pixel, with range [shapeBottomBand + shapeStepSize, infinity)
				 * @param shapeStepSize The step size of the shapes, in pixel, with range [1, infinity), must be odd
				 * @param shapeTopBand The top band size of the shapes, in pixel, with range [1, infinity)
				 * @param shapeBottomBand The bottom band size of the shapes, in pixel, with range [1, infinity)
				 * @param responseType The response type to be used for shape detection
				 * @param minimalGradient The minimal gradient which is necessary to accept a response, with range [0, 256)
				 * @param maximalResponseRatio The maximal ratio between horizontal and vertical responses to accept a response, with range [1, infinity)
				 * @param bandStrategy The band strategy to be applied
				 * @param horizontalResponse Optional resulting horizontal response
				 * @param verticalResponse Optional vertical horizontal response
				 */
				static double tShapeDetectorResponseModified(const uint32_t* linedIntegral, const uint64_t* linedIntegralSquared, const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y, const int sign, const unsigned int shapeWidth, const unsigned int shapeHeight, const unsigned int shapeStepSize, const unsigned int shapeTopBand, const unsigned int shapeBottomBand, const ResponseType responseType, const double minimalGradient, const double maximalResponseRatio, const BandStrategy bandStrategy, double* horizontalResponse = nullptr, double* verticalResponse = nullptr);
		};

	public:

		/**
		 * Tests the shape detector functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Test the gradient-based T-shape detector.
		 * @param testDuration The duration in seconds for which this test will be run, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testGradientBasedTShapeDetector(const double testDuration);

		/**
		 * Test the horizontal response of the gradient&variance-based T-shape detector.
		 * @param testDuration The duration in seconds for which this test will be run, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testGradientVarianceBasedTShapeDetectorHorizontalResponse(const double testDuration);

		/**
		 * Test the vertical response of the gradient&variance-based T-shape detector.
		 * @param testDuration The duration in seconds for which this test will be run, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testGradientVarianceBasedTShapeDetectorVerticalResponse(const double testDuration);

		/**
		 * Test the gradient&variance-based T-shape detector.
		 * @param testDuration The duration in seconds for which this test will be run, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testGradientVarianceBasedTShapeDetector(const double testDuration);

	protected:

		/**
		 * Compares two T-shapes and returns whether the left shape has a higher score.
		 * @param left The left T-shape to compare
		 * @param right The right T-shape to compare
		 * @return True, if so
		 */
		static inline bool compareTshapes(const TShape& left, const TShape& right);
};

inline bool TestShapeDetector::compareTshapes(const TShape& left, const TShape& right)
{
	return left.score() > right.score();
}

} // namespace TestDetector

} // namespace TestCV

} // namespce Test

} // namespace Ocean

#endif // META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_SHAPE_DETECTOR_H
