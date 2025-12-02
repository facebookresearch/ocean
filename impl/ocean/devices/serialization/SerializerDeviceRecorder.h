/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_SERIALIZATION_SERIALIZER_DEVICE_RECORDER_H
#define META_OCEAN_DEVICES_SERIALIZATION_SERIALIZER_DEVICE_RECORDER_H

#include "ocean/devices/serialization/Serialization.h"

#include "ocean/base/Thread.h"

#include "ocean/devices/AccelerationSensor3DOF.h"
#include "ocean/devices/Device.h"
#include "ocean/devices/GPSTracker.h"
#include "ocean/devices/GravityTracker3DOF.h"
#include "ocean/devices/GyroSensor3DOF.h"
#include "ocean/devices/Measurement.h"
#include "ocean/devices/OrientationTracker3DOF.h"
#include "ocean/devices/PositionTracker3DOF.h"
#include "ocean/devices/Tracker6DOF.h"

#include "ocean/io/serialization/OutputDataSerializer.h"

#include "ocean/media/FrameMedium.h"

#include <queue>

namespace Ocean
{

namespace Devices
{

namespace Serialization
{

/**
 * This class implements a serialization recorder for devices and media objects.
 * The recorder serializes device sensor data and frame medium content using Ocean's DataSerializer.
 * @ingroup devicesserialization
 */
class OCEAN_DEVICES_SERIALIZATION_EXPORT SerializerDeviceRecorder : protected Thread
{
	protected:

		/**
		 * Definition of different recorder states.
		 */
		enum RecorderState
		{
			/// Idle state, not recording.
			RS_IDLE = 0u,
			/// Currently recording.
			RS_RECORDING,
			/// Stopping recording.
			RS_STOPPING,
			/// Recording has stopped.
			RS_STOPPED
		};

		/**
		 * This class combines relevant data for a FrameMedium object.
		 */
		class FrameMediumData
		{
			public:

				/**
				 * Creates a new object and initializes the object with an existing FrameMedium object.
				 * @param frameMedium The frame medium object, must be valid
				 */
				explicit inline FrameMediumData(const Media::FrameMediumRef& frameMedium);

			public:

				/// The actual medium object.
				Media::FrameMediumRef frameMedium_;

				/// The camera model of the medium which has been used to record the last frame(s).
				SharedAnyCamera camera_;

				/// The transformation between the camera and the device which has been used to record the last frame(s).
				HomogenousMatrixD4 device_T_camera_ = HomogenousMatrixD4(false);

				/// The reference counter for the medium object.
				unsigned int referenceCounter_ = 0u;

				/// The channel id to which the medium's image data will be streamed.
				IO::Serialization::DataSerializer::ChannelId channelId_ = IO::Serialization::DataSerializer::invalidChannelId();

				/// The timestamp of the last image that has been recorded.
				Timestamp lastTimestamp_;
		};

		/**
		 * Definition of an unordered map mapping devices to sample event subscription objects.
		 */
		using SampleEventSubscriptionMap = std::unordered_map<Device*, Measurement::SampleEventSubscription>;

		/**
		 * Definition of an unordered map mapping devices to tracker object event subscription objects.
		 */
		using TrackerObjectEventSubscriptionMap = std::unordered_map<Device*, Tracker::TrackerObjectEventSubscription>;

		/**
		 * Definition of an unordered map mapping measurements to channel ids.
		 */
		using MeasurementChannelMap = std::unordered_map<const Measurement*, IO::Serialization::DataSerializer::ChannelId>;

		/**
		 * Definition of an unordered set holding measurement objects.
		 */
		using MeasurementSet = std::unordered_set<const Measurement*>;

		/**
		 * Definition of a pair combining a channel id with a data sample for extra data.
		 */
		using ExtraDataSample = std::pair<IO::Serialization::DataSerializer::ChannelId, IO::Serialization::UniqueDataSample>;

		/**
		 * Definition of a queue holding extra data samples.
		 */
		using ExtraDataSampleQueue = std::queue<ExtraDataSample>;

		/**
		 * This class combines relevant data from a sample so that it can be queued for recording.
		 */
		class DeviceSampleData
		{
			public:

				/**
				 * Creates a new object based on a sample.
				 * @param measurement The sender of the sample, must be valid
				 * @param sample The actual sample, must be valid
				 */
				inline DeviceSampleData(const Measurement* measurement, const Measurement::SampleRef& sample);

			public:

				/// The sender of the sample.
				const Measurement* measurement_;

				/// The sample as received from the sender, may be invalid.
				Measurement::SampleRef sample_;

				/// The timestamp when the sample was received, used to determine the playback timestamp.
				Timestamp sampleCreationTimestamp_ = Timestamp(true);
		};

		/**
		 * Definition of a vector holding DeviceSampleData objects.
		 */
		using DeviceSampleDatas = std::vector<DeviceSampleData>;

		/**
		 * Definition of a queue holding DeviceSampleDatas objects.
		 */
		using DeviceSampleDataQueue = std::queue<DeviceSampleDatas>;

		/**
		 * Definition of an unordered map mapping medium objects to medium data objects.
		 */
		using FrameMediumMap = std::unordered_map<const Media::FrameMedium*, FrameMediumData>;

	public:

		/**
		 * Creates a new recorder.
		 */
		SerializerDeviceRecorder();

		/**
		 * Destructs the recorder, an active recording will be stopped.
		 */
		~SerializerDeviceRecorder() override;

		/**
		 * Starts recording with a given filename for the new serialization file.
		 * If the recorder is currently already recording, nothing happens.
		 * @param filename The filename for the new serialization file, must be valid
		 * @return True, if succeeded
		 * @see isStarted(), stop().
		 */
		bool start(const std::string& filename);

		/**
		 * Stops the recording, returns immediately while pending samples are finished to be recorded.
		 * @return True, if succeeded
		 */
		bool stop();

		/**
		 * Returns whether the recorder is currently recording.
		 * @return True, if so
		 */
		inline bool isRecording() const;

		/**
		 * Returns whether the recorder has finished recording.
		 * @return True, if so
		 */
		inline bool hasStopped() const;

		/**
		 * Adds a new frame medium for recording.
		 * A new medium can be added at any time, even if the recorder is currently actively recording.
		 * Each call of addFrameMedium() must be balanced with a call of removeFrameMedium().
		 * @param frameMedium The new frame medium to be added, must be valid
		 * @return True, if succeeded
		 */
		bool addFrameMedium(const Media::FrameMediumRef& frameMedium);

		/**
		 * Removes a frame medium from the recording.
		 * A medium can be removed at any time from the recorder, even if the recorder is currently actively recording.
		 * @param frameMedium The frame medium to be removed, must be valid
		 * @return True, if succeeded
		 */
		bool removeFrameMedium(const Media::FrameMediumRef& frameMedium);

		/**
		 * Adds a new extra channel for custom data recording.
		 * @param sampleType The type of samples that will be recorded on this channel, must be valid
		 * @param channelName The name of the channel, must be valid
		 * @param contentType The content type of the data that will be recorded, must be valid
		 * @return The id of the newly created channel, invalidChannelId() if the channel could not be added
		 */
		[[nodiscard]] IO::Serialization::DataSerializer::ChannelId addExtraChannel(const std::string& sampleType, const std::string& channelName, const std::string& contentType);

		/**
		 * Adds a new extra data sample to be recorded on a specific channel.
		 * @param channelId The id of the channel on which to record the sample, must be valid
		 * @param sample The data sample to be recorded, will be moved
		 * @return True, if succeeded
		 */
		bool addExtraDataSample(const IO::Serialization::DataSerializer::ChannelId channelId, IO::Serialization::UniqueDataSample&& sample);

		/**
		 * Releases this device recorder explicitly before the recorder is disposed.
		 */
		void release();

	protected:

		/**
		 * Disabled copy constructor.
		 */
		SerializerDeviceRecorder(const SerializerDeviceRecorder&) = delete;

		/**
		 * Event function for new or removed devices.
		 * @param device The device which has been added or removed from the system, must be valid
		 * @param added True, if the device has been added; False, if the device has been removed
		 */
		void onDeviceChanged(Device* device, const bool added);

		/**
		 * Event function for new sample events from Measurement objects.
		 * @param sender The sender of the event, must be valid
		 * @param sample The new sample with the measurement information
		 */
		void onMeasurementSample(const Measurement* sender, const Measurement::SampleRef& sample);

		/**
		 * Event function for lost or found tracker object ids.
		 * @param sender The sender of the event, must be valid
		 * @param found True, if the given objects have been found; False, if the given objects have been lost
		 * @param objectIds The ids of all objects, at least one
		 * @param timestamp The timestamp of the event
		 */
		void onTrackerObject(const Tracker* sender, const bool found, const Measurement::ObjectIdSet& objectIds, const Timestamp& timestamp);

		/**
		 * Records the content of a given sample.
		 * @param sender The sender of the sample, must be valid
		 * @param sample The sample holding the information to be recorded
		 * @param sampleCreationTimestamp The timestamp when the sample was received
		 */
		void recordSample(const Measurement* sender, const Measurement::SampleRef& sample, const Timestamp& sampleCreationTimestamp);

		/**
		 * Records the content of a given sensor sample.
		 * @param sender The sender of the sample, must be valid
		 * @param sample The sample holding the information to be recorded
		 * @param sampleCreationTimestamp The timestamp when the sample was received
		 */
		void recordSensorSample(const Measurement* sender, const Measurement::SampleRef& sample, const Timestamp& sampleCreationTimestamp);

		/**
		 * Records the content of a given tracker sample.
		 * @param sender The sender of the sample, must be valid
		 * @param sample The sample holding the information to be recorded
		 * @param sampleCreationTimestamp The timestamp when the sample was received
		 */
		void recordTrackerSample(const Measurement* sender, const Measurement::SampleRef& sample, const Timestamp& sampleCreationTimestamp);

		/**
		 * Records the content of a given 6DOF tracker sample.
		 * @param sender The sender of the sample, must be valid
		 * @param sample The sample holding the information to be recorded
		 * @param sampleCreationTimestamp The timestamp when the sample was received
		 */
		void recordTracker6DOFSample(const Measurement* sender, const Tracker6DOF::Tracker6DOFSampleRef& sample, const Timestamp& sampleCreationTimestamp);

		/**
		 * Records the content of a given GPS tracker sample.
		 * @param sender The sender of the sample, must be valid
		 * @param sample The sample holding the information to be recorded
		 * @param sampleCreationTimestamp The timestamp when the sample was received
		 */
		void recordGPSTrackerSample(const Measurement* sender, const GPSTracker::GPSTrackerSampleRef& sample, const Timestamp& sampleCreationTimestamp);

		/**
		 * Records the content of a given 3DOF acceleration sensor sample.
		 * @param sender The sender of the sample, must be valid
		 * @param sample The acceleration sensor sample, must be valid
		 * @param sampleCreationTimestamp The timestamp when the sample was received
		 */
		void recordAccelerationSensor3DOFSample(const Measurement* sender, const AccelerationSensor3DOF::Acceleration3DOFSampleRef& sample, const Timestamp& sampleCreationTimestamp);

		/**
		 * Records the content of a given 3DOF gyro sensor sample.
		 * @param sender The sender of the sample, must be valid
		 * @param sample The gyro sensor sample, must be valid
		 * @param sampleCreationTimestamp The timestamp when the sample was received
		 */
		void recordGyroSensor3DOFSample(const Measurement* sender, const GyroSensor3DOF::Gyro3DOFSampleRef& sample, const Timestamp& sampleCreationTimestamp);

		/**
		 * Records the content of a given 3DOF orientation tracker sample.
		 * @param sender The sender of the sample, must be valid
		 * @param sample The orientation tracker sample, must be valid
		 * @param sampleCreationTimestamp The timestamp when the sample was received
		 */
		void recordOrientationTracker3DOFSample(const Measurement* sender, const OrientationTracker3DOF::OrientationTracker3DOFSampleRef& sample, const Timestamp& sampleCreationTimestamp);

		/**
		 * Records the content of a given 3DOF gravity tracker sample.
		 * @param sender The sender of the sample, must be valid
		 * @param sample The gravity tracker sample, must be valid
		 * @param sampleCreationTimestamp The timestamp when the sample was received
		 */
		void recordGravityTracker3DOFSample(const Measurement* sender, const GravityTracker3DOF::GravityTracker3DOFSampleRef& sample, const Timestamp& sampleCreationTimestamp);

		/**
		 * Records the content of a given 3DOF position tracker sample.
		 * @param sender The sender of the sample, must be valid
		 * @param sample The position tracker sample, must be valid
		 * @param sampleCreationTimestamp The timestamp when the sample was received
		 */
		void recordPositionTracker3DOFSample(const Measurement* sender, const PositionTracker3DOF::PositionTracker3DOFSampleRef& sample, const Timestamp& sampleCreationTimestamp);

		/**
		 * Records a new frame from a frame medium.
		 * @param frameMediumData The data describing the frame medium
		 * @param frame The actual frame to record, must be valid
		 * @param camera The camera profile of the current frame, if known
		 * @return True, if succeeded
		 */
		bool recordFrame(FrameMediumData& frameMediumData, const Frame& frame, const SharedAnyCamera& camera);

		/**
		 * Moves all not yet queued samples to the queue.
		 */
		void queueUnqueuedSamples();

		/**
		 * The recorder's thread function in which the actual recording is happening.
		 * @see Thread::threadRun().
		 */
		void threadRun() override;

		/**
		 * Disabled copy operator.
		 * @return This object
		 */
		SerializerDeviceRecorder& operator=(const SerializerDeviceRecorder&) = delete;

	protected:

		/// The state of the recorder.
		std::atomic<RecorderState> recorderState_ = RS_IDLE;

		/// True, if the callback event function for changed devices has been registered.
		bool callbackEventDeviceChangedRegistered_ = false;

		/// The map of event subscriptions for sample events.
		SampleEventSubscriptionMap sampleEventSubscriptionMap_;

		/// The map of event subscriptions for object events.
		TrackerObjectEventSubscriptionMap trackerObjectEventSubscriptionMap_;

		/// The map mapping measurements objects to channel ids.
		MeasurementChannelMap measurementChannelMap_;

		/// The map mapping media pointers to media data objects.
		FrameMediumMap frameMediumMap_;

		/// The lock for the 'frameMediumMap_' object.
		Lock frameMediumMapLock_;

		/// The samples which are not yet pushed into the recording queue.
		DeviceSampleDatas unqueuedDeviceSampleDatas_;

		/// The lock for the 'unqueuedDeviceSampleDatas_' object.
		Lock unqueuedDeviceSampleDatasLock_;

		/// The queue with samples that are waiting to be recorded.
		DeviceSampleDataQueue deviceSampleDataQueue_;

		/// The lock for the 'deviceSampleDataQueue_' object.
		Lock deviceSampleDataQueueLock_;

		/// The queue with extra data samples that are waiting to be recorded.
		ExtraDataSampleQueue extraDataSampleQueue_;

		/// The lock for the 'extraDataSampleQueue_' object.
		Lock extraDataSampleQueueLock_;

		/// The lock for the entire recorder.
		Lock recorderLock_;

		/// The set of measurement objects which are not valid anymore.
		MeasurementSet invalidMeasurements_;

		/// The actual serialization output serializer.
		std::unique_ptr<IO::Serialization::FileOutputDataSerializer> outputSerializer_;
};

inline SerializerDeviceRecorder::FrameMediumData::FrameMediumData(const Media::FrameMediumRef& frameMedium) :
	frameMedium_(frameMedium)
{
	// nothing to do here
}

inline SerializerDeviceRecorder::DeviceSampleData::DeviceSampleData(const Measurement* measurement, const Measurement::SampleRef& sample) :
	measurement_(measurement),
	sample_(sample)
{
	// nothing to do here
}

inline bool SerializerDeviceRecorder::isRecording() const
{
	const RecorderState recorderState = recorderState_;

	return recorderState == RS_RECORDING || recorderState == RS_STOPPING;
}

inline bool SerializerDeviceRecorder::hasStopped() const
{
	return recorderState_ == RS_STOPPED;
}

}

}

}

#endif // META_OCEAN_DEVICES_SERIALIZATION_SERIALIZER_DEVICE_RECORDER_H
