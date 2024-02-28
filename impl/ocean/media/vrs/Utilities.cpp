// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/vrs/Utilities.h"

#include "ocean/base/Triple.h"

#include "ocean/math/ExponentialMap.h"

#include "ocean/io/FileConfig.h"
#include "ocean/io/JSONConfig.h"

#include <perception/sensor_calibration_io/CameraCalibrationImport.h>
#include <perception/sensor_calibration_io/ImuCalibrationImport.h>

#include <vrs/RecordFileReader.h>

namespace Ocean
{

namespace Media
{

namespace VRS
{

Utilities::IMUDataPlayable::IMUDataPlayable() :
	device_T_imu_(false),
	imu_R_imuraw_(false)
{
	// nothing to do here
}

bool Utilities::IMUDataPlayable::onDataLayoutRead(const vrs::CurrentRecord& header, size_t /*blockIndex*/, vrs::DataLayout& dataLayout)
{
	// The use of DataLayout::findDataPieceXXX an anti-pattern, please do not clone this bad practice.
	// See D39522991 or D39527762 for proper efficient use of datalayout objects.
	if (header.recordType == vrs::Record::Type::CONFIGURATION)
	{
		const vrs::DataPieceString* factoryCalibrationDataPieceString = dataLayout.findDataPieceString("factory_calibration");

		if (factoryCalibrationDataPieceString)
		{
			const std::string factoryCalibration = factoryCalibrationDataPieceString->get();

			perception::sensor_calibration_io::ImuCalibration calibration;
			if (perception::sensor_calibration_io::parseFromJson(factoryCalibration, calibration))
			{
				const VectorD3 deviceFromImuTranslation(calibration.deviceFromImuTranslation);

				const SquareMatrixD3 deviceFromImuRotation((const double*)calibration.deviceFromImuRotation, /* rowAligned = */ true);
				ocean_assert(deviceFromImuRotation.isOrthonormal());

				// transformation transforming points defined in the coordinate system of the imu to points defined in the coordinate system of the device
				device_T_imu_ = HomogenousMatrixD4(deviceFromImuTranslation, deviceFromImuRotation);

				const HomogenousMatrixD4 accelerometerMatrix(SquareMatrixD3((const double*)calibration.accelerometerMatrix, /* rowAligned = */ true));

				VectorD3 accelerometerMatrixTranslation;
				QuaternionD accelerometerMatrixRotation;
				VectorD3 accelerometerMatrixScale;
				VectorD3 accelerometerMatrixShear;
				if (accelerometerMatrix.decompose(accelerometerMatrixTranslation, accelerometerMatrixRotation, accelerometerMatrixScale, accelerometerMatrixShear))
				{
					ocean_assert(accelerometerMatrixRotation.isValid());
					imu_R_imuraw_ = accelerometerMatrixRotation;
				}
			}
		}
	}

	return true;
}

Utilities::ImagePlayable::ImagePlayable() :
	device_T_camera_(false)
{
	// nothing to do here
}

bool Utilities::ImagePlayable::onDataLayoutRead(const vrs::CurrentRecord& header, size_t /*blockIndex*/, vrs::DataLayout& dataLayout)
{
	// The use of DataLayout::findDataPieceXXX an anti-pattern, please do not clone this bad practice.
	// See D39522991 or D39527762 for proper efficient use of datalayout objects.
	if (header.recordType == vrs::Record::Type::CONFIGURATION)
	{
		const vrs::DataPieceValue<unsigned int>* cameraIdDataPieceValue = dataLayout.findDataPieceValue<unsigned int>("camera_id");

		if (cameraIdDataPieceValue)
		{
			const unsigned int cameraId = cameraIdDataPieceValue->get();

			const vrs::DataPieceString* factoryCalibrationDataPieceString = dataLayout.findDataPieceString("factory_calibration");

			if (factoryCalibrationDataPieceString)
			{
				const std::string factoryCalibration = factoryCalibrationDataPieceString->get();

				std::map<int, perception::sensor_calibration_io::CameraCalibration> parsedCalibrations;
				if (perception::sensor_calibration_io::parseFromJson(factoryCalibration, parsedCalibrations))
				{
					const std::map<int, perception::sensor_calibration_io::CameraCalibration>::const_iterator iCalibration = parsedCalibrations.find(int(cameraId));

					if (iCalibration != parsedCalibrations.cend())
					{
						const perception::sensor_calibration_io::CameraCalibration& calibration = iCalibration->second;

						const VectorD3 translation(calibration.deviceFromCameraTranslation);
						const SquareMatrixD3 rotationF((const double*)calibration.deviceFromCameraRotation, /* rowAligned = */ true);
						ocean_assert(rotationF.isOrthonormal());

						// the coordinate system of the camera is flipped (the camera is looking towards the negative z-space), this is the flipped coordinate system in Ocean
						// therefore, we need to rotate the coordinate system around the x-axis to determine Ocean's default coordinate system, a camera looking towards the negative z-space
						const SquareMatrixD3 rotation(rotationF * SquareMatrixD3(1, 0, 0, 0, -1, 0, 0, 0, -1));

						// transformation transforming points defined in the coordinate system of the camera to points defined in the coordinate system of the device
						device_T_camera_ = HomogenousMatrixD4(translation, rotation);

						if (calibration.width > 0 && calibration.width <= int(NumericT<int16_t>::maxValue()) && calibration.height > 0 && calibration.height <= int(NumericT<int16_t>::maxValue()))
						{
							const perception::ImageSize imageSize(int16_t(calibration.width), int16_t(calibration.height));

							std::vector<double> parameters;
							parameters.insert(parameters.end(), calibration.projectionCoefficients.cbegin(), calibration.projectionCoefficients.cend());
							parameters.insert(parameters.end(), calibration.distortionCoefficients.cbegin(), calibration.distortionCoefficients.cend());

							cameraModel_ = std::make_shared<perception::Fisheye62CameraModelLut1D<double>>(imageSize, parameters);
						}
					}
					else
					{
						ocean_assert(false && "The camera id does not provide a valid camera calibration - should never happen!");
					}
				}
			}
		}
	}

	return true;
}

bool Utilities::readCalibrationData(const std::string& vrsFile, HomogenousMatrixD4& device_T_imu, QuaternionD& imu_R_imuraw)
{
	vrs::RecordFileReader recordFileReader;

	if (recordFileReader.openFile(vrsFile) != 0)
	{
		return false;
	}

	const std::set<vrs::StreamId>& streamIds = recordFileReader.getStreams();

	vrs::StreamId imuDataStreamId;

	for (const vrs::StreamId& streamId : streamIds)
	{
		if (streamId.getTypeId() == vrs::RecordableTypeId::SlamImuData)
		{
			if (imuDataStreamId.isValid())
			{
				ocean_assert(false && "The vrs file contains more than one IMU data stream!");
				return false;
			}

			imuDataStreamId = streamId;
		}
	}

	if (!imuDataStreamId.isValid())
	{
		return false;
	}

	device_T_imu.toNull();
	imu_R_imuraw = QuaternionD(false);

	IMUDataPlayable imuDataPlayable;
	recordFileReader.setStreamPlayer(imuDataStreamId, &imuDataPlayable);

	const std::vector<const vrs::IndexRecord::RecordInfo*> recordInfos = recordFileReader.getIndex(imuDataStreamId);

	// first we need to read all configuration records

	for (size_t n = 0; n < recordInfos.size(); ++n)
	{
		if (recordInfos[n]->recordType == vrs::Record::Type::CONFIGURATION)
		{
			recordFileReader.readRecord(*recordInfos[n], &imuDataPlayable);

			ocean_assert(!device_T_imu.isValid());
			device_T_imu = imuDataPlayable.device_T_imu();
			imu_R_imuraw = imuDataPlayable.imu_R_imuraw();
		}
	}

	return device_T_imu.isValid() && imu_R_imuraw.isValid();
}

bool Utilities::readCameraCalibrationData(const std::string& vrsFile, std::vector<std::string>& cameraRecordables, std::vector<std::shared_ptr<perception::CameraModelInterface<double>>>* cameraModels, HomogenousMatricesD4* device_T_cameras)
{
	vrs::RecordFileReader recordFileReader;

	if (recordFileReader.openFile(vrsFile) != 0)
	{
		return false;
	}

	const std::set<vrs::StreamId>& streamIds = recordFileReader.getStreams();

	for (const vrs::StreamId& streamId : streamIds)
	{
		if (streamId.getTypeId() == vrs::RecordableTypeId::SlamCameraData
		    || streamId.getTypeId() == vrs::RecordableTypeId::ForwardCameraRecordableClass)
		{
			ImagePlayable imagePlayable;
			recordFileReader.setStreamPlayer(streamId, &imagePlayable);

			const std::vector<const vrs::IndexRecord::RecordInfo*> recordInfos = recordFileReader.getIndex(streamId);

			// first we need to read all configuration records

			for (size_t n = 0; n < recordInfos.size(); ++n)
			{
				if (recordInfos[n]->recordType == vrs::Record::Type::CONFIGURATION)
				{
					recordFileReader.readRecord(*recordInfos[n], &imagePlayable);

					if (imagePlayable.device_T_camera().isValid())
					{
						cameraRecordables.push_back(streamId.getName());

						if (cameraModels)
						{
							cameraModels->push_back(imagePlayable.cameraModel());
						}

						if (device_T_cameras)
						{
							device_T_cameras->push_back(imagePlayable.device_T_camera());
						}
					}
				}
			}
		}
	}

	return true;
}

bool Utilities::writePosesMap(const PosesMap& poses, const std::string& filename)
{
	std::ofstream stream(filename);

	if (!stream.is_open())
	{
		ocean_assert(false && "Failed to load poses file!");
		return false;
	}

	const std::vector<std::pair<double, HomogenousMatrixD4>> posesVector = poses.samples();

	for (size_t n = 0; n < posesVector.size(); ++n)
	{
		const double& timestamp = posesVector[n].first;
		const HomogenousMatrixD4& pose = posesVector[n].second;

		const VectorD3 translation = pose.translation();
		const ExponentialMap exponentialMap(Quaternion(pose.rotation()));

		stream << String::toAString(timestamp, 16u) << " ";
		stream << String::toAString(exponentialMap.data()[0], 16u) << " " << String::toAString(exponentialMap.data()[1], 16u) << " " << String::toAString(exponentialMap.data()[2], 16u) << " ";
		stream << String::toAString(translation.x(), 16u) << " " << String::toAString(translation.y(), 16u) << " " << String::toAString(translation.z(), 16u) << std::endl;
	}

	return stream.good();
}

Utilities::PosesMap Utilities::loadPosesMap(const std::string& filename)
{
	std::ifstream stream(filename);

	if (!stream.is_open())
	{
		ocean_assert(false && "Failed to load poses file!");
		return PosesMap();
	}

	typedef Triple<double, VectorD3, VectorD3> PoseTriple;
	typedef std::vector<PoseTriple> PoseTriples;

	PoseTriples poseTriples;

	std::string line;
	std::vector<std::string> lineTokens;

	while (getline(stream, line))
	{
		lineTokens.clear();

		std::istringstream lineStringStream(line);

		for (std::string token; lineStringStream >> token; /* noop */)
		{
			lineTokens.push_back(token);
		}

		if (lineTokens.size() != 7)
		{
			ocean_assert(false && "Invalid line!");
			return PosesMap();
		}

		poseTriples.push_back(PoseTriple());

		PoseTriple& poseTriple = poseTriples.back();

		if (!String::isNumber(lineTokens[0], false, &poseTriple.first()))
		{
			ocean_assert(false && "Invalid timestamp!");
			return PosesMap();
		}

		if (!String::isNumber(lineTokens[1], false, &poseTriple.second().x()) || !String::isNumber(lineTokens[2], false, &poseTriple.second().y()) || !String::isNumber(lineTokens[3], false, &poseTriple.second().z()))
		{
			ocean_assert(false && "Invalid rotation!");
			return PosesMap();
		}

		if (!String::isNumber(lineTokens[4], false, &poseTriple.third().x()) || !String::isNumber(lineTokens[5], false, &poseTriple.third().y()) || !String::isNumber(lineTokens[6], false, &poseTriple.third().z()))
		{
			ocean_assert(false && "Invalid translation!");
			return PosesMap();
		}
	}

	PosesMap sampleMap(poseTriples.size() + 10);

	for (const PoseTriple& poseTriple : poseTriples)
	{
		const double& timestamp = poseTriple.first();
		const VectorD3& rotation = poseTriple.second();
		const VectorD3& translation = poseTriple.third();

		const ExponentialMap exponentialMap = ExponentialMap(Scalar(rotation[0]), Scalar(rotation[1]), Scalar(rotation[2]));

		sampleMap.insert(HomogenousMatrixD4(translation, QuaternionD(exponentialMap.quaternion())), timestamp);
	}

	ocean_assert(sampleMap.size() == poseTriples.size());

	return sampleMap;
}

bool Utilities::extractReplayPosesFromJSON(const std::string& jsonFilename, PosesMap& poses, const bool extractEstimatedPoses)
{
	const std::string posesType = extractEstimatedPoses ? "vision_estimate" : "vision_prediction";

	poses = PosesMap(size_t(-1));
	IO::JSONConfig jsonConfig(jsonFilename, true);

	Config::Value& config = jsonConfig["map_data"];
	for (unsigned int nA = 0; nA < config.values(); ++nA)
	{
		std::string nameA;
		Config::Value& valueA = config.value(nA, nameA);

		if (nameA == "frames")
		{
			double timestamp = 0.0;
			VectorD3 translation(0, 0, 0);
			VectorD3 rotation(0, 0, 0);

			for (unsigned int nB = 0; nB < valueA.values(); ++nB)
			{
				std::string nameB;
				Config::Value& valueB = valueA.value(nB, nameB);

				if (nameB == posesType)
				{
					for (unsigned int nC = 0; nC < valueB.values(); ++nC)
					{
						std::string nameC;
						Config::Value& valueC = valueB.value(nC, nameC);

						if (nameC == "se3")
						{
							unsigned int tIndex = 0u;
							unsigned int rIndex = 0u;

							for (unsigned int nD = 0; nD < valueC.values(); ++nD)
							{
								std::string nameD;
								Config::Value& valueD = valueC.value(nD, nameD);

								if (nameD == "rotation")
								{
									rotation[rIndex++] = valueD(0.0);
								}
								else if (nameD == "translation")
								{
									translation[tIndex++] = valueD(0.0);
								}
							}
						}
					}
				}
				else if (nameB == "tracking_arrival_timestamp")
				{
					timestamp = valueB(0.0);
				}
			}

			poses.insert(HomogenousMatrixD4(translation, QuaternionD(ExponentialMap(Vector3(rotation)).quaternion())), timestamp);
		}
	}

	return poses.size() != 0;
}

}

}

}
