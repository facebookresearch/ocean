/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/serialization/DeviceSerializer.h"

namespace Ocean
{

namespace Devices
{

namespace Serialization
{

DeviceSerializer::DataSampleOrientationTracker3DOF::DataSampleOrientationTracker3DOF(const OrientationTracker3DOF::OrientationTracker3DOFSample& sample, const Timestamp sampleCreationTimestamp) :
	DataSample(IO::Serialization::DataTimestamp(double(sample.timestamp())), sampleCreationTimestamp)
{
	referenceSystem_ = int8_t(sample.referenceSystem());

	const Measurement::ObjectIds& sampleObjectIds = sample.objectIds();
	objectIds_.reserve(sampleObjectIds.size());
	for (const Measurement::ObjectId objectId : sampleObjectIds)
	{
		objectIds_.emplace_back(objectId);
	}

	const OrientationTracker3DOF::OrientationTracker3DOFSample::Orientations& sampleOrientations = sample.orientations();
	orientations_.reserve(sampleOrientations.size());
	for (const Quaternion& orientation : sampleOrientations)
	{
		orientations_.emplace_back(orientation);
	}
}

bool DeviceSerializer::SampleMeasurement::readMeasurement(IO::InputBitstream& inputBitstream)
{
	uint32_t numberObjectIds = 0u;
	if (!inputBitstream.read<uint32_t>(numberObjectIds) || numberObjectIds > maximalMeasurements_)
	{
		return false;
	}

	static_assert(std::is_same<Index32, uint32_t>::value, "Invalid data type!");

	objectIds_.resize(numberObjectIds);
	if (!inputBitstream.read(objectIds_.data(), objectIds_.size() * sizeof(Index32)))
	{
		return false;
	}

	return true;
}

bool DeviceSerializer::SampleMeasurement::writeMeasurement(IO::OutputBitstream& outputBitstream) const
{
	ocean_assert(objectIds_.size() <= maximalMeasurements_);
	if (!outputBitstream.write<uint32_t>(uint32_t(objectIds_.size())))
	{
		return false;
	}

	static_assert(std::is_same<Index32, uint32_t>::value, "Invalid data type!");

	if (!outputBitstream.write(objectIds_.data(), objectIds_.size() * sizeof(Index32)))
	{
		return false;
	}

	return true;
}

bool DeviceSerializer::SampleTracker::readTracker(IO::InputBitstream& inputBitstream)
{
	if (!readMeasurement(inputBitstream))
	{
		return false;
	}

	if (!inputBitstream.read<int8_t>(referenceSystem_))
	{
		return false;
	}

	return true;
}

bool DeviceSerializer::SampleTracker::writeTracker(IO::OutputBitstream& outputBitstream) const
{
	if (!writeMeasurement(outputBitstream))
	{
		return false;
	}

	if (!outputBitstream.write<int8_t>(referenceSystem_))
	{
		return false;
	}

	return true;
}

bool DeviceSerializer::DataSampleOrientationTracker3DOF::readSample(IO::InputBitstream& inputBitstream)
{
	if (!DataSample::readSample(inputBitstream))
	{
		return false;
	}

	if (!readTracker(inputBitstream))
	{
		return false;
	}

	uint32_t numberOrientations = 0u;
	if (!inputBitstream.read<uint32_t>(numberOrientations) || numberOrientations > maximalMeasurements_)
	{
		return false;
	}

	orientations_.resize(numberOrientations);

	for (QuaternionF& orientation : orientations_)
	{
		if (!readQuaternionF(inputBitstream, orientation))
		{
			return false;
		}
	}

	if (objectIds_.size() != orientations_.size())
	{
		return false;
	}

	return true;
}

bool DeviceSerializer::DataSampleOrientationTracker3DOF::writeSample(IO::OutputBitstream& outputBitstream) const
{
	if (!DataSample::writeSample(outputBitstream))
	{
		return false;
	}

	if (!writeTracker(outputBitstream))
	{
		return false;
	}

	ocean_assert(orientations_.size() <= maximalMeasurements_);
	if (!outputBitstream.write<uint32_t>(uint32_t(orientations_.size())))
	{
		return false;
	}

	for (const QuaternionF& orientation : orientations_)
	{
		if (!writeQuaternionF(outputBitstream, orientation))
		{
			return false;
		}
	}

	return true;
}

DeviceSerializer::DataSampleAccelerationSensor3DOF::DataSampleAccelerationSensor3DOF(const AccelerationSensor3DOF::Acceleration3DOFSample& sample, const Timestamp sampleCreationTimestamp) :
	DataSample(IO::Serialization::DataTimestamp(double(sample.timestamp())), sampleCreationTimestamp)
{
	const Measurement::ObjectIds& sampleObjectIds = sample.objectIds();
	objectIds_.reserve(sampleObjectIds.size());
	for (const Measurement::ObjectId objectId : sampleObjectIds)
	{
		objectIds_.emplace_back(objectId);
	}

	const AccelerationSensor3DOF::Acceleration3DOFSample::Measurements& sampleMeasurements = sample.measurements();
	measurements_.reserve(sampleMeasurements.size());
	for (const Vector3& measurement : sampleMeasurements)
	{
		measurements_.emplace_back(measurement);
	}
}

bool DeviceSerializer::DataSampleAccelerationSensor3DOF::readSample(IO::InputBitstream& inputBitstream)
{
	if (!DataSample::readSample(inputBitstream))
	{
		return false;
	}

	if (!readMeasurement(inputBitstream))
	{
		return false;
	}

	uint32_t numberMeasurements = 0u;
	if (!inputBitstream.read<uint32_t>(numberMeasurements) || numberMeasurements > maximalMeasurements_)
	{
		return false;
	}

	measurements_.resize(numberMeasurements);

	for (VectorF3& measurement : measurements_)
	{
		if (!readVectorF3(inputBitstream, measurement))
		{
			return false;
		}
	}

	if (objectIds_.size() != measurements_.size())
	{
		return false;
	}

	return true;
}

bool DeviceSerializer::DataSampleAccelerationSensor3DOF::writeSample(IO::OutputBitstream& outputBitstream) const
{
	if (!DataSample::writeSample(outputBitstream))
	{
		return false;
	}

	if (!writeMeasurement(outputBitstream))
	{
		return false;
	}

	ocean_assert(measurements_.size() <= maximalMeasurements_);
	if (!outputBitstream.write<uint32_t>(uint32_t(measurements_.size())))
	{
		return false;
	}

	for (const VectorF3& measurement : measurements_)
	{
		if (!writeVectorF3(outputBitstream, measurement))
		{
			return false;
		}
	}

	return true;
}

DeviceSerializer::DataSampleGyroSensor3DOF::DataSampleGyroSensor3DOF(const GyroSensor3DOF::Gyro3DOFSample& sample, const Timestamp sampleCreationTimestamp) :
	DataSample(IO::Serialization::DataTimestamp(double(sample.timestamp())), sampleCreationTimestamp)
{
	const Measurement::ObjectIds& sampleObjectIds = sample.objectIds();
	objectIds_.reserve(sampleObjectIds.size());
	for (const Measurement::ObjectId objectId : sampleObjectIds)
	{
		objectIds_.emplace_back(objectId);
	}

	const GyroSensor3DOF::Gyro3DOFSample::Measurements& sampleMeasurements = sample.measurements();
	measurements_.reserve(sampleMeasurements.size());
	for (const Vector3& measurement : sampleMeasurements)
	{
		measurements_.emplace_back(measurement);
	}
}

bool DeviceSerializer::DataSampleGyroSensor3DOF::readSample(IO::InputBitstream& inputBitstream)
{
	if (!DataSample::readSample(inputBitstream))
	{
		return false;
	}

	if (!readMeasurement(inputBitstream))
	{
		return false;
	}

	uint32_t numberMeasurements = 0u;
	if (!inputBitstream.read<uint32_t>(numberMeasurements) || numberMeasurements > maximalMeasurements_)
	{
		return false;
	}

	measurements_.resize(numberMeasurements);

	for (VectorF3& measurement : measurements_)
	{
		if (!readVectorF3(inputBitstream, measurement))
		{
			return false;
		}
	}

	if (objectIds_.size() != measurements_.size())
	{
		return false;
	}

	return true;
}

bool DeviceSerializer::DataSampleGyroSensor3DOF::writeSample(IO::OutputBitstream& outputBitstream) const
{
	if (!DataSample::writeSample(outputBitstream))
	{
		return false;
	}

	if (!writeMeasurement(outputBitstream))
	{
		return false;
	}

	ocean_assert(measurements_.size() <= maximalMeasurements_);
	if (!outputBitstream.write<uint32_t>(uint32_t(measurements_.size())))
	{
		return false;
	}

	for (const VectorF3& measurement : measurements_)
	{
		if (!writeVectorF3(outputBitstream, measurement))
		{
			return false;
		}
	}

	return true;
}

DeviceSerializer::DataSampleGravityTracker3DOF::DataSampleGravityTracker3DOF(const GravityTracker3DOF::GravityTracker3DOFSample& sample, const Timestamp sampleCreationTimestamp) :
	DataSample(IO::Serialization::DataTimestamp(double(sample.timestamp())), sampleCreationTimestamp)
{
	referenceSystem_ = int8_t(sample.referenceSystem());

	const Measurement::ObjectIds& sampleObjectIds = sample.objectIds();
	objectIds_.reserve(sampleObjectIds.size());
	for (const Measurement::ObjectId objectId : sampleObjectIds)
	{
		objectIds_.emplace_back(objectId);
	}

	const GravityTracker3DOF::GravityTracker3DOFSample::Gravities& sampleGravities = sample.gravities();
	gravities_.reserve(sampleGravities.size());
	for (const Vector3& gravity : sampleGravities)
	{
		gravities_.emplace_back(gravity);
	}
}

bool DeviceSerializer::DataSampleGravityTracker3DOF::readSample(IO::InputBitstream& inputBitstream)
{
	if (!DataSample::readSample(inputBitstream))
	{
		return false;
	}

	if (!readTracker(inputBitstream))
	{
		return false;
	}

	uint32_t numberGravities = 0u;
	if (!inputBitstream.read<uint32_t>(numberGravities) || numberGravities > maximalMeasurements_)
	{
		return false;
	}

	gravities_.resize(numberGravities);

	for (VectorF3& gravity : gravities_)
	{
		if (!readVectorF3(inputBitstream, gravity))
		{
			return false;
		}
	}

	if (objectIds_.size() != gravities_.size())
	{
		return false;
	}

	return true;
}

bool DeviceSerializer::DataSampleGravityTracker3DOF::writeSample(IO::OutputBitstream& outputBitstream) const
{
	if (!DataSample::writeSample(outputBitstream))
	{
		return false;
	}

	if (!writeTracker(outputBitstream))
	{
		return false;
	}

	ocean_assert(gravities_.size() <= maximalMeasurements_);
	if (!outputBitstream.write<uint32_t>(uint32_t(gravities_.size())))
	{
		return false;
	}

	for (const VectorF3& gravity : gravities_)
	{
		if (!writeVectorF3(outputBitstream, gravity))
		{
			return false;
		}
	}

	return true;
}

DeviceSerializer::DataSamplePositionTracker3DOF::DataSamplePositionTracker3DOF(const PositionTracker3DOF::PositionTracker3DOFSample& sample, const Timestamp sampleCreationTimestamp) :
	DataSample(IO::Serialization::DataTimestamp(double(sample.timestamp())), sampleCreationTimestamp)
{
	referenceSystem_ = int8_t(sample.referenceSystem());

	const Measurement::ObjectIds& sampleObjectIds = sample.objectIds();
	objectIds_.reserve(sampleObjectIds.size());
	for (const Measurement::ObjectId objectId : sampleObjectIds)
	{
		objectIds_.emplace_back(objectId);
	}

	const PositionTracker3DOF::PositionTracker3DOFSample::Positions& samplePositions = sample.positions();
	positions_.reserve(samplePositions.size());
	for (const Vector3& position : samplePositions)
	{
		positions_.emplace_back(position);
	}
}

bool DeviceSerializer::DataSamplePositionTracker3DOF::readSample(IO::InputBitstream& inputBitstream)
{
	if (!DataSample::readSample(inputBitstream))
	{
		return false;
	}

	if (!readTracker(inputBitstream))
	{
		return false;
	}

	uint32_t numberPositions = 0u;
	if (!inputBitstream.read<uint32_t>(numberPositions) || numberPositions > maximalMeasurements_)
	{
		return false;
	}

	positions_.resize(numberPositions);

	for (VectorF3& position : positions_)
	{
		if (!readVectorF3(inputBitstream, position))
		{
			return false;
		}
	}

	if (objectIds_.size() != positions_.size())
	{
		return false;
	}

	return true;
}

bool DeviceSerializer::DataSamplePositionTracker3DOF::writeSample(IO::OutputBitstream& outputBitstream) const
{
	if (!DataSample::writeSample(outputBitstream))
	{
		return false;
	}

	if (!writeTracker(outputBitstream))
	{
		return false;
	}

	ocean_assert(positions_.size() <= maximalMeasurements_);
	if (!outputBitstream.write<uint32_t>(uint32_t(positions_.size())))
	{
		return false;
	}

	for (const VectorF3& position : positions_)
	{
		if (!writeVectorF3(outputBitstream, position))
		{
			return false;
		}
	}

	return true;
}

DeviceSerializer::DataSampleTracker6DOF::DataSampleTracker6DOF(const Tracker6DOF::Tracker6DOFSample& sample, const Timestamp sampleCreationTimestamp) :
	DataSample(IO::Serialization::DataTimestamp(double(sample.timestamp())), sampleCreationTimestamp)
{
	referenceSystem_ = int8_t(sample.referenceSystem());

	const Measurement::ObjectIds& sampleObjectIds = sample.objectIds();
	objectIds_.reserve(sampleObjectIds.size());
	for (const Measurement::ObjectId objectId : sampleObjectIds)
	{
		objectIds_.emplace_back(objectId);
	}

	const Tracker6DOF::Tracker6DOFSample::Orientations& sampleOrientations = sample.orientations();
	orientations_.reserve(sampleOrientations.size());
	for (const Quaternion& orientation : sampleOrientations)
	{
		orientations_.emplace_back(orientation);
	}

	const Tracker6DOF::Tracker6DOFSample::Positions& samplePositions = sample.positions();
	positions_.reserve(samplePositions.size());
	for (const Vector3& position : samplePositions)
	{
		positions_.emplace_back(position);
	}
}

bool DeviceSerializer::DataSampleTracker6DOF::readSample(IO::InputBitstream& inputBitstream)
{
	if (!DataSample::readSample(inputBitstream))
	{
		return false;
	}

	if (!readTracker(inputBitstream))
	{
		return false;
	}

	uint32_t numberOrientations = 0u;
	if (!inputBitstream.read<uint32_t>(numberOrientations) || numberOrientations > maximalMeasurements_)
	{
		return false;
	}

	orientations_.resize(numberOrientations);

	for (QuaternionF& orientation : orientations_)
	{
		if (!readQuaternionF(inputBitstream, orientation))
		{
			return false;
		}
	}

	if (objectIds_.size() != orientations_.size())
	{
		return false;
	}

	uint32_t numberPositions = 0u;
	if (!inputBitstream.read<uint32_t>(numberPositions) || numberPositions > maximalMeasurements_)
	{
		return false;
	}

	positions_.resize(numberPositions);

	for (VectorF3& position : positions_)
	{
		if (!readVectorF3(inputBitstream, position))
		{
			return false;
		}
	}

	if (objectIds_.size() != positions_.size())
	{
		return false;
	}

	return true;
}

bool DeviceSerializer::DataSampleTracker6DOF::writeSample(IO::OutputBitstream& outputBitstream) const
{
	if (!DataSample::writeSample(outputBitstream))
	{
		return false;
	}

	if (!writeTracker(outputBitstream))
	{
		return false;
	}

	ocean_assert(orientations_.size() <= maximalMeasurements_);
	if (!outputBitstream.write<uint32_t>(uint32_t(orientations_.size())))
	{
		return false;
	}

	for (const QuaternionF& orientation : orientations_)
	{
		if (!writeQuaternionF(outputBitstream, orientation))
		{
			return false;
		}
	}

	ocean_assert(positions_.size() <= maximalMeasurements_);
	if (!outputBitstream.write<uint32_t>(uint32_t(positions_.size())))
	{
		return false;
	}

	for (const VectorF3& position : positions_)
	{
		if (!writeVectorF3(outputBitstream, position))
		{
			return false;
		}
	}

	return true;
}

DeviceSerializer::DataSampleGPSTracker::Location::Location(const GPSTracker::Location& location) :
	latitude_(location.latitude()),
	longitude_(location.longitude()),
	altitude_(location.altitude()),
	direction_(location.direction()),
	speed_(location.speed()),
	accuracy_(location.accuracy()),
	altitudeAccuracy_(location.altitudeAccuracy()),
	directionAccuracy_(location.directionAccuracy()),
	speedAccuracy_(location.speedAccuracy())
{
	// nothing to do here
}

bool DeviceSerializer::DataSampleGPSTracker::Location::read(IO::InputBitstream& inputBitstream)
{
	if (!inputBitstream.read<double>(latitude_))
	{
		return false;
	}

	if (!inputBitstream.read<double>(longitude_))
	{
		return false;
	}

	if (!inputBitstream.read<float>(altitude_))
	{
		return false;
	}

	if (!inputBitstream.read<float>(direction_))
	{
		return false;
	}

	if (!inputBitstream.read<float>(speed_))
	{
		return false;
	}

	if (!inputBitstream.read<float>(accuracy_))
	{
		return false;
	}

	if (!inputBitstream.read<float>(altitudeAccuracy_))
	{
		return false;
	}

	if (!inputBitstream.read<float>(directionAccuracy_))
	{
		return false;
	}

	if (!inputBitstream.read<float>(speedAccuracy_))
	{
		return false;
	}

	return true;
}

bool DeviceSerializer::DataSampleGPSTracker::Location::write(IO::OutputBitstream& outputBitstream) const
{
	if (!outputBitstream.write<double>(latitude_))
	{
		return false;
	}

	if (!outputBitstream.write<double>(longitude_))
	{
		return false;
	}

	if (!outputBitstream.write<float>(altitude_))
	{
		return false;
	}

	if (!outputBitstream.write<float>(direction_))
	{
		return false;
	}

	if (!outputBitstream.write<float>(speed_))
	{
		return false;
	}

	if (!outputBitstream.write<float>(accuracy_))
	{
		return false;
	}

	if (!outputBitstream.write<float>(altitudeAccuracy_))
	{
		return false;
	}

	if (!outputBitstream.write<float>(directionAccuracy_))
	{
		return false;
	}

	if (!outputBitstream.write<float>(speedAccuracy_))
	{
		return false;
	}

	return true;
}

DeviceSerializer::DataSampleGPSTracker::DataSampleGPSTracker(const GPSTracker::GPSTrackerSample& sample, const Timestamp sampleCreationTimestamp) :
	DataSample(IO::Serialization::DataTimestamp(double(sample.timestamp())), sampleCreationTimestamp)
{
	referenceSystem_ = int8_t(sample.referenceSystem());

	const Measurement::ObjectIds& sampleObjectIds = sample.objectIds();
	objectIds_.reserve(sampleObjectIds.size());
	for (const Measurement::ObjectId objectId : sampleObjectIds)
	{
		objectIds_.emplace_back(objectId);
	}

	const GPSTracker::Locations& sampleLocations = sample.locations();
	locations_.reserve(sampleLocations.size());
	for (const GPSTracker::Location& location : sampleLocations)
	{
		locations_.emplace_back(location);
	}
}

bool DeviceSerializer::DataSampleGPSTracker::readSample(IO::InputBitstream& inputBitstream)
{
	if (!DataSample::readSample(inputBitstream))
	{
		return false;
	}

	if (!readTracker(inputBitstream))
	{
		return false;
	}

	uint32_t numberLocations = 0u;
	if (!inputBitstream.read<uint32_t>(numberLocations) || numberLocations > maximalMeasurements_)
	{
		return false;
	}

	locations_.resize(numberLocations);

	for (Location& location : locations_)
	{
		if (!location.read(inputBitstream))
		{
			return false;
		}
	}

	if (objectIds_.size() != locations_.size())
	{
		return false;
	}

	return true;
}

bool DeviceSerializer::DataSampleGPSTracker::writeSample(IO::OutputBitstream& outputBitstream) const
{
	if (!DataSample::writeSample(outputBitstream))
	{
		return false;
	}

	if (!writeTracker(outputBitstream))
	{
		return false;
	}

	ocean_assert(locations_.size() <= maximalMeasurements_);
	if (!outputBitstream.write<uint32_t>(uint32_t(locations_.size())))
	{
		return false;
	}

	for (const Location& location : locations_)
	{
		if (!location.write(outputBitstream))
		{
			return false;
		}
	}

	return true;
}

}

}

}
