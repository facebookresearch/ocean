/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/calibration/CalibrationDebugElements.h"
#include "ocean/cv/calibration/Utilities.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameInterpolatorNearestPixel.h"

namespace Ocean
{

namespace CV
{

namespace Calibration
{

void CalibrationDebugElements::updatePointDetectorPointsOptimizationPointPatterns(const PointDetector::PointPatterns& pointPatterns, const unsigned int imageSize)
{
	if (!isElementActive(EI_POINT_DETECTOR_POINTS_OPTIMIZATION_POINT_PATTERNS))
	{
		return;
	}

	Frame yPointPatternImages(FrameType(imageSize * 3u, imageSize * (unsigned int)(pointPatterns.size()) , FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
	yPointPatternImages.setValue(0xFFu);

	for (unsigned int nPointPattern = 0; nPointPattern < pointPatterns.size(); ++nPointPattern)
	{
		Frame yDarkPointPatternImage = yPointPatternImages.subFrame(imageSize, imageSize * nPointPattern, imageSize, imageSize);
		Frame yBrightPointPatternImage = yPointPatternImages.subFrame(imageSize * 2u, imageSize * nPointPattern, imageSize, imageSize);

		const PointDetector::PointPattern& pointPattern = pointPatterns[nPointPattern];

		PointDetector::paintPointPattern(yDarkPointPatternImage, pointPattern.radius(), 0x00u);
		PointDetector::paintPointPattern(yBrightPointPatternImage, pointPattern.radius(), 0xFFu);

		CV::Canvas::drawText(yPointPatternImages, String::toAString(pointPattern.radius()), 5, 5 + int(imageSize * nPointPattern), CV::Canvas::black(), CV::Canvas::white());
	}

	updateElement(EI_POINT_DETECTOR_POINTS_OPTIMIZATION_POINT_PATTERNS, std::move(yPointPatternImages));
}

void CalibrationDebugElements::updatePointDetectorPointsOptimization(const Frame& yFrame, const Vectors2& pointsMovedFrom, const Vectors2& pointsMovedTo, const Vectors2& pointsFlipped, const Vectors2& pointsFailed)
{
	if (!isElementActive(EI_POINT_DETECTOR_POINTS_OPTIMIZATION))
	{
		return;
	}

	Frame rgbFrame;
	if (!CV::FrameConverter::Comfort::convert(yFrame, FrameType::FORMAT_RGB24, rgbFrame, CV::FrameConverter::CP_ALWAYS_COPY))
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	for (unsigned int y = 0u; y < rgbFrame.height(); ++y)
	{
		uint8_t* row = rgbFrame.row<uint8_t>(y);

		for (unsigned int x = 0u; x < rgbFrame.width() * 3u; ++x)
		{
			row[x] /= 2u;
		}
	}

	ocean_assert(pointsMovedFrom.size() == pointsMovedTo.size());
	if (pointsMovedFrom.size() == pointsMovedTo.size())
	{
		for (size_t n = 0; n < pointsMovedFrom.size(); ++n)
		{
			CV::Canvas::line<3u>(rgbFrame, pointsMovedFrom[n], pointsMovedTo[n], CV::Canvas::green());

			CV::Canvas::point<3u>(rgbFrame, pointsMovedFrom[n], CV::Canvas::yellow());
		}
	}

	for (const Vector2& flipped : pointsFlipped)
	{
		CV::Canvas::point<7u>(rgbFrame, flipped, CV::Canvas::red());
	}

	for (const Vector2& failed : pointsFailed)
	{
		CV::Canvas::point<7u>(rgbFrame, failed, CV::Canvas::blue());
	}

	updateElement(EI_POINT_DETECTOR_POINTS_OPTIMIZATION, std::move(rgbFrame));
}

void CalibrationDebugElements::updateCameraCalibratorMarkerCandidates(const ElementId elementId, const Frame& yFrame, const Points& points, const MarkerCandidates& markerCandidates)
{
	ocean_assert(elementId == EI_CAMERA_CALIBRATOR_MARKER_CANDIDATES || elementId == EI_CAMERA_CALIBRATOR_MARKER_CANDIDATES_WITH_IDS || elementId == EI_CAMERA_CALIBRATOR_MARKER_CANDIDATES_WITH_IDS_WITH_COORDINATES);

	if (!isElementActive(elementId))
	{
		return;
	}

	Frame rgbFrame;
	if (!CV::FrameConverter::Comfort::convert(yFrame, FrameType::FORMAT_RGB24, rgbFrame, CV::FrameConverter::CP_ALWAYS_COPY))
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	for (const MarkerCandidate& markerCandidate : markerCandidates)
	{
		ocean_assert(markerCandidate.isValid());
		ocean_assert(markerCandidate.hasSign());

		const uint8_t* color = markerCandidate.sign() ? CV::Canvas::green() : CV::Canvas::red();

		for (size_t borderIndex = 0; borderIndex < 16; ++borderIndex)
		{
			const Index32 pointIndexA = markerCandidate.borderIndex(borderIndex);
			const Index32 pointIndexB = markerCandidate.borderIndex((borderIndex + 1) % 16);

			const CV::Calibration::Point& pointA = points[pointIndexA];
			const CV::Calibration::Point& pointB = points[pointIndexB];

			CV::Canvas::line<1u>(rgbFrame, pointA.observation(), pointB.observation(), color);
		}

		for (const CV::Calibration::MarkerCandidate::NeighborMap::value_type& neighborPair : markerCandidate.neighbors())
		{
			CV::Canvas::line<3u>(rgbFrame, markerCandidate.center(points), markerCandidates[neighborPair.second].center(points), CV::Canvas::gray());
		}
	}

	if (elementId == EI_CAMERA_CALIBRATOR_MARKER_CANDIDATES_WITH_IDS || elementId == EI_CAMERA_CALIBRATOR_MARKER_CANDIDATES_WITH_IDS_WITH_COORDINATES)
	{
		for (const MarkerCandidate& markerCandidate : markerCandidates)
		{
			ocean_assert(markerCandidate.hasMarkerId());

			CV::Canvas::point<11u>(rgbFrame, points[markerCandidate.pointIndex(0)].observation(), CV::Canvas::yellow());

			const Vector2 center = markerCandidate.center(points);

			const size_t markerId = markerCandidate.markerId();

			int x = Numeric::round32(center.x());
			int y = Numeric::round32(center.y());

			std::string text = String::toAString(markerId);

			if (elementId == EI_CAMERA_CALIBRATOR_MARKER_CANDIDATES_WITH_IDS_WITH_COORDINATES)
			{
				if (markerCandidate.hasMarkerCoordinate())
				{
					const CV::PixelPosition& markerCoordinate = markerCandidate.markerCoordinate();

					text += "\n" + String::toAString(markerCoordinate.x()) + ", " + String::toAString(markerCoordinate.y());
				}
				else
				{
					text += "\nno coord";
				}

				x -= 20;
				y -= 20;
			}

			CV::Canvas::drawText(rgbFrame, text, x, y, CV::Canvas::black(), CV::Canvas::white());
		}
	}

	updateElement(elementId, std::move(rgbFrame));
}

void CalibrationDebugElements::updateCameraCalibratorInitialCameraPoseWithValidMarkerCandidates(const ElementId elementId, const Frame& yFrame, const Points& points, const MarkerCandidates& markerCandidates, const Indices32& usedInitialMarkerCandidateIndices, const MetricCalibrationBoard& calibrationBoard, const AnyCamera& camera, const HomogenousMatrix4& board_T_camera)
{
	if (!isElementActive(elementId))
	{
		return;
	}

	Frame rgbFrame;
	if (!CV::FrameConverter::Comfort::convert(yFrame, FrameType::FORMAT_RGB24, rgbFrame, CV::FrameConverter::CP_ALWAYS_COPY))
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	for (unsigned int y = 0u; y < rgbFrame.height(); ++y)
	{
		uint8_t* row = rgbFrame.row<uint8_t>(y);

		for (unsigned int x = 0u; x < rgbFrame.width() * 3u; ++x)
		{
			row[x] /= 2u;
		}
	}

	Utilities::paintCalibrationBoardOutline(rgbFrame, camera, board_T_camera, calibrationBoard, CV::Canvas::blue());

	const HomogenousMatrix4 flippedCamera_T_board = Camera::standard2InvertedFlipped(board_T_camera);

	for (const Index32 usedInitialMarkerCandidateIndex : usedInitialMarkerCandidateIndices)
	{
		const MarkerCandidate& markerCandidate = markerCandidates[usedInitialMarkerCandidateIndex];

		ocean_assert(markerCandidate.isValid());
		ocean_assert(markerCandidate.hasMarkerCoordinate());

		for (size_t indexInMarker = 0; indexInMarker < 25; ++indexInMarker)
		{
			const Vector3 boardPoint = calibrationBoard.objectPoint(markerCandidate.markerCoordinate(), indexInMarker);
			const Vector2 projectedBoardPoint = camera.projectToImageIF(flippedCamera_T_board, boardPoint);

			const Index32 pointIndex = markerCandidate.pointIndex(indexInMarker);
			ocean_assert(pointIndex < points.size());

			const Point& point = points[pointIndex];

			const Vector2& imagePoint = point.observation();

			const bool sign = point.sign();

#ifdef OCEAN_DEBUG
			const CalibrationBoard::BoardMarker& marker = calibrationBoard.marker(markerCandidate.markerCoordinate());
			ocean_assert(sign == marker.pointSign<true /*tOriented*/>(indexInMarker));
#endif

			const uint8_t* color = sign ? CV::Canvas::green() : CV::Canvas::blue();

			CV::Canvas::line<3u>(rgbFrame, projectedBoardPoint, imagePoint, color);
		}
	}

	updateElement(elementId, std::move(rgbFrame));
}

void CalibrationDebugElements::updateCameraCalibratorCorrespondences(const ElementId elementId, const Frame& yFrame, const MetricCalibrationBoard& calibrationBoard, const AnyCamera& camera, const HomogenousMatrix4& board_T_camera, const CalibrationBoard::ObjectPointIds& objectPointIds, const Vectors3& objectPoints, const Vectors2& imagePoints, const std::string& text)
{
	if (!isElementActive(elementId))
	{
		return;
	}

	Frame rgbFrame;
	if (!CV::FrameConverter::Comfort::convert(yFrame, FrameType::FORMAT_RGB24, rgbFrame, CV::FrameConverter::CP_ALWAYS_COPY))
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	for (unsigned int y = 0u; y < rgbFrame.height(); ++y)
	{
		uint8_t* row = rgbFrame.row<uint8_t>(y);

		for (unsigned int x = 0u; x < rgbFrame.width() * 3u; ++x)
		{
			row[x] /= 2u;
		}
	}

	Utilities::paintCalibrationBoardOutline(rgbFrame, camera, board_T_camera, calibrationBoard, CV::Canvas::blue());

	const HomogenousMatrix4 flippedCamera_T_board = Camera::standard2InvertedFlipped(board_T_camera);

	ocean_assert(objectPointIds.size() == objectPoints.size());
	ocean_assert(objectPoints.size() == imagePoints.size());

	for (size_t n = 0; n < objectPoints.size(); ++n)
	{
		const CalibrationBoard::ObjectPointId& objectPointId = objectPointIds[n];
		const Vector3& objectPoint = objectPoints[n];
		const Vector2& imagePoint = imagePoints[n];

		const Vector2 projectedBoardPoint = camera.projectToImageIF(flippedCamera_T_board, objectPoint);

		const CalibrationBoard::BoardMarker& marker = calibrationBoard.marker(objectPointId.markerCoordinate());

		constexpr bool tOriented = true; // the object points are already oriented
		const bool sign = marker.pointSign<tOriented>(objectPointId.indexInMarker());

		const uint8_t* color = sign ? CV::Canvas::green() : CV::Canvas::blue();

		CV::Canvas::line<3u>(rgbFrame, projectedBoardPoint, imagePoint, color);
	}

	const double percent = NumericD::ratio(double(imagePoints.size()), double(calibrationBoard.numberPoints()), 0.0);

	int yText = 5;

	if (!text.empty())
	{
		CV::Canvas::drawText(rgbFrame, text, 5, yText, CV::Canvas::white(), CV::Canvas::black());

		yText += 20;
	}

	CV::Canvas::drawText(rgbFrame, "Points: " + String::toAString(percent* 100.0, 1u) + "%", 5, yText, CV::Canvas::white(), CV::Canvas::black());

	updateElement(elementId, std::move(rgbFrame));
}

void CalibrationDebugElements::updateCameraCalibratorCameraBoundary(const AnyCameraClipper& cameraClipper)
{
	if (!isElementActive(EI_CAMERA_CALIBRATOR_CAMERA_BOUNDARY))
	{
		return;
	}

	Frame frame = Utilities::visualizeDistortionValidity(cameraClipper);

	updateElement(EI_CAMERA_CALIBRATOR_CAMERA_BOUNDARY, std::move(frame));
}

void CalibrationDebugElements::updateCameraCalibratorCoverage(const CalibrationBoardObservation* observations, const size_t numberObservations, const unsigned int expectedCoverage, const bool showNumbers)
{
	if (!isElementActive(EI_CAMERA_CALIBRATOR_COVERAGE))
	{
		return;
	}

	ocean_assert(observations != nullptr);
	ocean_assert(numberObservations >= 1);
	ocean_assert(expectedCoverage >= 1u);

	Indices32 coveredBins;

	unsigned int width = 0u;
	unsigned int height = 0u;

	for (size_t nObservation = 0; nObservation < numberObservations; ++nObservation)
	{
		const CalibrationBoardObservation& observation = observations[nObservation];

		const Geometry::SpatialDistribution::OccupancyArray& occupancyArray = observation.occupancyArray();

		if (coveredBins.empty())
		{
			coveredBins.resize(occupancyArray.bins(), 0u);

			width = observation.camera()->width();
			height = observation.camera()->height();
		}

		ocean_assert(width == observation.camera()->width());
		ocean_assert(height == observation.camera()->height());

		for (unsigned int nBin = 0u; nBin < occupancyArray.bins(); ++nBin)
		{
			if (occupancyArray[nBin])
			{
				++coveredBins[nBin];
			}
		}
	}

	if (width == 0u || height == 0u)
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	Frame rgbFrame(FrameType(width, height, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));

	const uint8_t* colorRed = CV::Canvas::red();
	const uint8_t* colorGreen = CV::Canvas::green();

	rgbFrame.setValue<uint8_t>(colorRed, 3u);

	const Geometry::SpatialDistribution::OccupancyArray& occupancyArray = observations[0].occupancyArray();

	for (unsigned int y = 0u; y < rgbFrame.height(); ++y)
	{
		const unsigned int yBin = occupancyArray.verticalBin(Scalar(y));

		for (unsigned int x = 0u; x < rgbFrame.width(); ++x)
		{
			const unsigned int xBin = occupancyArray.horizontalBin(Scalar(x));

			const unsigned int index = yBin * occupancyArray.horizontalBins() + xBin;
			ocean_assert(index < coveredBins.size());

			const unsigned int coverage = coveredBins[index];

			if (coverage == 0u)
			{
				continue;
			}

			uint8_t* const pixel = rgbFrame.pixel<uint8_t>(x, y);

			if (coverage >= expectedCoverage)
			{
				pixel[0] = colorGreen[0];
				pixel[1] = colorGreen[1];
				pixel[2] = colorGreen[2];

				continue;
			}

			const float factor = float(coverage) / float(expectedCoverage);

			for (unsigned int nChannel = 0u; nChannel < 3u; ++nChannel)
			{
				const float color = colorRed[nChannel] * (1.0f - factor) + colorGreen[nChannel] * factor;
				ocean_assert(color >= 0.0f && color <= 255.5f);

				pixel[nChannel] = uint8_t(color);
			}
		}
	}

	if (showNumbers)
	{
		for (unsigned int y = 0u; y < rgbFrame.height(); ++y)
		{
			const unsigned int yBin = occupancyArray.verticalBin(Scalar(y));

			for (unsigned int x = 0u; x < rgbFrame.width(); ++x)
			{
				const unsigned int xBin = occupancyArray.horizontalBin(Scalar(x));

				const unsigned int index = yBin * occupancyArray.horizontalBins() + xBin;
				ocean_assert(index < coveredBins.size());

				unsigned int& coverage = coveredBins[index];

				if (coverage == Index32(-1))
				{
					continue;
				}

				CV::Canvas::drawText(rgbFrame, String::toAString(coverage), x + 5, y + 5, CV::Canvas::black());

				coverage = Index32(-1);
			}
		}
	}

	updateElement(EI_CAMERA_CALIBRATOR_COVERAGE, std::move(rgbFrame));
}

void CalibrationDebugElements::updateCameraCalibratorProjectionError(const CalibrationBoardObservation* observations, const size_t numberObservations, const bool showNumbers)
{
	if (!isElementActive(EI_CAMERA_CALIBRATOR_PROJECTION_ERROR))
	{
		return;
	}

	ocean_assert(observations != nullptr);
	ocean_assert(numberObservations >= 1);

	Scalars sumProjectionErrorsPerBin;
	Indices32 observationsPerBin;

	unsigned int width = 0u;
	unsigned int height = 0u;

	unsigned int horizontalBins = 0u;
	unsigned int verticalBins = 0u;

	for (size_t nObservation = 0; nObservation < numberObservations; ++nObservation)
	{
		const CalibrationBoardObservation& observation = observations[nObservation];

		const Geometry::SpatialDistribution::OccupancyArray& occupancyArray = observation.occupancyArray();

		if (sumProjectionErrorsPerBin.empty())
		{
			sumProjectionErrorsPerBin.resize(occupancyArray.bins(), Scalar(0));
			observationsPerBin.resize(occupancyArray.bins(), 0u);

			horizontalBins = occupancyArray.horizontalBins();
			verticalBins = occupancyArray.verticalBins();

			width = observation.camera()->width();
			height = observation.camera()->height();
		}

		ocean_assert(width == observation.camera()->width());
		ocean_assert(height == observation.camera()->height());

		ocean_assert(horizontalBins == occupancyArray.horizontalBins());
		ocean_assert(verticalBins == occupancyArray.verticalBins());

		ocean_assert(observation.imagePoints().size() == observation.objectPoints().size());

		const HomogenousMatrix4 flippedCamera_T_board = Camera::standard2InvertedFlipped(observation.board_T_camera());

		for (size_t nCorrespondence = 0; nCorrespondence < observation.objectPoints().size(); ++nCorrespondence)
		{
			const Vector3& objectPoint = observation.objectPoints()[nCorrespondence];
			const Vector2& imagePoint = observation.imagePoints()[nCorrespondence];

			const Vector2 projectedObjectPoint = observation.camera()->projectToImageIF(flippedCamera_T_board, objectPoint);

			const Scalar projectionError = projectedObjectPoint.distance(imagePoint);

			const unsigned int binIndex = occupancyArray.index(imagePoint.x(), imagePoint.y());
			ocean_assert(binIndex < sumProjectionErrorsPerBin.size());

			sumProjectionErrorsPerBin[binIndex] += projectionError;
			++observationsPerBin[binIndex];
		}
	}

	if (width == 0u || height == 0u)
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	Scalar maxError = Numeric::minValue();

	for (size_t nBin = 0; nBin < sumProjectionErrorsPerBin.size(); ++nBin)
	{
		Scalar& error = sumProjectionErrorsPerBin[nBin];
		unsigned int& number = observationsPerBin[nBin];

		if (number == 0u)
		{
			error = Scalar(-1);
		}
		else
		{
			error /= Scalar(number);

			maxError = std::max(maxError, error);
		}
	}

	if (maxError == Numeric::minValue())
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	Frame rgbFrame(FrameType(horizontalBins, verticalBins, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));

	if (maxError < Numeric::eps())
	{
		rgbFrame.setValue<uint8_t>(CV::Canvas::green(), 3u);
	}
	else
	{
		const uint8_t* colorRed = CV::Canvas::red();
		const uint8_t* colorGreen = CV::Canvas::green();

		rgbFrame.setValue(0x00u);

		for (unsigned int y = 0u; y < verticalBins; ++y)
		{
			uint8_t* row = rgbFrame.row<uint8_t>(y);

			const Scalar* error = sumProjectionErrorsPerBin.data() + y * horizontalBins;

			for (unsigned int x = 0u; x < horizontalBins; ++x)
			{
				const float factor = error[x] < Scalar(0) ? 1.0f : float(error[x] / maxError);

				for (unsigned int nChannel = 0u; nChannel < 3u; ++nChannel)
				{
					const float color = colorGreen[nChannel] * (1.0f - factor) + colorRed[nChannel] * factor;
					ocean_assert(color >= 0.0f && color <= 255.5f);

					row[nChannel] = uint8_t(color);
				}

				row += 3;
			}
		}
	}

	CV::FrameInterpolatorNearestPixel::Comfort::resize(rgbFrame, width, height);

	if (showNumbers)
	{
		for (unsigned int yBin = 0u; yBin < verticalBins; ++yBin)
		{
			const unsigned int y = yBin * rgbFrame.height() / verticalBins;

			const Scalar* error = sumProjectionErrorsPerBin.data() + yBin * horizontalBins;

			for (unsigned int xBin = 0u; xBin < horizontalBins; ++xBin)
			{
				const unsigned int x = xBin * rgbFrame.width() / horizontalBins;

				CV::Canvas::drawText(rgbFrame, String::toAString(error[xBin], 1u), x + 5, y + 5, CV::Canvas::black());
			}
		}
	}

	updateElement(EI_CAMERA_CALIBRATOR_PROJECTION_ERROR, std::move(rgbFrame));
}

void CalibrationDebugElements::updateDistortionElement(const ElementId elementId, const AnyCamera& camera, const bool useGrid)
{
	if (!isElementActive(elementId))
	{
		return;
	}

	Frame frame;

	if (useGrid)
	{
		constexpr unsigned int pixelsPerBin = 20u;

		const unsigned int horizontalBins = std::max(1u, (camera.width() + pixelsPerBin / 2u) / pixelsPerBin);
		const unsigned int verticalBins = std::max(1u, (camera.height() + pixelsPerBin / 2u) / pixelsPerBin);

		frame = Utilities::visualizeDistortionGrid(camera, horizontalBins, verticalBins, true /*undistort*/);
	}
	else
	{
		constexpr unsigned int pixelsPerBin = 40u;

		const unsigned int horizontalBins = std::max(1u, (camera.width() + pixelsPerBin / 2u) / pixelsPerBin);
		const unsigned int verticalBins = std::max(1u, (camera.height() + pixelsPerBin / 2u) / pixelsPerBin);

		frame = Utilities::visualizeDistortionVectors(camera, horizontalBins, verticalBins);
	}

	updateElement(elementId, std::move(frame));
}

void CalibrationDebugElements::updatePointsElement(const ElementId elementId, const Frame& yFrame, const Points& points)
{
	if (!isElementActive(elementId))
	{
		return;
	}

	Frame rgbFrame;
	if (!CV::FrameConverter::Comfort::convert(yFrame, FrameType::FORMAT_RGB24, rgbFrame, CV::FrameConverter::CP_ALWAYS_COPY))
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	for (const Point& point : points)
	{
		const uint8_t* color = point.sign() ? CV::Canvas::green() : CV::Canvas::blue();

		CV::Canvas::point<5u>(rgbFrame, point.observation(), color);
	}

	updateElement(elementId, std::move(rgbFrame));
}

}

}

}
