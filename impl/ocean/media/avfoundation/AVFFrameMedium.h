/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_AVF_FRAME_MEDIUM_H
#define META_OCEAN_MEDIA_AVF_FRAME_MEDIUM_H

#include "ocean/media/avfoundation/AVFoundation.h"
#include "ocean/media/avfoundation/AVFMedium.h"

#include "ocean/math/AnyCamera.h"

#include "ocean/media/FrameMedium.h"

namespace Ocean
{

namespace Media
{

namespace AVFoundation
{

/**
 * This is the base class for all AVFoundation frame mediums.
 * @ingroup mediaavf
 */
class AVFFrameMedium :
	virtual public AVFMedium,
	virtual public FrameMedium
{
	protected:

		/**
		 * Creates a new frame medium by a given url.
		 * @param url Url of the frame medium
		 */
		explicit AVFFrameMedium(const std::string& url);

		/**
		 * Destructs a AVFFrameMedium object
		 */
		~AVFFrameMedium() override;

		/**
		 * Called, if a new sample arrived.
		 * @param pixelBuffer A pointer to a buffer that contains the pixel data of the sample
		 * @param anyCamera The camera profile if known; nullptr otherwise
		 * @param unixTimestamp Used to set timestamp of the Frame associated to the sample (measured in seconds since 1970-01-01 00:00 UTC)
		 * @param sampleTime The presentation time of the sample in seconds, used to set the relative timestamp of the frame associated to the sample
		 */
		virtual void onNewSample(CVPixelBufferRef pixelBuffer, SharedAnyCamera anyCamera, const double unixTimestamp, const double sampleTime);

	protected:

		/// The frame type of the most recent.
		FrameType recentFrameType_;
};

}

}

}

#endif // META_OCEAN_MEDIA_AVF_FRAME_MEDIUM_H
