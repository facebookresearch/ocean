// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/vrs/VRSFrameMedium.h"
#include "ocean/media/vrs/VRSFiniteMedium.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"

#include "ocean/io/CameraCalibrationManager.h"

#include "ocean/system/Performance.h"

#include <perception/sensor_calibration_io/CameraCalibrationImport.h>

namespace Ocean
{

namespace Media
{

namespace VRS
{

VRSFrameMedium::ImagePlayable::ImagePlayable(VRSFrameMedium* owner) :
	owner_(owner),
	explicitTimestamp_(NumericD::minValue())
{
	ocean_assert(owner_ != nullptr);
}

bool VRSFrameMedium::ImagePlayable::onDataLayoutRead(const vrs::CurrentRecord& header, size_t /*blockIndex*/, vrs::DataLayout& dataLayout)
{
	// The use of DataLayout::findDataPieceXXX an anti-pattern, please do not clone this bad practice.
	// See D39522991 or D39527762 for proper efficient use of datalayout objects.
	if (header.recordType == vrs::Record::Type::CONFIGURATION)
	{
		if (header.streamId.getTypeId() == vrs::RecordableTypeId::FacebookARCamera)
		{
			const vrs::DataPieceValue<unsigned int>* imageWidthValue = dataLayout.findDataPieceValue<unsigned int>("image_width");
			const vrs::DataPieceValue<unsigned int>* imageHeightValue = dataLayout.findDataPieceValue<unsigned int>("image_height");
			const vrs::DataPieceValue<float>* fieldOfViewRadiansValue = dataLayout.findDataPieceValue<float>("field_of_view_radians");

			const vrs::DataPieceValue<int32_t>* rotationValue = dataLayout.findDataPieceValue<int32_t>("rotation");
			const vrs::DataPieceValue<vrs::Bool>* flipVerticallyValue = dataLayout.findDataPieceValue<vrs::Bool>("flip_vertically");
			const vrs::DataPieceValue<int32_t>* cameraSensorRotationValue = dataLayout.findDataPieceValue<int32_t>("camera_sensor_rotation");

			if (rotationValue)
			{
				ocean_assert(rotationValue->get() == 0);
			}

			if (flipVerticallyValue)
			{
				ocean_assert(flipVerticallyValue->get() == false);
			}

			if (cameraSensorRotationValue)
			{
				ocean_assert(cameraSensorRotationValue->get() == 0);
			}

			if (imageWidthValue != nullptr && imageHeightValue != nullptr && fieldOfViewRadiansValue != nullptr)
			{
				const Scalar fovX = Scalar(fieldOfViewRadiansValue->get()); // 'field_of_view_radians' is not documented so we "hope" it is horizontal field of view

				const PinholeCamera camera(imageWidthValue->get(), imageHeightValue->get(), fovX);
				const HomogenousMatrixD4 device_T_camera(true);

				owner_->onNewCameraCalibration(std::make_shared<AnyCameraPinhole>(camera), device_T_camera);
			}
		}
		else
		{
			const vrs::DataPieceValue<unsigned int>* cameraIdDataPieceValue = dataLayout.findDataPieceValue<unsigned int>("camera_id");

			if (cameraIdDataPieceValue == nullptr)
			{
				// the VRS files does not contain a camera calibration which we can read
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

				ocean_assert(owner_ != nullptr);
				owner_->onNewCameraCalibration(std::make_shared<AnyCameraFisheye>(FisheyeCamera(fisheyeCamera)), device_T_camera);
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

				const PinholeCamera camera(width, height, focalX, focalY, principalX, principalY, radialDistortion, tangentialDistortion);

				ocean_assert(owner_ != nullptr);
				owner_->onNewCameraCalibration(std::make_shared<AnyCameraPinhole>(camera), device_T_camera);
			}
			else
			{
				ocean_assert(false && "This camera model is not supported!");
				return true;
			}
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

bool VRSFrameMedium::ImagePlayable::onImageRead(const vrs::CurrentRecord& header, size_t /*index*/, const vrs::ContentBlock& block)
{
	const vrs::ImageContentBlockSpec& imageContentBlockSpec = block.image();

	if (imageContentBlockSpec.getImageFormat() == vrs::ImageFormat::RAW)
	{
		const unsigned int width = imageContentBlockSpec.getWidth();
		const unsigned int height = imageContentBlockSpec.getHeight();

		const unsigned int strideBytes = imageContentBlockSpec.getStride();

		FrameType::PixelFormat pixelFormat = FrameType::FORMAT_UNDEFINED;

		switch (imageContentBlockSpec.getPixelFormat())
		{
			case vrs::PixelFormat::GREY8:
				pixelFormat = FrameType::FORMAT_Y8;
				break;

			case vrs::PixelFormat::GREY10:
				pixelFormat = FrameType::FORMAT_Y10;
				break;

			case vrs::PixelFormat::BGR8:
				pixelFormat = FrameType::FORMAT_BGR24;
				break;

			case vrs::PixelFormat::DEPTH32F:
				pixelFormat = FrameType::genericPixelFormat<float, 1u>();
				break;

			case vrs::PixelFormat::RGB8:
				pixelFormat = FrameType::FORMAT_RGB24;
				break;

			case vrs::PixelFormat::RGBA8:
				pixelFormat = FrameType::FORMAT_RGBA32;
				break;

			case vrs::PixelFormat::RAW10:
				pixelFormat = FrameType::FORMAT_Y10_PACKED;
				break;

			case vrs::PixelFormat::RAW10_BAYER_RGGB:
				pixelFormat = FrameType::FORMAT_RGGB10_PACKED;
				break;

			case vrs::PixelFormat::YUV_I420_SPLIT:
				pixelFormat = FrameType::FORMAT_Y_U_V12;
				break;

			default:
				break;
		}

		ocean_assert(pixelFormat != FrameType::FORMAT_UNDEFINED);
		if (pixelFormat != FrameType::FORMAT_UNDEFINED)
		{
			ocean_assert(imageContentBlockSpec.getChannelCountPerPixel() == FrameType::channels(pixelFormat));

			Frame frame;

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

			ocean_assert(frame.isValid());

			const Timestamp frameTimestamp = explicitTimestamp_ != NumericD::minValue() ? Timestamp(explicitTimestamp_) : Timestamp(header.timestamp);
			explicitTimestamp_ = NumericD::minValue(); // resetting the explicit timestamp

			frame.setTimestamp(frameTimestamp);

			ocean_assert(owner_ != nullptr);
			owner_->onNewFrame(std::move(frame), double(frameTimestamp));
		}
	}

	return true;
}

VRSFrameMedium::VRSFrameMedium(const std::string& url) :
	Medium(url),
	VRSMedium(url),
	FrameMedium(url),
	imagePlayable_(this)
{
	const unsigned int frameCollectionFrames = System::Performance::get().performanceLevel() > System::Performance::LEVEL_MEDIUM ? 300u : 30u;

	frameCollection_ = FrameCollection(frameCollectionFrames);
}

VRSFrameMedium::~VRSFrameMedium()
{
	frameCollection_.release();
}

void VRSFrameMedium::onNewCameraCalibration(SharedAnyCamera anyCamera, const HomogenousMatrixD4& device_T_camera)
{
	const ScopedLock scopedLock(lock_);

	anyCamera_ = std::move(anyCamera);

	device_T_camera_ = device_T_camera;
}

void VRSFrameMedium::onNewFrame(Frame&& frame, const double vrsTimestamp)
{
	Timestamp desiredUnixTimestamp;

	if (previousVRSFrameTimestamp_.isInvalid() || respectPlaybackTime_ == false)
	{
		desiredUnixTimestamp.toNow();
	}
	else
	{
		ocean_assert(speed_ > 0.0f);
		ocean_assert(previousUnixFrameTimestamp_.isValid());

		desiredUnixTimestamp = double(previousUnixFrameTimestamp_ + (vrsTimestamp - double(previousVRSFrameTimestamp_))) / double(speed_);
	}

	Timestamp currentUnixTimestamp(true);

	while (desiredUnixTimestamp > currentUnixTimestamp)
	{
		const double msToSleep = double(desiredUnixTimestamp - currentUnixTimestamp) * 500.0; // 50% of the actual ms

		if (msToSleep >= 0.0)
		{
			Thread::sleep((unsigned int)(msToSleep));
		}

		currentUnixTimestamp.toNow();
	}

	frame.setTimestamp(Timestamp(vrsTimestamp));

	const FrameRef lastFrame = this->frame();

	if (lastFrame && lastFrame->timestamp() >= frame.timestamp())
	{
		Log::warning() << "Invalid frame timestamp in `" << url() << "`";

		// we stop the medium, as we cannot guarantee that the remaining information is correct
		stop();

		return;
	}

	deliverNewFrame(std::move(frame), SharedAnyCamera(anyCamera_));

	previousUnixFrameTimestamp_ = desiredUnixTimestamp;
	previousVRSFrameTimestamp_ = vrsTimestamp;
}

bool VRSFrameMedium::setRespectPlaybackTime(const bool respectPlaybackTime)
{
	if (respectPlaybackTime_ == respectPlaybackTime)
	{
		return true;
	}

	if (startTimestamp_.isValid())
	{
		// the medium is already running
		return false;
	}

	respectPlaybackTime_ = respectPlaybackTime;

	return true;
}

}

}

}
