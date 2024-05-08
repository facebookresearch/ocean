/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/barcodes/BarcodeDetector2D.h"
#include "ocean/cv/detector/barcodes/ParserEan13Upca.h"

#include "ocean/cv/Bresenham.h"
#include "ocean/cv/Histogram.h"

#include "ocean/math/Quaternion.h"
#include "ocean/math/Vector3.h"

#include <numeric>

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace Barcodes
{

BarcodeDetector2D::Observation::Observation(const Vector2& startPoint, const Vector2& endPoint) :
	location_(startPoint, endPoint)
{
	// Nothing else to do.
}

const FiniteLine2& BarcodeDetector2D::Observation::location() const
{
	return location_;
}

Barcodes BarcodeDetector2D::detectBarcodes(const Frame& yFrame, const uint32_t detectionFeatures, const BarcodeTypeSet& enabledBarcodeTypes, const unsigned int scanlineSpacing, Observations* observations, FiniteLines2* scanlines)
{
	ocean_assert(yFrame.isValid() && yFrame.isPixelFormatCompatible(FrameType::FORMAT_Y8) && yFrame.pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT);
	ocean_assert(scanlineSpacing != 0u);

	Barcodes barcodes;
	Observations localObservations;

	constexpr unsigned int frameBorder = 10u;
	constexpr unsigned int minimumFrameSize = 50u;

	constexpr unsigned int minimumFrameSizeWithBorder = minimumFrameSize + 2u * frameBorder;

	if (yFrame.width() < minimumFrameSizeWithBorder || yFrame.height() < minimumFrameSizeWithBorder)
	{
		return Barcodes();
	}

	const bool enableMultipleScanlineDirections = detectionFeatures & DF_ENABLE_MULTIPLE_SCANLINE_DIRECTIONS;
	const bool enableInvertedReflectance = detectionFeatures & DF_ENABLE_INVERTED_REFLECTANCE;
	const bool enableScanlineMirroring = false; // TODO detectionFeatures & DF_ENABLE_SCANLINE_MIRRORING;
	const bool enableMulticodeDetection = (detectionFeatures & DF_ENABLE_MULTI_CODE_DETECTION) || (detectionFeatures & DF_ENABLE_MULTI_CODE_DETECTION_WITH_DUPLICATES);
	const bool allowDuplicates = detectionFeatures & DF_ENABLE_MULTI_CODE_DETECTION_WITH_DUPLICATES;

	const Scalars scanlineAnglesStandard = {Numeric::deg2rad(0)};
	const Scalars scanlineAnglesMultiple = {Numeric::deg2rad(0), Numeric::deg2rad(90), Numeric::deg2rad(45), Numeric::deg2rad(135)};
	const Scalars& scanlineAngles = enableMultipleScanlineDirections ? scanlineAnglesMultiple : scanlineAnglesStandard;

	const std::vector<bool> reflectanceSettings = enableInvertedReflectance ? std::vector<bool>{true, false} : std::vector<bool>{true};
	const std::vector<bool> mirroringSettings = enableScanlineMirroring ? std::vector<bool>{false, true} : std::vector<bool>{false};

	const ParserFunctionSet parserFunctionsSet = getParserFunctions(enabledBarcodeTypes);
	ocean_assert(!parserFunctionsSet.empty());

	ScanlineData scanlineData;
	CV::PixelPositionsI scanlinePositions;

	scanlineData.reserve((15 * size_t(std::max(yFrame.width(), yFrame.height()))) / 10); // ~ sqrt(2) * max(width, height)
	scanlinePositions.reserve(scanlineData.size());

	for (const Scalar angle : scanlineAngles)
	{
		const Vector2 scanlineDirection = computeDirectionVector(angle, /* length */ Scalar(1));
		ocean_assert(scanlineDirection.isUnit());

		const FiniteLines2 localScanlines = computeScanlines(yFrame.width(), yFrame.height(), scanlineDirection, scanlineSpacing, frameBorder, minimumFrameSize);

		if (scanlines != nullptr)
		{
			scanlines->insert(scanlines->end(), localScanlines.begin(), localScanlines.end());
		}

		const size_t maxNumberRequiredSegments = 59; // TODO Determine this once for all enabled barcode types.
		const size_t minNumberRequiredSegments = 59;

		for (const FiniteLine2& scanline : localScanlines)
		{
			if (!extractScanlineData(yFrame, scanline, scanlineData, scanlinePositions, minimumFrameSize))
			{
				continue;
			}

			for (const bool isNormalReflectance : reflectanceSettings)
			{
				for (const bool mirrorScanline : mirroringSettings)
				{
					constexpr uint8_t gradientThreshold = 20u;

					RowSegmenter rowSegmenter(scanlineData.data(), scanlineData.size(), gradientThreshold, isNormalReflectance /* TODO , mirrorScanline */);

					while (rowSegmenter.findNextTransitionToForeground())
					{
						rowSegmenter.prepareSegments(maxNumberRequiredSegments);

						const SegmentData& segmentData = rowSegmenter.segmentData();

						if (segmentData.size() < minNumberRequiredSegments)
						{
							// There aren't enough segments even for the smallest barcode type.
							continue;
						}

						// Search for all enabled barcode types

						for (const ParserFunction& findBarcodeInSegmentData : parserFunctionsSet)
						{
							ocean_assert(findBarcodeInSegmentData != nullptr);

							IndexPair32 xCoordinates;
							Barcode newBarcode;

							if (!findBarcodeInSegmentData(segmentData.data(), segmentData.size(), newBarcode, xCoordinates))
							{
								continue;
							}

							ocean_assert(newBarcode.isValid());

							if (!enabledBarcodeTypes.empty() && enabledBarcodeTypes.find(newBarcode.barcodeType()) == enabledBarcodeTypes.cend())
							{
								// This type of barcode is not requested and can be discarded.
								//
								// This can happen if a parser is able to find multiple types of barcodes,
								// for example like EAN-13 and UPC-A when only one of them is requested.
								continue;
							}

							if (!allowDuplicates)
							{
								bool isDuplicate = false;

								for (const Barcode& barcode : barcodes)
								{
									if (newBarcode.barcodeType() == barcode.barcodeType() && newBarcode.data() == barcode.data())
									{
										isDuplicate = true;
										break;
									}
								}

								if (isDuplicate)
								{
									continue;
								}
							}

							// Determine the image location where this barcode has been detected.

							const Index32 xScanlineStart = Index32(rowSegmenter.position()) + xCoordinates.first;
							const Index32 xScanlineEnd = Index32(rowSegmenter.position()) + xCoordinates.second; // Inclusive!

							const CV::PixelPositionI& barcodeStart = scanlinePositions[xScanlineStart];
							const CV::PixelPositionI& barcodeEnd = scanlinePositions[xScanlineEnd];

							// Save the results.

							barcodes.emplace_back(std::move(newBarcode));
							localObservations.emplace_back(Vector2(Scalar(barcodeStart.x()) + Scalar(0.5), Scalar(barcodeStart.y()) + Scalar(0.5)), Vector2(Scalar(barcodeEnd.x()) + Scalar(0.5), Scalar(barcodeEnd.y()) + Scalar(0.5))); // Adding 0.5 offsets to move to pixel center

							// In the next iteration — in order to avoid duplicate detections — move the segmenter behind the barcode that has been detected just now.

							if (mirrorScanline)
							{
								rowSegmenter.setPosition(size_t(std::min(xScanlineStart, xScanlineEnd)));
							}
							else
							{
								rowSegmenter.setPosition(size_t(std::max(xScanlineStart, xScanlineEnd)));
							}

							// Check if the detection should end here.

							if (!enableMulticodeDetection && barcodes.size() >= 1)
							{
								// Only return the first detection.
								ocean_assert(barcodes.size() == localObservations.size());

								while (barcodes.size() > 1)
								{
									barcodes.pop_back();
									localObservations.pop_back();
								}

								if (observations != nullptr)
								{
									*observations = std::move(localObservations);
								}

								return barcodes;
							}

							// No need to call the remaining parser functions at this point anymore, because a barcode has been detected with the current parser function. So, there won't be a second one that starts at the same segment.
							break;
						}
					}
				}
			}
		}
	}

	ocean_assert(barcodes.size() == localObservations.size());

	if (observations)
	{
		*observations = std::move(localObservations);
	}

	return barcodes;
}

Vector2 BarcodeDetector2D::computeDirectionVector(const Scalar angle, const Scalar length)
{
	ocean_assert(angle >= Scalar(0) && angle < Numeric::pi2());
	ocean_assert(length > Scalar(0));

	const Vector3 homogenousDirection = Quaternion(Vector3(0, 0, 1), angle) * Vector3(1, 0, 1);
	ocean_assert(Numeric::isNotEqualEps(homogenousDirection.z()));

	const Vector2 direction = Vector2(homogenousDirection.x() / homogenousDirection.z(), homogenousDirection.y() / homogenousDirection.z()).normalizedOrZero() * length;

	return direction;
}

bool BarcodeDetector2D::computeFrameIntersection(const unsigned int frameWidth, const unsigned frameHeight, const unsigned int frameBorder, const Line2& line, CV::PixelPositionI& point0, CV::PixelPositionI& point1)
{
	ocean_assert(frameWidth != 0u && frameHeight != 0u);
	ocean_assert(line.isValid());

	if (CV::Bresenham::borderIntersection(line, int(frameBorder), int(frameBorder), int(frameWidth - frameBorder), int(frameHeight - frameBorder), point0.x(), point0.y(), point1.x(), point1.y()))
	{
		if (point0.x() > point1.x())
		{
			std::swap(point0, point1);
		}
		else if (point0.x() == point1.x() && point0.y() > point1.y())
		{
			std::swap(point0, point1);
		}

		ocean_assert(point0.x() >= 0 && point0.x() < int(frameWidth) && point0.y() >= 0 && point0.y() < int(frameHeight));
		ocean_assert(point1.x() >= 0 && point1.x() < int(frameWidth) && point1.y() >= 0 && point1.y() < int(frameHeight));
		ocean_assert(point0.x() != point1.x() || point0.y() != point1.y());

		return true;
	}

	return false;
}

FiniteLines2 BarcodeDetector2D::computeScanlines(const unsigned int frameWidth, const unsigned frameHeight, const Vector2& scanlineDirection, const unsigned int scanlineSpacing, const unsigned int frameBorder, const unsigned int minimumScanlineLength)
{
	ocean_assert(frameWidth != 0u && frameHeight != 0u);
	ocean_assert(scanlineDirection.isUnit());
	ocean_assert(scanlineSpacing != 0u);

	const Vector2 frameCenter(Scalar(frameWidth) / Scalar(2), Scalar(frameHeight) / Scalar(2));

	// First scan line
	CV::PixelPositionI scanlinePoint0;
	CV::PixelPositionI scanlinePoint1;
	if (!computeFrameIntersection(frameWidth, frameHeight, frameBorder, Line2(frameCenter, scanlineDirection), scanlinePoint0, scanlinePoint1))
	{
		return FiniteLines2();
	}

	FiniteLines2 scanlines = {FiniteLine2(Vector2(Scalar(scanlinePoint0.x()), Scalar(scanlinePoint0.y())), Vector2(Scalar(scanlinePoint1.x()), Scalar(scanlinePoint1.y())))};

	// Remaining scan lines
	for (unsigned int step = 1u; /* in-loop check */; ++step)
	{
		unsigned int numberScanlinesAdded = 0u;

		const Vector2 stepVector = scanlineDirection.perpendicular() * Scalar(step * scanlineSpacing);

		for (const Vector2& center : {frameCenter - stepVector, frameCenter + stepVector})
		{
			if (computeFrameIntersection(frameWidth, frameHeight, frameBorder, Line2(center, scanlineDirection), scanlinePoint0, scanlinePoint1))
			{
				if (scanlinePoint0.sqrDistance(scanlinePoint1) >= Scalar(minimumScanlineLength * minimumScanlineLength))
				{
					scanlines.emplace_back(Vector2(Scalar(scanlinePoint0.x()), Scalar(scanlinePoint0.y())), Vector2(Scalar(scanlinePoint1.x()), Scalar(scanlinePoint1.y())));

					numberScanlinesAdded++;
				}
			}
		}

		if (numberScanlinesAdded == 0u)
		{
			break;
		}
	}

	return scanlines;
}

bool BarcodeDetector2D::extractScanlineData(const Frame& yFrame, const FiniteLine2& scanline, ScanlineData& scanlineData, CV::PixelPositionsI& scanlinePositions, const unsigned int minimumScanlineLength)
{
	ocean_assert(yFrame.isValid() && yFrame.isPixelFormatCompatible(FrameType::FORMAT_Y8) && yFrame.pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT);
	ocean_assert(scanline.isValid());

	scanlineData.clear();
	scanlinePositions.clear();

	const CV::PixelPositionI firstPoint(int(scanline.point0().x()), int(scanline.point0().y()));
	const CV::PixelPositionI lastPoint(int(scanline.point1().x()), int(scanline.point1().y()));

	for (const CV::PixelPositionI& point : {firstPoint, lastPoint})
	{
		if (point.x() < 0 || point.x() >= int(yFrame.width()) || point.y() < 0 || point.y() >= int(yFrame.height()))
		{
			ocean_assert(false && "Scan line is outside the image border - this should never happen!");
			return false;
		}
	}

	CV::Bresenham bresenham(firstPoint.x(), firstPoint.y(), lastPoint.x(), lastPoint.y());

	CV::PixelPositionI scanlinePosition = firstPoint;

	do
	{
		ocean_assert(scanlinePosition.x() >= 0 && scanlinePosition.x() < int(yFrame.width()) && scanlinePosition.y() >= 0 && scanlinePosition.y() < int(yFrame.height()));

		scanlineData.emplace_back(*yFrame.constpixel<uint8_t>((unsigned int)scanlinePosition.x(), (unsigned int)scanlinePosition.y()));
		scanlinePositions.emplace_back(scanlinePosition);

		bresenham.findNext(scanlinePosition.x(), scanlinePosition.y());
	}
	while (scanlinePosition.x() != lastPoint.x() || scanlinePosition.y() != lastPoint.y());

	// Add the last point
	scanlineData.emplace_back(*yFrame.constpixel<uint8_t>((unsigned int)scanlinePosition.x(), (unsigned int)scanlinePosition.y()));
	scanlinePositions.emplace_back(scanlinePosition);

	ocean_assert(scanlineData.size() == scanlinePositions.size());

	if (scanlineData.size() >= size_t(minimumScanlineLength))
	{
		return true;
	}

	return false;
}

template <bool tIsNormalReflectance>
bool BarcodeDetector2D::isForegroundPixel(const uint8_t pixelValue, const uint8_t grayThreshold)
{
	//             | tIsNormalReflectance | !tIsNormalReflectance
	// ------------+----------------------+------------------------
	// dark pixel  | foreground or 1      | background or 0
	// light pixel | background or 0      | foreground or 1

	if constexpr (tIsNormalReflectance)
	{
		return pixelValue < grayThreshold;
	}

	return pixelValue >= grayThreshold;
}

BarcodeDetector2D::ParserFunctionSet BarcodeDetector2D::getParserFunctions(const BarcodeTypeSet& barcodeTypeSet)
{
	typedef std::unordered_map<BarcodeType, ParserFunction> ParserFunctionMap;

	const ParserFunctionMap parserFunctionMap =
	{
		{BarcodeType::EAN_13, ParserEan13Upca::parse},
		{BarcodeType::UPC_A, ParserEan13Upca::parse},
	};

	ParserFunctionSet parserFunctionSet;

	if (barcodeTypeSet.empty())
	{
		// Return all available parser functions.
		for (const ParserFunctionMap::value_type& valuePair : parserFunctionMap)
		{
			parserFunctionSet.insert(valuePair.second);
		}
	}
	else
	{
		// Return all selected parser functions.
		for (const BarcodeTypeSet::value_type& barcodeType : barcodeTypeSet)
		{
			const ParserFunctionMap::const_iterator iter = parserFunctionMap.find(barcodeType);

			if (iter != parserFunctionMap.cend())
			{
				parserFunctionSet.insert(iter->second);
			}
		}
	}

	ocean_assert(!parserFunctionSet.empty());

	return parserFunctionSet;
}

} // namespace Barcodes

} // namespace Detector

} // namespace CV

} // namespace Ocean
