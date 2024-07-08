/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/avfoundation/AVFFrameMedium.h"
#include "ocean/media/avfoundation/AVFLibrary.h"
#include "ocean/media/avfoundation/PixelBufferAccessor.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/system/Performance.h"

namespace Ocean
{

namespace Media
{

namespace AVFoundation
{

AVFFrameMedium::AVFFrameMedium(const std::string& url) :
	Medium(url),
	AVFMedium(url),
	FrameMedium(url)
{
	const size_t capacity = System::Performance::get().performanceLevel() > System::Performance::LEVEL_MEDIUM ? 20 : 10;

	frameCollection_ = FrameCollection(capacity);
}

AVFFrameMedium::~AVFFrameMedium()
{
	// nothing to do here
}

void AVFFrameMedium::onNewSample(CVPixelBufferRef pixelBuffer, SharedAnyCamera anyCamera, const double unixTimestamp, const double sampleTime)
{
	ocean_assert(pixelBuffer);

	const PixelBufferAccessor pixelBufferAccessor(pixelBuffer, true);

	if (!pixelBufferAccessor)
	{
		return;
	}

	Frame frame = Frame(pixelBufferAccessor.frame(), Frame::ACM_USE_KEEP_LAYOUT);
	ocean_assert(frame);

	if (recentFrameType_ != frame.frameType())
	{
		recentFrameType_ = frame.frameType();

		Log::debug() << "The frame type of the medium has changed now we receive a " << recentFrameType_.width() << "x" << recentFrameType_.height() << ", " << FrameType::translatePixelFormat(recentFrameType_.pixelFormat()) << " frame";
	}

	frame.setTimestamp(Timestamp(unixTimestamp));
	frame.setRelativeTimestamp(Timestamp(sampleTime));

	deliverNewFrame(std::move(frame), std::move(anyCamera));
}

}

}

}
