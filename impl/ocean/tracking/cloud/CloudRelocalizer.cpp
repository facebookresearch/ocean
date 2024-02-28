// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/tracking/cloud/CloudRelocalizer.h"

#include "ocean/io/JSONConfig.h"

#include "ocean/math/SophusUtilities.h"

#include "ocean/network/tigon/TigonClient.h"

#include "ocean/platform/meta/Login.h"

#include "ocean/unifiedfeatures/Utilities.h"
#include "ocean/unifiedfeatures/oceanfreak/OFDescriptorContainer.h"
#include "ocean/unifiedfeatures/oceanfreak/OFHarrisFeatureContainer.h"
#include "ocean/unifiedfeatures/oceanfreak/OFHarrisFreakFeatureDetector.h"

#ifdef OCEAN_PLATFORM_BUILD_APPLE_MACOS
	#include <artech/rnb/curlremotemapsclient/CurlRemoteMapsClient.h>
#endif

#include <artech/network/TigonHTTPClient.hpp>

#include <AsyncExecutor/InlineSerialExecutor.h>

#include <RemoteMapsClient/waldo/WaldoClient.hpp>

#include <ar/livemaps_service/client_utils/OceanFREAKThriftSerializer.h>

#include <thrift/lib/cpp2/protocol/Serializer.h>

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace Ocean
{

namespace Tracking
{

namespace Cloud
{

bool CloudRelocalizer::Configuration::parseConfiguration(const std::string& value, Configuration& configuration, std::string* errorMessage)
{
	IO::JSONConfig config(std::shared_ptr<std::istream>(new std::istringstream(value)));

	const std::unordered_set<std::string> supportedFields =
		{
			"mapLabel",
			"releaseEnvironment",
			"mode",
			"coordinateProviderLevel",
		};

	for (unsigned int n = 0u; n < config.values(); ++n)
	{
		std::string field;
		config.value(n, field);

		if (supportedFields.find(field) == supportedFields.cend())
		{
			if (errorMessage != nullptr)
			{
				*errorMessage = "Not supported configuration field '" + field + "'";
			}

			return false;
		}
	}

	std::string mapLabel = config["mapLabel"](std::string());
	std::string releaseEnvironment = config["releaseEnvironment"](std::string());
	std::string mode = config["mode"](std::string());
	std::string coordinateProviderLevel = config["coordinateProviderLevel"](std::string());

	configuration = Configuration(std::move(mapLabel), std::move(releaseEnvironment), std::move(mode), std::move(coordinateProviderLevel));

	return true;
}

bool CloudRelocalizer::relocalize(const AnyCamera& camera, const Frame& yFrame, const VectorD2& gpsLocation, const Vector3& gravityVector, const Configuration& configuration, facebook::mobile::xr::IRelocalizationClient& client, HomogenousMatrix4& anchor_T_camera, Vectors3* objectPoints, Vectors2* imagePoints, std::string* alignerAnchorId, visiontypes::AnchorUuid* alignerAnchorUuid)
{
	ocean_assert(camera.isValid() && yFrame.isValid() && yFrame.isPixelFormatCompatible(FrameType::FORMAT_Y8));
	ocean_assert(camera.width() == yFrame.width() && camera.height() == yFrame.height());
	ocean_assert(gravityVector.isUnit());

	std::unique_ptr<facebook::mobile::xr::RelocalizationParameters> relocalizationParameters = createRelocalizationParameters(camera, yFrame, gpsLocation, gravityVector);

	if (!relocalizationParameters)
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	constexpr int clientRelocalizationParametersSearchRadiusWithoutMapLabel = 50;
	constexpr int clientRelocalizationParametersSearchRadiusWithMapLabel = 1000;
	relocalizationParameters->radius = configuration.mapLabel().empty() ? clientRelocalizationParametersSearchRadiusWithoutMapLabel : clientRelocalizationParametersSearchRadiusWithMapLabel;

	relocalizationParameters->coordinateProviderLevel = configuration.coordinateProviderLevel();

	relocalizationParameters->parameters["c.label"] = configuration.mapLabel();
	relocalizationParameters->parameters["c.release_environment"] = configuration.releaseEnvironment();
	relocalizationParameters->parameters["c.mode"] = configuration.mode();

	if (objectPoints != nullptr || imagePoints != nullptr)
	{
		relocalizationParameters->parameters["c.inlier_info"] = "true";
	}

	std::future<facebook::mobile::xr::IRelocalizationClient::Result<facebook::mobile::xr::RelocalizationSpatialAnchorsResult>> relocalizationResultFuture = client.relocalizeSpatialAnchor(std::move(relocalizationParameters));

	if (!relocalizationResultFuture.valid())
	{
		Log::error() << "Invalid relocalization result";
		return false;
	}

	const std::future_status futureStatus = relocalizationResultFuture.wait_for(std::chrono::seconds(5));

	if (futureStatus == std::future_status::timeout)
	{
		return false;
	}

	facebook::mobile::xr::IRelocalizationClient::Result<facebook::mobile::xr::RelocalizationSpatialAnchorsResult> relocalizationResult = relocalizationResultFuture.get();

	if (relocalizationResult.hasError())
	{
		Log::error() << "Relocalization failed: '" << relocalizationResult.error() << "'";

		return false;
	}

	const facebook::mobile::xr::RelocalizationSpatialAnchorsResult& result = *relocalizationResult;

	const HomogenousMatrix4 anchor_T_flippedCamera(SophusUtilities::toHomogenousMatrix4<float, Scalar>(result.transformAlignerAnchorCamera));
	anchor_T_camera = AnyCamera::flippedTransformationRightSide(anchor_T_flippedCamera);

	extractFeatureCorrespondences(result, anchor_T_camera, objectPoints, imagePoints);

	if (alignerAnchorId != nullptr)
	{
		*alignerAnchorId = result.alignerAnchorId;
	}

	if (alignerAnchorUuid != nullptr)
	{
		*alignerAnchorUuid = result.alignerAnchorUuid;
	}

	return true;
}

bool CloudRelocalizer::relocalizeWithPlacements(const AnyCamera& camera, const Frame& yFrame, const VectorD2& gpsLocation, const Vector3& gravityVector, const Configuration& configuration, facebook::mobile::xr::IRelocalizationClient& client, HomogenousMatrix4& anchor_T_camera, Placements& placements, Vectors3* objectPoints, Vectors2* imagePoints)
{
	ocean_assert(camera.isValid() && yFrame.isValid() && yFrame.isPixelFormatCompatible(FrameType::FORMAT_Y8));
	ocean_assert(camera.width() == yFrame.width() && camera.height() == yFrame.height());
	ocean_assert(gravityVector.isUnit());

	std::unique_ptr<facebook::mobile::xr::RelocalizationParameters> relocalizationParameters = createRelocalizationParameters(camera, yFrame, gpsLocation, gravityVector);

	if (!relocalizationParameters)
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	constexpr int clientRelocalizationParametersSearchRadiusWithoutMapLabel = 50;
	constexpr int clientRelocalizationParametersSearchRadiusWithMapLabel = 1000;
	relocalizationParameters->radius = configuration.mapLabel().empty() ? clientRelocalizationParametersSearchRadiusWithoutMapLabel : clientRelocalizationParametersSearchRadiusWithMapLabel;

	if (configuration.coordinateProviderLevel() != "l1")
	{
		Log::error() << "The relocalizer with placements needs 'l1' as coordinate provider level";
		return false;
	}

	relocalizationParameters->coordinateProviderLevel = configuration.coordinateProviderLevel();

	relocalizationParameters->parameters["c.label"] = configuration.mapLabel();
	relocalizationParameters->parameters["c.release_environment"] = configuration.releaseEnvironment();
	relocalizationParameters->parameters["c.mode"] = configuration.mode();

	if (objectPoints != nullptr || imagePoints != nullptr)
	{
		relocalizationParameters->parameters["c.inlier_info"] = "true";
	}

	std::future<facebook::mobile::xr::IRelocalizationClient::Result<facebook::mobile::xr::RelocalizationSpatialAnchorsResult>> relocalizationResultFuture = client.relocalizeSpatialAnchor(std::move(relocalizationParameters));

	if (!relocalizationResultFuture.valid())
	{
		Log::error() << "Invalid relocalization result";
		return false;
	}

	const std::future_status futureStatus = relocalizationResultFuture.wait_for(std::chrono::seconds(5));

	if (futureStatus == std::future_status::timeout)
	{
		return false;
	}

	facebook::mobile::xr::IRelocalizationClient::Result<facebook::mobile::xr::RelocalizationSpatialAnchorsResult> relocalizationResult = relocalizationResultFuture.get();

	if (relocalizationResult.hasError())
	{
		Log::error() << "Relocalization failed: '" << relocalizationResult.error() << "'";

		return false;
	}

	placements.clear();

	const facebook::mobile::xr::RelocalizationSpatialAnchorsResult& result = *relocalizationResult;

	const HomogenousMatrix4 anchor_T_flippedCamera(SophusUtilities::toHomogenousMatrix4<float, Scalar>(result.transformAlignerAnchorCamera));
	anchor_T_camera = AnyCamera::flippedTransformationRightSide(anchor_T_flippedCamera);

	StringTransformationMap internalAnchorsMap;
	internalAnchorsMap.reserve(result.internalAnchors.size());

	for (const facebook::mobile::xr::RelocalizationSpatialAnchorsResult::InternalAnchorData& internalAnchorData : result.internalAnchors)
	{
		const HomogenousMatrixF4 anchor_T_internalAnchor(SophusUtilities::toHomogenousMatrix4<float, float>(internalAnchorData.transformAlignerAnchorInternalAnchor));
		ocean_assert(anchor_T_internalAnchor.rotationMatrix().isOrthonormal());

		internalAnchorsMap.emplace(internalAnchorData.uuid, anchor_T_internalAnchor);
	}

	for (const facebook::mobile::xr::RelocalizationSpatialAnchorsResult::SpatialAnchorData& spactialAnchorData : result.spatialAnchors)
	{
		const std::string& uuid = spactialAnchorData.internal_anchor_uuid;

		const StringTransformationMap::const_iterator iInternal = internalAnchorsMap.find(uuid);

		if (iInternal != internalAnchorsMap.cend())
		{
			const HomogenousMatrixF4 internalAnchor_T_spactialAnchor(SophusUtilities::toHomogenousMatrix4<float, float>(spactialAnchorData.transformInternalAnchorSpatialAnchor));
			ocean_assert(internalAnchor_T_spactialAnchor.rotationMatrix().isOrthonormal());

			const HomogenousMatrixF4& anchor_T_internalAnchor = iInternal->second;

			const HomogenousMatrix4 anchor_T_spactialAnchor(anchor_T_internalAnchor * internalAnchor_T_spactialAnchor);
			ocean_assert(anchor_T_spactialAnchor.rotationMatrix().isOrthonormal(Numeric::weakEps()));

			placements.emplace_back(std::string(spactialAnchorData.fbid), anchor_T_spactialAnchor);
		}
		else
		{
			Log::warning() << "Missing internal anchor for id '" << uuid << "'";
		}
	}

	extractFeatureCorrespondences(result, anchor_T_camera, objectPoints, imagePoints);

	return true;
}

bool CloudRelocalizer::relocalizeToECEF(const AnyCamera& camera, const Frame& yFrame, const VectorD2& gpsLocation, const Vector3& gravityVector, const Configuration& configuration, facebook::mobile::xr::IRelocalizationClient& client, HomogenousMatrixD4& ecefWorld_T_camera, VectorsD3* objectPoints, VectorsD2* imagePoints)
{
	ocean_assert(camera.isValid() && yFrame.isValid() && yFrame.isPixelFormatCompatible(FrameType::FORMAT_Y8));
	ocean_assert(camera.width() == yFrame.width() && camera.height() == yFrame.height());
	ocean_assert(gravityVector.isUnit());

	std::unique_ptr<facebook::mobile::xr::RelocalizationParameters> relocalizationParameters = createRelocalizationParameters(camera, yFrame, gpsLocation, gravityVector);

	if (!relocalizationParameters)
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	constexpr int clientRelocalizationParametersSearchRadiusWithoutMapLabel = 50;
	constexpr int clientRelocalizationParametersSearchRadiusWithMapLabel = 1000;
	relocalizationParameters->radius = configuration.mapLabel().empty() ? clientRelocalizationParametersSearchRadiusWithoutMapLabel : clientRelocalizationParametersSearchRadiusWithMapLabel;

	relocalizationParameters->coordinateProviderLevel = configuration.coordinateProviderLevel();

	relocalizationParameters->parameters["c.label"] = configuration.mapLabel();
	relocalizationParameters->parameters["c.release_environment"] = configuration.releaseEnvironment();
	relocalizationParameters->parameters["c.mode"] = configuration.mode();

	if (objectPoints != nullptr || imagePoints != nullptr)
	{
		relocalizationParameters->parameters["c.inlier_info"] = "true";
	}

	std::future<facebook::mobile::xr::IRelocalizationClient::Result<facebook::mobile::xr::RelocalizationSpatialAnchorsResult>> relocalizationResultFuture = client.relocalizeSpatialAnchor(std::move(relocalizationParameters));

	if (!relocalizationResultFuture.valid())
	{
		Log::error() << "Invalid relocalization result";
		return false;
	}

	const std::future_status futureStatus = relocalizationResultFuture.wait_for(std::chrono::seconds(5));

	if (futureStatus == std::future_status::timeout)
	{
		return false;
	}

	facebook::mobile::xr::IRelocalizationClient::Result<facebook::mobile::xr::RelocalizationSpatialAnchorsResult> relocalizationResult = relocalizationResultFuture.get();

	if (relocalizationResult.hasError())
	{
		Log::error() << "Relocalization failed: '" << relocalizationResult.error() << "'";

		return false;
	}

	const facebook::mobile::xr::RelocalizationSpatialAnchorsResult& result = *relocalizationResult;

	const HomogenousMatrixD4 flippedECEFWorld_T_camera = HomogenousMatrixD4(SophusUtilities::toHomogenousMatrix4<double, double>(result.ecefTransform));

	if (flippedECEFWorld_T_camera.isIdentity())
	{
		// **TODO** this is a hack, we need a null matrix to specify invalid ECEF coordinates instead
		return false;
	}

	ecefWorld_T_camera = AnyCamera::flippedTransformationRightSide(flippedECEFWorld_T_camera);

	extractFeatureCorrespondences<double>(result, ecefWorld_T_camera, objectPoints, imagePoints);

	return true;
}

std::shared_ptr<facebook::mobile::xr::IRelocalizationClient> CloudRelocalizer::createClient()
{

#if defined(OCEAN_PLATFORM_BUILD_APPLE_MACOS)

	const std::string userToken = Platform::Meta::Login::get().userToken(Platform::Meta::Login::LT_FACEBOOK);
	const std::string userId = Platform::Meta::Login::get().userId(Platform::Meta::Login::LT_FACEBOOK);

	if (!userId.empty() && !userToken.empty())
	{
		return facebook::artech::rnb::ConfigureJord(userId, userToken);
	}
	else
	{
		Log::error() << "Missing user id/token";
		return nullptr;
	}

#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

	facebook::mobile::xr::WaldoMapsClientConfig waldoMapsClientConfig;
	waldoMapsClientConfig.appId = "ocean_cloud_relocalizer";
	waldoMapsClientConfig.deviceType = "unknown";
	waldoMapsClientConfig.clientVersion = "0";

	std::shared_ptr<facebook::tigon::TigonService> tigonService = Network::Tigon::TigonClient::get().clientTigonService();
	std::shared_ptr<facebook::mobile::xplat::executor::SerialExecutor> clientExecutor = std::make_shared<facebook::mobile::xplat::executor::InlineSerialExecutor>();

	if (!tigonService || !clientExecutor)
	{
		Log::error() << "Tigon service not available";
		return nullptr;
	}

	std::shared_ptr<facebook::mobile::xr::IHTTPClient> httpClient(std::make_shared<facebook::mobile::xr::TigonHTTPClient>(tigonService, clientExecutor));

	return std::make_shared<facebook::mobile::xr::WaldoClient>(waldoMapsClientConfig, httpClient);
}

std::string CloudRelocalizer::parseMapLabel(const std::string& mapLabel, VectorD2& explicitGPSLocation)
{
	ocean_assert(!mapLabel.empty());

	const std::string::size_type gpsStartPos = mapLabel.find("@GPS");

	if (gpsStartPos == std::string::npos)
	{
		return mapLabel;
	}

	explicitGPSLocation = VectorD2(NumericD::minValue(), NumericD::minValue());

	const std::string::size_type commaPos = mapLabel.find(',', gpsStartPos + 4);

	if (commaPos != std::string::npos)
	{
		const std::string latitudeString = mapLabel.substr(gpsStartPos + 4, commaPos - gpsStartPos - 4);
		const std::string longitudeString = mapLabel.substr(commaPos + 1);

		double latitude;
		double longitude;
		if (String::isNumber(latitudeString, true, &latitude) && String::isNumber(longitudeString, true, &longitude))
		{
			explicitGPSLocation = VectorD2(latitude, longitude);
		}
	}

	if (explicitGPSLocation == VectorD2(NumericD::minValue(), NumericD::minValue()))
	{
		Log::warning() << "Invalid GPS location in map label";
	}

	return mapLabel.substr(0, gpsStartPos);
}

bool CloudRelocalizer::parseMapParameters(const std::string& parameters, std::string& mapLabel, std::string& releaseEnvironment)
{
	if (parameters.empty())
	{
		return false;
	}

	mapLabel.clear();
	releaseEnvironment.clear();

	const std::string tagMapLabel = "mapLabel=";
	const std::string tagReleaseEnvironment = "releaseEnvironment=";

	const std::string::size_type positionMapLabel = parameters.find(tagMapLabel);
	const std::string::size_type positionReleaseEnvironment = parameters.find(tagReleaseEnvironment);

	if (positionMapLabel == std::string::npos && positionReleaseEnvironment == std::string::npos)
	{
		mapLabel = String::trim(parameters);
		return !mapLabel.empty();
	}

	if (positionMapLabel != std::string::npos && positionReleaseEnvironment == std::string::npos)
	{
		mapLabel = String::trim(parameters.substr(positionMapLabel + tagMapLabel.size()));
		return !mapLabel.empty();
	}

	if (positionMapLabel == std::string::npos && positionReleaseEnvironment != std::string::npos)
	{
		releaseEnvironment = String::trim(parameters.substr(positionReleaseEnvironment + tagReleaseEnvironment.size()));
		return !releaseEnvironment.empty();
	}

	ocean_assert(positionMapLabel != std::string::npos && positionReleaseEnvironment != std::string::npos);

	size_t mapLabelLength = 0;
	size_t releaseEnvironmentLength = 0;

	if (positionMapLabel < positionReleaseEnvironment)
	{
		mapLabelLength = positionReleaseEnvironment - positionMapLabel - tagMapLabel.size();
		releaseEnvironmentLength = parameters.size() - positionReleaseEnvironment - tagReleaseEnvironment.size();
	}
	else
	{
		releaseEnvironmentLength = positionMapLabel - positionReleaseEnvironment - tagReleaseEnvironment.size();
		mapLabelLength = parameters.size() - positionMapLabel - tagMapLabel.size();
	}

	mapLabel = String::trim(parameters.substr(positionMapLabel + tagMapLabel.size(), mapLabelLength));
	releaseEnvironment = String::trim(parameters.substr(positionReleaseEnvironment + tagReleaseEnvironment.size(), releaseEnvironmentLength));

	return true;
}

std::unique_ptr<facebook::mobile::xr::RelocalizationParameters> CloudRelocalizer::createRelocalizationParameters(const AnyCamera& camera, const Frame& yFrame, const VectorD2& gpsLocation, const Vector3& gravityVector)
{
	ocean_assert(camera.isValid() && yFrame.isValid() && yFrame.isPixelFormatCompatible(FrameType::FORMAT_Y8));
	ocean_assert(camera.width() == yFrame.width() && camera.height() == yFrame.height());
	ocean_assert(gravityVector.isUnit());

	if (!camera.isValid() || camera.anyCameraType() != AnyCameraType::PINHOLE || camera.name() != AnyCameraPinhole::WrappedCamera::name())
	{
		Log::error() << "CloudRelocalizer: Provided camera model must be a pinhole camera";
		ocean_assert(false && "CloudRelocalizer: Provided camera model must be a pinhole camera");

		return nullptr;
	}

	const PinholeCamera& pinholeCamera = ((const AnyCameraPinhole&)camera).actualCamera();

	std::vector<uint8_t> serializedFeatures;
	if (!extractAndSerializeFeatures(pinholeCamera, yFrame, serializedFeatures))
	{
		return nullptr;
	}

	facebook::mobile::xr::RelocalizationParameters::FeaturesQuery featuresQuery;

	featuresQuery.cameraType = facebook::mobile::xr::RelocalizationParameters::RawImageQuery::CameraType::POLY;
	featuresQuery.cameraIntrinsics.normalized_fx = double(pinholeCamera.focalLengthX()) / double(pinholeCamera.width());
	featuresQuery.cameraIntrinsics.normalized_fy = double(pinholeCamera.focalLengthY()) / double(pinholeCamera.height());
	featuresQuery.cameraIntrinsics.normalized_mx = double(pinholeCamera.principalPointX()) / double(pinholeCamera.width());
	featuresQuery.cameraIntrinsics.normalized_my = double(pinholeCamera.principalPointY()) / double(pinholeCamera.height());
	featuresQuery.cameraIntrinsics.k1 = double(pinholeCamera.radialDistortion().first);
	featuresQuery.cameraIntrinsics.k2 = double(pinholeCamera.radialDistortion().second);

	typedef UnifiedFeatures::OceanFreak::OFHarrisFreakFeatureDetector32 OFHarrisFreakFeatureDetector32;
	using DescriptorType = OFHarrisFreakFeatureDetector32::FreakDescriptor;
	featuresQuery.descriptorLength = std::tuple_size<DescriptorType::MultilevelDescriptorData::value_type>::value;
	assert(featuresQuery.descriptorLength == 32 || featuresQuery.descriptorLength == 64);

	featuresQuery.data = std::make_unique<std::vector<uint8_t>>(std::move(serializedFeatures));
	featuresQuery.size = Eigen::Vector2i(int(pinholeCamera.width()), int(pinholeCamera.height()));

	std::unique_ptr<facebook::mobile::xr::RelocalizationParameters> relocalizationParameters = std::make_unique<facebook::mobile::xr::RelocalizationParameters>();
	relocalizationParameters->clientMutationID = boost::uuids::to_string(boost::uuids::random_generator()());

	relocalizationParameters->imageQuery = std::move(featuresQuery);

	relocalizationParameters->parameters["c.descriptor_type"] = featuresQuery.descriptorLength == 32 ? "Multi3FREAK32" : "Multi3FREAK64";

	relocalizationParameters->location.latitude = gpsLocation.x();
	relocalizationParameters->location.longitude = gpsLocation.y();

	// **TODO** currently, the cloud reloc pipeline flips the gravity vector (rotation around x-axis by 180 degree), therfore
	// therefore, currently we need to provide the gravity vector in the CG camera (not flipped)
	relocalizationParameters->gravity = Eigen::Vector3f(float(gravityVector.x()), float(gravityVector.y()), float(gravityVector.z()));

	relocalizationParameters->timestamp = double(yFrame.timestamp());

	relocalizationParameters->parameters["c.client_tag"] = "cloud_relocalizer";

	return relocalizationParameters;
}

template <typename T>
void CloudRelocalizer::extractFeatureCorrespondences(const facebook::mobile::xr::RelocalizationSpatialAnchorsResult& relocalizationResult, const HomogenousMatrixT4<T>& objectPoints_T_camera, VectorsT3<T>* objectPoints, VectorsT2<T>* imagePoints)
{
	ocean_assert(objectPoints_T_camera.isValid());

	if (objectPoints != nullptr)
	{
		objectPoints->clear();
		objectPoints->reserve(relocalizationResult.inlierPoints.size());

		for (const facebook::mobile::xr::RelocalizationSpatialAnchorsResult::InlierPoint& inlier : relocalizationResult.inlierPoints)
		{
			// the server is sending object points defined in the flipped camera coordinate system,
			// we convert the object points so that they are defined in the standard camera coordinate system
			// further, we convert the objects points so that they are defined in the anchor

			objectPoints->emplace_back(objectPoints_T_camera * VectorT3(T(inlier.position.x()), T(-inlier.position.y()), T(-inlier.position.z())));
		}
	}

	if (imagePoints != nullptr)
	{
		imagePoints->clear();
		imagePoints->reserve(relocalizationResult.inlierPoints.size());

		for (const facebook::mobile::xr::RelocalizationSpatialAnchorsResult::InlierPoint& inlier : relocalizationResult.inlierPoints)
		{
			imagePoints->emplace_back(T(inlier.coordinates.x()), T(inlier.coordinates.y()));
		}
	}
}

bool CloudRelocalizer::extractAndSerializeFeatures(const PinholeCamera& pinholeCamera, const Frame& yFrame, std::vector<uint8_t>& serializedFeatures)
{
	ocean_assert(pinholeCamera.isValid() && yFrame.isValid());
	ocean_assert(pinholeCamera.width() == yFrame.width() && pinholeCamera.height() == yFrame.height());

	constexpr unsigned int featurePyramidLevels = 10u;

	typedef UnifiedFeatures::OceanFreak::OFHarrisFreakFeatureDetector32 OFHarrisFreakFeatureDetector32;

	const PinholeCamera cameraWithoutDistortion(pinholeCamera.width(), pinholeCamera.height(), pinholeCamera.focalLengthX(), pinholeCamera.focalLengthY(), pinholeCamera.principalPointX(), pinholeCamera.principalPointY()); // the freak descriptors do not support pinholeCamera distortion

	std::shared_ptr<OFHarrisFreakFeatureDetector32::FreakDescriptor::CameraDerivativeFunctor> cameraDerivativeFunctor = std::make_shared<OFHarrisFreakFeatureDetector32::FreakDescriptor::PinholeCameraDerivativeFunctor>(cameraWithoutDistortion, featurePyramidLevels);

	const float inverseFocalLength = 2.0f / float(pinholeCamera.focalLengthX() + pinholeCamera.focalLengthY());
	OFHarrisFreakFeatureDetector32::OFParameters parameters(cameraDerivativeFunctor, inverseFocalLength);

	parameters.minFrameArea = 50u * 50u; // **TODO** use function to create default parameters for this case
	parameters.maxFrameArea = 640u * 480u;
	parameters.expectedHarrisCorners640x480 = 1000u;
	parameters.harrisCornersReductionScale = Scalar(0.4);
	parameters.harrisCornerThreshold = 1u;
	parameters.removeInvalid = true;
	parameters.border = Scalar(20);
	parameters.determineExactHarrisCornerPositions = true;
	parameters.yFrameIsUndistorted = false;

	std::shared_ptr<UnifiedFeatures::FeatureDetector> featureDetector = std::make_shared<OFHarrisFreakFeatureDetector32>(std::move(parameters));

	return extractAndSerializeFeatures(yFrame, featureDetector, nullptr, serializedFeatures);
}

bool CloudRelocalizer::extractAndSerializeFeatures(const Frame& yFrame, std::shared_ptr<UnifiedFeatures::FeatureDetector> featureDetector, std::shared_ptr<UnifiedFeatures::DescriptorGenerator> descriptorGenerator, std::vector<uint8_t>& serializedFeatures)
{
	if (featureDetector == nullptr)
	{
		ocean_assert(false && "Invalid feature detector");
		return false;
	}

	std::shared_ptr<UnifiedFeatures::FeatureContainer> featureContainer;
	if (featureDetector->detectFeatures(yFrame, featureContainer) != UnifiedFeatures::FeatureDetector::Result::SUCCESS)
	{
		return false;
	}

	if (descriptorGenerator == nullptr)
	{
		// no explicit descriptor generator was provided, we try to use the default generator which comes with the detector

		descriptorGenerator = dynamic_cast<UnifiedFeatures::OceanFreak::OFHarrisFreakFeatureDetector32*>(featureDetector.get())->getDescriptorGenerator(); // **TODO** avoid specialization once possible
		if (!descriptorGenerator)
		{
			return false;
		}
	}

	std::shared_ptr<UnifiedFeatures::DescriptorContainer> descriptorContainer;
	if (descriptorGenerator->generateDescriptors(yFrame, *featureContainer, descriptorContainer) != UnifiedFeatures::DescriptorGenerator::Result::SUCCESS)
	{
		return false;
	}

	UnifiedFeatures::OceanFreak::OFHarrisFeatureContainer& ofHarrisFeatureContainer = dynamic_cast<UnifiedFeatures::OceanFreak::OFHarrisFeatureContainer&>(*featureContainer); // **TODO** avoid specialization once possible
	UnifiedFeatures::OceanFreak::OFDescriptorContainer32& ofDescriptorContainer32 = dynamic_cast<UnifiedFeatures::OceanFreak::OFDescriptorContainer32&>(*descriptorContainer);

	arvr::thrift_if::relocalization::ImageFeatures imageFeatures = surreal::livemaps_service::toThrift(ofHarrisFeatureContainer.harrisCorners(), ofHarrisFeatureContainer.harrisCornerPyramidLevels(), ofDescriptorContainer32.freakDescriptors());

	const std::string bufferString = apache::thrift::CompactSerializer::serialize<std::string>(imageFeatures);

	serializedFeatures = std::vector<uint8_t>(bufferString.begin(), bufferString.end());

	return true;
}

} // namespace Cloud

} // namespace Tracking

} // namespace Ocean
