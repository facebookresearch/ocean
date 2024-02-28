// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/io/vrs/Utilities.h"

#include <vrs/RecordFileReader.h>

namespace Ocean
{

namespace IO
{

namespace VRS
{

std::vector<std::string> Utilities::availableRecordablesInFile(const std::string& vrsFile)
{
	ocean_assert(!vrsFile.empty());

	std::vector<std::string> result;

	const std::shared_ptr<vrs::RecordFileReader> recordFileReader = std::make_shared<vrs::RecordFileReader>();

	if (recordFileReader->openFile(vrsFile) == 0)
	{
		const std::set<vrs::StreamId>& streamIds = recordFileReader->getStreams();

		for (const vrs::StreamId& recordable : streamIds)
		{
			result.push_back(recordable.getName());
		}
	}

	return result;
}

std::vector<std::string> Utilities::availableSlamCameraDataRecordablesInFile(const std::string& vrsFile)
{
	return availableRecordablesInFile(vrsFile, {vrs::RecordableTypeId::SlamCameraData});
}

std::vector<std::string> Utilities::availableSlamIMUDataRecordablesInFile(const std::string& vrsFile)
{
	return availableRecordablesInFile(vrsFile, {vrs::RecordableTypeId::SlamImuData});
}

std::vector<std::string> Utilities::availableRecordablesInFile(const std::string& vrsFile, const RecordableTypeIdSet& typeIds)
{
	ocean_assert(!vrsFile.empty());

	std::vector<std::string> result;

	const std::shared_ptr<vrs::RecordFileReader> recordFileReader = std::make_shared<vrs::RecordFileReader>();

	if (recordFileReader->openFile(vrsFile) == 0)
	{
		const std::set<vrs::StreamId>& streamIds = recordFileReader->getStreams();

		for (const vrs::StreamId& recordable : streamIds)
		{
			if (typeIds.find(recordable.getTypeId()) != typeIds.cend())
			{
				result.push_back(recordable.getName());
			}
		}
	}

	return result;
}


bool Utilities::toOceanPixelFormat(const vrs::PixelFormat& vrsPixelFormat, FrameType::PixelFormat& pixelFormat)
{
	switch (vrsPixelFormat)
	{
		case vrs::PixelFormat::GREY8:
			pixelFormat = FrameType::FORMAT_Y8;
			break;

		case vrs::PixelFormat::GREY10:
			pixelFormat = FrameType::FORMAT_Y10;
			break;

		case vrs::PixelFormat::BGR8:
			pixelFormat = FrameType::FORMAT_BGR24;
			break;

		case vrs::PixelFormat::DEPTH32F:
			pixelFormat = FrameType::genericPixelFormat<float, 1u>();
			break;

		case vrs::PixelFormat::RGB8:
			pixelFormat = FrameType::FORMAT_RGB24;
			break;

		case vrs::PixelFormat::RGBA8:
			pixelFormat = FrameType::FORMAT_RGBA32;
			break;

		case vrs::PixelFormat::RAW10:
			pixelFormat = FrameType::FORMAT_Y10_PACKED;
			break;

		case vrs::PixelFormat::RAW10_BAYER_RGGB:
			pixelFormat = FrameType::FORMAT_RGGB10_PACKED;
			break;

		case vrs::PixelFormat::YUV_I420_SPLIT:
			pixelFormat = FrameType::FORMAT_Y_U_V12;
			break;

		default:
			pixelFormat = FrameType::FORMAT_UNDEFINED;
			return false;
	}

	return true;
}

bool Utilities::toOceanFrame(const vrs::utils::PixelFrame& vrsPixelFrame, Frame& frame, const Frame::CopyMode copyMode)
{
	const unsigned int width = vrsPixelFrame.getWidth();
	const unsigned int height = vrsPixelFrame.getHeight();

	if (width == 0u || height == 0u)
	{
		ocean_assert(false && "Invalid image dimensions");
		return false;
	}

	FrameType::PixelFormat pixelFormat = FrameType::FORMAT_UNDEFINED;
	if (!Utilities::toOceanPixelFormat(vrsPixelFrame.getPixelFormat(), pixelFormat) || pixelFormat == FrameType::FORMAT_UNDEFINED)
	{
		ocean_assert(false && "Undefined pixel format");
		return false;
	}

	if (pixelFormat == FrameType::FORMAT_Y_U_V12)
	{
		ocean_assert(width % 2u == 0u && height % 2u == 0u);
		ocean_assert(width == vrsPixelFrame.getSpec().getWidth());
		ocean_assert(height == vrsPixelFrame.getSpec().getPlaneHeight(0u));

		const FrameType frameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT);

		bool isContiguous = true;
		unsigned int planesPaddingElements[3] = {0u, 0u, 0u};

		for (unsigned int planeIndex = 0u; planeIndex < 3u; ++planeIndex)
		{
			if (!Frame::strideBytes2paddingElements(pixelFormat, width, vrsPixelFrame.getSpec().getPlaneStride(planeIndex), planesPaddingElements[planeIndex], planeIndex))
			{
				ocean_assert(false && "Invalid padding elements");
				return false;
			}

			isContiguous = isContiguous && planesPaddingElements[planeIndex] == 0u;
		}

		if (isContiguous && copyMode != Frame::CM_USE_KEEP_LAYOUT)
		{
			frame = Frame(frameType);

			memcpy(frame.data<void>(0), vrsPixelFrame.rdata() + 0, frame.size(0u));
			memcpy(frame.data<void>(1), vrsPixelFrame.rdata() + frame.size(0u), frame.size(1u));
			memcpy(frame.data<void>(2), vrsPixelFrame.rdata() + frame.size(0u) + frame.size(1u), frame.size(2u));
		}
		else
		{
			Frame::PlaneInitializers<uint8_t> planeInitializers;
			planeInitializers.reserve(3);

			const uint8_t* planeData = vrsPixelFrame.rdata();

			for (unsigned int planeIndex = 0u; planeIndex < 3u; ++planeIndex)
			{
				planeInitializers.emplace_back(planeData, copyMode, planesPaddingElements[planeIndex]);

				// Forward the data pointer to the beginning of the next plane.
				ocean_assert((planeIndex == 0u && vrsPixelFrame.getSpec().getPlaneHeight(planeIndex) == height) || vrsPixelFrame.getSpec().getPlaneHeight(planeIndex) == (height / 2u));
				planeData += vrsPixelFrame.getSpec().getPlaneHeight(planeIndex) * vrsPixelFrame.getSpec().getPlaneStride(planeIndex);
			}

			frame = Frame(frameType, planeInitializers);
		}


		ocean_assert(frame.isValid());

		return true;
	}
	else
	{
		unsigned int paddingElements = 0u;
		if (!Frame::strideBytes2paddingElements(pixelFormat, vrsPixelFrame.getWidth(), vrsPixelFrame.getStride(), paddingElements, /* planeIndex */ 0u))
		{
			ocean_assert(false && "Invalid padding elements");
			return false;
		}

		frame = Frame(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), vrsPixelFrame.rdata(), copyMode, paddingElements);
		ocean_assert(frame.isValid());

		return true;
	}

	return false;
}


}

}

}
