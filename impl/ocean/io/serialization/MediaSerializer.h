/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_IO_SERIALIZATION_MEDIA_SERIALIZER_H
#define META_OCEAN_IO_SERIALIZATION_MEDIA_SERIALIZER_H

#include "ocean/io/serialization/Serialization.h"
#include "ocean/io/serialization/DataSerializer.h"
#include "ocean/io/serialization/DataSample.h"

#include "ocean/base/Frame.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/HomogenousMatrix4.h"

namespace Ocean
{

namespace IO
{

namespace Serialization
{

/**
 * This class implements media serialization functionalities.
 * @ingroup ioserialization
 */
class OCEAN_IO_SERIALIZATION_EXPORT MediaSerializer
{
	public:

		/**
		 * This class implements a sample for Ocean::Frame objects.
		 *
		 * The class supports two primary use cases:
		 * 1. Automatic encoding/decoding:
		 *    A Frame can be provided and will be automatically encoded using the Ocean Image library with a preferred encoding type (e.g., "ocn" or "jpg").
		 *    The frame() function will decode the data on-demand.
		 * 2. Custom encoded data: An already encoded buffer can be provided directly (e.g., an mp4 frame with custom imageType).
		 *    In this case, the user is responsible for decoding the data. The buffer() function provides direct access to the encoded data for custom decoding.
		 */
		class DataSampleFrame : public DataSample
		{
			public:

				/**
				 * Definition of a vector holding encoded frame data.
				 */
				using Buffer = std::vector<uint8_t>;

			public:

				/**
				 * Default constructor creating an invalid sample.
				 */
				DataSampleFrame() = default;

				/**
				 * Creates a new DataSampleFrame object from a frame.
				 * The frame will be encoded immediately.
				 * @param frame The frame to be serialized, must be valid
				 * @param imageType The image type to be used for encoding
				 * @param camera Optional camera model to be associated with the frame
				 * @param device_T_camera Optional transformation between camera and device
				 * @param sampleCreationTimestamp The timestamp when the sample was created, this timestamp is not serialized and is only used to automatically determine the playback timestamp
				 */
				explicit DataSampleFrame(const Frame& frame, const std::string& imageType = "ocn", SharedAnyCamera camera = SharedAnyCamera(), const HomogenousMatrixD4& device_T_camera = HomogenousMatrixD4(false), const Timestamp& sampleCreationTimestamp = Timestamp(true));

				/**
				 * Creates a new DataSampleFrame object from an encoded buffer.
				 * @param buffer The encoded frame buffer, will be moved
				 * @param imageType The image type of the encoded buffer
				 * @param dataTimestamp The timestamp of the data
				 * @param camera Optional camera model to be associated with the frame
				 * @param device_T_camera Optional transformation between camera and device
				 * @param sampleCreationTimestamp The timestamp when the sample was created, this timestamp is not serialized and is only used to automatically determine the playback timestamp
				 */
				explicit DataSampleFrame(Buffer&& buffer, const std::string& imageType, const DataTimestamp& dataTimestamp, SharedAnyCamera camera = SharedAnyCamera(), const HomogenousMatrixD4& device_T_camera = HomogenousMatrixD4(false), const Timestamp& sampleCreationTimestamp = Timestamp(true));

				/**
				 * Reads the sample from an input bitstream.
				 * @param inputBitstream The input bitstream from which the sample will be read
				 * @return True, if succeeded
				 */
				bool readSample(InputBitstream& inputBitstream) override;

				/**
				 * Writes the sample to an output bitstream.
				 * @param outputBitstream The output bitstream to which the sample will be written
				 * @return True, if succeeded
				 */
				bool writeSample(OutputBitstream& outputBitstream) const override;

				/**
				 * Returns the type of the sample.
				 * @return The sample type
				 */
				inline const std::string& type() const override;

				/**
				 * Returns the frame stored in this sample.
				 * The frame will be decoded from the buffer on-demand.
				 * @param camera Optional output parameter to receive the parsed camera model, nullptr if not of interest
				 * @return The frame, invalid if the sample is invalid or decoding fails
				 */
				Frame frame(SharedAnyCamera* camera = nullptr) const;

				/**
				 * Returns the camera model associated with this sample.
				 * The camera will be parsed from the cameraModel_ JSON string on-demand.
				 * @return The camera model, nullptr if no camera model is set or parsing fails
				 */
				SharedAnyCamera camera() const;

				/**
				 * Returns the encoded buffer.
				 * @return The buffer storing the encoded frame data
				 */
				inline const Buffer& buffer() const;

				/**
				 * Returns the image type used for encoding/decoding.
				 * @return The image type
				 */
				inline const std::string& imageType() const;

				/**
				 * Returns the optional camera model associated with the frame.
				 * @return The camera model (JSON string), empty if not set
				 */
				inline const std::string& cameraModel() const;

				/**
				 * Returns the device_T_camera transformation.
				 * @return The transformation between camera and device, invalid if not set
				 */
				inline const HomogenousMatrixD4& device_T_camera() const;

				/**
				 * Returns whether this sample is valid.
				 * @return True, if the buffer is not empty and the image type is not empty
				 */
				inline bool isValid() const;

				/**
				 * Returns the type of the sample.
				 * @return The sample type
				 */
				static const std::string& sampleType();

			protected:

				/// The encoded frame buffer.
				Buffer buffer_;

				/// The image type used for encoding/decoding.
				std::string imageType_;

				/// The optional camera model associated with the frame (JSON string from CameraCalibrationManager).
				std::string cameraModel_;

				/// The optional transformation between camera and device.
				HomogenousMatrixD4 device_T_camera_ = HomogenousMatrixD4(false);
		};
};

inline const std::string& MediaSerializer::DataSampleFrame::type() const
{
	return sampleType();
}

inline const MediaSerializer::DataSampleFrame::Buffer& MediaSerializer::DataSampleFrame::buffer() const
{
	return buffer_;
}

inline const std::string& MediaSerializer::DataSampleFrame::imageType() const
{
	return imageType_;
}

inline const std::string& MediaSerializer::DataSampleFrame::cameraModel() const
{
	return cameraModel_;
}

inline const HomogenousMatrixD4& MediaSerializer::DataSampleFrame::device_T_camera() const
{
	return device_T_camera_;
}

inline bool MediaSerializer::DataSampleFrame::isValid() const
{
	return !buffer_.empty() && !imageType_.empty();
}

}

}

}

#endif // META_OCEAN_IO_SERIALIZATION_MEDIA_SERIALIZER_H
