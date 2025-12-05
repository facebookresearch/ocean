/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_SERIALIZATION_DEVICE_SERIALIZER_H
#define META_OCEAN_DEVICES_SERIALIZATION_DEVICE_SERIALIZER_H

#include "ocean/devices/serialization/Serialization.h"

#include "ocean/devices/AccelerationSensor3DOF.h"
#include "ocean/devices/GPSTracker.h"
#include "ocean/devices/GravityTracker3DOF.h"
#include "ocean/devices/GyroSensor3DOF.h"
#include "ocean/devices/OrientationTracker3DOF.h"
#include "ocean/devices/PositionTracker3DOF.h"
#include "ocean/devices/Tracker6DOF.h"

#include "ocean/io/Bitstream.h"

#include "ocean/io/serialization/DataSample.h"

namespace Ocean
{

namespace Devices
{

namespace Serialization
{

/**
 * This class provides serialization capabilities for devices.
 * The class contains data sample types for various device types including orientation trackers, acceleration sensors, gyro sensors, gravity trackers, position trackers, 6DOF trackers, and GPS trackers.
 * @ingroup devicesserialization
 */
class OCEAN_DEVICES_EXPORT DeviceSerializer
{
	protected:

		/// The maximal number of measurements that can be stored in a sample.
		static constexpr size_t maximalMeasurements_ = 1000 * 1000;

		/**
		 * This class is the base class for all sample measurements.
		 */
		class SampleMeasurement
		{
			protected:

				/**
				 * Reads measurement data from an input bitstream.
				 * @param inputBitstream The input bitstream from which the measurement will be read
				 * @return True, if succeeded
				 */
				bool readMeasurement(IO::InputBitstream& inputBitstream);

				/**
				 * Writes measurement data to an output bitstream.
				 * @param outputBitstream The output bitstream to which the measurement will be written
				 * @return True, if succeeded
				 */
				bool writeMeasurement(IO::OutputBitstream& outputBitstream) const;

			protected:

				/// The object ids of the measurement units, each id corresponds to a different measurement.
				Indices32 objectIds_;
		};

		/**
		 * This class is the base class for all sample tracker measurements.
		 */
		class SampleTracker : public SampleMeasurement
		{
			protected:

				/**
				 * Reads tracker data from an input bitstream.
				 * @param inputBitstream The input bitstream from which the tracker data will be read
				 * @return True, if succeeded
				 */
				bool readTracker(IO::InputBitstream& inputBitstream);

				/**
				 * Writes tracker data to an output bitstream.
				 * @param outputBitstream The output bitstream to which the tracker data will be written
				 * @return True, if succeeded
				 */
				bool writeTracker(IO::OutputBitstream& outputBitstream) const;

			protected:

				/// The reference system used by the tracker, -1 if not defined.
				int8_t referenceSystem_ = -1;
		};

	public:

		/**
		 * This class implements a data sample for 3DOF orientation tracker measurements.
		 */
		class DataSampleOrientationTracker3DOF :
			public IO::Serialization::DataSample,
			public SampleTracker
		{
			public:

				/**
				 * Creates a new 3DOF orientation tracker data sample.
				 */
				DataSampleOrientationTracker3DOF() = default;

				/**
				 * Creates a new 3DOF orientation tracker data sample from a tracker sample.
				 * @param sample The orientation tracker 3DOF sample to serialize
				 * @param sampleCreationTimestamp The timestamp when the sample was created, used to determine playback timestamp
				 */
				DataSampleOrientationTracker3DOF(const OrientationTracker3DOF::OrientationTracker3DOFSample& sample, const Timestamp sampleCreationTimestamp = Timestamp(true));

				/**
				 * Reads the sample from an input bitstream.
				 * @param inputBitstream The input bitstream from which the sample will be read
				 * @return True, if succeeded
				 * @see writeSample().
				 */
				bool readSample(IO::InputBitstream& inputBitstream) override;

				/**
				 * Writes the sample to an output bitstream.
				 * @param outputBitstream The output bitstream to which the sample will be written
				 * @return True, if succeeded
				 * @see readSample().
				 */
				bool writeSample(IO::OutputBitstream& outputBitstream) const override;

				/**
				 * Returns the type of the sample.
				 * @return The sample type
				 * @see sampleType().
				 */
				inline const std::string& type() const override;

				/**
				 * Returns the orientation measurements.
				 * @return The orientation measurements as quaternions
				 */
				inline const QuaternionsF& orientations() const;

				/**
				 * Returns the object ids.
				 * @return The object ids, each id corresponds to a different orientation measurement
				 */
				inline const Indices32& objectIds() const;

				/**
				 * Returns the reference system.
				 * @return The reference system used by the tracker, -1 if not defined
				 */
				inline int8_t referenceSystem() const;

				/**
				 * Returns the static sample type.
				 * @return The sample type
				 */
				static inline const std::string& sampleType();

				/**
				 * Factory function for creating a DataSampleOrientationTracker3DOF.
				 * This function can be used with InputDataSerializer::registerFactoryFunction().
				 * @param sampleType The sample type (unused, but required by the factory function signature)
				 * @return A new DataSampleOrientationTracker3DOF instance
				 */
				static IO::Serialization::UniqueDataSample createSample(const std::string& sampleType);

			protected:

				/// The orientation measurements as quaternions.
				QuaternionsF orientations_;
		};

		/**
		 * This class implements a data sample for 3DOF acceleration sensor measurements.
		 */
		class DataSampleAccelerationSensor3DOF :
			public IO::Serialization::DataSample,
			public SampleMeasurement
		{
			public:

				/**
				 * Creates a new 3DOF acceleration sensor data sample.
				 */
				DataSampleAccelerationSensor3DOF() = default;

				/**
				 * Creates a new 3DOF acceleration sensor data sample from a sensor sample.
				 * @param sample The acceleration sensor 3DOF sample to serialize
				 * @param sampleCreationTimestamp The timestamp when the sample was created, used to determine playback timestamp
				 */
				DataSampleAccelerationSensor3DOF(const AccelerationSensor3DOF::Acceleration3DOFSample& sample, const Timestamp sampleCreationTimestamp = Timestamp(true));

				/**
				 * Reads the sample from an input bitstream.
				 * @param inputBitstream The input bitstream from which the sample will be read
				 * @return True, if succeeded
				 * @see writeSample().
				 */
				bool readSample(IO::InputBitstream& inputBitstream) override;

				/**
				 * Writes the sample to an output bitstream.
				 * @param outputBitstream The output bitstream to which the sample will be written
				 * @return True, if succeeded
				 * @see readSample().
				 */
				bool writeSample(IO::OutputBitstream& outputBitstream) const override;

				/**
				 * Returns the type of the sample.
				 * @return The sample type
				 * @see sampleType().
				 */
				inline const std::string& type() const override;

				/**
				 * Returns the acceleration measurements.
				 * @return The acceleration measurements, each in [m / s^2]
				 */
				inline const VectorsF3& measurements() const;

				/**
				 * Returns the object ids.
				 * @return The object ids, each id corresponds to a different acceleration measurement
				 */
				inline const Indices32& objectIds() const;

				/**
				 * Returns the static sample type.
				 * @return The sample type
				 */
				static inline const std::string& sampleType();

				/**
				 * Factory function for creating a DataSampleAccelerationSensor3DOF.
				 * This function can be used with InputDataSerializer::registerFactoryFunction().
				 * @param sampleType The sample type (unused, but required by the factory function signature)
				 * @return A new DataSampleAccelerationSensor3DOF instance
				 */
				static IO::Serialization::UniqueDataSample createSample(const std::string& sampleType);

			protected:

				/// The acceleration measurements, each in [m / s^2].
				VectorsF3 measurements_;
		};

		/**
		 * This class implements a data sample for 3DOF gyro sensor measurements.
		 */
		class DataSampleGyroSensor3DOF :
			public IO::Serialization::DataSample,
			public SampleMeasurement
		{
			public:

				/**
				 * Creates a new 3DOF gyro sensor data sample.
				 */
				DataSampleGyroSensor3DOF() = default;

				/**
				 * Creates a new 3DOF gyro sensor data sample from a sensor sample.
				 * @param sample The gyro sensor 3DOF sample to serialize
				 * @param sampleCreationTimestamp The timestamp when the sample was created, used to determine playback timestamp
				 */
				DataSampleGyroSensor3DOF(const GyroSensor3DOF::Gyro3DOFSample& sample, const Timestamp sampleCreationTimestamp = Timestamp(true));

				/**
				 * Reads the sample from an input bitstream.
				 * @param inputBitstream The input bitstream from which the sample will be read
				 * @return True, if succeeded
				 * @see writeSample().
				 */
				bool readSample(IO::InputBitstream& inputBitstream) override;

				/**
				 * Writes the sample to an output bitstream.
				 * @param outputBitstream The output bitstream to which the sample will be written
				 * @return True, if succeeded
				 * @see readSample().
				 */
				bool writeSample(IO::OutputBitstream& outputBitstream) const override;

				/**
				 * Returns the type of the sample.
				 * @return The sample type
				 * @see sampleType().
				 */
				inline const std::string& type() const override;

				/**
				 * Returns the gyro measurements.
				 * @return The gyro measurements
				 */
				inline const VectorsF3& measurements() const;

				/**
				 * Returns the object ids.
				 * @return The object ids, each id corresponds to a different gyro measurement
				 */
				inline const Indices32& objectIds() const;

				/**
				 * Returns the static sample type.
				 * @return The sample type
				 */
				static inline const std::string& sampleType();

				/**
				 * Factory function for creating a DataSampleGyroSensor3DOF.
				 * This function can be used with InputDataSerializer::registerFactoryFunction().
				 * @param sampleType The sample type (unused, but required by the factory function signature)
				 * @return A new DataSampleGyroSensor3DOF instance
				 */
				static IO::Serialization::UniqueDataSample createSample(const std::string& sampleType);

			protected:

				/// The gyro measurements.
				VectorsF3 measurements_;
		};

		/**
		 * This class implements a data sample for 3DOF gravity tracker measurements.
		 */
		class DataSampleGravityTracker3DOF :
			public IO::Serialization::DataSample,
			public SampleTracker
		{
			public:

				/**
				 * Creates a new 3DOF gravity tracker data sample.
				 */
				DataSampleGravityTracker3DOF() = default;

				/**
				 * Creates a new 3DOF gravity tracker data sample from a tracker sample.
				 * @param sample The gravity tracker 3DOF sample to serialize
				 * @param sampleCreationTimestamp The timestamp when the sample was created, used to determine playback timestamp
				 */
				DataSampleGravityTracker3DOF(const GravityTracker3DOF::GravityTracker3DOFSample& sample, const Timestamp sampleCreationTimestamp = Timestamp(true));

				/**
				 * Reads the sample from an input bitstream.
				 * @param inputBitstream The input bitstream from which the sample will be read
				 * @return True, if succeeded
				 * @see writeSample().
				 */
				bool readSample(IO::InputBitstream& inputBitstream) override;

				/**
				 * Writes the sample to an output bitstream.
				 * @param outputBitstream The output bitstream to which the sample will be written
				 * @return True, if succeeded
				 * @see readSample().
				 */
				bool writeSample(IO::OutputBitstream& outputBitstream) const override;

				/**
				 * Returns the type of the sample.
				 * @return The sample type
				 * @see sampleType().
				 */
				inline const std::string& type() const override;

				/**
				 * Returns the gravity measurements.
				 * @return The gravity measurements
				 */
				inline const VectorsF3& gravities() const;

				/**
				 * Returns the object ids.
				 * @return The object ids, each id corresponds to a different gravity measurement
				 */
				inline const Indices32& objectIds() const;

				/**
				 * Returns the reference system.
				 * @return The reference system used by the tracker, -1 if not defined
				 */
				inline int8_t referenceSystem() const;

				/**
				 * Returns the static sample type.
				 * @return The sample type
				 */
				static inline const std::string& sampleType();

				/**
				 * Factory function for creating a DataSampleGravityTracker3DOF.
				 * This function can be used with InputDataSerializer::registerFactoryFunction().
				 * @param sampleType The sample type (unused, but required by the factory function signature)
				 * @return A new DataSampleGravityTracker3DOF instance
				 */
				static IO::Serialization::UniqueDataSample createSample(const std::string& sampleType);

			protected:

				/// The gravity measurements.
				VectorsF3 gravities_;
		};

		/**
		 * This class implements a data sample for 3DOF position tracker measurements.
		 */
		class DataSamplePositionTracker3DOF :
			public IO::Serialization::DataSample,
			public SampleTracker
		{
			public:

				/**
				 * Creates a new 3DOF position tracker data sample.
				 */
				DataSamplePositionTracker3DOF() = default;

				/**
				 * Creates a new 3DOF position tracker data sample from a tracker sample.
				 * @param sample The position tracker 3DOF sample to serialize
				 * @param sampleCreationTimestamp The timestamp when the sample was created, used to determine playback timestamp
				 */
				DataSamplePositionTracker3DOF(const PositionTracker3DOF::PositionTracker3DOFSample& sample, const Timestamp sampleCreationTimestamp = Timestamp(true));

				/**
				 * Reads the sample from an input bitstream.
				 * @param inputBitstream The input bitstream from which the sample will be read
				 * @return True, if succeeded
				 * @see writeSample().
				 */
				bool readSample(IO::InputBitstream& inputBitstream) override;

				/**
				 * Writes the sample to an output bitstream.
				 * @param outputBitstream The output bitstream to which the sample will be written
				 * @return True, if succeeded
				 * @see readSample().
				 */
				bool writeSample(IO::OutputBitstream& outputBitstream) const override;

				/**
				 * Returns the type of the sample.
				 * @return The sample type
				 * @see sampleType().
				 */
				inline const std::string& type() const override;

				/**
				 * Returns the position measurements.
				 * @return The position measurements in meter
				 */
				inline const VectorsF3& positions() const;

				/**
				 * Returns the object ids.
				 * @return The object ids, each id corresponds to a different position measurement
				 */
				inline const Indices32& objectIds() const;

				/**
				 * Returns the reference system.
				 * @return The reference system used by the tracker, -1 if not defined
				 */
				inline int8_t referenceSystem() const;

				/**
				 * Returns the static sample type.
				 * @return The sample type
				 */
				static inline const std::string& sampleType();

				/**
				 * Factory function for creating a DataSamplePositionTracker3DOF.
				 * This function can be used with InputDataSerializer::registerFactoryFunction().
				 * @param sampleType The sample type (unused, but required by the factory function signature)
				 * @return A new DataSamplePositionTracker3DOF instance
				 */
				static IO::Serialization::UniqueDataSample createSample(const std::string& sampleType);

			protected:

				/// The position measurements in meter.
				VectorsF3 positions_;
		};

		/**
		 * This class implements a data sample for 6DOF tracker measurements.
		 */
		class DataSampleTracker6DOF :
			public IO::Serialization::DataSample,
			public SampleTracker
		{
			public:

				/**
				 * Creates a new 6DOF tracker data sample.
				 */
				DataSampleTracker6DOF() = default;

				/**
				 * Creates a new 6DOF tracker data sample from a tracker sample.
				 * @param sample The tracker 6DOF sample to serialize
				 * @param sampleCreationTimestamp The timestamp when the sample was created, used to determine playback timestamp
				 */
				DataSampleTracker6DOF(const Tracker6DOF::Tracker6DOFSample& sample, const Timestamp sampleCreationTimestamp = Timestamp(true));

				/**
				 * Reads the sample from an input bitstream.
				 * @param inputBitstream The input bitstream from which the sample will be read
				 * @return True, if succeeded
				 * @see writeSample().
				 */
				bool readSample(IO::InputBitstream& inputBitstream) override;

				/**
				 * Writes the sample to an output bitstream.
				 * @param outputBitstream The output bitstream to which the sample will be written
				 * @return True, if succeeded
				 * @see readSample().
				 */
				bool writeSample(IO::OutputBitstream& outputBitstream) const override;

				/**
				 * Returns the type of the sample.
				 * @return The sample type
				 * @see sampleType().
				 */
				inline const std::string& type() const override;

				/**
				 * Returns the orientation measurements.
				 * @return The orientation measurements as quaternions
				 */
				inline const QuaternionsF& orientations() const;

				/**
				 * Returns the position measurements.
				 * @return The position measurements in meter
				 */
				inline const VectorsF3& positions() const;

				/**
				 * Returns the object ids.
				 * @return The object ids, each id corresponds to a different 6DOF measurement
				 */
				inline const Indices32& objectIds() const;

				/**
				 * Returns the reference system.
				 * @return The reference system used by the tracker, -1 if not defined
				 */
				inline int8_t referenceSystem() const;

				/**
				 * Returns the static sample type.
				 * @return The sample type
				 */
				static inline const std::string& sampleType();

				/**
				 * Factory function for creating a DataSampleTracker6DOF.
				 * This function can be used with InputDataSerializer::registerFactoryFunction().
				 * @param sampleType The sample type (unused, but required by the factory function signature)
				 * @return A new DataSampleTracker6DOF instance
				 */
				static IO::Serialization::UniqueDataSample createSample(const std::string& sampleType);

			protected:

				/// The orientation measurements as quaternions.
				QuaternionsF orientations_;

				/// The position measurements in meter.
				VectorsF3 positions_;
		};

		/**
		 * This class implements a data sample for GPS tracker measurements.
		 */
		class DataSampleGPSTracker :
			public IO::Serialization::DataSample,
			public SampleTracker
		{
			public:

				/**
				 * This class implements a GPS location.
				 */
				class Location
				{
					public:

						/**
						 * Creates a new GPS location with default values.
						 */
						Location() = default;

						/**
						 * Creates a new GPS location from a GPS tracker location.
						 * @param location The GPS tracker location
						 */
						Location(const GPSTracker::Location& location);

						/**
						 * Reads a GPS location from an input bitstream.
						 * @param inputBitstream The input bitstream from which the location will be read
						 * @return True, if succeeded
						 */
						bool read(IO::InputBitstream& inputBitstream);

						/**
						 * Writes a GPS location to an output bitstream.
						 * @param outputBitstream The output bitstream to which the location will be written
						 * @return True, if succeeded
						 */
						bool write(IO::OutputBitstream& outputBitstream) const;

					public:

						/// The latitude in degrees, range [-90, 90], NumericD::minValue() if unknown.
						double latitude_ = NumericD::minValue();

						/// The longitude in degrees, range [-180, 180], NumericD::minValue() if unknown.
						double longitude_ = NumericD::minValue();

						/// The altitude in meters, NumericF::minValue() if unknown.
						float altitude_ = NumericF::minValue();

						/// The direction (heading) in degrees, range [0, 360), -1 if unknown.
						float direction_ = -1.0f;

						/// The speed in meters per second, -1 if unknown.
						float speed_ = -1.0f;

						/// The horizontal accuracy in meters, -1 if unknown.
						float accuracy_ = -1.0f;

						/// The altitude accuracy in meters, -1 if unknown.
						float altitudeAccuracy_ = -1.0f;

						/// The direction accuracy in degrees, -1 if unknown.
						float directionAccuracy_ = -1.0f;

						/// The speed accuracy in meters per second, -1 if unknown.
						float speedAccuracy_ = -1.0f;
				};

				/**
				 * Definition of a vector holding GPS locations.
				 */
				using Locations = std::vector<Location>;

			public:

				/**
				 * Creates a new GPS tracker data sample.
				 */
				DataSampleGPSTracker() = default;

				/**
				 * Creates a new GPS tracker data sample from a tracker sample.
				 * @param sample The GPS tracker sample to serialize
				 * @param sampleCreationTimestamp The timestamp when the sample was created, used to determine playback timestamp
				 */
				DataSampleGPSTracker(const GPSTracker::GPSTrackerSample& sample, const Timestamp sampleCreationTimestamp = Timestamp(true));

				/**
				 * Reads the sample from an input bitstream.
				 * @param inputBitstream The input bitstream from which the sample will be read
				 * @return True, if succeeded
				 * @see writeSample().
				 */
				bool readSample(IO::InputBitstream& inputBitstream) override;

				/**
				 * Writes the sample to an output bitstream.
				 * @param outputBitstream The output bitstream to which the sample will be written
				 * @return True, if succeeded
				 * @see readSample().
				 */
				bool writeSample(IO::OutputBitstream& outputBitstream) const override;

				/**
				 * Returns the type of the sample.
				 * @return The sample type
				 * @see sampleType().
				 */
				inline const std::string& type() const override;

				/**
				 * Returns the GPS locations.
				 * @return The GPS locations
				 */
				inline const Locations& locations() const;

				/**
				 * Returns the object ids.
				 * @return The object ids, each id corresponds to a different GPS location
				 */
				inline const Indices32& objectIds() const;

				/**
				 * Returns the reference system.
				 * @return The reference system used by the tracker, -1 if not defined
				 */
				inline int8_t referenceSystem() const;

				/**
				 * Returns the static sample type.
				 * @return The sample type
				 */
				static inline const std::string& sampleType();

				/**
				 * Factory function for creating a DataSampleGPSTracker.
				 * This function can be used with InputDataSerializer::registerFactoryFunction().
				 * @param sampleType The sample type (unused, but required by the factory function signature)
				 * @return A new DataSampleGPSTracker instance
				 */
				static IO::Serialization::UniqueDataSample createSample(const std::string& sampleType);

			protected:

				/// The GPS locations.
				Locations locations_;
		};
};

inline const std::string& DeviceSerializer::DataSampleOrientationTracker3DOF::type() const
{
	return sampleType();
}

inline const QuaternionsF& DeviceSerializer::DataSampleOrientationTracker3DOF::orientations() const
{
	return orientations_;
}

inline const Indices32& DeviceSerializer::DataSampleOrientationTracker3DOF::objectIds() const
{
	return objectIds_;
}

inline int8_t DeviceSerializer::DataSampleOrientationTracker3DOF::referenceSystem() const
{
	return referenceSystem_;
}

inline const std::string& DeviceSerializer::DataSampleOrientationTracker3DOF::sampleType()
{
	static const std::string typeName = "ocean/devices/datasampleorientationtracker3dof";
	return typeName;
}

inline const std::string& DeviceSerializer::DataSampleAccelerationSensor3DOF::type() const
{
	return sampleType();
}

inline const VectorsF3& DeviceSerializer::DataSampleAccelerationSensor3DOF::measurements() const
{
	return measurements_;
}

inline const Indices32& DeviceSerializer::DataSampleAccelerationSensor3DOF::objectIds() const
{
	return objectIds_;
}

inline const std::string& DeviceSerializer::DataSampleAccelerationSensor3DOF::sampleType()
{
	static const std::string typeName = "ocean/devices/datasampleaccelerationsensor3dof";
	return typeName;
}

inline const std::string& DeviceSerializer::DataSampleGyroSensor3DOF::type() const
{
	return sampleType();
}

inline const VectorsF3& DeviceSerializer::DataSampleGyroSensor3DOF::measurements() const
{
	return measurements_;
}

inline const Indices32& DeviceSerializer::DataSampleGyroSensor3DOF::objectIds() const
{
	return objectIds_;
}

inline const std::string& DeviceSerializer::DataSampleGyroSensor3DOF::sampleType()
{
	static const std::string typeName = "ocean/devices/datasamplegyrosensor3dof";
	return typeName;
}

inline const std::string& DeviceSerializer::DataSampleGravityTracker3DOF::type() const
{
	return sampleType();
}

inline const VectorsF3& DeviceSerializer::DataSampleGravityTracker3DOF::gravities() const
{
	return gravities_;
}

inline const Indices32& DeviceSerializer::DataSampleGravityTracker3DOF::objectIds() const
{
	return objectIds_;
}

inline int8_t DeviceSerializer::DataSampleGravityTracker3DOF::referenceSystem() const
{
	return referenceSystem_;
}

inline const std::string& DeviceSerializer::DataSampleGravityTracker3DOF::sampleType()
{
	static const std::string typeName = "ocean/devices/datasamplegravitytracker3dof";
	return typeName;
}

inline const std::string& DeviceSerializer::DataSamplePositionTracker3DOF::type() const
{
	return sampleType();
}

inline const VectorsF3& DeviceSerializer::DataSamplePositionTracker3DOF::positions() const
{
	return positions_;
}

inline const Indices32& DeviceSerializer::DataSamplePositionTracker3DOF::objectIds() const
{
	return objectIds_;
}

inline int8_t DeviceSerializer::DataSamplePositionTracker3DOF::referenceSystem() const
{
	return referenceSystem_;
}

inline const std::string& DeviceSerializer::DataSamplePositionTracker3DOF::sampleType()
{
	static const std::string typeName = "ocean/devices/datasamplepositiontracker3dof";
	return typeName;
}

inline const std::string& DeviceSerializer::DataSampleTracker6DOF::type() const
{
	return sampleType();
}

inline const QuaternionsF& DeviceSerializer::DataSampleTracker6DOF::orientations() const
{
	return orientations_;
}

inline const VectorsF3& DeviceSerializer::DataSampleTracker6DOF::positions() const
{
	return positions_;
}

inline const Indices32& DeviceSerializer::DataSampleTracker6DOF::objectIds() const
{
	return objectIds_;
}

inline int8_t DeviceSerializer::DataSampleTracker6DOF::referenceSystem() const
{
	return referenceSystem_;
}

inline const std::string& DeviceSerializer::DataSampleTracker6DOF::sampleType()
{
	static const std::string typeName = "ocean/devices/datasampletracker6dof";
	return typeName;
}

inline const std::string& DeviceSerializer::DataSampleGPSTracker::type() const
{
	return sampleType();
}

inline const DeviceSerializer::DataSampleGPSTracker::Locations& DeviceSerializer::DataSampleGPSTracker::locations() const
{
	return locations_;
}

inline const Indices32& DeviceSerializer::DataSampleGPSTracker::objectIds() const
{
	return objectIds_;
}

inline int8_t DeviceSerializer::DataSampleGPSTracker::referenceSystem() const
{
	return referenceSystem_;
}

inline const std::string& DeviceSerializer::DataSampleGPSTracker::sampleType()
{
	static const std::string typeName = "ocean/devices/datasamplegpstracker";
	return typeName;
}



}

}

}

#endif // META_OCEAN_DEVICES_SERIALIZATION_DEVICE_SERIALIZER_H
