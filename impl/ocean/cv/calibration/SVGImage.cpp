/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/calibration/SVGImage.h"
#include "ocean/cv/calibration/MetricCalibrationBoard.h"

namespace Ocean
{

namespace CV
{

namespace Calibration
{

bool SVGImage::writeCalibrationBoardImage(const std::string& filename, const MetricSize& width, const MetricSize& height, const CalibrationBoard& calibrationBoard, const MetricSize::UnitType unitType, const unsigned int precision, const bool debugImage, const bool centerDot)
{
	ocean_assert(!filename.empty());
	ocean_assert(width.isValid() && height.isValid());
	ocean_assert(calibrationBoard.isValid());

	ocean_assert(unitType != MetricSize::UT_INVALID);

	std::ofstream stream(filename, std::ios::binary);

	if (!stream.is_open())
	{
		return false;
	}

	const MetricSize margin(6.0, MetricSize::UT_MILLIMETER);

	const MetricSize measurementIndicationSize(2.0, MetricSize::UT_MILLIMETER);
	const MetricSize measurementIndicationThickness(0.2, MetricSize::UT_MILLIMETER);

	constexpr double paddingFactor = MetricCalibrationBoard::paddingFactor();

	const MetricSize dotSize(0.25, MetricSize::UT_MILLIMETER);

	stream << "<svg ";

	if (!writeSize(stream, width, height, unitType, precision))
	{
		return false;
	}

	stream << " ";

	if (!writeVersion(stream))
	{
		return false;
	}

	stream << ">\n";

	if (!writeComment(stream, "\t", "Camera Calibration Board, created by Ocean https://facebookresearch.github.io/ocean/"))
	{
		return false;
	}

	const MetricSize contentWidth = width - margin * 2.0;
	const MetricSize contentHeight = height - margin * 2.0;

	const MetricSize markerSize = std::min(contentWidth / (double(calibrationBoard.xMarkers()) + 2.0 * paddingFactor), contentHeight / (double(calibrationBoard.yMarkers()) + 2.0 * paddingFactor));

	const MetricSize padding = markerSize * paddingFactor;

	const MetricSize markersWidth = markerSize * double(calibrationBoard.xMarkers());
	const MetricSize markersHeight = markerSize * double(calibrationBoard.yMarkers());

	const MetricSize markerStartLeft = (width - markersWidth) / 2.0;
	const MetricSize markerStartTop = (height - markersHeight) / 2.0;

	stream << "\n";

	if (!writeMeasurementIndication(stream, "\t", markersWidth, markersHeight, padding, markerStartLeft, markerStartTop, measurementIndicationSize, measurementIndicationThickness, "gray", unitType, precision))
	{
		return false;
	}

	for (size_t yMarker = 0; yMarker < calibrationBoard.yMarkers(); ++yMarker)
	{
		for (size_t xMarker = 0; xMarker < calibrationBoard.xMarkers(); ++xMarker)
		{
			const CalibrationBoard::BoardMarker& boardMarker = calibrationBoard.marker(xMarker, yMarker);

			ocean_assert(boardMarker.isValid());
			if (!boardMarker.isValid())
			{
				return false;
			}

			const std::string backgroundComment = boardMarker.sign() ? "White marker" : "Black marker";

			std::string orientationComment;

			switch (boardMarker.orientation())
			{
				case CV::PD_NORTH:
					 orientationComment = "oriented north (0 degree)";
					 break;

				case CV::PD_WEST:
					orientationComment = "oriented west (90 degree)";
					break;

				case CV::PD_SOUTH:
					orientationComment = "oriented south (180 degree)";
					break;

				case CV::PD_EAST:
					orientationComment = "oriented east (270 degree)";
					break;

				default:
					ocean_assert(false && "This should never happen!");
					return false;
			}

			stream << "\n";

			if (!writeComment(stream, "\t", "Marker coordinate " + String::toAString(xMarker) + ", " + String::toAString(yMarker) + ": " + backgroundComment + " with id " + String::toAString(boardMarker.markerId()) + ", " + orientationComment))
			{
				return false;
			}

			const MetricSize markerLeft = markerStartLeft + markerSize * double(xMarker);
			const MetricSize markerTop = markerStartTop + markerSize * double(yMarker);

			if (!writeBoardMarker(stream, "\t", boardMarker, markerLeft, markerTop, markerSize, dotSize, unitType, precision, debugImage))
			{
				return false;
			}
		}
	}

	if (centerDot)
	{
		// the center dot is intended for e.g., manual calibration and will be placed over all markers in the center of the board (in the origin of the calibration board)

		const MetricSize width_2 = width / 2.0;
		const MetricSize height_2 = height / 2.0;

		const MetricSize innerCenterDotRadius = dotSize * 5.0;
		const MetricSize outerCenterDotRadius = markerSize * 0.25;

		writeCircle(stream, "\t", width_2, height_2, outerCenterDotRadius, "#C0C0C0", unitType, precision);
		writeCircle(stream, "\t", width_2, height_2, innerCenterDotRadius, "black", unitType, precision);
	}

	stream << "</svg>";

	if (!stream.good())
	{
		return false;
	}

	stream.close();

	return true;
}

bool SVGImage::writeMarkerTestImage(const std::string& filename, const MetricSize& width, const MetricSize& height, const bool sign, const MetricSize::UnitType unitType, const unsigned int precision)
{
	ocean_assert(!filename.empty());
	ocean_assert(width.isValid() && height.isValid());

	ocean_assert(unitType != MetricSize::UT_INVALID);

	std::ofstream stream(filename, std::ios::binary);

	if (!stream.is_open())
	{
		return false;
	}

	const MetricSize margin(6.0, MetricSize::UT_MILLIMETER);

	const MetricSize dotSize(0.25, MetricSize::UT_MILLIMETER);

	const MetricSize markerOutlineThickness(0.25, MetricSize::UT_MILLIMETER);

	stream << "<svg ";

	if (!writeSize(stream, width, height, unitType, precision))
	{
		return false;
	}

	stream << " ";

	if (!writeVersion(stream))
	{
		return false;
	}

	stream << ">\n";

	if (!writeComment(stream, "\t", "Marker Image, created by Ocean https://facebookresearch.github.io/ocean/"))
	{
		return false;
	}

	const MetricSize contentWidth = width - margin * 2.0;
	const MetricSize contentHeight = height - margin * 2.0;

	const double contentAspectRatio = contentWidth / contentHeight;

	const size_t numberMarkerIds = Marker::numberMarkerIds();

	const double spacingFactor = 0.05;

	size_t xMarkers = 0;
	size_t yMarkers = 0;
	if (!CalibrationBoard::determineOptimalMarkerGrid(contentAspectRatio, numberMarkerIds, xMarkers, yMarkers))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	ocean_assert(xMarkers * yMarkers >= numberMarkerIds);

	const MetricSize markerSizeX = contentWidth / (double(xMarkers) + double(xMarkers - 1) * spacingFactor);
	const MetricSize markerSizeY = contentHeight / (double(yMarkers) + double(yMarkers - 1) * spacingFactor);

	const MetricSize markerSize = std::min(markerSizeX, markerSizeY);

	const MetricSize xSpacing = xMarkers == 0 ? MetricSize(0, unitType) : (contentWidth - markerSize * double(xMarkers)) / double(xMarkers - 1);
	const MetricSize ySpacing = yMarkers == 0 ? MetricSize(0, unitType) : (contentHeight - markerSize * double(yMarkers)) / double(yMarkers - 1);

	const MetricSize markerStartLeft = margin;
	const MetricSize markerStartTop = margin;

	size_t markerId = 0;

	for (size_t yMarker = 0; markerId < numberMarkerIds && yMarker < yMarkers; ++yMarker)
	{
		const MetricSize markerTop = markerStartTop + (markerSize + ySpacing) * double(yMarker);

		for (size_t xMarker = 0; markerId < numberMarkerIds && xMarker < xMarkers; ++xMarker)
		{
			const CalibrationBoard::BoardMarker boardMarker(markerId, sign, CV::PD_NORTH, CV::PixelPosition(0u, 0u));

			ocean_assert(boardMarker.isValid());
			if (!boardMarker.isValid())
			{
				return false;
			}

			stream << "\n";

			if (!writeComment(stream, "\t", "Marker id " + String::toAString(boardMarker.markerId())))
			{
				return false;
			}

			const MetricSize markerLeft = markerStartLeft + (markerSize + xSpacing) * double(xMarker);

			if (!writeBoardMarker(stream, "\t", boardMarker, markerLeft, markerTop, markerSize, dotSize, unitType, precision, true /*debugMarker*/))
			{
				return false;
			}

			if (boardMarker.sign())
			{
				// we draw a black outline around the white marker

				const MetricSize markerRight = markerLeft + markerSize;
				const MetricSize markerBottom = markerTop + markerSize;

				writeLine(stream, "\t", markerLeft, markerTop, markerLeft, markerBottom, markerOutlineThickness, "black", unitType, precision);
				writeLine(stream, "\t", markerLeft, markerBottom, markerRight, markerBottom, markerOutlineThickness, "black", unitType, precision);
				writeLine(stream, "\t", markerRight, markerBottom, markerRight, markerTop, markerOutlineThickness, "black", unitType, precision);
				writeLine(stream, "\t", markerRight, markerTop, markerLeft, markerTop, markerOutlineThickness, "black", unitType, precision);
			}

			++markerId;
		}
	}

	stream << "</svg>";

	if (!stream.good())
	{
		return false;
	}

	stream.close();

	return true;
}

bool SVGImage::writePointTestImage(const std::string& filename, const MetricSize& width, const MetricSize& height, const bool blackDots, const MetricSize::UnitType unitType, const unsigned int precision)
{
	ocean_assert(!filename.empty());
	ocean_assert(width.isValid() && height.isValid());

	ocean_assert(unitType != MetricSize::UT_INVALID);

	std::ofstream stream(filename, std::ios::binary);

	if (!stream.is_open())
	{
		return false;
	}

	const MetricSize margin(6.0, MetricSize::UT_MILLIMETER);

	const MetricSize contentWidth = width - margin * 2.0;
	const MetricSize contentHeight = height - margin * 2.0;

	const MetricSize minDotRadius(0.1, MetricSize::UT_MILLIMETER);
	const MetricSize maxDotRadius(2.5, MetricSize::UT_MILLIMETER);

	stream << "<svg ";

	if (!writeSize(stream, width, height, unitType, precision))
	{
		return false;
	}

	stream << " ";

	if (!writeVersion(stream))
	{
		return false;
	}

	stream << ">\n";

	if (!writeComment(stream, "\t", "Point Test Image, created by Ocean https://facebookresearch.github.io/ocean/"))
	{
		return false;
	}

	if (!blackDots)
	{
		stream << "\n";
		if (!writeRectangle(stream, "\t", MetricSize(0.0, MetricSize::UT_MILLIMETER), MetricSize(0.0, MetricSize::UT_MILLIMETER), width, height, "black", unitType, precision))
		{
			return false;
		}
	}

	const std::string dotColor = blackDots ? "black" : "white";

	constexpr double minSpacingBetweenDotsFactor = 4.0;

	const MetricSize minOffsetBetweenDots = maxDotRadius * (2.0 + minSpacingBetweenDotsFactor);

	const size_t numberDotColumns = size_t(NumericD::floor(((contentWidth - maxDotRadius * 2.0) / minOffsetBetweenDots))) + 1;
	const size_t numberDotRows = size_t(NumericD::floor(((contentHeight - maxDotRadius * 2.0) / minOffsetBetweenDots))) + 1;

	const MetricSize xDotArea = maxDotRadius * 2.0 + minOffsetBetweenDots * double(numberDotColumns - 1);
	const MetricSize yDotArea = maxDotRadius * 2.0 + minOffsetBetweenDots * double(numberDotRows - 1);

	ocean_assert(xDotArea <= contentWidth && yDotArea <= contentHeight);

	const MetricSize xSpacing = minOffsetBetweenDots + (contentWidth - xDotArea) / double(numberDotColumns - 1);
	const MetricSize ySpacing = minOffsetBetweenDots + (contentHeight - yDotArea) / double(numberDotRows - 1);

	ocean_assert(minOffsetBetweenDots <= xSpacing && minOffsetBetweenDots <= ySpacing);

	const size_t numberDots = numberDotRows * numberDotColumns;

	size_t dotIndex = 0;

	for (size_t nDotRow = 0; nDotRow < numberDotRows; ++nDotRow)
	{
		const MetricSize yDot = margin + maxDotRadius + ySpacing * double(nDotRow);

		stream << "\n";
		writeComment(stream, "\t", "Row with index " + String::toAString(nDotRow));

		for (size_t nDotColumn = 0; nDotColumn < numberDotColumns; ++nDotColumn)
		{
			const MetricSize xDot = margin + maxDotRadius + xSpacing * double(nDotColumn);

			const MetricSize dotRadius = minDotRadius + (maxDotRadius - minDotRadius) * double(dotIndex++) / double(numberDots - 1);

			writeCircle(stream, "\t", xDot, yDot, dotRadius, dotColor, unitType, precision);
		}
	}

	stream << "</svg>";

	if (!stream.good())
	{
		return false;
	}

	stream.close();

	return true;
}

bool SVGImage::writeVersion(std::ostream& stream, const unsigned int majorVersion, const unsigned int minorVersion)
{
	ocean_assert(stream.good());
	ocean_assert(majorVersion >= 1u && minorVersion >= 1u);

	if (!stream.good() || majorVersion < 1u || (majorVersion == 1u && minorVersion < 1u))
	{
		return false;
	}

	stream << "version=\"" << majorVersion << "." << minorVersion << "\" xmlns=\"http://www.w3.org/2000/svg\"";

	return stream.good();
}

bool SVGImage::writeValue(std::ostream& stream, const std::string& name, const MetricSize& value, const MetricSize::UnitType unitType, const unsigned int precision)
{
	ocean_assert(stream.good());
	if (!stream.good())
	{
		return false;
	}

	stream << name << "=" << value.value(unitType, precision);

	return stream.good();
}

bool SVGImage::writeValue(std::ostream& stream, const std::string& name, const std::string& value)
{
	ocean_assert(stream.good());
	if (!stream.good())
	{
		return false;
	}

	stream << name << "=\"" << value << "\"";

	return stream.good();
}

bool SVGImage::writePosition(std::ostream& stream, const MetricSize& x, const MetricSize& y, const MetricSize::UnitType unitType, const unsigned int precision, const std::string& xName, const std::string& yName)
{
	ocean_assert(stream.good());
	if (!stream.good())
	{
		return false;
	}

	if (!writeValue(stream, xName, x, unitType, precision))
	{
		return false;
	}

	stream << " ";

	if (!writeValue(stream, yName, y, unitType, precision))
	{
		return false;
	}

	return stream.good();
}

bool SVGImage::writeSize(std::ostream& stream, const MetricSize& width, const MetricSize& height, const MetricSize::UnitType unitType, const unsigned int precision, const std::string& widthName, const std::string& heightName)
{
	ocean_assert(stream.good());
	if (!stream.good())
	{
		return false;
	}

	if (!writeValue(stream, widthName, width, unitType, precision))
	{
		return false;
	}

	stream << " ";

	if (!writeValue(stream, heightName, height, unitType, precision))
	{
		return false;
	}

	return stream.good();
}

bool SVGImage::writeRectangle(std::ostream& stream, const std::string& indentation, const MetricSize& left, const MetricSize& top, const MetricSize& width, const MetricSize& height, const std::string& color, const MetricSize::UnitType unitType, const unsigned int precision)
{
	ocean_assert(stream.good());
	if (!stream.good())
	{
		return false;
	}

	stream << indentation << "<rect ";

	if (!writePosition(stream, left, top, unitType, precision))
	{
		return false;
	}

	stream << " ";

	if (!writeSize(stream, width, height, unitType, precision))
	{
		return false;
	}

	stream << " ";

	if (!writeValue(stream, "fill", color))
	{
		return false;
	}

	stream << " />\n";

	return stream.good();
}

bool SVGImage::writeCircle(std::ostream& stream, const std::string& indentation, const MetricSize& x, const MetricSize& y, const MetricSize& radius, const std::string& color, const MetricSize::UnitType unitType, const unsigned int precision)
{
	ocean_assert(stream.good());
	if (!stream.good())
	{
		return false;
	}

	stream << indentation << "<circle ";

	if (!writePosition(stream, x, y, unitType, precision, "cx", "cy"))
	{
		return false;
	}

	stream << " ";

	if (!writeValue(stream, "r", radius, unitType, precision))
	{
		return false;
	}

	stream << " ";

	if (!writeValue(stream, "fill", color))
	{
		return false;
	}

	stream << " />\n";

	return stream.good();
}

bool SVGImage::writeLine(std::ostream& stream, const std::string& indentation, const MetricSize& x1, const MetricSize& y1, const MetricSize& x2, const MetricSize& y2, const MetricSize& thickness, const std::string& color, const MetricSize::UnitType unitType, const unsigned int precision)
{
	ocean_assert(stream.good());
	if (!stream.good())
	{
		return false;
	}

	stream << indentation << "<line ";

	if (!writePosition(stream, x1, y1, unitType, precision, "x1", "y1"))
	{
		return false;
	}

	stream << " ";

	if (!writePosition(stream, x2, y2, unitType, precision, "x2", "y2"))
	{
		return false;
	}

	stream << " ";

	if (!writeValue(stream, "stroke", color))
	{
		return false;
	}

	stream << " ";

	if (!writeValue(stream, "stroke-width", thickness, unitType, precision))
	{
		return false;
	}

	stream << " />\n";

	return stream.good();
}

bool SVGImage::writeText(std::ostream& stream, const std::string& indentation, const std::string& text, const MetricSize& x, const MetricSize& y, const MetricSize& size, const std::string& color, const MetricSize::UnitType unitType, const unsigned int precision, const std::string& fontFamily)
{
	ocean_assert(stream.good());
	if (!stream.good())
	{
		return false;
	}

	ocean_assert(!text.empty());
	ocean_assert(size > MetricSize(0, unitType));

	stream << indentation << "<text ";

	if (!writePosition(stream, x, y, unitType, precision))
	{
		return false;
	}

	stream << " ";

	if (!writeValue(stream, "font-size", size, unitType, precision))
	{
		return false;
	}

	stream << " ";

	if (!writeValue(stream, "fill", color))
	{
		return false;
	}

	if (!fontFamily.empty())
	{
		stream << " ";

		if (!writeValue(stream, "font-family", fontFamily))
		{
			return false;
		}
	}

	stream << ">\n";
	stream << indentation << "\t" << text << "\n";
	stream << indentation << "</text>\n";

	return stream.good();
}

bool SVGImage::writeComment(std::ostream& stream, const std::string& indentation, const std::string& comment)
{
	ocean_assert(stream.good());
	if (!stream.good())
	{
		return false;
	}

	stream << indentation << "<!-- " << comment << " -->\n";

	return stream.good();
}

bool SVGImage::writeBoardMarker(std::ostream& stream, const std::string& indentation, const CalibrationBoard::BoardMarker& boardMarker, const MetricSize& left, const MetricSize& top, const MetricSize& markerSize, const MetricSize& dotRadius, const MetricSize::UnitType unitType, const unsigned int precision, const bool debugMarker)
{
	assert(stream.good());
	if (!stream.good())
	{
		return false;
	}

	ocean_assert(boardMarker.isValid());
	ocean_assert(markerSize > MetricSize(0, unitType));
	ocean_assert(dotRadius > MetricSize(0, unitType));

	constexpr size_t numberDots = 5;

	const MetricSize dotDistance = markerSize / double(numberDots);
	const MetricSize dotOffset = markerSize / double(numberDots * 2);

	const bool whiteBackground = boardMarker.sign(); // positive sign means black dots with white background, negative sign means white dots with black background

	if (!whiteBackground)
	{
		if (!writeRectangle(stream, indentation, left, top, markerSize, markerSize, "black", unitType, precision))
		{
			return false;
		}
	}

	size_t dotIndex = 0;

	for (size_t yDot = 0; yDot < numberDots; ++yDot)
	{
		const MetricSize y = top + dotDistance * double(yDot);

		for (size_t xDot = 0; xDot < numberDots; ++xDot)
		{
			const MetricSize x = left + dotDistance * double(xDot);

			const MetricSize radius = dotIndex % 2 == 0 ? dotRadius : dotRadius * 2.0;

			const bool whiteElement = boardMarker.pointSign<false /*tOriented*/>(xDot, yDot);
			const bool whitePoint = !whiteElement;

			if (whiteBackground == whitePoint)
			{
				if (!writeRectangle(stream, indentation, x, y, dotDistance, dotDistance, whitePoint ? "black" : "white", unitType, precision))
				{
					return false;
				}
			}

			const bool skipCircle = debugMarker && xDot == 0 && yDot == 0;

			if (!skipCircle)
			{
				if (!writeCircle(stream, indentation, x + dotOffset, y + dotOffset, radius, whitePoint ? "white" : "black", unitType, precision))
				{
					return false;
				}
			}

			++dotIndex;
		}
	}

	if (debugMarker)
	{
		const MetricSize textSize = (markerSize / 5.0) * 0.75;
		const MetricSize textOffset = MetricSize(1.0, MetricSize::UT_MILLIMETER);

		writeText(stream, indentation, String::toAString(boardMarker.markerId()), left + textOffset, top + textSize, textSize, boardMarker.sign() ? "black" : "white", unitType, precision);
	}

	return stream.good();
}

bool SVGImage::writeMeasurementIndication(std::ostream& stream, const std::string& indentation, const MetricSize& markersWidth, const MetricSize& markersHeight, const MetricSize& padding, const MetricSize& markerStartLeft, const MetricSize& markerStartTop, const MetricSize& measurementIndicationSize, const MetricSize& measurementIndicationThickness, const std::string& color, const MetricSize::UnitType unitType, const unsigned int precision)
{
	assert(stream.good());
	if (!stream.good())
	{
		return false;
	}

	ocean_assert(markersWidth.isValid() && markersHeight.isValid());
	ocean_assert(padding.isValid());
	ocean_assert(markerStartLeft.isValid() && markerStartTop.isValid());
	ocean_assert(measurementIndicationSize.isValid() && measurementIndicationThickness.isValid());

	const MetricSize measurementIndicationSize_2 = measurementIndicationSize / 2.0;

	const std::array<MetricSize, 4> xPositions =
	{
		markerStartLeft - padding,
		markerStartLeft - padding,
		markerStartLeft + markersWidth + padding,
		markerStartLeft + markersWidth + padding
	};

	const std::array<MetricSize, 4> yPositions =
	{
		markerStartTop - padding,
		markerStartTop + markersHeight + padding,
		markerStartTop + markersHeight + padding,
		markerStartTop - padding
	};

	for (size_t n = 0; n < 4; ++n)
	{
		const MetricSize& x = xPositions[n];
		const MetricSize& y = yPositions[n];

		if (!writeLine(stream, indentation, x, y - measurementIndicationSize_2, x, y + measurementIndicationSize_2, measurementIndicationThickness, color, unitType, precision))
		{
			return false;
		}

		if (!writeLine(stream, indentation, x - measurementIndicationSize_2, y, x + measurementIndicationSize_2, y, measurementIndicationThickness, color, unitType, precision))
		{
			return false;
		}

		stream << "\n";
	}

	return stream.good();
}

}

}

}
