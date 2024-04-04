// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_PLATFORM_META_QUEST_SENSOR_FRAME_PROVIDER_H
#define META_OCEAN_PLATFORM_META_QUEST_SENSOR_FRAME_PROVIDER_H

#include "ocean/platform/meta/quest/sensors/Sensors.h"
#include "ocean/platform/meta/quest/sensors/SensorSettingsManager.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Lock.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/HomogenousMatrix4.h"

#include <perception/sensor_calibration_io/CameraCalibrationImport.h>

#include <vros/sys/CalibrationManager.h>

#include <vros/sys/sensors/FrameSetConsumer.h>
#include <vros/sys/sensors/FrameType.h>
#include <vros/sys/sensors/SensorDataProvider.h>
#include <sensoraccess/CameraDataProvider.h>
#include <sensoraccess/DispatchThreadFactory.h>
#include <visiontypes/conversion/FrameSetConsumerAdapter.h>

#include <vros/sys/tracking/HeadTracker.h>

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

// Forward declaration
template <bool tAllowInvalidCameras>
class FrameProviderT;

/**
 * This class implements a provider for camera frames of Meta's Quest devices which guarantees camera profiles to always be valid.
 * @ingroup platformmetaquestsensors
 */
typedef FrameProviderT<false> FrameProvider;

/**
 * This class implements a provider for camera frames of Meta's Quest devices which may also include invalid camera profiles, e.g. in case no intrinsics are available.
 * @note It is the callers responsibility to check if a provided camera is valid or not.
 * @ingroup platformmetaquestsensors
 */
typedef FrameProviderT<true> FrameProviderWithInvalid;

/**
 * The key is used to identify purpose-based streams after their creation
 */
typedef std::size_t CameraStreamKey;
constexpr static CameraStreamKey kInvalidCameraStreamKey = std::numeric_limits<CameraStreamKey>::max();

/**
 * This class implements a provider for camera frames of Meta's Quest devices.
 * The provider supports two copy modes of the image content:<br>
 * - FCM_USE_IMMEDIATELY: The image content is not copied and the image data is handled in a derived class of this provider.
 * - FCM_MAKE_COPY: The image content is copied and can be moved out of this provider so that no derived class is necessary.
 * The provider needs a granted `com.oculus.permission.ACCESS_MR_SENSOR_DATA` permission.
 * @tparam tAllowInvalidCameras True, allows this class to provide invalid cameras. Otherwise only valid cameras will be provided (invalid ones will be unaccessible).
 * @note For `tAllowInvalidCameras == true` it is the callers responsibility to check if a provided camera is valid or not.
 * @ingroup platformmetaquestsensors
 */
template <bool tAllowInvalidCameras>
class OCEAN_PLATFORM_META_QUEST_SENSORS_EXPORT FrameProviderT
{
	friend class CustomFrameSetConsumer;

	public:

		/**
		 * Definition of individual camera types.
		 */
		enum CameraType : uint32_t
		{
			/// The lower two stereo cameras.
			CT_LOWER_STEREO_CAMERAS = 0u,
			/// The upper two stereo camera.
			CT_UPPER_STEREO_CAMERAS,
			/// All cameras.
			CT_ALL_CAMERAS,
			/// The number of camera types.
			CT_END
		};

		/**
		 * Definition of individual copy modes for the camera frames.
		 */
		enum FrameCopyMode : uint32_t
		{
			/// Invalid copy mode.
			FCM_INVALID = 0u,
			/// The camera frames are not copied as they will be used immediately by a derived class of this class.
			FCM_USE_IMMEDIATELY,
			/// The camera frames are copied and stored internally.
			FCM_MAKE_COPY,
		};

		/**
		 * Definition of an unordered set holding types of camera frames.
		 */
		typedef std::unordered_set<OSSDK::Sensors::v3::FrameType> CameraFrameTypes;
		typedef std::vector<sensoraccess::CameraStreamPurpose> CameraStreamPurposes;

		/**
		 * Definition of frame metadata.
		 */
		class FrameMetadata
		{
			public:

				/**
				 * Creates a new metadata object.
				 * @param exposureDuration The exposure time used to capture the frame, in seconds, with range [0, infinity)
				 * @param gain The gain used to capture the frame, with range [0, infinity)
				 * @param readoutDuration Duration between the start of the first row exposure and the start of the last row, read from the image metadata, in seconds, with range [0, infinity), 0 in case of Global-Shutter cameras
				 */
				inline FrameMetadata(const double exposureDuration, const double gain, const double readoutDuration);

				/**
				 * Returns the exposure tim of the frame.
				 * @return The exposure time used to capture the frame, in seconds, with range [0, infinity)
				 */
				inline double exposureDuration() const;

				/**
				 * Returns the gain used to capture the frame.
				 * @return The gain used to capture the frame, with range [0, infinity)
				 */
				inline double gain() const;

				/**
				 * REturns the exposure duration
				 * @return Duration between the start of the first row exposure and the start of the last row, read from the image metadata, in seconds, with range [0, infinity), 0 in case of Global-Shutter cameras
				 */
				inline double readoutDuration() const;

			protected:

				/// The exposure time used to capture the frame, in seconds, with range [0, infinity)
				double exposureDuration_ = 0.0;

				/// The gain used to capture the frame, with range [0, infinity)
				double gain_ = 0.0;

				/// Duration between the start of the first row exposure and the start of the last row, read from the image metadata, in seconds, with range [0, infinity), 0 in case of Global-Shutter cameras
				double readoutDuration_ = 0.0;
		};

		/**
		 * Definition of a vector holding FrameMetadata objets.
		 */
		typedef std::vector<FrameMetadata> FrameMetadatas;

	protected:

		/**
		 * Implements a custom consumer for frame sets.
		 */
		class CustomFrameSetConsumer : public OSSDK::Sensors::v4::FrameSetConsumer
		{
			protected:

				/**
				 * Definition of a vector holding ImageSensorConfiguration objects.
				 */
				typedef std::vector<OSSDK::Sensors::v4::ImageSensorConfiguration> ImageSensorConfigurations;

				/**
				 * Definition of an unordered set holding indices of cameras from which images will be extracted.
				 */
				typedef std::unordered_set<size_t> CameraIndexSet;

			public:

				/**
				 * Creates a new frame consumer.
				 * @param owner The owner of the new consumer
				 * @param ossdkHeadTracker The access to the head tracker
				 * @param ossdkCalibrationManager The manager for calibration data, nullptr to avoid access to realtime calibration
				 * @param cameraFrameType The type of the camera frames which this frame consumer will receive, must be valid
				 * @param frameCopyMode The copy mode for the frames to be used
				 * @param cameraType The type of the cameras to be received
				 */
				CustomFrameSetConsumer(FrameProviderT<tAllowInvalidCameras>& owner, const std::shared_ptr<OSSDK::Tracking::v8::IHeadTracker>& ossdkHeadTracker, const std::shared_ptr<OSSDK::Calibration::v4::ICalibrationManager>& ossdkCalibrationManager, OSSDK::Sensors::v3::FrameType cameraFrameType, const FrameCopyMode frameCopyMode, const CameraType cameraType);

				/**
				 * Requests the latest frames of this provider.
				 * The provider's copy mode must be `FCM_MAKE_COPY`, otherwise this function will never return any frames.
				 * After calling this function, the provider does not have any latest frames anymore until the next frames arrive internally.
				 * @param frames The resulting latest frames moved out of this provider, the frames will own the image content and the caller can keep/use the frames as long as necessary
				 * @param cameras Optional the resulting camera profiles of the frames, one for each frame, based on online calibration if available; otherwise factory calibration
				 * @param world_T_device Optional the resulting transformation between device and world, may be invalid if head tracking is currently failing; nullptr if not of interest
				 * @param device_T_cameras Optional the resulting transformations between cameras and device, one for each camera, based on online calibration if available; otherwise factory calibration
				 * @param cameraType Optional resulting camera type of the resulting frames; nullptr if not of interest
				 * @param frameMetadatas Optional the resulting frame metadata, one object associated with each resulting frame; nullptr if not of interest
				 * @return True, if succeeded
				 */
				bool latestFrames(Frames& frames, SharedAnyCamerasD* cameras = nullptr, HomogenousMatrixD4* world_T_device = nullptr, HomogenousMatricesD4* device_T_cameras = nullptr, CameraType* cameraType = nullptr, FrameMetadatas* frameMetadatas = nullptr);

				/**
				 * Returns the camera frame type of this frame consumer.
				 * @return The consumer's camera frame type
				 */
				inline OSSDK::Sensors::v3::FrameType cameraFrameType() const;

				/**
				 * Returns the exposure settings range of this frame consumer.
				 * @param minExposure The resulting minimal exposure durtation, in seconds, [0, infinity)
				 * @param maxExposure The resulting maximal exposure durtation, in seconds, with range [minExposure, infinity)
				 * @param minGain The resulting maximal exposure durtation, in seconds, with range [0, infinity)
				 * @param maxGain The resulting maximal exposure durtation, in seconds, with range [minGain, infinity)
				 * @return True, if succeeded; False, if the value range is unknown
				 */
				bool exposureSettingsRange(double& minExposure, double& maxExposure, double& minGain, double& maxGain) const;

				/**
				 * Returns the current exposure of all active cameras of this frame consumer.
				 * @param minExposure The minimal exposure value of all cameras, in seconds, with range [0, infinity)
				 * @param meanExposure The mean of all exposure values of all cameras, in seconds, with range [minExposure, maxExposure]
				 * @param maxExposure The maximal exposure value of all cameras, in seconds, with range [minExposure, infinity)
				 * @param minGain The minimal gain value of all cameras, with range [0, infinity)
				 * @param meanGain The mean of all gain values of all cameras, in seconds, with range [minGain, maxGain]
				 * @param maxGain The maximal gain value of all cameras, in seconds, with range [minGain, infinity)
				 * @return True, if succeeded; False, if the camera frame type is not active
				 */
				bool currentExposureSettings(double& minExposure, double& meanExposure, double& maxExposure, double& minGain, double& meanGain, double& maxGain) const;

				/**
				 * Returns the maximal number of frames this consumer can provide.
				 * @return The consumer's maximal number of frames
				 */
				inline size_t maximalFrames() const;

				void setCameraType(CameraType cameraType)
				{
					cameraType_ = cameraType;
					cameraIndexSetForCameraType_.clear();
				}

			protected:

			    /**
				 * Sets the number of cameras.
				 * @see OSSDK::Sensors::v3::FrameSetConsumer documentation.
				 */
				void setNumCameras(uint32_t numCameras) override;

				/**
				 * @see OSSDK::Sensors::v3::FrameSetConsumer documentation.
				 */
				void startConfigurationUpdate() override;

				/**
				 * @see OSSDK::Sensors::v3::FrameSetConsumer documentation.
				 */
				void finishConfigurationUpdate() override;

				/**
				 * Registers a new camera type.
				 * @param cameraIndex The index of the camera
				 * @param configuration The configuration of the new camera
				 */
				void setCameraConfiguration(uint32_t cameraIndex, const OSSDK::Sensors::v4::ImageSensorConfiguration& configuration) override;

			    /**
				 * @see OSSDK::Sensors::v3::FrameSetConsumer documentation.
				 */
				void beforeFirstFrameSet() override;

				/**
				 * @see OSSDK::Sensors::v3::FrameSetConsumer documentation.
				 */
				void afterLastFrameSet() override;

				/**
				 * Event functions for new sets of frames.
				 * @param images The new set of images
				 */
				void onFrameSet(OSSDK::ArrayView<const OSSDK::Sensors::v4::ImageData> images) override;

				/**
				 * Returns the indices of all cameras from which images will be extracted.
				 * @param cameraType The type of the cameras from which images will be extracted
				 * @param device_T_cameras The transformations between all cameras and the device, at least one
				 * @return The camera indices
				 */
				static CameraIndexSet determineCameraIndices(const CameraType cameraType, const HomogenousMatricesD4& device_T_cameras);

			protected:

				/// The owner of this frame consumer.
				FrameProviderT<tAllowInvalidCameras>& owner_;

				/// The access to the head tracker.
				std::shared_ptr<OSSDK::Tracking::v8::IHeadTracker> ossdkHeadTracker_;

				/// The consumer for the camera calibration, if any.
				OSSDK::Calibration::v4::ICalibrationConsumer* ossdkCalibrationConsumer_ = nullptr;

				/// The type of the camera frames which this frame consumer will receive.
				OSSDK::Sensors::v3::FrameType cameraFrameType_ = OSSDK::Sensors::v3::FrameType::Invalid;

				/// The type of the cameras from which the images will be extracted.
				CameraType cameraType_ = CT_ALL_CAMERAS;

				/// The frame copy mode to be used.
				FrameCopyMode frameCopyMode_ = FCM_INVALID;

				/// The configurations of the individual cameras, with same order as the provided frames.
				ImageSensorConfigurations imageSensorConfigurations_;

				/// The indices of all cameras from which images will be extracted (depends on `cameraType_`).
				CameraIndexSet cameraIndexSetForCameraType_;

				/// The camera profiles of the factory calibration, one for each camera.
				SharedAnyCamerasD factoryCameras_;

				/// The camera profiles of the online calibration, one for each camera.
				SharedAnyCamerasD onlineCameras_;

				/// The transformation between device and world for the latest owning frames; otherwise always invalid.
				HomogenousMatrixD4 world_T_device_ = HomogenousMatrixD4(false);

				/// The transformations between cameras and device from the factory calibration.
				HomogenousMatricesD4 device_T_factoryCameras_;

				/// The transformations between cameras and device from the online calibration.
				HomogenousMatricesD4 device_T_onlineCameras_;

				/// The latest frames in case frameCopyMode_ is `FCM_MAKE_COPY`, otherwise always empty.
				Frames latestOwningFrames_;

				/// The transforamtions between camera and device matching with the latest frames in case frameCopyMode_ is `FCM_MAKE_COPY`, otherwise always empty.
				HomogenousMatricesD4 latest_device_T_cameras_;

				/// The camera profiles between matching with the latest frames in case frameCopyMode_ is `FCM_MAKE_COPY`, otherwise always empty.
				SharedAnyCamerasD latestCameras_;

				/// The metadata of the latest frames.
				FrameMetadatas latestFrameMetadatas_;

				/// The lock for the owned frame.
				mutable Lock ownedFramesLock_;
		};

		/**
		 * Definition of an unordered map mapping sensor frame types to frame consumers.
		 */
		typedef std::unordered_map<OSSDK::Sensors::v3::FrameType, std::shared_ptr<CustomFrameSetConsumer>> FrameSetConsumerMap;

		/**
		 * Definition of a vector providing storage for pupose-based stream controls.
		 */
		struct CameraStreamStorageElement {
			sensoraccess::CameraStreamPurpose purpose;
			std::shared_ptr<CustomFrameSetConsumer> consumer;
			std::shared_ptr<sensoraccess::StreamControl> control;
		};
		typedef std::vector<CameraStreamStorageElement> CameraStreamStorage;

		/**
         * Definition of a vector providing storage for frame set consumers.
         */
        typedef std::vector<std::shared_ptr<CustomFrameSetConsumer>> FrameSetConsumers;

	public:

		/**
		 * Creates a new frame provider which will be invalid until initialize() is called.
		 * @see initialize().
		 */
		FrameProviderT() = default;

		/**
		 * Destructs this frame provider and releases all resources.
		 */
		virtual ~FrameProviderT();

		/**
		 * Initializes this frame provider and connects with the underlying sensor data provider.
		 * Ensure that the permission 'com.oculus.permission.ACCESS_MR_SENSOR_DATA' is granted before calling this function.
		 * The provider must not be initialized more than once.
		 * Afterwards, the camera frames can be received via startReceivingCameraFrames().
		 * @param frameCopyMode The copy mode for the frames to be used
		 * @return True, if succeeded
		 * @see startReceivingCameraFrames().
		 */
		bool initialize(const FrameCopyMode frameCopyMode);

		/**
		 * Returns all available types of camera frames.
		 * @return All types of camera frames
		 */
		const CameraFrameTypes& availableCameraFrameTypes();

		/**
		 * Returns whether a specific camera frame type is available.
		 * @param cameraFrameType The type of the camera frame to check
		 * @return True, if so
		 */
		bool isCameraFrameTypeAvailable(const OSSDK::Sensors::v3::FrameType& cameraFrameType);

		/**
		 * @return All available camera stream purposes.
		 */
		const CameraStreamPurposes& availableCameraStreamPurposes();

        /**
		 * Returns whether a specific purpose is available.
		 * @param cameraSteramPurpose the camera stream purpose to check
		 * @return True, if so
		 */
		bool isCameraStreamPurposeAvailable(const sensoraccess::CameraStreamPurpose& cameraStreamPurpose);

		/**
		 * Starts receiving camera frames of a specific camera frame type.
		 * In case the provider is already receiving camera frames for the specified frame type, the provider will restart stream.<br>
		 * The provider can access realtime camera calibration from the calibration manager if necessary.<br>
		 * The app needs to be whitelisted in the OS (https://fburl.com/diffusion/9qllda94).
		 * @param cameraFrameType The type of the camera frame to be received
		 * @param cameraType The type of the cameras to be received
		 * @param useRealtimeCalibration True, to extract real-time camera calibration, if available; False, to use the device's factory or online calibration
		 * @return True, if succeeded
		 * @see stopReceivingCameraFrames().
		 */
		bool startReceivingCameraFrames(const OSSDK::Sensors::v3::FrameType& cameraFrameType, const CameraType cameraType = CT_ALL_CAMERAS, const bool useRealtimeCalibration = false);

		/**
		 * Stops receiving camera frames.
		 * @return True, if succeeded
		 * @see startReceivingCameraFrames().
		 */
		bool stopReceivingCameraFrames(const OSSDK::Sensors::v3::FrameType& cameraFrameType);

		/**
		 * Prepares the system to receive the given camera stream purpose. This function needs to be called for every potential stream
		 * that is to be received *before* starting any streams.
		 * @param purpose The purpose of the camera stream to be created.
		 * @return The key to identify the prepared stream, or kInvalidKey upon error.
		 */
		CameraStreamKey preparePurposeStream(const sensoraccess::CameraStreamPurpose& purpose);

		/**
		 * Starts receiving camera frames of a specific camera purpose.
		 * In case the provider is already receiving camera frames for the specified frame type<br>
		 * The provider can access realtime camera calibration from the calibration manager if necessary.<br>
		 * The app needs to be whitelisted in the OS (https://fburl.com/diffusion/9qllda94).
		 * @param streamKey The key that was returned in preparePurposeStream().
		 * @param cameraType The type of the cameras to be received
		 * @param useRealtimeCalibration True, to extract real-time camera calibration, if available; False, to use the device's factory or online calibration
		 * @return True, if succeeded
		 * @see stopReceivingCameraFrames().
		 */
		bool startReceivingCameraFrames(CameraStreamKey streamKey, const CameraType cameraType = CT_ALL_CAMERAS);

		/**
		 * Stops receiving camera frames.
		 * @return True, if succeeded
		 * @see startReceivingCameraFrames().
		 */
		bool stopReceivingCameraFrames(CameraStreamKey streamKey);

		/**
		 * Stops reciving camera frames.
		 * @return True, if succeeded
		 * @see startReceivingCameraFrames().
		 */
		bool stopReceivingCameraFrames();

		/**
		 * Overrides the exposure settings for a specific camera frame type which is currently active.
		 * @param cameraFrameType The type of the camera frame for which the exposure setting will be overwritten
		 * @param exposure The exposure to be set in seconds, with range [minExposure, maxExposure], -1 to set exposure settings to default
		 * @param gain The gain to be set, with range [minGain, maxGain], -1 to set exposure settings to default
		 * @return True, if succeeded
		 * @see exposureSettingsRange(), activeCameraFrameTypes().
		 */
		bool overwriteExposureSettings(const OSSDK::Sensors::v3::FrameType& cameraFrameType, const double exposure, const double gain);

		/**
		 * Overrides the exposure settings for all currently active camera frame types.
		 * @param exposure The exposure to be set in seconds, with range [minExposure, maxExposure], -1 to set exposure settings to default
		 * @param gain The gain to be set, with range [minGain, maxGain], -1 to set exposure settings to default
		 * @return True, if succeeded
		 * @see exposureSettingsRange(), activeCameraFrameTypes().
		 */
		bool overwriteExposureSettings(const double exposure, const double gain);

		/**
		 * Returns the exposure settings range of an active frame camera type.
		 * @param cameraFrameType The camera frame type for which the exposure value range will be returned, must be active
		 * @param minExposure The resulting minimal exposure durtation, in seconds, with range [0, infinity)
		 * @param maxExposure The resulting maximal exposure durtation, in seconds, with range [minExposure, infinity)
		 * @param minGain The resulting maximal exposure durtation, in seconds, with range [0, infinity)
		 * @param maxGain The resulting maximal exposure durtation, in seconds, with range [minGain, infinity)
		 * @return True, if succeeded; False, if the value range is unknown
		 */
		bool exposureSettingsRange(const OSSDK::Sensors::v3::FrameType& cameraFrameType, double& minExposure, double& maxExposure, double& minGain, double& maxGain) const;

		/**
		 * Returns the current exposure of an active frame camera type.
		 * @param cameraFrameType The camera frame type for which the current exposure value will be returned, must be active
		 * @param minExposure Optional resulting minimal exposure value of all cameras, in seconds, with range [0, infinity), nullptr if not of interest
		 * @param meanExposure Optional resulting mean of all exposure values of all cameras, in seconds, with range [minExposure, maxExposure], nullptr if not of interest
		 * @param maxExposure Optional resulting maximal exposure value of all cameras, in seconds, with range [minExposure, infinity), nullptr if not of interest
		 * @param minGain Optional resulting minimal gain value of all cameras, with range [0, infinity), nullptr if not of interest
		 * @param meanGain Optional resulting mean of all gain values of all cameras, in seconds, with range [minGain, maxGain], nullptr if not of interest
		 * @param maxGain Optional resulting maximal gain value of all cameras, in seconds, with range [minGain, infinity), nullptr if not of interest
		 * @return True, if succeeded; False, if the camera frame type is not active
		 */
		bool currentExposureSettings(const OSSDK::Sensors::v3::FrameType& cameraFrameType, double* minExposure, double* meanExposure, double* maxExposure, double* minGain, double* meanGain, double* maxGain) const;

		/**
		 * Returns the exposure settings range of an active frame camera type.
		 * @param streamKey The key returned in preparePurposeStream().
		 * @param minExposure The resulting minimal exposure durtation, in seconds, with range [0, infinity)
		 * @param maxExposure The resulting maximal exposure durtation, in seconds, with range [minExposure, infinity)
		 * @param minGain The resulting maximal exposure durtation, in seconds, with range [0, infinity)
		 * @param maxGain The resulting maximal exposure durtation, in seconds, with range [minGain, infinity)
		 * @return True, if succeeded; False, if the value range is unknown
		 */
		bool exposureSettingsRange(CameraStreamKey streamKey, double& minExposure, double& maxExposure, double& minGain, double& maxGain) const;

		/**
		 * Returns the current exposure of an active frame camera type.
		 * @param streamKey The key returned in preparePurposeStream().
		 * @param minExposure Optional resulting minimal exposure value of all cameras, in seconds, with range [0, infinity), nullptr if not of interest
		 * @param meanExposure Optional resulting mean of all exposure values of all cameras, in seconds, with range [minExposure, maxExposure], nullptr if not of interest
		 * @param maxExposure Optional resulting maximal exposure value of all cameras, in seconds, with range [minExposure, infinity), nullptr if not of interest
		 * @param minGain Optional resulting minimal gain value of all cameras, with range [0, infinity), nullptr if not of interest
		 * @param meanGain Optional resulting mean of all gain values of all cameras, in seconds, with range [minGain, maxGain], nullptr if not of interest
		 * @param maxGain Optional resulting maximal gain value of all cameras, in seconds, with range [minGain, infinity), nullptr if not of interest
		 * @return True, if succeeded; False, if the camera frame type is not active
		 */
		bool currentExposureSettings(CameraStreamKey streamKey, double* minExposure, double* meanExposure, double* maxExposure, double* minGain, double* meanGain, double* maxGain) const;

		/**
		 * Requests the latest frames of this provider when using frame types.
		 * The provider's copy mode must be `FCM_MAKE_COPY`, otherwise this function will never return any frames.
		 * After calling this function, the provider does not have any latest frames anymore until the next frames arrive internally.
		 * @param frames The resulting latest frames moved out of this provider, the frames will own the image content and the caller can keep/use the frames as long as necessary
		 * @param cameras Optional the resulting camera profiles of the frames, one for each frame, based on online calibration if available; otherwise factory calibration
		 * @param world_T_device Optional the resulting transformation between device and world, may be invalid if head tracking is currently failing; nullptr if not of interest
		 * @param device_T_cameras Optional the resulting transformations between cameras and device, one for each camera, based on online calibration if available; otherwise factory calibration
		 * @param cameraFrameType Optional resulting camera frame type of the resulting frames; nullptr if not of interest
		 * @param cameraType Optional resulting camera type of the resulting frames; nullptr if not of interest
		 * @param frameMetadatas Optional the resulting frame metadata, one object associated with each resulting frame; nullptr if not of interest
		 * @param requestCameraFrameType Optional explicit camera frame type to request frames of a specific camera frame type only; otherwise FrameType::Invalid to receive frames from the next available camera
		 * @return True, if succeeded
		 */
		bool latestFrames(Frames& frames, SharedAnyCamerasD* cameras = nullptr, HomogenousMatrixD4* world_T_device = nullptr, HomogenousMatricesD4* device_T_cameras = nullptr, OSSDK::Sensors::v3::FrameType* cameraFrameType = nullptr, CameraType* cameraType = nullptr, FrameMetadatas* frameMetadatas = nullptr, const OSSDK::Sensors::v3::FrameType requestCameraFrameType = OSSDK::Sensors::v3::FrameType::Invalid);

		/**
		 * Requests the latest frames of this provider when using purposes.
		 * The provider's copy mode must be `FCM_MAKE_COPY`, otherwise this function will never return any frames.
		 * After calling this function, the provider does not have any latest frames anymore until the next frames arrive internally.
		 * @param streamKey The key returned in preparePurposeStream().
		 * @param frames The resulting latest frames moved out of this provider, the frames will own the image content and the caller can keep/use the frames as long as necessary
		 * @param cameras Optional the resulting camera profiles of the frames, one for each frame, based on online calibration if available; otherwise factory calibration
		 * @param world_T_device Optional the resulting transformation between device and world, may be invalid if head tracking is currently failing; nullptr if not of interest
		 * @param device_T_cameras Optional the resulting transformations between cameras and device, one for each camera, based on online calibration if available; otherwise factory calibration
		 * @param cameraType Optional resulting camera type of the resulting frames; nullptr if not of interest
		 * @param frameMetadatas Optional the resulting frame metadata, one object associated with each resulting frame; nullptr if not of interest
		 * @param requestCameraFrameType Optional explicit camera frame type to request frames of a specific camera frame type only; otherwise FrameType::Invalid to receive frames from the next available camera
		 * @return True, if succeeded
		 */
		bool latestFrames(CameraStreamKey streamKey, Frames& frames, SharedAnyCamerasD* cameras = nullptr, HomogenousMatrixD4* world_T_device = nullptr, HomogenousMatricesD4* device_T_cameras = nullptr, CameraType* cameraType = nullptr, FrameMetadatas* frameMetadatas = nullptr);

		/**
		 * Returns all currently active camera frame types which has been selected for receiving frames.
		 * @return The provider's currently active frame types
		 */
		CameraFrameTypes activeCameraFrameTypes() const;

		/**
		 * Releases this frame provider and all associated resources.
		 */
		void release();

		/**
		 * Returns whether this frame provider has access to a valid sensor data provider which allows to deliver frames.
		 * @return True, if so
		 */
		bool isValid() const;

		/**
		 * Translates the type of a camera frame to a readable string.
		 * @param cameraFrameType The camera frame type to translate
		 * @return The readable string
		 */
		static std::string translateCameraFrameType(const OSSDK::Sensors::v3::FrameType& cameraFrameType);

		/**
		 * Translates the readable type of a camera frame to a value.
		 * @param cameraFrameType The readable camera frame type to translate, must be valid
		 * @return The camera frame type
		 */
		static OSSDK::Sensors::v3::FrameType translateCameraFrameType(const std::string& cameraFrameType);

		/**
		 * Translates the type of a camera to a readable string.
		 * @param cameraType The camera type to translate
		 * @return The readable string
		 */
		static std::string translateCameraType(const CameraType cameraType);

	protected:

		/**
		 * Disabled move constructor.
		 */
		FrameProviderT(FrameProviderT<tAllowInvalidCameras>&&) = delete;

		/**
		 * Disabled copy constructor.
		 */
		FrameProviderT(const FrameProviderT<tAllowInvalidCameras>&) = delete;

		/**
		 * Event function for new frames.
		 * Beware: Calling startReceivingCameraFrames() or stopReceivingCameraFrames() will result in a dead-lock.
		 * This function can be implemented by derived classes and is intended for providers with copy mode `FCM_USE_IMMEDIATELY`
		 * The function should return as quickly as possible to avoid blocking the underyling system components.
		 * @param cameraFrameType The camera frame type of the frames
		 * @param cameraType The camera type of the frames
		 * @param frames The new frames
		 * @param cameras The camera profiles of the frames, one for each frame, based on online calibration if available; otherwise factory calibration
		 * @param world_T_device The transformation between device and world
		 * @param device_T_cameras The transformations between cameras and device, one for each camera, based on online calibration if available; otherwise factory calibration
		 * @param frameMetadatas The frame metadata, one object associated with each new frame
		 */
		virtual void onFrames(const OSSDK::Sensors::v3::FrameType& cameraFrameType, const CameraType& cameraType, const Frames& frames, const SharedAnyCamerasD& cameras, const HomogenousMatrixD4& world_T_device, const HomogenousMatricesD4& device_T_cameras, const FrameMetadatas& frameMetadatas);

		/**
		 * Disabled move operator.
		 * @return This object
		 */
		FrameProviderT& operator=(FrameProviderT<tAllowInvalidCameras>&&) = delete;

		/**
		 * Disabled copy operator.
		 * @return This object
		 */
		FrameProviderT& operator=(const FrameProviderT<tAllowInvalidCameras>&) = delete;

		/**
		 * Translates the OSSDK image format to an Ocean pixel format.
		 * @param ossdkImageFormat The OSSDK image format to be translated
		 * @return The corresponding Ocean pixel format, FrameType::FORMAT_UNDEFINED if not supported
		 */
		static FrameType::PixelFormat translateImageFormat(const OSSDK::Sensors::v4::ImageFormat& ossdkImageFormat);

		/**
		 * Reads the latest camera calibration from a calibration consumer.
		 * @param ossdkCalibrationConsumer The calibration consumer from which the lastest camera calibration will be read, must be valid
		 * @param cameraCalibrations The resulting camera calibrations
		 * @return True, if succeeded
		 */
		static bool readLatestCameraCalibration(OSSDK::Calibration::v4::ICalibrationConsumer* ossdkCalibrationConsumer, std::vector<perception::sensor_calibration_io::CameraCalibration>& cameraCalibrations);

		/**
		 * Converts perception's camera calibration to a fisheye camera and transformation matrix.
		 * @param calibration The calibration to convert
		 * @param device_T_camera The resulting transformation between camera and device
		 * @return The resulting camera profile, nullptr if the profile could not be converted
		 */
		static SharedAnyCameraD convertCameraProfile(const perception::sensor_calibration_io::CameraCalibration& calibration, HomogenousMatrixD4& device_T_camera);

	protected:

		/// The legacy sensor data provider which used for FrameType-based streaming
		std::shared_ptr<OSSDK::Sensors::v3::ISensorDataProvider> ossdkSensorDataProvider_;

		/// The purpose-based sensor data access.
		std::shared_ptr<sensoraccess::DispatchThreadHandle> ossdkDispatchThreadHandle_;
		std::unique_ptr<sensoraccess::CameraDataProvider> ossdkCameraDataProvider_;

		/// The access to the head tracker.
		std::shared_ptr<OSSDK::Tracking::v8::IHeadTracker> ossdkHeadTracker_;

		/// The manager for exposure settings, if used.
		SensorSettingsManager sensorSettingsManager_;

		/// The manager for the camera calibration.
		std::shared_ptr<OSSDK::Calibration::v4::ICalibrationManager> ossdkCalibrationManager_;

		/// The frame copy mode to be used.
		FrameCopyMode frameCopyMode_ = FCM_INVALID;

		/// The map of frame consumers.
		FrameSetConsumerMap frameSetConsumerMap_;
		using FrameSetConsumerAdapter = visiontypes::FrameSetConsumerAdapter<OSSDK::Sensors::v4::FrameSetConsumer, OSSDK::Sensors::v3::FrameSetConsumer>;
		std::vector<std::shared_ptr<FrameSetConsumerAdapter>> frameSetConsumerAdapterStorage_;

		/// The list of purpose-based camera streams.
		CameraStreamStorage purposeCameraStreams_;

		/// The index of the next frame consumer which will be used to deliver the last frame.
		size_t nextRoundRobinCameraFrameSetIndex_ = 0;

		/// The available types of camera frames e.g., `Headset`, `Controller`, `Hand`, etc.
		CameraFrameTypes ossdkAvailableCameraFrameTypes_;

		/// The available purposes, e.g. "worldTracking/iot", "color/passtrhough", etc.
		CameraStreamPurposes ossdkAvailableCameraStreamPurposes_;

		/// The provider's lock to make the public functions thread-safe.
		mutable Lock lock_;
};

template <bool tAllowInvalidCameras>
FrameProviderT<tAllowInvalidCameras>::FrameMetadata::FrameMetadata(const double exposureDuration, const double gain, const double readoutDuration) :
	exposureDuration_(exposureDuration),
	gain_(gain),
	readoutDuration_(readoutDuration)
{
	// nothing to do here
}

template <bool tAllowInvalidCameras>
double FrameProviderT<tAllowInvalidCameras>::FrameMetadata::exposureDuration() const
{
	return exposureDuration_;
}

template <bool tAllowInvalidCameras>
double FrameProviderT<tAllowInvalidCameras>::FrameMetadata::gain() const
{
	return gain_;
}

template <bool tAllowInvalidCameras>
double FrameProviderT<tAllowInvalidCameras>::FrameMetadata::readoutDuration() const
{
	return readoutDuration_;
}

template <bool tAllowInvalidCameras>
const typename FrameProviderT<tAllowInvalidCameras>::CameraFrameTypes& FrameProviderT<tAllowInvalidCameras>::availableCameraFrameTypes()
{
	const ScopedLock scopedLock(lock_);

	return ossdkAvailableCameraFrameTypes_;
}

template <bool tAllowInvalidCameras>
OSSDK::Sensors::v3::FrameType FrameProviderT<tAllowInvalidCameras>::CustomFrameSetConsumer::cameraFrameType() const
{
	return cameraFrameType_;
}

template <bool tAllowInvalidCameras>
size_t FrameProviderT<tAllowInvalidCameras>::CustomFrameSetConsumer::maximalFrames() const
{
	const ScopedLock scopedLock(ownedFramesLock_);

	return imageSensorConfigurations_.size();
}

template <bool tAllowInvalidCameras>
bool FrameProviderT<tAllowInvalidCameras>::isCameraFrameTypeAvailable(const OSSDK::Sensors::v3::FrameType& cameraFrameType)
{
	const ScopedLock scopedLock(lock_);

	return ossdkAvailableCameraFrameTypes_.find(cameraFrameType) != ossdkAvailableCameraFrameTypes_.cend();
}

template <bool tAllowInvalidCameras>
const typename FrameProviderT<tAllowInvalidCameras>::CameraStreamPurposes& FrameProviderT<tAllowInvalidCameras>::availableCameraStreamPurposes()
{
	const ScopedLock scopedLock(lock_);

	return ossdkAvailableCameraStreamPurposes_;
}

template <bool tAllowInvalidCameras>
bool FrameProviderT<tAllowInvalidCameras>::isCameraStreamPurposeAvailable(const sensoraccess::CameraStreamPurpose& cameraStreamPurpose)
{
	const ScopedLock scopedLock(lock_);

	return std::find(ossdkAvailableCameraStreamPurposes_.cbegin(), ossdkAvailableCameraStreamPurposes_.cend(), cameraStreamPurpose) != ossdkAvailableCameraStreamPurposes_.cend();
}

template <bool tAllowInvalidCameras>
bool FrameProviderT<tAllowInvalidCameras>::isValid() const
{
	const ScopedLock scopedLock(lock_);

	return ossdkSensorDataProvider_ != nullptr && !ossdkAvailableCameraFrameTypes_.empty();
}

template <bool tAllowInvalidCameras>
std::string FrameProviderT<tAllowInvalidCameras>::translateCameraFrameType(const OSSDK::Sensors::v3::FrameType& cameraFrameType)
{
	return OSSDK::Sensors::v3::frameTypeToString(cameraFrameType);
}

template <bool tAllowInvalidCameras>
OSSDK::Sensors::v3::FrameType FrameProviderT<tAllowInvalidCameras>::translateCameraFrameType(const std::string& cameraFrameType)
{
	ocean_assert(!cameraFrameType.empty());

	return OSSDK::Sensors::v3::frameTypeFromString(cameraFrameType.c_str());
}

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_SENSOR_FRAME_PROVIDER_H
