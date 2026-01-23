/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/slam/Utilities.h"

#include "ocean/base/String.h"

#include "ocean/cv/Canvas.h"

#include "ocean/io/JSONParser.h"

#include <fstream>

namespace Ocean
{

namespace Tracking
{

namespace SLAM
{

bool Utilities::paintDebugDataByLocalizationPrecision(Frame& frame, const AnyCameraClipper& cameraClipper, const HomogenousMatrix4& world_T_camera, const TrackerMono::DebugData& debugData, const size_t maximalTrackLength, const unsigned int darkeningFactor)
{
	ocean_assert(frame.isValid());
	ocean_assert(frame.isPixelFormatDataLayoutCompatible(FrameType::FORMAT_RGB24));

	if (!frame.isPixelFormatDataLayoutCompatible(FrameType::FORMAT_RGB24))
	{
		return false;
	}

	if (darkeningFactor >= 1u)
	{
		for (unsigned int y = 0u; y < frame.height(); ++y)
		{
			uint8_t* row = frame.row<uint8_t>(y);

			for (unsigned int x = 0u; x < frame.planeWidthElements(0u); ++x)
			{
				row[x] = uint8_t(row[x] / darkeningFactor);
			}
		}
	}

	if (world_T_camera.isValid())
	{
		const HomogenousMatrix4 flippedCamera_T_world = Camera::standard2InvertedFlipped(world_T_camera);

		for (const TrackerMono::DebugData::PointMap::value_type& pair : debugData.pointMap_)
		{
			const TrackerMono::DebugData::Point& point = pair.second;

			const Vector3& position = point.position_;
			const LocalizedObjectPoint::LocalizationPrecision localizationPrecision = point.precision_;

			Vector2 projectedObjectPoint;
			if (cameraClipper.projectToImageIF(flippedCamera_T_world, position, &projectedObjectPoint))
			{
				const uint8_t* color = nullptr;

				switch (localizationPrecision)
				{
					case Tracking::SLAM::LocalizedObjectPoint::LP_INVALID:
						ocean_assert(false && "This should never happen!");
						color = CV::Canvas::black(frame.pixelFormat());
						break;

					case Tracking::SLAM::LocalizedObjectPoint::LP_UNKNOWN:
						color = CV::Canvas::gray(frame.pixelFormat());
						break;

					case Tracking::SLAM::LocalizedObjectPoint::LP_LOW:
						color = CV::Canvas::red(frame.pixelFormat());
						break;

					case Tracking::SLAM::LocalizedObjectPoint::LP_MEDIUM:
						color = CV::Canvas::blue(frame.pixelFormat());
						break;

					case Tracking::SLAM::LocalizedObjectPoint::LP_HIGH:
						color = CV::Canvas::green(frame.pixelFormat());
						break;
				}

				CV::Canvas::point<5u>(frame, projectedObjectPoint, color);
			}
		}
	}

	if (maximalTrackLength >= 1)
	{
		constexpr size_t stableTrackLength = 100;

		constexpr float unstableColor[3] = {255.0f, 0.0f, 0.0f};
		constexpr float stableColor[3] = {0.0f, 255.0f, 0.0f};

		for (const TrackerMono::DebugData::TracksMap::value_type& tracksPair : debugData.tracksMap_)
		{
			const Vectors2& imagePoints = tracksPair.second.second;

			const float stabilityFactor = std::min(float(imagePoints.size()) / float(stableTrackLength), 1.0f);

			uint8_t color[3];

			for (unsigned int n = 0u; n < 3u; ++n)
			{
				color[n] = uint8_t(std::min(unstableColor[n] * (1.0f - stabilityFactor) + stableColor[n] * stabilityFactor, 255.0f));
			}

			const size_t startIndex = size_t(std::max(int64_t(1), int64_t(imagePoints.size()) - int64_t(maximalTrackLength)));

			for (size_t n = startIndex; n < imagePoints.size(); ++n)
			{
				CV::Canvas::line<1u>(frame, imagePoints[n - 1], imagePoints[n], color);
			}
		}
	}

	return true;
}

bool Utilities::paintDebugDataByPosePrecision(Frame& frame, const AnyCameraClipper& cameraClipper, const HomogenousMatrix4& world_T_camera, const TrackerMono::DebugData& debugData, const size_t maximalTrackLength, const unsigned int darkeningFactor)
{
	ocean_assert(frame.isValid());
	ocean_assert(frame.isPixelFormatDataLayoutCompatible(FrameType::FORMAT_RGB24));

	if (!frame.isPixelFormatDataLayoutCompatible(FrameType::FORMAT_RGB24))
	{
		return false;
	}

	if (darkeningFactor >= 1u)
	{
		for (unsigned int y = 0u; y < frame.height(); ++y)
		{
			uint8_t* row = frame.row<uint8_t>(y);

			for (unsigned int x = 0u; x < frame.planeWidthElements(0u); ++x)
			{
				row[x] = uint8_t(row[x] / darkeningFactor);
			}
		}
	}

	if (world_T_camera.isValid())
	{
		const UnorderedIndexSet32& posePreciseObjectPointIds = debugData.posePreciseObjectPointIds_;
		const UnorderedIndexSet32& poseNotPreciseObjectPointIds = debugData.poseNotPreciseObjectPointIds_;

#ifdef OCEAN_DEBUG
		{
			UnorderedIndexSet32 debugObjectPointIds(posePreciseObjectPointIds);
			debugObjectPointIds.insert(poseNotPreciseObjectPointIds.begin(), poseNotPreciseObjectPointIds.end());

			ocean_assert(debugObjectPointIds.size() == posePreciseObjectPointIds.size() + poseNotPreciseObjectPointIds.size());
		}
#endif

		const bool largePoints = frame.pixels() >= 640u * 480u;

		const HomogenousMatrix4 flippedCamera_T_world = Camera::standard2InvertedFlipped(world_T_camera);

		for (const TrackerMono::DebugData::PointMap::value_type& pair : debugData.pointMap_)
		{
			const Index32 objectPointId = pair.first;
			const TrackerMono::DebugData::Point& point = pair.second;

			const Vector3& position = point.position_;

			Vector2 projectedObjectPoint;
			if (cameraClipper.projectToImageIF(flippedCamera_T_world, position, &projectedObjectPoint))
			{
				const uint8_t* color = CV::Canvas::gray(frame.pixelFormat());

				if (posePreciseObjectPointIds.contains(objectPointId))
				{
					color = CV::Canvas::green(frame.pixelFormat());
				}
				else if (poseNotPreciseObjectPointIds.contains(objectPointId))
				{
					color = CV::Canvas::red(frame.pixelFormat());
				}
				else
				{
					continue;
				}

				if (point.isBundleAdjusted_)
				{
					if (largePoints)
					{
						CV::Canvas::point<7u>(frame, projectedObjectPoint, color);
					}
					else
					{
						CV::Canvas::point<5u>(frame, projectedObjectPoint, color);
					}
				}
				else
				{
					if (largePoints)
					{
						CV::Canvas::point<3u>(frame, projectedObjectPoint, color);
					}
					else
					{
						CV::Canvas::point<1u>(frame, projectedObjectPoint, color);
					}
				}
			}
		}
	}

	if (maximalTrackLength >= 1)
	{
		constexpr size_t stableTrackLength = 100;

		constexpr float unstableColor[3] = {255.0f, 0.0f, 0.0f};
		constexpr float stableColor[3] = {0.0f, 255.0f, 0.0f};

		for (const TrackerMono::DebugData::TracksMap::value_type& tracksPair : debugData.tracksMap_)
		{
			const Vectors2& imagePoints = tracksPair.second.second;

			const float stabilityFactor = std::min(float(imagePoints.size()) / float(stableTrackLength), 1.0f);

			uint8_t color[3];

			for (unsigned int n = 0u; n < 3u; ++n)
			{
				color[n] = uint8_t(std::min(unstableColor[n] * (1.0f - stabilityFactor) + stableColor[n] * stabilityFactor, 255.0f));
			}

			const size_t startIndex = size_t(std::max(int64_t(1), int64_t(imagePoints.size()) - int64_t(maximalTrackLength)));

			for (size_t n = startIndex; n < imagePoints.size(); ++n)
			{
				CV::Canvas::line<1u>(frame, imagePoints[n - 1], imagePoints[n], color);
			}
		}
	}

	return true;
}

bool Utilities::paintObjectPoints(Frame& frame, const AnyCameraClipper& cameraClipper, const HomogenousMatrix4& world_T_camera, const TrackerMono::DebugData& debugData, const unsigned int darkeningFactor)
{
	ocean_assert(frame.isValid());
	ocean_assert(frame.isPixelFormatDataLayoutCompatible(FrameType::FORMAT_RGB24));

	if (!frame.isPixelFormatDataLayoutCompatible(FrameType::FORMAT_RGB24))
	{
		return false;
	}

	if (darkeningFactor >= 1u)
	{
		for (unsigned int y = 0u; y < frame.height(); ++y)
		{
			uint8_t* row = frame.row<uint8_t>(y);

			for (unsigned int x = 0u; x < frame.planeWidthElements(0u); ++x)
			{
				row[x] = uint8_t(row[x] / darkeningFactor);
			}
		}
	}

	if (world_T_camera.isValid())
	{
		const HomogenousMatrix4 flippedCamera_T_world = Camera::standard2InvertedFlipped(world_T_camera);

		for (const TrackerMono::DebugData::PointMap::value_type& pair : debugData.pointMap_)
		{
			const TrackerMono::DebugData::Point& point = pair.second;

			const Vector3& position = point.position_;

			Vector2 projectedObjectPoint;
			if (cameraClipper.projectToImageIF(flippedCamera_T_world, position, &projectedObjectPoint))
			{
				const uint8_t* color = CV::Canvas::green(frame.pixelFormat());

				CV::Canvas::point<5u>(frame, projectedObjectPoint, color);
			}
		}
	}

	return true;
}

bool Utilities::serializeFramesStatisticsToJSON(const TrackerMono::FramesStatistics& framesStatistics, const std::string& filename)
{
	if (filename.empty())
	{
		return false;
	}

	std::ofstream file(filename, std::ios::binary);

	if (!file.is_open())
	{
		return false;
	}

	file << "{\n";
	file << "  \"version\": 1,\n";
	file << "  \"framesStatistics\": [\n";

	for (size_t frameIndex = 0; frameIndex < framesStatistics.size(); ++frameIndex)
	{
		const TrackerMono::FrameStatistics& frameStatistics = framesStatistics[frameIndex];

		file << "    {\n";
		file << "      \"frameIndex\": " << frameStatistics.frameIndex_ << ",\n";
		file << "      \"frameToFrameTrackingPossible\": " << frameStatistics.frameToFrameTrackingPossible_ << ",\n";
		file << "      \"frameToFrameTrackingActual\": " << frameStatistics.frameToFrameTrackingActual_ << ",\n";
		file << "      \"poseEstimationCorrespondences\": " << frameStatistics.poseEstimationCorrespondences_ << ",\n";
		file << "      \"projectionError\": " << String::toAString(frameStatistics.projectionError_, 10u) << ",\n";
		file << "      \"mapVersion\": " << frameStatistics.mapVersion_ << ",\n";

		file << "      \"world_T_camera\": {\n";
		file << "        \"isValid\": " << (frameStatistics.world_T_camera_.isValid() ? "true" : "false") << ",\n";
		file << "        \"values\": [";

		const Scalar* matrixData = frameStatistics.world_T_camera_.data();

		for (unsigned int i = 0u; i < 16u; ++i)
		{
			file << String::toAString(matrixData[i], 10u);

			if (i < 15u)
			{
				file << ", ";
			}
		}

		file << "]\n";
		file << "      }\n";
		file << "    }";

		if (frameIndex + 1 < framesStatistics.size())
		{
			file << ",";
		}

		file << "\n";
	}

	file << "  ]\n";
	file << "}\n";

	return file.good();
}

bool Utilities::deserializeFramesStatisticsFromJSON(const std::string& filename, TrackerMono::FramesStatistics& framesStatistics)
{
	if (filename.empty())
	{
		return false;
	}

	framesStatistics.clear();

	std::string errorMessage;
	const IO::JSONParser::JSONValue root = IO::JSONParser::parse(filename, std::string(), false, &errorMessage);

	if (!root.isValid() || !root.isObject())
	{
		return false;
	}

	int version = 0;

	const double* versionValue = root.numberFromObject("version");

	if (versionValue != nullptr)
	{
		version = NumericD::round32(*versionValue);
	}

	if (version != 1)
	{
		return false;
	}

	const IO::JSONParser::JSONValue::Array* framesArray = root.arrayFromObject("framesStatistics");

	if (framesArray == nullptr)
	{
		return false;
	}

	framesStatistics.reserve(framesArray->size());

	for (const IO::JSONParser::JSONValue& frameValue : *framesArray)
	{
		if (!frameValue.isObject())
		{
			return false;
		}

		const double* frameIndex = frameValue.numberFromObject("frameIndex");
		const double* frameToFrameTrackingPossible = frameValue.numberFromObject("frameToFrameTrackingPossible");
		const double* frameToFrameTrackingActual = frameValue.numberFromObject("frameToFrameTrackingActual");
		const double* poseEstimationCorrespondences = frameValue.numberFromObject("poseEstimationCorrespondences");
		const double* projectionError = frameValue.numberFromObject("projectionError");
		const double* mapVersion = frameValue.numberFromObject("mapVersion");

		if (frameIndex == nullptr || frameToFrameTrackingPossible == nullptr || frameToFrameTrackingActual == nullptr || poseEstimationCorrespondences == nullptr || projectionError == nullptr || mapVersion == nullptr)
		{
			return false;
		}

		TrackerMono::FrameStatistics frameStatistics(Index32(NumericD::round32(*frameIndex)));
		frameStatistics.frameToFrameTrackingPossible_ = size_t(*frameToFrameTrackingPossible);
		frameStatistics.frameToFrameTrackingActual_ = size_t(*frameToFrameTrackingActual);
		frameStatistics.poseEstimationCorrespondences_ = size_t(*poseEstimationCorrespondences);
		frameStatistics.projectionError_ = Scalar(*projectionError);
		frameStatistics.mapVersion_ = std::max<int>(0, NumericD::round32(*mapVersion));

		const IO::JSONParser::JSONValue::ObjectMap* matrixObject = frameValue.objectFromObject("world_T_camera");

		if (matrixObject == nullptr)
		{
			return false;
		}

		const IO::JSONParser::JSONValue matrixValueRoot{IO::JSONParser::JSONValue::ObjectMap(*matrixObject)};

		const bool* isValid = matrixValueRoot.booleanFromObject("isValid");
		const IO::JSONParser::JSONValue::Array* valuesArray = matrixValueRoot.arrayFromObject("values");

		if (isValid == nullptr || valuesArray == nullptr || valuesArray->size() != 16u)
		{
			return false;
		}

		Scalar matrixValues[16];

		for (size_t i = 0; i < 16; ++i)
		{
			if (!(*valuesArray)[i].isNumber())
			{
				return false;
			}

			matrixValues[i] = Scalar((*valuesArray)[i].number());
		}

		if (*isValid)
		{
			frameStatistics.world_T_camera_ = HomogenousMatrix4(matrixValues);
		}
		else
		{
			frameStatistics.world_T_camera_ = HomogenousMatrix4(false);
		}

		framesStatistics.push_back(std::move(frameStatistics));
	}

	return true;
}

}

}

}
