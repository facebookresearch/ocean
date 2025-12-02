/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/serialization/SerializerDeviceRecorder.h"
#include "ocean/devices/serialization/DeviceSerializer.h"

#include "ocean/devices/DeviceRef.h"
#include "ocean/devices/Manager.h"

#include "ocean/io/serialization/MediaSerializer.h"

#include "ocean/media/PixelImage.h"

namespace Ocean
{

namespace Devices
{

namespace Serialization
{

SerializerDeviceRecorder::SerializerDeviceRecorder()
{
	DeviceRefManager::get().addDeviceCallbackFunction(DeviceRefManager::DeviceCallback::create(*this, &SerializerDeviceRecorder::onDeviceChanged));
	callbackEventDeviceChangedRegistered_ = true;

	outputSerializer_ = std::make_unique<IO::Serialization::FileOutputDataSerializer>();
}

SerializerDeviceRecorder::~SerializerDeviceRecorder()
{
	release();
}

bool SerializerDeviceRecorder::start(const std::string& filename)
{
	if (filename.empty())
	{
		return false;
	}

	const ScopedLock scopedLock(recorderLock_);

	const RecorderState recorderState = recorderState_;

	if (recorderState == RS_RECORDING)
	{
		return true;
	}

	if (recorderState >= RS_STOPPING)
	{
		return false;
	}

	if (!outputSerializer_)
	{
		ocean_assert(false && "The serializer has been released already");
		return false;
	}

	if (!outputSerializer_->setFilename(filename))
	{
		outputSerializer_ = nullptr;
		return false;
	}

	if (!outputSerializer_->start())
	{
		outputSerializer_ = nullptr;
		return false;
	}

	recorderState_ = RS_RECORDING;

	startThread();

	return true;
}

bool SerializerDeviceRecorder::stop()
{
	const ScopedLock scopedLock(recorderLock_);

	const RecorderState recorderState = recorderState_;

	if (recorderState == RS_IDLE)
	{
		return false;
	}

	if (recorderState >= RS_STOPPING)
	{
		return true;
	}

	recorderState_ = RS_STOPPING;

	queueUnqueuedSamples();

	return true;
}

bool SerializerDeviceRecorder::addFrameMedium(const Media::FrameMediumRef& frameMedium)
{
	if (frameMedium.isNull())
	{
		ocean_assert(false && "Invalid medium!");
		return false;
	}

	const Media::FrameMedium* frameMediumPointer = nullptr;

	{
		const ScopedLock scopedLock(frameMediumMapLock_);

		frameMediumPointer = &*frameMedium;

		FrameMediumMap::iterator iFrameMedium = frameMediumMap_.emplace(frameMediumPointer, FrameMediumData(frameMedium)).first;
		ocean_assert(iFrameMedium != frameMediumMap_.end());

		++iFrameMedium->second.referenceCounter_;

		if (iFrameMedium->second.referenceCounter_ >= 2u)
		{
			// the frame has been registered already before, nothing to do anymore
			return true;
		}
	}

	IO::Serialization::DataSerializer::ChannelId channelId = IO::Serialization::DataSerializer::invalidChannelId();

	{
		const ScopedLock scopedLockRecorder(recorderLock_);

		if (!outputSerializer_)
		{
			Log::error() << "SerializerDeviceRecorder: Failed to add frame medium, output serializer does not exist";
			return false;
		}

		const std::string channelName = std::string("FrameMedium,") + frameMedium->url();
		channelId = outputSerializer_->addChannel(IO::Serialization::MediaSerializer::DataSampleFrame::sampleType(), channelName, "frame");
	}

	ocean_assert(channelId != IO::Serialization::DataSerializer::invalidChannelId());
	if (channelId == IO::Serialization::DataSerializer::invalidChannelId())
	{
		Log::error() << "SerializerDeviceRecorder: Failed to add channel for frame medium '" << frameMedium->url() << "'";
		return false;
	}

	const ScopedLock scopedLock(frameMediumMapLock_);

	FrameMediumMap::iterator iFrameMedium = frameMediumMap_.find(frameMediumPointer);

	ocean_assert(iFrameMedium != frameMediumMap_.cend());
	if (iFrameMedium == frameMediumMap_.cend())
	{
		return false;
	}

	iFrameMedium->second.channelId_ = channelId;

	return true;
}

bool SerializerDeviceRecorder::removeFrameMedium(const Media::FrameMediumRef& frameMedium)
{
	if (frameMedium.isNull())
	{
		ocean_assert(false && "Invalid medium!");
		return false;
	}

	const ScopedLock scopedLock(frameMediumMapLock_);

	const Media::FrameMedium* frameMediumPtr = &*frameMedium;

	const FrameMediumMap::iterator iFrameMedium = frameMediumMap_.find(frameMediumPtr);

	if (iFrameMedium == frameMediumMap_.end())
	{
		ocean_assert(false && "Medium not found!");
		return false;
	}

	FrameMediumData& frameMediumData = iFrameMedium->second;

	ocean_assert(frameMediumData.referenceCounter_ > 0u);
	--frameMediumData.referenceCounter_;

	if (frameMediumData.referenceCounter_ == 0u)
	{
		frameMediumMap_.erase(iFrameMedium);
	}

	return true;
}

IO::Serialization::DataSerializer::ChannelId SerializerDeviceRecorder::addExtraChannel(const std::string& sampleType, const std::string& channelName, const std::string& contentType)
{
	ocean_assert(!sampleType.empty());
	ocean_assert(!channelName.empty());
	ocean_assert(!contentType.empty());

	const ScopedLock scopedLock(recorderLock_);

	return outputSerializer_->addChannel(sampleType, channelName, contentType);
}

bool SerializerDeviceRecorder::addExtraDataSample(const IO::Serialization::DataSerializer::ChannelId channelId, IO::Serialization::UniqueDataSample&& sample)
{
	ocean_assert(channelId != IO::Serialization::DataSerializer::invalidChannelId());

	if (channelId == IO::Serialization::DataSerializer::invalidChannelId())
	{
		return false;
	}

	const ScopedLock scopedLock(extraDataSampleQueueLock_);

	extraDataSampleQueue_.emplace(channelId, std::move(sample));

	return true;
}

void SerializerDeviceRecorder::release()
{
	if (callbackEventDeviceChangedRegistered_)
	{
		DeviceRefManager::get().removeDeviceCallbackFunction(DeviceRefManager::DeviceCallback::create(*this, &SerializerDeviceRecorder::onDeviceChanged));
		callbackEventDeviceChangedRegistered_ = false;
	}

	stop();

	TemporaryScopedLock scopedLock(recorderLock_);
		sampleEventSubscriptionMap_.clear();
		trackerObjectEventSubscriptionMap_.clear();
	scopedLock.release();

	ocean_assert(recorderState_ >= RS_STOPPING);

	while (recorderState_ != RS_STOPPED)
	{
		Thread::sleep(1u);
	}
}

void SerializerDeviceRecorder::onDeviceChanged(Device* device, const bool added)
{
	ocean_assert(device != nullptr);

	Measurement* measurement = dynamic_cast<Measurement*>(device);

	const ScopedLock scopedLock(recorderLock_);

	if (added)
	{
		if (measurement != nullptr)
		{
			ocean_assert(sampleEventSubscriptionMap_.find(device) == sampleEventSubscriptionMap_.cend());

			Measurement::SampleEventSubscription sampleEventSubscription(measurement->subscribeSampleEvent(Measurement::SampleCallback::create(*this, &SerializerDeviceRecorder::onMeasurementSample)));
			sampleEventSubscription.makeWeak();

			sampleEventSubscriptionMap_.emplace(device, std::move(sampleEventSubscription));
		}

		Tracker* tracker = dynamic_cast<Tracker*>(device);

		if (tracker != nullptr)
		{
			ocean_assert(trackerObjectEventSubscriptionMap_.find(device) == trackerObjectEventSubscriptionMap_.cend());

			Tracker::TrackerObjectEventSubscription trackerObjectEventSubscription(tracker->subscribeTrackerObjectEvent(Tracker::TrackerObjectCallback::create(*this, &SerializerDeviceRecorder::onTrackerObject)));
			trackerObjectEventSubscription.makeWeak();

			trackerObjectEventSubscriptionMap_.emplace(device, std::move(trackerObjectEventSubscription));
		}
	}
	else
	{
		trackerObjectEventSubscriptionMap_.erase(device);
		sampleEventSubscriptionMap_.erase(device);

		if (measurement != nullptr)
		{
			const MeasurementChannelMap::const_iterator iMeasurement = measurementChannelMap_.find(measurement);

			if (iMeasurement != measurementChannelMap_.cend())
			{
				invalidMeasurements_.emplace(iMeasurement->first);
			}
		}
	}
}

void SerializerDeviceRecorder::onMeasurementSample(const Measurement* sender, const Measurement::SampleRef& sample)
{
	ocean_assert(sender != nullptr);
	ocean_assert(sample);

	if (recorderState_ != RS_RECORDING)
	{
		return;
	}

	TemporaryScopedLock unqueuedScopedLock(unqueuedDeviceSampleDatasLock_);

		unqueuedDeviceSampleDatas_.emplace_back(sender, sample);

		if (unqueuedDeviceSampleDatas_.size() < 20)
		{
			return;
		}

		// we have a enough samples which now can be moved to the real queue
		// this two step approach is supposed to reduce waiting time for locks

		DeviceSampleDatas unqueuedDeviceSampleDatas = std::move(unqueuedDeviceSampleDatas_);

	unqueuedScopedLock.release();

	ocean_assert(!unqueuedDeviceSampleDatas.empty());

	const ScopedLock queueScopedLock(deviceSampleDataQueueLock_);

	deviceSampleDataQueue_.emplace(std::move(unqueuedDeviceSampleDatas));
}

void SerializerDeviceRecorder::onTrackerObject(const Tracker* /*sender*/, const bool /*found*/, const Measurement::ObjectIdSet& /*objectIds*/, const Timestamp& /*timestamp*/)
{
	// nothing to do here
}

void SerializerDeviceRecorder::recordSample(const Measurement* sender, const Measurement::SampleRef& sample, const Timestamp& sampleCreationTimestamp)
{
	ocean_assert(sender != nullptr);

	if (const Sensor* sensor = dynamic_cast<const Sensor*>(sender))
	{
		recordSensorSample(sender, sample, sampleCreationTimestamp);
	}
	else if (const Tracker* tracker = dynamic_cast<const Tracker*>(sender))
	{
		recordTrackerSample(sender, sample, sampleCreationTimestamp);
	}
	else
	{
		ocean_assert(false && "Unknown measurement type!");
	}
}

void SerializerDeviceRecorder::recordSensorSample(const Measurement* sender, const Measurement::SampleRef& sample, const Timestamp& sampleCreationTimestamp)
{
	ocean_assert(sender != nullptr);

	if (sample)
	{
		if (AccelerationSensor3DOF::Acceleration3DOFSampleRef accelerationSample = sample)
		{
			recordAccelerationSensor3DOFSample(sender, accelerationSample, sampleCreationTimestamp);
			return;
		}

		if (GyroSensor3DOF::Gyro3DOFSampleRef gyroSample = sample)
		{
			recordGyroSensor3DOFSample(sender, gyroSample, sampleCreationTimestamp);
			return;
		}
	}
}

void SerializerDeviceRecorder::recordTrackerSample(const Measurement* sender, const Measurement::SampleRef& sample, const Timestamp& sampleCreationTimestamp)
{
	ocean_assert(sender != nullptr);

	if (sample)
	{
		if (Tracker6DOF::Tracker6DOFSampleRef tracker6DOFSample = sample)
		{
			recordTracker6DOFSample(sender, tracker6DOFSample, sampleCreationTimestamp);
			return;
		}

		if (GPSTracker::GPSTrackerSampleRef gpsTrackerSample = sample)
		{
			recordGPSTrackerSample(sender, gpsTrackerSample, sampleCreationTimestamp);
			return;
		}

		if (OrientationTracker3DOF::OrientationTracker3DOFSampleRef orientationSample = sample)
		{
			recordOrientationTracker3DOFSample(sender, orientationSample, sampleCreationTimestamp);
			return;
		}

		if (GravityTracker3DOF::GravityTracker3DOFSampleRef gravitySample = sample)
		{
			recordGravityTracker3DOFSample(sender, gravitySample, sampleCreationTimestamp);
			return;
		}

		if (PositionTracker3DOF::PositionTracker3DOFSampleRef positionSample = sample)
		{
			recordPositionTracker3DOFSample(sender, positionSample, sampleCreationTimestamp);
			return;
		}
	}
}

void SerializerDeviceRecorder::recordTracker6DOFSample(const Measurement* sender, const Tracker6DOF::Tracker6DOFSampleRef& sample, const Timestamp& sampleCreationTimestamp)
{
	ocean_assert(sender != nullptr);
	ocean_assert(sample);
	ocean_assert(outputSerializer_);

	IO::Serialization::DataSerializer::ChannelId channelId = IO::Serialization::DataSerializer::invalidChannelId();

	const MeasurementChannelMap::const_iterator iMeasurement = measurementChannelMap_.find(sender);

	if (iMeasurement != measurementChannelMap_.cend())
	{
		channelId = iMeasurement->second;
	}
	else
	{
		const std::string& channelName = sender->name();

		const Device::DeviceType deviceType = sender->type();
		const std::string contentType = Device::translateMajorType(deviceType.majorType()) + "," + Tracker::translateTrackerType(Tracker::TrackerType(deviceType.minorType()));

		channelId = outputSerializer_->addChannel(DeviceSerializer::DataSampleTracker6DOF::sampleType(), channelName, contentType);
		measurementChannelMap_.emplace(sender, channelId);

		Log::info() << "Serialization recorder contains 6DOF tracker '" << channelName << "' with channel id: " << channelId << " (type: " << contentType << ")";
	}

	ocean_assert(channelId != IO::Serialization::DataSerializer::invalidChannelId());

	if (sample)
	{
		std::unique_ptr<DeviceSerializer::DataSampleTracker6DOF> dataSample = std::make_unique<DeviceSerializer::DataSampleTracker6DOF>(*sample, sampleCreationTimestamp);

		outputSerializer_->addSample(channelId, std::move(dataSample));
	}
}

void SerializerDeviceRecorder::recordGPSTrackerSample(const Measurement* sender, const GPSTracker::GPSTrackerSampleRef& sample, const Timestamp& sampleCreationTimestamp)
{
	ocean_assert(sender != nullptr);
	ocean_assert(sample);
	ocean_assert(outputSerializer_);

	IO::Serialization::DataSerializer::ChannelId channelId = IO::Serialization::DataSerializer::invalidChannelId();

	const MeasurementChannelMap::const_iterator iMeasurement = measurementChannelMap_.find(sender);

	if (iMeasurement != measurementChannelMap_.cend())
	{
		channelId = iMeasurement->second;
	}
	else
	{
		const std::string& channelName = sender->name();

		const Device::DeviceType deviceType = sender->type();
		const std::string contentType = Device::translateMajorType(deviceType.majorType()) + "," + Tracker::translateTrackerType(Tracker::TrackerType(deviceType.minorType()));

		channelId = outputSerializer_->addChannel(DeviceSerializer::DataSampleGPSTracker::sampleType(), channelName, contentType);
		measurementChannelMap_.emplace(sender, channelId);

		Log::info() << "Serialization recorder contains GPS tracker '" << channelName << "' with channel id: " << channelId << " (type: " << contentType << ")";
	}

	ocean_assert(channelId != IO::Serialization::DataSerializer::invalidChannelId());

	if (sample)
	{
		std::unique_ptr<DeviceSerializer::DataSampleGPSTracker> dataSample = std::make_unique<DeviceSerializer::DataSampleGPSTracker>(*sample, sampleCreationTimestamp);

		outputSerializer_->addSample(channelId, std::move(dataSample));
	}
}

void SerializerDeviceRecorder::recordAccelerationSensor3DOFSample(const Measurement* sender, const AccelerationSensor3DOF::Acceleration3DOFSampleRef& sample, const Timestamp& sampleCreationTimestamp)
{
	ocean_assert(sender != nullptr);
	ocean_assert(sample);
	ocean_assert(outputSerializer_);

	IO::Serialization::DataSerializer::ChannelId channelId = IO::Serialization::DataSerializer::invalidChannelId();

	const MeasurementChannelMap::const_iterator iMeasurement = measurementChannelMap_.find(sender);

	if (iMeasurement != measurementChannelMap_.cend())
	{
		channelId = iMeasurement->second;
	}
	else
	{
		const std::string& channelName = sender->name();

		const Device::DeviceType deviceType = sender->type();
		const std::string contentType = Device::translateMajorType(deviceType.majorType()) + "," + Sensor::translateSensorType(Sensor::SensorType(deviceType.minorType()));

		channelId = outputSerializer_->addChannel(DeviceSerializer::DataSampleAccelerationSensor3DOF::sampleType(), channelName, contentType);
		measurementChannelMap_.emplace(sender, channelId);

		Log::info() << "Serialization recorder contains 3DOF acceleration sensor '" << channelName << "' with channel id: " << channelId << " (type: " << contentType << ")";
	}

	ocean_assert(channelId != IO::Serialization::DataSerializer::invalidChannelId());

	std::unique_ptr<DeviceSerializer::DataSampleAccelerationSensor3DOF> dataSample = std::make_unique<DeviceSerializer::DataSampleAccelerationSensor3DOF>(*sample, sampleCreationTimestamp);

	outputSerializer_->addSample(channelId, std::move(dataSample));
}

void SerializerDeviceRecorder::recordGyroSensor3DOFSample(const Measurement* sender, const GyroSensor3DOF::Gyro3DOFSampleRef& sample, const Timestamp& sampleCreationTimestamp)
{
	ocean_assert(sender != nullptr);
	ocean_assert(sample);
	ocean_assert(outputSerializer_);

	IO::Serialization::DataSerializer::ChannelId channelId = IO::Serialization::DataSerializer::invalidChannelId();

	const MeasurementChannelMap::const_iterator iMeasurement = measurementChannelMap_.find(sender);

	if (iMeasurement != measurementChannelMap_.cend())
	{
		channelId = iMeasurement->second;
	}
	else
	{
		const std::string& channelName = sender->name();

		const Device::DeviceType deviceType = sender->type();
		const std::string contentType = Device::translateMajorType(deviceType.majorType()) + "," + Sensor::translateSensorType(Sensor::SensorType(deviceType.minorType()));

		channelId = outputSerializer_->addChannel(DeviceSerializer::DataSampleGyroSensor3DOF::sampleType(), channelName, contentType);
		measurementChannelMap_.emplace(sender, channelId);

		Log::info() << "Serialization recorder contains 3DOF gyro sensor '" << channelName << "' with channel id: " << channelId << " (type: " << contentType << ")";
	}

	ocean_assert(channelId != IO::Serialization::DataSerializer::invalidChannelId());

	std::unique_ptr<DeviceSerializer::DataSampleGyroSensor3DOF> dataSample = std::make_unique<DeviceSerializer::DataSampleGyroSensor3DOF>(*sample, sampleCreationTimestamp);

	outputSerializer_->addSample(channelId, std::move(dataSample));
}

void SerializerDeviceRecorder::recordOrientationTracker3DOFSample(const Measurement* sender, const OrientationTracker3DOF::OrientationTracker3DOFSampleRef& sample, const Timestamp& sampleCreationTimestamp)
{
	ocean_assert(sender != nullptr);
	ocean_assert(sample);
	ocean_assert(outputSerializer_);

	IO::Serialization::DataSerializer::ChannelId channelId = IO::Serialization::DataSerializer::invalidChannelId();

	const MeasurementChannelMap::const_iterator iMeasurement = measurementChannelMap_.find(sender);

	if (iMeasurement != measurementChannelMap_.cend())
	{
		channelId = iMeasurement->second;
	}
	else
	{
		const std::string& channelName = sender->name();

		const Device::DeviceType deviceType = sender->type();
		const std::string contentType = Device::translateMajorType(deviceType.majorType()) + "," + Tracker::translateTrackerType(Tracker::TrackerType(deviceType.minorType()));

		channelId = outputSerializer_->addChannel(DeviceSerializer::DataSampleOrientationTracker3DOF::sampleType(), channelName, contentType);
		measurementChannelMap_.emplace(sender, channelId);

		Log::info() << "Serialization recorder contains 3DOF orientation tracker '" << channelName << "' with channel id: " << channelId << " (type: " << contentType << ")";
	}

	ocean_assert(channelId != IO::Serialization::DataSerializer::invalidChannelId());

	std::unique_ptr<DeviceSerializer::DataSampleOrientationTracker3DOF> dataSample = std::make_unique<DeviceSerializer::DataSampleOrientationTracker3DOF>(*sample, sampleCreationTimestamp);

	outputSerializer_->addSample(channelId, std::move(dataSample));
}

void SerializerDeviceRecorder::recordGravityTracker3DOFSample(const Measurement* sender, const GravityTracker3DOF::GravityTracker3DOFSampleRef& sample, const Timestamp& sampleCreationTimestamp)
{
	ocean_assert(sender != nullptr);
	ocean_assert(sample);
	ocean_assert(outputSerializer_);

	IO::Serialization::DataSerializer::ChannelId channelId = IO::Serialization::DataSerializer::invalidChannelId();

	const MeasurementChannelMap::const_iterator iMeasurement = measurementChannelMap_.find(sender);

	if (iMeasurement != measurementChannelMap_.cend())
	{
		channelId = iMeasurement->second;
	}
	else
	{
		const std::string& channelName = sender->name();

		const Device::DeviceType deviceType = sender->type();
		const std::string contentType = Device::translateMajorType(deviceType.majorType()) + "," + Tracker::translateTrackerType(Tracker::TrackerType(deviceType.minorType()));

		channelId = outputSerializer_->addChannel(DeviceSerializer::DataSampleGravityTracker3DOF::sampleType(), channelName, contentType);
		measurementChannelMap_.emplace(sender, channelId);

		Log::info() << "Serialization recorder contains 3DOF gravity tracker '" << channelName << "' with channel id: " << channelId << " (type: " << contentType << ")";
	}

	ocean_assert(channelId != IO::Serialization::DataSerializer::invalidChannelId());

	std::unique_ptr<DeviceSerializer::DataSampleGravityTracker3DOF> dataSample = std::make_unique<DeviceSerializer::DataSampleGravityTracker3DOF>(*sample, sampleCreationTimestamp);

	outputSerializer_->addSample(channelId, std::move(dataSample));
}

void SerializerDeviceRecorder::recordPositionTracker3DOFSample(const Measurement* sender, const PositionTracker3DOF::PositionTracker3DOFSampleRef& sample, const Timestamp& sampleCreationTimestamp)
{
	ocean_assert(sender != nullptr);
	ocean_assert(sample);
	ocean_assert(outputSerializer_);

	IO::Serialization::DataSerializer::ChannelId channelId = IO::Serialization::DataSerializer::invalidChannelId();

	const MeasurementChannelMap::const_iterator iMeasurement = measurementChannelMap_.find(sender);

	if (iMeasurement != measurementChannelMap_.cend())
	{
		channelId = iMeasurement->second;
	}
	else
	{
		const std::string& channelName = sender->name();

		const Device::DeviceType deviceType = sender->type();
		const std::string contentType = Device::translateMajorType(deviceType.majorType()) + "," + Tracker::translateTrackerType(Tracker::TrackerType(deviceType.minorType()));

		channelId = outputSerializer_->addChannel(DeviceSerializer::DataSamplePositionTracker3DOF::sampleType(), channelName, contentType);
		measurementChannelMap_.emplace(sender, channelId);

		Log::info() << "Serialization recorder contains 3DOF position tracker '" << channelName << "' with channel id: " << channelId << " (type: " << contentType << ")";
	}

	ocean_assert(channelId != IO::Serialization::DataSerializer::invalidChannelId());

	std::unique_ptr<DeviceSerializer::DataSamplePositionTracker3DOF> dataSample = std::make_unique<DeviceSerializer::DataSamplePositionTracker3DOF>(*sample, sampleCreationTimestamp);

	outputSerializer_->addSample(channelId, std::move(dataSample));
}

bool SerializerDeviceRecorder::recordFrame(FrameMediumData& frameMediumData, const Frame& frame, const SharedAnyCamera& camera)
{
	ocean_assert(frame.isValid());
	ocean_assert(outputSerializer_);
	ocean_assert(frameMediumData.frameMedium_);

	if (!frame.isValid())
	{
		return false;
	}

	if (frameMediumData.channelId_ == IO::Serialization::DataSerializer::invalidChannelId())
	{
		return false;
	}

	const Timestamp sampleCreationTimestamp(true);

	SharedAnyCamera cameraToUse;

	if (camera)
	{
		if (!frameMediumData.camera_ || !frameMediumData.camera_->isEqual(*camera))
		{
			frameMediumData.camera_ = camera;
			cameraToUse = camera;
		}
		else
		{
			// the camera model has been recorded and has not changed, so we don't need to record it again
		}
	}

	HomogenousMatrixD4 device_T_camera = frameMediumData.frameMedium_->device_T_camera();

	if (device_T_camera.isValid())
	{
		if (!frameMediumData.device_T_camera_.isValid() || !frameMediumData.device_T_camera_.isEqual(device_T_camera, NumericD::weakEps()))
		{
			frameMediumData.device_T_camera_ = device_T_camera;
		}
		else
		{
			// the device transformation has been recorded and has not changed, so we don't need to record it again
			device_T_camera.toNull();
		}
	}

	std::unique_ptr<IO::Serialization::MediaSerializer::DataSampleFrame> dataSample = std::make_unique<IO::Serialization::MediaSerializer::DataSampleFrame>(frame, "ocn", cameraToUse, device_T_camera, sampleCreationTimestamp);

	outputSerializer_->addSample(frameMediumData.channelId_, std::move(dataSample));

	frameMediumData.lastTimestamp_ = frame.timestamp();

	return true;
}

void SerializerDeviceRecorder::queueUnqueuedSamples()
{
	DeviceSampleDatas deviceSampleDatas;

	{
		const ScopedLock scopedLock(unqueuedDeviceSampleDatasLock_);

		if (unqueuedDeviceSampleDatas_.empty())
		{
			return;
		}

		deviceSampleDatas = std::move(unqueuedDeviceSampleDatas_);
		unqueuedDeviceSampleDatas_.clear();
	}

	const ScopedLock scopedLock(deviceSampleDataQueueLock_);

	deviceSampleDataQueue_.emplace(std::move(deviceSampleDatas));
}

void SerializerDeviceRecorder::threadRun()
{
	ocean_assert(outputSerializer_);

	DeviceSampleDatas deviceSampleDatas;

	while (!shouldThreadStop())
	{
		deviceSampleDatas.clear();

		// first let's write the queued samples

		TemporaryScopedLock sampleQueueScopedLock(deviceSampleDataQueueLock_);

			if (!deviceSampleDataQueue_.empty())
			{
				deviceSampleDatas = std::move(deviceSampleDataQueue_.front());
				deviceSampleDataQueue_.pop();
			}

		sampleQueueScopedLock.release();

		const RecorderState recorderState = recorderState_;

		if (deviceSampleDatas.empty())
		{
			if (recorderState >= RS_STOPPING)
			{
				break;
			}
		}

		TemporaryScopedLock recorderScopedLock(recorderLock_);

			for (const DeviceSampleData& deviceSampleData : deviceSampleDatas)
			{
				if (invalidMeasurements_.find(deviceSampleData.measurement_) == invalidMeasurements_.cend())
				{
					recordSample(deviceSampleData.measurement_, deviceSampleData.sample_, deviceSampleData.sampleCreationTimestamp_);
				}
			}

		recorderScopedLock.release();

		bool shouldSleep = deviceSampleDatas.empty();

		if (recorderState == RS_RECORDING)
		{
			const ScopedLock scopedLock(frameMediumMapLock_);

			for (FrameMediumMap::iterator iFrameMedium = frameMediumMap_.begin(); iFrameMedium != frameMediumMap_.end(); ++iFrameMedium)
			{
				FrameMediumData& frameMediumData = iFrameMedium->second;

				SharedAnyCamera camera;
				FrameRef frame = frameMediumData.frameMedium_->frame(&camera);

				if (frame && frame->isValid())
				{
					const Timestamp& frameTimestamp = frame->timestamp();

					if (frameTimestamp.isValid() && (frameMediumData.lastTimestamp_.isInvalid() || frameTimestamp > frameMediumData.lastTimestamp_))
					{
						// PixelImage may have camera in the future, for now camera is nullptr

						const ScopedLock scopedLockRecorder(recorderLock_);

						recordFrame(frameMediumData, *frame, camera);

						shouldSleep = false;
					}
				}
			}
		}

		{
			sampleQueueScopedLock.relock(extraDataSampleQueueLock_);

				ExtraDataSample extraDataSample(IO::Serialization::DataSerializer::invalidChannelId(), nullptr);

				if (!extraDataSampleQueue_.empty())
				{
					extraDataSample = std::move(extraDataSampleQueue_.front());
					extraDataSampleQueue_.pop();
				}

			sampleQueueScopedLock.release();

			if (extraDataSample.first != IO::Serialization::DataSerializer::invalidChannelId())
			{
				ocean_assert(extraDataSample.second);
				outputSerializer_->addSample(extraDataSample.first, std::move(extraDataSample.second));

				shouldSleep = false;
			}
		}

		if (shouldSleep)
		{
			Thread::sleep(1u);
		}
	}

	if (outputSerializer_)
	{
		outputSerializer_->stop();

		while (!outputSerializer_->hasStopped())
		{
			Thread::sleep(1u);
		}
	}

	recorderState_ = RS_STOPPED;
	outputSerializer_ = nullptr;

	Log::debug() << "SerializerDeviceRecorder: Recording thread stopped.";
}

}

}

}
