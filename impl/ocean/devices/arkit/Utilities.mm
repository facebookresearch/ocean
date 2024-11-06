/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/arkit/Utilities.h"

#include <ocean/media/avfoundation/PixelBufferAccessor.h>

namespace Ocean
{

namespace Devices
{

namespace ARKit
{

Frame Utilities::extractFrame(const ARFrame* arFrame, const bool copyData, const FrameType::PixelFormat preferredPixelFormat)
{
	ocean_assert(arFrame != nullptr);

	const CVPixelBufferRef capturedImage = arFrame.capturedImage;

	const bool accessYPlaneOnly = preferredPixelFormat == FrameType::FORMAT_Y8;

	const Media::AVFoundation::PixelBufferAccessor pixelBufferAccessor(capturedImage, true /*readOnly*/, accessYPlaneOnly);

	if (!pixelBufferAccessor)
	{
		ocean_assert(false && "This should never happen");
		return Frame();
	}

	const Frame::AdvancedCopyMode advancedCopyMode = copyData ? Frame::ACM_COPY_REMOVE_PADDING_LAYOUT : Frame::ACM_USE_KEEP_LAYOUT;

	return Frame(pixelBufferAccessor.frame(), advancedCopyMode);
}

Frame Utilities::extractDepthFrame(const ARFrame* arFrame, Frame* confidenceFrame, const bool copyData)
{
	ocean_assert(arFrame != nullptr);

	if (@available(iOS 14.0, *))
	{
		if (arFrame.sceneDepth != nullptr)
		{
			const Media::AVFoundation::PixelBufferAccessor depthPixelBufferAccessor(arFrame.sceneDepth.depthMap, true /*readonly*/);

			if (!depthPixelBufferAccessor)
			{
				ocean_assert(false && "This should never happen");
				return Frame();
			}

			const Frame::AdvancedCopyMode advancedCopyMode = copyData ? Frame::ACM_COPY_REMOVE_PADDING_LAYOUT : Frame::ACM_USE_KEEP_LAYOUT;

			if (confidenceFrame != nullptr)
			{
				if (arFrame.sceneDepth.confidenceMap != nullptr)
				{
					const Media::AVFoundation::PixelBufferAccessor confidencePixelBufferAccessor(arFrame.sceneDepth.confidenceMap, true /*readonly*/);

					if (confidencePixelBufferAccessor)
					{
						*confidenceFrame = Frame(confidencePixelBufferAccessor.frame(), advancedCopyMode);
					}
				}
				else
				{
					confidenceFrame->release();
				}
			}

			return Frame(depthPixelBufferAccessor.frame(), advancedCopyMode);
		}
	}

	return Frame();
}

}

}

}
