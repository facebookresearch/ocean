/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_IO_SERIALIZATION_DATA_SAMPLE_H
#define META_OCEAN_IO_SERIALIZATION_DATA_SAMPLE_H

#include "ocean/io/serialization/Serialization.h"
#include "ocean/io/serialization/DataTimestamp.h"

#include "ocean/base/Timestamp.h"

#include "ocean/io/Bitstream.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace IO
{

namespace Serialization
{

// Forward declaration.
class DataSample;

/**
 * Definition of a unique pointer holding a DataSample.
 * @see DataSample.
 * @ingroup ioserialization
 */
using UniqueDataSample = std::unique_ptr<DataSample>;

/**
 * This class implements a base class for data samples.
 * A data sample represents a single unit of data that can be serialized and deserialized through bitstreams.<br>
 * Each sample contains two timestamps: a data timestamp representing when the data was originally captured (e.g., the camera frame timestamp), and a playback timestamp representing the relative time offset from when serialization started.<br>
 * Derived classes must implement the type() function to identify the sample type and should override readSample()/writeSample() to serialize their specific data.<br>
 * The class also provides helper functions for reading/writing common mathematical types like HomogenousMatrix4, Quaternion, and Vector3 with float precision.
 * @ingroup ioserialization
 */
class OCEAN_IO_SERIALIZATION_EXPORT DataSample
{
	friend class OutputDataSerializer;

	public:

		/**
		 * Creates a new data sample.
		 */
		DataSample() = default;

		/**
		 * Creates a new data sample with a playback timestamp and data timestamp.
		 * @param dataTimestamp The data timestamp of the sample
		 * @param sampleCreationTimestamp The timestamp when the sample was created, this timestamp is not serialized and is only used to automatically determine the playback timestamp
		 */
		explicit inline DataSample(const DataTimestamp& dataTimestamp, const Timestamp sampleCreationTimestamp = Timestamp(true));

		/**
		 * Destructs a data sample.
		 */
		virtual ~DataSample() = default;

		/**
		 * Returns the relative timestamp of this sample at which the sample will be played back, in relation to the moment in time when the serialization started.
		 * @return The playback timestamp, in seconds, with range [0, infinity)
		 */
		inline double playbackTimestamp() const;

		/**
		 * Returns the timestamp of the data, e.g., the timestamp of the camera frame when it was captured.
		 * @return The data timestamp
		 */
		inline const DataTimestamp& dataTimestamp() const;

		/**
		 * Reads the sample from an input bitstream.
		 * @param inputBitstream The input bitstream from which the sample will be read
		 * @return True, if succeeded
		 */
		virtual bool readSample(InputBitstream& inputBitstream);

		/**
		 * Writes the sample to an output bitstream.
		 * @param outputBitstream The output bitstream to which the sample will be written
		 * @return True, if succeeded
		 */
		virtual bool writeSample(OutputBitstream& outputBitstream) const;

		/**
		 * Returns the type of the sample, mainly describing the content of the sample, not the source of the sample.
		 * @return The sample type
		 */
		virtual const std::string& type() const = 0;

		/**
		 * Configures the playback timestamp based on when serialization started.
		 * This calculates the relative playback time as the difference between when this sample was created and when serialization began.
		 * @param serializationStartTimestamp The timestamp when serialization started
		 */
		inline void configurePlaybackTimestamp(const Timestamp& serializationStartTimestamp);

	protected:

		/**
		 * Reads a 4x4 homogenous matrix with float precision from an input bitstream.
		 * @param inputBitstream The input bitstream from which the matrix will be read
		 * @param homogenousMatrix The resulting homogenous matrix
		 * @return True, if succeeded
		 */
		static bool readHomogenousMatrix4F(InputBitstream& inputBitstream, HomogenousMatrixF4& homogenousMatrix);

		/**
		 * Writes a 4x4 homogenous matrix with float precision to an output bitstream.
		 * @param outputBitstream The output bitstream to which the matrix will be written
		 * @param homogenousMatrix The homogenous matrix to write
		 * @return True, if succeeded
		 */
		static bool writeHomogenousMatrix4F(OutputBitstream& outputBitstream, const HomogenousMatrixF4& homogenousMatrix);

		/**
		 * Reads a quaternion with float precision from an input bitstream.
		 * @param inputBitstream The input bitstream from which the quaternion will be read
		 * @param quaternion The resulting quaternion
		 * @return True, if succeeded
		 */
		static bool readQuaternionF(InputBitstream& inputBitstream, QuaternionF& quaternion);

		/**
		 * Writes a quaternion with float precision to an output bitstream.
		 * @param outputBitstream The output bitstream to which the quaternion will be written
		 * @param quaternion The quaternion to write
		 * @return True, if succeeded
		 */
		static bool writeQuaternionF(OutputBitstream& outputBitstream, const QuaternionF& quaternion);

		/**
		 * Reads a 3D vector with float precision from an input bitstream.
		 * @param inputBitstream The input bitstream from which the vector will be read
		 * @param vector The resulting vector
		 * @return True, if succeeded
		 */
		static bool readVectorF3(InputBitstream& inputBitstream, VectorF3& vector);

		/**
		 * Writes a 3D vector with float precision to an output bitstream.
		 * @param outputBitstream The output bitstream to which the vector will be written
		 * @param vector The vector to write
		 * @return True, if succeeded
		 */
		static bool writeVectorF3(OutputBitstream& outputBitstream, const VectorF3& vector);

	protected:

		/// The relative timestamp of this sample at which the sample will be played back, in relation to the moment in time when the serialization started, in seconds.
		double playbackTimestamp_ = NumericD::minValue();

		/// The timestamp of the data, e.g., the timestamp of the camera frame when it was captured.
		DataTimestamp dataTimestamp_;

	private:

		/// The unix timestamp when the sample was created, this timestamp is not serialized and is only used to automatically determine the playback timestamp.
		Timestamp sampleCreationTimestamp_ = Timestamp(true);
};

inline DataSample::DataSample(const DataTimestamp& dataTimestamp, const Timestamp sampleCreationTimestamp) :
	dataTimestamp_(dataTimestamp),
	sampleCreationTimestamp_(sampleCreationTimestamp)
{
	ocean_assert(sampleCreationTimestamp_.isValid());
}

inline double DataSample::playbackTimestamp() const
{
	return playbackTimestamp_;
}

inline const DataTimestamp& DataSample::dataTimestamp() const
{
	return dataTimestamp_;
}

inline void DataSample::configurePlaybackTimestamp(const Timestamp& serializationStartTimestamp)
{
	ocean_assert(serializationStartTimestamp.isValid());
	ocean_assert(sampleCreationTimestamp_.isValid());

	playbackTimestamp_ = double(sampleCreationTimestamp_ - serializationStartTimestamp);
}

inline bool DataSample::readHomogenousMatrix4F(InputBitstream& inputBitstream, HomogenousMatrixF4& homogenousMatrix)
{
	return inputBitstream.read(homogenousMatrix(), sizeof(float) * 16);
}

inline bool DataSample::writeHomogenousMatrix4F(OutputBitstream& outputBitstream, const HomogenousMatrixF4& homogenousMatrix)
{
	return outputBitstream.write(homogenousMatrix(), sizeof(float) * 16);
}

inline bool DataSample::readQuaternionF(InputBitstream& inputBitstream, QuaternionF& quaternion)
{
	return inputBitstream.read(quaternion(), sizeof(float) * 4);
}

inline bool DataSample::writeQuaternionF(OutputBitstream& outputBitstream, const QuaternionF& quaternion)
{
	return outputBitstream.write(quaternion(), sizeof(float) * 4);
}

inline bool DataSample::readVectorF3(InputBitstream& inputBitstream, VectorF3& vector)
{
	return inputBitstream.read(vector(), sizeof(float) * 3);
}

inline bool DataSample::writeVectorF3(OutputBitstream& outputBitstream, const VectorF3& vector)
{
	return outputBitstream.write(vector(), sizeof(float) * 3);
}

}

}

}

#endif // META_OCEAN_IO_SERIALIZATION_DATA_SAMPLE_H
