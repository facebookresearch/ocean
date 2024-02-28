// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include <memory>

#include "ocean/unifiedfeatures/vlfeatsift/VlfeatSift.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace VlfeatSift
{

/**
 * This class is a collection of utility functions for the VLFeat SIFT interface.
 * @ingroup unifiedfeaturesvlfeatsift
 */
class OCEAN_UNIFIEDFEATURES_VLFEAT_SIFT_EXPORT Utilities
{
	public:

		/**
		 * Given a frame that might be Y8 or F32, and which may or may not have row padding, convert to a zero-padding F32 frame if necessary and return a reference to the frame.
		 * @param frame Input frame
		 * @param float32FramePtr Optional frame object that will be populated iff `frame` is not already a zero-padding F32 frame
		 * @param worker Optional worker with which to perform the operation
		 * @return Reference to `frame` if it is a zero-padding F32 frame, otherwise a reference to the newly created frame stored in float32FramePtr
		 */
		static const Frame& frameAsContiguousF32(const Frame& frame, std::unique_ptr<Frame>& float32FramePtr, Worker* worker = nullptr);
};

} // namespace VlfeatSift

} // namespace UnifiedFeatures

} // namespace Ocean
