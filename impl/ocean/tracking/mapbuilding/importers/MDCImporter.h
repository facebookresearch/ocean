// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_TRACKING_MAPBUILDING_IMPORTERS_MDC_IMPORTER_H
#define META_OCEAN_TRACKING_MAPBUILDING_IMPORTERS_MDC_IMPORTER_H

#include "ocean/tracking/mapbuilding/importers/Importers.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/HomogenousMatrix4.h"

#include <arvr/libraries/thrift_if/mapping/gen-cpp2/geometry_types.h>

namespace Ocean
{

namespace Tracking
{

namespace MapBuilding
{

namespace Importers
{

/**
 * This class implements an importer for MDCs (Map Data Contracts).
 * @ingroup trackingmapbuildingimporters
 */
class OCEAN_TRACKING_MAPBUILDING_IMPORTERS_EXPORT MDCImporter
{
	public:

		/**
		 * Definition of an observation pair combining an image view id with a point feature id.
		 */
		class ObservationPair
		{
			public:

				/**
				 * Default constructor.
				 */
				ObservationPair() = default;

				/**
				 * Creates a new pair object.
				 * @param imageViewId The id of the image view
				 * @param pointFeatureId The id of the point feature
				 */
				inline ObservationPair(const uint32_t imageViewId, const uint32_t pointFeatureId);

				/**
				 * Retuns whether two pairs are identical.
				 * @param observationPair The second pair to compare
				 * @return True, if so
				 */
				inline bool operator==(const ObservationPair& observationPair) const;

				/**
				 * Hash function.
				 * @param observationPair The object for which the hash value will be determined
				 * @return The resulting hash value
				 */
				inline size_t operator()(const ObservationPair& observationPair) const;

			public:

				/// The id of the image view.
				uint32_t imageViewId_ = uint32_t(-1);

				/// The id of the point feature.
				uint32_t pointFeatureId_ = uint32_t(-1);
		};

		/**
		 * This class combines the relevant information for a frame.
		 */
		class FrameData
		{
			public:

				/**
				 * Default constructor.
				 */
				FrameData() = default;

			public:

				/// The transformation between device and world, if known.
				HomogenousMatrixD4 world_T_device_ = HomogenousMatrixD4(false);

				/// The UTC timestamp in nanoseconds.
				uint64_t utcTimestamp_ = uint64_t(-1);

				/// The capture timestamp in microseconds.
				uint64_t captureTimestamp_ = uint64_t(-1);

				/// The ids of all image views.
				Indices32 imageViewIds_;
		};

		/**
		 * Definition of a pair combining a camera object with a device transformation (device_T_camera).
		 */
		typedef std::pair<SharedAnyCameraD, HomogenousMatrixD4> CameraPair;

		/**
		 * Definition of an unordered map mapping object point ids to object points.
		 */
		typedef std::unordered_map<uint32_t, Vector3> ObjectPointMap;

		/**
		 * Definition of a map combining an image point with a point track id.
		 */
		typedef std::pair<Vector2, uint32_t> ImagePointPair;

		/**
		 * Definition of an unordered map mapping image point ids to image point pairs.
		 */
		typedef std::unordered_map<ObservationPair, ImagePointPair, ObservationPair> ImagePointMap;

		/**
		 * Definition of an unordered map mapping ids to camera pairs.
		 */
		typedef std::unordered_map<uint32_t, CameraPair> CameraMap;

		/**
		 * Definition of a pair combining camera ids with point feature ids.
		 */
		typedef std::pair<uint32_t, Indices32> ImageViewPair;

		/**
		 * Definition of an unordered map mapping image view ids to image view pairs.
		 */
		typedef std::unordered_map<uint32_t, ImageViewPair> ImageViewMap;

		/**
		 * Definition of an unordered map mapping frame ids to frame data objects.
		 */
		typedef std::unordered_map<uint32_t, FrameData> FrameDataMap;

	public:

		/**
		 * Imports the MDC geometry data stored in a file.
		 * @param filename The name of the MDC geometry file containing the geometry data, must be valid
		 * @param objectPointMap The resulting map mapping point track ids to 3D object points
		 * @param imagePointMap The resulting map mapping image view/feature point id pairs to 2D image points and point track ids
		 * @param cameraMap The resulting map mapping camera ids to camera pairs
		 * @param imageViewMap The resulting map mapping image view ids to image view pairs
		 * @param frameDataMap The resulting map mapping frame ids to frame data objects
		 * @return True, if succeeded
		 */
		static bool importGeometryFromFile(const std::string& filename, ObjectPointMap& objectPointMap, ImagePointMap& imagePointMap, CameraMap& cameraMap, ImageViewMap& imageViewMap, FrameDataMap& frameDataMap);

		/**
		 * Imports the MDC geometry data stored in a file.
		 * @param filename The name of the MDC geometry file containing the geometry data, must be valid
		 * @param objectPoints The resulting 3D object points of all 3D features
		 * @return True, if succeeded
		 */
		static bool importGeometryFromFile(const std::string& filename, Vectors3& objectPoints);

	protected:

		/**
		 * Reads a map chunk from a given file.
		 * @param filename The name of the file from which the reloc chunk will be read
		 * @param mapChunk The resulting map chunk object
		 * @return True, if succeeded
		 */
		static bool readMapChunk(const std::string& filename, arvr::thrift_if::mapping::geometry::MapChunk& mapChunk);

		/**
		 * Extracts the camera profile from a given Thrift Camera object.
		 * @param camera The Thrift object from which the camera profile will be extracted
		 * @param id Optional resulting id of the camera, nullptr if not of interest
		 * @param device_T_camera Optional resulting transformation between camera and device (default camera is pointing towards the negative z-space with y-axis upwards), nullptr if not of interest
		 * @return The camera profile, nullptr if the profile could not be extracted
		 */
		static SharedAnyCameraD extractCamera(const arvr::thrift_if::mapping::geometry::Camera& camera, uint32_t* id = nullptr, HomogenousMatrixD4* device_T_camera = nullptr);
};

inline MDCImporter::ObservationPair::ObservationPair(const uint32_t imageViewId, const uint32_t pointFeatureId) :
	imageViewId_(imageViewId),
	pointFeatureId_(pointFeatureId)
{
	// nothing to do here
}

inline bool MDCImporter::ObservationPair::operator==(const ObservationPair& observationPair) const
{
	return imageViewId_ == observationPair.imageViewId_ && pointFeatureId_ == observationPair.pointFeatureId_;
}

inline size_t MDCImporter::ObservationPair::operator()(const ObservationPair& observationPair) const
{
	size_t seed = std::hash<Index32>{}(observationPair.imageViewId_);
	seed ^= std::hash<Index32>{}(observationPair.pointFeatureId_) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

	return seed;
}

}

}

}

}

#endif // META_OCEAN_TRACKING_MAPBUILDING_IMPORTERS_MDC_IMPORTER_H
