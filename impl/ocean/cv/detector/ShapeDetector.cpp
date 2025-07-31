/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/ShapeDetector.h"

#include "ocean/cv/FrameConverterY8.h"
#include "ocean/cv/FrameFilterGradient.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"
#include "ocean/cv/IntegralImage.h"
#include "ocean/cv/NEON.h"

#include "ocean/cv/detector/HemiCube.h"
#include "ocean/cv/detector/LineDetectorULF.h"
#include "ocean/cv/detector/Utilities.h"

#include "ocean/geometry/RANSAC.h"
#include "ocean/geometry/SpatialDistribution.h"

#include <set>

namespace Ocean
{

namespace CV
{

namespace Detector
{

bool ShapeDetector::XShape::verifyShape(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const bool darkShape, const unsigned int minimalValueRange, const unsigned int sampleOffset, const unsigned int samples, const unsigned int yFramePaddingElements) const
{
	ocean_assert(yFrame != nullptr);
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(minimalValueRange < 255u);
	ocean_assert(samples >= 3u);

	// a valid X-shape has similar color intensities in all directions
	// an invalid X-shape has one direction with color intensities not matching to the remaining three directions

	constexpr unsigned int numberDirections = 4u;

	const Vector2 directions[numberDirections] =
	{
		direction0_,
		-direction0_,
		direction1_,
		-direction1_,
	};

	std::vector<uint8_t> values(samples * numberDirections);

	uint8_t minValue = 255u;
	uint8_t maxValue = 0u;

	for (unsigned int nDirection = 0u; nDirection < numberDirections; ++nDirection)
	{
		for (unsigned int nSample = 0u; nSample < samples; ++nSample)
		{
			const Vector2 location = position_ + directions[nDirection] * Scalar(sampleOffset + nSample);

			if (location.x() < Scalar(0) || location.y() < Scalar(0) || location.x() >= Scalar(width) || location.y() >= Scalar(height))
			{
				return false;
			}

			uint8_t value;
			CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<1u, CV::PC_TOP_LEFT>(yFrame, width, height, yFramePaddingElements, location, &value);

			if (value > maxValue)
			{
				maxValue = value;
			}

			if (value < minValue)
			{
				minValue = value;
			}

			if (darkShape)
			{
				value = 255u - value;
			}

			values[samples * nDirection + nSample] = value;
		}
	}

	ocean_assert(minValue <= maxValue);

	if ((unsigned int)(maxValue - minValue) < minimalValueRange)
	{
		// the value range is too small so that we have a valid shape
		return true;
	}

	const std::vector<uint8_t> copyValues(values);

	// we select that threshold to cover half of one sample direction

	const unsigned int samples_2 = (samples + 1u) / 2u;

	std::nth_element(values.begin(), values.begin() + samples_2, values.end());

	const uint8_t threshold = values[samples_2];

	unsigned int samplesBelowThreshold[numberDirections] = {0u};

	for (unsigned int nDirection = 0u; nDirection < numberDirections; ++nDirection)
	{
		for (unsigned int nSample = 0u; nSample < samples; ++nSample)
		{
			if (copyValues[samples * nDirection + nSample] <= threshold)
			{
				samplesBelowThreshold[nDirection]++;
			}
		}
	}

	unsigned int directionsWithSampleBelowThreshold = 0u;

	for (unsigned int nDirection = 0u; nDirection < numberDirections; ++nDirection)
	{
		if (samplesBelowThreshold[nDirection] != 0u)
		{
			directionsWithSampleBelowThreshold++;
		}
	}

	ocean_assert(directionsWithSampleBelowThreshold >= 1u);
	return directionsWithSampleBelowThreshold >= 2u;
}

void ShapeDetector::PatternDetectorGradientBased::detectShapes(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, LShapes& lShapes, TShapes& tShapes, XShapes& xShapes, const int sign, const double minimalThreshold, const unsigned int shapeWidth, const unsigned int shapeHeight, const unsigned int stepSize, const unsigned int topBand, const unsigned int bottomBand, const unsigned int minimalDelta, const unsigned int framePaddingElements)
{
	ocean_assert(yFrame != nullptr);
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(lShapes.empty() && tShapes.empty() && xShapes.empty());
	ocean_assert(sign != 0);
	ocean_assert(minimalThreshold >= 0.0f);

	ocean_assert(shapeWidth % 2u == 1u);
	ocean_assert(bottomBand + stepSize < shapeWidth);
	ocean_assert(bottomBand + stepSize < shapeHeight);

	ocean_assert(topBand >= 1u && bottomBand >= 1u);
	ocean_assert(stepSize != 0u && stepSize % 2u == 1u);

	if (width <= 20u || height <= 20u)
	{
		return;
	}

	constexpr unsigned int nonMaximumSupressionRadius = 9u;

	Frame linedIntegralHorizontalSignedGradientFrame(FrameType(width, height + 1u, FrameType::genericPixelFormat<int32_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT));
	Frame linedIntegralHorizontalAbsoluteGradientFrame(FrameType(width, height + 1u, FrameType::genericPixelFormat<uint32_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT));

	Frame linedIntegralVerticalSignedGradientFrame(FrameType(width + 1u, height, FrameType::genericPixelFormat<int32_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT));
	Frame linedIntegralVerticalAbsoluteGradientFrame(FrameType(width + 1u, height, FrameType::genericPixelFormat<uint32_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT));

	{
		CV::FrameFilterGradient::filterHorizontal1x2LinedIntegralImage<uint8_t, int32_t, false>(yFrame, width, height, linedIntegralHorizontalSignedGradientFrame.data<int32_t>(), framePaddingElements, linedIntegralHorizontalSignedGradientFrame.paddingElements());
		CV::FrameFilterGradient::filterHorizontal1x2LinedIntegralImage<uint8_t, uint32_t, true>(yFrame, width, height, linedIntegralHorizontalAbsoluteGradientFrame.data<uint32_t>(), framePaddingElements, linedIntegralHorizontalAbsoluteGradientFrame.paddingElements());
		CV::FrameFilterGradient::filterVertical2x1LinedIntegralImage<uint8_t, int32_t, false>(yFrame, width, height, linedIntegralVerticalSignedGradientFrame.data<int32_t>(), framePaddingElements, linedIntegralVerticalSignedGradientFrame.paddingElements());
		CV::FrameFilterGradient::filterVertical2x1LinedIntegralImage<uint8_t, uint32_t, true>(yFrame, width, height, linedIntegralVerticalAbsoluteGradientFrame.data<uint32_t>(), framePaddingElements, linedIntegralVerticalAbsoluteGradientFrame.paddingElements());

		CV::NonMaximumSuppression<double> nonMaximumSuppressionAlignedTShape(width, height);

		for (unsigned int y = 0u; y < height; ++y)
		{
			for (unsigned int x = 0u; x < width; ++x)
			{
				// aligned T-shape
				const double response = tShapeResponse(linedIntegralHorizontalSignedGradientFrame.constdata<int32_t>(), linedIntegralHorizontalAbsoluteGradientFrame.constdata<uint32_t>(), linedIntegralVerticalSignedGradientFrame.constdata<int32_t>(), linedIntegralVerticalAbsoluteGradientFrame.constdata<uint32_t>(), width, height, x, y, sign, shapeWidth, shapeHeight, stepSize, topBand, bottomBand, minimalDelta, linedIntegralHorizontalSignedGradientFrame.paddingElements(), linedIntegralHorizontalAbsoluteGradientFrame.paddingElements(), linedIntegralVerticalSignedGradientFrame.paddingElements(), linedIntegralVerticalAbsoluteGradientFrame.paddingElements());

				if (response >= minimalThreshold)
				{
					nonMaximumSuppressionAlignedTShape.addCandidate(x, y, response);
				}
			}
		}

		NonMaximumSuppression<double>::StrengthPositions<unsigned int, double> shapes = nonMaximumSuppressionAlignedTShape.suppressNonMaximum<unsigned int, double>(1u, width - 2u, 1u, height - 2u, nullptr);
		shapes = CV::NonMaximumSuppression<double>::suppressNonMaximum<unsigned int, double, true>(width, height, shapes, nonMaximumSupressionRadius);

		tShapes.reserve(tShapes.size() + shapes.size());

		for (size_t n = 0; n < shapes.size(); ++n)
		{
			// determining the sub-pixel location of the shape

			const unsigned int& x = shapes[n].x();
			const unsigned int& y = shapes[n].y();

			const double topResponses[3] =
			{
				tShapeResponse(linedIntegralHorizontalSignedGradientFrame.constdata<int32_t>(), linedIntegralHorizontalAbsoluteGradientFrame.data<uint32_t>(), linedIntegralVerticalSignedGradientFrame.data<int32_t>(), linedIntegralVerticalAbsoluteGradientFrame.data<uint32_t>(), width, height, x - 1u, y - 1u, sign, shapeWidth, shapeHeight, stepSize, topBand, bottomBand, minimalDelta),
				tShapeResponse(linedIntegralHorizontalSignedGradientFrame.constdata<int32_t>(), linedIntegralHorizontalAbsoluteGradientFrame.data<uint32_t>(), linedIntegralVerticalSignedGradientFrame.data<int32_t>(), linedIntegralVerticalAbsoluteGradientFrame.data<uint32_t>(), width, height, x + 0u, y - 1u, sign, shapeWidth, shapeHeight, stepSize, topBand, bottomBand, minimalDelta),
				tShapeResponse(linedIntegralHorizontalSignedGradientFrame.constdata<int32_t>(), linedIntegralHorizontalAbsoluteGradientFrame.data<uint32_t>(), linedIntegralVerticalSignedGradientFrame.data<int32_t>(), linedIntegralVerticalAbsoluteGradientFrame.data<uint32_t>(), width, height, x + 1u, y - 1u, sign, shapeWidth, shapeHeight, stepSize, topBand, bottomBand, minimalDelta)
			};

			const double centerResponses[3] =
			{
				tShapeResponse(linedIntegralHorizontalSignedGradientFrame.constdata<int32_t>(), linedIntegralHorizontalAbsoluteGradientFrame.data<uint32_t>(), linedIntegralVerticalSignedGradientFrame.data<int32_t>(), linedIntegralVerticalAbsoluteGradientFrame.data<uint32_t>(), width, height, x - 1u, y, sign, shapeWidth, shapeHeight, stepSize, topBand, bottomBand, minimalDelta),
				tShapeResponse(linedIntegralHorizontalSignedGradientFrame.constdata<int32_t>(), linedIntegralHorizontalAbsoluteGradientFrame.data<uint32_t>(), linedIntegralVerticalSignedGradientFrame.data<int32_t>(), linedIntegralVerticalAbsoluteGradientFrame.data<uint32_t>(), width, height, x + 0u, y, sign, shapeWidth, shapeHeight, stepSize, topBand, bottomBand, minimalDelta),
				tShapeResponse(linedIntegralHorizontalSignedGradientFrame.constdata<int32_t>(), linedIntegralHorizontalAbsoluteGradientFrame.data<uint32_t>(), linedIntegralVerticalSignedGradientFrame.data<int32_t>(), linedIntegralVerticalAbsoluteGradientFrame.data<uint32_t>(), width, height, x + 1u, y, sign, shapeWidth, shapeHeight, stepSize, topBand, bottomBand, minimalDelta)
			};

			const double bottomResponses[3] =
			{
				tShapeResponse(linedIntegralHorizontalSignedGradientFrame.constdata<int32_t>(), linedIntegralHorizontalAbsoluteGradientFrame.data<uint32_t>(), linedIntegralVerticalSignedGradientFrame.data<int32_t>(), linedIntegralVerticalAbsoluteGradientFrame.data<uint32_t>(), width, height, x - 1u, y + 1u, sign, shapeWidth, shapeHeight, stepSize, topBand, bottomBand, minimalDelta),
				tShapeResponse(linedIntegralHorizontalSignedGradientFrame.constdata<int32_t>(), linedIntegralHorizontalAbsoluteGradientFrame.data<uint32_t>(), linedIntegralVerticalSignedGradientFrame.data<int32_t>(), linedIntegralVerticalAbsoluteGradientFrame.data<uint32_t>(), width, height, x + 0u, y + 1u, sign, shapeWidth, shapeHeight, stepSize, topBand, bottomBand, minimalDelta),
				tShapeResponse(linedIntegralHorizontalSignedGradientFrame.constdata<int32_t>(), linedIntegralHorizontalAbsoluteGradientFrame.data<uint32_t>(), linedIntegralVerticalSignedGradientFrame.data<int32_t>(), linedIntegralVerticalAbsoluteGradientFrame.data<uint32_t>(), width, height, x + 1u, y + 1u, sign, shapeWidth, shapeHeight, stepSize, topBand, bottomBand, minimalDelta)
			};

			ocean_assert(centerResponses[1] == shapes[n].strength());

			Vector2 offset(0, 0);
			CV::NonMaximumSuppression<double>::determinePrecisePeakLocation2<Scalar>(topResponses, centerResponses, bottomResponses, offset);

			tShapes.emplace_back(Vector2(Scalar(shapes[n].x()), Scalar(shapes[n].y())) + offset, Vector2(0, 1), Scalar(shapes[n].strength()));
		}
	}

	{
		// rotating the image and running the detector again

		Frame yRotatedFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
		CV::FrameConverterY8::convertY8ToY8(yFrame, yRotatedFrame.data<uint8_t>(), width, height, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, framePaddingElements, yRotatedFrame.paddingElements());

		CV::FrameFilterGradient::filterHorizontal1x2LinedIntegralImage<uint8_t, int32_t, false>(yRotatedFrame.constdata<uint8_t>(), width, height, linedIntegralHorizontalSignedGradientFrame.data<int32_t>(), yRotatedFrame.paddingElements(), linedIntegralHorizontalSignedGradientFrame.paddingElements());
		CV::FrameFilterGradient::filterHorizontal1x2LinedIntegralImage<uint8_t, uint32_t, true>(yRotatedFrame.constdata<uint8_t>(), width, height, linedIntegralHorizontalAbsoluteGradientFrame.data<uint32_t>(), yRotatedFrame.paddingElements(), linedIntegralHorizontalAbsoluteGradientFrame.paddingElements());
		CV::FrameFilterGradient::filterVertical2x1LinedIntegralImage<uint8_t, int32_t, false>(yRotatedFrame.constdata<uint8_t>(), width, height, linedIntegralVerticalSignedGradientFrame.data<int32_t>(), yRotatedFrame.paddingElements(), linedIntegralVerticalSignedGradientFrame.paddingElements());
		CV::FrameFilterGradient::filterVertical2x1LinedIntegralImage<uint8_t, uint32_t, true>(yRotatedFrame.constdata<uint8_t>(), width, height, linedIntegralVerticalAbsoluteGradientFrame.data<uint32_t>(), yRotatedFrame.paddingElements(), linedIntegralVerticalAbsoluteGradientFrame.paddingElements());

		CV::NonMaximumSuppression<double> nonMaximumSuppressionAlignedTShape(width, height);

		for (unsigned int y = 0u; y < height; ++y)
		{
			for (unsigned int x = 0u; x < width; ++x)
			{
				// aligned T-shape
				const double response = tShapeResponse(linedIntegralHorizontalSignedGradientFrame.constdata<int32_t>(), linedIntegralHorizontalAbsoluteGradientFrame.constdata<uint32_t>(), linedIntegralVerticalSignedGradientFrame.constdata<int32_t>(), linedIntegralVerticalAbsoluteGradientFrame.constdata<uint32_t>(), width, height, x, y, sign, shapeWidth, shapeHeight, stepSize, topBand, bottomBand, minimalDelta, linedIntegralHorizontalSignedGradientFrame.paddingElements(), linedIntegralHorizontalAbsoluteGradientFrame.paddingElements(), linedIntegralVerticalSignedGradientFrame.paddingElements(), linedIntegralVerticalAbsoluteGradientFrame.paddingElements());

				if (response >= minimalThreshold)
				{
					nonMaximumSuppressionAlignedTShape.addCandidate(x, y, response);
				}
			}
		}

		NonMaximumSuppression<double>::StrengthPositions<unsigned int, double> shapes = nonMaximumSuppressionAlignedTShape.suppressNonMaximum<unsigned int, double>(1u, width - 2u, 1u, height - 2u, nullptr);
		shapes = CV::NonMaximumSuppression<double>::suppressNonMaximum<unsigned int, double, true>(width, height, shapes, nonMaximumSupressionRadius);

		tShapes.reserve(tShapes.size() + shapes.size());

		for (size_t n = 0; n < shapes.size(); ++n)
		{
			// determining the sub-pixel location of the shape

			const unsigned int& x = shapes[n].x();
			const unsigned int& y = shapes[n].y();

			const double topResponses[3] =
			{
				tShapeResponse(linedIntegralHorizontalSignedGradientFrame.constdata<int32_t>(), linedIntegralHorizontalAbsoluteGradientFrame.data<uint32_t>(), linedIntegralVerticalSignedGradientFrame.constdata<int32_t>(), linedIntegralVerticalAbsoluteGradientFrame.data<uint32_t>(), width, height, x - 1u, y - 1u, sign, shapeWidth, shapeHeight, stepSize, topBand, bottomBand, minimalDelta),
				tShapeResponse(linedIntegralHorizontalSignedGradientFrame.constdata<int32_t>(), linedIntegralHorizontalAbsoluteGradientFrame.data<uint32_t>(), linedIntegralVerticalSignedGradientFrame.constdata<int32_t>(), linedIntegralVerticalAbsoluteGradientFrame.data<uint32_t>(), width, height, x + 0u, y - 1u, sign, shapeWidth, shapeHeight, stepSize, topBand, bottomBand, minimalDelta),
				tShapeResponse(linedIntegralHorizontalSignedGradientFrame.constdata<int32_t>(), linedIntegralHorizontalAbsoluteGradientFrame.data<uint32_t>(), linedIntegralVerticalSignedGradientFrame.constdata<int32_t>(), linedIntegralVerticalAbsoluteGradientFrame.data<uint32_t>(), width, height, x + 1u, y - 1u, sign, shapeWidth, shapeHeight, stepSize, topBand, bottomBand, minimalDelta)
			};

			const double centerResponses[3] =
			{
				tShapeResponse(linedIntegralHorizontalSignedGradientFrame.constdata<int32_t>(), linedIntegralHorizontalAbsoluteGradientFrame.data<uint32_t>(), linedIntegralVerticalSignedGradientFrame.constdata<int32_t>(), linedIntegralVerticalAbsoluteGradientFrame.data<uint32_t>(), width, height, x - 1u, y, sign, shapeWidth, shapeHeight, stepSize, topBand, bottomBand, minimalDelta),
				tShapeResponse(linedIntegralHorizontalSignedGradientFrame.constdata<int32_t>(), linedIntegralHorizontalAbsoluteGradientFrame.data<uint32_t>(), linedIntegralVerticalSignedGradientFrame.constdata<int32_t>(), linedIntegralVerticalAbsoluteGradientFrame.data<uint32_t>(), width, height, x + 0u, y, sign, shapeWidth, shapeHeight, stepSize, topBand, bottomBand, minimalDelta),
				tShapeResponse(linedIntegralHorizontalSignedGradientFrame.constdata<int32_t>(), linedIntegralHorizontalAbsoluteGradientFrame.data<uint32_t>(), linedIntegralVerticalSignedGradientFrame.constdata<int32_t>(), linedIntegralVerticalAbsoluteGradientFrame.data<uint32_t>(), width, height, x + 1u, y, sign, shapeWidth, shapeHeight, stepSize, topBand, bottomBand, minimalDelta)
			};

			const double bottomResponses[3] =
			{
				tShapeResponse(linedIntegralHorizontalSignedGradientFrame.constdata<int32_t>(), linedIntegralHorizontalAbsoluteGradientFrame.data<uint32_t>(), linedIntegralVerticalSignedGradientFrame.constdata<int32_t>(), linedIntegralVerticalAbsoluteGradientFrame.data<uint32_t>(), width, height, x - 1u, y + 1u, sign, shapeWidth, shapeHeight, stepSize, topBand, bottomBand, minimalDelta),
				tShapeResponse(linedIntegralHorizontalSignedGradientFrame.constdata<int32_t>(), linedIntegralHorizontalAbsoluteGradientFrame.data<uint32_t>(), linedIntegralVerticalSignedGradientFrame.constdata<int32_t>(), linedIntegralVerticalAbsoluteGradientFrame.data<uint32_t>(), width, height, x + 0u, y + 1u, sign, shapeWidth, shapeHeight, stepSize, topBand, bottomBand, minimalDelta),
				tShapeResponse(linedIntegralHorizontalSignedGradientFrame.constdata<int32_t>(), linedIntegralHorizontalAbsoluteGradientFrame.data<uint32_t>(), linedIntegralVerticalSignedGradientFrame.constdata<int32_t>(), linedIntegralVerticalAbsoluteGradientFrame.data<uint32_t>(), width, height, x + 1u, y + 1u, sign, shapeWidth, shapeHeight, stepSize, topBand, bottomBand, minimalDelta)
			};

			ocean_assert(centerResponses[1] == shapes[n].strength());

			Vector2 offset(0, 0);
			CV::NonMaximumSuppression<double>::determinePrecisePeakLocation2<Scalar>(topResponses, centerResponses, bottomResponses, offset);

			tShapes.emplace_back(Vector2(Scalar(width - shapes[n].x() - 1u), Scalar(height - shapes[n].y() - 1u)) - offset, Vector2(0, -1), Scalar(shapes[n].strength()));
		}
	}

	ShapeDetector::postAdjustShapes(width, height, lShapes, tShapes, xShapes);
}

double ShapeDetector::PatternDetectorGradientBased::tShapeResponse(const int32_t* linedIntegralHorizontalSignedGradient, const uint32_t* linedIntegralHorizontalAbsoluteGradient, const int32_t* linedIntegralVerticalSignedGradient, const uint32_t* linedIntegralVerticalAbsoluteGradient, const unsigned int imageWidth, const unsigned int imageHeight, const unsigned int x, const unsigned int y, const int sign, const unsigned int shapeWidth, const unsigned int shapeHeight, const unsigned int stepSize, const unsigned int topBand, const unsigned int bottomBand, const unsigned int minimalDelta, const unsigned int horizontalSignedGradientPaddingElements, const unsigned int horizontalAbsoluteGradientPaddingElements, const unsigned int verticalSignedGradientPaddingElements, const unsigned int verticalAbsoluteGradientPaddingElements)
{
	ocean_assert(linedIntegralHorizontalSignedGradient != nullptr);
	ocean_assert(linedIntegralHorizontalAbsoluteGradient != nullptr);

	ocean_assert(linedIntegralVerticalSignedGradient != nullptr);
	ocean_assert(linedIntegralVerticalAbsoluteGradient != nullptr);

	const unsigned int horizontalSignedGradientStrideElements = imageWidth + horizontalSignedGradientPaddingElements;
	const unsigned int horizontalAbsoluteGradientStrideElements = imageWidth + horizontalAbsoluteGradientPaddingElements;

	const unsigned int verticalSignedGrandientStrideElements = (imageWidth + 1u) + verticalSignedGradientPaddingElements;
	const unsigned int verticalAbsoluteGrandientStrideElements = (imageWidth + 1u) + verticalAbsoluteGradientPaddingElements;

	/*
	 *                                T-shape width
	 *                  <--------------------------------------->
	 *
	 *                   ---------------------------------------         ^
	 *                  |                                       |        |  top band
	 *                  |                                       |        V
	 *              ^   |#######################################|    ^
	 *              |   |                                       |    |
	 *              |   |                   X                   |    |  stepSize
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
	 */

	ocean_assert(shapeWidth < imageWidth);
	ocean_assert(shapeHeight + topBand < imageHeight);

	const unsigned int shapeWidth_2 = shapeWidth / 2u;
	const unsigned int stepSize_2 = stepSize / 2u;

	if (x < shapeWidth_2 || y < topBand + stepSize_2 || x + shapeWidth_2 >= imageWidth || y + shapeHeight - stepSize_2 >= imageHeight)
	{
		return 0.0;
	}

	/*
		*    |                                       |  <- first outer row:      y - stepSize_2 - 1
		*    |#######################################|  <- last inner step row:  y - stepSize_2
		*    |                                       |
		*    |                   X                   |  <- shape position:       y
		*    |                                       |
		*/

	/*
		*                  T-shape width
		*    |                                       |
		*    |                   X                   | <- shape position:       y
		*    |                                       |
		*    |################       ################| <- last inner step row:  y + stepSize_2
		*    |               #       #               | <- first outer step row: y + stepSize_2 + 1
		*
		*                    <------->
		*                    stepSize
		*/

	const int32_t horizontalEdgeResponsePositive = IntegralImage::linedIntegralSum(linedIntegralVerticalSignedGradient, verticalSignedGrandientStrideElements, x - shapeWidth_2, y - stepSize_2 - 1u, shapeWidth, 1u); // top horizontal edge

	const int32_t horizontalEdgeResponseNegative = IntegralImage::linedIntegralSum(linedIntegralVerticalSignedGradient, verticalSignedGrandientStrideElements, x - shapeWidth_2, y + stepSize_2, (shapeWidth - stepSize) / 2u, 1u) // bottom horizontal edge left
													+ IntegralImage::linedIntegralSum(linedIntegralVerticalSignedGradient, verticalSignedGrandientStrideElements, x + stepSize_2 + 1u, y + stepSize_2, (shapeWidth - stepSize) / 2u, 1u); // bottom horizontal edge right

	if (abs(horizontalEdgeResponsePositive) < int(shapeWidth * minimalDelta)
			|| abs(horizontalEdgeResponseNegative) < int((shapeWidth - stepSize) * 2u * minimalDelta))
	{
		return 0.0;
	}

	int32_t horizontalEdgeResponse = horizontalEdgeResponsePositive - horizontalEdgeResponseNegative;

	if ((sign < 0 && horizontalEdgeResponse > 0) || (sign > 0 && horizontalEdgeResponse < 0.0))
	{
		return 0.0;
	}
	else
	{
		horizontalEdgeResponse = abs(horizontalEdgeResponse);
	}

	if (horizontalEdgeResponse < int(shapeWidth * 2u * minimalDelta))
	{
		return 0.0;
	}

	const int32_t penaltyHorizontalEdgeResponse = IntegralImage::linedIntegralSum(linedIntegralHorizontalAbsoluteGradient, horizontalAbsoluteGradientStrideElements, x - shapeWidth_2,  y, shapeWidth - 1u, 1u) * stepSize;

	/*
		*
		*          X               <- shape position:       y
		*
		*   ####       ####        <- last inner step row:  y + stepSize_2
		*      #       #			<- first outer step row: y + stepSize_2 + 1
		*      #       #
		*
		*      ^
		*      |
		*   first inner:  x - stepSize_2
		*/

		const int32_t verticalEdgeResponsePositive = IntegralImage::linedIntegralSum(linedIntegralHorizontalSignedGradient, horizontalSignedGradientStrideElements, x - stepSize_2 - 1u, y + stepSize_2 + 1u, 1u, shapeHeight - stepSize); // left vertical edge
 	const int32_t verticalEdgeResponseNegative = IntegralImage::linedIntegralSum(linedIntegralHorizontalSignedGradient, horizontalSignedGradientStrideElements, x + stepSize_2, y + stepSize_2 + 1u, 1u, shapeHeight - stepSize); // right vertical edge

 	if (abs(verticalEdgeResponsePositive) < int((shapeHeight - stepSize) * minimalDelta)
 		|| abs(verticalEdgeResponseNegative) < int((shapeHeight - stepSize) * minimalDelta))
 	{
 		return 0.0;
 	}

 	int32_t verticalEdgeResponse = verticalEdgeResponsePositive - verticalEdgeResponseNegative;

	if ((sign < 0 && verticalEdgeResponse > 0.0) || (sign > 0 && verticalEdgeResponse < 0.0))
	{
		return 0.0;
	}
	else
	{
		verticalEdgeResponse = abs(verticalEdgeResponse);
	}

	if (verticalEdgeResponse < int((shapeHeight - stepSize) * 2u * minimalDelta))
	{
		return 0.0;
	}

	const int32_t penaltyVerticalEdgeResponse = IntegralImage::linedIntegralSum(linedIntegralVerticalAbsoluteGradient, verticalAbsoluteGrandientStrideElements, x,  y, 1u, shapeHeight - stepSize_2 - 1u) * stepSize;

	/*
		*     ---------------------------------------    <- first top band row:  y - stepSize_2 - topBand
		*    |                                       |
		*    |                                       |   <- last top band row:   y - stepSize_2 - 1
		*    |#######################################|
		*    |                                       |
		*    |                   X                   |
		*    |                                       |
		*    |################       ################|
		*    |               #       #               |   <- first bottom band row: y + stepSize_2 + 1
		*    |               #       #               |
		*     --------       #       #       --------    <- inner bottom band row: y + stepSize_2 + bottomBand
		*             |      #       #      |
		*             |      #       #      |
		*             |      #       #      |
		*             |      #       #      |
		*             |      #       #      |
		*             |      #       #      |
		*             |      #       #      |
		*             |      #       #      |
		*              ---------------------            <- last bottom band row: y + shapeHeight - stepSize_2 - 1
		*                   ^
		*                   |
		*                last band column:  x - stepSize_2 - 1
		*/

	const int32_t backgroundResponse = IntegralImage::linedIntegralSum(linedIntegralHorizontalAbsoluteGradient, horizontalAbsoluteGradientStrideElements, x - shapeWidth_2, y - stepSize_2 - topBand, shapeWidth - 1u, topBand) // top band
										+ IntegralImage::linedIntegralSum(linedIntegralVerticalAbsoluteGradient, verticalAbsoluteGrandientStrideElements, x - shapeWidth_2, y - stepSize_2 - topBand, shapeWidth, topBand - 1u)

										+ IntegralImage::linedIntegralSum(linedIntegralHorizontalAbsoluteGradient, horizontalAbsoluteGradientStrideElements, x - shapeWidth_2, y + stepSize_2 + 1u, (shapeWidth - stepSize) / 2u - 1u, bottomBand) // bottom band (left upper area)
										+ IntegralImage::linedIntegralSum(linedIntegralVerticalAbsoluteGradient, verticalAbsoluteGrandientStrideElements, x - shapeWidth_2, y + stepSize_2 + 1u, (shapeWidth - stepSize) / 2u, bottomBand - 1u)

										+ IntegralImage::linedIntegralSum(linedIntegralHorizontalAbsoluteGradient, horizontalAbsoluteGradientStrideElements, x + stepSize_2 + 1u, y + stepSize_2 + 1u, (shapeWidth - stepSize) / 2u - 1u, bottomBand) // bottom band (right upper area)
										+ IntegralImage::linedIntegralSum(linedIntegralVerticalAbsoluteGradient, verticalAbsoluteGrandientStrideElements, x + stepSize_2 + 1u, y + stepSize_2 + 1u, (shapeWidth - stepSize) / 2u, bottomBand - 1u)

										+ IntegralImage::linedIntegralSum(linedIntegralHorizontalAbsoluteGradient, horizontalAbsoluteGradientStrideElements, x - stepSize_2 - bottomBand, y + stepSize_2 + bottomBand + 1u, bottomBand - 1u, shapeHeight - stepSize - bottomBand) // bottom band (left lower area)
										+ IntegralImage::linedIntegralSum(linedIntegralVerticalAbsoluteGradient, verticalAbsoluteGrandientStrideElements, x - stepSize_2 - bottomBand, y + stepSize_2 + bottomBand, bottomBand, shapeHeight - stepSize - bottomBand)

										+ IntegralImage::linedIntegralSum(linedIntegralHorizontalAbsoluteGradient, horizontalAbsoluteGradientStrideElements, x + stepSize_2 + 1u, y + stepSize_2 + bottomBand + 1u, bottomBand - 1u, shapeHeight - stepSize - bottomBand) // bottom band (right lower area)
										+ IntegralImage::linedIntegralSum(linedIntegralVerticalAbsoluteGradient, verticalAbsoluteGrandientStrideElements, x + stepSize_2 + 1u, y + stepSize_2 + bottomBand, bottomBand, shapeHeight - stepSize - bottomBand);

	const int32_t edgeResponse = max(0, horizontalEdgeResponse - penaltyHorizontalEdgeResponse) * max(0, verticalEdgeResponse - penaltyVerticalEdgeResponse);

	return double(edgeResponse) / double(std::max(1, backgroundResponse));
}

void ShapeDetector::PatternDetectorVarianceBased::detectShapes(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, LShapes& lShapes, TShapes& tShapes, XShapes& xShapes, const float minimalThreshold, const unsigned int shapeWidth, const unsigned int shapeHeight, const unsigned int stepSize, const unsigned int topBand, const unsigned int bottomBand, const unsigned int framePaddingElements)
{
	ocean_assert(yFrame != nullptr);
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(lShapes.empty() && tShapes.empty() && xShapes.empty());

	ocean_assert(minimalThreshold >= 0.0f);

	ocean_assert(shapeWidth % 2u == 1u);
	ocean_assert(bottomBand + stepSize < shapeWidth);
	ocean_assert(bottomBand + stepSize < shapeHeight);

	ocean_assert(topBand >= 1u && bottomBand >= 1u);
	ocean_assert(stepSize != 0u && stepSize % 2u == 1u);

	constexpr unsigned int nonMaximumSupressionRadius = 9u;

	Frame linedIntegral(FrameType(width + 1u, height + 1u, FrameType::genericPixelFormat<uint32_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT));
	Frame linedIntegralSquared(FrameType(linedIntegral, FrameType::genericPixelFormat<uint64_t, 1u>()));

	{
		// top-down

		CV::IntegralImage::createLinedImage<uint8_t, uint32_t, 1u>(yFrame, linedIntegral.data<uint32_t>(), width, height, framePaddingElements, linedIntegral.paddingElements());
		CV::IntegralImage::createLinedImageSquared<uint8_t, uint64_t, 1u>(yFrame, linedIntegralSquared.data<uint64_t>(), width, height, framePaddingElements, linedIntegralSquared.paddingElements());

		CV::NonMaximumSuppression<float> nonMaximumSuppressionAlignedTShape(width, height);

		for (unsigned int y = 0u; y < height; ++y)
		{
			for (unsigned int x = 0u; x < width; ++x)
			{
				const float response = tShapeResponse(linedIntegral.constdata<uint32_t>(), linedIntegralSquared.constdata<uint64_t>(), width, height, x, y, shapeWidth, shapeHeight, stepSize, topBand, bottomBand, linedIntegral.paddingElements(), linedIntegralSquared.paddingElements());

				if (response >= minimalThreshold)
				{
					nonMaximumSuppressionAlignedTShape.addCandidate(x, y, response);
				}
			}
		}

		CV::NonMaximumSuppression<float>::StrengthPositions<unsigned int, float> shapes = nonMaximumSuppressionAlignedTShape.suppressNonMaximum<unsigned int, float, false /*tStrictMaximum*/>(1u, width - 2u, 1u, height - 2u, nullptr, nullptr);
		shapes = CV::NonMaximumSuppression<float>::suppressNonMaximum<unsigned int, float, true>(width, height, shapes, nonMaximumSupressionRadius);

		tShapes.reserve(tShapes.size() + shapes.size());

		for (size_t n = 0; n < shapes.size(); ++n)
		{
			// determining the sub-pixel location of the shape

			const unsigned int& x = shapes[n].x();
			const unsigned int& y = shapes[n].y();

			const float topResponses[3] =
			{
				tShapeResponse(linedIntegral.constdata<uint32_t>(), linedIntegralSquared.constdata<uint64_t>(), width, height, x - 1u, y - 1u, shapeWidth, shapeHeight, stepSize, topBand, bottomBand),
				tShapeResponse(linedIntegral.constdata<uint32_t>(), linedIntegralSquared.constdata<uint64_t>(), width, height, x + 0u, y - 1u, shapeWidth, shapeHeight, stepSize, topBand, bottomBand),
				tShapeResponse(linedIntegral.constdata<uint32_t>(), linedIntegralSquared.constdata<uint64_t>(), width, height, x + 1u, y - 1u, shapeWidth, shapeHeight, stepSize, topBand, bottomBand)
			};

			const float centerResponses[3] =
			{
				tShapeResponse(linedIntegral.constdata<uint32_t>(), linedIntegralSquared.constdata<uint64_t>(), width, height, x - 1u, y, shapeWidth, shapeHeight, stepSize, topBand, bottomBand),
				tShapeResponse(linedIntegral.constdata<uint32_t>(), linedIntegralSquared.constdata<uint64_t>(), width, height, x + 0u, y, shapeWidth, shapeHeight, stepSize, topBand, bottomBand),
				tShapeResponse(linedIntegral.constdata<uint32_t>(), linedIntegralSquared.constdata<uint64_t>(), width, height, x + 1u, y, shapeWidth, shapeHeight, stepSize, topBand, bottomBand)
			};

			const float bottomResponses[3] =
			{
				tShapeResponse(linedIntegral.constdata<uint32_t>(), linedIntegralSquared.constdata<uint64_t>(), width, height, x - 1u, y + 1u, shapeWidth, shapeHeight, stepSize, topBand, bottomBand),
				tShapeResponse(linedIntegral.constdata<uint32_t>(), linedIntegralSquared.constdata<uint64_t>(), width, height, x + 0u, y + 1u, shapeWidth, shapeHeight, stepSize, topBand, bottomBand),
				tShapeResponse(linedIntegral.constdata<uint32_t>(), linedIntegralSquared.constdata<uint64_t>(), width, height, x + 1u, y + 1u, shapeWidth, shapeHeight, stepSize, topBand, bottomBand)
			};

			ocean_assert(centerResponses[1] == shapes[n].strength());

			Vector2 offset(0, 0);
			CV::NonMaximumSuppression<float>::determinePrecisePeakLocation2<Scalar>(topResponses, centerResponses, bottomResponses, offset);

			tShapes.emplace_back(Vector2(Scalar(x), Scalar(y)) + offset, Vector2(0, 1), Scalar(shapes[n].strength()));
		}
	}

	{
		// bottom-up

		Frame yRotatedFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
		CV::FrameConverterY8::convertY8ToY8(yFrame, yRotatedFrame.data<uint8_t>(), width, height, CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, framePaddingElements, 0u);

		CV::IntegralImage::createLinedImage<uint8_t, uint32_t, 1u>(yRotatedFrame.constdata<uint8_t>(), linedIntegral.data<uint32_t>(), yRotatedFrame.width(), yRotatedFrame.height(), yRotatedFrame.paddingElements(), linedIntegral.paddingElements());
		CV::IntegralImage::createLinedImageSquared<uint8_t, uint64_t, 1u>(yRotatedFrame.constdata<uint8_t>(), linedIntegralSquared.data<uint64_t>(), yRotatedFrame.width(), yRotatedFrame.height(), yRotatedFrame.paddingElements(), linedIntegralSquared.paddingElements());

		CV::NonMaximumSuppression<float> nonMaximumSuppressionAlignedTShape(width, height);

		for (unsigned int y = 0u; y < height; ++y)
		{
			for (unsigned int x = 0u; x < width; ++x)
			{
				const float response = tShapeResponse(linedIntegral.constdata<uint32_t>(), linedIntegralSquared.constdata<uint64_t>(), width, height, x, y, shapeWidth, shapeHeight, stepSize, topBand, bottomBand, linedIntegral.paddingElements(), linedIntegralSquared.paddingElements());

				if (response >= minimalThreshold)
				{
					nonMaximumSuppressionAlignedTShape.addCandidate(x, y, response);
				}
			}
		}

		CV::NonMaximumSuppression<float>::StrengthPositions<unsigned int, float> shapes = nonMaximumSuppressionAlignedTShape.suppressNonMaximum<unsigned int, float>(1u, width - 2u, 1u, height - 2u, nullptr, nullptr);
		shapes = CV::NonMaximumSuppression<float>::suppressNonMaximum<unsigned int, float, true>(width, height, shapes, nonMaximumSupressionRadius);

		tShapes.reserve(tShapes.size() + shapes.size());

		for (size_t n = 0; n < shapes.size(); ++n)
		{
			// determining the sub-pixel location of the shape

			const unsigned int& x = shapes[n].x();
			const unsigned int& y = shapes[n].y();

			const float topResponses[3] =
			{
				tShapeResponse(linedIntegral.constdata<uint32_t>(), linedIntegralSquared.constdata<uint64_t>(), width, height, x - 1u, y - 1u, shapeWidth, shapeHeight, stepSize, topBand, bottomBand),
				tShapeResponse(linedIntegral.constdata<uint32_t>(), linedIntegralSquared.constdata<uint64_t>(), width, height, x + 0u, y - 1u, shapeWidth, shapeHeight, stepSize, topBand, bottomBand),
				tShapeResponse(linedIntegral.constdata<uint32_t>(), linedIntegralSquared.constdata<uint64_t>(), width, height, x + 1u, y - 1u, shapeWidth, shapeHeight, stepSize, topBand, bottomBand)
			};

			const float centerResponses[3] =
			{
				tShapeResponse(linedIntegral.constdata<uint32_t>(), linedIntegralSquared.constdata<uint64_t>(), width, height, x - 1u, y, shapeWidth, shapeHeight, stepSize, topBand, bottomBand),
				tShapeResponse(linedIntegral.constdata<uint32_t>(), linedIntegralSquared.constdata<uint64_t>(), width, height, x + 0u, y, shapeWidth, shapeHeight, stepSize, topBand, bottomBand),
				tShapeResponse(linedIntegral.constdata<uint32_t>(), linedIntegralSquared.constdata<uint64_t>(), width, height, x + 1u, y, shapeWidth, shapeHeight, stepSize, topBand, bottomBand)
			};

			const float bottomResponses[3] =
			{
				tShapeResponse(linedIntegral.constdata<uint32_t>(), linedIntegralSquared.constdata<uint64_t>(), width, height, x - 1u, y + 1u, shapeWidth, shapeHeight, stepSize, topBand, bottomBand),
				tShapeResponse(linedIntegral.constdata<uint32_t>(), linedIntegralSquared.constdata<uint64_t>(), width, height, x + 0u, y + 1u, shapeWidth, shapeHeight, stepSize, topBand, bottomBand),
				tShapeResponse(linedIntegral.constdata<uint32_t>(), linedIntegralSquared.constdata<uint64_t>(), width, height, x + 1u, y + 1u, shapeWidth, shapeHeight, stepSize, topBand, bottomBand)
			};

			ocean_assert(centerResponses[1] == shapes[n].strength());

			Vector2 offset(0, 0);
			CV::NonMaximumSuppression<float>::determinePrecisePeakLocation2<Scalar>(topResponses, centerResponses, bottomResponses, offset);

			tShapes.emplace_back(Vector2(Scalar(yRotatedFrame.width() - x - 1u), Scalar(yRotatedFrame.height() - y - 1u)) - offset, Vector2(0, -1), Scalar(shapes[n].strength()));
		}
	}

	CV::Detector::ShapeDetector::postAdjustShapes(width, height, lShapes, tShapes, xShapes);
}

float ShapeDetector::PatternDetectorVarianceBased::tShapeResponse(const uint32_t* linedIntegral, const uint64_t* linedIntegralSquare, const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y, const unsigned int shapeWidth, const unsigned int shapeHeight, const unsigned int stepSize, const unsigned int topBand, const unsigned int bottomBand, const unsigned int linedIntegralPaddingElements, const unsigned int linedIntegralSquaredPaddingElements)
{
	ocean_assert(shapeWidth % 2u == 1u);
	ocean_assert(bottomBand + stepSize < shapeWidth);
	ocean_assert(bottomBand + stepSize < shapeHeight);

	ocean_assert(topBand >= 1u && bottomBand >= 1u);
	ocean_assert(stepSize != 0u && stepSize % 2u == 1u);

	const unsigned int shapeWidth_2 = shapeWidth / 2u;
	const unsigned int stepSize_2 = stepSize / 2u;

	if (shapeWidth > width || shapeHeight + topBand > height)
	{
		return 0.0f;
	}

	if (x < shapeWidth_2 || y < topBand + stepSize_2 || x >= width - shapeWidth_2 || y >= height - shapeHeight + stepSize_2)
	{
		return 0.0f;
	}

	const unsigned int linedIntegralStrideElements = width + 1u + linedIntegralPaddingElements;
	const unsigned int linedIntegralSquaredStrideElements = width + 1u + linedIntegralSquaredPaddingElements;


	// horizontal response

	const unsigned int xHorizontalStartA = x - shapeWidth_2;
	const unsigned int xHorizontalStartB = x + stepSize_2 + 1u;

	const float roofBandVariance = CV::IntegralImage::linedIntegralVariance<uint32_t, uint64_t, float>(linedIntegral, linedIntegralSquare, linedIntegralStrideElements, linedIntegralSquaredStrideElements, xHorizontalStartA, y - stepSize_2 - topBand, shapeWidth, topBand, xHorizontalStartA, y + stepSize_2 + 1u, shapeWidth_2 - stepSize_2, bottomBand, xHorizontalStartB, y + stepSize_2 + 1u, shapeWidth_2 - stepSize_2, bottomBand);

	// center row

	float roofVariance = 0.0f;
	float lastRoofMean = NumericF::minValue();

	float roofResponse = 0.0f;

	for (unsigned int yHorizontalStart = y - stepSize_2 - 1u; yHorizontalStart <= y + stepSize_2 + 1u; ++yHorizontalStart)
	{
		float mean = -1.0f;
		float variance = 1.0f;

		if (yHorizontalStart <= y + stepSize_2)
		{
			variance = CV::IntegralImage::linedIntegralVariance<uint32_t, uint64_t, float, true>(linedIntegral, linedIntegralSquare, linedIntegralStrideElements, linedIntegralSquaredStrideElements, xHorizontalStartA, yHorizontalStart, shapeWidth, 1u, &mean);
		}
		else
		{
			variance = CV::IntegralImage::linedIntegralVariance<uint32_t, uint64_t, float, true>(linedIntegral, linedIntegralSquare, linedIntegralStrideElements, linedIntegralSquaredStrideElements, xHorizontalStartA, yHorizontalStart, shapeWidth_2 - stepSize_2, 1u, xHorizontalStartB, yHorizontalStart, shapeWidth_2 - stepSize_2, 1u, &mean);
		}

		ocean_assert(mean >= 0.0f);
		ocean_assert(variance >= 0.0f);

		roofVariance += variance;

		if (lastRoofMean >= 0.0f)
		{
			roofResponse += NumericF::abs(mean - lastRoofMean);
		}

		lastRoofMean = mean;
	}

	constexpr float minimalGradientResponse = 2.0f;

	if (roofResponse < minimalGradientResponse)
	{
		roofResponse = 0.0f;
	}

	const float horizontalResponse = roofResponse * roofResponse / std::max(1.0f, roofVariance + roofBandVariance);


	// vertical response

	const unsigned int yVerticalStart = y + stepSize_2 + 1u;

	const float trunkBandVariance = CV::IntegralImage::linedIntegralVariance<uint32_t, uint64_t, float>(linedIntegral, linedIntegralSquare, linedIntegralStrideElements, linedIntegralSquaredStrideElements, x - stepSize_2 - bottomBand, yVerticalStart, bottomBand, shapeHeight - stepSize, x + stepSize_2 + 1u, yVerticalStart, bottomBand, shapeHeight - stepSize);

	// center columns

	float trunkVariance = 0.0f;
	float lastTrunkMean = NumericF::minValue();

	float trunkResponse = 0.0f;

	for (unsigned int xVerticalStart = x - stepSize_2 - 1u; xVerticalStart <= x + stepSize_2 + 1u; ++xVerticalStart)
	{
		float mean = -1.0f;
		float variance = -1.0f;

		if (xVerticalStart >= x - stepSize_2 && xVerticalStart <= x + stepSize_2)
		{
			variance = CV::IntegralImage::linedIntegralVariance<uint32_t, uint64_t, float, true>(linedIntegral, linedIntegralSquare, linedIntegralStrideElements, linedIntegralSquaredStrideElements, xVerticalStart, y, 1u, shapeHeight - stepSize_2, &mean);
		}
		else
		{
			variance = CV::IntegralImage::linedIntegralVariance<uint32_t, uint64_t, float, true>(linedIntegral, linedIntegralSquare, linedIntegralStrideElements, linedIntegralSquaredStrideElements, xVerticalStart, yVerticalStart, 1u, shapeHeight - stepSize, &mean);
		}

		ocean_assert(mean >= 0.0f);
		ocean_assert(variance >= 0.0f);

		trunkVariance += variance;

		if (lastTrunkMean >= 0.0f)
		{
			trunkResponse += NumericF::abs(mean - lastTrunkMean);
		}

		lastTrunkMean = mean;
	}

	if (trunkResponse < minimalGradientResponse)
	{
		trunkResponse = 0.0f;
	}

	const float verticalResponse = trunkResponse * trunkResponse / std::max(1.0f, trunkVariance + trunkBandVariance);

	constexpr float maximalRatio = 2.5f;

	if (horizontalResponse > maximalRatio * maximalRatio * verticalResponse || verticalResponse > maximalRatio * maximalRatio * horizontalResponse)
	{
		return 0.0f;
	}

	return horizontalResponse * verticalResponse;
}

void ShapeDetector::PatternDetectorGradientVarianceBased::detectShapesF(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, LShapes& lShapes, TShapes& tShapes, XShapes& xShapes, const int sign, const float minimalThreshold, const unsigned int framePaddingElements, Frame* topDownResponseFrame, Frame* bottomUpResponseFrame)
{
	ocean_assert(yFrame != nullptr);
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(lShapes.empty() && tShapes.empty() && xShapes.empty());

	ocean_assert(sign >= -1 && sign <= 1);

	ocean_assert(minimalThreshold >= 0.0f);
	ocean_assert(width * height <= 65536u);

	constexpr unsigned int nonMaximumSupressionRadius = 9u;

	Frame linedIntegralAndSquared(FrameType(width + 1u, height + 1u, FrameType::genericPixelFormat<uint32_t, 2u>(), FrameType::ORIGIN_UPPER_LEFT));
	CV::IntegralImage::createLinedImageAndSquared<uint8_t, uint32_t, 1u>(yFrame, linedIntegralAndSquared.data<uint32_t>(), width, height, framePaddingElements, linedIntegralAndSquared.paddingElements());

	constexpr unsigned int horizontalResponseAreaWidth = shapeWidth_;
	constexpr unsigned int horizontalResponseAreaHeight = shapeBandSize_ * 2u + shapeStepSize_;

	constexpr bool tSquaredResponse = false; // using deviation-based responses

	Frame horizontalResponses(FrameType(width - horizontalResponseAreaWidth + 1u, height - horizontalResponseAreaHeight + 1u, FrameType::genericPixelFormat<float, 1u>(), FrameType::ORIGIN_UPPER_LEFT));
	determineHorizontalResponsesF<tSquaredResponse>(linedIntegralAndSquared.constdata<uint32_t>(), width, height, horizontalResponses.data<float>(), linedIntegralAndSquared.paddingElements(), horizontalResponses.paddingElements());

	constexpr unsigned int verticalResponseAreaWidth = shapeBandSize_ * 2u + shapeStepSize_;
	constexpr unsigned int verticalResponseAreaHeight = shapeHeight_ - shapeStepSize_;

	Frame verticalResponses(FrameType(width - verticalResponseAreaWidth + 1u, height - verticalResponseAreaHeight + 1u, FrameType::genericPixelFormat<float, 1u>(), FrameType::ORIGIN_UPPER_LEFT));
	determineVerticalResponsesF<tSquaredResponse>(linedIntegralAndSquared.constdata<uint32_t>(), width, height, verticalResponses.data<float>(), linedIntegralAndSquared.paddingElements(), verticalResponses.paddingElements());

	const float adjustedMinimalThreshold = tSquaredResponse ? (minimalThreshold * 9.5f / 6.0f) : minimalThreshold; // 9.5 when using squared responses seems to be similar to 6.0 when using non-squared responses

	CV::NonMaximumSuppression<float> nonMaximumSuppression(width, height);

	if (topDownResponseFrame)
	{
		ocean_assert(horizontalResponses.isContinuous() && verticalResponses.isContinuous());

		determineTopDownResponsesF<tSquaredResponse, true>(horizontalResponses.constdata<float>(), verticalResponses.constdata<float>(), width, height, sign, adjustedMinimalThreshold, nonMaximumSuppression, topDownResponseFrame);
	}
	else
	{
		ocean_assert(horizontalResponses.isContinuous() && verticalResponses.isContinuous());

		determineTopDownResponsesF<tSquaredResponse, false>(horizontalResponses.constdata<float>(), verticalResponses.constdata<float>(), width, height, sign, adjustedMinimalThreshold, nonMaximumSuppression);
	}

	constexpr unsigned int nonMaximumBorderTopDownLeft = frameX_T_topDownResponseX() + 1u;
	constexpr unsigned int nonMaximumBorderTopDownRight = shapeWidth_2_ + 1u;
	constexpr unsigned int nonMaximumBorderTopDownTop = frameY_T_topDownResponseY() + 1u;
	constexpr unsigned int nonMaximumBorderTopDownBottom = shapeHeight_ - shapeStepSize_2_ + 1u;

	CV::NonMaximumSuppression<float>::StrengthPositions<unsigned int, float> shapes = nonMaximumSuppression.suppressNonMaximum<unsigned int, float, false /*tStrictMaximum*/>(nonMaximumBorderTopDownLeft, width - nonMaximumBorderTopDownLeft - nonMaximumBorderTopDownRight, nonMaximumBorderTopDownTop, height - nonMaximumBorderTopDownTop - nonMaximumBorderTopDownBottom, nullptr, nullptr);
	shapes = CV::NonMaximumSuppression<float>::suppressNonMaximum<unsigned int, float, true>(width, height, shapes, nonMaximumSupressionRadius);

	tShapes.reserve(tShapes.size() + shapes.size());

	for (size_t n = 0; n < shapes.size(); ++n)
	{
		// determining the sub-pixel location of the shape

		const unsigned int& x = shapes[n].x();
		const unsigned int& y = shapes[n].y();

		ocean_assert(x >= 1u && x <= width - 2u && y >= 1u && y <= height - 2u);

		const float* const horizontalResponsesTopLeft = horizontalResponses.constpixel<float>((unsigned int)(int(x) - frameX_T_topDownHorizontalResponseX() - 1), (unsigned int)(int(y) - frameY_T_topDownHorizontalResponseY() - 1));
		const float* const verticalResponsesTopLeft = verticalResponses.constpixel<float>((unsigned int)(int(x) - frameX_T_topDownVerticalResponseX() - 1), (unsigned int)(int(y) - frameY_T_topDownVerticalResponseY() - 1));

		const unsigned int horizontalResponsesStrideElements = horizontalResponses.strideElements();
		const unsigned int verticalResponsesStrideElements = verticalResponses.strideElements();

		const float topResponses[3] =
		{
			NumericF::abs(horizontalResponsesTopLeft[0] * verticalResponsesTopLeft[0]),
			NumericF::abs(horizontalResponsesTopLeft[1] * verticalResponsesTopLeft[1]),
			NumericF::abs(horizontalResponsesTopLeft[2] * verticalResponsesTopLeft[2])
		};

		const float centerResponses[3] =
		{
			NumericF::abs(horizontalResponsesTopLeft[horizontalResponsesStrideElements + 0u] * verticalResponsesTopLeft[verticalResponsesStrideElements + 0u]),
			NumericF::abs(horizontalResponsesTopLeft[horizontalResponsesStrideElements + 1u] * verticalResponsesTopLeft[verticalResponsesStrideElements + 1u]),
			NumericF::abs(horizontalResponsesTopLeft[horizontalResponsesStrideElements + 2u] * verticalResponsesTopLeft[verticalResponsesStrideElements + 2u])
		};

		const float bottomResponses[3] =
		{
			NumericF::abs(horizontalResponsesTopLeft[horizontalResponsesStrideElements * 2u + 0u] * verticalResponsesTopLeft[verticalResponsesStrideElements * 2u + 0u]),
			NumericF::abs(horizontalResponsesTopLeft[horizontalResponsesStrideElements * 2u + 1u] * verticalResponsesTopLeft[verticalResponsesStrideElements * 2u + 1u]),
			NumericF::abs(horizontalResponsesTopLeft[horizontalResponsesStrideElements * 2u + 2u] * verticalResponsesTopLeft[verticalResponsesStrideElements * 2u + 2u])
		};

		ocean_assert(centerResponses[1] == shapes[n].strength());

		Vector2 offset(0, 0);
		CV::NonMaximumSuppression<float>::determinePrecisePeakLocation2<Scalar>(topResponses, centerResponses, bottomResponses, offset);

		tShapes.emplace_back(Vector2(Scalar(x), Scalar(y)) + offset, Vector2(0, 1), Scalar(shapes[n].strength()));
	}

	nonMaximumSuppression.reset();

	if (bottomUpResponseFrame)
	{
		ocean_assert(horizontalResponses.isContinuous() && verticalResponses.isContinuous());

		determineBottomUpResponsesF<tSquaredResponse, true>(horizontalResponses.constdata<float>(), verticalResponses.constdata<float>(), width, height, sign, adjustedMinimalThreshold, nonMaximumSuppression, bottomUpResponseFrame);
	}
	else
	{
		ocean_assert(horizontalResponses.isContinuous() && verticalResponses.isContinuous());

		determineBottomUpResponsesF<tSquaredResponse, false>(horizontalResponses.constdata<float>(), verticalResponses.constdata<float>(), width, height, sign, adjustedMinimalThreshold, nonMaximumSuppression);
	}

	constexpr unsigned int nonMaximumBorderBottomUpLeft = frameX_T_bottomUpResponseX() + 1u;
	constexpr unsigned int nonMaximumBorderBottomUpRight = shapeWidth_2_ + 1u;
	constexpr unsigned int nonMaximumBorderBottomUpTop = frameY_T_bottomUpResponseY() + 1u;
	constexpr unsigned int nonMaximumBorderBottomUpBottom = shapeBandSize_ + shapeStepSize_2_ + 1u;

	shapes = nonMaximumSuppression.suppressNonMaximum<unsigned int, float, false /*tStrictMaximum*/>(nonMaximumBorderBottomUpLeft, width - nonMaximumBorderBottomUpLeft - nonMaximumBorderBottomUpRight, nonMaximumBorderBottomUpTop, height - nonMaximumBorderBottomUpTop - nonMaximumBorderBottomUpBottom, nullptr, nullptr);
	shapes = CV::NonMaximumSuppression<float>::suppressNonMaximum<unsigned int, float, true>(width, height, shapes, nonMaximumSupressionRadius);

	tShapes.reserve(tShapes.size() + shapes.size());

	for (size_t n = 0; n < shapes.size(); ++n)
	{
		// determining the sub-pixel location of the shape

		const unsigned int& x = shapes[n].x();
		const unsigned int& y = shapes[n].y();

		ocean_assert(x >= 1u && x <= width - 2u && y >= 1u && y <= height - 2u);

		const float* const horizontalResponsesTopLeft = horizontalResponses.constpixel<float>((unsigned int)(int(x) - frameX_T_bottomUpHorizontalResponseX() - 1), (unsigned int)(int(y) - frameY_T_bottomUpHorizontalResponseY() - 1));
		const float* const verticalResponsesTopLeft = verticalResponses.constpixel<float>((unsigned int)(int(x) - frameX_T_bottomUpVerticalResponseX() - 1), (unsigned int)(int(y) - frameY_T_bottomUpVerticalResponseY() - 1));

		const unsigned int horizontalResponsesStrideElements = horizontalResponses.strideElements();
		const unsigned int verticalResponsesStrideElements = verticalResponses.strideElements();

		const float topResponses[3] =
		{
			NumericF::abs(horizontalResponsesTopLeft[0] * verticalResponsesTopLeft[0]),
			NumericF::abs(horizontalResponsesTopLeft[1] * verticalResponsesTopLeft[1]),
			NumericF::abs(horizontalResponsesTopLeft[2] * verticalResponsesTopLeft[2])
		};

		const float centerResponses[3] =
		{
			NumericF::abs(horizontalResponsesTopLeft[horizontalResponsesStrideElements + 0u] * verticalResponsesTopLeft[verticalResponsesStrideElements + 0u]),
			NumericF::abs(horizontalResponsesTopLeft[horizontalResponsesStrideElements + 1u] * verticalResponsesTopLeft[verticalResponsesStrideElements + 1u]),
			NumericF::abs(horizontalResponsesTopLeft[horizontalResponsesStrideElements + 2u] * verticalResponsesTopLeft[verticalResponsesStrideElements + 2u])
		};

		const float bottomResponses[3] =
		{
			NumericF::abs(horizontalResponsesTopLeft[horizontalResponsesStrideElements * 2u + 0u] * verticalResponsesTopLeft[verticalResponsesStrideElements * 2u + 0u]),
			NumericF::abs(horizontalResponsesTopLeft[horizontalResponsesStrideElements * 2u + 1u] * verticalResponsesTopLeft[verticalResponsesStrideElements * 2u + 1u]),
			NumericF::abs(horizontalResponsesTopLeft[horizontalResponsesStrideElements * 2u + 2u] * verticalResponsesTopLeft[verticalResponsesStrideElements * 2u + 2u])
		};

		ocean_assert(centerResponses[1] == shapes[n].strength());

		Vector2 offset(0, 0);
		CV::NonMaximumSuppression<float>::determinePrecisePeakLocation2<Scalar>(topResponses, centerResponses, bottomResponses, offset);

		tShapes.emplace_back(Vector2(Scalar(x), Scalar(y)) + offset, Vector2(0, -1), Scalar(shapes[n].strength()));
	}

	CV::Detector::ShapeDetector::postAdjustShapes(width, height, lShapes, tShapes, xShapes);
}

void ShapeDetector::PatternDetectorGradientVarianceBased::detectShapesI(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, LShapes& lShapes, TShapes& tShapes, XShapes& xShapes, const int sign, const float minimalThreshold, const unsigned int framePaddingElements, Frame* topDownResponseFrame, Frame* bottomUpResponseFrame)
{
	ocean_assert(yFrame != nullptr);
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(lShapes.empty() && tShapes.empty() && xShapes.empty());

	ocean_assert(sign >= -1 && sign <= 1);

	ocean_assert(minimalThreshold >= 0.0f);

	if (width < 20u || height < 20u)
	{
		return;
	}

	constexpr unsigned int nonMaximumSupressionRadius = 9u;

	constexpr bool tUseSIMD = true;

	Frame horizontalResponses(FrameType(determineHorizontalResponseWidth(width), determineHorizontalResponseHeight(height), FrameType::genericPixelFormat<int32_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT));
	Frame verticalResponses(FrameType(determineVerticalResponseWidth(width), determineVerticalResponseHeight(height), FrameType::genericPixelFormat<int32_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT));

	if (width * height > 65536u)
	{
		Frame linedIntegral(FrameType(width + 1u, height + 1u, FrameType::genericPixelFormat<uint32_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT));
		Frame linedIntegralSquared(FrameType(width + 1u, height + 1u, FrameType::genericPixelFormat<uint64_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT));

		CV::IntegralImage::createLinedImageAndSquared<uint8_t, uint32_t, uint64_t, 1u>(yFrame, linedIntegral.data<uint32_t>(), linedIntegralSquared.data<uint64_t>(), width, height, framePaddingElements, linedIntegral.paddingElements(), linedIntegralSquared.paddingElements());

		determineHorizontalResponsesI<tUseSIMD>(linedIntegral.constdata<uint32_t>(), linedIntegralSquared.constdata<uint64_t>(), width, height, horizontalResponses.data<int32_t>(), linedIntegral.paddingElements(), linedIntegralSquared.paddingElements(), horizontalResponses.paddingElements());
		determineVerticalResponsesI<tUseSIMD>(linedIntegral.constdata<uint32_t>(), linedIntegralSquared.constdata<uint64_t>(), width, height, verticalResponses.data<int32_t>(), linedIntegral.paddingElements(), linedIntegralSquared.paddingElements(), verticalResponses.paddingElements());
	}
	else
	{
		Frame linedIntegralAndSquared(FrameType(width + 1u, height + 1u, FrameType::genericPixelFormat<uint32_t, 2u>(), FrameType::ORIGIN_UPPER_LEFT));
		CV::IntegralImage::createLinedImageAndSquared<uint8_t, uint32_t, 1u>(yFrame, linedIntegralAndSquared.data<uint32_t>(), width, height, framePaddingElements, linedIntegralAndSquared.paddingElements());

		determineHorizontalResponsesI<tUseSIMD>(linedIntegralAndSquared.constdata<uint32_t>(), width, height, horizontalResponses.data<int32_t>(), linedIntegralAndSquared.paddingElements(), horizontalResponses.paddingElements());
		determineVerticalResponsesI<tUseSIMD>(linedIntegralAndSquared.constdata<uint32_t>(), width, height, verticalResponses.data<int32_t>(), linedIntegralAndSquared.paddingElements(), verticalResponses.paddingElements());
	}

	const float adjustedMinimalThreshold = (minimalThreshold * 9.5f / 6.0f); // 9.5 when using squared responses seems to be similar to 6.0 when using non-squared responses
	const uint32_t sqrAdjustedMinimalThreshold = uint32_t(adjustedMinimalThreshold * adjustedMinimalThreshold + 0.5f);

	CV::NonMaximumSuppression<uint32_t> nonMaximumSuppressionTopDown(width, height);
	CV::NonMaximumSuppression<uint32_t> nonMaximumSuppressionBottomUp(width, height);

	if (topDownResponseFrame && bottomUpResponseFrame)
	{
		ocean_assert(horizontalResponses.isContinuous());

		determineResponsesI<true>(horizontalResponses.constdata<int32_t>(), verticalResponses.constdata<int32_t>(), width, height, sign, sqrAdjustedMinimalThreshold, nonMaximumSuppressionTopDown, nonMaximumSuppressionBottomUp, topDownResponseFrame, bottomUpResponseFrame);
	}
	else
	{
		ocean_assert(horizontalResponses.isContinuous());

		determineResponsesI<false>(horizontalResponses.constdata<int32_t>(), verticalResponses.constdata<int32_t>(), width, height, sign, sqrAdjustedMinimalThreshold, nonMaximumSuppressionTopDown, nonMaximumSuppressionBottomUp);
	}

	constexpr unsigned int nonMaximumBorderTopDownLeft = frameX_T_topDownResponseX() + 1u;
	constexpr unsigned int nonMaximumBorderTopDownRight = shapeWidth_2_ + 1u;
	constexpr unsigned int nonMaximumBorderTopDownTop = frameY_T_topDownResponseY() + 1u;
	constexpr unsigned int nonMaximumBorderTopDownBottom = shapeHeight_ - shapeStepSize_2_ + 1u;

	CV::NonMaximumSuppression<uint32_t>::StrengthPositions<unsigned int, uint32_t> shapes = nonMaximumSuppressionTopDown.suppressNonMaximum<unsigned int, uint32_t, false /*tStrictMaximum*/>(nonMaximumBorderTopDownLeft, width - nonMaximumBorderTopDownLeft - nonMaximumBorderTopDownRight, nonMaximumBorderTopDownTop, height - nonMaximumBorderTopDownTop - nonMaximumBorderTopDownBottom, nullptr, nullptr);
	shapes = CV::NonMaximumSuppression<uint32_t>::suppressNonMaximum<unsigned int, uint32_t, true>(width, height, shapes, nonMaximumSupressionRadius);

	tShapes.reserve(tShapes.size() + shapes.size());
	for (size_t n = 0; n < shapes.size(); ++n)
	{
		const unsigned int& x = shapes[n].x();
		const unsigned int& y = shapes[n].y();

		ocean_assert(x >= 1u && x <= width - 2u && y >= 1u && y <= height - 2u);

		const int32_t* const horizontalResponsesTopLeft = horizontalResponses.constpixel<int32_t>((unsigned int)(int(x) - frameX_T_topDownHorizontalResponseX() - 1), (unsigned int)(int(y) - frameY_T_topDownHorizontalResponseY() - 1));
		const int32_t* const verticalResponsesTopLeft = verticalResponses.constpixel<int32_t>((unsigned int)(int(x) - frameX_T_topDownVerticalResponseX() - 1), (unsigned int)(int(y) - frameY_T_topDownVerticalResponseY() - 1));

		const unsigned int horizontalResponsesStrideElements = horizontalResponses.strideElements();
		const unsigned int verticalResponsesStrideElements = verticalResponses.strideElements();

		// the integer-based responses are actually squared responses, so that we have to apply the square root

		const Scalar topResponses[3] =
		{
			Numeric::sqrt(Numeric::abs(Scalar(horizontalResponsesTopLeft[0] * verticalResponsesTopLeft[0]))),
			Numeric::sqrt(Numeric::abs(Scalar(horizontalResponsesTopLeft[1] * verticalResponsesTopLeft[1]))),
			Numeric::sqrt(Numeric::abs(Scalar(horizontalResponsesTopLeft[2] * verticalResponsesTopLeft[2])))
		};

		const Scalar centerResponses[3] =
		{
			Numeric::sqrt(Numeric::abs(Scalar(horizontalResponsesTopLeft[horizontalResponsesStrideElements + 0u] * verticalResponsesTopLeft[verticalResponsesStrideElements + 0u]))),
			Numeric::sqrt(Numeric::abs(Scalar(horizontalResponsesTopLeft[horizontalResponsesStrideElements + 1u] * verticalResponsesTopLeft[verticalResponsesStrideElements + 1u]))),
			Numeric::sqrt(Numeric::abs(Scalar(horizontalResponsesTopLeft[horizontalResponsesStrideElements + 2u] * verticalResponsesTopLeft[verticalResponsesStrideElements + 2u])))
		};

		const Scalar bottomResponses[3] =
		{
			Numeric::sqrt(Numeric::abs(Scalar(horizontalResponsesTopLeft[horizontalResponsesStrideElements * 2u + 0u] * verticalResponsesTopLeft[verticalResponsesStrideElements * 2u + 0u]))),
			Numeric::sqrt(Numeric::abs(Scalar(horizontalResponsesTopLeft[horizontalResponsesStrideElements * 2u + 1u] * verticalResponsesTopLeft[verticalResponsesStrideElements * 2u + 1u]))),
			Numeric::sqrt(Numeric::abs(Scalar(horizontalResponsesTopLeft[horizontalResponsesStrideElements * 2u + 2u] * verticalResponsesTopLeft[verticalResponsesStrideElements * 2u + 2u])))
		};

		ocean_assert(Numeric::isEqual(centerResponses[1], Numeric::sqrt(Scalar(shapes[n].strength()))));

		Vector2 offset(0, 0);
		CV::NonMaximumSuppression<Scalar>::determinePrecisePeakLocation2<Scalar>(topResponses, centerResponses, bottomResponses, offset);

		tShapes.emplace_back(Vector2(Scalar(x), Scalar(y)) + offset, Vector2(0, 1), Scalar(shapes[n].strength()));
	}

	constexpr unsigned int nonMaximumBorderBottomUpLeft = frameX_T_bottomUpResponseX() + 1u;
	constexpr unsigned int nonMaximumBorderBottomUpRight = shapeWidth_2_ + 1u;
	constexpr unsigned int nonMaximumBorderBottomUpTop = frameY_T_bottomUpResponseY() + 1u;
	constexpr unsigned int nonMaximumBorderBottomUpBottom = shapeBandSize_ + shapeStepSize_2_ + 1u;

	shapes = nonMaximumSuppressionBottomUp.suppressNonMaximum<unsigned int, uint32_t, false /*tStrictMaximum*/>(nonMaximumBorderBottomUpLeft, width - nonMaximumBorderBottomUpLeft - nonMaximumBorderBottomUpRight, nonMaximumBorderBottomUpTop, height - nonMaximumBorderBottomUpTop - nonMaximumBorderBottomUpBottom, nullptr, nullptr);
	shapes = CV::NonMaximumSuppression<uint32_t>::suppressNonMaximum<unsigned int, uint32_t, true>(width, height, shapes, nonMaximumSupressionRadius);

	tShapes.reserve(tShapes.size() + shapes.size());
	for (size_t n = 0; n < shapes.size(); ++n)
	{
		// determining the sub-pixel location of the shape

		const unsigned int& x = shapes[n].x();
		const unsigned int& y = shapes[n].y();

		ocean_assert(x >= 1u && x <= width - 2u && y >= 1u && y <= height - 2u);

		const int32_t* const horizontalResponsesTopLeft = horizontalResponses.constpixel<int32_t>((unsigned int)(int(x) - frameX_T_bottomUpHorizontalResponseX() - 1), (unsigned int)(int(y) - frameY_T_bottomUpHorizontalResponseY() - 1));
		const int32_t* const verticalResponsesTopLeft = verticalResponses.constpixel<int32_t>((unsigned int)(int(x) - frameX_T_bottomUpVerticalResponseX() - 1), (unsigned int)(int(y) - frameY_T_bottomUpVerticalResponseY() - 1));

		const unsigned int horizontalResponsesStrideElements = horizontalResponses.strideElements();
		const unsigned int verticalResponsesStrideElements = verticalResponses.strideElements();

		// the integer-based responses are actually squared responses, so that we have to apply the square root

		const Scalar topResponses[3] =
		{
			Numeric::sqrt(Numeric::abs(Scalar(horizontalResponsesTopLeft[0] * verticalResponsesTopLeft[0]))),
			Numeric::sqrt(Numeric::abs(Scalar(horizontalResponsesTopLeft[1] * verticalResponsesTopLeft[1]))),
			Numeric::sqrt(Numeric::abs(Scalar(horizontalResponsesTopLeft[2] * verticalResponsesTopLeft[2])))
		};

		const Scalar centerResponses[3] =
		{
			Numeric::sqrt(Numeric::abs(Scalar(horizontalResponsesTopLeft[horizontalResponsesStrideElements + 0u] * verticalResponsesTopLeft[verticalResponsesStrideElements + 0u]))),
			Numeric::sqrt(Numeric::abs(Scalar(horizontalResponsesTopLeft[horizontalResponsesStrideElements + 1u] * verticalResponsesTopLeft[verticalResponsesStrideElements + 1u]))),
			Numeric::sqrt(Numeric::abs(Scalar(horizontalResponsesTopLeft[horizontalResponsesStrideElements + 2u] * verticalResponsesTopLeft[verticalResponsesStrideElements + 2u])))
		};

		const Scalar bottomResponses[3] =
		{
			Numeric::sqrt(Numeric::abs(Scalar(horizontalResponsesTopLeft[horizontalResponsesStrideElements * 2u + 0u] * verticalResponsesTopLeft[verticalResponsesStrideElements * 2u + 0u]))),
			Numeric::sqrt(Numeric::abs(Scalar(horizontalResponsesTopLeft[horizontalResponsesStrideElements * 2u + 1u] * verticalResponsesTopLeft[verticalResponsesStrideElements * 2u + 1u]))),
			Numeric::sqrt(Numeric::abs(Scalar(horizontalResponsesTopLeft[horizontalResponsesStrideElements * 2u + 2u] * verticalResponsesTopLeft[verticalResponsesStrideElements * 2u + 2u])))
		};

		ocean_assert(Numeric::isEqual(centerResponses[1], Numeric::sqrt(Scalar(shapes[n].strength()))));

		Vector2 offset(0, 0);
		CV::NonMaximumSuppression<Scalar>::determinePrecisePeakLocation2<Scalar>(topResponses, centerResponses, bottomResponses, offset);

		tShapes.emplace_back(Vector2(Scalar(x), Scalar(y)) + offset, Vector2(0, -1), Scalar(shapes[n].strength()));
	}

	CV::Detector::ShapeDetector::postAdjustShapes(width, height, lShapes, tShapes, xShapes);
}

template <bool tUseSIMD>
void ShapeDetector::PatternDetectorGradientVarianceBased::determineHorizontalResponsesI(const uint32_t* linedIntegralAndSquared, const unsigned int width, const unsigned int height, int32_t* horizontalResponses, const unsigned int linedIntegralAndSquaredPaddingElements, const unsigned int horizontalResponsesPaddingElements)
{
	static_assert(shapeWidth_ >= 1u && shapeWidth_ % 2u == 1u, "Invalid shape width!");
	static_assert(shapeHeight_ >= shapeStepSize_ + 1u, "Invalid shape height!");
	static_assert(shapeStepSize_ >= 1u && shapeStepSize_ % 2u == 1u, "Invalid shape step size!");
	static_assert(shapeBandSize_ >= 1u, "Invalid shape band size!");

	constexpr unsigned int precision = 10u;
	constexpr unsigned int factorPrecision = 1u << precision;

	constexpr unsigned int responseAreaWidth = shapeWidth_;
	constexpr unsigned int responseAreaHeight = shapeBandSize_ * 2u + shapeStepSize_;

	// the width of the left and right band box
	constexpr unsigned int shapeBandBoxWidth = shapeWidth_2_ - shapeStepSize_2_;
	constexpr unsigned int shapeBandBoxOffset = shapeWidth_ - shapeWidth_2_ + shapeStepSize_2_;

	constexpr unsigned int shapeBandBoxWidth2 = shapeBandBoxWidth * 2u;
	constexpr unsigned int shapeBandBoxOffset2 = shapeBandBoxOffset * 2u;

	constexpr unsigned int twoBandBoxesSize = shapeBandBoxWidth * shapeBandSize_ * 2u;

	constexpr unsigned int fourBandBoxesSize = twoBandBoxesSize * 2u;

	constexpr unsigned int centerSize = shapeWidth_;

	// as factorPrecision / (twoBandBoxesSize * centerSize) is ~1.4, we need to apply a multiplication + shift (division with power of two) to get a more precise response
	constexpr int32_t invTwoBandBoxesCenterSize_normalization = 64;
	constexpr int32_t invTwoBandBoxesCenterSize_precision = int((factorPrecision * invTwoBandBoxesCenterSize_normalization + (twoBandBoxesSize * centerSize) / 2u) / (twoBandBoxesSize * centerSize));

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
	const uint32x4_t centerSize_u_32x4 = vdupq_n_u32(centerSize);
	const uint32x4_t twoBandBoxesSize_u_32x4 = vdupq_n_u32(twoBandBoxesSize);
	const uint32x4_t fourBandBoxesSize_u_32x4 = vdupq_n_u32(fourBandBoxesSize);

	const int32x4_t invTwoBandBoxesCenterSize_precision_s_32x4 = vdupq_n_s32(invTwoBandBoxesCenterSize_precision);

	const uint32x4_t constant_1_u_32x4 = vdupq_n_u32(1u);
	const uint32x4_t constant_7_u_32x4 = vdupq_n_u32(7u);
	const uint32x4_t constant_287_u_32x4 = vdupq_n_u32(287u);

	const uint32x4_t minimalGradientResponse_precision_u_32x4 = vdupq_n_u32(factorPrecision / 2u);
#endif

#ifdef OCEAN_INTENSIVE_DEBUG
	constexpr float debugInvTwoBandBoxesSize = 1.0f / float(twoBandBoxesSize);

	constexpr float debugInvCenterSize = 1.0f / float(centerSize);
	constexpr float debugInvCenterSizeSqr = 1.0f / float(centerSize * centerSize);

	constexpr float debugInvFourBandBoxesSizeSqr = 1.0f / float(fourBandBoxesSize * fourBandBoxesSize);
#endif

	ocean_assert(linedIntegralAndSquared != nullptr);
	ocean_assert(width >= shapeWidth_ && width >= 20u);
	ocean_assert(height >= shapeHeight_ && height >= 20u);
	ocean_assert(width * height <= 65536u);
	ocean_assert(horizontalResponses != nullptr);

#ifdef OCEAN_DEBUG
	const int32_t* const debugHorizontalResponses = horizontalResponses;
#endif

	const unsigned int coreResponseWidth = width - responseAreaWidth + 1u;
	const unsigned int coreResponseHeight = height - responseAreaHeight + 1u;

	ocean_assert(coreResponseWidth >= 4u);

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
	constexpr unsigned int xResponseSteps = tUseSIMD ? 4u : 1u;
#else
	constexpr unsigned int xResponseSteps = 1u;
#endif

	const unsigned int linedIntegralAndSquaredStrideElements = (width + 1u) * 2u + linedIntegralAndSquaredPaddingElements;
	const unsigned int horizontalResponsesStrideElements = coreResponseWidth + horizontalResponsesPaddingElements;

	const uint32_t* topBand_top = linedIntegralAndSquared;
	const uint32_t* topBand_bottom = linedIntegralAndSquared + linedIntegralAndSquaredStrideElements * shapeBandSize_;

	const uint32_t* centerBand_top = linedIntegralAndSquared + linedIntegralAndSquaredStrideElements * (shapeBandSize_ + shapeStepSize_2_);
	const uint32_t* centerBand_bottom = linedIntegralAndSquared + linedIntegralAndSquaredStrideElements * (shapeBandSize_ + shapeStepSize_2_ + 1u);

	const uint32_t* bottomBand_top = linedIntegralAndSquared + linedIntegralAndSquaredStrideElements * (shapeBandSize_ + shapeStepSize_);
	const uint32_t* bottomBand_bottom = linedIntegralAndSquared + linedIntegralAndSquaredStrideElements * (shapeBandSize_ * 2u + shapeStepSize_);

	for (unsigned int yResponse = 0u; yResponse < coreResponseHeight; ++yResponse)
	{
		ocean_assert_and_suppress_unused((horizontalResponses - debugHorizontalResponses) % horizontalResponsesStrideElements == 0u, horizontalResponsesStrideElements);
		ocean_assert((topBand_top - linedIntegralAndSquared) % linedIntegralAndSquaredStrideElements == 0u);

		for (unsigned int xResponse = 0u; xResponse < coreResponseWidth; xResponse += xResponseSteps)
		{
#ifdef OCEAN_DEBUG
			// The response offset for the top-down T-shape is (shapeWidth / 2)x(shapeBandSize + shapeStepSize / 2)

			// x and y location to which the response belongs (for top-down filter)
			const unsigned int debugFrameTopDownX = xResponse + shapeWidth_2_;
			const unsigned int debugFrameTopDownY = yResponse + shapeBandSize_ + shapeStepSize_2_;

			ocean_assert(debugFrameTopDownX < width);
			ocean_assert(debugFrameTopDownY < height);
#endif

			if constexpr (xResponseSteps != 1u) // constexpr evaluated at compile time
			{
				if (xResponse + xResponseSteps > coreResponseWidth)
				{
					// the last iteration will not fit
					// so we simply shift x left by some pixels (at most 3) and we will calculate some pixels again

					ocean_assert(xResponse >= xResponseSteps && coreResponseWidth > xResponseSteps);
					const unsigned int newXResponse = coreResponseWidth - xResponseSteps;

					ocean_assert(xResponse > newXResponse);
					const unsigned int offset = xResponse - newXResponse;

					topBand_top -= 2u * offset;
					topBand_bottom -= 2u * offset;

					centerBand_top -= 2u * offset;
					centerBand_bottom -= 2u * offset;

					bottomBand_top -= 2u * offset;
					bottomBand_bottom -= 2u * offset;

					horizontalResponses -= offset;

					xResponse = newXResponse;

					// the for loop will stop after this iteration
					ocean_assert(!(xResponse + xResponseSteps < coreResponseWidth));
				}

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

				// top left band box
				const uint32x4x2_t topLeftSumAndSquareSumA_u_32x4x2 = vld2q_u32(topBand_top);
				const uint32x4x2_t topLeftSumAndSquareSumB_u_32x4x2 = vld2q_u32(topBand_top + shapeBandBoxWidth2);
				const uint32x4x2_t topLeftSumAndSquareSumC_u_32x4x2 = vld2q_u32(topBand_bottom);
				const uint32x4x2_t topLeftSumAndSquareSumD_u_32x4x2 = vld2q_u32(topBand_bottom + shapeBandBoxWidth2);

				const uint32x4_t topLeftSum_u_32x4 = vsubq_u32(vaddq_u32(topLeftSumAndSquareSumA_u_32x4x2.val[0], topLeftSumAndSquareSumD_u_32x4x2.val[0]), vaddq_u32(topLeftSumAndSquareSumB_u_32x4x2.val[0], topLeftSumAndSquareSumC_u_32x4x2.val[0])); // (A + D) - (B + C)
				const uint32x4_t topLeftSquareSum_u_32x4 = vsubq_u32(vaddq_u32(topLeftSumAndSquareSumA_u_32x4x2.val[1], topLeftSumAndSquareSumD_u_32x4x2.val[1]), vaddq_u32(topLeftSumAndSquareSumB_u_32x4x2.val[1], topLeftSumAndSquareSumC_u_32x4x2.val[1]));

				// top right band box
				const uint32x4x2_t topRightSumAndSquareSumA_u_32x4x2 = vld2q_u32(topBand_top + shapeBandBoxOffset2);
				const uint32x4x2_t topRightSumAndSquareSumB_u_32x4x2 = vld2q_u32(topBand_top + shapeBandBoxOffset2 + shapeBandBoxWidth2);
				const uint32x4x2_t topRightSumAndSquareSumC_u_32x4x2 = vld2q_u32(topBand_bottom + shapeBandBoxOffset2);
				const uint32x4x2_t topRightSumAndSquareSumD_u_32x4x2 = vld2q_u32(topBand_bottom + shapeBandBoxOffset2 + shapeBandBoxWidth2);

				const uint32x4_t topRightSum_u_32x4 = vsubq_u32(vaddq_u32(topRightSumAndSquareSumA_u_32x4x2.val[0], topRightSumAndSquareSumD_u_32x4x2.val[0]), vaddq_u32(topRightSumAndSquareSumB_u_32x4x2.val[0], topRightSumAndSquareSumC_u_32x4x2.val[0]));
				const uint32x4_t topRightSquareSum_u_32x4 = vsubq_u32(vaddq_u32(topRightSumAndSquareSumA_u_32x4x2.val[1], topRightSumAndSquareSumD_u_32x4x2.val[1]), vaddq_u32(topRightSumAndSquareSumB_u_32x4x2.val[1], topRightSumAndSquareSumC_u_32x4x2.val[1]));

				// top mean
				const uint32x4_t topSum_adjusted_u_32x4 = vmulq_u32(vaddq_u32(topLeftSum_u_32x4, topRightSum_u_32x4), centerSize_u_32x4); // topMean * twoBandBoxesSize * centerSize


				// center mean
				const uint32x4x2_t centerSumAndSquareSumA_u_32x4x2 = vld2q_u32(centerBand_top);
				const uint32x4x2_t centerSumAndSquareSumB_u_32x4x2 = vld2q_u32(centerBand_top + shapeWidth_ * 2u);
				const uint32x4x2_t centerSumAndSquareSumC_u_32x4x2 = vld2q_u32(centerBand_bottom);
				const uint32x4x2_t centerSumAndSquareSumD_u_32x4x2 = vld2q_u32(centerBand_bottom + shapeWidth_ * 2u);

				const uint32x4_t centerSum_u_32x4 = vsubq_u32(vaddq_u32(centerSumAndSquareSumA_u_32x4x2.val[0], centerSumAndSquareSumD_u_32x4x2.val[0]), vaddq_u32(centerSumAndSquareSumB_u_32x4x2.val[0], centerSumAndSquareSumC_u_32x4x2.val[0]));
				const uint32x4_t centerSquareSum_u_32x4 = vsubq_u32(vaddq_u32(centerSumAndSquareSumA_u_32x4x2.val[1], centerSumAndSquareSumD_u_32x4x2.val[1]), vaddq_u32(centerSumAndSquareSumB_u_32x4x2.val[1], centerSumAndSquareSumC_u_32x4x2.val[1]));

				const uint32x4_t centerSum_adjusted_u_32x4 = vmulq_u32(centerSum_u_32x4, twoBandBoxesSize_u_32x4); // centerMean * centerSize * twoBandBoxesSize


				// bottom left band box
				const uint32x4x2_t bottomLeftSumAndSquareSumA_u_32x4x2 = vld2q_u32(bottomBand_top);
				const uint32x4x2_t bottomLeftSumAndSquareSumB_u_32x4x2 = vld2q_u32(bottomBand_top + shapeBandBoxWidth2);
				const uint32x4x2_t bottomLeftSumAndSquareSumC_u_32x4x2 = vld2q_u32(bottomBand_bottom);
				const uint32x4x2_t bottomLeftSumAndSquareSumD_u_32x4x2 = vld2q_u32(bottomBand_bottom + shapeBandBoxWidth2);

				const uint32x4_t bottomLeftSum_u_32x4 = vsubq_u32(vaddq_u32(bottomLeftSumAndSquareSumA_u_32x4x2.val[0], bottomLeftSumAndSquareSumD_u_32x4x2.val[0]), vaddq_u32(bottomLeftSumAndSquareSumB_u_32x4x2.val[0], bottomLeftSumAndSquareSumC_u_32x4x2.val[0]));
				const uint32x4_t bottomLeftSquareSum_u_32x4 = vsubq_u32(vaddq_u32(bottomLeftSumAndSquareSumA_u_32x4x2.val[1], bottomLeftSumAndSquareSumD_u_32x4x2.val[1]), vaddq_u32(bottomLeftSumAndSquareSumB_u_32x4x2.val[1], bottomLeftSumAndSquareSumC_u_32x4x2.val[1]));

				// bottom right band box
				const uint32x4x2_t bottomRightSumAndSquareSumA_u_32x4x2 = vld2q_u32(bottomBand_top + shapeBandBoxOffset2);
				const uint32x4x2_t bottomRightSumAndSquareSumB_u_32x4x2 = vld2q_u32(bottomBand_top + shapeBandBoxOffset2 + shapeBandBoxWidth2);
				const uint32x4x2_t bottomRightSumAndSquareSumC_u_32x4x2 = vld2q_u32(bottomBand_bottom + shapeBandBoxOffset2);
				const uint32x4x2_t bottomRightSumAndSquareSumD_u_32x4x2 = vld2q_u32(bottomBand_bottom + shapeBandBoxOffset2 + shapeBandBoxWidth2);

				const uint32x4_t bottomRightSum_u_32x4 = vsubq_u32(vaddq_u32(bottomRightSumAndSquareSumA_u_32x4x2.val[0], bottomRightSumAndSquareSumD_u_32x4x2.val[0]), vaddq_u32(bottomRightSumAndSquareSumB_u_32x4x2.val[0], bottomRightSumAndSquareSumC_u_32x4x2.val[0]));
				const uint32x4_t bottomRightSquareSum_u_32x4 = vsubq_u32(vaddq_u32(bottomRightSumAndSquareSumA_u_32x4x2.val[1], bottomRightSumAndSquareSumD_u_32x4x2.val[1]), vaddq_u32(bottomRightSumAndSquareSumB_u_32x4x2.val[1], bottomRightSumAndSquareSumC_u_32x4x2.val[1]));

				// bottom mean
				const uint32x4_t bottomSum_adjusted_u_32x4 = vmulq_u32(vaddq_u32(bottomLeftSum_u_32x4, bottomRightSum_u_32x4), centerSize_u_32x4); // bottomMean * twoBandBoxesSize * centerSize


				// gradient

				const int32x4_t topGradient_precision_s_32x4 = vrshrq_n_s32(vmulq_s32(vreinterpretq_s32_u32(vsubq_u32(centerSum_adjusted_u_32x4, topSum_adjusted_u_32x4)), invTwoBandBoxesCenterSize_precision_s_32x4), 6); // (centerSum - topSum)) * invTwoBandBoxesCenterSize_precision_s_32x4 / invTwoBandBoxesCenterSize_normalization_s_32x4
				const int32x4_t bottomGradient_precision_s_32x4 = vrshrq_n_s32(vmulq_s32(vreinterpretq_s32_u32(vsubq_u32(bottomSum_adjusted_u_32x4, centerSum_adjusted_u_32x4)), invTwoBandBoxesCenterSize_precision_s_32x4), 6); // (bottomSum - centerSum)) * invTwoBandBoxesCenterSize_precision_s_32x4 / invTwoBandBoxesCenterSize_normalization_s_32x4

				const int32x4_t horizontalResponse_precision_s_32x4 = vsubq_s32(topGradient_precision_s_32x4, bottomGradient_precision_s_32x4);

				const uint32x4_t absTopGradient_precision_u_32x4 = vreinterpretq_u32_s32(vabsq_s32(topGradient_precision_s_32x4));
				const uint32x4_t absBottomGradient_precision_u_32x4 = vreinterpretq_u32_s32(vabsq_s32(bottomGradient_precision_s_32x4));


				// abs(topGradient) >= minimalGradient && abs(bottomGradient) >= minimalGradient
				const uint32x4_t validGradient_u_32x4 = vandq_u32(vcgeq_u32(absTopGradient_precision_u_32x4, minimalGradientResponse_precision_u_32x4), vcgeq_u32(absBottomGradient_precision_u_32x4, minimalGradientResponse_precision_u_32x4));

#ifdef OCEAN_INTENSIVE_DEBUG
				const uint32_t debugTopLeftSum = topBand_top[0] - topBand_top[shapeBandBoxWidth2 + 0u] - topBand_bottom[0] + topBand_bottom[shapeBandBoxWidth2 + 0u];
				const uint32_t debugTopLeftSquareSum = topBand_top[1] - topBand_top[shapeBandBoxWidth2 + 1u] - topBand_bottom[1] + topBand_bottom[shapeBandBoxWidth2 + 1u];
				const uint32_t debugTopRightSum = topBand_top[shapeBandBoxOffset2 + 0u] - topBand_top[shapeBandBoxOffset2 + shapeBandBoxWidth2 + 0u] - topBand_bottom[shapeBandBoxOffset2 + 0u] + topBand_bottom[shapeBandBoxOffset2 + shapeBandBoxWidth2 + 0u];
				const uint32_t debugTopRightSquareSum = topBand_top[shapeBandBoxOffset2 + 1u] - topBand_top[shapeBandBoxOffset2 + shapeBandBoxWidth2 + 1u] - topBand_bottom[shapeBandBoxOffset2 + 1u] + topBand_bottom[shapeBandBoxOffset2 + shapeBandBoxWidth2 + 1u];
				const uint32_t debugTopSum_adjusted = (debugTopLeftSum + debugTopRightSum) * centerSize;

				const uint32_t debugCenterSum = centerBand_top[0] - centerBand_top[shapeWidth_ * 2u + 0u] - centerBand_bottom[0] + centerBand_bottom[shapeWidth_ * 2u + 0u];
				const uint32_t debugCenterSquareSum = centerBand_top[1] - centerBand_top[shapeWidth_ * 2u + 1u] - centerBand_bottom[1] + centerBand_bottom[shapeWidth_ * 2u + 1u];
				const uint32_t debugCenterSum_adjusted = debugCenterSum * twoBandBoxesSize;

				const uint32_t debugBottomLeftSum = bottomBand_top[0] - bottomBand_top[shapeBandBoxWidth2 + 0u] - bottomBand_bottom[0] + bottomBand_bottom[shapeBandBoxWidth2 + 0u];
				const uint32_t debugBottomLeftSquareSum = bottomBand_top[1] - bottomBand_top[shapeBandBoxWidth2 + 1u] - bottomBand_bottom[1] + bottomBand_bottom[shapeBandBoxWidth2 + 1u];
				const uint32_t debugBottomRightSum = bottomBand_top[shapeBandBoxOffset2 + 0u] - bottomBand_top[shapeBandBoxOffset2 + shapeBandBoxWidth2 + 0u] - bottomBand_bottom[shapeBandBoxOffset2 + 0u] + bottomBand_bottom[shapeBandBoxOffset2 + shapeBandBoxWidth2 + 0u];
				const uint32_t debugBottomRightSquareSum = bottomBand_top[shapeBandBoxOffset2 + 1u] - bottomBand_top[shapeBandBoxOffset2 + shapeBandBoxWidth2 + 1u] - bottomBand_bottom[shapeBandBoxOffset2 + 1u] + bottomBand_bottom[shapeBandBoxOffset2 + shapeBandBoxWidth2 + 1u];
				const uint32_t debugBottomSum_adjusted = (debugBottomLeftSum + debugBottomRightSum) * centerSize;

				const int32_t debugTopGradient_precision = (int32_t(debugCenterSum_adjusted - debugTopSum_adjusted) * invTwoBandBoxesCenterSize_precision) / invTwoBandBoxesCenterSize_normalization;
				const int32_t debugBottomGradient_precision = (int32_t(debugBottomSum_adjusted - debugCenterSum_adjusted) * invTwoBandBoxesCenterSize_precision) / invTwoBandBoxesCenterSize_normalization;
				const int32_t debugHorizontalResponse_precision = debugTopGradient_precision - debugBottomGradient_precision;

				ocean_assert(NumericT<uint32_t>::isEqual(vgetq_lane_u32(horizontalResponse_precision_s_32x4, 0), debugHorizontalResponse_precision, 16u));

				constexpr uint32_t debugMinimalGradientResponse_precision = (factorPrecision + 1u) / 2u;
				const bool debugFirstResponseValid = NumericT<int32_t>::secureAbs(debugTopGradient_precision) >= debugMinimalGradientResponse_precision && NumericT<int32_t>::secureAbs(debugBottomGradient_precision) >= debugMinimalGradientResponse_precision;
				OCEAN_SUPPRESS_UNUSED_WARNING(debugFirstResponseValid);
#endif // OCEAN_INTENSIVE_DEBUG

				const uint32x4_t absHorizontalResponse_precision_8_u_32x4 = vrshrq_n_u32(vreinterpretq_u32_s32(vabsq_s32(horizontalResponse_precision_s_32x4)), 3); // (|horizontalRespones| + 4) / 8
				const uint32x4_t sqrHorizontalResponse_16384_u_32x4 = vmulq_u32(absHorizontalResponse_precision_8_u_32x4, absHorizontalResponse_precision_8_u_32x4); // gradient^2 * 16,384 < 2^32

				const uint32x4_t centerVarianceSmallSizeSqr_u_32x4 = vmlsq_u32(vmulq_u32(centerSquareSum_u_32x4, centerSize_u_32x4), centerSum_u_32x4, centerSum_u_32x4); // centerVariance * 225

				const uint32x4_t topBottomSum_u_32x4 = vaddq_u32(vaddq_u32(topLeftSum_u_32x4, topRightSum_u_32x4), vaddq_u32(bottomLeftSum_u_32x4, bottomRightSum_u_32x4));
				const uint32x4_t topBottomSquareSum_u_32x4 = vaddq_u32(vaddq_u32(topLeftSquareSum_u_32x4, topRightSquareSum_u_32x4), vaddq_u32(bottomLeftSquareSum_u_32x4, bottomRightSquareSum_u_32x4));

				const uint32x4_t topBottomVarianceLargeSizeSqr_u_32x4 = vmlsq_u32(vmulq_u32(topBottomSquareSum_u_32x4, fourBandBoxesSize_u_32x4), topBottomSum_u_32x4, topBottomSum_u_32x4); // topBottomVariance * 9,216

				const uint32x4_t topBottomAdjustedVariance_u_32x4 = vrshrq_n_u32(vmulq_u32(topBottomVarianceLargeSizeSqr_u_32x4, constant_7_u_32x4), 2); // (topBottomVarianceLargeSizeSqr * 7 + 2) / 4
				const uint32x4_t centerAdjustedVariance_u_32x4 = vrshrq_n_u32(vmulq_u32(centerVarianceSmallSizeSqr_u_32x4, constant_287_u_32x4), 2); // (centerVarianceSmallSizeSqr * 287 + 2) / 4

				const uint32x4_t horizontalVariance_16384_u_32x4 = vrhaddq_u32(topBottomAdjustedVariance_u_32x4, centerAdjustedVariance_u_32x4); // (topBottomAdjustedVariance + centerAdjustedVariance + 1) / 2

#ifdef OCEAN_INTENSIVE_DEBUG
				const uint32_t debugAbsHorizontalResponse_precision_8 = (NumericT<int32_t>::secureAbs(debugHorizontalResponse_precision) + 4u) / 8u;
				const uint32_t debugSqrHorizontalResponse_16384 = debugAbsHorizontalResponse_precision_8 * debugAbsHorizontalResponse_precision_8;
				const uint32_t debugCenterVarianceSmallSizeSqr = debugCenterSquareSum * centerSize - debugCenterSum * debugCenterSum;
				const uint32_t debugTopBottomSum = debugTopLeftSum + debugTopRightSum + debugBottomLeftSum + debugBottomRightSum;
				const uint32_t debugTopBottomSquareSum = debugTopLeftSquareSum + debugTopRightSquareSum + debugBottomLeftSquareSum + debugBottomRightSquareSum;
				const uint32_t debugTopBottomVarianceLargeSizeSqr = debugTopBottomSquareSum * fourBandBoxesSize - debugTopBottomSum * debugTopBottomSum;
				const uint32_t debugHorizontalVariance_16384 = ((((debugTopBottomVarianceLargeSizeSqr * 7u) + 2u) / 4u + ((debugCenterVarianceSmallSizeSqr * 287u) + 2u) / 4u) + 1u) / 2u;

				ocean_assert(vgetq_lane_u32(horizontalVariance_16384_u_32x4, 0) == debugHorizontalVariance_16384);
#endif // OCEAN_INTENSIVE_DEBUG

				const uint32x4_t responseDifference_u_32x4 = vqsubq_u32(sqrHorizontalResponse_16384_u_32x4, horizontalVariance_16384_u_32x4); // abs(0, sqrHorizontalResponse_16384 - horizontalVariance_16384)

				const float32x4_t horizontalVariance_16384_f_32x4 = vcvtq_f32_u32(vmaxq_u32(constant_1_u_32x4, horizontalVariance_16384_u_32x4));

				// we calculate the (approximated) inverse of horizontalVariance: 1 / horizontalVariance
				float32x4_t invHorizontalVariance_16384_f_32x4 = vrecpeq_f32(horizontalVariance_16384_f_32x4);
				invHorizontalVariance_16384_f_32x4 = vmulq_f32(vrecpsq_f32(horizontalVariance_16384_f_32x4, invHorizontalVariance_16384_f_32x4), invHorizontalVariance_16384_f_32x4); // improving the accuracy of the approx. inverse by Newton/Raphson

				const uint32x4_t horziontalResponse_u_32x4 = vandq_u32(validGradient_u_32x4, vcvtq_u32_f32(vmulq_f32(vcvtq_f32_u32(responseDifference_u_32x4), invHorizontalVariance_16384_f_32x4)));

				vst1q_s32(horizontalResponses, NEON::copySign(horziontalResponse_u_32x4, horizontalResponse_precision_s_32x4));

				topBand_top += 2u * xResponseSteps;
				topBand_bottom += 2u * xResponseSteps;

				centerBand_top += 2u * xResponseSteps;
				centerBand_bottom += 2u * xResponseSteps;

				bottomBand_top += 2u * xResponseSteps;
				bottomBand_bottom += 2u * xResponseSteps;

				horizontalResponses += xResponseSteps;

#endif // defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
			}
			else
			{
				ocean_assert(xResponseSteps == 1u);

				// the integral image is a two-channel integral image with simple sum in the first channel, and sum of squared in the second channel

				// top left band box
				const uint32_t topLeftSum = topBand_top[0] - topBand_top[shapeBandBoxWidth2 + 0u] - topBand_bottom[0] + topBand_bottom[shapeBandBoxWidth2 + 0u];
				const uint32_t topLeftSquareSum = topBand_top[1] - topBand_top[shapeBandBoxWidth2 + 1u] - topBand_bottom[1] + topBand_bottom[shapeBandBoxWidth2 + 1u];

				// top right band box
				const uint32_t topRightSum = topBand_top[shapeBandBoxOffset2 + 0u] - topBand_top[shapeBandBoxOffset2 + shapeBandBoxWidth2 + 0u] - topBand_bottom[shapeBandBoxOffset2 + 0u] + topBand_bottom[shapeBandBoxOffset2 + shapeBandBoxWidth2 + 0u];
				const uint32_t topRightSquareSum = topBand_top[shapeBandBoxOffset2 + 1u] - topBand_top[shapeBandBoxOffset2 + shapeBandBoxWidth2 + 1u] - topBand_bottom[shapeBandBoxOffset2 + 1u] + topBand_bottom[shapeBandBoxOffset2 + shapeBandBoxWidth2 + 1u];

				const uint32_t topSum_adjusted = (topLeftSum + topRightSum) * centerSize; // topMean * twoBandBoxesSize * centerSize
				ocean_assert(topSum_adjusted < 256u * twoBandBoxesSize * centerSize);

#ifdef OCEAN_INTENSIVE_DEBUG
				const float debugTopMean = float(topLeftSum + topRightSum) * debugInvTwoBandBoxesSize;
				ocean_assert(debugTopMean >= 0.0f && debugTopMean < 256.0f);
				ocean_assert(NumericF::isEqual(debugTopMean, float(topSum_adjusted) / float(twoBandBoxesSize * centerSize), 0.01f));
#endif

				// center
				const uint32_t centerSum = centerBand_top[0] - centerBand_top[shapeWidth_ * 2u + 0u] - centerBand_bottom[0] + centerBand_bottom[shapeWidth_ * 2u + 0u];
				const uint32_t centerSquareSum = centerBand_top[1] - centerBand_top[shapeWidth_ * 2u + 1u] - centerBand_bottom[1] + centerBand_bottom[shapeWidth_ * 2u + 1u];

				const uint32_t centerSum_adjusted = centerSum * twoBandBoxesSize; // centerMean * centerSize * twoBandSize
				ocean_assert(centerSum_adjusted < 256u * centerSize * twoBandBoxesSize);

#ifdef OCEAN_INTENSIVE_DEBUG
				const float debugCenterMean = float(centerSum) * debugInvCenterSize;
				ocean_assert(debugCenterMean >= 0.0f && debugCenterMean < 256.0f);
				ocean_assert(NumericF::isEqual(debugCenterMean, float(centerSum_adjusted) / float(centerSize * twoBandBoxesSize), 0.01f));
#endif

				// bottom left band box
				const uint32_t bottomLeftSum = bottomBand_top[0] - bottomBand_top[shapeBandBoxWidth2 + 0u] - bottomBand_bottom[0] + bottomBand_bottom[shapeBandBoxWidth2 + 0u];
				const uint32_t bottomLeftSquareSum = bottomBand_top[1] - bottomBand_top[shapeBandBoxWidth2 + 1u] - bottomBand_bottom[1] + bottomBand_bottom[shapeBandBoxWidth2 + 1u];

				// bottom right band box
				const uint32_t bottomRightSum = bottomBand_top[shapeBandBoxOffset2 + 0u] - bottomBand_top[shapeBandBoxOffset2 + shapeBandBoxWidth2 + 0u] - bottomBand_bottom[shapeBandBoxOffset2 + 0u] + bottomBand_bottom[shapeBandBoxOffset2 + shapeBandBoxWidth2 + 0u];
				const uint32_t bottomRightSquareSum = bottomBand_top[shapeBandBoxOffset2 + 1u] - bottomBand_top[shapeBandBoxOffset2 + shapeBandBoxWidth2 + 1u] - bottomBand_bottom[shapeBandBoxOffset2 + 1u] + bottomBand_bottom[shapeBandBoxOffset2 + shapeBandBoxWidth2 + 1u];

				const uint32_t bottomSum_adjusted = (bottomLeftSum + bottomRightSum) * centerSize; // bottomMean * twoBandBoxesSize * centerSize
				ocean_assert(bottomSum_adjusted < 256u * twoBandBoxesSize * centerSize);

#ifdef OCEAN_INTENSIVE_DEBUG
				const float debugBottomMean = float(bottomLeftSum + bottomRightSum) * debugInvTwoBandBoxesSize;
				ocean_assert(debugBottomMean >= 0.0f && debugBottomMean < 256.0f);
				ocean_assert(NumericF::isEqual(debugBottomMean, float(bottomSum_adjusted) / float(twoBandBoxesSize * centerSize), 0.01f));
#endif

				const int32_t topGradient_precision = (int32_t(centerSum_adjusted - topSum_adjusted) * invTwoBandBoxesCenterSize_precision) / invTwoBandBoxesCenterSize_normalization;
				const int32_t bottomGradient_precision = (int32_t(bottomSum_adjusted - centerSum_adjusted) * invTwoBandBoxesCenterSize_precision) / invTwoBandBoxesCenterSize_normalization;
				ocean_assert(topGradient_precision >= -int(256u * factorPrecision) && topGradient_precision < int(256u * factorPrecision));
				ocean_assert(bottomGradient_precision >= -int(256u * factorPrecision) && bottomGradient_precision < int(256u * factorPrecision));

				const int32_t horizontalResponse_precision = topGradient_precision - bottomGradient_precision;
				ocean_assert(horizontalResponse_precision >= -int(256u * factorPrecision * 2u) && horizontalResponse_precision < int(256u * factorPrecision * 2u));

#ifdef OCEAN_INTENSIVE_DEBUG
				const float debugTopGradient = debugCenterMean - debugTopMean;
				const float debugBottomGradient = debugBottomMean - debugCenterMean;
				const float debugHorizontalResponse = debugTopGradient - debugBottomGradient;
				ocean_assert(NumericF::isEqual(debugHorizontalResponse, float(horizontalResponse_precision) / float(factorPrecision), 1.0f));
#endif

				constexpr uint32_t minimalGradientResponse_precision = (factorPrecision + 1u) / 2u; // minimalGradientResponse = 0.5f

				if (NumericT<int32_t>::secureAbs(topGradient_precision) >= minimalGradientResponse_precision && NumericT<int32_t>::secureAbs(bottomGradient_precision) >= minimalGradientResponse_precision)
				{
					static_assert(uint64_t(2u * 255u * factorPrecision) * uint64_t(2u * 255u * factorPrecision) / uint64_t(8u * 8u) <= uint64_t(0xFFFFFFFFu), "Out of range!");

					const uint32_t absHorizontalResponse_precision_8 = (NumericT<int32_t>::secureAbs(horizontalResponse_precision) + 4u) / 8u; // division by 8 to stay inside 32 bit when squaring below
					ocean_assert(uint64_t(absHorizontalResponse_precision_8) * uint64_t(absHorizontalResponse_precision_8) <= uint64_t(0xFFFFFFFFu));

					const uint32_t sqrHorizontalResponse_16384 = absHorizontalResponse_precision_8 * absHorizontalResponse_precision_8; // [(gradient * 1,024) / 8] ^ 2 == gradient^2 * 16,384 < 2^32

#ifdef OCEAN_INTENSIVE_DEBUG
					const float debugHorizontalResponseInteger = NumericF::sqrt(float(sqrHorizontalResponse_16384) / 16384.0f);
					ocean_assert(NumericF::isEqual(NumericF::abs(debugHorizontalResponse), debugHorizontalResponseInteger, 1.0f));
#endif

					const uint32_t centerVarianceSmallSizeSqr = centerSquareSum * centerSize - centerSum * centerSum; // centerVariance * 15^2 = centerVariance * 225
					ocean_assert(centerVarianceSmallSizeSqr < 256u * 256u * 15u* 15u);

#ifdef OCEAN_INTENSIVE_DEBUG
					const float debugCenterVariance = float(centerSquareSum * centerSize - centerSum * centerSum) * debugInvCenterSizeSqr;
					ocean_assert(debugCenterVariance >= 0.0f && debugCenterVariance < 256.0f * 256.0f);
					ocean_assert(NumericF::isWeakEqual(debugCenterVariance, float(centerVarianceSmallSizeSqr) / float(centerSize * centerSize)));
#endif

					const uint32_t topBottomSum = topLeftSum + topRightSum + bottomLeftSum + bottomRightSum;
					const uint32_t topBottomSquareSum = topLeftSquareSum + topRightSquareSum + bottomLeftSquareSum + bottomRightSquareSum;

					const uint32_t topBottomVarianceLargeSizeSqr = topBottomSquareSum * fourBandBoxesSize - topBottomSum * topBottomSum; // topBottomVariance * 96^2 = topBottomVariance * 9,216
					ocean_assert(uint64_t(topBottomVarianceLargeSizeSqr) < uint64_t(256u * 256u) * uint64_t(fourBandBoxesSize * fourBandBoxesSize)); // <= 255^2 * 96^2 < 600,000,000

#ifdef OCEAN_INTENSIVE_DEBUG
					const float debugTopBottomVariance = float(topBottomSquareSum * fourBandBoxesSize - topBottomSum * topBottomSum) * debugInvFourBandBoxesSizeSqr;
					ocean_assert(debugTopBottomVariance >= 0.0f && debugTopBottomVariance < 256.0f * 256.0f);
					ocean_assert(NumericF::isWeakEqual(debugTopBottomVariance, float(topBottomVarianceLargeSizeSqr) / float(fourBandBoxesSize * fourBandBoxesSize)));
#endif

					// getting same (similar) nominator for gradient, centerVariance, and topBottom variance

					// centerVariance, topBottomVariance
					// 9216 / 255 = 40.96 ~ 41

					// gradient, variance
					// division should be realized with shifts, therfore 2^a and 2^c
					// 16,384 / 2^a  ==  9,216 * b / 2^c,     while b <= 7, 7.166994 = 2^32 / (255^2 * 96^2)
					// 1.7777 / 2^a  ==  b / 2^c

					// best solution: a = 0, b = 7, c = 2: 7/4 = 1.75 ~ 1.7777

					const uint32_t horizontalVariance_16384 = ((((topBottomVarianceLargeSizeSqr * 7u) + 2u) / 4u + ((centerVarianceSmallSizeSqr * 287u) + 2u) / 4u) + 1u) / 2u; // 287 = 7 * 41

#ifdef OCEAN_INTENSIVE_DEBUG
					const float debugHorizontalVariance = (debugTopBottomVariance + debugCenterVariance) * 0.5f;
					const float debugHorizontalDeviation = NumericF::sqrt(debugHorizontalVariance);
					const float debugHorizontalDeviationInteger = NumericF::sqrt(float(horizontalVariance_16384) / 16384.0f);
					ocean_assert(NumericF::isEqual(debugHorizontalDeviation, debugHorizontalDeviationInteger, 1.0f));
#endif

					const uint32_t normalizedSqrHorizontalResponse = (uint32_t(std::max(0, int32_t(sqrHorizontalResponse_16384 - horizontalVariance_16384))) + horizontalVariance_16384 / 2u) / std::max(1u, horizontalVariance_16384);

#ifdef OCEAN_INTENSIVE_DEBUG
					const float debugNormalizedSqrHorizontalResponse = std::max(0.0f, NumericF::sqr(debugHorizontalResponse) - debugHorizontalVariance) / std::max(1.0f, debugHorizontalVariance);
					const float debugNormalizedHorizontalResponse = NumericF::sqrt(debugNormalizedSqrHorizontalResponse);
					const float debugNormalizedHorizontalResponseInteger = NumericF::sqrt(float(normalizedSqrHorizontalResponse));
					ocean_assert(debugNormalizedHorizontalResponse < 15.0f || NumericF::isEqual(debugNormalizedHorizontalResponse, debugNormalizedHorizontalResponseInteger, 1.0f));
#endif

					ocean_assert(normalizedSqrHorizontalResponse < uint32_t(NumericT<int32_t>::maxValue()));
					*horizontalResponses = NumericT<int32_t>::copySign(int32_t(normalizedSqrHorizontalResponse), horizontalResponse_precision);
				}
				else
				{
					*horizontalResponses = 0;
				}

				topBand_top += 2;
				topBand_bottom += 2;

				centerBand_top += 2;
				centerBand_bottom += 2;

				bottomBand_top += 2;
				bottomBand_bottom += 2;

				++horizontalResponses;
			}
		}

		topBand_top += responseAreaWidth * 2u + linedIntegralAndSquaredPaddingElements;
		topBand_bottom += responseAreaWidth * 2u + linedIntegralAndSquaredPaddingElements;

		centerBand_top += responseAreaWidth * 2u + linedIntegralAndSquaredPaddingElements;
		centerBand_bottom += responseAreaWidth * 2u + linedIntegralAndSquaredPaddingElements;

		bottomBand_top += responseAreaWidth * 2u + linedIntegralAndSquaredPaddingElements;
		bottomBand_bottom += responseAreaWidth * 2u + linedIntegralAndSquaredPaddingElements;

		horizontalResponses += horizontalResponsesPaddingElements;
	}

#ifdef OCEAN_DEBUG
	ocean_assert(horizontalResponses == debugHorizontalResponses + (height - responseAreaHeight + 1u) * horizontalResponsesStrideElements);
#endif

	ocean_assert(topBand_top == linedIntegralAndSquared + (height - responseAreaHeight + 1u) * linedIntegralAndSquaredStrideElements);
}

template void OCEAN_CV_DETECTOR_EXPORT ShapeDetector::PatternDetectorGradientVarianceBased::determineHorizontalResponsesI<true>(const uint32_t*, const unsigned int, const unsigned int, int32_t*, const unsigned int, const unsigned int);
template void OCEAN_CV_DETECTOR_EXPORT ShapeDetector::PatternDetectorGradientVarianceBased::determineHorizontalResponsesI<false>(const uint32_t*, const unsigned int, const unsigned int, int32_t*, const unsigned int, const unsigned int);

template <bool tUseSIMD>
void ShapeDetector::PatternDetectorGradientVarianceBased::determineHorizontalResponsesI(const uint32_t* linedIntegral, const uint64_t* linedIntegralSquared, const unsigned int width, const unsigned int height, int32_t* horizontalResponses, const unsigned int linedIntegralPaddingElements, const unsigned int linedIntegralSquaredPaddingElements, const unsigned int horizontalResponsesPaddingElements)
{
	static_assert(shapeWidth_ >= 1u && shapeWidth_ % 2u == 1u, "Invalid shape width!");
	static_assert(shapeHeight_ >= shapeStepSize_ + 1u, "Invalid shape height!");
	static_assert(shapeStepSize_ >= 1u && shapeStepSize_ % 2u == 1u, "Invalid shape step size!");
	static_assert(shapeBandSize_ >= 1u, "Invalid shape band size!");

	constexpr unsigned int precision = 10u;
	constexpr unsigned int factorPrecision = 1u << precision;

	constexpr unsigned int responseAreaWidth = shapeWidth_;
	constexpr unsigned int responseAreaHeight = shapeBandSize_ * 2u + shapeStepSize_;

	// the width of the left and right band box
	constexpr unsigned int shapeBandBoxWidth = shapeWidth_2_ - shapeStepSize_2_;
	constexpr unsigned int shapeBandBoxOffset = shapeWidth_ - shapeWidth_2_ + shapeStepSize_2_;

	constexpr unsigned int twoBandBoxesSize = shapeBandBoxWidth * shapeBandSize_ * 2u;
	constexpr unsigned int fourBandBoxesSize = twoBandBoxesSize * 2u;

	constexpr unsigned int centerSize = shapeWidth_;

	// as factorPrecision / (twoBandBoxesSize * centerSize) is ~1.4, we need to apply a multiplication + shift (division with power of two) to get a more precise response
	constexpr int32_t invTwoBandBoxesCenterSize_normalization = 64;
	constexpr int32_t invTwoBandBoxesCenterSize_precision = int((factorPrecision * invTwoBandBoxesCenterSize_normalization + (twoBandBoxesSize * centerSize) / 2u) / (twoBandBoxesSize * centerSize));

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
	const uint32x2_t centerSize_u_32x2 = vdup_n_u32(centerSize);
	const uint32x4_t centerSize_u_32x4 = vdupq_n_u32(centerSize);
	const uint32x4_t twoBandBoxesSize_u_32x4 = vdupq_n_u32(twoBandBoxesSize);
	const uint32x2_t fourBandBoxesSize_u_32x2 = vdup_n_u32(fourBandBoxesSize);

	const int32x4_t invTwoBandBoxesCenterSize_precision_s_32x4 = vdupq_n_s32(invTwoBandBoxesCenterSize_precision);

	const uint32x4_t constant_1_u_32x4 = vdupq_n_u32(1u);
	const uint32x4_t constant_7_u_32x4 = vdupq_n_u32(7u);
	const uint32x4_t constant_287_u_32x4 = vdupq_n_u32(287u);

	const uint32x4_t minimalGradientResponse_precision_u_32x4 = vdupq_n_u32(factorPrecision / 2u);
#endif

#ifdef OCEAN_INTENSIVE_DEBUG
	constexpr float debugInvTwoBandBoxesSize = 1.0f / float(twoBandBoxesSize);

	constexpr float debugInvCenterSize = 1.0f / float(centerSize);
	constexpr float debugInvCenterSizeSqr = 1.0f / float(centerSize * centerSize);

	constexpr float debugInvFourBandBoxesSizeSqr = 1.0f / float(fourBandBoxesSize * fourBandBoxesSize);
#endif

	ocean_assert(linedIntegral != nullptr);
	ocean_assert(linedIntegralSquared != nullptr);
	ocean_assert(width >= shapeWidth_ && width >= 20u);
	ocean_assert(height >= shapeHeight_ && height >= 20u);
	ocean_assert(horizontalResponses != nullptr);

#ifdef OCEAN_DEBUG
	const int32_t* const debugHorizontalResponses = horizontalResponses;
#endif

	const unsigned int coreResponseWidth = width - responseAreaWidth + 1u;
	const unsigned int coreResponseHeight = height - responseAreaHeight + 1u;

	ocean_assert(coreResponseWidth >= 4u);

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
	constexpr unsigned int xResponseSteps = tUseSIMD ? 4u : 1u;
#else
	constexpr unsigned int xResponseSteps = 1u;
#endif

	const unsigned int linedIntegralStrideElements = (width + 1u) + linedIntegralPaddingElements;
	const unsigned int linedIntegralSquaredStrideElements = (width + 1u) + linedIntegralSquaredPaddingElements;
	const unsigned int horizontalResponsesStrideElements = coreResponseWidth + horizontalResponsesPaddingElements;

	const uint32_t* topBand_top = linedIntegral;
	const uint32_t* topBand_bottom = linedIntegral + linedIntegralStrideElements * shapeBandSize_;
	const uint64_t* topBandSquared_top = linedIntegralSquared;
	const uint64_t* topBandSquared_bottom = linedIntegralSquared + linedIntegralSquaredStrideElements * shapeBandSize_;

	const uint32_t* centerBand_top = linedIntegral + linedIntegralStrideElements * (shapeBandSize_ + shapeStepSize_2_);
	const uint32_t* centerBand_bottom = linedIntegral + linedIntegralStrideElements * (shapeBandSize_ + shapeStepSize_2_ + 1u);
	const uint64_t* centerBandSquared_top = linedIntegralSquared + linedIntegralSquaredStrideElements * (shapeBandSize_ + shapeStepSize_2_);
	const uint64_t* centerBandSquared_bottom = linedIntegralSquared + linedIntegralSquaredStrideElements * (shapeBandSize_ + shapeStepSize_2_ + 1u);

	const uint32_t* bottomBand_top = linedIntegral + linedIntegralStrideElements * (shapeBandSize_ + shapeStepSize_);
	const uint32_t* bottomBand_bottom = linedIntegral + linedIntegralStrideElements * (shapeBandSize_ * 2u + shapeStepSize_);
	const uint64_t* bottomBandSquared_top = linedIntegralSquared + linedIntegralSquaredStrideElements * (shapeBandSize_ + shapeStepSize_);
	const uint64_t* bottomBandSquared_bottom = linedIntegralSquared + linedIntegralSquaredStrideElements * (shapeBandSize_ * 2u + shapeStepSize_);

	for (unsigned int yResponse = 0u; yResponse < coreResponseHeight; ++yResponse)
	{
		ocean_assert_and_suppress_unused((horizontalResponses - debugHorizontalResponses) % horizontalResponsesStrideElements == 0u, horizontalResponsesStrideElements);
		ocean_assert((topBand_top - linedIntegral) % linedIntegralStrideElements == 0u);
		ocean_assert((topBandSquared_top - linedIntegralSquared) % linedIntegralSquaredStrideElements == 0u);

		for (unsigned int xResponse = 0u; xResponse < coreResponseWidth; xResponse += xResponseSteps)
		{
#ifdef OCEAN_DEBUG
			// The response offset for the top-down T-shape is (shapeWidth / 2)x(shapeBandSize + shapeStepSize / 2)

			// x and y location to which the response belongs (for top-down filter)
			const unsigned int debugFrameTopDownX = xResponse + shapeWidth_2_;
			const unsigned int debugFrameTopDownY = yResponse + shapeBandSize_ + shapeStepSize_2_;

			ocean_assert(debugFrameTopDownX < width);
			ocean_assert(debugFrameTopDownY < height);
#endif

			if constexpr (xResponseSteps != 1u) // constexpr evaluated at compile time
			{
				if (xResponse + xResponseSteps > coreResponseWidth)
				{
					// the last iteration will not fit
					// so we simply shift x left by some pixels (at most 3) and we will calculate some pixels again

					ocean_assert(xResponse >= xResponseSteps && coreResponseWidth > xResponseSteps);
					const unsigned int newXResponse = coreResponseWidth - xResponseSteps;

					ocean_assert(xResponse > newXResponse);
					const unsigned int offset = xResponse - newXResponse;

					topBand_top -= offset;
					topBand_bottom -= offset;
					topBandSquared_top -= offset;
					topBandSquared_bottom -= offset;

					centerBand_top -= offset;
					centerBand_bottom -= offset;
					centerBandSquared_top -= offset;
					centerBandSquared_bottom -= offset;

					bottomBand_top -= offset;
					bottomBand_bottom -= offset;
					bottomBandSquared_top -= offset;
					bottomBandSquared_bottom -= offset;

					horizontalResponses -= offset;

					xResponse = newXResponse;

					// the for loop will stop after this iteration
					ocean_assert(!(xResponse + xResponseSteps < coreResponseWidth));
				}

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

				// top left band box
				const uint32x4_t topLeftSumA_u_32x4 = vld1q_u32(topBand_top);
				const uint32x4_t topLeftSumB_u_32x4 = vld1q_u32(topBand_top + shapeBandBoxWidth);
				const uint32x4_t topLeftSumC_u_32x4 = vld1q_u32(topBand_bottom);
				const uint32x4_t topLeftSumD_u_32x4 = vld1q_u32(topBand_bottom + shapeBandBoxWidth);
				const uint32x4_t topLeftSum_u_32x4 = vsubq_u32(vaddq_u32(topLeftSumA_u_32x4, topLeftSumD_u_32x4), vaddq_u32(topLeftSumB_u_32x4, topLeftSumC_u_32x4)); // (A + D) - (B + C)

				uint64x2_t topLeftSquareSumA_u_64x2 = vld1q_u64(topBandSquared_top);
				uint64x2_t topLeftSquareSumB_u_64x2 = vld1q_u64(topBandSquared_top + shapeBandBoxWidth);
				uint64x2_t topLeftSquareSumC_u_64x2 = vld1q_u64(topBandSquared_bottom);
				uint64x2_t topLeftSquareSumD_u_64x2 = vld1q_u64(topBandSquared_bottom + shapeBandBoxWidth);
				const uint64x2_t topLeftSquareSum01_u_64x2 = vsubq_u64(vaddq_u64(topLeftSquareSumA_u_64x2, topLeftSquareSumD_u_64x2), vaddq_u64(topLeftSquareSumB_u_64x2, topLeftSquareSumC_u_64x2));

				topLeftSquareSumA_u_64x2 = vld1q_u64(topBandSquared_top + 2);
				topLeftSquareSumB_u_64x2 = vld1q_u64(topBandSquared_top + shapeBandBoxWidth + 2u);
				topLeftSquareSumC_u_64x2 = vld1q_u64(topBandSquared_bottom + 2);
				topLeftSquareSumD_u_64x2 = vld1q_u64(topBandSquared_bottom + shapeBandBoxWidth + 2u);
				const uint64x2_t topLeftSquareSum23_u_64x2 = vsubq_u64(vaddq_u64(topLeftSquareSumA_u_64x2, topLeftSquareSumD_u_64x2), vaddq_u64(topLeftSquareSumB_u_64x2, topLeftSquareSumC_u_64x2));

				// top right band box
				const uint32x4_t topRightSumA_u_32x4 = vld1q_u32(topBand_top + shapeBandBoxOffset);
				const uint32x4_t topRightSumB_u_32x4 = vld1q_u32(topBand_top + shapeBandBoxOffset + shapeBandBoxWidth);
				const uint32x4_t topRightSumC_u_32x4 = vld1q_u32(topBand_bottom + shapeBandBoxOffset);
				const uint32x4_t topRightSumD_u_32x4 = vld1q_u32(topBand_bottom + shapeBandBoxOffset + shapeBandBoxWidth);
				const uint32x4_t topRightSum_u_32x4 = vsubq_u32(vaddq_u32(topRightSumA_u_32x4, topRightSumD_u_32x4), vaddq_u32(topRightSumB_u_32x4, topRightSumC_u_32x4));

				uint64x2_t topRightSquareSumA_u_64x2 = vld1q_u64(topBandSquared_top + shapeBandBoxOffset);
				uint64x2_t topRightSquareSumB_u_64x2 = vld1q_u64(topBandSquared_top + shapeBandBoxOffset + shapeBandBoxWidth);
				uint64x2_t topRightSquareSumC_u_64x2 = vld1q_u64(topBandSquared_bottom + shapeBandBoxOffset);
				uint64x2_t topRightSquareSumD_u_64x2 = vld1q_u64(topBandSquared_bottom + shapeBandBoxOffset + shapeBandBoxWidth);
				const uint64x2_t topRightSquareSum01_u_64x2 = vsubq_u64(vaddq_u64(topRightSquareSumA_u_64x2, topRightSquareSumD_u_64x2), vaddq_u64(topRightSquareSumB_u_64x2, topRightSquareSumC_u_64x2));

				topRightSquareSumA_u_64x2 = vld1q_u64(topBandSquared_top + shapeBandBoxOffset + 2u);
				topRightSquareSumB_u_64x2 = vld1q_u64(topBandSquared_top + shapeBandBoxOffset + shapeBandBoxWidth + 2u);
				topRightSquareSumC_u_64x2 = vld1q_u64(topBandSquared_bottom + shapeBandBoxOffset + 2u);
				topRightSquareSumD_u_64x2 = vld1q_u64(topBandSquared_bottom + shapeBandBoxOffset + shapeBandBoxWidth + 2u);
				const uint64x2_t topRightSquareSum23_u_64x2 = vsubq_u64(vaddq_u64(topRightSquareSumA_u_64x2, topRightSquareSumD_u_64x2), vaddq_u64(topRightSquareSumB_u_64x2, topRightSquareSumC_u_64x2));

				// top mean
				const uint32x4_t topSum_adjusted_u_32x4 = vmulq_u32(vaddq_u32(topLeftSum_u_32x4, topRightSum_u_32x4), centerSize_u_32x4); // topMean * twoBandBoxesSize * centerSize


				// center mean
				const uint32x4_t centerSumA_u_32x4 = vld1q_u32(centerBand_top);
				const uint32x4_t centerSumB_u_32x4 = vld1q_u32(centerBand_top + shapeWidth_);
				const uint32x4_t centerSumC_u_32x4 = vld1q_u32(centerBand_bottom);
				const uint32x4_t centerSumD_u_32x4 = vld1q_u32(centerBand_bottom + shapeWidth_);
				const uint32x4_t centerSum_u_32x4 = vsubq_u32(vaddq_u32(centerSumA_u_32x4, centerSumD_u_32x4), vaddq_u32(centerSumB_u_32x4, centerSumC_u_32x4));

				uint64x2_t centerSquareSumA_u_64x2 = vld1q_u64(centerBandSquared_top);
				uint64x2_t centerSquareSumB_u_64x2 = vld1q_u64(centerBandSquared_top + shapeWidth_);
				uint64x2_t centerSquareSumC_u_64x2 = vld1q_u64(centerBandSquared_bottom);
				uint64x2_t centerSquareSumD_u_64x2 = vld1q_u64(centerBandSquared_bottom + shapeWidth_);
				const uint64x2_t centerSquareSum01_u_64x2 = vsubq_u64(vaddq_u64(centerSquareSumA_u_64x2, centerSquareSumD_u_64x2), vaddq_u64(centerSquareSumB_u_64x2, centerSquareSumC_u_64x2));

				centerSquareSumA_u_64x2 = vld1q_u64(centerBandSquared_top + 2);
				centerSquareSumB_u_64x2 = vld1q_u64(centerBandSquared_top + shapeWidth_ + 2u);
				centerSquareSumC_u_64x2 = vld1q_u64(centerBandSquared_bottom + 2);
				centerSquareSumD_u_64x2 = vld1q_u64(centerBandSquared_bottom + shapeWidth_ + 2u);
				const uint64x2_t centerSquareSum23_u_64x2 = vsubq_u64(vaddq_u64(centerSquareSumA_u_64x2, centerSquareSumD_u_64x2), vaddq_u64(centerSquareSumB_u_64x2, centerSquareSumC_u_64x2));

				const uint32x4_t centerSum_adjusted_u_32x4 = vmulq_u32(centerSum_u_32x4, twoBandBoxesSize_u_32x4); // centerMean * centerSize * twoBandBoxesSize


				// bottom left band box
				const uint32x4_t bottomLeftSumA_u_32x4 = vld1q_u32(bottomBand_top);
				const uint32x4_t bottomLeftSumB_u_32x4 = vld1q_u32(bottomBand_top + shapeBandBoxWidth);
				const uint32x4_t bottomLeftSumC_u_32x4 = vld1q_u32(bottomBand_bottom);
				const uint32x4_t bottomLeftSumD_u_32x4 = vld1q_u32(bottomBand_bottom + shapeBandBoxWidth);
				const uint32x4_t bottomLeftSum_u_32x4 = vsubq_u32(vaddq_u32(bottomLeftSumA_u_32x4, bottomLeftSumD_u_32x4), vaddq_u32(bottomLeftSumB_u_32x4, bottomLeftSumC_u_32x4));

				uint64x2_t bottomLeftSquareSumA_u_64x2 = vld1q_u64(bottomBandSquared_top);
				uint64x2_t bottomLeftSquareSumB_u_64x2 = vld1q_u64(bottomBandSquared_top + shapeBandBoxWidth);
				uint64x2_t bottomLeftSquareSumC_u_64x2 = vld1q_u64(bottomBandSquared_bottom);
				uint64x2_t bottomLeftSquareSumD_u_64x2 = vld1q_u64(bottomBandSquared_bottom + shapeBandBoxWidth);
				const uint64x2_t bottomLeftSquareSum01_u_64x2 = vsubq_u64(vaddq_u64(bottomLeftSquareSumA_u_64x2, bottomLeftSquareSumD_u_64x2), vaddq_u64(bottomLeftSquareSumB_u_64x2, bottomLeftSquareSumC_u_64x2));

				bottomLeftSquareSumA_u_64x2 = vld1q_u64(bottomBandSquared_top + 2);
				bottomLeftSquareSumB_u_64x2 = vld1q_u64(bottomBandSquared_top + shapeBandBoxWidth + 2u);
				bottomLeftSquareSumC_u_64x2 = vld1q_u64(bottomBandSquared_bottom + 2);
				bottomLeftSquareSumD_u_64x2 = vld1q_u64(bottomBandSquared_bottom + shapeBandBoxWidth + 2u);
				const uint64x2_t bottomLeftSquareSum23_u_64x2 = vsubq_u64(vaddq_u64(bottomLeftSquareSumA_u_64x2, bottomLeftSquareSumD_u_64x2), vaddq_u64(bottomLeftSquareSumB_u_64x2, bottomLeftSquareSumC_u_64x2));

				// bottom right band box
				const uint32x4_t bottomRightSumA_u_32x4 = vld1q_u32(bottomBand_top + shapeBandBoxOffset);
				const uint32x4_t bottomRightSumB_u_32x4 = vld1q_u32(bottomBand_top + shapeBandBoxOffset + shapeBandBoxWidth);
				const uint32x4_t bottomRightSumC_u_32x4 = vld1q_u32(bottomBand_bottom + shapeBandBoxOffset);
				const uint32x4_t bottomRightSumD_u_32x4 = vld1q_u32(bottomBand_bottom + shapeBandBoxOffset + shapeBandBoxWidth);
				const uint32x4_t bottomRightSum_u_32x4 = vsubq_u32(vaddq_u32(bottomRightSumA_u_32x4, bottomRightSumD_u_32x4), vaddq_u32(bottomRightSumB_u_32x4, bottomRightSumC_u_32x4));

				uint64x2_t bottomRightSquareSumA_u_64x2 = vld1q_u64(bottomBandSquared_top + shapeBandBoxOffset);
				uint64x2_t bottomRightSquareSumB_u_64x2 = vld1q_u64(bottomBandSquared_top + shapeBandBoxOffset + shapeBandBoxWidth);
				uint64x2_t bottomRightSquareSumC_u_64x2 = vld1q_u64(bottomBandSquared_bottom + shapeBandBoxOffset);
				uint64x2_t bottomRightSquareSumD_u_64x2 = vld1q_u64(bottomBandSquared_bottom + shapeBandBoxOffset + shapeBandBoxWidth);
				const uint64x2_t bottomRightSquareSum01_u_64x2 = vsubq_u64(vaddq_u64(bottomRightSquareSumA_u_64x2, bottomRightSquareSumD_u_64x2), vaddq_u64(bottomRightSquareSumB_u_64x2, bottomRightSquareSumC_u_64x2));

				bottomRightSquareSumA_u_64x2 = vld1q_u64(bottomBandSquared_top + shapeBandBoxOffset + 2u);
				bottomRightSquareSumB_u_64x2 = vld1q_u64(bottomBandSquared_top + shapeBandBoxOffset + shapeBandBoxWidth + 2u);
				bottomRightSquareSumC_u_64x2 = vld1q_u64(bottomBandSquared_bottom + shapeBandBoxOffset + 2u);
				bottomRightSquareSumD_u_64x2 = vld1q_u64(bottomBandSquared_bottom + shapeBandBoxOffset + shapeBandBoxWidth + 2u);
				const uint64x2_t bottomRightSquareSum23_u_64x2 = vsubq_u64(vaddq_u64(bottomRightSquareSumA_u_64x2, bottomRightSquareSumD_u_64x2), vaddq_u64(bottomRightSquareSumB_u_64x2, bottomRightSquareSumC_u_64x2));

				// bottom mean
				const uint32x4_t bottomSum_adjusted_u_32x4 = vmulq_u32(vaddq_u32(bottomLeftSum_u_32x4, bottomRightSum_u_32x4), centerSize_u_32x4); // bottomMean * twoBandBoxesSize * centerSize


				// gradient

				const int32x4_t topGradient_precision_s_32x4 = vrshrq_n_s32(vmulq_s32(vreinterpretq_s32_u32(vsubq_u32(centerSum_adjusted_u_32x4, topSum_adjusted_u_32x4)), invTwoBandBoxesCenterSize_precision_s_32x4), 6); // ((centerSum - topSum) * invTwoBandBoxesCenterSize_precision_s_32x4) / invTwoBandBoxesCenterSize_normalization_s_32x4
				const int32x4_t bottomGradient_precision_s_32x4 = vrshrq_n_s32(vmulq_s32(vreinterpretq_s32_u32(vsubq_u32(bottomSum_adjusted_u_32x4, centerSum_adjusted_u_32x4)), invTwoBandBoxesCenterSize_precision_s_32x4), 6); // ((bottomSum - centerSum) * invTwoBandBoxesCenterSize_precision_s_32x4) / invTwoBandBoxesCenterSize_normalization_s_32x4

				const int32x4_t horizontalResponse_precision_s_32x4 = vsubq_s32(topGradient_precision_s_32x4, bottomGradient_precision_s_32x4);

				const uint32x4_t absTopGradient_precision_u_32x4 = vreinterpretq_u32_s32(vabsq_s32(topGradient_precision_s_32x4));
				const uint32x4_t absBottomGradient_precision_u_32x4 = vreinterpretq_u32_s32(vabsq_s32(bottomGradient_precision_s_32x4));


				// abs(topGradient) >= minimalGradient && abs(bottomGradient) >= minimalGradient
				const uint32x4_t validGradient_u_32x4 = vandq_u32(vcgeq_u32(absTopGradient_precision_u_32x4, minimalGradientResponse_precision_u_32x4), vcgeq_u32(absBottomGradient_precision_u_32x4, minimalGradientResponse_precision_u_32x4));

#ifdef OCEAN_INTENSIVE_DEBUG
				const uint32_t debugTopLeftSum = topBand_top[0] - topBand_top[shapeBandBoxWidth] - topBand_bottom[0] + topBand_bottom[shapeBandBoxWidth];
				const uint64_t debugTopLeftSquareSum = topBandSquared_top[0] - topBandSquared_top[shapeBandBoxWidth] - topBandSquared_bottom[0] + topBandSquared_bottom[shapeBandBoxWidth];
				const uint32_t debugTopRightSum = topBand_top[shapeBandBoxOffset] - topBand_top[shapeBandBoxOffset + shapeBandBoxWidth] - topBand_bottom[shapeBandBoxOffset] + topBand_bottom[shapeBandBoxOffset + shapeBandBoxWidth];
				const uint64_t debugTopRightSquareSum = topBandSquared_top[shapeBandBoxOffset] - topBandSquared_top[shapeBandBoxOffset + shapeBandBoxWidth] - topBandSquared_bottom[shapeBandBoxOffset] + topBandSquared_bottom[shapeBandBoxOffset + shapeBandBoxWidth];
				const uint32_t debugTopSum_adjusted = (debugTopLeftSum + debugTopRightSum) * centerSize;
				ocean_assert(debugTopSum_adjusted < 256u * twoBandBoxesSize * centerSize);

				const uint32_t debugCenterSum = centerBand_top[0] - centerBand_top[shapeWidth_] - centerBand_bottom[0] + centerBand_bottom[shapeWidth_];
				const uint64_t debugCenterSquareSum = centerBandSquared_top[0] - centerBandSquared_top[shapeWidth_] - centerBandSquared_bottom[0] + centerBandSquared_bottom[shapeWidth_];
				const uint32_t debugCenterSum_adjusted = debugCenterSum * twoBandBoxesSize;
				ocean_assert(debugCenterSum_adjusted < 256u * centerSize * twoBandBoxesSize);

				const uint32_t debugBottomLeftSum = bottomBand_top[0] - bottomBand_top[shapeBandBoxWidth] - bottomBand_bottom[0] + bottomBand_bottom[shapeBandBoxWidth];
				const uint64_t debugBottomLeftSquareSum = bottomBandSquared_top[0] - bottomBandSquared_top[shapeBandBoxWidth] - bottomBandSquared_bottom[0] + bottomBandSquared_bottom[shapeBandBoxWidth];
				const uint32_t debugBottomRightSum = bottomBand_top[shapeBandBoxOffset] - bottomBand_top[shapeBandBoxOffset + shapeBandBoxWidth] - bottomBand_bottom[shapeBandBoxOffset] + bottomBand_bottom[shapeBandBoxOffset + shapeBandBoxWidth];
				const uint64_t debugBottomRightSquareSum = bottomBandSquared_top[shapeBandBoxOffset] - bottomBandSquared_top[shapeBandBoxOffset + shapeBandBoxWidth] - bottomBandSquared_bottom[shapeBandBoxOffset] + bottomBandSquared_bottom[shapeBandBoxOffset + shapeBandBoxWidth];
				const uint32_t debugBottomSum_adjusted = (debugBottomLeftSum + debugBottomRightSum) * centerSize;
				ocean_assert(debugBottomSum_adjusted < 256u * twoBandBoxesSize * centerSize);

				const int32_t debugTopGradient_precision = (int32_t(debugCenterSum_adjusted - debugTopSum_adjusted) * invTwoBandBoxesCenterSize_precision) / invTwoBandBoxesCenterSize_normalization;
				const int32_t debugBottomGradient_precision = (int32_t(debugBottomSum_adjusted - debugCenterSum_adjusted) * invTwoBandBoxesCenterSize_precision) / invTwoBandBoxesCenterSize_normalization;
				const int32_t debugHorizontalResponse_precision = debugTopGradient_precision - debugBottomGradient_precision;
				ocean_assert(abs(debugHorizontalResponse_precision) < 256 * 2 * int(factorPrecision));

				ocean_assert(NumericT<uint32_t>::isEqual(vgetq_lane_u32(horizontalResponse_precision_s_32x4, 0), debugHorizontalResponse_precision, 16u));

				constexpr uint32_t debugMinimalGradientResponse_precision = (factorPrecision + 1u) / 2u;
				const bool debugFirstResponseValid = (sign == 0 || NumericT<int32_t>::sign(debugHorizontalResponse_precision) == sign) && NumericT<int32_t>::secureAbs(debugTopGradient_precision) >= debugMinimalGradientResponse_precision && NumericT<int32_t>::secureAbs(debugBottomGradient_precision) >= debugMinimalGradientResponse_precision;
				OCEAN_SUPPRESS_UNUSED_WARNING(debugFirstResponseValid);
#endif // OCEAN_INTENSIVE_DEBUG

				const uint32x4_t absHorizontalResponse_precision_8_u_32x4 = vrshrq_n_u32(vreinterpretq_u32_s32(vabsq_s32(horizontalResponse_precision_s_32x4)), 3); // (|horizontalRespones| + 4) / 8
				const uint32x4_t sqrHorizontalResponse_16384_u_32x4 = vmulq_u32(absHorizontalResponse_precision_8_u_32x4, absHorizontalResponse_precision_8_u_32x4); // gradient^2 * 16,384 < 2^32

				const uint32x2_t centerSum01_u_32x2 = vget_low_u32(centerSum_u_32x4);
				const uint32x2_t centerSum23_u_32x2 = vget_high_u32(centerSum_u_32x4);

				const uint64x2_t centerVarianceSmallSizeSqr01_u_64x2 = vmlsl_u32(NEON::multiply(centerSquareSum01_u_64x2, centerSize_u_32x2), centerSum01_u_32x2, centerSum01_u_32x2); // centerVariance * 225
				const uint64x2_t centerVarianceSmallSizeSqr23_u_64x2 = vmlsl_u32(NEON::multiply(centerSquareSum23_u_64x2, centerSize_u_32x2), centerSum23_u_32x2, centerSum23_u_32x2);
				const uint32x4_t centerVarianceSmallSizeSqr_u_32x4 = vcombine_u32(vmovn_u64(centerVarianceSmallSizeSqr01_u_64x2), vmovn_u64(centerVarianceSmallSizeSqr23_u_64x2));

				const uint32x4_t topBottomSum_u_32x4 = vaddq_u32(vaddq_u32(topLeftSum_u_32x4, topRightSum_u_32x4), vaddq_u32(bottomLeftSum_u_32x4, bottomRightSum_u_32x4));
				const uint64x2_t topBottomSquareSum01_u_64x2 = vaddq_u64(vaddq_u64(topLeftSquareSum01_u_64x2, topRightSquareSum01_u_64x2), vaddq_u64(bottomLeftSquareSum01_u_64x2, bottomRightSquareSum01_u_64x2));
				const uint64x2_t topBottomSquareSum23_u_64x2 = vaddq_u64(vaddq_u64(topLeftSquareSum23_u_64x2, topRightSquareSum23_u_64x2), vaddq_u64(bottomLeftSquareSum23_u_64x2, bottomRightSquareSum23_u_64x2));

				const uint32x2_t topBottomSum01_u_64x2 = vget_low_u32(topBottomSum_u_32x4);
				const uint32x2_t topBottomSum23_u_64x2 = vget_high_u32(topBottomSum_u_32x4);

				const uint64x2_t topBottomVarianceLargeSizeSqr01_u_64x2 = vmlsl_u32(NEON::multiply(topBottomSquareSum01_u_64x2, fourBandBoxesSize_u_32x2), topBottomSum01_u_64x2, topBottomSum01_u_64x2); // topBottomVariance * 9,216
				const uint64x2_t topBottomVarianceLargeSizeSqr23_u_64x2 = vmlsl_u32(NEON::multiply(topBottomSquareSum23_u_64x2, fourBandBoxesSize_u_32x2), topBottomSum23_u_64x2, topBottomSum23_u_64x2);
				const uint32x4_t topBottomVarianceLargeSizeSqr_u_32x4 = vcombine_u32(vmovn_u64(topBottomVarianceLargeSizeSqr01_u_64x2), vmovn_u64(topBottomVarianceLargeSizeSqr23_u_64x2));

				const uint32x4_t topBottomAdjustedVariance_u_32x4 = vrshrq_n_u32(vmulq_u32(topBottomVarianceLargeSizeSqr_u_32x4, constant_7_u_32x4), 2); // (topBottomVarianceLargeSizeSqr * 7 + 2) / 4
				const uint32x4_t centerAdjustedVariance_u_32x4 = vrshrq_n_u32(vmulq_u32(centerVarianceSmallSizeSqr_u_32x4, constant_287_u_32x4), 2); // (centerVarianceSmallSizeSqr * 287 + 2) / 4

				const uint32x4_t horizontalVariance_16384_u_32x4 = vrhaddq_u32(topBottomAdjustedVariance_u_32x4, centerAdjustedVariance_u_32x4); // (topBottomAdjustedVariance + centerAdjustedVariance + 1) / 2

#ifdef OCEAN_INTENSIVE_DEBUG
				const uint32_t debugAbsHorizontalResponse_precision_8 = (NumericT<int32_t>::secureAbs(debugHorizontalResponse_precision) + 4u) / 8u;
				const uint32_t debugSqrHorizontalResponse_16384 = debugAbsHorizontalResponse_precision_8 * debugAbsHorizontalResponse_precision_8;
				const uint32_t debugCenterVarianceSmallSizeSqr = uint32_t(debugCenterSquareSum * uint64_t(centerSize) - uint64_t(debugCenterSum * debugCenterSum));
				const uint32_t debugTopBottomSum = debugTopLeftSum + debugTopRightSum + debugBottomLeftSum + debugBottomRightSum;
				const uint64_t debugTopBottomSquareSum = debugTopLeftSquareSum + debugTopRightSquareSum + debugBottomLeftSquareSum + debugBottomRightSquareSum;
				const uint32_t debugTopBottomVarianceLargeSizeSqr = uint32_t(debugTopBottomSquareSum * uint64_t(fourBandBoxesSize) - uint64_t(debugTopBottomSum * debugTopBottomSum));
				const uint32_t debugHorizontalVariance_16384 = ((((debugTopBottomVarianceLargeSizeSqr * 7u) + 2u) / 4u + ((debugCenterVarianceSmallSizeSqr * 287u) + 2u) / 4u) + 1u) / 2u;

				ocean_assert(vgetq_lane_u32(horizontalVariance_16384_u_32x4, 0) == debugHorizontalVariance_16384);
#endif // OCEAN_INTENSIVE_DEBUG

				const uint32x4_t responseDifference_u_32x4 = vqsubq_u32(sqrHorizontalResponse_16384_u_32x4, horizontalVariance_16384_u_32x4); // abs(0, sqrHorizontalResponse_16384 - horizontalVariance_16384)

				const float32x4_t horizontalVariance_16384_f_32x4 = vcvtq_f32_u32(vmaxq_u32(constant_1_u_32x4, horizontalVariance_16384_u_32x4));

				// we calculate the (approximated) inverse of horizontalVariance: 1 / horizontalVariance
				float32x4_t invHorizontalVariance_16384_f_32x4 = vrecpeq_f32(horizontalVariance_16384_f_32x4);
				invHorizontalVariance_16384_f_32x4 = vmulq_f32(vrecpsq_f32(horizontalVariance_16384_f_32x4, invHorizontalVariance_16384_f_32x4), invHorizontalVariance_16384_f_32x4); // improving the accuracy of the approx. inverse by Newton/Raphson

				const uint32x4_t horziontalResponse_u_32x4 = vandq_u32(validGradient_u_32x4, vcvtq_u32_f32(vmulq_f32(vcvtq_f32_u32(responseDifference_u_32x4), invHorizontalVariance_16384_f_32x4)));

				vst1q_s32(horizontalResponses, NEON::copySign(horziontalResponse_u_32x4, horizontalResponse_precision_s_32x4));

				topBand_top += xResponseSteps;
				topBand_bottom += xResponseSteps;
				topBandSquared_top += xResponseSteps;
				topBandSquared_bottom += xResponseSteps;

				centerBand_top += xResponseSteps;
				centerBand_bottom += xResponseSteps;
				centerBandSquared_top += xResponseSteps;
				centerBandSquared_bottom += xResponseSteps;

				bottomBand_top += xResponseSteps;
				bottomBand_bottom += xResponseSteps;
				bottomBandSquared_top += xResponseSteps;
				bottomBandSquared_bottom += xResponseSteps;

				horizontalResponses += xResponseSteps;

#endif // defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
			}
			else
			{
				ocean_assert(xResponseSteps == 1u);

				// the integral image is a two-channel integral image with simple sum in the first channel, and sum of squared in the second channel

				// top left band box
				const uint32_t topLeftSum = topBand_top[0] - topBand_top[shapeBandBoxWidth] - topBand_bottom[0] + topBand_bottom[shapeBandBoxWidth];
				const uint64_t topLeftSquareSum = topBandSquared_top[0] - topBandSquared_top[shapeBandBoxWidth] - topBandSquared_bottom[0] + topBandSquared_bottom[shapeBandBoxWidth];

				// top right band box
				const uint32_t topRightSum = topBand_top[shapeBandBoxOffset] - topBand_top[shapeBandBoxOffset + shapeBandBoxWidth] - topBand_bottom[shapeBandBoxOffset] + topBand_bottom[shapeBandBoxOffset + shapeBandBoxWidth];
				const uint64_t topRightSquareSum = topBandSquared_top[shapeBandBoxOffset] - topBandSquared_top[shapeBandBoxOffset + shapeBandBoxWidth] - topBandSquared_bottom[shapeBandBoxOffset] + topBandSquared_bottom[shapeBandBoxOffset + shapeBandBoxWidth];

				const uint32_t topSum_adjusted = (topLeftSum + topRightSum) * centerSize; // topMean * twoBandBoxesSize * centerSize
				ocean_assert(topSum_adjusted < 256u * twoBandBoxesSize * centerSize);

#ifdef OCEAN_INTENSIVE_DEBUG
				const float debugTopMean = float(topLeftSum + topRightSum) * debugInvTwoBandBoxesSize;
				ocean_assert(debugTopMean >= 0.0f && debugTopMean < 256.0f);
				ocean_assert(NumericF::isEqual(debugTopMean, float(topSum_adjusted) / float(twoBandBoxesSize * centerSize), 0.01f));
#endif

				// center
				const uint32_t centerSum = centerBand_top[0] - centerBand_top[shapeWidth_] - centerBand_bottom[0] + centerBand_bottom[shapeWidth_];
				const uint64_t centerSquareSum = centerBandSquared_top[0] - centerBandSquared_top[shapeWidth_] - centerBandSquared_bottom[0] + centerBandSquared_bottom[shapeWidth_];

				const uint32_t centerSum_adjusted = centerSum * twoBandBoxesSize; // centerMean * centerSize * twoBandSize
				ocean_assert(centerSum_adjusted < 256u * centerSize * twoBandBoxesSize);

#ifdef OCEAN_INTENSIVE_DEBUG
				const float debugCenterMean = float(centerSum) * debugInvCenterSize;
				ocean_assert(debugCenterMean >= 0.0f && debugCenterMean < 256.0f);
				ocean_assert(NumericF::isEqual(debugCenterMean, float(centerSum_adjusted) / float(centerSize * twoBandBoxesSize), 0.01f));
#endif

				// bottom left band box
				const uint32_t bottomLeftSum = bottomBand_top[0] - bottomBand_top[shapeBandBoxWidth] - bottomBand_bottom[0] + bottomBand_bottom[shapeBandBoxWidth];
				const uint64_t bottomLeftSquareSum = bottomBandSquared_top[0] - bottomBandSquared_top[shapeBandBoxWidth] - bottomBandSquared_bottom[0] + bottomBandSquared_bottom[shapeBandBoxWidth];

				// bottom right band box
				const uint32_t bottomRightSum = bottomBand_top[shapeBandBoxOffset] - bottomBand_top[shapeBandBoxOffset + shapeBandBoxWidth] - bottomBand_bottom[shapeBandBoxOffset] + bottomBand_bottom[shapeBandBoxOffset + shapeBandBoxWidth];
				const uint64_t bottomRightSquareSum = bottomBandSquared_top[shapeBandBoxOffset] - bottomBandSquared_top[shapeBandBoxOffset + shapeBandBoxWidth] - bottomBandSquared_bottom[shapeBandBoxOffset] + bottomBandSquared_bottom[shapeBandBoxOffset + shapeBandBoxWidth];

				const uint32_t bottomSum_adjusted = (bottomLeftSum + bottomRightSum) * centerSize; // bottomMean * twoBandBoxesSize * centerSize
				ocean_assert(bottomSum_adjusted < 256u * twoBandBoxesSize * centerSize);

#ifdef OCEAN_INTENSIVE_DEBUG
				const float debugBottomMean = float(bottomLeftSum + bottomRightSum) * debugInvTwoBandBoxesSize;
				ocean_assert(debugBottomMean >= 0.0f && debugBottomMean < 256.0f);
				ocean_assert(NumericF::isEqual(debugBottomMean, float(bottomSum_adjusted) / float(twoBandBoxesSize * centerSize), 0.01f));
#endif

				const int32_t topGradient_precision = (int32_t(centerSum_adjusted - topSum_adjusted) * invTwoBandBoxesCenterSize_precision) / invTwoBandBoxesCenterSize_normalization;
				const int32_t bottomGradient_precision = (int32_t(bottomSum_adjusted - centerSum_adjusted) * invTwoBandBoxesCenterSize_precision) / invTwoBandBoxesCenterSize_normalization;
				ocean_assert(topGradient_precision >= -int(256u * factorPrecision) && topGradient_precision < int(256u * factorPrecision));
				ocean_assert(bottomGradient_precision >= -int(256u * factorPrecision) && bottomGradient_precision < int(256u * factorPrecision));

				const int32_t horizontalResponse_precision = topGradient_precision - bottomGradient_precision;
				ocean_assert(horizontalResponse_precision >= -int(256u * factorPrecision * 2u) && horizontalResponse_precision < int(256u * factorPrecision * 2u));

#ifdef OCEAN_INTENSIVE_DEBUG
				const float debugTopGradient = debugCenterMean - debugTopMean;
				const float debugBottomGradient = debugBottomMean - debugCenterMean;
				const float debugHorizontalResponse = debugTopGradient - debugBottomGradient;
				ocean_assert(NumericF::isEqual(debugHorizontalResponse, float(horizontalResponse_precision) / float(factorPrecision), 1.0f));
#endif

				constexpr uint32_t minimalGradientResponse_precision = (factorPrecision + 1u) / 2u; // minimalGradientResponse = 0.5f

				if (NumericT<int32_t>::secureAbs(topGradient_precision) >= minimalGradientResponse_precision && NumericT<int32_t>::secureAbs(bottomGradient_precision) >= minimalGradientResponse_precision)
				{
					static_assert(uint64_t(2u * 255u * factorPrecision) * uint64_t(2u * 255u * factorPrecision) / uint64_t(8u * 8u) <= uint64_t(0xFFFFFFFFu), "Out of range!");

					const uint32_t absHorizontalResponse_precision_8 = (NumericT<int32_t>::secureAbs(horizontalResponse_precision) + 4u) / 8u; // division by 8 to stay inside 32 bit when squaring below
					ocean_assert(uint64_t(absHorizontalResponse_precision_8) * uint64_t(absHorizontalResponse_precision_8) <= uint64_t(0xFFFFFFFFu));

					const uint32_t sqrHorizontalResponse_16384 = absHorizontalResponse_precision_8 * absHorizontalResponse_precision_8; // [(gradient * 1,024) / 8] ^ 2 == gradient^2 * 16,384 < 2^32

#ifdef OCEAN_INTENSIVE_DEBUG
					const float debugHorizontalResponseInteger = NumericF::sqrt(float(sqrHorizontalResponse_16384) / 16384.0f);
					ocean_assert(NumericF::isEqual(NumericF::abs(debugHorizontalResponse), debugHorizontalResponseInteger, 1.0f));
#endif

					const uint32_t centerVarianceSmallSizeSqr = uint32_t(centerSquareSum * uint64_t(centerSize) - uint64_t(centerSum * centerSum)); // centerVariance * 15^2 = centerVariance * 225
					ocean_assert(centerVarianceSmallSizeSqr < uint64_t(256u * 256u * 15u* 15u));

#ifdef OCEAN_INTENSIVE_DEBUG
					const float debugCenterVariance = float(float(centerSquareSum) * float(centerSize) - float(centerSum * centerSum)) * debugInvCenterSizeSqr;
					ocean_assert(debugCenterVariance >= 0.0f && debugCenterVariance < 256.0f * 256.0f);
					ocean_assert(NumericF::isWeakEqual(debugCenterVariance, float(centerVarianceSmallSizeSqr) / float(centerSize * centerSize)));
#endif

					const uint32_t topBottomSum = topLeftSum + topRightSum + bottomLeftSum + bottomRightSum;
					const uint64_t topBottomSquareSum = topLeftSquareSum + topRightSquareSum + bottomLeftSquareSum + bottomRightSquareSum;

					const uint32_t topBottomVarianceLargeSizeSqr = uint32_t(topBottomSquareSum * uint64_t(fourBandBoxesSize) - uint64_t(topBottomSum * topBottomSum)); // topBottomVariance * 96^2 = topBottomVariance * 9,216
					ocean_assert(uint64_t(topBottomVarianceLargeSizeSqr) < uint64_t(256u * 256u) * uint64_t(fourBandBoxesSize * fourBandBoxesSize)); // <= 255^2 * 96^2 < 600,000,000

#ifdef OCEAN_INTENSIVE_DEBUG
					const float debugTopBottomVariance = float(topBottomSquareSum * fourBandBoxesSize - topBottomSum * topBottomSum) * debugInvFourBandBoxesSizeSqr;
					ocean_assert(debugTopBottomVariance >= 0.0f && debugTopBottomVariance < 256.0f * 256.0f);
					ocean_assert(NumericF::isWeakEqual(debugTopBottomVariance, float(topBottomVarianceLargeSizeSqr) / float(fourBandBoxesSize * fourBandBoxesSize)));
#endif

					// getting same (similar) nominator for gradient, centerVariance, and topBottom variance

					// centerVariance, topBottomVariance
					// 9216 / 255 = 40.96 ~ 41

					// gradient, variance
					// division should be realized with shifts, therfore 2^a and 2^c
					// 16,384 / 2^a  ==  9,216 * b / 2^c,     while b <= 7, 7.166994 = 2^32 / (255^2 * 96^2)
					// 1.7777 / 2^a  ==  b / 2^c

					// best solution: a = 0, b = 7, c = 2: 7/4 = 1.75 ~ 1.7777

					const uint32_t horizontalVariance_16384 = ((((topBottomVarianceLargeSizeSqr * 7u) + 2u) / 4u + ((centerVarianceSmallSizeSqr * 287u) + 2u) / 4u) + 1u) / 2u; // 287 = 7 * 41

#ifdef OCEAN_INTENSIVE_DEBUG
					const float debugHorizontalVariance = (debugTopBottomVariance + debugCenterVariance) * 0.5f;
					const float debugHorizontalDeviation = NumericF::sqrt(debugHorizontalVariance);
					const float debugHorizontalDeviationInteger = NumericF::sqrt(float(horizontalVariance_16384) / 16384.0f);
					ocean_assert(NumericF::isEqual(debugHorizontalDeviation, debugHorizontalDeviationInteger, 1.0f));
#endif

					const uint32_t normalizedSqrHorizontalResponse = (uint32_t(std::max(0, int32_t(sqrHorizontalResponse_16384 - horizontalVariance_16384))) + horizontalVariance_16384 / 2u) / std::max(1u, horizontalVariance_16384);

#ifdef OCEAN_INTENSIVE_DEBUG
					const float debugNormalizedSqrHorizontalResponse = std::max(0.0f, NumericF::sqr(debugHorizontalResponse) - debugHorizontalVariance) / std::max(1.0f, debugHorizontalVariance);
					const float debugNormalizedHorizontalResponse = NumericF::sqrt(debugNormalizedSqrHorizontalResponse);
					const float debugNormalizedHorizontalResponseInteger = NumericF::sqrt(float(normalizedSqrHorizontalResponse));
					ocean_assert(debugNormalizedHorizontalResponse < 15.0f || NumericF::isEqual(debugNormalizedHorizontalResponse, debugNormalizedHorizontalResponseInteger, 1.0f));
#endif

					ocean_assert(normalizedSqrHorizontalResponse < uint32_t(NumericT<int32_t>::maxValue()));
					*horizontalResponses = NumericT<int32_t>::copySign(int32_t(normalizedSqrHorizontalResponse), horizontalResponse_precision);
				}
				else
				{
					*horizontalResponses = 0;
				}

				++topBand_top;
				++topBand_bottom;
				++topBandSquared_top;
				++topBandSquared_bottom;

				++centerBand_top;
				++centerBand_bottom;
				++centerBandSquared_top;
				++centerBandSquared_bottom;

				++bottomBand_top;
				++bottomBand_bottom;
				++bottomBandSquared_top;
				++bottomBandSquared_bottom;

				++horizontalResponses;
			}
		}

		topBand_top += responseAreaWidth + linedIntegralPaddingElements;
		topBand_bottom += responseAreaWidth + linedIntegralPaddingElements;
		topBandSquared_top += responseAreaWidth + linedIntegralSquaredPaddingElements;
		topBandSquared_bottom += responseAreaWidth + linedIntegralSquaredPaddingElements;

		centerBand_top += responseAreaWidth + linedIntegralPaddingElements;
		centerBand_bottom += responseAreaWidth + linedIntegralPaddingElements;
		centerBandSquared_top += responseAreaWidth + linedIntegralSquaredPaddingElements;
		centerBandSquared_bottom += responseAreaWidth + linedIntegralSquaredPaddingElements;

		bottomBand_top += responseAreaWidth + linedIntegralPaddingElements;
		bottomBand_bottom += responseAreaWidth + linedIntegralPaddingElements;
		bottomBandSquared_top += responseAreaWidth + linedIntegralSquaredPaddingElements;
		bottomBandSquared_bottom += responseAreaWidth + linedIntegralSquaredPaddingElements;

		horizontalResponses += horizontalResponsesPaddingElements;
	}

#ifdef OCEAN_DEBUG
	ocean_assert(horizontalResponses == debugHorizontalResponses + (height - responseAreaHeight + 1u) * horizontalResponsesStrideElements);
#endif

	ocean_assert(topBand_top == linedIntegral + (height - responseAreaHeight + 1u) * linedIntegralStrideElements);
	ocean_assert(topBandSquared_top == linedIntegralSquared + (height - responseAreaHeight + 1u) * linedIntegralSquaredStrideElements);
}

template void OCEAN_CV_DETECTOR_EXPORT ShapeDetector::PatternDetectorGradientVarianceBased::determineHorizontalResponsesI<true>(const uint32_t*, const uint64_t*, const unsigned int, const unsigned int, int32_t*, const unsigned int, const unsigned int, const unsigned int);
template void OCEAN_CV_DETECTOR_EXPORT ShapeDetector::PatternDetectorGradientVarianceBased::determineHorizontalResponsesI<false>(const uint32_t*, const uint64_t*, const unsigned int, const unsigned int, int32_t*, const unsigned int, const unsigned int, const unsigned int);

template <bool tSquaredResponse>
void ShapeDetector::PatternDetectorGradientVarianceBased::determineHorizontalResponsesF(const uint32_t* linedIntegralAndSquared, const unsigned int width, const unsigned int height, float* horizontalResponses, const unsigned int linedIntegralAndSquaredPaddingElements, const unsigned int horizontalResponsesPaddingElements)
{
	static_assert(shapeWidth_ >= 1u && shapeWidth_ % 2u == 1u, "Invalid shape width!");
	static_assert(shapeHeight_ >= shapeStepSize_ + 1u, "Invalid shape height!");
	static_assert(shapeStepSize_ >= 1u && shapeStepSize_ % 2u == 1u, "Invalid shape step size!");
	static_assert(shapeBandSize_ >= 1u, "Invalid shape band size!");

	constexpr unsigned int responseAreaWidth = shapeWidth_;
	constexpr unsigned int responseAreaHeight = shapeBandSize_ * 2u + shapeStepSize_;

	// the width of the left and right band box
	constexpr unsigned int shapeBandBoxWidth = shapeWidth_2_ - shapeStepSize_2_;
	constexpr unsigned int shapeBandBoxOffset = shapeWidth_ - shapeWidth_2_ + shapeStepSize_2_;

	constexpr unsigned int shapeBandBoxWidth2 = shapeBandBoxWidth * 2u;
	constexpr unsigned int shapeBandBoxOffset2 = shapeBandBoxOffset * 2u;

	constexpr unsigned int twoBandBoxesSize = shapeBandBoxWidth * shapeBandSize_ * 2u;
	constexpr float invTwoBandBoxesSize = 1.0f / float(twoBandBoxesSize);

	constexpr unsigned int fourBandBoxesSize = twoBandBoxesSize * 2u;
	constexpr float invFourBandBoxesSizeSqr = 1.0f / float(fourBandBoxesSize * fourBandBoxesSize);

	constexpr unsigned int centerSize = shapeWidth_;
	constexpr float invCenterSize = 1.0f / float(centerSize);
	constexpr float invCenterSizeSqr = 1.0f / float(centerSize * centerSize);

	ocean_assert(linedIntegralAndSquared != nullptr);
	ocean_assert(width >= shapeWidth_ && width >= 20u);
	ocean_assert(height >= shapeHeight_ && height >= 20u);
	ocean_assert(width * height <= 65536u);
	ocean_assert(horizontalResponses != nullptr);

#ifdef OCEAN_DEBUG
	const float* const debugHorizontalResponses = horizontalResponses;
#endif

	const unsigned int linedIntegralAndSquaredStrideElements = (width + 1u) * 2u + linedIntegralAndSquaredPaddingElements;
	const unsigned int horizontalResponsesStrideElements = width - responseAreaWidth + 1u + horizontalResponsesPaddingElements;

	const uint32_t* topBand_top = linedIntegralAndSquared;
	const uint32_t* topBand_bottom = linedIntegralAndSquared + linedIntegralAndSquaredStrideElements * shapeBandSize_;

	const uint32_t* centerBand_top = linedIntegralAndSquared + linedIntegralAndSquaredStrideElements * (shapeBandSize_ + shapeStepSize_2_);
	const uint32_t* centerBand_bottom = linedIntegralAndSquared + linedIntegralAndSquaredStrideElements * (shapeBandSize_ + shapeStepSize_2_ + 1u);

	const uint32_t* bottomBand_top = linedIntegralAndSquared + linedIntegralAndSquaredStrideElements * (shapeBandSize_ + shapeStepSize_);
	const uint32_t* bottomBand_bottom = linedIntegralAndSquared + linedIntegralAndSquaredStrideElements * (shapeBandSize_ * 2u + shapeStepSize_);

	for (unsigned int yResponse = 0u; yResponse < height - responseAreaHeight + 1u; ++yResponse)
	{
		ocean_assert_and_suppress_unused((horizontalResponses - debugHorizontalResponses) % horizontalResponsesStrideElements == 0u, horizontalResponsesStrideElements);
		ocean_assert((topBand_top - linedIntegralAndSquared) % linedIntegralAndSquaredStrideElements == 0u);

		for (unsigned int xResponse = 0u; xResponse < width - responseAreaWidth + 1u; ++xResponse)
		{
#ifdef OCEAN_DEBUG
			// The response offset for the top-down T-shape is (shapeWidth / 2)x(shapeBandSize + shapeStepSize / 2).<br>

			// x and y location to which the response belongs (for top-down filter)
			const unsigned int debugFrameTopDownX = xResponse + shapeWidth_2_;
			const unsigned int debugFrameTopDownY = yResponse + shapeBandSize_ + shapeStepSize_2_;

			ocean_assert(debugFrameTopDownX < width);
			ocean_assert(debugFrameTopDownY < height);
#endif

			// top left band box
			const uint32_t topLeftSum = topBand_top[0] - topBand_top[shapeBandBoxWidth2 + 0u] - topBand_bottom[0] + topBand_bottom[shapeBandBoxWidth2 + 0u];
			const uint32_t topLeftSquareSum = topBand_top[1] - topBand_top[shapeBandBoxWidth2 + 1u] - topBand_bottom[1] + topBand_bottom[shapeBandBoxWidth2 + 1u];

			// top right band box
			const uint32_t topRightSum = topBand_top[shapeBandBoxOffset2 + 0u] - topBand_top[shapeBandBoxOffset2 + shapeBandBoxWidth2 + 0u] - topBand_bottom[shapeBandBoxOffset2 + 0u] + topBand_bottom[shapeBandBoxOffset2 + shapeBandBoxWidth2 + 0u];
			const uint32_t topRightSquareSum = topBand_top[shapeBandBoxOffset2 + 1u] - topBand_top[shapeBandBoxOffset2 + shapeBandBoxWidth2 + 1u] - topBand_bottom[shapeBandBoxOffset2 + 1u] + topBand_bottom[shapeBandBoxOffset2 + shapeBandBoxWidth2 + 1u];

			const float topMean = float(topLeftSum + topRightSum) * invTwoBandBoxesSize;
			ocean_assert(topMean >= 0.0f && topMean < 256.0f);

			// center
			const uint32_t centerSum = centerBand_top[0] - centerBand_top[shapeWidth_ * 2u + 0u] - centerBand_bottom[0] + centerBand_bottom[shapeWidth_ * 2u + 0u];
			const uint32_t centerSquareSum = centerBand_top[1] - centerBand_top[shapeWidth_ * 2u + 1u] - centerBand_bottom[1] + centerBand_bottom[shapeWidth_ * 2u + 1u];

			const float centerMean = float(centerSum) * invCenterSize;
			ocean_assert(centerMean >= 0.0f && centerMean < 256.0f);

			// bottom left band box
			const uint32_t bottomLeftSum = bottomBand_top[0] - bottomBand_top[shapeBandBoxWidth2 + 0u] - bottomBand_bottom[0] + bottomBand_bottom[shapeBandBoxWidth2 + 0u];
			const uint32_t bottomLeftSquareSum = bottomBand_top[1] - bottomBand_top[shapeBandBoxWidth2 + 1u] - bottomBand_bottom[1] + bottomBand_bottom[shapeBandBoxWidth2 + 1u];

			// bottom right band box
			const uint32_t bottomRightSum = bottomBand_top[shapeBandBoxOffset2 + 0u] - bottomBand_top[shapeBandBoxOffset2 + shapeBandBoxWidth2 + 0u] - bottomBand_bottom[shapeBandBoxOffset2 + 0u] + bottomBand_bottom[shapeBandBoxOffset2 + shapeBandBoxWidth2 + 0u];
			const uint32_t bottomRightSquareSum = bottomBand_top[shapeBandBoxOffset2 + 1u] - bottomBand_top[shapeBandBoxOffset2 + shapeBandBoxWidth2 + 1u] - bottomBand_bottom[shapeBandBoxOffset2 + 1u] + bottomBand_bottom[shapeBandBoxOffset2 + shapeBandBoxWidth2 + 1u];

			const float bottomMean = float(bottomLeftSum + bottomRightSum) * invTwoBandBoxesSize;
			ocean_assert(bottomMean >= 0.0f && bottomMean < 256.0f);

			const float topGradient = centerMean - topMean;
			const float bottomGradient = bottomMean - centerMean;

			const float horizontalResponse = topGradient - bottomGradient;

			constexpr float minimalGradientResponse = 0.5f;

			if (NumericF::abs(topGradient) >= minimalGradientResponse && NumericF::abs(bottomGradient) >= minimalGradientResponse)
			{
				const float centerVariance = float(centerSquareSum * centerSize - centerSum * centerSum) * invCenterSizeSqr;
				ocean_assert(centerVariance >= 0.0f && centerVariance < 256.0f * 256.0f);

				const uint32_t topBottomSum = topLeftSum + topRightSum + bottomLeftSum + bottomRightSum;
				const uint32_t topBottomSquareSum = topLeftSquareSum + topRightSquareSum + bottomLeftSquareSum + bottomRightSquareSum;

				const float topBottomVariance = float(topBottomSquareSum * fourBandBoxesSize - topBottomSum * topBottomSum) * invFourBandBoxesSizeSqr;
				ocean_assert(topBottomVariance >= 0.0f && topBottomVariance < 256.0f * 256.0f);

				if constexpr (tSquaredResponse)
				{
					const float horizontalVariance = (topBottomVariance + centerVariance) * 0.5f;

					const float normalizedHorizontalResponse = std::max(0.0f, NumericF::sqr(horizontalResponse) - horizontalVariance) / std::max(1.0f, horizontalVariance); // using sqr(horizontalResponse) and variance

					*horizontalResponses = NumericF::copySign(normalizedHorizontalResponse, horizontalResponse);
				}
				else
				{
					const float centerDeviation = NumericF::sqrt(centerVariance);
					const float topBottomDeviation = NumericF::sqrt(topBottomVariance);

					const float horizontalDeviation = (topBottomDeviation + centerDeviation) * 0.5f;

					const float normalizedHorizontalResponse = std::max(0.0f, NumericF::abs(horizontalResponse) - horizontalDeviation) / std::max(1.0f, horizontalDeviation); // using abs(horizontalResponse) and deviation

					*horizontalResponses = NumericF::copySign(normalizedHorizontalResponse, horizontalResponse);
				}
			}
			else
			{
				*horizontalResponses = 0.0f;
			}

			topBand_top +=2 ;
			topBand_bottom += 2;

			centerBand_top += 2;
			centerBand_bottom += 2;

			bottomBand_top += 2;
			bottomBand_bottom += 2;

			++horizontalResponses;
		}

		topBand_top += responseAreaWidth * 2u + linedIntegralAndSquaredPaddingElements;
		topBand_bottom += responseAreaWidth * 2u + linedIntegralAndSquaredPaddingElements;

		centerBand_top += responseAreaWidth * 2u + linedIntegralAndSquaredPaddingElements;
		centerBand_bottom += responseAreaWidth * 2u + linedIntegralAndSquaredPaddingElements;

		bottomBand_top += responseAreaWidth * 2u + linedIntegralAndSquaredPaddingElements;
		bottomBand_bottom += responseAreaWidth * 2u + linedIntegralAndSquaredPaddingElements;

		horizontalResponses += horizontalResponsesPaddingElements;
	}

#ifdef OCEAN_DEBUG
	ocean_assert(horizontalResponses == debugHorizontalResponses + (height - responseAreaHeight + 1u) * horizontalResponsesStrideElements);
#endif

	ocean_assert(topBand_top == linedIntegralAndSquared + (height - responseAreaHeight + 1u) * linedIntegralAndSquaredStrideElements);
}

template void OCEAN_CV_DETECTOR_EXPORT ShapeDetector::PatternDetectorGradientVarianceBased::determineHorizontalResponsesF<true>(const uint32_t*, const unsigned int, const unsigned int, float*, const unsigned int, const unsigned int);
template void OCEAN_CV_DETECTOR_EXPORT ShapeDetector::PatternDetectorGradientVarianceBased::determineHorizontalResponsesF<false>(const uint32_t*, const unsigned int, const unsigned int, float*, const unsigned int, const unsigned int);

template <bool tUseSIMD>
void ShapeDetector::PatternDetectorGradientVarianceBased::determineVerticalResponsesI(const uint32_t* linedIntegralAndSquared, const unsigned int width, const unsigned int height, int32_t* verticalResponses, const unsigned int linedIntegralAndSquaredPaddingElements, const unsigned int verticalResponsesPaddingElements)
{
	static_assert(shapeWidth_ >= 1u && shapeWidth_ % 2u == 1u, "Invalid shape width!");
	static_assert(shapeHeight_ >= shapeStepSize_ + 1u, "Invalid shape height!");
	static_assert(shapeStepSize_ >= 1u && shapeStepSize_ % 2u == 1u, "Invalid shape step size!");
	static_assert(shapeBandSize_ >= 1u, "Invalid shape band size!");

	constexpr unsigned int precision = 10u;
	constexpr unsigned int factorPrecision = 1u << precision;

	constexpr unsigned int responseAreaWidth = shapeBandSize_ * 2u + shapeStepSize_;
	constexpr unsigned int responseAreaHeight = shapeHeight_ - shapeStepSize_;

	constexpr unsigned int shapeBandSize2 = shapeBandSize_ * 2u;

	constexpr unsigned int bandBoxSize = shapeBandSize_ * responseAreaHeight;

	constexpr unsigned int twoBandBoxesSize = bandBoxSize * 2u;

	constexpr unsigned int middleSize = responseAreaHeight;

	// as factorPrecision / (twoBandBoxesSize * centerSize) is ~1.4, we need to apply a multiplication + shift (division with power of two) to get a more precise response
	constexpr int32_t invBandBoxMiddleSize_normalization = 64;
	constexpr int32_t invBandBoxMiddleSize_precision = int((factorPrecision * invBandBoxMiddleSize_normalization + (bandBoxSize * middleSize) / 2u) / (bandBoxSize * middleSize));

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
	const uint32x4_t middleSize_u_32x4 = vdupq_n_u32(middleSize);
	const uint32x4_t bandBoxSize_u_32x4 = vdupq_n_u32(bandBoxSize);
	const uint32x4_t twoBandBoxesSize_u_32x4 = vdupq_n_u32(twoBandBoxesSize);

	const uint32x4_t invBandBoxMiddleSize_precision_u_32x4 = vdupq_n_u32(invBandBoxMiddleSize_precision);

	const uint32x4_t constant_1_u_32x4 = vdupq_n_u32(1u);
	const uint32x4_t constant_7_u_32x4 = vdupq_n_u32(7u);
	const uint32x4_t constant_448_u_32x4 = vdupq_n_u32(448u);

	const uint32x4_t minimalGradientResponse_precision_u_32x4 = vdupq_n_u32(factorPrecision / 2u);
#endif

#ifdef OCEAN_INTENSIVE_DEBUG
	constexpr float debugInvBandBoxSize = 1.0f / float(bandBoxSize);

	constexpr float debugInvTwoBandBoxesSizeSqr = 1.0f / float(twoBandBoxesSize * twoBandBoxesSize);

	constexpr float debugInvMiddleSize = 1.0f / float(middleSize);
	constexpr float debugInvMiddleSizeSqr = 1.0f / float(middleSize * middleSize);
#endif

	ocean_assert(linedIntegralAndSquared != nullptr);
	ocean_assert(width >= shapeWidth_ && width >= 20u);
	ocean_assert(height >= shapeHeight_ && height >= 20u);
	ocean_assert(width * height <= 65536u);
	ocean_assert(verticalResponses != nullptr);

#ifdef OCEAN_DEBUG
	const int32_t* const debugVerticalResponses = verticalResponses;
#endif

	const unsigned int coreResponseWidth = width - responseAreaWidth + 1u;
	const unsigned int coreResponseHeight = height - responseAreaHeight + 1u;

	ocean_assert(coreResponseWidth >= 4u);

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
	constexpr unsigned int xResponseSteps = tUseSIMD ? 4u : 1u;
#else
	constexpr unsigned int xResponseSteps = 1u;
#endif

	const unsigned int linedIntegralAndSquaredStrideElements = (width + 1u) * 2u + linedIntegralAndSquaredPaddingElements;
	const unsigned int verticalResponsesStrideElements = width - responseAreaWidth + 1u + verticalResponsesPaddingElements;

	const uint32_t* leftBand_top = linedIntegralAndSquared;
	const uint32_t* leftBand_bottom = linedIntegralAndSquared + linedIntegralAndSquaredStrideElements * responseAreaHeight;

	const uint32_t* middleBand_top = linedIntegralAndSquared + (shapeBandSize_ + shapeStepSize_2_) * 2u;
	const uint32_t* middleBand_bottom = linedIntegralAndSquared + linedIntegralAndSquaredStrideElements * responseAreaHeight + (shapeBandSize_ + shapeStepSize_2_) * 2u;

	const uint32_t* rightBand_top = linedIntegralAndSquared + (shapeBandSize_ + shapeStepSize_) * 2u;
	const uint32_t* rightBand_bottom = linedIntegralAndSquared + linedIntegralAndSquaredStrideElements * responseAreaHeight + (shapeBandSize_ + shapeStepSize_) * 2u;

	for (unsigned int yResponse = 0u; yResponse < coreResponseHeight; ++yResponse)
	{
		ocean_assert_and_suppress_unused((verticalResponses - debugVerticalResponses) % verticalResponsesStrideElements == 0u, verticalResponsesStrideElements);
		ocean_assert((leftBand_top - linedIntegralAndSquared) % linedIntegralAndSquaredStrideElements == 0u);

		for (unsigned int xResponse = 0u; xResponse < coreResponseWidth; xResponse += xResponseSteps)
		{
#ifdef OCEAN_DEBUG
			// The response offset for the top-down T-shape is (shapeBandSize + shapeStepSize_ / 2)x(-shapeStepSize / 2 - 1)

			// x and y location to which the response belongs (for top-down filter)
			const unsigned int debugFrameTopDownX = xResponse + shapeBandSize_ + shapeStepSize_2_;
			const int debugFrameTopDownY = int(yResponse - shapeStepSize_2_ - 1u);

			ocean_assert(debugFrameTopDownX < width);
			ocean_assert(debugFrameTopDownY >= -int(shapeStepSize_2_ + 1u) && debugFrameTopDownY < int(height));
#endif

			if constexpr (xResponseSteps != 1u) // constexpr evaluated at compile time
			{
				if (xResponse + xResponseSteps > coreResponseWidth)
				{
					// the last iteration will not fit
					// so we simply shift x left by some pixels (at most 3) and we will calculate some pixels again

					ocean_assert(xResponse >= xResponseSteps && coreResponseWidth > xResponseSteps);
					const unsigned int newXResponse = coreResponseWidth - xResponseSteps;

					ocean_assert(xResponse > newXResponse);
					const unsigned int offset = xResponse - newXResponse;

					leftBand_top -= 2u * offset;
					leftBand_bottom -= 2u * offset;

					middleBand_top -= 2u * offset;
					middleBand_bottom -= 2u * offset;

					rightBand_top -= 2u * offset;
					rightBand_bottom -= 2u * offset;

					verticalResponses -= offset;

					xResponse = newXResponse;

					// the for loop will stop after this iteration
					ocean_assert(!(xResponse + xResponseSteps < coreResponseWidth));
				}

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

				// left band box
				const uint32x4x2_t leftSumAndSquareSumA_u_32x4x2 = vld2q_u32(leftBand_top);
				const uint32x4x2_t leftSumAndSquareSumB_u_32x4x2 = vld2q_u32(leftBand_top + shapeBandSize2);
				const uint32x4x2_t leftSumAndSquareSumC_u_32x4x2 = vld2q_u32(leftBand_bottom);
				const uint32x4x2_t leftSumAndSquareSumD_u_32x4x2 = vld2q_u32(leftBand_bottom + shapeBandSize2);

				const uint32x4_t leftSum_u_32x4 = vsubq_u32(vaddq_u32(leftSumAndSquareSumA_u_32x4x2.val[0], leftSumAndSquareSumD_u_32x4x2.val[0]), vaddq_u32(leftSumAndSquareSumB_u_32x4x2.val[0], leftSumAndSquareSumC_u_32x4x2.val[0])); // (A + D) - (B + C)
				const uint32x4_t leftSquareSum_u_32x4 = vsubq_u32(vaddq_u32(leftSumAndSquareSumA_u_32x4x2.val[1], leftSumAndSquareSumD_u_32x4x2.val[1]), vaddq_u32(leftSumAndSquareSumB_u_32x4x2.val[1], leftSumAndSquareSumC_u_32x4x2.val[1]));

				// left mean
				const uint32x4_t leftSum_adjusted_u_32x4 = vmulq_u32(leftSum_u_32x4, middleSize_u_32x4); // leftMean * bandBoxSize * middleSize


				// middle
				const uint32x4x2_t middleSumAndSquareSumA_u_32x4x2 = vld2q_u32(middleBand_top);
				const uint32x4x2_t middleSumAndSquareSumB_u_32x4x2 = vld2q_u32(middleBand_top + 2);
				const uint32x4x2_t middleSumAndSquareSumC_u_32x4x2 = vld2q_u32(middleBand_bottom);
				const uint32x4x2_t middleSumAndSquareSumD_u_32x4x2 = vld2q_u32(middleBand_bottom + 2);

				const uint32x4_t middleSum_u_32x4 = vsubq_u32(vaddq_u32(middleSumAndSquareSumA_u_32x4x2.val[0], middleSumAndSquareSumD_u_32x4x2.val[0]), vaddq_u32(middleSumAndSquareSumB_u_32x4x2.val[0], middleSumAndSquareSumC_u_32x4x2.val[0]));
				const uint32x4_t middleSquareSum_u_32x4 = vsubq_u32(vaddq_u32(middleSumAndSquareSumA_u_32x4x2.val[1], middleSumAndSquareSumD_u_32x4x2.val[1]), vaddq_u32(middleSumAndSquareSumB_u_32x4x2.val[1], middleSumAndSquareSumC_u_32x4x2.val[1]));

				// middle mean
				const uint32x4_t middleSum_adjusted_u_32x4 = vmulq_u32(middleSum_u_32x4, bandBoxSize_u_32x4); // middleMean * middleSize * bandBoxSize


				// right band box
				const uint32x4x2_t rightSumAndSquareSumA_u_32x4x2 = vld2q_u32(rightBand_top);
				const uint32x4x2_t rightSumAndSquareSumB_u_32x4x2 = vld2q_u32(rightBand_top + shapeBandSize2);
				const uint32x4x2_t rightSumAndSquareSumC_u_32x4x2 = vld2q_u32(rightBand_bottom);
				const uint32x4x2_t rightSumAndSquareSumD_u_32x4x2 = vld2q_u32(rightBand_bottom + shapeBandSize2);

				const uint32x4_t rightSum_u_32x4 = vsubq_u32(vaddq_u32(rightSumAndSquareSumA_u_32x4x2.val[0], rightSumAndSquareSumD_u_32x4x2.val[0]), vaddq_u32(rightSumAndSquareSumB_u_32x4x2.val[0], rightSumAndSquareSumC_u_32x4x2.val[0]));
				const uint32x4_t rightSquareSum_u_32x4 = vsubq_u32(vaddq_u32(rightSumAndSquareSumA_u_32x4x2.val[1], rightSumAndSquareSumD_u_32x4x2.val[1]), vaddq_u32(rightSumAndSquareSumB_u_32x4x2.val[1], rightSumAndSquareSumC_u_32x4x2.val[1]));

				// right mean
				const uint32x4_t rightSum_adjusted_u_32x4 = vmulq_u32(rightSum_u_32x4, middleSize_u_32x4); // rightMean * bandBoxSize * middleSize


				// gradient

				const int32x4_t leftGradient_precision_s_32x4 = vrshrq_n_s32(vmulq_s32(vreinterpretq_s32_u32(vsubq_u32(middleSum_adjusted_u_32x4, leftSum_adjusted_u_32x4)), invBandBoxMiddleSize_precision_u_32x4), 6); // ((middleSum - leftSum) * invBandMiddleSize_precision_s_32x4) / invBandMiddleSize_normalization_s_32x4
				const int32x4_t rightGradient_precision_s_32x4 = vrshrq_n_s32(vmulq_s32(vreinterpretq_s32_u32(vsubq_u32(rightSum_adjusted_u_32x4, middleSum_adjusted_u_32x4)), invBandBoxMiddleSize_precision_u_32x4), 6); // ((rightSum - middleSum) * invBandMiddleSize_precision_s_32x4) / invBandMiddleSize_normalization_s_32x4

				const int32x4_t verticalResponse_precision_s_32x4 = vsubq_s32(leftGradient_precision_s_32x4, rightGradient_precision_s_32x4);

				const uint32x4_t absLeftGradient_precision_u_32x4 = vreinterpretq_u32_s32(vabsq_s32(leftGradient_precision_s_32x4));
				const uint32x4_t absRightGradient_precision_u_32x4 = vreinterpretq_u32_s32(vabsq_s32(rightGradient_precision_s_32x4));


				// abs(leftGradient) >= minimalGradient && abs(rightGradient) >= minimalGradient
				const uint32x4_t validGradient_u_32x4 = vandq_u32(vcgeq_u32(absLeftGradient_precision_u_32x4, minimalGradientResponse_precision_u_32x4), vcgeq_u32(absRightGradient_precision_u_32x4, minimalGradientResponse_precision_u_32x4));

#ifdef OCEAN_INTENSIVE_DEBUG
				const uint32_t debugLeftSum = leftBand_top[0] - leftBand_top[shapeBandSize2 + 0u] - leftBand_bottom[0] + leftBand_bottom[shapeBandSize2 + 0u];
				const uint32_t debugLeftSquareSum = leftBand_top[1] - leftBand_top[shapeBandSize2 + 1u] - leftBand_bottom[1] + leftBand_bottom[shapeBandSize2 + 1u];
				const uint32_t debugLeftSum_adjusted = debugLeftSum * middleSize;

				const uint32_t debugMiddleSum = middleBand_top[0] - middleBand_top[2u + 0u] - middleBand_bottom[0] + middleBand_bottom[2u + 0u];
				const uint32_t debugMiddleSquareSum = middleBand_top[1] - middleBand_top[2u + 1u] - middleBand_bottom[1] + middleBand_bottom[2u + 1u];
				const uint32_t debugMiddleSum_adjusted = debugMiddleSum * bandBoxSize;

				const uint32_t debugRightSum = rightBand_top[0] - rightBand_top[shapeBandSize2 + 0u] - rightBand_bottom[0] + rightBand_bottom[shapeBandSize2 + 0u];
				const uint32_t debugRightSquareSum = rightBand_top[1] - rightBand_top[shapeBandSize2 + 1u] - rightBand_bottom[1] + rightBand_bottom[shapeBandSize2 + 1u];
				const uint32_t debugRightSum_adjusted = debugRightSum * middleSize;

				const int32_t debugLeftGradient_precision = (int32_t(debugMiddleSum_adjusted - debugLeftSum_adjusted) * invBandBoxMiddleSize_precision) / invBandBoxMiddleSize_normalization;
				const int32_t debugRightGradient_precision = (int32_t(debugRightSum_adjusted - debugMiddleSum_adjusted) * invBandBoxMiddleSize_precision) / invBandBoxMiddleSize_normalization;
				const int32_t debugVerticalResponse_precision = debugLeftGradient_precision - debugRightGradient_precision;

				ocean_assert(NumericT<uint32_t>::isEqual(vgetq_lane_u32(verticalResponse_precision_s_32x4, 0), debugVerticalResponse_precision, 16u));

				constexpr uint32_t debugMinimalGradientResponse_precision = (factorPrecision + 1u) / 2u;
				const bool debugFirstResponseValid = NumericT<int32_t>::secureAbs(debugLeftGradient_precision) >= debugMinimalGradientResponse_precision && NumericT<int32_t>::secureAbs(debugRightGradient_precision) >= debugMinimalGradientResponse_precision;
				OCEAN_SUPPRESS_UNUSED_WARNING(debugFirstResponseValid);
#endif // OCEAN_INTENSIVE_DEBUG

				const uint32x4_t absVerticalResponse_precision_8_u_32x4 = vrshrq_n_u32(vreinterpretq_u32_s32(vabsq_s32(verticalResponse_precision_s_32x4)), 3); // (|verticalRespones| + 4) / 8
				const uint32x4_t sqrVerticalResponse_16384_u_32x4 = vmulq_u32(absVerticalResponse_precision_8_u_32x4, absVerticalResponse_precision_8_u_32x4); // gradient^2 * 16,384 < 2^32

				const uint32x4_t middleVarianceSmallSizeSqr_u_32x4 = vmlsq_u32(vmulq_u32(middleSquareSum_u_32x4, middleSize_u_32x4), middleSum_u_32x4, middleSum_u_32x4); // middleVariance * 144

				const uint32x4_t leftRightSum_u_32x4 = vaddq_u32(leftSum_u_32x4, rightSum_u_32x4);
				const uint32x4_t leftRightSquareSum_u_32x4 = vaddq_u32(leftSquareSum_u_32x4, rightSquareSum_u_32x4);

				const uint32x4_t leftRightVarianceLargeSizeSqr_u_32x4 = vmlsq_u32(vmulq_u32(leftRightSquareSum_u_32x4, twoBandBoxesSize_u_32x4), leftRightSum_u_32x4, leftRightSum_u_32x4); // leftRightVariance * 9,216

				const uint32x4_t leftRightAdjustedVariance_u_32x4 = vrshrq_n_u32(vmulq_u32(leftRightVarianceLargeSizeSqr_u_32x4, constant_7_u_32x4), 2); // (leftRightVarianceLargeSizeSqr * 7 + 2) / 4
				const uint32x4_t middleAdjustedVariance_u_32x4 = vrshrq_n_u32(vmulq_u32(middleVarianceSmallSizeSqr_u_32x4, constant_448_u_32x4), 2); // (middleVarianceSmallSizeSqr * 448 + 2) / 4

				const uint32x4_t verticalVariance_16384_u_32x4 = vrhaddq_u32(leftRightAdjustedVariance_u_32x4, middleAdjustedVariance_u_32x4); // (leftRightAdjustedVariance + middleAdjustedVariance + 1) / 2

#ifdef OCEAN_INTENSIVE_DEBUG

				const uint32_t debugAbsVerticalResponse_precision_8 = (NumericT<int32_t>::secureAbs(debugVerticalResponse_precision) + 4u) / 8u;
				const uint32_t debugSqrVerticalResponse_16384 = debugAbsVerticalResponse_precision_8 * debugAbsVerticalResponse_precision_8;
				const uint32_t debugMiddleVarianceSmallSizeSqr = debugMiddleSquareSum * middleSize - debugMiddleSum * debugMiddleSum;
				const uint32_t debugLeftRightSum = debugLeftSum + debugRightSum;
				const uint32_t debugLeftRightSquareSum = debugLeftSquareSum + debugRightSquareSum;
				const uint32_t debugLeftRightVarianceLargeSizeSqr = debugLeftRightSquareSum * twoBandBoxesSize - debugLeftRightSum * debugLeftRightSum;
				const uint32_t debugVerticalVariance_16384 = ((((debugLeftRightVarianceLargeSizeSqr * 7u) + 2u) / 4u + ((debugMiddleVarianceSmallSizeSqr * 448u) + 2u) / 4u) + 1u) / 2u;

				ocean_assert(vgetq_lane_u32(verticalVariance_16384_u_32x4, 0) == debugVerticalVariance_16384);
#endif // OCEAN_INTENSIVE_DEBUG

				const uint32x4_t responseDifference_u_32x4 = vqsubq_u32(sqrVerticalResponse_16384_u_32x4, verticalVariance_16384_u_32x4); // abs(0, sqrVerticalResponse - verticalVariance_16384)

				const float32x4_t verticalVariance_16384_f_32x4 = vcvtq_f32_u32(vmaxq_u32(constant_1_u_32x4, verticalVariance_16384_u_32x4));

				// we calculate the (approximated) inverse of verticalVariance: 1 / verticalVariance
				float32x4_t invVerticalVariance_16384_f_32x4 = vrecpeq_f32(verticalVariance_16384_f_32x4);
				invVerticalVariance_16384_f_32x4 = vmulq_f32(vrecpsq_f32(verticalVariance_16384_f_32x4, invVerticalVariance_16384_f_32x4), invVerticalVariance_16384_f_32x4); // improving the accuracy of the approx. inverse by Newton/Raphson

				const uint32x4_t verticalResponse_u_32x4 = vandq_u32(validGradient_u_32x4, vcvtq_u32_f32(vmulq_f32(vcvtq_f32_u32(responseDifference_u_32x4), invVerticalVariance_16384_f_32x4)));

				vst1q_s32(verticalResponses, NEON::copySign(verticalResponse_u_32x4, verticalResponse_precision_s_32x4));

				leftBand_top += 2u * xResponseSteps;
				leftBand_bottom += 2u * xResponseSteps;

				middleBand_top += 2u * xResponseSteps;
				middleBand_bottom += 2u * xResponseSteps;

				rightBand_top += 2u * xResponseSteps;
				rightBand_bottom += 2u * xResponseSteps;

				verticalResponses += xResponseSteps;

#endif // defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
			}
			else
			{
				ocean_assert(xResponseSteps == 1u);

				// left band box
				const uint32_t leftSum = leftBand_top[0] - leftBand_top[shapeBandSize2 + 0u] - leftBand_bottom[0] + leftBand_bottom[shapeBandSize2 + 0u];
				const uint32_t leftSquareSum = leftBand_top[1] - leftBand_top[shapeBandSize2 + 1u] - leftBand_bottom[1] + leftBand_bottom[shapeBandSize2 + 1u];

				const uint32_t leftSum_adjusted = leftSum * middleSize; // leftMean * bandBoxSize * middleSize
				ocean_assert(leftSum_adjusted < 256u * bandBoxSize * middleSize);

#ifdef OCEAN_INTENSIVE_DEBUG
				const float debugLeftMean = float(leftSum) * debugInvBandBoxSize;
				ocean_assert(debugLeftMean >= 0.0f && debugLeftMean < 256.0f);
				ocean_assert(NumericF::isEqual(debugLeftMean, float(leftSum_adjusted) / float(bandBoxSize * middleSize), 0.01f));
#endif

				// middle
				const uint32_t middleSum = middleBand_top[0] - middleBand_top[2u + 0u] - middleBand_bottom[0] + middleBand_bottom[2u + 0u];
				const uint32_t middleSquareSum = middleBand_top[1] - middleBand_top[2u + 1u] - middleBand_bottom[1] + middleBand_bottom[2u + 1u];

				const uint32_t middleSum_adjusted = middleSum * bandBoxSize; // middleMean * middleSize * bandBoxSize
				ocean_assert(middleSum_adjusted < 256u * middleSize * bandBoxSize);

#ifdef OCEAN_INTENSIVE_DEBUG
				const float debugMiddleMean = float(middleSum) * debugInvMiddleSize;
				ocean_assert(debugMiddleMean >= 0.0f && debugMiddleMean < 256.0f);
				ocean_assert(NumericF::isEqual(debugMiddleMean, float(middleSum_adjusted) / float(middleSize * bandBoxSize), 0.01f));
#endif

				// right band box
				const uint32_t rightSum = rightBand_top[0] - rightBand_top[shapeBandSize2 + 0u] - rightBand_bottom[0] + rightBand_bottom[shapeBandSize2 + 0u];
				const uint32_t rightSquareSum = rightBand_top[1] - rightBand_top[shapeBandSize2 + 1u] - rightBand_bottom[1] + rightBand_bottom[shapeBandSize2 + 1u];

				const uint32_t rightSum_adjusted = rightSum * middleSize; // rightMean * bandBoxSize * middleSize
				ocean_assert(rightSum_adjusted < 256u * bandBoxSize * middleSize);

#ifdef OCEAN_INTENSIVE_DEBUG
				const float debugRightMean = float(rightSum) * debugInvBandBoxSize;
				ocean_assert(debugRightMean >= 0.0f && debugRightMean < 256.0f);
				ocean_assert(NumericF::isEqual(debugRightMean, float(rightSum_adjusted) / float(bandBoxSize * middleSize), 0.01f));
#endif

				const int32_t leftGradient_precision = (int32_t(middleSum_adjusted - leftSum_adjusted) * invBandBoxMiddleSize_precision) / invBandBoxMiddleSize_normalization;
				const int32_t rightGradient_precision = (int32_t(rightSum_adjusted - middleSum_adjusted) * invBandBoxMiddleSize_precision) / invBandBoxMiddleSize_normalization;
				ocean_assert(leftGradient_precision >= -int(256u * factorPrecision) && leftGradient_precision < int(256u * factorPrecision));
				ocean_assert(rightGradient_precision >= -int(256u * factorPrecision) && rightGradient_precision < int(256u * factorPrecision));

				const int32_t verticalResponse_precision = leftGradient_precision - rightGradient_precision;
				ocean_assert(verticalResponse_precision >= -int(256u * factorPrecision * 2u) && verticalResponse_precision < int(256u * factorPrecision * 2u));

#ifdef OCEAN_INTENSIVE_DEBUG
				const float debugLeftGradient = debugMiddleMean - debugLeftMean;
				const float debugRightGradient = debugRightMean - debugMiddleMean;
				const float debugVerticalResponse = debugLeftGradient - debugRightGradient;
				ocean_assert(NumericF::isEqual(debugVerticalResponse, float(verticalResponse_precision) / float(factorPrecision), 1.0f));
#endif

				constexpr uint32_t minimalGradientResponse_precision = (factorPrecision + 1u) / 2u; // minimalGradientResponse = 0.5f;

				if (NumericT<int32_t>::secureAbs(leftGradient_precision) >= minimalGradientResponse_precision && NumericT<int32_t>::secureAbs(rightGradient_precision) >= minimalGradientResponse_precision)
				{
					static_assert(uint64_t(2u * 255u * factorPrecision) * uint64_t(2u * 255u * factorPrecision) / uint64_t(8u * 8u) <= uint64_t(0xFFFFFFFFu), "Out of range!");

					const uint32_t absVerticalResponse_precision_8 = (NumericT<int32_t>::secureAbs(verticalResponse_precision) + 4u) / 8u;
					ocean_assert(uint64_t(absVerticalResponse_precision_8) * uint64_t(absVerticalResponse_precision_8) <= uint64_t(0xFFFFFFFFu));

					const uint32_t sqrVerticalResponse_16384 = absVerticalResponse_precision_8 * absVerticalResponse_precision_8; // [(gradient * 1,024) / 8] ^ 2 == gradient^2 * 16,384 < 2^32

#ifdef OCEAN_INTENSIVE_DEBUG
					const float debugVerticalResponseInteger = NumericF::sqrt(float(sqrVerticalResponse_16384) / 16384.0f);
					ocean_assert(NumericF::isEqual(NumericF::abs(debugVerticalResponse), debugVerticalResponseInteger, 1.0f));
#endif

					const uint32_t middleVarianceSmallSizeSqr = middleSquareSum * middleSize - middleSum * middleSum; // middleVariance * 12^2 = middleVariance * 144
					ocean_assert(middleVarianceSmallSizeSqr < 256u * 256u * 12u * 12u);

#ifdef OCEAN_INTENSIVE_DEBUG
					const float debugMiddleVariance = float(middleSquareSum * middleSize - middleSum * middleSum) * debugInvMiddleSizeSqr;
					ocean_assert(debugMiddleVariance >= 0.0f && debugMiddleVariance < 256.0f * 256.0f);
					ocean_assert(NumericF::isWeakEqual(debugMiddleVariance, float(middleVarianceSmallSizeSqr) / float(middleSize * middleSize)));
#endif

					const uint32_t leftRightSum = leftSum + rightSum;
					const uint32_t leftRightSquareSum = leftSquareSum + rightSquareSum;

					const uint32_t leftRightVarianceLargeSizeSqr = leftRightSquareSum * twoBandBoxesSize - leftRightSum * leftRightSum; // leftRightVariance * 96^2 = leftRightVariance * 9,216
					ocean_assert(uint64_t(leftRightVarianceLargeSizeSqr) < uint64_t(256u * 256u) * uint64_t(twoBandBoxesSize * twoBandBoxesSize)); // <= 255^2 * 96^2 < 600,000,000

#ifdef OCEAN_INTENSIVE_DEBUG
					const float debugLeftRightVariance = float(leftRightSquareSum * twoBandBoxesSize - leftRightSum * leftRightSum) * debugInvTwoBandBoxesSizeSqr;
					ocean_assert(debugLeftRightVariance >= 0.0f && debugLeftRightVariance < 256.0f * 256.0f);
					ocean_assert(NumericF::isWeakEqual(debugLeftRightVariance, float(leftRightVarianceLargeSizeSqr) / float(twoBandBoxesSize * twoBandBoxesSize)));
#endif

					// getting same (similar) nominator for gradient, centerVariance, and topBottom variance

					// mittleVariance, leftRightVariance
					// 9216 / 144 = 64

					// gradient, variance
					// division should be realized with shifts
					// 16,384 / 2^a  ==  9,216 * b / 2^c, while b <= 7, 7.166994 = 2^32 / (255^2 * 96^2)
					// 1.7777 / 2^a  ==  b / 2^c

					// a = 0, b = 7, c = 2: 7/4 = 1.75

					const uint32_t verticalVariance_16384 = ((((leftRightVarianceLargeSizeSqr * 7u) + 2u) / 4u + ((middleVarianceSmallSizeSqr * 448u) + 2u) / 4u) + 1u) / 2u; // 448 = 7 * 64

#ifdef OCEAN_INTENSIVE_DEBUG
					const float debugVerticalVariance = (debugLeftRightVariance + debugMiddleVariance) * 0.5f;
					const float debugVerticalDeviation = NumericF::sqrt(debugVerticalVariance);
					const float debugVerticalDeviationInteger = NumericF::sqrt(float(verticalVariance_16384) / 16384.0f);
					ocean_assert(NumericF::isEqual(debugVerticalDeviation, debugVerticalDeviationInteger, 1.0f));
#endif

					const uint32_t normalizedSqrVerticalResponse = (uint32_t(std::max(0, int32_t(sqrVerticalResponse_16384 - verticalVariance_16384))) + verticalVariance_16384 / 2u) / std::max(1u, verticalVariance_16384);

#ifdef OCEAN_INTENSIVE_DEBUG
					const float debugNormalizedSqrVerticalResponse = std::max(0.0f, NumericF::sqr(debugVerticalResponse) - debugVerticalVariance) / std::max(1.0f, debugVerticalVariance);
					const float debugNormalizedVerticalResponse = NumericF::sqrt(debugNormalizedSqrVerticalResponse);
					const float debugNormalizedVerticalResponseInteger = NumericF::sqrt(float(normalizedSqrVerticalResponse));
					ocean_assert(debugNormalizedVerticalResponse < 15.0f || NumericF::isEqual(debugNormalizedVerticalResponse, debugNormalizedVerticalResponseInteger, 1.0f));
#endif

					ocean_assert(normalizedSqrVerticalResponse < uint32_t(NumericT<int32_t>::maxValue()));
					*verticalResponses = NumericT<int32_t>::copySign(normalizedSqrVerticalResponse, verticalResponse_precision);
				}
				else
				{
					*verticalResponses = 0;
				}

				leftBand_top += 2;
				leftBand_bottom += 2;

				middleBand_top += 2;
				middleBand_bottom += 2;

				rightBand_top += 2;
				rightBand_bottom += 2;

				++verticalResponses;
			}
		}

		leftBand_top += responseAreaWidth * 2u + linedIntegralAndSquaredPaddingElements;
		leftBand_bottom += responseAreaWidth * 2u + linedIntegralAndSquaredPaddingElements;

		middleBand_top += responseAreaWidth * 2u + linedIntegralAndSquaredPaddingElements;
		middleBand_bottom += responseAreaWidth * 2u + linedIntegralAndSquaredPaddingElements;

		rightBand_top += responseAreaWidth * 2u + linedIntegralAndSquaredPaddingElements;
		rightBand_bottom += responseAreaWidth * 2u + linedIntegralAndSquaredPaddingElements;

		verticalResponses += verticalResponsesPaddingElements;
	}

#ifdef OCEAN_DEBUG
	ocean_assert(verticalResponses == debugVerticalResponses + (height - responseAreaHeight + 1u) * verticalResponsesStrideElements);
#endif

	ocean_assert(leftBand_top == linedIntegralAndSquared + (height - responseAreaHeight + 1u) * linedIntegralAndSquaredStrideElements);
}

template void OCEAN_CV_DETECTOR_EXPORT ShapeDetector::PatternDetectorGradientVarianceBased::determineVerticalResponsesI<true>(const uint32_t*, const unsigned int, const unsigned int, int32_t*, const unsigned int, const unsigned int);
template void OCEAN_CV_DETECTOR_EXPORT ShapeDetector::PatternDetectorGradientVarianceBased::determineVerticalResponsesI<false>(const uint32_t*, const unsigned int, const unsigned int, int32_t*, const unsigned int, const unsigned int);

template <bool tUseSIMD>
void ShapeDetector::PatternDetectorGradientVarianceBased::determineVerticalResponsesI(const uint32_t* linedIntegral, const uint64_t* linedIntegralSquared, const unsigned int width, const unsigned int height, int32_t* verticalResponses, const unsigned int linedIntegralPaddingElements, const unsigned int linedIntegralSquaredPaddingElements, const unsigned int verticalResponsesPaddingElements)
{
	static_assert(shapeWidth_ >= 1u && shapeWidth_ % 2u == 1u, "Invalid shape width!");
	static_assert(shapeHeight_ >= shapeStepSize_ + 1u, "Invalid shape height!");
	static_assert(shapeStepSize_ >= 1u && shapeStepSize_ % 2u == 1u, "Invalid shape step size!");
	static_assert(shapeBandSize_ >= 1u, "Invalid shape band size!");

	constexpr unsigned int precision = 10u;
	constexpr unsigned int factorPrecision = 1u << precision;

	constexpr unsigned int responseAreaWidth = shapeBandSize_ * 2u + shapeStepSize_;
	constexpr unsigned int responseAreaHeight = shapeHeight_ - shapeStepSize_;

	constexpr unsigned int bandBoxSize = shapeBandSize_ * responseAreaHeight;

	constexpr unsigned int twoBandBoxesSize = bandBoxSize * 2u;

	constexpr unsigned int middleSize = responseAreaHeight;

	// as factorPrecision / (twoBandBoxesSize * centerSize) is ~1.4, we need to apply a multiplication + shift (division with power of two) to get a more precise response
	constexpr int32_t invBandBoxMiddleSize_normalization = 64;
	constexpr int32_t invBandBoxMiddleSize_precision = int((factorPrecision * invBandBoxMiddleSize_normalization + (bandBoxSize * middleSize) / 2u) / (bandBoxSize * middleSize));

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
	const uint32x4_t middleSize_u_32x4 = vdupq_n_u32(middleSize);
	const uint32x2_t middleSize_u_32x2 = vdup_n_u32(middleSize);
	const uint32x4_t bandBoxSize_u_32x4 = vdupq_n_u32(bandBoxSize);
	const uint32x2_t twoBandBoxesSize_u_32x2 = vdup_n_u32(twoBandBoxesSize);

	const uint32x4_t invBandBoxMiddleSize_precision_u_32x4 = vdupq_n_u32(invBandBoxMiddleSize_precision);

	const uint32x4_t constant_1_u_32x4 = vdupq_n_u32(1u);
	const uint32x4_t constant_7_u_32x4 = vdupq_n_u32(7u);
	const uint32x4_t constant_448_u_32x4 = vdupq_n_u32(448u);

	const uint32x4_t minimalGradientResponse_precision_u_32x4 = vdupq_n_u32(factorPrecision / 2u);
#endif

#ifdef OCEAN_INTENSIVE_DEBUG
	constexpr float debugInvBandBoxSize = 1.0f / float(bandBoxSize);

	constexpr float debugInvTwoBandBoxesSizeSqr = 1.0f / float(twoBandBoxesSize * twoBandBoxesSize);

	constexpr float debugInvMiddleSize = 1.0f / float(middleSize);
	constexpr float debugInvMiddleSizeSqr = 1.0f / float(middleSize * middleSize);
#endif

	ocean_assert(linedIntegral != nullptr);
	ocean_assert(linedIntegralSquared != nullptr);
	ocean_assert(width >= shapeWidth_ && width >= 20u);
	ocean_assert(height >= shapeHeight_ && height >= 20u);
	ocean_assert(verticalResponses != nullptr);

#ifdef OCEAN_DEBUG
	const int32_t* const debugVerticalResponses = verticalResponses;
#endif

	const unsigned int coreResponseWidth = width - responseAreaWidth + 1u;
	const unsigned int coreResponseHeight = height - responseAreaHeight + 1u;

	ocean_assert(coreResponseWidth >= 4u);

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
	constexpr unsigned int xResponseSteps = tUseSIMD ? 4u : 1u;
#else
	constexpr unsigned int xResponseSteps = 1u;
#endif

	const unsigned int linedIntegralStrideElements = width + 1u + linedIntegralPaddingElements;
	const unsigned int linedIntegralSquaredStrideElements = width + 1u + linedIntegralSquaredPaddingElements;
	const unsigned int verticalResponsesStrideElements = width - responseAreaWidth + 1u + verticalResponsesPaddingElements;

	const uint32_t* leftBand_top = linedIntegral;
	const uint32_t* leftBand_bottom = linedIntegral + linedIntegralStrideElements * responseAreaHeight;
	const uint64_t* leftBandSquared_top = linedIntegralSquared;
	const uint64_t* leftBandSquared_bottom = linedIntegralSquared + linedIntegralSquaredStrideElements * responseAreaHeight;

	const uint32_t* middleBand_top = linedIntegral + shapeBandSize_ + shapeStepSize_2_;
	const uint32_t* middleBand_bottom = linedIntegral + linedIntegralStrideElements * responseAreaHeight + shapeBandSize_ + shapeStepSize_2_;
	const uint64_t* middleBandSquared_top = linedIntegralSquared + shapeBandSize_ + shapeStepSize_2_;
	const uint64_t* middleBandSquared_bottom = linedIntegralSquared + linedIntegralSquaredStrideElements * responseAreaHeight + shapeBandSize_ + shapeStepSize_2_;

	const uint32_t* rightBand_top = linedIntegral + shapeBandSize_ + shapeStepSize_;
	const uint32_t* rightBand_bottom = linedIntegral + linedIntegralStrideElements * responseAreaHeight + shapeBandSize_ + shapeStepSize_;
	const uint64_t* rightBandSquared_top = linedIntegralSquared + shapeBandSize_ + shapeStepSize_;
	const uint64_t* rightBandSquared_bottom = linedIntegralSquared + linedIntegralSquaredStrideElements * responseAreaHeight + shapeBandSize_ + shapeStepSize_;

	for (unsigned int yResponse = 0u; yResponse < coreResponseHeight; ++yResponse)
	{
		ocean_assert_and_suppress_unused((verticalResponses - debugVerticalResponses) % verticalResponsesStrideElements == 0u, verticalResponsesStrideElements);
		ocean_assert((leftBand_top - linedIntegral) % linedIntegralStrideElements == 0u);
		ocean_assert((leftBandSquared_top - linedIntegralSquared) % linedIntegralSquaredStrideElements == 0u);

		for (unsigned int xResponse = 0u; xResponse < coreResponseWidth; xResponse += xResponseSteps)
		{
#ifdef OCEAN_DEBUG
			// The response offset for the top-down T-shape is (shapeBandSize + shapeStepSize_ / 2)x(-shapeStepSize / 2 - 1)

			// x and y location to which the response belongs (for top-down filter)
			const unsigned int debugFrameTopDownX = xResponse + shapeBandSize_ + shapeStepSize_2_;
			const int debugFrameTopDownY = int(yResponse - shapeStepSize_2_ - 1u);

			ocean_assert(debugFrameTopDownX < width);
			ocean_assert(debugFrameTopDownY >= -int(shapeStepSize_2_ + 1u) && debugFrameTopDownY < int(height));
#endif

			if constexpr (xResponseSteps != 1u) // constexpr evaluated at compile time
			{
				if (xResponse + xResponseSteps > coreResponseWidth)
				{
					// the last iteration will not fit
					// so we simply shift x left by some pixels (at most 3) and we will calculate some pixels again

					ocean_assert(xResponse >= xResponseSteps && coreResponseWidth > xResponseSteps);
					const unsigned int newXResponse = coreResponseWidth - xResponseSteps;

					ocean_assert(xResponse > newXResponse);
					const unsigned int offset = xResponse - newXResponse;

					leftBand_top -= offset;
					leftBand_bottom -= offset;
					leftBandSquared_top -= offset;
					leftBandSquared_bottom -= offset;

					middleBand_top -= offset;
					middleBand_bottom -= offset;
					middleBandSquared_top -= offset;
					middleBandSquared_bottom -= offset;

					rightBand_top -= offset;
					rightBand_bottom -= offset;
					rightBandSquared_top -= offset;
					rightBandSquared_bottom -= offset;

					verticalResponses -= offset;

					xResponse = newXResponse;

					// the for loop will stop after this iteration
					ocean_assert(!(xResponse + xResponseSteps < coreResponseWidth));
				}

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

				// left band box
				const uint32x4_t leftSumA_u_32x4x2 = vld1q_u32(leftBand_top);
				const uint32x4_t leftSumB_u_32x4x2 = vld1q_u32(leftBand_top + shapeBandSize_);
				const uint32x4_t leftSumC_u_32x4x2 = vld1q_u32(leftBand_bottom);
				const uint32x4_t leftSumD_u_32x4x2 = vld1q_u32(leftBand_bottom + shapeBandSize_);
				const uint32x4_t leftSum_u_32x4 = vsubq_u32(vaddq_u32(leftSumA_u_32x4x2, leftSumD_u_32x4x2), vaddq_u32(leftSumB_u_32x4x2, leftSumC_u_32x4x2)); // (A + D) - (B + C)

				uint64x2_t leftSquareSumA_u_64x2 = vld1q_u64(leftBandSquared_top);
				uint64x2_t leftSquareSumB_u_64x2 = vld1q_u64(leftBandSquared_top + shapeBandSize_);
				uint64x2_t leftSquareSumC_u_64x2 = vld1q_u64(leftBandSquared_bottom);
				uint64x2_t leftSquareSumD_u_64x2 = vld1q_u64(leftBandSquared_bottom + shapeBandSize_);
				const uint64x2_t leftSquareSum01_u_64x2 = vsubq_u64(vaddq_u64(leftSquareSumA_u_64x2, leftSquareSumD_u_64x2), vaddq_u64(leftSquareSumB_u_64x2, leftSquareSumC_u_64x2));

				leftSquareSumA_u_64x2 = vld1q_u64(leftBandSquared_top + 2);
				leftSquareSumB_u_64x2 = vld1q_u64(leftBandSquared_top + shapeBandSize_ + 2u);
				leftSquareSumC_u_64x2 = vld1q_u64(leftBandSquared_bottom + 2);
				leftSquareSumD_u_64x2 = vld1q_u64(leftBandSquared_bottom + shapeBandSize_ + 2u);
				const uint64x2_t leftSquareSum23_u_64x2 = vsubq_u64(vaddq_u64(leftSquareSumA_u_64x2, leftSquareSumD_u_64x2), vaddq_u64(leftSquareSumB_u_64x2, leftSquareSumC_u_64x2));

				// left mean
				const uint32x4_t leftSum_adjusted_u_32x4 = vmulq_u32(leftSum_u_32x4, middleSize_u_32x4); // leftMean * bandBoxSize * middleSize


				// middle
				const uint32x4_t middleSumA_u_32x4 = vld1q_u32(middleBand_top);
				const uint32x4_t middleSumB_u_32x4 = vld1q_u32(middleBand_top + 1);
				const uint32x4_t middleSumC_u_32x4 = vld1q_u32(middleBand_bottom);
				const uint32x4_t middleSumD_u_32x4 = vld1q_u32(middleBand_bottom + 1);
				const uint32x4_t middleSum_u_32x4 = vsubq_u32(vaddq_u32(middleSumA_u_32x4, middleSumD_u_32x4), vaddq_u32(middleSumB_u_32x4, middleSumC_u_32x4));

				uint64x2_t middleSquareSumA_u_64x2 = vld1q_u64(middleBandSquared_top);
				uint64x2_t middleSquareSumB_u_64x2 = vld1q_u64(middleBandSquared_top + 1);
				uint64x2_t middleSquareSumC_u_64x2 = vld1q_u64(middleBandSquared_bottom);
				uint64x2_t middleSquareSumD_u_64x2 = vld1q_u64(middleBandSquared_bottom + 1);
				const uint64x2_t middleSquareSum01_u_64x2 = vsubq_u64(vaddq_u64(middleSquareSumA_u_64x2, middleSquareSumD_u_64x2), vaddq_u64(middleSquareSumB_u_64x2, middleSquareSumC_u_64x2));

				middleSquareSumA_u_64x2 = vld1q_u64(middleBandSquared_top + 2);
				middleSquareSumB_u_64x2 = vld1q_u64(middleBandSquared_top + 2 + 1);
				middleSquareSumC_u_64x2 = vld1q_u64(middleBandSquared_bottom + 2);
				middleSquareSumD_u_64x2 = vld1q_u64(middleBandSquared_bottom + 2 + 1);
				const uint64x2_t middleSquareSum23_u_64x2 = vsubq_u64(vaddq_u64(middleSquareSumA_u_64x2, middleSquareSumD_u_64x2), vaddq_u64(middleSquareSumB_u_64x2, middleSquareSumC_u_64x2));

				// middle mean
				const uint32x4_t middleSum_adjusted_u_32x4 = vmulq_u32(middleSum_u_32x4, bandBoxSize_u_32x4); // middleMean * middleSize * bandBoxSize


				// right band box
				const uint32x4_t rightSumA_u_32x4 = vld1q_u32(rightBand_top);
				const uint32x4_t rightSumB_u_32x4 = vld1q_u32(rightBand_top + shapeBandSize_);
				const uint32x4_t rightSumC_u_32x4 = vld1q_u32(rightBand_bottom);
				const uint32x4_t rightSumD_u_32x4 = vld1q_u32(rightBand_bottom + shapeBandSize_);
				const uint32x4_t rightSum_u_32x4 = vsubq_u32(vaddq_u32(rightSumA_u_32x4, rightSumD_u_32x4), vaddq_u32(rightSumB_u_32x4, rightSumC_u_32x4));

				uint64x2_t rightSquareSumA_u_64x2 = vld1q_u64(rightBandSquared_top);
				uint64x2_t rightSquareSumB_u_64x2 = vld1q_u64(rightBandSquared_top + shapeBandSize_);
				uint64x2_t rightSquareSumC_u_64x2 = vld1q_u64(rightBandSquared_bottom);
				uint64x2_t rightSquareSumD_u_64x2 = vld1q_u64(rightBandSquared_bottom + shapeBandSize_);
				const uint64x2_t rightSquareSum01_u_64x2 = vsubq_u64(vaddq_u64(rightSquareSumA_u_64x2, rightSquareSumD_u_64x2), vaddq_u64(rightSquareSumB_u_64x2, rightSquareSumC_u_64x2));

				rightSquareSumA_u_64x2 = vld1q_u64(rightBandSquared_top + 2);
				rightSquareSumB_u_64x2 = vld1q_u64(rightBandSquared_top + shapeBandSize_ + 2u);
				rightSquareSumC_u_64x2 = vld1q_u64(rightBandSquared_bottom + 2);
				rightSquareSumD_u_64x2 = vld1q_u64(rightBandSquared_bottom + shapeBandSize_ + 2u);
				const uint64x2_t rightSquareSum23_u_64x2 = vsubq_u64(vaddq_u64(rightSquareSumA_u_64x2, rightSquareSumD_u_64x2), vaddq_u64(rightSquareSumB_u_64x2, rightSquareSumC_u_64x2));

				// right mean
				const uint32x4_t rightSum_adjusted_u_32x4 = vmulq_u32(rightSum_u_32x4, middleSize_u_32x4); // rightMean * bandBoxSize * middleSize


				// gradient

				const int32x4_t leftGradient_precision_s_32x4 = vrshrq_n_s32(vmulq_s32(vreinterpretq_s32_u32(vsubq_u32(middleSum_adjusted_u_32x4, leftSum_adjusted_u_32x4)), invBandBoxMiddleSize_precision_u_32x4), 6); // ((middleSum - leftSum) * invBandMiddleSize_precision_s_32x4) / invBandMiddleSize_normalization_s_32x4
				const int32x4_t rightGradient_precision_s_32x4 = vrshrq_n_s32(vmulq_s32(vreinterpretq_s32_u32(vsubq_u32(rightSum_adjusted_u_32x4, middleSum_adjusted_u_32x4)), invBandBoxMiddleSize_precision_u_32x4), 6); // ((rightSum - middleSum) * invBandMiddleSize_precision_s_32x4) / invBandMiddleSize_normalization_s_32x4

				const int32x4_t verticalResponse_precision_s_32x4 = vsubq_s32(leftGradient_precision_s_32x4, rightGradient_precision_s_32x4);

				const uint32x4_t absLeftGradient_precision_u_32x4 = vreinterpretq_u32_s32(vabsq_s32(leftGradient_precision_s_32x4));
				const uint32x4_t absRightGradient_precision_u_32x4 = vreinterpretq_u32_s32(vabsq_s32(rightGradient_precision_s_32x4));


				// abs(leftGradient) >= minimalGradient && abs(rightGradient) >= minimalGradient
				const uint32x4_t validGradient_u_32x4 = vandq_u32(vcgeq_u32(absLeftGradient_precision_u_32x4, minimalGradientResponse_precision_u_32x4), vcgeq_u32(absRightGradient_precision_u_32x4, minimalGradientResponse_precision_u_32x4));

#ifdef OCEAN_INTENSIVE_DEBUG
				const uint32_t debugLeftSum = leftBand_top[0] - leftBand_top[shapeBandSize_] - leftBand_bottom[0] + leftBand_bottom[shapeBandSize_];
				const uint64_t debugLeftSquareSum = leftBandSquared_top[0] - leftBandSquared_top[shapeBandSize_] - leftBandSquared_bottom[0] + leftBandSquared_bottom[shapeBandSize_];
				const uint32_t debugLeftSum_adjusted = debugLeftSum * middleSize;

				const uint32_t debugMiddleSum = middleBand_top[0] - middleBand_top[1] - middleBand_bottom[0] + middleBand_bottom[1];
				const uint64_t debugMiddleSquareSum = middleBandSquared_top[0] - middleBandSquared_top[1] - middleBandSquared_bottom[0] + middleBandSquared_bottom[1];
				const uint32_t debugMiddleSum_adjusted = debugMiddleSum * bandBoxSize;

				const uint32_t debugRightSum = rightBand_top[0] - rightBand_top[shapeBandSize_] - rightBand_bottom[0] + rightBand_bottom[shapeBandSize_];
				const uint64_t debugRightSquareSum = rightBandSquared_top[0] - rightBandSquared_top[shapeBandSize_] - rightBandSquared_bottom[0] + rightBandSquared_bottom[shapeBandSize_];
				const uint32_t debugRightSum_adjusted = debugRightSum * middleSize;

				const int32_t debugLeftGradient_precision = (int32_t(debugMiddleSum_adjusted - debugLeftSum_adjusted) * invBandBoxMiddleSize_precision) / invBandBoxMiddleSize_normalization;
				const int32_t debugRightGradient_precision = (int32_t(debugRightSum_adjusted - debugMiddleSum_adjusted) * invBandBoxMiddleSize_precision) / invBandBoxMiddleSize_normalization;
				const int32_t debugVerticalResponse_precision = debugLeftGradient_precision - debugRightGradient_precision;

				ocean_assert(NumericT<uint32_t>::isEqual(vgetq_lane_u32(verticalResponse_precision_s_32x4, 0), debugVerticalResponse_precision, 16u));

				constexpr uint32_t debugMinimalGradientResponse_precision = (factorPrecision + 1u) / 2u;
				const bool debugFirstResponseValid = (sign == 0 || NumericT<int32_t>::sign(debugVerticalResponse_precision) == sign) && NumericT<int32_t>::secureAbs(debugLeftGradient_precision) >= debugMinimalGradientResponse_precision && NumericT<int32_t>::secureAbs(debugRightGradient_precision) >= debugMinimalGradientResponse_precision;
				OCEAN_SUPPRESS_UNUSED_WARNING(debugFirstResponseValid);
#endif // OCEAN_INTENSIVE_DEBUG

				const uint32x4_t absVerticalResponse_precision_8_u_32x4 = vrshrq_n_u32(vreinterpretq_u32_s32(vabsq_s32(verticalResponse_precision_s_32x4)), 3); // (|verticalRespones| + 4) / 8
				const uint32x4_t sqrVerticalResponse_16384_u_32x4 = vmulq_u32(absVerticalResponse_precision_8_u_32x4, absVerticalResponse_precision_8_u_32x4); // gradient^2 * 16,384 < 2^32

				const uint32x2_t middleSum01_u_32x2 = vget_low_u32(middleSum_u_32x4);
				const uint32x2_t middleSum23_u_32x2 = vget_high_u32(middleSum_u_32x4);

				const uint64x2_t middleVarianceSmallSizeSqr01_u_64x2 = vmlsl_u32(NEON::multiply(middleSquareSum01_u_64x2, middleSize_u_32x2), middleSum01_u_32x2, middleSum01_u_32x2); // middleVariance * 144
				const uint64x2_t middleVarianceSmallSizeSqr23_u_64x2 = vmlsl_u32(NEON::multiply(middleSquareSum23_u_64x2, middleSize_u_32x2), middleSum23_u_32x2, middleSum23_u_32x2);
				const uint32x4_t middleVarianceSmallSizeSqr_u_32x4 = vcombine_u32(vmovn_u64(middleVarianceSmallSizeSqr01_u_64x2), vmovn_u64(middleVarianceSmallSizeSqr23_u_64x2));

				const uint32x4_t leftRightSum_u_32x4 = vaddq_u32(leftSum_u_32x4, rightSum_u_32x4);
				const uint64x2_t leftRightSquareSum01_u_64x2 = vaddq_u64(leftSquareSum01_u_64x2, rightSquareSum01_u_64x2);
				const uint64x2_t leftRightSquareSum23_u_64x2 = vaddq_u64(leftSquareSum23_u_64x2, rightSquareSum23_u_64x2);

				const uint32x2_t leftRightSum01_u_32x2 = vget_low_u32(leftRightSum_u_32x4);
				const uint32x2_t leftRightSum23_u_32x2 = vget_high_u32(leftRightSum_u_32x4);

				const uint64x2_t leftRightVarianceLargeSizeSqr01_u_64x2 = vmlsl_u32(NEON::multiply(leftRightSquareSum01_u_64x2, twoBandBoxesSize_u_32x2), leftRightSum01_u_32x2, leftRightSum01_u_32x2); // leftRightVariance * 9,216
				const uint64x2_t leftRightVarianceLargeSizeSqr23_u_64x2 = vmlsl_u32(NEON::multiply(leftRightSquareSum23_u_64x2, twoBandBoxesSize_u_32x2), leftRightSum23_u_32x2, leftRightSum23_u_32x2);
				const uint32x4_t leftRightVarianceLargeSizeSqr_u_32x4 = vcombine_u32(vmovn_u64(leftRightVarianceLargeSizeSqr01_u_64x2), vmovn_u64(leftRightVarianceLargeSizeSqr23_u_64x2));

				const uint32x4_t leftRightAdjustedVariance_u_32x4 = vrshrq_n_u32(vmulq_u32(leftRightVarianceLargeSizeSqr_u_32x4, constant_7_u_32x4), 2); // (leftRightVarianceLargeSizeSqr * 7 + 2) / 4
				const uint32x4_t middleAdjustedVariance_u_32x4 = vrshrq_n_u32(vmulq_u32(middleVarianceSmallSizeSqr_u_32x4, constant_448_u_32x4), 2); // (middleVarianceSmallSizeSqr * 448 + 2) / 4

				const uint32x4_t verticalVariance_16384_u_32x4 = vrhaddq_u32(leftRightAdjustedVariance_u_32x4, middleAdjustedVariance_u_32x4); // (leftRightAdjustedVariance + middleAdjustedVariance + 1) / 2

#ifdef OCEAN_INTENSIVE_DEBUG

				const uint32_t debugAbsVerticalResponse_precision_8 = (NumericT<int32_t>::secureAbs(debugVerticalResponse_precision) + 4u) / 8u;
				const uint32_t debugSqrVerticalResponse_16384 = debugAbsVerticalResponse_precision_8 * debugAbsVerticalResponse_precision_8;
				const uint32_t debugMiddleVarianceSmallSizeSqr = uint32_t(debugMiddleSquareSum * uint64_t(middleSize) - uint64_t(debugMiddleSum * debugMiddleSum));
				const uint32_t debugLeftRightSum = debugLeftSum + debugRightSum;
				const uint64_t debugLeftRightSquareSum = debugLeftSquareSum + debugRightSquareSum;
				const uint32_t debugLeftRightVarianceLargeSizeSqr = uint32_t(debugLeftRightSquareSum * uint64_t(twoBandBoxesSize) - uint64_t(debugLeftRightSum * debugLeftRightSum));
				const uint32_t debugVerticalVariance_16384 = ((((debugLeftRightVarianceLargeSizeSqr * 7u) + 2u) / 4u + ((debugMiddleVarianceSmallSizeSqr * 448u) + 2u) / 4u) + 1u) / 2u;

				ocean_assert(vgetq_lane_u32(verticalVariance_16384_u_32x4, 0) == debugVerticalVariance_16384);
#endif // OCEAN_INTENSIVE_DEBUG

				const uint32x4_t responseDifference_u_32x4 = vqsubq_u32(sqrVerticalResponse_16384_u_32x4, verticalVariance_16384_u_32x4); // abs(0, sqrVerticalResponse - verticalVariance_16384)

				const float32x4_t verticalVariance_16384_f_32x4 = vcvtq_f32_u32(vmaxq_u32(constant_1_u_32x4, verticalVariance_16384_u_32x4));

				// we calculate the (approximated) inverse of verticalVariance: 1 / verticalVariance
				float32x4_t invVerticalVariance_16384_f_32x4 = vrecpeq_f32(verticalVariance_16384_f_32x4);
				invVerticalVariance_16384_f_32x4 = vmulq_f32(vrecpsq_f32(verticalVariance_16384_f_32x4, invVerticalVariance_16384_f_32x4), invVerticalVariance_16384_f_32x4); // improving the accuracy of the approx. inverse by Newton/Raphson

				const uint32x4_t verticalResponse_u_32x4 = vandq_u32(validGradient_u_32x4, vcvtq_u32_f32(vmulq_f32(vcvtq_f32_u32(responseDifference_u_32x4), invVerticalVariance_16384_f_32x4)));

				vst1q_s32(verticalResponses, NEON::copySign(verticalResponse_u_32x4, verticalResponse_precision_s_32x4));

				leftBand_top += xResponseSteps;
				leftBand_bottom += xResponseSteps;
				leftBandSquared_top += xResponseSteps;
				leftBandSquared_bottom += xResponseSteps;

				middleBand_top += xResponseSteps;
				middleBand_bottom += xResponseSteps;
				middleBandSquared_top += xResponseSteps;
				middleBandSquared_bottom += xResponseSteps;

				rightBand_top += xResponseSteps;
				rightBand_bottom += xResponseSteps;
				rightBandSquared_top += xResponseSteps;
				rightBandSquared_bottom += xResponseSteps;

				verticalResponses += xResponseSteps;

#endif // defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
			}
			else
			{
				ocean_assert(xResponseSteps == 1u);

				// left band box
				const uint32_t leftSum = leftBand_top[0] - leftBand_top[shapeBandSize_] - leftBand_bottom[0] + leftBand_bottom[shapeBandSize_];
				const uint64_t leftSquareSum = leftBandSquared_top[0] - leftBandSquared_top[shapeBandSize_] - leftBandSquared_bottom[0] + leftBandSquared_bottom[shapeBandSize_];

				const uint32_t leftSum_adjusted = leftSum * middleSize; // leftMean * bandBoxSize * middleSize
				ocean_assert(leftSum_adjusted < 256u * bandBoxSize * middleSize);

#ifdef OCEAN_INTENSIVE_DEBUG
				const float debugLeftMean = float(leftSum) * debugInvBandBoxSize;
				ocean_assert(debugLeftMean >= 0.0f && debugLeftMean < 256.0f);
				ocean_assert(NumericF::isEqual(debugLeftMean, float(leftSum_adjusted) / float(bandBoxSize * middleSize), 0.01f));
#endif

				// middle
				const uint32_t middleSum = middleBand_top[0] - middleBand_top[1] - middleBand_bottom[0] + middleBand_bottom[1];
				const uint64_t middleSquareSum = middleBandSquared_top[0] - middleBandSquared_top[1] - middleBandSquared_bottom[0] + middleBandSquared_bottom[1];

				const uint32_t middleSum_adjusted = middleSum * bandBoxSize; // middleMean * middleSize * bandBoxSize
				ocean_assert(middleSum_adjusted < 256u * middleSize * bandBoxSize);

#ifdef OCEAN_INTENSIVE_DEBUG
				const float debugMiddleMean = float(middleSum) * debugInvMiddleSize;
				ocean_assert(debugMiddleMean >= 0.0f && debugMiddleMean < 256.0f);
				ocean_assert(NumericF::isEqual(debugMiddleMean, float(middleSum_adjusted) / float(middleSize * bandBoxSize), 0.01f));
#endif

				// right band box
				const uint32_t rightSum = rightBand_top[0] - rightBand_top[shapeBandSize_] - rightBand_bottom[0] + rightBand_bottom[shapeBandSize_];
				const uint64_t rightSquareSum = rightBandSquared_top[0] - rightBandSquared_top[shapeBandSize_] - rightBandSquared_bottom[0] + rightBandSquared_bottom[shapeBandSize_];

				const uint32_t rightSum_adjusted = rightSum * middleSize; // rightMean * bandBoxSize * middleSize
				ocean_assert(rightSum_adjusted < 256u * bandBoxSize * middleSize);

#ifdef OCEAN_INTENSIVE_DEBUG
				const float debugRightMean = float(rightSum) * debugInvBandBoxSize;
				ocean_assert(debugRightMean >= 0.0f && debugRightMean < 256.0f);
				ocean_assert(NumericF::isEqual(debugRightMean, float(rightSum_adjusted) / float(bandBoxSize * middleSize), 0.01f));
#endif

				const int32_t leftGradient_precision = (int32_t(middleSum_adjusted - leftSum_adjusted) * invBandBoxMiddleSize_precision) / invBandBoxMiddleSize_normalization;
				const int32_t rightGradient_precision = (int32_t(rightSum_adjusted - middleSum_adjusted) * invBandBoxMiddleSize_precision) / invBandBoxMiddleSize_normalization;
				ocean_assert(leftGradient_precision >= -int(256u * factorPrecision) && leftGradient_precision < int(256u * factorPrecision));
				ocean_assert(rightGradient_precision >= -int(256u * factorPrecision) && rightGradient_precision < int(256u * factorPrecision));

				const int32_t verticalResponse_precision = leftGradient_precision - rightGradient_precision;
				ocean_assert(verticalResponse_precision >= -int(256u * factorPrecision * 2u) && verticalResponse_precision < int(256u * factorPrecision * 2u));

#ifdef OCEAN_INTENSIVE_DEBUG
				const float debugLeftGradient = debugMiddleMean - debugLeftMean;
				const float debugRightGradient = debugRightMean - debugMiddleMean;
				const float debugVerticalResponse = debugLeftGradient - debugRightGradient;
				ocean_assert(NumericF::isEqual(debugVerticalResponse, float(verticalResponse_precision) / float(factorPrecision), 1.0f));
#endif

				constexpr uint32_t minimalGradientResponse_precision = (factorPrecision + 1u) / 2u; // minimalGradientResponse = 0.5f;

				if (NumericT<int32_t>::secureAbs(leftGradient_precision) >= minimalGradientResponse_precision && NumericT<int32_t>::secureAbs(rightGradient_precision) >= minimalGradientResponse_precision)
				{
					static_assert(uint64_t(2u * 255u * factorPrecision) * uint64_t(2u * 255u * factorPrecision) / uint64_t(8u * 8u) <= uint64_t(0xFFFFFFFFu), "Out of range!");

					const uint32_t absVerticalResponse_precision_8 = (NumericT<int32_t>::secureAbs(verticalResponse_precision) + 4u) / 8u;
					ocean_assert(uint64_t(absVerticalResponse_precision_8) * uint64_t(absVerticalResponse_precision_8) <= uint64_t(0xFFFFFFFFu));

					const uint32_t sqrVerticalResponse_16384 = absVerticalResponse_precision_8 * absVerticalResponse_precision_8; // [(gradient * 1,024) / 8] ^ 2 == gradient^2 * 16,384 < 2^32

#ifdef OCEAN_INTENSIVE_DEBUG
					const float debugVerticalResponseInteger = NumericF::sqrt(float(sqrVerticalResponse_16384) / 16384.0f);
					ocean_assert(NumericF::isEqual(NumericF::abs(debugVerticalResponse), debugVerticalResponseInteger, 1.0f));
#endif

					const uint32_t middleVarianceSmallSizeSqr = uint32_t(middleSquareSum * uint64_t(middleSize) - uint64_t(middleSum * middleSum)); // middleVariance * 12^2 = middleVariance * 144
					ocean_assert(middleVarianceSmallSizeSqr < 256u * 256u * 12u * 12u);

#ifdef OCEAN_INTENSIVE_DEBUG
					const float debugMiddleVariance = float(middleSquareSum * middleSize - middleSum * middleSum) * debugInvMiddleSizeSqr;
					ocean_assert(debugMiddleVariance >= 0.0f && debugMiddleVariance < 256.0f * 256.0f);
					ocean_assert(NumericF::isWeakEqual(debugMiddleVariance, float(middleVarianceSmallSizeSqr) / float(middleSize * middleSize)));
#endif

					const uint32_t leftRightSum = leftSum + rightSum;
					const uint64_t leftRightSquareSum = leftSquareSum + rightSquareSum;

					const uint32_t leftRightVarianceLargeSizeSqr = uint32_t(leftRightSquareSum * uint64_t(twoBandBoxesSize) - uint64_t(leftRightSum * leftRightSum)); // leftRightVariance * 96^2 = leftRightVariance * 9,216
					ocean_assert(uint64_t(leftRightVarianceLargeSizeSqr) < uint64_t(256u * 256u) * uint64_t(twoBandBoxesSize * twoBandBoxesSize)); // <= 255^2 * 96^2 < 600,000,000

#ifdef OCEAN_INTENSIVE_DEBUG
					const float debugLeftRightVariance = float(leftRightSquareSum * twoBandBoxesSize - leftRightSum * leftRightSum) * debugInvTwoBandBoxesSizeSqr;
					ocean_assert(debugLeftRightVariance >= 0.0f && debugLeftRightVariance < 256.0f * 256.0f);
					ocean_assert(NumericF::isWeakEqual(debugLeftRightVariance, float(leftRightVarianceLargeSizeSqr) / float(twoBandBoxesSize * twoBandBoxesSize)));
#endif

					// getting same (similar) nominator for gradient, centerVariance, and topBottom variance

					// mittleVariance, leftRightVariance
					// 9216 / 144 = 64

					// gradient, variance
					// division should be realized with shifts
					// 16,384 / 2^a  ==  9,216 * b / 2^c, while b <= 7, 7.166994 = 2^32 / (255^2 * 96^2)
					// 1.7777 / 2^a  ==  b / 2^c

					// a = 0, b = 7, c = 2: 7/4 = 1.75

					const uint32_t verticalVariance_16384 = ((((leftRightVarianceLargeSizeSqr * 7u) + 2u) / 4u + ((middleVarianceSmallSizeSqr * 448u) + 2u) / 4u) + 1u) / 2u; // 448 = 7 * 64

#ifdef OCEAN_INTENSIVE_DEBUG
					const float debugVerticalVariance = (debugLeftRightVariance + debugMiddleVariance) * 0.5f;
					const float debugVerticalDeviation = NumericF::sqrt(debugVerticalVariance);
					const float debugVerticalDeviationInteger = NumericF::sqrt(float(verticalVariance_16384) / 16384.0f);
					ocean_assert(NumericF::isEqual(debugVerticalDeviation, debugVerticalDeviationInteger, 1.0f));
#endif

					const uint32_t normalizedSqrVerticalResponse = (uint32_t(std::max(0, int32_t(sqrVerticalResponse_16384 - verticalVariance_16384))) + verticalVariance_16384 / 2u) / std::max(1u, verticalVariance_16384);

#ifdef OCEAN_INTENSIVE_DEBUG
					const float debugNormalizedSqrVerticalResponse = std::max(0.0f, NumericF::sqr(debugVerticalResponse) - debugVerticalVariance) / std::max(1.0f, debugVerticalVariance);
					const float debugNormalizedVerticalResponse = NumericF::sqrt(debugNormalizedSqrVerticalResponse);
					const float debugNormalizedVerticalResponseInteger = NumericF::sqrt(float(normalizedSqrVerticalResponse));
					ocean_assert(debugNormalizedVerticalResponse < 15.0f || NumericF::isEqual(debugNormalizedVerticalResponse, debugNormalizedVerticalResponseInteger, 1.0f));
#endif

					ocean_assert(normalizedSqrVerticalResponse < uint32_t(NumericT<int32_t>::maxValue()));
					*verticalResponses = NumericT<int32_t>::copySign(normalizedSqrVerticalResponse, verticalResponse_precision);
				}
				else
				{
					*verticalResponses = 0;
				}

				++leftBand_top;
				++leftBand_bottom;
				++leftBandSquared_top;
				++leftBandSquared_bottom;

				++middleBand_top;
				++middleBand_bottom;
				++middleBandSquared_top;
				++middleBandSquared_bottom;

				++rightBand_top;
				++rightBand_bottom;
				++rightBandSquared_top;
				++rightBandSquared_bottom;

				++verticalResponses;
			}
		}

		leftBand_top += responseAreaWidth + linedIntegralPaddingElements;
		leftBand_bottom += responseAreaWidth + linedIntegralPaddingElements;
		leftBandSquared_top += responseAreaWidth + linedIntegralSquaredPaddingElements;
		leftBandSquared_bottom += responseAreaWidth + linedIntegralSquaredPaddingElements;

		middleBand_top += responseAreaWidth + linedIntegralPaddingElements;
		middleBand_bottom += responseAreaWidth + linedIntegralPaddingElements;
		middleBandSquared_top += responseAreaWidth + linedIntegralSquaredPaddingElements;
		middleBandSquared_bottom += responseAreaWidth + linedIntegralSquaredPaddingElements;

		rightBand_top += responseAreaWidth + linedIntegralPaddingElements;
		rightBand_bottom += responseAreaWidth + linedIntegralPaddingElements;
		rightBandSquared_top += responseAreaWidth + linedIntegralSquaredPaddingElements;
		rightBandSquared_bottom += responseAreaWidth + linedIntegralSquaredPaddingElements;

		verticalResponses += verticalResponsesPaddingElements;
	}

#ifdef OCEAN_DEBUG
	ocean_assert(verticalResponses == debugVerticalResponses + (height - responseAreaHeight + 1u) * verticalResponsesStrideElements);
#endif

	ocean_assert(leftBand_top == linedIntegral + (height - responseAreaHeight + 1u) * linedIntegralStrideElements);
	ocean_assert(leftBandSquared_top == linedIntegralSquared + (height - responseAreaHeight + 1u) * linedIntegralSquaredStrideElements);
}

template void OCEAN_CV_DETECTOR_EXPORT ShapeDetector::PatternDetectorGradientVarianceBased::determineVerticalResponsesI<true>(const uint32_t*, const uint64_t*, const unsigned int, const unsigned int, int32_t*, const unsigned int, const unsigned int, const unsigned int);
template void OCEAN_CV_DETECTOR_EXPORT ShapeDetector::PatternDetectorGradientVarianceBased::determineVerticalResponsesI<false>(const uint32_t*, const uint64_t*, const unsigned int, const unsigned int, int32_t*, const unsigned int, const unsigned int, const unsigned int);

template <bool tSquaredResponse>
void ShapeDetector::PatternDetectorGradientVarianceBased::determineVerticalResponsesF(const uint32_t* linedIntegralAndSquared, const unsigned int width, const unsigned int height, float* verticalResponses, const unsigned int linedIntegralAndSquaredPaddingElements, const unsigned int verticalResponsesPaddingElements)
{
	static_assert(shapeWidth_ >= 1u && shapeWidth_ % 2u == 1u, "Invalid shape width!");
	static_assert(shapeHeight_ >= shapeStepSize_ + 1u, "Invalid shape height!");
	static_assert(shapeStepSize_ >= 1u && shapeStepSize_ % 2u == 1u, "Invalid shape step size!");
	static_assert(shapeBandSize_ >= 1u, "Invalid shape band size!");

	constexpr unsigned int responseAreaWidth = shapeBandSize_ * 2u + shapeStepSize_;
	constexpr unsigned int responseAreaHeight = shapeHeight_ - shapeStepSize_;

	constexpr unsigned int shapeBandSize2 = shapeBandSize_ * 2u;

	constexpr unsigned int bandBoxSize = shapeBandSize_ * responseAreaHeight;
	constexpr float invBandBoxSize = 1.0f / float(bandBoxSize);

	constexpr unsigned int twoBandBoxesSize = bandBoxSize * 2u;
	constexpr float invTwoBandBoxesSizeSqr = 1.0f / float(twoBandBoxesSize * twoBandBoxesSize);

	constexpr unsigned int middleSize = responseAreaHeight;
	constexpr float invMiddleSize = 1.0f / float(middleSize);
	constexpr float invMiddleSizeSqr = 1.0f / float(middleSize * middleSize);

	ocean_assert(linedIntegralAndSquared != nullptr);
	ocean_assert(width >= shapeWidth_ && width >= 20u);
	ocean_assert(height >= shapeHeight_ && height >= 20u);
	ocean_assert(width * height <= 65536u);
	ocean_assert(verticalResponses != nullptr);

#ifdef OCEAN_DEBUG
	const float* const debugVerticalResponses = verticalResponses;
#endif

	const unsigned int linedIntegralAndSquaredStrideElements = (width + 1u) * 2u + linedIntegralAndSquaredPaddingElements;
	const unsigned int verticalResponsesStrideElements = width - responseAreaWidth + 1u + verticalResponsesPaddingElements;

	const uint32_t* leftBand_top = linedIntegralAndSquared;
	const uint32_t* leftBand_bottom = linedIntegralAndSquared + linedIntegralAndSquaredStrideElements * responseAreaHeight;

	const uint32_t* middleBand_top = linedIntegralAndSquared + (shapeBandSize_ + shapeStepSize_2_) * 2u;
	const uint32_t* middleBand_bottom = linedIntegralAndSquared + linedIntegralAndSquaredStrideElements * responseAreaHeight + (shapeBandSize_ + shapeStepSize_2_) * 2u;

	const uint32_t* rightBand_top = linedIntegralAndSquared + (shapeBandSize_ + shapeStepSize_) * 2u;
	const uint32_t* rightBand_bottom = linedIntegralAndSquared + linedIntegralAndSquaredStrideElements * responseAreaHeight + (shapeBandSize_ + shapeStepSize_) * 2u;

	for (unsigned int yResponse = 0u; yResponse < height - responseAreaHeight + 1u; ++yResponse)
	{
		ocean_assert_and_suppress_unused((verticalResponses - debugVerticalResponses) % verticalResponsesStrideElements == 0u, verticalResponsesStrideElements);
		ocean_assert((leftBand_top - linedIntegralAndSquared) % linedIntegralAndSquaredStrideElements == 0u);

		for (unsigned int xResponse = 0u; xResponse < width - responseAreaWidth + 1u; ++xResponse)
		{
#ifdef OCEAN_DEBUG
			// The response offset for the top-down T-shape is (shapeBandSize + shapeStepSize_ / 2)x(-shapeStepSize / 2 - 1)

			// x and y location to which the response belongs (for top-down filter)
			const unsigned int debugFrameTopDownX = xResponse + shapeBandSize_ + shapeStepSize_2_;
			const int debugFrameTopDownY = int(yResponse - shapeStepSize_2_ - 1u);

			ocean_assert(debugFrameTopDownX < width);
			ocean_assert(debugFrameTopDownY >= -int(shapeStepSize_2_ + 1u) && debugFrameTopDownY < int(height));
#endif

			// left band box
			const uint32_t leftSum = leftBand_top[0] - leftBand_top[shapeBandSize2 + 0u] - leftBand_bottom[0] + leftBand_bottom[shapeBandSize2 + 0u];
			const uint32_t leftSquareSum = leftBand_top[1] - leftBand_top[shapeBandSize2 + 1u] - leftBand_bottom[1] + leftBand_bottom[shapeBandSize2 + 1u];

			const float leftMean = float(leftSum) * invBandBoxSize;
			ocean_assert(leftMean >= 0.0f && leftMean < 256.0f);

			// middle
			const uint32_t middleSum = middleBand_top[0] - middleBand_top[2u + 0u] - middleBand_bottom[0] + middleBand_bottom[2u + 0u];
			const uint32_t middleSquareSum = middleBand_top[1] - middleBand_top[2u + 1u] - middleBand_bottom[1] + middleBand_bottom[2u + 1u];

			const float middleMean = float(middleSum) * invMiddleSize;
			ocean_assert(middleMean >= 0.0f && middleMean < 256.0f);

			// right band box
			const uint32_t rightSum = rightBand_top[0] - rightBand_top[shapeBandSize2 + 0u] - rightBand_bottom[0] + rightBand_bottom[shapeBandSize2 + 0u];
			const uint32_t rightSquareSum = rightBand_top[1] - rightBand_top[shapeBandSize2 + 1u] - rightBand_bottom[1] + rightBand_bottom[shapeBandSize2 + 1u];

			const float rightMean = float(rightSum) * invBandBoxSize;
			ocean_assert(rightMean >= 0.0f && rightMean < 256.0f);

			const float leftGradient = middleMean - leftMean;
			const float rightGradient = rightMean - middleMean;

			const float verticalResponse = leftGradient - rightGradient;

			constexpr float minimalGradientResponse = 0.5f;

			if (NumericF::abs(leftGradient) >= minimalGradientResponse && NumericF::abs(rightGradient) >= minimalGradientResponse)
			{
				const float middleVariance = float(middleSquareSum * middleSize - middleSum * middleSum) * invMiddleSizeSqr;
				ocean_assert(middleVariance >= 0.0f && middleVariance < 256.0f * 256.0f);

				const uint32_t leftRightSum = leftSum + rightSum;
				const uint32_t leftRightSquareSum = leftSquareSum + rightSquareSum;

				const float leftRightVariance = float(leftRightSquareSum * twoBandBoxesSize - leftRightSum * leftRightSum) * invTwoBandBoxesSizeSqr;
				ocean_assert(leftRightVariance >= 0.0f && leftRightVariance < 256.0f * 256.0f);

				if constexpr (tSquaredResponse)
				{
					const float verticalVariance = (leftRightVariance + middleVariance) * 0.5f;

					const float normalizedVerticalResponse = std::max(0.0f, NumericF::sqr(verticalResponse) - verticalVariance) / std::max(1.0f, verticalVariance); // using sqr(verticalResponse), and variance

					*verticalResponses = NumericF::copySign(normalizedVerticalResponse, verticalResponse);
				}
				else
				{
					const float middleDeviation = NumericF::sqrt(middleVariance);
					const float leftRightDeviation = NumericF::sqrt(leftRightVariance);

					const float verticalDeviation = (leftRightDeviation + middleDeviation) * 0.5f;

					const float normalizedVerticalResponse = std::max(0.0f, NumericF::abs(verticalResponse) - verticalDeviation) / std::max(1.0f, verticalDeviation); // using abs(verticalResponse), and deviation

					*verticalResponses = NumericF::copySign(normalizedVerticalResponse, verticalResponse);
				}
			}
			else
			{
				*verticalResponses = 0.0f;
			}

			leftBand_top += 2;
			leftBand_bottom += 2;

			middleBand_top += 2;
			middleBand_bottom += 2;

			rightBand_top += 2;
			rightBand_bottom += 2;

			++verticalResponses;
		}

		leftBand_top += responseAreaWidth * 2u + linedIntegralAndSquaredPaddingElements;
		leftBand_bottom += responseAreaWidth * 2u + linedIntegralAndSquaredPaddingElements;

		middleBand_top += responseAreaWidth * 2u + linedIntegralAndSquaredPaddingElements;
		middleBand_bottom += responseAreaWidth * 2u + linedIntegralAndSquaredPaddingElements;

		rightBand_top += responseAreaWidth * 2u + linedIntegralAndSquaredPaddingElements;
		rightBand_bottom += responseAreaWidth * 2u + linedIntegralAndSquaredPaddingElements;

		verticalResponses += verticalResponsesPaddingElements;
	}

#ifdef OCEAN_DEBUG
	ocean_assert(verticalResponses == debugVerticalResponses + (height - responseAreaHeight + 1u) * verticalResponsesStrideElements);
#endif

	ocean_assert(leftBand_top == linedIntegralAndSquared + (height - responseAreaHeight + 1u) * linedIntegralAndSquaredStrideElements);
}

template void OCEAN_CV_DETECTOR_EXPORT ShapeDetector::PatternDetectorGradientVarianceBased::determineVerticalResponsesF<true>(const uint32_t*, const unsigned int, const unsigned int, float*, const unsigned int, const unsigned int);
template void OCEAN_CV_DETECTOR_EXPORT ShapeDetector::PatternDetectorGradientVarianceBased::determineVerticalResponsesF<false>(const uint32_t*, const unsigned int, const unsigned int, float*, const unsigned int, const unsigned int);

template <bool tSquaredResponse, bool tCreateResponseFrame>
void ShapeDetector::PatternDetectorGradientVarianceBased::determineTopDownResponsesF(const float* horizontalResponses, const float* verticalResponses, const unsigned int width, const unsigned int height, const int32_t sign, const float minimalThreshold, CV::NonMaximumSuppression<float>& nonMaximumSuppression, Frame* responseFrame)
{
	static_assert(shapeWidth_ >= 1u && shapeWidth_ % 2u == 1u, "Invalid shape width!");
	static_assert(shapeHeight_ >= shapeStepSize_ + 1u, "Invalid shape height!");
	static_assert(shapeStepSize_ >= 1u && shapeStepSize_ % 2u == 1u, "Invalid shape step size!");
	static_assert(shapeBandSize_ >= 1u, "Invalid shape band size!");

	ocean_assert(horizontalResponses != nullptr);
	ocean_assert(verticalResponses != nullptr);
	ocean_assert(width >= shapeWidth_ && width >= 20u);
	ocean_assert(height >= shapeHeight_ && height >= 20u);
	ocean_assert(sign >= -1 && sign <= 1);

	ocean_assert(nonMaximumSuppression.width() == width);
	ocean_assert(nonMaximumSuppression.height() == height);

	if constexpr (tCreateResponseFrame)
	{
		ocean_assert(responseFrame);

		responseFrame->set(FrameType(width, height, FrameType::genericPixelFormat<float, 1u>(), FrameType::ORIGIN_UPPER_LEFT), false /*forceOwner*/, true /*forceWritable*/);
		responseFrame->setValue<float, 1u>(Frame::PixelType<float, 1u>({{0.0f}}));
	}
	else
	{
		ocean_assert(responseFrame == nullptr);
	}

	const unsigned int horizontalResponseWidth = determineHorizontalResponseWidth(width);
	const unsigned int verticalResponseWidth = determineVerticalResponseWidth(width);

	const unsigned int responseWidth = std::min(horizontalResponseWidth, verticalResponseWidth); // the actual width of the response area
	ocean_assert(responseWidth == width - shapeWidth_2_ * 2u);

	const unsigned int horizontalResponseSkippedElements = horizontalResponseWidth - responseWidth;
	const unsigned int verticalResponseSkippedElements = verticalResponseWidth - responseWidth;
	ocean_assert(horizontalResponseSkippedElements < width);
	ocean_assert(verticalResponseSkippedElements < width);

	constexpr unsigned int xFrameStart = frameX_T_topDownResponseX();
	const unsigned int xFrameEnd = width - shapeWidth_2_;
	ocean_assert(responseWidth == xFrameEnd - xFrameStart);

	constexpr unsigned int yFrameStart = frameY_T_topDownResponseY();
	const unsigned int yFrameEnd = height - shapeHeight_ + shapeStepSize_2_;

	constexpr unsigned int horizontalResponseStartX = frameX_T_topDownResponseX() - frameX_T_topDownHorizontalResponseX();
	constexpr unsigned int horizontalResponseStartY = frameY_T_topDownResponseY() - frameY_T_topDownHorizontalResponseY();

	constexpr unsigned int verticalResponseStartX = frameX_T_topDownResponseX() - frameX_T_topDownVerticalResponseX();
	constexpr unsigned int verticalResponseStartY = frameY_T_topDownResponseY() - frameY_T_topDownVerticalResponseY();

	static_assert(int(horizontalResponseStartX) >= 0 && int(horizontalResponseStartY) >= 0, "Invalid start location!");
	static_assert(int(verticalResponseStartX) >= 0 && int(verticalResponseStartY) >= 0, "Invalid start location!");

	// start pointer for the first response
	const float* horizontalResponse = horizontalResponses + horizontalResponseStartY * horizontalResponseWidth + horizontalResponseStartX;
	const float* verticalResponse = verticalResponses + verticalResponseStartY * verticalResponseWidth + verticalResponseStartX;

	for (unsigned int y = yFrameStart; y < yFrameEnd; ++y)
	{
		for (unsigned int x = xFrameStart; x < xFrameEnd; ++x)
		{
			ocean_assert(x - xFrameStart < responseWidth);

			const float& signedHorizontalResponse = *horizontalResponse;
			const float& signedVerticalResponse = *verticalResponse;

			const float absHorizontalResponse = NumericF::abs(signedHorizontalResponse);
			const float absVerticalResponse = NumericF::abs(signedVerticalResponse);

			constexpr float maximalResponseRatio = tSquaredResponse ? (4.0f * 4.0f) : 4.0f;

			if (absHorizontalResponse < absVerticalResponse * maximalResponseRatio && absVerticalResponse < absHorizontalResponse * maximalResponseRatio)
			{
				if (haveCorrectSign(sign, signedHorizontalResponse, signedVerticalResponse))
				{
					const float response = absHorizontalResponse * absVerticalResponse;

					if constexpr (tSquaredResponse)
					{
						if (response >= minimalThreshold * minimalThreshold)
						{
							nonMaximumSuppression.addCandidate(x, y, response);
						}
					}
					else
					{
						if (response >= minimalThreshold)
						{
							nonMaximumSuppression.addCandidate(x, y, response);
						}
					}

					if constexpr (tCreateResponseFrame)
					{
						responseFrame->pixel<float>(x, y)[0] = response;
					}
				}
			}

			++horizontalResponse;
			++verticalResponse;
		}

		horizontalResponse += horizontalResponseSkippedElements;
		verticalResponse += verticalResponseSkippedElements;
	}
}

template <bool tSquaredResponse, bool tCreateResponseFrame>
void ShapeDetector::PatternDetectorGradientVarianceBased::determineBottomUpResponsesF(const float* horizontalResponses, const float* verticalResponses, const unsigned int width, const unsigned int height, const int32_t sign, const float minimalThreshold, CV::NonMaximumSuppression<float>& nonMaximumSuppression, Frame* responseFrame)
{
	static_assert(shapeWidth_ >= 1u && shapeWidth_ % 2u == 1u, "Invalid shape width!");
	static_assert(shapeHeight_ >= shapeStepSize_ + 1u, "Invalid shape height!");
	static_assert(shapeStepSize_ >= 1u && shapeStepSize_ % 2u == 1u, "Invalid shape step size!");
	static_assert(shapeBandSize_ >= 1u, "Invalid shape band size!");

	ocean_assert(horizontalResponses != nullptr);
	ocean_assert(verticalResponses != nullptr);
	ocean_assert(width >= shapeWidth_ && width >= 20u);
	ocean_assert(height >= shapeHeight_ && height >= 20u);
	ocean_assert(sign >= -1 && sign <= 1);

	ocean_assert(nonMaximumSuppression.width() == width);
	ocean_assert(nonMaximumSuppression.height() == height);

	if constexpr (tCreateResponseFrame)
	{
		ocean_assert(responseFrame);

		responseFrame->set(FrameType(width, height, FrameType::genericPixelFormat<float, 1u>(), FrameType::ORIGIN_UPPER_LEFT), false /*forceOwner*/, true /*forceWritable*/);
		responseFrame->setValue<float, 1u>(Frame::PixelType<float, 1u>({{0.0f}}));
	}
	else
	{
		ocean_assert(responseFrame == nullptr);
	}

	const unsigned int horizontalResponseWidth = determineHorizontalResponseWidth(width);
	const unsigned int verticalResponseWidth = determineVerticalResponseWidth(width);
	const unsigned int responseWidth = std::min(horizontalResponseWidth, verticalResponseWidth); // the actual width of the response area
	ocean_assert(responseWidth == width - shapeWidth_2_ * 2u);

	const unsigned int horizontalResponseSkippedElements = horizontalResponseWidth - responseWidth;
	const unsigned int verticalResponseSkippedElements = verticalResponseWidth - responseWidth;
	ocean_assert(horizontalResponseSkippedElements < width);
	ocean_assert(verticalResponseSkippedElements < width);

	constexpr unsigned int xFrameStart = frameX_T_bottomUpResponseX();
	const unsigned int xFrameEnd = width - shapeWidth_2_;
	ocean_assert(responseWidth == xFrameEnd - xFrameStart);

	constexpr unsigned int yFrameStart = frameY_T_bottomUpResponseY();
	const unsigned int yFrameEnd = height - shapeBandSize_ - shapeStepSize_2_;

	constexpr unsigned int horizontalResponseStartX = frameX_T_bottomUpResponseX() - frameX_T_bottomUpHorizontalResponseX();
	constexpr unsigned int horizontalResponseStartY = frameY_T_bottomUpResponseY() - frameY_T_bottomUpHorizontalResponseY();

	constexpr unsigned int verticalResponseStartX = frameX_T_bottomUpResponseX() - frameX_T_bottomUpVerticalResponseX();
	constexpr unsigned int verticalResponseStartY = frameY_T_bottomUpResponseY() - frameY_T_bottomUpVerticalResponseY();

	static_assert(int(horizontalResponseStartX) >= 0 && int(horizontalResponseStartY) >= 0, "Invalid start location!");
	static_assert(int(verticalResponseStartX) >= 0 && int(verticalResponseStartY) >= 0, "Invalid start location!");

	// start pointer for the first response
	const float* horizontalResponse = horizontalResponses + horizontalResponseStartY * horizontalResponseWidth + horizontalResponseStartX;
	const float* verticalResponse = verticalResponses + verticalResponseStartY * verticalResponseWidth + verticalResponseStartX;

	for (unsigned int y = yFrameStart; y < yFrameEnd; ++y)
	{
		for (unsigned int x = xFrameStart; x < xFrameEnd; ++x)
		{
			ocean_assert(x - xFrameStart < responseWidth);

			const float& signedHorizontalResponse = *horizontalResponse;
			const float& signedVerticalResponse = *verticalResponse;

			const float absHorizontalResponse = NumericF::abs(signedHorizontalResponse);
			const float absVerticalResponse = NumericF::abs(signedVerticalResponse);

			constexpr float maximalResponseRatio = tSquaredResponse ? (4.0f * 4.0f) : 4.0f;

			if (absHorizontalResponse < absVerticalResponse * maximalResponseRatio && absVerticalResponse < absHorizontalResponse * maximalResponseRatio)
			{
				if (haveCorrectSign(sign, signedHorizontalResponse, signedVerticalResponse))
				{
					const float response = absHorizontalResponse * absVerticalResponse;

					if constexpr (tSquaredResponse)
					{
						if (response >= minimalThreshold * minimalThreshold)
						{
							nonMaximumSuppression.addCandidate(x, y, response);
						}
					}
					else
					{
						if (response >= minimalThreshold)
						{
							nonMaximumSuppression.addCandidate(x, y, response);
						}
					}

					if constexpr (tCreateResponseFrame)
					{
						responseFrame->pixel<float>(x, y)[0] = response;
					}
				}
			}

			++horizontalResponse;
			++verticalResponse;
		}

		horizontalResponse += horizontalResponseSkippedElements;
		verticalResponse += verticalResponseSkippedElements;
	}
}

template <bool tCreateResponseFrame>
void ShapeDetector::PatternDetectorGradientVarianceBased::determineResponsesI(const int32_t* horizontalResponses, const int32_t* verticalResponses, const unsigned int width, const unsigned int height, const int32_t sign, const uint32_t minimalSqrThreshold, CV::NonMaximumSuppression<uint32_t>& nonMaximumSuppressionTopDown, CV::NonMaximumSuppression<uint32_t>& nonMaximumSuppressionBottomUp, Frame* responseFrameTopDown, Frame* responseFrameBottomUp)
{
	static_assert(shapeWidth_ >= 1u && shapeWidth_ % 2u == 1u, "Invalid shape width!");
	static_assert(shapeHeight_ >= shapeStepSize_ + 1u, "Invalid shape height!");
	static_assert(shapeStepSize_ >= 1u && shapeStepSize_ % 2u == 1u, "Invalid shape step size!");
	static_assert(shapeBandSize_ >= 1u, "Invalid shape band size!");

	ocean_assert(horizontalResponses != nullptr);
	ocean_assert(verticalResponses != nullptr);
	ocean_assert(width >= shapeWidth_ && width >= 20u);
	ocean_assert(height >= shapeHeight_ && height >= 20u);
	ocean_assert(sign >= -1 && sign <= 1);

	ocean_assert(nonMaximumSuppressionTopDown.width() == width);
	ocean_assert(nonMaximumSuppressionTopDown.height() == height);

	ocean_assert(nonMaximumSuppressionBottomUp.width() == width);
	ocean_assert(nonMaximumSuppressionBottomUp.height() == height);

	if constexpr (tCreateResponseFrame)
	{
		ocean_assert(responseFrameTopDown != nullptr);
		ocean_assert(responseFrameBottomUp != nullptr);

		responseFrameTopDown->set(FrameType(width, height, FrameType::FORMAT_F32, FrameType::ORIGIN_UPPER_LEFT), false /*forceOwner*/, true /*forceWritable*/);
		responseFrameTopDown->setValue<float, 1u>(Frame::PixelType<float, 1u>({{0.0f}}));

		responseFrameBottomUp->set(FrameType(width, height, FrameType::FORMAT_F32, FrameType::ORIGIN_UPPER_LEFT), false /*forceOwner*/, true /*forceWritable*/);
		responseFrameBottomUp->setValue<float, 1u>(Frame::PixelType<float, 1u>({{0.0f}}));
	}
	else
	{
		ocean_assert(responseFrameTopDown == nullptr);
		ocean_assert(responseFrameBottomUp == nullptr);
	}

	const unsigned int horizontalResponseWidth = determineHorizontalResponseWidth(width);
	const unsigned int verticalResponseWidth = determineVerticalResponseWidth(width);
	const unsigned int responseWidth = std::min(horizontalResponseWidth, verticalResponseWidth); // the actual width of the response area
	ocean_assert(responseWidth == width - shapeWidth_2_ * 2u);

	const unsigned int horizontalResponseSkippedElements = horizontalResponseWidth - responseWidth;
	const unsigned int verticalResponseSkippedElements = verticalResponseWidth - responseWidth;
	ocean_assert(horizontalResponseSkippedElements < width);
	ocean_assert(verticalResponseSkippedElements < width);


	// start pointer for the first top-down response

	constexpr unsigned int xFrameStartTopDown = frameX_T_topDownResponseX();
	const unsigned int xFrameEndTopDown = width - shapeWidth_2_;
	ocean_assert(responseWidth == xFrameEndTopDown - xFrameStartTopDown);

	constexpr unsigned int yFrameStartTopDown = frameY_T_topDownResponseY();
	const unsigned int yFrameEndTopDown = height - shapeHeight_ + shapeStepSize_2_;

	constexpr unsigned int horizontalResponseStartTopDownX = frameX_T_topDownResponseX() - frameX_T_topDownHorizontalResponseX();
	constexpr unsigned int horizontalResponseStartTopDownY = frameY_T_topDownResponseY() - frameY_T_topDownHorizontalResponseY();

	constexpr unsigned int verticalResponseStartTopDownX = frameX_T_topDownResponseX() - frameX_T_topDownVerticalResponseX();
	constexpr unsigned int verticalResponseStartTopDownY = frameY_T_topDownResponseY() - frameY_T_topDownVerticalResponseY();

	static_assert(int(horizontalResponseStartTopDownX) >= 0 && int(horizontalResponseStartTopDownY) >= 0, "Invalid start location!");
	static_assert(int(verticalResponseStartTopDownX) >= 0 && int(verticalResponseStartTopDownY) >= 0, "Invalid start location!");

	const int32_t* horizontalResponseTopDown = horizontalResponses + horizontalResponseStartTopDownY * horizontalResponseWidth + horizontalResponseStartTopDownX;
	const int32_t* verticalResponseTopDown = verticalResponses + verticalResponseStartTopDownY * verticalResponseWidth + verticalResponseStartTopDownX;


	// start pointer for the first bottom-up response

	constexpr unsigned int xFrameStartBottomUp = frameX_T_bottomUpResponseX();
	const unsigned int xFrameEndBottomUp = width - shapeWidth_2_;
	ocean_assert_and_suppress_unused(responseWidth == xFrameEndBottomUp - xFrameStartBottomUp, xFrameEndBottomUp);

	constexpr unsigned int yFrameStartBottomUp = frameY_T_bottomUpResponseY();
	const unsigned int yFrameEndBottomUp = height - shapeBandSize_ - shapeStepSize_2_;

	constexpr unsigned int horizontalResponseStartBottomUpX = frameX_T_bottomUpResponseX() - frameX_T_bottomUpHorizontalResponseX();
	constexpr unsigned int horizontalResponseStartBottomUpY = frameY_T_bottomUpResponseY() - frameY_T_bottomUpHorizontalResponseY();

	constexpr unsigned int verticalResponseStartBottomUpX = frameX_T_bottomUpResponseX() - frameX_T_bottomUpVerticalResponseX();
	constexpr unsigned int verticalResponseStartBottomUpY = frameY_T_bottomUpResponseY() - frameY_T_bottomUpVerticalResponseY();

	static_assert(int(horizontalResponseStartBottomUpX) >= 0 && int(horizontalResponseStartBottomUpY) >= 0, "Invalid start location!");
	static_assert(int(verticalResponseStartBottomUpX) >= 0 && int(verticalResponseStartBottomUpY) >= 0, "Invalid start location!");

	const int32_t* horizontalResponseBottomUp = horizontalResponses + horizontalResponseStartBottomUpY * horizontalResponseWidth + horizontalResponseStartBottomUpX;
	const int32_t* verticalResponseBottomUp = verticalResponses + verticalResponseStartBottomUpY * verticalResponseWidth + verticalResponseStartBottomUpX;


	unsigned int yFrameBottomUp = yFrameStartBottomUp;

	for (unsigned int yFrameTopDown = yFrameStartTopDown; yFrameTopDown < yFrameEndTopDown; ++yFrameTopDown)
	{
		ocean_assert_and_suppress_unused(yFrameBottomUp < yFrameEndBottomUp, yFrameEndBottomUp);

		unsigned int xFrameBottomUp = xFrameStartBottomUp;

		for (unsigned int xFrameTopDown = xFrameStartTopDown; xFrameTopDown < xFrameEndTopDown; ++xFrameTopDown)
		{
			ocean_assert(xFrameTopDown - xFrameStartTopDown < responseWidth);
			ocean_assert(xFrameBottomUp < xFrameEndBottomUp);

			const int32_t& signedHorizontalResponseTopDown = *horizontalResponseTopDown;
			const int32_t& signedVerticalResponseTopDown = *verticalResponseTopDown;

			const uint32_t absHorizontalResponseTopDown = uint32_t(abs(signedHorizontalResponseTopDown));
			const uint32_t absVerticalResponseTopDown = uint32_t(abs(signedVerticalResponseTopDown));

			constexpr uint32_t maximalSquareResponseRatio = 4u * 4u;

			if (absHorizontalResponseTopDown < absVerticalResponseTopDown * maximalSquareResponseRatio && absVerticalResponseTopDown < absHorizontalResponseTopDown * maximalSquareResponseRatio)
			{
				if (haveCorrectSign(sign, signedHorizontalResponseTopDown, signedVerticalResponseTopDown))
				{
					const uint32_t response = absHorizontalResponseTopDown * absVerticalResponseTopDown;

					if (response >= minimalSqrThreshold)
					{
						nonMaximumSuppressionTopDown.addCandidate(xFrameTopDown, yFrameTopDown, response);
					}

					if constexpr (tCreateResponseFrame)
					{
						responseFrameTopDown->pixel<float>(xFrameTopDown, yFrameTopDown)[0] = float(response);
					}
				}
			}

			const int32_t& signedHorizontalResponseBottomUp = *horizontalResponseBottomUp;
			const int32_t& signedVerticalResponseBottomUp = *verticalResponseBottomUp;

			const uint32_t absHorizontalResponseBottomUp = uint32_t(abs(signedHorizontalResponseBottomUp));
			const uint32_t absVerticalResponseBottomUp = uint32_t(abs(signedVerticalResponseBottomUp));

			if (absHorizontalResponseBottomUp < absVerticalResponseBottomUp * maximalSquareResponseRatio && absVerticalResponseBottomUp < absHorizontalResponseBottomUp * maximalSquareResponseRatio)
			{
				if (haveCorrectSign(sign, signedHorizontalResponseBottomUp, signedVerticalResponseBottomUp))
				{
					const uint32_t response = absHorizontalResponseBottomUp * absVerticalResponseBottomUp;

					if (response >= minimalSqrThreshold)
					{
						nonMaximumSuppressionBottomUp.addCandidate(xFrameBottomUp, yFrameBottomUp, response);
					}

					if constexpr (tCreateResponseFrame)
					{
						responseFrameBottomUp->pixel<float>(xFrameBottomUp, yFrameBottomUp)[0] = float(response);
					}
				}
			}

			++horizontalResponseTopDown;
			++verticalResponseTopDown;

			++horizontalResponseBottomUp;
			++verticalResponseBottomUp;

			++xFrameBottomUp;
		}

		horizontalResponseTopDown += horizontalResponseSkippedElements;
		verticalResponseTopDown += verticalResponseSkippedElements;

		horizontalResponseBottomUp += horizontalResponseSkippedElements;
		verticalResponseBottomUp += verticalResponseSkippedElements;

		++yFrameBottomUp;
	}
}

ShapeDetector::Rectangles ShapeDetector::detectAlignedRectangles(const Frame& yFrame, const unsigned int rectangleWidth, const Scalar aspectRatio, const Scalar aspectRatioTolerance, const Scalar alignmentAngleTolerance, const bool sortRectangles, const unsigned int lineImageBorderDistanceThreshold, const unsigned int perpendicularSampleDistance)
{
	ocean_assert(yFrame.isValid() && yFrame.pixelFormat() == FrameType::FORMAT_Y8);
	ocean_assert(Scalar(rectangleWidth) >= Scalar(1) && Scalar(rectangleWidth) < Scalar(yFrame.width()));
	ocean_assert(Scalar(rectangleWidth) / aspectRatio >= Scalar(1) && Scalar(rectangleWidth) / aspectRatio < Scalar(yFrame.height()));
	ocean_assert(aspectRatio >= Scalar(0.01) && aspectRatio <= Scalar(100));
	ocean_assert(aspectRatioTolerance >= 0 && aspectRatioTolerance < Scalar(1));
	ocean_assert(alignmentAngleTolerance >= Numeric::deg2rad(0) && alignmentAngleTolerance <= Numeric::deg2rad(90));

	// Extract line segments

	const unsigned int lineDetectionThreshold = 20u;
	const unsigned int lineMinimumLength = (rectangleWidth + 3u) / 6u;
	const float lineMaximumDistance = 2.1f;

	FiniteLines2 finiteLines = CV::Detector::LineDetectorULF::detectLines(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), CV::Detector::LineDetectorULF::RMSStepEdgeDetectorI::asEdgeDetectors(), lineDetectionThreshold, lineMinimumLength, lineMaximumDistance);

	// Remove lines that are too close to the image border because those lines could be part of other objects already (tables, windows, frames, etc.).

	CV::Detector::ShapeDetector::removeLinesTooCloseToBorder(finiteLines, yFrame.width(), yFrame.height(), Scalar(lineImageBorderDistanceThreshold)); // TODOX Pixel-precision for distance should be fine

	// Merge lines that are close to each other and have similar orientation. This allows small gaps in the edge lines to be bridged, e.g., because of objects or finger tips.

	const Scalar mergeMaximumDistanceToInfiniteLine = Scalar(5);
	const Scalar mergeMaximumLineEndpointDistance = Scalar((rectangleWidth + 1u) / 2u);
	const Scalar mergeMaximumCosAngleDifference = Numeric::cos(Numeric::deg2rad(15));

	finiteLines = HemiCube::mergeGreedyBruteForce(finiteLines, mergeMaximumDistanceToInfiniteLine, mergeMaximumLineEndpointDistance, nullptr, mergeMaximumCosAngleDifference);

	CV::Detector::ShapeDetector::removeLinesTooCloseToBorder(finiteLines, yFrame.width(), yFrame.height(), Scalar(lineImageBorderDistanceThreshold)); // TODOX Pixel-precision for distance should be fine

	// Determine L-shape from the collection of line segments

	const Scalar lShapeDistanceThreshold = Scalar((rectangleWidth + 1u) / 2u);
	const Scalar lShapeAngleThreshold = Numeric::deg2rad(25);

	CV::Detector::ShapeDetector::LShapes lShapes = CV::Detector::ShapeDetector::determineLShapes(finiteLines, yFrame.width(), yFrame.height(), lShapeDistanceThreshold, lShapeAngleThreshold);

	// Remove L-shapes that exceed the alignment direction by a specified amount

	lShapes = CV::Detector::ShapeDetector::filterLShapesBasedOnDirection<true>(lShapes, Vector2(1, 1).normalized(), alignmentAngleTolerance);

	// Apply non maximum suppression to remove redundant L-shapes

	const Scalar nonMaximumSuppressionDistanceThreshold = Scalar(lShapeDistanceThreshold) * Scalar(0.5);
	const Scalar nonMaximumSuppressionAngleThreshold = Numeric::deg2rad(25);

	lShapes = CV::Detector::ShapeDetector::nonMaximumSuppressionLShapes(lShapes, yFrame.width(), yFrame.height(), nonMaximumSuppressionDistanceThreshold, nonMaximumSuppressionAngleThreshold);

	// Determine all valid combinations of rectangles from the L-shapes

	const Vector2 alignedRectTopLeftCornerDirection = Vector2(1, 1).normalized();
	const Scalar alignedRectMinimalCornerDistance = Scalar(10);
	const Scalar alignedRectConnectedShapesAngleThreshold = Numeric::deg2rad(15);

	CV::Detector::ShapeDetector::IndexedRectangles indexedRectangles = CV::Detector::ShapeDetector::determineAlignedRectangles(finiteLines, lShapes, alignedRectTopLeftCornerDirection, alignedRectMinimalCornerDistance, alignmentAngleTolerance, alignedRectConnectedShapesAngleThreshold); // (1,1), 10, ..., 10

	// Refine the selection of detected rectangles based on additional geometric constraints

	const Scalar shapedRectMinimumRectangleWidth = Scalar(rectangleWidth) * Scalar(0.5);
	const Scalar shapedRectMaximumRectangleWidth = Scalar(rectangleWidth) * Scalar(2.0);
	const Scalar shapedRectAspectRatioTolerance = aspectRatioTolerance;
	const Scalar shapedRectOrthogonalAngleTolerance = Numeric::deg2rad(5);

	indexedRectangles = CV::Detector::ShapeDetector::determineShapedRectangles(finiteLines, lShapes, indexedRectangles, aspectRatio, shapedRectMinimumRectangleWidth, shapedRectMaximumRectangleWidth, shapedRectAspectRatioTolerance, shapedRectOrthogonalAngleTolerance);

	// Store the corner locations of the detected rectangles

	Rectangles rectangles;
	rectangles.reserve(indexedRectangles.size());

	for (const IndexedRectangle& rectangle : indexedRectangles)
	{
		Rectangle newRectangle =
		{{
			lShapes[rectangle[0]].position(), // TL
			lShapes[rectangle[1]].position(), // BL
			lShapes[rectangle[2]].position(), // BR
			lShapes[rectangle[3]].position(), // TR
		}};

		if (optimizeRectangleAlongEdges(yFrame, newRectangle, perpendicularSampleDistance))
		{
			rectangles.emplace_back(std::move(newRectangle));
		}
	}

	// If requested, sort the rectangles by their area in descending order

	if (sortRectangles)
	{
		std::sort(rectangles.begin(), rectangles.end(), ShapeDetector::hasGreaterArea);
	}

	return rectangles;
}

ShapeDetector::LShapes ShapeDetector::determineLShapes(const FiniteLines2& finiteLines, const unsigned int width, const unsigned int height, const Scalar thresholdDistance, const Scalar thresholdAngle)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(thresholdDistance >= 0);
	ocean_assert(thresholdAngle < Numeric::deg2rad(45));

	const Scalar thresholdDistanceSqr = thresholdDistance * thresholdDistance;
	const Scalar thresholdAngleCos = Numeric::cos(Numeric::pi_2() - thresholdAngle);

	// we use a distribution array for nearest neighbor search

	unsigned int horizontalBins = 1u;
	unsigned int verticalBins = 1u;
	Geometry::SpatialDistribution::idealBinsNeighborhood9(width, height, thresholdDistance, horizontalBins, verticalBins);
	Geometry::SpatialDistribution::DistributionArray distributionArray(Scalar(0), Scalar(0), Scalar(width), Scalar(height), horizontalBins, verticalBins);

	for (unsigned int n = 0u; n < (unsigned int)finiteLines.size(); ++n)
	{
		const FiniteLine2& line = finiteLines[n];

		if (line.point0().x() < 0 || line.point0().x() > Scalar(width - 1u) || line.point0().y() < 0 || line.point0().y() > Scalar(height - 1u)
			|| line.point1().x() < 0 || line.point1().x() > Scalar(width - 1u) || line.point1().y() < 0 || line.point1().y() > Scalar(height - 1u))
		{
			continue;
		}


		const unsigned int xBin0 = distributionArray.horizontalBin(line.point0().x());
		const unsigned int yBin0 = distributionArray.verticalBin(line.point0().y());

		distributionArray(xBin0, yBin0).push_back(n);

		const unsigned int xBin1 = distributionArray.horizontalBin(line.point1().x());
		const unsigned int yBin1 = distributionArray.verticalBin(line.point1().y());

		if (xBin0 != xBin1 || yBin0 != yBin1)
		{
			distributionArray(xBin1, yBin1).push_back(n);
		}
	}

	LShapes lShapes;
	lShapes.reserve(finiteLines.size() / 8);

	std::set<IndexPair32> indexPairSet;

	Indices32 indicesNeighborhood;

	for (unsigned int a = 0u; a < finiteLines.size(); ++a)
	{
		const FiniteLine2& lineA = finiteLines[a];

		for (unsigned int i = 0u; i < 2u; ++i)
		{
			// we check each end point individually
			const Vector2& linePoint = lineA.point(i);

			const unsigned int xBin = distributionArray.horizontalBin(linePoint.x());
			const unsigned int yBin = distributionArray.verticalBin(linePoint.y());

			indicesNeighborhood.clear();
			distributionArray.indicesNeighborhood9(xBin, yBin, indicesNeighborhood);

			for (const Index32& b : indicesNeighborhood)
			{
				// we ensure that we do not get reverse pairs, first index will always be smaller than the second index
				if (b > a)
				{
					if (indexPairSet.find(IndexPair32(a, b)) == indexPairSet.cend())
					{
						const FiniteLine2& lineB = finiteLines[b];

						if (Numeric::abs(lineA.direction() * lineB.direction()) <= thresholdAngleCos)
						{
							// both line segments are perpendicular

							if (linePoint.sqrDistance(lineB.point0()) <= thresholdDistanceSqr || linePoint.sqrDistance(lineB.point1()) <= thresholdDistanceSqr)
							{
								Vector2 intersection;
								if (Line2(lineA.point0(), lineA.direction()).intersection(Line2(lineB.point0(), lineB.direction()), intersection) && intersection.x() >= Scalar(0) && intersection.y() >= Scalar(0) && intersection.x() < Scalar(width) && intersection.y() < Scalar(height))
								{
									// both line segments have a direct or indirect intersection point close to an end point of both line segments

									const Vector2& line0Point = lineA.point0().sqrDistance(intersection) > lineA.point1().sqrDistance(intersection) ? lineA.point0() : lineA.point1();
									const Vector2& line1Point = lineB.point0().sqrDistance(intersection) > lineB.point1().sqrDistance(intersection) ? lineB.point0() : lineB.point1();

									Vector2 edgeLeft = line0Point - intersection;
									Vector2 edgeRight = line1Point - intersection;

									if (edgeLeft.normalize() && edgeRight.normalize())
									{
										Vector2 direction = edgeLeft + edgeRight;

										if (direction.normalize())
										{
											const Scalar score = std::max(Scalar(0), lineA.length() - lineA.distance(intersection)) * std::max(Scalar(0), (lineB.length() - lineB.distance(intersection)));

											Index32 lineIndexLeft = a;
											Index32 lineIndexRight = b;

											// we need a left and a right edge
											if (edgeLeft.cross(edgeRight) < 0)
											{
												std::swap(edgeLeft, edgeRight);
												std::swap(lineIndexLeft, lineIndexRight);
											}

											ocean_assert(edgeLeft.isParallel(lineA.direction()) || edgeLeft.isParallel(lineB.direction()));
											ocean_assert(edgeRight.isParallel(lineA.direction()) || edgeRight.isParallel(lineB.direction()));

											lShapes.emplace_back(lineIndexLeft, lineIndexRight, intersection, direction, edgeLeft, edgeRight, score);

											indexPairSet.insert(IndexPair32(a, b));
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return lShapes;
}

void ShapeDetector::determineShapes(const FiniteLines2& horizontalFiniteLines, const FiniteLines2& verticalFiniteLines, const unsigned int /*width*/, const unsigned int /*height*/, LShapes& lShapes, TShapes& tShapes, XShapes& xShapes, const Scalar thresholdShortDistance, const Scalar thresholdLongDistance, const Scalar thresholdAngleShape, const Scalar thresholdAngleXShape)
{
	ocean_assert(thresholdAngleShape <= Numeric::deg2rad(25));
	ocean_assert(thresholdAngleXShape <= thresholdAngleShape);
	ocean_assert(Numeric::eps() < thresholdShortDistance && thresholdShortDistance < thresholdLongDistance);

	const Scalar thresholdAngleShapeCos = Numeric::cos(Numeric::pi_2() - thresholdAngleShape);
	const Scalar thresholdAngleXShapeCos = Numeric::cos(Numeric::pi_2() - thresholdAngleXShape);

	const Scalar thresholdShortDistance2 = thresholdShortDistance * Scalar(2);

	Scalars lengthHorizontalFiniteLines;
	lengthHorizontalFiniteLines.reserve(horizontalFiniteLines.size());
	for (const FiniteLine2& horizontalFiniteLine : horizontalFiniteLines)
	{
		ocean_assert(horizontalFiniteLine.isValid());
		lengthHorizontalFiniteLines.push_back(horizontalFiniteLine.length());
	}

	Scalars lengthVerticalFiniteLines;
	lengthVerticalFiniteLines.reserve(verticalFiniteLines.size());
	for (const FiniteLine2& verticalFiniteLine : verticalFiniteLines)
	{
		ocean_assert(verticalFiniteLine.isValid());
		lengthVerticalFiniteLines.push_back(verticalFiniteLine.length());
	}

	ocean_assert(lengthHorizontalFiniteLines.size() == horizontalFiniteLines.size());
	ocean_assert(lengthVerticalFiniteLines.size() == verticalFiniteLines.size());

	for (unsigned int nHorizontal = 0u; nHorizontal < (unsigned int)horizontalFiniteLines.size(); ++nHorizontal)
	{
		const Scalar& lengthHorizontalFiniteLine = lengthHorizontalFiniteLines[nHorizontal];

		if (lengthHorizontalFiniteLine <= thresholdLongDistance || lengthHorizontalFiniteLine <= thresholdShortDistance2)
		{
			// the line is too short so that we do not need to investage it
			continue;
		}

		const FiniteLine2& horizontalFiniteLine = horizontalFiniteLines[nHorizontal];

		const Line2 horizontalInfiniteLine = Line2(horizontalFiniteLine.point0(), horizontalFiniteLine.direction());

		for (unsigned int nVertical = 0u; nVertical < (unsigned int)verticalFiniteLines.size(); ++nVertical)
		{
			const Scalar& lengthVerticalFiniteLine = lengthVerticalFiniteLines[nVertical];

			if (lengthVerticalFiniteLine <= thresholdLongDistance || lengthVerticalFiniteLine <= thresholdShortDistance2)
			{
				// the line is too short so that we do not need to investage it
				continue;
			}

			const FiniteLine2& verticalFiniteLine = verticalFiniteLines[nVertical];

			if (Numeric::abs(horizontalFiniteLine.direction() * verticalFiniteLine.direction()) <= thresholdAngleShapeCos)
			{
				Vector2 intersectionPoint;
				if (horizontalInfiniteLine.intersection(Line2(verticalFiniteLine.point0(), verticalFiniteLine.direction()), intersectionPoint))
				{
					ocean_assert(horizontalInfiniteLine.distance(intersectionPoint) < Scalar(0.1));
					ocean_assert(Line2(verticalFiniteLine.point0(), verticalFiniteLine.direction()).distance(intersectionPoint) < Scalar(0.1));

					const Scalar lengthOnHorizontalLine = horizontalFiniteLine.direction() * (intersectionPoint - horizontalFiniteLine.point0());
					const Scalar lengthOnVerticalLine = verticalFiniteLine.direction() * (intersectionPoint - verticalFiniteLine.point0());

					ocean_assert((horizontalFiniteLine.point0() + horizontalFiniteLine.direction() * lengthOnHorizontalLine).sqrDistance(intersectionPoint) < 1);
					ocean_assert((verticalFiniteLine.point0() + verticalFiniteLine.direction() * lengthOnVerticalLine).sqrDistance(intersectionPoint) < 1);

					ocean_assert(lengthHorizontalFiniteLine > thresholdLongDistance && lengthVerticalFiniteLine > thresholdLongDistance);
					ocean_assert(lengthHorizontalFiniteLine > Scalar(2) * thresholdShortDistance && lengthVerticalFiniteLine > Scalar(2) * thresholdShortDistance);


					// short range   {++++++++}                        {++++++++}, short ranges define an area at the ends of the line
					// finite line        ----------------------------------
					// long range                    {++++++++++}                , long ranges define an area in the center of the line

					const bool validHorizontalInsideLong = lengthOnHorizontalLine >= thresholdLongDistance && lengthOnHorizontalLine <= lengthHorizontalFiniteLine - thresholdLongDistance;
					const bool validHorizontalInsideShortStart = lengthOnHorizontalLine >= -thresholdShortDistance && lengthOnHorizontalLine <= thresholdShortDistance;
					const bool validHorizontalInsideShortEnd = lengthOnHorizontalLine >= lengthHorizontalFiniteLine - thresholdShortDistance && lengthOnHorizontalLine <= lengthHorizontalFiniteLine + thresholdShortDistance;

					const bool validVerticalInsideLong = lengthOnVerticalLine >= thresholdLongDistance && lengthOnVerticalLine <= lengthVerticalFiniteLine - thresholdLongDistance;
					const bool validVerticalInsideShortStart = lengthOnVerticalLine >= -thresholdShortDistance && lengthOnVerticalLine <= thresholdShortDistance;
					const bool validVerticalInsideShortEnd = lengthOnVerticalLine >= lengthVerticalFiniteLine - thresholdShortDistance && lengthOnVerticalLine <= lengthVerticalFiniteLine + thresholdShortDistance;

					// first check whether we have a X-shape
					if (validHorizontalInsideLong && validVerticalInsideLong)
					{
						if (Numeric::abs(horizontalFiniteLine.direction() * verticalFiniteLine.direction()) <= thresholdAngleXShapeCos)
						{
							xShapes.emplace_back(nHorizontal, nVertical, intersectionPoint, horizontalFiniteLine.direction(), verticalFiniteLine.direction());
						}
					}

					// then we check for a T-shape
					else if (validHorizontalInsideLong && validVerticalInsideShortStart)
					{
						// T-shape with vertical direction and joint at the start of the vertical line
						tShapes.emplace_back(nHorizontal, nVertical, intersectionPoint, verticalFiniteLine.direction());
					}
					else if (validHorizontalInsideLong && validVerticalInsideShortEnd)
					{
						// T-shape with vertical direction and joint at the end of the vertical line
						tShapes.emplace_back(nHorizontal, nVertical, intersectionPoint, -verticalFiniteLine.direction());
					}
					else if (validHorizontalInsideShortStart && validVerticalInsideLong)
					{
						// T-shape with horizontal direction and joint at the start of the horizontal line
						tShapes.emplace_back(nHorizontal, nVertical, intersectionPoint, horizontalFiniteLine.direction());
					}
					else if (validHorizontalInsideShortEnd && validVerticalInsideLong)
					{
						// T-shape with horizontal direction and joint at the end of the horizontal line
						tShapes.emplace_back(nHorizontal, nVertical, intersectionPoint, -horizontalFiniteLine.direction());
					}

					// then we check for a L-shape
					else if ((validHorizontalInsideShortStart || validHorizontalInsideShortEnd) && (validVerticalInsideShortStart || validVerticalInsideShortEnd))
					{
						Vector2 edgeA;
						Vector2 edgeB;

						if (validHorizontalInsideShortStart && validVerticalInsideShortStart)
						{
							edgeA = horizontalFiniteLine.point1() - intersectionPoint;
							edgeB = verticalFiniteLine.point1() - intersectionPoint;
						}
						else if (validHorizontalInsideShortStart && validVerticalInsideShortEnd)
						{
							edgeA = horizontalFiniteLine.point1() - intersectionPoint;
							edgeB = verticalFiniteLine.point0() - intersectionPoint;
						}
						else if (validHorizontalInsideShortEnd && validVerticalInsideShortStart)
						{
							edgeA = horizontalFiniteLine.point0() - intersectionPoint;
							edgeB = verticalFiniteLine.point1() - intersectionPoint;
						}
						else if (validHorizontalInsideShortEnd && validVerticalInsideShortEnd)
						{
							edgeA = horizontalFiniteLine.point0() - intersectionPoint;
							edgeB = verticalFiniteLine.point0() - intersectionPoint;
						}
						else
						{
							ocean_assert(false && "This must never happen!");
						}

						if (edgeA.normalize() && edgeB.normalize())
						{
							Vector2 direction = edgeA + edgeB;

							if (direction.normalize())
							{
								const Scalar score = lengthHorizontalFiniteLine * lengthVerticalFiniteLine;

								// we need a left and a right edge
								if (edgeA.cross(edgeB) < 0)
								{
									std::swap(edgeA, edgeB);
								}

								ocean_assert(edgeA.isParallel(horizontalFiniteLine.direction()) || edgeA.isParallel(verticalFiniteLine.direction()));
								ocean_assert(edgeB.isParallel(horizontalFiniteLine.direction()) || edgeB.isParallel(verticalFiniteLine.direction()));

								lShapes.emplace_back(nHorizontal, nVertical, intersectionPoint, direction, edgeA, edgeB, score);
							}
						}
					}
				}
			}
		}
	}
}

void ShapeDetector::postAdjustShapes(const unsigned int width, const unsigned int height, LShapes& lShapes, TShapes& tShapes, XShapes& xShapes, const Scalar similarPointDistance, const Scalar similarAngle)
{
	ocean_assert(similarPointDistance >= 0);
	ocean_assert(similarAngle >= 0 && similarAngle < Numeric::pi_4());

	const Scalar similarPointDistanceSqr = Numeric::sqr(similarPointDistance);

	const Scalar similarAngleParallelCos = Numeric::cos(similarAngle);
	const Scalar similarAnglePerpendicularCos = Numeric::cos(Numeric::pi_2() - similarAngle);

	const unsigned int horizontalBins = std::max(1u, width / 5u);
	const unsigned int verticalBins = std::max(1u, height / 5u);

	Geometry::SpatialDistribution::DistributionArray distributionArray(Scalar(0), Scalar(0), Scalar(width), Scalar(height), horizontalBins, verticalBins);

	const unsigned int lShapeOffset = (unsigned int)tShapes.size();

	for (size_t n = 0; n < tShapes.size(); ++n)
	{
		const TShape& tShape = tShapes[n];

		const unsigned int xBin = distributionArray.horizontalBin(tShape.position().x());
		const unsigned int yBin = distributionArray.verticalBin(tShape.position().y());

		distributionArray(xBin, yBin).push_back(Index32(n));
	}

	for (size_t n = 0; n < lShapes.size(); ++n)
	{
		const LShape& lShape = lShapes[n];

		const unsigned int xBin = distributionArray.horizontalBin(lShape.position().x());
		const unsigned int yBin = distributionArray.verticalBin(lShape.position().y());

		distributionArray(xBin, yBin).push_back(lShapeOffset + Index32(n));
	}

#ifdef OCEAN_DEBUG
	const size_t debugInitialLShapes = lShapes.size();
	const size_t debugInitialTShapes = tShapes.size();
	const size_t debugInitialXShapes = xShapes.size();
#endif

	IndexSet32 removedTShapeIndices;
	IndexSet32 removedLShapeIndices;

	for (unsigned int yBin = 0u; yBin < distributionArray.verticalBins(); ++yBin)
	{
		for (unsigned int xBin = 0u; xBin < distributionArray.horizontalBins(); ++xBin)
		{
			const Indices32& indices = distributionArray(xBin, yBin);

			for (const Index32& index : indices)
			{
				if (index < lShapeOffset)
				{
					// we have a T-shape

					ocean_assert(index < tShapes.size());
					const TShape& tShape = tShapes[index];

					ocean_assert((unsigned int)(distributionArray.horizontalBin(tShape.position().x())) == xBin);
					ocean_assert((unsigned int)(distributionArray.verticalBin(tShape.position().y())) == yBin);

					for (unsigned int yTestBin = (unsigned int)std::max(0, int(yBin) - 1); yTestBin < std::min(yBin + 2u, distributionArray.verticalBins()); ++yTestBin)
					{
						for (unsigned int xTestBin = (unsigned int)std::max(0, int(xBin) - 1); xTestBin < std::min(xBin + 2u, distributionArray.horizontalBins()); ++xTestBin)
						{
							const Indices32& testIndices = distributionArray(xTestBin, yTestBin);

							for (const Index32& testIndex : testIndices)
							{
								if (index == testIndex)
								{
									// we need to individual shapes
									continue;
								}

								if (testIndex < lShapeOffset)
								{
									// we check whether two T-shapes can be converted to one X-shape

									ocean_assert(testIndex < tShapes.size());
									const TShape& tTestShape = tShapes[testIndex];

									ocean_assert((unsigned int)(distributionArray.horizontalBin(tTestShape.position().x())) == xTestBin);
									ocean_assert((unsigned int)(distributionArray.verticalBin(tTestShape.position().y())) == yTestBin);

									if (tShape.position().sqrDistance(tTestShape.position()) <= similarPointDistanceSqr)
									{
										// both T-shapes are close enough together

										const Scalar absAngleCos = Numeric::abs(tShape.direction() * tTestShape.direction());
										ocean_assert(Numeric::isInsideRange(0, absAngleCos, 1));

										if (absAngleCos >= similarAngleParallelCos || absAngleCos <= similarAnglePerpendicularCos)
										{
											// both T-shapes are parallel or perpendicular

											if (removedTShapeIndices.find(index) == removedTShapeIndices.cend() && removedTShapeIndices.find(testIndex) == removedTShapeIndices.cend())
											{
												xShapes.emplace_back((tShape.position() + tTestShape.position()) * Scalar(0.5), Vector2(1, 0), Vector2(0, 1) /* todo HACK */);

												removedTShapeIndices.insert(index);
												removedTShapeIndices.insert(testIndex);
											}
										}
									}
								}
								else
								{
									// we check whether one T-shape and one L-shape can be converted to one X-shape

									// **TODO** we should also handle this case
								}
							}
						}
					}
				}
				else
				{
					// we have an L-shape

					ocean_assert(index - lShapeOffset < lShapes.size());
					const LShape& lShape = lShapes[index - lShapeOffset];

					ocean_assert((unsigned int)(distributionArray.horizontalBin(lShape.position().x())) == xBin);
					ocean_assert((unsigned int)(distributionArray.verticalBin(lShape.position().y())) == yBin);

					for (unsigned int yTestBin = (unsigned int)std::max(0, int(yBin) - 1); yTestBin < std::min(yBin + 2u, distributionArray.verticalBins()); ++yTestBin)
					{
						for (unsigned int xTestBin = (unsigned int)std::max(0, int(xBin) - 1); xTestBin < std::min(xBin + 2u, distributionArray.horizontalBins()); ++xTestBin)
						{
							const Indices32& testIndices = distributionArray(xTestBin, yTestBin);

							for (const Index32& testIndex : testIndices)
							{
								if (index == testIndex)
								{
									// we need to individual shapes
									continue;
								}

								if (testIndex < lShapeOffset)
								{
									// we have check the combintation of one L-shape and one T-shape above already

									continue;
								}
								else
								{
									// we check whether two L-shapes can be converted to one T-shape, or one X-shape

									ocean_assert(testIndex - lShapeOffset < lShapes.size());
									const LShape& lTestShape = lShapes[testIndex - lShapeOffset];

									ocean_assert((unsigned int)(distributionArray.horizontalBin(lTestShape.position().x())) == xTestBin);
									ocean_assert((unsigned int)(distributionArray.verticalBin(lTestShape.position().y())) == yTestBin);

									if (lShape.position().sqrDistance(lTestShape.position()) <= similarPointDistanceSqr)
									{
										// both L-shapes are close enought toghether

										const Scalar angleCos = lShape.direction() * lTestShape.direction();

										if (angleCos <= -similarAngleParallelCos)
										{
											// we have two L-shapes with parallel but opposite directions, this is a X-shape

											if (removedLShapeIndices.find(index - lShapeOffset) == removedLShapeIndices.cend() && removedLShapeIndices.find(testIndex - lShapeOffset) == removedLShapeIndices.cend())
											{
												xShapes.emplace_back((lShape.position() + lTestShape.position()) * Scalar(0.5), Vector2(1, 0), Vector2(0, 1) /* todo HACK */);

												removedLShapeIndices.insert(index - lShapeOffset);
												removedLShapeIndices.insert(testIndex - lShapeOffset);
											}
										}
										else if (Numeric::abs(angleCos) <= similarAnglePerpendicularCos)
										{
											// we have two L-shapes with perpendicular direction, this is an L-shape

											Vector2 direction = lShape.direction() + lTestShape.direction();
											ocean_assert(Numeric::isNotEqualEps(direction.length()));

											if (direction.normalize())
											{
												if (removedLShapeIndices.find(index - lShapeOffset) == removedLShapeIndices.cend() && removedLShapeIndices.find(testIndex - lShapeOffset) == removedLShapeIndices.cend())
												{
													tShapes.emplace_back((lShape.position() + lTestShape.position()) * Scalar(0.5), direction);

													removedLShapeIndices.insert(index - lShapeOffset);
													removedLShapeIndices.insert(testIndex - lShapeOffset);
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

#ifdef OCEAN_DEBUG
	ocean_assert((removedLShapeIndices.size() + removedTShapeIndices.size()) % 2 == 0);

	const unsigned int newShapes = (unsigned int)(removedLShapeIndices.size() + removedTShapeIndices.size()) / 2u;
	ocean_assert(debugInitialLShapes + debugInitialTShapes + debugInitialXShapes + newShapes == lShapes.size() + tShapes.size() + xShapes.size());
#endif

	// now we remove L- and T-shapes which have been transformed to T- and X-shapes

	for (IndexSet32::reverse_iterator i = removedLShapeIndices.rbegin(); i != removedLShapeIndices.rend(); ++i) // note the reverse iterator
	{
		ocean_assert(*i < (unsigned int)lShapes.size());

		lShapes[*i] = lShapes.back();
		lShapes.pop_back();
	}

	for (IndexSet32::reverse_iterator i = removedTShapeIndices.rbegin(); i != removedTShapeIndices.rend(); ++i) // note the reverse iterator
	{
		ocean_assert(*i < (unsigned int)tShapes.size());

		tShapes[*i] = tShapes.back();
		tShapes.pop_back();
	}
}

ShapeDetector::LShapes ShapeDetector::nonMaximumSuppressionLShapes(const LShapes& lShapes, const unsigned int width, const unsigned int height, const Scalar thresholdDistance, const Scalar thresholdAngle)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(thresholdDistance >= 0);
	ocean_assert(thresholdAngle >= 0 && thresholdAngle <= Numeric::pi_2());

	const Scalar thresholdDistanceSqr = thresholdDistance * thresholdDistance;
	const Scalar thresholdAngleCos = Numeric::cos(thresholdAngle);

	unsigned int horizontalBins = 1u;
	unsigned int verticalBins = 1u;
	Geometry::SpatialDistribution::idealBinsNeighborhood9(width, height, thresholdDistance, horizontalBins, verticalBins);
	Geometry::SpatialDistribution::DistributionArray distributionArray(Scalar(0), Scalar(0), Scalar(width), Scalar(height), horizontalBins, verticalBins);

	for (unsigned int n = 0u; n < (unsigned int)lShapes.size(); ++n)
	{
		const LShape& lShape = lShapes[n];

		const unsigned int xBin = distributionArray.horizontalBin(lShape.position().x());
		const unsigned int yBin = distributionArray.verticalBin(lShape.position().y());

		distributionArray(xBin, yBin).push_back(n);
	}

	LShapes filteredLShapes;
	filteredLShapes.reserve(lShapes.size() / 2);

	Indices32 indices;

	for (unsigned int n = 0u; n < (unsigned int)lShapes.size(); ++n)
	{
		const LShape& lShape = lShapes[n];

		const unsigned int xBin = distributionArray.horizontalBin(lShape.position().x());
		const unsigned int yBin = distributionArray.verticalBin(lShape.position().y());

		indices.clear();
		distributionArray.indicesNeighborhood9(xBin, yBin, indices);

		bool foundBetter = false;

		for (const Index32& index : indices)
		{
			if (index != n)
			{
				if (lShape.position().sqrDistance(lShapes[index].position()) <= thresholdDistanceSqr && (lShape.direction() * lShapes[index].direction()) >= thresholdAngleCos)
				{
					// we have a similar l-shape

					if (lShapes[index].score() > lShape.score() || (lShapes[index].score() == lShape.score() && index < n))
					{
						foundBetter = true;
						break;
					}
				}
			}
		}

		if (!foundBetter)
		{
			filteredLShapes.push_back(lShape);
		}
	}

	return filteredLShapes;
}

ShapeDetector::IndexedRectangles ShapeDetector::determineAlignedRectangles(const FiniteLines2& /*finiteLines*/, const LShapes& lShapes, const Vector2& topLeftCornerDirection, const Scalar minDistanceBetweenCorners, const Scalar thresholdCornerDirectionAngle, const Scalar thresholdConnectedShapesAngle)
{
	ocean_assert(Numeric::isEqual(topLeftCornerDirection.length(), 1));

	ocean_assert(minDistanceBetweenCorners >= 0);
	ocean_assert(thresholdCornerDirectionAngle >= 0 && thresholdCornerDirectionAngle <= Numeric::pi_4());
	ocean_assert(thresholdConnectedShapesAngle >= 0 && thresholdConnectedShapesAngle <= Numeric::deg2rad(15));

	const Scalar minDistanceBetweenCornersSqr = Numeric::sqr(minDistanceBetweenCorners);

	const Scalar thresholdCornerDirectionAngleCos = Numeric::cos(thresholdCornerDirectionAngle);
	const Scalar thresholdConnectedShapesAngleCos = Numeric::cos(thresholdConnectedShapesAngle);

	/// the indices of the L-shapes separated into corners (based on 90 degree rotations), top left, bottom left, bottom right, top right
	Indices32 cornerGroups[4];

	Vector2 cornerDirection = topLeftCornerDirection;

	for (unsigned int nCorner = 0u; nCorner < 4u; ++nCorner)
	{
		for (Index32 nLShape = 0u; nLShape < Index32(lShapes.size()); ++nLShape)
		{
			if (cornerDirection * lShapes[nLShape].direction() >= thresholdCornerDirectionAngleCos)
			{
				cornerGroups[nCorner].push_back(nLShape);
			}
		}

		// counter clock wise rotation by 90
		cornerDirection = -cornerDirection.perpendicular();
	}

#ifdef OCEAN_DEBUG
	ocean_assert(cornerGroups[0].size() + cornerGroups[1].size() + cornerGroups[2].size() + cornerGroups[3].size() <= lShapes.size());

	// we ensure that we do not have any index in more than one group

	IndexSet32 debugIndexSet;
	for (unsigned int n = 0u; n < 4u; ++n)
	{
		debugIndexSet.insert(cornerGroups[n].begin(), cornerGroups[n].end());
	}

	ocean_assert(debugIndexSet.size() == cornerGroups[0].size() + cornerGroups[1].size() + cornerGroups[2].size() + cornerGroups[3].size());
#endif

	for (unsigned int n = 0u; n < 4u; ++n)
	{
		if (cornerGroups[n].empty())
		{
			return IndexedRectangles();
		}
	}

	IndexedRectangles rectangles;

	for (Index32& indexTopLeft : cornerGroups[0])
	{
		const LShape& topLeft = lShapes[indexTopLeft];

		for (Index32& indexBottomLeft : cornerGroups[1])
		{
			const LShape& bottomLeft = lShapes[indexBottomLeft];

			if (topLeft.position().sqrDistance(bottomLeft.position()) < minDistanceBetweenCornersSqr || !areLShapesConnected(topLeft, bottomLeft, topLeft.edgeRight(), bottomLeft.edgeLeft(), thresholdConnectedShapesAngleCos))
			{
				continue;
			}

			for (Index32& indexBottomRight : cornerGroups[2])
			{
				const LShape& bottomRight = lShapes[indexBottomRight];

				if (bottomLeft.position().sqrDistance(bottomRight.position()) < minDistanceBetweenCornersSqr || !areLShapesConnected(bottomLeft, bottomRight, bottomLeft.edgeRight(), bottomRight.edgeLeft(), thresholdConnectedShapesAngleCos))
				{
					continue;
				}

				for (Index32& indexTopRight : cornerGroups[3])
				{
					const LShape& topRight = lShapes[indexTopRight];

					if (bottomRight.position().sqrDistance(topRight.position()) < minDistanceBetweenCornersSqr || !areLShapesConnected(bottomRight, topRight, bottomRight.edgeRight(), topRight.edgeLeft(), thresholdConnectedShapesAngleCos))
					{
						continue;
					}

					if (topRight.position().sqrDistance(topLeft.position()) < minDistanceBetweenCornersSqr || !areLShapesConnected(topRight, topLeft, topRight.edgeRight(), topLeft.edgeLeft(), thresholdConnectedShapesAngleCos))
					{
						continue;
					}

					rectangles.push_back({{indexTopLeft, indexBottomLeft, indexBottomRight, indexTopRight}});
				}
			}
		}
	}

	return rectangles;
}

ShapeDetector::IndexedRectangles ShapeDetector::determineShapedRectangles(const FiniteLines2& /*finiteLines*/, const LShapes& lShapes, const IndexedRectangles& rectangles, const Scalar aspectRatio, const Scalar minimalWidth, const Scalar maximalWidth, const Scalar aspectRatioTolerance, const Scalar orthogonalAngleTolerance)
{
	ocean_assert(minimalWidth >= 10 && minimalWidth <= maximalWidth);
	ocean_assert(aspectRatio > Scalar(0.01) && aspectRatio < Scalar(100));
	ocean_assert(aspectRatioTolerance >= Scalar(0) && aspectRatioTolerance < Scalar(1));
	ocean_assert(orthogonalAngleTolerance >= Scalar(0) && orthogonalAngleTolerance < Numeric::pi_2());

	if (rectangles.empty())
	{
		return IndexedRectangles();
	}

	IndexedRectangles rectangleCandidates;
	rectangleCandidates.reserve(rectangles.size());

	const Scalar minimalAspectRatio = aspectRatio * (Scalar(1) - aspectRatioTolerance); // e.g., 90%
	const Scalar maximalAspectRatio = aspectRatio * (Scalar(1) + aspectRatioTolerance); // e.g., 110%

	const Scalar orthogonalAngleToleranceCos = Numeric::cos(orthogonalAngleTolerance);

	for (const IndexedRectangle& rectangleCandidate : rectangles)
	{
		const Vector2& topLeft = lShapes[rectangleCandidate[0]].position();
		const Vector2& bottomLeft = lShapes[rectangleCandidate[1]].position();
		const Vector2& bottomRight = lShapes[rectangleCandidate[2]].position();
		const Vector2& topRight = lShapes[rectangleCandidate[3]].position();

		const Scalar leftHeight = topLeft.distance(bottomLeft);
		const Scalar bottomWidth = bottomLeft.distance(bottomRight);
		const Scalar rightHeight = bottomRight.distance(topRight);
		const Scalar topWidth = topLeft.distance(topRight);

		if (topWidth >= minimalWidth && topWidth <= maximalWidth
			&& bottomWidth >= minimalWidth && bottomWidth <= maximalWidth
			&& leftHeight >= 5 && rightHeight >= 5)
		{
			const Scalar invLeftHeight = Scalar(1) / leftHeight;
			const Scalar invRightHeight = Scalar(1) / rightHeight;

			const Vector2 topDirection = (topRight - topLeft) / topWidth;
			const Vector2 leftDirection = (bottomLeft - topLeft) * invLeftHeight;
			const Vector2 bottomDirection = (bottomRight - bottomLeft) / bottomWidth;
			const Vector2 rightDirection = (bottomRight - topRight) * invRightHeight;

			ocean_assert(Numeric::isEqual(topDirection.length(), 1));
			ocean_assert(Numeric::isEqual(leftDirection.length(), 1));
			ocean_assert(Numeric::isEqual(bottomDirection.length(), 1));
			ocean_assert(Numeric::isEqual(rightDirection.length(), 1));

			const Scalar topLeftAngleCos = Numeric::abs(topDirection * leftDirection);
			const Scalar bottomLeftAngleCos = Numeric::abs(bottomDirection * leftDirection);
			const Scalar bottomRightAngleCos = Numeric::abs(bottomDirection * rightDirection);
			const Scalar topRightAngleCos = Numeric::abs(topDirection * rightDirection);

			if (topLeftAngleCos <= orthogonalAngleToleranceCos && bottomLeftAngleCos <= orthogonalAngleToleranceCos
				&& bottomRightAngleCos <= orthogonalAngleToleranceCos && topRightAngleCos <= orthogonalAngleToleranceCos)
			{
				const Scalar aspectRatioTopLeft = topWidth * invLeftHeight;
				const Scalar aspectRatioTopRight = topWidth * invRightHeight;
				const Scalar aspectRatioBottomLeft = bottomWidth * invLeftHeight;
				const Scalar aspectRatioBottomRight = bottomWidth * invRightHeight;

				if (minimalAspectRatio <= aspectRatioTopLeft && aspectRatioTopLeft <= maximalAspectRatio
					&& minimalAspectRatio <= aspectRatioTopRight && aspectRatioTopRight <= maximalAspectRatio
					&& minimalAspectRatio <= aspectRatioBottomLeft && aspectRatioBottomLeft <= maximalAspectRatio
					&& minimalAspectRatio <= aspectRatioBottomRight && aspectRatioBottomRight <= maximalAspectRatio)
				{
					rectangleCandidates.push_back(rectangleCandidate);
				}
			}
		}
	}

	return rectangleCandidates;
}

ShapeDetector::Rectangles ShapeDetector::guessShapedRectanglesFromUpperCorners(const LShapes& lShapes, const Vector2& topLeftCornerDirection, const Scalar aspectRatio, const unsigned int imageHeight, const unsigned int maxNumberOfCandidates, const Scalar thresholdCornerDirectionAngle, const Scalar thresholdConnectedShapesAngle, const Scalar minimalRectangleWidth, const Scalar maximalRectangleWidth, const unsigned int numCandidatePairsToGeneratePerEdge, const Scalar sideEdgeRatioMultiplier)
{
	ocean_assert(topLeftCornerDirection.isUnit());
	ocean_assert(thresholdCornerDirectionAngle >= 0 && thresholdCornerDirectionAngle <= Numeric::pi_4());
	ocean_assert(thresholdConnectedShapesAngle >= 0 && thresholdConnectedShapesAngle <= Numeric::deg2rad(15));
	ocean_assert(aspectRatio > Scalar(0.1) && aspectRatio < Scalar(10));
	ocean_assert(minimalRectangleWidth >= Scalar(10) && minimalRectangleWidth <= maximalRectangleWidth);
	ocean_assert(numCandidatePairsToGeneratePerEdge > 0u && numCandidatePairsToGeneratePerEdge <= 10u);
	ocean_assert(maxNumberOfCandidates > 0u);
	ocean_assert(sideEdgeRatioMultiplier >= Scalar(0.5) && sideEdgeRatioMultiplier <= Scalar(2.0));

	const Scalar minimalWidthSqr = Numeric::sqr(minimalRectangleWidth);
	const Scalar maximalWidthSqr = Numeric::sqr(maximalRectangleWidth);
	const Scalar thresholdCornerDirectionAngleCos = Numeric::cos(thresholdCornerDirectionAngle);
	const Scalar thresholdConnectedShapesAngleCos = Numeric::cos(thresholdConnectedShapesAngle);

	/// The indices of the L-shapes separated into corners (based on 90 degree rotations): top left, top right
	Indices32 cornerIndicesTopLeft;
	Indices32 cornerIndicesTopRight;
	cornerIndicesTopLeft.reserve(lShapes.size() / 2);
	cornerIndicesTopRight.reserve(lShapes.size() / 2);

	const Vector2 topRightCornerDirection = topLeftCornerDirection.perpendicular();

	for (Index32 nLShape = 0u; nLShape < Index32(lShapes.size()); ++nLShape)
	{
		if (topLeftCornerDirection * lShapes[nLShape].direction() >= thresholdCornerDirectionAngleCos)
			cornerIndicesTopLeft.emplace_back(nLShape);
		else if (topRightCornerDirection * lShapes[nLShape].direction() >= thresholdCornerDirectionAngleCos)
			cornerIndicesTopRight.emplace_back(nLShape);
	}

	// Make sure we have potential corners for both upper vertices of the rectangle
	if (cornerIndicesTopLeft.empty() || cornerIndicesTopRight.empty())
	{
		return Rectangles{};
	}

	// Generate candidates
	Rectangles rectangles;
	rectangles.reserve(lShapes.size() / 2);

	for (const Index32& indexTopLeft : cornerIndicesTopLeft)
	{
		const LShape& topLeft = lShapes[indexTopLeft];

		for (const Index32& indexTopRight : cornerIndicesTopRight)
		{
			const LShape& topRight = lShapes[indexTopRight];

			// Check the width of the rectangle
			const Scalar topWidthSqr = topLeft.position().sqrDistance(topRight.position());
			if (topWidthSqr < minimalWidthSqr || topWidthSqr > maximalWidthSqr)
			{
				continue;
			}

			// Impose thresholds on the L-shapes to connect (consistent angles)
			if (!areLShapesConnected(topLeft, topRight, topLeft.edgeLeft(), topRight.edgeRight(), thresholdConnectedShapesAngleCos))
			{
				continue;
			}

			// Generate bottom vertices based on top ones and aspect ratio
			const Vector2 topLeftToTopRightDirection = topRight.position() - topLeft.position();
			const Scalar verticalEdgeExtraLenghtFactor = Scalar(0.10) / Scalar(numCandidatePairsToGeneratePerEdge); // TODO - tune this (runtime vs accuracy)
			for (unsigned int candidatePair = 0u; candidatePair < numCandidatePairsToGeneratePerEdge; ++candidatePair)
			{
				const Scalar sideEdgeRatio = (sideEdgeRatioMultiplier + verticalEdgeExtraLenghtFactor * Scalar(candidatePair)) / aspectRatio;
				const Vector2 verticalEdge = topLeftToTopRightDirection.perpendicular() * sideEdgeRatio;
				Vector2 bottomLeftCorner = topLeft.position() + verticalEdge;
				Vector2 bottomRightCorner = topRight.position() + verticalEdge;

				// TODO - Check this condition
				if (bottomLeftCorner[1] < Scalar(imageHeight) || bottomRightCorner[1] < Scalar(imageHeight))
				{
					rectangles.emplace_back<Rectangle>({ lShapes[indexTopLeft].position(), bottomLeftCorner, bottomRightCorner, lShapes[indexTopRight].position() });
				}

				// Generate bottom vertices using the direction of the L-shape instead of the orthogonal direction to the top edge
				const Scalar verticalEdgeNorm = topLeftToTopRightDirection.length() * sideEdgeRatio;
				bottomLeftCorner = topLeft.position() + topLeft.edgeRight().normalized() * verticalEdgeNorm;
				bottomRightCorner = topRight.position() + topRight.edgeLeft().normalized() * verticalEdgeNorm;

				// TODO - Check this condition
				// TODO - Check if these corners are very close the ones used above and, if so, do not add this candidate (it would be redundant).
				if (bottomLeftCorner[1] < Scalar(imageHeight) || bottomRightCorner[1] < Scalar(imageHeight))
				{
					rectangles.emplace_back<Rectangle>({ lShapes[indexTopLeft].position(), bottomLeftCorner, bottomRightCorner, lShapes[indexTopRight].position() });
				}

				// Return if the max. number of candidates is reached
				if (rectangles.size() >= maxNumberOfCandidates)
				{
					rectangles.resize(maxNumberOfCandidates);
					return rectangles;
				}
			}
		}
	}

	return rectangles;
}

void ShapeDetector::removeLinesTooCloseToBorder(FiniteLines2& finiteLines, const unsigned int width, const unsigned int height, const Scalar thresholdDistance)
{
	ocean_assert(thresholdDistance >= 0);
	ocean_assert(Scalar(width) > thresholdDistance && Scalar(height) > thresholdDistance);

	const Scalar rightBorder = Scalar(width) - thresholdDistance;
	const Scalar bottomBorder = Scalar(height) - thresholdDistance;

	for (size_t n = 0; n < finiteLines.size(); /* noop */)
	{
		const FiniteLine2& finiteLine = finiteLines[n];

		if (finiteLine.point0().x() < thresholdDistance || finiteLine.point0().y() < thresholdDistance || finiteLine.point1().x() < thresholdDistance || finiteLine.point1().y() < thresholdDistance
				|| finiteLine.point0().x() > rightBorder || finiteLine.point0().y() > bottomBorder || finiteLine.point1().x() > rightBorder || finiteLine.point1().y() > bottomBorder)
		{
			finiteLines[n] = finiteLines.back();
			finiteLines.pop_back();
		}
		else
		{
			++n;
		}
	}
}

bool ShapeDetector::optimizeRectangleAlongEdges(const Frame& yFrame, Rectangle& rectangle, const unsigned int perpendicularSampleDistance)
{
	const FiniteLine2 leftEdge(rectangle[0], rectangle[1]);
	const FiniteLine2 bottomEdge(rectangle[1], rectangle[2]);
	const FiniteLine2 rightEdge(rectangle[2], rectangle[3]);
	const FiniteLine2 topEdge(rectangle[3], rectangle[0]);

	Line2 optimizedLeftEdge, optimizedBottomEdge, optimizedRightEdge, optimizedTopEdge;

	if (!optimizeLineAlongEdge(yFrame, leftEdge, optimizedLeftEdge, perpendicularSampleDistance) ||
		!optimizeLineAlongEdge(yFrame, bottomEdge, optimizedBottomEdge, perpendicularSampleDistance) ||
		!optimizeLineAlongEdge(yFrame, rightEdge, optimizedRightEdge, perpendicularSampleDistance) ||
		!optimizeLineAlongEdge(yFrame, topEdge, optimizedTopEdge, perpendicularSampleDistance))
	{
		return false;
	}

	if (!optimizedLeftEdge.intersection(optimizedTopEdge, rectangle[0]) ||
		!optimizedLeftEdge.intersection(optimizedBottomEdge, rectangle[1]) ||
		!optimizedBottomEdge.intersection(optimizedRightEdge, rectangle[2]) ||
		!optimizedRightEdge.intersection(optimizedTopEdge, rectangle[3]))
	{
		return false;
	}

	for (unsigned int n = 0u; n < 4u; ++n)
	{
		if (rectangle[n].x() < Scalar(0) || rectangle[n].y() < Scalar(0) || rectangle[n].x() >= Scalar(yFrame.width()) || rectangle[n].y() >= Scalar(yFrame.height()))
		{
			return false;
		}
	}

	return true;
}

bool ShapeDetector::optimizeLineAlongEdge(const Frame& yFrame, const FiniteLine2& line, Line2& optimizedLine, const unsigned int perpendicularSampleDistance, unsigned int sampleLocations, const unsigned int minimalValidSampleLocations, const Scalars& sampleLocationsPercent)
{
	ocean_assert(yFrame.isValid() && FrameType::formatIsGeneric(yFrame.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 1u));
	ocean_assert(line.isValid());

	ocean_assert(perpendicularSampleDistance >= 1u);
	ocean_assert(minimalValidSampleLocations >= 2u);

	ocean_assert((sampleLocations != 0u && sampleLocationsPercent.empty()) || (sampleLocations == 0u && !sampleLocationsPercent.empty()));

	if (sampleLocations == 0u)
	{
		ocean_assert(!sampleLocationsPercent.empty());
		sampleLocations = (unsigned int)(sampleLocationsPercent.size());
	}

	if (sampleLocations < 2u || minimalValidSampleLocations > sampleLocations)
	{
		ocean_assert(false && "Invalid parameters!");
		return false;
	}

	if (yFrame.width() <= perpendicularSampleDistance * 2u || yFrame.height() <= perpendicularSampleDistance * 2u)
	{
		return false;
	}

	const Scalar leftBorder = Scalar(perpendicularSampleDistance * 2u);
	const Scalar topBorder = Scalar(perpendicularSampleDistance * 2u);
	const Scalar rightBorder = Scalar(yFrame.width() - perpendicularSampleDistance * 2u);
	const Scalar bottomBorder = Scalar(yFrame.height() - perpendicularSampleDistance * 2u);

	const Scalar sampleArea = Scalar(0.9);

	const Scalar sampleStart = (Scalar(1) - sampleArea) * Scalar(0.5);

	const Vector2 perpendicular = line.direction().perpendicular();

	Vectors2 peakLocations;
	peakLocations.reserve(sampleLocations);

	for (unsigned int n = 0u; n < sampleLocations; ++n)
	{
		// we use the given sampling locations, or we equally distribute the sample locations along the edges

		const Scalar samplePercent = sampleLocationsPercent.empty() ? (sampleStart + Scalar(n) * sampleArea / Scalar(sampleLocations - 1u)) : sampleLocationsPercent[n];
		ocean_assert(Numeric::isInsideRange(sampleStart, samplePercent, Scalar(1) - sampleStart));

		const Vector2 sampleLocationCenter = line.point0() + (line.point1() - line.point0()) * samplePercent;
		ocean_assert(line.distance(sampleLocationCenter) <= Numeric::weakEps());

		int previousValue = -1;
		unsigned int bestDelta = 10u;
		Vector2 bestLocation(-1, -1);

		if (sampleLocationCenter.x() >= leftBorder && sampleLocationCenter.y() >= topBorder && sampleLocationCenter.x() < rightBorder && sampleLocationCenter.y() < bottomBorder)
		{
			for (int nPerpendicular = int(perpendicularSampleDistance); nPerpendicular >= -int(perpendicularSampleDistance); --nPerpendicular)
			{
				const Vector2 sampleLocation = sampleLocationCenter + perpendicular * Scalar(nPerpendicular);

				unsigned char ySampleValue;
				CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<1u, CV::PC_TOP_LEFT>(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), sampleLocation, &ySampleValue);

				if (previousValue >= 0)
				{
					const int signedDelta = int(ySampleValue) - previousValue;

					const unsigned int delta = (unsigned int)(std::abs(signedDelta));

					if (delta > bestDelta)
					{
						bestDelta = delta;
						bestLocation = sampleLocation - perpendicular * Scalar(0.5);
					}
				}

				previousValue = int(ySampleValue);
			}

			if (bestLocation.x() >= 0)
			{
				peakLocations.push_back(bestLocation);
			}
		}
	}

	if (peakLocations.size() < size_t(minimalValidSampleLocations))
	{
		return false;
	}

	RandomGenerator randomGenerator;

	Indices32 usedIndices;
	if (!Geometry::RANSAC::line(ConstArrayAccessor<Vector2>(peakLocations), randomGenerator, optimizedLine, true, 30u, Scalar(1.5 * 1.5), nullptr, &usedIndices) || usedIndices.size() < size_t(minimalValidSampleLocations))
	{
		return false;
	}

	return true;
}

} // namespace Detector

} // namespace CV

} // namespace Ocean
