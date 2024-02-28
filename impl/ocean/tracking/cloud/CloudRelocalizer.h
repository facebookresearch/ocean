// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_TRACKING_CLOUD_CLOUD_RELOCALIZER_H
#define META_OCEAN_TRACKING_CLOUD_CLOUD_RELOCALIZER_H

#include "ocean/tracking/cloud/Cloud.h"

#include "ocean/base/Frame.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/PinholeCamera.h"

#include "ocean/unifiedfeatures/FeatureDetector.h"
#include "ocean/unifiedfeatures/DescriptorGenerator.h"

#include <artech/relocalization/IRelocalizationClient.hpp>

namespace Ocean
{

namespace Tracking
{

namespace Cloud
{

/**
 * This class implements a low level cloud relocalizer.
 * @ingroup trackingcloud
 */
class OCEAN_TRACKING_CLOUD_EXPORT CloudRelocalizer
{
	public:

		/**
		 * This class holds parameters to configure the relocalizer.
		 */
		class Configuration
		{
			public:

				/**
				 * Creates a new configuration object with default parameters.
				 */
				Configuration() = default;

				/**
				 * Creates a new configuration object.
				 * @param mapLabel The label of the relocalization map, i.e. the exact version of the map to be used; empty to use any suitable map close to the GPS location
				 * @param releaseEnvironment The release environment the relocalization map is part of, if empty the default environment 'Production' is used
				 * @param mode The mode to be used during the relocalization
				 * @param coordinateProviderLevel The coordinate level to be used during relocalization, possible values are "l1" or "l2".
				 */
				inline Configuration(std::string&& mapLabel, std::string&& releaseEnvironment, std::string&& mode = std::string(), std::string&& coordinateProviderLevel = std::string());

				/**
				 * Returns the map label.
				 * @return The map label to be used during relocalization
				 */
				inline const std::string& mapLabel() const;

				/**
				 * Returns the release environment.
				 * @return The release environment to be used during relocalization
				 */
				inline const std::string& releaseEnvironment() const;

				/**
				 * Returns the mode.
				 * @return The mode to be used during the relocalization
				 */
				inline const std::string& mode() const;

				/**
				 * Returns the coordinate provider level.
				 * @return The coordinate provider level to be used during the relocalization
				 */
				inline const std::string& coordinateProviderLevel() const;

				/**
				 * Parses the configuration for the relocalizer from a JSON string.
				 * @param value The JSON string from which the configuration will be parsed
				 * @param configuration The resulting configuration
				 * @param errorMessage Optional resulting error message; nullptr if not of interest
				 * @return True, if succeeded
				 */
				static bool parseConfiguration(const std::string& value, Configuration& configuration, std::string* errorMessage = nullptr);

			protected:

				/// The label of the relocalization map, i.e. the exact version of the map to be used; empty to use any suitable map close to the GPS location.
				std::string mapLabel_;

				/// The release environment the relocalization map is part of, if empty the default environment 'Production' is used.
				std::string releaseEnvironment_;

				/// The mode to be used during relocalization, empty to use the default mode.
				std::string mode_;

				/// The coordinate provider level to be used during relocalization, empty to use the default level (specified in the relocalization client and subject to change).
				std::string coordinateProviderLevel_;
		};

		/**
		 * This class holds the relevant information of a placement.
		 */
		class Placement
		{
			public:

				/**
				 * Creates a new placement object.
				 * @param identification The placement's identification, must be valid
				 * @param anchor_T_placement The transformation between this new placement and the corresponding anchor, must be valid
				 */
				inline Placement(std::string&& identification, const HomogenousMatrix4& anchor_T_placement);

				/**
				 * Returns the identification of this placement.
				 * @return The placement's identification
				 */
				inline const std::string& identification() const;

				/**
				 * Returns the transformation between this placement and the corresponding anchor.
				 * @return The placement's transformation
				 */
				inline const HomogenousMatrix4& anchor_T_placement() const;

			public:

				/// The identification of the placement.
				std::string identification_;

				/// The transformation between the placement and the corresponding anchor.
				HomogenousMatrix4 anchor_T_placement_ = HomogenousMatrix4(false);
		};

		/**
		 * Definition of a vector holding placements.
		 */
		typedef std::vector<Placement> Placements;

	protected:

		/**
		 * Definition of an unordered map mapping strings to transformations.
		 */
		typedef std::unordered_map<std::string, HomogenousMatrixF4> StringTransformationMap;

	public:

		/**
		 * Determines the camera pose in relation to a specified relocalization map.
		 * This function applies a pure per-frame relocalization without any additional filter or pre/post-processing.
		 * @param camera The camera profile defining the projection, must be a valid pinhole camera
		 * @param yFrame The image to be used for relocalization, must have pixel format FORMAT_Y8, must be valid
		 * @param gpsLocation The GPS location, with latitude (x) in degree and longitude (y) in degree, must be valid
		 * @param gravityVector The gravity vector with unit length, defined in the coordinate system of the camera (with a default camera pointing towards negative z space), the gravity vector points towards the center of earth, must be valid
		 * @param configuration The configuration to be used during relocalization
		 * @param client The HTTP client which will be used for the relocalization request, must be valid
		 * @param anchor_T_camera The resulting transformation between camera and anchor, with a default camera pointing towards negative z space
		 * @param objectPoints Optional resulting 3D object points of the feature coorespondences which have been used to determine the camera pose, defined in the coordinate system of the map anchor
		 * @param imagePoints Optional resulting 2D image points of the feature correspondences, one for each object point
		 * @param alignerAnchorId Optional FBID of the nearest trackable
		 * @param alignerAnchorUuid Optional UUID of the nearest trackable
		 * @return True, if succeeded
		 */
		static bool relocalize(const AnyCamera& camera, const Frame& yFrame, const VectorD2& gpsLocation, const Vector3& gravityVector, const Configuration& configuration, facebook::mobile::xr::IRelocalizationClient& client, HomogenousMatrix4& anchor_T_camera, Vectors3* objectPoints = nullptr, Vectors2* imagePoints = nullptr, std::string* alignerAnchorId = nullptr, visiontypes::AnchorUuid* alignerAnchorUuid = nullptr);

		/**
		 * Determines the camera pose in relation to a specified relocalization map and determines placements corresponding to the relocalization map.
		 * This function applies a pure per-frame relocalization without any additional filter or pre/post-processing.
		 * @param camera The camera profile defining the projection, must be a valid pinhole camera
		 * @param yFrame The image to be used for relocalization, must have pixel format FORMAT_Y8, must be valid
		 * @param gpsLocation The GPS location, with latitude (x) in degree and longitude (y) in degree, must be valid
		 * @param gravityVector The gravity vector with unit length, defined in the coordinate system of the camera (with a default camera pointing towards negative z space), the gravity vector points towards the center of earth, must be valid
		 * @param configuration The configuration to be used during relocalization
		 * @param client The HTTP client which will be used for the relocalization request, must be valid
		 * @param anchor_T_camera The resulting transformation between camera and anchor, with a default camera pointing towards negative z space
		 * @param placements The resulting placements associated with the relocalization map which was used to successfully determine the camera pose
		 * @param objectPoints Optional resulting 3D object points of the feature coorespondences which have been used to determine the camera pose, defined in the coordinate system of the map
		 * @param imagePoints Optional resulting 2D image points of the feature correspondences, one for each object point
		 * @return True, if succeeded
		 */
		static bool relocalizeWithPlacements(const AnyCamera& camera, const Frame& yFrame, const VectorD2& gpsLocation, const Vector3& gravityVector, const Configuration& configuration, facebook::mobile::xr::IRelocalizationClient& client, HomogenousMatrix4& anchor_T_camera, Placements& placements, Vectors3* objectPoints = nullptr, Vectors2* imagePoints = nullptr);

		/**
		 * Determines the camera pose in relation to the ECEF coordinate system.
		 * @param camera The camera profile defining the projection, must be a valid pinhole camera
		 * @param yFrame The image to be used for relocalization, must have pixel format FORMAT_Y8, must be valid
		 * @param gpsLocation The GPS location, with latitude (x) in degree and longitude (y) in degree, must be valid
		 * @param gravityVector The gravity vector with unit length, defined in the coordinate system of the camera (with a default camera pointing towards negative z space), the gravity vector points towards the center of earth, must be valid
		 * @param configuration The configuration to be used during relocalization
		 * @param client The HTTP client which will be used for the relocalization request, must be valid
		 * @param ecefWorld_T_camera The resulting transformation between camera and ECEF world, with a default camera pointing towards negative z space
		 * @param objectPoints Optional resulting 3D object points of the feature coorespondences which have been used to determine the camera pose, defined in the ECEF world coordinate system
		 * @param imagePoints Optional resulting 2D image points of the feature correspondences, one for each object point
		 */
		static bool relocalizeToECEF(const AnyCamera& camera, const Frame& yFrame, const VectorD2& gpsLocation, const Vector3& gravityVector, const Configuration& configuration, facebook::mobile::xr::IRelocalizationClient& client, HomogenousMatrixD4& ecefWorld_T_camera, VectorsD3* objectPoints = nullptr, VectorsD2* imagePoints = nullptr);

		/**
		 * Returns the HTTP client which can be used for sending relocalization requests.
		 * The function extracts the user login information from Platform::Meta::Login.
		 * @return The resulting HTTP client, nullptr if the client could not be created
		 * @see Platform::Meta::Login.
		 */
		static std::shared_ptr<facebook::mobile::xr::IRelocalizationClient> createClient();

		/**
		 * Parses the label of a map which may contain an explicit GPS location.
		 * Optional, the map label can contain an explicit GPS location with pattern "<MapLabel>@GPS<Latitude>,<Longitude>".
		 * @param mapLabel The label of the map with potential explicit GPS location, must be valid
		 * @param explicitGPSLocation The resulting explicit GPS location if part of the given label of the map, otherwise both components will be NumericD::minValue()
		 * @return The label of the map without GPS location
		 */
		static std::string parseMapLabel(const std::string& mapLabel, VectorD2& explicitGPSLocation);

		/**
		 * Parses the parameters of a map.
		 * The parameters can be specified in the following pattern:
		 * <pre>
		 * Only one map label:
		 * \<MapLabel>
		 * Only one map label:
		 * mapLabel=\<MapLabel>
		 * Only one release environment:
		 * releaseEnvironment=\<ReleaseEnvironment>
		 * A map label and a release environment:
		 * mapLabel=\<MapLabel> releaseEnvironment=\<ReleaseEnvironment>
		 * </pre>
		 * @param parameters The parameters to parse
		 * @param mapLabel The map label in case the parameters contained a map label
		 * @param releaseEnvironment The release environment in case the parameters contained a release environment
		 * @return True, if succeeded
		 */
		static bool parseMapParameters(const std::string& parameters, std::string& mapLabel, std::string& releaseEnvironment);

	protected:

		/**
		 * Creates the RelocalizationParameters object for a given image with known GPS coordinate and gravity vector.
		 * @param anyCamera The camera profile defining the projection, must be a valid pinhole camera
		 * @param yFrame The image in which the features will be detected, must have pixel format FORMAT_Y8, must be valid
		 * @param gpsLocation The GPS location, with latitude (x) in degree and longitude (y) in degree, must be valid
		 * @param gravityVector The gravity vector with unit length, defined in the coordinate system of the camera (with a default camera pointing towards negative z space), the gravity vector points towards the center of earth, must be valid
		 * @return The resulting RelocalizationParameters object, nullptr if invalid
		 */
		static std::unique_ptr<facebook::mobile::xr::RelocalizationParameters> createRelocalizationParameters(const AnyCamera& anyCamera, const Frame& yFrame, const VectorD2& gpsLocation, const Vector3& gravityVector);

		/**
		 * Extracts the feature correspondences from a RelocalizationSpatialAnchorsResult object if the object contains the correspondences.
		 * @param relocalizationResult The relocalization result from which the feature coorespondences will be extracted
		 * @param objectPoints_T_camera The transformation between the camera and the object points, will be applied to each resulting 3D object point as the relocalization result provides 3D object points defined in the camera, must be valid
		 * @param objectPoints Optional resulting 3D object points of the feature coorespondences which have been used to determine the camera pose, defined in the coordinate system of the map, nullptr if not of interest
		 * @param imagePoints Optional resulting 2D image points of the feature correspondences, one for each object point, nullptr if not of interest
		 * @tparam T The scalar data type, either 'float' or 'double'
		 */
		template <typename T>
		static void extractFeatureCorrespondences(const facebook::mobile::xr::RelocalizationSpatialAnchorsResult& relocalizationResult, const HomogenousMatrixT4<T>& objectPoints_T_camera, VectorsT3<T>* objectPoints = nullptr, VectorsT2<T>* imagePoints = nullptr);

		/*
		 * Extracts and serializes feature points in a given image.
		 * @param pinholeCamera The pinhole camera profile to be used, must be valid
		 * @param yFrame The image in which the features will be detected, must have pixel format FORMAT_Y8, must be valid
		 * @param serializedFeatures The resulting serialized features as binary data
		 * @return True, if succeeded
		 */
		static bool extractAndSerializeFeatures(const PinholeCamera& pinholeCamera, const Frame& yFrame, std::vector<uint8_t>& serializedFeatures);

		/*
		 * Extracts and serializes feature points in a given image.
		 * @param yFrame The image in which the features will be detected, must have pixel format FORMAT_Y8, must be valid
		 * @param featureDetector The feature detector to be used, must be valid
		 * @param descriptorGenerator The explicit descriptor generator to be used to generate the feature descriptors, nullptr to use feature detector's default descriptor generator (the feature detector must offer a default descriptor generator in this case)
		 * @param serializedFeatures The resulting serialized features as binary data
		 * @return True, if succeeded
		 */
		static bool extractAndSerializeFeatures(const Frame& yFrame, std::shared_ptr<UnifiedFeatures::FeatureDetector> featureDetector, std::shared_ptr<UnifiedFeatures::DescriptorGenerator> descriptorGenerator, std::vector<uint8_t>& serializedFeatures);
};

inline CloudRelocalizer::Configuration::Configuration(std::string&& mapLabel, std::string&& releaseEnvironment, std::string&& mode, std::string&& coordinateProviderLevel) :
	mapLabel_(std::move(mapLabel)),
	releaseEnvironment_(std::move(releaseEnvironment)),
	mode_(std::move(mode)),
	coordinateProviderLevel_(std::move(coordinateProviderLevel))
{
	// nothing to do here
}

inline const std::string& CloudRelocalizer::Configuration::mapLabel() const
{
	return mapLabel_;
}

inline const std::string& CloudRelocalizer::Configuration::releaseEnvironment() const
{
	return releaseEnvironment_;
}

inline const std::string& CloudRelocalizer::Configuration::mode() const
{
	return mode_;
}

inline const std::string& CloudRelocalizer::Configuration::coordinateProviderLevel() const
{
	return coordinateProviderLevel_;
}

inline CloudRelocalizer::Placement::Placement(std::string&& identification, const HomogenousMatrix4& anchor_T_placement) :
	identification_(std::move(identification)),
	anchor_T_placement_(anchor_T_placement)
{
	// nothing to do here
}

inline const std::string& CloudRelocalizer::Placement::identification() const
{
	return identification_;
}

inline const HomogenousMatrix4& CloudRelocalizer::Placement::anchor_T_placement() const
{
	return anchor_T_placement_;
}

}

}

}

#endif // META_OCEAN_TRACKING_CLOUD_CLOUD_RELOCALIZER_H
