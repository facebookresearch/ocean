// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/tracking/mapbuilding/importers/MDCImporter.h"
#include "ocean/tracking/mapbuilding/importers/Utilities.h"

#include <thrift/lib/cpp2/protocol/Serializer.h>

namespace Ocean
{

namespace Tracking
{

namespace MapBuilding
{

namespace Importers
{

bool MDCImporter::importGeometryFromFile(const std::string& filename, ObjectPointMap& objectPointMap, ImagePointMap& imagePointMap, CameraMap& cameraMap, ImageViewMap& imageViewMap, FrameDataMap& frameDataMap)
{
	arvr::thrift_if::mapping::geometry::MapChunk mapChunk;

	if (!readMapChunk(filename, mapChunk))
	{
		return false;
	}

	cameraMap.clear();

	for (const arvr::thrift_if::mapping::geometry::Camera& camera : *mapChunk.cameras())
	{
		uint32_t cameraId = 0u;
		HomogenousMatrixD4 device_T_camera(false);
		SharedAnyCameraD anyCamera = extractCamera(camera, &cameraId, &device_T_camera);

		if (anyCamera == nullptr)
		{
			return false;
		}

		if (!cameraMap.emplace(cameraId, CameraPair(std::move(anyCamera), device_T_camera)).second)
		{
			ocean_assert(false && "Camera id exists already");
			return false;
		}
	}

	const arvr::thrift_if::mapping::geometry::TrackContainer& trackContainer = *mapChunk.track_container();

	objectPointMap.clear();
	objectPointMap.reserve(trackContainer.get_point_tracks().size());

	for (const arvr::thrift_if::mapping::geometry::PointTrack& pointTrack : trackContainer.get_point_tracks())
	{
		const uint32_t pointTrackId = pointTrack.get_id();

		const arvr::thrift_if::mapping::geometry::PointTrackRepresentation* pointTrackRepresentation = pointTrack.get_point_representation();

		if (pointTrackRepresentation != nullptr)
		{
			if (pointTrackRepresentation->getType() == arvr::thrift_if::mapping::geometry::PointTrackRepresentation::Type::cartesian_point)
			{
				const arvr::thrift_if::mapping::geometry::CartesianPoint& cartesianPoint = pointTrackRepresentation->get_cartesian_point();

				const arvr::thrift_if::common::numerics::CompactVector3D* point = cartesianPoint.get_cartesian_point();

				if (point != nullptr)
				{
					if (!objectPointMap.emplace(pointTrackId, Utilities::toVector3<double, Scalar>(*point)).second)
					{
						ocean_assert(false && "Point track id exists already");
						return false;
					}
				}
			}
			else
			{
				ocean_assert(false && "Not supported");
				return false;
			}
		}
	}

	imagePointMap.clear();
	imagePointMap.reserve(objectPointMap.size() * 2);

	imageViewMap.clear();
	frameDataMap.clear();

	for (const arvr::thrift_if::mapping::geometry::Frame& frame : *mapChunk.frames())
	{
		const uint32_t frameId = frame.get_id();

		const arvr::thrift_if::common::numerics::CompactSE3D* transformFrameWorld = frame.get_transform_frame_world();

		FrameData frameData;

		if (frame.get_frame_observation().get_utc_timestamp_ns() != nullptr)
		{
			frameData.utcTimestamp_ = *frame.get_frame_observation().get_utc_timestamp_ns();
		}

		if (frame.get_frame_observation().get_capture_timestamp_us() != nullptr)
		{
			frameData.captureTimestamp_ = *frame.get_frame_observation().get_capture_timestamp_us();
		}

		if (transformFrameWorld != nullptr)
		{
			frameData.world_T_device_ = Utilities::toHomogenousMatrix4<double, double>(*transformFrameWorld).inverted();
		}

		for (const arvr::thrift_if::mapping::geometry::ImageView& imageView : frame.get_image_views())
		{
			const uint32_t imageViewId = imageView.get_id();
			const uint32_t cameraId = imageView.get_camera_id();

			const arvr::thrift_if::mapping::geometry::FeatureContainer& featureContainer = imageView.get_feature_container();

			Indices32 pointFeatureIds;
			pointFeatureIds.reserve(featureContainer.get_point_features().size());

			for (const arvr::thrift_if::mapping::geometry::PointFeature& pointFeature : featureContainer.get_point_features())
			{
				const uint32_t pointFeatureId = pointFeature.get_id();

				const arvr::thrift_if::mapping::geometry::PointFeatureObservation& pointFeatureObservation = pointFeature.get_point_feature_observation();

				const float xObservation = pointFeatureObservation[1];
				const float yObservation = pointFeatureObservation[2];

				const uint32_t pointTrackId = pointFeature.get_point_track_id();

				if (!imagePointMap.emplace(ObservationPair(imageViewId, pointFeatureId), ImagePointPair(Vector2(Scalar(xObservation), Scalar(yObservation)), pointTrackId)).second)
				{
					ocean_assert(false && "Image view/point feature exists already!");
					return false;
				}

				pointFeatureIds.emplace_back(pointFeatureId);
			}

			if (!imageViewMap.emplace(imageViewId, ImageViewPair(cameraId, std::move(pointFeatureIds))).second)
			{
				ocean_assert(false && "Image view id exists already!");
				return false;
			}

			ocean_assert(!hasElement(frameData.imageViewIds_, imageViewId));
			frameData.imageViewIds_.emplace_back(imageViewId);
		}

		if (!frameDataMap.emplace(frameId, std::move(frameData)).second)
		{
			ocean_assert(false && "Frame id exists already!");
			return false;
		}
	}

	return true;
}

bool MDCImporter::importGeometryFromFile(const std::string& filename, Vectors3& objectPoints)
{
	arvr::thrift_if::mapping::geometry::MapChunk mapChunk;

	if (!readMapChunk(filename, mapChunk))
	{
		return false;
	}

	const arvr::thrift_if::mapping::geometry::TrackContainer& trackContainer = *mapChunk.track_container();

	objectPoints.clear();
	objectPoints.reserve(trackContainer.get_point_tracks().size());

	for (const arvr::thrift_if::mapping::geometry::PointTrack& pointTrack : trackContainer.get_point_tracks())
	{
		const arvr::thrift_if::mapping::geometry::PointTrackRepresentation* pointTrackRepresentation = pointTrack.get_point_representation();

		if (pointTrackRepresentation != nullptr)
		{
			if (pointTrackRepresentation->getType() == arvr::thrift_if::mapping::geometry::PointTrackRepresentation::Type::cartesian_point)
			{
				const arvr::thrift_if::mapping::geometry::CartesianPoint& cartesianPoint = pointTrackRepresentation->get_cartesian_point();

				const arvr::thrift_if::common::numerics::CompactVector3D* point = cartesianPoint.get_cartesian_point();

				if (point != nullptr)
				{
					objectPoints.emplace_back(Utilities::toVector3<double, Scalar>(*point));
				}
			}
			else
			{
				ocean_assert(false && "Not supported");
				return false;
			}
		}
	}

	return true;
}

bool MDCImporter::readMapChunk(const std::string& filename, arvr::thrift_if::mapping::geometry::MapChunk& mapChunk)
{
	std::ifstream inputStream(filename, std::ios::binary);

	if (!inputStream.good())
	{
		return false;
	}

	std::ostringstream stringStream;
	stringStream << inputStream.rdbuf();

	if (!stringStream.good())
	{
		return false;
	}

	const std::string buffer = stringStream.str();

	if (buffer.size() == 0)
	{
		return false;
	}

	const size_t position = apache::thrift::CompactSerializer::deserialize(stringStream.str(), mapChunk);

	return position == buffer.size();
}

SharedAnyCameraD MDCImporter::extractCamera(const arvr::thrift_if::mapping::geometry::Camera& camera, uint32_t* id, HomogenousMatrixD4* device_T_camera)
{
	const std::string& modelName = camera.get_model_name();

	const arvr::thrift_if::mapping::geometry::CameraMetadata& cameraMetadata = camera.get_metadata();

	const uint32_t width = cameraMetadata.get_image_width();
	const uint32_t height = cameraMetadata.get_image_height();

	const uint32_t layoutCameraIndex = cameraMetadata.get_layout_camera_index();
	OCEAN_SUPPRESS_UNUSED_WARNING(layoutCameraIndex);

	ocean_assert(width >= 0u && height >= 0u);

	if (width > 0u && height > 0u)
	{
		if (id != nullptr)
		{
			*id = camera.get_id();
		}

		if (device_T_camera != nullptr)
		{
			const arvr::thrift_if::common::numerics::CompactSE3D* device_T_flippedCamera = cameraMetadata.get_transform_frame_camera();

			if (device_T_flippedCamera != nullptr)
			{
				*device_T_camera = AnyCameraD::flippedTransformationRightSide(Utilities::toHomogenousMatrix4<double, double>(*device_T_flippedCamera));
			}
			else
			{
				device_T_camera->toNull();
			}
		}

		if (modelName == "Fisheye62:f,u0,v0,k0,k1,k2,k3,k4,k5,p0,p1")
		{
			const std::vector<double>& parameters = camera.get_params();

			if (parameters.size() == 11)
			{
				return std::make_shared<AnyCameraFisheyeD>(FisheyeCameraD(width, height, FisheyeCameraD::PC_11_PARAMETERS_ONE_FOCAL_LENGTH, parameters.data()));
			}

			ocean_assert(false && "Invalid parameter number!");
		}
	}

	return nullptr;
}

}

}

}

}
