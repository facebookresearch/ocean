// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/vrs/DeviceRecorder.h"

#include "ocean/base/Memory.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/devices/DeviceRef.h"
#include "ocean/devices/SceneTracker6DOF.h"
#include "ocean/devices/Sensor.h"

#include "ocean/io/vrs/Recorder.h"
#include "ocean/io/vrs/Utilities.h"

namespace Ocean
{

namespace Devices
{

namespace VRS
{

DeviceRecorder::RecordableTracker::RecordableTracker(const vrs::RecordableTypeId recordableTypeId, const std::string& flavor, const Timestamp& vrsRecordStartingTimestamp) :
	Recordable(recordableTypeId, flavor, vrsRecordStartingTimestamp)
{
	addRecordFormat(vrs::Record::Type::CONFIGURATION, configurationLayoutVersion_, configurationLayout_.getContentBlock(), {&configurationLayout_});
}

bool DeviceRecorder::RecordableTracker::setDevice(const std::string& deviceName, const std::string& deviceTypeMajor, const std::string& deviceTypeMinor, const Timestamp& recordTimestamp)
{
	ocean_assert(!deviceName.empty() && !deviceTypeMajor.empty());

	if (deviceName_ == deviceName)
	{
		return true;
	}

	deviceName_ = deviceName;
	deviceTypeMajor_ = deviceTypeMajor;
	deviceTypeMinor_ = deviceTypeMinor;

	vrsRecordTimestamp_ = recordTimestamp;

	return createConfigurationRecord() != nullptr;
}

bool DeviceRecorder::RecordableTracker::isValid() const
{
	return !deviceName_.empty() && !deviceTypeMajor_.empty() && !deviceTypeMinor_.empty();
}

bool DeviceRecorder::RecordableTracker::writeMetadataToBitstream(const Measurement::Metadata& metadata, IO::OutputBitstream& bitstream)
{
	if (!bitstream.write<unsigned long long>(metadataTag_))
	{
		return false;
	}

	constexpr unsigned long long version = 1ull;
	if (!bitstream.write<unsigned long long>(version))
	{
		return false;
	}

	const unsigned int numberMetadataEntries = (unsigned int)(metadata.size());
	if (!bitstream.write<unsigned int>(numberMetadataEntries))
	{
		return false;
	}

	for (Measurement::Metadata::const_iterator iEntry = metadata.cbegin(); iEntry != metadata.cend(); ++iEntry)
	{
		const std::string& name = iEntry->first;
		const Value& value = iEntry->second;

		if (!bitstream.write(name))
		{
			return false;
		}

		switch (value.type())
		{
			case Value::VT_BOOL:
			{
				if (!bitstream.write<unsigned long long>(metadataTypeTagBool_))
				{
					return false;
				}

				if (!bitstream.write<bool>(value.boolValue()))
				{
					return false;
				}

				break;
			}

			case Value::VT_INT_32:
			{
				if (!bitstream.write<unsigned long long>(metadataTypeTagInt32_))
				{
					return false;
				}

				if (!bitstream.write<int>(value.intValue()))
				{
					return false;
				}

				break;
			}

			case Value::VT_INT_64:
			{
				if (!bitstream.write<unsigned long long>(metadataTypeTagInt64_))
				{
					return false;
				}

				if (!bitstream.write<long long>(value.int64Value()))
				{
					return false;
				}

				break;
			}

			case Value::VT_FLOAT_32:
			{
				if (!bitstream.write<unsigned long long>(metadataTypeTagFloat32_))
				{
					return false;
				}

				if (!bitstream.write<float>(value.floatValue()))
				{
					return false;
				}

				break;
			}

			case Value::VT_FLOAT_64:
			{
				if (!bitstream.write<unsigned long long>(metadataTypeTagFloat64_))
				{
					return false;
				}

				if (!bitstream.write<double>(value.float64Value()))
				{
					return false;
				}

				break;
			}

			case Value::VT_STRING:
			{
				if (!bitstream.write<unsigned long long>(metadataTypeTagString_))
				{
					return false;
				}

				if (!bitstream.write<std::string>(value.stringValue()))
				{
					return false;
				}

				break;
			}

			case Value::VT_BUFFER:
			{
				if (!bitstream.write<unsigned long long>(metadataTypeTagBuffer_))
				{
					return false;
				}

				size_t size = 0;
				const void* data = value.bufferValue(size);

				if (!bitstream.write<unsigned long long>((unsigned long long)(size)))
				{
					return false;
				}

				if (!bitstream.write(data, size))
				{
					return false;
				}

				break;
			}

			default:
				ocean_assert(false && "Metadata type is not supported!");
				return false;
		}
	}

	return true;
}

bool DeviceRecorder::RecordableTracker::readMetadataFromBitstream(IO::InputBitstream& bitstream, Measurement::Metadata& metadata)
{
	unsigned long long metadataTag = 0ull;
	if (!bitstream.read<unsigned long long>(metadataTag) || metadataTag != metadataTag_)
	{
		return false;
	}

	unsigned long long version = 0ull;
	if (!bitstream.read<unsigned long long>(version) || version != 1ull)
	{
		return false;
	}

	unsigned int numberMetadataEntries = 0u;
	if (!bitstream.read<unsigned int>(numberMetadataEntries))
	{
		return false;
	}

	metadata.clear();
	metadata.reserve(numberMetadataEntries);

	for (unsigned int n = 0u; n < numberMetadataEntries; ++n)
	{
		std::string name;
		if (!bitstream.read<std::string>(name))
		{
			return false;
		}

		if (metadata.find(name) != metadata.cend())
		{
			ocean_assert(false && "The entry exists already");
			return false;
		}

		unsigned long long valueTypeTag = 0ull;
		if (!bitstream.read<unsigned long long>(valueTypeTag))
		{
			return false;
		}

		switch (valueTypeTag)
		{
			case metadataTypeTagBool_:
			{
				bool value;
				if (!bitstream.read<bool>(value))
				{
					return false;
				}

				metadata.emplace(name, Value(value));

				break;
			}

			case metadataTypeTagInt32_:
			{
				int value;
				if (!bitstream.read<int>(value))
				{
					return false;
				}

				metadata.emplace(name, Value(value));

				break;
			}

			case metadataTypeTagInt64_:
			{
				long long value;
				if (!bitstream.read<long long>(value))
				{
					return false;
				}

				metadata.emplace(name, Value(int64_t(value)));

				break;
			}

			case metadataTypeTagFloat32_:
			{
				float value;
				if (!bitstream.read<float>(value))
				{
					return false;
				}

				metadata.emplace(name, Value(value));

				break;
			}

			case metadataTypeTagFloat64_:
			{
				double value;
				if (!bitstream.read<double>(value))
				{
					return false;
				}

				metadata.emplace(name, Value(value));

				break;
			}

			case metadataTypeTagString_:
			{
				std::string value;
				if (!bitstream.read<std::string>(value))
				{
					return false;
				}

				metadata.emplace(name, Value(value));

				break;
			}

			case metadataTypeTagBuffer_:
			{
				unsigned long long size = 0u;
				if (!bitstream.read<unsigned long long>(size) || size > 1024u * 1024u * 1024u) // at most 1GB
				{
					return false;
				}

				std::vector<uint8_t> value(size);
				if (!bitstream.read(value.data(), value.size()))
				{
					return false;
				}

				metadata.emplace(name, Value(value.data(), value.size()));

				break;
			}

			default:
				ocean_assert(false && "Metadata type is not supported!");
				return false;
		}
	}

	return true;
}

const vrs::Record* DeviceRecorder::RecordableTracker::createConfigurationRecord()
{
	if (!isValid())
	{
		return nullptr;
	}

	configurationLayout_.deviceNameValue.stage(deviceName_);
	configurationLayout_.deviceTypeMajorValue.stage(std::move(deviceTypeMajor_));
	configurationLayout_.deviceTypeMinorValue.stage(std::move(deviceTypeMinor_));

	const Timestamp vrsRecordTimestamp = vrsRecordTimestamp_.isValid() ? vrsRecordTimestamp_ : Timestamp(true);
	return createRecord(double(vrsRecordTimestamp), vrs::Record::Type::CONFIGURATION, configurationLayoutVersion_, vrs::DataSource(configurationLayout_));
}

const vrs::Record* DeviceRecorder::RecordableTracker::createStateRecord()
{
	// Best practice is to always create a record when asked, with a reasonable timestamp, even if the record is empty.
	const Timestamp vrsRecordTimestamp = vrsRecordTimestamp_.isValid() ? vrsRecordTimestamp_ : Timestamp(true);
	return createRecord(double(vrsRecordTimestamp), vrs::Record::Type::STATE, 0);
}

DeviceRecorder::RecordableTracker6DOFSample::RecordableTracker6DOFSample(const vrs::RecordableTypeId recordableTypeId, const std::string& flavor, const Timestamp& vrsRecordStartingTimestamp) :
	RecordableTracker(recordableTypeId, flavor, vrsRecordStartingTimestamp)
{
	release();

	addRecordFormat(vrs::Record::Type::DATA, sampleDataLayoutVersion_, dataSampleLayout_.getContentBlock() + vrs::ContentBlock(vrs::ImageFormat::RAW), {&dataSampleLayout_});
}

void DeviceRecorder::RecordableTracker6DOFSample::addData(const Measurement* sender, const Measurement::ObjectIdSet& foundObjects, const Measurement::ObjectIdSet& lostObjects, const Tracker6DOF::Tracker6DOFSampleRef& sample, const Timestamp& timestamp, const double* recordTimestamp)
{
	ocean_assert(sender != nullptr);

	DescriptionMap descriptionMap;

	if (sample)
	{
		for (const Measurement::ObjectId& objectId : sample->objectIds())
		{
			std::string description = sender->objectDescription(objectId);

			descriptionMap.emplace(std::move(description), objectId);
		}
	}

	for (const Measurement::ObjectId& objectId : foundObjects)
	{
		std::string description = sender->objectDescription(objectId);

		descriptionMap.emplace(std::move(description), objectId);
	}

	for (const Measurement::ObjectId& objectId : lostObjects)
	{
		std::string description = sender->objectDescription(objectId);

		descriptionMap.emplace(std::move(description), objectId);
	}

	addData(std::move(descriptionMap), foundObjects, lostObjects, sample, timestamp, recordTimestamp);
}

void DeviceRecorder::RecordableTracker6DOFSample::addData(DescriptionMap&& descriptionMap, const Measurement::ObjectIdSet& foundObjects, const Measurement::ObjectIdSet& lostObjects, const Tracker6DOF::Tracker6DOFSampleRef& sample, const Timestamp& timestamp, const double* recordTimestamp)
{
	dataSampleLayout_.timestamp.set(double(timestamp));

	if (sample)
	{
		ocean_assert(sample->objectIds().size() == sample->orientations().size());
		ocean_assert(sample->objectIds().size() == sample->positions().size());

		std::vector<vrs::Matrix4Dd> vrsMatrices4Dd;
		vrsMatrices4Dd.reserve(sample->objectIds().size());

		for (size_t n = 0; n < sample->objectIds().size(); ++n)
		{
			const HomogenousMatrix4 homogenousMatrix(sample->positions()[n], sample->orientations()[n]);

			vrsMatrices4Dd.emplace_back(IO::VRS::Utilities::homogenousMatrix4ToVRS<Scalar, double>(homogenousMatrix));
		}

		if (sample->referenceSystem() == Tracker6DOF::RS_DEVICE_IN_OBJECT)
		{
			dataSampleLayout_.referenceSystem.stage("DEVICE_IN_OBJECT");
		}
		else
		{
			ocean_assert(sample->referenceSystem() == Tracker6DOF::RS_OBJECT_IN_DEVICE);
			dataSampleLayout_.referenceSystem.stage("OBJECT_IN_DEVICE");
		}

		dataSampleLayout_.homogenousMatrix4.stage(std::move(vrsMatrices4Dd));
		dataSampleLayout_.objectIds.stage(sample->objectIds());

		std::ostringstream stringStream(std::ios::binary);
		IO::OutputBitstream bitstream(stringStream);
		if (!writeMetadataToBitstream(sample->metadata(), bitstream))
		{
			ocean_assert(false && "Failed to encode metadata!");
		}

		dataSampleLayout_.metadata.stage(stringStream.str());
	}
	else
	{
		dataSampleLayout_.referenceSystem.stagedValue().clear();
		dataSampleLayout_.homogenousMatrix4.stagedValues().clear();
		dataSampleLayout_.objectIds.stagedValues().clear();
		dataSampleLayout_.metadata.stagedValue().clear();
	}

	dataSampleLayout_.objectDescriptionMap.stage(std::move(descriptionMap));

	dataSampleLayout_.foundObjects.stage(std::vector<Measurement::ObjectId>(foundObjects.cbegin(), foundObjects.cend()));

	dataSampleLayout_.lostObjects.stage(std::vector<Measurement::ObjectId>(lostObjects.cbegin(), lostObjects.cend()));

	const Timestamp vrsRecordTimestamp = recordTimestamp ? Timestamp(*recordTimestamp) : Timestamp(true);
	const vrs::Record* record = createRecord(double(vrsRecordTimestamp), vrs::Record::Type::DATA, sampleDataLayoutVersion_, vrs::DataSource(dataSampleLayout_));

	ocean_assert_and_suppress_unused(record != nullptr, record);
}

std::string DeviceRecorder::RecordableTracker6DOFSample::defaultFlavor()
{
	return std::string("ocean/devices/tracker6dofsample");
}

DeviceRecorder::RecordableGPSTrackerSample::RecordableGPSTrackerSample(const vrs::RecordableTypeId recordableTypeId, const std::string& flavor, const Timestamp& vrsRecordStartingTimestamp) :
	RecordableTracker(recordableTypeId, flavor, vrsRecordStartingTimestamp)
{
	release();

	addRecordFormat(vrs::Record::Type::DATA, sampleDataLayoutVersion_, dataSampleLayout_.getContentBlock() + vrs::ContentBlock(vrs::ImageFormat::RAW), {&dataSampleLayout_});
}

void DeviceRecorder::RecordableGPSTrackerSample::addData(const Measurement* sender, const Measurement::ObjectIdSet& foundObjects, const Measurement::ObjectIdSet& lostObjects, const GPSTracker::GPSTrackerSampleRef& sample, const Timestamp& timestamp, const double* recordTimestamp)
{
	ocean_assert(sender != nullptr);

	std::map<std::string, unsigned int> descriptionMap;

	dataSampleLayout_.timestamp.set(double(timestamp));

	if (sample)
	{
		for (const Measurement::ObjectId& objectId : sample->objectIds())
		{
			std::string description = sender->objectDescription(objectId);

			descriptionMap.emplace(std::move(description), objectId);
		}

		ocean_assert(sample->objectIds().size() == sample->locations().size());

		std::vector<double> longitudes;
		longitudes.reserve(sample->objectIds().size());

		std::vector<double> latitudes;
		latitudes.reserve(sample->objectIds().size());

		std::vector<float> altitudes;
		altitudes.reserve(sample->objectIds().size());

		std::vector<float> directions;
		directions.reserve(sample->objectIds().size());

		std::vector<float> speeds;
		speeds.reserve(sample->objectIds().size());

		std::vector<float> accuracies;
		accuracies.reserve(sample->objectIds().size());

		std::vector<float> altitudeAccuracies;
		altitudeAccuracies.reserve(sample->objectIds().size());

		std::vector<float> directionAccuracies;
		directionAccuracies.reserve(sample->objectIds().size());

		std::vector<float> speedAccuracies;
		speedAccuracies.reserve(sample->objectIds().size());

		for (const GPSTracker::Location& location : sample->locations())
		{
			latitudes.emplace_back(location.latitude());
			longitudes.emplace_back(location.longitude());

			altitudes.emplace_back(location.altitude());
			directions.emplace_back(location.direction());
			speeds.emplace_back(location.speed());

			accuracies.emplace_back(location.accuracy());
			altitudeAccuracies.emplace_back(location.altitudeAccuracy());
			directionAccuracies.emplace_back(location.directionAccuracy());
			speedAccuracies.emplace_back(location.speedAccuracy());
		}

		if (sample->referenceSystem() == Tracker6DOF::RS_DEVICE_IN_OBJECT)
		{
			dataSampleLayout_.referenceSystem.stage("DEVICE_IN_OBJECT");
		}
		else
		{
			ocean_assert(sample->referenceSystem() == Tracker6DOF::RS_OBJECT_IN_DEVICE);
			dataSampleLayout_.referenceSystem.stage("OBJECT_IN_DEVICE");
		}

		dataSampleLayout_.latitudes.stage(std::move(latitudes));
		dataSampleLayout_.longitudes.stage(std::move(longitudes));
		dataSampleLayout_.altitudes.stage(std::move(altitudes));
		dataSampleLayout_.directions.stage(std::move(directions));
		dataSampleLayout_.speeds.stage(std::move(speeds));
		dataSampleLayout_.accuracies.stage(std::move(accuracies));
		dataSampleLayout_.altitudeAccuracies.stage(std::move(altitudeAccuracies));
		dataSampleLayout_.directionAccuracies.stage(std::move(directionAccuracies));
		dataSampleLayout_.speedAccuracies.stage(std::move(speedAccuracies));

		dataSampleLayout_.objectIds.stage(sample->objectIds());

		std::ostringstream stringStream(std::ios::binary);
		IO::OutputBitstream bitstream(stringStream);
		if (!writeMetadataToBitstream(sample->metadata(), bitstream))
		{
			ocean_assert(false && "Failed to encode metadata!");
		}

		dataSampleLayout_.metadata.stage(stringStream.str());
	}
	else
	{
		dataSampleLayout_.referenceSystem.stagedValue().clear();
		dataSampleLayout_.latitudes.stagedValues().clear();
		dataSampleLayout_.longitudes.stagedValues().clear();
		dataSampleLayout_.altitudes.stagedValues().clear();
		dataSampleLayout_.directions.stagedValues().clear();
		dataSampleLayout_.speeds.stagedValues().clear();
		dataSampleLayout_.accuracies.stagedValues().clear();
		dataSampleLayout_.altitudeAccuracies.stagedValues().clear();
		dataSampleLayout_.directionAccuracies.stagedValues().clear();
		dataSampleLayout_.speedAccuracies.stagedValues().clear();
		dataSampleLayout_.objectIds.stagedValues().clear();
		dataSampleLayout_.metadata.stagedValue().clear();
	}

	for (const Measurement::ObjectId& objectId : foundObjects)
	{
		std::string description = sender->objectDescription(objectId);

		descriptionMap.emplace(std::move(description), objectId);
	}

	for (const Measurement::ObjectId& objectId : lostObjects)
	{
		std::string description = sender->objectDescription(objectId);

		descriptionMap.emplace(std::move(description), objectId);
	}

	dataSampleLayout_.objectDescriptionMap.stage(std::move(descriptionMap));

	dataSampleLayout_.foundObjects.stage(std::vector<Measurement::ObjectId>(foundObjects.cbegin(), foundObjects.cend()));

	dataSampleLayout_.lostObjects.stage(std::vector<Measurement::ObjectId>(lostObjects.cbegin(), lostObjects.cend()));

	const Timestamp vrsRecordTimestamp = recordTimestamp ? Timestamp(*recordTimestamp) : Timestamp(true);
	const vrs::Record* record = createRecord(double(vrsRecordTimestamp), vrs::Record::Type::DATA, sampleDataLayoutVersion_, vrs::DataSource(dataSampleLayout_));

	ocean_assert_and_suppress_unused(record != nullptr, record);
}

std::string DeviceRecorder::RecordableGPSTrackerSample::defaultFlavor()
{
	return std::string("ocean/devices/gpstrackersample");
}

DeviceRecorder::RecordableSceneTracker6DOFSample::RecordableSceneTracker6DOFSample(const vrs::RecordableTypeId recordableTypeId, const std::string& flavor, const Timestamp& vrsRecordStartingTimestamp) :
	RecordableTracker(recordableTypeId, flavor, vrsRecordStartingTimestamp)
{
	release();

	addRecordFormat(vrs::Record::Type::DATA, sampleDataLayoutVersion_, dataSampleLayout_.getContentBlock() + vrs::ContentBlock(vrs::ImageFormat::RAW), {&dataSampleLayout_});
}

void DeviceRecorder::RecordableSceneTracker6DOFSample::addData(const Measurement* sender, const Measurement::ObjectIdSet& foundObjects, const Measurement::ObjectIdSet& lostObjects, const SceneTracker6DOF::SceneTracker6DOFSampleRef& sample, const Timestamp& timestamp, const double* recordTimestamp)
{
	ocean_assert(sender != nullptr);

	std::map<std::string, unsigned int> descriptionMap;

	dataSampleLayout_.timestamp.set(double(timestamp));

	if (sample)
	{
		for (const Measurement::ObjectId& objectId : sample->objectIds())
		{
			std::string description = sender->objectDescription(objectId);

			descriptionMap.emplace(std::move(description), objectId);
		}

		if (sample->referenceSystem() == Tracker6DOF::RS_DEVICE_IN_OBJECT)
		{
			dataSampleLayout_.referenceSystem.stage("DEVICE_IN_OBJECT");
		}
		else
		{
			ocean_assert(sample->referenceSystem() == Tracker6DOF::RS_OBJECT_IN_DEVICE);
			dataSampleLayout_.referenceSystem.stage("OBJECT_IN_DEVICE");
		}

		std::vector<vrs::Matrix4Dd> vrsMatrices4Dd;
		vrsMatrices4Dd.reserve(sample->objectIds().size());

		for (size_t n = 0; n < sample->objectIds().size(); ++n)
		{
			const HomogenousMatrix4 homogenousMatrix(sample->positions()[n], sample->orientations()[n]);

			vrsMatrices4Dd.emplace_back(IO::VRS::Utilities::homogenousMatrix4ToVRS<Scalar, double>(homogenousMatrix));
		}

		dataSampleLayout_.homogenousMatrix4.stage(std::move(vrsMatrices4Dd));

		dataSampleLayout_.objectIds.stage(sample->objectIds());

		const SceneTracker6DOF::SharedSceneElements& sampleSceneElements = sample->sceneElements();
		ocean_assert(sampleSceneElements.size() == sample->objectIds().size());

		{
			std::ostringstream stringStream(std::ios::binary);
			IO::OutputBitstream bitstream(stringStream);
			if (!writeSceneElementsToBitstream(sampleSceneElements, bitstream))
			{
				ocean_assert(false && "Failed to encode scene elements!");
			}

			dataSampleLayout_.sceneElements.stage(stringStream.str());
		}

		{
			std::ostringstream stringStream(std::ios::binary);
			IO::OutputBitstream bitstream(stringStream);
			if (!writeMetadataToBitstream(sample->metadata(), bitstream))
			{
				ocean_assert(false && "Failed to encode metadata!");
			}

			dataSampleLayout_.metadata.stage(stringStream.str());
		}
	}
	else
	{
		dataSampleLayout_.referenceSystem.stagedValue().clear();
		dataSampleLayout_.homogenousMatrix4.stagedValues().clear();
		dataSampleLayout_.objectIds.stagedValues().clear();
		dataSampleLayout_.sceneElements.stagedValue().clear();
		dataSampleLayout_.metadata.stagedValue().clear();
	}

	for (const Measurement::ObjectId& objectId : foundObjects)
	{
		std::string description = sender->objectDescription(objectId);

		descriptionMap.emplace(std::move(description), objectId);
	}

	for (const Measurement::ObjectId& objectId : lostObjects)
	{
		std::string description = sender->objectDescription(objectId);

		descriptionMap.emplace(std::move(description), objectId);
	}

	dataSampleLayout_.objectDescriptionMap.stage(std::move(descriptionMap));

	dataSampleLayout_.foundObjects.stage(std::vector<Measurement::ObjectId>(foundObjects.cbegin(), foundObjects.cend()));

	dataSampleLayout_.lostObjects.stage(std::vector<Measurement::ObjectId>(lostObjects.cbegin(), lostObjects.cend()));

	const Timestamp vrsRecordTimestamp = recordTimestamp ? Timestamp(*recordTimestamp) : Timestamp(true);

	const vrs::Record* record = createRecord(double(vrsRecordTimestamp), vrs::Record::Type::DATA, sampleDataLayoutVersion_, vrs::DataSource(dataSampleLayout_));

	ocean_assert_and_suppress_unused(record != nullptr, record);
}

std::string DeviceRecorder::RecordableSceneTracker6DOFSample::defaultFlavor()
{
	return std::string("ocean/devices/scenetracker6dofsample");
}

bool DeviceRecorder::RecordableSceneTracker6DOFSample::writeSceneElementsToBitstream(const SceneTracker6DOF::SharedSceneElements& sceneElements, IO::OutputBitstream& bitstream)
{
	if (!bitstream.write<unsigned long long>(sceneElementsTag_))
	{
		return false;
	}

	constexpr unsigned long long version = 1ull;
	if (!bitstream.write<unsigned long long>(version))
	{
		return false;
	}

	const unsigned int numberSceneElements = (unsigned int)(sceneElements.size());
	if (!bitstream.write<unsigned int>(numberSceneElements))
	{
		return false;
	}

	for (const SceneTracker6DOF::SharedSceneElement& sceneElement : sceneElements)
	{
		if (sceneElement)
		{
			switch (sceneElement->sceneElementType())
			{
				case SceneTracker6DOF::SceneElement::SET_INVALID:
					ocean_assert(false && "Invalid scene element!");
					break;

				case SceneTracker6DOF::SceneElement::SET_OBJECT_POINTS:
				{
					const SceneTracker6DOF::SceneElementObjectPoints& objectPoints = (const SceneTracker6DOF::SceneElementObjectPoints&)(*sceneElement);

					if (!writeObjectPointsToBitstream(objectPoints, bitstream))
					{
						return false;
					}

					continue;
				}

				case SceneTracker6DOF::SceneElement::SET_FEATURE_CORRESPONDENCES:
				{
					const SceneTracker6DOF::SceneElementFeatureCorrespondences& featureCorrespondences = (const SceneTracker6DOF::SceneElementFeatureCorrespondences&)(*sceneElement);

					if (!writeFeatureCorrespondencesToBitstream(featureCorrespondences, bitstream))
					{
						return false;
					}

					continue;
				}

				case SceneTracker6DOF::SceneElement::SET_PLANES:
				{
					const SceneTracker6DOF::SceneElementPlanes& planes = (const SceneTracker6DOF::SceneElementPlanes&)(*sceneElement);

					if (!writePlanesToBitstream(planes, bitstream))
					{
						return false;
					}

					continue;
				}

				case SceneTracker6DOF::SceneElement::SET_MESHES:
				{
					const SceneTracker6DOF::SceneElementMeshes& meshes = (const SceneTracker6DOF::SceneElementMeshes&)(*sceneElement);

					if (!writeMeshsToBitstream(meshes, bitstream))
					{
						return false;
					}

					continue;
				}

				case SceneTracker6DOF::SceneElement::SET_DEPTH:
				{
					const SceneTracker6DOF::SceneElementDepth& depth = (const SceneTracker6DOF::SceneElementDepth&)(*sceneElement);

					if (!writeDepthToBitstream(depth, bitstream))
					{
						return false;
					}

					continue;
				}

				case SceneTracker6DOF::SceneElement::SET_ROOM:
				{
					ocean_assert(false && "Missing implementation!");
					continue;
				}
			}
		}

		if (!bitstream.write<unsigned long long>(sceneElementEmptyTag_))
		{
			return false;
		}
	}

	return true;
}

bool DeviceRecorder::RecordableSceneTracker6DOFSample::readSceneElementsFromBitstream(IO::InputBitstream& bitstream, SceneTracker6DOF::SharedSceneElements& sceneElements)
{
	unsigned long long sceneElementsTag = 0ull;
	if (!bitstream.read<unsigned long long>(sceneElementsTag) || sceneElementsTag != sceneElementsTag_)
	{
		return false;
	}

	unsigned long long version = 0ull;
	if (!bitstream.read<unsigned long long>(version) || version != 1ull)
	{
		return false;
	}

	unsigned int numberSceneElements = 0u;
	if (!bitstream.read<unsigned int>(numberSceneElements))
	{
		return false;
	}

	sceneElements.clear();
	sceneElements.reserve(numberSceneElements);

	for (unsigned int n = 0u; n < numberSceneElements; ++n)
	{
		unsigned long long sceneElementTag = 0ull;
		if (!bitstream.look<unsigned long long>(sceneElementTag))
		{
			return false;
		}

		SceneTracker6DOF::SharedSceneElement sceneElement;

		switch (sceneElementTag)
		{
			case sceneElementEmptyTag_:
			{
				sceneElementTag = 0ull;
				if (!bitstream.read<unsigned long long>(sceneElementTag))
				{
					return false;
				}

				// nowthing to do here: sceneElement = nullptr;
				break;
			}

			case sceneElementObjectPointsTag_:
			{
				if (!readObjectPointsFromBitstream(bitstream, sceneElement))
				{
					return false;
				}

				break;
			}

			case sceneElementFeatureCorrespondencesTag_:
			{
				if (!readFeatureCorrespondencesFromBitstream(bitstream, sceneElement))
				{
					return false;
				}

				break;
			}

			case sceneElementPlanesTag_:
			{
				if (!readPlanesFromBitstream(bitstream, sceneElement))
				{
					return false;
				}

				break;
			}

			case sceneElementMeshesTag_:
			{
				if (!readMeshsFromBitstream(bitstream, sceneElement))
				{
					return false;
				}

				break;
			}

			case sceneElementDepthTag_:
			{
				if (!readDepthFromBitstream(bitstream, sceneElement))
				{
					return false;
				}

				break;
			}

			default:
				ocean_assert(false && "Unknown scene element!");
				return false;
		}

		sceneElements.emplace_back(std::move(sceneElement));
	}

	return true;
}

bool DeviceRecorder::RecordableSceneTracker6DOFSample::writeObjectPointsToBitstream(const SceneTracker6DOF::SceneElementObjectPoints& sceneElementObjectPoints, IO::OutputBitstream& bitstream)
{
	if (!bitstream.write<unsigned long long>(sceneElementObjectPointsTag_))
	{
		return false;
	}

	constexpr unsigned long long version = 1ull;
	if (!bitstream.write<unsigned long long>(version))
	{
		return false;
	}

	const unsigned int numberObjectPoints = (unsigned int)(sceneElementObjectPoints.objectPoints().size());
	if (!bitstream.write<unsigned int>(numberObjectPoints))
	{
		return false;
	}

	if (!writeVectorsF3(sceneElementObjectPoints.objectPoints(), bitstream))
	{
		return false;
	}

	const unsigned int numberObjectPointIds = (unsigned int)(sceneElementObjectPoints.objectPointIds().size());
	if (!bitstream.write<unsigned int>(numberObjectPointIds))
	{
		return false;
	}

	if (!bitstream.write(sceneElementObjectPoints.objectPointIds().data(), sceneElementObjectPoints.objectPointIds().size() * sizeof(Index64)))
	{
		return false;
	}

	return true;
}

bool DeviceRecorder::RecordableSceneTracker6DOFSample::writeFeatureCorrespondencesToBitstream(const SceneTracker6DOF::SceneElementFeatureCorrespondences& sceneElementFeatureCorrespondences, IO::OutputBitstream& bitstream)
{
	if (!bitstream.write<unsigned long long>(sceneElementFeatureCorrespondencesTag_))
	{
		return false;
	}

	constexpr unsigned long long version = 1ull;
	if (!bitstream.write<unsigned long long>(version))
	{
		return false;
	}

	const unsigned int numberObjectPoints = (unsigned int)(sceneElementFeatureCorrespondences.objectPoints().size());
	if (!bitstream.write<unsigned int>(numberObjectPoints))
	{
		return false;
	}

	if (!writeVectorsF3(sceneElementFeatureCorrespondences.objectPoints(), bitstream))
	{
		return false;
	}

	const unsigned int numberImagePoints = (unsigned int)(sceneElementFeatureCorrespondences.imagePoints().size());
	if (!bitstream.write<unsigned int>(numberImagePoints))
	{
		return false;
	}

	if (!writeVectorsF2(sceneElementFeatureCorrespondences.imagePoints(), bitstream))
	{
		return false;
	}

	const unsigned int numberObjectPointIds = (unsigned int)(sceneElementFeatureCorrespondences.objectPointIds().size());
	if (!bitstream.write<unsigned int>(numberObjectPointIds))
	{
		return false;
	}

	if (!bitstream.write(sceneElementFeatureCorrespondences.objectPointIds().data(), sceneElementFeatureCorrespondences.objectPointIds().size() * sizeof(Index64)))
	{
		return false;
	}

	return true;
}

bool DeviceRecorder::RecordableSceneTracker6DOFSample::writePlanesToBitstream(const SceneTracker6DOF::SceneElementPlanes& sceneElementPlanes, IO::OutputBitstream& bitstream)
{
	if (!bitstream.write<unsigned long long>(sceneElementPlanesTag_))
	{
		return false;
	}

	constexpr unsigned long long version = 1ull;
	if (!bitstream.write<unsigned long long>(version))
	{
		return false;
	}

	const unsigned int numberPlanes = (unsigned int)(sceneElementPlanes.planes().size());
	if (!bitstream.write<unsigned int>(numberPlanes))
	{
		return false;
	}

	for (const SceneTracker6DOF::SceneElementPlanes::Plane& plane : sceneElementPlanes.planes())
	{
		if (!bitstream.write<unsigned int>(plane.planeId()))
		{
			return false;
		}

		std::string planeType = "UNKNOWN";
		if (plane.planeType() == SceneTracker6DOF::SceneElementPlanes::Plane::PT_HORIZONTAL)
		{
			planeType = "HORIZONTAL";
		}
		else if (plane.planeType() == SceneTracker6DOF::SceneElementPlanes::Plane::PT_VERTICAL)
		{
			planeType = "VERTICAL";
		}

		ocean_assert(!planeType.empty());
		if (!bitstream.write<std::string>(planeType))
		{
			return false;
		}

		const HomogenousMatrixD4 world_T_plane(plane.world_T_plane());
		if (!bitstream.write(world_T_plane.data(), sizeof(HomogenousMatrixD4)))
		{
			return false;
		}

		const VectorD3 boundingBoxLower(plane.boundingBox().lower());
		const VectorD3 boundingBoxHigher(plane.boundingBox().higher());

		if (!bitstream.write(boundingBoxLower.data(), sizeof(VectorD3)) || !bitstream.write(boundingBoxHigher.data(), sizeof(VectorD3)))
		{
			return false;
		}

		const unsigned int numberVertices = (unsigned int)(plane.vertices().size());
		if (!bitstream.write<unsigned int>(numberVertices))
		{
			return false;
		}

		if (!writeVectorsF3(plane.vertices(), bitstream))
		{
			return false;
		}

		const unsigned int numberTextureCoordinates = (unsigned int)(plane.textureCoordinates().size());
		if (!bitstream.write<unsigned int>(numberTextureCoordinates))
		{
			return false;
		}

		if (!writeVectorsF2(plane.textureCoordinates(), bitstream))
		{
			return false;
		}

		const unsigned int numberTriangleIndices = (unsigned int)(plane.triangleIndices().size());
		if (!bitstream.write<unsigned int>(numberTriangleIndices))
		{
			return false;
		}

		if (!bitstream.write(plane.triangleIndices().data(), plane.triangleIndices().size() * sizeof(Index32)))
		{
			return false;
		}

		const unsigned int numberBoundaryVertices = (unsigned int)(plane.boundaryVertices().size());
		if (!bitstream.write<unsigned int>(numberBoundaryVertices))
		{
			return false;
		}

		if (!writeVectorsF3(plane.boundaryVertices(), bitstream))
		{
			return false;
		}
	}

	return true;
}

bool DeviceRecorder::RecordableSceneTracker6DOFSample::writeMeshsToBitstream(const SceneTracker6DOF::SceneElementMeshes& sceneElementMeshes, IO::OutputBitstream& bitstream)
{
	if (!bitstream.write<unsigned long long>(sceneElementMeshesTag_))
	{
		return false;
	}

	constexpr unsigned long long version = 1ull;
	if (!bitstream.write<unsigned long long>(version))
	{
		return false;
	}

	const unsigned int numberMeshes = (unsigned int)(sceneElementMeshes.meshes().size());
	if (!bitstream.write<unsigned int>(numberMeshes))
	{
		return false;
	}

	for (const SceneTracker6DOF::SceneElementMeshes::SharedMesh& mesh : sceneElementMeshes.meshes())
	{
		ocean_assert(mesh);

		if (!bitstream.write<unsigned int>(mesh->meshId()))
		{
			return false;
		}

		const HomogenousMatrixD4 world_T_mesh(mesh->world_T_mesh());
		if (!bitstream.write(world_T_mesh.data(), sizeof(HomogenousMatrixD4)))
		{
			return false;
		}

		const unsigned int numberVertices = (unsigned int)(mesh->vertices().size());
		if (!bitstream.write<unsigned int>(numberVertices))
		{
			return false;
		}

		if (!writeVectorsF3(mesh->vertices(), bitstream))
		{
			return false;
		}

		const unsigned int numberNormalsPerVertex = (unsigned int)(mesh->perVertexNormals().size());
		if (!bitstream.write<unsigned int>(numberNormalsPerVertex))
		{
			return false;
		}

		if (!writeVectorsF3(mesh->perVertexNormals(), bitstream))
		{
			return false;
		}

		const unsigned int numberTriangleIndices = (unsigned int)(mesh->triangleIndices().size());
		if (!bitstream.write<unsigned int>(numberTriangleIndices))
		{
			return false;
		}

		if (!bitstream.write(mesh->triangleIndices().data(), mesh->triangleIndices().size() * sizeof(Index32)))
		{
			return false;
		}

		constexpr unsigned int numberVertexTypes = 0u; // placeholder for now
		if (!bitstream.write<unsigned int>(numberVertexTypes))
		{
			return false;
		}
	}

	return true;
}

bool DeviceRecorder::RecordableSceneTracker6DOFSample::writeDepthToBitstream(const SceneTracker6DOF::SceneElementDepth& sceneElementDepth, IO::OutputBitstream& bitstream)
{
	if (!bitstream.write<unsigned long long>(sceneElementDepthTag_))
	{
		return false;
	}

	constexpr unsigned long long version = 1ull;
	if (!bitstream.write<unsigned long long>(version))
	{
		return false;
	}

	std::shared_ptr<Frame> confidenceFrame;
	std::shared_ptr<Frame> depthFrame = sceneElementDepth.depth(&confidenceFrame);

	if (!depthFrame)
	{
		ocean_assert(false && "Invalid depth frame!");
		return false;
	}

	if (!sceneElementDepth.camera())
	{
		ocean_assert(false && "Invalid camera!");
		return false;
	}

	const SharedAnyCameraD cameraD = sceneElementDepth.camera()->cloneToDouble();

	if (!cameraD)
	{
		ocean_assert(false && "Failed to clone camera");
		return false;
	}

	std::string jsonCameraCalibration;
	if (!IO::VRS::RecordableCamera::exportCameraToPerceptionJSON(*cameraD, HomogenousMatrixD4(sceneElementDepth.device_T_depth()), jsonCameraCalibration))
	{
		return false;
	}

	ocean_assert(depthFrame->width() == cameraD->width() && depthFrame->height() == cameraD->height());
	ocean_assert(!confidenceFrame || (depthFrame->width() == confidenceFrame->width() && depthFrame->height() == confidenceFrame->height()));

	if (depthFrame->numberPlanes() != 1u)
	{
		ocean_assert(false && "Invalid plane number!");
		return false;
	}

	const uint64_t depthFrameSize = uint64_t(depthFrame->planeWidthBytes(0u)) * uint64_t(depthFrame->height());
	static_assert(sizeof(uint64_t) == sizeof(unsigned long long), "Invalid data type!");

	const std::string depthFramePixelFormat = FrameType::translatePixelFormat(depthFrame->pixelFormat());
	const std::string depthFramePixelOrigin = FrameType::translatePixelOrigin(depthFrame->pixelOrigin());

	if (depthFramePixelFormat == "UNDEFINED" || depthFramePixelOrigin == "INVALID")
	{
		return false;
	}

	if (!bitstream.write<unsigned long long>((unsigned long long)(depthFrameSize)))
	{
		return false;
	}

	if (!bitstream.write<unsigned int>(depthFrame->width()))
	{
		return false;
	}

	if (!bitstream.write<unsigned int>(depthFrame->height()))
	{
		return false;
	}

	if (!bitstream.write<std::string>(depthFramePixelFormat))
	{
		return false;
	}

	if (!bitstream.write<std::string>(depthFramePixelOrigin))
	{
		return false;
	}

	for (unsigned int y = 0u; y < depthFrame->height(); ++y)
	{
		if (!bitstream.write(depthFrame->constrow<void>(y), depthFrame->planeWidthBytes(0u)))
		{
			return false;
		}
	}

	if (confidenceFrame)
	{
		const uint64_t confidenceFrameSize = uint64_t(confidenceFrame->planeWidthBytes(0u)) * uint64_t(confidenceFrame->height());

		const std::string confidenceFramePixelFormat = FrameType::translatePixelFormat(confidenceFrame->pixelFormat());
		const std::string confidenceFramePixelOrigin = FrameType::translatePixelOrigin(confidenceFrame->pixelOrigin());

		if (confidenceFramePixelFormat == "UNDEFINED" || confidenceFramePixelOrigin == "INVALID")
		{
			return false;
		}

		if (!bitstream.write<unsigned long long>((unsigned long long)(confidenceFrameSize)))
		{
			return false;
		}

		if (!bitstream.write<unsigned int>(confidenceFrame->width()))
		{
			return false;
		}

		if (!bitstream.write<unsigned int>(confidenceFrame->height()))
		{
			return false;
		}

		if (!bitstream.write<std::string>(confidenceFramePixelFormat))
		{
			return false;
		}

		if (!bitstream.write<std::string>(confidenceFramePixelOrigin))
		{
			return false;
		}

		for (unsigned int y = 0u; y < confidenceFrame->height(); ++y)
		{
			if (!bitstream.write(confidenceFrame->constrow<void>(y), confidenceFrame->planeWidthBytes(0u)))
			{
				return false;
			}
		}
	}
	else
	{
		const uint64_t confidenceFrameSize = 0ull;
		static_assert(sizeof(uint64_t) == sizeof(unsigned long long), "Invalid data type!");

		if (!bitstream.write<unsigned long long>((unsigned long long)(confidenceFrameSize)))
		{
			return false;
		}
	}

	if (!bitstream.write<std::string>(jsonCameraCalibration))
	{
		return false;
	}

	return true;
}

bool DeviceRecorder::RecordableSceneTracker6DOFSample::readObjectPointsFromBitstream(IO::InputBitstream& bitstream, SceneTracker6DOF::SharedSceneElement& sceneElement)
{
	unsigned long long tag = 0ull;
	if (!bitstream.read<unsigned long long>(tag) || tag != sceneElementObjectPointsTag_)
	{
		return false;
	}

	unsigned long long version = 0ull;
	if (!bitstream.read<unsigned long long>(version) || version != 1ull)
	{
		return false;
	}

	unsigned int numberObjectPoints = 0u;
	if (!bitstream.read<unsigned int>(numberObjectPoints))
	{
		return false;
	}

	Vectors3 objectPoints;
	if (!readVectorsF3(bitstream, size_t(numberObjectPoints), objectPoints))
	{
		return false;
	}

	unsigned int numberObjectPointIds = 0u;
	if (!bitstream.read<unsigned int>(numberObjectPointIds))
	{
		return false;
	}

	if (numberObjectPointIds != 0u && numberObjectPoints != numberObjectPointIds)
	{
		// we expect either no object point ids, or exactly the same number of ids as we have object points
		return false;
	}

	Indices64 objectPointIds(numberObjectPointIds);
	if (!bitstream.read(objectPointIds.data(), objectPointIds.size() * sizeof(Index64)))
	{
		return false;
	}

	sceneElement = std::make_shared<SceneTracker6DOF::SceneElementObjectPoints>(std::move(objectPoints), std::move(objectPointIds));

	return true;
}

bool DeviceRecorder::RecordableSceneTracker6DOFSample::readFeatureCorrespondencesFromBitstream(IO::InputBitstream& bitstream, SceneTracker6DOF::SharedSceneElement& sceneElement)
{
	unsigned long long tag = 0ull;
	if (!bitstream.read<unsigned long long>(tag) || tag != sceneElementFeatureCorrespondencesTag_)
	{
		return false;
	}

	unsigned long long version = 0ull;
	if (!bitstream.read<unsigned long long>(version) || version != 1ull)
	{
		return false;
	}

	unsigned int numberObjectPoints = 0u;
	if (!bitstream.read<unsigned int>(numberObjectPoints))
	{
		return false;
	}

	Vectors3 objectPoints;
	if (!readVectorsF3(bitstream, size_t(numberObjectPoints), objectPoints))
	{
		return false;
	}

	unsigned int numberImagePoints = 0u;
	if (!bitstream.read<unsigned int>(numberImagePoints))
	{
		return false;
	}

	if (numberObjectPoints != numberImagePoints)
	{
		return false;
	}

	Vectors2 imagePoints;
	if (!readVectorsF2(bitstream, size_t(numberImagePoints), imagePoints))
	{
		return false;
	}

	unsigned int numberObjectPointIds = 0u;
	if (!bitstream.read<unsigned int>(numberObjectPointIds))
	{
		return false;
	}

	if (numberObjectPointIds != 0u && numberObjectPoints != numberObjectPointIds)
	{
		// we expect either no object point ids, or exactly the same number of ids as we have object points
		return false;
	}

	Indices64 objectPointIds(numberObjectPointIds);
	if (!bitstream.read(objectPointIds.data(), objectPointIds.size() * sizeof(Index64)))
	{
		return false;
	}

	sceneElement = std::make_shared<SceneTracker6DOF::SceneElementFeatureCorrespondences>(std::move(objectPoints), std::move(imagePoints), std::move(objectPointIds));

	return true;
}

bool DeviceRecorder::RecordableSceneTracker6DOFSample::readPlanesFromBitstream(IO::InputBitstream& bitstream, SceneTracker6DOF::SharedSceneElement& sceneElement)
{
	unsigned long long tag = 0ull;
	if (!bitstream.read<unsigned long long>(tag) || tag != sceneElementPlanesTag_)
	{
		return false;
	}

	unsigned long long version = 0ull;
	if (!bitstream.read<unsigned long long>(version) || version != 1ull)
	{
		return false;
	}

	unsigned int numberPlanes = 0u;
	if (!bitstream.read<unsigned int>(numberPlanes))
	{
		return false;
	}

	SceneTracker6DOF::SceneElementPlanes::Planes planes;
	planes.reserve(numberPlanes);

	for (unsigned int nPlane = 0u; nPlane < numberPlanes; ++nPlane)
	{
		unsigned int planeId = (unsigned int)(-1);
		if (!bitstream.read<unsigned int>(planeId))
		{
			return false;
		}

		SceneTracker6DOF::SceneElementPlanes::Plane::PlaneType planeType = SceneTracker6DOF::SceneElementPlanes::Plane::PT_UNKNOWN;

		std::string planeTypeString;
		if (!bitstream.read<std::string>(planeTypeString))
		{
			return false;
		}

		if (planeTypeString == "HORIZONTAL")
		{
			planeType = SceneTracker6DOF::SceneElementPlanes::Plane::PT_HORIZONTAL;
		}
		else if (planeTypeString == "VERTICAL")
		{
			planeType = SceneTracker6DOF::SceneElementPlanes::Plane::PT_VERTICAL;
		}
		else if (planeTypeString != "UNKNOWN")
		{
			return false;
		}

		HomogenousMatrixD4 world_T_plane(false);
		if (!bitstream.read(world_T_plane.data(), sizeof(HomogenousMatrixD4)))
		{
			return false;
		}

		if (NumericD::isEqual(world_T_plane[15], 1.0, double(NumericF::weakEps()))) // handle potential rounding issues between float32 and float64
		{
			world_T_plane[15] = 1;
		}

		if (!world_T_plane.isValid())
		{
			return false;
		}

		VectorD3 boundingBoxLower;
		VectorD3 boundingBoxHigher;
		if (!bitstream.read(boundingBoxLower.data(), sizeof(VectorD3)) || !bitstream.read(boundingBoxHigher.data(), sizeof(VectorD3)))
		{
			return false;
		}

		const Box3 boundingBox = Box3(Vector3(boundingBoxLower), Vector3(boundingBoxHigher));
		if (!boundingBox.isValid())
		{
			return false;
		}

		unsigned int numberVertices = 0u;
		if (!bitstream.read<unsigned int>(numberVertices))
		{
			return false;
		}

		Vectors3 vertices;
		if (!readVectorsF3(bitstream, size_t(numberVertices), vertices))
		{
			return false;
		}

		unsigned int numberTextureCoordinates = 0u;
		if (!bitstream.read<unsigned int>(numberTextureCoordinates))
		{
			return false;
		}

		Vectors2 textureCoordinates;
		if (!readVectorsF2(bitstream, size_t(numberTextureCoordinates), textureCoordinates))
		{
			return false;
		}

		unsigned int numberTriangleIndices = 0u;
		if (!bitstream.read<unsigned int>(numberTriangleIndices))
		{
			return false;
		}

		Indices32 triangleIndices(numberTriangleIndices);
		if (!bitstream.read(triangleIndices.data(), triangleIndices.size() * sizeof(Index32)))
		{
			return false;
		}

		unsigned int numberBoundaryVertices = 0u;
		if (!bitstream.read<unsigned int>(numberBoundaryVertices))
		{
			return false;
		}

		Vectors3 boundaryVertices;
		if (!readVectorsF3(bitstream, size_t(numberBoundaryVertices), boundaryVertices))
		{
			return false;
		}

		planes.emplace_back(planeId, planeType, HomogenousMatrix4(world_T_plane), boundingBox, std::move(vertices), std::move(textureCoordinates), std::move(triangleIndices), std::move(boundaryVertices));
	}

	sceneElement = std::make_shared<SceneTracker6DOF::SceneElementPlanes>(std::move(planes));

	return true;
}

bool DeviceRecorder::RecordableSceneTracker6DOFSample::readMeshsFromBitstream(IO::InputBitstream& bitstream, SceneTracker6DOF::SharedSceneElement& sceneElement)
{
	unsigned long long tag = 0ull;
	if (!bitstream.read<unsigned long long>(tag) || tag != sceneElementMeshesTag_)
	{
		return false;
	}

	unsigned long long version = 0ull;
	if (!bitstream.read<unsigned long long>(version) || version != 1ull)
	{
		return false;
	}

	unsigned int numberMeshes = 0u;
	if (!bitstream.read<unsigned int>(numberMeshes))
	{
		return false;
	}

	SceneTracker6DOF::SceneElementMeshes::SharedMeshes meshes;
	meshes.reserve(numberMeshes);

	for (unsigned int nMesh = 0u; nMesh < numberMeshes; ++nMesh)
	{
		unsigned int meshId = (unsigned int)(-1);
		if (!bitstream.read<unsigned int>(meshId))
		{
			return false;
		}

		HomogenousMatrixD4 world_T_mesh(false);
		if (!bitstream.read(world_T_mesh.data(), sizeof(HomogenousMatrixD4)))
		{
			return false;
		}

		if (NumericD::isEqual(world_T_mesh[15], 1.0, double(NumericF::weakEps())))
		{
			world_T_mesh[15] = 1;
		}

		if (!world_T_mesh.isValid())
		{
			return false;
		}

		unsigned int numberVertices = 0u;
		if (!bitstream.read<unsigned int>(numberVertices))
		{
			return false;
		}

		Vectors3 vertices;
		if (!readVectorsF3(bitstream, size_t(numberVertices), vertices))
		{
			return false;
		}

		unsigned int numberNormalsPerVertex = 0u;
		if (!bitstream.read<unsigned int>(numberNormalsPerVertex))
		{
			return false;
		}

		Vectors3 normalsPerVertex;
		if (!readVectorsF3(bitstream, size_t(numberNormalsPerVertex), normalsPerVertex))
		{
			return false;
		}

		unsigned int numberTriangleIndices = 0u;
		if (!bitstream.read<unsigned int>(numberTriangleIndices))
		{
			return false;
		}

		Indices32 triangleIndices(numberTriangleIndices);
		if (!bitstream.read(triangleIndices.data(), triangleIndices.size() * sizeof(Index32)))
		{
			return false;
		}

		unsigned int numberVertexTypes = 0u;
		if (!bitstream.read<unsigned int>(numberVertexTypes))
		{
			return false;
		}

		if (numberVertexTypes != 0u)
		{
			return false;
		}

		meshes.emplace_back(std::make_shared<SceneTracker6DOF::SceneElementMeshes::Mesh>(meshId, HomogenousMatrix4(world_T_mesh), std::move(vertices), std::move(normalsPerVertex), std::move(triangleIndices)));
	}

	sceneElement = std::make_shared<SceneTracker6DOF::SceneElementMeshes>(std::move(meshes));

	return true;
}

bool DeviceRecorder::RecordableSceneTracker6DOFSample::readDepthFromBitstream(IO::InputBitstream& bitstream, SceneTracker6DOF::SharedSceneElement& sceneElement)
{
	unsigned long long tag = 0ull;
	if (!bitstream.read<unsigned long long>(tag) || tag != sceneElementDepthTag_)
	{
		return false;
	}

	unsigned long long version = 0ull;
	if (!bitstream.read<unsigned long long>(version) || version != 1ull)
	{
		return false;
	}

	unsigned long long depthFrameSize = 0ull;
	if (!bitstream.read<unsigned long long>(depthFrameSize))
	{
		return false;
	}

	unsigned int depthFrameWidth = 0u;
	if (!bitstream.read<unsigned int>(depthFrameWidth))
	{
		return false;
	}

	unsigned int depthFrameHeight = 0u;
	if (!bitstream.read<unsigned int>(depthFrameHeight))
	{
		return false;
	}

	constexpr unsigned int maxFrameResolution = 1920u * 4u;

	if (depthFrameWidth == 0u || depthFrameWidth > maxFrameResolution || depthFrameHeight == 0u || depthFrameHeight > maxFrameResolution)
	{
		return false;
	}

	std::string depthFramePixelFormatString;
	if (!bitstream.read<std::string>(depthFramePixelFormatString))
	{
		return false;
	}

	std::string depthFramePixelOriginString;
	if (!bitstream.read<std::string>(depthFramePixelOriginString))
	{
		return false;
	}

	const FrameType::PixelFormat depthFramePixelFormat = FrameType::translatePixelFormat(depthFramePixelFormatString);
	const FrameType::PixelOrigin depthFramePixelOrigin = FrameType::translatePixelOrigin(depthFramePixelOriginString);

	if (depthFramePixelFormat == FrameType::FORMAT_UNDEFINED || depthFramePixelOrigin == FrameType::ORIGIN_INVALID)
	{
		return false;
	}

	Frame depthFrame(FrameType(depthFrameWidth, depthFrameHeight, depthFramePixelFormat, depthFramePixelOrigin));

	if (uint64_t(depthFrame.size()) != depthFrameSize)
	{
		return false;
	}

	if (!bitstream.read(depthFrame.data<void>(), depthFrame.size()))
	{
		return false;
	}

	std::shared_ptr<Frame> sharedConfidenceFrame;

	unsigned long long confidenceFrameSize = 0ull;
	if (!bitstream.read<unsigned long long>(confidenceFrameSize))
	{
		return false;
	}

	if (confidenceFrameSize != 0ull)
	{
		unsigned int confidenceFrameWidth = 0u;
		if (!bitstream.read<unsigned int>(confidenceFrameWidth))
		{
			return false;
		}

		unsigned int confidenceFrameHeight = 0u;
		if (!bitstream.read<unsigned int>(confidenceFrameHeight))
		{
			return false;
		}

		if (confidenceFrameWidth == 0u || confidenceFrameWidth > maxFrameResolution || confidenceFrameHeight == 0u || confidenceFrameHeight > maxFrameResolution)
		{
			return false;
		}

		std::string confidenceFramePixelFormatString;
		if (!bitstream.read<std::string>(confidenceFramePixelFormatString))
		{
			return false;
		}

		std::string confidenceFramePixelOriginString;
		if (!bitstream.read<std::string>(confidenceFramePixelOriginString))
		{
			return false;
		}

		const FrameType::PixelFormat confidenceFramePixelFormat = FrameType::translatePixelFormat(confidenceFramePixelFormatString);
		const FrameType::PixelOrigin confidenceFramePixelOrigin = FrameType::translatePixelOrigin(confidenceFramePixelOriginString);

		if (confidenceFramePixelFormat == FrameType::FORMAT_UNDEFINED || confidenceFramePixelOrigin == FrameType::ORIGIN_INVALID)
		{
			return false;
		}

		Frame confidenceFrame(FrameType(confidenceFrameWidth, confidenceFrameHeight, confidenceFramePixelFormat, confidenceFramePixelOrigin));

		if (uint64_t(confidenceFrame.size()) != confidenceFrameSize)
		{
			return false;
		}

		if (!bitstream.read(confidenceFrame.data<void>(), confidenceFrame.size()))
		{
			return false;
		}

		sharedConfidenceFrame = std::make_shared<Frame>(std::move(confidenceFrame));
	}

	std::string jsonCameraCalibration;
	if (!bitstream.read<std::string>(jsonCameraCalibration))
	{
		return false;
	}

	SharedAnyCameraD anyCameraD;
	HomogenousMatrixD4 device_T_depth(false);
	if (!IO::VRS::RecordableCamera::importCameraFromPerceptionJSON(jsonCameraCalibration, anyCameraD, device_T_depth))
	{
		return false;
	}

	std::shared_ptr<Frame> sharedDepthFrame = std::make_shared<Frame>(std::move(depthFrame));

#ifdef OCEAN_MATH_USE_SINGLE_PRECISION
	SharedAnyCamera anyCamera = anyCameraD->cloneToFloat();
#else
	SharedAnyCamera anyCamera = anyCameraD;
#endif

	sceneElement = std::make_shared<SceneTracker6DOF::SceneElementDepth>(std::move(anyCamera), HomogenousMatrix4(device_T_depth), std::move(sharedDepthFrame), std::move(sharedConfidenceFrame));

	return true;
}

bool DeviceRecorder::RecordableSceneTracker6DOFSample::writeVectorsF3(const Vectors3& vectors, IO::OutputBitstream& bitstream)
{
	if (std::is_same<Scalar, float>::value)
	{
		if (!bitstream.write(vectors.data(), vectors.size() * sizeof(VectorF3)))
		{
			return false;
		}
	}
	else
	{
		VectorsF3 vectorsF;
		vectorsF.reserve(vectors.size());

		for (const Vector3& vector : vectors)
		{
			vectorsF.emplace_back(float(vector.x()), float(vector.y()), float(vector.z()));
		}

		if (!bitstream.write(vectorsF.data(), vectorsF.size() * sizeof(VectorF3)))
		{
			return false;
		}
	}

	return true;
}

bool DeviceRecorder::RecordableSceneTracker6DOFSample::writeVectorsF2(const Vectors2& vectors, IO::OutputBitstream& bitstream)
{
	if (std::is_same<Scalar, float>::value)
	{
		if (!bitstream.write(vectors.data(), vectors.size() * sizeof(VectorF2)))
		{
			return false;
		}
	}
	else
	{
		VectorsF2 vectorsF;
		vectorsF.reserve(vectors.size());

		for (const Vector2& vector : vectors)
		{
			vectorsF.emplace_back(float(vector.x()), float(vector.y()));
		}

		if (!bitstream.write(vectorsF.data(), vectorsF.size() * sizeof(VectorF2)))
		{
			return false;
		}
	}

	return true;
}

bool DeviceRecorder::RecordableSceneTracker6DOFSample::readVectorsF3(IO::InputBitstream& bitstream, const size_t size, Vectors3& vectors)
{
	if (std::is_same<Scalar, float>::value)
	{
		vectors.resize(size);
		if (!bitstream.read(vectors.data(), vectors.size() * sizeof(VectorF3)))
		{
			return false;
		}
	}
	else
	{
		VectorsF3 vectorsF(size);
		if (!bitstream.read(vectorsF.data(), vectorsF.size() * sizeof(VectorF3)))
		{
			return false;
		}

		vectors.reserve(vectorsF.size());
		for (const VectorF3& vectorF : vectorsF)
		{
			vectors.emplace_back(Scalar(vectorF.x()), Scalar(vectorF.y()), Scalar(vectorF.z()));
		}
	}

	return true;
}

bool DeviceRecorder::RecordableSceneTracker6DOFSample::readVectorsF2(IO::InputBitstream& bitstream, const size_t size, Vectors2& vectors)
{
	if (std::is_same<Scalar, float>::value)
	{
		vectors.resize(size);
		if (!bitstream.read(vectors.data(), vectors.size() * sizeof(VectorF2)))
		{
			return false;
		}
	}
	else
	{
		VectorsF2 vectorsF(size);
		if (!bitstream.read(vectorsF.data(), vectorsF.size() * sizeof(VectorF2)))
		{
			return false;
		}

		vectors.reserve(vectorsF.size());
		for (const VectorF2& vectorF : vectorsF)
		{
			vectors.emplace_back(Scalar(vectorF.x()), Scalar(vectorF.y()));
		}
	}

	return true;
}

DeviceRecorder::DeviceRecorder() :
	flexibleRecorder_(std::make_shared<IO::VRS::FlexibleRecorder>())
{
	DeviceRefManager::get().addDeviceCallbackFunction(DeviceRefManager::DeviceCallback::create(*this, &DeviceRecorder::onDeviceChanged));
	callbackEventDeviceChangedRegistered_ = true;
}

DeviceRecorder::~DeviceRecorder()
{
	release();
}

bool DeviceRecorder::start(const std::string& filename)
{
	const ScopedLock scopedLock(recorderLock_);

	ocean_assert(flexibleRecorder_);

	if (isStarted_)
	{
		return true;
	}

	isStarted_ = true;

	if (!flexibleRecorder_->start(filename))
	{
		return false;
	}

	startThread();

	return true;
}

bool DeviceRecorder::stop()
{
	const ScopedLock recorderScopedLock(recorderLock_);

	if (!isStarted_)
	{
		return false;
	}

	isStarted_ = false;

	recordPendingSamples();

	stopThreadExplicitly();

	if (!flexibleRecorder_->stop())
	{
		return false;
	}

	for (FrameMediumMap::iterator i = frameMediumMap_.begin(); i != frameMediumMap_.end(); ++i)
	{
		FrameMediumData& frameMediumData = i->second;

		frameMediumData.lastTimestamp_.toInvalid();
		frameMediumData.recordableIndex_ = (unsigned int)(-1);
	}

	measurementRecordableMap_.clear();

	flexibleRecorder_ = std::make_shared<IO::VRS::FlexibleRecorder>();

	return true;
}

bool DeviceRecorder::addFrameMedium(const Media::FrameMediumRef& frameMedium)
{
	if (frameMedium.isNull())
	{
		ocean_assert(false && "Invalid medium!");
		return false;
	}

	const ScopedLock scopedLock(frameMediumMapLock_);

	FrameMediumMap::iterator i = frameMediumMap_.find(&*frameMedium);

	if (i == frameMediumMap_.cend())
	{
		i = frameMediumMap_.emplace(&*frameMedium, FrameMediumData(frameMedium)).first;
	}

	++i->second.referenceCounter_;
	return true;
}

bool DeviceRecorder::removeFrameMedium(const Media::FrameMediumRef& frameMedium)
{
	if (frameMedium.isNull())
	{
		ocean_assert(false && "Invalid medium!");
		return false;
	}

	const ScopedLock scopedLock(frameMediumMapLock_);

	FrameMediumMap::iterator i = frameMediumMap_.find(&*frameMedium);

	if (i == frameMediumMap_.cend())
	{
		ocean_assert(false && "The medium is unknown!");
		return false;
	}

	ocean_assert(i->second.referenceCounter_ > 0);
	--i->second.referenceCounter_;

	if (i->second.referenceCounter_ == 0u)
	{
		frameMediumMap_.erase(i);
	}

	return true;
}

void DeviceRecorder::release()
{
	if (callbackEventDeviceChangedRegistered_)
	{
		DeviceRefManager::get().removeDeviceCallbackFunction(DeviceRefManager::DeviceCallback::create(*this, &DeviceRecorder::onDeviceChanged));
		callbackEventDeviceChangedRegistered_ = false;
	}

	stop();

	TemporaryScopedLock scopedLock(recorderLock_);
		sampleEventSubscriptionMap_.clear();
		trackerObjectEventSubscriptionMap_.clear();
	scopedLock.release();

	stopThreadExplicitly();
}

void DeviceRecorder::onDeviceChanged(Device* device, bool added)
{
	ocean_assert(device != nullptr);

	if ((device->type().majorType() & Device::DEVICE_MEASUREMENT) != Device::DEVICE_MEASUREMENT)
	{
		// we record measurement devices only
		return;
	}

	Measurement* measurement = dynamic_cast<Measurement*>(device);
	ocean_assert(measurement != nullptr);

	Tracker* tracker = dynamic_cast<Tracker*>(device);

	const ScopedLock scopedLock(recorderLock_);

	if (added)
	{
		ocean_assert(sampleEventSubscriptionMap_.find(device) == sampleEventSubscriptionMap_.cend());

		Measurement::SampleEventSubscription sampleEventSubscription(measurement->subscribeSampleEvent(Measurement::SampleCallback::create(*this, &DeviceRecorder::onMeasurementSample)));
		sampleEventSubscription.makeWeak();

		sampleEventSubscriptionMap_.emplace(device, std::move(sampleEventSubscription));

		if (tracker)
		{
			ocean_assert(trackerObjectEventSubscriptionMap_.find(tracker) == trackerObjectEventSubscriptionMap_.cend());

			Tracker::TrackerObjectEventSubscription objectEventSubscription(tracker->subscribeTrackerObjectEvent(Tracker::TrackerObjectCallback::create(*this, &DeviceRecorder::onTrackerObject)));
			objectEventSubscription.makeWeak();

			trackerObjectEventSubscriptionMap_.emplace(device, std::move(objectEventSubscription));
		}
	}
	else
	{
		// we need to ensure that we do not have any samples in the queue associated with the device
		recordPendingSamples();

		SampleEventSubscriptionMap::iterator iDeviceSample = sampleEventSubscriptionMap_.find(device);
		ocean_assert(iDeviceSample != sampleEventSubscriptionMap_.cend());
		sampleEventSubscriptionMap_.erase(iDeviceSample);

		if (tracker)
		{
			TrackerObjectEventSubscriptionMap::iterator iTrackerObject = trackerObjectEventSubscriptionMap_.find(device);
			ocean_assert(iTrackerObject != trackerObjectEventSubscriptionMap_.cend());
			trackerObjectEventSubscriptionMap_.erase(iTrackerObject);
		}

		measurementRecordableMap_.erase(measurement);

		invalidMeasurements_.emplace(measurement);
	}
}

void DeviceRecorder::onMeasurementSample(const Measurement* sender, const Measurement::SampleRef& sample)
{
	ocean_assert(sender != nullptr);
	ocean_assert(sample);

	if (!isStarted_)
	{
		return;
	}

	TemporaryScopedLock unqueuedScopedLock(unqueuedSampleDatasLock_);

		unqueuedSampleDatas_.emplace_back(sender, sample, sample->timestamp());

		if (unqueuedSampleDatas_.size() < 20)
		{
			return;
		}

		// we have a chunk for samples which now can be moved to the real queue
		// this two step approach is supposed to reduce waiting time for locks

		SampleDatas unqueuedSampleDatas = std::move(unqueuedSampleDatas_);

	unqueuedScopedLock.release();

	ocean_assert(!unqueuedSampleDatas.empty());

	const ScopedLock queueScopedLock(sampleDataQueueLock_);

	sampleDataQueue_.emplace(std::move(unqueuedSampleDatas));
}

void DeviceRecorder::onTrackerObject(const Tracker* sender, const bool found, const Measurement::ObjectIdSet& objectIds, const Timestamp& timestamp)
{
	ocean_assert(sender != nullptr);

	if (!isStarted_)
	{
		return;
	}

	const Measurement::ObjectIdSet& foundObjectIds = found ? objectIds : Measurement::ObjectIdSet();
	const Measurement::ObjectIdSet& lostObjectIds = found ? Measurement::ObjectIdSet() : objectIds;

	TemporaryScopedLock unqueuedScopedLock(unqueuedSampleDatasLock_);

		unqueuedSampleDatas_.emplace_back(sender, foundObjectIds, lostObjectIds, timestamp);

		if (unqueuedSampleDatas_.size() < 20)
		{
			return;
		}

		// we have a chunk for samples which now can be moved to the real queue
		// this two step approach is supposed to reduce waiting time for locks

		SampleDatas unqueuedSampleDatas = std::move(unqueuedSampleDatas_);

	unqueuedScopedLock.release();

	ocean_assert(!unqueuedSampleDatas.empty());

	const ScopedLock queueScopedLock(sampleDataQueueLock_);

	sampleDataQueue_.emplace(std::move(unqueuedSampleDatas));
}

void DeviceRecorder::recordSample(const Measurement* sender, const Measurement::SampleRef& sample, const Measurement::ObjectIdSet& foundObjects, const Measurement::ObjectIdSet& lostObjects, const Timestamp& timestamp)
{
	ocean_assert(sender != nullptr);

	switch (sender->type().majorType())
	{
		case Device::DEVICE_SENSOR:
			recordSensorSample(sender, sample, foundObjects, lostObjects, timestamp);
			break;

		case Device::DEVICE_TRACKER:
			recordTrackerSample(sender, sample, foundObjects, lostObjects, timestamp);
			break;

		default:
			// not supported
			break;
	}
}

void DeviceRecorder::recordSensorSample(const Measurement* sender, const Measurement::SampleRef& sample, const Measurement::ObjectIdSet& foundObjects, const Measurement::ObjectIdSet& lostObjects, const Timestamp& timestamp)
{
	ocean_assert(sender != nullptr);

	switch (sender->type().minorType())
	{
		case Sensor::SENSOR_ACCELERATION_3DOF:
			break;

		case Sensor::SENSOR_LINEAR_ACCELERATION_3DOF:
			break;

		case Sensor::SENSOR_GYRO_RAW_3DOF:
			break;

		case Sensor::SENSOR_GYRO_UNBIASED_3DOF:
			break;

		default:
			ocean_assert(false && "Missing implementation!");
			break;
	}
}

void DeviceRecorder::recordTrackerSample(const Measurement* sender, const Measurement::SampleRef& sample, const Measurement::ObjectIdSet& foundObjects, const Measurement::ObjectIdSet& lostObjects, const Timestamp& timestamp)
{
	ocean_assert(sender != nullptr);

	switch (sender->type().minorType())
	{
		case Tracker::TRACKER_ORIENTATION_3DOF:
			break;

		case Tracker::TRACKER_POSITION_3DOF:
			break;

		case Tracker::TRACKER_6DOF:
		case Tracker::TRACKER_6DOF | Tracker::TRACKER_VISUAL:
		case Tracker::TRACKER_6DOF | Tracker::TRACKER_VISUAL | Tracker::TRACKER_OBJECT:
		{
			const Tracker6DOF::Tracker6DOFSampleRef tracker6DOFSample(sample);
			ocean_assert(sample.isNull() || tracker6DOFSample);

			recordTracker6DOFSample(sender, tracker6DOFSample, foundObjects, lostObjects, timestamp);
			break;
		}

		case Tracker::SCENE_TRACKER_6DOF:
		case Tracker::SCENE_TRACKER_6DOF | Tracker::TRACKER_VISUAL:
		case Tracker::SCENE_TRACKER_6DOF | Tracker::TRACKER_VISUAL | Tracker::TRACKER_OBJECT:
		{
			const SceneTracker6DOF::SceneTracker6DOFSampleRef sceneTracker6DOFSample(sample);
			ocean_assert(sample.isNull() || sceneTracker6DOFSample);

			recordSceneTracker6DOFSample(sender, sceneTracker6DOFSample, foundObjects, lostObjects, timestamp);
			break;
		}

		case Tracker::TRACKER_GPS:
		{
			const GPSTracker::GPSTrackerSampleRef gpsTrackerSample(sample);
			ocean_assert(sample.isNull() || gpsTrackerSample);

			recordGPSTrackerSample(sender, gpsTrackerSample, foundObjects, lostObjects, timestamp);
			break;
		}

		case Tracker::TRACKER_MAGNETIC:
			break;

		default:
			ocean_assert(false && "Missing implementation!");
			break;
	}
}

void DeviceRecorder::recordTracker6DOFSample(const Measurement* sender, const Tracker6DOF::Tracker6DOFSampleRef& sample, const Measurement::ObjectIdSet& foundObjects, const Measurement::ObjectIdSet& lostObjects, const Timestamp& timestamp)
{
	ocean_assert(sender != nullptr);

	MeasurementRecordableMap::iterator i = measurementRecordableMap_.find(sender);

	if (i == measurementRecordableMap_.cend())
	{
		const unsigned int recordableIndex = flexibleRecorder_->addRecordable<RecordableTracker6DOFSample>();

		i = measurementRecordableMap_.emplace(sender, recordableIndex).first;
	}

	RecordableTracker6DOFSample& recordableTracker6DOFSample = flexibleRecorder_->recordable<RecordableTracker6DOFSample>(i->second);

	if (!recordableTracker6DOFSample.isValid())
	{
		const std::string deviceTypeMajor = Tracker::translateMajorType(sender->type().majorType());
		const std::string deviceTypeMinor = Tracker::translateTrackerType(Tracker6DOF::TrackerType(sender->type().minorType()));

		recordableTracker6DOFSample.setDevice(sender->name(), deviceTypeMajor, deviceTypeMinor, timestamp);
	}

	// we use an explicit record timestamp as we may have queued several samples which may be older already
	const double recordTimestamp = double(timestamp);

	recordableTracker6DOFSample.addData(sender, foundObjects, lostObjects, sample, timestamp, &recordTimestamp);
}

void DeviceRecorder::recordSceneTracker6DOFSample(const Measurement* sender, const SceneTracker6DOF::SceneTracker6DOFSampleRef& sample, const Measurement::ObjectIdSet& foundObjects, const Measurement::ObjectIdSet& lostObjects, const Timestamp& timestamp)
{
	ocean_assert(sender != nullptr);

	MeasurementRecordableMap::iterator i = measurementRecordableMap_.find(sender);

	if (i == measurementRecordableMap_.cend())
	{
		const unsigned int recordableIndex = flexibleRecorder_->addRecordable<RecordableSceneTracker6DOFSample>();

		i = measurementRecordableMap_.emplace(sender, recordableIndex).first;
	}

	RecordableSceneTracker6DOFSample& recordableSceneTracker6DOFSample = flexibleRecorder_->recordable<RecordableSceneTracker6DOFSample>(i->second);

	if (!recordableSceneTracker6DOFSample.isValid())
	{
		const std::string deviceTypeMajor = Tracker::translateMajorType(sender->type().majorType());
		const std::string deviceTypeMinor = Tracker::translateTrackerType(Tracker6DOF::TrackerType(sender->type().minorType()));

		recordableSceneTracker6DOFSample.setDevice(sender->name(), deviceTypeMajor, deviceTypeMinor, timestamp);
	}

	// we use an explicit record timestamp as we may have queued several samples which may be older already
	const double recordTimestamp = double(timestamp);

	recordableSceneTracker6DOFSample.addData(sender, foundObjects, lostObjects, sample, timestamp, &recordTimestamp);
}

void DeviceRecorder::recordGPSTrackerSample(const Measurement* sender, const GPSTracker::GPSTrackerSampleRef& sample, const Measurement::ObjectIdSet& foundObjects, const Measurement::ObjectIdSet& lostObjects, const Timestamp& timestamp)
{
	ocean_assert(sender != nullptr);

	MeasurementRecordableMap::iterator i = measurementRecordableMap_.find(sender);

	if (i == measurementRecordableMap_.cend())
	{
		const unsigned int recordableIndex = flexibleRecorder_->addRecordable<RecordableGPSTrackerSample>();

		i = measurementRecordableMap_.emplace(sender, recordableIndex).first;
	}

	RecordableGPSTrackerSample& recordableGPSTrackerSample = flexibleRecorder_->recordable<RecordableGPSTrackerSample>(i->second);

	if (!recordableGPSTrackerSample.isValid())
	{
		const std::string deviceTypeMajor = Tracker::translateMajorType(sender->type().majorType());
		const std::string deviceTypeMinor = Tracker::translateTrackerType(Tracker6DOF::TrackerType(sender->type().minorType()));

		recordableGPSTrackerSample.setDevice(sender->name(), deviceTypeMajor, deviceTypeMinor, timestamp);
	}

	// we use an explicit record timestamp as we may have queued several samples which may be older already
	const double recordTimestamp = double(timestamp);

	recordableGPSTrackerSample.addData(sender, foundObjects, lostObjects, sample, timestamp, &recordTimestamp);
}

bool DeviceRecorder::recordFrame(FrameMediumData& frameMediumData, const Frame& frame, const SharedAnyCameraD& camera)
{
	ocean_assert(frame.isValid() && camera && camera->isValid());
	ocean_assert(frameMediumData.lastTimestamp_ != frame.timestamp());

	if (frameMediumData.recordableIndex_ == (unsigned int)(-1))
	{
		frameMediumData.recordableIndex_ = flexibleRecorder_->addRecordable<RecordableCamera>();
	}

	const uint32_t cameraId = uint32_t(frameMediumData.recordableIndex_);

	RecordableCamera& recordableCamera = flexibleRecorder_->recordable<RecordableCamera>(frameMediumData.recordableIndex_);

	if (!recordableCamera.addData(cameraId, camera, frame, frameMediumData.frameMedium_->device_T_camera(), frame.timestamp(), frame.timestamp()))
	{
		Log::error() << "Failed to write frame to VRS stream";
		return false;
	}

	frameMediumData.lastTimestamp_ = frame.timestamp();

	return true;
}

void DeviceRecorder::recordPendingSamples()
{
	// put all unqueued samples into the queue

	TemporaryScopedLock unqueuedScopedLock(unqueuedSampleDatasLock_);
		SampleDatas unqueuedSampleDatas = std::move(unqueuedSampleDatas_);
	unqueuedScopedLock.release();

	if (!unqueuedSampleDatas.empty())
	{
		const ScopedLock queueScopedLock(sampleDataQueueLock_);
		sampleDataQueue_.emplace(std::move(unqueuedSampleDatas));
	}

	// wait until all remaining samples have been processed

	const Timestamp timestamp(true);

	while (true)
	{
		const ScopedLock queueScopedLock(sampleDataQueueLock_);

		if (sampleDataQueue_.empty())
		{
			break;
		}

		sleep(1u);

		if (timestamp + 5.0 < Timestamp(true))
		{
			ocean_assert(false && "Unwritten samples!");
			break;
		}
	}
}

void DeviceRecorder::threadRun()
{
	SampleDatas sampleDatas;

	while (!shouldThreadStop())
	{
		// first writing a chunk for samples

		sampleDatas.clear();

		TemporaryScopedLock sampleScopedLock(sampleDataQueueLock_);

			if (!sampleDataQueue_.empty())
			{
				sampleDatas = std::move(sampleDataQueue_.front());
				sampleDataQueue_.pop();
			}

		sampleScopedLock.release();

		for (const SampleData& sampleData : sampleDatas)
		{
			if (invalidMeasurements_.find(sampleData.measurement_) == invalidMeasurements_.cend())
			{
				recordSample(sampleData.measurement_, sampleData.sample_, sampleData.foundObjects_, sampleData.lostObjects_, sampleData.timestamp_);
			}
		}

		// now writing all media streams

		const ScopedLock mediumScopedLock(frameMediumMapLock_);

		SharedAnyCamera camera;
		bool frameRecorded = false;

		for (FrameMediumMap::iterator iFrameMedium = frameMediumMap_.begin(); iFrameMedium != frameMediumMap_.end(); ++iFrameMedium)
		{
			FrameMediumData& frameMediumData = iFrameMedium->second;

			ocean_assert(frameMediumData.frameMedium_);
			FrameRef frame = frameMediumData.frameMedium_->frame(&camera);

			if (frame && *frame && frame->timestamp() != frameMediumData.lastTimestamp_)
			{
				// the frame is a new frame

				if (flexibleRecorder_->queueByteSize() >= 512u * 1024u * 1024u)
				{
					// we need to skip the frame as the recorder's queue is too large

					Log::warning() << "Skipped frame in VRS recording, due to queue size " << flexibleRecorder_->queueByteSize() / (1024u * 1024u) << "MB";
					continue;
				}

				recordFrame(frameMediumData, *frame, AnyCameraD::convert(camera));

				frameRecorded = true;
			}
		}

		if (!sampleDatas.empty() && !frameRecorded)
		{
			sleep(1u);
		}
	}
}

}

}

}
