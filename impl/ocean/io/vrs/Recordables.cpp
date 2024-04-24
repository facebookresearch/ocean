// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/io/vrs/Recordables.h"
#include "ocean/io/vrs/Utilities.h"

#include "ocean/base/Memory.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/io/Directory.h"
#include "ocean/io/File.h"

#include "ocean/math/AnyCameraPerception.h"

#include <perception/camera_model_helpers/CalibrationConversions.h>

#include <perception/sensor_calibration_io/CameraCalibration.h>
#include <perception/sensor_calibration_io/CameraCalibrationExport.h>
#include <perception/sensor_calibration_io/CameraCalibrationImport.h>
#include <perception/sensor_calibration_io/DeviceInfo.h>
#include <perception/sensor_calibration_io/Metadata.h>

namespace Ocean
{

namespace IO
{

namespace VRS
{

Recordable::Recordable(vrs::RecordableTypeId typeId, const std::string& flavor, const Timestamp& vrsRecordStartingTimestamp) :
	vrs::Recordable(typeId, flavor),
	vrsRecordTimestamp_(vrsRecordStartingTimestamp)
{
	// we select a compression which creates larger files but does not steel CPU capacity for compression
	setCompression(vrs::CompressionPreset::Lz4Fast);
}

void Recordable::release()
{
	// should be implemented in derived classes
}

bool Recordable::isValid() const
{
	// should be implemented in derived classes
	return true;
}

RecordableCamera::RecordableCamera(const vrs::RecordableTypeId recordableTypeId, const std::string& flavor, const Timestamp& vrsRecordStartingTimestamp) :
	Recordable(recordableTypeId, flavor, vrsRecordStartingTimestamp)
{
	release();

	addRecordFormat(vrs::Record::Type::CONFIGURATION, configurationLayoutVersion_, configurationLayout_.getContentBlock(), {&configurationLayout_});

	addRecordFormat(vrs::Record::Type::DATA, dataLayoutVersion_, dataLayout_.getContentBlock() + vrs::ContentBlock(vrs::ImageFormat::RAW), {&dataLayout_});
}

bool RecordableCamera::addData(const uint32_t cameraId, const SharedAnyCameraD& anyCamera, const Frame& frame, const HomogenousMatrixD4& device_T_camera, const Timestamp& frameTimestamp, const Timestamp& recordTimestamp)
{
	ocean_assert(anyCamera && anyCamera->isValid());
	ocean_assert(frame.isValid());

	if (!anyCamera || !frame.isValid() || anyCamera->width() != frame.width() || anyCamera->height() != frame.height())
	{
		ocean_assert(false && "Camera and frame must be valid and must have the same resolution!");
		return false;
	}

	constexpr unsigned int planeIndex = 0u; // VRS always expects stride for the first plane (stride for multipe planes are ignored)

	if (!setCamera(cameraId, anyCamera->width(), anyCamera->height(), anyCamera, frame.pixelFormat(), frame.planeWidthBytes(planeIndex), device_T_camera, recordTimestamp))
	{
		return false;
	}

	const Timestamp timestamp = frameTimestamp.isValid() ? frameTimestamp : frame.timestamp();

	const double vrsRecordTimestamp = double(recordTimestamp);

	if (frame.pixelFormat() == FrameType::FORMAT_Y_U_V12 || frame.pixelFormat() == FrameType::FORMAT_Y_UV12)
	{
		// special handling for Y_U_V12 as we need the memory as one block, as VRS does not support multiple planes (and actally expects one memory block)

		Memory memoryForY_U_V12(frame.width() * (frame.height() + frame.height() / 2u));

		const Frame::PlaneInitializers<uint8_t> planeInitializers =
		{
			Frame::PlaneInitializer<uint8_t>(memoryForY_U_V12.data<uint8_t>(), Frame::CM_USE_KEEP_LAYOUT, 0u),
			Frame::PlaneInitializer<uint8_t>(memoryForY_U_V12.data<uint8_t>() + frame.width() * frame.height(), Frame::CM_USE_KEEP_LAYOUT, 0u),
			Frame::PlaneInitializer<uint8_t>(memoryForY_U_V12.data<uint8_t>() + frame.width() * frame.height() + frame.width() * frame.height() / 4u, Frame::CM_USE_KEEP_LAYOUT, 0u)
		};

		Frame onePlaneFrameY_U_V12(FrameType(frame, FrameType::FORMAT_Y_U_V12, FrameType::ORIGIN_UPPER_LEFT), planeInitializers);

		if (!CV::FrameConverter::Comfort::convertAndCopy(frame, onePlaneFrameY_U_V12))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		ocean_assert(onePlaneFrameY_U_V12.constdata<void>() == memoryForY_U_V12.data());

		addData(onePlaneFrameY_U_V12.constdata<void>(), onePlaneFrameY_U_V12.frameTypeSize(), double(timestamp), recordTimestamp.isValid() ? &vrsRecordTimestamp : nullptr);
	}
	else
	{
		// ensuring that the pixel origin is upper left

		Frame convertedFrame;
		if (!CV::FrameConverter::Comfort::convert(frame, FrameType::ORIGIN_UPPER_LEFT, convertedFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		ocean_assert(convertedFrame.numberPlanes() == 1u);

		convertedFrame.makeContinuous();
		ocean_assert(convertedFrame.size() == convertedFrame.frameTypeSize());

		addData(convertedFrame.constdata<void>(), convertedFrame.size(), double(timestamp), recordTimestamp.isValid() ? &vrsRecordTimestamp : nullptr);
	}

	return true;
}

bool RecordableCamera::setCamera(const uint32_t cameraId, const SharedAnyCameraD& anyCamera, const FrameType::PixelFormat pixelFormat, const unsigned int strideBytes, const HomogenousMatrix4& device_T_camera, const Timestamp& recordTimestamp)
{
	ocean_assert(cameraId != uint32_t(-1));
	ocean_assert(anyCamera && anyCamera->isValid());
	ocean_assert(strideBytes >= anyCamera->width());

	return setCamera(cameraId, anyCamera->width(), anyCamera->height(), anyCamera, pixelFormat, strideBytes, HomogenousMatrixD4(device_T_camera), recordTimestamp);
}

bool RecordableCamera::setCamera(const uint32_t cameraId, const unsigned int width, const unsigned int height, std::shared_ptr<AnyCameraD> anyCamera, const FrameType::PixelFormat pixelFormat, const unsigned int strideBytes, const HomogenousMatrixD4& device_T_camera, const Timestamp& recordTimestamp)
{
	ocean_assert(cameraId != uint32_t(-1) && width != 0u && height != 0u && strideBytes >= width);
	ocean_assert(strideBytes != 0u);

	if (cameraId == uint32_t(-1) || width == 0u || height == 0u || strideBytes < width)
	{
		return false;
	}

	vrs::PixelFormat vrsPixelFormat;

	switch ((std::underlying_type<FrameType::PixelFormat>::type)(pixelFormat))
	{
		case FrameType::FORMAT_Y8:
		case FrameType::genericPixelFormat<uint8_t, 1u>():
			vrsPixelFormat = vrs::PixelFormat::GREY8;
			break;

		case FrameType::FORMAT_Y10:
			vrsPixelFormat = vrs::PixelFormat::GREY10;
			break;

		case FrameType::FORMAT_Y10_PACKED:
			vrsPixelFormat = vrs::PixelFormat::RAW10;
			break;

		case FrameType::FORMAT_Y16:
		case FrameType::genericPixelFormat<uint16_t, 1u>():
			vrsPixelFormat = vrs::PixelFormat::GREY16;
			break;

		case FrameType::FORMAT_BGR24:
			vrsPixelFormat = vrs::PixelFormat::BGR8;
			break;

		case FrameType::FORMAT_F32:
		case FrameType::genericPixelFormat<float, 1u>():
			vrsPixelFormat = vrs::PixelFormat::DEPTH32F;
			break;

		case FrameType::FORMAT_F64:
		case FrameType::genericPixelFormat<double, 1u>():
			vrsPixelFormat = vrs::PixelFormat::SCALAR64F;
			break;

		case FrameType::FORMAT_RGB24:
		case FrameType::genericPixelFormat<uint8_t, 3u>():
			vrsPixelFormat = vrs::PixelFormat::RGB8;
			break;

		case FrameType::FORMAT_RGBA32:
		case FrameType::genericPixelFormat<uint8_t, 4u>():
			vrsPixelFormat = vrs::PixelFormat::RGBA8;
			break;

		case FrameType::FORMAT_Y_UV12:
		case FrameType::FORMAT_Y_U_V12:
		{
			// special handling, VRS expects the stride for the first plane only
			// however, the entire memory must be given as one memory block only, so that we do not allow any memory padding at all

			if (width < 2u || height < 2u || width % 2u != 0u || height % 2u != 0u)
			{
				ocean_assert(false && "Invalid image resolution!");
				return false;
			}

			if (strideBytes != width)
			{
				ocean_assert(false && "No padding allowed in any plane!");
				return false;
			}

			vrsPixelFormat = vrs::PixelFormat::YUV_I420_SPLIT;
			break;
		}

		case FrameType::FORMAT_RGGB10_PACKED:
			vrsPixelFormat = vrs::PixelFormat::RAW10_BAYER_RGGB;
			break;

		default:
			ocean_assert(false && "Pixel format not supported!");
			return false;
	}

	bool valuesHaveChanges = false;

	if (vrsPixelFormat_ != vrsPixelFormat)
	{
		vrsPixelFormat_ = vrsPixelFormat;
		valuesHaveChanges = true;
	}

	if (cameraId_ != cameraId)
	{
		cameraId_ = cameraId;
		valuesHaveChanges = true;
	}

	if (cameraWidth_ != width ||  cameraHeight_ != height)
	{
		cameraWidth_ = width;
		cameraHeight_ = height;
		valuesHaveChanges = true;
	}

	if (anyCamera)
	{
		if (!anyCamera_ || (anyCamera_ && !anyCamera_->isEqual(*anyCamera)))
		{
			anyCamera_ = std::move(anyCamera);
			valuesHaveChanges = true;
		}
	}

	if (strideBytes_ != strideBytes)
	{
		strideBytes_ = strideBytes;
		valuesHaveChanges = true;
	}

	if (device_T_camera_ != device_T_camera)
	{
		device_T_camera_ = device_T_camera;
		valuesHaveChanges = true;
	}

	if (!valuesHaveChanges)
	{
		return true;
	}

	if (recordTimestamp.isValid())
	{
		vrsRecordTimestamp_ = recordTimestamp;
	}

	return createConfigurationRecord() != nullptr;
}

void RecordableCamera::addData(const void* frame, const unsigned int size, const double timestamp, const double* recordTimestamp)
{
	ocean_assert(frame != nullptr || size == 0u);
	ocean_assert(vrsPixelFormat_ != vrs::PixelFormat::YUV_I420_SPLIT || size == cameraWidth_ * cameraHeight_ + cameraWidth_ * cameraHeight_ / 2u);

	dataLayout_.timestamp.set(timestamp);

	const Timestamp vrsRecordTimestamp = recordTimestamp != nullptr ? Timestamp(*recordTimestamp) : Timestamp(true);
	createRecord(double(vrsRecordTimestamp), vrs::Record::Type::DATA, dataLayoutVersion_, vrs::DataSource(dataLayout_, {frame, size}));
}

void RecordableCamera::release()
{
	cameraId_ = uint32_t(-1);

	anyCamera_ = nullptr;

	vrsPixelFormat_ = vrs::PixelFormat::UNDEFINED;

	strideBytes_ = 0u;

	device_T_camera_.toNull();
}

bool RecordableCamera::isValid() const
{
	return anyCamera_ && anyCamera_->isValid() && vrsPixelFormat_ != vrs::PixelFormat::UNDEFINED && strideBytes_ >= anyCamera_->width();
}

std::string RecordableCamera::defaultFlavor()
{
	return "ocean/camera";
}

bool RecordableCamera::exportCameraToPerceptionJSON(const AnyCameraD& anyCamera, const HomogenousMatrixD4& device_T_camera, std::string& perceptionJsonCalibration)
{
	if (!anyCamera.isValid() || !device_T_camera.isValid())
	{
		ocean_assert(false && "Invalid camera or transformation!");
		return false;
	}

	perception::sensor_calibration_io::CameraCalibration perceptionCameraCalibration;
	perceptionCameraCalibration.cameraName = anyCamera.name() + " 0"; // perception's cameraName seems to be more an id than a name, adding an id so that it can be imported again
	perceptionCameraCalibration.width = int(anyCamera.width());
	perceptionCameraCalibration.height = int(anyCamera.height());

	const HomogenousMatrixD4 device_T_flippedCamera(PinholeCamera::flippedTransformationRightSide(device_T_camera));

	const VectorD3 translation(device_T_flippedCamera.translation());
	memcpy(perceptionCameraCalibration.deviceFromCameraTranslation, translation.data(), sizeof(double) * 3);

	device_T_flippedCamera.rotationMatrix().copyElements((double*)perceptionCameraCalibration.deviceFromCameraRotation, true /* row aligned */);

	if (anyCamera.name() == AnyCameraPinholeD::WrappedCamera::name())
	{
		const PinholeCameraD& camera = ((const AnyCameraPinholeD&)(anyCamera)).actualCamera();

		perceptionCameraCalibration.projectionModel = perception::sensor_calibration_io::CameraCalibration::ProjectionModel::PINHOLE;
		perceptionCameraCalibration.distortionModel = perception::sensor_calibration_io::CameraCalibration::DistortionModel::PLANAR;

		perceptionCameraCalibration.projectionCoefficients = {double(camera.focalLengthX()), double(camera.focalLengthY()), double(camera.principalPointX()), double(camera.principalPointY())};
		perceptionCameraCalibration.distortionCoefficients = {double(camera.radialDistortion().first), double(camera.radialDistortion().second), double(camera.tangentialDistortion().first), double(camera.tangentialDistortion().second)};
	}
	else if (anyCamera.name() == AnyCameraFisheyeD::WrappedCamera::name())
	{
		const FisheyeCameraD& camera = ((const AnyCameraFisheyeD&)(anyCamera)).actualCamera();

		ocean_assert(NumericD::isEqual(camera.focalLengthX(), camera.focalLengthY()));

		perceptionCameraCalibration.projectionModel = perception::sensor_calibration_io::CameraCalibration::ProjectionModel::PINHOLE_SYMMETRIC;
		perceptionCameraCalibration.distortionModel = perception::sensor_calibration_io::CameraCalibration::DistortionModel::FISHEYE62;

		perceptionCameraCalibration.projectionCoefficients = {camera.focalLengthX(), camera.principalPointX(), camera.principalPointY()};
		perceptionCameraCalibration.distortionCoefficients = {camera.radialDistortion()[0], camera.radialDistortion()[1], camera.radialDistortion()[2], camera.radialDistortion()[3], camera.radialDistortion()[4], camera.radialDistortion()[5], camera.tangentialDistortion()[0], camera.tangentialDistortion()[1]};
	}
	else if (anyCamera.name() == AnyCameraPerceptionD::WrappedCamera::name())
	{
		const std::shared_ptr<perception::CameraModelInterface<double>>& cameraModel = ((const AnyCameraPerceptionD&)(anyCamera)).actualCamera();

		perception::ParsedCameraInfo parsedCameraInfo;
		parsedCameraInfo.cameraId = int32_t(0);
		parsedCameraInfo.imageWidth = int32_t(anyCamera.width());
		parsedCameraInfo.imageHeight = int32_t(anyCamera.height());
		parsedCameraInfo.cameraModel.model = cameraModel->clone();

		const std::string sensorModel = "unknown";

		bool success = false;
		const perception::sensor_calibration_io::CameraCalibration localCameraCalibration = perception::parsedCameraInfoToCameraCalibration(parsedCameraInfo, *cameraModel, sensorModel, success, true /*populateLookupTable*/);

		if (!success)
		{
			return false;
		}

		perceptionCameraCalibration.projectionModel = localCameraCalibration.projectionModel;
		perceptionCameraCalibration.distortionModel = localCameraCalibration.distortionModel;

		perceptionCameraCalibration.projectionCoefficients = localCameraCalibration.projectionCoefficients;
		perceptionCameraCalibration.distortionCoefficients = localCameraCalibration.distortionCoefficients;

		perceptionCameraCalibration.lookupTableSpacingPx = localCameraCalibration.lookupTableSpacingPx;
		perceptionCameraCalibration.lookupTableData = localCameraCalibration.lookupTableData;
	}
	else
	{
		ocean_assert(false && "This camera model not yet supported!");

		return false;
	}

	perception::sensor_calibration_io::DeviceInfo deviceInfo;
	deviceInfo.deviceType = "Unknown";

	perception::sensor_calibration_io::Metadata metadata;
	perceptionJsonCalibration = perception::sensor_calibration_io::exportToJson(deviceInfo, {perceptionCameraCalibration}, metadata);

	return !perceptionJsonCalibration.empty();
}

bool RecordableCamera::importCameraFromPerceptionJSON(const std::string& perceptionJsonCalibration, SharedAnyCameraD& anyCamera, HomogenousMatrixD4& device_T_camera)
{
	if (perceptionJsonCalibration.empty())
	{
		ocean_assert(false && "Invalid input!");
		return false;
	}

	std::map<int, perception::sensor_calibration_io::CameraCalibration> parsedCalibrations;
	if (!perception::sensor_calibration_io::parseFromJson(perceptionJsonCalibration, parsedCalibrations) || parsedCalibrations.size() != 1)
	{
		return false;
	}

	const perception::sensor_calibration_io::CameraCalibration& cameraCalibration = parsedCalibrations.cbegin()->second;

	if (cameraCalibration.width <= 0 || cameraCalibration.height <= 0)
	{
		return false;
	}

	const unsigned int width = (unsigned int)(cameraCalibration.width);
	const unsigned int height = (unsigned int)(cameraCalibration.height);

	if (cameraCalibration.projectionModel == perception::sensor_calibration_io::CameraCalibration::ProjectionModel::PINHOLE
			&& cameraCalibration.distortionModel == perception::sensor_calibration_io::CameraCalibration::DistortionModel::PLANAR)
	{
		if (cameraCalibration.projectionCoefficients.size() == 4 && cameraCalibration.distortionCoefficients.size() == 4)
		{
			// fx, fy, mx, my

			const double fx = cameraCalibration.projectionCoefficients[0];
			const double fy = cameraCalibration.projectionCoefficients[1];

			const double mx = cameraCalibration.projectionCoefficients[2];
			const double my = cameraCalibration.projectionCoefficients[3];

			const PinholeCameraD::DistortionPair radialDistortionPair(cameraCalibration.distortionCoefficients[0], cameraCalibration.distortionCoefficients[1]);
			const PinholeCameraD::DistortionPair tangentialDistortionPair(cameraCalibration.distortionCoefficients[2], cameraCalibration.distortionCoefficients[3]);

			anyCamera = std::make_shared<AnyCameraPinholeD>(PinholeCameraD(width, height, fx, fy, mx, my, radialDistortionPair, tangentialDistortionPair));
		}
		else
		{
			return false;
		}
	}
	else if (cameraCalibration.projectionModel == perception::sensor_calibration_io::CameraCalibration::ProjectionModel::PINHOLE_SYMMETRIC
				&& cameraCalibration.distortionModel == perception::sensor_calibration_io::CameraCalibration::DistortionModel::FISHEYE62)
	{
		const double& focal = cameraCalibration.projectionCoefficients[0];
		const double& mx = cameraCalibration.projectionCoefficients[1];
		const double& my = cameraCalibration.projectionCoefficients[2];

		const double* radialDistortion = cameraCalibration.distortionCoefficients.data();
		const double* tangentialDistortion = cameraCalibration.distortionCoefficients.data() + 6;

		anyCamera = std::make_shared<AnyCameraFisheyeD>(FisheyeCameraD(width, height, focal, focal, mx, my, radialDistortion, tangentialDistortion));
	}
	else
	{
		ocean_assert(false && "Missing implementation!");
		return false;
	}

	const VectorD3 device_t_flippedCamera(cameraCalibration.deviceFromCameraTranslation);
	const SquareMatrixD3 device_r_flippedCamera((double*)(cameraCalibration.deviceFromCameraRotation), true /*row aligned*/);

	const HomogenousMatrixD4 device_T_flippedCamera(device_t_flippedCamera, device_r_flippedCamera);

	if (!device_T_flippedCamera.isValid())
	{
		return false;
	}

	device_T_camera = PinholeCamera::flippedTransformationRightSide(device_T_flippedCamera);

	return true;
}

const vrs::Record* RecordableCamera::createConfigurationRecord()
{
	if (!isValid())
	{
		// the recordable is not yet configured (e.g., because the camera profile/image resolution is still unknown)
		return nullptr;
	}

	ocean_assert(cameraWidth_ != 0u && cameraHeight_ != 0u);

	HomogenousMatrixD4 device_T_camera(true);
	HomogenousMatrixD4 device_T_flippedCamera(true);

	if (device_T_camera_.isValid())
	{
		device_T_camera = HomogenousMatrixD4(VectorD3(device_T_camera_.translation()), QuaternionD(device_T_camera_.rotation()).normalized()); // separation into translation and rotation to handle precission issues between float and double
		device_T_flippedCamera = PinholeCamera::flippedTransformationRightSide(device_T_camera);
	}

	perception::sensor_calibration_io::CameraCalibration perceptionCameraCalibration;
	perceptionCameraCalibration.cameraName = String::toAString(cameraId_);
	perceptionCameraCalibration.width = int(cameraWidth_);
	perceptionCameraCalibration.height = int(cameraHeight_);

	const VectorD3 translation(device_T_flippedCamera.translation());
	memcpy(perceptionCameraCalibration.deviceFromCameraTranslation, translation.data(), sizeof(double) * 3);

	device_T_flippedCamera.rotationMatrix().copyElements((double*)(perceptionCameraCalibration.deviceFromCameraRotation), true /* row aligned */);

	std::vector<float> cameraCalibrationValues;

	if (anyCamera_)
	{
		if (anyCamera_->name() == AnyCameraPinholeD::WrappedCamera::name())
		{
			const PinholeCameraD& camera = ((const AnyCameraPinholeD&)(*anyCamera_)).actualCamera();

			perceptionCameraCalibration.projectionModel = perception::sensor_calibration_io::CameraCalibration::ProjectionModel::PINHOLE;
			perceptionCameraCalibration.distortionModel = perception::sensor_calibration_io::CameraCalibration::DistortionModel::PLANAR;

			perceptionCameraCalibration.projectionCoefficients = {double(camera.focalLengthX()), double(camera.focalLengthY()), double(camera.principalPointX()), double(camera.principalPointY())};
			perceptionCameraCalibration.distortionCoefficients = {double(camera.radialDistortion().first), double(camera.radialDistortion().second), double(camera.tangentialDistortion().first), double(camera.tangentialDistortion().second)};

			cameraCalibrationValues = {float(camera.focalLengthX()), float(camera.focalLengthY()), float(camera.principalPointX()), float(camera.principalPointY()), float(camera.radialDistortion().first), float(camera.radialDistortion().second), float(camera.tangentialDistortion().first), float(camera.tangentialDistortion().second)};
			ocean_assert(cameraCalibrationValues.size() == 8);
		}
		else if (anyCamera_->name() == AnyCameraFisheyeD::WrappedCamera::name())
		{
			const FisheyeCameraD& camera = ((const AnyCameraFisheyeD&)(*anyCamera_)).actualCamera();

			ocean_assert(NumericD::isEqual(camera.focalLengthX(), camera.focalLengthY()));

			perceptionCameraCalibration.projectionModel = perception::sensor_calibration_io::CameraCalibration::ProjectionModel::PINHOLE_SYMMETRIC;
			perceptionCameraCalibration.distortionModel = perception::sensor_calibration_io::CameraCalibration::DistortionModel::FISHEYE62;

			perceptionCameraCalibration.projectionCoefficients = {camera.focalLengthX(), camera.principalPointX(), camera.principalPointY()};
			perceptionCameraCalibration.distortionCoefficients = {camera.radialDistortion()[0], camera.radialDistortion()[1], camera.radialDistortion()[2], camera.radialDistortion()[3], camera.radialDistortion()[4], camera.radialDistortion()[5], camera.tangentialDistortion()[0], camera.tangentialDistortion()[1]};

			cameraCalibrationValues = {float(camera.focalLengthX()), float(camera.focalLengthY()), float(camera.principalPointX()), float(camera.principalPointY()), float(camera.radialDistortion()[0]), float(camera.radialDistortion()[1]), float(camera.radialDistortion()[2]), float(camera.radialDistortion()[3]), float(camera.radialDistortion()[4]), float(camera.radialDistortion()[5]), float(camera.tangentialDistortion()[0]), float(camera.tangentialDistortion()[1])};
			ocean_assert(cameraCalibrationValues.size() == 12);
		}
		else if (anyCamera_->name() == AnyCameraPerceptionD::WrappedCamera::name())
		{
			const std::shared_ptr<perception::CameraModelInterface<double>>& cameraModel = ((const AnyCameraPerceptionD&)(*anyCamera_)).actualCamera();

			perception::ParsedCameraInfo parsedCameraInfo;
			parsedCameraInfo.cameraId = int32_t(cameraId_);
			parsedCameraInfo.imageWidth = int32_t(cameraWidth_);
			parsedCameraInfo.imageHeight = int32_t(cameraHeight_);
			parsedCameraInfo.cameraModel.model = cameraModel->clone();

			const std::string sensorModel = "unknown";

			bool success = false;
			const perception::sensor_calibration_io::CameraCalibration localCameraCalibration = perception::parsedCameraInfoToCameraCalibration(parsedCameraInfo, *cameraModel, sensorModel, success, true /*populateLookupTable*/);
			ocean_assert(success);

			if (success)
			{
				perceptionCameraCalibration.projectionModel = localCameraCalibration.projectionModel;
				perceptionCameraCalibration.distortionModel = localCameraCalibration.distortionModel;

				perceptionCameraCalibration.projectionCoefficients = localCameraCalibration.projectionCoefficients;
				perceptionCameraCalibration.distortionCoefficients = localCameraCalibration.distortionCoefficients;

				perceptionCameraCalibration.lookupTableSpacingPx = localCameraCalibration.lookupTableSpacingPx;
				perceptionCameraCalibration.lookupTableData = localCameraCalibration.lookupTableData;

				assert(cameraCalibrationValues.empty());
				for (const double& projectionCoefficient : perceptionCameraCalibration.projectionCoefficients)
				{
					cameraCalibrationValues.emplace_back(float(projectionCoefficient));
				}

				for (const double& distortionCoefficient : perceptionCameraCalibration.distortionCoefficients)
				{
					cameraCalibrationValues.emplace_back(float(distortionCoefficient));
				}
			}
		}
		else
		{
			ocean_assert(false && "This camera model is not yet supported!");
		}
	}

	configurationLayout_.camera_id.set(cameraId_);
	configurationLayout_.width.set(cameraWidth_);
	configurationLayout_.height.set(cameraHeight_);
	configurationLayout_.pixelFormat.set(vrsPixelFormat_);
	configurationLayout_.stride.set(strideBytes_),
	configurationLayout_.cameraCalibrationValues.stage(cameraCalibrationValues);
	configurationLayout_.device_T_camera.set(Utilities::homogenousMatrix4ToVRS<double, double>(device_T_camera));
	configurationLayout_.device_T_flippedCamera.set(Utilities::homogenousMatrix4ToVRS<double, double>(device_T_flippedCamera));

	perception::sensor_calibration_io::DeviceInfo deviceInfo;
	deviceInfo.deviceType = "Unknown";

	std::string jsonCalibration;

	if (!cameraCalibrationValues.empty())
	{
		perception::sensor_calibration_io::Metadata metadata;

		jsonCalibration = perception::sensor_calibration_io::exportToJson(deviceInfo, {perceptionCameraCalibration}, metadata);
	}

	configurationLayout_.factory_calibration.stage(jsonCalibration);

	const Timestamp vrsRecordTimestamp = vrsRecordTimestamp_.isValid() ? vrsRecordTimestamp_ : Timestamp(true);
	return createRecord(double(vrsRecordTimestamp), vrs::Record::Type::CONFIGURATION, configurationLayoutVersion_, vrs::DataSource(configurationLayout_));
}

const vrs::Record* RecordableCamera::createStateRecord()
{
	// Best practice is to always create a record when asked, with a reasonable timestamp, even if the record is empty.
	const Timestamp vrsRecordTimestamp = vrsRecordTimestamp_.isValid() ? vrsRecordTimestamp_ : Timestamp(true);
	return createRecord(double(vrsRecordTimestamp), vrs::Record::Type::STATE, 0);
}

RecordableHomogenousMatrix4::RecordableHomogenousMatrix4(const vrs::RecordableTypeId recordableTypeId, const std::string& flavor, const Timestamp& vrsRecordStartingTimestamp) :
	Recordable(recordableTypeId, flavor, vrsRecordStartingTimestamp)
{
	addRecordFormat(vrs::Record::Type::CONFIGURATION, configurationLayoutVersion_, configurationLayout_.getContentBlock(), {&configurationLayout_});

	addRecordFormat(vrs::Record::Type::DATA, dataLayoutVersion_, dataLayout_.getContentBlock(), {&dataLayout_});
}

const vrs::Record* RecordableHomogenousMatrix4::createConfigurationRecord()
{
	ocean_assert(isValid());
	if (!isValid())
	{
		return nullptr;
	}

	const Timestamp vrsRecordTimestamp = vrsRecordTimestamp_.isValid() ? vrsRecordTimestamp_ : Timestamp(true);
	return createRecord(double(vrsRecordTimestamp), vrs::Record::Type::CONFIGURATION, configurationLayoutVersion_, vrs::DataSource(configurationLayout_));
}

const vrs::Record* RecordableHomogenousMatrix4::createStateRecord()
{
	// Best practice is to always create a record when asked, with a reasonable timestamp, even if the record is empty.
	const Timestamp vrsRecordTimestamp = vrsRecordTimestamp_.isValid() ? vrsRecordTimestamp_ : Timestamp(true);
	return createRecord(double(vrsRecordTimestamp), vrs::Record::Type::STATE, 0);
}

void RecordableHomogenousMatrix4::addData(const HomogenousMatrix4& homogenousMatrix4, const double timestamp, const double* recordTimestamp)
{
	HomogenousMatrixD4 homogenousMatrixD4(false);

	if (homogenousMatrix4.isValid())
	{
		homogenousMatrixD4 = HomogenousMatrixD4(VectorD3(homogenousMatrix4.translation()), QuaternionD(homogenousMatrix4.rotation()).normalized()); // separation into translation and rotation to handle precission issues between float and double
	}

	dataLayout_.homogenousMatrix4.set(Utilities::homogenousMatrix4ToVRS<double, double>(homogenousMatrixD4));
	dataLayout_.timestamp.set(timestamp);
	const Timestamp vrsRecordTimestamp = recordTimestamp != nullptr ? Timestamp(*recordTimestamp) : Timestamp(true);
	createRecord(double(vrsRecordTimestamp), vrs::Record::Type::DATA, dataLayoutVersion_, vrs::DataSource(dataLayout_));
}

std::string RecordableHomogenousMatrix4::defaultFlavor()
{
	return "ocean/homogenousmatrix4";
}

RecordableHomogenousMatrices4::RecordableHomogenousMatrices4(const vrs::RecordableTypeId recordableTypeId, const std::string& flavor, const Timestamp& vrsRecordStartingTimestamp) :
	Recordable(recordableTypeId, flavor, vrsRecordStartingTimestamp)
{
	addRecordFormat(vrs::Record::Type::CONFIGURATION, configurationLayoutVersion_, configurationLayout_.getContentBlock(), {&configurationLayout_});

	addRecordFormat(vrs::Record::Type::DATA, dataLayoutVersion_, dataLayout_.getContentBlock(), {&dataLayout_});
}

const vrs::Record* RecordableHomogenousMatrices4::createConfigurationRecord()
{
	ocean_assert(isValid());
	if (!isValid())
	{
		return nullptr;
	}

	const Timestamp vrsRecordTimestamp = vrsRecordTimestamp_.isValid() ? vrsRecordTimestamp_ : Timestamp(true);
	return createRecord(double(vrsRecordTimestamp), vrs::Record::Type::CONFIGURATION, configurationLayoutVersion_, vrs::DataSource(configurationLayout_));
}

const vrs::Record* RecordableHomogenousMatrices4::createStateRecord()
{
	// Best practice is to always create a record when asked, with a reasonable timestamp, even if the record is empty.
	const Timestamp vrsRecordTimestamp = vrsRecordTimestamp_.isValid() ? vrsRecordTimestamp_ : Timestamp(true);
	return createRecord(double(vrsRecordTimestamp), vrs::Record::Type::STATE, 0);
}

void RecordableHomogenousMatrices4::addData(const HomogenousMatrices4& homogenousMatrices4, const double timestamp, const double* recordTimestamp)
{
	std::vector<vrs::Matrix4Dd> vrsMatrices4Dd;
	vrsMatrices4Dd.reserve(homogenousMatrices4.size());

	for (const HomogenousMatrix4& homogenousMatrix4 : homogenousMatrices4)
	{
		HomogenousMatrixD4 homogenousMatrixD4(false);

		if (homogenousMatrix4.isValid())
		{
			homogenousMatrixD4 = HomogenousMatrixD4(VectorD3(homogenousMatrix4.translation()), QuaternionD(homogenousMatrix4.rotation()).normalized()); // separation into translation and rotation to handle precission issues between float and double
		}

		vrsMatrices4Dd.emplace_back(Utilities::homogenousMatrix4ToVRS<double, double>(homogenousMatrixD4));
	}

	dataLayout_.homogenousMatrix4.stage(std::move(vrsMatrices4Dd));
	dataLayout_.timestamp.set(timestamp);

	const Timestamp vrsRecordTimestamp = recordTimestamp != nullptr ? Timestamp(*recordTimestamp) : Timestamp(true);
	createRecord(double(vrsRecordTimestamp), vrs::Record::Type::DATA, dataLayoutVersion_, vrs::DataSource(dataLayout_));
}

std::string RecordableHomogenousMatrices4::defaultFlavor()
{
	return "ocean/homogenousmatrices4";
}

RecordableString::RecordableString(const vrs::RecordableTypeId recordableTypeId, const std::string& flavor, const Timestamp& vrsRecordStartingTimestamp) :
	Recordable(recordableTypeId, flavor, vrsRecordStartingTimestamp)
{
	addRecordFormat(vrs::Record::Type::CONFIGURATION, configurationLayoutVersion_, configurationLayout_.getContentBlock(), {&configurationLayout_});

	addRecordFormat(vrs::Record::Type::DATA, dataLayoutVersion_, dataLayout_.getContentBlock(), {&dataLayout_});
}

const vrs::Record* RecordableString::createConfigurationRecord()
{
	ocean_assert(isValid());
	if (!isValid())
	{
		return nullptr;
	}

	const Timestamp vrsRecordTimestamp = vrsRecordTimestamp_.isValid() ? vrsRecordTimestamp_ : Timestamp(true);
	return createRecord(double(vrsRecordTimestamp), vrs::Record::Type::CONFIGURATION, configurationLayoutVersion_, vrs::DataSource(configurationLayout_));
}

const vrs::Record* RecordableString::createStateRecord()
{
	// Best practice is to always create a record when asked, with a reasonable timestamp, even if the record is empty.
	const Timestamp vrsRecordTimestamp = vrsRecordTimestamp_.isValid() ? vrsRecordTimestamp_ : Timestamp(true);
	return createRecord(double(vrsRecordTimestamp), vrs::Record::Type::STATE, 0);
}

void RecordableString::addData(const std::string& value, const double timestamp, const double* recordTimestamp)
{
	dataLayout_.stringValue.stage(value);
	dataLayout_.timestamp.set(timestamp);
	const Timestamp vrsRecordTimestamp = recordTimestamp != nullptr ? Timestamp(*recordTimestamp) : Timestamp(true);
	createRecord(double(vrsRecordTimestamp), vrs::Record::Type::DATA, dataLayoutVersion_, vrs::DataSource(dataLayout_));
}

std::string RecordableString::defaultFlavor()
{
	return "ocean/string";
}

RecordableObjectBoundary::RecordableObjectBoundary(const vrs::RecordableTypeId recordableTypeId, const std::string& flavor, const Timestamp& vrsRecordStartingTimestamp) :
	Recordable(recordableTypeId, flavor, vrsRecordStartingTimestamp)
{
	addRecordFormat(vrs::Record::Type::CONFIGURATION, configurationLayoutVersion_, configurationLayout_.getContentBlock(), {&configurationLayout_});

	addRecordFormat(vrs::Record::Type::DATA, dataLayoutVersion_, dataLayout_.getContentBlock(), {&dataLayout_});
}

bool RecordableObjectBoundary::isValid() const
{
	return boundary_.size() > 0;
}

std::string RecordableObjectBoundary::defaultFlavor()
{
	return "ocean/gt_object_boundary";
}

void RecordableObjectBoundary::setObjectBoundaryConfig(const uint32_t objectId, const VectorsD3& boundary, const int semanticLabel)
{
	objectId_ = objectId;

	boundary_.reserve(boundary.size());
	for (const VectorD3& boundaryPoint : boundary)
	{
		boundary_.emplace_back(boundaryPoint.x(), boundaryPoint.y(), boundaryPoint.z());
	}

	semanticLabel_ = semanticLabel;
}

const vrs::Record* RecordableObjectBoundary::createConfigurationRecord()
{
	ocean_assert(isValid());
	if (!isValid())
	{
		return nullptr;
	}

	configurationLayout_.objectId.set(objectId_);
	configurationLayout_.boundary.stage(boundary_);
	configurationLayout_.semanticLabel.set(semanticLabel_);

	const Timestamp vrsRecordTimestamp = vrsRecordTimestamp_.isValid() ? vrsRecordTimestamp_ : Timestamp(true);
	return createRecord(double(vrsRecordTimestamp), vrs::Record::Type::CONFIGURATION, configurationLayoutVersion_, vrs::DataSource(configurationLayout_));
}

const vrs::Record* RecordableObjectBoundary::createStateRecord()
{
	// Best practice is to always create a record when asked, with a reasonable timestamp, even if the record is empty.
	const Timestamp vrsRecordTimestamp = vrsRecordTimestamp_.isValid() ? vrsRecordTimestamp_ : Timestamp(true);
	return createRecord(double(vrsRecordTimestamp), vrs::Record::Type::STATE, 0);
}

void RecordableObjectBoundary::addData(const HomogenousMatrix4& T_World_Object, const double timestamp, const double* recordTimestamp)
{
	HomogenousMatrixD4 homogenousMatrixD4(false);

	if (T_World_Object.isValid())
	{
		homogenousMatrixD4 = HomogenousMatrixD4(VectorD3(T_World_Object.translation()), QuaternionD(T_World_Object.rotation()).normalized()); // separation into translation and rotation to handle precission issues between float and double
	}

	dataLayout_.T_World_Object.set(Utilities::homogenousMatrix4ToVRS<double, double>(homogenousMatrixD4));
	dataLayout_.timestamp.set(timestamp);
	const Timestamp vrsRecordTimestamp = recordTimestamp != nullptr ? Timestamp(*recordTimestamp) : Timestamp(true);
	createRecord(double(vrsRecordTimestamp), vrs::Record::Type::DATA, dataLayoutVersion_, vrs::DataSource(dataLayout_));
}

}

}

}
