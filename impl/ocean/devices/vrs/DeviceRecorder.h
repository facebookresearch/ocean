// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_DEVICES_VRS_DEVICE_RECORDER_H
#define META_OCEAN_DEVICES_VRS_DEVICE_RECORDER_H

#include "ocean/devices/vrs/VRS.h"

#include "ocean/base/Thread.h"

#include "ocean/devices/Device.h"
#include "ocean/devices/GPSTracker.h"
#include "ocean/devices/Measurement.h"
#include "ocean/devices/SceneTracker6DOF.h"
#include "ocean/devices/Tracker6DOF.h"

#include "ocean/io/Bitstream.h"

#include "ocean/io/vrs/FlexibleRecorder.h"

#include "ocean/media/FrameMedium.h"

#include <queue>

namespace Ocean
{

namespace Devices
{

namespace VRS
{

/**
 * This class implements a VRS recorder for devices and also media objects.
 * @ingroup devicesvrs
 */
class OCEAN_DEVICES_VRS_EXPORT DeviceRecorder : protected Thread
{
	public:

		/// Re-using the RecordableCamera of VRS::Recorder
		using RecordableCamera = IO::VRS::RecordableCamera;

		/**
		 * This is the base recordable for all trackers.
		 */
		class OCEAN_DEVICES_VRS_EXPORT RecordableTracker : public IO::VRS::Recordable
		{
			protected:

				/// Version of the configuration layout.
				static const uint32_t configurationLayoutVersion_ = 1u;

				/**
				 * This struct defines the configuration layout of poses stream.
				 */
				struct ConfigurationLayoutTracker : public vrs::AutoDataLayout
				{
					/// The name of the device.
					vrs::DataPieceString deviceNameValue{"device_name"};

					/// The major type of the device.
					vrs::DataPieceString deviceTypeMajorValue{"device_type_major"};

					/// The minor type of the device.
					vrs::DataPieceString deviceTypeMinorValue{"device_type_minor"};

					/// The layout's end object.
					vrs::AutoDataLayoutEnd endLayout;
				};

			public:

				/**
				 * Sets the type of this tracker.
				 * @param deviceName The name of the tracker, must be valid
				 * @param deviceTypeMajor The major type of the tracker, must be valid
				 * @param deviceTypeMinor The minor type of the tracker, must be valid
				 * @param recordTimestamp The opional timestamp of the configuration record, invalid to use the current time
				 * @return True, if succeeded
				 */
				bool setDevice(const std::string& deviceName, const std::string& deviceTypeMajor, const std::string& deviceTypeMinor, const Timestamp& recordTimestamp);

				/**
				 * Returns whether this recodable is valid and configured.
				 * @return True, if succeeded
				 */
				bool isValid() const override;

				/**
				 * Writes metadata to a bitstream.
				 * @param metadata The metadata to write
				 * @param bitstream The bitstream receiving the metadata
				 * @return True, if succeeded
				 */
				static bool writeMetadataToBitstream(const Measurement::Metadata& metadata, IO::OutputBitstream& bitstream);

				/**
				 * Reads metadata rom a bitstream.
				 * @param bitstream The bitstream from which the metadata will be read
				 * @param metadata The resulting metadata
				 * @return True, if succeeded
				 */
				static bool readMetadataFromBitstream(IO::InputBitstream& bitstream, Measurement::Metadata& metadata);

			protected:

				/**
				 * Creates a new recordable.
				 * @param recordableTypeId The type id of the recordable
				 * @param flavor The optional flavor of the recordable
				 * @param vrsRecordStartingTimestamp Optional VRS recording timestamp. If invalid, the current time will be used to create the configuration and state record. Range: (0, infinity)
				 */
				RecordableTracker(const vrs::RecordableTypeId recordableTypeId, const std::string& flavor, const Timestamp& vrsRecordStartingTimestamp);

				/**
				 * Configuration records describe how the device recorded is configured/setup.
				 * @see Recordable::createConfigurationRecord().
				 */
				const vrs::Record* createConfigurationRecord() override;

				/**
				 * State records describe the internal state of the device, if it's stateful.
				 * @see Recordable::createStateRecord();
				 */
				const vrs::Record* createStateRecord() override;

			protected:

				/// The name of the device sending the tracker events.
				std::string deviceName_;

				/// The major type of the device sending the tracker events.
				std::string deviceTypeMajor_;

				/// The minor type of the device sending the tracker events.
				std::string deviceTypeMinor_;

				/// The configuration layout.
				ConfigurationLayoutTracker configurationLayout_;

				/// The unique bitstream tag for metadata.
				static constexpr unsigned long long metadataTag_ = IO::Tag::string2tag("_OCNMDA_");

				/// The unique bitstream tag for bool values.
				static constexpr unsigned long long metadataTypeTagBool_ = IO::Tag::string2tag("__BOOL__");

				/// The unique bitstream tag for int32 values.
				static constexpr unsigned long long metadataTypeTagInt32_ = IO::Tag::string2tag("__INT32_");

				/// The unique bitstream tag for int64 values.
				static constexpr unsigned long long metadataTypeTagInt64_ = IO::Tag::string2tag("__INT64_");

				/// The unique bitstream tag for float32 values.
				static constexpr unsigned long long metadataTypeTagFloat32_ = IO::Tag::string2tag("_FLOAT32");

				/// The unique bitstream tag for float64 values.
				static constexpr unsigned long long metadataTypeTagFloat64_ = IO::Tag::string2tag("_FLOAT64");

				/// The unique bitstream tag for string values.
				static constexpr unsigned long long metadataTypeTagString_ = IO::Tag::string2tag("_STRING_");

				/// The unique bitstream tag for buffer values.
				static constexpr unsigned long long metadataTypeTagBuffer_ = IO::Tag::string2tag("_BUFFER_");
		};

		/**
		 * This class is implements a recordable for 6DOF tracker samples or object tracking events.
		 */
		class OCEAN_DEVICES_VRS_EXPORT RecordableTracker6DOFSample : public RecordableTracker
		{
			public:

				/// Version of the data layout.
				static const uint32_t sampleDataLayoutVersion_ = 1u;

				/**
				 * This struct defines the data layout of poses stream.
				 */
				struct DataLayoutTracker6DOFSample : public vrs::AutoDataLayout
				{
					/// The description of tracking objects.
					vrs::DataPieceStringMap<unsigned int> objectDescriptionMap{"object_description_map"};

					/// The ids of found objects.
					vrs::DataPieceVector<unsigned int> foundObjects{"found_objects"};

					/// The ids of lost objects.
					vrs::DataPieceVector<unsigned int> lostObjects{"lost_objects"};

					/// The reference system of the sample.
					vrs::DataPieceString referenceSystem{"reference_system"};

					/// The 6-DOF transformations of the sample, one for each object id.
					vrs::DataPieceVector<vrs::Matrix4Dd> homogenousMatrix4{"homogenous_matrices_4"};

					/// The object ids of the sample, one for each transformation.
					vrs::DataPieceVector<unsigned int> objectIds{"object_ids"};

					/// The timestamp of the sample or object tacking events.
					vrs::DataPieceValue<double> timestamp{"timestamp"};

					/// The encoded metadata of the sample.
					vrs::DataPieceString metadata{"metadata"};

					/// The layout's end object.
					vrs::AutoDataLayoutEnd endLayout;
				};

				/**
				 * Definition of a map mapping object descriptions to object ids.
				 */
				typedef std::map<std::string, unsigned int> DescriptionMap;

			public:

				/**
				 * Creates a new recordable.
				 * @param recordableTypeId The type id of the recordable
				 * @param flavor The optional flavor of the recordable
				 * @param vrsRecordStartingTimestamp Optional VRS recording timestamp. If invalid, the current time will be used to create the configuration and state record. Range: (0, infinity)
				 */
				RecordableTracker6DOFSample(const vrs::RecordableTypeId recordableTypeId = defaultRecordableTypeId(), const std::string& flavor = defaultFlavor(), const Timestamp& vrsRecordStartingTimestamp = Timestamp(false));

				/**
				 * Adds new tracker samples or object events to this stream.
				 * @param sender The sender of the event, must be valid
				 * @param foundObjects The ids of all found objects
				 * @param lostObjects The ids of all lost objects
				 * @param sample The tracker sample, must be invalid
				 * @param timestamp The timestamp of the pose
				 * @param recordTimestamp Optional VRS recording timestamp. If not specified, the current time will be used. Range: (-infinity, infinity)
				 */
				void addData(const Measurement* sender, const Measurement::ObjectIdSet& foundObjects, const Measurement::ObjectIdSet& lostObjects, const Tracker6DOF::Tracker6DOFSampleRef& sample, const Timestamp& timestamp, const double* recordTimestamp = nullptr);

				/**
				 * Adds new tracker samples or object events to this stream.
				 * @param descriptionMap The map with descriptions of all objects, will be moved
				 * @param foundObjects The ids of all found objects
				 * @param lostObjects The ids of all lost objects
				 * @param sample The tracker sample, must be invalid
				 * @param timestamp The timestamp of the pose
				 * @param recordTimestamp Optional VRS recording timestamp. If not specified, the current time will be used. Range: (-infinity, infinity)
				 */
				void addData(DescriptionMap&& descriptionMap, const Measurement::ObjectIdSet& foundObjects, const Measurement::ObjectIdSet& lostObjects, const Tracker6DOF::Tracker6DOFSampleRef& sample, const Timestamp& timestamp, const double* recordTimestamp = nullptr);

				/**
				 * Returns the default flavor of this recoding.
				 * @return The recording's default flavor
				 */
				static std::string defaultFlavor();

				/**
				 * Returns the default recordable type id of this recording.
				 * @return The recording's default id
				 */
				static constexpr vrs::RecordableTypeId defaultRecordableTypeId();

			protected:

				/// The data layout for sample events.
				DataLayoutTracker6DOFSample dataSampleLayout_;
		};

		/**
		 * This class is implements a recordable for GPS tracker samples or object tracking events.
		 */
		class OCEAN_DEVICES_VRS_EXPORT RecordableGPSTrackerSample : public RecordableTracker
		{
			public:

				/// Version of the data layout.
				static const uint32_t sampleDataLayoutVersion_ = 1u;

				/**
				 * This struct defines the data layout of gps stream.
				 */
				struct DataLayoutGPSTrackerSample : public vrs::AutoDataLayout
				{
					/// The description of tracking objects.
					vrs::DataPieceStringMap<unsigned int> objectDescriptionMap{"object_description_map"};

					/// The ids of found objects.
					vrs::DataPieceVector<unsigned int> foundObjects{"found_objects"};

					/// The ids of lost objects.
					vrs::DataPieceVector<unsigned int> lostObjects{"lost_objects"};

					/// The reference system of the sample.
					vrs::DataPieceString referenceSystem{"reference_system"};

					/// The longitude coordinates, in degree, one for each object id.
					vrs::DataPieceVector<double> longitudes{"longitudes"};

					/// The latitude coordinates, in degree, one for each object id.
					vrs::DataPieceVector<double> latitudes{"latitudes"};

					/// The altitudes, in meter, one for each object id.
					vrs::DataPieceVector<float> altitudes{"altitudes"};

					/// The directions, in degree, one for each object id.
					vrs::DataPieceVector<float> directions{"directions"};

					/// The speeds, in meter per second, one for each object id.
					vrs::DataPieceVector<float> speeds{"speeds"};

					/// The horizontal accuracies, in meter, one for each object id.
					vrs::DataPieceVector<float> accuracies{"accuracies"};

					/// The vertical accuracies, in meter, one for each object id.
					vrs::DataPieceVector<float> altitudeAccuracies{"altitudeAccuracies"};

					/// The direction accuracies, in degree, one for each object id.
					vrs::DataPieceVector<float> directionAccuracies{"directionAccuracies"};

					/// The speed accuracies, in meter per second, one for each object id.
					vrs::DataPieceVector<float> speedAccuracies{"speedAccuracies"};

					/// The object ids of the sample, one for each transformation.
					vrs::DataPieceVector<unsigned int> objectIds{"object_ids"};

					/// The timestamp of the sample or object tacking events.
					vrs::DataPieceValue<double> timestamp{"timestamp"};

					/// The encoded metadata of the sample.
					vrs::DataPieceString metadata{"metadata"};

					/// The layout's end object.
					vrs::AutoDataLayoutEnd endLayout;
				};

				/**
				 * Creates a new recordable.
				 * @param recordableTypeId The type id of the recordable
				 * @param flavor The optional flavor of the recordable
				 * @param vrsRecordStartingTimestamp Optional VRS recording timestamp. If invalid, the current time will be used to create the configuration and state record. Range: (0, infinity)
				 */
				RecordableGPSTrackerSample(const vrs::RecordableTypeId recordableTypeId = defaultRecordableTypeId(), const std::string& flavor = defaultFlavor(), const Timestamp& vrsRecordStartingTimestamp = Timestamp(false));

				/**
				 * Adds new GPS samples or object events to this stream.
				 * @param sender The sender of the event, must be valid
				 * @param foundObjects The ids of all found objects
				 * @param lostObjects The ids of all lost objects
				 * @param sample The tracker sample, must be invalid
				 * @param timestamp The timestamp of the pose
				 * @param recordTimestamp Optional VRS recording timestamp. If not specified, the current time will be used. Range: (-infinity, infinity)
				 */
				void addData(const Measurement* sender, const Measurement::ObjectIdSet& foundObjects, const Measurement::ObjectIdSet& lostObjects, const GPSTracker::GPSTrackerSampleRef& sample, const Timestamp& timestamp, const double* recordTimestamp = nullptr);

				/**
				 * Returns the default flavor of this recoding.
				 * @return The recording's default flavor
				 */
				static std::string defaultFlavor();

				/**
				 * Returns the default recordable type id of this recording.
				 * @return The recording's default id
				 */
				static constexpr vrs::RecordableTypeId defaultRecordableTypeId();

			protected:

				/// The data layout for sample events.
				DataLayoutGPSTrackerSample dataSampleLayout_;
		};

		/**
		 * This class is implements a recordable for scene tracker samples or object tracking events.
		 */
		class OCEAN_DEVICES_VRS_EXPORT RecordableSceneTracker6DOFSample : public RecordableTracker
		{
			public:

				/// Version of the data layout.
				static const uint32_t sampleDataLayoutVersion_ = 1u;

				/**
				 * This struct defines the data layout of gps stream.
				 */
				struct DataLayoutSceneTracker6DOFSample : public vrs::AutoDataLayout
				{
					/// The description of tracking objects.
					vrs::DataPieceStringMap<unsigned int> objectDescriptionMap{"object_description_map"};

					/// The ids of found objects.
					vrs::DataPieceVector<unsigned int> foundObjects{"found_objects"};

					/// The ids of lost objects.
					vrs::DataPieceVector<unsigned int> lostObjects{"lost_objects"};

					/// The reference system of the sample.
					vrs::DataPieceString referenceSystem{"reference_system"};

					/// The 6-DOF transformations of the sample, one for each object id.
					vrs::DataPieceVector<vrs::Matrix4Dd> homogenousMatrix4{"homogenous_matrices_4"};

					/// The object ids of the sample, one for each transformation.
					vrs::DataPieceVector<unsigned int> objectIds{"object_ids"};

					/// The encoded scene elements system of the sample.
					vrs::DataPieceString sceneElements{"scene_elements"};

					/// The timestamp of the sample or object tacking events.
					vrs::DataPieceValue<double> timestamp{"timestamp"};

					/// The encoded metadata of the sample.
					vrs::DataPieceString metadata{"metadata"};

					/// The layout's end object.
					vrs::AutoDataLayoutEnd endLayout;
				};

				/**
				 * Creates a new recordable.
				 * @param recordableTypeId The type id of the recordable
				 * @param flavor The optional flavor of the recordable
				 * @param vrsRecordStartingTimestamp Optional VRS recording timestamp. If invalid, the current time will be used to create the configuration and state record. Range: (0, infinity)
				 */
				RecordableSceneTracker6DOFSample(const vrs::RecordableTypeId recordableTypeId = defaultRecordableTypeId(), const std::string& flavor = defaultFlavor(), const Timestamp& vrsRecordStartingTimestamp = Timestamp(false));

				/**
				 * Adds new GPS samples or object events to this stream.
				 * @param sender The sender of the event, must be valid
				 * @param foundObjects The ids of all found objects
				 * @param lostObjects The ids of all lost objects
				 * @param sample The tracker sample, must be invalid
				 * @param timestamp The timestamp of the pose
				 * @param recordTimestamp Optional VRS recording timestamp. If not specified, the current time will be used. Range: (-infinity, infinity)
				 */
				void addData(const Measurement* sender, const Measurement::ObjectIdSet& foundObjects, const Measurement::ObjectIdSet& lostObjects, const SceneTracker6DOF::SceneTracker6DOFSampleRef& sample, const Timestamp& timestamp, const double* recordTimestamp = nullptr);

				/**
				 * Returns the default flavor of this recoding.
				 * @return The recording's default flavor
				 */
				static std::string defaultFlavor();

				/**
				 * Returns the default recordable type id of this recording.
				 * @return The recording's default id
				 */
				static constexpr vrs::RecordableTypeId defaultRecordableTypeId();

				/**
				 * Writes scene elements to a bitstream.
				 * @param sceneElements The scene elements to write
				 * @param bitstream The bitstream receiving the scene elements
				 * @return True, if succeeded
				 */
				static bool writeSceneElementsToBitstream(const SceneTracker6DOF::SharedSceneElements& sceneElements, IO::OutputBitstream& bitstream);

				/**
				 * Reads scene elements from a bitstream.
				 * @param bitstream The bitstream from which the scene elements will be read
				 * @param sceneElements The resulting scene elements
				 * @return True, if succeeded
				 */
				static bool readSceneElementsFromBitstream(IO::InputBitstream& bitstream, SceneTracker6DOF::SharedSceneElements& sceneElements);

			protected:

				/**
				 * Writes object points to a bitstream.
				 * @param sceneElementObjectPoints The object points to write
				 * @param bitstream The bitstream receiving the scene element
				 * @return True, if succeeded
				 */
				static bool writeObjectPointsToBitstream(const SceneTracker6DOF::SceneElementObjectPoints& sceneElementObjectPoints, IO::OutputBitstream& bitstream);

				/**
				 * Writes feature correspondences to a bitstream.
				 * @param sceneElementFeatureCorrespondences The feature correspondences to write
				 * @param bitstream The bitstream receiving the scene element
				 * @return True, if succeeded
				 */
				static bool writeFeatureCorrespondencesToBitstream(const SceneTracker6DOF::SceneElementFeatureCorrespondences& sceneElementFeatureCorrespondences, IO::OutputBitstream& bitstream);

				/**
				 * Writes planes to a bitstream.
				 * @param sceneElementPlanes The planes to write
				 * @param bitstream The bitstream receiving the scene element
				 * @return True, if succeeded
				 */
				static bool writePlanesToBitstream(const SceneTracker6DOF::SceneElementPlanes& sceneElementPlanes, IO::OutputBitstream& bitstream);

				/**
				 * Writes meshes to a bitstream.
				 * @param sceneElementMeshes The meshes to write
				 * @param bitstream The bitstream receiving the scene element
				 * @return True, if succeeded
				 */
				static bool writeMeshsToBitstream(const SceneTracker6DOF::SceneElementMeshes& sceneElementMeshes, IO::OutputBitstream& bitstream);

				/**
				 * Writes depth to a bitstream.
				 * @param sceneElementDepth The depth to write
				 * @param bitstream The bitstream receiving the scene element
				 * @return True, if succeeded
				 */
				static bool writeDepthToBitstream(const SceneTracker6DOF::SceneElementDepth& sceneElementDepth, IO::OutputBitstream& bitstream);

				/**
				 * Reads object points from a bitstream.
				 * @param bitstream The bitstream from which the scene elements will be read
				 * @param sceneElement The resulting scene element
				 * @return True, if succeeded
				 */
				static bool readObjectPointsFromBitstream(IO::InputBitstream& bitstream, SceneTracker6DOF::SharedSceneElement& sceneElement);

				/**
				 * Reads feature correspondences from a bitstream.
				 * @param bitstream The bitstream from which the scene elements will be read
				 * @param sceneElement The resulting scene element
				 * @return True, if succeeded
				 */
				static bool readFeatureCorrespondencesFromBitstream(IO::InputBitstream& bitstream, SceneTracker6DOF::SharedSceneElement& sceneElement);

				/**
				 * Reads planes from a bitstream.
				 * @param bitstream The bitstream from which the scene elements will be read
				 * @param sceneElement The resulting scene element
				 * @return True, if succeeded
				 */
				static bool readPlanesFromBitstream(IO::InputBitstream& bitstream, SceneTracker6DOF::SharedSceneElement& sceneElement);

				/**
				 * Reads meshes from a bitstream.
				 * @param bitstream The bitstream from which the scene elements will be read
				 * @param sceneElement The resulting scene element
				 * @return True, if succeeded
				 */
				static bool readMeshsFromBitstream(IO::InputBitstream& bitstream, SceneTracker6DOF::SharedSceneElement& sceneElement);

				/**
				 * Reads depth from a bitstream.
				 * @param bitstream The bitstream from which the scene elements will be read
				 * @param sceneElement The resulting scene element
				 * @return True, if succeeded
				 */
				static bool readDepthFromBitstream(IO::InputBitstream& bitstream, SceneTracker6DOF::SharedSceneElement& sceneElement);

				/**
				 * Writes 2D vectors to a bitstream with float32 precision.
				 * @param vectors The vectors to write
				 * @param bitstream The bitstream receiving the vectors
				 * @return True, if succeeded
				 */
				static bool writeVectorsF2(const Vectors2& vectors, IO::OutputBitstream& bitstream);

				/**
				 * Writes 3D vectors to a bitstream with float32 precision.
				 * @param vectors The vectors to write
				 * @param bitstream The bitstream receiving the vectors
				 * @return True, if succeeded
				 */
				static bool writeVectorsF3(const Vectors3& vectors, IO::OutputBitstream& bitstream);

				/**
				 * Reads 2D vectors from a bitstream with float32 precision.
				 * @param bitstream The bitstream from which the vectors will be read
				 * @param size The number of vectors
				 * @param vectors The resulting vectors
				 * @return True, if succeeded
				 */
				static bool readVectorsF2(IO::InputBitstream& bitstream, const size_t size, Vectors2& vectors);

				/**
				 * Reads 3D vectors from a bitstream with float32 precision.
				 * @param bitstream The bitstream from which the vectors will be read
				 * @param size The number of vectors
				 * @param vectors The resulting vectors
				 * @return True, if succeeded
				 */
				static bool readVectorsF3(IO::InputBitstream& bitstream, const size_t size, Vectors3& vectors);

			protected:

				/// The data layout for sample events.
				DataLayoutSceneTracker6DOFSample dataSampleLayout_;

				/// The unique bitstream tag for scene elements.
				static constexpr unsigned long long sceneElementsTag_ = IO::Tag::string2tag("_OCNSES_");

				/// The unique bitstream tag for an empty scene element (a pure 6-DOF pose).
				static constexpr unsigned long long sceneElementEmptyTag_ = IO::Tag::string2tag("_OCNETY_");

				/// The unique bitstream tag for object points.
				static constexpr unsigned long long sceneElementObjectPointsTag_ = IO::Tag::string2tag("_OCNOPS_");

				/// The unique bitstream tag for feature correspondences.
				static constexpr unsigned long long sceneElementFeatureCorrespondencesTag_ = IO::Tag::string2tag("_OCNFCS_");

				/// The unique bitstream tag for planes.
				static constexpr unsigned long long sceneElementPlanesTag_ = IO::Tag::string2tag("_OCNPLS_");

				/// The unique bitstream tag for meshes.
				static constexpr unsigned long long sceneElementMeshesTag_ = IO::Tag::string2tag("_OCNMES_");

				/// The unique bitstream tag for depth.
				static constexpr unsigned long long sceneElementDepthTag_ = IO::Tag::string2tag("_OCNDPH_");
		};

	protected:

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

				/// The reference counter for the medium object.
				unsigned int referenceCounter_;

				/// The index of the recordable to which the medium's image data will be streamed.
				unsigned int recordableIndex_;

				/// The timestamp of the last image that has been recorded.
				Timestamp lastTimestamp_;
		};

		/**
		 * Definition of an unordered map mapping devices to sample event subscription objects.
		 */
		typedef std::unordered_map<Device*, Measurement::SampleEventSubscription> SampleEventSubscriptionMap;

		/**
		 * Definition of an unordered map mapping devices to tracker object event subscription objects.
		 */
		typedef std::unordered_map<Device*, Tracker::TrackerObjectEventSubscription> TrackerObjectEventSubscriptionMap;

		/**
		 * Definition of an unordered map mapping measurements to indices of recordables.
		 */
		typedef std::unordered_map<const Measurement*, unsigned int> MeasurementRecordableMap;

		/**
		 * Definition of an unordered set holding measurement objects.
		 */
		typedef std::unordered_set<const Measurement*> MeasurementSet;

		/**
		 * This class combines relevant data from a sample or object tracking events so that it can be queued for recording.
		 */
		class SampleData
		{
			public:

				/**
				 * Creates a new object based on a sample.
				 * @param measurement The sender of the sample, must be valid
				 * @param sample The actual sample, must be valid
				 * @param timestamp The timestamp of the sample
				 */
				inline SampleData(const Measurement* measurement, const Measurement::SampleRef& sample, const Timestamp& timestamp);

				/**
				 * Creates a new object based on object tracking events.
				 * @param measurement The sender of the sampel, must be valid
				 * @param foundObjects The ids of all found objects
				 * @param lostObjects The ids of all lost objects
				 * @param timestamp The timestamp of the sample
				 */
				inline SampleData(const Measurement* measurement, const Measurement::ObjectIdSet& foundObjects, const Measurement::ObjectIdSet& lostObjects, const Timestamp& timestamp);

			public:

				/// The sender of the sample or object tracking events.
				const Measurement* measurement_;

				/// The sample as received from the sender, may be invalid.
				Measurement::SampleRef sample_;

				/// The ids of all found objects as received from the sender.
				Measurement::ObjectIdSet foundObjects_;

				/// The ids of all lost objects as received from the sender.
				Measurement::ObjectIdSet lostObjects_;

				/// The timestamp of the sample or object tracking events.
				Timestamp timestamp_;
		};

		/**
		 * Definition of a vector holding SampleData objects.
		 */
		typedef std::vector<SampleData> SampleDatas;

		/**
		 * Definition of a queue holding SampleDatas objects.
		 */
		typedef std::queue<SampleDatas> SampleDataQueue;

		/**
		 * Definition of an unordered map mapping medium objects to medium data objects.
		 */
		typedef std::unordered_map<const Media::FrameMedium*, FrameMediumData> FrameMediumMap;

	public:

		/**
		 * Creates a new recorder.
		 */
		DeviceRecorder();

		/**
		 * Destructs the recorder, an active recording will be stopped.
		 */
		~DeviceRecorder() override;

		/**
		 * Returns whether the recorder is currently recording.
		 * @return True, if so
		 */
		inline bool isStarted() const;

		/**
		 * Starts VRS recording with a given filename for the new VRS file.
		 * If the recorder is currently already recording, nothing happens.
		 * @return True, if succeeded
		 * @see isStarted(), stop().
		 */
		bool start(const std::string& filename);

		/**
		 * Stops the currently active VRS recording.
		 * @return True, if succeeded
		 */
		bool stop();

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
		 * @param frameMedium The new frame medium to be added, must be valid
		 * @return True, if succeeded
		 */
		bool removeFrameMedium(const Media::FrameMediumRef& frameMedium);

		/**
		 * Releases this device recorder before explicitly before the recorder is disposed.
		 */
		void release();

	protected:

		/**
		 * Disabled copy constructor.
		 */
		DeviceRecorder(const DeviceRecorder&) = delete;

		/**
		 * Event function for new or removed devices.
		 * @param device The device which has been added or removed from the system, must be valid
		 * @param added True, if the device has been added; False, if the device has been removed
		 */
		void onDeviceChanged(Device* device, bool added);

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
		 * Records the content of a given sample or/and the lost/found tracking objects.
		 * @param sender The sender of the sample, must be valid
		 * @param sample The sample holding the information to be recorded, can be invalid if lost/found objects are recorded only
		 * @param foundObjects The ids of all found objects, can be empty
		 * @param lostObjects The ids of all lost objects, can be empty
		 * @param timestamp The timestamp of the sample or the lost/found objects
		 */
		void recordSample(const Measurement* sender, const Measurement::SampleRef& sample, const Measurement::ObjectIdSet& foundObjects, const Measurement::ObjectIdSet& lostObjects, const Timestamp& timestamp);

		/**
		 * Records the content of a given sensor sample or/and the lost/found tracking objects.
		 * @param sender The sender of the sample, must be valid
		 * @param sample The sample holding the information to be recorded, can be invalid if lost/found objects are recorded only
		 * @param foundObjects The ids of all found objects, can be empty
		 * @param lostObjects The ids of all lost objects, can be empty
		 * @param timestamp The timestamp of the sample or the lost/found objects
		 */
		void recordSensorSample(const Measurement* sender, const Measurement::SampleRef& sample, const Measurement::ObjectIdSet& foundObjects, const Measurement::ObjectIdSet& lostObjects, const Timestamp& timestamp);

		/**
		 * Records the content of a given tracker sample or/and the lost/found tracking objects.
		 * @param sender The sender of the sample, must be valid
		 * @param sample The sample holding the information to be recorded, can be invalid if lost/found objects are recorded only
		 * @param foundObjects The ids of all found objects, can be empty
		 * @param lostObjects The ids of all lost objects, can be empty
		 * @param timestamp The timestamp of the sample or the lost/found objects
		 */
		void recordTrackerSample(const Measurement* sender, const Measurement::SampleRef& sample, const Measurement::ObjectIdSet& foundObjects, const Measurement::ObjectIdSet& lostObjects, const Timestamp& timestamp);

		/**
		 * Records the content of a given 6DOF tracker sample or/and the lost/found tracking objects.
		 * @param sender The sender of the sample, must be valid
		 * @param sample The sample holding the information to be recorded, can be invalid if lost/found objects are recorded only
		 * @param foundObjects The ids of all found objects, can be empty
		 * @param lostObjects The ids of all lost objects, can be empty
		 * @param timestamp The timestamp of the sample or the lost/found objects
		 */
		void recordTracker6DOFSample(const Measurement* sender, const Tracker6DOF::Tracker6DOFSampleRef& sample, const Measurement::ObjectIdSet& foundObjects, const Measurement::ObjectIdSet& lostObjects, const Timestamp& timestamp);

		/**
		 * Records the content of a given 6DOF scene tracker sample or/and the lost/found tracking objects.
		 * @param sender The sender of the sample, must be valid
		 * @param sample The sample holding the information to be recorded, can be invalid if lost/found objects are recorded only
		 * @param foundObjects The ids of all found objects, can be empty
		 * @param lostObjects The ids of all lost objects, can be empty
		 * @param timestamp The timestamp of the sample or the lost/found objects
		 */
		void recordSceneTracker6DOFSample(const Measurement* sender, const SceneTracker6DOF::SceneTracker6DOFSampleRef& sample, const Measurement::ObjectIdSet& foundObjects, const Measurement::ObjectIdSet& lostObjects, const Timestamp& timestamp);

		/**
		 * Records the content of a given GPS tracker sample or/and the lost/found tracking objects.
		 * @param sender The sender of the sample, must be valid
		 * @param sample The sample holding the information to be recorded, can be invalid if lost/found objects are recorded only
		 * @param foundObjects The ids of all found objects, can be empty
		 * @param lostObjects The ids of all lost objects, can be empty
		 * @param timestamp The timestamp of the sample or the lost/found objects
		 */
		void recordGPSTrackerSample(const Measurement* sender, const GPSTracker::GPSTrackerSampleRef& sample, const Measurement::ObjectIdSet& foundObjects, const Measurement::ObjectIdSet& lostObjects, const Timestamp& timestamp);

		/**
		 * Records a new frame from a frame medium.
		 * @param frameMediumData The data describing the frame medium
		 * @param frame The actual frame to record, must be valid
		 * @param camera The camera profile of the current frame, if known
		 * @return True, if succeeded
		 */
		bool recordFrame(FrameMediumData& frameMediumData, const Frame& frame, const SharedAnyCameraD& camera);

		/**
		 * Waits until all currently pending samples have been recorded.
		 */
		void recordPendingSamples();

		/**
		 * The recorder's thread function in which the actual recording is happening.
		 * @see Thread::threadRun().
		 */
		void threadRun() override;

		/**
		 * Disabled copy operator.
		 * @return This object
		 */
		DeviceRecorder& operator=(const DeviceRecorder&) = delete;

	protected:

		/// True, if the recorder is currently recording.
		std::atomic<bool> isStarted_ = false;

		/// True, if the callback event function for changed devices has been registered.
		bool callbackEventDeviceChangedRegistered_ = false;

		/// The map of event subscriptions for sample events.
		SampleEventSubscriptionMap sampleEventSubscriptionMap_;

		/// The map of event subscriptions for object events.
		TrackerObjectEventSubscriptionMap trackerObjectEventSubscriptionMap_;

		/// The map mapping measurements objects to recordables.
		MeasurementRecordableMap measurementRecordableMap_;

		/// The map mapping media pointers to media data objects.
		FrameMediumMap frameMediumMap_;

		/// The lock for the 'frameMediumMap_' object.
		Lock frameMediumMapLock_;

		/// The samples which are not yet pushed into the recording queue.
		SampleDatas unqueuedSampleDatas_;

		/// The lock for the 'unqueuedSampleDatas_' object.
		Lock unqueuedSampleDatasLock_;

		/// The queue with samples that are waiting to be recorded.
		SampleDataQueue sampleDataQueue_;

		/// The lock for the 'sampleDataQueue_' object.
		Lock sampleDataQueueLock_;

		/// The lock for the entire recorder.
		Lock recorderLock_;

		/// The set of measurement objects which are not valid anymore.
		MeasurementSet invalidMeasurements_;

		/// The actual VRS recorder.
		std::shared_ptr<IO::VRS::FlexibleRecorder> flexibleRecorder_;
};

inline DeviceRecorder::FrameMediumData::FrameMediumData(const Media::FrameMediumRef& frameMedium) :
	frameMedium_(frameMedium),
	referenceCounter_(0u),
	recordableIndex_((unsigned int)(-1)),
	lastTimestamp_(false)
{
	// nothing to do here
}

inline DeviceRecorder::SampleData::SampleData(const Measurement* measurement, const Measurement::SampleRef& sample, const Timestamp& timestamp) :
	measurement_(measurement),
	sample_(sample),
	timestamp_(timestamp)
{
	// nothing to do here
}

inline DeviceRecorder::SampleData::SampleData(const Measurement* measurement, const Measurement::ObjectIdSet& foundObjects, const Measurement::ObjectIdSet& lostObjects, const Timestamp& timestamp) :
	measurement_(measurement),
	foundObjects_(foundObjects),
	lostObjects_(lostObjects),
	timestamp_(timestamp)
{
	// nothing to do here
}

constexpr vrs::RecordableTypeId DeviceRecorder::RecordableTracker6DOFSample::defaultRecordableTypeId()
{
	return vrs::RecordableTypeId::PoseRecordableClass;
}

constexpr vrs::RecordableTypeId DeviceRecorder::RecordableGPSTrackerSample::defaultRecordableTypeId()
{
	return vrs::RecordableTypeId::GpsRecordableClass;
}

constexpr vrs::RecordableTypeId DeviceRecorder::RecordableSceneTracker6DOFSample::defaultRecordableTypeId()
{
	return vrs::RecordableTypeId::PoseRecordableClass;
}

inline bool DeviceRecorder::isStarted() const
{
	return bool(isStarted_);
}

}

}

}

#endif // META_OCEAN_DEVICES_VRS_DEVICE_RECORDER_H
