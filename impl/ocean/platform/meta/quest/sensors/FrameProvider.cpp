// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/platform/meta/quest/sensors/FrameProvider.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/HomogenousMatrix4.h"

#include "ocean/platform/android/Utilities.h"

#include <perception/sensor_calibration_io/DeviceInfo.h>
#include <perception/sensor_calibration_io/FormatInfo.h>
#include <perception/sensor_calibration_io/Metadata.h>
#include <perception/sensor_calibration_io/binary_serialization/CameraCalibrationImport.h>

#include <vros/sys/sensors/ExposureControlAlgorithm.h>

#include <folly/io/IOBuf.h>

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace Sensors
{

template <bool tAllowInvalidCameras>
FrameProviderT<tAllowInvalidCameras>::CustomFrameSetConsumer::CustomFrameSetConsumer(FrameProviderT& owner, const std::shared_ptr<OSSDK::Tracking::v8::IHeadTracker>& ossdkHeadTracker, const std::shared_ptr<OSSDK::Calibration::v4::ICalibrationManager>& ossdkCalibrationManager, OSSDK::Sensors::v3::FrameType cameraFrameType, const FrameCopyMode frameCopyMode, const CameraType cameraType) :
	owner_(owner),
	ossdkHeadTracker_(ossdkHeadTracker),
	ossdkCalibrationConsumer_(nullptr),
	cameraFrameType_(cameraFrameType),
	cameraType_(cameraType),
	frameCopyMode_(frameCopyMode),
	world_T_device_(false)
{
	if (ossdkCalibrationManager && (cameraFrameType_ == OSSDK::Sensors::v3::FrameType::Headset || cameraFrameType_ == OSSDK::Sensors::v3::FrameType::Hand))
	{
		ossdkCalibrationConsumer_ = ossdkCalibrationManager->getCalibrationConsumer(OSSDK::Calibration::v4::SensorGroup::Slam);

		if (ossdkCalibrationConsumer_ == nullptr)
		{
			Log::warning() << "Failed to access CalibrationConsumer for Headset cameras";

			// reason can be that the app is not whitelisted in the OS https://fburl.com/diffusion/9qllda94
		}
	}
}

template <bool tAllowInvalidCameras>
bool FrameProviderT<tAllowInvalidCameras>::CustomFrameSetConsumer::latestFrames(Frames& frames, SharedAnyCamerasD* cameras, HomogenousMatrixD4* world_T_device, HomogenousMatricesD4* device_T_cameras, CameraType* cameraType, FrameMetadatas* frameMetadatas)
{
	const ScopedLock scopedLock(ownedFramesLock_);

	if (latestOwningFrames_.empty())
	{
		return false;
	}

	ocean_assert(latestOwningFrames_.size() == latestFrameMetadatas_.size());

	ocean_assert(latestOwningFrames_.front().isOwner());
	frames = std::move(latestOwningFrames_);

	if (cameras != nullptr)
	{
		*cameras = std::move(latestCameras_);

		ocean_assert(cameras->size() == frames.size());
	}
	else
	{
		latestCameras_.clear();
	}

	if (world_T_device)
	{
		*world_T_device = world_T_device_;
	}
	world_T_device_.toNull();

	if (device_T_cameras)
	{
		*device_T_cameras = std::move(latest_device_T_cameras_);
		ocean_assert(device_T_cameras->size() == frames.size());
	}
	else
	{
		latest_device_T_cameras_.clear();
	}

	if (cameraType)
	{
		*cameraType = cameraType_;
	}

	if (frameMetadatas)
	{
		*frameMetadatas = latestFrameMetadatas_;
		ocean_assert(frameMetadatas->size() == frames.size());
	}

	return true;
}

template <bool tAllowInvalidCameras>
bool FrameProviderT<tAllowInvalidCameras>::CustomFrameSetConsumer::exposureSettingsRange(double& minExposure, double& maxExposure, double& minGain, double& maxGain) const
{
	const ScopedLock scopedLock(ownedFramesLock_);

	if (imageSensorConfigurations_.empty())
	{
		return false;
	}

	double minExposureLocal = NumericD::minValue();
	double maxExposureLocal = NumericD::maxValue();

	double minGainLocal = NumericD::minValue();
	double maxGainLocal = NumericD::maxValue();

	for (const OSSDK::Sensors::v4::ImageSensorConfiguration& imageSensorConfiguration : imageSensorConfigurations_)
	{
		minExposureLocal = std::max(minExposureLocal, imageSensorConfiguration.exposureDurationMin);
		maxExposureLocal = std::min(maxExposureLocal, imageSensorConfiguration.exposureDurationMax);

		minGainLocal = std::max(minGainLocal, imageSensorConfiguration.gainMin);
		maxGainLocal = std::min(maxGainLocal, imageSensorConfiguration.gainMax);
	}

	if (minExposureLocal == NumericD::minValue() || maxExposureLocal == NumericD::maxValue() || minGainLocal == NumericD::minValue() || maxGainLocal == NumericD::maxValue())
	{
		return false;
	}

	minExposure = minExposureLocal;
	maxExposure = maxExposureLocal;
	ocean_assert(minExposure <= maxExposure);

	minGain = minGainLocal;
	maxGain = maxGainLocal;
	ocean_assert(minGain <= maxGain);

	return true;
}

template <bool tAllowInvalidCameras>
bool FrameProviderT<tAllowInvalidCameras>::CustomFrameSetConsumer::currentExposureSettings(double& minExposure, double& meanExposure, double& maxExposure, double& minGain, double& meanGain, double& maxGain) const
{
	const ScopedLock scopedLock(ownedFramesLock_);

	minExposure = NumericD::maxValue();
	meanExposure = 0.0;
	maxExposure = NumericD::minValue();

	minGain = NumericD::maxValue();
	meanGain = 0.0;
	maxGain = NumericD::minValue();

	for (const FrameMetadata& frameMetadata : latestFrameMetadatas_)
	{
		minExposure = std::min(minExposure, frameMetadata.exposureDuration());
		meanExposure += frameMetadata.exposureDuration();
		maxExposure = std::max(maxExposure, frameMetadata.exposureDuration());

		minGain = std::min(minGain, frameMetadata.gain());
		meanGain += frameMetadata.gain();
		maxGain = std::max(maxGain, frameMetadata.gain());
	}

	if (minExposure == NumericD::maxValue() || minGain == NumericD::maxValue())
	{
		return false;
	}

	ocean_assert(latestFrameMetadatas_.size() != 0);
	meanExposure /= double(latestFrameMetadatas_.size());
	meanGain /= double(latestFrameMetadatas_.size());

	return true;
}

template <bool tAllowInvalidCameras>
void FrameProviderT<tAllowInvalidCameras>::CustomFrameSetConsumer::setNumCameras(uint32_t numCameras) {
	const ScopedLock scopedLock(ownedFramesLock_);
	imageSensorConfigurations_.resize(numCameras);
}

template <bool tAllowInvalidCameras>
void FrameProviderT<tAllowInvalidCameras>::CustomFrameSetConsumer::startConfigurationUpdate() {}

template <bool tAllowInvalidCameras>
void FrameProviderT<tAllowInvalidCameras>::CustomFrameSetConsumer::finishConfigurationUpdate() {}

template <bool tAllowInvalidCameras>
void FrameProviderT<tAllowInvalidCameras>::CustomFrameSetConsumer::setCameraConfiguration(uint32_t cameraIndex, const OSSDK::Sensors::v4::ImageSensorConfiguration& configuration)
{
	const ScopedLock scopedLock(ownedFramesLock_);

	if (cameraIndex >= imageSensorConfigurations_.size())
	{
		return;
	}
	imageSensorConfigurations_[cameraIndex] = configuration;

	typedef std::map<int, perception::sensor_calibration_io::CameraCalibration> CameraCalibrationMap;

	for (const bool doOnlineCalibration : {true, false})
	{
		SharedAnyCamerasD& cameras = doOnlineCalibration ? onlineCameras_ : factoryCameras_;
		HomogenousMatricesD4& device_T_cameras = doOnlineCalibration ? device_T_onlineCameras_ : device_T_factoryCameras_;

		SharedAnyCameraD camera;
		HomogenousMatrixD4 device_T_camera(false);

		const OSSDK::String& calibrationString = doOnlineCalibration ? configuration.onlineCalibration : configuration.factoryCalibration;

		if (!calibrationString.empty())
		{
			CameraCalibrationMap parsedCalibrations;
			if (perception::sensor_calibration_io::parseFromJson(calibrationString.c_str(), parsedCalibrations))
			{
				const CameraCalibrationMap::const_iterator iCalibration = parsedCalibrations.find(int(configuration.cameraId));
				ocean_assert(iCalibration != parsedCalibrations.cend());

				if (iCalibration != parsedCalibrations.cend())
				{
					camera = convertCameraProfile(iCalibration->second, device_T_camera);

					if (!camera)
					{
						ocean_assert(false && "This should never happen!");
					}
				}
			}
		}

		if (camera && camera->isValid() && device_T_camera.isValid())
		{
			cameras.emplace_back(std::move(camera));
			device_T_cameras.emplace_back(device_T_camera);
		}
		else if constexpr (tAllowInvalidCameras)
		{
			std::string cameraDescription;

			if (!configuration.description.empty())
			{
				cameraDescription = std::string(configuration.description.c_str());
			}
			else
			{
				cameraDescription = "UNKNOWN (camera ID: " + String::toAString(configuration.cameraId) + ")";
			}

			std::string reason = "The SDAU interface is not maintained for: " + cameraDescription + ". Please request support in this Workplace group: https://fb.workplace.com/groups/314176693283482";

			Log::warning() << "Invalid camera: " << reason;

			camera = std::make_shared<AnyCameraInvalidD>(InvalidCameraD(reason));

			cameras.emplace_back(std::move(camera));
			device_T_cameras.emplace_back(device_T_camera);
		}
		else
		{
			Log::error() << "Failed to parse calibration for camera " << configuration.cameraId;
		}
	}

	ocean_assert(!onlineCameras_.empty() || !factoryCameras_.empty());

	ocean_assert(onlineCameras_.size() == device_T_onlineCameras_.size());
	ocean_assert(factoryCameras_.size() == device_T_factoryCameras_.size());
}

template <bool tAllowInvalidCameras>
void FrameProviderT<tAllowInvalidCameras>::CustomFrameSetConsumer::beforeFirstFrameSet() {}

template <bool tAllowInvalidCameras>
void FrameProviderT<tAllowInvalidCameras>::CustomFrameSetConsumer::afterLastFrameSet() {}

template <bool tAllowInvalidCameras>
void FrameProviderT<tAllowInvalidCameras>::CustomFrameSetConsumer::onFrameSet(OSSDK::ArrayView<const OSSDK::Sensors::v4::ImageData> images)
{
	if (ossdkCalibrationConsumer_ != nullptr)
	{
		std::vector<perception::sensor_calibration_io::CameraCalibration> cameraCalibrations;

		if (readLatestCameraCalibration(ossdkCalibrationConsumer_, cameraCalibrations))
		{
			SharedAnyCamerasD cameras(cameraCalibrations.size());
			HomogenousMatricesD4 device_T_cameras(cameraCalibrations.size());

			bool succeeded = cameraCalibrations.size() == images.size();

			if (succeeded)
			{
				for (size_t n = 0; n < cameraCalibrations.size(); ++n)
				{
					cameras[n] = FrameProviderT<tAllowInvalidCameras>::convertCameraProfile(cameraCalibrations[n], device_T_cameras[n]);

					if (!cameras[n])
					{
						succeeded = false;
						break;
					}
				}
			}

			if (succeeded)
			{
				onlineCameras_ = std::move(cameras);
				device_T_onlineCameras_ = std::move(device_T_cameras);
			}
			else
			{
				Log::warning() << "Failed to read latest camera calibration";
			}
		}
	}

	TemporaryScopedLock temporaryScopedLock(ownedFramesLock_);

	if (cameraIndexSetForCameraType_.empty())
	{
		cameraIndexSetForCameraType_ = determineCameraIndices(cameraType_, device_T_onlineCameras_.empty() ? device_T_factoryCameras_ : device_T_onlineCameras_);
	}

	const CameraIndexSet cameraIndexSetForCameraType(cameraIndexSetForCameraType_);

	SharedAnyCamerasD cameras;
	cameras.reserve(cameraIndexSetForCameraType.size());

	HomogenousMatricesD4 device_T_cameras;
	device_T_cameras.reserve(cameraIndexSetForCameraType.size());

	for (size_t n = 0; n < images.size(); ++n)
	{
		if (cameraIndexSetForCameraType.find(n) == cameraIndexSetForCameraType.cend())
		{
			continue;
		}

		if (onlineCameras_.empty())
		{
			ocean_assert(n < factoryCameras_.size());
			cameras.emplace_back(factoryCameras_[n]);
		}
		else
		{
			ocean_assert(n < onlineCameras_.size());
			cameras.emplace_back(onlineCameras_[n]);
		}

		if (device_T_onlineCameras_.empty())
		{
			ocean_assert(n < device_T_factoryCameras_.size());
			device_T_cameras.emplace_back(device_T_factoryCameras_[n]);
		}
		else
		{
			ocean_assert(n < device_T_onlineCameras_.size());
			device_T_cameras.emplace_back(device_T_onlineCameras_[n]);
		}
	}

	const Frame::CopyMode frameCopyMode = frameCopyMode_ == FCM_USE_IMMEDIATELY ? Frame::CM_USE_KEEP_LAYOUT : Frame::CM_COPY_REMOVE_PADDING_LAYOUT;

	temporaryScopedLock.release();

	HomogenousMatrixD4 world_T_device(false);

	Frames newFrames;
	newFrames.reserve(cameraIndexSetForCameraType.size());

	FrameMetadatas newFrameMetadatas;
	newFrameMetadatas.reserve(images.size());

	Frame::PlaneInitializers<uint8_t> planeInitializers;

	for (size_t n = 0; n < images.size(); ++n)
	{
		if (cameraIndexSetForCameraType.find(n) == cameraIndexSetForCameraType.cend())
		{
			continue;
		}

		const OSSDK::Sensors::v4::ImageData& image = images.at(n);

		// Pointer to the actual image data. The lifetime of this data does not extend beyond the callback return, i.e. the consumer is required to make a copy
		// It is possible for @data to be nullptr - this may happen when an incomplete frame group gets emitted by the lower layers.
		if (image.data == nullptr)
		{
			// we will handle the current set of frames
			return;
		}

		const FrameType::PixelFormat pixelFormat = translateImageFormat(image.imageFormat);

		if (pixelFormat == FrameType::FORMAT_UNDEFINED)
		{
			continue;
		}

		const unsigned int numberPlanes = FrameType::numberPlanes(pixelFormat);

		if (numberPlanes >= 3u)
		{
			ocean_assert(false && "This should never happen!");
			continue;
		}

		const FrameType frameType(image.imageFormat.width(), image.imageFormat.height(), pixelFormat, FrameType::ORIGIN_UPPER_LEFT);

		planeInitializers.clear();

		const uint8_t* data = (const uint8_t*)(image.data);

		for (unsigned int planeIndex = 0u; planeIndex < numberPlanes; ++planeIndex)
		{
			const unsigned int planeStrideBytes = planeIndex == 0u ? image.imageFormat.stride() : image.imageFormat.plane23Stride();
			const unsigned int planeOffsetRows = planeIndex == 0u ? image.imageFormat.plane2OffsetRows() : image.imageFormat.plane3OffsetRows();

			unsigned int planePaddingElements = 0u;
			if (Frame::strideBytes2paddingElements(pixelFormat, image.imageFormat.width(), planeStrideBytes, planePaddingElements, planeIndex))
			{
				planeInitializers.emplace_back(data, frameCopyMode, planePaddingElements);

				data += planeOffsetRows * planeStrideBytes;
			}
			else
			{
				ocean_assert(false && "This should never happen!");
				continue;
			}
		}

		const Timestamp timestamp(image.captureTimestampInProcessingClockDomain.asSeconds());

		newFrames.emplace_back(frameType, planeInitializers, timestamp);

		newFrameMetadatas.emplace_back(image.exposureDuration, image.gain, image.readoutDurationSeconds);

		if (world_T_device.isNull() && ossdkHeadTracker_)
		{
			OSSDK::Optional<OSSDK::Tracking::v5::RigidBodyTrackingDataMapFromDevice> headTrackerState = ossdkHeadTracker_->getState(image.captureTimestampInProcessingClockDomain);

			if (headTrackerState.hasValue() && headTrackerState.value().valid)
			{
				const OSSDK::Transform& pose = headTrackerState.value().referenceFromBody.pose;

				const QuaternionD rotation(pose.rotation.w, pose.rotation.x, pose.rotation.y, pose.rotation.z);
				ocean_assert(rotation.isValid());

				const VectorD3 translation(pose.translation.x, pose.translation.y, pose.translation.z);

				world_T_device = HomogenousMatrixD4(translation, rotation);
			}
		}
	}

#ifdef OCEAN_DEBUG
	{
		const ScopedLock scopedLock(ownedFramesLock_);
		ocean_assert(cameraIndexSetForCameraType_.size() <= imageSensorConfigurations_.size());
		ocean_assert(cameraIndexSetForCameraType_.size() == newFrames.size());
		ocean_assert(cameraIndexSetForCameraType_.size() == cameras.size());
		ocean_assert(cameraIndexSetForCameraType_.size() == device_T_cameras.size());
	}
#endif

	if (!newFrames.empty())
	{
		if (!world_T_device.isValid())
		{
			Log::warning() << "Failed to determine headset pose for timestamp " << double(newFrames.front().timestamp());
		}

#ifdef OCEAN_DEBUG
		ocean_assert(newFrames.size() == cameras.size());
		for (size_t n = 0; n < newFrames.size(); ++n)
		{
			if constexpr (tAllowInvalidCameras)
			{
				ocean_assert(!cameras[n]->isValid() || newFrames[n].width() == cameras[n]->width());
				ocean_assert(!cameras[n]->isValid() || newFrames[n].height() == cameras[n]->height());
			}
			else
			{
				ocean_assert(newFrames[n].width() == cameras[n]->width());
				ocean_assert(newFrames[n].height() == cameras[n]->height());
			}
		}
#endif

		owner_.onFrames(cameraFrameType_, cameraType_, newFrames, cameras, world_T_device, device_T_cameras, newFrameMetadatas);

		const ScopedLock scopedLock(ownedFramesLock_);

		if (newFrames.front().isOwner())
		{
			latestOwningFrames_ = std::move(newFrames);
			latestCameras_ = std::move(cameras);
			latest_device_T_cameras_ = std::move(device_T_cameras);

			world_T_device_ = world_T_device;
		}

		latestFrameMetadatas_ = std::move(newFrameMetadatas);
	}
}

template <bool tAllowInvalidCameras>
typename FrameProviderT<tAllowInvalidCameras>::CustomFrameSetConsumer::CameraIndexSet FrameProviderT<tAllowInvalidCameras>::CustomFrameSetConsumer::determineCameraIndices(const CameraType cameraType, const HomogenousMatricesD4& device_T_cameras)
{
	ocean_assert(!device_T_cameras.empty());

	CameraIndexSet cameraIndexSet;

	if (cameraType == CT_ALL_CAMERAS || device_T_cameras.size() <= 2)
	{
		for (size_t n = 0; n < device_T_cameras.size(); ++n)
		{
			cameraIndexSet.emplace(n);
		}
	}
	else
	{
		// we sort the cameras wrt their y-axis

		std::multimap<Scalar, size_t> cameraHeightMap;

		for (size_t n = 0; n < device_T_cameras.size(); ++n)
		{
			const Scalar yTranslation = device_T_cameras[n].translation().y();

			cameraHeightMap.insert(std::make_pair(yTranslation, n));
		}

		ocean_assert(cameraHeightMap.size() == device_T_cameras.size());

		switch (cameraType)
		{
			case CT_ALL_CAMERAS:
			case CT_END:
				ocean_assert(false && "Should never happen!");
				break;

			case CT_LOWER_STEREO_CAMERAS:
				{
					std::multimap<Scalar, size_t>::iterator i = cameraHeightMap.begin();

					cameraIndexSet.emplace(i->second);
					cameraIndexSet.emplace((++i)->second);
					break;
				}

			case CT_UPPER_STEREO_CAMERAS:
				{
					std::multimap<Scalar, size_t>::reverse_iterator i = cameraHeightMap.rbegin();

					cameraIndexSet.emplace(i->second);
					cameraIndexSet.emplace((++i)->second);
					break;
				}
		}
	}

	ocean_assert(!cameraIndexSet.empty());

	return cameraIndexSet;
}

template <bool tAllowInvalidCameras>
FrameProviderT<tAllowInvalidCameras>::~FrameProviderT()
{
	release();
}

template <bool tAllowInvalidCameras>
bool FrameProviderT<tAllowInvalidCameras>::initialize(const FrameCopyMode frameCopyMode)
{
	const ScopedLock scopedLock(lock_);

	if (frameCopyMode == FCM_INVALID)
	{
		return false;
	}

	if (ossdkSensorDataProvider_)
	{
		return false;
	}

	if (ossdkDispatchThreadHandle_ || ossdkCameraDataProvider_)
	{
		return false;
	}

	ocean_assert(!ossdkHeadTracker_ && !ossdkCalibrationManager_);

	ossdkSensorDataProvider_ = OSSDK::Sensors::v3::createSensorDataProvider();
	ossdkCameraDataProvider_ = sensoraccess::CameraDataProvider::create();
	ossdkHeadTracker_ = OSSDK::Tracking::v8::createHeadTracker();

	if (!ossdkHeadTracker_)
	{
		Log::error() << "Failed to create head tracker";
	}

	ossdkCalibrationManager_ = OSSDK::Calibration::v4::createCalibrationManager();

	if (!ossdkCalibrationManager_)
	{
		Log::error() << "Failed to create calibration manager";
	}

	{
		auto dispatchThreadFactory = sensoraccess::DispatchThreadFactory::create();
		if (dispatchThreadFactory)
		{
			auto createResult = dispatchThreadFactory->createDispatchThread("dispatch_thread", "");
			if (!createResult.success())
			{
				Log::error() << "Failed to create dispatch thread";
			}
			else
			{
				ossdkDispatchThreadHandle_ = std::move(createResult.returnValue);
			}
		}
		else
		{
			ossdkCameraDataProvider_ = nullptr;
		}
	}

	if (ossdkCameraDataProvider_ != nullptr)
	{
		ossdkAvailableCameraStreamPurposes_ = ossdkCameraDataProvider_->getAvailablePurposes();

#ifdef OCEAN_DEBUG
		Log::info() << "FrameProviderT has access to " << ossdkAvailableCameraStreamPurposes_.size() << " different camera streams:";
		for (sensoraccess::CameraStreamPurpose purpose : ossdkAvailableCameraStreamPurposes_)
		{
			Log::info() << purpose.cameraSystem.toStdString() << "/" << purpose.purpose.toStdString();
		}
#endif

		if (ossdkAvailableCameraStreamPurposes_.empty())
		{
			Log::error() << "Failed to initialize camera data provider";

			std::string value;
			if (Platform::Android::Utilities::systemPropertyValue("persist.ovr.enable.sensorproxy", value) && value == "true")
			{
				Log::error() << "The camera sensors need a userdev OS and the app must have granted permission for sensor data";
			}
			else
			{
				Log::error() << "Ensure that the OS is a userdebug build and that persist.ovr.enable.sensorproxy is set to 'true', see also https://fburl.com/access_cameras";
			}
		}

		// Fall back to the legacy sensor data provider
	}

	if (ossdkSensorDataProvider_ != nullptr)
	{
		OSSDK::Optional<OSSDK::Array<OSSDK::Sensors::v3::FrameType>> frameTypes = ossdkSensorDataProvider_->getAvailableFrameTypes();

		if (frameTypes.hasValue())
		{
			const size_t size = frameTypes.value().size();

			for (size_t n = 0; n < size; ++n)
			{
				ossdkAvailableCameraFrameTypes_.emplace(frameTypes.value().at(n));
			}
		}

#ifdef OCEAN_DEBUG
		Log::info() << "FrameProviderT has access to " << ossdkAvailableCameraFrameTypes_.size() << " different camera frame types:";
		for (const OSSDK::Sensors::v3::FrameType& frameType : ossdkAvailableCameraFrameTypes_)
		{
			Log::info() << OSSDK::Sensors::v3::frameTypeToString(frameType);
		}
#endif

		if (ossdkAvailableCameraFrameTypes_.empty())
		{
			Log::error() << "Failed to initialize camera sensor provider";

			std::string value;
			if (Platform::Android::Utilities::systemPropertyValue("persist.ovr.enable.sensorproxy", value) && value == "true")
			{
				Log::error() << "The camera sensors need a userdev OS and the app must have granted permission for sensor data";
			}
			else
			{
				Log::error() << "Ensure that the OS is a userdebug build and that persist.ovr.enable.sensorproxy is set to 'true', see also https://fburl.com/access_cameras";
			}

			return false;
		}

		frameCopyMode_ = frameCopyMode;

		return true;
	}
	else
	{
		Log::error() << "Failed to create sensor data provider";
	}

	return false;
}

template <bool tAllowInvalidCameras>
bool FrameProviderT<tAllowInvalidCameras>::startReceivingCameraFrames(const OSSDK::Sensors::v3::FrameType& cameraFrameType, const CameraType cameraType, const bool useRealtimeCalibration)
{
	ocean_assert(cameraType < CT_END);

	const ScopedLock scopedLock(lock_);

	if (!isValid())
	{
		ocean_assert(false && "Invalid frame provider!");
		return false;
	}

	typename FrameSetConsumerMap::iterator i = frameSetConsumerMap_.find(cameraFrameType);
	if (i != frameSetConsumerMap_.cend())
	{
		stopReceivingCameraFrames(cameraFrameType);
	}

	ocean_assert(frameSetConsumerMap_.find(cameraFrameType) == frameSetConsumerMap_.cend());

	std::shared_ptr<CustomFrameSetConsumer> customFrameSetConsumer = std::make_shared<CustomFrameSetConsumer>(*this, ossdkHeadTracker_, useRealtimeCalibration ? ossdkCalibrationManager_ : nullptr, cameraFrameType, frameCopyMode_, cameraType);
	std::shared_ptr<FrameSetConsumerAdapter> frameSetConsumerAdapter = std::make_shared<FrameSetConsumerAdapter>(customFrameSetConsumer.get());

	if (ossdkSensorDataProvider_->registerFrameSetConsumer(*frameSetConsumerAdapter.get()->get(), cameraFrameType))
	{
		frameSetConsumerMap_.insert(std::make_pair(cameraFrameType, std::move(customFrameSetConsumer)));
		frameSetConsumerAdapterStorage_.push_back(std::move(frameSetConsumerAdapter));
	}
	else
	{
		Log::error() << "Failed to register frame set consumer <" << OSSDK::Sensors::v3::frameTypeToString(cameraFrameType) << ">";
		return false;
	}

	return true;
}

template <bool tAllowInvalidCameras>
bool FrameProviderT<tAllowInvalidCameras>::stopReceivingCameraFrames(const OSSDK::Sensors::v3::FrameType& cameraFrameType)
{
	const ScopedLock scopedLock(lock_);

	typename FrameSetConsumerMap::iterator i = frameSetConsumerMap_.find(cameraFrameType);
	if (i == frameSetConsumerMap_.cend())
	{
		ocean_assert(false && "Invalid camera frame type!");
		return false;
	}

	if (!ossdkSensorDataProvider_->unregisterFrameSetConsumer(i->first))
	{
		return false;
	}

	frameSetConsumerMap_.erase(i);

	return true;
}

template <bool tAllowInvalidCameras>
bool FrameProviderT<tAllowInvalidCameras>::stopReceivingCameraFrames()
{
	const ScopedLock scopedLock(lock_);

	bool allSucceeded = true;

	for (typename FrameSetConsumerMap::iterator i = frameSetConsumerMap_.begin(); i != frameSetConsumerMap_.end(); ++i)
	{
		if (!ossdkSensorDataProvider_->unregisterFrameSetConsumer(i->first))
		{
			allSucceeded = false;
		}
	}

	frameSetConsumerMap_.clear();

	return allSucceeded;
}

template <bool tAllowInvalidCameras>
bool FrameProviderT<tAllowInvalidCameras>::overwriteExposureSettings(const OSSDK::Sensors::v3::FrameType& cameraFrameType, const double exposure, const double gain)
{
	if (cameraFrameType == OSSDK::Sensors::v3::FrameType::Invalid)
	{
		ocean_assert(false && "Invalid camera frame type!");
		return false;
	}

	const ScopedLock scopedLock(lock_);

	typename FrameSetConsumerMap::iterator i = frameSetConsumerMap_.find(cameraFrameType);
	if (i == frameSetConsumerMap_.cend())
	{
		ocean_assert(false && "Invalid camera frame type!");
		return false;
	}

	if (exposure > 0.0 && gain >= 0.0)
	{
		double minExposure, maxExposure;
		double minGain, maxGain;
		if (i->second->exposureSettingsRange(minExposure, maxExposure, minGain, maxGain))
		{
			if (exposure < minExposure || exposure > maxExposure)
			{
				Log::error() << "Exposure settings outside of range: " << exposure << " [" << minExposure << ", " << maxExposure << "]";
				return false;
			}

#if 0 // temporary disabled until min/max gain contain correct values
			if (gain < minGain || gain > maxGain)
			{
				Log::error() << "Gain settings outside of range: " << gain << " [" << minGain << ", " << maxGain << "]";
				return false;
			}
#endif
		}

		const size_t numberOfCameras = i->second->maximalFrames();
		const std::vector<OSSDK::Sensors::v3::ExposureInfo> exposureInfos(numberOfCameras, OSSDK::Sensors::v3::ExposureInfo(exposure, gain));

		return sensorSettingsManager_.setExposureSettings(cameraFrameType, OSSDK::ArrayView<const OSSDK::Sensors::v3::ExposureInfo>(exposureInfos));
	}
	else
	{
		return sensorSettingsManager_.resetExposureSettings(cameraFrameType);
	}
}

template <bool tAllowInvalidCameras>
bool FrameProviderT<tAllowInvalidCameras>::overwriteExposureSettings(const double exposure, const double gain)
{
	const ScopedLock scopedLock(lock_);

	bool result = true;

	for (typename FrameSetConsumerMap::const_iterator i = frameSetConsumerMap_.cbegin(); i != frameSetConsumerMap_.cend(); ++i)
	{
		if (exposure > 0.0 && gain >= 0.0)
		{
			const size_t numberOfCameras = i->second->maximalFrames();
			const std::vector<OSSDK::Sensors::v3::ExposureInfo> exposureInfos(numberOfCameras, OSSDK::Sensors::v3::ExposureInfo(exposure, gain));

			result = sensorSettingsManager_.setExposureSettings(i->first, OSSDK::ArrayView<const OSSDK::Sensors::v3::ExposureInfo>(exposureInfos)) && result;
		}
		else
		{
			result = sensorSettingsManager_.resetExposureSettings(i->first) && result;
		}
	}

	return result;
}

template <bool tAllowInvalidCameras>
bool FrameProviderT<tAllowInvalidCameras>::exposureSettingsRange(const OSSDK::Sensors::v3::FrameType& cameraFrameType, double& minExposure, double& maxExposure, double& minGain, double& maxGain) const
{
	const ScopedLock scopedLock(lock_);

	typename FrameSetConsumerMap::const_iterator i = frameSetConsumerMap_.find(cameraFrameType);
	if (i == frameSetConsumerMap_.cend())
	{
		return false;
	}

	return i->second->exposureSettingsRange(minExposure, maxExposure, minGain, maxGain);
}

template <bool tAllowInvalidCameras>
bool FrameProviderT<tAllowInvalidCameras>::currentExposureSettings(const OSSDK::Sensors::v3::FrameType& cameraFrameType, double* minExposure, double* meanExposure, double* maxExposure, double* minGain, double* meanGain, double* maxGain) const
{
	const ScopedLock scopedLock(lock_);

	typename FrameSetConsumerMap::const_iterator i = frameSetConsumerMap_.find(cameraFrameType);
	if (i == frameSetConsumerMap_.cend())
	{
		return false;
	}

	double internalMinExposure;
	double internalMeanExposure;
	double internalMaxExposure;

	double internalMinGain;
	double internalMeanGain;
	double internalMaxGain;

	if (!i->second->currentExposureSettings(internalMinExposure, internalMeanExposure, internalMaxExposure, internalMinGain, internalMeanGain, internalMaxGain))
	{
		return false;
	}

	if (minExposure)
	{
		*minExposure = internalMinExposure;
	}

	if (meanExposure)
	{
		*meanExposure = internalMeanExposure;
	}

	if (maxExposure)
	{
		*maxExposure = internalMaxExposure;
	}

	if (minGain)
	{
		*minGain = internalMinGain;
	}

	if (meanGain)
	{
		*meanGain = internalMeanGain;
	}

	if (maxGain)
	{
		*maxGain = internalMaxGain;
	}

	return true;
}

template <bool tAllowInvalidCameras>
bool FrameProviderT<tAllowInvalidCameras>::latestFrames(Frames& frames, SharedAnyCamerasD* cameras, HomogenousMatrixD4* world_T_device, HomogenousMatricesD4* device_T_cameras, OSSDK::Sensors::v3::FrameType* cameraFrameType, CameraType* cameraType, FrameMetadatas* frameMetadatas, const OSSDK::Sensors::v3::FrameType requestCameraFrameType)
{
	TemporaryScopedLock scopedLock(lock_);

	if (!ossdkSensorDataProvider_ || frameSetConsumerMap_.empty())
	{
		return false;
	}

	if (requestCameraFrameType == OSSDK::Sensors::v3::FrameType::Invalid)
	{
		if (frameSetConsumerMap_.size() == 1)
		{
			const std::shared_ptr<CustomFrameSetConsumer> frameConsumer(frameSetConsumerMap_.begin()->second);

			scopedLock.release();

			ocean_assert(frameConsumer);
			if (frameConsumer->latestFrames(frames, cameras, world_T_device, device_T_cameras, cameraType, frameMetadatas))
			{
				if (cameraFrameType != nullptr)
				{
					*cameraFrameType = frameSetConsumerMap_.begin()->second->cameraFrameType();
				}

				return true;
			}

			return false;
		}

		// we apply a round robin approach to extract the frames from the next frame set provider

		FrameSetConsumers frameSetConsumers;
		frameSetConsumers.reserve(frameSetConsumerMap_.size());

		for (typename FrameSetConsumerMap::const_iterator i = frameSetConsumerMap_.cbegin(); i != frameSetConsumerMap_.cend(); ++i)
		{
			frameSetConsumers.emplace_back(i->second);
		}

		scopedLock.release();

		for (size_t n = 0; n < frameSetConsumers.size(); ++n)
		{
			const size_t index = (n + nextRoundRobinCameraFrameSetIndex_) % frameSetConsumers.size();

			if (frameSetConsumers[index]->latestFrames(frames, cameras, world_T_device, device_T_cameras, cameraType, frameMetadatas))
			{
				if (cameraFrameType != nullptr)
				{
					*cameraFrameType = frameSetConsumers[index]->cameraFrameType();
				}

				nextRoundRobinCameraFrameSetIndex_ = (n + 1) % frameSetConsumers.size();

				return true;
			}
		}
	}
	else
	{
		const typename FrameSetConsumerMap::iterator i = frameSetConsumerMap_.find(requestCameraFrameType);
		if (i == frameSetConsumerMap_.cend())
		{
			Log::warning() << "The frame provider does not provide frames with the camera type '" << translateCameraFrameType(requestCameraFrameType) << "'";
			return false;
		}

		const std::shared_ptr<CustomFrameSetConsumer> frameConsumer(i->second);

		scopedLock.release();

		if (frameConsumer->latestFrames(frames, cameras, world_T_device, device_T_cameras, cameraType, frameMetadatas))
		{
			if (cameraFrameType != nullptr)
			{
				*cameraFrameType = requestCameraFrameType;
			}

			return true;
		}
	}

	return false;
}

template <bool tAllowInvalidCameras>
typename FrameProviderT<tAllowInvalidCameras>::CameraFrameTypes FrameProviderT<tAllowInvalidCameras>::activeCameraFrameTypes() const
{
	const ScopedLock scopedLock(lock_);

	CameraFrameTypes frameTypes;
	frameTypes.reserve(frameSetConsumerMap_.size());

	for (typename FrameSetConsumerMap::const_iterator i = frameSetConsumerMap_.cbegin(); i != frameSetConsumerMap_.cend(); ++i)
	{
		frameTypes.insert(i->first);
	}

	return frameTypes;
}

template <bool tAllowInvalidCameras>
void FrameProviderT<tAllowInvalidCameras>::release()
{
	const ScopedLock scopedLock(lock_);

	ossdkCalibrationManager_ = nullptr;

	sensorSettingsManager_.release();

	stopReceivingCameraFrames();

	ossdkHeadTracker_ = nullptr;
	ossdkSensorDataProvider_ = nullptr;

	frameSetConsumerMap_.clear();
	ossdkAvailableCameraFrameTypes_.clear();
}

template <bool tAllowInvalidCameras>
std::string FrameProviderT<tAllowInvalidCameras>::translateCameraType(const CameraType cameraType)
{
	switch (cameraType)
	{
		case CT_ALL_CAMERAS:
			return std::string("ALL_CAMERAS");

		case CT_LOWER_STEREO_CAMERAS:
			return std::string("LOWER_STEREO_CAMERAS");

		case CT_UPPER_STEREO_CAMERAS:
			return std::string("UPPER_STEREO_CAMERAS");

		case CT_END:
			break;
	}

	ocean_assert(false && "Invalid camera type!");
	return std::string("Invalid");
}

template <bool tAllowInvalidCameras>
void FrameProviderT<tAllowInvalidCameras>::onFrames(const OSSDK::Sensors::v3::FrameType& /*cameraFrameType*/, const CameraType& /*cameraType*/, const Frames& /*frames*/, const SharedAnyCamerasD& /*cameras*/, const HomogenousMatrixD4& /*world_T_device*/, const HomogenousMatricesD4& /*device_T_cameras*/, const FrameMetadatas& /*frameMetadatas*/)
{
	// can be implemented in a derived class
}

template <bool tAllowInvalidCameras>
FrameType::PixelFormat FrameProviderT<tAllowInvalidCameras>::translateImageFormat(const OSSDK::Sensors::v4::ImageFormat& ossdkImageFormat)
{
	switch (ossdkImageFormat.pixelFormat())
	{
		case OSSDK::Sensors::v3::PixelFormat::Invalid:
			return FrameType::FORMAT_UNDEFINED;

		case OSSDK::Sensors::v3::PixelFormat::MONO_8: // 1 channel with 8 bit integers. 1 byte per pixel.
			return FrameType::FORMAT_Y8;

		case OSSDK::Sensors::v3::PixelFormat::MONO_10: // 1 channel with 10 bit integers (uint16_t with 6 msb unused). 2 bytes per pixel.
			return FrameType::FORMAT_Y10;

		case OSSDK::Sensors::v3::PixelFormat::YUV_I420_Planar:
			return FrameType::FORMAT_Y_UV12_LIMITED_RANGE; // NOTE: it's not FORMAT_Y_U_V12

		case OSSDK::Sensors::v3::PixelFormat::YUV_420:
		{
			// can be FORMAT_Y_UV12_LIMITED_RANGE or FORMAT_Y_U_V12_LIMITED_RANGE depending on the plane offset parameter
			if (ossdkImageFormat.plane3OffsetRows() == 0u)
			{
				return FrameType::FORMAT_Y_UV12_LIMITED_RANGE;
			}

			return FrameType::FORMAT_Y_U_V12_LIMITED_RANGE;
		}

		case OSSDK::Sensors::v3::PixelFormat::YVU_420:
		{
			// can be FORMAT_Y_VU12_LIMITED_RANGE or FORMAT_Y_V_U12_LIMITED_RANGE depending on the plane offset parameter
			if (ossdkImageFormat.plane3OffsetRows() == 0u)
			{
				return FrameType::FORMAT_Y_VU12_LIMITED_RANGE;
			}

			return FrameType::FORMAT_Y_V_U12_LIMITED_RANGE;
		}

		case OSSDK::Sensors::v3::PixelFormat::MONO_32F: // 1 channel with 32 bit floating point. 4 bytes per pixel.
			return FrameType::FORMAT_F32;

		case OSSDK::Sensors::v3::PixelFormat::RGB_8: // 3 channels interleaved rgb data, with 8 bit integers each (RGBRGB...). 3 bytes per
			return FrameType::FORMAT_RGB24;

		case OSSDK::Sensors::v3::PixelFormat::MONO_16: // 1 channel with 16 bit integers. 2 bytes per pixel.
			return FrameType::FORMAT_Y16;

		case OSSDK::Sensors::v3::PixelFormat::RAW_10: // 1 channel, 4 pixels packed into 5 bytes. see
			return FrameType::FORMAT_Y10_PACKED;

		case OSSDK::Sensors::v3::PixelFormat::RAW10_BAYER_RGGB: // 1 channel, 10 bit raw bayer pattern with RGGB format.
			return FrameType::FORMAT_RGGB10_PACKED;

		case OSSDK::Sensors::v3::PixelFormat::BAYER_8_RGGB: // 1 channel, 8 bit raw bayer pattern with RGGB format. The pattern is arranged as follows:  [[ R, G, R, G .. ], [ G, B, G, B .. ], [ R, G, R, G .. ], ...]
		case OSSDK::Sensors::v3::PixelFormat::JPEG: // JPEG-compressed image stream. Image details are encoded in the JPEG header.
		case OSSDK::Sensors::v3::PixelFormat::MONO_12: // 1 channel with 12 bit integers (uint16_t with 4 msb unused). 2 bytes per pixel.
		case OSSDK::Sensors::v3::PixelFormat::RGB_10: // 3 channels interleaved rgb data with 10 bit integers each (uint16_t with 6 msb unused), (RGBRGB...). 6 bytes per pixel.
		case OSSDK::Sensors::v3::PixelFormat::RGB_12: // 3 channels interleaved rgb data with 12 bit integers each (uint16_t with 4 msb unused), (RGBRGB...). 6 bytes per pixel.
			ocean_assert(false && "Undefined pixel format!");
			return FrameType::FORMAT_UNDEFINED;

		default:
		case OSSDK::Sensors::v3::PixelFormat::Count:
			break;
	}

	ocean_assert(false && "Undefined pixel format!");
	return FrameType::FORMAT_UNDEFINED;
}

template <bool tAllowInvalidCameras>
bool FrameProviderT<tAllowInvalidCameras>::readLatestCameraCalibration(OSSDK::Calibration::v4::ICalibrationConsumer* ossdkCalibrationConsumer, std::vector<perception::sensor_calibration_io::CameraCalibration>& cameraCalibrations)
{
	ocean_assert(ossdkCalibrationConsumer != nullptr);

	cameraCalibrations.clear();

	const OSSDK::Calibration::v4::Result<bool> newDataAvailable = ossdkCalibrationConsumer->isNewDataAvailable();

	if (newDataAvailable.status != OSSDK::Calibration::v4::Status::Ok)
	{
		Log::warning() << "Calibration consumer isNewDataAvailable() status: " << int(newDataAvailable.status);
		return false;
	}

	if (!newDataAvailable.value)
	{
		return false;
	}

	OSSDK::Calibration::v4::Result<OSSDK::Array<uint8_t>> data = ossdkCalibrationConsumer->readData();

	if (data.status != OSSDK::Calibration::v4::Status::Ok)
	{
		Log::warning() << "Calibration consumer readData() status: " << int(newDataAvailable.status);
		return false;
	}

	std::unique_ptr<folly::IOBuf> follyBuffer = folly::IOBuf::wrapBuffer(data.value.data(), data.value.size());

	perception::sensor_calibration_io::FormatInfo formatInfo;
	perception::sensor_calibration_io::DeviceInfo deviceInfo;
	perception::sensor_calibration_io::Metadata metadata;

	if (!perception::sensor_calibration_io::parseFromBinary(*follyBuffer, formatInfo, deviceInfo, metadata, cameraCalibrations))
	{
		Log::error() << "Failed to parse binary camera calibration";
		return false;
	}

	return !cameraCalibrations.empty();
}

template <bool tAllowInvalidCameras>
SharedAnyCameraD FrameProviderT<tAllowInvalidCameras>::convertCameraProfile(const perception::sensor_calibration_io::CameraCalibration& calibration, HomogenousMatrixD4& device_T_camera)
{
	if (calibration.width <= 0 || calibration.height <= 0)
	{
		return nullptr;
	}

	const VectorD3 translation(calibration.deviceFromCameraTranslation);
	const SquareMatrixD3 rotationF((const double*)(calibration.deviceFromCameraRotation), /* rowAligned = */ true);
	ocean_assert(rotationF.isOrthonormal());

	// the coordinate system of the camera is flipped (the camera is looking towards the negative z-space), this is the flipped coordinate system in Ocean
	// therefore, we need to rotate the coordinate system around the x-axis to determine Ocean's default coordinate system, a camera looking towards the negative z-space
	const SquareMatrixD3 rotation(rotationF * SquareMatrixD3(1, 0, 0, 0, -1, 0, 0, 0, -1));

	// transformation transforming points defined in the coordinate system of the camera to points defined in the coordinate system of the device
	device_T_camera = HomogenousMatrixD4(translation, rotation);

	const unsigned int width = (unsigned int)(calibration.width);
	const unsigned int height = (unsigned int)(calibration.height);

	if (calibration.projectionCoefficients.size() != 3 || calibration.distortionCoefficients.size() != 8)
	{
		return nullptr;
	}

	const double& focal = calibration.projectionCoefficients[0];
	const double& principalX = calibration.projectionCoefficients[1];
	const double& principalY = calibration.projectionCoefficients[2];

	const double* radialDistortion = calibration.distortionCoefficients.data();
	const double* tangentialDistortion = calibration.distortionCoefficients.data() + 6;

	return std::make_shared<AnyCameraFisheyeD>(FisheyeCameraD(width, height, focal, focal, principalX, principalY, radialDistortion, tangentialDistortion));
}

// Explicit instantiations
template class FrameProviderT<false>;
template class FrameProviderT<true>;

} // namespace Sensors

} // namespace Quest

} // namespace Meta

} // namespace Platform

} // namespace Ocean
