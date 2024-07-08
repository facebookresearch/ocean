/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testdetector/TestShapeDetector.h"

#include "ocean/base/RandomGenerator.h"
#include "ocean/base/RandomI.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameConverterY8.h"
#include "ocean/cv/FrameFilterGradient.h"
#include "ocean/cv/IntegralImage.h"
#include "ocean/cv/NonMaximumSuppression.h"

#include "ocean/cv/detector/ShapeDetector.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

void TestShapeDetector::GradientBasedDetector::detectShapes(const Frame& yFrame, const double threshold, const double responseMultiplicationFactor, LShapes& lShapes, TShapes& tShapes, XShapes& xShapes, const int sign, const unsigned int shapeWidth, const unsigned int shapeHeight, const unsigned int shapeStepSize, const unsigned int shapeTopBand, const unsigned int shapeBottomBand, const ResponseType responseType, const double penaltyFactor, const unsigned int minimalEdgeResponse, const double nonMaximumSupressionRadius, const EdgeResponseStrategy edgeResponseStrategy, const MinimalResponseStrategy minimalResponseStrategy, const PenaltyUsage penaltyUsage, Frame* fResponseTopDown, Frame* fResponseBottomUp)
{
	ocean_assert(yFrame.isValid());
	ocean_assert(lShapes.empty() && tShapes.empty() && xShapes.empty());
	ocean_assert(sign != 0);

	ocean_assert(threshold >= 0.0);
	ocean_assert(responseMultiplicationFactor > 0.0);

	ocean_assert(shapeWidth % 2u == 1u);
	ocean_assert(shapeBottomBand * 2u + shapeStepSize < shapeWidth);
	ocean_assert(shapeBottomBand + shapeStepSize < shapeHeight);

	ocean_assert(shapeTopBand >= 1u && shapeBottomBand >= 1u);
	ocean_assert(shapeStepSize != 0u && shapeStepSize % 2u == 1u);

	Frame floatResponseFrameTopDown(FrameType(yFrame, FrameType::genericPixelFormat<double, 1u>()));
	Frame floatResponseFrameBottomUp(FrameType(yFrame, FrameType::genericPixelFormat<double, 1u>()));

	{
		// top-down

		CV::NonMaximumSuppression<double> nonMaximumSuppressionAlignedTShape(yFrame.width(), yFrame.height());

		for (unsigned int y = 0u; y < yFrame.height(); ++y)
		{
			for (unsigned int x = 0u; x < yFrame.width(); ++x)
			{
				const double response = tShapeDetectorResponse(yFrame, x, y, sign, shapeWidth, shapeHeight, shapeStepSize, shapeTopBand, shapeBottomBand, responseType, penaltyFactor, minimalEdgeResponse, edgeResponseStrategy, minimalResponseStrategy, penaltyUsage) * responseMultiplicationFactor;

				if (response >= threshold)
				{
					nonMaximumSuppressionAlignedTShape.addCandidate(x, y, response);
				}

				*floatResponseFrameTopDown.pixel<double>(x, y) = response;
			}
		}

		CV::NonMaximumSuppression<double>::StrengthPositions<unsigned int, double> shapes = nonMaximumSuppressionAlignedTShape.suppressNonMaximum<unsigned int, double, false /*tStrictMaximum*/>(1u, yFrame.width() - 2u, 1u, yFrame.height() - 2u, nullptr, nullptr);
		shapes = CV::NonMaximumSuppression<double>::suppressNonMaximum<unsigned int, double, true>(yFrame.width(), yFrame.height(), shapes, (unsigned int)(nonMaximumSupressionRadius));

		tShapes.reserve(tShapes.size() + shapes.size());

		for (size_t n = 0; n < shapes.size(); ++n)
		{
			const unsigned int& x = shapes[n].x();
			const unsigned int& y = shapes[n].y();

			ocean_assert(x >= 1u && x <= yFrame.width() - 2u && y >= 1u && y <= yFrame.height() - 2u);

			Vector2 offset(0, 0);
			CV::NonMaximumSuppression<double>::determinePrecisePeakLocation2(floatResponseFrameTopDown.constpixel<double>(x - 1u, y - 1u), floatResponseFrameTopDown.constpixel<double>(x - 1u, y + 0u), floatResponseFrameTopDown.constpixel<double>(x - 1u, y + 1u), offset);

			tShapes.emplace_back(Vector2(Scalar(x), Scalar(y)) + offset, Vector2(0, 1), Scalar(shapes[n].strength()));
		}
	}

	{
		// bottom-up

		Frame yRotatedFrame(yFrame.frameType(), yFrame.paddingElements());
		CV::FrameConverterY8::convertY8ToY8(yFrame.constdata<uint8_t>(), yRotatedFrame.data<uint8_t>(), yFrame.width(), yFrame.height(), CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, yFrame.paddingElements(), yRotatedFrame.paddingElements());

		CV::NonMaximumSuppression<double> nonMaximumSuppressionAlignedTShape(yRotatedFrame.width(), yRotatedFrame.height());

		for (unsigned int y = 0u; y < yRotatedFrame.height(); ++y)
		{
			for (unsigned int x = 0u; x < yRotatedFrame.width(); ++x)
			{
				const double response = tShapeDetectorResponse(yRotatedFrame, x, y, sign, shapeWidth, shapeHeight, shapeStepSize, shapeTopBand, shapeBottomBand, responseType, penaltyFactor, minimalEdgeResponse, edgeResponseStrategy, minimalResponseStrategy, penaltyUsage) * responseMultiplicationFactor;

				if (response >= threshold)
				{
					nonMaximumSuppressionAlignedTShape.addCandidate(x, y, response);
				}

				*floatResponseFrameBottomUp.pixel<double>(x, y) = response;
			}
		}

		CV::NonMaximumSuppression<double>::StrengthPositions<unsigned int, double> shapes = nonMaximumSuppressionAlignedTShape.suppressNonMaximum<unsigned int, double, false /*tStrictMaximum*/>(1u, yRotatedFrame.width() - 2u, 1u, yRotatedFrame.height() - 2u, nullptr, nullptr);
		shapes = CV::NonMaximumSuppression<double>::suppressNonMaximum<unsigned int, double, true>(yFrame.width(), yFrame.height(), shapes, (unsigned int)(nonMaximumSupressionRadius));

		tShapes.reserve(tShapes.size() + shapes.size());

		for (size_t n = 0; n < shapes.size(); ++n)
		{
			const unsigned int& x = shapes[n].x();
			const unsigned int& y = shapes[n].y();

			ocean_assert(x >= 1u && x <= yFrame.width() - 2u && y >= 1u && y <= yFrame.height() - 2u);

			Vector2 offset(0, 0);
			CV::NonMaximumSuppression<double>::determinePrecisePeakLocation2(floatResponseFrameBottomUp.constpixel<double>(x - 1u, y - 1u), floatResponseFrameBottomUp.constpixel<double>(x - 1u, y + 0u), floatResponseFrameBottomUp.constpixel<double>(x - 1u, y + 1u), offset);

			tShapes.emplace_back(Vector2(Scalar(yRotatedFrame.width() - x - 1u), Scalar(yRotatedFrame.height() - y - 1u)) - offset, Vector2(0, -1), Scalar(shapes[n].strength()));
		}

		Frame tmpFloatResponseFrameBottomUp(floatResponseFrameBottomUp.frameType());
		CV::FrameChannels::transformGeneric<double, 1u>(floatResponseFrameBottomUp.constdata<double>(), tmpFloatResponseFrameBottomUp.data<double>(), floatResponseFrameBottomUp.width(), floatResponseFrameBottomUp.height(), CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, floatResponseFrameBottomUp.paddingElements(), tmpFloatResponseFrameBottomUp.paddingElements(), nullptr);

		floatResponseFrameBottomUp = std::move(tmpFloatResponseFrameBottomUp);
	}

	CV::Detector::ShapeDetector::postAdjustShapes(yFrame.width(), yFrame.height(), lShapes, tShapes, xShapes);

	if (fResponseTopDown)
	{
		*fResponseTopDown = std::move(floatResponseFrameTopDown);
	}

	if (fResponseBottomUp)
	{
		*fResponseBottomUp = std::move(floatResponseFrameBottomUp);
	}
}

double TestShapeDetector::GradientBasedDetector::tShapeDetectorResponse(const Frame& yFrame, const unsigned int x, const unsigned int y, const int sign, const unsigned int shapeWidth, const unsigned int shapeHeight, const unsigned int shapeStepSize, const unsigned int shapeTopBand, const unsigned int shapeBottomBand, const ResponseType responseType, const double penaltyFactor, const unsigned int minimalEdgeResponse, const EdgeResponseStrategy edgeResponseStrategy, const MinimalResponseStrategy minimalResponseStrategy, const PenaltyUsage penaltyUsage)
{
	ocean_assert(yFrame.isValid() && yFrame.pixelFormat() == FrameType::FORMAT_Y8);

	/*
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
	 */

	ocean_assert(shapeWidth % 2u == 1u);
	ocean_assert(shapeBottomBand * 2u + shapeStepSize < shapeWidth);
	ocean_assert(shapeBottomBand + shapeStepSize < shapeHeight);

	ocean_assert(shapeTopBand >= 1u && shapeBottomBand >= 1u);
	ocean_assert(shapeStepSize != 0u && shapeStepSize % 2u == 1u);

	const unsigned int shapeWidth_2 = shapeWidth / 2u;
	const unsigned int shapeStepSize_2 = shapeStepSize / 2u;

	if (shapeWidth > yFrame.width() || shapeHeight + shapeTopBand > yFrame.height())
	{
		return 0.0;
	}

	if (x < shapeWidth_2 || y < shapeTopBand + shapeStepSize_2 || x >= yFrame.width() - shapeWidth_2 || y >= yFrame.height() - shapeHeight + shapeStepSize_2)
	{
		return 0.0;
	}

	// response for horizontal edge(s), summing up all vertical gradients along the horizontal edges

	double horizontalResponsePositive = 0.0;

	/*
	 *    |                                       |  <- first outer row:      y - shapeStepSize_2 - 1
	 *    |#######################################|  <- last inner step row:  y - shapeStepSize_2
	 *    |                   X                   |  <- shape position:       y
	 *    |                                       |
	 *    |                                       |
	 */

	unsigned int horizontalResponsePositiveCounter = 0u;
	for (unsigned int xx = x - shapeWidth_2; xx <= x + shapeWidth_2; ++xx)
	{
		if (edgeResponseStrategy == ERS_GRADIENT_TO_NEIGHBOR)
		{
			horizontalResponsePositive += double(*yFrame.constpixel<uint8_t>(xx, y - shapeStepSize_2)) - double(*yFrame.constpixel<uint8_t>(xx, y - shapeStepSize_2 - 1u));
		}
		else
		{
			ocean_assert(edgeResponseStrategy == ERS_GRADIENT_TO_CENTER);

			horizontalResponsePositive += double(*yFrame.constpixel<uint8_t>(xx, y)) - double(*yFrame.constpixel<uint8_t>(xx, y - shapeStepSize_2 - 1u));
		}

		++horizontalResponsePositiveCounter;
	}
	ocean_assert(horizontalResponsePositiveCounter == shapeWidth);

	/*
	 *                  T-shape width
	 *    |                                       |
	 *    |                   X                   | <- shape position:       y
	 *    |                                       |
	 *    |################       ################| <- last inner step row:  y + shapeStepSize_2
	 *    |               #       #               | <- first outer step row: y + shapeStepSize_2 + 1
	 *
	 *                    <------->
	 *                    shapeStepSize
	 */

	double horizontalResponseNegative = 0.0;

	unsigned int horizontalResponseNegativeCounter = 0u;
	for (unsigned int xx = x - shapeWidth_2; xx < x - shapeStepSize_2; ++xx)
	{
		if (edgeResponseStrategy == ERS_GRADIENT_TO_NEIGHBOR)
		{
			horizontalResponseNegative += double(*yFrame.constpixel<uint8_t>(xx, y + shapeStepSize_2 + 1u)) - double(*yFrame.constpixel<uint8_t>(xx, y + shapeStepSize_2));
		}
		else
		{
			ocean_assert(edgeResponseStrategy == ERS_GRADIENT_TO_CENTER);

			horizontalResponseNegative += double(*yFrame.constpixel<uint8_t>(xx, y + shapeStepSize_2 + 1u)) - double(*yFrame.constpixel<uint8_t>(xx, y));
		}

		++horizontalResponseNegativeCounter;
	}
	ocean_assert(horizontalResponseNegativeCounter == (shapeWidth - shapeStepSize) / 2u);

	for (unsigned int xx = x + shapeStepSize_2 + 1u; xx < x + shapeWidth_2 + 1u; ++xx)
	{
		if (edgeResponseStrategy == ERS_GRADIENT_TO_NEIGHBOR)
		{
			horizontalResponseNegative += double(*yFrame.constpixel<uint8_t>(xx, y + shapeStepSize_2 + 1u)) - double(*yFrame.constpixel<uint8_t>(xx, y + shapeStepSize_2));
		}
		else
		{
			ocean_assert(edgeResponseStrategy == ERS_GRADIENT_TO_CENTER);

			horizontalResponseNegative += double(*yFrame.constpixel<uint8_t>(xx, y + shapeStepSize_2 + 1u)) - double(*yFrame.constpixel<uint8_t>(xx, y));
		}

		++horizontalResponseNegativeCounter;
	}
	ocean_assert(horizontalResponseNegativeCounter == shapeWidth - shapeStepSize);

	const unsigned int horizontalResponseCounter = horizontalResponsePositiveCounter + horizontalResponseNegativeCounter;


	double horizontalResponse = horizontalResponsePositive - horizontalResponseNegative;

	if (minimalResponseStrategy == MRS_SEPARATE_HORIZONTAL_VERTICAL)
	{
		if (NumericD::abs(horizontalResponse) < double(horizontalResponseCounter * minimalEdgeResponse))
		{
			horizontalResponse = 0.0;
		}
	}
	else if (minimalResponseStrategy == MRS_SEPARATE_OPPOSITE_SIDE)
	{
		if (NumericD::abs(horizontalResponsePositive) < double(horizontalResponsePositiveCounter * minimalEdgeResponse)
			|| NumericD::abs(horizontalResponseNegative) < double(horizontalResponseNegativeCounter * minimalEdgeResponse))
		{
			horizontalResponse = 0.0;
		}
	}


	// horizontal penalty response

	double penaltyHorizontalResponse = 0.0;

	/*
	 *    |                                       |
	 *    |#######################################|
	 *    |                                       |
	 *    |                   X                   | <- shape position:       y
	 *    |                                       |
	 *    |################       ################|
	 *    |               #       #               |
	 */

	unsigned int penaltyHorizontalResponseCounter = 0u;
	for (unsigned int xx = x - shapeWidth_2; xx < x + shapeWidth_2; ++xx)
	{
		penaltyHorizontalResponse += NumericD::abs(double(*yFrame.constpixel<uint8_t>(xx + 1u, y)) - double(*yFrame.constpixel<uint8_t>(xx, y)));
		++penaltyHorizontalResponseCounter;
	}
	ocean_assert_and_suppress_unused(penaltyHorizontalResponseCounter == shapeWidth - 1u, penaltyHorizontalResponseCounter);

	penaltyHorizontalResponse *= penaltyFactor;


	// response for vertical edge(s), summing up all horizontal gradients along the vertical edges

	double verticalResponsePositive = 0.0;
	double verticalResponseNegative = 0.0;

	/*
	 *
	 *          X               <- shape position:       y
	 *
	 *   ####       ####        <- last inner step row:  y + shapeStepSize_2
	 *      #       #			<- first outer step row: y + shapeStepSize_2 + 1
	 *      #       #
	 *
	 *      ^
	 *      |
	 *   first inner:  x - shapeStepSize_2
	 */

	unsigned int verticalResponsePositiveCounter = 0u;
	unsigned int verticalResponseNegativeCounter = 0u;
	for (unsigned int yy = y + shapeStepSize_2 + 1u; yy < y + shapeHeight - shapeStepSize_2; ++yy)
	{
		if (edgeResponseStrategy == ERS_GRADIENT_TO_NEIGHBOR)
		{
			verticalResponsePositive += double(*yFrame.constpixel<uint8_t>(x - shapeStepSize_2, yy)) - double(*yFrame.constpixel<uint8_t>(x - shapeStepSize_2 - 1u, yy));
			verticalResponseNegative += double(*yFrame.constpixel<uint8_t>(x + shapeStepSize_2 + 1u, yy)) - double(*yFrame.constpixel<uint8_t>(x + shapeStepSize_2, yy));
		}
		else
		{
			ocean_assert(edgeResponseStrategy == ERS_GRADIENT_TO_CENTER);

			verticalResponsePositive += double(*yFrame.constpixel<uint8_t>(x, yy)) - double(*yFrame.constpixel<uint8_t>(x - shapeStepSize_2 - 1u, yy));
			verticalResponseNegative += double(*yFrame.constpixel<uint8_t>(x + shapeStepSize_2 + 1u, yy)) - double(*yFrame.constpixel<uint8_t>(x, yy));
		}

		++verticalResponsePositiveCounter;
		++verticalResponseNegativeCounter;
	}
	ocean_assert(verticalResponsePositiveCounter == shapeHeight - shapeStepSize);
	ocean_assert(verticalResponseNegativeCounter == shapeHeight - shapeStepSize);

	const unsigned int verticalResponseCounter = verticalResponsePositiveCounter + verticalResponseNegativeCounter;

	double verticalResponse = verticalResponsePositive - verticalResponseNegative;

	if (minimalResponseStrategy == MRS_SEPARATE_HORIZONTAL_VERTICAL)
	{
		if (NumericD::abs(verticalResponse) < double(verticalResponseCounter * minimalEdgeResponse))
		{
			verticalResponse = 0.0;
		}
	}
	else if (minimalResponseStrategy == MRS_SEPARATE_OPPOSITE_SIDE)
	{
		if (NumericD::abs(verticalResponsePositive) < double(verticalResponsePositiveCounter * minimalEdgeResponse)
				|| NumericD::abs(verticalResponseNegative) < double(verticalResponseNegativeCounter * minimalEdgeResponse))
		{
			verticalResponse = 0.0;
		}
	}

	// vertical penalty response

	double penaltyVerticalResponse = 0.0;

	/*
	 *    |#######################################| <- first inner step row:  y - shapeStepSize_2
	 *    |                                       |
	 *    |                   X                   |
	 *    |                                       |
	 *    |################       ################|
	 *    |               #       #               |
	 *    |               #       #               |
	 *     --------       #       #       --------
	 *             |      #       #      |
	 *             |      #       #      |
	 *             |      #       #      |
	 *             |      #       #      |
	 *             |      #       #      |
	 *             |      #       #      |
	 *             |      #       #      |
	 *             |      #       #      |
	 *              ---------------------           <- last inner step row:  y + shapeHeight - shapeStepSize_2 - 1
	 */

	unsigned int penaltyVerticalResponseCounter = 0u;
	for (unsigned int yy = y; yy < y + shapeHeight - shapeStepSize_2 - 1u; ++yy)
	{
		penaltyVerticalResponse += NumericD::abs(double(*yFrame.constpixel<uint8_t>(x, yy + 1u)) - double(*yFrame.constpixel<uint8_t>(x, yy)));
		++penaltyVerticalResponseCounter;
	}
	ocean_assert_and_suppress_unused(penaltyVerticalResponseCounter == shapeHeight - shapeStepSize_2 - 1u, penaltyVerticalResponseCounter);

	penaltyVerticalResponse *= penaltyFactor;


	// calculating edge response

	if ((sign < 0 && horizontalResponse > 0.0) || (sign > 0 && horizontalResponse < 0.0))
	{
		horizontalResponse = 0.0;
	}
	else
	{
		horizontalResponse = NumericD::abs(horizontalResponse);
	}


	if ((sign < 0 && verticalResponse > 0.0) || (sign > 0 && verticalResponse < 0.0))
	{
		verticalResponse = 0.0;
	}
	else
	{
		verticalResponse = NumericD::abs(verticalResponse);
	}

	if (responseType == RT_HORIZONTAL)
	{
		if (penaltyUsage == PU_SUBTRACT)
		{
			return max(0.0, horizontalResponse - penaltyHorizontalResponse);
		}
		else
		{
			ocean_assert(penaltyUsage == PU_DIVISION);

			return horizontalResponse / std::max(1.0, penaltyHorizontalResponse);
		}
	}

	if (responseType == RT_VERTICAL)
	{
		if (penaltyUsage == PU_SUBTRACT)
		{
			return max(0.0, verticalResponse - penaltyVerticalResponse);
		}
		else
		{
			ocean_assert(penaltyUsage == PU_DIVISION);

			return verticalResponse / std::max(1.0, penaltyVerticalResponse);
		}
	}

	double edgeResponse = 0.0;

	if (penaltyUsage == PU_SUBTRACT)
	{
		edgeResponse = max(0.0, horizontalResponse - penaltyHorizontalResponse) * max(0.0, verticalResponse - penaltyVerticalResponse);
	}
	else
	{
		ocean_assert(penaltyUsage == PU_DIVISION);

		edgeResponse = (horizontalResponse / std::max(1.0, penaltyHorizontalResponse)) * (verticalResponse / std::max(1.0, penaltyVerticalResponse));
	}


	// background response

	double backgroundResponse = 0.0;

	/*
	 *     ---------------------------------------    <- first top band row:  y - shapeStepSize_2 - topBand
	 *    |                                       |
	 *    |                                       |   <- last top band row:   y - shapeStepSize_2 - 1
	 *    |#######################################|
	 *    |                                       |
	 *    |                   X                   |
	 *    |                                       |
	 *    |################       ################|
	 *    |               #       #               |   <- first bottom band row: y + shapeStepSize_2 + 1
	 *    |               #       #               |
	 *     --------       #       #       --------    <- inner bottom band row: y + shapeStepSize_2 + bottomBand
	 *             |      #       #      |
	 *             |      #       #      |
	 *             |      #       #      |
	 *             |      #       #      |
	 *             |      #       #      |
	 *             |      #       #      |
	 *             |      #       #      |
	 *             |      #       #      |
	 *              ---------------------            <- last bottom band row: y + shapeHeight - shapeStepSize_2 - 1
	 *                   ^
	 *                   |
	 *                last band column:  x - shapeStepSize_2 - 1
	 */

	// top band - horizontal gradient filter

	unsigned int backgroundResponseCounterA = 0u;
	for (unsigned int yy = y - shapeStepSize_2 - shapeTopBand; yy <= y - shapeStepSize_2 - 1u; ++yy)
	{
		for (unsigned int xx = x - shapeWidth_2; xx < x + shapeWidth_2; ++xx)
		{
			backgroundResponse += NumericD::abs(double(*yFrame.constpixel<uint8_t>(xx + 1u, yy)) - double(*yFrame.constpixel<uint8_t>(xx, yy)));
			++backgroundResponseCounterA;
		}
	}
	ocean_assert_and_suppress_unused(backgroundResponseCounterA == (shapeWidth - 1u) * shapeTopBand, backgroundResponseCounterA);

	// top band - vertical gradient filter

	unsigned int backgroundResponseCounterB = 0u;
	for (unsigned int yy = y - shapeStepSize_2 - shapeTopBand; yy < y - shapeStepSize_2 - 1u; ++yy)
	{
		for (unsigned int xx = x - shapeWidth_2; xx <= x + shapeWidth_2; ++xx)
		{
			backgroundResponse += NumericD::abs(double(*yFrame.constpixel<uint8_t>(xx, yy + 1u)) - double(*yFrame.constpixel<uint8_t>(xx, yy)));
			++backgroundResponseCounterB;
		}
	}
	ocean_assert_and_suppress_unused(backgroundResponseCounterB == shapeWidth * (shapeTopBand - 1u), backgroundResponseCounterB);

	// bottom band - horizontal gradient filter (upper area)

	unsigned int backgroundResponseCounterC = 0u;
	for (unsigned int yy = y + shapeStepSize_2 + 1u; yy <= y + shapeStepSize_2 + shapeBottomBand; ++yy)
	{
		for (unsigned int xx = x - shapeWidth_2; xx < x + shapeWidth_2; ++xx)
		{
			if (xx < x - shapeStepSize_2 - 1u || xx >= x + shapeStepSize_2 + 1u)
			{
				backgroundResponse += NumericD::abs(double(*yFrame.constpixel<uint8_t>(xx + 1u, yy)) - double(*yFrame.constpixel<uint8_t>(xx, yy)));
				++backgroundResponseCounterC;
			}
		}
	}
	ocean_assert_and_suppress_unused(backgroundResponseCounterC == (shapeWidth - shapeStepSize - 2u) * shapeBottomBand, backgroundResponseCounterC);

	// bottom band - vertial gradient filter (upper area)

	unsigned int backgroundResponseCounterD = 0u;
	for (unsigned int yy = y + shapeStepSize_2 + 1u; yy < y + shapeStepSize_2 + shapeBottomBand; ++yy)
	{
		for (unsigned int xx = x - shapeWidth_2; xx <= x + shapeWidth_2; ++xx)
		{
			if (xx <= x - shapeStepSize_2 - 1u || xx >= x + shapeStepSize_2 + 1u)
			{
				backgroundResponse += NumericD::abs(double(*yFrame.constpixel<uint8_t>(xx, yy + 1u)) - double(*yFrame.constpixel<uint8_t>(xx, yy)));
				++backgroundResponseCounterD;
			}
		}
	}
	ocean_assert_and_suppress_unused(backgroundResponseCounterD == (shapeWidth - shapeStepSize) * (shapeBottomBand - 1u), backgroundResponseCounterD);

	// bottom band - horizontal gradient filter (lower area)

	unsigned int backgroundResponseCounterE = 0u;
	for (unsigned int yy = y + shapeStepSize_2 + 1u + shapeBottomBand; yy <= y + shapeHeight - shapeStepSize_2 - 1u; ++yy)
	{
		for (unsigned int xx = x - shapeStepSize_2 - shapeBottomBand; xx < x + shapeStepSize_2 + shapeBottomBand; ++xx)
		{
			if (xx < x - shapeStepSize_2 - 1u || xx >= x + shapeStepSize_2 + 1u)
			{
				backgroundResponse += NumericD::abs(double(*yFrame.constpixel<uint8_t>(xx + 1u, yy)) - double(*yFrame.constpixel<uint8_t>(xx, yy)));
				++backgroundResponseCounterE;
			}
		}
	}
	ocean_assert_and_suppress_unused(backgroundResponseCounterE == (shapeBottomBand - 1u) * (shapeHeight - shapeBottomBand - shapeStepSize) * 2u, backgroundResponseCounterE);

	// bottom band - vertial gradient filter (lower area)

	unsigned int backgroundResponseCounterF = 0u;
	for (unsigned int yy = y + shapeStepSize_2 + shapeBottomBand; yy < y + shapeHeight - shapeStepSize_2 - 1u; ++yy)
	{
		for (unsigned int xx = x - shapeStepSize_2 - shapeBottomBand; xx <= x + shapeStepSize_2 + shapeBottomBand; ++xx)
		{
			if (xx <= x - shapeStepSize_2 - 1u || xx >= x + shapeStepSize_2 + 1u)
			{
				backgroundResponse += NumericD::abs(double(*yFrame.constpixel<uint8_t>(xx, yy + 1u)) - double(*yFrame.constpixel<uint8_t>(xx, yy)));
				++backgroundResponseCounterF;
			}
		}
	}
	ocean_assert_and_suppress_unused(backgroundResponseCounterF == shapeBottomBand * (shapeHeight - shapeBottomBand - shapeStepSize) * 2u, backgroundResponseCounterF);

	const double result = edgeResponse / double(std::max(1.0, backgroundResponse));

	return result;
}

void TestShapeDetector::VarianceBasedDetector::detectShapes(const Frame& yFrame, const double threshold, const double responseMultiplicationFactor, LShapes& lShapes, TShapes& tShapes, XShapes& xShapes, const unsigned int shapeWidth, const unsigned int shapeHeight, const unsigned int shapeStepSize, const unsigned int shapeTopBand, const unsigned int shapeBottomBand, const ResponseType responseType, const double minimalGradient, const double varianceFactor, const double minimalVariance, const double maximalRatio, const double nonMaximumSupressionRadius, const ThresholdStrategy thresholdStrategy, const GradientResponseStrategy gradientResponseStrategy, const BandStrategy bandStrategy, Frame* fResponseTopDown, Frame* fResponseBottomUp)
{
	ocean_assert(yFrame.isValid());
	ocean_assert(lShapes.empty() && tShapes.empty() && xShapes.empty());

	ocean_assert(threshold >= 0.0);
	ocean_assert(responseMultiplicationFactor > 0.0);

	ocean_assert(shapeWidth % 2u == 1u);
	ocean_assert(shapeBottomBand * 2u + shapeStepSize < shapeWidth);
	ocean_assert(shapeBottomBand + shapeStepSize < shapeHeight);

	ocean_assert(shapeTopBand >= 1u && shapeBottomBand >= 1u);
	ocean_assert(shapeStepSize != 0u && shapeStepSize % 2u == 1u);

	ocean_assert(minimalGradient >= 0.0);
	ocean_assert(minimalVariance >= 1.0);

	Frame linedIntegral(FrameType(yFrame.width() + 1u, yFrame.height() + 1u, FrameType::genericPixelFormat<uint32_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT));
	Frame linedIntegralSquared(FrameType(yFrame.width() + 1u, yFrame.height() + 1u, FrameType::genericPixelFormat<uint64_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT));

	Frame floatResponseFrameTopDown(FrameType(yFrame, FrameType::genericPixelFormat<double, 1u>()));
	Frame floatResponseFrameBottomUp(FrameType(yFrame, FrameType::genericPixelFormat<double, 1u>()));

	{
		// top-down

		CV::IntegralImage::createLinedImage<uint8_t, uint32_t, 1u>(yFrame.constdata<uint8_t>(), linedIntegral.data<uint32_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), linedIntegral.paddingElements());
		CV::IntegralImage::createLinedImageSquared<uint8_t, uint64_t, 1u>(yFrame.constdata<uint8_t>(), linedIntegralSquared.data<uint64_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), linedIntegralSquared.paddingElements());

		CV::NonMaximumSuppression<double> nonMaximumSuppressionAlignedTShape(yFrame.width(), yFrame.height());

		for (unsigned int y = 0u; y < yFrame.height(); ++y)
		{
			for (unsigned int x = 0u; x < yFrame.width(); ++x)
			{
				ocean_assert(linedIntegral.isContinuous() && linedIntegralSquared.isContinuous());

				const double response = tShapeDetectorResponse(linedIntegral.constdata<uint32_t>(), linedIntegralSquared.constdata<uint64_t>(), yFrame.width(), yFrame.height(), x, y, shapeWidth, shapeHeight, shapeStepSize, shapeTopBand, shapeBottomBand, responseType, minimalGradient, varianceFactor, minimalVariance, maximalRatio, gradientResponseStrategy, bandStrategy) * responseMultiplicationFactor;

				if (response >= threshold)
				{
					nonMaximumSuppressionAlignedTShape.addCandidate(x, y, response);
				}

				*floatResponseFrameTopDown.pixel<double>(x, y) = response;
			}
		}

		CV::NonMaximumSuppression<double>::StrengthPositions<unsigned int, double> shapes = nonMaximumSuppressionAlignedTShape.suppressNonMaximum<unsigned int, double, false /*tStrictMaximum*/>(1u, yFrame.width() - 2u, 1u, yFrame.height() - 2u, nullptr, nullptr);
		shapes = CV::NonMaximumSuppression<double>::suppressNonMaximum<unsigned int, double, true>(yFrame.width(), yFrame.height(), shapes, (unsigned int)(nonMaximumSupressionRadius));

		tShapes.reserve(tShapes.size() + shapes.size());

		for (size_t n = 0; n < shapes.size(); ++n)
		{
			const unsigned int& x = shapes[n].x();
			const unsigned int& y = shapes[n].y();

			ocean_assert(x >= 1u && x <= yFrame.width() - 2u && y >= 1u && y <= yFrame.height() - 2u);

			Vector2 offset(0, 0);
			CV::NonMaximumSuppression<double>::determinePrecisePeakLocation2(floatResponseFrameTopDown.constpixel<double>(x - 1u, y - 1u), floatResponseFrameTopDown.constpixel<double>(x - 1u, y + 0u), floatResponseFrameTopDown.constpixel<double>(x - 1u, y + 1u), offset);

			tShapes.emplace_back(Vector2(Scalar(x), Scalar(y)) + offset, Vector2(0, 1), Scalar(shapes[n].strength()));
		}
	}

	{
		// bottom-up

		Frame yRotatedFrame(yFrame.frameType(), yFrame.paddingElements());
		CV::FrameConverterY8::convertY8ToY8(yFrame.constdata<uint8_t>(), yRotatedFrame.data<uint8_t>(), yFrame.width(), yFrame.height(), CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, yFrame.paddingElements(), yRotatedFrame.paddingElements());

		CV::IntegralImage::createLinedImage<uint8_t, uint32_t, 1u>(yRotatedFrame.constdata<uint8_t>(), linedIntegral.data<uint32_t>(), yRotatedFrame.width(), yRotatedFrame.height(), yRotatedFrame.paddingElements(), linedIntegral.paddingElements());
		CV::IntegralImage::createLinedImageSquared<uint8_t, uint64_t, 1u>(yRotatedFrame.constdata<uint8_t>(), linedIntegralSquared.data<uint64_t>(), yRotatedFrame.width(), yRotatedFrame.height(), yRotatedFrame.paddingElements(), linedIntegralSquared.paddingElements());

		CV::NonMaximumSuppression<double> nonMaximumSuppressionAlignedTShape(yRotatedFrame.width(), yRotatedFrame.height());

		for (unsigned int y = 0u; y < yRotatedFrame.height(); ++y)
		{
			for (unsigned int x = 0u; x < yRotatedFrame.width(); ++x)
			{
				ocean_assert(linedIntegral.isContinuous() && linedIntegralSquared.isContinuous());

				const double response = tShapeDetectorResponse(linedIntegral.constdata<uint32_t>(), linedIntegralSquared.constdata<uint64_t>(), yRotatedFrame.width(), yRotatedFrame.height(), x, y, shapeWidth, shapeHeight, shapeStepSize, shapeTopBand, shapeBottomBand, responseType, minimalGradient, varianceFactor, minimalVariance, maximalRatio, gradientResponseStrategy, bandStrategy) * responseMultiplicationFactor;

				if (response >= threshold)
				{
					nonMaximumSuppressionAlignedTShape.addCandidate(x, y, response);
				}

				*floatResponseFrameBottomUp.pixel<double>(x, y) = response;
			}
		}

		CV::NonMaximumSuppression<double>::StrengthPositions<unsigned int, double> shapes = nonMaximumSuppressionAlignedTShape.suppressNonMaximum<unsigned int, double, false /*tStrictMaximum*/>(1u, yRotatedFrame.width() - 2u, 1u, yRotatedFrame.height() - 2u, nullptr, nullptr);
		shapes = CV::NonMaximumSuppression<double>::suppressNonMaximum<unsigned int, double, true>(yFrame.width(), yFrame.height(), shapes, (unsigned int)(nonMaximumSupressionRadius));

		tShapes.reserve(tShapes.size() + shapes.size());

		for (size_t n = 0; n < shapes.size(); ++n)
		{
			const unsigned int& x = shapes[n].x();
			const unsigned int& y = shapes[n].y();

			ocean_assert(x >= 1u && x <= yFrame.width() - 2u && y >= 1u && y <= yFrame.height() - 2u);

			Vector2 offset(0, 0);
			CV::NonMaximumSuppression<double>::determinePrecisePeakLocation2(floatResponseFrameBottomUp.constpixel<double>(x - 1u, y - 1u), floatResponseFrameBottomUp.constpixel<double>(x - 1u, y + 0u), floatResponseFrameBottomUp.constpixel<double>(x - 1u, y + 1u), offset);

			tShapes.emplace_back(Vector2(Scalar(yRotatedFrame.width() - x - 1u), Scalar(yRotatedFrame.height() - y - 1u)) - offset, Vector2(0, -1), Scalar(shapes[n].strength()));
		}

		Frame tmpFloatResponseFrameBottomUp(floatResponseFrameBottomUp.frameType());
		CV::FrameChannels::transformGeneric<double, 1u>(floatResponseFrameBottomUp.constdata<double>(), tmpFloatResponseFrameBottomUp.data<double>(), floatResponseFrameBottomUp.width(), floatResponseFrameBottomUp.height(), CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, floatResponseFrameBottomUp.paddingElements(), tmpFloatResponseFrameBottomUp.paddingElements(), nullptr);

		floatResponseFrameBottomUp = std::move(tmpFloatResponseFrameBottomUp);
	}

	if (thresholdStrategy == TS_BASED_ON_TOP_100_65 || thresholdStrategy == TS_BASED_ON_TOP_75_55)
	{
		const size_t decisionShapeIndex = thresholdStrategy == TS_BASED_ON_TOP_100_65 ? 100 : 75;
		const double percent = thresholdStrategy == TS_BASED_ON_TOP_100_65 ? 0.65 : 0.55;

		if (tShapes.size() > decisionShapeIndex)
		{
			std::sort(tShapes.begin(), tShapes.end(), compareTshapes);

			const double adjustedThreshold = double(tShapes[decisionShapeIndex].score()) * percent;

			for (size_t n = decisionShapeIndex + 1; n < tShapes.size(); ++n)
			{
				if (double(tShapes[n].score()) < adjustedThreshold)
				{
					tShapes.resize(n);
					break;
				}
			}
		}
	}

	CV::Detector::ShapeDetector::postAdjustShapes(yFrame.width(), yFrame.height(), lShapes, tShapes, xShapes);

	if (fResponseTopDown)
	{
		*fResponseTopDown = std::move(floatResponseFrameTopDown);
	}

	if (fResponseBottomUp)
	{
		*fResponseBottomUp = std::move(floatResponseFrameBottomUp);
	}
}

double TestShapeDetector::VarianceBasedDetector::tShapeDetectorResponse(const uint32_t* linedIntegral, const uint64_t* linedIntegralSquared, const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y, const unsigned int shapeWidth, const unsigned int shapeHeight, const unsigned int shapeStepSize, const unsigned int shapeTopBand, const unsigned int shapeBottomBand, const ResponseType responseType, const double minimalGradient, const double varianceFactor, const double minimalVariance, const double maximalRatio, const GradientResponseStrategy gradientResponseStrategy, const BandStrategy bandStrategy)
{
	ocean_assert(shapeWidth % 2u == 1u);
	ocean_assert(shapeBottomBand * 2u + shapeStepSize < shapeWidth);
	ocean_assert(shapeBottomBand + shapeStepSize < shapeHeight);

	ocean_assert(shapeTopBand >= 1u && shapeBottomBand >= 1u);
	ocean_assert(shapeStepSize != 0u && shapeStepSize % 2u == 1u);

	ocean_assert(minimalGradient >= 0.0);
	ocean_assert(minimalVariance >= 1.0);

	const unsigned int shapeWidth_2 = shapeWidth / 2u;
	const unsigned int shapeStepSize_2 = shapeStepSize / 2u;

	if (shapeWidth > width || shapeHeight + shapeTopBand > height)
	{
		return 0.0;
	}

	if (x < shapeWidth_2 || y < shapeTopBand + shapeStepSize_2 || x >= width - shapeWidth_2 || y >= height - shapeHeight + shapeStepSize_2)
	{
		return 0.0;
	}

	const unsigned int linedIntegralStrideElements = width + 1u;
	const unsigned int linedIntegralSquaredStrideElements = width + 1u;


	// horizontal response

	const unsigned int xHorizontalStartA = x - shapeWidth_2;
	const unsigned int xHorizontalStartB = x + shapeStepSize_2 + 1u;

	double roofBandVariance = -1.0;

	if (bandStrategy == BS_SKIP)
	{
		roofBandVariance = 0.0;
	}
	else if (bandStrategy == BS_JOINED)
	{
		roofBandVariance = CV::IntegralImage::linedIntegralVariance<uint32_t, uint64_t, double>(linedIntegral, linedIntegralSquared, linedIntegralStrideElements, linedIntegralSquaredStrideElements, xHorizontalStartA, y - shapeStepSize_2 - shapeTopBand, shapeWidth, shapeTopBand, xHorizontalStartA, y + shapeStepSize_2 + 1u, shapeWidth_2 - shapeStepSize_2, shapeBottomBand, xHorizontalStartB, y + shapeStepSize_2 + 1u, shapeWidth_2 - shapeStepSize_2, shapeBottomBand);
	}
	else
	{
		const double roofBandVarianceA = CV::IntegralImage::linedIntegralVariance<uint32_t, uint64_t, double>(linedIntegral, linedIntegralSquared, linedIntegralStrideElements, linedIntegralSquaredStrideElements, xHorizontalStartA, y - shapeStepSize_2 - shapeTopBand, shapeWidth, shapeTopBand);
		const double roofBandVarianceB = CV::IntegralImage::linedIntegralVariance<uint32_t, uint64_t, double>(linedIntegral, linedIntegralSquared, linedIntegralStrideElements, linedIntegralSquaredStrideElements, xHorizontalStartA, y + shapeStepSize_2 + 1u, shapeWidth_2 - shapeStepSize_2, shapeBottomBand);
		const double roofBandVarianceC = CV::IntegralImage::linedIntegralVariance<uint32_t, uint64_t, double>(linedIntegral, linedIntegralSquared, linedIntegralStrideElements, linedIntegralSquaredStrideElements, xHorizontalStartB, y + shapeStepSize_2 + 1u, shapeWidth_2 - shapeStepSize_2, shapeBottomBand);

		if (bandStrategy == BS_SEPARATE_AVERAGE)
		{
			roofBandVariance = (roofBandVarianceA + roofBandVarianceB + roofBandVarianceC) / 3.0;
		}
		else
		{
			ocean_assert(bandStrategy == BS_SEPARATE_MAX);

			roofBandVariance = std::max(roofBandVarianceA, std::max(roofBandVarianceB, roofBandVarianceC));
		}
	}

	ocean_assert(roofBandVariance >= 0.0);

	// center row

	double roofVariance = 0.0;
	double lastRoofMean = NumericD::minValue();

	double roofResponse = 0.0;

	unsigned int debugRoofCounterOneBlock = 0u;
	unsigned int debugRoofCounterTwoBlocks = 0u;

	for (unsigned int yHorizontalStart = y - shapeStepSize_2 - 1u; yHorizontalStart <= y + shapeStepSize_2 + 1u; ++yHorizontalStart)
	{
		double mean = -1.0;
		double variance = 1.0;

		if (yHorizontalStart <= y + shapeStepSize_2)
		{
			variance = CV::IntegralImage::linedIntegralVariance<uint32_t, uint64_t, double, true>(linedIntegral, linedIntegralSquared, linedIntegralStrideElements, linedIntegralSquaredStrideElements, xHorizontalStartA, yHorizontalStart, shapeWidth, 1u, &mean);

			++debugRoofCounterOneBlock;
		}
		else
		{
			variance = CV::IntegralImage::linedIntegralVariance<uint32_t, uint64_t, double, true>(linedIntegral, linedIntegralSquared, linedIntegralStrideElements, linedIntegralSquaredStrideElements, xHorizontalStartA, yHorizontalStart, shapeWidth_2 - shapeStepSize_2, 1u, xHorizontalStartB, yHorizontalStart, shapeWidth_2 - shapeStepSize_2, 1u, &mean);

			++debugRoofCounterTwoBlocks;
		}

		ocean_assert(mean >= 0.0);
		ocean_assert(variance >= 0.0);

		roofVariance += variance;

		if (lastRoofMean >= 0.0)
		{
			if (gradientResponseStrategy == GRS_MAX_ABSOLUTE_DIFFERENCE)
			{
				roofResponse = std::max(roofResponse, NumericD::abs(mean - lastRoofMean));
			}
			else
			{
				ocean_assert(gradientResponseStrategy == GRS_SUM_ABSOLUTE_DIFFERENCES);
				roofResponse += NumericD::abs(mean - lastRoofMean);
			}
		}

		lastRoofMean = mean;
	}

	ocean_assert_and_suppress_unused(debugRoofCounterOneBlock == shapeStepSize + 1u, debugRoofCounterOneBlock);
	ocean_assert_and_suppress_unused(debugRoofCounterTwoBlocks == 1u, debugRoofCounterTwoBlocks);

	if (roofResponse < double(minimalGradient))
	{
		roofResponse = 0.0;
	}


	const double horizontalResponse = (roofResponse * roofResponse / std::max(minimalVariance, roofVariance * varianceFactor + roofBandVariance));

	if (responseType == RT_HORIZONTAL)
	{
		return horizontalResponse;
	}



	// vertical response

	const unsigned int yVerticalStart = y + shapeStepSize_2 + 1u;

	double trunkBandVariance = -1.0;

	if (bandStrategy == BS_SKIP)
	{
		trunkBandVariance = 0.0;
	}
	else if (bandStrategy == BS_JOINED)
	{
		trunkBandVariance = CV::IntegralImage::linedIntegralVariance<uint32_t, uint64_t, double>(linedIntegral, linedIntegralSquared, linedIntegralStrideElements, linedIntegralSquaredStrideElements, x - shapeStepSize_2 - shapeBottomBand, yVerticalStart, shapeBottomBand, shapeHeight - shapeStepSize, x + shapeStepSize_2 + 1u, yVerticalStart, shapeBottomBand, shapeHeight - shapeStepSize);
	}
	else
	{
		const double trunkBandVarianceA = CV::IntegralImage::linedIntegralVariance<uint32_t, uint64_t, double>(linedIntegral, linedIntegralSquared, linedIntegralStrideElements, linedIntegralSquaredStrideElements, x - shapeStepSize_2 - shapeBottomBand, yVerticalStart, shapeBottomBand, shapeHeight - shapeStepSize);
		const double trunkBandVarianceB = CV::IntegralImage::linedIntegralVariance<uint32_t, uint64_t, double>(linedIntegral, linedIntegralSquared, linedIntegralStrideElements, linedIntegralSquaredStrideElements, x + shapeStepSize_2 + 1u, yVerticalStart, shapeBottomBand, shapeHeight - shapeStepSize);

		if (bandStrategy == BS_SEPARATE_AVERAGE)
		{
			trunkBandVariance = (trunkBandVarianceA + trunkBandVarianceB) * 0.5;
		}
		else
		{
			ocean_assert(bandStrategy == BS_SEPARATE_MAX);

			trunkBandVariance = std::max(trunkBandVarianceA, trunkBandVarianceB);
		}
	}

	ocean_assert(trunkBandVariance >= 0.0);

	// center columns

	double trunkVariance = 0.0;
	double lastTrunkMean = NumericD::minValue();

	double trunkResponse = 0.0;

	unsigned int debugTrunkCounterShortBlock = 0u;
	unsigned int debugTrunkCounterLongBlock = 0u;

	for (unsigned int xVerticalStart = x - shapeStepSize_2 - 1u; xVerticalStart <= x + shapeStepSize_2 + 1u; ++xVerticalStart)
	{
		double mean = -1.0;
		double variance = -1.0;

		if (xVerticalStart >= x - shapeStepSize_2 && xVerticalStart <= x + shapeStepSize_2)
		{
			variance = CV::IntegralImage::linedIntegralVariance<uint32_t, uint64_t, double, true>(linedIntegral, linedIntegralSquared, linedIntegralStrideElements, linedIntegralSquaredStrideElements, xVerticalStart, y, 1u, shapeHeight - shapeStepSize_2, &mean);

			++debugTrunkCounterLongBlock;
		}
		else
		{
			variance = CV::IntegralImage::linedIntegralVariance<uint32_t, uint64_t, double, true>(linedIntegral, linedIntegralSquared, linedIntegralStrideElements, linedIntegralSquaredStrideElements, xVerticalStart, yVerticalStart, 1u, shapeHeight - shapeStepSize, &mean);

			++debugTrunkCounterShortBlock;
		}

		ocean_assert(mean >= 0.0);
		ocean_assert(variance >= 0.0);

		trunkVariance += variance;

		if (lastTrunkMean >= 0.0)
		{
			if (gradientResponseStrategy == GRS_MAX_ABSOLUTE_DIFFERENCE)
			{
				trunkResponse = std::max(trunkResponse, NumericD::abs(mean - lastTrunkMean));
			}
			else
			{
				ocean_assert(gradientResponseStrategy == GRS_SUM_ABSOLUTE_DIFFERENCES);
				trunkResponse += NumericD::abs(mean - lastTrunkMean);
			}
		}

		lastTrunkMean = mean;
	}

	ocean_assert_and_suppress_unused(debugTrunkCounterShortBlock == 2u, debugTrunkCounterShortBlock);
	ocean_assert_and_suppress_unused(debugTrunkCounterLongBlock == shapeStepSize, debugTrunkCounterLongBlock);

	if (trunkResponse < double(minimalGradient))
	{
		trunkResponse = 0.0;
	}

	const double verticalResponse = (trunkResponse * trunkResponse / std::max(minimalVariance, trunkVariance * varianceFactor + trunkBandVariance));

	if (responseType == RT_VERTICAL)
	{
		return verticalResponse;
	}

	ocean_assert(responseType == RT_HORIZONTAL_AND_VERTICAL);

	if (maximalRatio > 1.0)
	{
		if (horizontalResponse < NumericD::eps() || verticalResponse < NumericD::eps())
		{
			return 0.0;
		}

		if (horizontalResponse / verticalResponse > maximalRatio * maximalRatio || verticalResponse / horizontalResponse > maximalRatio * maximalRatio)
		{
			return 0.0;
		}
	}

	return horizontalResponse * verticalResponse;
}

void TestShapeDetector::GradientVarianceBasedDetector::detectShapes(const Frame& yFrame, const double threshold, const double responseMultiplicationFactor, LShapes& lShapes, TShapes& tShapes, XShapes& xShapes, const int sign, const unsigned int shapeWidth, const unsigned int shapeHeight, const unsigned int shapeStepSize, const unsigned int shapeTopBand, const unsigned int shapeBottomBand, const ResponseType responseType, const double minimalGradient, const double maximalResponseRatio, const BandStrategy bandStrategy, const OptimizationStrategy optimizationStrategy, const double nonMaximumSupressionRadius, Frame* fResponseTopDown, Frame* fResponseBottomUp)
{
	ocean_assert(yFrame.isValid());
	ocean_assert(lShapes.empty() && tShapes.empty() && xShapes.empty());

	ocean_assert(threshold >= 0.0);
	ocean_assert(responseMultiplicationFactor > 0.0);

	ocean_assert(sign >= -1 && sign <= 1);

	ocean_assert(shapeWidth % 2u == 1u);
	ocean_assert(shapeBottomBand * 2u + shapeStepSize <= shapeWidth);
	ocean_assert(shapeBottomBand + shapeStepSize <= shapeHeight);

	ocean_assert(shapeTopBand >= 1u && shapeBottomBand >= 1u);
	ocean_assert(shapeStepSize != 0u && shapeStepSize % 2u == 1u);

	ocean_assert(minimalGradient >= 0.0);
	ocean_assert(maximalResponseRatio >= 1.0);

	Frame linedIntegral(FrameType(yFrame.width() + 1u, yFrame.height() + 1u, FrameType::genericPixelFormat<uint32_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT));
	Frame linedIntegralSquared(FrameType(yFrame.width() + 1u, yFrame.height() + 1u, FrameType::genericPixelFormat<uint64_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT));

	Frame floatResponseFrameTopDown(FrameType(yFrame, FrameType::genericPixelFormat<double, 1u>()));
	Frame floatResponseFrameBottomUp(FrameType(yFrame, FrameType::genericPixelFormat<double, 1u>()));

	{
		// top-down

		CV::IntegralImage::createLinedImage<uint8_t, uint32_t, 1u>(yFrame.constdata<uint8_t>(), linedIntegral.data<uint32_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), linedIntegral.paddingElements());
		CV::IntegralImage::createLinedImageSquared<uint8_t, uint64_t, 1u>(yFrame.constdata<uint8_t>(), linedIntegralSquared.data<uint64_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), linedIntegralSquared.paddingElements());

		CV::NonMaximumSuppression<double> nonMaximumSuppressionAlignedTShape(yFrame.width(), yFrame.height());

		for (unsigned int y = 0u; y < yFrame.height(); ++y)
		{
			for (unsigned int x = 0u; x < yFrame.width(); ++x)
			{
				ocean_assert(linedIntegral.isContinuous() && linedIntegralSquared.isContinuous());

				const double response = tShapeDetectorResponse(linedIntegral.constdata<uint32_t>(), linedIntegralSquared.constdata<uint64_t>(), yFrame.width(), yFrame.height(), x, y, sign, shapeWidth, shapeHeight, shapeStepSize, shapeTopBand, shapeBottomBand, responseType, minimalGradient, maximalResponseRatio, bandStrategy, optimizationStrategy) * responseMultiplicationFactor;

				if (response >= threshold)
				{
					nonMaximumSuppressionAlignedTShape.addCandidate(x, y, response);
				}

				*floatResponseFrameTopDown.pixel<double>(x, y) = response;
			}
		}

		CV::NonMaximumSuppression<double>::StrengthPositions<unsigned int, double> shapes = nonMaximumSuppressionAlignedTShape.suppressNonMaximum<unsigned int, double, false /*tStrictMaximum*/>(1u, yFrame.width() - 2u, 1u, yFrame.height() - 2u, nullptr, nullptr);
		shapes = CV::NonMaximumSuppression<double>::suppressNonMaximum<unsigned int, double, true>(yFrame.width(), yFrame.height(), shapes, (unsigned int)(nonMaximumSupressionRadius));

		tShapes.reserve(tShapes.size() + shapes.size());

		for (size_t n = 0; n < shapes.size(); ++n)
		{
			const unsigned int& x = shapes[n].x();
			const unsigned int& y = shapes[n].y();

			ocean_assert(x >= 1u && x <= yFrame.width() - 2u && y >= 1u && y <= yFrame.height() - 2u);

			Vector2 offset(0, 0);
			CV::NonMaximumSuppression<double>::determinePrecisePeakLocation2(floatResponseFrameTopDown.constpixel<double>(x - 1u, y - 1u), floatResponseFrameTopDown.constpixel<double>(x - 1u, y + 0u), floatResponseFrameTopDown.constpixel<double>(x - 1u, y + 1u), offset);

			tShapes.emplace_back(Vector2(Scalar(x), Scalar(y)) + offset, Vector2(0, 1), Scalar(shapes[n].strength()));
		}
	}

	{
		// bottom-up

		Frame yRotatedFrame(yFrame.frameType(), yFrame.paddingElements());
		CV::FrameConverterY8::convertY8ToY8(yFrame.constdata<uint8_t>(), yRotatedFrame.data<uint8_t>(), yFrame.width(), yFrame.height(), CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, yFrame.paddingElements(), yRotatedFrame.paddingElements());

		CV::IntegralImage::createLinedImage<uint8_t, uint32_t, 1u>(yRotatedFrame.constdata<uint8_t>(), linedIntegral.data<uint32_t>(), yRotatedFrame.width(), yRotatedFrame.height(), yRotatedFrame.paddingElements(), linedIntegral.paddingElements());
		CV::IntegralImage::createLinedImageSquared<uint8_t, uint64_t, 1u>(yRotatedFrame.constdata<uint8_t>(), linedIntegralSquared.data<uint64_t>(), yRotatedFrame.width(), yRotatedFrame.height(), yRotatedFrame.paddingElements(), linedIntegralSquared.paddingElements());

		CV::NonMaximumSuppression<double> nonMaximumSuppressionAlignedTShape(yRotatedFrame.width(), yRotatedFrame.height());

		for (unsigned int y = 0u; y < yRotatedFrame.height(); ++y)
		{
			for (unsigned int x = 0u; x < yRotatedFrame.width(); ++x)
			{
				ocean_assert(linedIntegral.isContinuous() && linedIntegralSquared.isContinuous());

				const double response = tShapeDetectorResponse(linedIntegral.constdata<uint32_t>(), linedIntegralSquared.constdata<uint64_t>(), yRotatedFrame.width(), yRotatedFrame.height(), x, y, sign, shapeWidth, shapeHeight, shapeStepSize, shapeTopBand, shapeBottomBand, responseType, minimalGradient, maximalResponseRatio, bandStrategy, optimizationStrategy) * responseMultiplicationFactor;

				if (response >= threshold)
				{
					nonMaximumSuppressionAlignedTShape.addCandidate(x, y, response);
				}

				*floatResponseFrameBottomUp.pixel<double>(x, y) = response;
			}
		}

		CV::NonMaximumSuppression<double>::StrengthPositions<unsigned int, double> shapes = nonMaximumSuppressionAlignedTShape.suppressNonMaximum<unsigned int, double, false /*tStrictMaximum*/>(1u, yRotatedFrame.width() - 2u, 1u, yRotatedFrame.height() - 2u, nullptr, nullptr);
		shapes = CV::NonMaximumSuppression<double>::suppressNonMaximum<unsigned int, double, true>(yFrame.width(), yFrame.height(), shapes, (unsigned int)(nonMaximumSupressionRadius));

		tShapes.reserve(tShapes.size() + shapes.size());

		for (size_t n = 0; n < shapes.size(); ++n)
		{
			const unsigned int& x = shapes[n].x();
			const unsigned int& y = shapes[n].y();

			ocean_assert(x >= 1u && x <= yFrame.width() - 2u && y >= 1u && y <= yFrame.height() - 2u);

			Vector2 offset(0, 0);
			CV::NonMaximumSuppression<double>::determinePrecisePeakLocation2(floatResponseFrameBottomUp.constpixel<double>(x - 1u, y - 1u), floatResponseFrameBottomUp.constpixel<double>(x - 1u, y + 0u), floatResponseFrameBottomUp.constpixel<double>(x - 1u, y + 1u), offset);

			tShapes.emplace_back(Vector2(Scalar(yRotatedFrame.width() - x - 1u), Scalar(yRotatedFrame.height() - y - 1u)) - offset, Vector2(0, -1), Scalar(shapes[n].strength()));
		}

		Frame tmpFloatResponseFrameBottomUp(floatResponseFrameBottomUp.frameType());
		CV::FrameChannels::transformGeneric<double, 1u>(floatResponseFrameBottomUp.constdata<double>(), tmpFloatResponseFrameBottomUp.data<double>(), floatResponseFrameBottomUp.width(), floatResponseFrameBottomUp.height(), CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, floatResponseFrameBottomUp.paddingElements(), tmpFloatResponseFrameBottomUp.paddingElements(), nullptr);

		floatResponseFrameBottomUp = std::move(tmpFloatResponseFrameBottomUp);
	}

	CV::Detector::ShapeDetector::postAdjustShapes(yFrame.width(), yFrame.height(), lShapes, tShapes, xShapes);

	if (fResponseTopDown)
	{
		*fResponseTopDown = std::move(floatResponseFrameTopDown);
	}

	if (fResponseBottomUp)
	{
		*fResponseBottomUp = std::move(floatResponseFrameBottomUp);
	}
}

void TestShapeDetector::GradientVarianceBasedDetector::detectShapesModified(const Frame& yFrame, const double threshold, const double responseMultiplicationFactor, LShapes& lShapes, TShapes& tShapes, XShapes& xShapes, const int sign, const unsigned int shapeWidth, const unsigned int shapeHeight, const unsigned int shapeStepSize, const unsigned int shapeTopBand, const unsigned int shapeBottomBand, const ResponseType responseType, const double minimalGradient, const double maximalResponseRatio, const BandStrategy bandStrategy, const double nonMaximumSupressionRadius, Frame* fResponseTopDown, Frame* fResponseBottomUp)
{
	ocean_assert(yFrame.isValid());
	ocean_assert(lShapes.empty() && tShapes.empty() && xShapes.empty());

	ocean_assert(threshold >= 0.0);
	ocean_assert(responseMultiplicationFactor > 0.0);

	ocean_assert(sign >= -1 && sign <= 1);

	ocean_assert(shapeWidth % 2u == 1u);
	ocean_assert(shapeBottomBand + shapeStepSize < shapeWidth);
	ocean_assert(shapeBottomBand + shapeStepSize < shapeHeight);

	ocean_assert(shapeTopBand >= 1u && shapeBottomBand >= 1u);
	ocean_assert(shapeStepSize != 0u && shapeStepSize % 2u == 1u);

	ocean_assert(minimalGradient >= 0.0);
	ocean_assert(maximalResponseRatio >= 1.0);

	Frame linedIntegral(FrameType(yFrame.width() + 1u, yFrame.height() + 1u, FrameType::genericPixelFormat<uint32_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT));
	Frame linedIntegralSquared(FrameType(yFrame.width() + 1u, yFrame.height() + 1u, FrameType::genericPixelFormat<uint64_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT));

	Frame floatResponseFrameTopDown(FrameType(yFrame, FrameType::genericPixelFormat<double, 1u>()));
	Frame floatResponseFrameBottomUp(FrameType(yFrame, FrameType::genericPixelFormat<double, 1u>()));

	{
		// top-down

		CV::IntegralImage::createLinedImage<uint8_t, uint32_t, 1u>(yFrame.constdata<uint8_t>(), linedIntegral.data<uint32_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), linedIntegral.paddingElements());
		CV::IntegralImage::createLinedImageSquared<uint8_t, uint64_t, 1u>(yFrame.constdata<uint8_t>(), linedIntegralSquared.data<uint64_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), linedIntegralSquared.paddingElements());

		CV::NonMaximumSuppression<double> nonMaximumSuppressionAlignedTShape(yFrame.width(), yFrame.height());

		for (unsigned int y = 0u; y < yFrame.height(); ++y)
		{
			for (unsigned int x = 0u; x < yFrame.width(); ++x)
			{
				ocean_assert(linedIntegral.isContinuous() && linedIntegralSquared.isContinuous());

				const double response = tShapeDetectorResponseModified(linedIntegral.constdata<uint32_t>(), linedIntegralSquared.constdata<uint64_t>(), yFrame.width(), yFrame.height(), x, y, sign, shapeWidth, shapeHeight, shapeStepSize, shapeTopBand, shapeBottomBand, responseType, minimalGradient, maximalResponseRatio, bandStrategy) * responseMultiplicationFactor;

				if (response >= threshold)
				{
					nonMaximumSuppressionAlignedTShape.addCandidate(x, y, response);
				}

				*floatResponseFrameTopDown.pixel<double>(x, y) = response;
			}
		}

		CV::NonMaximumSuppression<double>::StrengthPositions<unsigned int, double> shapes = nonMaximumSuppressionAlignedTShape.suppressNonMaximum<unsigned int, double, false /*tStrictMaximum*/>(1u, yFrame.width() - 2u, 1u, yFrame.height() - 2u, nullptr, nullptr);
		shapes = CV::NonMaximumSuppression<double>::suppressNonMaximum<unsigned int, double, true>(yFrame.width(), yFrame.height(), shapes, (unsigned int)(nonMaximumSupressionRadius));

		tShapes.reserve(tShapes.size() + shapes.size());

		for (size_t n = 0; n < shapes.size(); ++n)
		{
			const unsigned int& x = shapes[n].x();
			const unsigned int& y = shapes[n].y();

			ocean_assert(x >= 1u && x <= yFrame.width() - 2u && y >= 1u && y <= yFrame.height() - 2u);

			Vector2 offset(0, 0);
			CV::NonMaximumSuppression<double>::determinePrecisePeakLocation2(floatResponseFrameTopDown.constpixel<double>(x - 1u, y - 1u), floatResponseFrameTopDown.constpixel<double>(x - 1u, y + 0u), floatResponseFrameTopDown.constpixel<double>(x - 1u, y + 1u), offset);

			tShapes.emplace_back(Vector2(Scalar(x), Scalar(y)) + offset, Vector2(0, 1), Scalar(shapes[n].strength()));
		}
	}

	{
		// bottom-up

		Frame yRotatedFrame(yFrame.frameType(), yFrame.paddingElements());
		CV::FrameConverterY8::convertY8ToY8(yFrame.constdata<uint8_t>(), yRotatedFrame.data<uint8_t>(), yFrame.width(), yFrame.height(), CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, yFrame.paddingElements(), yRotatedFrame.paddingElements());

		CV::IntegralImage::createLinedImage<uint8_t, uint32_t, 1u>(yRotatedFrame.constdata<uint8_t>(), linedIntegral.data<uint32_t>(), yRotatedFrame.width(), yRotatedFrame.height(), yRotatedFrame.paddingElements(), linedIntegral.paddingElements());
		CV::IntegralImage::createLinedImageSquared<uint8_t, uint64_t, 1u>(yRotatedFrame.constdata<uint8_t>(), linedIntegralSquared.data<uint64_t>(), yRotatedFrame.width(), yRotatedFrame.height(), yRotatedFrame.paddingElements(), linedIntegralSquared.paddingElements());

		CV::NonMaximumSuppression<double> nonMaximumSuppressionAlignedTShape(yRotatedFrame.width(), yRotatedFrame.height());

		for (unsigned int y = 0u; y < yRotatedFrame.height(); ++y)
		{
			for (unsigned int x = 0u; x < yRotatedFrame.width(); ++x)
			{
				ocean_assert(linedIntegral.isContinuous() && linedIntegralSquared.isContinuous());

				const double response = tShapeDetectorResponseModified(linedIntegral.constdata<uint32_t>(), linedIntegralSquared.constdata<uint64_t>(), yRotatedFrame.width(), yRotatedFrame.height(), x, y, sign, shapeWidth, shapeHeight, shapeStepSize, shapeTopBand, shapeBottomBand, responseType, minimalGradient, maximalResponseRatio, bandStrategy) * responseMultiplicationFactor;

				if (response >= threshold)
				{
					nonMaximumSuppressionAlignedTShape.addCandidate(x, y, response);
				}

				*floatResponseFrameBottomUp.pixel<double>(x, y) = response;
			}
		}

		CV::NonMaximumSuppression<double>::StrengthPositions<unsigned int, double> shapes = nonMaximumSuppressionAlignedTShape.suppressNonMaximum<unsigned int, double, false /*tStrictMaximum*/>(1u, yRotatedFrame.width() - 2u, 1u, yRotatedFrame.height() - 2u, nullptr, nullptr);
		shapes = CV::NonMaximumSuppression<double>::suppressNonMaximum<unsigned int, double, true>(yFrame.width(), yFrame.height(), shapes, (unsigned int)(nonMaximumSupressionRadius));

		tShapes.reserve(tShapes.size() + shapes.size());

		for (size_t n = 0; n < shapes.size(); ++n)
		{
			const unsigned int& x = shapes[n].x();
			const unsigned int& y = shapes[n].y();

			ocean_assert(x >= 1u && x <= yFrame.width() - 2u && y >= 1u && y <= yFrame.height() - 2u);

			Vector2 offset(0, 0);
			CV::NonMaximumSuppression<double>::determinePrecisePeakLocation2(floatResponseFrameBottomUp.constpixel<double>(x - 1u, y - 1u), floatResponseFrameBottomUp.constpixel<double>(x - 1u, y + 0u), floatResponseFrameBottomUp.constpixel<double>(x - 1u, y + 1u), offset);

			tShapes.emplace_back(Vector2(Scalar(yRotatedFrame.width() - x - 1u), Scalar(yRotatedFrame.height() - y - 1u)) - offset, Vector2(0, -1), Scalar(shapes[n].strength()));
		}

		Frame tmpFloatResponseFrameBottomUp(floatResponseFrameBottomUp.frameType());
		CV::FrameChannels::transformGeneric<double, 1u>(floatResponseFrameBottomUp.constdata<double>(), tmpFloatResponseFrameBottomUp.data<double>(), floatResponseFrameBottomUp.width(), floatResponseFrameBottomUp.height(), CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, floatResponseFrameBottomUp.paddingElements(), tmpFloatResponseFrameBottomUp.paddingElements(), nullptr);

		floatResponseFrameBottomUp = std::move(tmpFloatResponseFrameBottomUp);
	}

	CV::Detector::ShapeDetector::postAdjustShapes(yFrame.width(), yFrame.height(), lShapes, tShapes, xShapes);

	if (fResponseTopDown != nullptr)
	{
		*fResponseTopDown = std::move(floatResponseFrameTopDown);
	}

	if (fResponseBottomUp != nullptr)
	{
		*fResponseBottomUp = std::move(floatResponseFrameBottomUp);
	}
}

double TestShapeDetector::GradientVarianceBasedDetector::tShapeDetectorResponse(const uint32_t* linedIntegral, const uint64_t* linedIntegralSquared, const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y, const int sign, const unsigned int shapeWidth, const unsigned int shapeHeight, const unsigned int shapeStepSize, const unsigned int shapeTopBand, const unsigned int shapeBottomBand, const ResponseType responseType, const double minimalGradient, const double maximalResponseRatio, const BandStrategy bandStrategy, const OptimizationStrategy optimizationStrategy, double* horizontalResponse, double* verticalResponse)
{
	ocean_assert(shapeWidth % 2u == 1u);
	ocean_assert(shapeBottomBand * 2u + shapeStepSize <= shapeWidth);
	ocean_assert(shapeBottomBand + shapeStepSize <= shapeHeight);

	ocean_assert(shapeTopBand >= 1u && shapeBottomBand >= 1u);
	ocean_assert(shapeStepSize != 0u && shapeStepSize % 2u == 1u);

	ocean_assert(minimalGradient >= 0.0);
	ocean_assert(maximalResponseRatio >= 1.0);

	ocean_assert(sign >= -1 && sign <= 1);

	const unsigned int shapeWidth_2 = shapeWidth / 2u;
	const unsigned int shapeStepSize_2 = shapeStepSize / 2u;

	if (shapeWidth > width || shapeHeight + shapeTopBand > height)
	{
		return 0.0;
	}

	if (x < shapeWidth_2 || y < shapeTopBand + shapeStepSize_2 || x >= width - shapeWidth_2 || y >= height - shapeHeight + shapeStepSize_2)
	{
		return 0.0;
	}

	const unsigned int linedIntegralStrideElements = width + 1u;
	const unsigned int linedIntegralSquaredStrideElements = width + 1u;


	// horizontal response

	const unsigned int xHorizontalStartLeft = x - shapeWidth_2;
	const unsigned int xHorizontalStartRight = x + shapeStepSize_2 + 1u;

	const unsigned int yHorizontalBottom = y + shapeStepSize_2 + 1u;

	double centerMean = -1.0;
	const double centerVariance = CV::IntegralImage::linedIntegralVariance<uint32_t, uint64_t, double, true>(linedIntegral, linedIntegralSquared, linedIntegralStrideElements, linedIntegralSquaredStrideElements, xHorizontalStartLeft, y, shapeWidth, 1u, &centerMean);

	double topMean = -1.0;
	double bottomMean = -1.0;
	double topBottomVariance = -1.0;

	switch (optimizationStrategy)
	{
		case OS_NONE:
		{
			// top is one block
			CV::IntegralImage::linedIntegralVariance<uint32_t, uint64_t, double, true>(linedIntegral, linedIntegralSquared, linedIntegralStrideElements, linedIntegralSquaredStrideElements, xHorizontalStartLeft, y - shapeStepSize_2 - shapeTopBand, shapeWidth, shapeTopBand, &topMean);

			// bottom are two blocks
			CV::IntegralImage::linedIntegralVariance<uint32_t, uint64_t, double, true>(linedIntegral, linedIntegralSquared, linedIntegralStrideElements, linedIntegralSquaredStrideElements, xHorizontalStartLeft, yHorizontalBottom, shapeWidth_2 - shapeStepSize_2, shapeBottomBand, xHorizontalStartRight, yHorizontalBottom, shapeWidth_2 - shapeStepSize_2, shapeBottomBand, &bottomMean);

			// variance across the one top block and the two bottom blocks
			topBottomVariance = CV::IntegralImage::linedIntegralVariance<uint32_t, uint64_t, double>(linedIntegral, linedIntegralSquared, linedIntegralStrideElements, linedIntegralSquaredStrideElements, xHorizontalStartLeft, yHorizontalBottom, shapeWidth_2 - shapeStepSize_2, shapeBottomBand, xHorizontalStartRight, yHorizontalBottom, shapeWidth_2 - shapeStepSize_2, shapeBottomBand, xHorizontalStartLeft, y - shapeStepSize_2 - shapeTopBand, shapeWidth, shapeTopBand);

			break;
		}

		case OS_SYMMETRIC_RESPONSES_FOUR_HORIZONTAL_SAME_VERTICAL:
		case OS_SYMMETRIC_RESPONSES_FOUR_HORIZONTAL_DIFFERENT_VERTICAL:
		{
			ocean_assert(shapeTopBand == shapeBottomBand);

			// top are two blocks
			CV::IntegralImage::linedIntegralVariance<uint32_t, uint64_t, double, true>(linedIntegral, linedIntegralSquared, linedIntegralStrideElements, linedIntegralSquaredStrideElements, xHorizontalStartLeft, y - shapeStepSize_2 - shapeTopBand, shapeWidth_2 - shapeStepSize_2, shapeTopBand, xHorizontalStartRight, y - shapeStepSize_2 - shapeTopBand, shapeWidth_2 - shapeStepSize_2, shapeTopBand, &topMean);

			// bottom are two blocks
			CV::IntegralImage::linedIntegralVariance<uint32_t, uint64_t, double, true>(linedIntegral, linedIntegralSquared, linedIntegralStrideElements, linedIntegralSquaredStrideElements, xHorizontalStartLeft, yHorizontalBottom, shapeWidth_2 - shapeStepSize_2, shapeBottomBand, xHorizontalStartRight, yHorizontalBottom, shapeWidth_2 - shapeStepSize_2, shapeBottomBand, &bottomMean);

			// variance across the two top blocks and the two bottom blocks

			const double topBottomSum = double(CV::IntegralImage::linedIntegralSum<uint32_t>(linedIntegral, linedIntegralStrideElements, xHorizontalStartLeft, y - shapeStepSize_2 - shapeTopBand, shapeWidth_2 - shapeStepSize_2, shapeTopBand)
												+ CV::IntegralImage::linedIntegralSum<uint32_t>(linedIntegral, linedIntegralStrideElements, xHorizontalStartRight, y - shapeStepSize_2 - shapeTopBand, shapeWidth_2 - shapeStepSize_2, shapeTopBand)
												+ CV::IntegralImage::linedIntegralSum<uint32_t>(linedIntegral, linedIntegralStrideElements, xHorizontalStartLeft, yHorizontalBottom, shapeWidth_2 - shapeStepSize_2, shapeBottomBand)
												+ CV::IntegralImage::linedIntegralSum<uint32_t>(linedIntegral, linedIntegralStrideElements, xHorizontalStartRight, yHorizontalBottom, shapeWidth_2 - shapeStepSize_2, shapeBottomBand));

			const double topBottomSquaredSum = double(CV::IntegralImage::linedIntegralSum<uint64_t>(linedIntegralSquared, linedIntegralSquaredStrideElements, xHorizontalStartLeft, y - shapeStepSize_2 - shapeTopBand, shapeWidth_2 - shapeStepSize_2, shapeTopBand)
													+ CV::IntegralImage::linedIntegralSum<uint64_t>(linedIntegralSquared, linedIntegralSquaredStrideElements, xHorizontalStartRight, y - shapeStepSize_2 - shapeTopBand, shapeWidth_2 - shapeStepSize_2, shapeTopBand)
													+ CV::IntegralImage::linedIntegralSum<uint64_t>(linedIntegralSquared, linedIntegralSquaredStrideElements, xHorizontalStartLeft, yHorizontalBottom, shapeWidth_2 - shapeStepSize_2, shapeBottomBand)
													+ CV::IntegralImage::linedIntegralSum<uint64_t>(linedIntegralSquared, linedIntegralSquaredStrideElements, xHorizontalStartRight, yHorizontalBottom, shapeWidth_2 - shapeStepSize_2, shapeBottomBand));

			const double topBottomSize = double((shapeWidth_2 - shapeStepSize_2) * shapeTopBand * 2u + (shapeWidth_2 - shapeStepSize_2) * shapeBottomBand * 2u);

			topBottomVariance = (topBottomSquaredSum - (topBottomSum * topBottomSum) / topBottomSize) / topBottomSize;

			break;
		}

		case OS_SYMMETRIC_RESPONSES_TWO_HORIZONTAL_SAME_VERTICAL:
		case OS_SYMMETRIC_RESPONSES_TWO_HORIZONTAL_DIFFERENT_VERTICAL:
		{

			ocean_assert(shapeTopBand == shapeBottomBand);

			// top is one block
			CV::IntegralImage::linedIntegralVariance<uint32_t, uint64_t, double, true>(linedIntegral, linedIntegralSquared, linedIntegralStrideElements, linedIntegralSquaredStrideElements, xHorizontalStartLeft, y - shapeStepSize_2 - shapeTopBand, shapeWidth, shapeTopBand, &topMean);

			// bottom is one block
			CV::IntegralImage::linedIntegralVariance<uint32_t, uint64_t, double, true>(linedIntegral, linedIntegralSquared, linedIntegralStrideElements, linedIntegralSquaredStrideElements, xHorizontalStartLeft, yHorizontalBottom, shapeWidth, shapeBottomBand, &bottomMean);

			// variance across the top block and the bottom blocs
			topBottomVariance = CV::IntegralImage::linedIntegralVariance<uint32_t, uint64_t, double>(linedIntegral, linedIntegralSquared, linedIntegralStrideElements, linedIntegralSquaredStrideElements, xHorizontalStartLeft, y - shapeStepSize_2 - shapeTopBand, shapeWidth, shapeTopBand, xHorizontalStartLeft, yHorizontalBottom, shapeWidth, shapeBottomBand);

			break;
		}

		default:
		{
			ocean_assert(false && "Invalid optimization strategy!");
			break;
		}
	}

	ocean_assert(topMean >= 0.0);
	ocean_assert(centerMean >= 0.0);
	ocean_assert(bottomMean >= 0.0);

	ocean_assert(centerVariance >= 0.0 && centerVariance < 256.0 * 256.0);
	ocean_assert(topBottomVariance >= 0.0 && topBottomVariance < 256.0 * 256.0);

	const double centerDeviation = NumericD::sqrt(centerVariance);
	const double topBottomDeviation = NumericD::sqrt(topBottomVariance);

	double topGradient = centerMean - topMean;
	double bottomGradient = bottomMean - centerMean;

	double internalHorizontalResponse = topGradient - bottomGradient;

	bool setHorizontalResponseZero = false;

	const double signInternalHorizontalResponse = NumericD::sign(internalHorizontalResponse);

	internalHorizontalResponse = NumericD::abs(internalHorizontalResponse);

	const double horizontalDeviation = (topBottomDeviation + centerDeviation) * 0.5;

	if (bandStrategy == BS_DIVIDE)
	{
		internalHorizontalResponse /= std::max(1.0, horizontalDeviation);
	}
	else if (bandStrategy == BS_SUBTRACT_AND_DIVIDE)
	{
		internalHorizontalResponse = std::max(0.0, internalHorizontalResponse - horizontalDeviation) / std::max(1.0, horizontalDeviation);
	}

	topGradient = NumericD::abs(topGradient);
	bottomGradient = NumericD::abs(bottomGradient);

	if (topGradient < double(minimalGradient) || bottomGradient < double(minimalGradient))
	{
		setHorizontalResponseZero = true;
	}



	// vertical response

	const unsigned int xVerticalLeft = x - shapeStepSize_2 - shapeBottomBand;
	const unsigned int xVerticalRight = x + shapeStepSize_2 + 1u;

	const unsigned int yVerticalTop = y + shapeStepSize_2 + 1u;

	double leftMean = -1.0;
	double rightMean = -1.0;
	double leftRightVariance = -1.0;

	switch (optimizationStrategy)
	{
		case OS_NONE:
		case OS_SYMMETRIC_RESPONSES_FOUR_HORIZONTAL_SAME_VERTICAL:
		case OS_SYMMETRIC_RESPONSES_TWO_HORIZONTAL_SAME_VERTICAL:
		{
			CV::IntegralImage::linedIntegralVariance<uint32_t, uint64_t, double, true>(linedIntegral, linedIntegralSquared, linedIntegralStrideElements, linedIntegralSquaredStrideElements, xVerticalLeft, yVerticalTop, shapeBottomBand, shapeHeight - shapeStepSize, &leftMean);
			CV::IntegralImage::linedIntegralVariance<uint32_t, uint64_t, double, true>(linedIntegral, linedIntegralSquared, linedIntegralStrideElements, linedIntegralSquaredStrideElements, xVerticalRight, yVerticalTop, shapeBottomBand, shapeHeight - shapeStepSize, &rightMean);

			leftRightVariance = CV::IntegralImage::linedIntegralVariance<uint32_t, uint64_t, double>(linedIntegral, linedIntegralSquared, linedIntegralStrideElements, linedIntegralSquaredStrideElements, xVerticalLeft, yVerticalTop, shapeBottomBand, shapeHeight - shapeStepSize, xVerticalRight, yVerticalTop, shapeBottomBand, shapeHeight - shapeStepSize);

			break;
		}

		case OS_SYMMETRIC_RESPONSES_FOUR_HORIZONTAL_DIFFERENT_VERTICAL:
		case OS_SYMMETRIC_RESPONSES_TWO_HORIZONTAL_DIFFERENT_VERTICAL:
		{
			CV::IntegralImage::linedIntegralVariance<uint32_t, uint64_t, double, true>(linedIntegral, linedIntegralSquared, linedIntegralStrideElements, linedIntegralSquaredStrideElements, xVerticalLeft, yVerticalTop, shapeBottomBand, shapeHeight - shapeStepSize - shapeStepSize_2 - 1u, &leftMean);
			CV::IntegralImage::linedIntegralVariance<uint32_t, uint64_t, double, true>(linedIntegral, linedIntegralSquared, linedIntegralStrideElements, linedIntegralSquaredStrideElements, xVerticalRight, yVerticalTop, shapeBottomBand, shapeHeight - shapeStepSize - shapeStepSize_2 - 1u, &rightMean);

			leftRightVariance = CV::IntegralImage::linedIntegralVariance<uint32_t, uint64_t, double>(linedIntegral, linedIntegralSquared, linedIntegralStrideElements, linedIntegralSquaredStrideElements, xVerticalLeft, yVerticalTop, shapeBottomBand, shapeHeight - shapeStepSize - shapeStepSize_2 - 1u, xVerticalRight, yVerticalTop, shapeBottomBand, shapeHeight - shapeStepSize - shapeStepSize_2 - 1u);

			break;
		}

		default:
		{
			ocean_assert(false && "Invlaid optimization stragegy!");
			break;
		}
	}

	double middleMean = -1.0;
	double middleVariance = -1.0;

	switch (optimizationStrategy)
	{
		case OS_NONE:
		case OS_SYMMETRIC_RESPONSES_FOUR_HORIZONTAL_DIFFERENT_VERTICAL:
		case OS_SYMMETRIC_RESPONSES_TWO_HORIZONTAL_DIFFERENT_VERTICAL:
		{
			middleVariance = CV::IntegralImage::linedIntegralVariance<uint32_t, uint64_t, double, true>(linedIntegral, linedIntegralSquared, linedIntegralStrideElements, linedIntegralSquaredStrideElements, x, y, 1u, shapeHeight - shapeStepSize_2, &middleMean);

			break;
		}

		case OS_SYMMETRIC_RESPONSES_FOUR_HORIZONTAL_SAME_VERTICAL:
		case OS_SYMMETRIC_RESPONSES_TWO_HORIZONTAL_SAME_VERTICAL:
		{
			middleVariance = CV::IntegralImage::linedIntegralVariance<uint32_t, uint64_t, double, true>(linedIntegral, linedIntegralSquared, linedIntegralStrideElements, linedIntegralSquaredStrideElements, x, yVerticalTop, 1u, shapeHeight - shapeStepSize, &middleMean);

			break;
		}

		default:
		{
			ocean_assert(false && "Invlaid optimization stragegy!");
			break;
		}
	}

	ocean_assert(leftMean >= 0.0);
	ocean_assert(middleMean >= 0.0);
	ocean_assert(rightMean >= 0.0);

	ocean_assert(middleVariance >= 0.0 && middleVariance < 256.0 * 256.0);
	ocean_assert(leftRightVariance >= 0.0 && leftRightVariance < 256.0 * 256.0);

	const double middleDeviation = NumericD::sqrt(middleVariance);

	const double leftRightDeviation = NumericD::sqrt(leftRightVariance);

	double leftGradient = middleMean - leftMean;
	double rightGradient = rightMean - middleMean;

	double internalVerticalResponse = leftGradient - rightGradient;

	bool setVerticalResponseZero = false;

	const double signInternalVerticalResponse = NumericD::sign(internalVerticalResponse);

	internalVerticalResponse = NumericD::abs(internalVerticalResponse);

	const double verticalDeviation = (leftRightDeviation + middleDeviation) * 0.5;

	if (bandStrategy == BS_DIVIDE)
	{
		internalVerticalResponse /= std::max(1.0, verticalDeviation);
	}
	else if (bandStrategy == BS_SUBTRACT_AND_DIVIDE)
	{
		internalVerticalResponse = std::max(0.0, internalVerticalResponse - verticalDeviation) / std::max(1.0, verticalDeviation);
	}

	leftGradient = NumericD::abs(leftGradient);
	rightGradient = NumericD::abs(rightGradient);

	if (leftGradient < double(minimalGradient) || rightGradient < double(minimalGradient))
	{
		setVerticalResponseZero = true;
	}


	if (sign == 0)
	{
		// we accept any sign, however the sign of horizontal and vertical reponse must be identical

		if (signInternalHorizontalResponse != signInternalVerticalResponse)
		{
			setHorizontalResponseZero = true;
			setVerticalResponseZero = true;
		}
	}
	else
	{
		if (signInternalHorizontalResponse != double(sign))
		{
			setHorizontalResponseZero = true;
		}

		if (signInternalVerticalResponse != double(sign))
		{
			setVerticalResponseZero = true;
		}
	}


	if (setHorizontalResponseZero)
	{
		internalHorizontalResponse = 0.0;
	}

	if (setVerticalResponseZero)
	{
		internalVerticalResponse = 0.0;
	}


	if (horizontalResponse)
	{
		*horizontalResponse = internalHorizontalResponse;
	}

	if (verticalResponse)
	{
		*verticalResponse = internalVerticalResponse;
	}

	if (responseType == RT_HORIZONTAL)
	{
		return internalHorizontalResponse;
	}

	if (responseType == RT_VERTICAL)
	{
		return internalVerticalResponse;
	}


	// combine horizontal and vertical response

	ocean_assert(responseType == RT_HORIZONTAL_AND_VERTICAL);

	if (maximalResponseRatio > 1.0)
	{
		if (internalHorizontalResponse < NumericD::eps() || internalVerticalResponse < NumericD::eps())
		{
			return 0.0;
		}

		if (internalHorizontalResponse > maximalResponseRatio * internalVerticalResponse || internalVerticalResponse > maximalResponseRatio * internalHorizontalResponse)
		{
			return 0.0;
		}
	}

	double response = internalHorizontalResponse * internalVerticalResponse;

	return response;
}

double TestShapeDetector::GradientVarianceBasedDetector::tShapeDetectorResponseModified(const uint32_t* linedIntegral, const uint64_t* linedIntegralSquared, const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y, const int sign, const unsigned int shapeWidth, const unsigned int shapeHeight, const unsigned int shapeStepSize, const unsigned int shapeTopBand, const unsigned int shapeBottomBand, const ResponseType responseType, const double minimalGradient, const double maximalResponseRatio, const BandStrategy bandStrategy, double* horizontalResponse, double* verticalResponse)
{
	ocean_assert(shapeWidth % 2u == 1u);
	ocean_assert(shapeBottomBand + shapeStepSize < shapeWidth);
	ocean_assert(shapeBottomBand + shapeStepSize < shapeHeight);

	ocean_assert(shapeTopBand >= 1u && shapeBottomBand >= 1u);
	ocean_assert(shapeStepSize != 0u && shapeStepSize % 2u == 1u);

	ocean_assert(minimalGradient >= 0.0);
	ocean_assert(maximalResponseRatio >= 1.0);

	ocean_assert(sign >= -1 && sign <= 1);

	const unsigned int shapeWidth_2 = shapeWidth / 2u;
	const unsigned int shapeStepSize_2 = shapeStepSize / 2u;

	if (shapeWidth > width || shapeHeight + shapeTopBand > height)
	{
		return 0.0;
	}

	if (x < shapeWidth_2 || y < shapeTopBand + shapeStepSize_2 || x >= width - shapeWidth_2 || y >= height - shapeHeight + shapeStepSize_2)
	{
		return 0.0;
	}

	const unsigned int linedIntegralStrideElements = width + 1u;
	const unsigned int linedIntegralSquaredStrideElements = width + 1u;


	// horizontal response

	const unsigned int xHorizontalStartLeft = x - shapeWidth_2;
	const unsigned int xHorizontalStartRight = x + shapeStepSize_2 + 1u;

	const unsigned int yHorizontalTop = y - shapeStepSize_2 - shapeTopBand;
	const unsigned int yHorizontalBottom = y + shapeStepSize_2 + 1u;

	double topMean = -1.0;
	double bottomMean = -1.0;
	double topBottomVariance = -1.0;

	// top is one block
	CV::IntegralImage::linedIntegralVariance<uint32_t, uint64_t, double, true>(linedIntegral, linedIntegralSquared, linedIntegralStrideElements, linedIntegralSquaredStrideElements, xHorizontalStartLeft, yHorizontalTop, shapeWidth, shapeStepSize_2 + shapeTopBand + 1u, &topMean);

	// bottom are two blocks
	CV::IntegralImage::linedIntegralVariance<uint32_t, uint64_t, double, true>(linedIntegral, linedIntegralSquared, linedIntegralStrideElements, linedIntegralSquaredStrideElements, xHorizontalStartLeft, yHorizontalBottom, shapeWidth_2 - shapeStepSize_2, shapeBottomBand, xHorizontalStartRight, yHorizontalBottom, shapeWidth_2 - shapeStepSize_2, shapeBottomBand, &bottomMean);

	// variance across the one top block and the two bottom blocks
	topBottomVariance = CV::IntegralImage::linedIntegralVariance<uint32_t, uint64_t, double>(linedIntegral, linedIntegralSquared, linedIntegralStrideElements, linedIntegralSquaredStrideElements, xHorizontalStartLeft, yHorizontalBottom, shapeWidth_2 - shapeStepSize_2, shapeBottomBand, xHorizontalStartRight, yHorizontalBottom, shapeWidth_2 - shapeStepSize_2, shapeBottomBand, xHorizontalStartLeft, y - shapeStepSize_2 - shapeTopBand, shapeWidth, shapeTopBand + shapeStepSize);

	ocean_assert(topMean >= 0.0);
	ocean_assert(bottomMean >= 0.0);

	ocean_assert(topBottomVariance >= 0.0 && topBottomVariance < 256.0 * 256.0);

	const double topBottomDeviation = NumericD::sqrt(topBottomVariance);

	double internalHorizontalResponse = topMean - bottomMean;

	bool setHorizontalResponseZero = false;

	if (sign != 0 && NumericD::sign(internalHorizontalResponse) != double(sign))
	{
		setHorizontalResponseZero = true;
	}

	internalHorizontalResponse = NumericD::abs(internalHorizontalResponse);

	const double horizontalDeviation = topBottomDeviation;

	if (bandStrategy == BS_DIVIDE)
	{
		internalHorizontalResponse /= std::max(1.0, horizontalDeviation);
	}
	else if (bandStrategy == BS_SUBTRACT_AND_DIVIDE)
	{
		internalHorizontalResponse = std::max(0.0, internalHorizontalResponse - horizontalDeviation) / std::max(1.0, horizontalDeviation);
	}

	if (NumericD::abs(internalHorizontalResponse) < double(minimalGradient))
	{
		setHorizontalResponseZero = true;
	}

	if (setHorizontalResponseZero)
	{
		internalHorizontalResponse = 0.0;
	}


	// vertical response

	const unsigned int xVerticalLeft = x - shapeStepSize_2 - shapeBottomBand;
	const unsigned int xVerticalRight = x + shapeStepSize_2 + 1u;

	const unsigned int yVerticalTop = y + 1u;

	double leftMean = -1.0;
	double rightMean = -1.0;
	double leftRightVariance = -1.0;

	CV::IntegralImage::linedIntegralVariance<uint32_t, uint64_t, double, true>(linedIntegral, linedIntegralSquared, linedIntegralStrideElements, linedIntegralSquaredStrideElements, xVerticalLeft, yVerticalTop, shapeBottomBand, shapeHeight - shapeStepSize, &leftMean);
	CV::IntegralImage::linedIntegralVariance<uint32_t, uint64_t, double, true>(linedIntegral, linedIntegralSquared, linedIntegralStrideElements, linedIntegralSquaredStrideElements, xVerticalRight, yVerticalTop, shapeBottomBand, shapeHeight - shapeStepSize, &rightMean);

	leftRightVariance = CV::IntegralImage::linedIntegralVariance<uint32_t, uint64_t, double>(linedIntegral, linedIntegralSquared, linedIntegralStrideElements, linedIntegralSquaredStrideElements, xVerticalLeft, yVerticalTop, shapeBottomBand, shapeHeight - shapeStepSize, xVerticalRight, yVerticalTop, shapeBottomBand, shapeHeight - shapeStepSize);

	double middleMean = -1.0;
	const double middleVariance = CV::IntegralImage::linedIntegralVariance<uint32_t, uint64_t, double, true>(linedIntegral, linedIntegralSquared, linedIntegralStrideElements, linedIntegralSquaredStrideElements, x, y + shapeStepSize_2 + 1u, 1u, shapeHeight - shapeStepSize, &middleMean);

	ocean_assert(leftMean >= 0.0);
	ocean_assert(middleMean >= 0.0);
	ocean_assert(rightMean >= 0.0);

	ocean_assert(middleVariance >= 0.0 && middleVariance < 256.0 * 256.0);
	ocean_assert(leftRightVariance >= 0.0 && leftRightVariance < 256.0 * 256.0);

	const double middleDeviation = NumericD::sqrt(middleVariance);

	const double leftRightDeviation = NumericD::sqrt(leftRightVariance);

	double leftGradient = middleMean - leftMean;
	double rightGradient = rightMean - middleMean;

	double internalVerticalResponse = leftGradient - rightGradient;

	bool setVerticalResponseZero = false;

	if (sign != 0 && NumericD::sign(internalVerticalResponse) != double(sign))
	{
		setVerticalResponseZero = true;
	}

	internalVerticalResponse = NumericD::abs(internalVerticalResponse);

	const double verticalDeviation = (leftRightDeviation + middleDeviation) * 0.5;

	if (bandStrategy == BS_DIVIDE)
	{
		internalVerticalResponse /= std::max(1.0, verticalDeviation);
	}
	else if (bandStrategy == BS_SUBTRACT_AND_DIVIDE)
	{
		internalVerticalResponse = std::max(0.0, internalVerticalResponse - verticalDeviation) / std::max(1.0, verticalDeviation);
	}

	leftGradient = NumericD::abs(leftGradient);
	rightGradient = NumericD::abs(rightGradient);

	if (leftGradient < double(minimalGradient) || rightGradient < double(minimalGradient))
	{
		setVerticalResponseZero = true;
	}

	if (setVerticalResponseZero)
	{
		internalVerticalResponse = 0.0;
	}

	if (horizontalResponse)
	{
		*horizontalResponse = internalHorizontalResponse;
	}

	if (verticalResponse)
	{
		*verticalResponse = internalVerticalResponse;
	}

	if (responseType == RT_HORIZONTAL)
	{
		return internalHorizontalResponse;
	}

	if (responseType == RT_VERTICAL)
	{
		return internalVerticalResponse;
	}


	// combine horizontal and vertical response

	ocean_assert(responseType == RT_HORIZONTAL_AND_VERTICAL);

	if (maximalResponseRatio > 1.0)
	{
		if (internalHorizontalResponse < NumericD::eps() || internalVerticalResponse < NumericD::eps())
		{
			return 0.0;
		}

		if (internalHorizontalResponse > maximalResponseRatio * internalVerticalResponse || internalVerticalResponse > maximalResponseRatio * internalHorizontalResponse)
		{
			return 0.0;
		}
	}

	double response = internalHorizontalResponse * internalVerticalResponse;

	return response;
}

bool TestShapeDetector::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Shape detector test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testGradientBasedTShapeDetector(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testGradientVarianceBasedTShapeDetectorHorizontalResponse(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testGradientVarianceBasedTShapeDetectorVerticalResponse(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testGradientVarianceBasedTShapeDetector(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Shape detector test succeeded.";
	}
	else
	{
		Log::info() << "Shape detector test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestShapeDetector, GradientBasedTShapeDetector)
{
	EXPECT_TRUE(TestShapeDetector::testGradientBasedTShapeDetector(GTEST_TEST_DURATION));
}

TEST(TestShapeDetector, GradientVarianceBasedTShapeDetectorHorizontalResponse)
{
	EXPECT_TRUE(TestShapeDetector::testGradientVarianceBasedTShapeDetectorHorizontalResponse(GTEST_TEST_DURATION));
}

TEST(TestShapeDetector, GradientVarianceBasedTShapeDetectorVerticalResponse)
{
	EXPECT_TRUE(TestShapeDetector::testGradientVarianceBasedTShapeDetectorVerticalResponse(GTEST_TEST_DURATION));
}

TEST(TestShapeDetector, GradientVarianceBasedTShapeDetector)
{
	EXPECT_TRUE(TestShapeDetector::testGradientVarianceBasedTShapeDetector(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestShapeDetector::testGradientBasedTShapeDetector(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Gradient-based T-shape detector test:";

	unsigned long long iterations = 0ull;
	unsigned long long successful = 0ull;

	RandomGenerator randomGenerator;

	const unsigned int frameWidth = RandomI::random(randomGenerator, 100u, 1920u);
	const unsigned int frameHeight = RandomI::random(randomGenerator, 100u, 1080u);
	const unsigned int framePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(0u, 1u);

	Frame yFrame(FrameType(frameWidth, frameHeight, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), framePaddingElements);

	for (unsigned int n = 0u; n < yFrame.size(); ++n)
	{
		yFrame.data<uint8_t>()[n] = uint8_t(RandomI::random(randomGenerator, 100u, 140u));
	}

	for (unsigned int n = 0u; n < frameWidth * frameHeight / 500u; ++n)
	{
		const int x = RandomI::random(randomGenerator, 0, int(frameWidth) - 1);
		const int y = RandomI::random(randomGenerator, 0, int(frameHeight) - 1);

		CV::Canvas::line8BitPerChannel<1u, 5u>(yFrame.data<uint8_t>(), yFrame.width(), yFrame.height(), Vector2(Scalar(x - 20), Scalar(y)), Vector2(Scalar(x + 20), Scalar(y)), CV::Canvas::white(yFrame.pixelFormat()), yFrame.paddingElements());
		CV::Canvas::line8BitPerChannel<1u, 5u>(yFrame.data<uint8_t>(), yFrame.width(), yFrame.height(), Vector2(Scalar(x), Scalar(y - 10)), Vector2(Scalar(x), Scalar(y + 20)), CV::Canvas::white(yFrame.pixelFormat()), yFrame.paddingElements());
	}

	const unsigned int linedIntegralHorizontalSignedPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(0u, 1u);

	Frame linedIntegralHorizontalSignedGradient(FrameType(yFrame.width(), yFrame.height() + 1u, FrameType::genericPixelFormat<int32_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT), linedIntegralHorizontalSignedPaddingElements);
	CV::FrameFilterGradient::filterHorizontal1x2LinedIntegralImage<uint8_t, int32_t, false>(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), linedIntegralHorizontalSignedGradient.data<int32_t>(), yFrame.paddingElements(), linedIntegralHorizontalSignedGradient.paddingElements());

	const unsigned int linedIntegralHorizontalAbsolutePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(0u, 1u);

	Frame linedIntegralHorizontalAbsoluteGradient(FrameType(yFrame.width(), yFrame.height() + 1u, FrameType::genericPixelFormat<uint32_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT), linedIntegralHorizontalAbsolutePaddingElements);
	CV::FrameFilterGradient::filterHorizontal1x2LinedIntegralImage<uint8_t, uint32_t, true>(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), linedIntegralHorizontalAbsoluteGradient.data<uint32_t>(), yFrame.paddingElements(), linedIntegralHorizontalAbsoluteGradient.paddingElements());

	const unsigned int linedIntegralVerticalSignedPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(0u, 1u);

	Frame linedIntegralVerticalSignedGradient(FrameType(yFrame.width() + 1u, yFrame.height(), FrameType::genericPixelFormat<int32_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT), linedIntegralVerticalSignedPaddingElements);
	CV::FrameFilterGradient::filterVertical2x1LinedIntegralImage<uint8_t, int32_t, false>(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), linedIntegralVerticalSignedGradient.data<int32_t>(), yFrame.paddingElements(), linedIntegralVerticalSignedGradient.paddingElements());

	const unsigned int linedIntegralVerticalAbsolutePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(0u, 1u);

	Frame linedIntegralVerticalAbsoluteGradient(FrameType(yFrame.width() + 1u, yFrame.height(), FrameType::genericPixelFormat<uint32_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT), linedIntegralVerticalAbsolutePaddingElements);
	CV::FrameFilterGradient::filterVertical2x1LinedIntegralImage<uint8_t, uint32_t, true>(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), linedIntegralVerticalAbsoluteGradient.data<uint32_t>(), yFrame.paddingElements(), linedIntegralVerticalAbsoluteGradient.paddingElements());

	Frame yRotatedFrame(yFrame.frameType(), yFrame.paddingElements());
	CV::FrameConverterY8::convertY8ToY8(yFrame.constdata<uint8_t>(), yRotatedFrame.data<uint8_t>(), yFrame.width(), yFrame.height(), CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, yFrame.paddingElements(), yRotatedFrame.paddingElements());

	Timestamp start(true);

	do
	{
		const unsigned int x = RandomI::random(randomGenerator, 0u, frameWidth - 1u);
		const unsigned int y = RandomI::random(randomGenerator, 0u, frameHeight - 1u);

		const unsigned int shapeWidth = 15u;
		const unsigned int shapeHeight = 15u;
		const unsigned int topBand = 4u;
		const unsigned int shapeStepSize = 3u;
		const unsigned int bottomBand = 4u;

		const unsigned int minimalDelta = 2u;

		const int sign = RandomI::random(0u, 1u) == 0u ? -1 : 1;

		const double response = CV::Detector::ShapeDetector::PatternDetectorGradientBased::tShapeResponse(linedIntegralHorizontalSignedGradient.constdata<int32_t>(), linedIntegralHorizontalAbsoluteGradient.constdata<uint32_t>(), linedIntegralVerticalSignedGradient.constdata<int32_t>(), linedIntegralVerticalAbsoluteGradient.constdata<uint32_t>(), frameWidth, frameHeight, x, y, sign, shapeWidth, shapeHeight, shapeStepSize, topBand, bottomBand, minimalDelta, linedIntegralHorizontalSignedGradient.paddingElements(), linedIntegralHorizontalAbsoluteGradient.paddingElements(), linedIntegralVerticalSignedGradient.paddingElements(), linedIntegralVerticalAbsoluteGradient.paddingElements());

		const double testResponse = GradientBasedDetector::tShapeDetectorResponse(yFrame, x, y, sign, shapeWidth, shapeHeight, shapeStepSize, topBand, bottomBand, RT_HORIZONTAL_AND_VERTICAL, double(shapeStepSize), minimalDelta, GradientBasedDetector::ERS_GRADIENT_TO_NEIGHBOR, GradientBasedDetector::MRS_IGNORE, GradientBasedDetector::PU_SUBTRACT);

		if (NumericD::isEqual(response, testResponse, 0.01))
		{
			++successful;
		}

		++iterations;
	}
	while (Timestamp(true) < start + testDuration);

	Log::info() << " ";

	ocean_assert(iterations != 0ull);
	const double percent = double(successful) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 2u) << "%";

	return percent >= 0.975;
}

bool TestShapeDetector::testGradientVarianceBasedTShapeDetectorHorizontalResponse(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Horizontal response of Gradient & Variance-based T-shape detector test:";

	unsigned long long iterations = 0ull;
	unsigned long long successful = 0ull;

	RandomGenerator randomGenerator;

	Timestamp start(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 20u, 400u);

		const unsigned int maximalFrameHeight = 65536u / width;
		const unsigned int height = RandomI::random(randomGenerator, 20u, maximalFrameHeight);

		const unsigned int framePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(0u, 1u);

		Frame yFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), framePaddingElements);

		for (unsigned int n = 0u; n < yFrame.size(); ++n)
		{
			yFrame.data<uint8_t>()[n] = uint8_t(RandomI::random(randomGenerator, 100u, 140u));
		}

		for (unsigned int n = 0u; n < width * height / 500u; ++n)
		{
			const int x = RandomI::random(randomGenerator, 0, int(width) - 1);
			const int y = RandomI::random(randomGenerator, 0, int(height) - 1);

			CV::Canvas::line8BitPerChannel<1u, 5u>(yFrame.data<uint8_t>(), yFrame.width(), yFrame.height(), Vector2(Scalar(x - 20), Scalar(y)), Vector2(Scalar(x + 20), Scalar(y)), CV::Canvas::white(yFrame.pixelFormat()), yFrame.paddingElements());
			CV::Canvas::line8BitPerChannel<1u, 5u>(yFrame.data<uint8_t>(), yFrame.width(), yFrame.height(), Vector2(Scalar(x), Scalar(y - 10)), Vector2(Scalar(x), Scalar(y + 20)), CV::Canvas::white(yFrame.pixelFormat()), yFrame.paddingElements());
		}

		Frame joinedLinedIntegralAndSquaredFrame(FrameType(width + 1u, height + 1u, FrameType::genericPixelFormat<uint32_t, 2u>(), FrameType::ORIGIN_UPPER_LEFT));
		CV::IntegralImage::createLinedImageAndSquared<uint8_t, uint32_t, 1u>(yFrame.constdata<uint8_t>(), joinedLinedIntegralAndSquaredFrame.data<uint32_t>(), width, height, yFrame.paddingElements(), joinedLinedIntegralAndSquaredFrame.paddingElements());

		Frame separateLinedIntegralFrame(FrameType(width + 1u, height + 1u, FrameType::genericPixelFormat<uint32_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT));
		Frame separateLinedIntegralSquaredFrame(FrameType(width + 1u, height + 1u, FrameType::genericPixelFormat<uint64_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT));
		CV::IntegralImage::createLinedImageAndSquared<uint8_t, uint32_t, uint64_t, 1u>(yFrame.constdata<uint8_t>(), separateLinedIntegralFrame.data<uint32_t>(), separateLinedIntegralSquaredFrame.data<uint64_t>(), width, height, yFrame.paddingElements(), separateLinedIntegralFrame.paddingElements(), separateLinedIntegralSquaredFrame.paddingElements());

		Frame horizontalResponseFrameF(FrameType(determineHorizontalResponseWidth(width), determineHorizontalResponseHeight(height), FrameType::genericPixelFormat<float, 1u>(), FrameType::ORIGIN_UPPER_LEFT));
		determineHorizontalResponsesF<true /*tSquaredResponse*/>(joinedLinedIntegralAndSquaredFrame.constdata<uint32_t>(), width, height, horizontalResponseFrameF.data<float>(), joinedLinedIntegralAndSquaredFrame.paddingElements(), horizontalResponseFrameF.paddingElements());

		Frame horizontalResponseFrameJoinedI(FrameType(horizontalResponseFrameF, FrameType::genericPixelFormat<int32_t, 1u>()));
		determineHorizontalResponsesI<false /*tUseSIMD*/>(joinedLinedIntegralAndSquaredFrame.constdata<uint32_t>(), width, height, horizontalResponseFrameJoinedI.data<int32_t>(), joinedLinedIntegralAndSquaredFrame.paddingElements(), horizontalResponseFrameJoinedI.paddingElements());

		Frame horizontalResponseFrameSeparateI(FrameType(horizontalResponseFrameF, FrameType::genericPixelFormat<int32_t, 1u>()));
		determineHorizontalResponsesI<false /*tUseSIMD*/>(separateLinedIntegralFrame.constdata<uint32_t>(), separateLinedIntegralSquaredFrame.constdata<uint64_t>(), width, height, horizontalResponseFrameSeparateI.data<int32_t>(), separateLinedIntegralFrame.paddingElements(), separateLinedIntegralSquaredFrame.paddingElements(), horizontalResponseFrameSeparateI.paddingElements());

		for (unsigned int y = 0u; y < horizontalResponseFrameF.height(); ++y)
		{
			for (unsigned int x = 0u; x < horizontalResponseFrameF.width(); ++x)
			{
				const float responseF = horizontalResponseFrameF.constpixel<float>(x, y)[0];
				const int32_t responseJoinedI = horizontalResponseFrameJoinedI.constpixel<int32_t>(x, y)[0];
				const int32_t responseSeparateI = horizontalResponseFrameSeparateI.constpixel<int32_t>(x, y)[0];

				const float sqrtAbsResponseF = NumericF::sqrt(NumericF::abs(responseF));
				const float sqrtAbsResponseI = NumericF::sqrt(NumericF::abs(float(responseJoinedI)));

				if (responseJoinedI == responseSeparateI) // both integer results must be identical
				{
					// the floating point-based version applies the sqrt to determine the derivation for normalization,
					// the integer-based version avoids using sqrt and uses the variance instead (with corresponding adjustments of detection thresholds)
					// therefore, the response precision can vary slightly for low responses - so that we have skip the precision test for small responses

					if (sqrtAbsResponseF < 15.0f || (NumericF::sign(responseF) == NumericF::sign(float(responseJoinedI)) && NumericF::isEqual(sqrtAbsResponseF, float(sqrtAbsResponseI), 1.0f)))
					{
						++successful;
					}
				}

				++iterations;
			}
		}

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

		Frame neonHorizontalResponseFrameJoinedI(horizontalResponseFrameJoinedI.frameType());
		determineHorizontalResponsesI<true /*tUseSIMD*/>(joinedLinedIntegralAndSquaredFrame.constdata<uint32_t>(), width, height, neonHorizontalResponseFrameJoinedI.data<int32_t>(), joinedLinedIntegralAndSquaredFrame.paddingElements(), neonHorizontalResponseFrameJoinedI.paddingElements());

		Frame neonHorizontalResponseFrameSeparatedI(horizontalResponseFrameJoinedI.frameType());
		determineHorizontalResponsesI<true /*tUseSIMD*/>(separateLinedIntegralFrame.constdata<uint32_t>(), separateLinedIntegralSquaredFrame.constdata<uint64_t>(), width, height, neonHorizontalResponseFrameSeparatedI.data<int32_t>(), separateLinedIntegralFrame.paddingElements(), separateLinedIntegralSquaredFrame.paddingElements(), neonHorizontalResponseFrameSeparatedI.paddingElements());

		for (unsigned int y = 0u; y < horizontalResponseFrameJoinedI.height(); ++y)
		{
			for (unsigned int x = 0u; x < horizontalResponseFrameJoinedI.width(); ++x)
			{
				const int32_t responseJoinedI = horizontalResponseFrameJoinedI.constpixel<int32_t>(x, y)[0];
				const int32_t neonResponseJoinedI = neonHorizontalResponseFrameJoinedI.constpixel<int32_t>(x, y)[0];
				const int32_t neonResponseSeparateI = neonHorizontalResponseFrameSeparatedI.constpixel<int32_t>(x, y)[0];

				const float sqrtAbsResponseJoinedI = NumericF::sqrt(NumericF::abs(float(responseJoinedI)));
				const float sqrtAbsNeonResponseSeparateI = NumericF::sqrt(NumericF::abs(float(neonResponseSeparateI)));

				if (neonResponseSeparateI == neonResponseJoinedI)
				{
					if (NumericF::isEqual(sqrtAbsResponseJoinedI, sqrtAbsNeonResponseSeparateI, 1.0f))
					{
						++successful;
					}
				}

				++iterations;
			}
		}
#endif // defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
	}
	while (Timestamp(true) < start + testDuration);

	Log::info() << " ";

	ocean_assert(iterations != 0ull);
	const double percent = double(successful) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 2u) << "%";

	return percent >= 0.975;
}

bool TestShapeDetector::testGradientVarianceBasedTShapeDetectorVerticalResponse(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Vertical response of Gradient & Variance-based T-shape detector test:";

	unsigned long long iterations = 0ull;
	unsigned long long successful = 0ull;

	RandomGenerator randomGenerator;

	Timestamp start(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 20u, 400u);

		const unsigned int maximalFrameHeight = 65536u / width;
		const unsigned int height = RandomI::random(randomGenerator, 20u, maximalFrameHeight);

		const unsigned int framePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(0u, 1u);

		Frame yFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), framePaddingElements);

		for (unsigned int n = 0u; n < yFrame.size(); ++n)
		{
			yFrame.data<uint8_t>()[n] = uint8_t(RandomI::random(randomGenerator, 100u, 140u));
		}

		for (unsigned int n = 0u; n < width * height / 500u; ++n)
		{
			const int x = RandomI::random(randomGenerator, 0, int(width) - 1);
			const int y = RandomI::random(randomGenerator, 0, int(height) - 1);

			CV::Canvas::line8BitPerChannel<1u, 5u>(yFrame.data<uint8_t>(), yFrame.width(), yFrame.height(), Vector2(Scalar(x - 20), Scalar(y)), Vector2(Scalar(x + 20), Scalar(y)), CV::Canvas::white(yFrame.pixelFormat()), yFrame.paddingElements());
			CV::Canvas::line8BitPerChannel<1u, 5u>(yFrame.data<uint8_t>(), yFrame.width(), yFrame.height(), Vector2(Scalar(x), Scalar(y - 10)), Vector2(Scalar(x), Scalar(y + 20)), CV::Canvas::white(yFrame.pixelFormat()), yFrame.paddingElements());
		}

		Frame joinedLinedIntegralAndSquaredFrame(FrameType(width + 1u, height + 1u, FrameType::genericPixelFormat<uint32_t, 2u>(), FrameType::ORIGIN_UPPER_LEFT));
		CV::IntegralImage::createLinedImageAndSquared<uint8_t, uint32_t, 1u>(yFrame.constdata<uint8_t>(), joinedLinedIntegralAndSquaredFrame.data<uint32_t>(), width, height, yFrame.paddingElements(), joinedLinedIntegralAndSquaredFrame.paddingElements());

		Frame separateLinedIntegralFrame(FrameType(width + 1u, height + 1u, FrameType::genericPixelFormat<uint32_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT));
		Frame separateLinedIntegralSquaredFrame(FrameType(width + 1u, height + 1u, FrameType::genericPixelFormat<uint64_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT));
		CV::IntegralImage::createLinedImageAndSquared<uint8_t, uint32_t, uint64_t, 1u>(yFrame.constdata<uint8_t>(), separateLinedIntegralFrame.data<uint32_t>(), separateLinedIntegralSquaredFrame.data<uint64_t>(), width, height, yFrame.paddingElements(), separateLinedIntegralFrame.paddingElements(), separateLinedIntegralSquaredFrame.paddingElements());

		Frame verticalResponseFrameF(FrameType(determineVerticalResponseWidth(width), determineVerticalResponseHeight(height), FrameType::genericPixelFormat<float, 1u>(), FrameType::ORIGIN_UPPER_LEFT));
		determineVerticalResponsesF<true /*tSquaredResponse*/>(joinedLinedIntegralAndSquaredFrame.constdata<uint32_t>(), width, height, verticalResponseFrameF.data<float>(), joinedLinedIntegralAndSquaredFrame.paddingElements(), verticalResponseFrameF.paddingElements());

		Frame verticalResponseFrameJoinedI(FrameType(verticalResponseFrameF, FrameType::genericPixelFormat<int32_t, 1u>()));
		determineVerticalResponsesI<false /*tUseSIMD*/>(joinedLinedIntegralAndSquaredFrame.constdata<uint32_t>(), width, height, verticalResponseFrameJoinedI.data<int32_t>(), joinedLinedIntegralAndSquaredFrame.paddingElements(), verticalResponseFrameJoinedI.paddingElements());

		Frame verticalResponseFrameSeparateI(FrameType(verticalResponseFrameF, FrameType::genericPixelFormat<int32_t, 1u>()));
		determineVerticalResponsesI<false /*tUseSIMD*/>(separateLinedIntegralFrame.constdata<uint32_t>(), separateLinedIntegralSquaredFrame.constdata<uint64_t>(), width, height, verticalResponseFrameSeparateI.data<int32_t>(), separateLinedIntegralFrame.paddingElements(), separateLinedIntegralSquaredFrame.paddingElements(), verticalResponseFrameSeparateI.paddingElements());

		for (unsigned int y = 0u; y < verticalResponseFrameF.height(); ++y)
		{
			for (unsigned int x = 0u; x < verticalResponseFrameF.width(); ++x)
			{
				const float responseF = verticalResponseFrameF.constpixel<float>(x, y)[0];
				const int32_t responseJoinedI = verticalResponseFrameJoinedI.constpixel<int32_t>(x, y)[0];
				const int32_t responseSeparateI = verticalResponseFrameSeparateI.constpixel<int32_t>(x, y)[0];

				const float sqrtAbsResponseF = NumericF::sqrt(NumericF::abs(responseF));
				const float sqrtAbsResponseI = NumericF::sqrt(NumericF::abs(float(responseJoinedI)));

				if (responseJoinedI == responseSeparateI) // both integer results must be identical
				{
					// the floating point-based version applies the sqrt to determine the derivation for normalization,
					// the integer-based version avoids using sqrt and uses the variance instead (with corresponding adjustments of detection thresholds)
					// therefore, the response precision can vary slightly for low responses - so that we have skip the precision test for small responses

					if (sqrtAbsResponseF < 15.0f || (NumericF::sign(responseF) == NumericF::sign(float(responseF)) && NumericF::isEqual(sqrtAbsResponseF, float(sqrtAbsResponseI), 1.0f)))
					{
						++successful;
					}
				}

				++iterations;
			}
		}

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

		Frame neonVerticalResponseFrameJoinedI(verticalResponseFrameJoinedI.frameType());
		determineVerticalResponsesI<true /*tUseSIMD*/>(joinedLinedIntegralAndSquaredFrame.constdata<uint32_t>(), width, height, neonVerticalResponseFrameJoinedI.data<int32_t>(), joinedLinedIntegralAndSquaredFrame.paddingElements(), neonVerticalResponseFrameJoinedI.paddingElements());

		Frame neonVerticalResponseFrameSeparatedI(verticalResponseFrameSeparateI.frameType());
		determineVerticalResponsesI<true /*tUseSIMD*/>(separateLinedIntegralFrame.constdata<uint32_t>(), separateLinedIntegralSquaredFrame.constdata<uint64_t>(), width, height, neonVerticalResponseFrameSeparatedI.data<int32_t>(), separateLinedIntegralFrame.paddingElements(), separateLinedIntegralSquaredFrame.paddingElements(), neonVerticalResponseFrameSeparatedI.paddingElements());

		for (unsigned int y = 0u; y < verticalResponseFrameF.height(); ++y)
		{
			for (unsigned int x = 0u; x < verticalResponseFrameF.width(); ++x)
			{
				const int32_t responseJoinedI = verticalResponseFrameJoinedI.constpixel<int32_t>(x, y)[0];
				const int32_t neonResponseJoinedI = neonVerticalResponseFrameJoinedI.constpixel<int32_t>(x, y)[0];
				const int32_t neonResponseSeparateI = neonVerticalResponseFrameSeparatedI.constpixel<int32_t>(x, y)[0];

				const float sqrtAbsResponseJoinedI = NumericF::sqrt(NumericF::abs(float(responseJoinedI)));
				const float sqrtAbsNeonResponseSeparateI = NumericF::sqrt(NumericF::abs(float(neonResponseSeparateI)));

				if (neonResponseSeparateI == neonResponseJoinedI)
				{
					if (NumericF::isEqual(sqrtAbsResponseJoinedI, sqrtAbsNeonResponseSeparateI, responseJoinedI == 0u ? 2.0f : 1.0f))
					{
						++successful;
					}
				}

				++iterations;
			}
		}
#endif // defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
	}
	while (Timestamp(true) < start + testDuration);

	Log::info() << " ";

	ocean_assert(iterations != 0ull);
	const double percent = double(successful) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 2u) << "%";

	return percent >= 0.975;
}

bool TestShapeDetector::testGradientVarianceBasedTShapeDetector(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Gradient & Variance-based T-shape detector test:";

	unsigned long long iterations = 0ull;
	unsigned long long successful = 0ull;

	RandomGenerator randomGenerator;

	Timestamp start(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 20u, 400u);

		const unsigned int maximalFrameHeight = 65536u / width;
		const unsigned int height = RandomI::random(randomGenerator, 20u, maximalFrameHeight);

		const unsigned int framePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(0u, 1u);

		Frame yFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), framePaddingElements);

		for (unsigned int n = 0u; n < yFrame.size(); ++n)
		{
			yFrame.data<uint8_t>()[n] = uint8_t(RandomI::random(randomGenerator, 100u, 140u));
		}

		for (unsigned int n = 0u; n < width * height / 500u; ++n)
		{
			const int x = RandomI::random(randomGenerator, 0, int(width) - 1);
			const int y = RandomI::random(randomGenerator, 0, int(height) - 1);

			CV::Canvas::line8BitPerChannel<1u, 5u>(yFrame.data<uint8_t>(), yFrame.width(), yFrame.height(), Vector2(Scalar(x - 20), Scalar(y)), Vector2(Scalar(x + 20), Scalar(y)), CV::Canvas::white(yFrame.pixelFormat()), yFrame.paddingElements());
			CV::Canvas::line8BitPerChannel<1u, 5u>(yFrame.data<uint8_t>(), yFrame.width(), yFrame.height(), Vector2(Scalar(x), Scalar(y - 10)), Vector2(Scalar(x), Scalar(y + 20)), CV::Canvas::white(yFrame.pixelFormat()), yFrame.paddingElements());
		}

		Frame linedIntegralFrame(FrameType(width + 1u, height + 1u, FrameType::genericPixelFormat<uint32_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT));
		CV::IntegralImage::createLinedImage<uint8_t, uint32_t, 1u>(yFrame.constdata<uint8_t>(), linedIntegralFrame.data<uint32_t>(), width, height, yFrame.paddingElements(), linedIntegralFrame.paddingElements());

		Frame linedIntegralSquaredFrame(FrameType(width + 1u, height + 1u, FrameType::genericPixelFormat<uint64_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT));
		CV::IntegralImage::createLinedImageSquared<uint8_t, uint64_t, 1u>(yFrame.constdata<uint8_t>(), linedIntegralSquaredFrame.data<uint64_t>(), width, height, yFrame.paddingElements(), linedIntegralSquaredFrame.paddingElements());

		Frame yRotatedFrame(yFrame.frameType(), yFrame.paddingElements());
		CV::FrameConverterY8::convertY8ToY8(yFrame.constdata<uint8_t>(), yRotatedFrame.data<uint8_t>(), yFrame.width(), yFrame.height(), CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED, yFrame.paddingElements(), yRotatedFrame.paddingElements());

		Frame linedIntegralRotatedFrame(FrameType(width + 1u, height + 1u, FrameType::genericPixelFormat<uint32_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT));
		CV::IntegralImage::createLinedImage<uint8_t, uint32_t, 1u>(yRotatedFrame.constdata<uint8_t>(), linedIntegralRotatedFrame.data<uint32_t>(), width, height, yRotatedFrame.paddingElements(), linedIntegralRotatedFrame.paddingElements());

		Frame linedIntegralSquaredRotatedFrame(FrameType(width + 1u, height + 1u, FrameType::genericPixelFormat<uint64_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT));
		CV::IntegralImage::createLinedImageSquared<uint8_t, uint64_t, 1u>(yRotatedFrame.constdata<uint8_t>(), linedIntegralSquaredRotatedFrame.data<uint64_t>(), width, height, yRotatedFrame.paddingElements(), linedIntegralSquaredRotatedFrame.paddingElements());

		constexpr unsigned int shapeWidth = 15u;
		constexpr unsigned int shapeHeight = 15u;
		constexpr unsigned int shapeTopBand = 4u;
		constexpr unsigned int shapeStepSize = 3u;
		constexpr unsigned int shapeBottomBand = 4u;

		constexpr ResponseType responseType = RT_HORIZONTAL_AND_VERTICAL;

		constexpr double minimalGradient = 0.5;
		constexpr double maximalResponseRatio = 4.0;

		constexpr GradientVarianceBasedDetector::BandStrategy bandStrategy = GradientVarianceBasedDetector::BS_SUBTRACT_AND_DIVIDE;
		constexpr GradientVarianceBasedDetector::OptimizationStrategy optimizationStrategy = GradientVarianceBasedDetector::OS_SYMMETRIC_RESPONSES_FOUR_HORIZONTAL_SAME_VERTICAL;

		const int sign = RandomI::random(-1, 1); // {-1, 0, 1}

		const float minimalThreshold = 2.2f;

		{
			// testing the float-based detector

			LShapes lShapes;
			TShapes tShapes;
			XShapes xShapes;

			Frame topDownResponseFrame;
			Frame bottomUpResponseFrame;
			CV::Detector::ShapeDetector::PatternDetectorGradientVarianceBased::detectShapesF(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), lShapes, tShapes, xShapes, sign, minimalThreshold, yFrame.paddingElements(), &topDownResponseFrame, &bottomUpResponseFrame);

			for (unsigned int y = 0u; y < height; ++y)
			{
				for (unsigned int x = 0u; x < width; ++x)
				{
					ocean_assert(linedIntegralFrame.isContinuous() && linedIntegralSquaredFrame.isContinuous());

					const double testTopDownResponse = GradientVarianceBasedDetector::tShapeDetectorResponse(linedIntegralFrame.constdata<uint32_t>(), linedIntegralSquaredFrame.constdata<uint64_t>(), width, height, x, y, sign, shapeWidth, shapeHeight, shapeStepSize, shapeTopBand, shapeBottomBand, responseType, minimalGradient, maximalResponseRatio, bandStrategy, optimizationStrategy);

					const float topDownResponse = topDownResponseFrame.constpixel<float>(x, y)[0];

					if (NumericF::isWeakEqual(float(testTopDownResponse), topDownResponse))
					{
						++successful;
					}

					ocean_assert(linedIntegralRotatedFrame.isContinuous() && linedIntegralSquaredRotatedFrame.isContinuous());

					const double testBottomUpResponse = GradientVarianceBasedDetector::tShapeDetectorResponse(linedIntegralRotatedFrame.constdata<uint32_t>(), linedIntegralSquaredRotatedFrame.constdata<uint64_t>(), width, height, width - x - 1u, height - y - 1u, sign, shapeWidth, shapeHeight, shapeStepSize, shapeTopBand, shapeBottomBand, responseType, minimalGradient, maximalResponseRatio, bandStrategy, optimizationStrategy);

					const float bottomUpResponse = bottomUpResponseFrame.constpixel<float>(x, y)[0];

					if (NumericF::isWeakEqual(float(testBottomUpResponse), bottomUpResponse))
					{
						++successful;
					}

					iterations += 2u;
				}
			}
		}

		{
			// just running the integer-based detector without checking the results

			LShapes lShapes;
			TShapes tShapes;
			XShapes xShapes;
			CV::Detector::ShapeDetector::PatternDetectorGradientVarianceBased::detectShapesI(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), lShapes, tShapes, xShapes, sign, minimalThreshold, yFrame.paddingElements());
		}
	}
	while (Timestamp(true) < start + testDuration);

	Log::info() << " ";

	ocean_assert(iterations != 0ull);
	const double percent = double(successful) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 2u) << "%";

	return percent >= 0.975;
}

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namespace Ocean
