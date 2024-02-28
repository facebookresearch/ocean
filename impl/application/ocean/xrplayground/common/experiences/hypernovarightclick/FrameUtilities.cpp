// (c) Meta Platform, Inc. and its affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/hypernovarightclick/FrameUtilities.h"

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	#include <libyuv/convert_from_argb.h>

namespace Ocean::XRPlayground
{

folly::Optional<Ocean::Frame> FrameUtilities::convertToBGRA(const Ocean::Frame& frame)
{
	Ocean::Frame bgrFrame;
	if (!Ocean::CV::FrameConverter::Comfort::convert(frame, Ocean::FrameType(frame, Ocean::FrameType::FORMAT_BGR24), bgrFrame))
	{
		Ocean::Log::info() << "bgr24 conversion failed";
		return folly::none;
	}

	const Ocean::FrameType bgra32FrameType(bgrFrame.width(), bgrFrame.height(), Ocean::FrameType::FORMAT_BGRA32, Ocean::FrameType::ORIGIN_UPPER_LEFT);
	Ocean::Frame bgra32Frame(bgra32FrameType);
	Ocean::CV::FrameConverterBGR24::convertBGR24ToBGRA32(bgrFrame.constdata<uint8_t>(), bgra32Frame.data<uint8_t>(), bgrFrame.width(), bgrFrame.height(), Ocean::CV::FrameChannels::CONVERT_NORMAL, bgrFrame.paddingElements(), bgra32Frame.paddingElements());
	return bgra32Frame;
}

folly::Optional<Ocean::Frame> FrameUtilities::convertFromBGRAToNV12(
	Ocean::Frame bgraFrame,
	std::shared_ptr<facebook::bufferpool::BufferPool> bufferPool)
{
	auto width = bgraFrame.width();
	auto height = bgraFrame.height();

	ComponentData y(bufferPool, width, width * height);
	ComponentData nv12(bufferPool, width, width * height / 2);
	// converts from ARGB to NV12. For an explanation on NV12, see the Y_UV12 format in
	// ocean/base/Frame.h

	// ARGB little endian (bgra in memory) to NV12.
	libyuv::ARGBToNV12(
		bgraFrame.constdata<uint8_t>(),
		width * 4,
		y.getBufferPtr(),
		width,
		nv12.getBufferPtr(),
		width,
		width,
		height);

	auto const yStride = y.bufferStride;
	auto const nv12Stride = nv12.bufferStride;
	const unsigned int yPlanePaddingElements = yStride - width;
	const unsigned int nv12PlanePaddingElements = nv12Stride - width;

	const uint8_t* baseAddressYPlane = y.getBufferPtr();
	const uint8_t* baseAddressNV12Plane = nv12.getBufferPtr();

	Frame::PlaneInitializers<uint8_t> planeInitializers;
	planeInitializers.emplace_back(
		(const uint8_t*)baseAddressYPlane, Frame::CM_USE_KEEP_LAYOUT, yPlanePaddingElements);
	planeInitializers.emplace_back(
		(const uint8_t*)baseAddressNV12Plane, Frame::CM_USE_KEEP_LAYOUT, nv12PlanePaddingElements);
	const FrameType frameType(width,
		height,
		FrameType::FORMAT_Y_UV12,
		FrameType::ORIGIN_UPPER_LEFT);
	auto frame = Frame(frameType, planeInitializers);
	return frame;
}


} // namespace Ocean::XRPlayground

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
