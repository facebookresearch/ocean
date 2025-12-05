/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/serialization/SerializerDevicePlayer.h"
#include "ocean/devices/serialization/DeviceSerializer.h"
#include "ocean/devices/serialization/SerializationAccelerationSensor3DOF.h"
#include "ocean/devices/serialization/SerializationGPSTracker.h"
#include "ocean/devices/serialization/SerializationGravityTracker3DOF.h"
#include "ocean/devices/serialization/SerializationGyroSensor3DOF.h"
#include "ocean/devices/serialization/SerializationOrientationTracker3DOF.h"
#include "ocean/devices/serialization/SerializationPositionTracker3DOF.h"
#include "ocean/devices/serialization/SerializationTracker6DOF.h"

#include "ocean/base/Timestamp.h"

#include "ocean/devices/AccelerationSensor3DOF.h"
#include "ocean/devices/GPSTracker.h"
#include "ocean/devices/GravityTracker3DOF.h"
#include "ocean/devices/GyroSensor3DOF.h"
#include "ocean/devices/Manager.h"
#include "ocean/devices/OrientationTracker3DOF.h"
#include "ocean/devices/PositionTracker3DOF.h"
#include "ocean/devices/Tracker6DOF.h"

#include "ocean/media/Manager.h"
#include "ocean/media/PixelImage.h"

namespace Ocean
{

namespace Devices
{

namespace Serialization
{

bool SerializerDevicePlayer::UsageManager::registerUsage()
{
	const ScopedLock scopedLock(lock_);

	if (isUsed_)
	{
		return false;
	}

	isUsed_ = true;

	return true;
}

void SerializerDevicePlayer::UsageManager::unregisterUsage()
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isUsed_);
	isUsed_ = false;
}

SerializerDevicePlayer::~SerializerDevicePlayer()
{
	release();
}

bool SerializerDevicePlayer::initialize(const std::string& filename)
{
	const ScopedLock scopedLock(lock_);

	if (inputSerializer_)
	{
		return false;
	}

	if (!UsageManager::get().registerUsage())
	{
		Log::error() << "Already one SerializerDevicePlayer in use";
		return false;
	}

	inputSerializer_ = std::make_unique<IO::Serialization::FileInputDataSerializer>();

	if (!inputSerializer_->setFilename(filename))
	{
		inputSerializer_ = nullptr;
		UsageManager::get().unregisterUsage();

		return false;
	}

	filename_ = filename;

	if (!initializeDeviceFactories())
	{
		inputSerializer_ = nullptr;
		filename_.clear();
		UsageManager::get().unregisterUsage();

		return false;
	}

	IO::Serialization::DataSerializer::Channels channels;
	if (!inputSerializer_->initialize(&channels))
	{
		return false;
	}

	for (const IO::Serialization::DataSerializer::Channel& channel : channels)
	{
		if (channel.sampleType() == IO::Serialization::MediaSerializer::DataSampleFrame::sampleType())
		{
			const IO::Serialization::DataSerializer::ChannelId channelId = channel.channelId();
			ocean_assert(!channelFrameMediumDataMap_.contains(channelId));

			if (firstMediaFrameChannelId_ == IO::Serialization::DataSerializer::invalidChannelId())
			{
				firstMediaFrameChannelId_ = channelId;
			}

			Media::PixelImageRef pixelImage = Media::Manager::get().newMedium("Serializer Pixel Medium " + String::toAString(channelId), Media::Medium::PIXEL_IMAGE);

			if (pixelImage)
			{
				pixelImage->setCapacity(30);
				pixelImage->start();

				channelFrameMediumDataMap_[channelId] = FrameMediumData(pixelImage);
				frameMediums_.emplace_back(pixelImage);
			}
		}
	}

	return true;
}

bool SerializerDevicePlayer::start(const float speed)
{
	const ScopedLock scopedLock(lock_);

	if (isStarted_)
	{
		return true;
	}

	if (!inputSerializer_)
	{
		return false;
	}

	if (!inputSerializer_->start())
	{
		return false;
	}

	isStarted_ = true;
	speed_ = speed;

	if (speed > 0.0f)
	{
		startThread();
	}

	return true;
}

bool SerializerDevicePlayer::stop()
{
	const ScopedLock scopedLock(lock_);

	if (!isStarted_)
	{
		return true;
	}

	stopThreadExplicitly();

	if (inputSerializer_)
	{
		inputSerializer_->stop();
	}

	isStarted_ = false;

	return true;
}

Timestamp SerializerDevicePlayer::playNextFrame()
{
	const ScopedLock scopedLock(lock_);

	if (!isStarted_ || speed_ > 0.0f || !inputSerializer_)
	{
		ocean_assert(false && "The player is not configured for stop-motion mode!");
		return Timestamp(false);
	}

	if (firstMediaFrameChannelId_ == IO::Serialization::DataSerializer::invalidChannelId())
	{
		ocean_assert(false && "The player does not contain any media channel!");
		return Timestamp(false);
	}

	while (true)
	{
		SamplePair samplePair;

		if (!stopMotionSampleQueue_.empty())
		{
			samplePair = std::move(stopMotionSampleQueue_.front());
			stopMotionSampleQueue_.pop_front();

			if (samplePair.second == nullptr)
			{
				// the sample has been processed in a previous iteration
				continue;
			}
		}
		else
		{
			samplePair.second = inputSerializer_->sample(samplePair.first, 0.0 /*speed*/);
		}

		if (!samplePair.second)
		{
			if (inputSerializer_->hasFinished())
			{
				// we have reached the end of the serializer data
				break;
			}
			else
			{
				Thread::sleep(1u);
				continue;
			}
		}

		ocean_assert(samplePair.first != IO::Serialization::DataSerializer::invalidChannelId());

		if (samplePair.first == firstMediaFrameChannelId_)
		{
			// we have a sample from the first media channel, so we can stop here
			// however, let's try to go through some additional samples to ensure that we have processed all samples with older or same timestamp

			const IO::Serialization::DataTimestamp dataTimestamp = samplePair.second->dataTimestamp();
			const double maxPlaybackTimestamp = samplePair.second->playbackTimestamp() + 0.5;

			processLookaheadSamples(dataTimestamp, maxPlaybackTimestamp);

			processSample(samplePair.first, std::move(samplePair.second));

			return Timestamp(dataTimestamp.forceDouble());
		}

		processSample(samplePair.first, std::move(samplePair.second));
	}

	isStarted_ = false;

	return Timestamp(false);
}

DevicePlayer::TransformationResult SerializerDevicePlayer::transformation(const std::string& /*name*/, const Timestamp& /*timestamp*/, HomogenousMatrixD4& /*matrix*/)
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid());

	if (!isValid())
	{
		return TR_DOES_NOT_EXIST;
	}

	// For now, transformation extraction is not implemented
	// This could be extended to extract transformations from tracker devices
	return TR_DOES_NOT_EXIST;
}

bool SerializerDevicePlayer::isPlaying() const
{
	return isStarted_;
}

void SerializerDevicePlayer::release()
{
	stop();

	while (isPlaying())
	{
		Thread::sleep(1u);
	}

	const ScopedLock scopedLock(lock_);

	if (inputSerializer_)
	{
		std::vector<std::string> deviceNames;
		deviceNames.reserve(channelDeviceMap_.size());

		for (const ChannelDeviceMap::value_type& iDevice : channelDeviceMap_)
		{
			deviceNames.emplace_back(iDevice.second->name());
		}

		channelDeviceMap_.clear();

		for (const std::string& deviceName : deviceNames)
		{
			Manager::get().unregisterAdhocDevice(deviceName);
		}

		for (ChannelFrameMediumDataMap::value_type& iMedium : channelFrameMediumDataMap_)
		{
			FrameMediumData& frameMediumData = iMedium.second;

			if (frameMediumData.pixelImage_)
			{
				frameMediumData.pixelImage_->stop();
			}
		}

		channelFrameMediumDataMap_.clear();

		frameMediums_.clear();
		channelProcessorMap_.clear();

		inputSerializer_ = nullptr;

		UsageManager::get().unregisterUsage();
	}
}

bool SerializerDevicePlayer::isValid() const
{
	const ScopedLock scopedLock(lock_);

	if (!DevicePlayer::isValid())
	{
		return false;
	}

	return inputSerializer_ != nullptr;
}

bool SerializerDevicePlayer::initializeDeviceFactories()
{
	ocean_assert(inputSerializer_);

	if (!inputSerializer_->registerSample<IO::Serialization::MediaSerializer::DataSampleFrame>())
	{
		return false;
	}

	if (!inputSerializer_->registerSample<DeviceSerializer::DataSampleOrientationTracker3DOF>())
	{
		return false;
	}

	if (!inputSerializer_->registerSample<DeviceSerializer::DataSampleAccelerationSensor3DOF>())
	{
		return false;
	}

	if (!inputSerializer_->registerSample<DeviceSerializer::DataSampleGyroSensor3DOF>())
	{
		return false;
	}

	if (!inputSerializer_->registerSample<DeviceSerializer::DataSampleGravityTracker3DOF>())
	{
		return false;
	}

	if (!inputSerializer_->registerSample<DeviceSerializer::DataSamplePositionTracker3DOF>())
	{
		return false;
	}

	if (!inputSerializer_->registerSample<DeviceSerializer::DataSampleTracker6DOF>())
	{
		return false;
	}

	if (!inputSerializer_->registerSample<DeviceSerializer::DataSampleGPSTracker>())
	{
		return false;
	}

	sampleTypeMap_ =
	{
		{IO::Serialization::MediaSerializer::DataSampleFrame::sampleType(), &SerializerDevicePlayer::processDataSampleFrame},
		{DeviceSerializer::DataSampleOrientationTracker3DOF::sampleType(), &SerializerDevicePlayer::processDataSampleOrientationTracker3DOF},
		{DeviceSerializer::DataSampleAccelerationSensor3DOF::sampleType(), &SerializerDevicePlayer::processDataSampleAccelerationSensor3DOF},
		{DeviceSerializer::DataSampleGyroSensor3DOF::sampleType(), &SerializerDevicePlayer::processDataSampleGyroSensor3DOF},
		{DeviceSerializer::DataSampleGravityTracker3DOF::sampleType(), &SerializerDevicePlayer::processDataSampleGravityTracker3DOF},
		{DeviceSerializer::DataSamplePositionTracker3DOF::sampleType(), &SerializerDevicePlayer::processDataSamplePositionTracker3DOF},
		{DeviceSerializer::DataSampleTracker6DOF::sampleType(), &SerializerDevicePlayer::processDataSampleTracker6DOF},
		{DeviceSerializer::DataSampleGPSTracker::sampleType(), &SerializerDevicePlayer::processDataSampleGPSTracker}
	};

	return true;
}

bool SerializerDevicePlayer::FrameMediumData::update(const IO::Serialization::MediaSerializer::DataSampleFrame& frameSample)
{
	ocean_assert(frameSample.isValid());

	SharedAnyCamera camera = nullptr;
	Frame frame = frameSample.frame(&camera);

	if (!frame.isValid())
	{
		return false;
	}

	if (camera)
	{
		camera_ = camera;
	}

	camera = camera_;

	const HomogenousMatrixD4& device_T_camera = frameSample.device_T_camera();

	if (device_T_camera.isValid())
	{
		device_T_camera_ = device_T_camera;
	}

	ocean_assert(pixelImage_);

	if (device_T_camera_.isValid())
	{
		pixelImage_->setDevice_T_camera(device_T_camera_);
	}

	pixelImage_->setPixelImage(std::move(frame), std::move(camera));

	return true;
}

void SerializerDevicePlayer::processDataSampleFrame(const IO::Serialization::DataSerializer::ChannelId channelId, IO::Serialization::UniqueDataSample&& sample)
{
	ocean_assert(sample);

	IO::Serialization::MediaSerializer::DataSampleFrame* frameSample = dynamic_cast<IO::Serialization::MediaSerializer::DataSampleFrame*>(sample.get());
	ocean_assert(frameSample);

	if (frameSample && frameSample->isValid())
	{
		Media::PixelImageRef pixelImage;

		ChannelFrameMediumDataMap::iterator iFrameMedium = channelFrameMediumDataMap_.find(channelId);

		ocean_assert(iFrameMedium != channelFrameMediumDataMap_.cend());
		if (iFrameMedium != channelFrameMediumDataMap_.cend())
		{
			iFrameMedium->second.update(*frameSample);
		}
	}
}

void SerializerDevicePlayer::processDataSampleOrientationTracker3DOF(const IO::Serialization::DataSerializer::ChannelId channelId, IO::Serialization::UniqueDataSample&& sample)
{
	ocean_assert(sample);

	DeviceSerializer::DataSampleOrientationTracker3DOF* orientationSample = dynamic_cast<DeviceSerializer::DataSampleOrientationTracker3DOF*>(sample.get());
	ocean_assert(orientationSample);

	if (orientationSample)
	{
		const std::string deviceName = "Serialization OrientationTracker3DOF";
		const Device::DeviceType deviceType = SerializationOrientationTracker3DOF::deviceTypeSerializationOrientationTracker3DOF();

		DeviceRef deviceRef = ensureDevice(channelId, deviceName, deviceType);

		if (deviceRef)
		{
			SerializationOrientationTracker3DOF* device = dynamic_cast<SerializationOrientationTracker3DOF*>(deviceRef.pointer());
			ocean_assert(device);

			if (device)
			{
				const Timestamp timestamp(sample->dataTimestamp().forceDouble());

				const Tracker::ReferenceSystem referenceSystem = Tracker::ReferenceSystem(orientationSample->referenceSystem());

				const QuaternionsF& orientationsF = orientationSample->orientations();

				OrientationTracker3DOF::OrientationTracker3DOFSample::Orientations orientations;
				orientations.reserve(orientationsF.size());

				if constexpr (std::is_same<Scalar, float>::value)
				{
					for (const QuaternionF& orientationF : orientationsF)
					{
						orientations.emplaceBack(orientationF);
					}
				}
				else
				{
					for (const QuaternionF& orientationF : orientationsF)
					{
						orientations.emplaceBack(Quaternion(orientationF).normalized());
					}
				}

				Measurement::Metadata metadata;

				device->forwardSampleEvent(std::move(orientations), referenceSystem, timestamp, std::move(metadata));
			}
		}
	}
}

void SerializerDevicePlayer::processDataSampleAccelerationSensor3DOF(const IO::Serialization::DataSerializer::ChannelId channelId, IO::Serialization::UniqueDataSample&& sample)
{
	ocean_assert(sample);

	DeviceSerializer::DataSampleAccelerationSensor3DOF* accelerationSample = dynamic_cast<DeviceSerializer::DataSampleAccelerationSensor3DOF*>(sample.get());
	ocean_assert(accelerationSample);

	if (accelerationSample)
	{
		const std::string deviceName = "Serialization AccelerationSensor3DOF";
		const Device::DeviceType deviceType = SerializationAccelerationSensor3DOF::deviceTypeSerializationAccelerationSensor3DOF(AccelerationSensor3DOF::SENSOR_ACCELERATION_3DOF);

		DeviceRef deviceRef = ensureDevice(channelId, deviceName, deviceType);

		if (deviceRef)
		{
			SerializationAccelerationSensor3DOF* device = dynamic_cast<SerializationAccelerationSensor3DOF*>(deviceRef.pointer());
			ocean_assert(device);

			if (device)
			{
				const Timestamp timestamp(sample->dataTimestamp().forceDouble());

				const VectorsF3& measurementsF = accelerationSample->measurements();

				AccelerationSensor3DOF::Acceleration3DOFSample::Measurements measurements;
				measurements.reserve(measurementsF.size());

				for (const VectorF3& measurementF : measurementsF)
				{
					measurements.emplaceBack(Vector3(measurementF));
				}

				Measurement::Metadata metadata;

				device->forwardSampleEvent(std::move(measurements), timestamp, std::move(metadata));
			}
		}
	}
}

void SerializerDevicePlayer::processDataSampleGyroSensor3DOF(const IO::Serialization::DataSerializer::ChannelId channelId, IO::Serialization::UniqueDataSample&& sample)
{
	ocean_assert(sample);

	DeviceSerializer::DataSampleGyroSensor3DOF* gyroSample = dynamic_cast<DeviceSerializer::DataSampleGyroSensor3DOF*>(sample.get());
	ocean_assert(gyroSample);

	if (gyroSample)
	{
		const std::string deviceName = "Serialization GyroSensor3DOF";
		const Device::DeviceType deviceType = SerializationGyroSensor3DOF::deviceTypeSerializationGyroSensor3DOF(GyroSensor3DOF::SENSOR_GYRO_RAW_3DOF);

		DeviceRef deviceRef = ensureDevice(channelId, deviceName, deviceType);

		if (deviceRef)
		{
			SerializationGyroSensor3DOF* device = dynamic_cast<SerializationGyroSensor3DOF*>(deviceRef.pointer());
			ocean_assert(device);

			if (device)
			{
				const Timestamp timestamp(sample->dataTimestamp().forceDouble());

				const VectorsF3& measurementsF = gyroSample->measurements();

				GyroSensor3DOF::Gyro3DOFSample::Measurements measurements;
				measurements.reserve(measurementsF.size());

				for (const VectorF3& measurementF : measurementsF)
				{
					measurements.emplaceBack(Vector3(measurementF));
				}

				Measurement::Metadata metadata;

				device->forwardSampleEvent(std::move(measurements), timestamp, std::move(metadata));
			}
		}
	}
}

void SerializerDevicePlayer::processDataSampleGravityTracker3DOF(const IO::Serialization::DataSerializer::ChannelId channelId, IO::Serialization::UniqueDataSample&& sample)
{
	ocean_assert(sample);

	DeviceSerializer::DataSampleGravityTracker3DOF* gravitySample = dynamic_cast<DeviceSerializer::DataSampleGravityTracker3DOF*>(sample.get());
	ocean_assert(gravitySample);

	if (gravitySample)
	{
		const std::string deviceName = "Serialization GravityTracker3DOF";
		const Device::DeviceType deviceType = SerializationGravityTracker3DOF::deviceTypeSerializationGravityTracker3DOF();

		DeviceRef deviceRef = ensureDevice(channelId, deviceName, deviceType);

		if (deviceRef)
		{
			SerializationGravityTracker3DOF* device = dynamic_cast<SerializationGravityTracker3DOF*>(deviceRef.pointer());
			ocean_assert(device);

			if (device)
			{
				const Timestamp timestamp(sample->dataTimestamp().forceDouble());

				const Tracker::ReferenceSystem referenceSystem = Tracker::ReferenceSystem(gravitySample->referenceSystem());

				const VectorsF3& gravitiesF = gravitySample->gravities();

				GravityTracker3DOF::GravityTracker3DOFSample::Gravities gravities;
				gravities.reserve(gravitiesF.size());

				if constexpr (std::is_same<Scalar, float>::value)
				{
					for (const VectorF3& gravityF : gravitiesF)
					{
						gravities.emplaceBack(gravityF);
					}
				}
				else
				{
					for (const VectorF3& gravityF : gravitiesF)
					{
						gravities.emplaceBack(Vector3(gravityF).normalizedOrZero());
					}
				}

				Measurement::Metadata metadata;

				device->forwardSampleEvent(std::move(gravities), referenceSystem, timestamp, std::move(metadata));
			}
		}
	}
}

void SerializerDevicePlayer::processDataSamplePositionTracker3DOF(const IO::Serialization::DataSerializer::ChannelId channelId, IO::Serialization::UniqueDataSample&& sample)
{
	ocean_assert(sample);

	DeviceSerializer::DataSamplePositionTracker3DOF* positionSample = dynamic_cast<DeviceSerializer::DataSamplePositionTracker3DOF*>(sample.get());
	ocean_assert(positionSample);

	if (positionSample)
	{
		const std::string deviceName = "Serialization PositionTracker3DOF";
		const Device::DeviceType deviceType = SerializationPositionTracker3DOF::deviceTypeSerializationPositionTracker3DOF();

		DeviceRef deviceRef = ensureDevice(channelId, deviceName, deviceType);

		if (deviceRef)
		{
			SerializationPositionTracker3DOF* device = dynamic_cast<SerializationPositionTracker3DOF*>(deviceRef.pointer());
			ocean_assert(device);

			if (device)
			{
				const Timestamp timestamp(sample->dataTimestamp().forceDouble());

				const Tracker::ReferenceSystem referenceSystem = Tracker::ReferenceSystem(positionSample->referenceSystem());

				const VectorsF3& positionsF = positionSample->positions();

				PositionTracker3DOF::PositionTracker3DOFSample::Positions positions;
				positions.reserve(positionsF.size());

				for (const VectorF3& positionF : positionsF)
				{
					positions.emplaceBack(Vector3(positionF));
				}

				Measurement::Metadata metadata;

				device->forwardSampleEvent(std::move(positions), referenceSystem, timestamp, std::move(metadata));
			}
		}
	}
}

void SerializerDevicePlayer::processDataSampleTracker6DOF(const IO::Serialization::DataSerializer::ChannelId channelId, IO::Serialization::UniqueDataSample&& sample)
{
	ocean_assert(sample);

	DeviceSerializer::DataSampleTracker6DOF* tracker6DOFSample = dynamic_cast<DeviceSerializer::DataSampleTracker6DOF*>(sample.get());
	ocean_assert(tracker6DOFSample);

	if (tracker6DOFSample)
	{
		const std::string deviceName = "Serialization Tracker6DOF";
		const Device::DeviceType deviceType = SerializationTracker6DOF::deviceTypeSerializationTracker6DOF();

		DeviceRef deviceRef = ensureDevice(channelId, deviceName, deviceType);

		if (deviceRef)
		{
			SerializationTracker6DOF* device = dynamic_cast<SerializationTracker6DOF*>(deviceRef.pointer());
			ocean_assert(device);

			if (device)
			{
				const Timestamp timestamp(sample->dataTimestamp().forceDouble());

				const Tracker::ReferenceSystem referenceSystem = Tracker::ReferenceSystem(tracker6DOFSample->referenceSystem());

				const QuaternionsF& orientationsF = tracker6DOFSample->orientations();
				const VectorsF3& positionsF = tracker6DOFSample->positions();

				Tracker6DOF::Tracker6DOFSample::Orientations orientations;
				orientations.reserve(orientationsF.size());

				if constexpr (std::is_same<Scalar, float>::value)
				{
					for (const QuaternionF& orientationF : orientationsF)
					{
						orientations.emplaceBack(orientationF);
					}
				}
				else
				{
					for (const QuaternionF& orientationF : orientationsF)
					{
						orientations.emplaceBack(Quaternion(orientationF).normalized());
					}
				}

				Tracker6DOF::Tracker6DOFSample::Positions positions;
				positions.reserve(positionsF.size());

				for (const VectorF3& positionF : positionsF)
				{
					positions.emplaceBack(Vector3(positionF));
				}

				Measurement::Metadata metadata;

				device->forwardSampleEvent(std::move(orientations), std::move(positions), referenceSystem, timestamp, std::move(metadata));
			}
		}
	}
}

void SerializerDevicePlayer::processDataSampleGPSTracker(const IO::Serialization::DataSerializer::ChannelId channelId, IO::Serialization::UniqueDataSample&& sample)
{
	ocean_assert(sample);

	DeviceSerializer::DataSampleGPSTracker* gpsSample = dynamic_cast<DeviceSerializer::DataSampleGPSTracker*>(sample.get());
	ocean_assert(gpsSample);

	if (gpsSample)
	{
		const std::string deviceName = "Serialization GPSTracker";
		const Device::DeviceType deviceType = SerializationGPSTracker::deviceTypeSerializationGPSTracker();

		DeviceRef deviceRef = ensureDevice(channelId, deviceName, deviceType);

		if (deviceRef)
		{
			SerializationGPSTracker* device = dynamic_cast<SerializationGPSTracker*>(deviceRef.pointer());
			ocean_assert(device);

			if (device)
			{
				const Timestamp timestamp(sample->dataTimestamp().forceDouble());

				const Tracker::ReferenceSystem referenceSystem = Tracker::ReferenceSystem(gpsSample->referenceSystem());

				DeviceSerializer::DataSampleGPSTracker::Locations serializerLocations = gpsSample->locations();

				GPSTracker::Locations locations;
				locations.reserve(serializerLocations.size());

				for (const DeviceSerializer::DataSampleGPSTracker::Location& serializerLocation : serializerLocations)
				{
					locations.emplaceBack(serializerLocation.latitude_, serializerLocation.longitude_, serializerLocation.altitude_, serializerLocation.direction_, serializerLocation.speed_, serializerLocation.accuracy_, serializerLocation.altitudeAccuracy_, serializerLocation.directionAccuracy_, serializerLocation.speedAccuracy_);
				}

				Measurement::Metadata metadata;

				device->forwardSampleEvent(std::move(locations), referenceSystem, timestamp, std::move(metadata));
			}
		}
	}
}

void SerializerDevicePlayer::processSample(const IO::Serialization::DataSerializer::ChannelId channelId, IO::Serialization::UniqueDataSample&& sample)
{
	ocean_assert(sample);

	const ChannelProcessorMap::const_iterator iChannelProcessor = channelProcessorMap_.find(channelId);

	if (iChannelProcessor != channelProcessorMap_.cend())
	{
		(this->*(iChannelProcessor->second))(channelId, std::move(sample));
	}
	else
	{
		const std::string& sampleType = sample->type();

		const SampleTypeMap::const_iterator iSampleProcessor = sampleTypeMap_.find(sampleType);

		if (iSampleProcessor != sampleTypeMap_.cend())
		{
			channelProcessorMap_[channelId] = iSampleProcessor->second;

			(this->*(iSampleProcessor->second))(channelId, std::move(sample));
		}
	}
}

void SerializerDevicePlayer::processLookaheadSamples(const IO::Serialization::DataTimestamp& dataTimestamp, const double maxPlaybackTimestamp)
{
	ocean_assert(firstMediaFrameChannelId_ != IO::Serialization::DataSerializer::invalidChannelId());

	// first, lets check whether we still have queued samples which we need to process

	for (size_t nQueuedSample = 0; nQueuedSample < stopMotionSampleQueue_.size(); ++nQueuedSample)
	{
		SamplePair& samplePair = stopMotionSampleQueue_[nQueuedSample];

		if (samplePair.second == nullptr)
		{
			// the sample has been processed in a previous iteration
			continue;
		}

		if (samplePair.second->playbackTimestamp() > maxPlaybackTimestamp)
		{
			/// we have reach the lookahead window
			return;
		}

		if (samplePair.first == firstMediaFrameChannelId_)
		{
			// this sample is from the first media channel, which we exclude from the lookahead window
			continue;
		}

		if (samplePair.second->dataTimestamp() > dataTimestamp)
		{
			// this sample has a newer timestamp than the sample we are currently processing
			continue;
		}

		processSample(samplePair.first, std::move(samplePair.second));
		samplePair.second = nullptr;
	}

	// now, lets check whether we still have samples in the input serializer

	while (true)
	{
		SamplePair samplePair;
		samplePair.second = inputSerializer_->sample(samplePair.first, 0.0 /*speed*/);

		if (!samplePair.second)
		{
			if (inputSerializer_->hasFinished())
			{
				// we have reached the end of the serializer data
				return;
			}
			else
			{
				Thread::sleep(1u);
				continue;
			}
		}

		if (samplePair.first != firstMediaFrameChannelId_)
		{
			if (samplePair.second->dataTimestamp() <= dataTimestamp)
			{
				processSample(samplePair.first, std::move(samplePair.second));
				continue;
			}
		}

		// the sample does not yet need to be processed, let's queue it

		const bool outsideLookaheadWindow = samplePair.second->playbackTimestamp() > maxPlaybackTimestamp;

		stopMotionSampleQueue_.emplace_back(std::move(samplePair));

		if (outsideLookaheadWindow)
		{
			// the sample is outside the lookahead window, so we can stop here
			break;
		}
	}
}

SerializerDevicePlayer::DeviceRef SerializerDevicePlayer::ensureDevice(const IO::Serialization::DataSerializer::ChannelId channelId, const std::string& deviceName, const Device::DeviceType& deviceType)
{
	ocean_assert(!deviceName.empty());
	ocean_assert(deviceType);

	const ChannelDeviceMap::const_iterator iDevice = channelDeviceMap_.find(channelId);

	if (iDevice != channelDeviceMap_.cend())
	{
		return iDevice->second;
	}

	// Device doesn't exist yet, create it via Manager adhoc registration (matching VRS pattern)
	const std::string uniqueDeviceName = deviceName + "_" + String::toAString(channelId);

	// Try to get the device type from the channel's contentType field (format: "DEVICE_TRACKER,TRACKER_6DOF")
	Device::DeviceType actualDeviceType = deviceType;

	ocean_assert(inputSerializer_);
	const IO::Serialization::DataSerializer::ChannelConfiguration channelConfiguration = inputSerializer_->channelConfiguration(channelId);

	if (channelConfiguration.isValid() && !channelConfiguration.contentType().empty())
	{
		// Parse contentType to extract major and minor type strings
		const std::string& contentType = channelConfiguration.contentType();
		const size_t commaPos = contentType.find(',');

		if (commaPos != std::string::npos)
		{
			const std::string deviceTypeMajor = contentType.substr(0, commaPos);
			const std::string deviceTypeMinor = contentType.substr(commaPos + 1);

			const Device::DeviceType parsedDeviceType = Device::DeviceType::translateDeviceType(deviceTypeMajor, deviceTypeMinor);

			if (parsedDeviceType)
			{
				actualDeviceType = parsedDeviceType;
			}
			else
			{
				Log::warning() << "SerializerDevicePlayer: Failed to parse device type from contentType: '" << contentType << "', using fallback type";
			}
		}
	}

	// Determine which factory function to use based on device type
	Manager::AdhocInstanceFunction factoryFunction;

	if (actualDeviceType == SerializationOrientationTracker3DOF::deviceTypeSerializationOrientationTracker3DOF())
	{
		factoryFunction = Manager::AdhocInstanceFunction::createStatic(&SerializerDevicePlayer::createOrientationTracker3DOF);
	}
	else if (actualDeviceType == SerializationAccelerationSensor3DOF::deviceTypeSerializationAccelerationSensor3DOF(AccelerationSensor3DOF::SENSOR_ACCELERATION_3DOF)
	    		|| actualDeviceType == SerializationAccelerationSensor3DOF::deviceTypeSerializationAccelerationSensor3DOF(AccelerationSensor3DOF::SENSOR_LINEAR_ACCELERATION_3DOF))
	{
		factoryFunction = Manager::AdhocInstanceFunction::createStatic(&SerializerDevicePlayer::createAccelerationSensor3DOF);
	}
	else if (actualDeviceType == SerializationGyroSensor3DOF::deviceTypeSerializationGyroSensor3DOF(GyroSensor3DOF::SENSOR_GYRO_RAW_3DOF)
	        	|| actualDeviceType == SerializationGyroSensor3DOF::deviceTypeSerializationGyroSensor3DOF(GyroSensor3DOF::SENSOR_GYRO_UNBIASED_3DOF))
	{
		factoryFunction = Manager::AdhocInstanceFunction::createStatic(&SerializerDevicePlayer::createGyroSensor3DOF);
	}
	else if (actualDeviceType == SerializationGravityTracker3DOF::deviceTypeSerializationGravityTracker3DOF())
	{
		factoryFunction = Manager::AdhocInstanceFunction::createStatic(&SerializerDevicePlayer::createGravityTracker3DOF);
	}
	else if (actualDeviceType == SerializationPositionTracker3DOF::deviceTypeSerializationPositionTracker3DOF())
	{
		factoryFunction = Manager::AdhocInstanceFunction::createStatic(&SerializerDevicePlayer::createPositionTracker3DOF);
	}
	else if (actualDeviceType == SerializationTracker6DOF::deviceTypeSerializationTracker6DOF())
	{
		factoryFunction = Manager::AdhocInstanceFunction::createStatic(&SerializerDevicePlayer::createTracker6DOF);
	}
	else if (actualDeviceType == SerializationGPSTracker::deviceTypeSerializationGPSTracker())
	{
		factoryFunction = Manager::AdhocInstanceFunction::createStatic(&SerializerDevicePlayer::createGPSTracker);
	}
	else
	{
		ocean_assert(false && "Unsupported device type");
		return DeviceRef();
	}

	// Register the device with the Manager (matching VRS pattern)
	if (!Manager::get().registerAdhocDevice(uniqueDeviceName, actualDeviceType, factoryFunction))
	{
		ocean_assert(false && "Failed to register device");
		return DeviceRef();
	}

	Log::info() << "Serialization contains device '" << uniqueDeviceName << "'";

	DeviceRef device = Manager::get().device(uniqueDeviceName, false /* useExclusive */);
	ocean_assert(device);

	if (device)
	{
		device->start();

		channelDeviceMap_[channelId] = device;
	}

	return device;
}

void SerializerDevicePlayer::threadRun()
{
	ocean_assert(inputSerializer_);
	ocean_assert(speed_ > 0.0f);

	ocean_assert(isStarted_);

	while (!shouldThreadStop())
	{
		IO::Serialization::DataSerializer::ChannelId channelId = IO::Serialization::DataSerializer::invalidChannelId();
		IO::Serialization::UniqueDataSample sample = inputSerializer_->sample(channelId, double(speed_));

		if (!sample)
		{
			if (inputSerializer_->hasFinished())
			{
				// we have reached the end of the input data

				break;
			}

			Thread::sleep(1u);
			continue;
		}

		processSample(channelId, std::move(sample));
	}

	isStarted_ = false;
}

Device* SerializerDevicePlayer::createOrientationTracker3DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	ocean_assert_and_suppress_unused(deviceType == SerializationOrientationTracker3DOF::deviceTypeSerializationOrientationTracker3DOF(), deviceType);

	return new SerializationOrientationTracker3DOF(name);
}

Device* SerializerDevicePlayer::createAccelerationSensor3DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	const Sensor::SensorType sensorType = Sensor::SensorType(deviceType.minorType());

	ocean_assert(sensorType == Sensor::SENSOR_ACCELERATION_3DOF || sensorType == Sensor::SENSOR_LINEAR_ACCELERATION_3DOF);

	if (sensorType == Sensor::SENSOR_ACCELERATION_3DOF || sensorType == Sensor::SENSOR_LINEAR_ACCELERATION_3DOF)
	{
		return new SerializationAccelerationSensor3DOF(name, sensorType);
	}

	ocean_assert(false && "Device type is not supported!");
	return nullptr;
}

Device* SerializerDevicePlayer::createGyroSensor3DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	const Sensor::SensorType sensorType = Sensor::SensorType(deviceType.minorType());

	ocean_assert(sensorType == Sensor::SENSOR_GYRO_RAW_3DOF || sensorType == Sensor::SENSOR_GYRO_UNBIASED_3DOF);

	if (sensorType == Sensor::SENSOR_GYRO_RAW_3DOF || sensorType == Sensor::SENSOR_GYRO_UNBIASED_3DOF)
	{
		return new SerializationGyroSensor3DOF(name, sensorType);
	}

	ocean_assert(false && "Device type is not supported!");
	return nullptr;
}

Device* SerializerDevicePlayer::createGravityTracker3DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	ocean_assert_and_suppress_unused(deviceType == SerializationGravityTracker3DOF::deviceTypeSerializationGravityTracker3DOF(), deviceType);

	return new SerializationGravityTracker3DOF(name);
}

Device* SerializerDevicePlayer::createPositionTracker3DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	ocean_assert_and_suppress_unused(deviceType == SerializationPositionTracker3DOF::deviceTypeSerializationPositionTracker3DOF(), deviceType);

	return new SerializationPositionTracker3DOF(name);
}

Device* SerializerDevicePlayer::createTracker6DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	ocean_assert_and_suppress_unused(deviceType == SerializationTracker6DOF::deviceTypeSerializationTracker6DOF(), deviceType);

	return new SerializationTracker6DOF(name);
}

Device* SerializerDevicePlayer::createGPSTracker(const std::string& name, const Device::DeviceType& deviceType)
{
	ocean_assert_and_suppress_unused(deviceType == SerializationGPSTracker::deviceTypeSerializationGPSTracker(), deviceType);

	return new SerializationGPSTracker(name);
}

}

}

}
