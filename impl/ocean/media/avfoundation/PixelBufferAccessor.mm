/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/avfoundation/PixelBufferAccessor.h"

#include "ocean/cv/FrameConverter.h"

namespace Ocean
{

namespace Media
{

namespace AVFoundation
{

PixelBufferAccessor::PixelBufferAccessor(PixelBufferAccessor&& pixelBufferAccessor)
{
	*this = std::move(pixelBufferAccessor);
}

PixelBufferAccessor::PixelBufferAccessor(CVPixelBufferRef pixelBuffer, const bool readOnly) :
	PixelBufferAccessor(pixelBuffer, readOnly, false)
{
	// nothing to do here
}

PixelBufferAccessor::PixelBufferAccessor(CVPixelBufferRef pixelBuffer, const bool readOnly, const bool accessYPlaneOnly)
{
	if (pixelBuffer == nullptr)
	{
		return;
	}

	const OSType osPixelFormatType = CVPixelBufferGetPixelFormatType(pixelBuffer);
	const FrameType::PixelFormat pixelFormat = PixelBufferAccessor::translatePixelFormat(osPixelFormatType);

	if (pixelFormat == FrameType::FORMAT_UNDEFINED)
	{
		ocean_assert(false && "Unknown format!");
		return;
	}

	const unsigned int width = (unsigned int)CVPixelBufferGetWidth(pixelBuffer);
	const unsigned int height = (unsigned int)CVPixelBufferGetHeight(pixelBuffer);

	ocean_assert(width > 0u && height > 0u);
	if (width == 0u || height == 0u)
	{
		return;
	}

	FrameType frameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT);

	const size_t bufferSize = CVPixelBufferGetDataSize(pixelBuffer);

	ocean_assert(bufferSize >= frameType.frameTypeSize());
	if (bufferSize < frameType.frameTypeSize())
	{
		return;
	}

	const CVPixelBufferLockFlags lockFlags = readOnly ? kCVPixelBufferLock_ReadOnly : 0;

	// it's necessary to lock the buffer before getting the base address
	CVReturn status = CVPixelBufferLockBaseAddress(pixelBuffer, lockFlags);

	ocean_assert(status == kCVReturnSuccess);
	if (status != kCVReturnSuccess)
	{
		return;
	}

	pixelBuffer_ = pixelBuffer;
	lockFlags_ = lockFlags;

	ocean_assert(frame_.isValid() == false);

	if (pixelFormat == FrameType::FORMAT_Y_UV12 || pixelFormat == FrameType::FORMAT_Y_VU12)
	{
		const size_t planeCount = CVPixelBufferGetPlaneCount(pixelBuffer_);

		if (planeCount != 2)
		{
			ocean_assert(false && "Invalid plane layout!");
			return;
		}

		void* yPlaneData = CVPixelBufferGetBaseAddressOfPlane(pixelBuffer_, 0);
		void* uvPlaneData = CVPixelBufferGetBaseAddressOfPlane(pixelBuffer_, 1);

		const unsigned int yPlaneStrideBytes = (unsigned int)CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer_, 0);
		const unsigned int uvPlaneStrideBytes = (unsigned int)CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer_, 1);

		ocean_assert(yPlaneStrideBytes >= width);
		ocean_assert(uvPlaneStrideBytes >= width);

		const unsigned int yPlanePaddingElements = yPlaneStrideBytes - width;
		const unsigned int uvPlanePaddingElements = uvPlaneStrideBytes - width;

		Frame::PlaneInitializers<uint8_t> planeInitializers;
		planeInitializers.reserve(2);

		if (accessYPlaneOnly)
		{
			if (readOnly)
			{
				planeInitializers.emplace_back((const uint8_t*)yPlaneData, Frame::CM_USE_KEEP_LAYOUT, yPlanePaddingElements);
			}
			else
			{
				planeInitializers.emplace_back((uint8_t*)yPlaneData, Frame::CM_USE_KEEP_LAYOUT, yPlanePaddingElements);
			}

			frameType = FrameType(frameType, FrameType::FORMAT_Y8);
		}
		else
		{
			if (readOnly)
			{
				planeInitializers.emplace_back((const uint8_t*)yPlaneData, Frame::CM_USE_KEEP_LAYOUT, yPlanePaddingElements);
				planeInitializers.emplace_back((const uint8_t*)uvPlaneData, Frame::CM_USE_KEEP_LAYOUT, uvPlanePaddingElements);
			}
			else
			{
				planeInitializers.emplace_back((uint8_t*)yPlaneData, Frame::CM_USE_KEEP_LAYOUT, yPlanePaddingElements);
				planeInitializers.emplace_back((uint8_t*)uvPlaneData, Frame::CM_USE_KEEP_LAYOUT, uvPlanePaddingElements);
			}
		}

		frame_ = Frame(frameType, planeInitializers);
	}
	else if (pixelFormat == FrameType::FORMAT_Y_U_V12_LIMITED_RANGE || pixelFormat == FrameType::FORMAT_Y_U_V12_FULL_RANGE)
	{
		const size_t planeCount = CVPixelBufferGetPlaneCount(pixelBuffer_);

		if (planeCount != 3)
		{
			ocean_assert(false && "Invalid plane layout!");
			return;
		}

		void* yPlaneData = CVPixelBufferGetBaseAddressOfPlane(pixelBuffer_, 0);
		void* uPlaneData = CVPixelBufferGetBaseAddressOfPlane(pixelBuffer_, 1);
		void* vPlaneData = CVPixelBufferGetBaseAddressOfPlane(pixelBuffer_, 2);

		const unsigned int yPlaneStrideBytes = (unsigned int)CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer_, 0);
		const unsigned int uPlaneStrideBytes = (unsigned int)CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer_, 1);
		const unsigned int vPlaneStrideBytes = (unsigned int)CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer_, 2);

		ocean_assert(yPlaneStrideBytes >= width);
		ocean_assert(uPlaneStrideBytes >= width / 2u);
		ocean_assert(vPlaneStrideBytes >= width / 2u);

		const unsigned int yPlanePaddingElements = yPlaneStrideBytes - width;
		const unsigned int uPlanePaddingElements = uPlaneStrideBytes - width / 2u;
		const unsigned int vPlanePaddingElements = vPlaneStrideBytes - width / 2u;

		Frame::PlaneInitializers<uint8_t> planeInitializers;
		planeInitializers.reserve(3);

		if (accessYPlaneOnly)
		{
			if (readOnly)
			{
				planeInitializers.emplace_back((const uint8_t*)yPlaneData, Frame::CM_USE_KEEP_LAYOUT, yPlanePaddingElements);
			}
			else
			{
				planeInitializers.emplace_back((uint8_t*)yPlaneData, Frame::CM_USE_KEEP_LAYOUT, yPlanePaddingElements);
			}

			frameType = FrameType(frameType, FrameType::FORMAT_Y8);
		}
		else
		{
			if (readOnly)
			{
				planeInitializers.emplace_back((const uint8_t*)yPlaneData, Frame::CM_USE_KEEP_LAYOUT, yPlanePaddingElements);
				planeInitializers.emplace_back((const uint8_t*)uPlaneData, Frame::CM_USE_KEEP_LAYOUT, uPlanePaddingElements);
				planeInitializers.emplace_back((const uint8_t*)vPlaneData, Frame::CM_USE_KEEP_LAYOUT, vPlanePaddingElements);
			}
			else
			{
				planeInitializers.emplace_back((uint8_t*)yPlaneData, Frame::CM_USE_KEEP_LAYOUT, yPlanePaddingElements);
				planeInitializers.emplace_back((uint8_t*)uPlaneData, Frame::CM_USE_KEEP_LAYOUT, uPlanePaddingElements);
				planeInitializers.emplace_back((uint8_t*)vPlaneData, Frame::CM_USE_KEEP_LAYOUT, vPlanePaddingElements);
			}
		}

		frame_ = Frame(frameType, planeInitializers);
	}
	else
	{
		// We can directly map the frame data without needing to copy.
		ocean_assert(frameType.numberPlanes() == 1u);

		// actual stride of the provided memory, in bytes
		const unsigned int planeStrideBytes = (unsigned int)(CVPixelBufferGetBytesPerRow(pixelBuffer_));

		void* planeData = CVPixelBufferGetBaseAddress(pixelBuffer_);

		unsigned int planePaddingElements = 0u;
		if (Frame::strideBytes2paddingElements(frameType.pixelFormat(), frameType.width(), planeStrideBytes, planePaddingElements))
		{
			Frame::PlaneInitializers<void> planeInitializers;
			planeInitializers.reserve(1);

			if (readOnly)
			{
				planeInitializers.emplace_back((const void*)planeData, Frame::CM_USE_KEEP_LAYOUT, planePaddingElements);
			}
			else
			{
				planeInitializers.emplace_back(planeData, Frame::CM_USE_KEEP_LAYOUT, planePaddingElements);
			}

			frame_ = Frame(frameType, planeInitializers);
		}
		else
		{
			ocean_assert(false && "Invalid stride!");
			return;
		}
	}

	ocean_assert(frame_.isOwner() == false);

#ifdef OCEAN_DEBUG

	if (frame_)
	{
		debugFrameData_ = frame_.constdata<void>();
	}

#endif
}

PixelBufferAccessor::~PixelBufferAccessor()
{
	release();
}

void PixelBufferAccessor::release()
{
	if (pixelBuffer_)
	{
#ifdef OCEAN_DEBUG
		ocean_assert(!frame_.isValid() || debugFrameData_ == frame_.constdata<void>());
#endif
		CVPixelBufferUnlockBaseAddress(pixelBuffer_, lockFlags_);

		pixelBuffer_ = nullptr;
		lockFlags_ = 0;

		frame_.release();

#ifdef OCEAN_DEBUG
		debugFrameData_ = nullptr;
#endif
	}
}

PixelBufferAccessor& PixelBufferAccessor::operator=(PixelBufferAccessor&& pixelBufferAccessor)
{
	if (this != &pixelBufferAccessor)
	{
		release();

		pixelBuffer_ = pixelBufferAccessor.pixelBuffer_;
		pixelBufferAccessor.pixelBuffer_ = nullptr;

		lockFlags_ = pixelBufferAccessor.lockFlags_;
		pixelBufferAccessor.lockFlags_ = 0;

		frame_ = std::move(pixelBufferAccessor.frame_);

#ifdef OCEAN_DEBUG
		debugFrameData_ = pixelBufferAccessor.debugFrameData_;
		pixelBufferAccessor.debugFrameData_ = nullptr;
#endif
	}

	return *this;
}

FrameType::PixelFormat PixelBufferAccessor::translatePixelFormat(const OSType pixelFormat)
{
	switch (pixelFormat)
	{
		case kCVPixelFormatType_24RGB:
			return FrameType::FORMAT_RGB24;

		case kCVPixelFormatType_24BGR:
			return FrameType::FORMAT_BGR24;

		case kCVPixelFormatType_32ARGB:
			return FrameType::FORMAT_ARGB32;

		case kCVPixelFormatType_32BGRA:
			return FrameType::FORMAT_BGRA32;

		case kCVPixelFormatType_32ABGR:
			return FrameType::FORMAT_ABGR32;

		case kCVPixelFormatType_32RGBA:
			return FrameType::FORMAT_RGBA32;

		case kCVPixelFormatType_OneComponent8:
			return FrameType::FORMAT_Y8;

		case kCVPixelFormatType_16Gray:
			return FrameType::FORMAT_Y16;

		case kCVPixelFormatType_420YpCbCr8Planar:
			return FrameType::FORMAT_Y_U_V12_LIMITED_RANGE;

		case kCVPixelFormatType_420YpCbCr8PlanarFullRange:
			return FrameType::FORMAT_Y_U_V12_FULL_RANGE;

		case kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange:
		case kCVPixelFormatType_420YpCbCr8BiPlanarFullRange:
			return FrameType::FORMAT_Y_UV12;

		case kCVPixelFormatType_DepthFloat32:
			return FrameType::FORMAT_F32;

		default:
			break;
	}

	return FrameType::FORMAT_UNDEFINED;
}

OSType PixelBufferAccessor::translatePixelFormat(const FrameType::PixelFormat pixelFormat)
{
	switch (pixelFormat)
	{
		case FrameType::FORMAT_RGB24:
			return kCVPixelFormatType_24RGB; // identical to k24RGBPixelFormat

		case FrameType::FORMAT_BGR24:
			return kCVPixelFormatType_24BGR;

		case FrameType::FORMAT_ARGB32:
			return kCVPixelFormatType_32ARGB; // identical to k32ARGBPixelFormat (Core Video, macOS)

		case FrameType::FORMAT_BGRA32:
			return kCVPixelFormatType_32BGRA;

		case FrameType::FORMAT_ABGR32:
			return kCVPixelFormatType_32ABGR;

		case FrameType::FORMAT_RGBA32:
			return kCVPixelFormatType_32RGBA;

		case FrameType::FORMAT_Y8:
			return kCVPixelFormatType_OneComponent8;

		case FrameType::FORMAT_Y16:
			return kCVPixelFormatType_16Gray;

		case FrameType::FORMAT_Y_U_V12_LIMITED_RANGE:
			return kCVPixelFormatType_420YpCbCr8Planar;

		case FrameType::FORMAT_Y_U_V12_FULL_RANGE:
			return kCVPixelFormatType_420YpCbCr8PlanarFullRange;

		case FrameType::FORMAT_Y_UV12:
			return kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange;

		case FrameType::FORMAT_F32:
			return kCVPixelFormatType_DepthFloat32;

		default:
			break;
	}

	ocean_assert(false && "Invalid pixel format!");
	return 0;
}

}

}

}
