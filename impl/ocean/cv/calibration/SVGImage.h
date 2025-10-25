/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_CALIBRATION_SVG_IMAGE_H
#define META_OCEAN_CV_CALIBRATION_SVG_IMAGE_H

#include "ocean/cv/calibration/Calibration.h"
#include "ocean/cv/calibration/CalibrationBoard.h"
#include "ocean/cv/calibration/MetricSize.h"

#include <iostream>

namespace Ocean
{

namespace CV
{

namespace Calibration
{

/**
 * This class implements functions to write SVG images.
 * @ingroup cvcalibration
 */
class OCEAN_CV_CALIBRATION_EXPORT SVGImage
{
	public:

		/**
		 * Writes a SVG image with a calibration board.
		 * @param filename The filename of the SVG image to be written, must be valid
		 * @param width The width of the image (or the paper the image is printed on), must be valid
		 * @param height The height of the image (or the paper the image is printed on), must be valid
		 * @param calibrationBoard The calibration board from which the SVG image will be created, must be valid
		 * @param unitType The unit type which will be used in the resulting SVG image, must be valid
		 * @param precision The number of decimal places to use, with range [0, infinity)
		 * @param debugImage True, to create a debug image with additional information
		 * @param centerDot True, to add a dot in the center of the calibration board (e.g., for manual calibration)
		 * @return True, if succeeded
		 */
		static bool writeCalibrationBoardImage(const std::string& filename, const MetricSize& width, const MetricSize& height, const CalibrationBoard& calibrationBoard, const MetricSize::UnitType unitType = MetricSize::UT_MILLIMETER, const unsigned int precision = 4u, const bool debugImage = false, const bool centerDot = false);

		/**
		 * Writes a SVG test image holding all possible markers.
		 * @param filename The filename of the SVG image to be written, must be valid
		 * @param width The width of the image (or the paper the image is printed on), must be valid
		 * @param height The height of the image (or the paper the image is printed on), must be valid
		 * @param sign True, to create an image with black points (and white markers); False, to create an image with white points (and black markers)
		 * @param unitType The unit type which will be used in the resulting SVG image, must be valid
		 * @param precision The number of decimal places to use, with range [0, infinity)
		 * @return True, if succeeded
		 */
		static bool writeMarkerTestImage(const std::string& filename, const MetricSize& width, const MetricSize& height, const bool sign = true, const MetricSize::UnitType unitType = MetricSize::UT_MILLIMETER, const unsigned int precision = 4u);

		/**
		 * Write a SVG test image showing points with different sizes.
		 * @param filename The filename of the SVG image to be written, must be valid
		 * @param width The width of the image (or the paper the image is printed on), must be valid
		 * @param height The height of the image (or the paper the image is printed on), must be valid
		 * @param blackDots True, to create an image with black dots (and white background); False, to create an image with white dots (and black background)
		 * @param unitType The unit type which will be used in the resulting SVG image, must be valid
		 * @param precision The number of decimal places to use, with range [0, infinity)
		 * @return True, if succeeded
		 */
		static bool writePointTestImage(const std::string& filename, const MetricSize& width, const MetricSize& height, const bool blackDots, const MetricSize::UnitType unitType = MetricSize::UT_MILLIMETER, const unsigned int precision = 4u);

	protected:

		/**
		 * Writes the SVG version to the SVG file.
		 * @param stream The stream to which the SVG image will be written, must be valid
		 * @param majorVersion The major version of the SVG image, with range [1, infinity)
		 * @param minorVersion The minor version of the SVG image, with range [0, infinity)
		 * @return True, if succeeded
		 */
		static bool writeVersion(std::ostream& stream, const unsigned int majorVersion = 1u, const unsigned int minorVersion = 1u);

		/**
		 * Writes a value to the SVG file.
		 * @param stream The stream to which the SVG image will be written, must be valid
		 * @param name The name of the value, must be valid
		 * @param value The value to be written, must be valid
		 * @param unitType The unit type which will be used, must be valid
		 * @param precision The number of decimal places to use, with range [0, infinity)
		 * @return True, if succeeded
		 */
		static bool writeValue(std::ostream& stream, const std::string& name, const MetricSize& value, const MetricSize::UnitType unitType, const unsigned int precision);

		/**
		 * Writes a value to the SVG file.
		 * @param stream The stream to which the SVG image will be written, must be valid
		 * @param name The name of the value, must be valid
		 * @param value The value to be written, must be valid
		 * @return True, if succeeded
		 */
		static bool writeValue(std::ostream& stream, const std::string& name, const std::string& value);

		/**
		 * Writes a position to the SVG file.
		 * @param stream The stream to which the SVG image will be written, must be valid
		 * @param x The x value of the position, must be valid
		 * @param y The y value of the position, must be valid
		 * @param unitType The unit type which will be used, must be valid
		 * @param precision The number of decimal places to use, with range [0, infinity)
		 * @param xName The name of the x value, must be valid
		 * @param yName The name of the y value, must be valid
		 * @return True, if succeeded
		 */
		static bool writePosition(std::ostream& stream, const MetricSize& x, const MetricSize& y, const MetricSize::UnitType unitType, const unsigned int precision, const std::string& xName = "x", const std::string& yName = "y");

		/**
		 * Writes a size/dimension to the SVG file.
		 * @param stream The stream to which the SVG image will be written, must be valid
		 * @param width The width of the size, must be valid
		 * @param height The height of the size, must be valid
		 * @param unitType The unit type which will be used, must be valid
		 * @param precision The number of decimal places to use, with range [0, infinity)
		 * @param widthName The name of the width value, must be valid
		 * @param heightName The name of the height value, must be valid
		 * @return True, if succeeded
		 */
		static bool writeSize(std::ostream& stream, const MetricSize& width, const MetricSize& height, const MetricSize::UnitType unitType, const unsigned int precision, const std::string& widthName = "width", const std::string& heightName = "height");

		/**
		 * Writes a rectangle to the SVG file.
		 * @param stream The stream to which the SVG image will be written, must be valid
		 * @param indentation The optional indentation within the SVG file when writing the text, can be empty
		 * @param left The left position of the rectangle, must be valid
		 * @param top The top position of the rectangle, must be valid
		 * @param width The width of the rectangle, must be valid
		 * @param height The height of the rectangle, must be valid
		 * @param color The filling color of the rectangle, must be valid
		 * @param unitType The unit type which will be used, must be valid
		 * @param precision The number of decimal places to use, with range [0, infinity)
		 * @return True, if succeeded
		 */
		static bool writeRectangle(std::ostream& stream, const std::string& indentation, const MetricSize& left, const MetricSize& top, const MetricSize& width, const MetricSize& height, const std::string& color, const MetricSize::UnitType unitType, const unsigned int precision);

		/**
		 * Writes a circle to the SVG file.
		 * @param stream The stream to which the SVG image will be written, must be valid
		 * @param indentation The optional indentation within the SVG file when writing the text, can be empty
		 * @param x The x position of the center of the circle, must be valid
		 * @param y The y position of the center of the circle, must be valid
		 * @param radius The radius of the circle, must be valid
		 * @param color The filling color of the circle, must be valid
		 * @param unitType The unit type which will be used, must be valid
		 * @param precision The number of decimal places to use, with range [0, infinity)
		 * @return True, if succeeded
		 */
		static bool writeCircle(std::ostream& stream, const std::string& indentation, const MetricSize& x, const MetricSize& y, const MetricSize& radius, const std::string& color, const MetricSize::UnitType unitType, const unsigned int precision);

		/**
		 * Writes a line to the SVG file.
		 * @param stream The stream to which the SVG image will be written, must be valid
		 * @param indentation The optional indentation within the SVG file when writing the text, can be empty
		 * @param x1 The x position of the start of the line, must be valid
		 * @param y1 The y position of the start of the line, must be valid
		 * @param x2 The x position of the end of the line, must be valid
		 * @param y2 The y position of the end of the line, must be valid
		 * @param thickness The thickness of the line, must be valid
		 * @param color The color of the line, must be valid
		 * @param unitType The unit type which will be used, must be valid
		 * @param precision The number of decimal places to use, with range [0, infinity)
		 * @return True, if succeeded
		 */
		static bool writeLine(std::ostream& stream, const std::string& indentation, const MetricSize& x1, const MetricSize& y1, const MetricSize& x2, const MetricSize& y2, const MetricSize& thickness, const std::string& color, const MetricSize::UnitType unitType, const unsigned int precision);

		/**
		 * Writes a text to the SVG file.
		 * @param stream The stream to which the SVG image will be written, must be valid
		 * @param indentation The optional indentation within the SVG file when writing the text, can be empty
		 * @param text The text to be written, must be valid
		 * @param x The x position of the text, must be valid
		 * @param y The y position of the text, must be valid
		 * @param size The size of the text, must be valid
		 * @param color The color of the text, must be valid
		 * @param unitType The unit type which will be used, must be valid
		 * @param precision The number of decimal places to use, with range [0, infinity)
		 * @param fontFamily The font family of the text, can be empty
		 * @return True, if succeeded
		 */
		static bool writeText(std::ostream& stream, const std::string& indentation, const std::string& text, const MetricSize& x, const MetricSize& y, const MetricSize& size, const std::string& color, const MetricSize::UnitType unitType, const unsigned int precision, const std::string& fontFamily = std::string());

		/**
		 * Writes a comment to the SVG file.
		 * @param stream The stream to which the SVG image will be written, must be valid
		 * @param indentation The optional indentation within the SVG file when writing the markers, can be empty
		 * @param comment The comment to be written, must be valid
		 */
		static bool writeComment(std::ostream& stream, const std::string& indentation, const std::string& comment);

		/**
		 * Writes the board markers of a calibration board to the SVG file.
		 * @param stream The stream to which the SVG image will be written, must be valid
		 * @param indentation The optional indentation within the SVG file when writing the markers, can be empty
		 * @param boardMarkers The board markers to be written, must be valid
		 * @param left The left position of the first marker, must be valid
		 * @param top The top position of the first marker, must be valid
		 * @param markerSize The size of the markers (the edge length of each marker), must be valid
		 * @param dotRadius The radius of the dots within the markers, must be valid
		 * @param unitType The unit type which will be used in the resulting SVG image, must be valid
		 * @param precision The number of decimal places to use, with range [0, infinity)
		 * @param debugMarker True, to create a debug marker with additional information
		 * @return True, if succeeded
		 */
		static bool writeBoardMarker(std::ostream& stream, const std::string& indentation, const CalibrationBoard::BoardMarker& boardMarker, const MetricSize& left, const MetricSize& top, const MetricSize& markerSize, const MetricSize& dotRadius, const MetricSize::UnitType unitType, const unsigned int precision, const bool debugMarker = false);

		/**
		 * Writes four measurement indications at the corners of a calibration board.
		 * @param stream The stream to which the SVG image will be written, must be valid
		 * @param indentation The optional indentation within the SVG file when writing the measurement indications, can be empty
		 * @param markersWidth The width of all markers, must be valid
		 * @param markersHeight The height of all markers, must be valid
		 * @param padding The padding around the markers (the offset between indication and marker), must be valid
		 * @param markerStartLeft The left position of the first marker, must be valid
		 * @param markerStartTop The top position of the first marker, must be valid
		 * @param measurementIndicationSize The size/length of the measurement indications, must be valid
		 * @param measurementIndicationThickness The thickness of the measurement indications, must be valid
		 * @param color The color of the measurement indications, must be valid
		 * @param unitType The unit type which will be used in the resulting SVG image, must be valid
		 * @param precision The number of decimal places to use, with range [0, infinity)
		 * @return True, if succeeded
		 */
		static bool writeMeasurementIndication(std::ostream& stream, const std::string& indentation, const MetricSize& markersWidth, const MetricSize& markersHeight, const MetricSize& padding, const MetricSize& markerStartLeft, const MetricSize& markerStartTop, const MetricSize& measurementIndicationSize, const MetricSize& measurementIndicationThickness, const std::string& color, const MetricSize::UnitType unitType, const unsigned int precision);
};

}

}

}

#endif // META_OCEAN_CV_CALIBRATION_SVG_IMAGE_H
