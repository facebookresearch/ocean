// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/unifiedfeatures/vlfeatsift/Utilities.h"

#include "ocean/cv/FrameNormalizer.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace VlfeatSift
{

const Frame& Utilities::frameAsContiguousF32(const Frame& frame, std::unique_ptr<Frame>& float32FramePtr, Worker* worker)
{
	if (frame.pixelFormat() == FrameType::FORMAT_F32)
	{
		if (frame.isContinuous())
		{
			return frame;
		}

		float32FramePtr = std::make_unique<Frame>(frame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
	}
	else
	{
		ocean_assert(frame.pixelFormat() == FrameType::FORMAT_Y8);

		float32FramePtr = std::make_unique<Frame>(FrameType(frame.width(), frame.height(), FrameType::FORMAT_F32, frame.pixelOrigin()));

		constexpr float kBias = 0.0f;
		constexpr float kScale = 1.0f / 255.0f;
		CV::FrameNormalizer::normalizeToFloat<uint8_t, float, 1u>(frame.constdata<uint8_t>(), float32FramePtr->data<float>(), frame.width(), frame.height(), &kBias, &kScale, frame.paddingElements(), float32FramePtr->paddingElements(), worker);
	}

	ocean_assert(float32FramePtr);

	return *float32FramePtr;
};

} // namespace VlfeatSift

} // namespace UnifiedFeatures

} // namespace Ocean
