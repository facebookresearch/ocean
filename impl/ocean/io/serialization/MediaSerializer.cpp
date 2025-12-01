/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/io/serialization/MediaSerializer.h"

#include "ocean/io/CameraCalibrationManager.h"
#include "ocean/io/image/Image.h"

namespace Ocean
{

namespace IO
{

namespace Serialization
{

MediaSerializer::DataSampleFrame::DataSampleFrame(const Frame& frame, const std::string& imageType, SharedAnyCamera camera, const HomogenousMatrixD4& device_T_camera, const Timestamp& sampleCreationTimestamp) :
	DataSample(DataTimestamp(double(frame.timestamp())), sampleCreationTimestamp),
	device_T_camera_(device_T_camera)
{
	ocean_assert(frame.isValid());

	if (Image::encodeImage(frame, imageType, buffer_))
	{
		ocean_assert(!buffer_.empty());
		imageType_ = imageType;

		if (camera != nullptr)
		{
			cameraModel_ = CameraCalibrationManager::get().serializeCamera(*camera);
		}

		ocean_assert(isValid());
	}
	else
	{
		buffer_.clear();

		ocean_assert(!isValid());
	}
}

MediaSerializer::DataSampleFrame::DataSampleFrame(Buffer&& buffer, const std::string& imageType, const DataTimestamp& dataTimestamp, SharedAnyCamera camera, const HomogenousMatrixD4& device_T_camera, const Timestamp& sampleCreationTimestamp) :
	DataSample(dataTimestamp, sampleCreationTimestamp),
	buffer_(std::move(buffer)),
	imageType_(imageType),
	device_T_camera_(device_T_camera)
{
	if (camera != nullptr)
	{
		cameraModel_ = CameraCalibrationManager::get().serializeCamera(*camera);
	}
}

SharedAnyCamera MediaSerializer::DataSampleFrame::camera() const
{
	if (cameraModel_.empty())
	{
		return nullptr;
	}

	return CameraCalibrationManager::get().parseCamera(std::string(), std::string(cameraModel_));
}

Frame MediaSerializer::DataSampleFrame::frame(SharedAnyCamera* camera) const
{
	ocean_assert(isValid());

	if (camera != nullptr)
	{
		*camera = this->camera();
	}

	Frame result = Image::decodeImage(buffer_.data(), buffer_.size(), imageType_);

	if (result)
	{
		if (dataTimestamp_.isValid())
		{
			if (dataTimestamp_.isDouble())
			{
				result.setTimestamp(Timestamp(dataTimestamp_.asDouble()));
			}
			else
			{
				result.setTimestamp(Timestamp(double(dataTimestamp_.asInt())));
			}
		}
	}

	return result;
}

bool MediaSerializer::DataSampleFrame::readSample(InputBitstream& inputBitstream)
{
	if (!DataSample::readSample(inputBitstream))
	{
		return false;
	}

	if (!inputBitstream.read<std::string>(imageType_))
	{
		return false;
	}
	uint32_t bufferSize = 0u;
	if (!inputBitstream.read<uint32_t>(bufferSize))
	{
		return false;
	}

	buffer_.resize(bufferSize);
	if (!inputBitstream.read(buffer_.data(), bufferSize))
	{
		return false;
	}

	if (!inputBitstream.read<std::string>(cameraModel_))
	{
		return false;
	}

	// if device_T_camera valid, then 16 doubles, otherwise one NumericD::minValue()

	double firstValue;
	if (!inputBitstream.read<double>(firstValue))
	{
		return false;
	}

	if (firstValue == NumericD::minValue())
	{
		device_T_camera_ = HomogenousMatrixD4(false);
	}
	else
	{
		if (!inputBitstream.read(device_T_camera_.data() + 1, sizeof(double) * 15))
		{
			return false;
		}

		device_T_camera_.data()[0] = firstValue;

		ocean_assert(device_T_camera_.isValid());
	}

	return true;
}

bool MediaSerializer::DataSampleFrame::writeSample(OutputBitstream& outputBitstream) const
{
	ocean_assert(isValid());

	if (!DataSample::writeSample(outputBitstream))
	{
		return false;
	}

	ocean_assert(!buffer_.empty());

	if (!outputBitstream.write<std::string>(imageType_))
	{
		return false;
	}

	if (!NumericT<uint32_t>::isInsideValueRange(buffer_.size()))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	if (!outputBitstream.write<uint32_t>(uint32_t(buffer_.size())))
	{
		return false;
	}

	if (!outputBitstream.write(buffer_.data(), buffer_.size()))
	{
		return false;
	}

	if (!outputBitstream.write<std::string>(cameraModel_))
	{
		return false;
	}

	// if device_T_camera valid, then 16 doubles, otherwise one NumericD::minValue()

	if (device_T_camera_.isValid())
	{
		if (!outputBitstream.write(device_T_camera_.data(), sizeof(double) * 16))
		{
			return false;
		}
	}
	else
	{
		if (!outputBitstream.write<double>(NumericD::minValue()))
		{
			return false;
		}
	}

	return true;
}

const std::string& MediaSerializer::DataSampleFrame::sampleType()
{
	static const std::string typeName = "ocean/media/datasampleframe";

	return typeName;
}

}

}

}
