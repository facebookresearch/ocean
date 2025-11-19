/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/calibration/Utilities.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"

#include "ocean/io/JSONParser.h"

#include <fstream>

namespace Ocean
{

namespace CV
{

namespace Calibration
{

bool Utilities::paintCalibrationBoardOutline(Frame& frame, const AnyCamera& camera, const HomogenousMatrix4& board_T_camera, const MetricCalibrationBoard& calibrationBoard, const uint8_t* color, const unsigned int thickness)
{
	ocean_assert(frame.isValid() && frame.numberPlanes() == 1u && frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	if (!frame.isValid() || frame.numberPlanes() != 1u || frame.dataType() != FrameType::DT_UNSIGNED_INTEGER_8)
	{
		return false;
	}

	ocean_assert(camera.isValid() && board_T_camera.isValid() && calibrationBoard.isValid());

	if (!camera.isValid() || !board_T_camera.isValid() || !calibrationBoard.isValid())
	{
		return false;
	}

	ocean_assert(thickness >= 1u && thickness <= 7u && thickness % 2u == 1u);

	const HomogenousMatrix4 flippedBoard_T_camera(Camera::standard2InvertedFlipped(board_T_camera));

	const Scalar xMetricMarkerSize = calibrationBoard.xMetricMarkerSize();
	const Scalar zMetricMarkerSize = calibrationBoard.zMetricMarkerSize();

	const Scalar xBoardSize_2 = Scalar(calibrationBoard.xMarkers()) * xMetricMarkerSize / Scalar(2);
	const Scalar zBoardSize_2 = Scalar(calibrationBoard.yMarkers()) * zMetricMarkerSize / Scalar(2);

	const Vectors3 corners =
	{
		Vector3(-xBoardSize_2, 0, -zBoardSize_2),
		Vector3(-xBoardSize_2, 0, zBoardSize_2),
		Vector3(xBoardSize_2, 0, zBoardSize_2),
		Vector3(xBoardSize_2, 0, -zBoardSize_2),
	};

	const CameraProjectionChecker cameraProjectionChecker(camera.clone());

	for (size_t n = 0; n < corners.size(); ++n)
	{
		const Vector3& cornerA = corners[n];
		const Vector3& cornerB = corners[(n + 1) % corners.size()];

		constexpr size_t steps = 20;

		Vector2 previousPoint;
		if (!cameraProjectionChecker.projectToImageIF(flippedBoard_T_camera, cornerA, &previousPoint))
		{
			previousPoint = Vector2::minValue();
		}

		for (size_t s = 0; s < steps; ++s)
		{
			const Scalar factor = Scalar(s + 1) / Scalar(steps);

			const Vector3 nextObjectPoint = cornerA * (Scalar(1) - factor) + cornerB * factor;

			Vector2 nextImagePoint;
			if (!cameraProjectionChecker.projectToImageIF(flippedBoard_T_camera, nextObjectPoint, &nextImagePoint))
			{
				nextImagePoint = Vector2::minValue();
			}

			if (previousPoint != Vector2::minValue() && nextImagePoint != Vector2::minValue())
			{
				if (thickness <= 1u)
				{
					CV::Canvas::line<1u>(frame, previousPoint, nextImagePoint, color);
				}
				else if (thickness <= 3u)
				{
					CV::Canvas::line<3u>(frame, previousPoint, nextImagePoint, color);
				}
				else if (thickness <= 5u)
				{
					CV::Canvas::line<5u>(frame, previousPoint, nextImagePoint, color);
				}
				else
				{
					CV::Canvas::line<7u>(frame, previousPoint, nextImagePoint, color);
				}
			}

			previousPoint = nextImagePoint;
		}
	}

	return true;
}

bool Utilities::paintCalibrationBoardObservation(Frame& frame, const MetricCalibrationBoard& calibrationBoard, const CalibrationBoardObservation& calibrationBoardObservation, const bool drawCorrespondences)
{
	ocean_assert(frame.isValid() && frame.numberPlanes() == 1u && frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	if (!frame.isValid() || frame.numberPlanes() != 1u || frame.dataType() != FrameType::DT_UNSIGNED_INTEGER_8)
	{
		return false;
	}

	ocean_assert(calibrationBoardObservation.isValid());
	if (!calibrationBoardObservation.isValid())
	{
		return false;
	}

	const AnyCamera& camera = *calibrationBoardObservation.camera();
	const HomogenousMatrix4& board_T_camera = calibrationBoardObservation.board_T_camera();

	if (!paintCalibrationBoardOutline(frame, camera, board_T_camera, calibrationBoard, CV::Canvas::blue(frame.pixelFormat()), 1u))
	{
		return false;
	}

	const HomogenousMatrix4 flippedCamera_T_board = Camera::standard2InvertedFlipped(board_T_camera);

	if (drawCorrespondences)
	{
		const CalibrationBoard::ObjectPointIds& objectPointIds = calibrationBoardObservation.objectPointIds();
		const Vectors3& objectPoints = calibrationBoardObservation.objectPoints();
		const Vectors2& imagePoints = calibrationBoardObservation.imagePoints();

		for (size_t n = 0; n < objectPoints.size(); ++n)
		{
			const CalibrationBoard::ObjectPointId& objectPointId = objectPointIds[n];
			const Vector3& objectPoint = objectPoints[n];
			const Vector2& imagePoint = imagePoints[n];

			const Vector2 projectedImagePoint = camera.projectToImageIF(flippedCamera_T_board, objectPoint);

			const CalibrationBoard::BoardMarker& marker = calibrationBoard.marker(objectPointId.markerCoordinate());

			constexpr bool tOriented = true; // the object points are already oriented

			const bool sign = marker.pointSign<tOriented>(objectPointId.indexInMarker());

			const uint8_t* color = sign ? CV::Canvas::green(frame.pixelFormat()) : CV::Canvas::blue(frame.pixelFormat());

			CV::Canvas::line<1u>(frame, projectedImagePoint, imagePoint, color);
		}
	}
	else
	{
		const Vectors3 objectPoints = calibrationBoard.objectPoints();

		for (size_t nPoint = 0; nPoint < objectPoints.size(); ++nPoint)
		{
			const Vector3& objectPoint = objectPoints[nPoint];

			if (Camera::isObjectPointInFrontIF(flippedCamera_T_board, objectPoint))
			{
				const Vector2 projectedImagePoint = camera.projectToImageIF(flippedCamera_T_board, objectPoint);

				CV::Canvas::point<1u>(frame, projectedImagePoint, CV::Canvas::green(frame.pixelFormat()));
			}
		}
	}

	return true;
}

Frame Utilities::visualizeDistortionGrid(const AnyCamera& camera, const unsigned int horizontalBins, const unsigned int verticalBins, const bool undistort, Worker* worker)
{
	ocean_assert(camera.isValid());
	ocean_assert(horizontalBins != 0u && verticalBins != 0u);

	Frame frame(FrameType(camera.width(), camera.height(), FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
	frame.setValue(0xFFu);

	// horizontal lines
	for (unsigned int n = 1u; n < verticalBins; ++n)
	{
		const unsigned int y = n * frame.height() / verticalBins;
		memset(frame.row<uint8_t>(y), 0x00, frame.planeWidthBytes(0u));
	}

	// vertical lines
	for (unsigned int y = 0u; y < frame.height(); ++y)
	{
		uint8_t* const row = frame.row<uint8_t>(y);

		for (unsigned int n = 1u; n < horizontalBins; ++n)
		{
			const unsigned int x = n * frame.width() / horizontalBins;
			row[x] = 0x00u;
		}
	}

	const AnyCameraPinhole perfectCamera(PinholeCamera(camera.width(), camera.height(), camera.fovX()));

	Frame result;

	if (undistort)
	{
		if (!FrameInterpolatorBilinear::Comfort::resampleCameraImage(frame, camera, SquareMatrix3(true), perfectCamera, result, nullptr, worker))
		{
			ocean_assert(false && "This should never happen!");
		}
	}
	else
	{
		if (!FrameInterpolatorBilinear::Comfort::resampleCameraImage(frame, perfectCamera, SquareMatrix3(true), camera, result, nullptr, worker))
		{
			ocean_assert(false && "This should never happen!");
		}
	}

	return result;
}

Frame Utilities::visualizeDistortionVectors(const AnyCamera& camera, const unsigned int horizontalBins, const unsigned int verticalBins)
{
	ocean_assert(camera.isValid());
	ocean_assert(horizontalBins != 0u || verticalBins != 0u);
	ocean_assert(horizontalBins <= camera.width() / 2u && verticalBins <= camera.height() / 2u);

	Frame yFrame(FrameType(camera.width(), camera.height(), FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
	yFrame.setValue(0xFFu);

	const Scalar focalLength = (camera.focalLengthX() + camera.focalLengthY()) * Scalar(0.5);

	ocean_assert(Numeric::isNotEqualEps(focalLength));
	const Scalar invFocalLength = Scalar(1) / focalLength;

	const Scalar mx = Scalar(camera.width()) * Scalar(0.5);
	const Scalar my = Scalar(camera.height()) * Scalar(0.5);

	for (unsigned yBin = 0u; yBin < verticalBins; ++yBin)
	{
		const Scalar yUndistorted = (Scalar(yBin) + Scalar(0.5)) * Scalar(yFrame.height()) / Scalar(verticalBins);

		const Scalar yNormalized = (yUndistorted - my) * invFocalLength;

		for (unsigned int xBin = 0u; xBin < horizontalBins; ++xBin)
		{
			const Scalar xUndistorted = (Scalar(xBin) + Scalar(0.5)) * Scalar(yFrame.width()) / Scalar(horizontalBins);

			const Scalar xNormalized = (xUndistorted - mx) * invFocalLength;

			const Vector3 normalizedImagePointIF(xNormalized, yNormalized, Scalar(1));

			const Vector2 distortedImagePoint = camera.projectToImageIF(normalizedImagePointIF);

			CV::Canvas::line<1u>(yFrame, Vector2(xUndistorted, yUndistorted), distortedImagePoint, CV::Canvas::black());
		}
	}

	return yFrame;
}

Frame Utilities::visualizeDistortionValidity(const CameraProjectionChecker& cameraProjectionChecker, const bool inPixelDomain)
{
	ocean_assert(cameraProjectionChecker.isValid());

	const AnyCamera& camera = *cameraProjectionChecker.camera();

	const unsigned int width = camera.width();
	const unsigned int height = camera.height();

	const Scalar width_2 = Scalar(width) * Scalar(0.5);
	const Scalar height_2 = Scalar(height) * Scalar(0.5);

	const FiniteLines2& cameraBoundarySegments = cameraProjectionChecker.cameraBoundarySegments();

	ocean_assert(cameraBoundarySegments.size() >= 3);

	Frame yFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
	yFrame.setValue(0xFFu);

	constexpr uint8_t gray = 0x80u;

	CV::Canvas::line<1u>(yFrame, Vector2(0, height_2), Vector2(Scalar(width - 1u), height_2), &gray);
	CV::Canvas::line<1u>(yFrame, Vector2(width_2, 0), Vector2(width_2, Scalar(height - 1u)), &gray);

	if (inPixelDomain)
	{
		Vector2 previousPoint = camera.projectToImageIF(Vector3(cameraBoundarySegments.front().point0(), Scalar(1)));

		for (const FiniteLine2& cameraBoundarySegment : cameraBoundarySegments)
		{
			const Vector2 nextPoint = camera.projectToImageIF(Vector3(cameraBoundarySegment.point1(), Scalar(1)));

			constexpr uint8_t black = 0x00u;
			CV::Canvas::line<3u>(yFrame, previousPoint, nextPoint, &black);

			previousPoint = nextPoint;
		}

		constexpr uint8_t black = 0x00u;
		CV::Canvas::point<3u>(yFrame, camera.principalPoint(), &black);
	}
	else
	{
		Box2 boundingBox;

		for (const FiniteLine2& cameraBoundarySegment : cameraBoundarySegments)
		{
			boundingBox += cameraBoundarySegment.point0();
		}

		const Scalar xRadius = std::max(Numeric::abs(boundingBox.left()), Numeric::abs(boundingBox.right()));
		const Scalar yRadius = std::max(Numeric::abs(boundingBox.top()), Numeric::abs(boundingBox.bottom()));

		ocean_assert(Numeric::isNotEqualEps(xRadius));
		ocean_assert(Numeric::isNotEqualEps(yRadius));

		const Vector2 cameraCenter = Vector2(width_2, height_2);

		const Scalar xRadiusNormalization = cameraCenter.x() / xRadius;
		const Scalar yRadiusNormalization = cameraCenter.y() / yRadius;

		for (const FiniteLine2& cameraBoundarySegment : cameraBoundarySegments)
		{
			const Vector2& normalizedPoint0 = cameraBoundarySegment.point0();
			const Vector2& normalizedPoint1 = cameraBoundarySegment.point1();

			const Vector2 point0 = Vector2(normalizedPoint0.x() * xRadiusNormalization, normalizedPoint0.y() * yRadiusNormalization);
			const Vector2 point1 = Vector2(normalizedPoint1.x() * xRadiusNormalization, normalizedPoint1.y() * yRadiusNormalization);

			constexpr uint8_t black = 0x00u;
			CV::Canvas::line<3u>(yFrame, point0 + cameraCenter, point1 + cameraCenter, &black);
		}

		Vector3 objectPoint = camera.vectorIF(cameraCenter);
		ocean_assert(objectPoint.z() >= Numeric::eps());

		const Vector2 normalizedImagePoint  = objectPoint.xy() / objectPoint.z();

		constexpr uint8_t black = 0x00u;
		CV::Canvas::point<3u>(yFrame, Vector2(normalizedImagePoint.x() * xRadiusNormalization, normalizedImagePoint.y() * yRadiusNormalization) + cameraCenter, &black);
	}

	return yFrame;
}

bool Utilities::writeCalibrationBoardToFile(const CalibrationBoard& calibrationBoard, const std::string& filename, const unsigned int seed)
{
	if (!calibrationBoard.isValid())
	{
		return false;
	}

	std::ofstream file(filename, std::ios::binary);
	if (!file.is_open())
	{
		return false;
	}

	const size_t xMarkers = calibrationBoard.xMarkers();
	const size_t yMarkers = calibrationBoard.yMarkers();

	file << "{\n";

	file << "\t\"comment\": \"Camera Calibration Board, created by Ocean https://facebookresearch.github.io/ocean/\",\n\n";

	file << "\t\"hash\": \"" << String::toAStringHex(calibrationBoard.hash()) << "\",\n\n";

	if (seed != (unsigned int)(-1))
	{
		file << "\t\"seed\": " << seed << ",\n\n";
	}

	file << "\t\"xMarkers\": " << xMarkers << ",\n";
	file << "\t\"yMarkers\": " << yMarkers << ",\n\n";

	file << "\t\"markers\":\n";
	file << "\t[\n";

	for (size_t y = 0; y < yMarkers; ++y)
	{
		for (size_t x = 0; x < xMarkers; ++x)
		{
			const CalibrationBoard::BoardMarker& boardMarker = calibrationBoard.marker(x, y);

			ocean_assert(boardMarker.isValid());
			if (!boardMarker.isValid())
			{
				return false;
			}

			file << "\t\t{\n";
			file << "\t\t\t\"markerId\": " << boardMarker.markerId() << ",\n";
			file << "\t\t\t\"sign\": " << (boardMarker.sign() ? "true" : "false") << ",\n";
			file << "\t\t\t\"orientation\": " << int(boardMarker.orientation()) << ",\n";
			file << "\t\t\t\"coordinate\": {\"x\": " << boardMarker.coordinate().x() << ", \"y\": " << boardMarker.coordinate().y() << "}\n";

			// Add comma if not the last marker
			if (y < yMarkers - 1 || x < xMarkers - 1)
			{
				file << "\t\t},\n";
			}
			else
			{
				file << "\t\t}\n";
			}
		}
	}

	file << "\t]\n";
	file << "}\n";

	file.close();

	return !file.fail();
}

bool Utilities::readCalibrationBoardFromFile(const std::string& filename, CalibrationBoard& calibrationBoard)
{
	std::string errorMessage;
	IO::JSONParser::JSONValue jsonRoot = IO::JSONParser::parse(filename, std::string(), true /*strict*/, &errorMessage);

	if (!jsonRoot.isObject())
	{
		Log::error() << "Calibration::readCalibrationBoardFromFile(): " << errorMessage;
		return false;
	}

	const std::string* hashString = jsonRoot.stringFromObject("hash");
	if (hashString == nullptr)
	{
		return false;
	}

	uint64_t hash;
	if (!String::isHexValue64(hashString->c_str(), hashString->size(), false, &hash))
	{
		return false;
	}

	const double* xMarkersNumber = jsonRoot.numberFromObject("xMarkers");
	const double* yMarkersNumber = jsonRoot.numberFromObject("yMarkers");

	if (xMarkersNumber == nullptr || yMarkersNumber == nullptr)
	{
		return false;
	}

	const int32_t xMarkersSigned = NumericD::round32(*xMarkersNumber);
	const int32_t yMarkersSigned = NumericD::round32(*yMarkersNumber);

	if (xMarkersSigned <= 0 || yMarkersSigned <= 0)
	{
		return false;
	}

	const size_t xMarkers = size_t(xMarkersSigned);
	const size_t yMarkers = size_t(yMarkersSigned);

	const IO::JSONParser::JSONValue::Array* markersArray = jsonRoot.arrayFromObject("markers");

	if (markersArray == nullptr)
	{
		return false;
	}

	CalibrationBoard::BoardMarkers boardMarkers;
	boardMarkers.reserve(xMarkers * yMarkers);

	for (size_t i = 0; i < markersArray->size(); ++i)
	{
		const IO::JSONParser::JSONValue& markerValue = (*markersArray)[i];

		const double* markerIdNumber = markerValue.numberFromObject("markerId");
		const bool* signBoolean = markerValue.booleanFromObject("sign");
		const double* orientationNumber = markerValue.numberFromObject("orientation");

		if (markerIdNumber == nullptr || signBoolean == nullptr || orientationNumber == nullptr)
		{
			return false;
		}

		const IO::JSONParser::JSONValue* coordinateValue = markerValue.valueFromObject("coordinate");

		if (coordinateValue == nullptr)
		{
			return false;
		}

		const double* xNumber = coordinateValue->numberFromObject("x");
		const double* yNumber = coordinateValue->numberFromObject("y");

		if (xNumber == nullptr || yNumber == nullptr)
		{
			return false;
		}

		const int32_t markerIdSigned = NumericD::round32(*markerIdNumber);

		if (markerIdSigned < 0)
		{
			return false;
		}

		const int32_t orientationSigned = NumericD::round32(*orientationNumber);

		if (orientationSigned < 0 || orientationSigned > int32_t(CV::PixelDirection::PD_EAST))
		{
			return false;
		}

		const bool sign = *signBoolean;

		const int32_t xSigned = NumericD::round32(*xNumber);
		const int32_t ySigned = NumericD::round32(*yNumber);

		if (xSigned < 0 || size_t(xSigned) >= xMarkers)
		{
			return false;
		}

		if (ySigned < 0 || size_t(ySigned) >= yMarkers)
		{
			return false;
		}

		const size_t markerId = size_t(markerIdSigned);

		const CV::PixelDirection orientation = CV::PixelDirection(orientationSigned);

		const unsigned int x = (unsigned int)(xSigned);
		const unsigned int y = (unsigned int)(ySigned);

		const CalibrationBoard::MarkerCoordinate coordinate(x, y);

		boardMarkers.emplace_back(markerId, sign, orientation, coordinate);
	}

	calibrationBoard = CalibrationBoard(xMarkers, yMarkers, std::move(boardMarkers));

	if (calibrationBoard.hash() != hash)
	{
		Log::warning() << "Calibration::readCalibrationBoardFromFile(): Hash does not match!";
	}

	return calibrationBoard.isValid();
}

bool Utilities::createCalibrationBoardFromSeed(const std::string& calibrationBoardType, CalibrationBoard& calibrationBoard)
{
	ocean_assert(!calibrationBoardType.empty());

	const std::string lowerCalibrationBoardType = String::toLower(calibrationBoardType);

	if (!lowerCalibrationBoardType.starts_with("calibrationboard_"))
	{
		return false;
	}

	const std::string::size_type seedPosition = lowerCalibrationBoardType.find('_', 17);

	int32_t calibrationBoardSeed = -1;
	if (seedPosition == std::string::npos)
	{
		return false;
	}

	if (!String::isInteger32(lowerCalibrationBoardType.substr(17, seedPosition - 17), &calibrationBoardSeed))
	{
		return false;
	}

	if (calibrationBoardSeed < 0)
	{
		return false;
	}

	int32_t calibrationHorizontalMarkers = -1;
	int32_t calibrationVerticalMarkers = -1;

	if (!parseMarkerDimension(lowerCalibrationBoardType.substr(seedPosition + 1), calibrationHorizontalMarkers, calibrationVerticalMarkers))
	{
		return false;
	}

	ocean_assert(calibrationHorizontalMarkers >= 0 && calibrationVerticalMarkers >= 0);

	return CalibrationBoard::createCalibrationBoard((unsigned int)(calibrationBoardSeed), size_t(calibrationHorizontalMarkers), size_t(calibrationVerticalMarkers), calibrationBoard);
}

bool Utilities::parseMarkerDimension(const std::string& markerDimension, int32_t& horizontalMarkers, int32_t& verticalMarkers)
{
	ocean_assert(!markerDimension.empty());

	const std::string::size_type markersPosition = markerDimension.find('x');

	if (markersPosition == std::string::npos)
	{
		return false;
	}

	int32_t calibrationHorizontalMarkers = -1;
	if (!String::isInteger32(markerDimension.substr(0, markersPosition), &calibrationHorizontalMarkers))
	{
		return false;
	}

	int32_t calibrationVerticalMarkers = -1;
	if (!String::isInteger32(markerDimension.substr(markersPosition + 1), &calibrationVerticalMarkers))
	{
		return false;
	}

	if (calibrationHorizontalMarkers <= 0 || calibrationVerticalMarkers <= 0)
	{
		return false;
	}

	horizontalMarkers = calibrationHorizontalMarkers;
	verticalMarkers = calibrationVerticalMarkers;

	return true;
}

CalibrationBoard Utilities::createBoardForPaper(const MetricSize::PaperType paperType, const unsigned int id, MetricSize* paperWidth, MetricSize* paperHeight)
{
	if (paperWidth != nullptr || paperHeight != nullptr)
	{
		MetricSize width;
		MetricSize height;

		if (!MetricSize::determinePaperSize(paperType, width, height))
		{
			return CalibrationBoard();
		}

		if (paperWidth != nullptr)
		{
			*paperWidth = width;
		}

		if (paperHeight != nullptr)
		{
			*paperHeight = height;
		}
	}

	size_t xMarkers = 0;
	size_t yMarkers = 0;

	switch (paperType)
	{
		case MetricSize::PT_DIN_A3:
		{
			ocean_assert(Marker::numberMarkerIds() >= 55); // TODO check bits

			xMarkers = 9;
			yMarkers = 13;

			break;
		}

		case MetricSize::PT_DIN_A4:
		{
			xMarkers = 6;
			yMarkers = 9;

			break;
		}

		case MetricSize::PT_LETTER:
		{
			xMarkers = 6;
			yMarkers = 8;

			break;
		}

		case MetricSize::PT_TABLOID:
		{
			xMarkers = 8;
			yMarkers = 13;

			break;
		}

		case MetricSize::PT_INVALID:
			break;
	}

	if (xMarkers == 0 || yMarkers == 0)
	{
		ocean_assert(false && "Invalid paper type!");
		return CalibrationBoard();
	}

	CalibrationBoard calibrationBoard;

	if (!CalibrationBoard::createCalibrationBoard(id, xMarkers, yMarkers, calibrationBoard))
	{
		return CalibrationBoard();
	}

	return calibrationBoard;
}

}

}

}
