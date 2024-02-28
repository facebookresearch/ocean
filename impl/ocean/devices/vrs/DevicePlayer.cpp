// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/vrs/DevicePlayer.h"
#include "ocean/devices/vrs/DeviceRecorder.h"

#include "ocean/base/Timestamp.h"

#include "ocean/devices/Manager.h"

#include "ocean/devices/vrs/VRSFactory.h"
#include "ocean/devices/vrs/VRSTracker.h"
#include "ocean/devices/vrs/VRSTracker6DOF.h"
#include "ocean/devices/vrs/VRSSceneTracker6DOF.h"
#include "ocean/devices/vrs/VRSVisualTracker6DOF.h"

#include "ocean/io/CameraCalibrationManager.h"

#include "ocean/io/image/Image.h"

#include "ocean/io/vrs/Reader.h"
#include "ocean/io/vrs/Utilities.h"

#include "ocean/media/Manager.h"

#include <folly/json.h>

#include <perception/sensor_calibration_io/CameraCalibrationImport.h>

#include <vrs/RecordFileReader.h>
#include <vrs/RecordFormatStreamPlayer.h>

#include <vrs/utils/DecoderFactory.h>
#include <vrs/utils/VideoRecordFormatStreamPlayer.h>
#include <vrs/utils/PixelFrame.h>

#if defined(OCEAN_PLATFORM_BUILD_APPLE_MACOS) || defined(OCEAN_PLATFORM_BUILD_LINUX)
	#include <vrs/utils/xprs/XprsDecoder.h>
#endif

namespace Ocean
{

namespace Devices
{

namespace VRS
{

/**
 * This class implements a specialization of a `RecordFormatStreamPlayer` object for images.
 */
class DevicePlayer::PlayableImage : public vrs::utils::VideoRecordFormatStreamPlayer
{
	public:

		/**
		 * Creates a new playable object.
		 */
		PlayableImage() = default;

		/**
		 * Data layout read event function of the playable.
		 * @see RecordFormatStreamPlayer::onDataLayoutRead().
		 */
		bool onDataLayoutRead(const vrs::CurrentRecord& header, size_t blockIndex, vrs::DataLayout& dataLayout) override;

		/**
		 * Image read event function of the playable.
		 * @see RecordFormatStreamPlayer::onImageRead().
		 */
		bool onImageRead(const vrs::CurrentRecord& header, size_t index, const vrs::ContentBlock& block) override;

		/**
		 * Returns the PixelImage object associated with the playable.
		 * @return The playable's pixel image object
		 */
		inline const Media::PixelImageRef& pixelImage() const;

	protected:

		/// The PixelImage object associated with the playable to which the image content will be forwarded.
		Media::PixelImageRef pixelImage_;

		/// The explicit timestamp if available in the data layout, otherwise invalid.
		double explicitTimestamp_ = NumericD::minValue();

		/// The transformation between camera and device, if known.
		HomogenousMatrixD4 device_T_camera_ = HomogenousMatrixD4(false);

		/// The camera profile, may or may not change with every new frame, if known.
		SharedAnyCamera latestAnyCamera_;

		/// Re-usable buffer for jpg data.
		std::vector<uint8_t> reusableBuffer_;
};

/**
 * This class implements a playable for 6-DOF tracker streams.
 */
class DevicePlayer::PlayableTracker6DOFSample : public vrs::RecordFormatStreamPlayer
{
	public:

		/**
		 * Creates a new playable object.
		 */
		PlayableTracker6DOFSample();

		/**
		 * Destructs this playable object.
		 */
		~PlayableTracker6DOFSample() override;

		/**
		 * Data layout read event function of the playable.
		 * @see RecordFormatStreamPlayer::onDataLayoutRead().
		 */
		bool onDataLayoutRead(const vrs::CurrentRecord& header, size_t blockIndex, vrs::DataLayout& dataLayout) override;

	protected:

		/// The name of the device in the stream.
		std::string deviceName_;

		/// The associated tracker device to which the stream information will be forwarded.
		VRSTracker6DOFRef vrsTracker6DOF_;
};

/**
 * This class implements a playable for GPS tracker streams.
 */
class DevicePlayer::PlayableGPSTrackerSample : public vrs::RecordFormatStreamPlayer
{
	public:

		/**
		 * Creates a new playable object.
		 */
		PlayableGPSTrackerSample();

		/**
		 * Destructs this playable object.
		 */
		~PlayableGPSTrackerSample() override;

		/**
		 * Data layout read event function of the playable.
		 * @see RecordFormatStreamPlayer::onDataLayoutRead().
		 */
		bool onDataLayoutRead(const vrs::CurrentRecord& header, size_t blockIndex, vrs::DataLayout& dataLayout) override;

	protected:

		/// The name of the device in the stream.
		std::string deviceName_;

		/// The associated tracker device to which the stream information will be forwarded.
		VRSGPSTrackerRef vrsGPSTracker_;
};

/**
 * This class implements a playable for scene 6-DOF tracker streams.
 */
class DevicePlayer::PlayableSceneTracker6DOFSample : public vrs::RecordFormatStreamPlayer
{
	public:

		/**
		 * Creates a new playable object.
		 */
		PlayableSceneTracker6DOFSample();

		/**
		 * Destructs this playable object.
		 */
		~PlayableSceneTracker6DOFSample() override;

		/**
		 * Data layout read event function of the playable.
		 * @see RecordFormatStreamPlayer::onDataLayoutRead().
		 */
		bool onDataLayoutRead(const vrs::CurrentRecord& header, size_t blockIndex, vrs::DataLayout& dataLayout) override;

	protected:

		/// The name of the device in the stream.
		std::string deviceName_;

		/// The associated tracker device to which the stream information will be forwarded.
		VRSSceneTracker6DOFRef vrsSceneTracker6DOF_;
};

/**
 * This class implements a playable for 6-DOF tracker streams.
 */
class DevicePlayer::PlayableHomogenousMatrix4 : public vrs::RecordFormatStreamPlayer
{
	public:

		/**
		 * Creates a new playable object.
		 * @param deviceName The name of the device
		 */
		explicit PlayableHomogenousMatrix4(const std::string& deviceName);

		/**
		 * Destructs this playable object.
		 */
		~PlayableHomogenousMatrix4() override;

		/**
		 * Data layout read event function of the playable.
		 * @see RecordFormatStreamPlayer::onDataLayoutRead().
		 */
		bool onDataLayoutRead(const vrs::CurrentRecord& header, size_t blockIndex, vrs::DataLayout& dataLayout) override;

	protected:

		/// The name of the device in the stream.
		std::string deviceName_;

		/// The associated tracker device to which the stream information will be forwarded.
		VRSTracker6DOFRef vrsTracker6DOF_;

		/// The id of the object associated with the transformation.
		VRSTracker6DOF::ObjectId objectId_ = VRSTracker6DOF::invalidObjectId();
};

bool DevicePlayer::PlayableImage::onDataLayoutRead(const vrs::CurrentRecord& header, size_t /*blockIndex*/, vrs::DataLayout& dataLayout)
{
	// The use of DataLayout::findDataPieceXXX an anti-pattern, please do not clone this bad practice.
	// See D39522991 or D39527762 for proper efficient use of datalayout objects.
	if (header.recordType == vrs::Record::Type::CONFIGURATION)
	{
		const vrs::DataPieceValue<unsigned int>* cameraIdDataPieceValue = dataLayout.findDataPieceValue<unsigned int>("camera_id");

		if (cameraIdDataPieceValue == nullptr)
		{
			ocean_assert(false && "Need camera_id value");
			return true;
		}

		const unsigned int cameraId = cameraIdDataPieceValue->get();

		const vrs::DataPieceString* factoryCalibrationDataPieceString = dataLayout.findDataPieceString("factory_calibration");

		if (factoryCalibrationDataPieceString == nullptr)
		{
			ocean_assert(false && "Need camera calibration string");
			return true;
		}

		const std::string factoryCalibration = factoryCalibrationDataPieceString->get();

		std::map<int, perception::sensor_calibration_io::CameraCalibration> parsedCalibrations;

		if (!perception::sensor_calibration_io::parseFromJson(factoryCalibration, parsedCalibrations))
		{
			ocean_assert(false && "Failed to parse JSON camera calibrations");
			return true;
		}

		const std::map<int, perception::sensor_calibration_io::CameraCalibration>::const_iterator iCalibration = parsedCalibrations.find(int(cameraId));

		if (iCalibration == parsedCalibrations.cend())
		{
			ocean_assert(false && "Unknown camera id!");
			return true;
		}

		const perception::sensor_calibration_io::CameraCalibration& calibration = iCalibration->second;

		const VectorD3 translation(calibration.deviceFromCameraTranslation);
		const SquareMatrixD3 rotationF((const double*)calibration.deviceFromCameraRotation, /* rowAligned = */ true);
		ocean_assert(rotationF.isOrthonormal());

		// the coordinate system of the camera is flipped (the camera is looking towards the negative z-space), this is the flipped coordinate system in Ocean
		// therefore, we need to rotate the coordinate system around the x-axis to determine Ocean's default coordinate system, a camera looking towards the negative z-space
		const SquareMatrixD3 rotation(rotationF * SquareMatrixD3(1, 0, 0, 0, -1, 0, 0, 0, -1));

		// transformation transforming points defined in the coordinate system of the camera to points defined in the coordinate system of the device
		const HomogenousMatrixD4 device_T_camera(translation, rotation);

		if (calibration.width <= 0 || calibration.height <= 0)
		{
			ocean_assert(false && "Invalid camera resolution!");
			return true;
		}

		if (pixelImage_.isNull())
		{
			pixelImage_ = Media::Manager::get().newMedium("VRS Pixel Medium " + String::toAString(cameraId), Media::Medium::PIXEL_IMAGE);
			ocean_assert(pixelImage_);

			pixelImage_->setCapacity(30);
			pixelImage_->start();
		}

		device_T_camera_ = device_T_camera;

		const unsigned int width = (unsigned int)(calibration.width);
		const unsigned int height = (unsigned int)(calibration.height);

		if (calibration.projectionModel == perception::sensor_calibration_io::CameraCalibration::ProjectionModel::PINHOLE_SYMMETRIC
					&& calibration.distortionModel == perception::sensor_calibration_io::CameraCalibration::DistortionModel::FISHEYE62
					&& calibration.projectionCoefficients.size() == 3 && calibration.distortionCoefficients.size() == 8)
			{
				const double& focal = calibration.projectionCoefficients[0];
				const double& principalX = calibration.projectionCoefficients[1];
				const double& principalY = calibration.projectionCoefficients[2];

				const double* radialDistortion = calibration.distortionCoefficients.data();
				const double* tangentialDistortion = calibration.distortionCoefficients.data() + 6;

				const FisheyeCameraD fisheyeCamera(width, height, focal, focal, principalX, principalY, radialDistortion, tangentialDistortion);

				latestAnyCamera_ = std::make_shared<AnyCameraFisheye>(FisheyeCamera(fisheyeCamera));
			}
			else if (calibration.projectionModel == perception::sensor_calibration_io::CameraCalibration::ProjectionModel::PINHOLE
						&& calibration.distortionModel == perception::sensor_calibration_io::CameraCalibration::DistortionModel::PLANAR
						&& calibration.projectionCoefficients.size() == 4 && calibration.distortionCoefficients.size() == 4)
			{
				const Scalar focalX = Scalar(calibration.projectionCoefficients[0]);
				const Scalar focalY = Scalar(calibration.projectionCoefficients[1]);
				const Scalar principalX = Scalar(calibration.projectionCoefficients[2]);
				const Scalar principalY = Scalar(calibration.projectionCoefficients[3]);

				const PinholeCamera::DistortionPair radialDistortion(Scalar(calibration.distortionCoefficients[0]), Scalar(calibration.distortionCoefficients[1]));
				const PinholeCamera::DistortionPair tangentialDistortion(Scalar(calibration.distortionCoefficients[2]), Scalar(calibration.distortionCoefficients[3]));

				const PinholeCamera pinholeCamera(width, height, focalX, focalY, principalX, principalY, radialDistortion, tangentialDistortion);

				latestAnyCamera_ = std::make_shared<AnyCameraPinhole>(pinholeCamera);
			}
			else
			{
				latestAnyCamera_ = nullptr;

				ocean_assert(false && "Camera model not supported!");
				return true;
			}
	}
	else if (header.recordType == vrs::Record::Type::DATA)
	{
		const vrs::DataPieceValue<double>* timestampDataPieceValue = dataLayout.findDataPieceValue<double>("timestamp");

		if (timestampDataPieceValue)
		{
			ocean_assert(explicitTimestamp_ == NumericD::minValue());
			explicitTimestamp_ = timestampDataPieceValue->get();
		}
	}

	return true;
}

bool DevicePlayer::PlayableImage::onImageRead(const vrs::CurrentRecord& header, size_t /*index*/, const vrs::ContentBlock& block)
{
	const vrs::ImageContentBlockSpec& imageContentBlockSpec = block.image();

	Frame frame;

	if (imageContentBlockSpec.getImageFormat() == vrs::ImageFormat::RAW)
	{
		const unsigned int width = imageContentBlockSpec.getWidth();
		const unsigned int height = imageContentBlockSpec.getHeight();

		const unsigned int strideBytes = imageContentBlockSpec.getStride();

		FrameType::PixelFormat pixelFormat = FrameType::FORMAT_UNDEFINED;
		if (!IO::VRS::Utilities::toOceanPixelFormat(imageContentBlockSpec.getPixelFormat(), pixelFormat) || pixelFormat == FrameType::FORMAT_UNDEFINED)
		{
			ocean_assert(false && "Undefined pixel format");
			return true;
		}

		ocean_assert(imageContentBlockSpec.getChannelCountPerPixel() == FrameType::channels(pixelFormat));

		if (pixelFormat == FrameType::FORMAT_Y_U_V12)
		{
			// special handling for multi-plane pixel formats as VRS does not support multiple planes (all planes are provided in one continuous buffer)

			frame = Frame(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT));

			if (frame.frameTypeSize() != imageContentBlockSpec.getBlockSize())
			{
				ocean_assert(false && "Invalid frame data!");
				return true;
			}

			std::vector<uint8_t> temporaryMemoryBlock(frame.frameTypeSize());
			header.reader->read(temporaryMemoryBlock.data(), imageContentBlockSpec.getBlockSize());

			memcpy(frame.data<void>(0), temporaryMemoryBlock.data() + 0, frame.size(0u));
			memcpy(frame.data<void>(1), temporaryMemoryBlock.data() + frame.size(0u), frame.size(1u));
			memcpy(frame.data<void>(2), temporaryMemoryBlock.data() + frame.size(0u) + frame.size(1u), frame.size(2u));
		}
		else
		{
			unsigned int paddingElements = 0u;
			if (Frame::strideBytes2paddingElements(pixelFormat, width, strideBytes, paddingElements))
			{
				frame = Frame(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), paddingElements);

				if (frame.size(0u) != imageContentBlockSpec.getBlockSize())
				{
					ocean_assert(false && "Invalid frame data!");
					return true;
				}

				header.reader->read(frame.data<uint8_t>(), imageContentBlockSpec.getBlockSize());
			}
		}
	}
	else if (imageContentBlockSpec.getImageFormat() == vrs::ImageFormat::JPG)
	{
		reusableBuffer_.resize(header.reader->getUnreadBytes());

		if (header.reader->read(reusableBuffer_) == 0)
		{
			frame = IO::Image::decodeImage(reusableBuffer_.data(), reusableBuffer_.size(), "jpg");
		}
	}
	else if (imageContentBlockSpec.getImageFormat() == vrs::ImageFormat::VIDEO)
	{
		std::shared_ptr<vrs::utils::PixelFrame> sharedPixelFrame;

		vrs::utils::PixelFrame::init(sharedPixelFrame, block.image());
		ocean_assert(sharedPixelFrame != nullptr);

		if (tryToDecodeFrame(*sharedPixelFrame, header, block) != 0)
		{
			Log::error() << "Decoding of frame failed. Are the VRS video codecs registered?";
			return true;
		}

		if (!IO::VRS::Utilities::toOceanFrame(*sharedPixelFrame, frame))
		{
			Log::error() << "Failed to convert pixel frame";
			return true;
		}
	}

	if (!frame.isValid())
	{
		ocean_assert(false && "Invalid frame!");
		return true;
	}

	const Timestamp frameTimestamp = explicitTimestamp_ != NumericD::minValue() ? Timestamp(explicitTimestamp_) : Timestamp(header.timestamp);
	explicitTimestamp_ = NumericD::minValue(); // resetting the explicit timestamp

	ocean_assert(pixelImage_);

	frame.setTimestamp(frameTimestamp);

	if (device_T_camera_.isValid())
	{
		pixelImage_->setDevice_T_camera(device_T_camera_);
	}

	pixelImage_->setPixelImage(std::move(frame), latestAnyCamera_);

	return true;
}

inline const Media::PixelImageRef& DevicePlayer::PlayableImage::pixelImage() const
{
	return pixelImage_;
}

DevicePlayer::PlayableTracker6DOFSample::PlayableTracker6DOFSample()
{
	// nothing to do here
}

DevicePlayer::PlayableTracker6DOFSample::~PlayableTracker6DOFSample()
{
	vrsTracker6DOF_.release();

	if (!deviceName_.empty())
	{
		Manager::get().unregisterAdhocDevice(deviceName_);
	}
}

bool DevicePlayer::PlayableTracker6DOFSample::onDataLayoutRead(const vrs::CurrentRecord& header, size_t blockIndex, vrs::DataLayout& dataLayout)
{
	// The use of DataLayout::findDataPieceXXX an anti-pattern, please do not clone this bad practice.
	// See D39522991 or D39527762 for proper efficient use of datalayout objects.
	if (header.recordType == vrs::Record::Type::CONFIGURATION)
	{
		const vrs::DataPieceString* deviceNameValue = dataLayout.findDataPieceString("device_name");
		const vrs::DataPieceString* deviceTypeMajorValue = dataLayout.findDataPieceString("device_type_major");
		const vrs::DataPieceString* deviceTypeMinorValue = dataLayout.findDataPieceString("device_type_minor");

		if (deviceNameValue != nullptr && deviceTypeMajorValue != nullptr && deviceTypeMinorValue != nullptr)
		{
			std::string vrsDeviceName = deviceNameValue->get();

			if (vrsDeviceName.empty())
			{
				ocean_assert(false && "Invalid device name!");
				return true;
			}

			// we add 'VRS' a prefix to each device name to allow using the original device as well in the same session
			vrsDeviceName = "VRS " + vrsDeviceName;

			if (deviceName_ != vrsDeviceName)
			{
				ocean_assert(deviceName_.empty());
				deviceName_ = vrsDeviceName;

				const std::string deviceTypeMajor = deviceTypeMajorValue->get();
				const std::string deviceTypeMinor = deviceTypeMinorValue->get();

				const Device::DeviceType deviceType = Device::DeviceType::translateDeviceType(deviceTypeMajor, deviceTypeMinor);
				ocean_assert(deviceType);

				if (!Manager::get().registerAdhocDevice(deviceName_, deviceType, Manager::AdhocInstanceFunction::createStatic(&VRSFactory::createTracker6DOF)))
				{
					ocean_assert(false && "Failed to register device");
					return true;
				}

				Log::info() << "VRS contains 6DOF tracker '" << deviceName_ << "'";

				vrsTracker6DOF_ = Manager::get().device(deviceName_, false /* useExclusive*/);
				ocean_assert(vrsTracker6DOF_);
			}
		}
	}
	else
	{
		ocean_assert(header.recordType == vrs::Record::Type::DATA);

		if (vrsTracker6DOF_.isNull())
		{
			return true;
		}

		const vrs::DataPieceValue<double>* timestampValue = dataLayout.findDataPieceValue<double>("timestamp");

		if (timestampValue == nullptr)
		{
			ocean_assert(false && "Invalid record!");
			return true;
		}

		const vrs::DataPieceStringMap<unsigned int>* objectDescriptionMapValue = dataLayout.findDataPieceStringMap<unsigned int>("object_description_map");

		if (objectDescriptionMapValue)
		{
			VRSTracker6DOF::VRSObjectDescriptionMap objectDescriptionMap;
			objectDescriptionMapValue->get(objectDescriptionMap);

			vrsTracker6DOF_->updateVRSObjects(objectDescriptionMap);
		}

		VRSTracker6DOF::ObjectIdSet foundObjectIds;
		VRSTracker6DOF::ObjectIdSet lostObjectIds;

		std::vector<unsigned int> objectIds;

		const vrs::DataPieceVector<unsigned int>* foundObjectValue = dataLayout.findDataPieceVector<unsigned int>("found_objects");
		if (foundObjectValue != nullptr)
		{
			foundObjectValue->get(objectIds);

			foundObjectIds = VRSTracker6DOF::ObjectIdSet(objectIds.cbegin(), objectIds.cend());
		}

		const vrs::DataPieceVector<unsigned int>* lostObjectValue = dataLayout.findDataPieceVector<unsigned int>("lost_objects");
		if (lostObjectValue != nullptr)
		{
			lostObjectValue->get(objectIds);

			lostObjectIds = VRSTracker6DOF::ObjectIdSet(objectIds.cbegin(), objectIds.cend());
		}

		if (!foundObjectIds.empty() || !lostObjectIds.empty())
		{
			vrsTracker6DOF_->forwardObjectEvent(foundObjectIds, lostObjectIds, Timestamp(timestampValue->get()));
		}

		const vrs::DataPieceVector<unsigned int>* objectIdsValue = dataLayout.findDataPieceVector<unsigned int>("object_ids");

		if (objectIdsValue != nullptr)
		{
			objectIdsValue->get(objectIds);

			if (!objectIds.empty())
			{
				const vrs::DataPieceString* referenceSystemValue = dataLayout.findDataPieceString("reference_system");
				const vrs::DataPieceVector<vrs::Matrix4Dd>* homogenousMatrices4Value = dataLayout.findDataPieceVector<vrs::Matrix4Dd>("homogenous_matrices_4");

				if (referenceSystemValue == nullptr || homogenousMatrices4Value == nullptr)
				{
					ocean_assert(false && "Invalid input!");
					return true;
				}

				VRSTracker6DOF::ReferenceSystem referenceSystem = VRSTracker6DOF::RS_OBJECT_IN_DEVICE;

				if (referenceSystemValue->get() == "DEVICE_IN_OBJECT")
				{
					referenceSystem = VRSTracker6DOF::RS_DEVICE_IN_OBJECT;
				}
				else
				{
					ocean_assert(referenceSystemValue->get() == "OBJECT_IN_DEVICE");
				}

				std::vector<vrs::Matrix4Dd> matrices;
				homogenousMatrices4Value->get(matrices);

				if (matrices.size() != objectIds.size())
				{
					ocean_assert(false && "Invalid input!");
					return true;
				}

				Quaternions orientations;
				orientations.reserve(matrices.size());

				Vectors3 positions;
				positions.reserve(matrices.size());

				for (const vrs::Matrix4Dd& matrix : matrices)
				{
					const HomogenousMatrix4 homogenousMatrix((const double*)&matrix, true /* row aligned */);

					ocean_assert(homogenousMatrix.isValid());
					ocean_assert(homogenousMatrix.rotationMatrix().isOrthonormal(Numeric::weakEps()));

					orientations.emplace_back(homogenousMatrix.rotation());
					positions.emplace_back(homogenousMatrix.translation());
				}

				Measurement::Metadata metadata;

				const vrs::DataPieceString* metadataValue = dataLayout.findDataPieceString("metadata");
				if (metadataValue != nullptr)
				{
					std::istringstream inputStream(metadataValue->get(), std::ios::binary);
					IO::InputBitstream bitstream(inputStream);

					if (!DeviceRecorder::RecordableTracker6DOFSample::readMetadataFromBitstream(bitstream, metadata))
					{
						ocean_assert(false && "Failed to read metadata!");
						return true;
					}
				}

				vrsTracker6DOF_->forwardSampleEvent(VRSTracker6DOF::ObjectIds(objectIds.cbegin(), objectIds.cend()), std::move(orientations), std::move(positions), referenceSystem, Timestamp(timestampValue->get()), std::move(metadata));
			}
		}
	}

	return true;
}

DevicePlayer::PlayableGPSTrackerSample::PlayableGPSTrackerSample()
{
	// nothing to do here
}

DevicePlayer::PlayableGPSTrackerSample::~PlayableGPSTrackerSample()
{
	vrsGPSTracker_.release();

	if (!deviceName_.empty())
	{
		Manager::get().unregisterAdhocDevice(deviceName_);
	}
}

bool DevicePlayer::PlayableGPSTrackerSample::onDataLayoutRead(const vrs::CurrentRecord& header, size_t blockIndex, vrs::DataLayout& dataLayout)
{
	// The use of DataLayout::findDataPieceXXX an anti-pattern, please do not clone this bad practice.
	// See D39522991 or D39527762 for proper efficient use of datalayout objects.
	if (header.recordType == vrs::Record::Type::CONFIGURATION)
	{
		const vrs::DataPieceString* deviceNameValue = dataLayout.findDataPieceString("device_name");
		const vrs::DataPieceString* deviceTypeMajorValue = dataLayout.findDataPieceString("device_type_major");
		const vrs::DataPieceString* deviceTypeMinorValue = dataLayout.findDataPieceString("device_type_minor");

		if (deviceNameValue != nullptr && deviceTypeMajorValue != nullptr && deviceTypeMinorValue != nullptr)
		{
			std::string vrsDeviceName = deviceNameValue->get();

			if (vrsDeviceName.empty())
			{
				ocean_assert(false && "Invalid device name!");
				return true;
			}

			// we add 'VRS' a prefix to each device name to allow using the original device as well in the same session
			vrsDeviceName = "VRS " + vrsDeviceName;

			if (deviceName_ != vrsDeviceName)
			{
				ocean_assert(deviceName_.empty());
				deviceName_ = vrsDeviceName;

				const std::string deviceTypeMajor = deviceTypeMajorValue->get();
				const std::string deviceTypeMinor = deviceTypeMinorValue->get();

				const Device::DeviceType deviceType = Device::DeviceType::translateDeviceType(deviceTypeMajor, deviceTypeMinor);
				ocean_assert(deviceType);

				if (!Manager::get().registerAdhocDevice(deviceName_, deviceType, Manager::AdhocInstanceFunction::createStatic(&VRSFactory::createGPSTracker)))
				{
					ocean_assert(false && "Failed to register device");
					return true;
				}

				Log::info() << "VRS contains GPS tracker '" << deviceName_ << "'";

				vrsGPSTracker_ = Manager::get().device(deviceName_, false /* useExclusive*/);
				ocean_assert(vrsGPSTracker_);
			}
		}
	}
	else
	{
		ocean_assert(header.recordType == vrs::Record::Type::DATA);

		if (vrsGPSTracker_.isNull())
		{
			return true;
		}

		const vrs::DataPieceValue<double>* timestampValue = dataLayout.findDataPieceValue<double>("timestamp");

		if (timestampValue == nullptr)
		{
			ocean_assert(false && "Invalid record!");
			return true;
		}

		const vrs::DataPieceStringMap<unsigned int>* objectDescriptionMapValue = dataLayout.findDataPieceStringMap<unsigned int>("object_description_map");

		if (objectDescriptionMapValue)
		{
			VRSTracker6DOF::VRSObjectDescriptionMap objectDescriptionMap;
			objectDescriptionMapValue->get(objectDescriptionMap);

			vrsGPSTracker_->updateVRSObjects(objectDescriptionMap);
		}

		VRSTracker6DOF::ObjectIdSet foundObjectIds;
		VRSTracker6DOF::ObjectIdSet lostObjectIds;

		std::vector<unsigned int> objectIds;

		const vrs::DataPieceVector<unsigned int>* foundObjectValue = dataLayout.findDataPieceVector<unsigned int>("found_objects");
		if (foundObjectValue != nullptr)
		{
			foundObjectValue->get(objectIds);

			foundObjectIds = VRSTracker6DOF::ObjectIdSet(objectIds.cbegin(), objectIds.cend());
		}

		const vrs::DataPieceVector<unsigned int>* lostObjectValue = dataLayout.findDataPieceVector<unsigned int>("lost_objects");
		if (lostObjectValue != nullptr)
		{
			lostObjectValue->get(objectIds);

			lostObjectIds = VRSTracker6DOF::ObjectIdSet(objectIds.cbegin(), objectIds.cend());
		}

		if (!foundObjectIds.empty() || !lostObjectIds.empty())
		{
			vrsGPSTracker_->forwardObjectEvent(foundObjectIds, lostObjectIds, Timestamp(timestampValue->get()));
		}

		const vrs::DataPieceVector<unsigned int>* objectIdsValue = dataLayout.findDataPieceVector<unsigned int>("object_ids");

		if (objectIdsValue != nullptr)
		{
			objectIdsValue->get(objectIds);

			if (!objectIds.empty())
			{
				const vrs::DataPieceString* referenceSystemValue = dataLayout.findDataPieceString("reference_system");
				const vrs::DataPieceVector<double>* longitudesValue = dataLayout.findDataPieceVector<double>("longitudes");
				const vrs::DataPieceVector<double>* latitudesValue = dataLayout.findDataPieceVector<double>("latitudes");
				const vrs::DataPieceVector<float>* altitudesValue = dataLayout.findDataPieceVector<float>("altitudes");
				const vrs::DataPieceVector<float>* directionsValue = dataLayout.findDataPieceVector<float>("directions");
				const vrs::DataPieceVector<float>* speedsValue = dataLayout.findDataPieceVector<float>("speeds");
				const vrs::DataPieceVector<float>* accuraciesValue = dataLayout.findDataPieceVector<float>("accuracies");
				const vrs::DataPieceVector<float>* altitudeAccuraciesValue = dataLayout.findDataPieceVector<float>("altitudeAccuracies");
				const vrs::DataPieceVector<float>* directionAccuraciesValue = dataLayout.findDataPieceVector<float>("directionAccuracies");
				const vrs::DataPieceVector<float>* speedAccuraciesValue = dataLayout.findDataPieceVector<float>("speedAccuracies");

				if (referenceSystemValue == nullptr || longitudesValue == nullptr || latitudesValue == nullptr)
				{
					ocean_assert(false && "Invalid input!");
					return true;
				}

				VRSTracker6DOF::ReferenceSystem referenceSystem = VRSTracker6DOF::RS_OBJECT_IN_DEVICE;

				if (referenceSystemValue->get() == "DEVICE_IN_OBJECT")
				{
					referenceSystem = VRSTracker6DOF::RS_DEVICE_IN_OBJECT;
				}
				else
				{
					ocean_assert(referenceSystemValue->get() == "OBJECT_IN_DEVICE");
				}

				std::vector<double> longitudes;
				longitudesValue->get(longitudes);

				std::vector<double> latitudes;
				latitudesValue->get(latitudes);

				std::vector<float> altitudes;
				if (altitudesValue != nullptr)
				{
					altitudesValue->get(altitudes);
				}

				std::vector<float> directions;
				if (directionsValue != nullptr)
				{
					directionsValue->get(directions);
				}

				std::vector<float> speeds;
				if (speedsValue != nullptr)
				{
					speedsValue->get(speeds);
				}

				std::vector<float> accuracies;
				if (accuraciesValue != nullptr)
				{
					accuraciesValue->get(accuracies);
				}

				std::vector<float> altitudeAccuracies;
				if (altitudeAccuraciesValue != nullptr)
				{
					altitudeAccuraciesValue->get(altitudeAccuracies);
				}

				std::vector<float> directionAccuracies;
				if (directionAccuraciesValue != nullptr)
				{
					directionAccuraciesValue->get(directionAccuracies);
				}

				std::vector<float> speedAccuracies;
				if (speedAccuraciesValue != nullptr)
				{
					speedAccuraciesValue->get(speedAccuracies);
				}

				if (longitudes.size() != objectIds.size() || latitudes.size() != objectIds.size())
				{
					ocean_assert(false && "Invalid input!");
					return true;
				}

				GPSTracker::Locations locations;
				locations.reserve(objectIds.size());

				for (size_t n = 0; n < objectIds.size(); ++n)
				{
					locations.emplace_back(
						latitudes[n],
						longitudes[n],
						n < altitudes.size() ? altitudes[n] : NumericF::minValue(),
						n < directions.size() ? directions[n] : -1.0f,
						n < speeds.size() ? speeds[n] : -1.0f,
						n < accuracies.size() ? accuracies[n] : -1.0f,
						n < altitudeAccuracies.size() ? altitudeAccuracies[n] : -1.0f,
						n < directionAccuracies.size() ? directionAccuracies[n] : -1.0f,
						n < speedAccuracies.size() ? speedAccuracies[n] : -1.0f);
				}

				Measurement::Metadata metadata;

				const vrs::DataPieceString* metadataValue = dataLayout.findDataPieceString("metadata");
				if (metadataValue != nullptr)
				{
					std::istringstream inputStream(metadataValue->get(), std::ios::binary);
					IO::InputBitstream bitstream(inputStream);

					if (!DeviceRecorder::RecordableGPSTrackerSample::readMetadataFromBitstream(bitstream, metadata))
					{
						ocean_assert(false && "Failed to read metadata!");
						return true;
					}
				}

				vrsGPSTracker_->forwardSampleEvent(VRSGPSTracker::ObjectIds(objectIds.cbegin(), objectIds.cend()), std::move(locations), referenceSystem, Timestamp(timestampValue->get()), std::move(metadata));
			}
		}
	}

	return true;
}

DevicePlayer::PlayableSceneTracker6DOFSample::PlayableSceneTracker6DOFSample()
{
	// nothing to do here
}

DevicePlayer::PlayableSceneTracker6DOFSample::~PlayableSceneTracker6DOFSample()
{
	vrsSceneTracker6DOF_.release();

	if (!deviceName_.empty())
	{
		Manager::get().unregisterAdhocDevice(deviceName_);
	}
}

bool DevicePlayer::PlayableSceneTracker6DOFSample::onDataLayoutRead(const vrs::CurrentRecord& header, size_t blockIndex, vrs::DataLayout& dataLayout)
{
	// The use of DataLayout::findDataPieceXXX an anti-pattern, please do not clone this bad practice.
	// See D39522991 or D39527762 for proper efficient use of datalayout objects.
	if (header.recordType == vrs::Record::Type::CONFIGURATION)
	{
		const vrs::DataPieceString* deviceNameValue = dataLayout.findDataPieceString("device_name");
		const vrs::DataPieceString* deviceTypeMajorValue = dataLayout.findDataPieceString("device_type_major");
		const vrs::DataPieceString* deviceTypeMinorValue = dataLayout.findDataPieceString("device_type_minor");

		if (deviceNameValue != nullptr && deviceTypeMajorValue != nullptr && deviceTypeMinorValue != nullptr)
		{
			std::string vrsDeviceName = deviceNameValue->get();

			if (vrsDeviceName.empty())
			{
				ocean_assert(false && "Invalid device name!");
				return true;
			}

			// we add 'VRS' a prefix to each device name to allow using the original device as well in the same session
			vrsDeviceName = "VRS " + vrsDeviceName;

			if (deviceName_ != vrsDeviceName)
			{
				ocean_assert(deviceName_.empty());
				deviceName_ = vrsDeviceName;

				const std::string deviceTypeMajor = deviceTypeMajorValue->get();
				const std::string deviceTypeMinor = deviceTypeMinorValue->get();

				const Device::DeviceType deviceType = Device::DeviceType::translateDeviceType(deviceTypeMajor, deviceTypeMinor);
				ocean_assert(deviceType);

				if (!Manager::get().registerAdhocDevice(deviceName_, deviceType, Manager::AdhocInstanceFunction::createStatic(&VRSFactory::createTracker6DOF)))
				{
					ocean_assert(false && "Failed to register device");
					return true;
				}

				Log::info() << "VRS contains 6DOF scene tracker '" << deviceName_ << "'";

				vrsSceneTracker6DOF_ = Manager::get().device(deviceName_, false /* useExclusive*/);
				ocean_assert(vrsSceneTracker6DOF_);
			}
		}
	}
	else
	{
		ocean_assert(header.recordType == vrs::Record::Type::DATA);

		if (vrsSceneTracker6DOF_.isNull())
		{
			return true;
		}

		const vrs::DataPieceValue<double>* timestampValue = dataLayout.findDataPieceValue<double>("timestamp");

		if (timestampValue == nullptr)
		{
			ocean_assert(false && "Invalid record!");
			return true;
		}

		const vrs::DataPieceStringMap<unsigned int>* objectDescriptionMapValue = dataLayout.findDataPieceStringMap<unsigned int>("object_description_map");

		if (objectDescriptionMapValue)
		{
			VRSTracker6DOF::VRSObjectDescriptionMap objectDescriptionMap;
			objectDescriptionMapValue->get(objectDescriptionMap);

			vrsSceneTracker6DOF_->updateVRSObjects(objectDescriptionMap);
		}

		VRSTracker6DOF::ObjectIdSet foundObjectIds;
		VRSTracker6DOF::ObjectIdSet lostObjectIds;

		std::vector<unsigned int> objectIds;

		const vrs::DataPieceVector<unsigned int>* foundObjectValue = dataLayout.findDataPieceVector<unsigned int>("found_objects");
		if (foundObjectValue != nullptr)
		{
			foundObjectValue->get(objectIds);

			foundObjectIds = VRSTracker6DOF::ObjectIdSet(objectIds.cbegin(), objectIds.cend());
		}

		const vrs::DataPieceVector<unsigned int>* lostObjectValue = dataLayout.findDataPieceVector<unsigned int>("lost_objects");
		if (lostObjectValue != nullptr)
		{
			lostObjectValue->get(objectIds);

			lostObjectIds = VRSTracker6DOF::ObjectIdSet(objectIds.cbegin(), objectIds.cend());
		}

		if (!foundObjectIds.empty() || !lostObjectIds.empty())
		{
			vrsSceneTracker6DOF_->forwardObjectEvent(foundObjectIds, lostObjectIds, Timestamp(timestampValue->get()));
		}

		const vrs::DataPieceVector<unsigned int>* objectIdsValue = dataLayout.findDataPieceVector<unsigned int>("object_ids");

		if (objectIdsValue != nullptr)
		{
			objectIdsValue->get(objectIds);

			if (!objectIds.empty())
			{
				const vrs::DataPieceString* referenceSystemValue = dataLayout.findDataPieceString("reference_system");
				const vrs::DataPieceVector<vrs::Matrix4Dd>* homogenousMatrices4Value = dataLayout.findDataPieceVector<vrs::Matrix4Dd>("homogenous_matrices_4");
				const vrs::DataPieceString* sceneElementsValue = dataLayout.findDataPieceString("scene_elements");

				if (referenceSystemValue == nullptr || homogenousMatrices4Value == nullptr || sceneElementsValue == nullptr)
				{
					ocean_assert(false && "Invalid input!");
					return true;
				}

				VRSTracker6DOF::ReferenceSystem referenceSystem = VRSTracker6DOF::RS_OBJECT_IN_DEVICE;

				if (referenceSystemValue->get() == "DEVICE_IN_OBJECT")
				{
					referenceSystem = VRSTracker6DOF::RS_DEVICE_IN_OBJECT;
				}
				else
				{
					ocean_assert(referenceSystemValue->get() == "OBJECT_IN_DEVICE");
				}

				std::vector<vrs::Matrix4Dd> matrices;
				homogenousMatrices4Value->get(matrices);

				if (matrices.size() != objectIds.size())
				{
					ocean_assert(false && "Invalid input!");
					return true;
				}

				Quaternions orientations;
				orientations.reserve(matrices.size());

				Vectors3 positions;
				positions.reserve(matrices.size());

				for (const vrs::Matrix4Dd& matrix : matrices)
				{
					const HomogenousMatrix4 homogenousMatrix((const double*)&matrix, true /* row aligned */);

					ocean_assert(homogenousMatrix.isValid());
					ocean_assert(homogenousMatrix.rotationMatrix().isOrthonormal(Numeric::weakEps()));

					orientations.emplace_back(homogenousMatrix.rotation());
					positions.emplace_back(homogenousMatrix.translation());
				}

				SceneTracker6DOF::SharedSceneElements sceneElements;

				{
					std::istringstream inputStream(sceneElementsValue->get(), std::ios::binary);
					IO::InputBitstream bitstream(inputStream);

					if (!DeviceRecorder::RecordableSceneTracker6DOFSample::readSceneElementsFromBitstream(bitstream, sceneElements))
					{
						ocean_assert(false && "Failed to read scene elements!");
						return true;
					}
				}

				if (objectIds.size() != sceneElements.size())
				{
					ocean_assert(false && "Invalid number of scene elements!");
					return true;
				}

				Measurement::Metadata metadata;

				const vrs::DataPieceString* metadataValue = dataLayout.findDataPieceString("metadata");
				if (metadataValue != nullptr)
				{
					std::istringstream inputStream(metadataValue->get(), std::ios::binary);
					IO::InputBitstream bitstream(inputStream);

					if (!DeviceRecorder::RecordableSceneTracker6DOFSample::readMetadataFromBitstream(bitstream, metadata))
					{
						ocean_assert(false && "Failed to read metadata!");
						return true;
					}
				}

				vrsSceneTracker6DOF_->forwardSampleEvent(VRSTracker6DOF::ObjectIds(objectIds.cbegin(), objectIds.cend()), std::move(orientations), std::move(positions), std::move(sceneElements), referenceSystem, Timestamp(timestampValue->get()), std::move(metadata));
			}
		}
	}

	return true;
}

DevicePlayer::PlayableHomogenousMatrix4::PlayableHomogenousMatrix4(const std::string& deviceName) :
	deviceName_(deviceName)
{
	// nothing to do here
}

DevicePlayer::PlayableHomogenousMatrix4::~PlayableHomogenousMatrix4()
{
	vrsTracker6DOF_.release();

	if (!deviceName_.empty())
	{
		Manager::get().unregisterAdhocDevice(deviceName_);
	}
}

bool DevicePlayer::PlayableHomogenousMatrix4::onDataLayoutRead(const vrs::CurrentRecord& header, size_t blockIndex, vrs::DataLayout& dataLayout)
{
	// The use of DataLayout::findDataPieceXXX an anti-pattern, please do not clone this bad practice.
	// See D39522991 or D39527762 for proper efficient use of datalayout objects.
	if (vrsTracker6DOF_.isNull() && !deviceName_.empty())
	{
		ocean_assert(!deviceName_.empty());

		const Device::DeviceType deviceType = Device::DeviceType(Devices::Device::DEVICE_TRACKER, Devices::Tracker::TRACKER_6DOF);
		ocean_assert(deviceType);

		if (!Manager::get().registerAdhocDevice(deviceName_, deviceType, Manager::AdhocInstanceFunction::createStatic(&VRSFactory::createTracker6DOF)))
		{
			ocean_assert(false && "Failed to register device");
			return true;
		}

		Log::info() << "VRS contains 6DOF tracker '" << deviceName_ << "'";

		vrsTracker6DOF_ = Manager::get().device(deviceName_, false /* useExclusive*/);
		ocean_assert(vrsTracker6DOF_);
	}

	if (header.recordType != vrs::Record::Type::DATA)
	{
		return true;
	}

	if (vrsTracker6DOF_.isNull())
	{
		return true;
	}

	const vrs::DataPieceValue<vrs::Matrix4Dd>* homogenousMatrix4DataPieceValue = dataLayout.findDataPieceValue<vrs::Matrix4Dd>("homogenous_matrix_4");
	const vrs::DataPieceValue<double>* timestampDataPieceValue = dataLayout.findDataPieceValue<double>("timestamp");

	HomogenousMatrixD4 homogenousMatrix4(false);
	double timestamp = NumericD::minValue();

	if (homogenousMatrix4DataPieceValue && timestampDataPieceValue)
	{
		const vrs::Matrix4Dd matrix = homogenousMatrix4DataPieceValue->get();

		homogenousMatrix4 = HomogenousMatrixD4((const double*)&matrix, true /* row aligned */);
		timestamp = timestampDataPieceValue->get();

		if (homogenousMatrix4.isValid())
		{
			if (objectId_ == VRSTracker6DOF::invalidObjectId())
			{
				objectId_ = VRSTracker6DOF::ObjectId(0);
				ocean_assert(objectId_ != VRSTracker6DOF::invalidObjectId());

				VRSTracker6DOF::VRSObjectDescriptionMap objectDescriptionMap;
				objectDescriptionMap.emplace("Transformation", objectId_);

				vrsTracker6DOF_->updateVRSObjects(objectDescriptionMap);

				vrsTracker6DOF_->forwardObjectEvent({objectId_}, {}, Timestamp(timestamp));
			}
		}

		return true;
	}

	if (homogenousMatrix4.isValid())
	{
		ocean_assert(objectId_ != VRSTracker6DOF::invalidObjectId());

		const Quaternion orientation(homogenousMatrix4.rotation());
		const Vector3 position(homogenousMatrix4.translation());

		vrsTracker6DOF_->forwardSampleEvent({objectId_}, {orientation}, {position}, VRSTracker6DOF::RS_DEVICE_IN_OBJECT, Timestamp(timestamp), VRSTracker6DOF::Metadata());
	}

	return true;
}

bool DevicePlayer::UsageManager::registerUsage()
{
	const ScopedLock scopedLock(lock_);

	if (isUsed_)
	{
		return false;
	}

	isUsed_ = true;

	return true;
}

void DevicePlayer::UsageManager::unregisterUsage()
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isUsed_);
	isUsed_ = false;
}

DevicePlayer::~DevicePlayer()
{
	stop();

	if (recordFileReader_)
	{
		UsageManager::get().unregisterUsage();
	}
}

bool DevicePlayer::loadRecording(const std::string& filename)
{
	const ScopedLock scopedLock(lock_);

#if defined(OCEAN_PLATFORM_BUILD_APPLE_MACOS) || defined(OCEAN_PLATFORM_BUILD_LINUX)

	static bool vrsVideoCodecsRegistered = false;
	if (!vrsVideoCodecsRegistered)
	{
		static Lock vrsLock;
		const ScopedLock vrsScopedLock(vrsLock);

		if (!vrsVideoCodecsRegistered)
		{
			vrs::utils::DecoderFactory::get().registerDecoderMaker(vrs::vxprs::xprsDecoderMaker);
			vrsVideoCodecsRegistered = true;
		}
	}
#endif // defined(OCEAN_PLATFORM_BUILD_APPLE_MACOS) || defined(OCEAN_PLATFORM_BUILD_LINUX)

	if (recordFileReader_)
	{
		return false;
	}

	if (!UsageManager::get().registerUsage())
	{
		Log::error() << "Already one DevicePlayer in use";
		return false;
	}

	recordFileReader_ = std::make_shared<vrs::RecordFileReader>();

	if (recordFileReader_->openFile(filename) != 0)
	{
		recordFileReader_ = nullptr;

		return false;
	}

	filename_ = filename;

	if (!analyzeRecording())
	{
		recordFileReader_ = nullptr;

		filename_.clear();

		return false;
	}

	return true;
}

bool DevicePlayer::start(const float speed)
{
	const ScopedLock scopedLock(lock_);

	if (isStarted_)
	{
		return true;
	}

	isStarted_ = true;

	speed_ = speed;

	if (speed > 0.0f)
	{
		startThread();
	}

	nextStopMotionRecordIndex_ = 0;

	return true;
}

bool DevicePlayer::stop()
{
	const ScopedLock scopedLock(lock_);

	if (!isStarted_)
	{
		return true;
	}

	stopThreadExplicitly();

	isStarted_ = false;

	return true;
}

Timestamp DevicePlayer::playNextFrame()
{
	const ScopedLock scopedLock(lock_);

	if (!isStarted_ || speed_ > 0.0f || !firstPlayableImageStreamId_)
	{
		return Timestamp(false);
	}

	ocean_assert(recordFileReader_);

	const std::vector<vrs::IndexRecord::RecordInfo>& recordInfos = recordFileReader_->getIndex();

	if (nextStopMotionRecordIndex_ >= recordInfos.size())
	{
		return Timestamp(false);
	}

	// we read all records before the next frame
	//                     current frame                   next frame
	// records:     |   |        |        |     |       |      |
	//                                                  ^
	//                                                  |
	//                                         last record to read

	Timestamp frameTimestamp(false);

	while (nextStopMotionRecordIndex_ < recordInfos.size())
	{
		const vrs::IndexRecord::RecordInfo& currentRecord = recordInfos[nextStopMotionRecordIndex_];

		if (currentRecord.streamId == *firstPlayableImageStreamId_)
		{
			if (currentRecord.recordType == vrs::Record::Type::DATA)
			{
				if (frameTimestamp.isValid())
				{
					if (currentRecord.timestamp > double(frameTimestamp))
					{
						// we already have seen the current frame, the current record contains the next frame, so we have read all records which can be of interest for the current frame
						break;
					}
				}
				else
				{
					frameTimestamp = currentRecord.timestamp;
				}
			}
		}

		recordFileReader_->readRecord(currentRecord);

		++nextStopMotionRecordIndex_;
	}

	return frameTimestamp;
}

bool DevicePlayer::isValid() const
{
	const ScopedLock scopedLock(lock_);

	return recordFileReader_ && recordFileReader_->isOpened();
}

bool DevicePlayer::analyzeRecording()
{
	ocean_assert(recordFileReader_);
	ocean_assert(recordFileReader_->isOpened());

	ocean_assert(playables_.empty());
	ocean_assert(frameMediums_.empty());

	firstPlayableImageStreamId_ = nullptr;

	const std::set<vrs::StreamId>& streamIds = recordFileReader_->getStreams();

	for (const vrs::StreamId& streamId : streamIds)
	{
		const std::string name = streamId.getName();
		const std::string& flavor = recordFileReader_->getFlavor(streamId);

		switch (streamId.getTypeId())
		{
			case vrs::RecordableTypeId::ForwardCameraRecordableClass:
			{
				if (flavor == "ocean/camera")
				{
					if (!firstPlayableImageStreamId_)
					{
						firstPlayableImageStreamId_ = std::make_shared<vrs::StreamId>(streamId);
					}

					playables_.emplace_back(std::make_shared<PlayableImage>());

					recordFileReader_->setStreamPlayer(streamId, playables_.back().get());
				}

				break;
			}

			case vrs::RecordableTypeId::PoseRecordableClass:
			{
				if (flavor == "ocean/devices/tracker6dofsample")
				{
					playables_.emplace_back(std::make_shared<PlayableTracker6DOFSample>());

					recordFileReader_->setStreamPlayer(streamId, playables_.back().get());
				}
				else if (flavor == "ocean/devices/scenetracker6dofsample")
				{
					playables_.emplace_back(std::make_shared<PlayableSceneTracker6DOFSample>());

					recordFileReader_->setStreamPlayer(streamId, playables_.back().get());
				}
				else if (flavor == "world_T_device")
				{
					playables_.emplace_back(std::make_shared<PlayableHomogenousMatrix4>("VRS World Tracker"));

					recordFileReader_->setStreamPlayer(streamId, playables_.back().get());
				}
				break;
			}

			case vrs::RecordableTypeId::GpsRecordableClass:
			{
				if (flavor == "ocean/devices/gpstrackersample")
				{
					playables_.emplace_back(std::make_shared<PlayableGPSTrackerSample>());

					recordFileReader_->setStreamPlayer(streamId, playables_.back().get());
				}
				break;
			}

			default:
				break;
		}
	}

	const std::vector<vrs::IndexRecord::RecordInfo>& recordInfos = recordFileReader_->getIndex();

	double firstTimestamp = NumericD::minValue();
	double lastTimestamp = NumericD::minValue();

	for (size_t n = 0; n < recordInfos.size(); ++n)
	{
		if (recordInfos[n].recordType == vrs::Record::Type::DATA)
		{
			firstTimestamp = recordInfos[n].timestamp;
			break;
		}
	}

	for (size_t n = recordInfos.size() - 1; n < recordInfos.size(); --n)
	{
		if (recordInfos[n].recordType == vrs::Record::Type::DATA)
		{
			lastTimestamp = recordInfos[n].timestamp;
			break;
		}
	}

	if (firstTimestamp == NumericD::minValue() || lastTimestamp == NumericD::minValue() || firstTimestamp > lastTimestamp)
	{
		return false;
	}

	duration_ = lastTimestamp - firstTimestamp;

	recordFileReader_->readFirstConfigurationRecords();

	for (const std::shared_ptr<vrs::StreamPlayer>& playable : playables_)
	{
		const PlayableImage* playableImage = dynamic_cast<PlayableImage*>(playable.get());

		if (playableImage)
		{
			frameMediums_.emplace_back(playableImage->pixelImage());
		}
	}

	return true;
}

void DevicePlayer::threadRun()
{
	ocean_assert(recordFileReader_);
	ocean_assert(speed_ > 0.0f);

	const std::vector<vrs::IndexRecord::RecordInfo>& recordInfos = recordFileReader_->getIndex();

	if (recordInfos.empty())
	{
		return;
	}

	const Timestamp playerAbsoluteStartTimestamp(true);
	Timestamp recordRelativeStartTimestamp(false);

	for (size_t n = 0; n < recordInfos.size(); ++n)
	{
		if (recordInfos[n].recordType == vrs::Record::Type::DATA)
		{
			recordRelativeStartTimestamp = Timestamp(recordInfos[n].timestamp);
			break;
		}
	}

	if (recordRelativeStartTimestamp.isInvalid())
	{
		return;
	}

	size_t recordIndex = 0;

	while (recordIndex < recordInfos.size() && !shouldThreadStop())
	{
		const vrs::IndexRecord::RecordInfo& currentRecord = recordInfos[recordIndex];

		const Timestamp playerAbsoluteCurrentTimestamp(true);
		const Timestamp recordRelativeCurrentTimestamp(currentRecord.timestamp);

		const double expectedTimeSinceStart = double(recordRelativeCurrentTimestamp - recordRelativeStartTimestamp);

		const Timestamp recordAbsoluteTimestamp(playerAbsoluteStartTimestamp + expectedTimeSinceStart / double(speed_));

		const double recordDelayMs = double(recordAbsoluteTimestamp - playerAbsoluteCurrentTimestamp) * 1000.0;

		if (recordDelayMs >= 0.5 && (!replayTimestampProviderStreamId_ || *replayTimestampProviderStreamId_ == currentRecord.streamId))
		{
			sleep((unsigned int)(recordDelayMs + 0.5));
		}

		recordFileReader_->readRecord(currentRecord);

		++recordIndex;
	}

	isStarted_ = false;
}

}

}

}
