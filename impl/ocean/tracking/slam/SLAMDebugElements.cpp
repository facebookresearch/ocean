/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/slam/SLAMDebugElements.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameTransposer.h"

#include "ocean/tracking/Utilities.h"

namespace Ocean
{

namespace Tracking
{

namespace SLAM
{

void SLAMDebugElements::updateOccupancyArray(const Frame& yFrame, const OccupancyArray& occupancyArray)
{
	if (!isElementActive(EI_OCCUPANCY_ARRAY))
	{
		return;
	}

	ocean_assert(yFrame.isValid() && occupancyArray.isValid());

	Frame debugFrame;
	if (!CV::FrameConverter::Comfort::convert(yFrame, FrameType::FORMAT_RGB24, debugFrame, true))
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	for (unsigned int y = 0u; y < debugFrame.height(); ++y)
	{
		const unsigned int yBin = occupancyArray.verticalBin(Scalar(y));

		uint8_t* row = debugFrame.row<uint8_t>(y);

		for (unsigned int x = 0u; x < debugFrame.width(); ++x)
		{
			const unsigned int xBin = occupancyArray.horizontalBin(Scalar(x));

			if (occupancyArray.isEmpty(xBin, yBin))
			{
				row[0] = row[0] * 2u / 3u;
				row[1] = row[1] * 2u / 3u;
				row[2] = row[2] * 2u / 3u;
			}
			else
			{
				row[1] = 0xFF;
			}

			row += 3;
		}
	}

	if (outputRotationAngle_ != 0)
	{
		CV::FrameTransposer::Comfort::rotate(debugFrame, outputRotationAngle_);
	}

	const float occupancy = occupancyArray.coverage();

	CV::Canvas::drawText(debugFrame, "Empty bins: " + String::toAString(occupancy * 100.0, 1u) + "%", 5, 5, CV::Canvas::white(), CV::Canvas::black());
	CV::Canvas::drawText(debugFrame, std::string("Need more: ") + (occupancyArray.needMorePoints() ? "true" : "false"), 5, 25, CV::Canvas::white(), CV::Canvas::black());

	updateElement(EI_OCCUPANCY_ARRAY, std::move(debugFrame));
}

void SLAMDebugElements::updateTrackedImagePoints(const Frame& yFrame, const Vector2* previousImagePoints, const Vector2* currentImagePoints, uint8_t* validCorrespondences, const size_t numberCorrespondences)
{
	if (!isElementActive(EI_TRACKED_IMAGE_POINTS))
	{
		return;
	}

	ocean_assert(yFrame.isValid());

	Frame debugFrame;
	if (!CV::FrameConverter::Comfort::convert(yFrame, FrameType::FORMAT_RGB24, debugFrame, true))
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	if (numberCorrespondences != 0)
	{
		ocean_assert(previousImagePoints != nullptr && currentImagePoints != nullptr && validCorrespondences != nullptr);

		for (size_t n = 0; n < numberCorrespondences; ++n)
		{
			const Vector2& previousImagePoint = previousImagePoints[n];
			const Vector2& currentImagePoint = currentImagePoints[n];
			const bool isValid = validCorrespondences[n] == uint8_t(1);

			CV::Canvas::line<3u>(debugFrame, previousImagePoint, currentImagePoint, isValid ? CV::Canvas::green() : CV::Canvas::red());
		}
	}

	if (outputRotationAngle_ != 0)
	{
		CV::FrameTransposer::Comfort::rotate(debugFrame, outputRotationAngle_);
	}

	updateElement(EI_TRACKED_IMAGE_POINTS, std::move(debugFrame));
}

void SLAMDebugElements::updateObjectPoints(const Frame& yFrame, const AnyCamera& camera, const CameraPose& cameraPose, const Index32 frameIndex, const LocalizedObjectPointMap& localizedObjectPointMap, const bool showAll)
{
	if (!isElementActive(EI_OBJECT_POINTS))
	{
		return;
	}

	ocean_assert(yFrame.isValid());

	Frame debugFrame;
	if (!CV::FrameConverter::Comfort::convert(yFrame, FrameType::FORMAT_RGB24, debugFrame, true))
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	AnyCameraClipper cameraClipper;

	if (showAll)
	{
		cameraClipper.update(camera.clone());
	}

	size_t numberTrackedObjectPoints = 0;
	size_t numberVisibleObjectPoints = 0;

	for (const LocalizedObjectPointMap::value_type& objectPointPair : localizedObjectPointMap)
	{
		const LocalizedObjectPoint& localizedObjectPoint = objectPointPair.second;
		ocean_assert(localizedObjectPoint.position() != Vector3::minValue());

		if (localizedObjectPoint.hasObservation(frameIndex))
		{
			ocean_assert(Camera::isObjectPointInFrontIF(cameraPose.flippedCamera_T_world(), localizedObjectPoint.position()));

			const Vector2 projectedObjectPoint = camera.projectToImageIF(cameraPose.flippedCamera_T_world(), localizedObjectPoint.position());

			CV::Canvas::point<7u>(debugFrame, projectedObjectPoint, CV::Canvas::blue());

			++numberTrackedObjectPoints;
		}
		else if (cameraClipper.isValid())
		{
			Vector2 projectedObjectPoint;
			if (cameraClipper.projectToImageIF(cameraPose.flippedCamera_T_world(), localizedObjectPoint.position(), &projectedObjectPoint))
			{
				CV::Canvas::point<7u>(debugFrame, projectedObjectPoint, CV::Canvas::red());

				++numberVisibleObjectPoints;
			}
		}
	}

	if (outputRotationAngle_ != 0)
	{
		CV::FrameTransposer::Comfort::rotate(debugFrame, outputRotationAngle_);
	}

	CV::Canvas::drawText(debugFrame, "Tracked: " + String::toAString(numberTrackedObjectPoints), 5, 5, CV::Canvas::white(), CV::Canvas::black());

	if (showAll)
	{
		CV::Canvas::drawText(debugFrame, "Visible: " + String::toAString(numberVisibleObjectPoints), 5, 25, CV::Canvas::white(), CV::Canvas::black());
	}

	updateElement(EI_OBJECT_POINTS, std::move(debugFrame));
}

void SLAMDebugElements::updateImagePoints(const Frame& yFrame, const Index32 frameIndex, const PointTrackMap& pointTrackMap)
{
	if (!isElementActive(EI_IMAGE_POINTS))
	{
		return;
	}

	ocean_assert(yFrame.isValid());

	Frame debugFrame;
	if (!CV::FrameConverter::Comfort::convert(yFrame, FrameType::FORMAT_RGB24, debugFrame, true))
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	size_t numberNewImagePoints = 0;
	size_t numberTrackedImagePoints = 0;

	for (const PointTrackMap::value_type& pointPair : pointTrackMap)
	{
		const PointTrack& pointTrack = pointPair.second;
		ocean_assert(pointTrack.isValid());

		ocean_assert_and_suppress_unused(pointTrack.lastFrameIndex() == frameIndex, frameIndex);

		const Vectors2& imagePoints = pointTrack.imagePoints();

		if (imagePoints.size() == 1)
		{
			CV::Canvas::point<7u>(debugFrame, imagePoints.back(), CV::Canvas::yellow());

			++numberNewImagePoints;
		}
		else
		{
			Vector2 previousImagePoint = imagePoints.back();

			for (size_t nObservation = imagePoints.size() - 2; nObservation < imagePoints.size(); --nObservation)
			{
				const Vector2& currentImagePoint = imagePoints[nObservation];

				CV::Canvas::line<1u>(debugFrame, previousImagePoint, currentImagePoint, CV::Canvas::green());

				previousImagePoint = currentImagePoint;
			}

			++numberTrackedImagePoints;
		}
	}

	if (outputRotationAngle_ != 0)
	{
		CV::FrameTransposer::Comfort::rotate(debugFrame, outputRotationAngle_);
	}

	CV::Canvas::drawText(debugFrame, "Tracked points: " + String::toAString(numberTrackedImagePoints), 5, 5, CV::Canvas::white(), CV::Canvas::black());
	CV::Canvas::drawText(debugFrame, "New points: " + String::toAString(numberNewImagePoints), 5, 25, CV::Canvas::white(), CV::Canvas::black());

	updateElement(EI_IMAGE_POINTS, std::move(debugFrame));
}

void SLAMDebugElements::updateResult(const Frame& yFrame, const AnyCamera& camera, const CameraPose& cameraPose, const Index32 frameIndex, const PointTrackMap& pointTrackMap, const LocalizedObjectPointMap& localizedObjectPointMap, const Triangle3* objectTriangles, const size_t numberObjectTriangles)
{
	if (!isElementActive(EI_RESULT))
	{
		return;
	}

	ocean_assert(yFrame.isValid());

	Frame debugFrame;
	if (!CV::FrameConverter::Comfort::convert(yFrame, FrameType::FORMAT_RGB24, debugFrame, true))
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	size_t numberNewImagePoints = 0;
	size_t numberPointCorrespondences2D = 0;

	for (const PointTrackMap::value_type& pointPair : pointTrackMap)
	{
		const PointTrack& pointTrack = pointPair.second;
		ocean_assert(pointTrack.isValid());

		const Vectors2& imagePoints = pointTrack.imagePoints();

		if (imagePoints.size() == 1)
		{
			CV::Canvas::point<7u>(debugFrame, imagePoints.back(), CV::Canvas::yellow());

			++numberNewImagePoints;
		}
		else
		{
			Vector2 previousImagePoint = imagePoints.back();

			for (size_t nObservation = imagePoints.size() - 2; nObservation < imagePoints.size(); --nObservation)
			{
				const Vector2& currentImagePoint = imagePoints[nObservation];

				CV::Canvas::line<1u>(debugFrame, previousImagePoint, currentImagePoint, CV::Canvas::green());

				previousImagePoint = currentImagePoint;
			}

			++numberPointCorrespondences2D;
		}
	}

	size_t numberPointCorrespondences3D = 0;

	size_t numberLocalizedObjectPointsHighPrecision = 0;
	size_t numberLocalizedObjectPointsMediumPrecision = 0;

	if (cameraPose.isValid())
	{
		const HomogenousMatrix4& world_T_camera = cameraPose.world_T_camera();
		const HomogenousMatrix4& flippedCamera_T_world = cameraPose.flippedCamera_T_world();

		if (numberObjectTriangles != 0)
		{
			ocean_assert(objectTriangles != nullptr);

			const AnyCameraClipper cameraClipper(camera.clone());

			for (size_t nTriangle = 0; nTriangle < numberObjectTriangles; ++nTriangle)
			{
				const Triangle3& objectTriangle = objectTriangles[nTriangle];

				Tracking::Utilities::paintTriangleIF<1u>(debugFrame, flippedCamera_T_world, cameraClipper, objectTriangle, 10 /*segments*/, CV::Canvas::white());
			}
		}

		Vectors3 objectPoints;
		Vectors2 imagePoints;

		for (const LocalizedObjectPointMap::value_type& objectPointPair : localizedObjectPointMap)
		{
			const LocalizedObjectPoint& localizedObjectPoint = objectPointPair.second;
			ocean_assert(localizedObjectPoint.position() != Vector3::minValue());

			const Observation lastObservation = localizedObjectPoint.lastObservation();

			if (lastObservation.frameIndex() == frameIndex)
			{
				objectPoints.push_back(localizedObjectPoint.position());
				imagePoints.push_back(lastObservation.imagePoint());

				++numberPointCorrespondences3D;

				if (localizedObjectPoint.localizationPrecision() == LocalizedObjectPoint::LP_HIGH)
				{
					++numberLocalizedObjectPointsHighPrecision;
				}
				else if (localizedObjectPoint.localizationPrecision() == LocalizedObjectPoint::LP_MEDIUM)
				{
					++numberLocalizedObjectPointsMediumPrecision;
				}
			}
		}

		if (!objectPoints.empty())
		{
			Tracking::Utilities::paintCorrespondences<11u, 3u>(debugFrame, camera, world_T_camera, objectPoints.data(), imagePoints.data(), objectPoints.size(), Scalar(3.5 * 3.5), CV::Canvas::blue(), CV::Canvas::green(), CV::Canvas::red(), CV::Canvas::gray());
		}
	}

	if (outputRotationAngle_ != 0)
	{
		CV::FrameTransposer::Comfort::rotate(debugFrame, outputRotationAngle_);
	}

	CV::Canvas::drawText(debugFrame, "New 2D points: " + String::toAString(numberNewImagePoints), 5, 5, CV::Canvas::white(), CV::Canvas::black());
	CV::Canvas::drawText(debugFrame, "Unlocalized 3D points: " + String::toAString(pointTrackMap.size()), 5, 30, CV::Canvas::white(), CV::Canvas::black());
	CV::Canvas::drawText(debugFrame, "Localized 3D points: " + String::toAString(localizedObjectPointMap.size()), 5, 50, CV::Canvas::white(), CV::Canvas::black());
	CV::Canvas::drawText(debugFrame, "Active 2D/2D correspondences: " + String::toAString(numberPointCorrespondences2D), 5, 75, CV::Canvas::white(), CV::Canvas::black());
	CV::Canvas::drawText(debugFrame, "Active 2D/3D correspondences: " + String::toAString(numberPointCorrespondences3D) + ", " + String::toAString(numberLocalizedObjectPointsHighPrecision) + " high, " + String::toAString(numberLocalizedObjectPointsMediumPrecision) + " medium", 5, 95, CV::Canvas::white(), CV::Canvas::black());

	updateElement(EI_RESULT, std::move(debugFrame));
}

}

}

}
